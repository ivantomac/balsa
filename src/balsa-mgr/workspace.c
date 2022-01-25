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

#include "workspace.h"

struct _Workspace workSpace;

void init_workspace (void)
{
    /* Initialise to NULL to start with so that accidentally called callbacks
        in the initialisation process have something to test for */
    workSpace.mainWindow = NULL;
    workSpace.executionWindow = NULL;
    workSpace.consoleWindow = NULL;
    workSpace.aboutDialogue = NULL;
    workSpace.optionsDialogue = NULL;
    workSpace.projectOptionsDialogue = NULL;

    workSpace.mainWindow = new_mainWindow ();
    init_mainWindow ();
    workSpace.executionWindow = new_executionWindow ();
    workSpace.consoleWindow = new_consoleWindow ();
    workSpace.aboutDialogue = new_aboutDialogue ();
    workSpace.optionsDialogue = new_optionsDialogue ();

    workSpace.projectOptionsDialogue = new_projectOptionsDialogue ();
}
