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

	`projopts.c'
	Project options dialogue handling

*/

#include <unistd.h>
#include <sys/stat.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <gdk/gdkkeysyms.h>
#include "projopts.h"
#include "miscgtk.h"
#include "paths.h"
#include "file.h"
#include "workspace.h"
#include "utils.h"
#include "project.h"
#include "options.h"
#include "support.h"

extern int NewProjectCount;
gboolean ProjectOptionsToggleActive = TRUE;

static gboolean DefinitionsEntryDisabled = 0;

void OnProjectFileImport_ConvertRelAbs (GtkObject * dialogue)
{
    GtkWidget *direntry = LookupWidget (workSpace.projectOptionsDialogue->dialogue,
        "ProjectDirectoryEntry");
    const char *directoryRef = gtk_entry_get_text (GTK_ENTRY (direntry));

    GtkWidget *entry = LookupWidget (GTK_WIDGET (dialogue), "ProjectFileImportPathEntry");
    const char *directory = gtk_entry_get_text (GTK_ENTRY (entry));

    if (directory && *directory)
    {
        if (*directory == '/')
            gtk_entry_set_text (GTK_ENTRY (entry), ConvertToRelativePath (directory, directoryRef));
        else
            gtk_entry_set_text (GTK_ENTRY (entry), ConvertToAbsolutePath (directory, directoryRef));
    }
}

void OnProjectFileImportPathUpButton (GtkObject * dialogue)
{
    GtkWidget *list = LookupWidget (GTK_WIDGET (dialogue), "ProjectFileImportPathList");
    int selectedRowNo = GPOINTER_TO_INT (gtk_object_get_data (GTK_OBJECT (dialogue), "SelectedPath"));

    if (selectedRowNo != PROJ_OPTS_NO_SELECTION)
    {
        gtk_object_set_data (GTK_OBJECT (dialogue), "PathChanged", (gpointer) TRUE);
        gtk_clist_unselect_row (GTK_CLIST (list), selectedRowNo, 0);
        gtk_clist_row_move (GTK_CLIST (list), selectedRowNo, selectedRowNo - 1);
        gtk_clist_select_row (GTK_CLIST (list), selectedRowNo - 1, 0);
    }
}

void OnProjectFileImportPathDownButton (GtkObject * dialogue)
{
    GtkWidget *list = LookupWidget (GTK_WIDGET (dialogue), "ProjectFileImportPathList");
    int selectedRowNo = GPOINTER_TO_INT (gtk_object_get_data (GTK_OBJECT (dialogue), "SelectedPath"));

    gtk_object_set_data (GTK_OBJECT (dialogue), "PathChanged", (gpointer) TRUE);
    gtk_clist_unselect_row (GTK_CLIST (list), selectedRowNo, 0);
    gtk_clist_row_move (GTK_CLIST (list), selectedRowNo, selectedRowNo + 1);
    gtk_clist_select_row (GTK_CLIST (list), selectedRowNo + 1, 0);
}

void OnProjectFileImportPathNewButton (GtkObject * dialogue)
{
    GtkWidget *list = LookupWidget (GTK_WIDGET (dialogue), "ProjectFileImportPathList");
    GtkWidget *entry = LookupWidget (GTK_WIDGET (dialogue), "ProjectFileImportPathEntry");
    int selectedRowNo = GPOINTER_TO_INT (gtk_object_get_data (GTK_OBJECT (dialogue), "SelectedPath"));

    if (selectedRowNo == PROJ_OPTS_NO_SELECTION)
    {
        char *entryContents = (char *) gtk_entry_get_text (GTK_ENTRY (entry));

        if (!*entryContents)
            entryContents = ".";
        gtk_clist_insert (GTK_CLIST (list), 0, &entryContents);
        gtk_clist_select_row (GTK_CLIST (list), 0, 0);
    } else
    {
        char *path = ".";

        gtk_clist_insert (GTK_CLIST (list), selectedRowNo + 1, &path);
        gtk_clist_select_row (GTK_CLIST (list), selectedRowNo + 1, 0);
    }
    gtk_object_set_data (GTK_OBJECT (dialogue), "PathChanged", (gpointer) TRUE);
}

void OnProjectFileImportPathDeleteButton (GtkObject * dialogue)
{
    GtkWidget *list = LookupWidget (GTK_WIDGET (dialogue), "ProjectFileImportPathList");
    int selectedRowNo = GPOINTER_TO_INT (gtk_object_get_data (GTK_OBJECT (dialogue), "SelectedPath"));

    gtk_clist_remove (GTK_CLIST (list), selectedRowNo);
    gtk_clist_select_row (GTK_CLIST (list), selectedRowNo, 0);
}

void OnProjectOptionsDefinitionsUpButton (GtkObject * dialogue)
{
    GtkWidget *list = LookupWidget (GTK_WIDGET (dialogue), "DefinesList");
    int selectedRowNo = GPOINTER_TO_INT (gtk_object_get_data (GTK_OBJECT (dialogue), "SelectedDefine"));

    if (selectedRowNo != PROJ_OPTS_NO_SELECTION)
    {
        gtk_object_set_data (GTK_OBJECT (dialogue), "DefinesChanged", (gpointer) TRUE);
        gtk_clist_unselect_row (GTK_CLIST (list), selectedRowNo, 0);
        gtk_clist_row_move (GTK_CLIST (list), selectedRowNo, selectedRowNo - 1);
        gtk_clist_select_row (GTK_CLIST (list), selectedRowNo - 1, 0);
    }
}

void OnProjectOptionsDefinitionsDownButton (GtkObject * dialogue)
{
    GtkWidget *list = LookupWidget (GTK_WIDGET (dialogue), "DefinesList");
    int selectedRowNo = GPOINTER_TO_INT (gtk_object_get_data (GTK_OBJECT (dialogue), "SelectedDefine"));

    gtk_object_set_data (GTK_OBJECT (dialogue), "DefinesChanged", (gpointer) TRUE);
    gtk_clist_unselect_row (GTK_CLIST (list), selectedRowNo, 0);
    gtk_clist_row_move (GTK_CLIST (list), selectedRowNo, selectedRowNo + 1);
    gtk_clist_select_row (GTK_CLIST (list), selectedRowNo + 1, 0);
}

void OnProjectOptionsDefinitionsNewButton (GtkObject * dialogue)
{
    GtkWidget *list = LookupWidget (GTK_WIDGET (dialogue), "DefinesList");
    GtkWidget *nameEntry = LookupWidget (GTK_WIDGET (dialogue), "DefineNameEntry");
    GtkWidget *valueEntry = LookupWidget (GTK_WIDGET (dialogue), "DefineValueEntry");
    int selectedRowNo = GPOINTER_TO_INT (gtk_object_get_data (GTK_OBJECT (dialogue), "SelectedDefine"));

    if (selectedRowNo == PROJ_OPTS_NO_SELECTION)
    {
        char *strings[2];

        strings[0] = (char *) gtk_entry_get_text (GTK_ENTRY (nameEntry));
        strings[1] = (char *) gtk_entry_get_text (GTK_ENTRY (valueEntry));

        if (!*strings[0])
            strings[0] = "Name";

        gtk_clist_insert (GTK_CLIST (list), 0, strings);
        gtk_clist_select_row (GTK_CLIST (list), 0, 0);
    } else
    {
        const char *textName, *textValue;

        textName = gtk_entry_get_text (GTK_ENTRY (nameEntry));
        textValue = gtk_entry_get_text (GTK_ENTRY (valueEntry));
        if (*textName || *textValue)
        {
            char *strings[2];

            strings[0] = "Name";
            strings[1] = "";

            gtk_clist_insert (GTK_CLIST (list), selectedRowNo + 1, strings);
            gtk_clist_select_row (GTK_CLIST (list), selectedRowNo + 1, 0);
        }
    }
    gtk_object_set_data (GTK_OBJECT (dialogue), "DefinesChanged", (gpointer) TRUE);

    /* Make name selected so we can directly overtype it */
    gtk_entry_select_region (GTK_ENTRY (nameEntry), 0, -1);
    gtk_widget_grab_focus (GTK_WIDGET (nameEntry));
}

void OnProjectOptionsDefinitionsDeleteButton (GtkObject * dialogue)
{
    GtkWidget *list = LookupWidget (GTK_WIDGET (dialogue), "DefinesList");
    int selectedRowNo = GPOINTER_TO_INT (gtk_object_get_data (GTK_OBJECT (dialogue), "SelectedDefine"));

    gtk_clist_remove (GTK_CLIST (list), selectedRowNo);
    gtk_clist_select_row (GTK_CLIST (list), selectedRowNo, 0);
}

void OnProjectOptionsDefinitionsSelectRow (GtkCList * list, gint row, gint column, GdkEvent * event)
{
    GtkWidget *dialogue = gtk_widget_get_toplevel (GTK_WIDGET (list));
    char *text;
    GtkWidget *nameEntry = LookupWidget (dialogue, "DefineNameEntry");
    GtkWidget *valueEntry = LookupWidget (dialogue, "DefineValueEntry");

    gtk_widget_set_sensitive (LookupWidget (dialogue, "DefinesUpButton"), (row == 0 ? FALSE : TRUE));
    gtk_widget_set_sensitive (LookupWidget (dialogue, "DefinesDownButton"), (row == list->rows - 1 ? FALSE : TRUE));
    gtk_widget_set_sensitive (LookupWidget (dialogue, "DefinesDeleteButton"), TRUE);

    gtk_object_set_data (GTK_OBJECT (dialogue), "SelectedDefine", GINT_TO_POINTER (row));

    gtk_clist_get_text (list, row, 0, &text);
    gtk_entry_set_text (GTK_ENTRY (nameEntry), text);
    gtk_clist_get_text (list, row, 1, &text);
    gtk_entry_set_text (GTK_ENTRY (valueEntry), text);
}

void OnProjectOptionsDefinitionsUnselectRow (GtkCList * list, gint row, gint column, GdkEvent * event)
{
    GtkWidget *dialogue = gtk_widget_get_toplevel (GTK_WIDGET (list));

    GtkWidget *nameEntry = LookupWidget (dialogue, "DefineNameEntry");
    GtkWidget *valueEntry = LookupWidget (dialogue, "DefineValueEntry");

    gtk_widget_set_sensitive (LookupWidget (dialogue, "DefinesUpButton"), FALSE);
    gtk_widget_set_sensitive (LookupWidget (dialogue, "DefinesDownButton"), FALSE);
    gtk_widget_set_sensitive (LookupWidget (dialogue, "DefinesDeleteButton"), FALSE);

    gtk_object_set_data (GTK_OBJECT (dialogue), "SelectedDefine", GINT_TO_POINTER (PROJ_OPTS_NO_SELECTION));

    DefinitionsEntryDisabled = 1;
    gtk_entry_set_text (GTK_ENTRY (nameEntry), "");
    gtk_entry_set_text (GTK_ENTRY (valueEntry), "");
    DefinitionsEntryDisabled = 0;
}

void OnProjectOptionsDialogueDefinitionsNameEntryChanged (GtkEditable * editable)
{
    GtkWidget *dialogue = gtk_widget_get_toplevel (GTK_WIDGET (editable));
    GtkCList *list = GTK_CLIST (LookupWidget (dialogue, "DefinesList"));
    int selectedRowNo = GPOINTER_TO_INT (gtk_object_get_data (GTK_OBJECT (dialogue), "SelectedDefine"));

    if (DefinitionsEntryDisabled)
        return;

    if (selectedRowNo == PROJ_OPTS_NO_SELECTION)
    {
        OnProjectOptionsDefinitionsNewButton (GTK_OBJECT (dialogue));
        selectedRowNo = GPOINTER_TO_INT (gtk_object_get_data (GTK_OBJECT (dialogue), "SelectedDefine"));
    }
    gtk_clist_set_text (list, selectedRowNo, 0, gtk_entry_get_text (GTK_ENTRY (editable)));
    gtk_object_set_data (GTK_OBJECT (dialogue), "PathChanged", GINT_TO_POINTER (TRUE));
}

void OnProjectOptionsDialogueDefinitionsValueEntryChanged (GtkEditable * editable)
{
    GtkWidget *dialogue = gtk_widget_get_toplevel (GTK_WIDGET (editable));
    GtkCList *list = GTK_CLIST (LookupWidget (dialogue, "DefinesList"));
    int selectedRowNo = GPOINTER_TO_INT (gtk_object_get_data (GTK_OBJECT (dialogue), "SelectedDefine"));

    if (DefinitionsEntryDisabled)
        return;

    if (selectedRowNo == PROJ_OPTS_NO_SELECTION)
    {
        OnProjectOptionsDefinitionsNewButton (GTK_OBJECT (dialogue));
        selectedRowNo = GPOINTER_TO_INT (gtk_object_get_data (GTK_OBJECT (dialogue), "SelectedDefine"));
    }
    gtk_clist_set_text (list, selectedRowNo, 1, gtk_entry_get_text (GTK_ENTRY (editable)));
    gtk_object_set_data (GTK_OBJECT (dialogue), "DefinesChanged", GINT_TO_POINTER (TRUE));
}

void OnProjectOptionsDialogueOK (GtkObject * dialogue)
{
    const char *project_name;
    const char *project_directory;
    const char *project_BALSACOPTS;
    const char *project_BREEZESIMOPTS;
    const char *project_BREEZELINKOPTS;

    enum SimulationSystem project_simulationSystem;

    project_name = gtk_entry_get_text (GTK_ENTRY (LookupWidget (GTK_WIDGET (dialogue), "ProjectNameEntry")));
    project_directory = gtk_entry_get_text (GTK_ENTRY (LookupWidget (GTK_WIDGET (dialogue), "ProjectDirectoryEntry")));
    project_BALSACOPTS = gtk_entry_get_text (
    	GTK_ENTRY (LookupWidget (GTK_WIDGET (dialogue), "ProjectOptionsBALSACOPTSentry")));
    project_BREEZESIMOPTS = gtk_entry_get_text (
    	GTK_ENTRY (LookupWidget (GTK_WIDGET (dialogue), "ProjectOptionsBREEZESIMOPTSentry")));
    project_BREEZELINKOPTS = gtk_entry_get_text (
    	GTK_ENTRY (LookupWidget (GTK_WIDGET (dialogue), "ProjectOptionsBREEZELINKOPTSentry")));

    {
        int toggle_BreezeDirectSimulation = gtk_toggle_button_get_active (
        	GTK_TOGGLE_BUTTON (LookupWidget (GTK_WIDGET (dialogue), "ProjectOptions_BreezeDirectSimulation")));

        if (toggle_BreezeDirectSimulation)
            project_simulationSystem = breezesim;
        else
        {
            printfConsole ("Warning: Bad simulation system choice.\n");
            project_simulationSystem = breezesim;
        }
    }

    /* Check the project directory is OK */
    {
        struct stat dirDetails;
        gboolean badDir = FALSE;

        errno = 0;
        stat (project_directory, &dirDetails);

        if (errno == ENOENT)
        {                       /* File doesn't exist */
            printfConsole ("Project directory must already exist");
            badDir = TRUE;
        }
        /* All other errors, just moan about perms */
        else if (errno || !S_ISDIR (dirDetails.st_mode) || (dirDetails.st_mode & S_IRWXU) != S_IRWXU)
        {
            printfConsole ("Problem with project directory. Need rwx permissions");
            badDir = TRUE;
        }

        if (badDir)
            return;
    }

    /* Check if we are in the case of "New Project" or "Edit Project Options" */
    if (gtk_object_get_data (GTK_OBJECT (dialogue), "Project"))
    {
        // Project Options
        if (strcmp (CurrentBalsaProject->name, project_name))
        {
            CurrentBalsaProject->name = g_strdup (project_name);
            CurrentBalsaProject->dirty = TRUE;
        }
        if (CurrentBalsaProject->simulationSystem != project_simulationSystem)
        {
            CurrentBalsaProject->simulationSystem = project_simulationSystem;
            CurrentBalsaProject->dirty = TRUE;
            CurrentBalsaProject->dirty_and_need_make_clean = TRUE;
        }
        if (strcmp (CurrentBalsaProject->directory, project_directory))
        {
            CurrentBalsaProject->directory = g_strdup (project_directory);
            CurrentBalsaProject->dirty = TRUE;
            chdir (CurrentBalsaProject->directory);
        }
        if ((CurrentBalsaProject->BALSACOPTS
            && strcmp (CurrentBalsaProject->BALSACOPTS, project_BALSACOPTS)) || (!CurrentBalsaProject->BALSACOPTS && project_BALSACOPTS[0]))
        {
            CurrentBalsaProject->BALSACOPTS = g_strdup (project_BALSACOPTS);
            CurrentBalsaProject->dirty = TRUE;
            CurrentBalsaProject->dirty_and_need_make_clean = TRUE;
        }
        if ((CurrentBalsaProject->BREEZESIMOPTS
            && strcmp (CurrentBalsaProject->BREEZESIMOPTS,
              project_BREEZESIMOPTS)) || (!CurrentBalsaProject->BREEZESIMOPTS && project_BREEZESIMOPTS[0]))
        {
            CurrentBalsaProject->BREEZESIMOPTS = g_strdup (project_BREEZESIMOPTS);
            CurrentBalsaProject->dirty = TRUE;
            CurrentBalsaProject->dirty_and_need_make_clean = TRUE;
        }
        if ((CurrentBalsaProject->BREEZELINKOPTS
            && strcmp (CurrentBalsaProject->BREEZELINKOPTS,
              project_BREEZELINKOPTS)) || (!CurrentBalsaProject->BREEZELINKOPTS && project_BREEZELINKOPTS[0]))
        {
            CurrentBalsaProject->BREEZELINKOPTS = g_strdup (project_BREEZELINKOPTS);
            CurrentBalsaProject->dirty = TRUE;
            CurrentBalsaProject->dirty_and_need_make_clean = TRUE;
        }
        if ((GtkWidget *) dialogue == workSpace.projectOptionsDialogue->dialogue)
            workSpace.projectOptionsDialogue->dialogue = NULL;
    } else
    {
        // New Project
        NewCurrentBalsaProject (project_name, project_directory);
        ShowProjectPane (TRUE);
        CurrentBalsaProject->BALSACOPTS = g_strdup (project_BALSACOPTS);
        CurrentBalsaProject->BREEZESIMOPTS = g_strdup (project_BREEZESIMOPTS);
        CurrentBalsaProject->BREEZELINKOPTS = g_strdup (project_BREEZELINKOPTS);
        NewProjectCount++;
    }

    /* Update project import list */
    if ((gboolean) GPOINTER_TO_INT (gtk_object_get_data (GTK_OBJECT (dialogue), "PathChanged")))
    {
        GtkCList *ImportPathList = GTK_CLIST (gtk_object_get_data (dialogue, "ProjectFileImportPathList"));
        int rowNo;

        // Clear the previous import paths list
        // TODO: free each element
        CurrentBalsaProject->importPath = NULL;

        for (rowNo = ImportPathList->rows - 1; rowNo >= 0; rowNo--)
        {
            char *directory;

            gtk_clist_get_text (ImportPathList, rowNo, 0, &directory);
            AddImportPathToCurrentBalsaProject (directory);
        }

        CurrentBalsaProject->dirty = TRUE;
    }

    /* Update defines list */
    if ((gboolean) GPOINTER_TO_INT (gtk_object_get_data (GTK_OBJECT (dialogue), "DefinesChanged")))
    {
        GtkCList *DefinesList = GTK_CLIST (gtk_object_get_data (dialogue, "DefinesList"));
        int rowNo;

        // Clear the previous defines list
        // TODO: free each element
        CurrentBalsaProject->defines = NULL;

        for (rowNo = DefinesList->rows - 1; rowNo >= 0; rowNo--)
        {
            char *name, *value;

            gtk_clist_get_text (DefinesList, rowNo, 0, &name);
            gtk_clist_get_text (DefinesList, rowNo, 1, &value);

            if (*name)
                AddDefineToCurrentBalsaProject (name, value);
        }

        CurrentBalsaProject->dirty = TRUE;
    }

    /* Close Options Dialog */
    gtk_clist_unselect_all (GTK_CLIST (LookupWidget (GTK_WIDGET (dialogue), "ProjectFileImportPathList")));

    gtk_widget_hide (GTK_WIDGET (dialogue));
    gtk_widget_destroy (GTK_WIDGET (dialogue));

    ShowProjectPane (TRUE);
    UpdateProjectTrees ();
}

void OnProjectOptionsDialogueCancel (GtkObject * dialogue)
{
    gtk_clist_unselect_all (GTK_CLIST (LookupWidget (GTK_WIDGET (dialogue), "ProjectFileImportPathList")));

    workSpace.projectOptionsDialogue->dialogue = NULL;
    //    workSpace.projectOptionsDialogue->dialogue_NewProject = NULL;
    gtk_widget_hide (GTK_WIDGET (dialogue));
    gtk_widget_destroy (GTK_WIDGET (dialogue));

    /* If this is a new project were deciding not to make, free the project */
    if (! gtk_object_get_data (GTK_OBJECT (dialogue), "Project"))
		CloseCurrentBalsaProject ();
}

void OnProjectFileImportPathListSelectChild (GtkCList * list, gint row, gint column, GdkEvent * event)
{
    GtkWidget *dialogue = gtk_widget_get_toplevel (GTK_WIDGET (list));
    char *text;
    GtkWidget *entry = LookupWidget (dialogue, "ProjectFileImportPathEntry");

    gtk_widget_set_sensitive (LookupWidget (dialogue, "ProjectFileImportPathUpButton"), (row == 0 ? FALSE : TRUE));
    gtk_widget_set_sensitive (LookupWidget (dialogue, "ProjectFileImportPathDownButton"), (row == list->rows - 1 ? FALSE : TRUE));
    gtk_widget_set_sensitive (LookupWidget (dialogue, "ProjectFileImportPathDeleteButton"), TRUE);

    gtk_clist_get_text (list, row, 0, &text);
    gtk_object_set_data (GTK_OBJECT (dialogue), "SelectedPath", GINT_TO_POINTER (row));
    gtk_entry_set_text (GTK_ENTRY (entry), text);
}

void OnProjectFileImportPathListUnselectChild (GtkCList * list, gint row, gint column, GdkEvent * event)
{
    GtkWidget *dialogue = gtk_widget_get_toplevel (GTK_WIDGET (list));

    gtk_widget_set_sensitive (LookupWidget (dialogue, "ProjectFileImportPathUpButton"), FALSE);
    gtk_widget_set_sensitive (LookupWidget (dialogue, "ProjectFileImportPathDownButton"), FALSE);
    gtk_widget_set_sensitive (LookupWidget (dialogue, "ProjectFileImportPathDeleteButton"), FALSE);

    gtk_object_set_data (GTK_OBJECT (dialogue), "SelectedPath", GINT_TO_POINTER (PROJ_OPTS_NO_SELECTION));
}

void OnProjectFileImportPathEntryChange (GtkEditable * editable)
{
    GtkWidget *dialogue = gtk_widget_get_toplevel (GTK_WIDGET (editable));
    GtkCList *list = GTK_CLIST (LookupWidget (dialogue, "ProjectFileImportPathList"));
    int selectedRowNo = GPOINTER_TO_INT (gtk_object_get_data (GTK_OBJECT (dialogue), "SelectedPath"));
    const char *text;

    if (selectedRowNo == PROJ_OPTS_NO_SELECTION)
    {
        OnProjectFileImportPathNewButton (GTK_OBJECT (dialogue));
        selectedRowNo = 0;
    }

    text = gtk_entry_get_text (GTK_ENTRY (editable));
    gtk_clist_set_text (list, selectedRowNo, 0, text);
    gtk_object_set_data (GTK_OBJECT (dialogue), "PathChanged", GINT_TO_POINTER (TRUE));

    /* Change the conversion button (relative/absolute path) appearance */
    {
        GtkButton *button = GTK_BUTTON (LookupWidget (dialogue, "ConvertButton"));

        if (text && *text)
        {
            GtkLabel *label = GTK_LABEL (GTK_BIN (button)->child);

            if (*text == '/')
                gtk_label_set_text (label, "->Relative path");
            else
                gtk_label_set_text (label, "->Absolute path");

            gtk_widget_show (GTK_WIDGET (button));
        } else
            gtk_widget_hide (GTK_WIDGET (button));
    }
}

void OnProjectDirectoryBrowse (GtkObject * dialogue)
{
    GtkWidget *entry = LookupWidget (GTK_WIDGET (dialogue), "ProjectDirectoryEntry");

    MakeDirectorySelectionDialogue ("Select A Project Directory",
      gtk_entry_get_text (GTK_ENTRY
        (entry)), (FileSelectionCallback) SetEntryStringCallback, (gpointer) GTK_WIDGET (entry), NULL, GTK_WIDGET (dialogue));
}

void AddRelativeDirectory (char *directory, GtkWidget * entry)
{
    GtkWidget *direntry = LookupWidget (workSpace.projectOptionsDialogue->dialogue, "ProjectDirectoryEntry");
    const char *directoryRef = gtk_entry_get_text (GTK_ENTRY (direntry));

    char *relativePath = ConvertToRelativePath (directory, directoryRef);

    //  printf("path=%s\n", relativePath);
    gtk_entry_set_text (GTK_ENTRY (entry), relativePath);
    gtk_entry_set_position (GTK_ENTRY (entry), -1);
}

gboolean SetDirectoryCallback (char *directory, GtkWidget * entry)
{
    AddRelativeDirectory (directory, entry);
    return TRUE;                //destroy dialogue
}

void OnProjectFileImportPathBrowse (GtkObject * dialogue)
{
    GtkWidget *entry = LookupWidget (GTK_WIDGET (dialogue), "ProjectFileImportPathEntry");

    gtk_object_set_data (GTK_OBJECT (dialogue), "PathChanged", GINT_TO_POINTER (TRUE));
    MakeDirectorySelectionDialogue ("Select A File Import Directory",
      gtk_entry_get_text (GTK_ENTRY
        (entry)), (FileSelectionCallback) SetDirectoryCallback, (gpointer) GTK_WIDGET (entry), NULL, GTK_WIDGET (dialogue));
}

void AddOptionTextFrom (GtkEntry * edit, char *optionText)
{
    const char *text = gtk_entry_get_text (edit);
    long containsValue = (long) strchr (optionText, '=');

    if (containsValue)
        containsValue -= (long) optionText;

    ProjectOptionsToggleActive = FALSE;
    if (text && *text)
    {
        char **tmp = g_strsplit (text, " ", 1000);
        char **ptr = tmp;

        for (; *ptr; ptr++)
            if ((!containsValue && !strcmp (*ptr, optionText)) || (containsValue && !strncmp (*ptr, optionText, containsValue + 1)))
                goto a;

        char *newText = g_strconcat (text, " ", optionText, NULL);
	g_strstrip (newText);
        gtk_entry_set_text (edit, newText);
	g_free (newText);
      a:
        g_strfreev (tmp);
    } else
        gtk_entry_set_text (edit, optionText);
    ProjectOptionsToggleActive = TRUE;
}

void RemoveOptionTextFrom (GtkEntry * edit, char *optionText)
{
    const char *text = gtk_entry_get_text (edit);
    long containsValue = (long) strchr (optionText, '=');

    if (containsValue)
        containsValue -= (long) optionText;

    ProjectOptionsToggleActive = FALSE;
    if (text && *text)
    {
        char **tmp = g_strsplit (text, " ", 1000);
        char **ptr = tmp;

        for (; *ptr; ptr++)
            if ((!containsValue && !strcmp (*ptr, optionText)) || (containsValue && !strncmp (*ptr, optionText, containsValue + 1)))
            {
                g_free (*ptr);
                *ptr = g_strdup ("");
            }

        char *newText = g_strjoinv (" ", tmp);
	g_strstrip (newText);
        gtk_entry_set_text (edit, newText);
	g_free (newText);
        g_strfreev (tmp);
    }
    ProjectOptionsToggleActive = TRUE;
}

void OnProjectOptions_FlattenedCompilation (GtkButton * button)
{
    if (ProjectOptionsToggleActive)
    {
        GtkWidget *dialogue = gtk_widget_get_toplevel (GTK_WIDGET (button));
        GtkWidget *project_BALSACOPTS = LookupWidget (dialogue, "ProjectOptionsBALSACOPTSentry");

        AddOptionTextFrom (GTK_ENTRY (project_BALSACOPTS), "-f");
    }
}

void OnProjectOptions_HierarchicalCompilation (GtkButton * button)
{
    if (ProjectOptionsToggleActive)
    {
        GtkWidget *dialogue = gtk_widget_get_toplevel (GTK_WIDGET (button));
        GtkWidget *project_BALSACOPTS = LookupWidget (dialogue, "ProjectOptionsBALSACOPTSentry");

        RemoveOptionTextFrom (GTK_ENTRY (project_BALSACOPTS), "-f");
    }
}

int ProjectOptions_UpdateCheckBox (GtkToggleButton * toggle1, GtkToggleButton * toggle2, GtkEntry * edit, char *keyword)
{
    int value = 0;
    gboolean found = FALSE;
    const char *text = gtk_entry_get_text (edit);
    long containsValue = (long) strchr (keyword, '=');

    if (containsValue)
        containsValue -= (long) keyword;

    if (text && *text)
    {
        char **tmp = g_strsplit (text, " ", 1000);
        char **ptr = tmp;

        for (; *ptr; ptr++)
            if ((!containsValue && !strcmp (*ptr, keyword)) || (containsValue && !strncmp (*ptr, keyword, containsValue + 1)))
            {
                found = TRUE;
                if (containsValue)
                    value = atoi ((*ptr) + containsValue + 1);
                goto a;
            }

      a:
        g_strfreev (tmp);
    }

    if (text)
    {
        ProjectOptionsToggleActive = FALSE;
        gtk_toggle_button_set_active (found ? toggle1 : toggle2, TRUE);
        ProjectOptionsToggleActive = TRUE;
    }

    return value;
}

int ProjectOptions_UpdateToggle (GtkToggleButton * toggle, GtkEntry * edit, char *keyword)
{
    int value = 0;
    gboolean found = FALSE;
    const char *text = gtk_entry_get_text (edit);
    long containsValue = (long) strchr (keyword, '=');

    if (containsValue)
        containsValue -= (long) keyword;

    if (text && *text)
    {
        char **tmp = g_strsplit (text, " ", 1000);
        char **ptr = tmp;

        for (; *ptr; ptr++)
            if ((!containsValue && !strcmp (*ptr, keyword)) || (containsValue && !strncmp (*ptr, keyword, containsValue + 1)))
            {
                found = TRUE;
                if (containsValue)
                    value = atoi ((*ptr) + containsValue + 1);
                goto a;
            }

      a:
        g_strfreev (tmp);
    }

    if (text)
    {
        ProjectOptionsToggleActive = FALSE;
        gtk_toggle_button_set_active (toggle, found);
        ProjectOptionsToggleActive = TRUE;
    }

    return value;
}

void OnProjectOptionsBALSACOPTSentryChanged (GtkEditable * editable)
{
    if (ProjectOptionsToggleActive)
    {
        GtkWidget *dialogue = gtk_widget_get_toplevel (GTK_WIDGET (editable));
        GtkWidget *project_FlattenedCompilation_checkbox = LookupWidget (dialogue,
          "ProjectOptions_FlattenedCompilation");
        GtkWidget *project_HierarchicalCompilation_checkbox = LookupWidget (dialogue,
          "ProjectOptions_HierarchicalCompilation");

        ProjectOptions_UpdateCheckBox (GTK_TOGGLE_BUTTON
          (project_FlattenedCompilation_checkbox), GTK_TOGGLE_BUTTON (project_HierarchicalCompilation_checkbox), GTK_ENTRY (editable), "-f");
    }
}

void ProjectOptions_UpdateCheckBoxes (void)
{
    GtkWidget *dialogue = workSpace.projectOptionsDialogue->dialogue;
    GtkWidget *project_FlattenedCompilation_checkbox = LookupWidget (dialogue, "ProjectOptions_FlattenedCompilation");
    GtkWidget *project_HierarchicalCompilation_checkbox = LookupWidget (dialogue,
    	"ProjectOptions_HierarchicalCompilation");
    GtkWidget *project_BreezeDirectSimulation_checkbox = LookupWidget (dialogue,
    	"ProjectOptions_BreezeDirectSimulation");
    GtkWidget *project_TraceAllChannels_checkbox = LookupWidget (dialogue, "ProjectOptions_TraceAllChannels");
    GtkWidget *project_TraceOnlyInterfacePorts_checkbox = LookupWidget (dialogue,
    	"ProjectOptions_TraceOnlyInterfacePorts");
    GtkWidget *project_NoTraceFlushing_checkbox = LookupWidget (dialogue, "ProjectOptions_NoTraceFlushing");
    GtkWidget *project_TraceFlushingDelay_checkbox = LookupWidget (dialogue, "ProjectOptions_TraceFlushingDelay");
    GtkWidget *project_SaveChannelNumbers = LookupWidget (dialogue, "ProjectOptions_SaveChannelNumbers");
    GtkWidget *BALSACOPTSentry = LookupWidget (dialogue, "ProjectOptionsBALSACOPTSentry");
    GtkWidget *BREEZESIMOPTSentry = LookupWidget (dialogue, "ProjectOptionsBREEZESIMOPTSentry");
    GtkWidget *BREEZELINKOPTSentry = LookupWidget (dialogue, "ProjectOptionsBREEZELINKOPTSentry");

    ProjectOptions_UpdateCheckBox (GTK_TOGGLE_BUTTON
      (project_FlattenedCompilation_checkbox), GTK_TOGGLE_BUTTON (project_HierarchicalCompilation_checkbox), GTK_ENTRY (BALSACOPTSentry), "-f");
    ProjectOptions_UpdateCheckBox (GTK_TOGGLE_BUTTON
      (project_TraceAllChannels_checkbox),
      GTK_TOGGLE_BUTTON (project_TraceOnlyInterfacePorts_checkbox), GTK_ENTRY (BREEZESIMOPTSentry), "--traceallchans");
    ProjectOptions_UpdateCheckBox (GTK_TOGGLE_BUTTON
      (project_TraceFlushingDelay_checkbox), GTK_TOGGLE_BUTTON (project_NoTraceFlushing_checkbox), GTK_ENTRY (BREEZESIMOPTSentry), "--flush=");
    ProjectOptions_UpdateToggle (GTK_TOGGLE_BUTTON (project_SaveChannelNumbers), GTK_ENTRY (BREEZELINKOPTSentry), "--save-channel-numbers");

    ProjectOptionsToggleActive = FALSE;
    switch (CurrentBalsaProject->simulationSystem)
    {
    case breezesim:
        gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (project_BreezeDirectSimulation_checkbox), TRUE);
    }
    ProjectOptionsToggleActive = TRUE;
}

void ProjectOptions_DisplayWindow (gboolean newProject)
{
    PtrBalsaProject project = CurrentBalsaProject;

    if (!project)
        return;

    if (GTK_IS_DIALOG (workSpace.projectOptionsDialogue->dialogue))
    {
        gdk_window_raise (workSpace.projectOptionsDialogue->dialogue->window);
        return;
    }

    workSpace.projectOptionsDialogue->dialogue = CreateWindow ("ProjectOptionsDialogue");

    /* This is a dialogue for an existing project */
    gtk_object_set_data (GTK_OBJECT (workSpace.projectOptionsDialogue->dialogue), "Project", GINT_TO_POINTER (project));
    gtk_object_set_data (GTK_OBJECT (workSpace.projectOptionsDialogue->dialogue), "SelectedPath", GINT_TO_POINTER (PROJ_OPTS_NO_SELECTION));
    gtk_object_set_data (GTK_OBJECT (workSpace.projectOptionsDialogue->dialogue), "PathChanged", GINT_TO_POINTER (FALSE));
    gtk_object_set_data (GTK_OBJECT (workSpace.projectOptionsDialogue->dialogue), "SelectedDefine", GINT_TO_POINTER (PROJ_OPTS_NO_SELECTION));
    gtk_object_set_data (GTK_OBJECT (workSpace.projectOptionsDialogue->dialogue), "DefinesChanged", GINT_TO_POINTER (FALSE));

    /* update the file list here to get directory updates right */
    //UpdateBalsaProjectFromFileView (project);

    /* Set fields from the Project structure */
    gtk_entry_set_text (
    	GTK_ENTRY (LookupWidget (workSpace.projectOptionsDialogue->dialogue, "ProjectNameEntry")),
    	project->name);
    gtk_entry_set_text (
    	GTK_ENTRY (LookupWidget (workSpace.projectOptionsDialogue->dialogue, "ProjectDirectoryEntry")),
    	project->directory);
    gtk_entry_set_text (
    	GTK_ENTRY (LookupWidget (workSpace.projectOptionsDialogue->dialogue, "ProjectOptionsBALSACOPTSentry")),
    	project->BALSACOPTS ? project->BALSACOPTS : "");
    gtk_entry_set_text (
    	GTK_ENTRY (LookupWidget (workSpace.projectOptionsDialogue->dialogue, "ProjectOptionsBREEZESIMOPTSentry")),
    	project->BREEZESIMOPTS ? project->BREEZESIMOPTS : "");
    gtk_entry_set_text (
    	GTK_ENTRY (LookupWidget (workSpace.projectOptionsDialogue->dialogue, "ProjectOptionsBREEZELINKOPTSentry")),
    	project->BREEZELINKOPTS ? project->BREEZELINKOPTS : "");

    ProjectOptions_UpdateCheckBoxes ();

    /* Set file import path */
    {
        GtkCList *ImportPathList = GTK_CLIST (LookupWidget (workSpace.projectOptionsDialogue->dialogue,
            "ProjectFileImportPathList"));
        GList *strings = project->importPath;

        while (strings)
        {
            gtk_clist_insert (ImportPathList, ImportPathList->rows, (char **) &(strings->data));
            strings = strings->next;
        }
    }

    /* Add defines */
    {
        GtkCList *DefinesList = GTK_CLIST (LookupWidget (workSpace.projectOptionsDialogue->dialogue, "DefinesList"));
        GList *defines = project->defines;
        char *strings[2];

        while (defines)
        {
            strings[0] = ((PtrDefine) defines->data)->name;
            strings[1] = ((PtrDefine) defines->data)->value;
            gtk_clist_insert (DefinesList, DefinesList->rows, (char **) strings);
            defines = defines->next;
        }
    }

    /* Make name selected so we can directly overtype it */
    {
        GtkEntry *nameEntry = GTK_ENTRY (LookupWidget (workSpace.projectOptionsDialogue->dialogue,
            "ProjectNameEntry"));

        gtk_entry_select_region (GTK_ENTRY (nameEntry), 0, -1);
        gtk_widget_grab_focus (GTK_WIDGET (nameEntry));
    }

    gtk_widget_show (workSpace.projectOptionsDialogue->dialogue);
}

gboolean OnProjectOptionsKeyPressEvent (GtkWidget * widget, GdkEventKey * event)
{
    if (event->keyval == GDK_Return)
        OnProjectOptionsDialogueOK (GTK_OBJECT (widget));
    else if (event->keyval == GDK_Escape)
        OnProjectOptionsDialogueCancel (GTK_OBJECT (widget));

    return FALSE;
}

void OnProjectOptions_BreezeDirectSimulation (GtkButton * button)
{
    GtkWidget *dialogue = gtk_widget_get_toplevel (GTK_WIDGET (button));

    gtk_widget_set_sensitive (LookupWidget (dialogue, "ProjectOptions_SaveChannelNumbers"), TRUE);
    gtk_widget_set_sensitive (LookupWidget (dialogue, "ProjectOptionsBREEZELINKOPTSentry"), TRUE);
}

void OnProjectOptions_TraceAllChannels (GtkButton * button)
{
    if (ProjectOptionsToggleActive)
    {
        GtkWidget *dialogue = gtk_widget_get_toplevel (GTK_WIDGET (button));
        GtkWidget *project_BREEZESIMOPTS = LookupWidget (dialogue,
          "ProjectOptionsBREEZESIMOPTSentry");

        AddOptionTextFrom (GTK_ENTRY (project_BREEZESIMOPTS), "--traceallchans");
    }
}

void OnProjectOptions_TraceOnlyInterfacePorts (GtkButton * button)
{
    if (ProjectOptionsToggleActive)
    {
        GtkWidget *dialogue = gtk_widget_get_toplevel (GTK_WIDGET (button));
        GtkWidget *project_BREEZESIMOPTS = LookupWidget (dialogue,
          "ProjectOptionsBREEZESIMOPTSentry");

        RemoveOptionTextFrom (GTK_ENTRY (project_BREEZESIMOPTS), "--traceallchans");
    }
}

void OnProjectOptions_NoTraceFlushing (GtkButton * button)
{
    GtkWidget *dialogue = gtk_widget_get_toplevel (GTK_WIDGET (button));

    gtk_widget_set_sensitive (LookupWidget (dialogue, "ProjectOptions_TraceFlushingDelaySpin"), FALSE);

    if (ProjectOptionsToggleActive)
    {
        GtkWidget *project_BREEZESIMOPTS = LookupWidget (dialogue, "ProjectOptionsBREEZESIMOPTSentry");

        RemoveOptionTextFrom (GTK_ENTRY (project_BREEZESIMOPTS), "--flush=");
    }
}

void OnProjectOptions_TraceFlushingDelaySpin (GtkEditable * editable)
{
    if (ProjectOptionsToggleActive)
    {
        GtkWidget *dialogue = gtk_widget_get_toplevel (GTK_WIDGET (editable));
        GtkWidget *project_BREEZESIMOPTS = LookupWidget (dialogue, "ProjectOptionsBREEZESIMOPTSentry");
        char *valueStr = gtk_editable_get_chars (editable, 0, -1);
        int value = atoi (valueStr);
        char *text = g_strdup_printf ("--flush=%d", value);

        ProjectOptionsToggleActive = FALSE;
        RemoveOptionTextFrom (GTK_ENTRY (project_BREEZESIMOPTS), "--flush=");
        AddOptionTextFrom (GTK_ENTRY (project_BREEZESIMOPTS), text);
        ProjectOptionsToggleActive = TRUE;

        g_free (text);
    }
}

void OnProjectOptions_TraceFlushingDelay (GtkButton * button)
{
    GtkWidget *dialogue = gtk_widget_get_toplevel (GTK_WIDGET (button));

    gtk_widget_set_sensitive (LookupWidget (dialogue, "ProjectOptions_TraceFlushingDelaySpin"), TRUE);

    if (ProjectOptionsToggleActive)
    {
        OnProjectOptions_TraceFlushingDelaySpin (GTK_EDITABLE (LookupWidget
        	(dialogue, "ProjectOptions_TraceFlushingDelaySpin")));
    }
}

gboolean OnProjectOptions_TraceFlushingDelaySpin_key (GtkWidget * widget, GdkEventKey * event)
{
    //    OnProjectOptions_TraceFlushingDelaySpin (GTK_EDITABLE (widget));
    return TRUE;
}

void OnProjectOptions_SaveChannelNumbers (GtkToggleButton * togglebutton)
{
    GtkWidget *dialogue = gtk_widget_get_toplevel (GTK_WIDGET (togglebutton));

    if (ProjectOptionsToggleActive)
    {
        GtkWidget *project_BREEZELINKOPTS = LookupWidget (dialogue, "ProjectOptionsBREEZELINKOPTSentry");
        int active = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (LookupWidget (dialogue,
              "ProjectOptions_SaveChannelNumbers")));

        if (active)
            AddOptionTextFrom (GTK_ENTRY (project_BREEZELINKOPTS), "--save-channel-numbers");
        else
            RemoveOptionTextFrom (GTK_ENTRY (project_BREEZELINKOPTS), "--save-channel-numbers");
    }
}

void OnProjectOptionsBREEZESIMOPTSentryChanged (GtkEditable * editable)
{
    if (ProjectOptionsToggleActive)
    {
        GtkWidget *dialogue = gtk_widget_get_toplevel (GTK_WIDGET (editable));
        GtkWidget *project_TraceAllChannels_checkbox = LookupWidget (dialogue,
          "ProjectOptions_TraceAllChannels");
        GtkWidget *project_TraceOnlyInterfacePorts_checkbox = LookupWidget (dialogue,
          "ProjectOptions_TraceOnlyInterfacePorts");
        GtkWidget *project_NoTraceFlushing_checkbox = LookupWidget (dialogue,
          "ProjectOptions_NoTraceFlushing");
        GtkWidget *project_TraceFlushingDelay_checkbox = LookupWidget (dialogue,
          "ProjectOptions_TraceFlushingDelay");

        ProjectOptions_UpdateCheckBox (GTK_TOGGLE_BUTTON
          (project_TraceAllChannels_checkbox), GTK_TOGGLE_BUTTON (project_TraceOnlyInterfacePorts_checkbox), GTK_ENTRY (editable), "--traceallchans");
        int value = ProjectOptions_UpdateCheckBox (GTK_TOGGLE_BUTTON (project_TraceFlushingDelay_checkbox),
          GTK_TOGGLE_BUTTON (project_NoTraceFlushing_checkbox),
          GTK_ENTRY (editable), "--flush=");

        if (value > 0)
        {
            GtkWidget *project_TraceFlushingDelay_spin = LookupWidget (dialogue,
              "ProjectOptions_TraceFlushingDelaySpin");

            ProjectOptionsToggleActive = FALSE;
            gtk_spin_button_set_value (GTK_SPIN_BUTTON (project_TraceFlushingDelay_spin), (gfloat) value);
            ProjectOptionsToggleActive = TRUE;
        }
    }
}

void OnProjectOptionsBREEZELINKOPTSentryChanged (GtkEditable * editable)
{
    if (ProjectOptionsToggleActive)
    {
        GtkWidget *dialogue = gtk_widget_get_toplevel (GTK_WIDGET (editable));
        GtkWidget *project_SaveChannelNumbers = LookupWidget (dialogue,
          "ProjectOptions_SaveChannelNumbers");

        ProjectOptions_UpdateToggle (GTK_TOGGLE_BUTTON (project_SaveChannelNumbers), GTK_ENTRY (editable), "--save-channel-numbers");
    }
}

void OnProjectOptionsDialogueSaveAsDefaultTemplate (GtkButton * button)
{
    GtkWidget *dialogue = gtk_widget_get_toplevel (GTK_WIDGET (button));
    const char *project_BALSACOPTS;
    const char *project_BREEZESIMOPTS;
    const char *project_BREEZELINKOPTS;

    char *project_simulationSystem = NULL;

    project_BALSACOPTS = gtk_entry_get_text (GTK_ENTRY (LookupWidget (dialogue, "ProjectOptionsBALSACOPTSentry")));
    project_BREEZESIMOPTS = gtk_entry_get_text (GTK_ENTRY (LookupWidget (dialogue, "ProjectOptionsBREEZESIMOPTSentry")));
    project_BREEZELINKOPTS = gtk_entry_get_text (GTK_ENTRY (LookupWidget (dialogue, "ProjectOptionsBREEZELINKOPTSentry")));
    {
        int toggle_BreezeDirectSimulation = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (LookupWidget (dialogue,
              "ProjectOptions_BreezeDirectSimulation")));

        if (toggle_BreezeDirectSimulation)
            project_simulationSystem = "breezesim";
    }

    if (EnvironmentOptions.newProjectTemplate)
    {
        g_slist_free (EnvironmentOptions.newProjectTemplate);
        EnvironmentOptions.newProjectTemplate = NULL;
    }

    GSList *list = NULL;

    if (project_BALSACOPTS && *project_BALSACOPTS)
        list = g_slist_prepend (list, g_strdup_printf ("BALSACOPTS=%s", project_BALSACOPTS));
    //    if (project_BREEZESIMOPTS && *project_BREEZESIMOPTS)
    //        list = g_slist_prepend (list, g_strdup_printf ("BREEZESIMOPTS=%s", project_BREEZESIMOPTS));
    if (project_BREEZELINKOPTS && *project_BREEZELINKOPTS)
        list = g_slist_prepend (list, g_strdup_printf ("BREEZELINKOPTS=%s", project_BREEZELINKOPTS));
    if (project_simulationSystem)
        list = g_slist_prepend (list, g_strdup_printf ("simulation-system=%s", project_simulationSystem));

    EnvironmentOptions.newProjectTemplate = list;
    WriteEnvironmentOptions ();
}
