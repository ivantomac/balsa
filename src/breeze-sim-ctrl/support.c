/*    
    Copyright (C) 1995-2009 School of Computer Science
    The University of Manchester, Oxford Road, Manchester, M13 9PL, UK
    
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
    
    `support.c'
    Support for GTK+, originally from Glade, hand modded.

*/

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#include <gtk/gtk.h>

#include "support.h"

#define BUILTIN_PIXMAPS

#ifdef BUILTIN_PIXMAPS
#include "pixmaps/traceallchans.xpm"
#include "pixmaps/traceportsonly.xpm"
#include "pixmaps/tracenamedportsonly.xpm"
#include "pixmaps/callgraph_16.xpm"
#include "pixmaps/calllist_16.xpm"
#include "pixmaps/sourcecode_16.xpm"
#include "pixmaps/gtkwave_16.xpm"
#include "pixmaps/play.xpm"
#include "pixmaps/stop.xpm"
#include "pixmaps/pause.xpm"
#include "pixmaps/stock_zoom_fit.xpm"
#include "pixmaps/stock_zoom_in.xpm"
#include "pixmaps/stock_zoom_out.xpm"
#include "pixmaps/showchannames.xpm"
#include "pixmaps/showchanvalues.xpm"
#include "pixmaps/screenshot.xpm"
#include "pixmaps/gnome-warning.xpm"
#endif

#ifdef BUILTIN_PIXMAPS
/* Hash of builtin pixmap names to GtkWidgets for those pixmaps */
static GHashTable *builtin_pixmaps = NULL;
#endif

/* This is an internally used function to check if a pixmap file exists. */
static gchar *check_file_exists (const gchar * directory, const gchar * filename);

static GtkWidget *create_pixmap_from_data (GtkWidget * widget, char **data);

/* This is an internally used function to create pixmaps. */
static GtkWidget *create_dummy_pixmap (GtkWidget * widget);

GtkWidget *lookup_widget (GtkWidget * widget, const gchar * widget_name)
{
    GtkWidget *parent, *found_widget;

    for (;;)
    {
        if (GTK_IS_MENU (widget))
            parent = gtk_menu_get_attach_widget (GTK_MENU (widget));
        else
            parent = widget->parent;
        if (parent == NULL)
            break;
        widget = parent;
    }

    found_widget = (GtkWidget *) gtk_object_get_data (GTK_OBJECT (widget), widget_name);
    if (!found_widget)
        g_warning ("Widget not found: %s", widget_name);
    return found_widget;
}

/* This is a dummy pixmap we use when a pixmap can't be found. */
static char *dummy_pixmap_xpm[] = {
/* columns rows colors chars-per-pixel */
    "1 1 1 1",
    "  c None",
/* pixels */
    " "
};

static GtkWidget *create_pixmap_from_data (GtkWidget * widget, char **data)
{
    GdkColormap *colormap;
    GdkPixmap *gdkpixmap;
    GdkBitmap *mask;
    GtkWidget *pixmap;

    colormap = gtk_widget_get_colormap (widget);
    gdkpixmap = gdk_pixmap_colormap_create_from_xpm_d (NULL, colormap, &mask, NULL, data);
    if (gdkpixmap == NULL)
        g_error ("Couldn't create replacement pixmap.");
    pixmap = gtk_pixmap_new (gdkpixmap, mask);
    gdk_pixmap_unref (gdkpixmap);
    gdk_bitmap_unref (mask);
    return pixmap;
}

/* This is an internally used function to create pixmaps. */
static GtkWidget *create_dummy_pixmap (GtkWidget * widget)
{
    return create_pixmap_from_data (widget, dummy_pixmap_xpm);
}

static GList *pixmaps_directories = NULL;

#ifdef BUILTIN_PIXMAPS
/* add_builtin_pixmap : add a pixmap to the internal pixmap hash for a pixmap
	built in to the binary rather than loaded from a file */
static void add_builtin_pixmap (const char *name, GtkWidget * widget, char **data)
{
    g_hash_table_insert (builtin_pixmaps, (gpointer) name, create_pixmap_from_data (widget, data));
}
#endif

/* breezesimctrl_register_pixmaps : add the pixmap directory to the pixmap path and register
	builtin pixmaps */
void breezesimctrl_register_pixmaps (GtkWidget * widget)
{
#ifdef BUILTIN_PIXMAPS
    builtin_pixmaps = g_hash_table_new (g_str_hash, g_str_equal);

    add_builtin_pixmap ("traceallchans.xpm", widget, traceallchans_xpm);
    add_builtin_pixmap ("traceportsonly.xpm", widget, traceportsonly_xpm);
    add_builtin_pixmap ("tracenamedportsonly.xpm", widget, tracenamedportsonly_xpm);
    add_builtin_pixmap ("callgraph_16.xpm", widget, callgraph_16_xpm);
    add_builtin_pixmap ("calllist_16.xpm", widget, calllist_16_xpm);
    add_builtin_pixmap ("sourcecode_16.xpm", widget, sourcecode_16_xpm);
    add_builtin_pixmap ("gtkwave_16.xpm", widget, gtkwave_16_xpm);
    add_builtin_pixmap ("play.xpm", widget, play_xpm);
    add_builtin_pixmap ("stop.xpm", widget, stop_xpm);
    add_builtin_pixmap ("pause.xpm", widget, pause_xpm);
    add_builtin_pixmap ("stock_zoom_fit.xpm", widget, stock_zoom_fit_xpm);
    add_builtin_pixmap ("stock_zoom_in.xpm", widget, stock_zoom_in_xpm);
    add_builtin_pixmap ("stock_zoom_out.xpm", widget, stock_zoom_out_xpm);
    add_builtin_pixmap ("showchannames.xpm", widget, showchannames_xpm);
    add_builtin_pixmap ("showchanvalues.xpm", widget, showchanvalues_xpm);
    add_builtin_pixmap ("screenshot.xpm", widget, screenshot_xpm);
    add_builtin_pixmap ("gnome-warning.xpm", widget, gnome_warning_xpm);
#endif
}

/* This is an internally used function to create pixmaps. */
GtkWidget *create_pixmap (GtkWidget * widget, const gchar * filename)
{
    gchar *found_filename = NULL;
    GdkColormap *colormap;
    GdkPixmap *gdkpixmap;
    GdkBitmap *mask;
    GtkWidget *pixmap;
    GList *elem;

    if (!filename || !filename[0])
        return create_dummy_pixmap (widget);

#ifdef BUILTIN_PIXMAPS
    pixmap = (GtkWidget *) g_hash_table_lookup (builtin_pixmaps, filename);

    if (pixmap)
        return pixmap;
#endif

    /* We first try any pixmaps directories set by the application. */
    elem = pixmaps_directories;
    while (elem)
    {
        found_filename = check_file_exists ((gchar *) elem->data, filename);
        if (found_filename)
            break;
        elem = elem->next;
    }

    /* If we haven't found the pixmap, try the source directory. */
    if (!found_filename)
    {
        found_filename = check_file_exists ("../pixmaps", filename);
    }

    if (!found_filename)
    {
        g_warning ("Couldn't find pixmap file: %s", filename);
        return create_dummy_pixmap (widget);
    }

    colormap = gtk_widget_get_colormap (widget);
    gdkpixmap = gdk_pixmap_colormap_create_from_xpm (NULL, colormap, &mask, NULL, found_filename);
    if (gdkpixmap == NULL)
    {
        g_warning ("Error loading pixmap file: %s", found_filename);
        g_free (found_filename);
        return create_dummy_pixmap (widget);
    }
    g_free (found_filename);
    pixmap = gtk_pixmap_new (gdkpixmap, mask);
    gdk_pixmap_unref (gdkpixmap);
    gdk_bitmap_unref (mask);
    return pixmap;
}

/* This is an internally used function to check if a pixmap file exists. */
static gchar *check_file_exists (const gchar * directory, const gchar * filename)
{
    gchar *full_filename;
    struct stat s;
    gint status;

    full_filename = (gchar *) g_malloc (strlen (directory) + 1 + strlen (filename) + 1);
    strcpy (full_filename, directory);
    strcat (full_filename, G_DIR_SEPARATOR_S);
    strcat (full_filename, filename);

    status = stat (full_filename, &s);
    if (status == 0 && S_ISREG (s.st_mode))
        return full_filename;
    g_free (full_filename);
    return NULL;
}

/* breezesimctrl_get_option_menu_active_data : get a named data element from the active element of
	the given option menu, returns NULL on any errors */
gpointer breezesimctrl_get_option_menu_active_data (GtkOptionMenu * option_menu, const char *data_element_name)
{
    GtkWidget *menu, *selected_item;

    menu = gtk_option_menu_get_menu (option_menu);
    if (!menu)
        return NULL;
    selected_item = gtk_menu_get_active (GTK_MENU (menu));
    if (!selected_item)
        return NULL;

    return gtk_object_get_data (GTK_OBJECT (selected_item), data_element_name);
}

/* breezesimctrl_get_option_menu_index : get the index of the active element in the given option menu */
guint breezesimctrl_get_option_menu_index (GtkOptionMenu * option_menu)
{
    GtkWidget *menu = gtk_option_menu_get_menu (option_menu);
    GtkWidget *active_item = gtk_menu_get_active (GTK_MENU (menu));

    return (guint) g_list_index (GTK_MENU_SHELL (menu)->children, active_item);
}
