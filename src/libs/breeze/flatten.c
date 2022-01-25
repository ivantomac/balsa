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

#include <string.h>
#include <glib.h>
#include "flatten.h"
#include "flatten_breezefiles.h"
#include "flatten_flatten.h"
#include "flatten_types.h"

#include "libbreeze.h"
#include "breezestructs.h"

/* LibBreeze_AddIncludePath : add a directory the the path used to find included files/libraries */
void LibBreeze_AddIncludePath (char *directory)
{
//    printf("adding %s\n", directory);
    addSearchPath (g_strdup (directory));
}

GList *LibBreeze_GetIncludePathList (void)
{
    return searchPaths;
}

void LibBreeze_FlattenBreezePart (struct BreezePart *breezePart)
{
    //    gboolean save_channel_numbers = TRUE; //FALSE;
    //    char *outputFile = "toto.breeze";

    if (save_channel_numbers)
        saveChannelNumbers (breezePart);
    flattenProcedureCalls (breezePart);
    //    resolveTypes (breezePart);

    makeCallcontextNamesUnique (getBreezePartCallcontextsList (breezePart));

    /* Todo better */
    struct BreezeFile *breezeFile = getBreezePartFile (breezePart);
    GList *imports = NULL;
    GList *types = NULL;

    getCompleteTypeList (breezeFile, &imports, &types);
    breezeFile->imports = imports;
    breezeFile->types = types;
    /*
       g_list_free (imports);
       g_list_free (types);
     */

    /* Keep only the flattened part in the breeze file */
    breezeFile->parts = g_list_prepend (NULL, breezePart);
}
