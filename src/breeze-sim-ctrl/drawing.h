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

#ifndef DRAWING_H
#define DRAWING_H

#define SQ(x) ((x)*(x))

GdkGC *GetColorGC (int colorNum);
void DrawColorEllipse (GdkGC * gc, double x, double y, double sizeX, double sizeY);
void DrawEllipse (double x, double y, double sizeX, double sizeY);
void DrawColorFilledEllipse (GdkGC * gc, double x, double y, double sizeX, double sizeY);
void DrawFilledEllipse (double x, double y, double sizeX, double sizeY);
void DrawColorRectangle (GdkGC * gc, double x, double y, double sizeX, double sizeY);
void DrawRectangle (double x, double y, double sizeX, double sizeY);
void DrawColorLine (GdkGC * gc, double x1, double y1, double x2, double y2);
void DrawThickColorLine (GdkGC * gc, double x1, double y1, double x2, double y2, int thickness);
void DrawColorArrow (GdkGC * gc, double x1, double y1, double x2, double y2);
void DrawLine (double x1, double y1, double x2, double y2);
void DrawColorText (GdkGC * colorGC, gdouble x, gdouble y, char *text, int alignX, int alignY, int fontsize);
void DrawText (gdouble x, gdouble y, char *text, int alignX, int alignY, int fontsize);
void DrawMultiLineText (gdouble x, gdouble y, char *text, int alignX, int alignY, int lineWidth, int fontsize);
void DrawColorMultiLineText2 (GdkGC * gc, gdouble x, gdouble y, char *text, int alignX, int alignY, int fontsize);
void DrawMultiLineText2 (gdouble x, gdouble y, char *text, int alignX, int alignY, int fontsize);
void DrawWireCount (double x1, double y1, double x2, double y2, int wireCount);

void DrawColorText_Raw (GdkPixmap * pixmap, int fontsize, GdkGC * colorGC, gdouble x, gdouble y, char *text, int alignX, int alignY, int textLength);
void DrawColorLine_Raw (GdkPixmap * pixmap, GdkGC * gc, double x1, double y1, double x2, double y2);

void PrepareNormals (double x1, double y1, double x2, double y2, double *length, double *dxnorm, double *dynorm);

#endif
