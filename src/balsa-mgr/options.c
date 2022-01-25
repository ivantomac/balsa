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

	`options.c'
	Global options/objects management

*/

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gtk/gtk.h>
#include <stdlib.h>
#include <stdio.h>
#include <glib.h>
#include <string.h>
#include "options.h"
#include "main.h"
#include "support.h"

char *Options_BalsaHome = NULL;
char *Options_TmpDir = NULL;
GList *Options_BalsaHomeList = NULL;
char *Options_Editor = NULL;
char *Options_PrintCommand = NULL;
char *Options_PSViewer = NULL;

extern GtkWidget *RecentProjectsMenu;
extern void RecentProjectsHistory_MouseClick (gchar * string);
extern void RecentProjectsHistory_ClearMenu (gchar * string);

/* StringArrayToList : convert a string array into a list of strings */
GList *StringArrayToList (char **array)
{
    GList *list = NULL;
    char **arrayIter = array;

    while (*arrayIter)
    {
        list = g_list_append (list, (gpointer) * arrayIter);
        arrayIter++;
    }
    return list;
}

/* StringListToArray : as StringArrayToList but the other way around */
char **StringListToArray (GList * list)
{
    GList *listIter = list;
    char **array = (char **) g_malloc (sizeof (char *) * (1 + g_list_length (list)));
    char **arrayIter = array;

    while (listIter)
    {
        *arrayIter = (char *) listIter->data;
        arrayIter++;
        listIter = listIter->next;
    }
    *arrayIter = NULL;
    return array;
}

/* InitOptions : initialise global objects */
void InitOptions (void)
{
    char *editor = getenv ("EDITOR");
    char **splitImportPath;

    Options_BalsaHome = getenv ("BALSAHOME");
    if (Options_BalsaHome)
    {
        splitImportPath = g_strsplit (Options_BalsaHome, ":", 1024);
        Options_BalsaHomeList = StringArrayToList (splitImportPath);
        g_free (splitImportPath);
    } else
        Options_BalsaHomeList = NULL;

    Options_TmpDir = "/tmp";

    Options_Editor = (editor && *editor ? editor : "emacs");

    Options_PrintCommand = "lpr";

    Options_PSViewer = "gv";
}

void UpdateRecentProjectsHistoryInMenu (void)
{
    GtkWidget *MenuItemWidget = LookupWidget (MainWindow, "ProjectMenu_OpenRecent");
    GtkWidget *MenuWidget = gtk_menu_new ();
    GSList *ptr = EnvironmentOptions.projectsHistory;

    RecentProjectsMenu = MenuWidget;

    /* Remove the previous submenu containing Recent Projects */
    gtk_menu_item_remove_submenu (GTK_MENU_ITEM (MenuItemWidget));

    /* Create the new one */
    gtk_menu_item_set_submenu (GTK_MENU_ITEM (MenuItemWidget), MenuWidget);
    gtk_widget_show (MenuWidget);

    while (ptr)
    {
        GtkWidget *tmp = gtk_menu_item_new_with_label (g_strdup ((char *) ptr->data));

        gtk_menu_prepend (GTK_MENU (MenuWidget), tmp);
        /* Attach the callback functions to the activate signal */
        gtk_signal_connect_object (GTK_OBJECT (tmp), "activate",
          GTK_SIGNAL_FUNC (RecentProjectsHistory_MouseClick), (gpointer) g_strdup ((char *) ptr->data));
        gtk_widget_show (tmp);
        ptr = ptr->next;
    }

    // Add the "Clear Menu" item
    {
        GtkWidget *tmp = gtk_menu_item_new ();

        gtk_menu_append (GTK_MENU (MenuWidget), tmp);
        gtk_widget_show (tmp);

        tmp = gtk_menu_item_new_with_label ("Clear Menu");
        gtk_menu_append (GTK_MENU (MenuWidget), tmp);
        gtk_widget_show (tmp);
        /* Attach the callback functions to the activate signal */
        gtk_signal_connect_object (GTK_OBJECT (tmp), "activate", GTK_SIGNAL_FUNC (RecentProjectsHistory_ClearMenu), NULL);
    }
}

void ReadEnvironmentOptions (void)
{
    FILE *f;
    const char *homedir = g_get_home_dir ();
    char filename[] = ".balsa-mgr";
    char *PathFilename = (char *) g_malloc ((homedir ? strlen (homedir) : 0) + strlen (filename) + 2);

    /* Initialization of the structure */
    EnvironmentOptions.BalsaHome = NULL;
    EnvironmentOptions.TmpDir = "/tmp";
    EnvironmentOptions.editor = NULL;
    EnvironmentOptions.PSViewer = NULL;
    EnvironmentOptions.projectsHistoryMaxSize = 4;
    //    EnvironmentOptions.pathsAbsoluteRelativeOption = 3;
    EnvironmentOptions.projectsHistory = NULL;
    EnvironmentOptions.tools = NULL;
    EnvironmentOptions.newProjectTemplate = NULL;

    /* Read the file */
    if (homedir)
        sprintf (PathFilename, "%s/%s", homedir, filename);
    else
        strcpy (PathFilename, filename);

    f = fopen (PathFilename, "r");
    if (f)
    {
        char buf[1000];
        int size;

        while (!feof (f))
        {
            if (!fgets (buf, 1000, f))
                continue;
            size = strlen (buf);
            if (size < 2)
                continue;
            buf[--size] = 0;    // remove CR at the end of the line
            if (!strncmp (buf, "BalsaHome=", 10))
            {
                EnvironmentOptions.BalsaHome = (char *) g_malloc (size - 10 + 1);
                strcpy (EnvironmentOptions.BalsaHome, buf + 10);
            } else if (!strncmp (buf, "TmpDir=", 7))
            {
                EnvironmentOptions.TmpDir = (char *) g_malloc (size - 7 + 1);
                strcpy (EnvironmentOptions.TmpDir, buf + 7);
            } else if (!strncmp (buf, "Editor=", 7))
            {
                EnvironmentOptions.editor = (char *) g_malloc (size - 7 + 1);
                strcpy (EnvironmentOptions.editor, buf + 7);
            } else if (!strncmp (buf, "PrintCommand=", 13))
            {
                EnvironmentOptions.PrintCommand = (char *) g_malloc (size - 13 + 1);
                strcpy (EnvironmentOptions.PrintCommand, buf + 13);
            } else if (!strncmp (buf, "PSViewer=", 9))
            {
                EnvironmentOptions.PSViewer = (char *) g_malloc (size - 9 + 1);
                strcpy (EnvironmentOptions.PSViewer, buf + 9);
            } else if (!strncmp (buf, "ProjectsHistoryMaxSize=", 23))
            {
                EnvironmentOptions.projectsHistoryMaxSize = atoi (buf + 23);
            } else if (!strncmp (buf, "RecentProject=", 14))
            {
                char *tmp = (char *) g_malloc (size - 14 + 1);

                strcpy (tmp, buf + 14);
                EnvironmentOptions.projectsHistory = g_slist_append (EnvironmentOptions.projectsHistory, tmp);
            } else if (!strncmp (buf, "Tools=", 6))
            {
                char *tmp = (char *) g_malloc (size - 6 + 1);

                strcpy (tmp, buf + 6);
                EnvironmentOptions.tools = g_slist_append (EnvironmentOptions.tools, tmp);
            } else if (!strncmp (buf, "NewProjectTemplate=", 19))
            {
                char *tmp = (char *) g_malloc (size - 19 + 1);

                strcpy (tmp, buf + 19);
                EnvironmentOptions.newProjectTemplate = g_slist_append (EnvironmentOptions.newProjectTemplate, tmp);
            }
        }

        fclose (f);
    }
    UpdateRecentProjectsHistoryInMenu ();
}

void WriteEnvironmentOptions (void)
{
    FILE *f;
    const char *homedir = g_get_home_dir ();
    char filename[] = ".balsa-mgr";
    char *PathFilename = (char *) g_malloc ((homedir ? strlen (homedir) : 0) + strlen (filename) + 2);

    if (homedir)
        sprintf (PathFilename, "%s/%s", homedir, filename);
    else
        strcpy (PathFilename, filename);

    f = fopen (PathFilename, "w");
    if (f)
    {
        GSList *ptr;

        // int counter;

        fprintf (f, "BalsaHome=%s\n" "TmpDir=%s\n" "Editor=%s\n" "PrintCommand=%s\n" "PSViewer=%s\n" "ProjectsHistoryMaxSize=%d\n",
          //             "PathsAbsoluteRelativeOption=%d\n",
          (EnvironmentOptions.
            BalsaHome ? EnvironmentOptions.BalsaHome : ""),
          (EnvironmentOptions.TmpDir ? EnvironmentOptions.
            TmpDir : "/tmp"),
          (EnvironmentOptions.editor ? EnvironmentOptions.
            editor : ""),
          (EnvironmentOptions.
            PrintCommand ? EnvironmentOptions.
            PrintCommand : ""), (EnvironmentOptions.PSViewer ? EnvironmentOptions.PSViewer : ""), EnvironmentOptions.projectsHistoryMaxSize
          //             EnvironmentOptions.pathsAbsoluteRelativeOption
          );

        /* truncate the list to the right size */
        while (g_slist_length (EnvironmentOptions.projectsHistory) > (unsigned int) EnvironmentOptions.projectsHistoryMaxSize)
            EnvironmentOptions.projectsHistory = g_slist_remove_link (EnvironmentOptions.projectsHistory,
              /*g_slist_last */
              (EnvironmentOptions.projectsHistory));

        ptr = EnvironmentOptions.projectsHistory;
        while (ptr)
        {
            fprintf (f, "RecentProject=%s\n", (char *) ptr->data);
            ptr = ptr->next;
        }

        ptr = EnvironmentOptions.tools;
        while (ptr)
        {
            fprintf (f, "Tools=%s\n", (char *) ptr->data);
            ptr = ptr->next;
        }

        ptr = EnvironmentOptions.newProjectTemplate;
        while (ptr)
        {
            fprintf (f, "NewProjectTemplate=%s\n", (char *) ptr->data);
            ptr = ptr->next;
        }

        fclose (f);
    }
    UpdateRecentProjectsHistoryInMenu ();
}

char *EnvironmentOptions_GetBalsaBinDirectory (void)
{
    char *balsaHome;

    if (EnvironmentOptions.BalsaHome && *EnvironmentOptions.BalsaHome)
        balsaHome = EnvironmentOptions.BalsaHome;
    else
        balsaHome = (char *) getenv ("BALSAHOME");

    if (balsaHome && *balsaHome)
        if (balsaHome[strlen (balsaHome) - 1] == '/')
            return g_strdup_printf ("%sbin/", balsaHome);
        else
            return g_strdup_printf ("%s/bin/", balsaHome);
    else
        return "";
}
