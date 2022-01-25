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

	`Sets.h'
	libreuse replacement functions.
	
 */

#ifndef SETS_HEADER
#define SETS_HEADER

#include <glib.h>
#include <misc.h>

/* Using 32b ints 'cause they're handy */
typedef struct
{
    guint32 *words;
    int wordCount;
}
tSet;

#define IsElement(element,set) \
	(((set)->words[(element) / 32] >> ((element) % 32)) & 1)

extern void MakeSet (tSet * set, unsigned int size);
extern void ReleaseSet (tSet * set);
extern void AssignEmpty (tSet * set);
extern bool IsEmpty (tSet * set);
extern void Include (tSet * set, unsigned int element);
extern void Exclude (tSet * set, unsigned int element);
extern unsigned int Extract (tSet * set);
extern void Union (tSet * set1, tSet * set2);

#endif /* SETS_HEADER */
