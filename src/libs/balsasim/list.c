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

	`list.c'
	Simple linked list handler

*/

#include "list.h"
#include <stdlib.h>

/* NewBalsaList : prepend a cell to a given list */
BalsaList *NewBalsaList (void *data, BalsaList * list)
{
    BalsaList *ret = malloc (sizeof (BalsaList));

    ret->data = data;
    ret->next = list;

    return ret;
}

/* BalsaListLength : find the length of the given list */
unsigned BalsaListLength (BalsaList * list)
{
    unsigned ret = 0;

    while (list)
    {
        ret++;
        list = list->next;
    }

    return ret;
}

/* BalsaListReverse : reverse the order of elements in a list */
BalsaList *BalsaListReverse (BalsaList * list)
{
    BalsaList *previousCell = NULL;

    while (list)
    {
        BalsaList *tmp = list->next;

        list->next = previousCell;
        previousCell = list;

        list = tmp;
    }

    return previousCell;
}

/* BalsaListToArray : make an array from the data elements in a given list, returns NULL for a NULL list,
	returns the array length in *length */
void **BalsaListToArray (BalsaList * list, unsigned *length)
{
    void **ret = NULL;

    if (list)
    {
        int i = 0;
        unsigned listLength = BalsaListLength (list);
        ret = malloc (sizeof (void *) * listLength);

        while (list)
        {
            ret[i] = list->data;

            list = list->next;
            i++;
        }

        if (length)
            *length = listLength;
    }

    return ret;
}

/* BalsaListDelete : deletes the cons cells of the given list, does not delete the ->data elements */
void BalsaListDelete (BalsaList * list)
{
    while (list)
    {
        BalsaList *next = list->next;

        free (list);

        list = next;
    }
}
