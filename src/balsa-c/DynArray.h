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

	`DynArray.h'
	Dynamic array defns. like libreuse but using libglib.
	
 */

#ifndef DYNARRAY_HEADER
#define DYNARRAY_HEADER

#include <glib.h>
#include <string.h>

/* extern void MakeArray (char **array, unsigned long *elementCount, unsigned long elementSize) */
#define MakeArray(array,elementCount,elementSize) \
	*(array) = g_malloc0 (*(elementCount) * (elementSize));

/* extern void ExtendArray (char **array, unsigned long *elementCount, unsigned long elementSize) */
#define ExtendArray(array,elementCount,elementSize) \
{ \
	void *newArray; \
	newArray = g_malloc0 (*(elementCount) * 2 * (elementSize)); \
	memcpy (newArray, *(array), *(elementCount) * (elementSize));  \
	*(elementCount) *= 2;  \
	g_free (*(array)); \
	*(array) = newArray;  \
}

/* FIX for the rex-generated file which tries to free a statically defined array */
#define ExtendArray_FIXED(array,elementCount,elementSize) \
{ \
	void *newArray; \
	newArray = g_malloc0 (*(elementCount) * 2 * (elementSize)); \
	memcpy (newArray, *(array), *(elementCount) * (elementSize));  \
	*(elementCount) *= 2;  \
	if (((void *) *(array)) != ((void *) yyInitStateStack)) \
		g_free (*(array)); \
	*(array) = newArray;  \
}

/* extern void ReleaseArray (char **array, unsigned long *elementCount, unsigned long elementSize) */
#define ReleaseArray(array,elementCount,elementSize) \
	g_free (*(array));

#endif /* DYNARRAY_HEADER */
