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

#include <gtk/gtk.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <unistd.h>

#include "libspring.h"
#include "main.h"
#include "drawing.h"
#include "structure.h"
#include "simulation.h"
#include "simtrace.h"
#include "sourceview.h"
#include "core.h"

//int timer_animate = 0;

#define ANIMATE_DELAY 100
#define MAX_SIGNED_INT ((int)((unsigned int)(1<<31)-1))

GtkLabel *timeLabel;

/********** position in hhh file **********/

char *hhh_filename = NULL;
FILE *hhh_file = NULL;
struct HHHTimeUnitInfo
{
    long position;
    //    gboolean processed;
    GPtrArray *trace;
}
*positions = NULL;
int nb_positions = 0;
int last_read_time;
int last_drawable_time;
int last_drawn_time;
int last_processed_time;
long last_nonCRed_position;

/********** control threads **********/

struct ControlThread *controlThreads = NULL;
int controlThreadsLength = 0;
int controlThreadsLastNum = -1;

/**************************************/

gint animate_timer_callback (gpointer data);

//void UpdateTimeSpin (void);
void UpdateTimeLabel (void);
void StartReadTraceTimer (void);
void StopReadTraceTimer (void);

/*************************************/

int Core_SimTrace_CurrentPosition (void)
{
    return last_drawn_time;
}

int Core_SimTrace_MaxPosition (void)
{
    return last_drawable_time;
}

static gboolean on_TimeSpin_changed_AdjustEdge_posInTrace (struct Edge *edge);
static void on_TimeSpin_changed_AdjustThread_posInTrace (struct ControlThread *thread);
void DrawTimeStep (int time);
void ProcessUntilTime (int untilTime);
void Core_SimTrace_ChangeTimeTo (int time)
{
    if (!positions)
    {
        fprintf (stderr, "Bad request Core_SimTrace_ChangeTimeTo(%d), when positions == NULL\n", time);
        return;
    }

    if (time < 0)
        time = 0;
    if (time > last_drawable_time)
        time = last_drawable_time;

    ProcessUntilTime (time);

    last_drawn_time = time;
    ForEachEdge (on_TimeSpin_changed_AdjustEdge_posInTrace);
    Core_Simtrace_ForEachThread (on_TimeSpin_changed_AdjustThread_posInTrace);

    Core_SimTraceTimeChanged (time);

    //a remettre:    DrawTimeStep (time);
}

void Core_Simtrace_ForEachThread (THREAD_CALLBACK fct)
{
    int i = 0;

    for (i = 0; i <= controlThreadsLastNum; i++)
    {
        if (controlThreads[i].num)
            fct (&controlThreads[i]);
    }
}

/*************************************/

void ProcessSignal (char *str)
{
    char *ptr = strchr (str, ' ') + 1;
    int num = atoi (ptr);

    ptr = strchr (ptr, ' ') + 1;
    struct Edge *edge = g_ptr_array_index (edges, num - 1);

    int signum = 0;

    if (BEGINS_WITH (ptr, "requp"))
        signum = 1;
    else if (BEGINS_WITH (ptr, "ackup"))
        signum = 2;
    else if (BEGINS_WITH (ptr, "reqdown"))
        signum = 3;
    else if (BEGINS_WITH (ptr, "ackdown"))
        signum = 0;

    //    ptr = strchr (ptr, ' ') + 1;
    int threadNum = 0;          //atoi (ptr);

    ChanTraceItem *traceItem = g_new0 (ChanTraceItem, 1);

    traceItem->time = last_processed_time;
    traceItem->signum = signum;
    traceItem->edge = edge;

    /* Add to Edge */
    PtrEdgeInfo edgeInfo = (PtrEdgeInfo) edge->data;
    GPtrArray *trace = edgeInfo->trace;

    g_ptr_array_add (trace, traceItem);

    /* Add to Thread */
    if (threadNum)
    {
        struct ControlThread *thread = &controlThreads[threadNum];

        g_ptr_array_add (thread->trace, traceItem);
    }

    /* Add to Positions */
    if (!positions[last_processed_time].trace)
        positions[last_processed_time].trace = g_ptr_array_new ();
    g_ptr_array_add (positions[last_processed_time].trace, traceItem);
}

void ProcessDataOn (char *str)
{
    //    printf("time %d: data %s", last_read_time, str);
    char *ptr = strchr (str, ' ') + 1;
    int chanNum = atoi (ptr);

    ptr = strchr (ptr, ' ') + 1;

    ChanValueTraceItem *valueTraceItem = g_new0 (ChanValueTraceItem, 1);

    valueTraceItem->beginTime = last_processed_time;
    valueTraceItem->endTime = MAX_SIGNED_INT;
    ptr[strlen (ptr) - 1] = 0;
    valueTraceItem->value = g_strdup (ptr);

    /* Add to Edge */
    struct Edge *edge = g_ptr_array_index (edges, chanNum - 1);
    PtrEdgeInfo edgeInfo = (PtrEdgeInfo) edge->data;

    g_ptr_array_add (edgeInfo->valueTrace, valueTraceItem);

    /* Add to Thread */
    ptr = strchr (ptr, ' ') + 1;
    int threadNum = 0;          //atoi (ptr);

    if (threadNum)
    {
        struct ControlThread *thread = &controlThreads[threadNum];

        g_ptr_array_add (thread->valueTrace, valueTraceItem);

        valueTraceItem->edge = edge;
    }
    //    printf("=> %x stored in %x (edge %d: %x)\n", valueTraceItem, valueTrace, num-1, edge);
}

void ProcessDataOff (char *str)
{
    //    printf("time %d: data %s", last_read_time, str);
    char *ptr = strchr (str, ' ') + 1;
    int num = atoi (ptr);

    ptr = strchr (ptr, ' ') + 1;
    struct Edge *edge = g_ptr_array_index (edges, num - 1);

    PtrEdgeInfo edgeInfo = (PtrEdgeInfo) edge->data;
    GPtrArray *valueTrace = edgeInfo->valueTrace;

    if (valueTrace->len == 0)
    {
        g_warning ("Big error in hhh file: DataOff appears before DataOn (\"%s\")", str);
        return;
    }

    ChanValueTraceItem *valueTraceItem = (ChanValueTraceItem *) g_ptr_array_index (valueTrace,
      valueTrace->len - 1);

    valueTraceItem->endTime = last_processed_time;
}

void ProcessNewThread (char *str)
{
    char *ptr = strchr (str, ' ') + 1;
    int parentNum = atoi (ptr);

    ptr = strchr (ptr, ' ') + 1;
    int threadNum = atoi (ptr);

    while (threadNum >= controlThreadsLength)
    {
        controlThreads = g_renew (struct ControlThread, controlThreads, 2 * controlThreadsLength);
        memset (&controlThreads[controlThreadsLength], 0, controlThreadsLength * sizeof (struct ControlThread));
        controlThreadsLength *= 2;
    }

    controlThreads[threadNum].num = threadNum;
    controlThreads[threadNum].parentNum = parentNum;
    controlThreads[threadNum].beginTime = last_processed_time;
    controlThreads[threadNum].endTime = MAX_SIGNED_INT;
    controlThreads[threadNum].trace = g_ptr_array_new ();
    controlThreads[threadNum].valueTrace = g_ptr_array_new ();

    if (threadNum > controlThreadsLastNum)
        controlThreadsLastNum = threadNum;
}

void ProcessEndThread (char *str)
{
    char *ptr = strchr (str, ' ') + 1;
    int threadNum = atoi (ptr);

    controlThreads[threadNum].endTime = last_processed_time;
}

void ProcessUntilTime (int untilTime)
{
    DEBUG_printf ("ProcessUntilTime %d\n", untilTime);
    ASSERT (untilTime <= last_drawable_time);

    if (untilTime <= last_processed_time)
        return;

    char buf[10000];
    int ret1;
    char *ret2;

    FILE *hhh_file2 = fopen (hhh_filename, "r");

    last_processed_time++;
    do
    {
        long pos = positions[last_processed_time].position;

        if (pos)
        {
            ret1 = fseek (hhh_file2, pos, SEEK_SET);
            break;
        }

        last_processed_time++;
    }
    while (last_processed_time <= untilTime);

    if (last_processed_time > untilTime)
    {
        last_processed_time = untilTime;
        goto end;
    }

    if (ret1 == -1)
    {
        fprintf (stderr, "Strange error in ProcessUntilTime: ret1==-1\n");
        exit (-1);
    }

    while (1)
    {
        ret2 = fgets (buf, 10000, hhh_file2);
        if (ret2 == NULL)
        {
            fprintf (stderr, "Strange error in ProcessUntilTime: ret2==NULL\n");
            exit (-1);
        }

        int size = strlen (buf);

        if (!size || buf[size - 1] != '\n')
            break;

        //      printf("process %s\n", buf);
        if (BEGINS_WITH (buf, "time "))
        {
            int time = atoi (buf + 5);

            if (time > untilTime)
                goto end;
            last_processed_time = time;
        } else if (BEGINS_WITH (buf, "signal"))
        {
            ProcessSignal (buf);
        } else if (BEGINS_WITH (buf, "dataon "))
        {
            ProcessDataOn (buf);
        } else if (BEGINS_WITH (buf, "dataoff "))
        {
            ProcessDataOff (buf);
        } else if (BEGINS_WITH (buf, "new-thread"))
        {
            ProcessNewThread (buf);
        } else if (BEGINS_WITH (buf, "end-thread"))
        {
            ProcessEndThread (buf);
        } else if (BEGINS_WITH (buf, "end"))
        {
            last_processed_time = last_read_time;
            goto end;
        }
    }

  end:
    fclose (hhh_file2);
}

static gboolean on_TimeSpin_changed_AdjustEdge_posInTrace (struct Edge *edge)
{
    int posInTrace = 0;
    PtrEdgeInfo edgeInfo = (PtrEdgeInfo) edge->data;
    GPtrArray *trace = edgeInfo->trace;

    while (posInTrace < (int) trace->len)
    {
        ChanTraceItem *traceItem = g_ptr_array_index (trace, posInTrace);

        if (traceItem->time >= last_drawn_time)
            break;
        posInTrace++;
    }

    edgeInfo->posInTrace = posInTrace;

    int posInValueTrace = 0;
    GPtrArray *valueTrace = edgeInfo->valueTrace;

    while (posInValueTrace < (int) valueTrace->len)
    {
        ChanValueTraceItem *valueTraceItem = g_ptr_array_index (valueTrace, posInValueTrace);

        if (valueTraceItem->endTime >= last_drawn_time)
            break;
        posInValueTrace++;
    }

    edgeInfo->posInValueTrace = posInValueTrace;
    return TRUE;
}

static void on_TimeSpin_changed_AdjustThread_posInTrace (struct ControlThread *thread)
{
    /* Ctrl Trace */
    int lastValidPos = -1;
    int posInTrace = 0;
    GPtrArray *trace = thread->trace;

    while (posInTrace < (int) trace->len)
    {
        ChanTraceItem *traceItem = g_ptr_array_index (trace, posInTrace);

        if (traceItem->time > last_drawn_time)
            break;

        lastValidPos = posInTrace;
        posInTrace++;
    }

    if (lastValidPos >= 0)
        thread->posInTrace = lastValidPos;

    if (lastValidPos >= 0)
    {
        if ((int) trace->len <= lastValidPos)
            return;

        ChanTraceItem *traceItem = g_ptr_array_index (trace, lastValidPos);
        PtrEdgeInfo edgeInfo = (PtrEdgeInfo) traceItem->edge->data;
        struct Chan *chan = edgeInfo->chan;

        if (!chan)
            return;
        struct BreezePartChannelsChannel *channel = chan->channel;

        if (!channel)
            return;
        struct Position *pos = getBreezePartChannelsChannelPosition (channel);

        if (!pos)
            return;

        thread->position = pos;

        /* Find value */
        GPtrArray *valueTrace = edgeInfo->valueTrace;

        if (edgeInfo->posInValueTrace < (int) valueTrace->len)
        {
            ChanValueTraceItem *valueTraceItem = g_ptr_array_index (valueTrace, edgeInfo->posInValueTrace);

            if ((valueTraceItem->beginTime <= last_drawn_time) && (valueTraceItem->endTime >= last_drawn_time))
            {
                if ((chan->direction == ChanDirection_Push
                    && traceItem->signum == 1) || (chan->direction == ChanDirection_Pull && traceItem->signum == 2))
                    thread->data = valueTraceItem->value;
                else
                    thread->data = g_strdup_printf ("(%s)", valueTraceItem->value);
            } else
                thread->data = 0;
        } else
            thread->data = 0;

    } else
    {
        thread->position = 0;
        thread->data = 0;
    }

    /* Value Trace */
    /*
       lastValidPos = -1;
       int posInValueTrace = 0;
       GPtrArray *valueTrace = thread->valueTrace;

       while (posInValueTrace < (int) valueTrace->len)
       {
       ChanValueTraceItem *valueTraceItem = g_ptr_array_index (valueTrace, posInValueTrace);

       if (valueTraceItem->endTime >= last_drawn_time)
       {
       if (valueTraceItem->beginTime <= last_drawn_time)
       lastValidPos = posInValueTrace;
       else
       break;
       }
       posInValueTrace++;
       }

       if (lastValidPos >= 0)
       thread->posInValueTrace = lastValidPos;

       if (lastValidPos >= 0)
       {
       if ((int) valueTrace->len <= lastValidPos)
       return;

       ChanValueTraceItem *valueTraceItem = g_ptr_array_index (valueTrace, lastValidPos);

       if (valueTraceItem->beginTime == last_drawn_time)
       thread->data = valueTraceItem->value;
       else
       thread->data = g_strdup_printf ("(%s)", valueTraceItem->value);
       } else
       thread->data = 0;
     */
}

int GetEdgeState (struct Edge *edge)
{
    int time = last_drawn_time;
    PtrEdgeInfo edgeInfo = (PtrEdgeInfo) edge->data;
    GPtrArray *trace = edgeInfo->trace;
    int posInTrace = edgeInfo->posInTrace;

    // Try next trace item
    if (posInTrace < (int) trace->len)
    {
        ChanTraceItem *traceItem = g_ptr_array_index (trace, posInTrace);

        if (traceItem->time == time)
        {
            //            edgeInfo->posInTrace++;
            return traceItem->signum | 128;
        }
    }
    // Otherwise, use the last one, if it exists
    if (posInTrace > 0)
    {
        ChanTraceItem *traceItem = g_ptr_array_index (trace, posInTrace - 1);

        return traceItem->signum;
    }

    return 0;
}

char *GetEdgeValue (struct Edge *edge)
{
    int time = last_drawn_time;
    PtrEdgeInfo edgeInfo = (PtrEdgeInfo) edge->data;
    GPtrArray *valueTrace = edgeInfo->valueTrace;

#define posInValueTrace (((PtrEdgeInfo)edge->data)->posInValueTrace)

    // Adjust position (forward)
    while ((posInValueTrace < (int) valueTrace->len) && ((PtrChanValueTraceItem) g_ptr_array_index (valueTrace, posInValueTrace))->endTime < time)
        posInValueTrace++;

    // Try next valueTrace item
    if (posInValueTrace < (int) valueTrace->len)
    {
        ChanValueTraceItem *valueTraceItem = g_ptr_array_index (valueTrace, posInValueTrace);

        if ((valueTraceItem->beginTime <= time) && (valueTraceItem->endTime >= time))
        {
            //            posInValueTrace++;
            //     printf("return value %s (edge %x)\n", valueTraceItem->value, edge);
            return valueTraceItem->value;
        }
    }
    /*
       // Otherwise, use the last one, if it exists
       if (posInValueTrace > 0)
       {
       ChanValueTraceItem *valueTraceItem = g_ptr_array_index (valueTrace, posInValueTrace - 1);

       // printf("return value %s (edge %x)\n", valueTraceItem->value, edge);
       return valueTraceItem->value;
       }
     */

    return NULL;
#undef posInValueTrace
}

GPtrArray *Core_SimTrace_GetCurrentTimeTrace (void)
{
    if (last_drawn_time >= 0)
        return positions[last_drawn_time].trace;
    else
        return 0;
}

GPtrArray *Core_SimTrace_GetTraceAtTime (int time)
{
    if (time >= 0)
    {
        if (time > last_drawable_time)
            return 0;
        if (time > last_processed_time)
            ProcessUntilTime (time);
        return positions[time].trace;
    } else
        return 0;
}

GList *Core_SimTrace_RunUntilActivityInChanList (GList * chanList)
{
    if (!chanList)
        return NULL;

    GList *activatedChans = NULL;
    int time = last_drawn_time + 1;

    for (; time <= last_drawable_time; time++)
    {
        GPtrArray *trace = Core_SimTrace_GetTraceAtTime (time);

        if (!trace)
            continue;

        GList *tmp;

        for (tmp = chanList; tmp; tmp = tmp->next)
        {
            struct Chan *chan = tmp->data;
            struct Edge *edge = chan->data;
            int i;

            for (i = 0; i < (int) trace->len; i++)
            {
                ChanTraceItem *traceItem = g_ptr_array_index (trace, i);

                if (traceItem->edge == edge)
                {
                    activatedChans = g_list_prepend (activatedChans, chan);
                    break;
                }
            }
        }

        if (activatedChans)
        {
            Core_SimTrace_ChangeTimeTo (time);
            return activatedChans;
        }
    }

    return NULL;
}

GList *Core_SimTrace_RunBackwardsUntilActivityInChanList (GList * chanList)
{
    if (!chanList)
        return NULL;

    GList *activatedChans = NULL;
    int time = last_drawn_time - 1;

    for (; time >= 0; time--)
    {
        GPtrArray *trace = Core_SimTrace_GetTraceAtTime (time);

        if (!trace)
            continue;

        GList *tmp;

        for (tmp = chanList; tmp; tmp = tmp->next)
        {
            struct Chan *chan = tmp->data;
            struct Edge *edge = chan->data;
            int i;

            for (i = 0; i < (int) trace->len; i++)
            {
                ChanTraceItem *traceItem = g_ptr_array_index (trace, i);

                if (traceItem->edge == edge)
                {
                    activatedChans = g_list_prepend (activatedChans, chan);
                    break;
                }
            }
        }

        if (activatedChans)
        {
            Core_SimTrace_ChangeTimeTo (time);
            return activatedChans;
        }
    }

    if (time < 0)
        Core_SimTrace_ChangeTimeTo (0);

    return NULL;
}

/*************************************/

int read_trace_timer = 0;
gboolean read_trace_timer1_enabled = FALSE;
gboolean read_trace_timer2_enabled = FALSE;

static gboolean ResetReadTrace_ResetEdge (struct Edge *edge)
{
    PtrEdgeInfo edgeInfo = (PtrEdgeInfo) edge->data;

    g_ptr_array_free (edgeInfo->trace, TRUE);
    edgeInfo->trace = g_ptr_array_new ();
    edgeInfo->posInTrace = 0;

    g_ptr_array_free (edgeInfo->valueTrace, TRUE);
    edgeInfo->valueTrace = g_ptr_array_new ();
    edgeInfo->posInValueTrace = 0;

    return TRUE;
}

void ResetReadTrace (gboolean delete_hhh)
{
    //reset everything
    if (delete_hhh)
    {
        if (!hhh_filename)
            hhh_filename = g_strdup_printf ("%s.vcd", projectName);
        remove (hhh_filename);

        GtkWidget *warningButton = GTK_WIDGET (gtk_object_get_data (GTK_OBJECT (MainWindow2), "WarningButton"));

        gtk_widget_hide (warningButton);
    }

    hhh_file = NULL;
    last_nonCRed_position = 0;
    last_processed_time = -1;
    last_read_time = -1;
    last_drawable_time = -1;
    last_drawn_time = -1;
    if (positions)
        g_free (positions);
    nb_positions = 100000;
    positions = g_new0 (struct HHHTimeUnitInfo, nb_positions);

    controlThreadsLength = 1000;
    controlThreads = g_new0 (struct ControlThread, controlThreadsLength);

    controlThreadsLastNum = -1;

    GtkObject *dialogue = GTK_OBJECT (MainWindow2);

    timeLabel = GTK_LABEL (gtk_object_get_data (dialogue, "TimeLabel"));

    UpdateTimeLabel ();

    ForEachEdge (ResetReadTrace_ResetEdge);

    StartReadTraceTimer ();
    //    ResetSourceView ();
}

void SimTrace_Init (void)
{
    ResetReadTrace (FALSE);
}

void SimTrace_ExportSimLength (int length)
{
    DEBUG_printf (" SimTrace_ExportSimLength\n");
    UpdateTimeLabel ();         //Simulation_SimLengthChanged (length);
    Core_SimLengthChanged (length);
}

gint read_trace_timer_callback (gpointer data)
{
    //    fprintf (stderr,"read_trace_timer_callback\n");

    int last_drawable_time_ref = last_drawable_time;

    //    if (read_trace_timer1_enabled || 1)
    {
        char buf[10000];
        int ret1;
        char *ret2;

        if (!hhh_filename)
            hhh_filename = g_strdup_printf ("%s.vcd", projectName);
        if (!hhh_file)
            hhh_file = fopen (hhh_filename, "r");

        if (hhh_file)
        {
            clearerr (hhh_file);
            ret1 = fseek (hhh_file, last_nonCRed_position, SEEK_SET);
          again:
            ret2 = fgets (buf, 10000, hhh_file);
            if (ret1 != -1 && ret2 != NULL)
            {
                int size = strlen (buf);

                if (size && buf[size - 1] == '\n')
                {
                    //      printf("process %s\n", buf);
                    if (BEGINS_WITH (buf, "time "))
                    {
                        last_drawable_time = last_read_time;
                        last_read_time = atoi (buf + 5);
                        ASSERT (last_drawable_time < last_read_time);

                        if (last_read_time == 0)
                        {
                            GtkWidget *warningButton = GTK_WIDGET (gtk_object_get_data (GTK_OBJECT (MainWindow2),
                                "WarningButton"));

                            if (!GTK_WIDGET_VISIBLE (warningButton))
                            {
				// Automatic launch of gtkwave disabled
				//                            if (access ("nogtkwave", R_OK))
				//				Launch_GTKWave (0);
                            }
                        }

                        last_nonCRed_position = ftell (hhh_file);
                        ASSERT (last_nonCRed_position != -1);
                        while (last_read_time >= nb_positions)
                        {
                            positions = g_renew (struct HHHTimeUnitInfo, positions, 2 * nb_positions);

                            memset (&positions[nb_positions], 0, nb_positions * sizeof (struct HHHTimeUnitInfo));
                            nb_positions *= 2;
                        }
                        positions[last_read_time].position = last_nonCRed_position;
                    } else if (BEGINS_WITH (buf, "signal"))
                    {
                        //                        if (read_trace_timer1_enabled)
                        //                            ProcessSignal (buf);
                    } else if (BEGINS_WITH (buf, "data"))
                    {
                        //                        if (read_trace_timer1_enabled)
                        //                            ProcessData (buf);
                    } else if (BEGINS_WITH (buf, "new-thread"))
                    {
                        //                        if (read_trace_timer1_enabled)
                        //                            ProcessNewThread (buf);
                    } else if (BEGINS_WITH (buf, "end"))
                    {
                        last_drawable_time = last_read_time;
                        StopReadTraceTimer ();
                    }

                    last_nonCRed_position = ftell (hhh_file);
                    ASSERT (last_nonCRed_position != -1);
                    goto again;
                }
            }

            if (last_drawable_time != last_drawable_time_ref)
                SimTrace_ExportSimLength (last_drawable_time);
            UpdateTimeLabel ();
        }
    }

    /*
       if (read_trace_timer2_enabled)
       {
       animate_timer_callback (0);
       }
     */

    return TRUE;
}

void StartReadTraceTimer (void)
{
    if (!read_trace_timer)
    {
        read_trace_timer = gtk_timeout_add (1000, read_trace_timer_callback, 0);
    }
}

void StopReadTraceTimer (void)
{
    if (read_trace_timer)
    {
        gtk_timeout_remove (read_trace_timer);
        read_trace_timer = 0;
    }
}

/*************************************/

void UpdateTimeLabel (void)
{
    char text[20];

    if (last_drawable_time >= 0)
        sprintf (text, "Duration:%d", last_drawable_time);
    else
        sprintf (text, "Duration:#");

    gtk_label_set_text (timeLabel, text);

    //    gfloat max_value = last_drawable_time;

    //    timeSpin->adjustment->upper = max_value;
}

/*************************************/

void FindAndPrintTimeTraceForEdge (struct Edge *edge)
{
    PtrEdgeInfo edgeInfo = (PtrEdgeInfo) edge->data;
    GPtrArray *trace = edgeInfo->trace;
    int i;

    printf ("-- Start trace --\n");

    for (i = 0; i < (int) trace->len; i++)
    {
        ChanTraceItem *traceItem = g_ptr_array_index (trace, i);

        printf (" time %d: action %d", traceItem->time, traceItem->signum);
        switch (traceItem->signum)
        {
        case 0:
            printf ("(ackdown)\n");
            break;
        case 1:
            printf ("(requp)\n");
            break;
        case 2:
            printf ("(ackup)\n");
            break;
        case 3:
            printf ("(reqdown)\n");
            break;
        default:
            printf ("(?)\n");
        }
    }

    printf ("-- End trace (max time %d) --\n", last_processed_time);
}
