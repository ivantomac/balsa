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

	`menus.c'
	Menus!
*/

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "aboutdialogue.h"
#include "builtinliboptsdialogue.h"
#include "commands.h"
#include "executionmanager.h"
#include "file.h"
#include "filemanager.h"
#include "imploptsdialogue.h"
#include "main.h"
#include "makefiletable.h"
#include "menus.h"
#include "paths.h"
#include "project.h"
#include "projopts.h"
#include "support.h"
#include "testopts.h"
#include "utils.h"
#include "workspace.h"

void OnFileMenu_New (GtkMenuItem * button)
{
    FileManager_NewFile ();
}

void OnFileMenu_Open (GtkMenuItem * button)
{
    FileManager_OpenFile (NULL);
}

void OnFileMenu_ReOpen (GtkMenuItem * button)
{
    FileManager_ReOpenFile ();
}

void OnFileMenu_Save (GtkMenuItem * button)
{
    FileManager_SaveFile ();
}

void OnFileMenu_SaveAs (GtkMenuItem * button)
{
    FileManager_SaveFileAs ();
}

void OnFileMenu_Close (GtkMenuItem * button)
{
    FileManager_CloseFile ();
}

void OnFileMenu_CloseAll (GtkMenuItem * button)
{
    FileManager_CloseAllFiles ();
}

void OnFileMenu_AddCurrentFileToProject (GtkMenuItem * button)
{
    char *filename = FileManager_GetCurrentFileName ();

    if (filename)
    {
        PtrBalsaFile newFile = NewBalsaFile ();

        newFile->name = GetWithoutPath (GetWithoutExtension (filename));
        AddFileToProject (CurrentBalsaProject, newFile, NULL); //todo: maybe change the NULL into the complete path+name, if the file CAN be absent from the imported paths.
    }
}

void OnFileMenu_LaunchEditor (GtkMenuItem * button)
{
    char *filename = FileManager_GetCurrentFileName ();

    if (filename)
    {
        int line = FileManager_GetCurrentLineNumber ();
        char *args[4] = { NULL, NULL, NULL, NULL };

        if (EnvironmentOptions.editor && EnvironmentOptions.editor[0])
            args[0] = EnvironmentOptions.editor;
        else
        {
            char *editor;

            if ((editor = (char *) getenv ("EDITOR")) != NULL)
                args[0] = editor;
            else
                args[0] = g_strdup ("emacs");
        }
        if (line > 0)
        {
            args[1] = g_strdup_printf ("+%d", line);
            args[2] = filename;
        } else
            args[1] = filename;

        if (CurrentBalsaProject)
            chdir (CurrentBalsaProject->directory);
        RunCommandWithoutOutput (args[0], args);
    }
}

void OnFileMenu_Print (GtkMenuItem * button)
{
    char *filename = FileManager_GetCurrentFileName ();

    if (filename)
    {
//        int line = FileManager_GetCurrentLineNumber ();
        char *args[4] = { NULL, NULL, NULL, NULL };

        if (EnvironmentOptions.PrintCommand && EnvironmentOptions.PrintCommand[0])
            args[0] = EnvironmentOptions.PrintCommand;
        else
            args[0] = g_strdup ("lpr");

        args[1] = filename;

        if (CurrentBalsaProject)
            chdir (CurrentBalsaProject->directory);
        RunCommandWithoutOutput (args[0], args);
    }
}

void OnHelpMenu_About (GtkMenuItem * button)
{
    GtkWidget *dialogue = get_workspace_aboutDialogue_dialogue ();

    gtk_widget_hide (dialogue);
    gtk_widget_show (dialogue);
}

void OnProjectMenu_New_afterSaveConfirmation (GtkMenuItem * button)
{
    ShowProjectPane (FALSE);
    NewCurrentBalsaProject ("Project Name", g_strdup (StartupPWD));
    AddImportPathToCurrentBalsaProject (".");
    CurrentBalsaProject->dirty = FALSE;
    UpdateProjectTrees ();
    ProjectOptions_DisplayWindow (TRUE);
}

void OnProjectMenu_New (GtkMenuItem * button)
{
    ConfirmSaveCurrentBalsaProjectAndExecute ((GtkSignalFunc) OnProjectMenu_New_afterSaveConfirmation);
}

void OnProjectMenu_ProjectOptions (GtkMenuItem * button)
{
    ProjectOptions_DisplayWindow (FALSE);
}

void OnProjectMenu_EnvironmentOptions (GtkMenuItem * button)
{
    if (workSpace.optionsDialogue->dialogue == NULL)
        workSpace.optionsDialogue->dialogue = CreateWindow ("OptionsDialogue");

    {
        GtkWidget *BalsaHomeEntry = LookupWidget (workSpace.optionsDialogue->dialogue, "BalsaHomeEntry");
        GtkWidget *TmpDirEntry = LookupWidget (workSpace.optionsDialogue->dialogue, "TmpDirEntry");
        GtkWidget *EditorNameEntry = LookupWidget (workSpace.optionsDialogue->dialogue, "EditorNameEntry");
        GtkWidget *PrintCommandEntry = LookupWidget (workSpace.optionsDialogue->dialogue, "PrintCommandEntry");
        GtkWidget *PSViewerNameEntry = LookupWidget (workSpace.optionsDialogue->dialogue, "PSViewerNameEntry");
        GtkWidget *HistoryMaxSizeValue = LookupWidget (workSpace.optionsDialogue->dialogue, "HistoryMaxSizeValue");

        gtk_entry_set_text (GTK_ENTRY (BalsaHomeEntry), EnvironmentOptions.BalsaHome ? EnvironmentOptions.BalsaHome : "");
        gtk_entry_set_text (GTK_ENTRY (TmpDirEntry), EnvironmentOptions.TmpDir ? EnvironmentOptions.TmpDir : "/tmp");
        gtk_entry_set_text (GTK_ENTRY (EditorNameEntry), EnvironmentOptions.editor ? EnvironmentOptions.editor : "");
        gtk_entry_set_text (GTK_ENTRY (PrintCommandEntry), EnvironmentOptions.PrintCommand ? EnvironmentOptions.PrintCommand : "");
        gtk_entry_set_text (GTK_ENTRY (PSViewerNameEntry), EnvironmentOptions.PSViewer ? EnvironmentOptions.PSViewer : "");
        gtk_spin_button_set_value (GTK_SPIN_BUTTON (HistoryMaxSizeValue), EnvironmentOptions.projectsHistoryMaxSize);

        gtk_widget_show (workSpace.optionsDialogue->dialogue);
        gdk_window_raise (workSpace.optionsDialogue->dialogue->window);
    }
}

void OnSelectionMenu_Delete_AfterConfirmation (GtkMenuItem * menuitem)
{
    PtrBalsaProjectEntry entry = ProjectFileViewSelectedEntry;

    switch (entry->nature)
    {
    case BalsaProjectEntry_File:
        {
            PtrBalsaFile file = BALSA_FILE (entry->data);

            CurrentBalsaProject->files = g_list_remove (CurrentBalsaProject->files, file);
            CurrentBalsaProject->dirty = TRUE;
            break;
        }
    case BalsaProjectEntry_Directory:
        break;
    case BalsaProjectEntry_Procedure:
        break;
    case BalsaProjectEntry_Test:
        {
            PtrBalsaTest test = BALSA_TEST (entry->data);
            PtrBalsaFile file = test->matchingBalsaFile;

            file->info.balsaFile.testFixtures = g_list_remove (file->info.balsaFile.testFixtures, test);
            CurrentBalsaProject->dirty = TRUE;
            break;
        }
    case BalsaProjectEntry_Implementation:
        {
            PtrImplementation impl = IMPLEMENTATION (entry->data);
            PtrBalsaFile file = impl->matchingBalsaFile;
            PtrBalsaTest test = impl->matchingBalsaTest;

            if (test)
                test->implementations = g_list_remove (test->implementations, impl);
            else
                file->info.balsaFile.implementations = g_list_remove (file->info.balsaFile.implementations, impl);
            CurrentBalsaProject->dirty = TRUE;
            break;
        }
    case BalsaProjectEntry_BuiltinLib:
        {
            PtrBuiltinLib lib = BUILTINLIB (entry->data);
            PtrBalsaFile file = lib->matchingBalsaFile;

            file->info.balsaFile.builtinLib = NULL;
            CurrentBalsaProject->dirty = TRUE;
            break;
        }
    case BalsaProjectEntry_TestComponent:
        break;
    }

    //  CurrentBalsaProject->needToRebuildMakefile = TRUE;
    UpdateProjectTrees ();
}

void OnSelectionMenu_Delete (GtkMenuItem * menuitem)
{
    PtrBalsaProjectEntry entry = ProjectFileViewSelectedEntry;

    switch (entry->nature)
    {
    case BalsaProjectEntry_Directory:
        printfConsole ("Cannot delete a Directory!\n");
        return;
    case BalsaProjectEntry_Procedure:
        printfConsole ("Cannot delete a Procedure!\n");
        return;
    case BalsaProjectEntry_TestComponent:
        printfConsole ("Cannot delete a Test component!\n");
        return;

    case BalsaProjectEntry_File:
    case BalsaProjectEntry_Test:
    case BalsaProjectEntry_Implementation:
    case BalsaProjectEntry_BuiltinLib:
        break;
    }

    GtkWidget *dialogue;
    const gchar *buttons[] = { "gtk-yes", "gtk-no" };
    GtkSignalFunc handlers[] = { (GtkSignalFunc) OnSelectionMenu_Delete_AfterConfirmation, NULL };

    dialogue = util_create_dialogue_with_buttons ("Are you sure you want to delete this item?", 2, buttons, 1, handlers, NULL);
    gtk_window_set_position (GTK_WINDOW (dialogue), GTK_WIN_POS_MOUSE);
    gtk_window_set_modal (GTK_WINDOW (dialogue), TRUE);
    gtk_widget_show (dialogue);
}

void OnSelectionMenu_Unselect (GtkMenuItem * menuitem)
{
    GtkWidget *TreeWidget = LookupWidget (MainWindow, "Tree1");

    gtk_ctree_unselect_recursive (GTK_CTREE (TreeWidget), NULL);
    gtk_object_set_data (GTK_OBJECT (TreeWidget), "SelectedNode", NULL);
}

void CallTestOptionsWindow (char *filename, char *procName, PtrBalsaTest editedTest)
{
    GtkWidget *dialogue = CreateWindow ("TestOptionsDialogue");
    GtkEntry *nameEntry = GTK_ENTRY (LookupWidget (dialogue, "TestNameEntry"));

    //   GtkCombo *filenameCombo = GTK_COMBO (LookupWidget (dialogue, "TestFilenameCombo"));
    GtkEntry *filenameEntry = GTK_ENTRY (LookupWidget (dialogue, "TestFilenameEntry"));
    GtkCList *list = GTK_CLIST (LookupWidget (dialogue, "TestPortList"));

    AddToOtherWindows_CurrentProject (dialogue);

    /* Adjust some properties */
    gtk_object_set_data (GTK_OBJECT (dialogue), "SelectedTestComponent", NULL);
    gtk_object_set_data (GTK_OBJECT (dialogue), "EditedTest", editedTest);

    /* Fill the Filenames combo box */
    {
        GList *filenames = NULL;
        GList *file;

        for (file = CurrentBalsaProject->files; file; file = file->next)
        {
            filenames = g_list_append (filenames, ConvertToBalsaFilename (BALSA_FILE (file->data)->name));
        }

        if (!filenames)
            filenames = g_list_append (filenames, g_strdup ("<no files>"));

        disable_filenameEntry = TRUE;
        //        gtk_combo_set_popdown_strings (filenameCombo, filenames);
        disable_filenameEntry = FALSE;
    }

    /* Select the right filename in the combo box => it will call OnTestFilenameEntryChanged(...) */
    if (filename)
    {
        gtk_entry_set_text (GTK_ENTRY (filenameEntry), ConvertToBalsaFilename (filename));
    }
    if (procName)
    {
        GtkEntry *procEntry = GTK_ENTRY (LookupWidget (dialogue, "TestProcNameEntry"));

        gtk_entry_set_text (procEntry, g_strdup (procName));
    }

    if (!editedTest)
    {                           // find a unused name for the test
        char *test_name = NULL;
        int num = 1;

      retry:
        g_free (test_name);
        test_name = g_strdup_printf ("test%d", num);
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

        gtk_entry_set_text (GTK_ENTRY (LookupWidget (dialogue, "TestNameEntry")), test_name);

        // Create new components
        //      CreateNewComponents (list, filename, procName);
    } else
    {
        TestOpts_clearList (dialogue);
        AddTestComponentsToTestComponentList (list, editedTest->testComponents);

        gtk_entry_set_text (GTK_ENTRY (LookupWidget (dialogue, "TestNameEntry")), editedTest->name);

        TestOpts_FillDefinesList (dialogue, editedTest->commandLineOptions);
    }

    TestOpts_UpdateForgottenPorts (list);
    gtk_clist_select_row (GTK_CLIST (list), 0, 0);

    /* Make name selected so we can directly overtype it */
    gtk_entry_select_region (nameEntry, 0, -1);
    gtk_widget_grab_focus (GTK_WIDGET (nameEntry));

    gtk_widget_show (dialogue);
}

void OnSelectionMenu_AddTest (GtkMenuItem * menuitem)
{
    PtrBalsaProjectEntry entry = ProjectFileViewSelectedEntry;

    switch (entry->nature)
    {                           /* different types of editing */
    case BalsaProjectEntry_File:
        {
            PtrBalsaFile file = BALSA_FILE (entry->data);

            CallTestOptionsWindow (file->name, NULL, NULL);
            break;
        }
    case BalsaProjectEntry_Procedure:
        {
            PtrBalsaProcedure proc = BALSA_PROCEDURE (entry->data);

            CallTestOptionsWindow (proc->associated_filename, proc->name, NULL);
            break;
        }
        // TODO
        break;
    case BalsaProjectEntry_TestComponent: /* edit the whole test fixture */
        {
            //GtkCTreeNode *treeNode = BALSA_PROCEDURE(entry->data)->treeNode;
            GtkCTree *tree = GTK_CTREE (LookupWidget (MainWindow, "TreeFilesView"));
            GtkCTreeNode *treeNode = GTK_CTREE_NODE (gtk_object_get_data (GTK_OBJECT (tree), "SelectedNode"));
            GtkCTreeNode *parentNode = GTK_CTREE_ROW (treeNode)->parent;

            entry = BALSA_PROJECT_ENTRY (gtk_ctree_node_get_row_data (tree, parentNode));
        }
        // continue on next case.
    case BalsaProjectEntry_Test:
        {
            PtrBalsaTest test = BALSA_TEST (entry->data);
            PtrBalsaFile file = test->matchingBalsaFile;

            CallTestOptionsWindow (file->name, test->procName, NULL);
            break;
        }
    case BalsaProjectEntry_Implementation:
        {
            PtrImplementation file = IMPLEMENTATION (entry->data);

            CallTestOptionsWindow (file->matchingBalsaFile->name, NULL, NULL);
            break;
        }
    case BalsaProjectEntry_BuiltinLib:
        {
            PtrBuiltinLib file = BUILTINLIB (entry->data);

            CallTestOptionsWindow (file->matchingBalsaFile->name, NULL, NULL);
            break;
        }
    case BalsaProjectEntry_Directory:
    default:
        printfConsole ("No Edition instructions for this type.\n");
    }
}

void OnSelectionMenu_AddImplementation (GtkMenuItem * menuitem)
{
    PtrBalsaProjectEntry entry = ProjectFileViewSelectedEntry;
    PtrBalsaFile file = NULL;
    PtrBalsaTest test = NULL;

    switch (entry->nature)
    {                           /* different types of editing */
    case BalsaProjectEntry_File:
        {
            file = BALSA_FILE (entry->data);
            break;
        }
    case BalsaProjectEntry_Procedure:
        {
            PtrBalsaProcedure proc = BALSA_PROCEDURE (entry->data);
            char *filename = proc->associated_filename;

            GList *files;

            for (files = CurrentBalsaProject->files; files; files = files->next)
            {
                file = (PtrBalsaFile) files->data;
                if (!strcmp (filename, file->name))
                    break;
            }

            break;
        }
        break;
    case BalsaProjectEntry_TestComponent: // edit the whole test fixture
        {
            //GtkCTreeNode *treeNode = BALSA_PROCEDURE(entry->data)->treeNode;
            GtkCTree *tree = GTK_CTREE (LookupWidget (MainWindow, "TreeFilesView"));
            GtkCTreeNode *treeNode = GTK_CTREE_NODE (gtk_object_get_data (GTK_OBJECT (tree), "SelectedNode"));
            GtkCTreeNode *parentNode = GTK_CTREE_ROW (treeNode)->parent;

            entry = BALSA_PROJECT_ENTRY (gtk_ctree_node_get_row_data (tree, parentNode));
        }
        // continue on next case.
    case BalsaProjectEntry_Test:
        {
            PtrBalsaTest test2 = BALSA_TEST (entry->data);

            file = test2->matchingBalsaFile;
            test = test2;
            break;
        }
    case BalsaProjectEntry_Implementation:
        {
            file = IMPLEMENTATION (entry->data)->matchingBalsaFile;
            test = IMPLEMENTATION (entry->data)->matchingBalsaTest;
            break;
        }
    case BalsaProjectEntry_BuiltinLib:
        {
            file = BUILTINLIB (entry->data)->matchingBalsaFile;
            break;
        }
    case BalsaProjectEntry_Directory:
    default:
        printfConsole ("No Edition instructions for this type.\n");
    }

    if (file)
        CallImplementationSelectionWindow (file, test, NULL);
}

void OnSelectionMenu_AddBuiltinLib (GtkMenuItem * menuitem)
{
    PtrBalsaProjectEntry entry = ProjectFileViewSelectedEntry;
    PtrBalsaFile file = NULL;

    switch (entry->nature)
    {                           /* different types of editing */
    case BalsaProjectEntry_File:
        {
            file = BALSA_FILE (entry->data);
            break;
        }
    case BalsaProjectEntry_Procedure:
        {
            PtrBalsaProcedure proc = BALSA_PROCEDURE (entry->data);
            char *filename = proc->associated_filename;

            GList *files;

            for (files = CurrentBalsaProject->files; files; files = files->next)
            {
                file = (PtrBalsaFile) files->data;
                if (!strcmp (filename, file->name))
                    break;
            }

            break;
        }
        break;
    case BalsaProjectEntry_TestComponent: // edit the whole test fixture
        {
            //GtkCTreeNode *treeNode = BALSA_PROCEDURE(entry->data)->treeNode;
            GtkCTree *tree = GTK_CTREE (LookupWidget (MainWindow, "TreeFilesView"));
            GtkCTreeNode *treeNode = GTK_CTREE_NODE (gtk_object_get_data (GTK_OBJECT (tree), "SelectedNode"));
            GtkCTreeNode *parentNode = GTK_CTREE_ROW (treeNode)->parent;

            entry = BALSA_PROJECT_ENTRY (gtk_ctree_node_get_row_data (tree, parentNode));
        }
        // continue on next case.
    case BalsaProjectEntry_Test:
        {
            PtrBalsaTest test = BALSA_TEST (entry->data);

            file = test->matchingBalsaFile;
            break;
        }
    case BalsaProjectEntry_BuiltinLib:
        {
            file = BUILTINLIB (entry->data)->matchingBalsaFile;
            break;
        }
    case BalsaProjectEntry_Directory:
    default:
        printfConsole ("No Edition instructions for this type.\n");
    }

    if (file)
        CallBuiltinLibSelectionWindow (file, NULL);
}

void OnSelectionMenu_Edit (GtkMenuItem * menuitem)
{
    PtrBalsaProjectEntry entry = ProjectFileViewSelectedEntry;

    switch (entry->nature)
    {                           /* different types of editing */
    case BalsaProjectEntry_File:
    case BalsaProjectEntry_Procedure:
        OnProjectToolbar_LaunchEditor (NULL);
        break;
    case BalsaProjectEntry_Test:
        {
            PtrBalsaTest test = BALSA_TEST (entry->data);
            PtrBalsaFile file = test->matchingBalsaFile;

            CallTestOptionsWindow (file->name, test->procName, test);
            break;
        }
    case BalsaProjectEntry_TestComponent: /* edit the whole test fixture */
        {
            //GtkCTreeNode *treeNode = BALSA_PROCEDURE(entry->data)->treeNode;
            GtkCTree *tree = GTK_CTREE (LookupWidget (MainWindow, "TreeFilesView"));
            GtkCTreeNode *treeNode = GTK_CTREE_NODE (gtk_object_get_data (GTK_OBJECT (tree), "SelectedNode"));
            GtkCTreeNode *parentNode = GTK_CTREE_ROW (treeNode)->parent;

            entry = BALSA_PROJECT_ENTRY (gtk_ctree_node_get_row_data (tree, parentNode));
            {
                PtrBalsaTest test = BALSA_TEST (entry->data);
                PtrBalsaFile file = test->matchingBalsaFile;

                CallTestOptionsWindow (file->name, test->procName, test);
            }
            break;
        }
    case BalsaProjectEntry_Implementation:
        {
            PtrImplementation implementation = IMPLEMENTATION (entry->data);

            CallImplementationSelectionWindow (implementation->matchingBalsaFile, implementation->matchingBalsaTest, implementation);
            break;
        }
    case BalsaProjectEntry_BuiltinLib:
        {
            PtrBuiltinLib builtinLib = BUILTINLIB (entry->data);

            CallBuiltinLibSelectionWindow (builtinLib->matchingBalsaFile, builtinLib);
            break;
        }
    case BalsaProjectEntry_Directory:
    default:
        printfConsole ("No Edition instructions for this type.\n");
    }
}

void OnSelectionMenu_Make (GtkMenuItem * menuitem)
{
    PtrBalsaProjectEntry entry = ProjectFileViewSelectedEntry;

    switch (entry->nature)
    {                           /* different types of editing */
    case BalsaProjectEntry_File:
        {
            PtrBalsaFile file = BALSA_FILE (entry->data);

            char *makeName = g_strdup_printf ("%s.breeze", file->name);

            MakefileTable_MakeCallback (NULL, makeName);
            break;
        }
    case BalsaProjectEntry_Procedure:
        {
            PtrBalsaProcedure proc = BALSA_PROCEDURE (entry->data);

            char *makeName = g_strdup_printf ("%s.breeze", proc->associated_filename);

            MakefileTable_MakeCallback (NULL, makeName);
            break;
        }
    case BalsaProjectEntry_Test:
        {
            PtrBalsaTest test = BALSA_TEST (entry->data);
            char *makeName = g_strdup_printf ("sim-win-%s", test->name);

            MakefileTable_MakeCallback (NULL, makeName);
            break;
        }
    case BalsaProjectEntry_TestComponent: /* edit the whole test fixture */
        {
            //GtkCTreeNode *treeNode = BALSA_PROCEDURE(entry->data)->treeNode;
            GtkCTree *tree = GTK_CTREE (LookupWidget (MainWindow, "TreeFilesView"));
            GtkCTreeNode *treeNode = GTK_CTREE_NODE (gtk_object_get_data (GTK_OBJECT (tree), "SelectedNode"));
            GtkCTreeNode *parentNode = GTK_CTREE_ROW (treeNode)->parent;

            entry = BALSA_PROJECT_ENTRY (gtk_ctree_node_get_row_data (tree, parentNode));
            {
                PtrBalsaTest test = BALSA_TEST (entry->data);

                char *makeName = g_strdup_printf ("sim-win-%s", test->name);

                MakefileTable_MakeCallback (NULL, makeName);
            }
            break;
        }
    case BalsaProjectEntry_Implementation:
        {
            PtrImplementation impl = IMPLEMENTATION (entry->data);

           char *makeName;

           if (impl->matchingBalsaTest)
               makeName = g_strdup_printf ("sim-pre-%s-%s", impl->matchingBalsaTest->name, impl->name);
           else
               makeName = g_strdup_printf ("impl-%s-%s", impl->matchingBalsaFile->name, impl->name);

            MakefileTable_MakeCallback (NULL, makeName);
            break;
        }
    case BalsaProjectEntry_BuiltinLib:
        {
            PtrBuiltinLib lib = BUILTINLIB (entry->data);

            char *makeName = g_strdup_printf ("lib-%s", lib->name);

            MakefileTable_MakeCallback (NULL, makeName);
            break;
        }
    case BalsaProjectEntry_Directory:
    default:
        printfConsole ("No Make instructions for this type.\n");
    }
}

int ViewFilesOption = 1;        // 0=when tests ; 1=always
int ViewProceduresOption = 1;   // 0=when tests ; 1=always
int ViewTestsOption = 1;        // 0=no ; 1=yes

int ConsoleWindowViewOptions = 2; //0=Always 1=Never 2=Automatic

void OnViewMenu_ViewFilesAlways (GtkMenuItem * button)
{
    ViewFilesOption = 1;
    UpdateProjectTrees ();
}

void OnViewMenu_ViewFilesNotAlways (GtkMenuItem * button)
{
    ViewFilesOption = 0;
    UpdateProjectTrees ();
}

void OnViewMenu_ViewProceduresAlways (GtkMenuItem * button)
{
    ViewProceduresOption = 1;
    UpdateProjectTrees ();
}

void OnViewMenu_ViewProceduresNotAlways (GtkMenuItem * button)
{
    ViewProceduresOption = 0;
    UpdateProjectTrees ();
}

void OnViewMenu_ViewTestsYes (GtkMenuItem * button)
{
    ViewTestsOption = 1;
    UpdateProjectTrees ();
}

void OnViewMenu_ViewTestsNo (GtkMenuItem * button)
{
    ViewTestsOption = 0;
    UpdateProjectTrees ();
}

void OnViewMenu_Console_Always (GtkMenuItem * button)
{
    ConsoleWindowViewOptions = 0; //0=Always 1=Never 2=Automatic
    if (workSpace.consoleWindow) gtk_widget_show (workSpace.consoleWindow->window);
}

void OnViewMenu_Console_Never (GtkMenuItem * button)
{
    ConsoleWindowViewOptions = 1; //0=Always 1=Never 2=Automatic
    if (workSpace.consoleWindow) gtk_widget_hide (workSpace.consoleWindow->window);
}

void OnViewMenu_Console_Automatic (GtkMenuItem * button)
{
    ConsoleWindowViewOptions = 2; //0=Always 1=Never 2=Automatic
}

void OnViewMenu_Console_DisplayHide (GtkMenuItem * button)
{
    if (GTK_WIDGET_VISIBLE (workSpace.consoleWindow->window))
        gtk_widget_hide (workSpace.consoleWindow->window);
    else
    {
        gtk_widget_hide (workSpace.consoleWindow->window);
        gtk_widget_show (workSpace.consoleWindow->window);
    }
}
void OnViewMenu_ExecutionWindow_Always (GtkMenuItem * button)
{
    ExecutionWindowViewOptions = 0; //0=Always 1=Never 2=Automatic

    if (ExecutionWindow) gtk_widget_show (ExecutionWindow);
}

void OnViewMenu_ExecutionWindow_Never (GtkMenuItem * button)
{
    ExecutionWindowViewOptions = 1; //0=Always 1=Never 2=Automatic
    if (ExecutionWindow) gtk_widget_hide (ExecutionWindow);
}

void OnViewMenu_ExecutionWindow_Automatic (GtkMenuItem * button)
{
    ExecutionWindowViewOptions = 2; //0=Always 1=Never 2=Automatic
}

void OnViewMenu_ExecutionWindow_DisplayHide (GtkMenuItem * button)
{
    if (GTK_WIDGET_VISIBLE (ExecutionWindow))
        gtk_widget_hide (ExecutionWindow);
    else
    {
        gtk_widget_hide (ExecutionWindow);
        gtk_widget_show (ExecutionWindow);
    }
}

void init_mainWindow_menus (void)
{
   /* glade-2 buggers up RadioMenuItem initial settings.  Set them here to be safe */
   gtk_check_menu_item_set_active (GTK_CHECK_MENU_ITEM (LookupWidget (MainWindow, "ViewMenu_ViewFilesNotAlways")), FALSE);
   gtk_check_menu_item_set_active (GTK_CHECK_MENU_ITEM (LookupWidget (MainWindow, "ViewMenu_ViewFilesAlways")), TRUE);
   ViewFilesOption = 1;

   gtk_check_menu_item_set_active (GTK_CHECK_MENU_ITEM (LookupWidget (MainWindow, "ViewMenu_ViewProceduresNotAlways")), FALSE);
   gtk_check_menu_item_set_active (GTK_CHECK_MENU_ITEM (LookupWidget (MainWindow, "ViewMenu_ViewProceduresAlways")), TRUE);
   ViewProceduresOption = 1;

   gtk_check_menu_item_set_active (GTK_CHECK_MENU_ITEM (LookupWidget (MainWindow, "ViewMenu_ViewTestsYes")), TRUE);
   gtk_check_menu_item_set_active (GTK_CHECK_MENU_ITEM (LookupWidget (MainWindow, "ViewMenu_ViewTestsNo")), FALSE);
   ViewTestsOption = 1;

   gtk_check_menu_item_set_active (GTK_CHECK_MENU_ITEM (LookupWidget (MainWindow, "ViewMenu_Console_Always")), FALSE);
   gtk_check_menu_item_set_active (GTK_CHECK_MENU_ITEM (LookupWidget (MainWindow, "ViewMenu_Console_Never")), FALSE);
   gtk_check_menu_item_set_active (GTK_CHECK_MENU_ITEM (LookupWidget (MainWindow, "ViewMenu_Console_Automatic")), TRUE);
   ConsoleWindowViewOptions = 2;

   gtk_check_menu_item_set_active (GTK_CHECK_MENU_ITEM (LookupWidget (MainWindow, "ViewMenu_ExecutionWindow_Always")), FALSE);
   gtk_check_menu_item_set_active (GTK_CHECK_MENU_ITEM (LookupWidget (MainWindow, "ViewMenu_ExecutionWindow_Never")), FALSE);
   gtk_check_menu_item_set_active (GTK_CHECK_MENU_ITEM (LookupWidget (MainWindow, "ViewMenu_ExecutionWindow_Automatic")), TRUE);
   ExecutionWindowViewOptions = 2;
}
