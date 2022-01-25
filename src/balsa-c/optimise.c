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

	`optimise.c'
	Handshake circuit level peephole optimisation
	
 */

#include <string.h>
#include "optimise.h"
#include "flags.h"

/* FindRootComponent : find the root component of a tree of like components,
   component port at index portNo is the `common' port leading towards the 
   root component. portNo can be negative and as such indicates the -portNo'th
   port from the end of the port list.
   activePort indicates the nature (active,/passive) of the common port */
PtrComponent FindRootComponent (PtrComponent comp, int portNo, bool activePort)
{
    PtrComponent currentBase = comp; /* Running base component */
    bool atBase = false;        /* Avoids need for break in while loop */

    while (!atBase)
    {
        PtrWire commonWire = NthOfWireList (currentBase->ports, portNo);
        PtrComponent commonConnectedComponent;

        commonConnectedComponent = GetComponentFromWire (commonWire, !activePort);

        /* not a port and same type of component */
        if (commonConnectedComponent && commonConnectedComponent->nature == comp->nature)
            currentBase = commonConnectedComponent;
        else
            atBase = true;
    }
    return currentBase;
}

/* GatherWiresFromComponentTree : gather the connected wires (except the ones at index commonPortNo
   (which can be -ve, with the usual meaning (see. NthOfWireList))) by a depth first L-R traversal
   of the components of the same nature as the `base' component. Marks all components encountered
   (except `base') for sweeping, marks all common wires similarly (except for the top level
   common wire if `removeCommonWire' is false. */
PtrWireList GatherWiresFromComponentTree (PtrComponent base, int commonPortNo, bool commonActive, bool removeCommonWire, PtrWire replacementWire)
{
    /* Which port ? */
    unsigned thisCommonPortNo = (commonPortNo < 0 ? LengthOfWireList (base->ports) + commonPortNo : commonPortNo);
    unsigned portNo = 0;
    PtrWireList portsIn = base->ports;
    PtrWireList portsOut = NULL;
    PtrWire wireToRemove = NULL;

    /* Visit each existing port, make a new list of compound ports */
    /* FOR_EACH (portsIn, true, ...) */
    while (portsIn)
    {
        if (portNo != thisCommonPortNo) /* Add to list */
        {
            PtrComponent nextComponentUp = GetComponentFromWire (CAR (portsIn), commonActive);

            /* if the next component up is the same type as this */
            if (nextComponentUp && nextComponentUp->nature == base->nature)
            {
                portsOut = AppendWireLists (portsOut,
                  GatherWiresFromComponentTree (nextComponentUp, commonPortNo, commonActive, true, replacementWire));
                MarkComponent (nextComponentUp);
            }
            /* Single wire, connected to a different component flavour */
            else
                portsOut = AppendWireLists (portsOut, NewWireList (CAR (portsIn), NULL));
        } else /* mark this redundent common wire for removal */ if (removeCommonWire)
        {
            MarkWire (CAR (portsIn));
            wireToRemove = CAR (portsIn);
        }
        portNo++;
        portsIn = CDR (portsIn);
    }
    if (removeCommonWire && wireToRemove)
        MarkWireWithReplacement (wireToRemove, replacementWire);
    return portsOut;
}

/* TreeContraction : contract a tree of components with a common root wire (which can be
   reached by traversing the `common' port of the component, which is either the 0th
   port if firstNlastPortIsCommon is true or the last port if not.) activeCommon indicates the
   sense of the common port.
   If `allowRootSubstitution' then allow the component to be removed where there is only
   one leaf connection made to a wire which is connected to no other component (ie. a port)
*/
PtrComponent TreeContraction (PtrComponent startComponent, bool firstNlastPortIsCommon, bool activeCommon, bool allowRootSubstitution)
{
    int commonPortIndex = (firstNlastPortIsCommon ? 0 : -1);
    PtrComponent base;
    PtrWireList newWires = NULL;
    int newWiresCount;
    PtrWire commonWire;

    if (Verbose)
    {
        fprintf (stderr, "compressing component tree starting from: ");
        StrPtrComponent (stderr, startComponent);
        putc ('\n', stderr);
    }

    base = FindRootComponent (startComponent, commonPortIndex, activeCommon);

    if (Verbose)
    {
        fprintf (stderr, "root component is: ");
        StrPtrComponent (stderr, base);
        putc ('\n', stderr);
    }

    commonWire = NthOfWireList (base->ports, commonPortIndex);

    /* don't remove top level common wire */
    newWires = GatherWiresFromComponentTree (base, commonPortIndex, activeCommon, false, commonWire);
    newWiresCount = LengthOfWireList (newWires);
    /* If we only have one wire get rid of the component */
    if (newWiresCount == 1)
    {
        PtrComponent farComponent = GetComponentFromWire (CAR (newWires), activeCommon);
        PtrComponent nearComponent = GetComponentFromWire (commonWire, !activeCommon);
        unsigned farPortNo = (activeCommon ? CAR (newWires)->activePort : CAR (newWires)->passivePort);
        PtrWire leafWire = CAR (newWires);

        if (farComponent)
        {
            MarkComponent (base);
            MarkWireWithReplacement (leafWire, commonWire);
            /* Change the wire in the target component and update common wire */
            UpdateWire (commonWire, activeCommon, farComponent, farPortNo);
            SubstituteWireForWireInWireList (farComponent->ports, leafWire, commonWire);
        } else
        {                       /* try and modify near component */
            if (allowRootSubstitution && nearComponent)
            {
                unsigned nearPortNo = (activeCommon ? leafWire->passivePort : leafWire->activePort);

                MarkComponent (base);
                MarkWireWithReplacement (commonWire, leafWire);
                UpdateWire (leafWire, !activeCommon, nearComponent, nearPortNo);
                SubstituteWireForWireInWireList (nearComponent->ports, commonWire, leafWire);
                return nearComponent;
            }
        }
        return farComponent;
    } else
    {
        /* Update wires in list */
        /* UpdateWires (newWires, !activeCommon, base, (firstNlastPortIsCommon ? 1 : 0), -1); */
        UpdateWires (newWires, !activeCommon, base, 0, -1);

        /* Place the common port on the front/back */
        if (firstNlastPortIsCommon)
            newWires = NewWireList (commonWire, newWires);
        else
            AppendWireLists (newWires, NewWireList (commonWire, NULL));

        base->ports = newWires;
        return base;
    }
}

/* MakeComponentType is the type of a procedure to make a component from 3 wires */
typedef PtrComponent (*MakeComponentType) (tPosition, PtrWire, PtrWire, PtrWire);

/* MakeEqualComponentType is the type of a procedure to make a component from a wire and a list of wires */
typedef PtrComponent (*MakeEqualComponentType) (tPosition, PtrWire, PtrWireList);

/* BuildCombineSplitTree : build a combine tree (adding new wires to `wires' and new components to
   `comps') to connect the inputs `inputWires' (in order CAR (inputWires) is the most significant
   wire) to the wire `outputWire' */
void BuildCombineSplitTree (PtrWire passiveWire, PtrWireList activeWires, bool combineNsplit, PtrComponentList * comps, PtrWireList * wires)
{
    unsigned noOfInputWires = LengthOfWireList (activeWires);
    MakeComponentType MakeComponent = (combineNsplit ? NewCombineComponent : NewSplitComponent);

    if (noOfInputWires == 2)    /* A leaf combine */
        *comps = NewComponentList (MakeComponent (NoPosition, passiveWire, CADR (activeWires), CAR (activeWires)), *comps);
    else
    {                           /* General combine */
        PtrWireList MSWires = activeWires;
        PtrWireList LSWires = NthCellOfWireList (activeWires, (noOfInputWires / 2) - 1); /* One cell before start of LSWires */
        PtrWire LSPassiveWire;

        {                       /* Place a NULL in the middle of the list to give two lists */
            PtrWireList next = CDR (LSWires);

            LSWires->next = NULL;
            LSWires = next;
        }
        LSPassiveWire = NewWire (1, WidthOfWireList (LSWires), NoType, 0 /* offset: no longer significant */ ,
          combineNsplit /* pull/push */ , CAR (LSWires)->position);
        *wires = NewWireList (LSPassiveWire, *wires);
        BuildCombineSplitTree (LSPassiveWire, LSWires, combineNsplit, comps, wires);

        /* MSWires will be the shorter of the two lists, if it has only one element then don't recurse
           down the MSWires list */
        if (!CDR (MSWires))
        {
            *comps = NewComponentList (MakeComponent (NoPosition, passiveWire, LSPassiveWire, CAR (MSWires)), *comps);
        } else
        {                       /* General case */
            PtrWire MSPassiveWire = NewWire (1, WidthOfWireList (MSWires), NoType,
              0 /* offset: no longer significant */ ,
              combineNsplit /* pull/push */ , CAR (LSWires)->position);

            *wires = NewWireList (MSPassiveWire, *wires);
            BuildCombineSplitTree (MSPassiveWire, MSWires, combineNsplit, comps, wires);
            *comps = NewComponentList (MakeComponent (NoPosition, passiveWire, LSPassiveWire, MSPassiveWire), *comps);
        }
    }
}

/* CombineSplitExpansion : take a combine/split component comp (which may have been previously marked), and
   generate a valid tree of Combine/Split and CombineEqual/SplitEqual components to replace it. NB. The original
   components ports may be of arbitrary width and in arbitrary numbers.  The resulting tree has
   only 2 input Combine/Split's and n-like-inputted CombineEqual/SplitEqual's. */
void CombineSplitExpansion (PtrComponent comp, PtrComponentList * comps, PtrWireList * wires)
{
    PtrWire passiveWire = CAR (comp->ports);
    PtrWireList wireIter = comp->ports;

    /* We are trying to find the maximal contiguous chunks of comp->ports with the same widths */
    unsigned equalChunkCount = 0;
    unsigned equalChunkSize = CADR (wireIter)->width;
    bool combineNsplit = comp->nature == CombineComponent;

    /* first-last equal cell dictates the range of cells of comp->ports which currently have the same width */
    PtrWireList firstEqualCell = CDR (wireIter);
    PtrWireList newAggregatedWires = NULL;

    /* Functions to create new CombineEqual and SplitEqual components */
    MakeEqualComponentType MakeEqualComponent = (combineNsplit ? NewCombineEqualComponent : NewSplitEqualComponent);

    while (wireIter)
    {
        if (CDR (wireIter) && CADR (wireIter)->width == equalChunkSize)
        {
            equalChunkCount++;
            wireIter = CDR (wireIter);
        } else
        {
            PtrWireList lastEqualCell = wireIter;

            wireIter = CDR (wireIter);
            if (firstEqualCell == lastEqualCell) /* Add no new component */
                newAggregatedWires = NewWireList (CAR (firstEqualCell), newAggregatedWires);
            else
            {
                PtrWire thisPassiveWire;

                if (!wireIter && firstEqualCell == CDR (comp->ports)) /* Is this a ...Equal of all the wires? */
                    thisPassiveWire = passiveWire;
                else
                {
                    thisPassiveWire = NewWire (1 /* fix later */ ,
                      equalChunkCount * CAR (firstEqualCell)->width,
                      passiveWire->type.type, CAR (firstEqualCell)->offset, combineNsplit /* pull/push */ ,
                      CAR (firstEqualCell)->position);
                    *wires = NewWireList (thisPassiveWire, *wires);
                }
                lastEqualCell->next = NULL; /* Pinch off the matching cells */
                *comps = NewComponentList (MakeEqualComponent (NoPosition, thisPassiveWire, firstEqualCell), *comps);
                newAggregatedWires = NewWireList (thisPassiveWire, newAggregatedWires);
            }
            equalChunkCount = 1;
            firstEqualCell = wireIter;
            if (firstEqualCell)
                equalChunkSize = CAR (wireIter)->width;
        }
    }
    /* Check that we have a Combine/SplitEqualled wire set or genuinely need a Combine/Split component */
    if (CAR (newAggregatedWires) != passiveWire)
        BuildCombineSplitTree (passiveWire, newAggregatedWires, combineNsplit, comps, wires);
}

/* OptimiseSequenceComponent : Generate the specString for Sequence components
*/
void OptimiseSequenceComponent (PtrComponent startComponent)
{
    if (Verbose)
    {
        fprintf (stderr, "optimising sequence component: ");
        StrPtrComponent (stderr, startComponent);
        putc ('\n', stderr);
    }

#if 0
AB2007
    PtrAccessList *accessListList = startComponent->param.sequence.accessListList;

    if (!accessListList)
        return;                 // This happens when an optimised sequence component has been read from a breeze file (during a flattened compilation)
#endif
    int length = LengthOfWireList (startComponent->ports) - 1;
    Ptrchar specString = NEW_ARRAY (char, length + 1);
    int i, j;

    for (i = 0; i < length - 1; i++)
    {
        bool WAWorWAR = false;
        PtrAccessList accessList = NthOfWireList (startComponent->ports, i + 1)->accesses;

        if (accessList == UnknownWireAccesses)
            WAWorWAR = 1;       // unknown case => safe
        else if (accessList)
        {
            for (j = i + 1; j < length; j++)
            {
                PtrAccessList accessList2 = NthOfWireList (startComponent->ports, j + 1)->accesses;

                if (accessList2 == UnknownWireAccesses)
                    WAWorWAR = 1; // unknown case => safe
                else if (accessList2)
                    WAWorWAR = CheckWAWandWAR (accessList, accessList2);

                if (WAWorWAR)
                    break;
        	}
        }

        specString[i] = WAWorWAR ? 'S' : 'T';
    }
    specString[length] = '\0';
    /* TODO: Check that startComponent->param.sequence.specString == 0 before overwriting it */
    startComponent->param.sequence.specString = specString;
}

/* TreeComponentContraction : contract tree like components into single component */
void TreeComponentContraction (PtrComponentList components, PtrComponentList * newComponents, PtrWireList * newWires)
{
    PtrComponentList compIter = components;

    /* FOR_EACH (compIter, true, ...) */
    while (compIter)
    {
        switch (CAR (compIter)->nature)
        {
/* {...},*_common */
        case CallComponent:
        case CallMuxComponent:
        case CallDemuxComponent:
        case SynchComponent:
        case SynchPullComponent:
            TreeContraction (CAR (compIter), false, true, true);
            break;
/* o_common,{...} */
        case ForkComponent:
        case WireForkComponent:
        case ForkPushComponent:
        case ConcurComponent:
        case CallDemuxPushComponent:
        case CallActiveComponent:
            TreeContraction (CAR (compIter), true, false, true);
            break;
        case SequenceComponent:
// Remove by LJ 09/05/2006 because there's a problem in it, exposed by SSEM
//            TreeContraction (CAR (compIter), true, false, true);
			TreeContraction (CAR (compIter), true, false, true);
			if (! DontOptimiseSequencers)
				OptimiseSequenceComponent (CAR (compIter));
            break;
        case CombineComponent:
        case SplitComponent:
            {
                PtrComponent base = TreeContraction (CAR (compIter), true, false, false);

                if (LengthOfWireList (base->ports) != 3)
                {
                    CombineSplitExpansion (base, newComponents, newWires);
                    MarkComponent (base);
                }
            }
            break;
/*
   SynchPushComponent
   SplitComponent
   PassivatorComponent
 */
        default:
            break;
        }
        compIter = CDR (compIter);
    }
}

/* RemoveLastElementFromWireList : remove the last element from the given list
   of more than two elements */
void RemoveLastElementFromWireList (PtrWireList wires)
{
    /* FOR_EACH (wires, true, ...) */
    while (wires)
    {
        if (!CDDR (wires))
        {
            wires->next = NULL;
            break;
        }
        wires = CDR (wires);
    }
}

/* PassivatorOptimise : transform Synch/Fork components with attached Continues into Passivators */
void PassivatorOptimise (PtrComponentList components)
{
    PtrComponentList compIter = components;

    /* FOR_EACH (compIter, true, ...) */
    while (compIter)
    {
        /* SynchPush with Continue connected to active port -> Passivator */
        if (CAR (compIter)->nature == ContinuePushComponent &&
          CAR (CAR (compIter)->ports)->activeComponent && CAR (CAR (compIter)->ports)->activeComponent->nature == SynchPushComponent)
        {
            PtrComponent synchPushComp = CAR (CAR (compIter)->ports)->activeComponent;

            /* A,{B,C,D},E => A,{B,C,D} */
            RemoveLastElementFromWireList (synchPushComp->ports);
            /* A,{B,C,D} => {B,C,D},A */
            synchPushComp->ports = AppendWireLists (CDR (synchPushComp->ports), NewWireList (CAR (synchPushComp->ports), NULL));
            MarkWire (CAR (CAR (compIter)->ports));
            MarkComponent (CAR (compIter));
            synchPushComp->nature = PassivatorPushComponent;
        } else
          if (CAR (compIter)->nature == ContinueComponent &&
          CAR (CAR (compIter)->ports)->activeComponent && CAR (CAR (compIter)->ports)->activeComponent->nature == SynchComponent)
        {
            PtrComponent synchComp = CAR (CAR (compIter)->ports)->activeComponent;

            /* {B,C,D},E => {B,C,D} */
            RemoveLastElementFromWireList (synchComp->ports);
            MarkWire (CAR (CAR (compIter)->ports));
            MarkComponent (CAR (compIter));
            synchComp->nature = PassivatorComponent;
        } else
          if (CAR (compIter)->nature == ContinuePushComponent &&
          CAR (CAR (compIter)->ports)->activeComponent && CAR (CAR (compIter)->ports)->activeComponent->nature == ForkPushComponent)
        {
            PtrComponent forkPushComp = CAR (CAR (compIter)->ports)->activeComponent;
            PtrComponent continuePushComp = CAR (compIter);
            PtrWireList forkPushPorts = forkPushComp->ports;
            unsigned originalPortCount = LengthOfWireList (forkPushComp->ports);

            /* Remove the Continue component and handle the transformation of the fork */
            RemoveWireFromWireList (CAR (CAR (compIter)->ports), &forkPushPorts);
            forkPushComp->ports = forkPushPorts;
            MarkWire (CAR (continuePushComp->ports));
            MarkComponent (continuePushComp);
            switch (originalPortCount)
            {
            case 2:
                forkPushComp->nature = ContinuePushComponent;
                break;
            case 3:
                {               /* ForkPush->Connector, remove connector */
                    PtrComponent passiveSideComponent = CAR (forkPushComp->ports)->activeComponent;
                    int passiveSidePortNo = CAR (forkPushComp->ports)->activePort;
                    PtrComponent activeSideComponent = CADR (forkPushComp->ports)->passiveComponent;
                    int activeSidePortNo = CADR (forkPushComp->ports)->passivePort;

                    if (passiveSideComponent)
                    {
                        UpdateWire (CADR (forkPushComp->ports), true, passiveSideComponent, passiveSidePortNo);
                        SubstituteWireForWireInWireList (passiveSideComponent->ports, CAR (forkPushComp->ports), CADR (forkPushComp->ports));
                        MarkWire (CAR (forkPushComp->ports));
                    } else if (activeSideComponent)
                    {
                        UpdateWire (CAR (forkPushComp->ports), false, activeSideComponent, activeSidePortNo);
                        SubstituteWireForWireInWireList (activeSideComponent->ports, CADR (forkPushComp->ports), CAR (forkPushComp->ports));
                        MarkWire (CADR (forkPushComp->ports));
                    } else
                        ASSERT (false);
                    MarkComponent (forkPushComp);
                    break;
                }
            default:
                break;          /* Still a ForkPush */
            }
        } else
          if (CAR (compIter)->nature == ContinueComponent &&
          CAR (CAR (compIter)->ports)->activeComponent && CAR (CAR (compIter)->ports)->activeComponent->nature == ForkComponent)
        {
            PtrComponent forkComp = CAR (CAR (compIter)->ports)->activeComponent;
            PtrWireList forkPorts = forkComp->ports;
            unsigned originalPortCount = LengthOfWireList (forkComp->ports);

            /* Remove the Continue component and handle the transformation of the fork */
            RemoveWireFromWireList (CAR (CAR (compIter)->ports), &forkPorts);
            forkComp->ports = forkPorts;
            MarkWire (CAR (CAR (compIter)->ports));
            MarkComponent (CAR (compIter));
            switch (originalPortCount)
            {
            case 2:
                forkComp->nature = ContinueComponent;
                break;
            case 3:
                {               /* Fork->Connector, remove connector */
                    PtrComponent passiveSideComponent = CAR (forkComp->ports)->activeComponent;
                    int passiveSidePortNo = CAR (forkComp->ports)->activePort;

                    UpdateWire (CADR (forkComp->ports), true, passiveSideComponent, passiveSidePortNo);
                    SubstituteWireForWireInWireList (passiveSideComponent->ports, CAR (forkComp->ports), CADR (forkComp->ports));
                    MarkWire (CAR (forkComp->ports));
                    MarkComponent (forkComp);
                    break;
                }
            default:
                break;          /* Still a Fork */
            }
        }
        compIter = CDR (compIter);
    }
}

/* AdapterOptimise : amalgamate adapters occuring after binary operators */
void AdapterOptimise (PtrComponentList components)
{
    PtrComponentList compIter = components;

    /* FOR_EACH (compIter, true, ...) */
    while (compIter)
    {
        /* Binary with Adapt connected to passive port */
        if ((CAR (compIter)->nature == BinaryFuncComponent ||
            CAR (compIter)->nature == BinaryFuncConstRComponent) &&
          CAR (CAR (compIter)->ports)->activeComponent && CAR (CAR (compIter)->ports)->activeComponent->nature == AdaptComponent)
        {
            PtrComponent func = CAR (compIter);
            PtrComponent adapter = CAR (func->ports)->activeComponent;

            if ((adapter->param.adapt.outputSigned
                && adapter->param.adapt.inputSigned) || (func->param.binary.operation >= EqualOperator && func->param.binary.operation <= GEOperator))
            {
                /* Don't remove sign-extending adapters, or adapters connected to equality/inequality operators */
            } else
            {
                MarkWire (CAR (func->ports)); /* Trim binary result wire */
                /* Make the functions result the adapter result wire */
                func->ports = NewWireList (CAR (adapter->ports), CDR (func->ports));
                MarkComponent (adapter); /* Remove adapter */
                /* Update functions result signedness */
                func->param.binary.resultSigned = adapter->param.adapt.outputSigned;
            }
        }
        compIter = CDR (compIter);
    }
}

/* CombineConstantOptimise : amalgamate constants joined with combine */
void CombineConstantOptimise (PtrComponentList components)
{
    bool modified = true;

    while (modified)
    {
        PtrComponentList compIter = components;

        modified = false;

        /* FOR_EACH (compIter, true, ...) */
        while (compIter)
        {
            /* Combine with constants connected */
            if (CAR (compIter)->nature == CombineComponent &&
              CADR (CAR (compIter)->ports)->passiveComponent &&
              CADDR (CAR (compIter)->ports)->passiveComponent &&
              CADR (CAR (compIter)->ports)->passiveComponent->nature ==
              ConstantComponent && CADDR (CAR (compIter)->ports)->passiveComponent->nature == ConstantComponent)
            {
                PtrComponent combine = CAR (compIter);
                PtrComponent lswConst = CADR (combine->ports)->passiveComponent;
                PtrComponent mswConst = CADDR (combine->ports)->passiveComponent;
                unsigned lswWidth = CAR (lswConst->ports)->width;

                MarkWire (CAR (lswConst->ports)); /* Trim both constant wires */
                MarkWire (CAR (mswConst->ports));
                MarkComponent (mswConst); /* Remove MSW constant and the combine */
                MarkComponent (combine);
                /* Use the same result wire */
                lswConst->ports = NewWireList (CAR (combine->ports), NULL);
                UpdateWire (CAR (lswConst->ports), false /* passive comp */ ,
                  lswConst, 0);
                /* Change the value in the constant */
                {
                    PtrMP_INT tmp = CopyMP_INT (mswConst->param.constant.value);

                    mpz_mul_2exp (tmp, tmp, lswWidth);
                    mpz_add (tmp, lswConst->param.constant.value, tmp);
                    lswConst->param.constant.value = tmp;
                }
                modified = true;
            }
            compIter = CDR (compIter);
        }
    }
}

/* PassivatorTransferrerMergeOptimise : remove passivators which are connected to
   transferrers on all ports and replace with a Synch (for the tfr. activations)
   a single transferrer and a Fork */
void PassivatorTransferrerMergeOptimise (PtrComponentList components, PtrComponentList * newComps, PtrWireList * newWires)
{
    PtrComponentList compIter = components;

    /* FOR_EACH (compIter, true, ...) */
    while (compIter)
    {
        if (CAR (compIter)->nature == PassivatorPushComponent)
        {
            PtrWireList ports = CAR (compIter)->ports;
            bool allFetch = true;

            /* Are all ports connected to a transferrer? */
            /* FOR_EACH (ports, true, ...) */
            while (ports)
            {
                if (!CAR (ports)->activeComponent || CAR (ports)->activeComponent->nature != FetchComponent)
                {
                    allFetch = false;
                    break;
                }
                ports = CDR (ports);
            }
            if (allFetch)
            {
                PtrWireList forkOutPorts = NULL;
                PtrComponent fetch = NthOfWireList (CAR (compIter)->ports, -1)->activeComponent;
                PtrWireList fetchActivations = NewWireList (CAR (fetch->ports), NULL);
                unsigned forkOutPortsCount = 0;

                ports = CAR (compIter)->ports;
                /* Iterate across output ports */
                /* FOR_EACH (ports, CDR (ports), ...) */
                while (ports && CDR (ports))
                {
                    PtrComponent redundentFetch = CAR (ports)->activeComponent;

                    forkOutPorts = NewWireList (CADDR (redundentFetch->ports), forkOutPorts);
                    fetchActivations = NewWireList (CAR (redundentFetch->ports), fetchActivations);
                    MarkWire (CAR (ports));
                    MarkComponent (redundentFetch);
                    forkOutPortsCount++;
                    ports = CDR (ports);
                }
                MarkComponent (CAR (compIter)); /* Remove the passivator */
                if (forkOutPortsCount == 1) /* No fork required */
                {
                    MarkWire (CADDR (fetch->ports)); /* Remove fetch output wire (to passivator) */
                    CDDR (fetch->ports)->body = CAR (forkOutPorts); /* Change fetch port */
                } else
                {               /* Need to create a fork */
                    PtrComponent fork = NewComponent (NoPosition, ForkPushComponent);

                    fork->ports = NewWireList (CADDR (fetch->ports), forkOutPorts);
                    /* Update ports, fetch output on 0, fork outputs on 1.. */
                    UpdateWire (CAR (fork->ports), false, fork, 0);
                    UpdateWires (CDR (fork->ports), true, fork, 1, -1);
                    *newComps = NewComponentList (fork, *newComps);
                }
                /* output of fetch should now be correctly connected, do the activation */
                {
                    /* New activation wire between fetch and synch components */
                    /* The NULL in the accessList field could be something more sensible if I cared more
                    	about being less conservative */
                    PtrWire newFetchActivation = NewSyncWire (1, CAR (fetch->ports)->position);
                    PtrComponent synch = NewComponent (NoPosition, SynchComponent);

                    synch->ports = AppendWireLists (fetchActivations, NewWireList (newFetchActivation, NULL));
                    fetch->ports->body = newFetchActivation;
                    /* Update both ends of the new fetch activation */
                    UpdateWire (newFetchActivation, false, fetch, 0);
                    UpdateWire (newFetchActivation, true, synch, forkOutPortsCount);
                    /* Update passive ends of removed transferrer activations */
                    UpdateWires (synch->ports, false, synch, 0, forkOutPortsCount + 1);
                    /* Add the new activation to the wire list and the synch to the comp. list */
                    *newWires = NewWireList (newFetchActivation, *newWires);
                    *newComps = NewComponentList (synch, *newComps);
                }
            }
        }
        compIter = CDR (compIter);
    }
}

/* ConcursorForkSynchOptimise: remove redundent Synch components which have all their
   inputs connected to the same Fork or Concur component */
void ConcursorForkSynchOptimise (PtrComponentList components)
{
    PtrComponentList compIter = components;

    /* FOR_EACH (compIter, true, ...) */
    while (compIter)
    {
        /* A Synch with the first input connected to either a Concur or a Fork */
        if (CAR (compIter)->nature == SynchComponent &&
          CAR (CAR (compIter)->ports)->activeComponent &&
          (CAR (CAR (compIter)->ports)->activeComponent->nature ==
            ConcurComponent || CAR (CAR (compIter)->ports)->activeComponent->nature == ForkComponent))
        {
            PtrWireList ports = CAR (compIter)->ports;
            PtrComponent commonComp = CAR (ports)->activeComponent;
            bool allConnected = true;

            /* Are all input ports connected to the same comp? */
            /* FOR_EACH (ports, CDR (ports), ...) */
            while (ports && CDR (ports))
            {
                if (!CAR (ports)->activeComponent || CAR (ports)->activeComponent != commonComp)
                {
                    allConnected = false;
                    break;
                }
                ports = CDR (ports);
            }
            if (allConnected)
            {
                PtrWireList newConcurPorts = NULL;
                PtrWireList concurPorts = CDR (commonComp->ports);
                unsigned uncommonPortsCount = 0;
                PtrWire synchOutWire = NthOfWireList (CAR (compIter)->ports, -1);

                /* Remove wires connected to the Synch */
                /* FOR_EACH (concurPorts, true, ...) */
                while (concurPorts)
                {
                    if (CAR (concurPorts)->passiveComponent != CAR (compIter))
                    {
                        newConcurPorts = NewWireList (CAR (concurPorts), newConcurPorts);
                        uncommonPortsCount++;
                    } else
                        MarkWire (CAR (concurPorts));
                    concurPorts = CDR (concurPorts);
                }
                MarkComponent (CAR (compIter)); /* Remove the synch */
                if (uncommonPortsCount == 0) /* Only one Concur output, remove the Concur */
                {
                    PtrWire activation = CAR (commonComp->ports);
                    PtrComponent passiveComponent = synchOutWire->passiveComponent;
                    unsigned passiveComponentPortNo = synchOutWire->passivePort;

                    /* Remove the Concur */
                    MarkComponent (commonComp);
                    /* Graft the Concur's activation into the passive component */
                    SubstituteWireForWireInWireList (passiveComponent->ports, synchOutWire, activation);
                    UpdateWire (activation, false, passiveComponent, passiveComponentPortNo);
                    /* Remove the original wire connected to the passive component */
                    MarkWire (synchOutWire);
                } else
                {               /* Change the concursor */
                    newConcurPorts = NewWireList (NthOfWireList (CAR (compIter)->ports, -1), newConcurPorts);
                    /* Update common component connections */
                    UpdateWires (newConcurPorts, true, commonComp, 1, -1);
                    /* Add the new wires onto the activation wire of the common component */
                    commonComp->ports->next = newConcurPorts;
                }
            }
        }
        compIter = CDR (compIter);
    }
}

/* EncodeOptimise: remove Fetch/Constant combinations connected to CallMux inputs */
void EncodeOptimise (PtrComponentList components, PtrComponentList * newComps, PtrWireList * newWires)
{
    PtrComponentList compIter = components;

    /* FOR_EACH (compIter, true, ...) */
    while (compIter)
    {
        /* Find a CallMux */
        if (CAR (compIter)->nature == CallMuxComponent)
        {
            PtrComponent call = CAR (compIter);

            /* Find the Constant->Fetch combos. connected to this component */
            PtrWireList callPorts = call->ports;
            PtrWire callOutputWire = NULL;

            /* constantWires: inputs to CallMux which come directly from constants */
            PtrWireList constantWires = NULL;
            unsigned constantInputCount = 0;

            /* nonConstantInputs: input wires which don't connect to constants */
            PtrWireList nonConstantWires = NULL;
            unsigned nonConstantInputCount = 0;
            unsigned inputCount = LengthOfWireList (callPorts) - 1;

            /* FOR_EACH (callPorts, inputCount, ...) */
            while (callPorts && inputCount) /* Differentiate const/non-const inputs */
            {
                if (CAR (callPorts)->activeComponent &&
                  CAR (callPorts)->activeComponent->nature == FetchComponent
                  && CADR (CAR (callPorts)->activeComponent->ports)->
                  passiveComponent && CADR (CAR (callPorts)->activeComponent->ports)->passiveComponent->nature == ConstantComponent)
                {
                    constantWires = NewWireList (CAR (callPorts), constantWires);
                    constantInputCount++;
                } else
                {
                    nonConstantWires = NewWireList (CAR (callPorts), nonConstantWires);
                    nonConstantInputCount++;
                }
                inputCount--;
                if (inputCount == 0)
                    callOutputWire = CADR (callPorts);
                callPorts = CDR (callPorts);
            }
            if (constantInputCount > 1) /* don't optimise CallMux's with only one constant input */
            {
                PtrSpanListList constantSpans = NULL;
                PtrWireList activations = NULL;

                /* FOR_EACH (constantWires, true, ...) */
                while (constantWires)
                {
                    PtrMP_INT constant = CADR (CAR (constantWires)->activeComponent->ports)->passiveComponent->param.constant.value;

                    /* Mark this wire, the Fetch, Constant and wire between Fetch and Constant */
                    MarkWire (CAR (constantWires));
                    MarkComponent (CAR (constantWires)->activeComponent);
                    MarkWire (CADR (CAR (constantWires)->activeComponent->ports));
                    MarkComponent (CADR (CAR (constantWires)->activeComponent->ports)->passiveComponent);
                    /* Get the constant value and the Fetch activation wire */
                    constantSpans = NewSpanListList (NewSpanList (NewSpan (constant, constant, NoType), NULL), constantSpans);
                    activations = NewWireList (CAR (CAR (constantWires)->activeComponent->ports), activations);
                    constantWires = CDR (constantWires);
                }
                {
                    /* Make the encode spec, the Encode component */
                    Ptrchar encodeSpec = MakeSpanCaseSpecification (constantSpans,
                      WireWidth (callOutputWire));
                    PtrWire encodeResultWire;
                    PtrComponent encodeComp;

                    if (nonConstantInputCount > 0) /* Keep a CallMux as well */
                    {
                        encodeResultWire = CopyAndRenumberWire (CAR (call->ports), 1 /* fix later */ ,
                          CAR (activations)->position);
                        *newWires = NewWireList (encodeResultWire, *newWires);
                    } else
                        encodeResultWire = callOutputWire;
                    encodeComp = NewEncodeComponent (NoPosition, encodeSpec, activations, encodeResultWire);
                    *newComps = NewComponentList (encodeComp, *newComps);
                    if (nonConstantInputCount > 0) /* Frig the CallMux */
                    {
                        UpdateWire (encodeResultWire, false /* passive */ , call,
                          0);
                        UpdateWires (nonConstantWires, false /* passive */ , call,
                          1, nonConstantInputCount);
                        UpdateWire (callOutputWire, true /* active */ , call,
                          nonConstantInputCount + 1);
                        call->ports = NewWireList (encodeResultWire, AppendWireLists (nonConstantWires, NewWireList (callOutputWire, NULL)));
                    } else
                        MarkComponent (call);
                }
            }
        }
        compIter = CDR (compIter);
    }
}

/* BinaryConstOptimise: transform BinaryFunc's with constant ports into BinaryFuncR's. */
void BinaryConstOptimise (PtrComponentList components, PtrComponentList * newComps, PtrWireList * newWires)
{
    PtrComponentList compIter = components;

    /* FOR_EACH (compIter, true, ...) */
    while (compIter)
    {
        /* Find BinaryFunc's with Constant's */
        if (CAR (compIter)->nature == BinaryFuncComponent)
        {
            PtrComponent comp = CAR (compIter);
            PtrWire leftWire = CADR (comp->ports);
            PtrWire rightWire = CADDR (comp->ports);
            bool hasConstant = false;
            bool leftConstant = false;

            /* Do we have a constant port? */
            if (leftWire->passiveComponent && leftWire->passiveComponent->nature == ConstantComponent)
            {
                hasConstant = true;
                leftConstant = true;
            }
            if (rightWire->passiveComponent && rightWire->passiveComponent->nature == ConstantComponent)
            {
                hasConstant = true;
                leftConstant = false;
            }

            if (hasConstant)
            {
                PtrWire discardedWire;

                comp->nature = BinaryFuncConstRComponent; /* Change nature */
                if (leftConstant)
                {               /* reverse the operation/ports ... */
                    bool tmp;

                    comp->param.binary.operation = MirrorOperators[comp->param.binary.operation];
                    discardedWire = CADR (comp->ports);
                    comp->ports = NewWireList (CAR (comp->ports), NewWireList (CADDR (comp->ports), NULL));
                    tmp = comp->param.binary.leftSigned;
                    comp->param.binary.leftSigned = comp->param.binary.rightSigned;
                    comp->param.binary.rightSigned = tmp;
                } else
                {               /* right hand constant, don't do much */
                    discardedWire = CADDR (comp->ports);
                    comp->ports = NewWireList (CAR (comp->ports), NewWireList (CADR (comp->ports), NULL));
                }
                comp->param.binary.rightValue = discardedWire->passiveComponent->param.constant.value;
                // LJ 14/06/04: fix bug "inst := 2; if inst = (-2 as 24 bits) then wrong result"
                // was: comp->param.binary.rightWidth = Abs (SmallestRangeToHoldValue (comp->param.binary.rightValue));
                comp->param.binary.rightWidth = discardedWire->width;
                MarkComponent (discardedWire->passiveComponent);
                MarkWire (discardedWire);
                /*
                   char buf[1000];
                   mpz_get_str (buf, 10, comp->param.binary.rightValue);
                   printf ("BinaryConstOptimise: %d %d %d %s\n", leftConstant, comp->param.binary.rightSigned, comp->param.binary.rightWidth, buf);
                   StrBreezeComponentParameters(stdout, comp);
                 */
            }
        }
        compIter = CDR (compIter);
    }
}

/* UpdateReadPortsBitfield : Update a [*]VariableComponent's read port's low and high indexes, written in the component's readPortsSpecString */
static void UpdateReadPortsBitfield (PtrComponent comp, int readPortNum, int lowIndex, int highIndex)
{
    gchar **indexes = g_strsplit (comp->param.variable.readPortsSpecString, ";", 0);
    int len = 0;

    while (indexes[len])
        len++;

    if (readPortNum > len - 1)
    {                           // Need to extend the array
        indexes = g_renew (gchar *, indexes, readPortNum + 1 + 1);
        for (; len < readPortNum + 1; len++)
            indexes[len] = g_strdup ("");
        indexes[len] = NULL;
    }

	/* AB 2010-09-09 made this increment the old index (if any) */
	int oldLowIndex = 0;
    if (indexes[len])
    {
		sscanf (indexes[len], "%d", &oldLowIndex);
    }

    g_free (indexes[readPortNum]);
    indexes[readPortNum] = g_strdup_printf ("%d..%d", oldLowIndex + lowIndex, oldLowIndex + highIndex);

    gchar *newSpecString = g_strjoinv (";", indexes);

    // g_strfreev (indexes);
    // g_free (comp->param.variable.readPortsSpecString);
    comp->param.variable.readPortsSpecString = newSpecString;
}

/* VariableSliceOptimise : amalgamate slices occuring after after any kind of Variable component */
void VariableSliceOptimise (PtrComponentList components)
{
    PtrComponentList compIter = components;

    /* FOR_EACH (compIter, true, ...) */
    while (compIter)
    {
        /* [*]Variable with Slice connected to read port */
        if (CAR (compIter)->nature == VariableComponent ||
          CAR (compIter)->nature == InitVariableComponent ||
          CAR (compIter)->nature == FalseVariableComponent ||
          CAR (compIter)->nature == PassiveEagerFalseVariableComponent || CAR (compIter)->nature == ActiveEagerFalseVariableComponent)
        {
            PtrComponent var = CAR (compIter);

            /* Find the read ports */
            PtrWireList readPorts;
            int readPortsOffset;

            switch (CAR (compIter)->nature)
            {
            case VariableComponent:
                readPorts = CDR (var->ports);
                readPortsOffset = 1;
                break;
            case InitVariableComponent:
            case FalseVariableComponent:
                readPorts = CDDR (var->ports);
                readPortsOffset = 2;
                break;
            case PassiveEagerFalseVariableComponent:
            case ActiveEagerFalseVariableComponent:
                readPorts = CDDDR (var->ports);
                readPortsOffset = 3;
                break;
            default:
                readPorts = NULL;
                readPortsOffset = 0;
            }

            /* Find Slice components connected to read ports */
            /* FOR_EACH (readPorts, ...) */
            int readPortNum = 0;

            while (readPorts)
            {
                if (CAR (readPorts)->activeComponent && CAR (readPorts)->activeComponent->nature == SliceComponent)
                {
                    PtrComponent slice = CAR (readPorts)->activeComponent;
                    PtrWire wireFromSliceToNextComp = CAR (slice->ports);
                    PtrWire wireFromVarToSlice = CAR (readPorts);

                    /* Update port's bitfield */
                    UpdateReadPortsBitfield (var, readPortNum, slice->param.slice.lowIndex,
                      slice->param.slice.lowIndex + wireFromSliceToNextComp->width - 1);

                    /* Connect the read port to the component originally connected to the Slice */
                    SubstituteWireForWireInWireList (var->ports, wireFromVarToSlice, wireFromSliceToNextComp);
                    UpdateWire (wireFromSliceToNextComp, false, var, readPortNum + readPortsOffset);

                    /* Trim old wire */
                    MarkWire (wireFromVarToSlice);

                    /* Remove slice */
                    MarkComponent (slice);
                }

                readPorts = CDR (readPorts);
                readPortNum++;
            }
        }
        compIter = CDR (compIter);
    }
}

/* OptimiseProcedure : perform peephole optimisation of the procedure in proc */
void OptimiseProcedure (PtrProcedure proc, tPosition position)
{
    PtrComponentList comps;
    PtrWireList wires;
    bool isFunction = proc->nature == SharedFunction || proc->nature == UnSharedFunction;

    if (isFunction)
    {
        comps = proc->info.function.attributes.components;
        wires = proc->info.function.attributes.wires;
    } else
    {
        comps = proc->info.procedure.attributes.components;
        wires = proc->info.procedure.attributes.wires;
    }

    if (Verbose)
    {
        fprintf (stderr, "-- Begin optimising part: ");
        WriteIdent (stderr, proc->ident);
        putc ('\n', stderr);
    }

    if (comps)
    {
        PtrComponentList newComps = NULL;
        PtrWireList newWires = NULL;

        AdapterOptimise (comps);
        CombineConstantOptimise (comps);

        BinaryConstOptimise (comps, &newComps, &newWires);
        if (newComps)
            comps = AppendComponentLists (comps, newComps);
        if (newWires)
            wires = AppendWireLists (newWires, wires);
        newComps = NULL;
        newWires = NULL;

        TreeComponentContraction (comps, &newComps, &newWires);
        if (newComps)
            comps = AppendComponentLists (comps, newComps);
        if (newWires)
            wires = AppendWireLists (newWires, wires);
        newComps = NULL;
        newWires = NULL;

        PassivatorOptimise (comps);

        PassivatorTransferrerMergeOptimise (comps, &newComps, &newWires);
        if (newComps)
            comps = AppendComponentLists (comps, newComps);
        if (newWires)
            wires = AppendWireLists (newWires, wires);
        newComps = NULL;
        newWires = NULL;

        ConcursorForkSynchOptimise (comps);

        EncodeOptimise (comps, &newComps, &newWires);
        if (newComps)
            comps = AppendComponentLists (comps, newComps);
        if (newWires)
            wires = AppendWireLists (newWires, wires);

        VariableSliceOptimise (comps);

        /* Remove marked components/wires (and renumber) */
        if (isFunction)
        {
            proc->info.function.attributes.wires = SweepWireList (wires);
            proc->info.function.attributes.components = SweepComponentList (comps);
        } else
        {
            proc->info.procedure.attributes.wires = SweepWireList (wires);
            proc->info.procedure.attributes.components = SweepComponentList (comps);
        }
    }

    if (Verbose)
    {
        fprintf (stderr, "-- End optimising part: ");
        WriteIdent (stderr, proc->ident);
        putc ('\n', stderr);
    }
}
