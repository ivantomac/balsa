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

*/

#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "flatten_breezefiles.h"
#include "libbreeze.h"

GList *searchPaths = 0;
GList *importedBreezeFiles = 0;

void addSearchPath (char *path)
{
    /* Check that the directory is not already present */
    GList *tmp = searchPaths;

    for (; tmp; tmp = tmp->next)
        if (strcmp ((char *) tmp->data, path) == 0)
            return;

    /* Add the directory to the list of paths */
    searchPaths = g_list_prepend (searchPaths, path);
}

char *findFullPathedNameFromDottedName (char *dottedName)
{
    char *tmpName = g_strdup (dottedName);
    char *ptr;

    // Dotted->Slashed name
    for (ptr = tmpName; *ptr; ptr++)
        if (*ptr == '.')
            *ptr = '/';

    // Check if this name is in the cache
    static GHashTable *hashtable = NULL;

    if (!hashtable)
        hashtable = g_hash_table_new (g_str_hash, g_str_equal);

    char *key = tmpName;
    gpointer value = g_hash_table_lookup (hashtable, (gconstpointer) key);

    if (value == NULL)
    {
        // Try each search path
        GList *tmp;

        for (tmp = g_list_last (searchPaths); tmp; tmp = tmp->prev)
        {
            char *completeName = g_strdup_printf ("%s/%s", (char *) tmp->data, tmpName);
            char *completeBreezeName = g_strdup_printf ("%s.breeze", completeName);

            int ret = access (completeBreezeName, R_OK);

            free (completeBreezeName);
            if (ret == 0)
            {
                g_hash_table_insert (hashtable, (gpointer) key, (gpointer) g_strdup (completeName));
                return completeName;
            }

            free (completeName);
        }
    } else
    {
        free (tmpName);
        return g_strdup (value);
    }

    fprintf (stderr, "File %s not found\n", dottedName);
    exit (EXIT_FAILURE);
}

struct BreezeFile *getImportedBreezeFileByName (char *name)
{
    GList *tmp;
    char *fullName = findFullPathedNameFromDottedName (name);

    for (tmp = importedBreezeFiles; tmp; tmp = tmp->next)
    {
        struct BreezeFile *breezeFile = (struct BreezeFile *) tmp->data;

        if (!strcmp (getBreezeFileName (breezeFile), fullName))
        {
            free (fullName);
            return breezeFile;
        }
    }

    // Imported breeze file not found in the list => We need to parse it now
#ifdef _DEBUG
    fprintf (stderr, " -- Importing %s (%s)\n", fullName, name);
#endif

    struct BreezeFile *breezeFile;

    breezeFile = breezeInitParse (fullName);

    free (fullName);
    if (breezeFile)
    {
        importedBreezeFiles = g_list_prepend (importedBreezeFiles, breezeFile);
        return breezeFile;
    } else
    {
        fprintf (stderr, "Importing Failed\n");
        exit (EXIT_FAILURE);
    }
}

/* Dunno where to put this function... */
void saveChannelNumbers (struct BreezePart *breezePart)
{
    //    char *partName = getBreezePartName (breezePart);
    int chanNum = 1;
    GList *chans = getBreezePartChannelsList (breezePart);

    for (chans = g_list_last (chans); chans; chans = chans->prev, chanNum++)
    {
        struct BreezePartChannelsChannel *chan = chans->data;
        char *chanName = getBreezePartChannelsChannelName (chan);

        if (chanName)
        {
            //     if (chanName[0]=='[')
            //  return;
            if (chanName[0] == ':')
                return;
            //     chanName = g_strdup_printf ("%s_[%s_#%d]", chanName, partName, chanNum);
            chanName = g_strdup_printf ("%s:%d", chanName, chanNum);
        } else
        {
            //     chanName = g_strdup_printf ("[%s_#%d]", partName, chanNum);
            chanName = g_strdup_printf (":%d", chanNum);
        }
        setBreezePartChannelsChannelName (chan, chanName);
    }
}
