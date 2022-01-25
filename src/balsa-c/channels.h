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

	`channels.h'
	Channel expression handling functions
	
 */

#ifndef CHANNELS_HEADER
#define CHANNELS_HEADER

#include "exprs.h"
#include "contexts.h"

/* MakeAttributesListForChannelInstancesAccesses: generate a list of accesses for a list
   of channel instances.  Only produces a maximum of `instanceCount' accesses */
extern PtrExprAttributesList
MakeAttributesListForChannelInstancesAccesses (PtrInstanceList instances,
  unsigned instanceCount, bool isInput, bool isPassive, bool lockChannels, tPosition position);

/* HandleIdentChannel : resolve an instance representing a channel and
   generate the appropriate access hardware */
extern ExprAttributes HandleIdentChannel (PtrContext context,
  PtrInstance * instance, bool isInput, bool isPassive, bool lockChannels, tPosition position);

/* HandleArrayedElemChannel : resolve a arrayed access on a channel remain quiet
   if an error was reported `upstream', *instance will contain on exit the same things as in
   HandleIdentChannel. */
extern ExprAttributes HandleArrayedElemChannel (PtrContext context,
  ExprAttributes lhsAttr, PtrInstance lhsInstance, ExprAttributes expr, PtrInstance * instance, bool isInput, bool isPassive, tPosition position);

/* HandleArrayedSliceChannel : resolve a arrayed access on a arrayed channel into another arrayed
   channel with elements as per the indices over `range'.  Remain quiet if an error was reported `upstream',
   *instance will contain on exit the same things as in HandleIdentChannel. */
extern ExprAttributes HandleArrayedSliceChannel (PtrContext context,
  ExprAttributes lhsAttr, PtrInstance lhsInstance, Span range, PtrInstance * instance, bool isInput, bool isPassive, tPosition position);

/* HandleArrayedAppendChannels : produce a single arrayed channel lvalue from a pair of others.
   Remain quiet if an error was reported `upstream', *instance will contain on exit the same things
   as in HandleIdentChannel. */
extern ExprAttributes HandleArrayedAppendChannels (PtrContext context,
  ExprAttributes lhsAttr,
  PtrInstance lhsInstance, ExprAttributes rhsAttr, PtrInstance rhsInstance, PtrInstance * instance, bool isInput, bool isPassive, tPosition position);

/* HandleArrayedConsChannels : produce a single arrayed channel lvalue from a list of attributes
   corresponding to accesses on individual channels.  Remain quiet if an error was reported `upstream',
   *instance will contain on exit the same things as in HandleIdentChannel. */
extern ExprAttributes HandleArrayedConsChannels (PtrContext context,
  PtrType baseType, PtrExprAttributesList attrs, PtrInstance * instance, bool isInput, bool isPassive, tPosition position);

/* FlattenDisjointChannelAccessList : reduce an ExprAttributesList containing passive channel accesses
   (test this, and that there is no overlap in accesses) into a single ExprAttributes value.
   NB. this will affect the wires and accesses elements of the return value */
extern ExprAttributes FlattenDisjointChannelAccessList (PtrExprAttributesList attrs, tPosition position);

#endif
