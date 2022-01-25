/*
	The Balsa Formatted Printing Library, as used in:
	  The Balsa Asynchronous Hardware Synthesis System
	  The GTKWave electronic waveform viewer
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

	`external.h'
	External (string) representation of formats.
	Note that this is all due to be replaced with some better, non printf-like
	system

*/

#ifndef FORMAT_EXTERNAL_HEADER
#define FORMAT_EXTERNAL_HEADER

#include <stdio.h>
#include "format/format.h"

/* FormatPrintEscapedString : print the given string with the escaping rules:
	'\', all non printable characters (!isprint(chr)) and the characters in
	the escapableChars string are printed as \xXX hex escape codes (with *exactly* 2 hex digits),
	`length' specifies the true length of the source string (if non NUL terminated, or contains
	NUL) or -1 indicating that strlen (string) is the true string length */
extern void FormatPrintEscapedString (FILE * stream, const char *string, int length, const char *escapableChars);

/* FormatPrintEscapedStringToString : Same as FormatPrintEscapedString but writes the output
   to the buffer 'destString'.
   Returns the number of chars written.
   You need to be careful to allocate a large enough buffer when calling this function! */
int FormatPrintEscapedStringToString (char *destString, const char *string, int length, const char *escapableChars);

/* FormatDeescapeString : modify the given string to reverse the escaping effects of FormatPrintEscapedString.
	Returns the number of characters in the final string (not including the NUL termination) */
extern int FormatDeescapeString (char *string);

/* FormatExternalToInternal : make an internal representation format structure
	from an external format string */
extern Format FormatExternalToInternal (char *format);

/* FormatListExternalToInternal : make an internal representation format structure
	from an external format string in the new lispy format, use `getString' and `putString'
	functions for strings and `quoteChar' whereever a quote character is required. */
extern Format FormatListExternalToInternal (const char *string, FormatGetFunction getString, FormatPutFunction putString, char quoteChar);

/* FormatPrintFormat : print a format using the lispy external representation for the format */
extern void FormatPrintFormat (FILE * stream, Format format);

#endif
