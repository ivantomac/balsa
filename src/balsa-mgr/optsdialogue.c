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

#include <gdk/gdkkeysyms.h>
#include "optsdialogue.h"
#include "workspace.h"
#include "main.h"
#include "options.h"
#include "support.h"

struct _OptionsDialogue *new_optionsDialogue (void)
{
    struct _OptionsDialogue *obj = (struct _OptionsDialogue *) g_malloc (sizeof (struct _OptionsDialogue));

    obj->dialogue = NULL;

    return obj;
}

void OnOptionsDialogueOK ()
{
    /* Copy entry values */
    GtkWidget *BalsaHomeEntry = LookupWidget (workSpace.optionsDialogue->dialogue, "BalsaHomeEntry");
    GtkWidget *TmpDirEntry = LookupWidget (workSpace.optionsDialogue->dialogue, "TmpDirEntry");
    GtkWidget *EditorNameEntry = LookupWidget (workSpace.optionsDialogue->dialogue, "EditorNameEntry");
    GtkWidget *PrintCommandEntry = LookupWidget (workSpace.optionsDialogue->dialogue, "PrintCommandEntry");
    GtkWidget *PSViewerNameEntry = LookupWidget (workSpace.optionsDialogue->dialogue, "PSViewerNameEntry");
    GtkWidget *HistoryMaxSizeValue = LookupWidget (workSpace.optionsDialogue->dialogue, "HistoryMaxSizeValue");

    const char *tmp;

    tmp = gtk_entry_get_text (GTK_ENTRY (BalsaHomeEntry));
    if (tmp && tmp[0])
    {
        //todo: free previous environment options
        EnvironmentOptions.BalsaHome = g_strdup (tmp);
    } else
    {
        EnvironmentOptions.BalsaHome = NULL;
    }
    tmp = gtk_entry_get_text (GTK_ENTRY (TmpDirEntry));
    if (tmp && tmp[0])
    {
        //todo: free previous environment options
        EnvironmentOptions.TmpDir = g_strdup (tmp);
    } else
    {
        EnvironmentOptions.TmpDir = g_strdup ("/tmp");
    }
    tmp = gtk_entry_get_text (GTK_ENTRY (EditorNameEntry));
    if (tmp && tmp[0])
    {
        //todo: free previous environment options
        EnvironmentOptions.editor = g_strdup (tmp);
    } else
    {
        EnvironmentOptions.editor = NULL;
    }
    tmp = gtk_entry_get_text (GTK_ENTRY (PrintCommandEntry));
    if (tmp && tmp[0])
    {
        //todo: free previous environment options
        EnvironmentOptions.PrintCommand = g_strdup (tmp);
    } else
    {
        EnvironmentOptions.PrintCommand = NULL;
    }
    tmp = gtk_entry_get_text (GTK_ENTRY (PSViewerNameEntry));
    if (tmp && tmp[0])
    {
        //todo: free previous environment options
        EnvironmentOptions.PSViewer = g_strdup (tmp);
    } else
    {
        EnvironmentOptions.PSViewer = NULL;
    }
    EnvironmentOptions.projectsHistoryMaxSize = gtk_spin_button_get_value_as_int (GTK_SPIN_BUTTON (HistoryMaxSizeValue));

    gtk_widget_hide (workSpace.optionsDialogue->dialogue);
    WriteEnvironmentOptions ();
}

gboolean OnOptionsDialogueCancel (void)
{
    gtk_widget_hide (workSpace.optionsDialogue->dialogue);
    return TRUE;
}

gboolean OnOptionsKeyPressEvent (GtkWidget * widget, GdkEventKey * event)
{
    if (event->keyval == GDK_Return)
        OnOptionsDialogueOK ();
    else if (event->keyval == GDK_Escape)
        OnOptionsDialogueCancel ();

    return FALSE;
}

void OnBalsaHomeDefault (GtkButton * button)
{
    GtkWidget *BalsaHomeEntry = LookupWidget (workSpace.optionsDialogue->dialogue, "BalsaHomeEntry");

    gtk_entry_set_text (GTK_ENTRY (BalsaHomeEntry), (Options_BalsaHome ? Options_BalsaHome : ""));
}

void OnTmpDirDefault (GtkButton * button)
{
    GtkWidget *TmpDirEntry = LookupWidget (workSpace.optionsDialogue->dialogue, "TmpDirEntry");

    gtk_entry_set_text (GTK_ENTRY (TmpDirEntry), Options_TmpDir);
}

void OnEditorNameDefault (GtkButton * button)
{
    GtkWidget *EditorNameEntry = LookupWidget (workSpace.optionsDialogue->dialogue, "EditorNameEntry");

    gtk_entry_set_text (GTK_ENTRY (EditorNameEntry), Options_Editor);
}

void OnPrintCommandDefault (GtkButton * button)
{
    GtkWidget *PrintCommandEntry = LookupWidget (workSpace.optionsDialogue->dialogue, "PrintCommandEntry");

    gtk_entry_set_text (GTK_ENTRY (PrintCommandEntry), Options_PrintCommand);
}

void OnPSViewerNameDefault (GtkButton * button)
{
    GtkWidget *EditorNameEntry = LookupWidget (workSpace.optionsDialogue->dialogue, "PSViewerNameEntry");

    gtk_entry_set_text (GTK_ENTRY (EditorNameEntry), Options_PSViewer);
}

