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

	`lvalues.h'
	Lvalue expression handling functions
	
 */

#ifndef LVALUES_HEADER
#define LVALUES_HEADER

#include "exprs.h"
#include "contexts.h"

/* HandleIdentLvalue : resolve an instance representing a writable variable and
   generate the appropriate access hardware.  Invalidates (makes NULL) *instance
   on error */
extern ExprAttributes HandleIdentLvalue (PtrContext context,
  PtrInstance * instance, PtrSpanList * partitions, PtrAccess * access, tPosition position);

/* HandleArrayElemLvalue : resolve a array access on a variable, remain quiet
   if an error was reported `upstream', *instance will contain on exit the same things as in
   HandleIdentLvalue. */
extern ExprAttributes HandleArrayElemLvalue (PtrContext context,
  ExprAttributes lhsAttr,
  PtrInstance lhsInstance,
  PtrSpanList lhsPartitions,
  PtrSpanListList lhsIndices,
  PtrWire lhsIndexWire,
  ExprAttributes expr,
  PtrInstance * instance, PtrSpanList * partitions, PtrSpanListList * indices, PtrWire * indexWire, PtrAccess access, tPosition position);

/* HandleRecordElemLvalue : handle the assignment into variable record elements */
extern ExprAttributes HandleRecordElemLvalue (ExprAttributes lhsAttr,
  PtrInstance lhsInstance,
  PtrSpanList lhsPartitions, tIdent elem, PtrInstance * instance, PtrSpanList * partitions, PtrAccess access, tPosition position);

/* FinaliseLvalue : place the hardware for an lvalue.  Creates a wire for non
   array accesses, a whole load of hardare (Case component, transferrers ...) for array accesses */
extern ExprAttributes FinaliseLvalue (ExprAttributes attributes,
  PtrInstance instance, PtrSpanList partitions, PtrSpanListList indices, PtrWire indexWire, PtrAccess access, tPosition position);

#endif
