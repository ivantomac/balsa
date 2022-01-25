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

#include "timelineview.h"
#include "core.h"
#include "structure.h"
#include "main.h"
#include "simtrace.h"

static struct ViewProperties *timeLineViewProperties = NULL;

static GtkSpinButton *timeSpin = NULL;
gboolean timespin_callback_disabled = FALSE;
gboolean timespin_in_edition = FALSE;

int animate_timer = 0;
gint animate_timer_callback (gpointer data);

void on_TimeSpin_changed (GtkEditable * editable, gpointer user_data)
{
    if (timespin_callback_disabled)
        return;

    int time = gtk_spin_button_get_value_as_int (timeSpin);

    gboolean save = timespin_callback_disabled;

    timespin_callback_disabled = TRUE;
    Core_SimTrace_ChangeTimeTo (time);
    timespin_callback_disabled = save;
}

void StartAnimateTimer (void)
{
    static gfloat oldfvalue = -1;
    GtkWidget *scale = gtk_object_get_data (GTK_OBJECT (MainWindow2), "AnimateSpeedScale");

    //AB gfloat fvalue = GTK_RANGE (scale)->old_value;
    gfloat fvalue = gtk_range_get_value (GTK_RANGE (scale));

    if (!animate_timer || oldfvalue != fvalue)
    {
        oldfvalue = fvalue;
        int delay = (int) (2000 / pow (1.6, fvalue)); // Range = 2s/frame -> 55 fps

        animate_timer = gtk_timeout_add (delay, animate_timer_callback, 0);
    }
}

void StopAnimateTimer (void)
{
    if (animate_timer)
    {
        gtk_timeout_remove (animate_timer);
        animate_timer = 0;
    }
}

void on_AnimateButton_toggled (GtkToggleButton * togglebutton, gpointer user_data)
{
    gboolean active = gtk_toggle_button_get_active (togglebutton);

    if (active)
        StartAnimateTimer ();
    else
        StopAnimateTimer ();
}

gboolean on_AnimateSpeedScale_event (GtkWidget * widget, GdkEvent * event, gpointer user_data)
{
    if (event->type != 2)
        return FALSE;
    if (!animate_timer)
        return FALSE;

    static gfloat oldfvalue = -1;
    GtkWidget *scale = gtk_object_get_data (GTK_OBJECT (MainWindow2), "AnimateSpeedScale");

    //AB gfloat fvalue = GTK_RANGE (scale)->old_value;
    gfloat fvalue = gtk_range_get_value (GTK_RANGE (scale));

    if (oldfvalue != fvalue && animate_timer)
    {
        StopAnimateTimer ();
        StartAnimateTimer ();
    }

    return FALSE;
}

static GList *on_TimeSpinNextActionButton_ChanList = NULL;
gboolean on_TimeSpinNextActionButton_ComputeNextAction (struct ConnectedVertex *con)
{
    GList *tmp = con->edges;

    for (; tmp; tmp = tmp->next)
    {
        struct Edge *edge = tmp->data;
        struct Chan *chan = ((PtrEdgeInfo) edge->data)->chan;

        on_TimeSpinNextActionButton_ChanList = g_list_prepend (on_TimeSpinNextActionButton_ChanList, chan);
    }

    return TRUE;
}

void on_TimeSpin_next (GtkButton * button, gpointer user_data)
{
    on_TimeSpinNextActionButton_ChanList = NULL;
    ForEachVisibleEdge (on_TimeSpinNextActionButton_ComputeNextAction);

    GList *activatedChans = Core_SimTrace_RunUntilActivityInChanList (on_TimeSpinNextActionButton_ChanList);

    g_list_free (activatedChans);
    g_list_free (on_TimeSpinNextActionButton_ChanList);
}

void on_TimeSpin_previous (GtkButton * button, gpointer user_data)
{
    on_TimeSpinNextActionButton_ChanList = NULL;
    ForEachVisibleEdge (on_TimeSpinNextActionButton_ComputeNextAction);

    GList *activatedChans = Core_SimTrace_RunBackwardsUntilActivityInChanList (on_TimeSpinNextActionButton_ChanList);

    g_list_free (activatedChans);
    g_list_free (on_TimeSpinNextActionButton_ChanList);
}

gint animate_timer_callback (gpointer data)
{
    on_TimeSpin_next (NULL, NULL);
    return TRUE;
}

/****************************************/

void TimeLineView_Initialise (void)
{
    static gboolean initialised = FALSE;

    if (initialised)
        return;
    else
        initialised = TRUE;

    timeSpin = GTK_SPIN_BUTTON (gtk_object_get_data (GTK_OBJECT (MainWindow2), "TimeSpin"));
}

void TimeLineView_UpdateTimeSpin (void)
{
    if (timespin_callback_disabled)
        return;

    gboolean saved = timespin_callback_disabled;

    timespin_callback_disabled = TRUE;

    gfloat currentPos = (gfloat) Core_SimTrace_CurrentPosition ();
    gfloat maxPos = (gfloat) Core_SimTrace_MaxPosition ();

    if (!timespin_in_edition)
        gtk_spin_button_set_value (timeSpin, currentPos);

    //    gfloat max_value = last_drawable_time;
    timeSpin->adjustment->upper = maxPos;

    timespin_callback_disabled = saved;
}

void TimeLineView_UpdateAllStates (void)
{
    TimeLineView_UpdateTimeSpin ();
}

/***************************/

void TimeLineView_SimTraceTimeChanged (int time)
{
    DEBUG_printf ("TimeLineView_SimTraceTimeChanged (%d)\n", time);

    TimeLineView_UpdateTimeSpin ();
}

void TimeLineView_SimLengthChanged (int length)
{
    DEBUG_printf ("TimeLineView_SimLengthChanged (%d)\n", length);

    TimeLineView_UpdateTimeSpin ();
}

void TimeLineView_ExpandCollapseGroup (struct Vertex *v, int expandCollapseType)
{
    DEBUG_printf ("TimeLineView_ExpandCollapseGroup %d\n", expandCollapseType);
}

void TimeLineView_SelectChannel (struct Chan *chan)
{
    DEBUG_printf ("TimeLineView_SelectChannel\n");
}

struct ViewProperties *TimeLineView_Init (void)
{
    timeLineViewProperties = g_new0 (struct ViewProperties, 1);

    timeLineViewProperties->SimLengthChanged = TimeLineView_SimLengthChanged;
    timeLineViewProperties->SelectChannel = TimeLineView_SelectChannel;
    timeLineViewProperties->ExpandCollapseGroup = (CALLBACK_voidstar_int) TimeLineView_ExpandCollapseGroup;
    timeLineViewProperties->SimTraceTimeChanged = TimeLineView_SimTraceTimeChanged;
    return timeLineViewProperties;
}

void TimeLineView_ToggleActivate (gboolean activate)
{
    if (activate)
    {
        TimeLineView_Initialise ();
        TimeLineView_UpdateAllStates ();
    }

    timeLineViewProperties->enabled = activate;
}
