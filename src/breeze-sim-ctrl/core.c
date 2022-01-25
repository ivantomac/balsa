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

#include "core.h"
#include "structure.h"
#include "simtrace.h"
#include "listview.h"
#include "graphview.h"
#include "timelineview.h"
#include "sourceview.h"
#include "selectionview.h"
#include "timelinebehaviourview.h"
/*
#include <stdlib.h>
#include <breeze/libbreeze.h>
#include "main.h"
#include "mainwindow2.h"
*/

struct ViewProperties *views[NB_VIEWS];

void Core_Init ()
{
    Structure_Init ();
    SimTrace_Init ();

    views[0] = ListView_Init ();
    views[1] = GraphView_Init ();
    views[2] = TimeLineView_Init ();
    views[3] = SourceView_Init ();
    views[4] = SelectionView_Init ();
    views[5] = TimeLineBehaviourView_Init ();
    views[6] = 0;               //GTKWaveInterface_Init ();
}

void Core_SimLengthChanged (int length)
{
    DEBUG_printf ("Core_SimLengthChanged (%d)\n", length);
    int i;

    for (i = 0; i < NB_VIEWS; i++)
        if (views[i] && views[i]->enabled && views[i]->SimLengthChanged)
            views[i]->SimLengthChanged (length);
}

void Core_SelectChannel (struct Chan *chan)
{
    DEBUG_printf ("Core_SelectChannel\n");
    struct Edge *edge = chan->data;

    edge->selectionLevel = 1 - edge->selectionLevel;

    int i;

    for (i = 0; i < NB_VIEWS; i++)
        if (views[i] && views[i]->enabled && views[i]->SelectChannel)
            views[i]->SelectChannel (chan);
}

void Core_ClearSelection (void)
{
    DEBUG_printf ("Core_ClearSelection\n");
    int i;

    for (i = 0; i < NB_VIEWS; i++)
        if (views[i] && views[i]->enabled && views[i]->ClearSelection)
            views[i]->ClearSelection ();
}

void Core_ExpandCollapseGroup (struct Vertex *v, int expandCollapseType)
{
    DEBUG_printf ("Core_ExpandCollapseGroup %d\n", expandCollapseType);

    switch (expandCollapseType)
    {
    case 0:
        v->areChildsVisible = TRUE;
        if (!v->areSubgroupsExpanded)
            v->areSubgroupsExpanded = TRUE;
        break;

    case 1:
        v->areChildsVisible = FALSE;
        break;

    case 2:
        v->areSubgroupsExpanded = TRUE;
        break;

    case 3:
        v->areSubgroupsExpanded = FALSE;
        if (v->areChildsVisible)
            v->areChildsVisible = FALSE;
        break;
    }

    int i;

    for (i = 0; i < NB_VIEWS; i++)
        if (views[i] && views[i]->enabled && views[i]->ExpandCollapseGroup)
            views[i]->ExpandCollapseGroup (v, expandCollapseType);
}

void Core_ExpandCollapseUpdateAllGroups (void)
{
    DEBUG_printf ("Core_ExpandCollapseUpdateAllGroups\n");
    int i;

    for (i = 0; i < NB_VIEWS; i++)
        if (views[i] && views[i]->enabled && views[i]->ExpandCollapseUpdateAllGroups)
            views[i]->ExpandCollapseUpdateAllGroups ();
}

void Core_SimTraceTimeChanged (int time)
{
    int i;

    for (i = 0; i < NB_VIEWS; i++)
        if (views[i] && views[i]->enabled && views[i]->SimTraceTimeChanged)
            views[i]->SimTraceTimeChanged (time);
}

void Core_SetContinuousMouseSelectionCapture (int active)
{
    int i;

    for (i = 0; i < NB_VIEWS; i++)
        if (views[i] && views[i]->enabled && views[i]->SetContinuousMouseSelectionCapture)
            views[i]->SetContinuousMouseSelectionCapture (active);
}
