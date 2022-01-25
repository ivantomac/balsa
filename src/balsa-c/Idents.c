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

	`Idents.c'
	Replacement for libreuse Idents but using glib GQuarks instead
	
 */

#include "Idents.h"
#include "misc.h"

tIdent NoIdent = 0;

extern tIdent MakeIdent1 (char *string)
{
    return (!string || *string == '\0' ? NoIdent : g_quark_from_string (string));
}

/* PeekString : returns the string for a given ident,
	you shouldn't modify this string */
extern const char *PeekString (tIdent ident)
{
    const char *ret = g_quark_to_string (ident);

    return (ret ? ret : "");
}

extern void WriteIdent (FILE * stream, tIdent ident)
{
    if (ident != NoIdent)
        fprintf (stream, "%s", g_quark_to_string (ident));
}
