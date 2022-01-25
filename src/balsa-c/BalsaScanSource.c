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

	`BalsaScanSource.c'
	Modified automatically generated rex Source.c file
	Added code to track file inclusion
	
 */

#include "BalsaScanSource.h"
#include "BreezeScanSource.h"
#include "output.h"
#include "rSystem.h"
#include <glib.h>

int BalsaScan_BeginSource (Ptrchar fileName)
{
    /* This function should only be called once at the top of the program */
    int fileNo = OpenInput (fileName);
    tIdent fileNameIdent = MakeIdent1 (fileName);

    /* Add this file onto the tree */
    CurrentFile = NewIdentList (fileNameIdent, NoPosition, CurrentFile);

    if (!VisitedFiles)
        VisitedFiles = g_hash_table_new (g_direct_hash, g_direct_equal);

    g_hash_table_insert (VisitedFiles, (gpointer) fileNameIdent, (gpointer) 1);
    FileNumber = 2;

    return fileNo;
}
