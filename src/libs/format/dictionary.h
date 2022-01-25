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

	`dictionary.h'
	Simple name (string) to pointer association collection

*/

#ifndef FORMAT_DICTIONARY_HEADER
#define FORMAT_DICTIONARY_HEADER

#include <stdbool.h>

typedef struct
{
    const char *name;
    void *data;
}
FormatDictionaryElement;

typedef struct FormatDictionary_
{
    bool isSorted;              /* must be sorted to do lookup */
    unsigned allocatedSize;     /* size of contents array */
    unsigned elementCount;      /* number of used elements */
    FormatDictionaryElement *contents;
}
FormatDictionary;

/* NewFormatDictionary : make a new, empty FormatDictionary */
extern FormatDictionary *NewFormatDictionary (void);

/* FormatDictionaryInsert : insert a dictionary element */
extern void FormatDictionaryInsert (FormatDictionary * dictionary, const char *name, void *data);

/* FormatDictionaryLookup : lookup an element by name */
extern void *FormatDictionaryLookup (FormatDictionary * dictionary, const char *name);

#endif
