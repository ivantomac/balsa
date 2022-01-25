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

#include <stdlib.h>
#include <gtk/gtk.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>
#include <breeze/libbreeze.h>

#include "interface.h"
#include "main.h"
#include "mainwindow2.h"
#include "structure.h"
#include "simulation.h"
#include "simtrace.h"
#include "listview.h"
#include "graphview.h"
#include "sourceview.h"
#include "selectionview.h"
#include "core.h"
#include "graphviewGUI.h"

void InitMainWindow2 (void)
{
    // Initialise correctly every checked menu item group
    gtk_check_menu_item_set_active (GTK_CHECK_MENU_ITEM (gtk_object_get_data (GTK_OBJECT (MainWindow2), "menuTraceAllChannels")), TRUE);
    gtk_check_menu_item_set_active (GTK_CHECK_MENU_ITEM (gtk_object_get_data (GTK_OBJECT (MainWindow2), "MenuGTKWaveViewNamedPortsOnly")), TRUE);
    gtk_check_menu_item_set_active (GTK_CHECK_MENU_ITEM (gtk_object_get_data (GTK_OBJECT (MainWindow2), "menuProcedureStructure")), TRUE);

    /*
       static gboolean already_initialised = FALSE;

       if (already_initialised)
       return;

       already_initialised = TRUE;
     */

    /*
       gboolean active1 = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (gtk_object_get_data (GTK_OBJECT (MainWindow2),
       "ChannelTreeToggleButton")));
       gboolean active2 = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (gtk_object_get_data (GTK_OBJECT (MainWindow2),
       "GraphToggleButton")));
     */

    //    if (active1 || active2)
    //        InitStructure ();

    //    if (active1)
    //        InitCallList ();

    //    if (active2)
    //    InitCallGraph ();

    Core_Init ();

    //    ResetReadTrace (FALSE);

    // test for hhh file
    struct stat st;
    char *hhh_filename = g_strdup_printf ("%s.hhh", projectName);

    if (!stat (hhh_filename, &st))
    {
        GtkWidget *warningButton = GTK_WIDGET (gtk_object_get_data (GTK_OBJECT (MainWindow2), "WarningButton"));

        gtk_widget_show (warningButton);

        GtkTooltips *tooltips = GTK_TOOLTIPS (gtk_object_get_data (GTK_OBJECT (MainWindow2), "tooltips"));
        char *timestr = ctime (&st.st_mtime);
        char *tooltiptext = g_strdup_printf ("The current trace file is from a previous session\ngenerated on %s",
          timestr);

        gtk_tooltips_set_tip (tooltips, warningButton, tooltiptext, NULL);
        g_free (tooltiptext);
    }
    free (hhh_filename);
}

enum TraceType getTraceType (void)
{
    GtkWidget *m1 = (gtk_object_get_data (GTK_OBJECT (MainWindow2), "menuTraceAllChannels"));

    //  GtkWidget *m2 = (gtk_object_get_data (GTK_OBJECT (MainWindow2), "menuTracePortsOnly"));

    if ((GTK_CHECK_MENU_ITEM (m1))->active)
        return TraceAllChans;
    else
        return TracePortsOnly;
}

enum GTKWaveViewType getGTKWaveViewType (void)
{
    GtkWidget *m1 = (gtk_object_get_data (GTK_OBJECT (MainWindow2), "MenuGTKWaveViewAllTracedChannels"));
    GtkWidget *m2 = (gtk_object_get_data (GTK_OBJECT (MainWindow2), "MenuGTKWaveViewPortsOnly"));
    GtkWidget *m3 = (gtk_object_get_data (GTK_OBJECT (MainWindow2), "MenuGTKWaveViewNamedPortsOnly"));

    //  GtkWidget *m4 = (gtk_object_get_data (GTK_OBJECT (MainWindow2), "menuGTKWaveViewNone"));

    if ((GTK_CHECK_MENU_ITEM (m1))->active)
        return GTKWaveViewAllTracedChans;
    else if ((GTK_CHECK_MENU_ITEM (m2))->active)
        return GTKWaveViewPortsOnly;
    else if ((GTK_CHECK_MENU_ITEM (m3))->active)
        return GTKWaveViewNamedPortsOnly;
    else
        return GTKWaveViewNone;
}

static gboolean disableChangeTraceTypeCallbacks = FALSE;
void ChangeTraceType (enum TraceType newTraceType)
{
    if (disableChangeTraceTypeCallbacks)
        return;

    disableChangeTraceTypeCallbacks = TRUE;

    GtkWidget *m1 = (gtk_object_get_data (GTK_OBJECT (MainWindow2), "menuTraceAllChannels"));
    GtkWidget *m2 = (gtk_object_get_data (GTK_OBJECT (MainWindow2), "menuTracePortsOnly"));

    //       GtkWidget *m3 = (gtk_object_get_data (GTK_OBJECT (MainWindow2), "menuTraceNamedPortsOnly"));

    //    GtkWidget *t1 = (gtk_object_get_data (GTK_OBJECT (MainWindow2), "toggleTraceAllChannels"));
    //    GtkWidget *t2 = (gtk_object_get_data (GTK_OBJECT (MainWindow2), "toggleTracePortsOnly"));
    //    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (t1), traceType==TraceAllChans);
    //    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (t2), traceType==TracePortsOnly);

    switch (newTraceType)
    {
    case TraceAllChans:
        gtk_check_menu_item_set_active (GTK_CHECK_MENU_ITEM (m1), TRUE);
        break;
    case TracePortsOnly:
        gtk_check_menu_item_set_active (GTK_CHECK_MENU_ITEM (m2), TRUE);
        break;
    }

    disableChangeTraceTypeCallbacks = FALSE;
    /*
       RefillCallList ();
       RefillCallGraph ();
     */
}

void OnMenuTraceTypeChanged (GtkMenuItem * menuitem, gpointer user_data)
{
    if (disableChangeTraceTypeCallbacks)
        return;

    ChangeTraceType ((enum TraceType) user_data);
}

void OnMenuTraceTypeChanged1 (GtkMenuItem * menuitem, gpointer user_data)
{
    OnMenuTraceTypeChanged (menuitem, (gpointer) 1);
}

void OnMenuTraceTypeChanged2 (GtkMenuItem * menuitem, gpointer user_data)
{
    OnMenuTraceTypeChanged (menuitem, (gpointer) 2);
}

void OnMenuTraceTypeChanged3 (GtkMenuItem * menuitem, gpointer user_data)
{
    OnMenuTraceTypeChanged (menuitem, (gpointer) 3);
}

void OnButtonTraceTypeChanged (GtkButton * button, gpointer user_data)
{
    if (disableChangeTraceTypeCallbacks)
        return;

    if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (button)) == FALSE)
    {
        gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (button), TRUE);
        return;
    }

    ChangeTraceType ((enum TraceType) user_data);
}

void KillAndQuit (GtkButton * button, gpointer user_data)
{
    KillThemAll ();
    gtk_main_quit ();
}

void Quit (GtkButton * button, gpointer user_data)
{
    gtk_main_quit ();
}

gboolean OnQuit (GtkWidget * widget, GdkEvent * event, gpointer user_data)
{
    if (simulationIsRunning || gtkwaveIsRunning)
    {
        GtkWidget *ProcessStillRunningDialogue = create_ProcessStillRunning ();

        gtk_widget_show (ProcessStillRunningDialogue);
    } else
        gtk_main_quit ();

    return TRUE;
}

void OnFileMenu_Quit (GtkMenuItem * menuitem, gpointer user_data)
{
    OnQuit (0, 0, 0);
}

/***************************************************************************/

GtkWidget *OnFileMenu_SaveEnvironment_file_selection_box = NULL;

void OnFileMenu_SaveEnvironment_OKpressed (GtkFileSelection * file_selection, gpointer data)
{
    const char *filename = gtk_file_selection_get_filename (GTK_FILE_SELECTION (OnFileMenu_SaveEnvironment_file_selection_box));

    SaveGraph (filename);

    OnFileMenu_SaveEnvironment_file_selection_box = NULL;
}

void OnFileMenu_SaveEnvironment (GtkMenuItem * menuitem, gpointer user_data)
{
    if (OnFileMenu_SaveEnvironment_file_selection_box)
        return;

 /*-- Create the selector widget --*/
    OnFileMenu_SaveEnvironment_file_selection_box = gtk_file_selection_new ("Please select a file for editing.");

 /*-- Set default file name --*/
    gtk_file_selection_set_filename (GTK_FILE_SELECTION (OnFileMenu_SaveEnvironment_file_selection_box), g_strdup_printf ("%s_saved.env",
        projectName));

 /*-- Link the ok button to the store_filename function --*/
    gtk_signal_connect (GTK_OBJECT (GTK_FILE_SELECTION (OnFileMenu_SaveEnvironment_file_selection_box)->ok_button),
      "clicked", GTK_SIGNAL_FUNC (OnFileMenu_SaveEnvironment_OKpressed), NULL);

 /*-- Destroy the dialog box when the user clicks the ok_button --*/
    gtk_signal_connect_object (GTK_OBJECT (GTK_FILE_SELECTION (OnFileMenu_SaveEnvironment_file_selection_box)->ok_button),
      "clicked", GTK_SIGNAL_FUNC (gtk_widget_destroy), (gpointer) OnFileMenu_SaveEnvironment_file_selection_box);

 /*-- Destroy the dialog box when the user clicks the cancel_button --*/
    gtk_signal_connect_object (GTK_OBJECT (GTK_FILE_SELECTION (OnFileMenu_SaveEnvironment_file_selection_box)->cancel_button),
      "clicked", GTK_SIGNAL_FUNC (gtk_widget_destroy), (gpointer) OnFileMenu_SaveEnvironment_file_selection_box);

    /* Display the file dialog box */
    gtk_widget_show (OnFileMenu_SaveEnvironment_file_selection_box);
}

/***************************************************************************/

GtkWidget *OnFileMenu_LoadEnvironment_file_selection_box = NULL;

void OnFileMenu_LoadEnvironment_OKpressed (GtkFileSelection * file_selection, gpointer data)
{
    layoutFilename = gtk_file_selection_get_filename (GTK_FILE_SELECTION (OnFileMenu_LoadEnvironment_file_selection_box));
    if (access (layoutFilename, R_OK) == 0)
    {
        FullLayoutWithDot ();
        ComputeRealVertexSizeAndPositionRecursively (NULL);
        ZoomFit ();
    } else
        fprintf (stderr, "Error opening %s\n", layoutFilename);

    OnFileMenu_LoadEnvironment_file_selection_box = NULL;
}

void OnFileMenu_LoadEnvironment (GtkMenuItem * menuitem, gpointer user_data)
{
    if (OnFileMenu_LoadEnvironment_file_selection_box)
        return;

 /*-- Create the selector widget --*/
    OnFileMenu_LoadEnvironment_file_selection_box = gtk_file_selection_new ("Please select a file for editing.");

 /*-- Set default file name --*/
    gtk_file_selection_set_filename (GTK_FILE_SELECTION (OnFileMenu_LoadEnvironment_file_selection_box), g_strdup_printf ("%s_saved.env",
        projectName));

 /*-- Link the ok button to the store_filename function --*/
    gtk_signal_connect (GTK_OBJECT (GTK_FILE_SELECTION (OnFileMenu_LoadEnvironment_file_selection_box)->ok_button),
      "clicked", GTK_SIGNAL_FUNC (OnFileMenu_LoadEnvironment_OKpressed), NULL);

 /*-- Destroy the dialog box when the user clicks the ok_button --*/
    gtk_signal_connect_object (GTK_OBJECT (GTK_FILE_SELECTION (OnFileMenu_LoadEnvironment_file_selection_box)->ok_button),
      "clicked", GTK_SIGNAL_FUNC (gtk_widget_destroy), (gpointer) OnFileMenu_LoadEnvironment_file_selection_box);

 /*-- Destroy the dialog box when the user clicks the cancel_button --*/
    gtk_signal_connect_object (GTK_OBJECT (GTK_FILE_SELECTION (OnFileMenu_LoadEnvironment_file_selection_box)->cancel_button),
      "clicked", GTK_SIGNAL_FUNC (gtk_widget_destroy), (gpointer) OnFileMenu_LoadEnvironment_file_selection_box);

    /* Display the file dialog box */
    gtk_widget_show (OnFileMenu_LoadEnvironment_file_selection_box);

    // If the graph is not displayed, display it
    {
        GtkToggleButton *graphToggleButton = GTK_TOGGLE_BUTTON (gtk_object_get_data (GTK_OBJECT (MainWindow2), "GraphToggleButton"));

        if (!gtk_toggle_button_get_active (graphToggleButton))
            gtk_toggle_button_set_active (graphToggleButton, TRUE);
    }
}

/***************************************************************************/

/*
gint try_first_read_hhh_file_timer_callback (gpointer data)
{
    char *hhh_filename = g_strdup_printf ("%s.hhh", projectName);
    FILE *f = fopen (hhh_filename, "r");

    free (hhh_filename);
    if (!f)
        return FALSE;
    fclose (f);
    if (!pixmap)
        return TRUE;
    StartReadTraceTimer1 ();
    return FALSE;
}
*/

void on_handlebox_toggle_clicked (GtkButton * button, gpointer user_data)
{
    int choice = GPOINTER_TO_INT (user_data);

    switch (choice)
    {
        /*
           case 1:
           // gtk bug fix
           //     gtk_widget_set_sensitive (GTK_WIDGET (button), FALSE);
           if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (button)) == TRUE)
           {
           GtkToggleButton *graphToggleButton =
           GTK_TOGGLE_BUTTON (gtk_object_get_data (GTK_OBJECT (MainWindow2), "GraphToggleButton"));

           if (!gtk_toggle_button_get_active (graphToggleButton))
           {
           gtk_toggle_button_set_active (graphToggleButton, TRUE);

           ListView_ToggleActivate (TRUE);
           SelectionView_ToggleActivate (TRUE);
           return;
           }
           }
           // END gtk bug fix
           ListView_ToggleActivate (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (button)));
           break;
         */
    case 2:
        GraphView_ToggleActivate (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (button)));
        SelectionView_ToggleActivate (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (button)));

        GtkWidget *mainHBox = GTK_WIDGET (gtk_object_get_data (GTK_OBJECT (MainWindow2), "MainHBox"));

        if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (button)))
        {
            gtk_widget_show (mainHBox);
            gtk_window_set_default_size (GTK_WINDOW (MainWindow2), 600, 500);
        } else
        {
            gtk_widget_hide (mainHBox);
            gtk_window_set_default_size (GTK_WINDOW (MainWindow2), 601, 50);
        }
        break;
    case 3:
        SourceView_ToggleActivate (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (button)));

        if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (button)))
            gtk_widget_show (SourceViewWindow);
        else
            gtk_widget_hide (SourceViewWindow);
        break;
    }
}

void on_WarningButton_clicked (GtkButton * button, gpointer user_data)
{
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (button), FALSE);
}
