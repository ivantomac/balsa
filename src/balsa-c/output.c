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

	`output.c'
	Definitions for output file generation
	
 */

#include "output.h"
#include "misc.h"
#include "flags.h"
#include "Semantics.h"
#include "banner.h"
#include <Errors.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <pwd.h>
#include <sys/utsname.h>
#include <limits.h>
#include <stdlib.h>
#include <errno.h>

/* FileNumber : number all the files so we can use this as a short ID */
unsigned FileNumber = 1;

/* Traces a list back to the top level file */
PtrIdentList CurrentFile = NULL;
GHashTable *VisitedFiles = NULL;

/* Breeze output file stream */
FILE *BreezeOutputFile = NULL;

/* Full path/file name of current output file */
char *BreezeOutputFileName = NULL;

/* ExpandPathToFile : search import file path to find named file 'path'.`extension' (where path can
   include directories. */
Ptrchar ExpandPathToFile (Ptrchar path, Ptrchar extension)
{
    size_t pathCopyLength = strlen (ImportFileSearchPath) + 1;
    Ptrchar pathCopy = NEW_ARRAY (char, pathCopyLength);
    Ptrchar tryFile = NULL;
    Ptrchar part;
    Ptrchar ret = NULL;

    strcpy (pathCopy, ImportFileSearchPath);

    /* Iterate across path testing filenames */
    part = strtok (pathCopy, ":");
    while (part)
    {
        size_t tryFileLen = strlen (part) + strlen (path) + strlen (extension) + 3;

        if (*part)              /* Check if this is an empty entry */
        {
            tryFile = NEW_ARRAY (char, tryFileLen);

            sprintf (tryFile, "%s/%s.%s", part, path, extension);

            if (access (tryFile, R_OK) == 0)
                break;
            else
                FREE_ARRAY (char, tryFileLen, tryFile);
        }
        part = strtok (NULL, ":");
    }
    if (part)                   /* Found a match */
    {
        Ptrchar path;

        if (g_path_is_absolute (part))
        {
            path = g_strdup (tryFile);
        } else
        {
            Ptrchar cwd = g_get_current_dir ();

            path = g_strdup_printf ("%s/%s", cwd, tryFile);
            g_free (cwd);
        }

        /* Now canonicalise the path */
        {
            int path_max = pathconf (tryFile, _PC_PATH_MAX);

            if (path_max < 0)
                path_max = 4096;
            Ptrchar canonPath = g_new (char, path_max);

            errno = 0;
            realpath (path, canonPath);
            if (errno != 0)
            {
                fprintf (stderr, "can't canonicalise path to file `%s'\n", tryFile);
                return NULL;
            }
            ret = StrDup (canonPath);

            g_free (path);
            g_free (canonPath);
        }
    }

    Free (pathCopyLength, pathCopy);
    return ret;
}

/* SplitFilename : split the filename `filename' into path, name and extension
   components at the last slash/last dot boundaries.  Assigns empty strings
   (freshly Alloced) where path or extension should be empty */
void SplitFilename (Ptrchar filename, Ptrchar * path, Ptrchar * name, Ptrchar * extension)
{
    Ptrchar lastSlash = strrchr (filename, '/');
    Ptrchar lastDot;

    if (lastSlash)
    {
        *path = filename;
        *lastSlash = '\0';
        *name = lastSlash + 1;
        lastDot = strrchr (*name, '.');
    } else
    {
        *path = StrDup ("");
        *name = filename;
        lastDot = strrchr (*name, '.');
    }

    if (lastDot)
    {
        *extension = lastDot + 1;
        *lastDot = '\0';
    } else
    {
        *extension = StrDup ("");
    }
}

/* CreateOutputFile : create a file blockName'.'formatExtension in the
   IntermediatePath directory, if you can't do this raise a fatal error,
   outputFileName will be set to the name of the generated file */
FILE *CreateOutputFile (char *blockName, char *formatExtension, char **outputFileName)
{
    FILE *outFile;
    char *filename;

    if (strcmp (blockName, "-") == 0)
        return stdout;

    filename = NEW_ARRAY (char, strlen (blockName) + 3 + (IntermediatePath ? strlen (IntermediatePath) : 0) + strlen (formatExtension));
    filename[0] = '\0';

    if (IntermediatePath)
        strcpy (filename, IntermediatePath);
    strcat (filename, "/");
    strcat (filename, blockName);
    strcat (filename, ".");
    strcat (filename, formatExtension);

    outFile = fopen (filename, "w");

    if (!outFile)
        LOG_ERROR (CannotOpenFileToWrite, MakeIdent1 (filename), NoPosition);
    (*outputFileName) = filename;
    return outFile;
}

/* WriteOutputFilePreamble : write out the prologue of an output intermediate file */
void WriteOutputFilePreamble (FILE * stream, char *fileType, char *commentString)
{
    time_t now = time (NULL);
    struct utsname systemName;
    struct passwd *passEntry;

    passEntry = getpwuid (getuid ());
    uname (&systemName);
    fprintf (stream,
      "%s %s\n%s Created: %s%s By: %s@%s (%s)\n%s With balsa-c version: %s\n%s Command: %s\n\n",
      commentString, fileType, commentString, ctime (&now),
      commentString, passEntry->pw_name, systemName.nodename,
      systemName.sysname, commentString, VERSION, commentString, MakeCommandLineString (NULL, 0));
}
