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

	`menus.h'
	Menus!
*/

#ifndef MENUS_HEADER

#include <gtk/gtk.h>

extern void OnProjectMenu_ProjectOptions (GtkMenuItem * button);

void OnSelectionMenu_Delete_AfterConfirmation (GtkMenuItem * menuitem);
void OnSelectionMenu_Delete (GtkMenuItem * menuitem);
void OnSelectionMenu_Unselect (GtkMenuItem * menuitem);
void OnSelectionMenu_AddTest (GtkMenuItem * menuitem);
void OnSelectionMenu_AddImplementation (GtkMenuItem * menuitem);
void OnSelectionMenu_AddBuiltinLib (GtkMenuItem * menuitem);
void OnSelectionMenu_Edit (GtkMenuItem * menuitem);
void OnSelectionMenu_Make (GtkMenuItem * menuitem);

extern int ViewFilesOption;
extern int ViewProceduresOption;
extern int ViewTestsOption;

extern int ConsoleWindowViewOptions;

void OnViewMenu_Console_DisplayHide (GtkMenuItem * button);
void OnViewMenu_ExecutionWindow_DisplayHide (GtkMenuItem * button);

void init_mainWindow_menus (void);

#endif
