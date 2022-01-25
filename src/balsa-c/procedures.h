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

	`procedures.h'
	Proedure/Shared block/Function (?) handling
	
 */

#ifndef PROCEDURES_HEADER
#define PROCEDURES_HEADER

#include "misclists.h"
#include "commands.h"
#include "instances.h"
#include "values.h"

/* Type to distinguish the differing types of argument lists for different `procedure' types */
typedef enum ProcedureArgsType
{
    ProcedurePorts,
    PartPorts,
    FunctionArgs
}
ProcedureArgsType;

/* What type of executable code block are we talking about? */
typedef enum ProcedureNature
{
    UnSharedProcedure,          /* procedure ... */
    SharedProcedure,            /* shared ... */
    ParameterisedProcedure,
    UnSharedFunction,
    SharedFunction,
    BuiltinFunction,
    ParameterisedBuiltinFunction,
    ContextMarkerProcedure,
    UnresolvableProcedure
}
ProcedureNature;

/* Forward declarations */
struct Context;
union Tree_Node;

typedef struct Procedure
{
    ProcedureNature nature;
    union
    {
        struct
        {
            CommandAttributes attributes; /* Components/Wires/Compilation attrs. */
        }
        procedure;
        struct
        {
            ExprAttributes attributes;
            PtrType returnType;
        }
        function;
        struct
        {
            union Tree_Node *tree; /* KCT tree defn. for procedure (kProcedureDecl) */
        }
        parameterisedProcedure;
        struct
        {
            union Tree_Node *tree;
            PtrType returnType;
            tIdent ident;       /* Ident of parameterised function rather than this instance */
            PtrComponentParameterList parameters;
        }
        parameterisedFunction;
    }
    info;
    PtrAccessList unplacedAccesses; /* Copy of attributes.accesses made `unplaced' */
    PtrWire unplacedActivation; /* hanging activation created from compiled calls to shared proc */
    PtrInstance dummyInstance;  /* dummyInstance entry used to sequence SharedCallAccesses in access lists */
    PtrInstanceList ports;      /* Ports, not including activation ? */
    PtrInstanceList extraPorts; /* Port elements which are are part of arrayed ports */
    unsigned portCount;         /* Number of port channels after arrayed port expansion */
    struct Context *context;    /* Context previous to the declaration of this procedure */
    Ptrchar portSpec;           /* ioSiso type port spec for this procedure */
    bool isExternal;            /* The body of this procedure/function is not defined in balsa/breeze */
    tPosition position;         /* Declaration position */
    tIdent ident;               /* name of procedure ... */
    Scope scope;                /* declaration scope */
    PtrLispList userAttributes; /* attributes read from Breeze file, not recognised/reconstructed
                                   by balsa-c */
    PtrLispList activationOptions; /* Breeze options for the activation port */
}
Procedure, *PtrProcedure;

DECLARE_CONS_LIST_TYPE (Procedure)
/* NULL object and Context marking objects */
extern PtrProcedure NoProcedure;
extern PtrProcedure ContextMarkerProcedureObj;

/* balsa.types.builtin functions */
extern PtrProcedure WriteMessageFunction;
extern PtrProcedure StringAppendFunction;
extern PtrProcedure ToStringFunction;
extern PtrProcedure StringFunction;

/* BeginProcedures : wouldn't Modula-2 make this sooo much easier */
extern void BeginProcedures (void);

/* NewProcedure : produce a bare procedure structure */
extern PtrProcedure NewProcedure (tIdent name, ProcedureNature nature, unsigned portCount, tPosition position);

/* CopyPtrProcedure : make a shallow copy of a PtrProcedure */
extern PtrProcedure CopyPtrProcedure (PtrProcedure proc);

/* ReassignPortWires : reassign the wire allocations of the procedure proc's ports so
   that activation is wire 1, the first port is wire 2 and so on */
extern void ReassignPortWires (PtrProcedure proc);

/* GetNextBalsaParam: find the next identifier and parameter tTree from the given identifier list
   (from the head of the given parameters) and a list of parameters.  Returns the head parameter
   and modifies `parameters', `ident' and `idents' to hold the parameter list tail, head identifier
   and identifier list tail respectively. */
extern union Tree_Node *GetNextBalsaParam (union Tree_Node **parameters, tIdent * ident, PtrIdentList * idents, tPosition position);

/* HandleExprParam: make an expression parameter instance from a parameter KCT tree and some
   other info.  Used to expand specified procedures. */
extern union Tree_Node *HandleExprParam (union Tree_Node *formalParams,
  struct Context **callingContext,
  PtrIdentList * remainingIdents, PtrInstanceList * constants, PtrComponentParameterList * params, TypedValue val, tPosition position);

/* HandleTypeParam: make a type parameter instance from a parameter KCT tree and some
   other info.  Used to expand specified procedures. */
extern union Tree_Node *HandleTypeParam (union Tree_Node
  *formalParameters,
  struct Context **callingContext,
  PtrIdentList * remainingIdents, PtrTypeList * types, PtrComponentParameterList * params, PtrType type, tPosition position);

/* MakeSpecifiedProcedureName: make a unique name for a specified parameterised procedure.
   The name will be of the form baseName-<unique-integer> */
extern tIdent MakeSpecifiedProcedureName (tIdent baseName);

/* StrBreezePortOptions : print a port trailing options in the Breeze format */
extern void StrBreezePortOptions (FILE * stream, PtrLispList options, tPosition position);

/* StrPtrSBreezeProcedure : print out a procedure defn, longForm will print out the full contents of
   the procedures attributes (ie. will generate breeze) */
extern void StrPtrSBreezeProcedureList (FILE * stream,
  PtrProcedureList procedures, bool onlyPrintLocals, Scope scopes, char *separator, bool longForm);

/* StrReversePtrSBreezeProcedureList : as above but print the list in reverse */
extern void StrReversePtrSBreezeProcedureList (FILE * stream,
  PtrProcedureList instances, bool onlyPrintLocals, Scope scopes, char *separator, bool longForm);

/* StrPtrBreezePortList : print out the given list's port elements in the SBreeze format */
extern void StrPtrBreezePortList (FILE * stream, PtrInstanceList ports, char *separator);

#endif /* PROCEDURES_HEADER */
