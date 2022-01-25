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

#ifndef BREEZESIM_BASE_H
#define BREEZESIM_BASE_H

#include <stdio.h>
#include <glib.h>
#include <gmp.h>
#include <string.h>
//#include <signal.h>

extern gboolean HLA_activated;
#define HLA_INITIALISATION_ENDING_TIME 1000   // When HLA is on, real execution only starts at timestep HLA_INITIALISATION_ENDING_TIME. Before that is RTZ initialisation.


struct comp;

typedef void (*_FCT2) (struct comp *, int port);

struct callback_
{
    _FCT2 fct;
    struct comp *comp;
    int portnum;
#ifdef DELAYS_WITH_ERROR
    float error;
#endif
};

struct chan
{
    struct callback_ requp, reqdown, ackup, ackdown, dataon, dataoff;
    unsigned long data;
    int width;                  //unused yet (todo)
    PtrTMPNode typeNode;
    char *name;
    int traceType;
    enum
    { DataDir_Sync, DataDir_Push, DataDir_Pull }
    dataDirection;
    gboolean isPort;

    //    int chan_num;
    int thread_num;
};

/* chan.traceType joinable values */
#define TRACETYPE_NONE       0
#define TRACETYPE_HHH        1
#define TRACETYPE_CONSOLELOG 2
#define TRACETYPE_VCD        4

#define STRUCT_COMP_BASE \
    int comp_num; \
	int delay_coef; \
    struct chan **chan; \
    int chanLength

struct comp
{
    STRUCT_COMP_BASE;
};

extern struct chan *channel;
extern int channelMax;
extern int WriteDebugAndAnimFiles;
void WriteDebugInfo_comps (char *str);
void GenericCallback_dataon (struct comp *comp, int port);
void GenericCallback_dataoff (struct comp *comp, int port);
void GenericCallback_dataon_OutOfOrder (struct comp *comp, int port);
void GenericCallback_dataoff_OutOfOrder (struct comp *comp, int port);

#define BREEZE_CONTROL_DELAY 1
#define BREEZE_DATAPATH_DELAY 4

#ifdef MPI
struct MPI_message
{
    int MPI_comp_num;
    int port;
    enum
    { REQUP, REQDOWN, ACKUP, ACKDOWN } event_type;
    int data_width;
    unsigned long long timestamp;
};

enum
{ MPI_TAG__IDLE_STATE_QUERY =
      3, MPI_TAG__REPLY_TO_IDLE_STATE_QUERY, MPI_TAG__I_AM_IDLE, MPI_TAG__I_AM_NOT_IDLE_ANYMORE, MPI_TAG__END_OF_IDLE_STATE_QUERY,
      MPI_TAG__EVERYBODY_IS_IDLE, MPI_TAG__KILLALL };
#endif // #ifdef MPI

/****************************/

#define WRITE_DEBUG_INFO_COMP(str,nb_ports) \
{ \
    if (WriteDebugAndAnimFiles) \
    { \
        int i; \
        WriteDebugInfo_comps (g_strdup_printf ("%d \"%s\" %d", c->comp_num, str, nb_ports)); \
        for (i=0; i<nb_ports; i++) \
        { \
	        WriteDebugInfo_comps (g_strdup_printf (" %d", (int) GET_CHAN_NUM(c->chan[i]))); \
        } \
        WriteDebugInfo_comps (g_strdup ("\n")); \
    } \
}

/***************************/

#define INIT_COMP(compname) \
    struct compname##_ *c = (struct compname##_ *) calloc (1, sizeof (struct compname##_)); \
    c->comp_num = num

#define INIT_COMP_PASSIVE_PORT(compname,port_num,channum) \
    ASSERT_CHANNEL_ARRAY_SIZE(port_num); \
    c->chan[port_num] = &channel[channum]; \
    channel[channum].requp.fct = compname##_requp; \
    channel[channum].reqdown.fct = compname##_reqdown; \
    channel[channum].requp.comp = channel[channum].reqdown.comp = (struct comp*)c; \
    channel[channum].requp.portnum = channel[channum].reqdown.portnum = port_num

#define INIT_COMP_ACTIVE_PORT(compname,port_num,channum) \
    ASSERT_CHANNEL_ARRAY_SIZE(port_num); \
    c->chan[port_num] = &channel[channum]; \
    channel[channum].ackup.fct = compname##_ackup; \
    channel[channum].ackdown.fct = compname##_ackdown; \
    channel[channum].ackup.comp = channel[channum].ackdown.comp = (struct comp*)c; \
    channel[channum].ackup.portnum = channel[channum].ackdown.portnum = port_num

#if defined(EVENT_DRIVEN_SCHEDULER) || defined(REORDERED_EVENT_DRIVEN_SCHEDULER)

extern struct callback_ *globalNextEvent_cb;
extern unsigned long long globalNextEvent_timestamp;

struct globalEventStruct
{
    struct callback_ *cb;
    unsigned long long timestamp;
};
extern struct globalEventStruct *globalEventArray;
extern int globalEventArray_max;
extern int globalEventArray_pos;

unsigned long long ProcessDelayCoef (unsigned long long delay, int coef);

#define SEND_REQUP_DELAYED(x,delay) { \
    if (globalNextEvent_cb != 0) { InsertEventAtLaterTime (&c->chan[x]->requp,BREEZE_CONTROL_DELAY+delay, c->delay_coef); } else {\
    globalNextEvent_cb = &c->chan[x]->requp; \
    globalNextEvent_timestamp = current_real_time + ProcessDelayCoef(BREEZE_CONTROL_DELAY + delay, c->delay_coef); }\
  }

#define SEND_REQDOWN_DELAYED(x,delay) { \
    if (globalNextEvent_cb != 0) { InsertEventAtLaterTime (&c->chan[x]->reqdown,BREEZE_CONTROL_DELAY+delay, c->delay_coef); } else {\
    globalNextEvent_cb = &c->chan[x]->reqdown; \
    globalNextEvent_timestamp = current_real_time + ProcessDelayCoef(BREEZE_CONTROL_DELAY + delay, c->delay_coef); }\
  }

#define SEND_ACKUP_DELAYED(x,delay) { \
    if (globalNextEvent_cb != 0) { InsertEventAtLaterTime (&c->chan[x]->ackup,BREEZE_CONTROL_DELAY+delay, c->delay_coef); } else {\
    globalNextEvent_cb = &c->chan[x]->ackup; \
    globalNextEvent_timestamp = current_real_time + ProcessDelayCoef(BREEZE_CONTROL_DELAY + delay, c->delay_coef); }\
  }

#define SEND_ACKDOWN_DELAYED(x,delay) { \
    if (globalNextEvent_cb != 0) { InsertEventAtLaterTime (&c->chan[x]->ackdown,BREEZE_CONTROL_DELAY+delay, c->delay_coef); } else {\
    globalNextEvent_cb = &c->chan[x]->ackdown; \
    globalNextEvent_timestamp = current_real_time + ProcessDelayCoef(BREEZE_CONTROL_DELAY + delay, c->delay_coef); }\
  }

#define SEND_DATAON_DELAYED(x,delay) { \
  WriteAnimationInfo_dataCallback_OutOfOrder (&c->chan[x]->dataon, current_real_time + ProcessDelayCoef(BREEZE_CONTROL_DELAY + delay, c->delay_coef), c->chan[x]); \
  }
//  InsertEventAtLaterTime (&c->chan[x]->dataon,BREEZE_CONTROL_DELAY+delay, c->delay_coef);

#define SEND_DATAOFF_DELAYED(x,delay) { \
  WriteAnimationInfo_signalCallback_OutOfOrder (&c->chan[x]->dataoff, current_real_time + ProcessDelayCoef(BREEZE_CONTROL_DELAY + delay, c->delay_coef)); \
  }
//  InsertEventAtLaterTime (&c->chan[x]->dataoff,BREEZE_CONTROL_DELAY+delay, c->delay_coef)

#define SEND_REQUP(x) SEND_REQUP_DELAYED(x,0)
#define SEND_REQDOWN(x) SEND_REQDOWN_DELAYED(x,0)
#define SEND_ACKUP(x) SEND_ACKUP_DELAYED(x,0)
#define SEND_ACKDOWN(x) SEND_ACKDOWN_DELAYED(x,0)

#define INIT_COMP_CONTROLLED_DATA_PORT(port_num,channum) \
    ASSERT_CHANNEL_ARRAY_SIZE(port_num); \
    channel[channum].dataon.fct = GenericCallback_dataon_OutOfOrder; \
    channel[channum].dataoff.fct = GenericCallback_dataoff_OutOfOrder; \
    channel[channum].dataon.comp = channel[channum].dataoff.comp = (struct comp*)c; \
    channel[channum].dataon.portnum = channel[channum].dataoff.portnum = port_num

#else //#if defined(EVENT_DRIVEN_SCHEDULER) || defined(REORDERED_EVENT_DRIVEN_SCHEDULER)

#define SEND_REQUP_DELAYED(x,delay) \
  InsertEventAtLaterTime (&c->chan[x]->requp,BREEZE_CONTROL_DELAY+delay, c->delay_coef)

#define SEND_REQDOWN_DELAYED(x,delay) \
  InsertEventAtLaterTime (&c->chan[x]->reqdown,BREEZE_CONTROL_DELAY+delay, c->delay_coef)

#define SEND_ACKUP_DELAYED(x,delay) \
  InsertEventAtLaterTime (&c->chan[x]->ackup,BREEZE_CONTROL_DELAY+delay, c->delay_coef)

#define SEND_ACKDOWN_DELAYED(x,delay) \
  InsertEventAtLaterTime (&c->chan[x]->ackdown,BREEZE_CONTROL_DELAY+delay, c->delay_coef)

#define SEND_DATAON_DELAYED(x,delay) \
  InsertEventAtLaterTime (&c->chan[x]->dataon,BREEZE_CONTROL_DELAY+delay, c->delay_coef)

#define SEND_DATAOFF_DELAYED(x,delay) \
  InsertEventAtLaterTime (&c->chan[x]->dataoff,BREEZE_CONTROL_DELAY+delay, c->delay_coef)

#define SEND_REQUP(x) SEND_REQUP_DELAYED(x,0)
#define SEND_REQDOWN(x) SEND_REQDOWN_DELAYED(x,0)
#define SEND_ACKUP(x) SEND_ACKUP_DELAYED(x,0)
#define SEND_ACKDOWN(x) SEND_ACKDOWN_DELAYED(x,0)

#define INIT_COMP_CONTROLLED_DATA_PORT(port_num,channum) \
    ASSERT_CHANNEL_ARRAY_SIZE(port_num); \
    channel[channum].dataon.fct = GenericCallback_dataon; \
    channel[channum].dataoff.fct = GenericCallback_dataoff; \
    channel[channum].dataon.comp = channel[channum].dataoff.comp = (struct comp*)c; \
    channel[channum].dataon.portnum = channel[channum].dataoff.portnum = port_num

#endif //#if defined(EVENT_DRIVEN_SCHEDULER) || defined(REORDERED_EVENT_DRIVEN_SCHEDULER)

#define SEND_REQUP_NODELAY(x) SEND_REQUP_DELAYED(x,-BREEZE_CONTROL_DELAY)
#define SEND_REQDOWN_NODELAY(x) SEND_REQDOWN_NODELAY(x,-BREEZE_CONTROL_DELAY)
#define SEND_ACKUP_NODELAY(x) SEND_ACKUP_NODELAY(x,-BREEZE_CONTROL_DELAY)
#define SEND_ACKDOWN_NODELAY(x) SEND_ACKDOWN_NODELAY(x,-BREEZE_CONTROL_DELAY)

#define NO_OPERATION()

#define GET_PORT_DATA(port_num) (c->chan[port_num]->data)

#define SET_PORT_DATA(port_num,value) \
  c->chan[port_num]->data = (long)(value);

//  WriteAnimationInfo_data(GET_CHAN_NUM(c->chan[port_num]),value);

#define FAKE_SET_PORT_DATA_FOR_DEBUG(port_num)

//  WriteAnimationInfo_data(GET_CHAN_NUM(c->chan[port_num]),GET_PORT_DATA(port_num))

#define FAKE_SET_PORT_DATA_FOR_DEBUG_MPINT(port_num)

//  WriteAnimationInfo_data_mpint(GET_CHAN_NUM(c->chan[port_num]),GET_PORT_DATA(port_num))

#define COPY_DATA_FROM_TO(src,dest) SET_PORT_DATA(dest,GET_PORT_DATA(src))

#define GET_CHAN_NUM(chann) (((long)(chann) - (long)channel) / (long)sizeof (struct chan))
//(((chan)-channel))

#define ASSERT_CHANNEL_ARRAY_SIZE(pos) \
    while (pos>=c->chanLength) { \
	    int newLength; \
        if (c->chanLength) \
	        newLength = c->chanLength * 2; \
	    else \
	        newLength = 10; \
        c->chan = realloc (c->chan, newLength*sizeof(struct chan*)); \
        memset (c->chan+c->chanLength, 0, newLength-c->chanLength); \
	    c->chanLength = newLength; \
    }

#endif
