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

	`main.c'
	Signal handlers and main function
*/

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

#include <breeze/libbreeze.h>

#include "workspace.h"
#include "main.h"
#include "project.h"
#include "miscgtk.h"
#include "options.h"
#include "commands.h"
#include "paths.h"
#include "file.h"
#include "testopts.h"
#include "projopts.h"
#include "utils.h"
#include "filemanager.h"
#include "executionmanager.h"
#include "makefiletable.h"
#include "addfiledialogue.h"
#include "icons.h"
#include "menus.h"
#include "consolewindow.h"
#include "technology.h"
#include "support.h"

struct environmentOptions EnvironmentOptions;

GList *OtherWindows_CurrentProject = NULL;
GList *OtherWindows_ClosedProjects = NULL;

GdkFont *MessagesFont = NULL;
GdkColor Red, Green, Blue;

int NewProjectCount = 1;

GList *Projects = NULL;
char *ProjectsFilename = NULL;

char *StartupPWD;
GtkWidget *RecentProjectsMenu = NULL;

PtrBalsaProjectEntry ProjectFileViewSelectedEntry = NULL;

static GList *FindFileProcedures (const char *filename);
static gboolean AddFileCallback (char *filename, gpointer userData);
static int GetProcedureLine (char *procedureName, char *filename);
static gboolean OpenProjectCallback (const char *directory);

void AddToOtherWindows_CurrentProject (GtkWidget * dialogue)
{
    OtherWindows_CurrentProject = g_list_append (OtherWindows_CurrentProject, dialogue);
}

void RemoveFromOtherWindows_CurrentProject (GtkWidget * dialogue)
{
    OtherWindows_CurrentProject = g_list_remove (OtherWindows_CurrentProject, dialogue);
}

void AddToOtherWindows_ClosedProjects (GtkWidget * dialogue)
{
    OtherWindows_ClosedProjects = g_list_append (OtherWindows_ClosedProjects, dialogue);
}

void RemoveFromOtherWindows_ClosedProjects (GtkWidget * dialogue)
{
    OtherWindows_ClosedProjects = g_list_remove (OtherWindows_ClosedProjects, dialogue);
}

void DestroyOtherWindow (GtkWidget * dialogue)
{
    RemoveFromOtherWindows_CurrentProject (dialogue);
    RemoveFromOtherWindows_ClosedProjects (dialogue);
    gtk_widget_destroy (dialogue);
}
static void RemoveProjectNameFromRecentProjectsHistory_OnYes (GtkWidget * button, char *projectName)
{
    //  printf("remove %s\n",projectName);
    GSList *history = EnvironmentOptions.projectsHistory;
    GSList *item;

    for (item = history; item; item = item->next)
        if (!strcmp (item->data, projectName))
            EnvironmentOptions.projectsHistory = g_slist_remove_link (history, item);

    WriteEnvironmentOptions ();
}

static char *NameProjectOpening;
void RecentProjectsHistory_MouseClick_afterSaveConfirmation (GtkWidget * button)
{
    if (OpenProjectCallback (NameProjectOpening) == FALSE)
    {
        // Ask to remove the project name from the RecentProjectsHistory
        GtkWidget *dialogue;
        const gchar *buttons[] = { "gtk-yes", "gtk-no" };
        GtkSignalFunc handlers[] = { (GtkSignalFunc) RemoveProjectNameFromRecentProjectsHistory_OnYes,
            NULL
        };

        dialogue =
          util_create_dialogue_with_buttons
          ("No Project file found in this directory.\nDo you want to remove it from the 'recent projects history'?",
          2, buttons, 1, handlers, NameProjectOpening);
        gtk_window_set_position (GTK_WINDOW (dialogue), GTK_WIN_POS_MOUSE);
        gtk_widget_show (dialogue);
    }
}
void RecentProjectsHistory_MouseClick (gchar * string)
{
    NameProjectOpening = string;
    ConfirmSaveCurrentBalsaProjectAndExecute ((GtkSignalFunc) RecentProjectsHistory_MouseClick_afterSaveConfirmation);
}

static void RecentProjectsHistory_ClearMenu_OnYes (GtkWidget * button, char *projectName)
{
    EnvironmentOptions.projectsHistory = NULL;
    WriteEnvironmentOptions ();
}

void RecentProjectsHistory_ClearMenu (gchar * string)
{
    GtkWidget *dialogue;
    const gchar *buttons[] = { "gtk-yes", "gtk-no" };
    GtkSignalFunc handlers[] = { (GtkSignalFunc) RecentProjectsHistory_ClearMenu_OnYes,
        NULL
    };

    dialogue = util_create_dialogue_with_buttons ("Are you sure you want to clear the 'recent projects history'?", 2, buttons, 1, handlers, NULL);
    gtk_window_set_position (GTK_WINDOW (dialogue), GTK_WIN_POS_MOUSE);
    gtk_widget_show (dialogue);
}

void AddRecentProject (char *projectName)
{
    GSList *ptr = EnvironmentOptions.projectsHistory;
    char *tmp;

    while (ptr)
    {
        if (!strcmp ((char *) ptr->data, projectName))
        {
            EnvironmentOptions.projectsHistory = g_slist_remove (EnvironmentOptions.projectsHistory, ptr->data);
            break;
        }
        ptr = ptr->next;
    }

    tmp = (char *) g_strdup (projectName);
    EnvironmentOptions.projectsHistory = g_slist_append (EnvironmentOptions.projectsHistory, tmp);

    WriteEnvironmentOptions ();
}

void UpdateSelectionMenuGreying (void)
{
    if (!ProjectFileViewSelectedEntry)
        return;

    GtkWidget *SelectionMenu_AddTest = LookupWidget (MainWindow, "SelectionMenu_AddTest");
    GtkWidget *SelectionMenu_AddImplementation = LookupWidget (MainWindow, "SelectionMenu_AddImplementation");
    GtkWidget *SelectionMenu_AddBuiltinLib = LookupWidget (MainWindow, "SelectionMenu_AddBuiltinLib");
    GtkWidget *SelectionMenu_Delete = LookupWidget (MainWindow, "SelectionMenu_Delete");

    switch (ProjectFileViewSelectedEntry->nature)
    {
    case BalsaProjectEntry_File:
        gtk_widget_set_sensitive (SelectionMenu_AddTest, 0);
        gtk_widget_set_sensitive (SelectionMenu_AddImplementation, 1);
        gtk_widget_set_sensitive (SelectionMenu_AddBuiltinLib, 1);
        gtk_widget_set_sensitive (SelectionMenu_Delete, 1);
        break;

    case BalsaProjectEntry_Procedure:
        gtk_widget_set_sensitive (SelectionMenu_AddTest, 1);
        gtk_widget_set_sensitive (SelectionMenu_AddImplementation, 1);
        gtk_widget_set_sensitive (SelectionMenu_AddBuiltinLib, 1);
        gtk_widget_set_sensitive (SelectionMenu_Delete, 0);
        break;

    case BalsaProjectEntry_Directory:
        gtk_widget_set_sensitive (SelectionMenu_AddTest, 0);
        gtk_widget_set_sensitive (SelectionMenu_AddImplementation, 1);
        gtk_widget_set_sensitive (SelectionMenu_AddBuiltinLib, 1);
        gtk_widget_set_sensitive (SelectionMenu_Delete, 0);
        break;

    case BalsaProjectEntry_TestComponent:
        gtk_widget_set_sensitive (SelectionMenu_AddTest, 1);
        gtk_widget_set_sensitive (SelectionMenu_AddImplementation, 1);
        gtk_widget_set_sensitive (SelectionMenu_AddBuiltinLib, 1);
        gtk_widget_set_sensitive (SelectionMenu_Delete, 0);
        break;

    case BalsaProjectEntry_Test:
    case BalsaProjectEntry_Implementation:
    case BalsaProjectEntry_BuiltinLib:
        gtk_widget_set_sensitive (SelectionMenu_AddTest, 1);
        gtk_widget_set_sensitive (SelectionMenu_AddImplementation, 1);
        gtk_widget_set_sensitive (SelectionMenu_AddBuiltinLib, 1);
        gtk_widget_set_sensitive (SelectionMenu_Delete, 1);
        break;
    }
}

/* UpdateMainWindowGreying : update the greying out of menus/menu items in the MainWindow */
void UpdateMainWindowGreying (void)
{
    gboolean projectActive = CurrentBalsaProject ? TRUE : FALSE;
    gboolean fileActive = (FileManager_GetCurrentDisplayedName () != NULL);
    gboolean fileActiveNamed = (FileManager_GetCurrentFileName () != NULL);
    gboolean projectSelectionActive = (ProjectFileViewSelectedEntry != NULL);

    // Project Toolbar
    gtk_widget_set_sensitive (LookupWidget (MainWindow, "Icon_SaveProject"), projectActive);
    gtk_widget_set_sensitive (LookupWidget (MainWindow, "Icon_AddFileToProject"), projectActive);
    gtk_widget_set_sensitive (LookupWidget (MainWindow, "Icon_LaunchEditorForFile"), projectSelectionActive);
    gtk_widget_set_sensitive (LookupWidget (MainWindow, "Icon_ProjectOptions"), projectActive);
    gtk_widget_set_sensitive (LookupWidget (MainWindow, "Icon_Update"), projectActive);

    // Files Toolbar
    gtk_widget_set_sensitive (LookupWidget (MainWindow, "Icon_SaveFile"), fileActive);
    gtk_widget_set_sensitive (LookupWidget (MainWindow, "Icon_CloseFile"), fileActive);
    gtk_widget_set_sensitive (LookupWidget (MainWindow, "Icon_LaunchEditor"), fileActiveNamed);
    gtk_widget_set_sensitive (LookupWidget (MainWindow, "Icon_Print"), fileActiveNamed);

    // Project Menu
    gtk_widget_set_sensitive (LookupWidget (MainWindow, "ProjectMenu_Save"), projectActive);
    gtk_widget_set_sensitive (LookupWidget (MainWindow, "ProjectMenu_SaveAs"), projectActive);
    gtk_widget_set_sensitive (LookupWidget (MainWindow, "ProjectMenu_Close"), projectActive);
    gtk_widget_set_sensitive (LookupWidget (MainWindow, "ProjectMenu_ProjectOptions"), projectActive);
    gtk_widget_set_sensitive (LookupWidget (MainWindow, "ProjectMenu_AddFilesIntoProject"), projectActive);

    // File Menu
    gtk_widget_set_sensitive (LookupWidget (MainWindow, "FileMenu_ReOpen"), fileActive);
    gtk_widget_set_sensitive (LookupWidget (MainWindow, "FileMenu_Save"), fileActive);
    gtk_widget_set_sensitive (LookupWidget (MainWindow, "FileMenu_SaveAs"), fileActive);
    gtk_widget_set_sensitive (LookupWidget (MainWindow, "FileMenu_Close"), fileActive);
    gtk_widget_set_sensitive (LookupWidget (MainWindow, "FileMenu_CloseAll"), fileActive);
    gtk_widget_set_sensitive (LookupWidget (MainWindow, "FileMenu_AddCurrentFileToProject"), fileActive);

    // Selection Menu
    gtk_widget_set_sensitive (LookupWidget (MainWindow, "SelectionMenu"), projectSelectionActive);
    gtk_widget_set_sensitive (LookupWidget (MainWindow, "SelectionMenu_GetInfo"), FALSE);

    // View Menu
    gtk_widget_set_sensitive (LookupWidget (MainWindow, "ViewMenu_Files"), projectActive);
    gtk_widget_set_sensitive (LookupWidget (MainWindow, "ViewMenu_Procedures"), projectActive);
    gtk_widget_set_sensitive (LookupWidget (MainWindow, "ViewMenu_Tests"), projectActive);

    // Build Menu
    gtk_widget_set_sensitive (LookupWidget (MainWindow, "BuildMenu"), projectActive);

    UpdateSelectionMenuGreying ();
}

/* FindDirectoryRow : returns 0 if the pathFragment given matches the `name' of
	the given BalsaFile (if that BalsaFile is a BalsaDirectoryNode) */
gint FindDirectoryRow (PtrBalsaProjectEntry entry, char *pathFragment)
{
    return !(entry->nature == BalsaProjectEntry_Directory && strcmp (BALSA_FILE (entry->data)->name, pathFragment) == 0);
}

static void ConfirmAddImportPathForAddingFile_OnYes (GtkWidget * button, char *name_with_path)
{
    //  printf("confirmed %s\n", (char*)name_with_path);
    // Add path to the import path list
    char *lastSlash = strrchr (name_with_path, '/');

    if (lastSlash && (lastSlash > name_with_path))
    {
        int size = lastSlash - name_with_path;
        char *path = (char *) malloc (size + 1);

        strncpy (path, name_with_path, size);
        path[size] = 0;
        CurrentBalsaProject->importPath = g_list_append (CurrentBalsaProject->importPath, path);
    } else
    {
        // should never happen
        printfConsole ("Unexpected error in ConfirmAddImportPathForAddingFile_OnYes\n");
        return;
    }

    // Retry to add the file into the project
    AddFileCallback (name_with_path, NULL);
}

/* AddFileToProject : add a named file to the project `project' */
void AddFileToProject (PtrBalsaProject project, PtrBalsaFile file, char *CompletePathName)
{
    //todo: display this in the console:
    //  printf( "ajout de %s (%s)\n", file->name, CompletePathName);

    // Check the file doesn't already appear in the list
    {
        GList *files = project->files;

        for (; files; files = files->next)
            if (!strcmp (((PtrBalsaFile) (files->data))->name, file->name))
            {
                printfConsole ("File already in the project\n");
                return;
            }
    }

    // Check this is a valid file name
    // Check there is no name conflict between different paths
    {
        char *matchingPath = FindImportPathForFile (g_strdup_printf ("%s.balsa", file->name));

        if (!matchingPath)
        {
            char *lastSlash = NULL;

            if (CompletePathName)
                lastSlash = strrchr (CompletePathName, '/');
            if (lastSlash && (lastSlash > CompletePathName))
            {
                GtkWidget *dialogue;
                const gchar *buttons[] = { "gtk-yes", "gtk-cancel" };
                GtkSignalFunc handlers[] = {
                    (GtkSignalFunc) ConfirmAddImportPathForAddingFile_OnYes,
                    NULL
                };

                dialogue =
                  util_create_dialogue_with_buttons
                  ("This filename doesn't appear in any import path.\nDo you want to add its path into the list of import paths?",
                  2, buttons, 1, handlers, CompletePathName);
                gtk_window_set_position (GTK_WINDOW (dialogue), GTK_WIN_POS_MOUSE);
                gtk_widget_show (dialogue);
            } else
            {
                printfConsole_s
                  ("Warning! This filename (%s.balsa) doesn't appear in any import path. Action 'AddFileToProject' cancelled.\n", file->name);
            }
            return;
        }
    }

    project->files = g_list_append (project->files, file);
    project->dirty = TRUE;
    //  project->needToRebuildMakefile = TRUE;
    UpdateProjectTrees ();
}

/* AddImplementationToFile : add a named implementation to the file section `balsaFile', in the project `project' */
void AddImplementationToFile (PtrBalsaProject project, PtrBalsaFile balsaFile, PtrImplementation impl)
{
    balsaFile->info.balsaFile.implementations = g_list_append (balsaFile->info.balsaFile.implementations, impl);
    project->dirty = TRUE;
    project->needToRebuildMakefile = TRUE;
    UpdateProjectTrees ();
}

/* AddImplementationToTest : add a named implementation to the test section `balsaTest', in the project `project' */
void AddImplementationToTest (PtrBalsaProject project, PtrBalsaTest balsaTest, PtrImplementation impl)
{
    balsaTest->implementations = g_list_append (balsaTest->implementations, impl);
    project->dirty = TRUE;
    project->needToRebuildMakefile = TRUE;
    UpdateProjectTrees ();
}

/* AddBuiltinLibToFile : add a named builtinLib to the file section `balsaFile', in the project `project' */
void AddBuiltinLibToFile (PtrBalsaProject project, PtrBalsaFile balsaFile, PtrBuiltinLib lib)
{
    if (balsaFile->info.balsaFile.builtinLib)
    {
        fprintf (stderr, "Error AddBuiltinLibToFile: Trying to add more than 1 lib (Only 1 lib authorised)\n");
    }
    balsaFile->info.balsaFile.builtinLib = lib;
    project->dirty = TRUE;
    project->needToRebuildMakefile = TRUE;
    UpdateProjectTrees ();
}

/* SetEntryStringCallback : set the given entry to the string value passed */
gboolean SetEntryStringCallback (char *string, GtkWidget * entry)
{
    gtk_entry_set_text (GTK_ENTRY (entry), string);
    gtk_entry_set_position (GTK_ENTRY (entry), -1);

    return TRUE;                /* destroy dialogue */
}

void OnDialogueCancel (GtkObject *dialogue)
{
    gtk_widget_destroy (GTK_WIDGET (dialogue));
}

/* MakeDirectorySelectionDialogue : make a directory selection dialogue
	which calls callback `callback' on pressing OK. Destroys itself
	on destruction of the parent window `parent' (if any) and when either
	of OK or Cancel are pressed. `initialDirectory' is the starting 
	directory selection and displayed directory. */
GtkWidget *MakeDirectorySelectionDialogue (const char *title,
  const char *initialDirectory, FileSelectionCallback callback, gpointer callbackData,
  GtkSignalFunc changeDirCallBack, GtkWidget * parent)
{
    /* GtkWidget *dialogue = CreateWindow ("DirectoryDialogue"); */
	GtkWidget *dialogue = gtk_file_chooser_dialog_new (title,
		GTK_WINDOW (parent), GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER,
		GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
		NULL);
	GtkWidget *ok = gtk_dialog_add_button (GTK_DIALOG (dialogue), GTK_STOCK_OK, GTK_RESPONSE_ACCEPT);

    if (initialDirectory && *initialDirectory)
    {
        char *directory = (char *) g_malloc (strlen (initialDirectory) + 2);

        sprintf (directory, "%s/", initialDirectory);
        gtk_file_chooser_set_filename (GTK_FILE_CHOOSER (dialogue), directory);
        g_free (directory);
    }

    if (changeDirCallBack)
    {
        gtk_signal_connect (GTK_OBJECT (dialogue), "current_folder_changed",
        	GTK_SIGNAL_FUNC (changeDirCallBack), ok);
        gtk_signal_connect (GTK_OBJECT (dialogue), "selection_changed",
        	GTK_SIGNAL_FUNC (changeDirCallBack), ok);
        ((void (*)(GtkWidget *, GtkWidget *)) (changeDirCallBack)) (dialogue, ok);
    }

	if (gtk_dialog_run (GTK_DIALOG (dialogue)) == GTK_RESPONSE_ACCEPT)
	{
		char *filename;

		filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialogue));
		callback (filename, callbackData);
		g_free (filename);
	}

    gtk_widget_destroy (dialogue);
    return NULL;
}

/* MakeFileSelectionDialogue : like MakeDirectorySelectionDialogue
	but for file selection instead of directories. */
GtkWidget *MakeFileSelectionDialogue (FileSelectionDialogueType type, char *title, FileSelectionCallback callback, gpointer callbackData, GtkWidget * parent)
{
	GtkFileChooserAction action;
	const char *stock;

	switch (type)
    {
	case FileSelectionDialogue_Save:
		action = GTK_FILE_CHOOSER_ACTION_SAVE;
		stock = GTK_STOCK_SAVE;
		break;
	default:
		action = GTK_FILE_CHOOSER_ACTION_OPEN;
		stock = GTK_STOCK_OPEN;
		break;
    }

	GtkWidget *dialogue = gtk_file_chooser_dialog_new (title,
		GTK_WINDOW (parent), action,
		GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
		stock, GTK_RESPONSE_ACCEPT,
		NULL);

	if (gtk_dialog_run (GTK_DIALOG (dialogue)) == GTK_RESPONSE_ACCEPT)
	{
		char *filename;

		filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialogue));
		callback (filename, callbackData);
		g_free (filename);
	}

	gtk_widget_destroy (dialogue);
	return NULL;
}

/* On{Directory,File,Confirm,}Dialogue{OK,Yes,Cancel} : raw callbacks for Directory/File/ConfirmDialogue */
void OnDirectoryDialogueOK (GtkObject * dialogue)
{
    FileSelectionCallback callback = (FileSelectionCallback) gtk_object_get_data (dialogue, "OKCallback");
    char *directory = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialogue));

    gboolean closeDialogue = callback (directory,
      (gpointer) gtk_object_get_data (dialogue, "CallbackData"));

    if (closeDialogue)
        gtk_widget_hide (GTK_WIDGET (dialogue)); /* AB 2008-10-20 was destroy.  Don't understand the prob. */
}

void OnFileDialogueOK (GtkObject * dialogue)
{
    FileSelectionCallback callback = (FileSelectionCallback) gtk_object_get_data (dialogue,
      "OKCallback");
    char *file = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialogue));
    gboolean closeDialogue = callback (file, (gpointer) gtk_object_get_data (dialogue, "CallbackData"));

    if (closeDialogue)
        gtk_widget_hide (GTK_WIDGET (dialogue)); /* AB 2008-10-20 was destroy.  Don't understand the prob. */
}

/* MakeTopLevelWindows : make persistent windows on startup */
void MakeTopLevelWindows (void)
{
    MainWindow = CreateWindow ("MainWindow");
    MainWindowObject = GTK_OBJECT (MainWindow);

    gtk_widget_set_sensitive (LookupWidget (MainWindow, "SelectionMenu"), FALSE);
}

int OpenRecentButtonPressedCallback (GtkWidget *widget, GdkEventButton *event)
{
    if (event->button == 3)     //Right button pressed
        gtk_menu_popup (GTK_MENU (RecentProjectsMenu), NULL, NULL, NULL, NULL, 0, 0);

    /* Tell calling code that we have not handled this event; pass it on. */
    return FALSE;
}

void AllocateFixedResources (void)
{
    GdkColormap *colours;

    MessagesFont = gdk_font_load ("Monospace");

    colours = gdk_colormap_get_system (); //gtk_widget_get_colormap (MessagesText);
    Red.red = 65535;
    Red.blue = 0;
    Red.green = 0;
    if (!gdk_color_alloc (colours, &Red))
        g_error ("couldn't allocate colour");
    Green.red = 0;
    Green.blue = 0;
    Green.green = 35000;
    if (!gdk_color_alloc (colours, &Green))
        g_error ("couldn't allocate colour");
    Blue.red = 0;
    Blue.blue = 65535;
    Blue.green = 0;
    if (!gdk_color_alloc (colours, &Blue))
        g_error ("couldn't allocate colour");

    InitIcons ();
}

char *program_name;
void signal_FAULT (int sig)
{
    printf
      ("\n*************************************\n Fatal Signal (%d) received !\nIf you want to help, run the following command: \"gdb %s %d\", and send us the result of the \"backtrace full\" command\nThis program is paused during 2 minutes to let you some time to run gdb, and dies afterwards.\n",
      sig, program_name, getpid ());
    sleep (120);
    exit (1);
}

/* *** MAIN *** */

int main (int argc, char *argv[])
{
    program_name = argv[0];

    gtk_set_locale ();
    gtk_init (&argc, &argv);

    if (argc > 2)
    {
        fprintf (stderr, "                                        \n");
        fprintf (stderr, " |_  _ |  _ _    ._  _ ._   [ balsa-mgr: Balsa Project Manager ]\n");
        fprintf (stderr, " |_)(_\\|_/ (_\\ - |||(_|| `  (C) 1999-2010, Amulet Group\n");
        fprintf (stderr, "                     -'        \n");
        fprintf (stderr, "usage: balsa-manager [<project-directory>]\n");
        exit (1);
    }
    //    signal (SIGSEGV, signal_FAULT);
    signal (SIGILL, signal_FAULT);
    //Mac pb:    signal (SIGBUS, signal_FAULT);
    signal (SIGFPE, signal_FAULT);

    StartupPWD = g_get_current_dir ();
    gboolean interfaceFound = FALSE;
    char *balsahome = getenv ("BALSAHOME");

    MakeTopLevelWindow ();
    InitOptions ();
    ReadEnvironmentOptions ();
    gtk_widget_show (MainWindow);
    AllocateFixedResources ();
    init_workspace ();

    ExecutionManager_RemoveTmpFilesOfOldSessions ();

    gboolean tech_dir_found = FALSE;

    if (balsahome && *balsahome)
    {
        char *techDir2 = g_strdup_printf ("%s/share/tech", balsahome);

        if (FindBalsaTechnologies (techDir2))
            tech_dir_found = TRUE;
        g_free (techDir2);
    }
    if (!tech_dir_found && !FindBalsaTechnologies (BALSAHOME "/share/tech"))
    {
        printfConsole ("No valid Balsa technologies found\n");
    }

    if (argc > 1)
        OpenProjectCallback (argv[1]);
    else
    {
        FILE *f = fopen ("Project", "r");

        if (f)
        {
            fclose (f);
            OpenProjectCallback (".");
        }
    }

    gtk_main ();
    chdir (StartupPWD);

    return EXIT_SUCCESS;
}

static gboolean AddFileCallback (char *filename, gpointer userData)
{
    PtrBalsaFile newFile = NewBalsaFileWithName (filename);

    if (newFile)
        AddFileToProject (CurrentBalsaProject, newFile, filename);
    else
        printfConsole ("You can only add .balsa files (for the moment)\n");
    return FALSE;
}

void OnProjectMenu_AddFile (GtkMenuItem * menuitem)
{
    showAddFileDialogue ();
}

void OnBuildMenu_BuildMakefile (GtkMenuItem * button)
{
    GList *commandsList;
    char *command = generate_balsamd_command ();

    commandsList = g_list_append (NULL, command);
    ExecutionManager_RunCommandList (commandsList);
}

void OnProjectMenu_Update (GtkMenuItem * menuitem)
{
    // Check each file in the project:
    // - If the breeze exists: If balsa has been modified later than breeze, then recompile.
    GList *files = CurrentBalsaProject->files;
    GList *commandList = NULL;

    for (; files; files = files->next)
    {
        PtrBalsaFile file = files->data;
        char *filename = file->name;
        char *breezeFilename = ConvertToPathedBreezeFilename (filename);
        char *balsaFilename = ConvertToPathedBalsaFilename (filename);
        struct stat stat_breeze, stat_balsa;

        if (stat (breezeFilename, &stat_breeze) == 0)
        {                       // Breeze file exists
            stat (balsaFilename, &stat_balsa);
            if (stat_balsa.st_mtime > stat_breeze.st_mtime)
            {
                // balsa has been modified later than breeze => recompile.
                char *command = g_strdup_printf ("make -n %s", breezeFilename);

                commandList = g_list_append (commandList, command);
                //      printf ("Need to recompile %s\n", balsaFilename);
            }
        }
    }

    // Re-generate the project views (Files and Makefile)
    if (commandList)
    {
        char *command = g_strdup_printf ("~ExecuteFunction0arg %d",
          (int) UpdateProjectTrees);

        commandList = g_list_append (commandList, command);
        ExecutionManager_CheckBuildMakefileAndRunCommandList (commandList);
    } else
    {
        UpdateProjectTrees ();
    }
}

gboolean OnMainWindowDestroy (GtkWidget * widget, GdkEvent * event)
{
    gtk_main_quit ();
    return TRUE;
}

void OnProjectMenu_Quit_afterSaveConfirmation (GtkWidget * button)
{
    OnExecutionWindowDelete (NULL, NULL, (gpointer) 1);
}

void OnProjectMenu_Quit (GtkMenuItem * button)
{
    ConfirmSaveCurrentBalsaProjectAndExecute ((GtkSignalFunc) OnProjectMenu_Quit_afterSaveConfirmation);
}

gboolean OnMainWindowDelete (GtkWidget * widget, GdkEvent * event)
{
    OnProjectMenu_Quit (NULL);
    return TRUE;
}

void OnOptionsWindowShow (GtkWidget * widget)
{
}

/* *** OptionsDialogue END */

void OnProjectFileviewSelectRow (GtkWidget * tree, GtkCTreeNode * node, gint column)
{
    PtrBalsaProjectEntry entry = BALSA_PROJECT_ENTRY (gtk_ctree_node_get_row_data (GTK_CTREE (tree),
        GTK_CTREE_NODE (node)));

    ProjectFileViewSelectedEntry = entry;

    gtk_object_set_data (GTK_OBJECT (tree), "SelectedNode", (gpointer) node);
    gtk_widget_set_sensitive (LookupWidget (MainWindow, "SelectionMenu"), TRUE);

    /* set the `Details' column title to something better */
    gtk_clist_set_column_title (GTK_CLIST (tree), 1, (!entry ? "Details" : BalsaProjectEntryDetailsNames[(int) entry->nature]));

    /* auto-edit the file */
    {
        char *filename = NULL;
        int line = 0;

        switch (entry->nature)
        {
        case BalsaProjectEntry_File:
            if (BALSA_FILE (entry->data)->nature == BalsaFile_File)
                filename = ConvertToBalsaFilename (BALSA_FILE (entry->data)->name);
            break;

        case BalsaProjectEntry_Procedure:
            {
                char *procedure_name = BALSA_PROCEDURE (entry->data)->name;

                filename = ConvertToBalsaFilename (BALSA_PROCEDURE (entry->data)->associated_filename);
                line = GetProcedureLine (procedure_name, filename);
            }
            break;

        case BalsaProjectEntry_TestComponent:
            switch (BALSA_TESTCOMPONENT (entry->data)->nature)
            {
            case BalsaTestComponent_InputFromFile:
            case BalsaTestComponent_OutputToFile:
            case BalsaTestComponent_Memory:
            case BalsaTestComponent_OutputToStdout:
                filename = ExtractCommaSeparatedParam (BALSA_TESTCOMPONENT (entry->data)->value, 0);
                break;
            case BalsaTestComponent_Sync:
            case BalsaTestComponent_InputFromValue:
            case BalsaTestComponent_Undefined:
                break;
            }
            break;

        case BalsaProjectEntry_Implementation:
            break;

        case BalsaProjectEntry_BuiltinLib:
        {
            PtrBuiltinLib lib = BUILTINLIB (entry->data);
            if (lib->sourceFilenames)
                filename = lib->sourceFilenames->data;
        }
           break;

        default:
            break;
        }

        if (filename && !BEGINS_WITH (filename, "Memory("))
        {
            if (line)
                FileManager_OpenFileAtLine (filename, line);
            else
                FileManager_OpenFile (filename);
        }
    }

    UpdateMainWindowGreying ();
}

void OnProjectFileviewUnselectRow (GtkWidget * tree, GtkCTreeNode * node, gint column)
{
    gtk_object_set_data (GTK_OBJECT (tree), "SelectedNode", NULL);

    /* set the `Details' column title back to "Details" */
    gtk_clist_set_column_title (GTK_CLIST (tree), 1, "Details");

    ProjectFileViewSelectedEntry = NULL;
    UpdateMainWindowGreying ();
}

gboolean OnProjectFileviewMousePressed (GtkWidget * tree, GdkEventButton * event)
{
    if (event->button != 1)
    {
        int row, column;

        gtk_clist_get_selection_info (GTK_CLIST (tree), event->x, event->y, &row, &column);
        gtk_clist_select_row (GTK_CLIST (tree), row, 0);
    }

    if ((event->button == 3) && ProjectFileViewSelectedEntry) //Right button pressed
    {
        GtkWidget *menu = LookupWidget (MainWindow, "SelectionMenu_menu");

        gtk_menu_popup (GTK_MENU (menu), NULL, NULL, NULL, NULL, 0, 0);
    } else if ((event->button == 2) && ProjectFileViewSelectedEntry) //Middle button pressed on test -> execute breeze-sim-ctrl
    {
        switch (ProjectFileViewSelectedEntry->nature)
        {
        case BalsaProjectEntry_Test:
            {
                PtrBalsaTest test = BALSA_TEST (ProjectFileViewSelectedEntry->data);
                char *makeName = g_strdup_printf ("sim-win-%s", test->name);

                MakefileTable_MakeCallback (NULL, makeName);
            }
            break;
        case BalsaProjectEntry_File:
            {
                char *makeName = ConvertToPathedBreezeFilename (BALSA_FILE (ProjectFileViewSelectedEntry->data)->name);

                MakefileTable_MakeCallback (NULL, makeName);
            }
            break;
        default:;
        }
    }
    return FALSE;
}

PtrBalsaProcedure NewBalsaProcedure (char *name, char *associated_filename)
{
    PtrBalsaProcedure proc = (PtrBalsaProcedure) g_malloc (sizeof (BalsaProcedure));

    proc->name = name;
    proc->associated_filename = associated_filename;
    proc->treeNode = NULL;
    return proc;
}

GList *FindProceduresInBalsaFile (const char *filename)
{
    FILE *f = fopen (ConvertToPathedBalsaFilename (filename), "r");
    GList *procedures = NULL;

    if (!f)
        return NULL;

    while (!feof (f))
    {
        char buf[1000];
        int keyword_size;

        if (!fgets (buf, 1000, f))
            continue;

        if (!strncmp (buf, "procedure ", keyword_size = 10) || !strncmp (buf, "function ", keyword_size = 9))
        {
            char *procname = GetFirstWord (buf + keyword_size);

            procedures = g_list_append (procedures, NewBalsaProcedure (procname, g_strdup (filename)));
        }
    }

    fclose (f);
    return procedures;
}

GList *FindProceduresInBreezeFile (const char *filename)
{
    GList *procedures = NULL;
    GList *tmp;
    struct BreezeFile *breezeFile = breezeInitParse (GetWithoutExtension (filename));

    if (breezeFile)
    {
        GList *parts = getBreezeParts (breezeFile);

        for (tmp = parts; tmp; tmp = g_list_next (tmp))
        {
            struct BreezePart *part = tmp->data;
            char *proc_name = g_strdup (getBreezePartName (part));

            procedures = g_list_append (procedures, NewBalsaProcedure (proc_name, g_strdup (filename)));
        }
    }

    return procedures;
}

static GList *FindFileProcedures (const char *filename)
{
    GList *proceduresInBreezeFile = FindProceduresInBreezeFile (filename);

    if (proceduresInBreezeFile == NULL)
    {
        GList *proceduresInBalsaFile = FindProceduresInBalsaFile (filename);

        return proceduresInBalsaFile;
    } else
        return proceduresInBreezeFile;
}

GPtrArray *FindTestPortsForProcedureInBreezeFile (const char *filename, const char *procedureName)
{
    GPtrArray *testPorts = NULL;
    GList *tmp;
    struct BreezeFile *breezeFile = breezeInitParse (GetWithoutExtension (ConvertToPathedBalsaFilename (filename)));

    if (breezeFile)
    {
        struct BreezePart *part = getBreezePartByName (breezeFile, procedureName);

        if (part)
        {
            GList *ports = getBreezePartPortsList (part);

            testPorts = g_ptr_array_new ();

            for (tmp = ports; tmp; tmp = g_list_next (tmp))
            {
                struct BreezePartPortsPort *port = tmp->data;
                char *name = getBreezePartPortsPortName (port);
                enum BreezePartPortsPortType type = getBreezePartPortsPortType (port);
                BalsaPort2Nature portNature;
                PtrBalsaPort2 newPort;

                switch (type)
                {
                case PassiveSyncPort:
                case ActiveSyncPort:
                    portNature = BalsaPort2_Sync;
                    break;
                case PassiveOutputPort:
                case ActiveOutputPort:
                    portNature = BalsaPort2_Output;
                    break;
                case PassiveInputPort:
                case ActiveInputPort:
                    portNature = BalsaPort2_Input;
                    break;
                }

                if (strcmp (name, "activate"))
                {
                    if (!getBreezePartPortsPortIsArray (port))
                    {
                        newPort = NewBalsaPort2 (portNature, name);
                        g_ptr_array_add (testPorts, newPort);
                    } else
                    {
                        int low = getBreezePartPortsPortArrayIndexLow (port);
                        int size = getBreezePartPortsPortArraySize (port);
                        int i;

                        for (i = low; i < low+size; i++)
                        {
                            newPort = NewBalsaPort2 (portNature, g_strdup_printf ("%s[%d]", name, i));
                            g_ptr_array_add (testPorts, newPort);
                        }
                    }
                }
            }
        }
    }

    return testPorts;
}

GPtrArray *FindTestPortsForProcedure (const char *filename, const char *procedureName)
{
    GPtrArray *testPortsForProceduresInBreezeFile = FindTestPortsForProcedureInBreezeFile (filename,
      procedureName);

    return testPortsForProceduresInBreezeFile;
}

static int GetProcedureLine (char *procedureName, char *filename)
{
    int line = 0;
    FILE *f = fopen (ConvertToPathedBalsaFilename (filename), "r");

    if (!f)
    {
        printfConsole_s ("ERROR! Impossible to open %s\n", filename);
        return 0;
    }

    while (!feof (f))
    {
        char buf[1000];
        int keyword_size;

        if (!fgets (buf, 1000, f))
            continue;
        line++;

        if (!strncmp (buf, "procedure ", keyword_size = 10) || !strncmp (buf, "function ", keyword_size = 9))
            if (!strncmp (buf + keyword_size, procedureName, strlen (procedureName)))
            {
                char nextChar = buf[keyword_size + strlen (procedureName)];

                if ((nextChar == '(') || (nextChar == ' ') || (nextChar == '\n'))
                {
                    fclose (f);
                    return line;
                }
            }
    }

    fclose (f);
    return 0;
}

gboolean IsProcedureContainingTest (PtrBalsaProcedure procedure, GList * tests)
{
    for (; tests; tests = tests->next)
        if (!strcmp (procedure->name, ((PtrBalsaTest) tests->data)->procName))
            return TRUE;
    return FALSE;
}

gboolean IsFileContainingTest (PtrBalsaFile file)
{
    if (file->info.balsaFile.testFixtures)
        return TRUE;
    else
        return FALSE;
}

void AddTestToProjectFilesViewTree (PtrBalsaTest test, GtkCTreeNode * matchingNode)
{
    GtkWidget *TreeWidget = LookupWidget (MainWindow, "TreeFilesView");
    GtkCTreeNode *testNode;
    gchar *text[2];

    text[0] = g_strdup (test->name);
    text[1] = g_strdup ("test");
    testNode = gtk_ctree_insert_node (GTK_CTREE (TreeWidget), matchingNode, NULL, text, 4, NULL, NULL, NULL, NULL, FALSE, TRUE);

    gtk_ctree_node_set_row_data (GTK_CTREE (TreeWidget), testNode, (gpointer) NewBalsaProjectEntry (BalsaProjectEntry_Test, (gpointer) test));

    UpdateTestFixtureCTreeNode (GTK_CTREE (TreeWidget), testNode);
}

void AddTestsToProjectFilesViewTree (GList * tests, GList * procedures)
{
    for (; tests; tests = tests->next)
    {
        PtrBalsaTest test = (PtrBalsaTest) tests->data;

        /* Find the procedure which is matching the test */
        GList *matchingProc;
        GtkCTreeNode *matchingNode;

        for (matchingProc = procedures; matchingProc; matchingProc = matchingProc->next)
            if (!strcmp (((PtrBalsaProcedure) matchingProc->data)->name, test->procName))
                break;

        if (matchingProc)
            matchingNode = ((PtrBalsaProcedure) matchingProc->data)->treeNode;
        else
            matchingNode = NULL;

        /* Add the test into the tree */
        AddTestToProjectFilesViewTree (test, matchingNode);
    }
}
void AddImplementationsToProjectFilesViewTree (GtkCTreeNode * nodeParent, GList * implementations)
{
    GtkWidget *TreeWidget = LookupWidget (MainWindow, "TreeFilesView");
    gchar *text[2];
    GtkCTreeNode *matchingNode;

    for (; implementations; implementations = implementations->next)
    {
        PtrImplementation impl = (PtrImplementation) implementations->data;

        text[0] = g_strdup (impl->name);
        text[1] = g_strdup ("");

        matchingNode =
          gtk_ctree_insert_node (GTK_CTREE (TreeWidget),
          nodeParent, NULL, text, 4, ImplementationPixmap, ImplementationMask, ImplementationPixmap, ImplementationMask, FALSE, TRUE);

        gtk_ctree_node_set_row_data (GTK_CTREE (TreeWidget), matchingNode, (gpointer) NewBalsaProjectEntry (BalsaProjectEntry_Implementation, impl));
    }
}

void AddBuiltinLibToProjectFilesViewTree (GtkCTreeNode * nodeParent, PtrBuiltinLib lib)
{
    if (!lib)
        return;

    GtkWidget *TreeWidget = LookupWidget (MainWindow, "TreeFilesView");
    gchar *text[2];
    GtkCTreeNode *matchingNode;

    text[0] = g_strdup (lib->name);
    text[1] = g_strdup ("builtin lib");

    matchingNode =
      gtk_ctree_insert_node (GTK_CTREE (TreeWidget),
      nodeParent, NULL, text, 4, BuiltinLibPixmap, BuiltinLibMask, BuiltinLibPixmap, BuiltinLibMask, FALSE, TRUE);

    gtk_ctree_node_set_row_data (GTK_CTREE (TreeWidget), matchingNode, (gpointer) NewBalsaProjectEntry (BalsaProjectEntry_BuiltinLib, lib));

    GList *tmp = lib->sourceFilenames;

    for (; tmp; tmp = tmp->next)
    {
        char *sourceFilename = tmp->data;

        text[0] = sourceFilename;
        text[1] = "";
        GtkCTreeNode *filenameNode = gtk_ctree_insert_node (GTK_CTREE (TreeWidget), matchingNode, NULL,
          text, 4, NULL, NULL, NULL, NULL, TRUE,
          TRUE);

        gtk_ctree_node_set_row_data (GTK_CTREE (TreeWidget), filenameNode,
          (gpointer) NewBalsaProjectEntry (BalsaProjectEntry_BuiltinLib, (gpointer) lib));
    }

    gtk_ctree_expand (GTK_CTREE (TreeWidget), matchingNode);
}

void AddProcedureToProjectFilesViewTree (GtkCTreeNode * nodeParent, PtrBalsaProcedure procedure)
{
    GtkWidget *TreeWidget = LookupWidget (MainWindow, "TreeFilesView");
    gchar *text[2];
    GtkCTreeNode *matchingNode;

    //  char *filename = file->name;
    //  char *prefix = FindPrefixPath (filename, CurrentBalsaProject->importPath);

    text[0] = g_strdup (procedure->name);
    text[1] = g_strdup ("procedure");
    // todo: display the right icon
    matchingNode =
      gtk_ctree_insert_node (GTK_CTREE (TreeWidget), nodeParent,
      NULL, text, 4, ProcedurePixmap, ProcedureMask, ProcedurePixmap, ProcedureMask, FALSE, TRUE);
    procedure->treeNode = matchingNode;

    gtk_ctree_node_set_row_data (GTK_CTREE (TreeWidget), matchingNode, (gpointer) NewBalsaProjectEntry (BalsaProjectEntry_Procedure, procedure));
}

void AddFileToProjectFilesViewTree (PtrBalsaFile file)
{
    GtkWidget *TreeWidget = LookupWidget (MainWindow, "TreeFilesView");
    GtkCTreeNode *nodeParent = NULL;
    gchar *text[2];
    GtkCTreeNode *matchingNode;
    GList *procedures, *procedure;

    //    char *filename = GetWithoutExtension (GetWithoutPath (file->name));
    const char *filename = GetWithoutExtension (file->name);
    char *filename_balsa = ConvertToBalsaFilename (file->name);
    char *filename_breeze = ConvertToPathedBreezeFilename (file->name);
    char *prefix = FindImportPathForFile (filename_balsa);

    //  printf("AddFileToProjectFilesViewTree of %s (%s,%s)\n", file->name, filename, prefix);
    if (!prefix)
        printfConsole_s
          ("Oops... The specified file (%s) doesn't appear in any imported path. We continue anyway, but you'll have to take care of that\n",
          file->name);

    text[0] = g_strdup (filename_balsa ? filename_balsa : "---"); //todo:change the ---
    text[1] = g_strdup (prefix ? prefix : "---"); //todo:change the ---
    // todo: display the right icon
    //   see   AddFileToProject (CurrentBalsaProject, BALSA_FILE (files->data));
    if (doesFileExist (filename_breeze))
        matchingNode =
          gtk_ctree_insert_node (GTK_CTREE (TreeWidget), nodeParent,
          NULL, text, 4, BalsaBlockPixmap, BalsaBlockMask, BalsaBlockPixmap, BalsaBlockMask, FALSE, TRUE);
    else
        matchingNode =
          gtk_ctree_insert_node (GTK_CTREE (TreeWidget), nodeParent,
          NULL, text, 4, BalsaBlockWithExclamationPixmap,
          BalsaBlockWithExclamationMask, BalsaBlockWithExclamationPixmap, BalsaBlockWithExclamationMask, FALSE, TRUE);

    gtk_ctree_node_set_row_data (GTK_CTREE (TreeWidget), matchingNode, (gpointer) NewBalsaProjectEntry (BalsaProjectEntry_File, file));

    /* Add procedures to the tree */
    procedures = FindFileProcedures (filename);
    for (procedure = procedures; procedure; procedure = procedure->next)
        if ((ViewProceduresOption == 1) || IsProcedureContainingTest (procedure->data, file->info.balsaFile.testFixtures))
            AddProcedureToProjectFilesViewTree (matchingNode, procedure->data);

    /* Insert the tests into the tree */
    if (file->nature == BalsaFile_File)
        if (ViewTestsOption == 1)
        {
            AddTestsToProjectFilesViewTree (file->info.balsaFile.testFixtures, procedures);
            // AddTestsToProjectFileTree (project, matchingNode, file->info.balsaFile.testFixtures);
            AddBuiltinLibToProjectFilesViewTree (matchingNode, file->info.balsaFile.builtinLib);
            AddImplementationsToProjectFilesViewTree (matchingNode, file->info.balsaFile.implementations);
        }
}

void UpdateProjectName (void)
{
    // todo: free the old label
    GtkWidget *ProjectsFrame = LookupWidget (MainWindow, "ProjectsFrame");

    gtk_frame_set_label (GTK_FRAME (ProjectsFrame),
      g_strdup_printf ("Project %c %s", (CurrentBalsaProject->dirty || CurrentBalsaProject->nb_dirty_files) ? '*' : '-', CurrentBalsaProject->name));
}

int compare_ptrbalsafile_names (PtrBalsaFile file1, PtrBalsaFile file2)
{
    return strcmp (file1->name, file2->name);
}

/* Add files to file view */
void UpdateProjectTreeFilesView (void)
{
    GtkWidget *TreeWidget = LookupWidget (MainWindow, "TreeFilesView");

    gtk_clist_freeze (GTK_CLIST (TreeWidget));

    /* Remember the scrollbar's position */
    GtkWidget *scrolledWindow = LookupWidget (MainWindow, "TreeFilesScrolledWindow");
    GtkAdjustment *vadjustment = gtk_scrolled_window_get_vadjustment (GTK_SCROLLED_WINDOW (scrolledWindow));
    gfloat vposition = vadjustment->value;

    /* Clear the tree */
    GtkCList *list = GTK_CLIST (TreeWidget);
    int rowCount = list->rows;
    int i;

    for (i = 0; i < rowCount; i++)
    {
        PtrBalsaProjectEntry entry = BALSA_PROJECT_ENTRY (gtk_clist_get_row_data (list, i));

        if (entry)
        {
            /* make sure we don't kill the contents */
            entry->data = NULL;
            g_free (entry);
        }
    }
    gtk_clist_clear (list);

    /* Sort the file list */
    {
        CurrentBalsaProject->files = g_list_sort (CurrentBalsaProject->files, (GCompareFunc) compare_ptrbalsafile_names);
    }

    /* Add files to the tree */
    {
        GList *files;

        for (files = CurrentBalsaProject->files; files; files = files->next)
            if ((ViewFilesOption == 1) || IsFileContainingTest (files->data))
                AddFileToProjectFilesViewTree (files->data);
    }

    gtk_clist_thaw (GTK_CLIST (TreeWidget));

    /* Restore the scrollbar's position */
    gtk_adjustment_set_value (vadjustment, vposition);

    /* TODO: re-select the right element in the tree */
    ProjectFileViewSelectedEntry = NULL;
    UpdateMainWindowGreying ();
}

void UpdateProjectTrees (void)
{
    if (! CurrentBalsaProject) return;

    UpdateProjectName ();
    UpdateProjectTreeFilesView ();

    {
        GtkWidget *ProjectNotebook = LookupWidget (MainWindow, "ProjectNotebook");
        int page = gtk_notebook_get_current_page (GTK_NOTEBOOK (ProjectNotebook));

        if (page == 2)
            UpdateMakefileTable ();
    }
}

void SaveAllFilesOfCurrentProject (void)
{
    GList *files = CurrentBalsaProject->files;

    for (; files; files = files->next)
    {
        PtrBalsaFile file = files->data;

        FileManager_SaveFileIfOpenedAndIfNeeded (ConvertToPathedBalsaFilename (file->name));
    }
}

static gboolean OpenProjectCallback (const char *directory)
{
    PtrBalsaProject project = ReadBalsaProjectFile (directory);

    if (project)
    {
        SetAsCurrentBalsaProject (project);

        UpdateProjectTrees ();
        //      UpdateMakefileList();
        AddRecentProject (CurrentBalsaProject->directory);
        UpdateMainWindowGreying ();
        return TRUE;
    } else
    {
        printfConsole_s ("Error: Can't find the Project file %s/Project\n", directory);
        return FALSE;
    }
}
static void OpenProjectWindow_DirectoryChanged (GtkWidget * dialogue, GtkWidget *ok)
{
	const char *choice = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialogue));

	if (choice)
	{
		char *filename = g_strdup_printf ("%s/Project", choice);

		FILE *f = fopen (filename, "r");
		g_free (filename);

		if (f)
		{
			gtk_widget_set_sensitive (ok, TRUE);
			fclose (f);
		} else
		{
			gtk_widget_set_sensitive (ok, FALSE);
		}
	}
}
void OnProjectMenu_Open_afterSaveConfirmation (GtkWidget * button)
{
    MakeDirectorySelectionDialogue
      ("Select the Project Directory you want to open", NULL,
      (FileSelectionCallback) OpenProjectCallback, NULL, GTK_SIGNAL_FUNC (OpenProjectWindow_DirectoryChanged), NULL);
}

void OnProjectMenu_Open (GtkMenuItem * button)
{
    ConfirmSaveCurrentBalsaProjectAndExecute ((GtkSignalFunc) OnProjectMenu_Open_afterSaveConfirmation);
}

gboolean OpenProjectMenu_SaveAs_Callback (const char *directory)
{
    CurrentBalsaProject->directory = g_strdup (directory);
    OnProjectMenu_Save (NULL);
    FileManager_RefreshAllDisplayedNames ();
    return TRUE;
}

void OnProjectMenu_SaveAs (GtkMenuItem * button)
{
    MakeDirectorySelectionDialogue
      ("Select the directory where you want to save the Project", NULL,
      (FileSelectionCallback) OpenProjectMenu_SaveAs_Callback, NULL, NULL, NULL);
}

void OnProjectMenu_Save (GtkMenuItem * button)
{
    if (CurrentBalsaProject->directory && *(CurrentBalsaProject->directory))
    {
        // todo:      PushStatusBarMessage (StatusBar, "Saved project file: %s", CurrentBalsaProject->directory);
        WriteBalsaProjectFile (CurrentBalsaProject->directory, CurrentBalsaProject);
        FileManager_SaveAllNamedFiles (); //      SaveAllFilesOfCurrentProject();
        AddRecentProject (CurrentBalsaProject->directory);
        UpdateProjectTrees ();
    } else
        OnProjectMenu_SaveAs (button);
}

void OnProjectMenu_Close_afterSaveConfirmation (GtkWidget * button)
{
    CloseCurrentBalsaProject ();
}

void OnProjectMenu_Close (GtkMenuItem * button)
{
    ConfirmSaveCurrentBalsaProjectAndExecute ((GtkSignalFunc) OnProjectMenu_Close_afterSaveConfirmation);
}

char *generate_balsamd_command (void)
{
    char *cmd = g_strdup_printf ("%s -b -p \"%s\"",
      (CurrentBalsaProject->simulationSystem == breezesim) ? "balsa-make-makefile" : "UNKNOWN",
      CurrentBalsaProject->directory);

    return cmd;
}

void OnProjectMenu_AddFilesIntoProject (GtkMenuItem * button)
{
    OnProjectMenu_AddFile (NULL);
}

void OnProjectToolbar_LaunchEditor (GtkMenuItem * button)
{
    /* Run the editor */
    char *args[5] = { NULL, NULL, NULL, NULL };

    if (EnvironmentOptions.editor && EnvironmentOptions.editor[0])
        args[0] = EnvironmentOptions.editor;
    else
    {
        char *editor = getenv ("EDITOR");

        if (editor)
            args[0] = g_strdup_printf ("xterm -e %s", editor);
        else
            args[0] = g_strdup ("emacs");
    }

    {
        switch (ProjectFileViewSelectedEntry->nature)
        {
        case BalsaProjectEntry_File:
            if (BALSA_FILE (ProjectFileViewSelectedEntry->data)->nature == BalsaFile_File)
                args[1] = ConvertToPathedBalsaFilename (BALSA_FILE (ProjectFileViewSelectedEntry->data)->name);
            break;

        case BalsaProjectEntry_Procedure:
            {
                char *procedure_name = BALSA_PROCEDURE (ProjectFileViewSelectedEntry->data)->name;
                char *filename = ConvertToPathedBalsaFilename (BALSA_PROCEDURE (ProjectFileViewSelectedEntry->data)->associated_filename);
                int line = GetProcedureLine (procedure_name,
                  filename);

                args[1] = g_strdup_printf ("+%d", line);
                args[2] = filename;
            }
            break;

        case BalsaProjectEntry_Test:
        case BalsaProjectEntry_TestComponent:
            OnSelectionMenu_Edit (NULL);
            break;

        default:
            break;
        }
    }

    if (args[1])
    {
        chdir (CurrentBalsaProject->directory);
        RunCommandWithoutOutput (args[0], args);
    }
}

void OnSelectionMenu_GetInfo (GtkMenuItem * button)
{
    //TODO
    PtrBalsaProjectEntry entry = ProjectFileViewSelectedEntry;

    switch (entry->nature)
    {
    case BalsaProjectEntry_File:
        break;
    case BalsaProjectEntry_Directory:
        break;
    case BalsaProjectEntry_Procedure:
        break;
    case BalsaProjectEntry_Test:
        break;
    case BalsaProjectEntry_TestComponent:
        break;
    default:
        break;
    }
}

gboolean disable_filenameEntry = FALSE;
void OnTestFilenameEntryChanged (GtkEditable * editable)
{
    if (disable_filenameEntry)
        return;

    //    GtkWidget *dialogue = gtk_widget_get_toplevel (GTK_WIDGET (editable));
    //    GtkCombo *procNameCombo = GTK_COMBO (gtk_object_get_data (GTK_OBJECT (dialogue), "TestProcNameCombo"));

    /* Fill the ProcNames combo box */
    {
        GList *procNames = NULL;
        const char *selectedFilename = gtk_entry_get_text (GTK_ENTRY (editable));
        GList *procedures = FindFileProcedures (selectedFilename);

        for (; procedures; procedures = procedures->next)
            procNames = g_list_append (procNames, g_strdup (BALSA_PROCEDURE (procedures->data)->name));

        if (!procNames)
            procNames = g_list_append (procNames, g_strdup ("<no procedures>"));

        //        gtk_combo_set_popdown_strings (procNameCombo, procNames);
    }
    /* Select the right procName in the combo box => it will call OnTestProcNameEntryChanged(...) */
    {
    }
}

void OnTestProcNameEntryChanged (GtkEditable * editable)
{
    GtkWidget *dialogue = gtk_widget_get_toplevel (GTK_WIDGET (editable));
    GtkCList *testPortList = GTK_CLIST (LookupWidget (dialogue, "TestPortList"));
    GtkEntry *filenameEntry = GTK_ENTRY (LookupWidget (dialogue, "TestFilenameEntry"));

    /* Fill the TestPort list */
    const char *selectedFilename = gtk_entry_get_text (filenameEntry);
    const char *selectedProcName = gtk_entry_get_text (GTK_ENTRY (editable));

    GtkButton *breezeWarningButton = GTK_BUTTON (LookupWidget (dialogue, "CompileSBreezeFileButton"));

    if (doesFileExist (ConvertToPathedBreezeFilename (selectedFilename)))
        gtk_widget_hide (GTK_WIDGET (breezeWarningButton));
    else                        // no .breeze file
        gtk_widget_show (GTK_WIDGET (breezeWarningButton));

    // Update the ports
    CreateNewComponents (testPortList, selectedFilename, selectedProcName);
}

void OnSBreezeWarningButtonClicked (GtkWidget * widget)
{
    GtkWidget *dialogue = gtk_widget_get_toplevel (GTK_WIDGET (widget));
    GtkEntry *filenameEntry = GTK_ENTRY (LookupWidget (dialogue, "TestFilenameEntry"));
    GtkEntry *procNameEntry = GTK_ENTRY (LookupWidget (dialogue, "TestProcNameEntry"));
    const char *filename = gtk_entry_get_text (filenameEntry);
    const char *breezeFilename = ConvertToPathedBreezeFilename (filename);

    GList *commandList = NULL;

    /* Execute the appropriate make function */
    char *firstCommand = g_strdup_printf ("make -n %s", breezeFilename);
    char *nextCommand = g_strdup_printf ("~ExecuteFunction2args %p %p %p",
      OnTestProcNameEntryChanged,
      GTK_EDITABLE (procNameEntry), NULL);

    commandList = g_list_append (commandList, firstCommand);
    commandList = g_list_append (commandList, nextCommand);

    ExecutionManager_CheckBuildMakefileAndRunCommandList (commandList);
}

int FileDialogFindNextBalsaFilenameRow (GList * row_list, const char *filename, int direction)
{
    // Find the row corresponding to the specified filename (if exists)
    GList *row_elt, *row_elt_found;

    for (row_elt = row_list; row_elt; row_elt = row_elt->next)
    {
        GtkCListRow *row = GTK_CLIST_ROW (row_elt);
        GtkCell *cell = row->cell;
        char *textCell = cell->u.text;

        if (!strcmp (textCell, filename))
            break;
    }
    if (!row_elt)
        row_elt = row_list;
    row_elt_found = row_elt;

    // Find the next .balsa filename
    for (;;)
    {
        char *text;

        if (direction == 1)
        {
            row_elt = row_elt->next;
            if (row_elt == NULL)
                row_elt = row_list;
        } else
        {
            row_elt = row_elt->prev;
            if (row_elt == NULL)
                row_elt = g_list_last (row_list);
        }

        if (row_elt == row_elt_found)
            break;
        text = GTK_CLIST_ROW (row_elt)->cell->u.text;
        if (!strcmp (text + strlen (text) - 6, ".balsa"))
            break;
    }

    return g_list_index (row_list, row_elt->data);
}

gboolean OnDirectoryDialogueKeyPressEvent (GtkWidget * widget, GdkEventKey * event)
{
    if (event->keyval == GDK_Escape)
        OnDialogueCancel (GTK_OBJECT (widget));
    return FALSE;
}

gboolean OnFileDialogueKeyPressEvent (GtkWidget * widget, GdkEventKey * event)
{
    if (event->keyval == GDK_Escape)
        OnDialogueCancel (GTK_OBJECT (widget));

/* AB2007
    else if ((event->keyval == GDK_Down) || (event->keyval == GDK_Up))
    {
        GtkWidget *dialogue = gtk_widget_get_toplevel (widget);
        GtkCList *file_list = GTK_CLIST (GTK_FILE_SELECTION (dialogue)->file_list);
        GtkEntry *selection_entry = GTK_ENTRY (GTK_FILE_SELECTION (dialogue)->selection_entry);
        const char *selectedFilename = gtk_entry_get_text (selection_entry);

        if (file_list->row_list)
        {
            int row = FileDialogFindNextBalsaFilenameRow (file_list->row_list,
              selectedFilename,
              (event->keyval == GDK_Down) ? 1 : -1);

            gtk_clist_moveto (file_list, row, 0, 0.5, -1);
            gtk_clist_select_row (file_list, row, 0);
        }

        gtk_widget_grab_focus (GTK_WIDGET (selection_entry));
    }
*/
    return FALSE;
}

gboolean OnConsoleWindowDelete (GtkWidget * widget, GdkEvent * event)
{
    gtk_widget_hide (workSpace.consoleWindow->window);
    return TRUE;
}

gboolean OnAuxillaryWindow_KeyPressed (GtkWidget * widget, GdkEventKey * event)
{
    if (event->state & 4)       // CTRL key
    {
        if ((event->keyval == GDK_Q) || (event->keyval == GDK_q))
            OnProjectMenu_Quit (NULL);

        if ((event->keyval == GDK_C) || (event->keyval == GDK_c))
            OnViewMenu_Console_DisplayHide (NULL);

        if ((event->keyval == GDK_E) || (event->keyval == GDK_e))
            OnViewMenu_ExecutionWindow_DisplayHide (NULL);
    }
    return TRUE;
}
