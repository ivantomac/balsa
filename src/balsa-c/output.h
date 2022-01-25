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

	`output.h'
	Definitions for output file generation
	
 */

#ifndef OUTPUT_HEADER
#define OUTPUT_HEADER

#include "Positions.h"
#include "misclists.h"
#include <stdio.h>
#include <sys/types.h>
#include <glib.h>

/* FileNumber : number all the files so we can use this as a short ID */
extern unsigned FileNumber;

extern PtrIdentList CurrentFile;
extern GHashTable *VisitedFiles;

/* Breeze output file stream */
extern FILE *BreezeOutputFile;

/* Full path/file name of current output file */
extern char *BreezeOutputFileName;

/* SplitFilename : split the filename `filename' into path, name and extension
   components at the last slash/last dot boundaries.  Assigns empty strings
   (freshly Alloced) where path or extension should be empty */
extern void SplitFilename (char *filename, char **path, char **name, char **extension);

/* WriteOutputFilePreamble : write out the prologue of an output intermediate file */
extern void WriteOutputFilePreamble (FILE * stream, char *fileType, char *commentString);

/* CreateOutputFile : create a file blockName'.'formatExtension in the
   IntermediatePath directory, if you can't do this raise a fatal error,
   outputFileName will be set to the name of the generated file */
extern FILE *CreateOutputFile (char *blockName, char *formatExtension, char **outputFileName);

/* ExpandPathToFile : search import file path to find named file 'path'.`extension' (where path can
   include directories. */
extern Ptrchar ExpandPathToFile (char *path, char *extension);

#endif /* OUTPUT_HEADER */
