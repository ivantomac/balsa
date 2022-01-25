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

	`wires.h'
	Functions/Type to store lists of handshake channels and their associated parameters
	
 */

#ifndef WIRES_HEADER
#define WIRES_HEADER

#include <Positions.h>
#include "misclists.h"
#include "types.h"
#include <stdio.h>

struct Component;               /* Forward decls. */
union Tree_Node;
struct AccessList;

/* Wire : a Wire is a description of a channel in the intermediate form. Wires have only a width and 
   a unique (within context) identifying number and sense (push/pull), in a WireList the entries are ordered
   (highest numberd) -> (highest-1) ... -> (lowest) -> NULL */
typedef struct Wire
{
    unsigned bundleNo;          /* Unique number of this wire, NB. a bundle number of 0 indicates this
                                   wire should be swept (see. SweepWireList) */
    tPosition position;         /* Position of wire declaration in code */
    Bits width;                 /* Number of bits in channel for wire */
    union
    {
        PtrType type;           /* Type of data on this wire */
        union Tree_Node *tree;  /* Tree leading to type */
    }
    type;
    Bits offset;                /* If this is a variable read/write then the offset into that
                                   instance at which this wire starts eg. offset=2, width=5 relates to range [6:2] */
    bool isPull;                /* false: push, true: pull */
    tIdent ident;               /* non-unique wire name */
    PtrLispList options;        /* Wire options not processed by balsa-c but to be passed on */

    struct Component *activeComponent; /* components at active/passive ends, NULL == no component */
    struct Component *passiveComponent;
    unsigned activePort;        /* port number on active/passive component (indexed from 0) */
    unsigned passivePort;

	struct AccessList *accesses; /* accesses if this is an activation sync channel */

    struct Wire *replacementWire;
}
Wire, *PtrWire;

/* UnknownWireAccesses : the accesses here are not just empty, they are uncertain.
	Only use with wire access optimisation and NewSyncWire */
struct AccessList *UnknownWireAccesses;

DECLARE_CONS_LIST_TYPE (Wire) DECLARE_CONS_LIST_DEEP_COPY (Wire) DECLARE_CONS_LIST_ARRAY_DEEP_COPY (Wire)
/* WireWidth : returns the .width field of a wire or -1 if the wire is NULL,
   useful for situations where wire->width might cause a SIG11 */
#define WireWidth(wire) ((wire) ? (wire)->width : -1)
/* WIRE_ARRAY_TO_LIST : convert a given wire array (of size size) to a list pointer,
   NB. the wire array must already be correctly linked */
#define WIRE_ARRAY_TO_LIST(array,size) ((PtrWireList)(array)+((size)-1))
/* WIRE_LIST_TO_ARRAY : convert a wire list (which was originally created as a wire array
   back into an array, DANGER this should be used with extreme care */
#define WIRE_LIST_TO_ARRAY(list,size) ((PtrWireArray)(list)-((size)-1))
/* NewWire : create a wire */
extern PtrWire NewWire (unsigned bundleNo, Bits width, PtrType type, Bits offset, bool isPull, tPosition position);

/* NewSyncWireWithAccesses : as NewSyncWire but has some certain accesses */
extern PtrWire NewSyncWireWithAccesses (unsigned bundleNo, struct AccessList *accesses, tPosition position);

/* NewSyncWire : make a wire of sync type with bundle number bundleNo and possibly some accesses
	(if this channel is an activation somewhere) */
extern PtrWire NewSyncWire (unsigned bundleNo, tPosition position);

/* CopyAndRenumberWire : copy a wire's parameters into a new wire marking it with bundle number bundleNo and
   declaration position position */
extern PtrWire CopyAndRenumberWire (PtrWire wire, unsigned bundleNo, tPosition position);

/* CopyWire : simple wire copy */
extern PtrWire CopyWire (PtrWire wire);

/* NameWire : give a name to a wire */
extern void NameWire (PtrWire wire, tIdent name);

/* SwapWireArrayElements : swap the two elements of the given wire array.  Note
   that the element numbers are 1 based and so element 1 corresponds to the wire
   with bundle number 1 in a correctly sorted list */
extern void SwapWireArrayElements (PtrWireArray wires, unsigned elem1, unsigned elem2);

/* ConvertWireListToArray : convert a wire list to an array of wires, renumbering
	as we go.  Returns the array pointer and modifies `size' (if not NULL) to reflect
	the length of the list. Deallocates the list links. */
extern PtrWireArray ConvertWireListToArray (PtrWireList list, unsigned *size);

/* CopyAndRemapWiresOfWireList : shallow copy a wire list and change each element for the same
   numbered element (by position CAR(newWireArray[bundleNo - 1])) in the given wire array */
extern PtrWireList CopyAndRemapWiresOfWireList (PtrWireList wires, PtrWireArray newWireArray);

/* NextBundleNumber : takes a wire list (remember these are ordered N -> N-1 ...) and returns the
   first bundle number not in the list (either CAR(wires)->bundleNo+1 or 1 iff wires == NULL */
extern unsigned NextBundleNumber (PtrWireList wires);

/* RenumberWireList : renumber a wire list from n to 1 (where n is the length of the list), renumbering
   works from the start of the list until an in-sequence bundle number is found eg.
   RenumberWireList (5 -> 4 -> 3 -> 2 -> 1 -> 4 -> 3 -> 2 -> 1 -> NULL) renumbers the elements
   ^^^^^^^^^^^^^^^^^^^^^ as ^ this element already has the correct number and 
   WireLists are guarenteed to be sequentially numbered and in decending order down to 1
   if fully is true then just renumber the whole list */
extern void RenumberWireList (PtrWireList list, bool fully);

/* MarkWire : mark a wire as ready to be swept (ie. set wire number to 0) */
extern void MarkWire (PtrWire wire);
extern void MarkWireWithReplacement (PtrWire wire, PtrWire replacementWire);

/* SweepWireList : traverse a wire list and remove elements with bundleNumber == 0, this
   function relinks the original list and returns a pointer to the new list head.
   The returned list has its elements correctly renumbered */
extern PtrWireList SweepWireList (PtrWireList list);

/* RemoveWireFromWireList : remove the given wire from the given list, possibly
   modifying the PtrWireList */
extern void RemoveWireFromWireList (PtrWire wire, PtrWireList * list);

/* GetComponentFromWire : return the component to which this wire is connected,
   activeNpassive selects which component field is returned */
extern struct Component *GetComponentFromWire (PtrWire wire, bool activeNpassive);

/* SubstituteWireForWireInWireList : remove the `from' wire if found in the given list
   and replace with `to' wire */
extern void SubstituteWireForWireInWireList (PtrWireList wires, PtrWire from, PtrWire to);

/* WidthOfWireList : returns the width of all the wires in the given list added together */
extern Bits WidthOfWireList (PtrWireList wires);

/* StrPtrWire : print out wire info */
extern void StrPtrWire (FILE * stream, PtrWire wire);

/* StrPtrWireList : print out a list of wires, iff longForm == true then print out all wire info,
   otherwise just the bundle number for that wire. If bundleNumberFormat is true then use
   the format #n otherwise just print the number for the short form print */
extern void StrPtrWireList (FILE * stream, PtrWireList list, bool longForm, char *separator, bool bundleNumberFormat);

/* StrNElementsOfPtrWireList : print out only the first n elements of the given list,
   return a pointer to the rest of the list. If bundleNumberFormat is true then use
   the format #n otherwise just print the number for the short form print */
extern PtrWireList StrNElementsOfPtrWireList (FILE * stream, int n, PtrWireList list, bool longForm, char *separator, bool bundleNumberFormat);

/* StrPtrSBreezeWires : print out a wire list in the sbreeze format,
   (sync) is a sync wire, (pull n) is a pull wire of width n, (push n) is ...
   Returns a count of the number of wires printed. */
extern int StrPtrSBreezeWires (FILE * stream, PtrWireList wires, char *leadingTabs);

/* BeginWires : initialise globals for this file */
extern void BeginWires (void);

#endif /* WIRES_HEADER */
