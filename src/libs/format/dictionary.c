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

	`dictionary.c'
	Simple name (string) to pointer association collection

*/

#include "dictionary.h"
#include <stdlib.h>
#include <string.h>

/* NewFormatDictionary : make a new, empty FormatDictionary */
FormatDictionary *NewFormatDictionary (void)
{
    FormatDictionary *ret = malloc (sizeof (FormatDictionary));

    ret->allocatedSize = 128;   /* Start with a reasonable size */
    ret->elementCount = 0;
    ret->isSorted = true;
    ret->contents = malloc (sizeof (FormatDictionaryElement) * ret->allocatedSize);

    return ret;
}

/* FormatDictionaryInsert : insert a dictionary element */
void FormatDictionaryInsert (FormatDictionary * dictionary, const char *name, void *data)
{
    FormatDictionaryElement *element;

    if (dictionary->allocatedSize == dictionary->elementCount)
    {
        dictionary->allocatedSize *= 2;
        dictionary->contents = realloc (dictionary->contents, sizeof (FormatDictionaryElement) * dictionary->allocatedSize);
    }

    element = dictionary->contents + dictionary->elementCount;
    dictionary->elementCount++;

    element->name = strdup (name);
    element->data = data;
    /* A dictionary is sorted if it only has one element, or it was sorted before this insertion
       and this insertion has a name >= the last name in the contents */
    dictionary->isSorted = (dictionary->elementCount == 1 ?
      true : dictionary->isSorted && strcmp (name, dictionary->contents[dictionary->elementCount - 2].name) >= 0);
}

static int DictionaryOrder (FormatDictionaryElement * e1, FormatDictionaryElement * e2)
{
    return strcmp (e1->name, e2->name);
}

/* FormatDictionaryLookup : lookup an element by name */
void *FormatDictionaryLookup (FormatDictionary * dictionary, const char *name)
{
    FormatDictionaryElement key;
    FormatDictionaryElement *foundElement = NULL;

    if (!dictionary)
        return NULL;

    if (!dictionary->isSorted)
    {
        qsort (dictionary->contents, dictionary->elementCount,
          sizeof (FormatDictionaryElement), (int (*)(const void *, const void *)) DictionaryOrder);
    }

    key.name = name;
    foundElement =
      bsearch (&key, dictionary->contents, dictionary->elementCount,
      sizeof (FormatDictionaryElement), (int (*)(const void *, const void *)) DictionaryOrder);

    return (foundElement ? foundElement->data : NULL);
}
