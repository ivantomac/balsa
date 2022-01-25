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

#include "behaviour.h"

//TMP (from callcontexttree.c)
extern GPtrArray *allComps;     // = NULL; // array of struct Chan*

//\TMP

gboolean FollowComponentsUp (struct Comp *comp)
{
    struct Chan *mainChan = comp->mainChannel;

    printf ("mainChan = %d\n", mainChan->num);
    if (mainChan->num <= 1)
        return TRUE;
    struct Comp *prevComp = mainChan->source;
    char *prevcompname = getBreezePartComponentsComponentName (prevComp->component);

    printf ("previous comp = %s\n", prevcompname);
    if (!strcmp (prevcompname, "$BrzConcur")
      || !strcmp (prevcompname, "$BrzWireFork") || !strcmp (prevcompname, "$BrzSequence"))
        return FollowComponentsUp (prevComp);
    else
        return FALSE;
}

void CreateMainLoopGroupForComp_DoITrecursive (struct Comp *comp, struct CallcontextTreeItem *hashedStruct,
  struct CallcontextTreeItem *hashedStruct2);
void CreateMainLoopGroupForComp (struct Comp *comp, struct BreezePart *breezePart)
{
    // If comp is the main component of the group, then just change the group's name. Otherwise, create a new grouip
    struct Chan *mainChan = comp->mainChannel;

    //    struct Comp *prevComp = mainChan->source;

    if (0)
    {                           //mainChan->num<=1 || (mainChan->callcontext != prevComp->mainChannel->callcontext)) {
        struct Chan *chan = comp->mainChannel;
        struct CallcontextTreeItem *cctitem = chan->callcontext;
        struct BreezePartCallcontextsCallcontext *callcontext = cctitem->callcontext;
        char *name = getBreezePartCallcontextsCallcontextName (callcontext);
        char *newname = g_strdup_printf ("ML_%s", name);

        setBreezePartCallcontextsCallcontextName (callcontext, newname);
    } else
    {
        printf ("// Create new ML_ group\n");
        static int ML_count = 0;

        ML_count++;

        struct ThreadGroupsItem *threadGroup = g_new0 (struct ThreadGroupsItem, 1);

        threadGroup->name = g_strdup_printf ("ML_%d", ML_count);
        threadGroup->comps = g_ptr_array_new ();
        g_ptr_array_add (breezePart->threadGroups, threadGroup);

        // Remove comp from old group
        //        g_ptr_array_remove (comp->threadGroup->comps, comp);

        // Add comps to new group
        comp->threadGroup = threadGroup;
        g_ptr_array_add (threadGroup->comps, comp);

        // Create new callcontext group
        struct Chan *chan2 = comp->mainChannel;
        struct CallcontextTreeItem *hashedStruct = chan2->callcontext;

        struct CallcontextTreeItem *hashedStruct2 = g_new0 (struct CallcontextTreeItem, 1);
        int *key = g_new0 (int, 1);
        static int crapnum = 123454532;

        *key = crapnum++;       //crap
        g_hash_table_insert (breezePart->callcontextTree_hashtable, (gpointer) key, hashedStruct2);

        hashedStruct2->parent = hashedStruct;
        hashedStruct->childs = g_list_prepend (hashedStruct->childs, hashedStruct2);

        struct Comp *prevComp = mainChan->source;
        char *prevcompname = getBreezePartComponentsComponentName (prevComp->component);

        if (!strcmp (prevcompname, "$BrzSequence"))
        {
            CreateMainLoopGroupForComp_DoITrecursive (prevComp, hashedStruct, hashedStruct2);
            hashedStruct2->type = 2;

            // Create new callcontext group
            chan2 = comp->mainChannel;
            hashedStruct = chan2->callcontext;

            hashedStruct2 = g_new0 (struct CallcontextTreeItem, 1);
            key = g_new0 (int, 1);

            *key = crapnum++;   //crap
            g_hash_table_insert (breezePart->callcontextTree_hashtable, (gpointer) key, hashedStruct2);

            hashedStruct2->parent = hashedStruct;
            hashedStruct->childs = g_list_prepend (hashedStruct->childs, hashedStruct2);

            CreateMainLoopGroupForComp_DoITrecursive (comp, hashedStruct, hashedStruct2);
            hashedStruct2->type = 1;
        } else
        {
            CreateMainLoopGroupForComp_DoITrecursive (comp, hashedStruct, hashedStruct2);
            hashedStruct2->type = 3;
        }
    }
}
void CreateMainLoopGroupForComp_DoITrecursive_RecTruc (PtrTMPNode node, struct Comp *comp, struct CallcontextTreeItem *hashedStruct,
  struct CallcontextTreeItem *hashedStruct2);
void CreateMainLoopGroupForComp_DoITrecursive (struct Comp *comp, struct CallcontextTreeItem *hashedStruct, struct CallcontextTreeItem *hashedStruct2)
{
    struct Chan *chan3 = comp->mainChannel;
    struct CallcontextTreeItem *hs = chan3->callcontext;

    while (hs != hashedStruct && hs != hashedStruct2)
    {
        if (hs)
            hs = hs->parent;
        else
            return;
    }
    if (hs == hashedStruct2)
        return;
    hashedStruct2->comps = g_list_prepend (hashedStruct2->comps, comp);
    /*hashedStruct->comps */ chan3->callcontext->comps =
      g_list_remove ( /*hashedStruct->comps */ chan3->callcontext->comps,
      comp);
    chan3->callcontext = hashedStruct2;

    {
        PtrTMPNode compNode = getBreezePartComponentsComponentTMPNode (comp->component);
        PtrTMPNode compChansNode = compNode->body.list->next->next->next->data;

        //  GList *argList = compChansNode->body.list;

        CreateMainLoopGroupForComp_DoITrecursive_RecTruc (compChansNode, comp, hashedStruct, hashedStruct2);
    }
}
void CreateMainLoopGroupForComp_DoITrecursive_RecTruc (PtrTMPNode node, struct Comp *comp, struct CallcontextTreeItem *hashedStruct,
  struct CallcontextTreeItem *hashedStruct2)
{
    switch (node->type)
    {
    case TMPList:
        {
            GList *elt = node->body.list;

            for (; elt; elt = elt->next)
            {
                PtrTMPNode node2 = elt->data;

                CreateMainLoopGroupForComp_DoITrecursive_RecTruc (node2, comp, hashedStruct, hashedStruct2);
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

            //                                    if (!comp2->threadGroup)
            CreateMainLoopGroupForComp_DoITrecursive (comp2, hashedStruct, hashedStruct2);
        }
        break;

    default:
        {
            char *compname = getBreezePartComponentsComponentName (comp->component);

            fprintf (stderr, "oh... error with component %s\n", compname);
        }
    }
}

void DiscoverBehaviourGroups (struct BreezePart *breezePart)
{
    unsigned int i;

    for (i = 0; i < allComps->len; i++)
    {
        struct Comp *comp = g_ptr_array_index (allComps, i);
        struct BreezePartComponentsComponent *component = comp->component;
        char *compname = getBreezePartComponentsComponentName (component);

        if (!strcmp (compname, "$BrzLoop"))
        {
            static int c = 0;

            printf ("found %d loops\n", ++c);
            if (FollowComponentsUp (comp))
            {
                CreateMainLoopGroupForComp (comp, breezePart);
            } else
                printf ("---------Bad one!-----\n");
        }
    }
}
