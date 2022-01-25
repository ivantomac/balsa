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

	`channels.c'
	Channel expression handling functions
	
 */

#include "channels.h"
#include "Errors.h"

/* MakeAttributesForChannelInstanceAccess: make an ExprAttributes for an access on a given instance */
ExprAttributes MakeAttributesForChannelInstanceAccess (PtrInstance instance, bool isInput, bool isPassive, bool lockChannels, tPosition position)
{
    ExprAttributes ret = NoExprAttributes;

    PtrWire accessWire = NewWire (1, Abs (instance->type->size), instance->type, 0,
      (isInput && !isPassive) || (!isInput && isPassive)
      /* Input:Pull/true, Output/Passive input:Push/false */ ,
      position);

    accessWire->ident = instance->ident;

    NameWire (accessWire, instance->ident);

    ret.value.type = instance->type;
    ret.value.value.baseValue = NULL;
    ret.accesses =
      NewAccessList (NewChannelAccess
      (instance,
        (isInput ? (isPassive ? ChannelPassiveRead : ChannelActiveRead) : (isPassive ? ChannelPassiveWrite : ChannelActiveWrite)), accessWire), NULL);
    ret.value.type = instance->type;
    ret.activation = accessWire;
    ret.wires = NewWireList (accessWire, NULL);
    if (isPassive && lockChannels) /* Lock passive channels, used in channel guards */
        instance->locked = true; /* lock instance */

    return ret;
}

/* MakeAttributesListForChannelInstancesAccesses: generate a list of accesses for a list
   of channel instances.  Only produces a maximum of `instanceCount' accesses */
PtrExprAttributesList
MakeAttributesListForChannelInstancesAccesses (PtrInstanceList instances,
  unsigned instanceCount, bool isInput, bool isPassive, bool lockChannels, tPosition position)
{
    PtrExprAttributesList retHead = NULL;
    PtrExprAttributesList retTail = NULL;

    /* FOR_EACH (instances, CAR (instances)->nature != ContextMarkerInstance && instanceCount, ...) */
    while (instances && CAR (instances)->nature != ContextMarkerInstance && instanceCount)
    {
        PtrExprAttributesList attr = NewExprAttributesList (MakeAttributesForChannelInstanceAccess (CAR (instances), isInput, isPassive,
            lockChannels, position), NULL);

        if (retTail)
        {
            retTail->next = attr;
            retTail = CDR (retTail);
        } else
            retHead = retTail = attr;
        instanceCount--;
        instances = CDR (instances);
    }

    return retHead;
}

/* HandleIdentChannel : resolve an instance representing a channel and
   generate the appropriate access hardware.  Invalidates (makes NULL) *instance
   on error */
ExprAttributes HandleIdentChannel (PtrContext context, PtrInstance * instance, bool isInput, bool isPassive, bool lockChannels, tPosition position)
{
    ExprAttributes ret = NoExprAttributes;
    PtrInstance thisInstance = *instance;

    *instance = NULL;           /* Assume the worst */

    if (thisInstance->locked && isPassive) /* Can't lock a channel twice */
        LOG_ERROR (ChannelAlreadyLocked, thisInstance->ident, position);
    else if (thisInstance->locked)
        /* Don't allow access to locked channels */
        LOG_ERROR (CannotAccessLockedChannel, thisInstance->ident, position);
    /* Not required if ports are assumed to be multicast
       else if (thisInstance->nature == OutputChannelInstance && isInput)
       LOG_ERROR (ExpectingAReadableChannel, NoIdent, position);
     */
    else if (thisInstance->nature == InputChannelInstance && !isInput)
        LOG_ERROR (ExpectingAWritableChannel, NoIdent, position);
    /* Arrayed type */
    else if (thisInstance->type->nature == ArrayedType)
    {
        ret.value.type = thisInstance->type;
        *instance = thisInstance;
    } else
    {
        *instance = thisInstance;
        ret = MakeAttributesForChannelInstanceAccess (thisInstance, isInput, isPassive, lockChannels, position);
    }

    return ret;
}

/* HandleArrayedElemChannel : resolve a arrayed access on a channel remain quiet
   if an error was reported `upstream', *instance will contain on exit the same things as in
   HandleIdentChannel. */
ExprAttributes
HandleArrayedElemChannel (PtrContext context, ExprAttributes lhsAttr,
  PtrInstance lhsInstance, ExprAttributes expr, PtrInstance * instance, bool isInput, bool isPassive, tPosition position)
{
    ExprAttributes ret = NoExprAttributes;
    Span lhsRange = (lhsAttr.value.type->nature == ArrayedType ? lhsAttr.value.type->info.arrayed.range : NoSpan);

    *instance = NULL;

    if (!lhsInstance)
    {                           /* Do nowt */
    }
    /* Bow out now if previous Channel failed */
    else if (lhsAttr.value.type->nature != ArrayedType)
        LOG_ERROR (ExpectingAnArrayedChannel, NoIdent, position);
    else if (!SubscriptTypeIsOK (lhsRange.boundingType, expr.value.type))
        LOG_ERROR (SubscriptHasWrongType, NoIdent, position);
    else if (!expr.value.value.baseValue) /* Non-const */
    {
        LOG_ERROR (CantDoNonConstArrayedChannelIndex, NoIdent, position);
    } else
        /* Constant index */
    {
        if (expr.value.value.dontCares)
            LOG_ERROR (ImplicantNotAllowedHere, NoIdent, position);
        else if (mpz_cmp (expr.value.value.baseValue, lhsRange.lowerBound) < 0 || mpz_cmp (expr.value.value.baseValue, lhsRange.upperBound) > 0)
            LOG_ERROR (SubscriptOutOfRange, NoIdent, position);
        else
        {
            PtrInstance elem;
            PtrWire accessWire;

            {                   /* Find the element offset */
                PtrMP_INT elementOffset = NewMP_INT (0);

                mpz_sub (elementOffset, expr.value.value.baseValue, lhsAttr.value.type->info.arrayed.range.lowerBound);
                elem = NthOfInstanceList (lhsAttr.value.type->info.arrayed.arrayedElements, mpz_get_ui (elementOffset));
                DeleteMP_INT (elementOffset);
            }

            /* Check that element isn't locked */
            if (elem->locked && isPassive) /* Can't lock a channel twice */
                LOG_ERROR (ChannelAlreadyLocked, elem->ident, position);
#if 0
/* Don't need this, investigate why */
            if (elem->locked)
                /* Don't allow access to locked channels */
                LOG_ERROR (CannotAccessLockedChannel, elem->ident, position);
#endif
            else
            {
                accessWire = NewWire (1, Abs (elem->type->size), elem->type, 0, isInput && !isPassive
                  /* Input:Pull/true, Output/Passive input:Push/false */
                  , position);
                NameWire (accessWire, elem->ident);
                ret.value.type = elem->type;
                ret.value.value.baseValue = NULL;
                ret.accesses =
                  NewAccessList (NewChannelAccess
                  (elem, (isInput ? (isPassive ? ChannelPassiveRead : ChannelActiveRead) : ChannelActiveWrite), accessWire), NULL);
                ret.activation = accessWire;
                ret.wires = NewWireList (accessWire, NULL);
                if (isPassive)
                    elem->locked = true;
                *instance = elem;
            }
        }
    }
    return ret;
}

/* HandleArrayedSliceChannel : resolve a arrayed access on a arrayed channel into another arrayed
   channel with elements as per the indices over `range'.  Remain quiet if an error was reported `upstream',
   *instance will contain on exit the same things as in HandleIdentChannel. */
ExprAttributes
HandleArrayedSliceChannel (PtrContext context, ExprAttributes lhsAttr,
  PtrInstance lhsInstance, Span range, PtrInstance * instance, bool isInput, bool isPassive, tPosition position)
{
    ExprAttributes ret = NoExprAttributes;
    Span lhsRange = (lhsAttr.value.type->nature == ArrayedType ? lhsAttr.value.type->info.arrayed.range : NoSpan);

    *instance = NULL;

    if (!lhsInstance)
    {                           /* Do nowt */
    }
    /* Bow out now if previous Channel failed */
    else if (lhsAttr.value.type->nature != ArrayedType)
        LOG_ERROR (ExpectingAnArrayedChannel, NoIdent, position);
    else if (!SubscriptTypeIsOK (lhsRange.boundingType, range.boundingType))
        LOG_ERROR (SubscriptHasWrongType, NoIdent, position);
    else if (!range.lowerBound || !range.upperBound) /* Non-const */
    {
        LOG_ERROR (CantDoNonConstArrayedChannelIndex, NoIdent, position);
    } else
        /* Constant index */
    {
        if (mpz_cmp (range.lowerBound, lhsRange.lowerBound) < 0 || mpz_cmp (range.upperBound, lhsRange.upperBound) > 0)
            LOG_ERROR (SubscriptOutOfRange, NoIdent, position);
        else
        {
            unsigned elementOffset;
            unsigned elementCount, elementCountIter;
            PtrInstanceList elements;
            PtrInstanceList newElementsHead = NULL;
            PtrInstanceList newElementsTail = NULL;

            /* Find the start of the element list and the number of elements to pluck */
            {
                PtrMP_INT tmp = NewMP_INT (0);

                mpz_sub (tmp, range.lowerBound, lhsAttr.value.type->info.arrayed.range.lowerBound);
                elementOffset = mpz_get_ui (tmp);
                elements = NthCellOfInstanceList (lhsAttr.value.type->info.arrayed.arrayedElements, elementOffset);
                mpz_sub (tmp, range.upperBound, range.lowerBound);
                elementCount = mpz_get_ui (tmp) + 1;
                DeleteMP_INT (tmp);
            }
            elementCountIter = elementCount;

            /* Make a new list of elements/check each element for locking */
            /* FOR_EACH (elements, elementCountIter > 0, ...) */
            while (elements && elementCountIter > 0)
            {
                PtrInstance elem = CAR (elements);

                /* Check that element isn't locked */
                if (elem->locked && isPassive) /* Can't lock a channel twice */
                {
                    LOG_ERROR (ChannelAlreadyLocked, elem->ident, position);
                    break;
                }
                if (elem->locked)
                {
                    /* Don't allow access to locked channels */
                    LOG_ERROR (CannotAccessLockedChannel, elem->ident, position);
                    break;
                } else
                {
                    PtrInstanceList newElem = NewInstanceList (elem, NULL);

                    if (newElementsTail)
                    {
                        newElementsTail->next = newElem;
                        newElementsTail = CDR (newElementsTail);
                    } else
                        newElementsHead = newElementsTail = newElem;
                }
                elementCountIter--;
                elements = CDR (elements);
            }
            if (elementCountIter == 0) /* Made a valid set of instances.  Need to make a new top instance and type */
            {
                PtrType newArrayedType = NewElementInitialisedArrayedType (MarkerIdent,
                  lhsInstance->type->info.arrayed.baseType,
                  newElementsHead,
                  elementCount);

                /* Handle the return stuff */
                ret.value.type = newArrayedType;
                *instance = NewChannelInstance (lhsInstance->ident, newArrayedType);
                (*instance)->nature = lhsInstance->nature;
                (*instance)->info.port = lhsInstance->info.port;
            }
        }
    }
    return ret;
}

/* HandleArrayedAppendChannels : produce a single arrayed channel lvalue from a pair of others.
   Remain quiet if an error was reported `upstream', *instance will contain on exit the same things
   as in HandleIdentChannel. */
ExprAttributes
HandleArrayedAppendChannels (PtrContext context, ExprAttributes lhsAttr,
  PtrInstance lhsInstance, ExprAttributes rhsAttr, PtrInstance rhsInstance, PtrInstance * instance, bool isInput, bool isPassive, tPosition position)
{
    ExprAttributes ret = NoExprAttributes;

    *instance = NULL;

    if (!lhsInstance || !rhsInstance)
    {                           /* Do nowt */
    }
    /* Bow out now if previous Channel failed */
    else if (lhsAttr.value.type->nature != ArrayedType || rhsAttr.value.type->nature != ArrayedType)
        LOG_ERROR (ExpectingAnArrayedChannel, NoIdent, position);
    else if (!TypeEquivalence (lhsAttr.value.type->info.arrayed.baseType, rhsAttr.value.type->info.arrayed.baseType))
        LOG_ERROR (ExpressionsMustHaveSameType, NoIdent, position);
    else
    {
        unsigned elementCountIter;
        PtrInstanceList elements;
        PtrInstanceList newElementsHead = NULL;
        PtrInstanceList newElementsTail = NULL;
        int lNr;

        for (lNr = 1; lNr >= 0; lNr--) /* Bodge to get around code replication */
        {
            elementCountIter = (lNr ? lhsAttr.value.type->info.arrayed.elementCount : rhsAttr.value.type->info.arrayed.elementCount);
            elements = (lNr ? lhsAttr.value.type->info.arrayed.arrayedElements : rhsAttr.value.type->info.arrayed.arrayedElements);

            /* Make a new list of LHS elements/check each element for locking */
            /* FOR_EACH (elements, elementCountIter > 0, ...) */
            while (elements && elementCountIter > 0)
            {
                PtrInstance elem = CAR (elements);
                PtrInstanceList newElem = NewInstanceList (elem, NULL);

                if (newElementsTail)
                {
                    newElementsTail->next = newElem;
                    newElementsTail = CDR (newElementsTail);
                } else
                    newElementsHead = newElementsTail = newElem;
                elementCountIter--;
                elements = CDR (elements);
            }
            if (elementCountIter != 0)
                break;          /* Quit on error */
        }

        if (elementCountIter == 0) /* Do the RHS */
        {
            PtrType newArrayedType = NewElementInitialisedArrayedType (MarkerIdent,
              lhsInstance->type->info.arrayed.baseType, newElementsHead,
              lhsAttr.value.type->info.arrayed.elementCount + rhsAttr.value.type->info.arrayed.elementCount);

            /* Handle the return stuff */
            ret.value.type = newArrayedType;
            *instance = NewChannelInstance (lhsInstance->ident, newArrayedType);
            (*instance)->nature = lhsInstance->nature;
            (*instance)->info.port = lhsInstance->info.port;
        }
    }
    return ret;
}

/* HandleArrayedConsChannels : produce a single arrayed channel lvalue from a list of attributes
   corresponding to accesses on individual channels.  Remain quiet if an error was reported `upstream',
   *instance will contain on exit the same things as in HandleIdentChannel. */
ExprAttributes
HandleArrayedConsChannels (PtrContext context, PtrType baseType,
  PtrExprAttributesList attrs, PtrInstance * instance, bool isInput, bool isPassive, tPosition position)
{
    ExprAttributes ret = NoExprAttributes;
    PtrInstance sampleInstance = CAR (CAR (attrs).accesses)->inst;
    unsigned elementCount = LengthOfExprAttributesList (attrs);

    *instance = NULL;

    if (!attrs)
    {                           /* Do nowt */
    } else
    {
        PtrInstanceList newElementsHead = NULL;
        PtrInstanceList newElementsTail = NULL;

        /* Assume that the first access in each access list is the one to the instance */
        while (attrs)
        {
            PtrInstanceList newElem = NewInstanceList (CAR (CAR (attrs).accesses)->inst, NULL);

            if (newElementsTail)
            {
                newElementsTail->next = newElem;
                newElementsTail = CDR (newElementsTail);
            } else
                newElementsHead = newElementsTail = newElem;
            attrs = CDR (attrs);
        }

        {
            PtrType newArrayedType = NewElementInitialisedArrayedType (MarkerIdent,
              baseType, newElementsHead,
              elementCount);

            /* Handle the return stuff */
            ret.value.type = newArrayedType;
            *instance = NewChannelInstance (sampleInstance->ident, newArrayedType);
            (*instance)->nature = sampleInstance->nature;
            (*instance)->info.port = sampleInstance->info.port;
        }
    }
    return ret;
}

/* FlattenDisjointChannelAccessList : reduce an ExprAttributesList containing passive channel accesses
   (test this, and that there is no overlap in accesses) into a single ExprAttributes value.
   NB. this will affect the wires and accesses elements of the return value */
ExprAttributes FlattenDisjointChannelAccessList (PtrExprAttributesList attrs, tPosition position)
{
    ExprAttributes ret = NoExprAttributes;

    /* FOR_EACH (attrs, true, ...) */
    while (attrs)
    {
        if (CAR (attrs).activation) /* Something went wrong upstream, ignore */
        {
            ret.accesses =
              ParallelCombineAccesses (ret.accesses, CAR (attrs).accesses, &CAR (attrs).components, &CAR (attrs).wires, position, NULL, false);
            ret.wires = NewWireList (CAR (attrs).activation, ret.wires);
        }
        attrs = CDR (attrs);
    }
    return ret;
}
