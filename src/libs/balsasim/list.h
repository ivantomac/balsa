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

#ifndef BALSA_SIM_LIST_H
#define BALSA_SIM_LIST_H

typedef struct BalsaList_
{
    void *data;
    struct BalsaList_ *next;
}
BalsaList;

/* NewBalsaList : prepend a cell to a given list */
extern BalsaList *NewBalsaList (void *data, BalsaList * list);

/* BalsaListLength : find the length of the given list */
extern unsigned BalsaListLength (BalsaList * list);

/* BalsaListReverse : reverse the order of elements in a list */
extern BalsaList *BalsaListReverse (BalsaList * list);

/* BalsaListToArray : make an array from the data elements in a given list, returns NULL for a NULL list,
	returns the array length in *length */
extern void **BalsaListToArray (BalsaList * list, unsigned *length);

/* BalsaListDelete : deletes the cons cells of the given list, does not delete the ->data elements */
extern void BalsaListDelete (BalsaList * list);

#endif
