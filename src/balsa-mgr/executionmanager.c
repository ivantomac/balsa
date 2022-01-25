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

	`executionmanager.c'

*/

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <glib.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <dirent.h>
#include <fcntl.h>

#include "executionmanager.h"
#include "main.h"
#include "makefiletable.h"
#include "utils.h"
#include "project.h"
#include "filemanager.h"
#include "commands.h"
#include "support.h"

#include <time.h>
#include <string.h>

struct InfoExecutionProcess
{
    GtkCTreeNode *treeNode;
    char *nameFileStdOut;
    //  char *nameFileStdErr;
    int numProcess;
};

struct InfoCommand
{
    char *name;
    GtkCTreeNode *treeNode;
    struct InfoExecutionProcess *infoExecutionProcess;
};

static gboolean executionStopped = FALSE;
static int currentlyExecutedPid = 0;
void signal_SIGUSR1_handler (int info)
{
    //  printf("handler SIGUSR\n");
    if (currentlyExecutedPid)
    {
        //      printf("handler SIGUSR: killing -%d\n", currentlyExecutedPid);
        executionStopped = TRUE;
        kill (-currentlyExecutedPid, SIGTERM);
    }
}

//#define ExecutionWindow (workSpace.executionWindow->window)
GtkWidget *ExecutionWindow = NULL;

int ExecutionWindowViewOptions = 2; //0=Always 1=Never 2=Automatic

GtkWidget *popupMenu1 = NULL;
GtkWidget *popupMenu1Caller = NULL;

GList *ExecutingProcesses = NULL;
gint timer_id = -1;
gint timer_update_selected_row_id = -1;

void ExecutionManager_RunMakeCommand (char *makeCommand, GList * nextCommands);
void ExecutionManager_ExecuteResultOfMakeNCommand (GList * infoCommandsList);
void ExecutionManager_RunInfoCommandListForNode (GList * infoCommandList, GtkCTreeNode * treeNode);
void OnExecutionCTree_SelectRow (GtkCTree * tree, GtkCTreeNode * node, gint column);

void ExecutionManager_StopProcessButton_SetAttachedNode (GtkCTreeNode * treeNode)
{
    GtkWidget *TreeWidget = LookupWidget (ExecutionWindow,
      "ExecutionCTree");
    GtkWidget *StopProcessButton = LookupWidget (ExecutionWindow,
      "StopProcessButton");
    struct InfoExecutionProcess *info = gtk_ctree_node_get_row_data (GTK_CTREE (TreeWidget),
      treeNode);
    int numProcess = info ? info->numProcess : 0;

    gtk_object_set_data (GTK_OBJECT (StopProcessButton), "associatedTreeNode", (gpointer) treeNode);
    gtk_widget_set_sensitive (StopProcessButton, treeNode && numProcess);
}

void CheckProcessOutputs (GtkCTreeNode * treeNode, gboolean processFinished)
{
    GtkWidget *TreeWidget = LookupWidget (ExecutionWindow,
      "ExecutionCTree");
    struct InfoExecutionProcess *infoRow;
    int fileStdOutExists, fileStdErrExists;
    struct stat infosFileStdOut, infosFileStdErr;
    int num_message = 0;
    char message[100];
    char *nameFileStdOut, *nameFileStdErr;

    infoRow = gtk_ctree_node_get_row_data (GTK_CTREE (TreeWidget), treeNode);

    nameFileStdOut = g_strdup_printf ("%s_Out", infoRow->nameFileStdOut);
    nameFileStdErr = g_strdup_printf ("%s_Err", infoRow->nameFileStdOut);

    fileStdOutExists = !stat (nameFileStdOut, &infosFileStdOut);
    fileStdErrExists = !stat (nameFileStdErr, &infosFileStdErr);

    if (fileStdOutExists && infosFileStdOut.st_size)
        num_message += 1;
    if (fileStdErrExists && infosFileStdErr.st_size)
        num_message += 2;

    switch (num_message)
    {
    case 0:
        strcpy (message, "");
        break;
    case 1:
        strcpy (message, "Out");
        break;
    case 2:
        strcpy (message, "Err");
        break;
    case 3:
        strcpy (message, "Err+Out");
        break;
    }

    {
        gboolean moveScrollbarToEnd = FALSE;
        char *buf;

        gtk_ctree_node_get_text (GTK_CTREE (TreeWidget), treeNode, 1, &buf);

        if (message[0] && strcmp (buf, message))
        {
            gtk_ctree_node_set_text (GTK_CTREE (TreeWidget), treeNode, 1, message);
            gtk_ctree_select ((GtkCTree *) TreeWidget, treeNode);
            moveScrollbarToEnd = TRUE;
        } else if (!strcmp (buf, "-"))
        {
            gtk_ctree_node_set_text (GTK_CTREE (TreeWidget), treeNode, 1, message);
            moveScrollbarToEnd = TRUE;
        }

        if (moveScrollbarToEnd) // Move the scrollbar to the end
        {
            GtkScrolledWindow *scrolledWindow = GTK_SCROLLED_WINDOW (LookupWidget (ExecutionWindow,
                "ExecutionCTreeScrollbar"));
            GtkVScrollbar *vScrollbar = GTK_VSCROLLBAR (scrolledWindow->vscrollbar);
            GtkScrollbar *scrollbar = &(vScrollbar->scrollbar);
            GtkRange *range = GTK_RANGE (&(scrollbar->range));

            GtkAdjustment *adjustment = gtk_range_get_adjustment (range);

            // printf("%f %f %f\n",adjustment->value,adjustment->lower,adjustment->upper);
            if (adjustment->upper > adjustment->page_size)
                gtk_adjustment_set_value (adjustment, adjustment->upper - adjustment->page_size);
            // printf("->%f %f %f  %f\n",adjustment->value,adjustment->lower,adjustment->upper,adjustment->page_size);
        }
    }

    if (processFinished)
    {                           //remove the empty tmp files
        if (fileStdOutExists && (infosFileStdOut.st_size == 0))
            unlink (nameFileStdOut);
        if (fileStdErrExists && (infosFileStdErr.st_size == 0))
            unlink (nameFileStdErr);
    }
}

#define INIT_COUNTER_IDLE_TIME_BEFORE_KILL_TIMER 2
gint ExecutionManager_function_timer (gpointer data)
{
    static int count_before_end = INIT_COUNTER_IDLE_TIME_BEFORE_KILL_TIMER;

    //  printf("function timer%d(%d)\n", timer_id, data);

    if (ExecutingProcesses)
    {
        GList *tmp = ExecutingProcesses;

        for (; tmp; tmp = tmp->next)
        {
            GtkCTreeNode *treeNode = GTK_CTREE_NODE (tmp->data);

            CheckProcessOutputs (treeNode, FALSE);
        }
        count_before_end = INIT_COUNTER_IDLE_TIME_BEFORE_KILL_TIMER;
    } else
    {
        count_before_end--;
        if (count_before_end == 0)
        {
            gtk_timeout_remove (timer_id);
            timer_id = -1;
            count_before_end = INIT_COUNTER_IDLE_TIME_BEFORE_KILL_TIMER;
        }
    }

    return TRUE;
}

void ExecutionManager_AddExecutingProcess (GtkCTreeNode * treeNode)
{
    ExecutingProcesses = g_list_append (ExecutingProcesses, treeNode);

    if (timer_id == -1)
        timer_id = gtk_timeout_add (1000, ExecutionManager_function_timer, NULL);

    {
        GtkWidget *TreeWidget = LookupWidget (ExecutionWindow,
          "ExecutionCTree");

        if (gtk_object_get_data (GTK_OBJECT (TreeWidget), "selectedNode") == treeNode)
            OnExecutionCTree_SelectRow ((GtkCTree *) TreeWidget, treeNode, 0);
    }
}

void ExecutionManager_RemoveExecutingProcess (GtkCTreeNode * treeNode)
{
    ExecutingProcesses = g_list_remove (ExecutingProcesses, treeNode);
    CheckProcessOutputs (treeNode, TRUE);

    {
        GtkWidget *TreeWidget = LookupWidget (ExecutionWindow,
          "ExecutionCTree");

        if (gtk_object_get_data (GTK_OBJECT (TreeWidget), "selectedNode") == treeNode)
            OnExecutionCTree_SelectRow ((GtkCTree *) TreeWidget, treeNode, 0);
    }
}

void ExecutionManager_AddFinishedPidToWaitPidList (int pid)
{
    static GList *waitpidList = NULL;
    GList *tmp, *tmp2;
    struct _local
    {
        int pid;
        //  int tag;
        //  int filedesc;
    };

    if (pid)
    {
        struct _local *st = g_new (struct _local, 1);

        st->pid = pid;
        waitpidList = g_list_prepend (waitpidList, st);
    }

    tmp = waitpidList;
    while (tmp)
    {
        struct _local *st = tmp->data;
        int statuspid;

        waitpid (st->pid, &statuspid, WNOHANG);
        tmp2 = tmp->next;
        if (WIFEXITED (statuspid) || WIFSIGNALED (statuspid))
        {
            waitpidList = g_list_remove_link (waitpidList, tmp);
            g_free (st);
        }
        tmp = tmp2;
    }
}

void communication_input_callback (gpointer tagp, gint source, GdkInputCondition condition)
{
    int filedesc = source;
    char buf[1000], *ptr = buf;
    int size = read (filedesc, buf, 1000);

    buf[size] = 0;

    while (*ptr)
    {
        GtkWidget *TreeWidget = LookupWidget (ExecutionWindow,
          "ExecutionCTree");
        int command;
        struct InfoCommand *infoCommand;

        if (sscanf (ptr, "%d %p", &command, &infoCommand) < 2)
            break;
        //      printf("communication_input_callback : %d %p - %s\n", command, infoCommand, buf);

        switch (command)
        {
            struct InfoExecutionProcess *infoRow;
            char filename[1000], *ptr_space, *ptr_endline;
            int numError, numProcess;
            GList *infoCommandsList;

        case 0:
            sscanf (ptr, "%d %p %d %s", &command, &infoCommand, (int *) &numProcess, filename);
            gtk_ctree_node_set_text (GTK_CTREE (TreeWidget), infoCommand->treeNode, 2, "Running...");

            infoRow = infoCommand->infoExecutionProcess;
            infoRow->nameFileStdOut = g_strdup (filename);
            //   infoRow->numProcess = numProcess;
            ExecutionManager_AddExecutingProcess (infoCommand->treeNode);
            break;
        case 1:
            gtk_ctree_node_set_text (GTK_CTREE (TreeWidget), infoCommand->treeNode, 2, "Finished");
            ExecutionManager_RemoveExecutingProcess (infoCommand->treeNode);
            ExecutionManager_AddFinishedPidToWaitPidList (infoCommand->infoExecutionProcess->numProcess);
            UpdateProjectTreeFilesView ();
            break;
        case 2:
            sscanf (ptr, "%d %p %d", &command, &infoCommand, &numError);
            if (numError < 100)
                gtk_ctree_node_set_text (GTK_CTREE (TreeWidget), infoCommand->treeNode, 2, g_strdup_printf ("Finished: Error %d", numError));
            else if (numError == 133)
                gtk_ctree_node_set_text (GTK_CTREE (TreeWidget),
                  infoCommand->treeNode, 2, g_strdup_printf ("Finished: Undefined Error %d", numError));
            else
            {
                char *signames[32] = {
                    "",
                    "Hangup",   // 1
                    "Interrupt", // 2
                    "Quit",     // 3
                    "Illegal instruction", // 4
                    "Trace trap", // 5
                    "Abort",    // 6
                    "BUS error", // 7
                    "Floating-point exception", // 8
                    "Kill",     // 9
                    "User-defined signal 1", // 10
                    "Segmentation fault", // 11
                    "User-defined signal 2", // 12
                    "Broken pipe", // 13
                    "Alarm clock", // 14
                    "Termination", // 15
                    "Stack fault", // 16
                    "Child status has changed", // 17
                    "Continue", // 18
                    "Stop",     // 19
                    "Keyboard stop", // 20
                    "Background read from tty", // 21
                    "Background write to tty", // 22
                    "Urgent condition on socket", // 23
                    "CPU limit exceeded", // 24
                    "File size limit exceeded", // 25
                    "Virtual alarm clock", // 26
                    "Profiling alarm clock", // 27
                    "Window size change", // 28
                    "I/O now possible", // 29
                    "Power failure restart", // 30
                    "Bad system call" // 31
                };

                if (numError - 100 < 32)
                    gtk_ctree_node_set_text (GTK_CTREE (TreeWidget),
                      infoCommand->treeNode, 2, g_strdup_printf ("Finished: Signal %d (%s)", numError - 100, signames[numError - 100]));
                else
                    gtk_ctree_node_set_text (GTK_CTREE (TreeWidget),
                      infoCommand->treeNode, 2, g_strdup_printf ("Finished: Signal %d", numError - 100));
            }
            ExecutionManager_RemoveExecutingProcess (infoCommand->treeNode);
            ExecutionManager_AddFinishedPidToWaitPidList (infoCommand->infoExecutionProcess->numProcess);
            break;
        case 3:
            gtk_ctree_node_set_text (GTK_CTREE (TreeWidget), infoCommand->treeNode, 2, "Not Executed");
            break;
        case 4:                //after a make -n command
            infoCommandsList = (GList *) infoCommand;
            infoCommand = infoCommandsList->data;
            gtk_ctree_node_set_text (GTK_CTREE (TreeWidget), infoCommand->treeNode, 1, "");
            gtk_ctree_node_set_text (GTK_CTREE (TreeWidget), infoCommand->treeNode, 2, "");
            ExecutionManager_RemoveExecutingProcess (infoCommand->treeNode);
            ExecutionManager_AddFinishedPidToWaitPidList (infoCommand->infoExecutionProcess->numProcess);

            ExecutionManager_ExecuteResultOfMakeNCommand (infoCommandsList);
            break;

        case 5:
            {
                int tag, comPipe0, comPipe1;

                sscanf (ptr, "%d %d %d %d", &command, &tag, &comPipe0, &comPipe1);
                if (tag > 0 && comPipe0 > 0 && comPipe1 > 0)
                {
                    gdk_input_remove (tag);
                    close (comPipe0);
                    close (comPipe1);
                } else
                    fprintf (stderr, "Please report \"error gdk_input_tag %d %d %d\". Thank you.\n", tag, comPipe0, comPipe1);
            }
            break;

        case 10:               //execute function (or other ~... command)
            //   CurrentBalsaProject->needToRebuildMakefile = FALSE;
            infoCommandsList = (GList *) infoCommand;
            ptr_space = strstr (ptr, " ");
            if (!ptr_space)
                break;
            ptr_space = strstr (ptr_space + 1, " ");
            if (!ptr_space)
                break;
            ptr_endline = strstr (ptr_space + 1, "\n");
            if (ptr_endline)
            {
                strncpy (filename, ptr_space + 1, ptr_endline - ptr_space - 1);
                filename[ptr_endline - ptr_space - 1] = 0;
            }

            if (!strncmp (filename, "~ExecuteFunction2args ", 22))
            {
                void (*proc) (GtkEditable * editable, gpointer arg2);
                gpointer arg1, arg2;

                sscanf (filename + 22, "%p %p %p", &proc, &arg1, &arg2);
                proc (GTK_EDITABLE (arg1), (gpointer) arg2);

                //todo : execute nextCommands (but for the moment, nothing is using this)
            }

            if (!strncmp (filename, "~ExecuteFunction0arg ", 21))
            {
                void (*proc) (void);

                sscanf (filename + 21, "%p", &proc);
                proc ();

                //todo : execute nextCommands (but for the moment, nothing is using this)
            }
            break;

        default:
            printf ("error : received command %d\n", command);
        }

        while ((*ptr != '\n') && (*ptr != 0))
            ptr++;
        while (*ptr == '\n')
            ptr++;
    }
}

void ExecutionManager_PopupMenu_SendBufferToInternalEditor (gpointer data)
{
    char *filename = gtk_object_get_data (GTK_OBJECT (popupMenu1Caller),
      "filename");

    if (filename)
        FileManager_OpenFile (filename);
}

void ExecutionManager_PopupMenu_SendBufferToExternalEditor (gpointer data)
{
    char *filename = gtk_object_get_data (GTK_OBJECT (popupMenu1Caller),
      "filename");

    if (filename)
    {
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
        args[1] = filename;

        RunCommandWithoutOutput (args[0], args);
    }
}

gboolean ExecutionManager_PopupMenu_SaveAsBuffer_Callback (const char *filename, gpointer fromFilename)
{
    /* cp `fromFilename` `filename` */
    FILE *src = fopen (fromFilename, "rb");
    FILE *dest = fopen (filename, "wb");
    char buf[100];
    int size;

    //    printf("%s => %s\n",fromFilename,filename);

    if (!src || !dest)
    {
        if (src)
        {
            printfConsole_s ("Error in opening %s for writing.\n", filename);
            fclose (src);
        }
        if (dest)
        {
            printfConsole_s ("Error in opening %s for reading.\n", fromFilename);
            fclose (dest);
        }

        return TRUE;
    }

    while ((size = fread (buf, 1, 100, src)) != 0)
        fwrite (buf, 1, size, dest);

    fclose (src);
    fclose (dest);
    return TRUE;
}

void ExecutionManager_PopupMenu_SaveAsBuffer (gpointer data)
{
    char *filename = gtk_object_get_data (GTK_OBJECT (popupMenu1Caller),
      "filename");

    if (filename)
        MakeFileSelectionDialogue (FileSelectionDialogue_Save,
        	"Save Execution Output As...", ExecutionManager_PopupMenu_SaveAsBuffer_Callback, g_strdup (filename), NULL);
}

void ExecutionManager_CreatePopupMenu1 (void)
{
    GtkWidget *open_item, *openextern_item, *saveas_item;

    popupMenu1 = gtk_menu_new ();

    /* Create the menu items */
    open_item = gtk_menu_item_new_with_label ("Open in internal editor");
    openextern_item = gtk_menu_item_new_with_label ("Open in external editor");
    saveas_item = gtk_menu_item_new_with_label ("Save as...");

    /* Add them to the menu */
    gtk_menu_append (GTK_MENU (popupMenu1), open_item);
    gtk_menu_append (GTK_MENU (popupMenu1), openextern_item);
    gtk_menu_append (GTK_MENU (popupMenu1), saveas_item);

    /* Attach the callback functions to the activate signal */
    gtk_signal_connect_object (GTK_OBJECT (open_item), "activate", GTK_SIGNAL_FUNC (ExecutionManager_PopupMenu_SendBufferToInternalEditor), NULL);
    gtk_signal_connect_object (GTK_OBJECT (openextern_item), "activate",
      GTK_SIGNAL_FUNC (ExecutionManager_PopupMenu_SendBufferToExternalEditor), NULL);
    gtk_signal_connect_object (GTK_OBJECT (saveas_item), "activate", GTK_SIGNAL_FUNC (ExecutionManager_PopupMenu_SaveAsBuffer), NULL);

    gtk_widget_show (open_item);
    gtk_widget_show (openextern_item);
    gtk_widget_show (saveas_item);
}

void ExecutionManager_Initialize (void)
{
    ExecutionManager_CreatePopupMenu1 ();
    ExecutionWindow = CreateWindow ("ExecutionWindow");
    ExecutionWindowViewOptions = 2;
    ExecutingProcesses = NULL;
    timer_id = -1;
    timer_update_selected_row_id = -1;

    ExecutionManager_StopProcessButton_SetAttachedNode (NULL);
}

void ExecutionManager_ExecuteResultOfMakeNCommand (GList * infoCommandsList)
{
    struct InfoCommand *infoCommand = infoCommandsList->data;
    struct InfoExecutionProcess *infoRow = infoCommand->infoExecutionProcess;

    char *nameFileStdOut = g_strdup_printf ("%s_Out", infoRow->nameFileStdOut);

    //  printf("ExecuteResultOfMakeNCommand!   %d, %s\n", treeNode, infoRow->nameFileStdOut);

    FILE *f = fopen (nameFileStdOut, "r");
    char buf[100000];

    int position = 1;

    if (f)
    {
        while (fgets (buf, 100000, f))
        {
            //printf("read:%s\n",buf);
            while (buf[strlen (buf) - 2] == '\\')
                fgets (buf + strlen (buf) - 2, 100000 - strlen (buf), f);

            if (strncmp (buf, "make", 4) && buf[0])
            {
                struct InfoCommand *newStruct = (struct InfoCommand *) malloc (sizeof (struct InfoCommand));

                buf[strlen (buf) - 1] = 0;
                newStruct->name = g_strdup (buf);
                newStruct->treeNode = NULL;
                newStruct->infoExecutionProcess = NULL;

		infoCommandsList = g_list_insert (infoCommandsList, newStruct, position);
                position++;
            }
        }

        fclose (f);
    }

    ExecutionManager_RunInfoCommandListForNode (infoCommandsList->next, infoCommand->treeNode);
}

int DumpFileInTextBox (GtkTextView * textBox, char *filename, GdkColor * colour, int startPos,
	GtkScrolledWindow * scrolledWindow)
{
    FILE *f = fopen (filename, "r");
    char buf[513];
    int size;

    GtkTextBuffer *buffer = gtk_text_view_get_buffer (textBox);

    if (!f)
        return 0;

    fseek (f, startPos, SEEK_SET);

    /* AB unneeded, always at end?
    gtk_text_set_point (textBox, gtk_text_get_length (textBox));
    */

    // Check if scrollbar is at the end
    gboolean scrollbarIsAtTheEnd;
    GtkAdjustment *adjustment;

    {
        GtkVScrollbar *vScrollbar = GTK_VSCROLLBAR (scrolledWindow->vscrollbar);
        GtkScrollbar *scrollbar = &(vScrollbar->scrollbar);
        GtkRange *range = GTK_RANGE (&(scrollbar->range));

        adjustment = gtk_range_get_adjustment (range);
        //  printf("\n1/ %f %f %f - %f %f %f\n",adjustment->value,adjustment->lower,adjustment->upper,adjustment->step_increment,adjustment->page_increment,adjustment->page_size);
        scrollbarIsAtTheEnd = (adjustment->value + adjustment->page_size >= adjustment->upper);
    }

    while ((size = fread (buf, 1, 512, f)) > 0)
    {
    	GtkTextIter endOfBuffer;
        buf[size] = 0;
        gtk_text_buffer_get_end_iter (buffer, &endOfBuffer);
        gtk_text_buffer_insert (buffer, &endOfBuffer, buf, -1);

        /* AB, handle colours etc.
        gtk_text_insert (textBox, MessagesFont, colour, NULL, buf, -1);
        */
    }

    int pos = ftell (f);

    fclose (f);

    if (scrollbarIsAtTheEnd && pos != startPos)
    {                           // Move scrollbar
    /* AB
        gtk_text_thaw (textBox);
       */
        //  printf("%f %f %f\n",adjustment->value,adjustment->lower,adjustment->upper);
        gtk_adjustment_set_value (adjustment, adjustment->upper - adjustment->page_size);
        //  printf("->%f %f %f\n",adjustment->value,adjustment->lower,adjustment->upper);
    }

    return pos;
}

GtkCTreeNode *lastShownRow = NULL;

gint update_selected_row (gpointer data)
{
    static int positionStdOut = 0;
    static int positionStdErr = 0;

    GtkWidget *TreeWidget = LookupWidget (ExecutionWindow,
      "ExecutionCTree");
    GtkCTreeNode *node = GTK_CTREE_NODE (data);

    GtkTextView *textStdOut = GTK_TEXT_VIEW (LookupWidget (ExecutionWindow, "Text_StdOut"));
    GtkTextView *textStdErr = GTK_TEXT_VIEW (LookupWidget (ExecutionWindow, "Text_StdErr"));
    GtkScrolledWindow *scrollbarStdOut = GTK_SCROLLED_WINDOW (LookupWidget (ExecutionWindow, "StdOutScrollBar"));
    GtkScrolledWindow *scrollbarStdErr = GTK_SCROLLED_WINDOW (LookupWidget (ExecutionWindow, "StdErrScrollBar"));

    struct InfoExecutionProcess *newInfo = gtk_ctree_node_get_row_data (GTK_CTREE (TreeWidget),
      GTK_CTREE_NODE (node));

    //  printf("update selected row :%d %d %d\n",newInfo, newInfo->treeNode, node);

    /* Freeze the text widget, ready for multiple updates */
    /* AB, look into this
    gtk_text_freeze (GTK_TEXT (textStdOut));
    gtk_text_freeze (GTK_TEXT (textStdErr));
    */

    if (lastShownRow != node)
    {
        /* Clear the current text */
		GtkTextBuffer *stdErrBuffer = gtk_text_view_get_buffer (textStdErr);
		GtkTextBuffer *stdOutBuffer = gtk_text_view_get_buffer (textStdOut);

		GtkTextIter start, end;

		gtk_text_buffer_get_start_iter (stdErrBuffer, &start);
		gtk_text_buffer_get_end_iter (stdErrBuffer, &end);
		gtk_text_buffer_delete (stdErrBuffer, &start, &end);

		gtk_text_buffer_get_start_iter (stdOutBuffer, &start);
		gtk_text_buffer_get_end_iter (stdOutBuffer, &end);
		gtk_text_buffer_delete (stdOutBuffer, &start, &end);

        lastShownRow = node;
        positionStdOut = 0;
        positionStdErr = 0;
    }

    if (newInfo && newInfo->treeNode == node)
    {
        char *nameFileStdOut = (char *) newInfo->nameFileStdOut;

        if (nameFileStdOut)
        {
            char *filenameStdOut = g_strdup_printf ("%s_Out", nameFileStdOut);
            char *filenameStdErr = g_strdup_printf ("%s_Err", nameFileStdOut);

            //   printf("update selected row : %s\n", nameFileStdOut);

            positionStdOut = DumpFileInTextBox (textStdOut, filenameStdOut, &Green, positionStdOut, scrollbarStdOut);
            positionStdErr = DumpFileInTextBox (textStdErr, filenameStdErr, &Red, positionStdErr, scrollbarStdErr);

            gtk_object_set_data (GTK_OBJECT (textStdOut), "filename", (gpointer) filenameStdOut);
            gtk_object_set_data (GTK_OBJECT (textStdErr), "filename", (gpointer) filenameStdErr);
        }
    }

    /* Thaw the text widget, allowing the updates to become visible */
    /* AB
    gtk_text_thaw (GTK_TEXT (textStdOut));
    gtk_text_thaw (GTK_TEXT (textStdErr));
    */

    if (g_list_find (ExecutingProcesses, data))
        return TRUE;
    else
    {
        ExecutionManager_StopProcessButton_SetAttachedNode (NULL);
        timer_update_selected_row_id = -1;
        return FALSE;
    }
}

void OnExecutionCTree_SelectRow (GtkCTree * tree, GtkCTreeNode * node, gint column)
{
    gtk_object_set_data (GTK_OBJECT (tree), "selectedNode", (gpointer) node);

    if (update_selected_row (node))
    {
        ExecutionManager_StopProcessButton_SetAttachedNode (node);
        if (timer_update_selected_row_id == -1)
            timer_update_selected_row_id = gtk_timeout_add (1000, update_selected_row, node);
    }
}

void OnExecutionCTree_UnselectRow (GtkCTree * tree, GtkCTreeNode * node, gint column)
{
    gtk_object_set_data (GTK_OBJECT (tree), "selectedNode", (gpointer) NULL);

    if (timer_update_selected_row_id != -1)
    {
        gtk_timeout_remove (timer_update_selected_row_id);
        timer_update_selected_row_id = -1;
    }
    ExecutionManager_StopProcessButton_SetAttachedNode (NULL);
}

gboolean OnTextBox_ButtonPressed (GtkWidget * widget, GdkEventButton * event)
{
    if (event->button == 3)     //Right button pressed
    {
//        GdkEventButton *bevent = (GdkEventButton *) event;

        popupMenu1Caller = widget;
        gtk_menu_popup (GTK_MENU (popupMenu1), NULL, NULL, NULL, NULL, 0, 0);
    } else
    {
        GtkEditable *editable = GTK_EDITABLE (widget);
        int pos = gtk_editable_get_position (editable);

        char *buf = NULL;

        do
        {
            g_free (buf);
            buf = gtk_editable_get_chars (editable, pos, pos + 1);
            if (pos)
                pos--;
            if (!buf)
                return FALSE;
        }
        while ((pos > 0) && (*buf != '\n'));
        if (*buf == '\n')
            pos += 2;

        g_free (buf);
        buf = gtk_editable_get_chars (editable, pos, pos + 1000);
        if (!buf)
            buf = gtk_editable_get_chars (editable, pos, -1);
        if (!buf)
            return FALSE;

        {
            char *ptrCR, *ptr1, *ptr2;

            ptr1 = ptr2 = NULL;

            ptrCR = strstr (buf, "\n");
            if (!ptrCR)
                ptrCR = buf + 1000;
            ptr1 = strstr (buf, ":");
            if (!ptr1 || (ptr1 > ptrCR))
                return FALSE;
            ptr2 = strstr (ptr1 + 1, ":");
            if (!ptr2 || (ptr2 > ptrCR))
                return FALSE;

            while (*buf == ' ' || *buf == '\t')
                buf++;

            {
                char *filename;
                int row, col;

                filename = (char *) malloc (ptr1 - buf + 1);
                strncpy (filename, buf, ptr1 - buf);
                filename[ptr1 - buf] = 0;
                row = atoi (ptr1 + 1);
                col = atoi (ptr2 + 1);

                if (col == 0)
                    col = 1;

                //      printf("coucou %d %d %s %d %d\n",event->button,pos,filename, row,col);
                FileManager_OpenFileAtLineAndColumn (filename, row, col - 1);

                gtk_editable_select_region (editable, pos, pos + (int) (ptrCR - buf));
            }
        }
    }

    /* Tell calling code that we have not handled this event; pass it on. */
    return FALSE;
}

void DeleteTreeNode (GtkCTree * ctree, GtkCTreeNode * node, gpointer data)
{
    struct InfoExecutionProcess *infoRow = gtk_ctree_node_get_row_data (ctree, node);

    if (infoRow && infoRow->nameFileStdOut)
    {
        char *nameFileStdOut = g_strdup_printf ("%s_Out", infoRow->nameFileStdOut);
        char *nameFileStdErr = g_strdup_printf ("%s_Err", infoRow->nameFileStdOut);

        unlink (nameFileStdOut);
        unlink (nameFileStdErr);
    }
    //  gtk_ctree_remove_node( ctree, node );
}

gboolean OnExecutionWindowDelete (GtkWidget * widget, GdkEvent * event, gpointer killAll)
{
    GtkCTree *TreeWidget = GTK_CTREE (LookupWidget (ExecutionWindow, "ExecutionCTree"));

    if (ExecutingProcesses)
    {
        if ((long) killAll != 1)
            printfConsole ("Some Processes are still running. You have to stop them before closing the Execution Window!\n");
        else
        {
            static GtkWidget *KillAllConfirmDialogue = NULL;

            if (!KillAllConfirmDialogue)
                KillAllConfirmDialogue = CreateWindow ("KillAllConfirmDialogue");
            gtk_widget_show (KillAllConfirmDialogue);
        }
        return TRUE;
    }

    gtk_ctree_post_recursive (TreeWidget, NULL, DeleteTreeNode, NULL);
    {
        GtkCList *list = GTK_CLIST (&(TreeWidget->clist));

        gtk_clist_clear (list);

        lastShownRow = NULL;
    }

    gtk_widget_hide (ExecutionWindow);

    if ((long) killAll == 1)
        gtk_main_quit ();

    return TRUE;
}

void ExecutionManager_RunInfoCommandsList (GList * infoCommandsList)
{
    GtkWidget *TreeWidget = LookupWidget (ExecutionWindow,
      "ExecutionCTree");

    // Create an InfoExecutionProcess structure for each command to be executed
    {
        GList *tmp;

        for (tmp = infoCommandsList; tmp; tmp = tmp->next)
        {
            struct InfoCommand *infoCommand = tmp->data;

            if (infoCommand->treeNode)
            {
                struct InfoExecutionProcess *newInfo = (struct InfoExecutionProcess *) malloc (sizeof (struct InfoExecutionProcess));

                newInfo->treeNode = GTK_CTREE_NODE (infoCommand->treeNode);
                newInfo->nameFileStdOut = NULL;
                //     newInfo->nameFileStdErr = NULL;
                newInfo->numProcess = 0;

                gtk_ctree_node_set_row_data (GTK_CTREE (TreeWidget), GTK_CTREE_NODE (infoCommand->treeNode), (gpointer) newInfo);
                infoCommand->infoExecutionProcess = newInfo;
            } else
                infoCommand->infoExecutionProcess = NULL;
        }

    }

    {
        int childPid;
        int parentProcessPid = getpid ();

        // Create the communication channel
        int comPipe[2];

        pipe (comPipe);
        int *tagp = g_new (int, 1);
        int tag = gdk_input_add (comPipe[0], GDK_INPUT_READ, communication_input_callback,
          (gpointer) tagp);

        *tagp = tag;

        childPid = fork ();

        if (childPid == 0)
        {                       /* in the child process */
            int processPid = getpid ();
            int index;

            executionStopped = FALSE;
            if (signal (SIGUSR1, signal_SIGUSR1_handler) == SIG_ERR)
            {                   //todo
            }

            for (index = 0; infoCommandsList; infoCommandsList = infoCommandsList->next, index++)
            {
                struct InfoCommand *infoCommand = infoCommandsList->data;
                char *command = infoCommand->name;

                // TODO: check command
//                char **args = str_split (command);
//                int i = 0;

                char *nameFileStdOut = g_strdup_printf ("%s/tmp_%d_%d_%d",
                  EnvironmentOptions.TmpDir,
                  parentProcessPid,
                  processPid, index);
                char *msgAfterExecution = g_strdup_printf ("1 %p\n", infoCommandsList->data);
                int statuspid;

                if (!infoCommand->treeNode && (command[0] == '~'))
                {
                    char *msgWhenMakeCommand = g_strdup_printf ("10 %p %s\n",
                      infoCommandsList->next,
                      command);

                    write (comPipe[1], msgWhenMakeCommand, strlen (msgWhenMakeCommand));
                    continue;
                }

                currentlyExecutedPid = fork ();

                if (currentlyExecutedPid == 0)
                {               /* in the child process */
                    int fileStdOut = open (g_strdup_printf ("%s_Out",
                        nameFileStdOut),
                      O_RDWR | O_CREAT,
                      S_IRUSR | S_IWUSR);
                    int fileStdErr = open (g_strdup_printf ("%s_Err",
                        nameFileStdOut),
                      O_RDWR | O_CREAT,
                      S_IRUSR | S_IWUSR);

                    chdir (CurrentBalsaProject->directory);
                    dup2 (fileStdOut, 1); /* redirect stdout */
                    dup2 (fileStdErr, 2); /* redirect stderr */
                    close (fileStdOut);
                    close (fileStdErr);

                    setpgid (0, 0);
                    int ret = system (command);

                    //  execvp (args[0], args);
                    //          printf ("2 %d %d %d %d %d\n",ret,WIFEXITED (ret),WEXITSTATUS (ret),WIFSIGNALED (ret),WTERMSIG (ret));
                    fflush (NULL);
                    if (ret)
                    {
                        if (WIFEXITED (ret))
                            _exit (WEXITSTATUS (ret));
                        else if (WIFSIGNALED (ret))
                            _exit (100 + WTERMSIG (ret));
                        else
                            _exit (133);
                    } else
                        _exit (0);
                } else
                {
                    char *msgBeforeExecution = g_strdup_printf ("0 %p %d %s\n",
                      infoCommandsList->data,
                      currentlyExecutedPid,
                      nameFileStdOut);

                    write (comPipe[1], msgBeforeExecution, strlen (msgBeforeExecution));
                    while (waitpid (currentlyExecutedPid, &statuspid, 0) == -1 && errno == EINTR);
                }

                if (!WIFEXITED (statuspid) || WEXITSTATUS (statuspid) || executionStopped || WIFSIGNALED (statuspid))
                {
                    //  printf("error pid %d\n",index);
                    char *msgAfterErrorExecution = g_strdup_printf ("2 %p %d\n",
                      infoCommandsList->data,
                      WEXITSTATUS (statuspid));

                    write (comPipe[1], msgAfterErrorExecution, strlen (msgAfterErrorExecution));
                    // don't execute other processes
                    for (infoCommandsList = infoCommandsList->next; infoCommandsList; infoCommandsList = infoCommandsList->next)
                    {
                        char *msgNoExecution = g_strdup_printf ("3 %p\n",
                          infoCommandsList->data);

                        write (comPipe[1], msgNoExecution, strlen (msgNoExecution));
                    }
                } else if (!strncmp (command, "make -n ", 8))
                {
                    char *msgAfterMakeNExecution = g_strdup_printf ("4 %p\n",
                      infoCommandsList);

                    write (comPipe[1], msgAfterMakeNExecution, strlen (msgAfterMakeNExecution));
                    // don't execute other processes
                    //                    infoCommandsList = 0;
                    break;
                } else
                    write (comPipe[1], msgAfterExecution, strlen (msgAfterExecution));
            }

            {
                char *msgAfterWholeExecution = g_strdup_printf ("5 %d %d %d\n", tag, comPipe[0], comPipe[1]);

                write (comPipe[1], msgAfterWholeExecution, strlen (msgAfterWholeExecution));
            }
            _exit (0);
        } else
        {                       /* the parent, logger */
            // update the field 'numProcess' in the ~going to be~ executed structures 'InfoExecutionProcess'
            GList *tmp;

            for (tmp = infoCommandsList; tmp; tmp = tmp->next)
            {
                struct InfoCommand *infoCommand = tmp->data;

                if (infoCommand->infoExecutionProcess)
                    infoCommand->infoExecutionProcess->numProcess = childPid;
            }
        }
    }
}

void ExecutionManager_RunInfoCommandListForNode (GList * infoCommandList, GtkCTreeNode * treeNode)
{
    GtkWidget *TreeWidget = LookupWidget (ExecutionWindow,
      "ExecutionCTree");
    gchar *text[3];

    // Write the command names in the tree
    GList *tmp;
    GtkCTreeNode *newChildNode;

    for (tmp = infoCommandList; tmp; tmp = tmp->next)
    {
        struct InfoCommand *infoCommand = tmp->data;

        if (!infoCommand->treeNode && (infoCommand->name[0] != '~'))
        {
            text[0] = infoCommand->name;
            text[1] = "-";
            text[2] = "waiting...";
            newChildNode =
              gtk_ctree_insert_node (GTK_CTREE (TreeWidget),
              treeNode, NULL, text, 4, NULL, NULL, NULL, NULL, (strncmp (infoCommand->name, "make ", 5)) ? TRUE : FALSE, TRUE);

            infoCommand->treeNode = newChildNode;
        }
    }

    gtk_widget_draw (ExecutionWindow, NULL);
    gtk_widget_draw (TreeWidget, NULL);

    ExecutionManager_RunInfoCommandsList (infoCommandList);
}

void ExecutionManager_CheckBuildMakefileAndRunCommandList_afterSaveConfirmation (GList * commandList)
{
    // If needed : Make Makefile
    if (CheckIfNeedToRebuildMakefile ()) //check according to dates
    {
        char *command = generate_balsamd_command ();

        commandList = g_list_prepend (commandList, command);
    }

    ExecutionManager_RunCommandList (commandList);
}

void ExecutionManager_CheckBuildMakefileAndRunCommandList_SaveCheck_OnNo (GtkWidget * button, GList * commandList)
{
    ExecutionManager_CheckBuildMakefileAndRunCommandList_afterSaveConfirmation (commandList);
}

void ExecutionManager_CheckBuildMakefileAndRunCommandList_SaveCheck_OnYes (GtkWidget * button, GList * commandList)
{
    OnProjectMenu_Save (NULL);
    if (CurrentBalsaProject->dirty_and_need_make_clean)
    {
        char *command = g_strdup ("make clean");

        commandList = g_list_prepend (commandList, command);
        CurrentBalsaProject->dirty_and_need_make_clean = FALSE;
    }

    ExecutionManager_CheckBuildMakefileAndRunCommandList_SaveCheck_OnNo (NULL, commandList);
}

void ExecutionManager_CheckBuildMakefileAndRunCommandList (GList * commandList)
{
    // todo maybe: improve the following function to be able to use it with a callback with arguments
    //  ConfirmSaveCurrentBalsaProjectAndExecute( ExecutionManager_CheckBuildMakefileAndRunCommandList_afterSaveConfirmation );

    /* check if the current project needs to be saved */
    if (CurrentBalsaProject && (CurrentBalsaProject->dirty || CurrentBalsaProject->nb_dirty_files))
    {
        GtkWidget *dialogue;
        const gchar *buttons[] = { "gtk-yes", "gtk-no", "gtk-cancel" };
        GtkSignalFunc handlers[] = {
            (GtkSignalFunc) ExecutionManager_CheckBuildMakefileAndRunCommandList_SaveCheck_OnYes,
            (GtkSignalFunc) ExecutionManager_CheckBuildMakefileAndRunCommandList_SaveCheck_OnNo,
            NULL
        };

        dialogue =
          util_create_dialogue_with_buttons
          ("The current project has been modified.\nDo you want to save it before executing the command?", 3, buttons, 3, handlers, commandList);
        gtk_window_set_position (GTK_WINDOW (dialogue), GTK_WIN_POS_MOUSE);
        gtk_widget_show (dialogue);
    } else
        ExecutionManager_CheckBuildMakefileAndRunCommandList_afterSaveConfirmation (commandList);
}

void ExecutionManager_RunCommandList (GList * commandList)
{
    GList *infoCommandList = NULL;

    // Display execution window if needed
    if ((ExecutionWindowViewOptions == 0) || (ExecutionWindowViewOptions == 2))
        gtk_widget_show (ExecutionWindow);

    {
        GList *tmp = commandList;

        //    printf("CommandList:\n");
        for (; tmp; tmp = tmp->next)
        {
            struct InfoCommand *newStruct = (struct InfoCommand *) malloc (sizeof (struct InfoCommand));

            newStruct->name = tmp->data;
            newStruct->treeNode = NULL;
            newStruct->infoExecutionProcess = NULL;
            infoCommandList = g_list_append (infoCommandList, newStruct);
            // printf(" %s\n",tmp->data);
        }
        //    printf(".\n");
    }

    ExecutionManager_RunInfoCommandListForNode (infoCommandList, NULL);
}

void OnExecutionWindowStopProcessButton (GtkButton * button)
{
    GtkWidget *TreeWidget = LookupWidget (ExecutionWindow,
      "ExecutionCTree");
    GtkCTreeNode *treeNode = GTK_CTREE_NODE (gtk_object_get_data (GTK_OBJECT (button),
        "associatedTreeNode"));
    struct InfoExecutionProcess *info = gtk_ctree_node_get_row_data (GTK_CTREE (TreeWidget),
      treeNode);
    int numProcess = info->numProcess;

    //  printf("killing %d\n", numProcess );
    if (numProcess)
        kill (numProcess, SIGUSR1);
}

void ExecutionManager_RemoveTmpFilesOfOldSessions (void)
{
    DIR *dir = opendir (EnvironmentOptions.TmpDir);
    struct dirent *entry;

    while (dir && (entry = readdir (dir)))
    {
        if (!strncmp (entry->d_name, "tmp_", 4))
        {
            int numPid = atoi (entry->d_name + 4);
            int pidNotRunning = kill (numPid, 0);

            if (pidNotRunning)
            {
                char *name = g_strdup_printf ("%s/%s",
                  EnvironmentOptions.TmpDir,
                  entry->d_name);

                unlink (name);
                free (name);
            }
        }
    }

    if (dir)
        closedir (dir);
}

gint finish_quit (gpointer data)
{
    OnExecutionWindowDelete (NULL, NULL, (gpointer) 1);
    return TRUE;
}

void OnKillAllConfirm (GtkButton * button)
{
    GtkWidget *dialogue = gtk_widget_get_toplevel (GTK_WIDGET (button));

    gtk_widget_hide (dialogue);

    GtkWidget *TreeWidget = LookupWidget (ExecutionWindow, "ExecutionCTree");

    GList *tmp = ExecutingProcesses;

    ExecutingProcesses = NULL;

    for (; tmp; tmp = tmp->next)
    {
        GtkCTreeNode *treeNode = GTK_CTREE_NODE (tmp->data);

        struct InfoExecutionProcess *infoRow = gtk_ctree_node_get_row_data (GTK_CTREE (TreeWidget), treeNode);

//    int fileStdOutExists, fileStdErrExists;
//    struct stat infosFileStdOut, infosFileStdErr;
//    int num_message = 0;
//    char message[100];
//    char *nameFileStdOut, *nameFileStdErr;

        int numProcess = infoRow->numProcess;

        //  printf("killing %d\n", numProcess );
        if (numProcess)
            kill (numProcess, SIGUSR1);
    }

    gtk_timeout_add (1000, finish_quit, NULL);
}

void OnKillAllCancel (GtkButton * button)
{
    GtkWidget *dialogue = gtk_widget_get_toplevel (GTK_WIDGET (button));

    gtk_widget_hide (dialogue);
}
