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

	`components.h'
	Handshake components list type
	and manipulation
	
 */

#ifndef COMPONENTS_HEADER
#define COMPONENTS_HEADER

#include "wires.h"
#include "operators.h"
#include "arith.h"
#include "types.h"
#include "misclists.h"
#include "instances.h"

/* Flavour of component */
typedef enum ComponentNature
{
    NoComponent,                /* Place holder type for invalidated components */
    CommentComponent,           /* Comment to include in target file */
    ProcedureComponent,         /* Balsa defined procedure component */
    ConstantComponent,          /* Constant ( val | out o_output_port ) */

    CallComponent,              /* Call CallMux ( sync o_in1, sync o_in2, sync *_out ) */
    CallMuxComponent,           /* Multiplexing call CallMux ( in o_in1, in o_in2, out *_out ) */
    CallDemuxComponent,         /* Demulitplexing call CallDemux ( out o_out1, out o_out2, in *_in ) */

    SynchComponent,             /* sync Join synchronisation Synch ( sync o_in1, sync o_in2, sync *_out ) */
    SynchPushComponent,         /* Push Join synchronisation SynchPush ( in o_in, out o_out1, out *_out2 ) */
    SynchPullComponent,         /* Push Join synchronisation SynchPull ( out o_out1, out o_out2, in *_in ) */

    SliceComponent,             /* Pull bit slice Slice ( lowIndex | out o_out, in *_in ) */
    CombineComponent,           /* Combine 2 inputs into a single output channel (bitwise concat)
                                   Combine ( out o_out, in *_lsw, in *_msw ) */
    AdaptComponent,             /* Adapt (sign extend, 0 pad) Adapt ( out_signed, in_signed | out o_out, in *_in ) */
    NullAdaptComponent,         /* NullAdapt NullAdapt ( sync *_out, in o_in ) */
    UnaryFuncComponent,         /* Func (unary) UnaryFunc (op, in_signed | out o_out, in *_in) */
    BinaryFuncComponent,        /* Func (binary) BinaryFunc (op, res_sgn, left_sgn, right_sgn |
                                   out o_out, in *_left, in *_right) */
    BinaryFuncConstRComponent,  /* Func (binary) with constant RHS, BinaryFunc (op, res_sgn, left_sgn, right_sgn,
                                   r_val | out o_out, in *_left) */
    ContinueComponent,          /* sync continue Continue ( out o_out ) */
    ContinuePushComponent,      /* Push continue ContinuePush ( out o_out ) */
    HaltComponent,              /* sync halt Halt ( out o_out ) */
    HaltPushComponent,          /* Push halt HaltPush ( out o_out ) */

    ForkComponent,              /* Fork ( in o_in, out *_out1, out *_out2 ) */
    WireForkComponent,          /* WireFork ( in o_in, out *_out1, out *_out2 ) */
    ForkPushComponent,          /* ForkPush ( in o_in, out *_out1, out *_out2 ) */
    ConcurComponent,            /* Concur ( sync o_in, sync *_out1, sync *_out2 ) */
    VariableComponent,          /* Variable ( width, readCount, name | in o_write, out[] o_read ), offset and name
                                   contain information linking the variable back to the original balsa variable */
    InitVariableComponent,      /* InitVariable ( width, readCount, initValue, name | in o_write, sync o_init,
                                   out[] o_read ), *as Variable* but init sync channel loads initValue */
    BuiltinVariableComponent,   /* BuiltinVariable ( readCount, name | in o_write, out[] o_read ) */
    FalseVariableComponent,     /* Variable ( name, offset, width | in o_write, sync actO, out[] o_read ) */
    SplitComponent,             /* Split ( in o_in, out *_lsw, out *_msw ) */
    LoopComponent,              /* Loop ( sync o_in, sync *_out ) */
    FetchComponent,             /* Fetch ( out_broad, sync o_act, in *_in, out *_out ) */
    BarComponent,               /* n way tender Bar ( out guardOut, sync do, out [] guardsOut, sync [] commands ) */
    WhileComponent,             /* While ( sync o_act, in *_bb, sync *_then ) */
    DecisionWaitComponent,      /* DecisionWait ( sync o_act, sync [] o_inp, sync [] *_out ) */
    ArbiterComponent,           /* Arbiter ( sync o_left, sync o_right, sync *_left, sync *_right ) */
    CaseComponent,              /* Case ( width,oCount,oSpec | o_in, sync [] outAct ) */
    CaseFetchComponent,         /* CaseFetch ( width,iWidth,iCount,spec | out o_out, in *_index, in *_inp [] ) */
    PassivatorComponent,        /* PassivatorComponent ( count | sync o_inp ) */
    PassivatorPushComponent,    /* PassivatorPushComponent ( count | in o_inp, out o_out[] ) */
    CombineEqualComponent,      /* Combine n inputs into a single output channel 
                                   CombineEqual ( out o_out, in [] *_inp ) */
    SplitEqualComponent,        /* Split n inputs from a single input channel
                                   SplitEqual ( out o_inp, in [] *_out ) */
    EncodeComponent,            /* Encode ( width,iCount,iSpec | sync [] o_in, *_out ) */
    CallDemuxPushComponent,     /* CallDemuxPush ( width,oCount | in o_inp, *_out [] ) */
    CallActiveComponent,        /* CallActive ( oCount | o_inp, *_out [] ) */

    UnaryFuncPushComponent,     /* Func (unary) UnaryFunc (op, in_signed | out *_out, in o_in) */
    BinaryFuncPushComponent,    /* Func (binary) BinaryFunc (op, res_sgn, left_sgn, right_sgn |
                                   out *_out, in o_left, in o_right) */
    BinaryFuncConstRPushComponent, /* Func (binary) with constant RHS, BinaryFunc (op, res_sgn, left_sgn, right_sgn,
                                      r_val | out *_out, in o_left) */
    StringComponent,            /* String (str, o_out) */
/*
   ConnectorComponent,
 */
    SequenceComponent, 			/* Sequence ( sync o_in, sync *_out1, sync *_out2 ) */
    PassiveEagerFalseVariableComponent, /* PassiveEagerFalseVariable ( name, offset, width | sync act, in o_write, sync write, out[] o_read ) */
    ActiveEagerFalseVariableComponent, /* ActiveEagerFalseVariable ( name, offset, width | sync act, in o_write, sync write, out[] o_read ) */
    PassiveSyncEagerFalseVariableComponent, /* PassiveSyncEagerFalseVariable ( name | sync act, sync o_write, sync write, out[] o_read ) */
    PassiveEagerNullAdaptComponent, /* PassiveEagerNullAdapt ( sync act, in o_write, sync write ) */
    ActiveEagerNullAdaptComponent, /* ActiveEagerNullAdapt ( sync act, in o_write, sync write ) */
    LastComponent               /* component count */
}
ComponentNature;

/* Forward decl */
struct Procedure;

typedef enum
{
    TypeComponentParameter,
    StringComponentParameter,
    NumberComponentParameter
}
ComponentParameterNature;

/* Parameter list for those components which don't place their
	parameters in particular parts of a Component */
typedef struct
{
    ComponentParameterNature nature;
    union
    {
        PtrType type;
        Ptrchar string;
        PtrMP_INT number;
    }
    value;
    tIdent ident;               /* Name of parameter, useful for error reporting! */
    PtrType type;               /* Type of parameter for number parameters */
}
ComponentParameter, *PtrComponentParameter;

DECLARE_CONS_LIST_TYPE (ComponentParameter)
/* Component data, linkable into lists */
typedef struct Component
{
    ComponentNature nature;
    union
    {
        struct
        {
            tIdent ident;       /* procedure name */
            PtrComponentParameterList parameters;
            Ptrchar portSpec;
            PtrInstanceList ports;
            tIdent componentType; /* what manner of thing this component implements,
                                     this will usually be `is-builtin-function' */
            tIdent baseComponentName; /* id. of definition (in the scope of componentType)
                                         which this procedure component implements, eg. `ToString' for a
                                         `ToString_blah_blah...' component */
        }
        procedure;
        struct
        {
            PtrMP_INT value;
        }
        constant;               /* constant / filter value */
        struct
        {
            tIdent ident;       /* component name */
            bool begin;         /* true for BEGIN comment, false for END comment */
        }
        comment;
        struct
        {
            bool outputSigned, inputSigned; /* i/o signednesses */
        }
        adapt;
        struct
        {
            Operators operation;
            bool inputSigned;
        }
        unary;
        struct
        {
            Operators operation;
            bool resultSigned, leftSigned, rightSigned;
            Bits rightWidth;
            PtrMP_INT rightValue;
        }
        binary;
        struct
        {
            Ptrchar name;
            Bits offset;
            Bits width;
            PtrMP_INT initialisedValue;
            Ptrchar readPortsSpecString;
        }
        variable;
        struct
        {
            Ptrchar specString; /* tIdents seem to be length limited */
        }
        casecomp;
        struct
        {
            Bits bits;
        }
        select;
        struct
        {
            Ptrchar string;     /* string or format */
        }
        print;
        struct
        {
            PtrType type;       /* type for PrintValue */
        }
        printValue;
        struct
        {
            Bits lowIndex;      /* low bit index for slice */
        }
        slice;
        struct
        {
            bool pre;           /* T: pre command (repeat command while ... end), F: do guards then command */
        }
        midWhile;
        struct
        {
            Ptrchar string;     /* string constant component $BrzString */
        }
        string;
        struct
        {
            Ptrchar specString;
        }
        sequence;
        struct
        {
            bool outBroad;      /* true when the output of the fetch needs to be broad instead of reduced-broad */
        }
        fetch;
    }
    param;                      /* extra parameter */
    int portCountParam;         /* extra port count parameter needed when parsing input */
    PtrLispList options;        /* User options list from Breeze */
    tPosition position;
    PtrWireList ports;          /* Dynamically allocated port list (in order 1st -> 2nd -> 3rd ... -> NULL) */
}
Component, *PtrComponent;

/* ComponentSpec : parameter and port expectations for components,
   paramSpec: [#isUB]*  portSpec: ([0-9]?[iIoOsS])*
   param spec is a list of symbols for the expected parameter type:
   # - number (stored in int/unsigned)
   i - number (stored in MP_INT) 
   $ - string (usually a spec string, a tIdent)
   U - unary operator
   B - binary operator
   b - Boolean (true/false)
   t - Type
   port spec is a list of expected port types (capital letter for active, lower case for passive):
   sS - sync
   iI - input
   oO - output
   a prefixing digit indicates a parameter (must be a `#' parameter) (numbered 0,1,2...) to act as
   a multiplier for the next port.
   eg: $BrzSequencer paramSpec: # portSpec: s0S
 */

/* Maximum number of parameters for a breeze component */
#define MAX_BREEZE_PARAMETER_COUNT (10)

typedef struct
{
    Ptrchar name;               /* Component name */
    Ptrchar paramSpec, portSpec; /* param and port specifications */
    int offsets[10];            /* Offsets into component structure at which parameter should
                                   be stored (as int for #, PtrMP_INT for i, tIdent for $,
                                   bool for b ...; 0 for don't remeber parameter */
}
ComponentSpec, *PtrComponentSpec;

/* BreezeComponents : specifications for components
   FirstBreezeComponentIdent : hashed component idents base, tIdent value for first component name,
   the ident number of a component of nature n is: (tIdent)(FirstBreezeComponentIdent + (int) n)
   or BREEZE_COMPONENT_IDENT(n) */
extern ComponentSpec BreezeComponents[];
extern tIdent FirstBreezeComponentIdent;

#define BREEZE_COMPONENT_IDENT(nature) ((tIdent)(((int)FirstBreezeComponentIdent) + ((int)(nature))))

DECLARE_CONS_LIST_TYPE (Component) DECLARE_CONS_LIST_DEEP_COPY (Component)
/* BeginComponents : initialise module local data structures,
   NB. must call AFTER BeginIdents */
extern void BeginComponents (void);

/* NewComponent: create empty component */
extern PtrComponent NewComponent (tPosition position, ComponentNature nature);

/* GetComponentPortSpec : return the spec string of the given component (based on nature,
   or if nature is ProcedureComponent then procedure->spec) */
extern Ptrchar GetComponentPortSpec (PtrComponent component);

/* GetComponentIntegerParameterAtPosition : get the integer parameter value at parameter
	position `pos' in a component's parameter set */
extern int GetComponentIntegerParameterAtPosition (PtrComponent component, unsigned pos);

/* UpdateWire : update the {active,passive}{Component,Port} entries in a wire */
extern void UpdateWire (PtrWire wire, bool isActive, PtrComponent component, unsigned portNo);

/* UpdateWires : update the {active,passive}{Component,Port} entries in a wire list
   only update noOfWires wires (if noOfWires == -1 then update all), wires are
   given port numbers from firstPortNo upwards */
extern void UpdateWires (PtrWireList wires, bool isActive, PtrComponent component, unsigned firstPortNo, int noOfWires);

/* NewComponentParameter : create a component parameter of the desire `nature' and
	put the void * argument into the appropriate place in the returned value */
extern PtrComponentParameter NewComponentParameter (ComponentParameterNature nature, void *value, tIdent ident, PtrType type);

/* ---------- Component constructors */

/* NewCommentComponent : create a comment component, includes an identifier and an indication of
   whether this is a begin or end comment */
extern PtrComponent NewCommentComponent (tPosition position, tIdent ident, bool isBegin);

/* NewProcedureComponent : create a procedure component from the given procedure and ident */
extern PtrComponent NewProcedureComponent (tPosition position, tIdent ident, struct Procedure *proc);

/* NewConstantComponent : create a constant component of value val connected to output */
extern PtrComponent NewConstantComponent (tPosition position, PtrMP_INT val, PtrWire output);

/* NewStringComponent : create a constant string component of value str connected to output */
extern PtrComponent NewStringComponent (tPosition position, Ptrchar str, PtrWire output);

/* NewPassivatorComponent : create a passivator */
extern PtrComponent NewPassivatorComponent (tPosition position, PtrWire in1, PtrWire in2);

/* NewPassivatorPushComponent : create a passivator */
extern PtrComponent NewPassivatorPushComponent (tPosition position, PtrWire out, PtrWire in);

/* NewCallComponent : create a call element */
extern PtrComponent NewCallComponent (tPosition position, PtrWire in1, PtrWire in2, PtrWire out);

/* NewCallMuxComponent : create a push multiplexing call element */
extern PtrComponent NewCallMuxComponent (tPosition position, PtrWire in1, PtrWire in2, PtrWire out);

/* NewCallDemuxComponent : create a pull demultiplexing call element */
extern PtrComponent NewCallDemuxComponent (tPosition position, PtrWire out1, PtrWire out2, PtrWire in);

/* NewSynchComponent : create a synchronising element */
extern PtrComponent NewSynchComponent (tPosition position, PtrWire in1, PtrWire in2, PtrWire out);

/* NewSynchPushComponent : create a push input, push and pull outputs synchronising element */
extern PtrComponent NewSynchPushComponent (tPosition position, PtrWire in, PtrWire out1, PtrWire out2);

/* NewSynchPullComponent : create a pull input, dual pull outputs synchronising element */
extern PtrComponent NewSynchPullComponent (tPosition position, PtrWire out1, PtrWire out2, PtrWire in);

/* NewSynchPullConnectComponent : create a pull connector */
extern PtrComponent NewSynchPullConnectComponent (tPosition position, PtrWire out1, PtrWire in);

/* NewSynchPullFromListComponent : create a SynchPull with multiple outputs, from a given list */
extern PtrComponent NewSynchPullFromListComponent (tPosition position, PtrWireList outputs, PtrWire input);

/* NewSliceComponent : slice channel in into channel out starting at index lowIndex in `in' */
extern PtrComponent NewSliceComponent (tPosition position, Bits lowIndex, PtrWire out, PtrWire in);

/* NewCombineComponent : combine pull channels into one */
extern PtrComponent NewCombineComponent (tPosition position, PtrWire out, PtrWire lsw, PtrWire msw);

/* NewCombineEqualComponent : combine pull channels into one */
extern PtrComponent NewCombineEqualComponent (tPosition position, PtrWire out, PtrWireList inp);

/* NewSplitEqualComponent : split a pull channel */
extern PtrComponent NewSplitEqualComponent (tPosition position, PtrWire inp, PtrWireList out);

/* NewAdaptComponent : create an adapter from sourceSigned signedness to targetSigned signedness */
extern PtrComponent NewAdaptComponent (tPosition position, bool targetSigned, bool sourceSigned, PtrWire target, PtrWire source);

/* NewNullAdaptComponent : create an adapter from sourceSigned signedness to sync */
extern PtrComponent NewNullAdaptComponent (tPosition position, PtrWire target, PtrWire source);

/* NewUnaryFuncComponent : create a unary function where the input is signed if inSigned is true */
extern PtrComponent NewUnaryFuncComponent (tPosition position, Operators op, bool inSigned, PtrWire out, PtrWire in);

/* NewBinaryFuncComponent : create a binary function where the inputs are signed if leftSigned, rightSigned
   are set, resultSet specifies whether the result is signed */
extern PtrComponent NewBinaryFuncComponent (tPosition position, Operators op, bool resultSigned,
	bool leftSigned, bool rightSigned, PtrWire result, PtrWire left, PtrWire right);

/* NewBinaryFuncConstRComponent : create a binary function with constant RHS */
extern PtrComponent NewBinaryFuncConstRComponent (tPosition position, Operators op, bool resultSigned,
	bool leftSigned, bool rightSigned, Bits rightWidth, PtrMP_INT rightValue, PtrWire result, PtrWire left);

/* NewContinueComponent : create a continue component */
extern PtrComponent NewContinueComponent (tPosition position, PtrWire out);

/* NewContinuePushComponent : create a continue component with a push port */
extern PtrComponent NewContinuePushComponent (tPosition position, PtrWire out);

/* NewHaltComponent : create a halt component */
extern PtrComponent NewHaltComponent (tPosition position, PtrWire out);

/* NewHaltPushComponent : create a halt component with a push port */
extern PtrComponent NewHaltPushComponent (tPosition position, PtrWire out);

/* NewForkComponent : create a fork */
extern PtrComponent NewForkComponent (tPosition position, PtrWire in, PtrWire out1, PtrWire out2);

/* NewForkComponentWithWireList : create a fork to many channels */
extern PtrComponent NewForkComponentWithWireList (tPosition position, PtrWire in, PtrWireList out);

/* NewConnectComponent : create a fork, with only one connection, a connector */
extern PtrComponent NewConnectComponent (tPosition position, PtrWire in, PtrWire out1);

/* NewWireForkComponent : create a fork */
extern PtrComponent NewWireForkComponent (tPosition position, PtrWire in, PtrWire out1, PtrWire out2);

/* NewForkConnectorComponent : create a fork with only one output */
extern PtrComponent NewForkConnectorComponent (tPosition position, PtrWire in, PtrWire out);

/* NewForkPushComponent : create a push demultiplexing fork */
extern PtrComponent NewForkPushComponent (tPosition position, PtrWire in, PtrWire out1, PtrWire out2);

/* NewCallDemuxPushComponent : create a push demultiplexing call */
extern PtrComponent NewCallDemuxPushComponent (tPosition position, PtrWire in, PtrWire out1, PtrWire out2);

/* NewCallActiveComponent : create a forked req, called ack component */
extern PtrComponent NewCallActiveComponent (tPosition position, PtrWire in, PtrWire out1, PtrWire out2);

/* NewConcurComponent : create a parallelising component */
extern PtrComponent NewConcurComponent (tPosition position, PtrWire act, PtrWire actout1, PtrWire actout2);

/* NewSequenceComponent : create a sequencing component with specString (generated during the optimisation stage from
	left and right accesses) for optimised backend */
extern PtrComponent NewSequenceComponent (tPosition position, PtrWire act, PtrWire left, PtrWire right);

/* NewVariableComponent : create a variable component (from the balsa variable name at offset offset), 
   with possibly many read ports */
extern PtrComponent NewVariableComponent (tPosition position, Ptrchar name, Bits offset, Bits width, PtrWire write, PtrWireList reads);

/* NewInitVariableComponent : create an initialised variable component (from the balsa variable name at offset offset), 
   with possibly many read ports */
extern PtrComponent NewInitVariableComponent (tPosition position, Ptrchar name, Bits offset, Bits width, PtrMP_INT initValue, PtrWire init, PtrWire write, PtrWireList reads);

/* NewFalseVariableComponent : create a false variable component, with possibly many read ports */
extern PtrComponent NewFalseVariableComponent (tPosition position, Bits offset, Bits width, PtrWire write, PtrWire activateOut, PtrWireList reads);

/* NewPassiveEagerFalseVariableComponent : create a passive eager false variable component, with possibly many read ports */
extern PtrComponent NewPassiveEagerFalseVariableComponent (tPosition position, Bits offset, Bits width, PtrWire activate, PtrWire write, PtrWire activateOut,
  PtrWireList reads);

/* NewActiveEagerFalseVariableComponent : create an active eager false variable component, with possibly many read ports */
extern PtrComponent NewActiveEagerFalseVariableComponent (tPosition position, Bits offset, Bits width, PtrWire activate, PtrWire write, PtrWire activateOut,
  PtrWireList reads);

/* NewPassiveSyncEagerFalseVariableComponent : create a passive sync eager false variable component */
extern PtrComponent NewPassiveSyncEagerFalseVariableComponent (tPosition position, PtrWire activate, PtrWire write, PtrWire activateOut);

/* NewPassiveEagerNullAdaptComponent : create a passive eager nullAdapt component, with possibly many read ports */
extern PtrComponent NewPassiveEagerNullAdaptComponent (tPosition position, PtrWire activate, PtrWire write, PtrWire activateOut);

/* NewActiveEagerNullAdaptComponent : create an active eager nullAdapt component, with possibly many read ports */
extern PtrComponent NewActiveEagerNullAdaptComponent (tPosition position, PtrWire activate, PtrWire write, PtrWire activateOut);

/* NewDecisionWaitComponent : create a decision wait component */
extern PtrComponent NewDecisionWaitComponent (tPosition position, PtrWire activate, PtrWireList inps, PtrWireList outs);

/* NewSplitComponent : create a channel splitting component */
extern PtrComponent NewSplitComponent (tPosition position, PtrWire in, PtrWire lsw, PtrWire msw);

/* NewLoopComponent : create an infinate loop */
extern PtrComponent NewLoopComponent (tPosition position, PtrWire in, PtrWire out);

/* NewFetchComponent : create a fetch component */
extern PtrComponent NewFetchComponent (tPosition position, bool outBroad, PtrWire act, PtrWire in, PtrWire out);

/* NewBarComponent : create an n (length of guards/commands lists) way if/while guard */
extern PtrComponent NewBarComponent (tPosition position, PtrWire guardOut, PtrWire activate, PtrWireList guards, PtrWireList commands);

/* NewWhileComponent : create a while component, without an else port */
extern PtrComponent NewWhileComponent (tPosition position, PtrWire act, PtrWire guard, PtrWire thenAct);

/* NewArbiterComponent : create a arbiter component */
extern PtrComponent NewArbiterComponent (tPosition position, PtrWire leftIn, PtrWire rightIn, PtrWire leftOut, PtrWire rightOut);

/* NewCaseComponent : create a case component from an activation channel and a list of output activations */
extern PtrComponent NewCaseComponent (tPosition position, Ptrchar specString, PtrWire activation, PtrWireList outActs);

/* NewEncodeComponent : create an encoder from a list of input activations and an output channel */
extern PtrComponent NewEncodeComponent (tPosition position, Ptrchar specString, PtrWireList inps, PtrWire out);

/* NewCaseFetchComponent : create a case fetch component from an activation output channel, an index
   input channel and a list of input channels */
extern PtrComponent NewCaseFetchComponent (tPosition position, Ptrchar specString, PtrWire out, PtrWire index, PtrWireList inp);

/* ---------- storcurtsnoc tnenopmoC */

/* MarkComponent : mark a component as having nature NoComponent, this makes the component eligible for
   sweeping by SweepComponentList */
extern void MarkComponent (PtrComponent comp);

/* SweepComponentList : remove elements of the given list which have nature NoComponent */
extern PtrComponentList SweepComponentList (PtrComponentList list);

/* CopyComponent : Copy a component, shallow copying the port lists */
extern PtrComponent CopyComponent (PtrComponent comp);

/* StrPtrComponent : Print a component description on stream */
extern void StrPtrComponent (FILE * stream, PtrComponent component);

/* StrValuesOfComponentParameterList : print a parameter list held in the unprocessed format */
extern void StrValuesOfComponentParameterList (FILE * stream, PtrComponentParameterList parameters, Ptrchar separator);

/* StrBreezeComponentParameterList : print Breeze parameterised-component parameter declaration
	list from the incidental ident and type into in a ComponentParameterList */
extern void StrBreezeComponentParameterList (FILE * stream, PtrComponentParameterList parameters, Ptrchar separator);

/* StrPtrSBreezeComponentList : print a list of abridged component descriptions */
extern void StrPtrSBreezeComponentList (FILE * stream, PtrComponentList list, Ptrchar separator);

#endif /* COMPONENTS_HEADER */
