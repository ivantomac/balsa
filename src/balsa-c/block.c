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

	`block.c'
	Functions for resolving local accesses and
	declarations into components and errors
	
 */

#include "block.h"
#include "flags.h"
#include "Errors.h"
#include <string.h>

/* CreateVariablePartitions : create an array of length `length' + 1 with 1 in all
   the places one element after the end of a partition, starting from 0
   a partition is the longest run of elements which don't contain a 1 in
   any place other than the first element eg. 10010000010001 is 3 runs of
   3 6 and 4 bits, NB the start and end of partitions are marked */
Ptrchar CreateVariablePartitions (PtrWireList accesses, Bits length)
{
    Ptrchar partitions = NEW_ARRAY (char, length + 1);
    int tmp;

    /* Blank the array */
    for (tmp = 0; tmp <= length; tmp++)
        partitions[tmp] = 0;

    while (accesses)
    {
        if (VariableReadSplit || !CAR (accesses)->isPull /* Write */  || !CAR (accesses)->activeComponent)
        {
            partitions[CAR (accesses)->offset + CAR (accesses)->width] = 1;
            partitions[CAR (accesses)->offset] = 1;
        }
        accesses = CDR (accesses);
    }
    return partitions;
}

/* CreateBuiltinElementPartitions : place partition boundaries at each builtin element's
	offset and offset + width to mark places where builtin Variables should be used.
	Use value 2 to mark upper partition indicator so that the Variable component
	generator can place a BuiltinVariable component instead of a normal Variable component */
void CreateBuiltinElementPartitions (Ptrchar partitions, PtrType type)
{
    switch (type->nature)
    {
    case BuiltinType:          /* Mark pointer start and end */
        partitions[0] = 1;
        partitions[64] = 2;
        break;
    case RecordType:           /* Visit each element and make choice */
        {
            InstanceList *elements = type->info.record.recordElements;

            while (elements)
            {
                PtrType subType = CAR (elements)->type;

                if (subType->hasBuiltinElements)
                    CreateBuiltinElementPartitions (partitions, subType);
                partitions += Abs (subType->size);
                elements = CDR (elements);
            }
        }
        break;
    case ArrayType:            /* hasBuiltinElements only if then baseType has */
        {
            int elementCount = 0;
            unsigned stride = Abs (type->info.array.baseType->size);

            for (; elementCount < type->info.array.elementCount; elementCount++)
            {
                CreateBuiltinElementPartitions (partitions, type->info.array.baseType);
                partitions += stride;
            }
        }
        break;
    default:
        break;
    }
}

/* MakeBitExtractedName : make a name from the given tIdent, offset and size of the form:
   name[offset..offset+size-1] */
Ptrchar MakeBitExtractedName (tIdent name, Bits offset, Bits size)
{
    const char *nameStr = PeekString (name);
    unsigned extractedNameSize = strlen (nameStr);
    char *rName = NEW_ARRAY (char,
      extractedNameSize + 24 /* Padding for the digits of offset and size */ );

    sprintf (rName, "%s[%d..%d]", nameStr, offset, offset + size - 1);

    return rName;
}

/* CreatePartitionedVariableComponents : Create a component list of parts of a variable from
   a given 'partition' (from the function CreateVariablePartitions), 
   each component entry will have the offset and width of the corresponding (working
   upwards from 0) variable part. If `makeFalseVariables' is true then produce FVs
   instead of real variables.  If `makeEagerFalseVariables' is true then produce Passive or
   Active EagerFVs instead of normal FVs, depending on eagerFVsAreActive.
   If `initialisedValue' is not NULL and we're not generating
   FVs then create InitVariable components with appropriate initValues. */
PtrComponentList
CreatePartitionedVariableComponents (tIdent name, bool * partition, Bits partitionLength, bool makeFalseVariables, bool makeEagerFalseVariables,
  bool eagerFVsAreActive, PtrMP_INT initialisedValue, tPosition position)
{
    /* Tacking new bundles onto end of list for convenience */
    PtrComponentList parts = NULL;
    PtrComponentList tail = NULL;
    Bits ctr;

    /* offset holds the position of the last 'true' */
    Bits offset = -1;

    for (ctr = 0; ctr <= partitionLength; ctr++)
    {
        if (partition[ctr] != 0)
        {
            if (offset != -1)   /* Skip first 1, this marks the least significant written bit */
            {                   /* Actual partitions, Defer placement of variable wires */
                Ptrchar fullName = (name == NoIdent ? "" : MakeBitExtractedName (name, offset, ctr - offset));
                PtrComponent newPart;
                PtrComponentList newLink;

                if (makeFalseVariables)
                {
                    if (makeEagerFalseVariables)
                        if (eagerFVsAreActive)
                            newPart = NewActiveEagerFalseVariableComponent (position, offset, ctr - offset, NULL, NULL, NULL, NULL);
                        else
                            newPart = NewPassiveEagerFalseVariableComponent (position, offset, ctr - offset, NULL, NULL, NULL, NULL);
                    else
                        newPart = NewFalseVariableComponent (position, offset, ctr - offset, NULL, NULL, NULL);
                } else
                {
                    if (initialisedValue)
                        newPart =
                          NewInitVariableComponent (position, fullName, offset, ctr - offset, FilterRangeFromMP_INT (initialisedValue, ctr - offset, offset),
                          NULL, NULL, NULL);
                    else
                    {
                        newPart = NewVariableComponent (position, fullName, offset, ctr - offset, NULL, NULL);
                        /* Change Variable to BuiltinVariable if necessary */
                        if (partition[ctr] == 2)
                            newPart->nature = BuiltinVariableComponent;
                    }
                    newPart->position = position;
                }

                newLink = NewComponentList (newPart, NULL);
                /* Add to end of list */
                CONS (tail, newLink);
                tail = newLink;
                if (!parts)
                    parts = tail;
            }
            offset = ctr;
        }
    }
    return parts;
}

/* CreateWriteConnections : Add variable write components to comps, update bundleCount and
   fill in the write port field of the writeParts */
void CreateWriteConnections (PtrComponentList writeParts, PtrWireList accesses, PtrWireList * wires, PtrComponentList * comps, tPosition position)
{
    while (accesses)
    {
        PtrComponentList parts = writeParts;
        Bits widthRemaining = CAR (accesses)->width;

        if (!CAR (accesses)->isPull /* Write */  || !CAR (accesses)->activeComponent)
        {
            PtrWire splitWire = CAR (accesses); /* Remainder of the write wire */

            /* Skip parts which are not used */
            while (parts && CAR (parts)->param.variable.offset < CAR (accesses)->offset)
            {
                parts = CDR (parts);
            }
            /* While we are in the range of our bundle */
            while (parts && widthRemaining != 0)
            {
                PtrWireList writePortPtr;

                if ((CAR (parts)->nature == PassiveEagerFalseVariableComponent) || (CAR (parts)->nature == ActiveEagerFalseVariableComponent))
                    writePortPtr = CDR (CAR (parts)->ports);
                else
                    writePortPtr = CAR (parts)->ports;

                if (CAR (parts)->param.variable.width != widthRemaining) /* Need to split */
                {
                    /* Create a wire for the connection to the write port (writeWire),
                       place rest of write in splitWire */
                    PtrWire writeWire = NewWire (NextBundleNumber (*wires),
                      CAR (parts)->param.variable.width,
                      NoType,
                      splitWire->offset /* Not really needed */ ,
                      false /* Push */ , position);
                    PtrWire newSplitWire = NewWire (NextBundleNumber (*wires) + 1,
                      widthRemaining - CAR (parts)->param.variable.width,
                      NoType,
                      splitWire->offset + (widthRemaining - CAR (parts)->param.variable.width), false /* Push */ ,
                      position);

                    *comps = NewComponentList (NewSplitComponent (position, splitWire /* In */ ,
                        writeWire /* LSW */ ,
                        newSplitWire /* MSW */ ),
                      *comps);
                    *wires = NewWireList (newSplitWire, NewWireList (writeWire, *wires));
                    splitWire = newSplitWire; /* Put the split remains into the splitWire */
                    if (CAR (writePortPtr)) /* This is the 2nd ... 3rd connection on the write port, need to mux */
                    {
                        PtrWire multiplexedWriteWire = CopyAndRenumberWire (writeWire,
                          NextBundleNumber (*wires),
                          position);

                        *comps = NewComponentList (NewCallMuxComponent (position, writeWire, CAR (writePortPtr), multiplexedWriteWire), *comps);
                        *wires = NewWireList (multiplexedWriteWire, *wires);
                        CAR (writePortPtr) = multiplexedWriteWire;
                    } else
                        CAR (writePortPtr) = writeWire;
                } else if (CAR (writePortPtr)) /* Don't need to split but need to Mux (already have a write connection on CAR(parts)) */
                {
                    PtrWire multiplexedWriteWire = CopyAndRenumberWire (splitWire, NextBundleNumber (*wires),
                      position);

                    multiplexedWriteWire->position = CAR (parts)->position;

                    *comps = NewComponentList (NewCallMuxComponent (position, splitWire, CAR (writePortPtr), multiplexedWriteWire), *comps);
                    *wires = NewWireList (multiplexedWriteWire, *wires);
                    CAR (writePortPtr) = multiplexedWriteWire;
                } else
                    CAR (writePortPtr) = splitWire; /* Don't need to split or Mux */

                /* Update write wire's component knowledge */
                if (CAR (parts)->nature == PassiveEagerFalseVariableComponent)
                    UpdateWire (CAR (writePortPtr), false /* Passive */ , CAR (parts), 1);
                else if (CAR (parts)->nature == ActiveEagerFalseVariableComponent)
                {
                    PtrWire w = CAR (writePortPtr);

                    if (w->activeComponent)
                    {           // if already has an active component, insert a passivator
                        PtrWire newPassivatorWire = CopyAndRenumberWire (w,
                          NextBundleNumber (*wires),
                          position);

                        *comps = NewComponentList (NewPassivatorPushComponent (position, newPassivatorWire, w), *comps);
                        *wires = NewWireList (newPassivatorWire, *wires);

                        CADR (CAR (parts)->ports) = newPassivatorWire;
                        UpdateWire (newPassivatorWire, true /* Active */ , CAR (parts), 1);
                    } else
                        UpdateWire (CAR (writePortPtr), true /* Active */ , CAR (parts), 1);
                } else
                    UpdateWire (CAR (writePortPtr), false /* Passive */ , CAR (parts), 0);
                widthRemaining -= CAR (parts)->param.variable.width;
                parts = CDR (parts);
            }
        }
        accesses = CDR (accesses);
    }
}

/* CreateReadConnections : Add variable read components to comps, update bundleCount and
   fill in the read ports fields of the variables. */
void CreateReadConnections (PtrComponentList readParts, PtrWireList accesses, PtrWireList * wires, PtrComponentList * comps, tPosition position)
{
#define WIRE_NO(w) ((w) ? (w)->bundleNo : -1)
    while (accesses)
    {
        if (CAR (accesses)->isPull /* Read */  && CAR (accesses)->activeComponent)
        {
            PtrComponentList parts = readParts;
            int widthRemaining = CAR (accesses)->width;

            Bits offsetInFirstPart;
            Bits fullWidth = 0;
            PtrWire combineWire = NULL;
            PtrComponent firstComp = NULL;
            PtrWireList singleReadConnection = NULL; /* Points to the read port of a variable */

            /* Skip parts which are not used */
            while (parts && CAR (parts)->param.variable.offset + CAR (parts)->param.variable.width <= CAR (accesses)->offset)
            {
                parts = CDR (parts);
            }

            offsetInFirstPart = CAR (accesses)->offset - CAR (parts)->param.variable.offset;
            widthRemaining += offsetInFirstPart; /* Count the leading space as part of this access */
            /* Adjust the widthRemaining to make it a correspond to a complete set of parts */
            {
                PtrComponentList partsIter = parts;
                int widthRemainingIter = widthRemaining;

                while (partsIter && widthRemainingIter > 0)
                {
                    widthRemainingIter -= CAR (partsIter)->param.variable.width;
                    partsIter = CDR (partsIter);
                }
                /* widthRemainingIter should now be negative to the degree of the number of superfluous
                   bits in the last part that should be included in the Combine tree */
                widthRemaining -= widthRemainingIter;
            }
            /* While we are in the range of our bundle */
            while (parts && widthRemaining > 0)
            {
                if (widthRemaining > CAR (parts)->param.variable.width) /* Need to combine */
                {
                    /* Need to combine combineAccess and a variable read port to make part of this read access */
                    PtrWire newReadWire = NewWire (NextBundleNumber (*wires),
                      CAR (parts)->param.variable.width,
                      NoType,
                      CAR (parts)->param.variable.offset,
                      true /* Pull */ , position);
                    PtrWire newCombineWire = NewWire (NextBundleNumber (*wires) + 1,
                      widthRemaining - CAR (parts)->param.variable.width, NoType,
                      CAR (parts)->param.variable.offset + CAR (parts)->param.variable.width,
                      true /* Pull */ , position);
                    PtrComponent comp = NewCombineComponent (position, combineWire, /* NB. for first combine component this will be NULL, fixed below */
                      newReadWire, newCombineWire); /* FIXME, check the ordering */

                    *comps = NewComponentList (comp, *comps);
                    *wires = NewWireList (newCombineWire, NewWireList (newReadWire, *wires));
                    if (!firstComp)
                        firstComp = comp;
                    combineWire = newCombineWire;
                    AppendWireLists (CAR (parts)->ports, NewWireList (newReadWire, NULL));
                } else
                {
                    PtrWireList newLink = NewWireList (combineWire, NULL);

                    AppendWireLists (CAR (parts)->ports, newLink);
                    if (!firstComp)
                    {
                        firstComp = CAR (parts);
                        singleReadConnection = newLink;
                    }
                }
                fullWidth += CAR (parts)->param.variable.width; /* Add the width of the var read */
                widthRemaining -= CAR (parts)->param.variable.width;
                parts = CDR (parts);
            }

            /* Don't need a filter */
            if (fullWidth == CAR (accesses)->width)
            {
                if (!singleReadConnection) /* At least one combine */
                {
                    CAR (firstComp->ports) = CAR (accesses); /* connect access to combine read port */
                    UpdateWire (CAR (accesses), false /* passive */ , firstComp, 0);
                } else          /* FIXME, need to consider an update here */
                {
                    CAR (singleReadConnection) = CAR (accesses); /* connect to a variable read port */
                    UpdateWire (CAR (accesses), false /* passive */ , firstComp, LengthOfWireList (firstComp->ports) - 1);
                }
            } else              /* Filter the result */
            {
                PtrWire newFilterInput = NewWire (NextBundleNumber (*wires), fullWidth, NoType,
                  CAR (accesses)->offset + offsetInFirstPart,
                  true /* Pull */ , position);

                *comps = NewComponentList (NewSliceComponent (position, offsetInFirstPart, CAR (accesses) /* Out */ , newFilterInput), *comps);
                /* Added HELPHELP */
                *wires = NewWireList (newFilterInput, *wires);
                if (!singleReadConnection) /* At least one combine */
                {
                    CAR (firstComp->ports) = newFilterInput; /* connect access to combine read port */
                    UpdateWire (CAR (firstComp->ports), false /* passive */ ,
                      firstComp, 0);
                } else
                {
                    CAR (singleReadConnection) = newFilterInput; /* connect to a variable read port */
                    UpdateWire (newFilterInput, false /* passive */ , firstComp,
                      LengthOfWireList (firstComp->ports) - 1);
                }
            }
        }
        accesses = CDR (accesses);
    }
}

/* TrimDanglingVariables: convert variables in the given component list
   into push continue components if no read ports are present, eliminate
   Variables with no ports at all. */
PtrComponentList TrimDanglingVariables (PtrComponentList writeParts)
{
    PtrComponentList prev = NULL;
    PtrComponentList ret = writeParts;

    while (writeParts)
    {
        if (CAR (writeParts)->nature == FalseVariableComponent)
        {                       /* FalseVariable -> ContinuePush */
            if (!CDR (CDR (CAR (writeParts)->ports)))
            {
                CAR (writeParts)->nature = ContinuePushComponent;
                CAR (writeParts)->ports->next = NULL;
            }
            prev = writeParts;
        } else if (CAR (writeParts)->nature == PassiveEagerFalseVariableComponent)
        {                       /* PassiveEagerFalseVariable -> ?? */
            if (!CDR (CDR (CAR (writeParts)->ports)))
            {
                g_assert (0);
                CAR (writeParts)->nature = ContinuePushComponent;
                CAR (writeParts)->ports->next = NULL;
            }
            prev = writeParts;
        } else if (CAR (writeParts)->nature == ActiveEagerFalseVariableComponent)
        {                       /* ActiveEagerFalseVariable -> ?? */
            if (!CDR (CDR (CAR (writeParts)->ports)))
            {
                g_assert (0);
                CAR (writeParts)->nature = ContinuePushComponent;
                CAR (writeParts)->ports->next = NULL;
            }
            prev = writeParts;
        } else
        {
            if (!CAR (writeParts)->ports || !CAR (CAR (writeParts)->ports)) /* No ports, or NULL write, trim */
            {
                if (prev)       /* Already seen at least one port */
                    prev->next = CDR (writeParts);
                else
                    ret = CDR (ret);
            } else if (!CDR (CAR (writeParts)->ports)) /* Only a write port */
            {
                CAR (writeParts)->nature = ContinuePushComponent;
                CAR (writeParts)->ports->next = NULL;
                prev = writeParts;
            } else
                prev = writeParts;
        }

        writeParts = CDR (writeParts);
    }
    return ret;
}

/* ResolveLocalSharedCallAccesses : resolve the connections to shared blocks, place
   the shared block code and connect up activations.
   NB. this function traverses the context.procedures to detect used shared blocks (those
   having non NULL unplacedActivations) */
PtrAccessList
ResolveLocalSharedCallAccesses (PtrContext context, PtrWireList * wires, PtrComponentList * comps, PtrAccessList accesses, tPosition position)
{
    PtrProcedureList procs = context->procedures;
    PtrAccessList ret = accesses;

    while (procs && CAR (procs)->nature != ContextMarkerProcedure)
    {                           /* Only worry about shared blocks */
        if (CAR (procs)->nature == SharedProcedure && CAR (procs)->info.procedure.attributes.activation)
        {
            if (!CAR (procs)->unplacedActivation) /* Shared block not used */
                LOG_ERROR (SharedBlockNotUsed, CAR (procs)->ident, position);
            else
            {
                /* Place accesses, NB this should produce NO errors */
                unsigned preAccessesErrorCount = ErrorCount;

                ret = SharedCombineAccesses (ret, CAR (procs)->info.procedure.attributes.accesses, comps, wires, position);
                ASSERT (ErrorCount == preAccessesErrorCount);
                FilterOutAccessToInstance (&ret, CAR (procs)->dummyInstance);

                /* Combine components */
                *comps = AppendComponentLists (CopyComponentList (CAR (procs)->info.procedure.attributes.components), *comps);

                /* Need to run this in 2002-10-02 AB */
#if 0
                /* Change activation, remove the calling end wire */
                if (CAR (procs)->unplacedActivation->activeComponent)
                {
                    PtrComponent callEndComponent = CAR (procs)->unplacedActivation->activeComponent;
                    unsigned callEndPortNumber = CAR (procs)->unplacedActivation->activePort;
                    PtrWireList callEndComponentWires = callEndComponent->ports;

                    /* Remove the call end wire from its wire list */
                    if (*wires)
                        RemoveWireFromWireList (CAR (procs)->unplacedActivation, wires);
                    /* Change the component, port number of the activations ACTIVE connection to that of the call end component */
                    CAR (procs)->info.procedure.attributes.activation->activeComponent = callEndComponent;
                    CAR (procs)->info.procedure.attributes.activation->activePort = callEndPortNumber;
                    /* Change call end components wire */
                    while (callEndComponentWires && callEndPortNumber)
                    {
                        callEndPortNumber--;
                        callEndComponentWires = CDR (callEndComponentWires);
                    }
                    /* CAR (callEndComponentWires) is the wire connected to activation (or should be) */
                    callEndComponentWires->body = CAR (procs)->info.procedure.attributes.activation;
                }
#endif
                /* Insert a 1 ported Fork here and let optimisation pick up the slack */
                *comps =
                  NewComponentList (NewConnectComponent (position, CAR (procs)->unplacedActivation, CAR (procs)->info.procedure.attributes.activation), *comps);
                /* OK, combine wires */
                *wires = AppendWireLists (CopyWireList (CAR (procs)->info.procedure.attributes.wires), *wires);
                RenumberWireList (*wires, false);
            }
        }
        procs = CDR (procs);
    }
    return ret;
}

/* ResolveLocalInstanceAccesses : place components for local variable/channel accesses.
   places split/combine components for multi variable (variable part) reads.
   If *`initWire' is set then some initialisation was required by the components in this
   access list and the wires left in `initWires' are the activations for that initialisation */
PtrAccessList
ResolveLocalInstanceAccesses (PtrContext context,
  PtrWireList * wires, PtrComponentList * comps, PtrAccessList accesses, PtrWireList * initWires, tPosition position)
{
    PtrInstance inst;

    if (!accesses)
        return NULL;            /* No local accesses */

    inst = CAR (accesses)->inst;
    /* Non local access - prepend this bundle set element to the front of the result */
    if (!InstanceIsLocal (inst, context->instances))
    {
        return NewAccessList (CAR (accesses), ResolveLocalInstanceAccesses (context, wires, comps, CDR (accesses), initWires, position));
    } else
    {
        switch (inst->nature)
        {
        case OutputChannelInstance:
        case InputChannelInstance: /* This must be present otherwise we wouldn't have got here */
            /* Leave it for the next level up to resolve (procedure level) */
            return NewAccessList (CAR (accesses), ResolveLocalInstanceAccesses (context, wires, comps, CDR (accesses), initWires, position));
            break;
        case ChannelInstance:
            /* Local channel */
            if (CAR (accesses)->info.channel.functioningChannel)
                /* Place a run component and carry on */
                *comps =
                  NewComponentList ((CAR (accesses)->info.channel.wire->width ==
                    0 ? NewContinueComponent : NewContinuePushComponent) (position, CAR (accesses)->info.channel.wire), *comps);
            else if (CAR (accesses)->inst->type->nature == SyncType)
                LOG_ERROR (SyncIsNotFunctional, inst->ident, position);
            else
                LOG_ERROR (ChannelIsNotFunctional, inst->ident, position);
            break;
        case VariableInstance:
            {
                /* Check variable accesses first, complain about non overlapping accesses,
                   if we have a record type then reduce the read/write Maps to exclude
                   the pad field */
                mpz_t readMap, writeMap;
                bool mismatchedAccesses, unusedWrites;

                mpz_init_set (readMap, CAR (accesses)->info.variable.readMap);
                mpz_init_set (writeMap, CAR (accesses)->info.variable.writeMap);

                if (inst->type->nature == RecordType)
                {
                    PtrMP_INT mask = MakeMaskForRange (inst->type->info.record.unpaddedLength,
                      0);

                    mpz_and (readMap, readMap, mask);
                    mpz_and (writeMap, writeMap, mask);
                    DeleteMP_INT (mask);
                }
                /* mismatchedAccesses: reads don't exactly match writes */
                /* unusedWrites: variable sections are written to and not read */
                mismatchedAccesses = mpz_cmp (readMap, writeMap) != 0;

                mpz_com (readMap, readMap); /* not read */
                mpz_and (writeMap, writeMap, readMap); /* unusedWrites : written and not read */
                unusedWrites = mpz_sgn (writeMap) != 0;

                if (mismatchedAccesses && !unusedWrites)
                    LOG_ERROR (MismatchedVariableAccesses, inst->ident, position);
                else
                    /* noVariableReads or balanced access */
                {
                    Bits width = Abs (inst->type->size);
                    Ptrchar partitions = CreateVariablePartitions (CAR (accesses)->info.variable.wires, width);
                    PtrComponentList varIter, trimmedVariables;
                    PtrComponentList variables;

                    if (inst->type->hasBuiltinElements)
                        CreateBuiltinElementPartitions (partitions, inst->type);

                    variables =
                      CreatePartitionedVariableComponents (inst->ident, partitions, width, false, false, false, inst->info.variable.initialisedValue,
                      inst->info.variable.position);

                    CreateWriteConnections (variables, CAR (accesses)->info.variable.wires, wires, comps, position);
                    CreateReadConnections (variables, CAR (accesses)->info.variable.wires, wires, comps, position);
                    trimmedVariables = TrimDanglingVariables (variables);

                    /* Find the initialisation activation wires required for the new variables */
                    varIter = trimmedVariables;
                    while (varIter)
                    {
                        if (CAR (varIter)->nature == InitVariableComponent)
                        {
                        	/* Think about making the accessList in this wire more sensible */
                            PtrWire initActWire = NewSyncWire (1, position);

                            CADR (CAR (varIter)->ports) = initActWire;
                            UpdateWire (initActWire, false /* Passive */ , CAR (varIter), 1);
                            *wires = NewWireList (initActWire, *wires);

                            if (initWires)
                                *initWires = NewWireList (initActWire, *initWires);
                        }
                        varIter = CDR (varIter);
                    }

                    *comps = AppendComponentLists (trimmedVariables, *comps);
                }

                mpz_clear (readMap);
                mpz_clear (writeMap);
            }
            break;
        case SharedCallInstance:
            /* Skip, these are handled by the ResolveSharedCall... function */
            break;
        case ValInstance:
            {
            	PtrAccessList newAccesses;

                /* Combine components */
                *comps = AppendComponentLists (CopyComponentList (inst->info.val.components), *comps);

                /* Insert a 1 ported ForkPush here and let optimisation pick up the slack */
                *comps = NewComponentList (NewSynchPullConnectComponent (position, CAR (accesses)->info.val.wire,
                  	inst->info.val.activation), *comps);
                /* OK, combine wires */
                *wires = AppendWireLists (CopyWireList (inst->info.val.wires), *wires);

            	newAccesses = ParallelCombineAccesses (CDR (accesses), inst->info.val.accesses,
            		comps, wires, position, NULL, false);
                RenumberWireList (*wires, false);

				return ResolveLocalInstanceAccesses (context, wires, comps, newAccesses, initWires, position);
            }
            break;
        default:
            break;
        }
        return ResolveLocalInstanceAccesses (context, wires, comps, CDR (accesses), initWires, position);
    }

    return NULL;                /* For the compiler */
}

/* SequenceBlockInitialisationAndCommand : add the control components to sequence
	block command and initialisation operations without resolving instance accesses.
	Returns the new activation channel for the block. */
PtrWire
SequenceBlockInitialisationAndCommand (PtrWire commandActivation,
  PtrWireList * wires, PtrComponentList * comps, PtrWireList initWires, tPosition position)
{
    PtrWire initAct = NewSyncWire (1, position);
    PtrWire activation = NewSyncWire (1, position);
    PtrComponent initFork = NewForkComponentWithWireList (position, initAct, initWires);
    PtrComponent initSeq = NewSequenceComponent (position, activation, initAct, commandActivation);

    *wires = NewWireList (initAct, NewWireList (activation, *wires));
    *comps = NewComponentList (initSeq, NewComponentList (initFork, *comps));

    return activation;
}
