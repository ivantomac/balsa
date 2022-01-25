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

	`utils.c'

*/

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gtk/gtk.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "main.h"
#include "project.h"
#include "miscgtk.h"
#include "options.h"
#include "commands.h"
#include "paths.h"
#include "file.h"
#include "testopts.h"
#include "projopts.h"

#include "workspace.h"
#include "menus.h"
#include "support.h"

/* This shows a dialogue box with a message and a number of buttons.
 * Signal handlers can be supplied for any of the buttons.
 * NOTE: The dialogue is automatically destroyed when any button is clicked.
 * default_button specifies the default button, numbered from 1..
 * data is passed to the signal handler.

   Example usage:
     GtkWidget *dialogue;
     gchar *buttons[] = { "gtk-yes", "gtk-no", "gtk-cancel" };
     GtkSignalFunc signal_handlers[] = { on_yes, on_no, NULL };

     dialogue = util_create_dialogue_with_buttons ("Do you want to save the current project?", 3, buttons, 3, signal_handlers, NULL);
     gtk_widget_show (dialogue);
 */
GtkWidget *util_create_dialogue_with_buttons (const gchar * message,
  gint nbuttons, const gchar * buttons[], gint default_button, GtkSignalFunc signal_handlers[], gpointer data)
{
    GtkWidget *dialogue, *hbox, *label, *button, *bbox;
    int i;

    dialogue = gtk_dialog_new ();
    gtk_window_set_position (GTK_WINDOW (dialogue), GTK_WIN_POS_CENTER);
    gtk_container_set_border_width (GTK_CONTAINER (dialogue), 5);

    hbox = gtk_hbox_new (FALSE, 0);
    gtk_container_set_border_width (GTK_CONTAINER (hbox), 20);
    gtk_box_pack_start (GTK_BOX (GTK_DIALOG (dialogue)->vbox), hbox, TRUE, TRUE, 0);
    gtk_widget_show (hbox);

    label = gtk_label_new (message);
    gtk_box_pack_start (GTK_BOX (hbox), label, TRUE, TRUE, 0);
    gtk_widget_show (label);

    bbox = gtk_hbutton_box_new ();
    gtk_button_box_set_layout (GTK_BUTTON_BOX (bbox), GTK_BUTTONBOX_END);
    gtk_button_box_set_spacing (GTK_BUTTON_BOX (bbox), 10);
    gtk_box_pack_start (GTK_BOX (GTK_DIALOG (dialogue)->action_area), bbox, FALSE, TRUE, 0);
    gtk_widget_show (bbox);

    for (i = 0; i < nbuttons; i++)
    {
	    if (strncmp (buttons[i], "gtk", 3) == 0)
            button = gtk_button_new_from_stock (buttons[i]);
        else button = gtk_button_new_with_label (buttons[i]);
        gtk_container_add (GTK_CONTAINER (bbox), button);
        GTK_WIDGET_SET_FLAGS (button, GTK_CAN_DEFAULT);
        if (i == default_button - 1)
        {
            gtk_widget_grab_default (button);
            gtk_widget_grab_focus (button);
        }
        gtk_widget_show (button);

        if (signal_handlers[i])
            gtk_signal_connect (GTK_OBJECT (button), "clicked", signal_handlers[i], data);

        gtk_signal_connect_object (GTK_OBJECT (button), "clicked", GTK_SIGNAL_FUNC (gtk_widget_destroy), GTK_OBJECT (dialogue));
    }
    return dialogue;
}

char *GetFirstWord (char *st)
{
    char *word, *ptr;
    int wordSize;

#define NB_SEPARATORS 7
    char separators[NB_SEPARATORS] = { ' ', '\t', '\n', '(', ':', ';', ',' };
    int i;

    ptr = st - 1;
    do
    {
        ptr++;
        for (i = 0; i < NB_SEPARATORS; i++)
            if (*ptr == separators[i])
                break;
    }
    while (i != NB_SEPARATORS);

    wordSize = 0;
    do
    {
        wordSize++;
        for (i = 0; i < NB_SEPARATORS; i++)
            if (ptr[wordSize] == separators[i])
                break;
    }
    while (i == NB_SEPARATORS);

    word = (char *) g_malloc (wordSize + 1);
    strncpy (word, ptr, wordSize);
    word[wordSize] = 0;
    return word;
}

char **str_split (char *st)
{
    char **splitted = g_strsplit (st, " ", 100);
    int src = 0, dest = 0;

    for (; splitted[src]; src++)
    {
        splitted[dest] = splitted[src];
        // test if empty string
        if (splitted[src][0])
            dest++;
        // test if string=="..."
        if ((splitted[src][0] == '"') && (splitted[src][strlen (splitted[src]) - 1] == '"'))
        {
            splitted[src]++;
            splitted[src][strlen (splitted[src]) - 1] = 0;
        }
    }

    splitted[dest] = NULL;
    return splitted;
}

void printfConsole_nobringtofront (const char *message)
{
    GtkWidget *MessagesText = LookupWidget (workSpace.consoleWindow->window, "ConsoleTextBox");

    if (MessagesText)
    {
		GtkTextBuffer *buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (MessagesText));
    	GtkTextIter endOfBuffer;

    	gtk_text_buffer_get_end_iter (buffer, &endOfBuffer);
        gtk_text_buffer_insert (buffer, &endOfBuffer, message, -1);

    	gtk_text_buffer_get_end_iter (buffer, &endOfBuffer);
		gtk_text_view_scroll_to_iter (GTK_TEXT_VIEW (MessagesText), &endOfBuffer, 0.0, FALSE, 0, 0);
    } else {
        printf (message);
    }
}

void printfConsole (const char *message)
{
    // Display Console if needed
    if ((ConsoleWindowViewOptions == 0) || (ConsoleWindowViewOptions == 2))
    {
        gtk_widget_show (workSpace.consoleWindow->window);
        gdk_window_raise (workSpace.consoleWindow->window->window);
    }

    printfConsole_nobringtofront (message);
}

void printfConsole_s (const char *message, const char *s)
{
    char *newMsg = g_strdup_printf (message, s);

    printfConsole (newMsg);
    g_free (newMsg);
}

char *str_replace_and_free (char *string, char *from, char *to)
{
    char *result = NULL;
    int result_length = 0;
    int to_length = to ? strlen (to) : 0;
    char *tmp, *original_ptr_on_string = string;

    if (!string || !from)
        return NULL;
    if (!to)
        to = "";

    while ((tmp = strstr (string, from)))
    {
        int new_result_length = result_length + tmp - string + to_length;

        result = (char *) g_realloc (result, new_result_length + 1);
        strncpy (result + result_length, string, tmp - string);
        result_length += tmp - string;
        strcpy (result + result_length, to);
        result_length += to_length;

        string = tmp + strlen (from);
    }

    if (result_length)
    {
        result = (char *) g_realloc (result, result_length + strlen (string) + 1);
        strcpy (result + result_length, string);
        g_free (original_ptr_on_string);
        return result;
    } else
        return string;
}

gboolean strEndsWith (char *st1, char *st2)
{
    if (strlen (st1) < strlen (st2))
        return FALSE;
    else
        return (!strcmp (st1 + strlen (st1) - strlen (st2), st2));
}

void ConvertDottedToSlashedName (char *name)
{
    for (; *name; name++)
        if (*name == '.')
            *name = '/';
}

void ConvertSlashedToDottedName (char *name)
{
    for (; *name; name++)
        if (*name == '/')
            *name = '.';
}

gboolean doesFileExist (const char *filename)
{
    FILE *f = fopen (filename, "r");

    if (!f)
        return FALSE;
    fclose (f);
    return TRUE;
}

char *ListToCommaString (GList * list)
{
    if (!list)
        return g_strdup ("");

    char *str = list->data;

    if (!strchr (str, ','))
        str = g_strdup (str);
    else
        str = g_strdup_printf ("\"%s\"", str);

    for (list = list->next; list; list = list->next)
    {
        char *txt = list->data;
        char *tmp;

        if (!strchr (txt, ','))
            tmp = g_strdup_printf ("%s,%s", str, txt);
        else
            tmp = g_strdup_printf ("%s,\"%s\"", str, txt);
        g_free (str);
        str = tmp;
    }

    return str;
}

GList *CommaStringToList (char *str)
{
    if (!str)
        return NULL;

    GList *list = NULL;
    char *ptr1 = str;
    char *ptr2 = str;
    int size;
    char *tmp;

  again:
    if (*ptr2 == '\"')
    {
        do
        {
            ptr2 = strchr (ptr2 + 1, '\"');
        }
        while (ptr2 && *(ptr2 - 1) == '\\');
        if (!ptr2)
            return list;
        ptr1++;
        size = ptr2 - ptr1;
        ptr2++;
    } else
    {
        ptr2 = strchr (ptr2, ',');
        if (ptr2)
            size = ptr2 - ptr1;
        else
        {
            size = strlen (ptr1);
            ptr2 = ptr1 + size;
        }
    }

    tmp = g_malloc (size + 1);
    strncpy (tmp, ptr1, size);
    tmp[size] = 0;
    list = g_list_append (list, tmp);

    if (*ptr2)
    {
        ptr2++;
        ptr1 = ptr2;
        goto again;
    }

    return list;
}

char *ExtractCommaSeparatedParam (char *str, int num)
{
    GList *list = CommaStringToList (str);
    GList *ptr = g_list_nth (list, num);

    if (ptr)
        return ptr->data;
    else
        return g_strdup ("");
}
