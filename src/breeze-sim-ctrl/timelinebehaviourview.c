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

#include <gtk/gtk.h>
#include <math.h>

#include "interface.h"
#include "timelinebehaviourview.h"
#include "core.h"
#include "structure.h"
#include "main.h"
#include "simtrace.h"
#include "drawing.h"

static struct ViewProperties *timeLineBehaviourViewProperties = NULL;
GtkWidget *timeLineBehaviourViewWindow = NULL;

static void RedrawTimeLineBehaviourScreen (void);

/***************************/

/* Backing pixmap for drawing area */
static GtkWidget *drawingArea = NULL;
static double drawingArea_width = 0, drawingArea_height = 0;

static GdkPixmap *pixmap = NULL;

//static double zoom = 1;
//static double shift_X = 0;
//static double shift_Y = 0;

/* Create a new backing pixmap of the appropriate size */
gboolean on_TimeLineBehaviour_drawingarea_configure_event (GtkWidget * widget, GdkEventConfigure * event, gpointer user_data)
{
    //    printf("configure %p\n",widget);
    if (pixmap)
        gdk_pixmap_unref (pixmap);
    pixmap = gdk_pixmap_new (widget->window, widget->allocation.width, widget->allocation.height, -1);
    gdk_draw_rectangle (pixmap, widget->style->white_gc, TRUE, 0, 0, widget->allocation.width, widget->allocation.height);

    drawingArea_width = widget->allocation.width;
    drawingArea_height = widget->allocation.height;

    if (drawingArea)
    {
        RedrawTimeLineBehaviourScreen ();
    }

    int width = drawingArea_width - 200;
    GtkWidget *zoomspinbutton = GTK_WIDGET (gtk_object_get_data (GTK_OBJECT (timeLineBehaviourViewWindow),
        "TimeLineBehaviourZoomSpinButton"));
    int zoom = gtk_spin_button_get_value_as_int (GTK_SPIN_BUTTON (zoomspinbutton));
    GtkWidget *spinbutton = GTK_WIDGET (gtk_object_get_data (GTK_OBJECT (timeLineBehaviourViewWindow),
        "TimeLineBehaviourSpinButton"));
    GtkAdjustment *adj = gtk_spin_button_get_adjustment (GTK_SPIN_BUTTON (spinbutton));

    adj->step_increment = (float) (width * zoom);

    return TRUE;
}

gboolean on_TimeLineBehaviour_drawingarea_expose_event (GtkWidget * widget, GdkEventExpose * event, gpointer user_data)
{
    //    printf("expose %p\n",widget);
    gdk_draw_pixmap (widget->window,
      widget->style->fg_gc[GTK_WIDGET_STATE (widget)],
      pixmap, event->area.x, event->area.y, event->area.x, event->area.y, event->area.width, event->area.height);

    return FALSE;
}

/***************************/

gboolean on_TimeLineBehaviourViewWindow_delete_event (GtkWidget * widget, GdkEvent * event, gpointer user_data)
{
    GtkWidget *timeLineBehaviourToggle = GTK_WIDGET (gtk_object_get_data (GTK_OBJECT (MainWindow2), "TimeLineBehaviourToggle"));

    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (timeLineBehaviourToggle), FALSE);
    gtk_widget_hide (widget);
    return TRUE;
}

void on_TimeLineBehaviourSpinButton_changed (GtkEditable * editable, gpointer user_data)
{
    RedrawTimeLineBehaviourScreen ();
}

void on_TimeLineBehaviourZoomSpinButton_changed (GtkEditable * editable, gpointer user_data)
{
    RedrawTimeLineBehaviourScreen ();
}

gboolean on_TimeLineBehaviour_drawingarea_button_press_event (GtkWidget * widget, GdkEventButton * event, gpointer user_data)
{
    GtkWidget *spinbutton = GTK_WIDGET (gtk_object_get_data (GTK_OBJECT (timeLineBehaviourViewWindow),
        "TimeLineBehaviourSpinButton"));
    GtkWidget *zoomspinbutton = GTK_WIDGET (gtk_object_get_data (GTK_OBJECT (timeLineBehaviourViewWindow),
        "TimeLineBehaviourZoomSpinButton"));
    int startTime = gtk_spin_button_get_value_as_int (GTK_SPIN_BUTTON (spinbutton));
    int zoom = gtk_spin_button_get_value_as_int (GTK_SPIN_BUTTON (zoomspinbutton));

    //    int width = drawingArea_width - 200;

    double x = event->x;
    double time = (x - 200) * zoom + startTime;

    Core_SimTrace_ChangeTimeTo ((int) time);
    return FALSE;
}

/***************************/
// Pattern Analysis

gboolean AreSame_ChanTraceItem_Arrays (GPtrArray * trace, GPtrArray * max_trace)
{
    if (!trace && !max_trace)
        return TRUE;
    if (!trace || !max_trace)
        return FALSE;
    if (trace->len != max_trace->len)
        return FALSE;

    int i, j;

    for (i = 0; i < (int) trace->len; i++)
    {
        ChanTraceItem *traceItem1 = g_ptr_array_index (trace, i);

        for (j = 0; j < (int) max_trace->len; j++)
        {
            ChanTraceItem *traceItem2 = g_ptr_array_index (max_trace, j);

            if (traceItem1->signum == traceItem2->signum && traceItem1->edge == traceItem2->edge)
                goto ok1;
        }
        return FALSE;
      ok1:;
    }
    return TRUE;
}

void on_TimeLineBehaviourLivelockAnalysis_clicked (GtkButton * button, gpointer user_data)
{
    int max = Core_SimTrace_MaxPosition ();

    GPtrArray *max_trace = Core_SimTrace_GetTraceAtTime (max);

    if (!max_trace)
    {
        printf ("Livelock Analysis: max_trace = NULL\n");
        return;
    }

    int best_interval = 0;
    int longest_repetition = 0;
    int time;

    for (time = max - 1; time >= 0; time--)
    {
        GPtrArray *trace = Core_SimTrace_GetTraceAtTime (time);

        if (AreSame_ChanTraceItem_Arrays (trace, max_trace))
        {
            printf ("trace at %d = %d \t repeated at", max, time);
            int interval = max - time;
            int i = 2;
            gboolean same = TRUE;

            for (i = 2; same; i++)
            {
                GPtrArray *trace2 = Core_SimTrace_GetTraceAtTime (max - i * interval);

                same = AreSame_ChanTraceItem_Arrays (trace2, max_trace);
                if (same)
                {
                    printf (" %d", max - i * interval);
                    if (i > longest_repetition)
                    {
                        best_interval = interval;
                        longest_repetition = i;
                    }
                } else
                    printf (".\n");
            }
        }
    }

    if (longest_repetition < 2)
    {
        printf ("No repeated pattern found\n");
        return;
    }

    int longest_interval = longest_repetition * best_interval;

    for (time = max - 1; time - longest_interval >= 0; time--)
    {
        GPtrArray *trace1 = Core_SimTrace_GetTraceAtTime (time);
        GPtrArray *trace2 = Core_SimTrace_GetTraceAtTime (time - longest_interval);

        if (!AreSame_ChanTraceItem_Arrays (trace1, trace2))
            break;
    }

    printf ("Pattern Start = %d, Length = %d, %d repetitions\n", time - longest_interval + 1, best_interval, longest_repetition);

    // Try to follow the pattern even it is split into 2 different "threads" shifted in time
    int shift = 100;

    for (; time - longest_interval >= 0; time--)
    {
        GPtrArray *trace1 = Core_SimTrace_GetTraceAtTime (time);
        GPtrArray *trace2 = Core_SimTrace_GetTraceAtTime (time - longest_interval);
        GPtrArray *trace3 = Core_SimTrace_GetTraceAtTime (time - longest_interval + shift);

        int i, j;

        if (trace1)
            for (i = 0; i < (int) trace1->len; i++)
            {
                ChanTraceItem *traceItem1 = g_ptr_array_index (trace1, i);

                if (trace2)
                    for (j = 0; j < (int) trace2->len; j++)
                    {
                        ChanTraceItem *traceItem2 = g_ptr_array_index (trace2, j);

                        if (traceItem1->signum == traceItem2->signum && traceItem1->edge == traceItem2->edge)
                            goto ok1;
                    }
                if (trace3)
                    for (j = 0; j < (int) trace3->len; j++)
                    {
                        ChanTraceItem *traceItem2 = g_ptr_array_index (trace3, j);

                        if (traceItem1->signum == traceItem2->signum && traceItem1->edge == traceItem2->edge)
                            goto ok1;
                    }
                goto end1;
              ok1:;
            }
    }
  end1:
    printf ("Splitted pattern can be followed until time %d/%d\n", time - longest_interval + 1, time - longest_interval + shift + 1);
}

/***************************/

static void Clear (GtkWidget * widget)
{
    gdk_draw_rectangle (pixmap, widget->style->white_gc, TRUE, 0, 0, widget->allocation.width, widget->allocation.height);
}

static int RedrawDrawingArea_line;
FILE *RedrawDrawingArea_drawInFigFile = NULL;

#define XFIG_COEF(x) 10*(x)
static gboolean RedrawDrawingArea_subfct (struct Vertex *vertex)
{
    if (vertex->isGroup)
    {
        if (!vertex->isHidden)
        {
            //            printf ("vname=%s\n", vertex->name);
            RedrawDrawingArea_line++;
            if (RedrawDrawingArea_drawInFigFile)
                fprintf (RedrawDrawingArea_drawInFigFile,
                  "2 1 0 1 37 0 0 0 -1 0.0000 0 0 0 0 0 2\n\t%d %d %d %d\n",
                  XFIG_COEF (20), XFIG_COEF (12 * RedrawDrawingArea_line),
                  XFIG_COEF ((int) drawingArea_width), XFIG_COEF (12 * RedrawDrawingArea_line));
            else
                DrawColorLine_Raw (pixmap, GetColorGC (5), 20, 12 * RedrawDrawingArea_line, drawingArea_width, 12 * RedrawDrawingArea_line);
            int x = 0;
            struct Vertex *tmp = vertex;

            while (tmp->parent)
            {
                tmp = tmp->parent;
                x += 10;
            }
            if (RedrawDrawingArea_drawInFigFile)
                fprintf (RedrawDrawingArea_drawInFigFile,
                  "4 0 0 0 0 0 8 0.0000 0 105 60 %d %d %s\\001\n", XFIG_COEF (10 + x), XFIG_COEF (12 * RedrawDrawingArea_line), vertex->name);
            else
                DrawColorText_Raw (pixmap, 10, GetColorGC (0), 10 + x, 12 * RedrawDrawingArea_line, vertex->name, 0, 0, -1);

            struct VertexInfo *vertexInfo = (struct VertexInfo *) GetVertexData (vertex);

            vertexInfo->infoForTimeLineBehaviourView.lineNum = RedrawDrawingArea_line;
        }
        return vertex->areSubgroupsExpanded;
    }

    return TRUE;
}

static void RedrawDrawingArea (void)
{
    if (!RedrawDrawingArea_drawInFigFile)
        Clear (drawingArea);

    RedrawDrawingArea_line = 0;
    ForEachVertexRecursive (Structure_GetRootVertex (), RedrawDrawingArea_subfct, NULL);

    GtkWidget *spinbutton = GTK_WIDGET (gtk_object_get_data (GTK_OBJECT (timeLineBehaviourViewWindow),
        "TimeLineBehaviourSpinButton"));
    GtkWidget *zoomspinbutton = GTK_WIDGET (gtk_object_get_data (GTK_OBJECT (timeLineBehaviourViewWindow),
        "TimeLineBehaviourZoomSpinButton"));
    int startTime = gtk_spin_button_get_value_as_int (GTK_SPIN_BUTTON (spinbutton));
    int zoom = gtk_spin_button_get_value_as_int (GTK_SPIN_BUTTON (zoomspinbutton));
    int width = drawingArea_width - 200;

    // Pre-buffer
    Core_SimTrace_GetTraceAtTime (startTime + width * zoom - 1);

    int i;

    for (i = 0; i < width * zoom; i++)
    {
        GPtrArray *trace = Core_SimTrace_GetTraceAtTime (startTime + i);

        if (trace)
        {
            int j;

            for (j = 0; j < (int) trace->len; j++)
            {
                ChanTraceItem *traceItem = g_ptr_array_index (trace, j);

                struct Vertex *p1 = traceItem->edge->source->deepestVisibleParent;
                struct Vertex *p2 = traceItem->edge->dest->deepestVisibleParent;

                while (!p1->isGroup)
                    p1 = p1->parent;
                while (!p2->isGroup)
                    p2 = p2->parent;
                if (p1 == p2)
                {
                    struct VertexInfo *vertexInfo = (struct VertexInfo *) GetVertexData (p1);
                    int line = vertexInfo->infoForTimeLineBehaviourView.lineNum;
                    int pos = 1;

                    if (traceItem->edge->type & EDGE_TYPE_WIREFORK_TREE)
                        pos = 2;
                    else if (traceItem->edge->type & EDGE_TYPE_MODULE_MAINLOOP)
                        pos = 3;

                    if (line)
                    {
                        if (RedrawDrawingArea_drawInFigFile)
                            fprintf (RedrawDrawingArea_drawInFigFile,
                              "2 1 0 1 %d 0 0 0 -1 0.0000 0 0 0 0 0 2\n\t%d %d %d %d\n",
                              32 + pos, XFIG_COEF (200 + i / zoom),
                              XFIG_COEF (12 * line - 4 * pos), XFIG_COEF (200 + i / zoom), XFIG_COEF (12 * line - 4 * (pos - 1)));
                        else
                            DrawColorLine_Raw (pixmap, GetColorGC (pos),
                              200 + i / zoom, 12 * line - 4 * pos, 200 + i / zoom, 12 * line - 4 * (pos - 1));
                    }
                }
            }
        }
    }

    // Draw time position
    int time = Core_SimTrace_CurrentPosition ();

    if (time >= startTime && time <= startTime + width * zoom)
    {
        if (RedrawDrawingArea_drawInFigFile)
            fprintf (RedrawDrawingArea_drawInFigFile,
              "2 1 0 1 %d 0 0 0 -1 0.0000 0 0 0 0 0 2\n\t%d %d %d %d\n",
              32 + 5, XFIG_COEF (200 + (time - startTime) / zoom),
              XFIG_COEF (0), XFIG_COEF (200 + (time - startTime) / zoom), XFIG_COEF (12 * RedrawDrawingArea_line));
        else
            DrawColorLine_Raw (pixmap, GetColorGC (5),
              200 + (time - startTime) / zoom, 0, 200 + (time - startTime) / zoom, 12 * RedrawDrawingArea_line);
    }
}

static void RedrawTimeLineBehaviourScreen (void)
{
    if (drawingArea && pixmap)
    {
        RedrawDrawingArea_drawInFigFile = NULL;
        RedrawDrawingArea ();
        gtk_widget_draw (drawingArea, 0);
    }
}

void on_TimeLineBehaviourScreenshot_clicked (GtkButton * button, gpointer user_data)
{
    FILE *f = fopen ("screenshot.fig", "w");

    fprintf (f,
      "#FIG 3.2\n"
      "Portrait\n"
      "Center\n"
      "Metric\n"
      "A4\n"
      "100.00\n" "Single\n" "-2\n" "1200 2\n" "0 32 #000000\n" "0 33 #ff0000\n" "0 34 #00ff00\n" "0 35 #4040ff\n" "0 36 #8000c0\n" "0 37 #808080\n");

    RedrawDrawingArea_drawInFigFile = f;
    RedrawDrawingArea ();

    fclose (f);
    RedrawDrawingArea_drawInFigFile = NULL;
}

/***************************/

void TimeLineBehaviourView_Initialise (void)
{
    static gboolean initialised = FALSE;

    if (initialised)
        return;
    else
        initialised = TRUE;

    timeLineBehaviourViewWindow = create_TimeLineBehaviourViewWindow ();
    drawingArea = GTK_WIDGET (gtk_object_get_data (GTK_OBJECT (timeLineBehaviourViewWindow), "drawingarea"));
}

/***************************/

void TimeLineBehaviourView_SimTraceTimeChanged (int time)
{
    DEBUG_printf ("TimeLineBehaviourView_SimTraceTimeChanged (%d)\n", time);

    RedrawTimeLineBehaviourScreen ();
}

void TimeLineBehaviourView_SimLengthChanged (int length)
{
    DEBUG_printf ("TimeLineBehaviourView_SimLengthChanged (%d)\n", length);
}

void TimeLineBehaviourView_ExpandCollapseGroup (struct Vertex *v, int expandCollapseType)
{
    DEBUG_printf ("TimeLineBehaviourView_ExpandCollapseGroup %d\n", expandCollapseType);
}

void TimeLineBehaviourView_SelectChannel (struct Chan *chan)
{
    DEBUG_printf ("TimeLineBehaviourView_SelectChannel\n");
}

struct ViewProperties *TimeLineBehaviourView_Init (void)
{
    timeLineBehaviourViewProperties = g_new0 (struct ViewProperties, 1);

    timeLineBehaviourViewProperties->SimTraceTimeChanged = TimeLineBehaviourView_SimTraceTimeChanged;
    timeLineBehaviourViewProperties->SimLengthChanged = TimeLineBehaviourView_SimLengthChanged;
    timeLineBehaviourViewProperties->SelectChannel = TimeLineBehaviourView_SelectChannel;
    timeLineBehaviourViewProperties->ExpandCollapseGroup = (CALLBACK_voidstar_int) TimeLineBehaviourView_ExpandCollapseGroup;
    return timeLineBehaviourViewProperties;
}

void TimeLineBehaviourView_ToggleActivate (gboolean activate)
{
    if (activate)
    {
        TimeLineBehaviourView_Initialise ();
        //        TimeLineBehaviourView_UpdateAllStates ();
        gtk_widget_show (timeLineBehaviourViewWindow);
    } else
        gtk_widget_hide (timeLineBehaviourViewWindow);

    timeLineBehaviourViewProperties->enabled = activate;
}
