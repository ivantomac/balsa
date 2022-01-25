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

	`options.h'
	Global options/objects management

*/

#ifndef OPTIONS_HEADER
#define OPTIONS_HEADER

#include <glib.h>

/* Options_BalsaHome : default system Balsa install directory */
extern char *Options_BalsaHome;
extern char *Options_TmpDir;
extern GList *Options_BalsaHomeList; /* Same but as a list of strings w/o :s */

/* Options_Editor : editor to spawn for project file editing */
extern char *Options_Editor;
extern char *Options_PrintCommand;
extern char *Options_PSViewer;

/* StringArrayToList : convert a string array into a list of strings */
extern GList *StringArrayToList (char **array);

/* StringListToArray : as StringArrayToList but the other way around */
extern char **StringListToArray (GList * list);

/* InitOptions : initialise global objects */
extern void InitOptions (void);

void UpdateRecentProjectsHistoryInMenu (void);
void ReadEnvironmentOptions (void);
void WriteEnvironmentOptions (void);
char *EnvironmentOptions_GetBalsaBinDirectory (void);

#endif
