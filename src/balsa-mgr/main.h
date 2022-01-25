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

	`main.h'
	Signal handlers and main function

*/

#ifndef MAIN_HEADER
#define MAIN_HEADER

#include <gtk/gtk.h>
#include "project.h"
#include "mainwindow.h"
#include "file.h"

#define BEGINS_WITH(x,y) !strncmp(x,y,strlen(y))

extern char *StartupPWD;

extern GList *OtherWindows_CurrentProject;
extern GList *OtherWindows_ClosedProjects;

/* Messages... : console message window colours, objects */
extern GdkFont *MessagesFont;
extern GdkColor Red, Green, Blue;

/* ...Pixmap : piccies and masks */
GdkPixmap *SyncPortPixmap;
GdkBitmap *SyncPortMask;
GdkPixmap *InputFilePortPixmap;
GdkPixmap *OutputFilePortPixmap;
GdkBitmap *InputFilePortMask;
GdkBitmap *OutputFilePortMask;
GdkPixmap *InputValuePortPixmap;
GdkPixmap *OutputStdoutPortPixmap;
GdkBitmap *InputValuePortMask;
GdkBitmap *OutputStdoutPortMask;

extern GdkPixmap *TickPixmap;
extern GdkBitmap *TickMask;
extern GdkPixmap *SelectedPixmap;
extern GdkBitmap *SelectedMask;

/* CurrentProject : currently visible project on the ProjectsNotebook */
extern PtrBalsaProject CurrentProject;

/* Projects... : project list and filename for same list.  If ProjectsFilename
	is NULL then no filename is yet bound to the project list */
extern GList *Projects;
extern char *ProjectsFilename;

/* ... : various top level widgets */
extern GtkWidget *ProjectsNotebook;
extern GtkWidget *StatusBar;

/* UpdateMainWindowGreying : update the greying out of menus/menu items in the MainWindow */
extern void UpdateMainWindowGreying (void);

/* UpdateProjectNotebookPage : refresh project page info */
extern void UpdateProjectNotebookPage (PtrBalsaProject project, gboolean updateFileView);

/* AddBalsaProjectToProjectList : Add a new project to the project list */
extern void AddBalsaProjectToProjectList (PtrBalsaProject project);

/* FileSelectionCallback : function to call when OK is pressed in a file/directory
	dialogue. Returns TRUE if the dialogue should be closed after the press or
	FALSE if the dialogue should persist */
typedef gboolean (*FileSelectionCallback) (const char *, gpointer);

typedef enum {
	FileSelectionDialogue_Open,
	FileSelectionDialogue_Save
} FileSelectionDialogueType;

/* MakeDirectorySelectionDialogue : make a directory selection dialogue
	which calls callback `callback' on pressing OK. Destroys itself
	on destruction of the parent window `parent' (if any) and when either
	of OK or Cancel are pressed. `initialDirectory' is the starting 
	directory selection and displayed directory. */
extern GtkWidget *MakeDirectorySelectionDialogue (const char *title,
  const char *initialDirectory, FileSelectionCallback callback, gpointer callbackData, GtkSignalFunc changeDirCallBack, GtkWidget * parent);

/* MakeFileSelectionDialogue : like MakeDirectorySelectionDialogue
	but for file selection instead of directories. */
extern GtkWidget *MakeFileSelectionDialogue (FileSelectionDialogueType type,
	char *title, FileSelectionCallback callback, gpointer callbackData, GtkWidget * parent);

/* SetEntryStringCallback : set the given entry to the string value passed */
extern gboolean SetEntryStringCallback (char *string, GtkWidget * entry);

/* CloseAllBalsaProjects : like CloseBalsaProject but run across the whole Projects
	list. Provides the user with confirmation and a YesToAll option. Calls
	the function `done' when all projects have been closed. */
extern void CloseAllBalsaProjects (void (*done) (void));

void OnProjectMenu_Save (GtkMenuItem * menuitem);
int OpenRecentButtonPressedCallback (GtkWidget * widget, GdkEventButton * event);

void UpdateProjectTree (void);
void OnProjectToolbar_LaunchEditor (GtkMenuItem * button);

void OnSelectionMenu_GetInfo (GtkMenuItem * button);

struct environmentOptions
{
    char *BalsaHome;
    char *TmpDir;
    char *editor;
    char *PrintCommand;
    char *PSViewer;
    int projectsHistoryMaxSize;
    //    int pathsAbsoluteRelativeOption; //1=absolute, 2=relative, 3=ask
    GSList *projectsHistory;
    GSList *tools;
    GSList *newProjectTemplate;
};
extern struct environmentOptions EnvironmentOptions;

GPtrArray *FindTestPortsForProcedure (const char *filename, const char *procedureName);
void UpdateProjectTrees (void);
void UpdateProjectName (void);
void UpdateProjectTreeFilesView (void);

extern PtrBalsaProjectEntry ProjectFileViewSelectedEntry;

void AddToOtherWindows_CurrentProject (GtkWidget * dialogue);
void AddToOtherWindows_ClosedProjects (GtkWidget * dialogue);
void RemoveFromOtherWindows_CurrentProject (GtkWidget * dialogue);
void RemoveFromOtherWindows_ClosedProjects (GtkWidget * dialogue);
void DestroyOtherWindow (GtkWidget * dialogue);

char *generate_balsamd_command (void);

extern gboolean disable_filenameEntry;

void AddImplementationToFile (PtrBalsaProject project, PtrBalsaFile balsaFile, PtrImplementation impl);
void AddImplementationToTest (PtrBalsaProject project, PtrBalsaTest balsaTest, PtrImplementation impl);
void AddBuiltinLibToFile (PtrBalsaProject project, PtrBalsaFile balsaFile, PtrBuiltinLib lib);
void AddFileToProject (PtrBalsaProject project, PtrBalsaFile file, char *CompletePathName);
void AddImplementationsToProjectFilesViewTree (GtkCTreeNode * nodeParent, GList * implementations);

#endif
