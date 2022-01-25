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

#ifndef MISCGTK_HEADER
#define MISCGTK_HEADER

#include <glib.h>
#include <gtk/gtk.h>

#define CTREE_HAS_NEXT_AT_SAME_LEVEL(node) (GTK_CTREE_NODE_NEXT (node) && \
	GTK_CTREE_ROW (GTK_CTREE_NODE_NEXT (node))->level == GTK_CTREE_ROW(node)->level)

/* CTreePrevNodeAtSameLevel : return the node previous to this one at the
	same level or NULL for none */
extern GtkCTreeNode *CTreePrevNodeAtSameLevel (GtkCTreeNode * node);

/* CTreeNextNodeAtSameLevel : ditto but for next node */
extern GtkCTreeNode *CTreeNextNodeAtSameLevel (GtkCTreeNode * node);

/* PushStatusBarMessage : push a formatted message onto the given status bar */
extern void PushStatusBarMessage (GtkWidget * statusBar, char *format, ...);

/* MakeListItemListFromStringList : turn a list of strings into a list of
	GtkListItems suitable for inclusion in a list with the usual functions */
extern GList *MakeListItemListFromStringList (GList * labelStrings);

/* GetListItemLabelText : get the char * text from the label inside the
	given list item in the currently favoured way. NB. don't deallocate this text */
extern char *GetListItemLabelText (GtkWidget * listItem);

#endif
