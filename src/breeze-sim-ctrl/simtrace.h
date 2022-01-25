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

#ifndef SIMTRACE_H
#define SIMTRACE_H

#include <glib.h>
#include "breeze/libbreeze.h"
#include "libspring.h"

/********** trace structures **********/

typedef struct ChanTraceItem
{
    int time;
    int signum;                 // 1=requp, 2=ackup, 3=reqdown, 0=ackdown, 4=dataon, 5=dataoff
    //    char *data;
    struct Edge *edge;          // used by thread thing (maybe tmp)
}
ChanTraceItem, *PtrChanTraceItem;

typedef struct ChanValueTraceItem
{
    int beginTime;
    int endTime;
    char *value;
    struct Edge *edge;          // used by thread thing (maybe tmp)
}
ChanValueTraceItem, *PtrChanValueTraceItem;

/****************************************/

int GetEdgeState (struct Edge *edge);
char *GetEdgeValue (struct Edge *edge);

struct ControlThread
{
    int num;
    int parentNum;
    int beginTime;
    int endTime;

    struct Position *position;
    char *data;

    GPtrArray *trace;
    int posInTrace;
    GPtrArray *valueTrace;
    int posInValueTrace;
};

typedef void (*THREAD_CALLBACK) (struct ControlThread *);

void ResetReadTrace (gboolean delete_hhh);
void SimTrace_Init (void);

int Core_SimTrace_CurrentPosition (void);
int Core_SimTrace_MaxPosition (void);
void Core_SimTrace_ChangeTimeTo (int time);
void Core_Simtrace_ForEachThread (THREAD_CALLBACK);
GPtrArray *Core_SimTrace_GetCurrentTimeTrace (void);
GPtrArray *Core_SimTrace_GetTraceAtTime (int time);
GList *Core_SimTrace_RunUntilActivityInChanList (GList * chanList);
GList *Core_SimTrace_RunBackwardsUntilActivityInChanList (GList * chanList);

void FindAndPrintTimeTraceForEdge (struct Edge *edge);

#endif
