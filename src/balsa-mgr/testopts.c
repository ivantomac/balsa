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

	`testopts.c'
	Test options dialogue

*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "testopts.h"
#include "paths.h"
#include "utils.h"
#include "icons.h"
#include "support.h"
#include <gdk/gdkkeysyms.h>
#include <format/format.h>
#include <format/external.h>

/* TEST_COMPONENT... : column count/positions for test testComponent clist */
#define TEST_COMPONENT_COLUMN_COUNT (3)
#define TEST_COMPONENT_TYPE_COLUMN (0)
#define TEST_COMPONENT_NAME_COLUMN (1)
#define TEST_COMPONENT_VALUE_COLUMN (2)

gboolean unactivate_change_events = FALSE;
static gboolean TestDefinitionsEntryDisabled = FALSE;

/*
 * TestSelectedTestComponentSense : returns the currently selected
 * testComponent nature on the Sense radiobuttons
 */
BalsaTestComponentNature TestSelectedTestComponentSense (GtkWidget * dialogue)
{
    BalsaTestComponentNature ret = BalsaTestComponent_Undefined;

    if (GTK_TOGGLE_BUTTON (LookupWidget (dialogue, "SyncToggle"))->active)
        ret = BalsaTestComponent_Sync;
    else if (GTK_TOGGLE_BUTTON (LookupWidget (dialogue, "InputFromFileToggle"))->active)
        ret = BalsaTestComponent_InputFromFile;
    else if (GTK_TOGGLE_BUTTON (LookupWidget (dialogue, "InputFromValueToggle"))->active)
        ret = BalsaTestComponent_InputFromValue;
    else if (GTK_TOGGLE_BUTTON (LookupWidget (dialogue, "OutputToFileToggle"))->active)
        ret = BalsaTestComponent_OutputToFile;
    else if (GTK_TOGGLE_BUTTON (LookupWidget (dialogue, "OutputToStdoutToggle"))->active)
        ret = BalsaTestComponent_OutputToStdout;
    else if (GTK_TOGGLE_BUTTON (LookupWidget (dialogue, "MemoryComponentToggle"))->active)
        ret = BalsaTestComponent_Memory;

    return ret;
}

/*
 * ChoosePixmapForTestComponentSenseAndSource : choose an appropriate icon
 * for the given sense/source combination. Set *pixmap and *bitmap
 */
void ChoosePixmapForTestComponentSenseAndSource (BalsaTestComponentNature sense, GdkPixmap ** pixmap, GdkBitmap ** bitmap)
{
    switch (sense)
    {
    case BalsaTestComponent_Sync:
        *pixmap = SyncPortPixmap;
        *bitmap = SyncPortMask;
        break;
    case BalsaTestComponent_InputFromFile:
        *pixmap = InputFilePortPixmap;
        *bitmap = InputFilePortMask;
        break;
    case BalsaTestComponent_InputFromValue:
        *pixmap = InputValuePortPixmap;
        *bitmap = InputValuePortMask;
        break;
    case BalsaTestComponent_OutputToFile:
        *pixmap = OutputFilePortPixmap;
        *bitmap = OutputFilePortMask;
        break;
    case BalsaTestComponent_OutputToStdout:
        *pixmap = OutputStdoutPortPixmap;
        *bitmap = OutputStdoutPortMask;
        break;
    case BalsaTestComponent_Memory:
        *pixmap = MemoryComponentPixmap;
        *bitmap = MemoryComponentMask;
        break;
    case BalsaTestComponent_Undefined:
        *pixmap = UndefinedPortPixmap;
        *bitmap = UndefinedPortMask;
        break;
    }
}

/*
 * UpdateTestDialogueWidgets : update the radio button settings in the test
 * window
 */
void UpdateTestDialogueWidgets (GtkWidget * dialogue, GtkCList * list, int row, gboolean dontToggleButtons)
{
    GdkPixmap *pixmap = 0;
    GdkBitmap *bitmap = 0;

    if (row == TEST_COMPONENT_NO_SELECTION)
    {                           /* if nothing is selected disable a lot of buttons/entries */
        gtk_widget_set_sensitive (LookupWidget (dialogue, "ComponentPropertiesBox"), FALSE);
        gtk_widget_set_sensitive (LookupWidget (dialogue, "TestPortFileBrowseButton"), FALSE);
        gtk_widget_set_sensitive (LookupWidget (dialogue, "TestPortFileBrowseButton"), FALSE);
        gtk_widget_set_sensitive (LookupWidget (dialogue, "ComponentTypeFrame"), FALSE);
        gtk_widget_set_sensitive (LookupWidget (dialogue, "TestPortValueEntry"), FALSE);
        //gtk_widget_set_sensitive (LookupWidget (dialogue, "ActivatedTestComponentButton"), FALSE);
        return;
    }
    if (row != TEST_COMPONENT_NEW)
    {                           /* TEST_COMPONENT_NEW means `entering new testComponent' */
        unactivate_change_events = TRUE;
        gtk_clist_get_pixmap (GTK_CLIST (list), row, TEST_COMPONENT_TYPE_COLUMN, &pixmap, &bitmap);

        if (pixmap == SyncPortPixmap)
        {
            if (!dontToggleButtons)
            {
                gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (LookupWidget (dialogue, "SyncToggle")), TRUE);
            }
        } else
        {
            if (!dontToggleButtons)
            {
                char *toggleName =
                  (pixmap ==
                  InputFilePortPixmap ? "InputFromFileToggle" : pixmap ==
                  InputValuePortPixmap ? "InputFromValueToggle" : pixmap ==
                  OutputFilePortPixmap ? "OutputToFileToggle" : pixmap ==
                  OutputStdoutPortPixmap ? "OutputToStdoutToggle" : pixmap ==
                  MemoryComponentPixmap ? "MemoryComponentToggle" : "UndefinedPortToggle");
                gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (LookupWidget (dialogue, toggleName)), TRUE);
            }
        }

        char *nameText = "", *valueText = "";

        gtk_clist_get_text (list, row, TEST_COMPONENT_NAME_COLUMN, &nameText);
        gtk_clist_get_text (list, row, TEST_COMPONENT_VALUE_COLUMN, &valueText);
        if (pixmap != MemoryComponentPixmap)
        {
            gtk_entry_set_text (GTK_ENTRY (LookupWidget (dialogue, "TestPortEntry")), nameText);

            GList *params = CommaStringToList (valueText);

            if (pixmap != OutputStdoutPortPixmap)
                gtk_entry_set_text (GTK_ENTRY (LookupWidget (dialogue, "TestPortValueEntry")), (params) ? params->data : "");

            if (pixmap == OutputFilePortPixmap || pixmap == OutputStdoutPortPixmap)
            {
                float value;

                value = (float) atoi ((params && params->next) ? params->next->data : "");
                gtk_spin_button_set_value (GTK_SPIN_BUTTON (LookupWidget (dialogue, "TestRadixSpinButton")), value);
                value = (float) atoi ((params && params->next && params->next->next) ? params->next->next->data : "");
                gtk_spin_button_set_value (GTK_SPIN_BUTTON (LookupWidget (dialogue, "TestUSpacingSpinButton")), value);
                value =
                  (float) atoi ((params && params->next && params->next->next && params->next->next->next) ? params->next->next->next->data : "");
                gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON
                  (LookupWidget (dialogue, "TestShowLeadingZeroesCheckButton")), (value != 0) ? TRUE : FALSE);
            }
            g_list_free (params);
        } else
        {
            gchar **names = g_strsplit (nameText, ",", 3);

            gtk_entry_set_text (GTK_ENTRY (LookupWidget (dialogue, "AddressPortEntry")), (names[0]) ? names[0] : "");
            gtk_entry_set_text (GTK_ENTRY (LookupWidget (dialogue, "WritePortEntry")), (names[0] && names[1]) ? names[1] : "");
            gtk_entry_set_text (GTK_ENTRY
              (LookupWidget (dialogue, "ReadPortEntry")), (names[0] && names[1] && names[2]) ? names[2] : "");
            g_strfreev (names);
            gtk_entry_set_text (GTK_ENTRY (LookupWidget (dialogue, "MemoryFilenameEntry")), valueText ? valueText : "");
        }

        unactivate_change_events = FALSE;
    }
    BalsaTestComponentNature nature = TestSelectedTestComponentSense (dialogue);

    switch (nature)
    {
    case BalsaTestComponent_Sync:
    case BalsaTestComponent_OutputToStdout:
    case BalsaTestComponent_Undefined:
        gtk_widget_set_sensitive (LookupWidget (dialogue, "TestPortFileBrowseButton"), FALSE);
        gtk_widget_set_sensitive (LookupWidget (dialogue, "TestPortValueEntry"), FALSE);
        break;
    case BalsaTestComponent_InputFromFile:
    case BalsaTestComponent_InputFromValue:
    case BalsaTestComponent_OutputToFile:
        gtk_widget_set_sensitive (LookupWidget (dialogue, "TestPortFileBrowseButton"), TRUE);
        gtk_widget_set_sensitive (LookupWidget (dialogue, "TestPortValueEntry"), TRUE);
        break;
    case BalsaTestComponent_Memory:
        break;
    }

    switch (nature)
    {
    case BalsaTestComponent_OutputToFile:
    case BalsaTestComponent_OutputToStdout:
    case BalsaTestComponent_Sync:
    case BalsaTestComponent_InputFromFile:
    case BalsaTestComponent_InputFromValue:
    case BalsaTestComponent_Undefined:
        gtk_widget_show (LookupWidget (dialogue, "ComponentPropertiesFrame2"));
        gtk_widget_hide (LookupWidget (dialogue, "ComponentPropertiesFrame3"));
        break;
    case BalsaTestComponent_Memory:
        gtk_widget_hide (LookupWidget (dialogue, "ComponentPropertiesFrame2"));
        gtk_widget_show (LookupWidget (dialogue, "ComponentPropertiesFrame3"));
        break;
    }

    switch (nature)
    {
    case BalsaTestComponent_Sync:
    case BalsaTestComponent_Undefined:
    case BalsaTestComponent_OutputToStdout:
        gtk_widget_hide (LookupWidget (dialogue, "TestPortsValueLabel"));
        gtk_widget_hide (LookupWidget (dialogue, "TestPortValueEntry"));
        gtk_widget_hide (LookupWidget (dialogue, "TestPortFileBrowseButton"));
        break;
    case BalsaTestComponent_InputFromFile:
    case BalsaTestComponent_InputFromValue:
    case BalsaTestComponent_OutputToFile:
        gtk_widget_show (LookupWidget (dialogue, "TestPortsValueLabel"));
        gtk_widget_show (LookupWidget (dialogue, "TestPortValueEntry"));
        gtk_widget_show (LookupWidget (dialogue, "TestPortFileBrowseButton"));
        break;
    case BalsaTestComponent_Memory:
        break;
    }

    switch (nature)
    {
    case BalsaTestComponent_InputFromFile:
    case BalsaTestComponent_OutputToFile:
        gtk_label_set_text (GTK_LABEL (LookupWidget (dialogue, "TestPortsValueLabel")), "Filename");
        gtk_widget_show (LookupWidget (dialogue, "TestPortFileBrowseButton"));
        break;
    case BalsaTestComponent_InputFromValue:
        gtk_label_set_text (GTK_LABEL (LookupWidget (dialogue, "TestPortsValueLabel")), "Value");
        gtk_widget_hide (LookupWidget (dialogue, "TestPortFileBrowseButton"));
        break;
    case BalsaTestComponent_Sync:
    case BalsaTestComponent_OutputToStdout:
    case BalsaTestComponent_Memory:
    case BalsaTestComponent_Undefined:
        break;
    }

    switch (nature)
    {
    case BalsaTestComponent_OutputToFile:
    case BalsaTestComponent_OutputToStdout:
        gtk_widget_show (LookupWidget (dialogue, "TestOptionsFrame"));
        break;
    default:
        gtk_widget_hide (LookupWidget (dialogue, "TestOptionsFrame"));
        break;
    }

    gtk_widget_set_sensitive (LookupWidget (dialogue, "ComponentPropertiesBox"), TRUE);
    gtk_widget_set_sensitive (LookupWidget (dialogue, "ComponentTypeFrame"), TRUE);
}

/*
 * GetBalsaTestComponentListFromTestDialogue : read from the displayed
 * testComponent list and make a new GList of BalsaTestComponent elements
 * with the activated testComponents
 */
GList *GetBalsaTestComponentListFromTestDialogue (GtkCList * list)
{
    GList *testComponents = NULL;
    int rowNo = list->rows - 1;

    while (rowNo >= 0)
    {                           /* iterate (backwards) over rows. Turn each row into a BalsaTestComponent */
        GdkPixmap *pixmap;
        GdkBitmap *mask;
        char *name;
        char *value;
        PtrBalsaTestComponent newTestComponent;

        {
            gtk_clist_get_text (list, rowNo, TEST_COMPONENT_NAME_COLUMN, &name);
            gtk_clist_get_text (list, rowNo, TEST_COMPONENT_VALUE_COLUMN, &value);

            gtk_clist_get_pixmap (list, rowNo, TEST_COMPONENT_TYPE_COLUMN, &pixmap, &mask);

            if (pixmap == SyncPortPixmap)
            {
                newTestComponent = NewBalsaTestComponent (BalsaTestComponent_Sync);
                newTestComponent->value = g_strdup ("");
            } else if (pixmap == InputFilePortPixmap)
            {
                newTestComponent = NewBalsaTestComponent (BalsaTestComponent_InputFromFile);
                newTestComponent->value = g_strdup (value);
            } else if (pixmap == InputValuePortPixmap)
            {
                newTestComponent = NewBalsaTestComponent (BalsaTestComponent_InputFromValue);
                newTestComponent->value = g_strdup (value);
            } else if (pixmap == OutputFilePortPixmap)
            {
                newTestComponent = NewBalsaTestComponent (BalsaTestComponent_OutputToFile);
                newTestComponent->value = g_strdup (value);
            } else if (pixmap == OutputStdoutPortPixmap)
            {
                newTestComponent = NewBalsaTestComponent (BalsaTestComponent_OutputToStdout);
                newTestComponent->value = g_strdup (value);
            } else if (pixmap == MemoryComponentPixmap)
            {
                newTestComponent = NewBalsaTestComponent (BalsaTestComponent_Memory);
                newTestComponent->value = g_strdup (value);
            } else
                goto next;
            newTestComponent->portNames = CommaStringToList (name);
            testComponents = g_list_prepend (testComponents, newTestComponent);
        }
      next:
        rowNo--;
    }
    return testComponents;
}

/*
 * MakeListOfCTreeTestComponentLeaves : iterator function used by
 * GetBalsaTestComponentListFromFileView
 */
void MakeListOfCTreeTestComponentLeaves (GtkCTree * tree, GtkCTreeNode * node, GList ** testComponents)
{
    if (GTK_CTREE_ROW (node)->is_leaf)
    {
        PtrBalsaProjectEntry entry = BALSA_PROJECT_ENTRY (gtk_ctree_node_get_row_data (tree, node));

        if (entry->nature == BalsaProjectEntry_TestComponent)
        {
            gpointer data = entry->data;

            *testComponents = g_list_prepend (*testComponents, data);
        }
    }
}

/*
 * GetBalsaTestComponentListFromFileView : read from the displayed `file
 * view' and make a new GList of BalsaTestComponent elements.  `node' should
 * be the test entry in the file view
 */
GList *GetBalsaTestComponentListFromFileView (GtkCTree * fileView, GtkCTreeNode * testNode)
{
    GList *testComponents = NULL;

    gtk_ctree_pre_recursive (fileView, testNode, GTK_CTREE_FUNC (MakeListOfCTreeTestComponentLeaves), &testComponents);
    return testComponents;
}

/*
 * DeleteRowDataFunc,RemoveCTreeLeaves : functions/Delete wrapper to remove
 * test testComponents
 */
typedef void (*DeleteRowDataFunc) (gpointer);
void RemoveCTreeTestComponentLeaves (GtkCTree * tree, GtkCTreeNode * node, DeleteRowDataFunc deleteFunc)
{
    if (GTK_CTREE_ROW (node)->is_leaf)
    {
        PtrBalsaProjectEntry entry = BALSA_PROJECT_ENTRY (gtk_ctree_node_get_row_data (tree, node));

        if (entry->nature == BalsaProjectEntry_TestComponent)
        {
            gpointer data = entry->data;

            g_free (entry);
            if (data && deleteFunc)
                deleteFunc (data);
            gtk_ctree_remove_node (tree, node);
        }
    }
}

/* UpdateTestFixtureCTreeNode : update the CTree test fixture after edits */
void UpdateTestFixtureCTreeNode (GtkCTree * tree, GtkCTreeNode * testNode)
{
    GtkCTreeNode *testComponentNode;
    PtrBalsaTest test = BALSA_TEST (BALSA_PROJECT_ENTRY (gtk_ctree_node_get_row_data (tree, testNode))->data);
    GList *testComponents = test->testComponents;
    GList *implementations = test->implementations;

    /* remove existing testComponents */
    gtk_ctree_collapse (tree, testNode);
    gtk_ctree_pre_recursive (tree, testNode, GTK_CTREE_FUNC (RemoveCTreeTestComponentLeaves), (DeleteRowDataFunc) NULL); /* don't delete the
                                                                                                                          * testComponents */

    /* update names */
    gtk_ctree_node_set_text (tree, testNode, 0, (test->name ? test->name : ""));
    gtk_ctree_node_set_text (tree, testNode, 1, "");

    while (testComponents)
    {
        GdkPixmap *pixmap;
        GdkBitmap *mask;
        char *testComponentText[2];
        PtrBalsaTestComponent testComponent = BALSA_TESTCOMPONENT (testComponents->data);

        testComponentText[0] = ListToCommaString (testComponent->portNames);

        /* don't convey value for logged outputs */
        testComponentText[1] = testComponent->value;

        ChoosePixmapForTestComponentSenseAndSource (testComponent->nature, &pixmap, &mask);

        testComponentNode = gtk_ctree_insert_node (tree, testNode, NULL, testComponentText, 4, pixmap, mask, pixmap, mask, TRUE, TRUE);
        gtk_ctree_node_set_row_data (tree, testComponentNode,
          (gpointer) NewBalsaProjectEntry (BalsaProjectEntry_TestComponent, (gpointer) testComponent));
        testComponents = testComponents->next;
    }
    gtk_ctree_expand (tree, testNode);

    AddImplementationsToProjectFilesViewTree (testNode, implementations);
}

void TestOpts_FillDefinesList (GtkWidget * dialogue, char *commandLineOptions)
{
    GtkWidget *list = LookupWidget (dialogue, "DefinesList");
    char *strings[2];

    gtk_object_set_data (GTK_OBJECT (dialogue), "SelectedDefine", (gpointer) TEST_OPTS_NO_SELECTION);
    gtk_object_set_data (GTK_OBJECT (dialogue), "DefinesChanged", (gpointer) FALSE);

    if (!commandLineOptions)
        return;

    gchar **split = g_strsplit (commandLineOptions, " ", 0);
    int i, pos = 0;

    for (i = 0; split[i]; i++)
    {
        if (strcmp (split[i], "-D") == 0 && split[i + 1] && split[i + 2])
        {
            strings[0] = split[i + 1];
            strings[1] = split[i + 2];
            gtk_clist_insert (GTK_CLIST (list), pos++, strings);
            i += 2;
        }
    }

    g_strfreev (split);
}

void OnTestOptionsDialogueOK (GtkObject * dialogue)
{
    //PtrBalsaProjectEntry entry = ProjectFileViewSelectedEntry;
    const char *test_name = gtk_entry_get_text (GTK_ENTRY (LookupWidget (GTK_WIDGET (dialogue), "TestNameEntry")));
    PtrBalsaTest test;
    PtrBalsaFile file = NULL;
    PtrBalsaTest testToRemove = gtk_object_get_data (dialogue, "EditedTest");

    if (!test_name || !*test_name)
    {
        //empty test name
        printfConsole ("Error in AddTest : Please specify a test name\n");
        return;
    }
    //now check the test name doesnt already exist (if it is a new test or if the name changed)
    if ((testToRemove == NULL) || strcmp (testToRemove->name, test_name))
    {
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
                    printfConsole ("Error in AddTest : The specified test name is already in use\n");
                    return;
                }
            }
        }
    }
    test = NewBalsaTest (test_name);

    /* update the test */
    test->procName = g_strdup (gtk_entry_get_text (GTK_ENTRY (LookupWidget (GTK_WIDGET (dialogue), "TestProcNameEntry"))));
    test->testComponents = GetBalsaTestComponentListFromTestDialogue (GTK_CLIST (LookupWidget (GTK_WIDGET (dialogue), "TestPortList")));

    /*
     * Test if we are in the case of a 'Test modification' or a 'New
     * test'
     */
    if (gtk_object_get_data (dialogue, "EditedTest") != NULL)
    {
        //Test modification
        PtrBalsaTest testToRemove = gtk_object_get_data (dialogue, "EditedTest");

        test->implementations = testToRemove->implementations;

        file = testToRemove->matchingBalsaFile;
        file->info.balsaFile.testFixtures = g_list_remove (file->info.balsaFile.testFixtures, testToRemove);
    }
    //
    else
        //New test
    {
        //Find the file structure which matches the specified filename
        const char *filename = gtk_entry_get_text (GTK_ENTRY (LookupWidget (GTK_WIDGET (dialogue), "TestFilenameEntry")));
        GList *files;

        for (files = CurrentBalsaProject->files; files; files = files->next)
        {
            PtrBalsaFile file_tmp = (PtrBalsaFile) files->data;

            if ((!strcmp (filename, file_tmp->name))
              || ((!strncmp (filename, file_tmp->name, strlen (file_tmp->name)) && !strcmp (filename + strlen (file_tmp->name), ".balsa"))))
            {
                file = file_tmp;
                break;
            }
        }
    }

    if (!file)
    {
        printfConsole ("Error : impossible to find the specified file name\n");
        return;
    }
    test->matchingBalsaFile = file;
    file->info.balsaFile.testFixtures = g_list_append (file->info.balsaFile.testFixtures, (gpointer) test);

    /* Update defines list */
    {
		GtkCList *DefinesList = GTK_CLIST (LookupWidget (GTK_WIDGET (dialogue), "DefinesList"));
        int rowNo;
        char *definesStr = 0;

        // Clear the previous defines list
        for (rowNo = 0; rowNo < DefinesList->rows; rowNo++)
        {
            char *name, *value;

            gtk_clist_get_text (DefinesList, rowNo, 0, &name);
            gtk_clist_get_text (DefinesList, rowNo, 1, &value);

            if (*name)
            {
                if (definesStr)
                {
                    char *tmp = g_strdup_printf ("%s -D %s %s", definesStr, name, value);

                    g_free (definesStr);
                    definesStr = tmp;
                } else
                    definesStr = g_strdup_printf ("-D %s %s", name, value);
            }
        }

        if (test->commandLineOptions)
            g_free (test->commandLineOptions);

        test->commandLineOptions = definesStr;
    }

    /* mark the project this test is part of as dirty (modified) */
    CurrentBalsaProject->dirty = TRUE;
    CurrentBalsaProject->needToRebuildMakefile = TRUE;
    UpdateProjectTrees ();

    DestroyOtherWindow (GTK_WIDGET (dialogue));
}

void OnTestOptionsDialogueCancel (GtkObject * dialogue)
{
    DestroyOtherWindow (GTK_WIDGET (dialogue));
}

/*
 * AddTestComponentToTestComponentList : insert a row into the testComponent
 * list at the row stated
 */
void AddTestComponentToTestComponentList (GtkCList * list, PtrBalsaTestComponent testComponent, int atRow, gboolean selectAfterInsert)
{
    GdkPixmap *pixmap;
    GdkBitmap *mask;

    char **clistText = g_new0 (char *, TEST_COMPONENT_COLUMN_COUNT);

    clistText[TEST_COMPONENT_TYPE_COLUMN] = "";
    clistText[TEST_COMPONENT_NAME_COLUMN] = ListToCommaString (testComponent->portNames);
    clistText[TEST_COMPONENT_VALUE_COLUMN] = (testComponent->value ? testComponent->value : "");

    ChoosePixmapForTestComponentSenseAndSource (testComponent->nature, &pixmap, &mask);
    /* gtk_clist_insert */ TestOpts_insertToList (GTK_CLIST (list), atRow,
      (char **) testComponent);
    gtk_clist_set_pixmap (GTK_CLIST (list), atRow, TEST_COMPONENT_TYPE_COLUMN, pixmap, mask);
    //if (testComponent->activated)
    //gtk_clist_set_pixmap (GTK_CLIST (list), atRow, TEST_COMPONENT_CHECKED_COLUMN, TickPixmap, TickMask);
    if (selectAfterInsert)
        gtk_clist_select_row (GTK_CLIST (list), atRow, 0);
}

/*
 * AddTestComponentsToTestComponentList : add a list of testComponents to the
 * test fixture testComponent list (at the end)
 */
void AddTestComponentsToTestComponentList (GtkCList * list, GList * testComponents)
{
    GList *tmp = g_list_last (testComponents);

    unactivate_change_events = TRUE;
    while (tmp)
    {
        AddTestComponentToTestComponentList (list, BALSA_TESTCOMPONENT (tmp->data), 0, FALSE);
        tmp = tmp->prev;
    }
    unactivate_change_events = FALSE;

    gtk_clist_select_row (GTK_CLIST (list), 0, 0);
}

void TestOpts_UpdateParameterValue (GtkWidget * dialogue, int rowNo)
{
    GtkCList *list = GTK_CLIST (LookupWidget (dialogue, "TestPortList"));
    BalsaTestComponentNature nature = TestSelectedTestComponentSense (dialogue);

    char *value;

    switch (nature)
    {
    case BalsaTestComponent_Sync:
    case BalsaTestComponent_Undefined:
        value = g_strdup ("");
        break;
    case BalsaTestComponent_InputFromFile:
    case BalsaTestComponent_InputFromValue:
        {
            const char *param1 = gtk_entry_get_text (GTK_ENTRY (LookupWidget (dialogue,
                  "TestPortValueEntry")));

            value = g_strdup (param1);
        }
        break;
    case BalsaTestComponent_OutputToStdout:
        {
            int param2 = gtk_spin_button_get_value_as_int (GTK_SPIN_BUTTON (LookupWidget (dialogue,
                  "TestRadixSpinButton")));
            int param3 = gtk_spin_button_get_value_as_int (GTK_SPIN_BUTTON (LookupWidget (dialogue,
                  "TestUSpacingSpinButton")));
            int param4 = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (LookupWidget (dialogue,
                  "TestShowLeadingZeroesCheckButton")));

            if (param2 >= 2)
                value = g_strdup_printf ("0,%d,%d,%d", param2, param3, param4);
            else
                value = g_strdup ("0");
        }
        break;
    case BalsaTestComponent_OutputToFile:
        {
            const char *param1 = gtk_entry_get_text (GTK_ENTRY (LookupWidget (dialogue,
                  "TestPortValueEntry")));
            int param2 = gtk_spin_button_get_value_as_int (GTK_SPIN_BUTTON (LookupWidget (dialogue,
                  "TestRadixSpinButton")));
            int param3 = gtk_spin_button_get_value_as_int (GTK_SPIN_BUTTON (LookupWidget (dialogue,
                  "TestUSpacingSpinButton")));
            int param4 = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (LookupWidget (dialogue,
                  "TestShowLeadingZeroesCheckButton")));

            if (param2 >= 2)
                value = g_strdup_printf ("%s,%d,%d,%d", param1, param2, param3, param4);
            else
                value = g_strdup_printf ("%s", param1);
        }
        break;
    case BalsaTestComponent_Memory:
        {
            const char *param1 = gtk_entry_get_text (GTK_ENTRY (LookupWidget (dialogue,
                  "MemoryFilenameEntry")));

            value = g_strdup (param1);
        }
        break;
    }

    gtk_clist_set_text (list, rowNo, TEST_COMPONENT_VALUE_COLUMN, value);
}

void OnTestPortsNewButton (GtkObject * dialogue)
{
    GtkWidget *list = GTK_WIDGET (LookupWidget (GTK_WIDGET (dialogue), "TestPortList"));
    int selectedRow = GPOINTER_TO_INT (gtk_object_get_data (dialogue, "SelectedTestComponent"));
    int targetRow = (selectedRow == TEST_COMPONENT_NO_SELECTION ? GTK_CLIST (list)->rows : selectedRow + 1);
    PtrBalsaTestComponent newTestComponent;

    if (selectedRow == TEST_COMPONENT_NO_SELECTION)
    {
        //char *nameEntryContents = gtk_entry_get_text (GTK_ENTRY (LookupWidget (dialogue, "TestPortEntry")));
        const char *valueEntryContents = gtk_entry_get_text (GTK_ENTRY (LookupWidget (GTK_WIDGET (dialogue), "TestPortValueEntry")));

        newTestComponent = NewBalsaTestComponent (TestSelectedTestComponentSense (GTK_WIDGET (dialogue)));
        newTestComponent->value = g_strdup (newTestComponent->nature == BalsaTestComponent_Sync ? "" : valueEntryContents);
    } else
        newTestComponent = NewBalsaTestComponent (BalsaTestComponent_Sync);
    AddTestComponentToTestComponentList (GTK_CLIST (list), newTestComponent, targetRow, TRUE);
    DeleteBalsaTestComponent (newTestComponent);
}

void OnTestPortsDelete (GtkObject * dialogue)
{
    GtkWidget *list = GTK_WIDGET (LookupWidget (GTK_WIDGET (dialogue), "TestPortList"));
    int selectedRow = GPOINTER_TO_INT (gtk_object_get_data (dialogue, "SelectedTestComponent"));

    /* gtk_clist_remove */ TestOpts_removeFromList (GTK_CLIST (list),
      selectedRow);
    gtk_clist_select_row (GTK_CLIST (list), selectedRow, 0);
}

void OnTestPortsRefill (GtkButton * button)
{
    GtkWidget *dialogue = gtk_widget_get_toplevel (GTK_WIDGET (button));
    GtkCList *testPortList = GTK_CLIST (LookupWidget (dialogue, "TestPortList"));
    GtkEntry *filenameEntry = GTK_ENTRY (LookupWidget (dialogue, "TestFilenameEntry"));
    GtkEntry *procNameEntry = GTK_ENTRY (LookupWidget (dialogue, "TestProcNameEntry"));

    //TestOpts_clearList (dialogue);

    const char *selectedFilename = gtk_entry_get_text (filenameEntry);
    const char *selectedProcName = gtk_entry_get_text (procNameEntry);

    CreateNewComponents (testPortList, selectedFilename, selectedProcName);

    TestOpts_UpdateForgottenPorts (testPortList);
}

void OnTestPortRadiobuttonToggle (GtkToggleButton * togglebutton)
{
    GtkWidget *dialogue = gtk_widget_get_toplevel (GTK_WIDGET (togglebutton));

    if (togglebutton->active)
    {                           /* only trigger on the button becoming active */
    	int selectedRow = GPOINTER_TO_INT (gtk_object_get_data (GTK_OBJECT (dialogue), "SelectedTestComponent"));

        if (selectedRow != TEST_COMPONENT_NO_SELECTION)
        {
            GdkPixmap *pixmap = NULL;
            GdkBitmap *bitmap = NULL;

            ChoosePixmapForTestComponentSenseAndSource (TestSelectedTestComponentSense (dialogue), &pixmap, &bitmap);
            gtk_clist_set_pixmap (GTK_CLIST
              (LookupWidget (dialogue, "TestPortList")), selectedRow, TEST_COMPONENT_TYPE_COLUMN, pixmap, bitmap);
        }
        UpdateTestDialogueWidgets (dialogue, GTK_CLIST (LookupWidget (dialogue, "TestPortList")),
            (selectedRow == TEST_COMPONENT_NO_SELECTION ? TEST_COMPONENT_NEW : selectedRow), TRUE);
    }
}

void OnTestPortListSelectRow (GtkCList * clist, gint row, gint column, GdkEvent * event)
{
    GtkWidget *dialogue = gtk_widget_get_toplevel (GTK_WIDGET (clist));

    if (unactivate_change_events)
        return;

    gtk_widget_set_sensitive (LookupWidget (dialogue, "TestPortsDeleteButton"), TRUE);
    gtk_object_set_data (GTK_OBJECT (dialogue), "SelectedTestComponent", GINT_TO_POINTER (row));
    UpdateTestDialogueWidgets (dialogue, clist, row, FALSE);
    gtk_widget_grab_focus (LookupWidget (dialogue, "TestPortValueEntry"));
}

void OnTestPortListUnselectRow (GtkCList * clist, gint row, gint column, GdkEvent * event)
{
    GtkWidget *dialogue = gtk_widget_get_toplevel (GTK_WIDGET (clist));

    if (unactivate_change_events)
        return;

    gtk_widget_set_sensitive (LookupWidget (dialogue, "TestPortsDeleteButton"), FALSE);
    gtk_object_set_data (GTK_OBJECT (dialogue), "SelectedTestComponent", (gpointer) TEST_COMPONENT_NO_SELECTION);
    UpdateTestDialogueWidgets (dialogue, clist, TEST_COMPONENT_NO_SELECTION, FALSE);
}

gboolean SetTestComponentFilename (char *filename, GtkWidget * dialogue)
{
    GtkWidget *entry;

    if (TestSelectedTestComponentSense (dialogue) == BalsaTestComponent_Memory)
        entry = LookupWidget (dialogue, "MemoryFilenameEntry");
    else
        entry = LookupWidget (dialogue, "TestPortValueEntry");

    char *relativeFilename = ImportPaths_ConvertFileToRelative (filename);

    //ConvertToRelativePath (filename, CurrentBalsaProject->directory);

    gtk_entry_set_text (GTK_ENTRY (entry), relativeFilename);
    return TRUE;
}

void OnTestPortFileBrowseButton (GtkObject * dialogue)
{
    MakeFileSelectionDialogue (FileSelectionDialogue_Open, "Select a TestComponent Data File",
      (FileSelectionCallback) SetTestComponentFilename, (gpointer) GTK_WIDGET (dialogue), GTK_WIDGET (dialogue));
}

void OnTestPortEntryChange (GtkEditable * editable)
{
    GtkWidget *dialogue = gtk_widget_get_toplevel (GTK_WIDGET (editable));
    GtkCList *list = GTK_CLIST (LookupWidget (dialogue, "TestPortList"));
    int rowNo = GPOINTER_TO_INT (gtk_object_get_data (GTK_OBJECT (dialogue), "SelectedTestComponent"));

    if (unactivate_change_events)
        return;

    gtk_widget_set_sensitive (LookupWidget (dialogue, "ComponentTypeFrame"), TRUE);

    if (rowNo == TEST_COMPONENT_NO_SELECTION)
    {
        const char *nameEntryContents = gtk_entry_get_text (GTK_ENTRY (editable));

        if (!nameEntryContents || !*nameEntryContents)
            return;
        OnTestPortsNewButton (GTK_OBJECT (dialogue));
        gtk_widget_grab_focus (LookupWidget (dialogue, "TestPortEntry"));
        rowNo = list->rows - 1;
    }
    gtk_clist_set_text (list, rowNo, TEST_COMPONENT_NAME_COLUMN, gtk_entry_get_text (GTK_ENTRY (editable)));
}

void OnTestPortValueEntryChange (GtkEditable * editable)
{
    GtkWidget *dialogue = gtk_widget_get_toplevel (GTK_WIDGET (editable));
    GtkCList *list = GTK_CLIST (LookupWidget (dialogue, "TestPortList"));
    int rowNo = GPOINTER_TO_INT (gtk_object_get_data (GTK_OBJECT (dialogue), "SelectedTestComponent"));

    if (unactivate_change_events)
        return;

    if (rowNo == TEST_COMPONENT_NO_SELECTION)
    {
        OnTestPortsNewButton (GTK_OBJECT (dialogue));
        rowNo = list->rows - 1;
    }
    //char *value = gtk_entry_get_text (GTK_ENTRY (editable));
    TestOpts_UpdateParameterValue (dialogue, rowNo);
}

void OnMemoryPortEntryChange (GtkEditable * editable)
{
    GtkWidget *dialogue = gtk_widget_get_toplevel (GTK_WIDGET (editable));
    GtkCList *list = GTK_CLIST (LookupWidget (dialogue, "TestPortList"));
    int rowNo = GPOINTER_TO_INT (gtk_object_get_data (GTK_OBJECT (dialogue), "SelectedTestComponent"));

    if (unactivate_change_events)
        return;

    gtk_widget_set_sensitive (LookupWidget (dialogue, "ComponentTypeFrame"), TRUE);

    if (rowNo == TEST_COMPONENT_NO_SELECTION)
    {
        const char *nameEntryContents = gtk_entry_get_text (GTK_ENTRY (editable));

        if (!nameEntryContents || !*nameEntryContents)
            return;
        OnTestPortsNewButton (GTK_OBJECT (dialogue));
        gtk_widget_grab_focus (LookupWidget (dialogue, "TestPortEntry"));
        rowNo = list->rows - 1;
    }
    GtkEntry *AddressPortEntry = GTK_ENTRY (LookupWidget (dialogue, "AddressPortEntry"));
    GtkEntry *WritePortEntry = GTK_ENTRY (LookupWidget (dialogue, "WritePortEntry"));
    GtkEntry *ReadPortEntry = GTK_ENTRY (LookupWidget (dialogue, "ReadPortEntry"));
    char *text = g_strdup_printf ("%s,%s,%s", gtk_entry_get_text (AddressPortEntry),
      gtk_entry_get_text (WritePortEntry),
      gtk_entry_get_text (ReadPortEntry));

    gtk_clist_set_text (list, rowNo, TEST_COMPONENT_NAME_COLUMN, text);
}

gboolean OnTestOptionsDialogue_keyPressEvent (GtkObject * widget, GdkEventKey * event)
{
    if (event->keyval == GDK_Return)
        OnTestOptionsDialogueOK (widget);
    else if (event->keyval == GDK_Escape)
        OnTestOptionsDialogueCancel (widget);
    return FALSE;
}

void OnTestFformatEntryChange (GtkEditable * editable)
{
    GtkWidget *dialogue = gtk_widget_get_toplevel (GTK_WIDGET (editable));
    GtkCList *list = GTK_CLIST (LookupWidget (dialogue, "TestPortList"));
    int rowNo = GPOINTER_TO_INT (gtk_object_get_data (GTK_OBJECT (dialogue), "SelectedTestComponent"));

    if (unactivate_change_events)
        return;

    if (rowNo == TEST_COMPONENT_NO_SELECTION)
    {
        OnTestPortsNewButton (GTK_OBJECT (dialogue));
        rowNo = list->rows - 1;
    }
    //char *value = gtk_entry_get_text (GTK_ENTRY (editable));
    TestOpts_UpdateParameterValue (dialogue, rowNo);
}

void CreateNewComponents (GtkCList * list, const char *filename, const char *procName)
{
    if (procName)
    {
        GtkWidget *dialogue = gtk_widget_get_toplevel (GTK_WIDGET (list));
        GPtrArray *knownTestPorts = FindTestPortsForProcedure (filename, procName);

        gtk_object_set_data (GTK_OBJECT (dialogue), "knownTestPorts", knownTestPorts);
    }
    TestOpts_UpdateForgottenPorts (list);
}

void TestOpts_clearList (GtkWidget * dialogue)
{
    GtkCList *list = GTK_CLIST (LookupWidget (dialogue, "TestPortList"));
    GList *displayedPortList = gtk_object_get_data (GTK_OBJECT (dialogue), "displayedPortList");

    gtk_clist_clear (list);
    displayedPortList = NULL;
//TODO:free list and elements
    gtk_object_set_data (GTK_OBJECT (dialogue), "displayedPortList", displayedPortList);
}

void TestOpts_insertToList (GtkCList * list, int atRow, char **text)
{
    GtkWidget *dialogue = gtk_widget_get_toplevel (GTK_WIDGET (list));
    GList *displayedPortList = gtk_object_get_data (GTK_OBJECT (dialogue), "displayedPortList");

    //gtk_clist_insert (list, atRow, text);
    displayedPortList = g_list_insert (displayedPortList, text, atRow);
    gtk_object_set_data (GTK_OBJECT (dialogue), "displayedPortList", displayedPortList);
    TestOpts_UpdateForgottenPorts (list);
}

void TestOpts_removeFromList (GtkCList * list, int row)
{
    GtkWidget *dialogue = gtk_widget_get_toplevel (GTK_WIDGET (list));
    GList *displayedPortList = gtk_object_get_data (GTK_OBJECT (dialogue), "displayedPortList");

    gtk_clist_remove (list, row);
    displayedPortList = g_list_remove_link (displayedPortList, g_list_nth (displayedPortList, row));
    gtk_object_set_data (GTK_OBJECT (dialogue), "displayedPortList", displayedPortList);
    TestOpts_UpdateForgottenPorts (list);
}

void TestOpts_UpdateForgottenPorts (GtkCList * list)
{
    GtkWidget *dialogue = gtk_widget_get_toplevel (GTK_WIDGET (list));

    GList *displayedPortList = gtk_object_get_data (GTK_OBJECT (dialogue), "displayedPortList");
    GPtrArray *knownTestPorts = gtk_object_get_data (GTK_OBJECT (dialogue), "knownTestPorts");
    GList *ptr;
    int atRow = 0;
    GList *knownTestPortsCopyRefs = NULL;
    unsigned int i;

    //gtk_clist_clear (list);

    if (knownTestPorts)
        for (i = 0; i < knownTestPorts->len; i++)
        {
            PtrBalsaPort2 port = g_ptr_array_index (knownTestPorts, i);

            knownTestPortsCopyRefs = g_list_prepend (knownTestPortsCopyRefs, port);
        }

    for (ptr = displayedPortList; ptr; ptr = ptr->next)
    {
        PtrBalsaTestComponent testComponent = ptr->data;
        GList *ptr3 = testComponent->portNames;

        for (; ptr3; ptr3 = ptr3->next)
        {
            char *portname = ptr3->data;

            GList *ptr2 = knownTestPortsCopyRefs;

            for (; ptr2; ptr2 = ptr2->next)
            {
                PtrBalsaPort2 port2 = ptr2->data;

                if (!strcmp (portname, port2->name))
                {
                    knownTestPortsCopyRefs = g_list_remove_link (knownTestPortsCopyRefs, ptr2);
                    break;
                }
            }
        }
        {
            GdkPixmap *pixmap;
            GdkBitmap *mask;

            char **clistText = g_new0 (char *, TEST_COMPONENT_COLUMN_COUNT);

            clistText[TEST_COMPONENT_TYPE_COLUMN] = "";
            clistText[TEST_COMPONENT_NAME_COLUMN] = ListToCommaString (testComponent->portNames);
            clistText[TEST_COMPONENT_VALUE_COLUMN] = (testComponent->value ? testComponent->value : "");

            ChoosePixmapForTestComponentSenseAndSource (testComponent->nature, &pixmap, &mask);
            gtk_clist_insert (GTK_CLIST (list), atRow, clistText);
            gtk_clist_set_pixmap (GTK_CLIST (list), atRow, TEST_COMPONENT_TYPE_COLUMN, pixmap, mask);
        }

        atRow++;
    }

    if (knownTestPortsCopyRefs)
    {
        GList *ptr;

        for (ptr = knownTestPortsCopyRefs; ptr; ptr = ptr->next)
        {
            PtrBalsaPort2 port = ptr->data;
            char **clistText = g_new0 (char *, TEST_COMPONENT_COLUMN_COUNT);
            GdkPixmap *pixmap;
            GdkBitmap *mask;

            clistText[TEST_COMPONENT_TYPE_COLUMN] = "";
            clistText[TEST_COMPONENT_NAME_COLUMN] = port->name;
            clistText[TEST_COMPONENT_VALUE_COLUMN] = "";

            switch (port->nature)
            {
            case BalsaPort2_Sync:
                ChoosePixmapForTestComponentSenseAndSource (BalsaTestComponent_Sync, &pixmap, &mask);
                break;
            case BalsaPort2_Input:
                ChoosePixmapForTestComponentSenseAndSource (BalsaTestComponent_InputFromValue, &pixmap, &mask);
                break;
            case BalsaPort2_Output:
                ChoosePixmapForTestComponentSenseAndSource (BalsaTestComponent_OutputToStdout, &pixmap, &mask);
                break;
            }
            //ChoosePixmapForTestComponentSenseAndSource (BalsaTestComponent_Undefined, &pixmap, &mask);

            gtk_clist_insert (GTK_CLIST (list), atRow, clistText);
            gtk_clist_set_pixmap (GTK_CLIST (list), atRow, TEST_COMPONENT_TYPE_COLUMN, pixmap, mask);

            atRow++;

        }
    }
    for (i = 0; i < 100; i++)
        gtk_clist_remove (list, atRow);

}

/*************************************/

void OnTestOptionsDefinitionsUpButton (GtkObject * dialogue)
{
    GtkWidget *list = GTK_WIDGET (LookupWidget (GTK_WIDGET (dialogue), "DefinesList"));
    int selectedRowNo = GPOINTER_TO_INT (gtk_object_get_data (dialogue, "SelectedDefine"));

    if (selectedRowNo != TEST_OPTS_NO_SELECTION)
    {
        gtk_object_set_data (GTK_OBJECT (dialogue), "DefinesChanged", (gpointer) TRUE);
        gtk_clist_unselect_row (GTK_CLIST (list), selectedRowNo, 0);
        gtk_clist_row_move (GTK_CLIST (list), selectedRowNo, selectedRowNo - 1);
        gtk_clist_select_row (GTK_CLIST (list), selectedRowNo - 1, 0);
    }
}

void OnTestOptionsDefinitionsDownButton (GtkObject * dialogue)
{
    GtkWidget *list = GTK_WIDGET (LookupWidget (GTK_WIDGET (dialogue), "DefinesList"));
    int selectedRowNo = GPOINTER_TO_INT (gtk_object_get_data (dialogue, "SelectedDefine"));

    gtk_object_set_data (GTK_OBJECT (dialogue), "DefinesChanged", (gpointer) TRUE);
    gtk_clist_unselect_row (GTK_CLIST (list), selectedRowNo, 0);
    gtk_clist_row_move (GTK_CLIST (list), selectedRowNo, selectedRowNo + 1);
    gtk_clist_select_row (GTK_CLIST (list), selectedRowNo + 1, 0);
}

void OnTestOptionsDefinitionsNewButton (GtkObject * dialogue)
{
    GtkWidget *list = GTK_WIDGET (LookupWidget (GTK_WIDGET (dialogue), "DefinesList"));
    GtkWidget *nameEntry = GTK_WIDGET (LookupWidget (GTK_WIDGET (dialogue), "DefineNameEntry"));
    GtkWidget *valueEntry = GTK_WIDGET (LookupWidget (GTK_WIDGET (dialogue), "DefineValueEntry"));
    int selectedRowNo = GPOINTER_TO_INT (gtk_object_get_data (dialogue, "SelectedDefine"));

    if (selectedRowNo == TEST_OPTS_NO_SELECTION)
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

void OnTestOptionsDefinitionsDeleteButton (GtkObject * dialogue)
{
    GtkWidget *list = GTK_WIDGET (LookupWidget (GTK_WIDGET (dialogue), "DefinesList"));
    int selectedRowNo = GPOINTER_TO_INT (gtk_object_get_data (dialogue, "SelectedDefine"));

    gtk_clist_remove (GTK_CLIST (list), selectedRowNo);
    gtk_clist_select_row (GTK_CLIST (list), selectedRowNo, 0);
}

void OnTestOptionsDefinitionsSelectRow (GtkCList * list, gint row, gint column, GdkEvent * event)
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

void OnTestOptionsDefinitionsUnselectRow (GtkCList * list, gint row, gint column, GdkEvent * event)
{
    GtkWidget *dialogue = gtk_widget_get_toplevel (GTK_WIDGET (list));

//    int selectedRowNo = (int) gtk_object_get_data (dialogue, "SelectedDefine");
    GtkWidget *nameEntry = LookupWidget (dialogue, "DefineNameEntry");
    GtkWidget *valueEntry = LookupWidget (dialogue, "DefineValueEntry");

    gtk_widget_set_sensitive (LookupWidget (dialogue, "DefinesUpButton"), FALSE);
    gtk_widget_set_sensitive (LookupWidget (dialogue, "DefinesDownButton"), FALSE);
    gtk_widget_set_sensitive (LookupWidget (dialogue, "DefinesDeleteButton"), FALSE);

    gtk_object_set_data (GTK_OBJECT (dialogue), "SelectedDefine", (gpointer) TEST_OPTS_NO_SELECTION);

    TestDefinitionsEntryDisabled = 1;
    gtk_entry_set_text (GTK_ENTRY (nameEntry), "");
    gtk_entry_set_text (GTK_ENTRY (valueEntry), "");
    TestDefinitionsEntryDisabled = 0;
}

void OnTestOptionsDialogueDefinitionsNameEntryChanged (GtkEditable * editable)
{
    GtkWidget *dialogue = gtk_widget_get_toplevel (GTK_WIDGET (editable));
    GtkCList *list = GTK_CLIST (LookupWidget (dialogue, "DefinesList"));
    int selectedRowNo = GPOINTER_TO_INT (gtk_object_get_data (GTK_OBJECT (dialogue), "SelectedDefine"));

    if (TestDefinitionsEntryDisabled)
        return;

    if (selectedRowNo == TEST_OPTS_NO_SELECTION)
    {
        OnTestOptionsDefinitionsNewButton (GTK_OBJECT (dialogue));
    	selectedRowNo = GPOINTER_TO_INT (gtk_object_get_data (GTK_OBJECT (dialogue), "SelectedDefine"));
    }
    gtk_clist_set_text (list, selectedRowNo, 0, gtk_entry_get_text (GTK_ENTRY (editable)));
    gtk_object_set_data (GTK_OBJECT (dialogue), "PathChanged", (gpointer) TRUE);
}

void OnTestOptionsDialogueDefinitionsValueEntryChanged (GtkEditable * editable)
{
    GtkWidget *dialogue = gtk_widget_get_toplevel (GTK_WIDGET (editable));
    GtkCList *list = GTK_CLIST (LookupWidget (dialogue, "DefinesList"));
    int selectedRowNo = GPOINTER_TO_INT (gtk_object_get_data (GTK_OBJECT (dialogue), "SelectedDefine"));

    if (TestDefinitionsEntryDisabled)
        return;

    if (selectedRowNo == TEST_OPTS_NO_SELECTION)
    {
        OnTestOptionsDefinitionsNewButton (GTK_OBJECT (dialogue));
    	selectedRowNo = GPOINTER_TO_INT (gtk_object_get_data (GTK_OBJECT (dialogue), "SelectedDefine"));
    }
    gtk_clist_set_text (list, selectedRowNo, 1, gtk_entry_get_text (GTK_ENTRY (editable)));
    gtk_object_set_data (GTK_OBJECT (dialogue), "DefinesChanged", (gpointer) TRUE);
}
