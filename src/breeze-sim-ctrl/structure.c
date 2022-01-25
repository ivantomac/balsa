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

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <stdlib.h>
#include <breeze/libbreeze.h>

#include "structure.h"
#include "main.h"
#include "mainwindow2.h"
#include "libspring.h"
#include "core.h"

GHashTable *callcontexts_hashtable;
GPtrArray *threadGroups_ptrarray;
struct BreezePart *projectBreezePart = NULL;

void InitStructure (void)
{
    struct BreezeFile *breezeStruct = breezeInitParse (projectName);

    if (breezeStruct == NULL)
    {
        fprintf (stderr, "Error (opening or parsing) in file %s.breeze\n", projectName);
        exit (EXIT_FAILURE);
    }

    if (partName)
    {
        projectBreezePart = getBreezePartByName (breezeStruct, partName);
        if (!projectBreezePart)
            fprintf (stderr, "Error: Part %s not defined in %s.breeze\nUsing default part.\n", partName, projectName);
    }

    if (!projectBreezePart)
    {
        GList *breezeParts = getBreezeParts (breezeStruct);

        if (breezeParts == NULL)
        {
            fprintf (stderr, "Error: No part defined in %s.breeze\n", projectName);
            exit (EXIT_FAILURE);
        }
        projectBreezePart = breezeParts->data;
    }

    LibBreeze_FlattenBreezePart (projectBreezePart);

    GtkWidget *menuThreadStructure = GTK_WIDGET (gtk_object_get_data (GTK_OBJECT (MainWindow2), "menuThreadStructure"));
    GtkWidget *menuBehaviourStructure = GTK_WIDGET (gtk_object_get_data (GTK_OBJECT (MainWindow2), "menuBehaviourStructure"));

    if (GTK_CHECK_MENU_ITEM (menuThreadStructure)->active)
        libbreeze_TEMPORARY_FLAG_threadgroups = TRUE;
    else
        libbreeze_TEMPORARY_FLAG_threadgroups = FALSE;
    if (GTK_CHECK_MENU_ITEM (menuBehaviourStructure)->active)
        libbreeze_TEMPORARY_FLAG_behaviourgroups = TRUE;
    else
        libbreeze_TEMPORARY_FLAG_behaviourgroups = FALSE;

    callcontexts_hashtable = generateBreezePartCallcontextTree (projectBreezePart);
    threadGroups_ptrarray = generateBreezePartThreadGroups (projectBreezePart);
}

void Structure_Init (void)
{
}

struct BreezePart *Structure_GetFlattenedBreezePart (void)
{
    if (!projectBreezePart)
        InitStructure ();

    return projectBreezePart;
}

/*****************************************************************************************/
/*****************************************************************************************/
/*****************************************************************************************/

static void CreateVerticesForCallcontextList_subfct (gpointer key, gpointer value, gpointer user_data)
{
    struct CallcontextTreeItem *callcontextTreeItem = value;
    struct BreezePartCallcontextsCallcontext *callcontext = callcontextTreeItem->callcontext;

    struct Vertex *v;
    struct Vertex *parent = callcontextTreeItem->parent ? ((struct CallcontextTreeData *) callcontextTreeItem->parent->data)->vertex : rootVertex;

    char *name;

    switch (callcontextTreeItem->type)
    {
    case 0:
        name = g_strdup (getBreezePartCallcontextsCallcontextName (callcontext));
        break;
    case 1:
        name = g_strdup ("ML_*");
        break;
    case 2:
        name = g_strdup ("MOD_*");
        break;
    case 3:
        name = g_strdup ("MOD_L_*");
        break;
    default:
        name = g_strdup ("???");
    }

    v = AddNewVertexWithData (name, TRUE, parent, g_new0 (VertexInfo, 1));

    callcontextTreeItem->data = g_new0 (struct CallcontextTreeData, 1);

    ((struct CallcontextTreeData *) callcontextTreeItem->data)->vertex = v;

    // Show small subtrees and First-level procedure, but not Balsa* [internal] procedures
    if ((g_list_length (callcontextTreeItem->comps) <= 5
        || !callcontextTreeItem->parent || !callcontextTreeItem->parent->parent) && !BEGINS_WITH (name, "Balsa") && !BEGINS_WITH (name, "ML"))
    {
        v->areSubgroupsExpanded = TRUE;
        v->areChildsVisible = TRUE;
    }

    GList *tmp = callcontextTreeItem->comps;

    for (; tmp; tmp = tmp->next)
    {
        struct Comp *comp = tmp->data;
        struct Vertex *v2;
        char *nickname = generateBreezePartComponentsComponentNickname (comp->component);

        v2 = AddNewVertexWithData (nickname, FALSE, v, g_new0 (VertexInfo, 1));
        comp->data = v2;
        ((PtrVertexInfo) v2->data)->infoForGraphView.comp = comp;
        g_free (nickname);
    }

    g_free (name);
}
static void CreateVerticesForCallcontextList (struct BreezePart *breezePart)
{
    callcontexts_hashtable_foreachWithParent (CreateVerticesForCallcontextList_subfct);
}

/*
struct Vertex *CreateVerticesForThreadGroups_Parent = NULL;
static void CreateVerticesForThreadGroups_subfct (gpointer threadGroupPtr, gpointer user_data, gpointer zero)
{
    struct ThreadGroupsItem *threadGroup = threadGroupPtr;
    char *name = g_strdup (threadGroup->name);
    struct Vertex *v, *parent;
    parent = CreateVerticesForThreadGroups_Parent;
    v = AddNewVertexWithData (name, TRUE, parent, g_new0 (VertexInfo, 1));
    v->areSubgroupsExpanded = TRUE;
    v->areChildsVisible = TRUE;
    if (CreateVerticesForThreadGroups_Parent == rootVertex)
        CreateVerticesForThreadGroups_Parent = v;

    unsigned int i;
    GPtrArray *comps = threadGroup->comps;
    for (i=0; i<comps->len; i++) {
        struct Comp *comp = g_ptr_array_index (comps, i);
        struct BreezePartComponentsComponent *component = comp->component;
        struct Vertex *v2;
        char *nickname = generateBreezePartComponentsComponentNickname (component);

        v2 = AddNewVertexWithData (nickname, FALSE, v, g_new0 (VertexInfo, 1));
        comp->data = v2;
        ((PtrVertexInfo) v2->data)->infoForGraphView.comp = comp;
        g_free (nickname);
    }
}
static void CreateVerticesForThreadGroups (struct BreezePart *breezePart)
{
    CreateVerticesForThreadGroups_Parent = rootVertex;
    ThreadGroups_Foreach (breezePart, CreateVerticesForThreadGroups_subfct, 0);
}
*/

static void InitVertices (struct BreezePart *breezePart)
{
    AddNewVertexWithData ("<root>", TRUE, NULL, g_new0 (VertexInfo, 1));

    CreateVerticesForCallcontextList (breezePart);
    //    else
    //        CreateVerticesForThreadGroups (breezePart);
}

static void InitEdges (struct BreezePart *breezePart)
{
    int i;

    for (i = 1; i < (int) allChans->len; i++)
    {
        struct Chan *chan = g_ptr_array_index (allChans, i);
        struct Vertex *v1 = chan->source ? chan->source->data : NULL;
        struct Vertex *v2 = chan->dest ? chan->dest->data : NULL;

        //TMP: add interface ports
        if (!v1)
            v1 = AddNewVertexWithData ("X", FALSE, rootVertex, g_new0 (VertexInfo, 1));
        if (!v2)
            v2 = AddNewVertexWithData ("X", FALSE, rootVertex, g_new0 (VertexInfo, 1));

        /*if (v1 && v2) */
        {
            struct Edge *edge = AddNewEdge (v1, v2);

            chan->data = edge;
            edge->data = g_new0 (EdgeInfo, 1);
            ((PtrEdgeInfo) edge->data)->chan = chan;
            ((PtrEdgeInfo) edge->data)->trace = g_ptr_array_new ();
            ((PtrEdgeInfo) edge->data)->posInTrace = 0;
            ((PtrEdgeInfo) edge->data)->valueTrace = g_ptr_array_new ();
            ((PtrEdgeInfo) edge->data)->posInValueTrace = 0;

            switch (chan->direction)
            {
            case ChanDirection_Sync:
                edge->direction = EdgeDirection_Sync;
                break;
            case ChanDirection_Pull:
                edge->direction = EdgeDirection_Pull;
                break;
            case ChanDirection_Push:
                edge->direction = EdgeDirection_Push;
                break;
            case ChanDirection_Unknown:
                edge->direction = EdgeDirection_Sync;
                break;
            }

            if (!strcmp (v1->name, "W.") || !strcmp (v2->name, "W.") || !strcmp (v1->name, "||") || !strcmp (v2->name, "||"))
                edge->type |= EDGE_TYPE_WIREFORK_TREE;

            struct Vertex *v = v1->parent;

            for (; v; v = v->parent)
            {
                if (BEGINS_WITH (v->name, "ML_") || BEGINS_WITH (v->name, "MOD_L_"))
                {
                    edge->type |= EDGE_TYPE_MODULE_MAINLOOP;
                    break;
                }
            }
        }
    }
}

static struct Vertex *InitialiseTestHarnessGroups_connectedTestHarnessGroup = NULL;
gboolean InitialiseTestHarnessGroups_FindConnectedTestHarnessGroup (struct Vertex *v)
{
    for (; v; v = v->parent)
        if (BEGINS_WITH (v->name, "TEST_HARNESS"))
        {
            InitialiseTestHarnessGroups_connectedTestHarnessGroup = v;
            return FALSE;
        }

    return TRUE;
}

void InitialiseTestHarnessGroups (void)
{
    struct Vertex *balsaVertex = NULL;

    GList *childs = rootVertex->childs;

    for (; childs; childs = childs->next)
    {
        struct Vertex *child = childs->data;
        char *name = GetVertexName (child);

        if (!strcmp (name, "balsa"))
        {
            balsaVertex = child;
            break;
        }
    }

    if (!balsaVertex)
        return;

    // Create new TEST_HARNESS_ groups for appropriate group vertices
    // and MAIN_INIT group for the toplevel Wirefork
    gboolean firstWireforkComponent = TRUE;

    childs = g_list_copy (balsaVertex->childs);
    for (; childs; childs = childs->next)
    {
        struct Vertex *child = childs->data;

        if (child->isGroup)
        {
            if (BEGINS_WITH (child->name, "Balsa"))
            {
                struct Vertex *v = AddNewVertexWithData ("TEST_HARNESS", TRUE, balsaVertex,
                  g_new0 (VertexInfo, 1));

                v->childs = g_list_prepend (NULL, child);
                balsaVertex->childs = g_list_remove (balsaVertex->childs, child);
                child->parent = v;
            }
        } else if (firstWireforkComponent && (!strcmp (child->name, "W.") || !strcmp (child->name, "||")))
        {
            struct Vertex *v = AddNewVertexWithData ("MAIN_INIT", TRUE, balsaVertex,
              g_new0 (VertexInfo, 1));

            v->childs = g_list_prepend (NULL, child);
            balsaVertex->childs = g_list_remove (balsaVertex->childs, child);
            child->parent = v;
            firstWireforkComponent = FALSE;
        }
    }
    g_list_free (childs);

    // Move other vertices into TEST_HARNESS_ groups
    childs = g_list_copy (balsaVertex->childs);
    for (; childs; childs = childs->next)
    {
        struct Vertex *child = childs->data;

        if (!child->isGroup)
        {
            InitialiseTestHarnessGroups_connectedTestHarnessGroup = NULL;
            ForEachVertexConnectedTo (child, InitialiseTestHarnessGroups_FindConnectedTestHarnessGroup);

            struct Vertex *group = InitialiseTestHarnessGroups_connectedTestHarnessGroup;

            if (group)
            {
                group->childs = g_list_prepend (group->childs, child);
                balsaVertex->childs = g_list_remove (balsaVertex->childs, child);
                child->parent = group;
            }
        }
    }
    g_list_free (childs);
}

struct Vertex *Structure_GetRootVertex (void)
{
    if (!rootVertex)
    {
        struct BreezePart *breezePart = Structure_GetFlattenedBreezePart ();

        InitVertices (breezePart);
        InitEdges (breezePart);

        ComputeThreadGroups ();
        InitialiseTestHarnessGroups ();
    }
    return rootVertex;
}
