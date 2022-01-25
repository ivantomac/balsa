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

#ifndef CORE_H
#define CORE_H

#include <glib.h>
#include <gtk/gtk.h>
#include <breeze/libbreeze.h>
#include <libspring.h>

#define DEBUG 0
#define DEBUG_printf(format,args...) if (DEBUG) fprintf (stderr, format, ## args);

#define NB_VIEWS 7
/*
#define LIST_VIEW 0
#define GRAPH_VIEW 1
#define TIMELINE_VIEW 2
#define SOURCE_VIEW 3
#define SELECTION_VIEW 4
#define TIMELINEBEHAVIOUR_VIEW 5
#define GTKWAVE_VIEW 6
*/

typedef void (*CALLBACK_void) (void);
typedef void (*CALLBACK_int) (int);
typedef void (*CALLBACK_chan) (struct Chan *);
typedef void (*CALLBACK_voidstar_int) (void *, int);

struct ViewProperties
{
    gboolean enabled;
    CALLBACK_int SimLengthChanged;
    CALLBACK_chan SelectChannel;
    CALLBACK_void ClearSelection;
    CALLBACK_voidstar_int ExpandCollapseGroup;
    CALLBACK_void ExpandCollapseUpdateAllGroups;
    CALLBACK_int SimTraceTimeChanged;
    CALLBACK_int SetContinuousMouseSelectionCapture;
};

typedef struct VertexInfo
{
    //    void *infoForView[NB_VIEWS];
    struct
    {
        GtkWidget *listViewItem;
        GtkWidget *listViewInternalItem;
        GtkWidget *listViewSubTree;
    }
    infoForListView;
    struct
    {
        struct Comp *comp;
        int activity_WireforkTree;
        int activity_MainLoop;
        int activity_Other;
    }
    infoForGraphView;
    struct
    {
        int lineNum;
    }
    infoForTimeLineBehaviourView;
}
VertexInfo, *PtrVertexInfo;

void Core_Init ();
struct BreezePart *Core_GetFlattenedBreezePart (void);

void Core_SimLengthChanged (int length);
void Core_SelectChannel (struct Chan *chan);
void Core_ClearSelection (void);
void Core_ExpandCollapseGroup (struct Vertex *v, int expandCollapseType);
void Core_ExpandCollapseUpdateAllGroups (void);
void Core_SimTraceTimeChanged (int time);
void Core_SetContinuousMouseSelectionCapture (int activate);

#endif
