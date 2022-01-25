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

#include <stdlib.h>

#include "aboutdialogue.h"
#include "workspace.h"
#include "support.h"

struct _AboutDialogue *new_aboutDialogue (void)
{
    struct _AboutDialogue *obj = (struct _AboutDialogue *) malloc (sizeof (struct _AboutDialogue));

    obj->dialogue = NULL;

    return obj;
}

GtkWidget *get_workspace_aboutDialogue_dialogue (void)
{
    if (workSpace.aboutDialogue->dialogue == NULL)
    {
        GtkWidget *dialogue;
        GtkLabel *VersionLabel;
        GtkLabel *DateLabel;

        dialogue = CreateWindow ("AboutDialogue");
        VersionLabel = GTK_LABEL (LookupWidget (dialogue, "VersionLabel"));
        DateLabel = GTK_LABEL (LookupWidget (dialogue, "DateLabel"));
        gtk_label_set_text (VersionLabel, BALSA_MGR_VERSION_NUMBER);
        gtk_label_set_text (DateLabel, BALSA_MGR_VERSION_DATE);

        workSpace.aboutDialogue->dialogue = dialogue;
    }

    return workSpace.aboutDialogue->dialogue;
}
