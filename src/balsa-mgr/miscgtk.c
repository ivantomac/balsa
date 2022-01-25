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

	`miscgtk.c'
	Miscellaneous GTK wrapper functions

*/

#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#include "miscgtk.h"

static char StatusBarBuffer[4096]; /* Fixed sized buffer, hmm */

/* CTreePrevNodeAtSameLevel : return the node previous to this one at the
	same level or NULL for none */
GtkCTreeNode *CTreePrevNodeAtSameLevel (GtkCTreeNode * node)
{
    GtkCTreeNode *prevNode;

    if (!node)
        return NULL;

    prevNode = GTK_CTREE_NODE_PREV (node);
    while (prevNode && GTK_CTREE_ROW (prevNode)->level != GTK_CTREE_ROW (node)->level)
    {
        prevNode = GTK_CTREE_NODE_PREV (prevNode);
    }

    return prevNode;
}

/* CTreeNextNodeAtSameLevel : ditto but for next node */
GtkCTreeNode *CTreeNextNodeAtSameLevel (GtkCTreeNode * node)
{
    GtkCTreeNode *nextNode;

    if (!node)
        return NULL;

    nextNode = GTK_CTREE_NODE_NEXT (node);
    while (nextNode && GTK_CTREE_ROW (nextNode)->level != GTK_CTREE_ROW (node)->level)
    {
        nextNode = GTK_CTREE_NODE_NEXT (nextNode);
    }

    return nextNode;
}

/* PushStatusBarMessage : push a formatted message onto the given status bar */
void PushStatusBarMessage (GtkWidget * statusBar, char *format, ...)
{
    va_list args;

    va_start (args, format);

    vsprintf (StatusBarBuffer, format, args);
    gtk_statusbar_push (GTK_STATUSBAR (statusBar), 1, StatusBarBuffer);

    va_end (args);
}

/* MakeListItemListFromStringList : turn a list of strings into a list of
	GtkListItems suitable for inclusion in a list with the usual functions,
	NB. may need to attach tags to these at some point */
GList *MakeListItemListFromStringList (GList * labelStrings)
{
    GList *items = NULL;

    while (labelStrings)
    {
        GtkWidget *item = gtk_list_item_new_with_label ((char *) labelStrings->data);

        gtk_object_set_data (GTK_OBJECT (item), "label", g_strdup (labelStrings->data));

        items = g_list_append (items, (gpointer) item);
        gtk_widget_show (item);

        labelStrings = labelStrings->next;
    }
    return items;
}

/* GetListItemLabelText : get the char * text from the label inside the
	given list item in the currently favoured way. NB. don't deallocate this text */
char *GetListItemLabelText (GtkWidget * listItem)
{
    GList *children = gtk_container_children (GTK_CONTAINER (listItem));
    char *labelText;

    gtk_label_get (GTK_LABEL (children->data), &labelText);
    g_list_free (children);

    return labelText;
}
