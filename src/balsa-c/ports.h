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

	`ports.h'
	Functions etc. to manipulate procedure/function/part ports
	
 */

#ifndef PORTS_HEADER
#define PORTS_HEADER

#include "instances.h"
#include "procedures.h"
#include "contexts.h"
#include "misclists.h"

/* HandleChannelPorts : handle the declaration of a group of formal ports for
   a procedure / part or function, returns an augmented
   version of the incomingPorts */
extern PtrInstanceList HandleChannelPorts (PtrIdentList idents, PtrType type,
  PortSense portSense,
  ProcedureArgsType portsType,
  bool isOutput, bool isArray,
  Span range, PtrInstanceList incomingPorts, PtrInstanceList * arrayedElements, unsigned *portCount, PtrLispList options, tPosition position);

/* HandleValuePorts : handle the declaration of value ports for functions, creates RO
   variable in a similar vein to those used with FalseVariable's */
extern PtrInstanceList HandleValuePorts (PtrIdentList idents, PtrType type,
  ProcedureArgsType portsType, PtrInstanceList incomingPorts, unsigned *portCount);

/* MakePortSpecString : build a spec string (like the component specs, eg. iOI)
   for a given port list, prepend preString to that spec list */
extern Ptrchar MakePortSpecString (PtrInstanceList ports, unsigned portListMaxLength);

union Tree_Node;

/* ConvertCoercedExprToLinkedChannel : convert an actual-port-representing CoercedExpr tTree
	node into a LinkedChannel */
extern union Tree_Node *ConvertCoercedExprToLinkedChannel (union Tree_Node *expr, PtrContext context, PtrInstanceList formalPorts);

/* ConvertFunctionPortsToProcedureComponentPorts : convert the usual ValuePort
	list of a function to a set of passive input ports like those of the
	part for that function. */
extern PtrInstanceList ConvertFunctionPortsToProcedureComponentPorts (PtrInstanceList ports, PtrType returnType);

/* StrPortSense : write out the port sense (- default -), active, passive */
extern void StrPortSense (FILE * stream, PortSense sense);

#endif /* PORTS_HEADER */
