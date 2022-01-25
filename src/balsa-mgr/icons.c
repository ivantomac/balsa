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
#include "icons.h"
#include <gtk/gtk.h>
#include <gdk/gdk.h>

#include "pixmaps/balsa-block.xpm"
#include "pixmaps/balsa-block-with-exclamation.xpm"
#include "pixmaps/breeze-block.xpm"

#include "pixmaps/sync-port.xpm"
#include "pixmaps/input-file-port.xpm"
#include "pixmaps/input-value-port.xpm"
#include "pixmaps/output-file-port.xpm"
#include "pixmaps/output-stdout-port.xpm"
#include "pixmaps/memory-component.xpm"
#include "pixmaps/undefined-port.xpm"
#include "pixmaps/procedure.xpm"
#include "pixmaps/implementation.xpm"
#include "pixmaps/builtin-lib.xpm"
#include "pixmaps/tick.xpm"
#include "pixmaps/selected.xpm"
#include "pixmaps/directory.xpm"

GdkBitmap *BalsaBlockMask;
GdkPixmap *BalsaBlockPixmap;
GdkBitmap *BalsaBlockWithExclamationMask;
GdkPixmap *BalsaBlockWithExclamationPixmap;
GdkBitmap *BreezeBlockMask;
GdkPixmap *BreezeBlockPixmap;

GdkPixmap *SyncPortPixmap;
GdkBitmap *SyncPortMask;
GdkPixmap *InputFilePortPixmap;
GdkPixmap *OutputFilePortPixmap;
GdkBitmap *InputFilePortMask;
GdkBitmap *OutputFilePortMask;
GdkPixmap *InputValuePortPixmap;
GdkPixmap *OutputStdoutPortPixmap;
GdkBitmap *InputValuePortMask;
GdkBitmap *OutputStdoutPortMask;
GdkPixmap *MemoryComponentPixmap;
GdkBitmap *MemoryComponentMask;
GdkPixmap *UndefinedPortPixmap;
GdkBitmap *UndefinedPortMask;

GdkPixmap *ProcedurePixmap;
GdkBitmap *ProcedureMask;
GdkPixmap *ImplementationPixmap;
GdkBitmap *ImplementationMask;
GdkPixmap *BuiltinLibPixmap;
GdkBitmap *BuiltinLibMask;
GdkPixmap *TickPixmap;
GdkBitmap *TickMask;
GdkPixmap *SelectedPixmap;
GdkBitmap *SelectedMask;
GdkPixmap *DirectoryPixmap;
GdkBitmap *DirectoryMask;

void InitIcons (void)
{
    GdkWindow *gdkwin = MainWindow->window;
    GtkStyle *style = gtk_widget_get_style (MainWindow);

    BalsaBlockPixmap = gdk_pixmap_create_from_xpm_d (gdkwin, &BalsaBlockMask, &style->bg[GTK_STATE_NORMAL], (gchar **) balsa_block_xpm);
    BalsaBlockWithExclamationPixmap =
      gdk_pixmap_create_from_xpm_d (gdkwin, &BalsaBlockWithExclamationMask,
      &style->bg[GTK_STATE_NORMAL], (gchar **) balsa_block_with_exclamation_xpm);
    BreezeBlockPixmap = gdk_pixmap_create_from_xpm_d (gdkwin, &BreezeBlockMask, &style->bg[GTK_STATE_NORMAL], (gchar **) breeze_block_xpm);

    SyncPortPixmap = gdk_pixmap_create_from_xpm_d (gdkwin, &SyncPortMask, &style->bg[GTK_STATE_NORMAL], (gchar **) sync_port_xpm);
    InputFilePortPixmap = gdk_pixmap_create_from_xpm_d (gdkwin, &InputFilePortMask, &style->bg[GTK_STATE_NORMAL], (gchar **) input_file_port_xpm);
    InputValuePortPixmap = gdk_pixmap_create_from_xpm_d (gdkwin, &InputValuePortMask, &style->bg[GTK_STATE_NORMAL], (gchar **) input_value_port_xpm);
    OutputFilePortPixmap = gdk_pixmap_create_from_xpm_d (gdkwin, &OutputFilePortMask, &style->bg[GTK_STATE_NORMAL], (gchar **) output_file_port_xpm);
    OutputStdoutPortPixmap =
      gdk_pixmap_create_from_xpm_d (gdkwin, &OutputStdoutPortMask, &style->bg[GTK_STATE_NORMAL], (gchar **) output_stdout_port_xpm);
    MemoryComponentPixmap =
      gdk_pixmap_create_from_xpm_d (gdkwin, &MemoryComponentMask, &style->bg[GTK_STATE_NORMAL], (gchar **) memory_component_xpm);
    UndefinedPortPixmap = gdk_pixmap_create_from_xpm_d (gdkwin, &UndefinedPortMask, &style->bg[GTK_STATE_NORMAL], (gchar **) undefined_port_xpm);

    ProcedurePixmap = gdk_pixmap_create_from_xpm_d (gdkwin, &ProcedureMask, &style->bg[GTK_STATE_NORMAL], (gchar **) procedure_xpm);
    ImplementationPixmap = gdk_pixmap_create_from_xpm_d (gdkwin, &ImplementationMask, &style->bg[GTK_STATE_NORMAL], (gchar **) implementation_xpm);
    BuiltinLibPixmap = gdk_pixmap_create_from_xpm_d (gdkwin, &BuiltinLibMask, &style->bg[GTK_STATE_NORMAL], (gchar **) builtin_lib_xpm);
    TickPixmap = gdk_pixmap_create_from_xpm_d (gdkwin, &TickMask, &style->bg[GTK_STATE_NORMAL], (gchar **) tick_xpm);
    SelectedPixmap = gdk_pixmap_create_from_xpm_d (gdkwin, &SelectedMask, &style->bg[GTK_STATE_NORMAL], (gchar **) selected_xpm);
    DirectoryPixmap = gdk_pixmap_create_from_xpm_d (gdkwin, &DirectoryMask, &style->bg[GTK_STATE_NORMAL], (gchar **) directory_xpm);
}
