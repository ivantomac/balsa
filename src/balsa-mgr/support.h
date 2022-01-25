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

	`support.h'
	Glade support functions
*/

#ifndef SUPPORT_HEADER
#define SUPPORT_HEADER

#include <gtk/gtk.h>

/* CreateWindow : create a named window from the Glade interface description file */
extern GtkWidget *CreateWindow (const char *windowName);

/*
 * This function returns a widget in a component created by Glade.
 * Call it with the toplevel widget in the component (i.e. a window/dialog),
 * or alternatively any widget in the component, and the name of the widget
 * you want returned.
 */
#define LookupWidget lookup_widget
GtkWidget* lookup_widget (GtkWidget *widget, const gchar *widget_name);

/* Use this function to set the directory containing installed pixmaps. */
void add_pixmap_directory (const gchar *directory);

/*
 * Private Functions.
 */

/* This is used to create the pixmaps used in the interface. */
GtkWidget* create_pixmap (GtkWidget *widget, const gchar *filename);

/* This is used to create the pixbufs used in the interface. */
GdkPixbuf* create_pixbuf (const gchar *filename);

/* This is used to set ATK action descriptions. */
void glade_set_atk_action_description (AtkAction *action, const gchar *action_name, const gchar *description);

#endif
