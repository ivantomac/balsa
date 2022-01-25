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

	`parts.c'
	Functions/Types to aid in recovery of procedures from parts
	
 */

#ifndef PARTS_HEADER
#define PARTS_HEADER

#include "contexts.h"
#include "misclists.h"
#include "components.h"

/* Ident of `is-builtin-function' */
extern tIdent IsBuiltinFunctionIdent;

/* BeginParts : initialise this module, apres BeginIdents */
extern void BeginParts (void);

/* NewAttributedProcedure : make a new procedure and populate it with the given 
   part information (ident and attributes) */
extern PtrProcedure NewAttributedProcedure (tIdent ident, PtrLispList attributes, unsigned portCount, tPosition position);

/* HandleParameter : handle to correct placement of a parameter into a component,
   set actualSpec to a string of possible types for the object eg. "#i" for IntegerParameter,
   paramNo is the parameter number (left->right, based at 0).
   If `parameters' is not NULL then use that as the place to hold the parameters values */
extern void HandleParameter (Ptrchar actualSpec, unsigned paramNo,
  PtrComponent component, void *actualValue, PtrContext context, PtrComponentParameterList parameters, tPosition position);

/* NewComponentFromComponentName : create a new component from the given ident,
   make a $BrzWhatEver component if the ident is valid and internal is true, 
   otherwise lookup the name as a procedure in context and make a ProcedureComponent */
extern PtrComponent NewComponentFromComponentName (PtrContext context, tIdent ident, bool internal, tPosition position);

/* HandleComp : handle resolution of component bundles (parsing breeze),
   add component to incoming list if all is OK */
extern PtrComponentList HandleComp (PtrComponentList list,
  PtrComponent component, PtrintList channelNumbers, PtrWireArray wires, tPosition position, PtrLispList options);

/* HandleUndeclaredComp : handle an undeclared component and all the extra port/parameter
	complexity that entails */
extern PtrComponentList HandleUndeclaredComp (PtrComponentList list,
  PtrComponent component,
  PtrintList channelNumbers,
  PtrWireArray wires,
  tPosition position,
  tIdent componentType, tIdent baseComponentName, PtrComponentParameterList parameters, PtrInstanceList ports, PtrLispList options);

#endif /* PARTS_HEADER */
