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

#ifndef GRAPHVIEWGUI_H
#define GRAPHVIEWGUI_H

#include <gtk/gtk.h>

extern GtkWidget *drawingArea[3];
extern GdkPixmap *pixmap;
extern double zoom;
extern double shift_X;
extern double shift_Y[3];

void GraphViewGUI_InitialiseGraph (void);
void GraphViewGUI_UpdateAllStates (void);

void ZoomFit (void);
void RedrawScreen (void);

#endif
