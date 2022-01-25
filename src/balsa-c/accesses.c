/*
	The Balsa Asynchronous Hardware Synthesis System
	Copyright (C) 1995-2009 School of Computer Science
	The University of Manchester, Oxford Road, Manchester, UK, M13 9PL
	
	This program is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 2 of the License, or
	(at your option) any later version.
	
	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.
	
	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA

	`accesses.c'
	Type for handling unresolved accesses on channels/variables/shared code blocks
	
 */

#include "accesses.h"
#include "procedures.h"
#include "Errors.h"
#include "exprs.h"
#include "flags.h"

/* NewAccess : create an access on instance inst */
PtrAccess NewAccess (PtrInstance inst)
{
    PtrAccess access = NEW (Access);

    access->inst = inst;
    return access;
}

/* CopyAccess : shallow copy an access */
PtrAccess CopyAccess (PtrAccess access)
{
    PtrAccess ret = NEW (Access);

    *ret = *access;
    return ret;
}

DEFINE_CONS_LIST_TYPE (Access)
/* NewChannelAccess : create an access element for channel access */
PtrAccess NewChannelAccess (PtrInstance inst, ChannelAccessNature nature, PtrWire wire)
{
    PtrAccess access = NewAccess (inst);

    access->info.channel.wire = wire;
    access->info.channel.nature = nature;
    access->info.channel.functioningChannel = false;
    access->info.channel.unplaced = false;
    access->info.channel.nonMulticast = false;

    return access;
}

/* NewVariableAccess : create an access list element for variable access, this is for
   forming leaf not access lists. NB. this creates an access which accesses the range
   (width, offset) specified by the given wire (NB. if the wire is push then this is a write,
   otherwise a read) */
PtrAccess NewVariableAccess (PtrInstance inst, PtrWire wire)
{
    PtrAccess access = NewAccess (inst);

    access->info.variable.wires = NewWireList (wire, NULL);

    /* Create new MP_INT's for the disjunctions */
    if (wire->isPull)
    {                           /* Variable read */
        access->info.variable.readMap = MakeMaskForRange (wire->width, wire->offset);
        access->info.variable.writeMap = NewMP_INT (0);
    } else
    {                           /* Variable write */
        access->info.variable.readMap = NewMP_INT (0);
        access->info.variable.writeMap = MakeMaskForRange (wire->width, wire->offset);
    }
    return access;
}

/* NewSharedCallAccess : create a request to call procedure identified by the sharedActivationRecord */
PtrAccess NewSharedCallAccess (PtrProcedure procedure, PtrWire activationPort)
{
    PtrAccess access = NewAccess (procedure->dummyInstance);

    access->info.shared.unplaced = false;
    access->info.shared.procedure = procedure;
    access->info.shared.wire = activationPort;
    procedure->unplacedActivation = activationPort;
    return access;
}

/* NewValAccess : create an access to a val declaration */
extern PtrAccess NewValAccess (PtrInstance valInstance, PtrWire activationPort)
{
    PtrAccess access = NewAccess (valInstance);

    access->info.val.wire = activationPort;
    return access;
}

/* CopyAndUnplaceAccessList : copy the access list provided but make all the entries unplaced
   (ie. no wires in variable access (only maps), channel accesses bear unplaced == true,
   nonMulticast == true */
PtrAccessList CopyAndUnplaceAccessList (PtrAccessList accesses)
{
    PtrAccess newAccess;

    if (!accesses)
        return NULL;

    newAccess = CopyAccess (CAR (accesses));
    switch (CAR (accesses)->inst->nature)
    {
    case VariableInstance:     /* VariableAccess */
        newAccess->info.variable.wires = NULL;
        break;
    case SharedCallInstance:
        newAccess->info.shared.unplaced = true;
        break;
    case ValInstance:
        /* FIXME, make unplaced */
        break;
    default:                   /* Must be some channel flavour */
        newAccess->info.channel.unplaced = true;
        newAccess->info.channel.nonMulticast = true;
        break;
    }
    return NewAccessList (newAccess, CopyAndUnplaceAccessList (CDR (accesses)));
}

/* ParallelCombineActiveReadActiveReadChannels : aR x aR parallel combine */
PtrAccess
ParallelCombineActiveReadActiveReadChannels (PtrAccess left, PtrAccess right, PtrComponentList * comps, PtrWireList * wires, tPosition position)
{
    PtrAccess newAccess = NULL;

    if (left->info.channel.nonMulticast || right->info.channel.nonMulticast)
        LOG_ERROR (CannotPerformMulticastOnChannelUsedInSharedBlock, left->inst->ident, position);
    else
    {
        /* Synch: Read * Read to single Read, also check for sync channel completeness */
        /* Outgoing wire - pull read */
        PtrWire newWire = CopyAndRenumberWire (left->info.channel.wire,
          NextBundleNumber (*wires), position);

        newAccess = NewChannelAccess (left->inst, ChannelActiveRead, newWire);

        /* Check for sync first, if LHS is sync then both sides must be and 
           so this channel is 'functioning' */
        if (left->inst->type->nature == SyncType)
        {
            newAccess->info.channel.functioningChannel = true;
            /* 0 width Synch */
            *comps = NewComponentList (NewSynchComponent (position, left->info.channel.wire, right->info.channel.wire, newWire), *comps);
        } else
            *comps = NewComponentList (NewSynchPullComponent (position, left->info.channel.wire, right->info.channel.wire, newWire), *comps);
        *wires = NewWireList (newWire, *wires);

        if ((!InstanceCanMulticast (right->inst) &&
            (right->info.channel.wire->width != 0
              || right->info.channel.functioningChannel))
          || (!InstanceCanMulticast (left->inst) && (left->info.channel.wire->width != 0 || left->info.channel.functioningChannel)))
        {
            LOG_ERROR (MulticastWarning, left->inst->ident, position);
        }
    }
    return newAccess;
}

/* ParallelCombineActiveReadActiveWriteChannels : aR x aW parallel combine */
PtrAccess
ParallelCombineActiveReadActiveWriteChannels (PtrAccess left, PtrAccess right, PtrComponentList * comps, PtrWireList * wires, tPosition position)
{
    PtrAccess newAccess = NULL;

    /* This can only happen on an internal channel so disallow */
    if (left->info.channel.unplaced || right->info.channel.unplaced)
        LOG_ERROR (CannotUseLocalChannelInSharedBlock, left->inst->ident, position);
    else
    {
        /* Synch: Read * Write to single WRITE CHAN DONE */
        PtrWire newWire = CopyAndRenumberWire (right->info.channel.wire,
          NextBundleNumber (*wires), position);

        newAccess = NewChannelAccess (left->inst, ChannelActiveWrite, newWire);

        /* This channel is potentially complete */
        newAccess->info.channel.functioningChannel = true;

        /* Forming a multicast? */
        if (!InstanceCanMulticast (right->inst) && right->info.channel.functioningChannel)
            LOG_ERROR (MulticastWarning, right->inst->ident, position);
        /* Multicast ability */
        newAccess->info.channel.nonMulticast = left->info.channel.nonMulticast || right->info.channel.nonMulticast;
        *comps =
          NewComponentList ((right->info.channel.wire->width ==
            0 ? NewSynchComponent : NewSynchPushComponent) (position, right->info.channel.wire, left->info.channel.wire, newWire), *comps);
        *wires = NewWireList (newWire, *wires);
    }
    return newAccess;
}

/* ParallelCombinePassiveReadActiveWriteChannels : pR x aW parallel combine */
PtrAccess
ParallelCombinePassiveReadActiveWriteChannels (PtrAccess left, PtrAccess right, PtrComponentList * comps, PtrWireList * wires, tPosition position)
{
    PtrAccess newAccess = NULL;

    /* This can only happen on an internal channel so disallow */
    if (left->info.channel.unplaced || right->info.channel.unplaced)
        LOG_ERROR (CannotUseLocalChannelInSharedBlock, left->inst->ident, position);
    else
    {
        /* Synch: PassiveRead * Write to single WRITE CHAN DONE */
        PtrWire newWire = CopyAndRenumberWire (right->info.channel.wire,
          NextBundleNumber (*wires), position);

        newAccess = NewChannelAccess (left->inst, ChannelActiveWrite, newWire);

        /* This channel is potentially complete */
        newAccess->info.channel.functioningChannel = true;

        /* Forming a multicast? */
        if (!InstanceCanMulticast (right->inst) && right->info.channel.functioningChannel)
            LOG_ERROR (MulticastWarning, right->inst->ident, position);
        /* Multicast ability */
        newAccess->info.channel.nonMulticast = left->info.channel.nonMulticast || right->info.channel.nonMulticast;
        *comps =
          NewComponentList ((right->info.channel.wire->width ==
            0 ? NewForkComponent : NewForkPushComponent) (position, right->info.channel.wire, left->info.channel.wire, newWire), *comps);

        *wires = NewWireList (newWire, *wires);
    }
    return newAccess;
}

/* ParallelCombinePassiveReadPassiveReadChannels : pR x pR parallel combine */
PtrAccess
ParallelCombinePassiveReadPassiveReadChannels (PtrAccess left, PtrAccess right, PtrComponentList * comps, PtrWireList * wires, tPosition position)
{
    PtrAccess newAccess = NULL;

    /* Double unplaced */
    if (left->info.channel.unplaced && right->info.channel.unplaced)
        newAccess = CopyAccess (left);
    else if (left->info.channel.unplaced)
    {
        newAccess = CopyAccess (right);
        newAccess->info.channel.nonMulticast = true;
    } else if (right->info.channel.unplaced)
    {
        newAccess = CopyAccess (left);
        newAccess->info.channel.nonMulticast = true;
    } else
    {
        /* Synch: PassiveRead * PassiveRead to PassiveRead */
        PtrWire newWire = CopyAndRenumberWire (right->info.channel.wire,
          NextBundleNumber (*wires), position);

        newAccess = NewChannelAccess (left->inst, ChannelPassiveRead, newWire);
        /* Multicast ability */
        newAccess->info.channel.nonMulticast = left->info.channel.nonMulticast || right->info.channel.nonMulticast;

        *comps = NewComponentList ((right->info.channel.wire->width == 0 ?
            NewForkComponent : NewForkPushComponent) (position, newWire, right->info.channel.wire, left->info.channel.wire), *comps);
        *wires = NewWireList (newWire, *wires);

        if (!InstanceCanMulticast (left->inst) || !InstanceCanMulticast (right->inst))
            LOG_ERROR (MulticastWarning, left->inst->ident, position);
    }
    return newAccess;
}

/* ParallelCombinePassiveReadActiveReadChannels : pR x aR parallel combine */
PtrAccess
ParallelCombinePassiveReadActiveReadChannels (PtrAccess left, PtrAccess right, PtrComponentList * comps, PtrWireList * wires, tPosition position)
{
    /* Synch: PassiveSync * ActiveSync to ActiveSync */
    PtrWire newWire;
    PtrAccess newAccess = NULL;

    /* Only allow sync */
    if (right->info.channel.wire->width != 0)
        LOG_ERROR (CannotMixReadNaturesForNonSync, right->inst->ident, position);
    /* Test for unplacable connections */
    else if (left->info.channel.unplaced && right->info.channel.unplaced)
        newAccess = CopyAccess (left);
    else if (left->info.channel.unplaced)
    {
        newAccess = CopyAccess (right);
        newAccess->info.channel.nonMulticast = true;
    } else if (right->info.channel.unplaced)
    {
        newAccess = CopyAccess (left);
        newAccess->info.channel.nonMulticast = true;
    } else
    {
        newWire = CopyAndRenumberWire (right->info.channel.wire, NextBundleNumber (*wires), position);
        newAccess = NewChannelAccess (left->inst, ChannelActiveRead, newWire);
        /* Sync channel becomes connected */
        newAccess->info.channel.functioningChannel = true;

        *comps = NewComponentList (NewForkComponent (position, right->info.channel.wire, newWire, left->info.channel.wire), *comps);
        *wires = NewWireList (newWire, *wires);

        if ((!InstanceCanMulticast (right->inst) &&
            (right->info.channel.wire->width != 0
              || right->info.channel.functioningChannel))
          || (!InstanceCanMulticast (left->inst) && (left->info.channel.wire->width != 0 || left->info.channel.functioningChannel)))
        {
            LOG_ERROR (MulticastWarning, left->inst->ident, position);
        }
    }
    return newAccess;
}

/* ParallelCombinePassiveWriteActiveReadChannels : pW x aR parallel combine */
PtrAccess
ParallelCombinePassiveWriteActiveReadChannels (PtrAccess left, PtrAccess right, PtrComponentList * comps, PtrWireList * wires, tPosition position)
{
    /* This can only happen on an internal channel so disallow */
    if (left->info.channel.unplaced || right->info.channel.unplaced)
        LOG_ERROR (CannotUseLocalChannelInSharedBlock, left->inst->ident, position);
    else
    {
        *comps = NewComponentList (NewSynchPullFromListComponent (position, NewWireList (right->info.channel.wire, NULL), left->info.channel.wire), *comps);
        /* Forming a multicast? */
        if (!InstanceCanMulticast (left->inst) && left->info.channel.functioningChannel)
            LOG_ERROR (MulticastWarning, left->inst->ident, position);
    }
    return NULL;                /* No continuation */
}

/* SeriesCombineActiveWriteActiveWriteChannels : aW x aW series combine */
PtrAccess
SeriesCombineActiveWriteActiveWriteChannels (PtrAccess left, PtrAccess right, PtrComponentList * comps, PtrWireList * wires, tPosition position)
{
    PtrAccess newAccess = NULL;

    /* Both unplaced, return one of the accesses */
    if (left->info.channel.unplaced && right->info.channel.unplaced)
        newAccess = CopyAccess (left);
    else if (left->info.channel.unplaced) /* Use right access, set nonMulticast flag */
    {
        newAccess = CopyAccess (right);
        newAccess->info.channel.nonMulticast = true;
    } else if (right->info.channel.unplaced) /* Use left ... */
    {
        newAccess = CopyAccess (left);
        newAccess->info.channel.nonMulticast = true;
    } else
    {
        /* Copy one of the wires for the type of the resulting wire */
        PtrWire newWire = CopyAndRenumberWire (left->info.channel.wire,
          NextBundleNumber (*wires), position);

        /* W x W : WRITE CHAN, PUSH MUX */
        newAccess = NewChannelAccess (left->inst, left->info.channel.nature, newWire);

        *wires = NewWireList (newWire, *wires);
        *comps = NewComponentList (NewCallMuxComponent (position, left->info.channel.wire, right->info.channel.wire, newWire), *comps);
    }
    /* Complete ? */
    newAccess->info.channel.functioningChannel = left->info.channel.functioningChannel && right->info.channel.functioningChannel;

    return newAccess;
}

/* ParallelCombineChannels : combine channel accesses left and right to give a new access and a
   component (which is placed on the front on the component list comps),
   report errors if they occur, using position as the error point */
PtrAccess
ParallelCombineChannels (PtrAccess left, PtrAccess right, PtrComponentList * comps, PtrWireList * wires, bool isPermissive, tPosition position)
{
    PtrAccess newAccess = NULL;

    switch (left->info.channel.nature)
    {
    case ChannelActiveRead:
        switch (right->info.channel.nature)
        {
        case ChannelActiveRead: /* aR, aR */
            newAccess = ParallelCombineActiveReadActiveReadChannels (left, right, comps, wires, position);
            break;
        case ChannelActiveWrite: /* aR, aW */
            newAccess = ParallelCombineActiveReadActiveWriteChannels (left, right, comps, wires, position);
            break;
        case ChannelPassiveRead: /* aR, pR */
            /* Only allow for sync */
            newAccess = ParallelCombinePassiveReadActiveReadChannels (right, left, comps, wires, position);
            break;
        case ChannelPassiveWrite: /* aR, pW */
            newAccess = ParallelCombinePassiveWriteActiveReadChannels (right, left, comps, wires, position);
            break;
        };
        break;
    case ChannelActiveWrite:
        switch (right->info.channel.nature)
        {
        case ChannelActiveRead: /* aW, aR */
            newAccess = ParallelCombineActiveReadActiveWriteChannels (right, left, comps, wires, position);
            break;
        case ChannelActiveWrite: /* aW, aW */
            if (isPermissive)
            {
                newAccess = SeriesCombineActiveWriteActiveWriteChannels (left, right, comps, wires, position);
                LOG_ERROR (PossibleChannelWWConflict, left->inst->ident, position);
            } else
                LOG_ERROR (CannotDoParallelChannelWrite, left->inst->ident, position);
            break;
        case ChannelPassiveRead: /* aW, pR */
            newAccess = ParallelCombinePassiveReadActiveWriteChannels (right, left, comps, wires, position);
            break;
        case ChannelPassiveWrite: /* aW, pW */
            LOG_ERROR (CannotDoParallelChannelWrite, left->inst->ident, position);
            break;
        };
        break;
    case ChannelPassiveRead:
        switch (right->info.channel.nature)
        {
        case ChannelActiveRead: /* pR, aR */
            /* Only allow for sync */
            newAccess = ParallelCombinePassiveReadActiveReadChannels (left, right, comps, wires, position);
            break;
        case ChannelActiveWrite: /* pR, aW */
            newAccess = ParallelCombinePassiveReadActiveWriteChannels (left, right, comps, wires, position);
            break;
        case ChannelPassiveRead: /* pR, pR */
            newAccess = ParallelCombinePassiveReadPassiveReadChannels (left, right, comps, wires, position);
            break;
        case ChannelPassiveWrite: /* pR, pW */
            LOG_ERROR (CannotMixPassiveReadsAndWrites, left->inst->ident, position);
            break;
        };
        break;
    case ChannelPassiveWrite:
        switch (right->info.channel.nature)
        {
        case ChannelActiveRead: /* pW, aR */
            newAccess = ParallelCombinePassiveWriteActiveReadChannels (left, right, comps, wires, position);
            break;
        case ChannelActiveWrite: /* pW, aW */
            LOG_ERROR (CannotDoParallelChannelWrite, left->inst->ident, position);
            break;
        case ChannelPassiveRead: /* pW, pR */
            LOG_ERROR (CannotMixPassiveReadsAndWrites, left->inst->ident, position);
            break;
        case ChannelPassiveWrite: /* pW, pW */
            LOG_ERROR (CannotDoParallelChannelWrite, left->inst->ident, position);
            break;
        };
        break;
    }
    /* nonMulticast property can be inherited from either left or right access */
    if (newAccess)
        newAccess->info.channel.nonMulticast = left->info.channel.nonMulticast || right->info.channel.nonMulticast;
    return newAccess;
}

/* SeriesCombinePassiveReadPassiveReadChannels : pR x pR series combine */
PtrAccess
SeriesCombinePassiveReadPassiveReadChannels (PtrAccess left, PtrAccess right, PtrComponentList * comps, PtrWireList * wires, tPosition position)
{
    PtrAccess newAccess = NULL;

    /* Double unplaced */
    if (left->info.channel.unplaced && right->info.channel.unplaced)
        newAccess = CopyAccess (left);
    else if (left->info.channel.unplaced)
    {
        newAccess = CopyAccess (right);
        newAccess->info.channel.nonMulticast = true;
    } else if (right->info.channel.unplaced)
    {
        newAccess = CopyAccess (left);
        newAccess->info.channel.nonMulticast = true;
    } else
    {
        /* Synch: PassiveRead * PassiveRead to PassiveRead */
        PtrWire newWire = CopyAndRenumberWire (right->info.channel.wire,
          NextBundleNumber (*wires), position);

        newAccess = NewChannelAccess (left->inst, ChannelPassiveRead, newWire);
        /* Multicast ability */
        newAccess->info.channel.nonMulticast = left->info.channel.nonMulticast || right->info.channel.nonMulticast;

        *comps = NewComponentList ((right->info.channel.wire->width == 0 ?
            NewCallActiveComponent : NewCallDemuxPushComponent) (position, newWire, right->info.channel.wire, left->info.channel.wire), *comps);
        *wires = NewWireList (newWire, *wires);
    }
    return newAccess;
}

/* SeriesCombinePassiveReadActiveReadChannels : pR x aR series combine */
PtrAccess
SeriesCombinePassiveReadActiveReadChannels (PtrAccess left, PtrAccess right, PtrComponentList * comps, PtrWireList * wires, tPosition position)
{
    /* Synch: PassiveSync * ActiveSync to ActiveSync */
    PtrWire newWire, passNewWire;
    PtrAccess newAccess = NULL;

    /* Test for unplacable connections */
    if (left->info.channel.unplaced && right->info.channel.unplaced)
        newAccess = CopyAccess (left);
    else if (left->info.channel.unplaced)
    {
        newAccess = CopyAccess (right);
        newAccess->info.channel.nonMulticast = true;
    } else if (right->info.channel.unplaced)
    {
        newAccess = CopyAccess (left);
        newAccess->info.channel.nonMulticast = true;
    } else
    {
        newWire = CopyAndRenumberWire (left->info.channel.wire, NextBundleNumber (*wires), position);
        passNewWire = CopyAndRenumberWire (left->info.channel.wire, NextBundleNumber (*wires) + 1, position);
        newAccess = NewChannelAccess (right->inst, ChannelPassiveRead, newWire);

        *comps = NewComponentList ((right->info.channel.wire->width == 0 ?
            NewCallActiveComponent :
            NewCallDemuxPushComponent) (position, newWire,
            left->info.
            channel.wire,
            passNewWire),
          NewComponentList ((right->info.channel.wire->
              width == 0 ? NewPassivatorComponent : NewPassivatorPushComponent)
              (position, right->info.channel.wire, passNewWire), *comps));
        *wires = NewWireList (passNewWire, NewWireList (newWire, *wires));
    }
    return newAccess;
}

/* SeriesCombineActiveReadActiveReadChannels : aR x aR series combine */
PtrAccess
SeriesCombineActiveReadActiveReadChannels (PtrAccess left, PtrAccess right, PtrComponentList * comps, PtrWireList * wires, tPosition position)
{
    PtrAccess newAccess = NULL;

    /* Both unplaced, return one of the accesses */
    if (left->info.channel.unplaced && right->info.channel.unplaced)
        newAccess = CopyAccess (left);
    else if (left->info.channel.unplaced) /* Use right access, set nonMulticast flag */
    {
        newAccess = CopyAccess (right);
        newAccess->info.channel.nonMulticast = true;
    } else if (right->info.channel.unplaced) /* Use left ... */
    {
        newAccess = CopyAccess (left);
        newAccess->info.channel.nonMulticast = true;
    } else
    {
        /* Copy one of the wires for the type of the resulting wire */
        PtrWire newWire = CopyAndRenumberWire (left->info.channel.wire,
          NextBundleNumber (*wires), position);

        /* R x R : READ CHAN, PULL DEMUX */
        newAccess = NewChannelAccess (left->inst, left->info.channel.nature, newWire);

        *wires = NewWireList (newWire, *wires);
        *comps = NewComponentList ((left->inst->type->nature == SyncType ?
            NewCallComponent : NewCallDemuxComponent) (position, left->info.channel.wire, right->info.channel.wire, newWire), *comps);
    }
    /* Complete ? */
    newAccess->info.channel.functioningChannel = left->info.channel.functioningChannel && right->info.channel.functioningChannel;

    return newAccess;
}

/* SeriesCombineChannels : Combine channel accesses in series, if ignorePassive is true then allow
   series PassiveReads if one side is unplaced */
PtrAccess
SeriesCombineChannels (PtrAccess left, PtrAccess right, PtrComponentList * comps, PtrWireList * wires, bool ignorePassive, tPosition position)
{
    PtrAccess newAccess = NULL;

    bool leftPassive = left->info.channel.nature == ChannelPassiveRead;
    bool rightPassive = right->info.channel.nature == ChannelPassiveRead;
    bool leftUnplaced = left->info.channel.unplaced;
    bool rightUnplaced = right->info.channel.unplaced;

    /* This is a cop-out to allow Series mixing of block bodies and shared block bodies where
       the block body has some unplaced accesses left in it */
    if (ignorePassive && leftPassive && rightPassive && (leftUnplaced || rightUnplaced))
    {
        if (leftUnplaced)
            newAccess = CopyAccess (right);
        else if (rightUnplaced)
            newAccess = CopyAccess (left);
    } else
    {
        switch (left->info.channel.nature)
        {
        case ChannelActiveRead:
            switch (right->info.channel.nature)
            {
            case ChannelActiveRead: /* aR, aR */
                newAccess = SeriesCombineActiveReadActiveReadChannels (left, right, comps, wires, position);
                break;
            case ChannelActiveWrite: /* aR, aW */
                LOG_ERROR (UnbalancedChannelLeftRead, left->inst->ident, position);
                break;
            case ChannelPassiveRead: /* aR, pR */
                LOG_ERROR (AllowSequentialSelection ? SerialUseOfPassiveChannelWarning : SerialUseOfPassiveChannelError, left->inst->ident, position);
                newAccess = SeriesCombinePassiveReadActiveReadChannels (right, left, comps, wires, position);
                break;
            case ChannelPassiveWrite: /* aR, pW */
                LOG_ERROR (UnbalancedChannelLeftRead, left->inst->ident, position);
                break;
            };
            break;
        case ChannelActiveWrite:
            switch (right->info.channel.nature)
            {
            case ChannelActiveRead: /* aW, aR */
                LOG_ERROR (UnbalancedChannelRightRead, left->inst->ident, position);
                break;
            case ChannelActiveWrite: /* aW, aW */
                newAccess = SeriesCombineActiveWriteActiveWriteChannels (left, right, comps, wires, position);
                break;
            case ChannelPassiveRead: /* aW, pR */
                LOG_ERROR (UnbalancedChannelRightRead, left->inst->ident, position);
                break;
            case ChannelPassiveWrite: /* aW, pW */
                LOG_ERROR (CannotMixActiveAndPassiveWrites, left->inst->ident, position);
                break;
            };
            break;
        case ChannelPassiveRead:
            switch (right->info.channel.nature)
            {
            case ChannelActiveRead: /* pR, aR */
                LOG_ERROR (AllowSequentialSelection ? SerialUseOfPassiveChannelWarning : SerialUseOfPassiveChannelError, left->inst->ident, position);
                newAccess = SeriesCombinePassiveReadActiveReadChannels (left, right, comps, wires, position);
                break;
            case ChannelActiveWrite: /* pR, aW */
                LOG_ERROR (UnbalancedChannelLeftRead, left->inst->ident, position);
                break;
            case ChannelPassiveRead: /* pR, pR */
                LOG_ERROR (AllowSequentialSelection ? SerialUseOfPassiveChannelWarning : SerialUseOfPassiveChannelError, left->inst->ident, position);
                newAccess = SeriesCombinePassiveReadPassiveReadChannels (left, right, comps, wires, position);
                break;
            case ChannelPassiveWrite: /* pR, pW */
                LOG_ERROR (UnbalancedChannelLeftRead, left->inst->ident, position);
                break;
            };
            break;
        case ChannelPassiveWrite:
            switch (right->info.channel.nature)
            {
            case ChannelActiveRead: /* pW, aR */
                LOG_ERROR (UnbalancedChannelRightRead, left->inst->ident, position);
                break;
            case ChannelActiveWrite: /* pW, aW */
                LOG_ERROR (CannotMixActiveAndPassiveWrites, left->inst->ident, position);
                break;
            case ChannelPassiveRead: /* pW, pR */
                LOG_ERROR (UnbalancedChannelRightRead, left->inst->ident, position);
                break;
            case ChannelPassiveWrite: /* pW, pW */
                LOG_ERROR (CannotSequencePassiveWrites, left->inst->ident, position);
                /* FIXME */
                break;
            };
            break;
        }
        if (newAccess)
            newAccess->info.channel.nonMulticast =
              newAccess->info.channel.nonMulticast || left->info.channel.nonMulticast || right->info.channel.nonMulticast;
    }

    return newAccess;
}

/* CombineVariables : combine variable accesses (analoguous to CombineChannels), variable
   accesses are not actually combined but the read and write access lists are compared
   and checked for read/write and write/write conflicts if parallelNotSerial is true,
   ie. parallelNotSerial == true : Disallow simultaneous conflicting accesses
   parallelNotSerial == false : Allow left and right accesses to 'conflict' */
PtrAccess CombineVariables (PtrAccess left, PtrAccess right, tPosition position, bool parallelNotSerial, bool isPermissive)
{
    PtrAccess jointAccesses = NewAccess (left->inst); /* This is an access to left->inst == right->inst */

    /* Copy the conflict bit lists */
    jointAccesses->info.variable.readMap = NewMP_INT (0);
    jointAccesses->info.variable.writeMap = NewMP_INT (0);

    /* Or the left and right read accesses together */
    mpz_ior (jointAccesses->info.variable.readMap, left->info.variable.readMap, right->info.variable.readMap);
    mpz_ior (jointAccesses->info.variable.writeMap, left->info.variable.writeMap, right->info.variable.writeMap);

    if (parallelNotSerial)      /* Check accesses for conflicts */
    {
        mpz_t tmp, tmp2;

        mpz_init (tmp);
        mpz_init (tmp2);

        /* test left write to right read conflict */
        mpz_and (tmp, left->info.variable.writeMap, right->info.variable.readMap);
        mpz_and (tmp2, left->info.variable.readMap, right->info.variable.writeMap);

        if (mpz_sgn (tmp) != 0 || mpz_sgn (tmp2) != 0)
        {
            if (isPermissive)
                LOG_ERROR (VariableReadWriteConflictWarning, left->inst->ident, position);
            else
                LOG_ERROR (VariableReadWriteConflict, left->inst->ident, position);
        }
        /* test write/write conflict */
        mpz_and (tmp, left->info.variable.writeMap, right->info.variable.writeMap);
        if (mpz_sgn (tmp) != 0)
        {
            if (isPermissive)
                LOG_ERROR (VariableWriteWriteConflictWarning, left->inst->ident, position);
            else
                LOG_ERROR (VariableWriteWriteConflict, left->inst->ident, position);
        }
        mpz_clear (tmp);
        mpz_clear (tmp2);
    }
    /* Copy over the right and left variable access lists to the jointAccesses */
    jointAccesses->info.variable.wires = AppendWireLists (CopyWireList (left->info.variable.wires), CopyWireList (right->info.variable.wires));
    return jointAccesses;
}

/* ParallelCombineVal : combine two accesses to the same val declaration with a SynchPullComponent */
PtrAccess ParallelCombineVal (PtrAccess left, PtrAccess right, PtrComponentList * comps, PtrWireList * wires, tPosition position)
{
    /* Create the new activation wire */
    PtrWire newWire = NewWire (NextBundleNumber (*wires),
      left->inst->info.val.activation->width,
      left->inst->type,
      0, true /* pull */ ,
      position);

    left->inst->info.val.newActivation = newWire;
    *comps = NewComponentList (NewSynchPullComponent (position, left->info.val.wire, right->info.val.wire, newWire), *comps);
    *wires = NewWireList (newWire, *wires);
    return NewValAccess (left->inst, newWire);
}

/* ParallelCombineAccesses : join two lists of accessess together 
   Adds to the list of components in comp, and wires in wires 
   NB if canGenerateFORK != NULL && left and right accesss 
   communicate with each other (R x W, W x R) then *canGenerateFORK gets false
   (to use this initialise *canGenerateFORK to true)
   If `isPermissive' is true then allow parallel W/W and W/R variable conflicts and W/W channel conflicts */
PtrAccessList
ParallelCombineAccesses (PtrAccessList left, PtrAccessList right,
  PtrComponentList * comps, PtrWireList * wires, tPosition position, bool * canGenerateFORK, bool isPermissive)
{
    /* Handle empty lists */
    if (!left)
        return right;
    if (!right)
        return left;
    /* The three combination cases, this function produces a sorted (by instance pointer) list
       of accesss, this allows faster list combination by crawling across the two lists and
       only taking action on the equal elements */
    if (CAR (left)->inst > CAR (right)->inst)
        return NewAccessList (CAR (left), ParallelCombineAccesses (CDR (left), right, comps, wires, position, canGenerateFORK, isPermissive));

    else if (CAR (left)->inst < CAR (right)->inst)
        return NewAccessList (CAR (right), ParallelCombineAccesses (left, CDR (right), comps, wires, position, canGenerateFORK, isPermissive));

    else
        /* if (CAR(left)->inst == CAR(right)->inst) */
    {
        switch (CAR (left)->inst->nature)
        {
        case VariableInstance: /* VariableAccess */
            return NewAccessList (CombineVariables (CAR (left), CAR (right), position, true /* Parallel */ ,
                isPermissive), ParallelCombineAccesses (CDR (left), CDR (right), comps, wires, position, canGenerateFORK, isPermissive));
            break;
        case SharedCallInstance:
            LOG_ERROR (CannotCallInParallel, CAR (left)->inst->ident, position);
            return NULL;
            break;
        case ValInstance:
            {
                PtrAccess newAccess = ParallelCombineVal (CAR (left), CAR (right), comps, wires,
                  position);

                return NewAccessList (newAccess,
                  ParallelCombineAccesses (CDR (left), CDR (right), comps, wires, position, canGenerateFORK, isPermissive));
            }
            break;
        default:               /* Must be some channel flavour */
            {
                PtrAccess combinedAccess;

                /* Check if left and right talk to each other, common reads are OK, common writes
                   aren't but chanBundle will be NULL and we wont get any code anyway */
                if (canGenerateFORK
                  && !(CAR (left)->info.channel.nature != ChannelActiveWrite && CAR (right)->info.channel.nature != ChannelActiveWrite))
                    *canGenerateFORK = false;

                /* Ordering is not significant on comp mods. */
                combinedAccess = ParallelCombineChannels (CAR (left), CAR (right), comps, wires, isPermissive, position);

                if (combinedAccess) /* no error occurred in combination */
                    return NewAccessList (combinedAccess,
                      ParallelCombineAccesses (CDR (left), CDR (right), comps, wires, position, canGenerateFORK, isPermissive));
                else
                    return ParallelCombineAccesses (CDR (left), CDR (right), comps, wires, position, canGenerateFORK, isPermissive);
                break;
            }
        }
    }
    return NULL;                /* Just to stop the compiler moaning */
}

/* SeriesCombineSharedCalls : combine 2 shared calls with a Call component */
PtrAccess SeriesCombineSharedCalls (PtrAccess left, PtrAccess right, PtrComponentList * comps, PtrWireList * wires, tPosition position)
{
    PtrAccess newAccess;

    /* If an input was unplaced, return a copy of the other one */
    if (left->info.shared.unplaced)
        newAccess = CopyAccess (right);
    else if (right->info.shared.unplaced)
        newAccess = CopyAccess (left);
    else
    {
        PtrWire newAct = NewSyncWire (NextBundleNumber (*wires), position);

        *comps = NewComponentList (NewCallComponent (position, left->info.shared.wire, right->info.shared.wire, newAct), *comps);
        *wires = NewWireList (newAct, *wires);
        newAccess = NewSharedCallAccess (left->info.shared.procedure, newAct);
    }
    return newAccess;
}

/* SeriesCombineAccessesBody : body of SeriesCombineAccesses and SharedCombineAccesses */
PtrAccessList
SeriesCombineAccessesBody (PtrAccessList left, PtrAccessList right,
  PtrComponentList * comps, PtrWireList * wires, bool ignorePassive, tPosition position)
{
    /* Handle empty lists */
    if (!left)
    {
        return right;
    }
    if (!right)
    {
        return left;
    }
    /* The three combination cases, this function produces a sorted (by instance pointer) list
       of accesss, this allows faster list combination by crawling across the two lists and
       only taking action on the equal elements */
    if (CAR (left)->inst > CAR (right)->inst)
        return NewAccessList (CAR (left), SeriesCombineAccessesBody (CDR (left), right, comps, wires, ignorePassive, position));

    else if (CAR (left)->inst < CAR (right)->inst)
        return NewAccessList (CAR (right), SeriesCombineAccessesBody (left, CDR (right), comps, wires, ignorePassive, position));

    else
        /* if (CAR(left)->inst == CAR(right)->inst) */
    {
        switch (CAR (left)->inst->nature)
        {
        case VariableInstance: /* VariableAccess */
            return NewAccessList (CombineVariables (CAR (left), CAR (right), position, false /* Sequential */ , false),
              SeriesCombineAccessesBody (CDR (left), CDR (right), comps, wires, ignorePassive, position));
            break;
        case SharedCallInstance:
            {
                PtrAccess newAccess = SeriesCombineSharedCalls (CAR (left), CAR (right), comps, wires,
                  position);

                return NewAccessList (newAccess, SeriesCombineAccessesBody (CDR (left), CDR (right), comps, wires, ignorePassive, position));
            }
            break;
        case ValInstance:
            /* FIXME */
            return NULL;
            /*
               return NewAccessList (
               ParallelCombineVal (CAR(left), CAR(right), comps, wires, position),
               ParallelCombineAccesses (CDR(left), CDR(right), comps, wires, position, 
               canGenerateFORK, false) );
             */
            break;
        default:               /* Must be some channel flavour */
            {
                PtrAccess chanAccess = SeriesCombineChannels (CAR (left), CAR (right), comps, wires,
                  ignorePassive,
                  position);
                PtrAccessList otherAccesses = SeriesCombineAccessesBody (CDR (left), CDR (right), comps, wires, ignorePassive,
                  position);

                /* Ordering is not significant on comp mods. */
                return (chanAccess ? NewAccessList (chanAccess, otherAccesses) : otherAccesses);
            }
            break;
        }
    }
    return NULL;                /* Just to stop the compiler moaning */
}

/* SeriesCombineAccesses : similar to parallel combine but without the FORK generation */
PtrAccessList SeriesCombineAccesses (PtrAccessList left, PtrAccessList right, PtrComponentList * comps, PtrWireList * wires, tPosition position)
{
    return SeriesCombineAccessesBody (left, right, comps, wires, false, position);
}

/* SharedCombineAccesses : same as SeriesCombine but don't moan about series accesses to passive
   channels where one access is unplaced */
PtrAccessList SharedCombineAccesses (PtrAccessList left, PtrAccessList right, PtrComponentList * comps, PtrWireList * wires, tPosition position)
{
    return SeriesCombineAccessesBody (left, right, comps, wires, true, position);
}

/* FilterOutAccessToInstance : remove instance accesses from the given AccessList
   to the given instance, returning the filtered out access */
PtrAccess FilterOutAccessToInstance (PtrAccessList * accesses, PtrInstance inst)
{
    PtrAccessList acc;

    if (!*accesses)
        return NULL;

    acc = *accesses;
    if (CAR (acc)->inst == inst)
    {
        *accesses = CDR (acc);
        return CAR (acc);
    } else
        /* FOR_EACH (acc, CDR (acc), ...) */
        while (acc && CDR (acc))
        {
            if (CADR (acc)->inst == inst)
            {
                PtrAccess ret = CADR (acc);

                acc->next = CDDR (acc);
                return ret;
            }
            acc = CDR (acc);
        }
    return NULL;
}

/* LookupVarAccess : returns access structure if wire is a variable access in the AccessList accesses */
PtrAccess LookupVarAccess (PtrAccessList accesses, PtrWire wire)
{
    /* FOR_EACH (accesses, true, ...) */
    while (accesses && true)    /* Search accesses */
    {
        if (CAR (accesses)->inst->nature == VariableInstance)
        {
            PtrWireList connections = CAR (accesses)->info.variable.wires;

            /* FOR_EACH (connections, true, ...) */
            while (connections) /* Search variable connections */
            {
                if (CAR (connections) == wire)
                    return CAR (accesses);
                connections = CDR (connections);
            }
        }
        accesses = CDR (accesses);
    }
    return NULL;
}

/* UpdateVarAccessMap : update one or other of the bit maps in a variable access. isRead == true => update readMap,
   isRead == false => update writeMap */
void UpdateVarAccessMap (PtrAccess varAccess, bool isRead)
{
    PtrWireList wires;
    PtrMP_INT map;

    ASSERT (varAccess);
    ASSERT (varAccess->inst->nature == VariableInstance);

    wires = varAccess->info.variable.wires;
    map = (isRead ? varAccess->info.variable.readMap : varAccess->info.variable.writeMap);
    mpz_set_ui (map, 0);

    /* FOR_EACH (wires, true, ...) */
    while (wires && true)
    {
        if (isRead == CAR (wires)->isPull)
        {
            PtrMP_INT mask = MakeMaskForRange (CAR (wires)->width, CAR (wires)->offset);

            mpz_ior (map, map, mask);
            DeleteMP_INT (mask);
        }
        wires = CDR (wires);
    }
}

/* VariableParallelAccessConflict : returns true if the two variable accesses either both write or one reads and the other
   writes to overlaping fragments of their shared variable */
bool VariableParallelAccessConflict (PtrAccess left, PtrAccess right)
{
    bool ret = false;
    mpz_t tmp;

    mpz_init (tmp);

    /* test left write to right read conflict */
    mpz_and (tmp, left->info.variable.writeMap, right->info.variable.readMap);
    if (mpz_sgn (tmp) != 0)
        ret = true;
    mpz_and (tmp, left->info.variable.readMap, right->info.variable.writeMap);
    if (mpz_sgn (tmp) != 0)
        ret = true;

    /* test write/write conflict */
    mpz_and (tmp, left->info.variable.writeMap, right->info.variable.writeMap);
    if (mpz_sgn (tmp) != 0)
        ret = true;

    mpz_clear (tmp);

    return ret;
}

/* VariableParallelAccessConflictInAccessList : As above but iterate across a pair of access lists */
bool VariableParallelAccessConflictInAccessList (PtrAccessList left, PtrAccessList right)
{
    while (left && right)
    {
        if (CAR (left)->inst > CAR (right)->inst)
            left = CDR (left);
        else if (CAR (left)->inst < CAR (right)->inst)
            right = CDR (right);
        else if (CAR (left)->inst == CAR (right)->inst
          && CAR (left)->inst->nature == VariableInstance && VariableParallelAccessConflict (CAR (left), CAR (right)))
            return true;        /* Does conflict */
        else
        {
            left = CDR (left);
            right = CDR (right);
        }
    }
    return false;               /* Doesn't conflict */
}

/* MakeReadOnlyVariablesFromInstances : take the given list of attributes and replicate the channel instances
   referred to (making them read only variables).  Iterate down idents too and use those
   names for the read only variables (if not NoIdent).  Only makes variables for accesses in
   `attributes' of nature `accessNature' */
PtrInstanceList MakeReadOnlyVariablesFromInstances (PtrExprAttributesList attributes, PtrIdentList idents, ChannelAccessNature accessNature)
{
    PtrInstanceList vars = NULL;

    /* FOR_EACH (attributes, idents, ...) */
    while (attributes && idents)
    {
        if (!CAR (attributes).accesses) /* Check we have at least one access */
            return vars;
        else
        {
            PtrInstance var;
            PtrAccess access = CAR (CAR (attributes).accesses);

            if (access->info.channel.nature == accessNature)
            {
                var = NewVariableInstance ((CAR (idents).ident == NoIdent ? access->inst->ident : CAR (idents).ident), access->inst->type);
                access->inst->selectionInfo.fvar = var; /* Mod this channel instance */
                var->info.variable.readOnly = true;
                var->info.variable.channelInstance = access->inst;
                vars = NewInstanceList (var, vars);
            }
        }
        idents = CDR (idents);
        attributes = CDR (attributes);
    }
    return vars;
}

/* CopyAndRemapWiresOfChannelAccess : remap the wire of the given access to a wire in
   wireArray in a copy of access */
PtrAccess CopyAndRemapWiresOfChannelAccess (PtrAccess access, PtrWireArray wireArray)
{
    PtrAccess ret = NEW (Access);

    *ret = *access;             /* Elementwise copy */
    ret->info.channel.wire = wireArray[access->info.channel.wire->bundleNo - 1].body;
    return ret;
}

/* CopyAndRemapWiresOfSharedCallAccess : ditto de SharedCall */
PtrAccess CopyAndRemapWiresOfSharedCallAccess (PtrAccess access, PtrWireArray wireArray)
{
    PtrAccess ret = NEW (Access);
    PtrWire newActivationWire;

    *ret = *access;             /* Elementwise copy */
    ASSERT (access->info.shared.wire);

    /* Map the activation wire into the new wire set */
    /* Only map the placed wires, the unplaced ones wont be local */
    if (!access->info.shared.unplaced)
        newActivationWire = wireArray[access->info.shared.wire->bundleNo - 1].body;
    else
        newActivationWire = ret->info.shared.wire;

    /* Change the unplacedActivation in the shared procedure to match this activation
       if that was this wire */
    if (ret->info.shared.wire == ret->info.shared.procedure->unplacedActivation)
    {
        ret->info.shared.procedure->unplacedActivation = newActivationWire;
    }
    ret->info.shared.wire = newActivationWire;
    return ret;
}

/* CopyAndRemapWiresOfVariableAccess : as above but for a variable access */
PtrAccess CopyAndRemapWiresOfVariableAccess (PtrAccess access, PtrWireArray wireArray)
{
    PtrAccess ret = NEW (Access);

    *ret = *access;             /* Elementwise copy */
    ret->info.variable.readMap = CopyMP_INT (access->info.variable.readMap);
    ret->info.variable.writeMap = CopyMP_INT (access->info.variable.writeMap);
    ret->info.variable.wires = CopyAndRemapWiresOfWireList (access->info.variable.wires, wireArray);
    return ret;
}

/* CopyAndRemapWiresOfAccessList : copy an access list (including copies of all MP_INT maps)
   and change all the wires to those from the wire array/list wireArray */
PtrAccessList CopyAndRemapWiresOfAccessList (PtrAccessList accesses, PtrWireArray wireArray)
{
    if (accesses)
    {
        switch (CAR (accesses)->inst->nature)
        {
        case VariableInstance: /* VariableAccess */
            return
              NewAccessList (CopyAndRemapWiresOfVariableAccess
              (CAR (accesses), wireArray), CopyAndRemapWiresOfAccessList (CDR (accesses), wireArray));
            break;
        case SharedCallInstance:
            return
              NewAccessList (CopyAndRemapWiresOfSharedCallAccess
              (CAR (accesses), wireArray), CopyAndRemapWiresOfAccessList (CDR (accesses), wireArray));
            break;
        case ValInstance:
            /* FIXME */
            return NULL;
            break;
        default:               /* Must be some channel flavour */
            return
              NewAccessList (CopyAndRemapWiresOfChannelAccess (CAR (accesses), wireArray), CopyAndRemapWiresOfAccessList (CDR (accesses), wireArray));
            break;
        }
    }
    return NULL;
}

/* CheckAccessListForNonPortChannels : raise an error if the given accesslist contains references
   to local channels, used to check shared blocks for illegal channel ops, returns true if
   no errors were generated */
bool CheckAccessListForNonPortChannels (PtrAccessList accesses)
{
    bool ret = true;

    /* FOR_EACH (accesses, true, ...) */
    while (accesses && true)
    {
        /* Local channel?, only bother reporting for placed channels */
        if (CAR (accesses)->inst->nature == ChannelInstance && !CAR (accesses)->info.channel.unplaced)
        {
            LOG_ERROR (CannotUseLocalChannelInSharedBlock, CAR (accesses)->inst->ident, CAR (accesses)->info.channel.wire->position);
            ret = false;
        }
        accesses = CDR (accesses);
    }
    return ret;
}

/* AccessIsChannelOutput : returns true if the given access is an output on a channel */
bool AccessIsChannelOutput (PtrAccess access)
{
	bool ret = false;
	switch (access->inst->nature)
	{
	case InputChannelInstance:
	case ChannelInstance:
		ret = access->info.channel.nature == ChannelActiveWrite ||
			access->info.channel.nature == ChannelPassiveWrite ||
			access->inst->type == SyncTypeObj;
		break;
	case OutputChannelInstance:
		ret = true;
		break;
	default:
		break;
	}

	return ret;
}

bool CheckWAWandWAR_internal (PtrAccessList left, PtrAccessList right, bool leftOutputDetected, bool rightOutputDetected)
{
	PtrInstance leftInst, rightInst;

    /* Handle empty lists */
    if (!left || !right)
        return false;

	leftInst = CAR (left)->inst;
	rightInst = CAR (right)->inst;

	if (AccessIsChannelOutput (CAR (left))) leftOutputDetected = true;
	if (AccessIsChannelOutput (CAR (right))) rightOutputDetected = true;

    if (leftOutputDetected && rightOutputDetected)
        return true;

    if (leftInst > rightInst)
        return CheckWAWandWAR_internal (CDR (left), right, leftOutputDetected, rightOutputDetected);
    else if (leftInst < rightInst)
        return CheckWAWandWAR_internal (left, CDR (right), leftOutputDetected, rightOutputDetected);
    else
// if (rightInst->nature == leftInst->nature)  // test always true, as leftInst==rightInst
    {
        switch (leftInst->nature)
        {
        case VariableInstance: /* VariableAccess */
            {
                mpz_t tmp;

                mpz_init (tmp);

                /* test write after read conflict */
                mpz_and (tmp, CAR (left)->info.variable.readMap, CAR (right)->info.variable.writeMap);
                if (mpz_sgn (tmp) != 0)
                {
                    mpz_clear (tmp);
                    return true; // *WAR = true;
                }

                /* test write/write conflict */
                mpz_and (tmp, CAR (left)->info.variable.writeMap, CAR (right)->info.variable.writeMap);
                if (mpz_sgn (tmp) != 0)
                {
                    mpz_clear (tmp);
                    return true; // *WAW = true;
                }
                mpz_clear (tmp);
            }
            break;
        case OutputChannelInstance: // WAW
        case InputChannelInstance: // In this case it actually is a RAR (read after read), but it doesn't matter
        case ChannelInstance:	// Local channels can be just as slippery 2007-05-08 AB
        case SharedCallInstance: // uncertain case => keep it safe by returning true
        case ValInstance:      // uncertain case => keep it safe by returning true
            return true;
        default:
            return true;
        }
        return CheckWAWandWAR_internal (CDR (left), CDR (right), leftOutputDetected, rightOutputDetected);
    }
}

/* CheckWAWandWAR : return true if a WAW or a WAR access occurs. Return false otherwise */
bool CheckWAWandWAR (PtrAccessList left, PtrAccessList right)
{
    return CheckWAWandWAR_internal (left, right, false, false);
}

/* FindChannelsFromAccessList : returns a list of channels from all the accesses in the given list */
PtrInstanceList FindChannelsFromAccessList (PtrAccessList accesses)
{
	PtrInstanceList ret = NULL;

	while (accesses)
	{
		switch (CAR (accesses)->inst->nature)
		{
		case OutputChannelInstance:
		case InputChannelInstance:
		case ChannelInstance:
			ret = NewInstanceList (CAR (accesses)->inst, ret);
		default:
			break;
		}
		accesses = CDR (accesses);
	}
	return ret;
}

/* StrPtrAccess : print out an individual access */
void StrPtrAccess (FILE * stream, PtrAccess access)
{
    fprintf (stream, "(");
    WriteIdent (stream, access->inst->ident);
    fprintf (stream, ": ");

    switch (access->inst->nature)
    {
    case VariableInstance:
        fprintf (stream, "VAR read: ");
        StrFilterMask (stream, access->info.variable.readMap, Abs (access->inst->type->size));
        fprintf (stream, " write: ");
        StrFilterMask (stream, access->info.variable.writeMap, Abs (access->inst->type->size));
        fprintf (stream, " (");
        StrPtrWireList (stream, access->info.variable.wires, true, ",", true);
        putc (')', stream);
        break;
    case SharedCallInstance:
        /* Shared Call Access */
        fprintf (stream, "CALL ");
        StrPtrWire (stream, access->info.shared.wire);
        break;
    case ValInstance:
        fprintf (stream, "VAL ");
        StrPtrWire (stream, access->info.val.wire);
        break;
    default:                   /* Channel access */
        fprintf (stream, "CHAN %s%s%s",
          (access->info.channel.nature ==
            ChannelActiveRead ? "READ " : (access->info.channel.nature ==
              ChannelPassiveRead ?
              "PASSIVE_READ " : "WRITE ")),
          (access->info.channel.functioningChannel ? "DONE " : "PART "),
          (access->info.channel.unplaced ? "UNPLACED" : (access->info.channel.nonMulticast ? "NOMULTI" : "")));
        /* if (! access->info.channel.unplaced) StrPtrWire (stream, access->info.shared.wire); */
        break;
    }
    putc (')', stream);
}

/* StrPtrAccessList : print out an access list */
void StrPtrAccessList (FILE * stream, PtrAccessList list)
{
    /* FOR_EACH (list, true, ...) */
    while (list && true)
    {
        StrPtrAccess (stream, CAR (list));
        if (CDR (list))
            fprintf (stream, ", ");
        list = CDR (list);
    }
}
