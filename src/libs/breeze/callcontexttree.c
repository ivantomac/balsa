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

#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#include "libbreeze.h"
#include "breezestructs.h"
#include "behaviour.h"

gboolean libbreeze_TEMPORARY_FLAG_threadgroups = FALSE;
gboolean libbreeze_TEMPORARY_FLAG_behaviourgroups = FALSE;
struct ThreadGroupsItem *threadGroup0;

//#define DEBUG
#ifdef DEBUG
#include <signal.h>
#define ASSERT(x) {if (!(x)) {fprintf(stderr,"ASSERT failed\n"); raise(SIGUSR1);}}
#else
#define ASSERT(x)
#endif

//TMP
static struct BreezePart *breezePart = NULL;
GHashTable *ports_hashtable;
struct CallcontextTreeItem *mainCallContextHashedStruct = NULL;
GPtrArray *allChans = NULL;     // array of struct Chan*
GPtrArray *allComps = NULL;     // array of struct Chan*

//\TMP

void ResetProcessing_CallContexts_hashtable_subfct (gpointer key, gpointer value, gpointer user_data)
{
    struct CallcontextTreeItem *hashedStruct = value;

    hashedStruct->processed = FALSE;
}

void ResetProcessing_CallContexts_hashtable (void)
{
    g_hash_table_foreach (breezePart->callcontextTree_hashtable, ResetProcessing_CallContexts_hashtable_subfct, NULL);
}

void callcontexts_hashtable_foreachWithParent_subfct (gpointer key, gpointer value, gpointer fct)
{
    struct CallcontextTreeItem *hashedStruct = value;

    if (!hashedStruct->processed)
    {
        if (hashedStruct->parent && !hashedStruct->parent->processed)
        {
            callcontexts_hashtable_foreachWithParent_subfct (NULL, hashedStruct->parent, fct);
        }

        ((GHFunc) fct) (key, value, NULL);
        hashedStruct->processed = TRUE;
    }
}
void callcontexts_hashtable_foreachWithParent (GHFunc fct)
{
    if (breezePart)
    {
        ResetProcessing_CallContexts_hashtable ();
        g_hash_table_foreach (breezePart->callcontextTree_hashtable, callcontexts_hashtable_foreachWithParent_subfct, fct);
    }
}

void CallcontextTree_ForeachTopDown (GHashTable * hash_table, GHFunc func, gpointer user_data)
{
    g_hash_table_foreach (hash_table, func, user_data);
}

static void ReadCallContexts_fillParent (gpointer key, gpointer value, gpointer data)
{
    struct CallcontextTreeItem *hashedStruct = value;
    int parentNum = getBreezePartCallcontextsCallcontextParentNum (hashedStruct->callcontext);
    gpointer val = g_hash_table_lookup (breezePart->callcontextTree_hashtable,
      (gconstpointer) & parentNum);

    if (val)
    {
        hashedStruct->parent = val;
        // todo: change next line in order to sort the list by position
        hashedStruct->parent->childs = g_list_prepend (hashedStruct->parent->childs, hashedStruct);
    } else if (parentNum == 0)
        mainCallContextHashedStruct = hashedStruct;
}
static void ReadCallContexts (struct BreezePart *breezePart)
{
    GList *callcontextsList = getBreezePartCallcontextsList (breezePart);
    GList *tmp;

    for (tmp = callcontextsList; tmp; tmp = tmp->next)
    {
        struct BreezePartCallcontextsCallcontext *callcontext = tmp->data;

        {                       // Handle callcontext hashlist
            struct CallcontextTreeItem *hashedStruct = g_new0 (struct CallcontextTreeItem, 1);
            int *key = g_new0 (int, 1);

            *key = getBreezePartCallcontextsCallcontextNum (callcontext);
            g_hash_table_insert (breezePart->callcontextTree_hashtable, (gpointer) key, hashedStruct);

            hashedStruct->callcontext = callcontext;
        }

        {                       // Handle port hashlist
            GList *tmp2 = getBreezePartCallcontextsCallcontextChannelList (callcontext);

            for (; tmp2; tmp2 = tmp2->next)
            {
                int port = (long) tmp2->data;

                gpointer val = g_hash_table_lookup (ports_hashtable, (gconstpointer) & port);

                if (!val)
                {
                    struct HashedPortStruct *hashedPortStruct = g_new0 (struct HashedPortStruct, 1);
                    int *key = g_new0 (int, 1);

                    *key = port;
                    g_hash_table_insert (ports_hashtable, (gpointer) key, hashedPortStruct);

                    hashedPortStruct->port = port;
                    if (port)
                    {
                        GList *channels = getBreezePartChannelsList (breezePart);

                        hashedPortStruct->chan = (struct BreezePartChannelsChannel *) g_list_nth_data (channels, g_list_length (channels) - port);
                    }

                    hashedPortStruct->callcontext_source = getBreezePartCallcontextsCallcontextNum (callcontext);
                    hashedPortStruct->callcontext_dest = getBreezePartCallcontextsCallcontextParentNum (callcontext);
                } else
                {
                    struct HashedPortStruct *hashedPortStruct = val;

                    hashedPortStruct->callcontext_dest = getBreezePartCallcontextsCallcontextNum (callcontext);
                }
            }
        }
    }

    g_hash_table_foreach (breezePart->callcontextTree_hashtable, ReadCallContexts_fillParent, NULL);
}

void ReadChans (void)
{
    GList *channelList = getBreezePartChannelsList (breezePart);
    GList *tmp;

    allChans = g_ptr_array_new ();
    g_ptr_array_add (allChans, NULL); // no channel 0

    int num = 1;

    for (tmp = g_list_last (channelList); tmp; tmp = tmp->prev, num++)
    {
        struct BreezePartChannelsChannel *channel = tmp->data;
        struct Chan *chan = g_new0 (struct Chan, 1);

        chan->num = num;
        chan->channel = channel;
        struct Position *position = getBreezePartChannelsChannelPosition (channel);
        int numcontext = 0;

        if (position)
            numcontext = position->context;

        gpointer val = g_hash_table_lookup (breezePart->callcontextTree_hashtable,
          (gconstpointer) & numcontext);
        struct CallcontextTreeItem *cctitem = val;

        if (cctitem)
        {
            cctitem->chans = g_list_prepend (cctitem->chans, chan);
            chan->callcontext = cctitem;
        }

        g_ptr_array_add (allChans, chan);
    }
}

void ReadComps_RecursivelyFillCompChannels (PtrTMPNode node, struct Comp *comp);
void ReadComps (void)
{
    /*
       struct ThreadGroupsItem *threadGroup = g_new0 (struct ThreadGroupsItem, 1);
       threadGroup->name = g_strdup ("thread 1");
       threadGroup->comps = g_ptr_array_new ();
       g_ptr_array_add (breezePart->threadGroups, threadGroup);
     */

    GList *componentList = getBreezePartComponentsList (breezePart);

    //    GList *channelList = getBreezePartChannelsList (breezePart);
    GList *tmp;

    allComps = g_ptr_array_new ();

    int num = 1;

    for (tmp = g_list_last (componentList); tmp; tmp = tmp->prev, num++)
    {
        struct BreezePartComponentsComponent *component = tmp->data;

        struct Comp *comp = g_new0 (struct Comp, 1);

        comp->num = num;
        comp->component = component;

        PtrTMPNode compNode = getBreezePartComponentsComponentTMPNode (component);
        PtrTMPNode compChansNode = compNode->body.list->next->next->next->data;
        GList *argList = compChansNode->body.list;

        PtrTMPNode tmp = compChansNode;
        GList *tmp2;

        while (tmp->type == TMPList)
        {
            tmp2 = tmp->body.list;
            tmp = tmp->body.list->data;
        }
        if (!strcmp (getBreezePartComponentsComponentName (component), "$BrzFalseVariable"))
            tmp = tmp2->next->data;
        // ASSERT (tmp->type == TMPNumberStr);
        int chanNum = atoi (tmp->body.string);

        comp->mainChannel = g_ptr_array_index (allChans, chanNum);

        // Fill comp->channels
        ReadComps_RecursivelyFillCompChannels (compChansNode, comp);

        char *compname = getBreezePartComponentsComponentName (component);
        int mainChannelNum;

#define ELT1 atoi(((PtrTMPNode)argList->data)->body.string)
#define ELT2 atoi(((PtrTMPNode)argList->next->data)->body.string)
#define ELT3 atoi(((PtrTMPNode)argList->next->next->data)->body.string)
#define ELT4 atoi(((PtrTMPNode)argList->next->next->next->data)->body.string)
#define PASSIVE_PORT(x) struct Chan *chan=g_ptr_array_index(allChans,x); ASSERT(!chan->dest); chan->dest=comp;
#define PASSIVE_SYNC_PORT(x) { PASSIVE_PORT(x); chan->direction=ChanDirection_Sync; }
#define PASSIVE_PUSH_PORT(x) { PASSIVE_PORT(x); chan->direction=ChanDirection_Push; }
#define PASSIVE_PULL_PORT(x) { PASSIVE_PORT(x); chan->direction=ChanDirection_Pull; }
#define ACTIVE_PORT(x) struct Chan *chan=g_ptr_array_index(allChans,x); ASSERT(!chan->source); chan->source=comp;
#define ACTIVE_SYNC_PORT(x) { ACTIVE_PORT(x); }
#define ACTIVE_PUSH_PORT(x) { ACTIVE_PORT(x); }
#define ACTIVE_PULL_PORT(x) { ACTIVE_PORT(x); }
        /*
           #define ACTIVE_SYNC_PORT(x) { ACTIVE_PORT(x); chan->direction=ChanDirection_Sync; }
           #define ACTIVE_PUSH_PORT(x) { ACTIVE_PORT(x); chan->direction=ChanDirection_Pull; }
           #define ACTIVE_PULL_PORT(x) { ACTIVE_PORT(x); chan->direction=ChanDirection_Push; }
         */
#define ELT1_1 atoi(((PtrTMPNode)((PtrTMPNode)argList->data)->body.list->data)->body.string)

#define LIST1 ((PtrTMPNode)argList->data)->body.list
#define LIST2 ((PtrTMPNode)argList->next->data)->body.list
#define LIST3 ((PtrTMPNode)argList->next->next->data)->body.list
#define LIST4 ((PtrTMPNode)argList->next->next->next->data)->body.list
#define ACTIVE_SYNC_PORT_LIST(l) { GList *tmp = l; for (; tmp; tmp=tmp->next) ((struct Chan *)g_ptr_array_index (allChans, atoi(((PtrTMPNode)tmp->data)->body.string)))->source = comp; }
#define ACTIVE_PUSH_PORT_LIST(x) { ACTIVE_SYNC_PORT_LIST(x); }
#define ACTIVE_PULL_PORT_LIST(x) { ACTIVE_SYNC_PORT_LIST(x); }
#define PASSIVE_SYNC_PORT_LIST(l) { GList *tmp = l; for (; tmp; tmp=tmp->next) { struct Chan *chan = g_ptr_array_index (allChans, atoi(((PtrTMPNode)tmp->data)->body.string)); chan->dest = comp; chan->direction=ChanDirection_Sync; } }
#define PASSIVE_PUSH_PORT_LIST(l) { GList *tmp = l; for (; tmp; tmp=tmp->next) { struct Chan *chan = g_ptr_array_index (allChans, atoi(((PtrTMPNode)tmp->data)->body.string)); chan->dest = comp; chan->direction=ChanDirection_Push; } }
#define PASSIVE_PULL_PORT_LIST(l) { GList *tmp = l; for (; tmp; tmp=tmp->next) { struct Chan *chan = g_ptr_array_index (allChans, atoi(((PtrTMPNode)tmp->data)->body.string)); chan->dest = comp; chan->direction=ChanDirection_Pull; } }

        /*
           if (!strcmp (compname, "$BrzContinue")) {
           mainChannelNum = ELT1;
           PASSIVE_PORT (ELT1);
           }
         */
        if (!strcmp (compname, "$BrzAdapt"))
        {
            mainChannelNum = ELT1;
            PASSIVE_PULL_PORT (ELT1);
            ACTIVE_PULL_PORT (ELT2);
        } else if (!strcmp (compname, "$BrzBinaryFunc"))
        {
            mainChannelNum = ELT1;
            PASSIVE_PULL_PORT (ELT1);
            ACTIVE_PULL_PORT (ELT2);
            ACTIVE_PULL_PORT (ELT3);
        } else if (!strcmp (compname, "$BrzBinaryFuncPush"))
        {
            mainChannelNum = ELT1;
            ACTIVE_PUSH_PORT (ELT1);
            PASSIVE_PUSH_PORT (ELT2);
            PASSIVE_PUSH_PORT (ELT3);
        } else if (!strcmp (compname, "$BrzBinaryFuncConstR"))
        {
            mainChannelNum = ELT1;
            PASSIVE_PULL_PORT (ELT1);
            ACTIVE_PULL_PORT (ELT2);
        } else if (!strcmp (compname, "$BrzBinaryFuncConstRPush"))
        {
            mainChannelNum = ELT1;
            ACTIVE_PUSH_PORT (ELT1);
            PASSIVE_PUSH_PORT (ELT2);
        } else if (!strcmp (compname, "$BrzCall"))
        {
            mainChannelNum = ELT2;
            PASSIVE_SYNC_PORT_LIST (LIST1);
            ACTIVE_SYNC_PORT (ELT2);
        } else if (!strcmp (compname, "$BrzCallMux"))
        {
            mainChannelNum = ELT2;
            PASSIVE_PUSH_PORT_LIST (LIST1);
            ACTIVE_PUSH_PORT (ELT2);
        } else if (!strcmp (compname, "$BrzCallDemux"))
        {
            mainChannelNum = ELT2;
            PASSIVE_PULL_PORT_LIST (LIST1);
            ACTIVE_PULL_PORT (ELT2);
        } else if (!strcmp (compname, "$BrzCase"))
        {
            mainChannelNum = ELT1;
            PASSIVE_PUSH_PORT (ELT1);
            ACTIVE_SYNC_PORT_LIST (LIST2);
        } else if (!strcmp (compname, "$BrzCaseFetch"))
        {
            mainChannelNum = ELT1;
            PASSIVE_PULL_PORT (ELT1);
            ACTIVE_PULL_PORT (ELT2);
            ACTIVE_PULL_PORT_LIST (LIST3);
        } else if (!strcmp (compname, "$BrzCombine"))
        {
            mainChannelNum = ELT1;
            PASSIVE_PULL_PORT (ELT1);
            ACTIVE_PULL_PORT (ELT2);
            ACTIVE_PULL_PORT (ELT3);
        } else if (!strcmp (compname, "$BrzCombineEqual"))
        {
            mainChannelNum = ELT1;
            PASSIVE_PULL_PORT (ELT1);
            ACTIVE_PULL_PORT_LIST (LIST2);
        } else if (!strcmp (compname, "$BrzConcur"))
        {
            mainChannelNum = ELT1;
            PASSIVE_SYNC_PORT (ELT1);
            ACTIVE_SYNC_PORT_LIST (LIST2);
        } else if (!strcmp (compname, "$BrzConstant"))
        {
            mainChannelNum = ELT1;
            PASSIVE_PULL_PORT (ELT1);
        } else if (!strcmp (compname, "$BrzDecisionWait"))
        {
            mainChannelNum = ELT1;
            PASSIVE_SYNC_PORT (ELT1);
            PASSIVE_SYNC_PORT_LIST (LIST2);
            ACTIVE_SYNC_PORT_LIST (LIST3);
        } else if (!strcmp (compname, "$BrzEncode"))
        {
            mainChannelNum = ELT2;
            PASSIVE_SYNC_PORT_LIST (LIST1);
            ACTIVE_PUSH_PORT (ELT2);
        } else if (!strcmp (compname, "$BrzFalseVariable"))
        {
            mainChannelNum = ELT1;
            PASSIVE_PUSH_PORT (ELT1);
            ACTIVE_SYNC_PORT (ELT2);
            PASSIVE_PULL_PORT_LIST (LIST3);
        } else if (!strcmp (compname, "$BrzEagerFalseVariable"))
        {
            g_warning
              ("Deprecated EagerFalseVariable component detected. You probably compiled your circuit with an old version of the Balsa compiler. I'm continuing anyway, but misbehaviours/crashes should be expected.");
            mainChannelNum = ELT1;
            PASSIVE_PUSH_PORT (ELT1);
            ACTIVE_SYNC_PORT (ELT2);
            PASSIVE_PULL_PORT_LIST (LIST3);
        } else if (!strcmp (compname, "$BrzPassiveEagerFalseVariable"))
        {
            mainChannelNum = ELT1;
            PASSIVE_SYNC_PORT (ELT1);
            PASSIVE_PUSH_PORT (ELT2);
            ACTIVE_SYNC_PORT (ELT3);
            PASSIVE_PULL_PORT_LIST (LIST4);
        } else if (!strcmp (compname, "$BrzActiveEagerFalseVariable"))
        {
            mainChannelNum = ELT1;
            PASSIVE_SYNC_PORT (ELT1);
            ACTIVE_PULL_PORT (ELT2);
            ACTIVE_SYNC_PORT (ELT3);
            PASSIVE_PULL_PORT_LIST (LIST4);
        } else if (!strcmp (compname, "$BrzPassiveSyncEagerFalseVariable"))
        {
            mainChannelNum = ELT1;
            PASSIVE_SYNC_PORT (ELT1);
            PASSIVE_SYNC_PORT (ELT2);
            ACTIVE_SYNC_PORT (ELT3);
        } else if (!strcmp (compname, "$BrzPassiveEagerNullAdapt"))
        {
            mainChannelNum = ELT1;
            PASSIVE_SYNC_PORT (ELT1);
            PASSIVE_PUSH_PORT (ELT2);
            ACTIVE_SYNC_PORT (ELT3);
        } else if (!strcmp (compname, "$BrzActiveEagerNullAdapt"))
        {
            mainChannelNum = ELT1;
            PASSIVE_SYNC_PORT (ELT1);
            ACTIVE_PULL_PORT (ELT2);
            ACTIVE_SYNC_PORT (ELT3);
        } else if (!strcmp (compname, "$BrzFetch"))
        {
            mainChannelNum = ELT1;
            PASSIVE_SYNC_PORT (ELT1);
            ACTIVE_PULL_PORT (ELT2);
            ACTIVE_PUSH_PORT (ELT3);
        } else if (!strcmp (compname, "$BrzLoop"))
        {
            mainChannelNum = ELT1;
            PASSIVE_SYNC_PORT (ELT1);
            ACTIVE_SYNC_PORT (ELT2);
        } else if (!strcmp (compname, "$BrzPassivatorPush"))
        {
            mainChannelNum = ELT2;
            PASSIVE_PULL_PORT_LIST (LIST1);
            PASSIVE_PUSH_PORT (ELT2);
        } else if (!strcmp (compname, "$BrzSequence"))
        {
            mainChannelNum = ELT1;
            PASSIVE_SYNC_PORT (ELT1);
            ACTIVE_SYNC_PORT_LIST (LIST2);
        } else if (!strcmp (compname, "$BrzSlice"))
        {
            mainChannelNum = ELT1;
            PASSIVE_PULL_PORT (ELT1);
            ACTIVE_PULL_PORT (ELT2);
        } else if (!strcmp (compname, "$BrzSynch"))
        {
            mainChannelNum = ELT2;
            PASSIVE_SYNC_PORT_LIST (LIST1);
            ACTIVE_SYNC_PORT (ELT2);
        } else if (!strcmp (compname, "$BrzUnaryFunc"))
        {
            mainChannelNum = ELT1;
            PASSIVE_PULL_PORT (ELT1);
            ACTIVE_PULL_PORT (ELT2);
        } else if (!strcmp (compname, "$BrzUnaryFuncPush"))
        {
            mainChannelNum = ELT1;
            ACTIVE_PUSH_PORT (ELT1);
            PASSIVE_PUSH_PORT (ELT2);
        } else if (!strcmp (compname, "$BrzVariable"))
        {
            mainChannelNum = ELT1;
            PASSIVE_PUSH_PORT (ELT1);
            PASSIVE_PULL_PORT_LIST (LIST2);
        } else if (!strcmp (compname, "$BrzBuiltinVariable"))
        {
            mainChannelNum = ELT1;
            PASSIVE_PUSH_PORT (ELT1);
            PASSIVE_PULL_PORT_LIST (LIST2);
        } else if (!strcmp (compname, "$BrzInitVariable"))
        {
            mainChannelNum = ELT1;
            PASSIVE_PUSH_PORT (ELT1);
            PASSIVE_SYNC_PORT (ELT2);
            PASSIVE_PULL_PORT_LIST (LIST3);
        } else if (!strcmp (compname, "$BrzContinuePush"))
        {
            mainChannelNum = ELT1;
            PASSIVE_PUSH_PORT (ELT1);
        } else if (!strcmp (compname, "$BrzForkPush"))
        {
            mainChannelNum = ELT1;
            PASSIVE_PUSH_PORT (ELT1);
            ACTIVE_PUSH_PORT_LIST (LIST2);
        } else if (!strcmp (compname, "$BrzCallDemuxPush"))
        {
            mainChannelNum = ELT1;
            PASSIVE_PUSH_PORT (ELT1);
            ACTIVE_PUSH_PORT_LIST (LIST2);
        } else if (!strcmp (compname, "$BrzCallActive"))
        {
            mainChannelNum = ELT1;
            PASSIVE_SYNC_PORT (ELT1);
            ACTIVE_SYNC_PORT_LIST (LIST2);
        } else if (!strcmp (compname, "$BrzSynchPush"))
        {
            mainChannelNum = ELT1;
            PASSIVE_PUSH_PORT (ELT1);
            PASSIVE_PULL_PORT_LIST (LIST2);
            ACTIVE_PUSH_PORT (ELT3);
        } else if (!strcmp (compname, "$BrzArbiter"))
        {
            mainChannelNum = ELT1;
            PASSIVE_SYNC_PORT (ELT1);
            PASSIVE_SYNC_PORT (ELT2);
            ACTIVE_SYNC_PORT (ELT3);
            ACTIVE_SYNC_PORT (ELT4);
        } else if (!strcmp (compname, "$BrzBar"))
        {
            mainChannelNum = ELT2;
            PASSIVE_PULL_PORT (ELT1);
            PASSIVE_SYNC_PORT (ELT2);
            ACTIVE_PULL_PORT_LIST (LIST3);
            ACTIVE_PULL_PORT_LIST (LIST4);
        } else if (!strcmp (compname, "$BrzContinue"))
        {
            mainChannelNum = ELT1;
            PASSIVE_SYNC_PORT (ELT1);
        } else if (!strcmp (compname, "$BrzFork"))
        {
            mainChannelNum = ELT1;
            PASSIVE_SYNC_PORT (ELT1);
            ACTIVE_SYNC_PORT_LIST (LIST2);
        } else if (!strcmp (compname, "$BrzNullAdapt"))
        {
            mainChannelNum = ELT1;
            ACTIVE_PULL_PORT (ELT1);
            PASSIVE_SYNC_PORT (ELT2);
        } else if (!strcmp (compname, "$BrzSplit"))
        {
            mainChannelNum = ELT1;
            PASSIVE_PUSH_PORT (ELT1);
            ACTIVE_PUSH_PORT (ELT2);
            ACTIVE_PUSH_PORT (ELT3);
        } else if (!strcmp (compname, "$BrzSplitEqual"))
        {
            mainChannelNum = ELT1;
            PASSIVE_PUSH_PORT (ELT1);
            ACTIVE_PUSH_PORT_LIST (LIST2);
        } else if (!strcmp (compname, "$BrzWhile"))
        {
            mainChannelNum = ELT1;
            PASSIVE_SYNC_PORT (ELT1);
            ACTIVE_PULL_PORT (ELT2);
            ACTIVE_SYNC_PORT (ELT3);
        } else if (!strcmp (compname, "$BrzWireFork"))
        {
            mainChannelNum = ELT1;
            PASSIVE_SYNC_PORT (ELT1);
            ACTIVE_SYNC_PORT_LIST (LIST2);
        } else if (!strcmp (compname, "$BrzHalt"))
        {
            mainChannelNum = ELT1;
            PASSIVE_SYNC_PORT (ELT1);
        } else if (!strcmp (compname, "$BrzHaltPush"))
        {
            mainChannelNum = ELT1;
            PASSIVE_PUSH_PORT (ELT1);
        } else if (!strcmp (compname, "$BrzPassivator"))
        {
            mainChannelNum = ELT1_1;
            PASSIVE_SYNC_PORT_LIST (LIST1);
        } else if (!strcmp (compname, "$BrzSynchPull"))
        {
            mainChannelNum = ELT2;
            PASSIVE_PULL_PORT_LIST (LIST1);
            ACTIVE_PULL_PORT (ELT2);
        }
	// other comps (probably invalid ones if they reach here)
	else
        {
            GList *elt = argList;

            for (; elt; elt = elt->next)
            {
                g_assert (((PtrTMPNode) elt->data)->type == TMPNumberStr);
                int arg = atoi (((PtrTMPNode) elt->data)->body.string);
                struct Chan *chan = g_ptr_array_index (allChans, arg);

                if (!chan)
                {
                    fprintf (stderr, "error with component %s\n", compname);
                    continue;
                }
                if (!chan->source)
                    chan->source = comp;
                else if (!chan->dest)
                    chan->dest = comp;
                else            /* Emphasize the fact that src and dest are occupied by unoriented components */
                    chan->dest = comp;
                chan->direction = ChanDirection_Unknown;
            }
        }

        struct CallcontextTreeItem *cctitem = comp->mainChannel->callcontext;

        if (cctitem)
        {
            cctitem->comps = g_list_prepend (cctitem->comps, comp);
        }
        //        g_ptr_array_add (threadGroup->comps, comp);
        g_ptr_array_add (allComps, comp);
    }
}
void ReadComps_RecursivelyFillCompChannels (PtrTMPNode node, struct Comp *comp)
{
    switch (node->type)
    {
    case TMPList:
        {
            GList *elt = node->body.list;

            for (; elt; elt = elt->next)
            {
                PtrTMPNode node2 = elt->data;

                ReadComps_RecursivelyFillCompChannels (node2, comp);
            }
        }
        break;
    case TMPNumberStr:
        {
            int arg = atoi (node->body.string);
            struct Chan *chan = g_ptr_array_index (allChans, arg);

            if (!chan)
            {
                char *compname = getBreezePartComponentsComponentName (comp->component);

                fprintf (stderr, "error with component %s\n", compname);
                return;
            }
            comp->channels = g_list_prepend (comp->channels, chan);
        }
        break;

    default:
        {
            char *compname = getBreezePartComponentsComponentName (comp->component);

            fprintf (stderr, "oh... error with component %s\n", compname);
        }
    }
}

void ReadPorts_subfct (gpointer key, gpointer value, gpointer user_data)
{
    struct CallcontextTreeItem *hashedStruct = value;
    GList *ports = getBreezePartCallcontextsCallcontextChannelList (hashedStruct->callcontext);

    for (; ports; ports = ports->next)
    {
        int portnum = (long) ports->data;
        gpointer hashedPortStruct = g_hash_table_lookup (ports_hashtable, (gconstpointer) & portnum);

        hashedStruct->ports = g_list_prepend (hashedStruct->ports, hashedPortStruct);
    }
}
void ReadPorts (void)
{
    // Read toplevel ports
    if (mainCallContextHashedStruct)
    {
        int nbPorts = g_list_length (getBreezePartPortsList (breezePart));
        int i;

        for (i = 1; i <= nbPorts; i++)
        {
            int portnum = (int) i;

            struct HashedPortStruct *hashedPortStruct = g_new0 (struct HashedPortStruct, 1);
            int *key = g_new0 (int, 1);

            *key = portnum;
            g_hash_table_insert (ports_hashtable, (gpointer) key, hashedPortStruct);

            hashedPortStruct->port = portnum;
            if (portnum)
            {
                GList *channels = getBreezePartChannelsList (breezePart);

                hashedPortStruct->chan = (struct BreezePartChannelsChannel *) g_list_nth_data (channels, g_list_length (channels) - portnum);
            }

            hashedPortStruct->callcontext_source = getBreezePartCallcontextsCallcontextNum (mainCallContextHashedStruct->callcontext);
            hashedPortStruct->callcontext_dest = 0;

            mainCallContextHashedStruct->ports = g_list_append (mainCallContextHashedStruct->ports, hashedPortStruct);
        }
    }
    // Read other ports
    callcontexts_hashtable_foreachWithParent (ReadPorts_subfct);
}

void DoITrecursive_RecTruc (PtrTMPNode node, struct Comp *comp, struct ThreadGroupsItem *threadGroup);
GList *middleComps;
void DoITrecursive (struct Comp *comp, struct ThreadGroupsItem *threadGroup)
{
    char *compname = getBreezePartComponentsComponentName (comp->component);

    if (!strcmp (compname, "$BrzConcur") || !strcmp (compname, "$BrzFork") || !strcmp (compname, "$BrzWireFork"))
    {
        comp->threadGroup = threadGroup;
        g_ptr_array_add (threadGroup->comps, comp);

        PtrTMPNode compNode = getBreezePartComponentsComponentTMPNode (comp->component);
        PtrTMPNode compChansNode = compNode->body.list->next->next->next->data;
        GList *argList = compChansNode->body.list;

        GList *l = ((PtrTMPNode) argList->next->data)->body.list;

        {
            GList *tmp = l;

            for (; tmp; tmp = tmp->next)
            {
                struct Chan *chan = ((struct Chan *) g_ptr_array_index (allChans,
                    atoi (((PtrTMPNode) tmp->data)->body.string)));
                struct Comp *comp2 = chan->dest;

                struct ThreadGroupsItem *threadGroup2 = g_new0 (struct ThreadGroupsItem, 1);

                threadGroup2->name = g_strdup_printf ("thread %d", breezePart->threadGroups->len + 1);
                threadGroup2->comps = g_ptr_array_new ();
                g_ptr_array_add (breezePart->threadGroups, threadGroup2);

                if (!comp2->threadGroup)
                    DoITrecursive (comp2, threadGroup2);
            }
        }
    } else if (!strcmp (compname, "$BrzFalseVariable"))
    {
        if (!comp->threadGroup)
        {
            comp->threadGroup = threadGroup0;
            g_ptr_array_add (threadGroup0->comps, comp);
        }

        PtrTMPNode compNode = getBreezePartComponentsComponentTMPNode (comp->component);
        PtrTMPNode compChansNode = compNode->body.list->next->next->next->data;
        GList *argList = compChansNode->body.list;
        int channum = atoi (((PtrTMPNode) argList->next->data)->body.string);

        {
            struct Chan *chan = (struct Chan *) g_ptr_array_index (allChans, channum);
            struct Comp *comp2 = chan->dest;

            middleComps = g_list_prepend (middleComps, comp2);
            /*
               struct ThreadGroupsItem *threadGroup2 = g_new0 (struct ThreadGroupsItem, 1);
               threadGroup2->name = g_strdup_printf ("thread %d", breezePart->threadGroups->len+1);
               threadGroup2->comps = g_ptr_array_new ();
               g_ptr_array_add (breezePart->threadGroups, threadGroup2);

               if (!comp2->threadGroup)
               DoITrecursive (comp2, threadGroup2);
             */
        }
    } else if (!strcmp (compname, "$BrzVariable") || !strcmp (compname, "$BrzPassivatorPush"))
    {
        if (!comp->threadGroup)
        {
            comp->threadGroup = threadGroup0;
            g_ptr_array_add (threadGroup0->comps, comp);
        }
    } else
    {
        comp->threadGroup = threadGroup;
        g_ptr_array_add (threadGroup->comps, comp);

        {
            PtrTMPNode compNode = getBreezePartComponentsComponentTMPNode (comp->component);
            PtrTMPNode compChansNode = compNode->body.list->next->next->next->data;

            //  GList *argList = compChansNode->body.list;

            DoITrecursive_RecTruc (compChansNode, comp, threadGroup);
        }
    }
}
void DoITrecursive_RecTruc (PtrTMPNode node, struct Comp *comp, struct ThreadGroupsItem *threadGroup)
{
    switch (node->type)
    {
    case TMPList:
        {
            GList *elt = node->body.list;

            for (; elt; elt = elt->next)
            {
                PtrTMPNode node2 = elt->data;

                DoITrecursive_RecTruc (node2, comp, threadGroup);
            }
        }
        break;
    case TMPNumberStr:
        {
            int arg = atoi (node->body.string);
            struct Chan *chan = g_ptr_array_index (allChans, arg);

            if (!chan)
            {
                char *compname = getBreezePartComponentsComponentName (comp->component);

                fprintf (stderr, "error with component %s\n", compname);
                return;
            }
            struct Comp *comp2;

            if (chan->source == comp)
                comp2 = chan->dest;
            else
            {
                return;         //comp2 = chan->source;
                if (chan->direction == ChanDirection_Unknown && chan->source->threadGroup == 0)
                    comp2 = chan->source;
                else
                    return;
            }

            if (!comp2->threadGroup)
                DoITrecursive (comp2, threadGroup);
        }
        break;

    default:
        {
            char *compname = getBreezePartComponentsComponentName (comp->component);

            fprintf (stderr, "oh... error with component %s\n", compname);
        }
    }
}

void InitBreezePartCallcontextTree (struct BreezePart *part)
{
    breezePart = part;
    breezePart->callcontextTree_hashtable = g_hash_table_new (g_int_hash, g_int_equal);
    ports_hashtable = g_hash_table_new (g_int_hash, g_int_equal);

    breezePart->threadGroups = g_ptr_array_new ();

    ReadCallContexts (breezePart);
    ReadChans ();
    ReadComps ();
    ReadPorts ();

    if (libbreeze_TEMPORARY_FLAG_threadgroups)
    {
        threadGroup0 = g_new0 (struct ThreadGroupsItem, 1);

        threadGroup0->name = g_strdup ("thread 0");
        threadGroup0->comps = g_ptr_array_new ();
        g_ptr_array_add (breezePart->threadGroups, threadGroup0);

        struct ThreadGroupsItem *threadGroup = g_new0 (struct ThreadGroupsItem, 1);

        threadGroup->name = g_strdup ("thread 1");
        threadGroup->comps = g_ptr_array_new ();
        g_ptr_array_add (breezePart->threadGroups, threadGroup);

        struct Chan *chan = g_ptr_array_index (allChans, 1);
        struct Comp *comp = chan->dest;

        middleComps = NULL;
        DoITrecursive (comp, threadGroup);
        GList *tmp;

        for (tmp = g_list_last (middleComps); tmp; tmp = tmp->prev)
        {
            struct Comp *comp2 = tmp->data;

            if (!comp2->threadGroup)
            {
                struct ThreadGroupsItem *threadGroup2 = g_new0 (struct ThreadGroupsItem, 1);

                threadGroup2->name = g_strdup_printf ("thread %d", breezePart->threadGroups->len + 1);
                threadGroup2->comps = g_ptr_array_new ();
                g_ptr_array_add (breezePart->threadGroups, threadGroup2);

                DoITrecursive (comp2, threadGroup2);
            }
        }

        unsigned int i;

        for (i = 1; i < breezePart->threadGroups->len; i++)
        {
            // Foreach thread: Find which callcontext it belongs to (= the GCC (Greatest Common Context))

            // Insert each thread in its GCC group
            struct ThreadGroupsItem *threadGroup2 = g_ptr_array_index (breezePart->threadGroups, i);

            if (threadGroup2->comps->len == 0)
            {
                fprintf (stderr, "Strange... 0 components in thread %s\n", threadGroup2->name);
                continue;
            }
            struct Comp *comp2 = g_ptr_array_index (threadGroup2->comps, 0);
            struct Chan *chan2 = comp2->mainChannel;
            struct CallcontextTreeItem *hashedStruct = chan2->callcontext;

            struct CallcontextTreeItem *hashedStruct2 = g_new0 (struct CallcontextTreeItem, 1);
            int *key = g_new0 (int, 1);

            *key = i;           //crap
            g_hash_table_insert (breezePart->callcontextTree_hashtable, (gpointer) key, hashedStruct2);

            hashedStruct2->parent = hashedStruct;
            hashedStruct->childs = g_list_prepend (hashedStruct->childs, hashedStruct2);

            // For each component: If the comp is in a thread, then move it from the callcontext group to the thread group.
            unsigned int j;

            for (j = 0; j < threadGroup2->comps->len; j++)
            {
                struct Comp *comp3 = g_ptr_array_index (threadGroup2->comps, j);
                struct Chan *chan3 = comp3->mainChannel;

                hashedStruct2->comps = g_list_prepend (hashedStruct2->comps, comp3);
                /*hashedStruct->comps */ chan3->callcontext->comps =
                  g_list_remove ( /*hashedStruct->comps */ chan3->callcontext->
                  comps, comp3);
                chan3->callcontext = hashedStruct2;
            }
        }
    }

    if (libbreeze_TEMPORARY_FLAG_behaviourgroups)
        DiscoverBehaviourGroups (breezePart);
}
