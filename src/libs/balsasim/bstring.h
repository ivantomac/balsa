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

	`bstring.h'
	BalsaString handling

*/

#ifndef BALSA_SIM_BSTRING_H
#define BALSA_SIM_BSTRING_H

#include <stdio.h>

/* BalsaString : Balsa reference counted string object */
typedef struct
{
    char *string;               /* Logical start of the string (always >= ->allocatedString */
    char *allocatedString;      /* Pointer to *start* of allocated space */
    unsigned length;            /* Length of string from ->string, use ->string - ->allocatedString
                                   to find allocated length */
    int *refCount;              /* Reference count on the allocatedString.  String's are immutable objects
                                   and so they can share sub-strings, this pointer should be copied when a new string is
                                   made which uses the same ->string as this one. */
}
BalsaString;

#define BALSA_STRING(ptr) ((BalsaString *)(ptr))

/* NewBalsaString : make a new string from the given string, if length < 0 then
	assume the given string is NUL terminated and copy all of it, otherwise
	copy the first `length' characters and allow NULs.  If string is NULL then
	don't copy the string or initialise the BalsaString's elements */
extern BalsaString *NewBalsaString (char *string, int length);

/* NewBalsaSubString : make a new BalsaString which is a sub-string of an existing BalsaString.
	This means they share an allocatedString and the refCount of the source string will be
	increased to reflect this.  `start' should be a pointer within string->allocated string
	which is to be the logical start of the new sub-string.  `length' specifies the length
	of the target sub-string or, if -1, implies that the sub-string should contain the
	remainder of the source string from `start' onwards */
extern BalsaString *NewBalsaSubString (BalsaString * string, char *start, int length);

/* BalsaStringRef : increase the reference count of the char * in the given string.  Useful
	when making a new BalsaString which is a sub-string of this one */
extern void BalsaStringRef (BalsaString * string);

/* BalsaStringUnref : deallocate a string object if it's refCount (for the char *) becomes 0 */
extern void BalsaStringUnref (BalsaString * string);

#endif
