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

	`BreezeScanSource.h'
	Modified automatically generated rex Source.c file
	Added code to track file inclusion
	
 */

#ifndef BREEZE_SOURCE_HEADER
#define BREEZE_SOURCE_HEADER

#include "Tree.h"
#include "contexts.h"
#include "misc.h"

extern int Breeze (void);

/* Breeze_TreeRoot : root of parse tree after calling BreezeScan_BeginFile */
extern tTree Breeze_TreeRoot;

extern int BreezeScan_BeginSource (char *fileName);
extern int BreezeScan_GetLine (int file, char *buffer, int size);
extern void BreezeScan_CloseSource (int file);

/* MakePathFromDottedPath : Alloc a string to hold the slash separated path given in a
   dotted_path, NB. this is in reverse order in the dotted_path */
extern Ptrchar MakePathFromDottedPath (tTree ident, char pathSeparator);

/* HandleImport : handle the importing of breeze files in balsa, this is called by the 
   parser for the 'import' non terminal */
extern PtrContext HandleImport (Ptrchar dotpath, tPosition position, PtrContext contextIn, Scope scope);

#endif /* BREEZE_SOURCE_HEADER */
