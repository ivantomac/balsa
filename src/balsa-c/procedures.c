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

	`procedures.c'
	Proedure/Shared block/Function (?) handling
	
 */

#include "procedures.h"
#include "misc.h"
#include "ports.h"
#include "pretty.h"
#include "contexts.h"
#include "flags.h"
#include "callcontexts.h"
#include <limits.h>

/* NULL object and Context marking objects */
PtrProcedure NoProcedure = NULL;
PtrProcedure ContextMarkerProcedureObj = NULL;

PtrProcedure WriteMessageFunction = NULL;
PtrProcedure StringAppendFunction = NULL;
PtrProcedure ToStringFunction = NULL;
PtrProcedure StringFunction = NULL;

DEFINE_CONS_LIST_TYPE (Procedure)
/* BeginProcedures : wouldn't Modula-2 make this sooo much easier */
void BeginProcedures (void)
{
    NoProcedure = NewProcedure (MarkerIdent, UnresolvableProcedure, 0, NoPosition);
    ContextMarkerProcedureObj = NewProcedure (MarkerIdent, ContextMarkerProcedure, 0, NoPosition);
}

/* NewProcedure : produce a bare procedure structure */
PtrProcedure NewProcedure (tIdent name, ProcedureNature nature, unsigned portCount, tPosition position)
{
    PtrProcedure newProc = NEW (Procedure);

    newProc->nature = nature;
    newProc->ident = name;
    newProc->position = position;
    newProc->portCount = portCount;
    newProc->scope = ImportedScope;
    newProc->userAttributes = NULL;
    newProc->activationOptions = NULL;
    switch (nature)
    {
    case UnSharedFunction:
    case SharedFunction:
    case BuiltinFunction:
        newProc->info.function.attributes.components = NULL;
        newProc->info.function.attributes.wires = NULL;
        newProc->info.function.attributes.accesses = NULL;
        newProc->info.function.attributes.activation = NULL;
        newProc->info.function.attributes.value.type = NoType;
        newProc->info.function.attributes.value.value.baseValue = NULL;
        break;
    case UnSharedProcedure:
    case SharedProcedure:
        newProc->info.procedure.attributes.components = NULL;
        newProc->info.procedure.attributes.wires = NULL;
        newProc->info.procedure.attributes.accesses = NULL;
        newProc->info.procedure.attributes.permanent = false;
        newProc->info.procedure.attributes.activation = NULL;
        break;
    case ParameterisedProcedure:
        newProc->info.parameterisedProcedure.tree = NULL;
    default:
        break;
    }
    newProc->unplacedAccesses = NULL;
    newProc->unplacedActivation = NULL;
    newProc->context = NULL;
    newProc->ports = NULL;
    newProc->extraPorts = NULL;
    newProc->portSpec = "";
    newProc->isExternal = false;
    return newProc;
}

/* CopyPtrProcedure : make a shallow copy of a PtrProcedure */
PtrProcedure CopyPtrProcedure (PtrProcedure proc)
{
    PtrProcedure ret = NEW (Procedure);

    *ret = *proc;
    return ret;
}

/* ReassignPortWires : reassign the wire allocations of the procedure proc's ports so
   that activation is wire 1, the first port is wire 2 and so on */
void ReassignPortWires (PtrProcedure proc)
{
    /* Activation as port 1 */
    bool isFunction = proc->nature == UnSharedFunction || proc->nature == SharedFunction;

    PtrInstanceList ports = proc->ports;
    PtrWireList wireList = (isFunction ? proc->info.function.attributes.wires : proc->info.procedure.attributes.wires);
    PtrWire activation = (isFunction ? proc->info.function.attributes.activation : proc->info.procedure.attributes.activation);
    PtrWireList newPortWires = NewWireList (activation, NULL);
    PtrWireList finalWireList;

    /* Create the list newPortWires with the activation, port wires in the order n -> n-1 -> ... -> 1 */
    /* Mark and sweep the original wire positions */
    /* activation, mark for sweeping */
    activation->bundleNo = 0;

    while (ports)
    {
        if (CAR (ports)->type->nature == ArrayedType)
        {
            PtrInstanceList portElems = CAR (ports)->type->info.arrayed.arrayedElements;
            unsigned elemCount = CAR (ports)->type->info.arrayed.elementCount;

            while (portElems && elemCount != 0)
            {
                newPortWires = NewWireList (CAR (portElems)->info.port.wire, newPortWires);
                MarkWire (CAR (portElems)->info.port.wire);
                elemCount--;    /* arrayed element lists aren't terminated */
                portElems = CDR (portElems);
            }
        } else
        {
            newPortWires = NewWireList (CAR (ports)->info.port.wire, newPortWires);
            MarkWire (CAR (ports)->info.port.wire);
        }
        ports = CDR (ports);
    }

    /* Sweep the port wires from the original wire list */
    wireList = SweepWireList (wireList);

    /* Build the final wire list and renumber it */
    finalWireList = AppendWireLists (wireList, newPortWires);
    RenumberWireList (finalWireList, true /* fully */ );

    /* Update the proc */
    if (isFunction)
        proc->info.function.attributes.wires = finalWireList;
    else
        proc->info.procedure.attributes.wires = finalWireList;
}

/* PushPullWireWidth : returns wire_width for push wire, or -wire_width for pull wire */
int PushPullWireWidth (PtrWire wire)
{
    return (wire->isPull ? -(wire->width) : wire->width);
}

/* GetNextBalsaParam: find the next identifier and parameter tTree from the given identifier list
   (from the head of the given parameters) and a list of parameters.  Returns the head parameter
   and modifies `parameters', `ident' and `idents' to hold the parameter list tail, head identifier
   and identifier list tail respectively. */
tTree GetNextBalsaParam (tTree * parameters, tIdent * ident, PtrIdentList * idents, tPosition position)
{
    tTree ret = *parameters;

    if (!*idents)               /* No idents left, this shouldn't happen */
    {
        LOG_ERROR (ParameterListTooLong, NoIdent, position);
        return ret;
    }
    /* Take the head of idents for ident */
    if (ident)
        *ident = CAR (*idents).ident;

    if (!CDR (*idents))         /* Only one identifier left, prepare the next param */
    {
        *parameters = (*parameters)->FormalPort.next;
        *idents = NULL;
        if ((*parameters)->Kind == kParamPort || (*parameters)->Kind == kTypeParamPort)
        {
            *idents = ((*parameters)->Kind == kParamPort
              ? (*parameters)->ParamPort.Idents->Idents.idents : (*parameters)->TypeParamPort.Idents->Idents.idents);
        }
    } else
    {
        *idents = CDR (*idents);
    }
    return ret;
}

/* HandleExprParam: make an expression parameter instance from a parameter KCT tree and some
   other info.  Used to expand specified procedures. */
tTree
HandleExprParam (tTree formalParameters, PtrContext * callingContext,
  PtrIdentList * remainingIdents, PtrInstanceList * constants, PtrComponentParameterList * params, TypedValue val, tPosition position)
{
    tTree ret = formalParameters;
    tTree parameter = formalParameters;
    PtrInstance constant;
    tIdent ident;

    parameter = GetNextBalsaParam (&ret, &ident, remainingIdents, position);
    if (parameter == NoTree)
        return ret;

    /* Right sort of parameter? */
    if (parameter->Kind != kParamPort)
    {
        LOG_ERROR (ExpectingATypeParam, NoIdent, position);
        return ret;
    }
    /* The right type? */
    if (!TypeEquivalence (val.type, parameter->ParamPort.AType->AType.type))
    {
        LOG_ERROR (TypeIsNotValid, NoIdent, position);
        return ret;
    } else if (!val.value.baseValue) /* Constant? */
    {
        LOG_ERROR (ExpressionMustBeConstant, NoIdent, position);
        return ret;
    }
    constant = NewConstantInstance (ident, val);
    *constants = NewInstanceList (constant, *constants);
    {
        PtrComponentParameter param;

        if (TypeEquivalence (val.type, StringTypeObj))
        {
            param = NewComponentParameter (StringComponentParameter, GetBuiltinValuePointer (val.value.baseValue), ident, NULL);
        } else
        {
            param = NewComponentParameter (NumberComponentParameter, val.value.baseValue, ident, val.type);
        }
        if (params)
            *params = NewComponentParameterList (param, *params);
    }
    *callingContext = NewContext ((*callingContext)->depth,
      (*callingContext)->types, NewInstanceList (constant, (*callingContext)->instances), (*callingContext)->procedures);
    return ret;
}

/* HandleTypeParam: make a type parameter instance from a parameter KCT tree and some
   other info.  Used to expand specified procedures. */
tTree
HandleTypeParam (tTree formalParameters, PtrContext * callingContext,
  PtrIdentList * remainingIdents, PtrTypeList * types, PtrComponentParameterList * params, PtrType type, tPosition position)
{
    tTree ret = formalParameters;
    tTree parameter = formalParameters;
    PtrType newType;
    tIdent ident;

    parameter = GetNextBalsaParam (&ret, &ident, remainingIdents, position);
    if (parameter == NoTree)
        return ret;

    /* Right sort of parameter? */
    if (parameter->Kind != kTypeParamPort)
    {
        LOG_ERROR (ExpectingAnExprParam, NoIdent, position);
        return ret;
    }
    if (type == NoType)
        return ret;

    newType = AliasType (type, ident);
    *types = NewTypeList (newType, *types);
    {
        PtrComponentParameter param = NewComponentParameter (TypeComponentParameter, newType, ident, NULL);

        if (params)
            *params = NewComponentParameterList (param, *params);
    }
    *callingContext = NewContext ((*callingContext)->depth,
      NewTypeList (newType, (*callingContext)->types), (*callingContext)->instances, (*callingContext)->procedures);
    return ret;
}

/* MakeSpecifiedProcedureName: make a unique name for a specified parameterised procedure.
   The name will be of the form baseName-<unique-integer> */
tIdent MakeSpecifiedProcedureName (tIdent baseName)
{
    static unsigned procedureCount = 0;
    const char *baseNameStr = PeekString (baseName);
    unsigned length = strlen (baseNameStr) + 20;
    tIdent ret;

    Ptrchar name = NEW_ARRAY (char, length);

    sprintf (name, "%s-%d", baseNameStr, procedureCount);
    procedureCount++;
    ret = MakeIdent1 (name);
    FREE_ARRAY (char, length, name);

    return ret;
}

/* StrPtrBreezePortList : print out the given list's port elements in the SBreeze format */
void StrPtrBreezePortList (FILE * stream, PtrInstanceList ports, char *separator)
{
    while (ports && CAR (ports)->nature != ContextMarkerInstance)
    {
        PtrType type = CAR (ports)->type;

        /* Port format: (sync-port "name" sense) (port "name" passive input type), with arrayed port
           (arrayed-sync-port "name" sense lowIndex n) (arrayed-port "name" sense direction type lowIndex n ) */
        bool isArrayed = type->nature == ArrayedType;
        bool isSync;

        putc ('(', stream);

        if (isArrayed)
            type = type->info.arrayed.baseType; /* Use the base type for arrayed ports */
        isSync = type->nature == SyncType;

        fprintf (stream, "%s%sport \"", (isArrayed ? "arrayed-" : ""), (isSync ? "sync-" : ""));
        WriteIdent (stream, CAR (ports)->ident);
        fprintf (stream, "\" %s", (CAR (ports)->info.port.sense == PassivePortSense ? "passive" : "active"));

        if (!isSync)
        {
            fprintf (stream, " %s ", (CAR (ports)->nature == InputChannelInstance ? "input" : "output"));
            StrPtrBreezeTopLevelType (stream, type);
        }

        if (isArrayed)
        {
            PtrMP_INT arraySize = NewMP_INT (0);

            mpz_sub (arraySize, CAR (ports)->type->info.arrayed.range.upperBound, CAR (ports)->type->info.arrayed.range.lowerBound);
            mpz_add_ui (arraySize, arraySize, 1);

            putc (' ', stream);
            StrPtrMP_INT (stream, CAR (ports)->type->info.arrayed.range.lowerBound);
            putc (' ', stream);
            StrPtrMP_INT (stream, arraySize);
            putc (' ', stream);

            StrPtrBreezeTopLevelType (stream, CAR (ports)->type->info.arrayed.range.boundingType);

            DeleteMP_INT (arraySize);
        }

        if (CAR (ports)->info.port.position.Line != 0)
        {
            putc (' ', stream);
            StrSBreezePosition (stream, CAR (ports)->info.port.position);
        }
        if (CAR (ports)->info.port.options)
        {
            putc (' ', stream);
            StrPtrLispList (stream, CAR (ports)->info.port.options, " ", " ");
        }

        putc (')', stream);
        if (CDR (ports) && CAR (CDR (ports))->nature != ContextMarkerInstance)
            fprintf (stream, "%s", separator);
        ports = CDR (ports);
    }
}

/* StrBreezePortOptions : print a port trailing options in the Breeze format */
void StrBreezePortOptions (FILE * stream, PtrLispList options, tPosition position)
{
    if (position.Line != 0)
    {
        putc (' ', stream);
        StrSBreezePosition (stream, position);
    }
    if (options)
    {
        putc (' ', stream);
        StrPtrLispList (stream, options, " ", " ");
    }
}

/* StrPtrSBreezeProcedure : like StrPtrProcedure but prints in a SBreeze compatible format,
   NB. longForm is ignored */
void StrPtrSBreezeProcedure (FILE * stream, PtrProcedure proc, bool longForm)
{
    if (proc)
    {
        switch (proc->nature)
        {
        case ContextMarkerProcedure:
            fprintf (stream, ";;; context-marker\n");
            break;
        case ParameterisedProcedure:
            PrettyPrintStream = stream;
            fprintf (stream, "(balsa\n  (");
            PrettyPrintProcedureDecl (proc->info.parameterisedProcedure.tree, 1);
            fprintf (stream, ")\n)\n");
            break;
        case ParameterisedBuiltinFunction:
            PrettyPrintStream = stream;
            fprintf (stream, "(balsa\n  (");
            PrettyPrintBuiltinFunctionDecl (proc->info.parameterisedFunction.tree, 1);
            fprintf (stream, ")\n)\n");
            break;
        case UnSharedProcedure:
            CallContext_Str_Reset ();
            fprintf (stream, "(breeze-part \"");
            WriteIdent (stream, proc->ident);
            fprintf (stream, "\"\n  (ports\n    (sync-port \"activate\" passive");
            StrBreezePortOptions (stream, proc->activationOptions, proc->position);
            putc (')', stream);
            if (proc->ports)
            {
                fprintf (stream, "\n    ");
                StrPtrBreezePortList (stream, proc->ports, "\n    ");
            }
            fprintf (stream, "\n  )\n");
            fprintf (stream, "  (attributes\n    (is-procedure)");
            if (proc->isExternal)
                fprintf (stream, "\n    (is-external)");
            if (proc->info.procedure.attributes.permanent)
                fprintf (stream, "\n    (is-permanent)");
            fprintf (stream, "\n    ");
            StrSBreezePosition (stream, proc->position);
            if (proc->userAttributes)
            {
                fprintf (stream, "\n    ");
                StrPtrLispList (stream, proc->userAttributes, "\n    ", " ");
            }
            fprintf (stream, "\n  )\n");
            /* Channels */
            fprintf (stream, "  (channels\n");
            StrPtrSBreezeWires (stream, proc->info.procedure.attributes.wires, "    ");
            fprintf (stream, "  )\n");
            /* Components */
            fprintf (stream, "  (components\n    ");
            StrPtrSBreezeComponentList (stream, proc->info.procedure.attributes.components, "\n    ");
            fprintf (stream, "\n  )\n");
            CallContext_Str_Proc (BreezeOutputFile, proc);
            fprintf (stream, ")\n");
            break;
        case UnSharedFunction:
            CallContext_Str_Reset ();
            fprintf (stream, "(breeze-part \"");
            WriteIdent (stream, proc->ident);
            fprintf (stream, "\"\n  (ports\n    (port \"activate\" passive output ");
            StrPtrSBreezeTypeName (stream, proc->info.function.attributes.value.type);
            StrBreezePortOptions (stream, proc->activationOptions, proc->position);
            putc (')', stream);
            if (proc->ports)
            {
                fprintf (stream, "\n    ");
                StrPtrBreezePortList (stream, proc->ports, "\n    ");
            }
            fprintf (stream, "\n  )\n");
            fprintf (stream, "  (attributes\n    (is-function)");
            if (proc->isExternal)
                fprintf (stream, "\n    (is-external)");
            fprintf (stream, "\n    ");
            StrSBreezePosition (stream, proc->position);
            if (proc->userAttributes)
            {
                fprintf (stream, "\n    ");
                StrPtrLispList (stream, proc->userAttributes, "\n    ", " ");
            }
            fprintf (stream, "\n  )\n");
            /* Channels */
            fprintf (stream, "  (channels\n");
            StrPtrSBreezeWires (stream, proc->info.function.attributes.wires, "    ");
            fprintf (stream, "  )\n");
            /* Components */
            fprintf (stream, "  (components\n    ");
            StrPtrSBreezeComponentList (stream, proc->info.function.attributes.components, "\n    ");
            fprintf (stream, "\n  )\n");
            CallContext_Str_Proc (BreezeOutputFile, proc);
            fprintf (stream, ")\n");
            break;
        case BuiltinFunction:
            fprintf (stream, "(breeze-part \"");
            WriteIdent (stream, proc->ident);
            fprintf (stream, "\"\n  (ports\n    (port \"activate\" passive output ");
            StrPtrSBreezeTypeName (stream, proc->info.function.returnType);
            StrBreezePortOptions (stream, proc->activationOptions, proc->position);
            putc (')', stream);
            if (proc->ports)
            {
                fprintf (stream, "\n    ");
                StrPtrBreezePortList (stream, proc->ports, "\n    ");
            }
            fprintf (stream, "\n  )\n");
            fprintf (stream, "  (attributes\n    (is-builtin-function)");
            if (proc->isExternal)
                fprintf (stream, "\n    (is-external)");
            fprintf (stream, "\n    ");
            StrSBreezePosition (stream, proc->position);
            if (proc->userAttributes)
            {
                fprintf (stream, "\n    ");
                StrPtrLispList (stream, proc->userAttributes, "\n    ", " ");
            }
            fprintf (stream, "\n  )\n");
            /* Channels */
            fprintf (stream, "  (channels\n");
            StrPtrSBreezeWires (stream, proc->info.procedure.attributes.wires, "    ");
            fprintf (stream, "  )\n");
            /* Components */
            fprintf (stream, "  (components)\n");
            fprintf (stream, ")\n");
            break;
        default:
            break;
        }
    }
}

void StrPtrSBreezeProcedureList (FILE * stream, PtrProcedureList procedures, bool onlyPrintLocals, Scope scopes, char *separator, bool longForm)
{
    while (procedures && (!onlyPrintLocals || CAR (procedures)->nature != ContextMarkerProcedure))
    {
        if ((int) CAR (procedures)->scope & (int) scopes)
        {
            StrPtrSBreezeProcedure (stream, CAR (procedures), longForm);
            if (CDR (procedures) && (onlyPrintLocals ? CADR (procedures)->nature != ContextMarkerProcedure : true))
                fprintf (stream, "%s", separator);
        }
        procedures = CDR (procedures);
    }
}
void
StrReversePtrSBreezeProcedureList (FILE * stream, PtrProcedureList procedures, bool onlyPrintLocals, Scope scopes, char *separator, bool longForm)
{
    if (procedures && (!onlyPrintLocals || CAR (procedures)->nature != ContextMarkerProcedure))
    {
        StrReversePtrSBreezeProcedureList (stream, CDR (procedures), onlyPrintLocals, scopes, separator, longForm);
        if ((int) CAR (procedures)->scope & (int) scopes)
        {
            if (CDR (procedures) && (onlyPrintLocals ? CADR (procedures)->nature != ContextMarkerProcedure : true))
                fprintf (stream, "%s", separator);
            StrPtrSBreezeProcedure (stream, CAR (procedures), longForm);
        }
    }
}
