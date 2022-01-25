
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

	`paths.h'
	Filename / directory name / path string handling

*/

#ifndef PATHS_HEADER
#define PATHS_HEADER

#include <glib.h>

/* GetHomeDir : get a home directory for user `user'. Newly allocated string returned */
extern char *GetHomeDir (const char *user);

/* CleanUpFilename : remove ./ components, repeated /s, ../ where a directory is
	present from the given string modifying that string, NB. the string always gets
	shorter */
extern void CleanUpFilename (char *filename);

/* ExpandFilename : "expand" the given file/dirname to make it absolute (if `makeAbsolute' is
	TRUE), remove leading ~ terms. Returns a newly allocated string. Cleans up name a bit
	by removing extra slashes and ./'es. `pwd' contains the current (perceived) working directory.
	Set this to NULL to use the actual cwd. */
extern char *ExpandFilename (const char *filename, gboolean makeAbsolute, const char *pwd);

/* FindPrefixPath : find from the given list the longest prefix which matches the
	start of the given filename. Returns that prefix (which will be one of the
	pointers in the original list) or NULL if no prefix is found */
extern char *FindPrefixPath (const char *filename, GList * paths);

/* Find the paths from the imported paths, where the specified file is.
   If filename is of the form "/path/name" : looks for "/path" in the list of imported paths,
   Else, tries each imported path to find out where the file can be located */
char *FindImportPathForFile (const char *filename);

char *GetPath (char *filename);
char *GetWithoutPath (const char *filename);
char *GetFileExtension (char *filename);
const char *GetWithoutExtension (const char *filename);
char *ConvertToBalsaFilename (const char *filename);
char *ConvertToPathedBalsaFilename (const char *filename);

//char *ConvertToSBreezeFilename (char *filename);
char *ConvertToBreezeFilename (const char *filename);
char *ConvertToPathedBreezeFilename (const char *filename);

gboolean isImportPath (char *path);
void ImportPaths_AddToCurrentBalsaProject (const char *newPath);
void ImportPaths_RemoveFromCurrentBalsaProject (const char *path);

char *ImportPaths_ConvertFileToRelative (const char *file);
char *ImportPaths_ConvertToRelativePath (const char *path);
char *ImportPaths_ConvertToAbsolutePath (const char *path);

char *ConvertToAbsolutePath (const char *path, const char *pathref);
char *ConvertToRelativePath (const char *path, const char *pathref);

char *ImportPaths_GetCompletePathedFilename (char *filename);

#define ConvertToPathedFilename(filename) GetWithoutExtension(ConvertToPathedBreezeFilename(filename))

#endif
