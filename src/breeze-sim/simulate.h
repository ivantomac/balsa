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

	`simulate.h'
	Simulation kernel

*/

#ifndef SIMULATE_H
#define SIMULATE_H

#include "../../config.h"
#include <stdio.h>
#include <glib.h>
#include <breeze/libbreeze.h>
#include <breezesim_plugin.h>

#define BEGINS_WITH(x,y) !strncmp(x,y,strlen(y))
#define ENDS_WITH(x,y) ((strlen(x)>=strlen(y)) && !strcmp(x+strlen(x)-strlen(y),y))

enum portDirection
{
    Input,
    Output,
    Sync
};

enum portActivity
{
    Passive,
    Active
};

struct portDecl
{
    char *name;
    enum portDirection type_input_output;
    enum portActivity type_passive_active;
    char *typeFormat;
    int typeWidth;
    int instantiated;
    int portnum;
};

struct signal_stack_timeslot
{
    int pos;
    int max;
    struct callback_ **cb;
};

extern gboolean CtrlPipe;
extern gboolean CtrlPipe_run;
extern int CtrlPipe_rununtil;
extern int CtrlPipe_speed;

extern unsigned long long current_real_time;
extern unsigned long long last_event_time;
extern int time_span;

#ifdef DELAYS_WITH_ERROR
extern float current_error;
#endif

extern struct chan *channel;

#ifdef REORDERED_EVENT_DRIVEN_SCHEDULER
extern GList *reordered_events_list;
extern int reordered_events_list_length;
extern int sequential_execution_duration;
#endif

extern struct signal_stack_timeslot *signal_stack;

extern int DebugFlag;
extern gboolean WriteDebugAndAnimFiles;
extern FILE *TraceFile;
extern gboolean TraceAllChans;
extern gboolean TraceNamedPortsOnly;
extern gboolean VcdTraceNamedPortsOnly;

extern void InitialisePorts (struct BreezePart *breezePart);
extern void InitialiseBreezeChannels (struct BreezePart *breezePart);
extern void InitialiseBreezeComponents (struct BreezePart *breezePart);
extern void InitialiseComponentDelayCoefs (struct BreezePart *breezePart);
extern void PrintChanData (FILE * f, struct chan *channel);

/* InitialiseBreezeTypes : register all the types read in from the Breeze file */
extern void InitialiseBreezeTypes (GList * types);

extern void CreateUndefinedInterfacePortComponents (GList * portDecls);

struct callback_;
extern void InsertEventAtLaterTime (struct callback_ *cb, unsigned long long delay, int coef);

#ifdef REORDERED_EVENT_DRIVEN_SCHEDULER
extern void InsertReorderedEventAtCurrentTime (struct globalEventStruct *event);
extern void InsertReorderedEventAfterLatestTime (struct globalEventStruct *event);
#endif

void ReportError (struct chan *chan, char *msg);

char *get_static_vcd_code (int channum);

#endif
