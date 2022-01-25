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

	`contexts.h'
	local definition lists for tree decoration/evaluation
	of `balsa', contexts are hierachical symbol tables.
	
 */

#ifndef CONTEXTS_HEADER
#define CONTEXTS_HEADER

#include "types.h"
#include "instances.h"
#include "procedures.h"
#include "misc.h"

/* Context : Type to represent definitions, contains three lists:
   types, instances, procedures */
typedef struct Context
{
    unsigned depth;             /* Nesting depth of contexts, == number of context markers */
    PtrTypeList types;
    PtrInstanceList instances;
    PtrProcedureList procedures;
}
Context, *PtrContext;

/* globalContext : contains default incoming (top level) definitions */
/* topLevelContext : contains the totallity of the top level defns. (after semantic analysis) */

/* PredefinedContext : context implicitly present at the start of any balsa/breeze program */
/* TopLevelContext : the context containing all block level definitions after semantic analysis */
extern PtrContext PredefinedContext;
extern PtrContext TopLevelContext;

/* TopLevelContextDepth : depth of top level definitions, initialised during first block level
   declaration compilation. This can be used in lieu of TopLevelContext->depth *during* compilation */
extern unsigned TopLevelContextDepth;

/* BeginContexts : set up the predefined context */
extern void BeginContexts (void);

/* NewContext : make a new context with the given parameters */
extern PtrContext NewContext (unsigned depth, PtrTypeList types, PtrInstanceList instances, PtrProcedureList procedures);

/* AddContextMarker : copy the given context and add ContextMarker's to the lists therein */
extern PtrContext AddContextMarker (PtrContext context);

/* LookupType : look up the named type in the context context, if localLookup == true then only search
   back to the first contex tmarker */
extern PtrType LookupType (PtrContext context, tIdent ident, bool localLookup);

/* LookupProcedure : look up the named procedure in the context context, acts like LookupType */
extern PtrProcedure LookupProcedure (PtrContext context, tIdent ident, bool localLookup);

/* LookupInstance : look up the named instance in the context context, if localLookup == true then only search
   back to the first context marker */
extern PtrInstance LookupInstance (PtrContext context, tIdent ident, bool localLookup);

/* StrPtrSBreezeContext : Print a context to a SBreeze file (using SBreeze syntax)
	Only print things which are match one of the scopes in `scopes' (eg. ImportedScope | InnerScope) */
extern void StrPtrSBreezeContext (FILE * stream, PtrContext context, bool onlyPrintLocals, Scope scopes);

/* ContextDeclaresBuiltins : search the local scope of the given context's functions and types
	and return true if any of them are declared builtin */
extern bool ContextDeclaresBuiltins (PtrContext context);

#endif /* CONTEXTS_HEADER */
