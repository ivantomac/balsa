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

	`bfile.h'
	Balsa File type

*/

#ifndef BALSA_SIM_BFILE_H
#define BALSA_SIM_BFILE_H

#include <stdio.h>
#include <stdbool.h>

typedef enum
{
    BalsaFileMode_read = 0,
    BalsaFileMode_write = 1,
    BalsaFileMode_writeUnbuffered = 2,
    BalsaFileMode_writeLineBuffered = 3
      /* Note that the modes writeUnbuffered and writeLineBuffered
         can be used with FileOpen to open a file for writing without buffering, but the ->mode of
         the file opened this way will show BalsaFileMode_write */
}
BalsaFileMode;

typedef struct
{
    FILE *file;
    char *filename;             /* For debugging! */
    BalsaFileMode mode;
}
BalsaFile;

#define BALSA_FILE(object) ((BalsaFile *)(object))

/* DeleteBalsaFile : destructor */
extern void DeleteBalsaFile (BalsaFile * file);

/* BalsaFileReadable : returns true if the given file struct. is open for reads, the function moans if it isn't */
extern bool BalsaFileReadable (BalsaFile * file);

/* BalsaFileWritable : " written " */
extern bool BalsaFileWritable (BalsaFile * file);

#endif
