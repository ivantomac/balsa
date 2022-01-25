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

	`block.h'
	Functions for resolving local accesses and
	declarations into components and errors
	
 */

#ifndef BLOCK_HEADER
#define BLOCK_HEADER

#include "contexts.h"
#include "accesses.h"

/* ResolveLocalSharedCallAccesses : resolve the connections to shared blocks, place
   the shared block code and connect up activations.
   NB. this function traverses the context.procedures to detect used shared blocks (those
   having non NULL unplacedActivations) */
extern PtrAccessList ResolveLocalSharedCallAccesses (PtrContext context,
  PtrWireList * wires, PtrComponentList * comps, PtrAccessList accesses, tPosition position);

/* ResolveLocalInstanceAccesses : place components for local variable/channel accesses.
   places split/combine components for multi variable (variable part) reads.
   If *`initWire' is set then some initialisation was required by the components in this
   access list and the wires left in `initWires' are the activations for that initialisation */
extern PtrAccessList ResolveLocalInstanceAccesses (PtrContext context,
  PtrWireList * wires, PtrComponentList * comps, PtrAccessList accesses, PtrWireList * initWires, tPosition position);

/* CreateVariablePartitions : create an array of length 'length' + 1 with true in all
   the places one element after the end of a partition, starting from 0
   a partition is the longest run of elements which don't contain a 1 in
   any place other than the first element eg. 10010000010001 is 3 runs of
   3 6 and 4 bits, NB the start and end of partitions are marked */
extern bool *CreateVariablePartitions (PtrWireList accesses, Bits length);

/* CreateWriteConnections : Add variable write components to comps, update bundleCount and
   fill in the write port field of the writeParts */
extern void CreateWriteConnections (PtrComponentList writeParts,
  PtrWireList accesses, PtrWireList * wires, PtrComponentList * comps, tPosition position);

/* CreateReadConnections : Add variable read components to comps, update bundleCount and
   fill in the read ports fields of the variables. */
extern void CreateReadConnections (PtrComponentList readParts,
  PtrWireList accesses, PtrWireList * wires, PtrComponentList * comps, tPosition position);

/* TrimDanglingVariables: convert variables in the given component list
   into push continue components if no read ports are present, eliminate
   Variables with no ports at all. */
extern PtrComponentList TrimDanglingVariables (PtrComponentList writeParts);

/* CreatePartitionedVariableComponents : Create a component list of parts of a variable from
   a given 'partition' (from the function CreateVariablePartitions), 
   each component entry will have the offset and width of the corresponding (working
   upwards from 0) variable part. If `makeFalseVariables' is true then produce FVs
   instead of real variables.  If `makeEagerFalseVariables' is true then produce Passive or
   Active EagerFVs instead of normal FVs, depending on eagerFVsAreActive.
   If `initialisedValue' is not NULL and we're not generating
   FVs then create InitVariable components with appropriate initValues. */
extern PtrComponentList CreatePartitionedVariableComponents (tIdent name, bool * partition, Bits partitionLength, bool makeFalseVariables,
  bool makeEagerFalseVariables, bool eagerFVsAreActive, PtrMP_INT initialisedValue, tPosition position);

/* SequenceBlockInitialisationAndCommand : add the control components to sequence
	block command and initialisation operations without resolving instance accesses.
	Returns the new activation channel for the block. */
extern PtrWire SequenceBlockInitialisationAndCommand (PtrWire
  commandActivation, PtrWireList * wires, PtrComponentList * comps, PtrWireList initWires, tPosition position);

#endif /* BLOCK_HEADER */
