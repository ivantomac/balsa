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

	`lists.c'
	Generic list manipulating macros/functions
	
 */

#include "lists.h"

/* PrepareConsArray : make an array (length len) of correctly (reverse 0 <- 1 <- n-1) linked
   PtrConsList cons cells with the body element set to NULL */
PtrConsArray PrepareConsArray (unsigned len)
{
    PtrConsArray ret = NEW_ARRAY (ConsList, len);
    int i;

    /* Link the array and copy the elements over */
    for (i = len - 1; i >= 0; i--)
    {
        ret[i].body = NULL;
        ret[i].next = ret + i - 1;
    }
    ret[0].next = NULL;
    return ret;
}

/* ConsListTail : returns the last element of the given list, or NULL for an empty list */
PtrConsList ConsListTail (PtrConsList list)
{
    /* FOR_EACH (list, CDR (list), ...) */
    while (list && CDR (list))  /* Find the first element for which CDR (list) is false */
    {
        list = CDR (list);
    }
    return list;
}

/* AppendConsLists : append the second cons list to the first */
PtrConsList AppendConsLists (PtrConsList list1, PtrConsList list2)
{
    if (!list1)
    {
        return list2;
    } else
    {
        PtrConsList tail = ConsListTail (list1);

        tail->next = list2;
    }
    return list1;
}

/* NewConsList : create a new cons cell initialised with the given car and cdr */
PtrConsList NewConsList (void *car, PtrConsList cdr)
{
    PtrConsList newCell = NEW (ConsList);

    CAR (newCell) = car;
    newCell->next = cdr;
    return newCell;
}

/* RawArrayCopyConsList : copy a cons cell list into an array, given the
   correct length of the list */
PtrConsArray RawArrayCopyConsList (PtrConsList list, unsigned listLength)
{
    PtrConsArray array;
    unsigned listLengthCounter = listLength;

    /* Don't bother for an empty list */
    if (!list || listLength == 0)
        return NULL;

    array = PrepareConsArray (listLength);

    /* FOR_EACH (list, true, ...) */
    while (list && true)
    {
        array[listLengthCounter - 1].body = CAR (list);
        listLengthCounter--;
        list = CDR (list);
    }

    return array;
}

/* ArrayCopyConsList : copy a list of cons cells into an array */
PtrConsArray ArrayCopyConsList (PtrConsList list)
{
    unsigned listLength = LengthOfConsList (list);

    return RawArrayCopyConsList (list, listLength);
}

/* CopyConsList : copy a list of cons cells into a new list */
PtrConsList CopyConsList (PtrConsList list)
{
    unsigned listLength = LengthOfConsList (list);

    if (listLength)
    {
        return CONS_ARRAY_TO_LIST (RawArrayCopyConsList (list, listLength), listLength);
    }
    return NULL;
}

/* LengthOfConsList : find the length of a cons cell list */
unsigned LengthOfConsList (PtrConsList list)
{
    unsigned length = 0;

    /* FOR_EACH (list, true, ...) */
    while (list && true)
    {
        length++;
        list = CDR (list);
    }
    return length;
}

/* RawArrayReverseConsList : reverse a given list, create an array, copy elements in reverse
   order and return head */
PtrConsArray RawArrayReverseConsList (PtrConsList list, unsigned listLength)
{
    PtrConsArray array;
    unsigned i = 0;

    /* Don't bother for an empty list */
    if (!list || listLength == 0)
        return NULL;

    array = PrepareConsArray (listLength);

    /* FOR_EACH (list, true, ...) */
    while (list && true)
    {
        array[i].body = CAR (list);
        i++;
        list = CDR (list);
    }
    return array;
}

/* ArrayReverseConsList : reverse a given list, create an array, copy elements in reverse
   order and return head */
PtrConsArray ArrayReverseConsList (PtrConsList list)
{
    unsigned listLength = LengthOfConsList (list);

    return RawArrayReverseConsList (list, listLength);
}

/* ReverseConsList : reverse a given list, create an array, copy elements in reverse
   order and return head */
PtrConsArray ReverseConsList (PtrConsList list)
{
    unsigned listLength = LengthOfConsList (list);

    if (listLength)
    {
        return CONS_ARRAY_TO_LIST (RawArrayReverseConsList (list, listLength), listLength);
    }
    return NULL;
}

/* NthCellOfConsList : return the nth cell of a given list, if the
   index is negative then work from the back of the list */
PtrConsList NthCellOfConsList (PtrConsList list, int elem)
{
    unsigned elemNo = (elem < 0 ? LengthOfConsList (list) + elem : elem);

    ASSERT (elemNo >= 0);
    /* FOR_EACH (list, elemNo, ...) */
    while (list && elemNo)
    {
        elemNo--;
        list = CDR (list);
    }
    return list;
}

/* NthOfConsList : return the nth cells car element, indexing as NthCellOfConsList */
void *NthOfConsList (PtrConsList list, int elem)
{
    PtrConsList cell = NthCellOfConsList (list, elem);

    return (cell ? CAR (cell) : NULL);
}

/* RawArrayDeepCopyConsList : copy a cons cell list by copying each element, need to know
   the function required in the copy and the length of the list */
PtrConsArray RawArrayDeepCopyConsList (PtrConsList list, CopyConsCellFunctionType copyFunction, unsigned listLength)
{
    PtrConsArray newArray;
    unsigned i;

    /* Don't bother for an empty list */
    if (!list || listLength == 0)
        return NULL;

    newArray = RawArrayCopyConsList (list, listLength);

    for (i = 0; i < listLength; i++)
    {
        newArray[i].body = copyFunction (newArray[i].body);
    }
    return newArray;
}

/* ArrayDeepCopyConsList : copy a cons list into an array using the copyFunction to copy each element */
PtrConsArray ArrayDeepCopyConsList (PtrConsList list, CopyConsCellFunctionType copyFunction)
{
    unsigned listLength = LengthOfConsList (list);

    return RawArrayDeepCopyConsList (list, copyFunction, listLength);
}

/* DeepCopyConsList : copy a cons list into a new list using the copyFunction to copy each element */
PtrConsList DeepCopyConsList (PtrConsList list, CopyConsCellFunctionType copyFunction)
{
    unsigned listLength = LengthOfConsList (list);

    if (listLength)
    {
        return CONS_ARRAY_TO_LIST (RawArrayDeepCopyConsList (list, copyFunction, listLength), listLength);
    }
    return NULL;
}
