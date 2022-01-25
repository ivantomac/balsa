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

	`filemanager.c'

*/

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif
#include <glib.h>
#include <gtk/gtk.h>
#include <stdio.h>
#include <sys/stat.h>
#include <string.h>

#include "filemanager.h"

#include "editor.h"
#include "main.h"
#include "utils.h"
#include "paths.h"
#include "support.h"

struct fileStruct
{
    char *filename;
    char *displayedName;
    int notebookPage;
    GtkWidget *TextWidget, *LabelWidget;
    gboolean dirty;
    gboolean externallyModified;
    struct stat lastStat;
};

GtkWidget *FilesNotebook;
GSList *FileManager_OpenedFiles = NULL;
GdkFont *EditorFont;

void UpdateLabel (struct fileStruct *fileStruct)
{
    char *label, *newlabel;

    gtk_label_get (GTK_LABEL (fileStruct->LabelWidget), &label);

    newlabel = g_strdup_printf ("%s%s%s", fileStruct->displayedName, fileStruct->dirty ? "*" : "", fileStruct->externallyModified ? "!" : "");

    gtk_label_set_text (GTK_LABEL (fileStruct->LabelWidget), newlabel);
    //g_free(label);
}

struct fileStruct *GetFileStruct (const char *filename)
{
    GSList *tmp;

    if (!filename || !filename[0])
        return NULL;
    else
        filename = ExpandFilename /*ImportPaths_GetCompletePathedFilename */ (filename,
          TRUE, NULL);
    // filename = ImportPaths_ConvertFileToRelative (filename);

    for (tmp = FileManager_OpenedFiles; tmp; tmp = tmp->next)
        if (((struct fileStruct *) tmp->data)->filename && !strcmp (((struct fileStruct *) tmp->data)->filename, filename))
            break;

    if (!tmp)
        return NULL;
    else
        return tmp->data;
}

struct fileStruct *GetFileStructFromPage (int numPage)
{
    GSList *tmp;

    for (tmp = FileManager_OpenedFiles; tmp; tmp = tmp->next)
        if (((struct fileStruct *) tmp->data)->notebookPage == numPage)
            break;

    if (!tmp)
        return NULL;
    else
        return tmp->data;
}

struct fileStruct *getCurrentFileStruct (void)
{
    int page = gtk_notebook_get_current_page (GTK_NOTEBOOK (FilesNotebook));

    return GetFileStructFromPage (page);
}

void SetDirtyFlag (struct fileStruct *fileStruct, gboolean dirty)
{
    if (dirty == FALSE)
    {
        stat (fileStruct->filename, &fileStruct->lastStat);
        if (fileStruct->externallyModified)
        {
            fileStruct->externallyModified = FALSE;
        }
    }

    if (fileStruct->dirty != dirty)
    {
        fileStruct->dirty = dirty;
        if (CurrentBalsaProject)
        {
            if (dirty)
                CurrentBalsaProject->nb_dirty_files++;
            else
                CurrentBalsaProject->nb_dirty_files--;

            UpdateProjectName ();
        }
    }

    UpdateLabel (fileStruct);
}

void FileManager_SetDirtyFlagToCurrentFile (gboolean dirty)
{
    SetDirtyFlag (getCurrentFileStruct (), dirty);
}

void BringToTop (struct fileStruct *fileStruct)
{
    gtk_notebook_set_page (GTK_NOTEBOOK (FilesNotebook), fileStruct->notebookPage);
}

void RefreshFile (struct fileStruct *fileStruct)
{
    if (!fileStruct->filename)
        return;

    if (Editor_LoadFile (fileStruct->TextWidget, fileStruct->filename))
        SetDirtyFlag (fileStruct, FALSE);
}

static void ConfirmReload_OnYes (GtkWidget * button, GtkSignalFunc fileStruct)
{
    RefreshFile ((struct fileStruct *) fileStruct);
}

void CheckIfFileWasExternallyModified (struct fileStruct *fileStruct)
{
    if (!fileStruct || !fileStruct->lastStat.st_mtime)
        return;
    else
    {
        struct stat newStat;

        if (stat (fileStruct->filename, &newStat) == 0)
            if (newStat.st_mtime != fileStruct->lastStat.st_mtime)
            {                   // file changed
                fileStruct->lastStat = newStat;
                fileStruct->externallyModified = TRUE;
                UpdateLabel (fileStruct);

                {
                    GtkWidget *dialogue;
                    const gchar *buttons[] = { "gtk-yes", "gtk-no" };
                    GtkSignalFunc handlers[] = { (GtkSignalFunc) ConfirmReload_OnYes, NULL };

                    dialogue =
                      util_create_dialogue_with_buttons ("File Externally modified\nDo you want to reload it?", 2, buttons, 2, handlers, fileStruct);
                    gtk_window_set_position (GTK_WINDOW (dialogue), GTK_WIN_POS_MOUSE);
                    gtk_widget_show (dialogue);
                }
            }

    }
}

gboolean FileManager_SignalTextActivate (GtkWidget * widget, gpointer data)
{
    struct fileStruct *fileStruct = gtk_object_get_data (GTK_OBJECT (widget), "fileStruct");

    if (fileStruct)
        CheckIfFileWasExternallyModified (fileStruct);
    else
        printfConsole ("Problem in SignalTextActivate\n");

    return FALSE;
}

void AddNewFile (const char *filename)
{
    struct fileStruct *newStruct;

    if (filename && filename[0])
        filename = ExpandFilename /*ImportPaths_GetCompletePathedFilename */ (filename,
          TRUE, NULL);
    //   filename = ImportPaths_ConvertFileToRelative (filename);

    /* Create the new structure */
    newStruct = (struct fileStruct *) g_malloc (sizeof (struct fileStruct));
    newStruct->filename = g_strdup (filename);
    stat (newStruct->filename, &newStruct->lastStat);
    newStruct->externallyModified = FALSE;

    if (filename)
        newStruct->displayedName = ImportPaths_ConvertFileToRelative (filename);
    else
        newStruct->displayedName = g_strdup ("<noname>");

    FileManager_OpenedFiles = g_slist_append (FileManager_OpenedFiles, newStruct);

    /* Create the new notebook pane */
    {
        GtkTooltips *tooltips = gtk_tooltips_new ();
        GtkWidget *newEditor, *newLabel;

        newEditor = create_new_editor ();
        newLabel = gtk_label_new (g_strdup (newStruct->displayedName));
        gtk_tooltips_set_tip (tooltips, GTK_WIDGET (newLabel), "toto", "titi");

        gtk_widget_show (newEditor);
        gtk_widget_show (newLabel);
        gtk_notebook_append_page (GTK_NOTEBOOK (FilesNotebook), newEditor, newLabel);
        gtk_notebook_set_page (GTK_NOTEBOOK (FilesNotebook), -1);

        newStruct->notebookPage = gtk_notebook_get_current_page (GTK_NOTEBOOK (FilesNotebook));
        newStruct->TextWidget = newEditor;
        newStruct->LabelWidget = newLabel;
        newStruct->dirty = FALSE;
        gtk_object_set_data (GTK_OBJECT (newEditor), "fileStruct", newStruct);

        RefreshFile (newStruct);
    }

    /* Show the notebook, in case where we create the first file in it */
    gtk_widget_show (FilesNotebook);
}

gboolean OnFilesNotebook_ButtonPressEvent (GtkWidget * widget, GdkEventButton * event)
{
    if (event->button == 3)     //Right button pressed
    {
        GtkWidget *menu = LookupWidget (MainWindow, "FileMenu_menu");

        gtk_menu_popup (GTK_MENU (menu), NULL, NULL, NULL, NULL, 0, 0);
    }
    return FALSE;
}

static void ConfirmSaveCurrentFile_OnYes (GtkWidget * button, GtkSignalFunc func)
{
    FileManager_SaveFile ();
    func ();
}
static void ConfirmSaveCurrentFile_OnNo (GtkWidget * button, GtkSignalFunc func)
{
    func ();
}

void ConfirmSaveCurrentFileAndExecute (GtkSignalFunc func)
{
    /* check if the current file needs to be saved */
    struct fileStruct *fileStruct = getCurrentFileStruct ();

    if (!fileStruct)
        return;

    if (fileStruct->dirty)
    {
        GtkWidget *dialogue;
        const gchar *buttons[] = { "gtk-yes", "gtk-no", "gtk-cancel" };
        GtkSignalFunc handlers[] = { (GtkSignalFunc) ConfirmSaveCurrentFile_OnYes,
            (GtkSignalFunc) ConfirmSaveCurrentFile_OnNo, NULL
        };

        dialogue = util_create_dialogue_with_buttons ("Do you want to save the current file?", 3, buttons, 3, handlers, func);
        gtk_window_set_position (GTK_WINDOW (dialogue), GTK_WIN_POS_MOUSE);
        gtk_window_set_modal (GTK_WINDOW (dialogue), TRUE);
        gtk_widget_show (dialogue);
    } else
        func ();
}

void SaveFileCorrespondingToFileStruct (struct fileStruct *fileStruct)
{
    GtkWidget *editor = fileStruct->TextWidget;

    if (!fileStruct->filename)
    {
        FileManager_SaveFileAs ();
        return;
    }

    if (Editor_SaveBuffer (editor, fileStruct->filename))
        SetDirtyFlag (fileStruct, FALSE);
}

/********************/
/* Public Functions */
/********************/

void FileManager_Initialize (void)
{
    EditorFont = gdk_font_load ("Monospace");
    FilesNotebook = LookupWidget (MainWindow, "FilesNotebook");
    gtk_notebook_remove_page (GTK_NOTEBOOK (FilesNotebook), 0);
}

void FileManager_NewFile (void)
{
    AddNewFile (NULL);
    UpdateMainWindowGreying ();
}

gboolean FileManager_OpenFile_Callback (const char *filename)
{
    FileManager_OpenFile (filename);
    return TRUE;
}

void FileManager_OpenFile (const char *filename)
{
    FileManager_OpenFileAtLineAndColumn (filename, 0, 0);
}

void FileManager_OpenFileAtLine (const char *filename, int line)
{
    FileManager_OpenFileAtLineAndColumn (filename, line, 0);
}

void FileManager_OpenFileAtLineAndColumn (const char *filename, int line, int column)
{
    if (filename && filename[0])
    {
        filename = ExpandFilename /*ImportPaths_GetCompletePathedFilename */ (filename,
          TRUE, NULL);
        //        filename = ImportPaths_ConvertFileToRelative (filename);

        if (!doesFileExist (filename))
            return;

        struct fileStruct *fileStruct = GetFileStruct (filename);

        if (fileStruct)
            BringToTop (fileStruct);
        else
            AddNewFile (filename);

        if (line)
        {
            Editor_GotoAndHighlight_LineColumn (getCurrentFileStruct ()->TextWidget, line, column);
        }

        UpdateMainWindowGreying ();
    } else
    {
        MakeFileSelectionDialogue (FileSelectionDialogue_Open, "Open File... (Balsa:Try Up/Down arrows)",
        	(FileSelectionCallback) FileManager_OpenFile_Callback, NULL, NULL);
    }
}

void FileManager_ReOpenFile_afterSaveConfirmation (void)
{
    struct fileStruct *fileStruct = getCurrentFileStruct ();

    RefreshFile (fileStruct);
}

void FileManager_ReOpenFile (void)
{
    ConfirmSaveCurrentFileAndExecute (FileManager_ReOpenFile_afterSaveConfirmation);
}

void FileManager_SaveFile (void)
{
    SaveFileCorrespondingToFileStruct (getCurrentFileStruct ());
}

gboolean FileManager_SaveAs_Callback (const char *filename)
{
    struct fileStruct *fileStruct = getCurrentFileStruct ();

    if (!filename || !filename[0])
        return TRUE;
    else
        filename = ExpandFilename /*ImportPaths_GetCompletePathedFilename */ (filename,
          TRUE, NULL);

    fileStruct->filename = g_strdup (filename);

    if (filename)
        fileStruct->displayedName = ImportPaths_ConvertFileToRelative (filename);
    else
        fileStruct->displayedName = g_strdup ("<noname>");

    UpdateMainWindowGreying ();
    FileManager_SaveFile ();

    return TRUE;
}

void FileManager_SaveFileAs (void)
{
    MakeFileSelectionDialogue (FileSelectionDialogue_Save,
    	"Save As...", (FileSelectionCallback) FileManager_SaveAs_Callback, NULL, NULL);
}

void FileManager_CloseFile_afterSaveConfirmation (void)
{
    int numPage = gtk_notebook_get_current_page (GTK_NOTEBOOK (FilesNotebook));

    /* Remove the page from the notebook */
    gtk_notebook_remove_page (GTK_NOTEBOOK (FilesNotebook), numPage);

    /* Remove the structure correspoonding to the page in the GSList */
    {
        GSList *tmp, *toBeRemoved;

        for (tmp = FileManager_OpenedFiles; tmp; tmp = tmp->next)
            if (((struct fileStruct *) tmp->data)->notebookPage == numPage)
                toBeRemoved = tmp;
            else if (((struct fileStruct *) tmp->data)->notebookPage > numPage)
                ((struct fileStruct *) tmp->data)->notebookPage--;

        FileManager_OpenedFiles = g_slist_remove_link (FileManager_OpenedFiles, toBeRemoved);
    }
    UpdateMainWindowGreying ();
}

void FileManager_CloseFile (void)
{
    ConfirmSaveCurrentFileAndExecute (FileManager_CloseFile_afterSaveConfirmation);
}

void FileManager_CloseAllFiles_afterSaveConfirmation (void)
{
    FileManager_CloseFile_afterSaveConfirmation ();
    FileManager_CloseAllFiles ();
}

void FileManager_CloseAllFiles (void)
{
    ConfirmSaveCurrentFileAndExecute (FileManager_CloseAllFiles_afterSaveConfirmation);
}

char *FileManager_GetCurrentFileName (void)
{
    struct fileStruct *fileStruct = getCurrentFileStruct ();

    if (fileStruct)
        return g_strdup (fileStruct->filename);
    else
        return NULL;
}

char *FileManager_GetCurrentDisplayedName (void)
{
    struct fileStruct *fileStruct = getCurrentFileStruct ();

    if (fileStruct)
        return g_strdup (fileStruct->displayedName);
    else
        return NULL;
}

int FileManager_GetCurrentLineNumber (void)
{
    return -1;
}

void FileManager_SaveFileIfOpenedAndIfNeeded (char *filename)
{
    if (!filename || !filename[0])
        return;
    else
        filename = ExpandFilename /*ImportPaths_GetCompletePathedFilename */ (filename,
          TRUE, NULL);

    {
        struct fileStruct *fileStruct = GetFileStruct (filename);

        if (fileStruct && fileStruct->dirty)
            SaveFileCorrespondingToFileStruct (fileStruct);
    }
}

void FileManager_SaveAllNamedFiles (void)
{
    int numPage;

    for (numPage = 0;; numPage++)
    {
        struct fileStruct *fileStruct = GetFileStructFromPage (numPage);

        if (!fileStruct)
            return;

        if (fileStruct && fileStruct->dirty && fileStruct->filename)
            SaveFileCorrespondingToFileStruct (fileStruct);
    }
}

void FileManager_RefreshAllDisplayedNames (void)
{
    static char *lastPath = NULL;
    int numPage;

    if (!CurrentBalsaProject || !CurrentBalsaProject->directory)
        return;

    if (!lastPath || strcmp (lastPath, CurrentBalsaProject->directory))
    {
        if (lastPath)
            g_free (lastPath);
        lastPath = g_strdup (CurrentBalsaProject->directory);
    }

    for (numPage = 0;; numPage++)
    {
        struct fileStruct *fileStruct = GetFileStructFromPage (numPage);

        if (!fileStruct)
            return;

        if (fileStruct && fileStruct->filename)
        {
            fileStruct->displayedName = ImportPaths_ConvertFileToRelative (fileStruct->filename);
            UpdateLabel (fileStruct);
        }
    }
}
