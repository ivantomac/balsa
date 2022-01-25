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

	`project.c'
	Project details structure

*/

#include <stdio.h>
#include <unistd.h>
#include <gtk/gtk.h>

#include "project.h"
#include <breeze/lparse.h>
#include "options.h"
#include "file.h"
#include "paths.h"
#include "main.h"
#include "utils.h"
#include "workspace.h"
#include "aboutdialogue.h"
#include "filemanager.h"
#include "support.h"
#include "../../config.h"

#include <pwd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/utsname.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

PtrBalsaProject CurrentBalsaProject = NULL;

static int MemoryComponentCount;

/* BalsaProjectEntryDetailsNames : names to be used in the `Details' column of the file view */
char *BalsaProjectEntryDetailsNames[8] = {
    "Path Prefix",
    "",
    "",
    "Description",
    "Filename / Value",
    "Filename",
    "",
    ""
};

static void ConfirmSaveCurrentBalsaProject_OnYes (GtkWidget * button, GtkSignalFunc func)
{
    OnProjectMenu_Save (NULL);
    func ();
}
static void ConfirmSaveCurrentBalsaProject_OnNo (GtkWidget * button, GtkSignalFunc func)
{
    func ();
}

void ConfirmSaveCurrentBalsaProjectAndExecute (GtkSignalFunc func)
{
    /* check if the current project needs to be saved */
    if (CurrentBalsaProject && (CurrentBalsaProject->dirty || CurrentBalsaProject->nb_dirty_files))
    {
        GtkWidget *dialogue;
        const gchar *buttons[] = { "gtk-yes", "gtk-no", "gtk-cancel" };
        GtkSignalFunc handlers[] = { (GtkSignalFunc) ConfirmSaveCurrentBalsaProject_OnYes,
            (GtkSignalFunc) ConfirmSaveCurrentBalsaProject_OnNo, NULL
        };

        dialogue = util_create_dialogue_with_buttons ("Do you want to save the current project?", 3, buttons, 3, handlers, func);
        gtk_window_set_position (GTK_WINDOW (dialogue), GTK_WIN_POS_MOUSE);
        gtk_widget_show (dialogue);
    } else
        func ();
}

void FreeBalsaProject (PtrBalsaProject project)
{
    if (!project)
        return;

    if (project->name)
        g_free (project->name);
    if (project->directory)
        g_free (project->directory);
    if (project->files)
    {
        g_list_foreach (project->files, (GFunc) g_free, NULL);
        g_list_free (project->files);
    }
    if (project->importPath)
    {
        g_list_foreach (project->importPath, (GFunc) g_free, NULL);
        g_list_free (project->importPath);
    }
    if (project->defines)
    {
        //        g_list_foreach (project->defines, (GFunc) , NULL);
        g_list_free (project->defines);
    }
    //  if ( project->technology )
    //      g_free( project->technology );
    if (project->BALSACOPTS)
        g_free (project->BALSACOPTS);
    if (project->BREEZESIMOPTS)
        g_free (project->BREEZESIMOPTS);
    if (project->BREEZELINKOPTS)
        g_free (project->BREEZELINKOPTS);
    g_free (project);
}

void CloseCurrentBalsaProject_CloseAttachedWindows_Yes (GtkWidget * button)
{
    {
        GList *ptr = OtherWindows_ClosedProjects;

        while (ptr)
        {
            GList *next = ptr->next;

            DestroyOtherWindow (ptr->data);
            ptr = next;
        }
        OtherWindows_ClosedProjects = NULL;
    }
}

void CloseCurrentBalsaProject_UnactivateAttachedWindows (void)
{
    if (GTK_IS_DIALOG (workSpace.projectOptionsDialogue->dialogue))
    {
        GtkWidget *buttonOK = GTK_WIDGET (LookupWidget (workSpace.projectOptionsDialogue->dialogue, "OKbutton"));

        gtk_widget_set_sensitive (buttonOK, FALSE);
        AddToOtherWindows_ClosedProjects (workSpace.projectOptionsDialogue->dialogue);
        workSpace.projectOptionsDialogue->dialogue = NULL;
    }

    if (OtherWindows_CurrentProject)
    {
        GList *ptr = OtherWindows_CurrentProject;

        while (ptr)
        {
            //todo:normalize OK button name
			GtkWidget *buttonOK = GTK_WIDGET (LookupWidget (workSpace.projectOptionsDialogue->dialogue, "OKbutton"));

            gtk_widget_set_sensitive (buttonOK, FALSE);
            AddToOtherWindows_ClosedProjects (ptr->data);
            ptr = ptr->next;
        }
        g_list_free (OtherWindows_CurrentProject);
        OtherWindows_CurrentProject = NULL;
    }

    if (OtherWindows_ClosedProjects)
    {
        GtkWidget *dialogue;
        const gchar *buttons[] = { "gtk-yes", "gtk-no" };
        GtkSignalFunc handlers[] = { (GtkSignalFunc) CloseCurrentBalsaProject_CloseAttachedWindows_Yes,
            NULL
        };

        dialogue =
          util_create_dialogue_with_buttons ("Do you want to close the windows related to the previous projects?", 2, buttons, 1, handlers, NULL);
        gtk_window_set_position (GTK_WINDOW (dialogue), GTK_WIN_POS_MOUSE);
        gtk_widget_show (dialogue);
    }
}

/* CloseCurrentBalsaProject : */
void CloseCurrentBalsaProject (void)
{
    if (!CurrentBalsaProject)
        return;

    FreeBalsaProject (CurrentBalsaProject);
    CurrentBalsaProject = NULL;

    ShowProjectPane (FALSE);

    /* Process the different options windows */
    CloseCurrentBalsaProject_UnactivateAttachedWindows ();

    UpdateMainWindowGreying ();
}

/* NewCurrentBalsaProject :  */
void NewCurrentBalsaProject (const char *name, const char *directory)
{
    CloseCurrentBalsaProject (); //ASSERT(CurrentBalsaProject==NULL);

    CurrentBalsaProject = (PtrBalsaProject) g_malloc (sizeof (BalsaProject));
    CurrentBalsaProject->name = g_strdup (name);
    CurrentBalsaProject->directory = g_strdup (directory);
    FileManager_RefreshAllDisplayedNames ();
    CurrentBalsaProject->files = NULL;
    CurrentBalsaProject->importPath = NULL;
    CurrentBalsaProject->defines = NULL;
    CurrentBalsaProject->simulationSystem = breezesim;
    //  CurrentBalsaProject->technology = NULL;
    CurrentBalsaProject->BALSACOPTS = NULL;
    CurrentBalsaProject->BREEZESIMOPTS = NULL;
    CurrentBalsaProject->BREEZELINKOPTS = NULL;
    CurrentBalsaProject->dirty = TRUE;
    CurrentBalsaProject->dirty_and_need_make_clean = FALSE;
    CurrentBalsaProject->nb_dirty_files = 0; //todo: count the number of already opened files that match this project.
    //  CurrentBalsaProject->needToRebuildMakefile = TRUE;

    /* Use the project template */
    GSList *list = EnvironmentOptions.newProjectTemplate;

    for (; list; list = list->next)
    {
        char *str = list->data;
        char *val = strchr (str, '=');

        if (!val)
            continue;
        val++;
        if (!strncmp (str, "BALSACOPTS=", val - str))
            CurrentBalsaProject->BALSACOPTS = g_strdup (val);
        else if (!strncmp (str, "BREEZESIMOPTS=", val - str))
            CurrentBalsaProject->BREEZESIMOPTS = g_strdup (val);
        else if (!strncmp (str, "BREEZELINKOPTS=", val - str))
            CurrentBalsaProject->BREEZELINKOPTS = g_strdup (val);
        else if (!strncmp (str, "simulation-system=", val - str))
        {
            if (!strcmp (val, "breezesim"))
                CurrentBalsaProject->simulationSystem = breezesim;
            else {
                CurrentBalsaProject->simulationSystem = breezesim;
            	printfConsole_s ("unsupported simulation system `%s'.  Using breezesim instead.", val);
            }
        }
    }

    /* change the working directory */
    chdir (CurrentBalsaProject->directory);
}

void ShowProjectPane (gboolean showNhide)
{
	if (showNhide)
    	gtk_widget_show (GTK_WIDGET (LookupWidget (MainWindow, "ProjectNotebook")));
    else gtk_widget_hide (GTK_WIDGET (LookupWidget (MainWindow, "ProjectNotebook")));
}

gboolean AddFileToCurrentBalsaProject (char *file)
{
    file = g_strdup (file);
    //  CleanUpFilename(file);
    CurrentBalsaProject->files = g_list_prepend (CurrentBalsaProject->files, (gpointer) file);
    return FALSE;
}

gboolean AddImportPathToCurrentBalsaProject (char *directory)
{
    directory = g_strdup (directory);
    //  CleanUpFilename(directory);

    //check the directory doesn't already exist in the list
    {
        GList *tmp = CurrentBalsaProject->importPath;

        for (; tmp; tmp = tmp->next)
            if (!strcmp (tmp->data, directory))
                return FALSE;
    }

    CurrentBalsaProject->importPath = g_list_prepend (CurrentBalsaProject->importPath, (gpointer) directory);
    return FALSE;
}

gboolean AddDefineToCurrentBalsaProject (char *name, char *value)
{
    CurrentBalsaProject->defines = g_list_prepend (CurrentBalsaProject->defines, NewDefine (name, value));
    return FALSE;
}

void SetAsCurrentBalsaProject (PtrBalsaProject project)
{
    /* Do like that in order to check at the same time the integrity of the project */
    NewCurrentBalsaProject (project->name, project->directory);
    ShowProjectPane (TRUE);
    {
        char buf[1000];

        getcwd (buf, 1000);
        CurrentBalsaProject->directory = g_strdup (buf);
        FileManager_RefreshAllDisplayedNames ();
    }
    CurrentBalsaProject->files = project->files;
    CurrentBalsaProject->defines = project->defines;
    CurrentBalsaProject->simulationSystem = project->simulationSystem;
    //  CurrentBalsaProject->technology = project->technology;
    CurrentBalsaProject->BALSACOPTS = project->BALSACOPTS;
    CurrentBalsaProject->BREEZESIMOPTS = project->BREEZESIMOPTS;
    CurrentBalsaProject->BREEZELINKOPTS = project->BREEZELINKOPTS;
    //g_list_foreach( project->files,      (GFunc) AddFileToCurrentBalsaProject,       NULL);
    g_list_foreach (project->importPath, (GFunc) AddImportPathToCurrentBalsaProject, NULL);
    CurrentBalsaProject->dirty = project->dirty;
    CurrentBalsaProject->dirty_and_need_make_clean = project->dirty_and_need_make_clean;
    CurrentBalsaProject->nb_dirty_files = project->nb_dirty_files;
    //  CurrentBalsaProject->needToRebuildMakefile = project->needToRebuildMakefile;
}

/* NewBalsaProject : make a new, empty, project */
PtrBalsaProject NewBalsaProject (void)
{
    PtrBalsaProject project = (PtrBalsaProject) g_malloc (sizeof (BalsaProject));

    project->name = g_strdup ("New Project");
    project->directory = g_strdup ("");
    project->files = NULL;
    project->importPath = NULL;
    project->defines = NULL;
    project->simulationSystem = breezesim;
    //  project->technology = NULL;
    project->BALSACOPTS = NULL;
    project->BREEZESIMOPTS = NULL;
    project->BREEZELINKOPTS = NULL;
    project->dirty = FALSE;
    project->dirty_and_need_make_clean = FALSE;
    project->nb_dirty_files = 0; //todo: count the number of already opened files that match this project.
    // project->needToRebuildMakefile = FALSE;

    return project;
}

/* NewBalsaProjectEntry : `file' view entry wrapper */
PtrBalsaProjectEntry NewBalsaProjectEntry (BalsaProjectEntryNature nature, gpointer data)
{
    PtrBalsaProjectEntry entry = (PtrBalsaProjectEntry) g_malloc (sizeof (BalsaProjectEntry));

    entry->nature = nature;
    entry->data = data;

    return entry;
}

/* DeleteBalsaProject : dispose of a Balsa project structure in an orderly way */
void DeleteBalsaProject (PtrBalsaProject project)
{
    if (project->name)
        g_free (project->name);
    if (project->directory)
        g_free (project->directory);
    if (project->importPath)
        g_list_free (project->importPath); /* NB. deallocate strings */

    g_free (project);
}

/* DeleteBalsaProjectEntry : dispose func. */
void DeleteBalsaProjectEntry (PtrBalsaProjectEntry entry)
{
    if (entry)
    {
        if (entry->data)
            g_free (entry->data);
        g_free (entry);
    }
}

/* WriteBalsaProjectImplementation : write an implementation description into the stream `stream'.
	Use `tabs' as start of line spacing */
void WriteBalsaProjectImplementation (FILE * stream, char *tabs, PtrImplementation impl)
{
    fprintf (stream, "%s(implementation \"%s\"\n", tabs, (impl->name ? impl->name : ""));
    if (impl->balsaNetlistOptions && *impl->balsaNetlistOptions)
        fprintf (stream, "%s    (balsa-netlist-options \"%s\")\n", tabs, impl->balsaNetlistOptions);
    if (impl->makeImplTestOptions && *impl->makeImplTestOptions)
        fprintf (stream, "%s    (make-impl-test-options \"%s\")\n", tabs, impl->makeImplTestOptions);
    if (impl->technology && *impl->technology)
        fprintf (stream, "%s    (technology \"%s\")\n", tabs, impl->technology);
    if (impl->DumpFile && *impl->DumpFile)
        fprintf (stream, "%s    (dump-file \"%s\")\n", tabs, impl->DumpFile);
    fprintf (stream, "%s)\n", tabs);
}

/* WriteBalsaProjectTest : write a test description into the stream `stream'.
	Use `tabs' as start of line spacing */
void WriteBalsaProjectTest (FILE * stream, char *tabs, PtrBalsaTest test)
{
    GList *testComponents = test->testComponents;
    GList *implementations = test->implementations;

    fprintf (stream, "%s(test \"%s\" \"%s\"\n", tabs, (test->name ? test->name : ""), (test->procName ? test->procName : ""));

    if (test->commandLineOptions && *test->commandLineOptions)
    {
        fprintf (stream, "%s    (command-line-options \"%s\")\n", tabs, test->commandLineOptions);
    }

    if (testComponents)
    {
        fprintf (stream, "%s    (ports\n", tabs);
        // Process Memory component ports
        while (testComponents)
        {
            PtrBalsaTestComponent testComponent = BALSA_TESTCOMPONENT (testComponents->data);

            fprintf (stream, "%s        (", tabs);

            int nbnames;

            switch (testComponent->nature)
            {
            case BalsaTestComponent_Sync:
                fprintf (stream, "sync-port");
                nbnames = 1;
                break;
            case BalsaTestComponent_InputFromFile:
            case BalsaTestComponent_InputFromValue:
                fprintf (stream, "input-port");
                nbnames = 1;
                break;
            case BalsaTestComponent_OutputToFile:
            case BalsaTestComponent_OutputToStdout:
                fprintf (stream, "output-port");
                nbnames = 1;
                break;
            case BalsaTestComponent_Memory:
                fprintf (stream, "memory-component");
                nbnames = 3;
                break;
            case BalsaTestComponent_Undefined:
                continue;
            }

            GList *tmp = testComponent->portNames;
            int i;

            for (i = 0; tmp && i < nbnames; tmp = tmp->next, i++)
                fprintf (stream, " \"%s\"", (char *) tmp->data);
            for (; i < nbnames; i++)
                fprintf (stream, " \"\"");

            int minparams, maxparams;

            switch (testComponent->nature)
            {
            case BalsaTestComponent_Sync:
                minparams = maxparams = 0;
                break;
            case BalsaTestComponent_InputFromFile:
                fprintf (stream, " file");
                minparams = 1;
                maxparams = 3;
                break;
            case BalsaTestComponent_InputFromValue:
                fprintf (stream, " value");
                minparams = 1;
                maxparams = 3;
                break;
            case BalsaTestComponent_OutputToFile:
                fprintf (stream, " file");
                minparams = 1;
                maxparams = 4;
                break;
            case BalsaTestComponent_OutputToStdout:
                fprintf (stream, " log");
                minparams = 1;
                maxparams = 4;
                break;
            case BalsaTestComponent_Memory:
                minparams = 1;
                maxparams = 1;
                break;
            case BalsaTestComponent_Undefined:
                break;
            }

            tmp = CommaStringToList (testComponent->value);
            for (i = 0; tmp && i < maxparams; tmp = tmp->next, i++)
                fprintf (stream, " \"%s\"", (char *) tmp->data);
            for (; i < minparams; i++)
                fprintf (stream, " \"\"");

            fprintf (stream, ")\n");

            testComponents = testComponents->next;
        }
        fprintf (stream, "%s    )\n", tabs);
    }

    if (implementations)
    {
        fprintf (stream, "%s    (implementations\n", tabs);
        while (implementations)
        {
            char *newTabs = g_strdup_printf ("%s        ", tabs);

            WriteBalsaProjectImplementation (stream, newTabs, IMPLEMENTATION (implementations->data));
            g_free (newTabs);

            implementations = implementations->next;
        }
        fprintf (stream, "    %s)\n", tabs);
    }

    fprintf (stream, "%s)\n", tabs);
}

/* WriteBalsaProjectBuiltinLib : write an builtinLib description into the stream `stream'.
	Use `tabs' as start of line spacing */
void WriteBalsaProjectBuiltinLib (FILE * stream, char *tabs, PtrBuiltinLib lib)
{
    fprintf (stream, "%s(builtin-lib \"%s\"\n", tabs, (lib->name ? lib->name : ""));
    if (lib->compileOptions && *lib->compileOptions)
        fprintf (stream, "%s    (compile-options \"%s\")\n", tabs, lib->compileOptions);
    if (lib->sourceFilenames)
    {
        fprintf (stream, "%s    (source-files\n", tabs);
        GList *tmp = lib->sourceFilenames;

        for (; tmp; tmp = tmp->next)
            fprintf (stream, "%s        \"%s\"\n", tabs, (char *) tmp->data);
        fprintf (stream, "%s    )\n", tabs);
    }
    fprintf (stream, "%s)\n", tabs);
}

/* WriteBalsaProjectFile : make a Balsa project file in the lispy format */
void WriteBalsaProjectFile (char *directory, PtrBalsaProject project)
{
    FILE *stream;
    char *filename = g_malloc (strlen (directory) + 9);
    char *importPath;
    GList *files;

    sprintf (filename, "%s/Project", directory);

    stream = fopen (filename, "w");

    if (!stream)
        fprintf (stderr, "WriteBalsaProjectFile: couldn't write file `%s'\n", filename);
    {
        //  UpdateBalsaProjectFromFileView (project);
        files = project->files;

        if (project->importPath)
        {
            char **importPathArray = StringListToArray (project->importPath);

            importPath = g_strjoinv ("\" \"", importPathArray);
            g_free (importPathArray);
        } else
            importPath = g_strdup ("");

        {                       /* Write the header */
            time_t now = time (NULL);
            struct utsname systemName;
            struct passwd *passEntry;
            char *commentString = ";;;";
            char *fileType = "Balsa Project file";

            passEntry = getpwuid (getuid ());
            uname (&systemName);
            fprintf (stream, "%s %s\n%s Created: %s%s By: %s@%s (%s)\n"
              "%s With balsa-mgr version: %s (%s)\n%s Supplied with Balsa version: %s\n\n",
              commentString, fileType,
              commentString, ctime (&now),
              commentString, passEntry->pw_name, systemName.nodename,
              systemName.sysname, commentString, BALSA_MGR_VERSION_NUMBER, BALSA_MGR_VERSION_DATE, commentString, VERSION);
        }

        fprintf (stream, "(balsa-project\n");
        fprintf (stream, "    (name \"%s\")\n", project->name);
        //        fprintf (stream, "    (directory \"%s\")\n", project->directory);
        fprintf (stream, "    (import-path \"%s\")\n", importPath);
        switch (project->simulationSystem)
        {
        case breezesim:
            fprintf (stream, "    (simulation-system \"breezesim\")\n");
        }

        if (project->defines
          || (project->BALSACOPTS && *project->BALSACOPTS)
          || (project->BREEZESIMOPTS && *project->BREEZESIMOPTS) || (project->BREEZELINKOPTS && *project->BREEZELINKOPTS))
        {
            fprintf (stream, "    (defines\n");

            if (project->BALSACOPTS && *project->BALSACOPTS)
                fprintf (stream, "        (define \"BALSACOPTS\" \"%s\")\n", project->BALSACOPTS);
            if (project->BREEZESIMOPTS && *project->BREEZESIMOPTS)
                fprintf (stream, "        (define \"BREEZESIMOPTS\" \"%s\")\n", project->BREEZESIMOPTS);
            if (project->BREEZELINKOPTS && *project->BREEZELINKOPTS)
                fprintf (stream, "        (define \"BREEZELINKOPTS\" \"%s\")\n", project->BREEZELINKOPTS);
            {
                GList *tmp = project->defines;

                for (; tmp; tmp = tmp->next)
                {
                    PtrDefine def = (PtrDefine) (tmp->data);

                    fprintf (stream, "        (define \"%s\" \"%s\")\n", def->name, def->value);
                }
            }

            fprintf (stream, "    )\n");
        }

        fprintf (stream, "    (files\n");
        while (files)
        {
            PtrBalsaFile file = BALSA_FILE (files->data);
            char *filename = g_strdup (file->name ? file->name : "");
            char *prefix = FindPrefixPath (filename,
              project->importPath);

            if (prefix)
            {                   /* has a prefix, change the filename to a dotted path */
                char *filenamePtr = filename + strlen (prefix);

                while (*filenamePtr == G_DIR_SEPARATOR)
                    filenamePtr++;
                filename = g_strdup (filenamePtr);
                filenamePtr = filename;

                while (*filenamePtr)
                {
                    if (*filenamePtr == G_DIR_SEPARATOR)
                        *filenamePtr = '.';
                    else if (*filenamePtr == '.')
                    {
                        *filenamePtr = '\0';
                        break;
                    }
                    filenamePtr++;
                }
            }

            ConvertSlashedToDottedName (filename);

            if (file->nature == BalsaFile_File
              && (file->info.balsaFile.testFixtures
                || file->info.balsaFile.implementations || file->info.balsaFile.builtinLib))
            {                   /* have some tests */
                GList *tests = file->info.balsaFile.testFixtures;

                fprintf (stream, "        (top-level \"%s\"\n", filename);
                if (file->info.balsaFile.builtinLib)
                    WriteBalsaProjectBuiltinLib (stream, "            ", BUILTINLIB (file->info.balsaFile.builtinLib));
                while (tests)
                {
                    WriteBalsaProjectTest (stream, "            ", BALSA_TEST (tests->data));
                    tests = tests->next;
                }
                tests = file->info.balsaFile.implementations;
                while (tests)
                {
                    WriteBalsaProjectImplementation (stream, "            ", IMPLEMENTATION (tests->data));
                    tests = tests->next;
                }
                fprintf (stream, "        )\n");
            } else
                fprintf (stream, "        \"%s\"\n", filename);

            if (prefix)
                g_free (filename);
            files = files->next;
        }
        fprintf (stream, "    )\n");
        fprintf (stream, ")\n");

        fclose (stream);
        project->dirty = FALSE; /* declare the project to now be clean (written) */
        g_free (importPath);
    }
    g_free (filename);
}

/* ReadBalsaProjectFilePort : read a port from the given node and
	return the constructed BalsaPort */
PtrBalsaTestComponent ReadBalsaProjectFileTestComponent (PtrTMPNode node)
{
    PtrBalsaTestComponent ret = NULL;
    gboolean isInput = TMPIsHeaded (node, "input-port");
    GList *elements = node->body.list;

    if (TMPIsHeaded (node, "sync-port"))
    {

        if (TMP_NODE (elements->next->data)->type != TMPString)
        {
            fprintf (stderr, "ReadBalsaProjectFileTestComponent: expecting " "(sync-port \"portName\")\n");
        } else
        {
            ret = NewBalsaTestComponent (BalsaTestComponent_Sync);
            ret->portNames = g_list_append (NULL, g_strdup (TMP_NODE (elements->next->data)->body.string));
        }
    } else if (isInput || TMPIsHeaded (node, "output-port"))
    {                           /* ({in,out}put-port "name" srcDst "value") */
        if (g_list_length (elements) < 4
          || TMP_NODE (elements->next->data)->type != TMPString
          || TMP_NODE (elements->next->next->data)->type != TMPSymbol || TMP_NODE (elements->next->next->next->data)->type != TMPString)
        {
            fprintf (stderr, "ReadBalsaProjectFileTestComponent: expecting " "({in,out}put-port \"portName\" srcDst \"value\")\n");
        } else
        {
            gboolean boundToFile = strcmp (TMP_NODE (elements->next->next->data)->body.string,
              "file") == 0;

            ret =
              NewBalsaTestComponent ((isInput
                ? (boundToFile ?
                  BalsaTestComponent_InputFromFile :
                  BalsaTestComponent_InputFromValue) : (boundToFile ? BalsaTestComponent_OutputToFile : BalsaTestComponent_OutputToStdout)));
            ret->portNames = g_list_append (NULL, g_strdup (TMP_NODE (elements->next->data)->body.string));

            ret->value = g_strdup (TMP_NODE (elements->next->next->next->data)->body.string);

            if (g_list_length (elements) >= 5)
            {
                char *tmp;
                char *txt = TMP_NODE (elements->next->next->next->next->data)->body.string;

                if (!strchr (txt, ','))
                    tmp = g_strdup_printf ("%s,%s", ret->value, txt);
                else
                    tmp = g_strdup_printf ("%s,\"%s\"", ret->value, txt);
                g_free (ret->value);
                ret->value = tmp;
            }
            if (g_list_length (elements) >= 6)
            {
                char *tmp;
                char *txt = TMP_NODE (elements->next->next->next->next->next->data)->body.string;

                if (!strchr (txt, ','))
                    tmp = g_strdup_printf ("%s,%s", ret->value, txt);
                else
                    tmp = g_strdup_printf ("%s,\"%s\"", ret->value, txt);
                g_free (ret->value);
                ret->value = tmp;
            }
            if (g_list_length (elements) >= 7)
            {
                char *tmp;
                char *txt = TMP_NODE (elements->next->next->next->next->next->next->data)->body.string;

                if (!strchr (txt, ','))
                    tmp = g_strdup_printf ("%s,%s", ret->value, txt);
                else
                    tmp = g_strdup_printf ("%s,\"%s\"", ret->value, txt);
                g_free (ret->value);
                ret->value = tmp;
            }

            /* source/destination: 'file 'log 'value */
        }
    } else if (TMPIsHeaded (node, "memory-component"))
    {
        GList *elements = node->body.list;

        ret = NewBalsaTestComponent (BalsaTestComponent_Memory);
        ret->portNames = g_list_append (NULL, g_strdup (TMP_NODE (elements->next->data)->body.string));
        ret->portNames = g_list_append (ret->portNames, g_strdup (TMP_NODE (elements->next->next->data)->body.string));
        ret->portNames = g_list_append (ret->portNames, g_strdup (TMP_NODE (elements->next->next->next->data)->body.string));

        ret->value = g_strdup (TMP_NODE (elements->next->next->next->next->data)->body.string);
    } else
    {
        fprintf (stderr,
          "ReadBalsaProjectFileTestComponent: expecting (sync-port \"portName\") or " "({in,out}put-port \"portName\" srcDst \"value\")\n");
    }

    return ret;
}

PtrImplementation ReadImplementation (GList * elements, PtrBalsaFile matchingBalsaFile, PtrBalsaTest matchingBalsaTest)
{
    PtrImplementation ret = NULL;

    ret = NewImplementationWithName (TMP_NODE (elements->next->data)->body.string);

    elements = elements->next->next; /* pointing just after the name */
    while (elements)
    {
        if (elements && TMPIsHeaded (TMP_NODE (elements->data), "balsa-netlist-options")) /* (balsa-netlist-options ...) */
            ret->balsaNetlistOptions = g_strdup (((PtrTMPNode) ((PtrTMPNode) (elements->data))->body.list->next->data)->body.string);
        if (elements && TMPIsHeaded (TMP_NODE (elements->data), "make-impl-test-options")) /* (balsa-netlist-options ...) */
            ret->makeImplTestOptions = g_strdup (((PtrTMPNode) ((PtrTMPNode) (elements->data))->body.list->next->data)->body.string);
        if (elements && TMPIsHeaded (TMP_NODE (elements->data), "technology")) /* (technology ...) */
            ret->technology = g_strdup (((PtrTMPNode) ((PtrTMPNode) (elements->data))->body.list->next->data)->body.string);

        if (elements && TMPIsHeaded (TMP_NODE (elements->data), "dump-file")) /* (DumpFile ...) */
            ret->DumpFile = g_strdup (((PtrTMPNode) ((PtrTMPNode) (elements->data))->body.list->next->data)->body.string);

        elements = elements->next;
    }

    ret->matchingBalsaFile = matchingBalsaFile;
    ret->matchingBalsaTest = matchingBalsaTest;
    return ret;
}

/* ReadBalsaProjectFileTest : read a (test ...) member from the given node and
	return the constructed BalsaTest */
/*PtrBalsaTest*/ void *
ReadBalsaProjectFileTest (PtrTMPNode node, PtrBalsaFile matchingBalsaFile, BalsaProjectEntryNature * type)
{
    void *result;

    MemoryComponentCount = 0;

    if (TMPIsHeaded (node, "test"))
    {                           /* (test "fixtureName" "procName" port1 port2 ...) */
        PtrBalsaTest ret = NULL;
        GList *elements = node->body.list;

        *type = BalsaProjectEntry_Test;

        if (g_list_length (elements) < 3
          || TMP_NODE (elements->next->data)->type != TMPString || TMP_NODE (elements->next->next->data)->type != TMPString)
            fprintf (stderr, "ReadBalsaProjectFileFile: expecting " "(top-level \"filename\" test1 test2 ...)\n");
        else
        {
            ret = NewBalsaTest (TMP_NODE (elements->next->data)->body.string);
            ret->procName = g_strdup (TMP_NODE (elements->next->next->data)->body.string);
            ret->testComponents = NULL;
            ret->matchingBalsaFile = matchingBalsaFile;
            elements = elements->next->next->next; /* pointing just after the procName */

            while (elements)
            {
                if (TMPIsHeaded (TMP_NODE (elements->data), "ports"))
                {               /* (ports ...) */
                    GList *ports = TMP_NODE (elements->data)->body.list->next;

                    while (ports)
                    {           /* read ports */
                        PtrBalsaTestComponent newPort = ReadBalsaProjectFileTestComponent (TMP_NODE (ports->data));

                        if (newPort)
                            ret->testComponents = g_list_append (ret->testComponents, newPort);
                        ports = ports->next;
                    }
                } else if (TMPIsHeaded (TMP_NODE (elements->data), "implementations"))
                {
                    GList *elts = (TMP_NODE (elements->data))->body.list->next;

                    while (elts)
                    {
                        PtrTMPNode elt = TMP_NODE (elts->data);

                        if (TMPIsHeaded (elt, "implementation"))
                            ret->implementations = g_list_append (ret->implementations, ReadImplementation (elt->body.list, matchingBalsaFile, ret));
                        elts = g_list_next (elts);
                    }
                } else if (TMPIsHeaded (TMP_NODE (elements->data), "command-line-options"))
                {
                    ret->commandLineOptions = g_strdup (((PtrTMPNode) ((PtrTMPNode) (elements->data))->body.list->next->data)->body.string);
                }

                elements = elements->next;
            }
        }
        result = (void *) ret;
    } else if (TMPIsHeaded (node, "lard-test-harness"))
    {                           /* (lard-test-harness "name" "fileName") */
        fprintf (stderr, "ReadBalsaProjectFileFile: skipping lard-test-harness\n");
    } else if (TMPIsHeaded (node, "implementation"))
    {                           /* (implementation "name" "balsa-netlist-options") */
        GList *elements = node->body.list;

        *type = BalsaProjectEntry_Implementation;
        result = (void *) ReadImplementation (elements, matchingBalsaFile, NULL);
    } else if (TMPIsHeaded (node, "builtin-lib"))
    {
        GList *elements = node->body.list;
        PtrBuiltinLib ret = NULL;

        *type = BalsaProjectEntry_BuiltinLib;
        ret = NewBuiltinLibWithName (TMP_NODE (elements->next->data)->body.string);

        elements = elements->next->next; /* pointing just after the name */
        while (elements)
        {
            if (elements && TMPIsHeaded (TMP_NODE (elements->data), "compile-options"))
                ret->compileOptions = g_strdup (((PtrTMPNode) ((PtrTMPNode) (elements->data))->body.list->next->data)->body.string);
            else if (elements && TMPIsHeaded (TMP_NODE (elements->data), "source-files"))
            {
                GList *elements2 = ((PtrTMPNode) (elements->data))->body.list->next;

                for (; elements2; elements2 = elements2->next)
                    ret->sourceFilenames = g_list_append (ret->sourceFilenames, g_strdup (((PtrTMPNode) (elements2->data))->body.string));
            }
            elements = elements->next;
        }

        ret->matchingBalsaFile = matchingBalsaFile;
        result = (void *) ret;
    } else
    {
        fprintf (stderr,
          "ReadBalsaProjectFileFile: expecting "
          "(test \"fixtureName\" \"procName\" port1 port2 ...) OR (implementation \"name\")\n");
    }

    return result;
}

/* ReadBalsaProjectFileFile : read a (files ...) member out of the given node and
	return the constructed BalsaFile */
PtrBalsaFile ReadBalsaProjectFileFile (GList * paths, PtrTMPNode node)
{
    PtrBalsaFile ret = NULL;

    if (node->type == TMPString)
    {                           /* "filename" */
        ret = NewBalsaFile ();
        //        ret->name = GetWithoutExtension (node->body.string);
        ret->name = g_strdup (node->body.string);
        ConvertDottedToSlashedName (ret->name);
    } else if (TMPIsHeaded (node, "top-level"))
    {                           /* (top-level "name" test1 test2 ...) */
        GList *elements = node->body.list;

        if (g_list_length (elements) < 2 || TMP_NODE (elements->next->data)->type != TMPString)
            fprintf (stderr, "ReadBalsaProjectFileFile: expecting " "(top-level \"filename\" test1 test2 ...)\n");
        else
        {
            ret = NewBalsaFile (); /* set the file name */
            ret->name = (char *) GetWithoutExtension (TMP_NODE (elements->next->data)->body.string);
            ConvertDottedToSlashedName (ret->name);
            elements = elements->next->next;
            ret->info.balsaFile.testFixtures = NULL;
            ret->info.balsaFile.implementations = NULL;
            ret->info.balsaFile.builtinLib = NULL;

            while (elements)
            {                   /* read test fixtures */
                BalsaProjectEntryNature type;
                void *newTest = ReadBalsaProjectFileTest (TMP_NODE (elements->data),
                  ret,
                  &type);

                if (newTest)
                    switch (type)
                    {
                    case BalsaProjectEntry_Test:
                        ret->info.balsaFile.testFixtures = g_list_append (ret->info.balsaFile.testFixtures, newTest);
                        break;

                    case BalsaProjectEntry_Implementation:
                        ((PtrImplementation) newTest)->matchingBalsaFile = ret;
                        ret->info.balsaFile.implementations = g_list_append (ret->info.balsaFile.implementations, newTest);
                        break;

                    case BalsaProjectEntry_BuiltinLib:
                        ((PtrBuiltinLib) newTest)->matchingBalsaFile = ret;
                        ret->info.balsaFile.builtinLib = newTest;
                        break;

                    default:
                        printf ("Error in reading the project file\n");
                    }

                elements = elements->next;
            }
        }
    } else
    {
        fprintf (stderr, "ReadBalsaProjectFileFile: expecting \"filename\" or " "(top-level \"filename\" test1 test2 ...)\n");
    }

    if (ret)
    {                           /* fixup filename */
        if (*ret->name == G_DIR_SEPARATOR) /* absolute */
            ret->name = g_strdup (ret->name);
        else
            ret->name = g_strdup (ret->name);
    }

    return ret;
}

/* ReadBalsaProjectFile : read in a Balsa project file (called Project)
	and make a new BalsaProject, return NULL on error */
PtrBalsaProject ReadBalsaProjectFile (const char *directory)
{
    PtrBalsaProject project;
    PtrTMPNode fileContents;

    char *filename = g_malloc (strlen (directory) + 9);
    gboolean abort = FALSE;

    sprintf (filename, "%s/Project", directory);

#ifdef DEBUG
    fprintf (stderr, "opening project: %s\n", filename);
#endif

    fileContents = ParseCompleteFileAsTMPNode (filename);
    if (!fileContents)
        project = NULL;
    else
    {
#ifdef DEBUG
        PrintTMPNode (stderr, fileContents);
#endif

        project = NewBalsaProject ();
        project->directory = g_strdup (directory);
        project->dirty = FALSE;
        project->dirty_and_need_make_clean = FALSE;
        g_free (project->name);

        /* file format: (balsa-project elem1 elem2 ...) */
        if (!TMPIsHeaded (fileContents, "balsa-project"))
        {
            fprintf (stderr, "ReadBalsaProjectFile: " "invalid header on project file `%s'\n", filename);
            abort = TRUE;
        } else
        {
            GList *fileDefns = g_list_next (fileContents->body.list);

            while (fileDefns)
            {
                /* (symbol ...) ? */
                if (TMP_NODE (fileDefns->data)->type != TMPList || TMP_NODE (TMP_NODE (fileDefns->data)->body.list->data)->type != TMPSymbol)
                {
                    fprintf (stderr, "ReadBalsaProjectFile: expecting a headed list \n");
                    abort = TRUE;
                } else
                {
                    char *symbol = TMP_NODE (TMP_NODE (fileDefns->data)->body.list->data)->body.string;
                    GList *data = g_list_next (TMP_NODE (fileDefns->data)->body.list);

                    if (strcmp (symbol, "name") == 0)
                    {           /* (name "name") */
                        if (g_list_length (data) != 1 || TMP_NODE (data->data)->type != TMPString)
                        {
                            fprintf (stderr, "ReadBalsaProjectFile: expecting (name \"projectName\")\n");
                            abort = TRUE;
                        } else
                        {
                            project->name = g_strdup (TMP_NODE (data->data)->body.string);
                        }
                    } else if (strcmp (symbol, "simulation-system") == 0)
                    {           /* (simulation-system "simulation-system") */
                        if (g_list_length (data) != 1 || TMP_NODE (data->data)->type != TMPString)
                        {
                            abort = TRUE;
                        } else
                        {
                            if (!strcmp (TMP_NODE (data->data)->body.string, "breezesim"))
                                project->simulationSystem = breezesim;
                            else
                            {
                                printfConsole_s (
                                	"ReadBalsaProjectFile: simulation system `%s' not supported. Using breezesim\n",
									TMP_NODE (data->data)->body.string);
                                project->simulationSystem = breezesim;
                            }
                        }
                        if (abort == TRUE)
                        {
                            fprintf (stderr, "ReadBalsaProjectFile: expecting (simulation-system <\"breezesim\">)\n");
                        }
                    } else if (strcmp (symbol, "directory") == 0)
                    {           /* (directory "dir") */
                        if (g_list_length (data) != 1 || TMP_NODE (data->data)->type != TMPString)
                        {
                            fprintf (stderr, "ReadBalsaProjectFile: expecting (directory \"projectDir\")\n");
                            abort = TRUE;
                        } else
                        {
                        }
                    } else if (strcmp (symbol, "BALSACOPTS") == 0)
                    {           /* (BALSACOPTS "BALSACOPTS") */
                        if (g_list_length (data) != 1 || TMP_NODE (data->data)->type != TMPString)
                        {
                            fprintf (stderr, "ReadBalsaProjectFile: expecting (BALSACOPTS \"string\")\n");
                            abort = TRUE;
                        } else
                        {
                            project->BALSACOPTS = g_strdup (TMP_NODE (data->data)->body.string);
                        }
                    } else if (strcmp (symbol, "BREEZESIMOPTS") == 0)
                    {           /* (BREEZESIMOPTS "BREEZESIMOPTS") */
                        if (g_list_length (data) != 1 || TMP_NODE (data->data)->type != TMPString)
                        {
                            fprintf (stderr, "ReadBalsaProjectFile: expecting (BREEZESIMOPTS \"string\")\n");
                            abort = TRUE;
                        } else
                        {
                            project->BREEZESIMOPTS = g_strdup (TMP_NODE (data->data)->body.string);
                        }
                    } else if (strcmp (symbol, "BREEZELINKOPTS") == 0)
                    {           /* (BREEZELINKOPTS "BREEZELINKOPTS") */
                        if (g_list_length (data) != 1 || TMP_NODE (data->data)->type != TMPString)
                        {
                            fprintf (stderr, "ReadBalsaProjectFile: expecting (BREEZELINKOPTS \"string\")\n");
                            abort = TRUE;
                        } else
                        {
                            project->BREEZELINKOPTS = g_strdup (TMP_NODE (data->data)->body.string);
                        }
                    } else if (strcmp (symbol, "import-path") == 0)
                    {           /* (import-path "dir" "dir" ...) */
                        GList *nodeIter = data;
                        GList *newImportPath = NULL;

                        while (nodeIter && !abort)
                        {
                            if (TMP_NODE (nodeIter->data)->type != TMPString)
                            {
                                fprintf (stderr, "ReadBalsaProjectFile: expecting " "(import-path \"dir\" \"dir\" ...)\n");
                                abort = TRUE;
                            } else
                            {
                                newImportPath = g_list_prepend (newImportPath, g_strdup (TMP_NODE (nodeIter->data)->body.string));
                                nodeIter = nodeIter->next;
                            }
                        }
                        project->importPath = newImportPath;
                    } else if (strcmp (symbol, "defines") == 0)
                    {           /* (defines (define "xx" "yy") (define "xx" "yy") ... ) */
                        GList *nodeIter = data;

                        while (nodeIter)
                        {
                            PtrTMPNode node = TMP_NODE (nodeIter->data);

                            if (TMPIsHeaded (node, "define"))
                            {
                                GList *elements = node->body.list;
                                char *name = TMP_NODE (elements->next->data)->body.string;
                                char *value = TMP_NODE (elements->next->next->data)->body.string;

                                if (!strcmp (name, "BALSACOPTS"))
                                    project->BALSACOPTS = g_strdup (value);
                                else if (!strcmp (name, "BREEZESIMOPTS"))
                                    project->BREEZESIMOPTS = g_strdup (value);
                                else if (!strcmp (name, "BREEZELINKOPTS"))
                                    project->BREEZELINKOPTS = g_strdup (value);
                                else
                                    project->defines = g_list_append (project->defines, NewDefine (name, value));
                            } else
                                break;

                            nodeIter = nodeIter->next;
                        }
                    } else if (strcmp (symbol, "files") == 0)
                    {           /* (files file1 file2) */
                        GList *nodeIter = data;
                        GList *newFileList = NULL;

                        while (nodeIter && !abort)
                        {
                            PtrBalsaFile newFile = ReadBalsaProjectFileFile (project->importPath, TMP_NODE (nodeIter->data));

                            if (!newFile)
                                abort = TRUE;
                            else
                            {
                                newFileList = g_list_append (newFileList, (gpointer) newFile);
                                nodeIter = nodeIter->next;
                            }
                        }
                        project->files = newFileList;
                    }
                }
                fileDefns = g_list_next (fileDefns);
            }
        }

        /* FIXME, check slots */

        DeleteTMPNode (fileContents);
    }
    g_free (filename);
    if (abort && project)
    {
        g_free (project);
        project = NULL;
    }

    return project;
}

/* ReadBalsaProjectListFile : read in the project list file.  This is usually
	used to read in the ~/.balsa-projects list on startup.  Opens all the projects,
	returns a list of them.  Returns FALSE on error or empty project list. */
GList *ReadBalsaProjectListFile (char *filename)
{
    PtrTMPNode fileContents = NULL;
    GList *projectDirectories;
    GList *projects = NULL;

    fileContents = ParseCompleteFileAsTMPNode (filename);

    if (!fileContents)
        return NULL;

    /* file format: (balsa-projects "name1" "name2" ...) */
    if (!TMPIsHeaded (fileContents, "balsa-projects"))
    {
        fprintf (stderr, "ReadBalsaProjectListFile: " "invalid header on project list file `%s'\n", filename);
        projects = NULL;
    } else
    {
        projectDirectories = fileContents->body.list->next;
        while (projectDirectories)
        {
            PtrBalsaProject newProject;

            if (TMP_NODE (projectDirectories->data)->type == TMPString)
            {
                newProject = ReadBalsaProjectFile (TMP_NODE (projectDirectories->data)->body.string);
                if (newProject)
                    projects = g_list_append (projects, (gpointer) newProject);
            } else
                fprintf (stderr, "ReadBalsaProjectListFile: expecting project directory name\n");
            projectDirectories = projectDirectories->next;
        }

#ifdef DEBUG
        PrintTMPNode (stderr, fileContents);
        fprintf (stderr, "\n\n");
#endif
    }

    DeleteTMPNode (fileContents);
    return projects;
}

/* WriteBalsaProjectListFile : write the given project list into the given file,
	return TRUE if successfull, FALSE if not */
gboolean WriteBalsaProjectListFile (char *filename, GList * projects)
{
    FILE *file = fopen (filename, "w");

    if (!file)
        return FALSE;

    fprintf (file, "(balsa-projects\n");
    while (projects)
    {
        fprintf (file, "\t\"%s\"\n", BALSA_PROJECT (projects->data)->directory);
        projects = projects->next;
    }
    fprintf (file, ")\n");

    fclose (file);
    return TRUE;
}

/* FIXME, add lots of stuff to handle tests/ports ... */

/* UpdateBalsaProjectFromTreeNode : update the balsa project `project'
	by traversing the file view tree */
void UpdateBalsaProjectFromTreeNode (GtkCTree * tree, GtkCTreeNode * node, PtrBalsaProject project)
{
    PtrBalsaProjectEntry entry = BALSA_PROJECT_ENTRY (gtk_ctree_node_get_row_data (tree, node));

    switch (entry->nature)
    {
    case BalsaProjectEntry_File:
        {
            PtrBalsaFile file = BALSA_FILE (entry->data);

            project->files = g_list_append (project->files, (gpointer) entry->data);
            /* clear out the file test list, this will be updated in further nodes */
            if (file->info.balsaFile.testFixtures)
                g_list_free (file->info.balsaFile.testFixtures);
            file->info.balsaFile.testFixtures = NULL;
            file->info.balsaFile.implementations = NULL;
        }
        break;
    case BalsaProjectEntry_Test:
        {                       /* tests have a file parent node */
            PtrBalsaFile file = BALSA_FILE (BALSA_PROJECT_ENTRY (gtk_ctree_node_get_row_data (tree,
                  GTK_CTREE_ROW (node)->parent))->data);
            PtrBalsaTest test = BALSA_TEST (entry->data);

            file->info.balsaFile.testFixtures = g_list_append (file->info.balsaFile.testFixtures, (gpointer) test);
            /* clear out the port list, this too will be updated */
            if (test->testComponents)
                g_list_free (test->testComponents);
            test->testComponents = NULL;
        }
        break;
    case BalsaProjectEntry_TestComponent:
        {                       /* ports have a test parent node */
            PtrBalsaTest test = BALSA_TEST (BALSA_PROJECT_ENTRY (gtk_ctree_node_get_row_data (tree,
                  GTK_CTREE_ROW (node)->parent))->data);
            PtrBalsaTestComponent testComponent = BALSA_TESTCOMPONENT (entry->data);

            test->testComponents = g_list_append (test->testComponents, (gpointer) testComponent);
        }
        break;
    default:
        break;
    }
}

/* UpdateBalsaProjectFromFileView : update the project from the project's ctree file view */
void UpdateBalsaProjectFromFileView (PtrBalsaProject project)
{
    if (project->files)
        g_list_free (project->files); /* get rid of old list */
    project->files = NULL;

    gtk_ctree_pre_recursive (GTK_CTREE
      (LookupWidget (MainWindow, "Tree1")), NULL, GTK_CTREE_FUNC (UpdateBalsaProjectFromTreeNode), (gpointer) project);
}

/* DeleteBalsaProjectList : deallocate everything to do with a given project list */
void DeleteBalsaProjectList (GList * projects)
{
    GList *projectsIter = projects;

    while (projectsIter)
    {
        DeleteBalsaProject (BALSA_PROJECT (projectsIter->data));
        projectsIter = projectsIter->next;
    }
    g_list_free (projects);
}

/* BalsaProjectNeedsToBeSaved : returns TRUE if the project is dirty and
	has a directory */
gboolean BalsaProjectNeedsToBeSaved (PtrBalsaProject project)
{
    return (project->directory && *(project->directory) && project->dirty ? TRUE : FALSE);
}

gboolean CheckIfNeedToRebuildMakefile (void)
{
    struct stat infosMakefile, infosProject;

    if (stat ("Makefile", &infosMakefile) != 0)
        return TRUE;
    stat ("Project", &infosProject);

    if (infosMakefile.st_size == 0)
        return TRUE;
    if (infosMakefile.st_mtime < infosProject.st_mtime)
        return TRUE;
    return FALSE;
    //  return CurrentBalsaProject->needToRebuildMakefile;
}

gboolean IsFileInProject (char *filename)
{
    GList *ptr = CurrentBalsaProject->files;

    for (; ptr; ptr = ptr->next)
    {
        PtrBalsaFile file = ptr->data;

        if (!strcmp (file->name, filename))
            return TRUE;
        if (!strcmp (ConvertToBalsaFilename (file->name), filename))
            return TRUE;
    }

    return FALSE;
}
