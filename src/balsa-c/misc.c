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

	`misc.c'
	Miscellaneous definitions
	
 */

#include "misc.h"
#include <ctype.h>
#include <string.h>

/* Definitions */
tIdent MarkerIdent = 0;

/* BeginMisc : initialise misc module */
void BeginMisc (void)
{
    MarkerIdent = MakeIdent1 ("<MARK>");
}

/* Abs : Absolute value of argument - saves including the maths lib */
unsigned Abs (int x)
{
    return (x > 0 ? x : -x);
}

/* Max : maximum of two ints */
extern int Max (int a, int b)
{
    return (a > b ? a : b);
}

/* IsInteger : returns true if string begins with -<digit> or <digit> */
bool IsInteger (char *string)
{
    return isdigit (*string) || (*string == '-' && isdigit (*string));
}

/* StrBundleNumber : print a BundleNumber */
void StrBundleNumber (FILE * stream, BundleNumber bundle)
{
    if (bundle == NoBundle)
        fprintf (stream, "NoBundle");
    else
        fprintf (stream, "#%d", bundle);
}

/* EndOfString : return a pointer to the NULL termination on string str */
char *EndOfString (char *str)
{
    while (*str)
        str++;
    return str;
}

/* StrDup : like strdup but using NEW_ARRAY */
char *StrDup (char *str)
{
    char *ret = NEW_ARRAY (char, strlen (str) + 1);

    strcpy (ret, str);
    return ret;
}

char *StrNDup (char *str, size_t n)
{
    char *ret = NEW_ARRAY (char, n + 1);

    strncpy (ret, str, n);
    ret[n] = '\0';

    return ret;
}

/* StrSignedBits : show a SignedBit's as though it were a numeric type declaration
   ie. <num> [signed] bits */
extern void StrSignedBits (FILE * stream, SignedBits bits)
{
    fprintf (stream, "%d %sbits", Abs (bits), (bits < 0 ? "signed " : ""));
}

/* SubstituteCharInString : swap one char for another in the given string */
void SubstituteCharInString (char *str, char fromChar, char toChar)
{
    do
    {
        str = strchr (str, fromChar);

        if (str)
        {
            *str = toChar;
            str++;
        }
    }
    while (str);
}

/* RemoveCharFromString : remove occurences of the character chr from the string str */
char *RemoveCharFromString (char *str, char chr)
{
    char *toPtr = str, *fromPtr = str;

    while (*fromPtr)
    {
        if (*fromPtr != chr)
        {
            *toPtr = *fromPtr;
            toPtr++;
        }
        fromPtr++;
    }
    *toPtr = '\0';
    return str;
}
