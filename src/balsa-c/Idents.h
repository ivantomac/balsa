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

	`Idents.h'
	Replacement for libreuse Idents but using glib GQuarks instead
	
 */

#ifndef IDENTS_HEADER
#define IDENTS_HEADER

#include <stdio.h>
#include <glib.h>

typedef GQuark tIdent;
extern tIdent NoIdent;

extern tIdent MakeIdent1 (char *string);

/* PeekString : returns the string for a given ident,
	you shouldn't modify this string */
extern const char *PeekString (tIdent ident);

extern void WriteIdent (FILE * stream, tIdent ident);

#endif /* IDENTS_HEADER */
