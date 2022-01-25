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

	`simulate.c'
	Simulation kernel

*/

#define DO_NOT_USE_CONSOLELOG

#include "simulate.h"
#include "breezesim_plugin.h"
#include "components_4phase.h"
#include <config.h>
#include <stdlib.h>
#include "balsasim/bstring.h"
#include "format/external.h"

extern gboolean NoDelayCoefs;
extern int StopAtTime;
extern gboolean StopAtTimeReached;

#ifdef MPI
extern int my_mpi_rank;
#endif

gboolean CtrlPipe = FALSE;
gboolean CtrlPipe_run;
int CtrlPipe_rununtil;
int CtrlPipe_speed;

unsigned long long current_real_time = 1;
unsigned long long last_event_time = 0;
int time_span = 0;

#ifdef DELAYS_WITH_ERROR
float current_error;
#endif

struct chan *channel;
int channelMax;

#if defined(EVENT_DRIVEN_SCHEDULER) || defined(REORDERED_EVENT_DRIVEN_SCHEDULER)

struct callback_ *globalNextEvent_cb = 0;
unsigned long long globalNextEvent_timestamp = 0;

struct globalEventStruct *globalEventArray = 0;
int globalEventArray_max = 0;
int globalEventArray_pos = 0;

#ifdef REORDERED_EVENT_DRIVEN_SCHEDULER
GList *reordered_events_list = 0;
int reordered_events_list_length = 0;
int sequential_execution_duration = 0;
#endif

#else

struct signal_stack_timeslot *signal_stack = NULL;

#endif

int DebugFlag = 0;
gboolean WriteDebugAndAnimFiles = FALSE;
FILE *TraceFile = NULL;
gboolean TraceAllChans = FALSE;
gboolean TraceNamedPortsOnly = FALSE;
gboolean VcdTraceNamedPortsOnly = FALSE;

char *get_static_vcd_code (int channum)
{
    static char vcd_wire_code_name[10];
    int pos = 0;

    do {
	int vcd_char_num = (channum%64)+33;
	vcd_wire_code_name[pos] = (char)vcd_char_num;
	pos++;
	channum /= 64;
    } while (channum);

    vcd_wire_code_name[pos] = 0;

    return vcd_wire_code_name;
}

static void freePortDecls (GList * portDecls)
{
    GList *tmp;

    portDecls = g_list_first (portDecls);
    tmp = portDecls;

    while (tmp)
    {
        struct portDecl *port = (struct portDecl *) tmp->data;

        if (port->name != NULL)
            g_free (port->name);
        if (port->typeFormat != NULL)
            g_free (port->typeFormat);
        g_free (port);
        tmp = g_list_next (tmp);
    }

    g_list_free (portDecls);
}

static GList *convertPortDecls (struct BreezePart *breezePart)
{
    GList *tmp;
    GList *portDecls = NULL;
    int portnum = 1;

    for (tmp = g_list_last (getBreezePartPortsList (breezePart)); tmp; tmp = tmp->prev)
    {
        struct BreezePartPortsPort *breezePort = tmp->data;
        struct portDecl *port = (struct portDecl *) calloc (1, sizeof (struct portDecl));

        //  printf ("addPortDecl: %s\n", breezePort->name);

        enum BreezePartPortsPortType breezePortType = getBreezePartPortsPortType (breezePort);

        port->name = getBreezePartPortsPortName (breezePort);

        switch (breezePortType)
        {
        case PassiveSyncPort:
            port->type_passive_active = 0;
            port->type_input_output = 2;
            break;
        case ActiveSyncPort:
            port->type_passive_active = 1;
            port->type_input_output = 2;
            break;
        case PassiveOutputPort:
            port->type_passive_active = 0;
            port->type_input_output = 1;
            break;
        case ActiveOutputPort:
            port->type_passive_active = 1;
            port->type_input_output = 1;
            break;
        case PassiveInputPort:
            port->type_passive_active = 0;
            port->type_input_output = 0;
            break;
        case ActiveInputPort:
            port->type_passive_active = 1;
            port->type_input_output = 0;
            break;
		default:
		fprintf (stderr, "Shsdhsdjould have hadnldfkl;jsdfkljsdfkl AB %d\n", breezePortType);
			break;
        }

/* AB2007 */
        if (port->type_input_output != 2)
        {
#if 0
            PtrTMPNode dataTypeNode = getBreezePartPortsPortDataTypeNode (breezePort);
            GList *typeList = dataTypeNode->body.list;
            port->typeWidth = atoi (((PtrTMPNode) typeList->next->data)->body.string);
            port->typeFormat = ((PtrTMPNode) typeList->next->next->data)->body.string;
#endif
			port->typeWidth = 0;
			port->typeFormat = NULL;
        }

        port->portnum = portnum++;
        portDecls = g_list_prepend (portDecls, port);
    }

    return portDecls;
}

void CreateUndefinedInterfacePortComponents (GList * portDecls)
{
    GList *tmp;

    for (tmp = g_list_last (portDecls); tmp; tmp = tmp->prev)
    {
        struct portDecl *port = tmp->data;
        char *args[7];

        if (port->type_passive_active != Passive || port->type_input_output != Sync)
        {
            fprintf (stderr, "breeze:sim: only passive sync ports (such as `activate') can remain" " unconnected on top-level part.\n");
            exit (EXIT_FAILURE);
        }
#ifdef MPI
        // In MPI, only create the active port on the master
        if (port->portnum == 1 && my_mpi_rank != 0)
            continue;
#endif

        args[0] = g_strdup_printf ("%d", port->typeWidth); // needs freeing afterwards
        args[1] = "-";
        args[2] = port->name;
        args[3] = g_strdup_printf ("%s", port->name);
        args[4] = g_strdup_printf ("%d", port->portnum); // needs freeing afterwards
        args[5] = (void *) stdin; // necessary ?
        args[6] = g_strdup_printf ("Sync %s", port->name);

        if (DebugFlag)
        {
            printf ("Creating BrzPassiveSyncPort2File component (%d) `%s\'\n", port->portnum, port->name);
        }

        BrzPassiveSyncPort2File (-1, args);
        port->instantiated = 1;

        g_free (args[0]);
        g_free (args[3]);
        g_free (args[4]);
        g_free (args[6]);
    }
}

static int countArgs (int nbArgs, GList * list)
{
    GList *tmp;

    for (tmp = list; tmp; tmp = tmp->next)
    {
        PtrTMPNode node = (PtrTMPNode) tmp->data;

        switch (node->type)
        {
        case TMPNumberStr:
        case TMPString:
        case TMPSymbol:
        case TMPBoolean:
            nbArgs++;
            break;
        case TMPList:
            if (TMPIsHeaded (node, NULL))
            {
                /* Handle formatted-type as a list of [i] = "formatted-type" [i+1] = "some number" [i+2] = "whatever"
                   instead of an interesting structure, FIXME */
                if (TMPIsHeaded (node, "formatted-type"))
                    nbArgs = countArgs (nbArgs, node->body.list);
                else
                    nbArgs++;
            } else
                nbArgs = countArgs (nbArgs, node->body.list);
            break;
        }
    }
    return nbArgs;
}

static int fillArgs (char **args, int nbArgs, GList * list)
{
    GList *tmp;

    for (tmp = list; tmp; tmp = tmp->next)
    {
        PtrTMPNode node = (PtrTMPNode) tmp->data;

        switch (node->type)
        {
        case TMPNumberStr:
            args[nbArgs] = g_strdup (node->body.string);
            nbArgs++;
            break;
        case TMPString:
        case TMPSymbol:
            args[nbArgs] = g_strdup_printf ("\"%s", node->body.string);
            nbArgs++;
            break;
        case TMPBoolean:
            args[nbArgs] = g_strdup_printf ("#%c", (node->body.boolean ? 't' : 'f'));
            nbArgs++;
            break;
        case TMPList:
            if (TMPIsHeaded (node, NULL))
            {
                /* See above, FIXME */
                if (TMPIsHeaded (node, "formatted-type"))
                    nbArgs = fillArgs (args, nbArgs, node->body.list);
                else
                {
                    char *symbol = ((PtrTMPNode) (node->body.list)->data)->body.string;
                    int symbolLength = strlen (symbol);

                    /* Check if this is a type decl */
                    if (symbolLength > 5 && strcmp (symbol + symbolLength - 5, "-type") == 0)
                    {
                        /* OK, this really isn't the way to do it, but processing a type
                           string is already supported in libbalsasim */
                        char *typeString = TMPNodeToString (NULL, node);

                        args[nbArgs] = typeString;
                    }
                    nbArgs++;
                }
            } else
                nbArgs = fillArgs (args, nbArgs, node->body.list);
            break;
        }
    }
    return nbArgs;
}

static void freeArgs (char **args, int nbArgs)
{
    int i;

    for (i = 0; i < nbArgs; i++)
        g_free (args[i]);

    g_free (args);
}

/* InitialiseBreezeTypes : register all the types read in from the Breeze file */
void InitialiseBreezeTypes (GList * types)
{
    while (types)
    {
        TMPNode *type = getBreezeTypeTMPNode (types->data);
        char *typeName = getBreezeTypeName (types->data);
        bool error = false;

        if (!TMPIsHeaded (type, "type"))
            error = true;
        else
        {
            GList *typeListElements = type->body.list;
            TMPNode *typeDefn = ((TMPNode *) g_list_nth (typeListElements, 2)->data);
            char *typeString = TMPNodeToString (NULL, typeDefn);
            BalsaType *type;

            if (typeString && BalsaTypeParseFromString (typeString, 0, &type) && type)
            {
                if (type->name)
                    type = BalsaTypeAlias (typeName, type);
                else
                    type->name = typeName;

                if (DebugFlag)
                    fprintf (stderr, "Interning type: %s\n", type->name);

                BalsaInternType (type);

                free (typeString);
            } else
                error = true;
        }

        if (error)
        {
            fprintf (stderr, "Invalid type declaration: ");
            PrintTMPNode (stderr, type);
            fprintf (stderr, "\n");
        }

        types = g_list_next (types);
    }
}

GList *componentPostInitialisationCallbacks = NULL;
void AddComponentPostInitialisation (_FCT2 fct, struct comp *comp, int optional_data)
{
    struct callback_ *cb = g_new0 (struct callback_, 1);

    cb->fct = fct;
    cb->comp = comp;
    cb->portnum = optional_data;

    componentPostInitialisationCallbacks = g_list_prepend (componentPostInitialisationCallbacks, cb);
}

void ProcessPortInitialisationCallbacks (void)
{
    GList *tmp = componentPostInitialisationCallbacks;

    for (; tmp; tmp=tmp->next) {
        struct callback_ *cb = tmp->data;
        cb->fct (cb->comp, cb->portnum);
    }

    g_list_free (componentPostInitialisationCallbacks);
    componentPostInitialisationCallbacks = NULL;
}

void InitialiseBreezeComponents (struct BreezePart *breezePart)
{
    GList *tmp = getBreezePartComponentsList (breezePart);

    int compNum = 0;
    int numComp = g_list_length (tmp);

    if (DebugFlag)
        printf ("Initialising %d components\n", numComp);

    // Create components
    for (tmp = g_list_last (tmp); tmp; tmp = tmp->prev)
    {
        struct BreezePartComponentsComponent *comp = tmp->data;

        char *compname = getBreezePartComponentsComponentName (comp);
        PtrTMPNode node = getBreezePartComponentsComponentTMPNode (comp);

        // Create the list of args
        int nbArgs = countArgs (0, node->body.list->next->next);
        char **args = (char **) calloc (nbArgs, sizeof (char *));

        fillArgs (args, 0, node->body.list->next->next);

#define PROCESS_COMPONENT(compName) \
      if (!strcmp (compname, "$" #compName))  { compName(compNum, args); compNum++; }

        PROCESS_COMPONENT (BrzAdapt)
        else
        PROCESS_COMPONENT (BrzBinaryFunc)
        else
        PROCESS_COMPONENT (BrzBinaryFuncPush)
        else
        PROCESS_COMPONENT (BrzBinaryFuncConstR)
        else
        PROCESS_COMPONENT (BrzBinaryFuncConstRPush)
        else
        PROCESS_COMPONENT (BrzCall)
        else
        PROCESS_COMPONENT (BrzCallMux)
        else
        PROCESS_COMPONENT (BrzCallDemux)
        else
        PROCESS_COMPONENT (BrzCase)
        else
        PROCESS_COMPONENT (BrzCaseFetch)
        else
        PROCESS_COMPONENT (BrzCombine)
        else
        PROCESS_COMPONENT (BrzCombineEqual)
        else
        PROCESS_COMPONENT (BrzConcur)
        else
        PROCESS_COMPONENT (BrzConstant)
        else
        PROCESS_COMPONENT (BrzDecisionWait)
        else
        PROCESS_COMPONENT (BrzEncode)
        else
        PROCESS_COMPONENT (BrzFalseVariable)
        else
        PROCESS_COMPONENT (BrzPassiveEagerFalseVariable)
        else
        PROCESS_COMPONENT (BrzActiveEagerFalseVariable)
        else
        PROCESS_COMPONENT (BrzPassiveSyncEagerFalseVariable)
        else
        PROCESS_COMPONENT (BrzPassiveEagerNullAdapt)
        else
        PROCESS_COMPONENT (BrzActiveEagerNullAdapt)
        else
        PROCESS_COMPONENT (BrzFetch)
        else
        PROCESS_COMPONENT (BrzLoop)
        else
        PROCESS_COMPONENT (BrzPassivatorPush)
        else
        PROCESS_COMPONENT (BrzSequence)
        else
        PROCESS_COMPONENT (BrzSlice)
        else
        PROCESS_COMPONENT (BrzSynch)
        else
        PROCESS_COMPONENT (BrzUnaryFunc)
        else
        PROCESS_COMPONENT (BrzUnaryFuncPush)
        else
        PROCESS_COMPONENT (BrzVariable)
        else
        PROCESS_COMPONENT (BrzInitVariable)
        else
        PROCESS_COMPONENT (BrzContinuePush)
        else
        PROCESS_COMPONENT (BrzForkPush)
        else
        PROCESS_COMPONENT (BrzCallDemuxPush)
        else
        PROCESS_COMPONENT (BrzCallActive)
        else
        PROCESS_COMPONENT (BrzSynchPush)
        else
        PROCESS_COMPONENT (BrzArbiter)
        else
        PROCESS_COMPONENT (BrzBar)
        else
        PROCESS_COMPONENT (BrzContinue)
        else
        PROCESS_COMPONENT (BrzFork)
        else
        PROCESS_COMPONENT (BrzNullAdapt)
        else
        PROCESS_COMPONENT (BrzSplit)
        else
        PROCESS_COMPONENT (BrzSplitEqual)
        else
        PROCESS_COMPONENT (BrzWhile)
        else
        PROCESS_COMPONENT (BrzWireFork)
        else
        PROCESS_COMPONENT (BrzHalt)
        else
        PROCESS_COMPONENT (BrzHaltPush)
        else
        PROCESS_COMPONENT (BrzPassivator)
        else
        PROCESS_COMPONENT (BrzSynchPull)
        else
        PROCESS_COMPONENT (BrzBuiltinVariable)

#ifdef MPI
		else
        PROCESS_COMPONENT (BrzMPISync)
        else
        PROCESS_COMPONENT (BrzMPIPush)
        else
        PROCESS_COMPONENT (BrzMPIPull)
#endif
		else
        PROCESS_COMPONENT (BrzFetchPush)
        else
        PROCESS_COMPONENT (BrzFetchReject)

//#ifdef HLA
		else if (!strcmp (compname, "cosim_interface_for_channel__active_sync"))
		{ BrzHLA_active_sync (compNum, args); compNum++; }
		else if (!strcmp (compname, "cosim_interface_for_channel__active_push_2_1_1_1"))
		{ BrzHLA_active_push (compNum, args); compNum++; }
		else if (!strcmp (compname, "cosim_interface_for_channel__active_push_2x2_1_1_1"))
		{ BrzHLA_active_push (compNum, args); compNum++; }
		else if (!strcmp (compname, "cosim_interface_for_channel__active_push_9_1_1_1"))
		{ BrzHLA_active_push (compNum, args); compNum++; }
		else if (!strcmp (compname, "cosim_interface_for_channel__active_push_2_1_2_1"))
		{ BrzHLA_active_push (compNum, args); compNum++; }
		else if (!strcmp (compname, "cosim_interface_for_channel__active_push_2_1_32_1"))
		{ BrzHLA_active_push (compNum, args); compNum++; }
		else if (!strcmp (compname, "cosim_interface_for_channel__passive_sync"))
		{ BrzHLA_passive_sync (compNum, args); compNum++; }
		else if (!strcmp (compname, "cosim_interface_for_channel__passive_push_2_1_32_1"))
		{ BrzHLA_passive_push (compNum, args); compNum++; }
		else if (!strcmp (compname, "cosim_interface_for_channel__passive_push_2_1_1_1"))
		{ BrzHLA_passive_push (compNum, args); compNum++; }
		else if (!strcmp (compname, "cosim_interface_for_channel__passive_push_9_1_1_1"))
		{ BrzHLA_passive_push (compNum, args); compNum++; }
		else if (!strcmp (compname, "cosim_interface_for_channel__passive_push_2x9_1_1_1"))
		{ BrzHLA_passive_push (compNum, args); compNum++; }
		else if (!strcmp (compname, "cosim_interface_for_channel__passive_push_2_1_37_1"))
		{ BrzHLA_passive_push (compNum, args); compNum++; }
		else if (!strcmp (compname, "cosim_interface__initialisation_and_runtime"))
		{ BrzHLA_init (compNum, args); compNum++; }
//#endif

        else
        {
            BuiltinFunction *function = BalsaSim_FindBuiltinFunction (compname);

            if (function)
            {
                InstantiateBuiltinFunction (function, compNum, args);
                compNum++;
            } else
            {
                printf ("Unknown component : %s\n", compname);
                exit (-2);
            }
        }

        freeArgs (args, nbArgs);
    }

    ProcessPortInitialisationCallbacks ();
}

void InitialiseComponentDelayCoefs (struct BreezePart *breezePart)
{
    /* Search for "balsa" top level callcontext */

    int balsaCallcontextNum = 0;
    GList *callcontexts;

    for (callcontexts = getBreezePartCallcontextsList (breezePart); callcontexts; callcontexts = callcontexts->next)
    {
        struct BreezePartCallcontextsCallcontext *callcontext = callcontexts->data;
        char *name = getBreezePartCallcontextsCallcontextName (callcontext);
        int parentnum = getBreezePartCallcontextsCallcontextParentNum (callcontext);

        if (parentnum == 0 && !strcmp (name, "balsa"))
        {
            balsaCallcontextNum = getBreezePartCallcontextsCallcontextNum (callcontext);
            break;
        }
    }

    if (!balsaCallcontextNum)
        return;

    /* Search for a unique non-TEST_HARNESS callcontext directly under balsaCallcontext */

    int mainCallcontextNum = 0;

    for (callcontexts = getBreezePartCallcontextsList (breezePart); callcontexts; callcontexts = callcontexts->next)
    {
        struct BreezePartCallcontextsCallcontext *callcontext = callcontexts->data;
        char *name = getBreezePartCallcontextsCallcontextName (callcontext);
        int parentnum = getBreezePartCallcontextsCallcontextParentNum (callcontext);

        if (parentnum == balsaCallcontextNum && !BEGINS_WITH (name, "Balsa"))
        {
            if (mainCallcontextNum)
                return;
            mainCallcontextNum = getBreezePartCallcontextsCallcontextNum (callcontext);
        }
    }

    if (!mainCallcontextNum)
        return;

    /* Adjust the delay coef of every component under this callcontext */
    if (!NoDelayCoefs)
    {
        GList *tmp = getBreezePartChannelsList (breezePart);
        int i = 0;

        for (tmp = g_list_last (tmp); tmp; tmp = tmp->prev)
        {
            struct BreezePartChannelsChannel *chan = tmp->data;
            struct Position *pos = getBreezePartChannelsChannelPosition (chan);

            i++;
            if (isBreezePartCallcontextsCallcontextNumChildOf (breezePart, pos->context, mainCallcontextNum))
            {
                if (channel[i].requp.comp)
                    channel[i].requp.comp->delay_coef = 100;
                if (channel[i].ackup.comp)
                    channel[i].ackup.comp->delay_coef = 100;
            }
        }
    }
}

static char *GetContextPrefixName_subfct (int context, struct BreezePart *breezePart)
{
    GList *callcontexts = getBreezePartCallcontextsList (breezePart);

    for (; callcontexts; callcontexts = callcontexts->next)
    {
        struct BreezePartCallcontextsCallcontext *callcontext = callcontexts->data;
        int numContext = getBreezePartCallcontextsCallcontextNum (callcontext);

        if (numContext == context)
        {
            char *prefix = getBreezePartCallcontextsCallcontextName (callcontext);
            int ParentNum = getBreezePartCallcontextsCallcontextParentNum (callcontext);

            if (ParentNum)
                return g_strdup_printf ("%s.%s", GetContextPrefixName_subfct (ParentNum, breezePart) ? : "", prefix);
            else
                return g_strdup_printf ("%s", prefix);
        }
    }
    return NULL;
}

static char *GetContextPrefixName (int context, struct BreezePart *breezePart)
    // Return (and create if necessary) a char* associated to the number context.
{
    static GHashTable *hashtable = NULL;
    int *key;
    gpointer value;

    if (hashtable == NULL)
        hashtable = g_hash_table_new (g_int_hash, g_int_equal);

    value = g_hash_table_lookup (hashtable, (gconstpointer) & context);

    if (value == NULL)
    {
        key = g_new0 (int, 1);

        *key = context;
        value = (gpointer) GetContextPrefixName_subfct (context, breezePart);
        g_hash_table_insert (hashtable, (gpointer) key, value);
    }

    return (char *) value;
}

void InitialiseBreezeChannels (struct BreezePart *breezePart)
{
    int i;
    GList *tmp = getBreezePartChannelsList (breezePart);

    int nbChans = g_list_length (tmp);

    if (DebugFlag)
        printf ("Initialising %d channels\n", nbChans);

    channel = (struct chan *) g_new0 (struct chan, nbChans + 1);

    channelMax = nbChans + 1;

    if (TraceFile)
    {
        if (TraceAllChans)
        {
            for (i = 0; i <= nbChans; i++)
                channel[i].traceType = TRACETYPE_HHH | TRACETYPE_VCD;
        }
        {
            // Trace every procedure port channel
            GList *callcontexts = getBreezePartCallcontextsList (breezePart);

            for (; callcontexts; callcontexts = callcontexts->next)
            {
                struct BreezePartCallcontextsCallcontext *callcontext = callcontexts->data;
                GList *ports = getBreezePartCallcontextsCallcontextChannelList (callcontext);

                for (; ports; ports = ports->next)
                {
                    long channum = (long) ports->data;

                    //      printf("port=%d\n",channum);
                    if (channum >= 0)
                    {
                        channel[channum].traceType = TRACETYPE_HHH | TRACETYPE_VCD;
                        channel[channum].name = g_strdup_printf ("->%s", getBreezePartCallcontextsCallcontextName (callcontext));
                        channel[channum].isPort = TRUE;
                    } else if (DebugFlag)
                        fprintf (stderr, "Warning: port channum is -1\n");
                }
            }

            // Trace ports of the main function
            int nbPorts = g_list_length (getBreezePartPortsList (breezePart));

            for (i = 1; i <= nbPorts; i++)
            {
                channel[i].traceType = TRACETYPE_HHH | TRACETYPE_VCD;
                channel[i].name = g_strdup_printf ("-><Main>");
                channel[i].isPort = TRUE;
            }
        }
    }
    // Console-trace ports of the function connected to test-harness comps (try to autodetect the user function inside the balsa-make-test generated function called "balsa")
    {
        GList *callcontexts;
        int balsa_contextNum = 0;

        for (callcontexts = getBreezePartCallcontextsList (breezePart); callcontexts; callcontexts = callcontexts->next)
        {
            struct BreezePartCallcontextsCallcontext *callcontext = callcontexts->data;
            char *partName = getBreezePartCallcontextsCallcontextName (callcontext);

            if (!strcmp (partName, "balsa"))
            {
                balsa_contextNum = getBreezePartCallcontextsCallcontextNum (callcontext);
                break;
            }
        }

        if (balsa_contextNum)
            for (callcontexts = getBreezePartCallcontextsList (breezePart); callcontexts; callcontexts = callcontexts->next)
            {
                struct BreezePartCallcontextsCallcontext *callcontext = callcontexts->data;
                char *partName = getBreezePartCallcontextsCallcontextName (callcontext);
                int parentNum = getBreezePartCallcontextsCallcontextParentNum (callcontext);

                if (parentNum == balsa_contextNum && !BEGINS_WITH (partName, "Balsa"))
                {
                    GList *ports = getBreezePartCallcontextsCallcontextChannelList (callcontext);

                    for (; ports; ports = ports->next)
                    {
                        long channum = (long) ports->data;

#ifndef DO_NOT_USE_CONSOLELOG
                        channel[channum].traceType |= TRACETYPE_CONSOLELOG;
#else
                        /* To avoid the "unused variable" compiler warning */
                        channum++;
#endif
                    }
                    break;
                }
            }

        i = 0;
        for (tmp = g_list_last (tmp); tmp; tmp = tmp->prev)
        {
            struct BreezePartChannelsChannel *chan = tmp->data;

            int chanWidth = getBreezePartChannelsChannelWidth (chan);

            i++;
            channel[i].width = chanWidth;
            channel[i].typeNode = getBreezePartChannelsChannelTypeNode (chan);

            switch (getBreezePartChannelsChannelType (chan))
            {
            case SyncChannel:
                channel[i].dataDirection = DataDir_Sync;
                break;
            case PushChannel:
                channel[i].dataDirection = DataDir_Push;
                break;
            case PullChannel:
                channel[i].dataDirection = DataDir_Pull;
                break;
            }

            if (channel[i].traceType != TRACETYPE_NONE)
            {
                char *chanName = getBreezePartChannelsChannelName (chan);

                if (!chanName || !*chanName || *chanName == ':')
                {
                    channel[i].traceType &= ~TRACETYPE_CONSOLELOG;
                    if (TraceNamedPortsOnly)
                        channel[i].traceType &= ~TRACETYPE_HHH;
                    if (VcdTraceNamedPortsOnly)
                        channel[i].traceType &= ~TRACETYPE_VCD;
                }
                if (channel[i].traceType == TRACETYPE_NONE)
                    continue;

                //                if (channel[i].name)
                //                    printf ("Warning in InitialiseBreezeChannels: channel name already assigned (%s -> %s)\n",
                //                      channel[i].name, chanName);

                int context = getBreezePartChannelsChannelPosition (chan)->context;
                char *contextPrefixName = GetContextPrefixName (context, breezePart);

                channel[i].name = g_strdup_printf ("%s%s.%s", contextPrefixName ? : "", channel[i].name ? : "", chanName ? : "");
                PtrTMPNode typeNode = getBreezePartChannelsChannelTypeNode (chan);

                if (channel[i].traceType & TRACETYPE_HHH)
                {
		    // print HHH part
                    fprintf (TraceFile, "\n$comment\ninit-channel %d %d \"%s\"", i, channel[i].width, channel[i].name ? : "");

                    if (typeNode)
                    {
                        fprintf (TraceFile, " ");
                        char *typeString = TMPNodeToString (NULL, typeNode);
                        BalsaType *type;

                        BalsaTypeParseFromString (typeString, 0, &type);
                        BalsaTypeMakeDefaultFormat (type);
                        FormatPrintFormat (TraceFile, type->format);
                    }

                    fprintf (TraceFile, " %s\n$end\n", channel[i].isPort ? "isPort" : "isNoPort");
		}

                if (channel[i].traceType & TRACETYPE_VCD)
		{ // print VCD part
		    int channum = i;
		    int chanwidth = channel[i].width;
		    // print to VCD: ctrl part
		    int vcd_wire_width = chanwidth;
		    char *vcd_wire_code_name = get_static_vcd_code (2*(channum-1));
		    char *vcd_wire_real_name = channel[i].name;
			
		    fprintf (TraceFile, "$var wire 1 %s %s $end\n", vcd_wire_code_name, vcd_wire_real_name);
			
		    // print to VCD: data part
		    if (chanwidth) {
			int vcd_wire_width = chanwidth;
			vcd_wire_code_name = get_static_vcd_code (2*(channum-1)+1);
			fprintf (TraceFile, "$var wire %d %s %s_data $end\n", vcd_wire_width, vcd_wire_code_name, vcd_wire_real_name);
		    }
		}
            }
        }

    }
}

unsigned long long ProcessDelayCoef (unsigned long long delay, int coef)
{
    if (coef)
        delay *= (unsigned long long) coef;

    if (delay != 0 && (coef == 100 || coef == 1000))
    {
        unsigned long long adjustment = ((current_real_time + delay + 1) % (unsigned long long) coef) - 1;

        delay -= adjustment;
    }
    return delay;
}

#if defined(EVENT_DRIVEN_SCHEDULER) || defined(REORDERED_EVENT_DRIVEN_SCHEDULER)

void InsertEventAtLaterTime (struct callback_ *cb, unsigned long long delay, int coef)
{
    delay = ProcessDelayCoef (delay, coef);

    if (StopAtTime && ((current_real_time + delay) > StopAtTime))
    {
        StopAtTimeReached = TRUE;
        return;
    }

    if (globalEventArray_pos == globalEventArray_max)
    {
        if (globalEventArray == 0)
        {
            globalEventArray_max = 1024;
            globalEventArray = g_new (struct globalEventStruct, globalEventArray_max);
        } else
        {
            globalEventArray_max *= 2;
            globalEventArray = g_renew (struct globalEventStruct, globalEventArray, globalEventArray_max);
        }
    }

    globalEventArray[globalEventArray_pos].cb = cb;
    globalEventArray[globalEventArray_pos].timestamp = current_real_time + delay;
    globalEventArray_pos++;
}

#else

void InsertEventAtLaterTime (struct callback_ *cb, unsigned long long delay, int coef)
{
    delay = ProcessDelayCoef (delay, coef);

#define INIT_NUM_SLOTS 64       /* must be non-zero and +ve */
#define INIT_SLOT_DEPTH 128     /* must be non-zero and +ve */

    static unsigned max_slot_depth = INIT_SLOT_DEPTH;

    //    int slot = (time_span == 0) ? 0 : (current_real_time + delay) % time_span;

    if (!cb || !cb->fct)
    {
        fprintf (stderr, "ERROR: %s: null callback inserted at time=%lld\n", __FUNCTION__, current_real_time);
#ifdef sparc
        printstack (fileno (stderr));
#endif
    }

    if (delay >= time_span || time_span == 0)
    {
        struct signal_stack_timeslot *old_signal_stack = signal_stack;
        int old_time_span = time_span;
        int current_time_slot = (time_span == 0) ? 0 : current_real_time % time_span;

        if ((signal_stack == NULL) && (time_span == 0))
            time_span = INIT_NUM_SLOTS;
        while (time_span < (delay + 1))
            time_span *= 2;
        signal_stack = (struct signal_stack_timeslot *) calloc (time_span, sizeof (struct signal_stack_timeslot));

        if (old_signal_stack != NULL)
        {
            int offset_current = (current_real_time % time_span) - (current_real_time % old_time_span);
            int offset_0 = (((current_real_time / old_time_span) + 1) * old_time_span) % time_span;

            /* copy things *below* current_time_slot */
            memcpy (signal_stack + offset_0, old_signal_stack, current_time_slot * sizeof (struct signal_stack_timeslot));
            /* copy things from current_time_slot up to old_time_span */
            memcpy (&(signal_stack[current_time_slot + offset_current]),
              &(old_signal_stack[current_time_slot]), (old_time_span - current_time_slot) * sizeof (struct signal_stack_timeslot));
        }

        g_free (old_signal_stack);

        if (DebugFlag != 0)
            fprintf (stderr, "INFO: enlarging time_span to %d at simulation time %lld\n", time_span, current_real_time);
    }

    int slot = (current_real_time + delay) % time_span;

    if (signal_stack[slot].pos >= signal_stack[slot].max)
    {
        if (signal_stack[slot].max * 2 > max_slot_depth)
            max_slot_depth = signal_stack[slot].max * 2;

        signal_stack[slot].cb = realloc (signal_stack[slot].cb, max_slot_depth * sizeof (struct callback_ *));
        signal_stack[slot].max = max_slot_depth;

        if (DebugFlag != 0)
            fprintf (stderr, "INFO: enlarging slot %d to %d places at simulation time %lld\n", slot, max_slot_depth, current_real_time);
    }

    signal_stack[slot].cb[signal_stack[slot].pos++] = cb;

#ifdef DELAYS_WITH_ERROR
    cb->error = current_error + ((float) delay) / 100;
#endif
}

#endif

#ifdef REORDERED_EVENT_DRIVEN_SCHEDULER

gint TimestampCompareFunc (struct globalEventStruct *a, struct globalEventStruct *b)
{
    if (a->timestamp < b->timestamp)
        return -1;
    else if (a->timestamp > b->timestamp)
        return 1;
    else
        return 0;
}

void InsertReorderedEventAtCurrentTime (struct globalEventStruct *event)
{
    event->timestamp = current_real_time;
    reordered_events_list = g_list_insert_sorted (reordered_events_list, event, (GCompareFunc) TimestampCompareFunc);
    reordered_events_list_length++;
}

void InsertReorderedEventAfterLatestTime (struct globalEventStruct *event)
{
    if (reordered_events_list)
    {
        GList *lastItem = g_list_last (reordered_events_list);
        struct globalEventStruct *lastEvent = lastItem->data;

        event->timestamp = lastEvent->timestamp + 1;
        lastItem = g_list_append (lastItem, event);
        reordered_events_list_length++;
    } else
        InsertReorderedEventAtCurrentTime (event);
}

#endif

void InitialisePorts (struct BreezePart *breezePart)
{
    GList *tmp = getBreezePartPortsList (breezePart);
    GList *portDecls = NULL;

    int numPorts = g_list_length (tmp);

    if (DebugFlag)
        printf ("Initialising %d ports\n", numPorts);

    portDecls = convertPortDecls (breezePart);

    CreateUndefinedInterfacePortComponents (portDecls);

    freePortDecls (portDecls);
}

void PrintChanData (FILE * f, struct chan *chan)
{
    if (!chan->typeNode)
    {
        if (chan->width <= (sizeof (int) * 8))
            fprintf (f, "0x%x", (int) chan->data);
        else
        {
            fprintf (f, "0x");
            mpz_out_str (f, 16, (mpz_ptr) chan->data);
        }
    } else
    {
        char *typeString = TMPNodeToString (NULL, chan->typeNode);
        BalsaType *type;

        BalsaTypeParseFromString (typeString, 0, &type);

        FormatData *fd = NewFormatData (FormatBitCountToWordCount (chan->width));
        size_t count;
        if (chan->width <= (sizeof (int) * 8))
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
//                PrintBalsaTypeData (f, type, fd);
        static char buf[10000];

        FormatPrint (buf, type->format, fd, FormatBareDecimalRadixChoice, NULL);
        fprintf (f, "%s", buf);
        DeleteFormatData (fd);
    }
}

void GenericCallback_dataon (struct comp *comp, int port)
{
    if (TraceFile)
    {
        struct chan *chan = comp->chan[port];

        if (chan->traceType != TRACETYPE_NONE)
        {
            int chan_num = GET_CHAN_NUM (chan);

            WriteAnimationInfo_time (current_real_time);

            if (!chan->typeNode)
            {
                if (channel[chan_num].width <= (sizeof (int) * 8))
                    fprintf (TraceFile, "\n$comment\ndataon %d 0x%x", chan_num, (int) chan->data);
                else
                {
                    fprintf (TraceFile, "\n$comment\ndataon %d 0x", chan_num);
                    mpz_out_str (TraceFile, 16, (mpz_ptr) chan->data);
                }
            } else
            {
                fprintf (TraceFile, "\n$comment\ndataon %d ", chan_num);

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
                PrintBalsaTypeData (TraceFile, type, fd);
                DeleteFormatData (fd);
            }
            fprintf (TraceFile, "\n$end\n");


	    { // print VCS part
		// print to VCD: data part
		char *vcd_wire_code_name = get_static_vcd_code (2*(chan_num-1)+1);

		fprintf (TraceFile, "b");
                if (channel[chan_num].width <= (sizeof (int) * 8)) {
		    int i;
		    unsigned int val = (unsigned int) chan->data;
		    val <<= ((sizeof (int) * 8) - channel[chan_num].width);
		    for (i=channel[chan_num].width; i>0; i--, val<<=1) {
			unsigned int maxbit = 1 << (sizeof(int)*8-1);
			int bit = val & maxbit;
			if (bit)
			    fprintf (TraceFile, "1");
			else
			    fprintf (TraceFile, "0");
		    }
		}
                else
                {
                    mpz_out_str (TraceFile, 2, (mpz_ptr) chan->data);
                }
		fprintf (TraceFile, " %s\n", vcd_wire_code_name);
	    }
        }
    }
}

void GenericCallback_dataoff (struct comp *comp, int port)
{
    if (TraceFile)
    {
        struct chan *chan = comp->chan[port];

        if (chan->traceType != TRACETYPE_NONE)
        {
            int chan_num = GET_CHAN_NUM (chan);

            WriteAnimationInfo_time (current_real_time);
	    // print HHH part
            fprintf (TraceFile, "\n$comment\ndataoff %d\n$end\n", chan_num);
	    { // print VCD part
		// print to VCD: data part
		char *vcd_wire_code_name = get_static_vcd_code (2*(chan_num-1)+1);
		fprintf (TraceFile, "bz %s\n", vcd_wire_code_name);
	    }
        }
    }
}

void GenericCallback_dataon_OutOfOrder (struct comp *comp, int port)
{
}
void GenericCallback_dataoff_OutOfOrder (struct comp *comp, int port)
{
}

void ReportError (struct chan *chan, char *msg)
{
    fprintf (stderr, msg);
    int chanNum = GET_CHAN_NUM (chan);
    extern struct BreezePart *breezePart; // defined in main.c

    GList *chanList = getBreezePartChannelsList (breezePart);
    int length = g_list_length (chanList);
    struct BreezePartChannelsChannel *c = g_list_nth (chanList, length - chanNum)->data;

    fprintf (stderr, "@chan(\"%s\", ", getBreezePartChannelsChannelName (c) ? : "<no name>");
    struct Position *pos = getBreezePartChannelsChannelPosition (c);

    dumpPosition (stderr, pos);
    fprintf (stderr, ")\n");
}
