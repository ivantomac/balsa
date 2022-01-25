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

#include "simulate.h"
#include "breezesim_plugin.h"
#include "components_4phase.h"
#include "trace.h"

#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <stdbool.h>
#include <gmp.h>
#ifdef MPI
#include <mpi.h>
#endif // #ifdef MPI

#include "balsasim/object.h"
#include "balsasim/parameter.h"
#include "balsasim/bstring.h"

#define HOST_INTEGER_WIDTH_IN_BYTES sizeof(int)

void mpzui (mpz_ptr mp)
{
    int ui = mpz_get_ui (mp);

    fprintf (stderr, "%d\n", ui);
}

int arg2bool (char *str)
{
    if (*str == '\"')
        str++;

    if (!g_strcasecmp (str, "true"))
        return 1;
    if (!g_strcasecmp (str, "false"))
        return 0;
    printf ("Error in str2bool with \"%s\"\n", str);
    CloseBreezeSimulation (2);
    return 0;                   // Even though we never reach this point (it removes a compiler warning)
}

enum BinaryFuncType arg2BinaryFuncType (char *str)
{
    if (*str == '\"')
        str++;

    if (!strcmp (str, "NotEquals"))
        return NotEquals;
    if (!strcmp (str, "LessThan"))
        return LessThan;
    if (!strcmp (str, "GreaterThan"))
        return GreaterThan;
    if (!strcmp (str, "LessOrEquals"))
        return LessOrEquals;
    if (!strcmp (str, "GreaterOrEquals"))
        return GreaterOrEquals;
    if (!strcmp (str, "Equals"))
        return Equals;
    if (!strcmp (str, "Add"))
        return Add;
    if (!strcmp (str, "Subtract"))
        return Subtract;
    if (!strcmp (str, "ReverseSubtract"))
        return ReverseSubtract;
    if (!strcmp (str, "And"))
        return And;
    if (!strcmp (str, "Or"))
        return Or;
    if (!strcmp (str, "Xor"))
        return Xor;

    printf ("Error in arg2BinaryFuncType with \"%s\"\n", str);
    CloseBreezeSimulation (2);
    return 0;                   // Even though we never reach this point (it removes a compiler warning)
}

enum UnaryFuncType arg2UnaryFuncType (char *str)
{
    if (*str == '\"')
        str++;

    if (!strcmp (str, "Invert"))
        return Invert;
    if (!strcmp (str, "Negate"))
        return Negate;

    printf ("Error in arg2UnaryFuncType with \"%s\"\n", str);
    CloseBreezeSimulation (2);
    return 0;                   // Even though we never reach this point (it removes a compiler warning)
}

mpz_ptr atompint (char *str)
{
    char *ptr = str;
    char *tmpstr;
    int length = 0;
    mpz_ptr result;

    while (*ptr && *ptr >= '0' && *ptr <= '9')
    {
        ptr++;
        length++;
    }

    tmpstr = (char *) malloc (length + 1);
    strncpy (tmpstr, str, length);
    tmpstr[length] = 0;

    result = (mpz_ptr) malloc (sizeof (mpz_t));
    mpz_init (result);
    mpz_set_str (result, tmpstr, 10);

    free (tmpstr);
    return result;
}

/* Bit field stuff */
struct BitField
{
    unsigned low_index;
//  unsigned high_index;
    unsigned width;
    gboolean is_mpint;
};

struct BitField *decode_bit_fields (char *readPortsSpecString, int nb_read_ports, int width)
{
    // Ignore the first \", which is due to my crap parsing of string arguments
    if (readPortsSpecString[0] == '\"')
        readPortsSpecString++;

    // Create result array with default values
    struct BitField *bit_fields = g_new (struct BitField, nb_read_ports);
    int i;

    for (i = 0; i < nb_read_ports; i++)
    {
        bit_fields[i].low_index = 0;
//  bit_fields[i].high_index = width-1;
        bit_fields[i].width = width;
        bit_fields[i].is_mpint = (width > (HOST_INTEGER_WIDTH_IN_BYTES * 8));
    }

    // Fill in the specified bit fields in the array
    char **parts = g_strsplit (readPortsSpecString, ";", 0);

    for (i = 0; i < nb_read_ports && parts[i]; i++)
    {
        bit_fields[i].low_index = atoi (parts[i]); // = 0 if parts[i]="" or parts[i]=":..."
        char *colon = strchr (parts[i], '.');
        unsigned high_index = width - 1;

        if (colon && colon[1] == '.' && colon[2])
        {
//   bit_fields[i].high_index = atoi (colon+1);
            high_index = atoi (colon + 2);
        }
        bit_fields[i].width = high_index - bit_fields[i].low_index + 1;
        bit_fields[i].is_mpint = (bit_fields[i].width > (HOST_INTEGER_WIDTH_IN_BYTES * 8));
    }

    return bit_fields;
}

/* SetMpzTFromPointer, GetPointerFromMpzT : set an mpint to a pointer value, get pointer from mpint. */
static void SetMpzTFromPointer (mpz_t mpint, void *pointer)
{
#if SIZEOF_VOID_P == 8
    mpz_set_ui (mpint, (((guint64) pointer) & 0xFFFFFFFF00000000) >> 32);
    mpz_mul_2exp (mpint, mpint, 32);
    mpz_add_ui (mpint, mpint, (GPOINTER_TO_INT (pointer)) & 0xFFFFFFFF);
#else
    mpz_set_ui (mpint, (guint32) pointer);
#endif
}
static void *GetPointerFromMpzT (mpz_t mpint)
{
/* FIXME, do SIZEOF_VOID_P == 8 case later */
    return (void *) mpz_get_ui (mpint);
}

#define EXPAND_INT_SIGN(val,width) ((width==(sizeof(int)*8) || !(val>>(width-1))) ? val : (val | ~((1<<width)-1)))

/*
#define INIT_TRACED_PORT(chan_num,chanWidth) \
    if (TraceFile && !channel[chan_num].isTraced) { \
        channel[chan_num].isTraced = TRUE; \
        fprintf (TraceFile, "init-channel %d %d\n", chan_num, chanWidth); \
    }
*/

#ifdef WRITE_THREAD_INFO_______USELESS_NOW
static int next_thread_num = 1;

#define TRACE_INITIAL_THREAD(portdest) if (TraceFile) { c->chan[portdest]->thread_num = next_thread_num; fprintf (TraceFile, "\n$comment\nnew-thread 0 %d\n$end\n", next_thread_num); next_thread_num++; }
#define TRACE_NEW_THREAD(portdest,portsrc) if (TraceFile) { c->chan[portdest]->thread_num = next_thread_num; fprintf (TraceFile, "\n$comment\nnew-thread %d %d\n$end\n", c->chan[portsrc]->thread_num, next_thread_num); next_thread_num++; }
#define TRACE_FOLLOW_THREAD(portdest,portsrc) if (TraceFile) { c->chan[portdest]->thread_num = c->chan[portsrc]->thread_num; }

#else

#define TRACE_INITIAL_THREAD(portdest) ;
#define TRACE_NEW_THREAD(portdest,portsrc) ;
#define TRACE_FOLLOW_THREAD(portdest,portsrc) ;

#endif

// Debug info output
FILE *GetDebugFile_vis_info_chans (void)
{
    static FILE *f = 0;

    if (!f)
    {
        f = fopen ("vis_info_chans", "w");
        if (!f)
        {
            printf ("Error in opening vis_info_chans for writing\n");
            CloseBreezeSimulation (2);
        }
    }
    return f;
}

FILE *GetDebugFile_vis_info_comps (void)
{
    static FILE *f = 0;

    if (!f)
    {
        f = fopen ("vis_info_comps", "w");
        if (!f)
        {
            printf ("Error in opening vis_info_comps for writing\n");
            CloseBreezeSimulation (2);
        }
    }
    return f;
}

FILE *GetDebugFile_anim_info (void)
{
    static FILE *f = 0;

    if (!f)
    {
        f = fopen ("anim_info", "w");
        if (!f)
        {
            printf ("Error in opening anim_info for writing\n");
            CloseBreezeSimulation (2);
        }
    }
    return f;
}

void WriteDebugInfo_chans (char *str)
{
    if (0 /*WriteDebugAndAnimFiles */ )
    {
        FILE *f = GetDebugFile_vis_info_chans ();

        fprintf (f, "%s\n", str);
        fflush (f);
        g_free (str);
    }
}
void WriteDebugInfo_comps (char *str)
{
    if (0 /*WriteDebugAndAnimFiles */ )
    {
        FILE *f = GetDebugFile_vis_info_comps ();

        fprintf (f, "%s", str);
        fflush (f);
        g_free (str);
    }
}

void WriteAnimationInfo (char *str, struct chan *chan)
{
    if (0 /*WriteDebugAndAnimFiles */ )
    {
        FILE *f_anim = GetDebugFile_anim_info ();

        fprintf (f_anim, "%s %d\n", str, (int) GET_CHAN_NUM (chan));
        fflush (f_anim);
    }
}

void WriteAnimationInfo_time (unsigned long long time)
{
    if (0 /*WriteDebugAndAnimFiles */ )
    {
        static unsigned long long last_written_time = ~((unsigned long long) 0);
        FILE *f_anim = GetDebugFile_anim_info ();

        if (time != last_written_time)
        {
            fprintf (f_anim, "time %lld\n", time);
            fflush (f_anim);
            last_written_time = time;
        }
    }

    if (TraceFile)
    {
#if defined(EVENT_DRIVEN_SCHEDULER) || defined(REORDERED_EVENT_DRIVEN_SCHEDULER)

        static int seq = 0;

        fprintf (TraceFile, "\n$comment\ntime %d\n$end\n", seq);
        fprintf (TraceFile, "#%d\n", seq);
        seq++;

#else

        static unsigned long long last_written_time = ~((unsigned long long) 0);

        if (time != last_written_time)
        {
            fprintf (TraceFile, "\n$comment\ntime %lld\n$end\n", time);
            fprintf (TraceFile, "#%lld\n", time);
            last_written_time = time;
        }
#endif
    }
}

void WriteAnimationInfo_signalCallback (struct callback_ *cb)
{
    int chan_num = (int) GET_CHAN_NUM ((struct chan *) cb);

    if (channel[chan_num].traceType != TRACETYPE_NONE)
    {
        int type = ((((long) cb) - ((long) channel)) % sizeof (struct chan)) / sizeof (struct callback_);
        char *typekw[4] = { "requp", "reqdown", "ackup", "ackdown" };

        if (type >= 4)
            return;

	WriteAnimationInfo_time (current_real_time);

        //        fprintf (TraceFile, "signal %d %d\n", chan_num, type);

        if (channel[chan_num].traceType & TRACETYPE_HHH)
        {
	    // print HHH part
            fprintf (TraceFile, "\n$comment\nsignal %d %s\n$end\n", chan_num, typekw[type]);
	}
        if (channel[chan_num].traceType & TRACETYPE_VCD)
	{ // print VCD part
	    int value = type;
	    // print to VCD: ctrl part
	    char *vcd_wire_code_name = get_static_vcd_code (2*(chan_num-1));
	    fprintf (TraceFile, "%s%s\n", vcd_value_strings[value], vcd_wire_code_name);
        }
        if (channel[chan_num].traceType & TRACETYPE_CONSOLELOG)
        {
            switch (channel[chan_num].dataDirection)
            {
            case DataDir_Sync:
                if (type == 0)
                {               // REQUP
                    printf ("** %lld: chan `%s' syncing\n", current_real_time,
                      channel[chan_num].name + (BEGINS_WITH (channel[chan_num].name, "balsa.") ? 6 : 0));
                }
                break;
            case DataDir_Push:
                if (type == 0)
                {               // REQUP
                    printf ("** %lld: chan `%s' reading ", current_real_time,
                      channel[chan_num].name + (BEGINS_WITH (channel[chan_num].name, "balsa.") ? 6 : 0));
                    PrintChanData (stdout, &channel[chan_num]);
                    printf ("\n");
                }
                break;
            case DataDir_Pull:
                if (type == 2)
                {               // ACKUP
                    printf ("** %lld: chan `%s' writing ", current_real_time,
                      channel[chan_num].name + (BEGINS_WITH (channel[chan_num].name, "balsa.") ? 6 : 0));
                    PrintChanData (stdout, &channel[chan_num]);
                    printf ("\n");
                }
                break;
            }
        }
    }
}

#if defined(EVENT_DRIVEN_SCHEDULER) || defined(REORDERED_EVENT_DRIVEN_SCHEDULER)
void WriteAnimationInfo_signalCallback_OutOfOrder (struct callback_ *cb, unsigned long long time)
{
    if (!TraceFile)
        return;

    int chan_num = (int) GET_CHAN_NUM ((struct chan *) cb);

    if (chan_num > channelMax || chan_num <= 0)
        return;

    if (channel[chan_num].traceType != TRACETYPE_NONE)
    {
        Trace_AppendSignal (cb, time);
    }
#ifdef EVENT_DRIVEN_SCHEDULER_DEBUG
    int type = ((((int) cb) - ((int) channel)) % sizeof (struct chan)) / sizeof (struct callback_);
    char *typekw[] = { "requp", "reqdown", "ackup", "ackdown", "5", "6" };
    printf ("pseudo-time:%lld - signal %d %s\n", current_real_time, chan_num, typekw[type]);
#endif
}

void WriteAnimationInfo_dataCallback_OutOfOrder (struct callback_ *cb, unsigned long long time, struct chan *chan)
{
    if (!TraceFile)
        return;

    unsigned int chan_num = (unsigned int) GET_CHAN_NUM ((struct chan *) cb);

    if (chan_num > channelMax || chan_num <= 0)
        return;

    if (channel[chan_num].traceType != TRACETYPE_NONE)
    {
        struct callback_ *new_cb = g_new0 (struct callback_, 1);

        *new_cb = *cb;
        new_cb->portnum = chan_num;

        if (!chan->typeNode)
        {
            if (channel[chan_num].width <= (HOST_INTEGER_WIDTH_IN_BYTES * 8))
                new_cb->comp = (struct comp *) g_strdup_printf ("0x%x", (int) chan->data);
            else
            {
                char *strtmp = mpz_get_str (0, 16, (mpz_ptr) chan->data);

                new_cb->comp = (struct comp *) g_strdup_printf ("0x%s", strtmp);
                free (strtmp);
            }
        } else
        {
            char *typeString = TMPNodeToString (NULL, chan->typeNode);
            BalsaType *type;

            BalsaTypeParseFromString (typeString, 0, &type);

            FormatData *fd = NewFormatData (FormatBitCountToWordCount (chan->width));
            size_t count;
            if (channel[chan_num].width <= (sizeof (int) * 8))
            {
                fd->words[0] = (unsigned) chan->data;
                count = 1;
            } else
                mpz_export (fd->words, &count, -1, sizeof (unsigned), 0, 0, (mpz_ptr) chan->data);

            /* 0 fill remaining words */
            while (count < fd->wordCount)
            {
                fd->words[count] = 0;
                count++;
            }
            new_cb->comp = (struct comp *) PrintBalsaTypeDataIntoNewString (type, fd);
            DeleteFormatData (fd);
        }

        Trace_AppendSignal (new_cb, time);
    }
}
#endif

/****************************************************/

void BrzDebug_BeginFunc (char *funcname)
{
    if (WriteDebugAndAnimFiles)
        WriteDebugInfo_chans (g_strdup_printf ("-1 %s", funcname));
}

void BrzDebug_EndFunc (void)
{
    if (WriteDebugAndAnimFiles)
        WriteDebugInfo_chans (g_strdup ("-2 end_func"));
}

//static int current_group = 0;
//int groups[10000]; // CHECKME
//int nb_groups = 0;
// 
//void AddGroup (int num_new_group, char *name, int num_parent, int start_line, int end_line)
//{
//    if (num_parent == 0)
//        nb_groups = 0;
// 
//    while (nb_groups && num_parent != groups[nb_groups - 1])
//    {
//        if (WriteDebugAndAnimFiles)
//            WriteDebugInfo_chans (g_strdup ("-2 end_func"));
//        nb_groups--;
//    }
// 
//    if (WriteDebugAndAnimFiles)
//        WriteDebugInfo_chans (g_strdup_printf ("-1 %s(@%d-%d)", name, start_line, end_line));
//    groups[nb_groups] = num_new_group;
//    nb_groups++;
//}
// 
//void FinishGroups (void)
//{
//    while (nb_groups)
//    {
//        if (WriteDebugAndAnimFiles)
//            WriteDebugInfo_chans (g_strdup ("-2 end_func"));
//        nb_groups--;
//    }
//}

/****************************************************/

#ifdef EARLY_BACKEND
#  include "components_4phase_early.c"
#else
#  include "components_4phase_broad.c"
#endif

#include "components_4phase_eagerfvs.c"

/****************************************************/

struct BrzWireFork_
{
    STRUCT_COMP_BASE;
    int nb_outs;
};

void BrzWireFork_requp (struct comp *comp, int port)
{
    int i;
    struct BrzWireFork_ *c = (struct BrzWireFork_ *) comp;

    for (i = c->nb_outs - 1; i >= 0; i--)
    {
        TRACE_NEW_THREAD (i + 1, 0);
        SEND_REQUP_DELAYED (i + 1, 0 /*(i + 2) * BREEZE_CONTROL_DELAY */ );
    }
}
void BrzWireFork_ackup (struct comp *comp, int port)
{
    //    struct BrzWireFork_ *c = (struct BrzWireFork_ *) comp;

    fprintf (stderr, "BrzWireFork: ackup should never occur\n");
    //CloseBreezeSimulation (2);
}

void BrzWireFork_reqdown (struct comp *comp, int port)
{
    //    struct BrzWireFork_ *c = (struct BrzWireFork_ *) comp;

#ifndef HLA
    fprintf (stderr, "BrzWireFork: reqdown should never occur\n");
    CloseBreezeSimulation (2);
#endif // ifndef HLA
}

void BrzWireFork_ackdown (struct comp *comp, int port)
{
    //    struct BrzWireFork_ *c = (struct BrzWireFork_ *) comp;

    fprintf (stderr, "BrzWireFork: ackdown should never occur\n");
    CloseBreezeSimulation (2);
}

//void BrzWireFork (int num, int nb_outs, int in, ...)
void BrzWireFork (int num, char *args[])
{
    int nb_outs = atoi (args[0]);
    int i;

    INIT_COMP (BrzWireFork);
    INIT_COMP_PASSIVE_PORT (BrzWireFork, 0, atoi (args[1]));

    c->nb_outs = nb_outs;

    for (i = 0; i < nb_outs; i++)
    {
        INIT_COMP_ACTIVE_PORT (BrzWireFork, i + 1, atoi (args[i + 2]));
    }

    WRITE_DEBUG_INFO_COMP ("W^", 1 + c->nb_outs);
}

/****************************************************/

struct BrzInitVariable_
{
    STRUCT_COMP_BASE;

    int nb_read_ports;
    int width;
    struct BitField *read_ports_bit_fields;
    long data;
    int isInitialised;
    char *variableName;
    long initValue;

    mpz_t mpint_tmp;
};

void BrzInitVariable_requp (struct comp *comp, int port)
{
    struct BrzInitVariable_ *c = (struct BrzInitVariable_ *) comp;

    if (port == 0)
    {
        c->isInitialised = 1;
        if (c->width > (HOST_INTEGER_WIDTH_IN_BYTES * 8))
        {
            mpz_set ((mpz_ptr) c->data, (mpz_ptr) GET_PORT_DATA (0));
        } else
        {
            c->data = GET_PORT_DATA (0);
        }

        SEND_ACKUP (0);
    } else if (port == 1)
    {
        c->isInitialised = 1;
        if (c->width > (HOST_INTEGER_WIDTH_IN_BYTES * 8))
        {
            mpz_set ((mpz_ptr) c->data, (mpz_ptr) c->initValue);
        } else
        {
            c->data = c->initValue;
        }

        SEND_ACKUP (1);
    } else
    {
        if (c->isInitialised == 0)
        {
            char *msg = g_strdup_printf ("Uninitialised variable read (0) `%s' ",
              c->variableName);

            ReportError (comp->chan[port], msg);
            g_free (msg);
            c->isInitialised = 1;
        }
//        SET_PORT_DATA (port, c->data);
        // Slice the data to the correct bit field
        if (c->width > (HOST_INTEGER_WIDTH_IN_BYTES * 8))
        {
            mpz_tdiv_q_2exp (c->mpint_tmp, (mpz_ptr) c->data, c->read_ports_bit_fields[port - 2].low_index);
            mpz_tdiv_r_2exp (c->mpint_tmp, c->mpint_tmp, c->read_ports_bit_fields[port - 2].width);
            if (c->read_ports_bit_fields[port - 2].is_mpint)
            {
                SET_PORT_DATA (port, (long) c->mpint_tmp);
            } else
            {
                SET_PORT_DATA (port, mpz_get_ui (c->mpint_tmp));
            }
        } else
        {
            int data = c->data;
            int width = c->read_ports_bit_fields[port - 2].width;
            if (width != (sizeof (int) * 8))
                data = (data >> c->read_ports_bit_fields[port - 2].low_index) & ((1 << width) - 1);
            else
                data = (data >> c->read_ports_bit_fields[port - 2].low_index);

            SET_PORT_DATA (port, data);
        }

        SEND_DATAON_DELAYED (port, 0);
        SEND_ACKUP_DELAYED (port, 1);
    }
}
void BrzInitVariable_ackup (struct comp *comp, int port)
{
    //    struct BrzInitVariable_ *c = (struct BrzInitVariable_ *) comp;
}
void BrzInitVariable_reqdown (struct comp *comp, int port)
{
    struct BrzInitVariable_ *c = (struct BrzInitVariable_ *) comp;

    SEND_ACKDOWN_DELAYED (port, 0);

    if (port >= 2)
    {
        SEND_DATAOFF_DELAYED (port, 1);
    }
}

void BrzInitVariable_ackdown (struct comp *comp, int port)
{
    //    struct BrzInitVariable_ *c = (struct BrzInitVariable_ *) comp;
}

//void BrzInitVariable (int num, int width, int nb_read_ports, char *name, ...)
void BrzInitVariable (int num, char *args[])
{
    int width = atoi (args[0]);
    int nb_read_ports = atoi (args[1]);
    char *initValueStr = (char *) args[2];
    char *name = (char *) args[3];
    char *readPortsSpecString = (char *) args[4];
    int i;

    INIT_COMP (BrzInitVariable);
    INIT_COMP_PASSIVE_PORT (BrzInitVariable, 0, atoi (args[5]));

    if (width > (HOST_INTEGER_WIDTH_IN_BYTES * 8))
    {
        c->data = (long) malloc (sizeof (mpz_t));
        mpz_init ((mpz_ptr) c->data);
        mpz_set_ui ((mpz_ptr) c->data, 0);

        c->initValue = (long) malloc (sizeof (mpz_t));
        mpz_init ((mpz_ptr) c->initValue);
        mpz_set_str ((mpz_ptr) c->initValue, initValueStr, 10);
        mpz_init (c->mpint_tmp);
    } else
    {
        c->data = 0;
        c->initValue = atoi (initValueStr);
    }

    c->width = width;
    c->nb_read_ports = nb_read_ports;
    c->read_ports_bit_fields = decode_bit_fields (readPortsSpecString, nb_read_ports, width);
    c->isInitialised = 0;
    c->variableName = g_strdup (name + 1); /* Skip leading " */

    for (i = 1; i <= nb_read_ports + 1; i++)
    {
        int chan_num = atoi (args[i + 5]);

        INIT_COMP_PASSIVE_PORT (BrzInitVariable, i, chan_num);
        INIT_COMP_CONTROLLED_DATA_PORT (i, chan_num);
    }

    WRITE_DEBUG_INFO_COMP (name, 2 + c->nb_read_ports);
}

/****************************************************/

struct BrzContinuePush_
{
    STRUCT_COMP_BASE;
};

void BrzContinuePush_requp (struct comp *comp, int port)
{
    struct BrzContinuePush_ *c = (struct BrzContinuePush_ *) comp;

    SEND_ACKUP_DELAYED (0, BREEZE_DATAPATH_DELAY);
}

void BrzContinuePush_ackup (struct comp *comp, int port)
{
    //    struct BrzContinuePush_ *c = (struct BrzContinuePush_ *) comp;
}
void BrzContinuePush_reqdown (struct comp *comp, int port)
{
    struct BrzContinuePush_ *c = (struct BrzContinuePush_ *) comp;

    SEND_ACKDOWN (0);
}

void BrzContinuePush_ackdown (struct comp *comp, int port)
{
    //    struct BrzContinuePush_ *c = (struct BrzContinuePush_ *) comp;
}

//void BrzContinuePush (int num, int width, int in)
void BrzContinuePush (int num, char *args[])
{
    INIT_COMP (BrzContinuePush);
    INIT_COMP_PASSIVE_PORT (BrzContinuePush, 0, atoi (args[1]));

    WRITE_DEBUG_INFO_COMP ("ContPush", 1);
}

/****************************************************/

struct BrzHaltPush_
{
    STRUCT_COMP_BASE;
};

void BrzHaltPush_requp (struct comp *comp, int port)
{
    //    struct BrzHaltPush_ *c = (struct BrzHaltPush_ *) comp;
}

void BrzHaltPush_ackup (struct comp *comp, int port)
{
    //    struct BrzHaltPush_ *c = (struct BrzHaltPush_ *) comp;
}
void BrzHaltPush_reqdown (struct comp *comp, int port)
{
    //    struct BrzHaltPush_ *c = (struct BrzHaltPush_ *) comp;
}
void BrzHaltPush_ackdown (struct comp *comp, int port)
{
    //    struct BrzHaltPush_ *c = (struct BrzHaltPush_ *) comp;
}

//void BrzHaltPush (int num, int width, int in)
void BrzHaltPush (int num, char *args[])
{
    INIT_COMP (BrzHaltPush);
    INIT_COMP_PASSIVE_PORT (BrzHaltPush, 0, atoi (args[1]));

    WRITE_DEBUG_INFO_COMP ("HaltPush", 1);
}

/****************************************************/

struct BrzLoop_
{
    STRUCT_COMP_BASE;
};

void BrzLoop_requp (struct comp *comp, int port)
{
    struct BrzLoop_ *c = (struct BrzLoop_ *) comp;

    TRACE_FOLLOW_THREAD (1, 0);
    SEND_REQUP (1);
}

void BrzLoop_ackup (struct comp *comp, int port)
{
    struct BrzLoop_ *c = (struct BrzLoop_ *) comp;

    SEND_REQDOWN (1);
}

void BrzLoop_reqdown (struct comp *comp, int port)
{
    //    struct BrzLoop_ *c = (struct BrzLoop_ *) comp;
}
void BrzLoop_ackdown (struct comp *comp, int port)
{
    struct BrzLoop_ *c = (struct BrzLoop_ *) comp;

    SEND_REQUP (1);
}

//void BrzLoop (int num, int in, int out)
void BrzLoop (int num, char *args[])
{
    INIT_COMP (BrzLoop);
    INIT_COMP_PASSIVE_PORT (BrzLoop, 0, atoi (args[0]));
    INIT_COMP_ACTIVE_PORT (BrzLoop, 1, atoi (args[1]));

    WRITE_DEBUG_INFO_COMP ("*", 2);
}

/****************************************************/

struct BrzContinue_
{
    STRUCT_COMP_BASE;
};

void BrzContinue_requp (struct comp *comp, int port)
{
    struct BrzContinue_ *c = (struct BrzContinue_ *) comp;

    SEND_ACKUP (0);
}

void BrzContinue_ackup (struct comp *comp, int port)
{
    //    struct BrzContinue_ *c = (struct BrzContinue_ *) comp;
}
void BrzContinue_reqdown (struct comp *comp, int port)
{
    struct BrzContinue_ *c = (struct BrzContinue_ *) comp;

    SEND_ACKDOWN (0);
}

void BrzContinue_ackdown (struct comp *comp, int port)
{
    //    struct BrzContinue_ *c = (struct BrzContinue_ *) comp;
}

//void BrzContinue (int num, int in)
void BrzContinue (int num, char *args[])
{
    INIT_COMP (BrzContinue);
    INIT_COMP_PASSIVE_PORT (BrzContinue, 0, atoi (args[0]));

    WRITE_DEBUG_INFO_COMP ("Continue", 1);
}

/****************************************************/

struct BrzHalt_
{
    STRUCT_COMP_BASE;
};

void BrzHalt_requp (struct comp *comp, int port)
{
    //    struct BrzHalt_ *c = (struct BrzHalt_ *) comp;
}

void BrzHalt_ackup (struct comp *comp, int port)
{
    //    struct BrzHalt_ *c = (struct BrzHalt_ *) comp;
}
void BrzHalt_reqdown (struct comp *comp, int port)
{
    //    struct BrzHalt_ *c = (struct BrzHalt_ *) comp;
}
void BrzHalt_ackdown (struct comp *comp, int port)
{
    //    struct BrzHalt_ *c = (struct BrzHalt_ *) comp;
}

//void BrzHalt (int num, int in)
void BrzHalt (int num, char *args[])
{
    INIT_COMP (BrzHalt);
    INIT_COMP_PASSIVE_PORT (BrzHalt, 0, atoi (args[0]));

    WRITE_DEBUG_INFO_COMP ("Halt", 1);
}

/****************************************************/

struct BrzFork_
{
    STRUCT_COMP_BASE;
    int nb_outs;
    int nb_acknowledges;
    unsigned long long max_event_time;
};

void BrzFork_requp (struct comp *comp, int port)
{
    int i;
    struct BrzFork_ *c = (struct BrzFork_ *) comp;

    for (i = c->nb_outs - 1; i >= 0; i--)
    {
        TRACE_NEW_THREAD (i + 1, 0);
        SEND_REQUP_DELAYED (i + 1, 0 /*(i + 2) * BREEZE_CONTROL_DELAY */ );
    }
    c->nb_acknowledges = 0;
    c->max_event_time = 0;
}

void BrzFork_ackup (struct comp *comp, int port)
{
    struct BrzFork_ *c = (struct BrzFork_ *) comp;

    c->nb_acknowledges++;
    c->max_event_time = MAX (c->max_event_time, current_real_time);
    if (c->nb_acknowledges == c->nb_outs)
    {
        current_real_time = c->max_event_time;
        SEND_ACKUP (0);
    }
}
void BrzFork_reqdown (struct comp *comp, int port)
{
    int i;
    struct BrzFork_ *c = (struct BrzFork_ *) comp;

    for (i = c->nb_outs - 1; i >= 0; i--)
    {
        SEND_REQDOWN (i + 1);
    }
    c->nb_acknowledges = 0;
    c->max_event_time = 0;
}

void BrzFork_ackdown (struct comp *comp, int port)
{
    struct BrzFork_ *c = (struct BrzFork_ *) comp;

    c->nb_acknowledges++;
    c->max_event_time = MAX (c->max_event_time, current_real_time);
    if (c->nb_acknowledges == c->nb_outs)
    {
        current_real_time = c->max_event_time;
        SEND_ACKDOWN (0);
    }
}

//void BrzFork (int num, int nb_outs, int in, ...)
void BrzFork (int num, char *args[])
{
    int nb_outs = atoi (args[0]);
    int i;

    INIT_COMP (BrzFork);
    INIT_COMP_PASSIVE_PORT (BrzFork, 0, atoi (args[1]));

    c->nb_outs = nb_outs;

    for (i = 0; i < nb_outs; i++)
    {
        INIT_COMP_ACTIVE_PORT (BrzFork, i + 1, atoi (args[i + 2]));
    }

    WRITE_DEBUG_INFO_COMP (".", 1 + c->nb_outs);
}

/****************************************************/

struct BrzSynch_
{
    STRUCT_COMP_BASE;
    int nb_inputs;
    int nb_reqs;
    unsigned long long max_event_time;
};

void BrzSynch_requp (struct comp *comp, int port)
{
    struct BrzSynch_ *c = (struct BrzSynch_ *) comp;

    c->nb_reqs++;
    c->max_event_time = MAX (c->max_event_time, current_real_time);
    if (c->nb_reqs == c->nb_inputs)
    {
        current_real_time = c->max_event_time;
        TRACE_FOLLOW_THREAD (c->nb_inputs, 0);
        SEND_REQUP (c->nb_inputs);
    }
}
void BrzSynch_ackup (struct comp *comp, int port)
{
    int i;
    struct BrzSynch_ *c = (struct BrzSynch_ *) comp;

    for (i = c->nb_inputs - 1; i >= 0; i--)
    {
        SEND_ACKUP_DELAYED (i, 0 /*(2 + i) * BREEZE_CONTROL_DELAY */ );
    }
    c->nb_reqs = 0;
    c->max_event_time = 0;
}

void BrzSynch_reqdown (struct comp *comp, int port)
{
    struct BrzSynch_ *c = (struct BrzSynch_ *) comp;

    c->nb_reqs++;
    c->max_event_time = MAX (c->max_event_time, current_real_time);
    if (c->nb_reqs == c->nb_inputs)
    {
        current_real_time = c->max_event_time;
        SEND_REQDOWN_DELAYED (c->nb_inputs, c->nb_inputs * BREEZE_CONTROL_DELAY);
    }
}
void BrzSynch_ackdown (struct comp *comp, int port)
{
    int i;
    struct BrzSynch_ *c = (struct BrzSynch_ *) comp;

    for (i = c->nb_inputs - 1; i >= 0; i--)
    {
        SEND_ACKDOWN_DELAYED (i, 0 /*i * BREEZE_CONTROL_DELAY */ );
    }
    c->nb_reqs = 0;
    c->max_event_time = 0;
}

//void BrzSynch (int num, int nb_inputs, ...)  // n in, 1 out
void BrzSynch (int num, char *args[])
{
    int nb_inputs = atoi (args[0]);
    int i;

    INIT_COMP (BrzSynch);

    c->nb_inputs = nb_inputs;

    for (i = 0; i < nb_inputs; i++)
    {
        INIT_COMP_PASSIVE_PORT (BrzSynch, i, atoi (args[i + 1]));
    }

    INIT_COMP_ACTIVE_PORT (BrzSynch, nb_inputs, atoi (args[nb_inputs + 1]));

    c->nb_reqs = 0;
    c->max_event_time = 0;

    WRITE_DEBUG_INFO_COMP ("Synch", 1 + c->nb_inputs);
}

/****************************************************/

struct BrzCallActive_
{
    STRUCT_COMP_BASE;
    int nb_outs;
    int nb_acknowledges;
    unsigned long long max_event_time;

#ifdef REORDERED_EVENT_DRIVEN_SCHEDULER
    struct globalEventStruct reordered_req_event;
    struct callback_ reordered_req_cb;
#endif
};

void BrzCallActive_requp (struct comp *comp, int port)
{
    int i;
    struct BrzCallActive_ *c = (struct BrzCallActive_ *) comp;

    for (i = c->nb_outs - 1; i >= 0; i--)
    {
        SEND_REQUP_DELAYED (i + 1, 1 /*(i + 2) * BREEZE_CONTROL_DELAY */ );
    }
    c->nb_acknowledges = 0;
    c->max_event_time = 0;
}

void BrzCallActive_ackup (struct comp *comp, int port)
{
    struct BrzCallActive_ *c = (struct BrzCallActive_ *) comp;

    c->nb_acknowledges++;
    c->max_event_time = MAX (c->max_event_time, current_real_time);
    if (c->nb_acknowledges == 1)
    {
        current_real_time = c->max_event_time;
        SEND_ACKUP (0);
    }
}

#ifdef REORDERED_EVENT_DRIVEN_SCHEDULER
void BrzCallActive2_reqdown (struct comp *comp, int port);
void BrzCallActive_reqdown (struct comp *comp, int port)
{
    struct BrzCallActive_ *c = (struct BrzCallActive_ *) comp;

    InsertReorderedEventAtCurrentTime (&c->reordered_req_event);
}

void BrzCallActive2_reqdown (struct comp *comp, int port)
#else //#ifdef REORDERED_EVENT_DRIVEN_SCHEDULER
void BrzCallActive_reqdown (struct comp *comp, int port)
#endif                          //#ifdef REORDERED_EVENT_DRIVEN_SCHEDULER
{
    int i;
    struct BrzCallActive_ *c = (struct BrzCallActive_ *) comp;

    for (i = c->nb_outs - 1; i >= 0; i--)
    {
        SEND_REQDOWN_DELAYED (i + 1, 0);
    }
    c->nb_acknowledges = 0;
    c->max_event_time = 0;
}

void BrzCallActive_ackdown (struct comp *comp, int port)
{
    struct BrzCallActive_ *c = (struct BrzCallActive_ *) comp;

    c->nb_acknowledges++;
    c->max_event_time = MAX (c->max_event_time, current_real_time);
    if (c->nb_acknowledges == c->nb_outs)
    {
        current_real_time = c->max_event_time;
        SEND_ACKDOWN_DELAYED (0, 0);
    }
}

//void BrzCallActive (int num, int nb_outs, int in, ...)
void BrzCallActive (int num, char *args[])
{
    int nb_outs = atoi (args[0]);
    int i;

    fprintf (stderr, "A non-DI CallActive components is instantiated. The simulation can behave differently from the real circuit.\n");

    INIT_COMP (BrzCallActive);
    INIT_COMP_PASSIVE_PORT (BrzCallActive, 0, atoi (args[1]));

    c->nb_outs = nb_outs;

    for (i = 0; i < nb_outs; i++)
    {
        INIT_COMP_ACTIVE_PORT (BrzCallActive, i + 1, atoi (args[i + 2]));
    }

#ifdef REORDERED_EVENT_DRIVEN_SCHEDULER
    c->reordered_req_event.cb = &c->reordered_req_cb;
    c->reordered_req_cb.fct = BrzCallActive2_reqdown;
    c->reordered_req_cb.comp = (struct comp *) c;
    c->reordered_req_cb.portnum = 0;
#endif

    WRITE_DEBUG_INFO_COMP ("CallActive", 1 + c->nb_outs);
}

/****************************************************/

struct BrzPassivator_
{
    STRUCT_COMP_BASE;
    int nb_outputs;
    int nb_reqs;
    unsigned long long max_event_time;
};

void BrzPassivator_requp (struct comp *comp, int port)
{
    struct BrzPassivator_ *c = (struct BrzPassivator_ *) comp;

    c->nb_reqs++;
    c->max_event_time = MAX (c->max_event_time, current_real_time);
    if (c->nb_reqs == c->nb_outputs)
    {
        int i;

        current_real_time = c->max_event_time;
        for (i = c->nb_outputs - 1; i >= 0; i--)
        {
            SEND_ACKUP_DELAYED (i, BREEZE_DATAPATH_DELAY
              /*+ i * BREEZE_CONTROL_DELAY */ );
        }
        c->nb_reqs = 0;
        c->max_event_time = 0;
    }
}
void BrzPassivator_ackup (struct comp *comp, int port)
{
    //    struct BrzPassivator_ *c = (struct BrzPassivator_ *) comp;

}
void BrzPassivator_reqdown (struct comp *comp, int port)
{
    struct BrzPassivator_ *c = (struct BrzPassivator_ *) comp;

    c->nb_reqs++;
    c->max_event_time = MAX (c->max_event_time, current_real_time);
    if (c->nb_reqs == c->nb_outputs)
    {
        int i;

        current_real_time = c->max_event_time;
        for (i = c->nb_outputs - 1; i >= 0; i--)
        {
            SEND_ACKDOWN_DELAYED (i, 0 /*(i + 2) * BREEZE_CONTROL_DELAY */ );
        }
        c->nb_reqs = 0;
        c->max_event_time = 0;
    }
}
void BrzPassivator_ackdown (struct comp *comp, int port)
{
    //    struct BrzPassivator_ *c = (struct BrzPassivator_ *) comp;
}

//void BrzPassivator (int num, int nb_outputs, ...)
void BrzPassivator (int num, char *args[])
{
    int nb_outputs = atoi (args[0]);
    int i;

    INIT_COMP (BrzPassivator);

    c->nb_outputs = nb_outputs;

    for (i = 0; i < nb_outputs; i++)
    {
        INIT_COMP_PASSIVE_PORT (BrzPassivator, i, atoi (args[i + 1]));
    }

    c->nb_reqs = 0;
    c->max_event_time = 0;

    WRITE_DEBUG_INFO_COMP ("Passivator", 1 + c->nb_outputs);
}

/****************************************************/

struct BrzPassiveSyncPort2File_
{
    STRUCT_COMP_BASE;
    int width;
    FILE *file;
    char *name;
    char *filename;
    char *fformat;
    char *pformat;

    mpz_t mpint_tmp;
};

void BrzPassiveSyncPort2File_requp (struct comp *comp, int port)
{
    struct BrzPassiveSyncPort2File_ *c = (struct BrzPassiveSyncPort2File_ *) comp;

    SEND_ACKUP (0);
}

void BrzPassiveSyncPort2File_ackup (struct comp *comp, int port)
{
    struct BrzPassiveSyncPort2File_ *c = (struct BrzPassiveSyncPort2File_ *) comp;

    SEND_REQDOWN (0);
}

void BrzPassiveSyncPort2File_reqdown (struct comp *comp, int port)
{
    struct BrzPassiveSyncPort2File_ *c = (struct BrzPassiveSyncPort2File_ *) comp;

    SEND_ACKDOWN (0);
}

void BrzPassiveSyncPort2File_ackdown (struct comp *comp, int port)
{
    struct BrzPassiveSyncPort2File_ *c = (struct BrzPassiveSyncPort2File_ *) comp;

    SEND_REQUP (0);
}

void BrzPassiveSyncPort2File (int num, char *args[])
{
    int width = atoi (args[0]);
    char *filename = g_strdup ((char *) args[1]);
    char *name = g_strdup ((char *) args[2]);
    char *fformat = g_strdup ((char *) args[3]);
    int compnum = atoi (args[4]);
    FILE *filehandle = (FILE *) args[5];
    char *pformat = g_strdup ((char *) args[6]);

    INIT_COMP (BrzPassiveSyncPort2File);
    INIT_COMP_ACTIVE_PORT (BrzPassiveSyncPort2File, 0, compnum);

    c->width = width;
    c->name = name;
    c->filename = filename;
    c->fformat = fformat;
    c->pformat = pformat;
    c->file = filehandle;

    if (width > (HOST_INTEGER_WIDTH_IN_BYTES * 8))
        mpz_init (c->mpint_tmp);

    TRACE_INITIAL_THREAD (0);
    if (!HLA_activated) {
        SEND_REQUP_NODELAY (0);     /* BrzPassiveSyncPort2File_ackdown ((struct comp *) c, 0); */
    }
    else
    {
        SEND_REQUP_DELAYED (0, HLA_INITIALISATION_ENDING_TIME);
    }

    WRITE_DEBUG_INFO_COMP ("PassiveSyncPort2File", 1);
}

/****************************************************/
#ifdef MPI
extern int my_mpi_rank;
struct comp *MPI_comps[1000];
extern int MPI_nb_msg_sent;
int MPI_num = 0;

struct BrzMPI_
{
    STRUCT_COMP_BASE;
    int width;
    int partition1;
    int partition2;

    int MPI_comp_num;
    struct MPI_message message;
};

void BrzMPI_send_message (struct BrzMPI_ *c, int port, int type, int dest, gboolean send_data)
{
//4 printf ("Sending message\n");
    c->message.MPI_comp_num = c->MPI_comp_num;
    c->message.port = port;
    c->message.event_type = type;
    c->message.data_width = send_data ? c->width : 0;
    c->message.timestamp = current_real_time;
    MPI_Send (&c->message, sizeof (struct MPI_message), MPI_BYTE, dest, 0, MPI_COMM_WORLD);
//4 printf ("Finished sending message\n");

    MPI_nb_msg_sent++;

// printf ("1 %d %d\n", send_data, c->width);
    if (!send_data || !c->width)
        return;

    int width = ABS (c->width);

    if (width == 64)
    {
        fprintf (stderr, "64 bits data sent through MPI by MPI comp %d(%d)!!!!!\n", c->comp_num, c->MPI_comp_num);
    }
    if (width <= (HOST_INTEGER_WIDTH_IN_BYTES * 8))
    {
        MPI_Send (&c->chan[1 - port]->data, sizeof (unsigned long), MPI_BYTE, dest, 0, MPI_COMM_WORLD);
//4  printf ("Sending value %d\n", (int)c->chan[1-port]->data);
    } else
    {
        char strtmp[10000];

        mpz_get_str (strtmp, 16, (mpz_ptr) c->chan[1 - port]->data);
        int len = strlen (strtmp) + 1;

        MPI_Send (&len, 1, MPI_INT, dest, 0, MPI_COMM_WORLD);
//4  printf ("Sending big value length = %d\n", len);
        MPI_Send (strtmp, len, MPI_BYTE, dest, 0, MPI_COMM_WORLD);
//4  printf ("Sending big value = \"%s\"\n", strtmp);
    }
}

/****************************************************/

#define BrzMPISync_ BrzMPI_

void BrzMPISync_requp (struct comp *comp, int port)
{
    struct BrzMPI_ *c = (struct BrzMPI_ *) comp;

//4 printf ("MPISync %d (%d) requp (%d->%d, width=%d)\n", c->comp_num, c->MPI_comp_num, c->partition1, c->partition2, c->width);
    g_assert (port == 0);
    g_assert (my_mpi_rank == c->partition1);

    BrzMPI_send_message (c, 1 - port, REQUP, c->partition2, FALSE);
}

void BrzMPISync_ackup (struct comp *comp, int port)
{
    struct BrzMPI_ *c = (struct BrzMPI_ *) comp;

//4 printf ("MPISync %d ackup (%d->%d, width=%d)\n", c->comp_num, c->partition1, c->partition2, c->width);
    g_assert (port == 1);
    g_assert (my_mpi_rank == c->partition2);

    BrzMPI_send_message (c, 1 - port, ACKUP, c->partition1, FALSE);
}

void BrzMPISync_reqdown (struct comp *comp, int port)
{
    struct BrzMPI_ *c = (struct BrzMPI_ *) comp;

//4 printf ("MPISync %d reqdown (%d->%d, width=%d)\n", c->comp_num, c->partition1, c->partition2, c->width);
    g_assert (port == 0);
    g_assert (my_mpi_rank == c->partition1);

    BrzMPI_send_message (c, 1 - port, REQDOWN, c->partition2, FALSE);
}

void BrzMPISync_ackdown (struct comp *comp, int port)
{
    struct BrzMPI_ *c = (struct BrzMPI_ *) comp;

//4 printf ("MPISync %d ackdown (%d->%d, width=%d)\n", c->comp_num, c->partition1, c->partition2, c->width);
    g_assert (port == 1);
    g_assert (my_mpi_rank == c->partition2);

    BrzMPI_send_message (c, 1 - port, ACKDOWN, c->partition1, FALSE);
}

void BrzMPISync (int num, char *args[])
{
    int width = atoi (args[0]);
    int partition1 = atoi (args[1]) - 1;
    int partition2 = atoi (args[2]) - 1;

    INIT_COMP (BrzMPISync);
    INIT_COMP_PASSIVE_PORT (BrzMPISync, 0, atoi (args[3]));
    INIT_COMP_ACTIVE_PORT (BrzMPISync, 1, atoi (args[4]));

    if (width)
    {
        if (width > 0)
        {
            INIT_COMP_CONTROLLED_DATA_PORT (0, atoi (args[3]));
        } else
        {
            INIT_COMP_CONTROLLED_DATA_PORT (1, atoi (args[4]));
        }
    }

    c->width = width;
    c->partition1 = partition1;
    c->partition2 = partition2;

    c->MPI_comp_num = MPI_num;
    MPI_comps[MPI_num] = (struct comp *) c;
    MPI_num++;

    WRITE_DEBUG_INFO_COMP ("MPISync", 1);
}

/****************************************************/

#define BrzMPIPush_ BrzMPI_

void BrzMPIPush_requp (struct comp *comp, int port)
{
    struct BrzMPI_ *c = (struct BrzMPI_ *) comp;

//4 printf ("MPIPush %d (%d) requp (%d->%d, width=%d)\n", c->comp_num, c->MPI_comp_num, c->partition1, c->partition2, c->width);
    g_assert (port == 0);
    g_assert (my_mpi_rank == c->partition1);

    BrzMPI_send_message (c, 1 - port, REQUP, c->partition2, TRUE);
}

void BrzMPIPush_ackup (struct comp *comp, int port)
{
    struct BrzMPI_ *c = (struct BrzMPI_ *) comp;

//4 printf ("MPIPush %d ackup (%d->%d, width=%d)\n", c->comp_num, c->partition1, c->partition2, c->width);
    g_assert (port == 1);
    g_assert (my_mpi_rank == c->partition2);

    BrzMPI_send_message (c, 1 - port, ACKUP, c->partition1, FALSE);
}

void BrzMPIPush_reqdown (struct comp *comp, int port)
{
    struct BrzMPI_ *c = (struct BrzMPI_ *) comp;

//4 printf ("MPIPush %d reqdown (%d->%d, width=%d)\n", c->comp_num, c->partition1, c->partition2, c->width);
    g_assert (port == 0);
    g_assert (my_mpi_rank == c->partition1);

    BrzMPI_send_message (c, 1 - port, REQDOWN, c->partition2, FALSE);
}

void BrzMPIPush_ackdown (struct comp *comp, int port)
{
    struct BrzMPI_ *c = (struct BrzMPI_ *) comp;

//4 printf ("MPIPush %d ackdown (%d->%d, width=%d)\n", c->comp_num, c->partition1, c->partition2, c->width);
    g_assert (port == 1);
    g_assert (my_mpi_rank == c->partition2);

    BrzMPI_send_message (c, 1 - port, ACKDOWN, c->partition1, FALSE);
}

void BrzMPIPush (int num, char *args[])
{
    int width = atoi (args[0]);
    int partition1 = atoi (args[1]) - 1;
    int partition2 = atoi (args[2]) - 1;

    INIT_COMP (BrzMPIPush);
    INIT_COMP_PASSIVE_PORT (BrzMPIPush, 0, atoi (args[3]));
    INIT_COMP_ACTIVE_PORT (BrzMPIPush, 1, atoi (args[4]));

    if (width)
    {
        if (width > 0)
        {
            INIT_COMP_CONTROLLED_DATA_PORT (0, atoi (args[3]));
        } else
        {
            INIT_COMP_CONTROLLED_DATA_PORT (1, atoi (args[4]));
        }
    }

    c->width = width;
    c->partition1 = partition1;
    c->partition2 = partition2;

    c->MPI_comp_num = MPI_num;
    MPI_comps[MPI_num] = (struct comp *) c;
    MPI_num++;

    if (width > (HOST_INTEGER_WIDTH_IN_BYTES * 8))
    {
        mpz_t *ptr = malloc (sizeof (mpz_t));

        mpz_init ((mpz_ptr) ptr);
        GET_PORT_DATA (1) = (long) ptr;
    }

    WRITE_DEBUG_INFO_COMP ("MPIPush", 1);
}

/****************************************************/

#define BrzMPIPull_ BrzMPI_

void BrzMPIPull_requp (struct comp *comp, int port)
{
    struct BrzMPI_ *c = (struct BrzMPI_ *) comp;

//4 printf ("MPIPull %d (%d) requp (%d->%d, width=%d)\n", c->comp_num, c->MPI_comp_num, c->partition1, c->partition2, c->width);
    g_assert (port == 0);
    g_assert (my_mpi_rank == c->partition1);

    BrzMPI_send_message (c, 1 - port, REQUP, c->partition2, FALSE);
}

void BrzMPIPull_ackup (struct comp *comp, int port)
{
    struct BrzMPI_ *c = (struct BrzMPI_ *) comp;

//4 printf ("MPIPull %d ackup (%d->%d, width=%d)\n", c->comp_num, c->partition1, c->partition2, c->width);
    g_assert (port == 1);
    g_assert (my_mpi_rank == c->partition2);

    BrzMPI_send_message (c, 1 - port, ACKUP, c->partition1, TRUE);
}

void BrzMPIPull_reqdown (struct comp *comp, int port)
{
    struct BrzMPI_ *c = (struct BrzMPI_ *) comp;

//4 printf ("MPIPull %d reqdown (%d->%d, width=%d)\n", c->comp_num, c->partition1, c->partition2, c->width);
    g_assert (port == 0);
    g_assert (my_mpi_rank == c->partition1);

    BrzMPI_send_message (c, 1 - port, REQDOWN, c->partition2, FALSE);
}

void BrzMPIPull_ackdown (struct comp *comp, int port)
{
    struct BrzMPI_ *c = (struct BrzMPI_ *) comp;

//4 printf ("MPIPull %d ackdown (%d->%d, width=%d)\n", c->comp_num, c->partition1, c->partition2, c->width);
    g_assert (port == 1);
    g_assert (my_mpi_rank == c->partition2);

    BrzMPI_send_message (c, 1 - port, ACKDOWN, c->partition1, FALSE);
}

void BrzMPIPull (int num, char *args[])
{
    int width = atoi (args[0]);
    int partition1 = atoi (args[1]) - 1;
    int partition2 = atoi (args[2]) - 1;

    INIT_COMP (BrzMPIPull);
    INIT_COMP_PASSIVE_PORT (BrzMPIPull, 0, atoi (args[3]));
    INIT_COMP_ACTIVE_PORT (BrzMPIPull, 1, atoi (args[4]));

    if (width)
    {
        if (width > 0)
        {
            INIT_COMP_CONTROLLED_DATA_PORT (0, atoi (args[3]));
        } else
        {
            INIT_COMP_CONTROLLED_DATA_PORT (1, atoi (args[4]));
        }
    }

    c->width = width;
    c->partition1 = partition1;
    c->partition2 = partition2;

    c->MPI_comp_num = MPI_num;
    MPI_comps[MPI_num] = (struct comp *) c;
    MPI_num++;

    if (width > (HOST_INTEGER_WIDTH_IN_BYTES * 8))
    {
        GET_PORT_DATA (0) = (long) malloc (sizeof (mpz_t));
        mpz_init ((mpz_ptr) GET_PORT_DATA (0));
    }

    WRITE_DEBUG_INFO_COMP ("MPIPull", 1);
}
#endif // #ifdef MPI

/****************************************************/

/* BrzBuiltinFunction_ : component instance structure */
struct BrzBuiltinFunction_
{
    STRUCT_COMP_BASE;

    BuiltinFunction *function;
    BuiltinFunctionInstanceData *instanceData;

    int nbInputsReady;
    unsigned long long max_event_time;

#ifdef REORDERED_EVENT_DRIVEN_SCHEDULER
    struct globalEventStruct reordered_req_event;
    struct callback_ reordered_req_cb;
#endif
};

static void BrzBuiltinFunction_ackup (struct comp *comp, int port)
{
    int i;
    struct BrzBuiltinFunction_ *c = (struct BrzBuiltinFunction_ *) comp;

    c->nbInputsReady++;
    c->max_event_time = MAX (c->max_event_time, current_real_time);

    if (c->nbInputsReady >= c->function->arity)
    {
        current_real_time = c->max_event_time;
        /* Read port data, convert all to FormatDatas */
        for (i = 1; i <= c->function->arity; i++)
        {
            long portData = GET_PORT_DATA (i);

            if (c->instanceData->argumentWidths[i - 1] <= (HOST_INTEGER_WIDTH_IN_BYTES * 8))
            {
                c->instanceData->arguments[i - 1]->words[0] = portData;
            } else
            {
                FormatData *dest = c->instanceData->arguments[i - 1];
                size_t count;

                mpz_export (dest->words, &count, -1, sizeof (unsigned), 0, 0, (mpz_ptr) portData);
                /* 0 fill remaining words */
                while (count < dest->wordCount)
                {
                    dest->words[count] = 0;
                    count++;
                }
            }
        }
        /* Update the function's returned/retained objects */
        BalsaSim_BuiltinFunctionRenewObjects (c->function, c->instanceData);
        /* Then execute body functions */
        c->function->function (c->function, c->instanceData);
        /* and return the result into int if available&necessary */
        if (c->instanceData->retryRequested)
        {
#ifdef REORDERED_EVENT_DRIVEN_SCHEDULER
            InsertReorderedEventAfterLatestTime (&c->reordered_req_event);
            return;
#else
            fprintf (stderr, "Error: Retry requests not handled by standard scheduler\n");
            CloseBreezeSimulation (2);
#endif
        }

        if (c->instanceData->resultWidth <= (HOST_INTEGER_WIDTH_IN_BYTES * 8))
        {
            SET_PORT_DATA (0, c->instanceData->result->words[0]);
        } else
        {
            mpz_import ((mpz_ptr) GET_PORT_DATA (0), c->instanceData->result->wordCount, -1, sizeof (unsigned), 0, 0, c->instanceData->result->words);
        }

        SEND_DATAON_DELAYED (0, 0);
        SEND_ACKUP_DELAYED (0, 1);
    }
}
static void BrzBuiltinFunction_requp (struct comp *comp, int port)
{
    int i;
    struct BrzBuiltinFunction_ *c = (struct BrzBuiltinFunction_ *) comp;

    c->nbInputsReady = 0;
    c->max_event_time = 0;

    for (i = 1; i <= c->function->arity; i++)
    {
        // TRACE_NEW_THREAD (i, 0);
        SEND_REQUP (i);
    }

    /* 0 arguments ! */
    if (c->function->arity == 0)
    {
        BrzBuiltinFunction_ackup (comp, 0);
    }
}
static void BrzBuiltinFunction_reqdown (struct comp *comp, int port)
{
    int i;
    struct BrzBuiltinFunction_ *c = (struct BrzBuiltinFunction_ *) comp;

    c->nbInputsReady = 0;
    c->max_event_time = 0;

    for (i = 1; i <= c->function->arity; i++)
    {
        SEND_REQDOWN (i);
    }

    if (c->function->arity == 0)
    {
        SEND_ACKDOWN_DELAYED (0, 0);
        SEND_DATAOFF_DELAYED (0, 1);
    }
}
static void BrzBuiltinFunction_ackdown (struct comp *comp, int port)
{
    struct BrzBuiltinFunction_ *c = (struct BrzBuiltinFunction_ *) comp;

    c->nbInputsReady++;
    c->max_event_time = MAX (c->max_event_time, current_real_time);

    if (c->nbInputsReady == c->function->arity)
    {
        current_real_time = c->max_event_time;
        if (c->function->function_ackdown)
        {
            int i;

            /* Read port data, convert all to FormatDatas */
            for (i = 1; i <= c->function->arity; i++)
            {
                long portData = GET_PORT_DATA (i);

                if (c->instanceData->argumentWidths[i - 1] <= (HOST_INTEGER_WIDTH_IN_BYTES * 8))
                {
                    c->instanceData->arguments[i - 1]->words[0] = portData;
                } else
                {
                    FormatData *dest = c->instanceData->arguments[i - 1];
                    size_t count;

                    mpz_export (dest->words, &count, -1, sizeof (unsigned), 0, 0, (mpz_ptr) portData);
                    /* 0 fill remaining words */
                    while (count < dest->wordCount)
                    {
                        dest->words[count] = 0;
                        count++;
                    }
                }
            }
            /* Update the function's returned/retained objects */
            BalsaSim_BuiltinFunctionRenewObjects (c->function, c->instanceData);
            /* Then execute body functions */
            c->function->function_ackdown (c->function, c->instanceData);
            /* and return the result into int if necesary */
            if (c->instanceData->resultWidth <= (HOST_INTEGER_WIDTH_IN_BYTES * 8))
            {
                SET_PORT_DATA (0, c->instanceData->result->words[0]);
            } else
            {
                mpz_import ((mpz_ptr) GET_PORT_DATA (0),
                  c->instanceData->result->wordCount, -1, sizeof (unsigned), 0, 0, c->instanceData->result->words);
            }

        }
        SEND_ACKDOWN_DELAYED (0, 0);
        SEND_DATAOFF_DELAYED (0, 1);
    }
}

/* InstantiateBuiltinFunction : instantiate a builtin function from a BuiltinFunction
	structure and the usual num/args pair used with the other component instantiation functions.
	NB. Need to update this if the function style in components_4phase.[ch] changes */
void InstantiateBuiltinFunction (BuiltinFunction * function, int num, char *args[])
{
    /* args: args[0] is result, args[1..] are the arguments */
    int i;
    BalsaParameter **parameters = malloc (sizeof (BalsaParameter *) * function->parameterCount);

    INIT_COMP (BrzBuiltinFunction);

    c->function = function;

    for (i = 0; i < function->parameterCount; i++)
        parameters[i] = BalsaParameterParseFromString (args[i]);

    c->instanceData = NewBuiltinFunctionInstanceData (function, parameters);

    args += function->parameterCount;

    INIT_COMP_PASSIVE_PORT (BrzBuiltinFunction, 0, atoi (args[0]));
    INIT_COMP_CONTROLLED_DATA_PORT (0, atoi (args[0]));

    for (i = 1; i <= function->arity; i++)
    {
        INIT_COMP_ACTIVE_PORT (BrzBuiltinFunction, i, atoi (args[i]));

        if (c->instanceData->argumentWidths[i - 1] > (HOST_INTEGER_WIDTH_IN_BYTES * 8))
        {
            GET_PORT_DATA (i) = (long) g_new (mpz_t, 1);
            mpz_init ((mpz_ptr) GET_PORT_DATA (i));
        }
    }

    if (c->instanceData->resultWidth > (HOST_INTEGER_WIDTH_IN_BYTES * 8))
    {
        GET_PORT_DATA (0) = (long) g_new (mpz_t, 1);
        mpz_init ((mpz_ptr) GET_PORT_DATA (0));
    }
#ifdef REORDERED_EVENT_DRIVEN_SCHEDULER
    c->reordered_req_event.cb = &c->reordered_req_cb;
    c->reordered_req_cb.fct = BrzBuiltinFunction_ackup;
    c->reordered_req_cb.comp = (struct comp *) c;
    c->reordered_req_cb.portnum = 0;
#endif

}

/****************************************************/

#include "components_4phase_for_sam.c"
#include "components_4phase_hla.c"
