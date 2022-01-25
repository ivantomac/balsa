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

	`bstring.c'
	BalsaString handling

*/

#include "./bstring.h"
#include "./object.h"
#include <stdlib.h>
#include <string.h>

/* NewBalsaString : make a new string from the given string, if length < 0 then
	assume the given string is NUL terminated and copy all of it, otherwise
	copy the first `length' characters and allow NULs.  If string is NULL then
	don't copy the string or initialise the BalsaString's elements */
BalsaString *NewBalsaString (char *string, int length)
{
    BalsaString *ret = malloc (sizeof (BalsaString));

    if (string != NULL)
    {
        int sourceLength = (length < 0 ? strlen (string) : length);

        ret->allocatedString = ret->string = malloc (sourceLength + 1);
        memcpy (ret->string, string, sourceLength);
        ret->string[sourceLength] = '\0'; /* NUL termination, for safety */

        ret->length = sourceLength;
    }
    ret->refCount = malloc (sizeof (int));
    (*ret->refCount) = 1;

    if (BalsaDebug)
    {
		fprintf (stderr, "Making string: %p `'\n", ret);
    }

    return ret;
}

/* NewBalsaSubString : make a new BalsaString which is a sub-string of an existing BalsaString.
	This means they share an allocatedString and the refCount of the source string will be
	increased to reflect this.  `start' should be a pointer within string->allocated string
	which is to be the logical start of the new sub-string.  `length' specifies the length
	of the target sub-string or, if -1, implies that the sub-string should contain the
	remainder of the source string from `start' onwards */
BalsaString *NewBalsaSubString (BalsaString * string, char *start, int length)
{
    BalsaString *ret = malloc (sizeof (BalsaString));

    BalsaStringRef (string);
    *ret = *string;

    if (length >= 0)
        ret->length = length;
    else
        ret->length = string->length - ((int) (start - string->string));

    ret->string = start;

    return ret;
}

/* BalsaStringRef : increase the reference count of the char * in the given string.  Useful
	when making a new BalsaString which is a sub-string of this one */
void BalsaStringRef (BalsaString * string)
{
    if (string)
        (*string->refCount)++;
}

/* BalsaStringUnref : deallocate a string object if it's refCount (for the char *) becomes 0 */
void BalsaStringUnref (BalsaString * string)
{
    (*string->refCount)--;

    if (string->allocatedString && (*string->refCount) <= 0)
    {
        free (string->allocatedString);
        free (string->refCount);
    }
    free (string);
}
