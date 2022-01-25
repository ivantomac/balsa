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

	`bfile.c'
	Balsa File type

*/

#include "balsasim/bfile.h"
#include <stdlib.h>

/* DeleteBalsaFile : destructor */
void DeleteBalsaFile (BalsaFile * file)
{
    if (!file)
        return;

    if (file->filename)
        free (file->filename);
    if (file->file)
        fclose (file->file);

    free (file);
}

/* BalsaFileReadable : returns true if the given file struct. is open for reads, the function moans if it isn't */
bool BalsaFileReadable (BalsaFile * file)
{
    bool ret = false;

    if (!file || !file->file)
        fprintf (stderr, "Can't read from closed file\n");
    else if (file->mode != BalsaFileMode_read)
        fprintf (stderr, "File `%s' can't be read\n", file->filename);
    else
        ret = true;

    return ret;
}

/* BalsaFileWritable : " written " */
bool BalsaFileWritable (BalsaFile * file)
{
    bool ret = false;

    if (!file || !file->file)
        fprintf (stderr, "Can't write to closed file\n");
    else if (file->mode != BalsaFileMode_write)
        fprintf (stderr, "File `%s' can't be written\n", file->filename);
    else
        ret = true;

    return ret;
}
