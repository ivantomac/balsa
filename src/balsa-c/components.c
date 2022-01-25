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

	`components.c'
	Handshake components list type
	and manipulation
	
 */

#include "components.h"
#include "procedures.h"
#include "misc.h"
#include "flags.h"
#include <string.h>

/* OFFSET: offset into Component structure */
#undef OFFSET
#undef NOPARAMS
#define OFFSET(element) OFFSETOF(Component,element)

#define NOPARAMS 0,0,0,0,0,0,0,0,0,0

ComponentSpec BreezeComponents[] = {
    /* Dummy entries */
    {"<None>", "", "",
          {NOPARAMS}
      }
    ,
    /* Comment */
    {"<Comment>", "", "",
          {NOPARAMS}
      }
    ,
    /* Procedure */
    {"<Procedure>", "", "",
          {NOPARAMS}
      }
    ,

    /* Components, name params ports param store offsets */
    {"$BrzConstant", "#i", "o",
          {0, OFFSET (param.constant.value), 0, 0, 0, 0, 0, 0, 0, 0}
      }
    ,
    {"$BrzCall", "#", "0sS",
          {OFFSET (portCountParam), 0, 0, 0, 0, 0, 0, 0, 0, 0}
      }
    ,
    {"$BrzCallMux", "##", "1iO",
          {0, OFFSET (portCountParam), 0, 0, 0, 0, 0, 0, 0, 0}
      }
    ,
    {"$BrzCallDemux", "##", "1oI",
          {0, OFFSET (portCountParam), 0, 0, 0, 0, 0, 0, 0, 0}
      }
    ,
    {"$BrzSynch", "#", "0sS",
          {OFFSET (portCountParam), 0, 0, 0, 0, 0, 0, 0, 0, 0}
      }
    ,
    {"$BrzSynchPush", "##", "i1oO",
          {0, OFFSET (portCountParam), 0, 0, 0, 0, 0, 0, 0, 0}
      }
    ,
    {"$BrzSynchPull", "##", "1oI",
          {0, OFFSET (portCountParam), 0, 0, 0, 0, 0, 0, 0, 0}
      }
    ,
    {"$BrzSlice", "###", "oI",
          {0, 0, OFFSET (param.slice.lowIndex), 0, 0, 0, 0, 0, 0, 0}
      }
    ,
    {"$BrzCombine", "###", "oII",
          {NOPARAMS}
      }
    ,
    {"$BrzAdapt", "##bb", "oI",
          {0, 0, OFFSET (param.adapt.outputSigned), OFFSET (param.adapt.inputSigned),
                0,
            0, 0, 0, 0, 0}
      }
    ,
    {"$BrzNullAdapt", "#", "Si",
          {NOPARAMS}
      }
    ,
    {"$BrzUnaryFunc", "##Ub", "oI",
          {0, 0, OFFSET (param.unary.operation), OFFSET (param.unary.inputSigned), 0,
            0, 0, 0, 0, 0}
      }
    ,
    {"$BrzBinaryFunc", "###Bbbb", "oII",
          {0, 0, 0, OFFSET (param.binary.operation),
                OFFSET (param.binary.resultSigned),
                OFFSET (param.binary.leftSigned), OFFSET (param.binary.rightSigned), 0, 0,
            0}
      }
    ,
    {"$BrzBinaryFuncConstR", "###Bbbbi", "oI",
          {0, 0, OFFSET (param.binary.rightWidth), OFFSET (param.binary.operation),
                OFFSET (param.binary.resultSigned),
                OFFSET (param.binary.leftSigned), OFFSET (param.binary.rightSigned),
                OFFSET (param.binary.rightValue),
            0, 0}
      }
    ,
    {"$BrzContinue", "", "s",
          {NOPARAMS}
      }
    ,
    {"$BrzContinuePush", "#", "i",
          {NOPARAMS}
      }
    ,
    {"$BrzHalt", "", "s",
          {NOPARAMS}
      }
    ,
    {"$BrzHaltPush", "", "i",
          {NOPARAMS}
      }
    ,
    {"$BrzFork", "#", "s0S",
          {OFFSET (portCountParam), 0, 0, 0, 0, 0, 0, 0, 0, 0}
      }
    ,
    {"$BrzWireFork", "#", "s0S",
          {OFFSET (portCountParam), 0, 0, 0, 0, 0, 0, 0, 0, 0}
      }
    ,
    {"$BrzForkPush", "##", "i1O",
          {0, OFFSET (portCountParam), 0, 0, 0, 0, 0, 0, 0, 0}
      }
    ,
    {"$BrzConcur", "#", "s0S",
          {OFFSET (portCountParam), 0, 0, 0, 0, 0, 0, 0, 0, 0}
      }
    ,
    {"$BrzVariable", "##$$", "i1o",
          {0, OFFSET (portCountParam), OFFSET (param.variable.name), OFFSET (param.variable.readPortsSpecString), 0, 0, 0, 0,
            0, 0}
      }
    ,
    {"$BrzInitVariable", "##i$$", "is1o",
          {0, OFFSET (portCountParam), OFFSET (param.variable.initialisedValue),
                OFFSET (param.variable.name), OFFSET (param.variable.readPortsSpecString), 0, 0,
            0, 0, 0}
      }
    ,
    {"$BrzBuiltinVariable", "#$", "i0o",
          {OFFSET (portCountParam), OFFSET (param.variable.name), 0, 0, 0, 0, 0, 0,
            0, 0}
      }
    ,
    {"$BrzFalseVariable", "##$", "iS1o",
          {0, OFFSET (portCountParam), OFFSET (param.variable.readPortsSpecString), 0, 0, 0, 0, 0, 0, 0}
      }
    ,
    {"$BrzSplit", "###", "iOO",
          {NOPARAMS}
      }
    ,
    {"$BrzLoop", "", "iO",
          {NOPARAMS}
      }
    ,
    {"$BrzFetch", "#b", "sIO",
          {0, OFFSET (param.fetch.outBroad), 0, 0, 0, 0, 0, 0, 0, 0}
      }
    ,
    {"$BrzBar", "#", "so0I0S",
          {OFFSET (portCountParam), 0, 0, 0, 0, 0, 0, 0, 0, 0}
      }
    ,
    {"$BrzWhile", "", "sIS",
          {NOPARAMS}
      }
    ,
    {"$BrzDecisionWait", "#", "s0s0S",
          {OFFSET (portCountParam), 0, 0, 0, 0, 0, 0, 0, 0, 0}
      }
    ,
    {"$BrzArbiter", "", "ssSS",
          {NOPARAMS}
      }
    ,
    {"$BrzCase", "##$", "i1S",
          {0, OFFSET (portCountParam), OFFSET (param.casecomp.specString), 0, 0, 0,
            0, 0, 0, 0}
      }
    ,
    {"$BrzCaseFetch", "###$", "oI2I",
          {0, 0, OFFSET (portCountParam), OFFSET (param.casecomp.specString), 0, 0,
            0, 0, 0, 0}
      }
    ,
    {"$BrzPassivator", "#", "0s",
          {OFFSET (portCountParam), 0, 0, 0, 0, 0, 0, 0, 0, 0}
      }
    ,
    {"$BrzPassivatorPush", "##", "1oi",
          {0, OFFSET (portCountParam), 0, 0, 0, 0, 0, 0, 0, 0}
      }
    ,
    {"$BrzCombineEqual", "###", "o2I",
          {0, 0, OFFSET (portCountParam), 0, 0, 0, 0, 0, 0, 0}
      }
    ,
    {"$BrzSplitEqual", "###", "i2O",
          {0, 0, OFFSET (portCountParam), 0, 0, 0, 0, 0, 0, 0}
      }
    ,
    {"$BrzEncode", "##$", "1sO",
          {0, OFFSET (portCountParam), OFFSET (param.casecomp.specString), 0, 0, 0,
            0, 0, 0, 0}
      }
    ,
    {"$BrzCallDemuxPush", "##", "i1O",
          {0, OFFSET (portCountParam), 0, 0, 0, 0, 0, 0, 0, 0}
      }
    ,
    {"$BrzCallActive", "#", "i0O",
          {OFFSET (portCountParam), 0, 0, 0, 0, 0, 0, 0, 0, 0}
      }
    ,
    {"$BrzUnaryFuncPush", "##Ub", "Oi",
          {0, 0, OFFSET (param.unary.operation), OFFSET (param.unary.inputSigned), 0,
            0, 0, 0, 0, 0}
      }
    ,
    {"$BrzBinaryFuncPush", "###Bbbb", "Oii",
          {0, 0, 0, OFFSET (param.binary.operation),
                OFFSET (param.binary.resultSigned),
                OFFSET (param.binary.leftSigned), OFFSET (param.binary.rightSigned), 0, 0,
            0}
      }
    ,
    {"$BrzBinaryFuncConstRPush", "###Bbbbi", "Oi",
          {0, 0, OFFSET (param.binary.rightWidth), OFFSET (param.binary.operation),
                OFFSET (param.binary.resultSigned),
                OFFSET (param.binary.leftSigned), OFFSET (param.binary.rightSigned),
                OFFSET (param.binary.rightValue),
            0, 0}
      }
    ,
    {"$BrzString", "$", "o",
          {OFFSET (param.string.string), 0, 0, 0, 0, 0, 0, 0, 0, 0}
      }
    ,
    {"$BrzSequence", "#$", "s0S",
          {OFFSET (portCountParam), OFFSET (param.sequence.specString), 0, 0, 0, 0, 0, 0, 0, 0}
      }
    ,
    {"$BrzPassiveEagerFalseVariable", "##$", "siS1o",
          {0, OFFSET (portCountParam), OFFSET (param.sequence.specString), 0, 0, 0, 0, 0, 0, 0}
      }
    ,
    {"$BrzActiveEagerFalseVariable", "##$", "sIS1o",
          {0, OFFSET (portCountParam), OFFSET (param.sequence.specString), 0, 0, 0, 0, 0, 0, 0}
      }
    ,
    {"$BrzPassiveSyncEagerFalseVariable", "", "ssS",
          {NOPARAMS}
      }
    ,
    {"$BrzPassiveEagerNullAdapt", "##", "siS",
          {NOPARAMS}
      }
    ,
    {"$BrzActiveEagerNullAdapt", "##", "sIS",
          {NOPARAMS}
      }
};

/* Constrain scope of handy defns. */
#undef OFFSET
#undef NOPARAMS

tIdent FirstBreezeComponentIdent = 0;

/* BeginComponents : initialise module local data structures,
   NB. must call AFTER BeginIdents */
void BeginComponents (void)
{
    unsigned count;

    FirstBreezeComponentIdent = MakeIdent1 (BreezeComponents[0].name);
    for (count = 1; count < (int) LastComponent; count++)
    {
        MakeIdent1 (BreezeComponents[count].name);
    }
}

/* NewComponent: create empty component */
PtrComponent NewComponent (tPosition position, ComponentNature nature)
{
    PtrComponent component = NEW (Component);

    component->nature = nature;
    component->ports = NULL;
    component->options = NULL;
    component->position = position;
    return component;
}

/* GetComponentPortSpec : return the spec string of the given component (based on nature,
   or if nature is ProcedureComponent then procedure->spec) */
Ptrchar GetComponentPortSpec (PtrComponent component)
{
    if (component->nature == ProcedureComponent)
        return component->param.procedure.portSpec;
    return BreezeComponents[component->nature].portSpec;
}

/* GetComponentIntegerParameterAtPosition : get the integer parameter value at parameter
	position `pos' in a component's parameter set */
int GetComponentIntegerParameterAtPosition (PtrComponent component, unsigned pos)
{
    return *((int *) (((Ptrchar) component) + BreezeComponents[(int) component->nature].offsets[pos]));
}

/* ---------- Component constructors */

/* UpdateWire : update the {active,passive}{Component,Port} entries in a wire */
void UpdateWire (PtrWire wire, bool isActive, PtrComponent component, unsigned portNo)
{
    if (wire)                   /* Don't respond to NULL wires */
    {
        if (isActive)
        {
            wire->activeComponent = component;
            wire->activePort = portNo;
        } else
        {
            wire->passiveComponent = component;
            wire->passivePort = portNo;
        }
    }
}

/* UpdateWires : update the {active,passive}{Component,Port} entries in a wire list
   only update noOfWires wires (if noOfWires == -1 then update all), wires are
   given port numbers from firstPortNo upwards */
void UpdateWires (PtrWireList wires, bool isActive, PtrComponent component, unsigned firstPortNo, int noOfWires)
{
    while (wires && noOfWires != 0)
    {
        UpdateWire (CAR (wires), isActive, component, firstPortNo);
        if (noOfWires != -1)
            noOfWires--;
        firstPortNo++;
        wires = CDR (wires);
    }
}

/* New2PortComponent : bodies of simple N ported components, isActive inputs specify
   which of the component entries in the wires should be updated */
PtrComponent New2PortComponent (tPosition position, ComponentNature nature, bool p1IsActive, PtrWire p1, bool p2IsActive, PtrWire p2)
{
    PtrComponent component = NewComponent (position, nature);

    component->ports = NewWireList (p1, NewWireList (p2, NULL));
    UpdateWire (p1, p1IsActive, component, 0);
    UpdateWire (p2, p2IsActive, component, 1);
    return component;
}

/* New3PortComponent : bodies of simple N ported components */
PtrComponent New3PortComponent (tPosition position, ComponentNature nature, bool p1IsActive, PtrWire p1, bool p2IsActive, PtrWire p2, bool p3IsActive, PtrWire p3)
{
    PtrComponent component = NewComponent (position, nature);

    component->ports = NewWireList (p1, NewWireList (p2, NewWireList (p3, NULL)));
    UpdateWire (p1, p1IsActive, component, 0);
    UpdateWire (p2, p2IsActive, component, 1);
    UpdateWire (p3, p3IsActive, component, 2);
    return component;
}

/* New4PortComponent : bodies of simple N ported components */
PtrComponent
New4PortComponent (tPosition position, ComponentNature nature, bool p1IsActive, PtrWire p1,
  bool p2IsActive, PtrWire p2, bool p3IsActive, PtrWire p3, bool p4IsActive, PtrWire p4)
{
    PtrComponent component = NewComponent (position, nature);

    component->ports = NewWireList (p1, NewWireList (p2, NewWireList (p3, NewWireList (p4, NULL))));
    UpdateWire (p1, p1IsActive, component, 0);
    UpdateWire (p2, p2IsActive, component, 1);
    UpdateWire (p3, p3IsActive, component, 2);
    UpdateWire (p4, p3IsActive, component, 3);
    return component;
}

/* NewComponentParameter : create a component parameter of the desire `nature' and
	put the void * argument into the appropriate place in the returned value */
PtrComponentParameter NewComponentParameter (ComponentParameterNature nature, void *value, tIdent ident, PtrType type)
{
    PtrComponentParameter ret = NEW (ComponentParameter);

    ret->type = type;
    ret->ident = ident;

    switch (nature)
    {
    case TypeComponentParameter:
        ret->value.type = value;
        break;
    case StringComponentParameter:
        ret->value.string = value;
        break;
    case NumberComponentParameter:
        ret->value.number = value;
        break;
    }
    ret->nature = nature;

    return ret;
}

/* NewCommentComponent : create a comment component, includes an identifier and an indication of
   whether this is a begin or end comment */
PtrComponent NewCommentComponent (tPosition position, tIdent ident, bool isBegin)
{
    PtrComponent component = NewComponent (position, CommentComponent);

    component->param.comment.ident = ident;
    component->param.comment.begin = isBegin;
    return component;
}

/* NewProcedureComponent : create a procedure component from the given procedure and ident */
PtrComponent NewProcedureComponent (tPosition position, tIdent ident, PtrProcedure proc)
{
    PtrComponent component = NewComponent (position, ProcedureComponent);

    component->param.procedure.ident = ident;
    component->param.procedure.parameters = NULL;
    component->param.procedure.portSpec = NULL;
    component->param.procedure.ports = NULL;
    component->param.procedure.componentType = NoIdent;
    component->param.procedure.baseComponentName = NoIdent;
    return component;
}

/* NewConstantComponent : create a constant component of value val connected to output
   NB. val becomes the property of the component */
PtrComponent NewConstantComponent (tPosition position, PtrMP_INT val, PtrWire output)
{
    PtrComponent component = NewComponent (position, ConstantComponent);

    /* NB. constants are always positive (bit representations in the size of the output wire) */
    /* Need to fix up the constant before printing */
    component->param.constant.value = val;
    component->ports = NewWireList (output, NULL);
    /* Reverse mapping */
    UpdateWire (output, false /* passive */ , component, 0);
    return component;
}

/* NewStringComponent : create a constant string component of value str connected to output */
PtrComponent NewStringComponent (tPosition position, Ptrchar str, PtrWire output)
{
    PtrComponent component = NewComponent (position, StringComponent);

    component->param.string.string = str;
    component->ports = NewWireList (output, NULL);
    /* Reverse mapping */
    UpdateWire (output, false /* passive */ , component, 0);
    return component;
}

/* NewPassivatorComponent : create a passivator */
PtrComponent NewPassivatorComponent (tPosition position, PtrWire in1, PtrWire in2)
{
    return New2PortComponent (position, PassivatorComponent, false, in1, false, in2);
}

/* NewPassivatorPushComponent : create a passivator */
PtrComponent NewPassivatorPushComponent (tPosition position, PtrWire out, PtrWire in)
{
    return New2PortComponent (position, PassivatorPushComponent, false, out, false, in);
}

/* NewCallComponent : create a call element */
PtrComponent NewCallComponent (tPosition position, PtrWire in1, PtrWire in2, PtrWire out)
{
    return New3PortComponent (position, CallComponent, false, in1, false, in2, true, out);
}

/* NewCallMuxComponent : create a push multiplexing call element */
PtrComponent NewCallMuxComponent (tPosition position, PtrWire in1, PtrWire in2, PtrWire out)
{
    return New3PortComponent (position, CallMuxComponent, false, in1, false, in2, true, out);
}

/* NewCallDemuxComponent : create a pull demultiplexing call element */
PtrComponent NewCallDemuxComponent (tPosition position, PtrWire out1, PtrWire out2, PtrWire in)
{
    return New3PortComponent (position, CallDemuxComponent, false, out1, false, out2, true, in);
}

/* NewSynchComponent : create a synchronising element */
PtrComponent NewSynchComponent (tPosition position, PtrWire in1, PtrWire in2, PtrWire out)
{
    return New3PortComponent (position, SynchComponent, false, in1, false, in2, true, out);
}

/* NewSynchPushComponent : create a push input, push and pull outputs synchronising element */
PtrComponent NewSynchPushComponent (tPosition position, PtrWire in, PtrWire out1, PtrWire out2)
{
    return New3PortComponent (position, SynchPushComponent, false, in, false, out1, true, out2);
}

/* NewSynchPullComponent : create a pull input, dual pull outputs synchronising element */
PtrComponent NewSynchPullComponent (tPosition position, PtrWire out1, PtrWire out2, PtrWire in)
{
    return New3PortComponent (position, SynchPullComponent, false, out1, false, out2, true, in);
}

/* NewSynchPullConnectComponent : create a pull connector */
PtrComponent NewSynchPullConnectComponent (tPosition position, PtrWire out1, PtrWire in)
{
    return New2PortComponent (position, SynchPullComponent, false, out1, true, in);
}

/* NewSynchPullFromListComponent : create a SynchPull with multiple outputs, from a given list */
PtrComponent NewSynchPullFromListComponent (tPosition position, PtrWireList outputs, PtrWire input)
{
    PtrComponent component = NewComponent (position, SynchPullComponent);
    unsigned count = 0;

    component->ports = AppendWireLists (CopyWireList (outputs), NewWireList (input, NULL));
    UpdateWires (outputs, false /* passive */ , component, count, -1);
    UpdateWire (input, true /* Active */ , component, count);
    return component;
}

/* NewSliceComponent : slice channel in into channel out starting at index lowIndex in `in' */
PtrComponent NewSliceComponent (tPosition position, Bits lowIndex, PtrWire out, PtrWire in)
{
    PtrComponent comp = New2PortComponent (position, SliceComponent, false, out, true, in);

    comp->param.slice.lowIndex = lowIndex;
    return comp;
}

/* NewCombineComponent : combine pull channels into one */
PtrComponent NewCombineComponent (tPosition position, PtrWire out, PtrWire lsw, PtrWire msw)
{
    return New3PortComponent (position, CombineComponent, false, out, true, lsw, true, msw);
}

/* NewCombineEqualComponent : combine pull channels into one */
PtrComponent NewCombineEqualComponent (tPosition position, PtrWire out, PtrWireList inp)
{
    PtrComponent component = NewComponent (position, CombineEqualComponent);

    component->ports = NewWireList (out, inp);
    UpdateWire (out, false /* Passive */ , component, 0);
    UpdateWires (inp, true /* Active */ , component, 1, -1);
    return component;
}

/* NewSplitEqualComponent : split a pull channel */
PtrComponent NewSplitEqualComponent (tPosition position, PtrWire inp, PtrWireList out)
{
    PtrComponent component = NewComponent (position, SplitEqualComponent);

    component->ports = NewWireList (inp, out);
    UpdateWire (inp, false /* Passive */ , component, 0);
    UpdateWires (out, true /* Active */ , component, 1, -1);
    return component;
}

/* NewAdaptComponent : create an adapter from sourceSigned signedness to targetSigned signedness */
PtrComponent NewAdaptComponent (tPosition position, bool targetSigned, bool sourceSigned, PtrWire target, PtrWire source)
{
    PtrComponent comp = New2PortComponent (position, AdaptComponent, false, target, true, source);

    comp->param.adapt.outputSigned = targetSigned;
    comp->param.adapt.inputSigned = sourceSigned;
    return comp;
}

/* NewNullAdaptComponent : create an adapter from sourceSigned signedness to sync */
PtrComponent NewNullAdaptComponent (tPosition position, PtrWire target, PtrWire source)
{
    PtrComponent comp = New2PortComponent (position, NullAdaptComponent, true, target, false, source);

    return comp;
}

/* NewUnaryFuncComponent : create a unary function where the input is signed if inSigned is true */
PtrComponent NewUnaryFuncComponent (tPosition position, Operators op, bool inSigned, PtrWire out, PtrWire in)
{
    PtrComponent comp = New2PortComponent (position, UnaryFuncComponent, false, out, true, in);

    comp->param.unary.operation = op;
    comp->param.unary.inputSigned = inSigned;
    return comp;
}

/* NewBinaryFuncComponent : create a binary function where the inputs are signed if leftSigned, rightSigned
   are set, resultSet specifies whether the result is signed */
PtrComponent NewBinaryFuncComponent (tPosition position, Operators op, bool resultSigned, bool leftSigned, bool rightSigned, PtrWire result, PtrWire left, PtrWire right)
{
    PtrComponent comp = New3PortComponent (position, BinaryFuncComponent, false, result, true, left, true, right);

    comp->param.binary.operation = op;
    comp->param.binary.resultSigned = resultSigned;
    comp->param.binary.leftSigned = leftSigned;
    comp->param.binary.rightSigned = rightSigned;
    return comp;
}

/* NewBinaryFuncConstRComponent : create a binary function with constant RHS */
extern PtrComponent
NewBinaryFuncConstRComponent (tPosition position, Operators op, bool resultSigned,
  bool leftSigned, bool rightSigned, Bits rightWidth, PtrMP_INT rightValue, PtrWire result, PtrWire left)
{
    PtrComponent comp = New2PortComponent (position, BinaryFuncConstRComponent, false, result, true, left);

    comp->param.binary.operation = op;
    comp->param.binary.resultSigned = resultSigned;
    comp->param.binary.leftSigned = leftSigned;
    comp->param.binary.rightSigned = rightSigned;
    comp->param.binary.rightWidth = rightWidth;
    comp->param.binary.rightValue = rightValue;
    return comp;
}

/* NewContinueComponent : create a continue component */
PtrComponent NewContinueComponent (tPosition position, PtrWire out)
{
    PtrComponent component = NewComponent (position, ContinueComponent);

    component->ports = NewWireList (out, NULL);
    /* Reverse mapping */
    UpdateWire (out, false /* passive */ , component, 0);
    return component;
}

/* NewContinuePushComponent : create a continue component with a push port */
PtrComponent NewContinuePushComponent (tPosition position, PtrWire out)
{
    PtrComponent component = NewComponent (position, ContinuePushComponent);

    component->ports = NewWireList (out, NULL);
    /* Reverse mapping */
    UpdateWire (out, false /* passive */ , component, 0);
    return component;
}

/* NewHaltComponent : create a halt component */
PtrComponent NewHaltComponent (tPosition position, PtrWire out)
{
    PtrComponent component = NewComponent (position, HaltComponent);

    component->ports = NewWireList (out, NULL);
    /* Reverse mapping */
    UpdateWire (out, false /* passive */ , component, 0);
    return component;
}

/* NewHaltPushComponent : create a halt component with a push port */
PtrComponent NewHaltPushComponent (tPosition position, PtrWire out)
{
    PtrComponent component = NewComponent (position, HaltPushComponent);

    component->ports = NewWireList (out, NULL);
    /* Reverse mapping */
    UpdateWire (out, false /* passive */ , component, 0);
    return component;
}

/* NewForkComponent : create a fork */
PtrComponent NewForkComponent (tPosition position, PtrWire in, PtrWire out1, PtrWire out2)
{
    return New3PortComponent (position, ForkComponent, false, in, true, out1, true, out2);
}

/* NewForkComponentWithWireList : create a fork to many channels */
PtrComponent NewForkComponentWithWireList (tPosition position, PtrWire in, PtrWireList out)
{
    PtrComponent component = NewComponent (position, ForkComponent);

    component->ports = NewWireList (in, out);
    UpdateWire (in, false /* Passive */ , component, 0);
    UpdateWires (out, true /* Active */ , component, 1, -1);

    return component;
}

/* NewConnectComponent : create a fork, with only one connection, a connector */
PtrComponent NewConnectComponent (tPosition position, PtrWire in, PtrWire out1)
{
    return New2PortComponent (position, ForkComponent, false, in, true, out1);
}

/* NewWireForkComponent : create a fork */
PtrComponent NewWireForkComponent (tPosition position, PtrWire in, PtrWire out1, PtrWire out2)
{
    return New3PortComponent (position, WireForkComponent, false, in, true, out1, true, out2);
}

/* NewForkConnectorComponent : create a fork with only one output */
PtrComponent NewForkConnectorComponent (tPosition position, PtrWire in, PtrWire out)
{
    return New2PortComponent (position, ForkComponent, false, in, true, out);
}

/* NewForkPushComponent : create a push demultiplexing fork */
PtrComponent NewForkPushComponent (tPosition position, PtrWire in, PtrWire out1, PtrWire out2)
{
    return New3PortComponent (position, ForkPushComponent, false, in, true, out1, true, out2);
}

/* NewCallDemuxPushComponent : create a push demultiplexing call */
PtrComponent NewCallDemuxPushComponent (tPosition position, PtrWire in, PtrWire out1, PtrWire out2)
{
    return New3PortComponent (position, CallDemuxPushComponent, false, in, true, out1, true, out2);
}

/* NewCallActiveComponent : create a forked req, called ack component */
PtrComponent NewCallActiveComponent (tPosition position, PtrWire in, PtrWire out1, PtrWire out2)
{
    return New3PortComponent (position, CallActiveComponent, false, in, true, out1, true, out2);
}

/* NewConcurComponent : create a parallelising component */
PtrComponent NewConcurComponent (tPosition position, PtrWire act, PtrWire actout1, PtrWire actout2)
{
    return New3PortComponent (position, ConcurComponent, false, act, true, actout1, true, actout2);
}

/* NewSequenceComponent : create a sequencing component with specString (generated during the optimisation stage from left and right accesses) for optimised backend */
PtrComponent
NewSequenceComponent (tPosition position, PtrWire act, PtrWire left, PtrWire right)
{
    return New3PortComponent (position, SequenceComponent, false, act, true, left, true, right);
}

/* NewVariableComponent : create a variable component (from the balsa variable name at offset offset),
   with possibly many read ports */
PtrComponent NewVariableComponent (tPosition position, Ptrchar name, Bits offset, Bits width, PtrWire write, PtrWireList reads)
{
    PtrComponent component = NewComponent (position, VariableComponent);

    component->param.variable.name = name;
    component->param.variable.offset = offset;
    component->param.variable.width = width;
    component->param.variable.initialisedValue = NULL; /* Just to make sure */
    component->param.variable.readPortsSpecString = NEW_ARRAY (char, 1);
    component->param.variable.readPortsSpecString[0] = 0;
    component->ports = NewWireList (write, reads);
    UpdateWire (write, false /* Passive */ , component, 0);
    UpdateWires (reads, false /* Passive */ , component, 1, -1);

    return component;
}

/* NewInitVariableComponent : create an initialised variable component (from the balsa variable name at offset offset), 
   with possibly many read ports */
PtrComponent NewInitVariableComponent (tPosition position, Ptrchar name, Bits offset, Bits width, PtrMP_INT initValue, PtrWire init, PtrWire write, PtrWireList reads)
{
    PtrComponent component = NewComponent (position, InitVariableComponent);

    component->param.variable.name = name;
    component->param.variable.offset = offset;
    component->param.variable.width = width;
    component->param.variable.initialisedValue = initValue;
    component->param.variable.readPortsSpecString = NEW_ARRAY (char, 1);
    component->param.variable.readPortsSpecString[0] = 0;
    component->ports = NewWireList (write, NewWireList (init, reads));
    UpdateWire (write, false /* Passive */ , component, 0);
    UpdateWire (write, false /* Passive */ , component, 1);
    UpdateWires (reads, false /* Passive */ , component, 2, -1);

    return component;
}

/* NewFalseVariableComponent : create a false variable component, with possibly many read ports */
PtrComponent NewFalseVariableComponent (tPosition position, Bits offset, Bits width, PtrWire write, PtrWire activateOut, PtrWireList reads)
{
    PtrComponent component = NewComponent (position, FalseVariableComponent);

    component->param.variable.offset = offset;
    component->param.variable.width = width;
    component->param.variable.readPortsSpecString = NEW_ARRAY (char, 1);
    component->param.variable.readPortsSpecString[0] = 0;
    component->ports = NewWireList (write, NewWireList (activateOut, reads));
    UpdateWire (write, false /* Passive */ , component, 0);
    UpdateWire (activateOut, true /* Active */ , component, 1);
    UpdateWires (reads, false /* Passive */ , component, 2, -1);
    return component;
}

/* NewPassiveEagerFalseVariableComponent : create a passive eager false variable component, with possibly many read ports */
PtrComponent NewPassiveEagerFalseVariableComponent (tPosition position, Bits offset, Bits width, PtrWire activate, PtrWire write, PtrWire activateOut, PtrWireList reads)
{
    PtrComponent component = NewComponent (position, PassiveEagerFalseVariableComponent);

    component->param.variable.offset = offset;
    component->param.variable.width = width;
    component->param.variable.readPortsSpecString = NEW_ARRAY (char, 1);
    component->param.variable.readPortsSpecString[0] = 0;
    component->ports = NewWireList (activate, NewWireList (write, NewWireList (activateOut, reads)));
    UpdateWire (activate, false /* Passive */ , component, 0);
    UpdateWire (write, false /* Passive */ , component, 1);
    UpdateWire (activateOut, true /* Active */ , component, 2);
    UpdateWires (reads, false /* Passive */ , component, 3, -1);
    return component;
}

/* NewActiveEagerFalseVariableComponent : create an active eager false variable component, with possibly many read ports */
PtrComponent NewActiveEagerFalseVariableComponent (tPosition position, Bits offset, Bits width, PtrWire activate, PtrWire write, PtrWire activateOut, PtrWireList reads)
{
    PtrComponent component = NewComponent (position, ActiveEagerFalseVariableComponent);

    component->param.variable.offset = offset;
    component->param.variable.width = width;
    component->param.variable.readPortsSpecString = NEW_ARRAY (char, 1);
    component->param.variable.readPortsSpecString[0] = 0;
    component->ports = NewWireList (activate, NewWireList (write, NewWireList (activateOut, reads)));
    UpdateWire (activate, false /* Passive */ , component, 0);
    UpdateWire (write, true /* Active */ , component, 1);
    UpdateWire (activateOut, true /* Active */ , component, 2);
    UpdateWires (reads, false /* Passive */ , component, 3, -1);
    return component;
}

/* NewPassiveSyncEagerFalseVariableComponent : create a passive sync eager false variable component */
PtrComponent NewPassiveSyncEagerFalseVariableComponent (tPosition position, PtrWire activate, PtrWire write, PtrWire activateOut)
{
    return New3PortComponent (position, PassiveSyncEagerFalseVariableComponent, false, activate, false, write, true, activateOut);
}

/* NewPassiveEagerNullAdaptComponent : create a passive eager nullAdapt component, with possibly many read ports */
PtrComponent NewPassiveEagerNullAdaptComponent (tPosition position, PtrWire activate, PtrWire write, PtrWire activateOut)
{
    return New3PortComponent (position, PassiveEagerNullAdaptComponent, false, activate, false, write, true, activateOut);
}

/* NewActiveEagerNullAdaptComponent : create an active eager nullAdapt component, with possibly many read ports */
PtrComponent NewActiveEagerNullAdaptComponent (tPosition position, PtrWire activate, PtrWire write, PtrWire activateOut)
{
    return New3PortComponent (position, ActiveEagerNullAdaptComponent, false, activate, true, write, true, activateOut);
}

/* NewDecisionWaitComponent : create a decision wait component */
PtrComponent NewDecisionWaitComponent (tPosition position, PtrWire activate, PtrWireList inps, PtrWireList outs)
{
    PtrComponent component = NewComponent (position, DecisionWaitComponent);
    unsigned inputCount = LengthOfWireList (inps);

    ASSERT (inputCount == LengthOfWireList (outs));
    UpdateWire (activate, false /* Passive */ , component, 0);
    UpdateWires (inps, false /* Passive */ , component, 1, -1);
    UpdateWires (outs, true /* Active */ , component, 1 + inputCount, -1);
    component->ports = NewWireList (activate, AppendWireLists (inps, outs));
    return component;
}

/* NewSplitComponent : create a channel splitting component */
PtrComponent NewSplitComponent (tPosition position, PtrWire in, PtrWire lsw, PtrWire msw)
{
    return New3PortComponent (position, SplitComponent, false, in, true, lsw, true, msw);
}

/* NewLoopComponent : create an infinate loop */
PtrComponent NewLoopComponent (tPosition position, PtrWire in, PtrWire out)
{
    return New2PortComponent (position, LoopComponent, false, in, true, out);
}

/* NewFetchComponent : create a fetch component */
PtrComponent NewFetchComponent (tPosition position, bool outBroad, PtrWire act, PtrWire in, PtrWire out)
{
    PtrComponent comp = New3PortComponent (position, FetchComponent, false, act, true, in, true, out);

    comp->param.fetch.outBroad = outBroad;
    return comp;
}

/* NewBarComponent : create an n (length of guards/commands lists) way if/while guard */
PtrComponent NewBarComponent (tPosition position, PtrWire guardOut, PtrWire activate, PtrWireList guards, PtrWireList commands)
{
    PtrComponent component = NewComponent (position, BarComponent);
    PtrWireList guardsAndCommands = AppendWireLists (CopyWireList (guards), commands);

    component->ports = NewWireList (guardOut, NewWireList (activate, guardsAndCommands));
    UpdateWire (guardOut, false /* Passive */ , component, 0);
    UpdateWire (activate, false /* Passive */ , component, 0);
    UpdateWires (guardsAndCommands, true /* Active */ , component, 2, -1);
    return component;
}

/* NewWhileComponent : create a while component, without an else port */
PtrComponent NewWhileComponent (tPosition position, PtrWire act, PtrWire guard, PtrWire thenAct)
{
    return New3PortComponent (position, WhileComponent, false, act, true, guard, true, thenAct);
}

/* NewArbiterComponent : create a arbiter component */
PtrComponent NewArbiterComponent (tPosition position, PtrWire leftIn, PtrWire rightIn, PtrWire leftOut, PtrWire rightOut)
{
    return New4PortComponent (position, ArbiterComponent, false, leftIn, false, rightIn, true, leftOut, true, rightOut);
}

/* NewCaseComponent : create a case component from an activation channel and a list of output activations */
PtrComponent NewCaseComponent (tPosition position, Ptrchar specString, PtrWire activation, PtrWireList outActs)
{
    PtrComponent component = NewComponent (position, CaseComponent);

    component->param.casecomp.specString = specString;
    component->ports = NewWireList (activation, outActs);
    UpdateWire (activation, false /* Passive */ , component, 0);
    UpdateWires (outActs, true /* Active */ , component, 1, -1);
    return component;
}

/* NewEncodeComponent : create an encoder from a list of input activations and an output channel */
PtrComponent NewEncodeComponent (tPosition position, Ptrchar specString, PtrWireList inps, PtrWire out)
{
    PtrComponent component = NewComponent (position, EncodeComponent);
    unsigned inputCount = LengthOfWireList (inps);

    component->param.casecomp.specString = specString;
    component->ports = AppendWireLists (CopyWireList (inps), NewWireList (out, NULL));
    UpdateWires (inps, false /* Passive */ , component, 0, inputCount);
    UpdateWire (out, true /* Active */ , component, inputCount);
    return component;
}

/* NewCaseFetchComponent : create a case fetch component from an activation output channel, an index
   input channel and a list of input channels */
PtrComponent NewCaseFetchComponent (tPosition position, Ptrchar specString, PtrWire out, PtrWire index, PtrWireList inp)
{
    PtrComponent component = NewComponent (position, CaseFetchComponent);

    component->param.casecomp.specString = specString;
    component->ports = NewWireList (out, NewWireList (index, inp));
    UpdateWire (out, false /* Passive */ , component, 0);
    UpdateWire (index, true /* Active */ , component, 1);
    UpdateWires (inp, true /* Active */ , component, 2, -1);
    return component;
}

/* ---------- storcurtsnoc tnenopmoC */

DEFINE_CONS_LIST_TYPE (Component) DEFINE_CONS_LIST_DEEP_COPY (Component) DEFINE_CONS_LIST_TYPE (ComponentParameter)
/* MarkComponent : mark a component as having nature NoComponent, this makes the component eligible for
   sweeping by SweepComponentList */
void MarkComponent (PtrComponent comp)
{
    comp->nature = NoComponent;
}

/* SweepComponentList : remove elements of the given list which have nature NoComponent */
PtrComponentList SweepComponentList (PtrComponentList list)
{
    PtrComponentList retHead = NULL;
    PtrComponentList retTail = NULL;

    while (list)
    {
        if (CAR (list)->nature != NoComponent) /* Link this element */
        {
            if (retHead)
            {
                retTail->next = list;
                retTail = list;
            } else
            {
                retHead = retTail = list;
            }
        }
        list = CDR (list);
    }
    if (retHead)
        retTail->next = NULL;

    return retHead;
}

/* CopyComponent : Copy a component, shallow copying the port lists */
PtrComponent CopyComponent (PtrComponent comp)
{
    PtrComponent newComp = NewComponent (NoPosition, CommentComponent);

    *newComp = *comp;
    newComp->ports = CopyWireList (comp->ports);
    return newComp;
}

/* StrComponentNature : Print the name of a component on stream */
void StrComponentNature (FILE * stream, ComponentNature nature)
{
    fprintf (stream, "%s", BreezeComponents[(int) nature].name);
}

/* StrValuesOfComponentParameterList : print a parameter list held in the unprocessed format */
void StrValuesOfComponentParameterList (FILE * stream, PtrComponentParameterList parameters, Ptrchar separator)
{
    while (parameters)
    {
        switch (CAR (parameters)->nature)
        {
        case TypeComponentParameter:
            StrPtrBreezeTopLevelType (stream, CAR (parameters)->value.type);
            break;
        case StringComponentParameter:
            fprintf (stream, "\"%s\"", CAR (parameters)->value.string);
            break;
        case NumberComponentParameter:
            StrPtrMP_INT (stream, CAR (parameters)->value.number);
            break;
        }

        parameters = CDR (parameters);
        if (parameters)
            fprintf (stream, "%s", separator);
    }
}

/* StrBreezeComponentParameterList : print Breeze undeclared-component parameter declaration
	list from the incidental ident and type into in a ComponentParameterList */
void StrBreezeComponentParameterList (FILE * stream, PtrComponentParameterList parameters, Ptrchar separator)
{
    while (parameters)
    {
        switch (CAR (parameters)->nature)
        {
        case TypeComponentParameter:
            fprintf (stream, "(type-parameter \"");
            WriteIdent (stream, CAR (parameters)->ident);
            fprintf (stream, "\"");
            break;
        case StringComponentParameter:
            fprintf (stream, "(parameter \"");
            WriteIdent (stream, CAR (parameters)->ident);
            fprintf (stream, "\" ");
            fprintf (stream, "(named-type \"String\")");
            break;
        case NumberComponentParameter:
            fprintf (stream, "(parameter \"");
            WriteIdent (stream, CAR (parameters)->ident);
            fprintf (stream, "\" ");
            {
                PtrType type = CAR (parameters)->type;

                if (!(type->scope & TOP_LEVEL_SCOPES))
                    StrPtrSBreezeType (stream, type, false, true);
                else
                    StrPtrSBreezeTypeName (stream, type);
            }
            break;
        }
        putc (')', stream);

        parameters = CDR (parameters);
        if (parameters)
            fprintf (stream, "%s", separator);
    }
}

/* StrParamSeparator : print the parameter/port separator */
#define StrParamSeparator(stream) fprintf ((stream), " : ")

/* StrBreezeComponentParameters : print out the parameters of a component, returns true
   if a parameter separator is required, false otherwise. */
bool StrBreezeComponentParameters (FILE * stream, PtrComponent component)
{
    bool insertSeparator = true;

    if (component)
    {
        PtrWireList ports = component->ports;
        unsigned portCount = LengthOfWireList (ports);

        switch (component->nature)
        {
        /* OK */ case NoComponent:
            insertSeparator = false;
            break;
        /* OK */ case CommentComponent: /* Begin or End comment */
            fprintf (stream, ";;; ! %s ", (component->param.comment.begin ? "begin" : "end"));
            WriteIdent (stream, component->param.comment.ident);
            insertSeparator = false;
            break;
        /* OK */ case ConstantComponent: /* Constant ( val ) */
            fprintf (stream, "%d ", WireWidth (CAR (ports)));
            StrPtrMP_INT (stream, FilterRangeFromMP_INT (component->param.constant.value, WireWidth (CAR (ports)), 0));
            break;
        /* OK */ case SliceComponent:
            fprintf (stream, "%d %d %d", WireWidth (CAR (ports)), WireWidth (CADR (ports)), component->param.slice.lowIndex);
            break;
        /* OK */ case AdaptComponent:
            fprintf (stream, "%d %d %s %s", WireWidth (CAR (ports)),
              WireWidth (CADR (ports)), BOOL_RAW_STRING (component->param.adapt.outputSigned), BOOL_RAW_STRING (component->param.adapt.inputSigned));
            break;
        /* OK */ case NullAdaptComponent:
            fprintf (stream, "%d", WireWidth (CADR (ports)));
            break;
        /* OK */ case UnaryFuncComponent:
        /* OK */ case UnaryFuncPushComponent:
            fprintf (stream, "%d %d ", WireWidth (CAR (ports)), WireWidth (CADR (ports)));
            StrOperators (stream, component->param.unary.operation);
            fprintf (stream, " %s", BOOL_RAW_STRING (component->param.unary.inputSigned));
            break;
        /* OK */ case BinaryFuncComponent:
        /* OK */ case BinaryFuncPushComponent:
            fprintf (stream, "%d %d %d ", WireWidth (CAR (ports)), WireWidth (CADR (ports)), WireWidth (CADDR (ports)));
            StrOperators (stream, component->param.binary.operation);
            fprintf (stream, " %s %s %s",
              BOOL_RAW_STRING (component->param.binary.resultSigned),
              BOOL_RAW_STRING (component->param.binary.leftSigned), BOOL_RAW_STRING (component->param.binary.rightSigned));
            break;
        /* OK */ case BinaryFuncConstRComponent:
        /* OK */ case BinaryFuncConstRPushComponent:
            fprintf (stream, "%d %d %d ", WireWidth (CAR (ports)), WireWidth (CADR (ports)), component->param.binary.rightWidth);
            StrOperators (stream, component->param.binary.operation);
            fprintf (stream, " %s %s %s ",
              BOOL_RAW_STRING (component->param.binary.resultSigned),
              BOOL_RAW_STRING (component->param.binary.leftSigned), BOOL_RAW_STRING (component->param.binary.rightSigned));
            StrPtrMP_INT (stream, FilterRangeFromMP_INT (component->param.binary.rightValue, component->param.binary.rightWidth, 0));
            break;
        /* OK */ case DecisionWaitComponent:
            fprintf (stream, "%d", portCount / 2);
            break;
        /* OK */ case VariableComponent:
            fprintf (stream, "%d %d \"%s\" \"%s\"", WireWidth (CAR (ports)), portCount - 1, component->param.variable.name,
              component->param.variable.readPortsSpecString);
            break;
        /* OK */ case InitVariableComponent:
            fprintf (stream, "%d %d ", WireWidth (CAR (ports)), portCount - 2);
            StrPtrMP_INT (stream, component->param.variable.initialisedValue);
            fprintf (stream, " \"%s\" \"%s\"", component->param.variable.name, component->param.variable.readPortsSpecString);
            break;
        /* OK */ case BuiltinVariableComponent:
            fprintf (stream, "%d \"%s\"", portCount - 1, component->param.variable.name);
            break;
        /* OK */ case FalseVariableComponent:
            fprintf (stream, "%d %d \"%s\"", WireWidth (CAR (ports)), portCount - 2, component->param.variable.readPortsSpecString);
            break;
        /* OK */ case PassiveEagerFalseVariableComponent:
        /* OK */ case ActiveEagerFalseVariableComponent:
            fprintf (stream, "%d %d \"%s\"", WireWidth (CADR (ports)), portCount - 3, component->param.variable.readPortsSpecString);
            break;
        /* OK */ case PassiveEagerNullAdaptComponent:
        /* OK */ case ActiveEagerNullAdaptComponent:
            fprintf (stream, "%d", WireWidth (CADR (ports)));
            break;
        /* OK */ case SynchPushComponent:
            fprintf (stream, "%d %d", WireWidth (CAR (ports)), portCount - 2);
            break;
        /* OK */ case CallMuxComponent:
        /* OK */ case CallDemuxComponent:
        /* OK */ case SynchPullComponent:
            fprintf (stream, "%d %d", WireWidth (CAR (ports)), portCount - 1);
            break;
        /* OK */ case ContinuePushComponent:
        /* OK */ case HaltPushComponent:
            fprintf (stream, "%d", WireWidth (CAR (ports)));
            break;
        /* OK */ case ForkPushComponent:
        /* OK */ case CallDemuxPushComponent:
        /* OK */ case PassivatorPushComponent:
            fprintf (stream, "%d %d", WireWidth (CAR (ports)), portCount - 1);
            break;
        /* OK */ case PassivatorComponent:
            fprintf (stream, "%d", portCount);
            break;
        /* OK */ case FetchComponent:
            fprintf (stream, "%d %s", WireWidth (CADR (ports)), BOOL_RAW_STRING (component->param.fetch.outBroad));
            break;
        /* OK */ case CaseComponent:
            fprintf (stream, "%d %d \"%s\"", WireWidth (CAR (ports)), portCount - 1, component->param.casecomp.specString);
            break;
        /* OK */ case EncodeComponent:
            fprintf (stream, "%d %d \"%s\"", WireWidth (NthOfWireList (ports, -1)), portCount - 1, component->param.casecomp.specString);
            break;
        /* OK */ case CaseFetchComponent:
            fprintf (stream, "%d %d %d \"%s\"", WireWidth (CAR (ports)),
              WireWidth (CADR (ports)), portCount - 2, component->param.casecomp.specString);
            break;
        /* OK */ case CombineComponent:
        /* OK */ case SplitComponent:
            fprintf (stream, "%d %d %d", WireWidth (CAR (ports)), WireWidth (CADR (ports)), WireWidth (CADDR (ports)));
            break;
            /* ( n : passive_sync, [n x active_syncs] ) */
        /* OK */ case ConcurComponent:
        /* OK */ case ForkComponent:
        /* OK */ case WireForkComponent:
        /* OK */ case CallActiveComponent:
            fprintf (stream, "%d", portCount - 1);
            break;
        /* OK */ case CallComponent:
        /* OK */ case SynchComponent:
            fprintf (stream, "%d", portCount - 1);
            break;
        /* OK */ case ProcedureComponent:
            StrValuesOfComponentParameterList (stream, component->param.procedure.parameters, " ");
            break;
            /* Unparameterised */
        /* OK */ case LoopComponent:
        /* OK */ case WhileComponent:
        /* OK */ case ContinueComponent:
        /* OK */ case HaltComponent:
        /* OK */ case ArbiterComponent:
        /* OK */ case PassiveSyncEagerFalseVariableComponent:
            insertSeparator = false;
            break;
        /* OK */ case BarComponent:
            fprintf (stream, "%d", (portCount / 2) - 1);
            break;
        /* OK */ case CombineEqualComponent:
        /* OK */ case SplitEqualComponent:
            fprintf (stream, "%d %d %d", WireWidth (CAR (ports)), WireWidth (CADR (ports)), portCount - 1);
            break;
        /* OK */ case StringComponent:
            fprintf (stream, "\"%s\"", component->param.string.string);
            break;
        /* OK */ case SequenceComponent:
            if (!component->param.sequence.specString)
                component->param.sequence.specString = g_strnfill (portCount - 2, 'S');
            else if (strlen (component->param.sequence.specString) != portCount - 2)
            {
                fprintf (stderr, "Warning: Compiler error in sequence. Please report. %s %d\n",
                  component->param.sequence.specString, portCount);
//     LOG_ERROR (ExpectedTokens, NoIdent, NoPosition);
            }
            fprintf (stream, "%d \"%s\"", portCount - 1, component->param.sequence.specString);
            break;
        case LastComponent:
            insertSeparator = false;
            break;
        }
    }
    return insertSeparator;
}

/* StrBreezeProcedurePorts : print out the given ProcedureComponent's port bundles with the right
   channel array groupings */
void
StrBreezeProcedurePorts (FILE * stream, PtrComponent component, bool bundleNumberFormat, Ptrchar separator, Ptrchar openBracket, Ptrchar closeBracket)
{
    ASSERT (component->nature == ProcedureComponent);
    {
        PtrInstanceList formalPorts = component->param.procedure.ports;
        PtrWireList actualPorts = component->ports;

/*
		{
			WriteIdent (stderr, component->param.procedure.ident);
			fprintf (stderr, ": ");
			PtrInstanceList i = formalPorts;
			while (i)
			{
				StrPtrInstance (stderr, CAR (i));
				fprintf (stderr, "\n");
				i = CDR (i);
			}
			fprintf (stderr, "\n");
		}
*/
        while (formalPorts)
        {
            if (CAR (formalPorts)->type->nature == ArrayedType)
            {
                fprintf (stream, "%s", openBracket);
                actualPorts =
                  StrNElementsOfPtrWireList (stream,
                  CAR (formalPorts)->type->info.arrayed.elementCount, actualPorts, false, separator, bundleNumberFormat);
                fprintf (stream, "%s", closeBracket);
            } else
                actualPorts = StrNElementsOfPtrWireList (stream, 1, actualPorts, false, "", bundleNumberFormat);
            formalPorts = CDR (formalPorts);
            if (formalPorts)
                fprintf (stream, "%s", separator);
        }
    }

    /* Handled undeclared-components */
    if (component->param.procedure.parameters)
    {
        fprintf (stream, ")\n      ");
        fprintf (stream, "(implements \"");
        WriteIdent (stream, component->param.procedure.componentType);
        fprintf (stream, "\" \"");
        WriteIdent (stream, component->param.procedure.baseComponentName);
        fprintf (stream, "\")\n      ");
        fprintf (stream, "(parameters\n        ");
        StrBreezeComponentParameterList (stream, component->param.procedure.parameters, "\n        ");
        fprintf (stream, "\n      )\n      ");
        fprintf (stream, "(ports\n        ");
        /* StrBreezePortOptions (stream, proc->activationOptions, proc->position); */
        StrPtrBreezePortList (stream, component->param.procedure.ports, "\n        ");
        fprintf (stream, "\n      ");
    }
}

/* StrBreezeComponentPorts : print out the ports of a component, if bundleNumberFormat is true
   then print #3,#4 etc. instead of 3,4. separator specifies the string to be used to separate
   port bundles/numbers, {open,close}Bracket specify the strings to use as open/close
   brackets for printing wire lists ({} for Breeze, () for SBreeze) */
void
StrBreezeComponentPorts (FILE * stream, PtrComponent component, bool bundleNumberFormat, Ptrchar separator, Ptrchar openBracket, Ptrchar closeBracket)
{
    if (component)
    {
        PtrWireList remainingPorts = component->ports;
        unsigned portCount = LengthOfWireList (remainingPorts);

        switch (component->nature)
        {
            /* No ports */
        /* OK */ case NoComponent:
        /* OK */ case CommentComponent: /* Begin or End comment */
            remainingPorts = NULL;
            break;
            /* No special ports */
        /* OK */ case ConstantComponent: /* Constant ( val ) */
        /* OK */ case SliceComponent:
        /* OK */ case AdaptComponent:
        /* OK */ case NullAdaptComponent:
        /* OK */ case UnaryFuncComponent:
        /* OK */ case BinaryFuncComponent:
        /* OK */ case BinaryFuncConstRComponent:
        /* OK */ case UnaryFuncPushComponent:
        /* OK */ case BinaryFuncPushComponent:
        /* OK */ case BinaryFuncConstRPushComponent:
        /* OK */ case ContinuePushComponent:
        /* OK */ case HaltPushComponent:
        /* OK */ case FetchComponent:
        /* OK */ case CombineComponent:
        /* OK */ case SplitComponent:
        /* OK */ case LoopComponent:
        /* OK */ case WhileComponent:
        /* OK */ case ContinueComponent:
        /* OK */ case HaltComponent:
        /* OK */ case ArbiterComponent:
        /* OK */ case StringComponent:
        /* OK */ case PassiveEagerNullAdaptComponent:
        /* OK */ case ActiveEagerNullAdaptComponent:
        case LastComponent:
            break;
        /* OK */ case DecisionWaitComponent:
            remainingPorts = StrNElementsOfPtrWireList (stream, 1, remainingPorts, false, "", bundleNumberFormat);
            fprintf (stream, "%s", separator);
            fprintf (stream, "%s", openBracket);
            remainingPorts = StrNElementsOfPtrWireList (stream, portCount / 2, remainingPorts, false, separator, bundleNumberFormat);
            fprintf (stream, "%s", closeBracket);
            fprintf (stream, "%s", separator);
            fprintf (stream, "%s", openBracket);
            StrPtrWireList (stream, remainingPorts, false, separator, bundleNumberFormat);
            fprintf (stream, "%s", closeBracket);
            remainingPorts = NULL;
            break;
        /* OK */ case FalseVariableComponent:
            remainingPorts = StrNElementsOfPtrWireList (stream, 2, remainingPorts, false, separator, bundleNumberFormat);
            fprintf (stream, "%s", separator);
            fprintf (stream, "%s", openBracket);
            StrPtrWireList (stream, remainingPorts, false, separator, bundleNumberFormat);
            fprintf (stream, "%s", closeBracket);
            remainingPorts = NULL;
            break;
        /* OK */ case PassiveEagerFalseVariableComponent:
        /* OK */ case ActiveEagerFalseVariableComponent:
            remainingPorts = StrNElementsOfPtrWireList (stream, 3, remainingPorts, false, separator, bundleNumberFormat);
            fprintf (stream, "%s", separator);
            fprintf (stream, "%s", openBracket);
            StrPtrWireList (stream, remainingPorts, false, separator, bundleNumberFormat);
            fprintf (stream, "%s", closeBracket);
            remainingPorts = NULL;
            break;
        /* OK */ case PassiveSyncEagerFalseVariableComponent:
            remainingPorts = StrNElementsOfPtrWireList (stream, 3, remainingPorts, false, separator, bundleNumberFormat);
            remainingPorts = NULL;
            break;
        /* OK */ case SynchPushComponent:
            remainingPorts = StrNElementsOfPtrWireList (stream, 1, remainingPorts, false, "", bundleNumberFormat);
            fprintf (stream, "%s", separator);
            fprintf (stream, "%s", openBracket);
            remainingPorts = StrNElementsOfPtrWireList (stream, portCount - 2, remainingPorts, false, separator, bundleNumberFormat);
            fprintf (stream, "%s", closeBracket);
            fprintf (stream, "%s", separator);
            StrPtrWireList (stream, remainingPorts, false, "", bundleNumberFormat);
            remainingPorts = NULL;
            break;
        /* OK */ case CallMuxComponent:
        /* OK */ case CallDemuxComponent:
        /* OK */ case SynchPullComponent:
        /* OK */ case PassivatorPushComponent:
        /* OK */ case EncodeComponent:
            fprintf (stream, "%s", openBracket);
            remainingPorts = StrNElementsOfPtrWireList (stream, portCount - 1, remainingPorts, false, separator, bundleNumberFormat);
            fprintf (stream, "%s", closeBracket);
            fprintf (stream, "%s", separator);
            StrPtrWireList (stream, remainingPorts, false, "", bundleNumberFormat);
            remainingPorts = NULL;
            break;
        /* OK */ case ForkPushComponent:
        /* OK */ case CallDemuxPushComponent:
            remainingPorts = StrNElementsOfPtrWireList (stream, 1, remainingPorts, false, "", bundleNumberFormat);
            fprintf (stream, "%s", separator);
            fprintf (stream, "%s", openBracket);
            StrPtrWireList (stream, remainingPorts, false, separator, bundleNumberFormat);
            fprintf (stream, "%s", closeBracket);
            remainingPorts = NULL;
            break;
        /* OK */ case PassivatorComponent:
            fprintf (stream, "%s", openBracket);
            StrPtrWireList (stream, remainingPorts, false, separator, bundleNumberFormat);
            fprintf (stream, "%s", closeBracket);
            remainingPorts = NULL;
            break;
        /* OK */ case CaseComponent:
            remainingPorts = StrNElementsOfPtrWireList (stream, 1, remainingPorts, false, "", bundleNumberFormat);
            fprintf (stream, "%s", separator);
            fprintf (stream, "%s", openBracket);
            StrPtrWireList (stream, remainingPorts, false, separator, bundleNumberFormat);
            fprintf (stream, "%s", closeBracket);
            remainingPorts = NULL;
            break;
        /* OK */ case CaseFetchComponent:
        /* OK */ case InitVariableComponent:
            remainingPorts = StrNElementsOfPtrWireList (stream, 2, remainingPorts, false, separator, bundleNumberFormat);
            fprintf (stream, "%s", separator);
            fprintf (stream, "%s", openBracket);
            StrPtrWireList (stream, remainingPorts, false, separator, bundleNumberFormat);
            fprintf (stream, "%s", closeBracket);
            remainingPorts = NULL;
            break;
        /* OK */ case ConcurComponent:
        /* OK */ case SequenceComponent:
        /* OK */ case ForkComponent:
        /* OK */ case WireForkComponent:
        /* OK */ case CallActiveComponent:
        /* OK */ case VariableComponent:
        /* OK */ case BuiltinVariableComponent:
        /* OK */ case CombineEqualComponent:
        /* OK */ case SplitEqualComponent:
            remainingPorts = StrNElementsOfPtrWireList (stream, 1, remainingPorts, false, "", bundleNumberFormat);
            fprintf (stream, "%s", separator);
            fprintf (stream, "%s", openBracket);
            StrPtrWireList (stream, remainingPorts, false, separator, bundleNumberFormat);
            fprintf (stream, "%s", closeBracket);
            remainingPorts = NULL;
            break;
        /* OK */ case CallComponent:
        /* OK */ case SynchComponent:
            fprintf (stream, "%s", openBracket);
            remainingPorts = StrNElementsOfPtrWireList (stream, portCount - 1, remainingPorts, false, separator, bundleNumberFormat);
            fprintf (stream, "%s", closeBracket);
            fprintf (stream, "%s", separator);
            StrPtrWireList (stream, remainingPorts, false, "", bundleNumberFormat);
            remainingPorts = NULL;
            break;
            /* Unparameterised */
            break;
        /* OK */ case BarComponent:
            remainingPorts = StrNElementsOfPtrWireList (stream, 2, remainingPorts, false, separator, bundleNumberFormat);
            fprintf (stream, "%s", separator);
            fprintf (stream, "%s", openBracket);
            remainingPorts = StrNElementsOfPtrWireList (stream, (portCount / 2) - 1, remainingPorts, false, separator, bundleNumberFormat);
            fprintf (stream, "%s", closeBracket);
            fprintf (stream, "%s", separator);
            fprintf (stream, "%s", openBracket);
            StrPtrWireList (stream, remainingPorts, false, separator, bundleNumberFormat);
            fprintf (stream, "%s", closeBracket);
            remainingPorts = NULL;
            break;
        /* OK */ case ProcedureComponent:
            StrBreezeProcedurePorts (stream, component, bundleNumberFormat, separator, openBracket, closeBracket);
            remainingPorts = NULL;
            break;
        }
        StrPtrWireList (stream, remainingPorts, false /* just the bundleNo */ ,
          separator, bundleNumberFormat);
    }
}

/* StrPtrComponent : Print a component description on stream */
void StrPtrComponent (FILE * stream, PtrComponent component)
{
    if (!component)
        fprintf (stream, "NULL");
    else
    {
        if (component->nature != ProcedureComponent)
            StrComponentNature (stream, component->nature);
        else
            WriteIdent (stream, component->param.procedure.ident);
        fprintf (stream, " ( ");

        if (StrBreezeComponentParameters (stream, component))
            StrParamSeparator (stream);
        StrBreezeComponentPorts (stream, component, true, ",", "{", "}");

        fprintf (stream, " )");
    }
}

/* StrPtrSBreezeComponent : Print a component description in the SBreeze format on stream.
	if componentNo is >= 0, then print a comment at the end of the component with
	that component number  */
void StrPtrSBreezeComponent (FILE * stream, PtrComponent component, int componentNo)
{
    if (!component)
        fprintf (stream, "()");
    else
    {
        if (component->nature == ProcedureComponent)
        {
            fprintf (stream, "(%s \"", (component->param.procedure.parameters ? "undeclared-component" : "component"));
            WriteIdent (stream, component->param.procedure.ident);
        } else
        {
            fprintf (stream, "(component \"");
            StrComponentNature (stream, component->nature);
        }
        fprintf (stream, "\" (");
        StrBreezeComponentParameters (stream, component);
        fprintf (stream, ") (");
        StrBreezeComponentPorts (stream, component, false, " ", "(", ")");
        fprintf (stream, ")");
        if (component->position.Line != 0)
        {
            fprintf (stream, " ");
            StrSBreezePosition (stream, component->position);
        }
        if (component->options)
        {
            fprintf (stream, " ");
            StrPtrLispList (stream, component->options, " ", " ");
        }
        fprintf (stream, ")");
        if (componentNo >= 0)
            fprintf (stream, " ; %d", componentNo);
    }
}

/* StrPtrSBreezeComponentList : print a list of abridged component descriptions in the SBreeze format */
void StrPtrSBreezeComponentList (FILE * stream, PtrComponentList list, Ptrchar separator)
{
    int componentNo = 0;

    while (list)
    {
        StrPtrSBreezeComponent (stream, CAR (list), componentNo);
        componentNo++;
        list = CDR (list);
        if (list)
            fprintf (stream, "%s", separator);
    }
}
