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

#include "wires.h"
#include "components.h"
#include "flags.h"
#include "accesses.h"

#define WIRE_NAME_LEN (32)
/* NewWire : create a wire */
PtrWire NewWire (unsigned bundleNo, Bits width, PtrType type, Bits offset, bool isPull, tPosition position)
{
    Ptrchar wireName = NEW_ARRAY (char, WIRE_NAME_LEN);
    PtrWire wire = NEW (Wire);

    wire->bundleNo = bundleNo;
    wire->width = width;
    wire->type.type = type;
    wire->offset = offset;
    wire->isPull = isPull;
    wire->position = position;
    wire->options = NULL;
    /* FIXME, better default name
       sprintf (wireName, "@%d:%d", position.Line, position.Column);
     */
    *wireName = '\0';
    wire->ident = MakeIdent1 (wireName);
    wire->activeComponent = NULL;
    wire->activePort = 0;
    wire->passiveComponent = NULL;
    wire->passivePort = 0;
    wire->accesses = UnknownWireAccesses;
    FREE_ARRAY (char, WIRE_NAME_LEN, wireName);

    return wire;
}

PtrAccessList UnknownWireAccesses = NULL;

/* NewSyncWireWithAccesses : as NewSyncWire but has some certain accesses */
PtrWire NewSyncWireWithAccesses (unsigned bundleNo, PtrAccessList accesses, tPosition position)
{
	PtrWire ret = NewWire (bundleNo, 0, SyncTypeObj, 0, false /* sync's are push */ , position);
	ret->accesses = accesses;

	return ret;
}

/* NewSyncWire : make a wire of sync type with bundle number bundleNo and possibly some accesses
	(if this channel is an activation somewhere) */
PtrWire NewSyncWire (unsigned bundleNo, tPosition position)
{
	return NewSyncWireWithAccesses (bundleNo, UnknownWireAccesses, position);
}

/* CopyAndRenumberWire : copy a wire's parameters into a new wire marking it with bundle number bundleNo and
   declaration position position */
PtrWire CopyAndRenumberWire (PtrWire wire, unsigned bundleNo, tPosition position)
{
    PtrWire newWire = CopyWire (wire);

    newWire->bundleNo = bundleNo;
    return newWire;
}

/* CopyWire : simple wire copy */
PtrWire CopyWire (PtrWire wire)
{
    PtrWire newWire = NEW (Wire);

    *newWire = *wire;
    return newWire;
}

/* NameWire : give a name to a wire */
void NameWire (PtrWire wire, tIdent name)
{
    wire->ident = name;
}

DEFINE_CONS_LIST_TYPE (Wire) DEFINE_CONS_LIST_DEEP_COPY (Wire) DEFINE_CONS_LIST_ARRAY_DEEP_COPY (Wire)
/* SwapWireArrayElements : swap the two elements of the given wire array.  Note
   that the element numbers are 1 based and so element 1 corresponds to the wire
   with bundle number 1 in a correctly sorted list */
void SwapWireArrayElements (PtrWireArray wires, unsigned elem1, unsigned elem2)
{
    PtrWire temp;

    if (elem1 != elem2)         /* Don't bother if the indices match */
    {
        temp = wires[elem1 - 1].body;
        wires[elem1 - 1].body = wires[elem2 - 1].body;
        wires[elem2 - 1].body = temp;
    }
}

/* ConvertWireListToArray : convert a wire list to an array of wires, renumbering
	as we go.  Returns the array pointer and modifies `size' (if not NULL) to reflect
	the length of the list. Deallocates the list links. */
PtrWireArray ConvertWireListToArray (PtrWireList list, unsigned *size)
{
    unsigned length = LengthOfWireList (list);
    PtrWireArray ret = NULL;

    if (length != 0)
    {
        int wireNo = length;
        PtrWireList tmpWireList;

        ret = (PtrWireArray) PrepareConsArray (length);

        while (wireNo > 0)
        {
            CAR (list)->bundleNo = wireNo;
            ret[wireNo - 1].body = CAR (list);
            tmpWireList = list;
            list = CDR (list);
            FREE_ARRAY (WireList, 1, tmpWireList);
            wireNo--;
        }
    }

    if (size)
        *size = length;

    return ret;
}

/* CopyAndRemapWiresOfWireList : shallow copy a wire list and change each element for the same
   numbered element (by position newWireArray[bundleNo - 1]) in the given wire array */
PtrWireList CopyAndRemapWiresOfWireList (PtrWireList wires, PtrWireArray newWireArray)
{
    PtrWireList newWires = CopyWireList (wires);
    PtrWireList newWiresIter = newWires;

    while (newWiresIter)
    {
        newWiresIter->body = newWireArray[CAR (newWiresIter)->bundleNo - 1].body;
        newWiresIter = CDR (newWiresIter);
    }
    return newWires;
}

/* NextBundleNumber : takes a wire list (remember these are ordered N -> N-1 ...) and returns the
   first bundle number not in the list (either CAR(wires)->bundleNo+1 or 1 iff wires == NULL */
unsigned NextBundleNumber (PtrWireList wires)
{
    return (wires ? CAR (wires)->bundleNo + 1 : 1);
}

/* RenumberWireList : renumber a wire list from n to 1 (where n is the length of the list), renumbering
   works from the start of the list until an in-sequence bundle number is found eg.
   RenumberWireList (5 -> 4 -> 3 -> 2 -> 1 -> 4 -> 3 -> 2 -> 1 -> NULL) renumbers the elements
   ^^^^^^^^^^^^^^^^^^^^^ as ^ this element already has the correct number and 
   WireLists are guarenteed to be sequentially numbered and in decending order down to 1
   if fully is true then just renumber the whole list */
void RenumberWireList (PtrWireList list, bool fully)
{
    unsigned length = LengthOfWireList (list);

    while (list)
    {
        ASSERT (fully || CAR (list)->bundleNo != 0);
        if (!fully && CAR (list)->bundleNo == length)
            break;              /* Finished */
        CAR (list)->bundleNo = length;
        length--;
        list = CDR (list);
    }
}

/* MarkWire : mark a wire as ready to be swept (ie. set wire number to 0) */
void MarkWire (PtrWire wire)
{
    wire->bundleNo = 0;
}
extern void MarkWireWithReplacement (PtrWire wire, PtrWire replacementWire)
{
    wire->bundleNo = 0;
    wire->replacementWire = replacementWire;
    while (wire->replacementWire->bundleNo == 0 && wire->replacementWire->replacementWire)
    {
        wire->replacementWire = wire->replacementWire->replacementWire;
    }
}

/* SweepWireList : traverse a wire list and remove elements with bundleNumber == 0, this
   function relinks the original list and returns a pointer to the new list head.
   The returned list has its elements correctly renumbered */
PtrWireList SweepWireList (PtrWireList list)
{
    PtrWireList retHead = NULL;
    PtrWireList retTail = NULL;

    while (list)
    {
        if (CAR (list)->bundleNo != 0) /* Link this element */
        {
            if (retHead)
            {
                retTail->next = list;
                retTail = list;
            } else
            {
                retHead = retTail = list;
            }
        }
        list = CDR (list);
    }

    if (retTail)
        retTail->next = NULL;
    RenumberWireList (retHead, false);
    return retHead;
}

/* RemoveWireFromWireList : remove the given wire from the given list, possibly
   modifying the PtrWireList */
void RemoveWireFromWireList (PtrWire wire, PtrWireList * list)
{
    if (CAR (*list) == wire)
    {
        *list = CDR (*list);
    } else
    {
        PtrWireList pre = *list;

        /* While the element after the head of the list != wire -> step */
        /* FOR_EACH (pre, CDR (pre) && CADR (pre) != wire, ...) */
        while (pre && CDR (pre) && CADR (pre) != wire)
        {
            pre = CDR (pre);
        }
        if (CDR (pre))
        {
            pre->next = CDDR (pre); /* Remove element */
        }
    }
}

/* GetComponentFromWire : return the component to which this wire is connected,
   activeNpassive selects which component field is returned */
PtrComponent GetComponentFromWire (PtrWire wire, bool activeNpassive)
{
    return (activeNpassive ? wire->activeComponent : wire->passiveComponent);
}

/* SubstituteWireForWireInWireList : remove the `from' wire if found in the given list
   and replace with `to' wire */
void SubstituteWireForWireInWireList (PtrWireList wires, PtrWire from, PtrWire to)
{
    /* FOR_EACH (wires, true, ...) */
    while (wires && true)
    {
        if (from == CAR (wires))
            wires->body = to;
        wires = CDR (wires);
    }
}

/* WidthOfWireList : returns the width of all the wires in the given list added together */
Bits WidthOfWireList (PtrWireList wires)
{
    Bits ret = 0;

    /* FOR_EACH (wires, true, ...) */
    while (wires)
    {
        ret += CAR (wires)->width;
        wires = CDR (wires);
    }
    return ret;
}

/* StrPtrWire : print out wire info */
void StrPtrWire (FILE * stream, PtrWire wire)
{
    if (!wire)
        fprintf (stream, "NULL");
    else
    {
        putc ('(', stream);
        StrBundleNumber (stream, wire->bundleNo);
        fprintf (stream, " `");
        WriteIdent (stream, wire->ident);
        fprintf (stream, "' ");
        if (wire->width == 0)
            fprintf (stream, " SYNC @ ");
        else
            fprintf (stream, " [%d:%d] %s @ ", wire->offset + wire->width - 1, wire->offset, (wire->isPull ? "PULL" : "PUSH"));
        WritePosition (stream, wire->position);
        fprintf (stream, " ACTIVE{port %d: ", wire->activePort);
        StrPtrComponent (stream, wire->activeComponent);
        fprintf (stream, "}, PASSIVE{port %d: ", wire->passivePort);
        StrPtrComponent (stream, wire->passiveComponent);
        fprintf (stream, "})");
    }
}

/* StrPtrWireNumber : print out a wire in one of the forms: #nnn (bundleNumberFormat is true) or nnn
	(bundleNumberFormat false).  If printName is true then print the name of the wire (if any)
	inside (-- --) comment brackets after the number */
void StrPtrWireNumber (FILE * stream, PtrWire wire, bool bundleNumberFormat, bool printName)
{
    if (bundleNumberFormat)
    {
        StrBundleNumber (stream, wire->bundleNo);
#if 0
        if (printName && wire->ident != NoIdent)
        {
            fprintf (stream, " (-- ");
            WriteIdent (stream, wire->ident);
            fprintf (stream, " --)");
        }
#endif
    } else
        fprintf (stream, "%d", wire->bundleNo);
}

/* StrPtrWireList : print out a list of wires, iff longForm == true then print out all wire info,
   otherwise just the bundle number for that wire. If bundleNumberFormat is true then use
   the format #n otherwise just print the number for the short form print */
void StrPtrWireList (FILE * stream, PtrWireList list, bool longForm, char *separator, bool bundleNumberFormat)
{
    /* FOR_EACH (list, true, ...) */
    while (list)
    {
        if (longForm)
            StrPtrWire (stream, CAR (list));
        else if (!CAR (list))
            fprintf (stream, "NULLWIRE");
        else
            StrPtrWireNumber (stream, CAR (list), bundleNumberFormat, true);
        if (CDR (list))
            fprintf (stream, "%s", separator);
        list = CDR (list);
    }
}

/* StrNElementsOfPtrWireList : print out only the first n elements of the given list,
   return a pointer to the rest of the list. if bundleNumberFormat is true then use
   the format #n otherwise just print the number for the short form print */
PtrWireList StrNElementsOfPtrWireList (FILE * stream, int n, PtrWireList list, bool longForm, char *separator, bool bundleNumberFormat)
{
    /* FOR_EACH (list, n > 0, ...) */
    while (list && n > 0)
    {
        if (longForm)
            StrPtrWire (stream, CAR (list));
        else if (!CAR (list))
            fprintf (stream, "NULLWIRE");
        else
            StrPtrWireNumber (stream, CAR (list), bundleNumberFormat, true);
        n--;
        if (CDR (list) && n > 0)
            fprintf (stream, "%s", separator);
        list = CDR (list);
    }
    return list;
}

/* StrPtrSBreezeWires : print out a wire list in the sbreeze format,
   (sync) is a sync wire, (pull n) is a pull wire of width n, (push n) is ...
   Returns a count of the number of wires printed. */
int StrPtrSBreezeWires (FILE * stream, PtrWireList wires, char *leadingTabs)
{
    int count = 1;

    if (!wires)
        return 0;

    if (CDR (wires))
        count += StrPtrSBreezeWires (stream, CDR (wires), leadingTabs);
    if (CAR (wires)->width == 0)
        fprintf (stream, "%s(sync", leadingTabs);
    else
        fprintf (stream, "%s(%s %d", leadingTabs, (CAR (wires)->isPull ? "pull" : "push"), CAR (wires)->width);
    if (CAR (wires)->position.Line != 0)
    {
        putc (' ', stream);
        StrSBreezePosition (stream, CAR (wires)->position);
    }
    if (CAR (wires)->ident != NoIdent)
    {
        fprintf (stream, " (name \"");
        WriteIdent (stream, CAR (wires)->ident);
        fprintf (stream, "\")");
    }
    if (CAR (wires)->type.type != NoType)
    {
        PtrType type = CAR (wires)->type.type;

        if (DoAndrewsMagicSwitch && Abs (type->size) != CAR (wires)->width)
        {
            fprintf (stderr, "%d %d %d\n", Abs (type->size), CAR (wires)->width, count);
        }

        if ((type->size != 0 && !(type->nature == NumericType && type->size >= 0)) || DoAndrewsMagicSwitch)
        {
            fprintf (stream, " (type ");
            if (!(type->scope & TOP_LEVEL_SCOPES))
                StrPtrSBreezeType (stream, type, false, true);
            else
                StrPtrSBreezeTypeName (stream, type);
            fprintf (stream, ")");
        }
    }
    if (CAR (wires)->options)
    {
        putc (' ', stream);
        StrPtrLispList (stream, CAR (wires)->options, " ", " ");
    }
    fprintf (stream, ") ; %d\n", count);

    return count;
}

/* BeginWires : initialise globals for this file */
void BeginWires (void)
{
	UnknownWireAccesses = NewAccessList (NewAccess (NoInstance), NULL);
}
