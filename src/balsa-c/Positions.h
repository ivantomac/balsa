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

	`Positions.h'
	Derived from KCT reuse/Positions.h
	Added support for file path in positions

 */

#ifndef POSITIONS_HEADER
#define POSITIONS_HEADER

#include <stdio.h>

/* Forward declaration of an IdentList */
struct IdentList;

/* Position within the given file */
typedef struct tPosition
{
    unsigned short Line, Column;
    struct IdentList *File;
    void *CallContext;          // = struct CallContext *
}
tPosition;

/* Dummy Position - 0,0 in no file */
extern tPosition NoPosition;

/* Compare : -1 iff pos1 < pos2, 0 iff pos1 = pos2, 1 iff pos1 > pos2 */
extern int Compare (tPosition pos1, tPosition pos2);

/* WritePosition : write a position onto a stream */
extern void WritePosition (FILE * stream, tPosition pos);

/* WriteErrorPosition : write a position onto a stream in a GCC like format,
   if the file list has the same value as the previous invocation don't reprint. */
extern void WriteErrorPosition (FILE * stream, tPosition pos);

/* StrSBreezePosition : write a position to stream in the Breeze format */
extern void StrSBreezePosition (FILE * stream, tPosition pos);

#endif /* POSITIONS_HEADER */
