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

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <gtk/gtk.h>

#include "drawing.h"
#include "main.h"
#include "graphviewGUI.h"

GdkGC *GetColorGC (int colorNum)
{
    static GdkGC *color_GC[9];
    static int initialised = 0;
    GtkWidget *widget = MainWindow2;

    if (!initialised)
    {
        GdkColor color;

        initialised = 1;

        color_GC[0] = gdk_gc_new (widget->window);
        color.red = 0;
        color.green = 0;
        color.blue = 0;
        gdk_color_alloc (gdk_colormap_get_system (), &color);
        gdk_gc_set_foreground (color_GC[0], &color);

        color_GC[1] = gdk_gc_new (widget->window);
        color.red = 0xFFFF;
        color.green = 0;
        color.blue = 0;
        gdk_color_alloc (gdk_colormap_get_system (), &color);
        gdk_gc_set_foreground (color_GC[1], &color);

        color_GC[2] = gdk_gc_new (widget->window);
        color.red = 0x0;
        color.green = 0xFFFF;
        color.blue = 0x0;
        gdk_color_alloc (gdk_colormap_get_system (), &color);
        gdk_gc_set_foreground (color_GC[2], &color);

        color_GC[3] = gdk_gc_new (widget->window);
        color.red = 0x4000;
        color.green = 0x4000;
        color.blue = 0xFFFF;
        gdk_color_alloc (gdk_colormap_get_system (), &color);
        gdk_gc_set_foreground (color_GC[3], &color);

        color_GC[4] = gdk_gc_new (widget->window);
        color.red = 0x8000;
        color.green = 0;
        color.blue = 0xC000;
        gdk_color_alloc (gdk_colormap_get_system (), &color);
        gdk_gc_set_foreground (color_GC[4], &color);

        color_GC[5] = gdk_gc_new (widget->window);
        color.red = 0x8000;
        color.green = 0x8000;
        color.blue = 0x8000;
        gdk_color_alloc (gdk_colormap_get_system (), &color);
        gdk_gc_set_foreground (color_GC[5], &color);

        color_GC[6] = gdk_gc_new (widget->window);
        color.red = 0xffff;
        color.green = 0xffff;
        color.blue = 0x8000;
        gdk_color_alloc (gdk_colormap_get_system (), &color);
        gdk_gc_set_foreground (color_GC[6], &color);

        color_GC[7] = gdk_gc_new (widget->window);
        color.red = 0xb800;
        color.green = 0xb800;
        color.blue = 0xb800;
        gdk_color_alloc (gdk_colormap_get_system (), &color);
        gdk_gc_set_foreground (color_GC[7], &color);

        color_GC[8] = gdk_gc_new (widget->window);
        color.red = 0xf000;
        color.green = 0xf000;
        color.blue = 0xf000;
        gdk_color_alloc (gdk_colormap_get_system (), &color);
        gdk_gc_set_foreground (color_GC[8], &color);
    }

    if (colorNum >= 0 && colorNum < 9)
        return color_GC[colorNum];
    else
        return color_GC[0];
}

void DrawColorEllipse (GdkGC * gc, double x, double y, double sizeX, double sizeY)
{
    x = (x + shift_X) * zoom;
    y = (y + shift_Y[0]) * zoom;
    sizeX *= zoom;
    sizeY *= zoom;

    if (sizeX < 1000000 && sizeY < 1000000 && x < 1000000 && y < 1000000 && x > -1000000 && y > -1000000)
        gdk_draw_arc (pixmap, gc, FALSE, x - sizeX, y - sizeY, 2 * sizeX, 2 * sizeY, 0, 360 * 64);
}

void DrawEllipse (double x, double y, double sizeX, double sizeY)
{
    DrawColorEllipse (drawingArea[0]->style->black_gc, x, y, sizeX, sizeY);
}

void DrawColorFilledEllipse (GdkGC * gc, double x, double y, double sizeX, double sizeY)
{
    x = (x + shift_X) * zoom;
    y = (y + shift_Y[0]) * zoom;
    sizeX *= zoom;
    sizeY *= zoom;

    if (sizeX < 1000000 && sizeY < 1000000 && x < 1000000 && y < 1000000 && x > -1000000 && y > -1000000)
        gdk_draw_arc (pixmap, gc, TRUE, x - sizeX, y - sizeY, 2 * sizeX, 2 * sizeY, 0, 360 * 64);
}

void DrawFilledEllipse (double x, double y, double sizeX, double sizeY)
{
    DrawColorFilledEllipse (drawingArea[0]->style->black_gc, x, y, sizeX, sizeY);
}

void DrawColorRectangle (GdkGC * gc, double x, double y, double sizeX, double sizeY)
{
    x = (x + shift_X) * zoom;
    y = (y + shift_Y[0]) * zoom;
    sizeX *= zoom;
    sizeY *= zoom;

    gdk_draw_rectangle (pixmap, gc, FALSE, x, y, sizeX, sizeY);
}

void DrawRectangle (double x, double y, double sizeX, double sizeY)
{
    DrawColorRectangle (drawingArea[0]->style->black_gc, x, y, sizeX, sizeY);
}

void DrawColorLine_Raw (GdkPixmap * pixmap, GdkGC * gc, double x1, double y1, double x2, double y2)
{
    gdk_draw_line (pixmap, gc, x1, y1, x2, y2);
}

void DrawColorLine (GdkGC * gc, double x1, double y1, double x2, double y2)
{
    x1 = (x1 + shift_X) * zoom;
    y1 = (y1 + shift_Y[0]) * zoom;
    x2 = (x2 + shift_X) * zoom;
    y2 = (y2 + shift_Y[0]) * zoom;

    if (x1 < 1000000 && y1 < 1000000 && x1 > -1000000 && y1 > -1000000 && x2 < 1000000 && y2 < 1000000 && x2 > -1000000 && y2 > -1000000)
        DrawColorLine_Raw (pixmap, gc, x1, y1, x2, y2);
}

void PrepareNormals (double x1, double y1, double x2, double y2, double *length, double *dxnorm, double *dynorm)
{
    double dx = x2 - x1;
    double dy = y2 - y1;

    double sqlength = dx * dx + dy * dy;

    if (sqlength)
    {
        *length = sqrt (sqlength);
        *dxnorm = dx / (*length);
        *dynorm = dy / (*length);
    } else
    {
        *length = sqrt (2);
        *dxnorm = 1;
        *dynorm = 1;
    }
}

void DrawThickColorLine (GdkGC * gc, double x1, double y1, double x2, double y2, int thickness)
{
    x1 = (x1 + shift_X) * zoom;
    y1 = (y1 + shift_Y[0]) * zoom;
    x2 = (x2 + shift_X) * zoom;
    y2 = (y2 + shift_Y[0]) * zoom;

    if (x1 < 1000000 && y1 < 1000000 && x1 > -1000000 && y1 > -1000000 && x2 < 1000000 && y2 < 1000000 && x2 > -1000000 && y2 > -1000000)
    {
        double length, dxnorm, dynorm;

        PrepareNormals (x1, y1, x2, y2, &length, &dxnorm, &dynorm);
        gdk_gc_set_line_attributes (gc, thickness, GDK_LINE_SOLID, GDK_CAP_BUTT, GDK_JOIN_MITER);
        gdk_draw_line (pixmap, gc, x1, y1, x2, y2);
        gdk_gc_set_line_attributes (gc, 0, GDK_LINE_SOLID, GDK_CAP_BUTT, GDK_JOIN_MITER);
    }
}

void DrawColorArrow (GdkGC * gc, double x1, double y1, double x2, double y2)
{
    x1 = (x1 + shift_X) * zoom;
    y1 = (y1 + shift_Y[0]) * zoom;
    x2 = (x2 + shift_X) * zoom;
    y2 = (y2 + shift_Y[0]) * zoom;
    //    int size = abs(x2-x1)+abs(y2-y1);
    double arrowsize = 8 * zoom;
    double arrowdist = 0 * zoom;

    if (x1 < 1000000 && y1 < 1000000 && x1 > -1000000 && y1 > -1000000 && x2 < 1000000 && y2 < 1000000 && x2 > -1000000 && y2 > -1000000)
    {
        double length, dxnorm, dynorm;

        PrepareNormals (x1, y1, x2, y2, &length, &dxnorm, &dynorm);
        double x1b = x1 + arrowdist * dxnorm;
        double y1b = y1 + arrowdist * dynorm;
        double x2b = x2 - arrowdist * dxnorm;
        double y2b = y2 - arrowdist * dynorm;

        gdk_draw_line (pixmap, gc, x1b, y1b, x2b, y2b);
        gdk_draw_line (pixmap, gc, x2b, y2b, x2b - arrowsize * (dxnorm - dynorm), y2b - arrowsize * (dxnorm + dynorm));
        gdk_draw_line (pixmap, gc, x2b, y2b, x2b - arrowsize * (dxnorm + dynorm), y2b + arrowsize * (dxnorm - dynorm));
    }
}

void DrawLine (double x1, double y1, double x2, double y2)
{
    DrawColorLine (drawingArea[0]->style->black_gc, x1, y1, x2, y2);
}

static GdkFont *gdkfont[41];
GdkFont *GetFontForSize (int fontsize)
{
    if (fontsize > 40)
        fontsize = 40;
    int increment = +1;

    if (fontsize < 2)
        return NULL;

  again:
    if (gdkfont[fontsize] == NULL)
    {
        if (fontsize != 13)
        {
            char *fontname = g_strdup_printf ("*-helvetica-bold-r-normal-*-*-%d0-*", fontsize);

            // char *fontname = g_strdup_printf ("-adobe-helvetica-bold-r-normal--%d-120-75-75-p-70-iso8859-1", fontsize);
            gdkfont[fontsize] = gdk_font_load (fontname);
            g_free (fontname);
        }
        if (gdkfont[fontsize] == NULL)
            gdkfont[fontsize] = (GdkFont *) - 1;
    }
    if (gdkfont[fontsize] == (GdkFont *) - 1)
    {
        fontsize += increment;
        if (fontsize > 40)
        {
            fontsize = 39;
            increment = -1;
        }
        if (fontsize == 0)
            return gdkfont[fontsize];
        goto again;
    }

    return gdkfont[fontsize];
}

void DrawColorText_Raw (GdkPixmap * pixmap, int fontsize, GdkGC * colorGC, gdouble x, gdouble y, char *text, int alignX, int alignY, int textLength)
{
    if (!text)
        return;

    if (textLength == -1)
        textLength = strlen (text);

    GdkFont *font = GetFontForSize (fontsize);

    if (!font)
        return;

    int sizeX = gdk_text_width (font, text, textLength);
    int sizeY = gdk_text_height (font, text, textLength);

    switch (alignX)
    {
    case 0:
        break;
    case 1:
        x -= sizeX / 2;
        break;
    case 2:
        x -= sizeX;
        break;
    }
    switch (alignY)
    {
    case 0:
        break;
    case 1:
        y += sizeY / 2;
        break;
    case 2:
        y += sizeY;
        break;
    case 2 + 1:
        y += sizeY + 1;
        break;
    }
    gdk_draw_text (pixmap, font, colorGC, x, y, text, textLength);
}

void DrawColorText2 (GdkGC * colorGC, gdouble x, gdouble y, char *text, int alignX, int alignY, int textLength, int fontsize)
{
    x = (x + shift_X) * zoom;
    y = (y + shift_Y[0]) * zoom;

    int fontsize2 = fontsize * zoom;

    DrawColorText_Raw (pixmap, fontsize2, colorGC, x, y, text, alignX, alignY, textLength);
}

void DrawColorText (GdkGC * colorGC, gdouble x, gdouble y, char *text, int alignX, int alignY, int fontsize)
{
    DrawColorText2 (colorGC, x, y, text, alignX, alignY, strlen (text), fontsize);
}

void DrawText (gdouble x, gdouble y, char *text, int alignX, int alignY, int fontsize)
{
    DrawColorText (GetColorGC (-1), x, y, text, alignX, alignY, fontsize);
}

void DrawMultiLineText (gdouble x, gdouble y, char *text, int alignX, int alignY, int lineWidth, int fontsize)
{
    int fontsize2 = fontsize * zoom;
    GdkFont *font = GetFontForSize (fontsize2);

    if (!font)
        return;

    int textLength = strlen (text);
    double sizeY = ((double) (font->ascent + font->descent)) / zoom;
    double sizeY0 = (gdk_text_height (font, text, textLength)) / zoom;
    int nbLines = textLength / lineWidth;
    int remainder = textLength - nbLines * lineWidth;
    int i;

    for (i = 0; i < nbLines; i++)
        DrawColorText2 (GetColorGC (-1), x, sizeY0 + y + i * sizeY, &text[i * lineWidth], alignX, 0 /*alignY */ ,
          lineWidth, fontsize);

    DrawColorText2 (GetColorGC (-1), x, sizeY0 + y + nbLines * sizeY, &text[nbLines * lineWidth], alignX, 0 /*alignY */ ,
      remainder, fontsize);
}

void DrawColorMultiLineText2 (GdkGC * gc, gdouble x, gdouble y, char *text, int alignX, int alignY, int fontsize)
{
    int fontsize2 = fontsize * zoom;
    GdkFont *font = GetFontForSize (fontsize2);

    if (!font)
        return;

    double sizeY = ((double) (font->ascent + font->descent)) / zoom;
    double sizeY0 = (gdk_text_height (font, text, strlen (text))) / zoom;
    int lineNum = 0;
    char *ptr = text;
    char *ptr2;

    do
    {
        ptr2 = strchr (ptr, '\n');
        int length = (ptr2 != NULL) ? ((int) (ptr2 - ptr)) : (int) strlen (ptr);

        DrawColorText2 (gc, x, sizeY0 + y + lineNum * sizeY, ptr, alignX, 0 /*alignY */ , length, fontsize);
        lineNum++;
        ptr = ptr2 + 1;
    }
    while (ptr2 && *ptr);
}

void DrawMultiLineText2 (gdouble x, gdouble y, char *text, int alignX, int alignY, int fontsize)
{
    DrawColorMultiLineText2 (drawingArea[0]->style->black_gc, x, y, text, alignX, alignY, fontsize);
}

void DrawWireCount (double x1, double y1, double x2, double y2, int wireCount)
{
    x1 = (x1 + shift_X) * zoom;
    y1 = (y1 + shift_Y[0]) * zoom;
    x2 = (x2 + shift_X) * zoom;
    y2 = (y2 + shift_Y[0]) * zoom;
    double slashsize = 4 * zoom;

    if (x1 < 1000000 && y1 < 1000000 && x1 > -1000000 && y1 > -1000000 && x2 < 1000000 && y2 < 1000000 && x2 > -1000000 && y2 > -1000000)
    {
        double length, dxnorm, dynorm;

        PrepareNormals (x1, y1, x2, y2, &length, &dxnorm, &dynorm);
        double x1b = x1 + (length / 4) * dxnorm;
        double y1b = y1 + (length / 4) * dynorm;
        double dx = slashsize * (dxnorm - 2 * dynorm);
        double dy = slashsize * (2 * dxnorm + dynorm);

        gdk_draw_line (pixmap, drawingArea[0]->style->black_gc, x1b - dx, y1b - dy, x1b + dx, y1b + dy);

        char *text = g_strdup_printf ("%d", wireCount);

        //  DrawText ( x1b+dx, y1b+dy, text, 0, 0);
        DrawColorText_Raw (pixmap, 8 * zoom, drawingArea[0]->style->black_gc, x1b - 2 * dx, y1b - 2 * dy, text, 0, 2, -1);
        g_free (text);
    }
}
