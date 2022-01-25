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

#include "partitioning.h"
#include "breeze/libbreeze.h"

#include "structure.h"
#include "core.h"
#include "simtrace.h"
#include "graphviewGUI.h"

#define MAX_COMPS 8000
int mat[MAX_COMPS][MAX_COMPS];
int nb_comps = 0;
struct Comp *comps[MAX_COMPS];

int maincomp_num = 0;

static gboolean Partitioning_AddEdge (struct Edge *edge)
{
    PtrEdgeInfo edgeInfo = edge->data;

    if (!edgeInfo)
        return TRUE;
    struct Chan *chan = edgeInfo->chan;

    if (!chan)
        return TRUE;
    struct Comp *source = chan->source;
    struct Comp *dest = chan->dest;

    if (!source)
        return TRUE;
    if (!dest)
        return TRUE;

    if (getBreezePartChannelsChannelWidth (chan->channel) != 64)
        mat[source->num][dest->num] = mat[dest->num][source->num] = 1;
    else
    {
        mat[source->num][dest->num] = mat[dest->num][source->num] = 100000;
        mat[source->num][maincomp_num] = mat[maincomp_num][source->num] = 100000;
        mat[maincomp_num][dest->num] = mat[dest->num][maincomp_num] = 100000;
    }

    nb_comps = MAX (MAX (nb_comps, source->num), dest->num);
    comps[source->num] = source;
    comps[dest->num] = dest;

    return TRUE;
}

void Partitioning (gboolean do_negative_weights)
{
    int i, j;

    // Take the hsc graph
    int max = Core_SimTrace_MaxPosition ();

    printf ("max=%d\n", max);

    // Process everything in one go to accelerate subsequent accesses
    Core_SimTrace_GetTraceAtTime (max);

    // Clear all weights
    nb_comps = 0;
    for (i = 0; i < MAX_COMPS; i++)
        for (j = 0; j < MAX_COMPS; j++)
            mat[i][j] = 0;

    // Search for the component connected to the main activation signal (channel 1)
    // Unfortunately, this component is not necessarily the first component.
    struct Chan *chan = g_ptr_array_index (allChans, 1);

    if (chan->source != NULL)
        printf ("Partitioning: Error in detecting main component\n");
    else
        maincomp_num = chan->dest->num;

    // Set each edge to 1, to avoid disconnected graphs in Jostle
    ForEachEdge (Partitioning_AddEdge);

    // Add 1 weight per simulation event, and Subtract 10 between components associated to concurrent events
    for (i = 0; i < max; i++)
    {
        GPtrArray *trace = Core_SimTrace_GetTraceAtTime (i);

        if (trace)
        {
            int j;

            for (j = 0; j < (int) trace->len; j++)
            {
                ChanTraceItem *traceItem = g_ptr_array_index (trace, j);

                struct Edge *edge = traceItem->edge;

                PtrEdgeInfo edgeInfo = edge->data;

                if (!edgeInfo)
                    continue;
                struct Chan *chan = edgeInfo->chan;

                if (!chan)
                    continue;
                struct Comp *source = chan->source;
                struct Comp *dest = chan->dest;

                if (!source)
                    continue;
                if (!dest)
                    continue;

                mat[source->num][dest->num]++;
                mat[dest->num][source->num]++;

                if (do_negative_weights)
                {
                    int k;

                    for (k = 0; k < j; k++)
                    {
                        ChanTraceItem *traceItem2 = g_ptr_array_index (trace, k);

                        struct Edge *edge2 = traceItem2->edge;

                        PtrEdgeInfo edgeInfo2 = edge2->data;

                        if (!edgeInfo2)
                            continue;
                        struct Chan *chan2 = edgeInfo2->chan;

                        if (!chan2)
                            continue;
                        struct Comp *source2 = chan2->source;
                        struct Comp *dest2 = chan2->dest;

                        if (!source2)
                            continue;
                        if (!dest2)
                            continue;

                        int negweight = -10 / ((int) trace->len - 1);

                        if (!mat[source->num][source2->num])
                        {
                            mat[source->num][source2->num] = negweight;
                            mat[source2->num][source->num] = negweight;
                        }

                        if (!mat[source->num][dest2->num])
                        {
                            mat[source->num][dest2->num] = negweight;
                            mat[dest2->num][source->num] = negweight;
                        }

                        if (!mat[dest->num][source2->num])
                        {
                            mat[dest->num][source2->num] = negweight;
                            mat[source2->num][dest->num] = negweight;
                        }

                        if (!mat[dest->num][dest2->num])
                        {
                            mat[dest->num][dest2->num] = negweight;
                            mat[dest2->num][dest->num] = negweight;
                        }
                    }
                }
            }
        }
    }

    // Export graph
    FILE *f = fopen ("graph.out", "w");

    fprintf (f, "%d %d 1\n", nb_comps, (nb_comps * (nb_comps - 1)) / 2);

    for (i = 1; i <= nb_comps; i++)
    {
        for (j = 1; j <= nb_comps; j++)
        {
            if (do_negative_weights)
            {
                if (i != j && mat[i][j] + 10)
                    fprintf (f, "%d %d  ", j, mat[i][j] + 10);
            } else
            {
                if (i != j && mat[i][j])
                    fprintf (f, "%d %d  ", j, mat[i][j]);
            }
        }

        fprintf (f, "\n");
    }
    fclose (f);

    printf ("done\n");
}

/****************************************/

void on_ImportPartition_clicked (GtkButton * button, gpointer user_data)
{
    // Read file
    FILE *f = fopen ("graph.out.ptn", "r");

    int i = 1;
    int n;

    while (fscanf (f, "%d\n", &n) == 1)
    {
        struct Comp *comp = comps[i];
        struct Vertex *v = comp->data;

        if (v)
            v->color = n + 1;

        i++;
    }

    fclose (f);

    // Search for the component connected to the main activation signal (channel 1)
    // Unfortunately, this component is not necessarily the first component.
    struct Chan *chan = g_ptr_array_index (allChans, 1);

    if (chan->source != NULL)
    {
        printf ("ImportPartition: Error in detecting main component\n");
    } else
    {
        struct Vertex *vertex_comp1 = chan->dest ? chan->dest->data : NULL;
        int comp1_color = vertex_comp1->color;

        if (comp1_color == 1)
        {
            printf ("component1's partition = 1. OK.\n");
        } else
        {
            printf ("component1's partition = %d. Swapping with partition 1.\n", comp1_color);

            for (i--; i >= 1; i--)
            {
                struct Comp *comp = comps[i];
                struct Vertex *v = comp->data;

                if (v->color == comp1_color)
                    v->color = 1;
                else if (v->color == 1)
                    v->color = comp1_color;
            }
        }
    }

    RedrawScreen ();
}

/****************************************/

GList *SearchAndReplace_UndoList = NULL;

static void SearchAndReplace (PtrTMPNode node, int from, int to)
{
    if (!node)
        return;

    switch (node->type)
    {
    case TMPNumberStr:
        if (atoi (node->body.string) == from)
        {
            SearchAndReplace_UndoList = g_list_prepend (SearchAndReplace_UndoList, node);
            SearchAndReplace_UndoList = g_list_prepend (SearchAndReplace_UndoList, node->body.string);
            node->body.string = g_strdup_printf ("%d", to);
        }
        break;
    case TMPList:
        {
            GList *listIter = node->body.list;

            while (listIter)
            {
                SearchAndReplace ((PtrTMPNode) listIter->data, from, to);
                listIter = listIter->next;
            }
        }
        break;
    case TMPBoolean:
    case TMPSymbol:
    case TMPString:
        break;
    }
}

static void UndoSearchAndReplace (void)
{
    GList *tmp = g_list_last (SearchAndReplace_UndoList);

    for (; tmp; tmp = tmp->prev)
    {
        PtrTMPNode node = tmp->data;

        tmp = tmp->prev;
        node->body.string = tmp->data;
    }

    g_list_free (SearchAndReplace_UndoList);
    SearchAndReplace_UndoList = NULL;
}

static gboolean Partitioning_CheckAndSplitEdge (struct Edge *edge)
{
    PtrEdgeInfo edgeInfo = edge->data;

    if (!edgeInfo)
        return TRUE;
    struct Chan *chan = edgeInfo->chan;

    if (!chan)
        return TRUE;
    struct Comp *source = chan->source;
    struct Comp *dest = chan->dest;

    if (!source)
        return TRUE;
    if (!dest)
        return TRUE;
    struct Vertex *v1 = source->data;
    struct Vertex *v2 = dest->data;

    if (!v1)
        return TRUE;
    if (!v2)
        return TRUE;

    if (v1->color == v2->color)
        return TRUE;

    struct BreezePartChannelsChannel *channel = chan->channel;

    if (!channel)
        return TRUE;

    struct BreezePartComponentsComponent *source_component = source->component;
    struct BreezePartComponentsComponent *dest_component = dest->component;

    if (!source_component || !dest_component)
        return TRUE;

    // Split channel into 2 channels
    struct BreezePartChannelsChannel *newChannel = DeepCopyBreezePartChannelsChannel (channel);

    setBreezePartChannelsList (projectBreezePart, g_list_prepend (getBreezePartChannelsList (projectBreezePart), newChannel));
    int newChannelNum = g_list_length (getBreezePartChannelsList (projectBreezePart));

    // Add a communication component in between
    // (source)->channel->(new intermediate)->newChannel->(dest)
    // (component "$BrzHLA" (width partition1 partition2) (source dest))
    struct BreezePartComponentsComponent *newComponent = DeepCopyBreezePartComponentsComponent (source_component);

    setBreezePartComponentsList (projectBreezePart, g_list_prepend (getBreezePartComponentsList (projectBreezePart), newComponent));
    PtrTMPNode node = getBreezePartComponentsComponentTMPNode (newComponent);
    int width = getBreezePartChannelsChannelWidth (channel);

    // Set name
    switch (getBreezePartChannelsChannelType (channel))
    {
    case SyncChannel:
        node->body.list->next->data = NewTMPString ("$BrzMPISync", -1);
        break;
    case PushChannel:
        node->body.list->next->data = NewTMPString ("$BrzMPIPush", -1);
        break;
    case PullChannel:
        node->body.list->next->data = NewTMPString ("$BrzMPIPull", -1);
        break;
    }

    // Set (width partition1 partition2)
    node->body.list->next->next->data =
      NewTMPList (g_list_append (g_list_append (g_list_append (0, NewTMPNumberStr (g_strdup_printf ("%d", width), -1)),
          NewTMPNumberStr (g_strdup_printf ("%d", v1->color), -1)), NewTMPNumberStr (g_strdup_printf ("%d", v2->color), -1)));

    // Set (source dest)
    node->body.list->next->next->next->data =
      NewTMPList (g_list_append (g_list_append (0, NewTMPNumberStr (g_strdup_printf ("%d", chan->num), -1)), NewTMPNumberStr (g_strdup_printf ("%d",
            newChannelNum), -1)));

    // Adjust the channel numbers in the source, dest and new intermediate components
    // Find channel->num in the dest component and replace it by newChannel->num
    PtrTMPNode node2 = getBreezePartComponentsComponentTMPNode (dest_component);
    PtrTMPNode argList2 = node2->body.list->next->next->next->data;

    SearchAndReplace (argList2, chan->num, newChannelNum);

    return TRUE;
}

void on_ExportNewBreze_clicked (GtkButton * button, gpointer user_data)
{
    // save the channels and components lists
    GList *oldChannelList = getBreezePartChannelsList (projectBreezePart);
    GList *oldComponentList = getBreezePartComponentsList (projectBreezePart);

    // Check each channel if it needs to be split into 2 channels + a communication component in between
    ForEachEdge (Partitioning_CheckAndSplitEdge);

    FILE *f = fopen ("flat2.breeze", "w");

    dumpBreezeFile (f, getBreezePartFile (projectBreezePart));

    fclose (f);

    printf ("ok\n");

    // restore the previous channels and components list, to allow for more than one experiment to be done
    oldChannelList->prev = NULL;
    oldComponentList->prev = NULL;
    setBreezePartChannelsList (projectBreezePart, oldChannelList);
    setBreezePartComponentsList (projectBreezePart, oldComponentList);
    UndoSearchAndReplace ();
}

/****************************************/

static gboolean SelectPartitionCut_subfct (struct Edge *edge)
{
    PtrEdgeInfo edgeInfo = edge->data;

    if (!edgeInfo)
        return TRUE;
    struct Chan *chan = edgeInfo->chan;

    if (!chan)
        return TRUE;
    struct Comp *source = chan->source;
    struct Comp *dest = chan->dest;

    if (!source)
        return TRUE;
    if (!dest)
        return TRUE;
    struct Vertex *v1 = source->data;
    struct Vertex *v2 = dest->data;

    if (!v1)
        return TRUE;
    if (!v2)
        return TRUE;

    if (v1->color != v2->color)
    {
        Core_SelectChannel (chan);
    }

    return TRUE;
}

void on_SelectPartitionCut_clicked (GtkButton * button, gpointer user_data)
{
    ForEachEdge (SelectPartitionCut_subfct);
}

/****************************************/

void Partitioning_PrintEdgeInfo (struct Edge *edge)
{
    PtrEdgeInfo edgeInfo = edge->data;

    if (!edgeInfo)
        return;
    struct Chan *chan = edgeInfo->chan;

    if (!chan)
        return;
    struct Comp *source = chan->source;
    struct Comp *dest = chan->dest;

    if (!source)
        return;
    if (!dest)
        return;

    if (nb_comps)
        printf (" Edge weight = %d\n", mat[source->num][dest->num]);
}
