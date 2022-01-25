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

	`BalsaScanSource.h'
	Modified automatically generated rex Source.c file
	Added code to track file inclusion
	
 */

#ifndef BALSA_SOURCE_HEADER
#define BALSA_SOURCE_HEADER

#include "rSystem.h"

extern int Balsa (void);

#define BalsaScan_GetLine(file, buffer, size) rRead ((file), (buffer), (size))
#define BalsaScan_CloseSource(file) rClose (file)
extern int BalsaScan_BeginSource (char *fileName);

#endif /* BALSA_SOURCE_HEADER */
