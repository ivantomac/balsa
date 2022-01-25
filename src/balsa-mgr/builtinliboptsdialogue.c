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

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include "builtinliboptsdialogue.h"
#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>
#include <string.h>
#include "workspace.h"
#include "file.h"
#include "project.h"
#include "icons.h"
#include "technology.h"
#include "utils.h"
#include "main.h"
#include "miscgtk.h"
#include "paths.h"
#include "support.h"

void OnBuiltinLibOptionsDialogueOK (GtkWidget * dialogue)
{
    GtkEntry *nameEntry = GTK_ENTRY (LookupWidget (dialogue, "NameEntry"));
    GtkEntry *compileOptionsEntry = GTK_ENTRY (LookupWidget (dialogue, "CompileOptionsEntry"));
    GtkList *sourceList = GTK_LIST (LookupWidget (dialogue, "SourceList"));

    const char *nameValue = gtk_entry_get_text (GTK_ENTRY (nameEntry));
    const char *compileOptionsValue = gtk_entry_get_text (GTK_ENTRY (compileOptionsEntry));

    PtrBuiltinLib editedTest = (PtrBuiltinLib) gtk_object_get_data (GTK_OBJECT (dialogue),
      "EditedTest");

    if (!nameValue || !*nameValue)
    {                           //empty test name
        printfConsole ("Error in AddBuiltinLib : Please specify a test name\n");
        return;
    }

    /* Prepare the List of filenames */
    GList *sourceFilenameList = NULL;
    GList *tmp = sourceList->children;

    for (; tmp; tmp = tmp->next)
    {
        GtkListItem *item = tmp->data;
        char *text = (char *) gtk_object_get_data (GTK_OBJECT (item), "label");

        //  GtkLabel *label = item->item;
        //  char *str = label->label;
        sourceFilenameList = g_list_append (sourceFilenameList, text);
    }

    if (editedTest)
    {
        editedTest->name = g_strdup (nameValue);
        editedTest->compileOptions = g_strdup (compileOptionsValue);
        editedTest->sourceFilenames = sourceFilenameList;

        CurrentBalsaProject->dirty = TRUE;
        CurrentBalsaProject->needToRebuildMakefile = TRUE;
        UpdateProjectTrees ();
    } else
    {
        PtrBalsaFile balsaFile = (PtrBalsaFile) gtk_object_get_data (GTK_OBJECT (dialogue),
          "BalsaFile");

        PtrBuiltinLib lib = NewBuiltinLibWithName (nameValue);

        lib->compileOptions = g_strdup (compileOptionsValue);
        lib->sourceFilenames = sourceFilenameList;

        lib->matchingBalsaFile = balsaFile;

        AddBuiltinLibToFile (CurrentBalsaProject, balsaFile, lib);
    }

    gtk_widget_hide (dialogue);
}

void OnBuiltinLibOptionsDialogueCancel (GtkWidget * dialogue)
{
    gtk_widget_hide (dialogue);
}

gboolean OnBuiltinLibOptionsDialogue_keyPressEvent (GtkWidget * widget, GdkEventKey * event)
{
    if (event->keyval == GDK_Return)
        OnBuiltinLibOptionsDialogueOK (widget);
    else if (event->keyval == GDK_Escape)
        OnBuiltinLibOptionsDialogueCancel (widget);
    return FALSE;
}

void CallBuiltinLibSelectionWindow (PtrBalsaFile file, PtrBuiltinLib editedTest)
{
    GtkWidget *dialogue = CreateWindow ("BuiltinLibOptionsDialogue");

    GtkEntry *nameEntry = GTK_ENTRY (LookupWidget (dialogue, "NameEntry"));
    GtkEntry *compileOptionsEntry = GTK_ENTRY (LookupWidget (dialogue, "CompileOptionsEntry"));
    GtkList *sourceList = GTK_LIST (LookupWidget (dialogue, "SourceList"));

    if (editedTest)
    {
        gtk_entry_set_text (nameEntry, g_strdup (editedTest->name));
        gtk_entry_set_text (GTK_ENTRY (compileOptionsEntry), editedTest->compileOptions);

        GList *items = MakeListItemListFromStringList (editedTest->sourceFilenames);

        gtk_list_append_items (sourceList, items);
    } else
    {                           // find a unused name for the test
        char *test_name = NULL;
        int num = 1;

        test_name = g_strdup (file->name);
        goto jump1;
      retry:
        g_free (test_name);
        test_name = g_strdup_printf ("lib%d", num);
      jump1:
        {                       // Check the test name doesn't already exist
            GList *files;

            for (files = CurrentBalsaProject->files; files; files = files->next)
            {
                PtrBalsaFile file = (PtrBalsaFile) files->data;

                GList *tests = file->info.balsaFile.testFixtures;

                for (; tests; tests = tests->next)
                {
                    PtrBalsaTest test = (PtrBalsaTest) tests->data;

                    if (!strcmp (test_name, test->name))
                    {
                        num++;
                        goto retry;
                    }
                }

            }
        }

        gtk_entry_set_text (nameEntry, test_name);
    }

    gtk_entry_select_region (nameEntry, 0, -1);
    gtk_widget_grab_focus (GTK_WIDGET (nameEntry));

    gtk_object_set_data (GTK_OBJECT (dialogue), "BalsaFile", file);
    gtk_object_set_data (GTK_OBJECT (dialogue), "EditedTest", editedTest);

    gtk_widget_show (dialogue);
}

gboolean BuiltinLibOptionsNewButtonClicked_callback (char *filename, gpointer userData)
{
    if (!filename || !*filename)
        return TRUE;

    GtkWidget *dialogue = GTK_WIDGET (userData);
    GtkList *sourceList = GTK_LIST (LookupWidget (dialogue, "SourceList"));
    char *relativeFilename = ImportPaths_ConvertFileToRelative (filename);
    GList *itemsStr = g_list_prepend (NULL, relativeFilename);
    GList *items = MakeListItemListFromStringList (itemsStr);

    gtk_list_append_items (sourceList, items);
    return TRUE;                /* destroy dialogue */
}

void OnBuiltinLibOptionsNewButtonClicked (GtkWidget * dialogue)
{
    MakeFileSelectionDialogue (FileSelectionDialogue_Open, "Select your Source File",
      (FileSelectionCallback) BuiltinLibOptionsNewButtonClicked_callback, (gpointer) GTK_WIDGET (dialogue), GTK_WIDGET (dialogue));
}

void OnBuiltinLibOptionsRemoveButtonClicked (GtkWidget * dialogue)
{
    GtkList *sourceList = GTK_LIST (LookupWidget (dialogue, "SourceList"));
    GList *selection = sourceList->selection;

    if (!selection)
        return;
    GList *copy = g_list_copy (selection);

    gtk_list_remove_items (sourceList, copy);
    g_list_free (copy);
}
