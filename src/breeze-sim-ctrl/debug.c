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
  Foundation, Inc., 59 Temple Place, Suite 330, Bostn, MA 02111-1307 USA

*/

#include <stdlib.h>
#include <string.h>

#include "debug.h"
#include "libspring.h"
#include "selectionview.h"
#include "simtrace.h"
#include "structure.h"
#include "main.h"

#define GetCompChannels_PASSIVE_PORTS 1
#define GetCompChannels_ACTIVE_PORTS 2
#define GetCompChannels_STATE_ANY 0
#define GetCompChannels_STATE_INACTIVE 1
#define GetCompChannels_STATE_ACTIVATED 2
GList *GetCompChannels (struct Comp *comp, int portType, int desiredState)
{
    GList *result = 0;

    if (comp)
    {
        GList *tmp2;

        for (tmp2 = comp->channels; tmp2; tmp2 = tmp2->next)
        {
            struct Chan *chan = tmp2->data;

            if ((portType & GetCompChannels_PASSIVE_PORTS) && chan->dest != comp)
                continue;
            if ((portType & GetCompChannels_ACTIVE_PORTS) && chan->source != comp)
                continue;

            if (desiredState != GetCompChannels_STATE_ANY)
            {
                struct Edge *edge = chan->data;
                int state = GetEdgeState (edge);

                if (desiredState == GetCompChannels_STATE_INACTIVE && state != 0)
                    continue;
                if (desiredState == GetCompChannels_STATE_ACTIVATED && state == 0)
                    continue;
            }

            if (!g_list_find (result, chan) && !g_list_find (selectedChans, chan))
                result = g_list_prepend (result, chan);
        }
    }

    return result;
}

void OnDebugMenu_HighlightDeadlock (GtkMenuItem * menuitem, gpointer user_data)
{
    /*
       -- Algo --
       Start from a selected channel (or a list thereof)
       1. Assume that its passive port is connected to a Synch component => look for the other (unactivated) chans connected to this comp.
       2. Iteratively follow the string of unactivated chans until we reach a component without any unactivated passive port.
       3. From this last component: Find the blocking active port chan.

       Note (&TODO): This only works with a single channel and a single string of components (yet).
     */

    GList *tmp, *tmp2;
    GList *newSelection = NULL;

    if (!selectedChans)
        return;

    // 1
    for (tmp = selectedChans; tmp; tmp = tmp->next)
    {
        struct Chan *chan = tmp->data;

        tmp2 = GetCompChannels (chan->dest, GetCompChannels_PASSIVE_PORTS, GetCompChannels_STATE_INACTIVE);
    }

    for (tmp = tmp2; tmp; tmp = tmp->next)
    {
        struct Chan *otherChan = tmp->data;

        if (otherChan && !g_list_find (selectedChans, otherChan))
            Core_SelectChannel (otherChan);
    }

    // 2
    do
    {
        newSelection = tmp2;
        tmp = 0;

        for (tmp = newSelection; tmp; tmp = tmp->next)
        {
            struct Chan *chan = tmp->data;

            tmp2 = GetCompChannels (chan->source, GetCompChannels_PASSIVE_PORTS, GetCompChannels_STATE_INACTIVE);
        }

        if (tmp2)               // We need the last valid newSelection for step 3 of the algorithm
            g_list_free (newSelection);

        for (tmp = tmp2; tmp; tmp = tmp->next)
        {
            struct Chan *otherChan = tmp->data;

            if (otherChan && !g_list_find (selectedChans, otherChan))
                Core_SelectChannel (otherChan);
        }
    }
    while (tmp2);

    // 3
    for (tmp = selectedChans; tmp; tmp = tmp->next)
    {
        struct Chan *chan = tmp->data;

        tmp2 = GetCompChannels (chan->source, GetCompChannels_ACTIVE_PORTS, GetCompChannels_STATE_ACTIVATED);
    }

    for (tmp = tmp2; tmp; tmp = tmp->next)
    {
        struct Chan *otherChan = tmp->data;

        if (otherChan && !g_list_find (selectedChans, otherChan))
            Core_SelectChannel (otherChan);
    }
}

/*******************************************************************************/

struct BreezePartCallcontextsCallcontext *FindCallcontext (GList * callcontexts, int parentCallcontextNum, int seqnum)
{
    GList *tmp;

    for (tmp = callcontexts; tmp; tmp = tmp->next)
    {
        struct BreezePartCallcontextsCallcontext *callcontext = tmp->data;

        if (getBreezePartCallcontextsCallcontextParentNum (callcontext) != parentCallcontextNum)
            continue;
        if (seqnum == 0)
            return callcontext;
        seqnum--;
    }

    return NULL;
}

struct Chan *FindChanInCallcontext (int callcontextNum, int seqnum)
{
/*
	int i;
	GList *channels = getBreezePartChannelsList (projectBreezePart);
	GList *tmp;
	struct BreezePartChannelsChannel *channel = NULL;

	for (tmp = channels; tmp; tmp=tmp->next) {
		channel = tmp->data;
		struct Position *pos = getBreezePartChannelsChannelPosition (channel);
		if (pos->context != callcontextNum)
			continue;
		seqnum--;
		if (seqnum==0)
			goto found;
	}

	return NULL;

  found:
    for (i = 1; i < (int) allChans->len; i++) {
        struct Chan *chan = g_ptr_array_index (allChans, i);
		if (chan->channel == channel)
			return chan;
	}

	return NULL;
*/

    char *str = g_strdup_printf (":%d", seqnum);
    int i;

    for (i = 1; i < (int) allChans->len; i++)
    {
        struct Chan *chan = g_ptr_array_index (allChans, i);
        struct BreezePartChannelsChannel *channel = chan->channel;
        struct Position *pos = getBreezePartChannelsChannelPosition (channel);

        if (pos->context != callcontextNum)
            continue;
        char *chan_name = getBreezePartChannelsChannelName (channel);

        if (chan_name && ENDS_WITH (chan_name, str))
        {
            g_free (str);
            return chan;
        }
    }

    g_free (str);
    return NULL;
}

struct Chan *FindChanFromCharliesString (char *buf)
{
    static int mainCallcontextNum = -1;
    GList *callcontexts = getBreezePartCallcontextsList (projectBreezePart);
    struct BreezePartCallcontextsCallcontext *callcontext;

    if (mainCallcontextNum == -1)
    {
        // Get main callcontext's num
        callcontext = g_list_last (callcontexts)->data;
        mainCallcontextNum = getBreezePartCallcontextsCallcontextNum (callcontext);
    }

    int currentCallcontextNum = mainCallcontextNum;

    char **str_elts = g_strsplit (buf, "/", 0);
    int i;

    for (i = 0; str_elts[i]; i++)
    {
        char *part_name = str_elts[i];

        if (part_name[0] == 'I')
        {
            int comp_num = atoi (part_name + 1);

            if (!comp_num)
                continue;
            callcontext = FindCallcontext (callcontexts, currentCallcontextNum, comp_num);
            if (!callcontext)
            {
                g_warning ("callcontext not found");
                break;
            }
            currentCallcontextNum = getBreezePartCallcontextsCallcontextNum (callcontext);
        } else if (part_name[0] == 'c')
        {
            int chan_num = atoi (part_name + 1);
            struct Chan *chan = FindChanInCallcontext (currentCallcontextNum, chan_num);

//    struct Chan *chan = g_ptr_array_index (allChans, i);
            if (chan)
            {
                return chan;
            } else
                g_warning ("not finding chan");
        } else if (strcmp (part_name, "testbench") && strcmp (part_name, "the_monstreous_beast_Module_Balsa___spa__V5T"))
            g_warning ("shouldn't reach here");
    }
    return NULL;
}

void OnDebugMenu_HighlightCharliesSlowestPath (GtkMenuItem * menuitem, gpointer user_data)
{
    system ("grep -e '/c[0-9][^/]*' s2alog.txt | grep -v _0n__ |sort -u >s2alog.processed");

    FILE *f = fopen ("s2alog.processed", "r");

    if (!f)
    {
        g_warning ("Error in HighlightCharliesSlowestPath: s2alog not found\n");
        return;
    }

    while (!feof (f))
    {
        char buf[100000];

        if (!fgets (buf, 100000, f))
            break;

        struct Chan *chan = FindChanFromCharliesString (buf);

        if (chan && !g_list_find (selectedChans, chan))
            Core_SelectChannel (chan);
    }
}

void OnDebugMenu_TranslateCharlieToTraceFile (GtkMenuItem * menuitem, gpointer user_data)
{
    FILE *f = fopen ("s2alog.txt", "r");

    if (!f)
    {
        g_warning ("Error in TranslateCharlieToTraceFile: s2alog.txt not found\n");
        return;
    }

    char buf[100000];

    fgets (buf, 100000, f);
    fgets (buf, 100000, f);
    fgets (buf, 100000, f);
    fgets (buf, 100000, f);
    fgets (buf, 100000, f);
    fgets (buf, 100000, f);
    fgets (buf, 100000, f);
    fgets (buf, 100000, f);
    fgets (buf, 100000, f);

    GList *transitions = NULL;

    while (!feof (f))
    {
        if (!fgets (buf, 100000, f))
            break;

        struct Chan *chan = FindChanFromCharliesString (buf);

        if (chan)
        {
            if (!fgets (buf, 100000, f))
                break;
            if (!fgets (buf, 100000, f))
                break;
//            int updown = atoi (buf);

            if (!fgets (buf, 100000, f))
                break;
            transitions = g_list_prepend (transitions, g_strdup_printf ("%d requp", chan->num));
        } else
        {
            if (!fgets (buf, 100000, f))
                break;
            if (!fgets (buf, 100000, f))
                break;
            if (!fgets (buf, 100000, f))
                break;
        }
    }

    fclose (f);
    f = fopen ("charlie.hhh", "w");
    fprintf (f, "time 0\n");
    g_assert (f);
    GList *tmp = transitions;
    int t = 1;

    for (; tmp; tmp = tmp->next)
    {
        fprintf (f, "time %d\n", t++);
        fprintf (f, "signal %s\n", (char *) tmp->data);
    }
}
