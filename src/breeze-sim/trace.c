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

	`trace.c'
	Pattern analysis for compressed out-of-order trace

*/

#if defined(EVENT_DRIVEN_SCHEDULER) || defined(REORDERED_EVENT_DRIVEN_SCHEDULER)

#include "trace.h"
#include "pattern_analysis.h"
#include <stdlib.h>
#include <stdio.h>
#include <glib.h>

struct event_trace_time_slot
{
    int pos;
    int max;
    GPtrArray *callbacks;
};

struct event_trace_time_slot *event_trace = NULL;
int event_trace_time_span = 0;
unsigned long long first_uncommited_timestep = 0;

void Trace_Initialise (char *breezeFilename)
{
#ifdef DO_PATTERN_ANALYSIS
    PatternAnalysis_Initialise (breezeFilename);
#endif
}

void Trace_StartNewThread (void)
{
#ifdef DO_PATTERN_ANALYSIS
    PatternAnalysis_StartNewThread ();
#endif
}

void Trace_AppendSignal (struct callback_ *cb, unsigned long long time)
{
    // printf ("Trace_AppendSignal %lld\n", time);

#ifdef DO_PATTERN_ANALYSIS
    int chan_num, int event_type;

    int chan_num = (((int) cb) - ((int) channel)) / sizeof (struct chan);
    int event_type = ((((int) cb) - ((int) channel)) % sizeof (struct chan)) / sizeof (struct callback_);
    char *typekw[4] = { "requp", "reqdown", "ackup", "ackdown" };

    if (type >= 4)
        return;

    //        fprintf (TraceFile, "signal %d %d\n", channum, type);

    if (channel[channum].traceType & TRACETYPE_HHH)
    {
        WriteAnimationInfo_time (time);
        fprintf (TraceFile, "\n$comment\nsignal %d %s (time %lld)\n$end\n", chan_num, typekw[event_type], time);
        PatternAnalysis_AppendSignal (chan_num, event_type, time);
    }
#endif

    /*
       struct EventTraceItem *trace_item = g_new (struct EventTraceItem, 1);
       trace_item->chan_num = chan_num;
       trace_item->event_type = event_type;
       trace_item->time= time;
     */

    int delay = time - first_uncommited_timestep;

    if (delay >= event_trace_time_span || event_trace_time_span == 0)
    {
        struct event_trace_time_slot *old_event_trace = event_trace;
        int old_event_trace_time_span = event_trace_time_span;
        int first_time_slot = (event_trace_time_span == 0) ? 0 : first_uncommited_timestep % event_trace_time_span;

        if ((event_trace == NULL) && (event_trace_time_span == 0))
            event_trace_time_span = 64; //INIT_NUM_SLOTS
        while (event_trace_time_span < (delay + 1))
            event_trace_time_span *= 2;
        event_trace = (struct event_trace_time_slot *) calloc (event_trace_time_span, sizeof (struct event_trace_time_slot));

        if (old_event_trace != NULL)
        {
            int offset_first = (first_uncommited_timestep % event_trace_time_span) - (first_uncommited_timestep % old_event_trace_time_span);
            int offset_0 = (((first_uncommited_timestep / old_event_trace_time_span) + 1) * old_event_trace_time_span) % event_trace_time_span;

            /* copy things *below* first_time_slot */
            memcpy (event_trace + offset_0, old_event_trace, first_time_slot * sizeof (struct event_trace_time_slot));
            /* copy things from first_time_slot up to old_event_trace_time_span */
            memcpy (&(event_trace[first_time_slot + offset_first]),
              &(old_event_trace[first_time_slot]), (old_event_trace_time_span - first_time_slot) * sizeof (struct event_trace_time_slot));

            g_free (old_event_trace);
        }

        if (DebugFlag != 0)
            fprintf (stderr, "INFO: enlarging event_trace_time_span to %d at simulation time %lld\n", event_trace_time_span, time);
    }

    int slot = time % event_trace_time_span;

    if (!event_trace[slot].callbacks)
        event_trace[slot].callbacks = g_ptr_array_new ();

    g_ptr_array_add (event_trace[slot].callbacks, cb);
}

void Trace_CommitUntilTime (unsigned long long time) // time excluded in the commit
{
    // printf ("Trace_CommitUntilTime %lld\n", time);
    if (event_trace_time_span == 0)
        return;

    unsigned int slot = first_uncommited_timestep % event_trace_time_span;
    unsigned int nb_timesteps = time - first_uncommited_timestep;

    if (nb_timesteps > event_trace_time_span)
        nb_timesteps = event_trace_time_span;

    for (; nb_timesteps; nb_timesteps--)
    {
        GPtrArray *callbacks = event_trace[slot].callbacks;

        if (callbacks && callbacks->len)
        {
            int i;

            fprintf (TraceFile, "\n$comment\ntime %lld\n$end\n", first_uncommited_timestep);
            fprintf (TraceFile, "#%lld\n", first_uncommited_timestep);
            for (i = 0; i < callbacks->len; i++)
            {
                struct callback_ *cb = g_ptr_array_index (callbacks, i);

                int chan_num = (((long) cb) - ((long) channel)) / sizeof (struct chan);
                int event_type = ((((long) cb) - ((long) channel)) % sizeof (struct chan)) / sizeof (struct callback_);

                if (chan_num > channelMax || chan_num < 0)
                    event_type = 4;
                char *typekw[6] = { "requp", "reqdown", "ackup", "ackdown", "dataon",
                    "dataoff"
                };

                switch (event_type)
                {
                case 0:
                case 1:
                case 2:
                case 3:
                    fprintf (TraceFile, "\n$comment\nsignal %d %s\n$end\n", chan_num, typekw[event_type]);
		    { // print VCD part
			int value = event_type;
			// print to VCD: ctrl part
			char *vcd_wire_code_name = get_static_vcd_code (2*(chan_num-1));
			fprintf (TraceFile, "%d%s\n", vcd_value_strings[value], vcd_wire_code_name);
		    }
                    break;
                case 4:
                    fprintf (TraceFile, "\n$comment\ndataon %d %s\n$end\n", cb->portnum, cb->comp ? ((char *) cb->comp) : "0");
                    g_free (cb->comp);
                    g_free (cb);
                    break;
                case 5:
                    fprintf (TraceFile, "\n$comment\ndataoff %d\n$end\n", chan_num);
		    { // print VCD part
			// print to VCD: data part
			char *vcd_wire_code_name = get_static_vcd_code (2*(chan_num-1)+1);
			fprintf (TraceFile, "bz %s\n", vcd_wire_code_name);
		    }
                    break;
                }
            }
            callbacks->len = 0;
        }

        first_uncommited_timestep++;
        slot++;
        if (slot >= event_trace_time_span)
            slot = 0;
    }
}

#endif
