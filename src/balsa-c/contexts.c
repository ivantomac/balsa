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

	`contexts.c'
	local definition lists for tree decoration/evaluation
	of `balsa', contexts are hierachical symbol tables.
	
 */

#include "contexts.h"
#include "flags.h"
#include "output.h"
#include "BreezeScanSource.h"

PtrContext PredefinedContext = NULL;
PtrContext TopLevelContext = NULL;
unsigned TopLevelContextDepth = 1;

/* BeginContexts : create an empty context except for the type bit */
void BeginContexts (void)
{
    if (DontReadBuiltin)
    {
        PredefinedContext = NewContext (0, NULL, NULL, NULL);
        /* Be careful when using this switch not to use any of these things */
        BitTypeObj = ErrorTypeObj = StringTypeObj = NoType;
    } else
    {
        PredefinedContext = HandleImport ("balsa.types.builtin", NoPosition, NewContext (0, NULL, NULL, NULL), ImportedScope);

        BitTypeObj = LookupType (PredefinedContext, MakeIdent1 ("bit"), false);
        ErrorTypeObj = LookupType (PredefinedContext, MakeIdent1 ("BalsaError"), false);
        StringTypeObj = LookupType (PredefinedContext, MakeIdent1 ("String"), false);
        WriteMessageFunction = LookupProcedure (PredefinedContext, MakeIdent1 ("WriteMessage"), false);
        StringAppendFunction = LookupProcedure (PredefinedContext, MakeIdent1 ("StringAppend"), false);
        ToStringFunction = LookupProcedure (PredefinedContext, MakeIdent1 ("ToString"), false);
        StringFunction = LookupProcedure (PredefinedContext, MakeIdent1 ("String"), false);

        if (!BitTypeObj || !ErrorTypeObj || !StringTypeObj || !WriteMessageFunction || !StringAppendFunction || !ToStringFunction || !StringFunction)
        {
            LOG_ERROR (BadBuiltinBlock, NoIdent, NoPosition);
        }
    }
}

/* NewContext : make a new context with the given parameters */
PtrContext NewContext (unsigned depth, PtrTypeList types, PtrInstanceList instances, PtrProcedureList procedures)
{
    PtrContext newContext = NEW (Context);

    newContext->depth = depth;
    newContext->types = types;
    newContext->instances = instances;
    newContext->procedures = procedures;
    return newContext;
}

/* AddContextMarker : copy the given context and add ContextMarker's to the lists therein */
PtrContext AddContextMarker (PtrContext context)
{
    return NewContext (context->depth + 1,
      NewTypeList (ContextMarkerTypeObj, context->types),
      NewInstanceList (ContextMarkerInstanceObj, context->instances), NewProcedureList (ContextMarkerProcedureObj, context->procedures));
}

/* LookupType : look up the named type in the context context, if localLookup == true then only search
   back to the first context marker */
PtrType LookupType (PtrContext context, tIdent ident, bool localLookup)
{
    PtrTypeList types = context->types;

    /* FOR_EACH (types, (localLookup ? CAR (types)->nature != ContextMarkerType : true), ...) */
    while (types && (localLookup ? CAR (types)->nature != ContextMarkerType : true))
    {
        if (CAR (types)->ident == ident)
            return CAR (types);
        types = CDR (types);
    }
    return NoType;
}

/* LookupProcedure : look up the named procedure in the context context, acts like LookupType */
PtrProcedure LookupProcedure (PtrContext context, tIdent ident, bool localLookup)
{
    PtrProcedureList procedures = context->procedures;

    /* FOR_EACH (procedures, (localLookup ? CAR (procedures)->nature != ContextMarkerProcedure : true), ...) */
    while (procedures && (localLookup ? CAR (procedures)->nature != ContextMarkerProcedure : true))
    {
        if (CAR (procedures)->ident == ident)
            return CAR (procedures);
        procedures = CDR (procedures);
    }
    return NoProcedure;
}

/* LookupInstance : look up the named instance in the context context, if localLookup == true then only search
   back to the first context marker */
PtrInstance LookupInstance (PtrContext context, tIdent ident, bool localLookup)
{
    PtrInstance ret;

    ret = LookupInstanceInInstanceList (context->instances, ident, localLookup);
    return ret;
}

static void PrintFileNumbers (tIdent file, int val, FILE * stream)
{
    fprintf (stream, "(file %d \"%s\")\n", val, PeekString (file));
}

/* StrPtrSBreezeContext : Print a context to a SBreeze file (using SBreeze syntax)
	Only print things which are match one of the scopes in `scopes' (eg. ImportedScope | InnerScope) */
void StrPtrSBreezeContext (FILE * stream, PtrContext context, bool onlyPrintLocals, Scope scopes)
{
    PtrInstanceList instances = context->instances;

    if (DoAndrewsMagicSwitch)
    {
        fprintf (stream, ";;; Files\n");
        g_hash_table_foreach (VisitedFiles, (GHFunc) PrintFileNumbers, (gpointer) stream);
    }

    fprintf (stream, ";;; Types\n");
    StrPtrSBreezeTypeList (stream, context->types, onlyPrintLocals, scopes, "\n");
    fprintf (stream, "\n;;; Constants\n");
    while (instances && (!onlyPrintLocals || CAR (instances)->nature != ContextMarkerInstance))
    {
        if (((int) CAR (instances)->scope & (int) scopes) || CAR (instances)->nature == ContextMarkerInstance)
        {
            switch (CAR (instances)->nature)
            {
            case ConstantInstance:
                {
                    bool isImplicant = CAR (instances)->info.constant.value.dontCares != NULL;

                    fprintf (stream, "(%s \"", (isImplicant ? "implicant" : "constant"));
                    WriteIdent (stream, CAR (instances)->ident);
                    fprintf (stream, "\" ");
                    StrPtrMP_INT (stream, CAR (instances)->info.constant.value.baseValue);
                    putc (' ', stream);
                    if (isImplicant)
                    {
                        StrPtrMP_INT (stream, CAR (instances)->info.constant.value.dontCares);
                        putc (' ', stream);
                    }
                    StrPtrSBreezeTypeName (stream, CAR (instances)->type);
                    fprintf (stream, ")\n");
                }
                break;
            case ContextMarkerInstance:
                fprintf (stream, ";;; context-marker\n");
                break;
            default:
                break;
            }
        }
        instances = CDR (instances);
    }
    fprintf (stream, "\n;;; Parts\n");
    /* FIXME */
    StrReversePtrSBreezeProcedureList (stream, context->procedures, onlyPrintLocals /* locals */ , scopes,
      "\n", true /* longForm */ );
    fprintf (stream, "\n");
}

/* ContextDeclaresBuiltins : search the local scope of the given context's functions and types
	and return true if any of them are declared builtin */
bool ContextDeclaresBuiltins (PtrContext context)
{
    bool ret = false;
    PtrProcedureList procedures = context->procedures;
    PtrTypeList types = context->types;

    while (!ret && procedures && CAR (procedures)->nature != ContextMarkerProcedure)
    {
        if (CAR (procedures)->nature == BuiltinFunction || CAR (procedures)->nature == ParameterisedBuiltinFunction)
        {
            ret = true;
        }
        procedures = CDR (procedures);
    }

    while (!ret && types && CAR (types)->nature != ContextMarkerProcedure)
    {
        if (CAR (types)->hasBuiltinElements)
        {
            ret = true;
        }
        types = CDR (types);
    }

    return ret;
}
