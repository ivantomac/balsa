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

	`makefiletable.c'
	Signal handlers and main function

*/

#include <gtk/gtk.h>
#include <string.h>
#include <glib.h>

#include "main.h"
#include "project.h"
#include "file.h"
#include "paths.h"
#include "commands.h"
#include "executionmanager.h"
#include "support.h"

BalsaProjectEntryNature CurrentSelection_Type = 0;
char *CurrentSelection_Name = 0;

#define STR_MAKE_SIM(name) g_strdup_printf("sim-pre-%s",name)
#define STR_RUN_SIM(name) g_strdup_printf("sim-%s",name)
#define STR_RUN_SIMWIN(name) g_strdup_printf("sim-win-%s",name)
#define STR_MAKE_PS(name) g_strdup_printf("%s.ps",name)
#define STR_MAKE_IMPL2(filename,implname) g_strdup_printf("impl-%s-%s",filename,implname)
#define STR_MAKE_TESTIMPL(testname,name) g_strdup_printf("sim-pre-%s-%s",testname,name)
#define STR_RUN_TESTIMPL(testname,name) g_strdup_printf("sim-%s-%s",testname,name)
#define STR_VIEW_TESTIMPL(testname,name) g_strdup_printf("view-sim-%s-%s",testname,name)

void MakefileTable_MakeCallback (GtkWidget * widget, gpointer data)
{
    char *command = g_strdup_printf ("make -n %s", (char *) data);
    GList *commandList = g_list_append (NULL, command);

    ExecutionManager_CheckBuildMakefileAndRunCommandList (commandList);
}

void MakefileTable_RunCostCallback (GtkWidget * widget, gpointer data)
{
    char *breezeFilename = g_strdup_printf ("%s.breeze", (char *) data);
    char *makeCommand = g_strdup_printf ("make -n %s", breezeFilename);
    char *costCommand = g_strdup_printf ("breeze-cost %s", breezeFilename);
    GList *commandList = g_list_append (NULL, makeCommand);

    commandList = g_list_append (commandList, costCommand);
    ExecutionManager_CheckBuildMakefileAndRunCommandList (commandList);
}

void MakefileTable_ViewPSCallback (GtkWidget * widget, gpointer data)
{
    char *makeCommand = g_strdup_printf ("make -n %s", (char *) data);
    char *viewCommand = g_strdup_printf ("%s %s",
      ((EnvironmentOptions.PSViewer && EnvironmentOptions.PSViewer[0]) ? EnvironmentOptions.PSViewer : "gv"),
      (char *) data);
    GList *commandList = g_list_append (NULL, makeCommand);

    commandList = g_list_append (commandList, viewCommand);
    ExecutionManager_CheckBuildMakefileAndRunCommandList (commandList);
}

void MakefileTable_ViewVCDCallback (GtkWidget * widget, gpointer data)
{
    char *viewCommand = g_strdup_printf ("gtkwave %s", (char *) data);
    GList *commandList = g_list_append (NULL, viewCommand);

    ExecutionManager_RunCommandList (commandList);
}

void MakefileTable_InsertBitmapInFrontOfCurrentSelectionAtRow (int row)
{
    GtkWidget *table = gtk_object_get_data (GTK_OBJECT (MainWindow), "MakefileTable");
    GtkWidget *iconw = gtk_pixmap_new (SelectedPixmap, SelectedMask);

    gtk_widget_show (iconw);
    gtk_table_attach (GTK_TABLE (table), iconw, 1, 2, row, row + 1, //left,right,top,bottom attaches
      GTK_FILL,                 //xoptions
      GTK_FILL,                 //yoptions
      10, 0);                   //x and y-padding
}

void
UpdateMakefileTable_AddRow (GtkTable * table, int row,
  const char *textLabel, const char *textButton1,
  GtkSignalFunc button1Callback, gpointer button1Data, const char *textButton2, GtkSignalFunc button2Callback, gpointer button2Data)
{
    GtkWidget *newLabel, *newButton;

    newLabel = gtk_label_new (textLabel);
    gtk_label_set_justify (GTK_LABEL (newLabel), GTK_JUSTIFY_LEFT);
    gtk_widget_show (newLabel);
    gtk_table_attach (table, newLabel, 0, 1, row, row + 1, //left,right,top,bottom attaches
      GTK_FILL,                 //xoptions
      GTK_FILL,                 //yoptions
      0, 0);                    //x and y-padding
    gtk_misc_set_alignment (GTK_MISC (newLabel), 0, 0.5);

    // Creation of the buttons
    if (textButton1)
    {
        newButton = gtk_button_new_with_label (textButton1);
        //gtk_label_set_justify( GTK_LABEL(newLabel), GTK_JUSTIFY_LEFT);
        gtk_widget_show (newButton);
        gtk_table_attach (table, newButton, 1, 2, row, row + 1, //left,right,top,bottom attaches
          GTK_FILL,             //xoptions
          GTK_FILL,             //yoptions
          5, 1);                //x and y-padding
        if (button1Callback)
            gtk_signal_connect (GTK_OBJECT (newButton), "clicked", button1Callback, button1Data);
    }

    if (textButton2)
    {
        newButton = gtk_button_new_with_label (textButton2);
        //gtk_label_set_justify( GTK_LABEL(newLabel), GTK_JUSTIFY_LEFT);
        gtk_widget_show (newButton);
        gtk_table_attach (table, newButton, (textButton1) ? 2 : 1, 3, row, row + 1, //left,right,top,bottom attaches
          GTK_FILL,             //xoptions
          GTK_FILL,             //yoptions
          5, 1);                //x and y-padding
        if (button2Callback)
            gtk_signal_connect (GTK_OBJECT (newButton), "clicked", button2Callback, button2Data);
    }
}

void UpdateMakefileTable_AddSeparator (GtkTable * table, int row, int nb_cols, int xpadding, int ypadding)
{
    GtkWidget *newSeparator;

    newSeparator = gtk_hseparator_new ();
    gtk_widget_show (newSeparator);
    gtk_table_attach (table, newSeparator, 0, nb_cols, row, row + 1, //left,right,top,bottom attaches
      GTK_FILL,                 //xoptions
      GTK_FILL,                 //yoptions
      xpadding, ypadding);      //x and y-padding
}

int UpdateMakefileTable_AddTest (GtkTable * table, int row, PtrBalsaTest test)
{
    if (CurrentSelection_Type == BalsaProjectEntry_Test)
        if (CurrentSelection_Name && !strcmp (CurrentSelection_Name, test->name))
            MakefileTable_InsertBitmapInFrontOfCurrentSelectionAtRow (row);

    UpdateMakefileTable_AddRow (table, row++, g_strdup_printf ("  %s", test->name), NULL, NULL, NULL, NULL, NULL, NULL);
    UpdateMakefileTable_AddRow (table, row++,
      g_strdup_printf ("     sim-%s", test->name),
      " Make ", (GtkSignalFunc) MakefileTable_MakeCallback, STR_MAKE_SIM (test->name), " Run ", (GtkSignalFunc) MakefileTable_MakeCallback, STR_RUN_SIM (test->name));
    UpdateMakefileTable_AddRow (table, row++,
      g_strdup_printf ("     sim-win-%s", test->name),
      " Make ", (GtkSignalFunc) MakefileTable_MakeCallback, STR_MAKE_SIM (test->name), " Run ", (GtkSignalFunc) MakefileTable_MakeCallback, STR_RUN_SIMWIN (test->name));

    GList *tmp = test->implementations;

    for (; tmp; tmp = tmp->next)
    {
        PtrImplementation impl = tmp->data;

        UpdateMakefileTable_AddRow (table, row++,
          g_strdup_printf ("     sim-%s-%s",
            test->name, impl->name),
          " Make ", (GtkSignalFunc) MakefileTable_MakeCallback,
          STR_MAKE_TESTIMPL (test->name, impl->name), " Run ", (GtkSignalFunc) MakefileTable_MakeCallback, STR_RUN_TESTIMPL (test->name, impl->name));

        char *VCDfilename = impl->DumpFile;

        if (VCDfilename)
            UpdateMakefileTable_AddRow (table, row++,
              g_strdup_printf ("        %s", VCDfilename), " View ", (GtkSignalFunc) MakefileTable_ViewVCDCallback, VCDfilename, NULL, NULL, NULL);
    }

    return row;
}

int UpdateMakefileTable_AddImplementation (GtkTable * table, int row, PtrImplementation impl)
{
    if (CurrentSelection_Type == BalsaProjectEntry_Implementation)
        if (CurrentSelection_Name && !strcmp (CurrentSelection_Name, impl->name))
            MakefileTable_InsertBitmapInFrontOfCurrentSelectionAtRow (row);

    UpdateMakefileTable_AddRow (table, row++,
				g_strdup_printf ("  %s", impl->name), " Make ", (GtkSignalFunc) MakefileTable_MakeCallback, STR_MAKE_IMPL2 (impl->matchingBalsaFile->name, impl->name), NULL, NULL, NULL);

    return row;
}

int UpdateMakefileTable_AddFile (GtkTable * table, int row, PtrBalsaFile file)
{
    if (CurrentSelection_Type == BalsaProjectEntry_File)
        if (CurrentSelection_Name && !strcmp (CurrentSelection_Name, file->name))
            MakefileTable_InsertBitmapInFrontOfCurrentSelectionAtRow (row);

    UpdateMakefileTable_AddRow (table, row++, ConvertToPathedBalsaFilename (file->name), NULL, NULL, NULL, NULL, NULL, NULL);
    UpdateMakefileTable_AddRow (table, row++,
      g_strdup_printf ("%s.breeze", file->name),
      " Compile ", (GtkSignalFunc) MakefileTable_MakeCallback, ConvertToPathedBreezeFilename (file->name), NULL, NULL, NULL);
    UpdateMakefileTable_AddRow (table, row++, "     cost", " Run ",
      (GtkSignalFunc) MakefileTable_RunCostCallback, (gpointer) ConvertToPathedFilename (file->name), NULL, NULL, NULL);
    UpdateMakefileTable_AddRow (table, row++,
      g_strdup_printf ("     %s.ps",
        ConvertToPathedFilename (file->
          name)),
      " Make ", (GtkSignalFunc) MakefileTable_MakeCallback,
      STR_MAKE_PS (ConvertToPathedFilename (file->name)), " View ", (GtkSignalFunc) MakefileTable_ViewPSCallback, STR_MAKE_PS (ConvertToPathedFilename (file->name)));
    return row;
}

int UpdateMakefileTable_Tests (GtkTable * table, int row)
{                               // Fill the test entries
    GList *files;
    GtkWidget *newLabel;        // *newButton;
    int doesNeedSeparator = 0;

    UpdateMakefileTable_AddSeparator (table, row++, 3, 2, 10);

    newLabel = gtk_label_new ("        Tests");
    gtk_table_attach (table, newLabel, 0, 1, row, row + 1, //left,right,top,bottom attaches
      GTK_FILL | GTK_EXPAND,    //xoptions
      GTK_FILL,                 //yoptions
      0, 0);                    //x and y-padding
    gtk_widget_show (newLabel);

    row++;

    for (files = CurrentBalsaProject->files; files; files = files->next)
    {
        PtrBalsaFile file = (PtrBalsaFile) files->data;
        GList *tests = file->info.balsaFile.testFixtures;

        for (; tests; tests = tests->next)
        {
            PtrBalsaTest test = (PtrBalsaTest) tests->data;

            if (doesNeedSeparator)
                UpdateMakefileTable_AddSeparator (table, row++, 1, 25, 3);
            else
                doesNeedSeparator = 1;
            row = UpdateMakefileTable_AddTest (table, row, test);
        }
    }

    return row;
}

int UpdateMakefileTable_Implementations (GtkTable * table, int row)
{                               // Fill the test entries
    GList *files;
    GtkWidget *newLabel;        // *newButton;
    int doesNeedSeparator = 0;

    UpdateMakefileTable_AddSeparator (table, row++, 3, 2, 10);

    newLabel = gtk_label_new ("        Implementations");
    gtk_table_attach (table, newLabel, 0, 1, row, row + 1, //left,right,top,bottom attaches
      GTK_FILL | GTK_EXPAND,    //xoptions
      GTK_FILL,                 //yoptions
      0, 0);                    //x and y-padding
    gtk_widget_show (newLabel);

    row++;

    for (files = CurrentBalsaProject->files; files; files = files->next)
    {
        PtrBalsaFile file = (PtrBalsaFile) files->data;
        GList *impls = file->info.balsaFile.implementations;

        for (; impls; impls = impls->next)
        {
            PtrImplementation impl = (PtrImplementation) impls->data;

            if (doesNeedSeparator)
                UpdateMakefileTable_AddSeparator (table, row++, 1, 25, 3);
            else
                doesNeedSeparator = 1;
            row = UpdateMakefileTable_AddImplementation (table, row, impl);
        }
    }

    return row;
}

int UpdateMakefileTable_Files (GtkTable * table, int row)
{                               // Fill the files entries
    GtkWidget *newLabel;        // *newButton;

    UpdateMakefileTable_AddSeparator (table, row++, 3, 2, 10);

    newLabel = gtk_label_new ("        Files");
    gtk_table_attach (table, newLabel, 0, 1, row, row + 1, //left,right,top,bottom attaches
      GTK_FILL | GTK_EXPAND,    //xoptions
      GTK_FILL,                 //yoptions
      0, 0);                    //x and y-padding
    row++;
    gtk_widget_show (newLabel);

    {
        GList *files;

        for (files = CurrentBalsaProject->files; files; files = files->next)
        {
            PtrBalsaFile file = files->data;

            row = UpdateMakefileTable_AddFile (table, row, file);
            if (files->next)
                UpdateMakefileTable_AddSeparator (table, row++, 1, 25, 3);
        }
    }
    return row;
}

int UpdateMakefileTable_Others (GtkTable * table, int row)
{                               // Fill the files entries
    GtkWidget *newLabel;        // *newButton;

    UpdateMakefileTable_AddSeparator (table, row++, 3, 2, 10);

    newLabel = gtk_label_new ("        Others");
    gtk_table_attach (table, newLabel, 0, 1, row, row + 1, //left,right,top,bottom attaches
      GTK_FILL | GTK_EXPAND,    //xoptions
      GTK_FILL,                 //yoptions
      0, 0);                    //x and y-padding
    row++;
    gtk_widget_show (newLabel);

    // Row "Make Clean"
    UpdateMakefileTable_AddRow (table, row++,
      g_strdup_printf ("  Clean intermediate files"), NULL, NULL, NULL, "Make Clean", (GtkSignalFunc) MakefileTable_MakeCallback, "clean");

    // Row "Make Very Clean"
    UpdateMakefileTable_AddRow (table, row++,
      g_strdup_printf
      ("  Clean intermediate and\n    test-harness files"), NULL, NULL, NULL, "Make Very-Clean", (GtkSignalFunc) MakefileTable_MakeCallback, "very-clean");

    return row;
}

void FillInfosAboutCurrentSelection (void)
{
    PtrBalsaProjectEntry entry = ProjectFileViewSelectedEntry;

    CurrentSelection_Type = 0;
    CurrentSelection_Name = 0;

    if (entry == NULL)
        return;

    switch (entry->nature)
    {
    case BalsaProjectEntry_File:
        CurrentSelection_Type = BalsaProjectEntry_File;
        CurrentSelection_Name = BALSA_FILE (entry->data)->name;
        break;

    case BalsaProjectEntry_Procedure:
        CurrentSelection_Type = BalsaProjectEntry_File;
        CurrentSelection_Name = BALSA_PROCEDURE (entry->data)->associated_filename;
        break;

    case BalsaProjectEntry_TestComponent: /* edit the whole test fixture */
        {
            GtkCTree *tree = GTK_CTREE (LookupWidget (MainWindow, "TreeFilesView"));
            GtkCTreeNode *treeNode = GTK_CTREE_NODE (gtk_object_get_data (GTK_OBJECT (tree), "SelectedNode"));
            GtkCTreeNode *parentNode = GTK_CTREE_ROW (treeNode)->parent;

            entry = BALSA_PROJECT_ENTRY (gtk_ctree_node_get_row_data (tree, parentNode));
        }
        // continue on next case.
    case BalsaProjectEntry_Test:
        CurrentSelection_Type = BalsaProjectEntry_Test;
        CurrentSelection_Name = BALSA_TEST (entry->data)->name;
        break;
    default:
        break;
    }
}

void UpdateMakefileTable (void)
{
    GtkWidget *table = gtk_object_get_data (GTK_OBJECT (MainWindow), "MakefileTable");
    int row = 0;

    if (table)
        gtk_widget_destroy (table);

    table = gtk_table_new (3, 10, FALSE);
    gtk_widget_show (table);

    gtk_container_add (GTK_CONTAINER (LookupWidget (MainWindow, "ViewportForMakefileTable")), table);
    gtk_object_set_data (MainWindowObject, "MakefileTable", table);

    FillInfosAboutCurrentSelection ();

    row = UpdateMakefileTable_Tests (GTK_TABLE (table), row);
    row = UpdateMakefileTable_Implementations (GTK_TABLE (table), row);
    row = UpdateMakefileTable_Files (GTK_TABLE (table), row);
    row = UpdateMakefileTable_Others (GTK_TABLE (table), row);
}

void OnProjectNotebook_SwitchPage (GtkNotebook * notebook, GtkNotebookPage * page, gint page_num)
{
    if (page_num == 2)
        UpdateMakefileTable ();
}
