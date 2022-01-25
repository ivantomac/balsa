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

	`utils.h'

*/

#ifndef UTILS_HEADER
#define UTILS_HEADER

#include <gtk/gtk.h>

/* This creates a dialogue box with a message and a number of buttons.
 * Signal handlers can be supplied for any of the buttons.
 * NOTE: The dialogue is automatically destroyed when any button is clicked.
 * default_button specifies the default button, numbered from 1..
 * data is passed to the signal handler.

   Example usage:
     gchar *buttons[] = { "Yes", "No", "Cancel" };
     GtkSignalFunc signal_handlers[] = { on_yes, on_no, NULL };
     util_show_dialogue ("Do you want to save the current project?",
			     3, buttons, 3, signal_handlers, NULL);
 */
GtkWidget *util_create_dialogue_with_buttons (const gchar * message,
  gint nbuttons, const gchar * buttons[], gint default_button, GtkSignalFunc signal_handlers[], gpointer data);

char *GetFirstWord (char *st);
char **str_split (char *st);
char *str_replace_and_free (char *string, char *from, char *to);
gboolean strEndsWith (char *st1, char *st2);
void ConvertDottedToSlashedName (char *name);
void ConvertSlashedToDottedName (char *name);

void printfConsole_nobringtofront (const char *message);
void printfConsole (const char *message);
void printfConsole_s (const char *message, const char *s);

gboolean doesFileExist (const char *filename);
char *ListToCommaString (GList * list);
GList *CommaStringToList (char *str);
char *ExtractCommaSeparatedParam (char *str, int num);

#endif
