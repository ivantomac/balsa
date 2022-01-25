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

	`pattern_analysis.c'
	Pattern analysis for compressed out-of-order trace

*/

#ifdef DO_PATTERN_ANALYSIS

#include "pattern_analysis.h"
#include <stdlib.h>
#include <stdio.h>
#include <glib.h>

static int nb_patterns = 0;

struct nodeStruct_
{
    int chan_num;
    int event_type;             // char *typekw[4] = { "requp", "reqdown", "ackup", "ackdown" };
    int pattern_num;
};

static GNode *rootNode, *currentNode;

static FILE *file1, *file2;

void PatternAnalysis_Initialise (char *breezeFilename)
{
    struct nodeStruct_ *data = g_new0 (struct nodeStruct_, 1);

    rootNode = g_node_new (data);
    currentNode = rootNode;

    file1 = fopen ("hhh1", "w");
    file2 = fopen ("hhh2", "w");
}

void PatternAnalysis_StartNewThread (void)
{
    if (!((struct nodeStruct_ *) currentNode->data)->pattern_num)
    {
        nb_patterns++;
        ((struct nodeStruct_ *) currentNode->data)->pattern_num = nb_patterns;

        fprintf (file1, "New Pattern %d:", nb_patterns);
        GNode *tmp = currentNode;

        for (; tmp; tmp = tmp->parent)
        {
            struct nodeStruct_ *data = tmp->data;

            fprintf (file1, " %d-%d", data->chan_num, data->event_type);
        }
        fprintf (file1, "\n");
    }

    fprintf (file2, "pattern %d\n", ((struct nodeStruct_ *) currentNode->data)->pattern_num);

    currentNode = rootNode;
}

void PatternAnalysis_AppendSignal (int chan_num, int event_type, unsigned long long time)
{
    // Check if the node already exists
    GNode *child = g_node_first_child (currentNode);

    for (; child; child = g_node_next_sibling (child))
    {
        struct nodeStruct_ *data = child->data;

        if (data->chan_num == chan_num && data->event_type == event_type)
        {
            // If yes: use it
            currentNode = child;
            return;
        }
    }

    // If not: create a new one
    struct nodeStruct_ *data = g_new0 (struct nodeStruct_, 1);

    data->chan_num = chan_num;
    data->event_type = event_type;

    currentNode = g_node_prepend_data (currentNode, data);
}

#endif
