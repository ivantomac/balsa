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

*/

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include "mainwindow.h"
#include "filemanager.h"
#include "icons.h"
#include "workspace.h"
#include "main.h"
#include "support.h"
#include "menus.h"

GtkWidget *MainWindow;
GtkObject *MainWindowObject;

void MakeTopLevelWindow (void)
{
    MainWindow = CreateWindow ("MainWindow");
    MainWindowObject = GTK_OBJECT (MainWindow);
}

struct _MainWindow *new_mainWindow (void)
{
    struct _MainWindow *obj = (struct _MainWindow *) g_malloc (sizeof (struct _MainWindow));

    return obj;
}

void init_mainWindow (void)
{
	gtk_signal_connect (GTK_OBJECT (LookupWidget (MainWindow, "Icon_OpenProject")), "button_press_event",
		(GtkSignalFunc) OpenRecentButtonPressedCallback, NULL);
	FileManager_Initialize ();
    init_mainWindow_menus ();
    UpdateMainWindowGreying ();
}
