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
#include <gdk/gdkkeysyms.h>

#include "interface.h"

#include "graphviewGUI.h"
#include "libspring.h"
#include "main.h"
#include "mainwindow2.h"
#include "structure.h"
#include "drawing.h"
#include "simtrace.h"
#include "sourceview.h"
#include "timelinebehaviourview.h"
#include "partitioning.h"

#include <breeze/libbreeze.h>
#include "core.h"

/********************************************/
/****************  NEW STUFF ****************/
/********************************************/

gboolean displayOnlyWireforkTree = FALSE;
gboolean isShowChannelNamesToggled = FALSE;
gboolean isShowChannelValuesToggled = TRUE;

/* Backing pixmap for drawing area */
int nbVisibleDrawingAreas = 1;
GtkWidget *drawingArea[3] = { NULL, NULL, NULL };
double drawingArea_width = 0, drawingArea_height = 0;

GdkPixmap *pixmap = NULL;
gboolean configureJustDone;

double zoom = 1;
double shift_X = 0;
double shift_Y[3] = { 0, 0, 0 };

int ChannelColorLimit1 = 3, ChannelColorLimit2 = 10;
int DrawEdge_PassNum = 0;

void InitialiseGraphPositions (void);
void RedrawPixmap (void);

void GraphViewGUI_InitialiseGraph (void)
{
    static gboolean initialised = FALSE;

    if (initialised)
        return;
    else
        initialised = TRUE;

    //    struct BreezePart *breezePart = Structure_GetFlattenedBreezePart ();
    drawingArea[0] = GTK_WIDGET (gtk_object_get_data (GTK_OBJECT (MainWindow2), "drawingarea1"));
    drawingArea[1] = GTK_WIDGET (gtk_object_get_data (GTK_OBJECT (MainWindow2), "drawingarea2"));
    drawingArea[2] = GTK_WIDGET (gtk_object_get_data (GTK_OBJECT (MainWindow2), "drawingarea3"));

    ChannelColorLimit1 =
      gtk_spin_button_get_value_as_int (GTK_SPIN_BUTTON (gtk_object_get_data (GTK_OBJECT (MainWindow2), "ChannelColorLimit1SpinButton")));
    ChannelColorLimit2 =
      gtk_spin_button_get_value_as_int (GTK_SPIN_BUTTON (gtk_object_get_data (GTK_OBJECT (MainWindow2), "ChannelColorLimit2SpinButton")));

    InitialiseGraphPositions ();
}

void GraphViewGUI_UpdateAllStates (void)
{
}

/********************************************/
/****************  OLD STUFF ****************/
/********************************************/

/* Create a new backing pixmap of the appropriate size */
gboolean on_drawingarea_configure_event (GtkWidget * widget, GdkEventConfigure * event, gpointer user_data)
{
    // printf("configure %p\n",widget);

    if (widget == drawingArea[0])
    {
        if (pixmap)
            gdk_pixmap_unref (pixmap);
        pixmap = gdk_pixmap_new (widget->window, widget->allocation.width, nbVisibleDrawingAreas * widget->allocation.height, -1);
        gdk_draw_rectangle (pixmap, widget->style->white_gc, TRUE, 0, 0, widget->allocation.width, nbVisibleDrawingAreas * widget->allocation.height);

        drawingArea_width = widget->allocation.width;
        drawingArea_height = widget->allocation.height;
    }

    configureJustDone = TRUE;
    return TRUE;
}

gboolean on_drawingarea1_expose_event (GtkWidget * widget, GdkEventExpose * event, gpointer user_data)
{
    // printf("expose %p\n",widget);

    long num = (long) user_data;

    if (configureJustDone)
    {
        configureJustDone = 0;

        if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (gtk_object_get_data (GTK_OBJECT (MainWindow2), "ZoomFit"))))
            ZoomFit ();

        RedrawPixmap ();
    }

    gdk_draw_pixmap (widget->window,
      widget->style->fg_gc[GTK_WIDGET_STATE (widget)],
      pixmap, event->area.x, event->area.y + num * drawingArea_height, event->area.x, event->area.y, event->area.width, event->area.height);

    return FALSE;
}

/******************************************************************************/

void SetVisibleWindow (double x1, double y1, double x2, double y2)
{
    if (drawingArea_width < 2 || drawingArea_height < 2)
        return;

    int i;

    zoom = MIN (drawingArea_width / (x2 - x1), nbVisibleDrawingAreas * drawingArea_height / (y2 - y1));

    shift_X = -(x1 + x2 - (drawingArea_width / zoom)) / 2;
    for (i = 0; i < nbVisibleDrawingAreas; i++)
    {
        shift_Y[i] = -(y1 + y2 - ((nbVisibleDrawingAreas) * drawingArea_height / zoom)) / 2 - i * drawingArea_height / zoom;
    }
}

#define ZOOM_COEF 1.414

void ZoomPlus (void)
{
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (gtk_object_get_data (GTK_OBJECT (MainWindow2), "ZoomFit")), FALSE);
    shift_X -= drawingArea_width / zoom * (1 - 1 / ZOOM_COEF) / 2;
    shift_Y[0] -= drawingArea_height / zoom * (1 - 1 / ZOOM_COEF) / 2;
    zoom *= ZOOM_COEF;
    RedrawScreen ();
}

void ZoomMinus (void)
{
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (gtk_object_get_data (GTK_OBJECT (MainWindow2), "ZoomFit")), FALSE);
    zoom /= ZOOM_COEF;
    shift_X += drawingArea_width / zoom * (1 - 1 / ZOOM_COEF) / 2;
    shift_Y[0] += drawingArea_height / zoom * (1 - 1 / ZOOM_COEF) / 2;
    RedrawScreen ();
}

void ZoomFit (void)
{
    if (rootVertex)
    {
        double posX = rootVertex->posX;
        double posY = rootVertex->posY;
        double sizeX = GetVertexSizeX (NULL);
        double sizeY = GetVertexSizeY (NULL);

        SetVisibleWindow (posX - sizeX, posY - sizeY, posX + sizeX, posY + sizeY);
    }

    RedrawScreen ();
}

void ClearPixmap ()
{
    gdk_draw_rectangle (pixmap, drawingArea[0]->style->white_gc, TRUE, 0, 0, drawingArea_width, nbVisibleDrawingAreas * drawingArea_height);
}

void RedrawPixmap (void)
{
    ClearPixmap ();

    //
    void UpdateVertexActivity (void);
    gboolean DrawEdgeCon (struct ConnectedVertex *con);
    gboolean DrawVertex (struct Vertex *vertex);

    //

    UpdateVertexActivity ();

    if (libspring_LayoutProblem)
    {
        DrawText (0, 0, "<<< Layout Error: Dot is probably unavailable >>>", 0, 2, 12);
        return;
    }

    if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (gtk_object_get_data (GTK_OBJECT (MainWindow2), "ShowChansToggle"))))
    {
        DrawEdge_PassNum = 1;
        ForEachVisibleEdgeAndPorts (DrawEdgeCon);
        DrawEdge_PassNum = 2;
        ForEachVisibleEdgeAndPorts (DrawEdgeCon);
        DrawEdge_PassNum = 3;
        ForEachVisibleEdgeAndPorts (DrawEdgeCon);
    }

    ForEachVisibleVertexRecursiveAndPorts (NULL, DrawVertex, NULL);
}

void RedrawScreen (void)
{
    RedrawPixmap ();

    int i;

    for (i = 0; i < nbVisibleDrawingAreas; i++)
    {
        if (drawingArea[i] && pixmap)
            gtk_widget_draw (drawingArea[i], 0);
    }
}

/****************************************************/

struct Vertex *clickedComp, *draggedComp = NULL;
struct Edge *clickedEdge;
double clickedComp_x;
double clickedComp_y;
double lastMouse_x, lastMouse_y;
double draggedComp_anchorX, draggedComp_anchorY;
double draggedComp_originalScreenPosX, draggedComp_originalScreenPosY;
int resizeGroup;

#define RESIZEGROUP_TOP    1
#define RESIZEGROUP_BOTTOM 2
#define RESIZEGROUP_LEFT   4
#define RESIZEGROUP_RIGHT  8

#define DISTANCE_1(x,y) (fabs(x)+fabs(y))
#define DISTANCE_2(x,y) sqrt((x)*(x)+(y)*(y))
#define DISTANCE_INF(x,y) MAX(fabs(x),fabs(y))

gboolean FindWhichVertexIsClicked (struct Vertex *v)
{
    if (v->isGroup)
    {
#define MIN_DIST (3/zoom)
        if ((fabs (v->posX - v->sizeX - clickedComp_x) <= MIN_DIST) || (fabs (v->posX + v->sizeX - clickedComp_x) <= MIN_DIST))
            if ((clickedComp_y >= v->posY - v->sizeY - MIN_DIST) && (clickedComp_y <= v->posY + v->sizeY + MIN_DIST))
            {
                clickedComp = v;
            }
        if ((fabs (v->posY - v->sizeY - clickedComp_y) <= MIN_DIST) || (fabs (v->posY + v->sizeY - clickedComp_y) <= MIN_DIST))
            if ((clickedComp_x >= v->posX - v->sizeX - MIN_DIST) && (clickedComp_x <= v->posX + v->sizeX + MIN_DIST))
            {
                clickedComp = v;
            }
    } else
    {
        if (DISTANCE_2 ((v->posX - clickedComp_x) / v->sizeX, (v->posY - clickedComp_y) / v->sizeY) <= 1)
            clickedComp = v;
    }
    return TRUE;
}

double FindWhichEdgeIsClicked_mindist;
gboolean FindWhichEdgeIsClicked (struct ConnectedVertex *con)
{
    struct Edge *edge = con->edges->data;

    struct Vertex *v1 = GetSourceVertex (edge)->deepestVisibleParent;
    struct Vertex *v2 = GetDestVertex (edge)->deepestVisibleParent;
    double x1 = GetVertexCoordX (v1);
    double y1 = GetVertexCoordY (v1);
    double x2 = GetVertexCoordX (v2);
    double y2 = GetVertexCoordY (v2);

    double length = DISTANCE_2 (x2 - x1, y2 - y1);
    double pos = ((x2 - x1) * (clickedComp_x - x1) + (y2 - y1) * (clickedComp_y - y1)) / length;

    if (pos < 0 || pos > length)
        return TRUE;

    double proj_x = x1 + (x2 - x1) * pos / length;
    double proj_y = y1 + (y2 - y1) * pos / length;
    double dist = DISTANCE_2 (clickedComp_x - proj_x, clickedComp_y - proj_y);

    if (dist <= FindWhichEdgeIsClicked_mindist)
    {
        clickedEdge = edge;
        FindWhichEdgeIsClicked_mindist = dist;
    }
    return TRUE;
}

gboolean on_drawingarea1_button_press_event (GtkWidget * widget, GdkEventButton * event, gpointer user_data)
{
    long numDrawingArea = (long) user_data;
    double x = event->x;
    double y = event->y + numDrawingArea * drawingArea_height;
    int button = event->button;

    double X = x / zoom - shift_X;
    double Y = y / zoom - shift_Y[0];

    lastMouse_x = x;
    lastMouse_y = y;

    //    printf("button %d pressed %d %d\n",button,x,y);

    draggedComp = NULL;
    clickedComp = NULL;
    clickedEdge = NULL;
    resizeGroup = 0;
    clickedComp_x = X;
    clickedComp_y = Y;
    ForEachVisibleVertexRecursiveAndPorts (NULL, FindWhichVertexIsClicked, NULL);
    if (!clickedComp)
    {
        FindWhichEdgeIsClicked_mindist = 3 / zoom;
        ForEachVisibleEdgeAndPorts (FindWhichEdgeIsClicked);
    }

    if (clickedComp)
    {
        switch (button)
        {
        case 1:
            if (!clickedComp->isGroup)
            {
                draggedComp = clickedComp;
                draggedComp_anchorX = draggedComp->posX - X;
                draggedComp_anchorY = draggedComp->posY - Y;
                printf ("=>Dragging component %s\n", clickedComp->name);
            } else
            {
                if (event->state & GDK_CONTROL_MASK)
                {
                    if (event->state & GDK_SHIFT_MASK)
                    {
                        printf ("=> SetVisibleRootVertex parent of %s (%s)\n",
                          clickedComp->name, clickedComp->parent ? clickedComp->parent->name : "<root>");
                        SetVisibleRootVertex (clickedComp->parent);
                    } else
                    {
                        printf ("=> SetVisibleRootVertex %s\n", clickedComp->name);
                        SetVisibleRootVertex (clickedComp);
                    }
                    Core_ExpandCollapseUpdateAllGroups ();
                } else
                {
                    if (event->state & GDK_SHIFT_MASK)
                    {
                        draggedComp = clickedComp;
                        draggedComp_anchorX = draggedComp->posX - X;

                        if (fabs (clickedComp->posY - clickedComp->sizeY - Y) <= clickedComp->sizeY * 2 / 3)
                        {
                            draggedComp_anchorY = clickedComp->posY - clickedComp->sizeY - Y;
                            resizeGroup |= RESIZEGROUP_TOP;
                        } else if (fabs (clickedComp->posY + clickedComp->sizeY - Y) <= clickedComp->sizeY * 2 / 3)
                        {
                            draggedComp_anchorY = clickedComp->posY + clickedComp->sizeY - Y;
                            resizeGroup |= RESIZEGROUP_BOTTOM;
                        }

                        if (fabs (clickedComp->posX - clickedComp->sizeX - X) <= clickedComp->sizeX * 2 / 3)
                        {
                            draggedComp_anchorX = clickedComp->posX - clickedComp->sizeX - X;
                            resizeGroup |= RESIZEGROUP_LEFT;
                        } else if (fabs (clickedComp->posX + clickedComp->sizeX - X) <= clickedComp->sizeX * 2 / 3)
                        {
                            draggedComp_anchorX = clickedComp->posX + clickedComp->sizeX - X;
                            resizeGroup |= RESIZEGROUP_RIGHT;
                        }

                        printf ("=>Resizing group frame %s (%d)\n", clickedComp->name, resizeGroup);
                    } else
                    {
                        draggedComp = clickedComp;
                        draggedComp_anchorX = draggedComp->posX - X;
                        draggedComp_anchorY = draggedComp->posY - Y;
                        printf ("=>Dragging group frame %s\n", clickedComp->name);
                    }
                }
            }
            draggedComp_originalScreenPosX = x;
            draggedComp_originalScreenPosY = y;
            break;

        case 2:
            if (!clickedComp->isGroup)
            {
                struct Comp *comp = ((PtrVertexInfo) clickedComp->data)->infoForGraphView.comp;

                printf ("=>Clicked component %s : comp %d\n", clickedComp->name, comp ? comp->num : -1);
            } else
            {
                printf ("=>Expanding/Reducing subgroups in group %s\n", clickedComp->name);
                Core_ExpandCollapseGroup (clickedComp, clickedComp->areSubgroupsExpanded ? 3 : 2);
            }
            break;

        case 3:
            if (clickedComp->isGroup)
            {
                printf ("=>Showing/Hiding childs in group %s\n", clickedComp->name);
                Core_ExpandCollapseGroup (clickedComp, clickedComp->areChildsVisible ? 1 : 0);
            }
            break;
        }
    }

    if (clickedEdge /* && button != 1 */ )
    {
        struct Chan *chan = ((PtrEdgeInfo) clickedEdge->data)->chan;
        struct BreezePartChannelsChannel *channel = chan->channel;
        struct Position *pos = getBreezePartChannelsChannelPosition (channel);

        printf ("=>Clicked Edge %s -> %s : channel %d @ ", clickedEdge->source->name, clickedEdge->dest->name, chan->num);
        dumpPosition (stdout, pos);
        printf ("\n");
        Partitioning_PrintEdgeInfo (clickedEdge);

        Core_SelectChannel (chan);

        if (button == 2)
            FindAndPrintTimeTraceForEdge (clickedEdge);
    }

    return FALSE;
}

gboolean fct_motion_notify_delayed_allchans (double x, double y)
{
    if (draggedComp)
    {
        double X = x / zoom - shift_X + draggedComp_anchorX;
        double Y = y / zoom - shift_Y[0] + draggedComp_anchorY;

        // printf("move component to %d %d\n",x,y);

        if (!draggedComp->isGroup)
        {
            draggedComp->posX = X;
            draggedComp->posY = Y;
        } else
        {
            if (!resizeGroup)
            {
                double diffX = draggedComp->posX - X;
                double diffY = draggedComp->posY - Y;

                MoveGroupContent_X = -diffX;
                MoveGroupContent_Y = -diffY;
                ForEachVertexRecursive (draggedComp, MoveGroupContent, NULL);
            } else
            {
                if (resizeGroup & RESIZEGROUP_TOP)
                {
                    double diffY = draggedComp->posY - draggedComp->sizeY - Y;

                    draggedComp->posY -= diffY / 2;
                    draggedComp->rel_posY -= diffY / 2;
                    draggedComp->sizeY += diffY / 2;
                    draggedComp->rel_sizeY += diffY / 2;
                    //              MoveGroupContent_Y = -diffY/2;
                } else if (resizeGroup & RESIZEGROUP_BOTTOM)
                {
                    double diffY = draggedComp->posY + draggedComp->sizeY - Y;

                    draggedComp->posY -= diffY / 2;
                    draggedComp->rel_posY -= diffY / 2;
                    draggedComp->sizeY -= diffY / 2;
                    draggedComp->rel_sizeY -= diffY / 2;
                }

                if (resizeGroup & RESIZEGROUP_LEFT)
                {
                    double diffX = draggedComp->posX - draggedComp->sizeX - X;

                    draggedComp->posX -= diffX / 2;
                    draggedComp->rel_posX -= diffX / 2;
                    draggedComp->sizeX += diffX / 2;
                    draggedComp->rel_sizeX += diffX / 2;
                } else if (resizeGroup & RESIZEGROUP_RIGHT)
                {
                    double diffX = draggedComp->posX + draggedComp->sizeX - X;

                    draggedComp->posX -= diffX / 2;
                    draggedComp->rel_posX -= diffX / 2;
                    draggedComp->sizeX -= diffX / 2;
                    draggedComp->rel_sizeX -= diffX / 2;
                }
            }
        }

        return TRUE;
    }

    return FALSE;
}

/************************************************/

#define MOVEMENT_DELAY 50
double lastMouse_x, lastMouse_y;
double motion_notify_X;
double motion_notify_Y;
int timer_fct_motion_notify_delayed = 0;

gboolean on_drawingarea1_button_release_event (GtkWidget * widget, GdkEventButton * event, gpointer user_data)
{

    return FALSE;
}

gboolean fct_motion_notify_delayed_allchans (double x, double y);

gint fct_motion_notify_delayed (gpointer data)
{
    if (fct_motion_notify_delayed_allchans (motion_notify_X, motion_notify_Y))
        goto done;

    double x = motion_notify_X;
    double y = motion_notify_Y;

    //    double X = x/zoom + shift_X;
    //    double Y = y/zoom + shift_Y[currentDrawingArea];

    //       printf("fct_motion_notify_delayed\n");

    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (gtk_object_get_data (GTK_OBJECT (MainWindow2), "ZoomFit")), FALSE);
    shift_X -= (lastMouse_x - x) / zoom;
    int i;

    for (i = 0; i < nbVisibleDrawingAreas; i++)
        shift_Y[i] -= (lastMouse_y - y) / zoom;
    lastMouse_x = x;
    lastMouse_y = y;

  done:
    RedrawScreen ();
    timer_fct_motion_notify_delayed = 0;
    return FALSE;
}

gint fct_motion_notify_delayed_noButtonPressed (gpointer data)
{
    clickedComp_x = motion_notify_X / zoom - shift_X;
    clickedComp_y = motion_notify_Y / zoom - shift_Y[0];
    clickedEdge = NULL;
    FindWhichEdgeIsClicked_mindist = 3 / zoom;
    ForEachVisibleEdge (FindWhichEdgeIsClicked);

    if (clickedEdge)
    {
        struct Chan *chan = ((PtrEdgeInfo) clickedEdge->data)->chan;

        Core_ClearSelection ();
        Core_SelectChannel (chan);
    }

    timer_fct_motion_notify_delayed = 0;
    return FALSE;
}

gboolean on_drawingarea1_motion_notify_event (GtkWidget * widget, GdkEventMotion * event, gpointer user_data)
{
    // printf("motion notify %d\n", event->state);

    gboolean anyButton = event->state & (GDK_BUTTON1_MASK | GDK_BUTTON2_MASK | GDK_BUTTON3_MASK);

    if (!anyButton)
    {
        /* Motion without button = ContinuousMouseSelectionCapture */
        if (event->state & GDK_CONTROL_MASK)
        {
            GtkWidget *selectionCaptureToggleButton = GTK_WIDGET (gtk_object_get_data (GTK_OBJECT (MainWindow2),
                "SelectionCaptureToggleButton"));

            gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (selectionCaptureToggleButton), FALSE);
            return FALSE;
        }
    }

    long numDrawingArea = (long) user_data;

    motion_notify_X = event->x;
    motion_notify_Y = event->y + numDrawingArea * drawingArea_height;

    if (!timer_fct_motion_notify_delayed)
        timer_fct_motion_notify_delayed =
          gtk_timeout_add (MOVEMENT_DELAY, anyButton ? fct_motion_notify_delayed : fct_motion_notify_delayed_noButtonPressed, 0);

    return FALSE;
}

void on_ZoomPlus_clicked (GtkButton * button, gpointer user_data)
{
    ZoomPlus ();
}

void on_ZoomMinus_clicked (GtkButton * button, gpointer user_data)
{
    ZoomMinus ();
}

void on_ZoomFit_pressed (GtkButton * button, gpointer user_data)
{
    ZoomFit ();
}

void on_ShowDebugToolbarToggle_toggled (GtkToggleButton * togglebutton, gpointer user_data)
{
    GtkWidget *toolbar = GTK_WIDGET (gtk_object_get_data (GTK_OBJECT (MainWindow2), "DebugToolbar"));

    if (gtk_toggle_button_get_active (togglebutton))
        gtk_widget_show (toolbar);
    else
        gtk_widget_hide (toolbar);
}

void on_ScreenShot_clicked (GtkButton * button, gpointer user_data)
{
    ScreenShot ("screenshot.ps");
}

void on_ShowChansToggle_toggled (GtkToggleButton * togglebutton, gpointer user_data)
{
    RedrawScreen ();
}

void on_ShowWireforkTreeToggle_toggled (GtkToggleButton * togglebutton, gpointer user_data)
{
    displayOnlyWireforkTree = !gtk_toggle_button_get_active (togglebutton);
    RefreshAllConnectedVertices ();
    FullLayoutWithDot ();
    ComputeRealVertexSizeAndPositionRecursively (NULL);
    ZoomFit ();
}

void on_ShowTimeLineBehaviourToggle_toggled (GtkToggleButton * togglebutton, gpointer user_data)
{
    TimeLineBehaviourView_ToggleActivate (gtk_toggle_button_get_active (togglebutton));
}

void on_ChannelColorLimitSpinButton_changed (GtkEditable * editable, gpointer user_data)
{
    ChannelColorLimit1 =
      gtk_spin_button_get_value_as_int (GTK_SPIN_BUTTON (gtk_object_get_data (GTK_OBJECT (MainWindow2), "ChannelColorLimit1SpinButton")));
    ChannelColorLimit2 =
      gtk_spin_button_get_value_as_int (GTK_SPIN_BUTTON (gtk_object_get_data (GTK_OBJECT (MainWindow2), "ChannelColorLimit2SpinButton")));
    RedrawScreen ();
}

void on_ShowChannelNamesToggle_toggled (GtkToggleButton * togglebutton, gpointer user_data)
{
    isShowChannelNamesToggled = gtk_toggle_button_get_active (togglebutton);
    RedrawScreen ();
}

void on_ShowChannelValuesToggle_toggled (GtkToggleButton * togglebutton, gpointer user_data)
{
    isShowChannelValuesToggled = gtk_toggle_button_get_active (togglebutton);
    RedrawScreen ();
}

gboolean on_key_press_event (GtkWidget * widget, GdkEventKey * event, gpointer user_data)
{
    if (draggedComp)
    {
        //  printf ("key_press_event %d %c %s\n", event->keyval, event->keyval, event->string);
        switch (event->keyval)
        {
        case GDK_Delete:
            draggedComp->posZ = 100000;
            RedrawScreen ();
            break;
        case GDK_Insert:
            draggedComp->posZ = 0;
            RedrawScreen ();
            break;
        case GDK_BackSpace:
            if (draggedComp->posZ)
                draggedComp->posZ = 0;
            else
                draggedComp->posZ = 100000;
            RedrawScreen ();
            break;

        case GDK_Escape:
            fct_motion_notify_delayed_allchans (draggedComp_originalScreenPosX, draggedComp_originalScreenPosY);
            RedrawScreen ();
            break;
        }
    }

    else if (clickedEdge)
    {
        switch (event->keyval)
        {
        case 'x':
        case 'X':
            printf ("New weight\n");
            clickedEdge->noweight = !clickedEdge->noweight;
            FullLayoutWithDot ();
            ComputeRealVertexSizeAndPositionRecursively (NULL);
            ZoomFit ();
            break;
        }
    }

    return FALSE;
}

void on_NbGraphviewColumnsSpinButton_changed (GtkEditable * editable, gpointer user_data)
{
    int nb = gtk_spin_button_get_value_as_int (GTK_SPIN_BUTTON (gtk_object_get_data (GTK_OBJECT (MainWindow2),
          "NbGraphviewColumnsSpinButton")));

    if (nb == nbVisibleDrawingAreas)
        return;

    while (nb < nbVisibleDrawingAreas)
    {
        gtk_widget_hide (drawingArea[nbVisibleDrawingAreas - 1]);
        nbVisibleDrawingAreas--;
    }

    while (nb > nbVisibleDrawingAreas)
    {
        gtk_widget_show (drawingArea[nbVisibleDrawingAreas]);
        nbVisibleDrawingAreas++;
    }
}

static long on_ControlOrDataRadioButton_toggled_ControlLevel = 0;
static long on_ControlOrDataRadioButton_toggled_DataLevel = 0;
gboolean on_ControlOrDataRadioButton_toggled_subfct (struct Vertex *v)
{
    PtrVertexInfo vertexInfo = (PtrVertexInfo) GetVertexData (v);
    struct Comp *comp = vertexInfo->infoForGraphView.comp;

    if (!comp)
        return TRUE;

    GList *channels = comp->channels;
    gboolean isDataComp = FALSE;

    for (; channels; channels = channels->next)
    {
        struct Chan *chan = channels->data;

        if (chan->direction != ChanDirection_Sync)
        {
            isDataComp = TRUE;
            break;
        }
    }

    if (isDataComp)
    {
        switch (on_ControlOrDataRadioButton_toggled_DataLevel)
        {
        case 0:
            v->posZ = 0;
            break;
        case 1:
            v->posZ = 100000;
            break;
        case 2:
            v->posZ = 100000;
            break;
        }
    } else
    {
        switch (on_ControlOrDataRadioButton_toggled_ControlLevel)
        {
        case 0:
            v->posZ = 0;
            break;
        case 1:
            v->posZ = 100000;
            break;
        case 2:
            v->posZ = 100000;
            break;
        }
    }

    return TRUE;
}

void on_ControlRadioButton_toggled (GtkToggleButton * togglebutton, gpointer user_data)
{
    on_ControlOrDataRadioButton_toggled_ControlLevel = (long) user_data;
    ForEachVertexRecursive (Structure_GetRootVertex (), on_ControlOrDataRadioButton_toggled_subfct, NULL);
    RedrawScreen ();
}

void on_DataRadioButton_toggled (GtkToggleButton * togglebutton, gpointer user_data)
{
    on_ControlOrDataRadioButton_toggled_DataLevel = (long) user_data;
    ForEachVertexRecursive (Structure_GetRootVertex (), on_ControlOrDataRadioButton_toggled_subfct, NULL);
    RedrawScreen ();
}

void on_ControlLayout_Checkbutton_toggled (GtkToggleButton * togglebutton, gpointer user_data)
{
    useControlForLayout = gtk_toggle_button_get_active (togglebutton);
    FullLayoutWithDot ();
    ComputeRealVertexSizeAndPositionRecursively (NULL);
    ZoomFit ();
}

static GtkWidget *graphviewShortcutsHelpWindow = NULL;
void on_ShortcutsHelpTogglebutton_toggled (GtkToggleButton * togglebutton, gpointer user_data)
{
    if (!graphviewShortcutsHelpWindow)
        graphviewShortcutsHelpWindow = create_GraphviewShortcutsHelpWindow ();

    if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (togglebutton)))
        gtk_widget_show (graphviewShortcutsHelpWindow);
    else
        gtk_widget_hide (graphviewShortcutsHelpWindow);
}

gboolean on_GraphviewShortcutsHelpWindow_delete_event (GtkWidget * widget, GdkEvent * event, gpointer user_data)
{
    GtkWidget *shortcutsHelpTogglebutton = GTK_WIDGET (gtk_object_get_data (GTK_OBJECT (MainWindow2), "ShortcutsHelpTogglebutton"));

    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (shortcutsHelpTogglebutton), FALSE);
    return TRUE;
}

/***********************************************************************************************/
/***********************************************************************************************/
/***********************************************************************************************/
/***********************************************************************************************/
/***********************************************************************************************/
/***********************************************************************************************/
/***********************************************************************************************/

void InitialiseVisibleRootVertex (void)
{
    struct Vertex *rootVertex = Structure_GetRootVertex ();
    struct Vertex *balsaVertex = NULL;

    GList *childs = rootVertex->childs;

    for (; childs; childs = childs->next)
    {
        struct Vertex *child = childs->data;
        char *name = GetVertexName (child);

        if (!strcmp (name, "balsa"))
        {
            balsaVertex = child;
            break;
        }
    }

    if (!balsaVertex)
        return;

    struct Vertex *newVisibleRootVertex = NULL;

    for (childs = balsaVertex->childs; childs; childs = childs->next)
    {
        struct Vertex *child = childs->data;

        if (child->isGroup && !BEGINS_WITH (child->name, "Balsa")
          && !BEGINS_WITH (child->name, "TEST_HARNESS") && !BEGINS_WITH (child->name, "MAIN_INIT"))
        {
            if (newVisibleRootVertex)
                return;
            newVisibleRootVertex = child;
        }
    }

    if (newVisibleRootVertex)
        SetVisibleRootVertex (newVisibleRootVertex);
}

void InitialiseGraphPositions (void)
{
    struct Vertex *rootVertex = Structure_GetRootVertex ();

    InitialiseVisibleRootVertex ();

    RecursivelyRefreshDeepestExpandedParent (rootVertex, TRUE /*Bad assumption */ ,
      rootVertex);
    RefreshAllConnectedVertices ();

    //    ShrinkGroupVertex (rootVertex);
    //    ExpandGroupVertex (rootVertex);
    //    SaveGraph (graph_filename);

    InitialiseVertexSizeRecursively (NULL);
    InitialiseVertexPositionRecursively (NULL, 0, 0);
    UpdateAllVertexLabels ();
    FullLayoutWithDot ();
    // todo: "else" maybe TMP
    ComputeRealVertexSizeAndPositionRecursively (NULL);
}

/**************************************************************************************/
/**************************************************************************************/
/**************************************************************************************/

gboolean UpdateVertexActivity_subfct (struct Vertex *vertex)
{
    if (vertex->isGroup)
    {
        struct VertexInfo *vertexInfo = (struct VertexInfo *) GetVertexData (vertex);

        vertexInfo->infoForGraphView.activity_WireforkTree /= 2;
        vertexInfo->infoForGraphView.activity_MainLoop /= 2;
        vertexInfo->infoForGraphView.activity_Other /= 2;
    }
    return TRUE;
}

void UpdateVertexActivity (void)
{
    ForEachVisibleVertexRecursiveAndPorts (NULL, UpdateVertexActivity_subfct, NULL);
    GPtrArray *trace = Core_SimTrace_GetCurrentTimeTrace ();

    if (!trace)
        return;

    int i;

    for (i = 0; i < (int) trace->len; i++)
    {
        ChanTraceItem *traceItem = g_ptr_array_index (trace, i);
        struct Vertex *p1 = traceItem->edge->source->deepestVisibleParent;
        struct Vertex *p2 = traceItem->edge->dest->deepestVisibleParent;

        if (p1 == p2)
        {
            struct VertexInfo *vertexInfo = (struct VertexInfo *) GetVertexData (p1);

            if (traceItem->edge->type & EDGE_TYPE_WIREFORK_TREE)
                vertexInfo->infoForGraphView.activity_WireforkTree += 6;
            else if (traceItem->edge->type & EDGE_TYPE_MODULE_MAINLOOP)
                vertexInfo->infoForGraphView.activity_MainLoop += 6;
            else
                vertexInfo->infoForGraphView.activity_Other += 6;
        }
    }
}

gboolean DrawVertex (struct Vertex *vertex)
{
    if (TRUE /*showGroups || !vertex->isGroup */ )
    {
        double x = GetVertexCoordX (vertex);
        double y = GetVertexCoordY (vertex);
        double sizeX = GetVertexSizeX (vertex);
        double sizeY = GetVertexSizeY (vertex);

        double pseudoLengthZ = vertex->posZ;

        //    printf("drawvertex %d %d %d\n",x,y,size);
        if (vertex->isGroup)
        {
            /* Draw activity */
            int i, j;
            struct VertexInfo *vertexInfo = (struct VertexInfo *) GetVertexData (vertex);

            i = 1;
            for (j = 0; j < vertexInfo->infoForGraphView.activity_WireforkTree; i++, j++)
                DrawColorLine (GetColorGC (2), x, y + i / zoom, x + 5 / zoom, y + /*sizeY - 1 */ i / zoom);

            for (j = 0; j < vertexInfo->infoForGraphView.activity_MainLoop; i++, j++)
                DrawColorLine (GetColorGC (3), x, y + i / zoom, x + 5 / zoom, y + /*sizeY - 1 */ i / zoom);

            for (j = 0; j < vertexInfo->infoForGraphView.activity_Other; i++, j++)
                DrawColorLine (GetColorGC (1), x, y + i / zoom, x + 5 / zoom, y + /*sizeY - 1 */ i / zoom);

            if (pseudoLengthZ)
            {
                DrawColorRectangle (GetColorGC (8), x - sizeX, y - sizeY, sizeX * 2, sizeY * 2);
                DrawColorMultiLineText2 (GetColorGC (8), x - sizeX + 5, y - sizeY + 5, vertex->label ? : vertex->name, 0, 2, 12);
            } else
            {
                DrawRectangle (x - sizeX, y - sizeY, sizeX * 2, sizeY * 2);
                DrawMultiLineText2 (x - sizeX + 5, y - sizeY + 5, vertex->label ? : vertex->name, 0, 2, 12);
            }
        } else
        {
            if (vertex->type == 0)
                DrawColorEllipse (GetColorGC (pseudoLengthZ ? 8 : vertex->color), x, y, sizeX, sizeY);
            else
                DrawFilledEllipse (x, y, sizeX, sizeY);

            if (pseudoLengthZ)
                DrawColorText (GetColorGC (8), x, y, vertex->label ? : vertex->name, 1, 1, 11);
            else
                DrawText (x, y, vertex->label ? : vertex->name, 1, 1, 11);
        }
    }
    return TRUE;
}

int DrawEdge_overlappingEdges_num = 0;

gboolean DrawEdge (struct Edge *edge)
{
    GdkGC *color;
    gboolean veryFarFlag = FALSE;

    struct Vertex *v1 = GetSourceVertex (edge)->deepestVisibleParent;
    struct Vertex *v2 = GetDestVertex (edge)->deepestVisibleParent;

#ifdef NOTDEFINED
    int nbcon1 = g_list_length (v1->connectedVertices);
    int nbcon2 = g_list_length (v2->connectedVertices);

    switch (DrawEdge_PassNum)
    {
    case 1:
        if (nbcon1 >= ChannelColorLimit2 || nbcon2 >= ChannelColorLimit2)
            color = GetColorGC (8 /*Light Light Grey */ );
        else
            return TRUE;
        break;
    case 2:
        if (nbcon1 >= ChannelColorLimit2 || nbcon2 >= ChannelColorLimit2)
            return TRUE;
        if (nbcon1 >= ChannelColorLimit1 || nbcon2 >= ChannelColorLimit1)
            color = GetColorGC (7 /*Light Grey */ );
        else
            return TRUE;
        break;
    case 3:
        if (nbcon1 >= ChannelColorLimit2 || nbcon2 >= ChannelColorLimit2)
            return TRUE;
        if (nbcon1 >= ChannelColorLimit1 || nbcon2 >= ChannelColorLimit1)
            return TRUE;
        else
            color = GetColorGC (5 /*Grey */ );
        break;
    default:
        printf ("Error DrawEdge_PassNum=%d\n", DrawEdge_PassNum);
        exit (-1);
    }
#endif

    // if (v1==draggedComp ||v2==draggedComp)
    //  printf("%f %f -> %f %f  ;  %f %f -> %f %f (%d)\n", GetVertexCoordX(v1), GetVertexCoordY(v1), GetVertexCoordX(v2), GetVertexCoordY(v2), x1, y1, x2, y2, edge->direction);

    double length, dxnorm, dynorm;

    double centre_x1 = v1->posX;
    double centre_y1 = v1->posY;
    double centre_x2 = v2->posX;
    double centre_y2 = v2->posY;

    PrepareNormals (centre_x1, centre_y1, centre_x2, centre_y2, &length, &dxnorm, &dynorm);

    double pseudoLengthZ = MAX (v1->posZ, v2->posZ);

    switch (DrawEdge_PassNum)
    {
    case 1:
        if (length + pseudoLengthZ >= ChannelColorLimit2)
            color = GetColorGC (8 /*Light Light Grey */ );
        else
            return TRUE;
        break;
    case 2:
        if (length + pseudoLengthZ >= ChannelColorLimit2)
            return TRUE;
        if (length + pseudoLengthZ >= ChannelColorLimit1)
            color = GetColorGC (7 /*Light Grey */ );
        else
            return TRUE;
        break;
    case 3:
        if (length + pseudoLengthZ >= ChannelColorLimit2)
            return TRUE;
        if (length + pseudoLengthZ >= ChannelColorLimit1)
            return TRUE;
        else
            color = GetColorGC (5 /*Grey */ );
        break;
    default:
        printf ("Error DrawEdge_PassNum=%d\n", DrawEdge_PassNum);
        exit (-1);
    }

    if (pseudoLengthZ >= 10000)
        veryFarFlag = TRUE;

    double k1, k2;

    if (!v1->isGroup)
    {                           // ellipse
        k1 = sqrt (1 / (SQ (dxnorm / v1->sizeX) + SQ (dynorm / v1->sizeY)));
    } else
    {                           // rectangle
        double k1a = ABS (v1->sizeX / dxnorm);
        double k1b = ABS (v1->sizeY / dynorm);

        if (k1a < k1b)
            k1 = k1a;
        else
            k1 = k1b;
    }

    if (!v2->isGroup)
    {                           // ellipse
        k2 = sqrt (1 / (SQ (dxnorm / v2->sizeX) + SQ (dynorm / v2->sizeY)));
    } else
    {                           // rectangle
        double k2a = ABS (v2->sizeX / dxnorm);
        double k2b = ABS (v2->sizeY / dynorm);

        if (k2a < k2b)
            k2 = k2a;
        else
            k2 = k2b;
    }

    double dx1a = dxnorm * (k1 + 4);
    double dy1a = dynorm * (k1 + 4);
    double dx2a = dxnorm * (k2 + 4);
    double dy2a = dynorm * (k2 + 4);
    double dx1b = dxnorm * (k1 + 8);
    double dy1b = dynorm * (k1 + 8);

    //double dx2b = dxnorm * (k2 + 8);
    //double dy2b = dynorm * (k2 + 8);
    //double dx1c = dxnorm * (k1 + 3);
    //double dy1c = dynorm * (k1 + 3);
    double dx2c = dxnorm * (k2 + 3);
    double dy2c = dynorm * (k2 + 3);

    //double dx1d = dxnorm * (k1 + 6);
    //double dy1d = dynorm * (k1 + 6);
    double dx2d = dxnorm * (k2 + 6);
    double dy2d = dynorm * (k2 + 6);

    if (DrawEdge_overlappingEdges_num)
    {
        dx1a += dynorm * 6 * DrawEdge_overlappingEdges_num;
        dy1a -= dxnorm * 6 * DrawEdge_overlappingEdges_num;
        dx1b += dynorm * 6 * DrawEdge_overlappingEdges_num;
        dy1b -= dxnorm * 6 * DrawEdge_overlappingEdges_num;
        dx2a -= dynorm * 6 * DrawEdge_overlappingEdges_num;
        dy2a += dxnorm * 6 * DrawEdge_overlappingEdges_num;
        dx2c -= dynorm * 6 * DrawEdge_overlappingEdges_num;
        dy2c += dxnorm * 6 * DrawEdge_overlappingEdges_num;
        dx2d -= dynorm * 6 * DrawEdge_overlappingEdges_num;
        dy2d += dxnorm * 6 * DrawEdge_overlappingEdges_num;
    }

    int state = GetEdgeState (edge);
    gboolean isCurrentState = ((state & 128) == 128);
    int action = state & 127;

    if (action && !veryFarFlag)
        color = GetColorGC (action);

    if (edge->selectionLevel)
    {
        DrawThickColorLine (GetColorGC (6) /*6=LightYellow for Selection */ ,
          centre_x1, centre_y1, centre_x2, centre_y2, 16);
    }

    GdkGC *portColor = GetColorGC (pseudoLengthZ ? 8 /*Light Light Grey */ : 0);

    switch (edge->direction)
    {
    case EdgeDirection_Undefined:
        DrawColorLine (color, centre_x1, centre_y1, centre_x2, centre_y2);
        break;
    case EdgeDirection_Sync:
        DrawColorLine (color, centre_x1 + dx1b, centre_y1 + dy1b, centre_x2 - dx2d, centre_y2 - dy2d);
        DrawColorFilledEllipse (portColor, centre_x1 + dx1a, centre_y1 + dy1a, 4, 4);
        DrawColorEllipse (portColor, centre_x2 - dx2c, centre_y2 - dy2c, 3, 3);
        if (!strcmp (v1->name, ";") || !strcmp (v1->name, "DW") || !strcmp (v1->name, "A"))
        {
            struct Comp *source_comp = ((struct VertexInfo *) v1->data)->infoForGraphView.comp;
            struct Chan *edge_chan = ((PtrEdgeInfo) edge->data)->chan;
            GList *chans = source_comp->channels;
            int nbchans = g_list_length (chans);
            int num = nbchans - 1;

            for (; chans; chans = chans->next, num--)
            {
                struct Chan *chan = chans->data;

                if (chan == edge_chan)
                {
                    if (!strcmp (v1->name, "DW"))
                        num -= (nbchans - 1) / 2;
                    else if (!strcmp (v1->name, "A"))
                        num -= nbchans / 2 - 1;
                    char *num_str = g_strdup_printf ("%d", num);

                    DrawColorText (GetColorGC (3), centre_x1 + dx1a + 6 * dxnorm, centre_y1 + dy1a + 6 * dynorm, num_str, 2, 1, 4);
                    g_free (num_str);
                    break;
                }
            }
        }
        if (!strcmp (v2->name, "DW") || !strcmp (v2->name, "A"))
        {
            struct Comp *dest_comp = ((struct VertexInfo *) v2->data)->infoForGraphView.comp;
            struct Chan *edge_chan = ((PtrEdgeInfo) edge->data)->chan;
            GList *chans = dest_comp->channels;
            int num = g_list_length (chans) - 1;

            for (; chans; chans = chans->next, num--)
            {
                struct Chan *chan = chans->data;

                if (chan == edge_chan)
                {
                    if (!strcmp (v2->name, "A"))
                        num++;
                    if (num > 0)
                    {
                        char *num_str = g_strdup_printf ("%d", num);

                        DrawColorText (GetColorGC (3), centre_x2 - dx2a - 6 * dxnorm, centre_y2 - dy2a - 6 * dynorm, num_str, 2, 1, 4);
                        g_free (num_str);
                    }
                    break;
                }
            }
        }
        break;
    case EdgeDirection_Push:
        DrawColorArrow (color, centre_x1 + dx1b, centre_y1 + dy1b, centre_x2 - dx2d, centre_y2 - dy2d);
        DrawColorFilledEllipse (portColor, centre_x1 + dx1a, centre_y1 + dy1a, 4, 4);
        DrawColorEllipse (portColor, centre_x2 - dx2c, centre_y2 - dy2c, 3, 3);
        break;
    case EdgeDirection_Pull:
        DrawColorArrow (color, centre_x2 - dx2d, centre_y2 - dy2d, centre_x1 + dx1b, centre_y1 + dy1b);
        DrawColorFilledEllipse (portColor, centre_x1 + dx1a, centre_y1 + dy1a, 4, 4);
        DrawColorEllipse (portColor, centre_x2 - dx2c, centre_y2 - dy2c, 3, 3);
        break;
    default:
        printf ("Error in DrawEdgeCon: invalid edge direction\n");
    }

    if (isShowChannelValuesToggled && state)
    {
        char *value = GetEdgeValue (edge);

        if (value)
        {
            //            fprintf (stderr, "drawvalue %s\n", value);
            DrawColorText (color, (centre_x1 + centre_x2) / 2, (centre_y1 + centre_y2) / 2, value, 1, 2 + 1, 12);
        }
    }

    if (isCurrentState)
    {
        DrawThickColorLine (color, centre_x1, centre_y1, centre_x2, centre_y2, 4);

        char *text[4] = { "AD", "RU", "AU", "RD" };

        if (action >= 0 && action < 4)
            DrawColorText (GetColorGC (4), (centre_x1 + centre_x2) / 2, (centre_y1 + centre_y2) / 2, text[action], 1, 0, 12);
        else
            printf ("Unknown value in DrawEdge: action=%d\n", action);
    }

    if (isShowChannelNamesToggled)
    {
        PtrEdgeInfo edgeInfo = edge->data;

        if (edgeInfo)
        {
            struct Chan *chanStruct = edgeInfo->chan;

            if (chanStruct)
            {
                struct BreezePartChannelsChannel *breezePartChannelsChannel = chanStruct->channel;

                if (breezePartChannelsChannel)
                {
                    char *channelName = getBreezePartChannelsChannelName (breezePartChannelsChannel);

                    if (channelName)
                    {
                        DrawColorText (GetColorGC (4), (centre_x1 + centre_x2) / 2, (centre_y1 + centre_y2) / 2, channelName, 0, 1, 12);
                    }
                }
            }
        }
    }

    return TRUE;
}

gboolean DrawEdgeCon (struct ConnectedVertex * con)
{
    if (g_list_length (con->edges) == 1)
    {
        DrawEdge_overlappingEdges_num = con->overlapping_edges_num;
        return DrawEdge (con->edges->data);
    }

    GList *tmp = con->edges;

    for (; tmp; tmp = tmp->next)
    {
        struct Edge *edge = tmp->data;

        DrawEdge_overlappingEdges_num = 0;
        DrawEdge (edge);
    }

    struct Edge *edge = con->edges->data;
    struct Vertex *v1 = GetSourceVertex (edge)->deepestVisibleParent;
    struct Vertex *v2 = GetDestVertex (edge)->deepestVisibleParent;
    double x1 = GetVertexCoordX (v1);
    double y1 = GetVertexCoordY (v1);
    double x2 = GetVertexCoordX (v2);
    double y2 = GetVertexCoordY (v2);

    DrawWireCount (x1, y1, x2, y2, g_list_length (con->edges));

    return TRUE;
}

void on_Partitioning_clicked (GtkButton * button, gpointer user_data)
{
    Partitioning (FALSE);
}

void on_PartitioningWithNegWeights_clicked (GtkButton * button, gpointer user_data)
{
    Partitioning (TRUE);
}
