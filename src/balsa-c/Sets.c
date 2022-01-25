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

	`Sets.c'
	libreuse replacement functions.
	
 */

#include "Sets.h"

typedef guint32 *Ptrguint32;

extern void MakeSet (tSet * set, unsigned int size)
{
    set->wordCount = g_bit_storage (size);
    set->words = NEW_ARRAY (guint32, set->wordCount);

    AssignEmpty (set);
}

extern void ReleaseSet (tSet * set)
{
    FREE_ARRAY (guint32, set->wordCount, set->words);
}

extern void AssignEmpty (tSet * set)
{
    int i;

    for (i = set->wordCount - 1; i >= 0; i--)
        set->words[i] = 0;
}

extern bool IsEmpty (tSet * set)
{
    int i;
    bool empty = true;

    for (i = 0; i < set->wordCount; i++)
    {
        if (set->words[i] != 0)
        {
            empty = false;
            break;
        }
    }

    return empty;
}

extern void Include (tSet * set, unsigned int element)
{
    if (element < set->wordCount * 32)
        set->words[element / 32] |= (1 << (element % 32));
}

extern void Exclude (tSet * set, unsigned int element)
{
    if (element < set->wordCount * 32)
        set->words[element / 32] &= ~(1 << (element % 32));
}

extern unsigned int Extract (tSet * set)
{
    int i;
    int bitNo = 0;

    for (i = 0; i < set->wordCount; i++)
    {
        if (set->words[i] != 0)
        {
            int bitPos = 0;
            int bitMask = 1;

            while (set->words[i] == (set->words[i] & ~bitMask))
            {
                bitPos++;
                bitMask <<= 1;
            }
            set->words[i] &= ~bitMask;
            bitNo += bitPos;
            break;
        }
        bitNo += 32;
    }
    return bitNo;
}

extern void Union (tSet * set1, tSet * set2)
{
    /* assume that set1->wordCount >= set2->wordCount */
    int i;

    for (i = 0; i < set2->wordCount; i++)
        set1->words[i] |= set2->words[i];
}
