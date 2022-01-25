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

	`paths.c'
	Filename / directory name / path string handling

*/

#include <unistd.h>
#include <pwd.h>
#include <sys/types.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#include "paths.h"
#include "project.h"
#include "utils.h"

/* GetHomeDir : get a home directory for user `user'. Newly allocated string returned */
char *GetHomeDir (const char *user)
{
    struct passwd *passEntry = getpwnam (user);

    return (passEntry ? g_strdup (passEntry->pw_dir) : NULL);
}

/* CleanUpFilename : remove ./ components, trailing /, repeated /s, ../ where a directory is
	present from the given string modifying that string, NB. the string always gets
	shorter */
void CleanUpFilename (char *filename)
{
    char *lastSlash = filename; /* used for backing up by 1 dir */
    char *endPtr = filename;

    while (endPtr && *endPtr)
    {
        gboolean advancePtr = TRUE;

        if (*endPtr == G_DIR_SEPARATOR)
        {
            if (!endPtr[1] && endPtr != filename) /* Trailing / */
                *endPtr = '\0';
            if (endPtr[1] == G_DIR_SEPARATOR)
            {                   /* remove runs of /// */
                char *newEnd = endPtr;

                while (*newEnd == G_DIR_SEPARATOR)
                    newEnd++;
                strcpy (endPtr + 1, newEnd);
                advancePtr = FALSE;
            } else if (endPtr[1] == '.')
            {
                if (endPtr[2])
                {
                    if (endPtr[2] == '/')
                    {           /* ./, trim */
                        strcpy (endPtr, endPtr + 2);
                        advancePtr = FALSE;
                    } else if (endPtr[2] == '.')
                    {
                        if (!endPtr[3])
                        {
                            *lastSlash = '\0';
                            return;
                        } else if (endPtr[3] == G_DIR_SEPARATOR)
                        {
                            strcpy (lastSlash, endPtr + 3); /* wipe out /../ */
                            endPtr = lastSlash;
                            /* Need to clean the whole string again */
                            endPtr = filename;
                            lastSlash = filename;
                            advancePtr = FALSE;
                        }
                    }
                } else
                    *endPtr = '\0'; /* abcd/.<eos> -> abcd<eos> */
            }
        }
        if (advancePtr)
        {
            lastSlash = endPtr;
            endPtr = strchr (endPtr + 1, G_DIR_SEPARATOR);
        }
    }
}

/* ExpandFilename : "expand" the given file/dirname to make it absolute (if `makeAbsolute' is
	TRUE), remove leading ~ terms. Returns a newly allocated string. Cleans up name a bit
	by removing extra slashes and ./'es. `pwd' contains the current (perceived) working directory.
	Set this to NULL to use the actual cwd. */
char *ExpandFilename (const char *filename, gboolean makeAbsolute, const char *pwd)
{
    char *workingFilename = g_strdup (filename);
    char *workingFilenamePtr = workingFilename;
    char *root = NULL;
    char *ret;

    if (!g_path_is_absolute (workingFilenamePtr))
    {
        if (*workingFilenamePtr == '~')
        {                       /* home dir relative */
            if (workingFilenamePtr[1] == '\0' || workingFilenamePtr[1] == G_DIR_SEPARATOR)
            {
                root = GetHomeDir (g_get_user_name ());
                workingFilenamePtr++;
            } else if (isalnum (workingFilenamePtr[1]))
            {                   /* ~whoever/a/b/c */
                char savedChar;
                char *endPtr = workingFilenamePtr + 1;

                while (isalnum (*endPtr))
                    endPtr++;
                savedChar = *endPtr;
                root = GetHomeDir (workingFilenamePtr + 1);
                *endPtr = savedChar;

                workingFilenamePtr = endPtr;
            }                   /* else leave the ~ in place, dunno what it means */
        } else if (makeAbsolute) /* starts with ./ (add current dir) or not / */
            root = g_strdup (pwd ? pwd : g_get_current_dir ());
    }
    ret = (char *) g_malloc ((root ? strlen (root) : 0) + strlen (workingFilenamePtr) + 2);
    *ret = '\0';
    if (root)
    {
        sprintf (ret, "%s%c", root, G_DIR_SEPARATOR);
        g_free (root);
    }
    strcat (ret, workingFilenamePtr);
    g_free (workingFilename);

    CleanUpFilename (ret);
    return ret;
}

/* FindPrefixPath : find from the given list the longest prefix which matches the
	start of the given filename. Returns that prefix (which will be one of the
	pointers in the original list) or NULL if no prefix is found */
char *FindPrefixPath (const char *filename, GList * paths)
{
    char *longestMatch = NULL;
    int longestMatchLength = 0;
    int filenameLength = strlen (filename);

    // printf("FindPrefixPath for %s\n",filename);
    while (paths)
    {
        int pathLength = strlen ((char *) paths->data);

        //  printf("  in %s\n",paths->data);

        if (filenameLength > pathLength
          && strncmp ((char *) paths->data, filename, pathLength) == 0 && filename[pathLength] == G_DIR_SEPARATOR && pathLength > longestMatchLength)
        {
            longestMatch = (char *) paths->data;
            longestMatchLength = pathLength;
        }
        paths = paths->next;
    }

    return longestMatch;
}

char *convertDottedPathToSlashedPath (char *filename)
{
    char *result = g_strdup (filename);
    char *tmp;

    for (tmp = result; *tmp; tmp++)
        if (*tmp == '.')
            *tmp = '/';

    if (strEndsWith (result, "/balsa"))
        result[strlen (result) - 6] = '.';

    return result;
}

/*LJ:*/
/* Find the paths from the imported paths, where the specified file is.
   If filename is of the form "/path/name" : looks for "/path" in the list of imported paths,
   Else, tries each imported path to find out where the file can be located */
char *FindImportPathForFile (const char *filename)
{
    FILE *tmp_file;

    if (filename[0] == '/')
    {
        tmp_file = fopen (filename, "r");
        if (tmp_file)
        {
            fclose (tmp_file);
            return "";
        }
    } else
    {
        char *foundPath = NULL;
        GList *path = CurrentBalsaProject->importPath;

        //      printf("filename=%s\n",filename);
        //  filename = convertDottedPathToSlashedPath (filename);

        for (; path; path = path->next)
        {
            char *tmp_filename = g_strdup_printf ("%s/%s", (char *) path->data, filename);

            tmp_file = fopen (tmp_filename, "r");
            g_free (tmp_filename);
            if (tmp_file)
            {
                fclose (tmp_file);
                if (!foundPath)
                    foundPath = path->data;
                else
                {
                    char message[500];

                    sprintf (message,
                      "Warning! More than one import path contains the researched filename (%s in %s). The first encoutered path will be used (%s)\n",
                      filename, (char *) path->data, foundPath);
                    printfConsole_s (message, "");
                }
            }
        }

        return foundPath;
    }

    return NULL;
}

gboolean isImportPath (char *path)
{
    char *absolutePath = ImportPaths_ConvertToAbsolutePath (path);

    GList *importPath = CurrentBalsaProject->importPath;

    for (; importPath; importPath = importPath->next)
    {
        char *import = importPath->data;
        char *absoluteImport = ImportPaths_ConvertToAbsolutePath (import);

        if (!strcmp (absoluteImport, absolutePath))
            return TRUE;
    }

    return FALSE;
}

char *GetWithoutPath (const char *filename)
{
    char *ptr = strrchr (filename, '/');

    if (!ptr)
        return g_strdup (filename);

    if (*(ptr + 1))
        return g_strdup (ptr + 1);
    else
        return g_strdup ("<noname>");
}

char *GetWithPath (const char *filename)
{
    char *path = FindImportPathForFile (filename);

    if (path)
        return g_strdup_printf ("%s/%s", path, filename);
    else
        return g_strdup (filename);
}

const char *GetWithoutExtension (const char *filename)
{
    char *lastpoint;

    lastpoint = strrchr (filename, '.');

    if (lastpoint == NULL)
        return filename;

    if (strcmp (lastpoint, ".balsa") != 0 && strcmp (lastpoint, ".breeze") != 0)
        return filename;

    {
        int size = lastpoint - filename;
        char *result = (char *) g_malloc (size + 1);

        strncpy (result, filename, size);
        result[size] = 0;
        return result;
    }
}

char *ConvertToBalsaFilename (const char *filename)
{
    // char *newname;

    if (strlen (filename) > 6 && !strcmp (filename + strlen (filename) - 6, ".balsa"))
        return g_strdup (filename);
    else
        return g_strdup_printf ("%s.balsa", filename);
}

char *ConvertToPathedBalsaFilename (const char *filename)
{
    char *balsaName = ConvertToBalsaFilename (filename);
    char *prefix = FindImportPathForFile (balsaName);

    if (!prefix || !strcmp (prefix, ".") || !strcmp (prefix, "./"))
        return balsaName;

    char *res;

    if (prefix[strlen (prefix) - 1] == '/')
        res = g_strdup_printf ("%s%s", prefix, balsaName);
    else
        res = g_strdup_printf ("%s/%s", prefix, balsaName);

    free (balsaName);
    return res;
}

char *ConvertToBreezeFilename (const char *filename)
{
    char *newname;

    if (!strcmp (filename + strlen (filename) - 6, ".balsa"))
    {
        newname = g_malloc (strlen (filename) + 2);
        strcpy (newname, filename);
        strcpy (newname + strlen (filename) - 6, ".breeze");
    } else
    {
        newname = g_malloc (strlen (filename) + 8);
        strcpy (newname, filename);
        strcpy (newname + strlen (filename), ".breeze");
    }
    return newname;
}

char *ConvertToPathedBreezeFilename (const char *filename)
{
    char *balsaName = ConvertToBalsaFilename (filename);
    char *prefix = FindImportPathForFile (balsaName);
    char *breezeName = ConvertToBreezeFilename (filename);

    if (!prefix || !strcmp (prefix, ".") || !strcmp (prefix, "./"))
        return breezeName;

    char *res;

    if (prefix[strlen (prefix) - 1] == '/')
        res = g_strdup_printf ("%s%s", prefix, breezeName);
    else
        res = g_strdup_printf ("%s/%s", prefix, breezeName);

    free (breezeName);
    return res;
}

/*************/
char *ImportPaths_ConvertFileToRelative (const char *file)
{
    char *lastslash = strrchr (file, '/');
    char *relpath, *newfile;

    if (!lastslash)
        return g_strdup (file);

    *lastslash = 0;
    relpath = ImportPaths_ConvertToRelativePath (file);
    *lastslash = '/';

    if (!strcmp (relpath, "."))
        newfile = g_strdup (lastslash + 1);
    else
        newfile = g_strconcat (relpath, lastslash, NULL);

    g_free (relpath);
    return newfile;
}

char *ImportPaths_ConvertToRelativePath (const char *path)
{
    if (CurrentBalsaProject)
    {
        return ConvertToRelativePath (path, CurrentBalsaProject->directory);
    } else
    {
        char buf[1000];

        getcwd (buf, 1000);
        return ConvertToRelativePath (path, buf);
    }
}

char *ImportPaths_ConvertToAbsolutePath (const char *path)
{
    char buf[1000];

    chdir (CurrentBalsaProject->directory);
    chdir (path);
    getcwd (buf, 1000);
    chdir (CurrentBalsaProject->directory);
    return g_strdup (buf);
}

char *ConvertToAbsolutePath (const char *path, const char *pathref)
{
    char buf[1000];
    char buf2[1000];

    getcwd (buf2, 1000);
    if (pathref)
        chdir (pathref);
    chdir (path);
    getcwd (buf, 1000);
    chdir (buf2);
    return g_strdup (buf);
}

char *ConvertToRelativePath (const char *path, const char *pathref)
{
    char *absolutePath = ConvertToAbsolutePath (path, pathref);

    char **splitted1 = g_strsplit (absolutePath, "/", 100);
    char **splitted2 = g_strsplit (pathref, "/", 100);

    char **ptr1 = splitted1;
    char **ptr2 = splitted2;

    //  printf("path=%s\nfrom:%s\n",absolutePath,CurrentBalsaProject->directory);

    // 1st step: remove the common part in the 2 paths
    while (*ptr1 && *ptr2 && !strcmp (*ptr1, *ptr2))
    {
        ptr1++;
        ptr2++;
    }

    //2nd step: add the right number of ".." directories.
    {
        char *newPath = g_strjoinv ("/", ptr1);

        while (*ptr2)
        {
            char *oldPath = newPath;

            newPath = g_strconcat ("../", oldPath, NULL);
            g_free (oldPath);
            ptr2++;
        }

        g_strfreev (splitted1);
        g_strfreev (splitted2);
        g_free (absolutePath);

        if (strlen (newPath) && (newPath[strlen (newPath) - 1] == '/'))
            newPath[strlen (newPath) - 1] = 0;

        if (!strlen (newPath))
            newPath = g_strdup (".");

        //    printf("relative path=>%s\n",newPath);
        return newPath;
    }
}

char *ImportPaths_GetCompletePathedFilename (char *filename)
{
    if (strstr (filename, "/"))
        return g_strdup (filename);
    {
        char *path = FindImportPathForFile (filename);

        if (!path || !*path)
            return g_strdup (filename);
        else if (path[strlen (path) - 1] == '/')
            return g_strconcat (path, filename, NULL);
        else if (!strcmp (path, "."))
            return g_strdup (filename);
        else
            return g_strconcat (path, "/", filename, NULL);
    }
}
