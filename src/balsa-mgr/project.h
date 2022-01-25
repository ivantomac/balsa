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

	`project.h'
	Project details structure

*/

#ifndef PROJECT_HEADER
#define PROJECT_HEADER

#include <glib.h>
#include <gtk/gtk.h>

typedef struct BalsaProcedure
{
    char *name;
    char *associated_filename;
    GtkCTreeNode *treeNode;
}
BalsaProcedure, *PtrBalsaProcedure;

/* FIXME, prefix this name */
enum SimulationSystem
{ breezesim };

typedef struct BalsaProject
{
    char *name;                 /* Pretty name of this project, can contain any chars */
    char *directory;            /* Directory in which it resides, unique! */
    GList *files;               /* list of PtrBalsaFile's */
    GList *importPath;          /* list of directories (char *'s) giving the
                                   elements of this project's BALSAHOME */
    enum SimulationSystem simulationSystem;
    //  char *technology;
    GList *defines;
    char *BALSACOPTS;
    char *BREEZESIMOPTS;
    char *BREEZELINKOPTS;

    gboolean dirty;             /* TRUE => this project needs to be saved */
    gboolean dirty_and_need_make_clean; /* TRUE when compilation options are changed */
    int nb_dirty_files;         /* Nb of files who are dirty in the internal filemanager */
    gboolean needToRebuildMakefile;
}
BalsaProject, *PtrBalsaProject;

/* BalsaProjectEntryDetailsNames : names to be used in the `Details' column of the file view */
extern char *BalsaProjectEntryDetailsNames[8];

typedef enum BalsaProjectEntryNature
{
    BalsaProjectEntry_File,
    BalsaProjectEntry_Directory,
    BalsaProjectEntry_Procedure,
    BalsaProjectEntry_Test,
    BalsaProjectEntry_TestComponent,
    BalsaProjectEntry_Implementation,
    BalsaProjectEntry_BuiltinLib
}
BalsaProjectEntryNature;

typedef struct BalsaProjectEntry
{
    BalsaProjectEntryNature nature;
    gpointer data;
}
BalsaProjectEntry, *PtrBalsaProjectEntry;

typedef struct BalsaDefine
{
    char *name;
    char *value;
}
BalsaDefine, *PtrBalsaDefine;

#define BALSA_PROJECT(ptr) ((PtrBalsaProject)(ptr))
#define BALSA_PROJECT_ENTRY(ptr) ((PtrBalsaProjectEntry)(ptr))
#define BALSA_PROCEDURE(ptr) ((PtrBalsaProcedure)(ptr))

/* NewBalsaProject : make a new, empty, project */
extern PtrBalsaProject NewBalsaProject (void);

/* NewBalsaProjectEntry : `file' view entry wrapper */
extern PtrBalsaProjectEntry NewBalsaProjectEntry (BalsaProjectEntryNature nature, gpointer data);

/* DeleteBalsaProject : dispose of a Balsa project structure in an orderly way */
extern void DeleteBalsaProject (PtrBalsaProject project);

/* DeleteBalsaProjectEntry : dispose func. */
extern void DeleteBalsaProjectEntry (PtrBalsaProjectEntry entry);

/* WriteBalsaProjectFile : make a Balsa project file in the lispy format */
extern void WriteBalsaProjectFile (char *directory, PtrBalsaProject project);

/* ReadBalsaProjectFile : read in a Balsa project file (called Project)
	and make a new BalsaProject, return NULL on error */
extern PtrBalsaProject ReadBalsaProjectFile (const char *directory);

/* ReadBalsaProjectListFile : read in the project list file.  This is usually
	used to read in the ~/.balsa-projects list on startup.  Opens all the projects,
	returns a list of them.  Returns FALSE on error or empty project list. */
extern GList *ReadBalsaProjectListFile (char *filename);

/* WriteBalsaProjectListFile : write the given project list into the given file,
	return TRUE if successfull, FALSE if not */
extern gboolean WriteBalsaProjectListFile (char *filename, GList * projects);

/* UpdateBalsaProjectFromFileView : update the project from the project's ctree file view */
extern void UpdateBalsaProjectFromFileView (PtrBalsaProject project);

/* DeleteBalsaProjectList : deallocate everything to do with a given project list */
extern void DeleteBalsaProjectList (GList * projects);

/* BalsaProjectNeedsToBeSaved : returns TRUE if the project is dirty and
	has a directory */
extern gboolean BalsaProjectNeedsToBeSaved (PtrBalsaProject project);

void CloseCurrentBalsaProject (void);
void ConfirmSaveCurrentBalsaProjectAndExecute (GtkSignalFunc func);
extern PtrBalsaProject CurrentBalsaProject;

gboolean CheckIfNeedToRebuildMakefile (void);

gboolean IsFileInProject (char *filename);

void SetAsCurrentBalsaProject (PtrBalsaProject project);

void NewCurrentBalsaProject (const char *name, const char *directory);
gboolean AddImportPathToCurrentBalsaProject (char *directory);
gboolean AddDefineToCurrentBalsaProject (char *name, char *value);
void ShowProjectPane (gboolean showNhide);

#endif
