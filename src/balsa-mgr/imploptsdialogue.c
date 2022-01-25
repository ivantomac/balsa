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

#include "imploptsdialogue.h"
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
#include "support.h"

gboolean existImplementationName (const char *impl_name, PtrBalsaTest test)
{                               // Check the impl name doesn't already exist
    if (test)
    {
        GList *impls = test->implementations;

        for (; impls; impls = impls->next)
        {
            PtrImplementation impl = (PtrImplementation) impls->data;

            if (!strcmp (impl_name, impl->name))
                return TRUE;
        }
    } else
    {
        GList *files;

        for (files = CurrentBalsaProject->files; files; files = files->next)
        {
            PtrBalsaFile file = (PtrBalsaFile) files->data;

            GList *tests = file->info.balsaFile.testFixtures;

            for (; tests; tests = tests->next)
            {
                PtrBalsaTest test2 = (PtrBalsaTest) tests->data;

                if (!strcmp (impl_name, test2->name))
                    return TRUE;
            }

            GList *impls = file->info.balsaFile.implementations;

            for (; impls; impls = impls->next)
            {
                PtrImplementation impl = (PtrImplementation) impls->data;

                if (!strcmp (impl_name, impl->name))
                    return TRUE;
            }
        }
    }

    return FALSE;
}

void OnBalsaNetlistOptionsDefault (GtkWidget * dialogue)
{
    GtkEntry *balsaNetlistOptionsEntry = GTK_ENTRY (LookupWidget (dialogue, "BalsaNetlistOptionsEntry"));
    PtrBalsaTechnology tech = (PtrBalsaTechnology) gtk_object_get_data (GTK_OBJECT (dialogue),
      "SelectedTechnology");

    gtk_entry_set_text (balsaNetlistOptionsEntry, (tech->balsaNetlistOptions ? tech->balsaNetlistOptions : ""));
}

void OnImplementationOptionsDialogueOK (GtkWidget * dialogue)
{
    GtkEntry *nameEntry = GTK_ENTRY (LookupWidget (dialogue, "NameEntry"));

    GtkEntry *balsaNetlistOptionsEntry = GTK_ENTRY (LookupWidget (dialogue, "BalsaNetlistOptionsEntry"));

    GtkEntry *makeImplTestOptionsEntry = GTK_ENTRY (LookupWidget (dialogue, "MakeImplTestOptionsEntry"));

    GtkEntry *techEntry = GTK_ENTRY (LookupWidget (dialogue, "BALSATECHEntry"));

    GtkCheckButton *DumpFileCheckButton = GTK_CHECK_BUTTON (LookupWidget (dialogue, "DumpFileCheckbutton"));
    GtkEntry *DumpFileEntry = GTK_ENTRY (LookupWidget (dialogue, "DumpFileEntry"));

    const char *nameValue = gtk_entry_get_text (GTK_ENTRY (nameEntry));

    const char *balsaNetlistOptionsValue = gtk_entry_get_text (GTK_ENTRY (balsaNetlistOptionsEntry));

    const char *makeImplTestOptionsValue = gtk_entry_get_text (GTK_ENTRY (makeImplTestOptionsEntry));

    const char *techValue = gtk_entry_get_text (GTK_ENTRY (techEntry));

    const char *DumpFileValue = 0;

    if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (DumpFileCheckButton)))
        DumpFileValue = gtk_entry_get_text (GTK_ENTRY (DumpFileEntry));

    PtrImplementation editedImpl = (PtrImplementation) gtk_object_get_data (GTK_OBJECT (dialogue),
      "EditedImpl");

    if (!nameValue || !*nameValue)
    {                           //empty test name
        printfConsole ("Error in AddImplementation : Please specify a name\n");
        return;
    }

    if (editedImpl)
    {
        editedImpl->name = g_strdup (nameValue);
        editedImpl->balsaNetlistOptions = g_strdup (balsaNetlistOptionsValue);
        editedImpl->makeImplTestOptions = g_strdup (makeImplTestOptionsValue);
        editedImpl->technology = g_strdup (techValue);
        editedImpl->DumpFile = (DumpFileValue && *DumpFileValue) ? g_strdup (DumpFileValue) : NULL;
        CurrentBalsaProject->dirty = TRUE;
        CurrentBalsaProject->needToRebuildMakefile = TRUE;
        UpdateProjectTrees ();
    } else
    {
        PtrBalsaFile balsaFile = (PtrBalsaFile) gtk_object_get_data (GTK_OBJECT (dialogue),
          "BalsaFile");
        PtrBalsaTest balsaTest = (PtrBalsaTest) gtk_object_get_data (GTK_OBJECT (dialogue),
          "BalsaTest");
        PtrImplementation impl;

        // check the test name doesn't already exist (if it is a new test or if the name changed)
        if (existImplementationName (nameValue, balsaTest))
        {
            printfConsole ("Error in OnImplementationOptionsDialogueOK: The specified implementation name is already in use\n");
            return;
        }

        impl = NewImplementationWithName (nameValue);

        impl->balsaNetlistOptions = g_strdup (balsaNetlistOptionsValue);
        impl->makeImplTestOptions = g_strdup (makeImplTestOptionsValue);
        impl->technology = g_strdup (techValue);
        impl->DumpFile = (DumpFileValue && *DumpFileValue) ? g_strdup (DumpFileValue) : NULL;
        impl->matchingBalsaFile = balsaFile;
        impl->matchingBalsaTest = balsaTest;
        if (balsaTest)
            AddImplementationToTest (CurrentBalsaProject, balsaTest, impl);
        else
            AddImplementationToFile (CurrentBalsaProject, balsaFile, impl);
    }

    gtk_widget_hide (dialogue);
}

void OnImplementationOptionsDialogueCancel (GtkWidget * dialogue)
{
    gtk_widget_hide (dialogue);
}

gboolean OnImplementationOptionsDialogue_keyPressEvent (GtkWidget * widget, GdkEventKey * event)
{
    if (event->keyval == GDK_Return)
        OnImplementationOptionsDialogueOK (widget);
    else if (event->keyval == GDK_Escape)
        OnImplementationOptionsDialogueCancel (widget);
    return FALSE;
}

void OnDumpFileCheckbuttonToggled (GtkToggleButton * togglebutton)
{
    GtkWidget *dialogue = gtk_widget_get_toplevel (GTK_WIDGET (togglebutton));
    GtkWidget *DumpFileLabel = LookupWidget (dialogue, "DumpFileLabel");
    GtkWidget *DumpFileEntry = LookupWidget (dialogue, "DumpFileEntry");
    gboolean active = gtk_toggle_button_get_active (togglebutton);

    gtk_widget_set_sensitive (DumpFileLabel, active);
    gtk_widget_set_sensitive (DumpFileEntry, active);
}

void CallImplementationSelectionWindow (PtrBalsaFile file, PtrBalsaTest test, PtrImplementation editedImpl)
{
    GtkWidget *dialogue = CreateWindow ("ImplementationOptionsDialogue");
    GtkEntry *nameEntry = GTK_ENTRY (LookupWidget (dialogue, "NameEntry"));

    GtkEntry *balsaNetlistOptionsEntry = GTK_ENTRY (LookupWidget (dialogue, "BalsaNetlistOptionsEntry"));

    GtkEntry *makeImplTestOptionsEntry = GTK_ENTRY (LookupWidget (dialogue, "MakeImplTestOptionsEntry"));

    GtkCheckButton *DumpFileCheckButton = GTK_CHECK_BUTTON (LookupWidget (dialogue, "DumpFileCheckbutton"));
    GtkLabel *DumpFileLabel = GTK_LABEL (LookupWidget (dialogue, "DumpFileLabel"));
    GtkEntry *DumpFileEntry = GTK_ENTRY (LookupWidget (dialogue, "DumpFileEntry"));

    if (editedImpl)
    {
        gtk_entry_set_text (nameEntry, g_strdup (editedImpl->name));
        gtk_entry_set_text (balsaNetlistOptionsEntry, g_strdup (editedImpl->balsaNetlistOptions));
        gtk_entry_set_text (makeImplTestOptionsEntry, g_strdup (editedImpl->makeImplTestOptions));
        gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (DumpFileCheckButton), (editedImpl->DumpFile != NULL));
        if (editedImpl->DumpFile && *editedImpl->DumpFile)
            gtk_entry_set_text (DumpFileEntry, g_strdup (editedImpl->DumpFile));
    } else
    {
        // find a unused name for the impl
        char *impl_name = NULL;
        int num = 0;

        do
        {
            num++;
            g_free (impl_name);
            impl_name = g_strdup_printf ("impl%d", num);
        }
        while (existImplementationName (impl_name, test));

        gtk_entry_set_text (nameEntry, impl_name);

        // fill the DumpFile filed
        if (test)
        {
            gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (DumpFileCheckButton), TRUE);
            gtk_entry_set_text (DumpFileEntry, g_strdup_printf ("%s-%s-impl.vcd", file->name, test->name));
        }
    }

    if (!test)
    {
        gtk_widget_hide (GTK_WIDGET (DumpFileCheckButton));
        gtk_widget_hide (GTK_WIDGET (DumpFileLabel));
        gtk_widget_hide (GTK_WIDGET (DumpFileEntry));
    }

    /* Add technology notebook */
    {
        PopulateTechnologyChooser (dialogue);
        if (editedImpl && editedImpl->technology && *editedImpl->technology)
            ParseBALSATECHValue (editedImpl->technology, dialogue);

        if (!editedImpl)        /* New impl */
        {
            PtrBalsaTechnology selectedTech = (PtrBalsaTechnology) gtk_object_get_data (GTK_OBJECT (dialogue),
              "SelectedTechnology");

            if (selectedTech && selectedTech->balsaNetlistOptions)
                gtk_entry_set_text (balsaNetlistOptionsEntry, selectedTech->balsaNetlistOptions);
        }

        /* Little tick icon */
        gtk_image_set_from_pixmap (GTK_IMAGE (LookupWidget (dialogue, "TickPixmapBox")), TickPixmap, TickMask);
    }

    /* Make name selected so we can directly overtype it */
    gtk_entry_select_region (nameEntry, 0, -1);
    gtk_widget_grab_focus (GTK_WIDGET (nameEntry));

    gtk_object_set_data (GTK_OBJECT (dialogue), "BalsaFile", file);
    gtk_object_set_data (GTK_OBJECT (dialogue), "BalsaTest", test);
    gtk_object_set_data (GTK_OBJECT (dialogue), "EditedImpl", editedImpl);

    gtk_widget_show (dialogue);
}
