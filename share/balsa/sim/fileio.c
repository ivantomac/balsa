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

	`fileio.c'
	File I/O builtin functions

*/

#include "balsasim/bfile.h"
#include "balsasim/builtin.h"
#include "balsasim/object.h"
#include "balsasim/bstring.h"
#include <stdlib.h>
#include <string.h>

/* FileOpen (fileName : String; mode : FileMode) : File */
static void Fileio_FileOpen (BuiltinFunction * function, BuiltinFunctionInstanceData * instance)
{
    BalsaObject *filenameObject = FormatDataGetBalsaObject (instance->arguments[0], 0);
    BalsaString *filenameStr = BALSA_STRING (filenameObject->data);
    BalsaFileMode mode = instance->arguments[1]->words[0];
    char *filename = malloc (filenameStr->length + 1);

    strncpy (filename, filenameStr->string, filenameStr->length);
    filename[filenameStr->length] = '\0';

    FILE *file;

    switch (mode)
    {
    case BalsaFileMode_write:
    case BalsaFileMode_writeUnbuffered:
    case BalsaFileMode_writeLineBuffered:
        file = fopen (filename, "w");
        break;
    default:                   /* Default to reaon `read' or an invalid value */
        file = fopen (filename, "r");
        mode = BalsaFileMode_read;
        break;
    }

    /* Handle write mode buffering */
    switch (mode)
    {
    case BalsaFileMode_writeUnbuffered:
        if (file)
            setvbuf (file, NULL, _IONBF, BUFSIZ);
        mode = BalsaFileMode_write;
        break;
    case BalsaFileMode_writeLineBuffered:
        if (file)
            setvbuf (file, NULL, _IOLBF, BUFSIZ);
        mode = BalsaFileMode_write;
        break;
    default:
        break;
    }

    if (!file)
    {
        BALSA_SIM_PRINTF ("Can't open file `%s' to %s\n", filename, (mode == BalsaFileMode_write ? "write" : "read"));
        free (filename);
        exit (EXIT_FAILURE);
    }

    BalsaFile *balsaFile = malloc (sizeof (BalsaFile));

    balsaFile->filename = filename;
    balsaFile->file = file;
    balsaFile->mode = mode;

    SetBalsaObject (instance->objects[0], balsaFile, (BalsaDestructor) DeleteBalsaFile);
    FormatDataSetBalsaObject (instance->result, instance->objects[0], 0);
}

/* StringStripTrailingNL : strip trailing NL and CR characters from the given
	string by moving the '\0' end of string marker */
static void StringStripTrailingNL (char *str)
{
    char *strPtr = str + strlen (str) - 1;

    while (strPtr >= str)
    {
        if (*strPtr == '\n' || *strPtr == '\r')
            *strPtr = '\0';
        else
            break;
        strPtr--;
    }
}

/* FileReadLine (file : File) : String */
static void Fileio_FileReadLine (BuiltinFunction * function, BuiltinFunctionInstanceData * instance)
{
    char line[1024];            /* FIXME, don't use fixed size buffer */
    BalsaObject *fileObject = FormatDataGetBalsaObject (instance->arguments[0], 0);
    BalsaFile *file = BALSA_FILE (fileObject->data);

    *line = '\0';

    if (BalsaFileReadable (file))
    {
        fgets (line, 1024, file->file);
        StringStripTrailingNL (line);
    }

    SetBalsaObject (instance->objects[0], NewBalsaString (line, -1), (BalsaDestructor) BalsaStringUnref);
    FormatDataSetBalsaObject (instance->result, instance->objects[0], 0);
}

/* FileReadLine_withRetry (file : File) : String */
static void Fileio_FileReadLine_withRetry (BuiltinFunction * function, BuiltinFunctionInstanceData * instance)
{
    char line[1024];            /* FIXME, don't use fixed size buffer */
    BalsaObject *fileObject = FormatDataGetBalsaObject (instance->arguments[0], 0);
    BalsaFile *file = BALSA_FILE (fileObject->data);

    *line = '\0';

    if (BalsaFileReadable (file))
    {
        char *ret;

        clearerr (file->file);
        ret = fgets (line, 1024, file->file);
        if (!ret)
        {
            instance->retryRequested = 1;
            return;
        }
        StringStripTrailingNL (line);
    }

    SetBalsaObject (instance->objects[0], NewBalsaString (line, -1), (BalsaDestructor) BalsaStringUnref);
    FormatDataSetBalsaObject (instance->result, instance->objects[0], 0);
}

/* FileWrite (file : File; string : String) : File */
static void Fileio_FileWrite (BuiltinFunction * function, BuiltinFunctionInstanceData * instance)
{
    BalsaObject *fileObject = FormatDataGetBalsaObject (instance->arguments[0], 0);
    BalsaFile *file = BALSA_FILE (fileObject->data);
    BalsaString *string = BALSA_STRING (FormatDataGetBalsaObject (instance->arguments[1], 0)->data);

    if (BalsaFileWritable (file))
    {
        fwrite (string->string, 1, string->length, file->file);
        fflush (file->file);
    }

    FormatDataSetBalsaObject (instance->result, fileObject, 0);
}

/* FileEOF (file : File) : String */
static void Fileio_FileEOF (BuiltinFunction * function, BuiltinFunctionInstanceData * instance)
{
    BalsaObject *fileObject = FormatDataGetBalsaObject (instance->arguments[0], 0);
    BalsaFile *file = BALSA_FILE (fileObject->data);
    int charFromStream = getc (file->file);
    int isEOF = charFromStream == EOF;

    if (!isEOF)
        ungetc (charFromStream, file->file);

    instance->result->words[0] = isEOF;
}

/* FileClose (file : File) : File */
static void Fileio_FileClose (BuiltinFunction * function, BuiltinFunctionInstanceData * instance)
{
    BalsaObject *fileObject = FormatDataGetBalsaObject (instance->arguments[0], 0);
    BalsaFile *file = BALSA_FILE (fileObject->data);

    if (!file || !file->file)
        BALSA_SIM_PRINTF ("Can't close an already closed file\n");
    else
    {
        fclose (file->file);
        file->file = NULL;
    }

    FormatDataSetBalsaObject (instance->result, fileObject, 0);
}

BALSA_SIM_REGISTER_BUILTIN_LIB (fileio)
{
    BalsaSim_RegisterBuiltinFunction ("FileOpen", 0, 2, Fileio_FileOpen, 64, (unsigned[])
      {
      64, 3}
      , 1);
    BalsaSim_RegisterBuiltinFunction ("FileReadLine", 0, 1, Fileio_FileReadLine, 64, (unsigned[])
      {
      64}
      , 1);
    BalsaSim_RegisterBuiltinFunction ("FileReadLine_withRetry", 0, 1, Fileio_FileReadLine_withRetry, 64, (unsigned[])
      {
      64}
      , 1);
    BalsaSim_RegisterBuiltinFunction ("FileEOF", 0, 1, Fileio_FileEOF, 1, (unsigned[])
      {
      64}
      , 0);
    BalsaSim_RegisterBuiltinFunction ("FileClose", 0, 1, Fileio_FileClose, 64, (unsigned[])
      {
      64}
      , 0);
    BalsaSim_RegisterBuiltinFunction ("FileWrite", 0, 2, Fileio_FileWrite, 64, (unsigned[])
      {
      64, 64}
      , 0);
}
