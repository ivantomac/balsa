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

#include "graphview.h"
#include "graphviewGUI.h"
#include "core.h"
#include "structure.h"
#include "timelineview.h"

static struct ViewProperties *graphViewProperties = NULL;

void GraphView_SetContinuousMouseSelectionCapture (int active)
{
    DEBUG_printf ("GraphView_SetContinuousMouseSelectionCapture (%d)\n", active);

    if (active)
        gtk_widget_add_events (drawingArea[0], GDK_POINTER_MOTION_MASK);
    else
    {
        // Not great, but working replacement for the unexisting gtk_widget_remove_events
        gdk_window_set_events (drawingArea[0]->window, gdk_window_get_events (drawingArea[0]->window) & ~GDK_POINTER_MOTION_MASK);
    }
}

void GraphView_SimTraceTimeChanged (int time)
{
    DEBUG_printf ("GraphView_SimTraceTimeChanged (%d)\n", time);

    RedrawScreen ();
}

void GraphView_SimLengthChanged (int length)
{
    DEBUG_printf ("GraphView_SimLengthChanged (%d)\n", length);
}

void GraphView_ExpandCollapseGroup (struct Vertex *v, int expandCollapseType)
{
    DEBUG_printf ("GraphView_ExpandCollapseGroup %d\n", expandCollapseType);

    UpdateVertexLabel (v);
    RefreshAllConnectedVertices ();
    FullLayoutWithDot ();
    ComputeRealVertexSizeAndPositionRecursively (NULL);
    ZoomFit ();
}

void GraphView_ExpandCollapseUpdateAllGroups (void)
{
    DEBUG_printf ("GraphView_ExpandCollapseUpdateAllGroups\n");

    UpdateAllVertexLabels ();
    RefreshAllConnectedVertices ();
    FullLayoutWithDot ();
    ComputeRealVertexSizeAndPositionRecursively (NULL);
    ZoomFit ();
}

void GraphView_SelectChannel (struct Chan *chan)
{
    DEBUG_printf ("GraphView_SelectChannel\n");

    /*
       struct Edge *edge = chan->data;
       DrawEdge (edge);
     */
    RedrawScreen ();

    /* Moved to selectionView->ShowHide
       struct Comp *compSource, *compDest;
       struct Vertex *vertexSource = NULL, *vertexDest = NULL;
       compSource = chan->source;
       compDest = chan->dest;
       if (compSource)
       vertexSource = compSource->data;
       if (compDest)
       vertexDest = compDest->data;

       if (!(vertexSource && isVertexVisible (vertexSource)) && !(vertexDest && isVertexVisible (vertexDest)))
       printf("need to show chan\n");
     */
}

struct ViewProperties *GraphView_Init (void)
{
    graphViewProperties = g_new0 (struct ViewProperties, 1);

    graphViewProperties->SimLengthChanged = GraphView_SimLengthChanged;
    graphViewProperties->SelectChannel = GraphView_SelectChannel;
    graphViewProperties->ExpandCollapseGroup = (CALLBACK_voidstar_int) GraphView_ExpandCollapseGroup;
    graphViewProperties->ExpandCollapseUpdateAllGroups = (CALLBACK_void) GraphView_ExpandCollapseUpdateAllGroups;
    graphViewProperties->SimTraceTimeChanged = GraphView_SimTraceTimeChanged;
    graphViewProperties->SetContinuousMouseSelectionCapture = GraphView_SetContinuousMouseSelectionCapture;
    return graphViewProperties;
}

void GraphView_ToggleActivate (gboolean activate)
{
    if (activate)
    {
        GraphViewGUI_InitialiseGraph ();
        GraphViewGUI_UpdateAllStates ();
    }

    graphViewProperties->enabled = activate;

    TimeLineView_ToggleActivate (activate);
}
