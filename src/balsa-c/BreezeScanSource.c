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

	`BreezeScanSource.c'
	Modified automatically generated rex Source.c file
	Added code to track file inclusion
	
 */

#include "output.h"
#include "BreezeScan.h"
#include "BreezeScanSource.h"
#include "flags.h"
#include "rSystem.h"
#include <stdio.h>
#include <unistd.h>
#include <glib.h>

/* Breeze_TreeRoot : root of parse tree after calling BreezeScan_BeginFile */
tTree Breeze_TreeRoot = NULL;

int BreezeScan_BeginSource (Ptrchar fileName)
{
    int fileNo = OpenInput (fileName);
    tIdent fileNameIdent = MakeIdent1 (fileName);

    /* Add this file onto the tree */
    CurrentFile = NewIdentList (fileNameIdent, NoPosition, CurrentFile);

    if (!VisitedFiles)
        VisitedFiles = g_hash_table_new (g_direct_hash, g_direct_equal);

    g_hash_table_insert (VisitedFiles, (gpointer) fileNameIdent, (gpointer) true);

    return fileNo;
}

int BreezeScan_GetLine (int file, Ptrchar buffer, int size)
{
    return rRead (file, buffer, size);
}

void BreezeScan_CloseSource (int file)
{
    /* Need to update Attribute file in scanner */
    extern BreezeScan_tScanAttribute BreezeScan_Attribute;
    PtrIdentList lastFile = CurrentFile;

    CurrentFile = CDR (CurrentFile);
    /* Don't drop the file in Attribute.Position.File if this is the last file */
    BreezeScan_Attribute.Position.File = (CurrentFile ? CurrentFile : lastFile);

    rClose (file);
}

/* ReverseMakeString : Add elements of ident list in reverse order into the preallocated 
   string string, separating with the given character, returns length of constructed string */
size_t ReverseMakeString (Ptrchar string, tTree ident, char separator)
{
    size_t lengthSoFar = 1;
    Ptrconstchar identStr = PeekString (ident->Ident.ident);

    if (ident->Ident.next->Kind != kNullIdents)
    {
        lengthSoFar = ReverseMakeString (string, ident->Ident.next, separator);

        if (identStr)
        {
            string[lengthSoFar - 1] = separator;
            string[lengthSoFar] = '\0';
            lengthSoFar++;
        }
    }
    if (identStr)
    {
        strcat (string, identStr);
        lengthSoFar += strlen (identStr);
    }
    return lengthSoFar;
}

/* MakePathFromDottedPath : Alloc a string to hold the slash separated path given in a
   dotted_path, NB. this is in reverse order in the dotted_path */
Ptrchar MakePathFromDottedPath (tTree ident, char pathSeparator)
{
    size_t stringLength = 0;
    Ptrchar returnString;
    tTree tree = ident;

    while (tree->Kind != kNullIdents)
    {
        Ptrconstchar ident = PeekString (tree->Ident.ident);

        if (ident)
            stringLength += strlen (ident) + 1;
        tree = tree->Ident.next;
    }
    returnString = NEW_ARRAY (char, stringLength);

    ReverseMakeString (returnString, ident, pathSeparator);
    return returnString;
}

/* HandleImport : handle the importing of breeze files in balsa, this is called by the 
   parser for the 'import' non terminal */
PtrContext HandleImport (Ptrchar dotpath, tPosition position, PtrContext contextIn, Scope scope)
{
    /* Find the imported file */
    /* Do the MakePath's once? */
    Ptrchar filename = NULL;
    Ptrchar intermediatePath = StrDup (dotpath);
    PtrContext contextOut = contextIn;

    SubstituteCharInString (intermediatePath, '.', '/');

    if (intermediatePath)
        filename = ExpandPathToFile (intermediatePath, "breeze");

    if (!filename)
        LOG_ERROR (CannotOpenFileToRead, MakeIdent1 (dotpath), position);
    else
    {
        bool alreadyVisitedFile = (VisitedFiles ? (g_hash_table_lookup (VisitedFiles,
              (gpointer) MakeIdent1 (filename)) != NULL) : false);

        /* Is this the latest identifier? (is the filename new?) */
        if (Verbose)
        {
            WriteErrorPosition (stderr, position);
            if (!alreadyVisitedFile)
                fprintf (stderr, " verbose: importing file `%s' for [%s]\n", filename, dotpath);
            else
                fprintf (stderr, " verbose: already imported [%s], skipping\n", dotpath);
        }
        if (!alreadyVisitedFile)
        {
            /* Top level import */
            if (!FlattenOutputFile && scope == TopLevelScope)
            {
                if (DoGenerateBreezeFile)
                    fprintf (BreezeOutputFile, "(import \"%s\")\n", dotpath);
            }
            BreezeScan_BeginFile (filename);

            if (Breeze () == 0 && Breeze_TreeRoot->Kind != kNullDecls)
            {
                tTree root = Breeze_TreeRoot;

                root->Decls.skip = false;
                root->Decls.contextIn = contextIn;
                root->Decls.scope = ImportedScope;

                VisitTreeNode (root);

                contextOut = root->Decl.contextOut;
            }
            if (Verbose)
            {
                WriteErrorPosition (stderr, position);
                fprintf (stderr, " verbose: closed file `%s'\n", filename);
            }
        }
    }
    return contextOut;
}
