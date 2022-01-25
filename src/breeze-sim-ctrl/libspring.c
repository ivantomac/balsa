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
#include <glib.h>
#include <math.h>
#include <string.h>
#include <unistd.h>

#include "libspring_internals.h"
#include "libspring.h"
#include "main.h"
#include "drawing.h"
#include <breeze/libbreeze.h>

#include <signal.h>
#include <stdio.h>
#define ASSERT(x) {if (!(x)) {fprintf(stderr,"ASSERT failed\n"); raise(SIGUSR1);}}
#define SQ(x) ((x)*(x))
#define DPI 72

GPtrArray *vertices = NULL;
GPtrArray *edges = NULL;
GPtrArray *threadGroups = NULL;

struct Vertex *rootVertex;
struct Vertex *visibleRootVertex;

GList *connectedVertices_listOfPorts = NULL;

gboolean libspring_LayoutProblem = 0;
gboolean useControlForLayout = 0;

void reset_libspring (void)
{
    vertices = NULL;
    edges = NULL;
    rootVertex = NULL;
    visibleRootVertex = NULL;
}

void SetVisibleRootVertex (struct Vertex *v)
{
    if (v)
        visibleRootVertex = v;
    else
        visibleRootVertex = rootVertex;
}

struct Vertex *AddNewVertex_NoParent (const char *name)
{
    struct Vertex *newVertex = g_new0 (struct Vertex, 1);

    if (vertices == NULL)
    {
        vertices = g_ptr_array_new ();
        rootVertex = newVertex;
        rootVertex->isGroup = TRUE;
        rootVertex->areSubgroupsExpanded = TRUE;
        rootVertex->areChildsVisible = TRUE;
        visibleRootVertex = rootVertex;
    }

    newVertex->name = g_strdup (name);

    g_ptr_array_add (vertices, newVertex);

    return newVertex;
}

struct Vertex *AddNewVertex (const char *name, gboolean isGroup, struct Vertex *parent)
{
    struct Vertex *newVertex = g_new0 (struct Vertex, 1);

    if (vertices == NULL)
    {
        vertices = g_ptr_array_new ();
        rootVertex = newVertex;
        rootVertex->isGroup = TRUE;
        rootVertex->areSubgroupsExpanded = TRUE;
        rootVertex->areChildsVisible = TRUE;
        visibleRootVertex = rootVertex;
    }

    newVertex->name = g_strdup (name);
    newVertex->isGroup = isGroup;

    if (isGroup)
    {
        newVertex->isShrunk = FALSE;
//        newVertex->areSubgroupsExpanded = TRUE;
    }

    if (parent)
    {
        newVertex->parent = parent;
        parent->childs = g_list_prepend (parent->childs, newVertex);
    } else if (!BEGINS_WITH (name, "<root>"))
        rootVertex->childs = g_list_prepend (rootVertex->childs, newVertex);

    g_ptr_array_add (vertices, newVertex);

    return newVertex;
}

struct Vertex *AddNewVertexWithData (const char *name, gboolean isGroup, struct Vertex *parent, void *data)
{
    struct Vertex *v = AddNewVertex (name, isGroup, parent);

    SetVertexData (v, data);
    return v;
}

struct Edge *NewEdge (struct Vertex *source, struct Vertex *dest)
{
    struct Edge *newEdge = g_new0 (struct Edge, 1);

    if (edges == NULL)
        edges = g_ptr_array_new ();

    newEdge->source = source;
    newEdge->dest = dest;

    return newEdge;
}

struct Edge *AddNewEdge (struct Vertex *source, struct Vertex *dest)
{
    struct Edge *newEdge = NewEdge (source, dest);

    g_ptr_array_add (edges, newEdge);
    return newEdge;
}

gboolean ComputeThreadGroups_subfct (struct Vertex * vertex)
{
    return TRUE;
}

void ComputeThreadGroups ()
{
    ForEachVertex (ComputeThreadGroups_subfct);
}

void InitialiseVertexSizeRecursively (struct Vertex *v)
{
    if (v == NULL)
        v = rootVertex;

    {
        GList *tmp;
        int size = 0;

        // double maxsize = 0;
        int nbChilds = g_list_length (v->childs);

        for (tmp = v->childs; tmp; tmp = tmp->next)
        {
            struct Vertex *v2 = tmp->data;

            InitialiseVertexSizeRecursively (v2);
            size += v2->rel_sizeX * v2->rel_sizeY;
            //     maxsize = MAX (maxsize, v2->rel_size);
        }

        switch (nbChilds)
        {
        case 0:
            v->rel_sizeX = 1;   //50
            v->rel_sizeY = 1;   //50
            break;
        case 1:
            v->rel_sizeX = 1.01 * sqrt (size);
            v->rel_sizeY = 1.01 * sqrt (size);
            break;
        default:
            v->rel_sizeX = sqrt (size); //(2.5 - 1/(double)nbChilds) * maxsize;
            v->rel_sizeY = sqrt (size); //(2.5 - 1/(double)nbChilds) * maxsize;
        }
    }
}

void InitialiseVertexPositionRecursively (struct Vertex *v, double x, double y)
{
    if (v == NULL)
    {
        v = rootVertex;
        x = v->rel_posX;
        y = v->rel_posY;
    }

    {
        GList *tmp;
        int nbChilds = g_list_length (v->childs);
        double angle = 2 * M_PI / (double) nbChilds;
        double currentAngle = 0;
        double maxsize = 0;

        v->rel_posX = x;
        v->rel_posY = y;

        if (nbChilds > 1)
        {
            double maxrelsize = 0;
            double sumrelsize = 0;
            double freespace;

            for (tmp = v->childs; tmp; tmp = tmp->next)
            {
                struct Vertex *v2 = tmp->data;

                maxrelsize = MAX (maxrelsize, v2->rel_sizeX);
                sumrelsize += v2->rel_sizeX;
            }
            freespace = 2 * 3.1416 * maxrelsize - 2 * sumrelsize;
            if (freespace < 0)
            {
                freespace = 0;
                maxsize = maxrelsize + sumrelsize / 3.1416;
                maxrelsize = sumrelsize / 3.1416;
            } else
                maxsize = 2 * maxrelsize; //+v->rel_size;

            for (tmp = v->childs; tmp; tmp = tmp->next)
            {
                struct Vertex *v2 = tmp->data;
                double x2, y2;

                currentAngle += v2->rel_sizeX / maxrelsize;
                x2 = maxrelsize * cos (currentAngle);
                y2 = maxrelsize * sin (currentAngle);
                InitialiseVertexPositionRecursively (v2, x2, y2);
                currentAngle += v2->rel_sizeX / maxrelsize;
                currentAngle += (freespace / nbChilds) / maxrelsize;
            }
        } else if (nbChilds == 1)
        {
            struct Vertex *v2 = v->childs->data;
            double x2, y2;

            x2 = 0;
            y2 = 0;
            maxsize = v->rel_sizeX;
            InitialiseVertexPositionRecursively (v2, x2, y2);
            currentAngle += angle;
        }

        v->circle_containing_childs_sizeX = maxsize;
        v->circle_containing_childs_sizeY = maxsize;
    }
}

void ComputeRealVertexSizeAndPositionRecursively (struct Vertex *v)
{
    if (v == NULL)
    {
        v = rootVertex;
        v->posX = v->rel_posX;
        v->posY = v->rel_posY;
        v->sizeX = v->rel_sizeX;
        v->sizeY = v->rel_sizeY;
    }

    {
        GList *tmp;

        // v->posX = v->rel_posX + x;
        // v->posY = v->rel_posY + y;

        for (tmp = v->childs; tmp; tmp = tmp->next)
        {
            struct Vertex *v2 = tmp->data;
            double scalingX = 1; //v->sizeX/v->circle_containing_childs_sizeX;
            double scalingY = 1; //v->sizeY/v->circle_containing_childs_sizeY;

            v2->posX = v2->rel_posX * scalingX; // + v->posX;
            v2->posY = v2->rel_posY * scalingY; // + v->posY;
            v2->sizeX = v2->rel_sizeX * scalingX;
            v2->sizeY = v2->rel_sizeY * scalingY;
            ComputeRealVertexSizeAndPositionRecursively (v2);
        }

        // v->circle_containing_childs_size = maxsize;
    }
}

// Accessing Vertices
void ForEachVertex (CALLBACK_VERTEX fct)
{
/* Doesn't work - I don't know why
    int i;
    for (i=0; i<vertices->len; i++) {
	struct Vertex *vertex = g_ptr_array_index (vertices, i);
	fct (vertex);
    }
*/
    ForEachVertexRecursive (NULL, fct, NULL);
}

void ForEachVisibleVertex (CALLBACK_VERTEX fct)
{
    ForEachVisibleVertexRecursive (NULL, fct, NULL);
}

void ForEachVertexRecursive (struct Vertex *vertex, CALLBACK_VERTEX fct_begin, CALLBACK_VERTEX fct_end)
{
    GList *tmp;

    if (!vertex)
        vertex = rootVertex;

    if (fct_begin)
        if (!fct_begin (vertex))
            goto jump_childs;

    for (tmp = vertex->childs; tmp;)
    {
        struct Vertex *v2 = tmp->data;

        tmp = tmp->next;
        ForEachVertexRecursive (v2, fct_begin, fct_end);
    }

  jump_childs:
    if (fct_end)
        fct_end (vertex);
}

void ForEachVisibleVertexRecursive (struct Vertex *vertex, CALLBACK_VERTEX fct_begin, CALLBACK_VERTEX fct_end)
{
    GList *tmp;

    if (!vertex)
        vertex = visibleRootVertex;

    if (vertex->isHidden)
        return;

    if (fct_begin)
        if (!fct_begin (vertex))
            goto jump_childs;

    for (tmp = vertex->childs; tmp;)
    {
        struct Vertex *v2 = tmp->data;

        if (vertex->areChildsVisible || (v2->isGroup && vertex->areSubgroupsExpanded))
            ForEachVisibleVertexRecursive (v2, fct_begin, fct_end);

        tmp = tmp->next;
    }

  jump_childs:
    if (fct_end)
        fct_end (vertex);
}

void ForEachVisibleVertexRecursiveAndPorts (struct Vertex *vertex, CALLBACK_VERTEX fct_begin, CALLBACK_VERTEX fct_end)
{
    // Process ports
    GList *tmp;

    for (tmp = connectedVertices_listOfPorts; tmp; tmp = tmp->next)
    {
        struct ConnectedVertex *con = tmp->data;
        struct Vertex *v = NULL;

        switch (con->visibleVertices)
        {
        case 1:
            v = con->edge.dest;
            break;
        case 2:
            v = con->edge.source;
            break;
        }
        if (v && fct_begin (v) && fct_end)
            fct_end (v);
    }

    // Call the normal function
    ForEachVisibleVertexRecursive (vertex, fct_begin, fct_end);
}

/*
void ForEachVertexRecursiveRemove (struct Vertex *vertex, CALLBACK_VERTEX fct_begin, CALLBACK_VERTEX fct_end, CALLBACK_VERTEX fct_remove)
{
    GList *tmp;

    if (!vertex)
	vertex = rootVertex;

    if (fct_begin && fct_begin (vertex))
	goto remove;

    for (tmp=vertex->childs; tmp; ) {
	struct Vertex *v2 = tmp->data;
	tmp=tmp->next;
	ForEachVertexRecursive (v2, fct_begin, fct_end);
    }

    if (fct_end && fct_end (vertex))
	goto remove;

    return;

 remove:
    if (fct_remove)
	fct_remove (vertex);
}
*/

static CALLBACK_VERTEX ForEachVertexConnectedTo_fct = NULL;
static struct Vertex *ForEachVertexConnectedTo_vertex = NULL;
gboolean ForEachVertexConnectedTo_subfct (struct Edge *edge)
{
    if (edge->source == ForEachVertexConnectedTo_vertex)
        ForEachVertexConnectedTo_fct (edge->dest);
    else if (edge->dest == ForEachVertexConnectedTo_vertex)
        ForEachVertexConnectedTo_fct (edge->source);
    return TRUE;
}

void ForEachVertexConnectedTo (struct Vertex *vertex, CALLBACK_VERTEX fct)
{
    ForEachVertexConnectedTo_vertex = vertex;
    ForEachVertexConnectedTo_fct = fct;
    ForEachEdge (ForEachVertexConnectedTo_subfct);
}

static GList *GetEdgesConnectedTo_edgeList;
static struct Vertex *GetEdgesConnectedTo_vertex = NULL;
gboolean GetEdgesConnectedTo_subfct (struct Edge *edge)
{
    if ((edge->source == GetEdgesConnectedTo_vertex) || (edge->dest == GetEdgesConnectedTo_vertex))
        GetEdgesConnectedTo_edgeList = g_list_prepend (GetEdgesConnectedTo_edgeList, edge);

    return TRUE;
}

GList *GetEdgesConnectedTo (struct Vertex * vertex)
{
    GetEdgesConnectedTo_vertex = vertex;
    GetEdgesConnectedTo_edgeList = NULL;
    ForEachEdge (GetEdgesConnectedTo_subfct);
    return GetEdgesConnectedTo_edgeList;
}

double GetVertexCoordX (struct Vertex *vertex)
{
    return vertex->posX;
}

double GetVertexCoordY (struct Vertex *vertex)
{
    return vertex->posY;
}

double GetVertexSizeX (struct Vertex *vertex)
{
    if (vertex)
        return vertex->sizeX;
    else
        return rootVertex->sizeX;
}

double GetVertexSizeY (struct Vertex *vertex)
{
    if (vertex)
        return vertex->sizeY;
    else
        return rootVertex->sizeY;
}

double GetVertexRelSizeX (struct Vertex *vertex)
{
    if (vertex)
        return vertex->rel_sizeX;
    else
        return rootVertex->rel_sizeX;
}

double GetVertexRelSizeY (struct Vertex *vertex)
{
    if (vertex)
        return vertex->rel_sizeY;
    else
        return rootVertex->rel_sizeY;
}

char *GetVertexName (struct Vertex *vertex)
{
    return vertex->name;
}

void SetVertexName (struct Vertex *vertex, char *name)
{
    if (vertex->name)
        free (vertex->name);
    vertex->name = name;
}

void *GetVertexData (struct Vertex *vertex)
{
    return vertex->data;
}

void SetVertexData (struct Vertex *vertex, void *data)
{
    vertex->data = data;
}

struct Vertex *SearchVertex (COMPARISON_FCT fct, void *data)
{
    int i;

    for (i = 0; i < (int) vertices->len; i++)
    {
        struct Vertex *vertex = g_ptr_array_index (vertices, i);

        if (fct (vertex->data, data) == 0)
            return vertex;
    }
    return NULL;
}

void DestroyVertex (struct Vertex *vertex)
{
    GList *tmp;

    g_ptr_array_remove (vertices, vertex);
    for (tmp = vertex->parent->childs; tmp; tmp = tmp->next)
    {
        struct Vertex *v = tmp->data;

        if (v == vertex)
        {
            vertex->parent->childs = g_list_remove_link (vertex->parent->childs, tmp);
            g_free (vertex);
            return;
        }
    }
}

static gboolean RefreshAllConnectedVertices_Clear1 (struct Vertex *vertex)
{
    if (vertex->connectedVertices)
    {
        GList *tmp;

        for (tmp = vertex->connectedVertices; tmp;)
        {
            struct ConnectedVertex *con = tmp->data;

            GList *tmp2 = tmp->next;

            if (con->edge.source != vertex)
                vertex->connectedVertices = g_list_remove_link (vertex->connectedVertices, tmp);
            tmp = tmp2;
        }
    }

    return TRUE;
}
static gboolean RefreshAllConnectedVertices_Clear2 (struct Vertex *vertex)
{
    if (vertex->connectedVertices)
    {
        GList *tmp;

        for (tmp = vertex->connectedVertices; tmp; tmp = tmp->next)
        {
            struct ConnectedVertex *con = tmp->data;

            if (con->edge.source == vertex)
                g_free (con);
        }
        g_list_free (vertex->connectedVertices);
        vertex->connectedVertices = NULL;
    }

    return TRUE;
}
static gboolean RefreshAllConnectedVertices_Add (struct Edge *edge)
{
    struct Vertex *source;
    struct Vertex *dest;
    struct ConnectedVertex *con;
    GList *tmp;
    extern gboolean displayOnlyWireforkTree;

    if (displayOnlyWireforkTree && !(edge->type & EDGE_TYPE_WIREFORK_TREE))
        return TRUE;            // TMP test to show only wirefork tree

    // Adjust the source to be the corresponding visible component
    source = edge->source->deepestVisibleParent;

    // Adjust the dest to be the corresponding visible component
    dest = edge->dest->deepestVisibleParent;

    if (source == dest)
        return TRUE;

    int nb_overlapping_edges = 0;

    // Test if this link already exists, and count how many times
    for (tmp = dest->connectedVertices; tmp; tmp = tmp->next)
    {
        con = tmp->data;
        if ((con->edge.source == source && con->edge.dest == dest) || (con->edge.source == dest && con->edge.dest == source))
        {
            // If yes: increase the weight of groups/count overlapping edges for comp->comp
            if (source->isGroup || dest->isGroup)
            {
                con->weight++;
                con->edges = g_list_prepend (con->edges, edge);
                return TRUE;
            } else
                nb_overlapping_edges++;
        }
    }

    // If no: create a new link
    con = g_new0 (struct ConnectedVertex, 1);

    con->edge = *edge;
    con->edge.source = source;
    con->edge.dest = dest;
    con->weight = 1;
    con->overlapping_edges_num = nb_overlapping_edges;
    con->edges = g_list_prepend (con->edges, edge);

    ASSERT ((unsigned long) con->edge.source > 0x20000);
    ASSERT ((unsigned long) con->edge.dest > 0x20000);

    source->connectedVertices = g_list_prepend (source->connectedVertices, con);
    dest->connectedVertices = g_list_prepend (dest->connectedVertices, con);

    return TRUE;
}

gboolean RefreshAllConnectedVertices_ClearVisibleVerticesCounter (struct Vertex * vertex)
{
    GList *tmp;

    for (tmp = vertex->connectedVertices; tmp; tmp = tmp->next)
    {
        struct ConnectedVertex *con = tmp->data;

        con->visibleVertices = 0;
    }
    return TRUE;
}

gboolean RefreshAllConnectedVertices_IncreaseVisibleVerticesCounter (struct Vertex * vertex)
{
    GList *tmp;

    for (tmp = vertex->connectedVertices; tmp; tmp = tmp->next)
    {
        struct ConnectedVertex *con = tmp->data;

        if (vertex == con->edge.source)
            con->visibleVertices += 1;
        else
            con->visibleVertices += 2;
    }
    return TRUE;
}

gboolean RefreshAllConnectedVertices_FillListOfPorts (struct Vertex * vertex)
{
    GList *tmp;

    for (tmp = vertex->connectedVertices; tmp; tmp = tmp->next)
    {
        struct ConnectedVertex *con = tmp->data;

        if (con->visibleVertices != 3)
        {
            connectedVertices_listOfPorts = g_list_prepend (connectedVertices_listOfPorts, con);
        }
        //printf ("detected %d: %s->%s\n",con->visibleVertices,con->edge.source->name,con->edge.dest->name);
    }
    return TRUE;
}

void RefreshAllConnectedVertices (void)
{
    RecursivelyRefreshDeepestExpandedParent (rootVertex, TRUE, rootVertex);
    ForEachVertex (RefreshAllConnectedVertices_Clear1);
    ForEachVertex (RefreshAllConnectedVertices_Clear2);
    ForEachEdge (RefreshAllConnectedVertices_Add);

    // Update connectedVertices_listOfPorts
    g_list_free (connectedVertices_listOfPorts);
    connectedVertices_listOfPorts = NULL;
    ForEachVisibleVertexRecursive (NULL, RefreshAllConnectedVertices_ClearVisibleVerticesCounter, NULL);
    ForEachVisibleVertexRecursive (NULL, RefreshAllConnectedVertices_IncreaseVisibleVerticesCounter, NULL);
    ForEachVisibleVertexRecursive (NULL, RefreshAllConnectedVertices_FillListOfPorts, NULL);
}

void RecursivelyRefreshDeepestExpandedParent (struct Vertex *vertex, gboolean isVisible, struct Vertex *deepestVisibleParent)
{
    GList *tmp;

    if (isVisible)
        vertex->deepestVisibleParent = vertex;
    else
        vertex->deepestVisibleParent = deepestVisibleParent;

    if (!vertex->isGroup)
        return;

    for (tmp = vertex->childs; tmp; tmp = tmp->next)
    {
        struct Vertex *v = tmp->data;

        if (isVisible)
            RecursivelyRefreshDeepestExpandedParent (v, isVisible
              && (vertex->areChildsVisible || (v->isGroup && vertex->areSubgroupsExpanded)) && !v->isHidden, vertex);
        else
            RecursivelyRefreshDeepestExpandedParent (v, FALSE, deepestVisibleParent);
    }
}

void ExpandGroupVertex (struct Vertex *vertex)
{
    if (!vertex->isGroup)
        return;
    if (!vertex->isShrunk)
        return;

    vertex->isShrunk = FALSE;

    RecursivelyRefreshDeepestExpandedParent (vertex, TRUE /*Bad assumption */ ,
      vertex);
    RefreshAllConnectedVertices ();
}

void ShrinkGroupVertex (struct Vertex *vertex)
{
    if (!vertex->isGroup)
        return;
    if (vertex->isShrunk)
        return;

    vertex->isShrunk = TRUE;

    RecursivelyRefreshDeepestExpandedParent (vertex, TRUE /*Bad assumption */ ,
      vertex);
    RefreshAllConnectedVertices ();
}

// Accessing Edges
void ForEachEdge (CALLBACK_EDGE fct)
{
    if (edges)
    {
        int i;

        for (i = 0; i < (int) edges->len; i++)
        {
            struct Edge *edge = g_ptr_array_index (edges, i);

            fct (edge);
        }
    }
}

static CALLBACK_EDGECON ForEachVisibleEdge_fct = NULL;
static gboolean ForEachVisibleEdge_subfct (struct Vertex *vertex)
{
    GList *tmp;

    for (tmp = vertex->connectedVertices; tmp; tmp = tmp->next)
    {
        struct ConnectedVertex *con = tmp->data;

        if (con->edge.source == vertex->deepestVisibleParent)
        {                       // Without this, it would report the same edge twice: once for the source vertex and once for the destination vertex.
            ASSERT ((unsigned long) con->edge.source > 0x20000);
            ASSERT ((unsigned long) con->edge.dest > 0x20000);
            ForEachVisibleEdge_fct (con);
        }
    }
    return TRUE;
}

void ForEachVisibleEdge (CALLBACK_EDGECON fct)
{
    /*
       int i;
       for (i=0; i<edges->len; i++) {
       struct Edge *edge = g_ptr_array_index (edges, i);
       struct Vertex *tmp;
       // Check that the vertices are displayed
       for (tmp=edge->source; tmp; tmp=tmp->parent)
       if (tmp->isShrunk)
       goto next;

       for (tmp=edge->dest; tmp; tmp=tmp->parent)
       if (tmp->isShrunk)
       goto next;

       fct (edge);
       next:;
       }
     */
    ForEachVisibleEdge_fct = fct;
    ForEachVisibleVertex (ForEachVisibleEdge_subfct);
}

void ForEachVisibleEdgeAndPorts (CALLBACK_EDGECON fct)
{
    // Process ports
    GList *tmp;

    for (tmp = connectedVertices_listOfPorts; tmp; tmp = tmp->next)
    {
        struct ConnectedVertex *con = tmp->data;

        if (con->visibleVertices == 2)
            fct (con);
    }

    // Call the normal function
    ForEachVisibleEdge (fct);
}

/*
static CALLBACK_EDGE ForEachVisibleLevelEdge_fct = NULL;
gboolean ForEachVisibleLevelEdge_subfct (struct Vertex *vertex)
{
    if (!vertex->isShrunk) {
	GList *tmp;
	for (tmp=vertex->levelEdges; tmp; tmp=tmp->next) {
	    struct Edge *edge = tmp->data;
	    ForEachVisibleLevelEdge_fct (edge);
	}
    }
    return TRUE;
}
void ForEachVisibleLevelEdge (CALLBACK_EDGE fct)
{
    ForEachVisibleLevelEdge_fct = fct;
    ForEachVisibleVertexRecursive (NULL, ForEachVisibleLevelEdge_subfct, NULL);
}
*/

void DestroyEdge (struct Edge *edge)
{
    if (!g_ptr_array_remove (edges, edge))
        printf ("Error removing edge\n");
    g_free (edge);
}

void DestroyEdgeWithoutFree (struct Edge *edge)
{
    if (!g_ptr_array_remove (edges, edge))
        printf ("Error removing edge\n");
}

static struct Vertex *SearchAndDestroyEdge_v1 = NULL;
static struct Vertex *SearchAndDestroyEdge_v2 = NULL;
static gboolean SearchAndDestroyEdge_done = FALSE;
enum EdgeDirection SearchAndDestroyEdge_DestroyedEdgeDirection;
gboolean SearchAndDestroyEdge_subfct (struct Edge *edge)
{
    if ((edge->source == SearchAndDestroyEdge_v1
        && edge->dest == SearchAndDestroyEdge_v2) || (edge->source == SearchAndDestroyEdge_v2 && edge->dest == SearchAndDestroyEdge_v1))
    {
        if (!g_ptr_array_remove (edges, edge))
            printf ("Error removing edge\n");

        SearchAndDestroyEdge_DestroyedEdgeDirection = edge->direction;
        g_free (edge);
        SearchAndDestroyEdge_done = TRUE;
    }
    return TRUE;
}

gboolean SearchAndDestroyEdge (struct Vertex * v1, struct Vertex * v2)
{
    SearchAndDestroyEdge_v1 = v1;
    SearchAndDestroyEdge_v2 = v2;
    SearchAndDestroyEdge_done = FALSE;
    ForEachEdge (SearchAndDestroyEdge_subfct);
    return SearchAndDestroyEdge_done;
}

struct Vertex *GetSourceVertex (struct Edge *edge)
{
    return edge->source;
}

struct Vertex *GetDestVertex (struct Edge *edge)
{
    return edge->dest;
}

gboolean RemoveIfEmptyGroup (struct Vertex * v)
{
    if (v->isGroup && !v->childs)
    {
        v->parent->childs = g_list_remove (v->parent->childs, v);

        if (!g_ptr_array_remove (vertices, v))
            printf ("Error removing vertex\n");
        // TODO: Destroy vertex
    }
    return TRUE;
}

void PruneEmptyGroups (struct Vertex *v)
{
    ForEachVertexRecursive (v, NULL, RemoveIfEmptyGroup);
}

gboolean RemoveIfGroupWithOneGroupElement (struct Vertex *v)
{
    if (v->isGroup && v->childs && !v->childs->next)
    {
        struct Vertex *onlyChild = v->childs->data;

        if (onlyChild->isGroup)
        {
            v->childs = onlyChild->childs;
            v->name = g_strdup_printf ("%s - %s", v->name, onlyChild->name);

            if (!g_ptr_array_remove (vertices, onlyChild))
                printf ("Error removing vertex (2)\n");

            {
                GList *tmp;

                for (tmp = v->childs; tmp; tmp = tmp->next)
                {
                    struct Vertex *child = tmp->data;

                    child->parent = v;
                }
            }
            // TODO: Destroy child vertex
        }
    }
    return TRUE;
}

void PruneGroupsWithOneGroupElement (struct Vertex *v)
{
    ForEachVertexRecursive (v, NULL, RemoveIfGroupWithOneGroupElement);
}

gboolean RemoveIfColoredVertex (struct Vertex *v)
{
    if (v->color == 1)
    {
        if (!v->parent)
            return TRUE;        // Added because of pb when a vertex has not been put in a group (but keep the vertex in order to debug the pb anyway.

        v->parent->childs = g_list_remove (v->parent->childs, v);

        if (!g_ptr_array_remove (vertices, v))
            printf ("Error removing vertex (3)\n");
        // TODO: Destroy vertex
    }
    return TRUE;
}

void PruneColoredVertices (struct Vertex *v)
{
    ForEachVertexRecursive (v, NULL, RemoveIfColoredVertex);
}

/*
gboolean InitialiseLevelEdges_subfct (struct Edge *edge)
{
    struct Vertex *tmp;
    // Find common parent of edge->source and edge->dest
    struct Vertex *v1 = edge->source;
    struct Vertex *v2 = edge->dest;
    struct Vertex *lastv1, *lastv2;
    int depth1 = 0;
    int depth2 = 0;
    for (tmp=v1; tmp; tmp=tmp->parent) depth1++;
    for (tmp=v2; tmp; tmp=tmp->parent) depth2++;

    while (depth1>depth2) {
	v1 = v1->parent;
	depth1--;
    }
    while (depth2>depth1) {
	v2 = v2->parent;
	depth2--;
    }

    while (v1 != v2) {
	v1 = v1->parent;
	v2 = v2->parent;
    }

    if (!v1)
	v1 = rootVertex;

    lastv1=edge->source;
    while (lastv1 && lastv1->parent != v1) lastv1=lastv1->parent;
    lastv2=edge->dest;
    while (lastv2 && lastv2->parent != v1) lastv2=lastv2->parent;
    v1->levelEdges = g_list_prepend (v1->levelEdges, NewEdge (lastv1, lastv2));

    return TRUE;
}
void InitialiseLevelEdges (void)
{
    ForEachEdge (InitialiseLevelEdges_subfct);
}
*/

void UpdateVertexPositions (void)
{

}

/* New algorithm for force-directed layout:
 1/ Find contour vertices
 2/ Take and pull them as if you wanted to extend a cloth by pulling its edges :)
 3/ Some local sub-graphs can be too dense to be pulled enough => reiterate at their level.

1/ How to find the contour vertices?

*/

static FILE *SaveGraph_file = NULL;
static int SaveGraph_SetSequentialVertexNum_value = 0;
gboolean SaveGraph_SetSequentialVertexNum (struct Vertex *vertex)
{
    vertex->data2 = GINT_TO_POINTER (SaveGraph_SetSequentialVertexNum_value);
    SaveGraph_SetSequentialVertexNum_value++;
    return TRUE;
}

gboolean SaveGraphVertex (struct Vertex * vertex)
{
    GList *tmp;

    fprintf (SaveGraph_file, "name=%s\n" "type=%d\n" "data2=%d\n" "parent=%d\n" "isGroup=%d\n" "isShrunk=%d\n"
      "areSubgroupsExpanded=%d\n" "areChildsVisible=%d\n" "isHidden=%d\n"
      //      "childs=%x\n"
      "posX=%.16f\n" "posY=%.16f\n" "posZ=%.16f\n" "sizeX=%.16f\n" "sizeY=%.16f\n"
      "rel_posX=%.16f\n" "rel_posY=%.16f\n" "rel_sizeX=%.16f\n" "rel_sizeY=%.16f\n" "color=%d\n"
      //      "hierarchyWindowNode=%x\n"
      , vertex->name, vertex->type, GPOINTER_TO_INT (vertex->data2),
      (vertex->parent == NULL) ? -1 : GPOINTER_TO_INT (vertex->parent->data2), vertex->isGroup, vertex->isShrunk, vertex->areSubgroupsExpanded,
      vertex->areChildsVisible, vertex->isHidden
      //      , vertex->childs
      , vertex->posX, vertex->posY, vertex->posZ, vertex->sizeX, vertex->sizeY,
      vertex->rel_posX, vertex->rel_posY, vertex->rel_sizeX, vertex->rel_sizeY, vertex->color
      //      , vertex->hierarchyWindowNode
      );

    ASSERT (((vertex->parent == NULL) ? -1 : GPOINTER_TO_INT (vertex->parent->data2)) < 100000);

    fprintf (SaveGraph_file, "balsaPosition=");
    if (vertex->balsaPosition)
        dumpPosition (SaveGraph_file, vertex->balsaPosition);
    fprintf (SaveGraph_file, "\n");

    fprintf (SaveGraph_file, "childs=");
    //    if (vertex->childs) {
    for (tmp = vertex->childs; tmp; tmp = tmp->next)
    {
        struct Vertex *v = tmp->data;

        fprintf (SaveGraph_file, " %d", GPOINTER_TO_INT (v->data2));
        //    }
    }
    fprintf (SaveGraph_file, "\n");

    fprintf (SaveGraph_file, "\n");

    return TRUE;
}

gboolean SaveGraphEdge (struct Edge * edge)
{
    fprintf (SaveGraph_file,
      "source=%d\n"
      "dest=%d\n"
      "direction=%d\n"
      "\n", (edge->source == NULL) ? -1 : GPOINTER_TO_INT (edge->source->data2), (edge->dest == NULL) ? -1 : GPOINTER_TO_INT (edge->dest->data2),
      edge->direction);

    ASSERT (((edge->source == NULL) ? -1 : GPOINTER_TO_INT (edge->source->data2)) < 100000);
    ASSERT (((edge->dest == NULL) ? -1 : GPOINTER_TO_INT (edge->dest->data2)) < 100000);

    return TRUE;
}

void SaveGraph (const char *filename)
{
    if (!vertices)
        fprintf (stderr, "No graph to save\n");

    SaveGraph_file = fopen (filename, "w");
    if (!SaveGraph_file)
        return;

    /*
       SaveGraph_SetSequentialVertexNum_value = 0;
       ForEachVertex (SaveGraph_SetSequentialVertexNum);
     */
    int i;

    for (i = 0; i < vertices->len; i++)
    {
        struct Vertex *vertex = g_ptr_array_index (vertices, i);

        vertex->data2 = GINT_TO_POINTER (i);
    }

    fprintf (SaveGraph_file, "visibleRootVertex=%d\n", GPOINTER_TO_INT (visibleRootVertex->data2));

    ForEachVertex (SaveGraphVertex);

//    fprintf (SaveGraph_file, "***Edges***\n");
//    ForEachEdge (SaveGraphEdge);

    fclose (SaveGraph_file);
}

#if 0
void LoadGraph (const char *filename)
{
    struct Vertex *vertex;
    char name[10000];
    FILE *f = fopen (filename, "r");

    if (!f)
        return;

    while (!feof (f))
    {
        if (fgets (name, 10000, f) == NULL)
            break;
        if (!BEGINS_WITH (name, "name="))
            break;
        name[strlen (name) - 1] = 0;
        vertex = AddNewVertex_NoParent (name + 5);
        ASSERT (fscanf (f, "type=%d\n" "data2=%ld\n" "parent=%ld\n" "isGroup=%d\n" "isShrunk=%d\n"
            "areSubgroupsExpanded=%d\n" "areChildsVisible=%d\n" "isHidden=%d\n"
            //      "childs=%x\n"
            "posX=%lf\n" "posY=%lf\n" "posZ=%lf\n" "sizeX=%lf\n" "sizeY=%lf\n" "rel_posX=%lf\n" "rel_posY=%lf\n" "rel_sizeX=%lf\n" "rel_sizeY=%lf\n"
            "color=%d\n"
            //      "hierarchyWindowNode=%x\n"
            , &vertex->type, (long *) &vertex->data2, (long *) &vertex->parent, &vertex->isGroup, &vertex->isShrunk, &vertex->areSubgroupsExpanded,
            &vertex->areChildsVisible, &vertex->isHidden
            //      , vertex->childs
            , &vertex->posX, &vertex->posY, &vertex->posZ, &vertex->sizeX, &vertex->sizeY,
            &vertex->rel_posX, &vertex->rel_posY, &vertex->rel_sizeX, &vertex->rel_sizeY, &vertex->color
            //      , vertex->hierarchyWindowNode
          ) == 18);

        ASSERT (fgets (name, 10000, f) != NULL);
        ASSERT (BEGINS_WITH (name, "balsaPosition="));
        if (BEGINS_WITH (name + 14, "(at "))
        {
            vertex->balsaPosition = g_new0 (struct Position, 1);
            char *ptr = name + 18;

            vertex->balsaPosition->line = atoi (ptr);
            ptr = strchr (ptr, ' ') + 1;
            vertex->balsaPosition->column = atoi (ptr);
            ptr = strrchr (ptr, ' ') + 1;
            vertex->balsaPosition->context = atoi (ptr);
        }

        ASSERT (fgets (name, 10000, f) != NULL) ASSERT (BEGINS_WITH (name, "childs=")) vertex->childs = (GList *) g_strdup (name + 7);

        fgets (name, 10000, f);
    }

    {
        int i;

        for (i = 0; i < (int) vertices->len; i++)
        {
            struct Vertex *vertex = g_ptr_array_index (vertices, i);
            char *str, *ptr;

            if (((long) vertex->parent) == -1)
                vertex->parent = NULL;
            else
            {
                ASSERT ((long) vertex->parent < (long) vertices->len);
                vertex->parent = g_ptr_array_index (vertices, (long) vertex->parent);
            }

            str = (char *) vertex->childs;
            ptr = str;
            vertex->childs = NULL;
            if (*ptr != '\n')
            {
                while (ptr)
                {
                    int num = atoi (ptr + 1);

                    ASSERT (num < (int) vertices->len);
                    vertex->childs = g_list_prepend (vertex->childs, g_ptr_array_index (vertices, num));
                    ptr = strchr (ptr + 1, ' ');
                }
                vertex->childs = g_list_reverse (vertex->childs);
            }
            free (str);
        }
    }

    if (!strcmp (name, "***Edges***\n"))
    {
        while (!feof (f))
        {
            int sourcenum, destnum, dirnum;
            struct Vertex *source, *dest;

            if (fscanf (f, "source=%d\n" "dest=%d\n" "direction=%d\n" "\n", &sourcenum, &destnum, &dirnum) == 3)
            {
                source = g_ptr_array_index (vertices, sourcenum);
                dest = g_ptr_array_index (vertices, destnum);
                struct Edge *newEdge = AddNewEdge (source, dest);

                newEdge->direction = dirnum;
            }
        }
    }

    fclose (f);
}
#endif

void LoadGraphOntoExistingGraph (const char *filename)
{
    struct Vertex *vertex = g_new (struct Vertex, 1);
    char name[10000];
    FILE *f = fopen (filename, "r");

    if (!f)
        return;

    // read "rootVertex=%d"
    if (fgets (name, 10000, f))
    {
        ASSERT (BEGINS_WITH (name, "visibleRootVertex="));
        int num = atoi (name + 18);

        if (num == -1)
            visibleRootVertex = rootVertex;
        else
        {
            ASSERT (num < vertices->len);
            visibleRootVertex = g_ptr_array_index (vertices, (long) num);
        }
    }
    // Read the rest of the file
    while (!feof (f))
    {
        if (fgets (name, 10000, f) == NULL)
            break;
        if (!BEGINS_WITH (name, "name="))
            break;
        name[strlen (name) - 1] = 0;
        ASSERT (fscanf (f,
            "type=%d\n" "data2=%d\n" "parent=%d\n" "isGroup=%d\n"
            "isShrunk=%d\n"
            "areSubgroupsExpanded=%d\n"
            "areChildsVisible=%d\n"
            "isHidden=%d\n"
            "posX=%lf\n" "posY=%lf\n" "posZ=%lf\n" "sizeX=%lf\n"
            "sizeY=%lf\n" "rel_posX=%lf\n" "rel_posY=%lf\n"
            "rel_sizeX=%lf\n" "rel_sizeY=%lf\n" "color=%d\n", &vertex->type,
            (int *) &vertex->data2, (int *) &vertex->parent,
            &vertex->isGroup, &vertex->isShrunk,
            &vertex->areSubgroupsExpanded, &vertex->areChildsVisible, &vertex->isHidden,
            &vertex->posX,
            &vertex->posY, &vertex->posZ, &vertex->sizeX, &vertex->sizeY,
            &vertex->rel_posX, &vertex->rel_posY, &vertex->rel_sizeX, &vertex->rel_sizeY, &vertex->color) == 18);

        struct Vertex *v = g_ptr_array_index (vertices, GPOINTER_TO_INT (vertex->data2));

        if (v->posY != vertex->posY)
        {
            v->rel_posX = vertex->posX;
            v->rel_posY = vertex->posY;
            v->posX = vertex->posX;
            v->posY = vertex->posY;
            v->posZ = vertex->posZ;
        }
        if (v->sizeY != vertex->sizeY)
        {
            v->rel_sizeX = vertex->sizeX;
            v->rel_sizeY = vertex->sizeY;
            v->sizeX = vertex->sizeX;
            v->sizeY = vertex->sizeY;
        }
        v->isShrunk = vertex->isShrunk;
        v->areSubgroupsExpanded = vertex->areSubgroupsExpanded;
        v->areChildsVisible = vertex->areChildsVisible;
        v->isHidden = vertex->isHidden;
        v->color = vertex->color;

        ASSERT (fgets (name, 10000, f) != NULL);
        ASSERT (BEGINS_WITH (name, "balsaPosition="));

        ASSERT (fgets (name, 10000, f) != NULL);
        ASSERT (BEGINS_WITH (name, "childs="));

        fgets (name, 10000, f);
    }

    RefreshAllConnectedVertices ();

    fclose (f);
}

/*
void LayoutWithDot (struct Vertex *v)
{
    FILE *f = fopen ("tmp.dot", "w");

    SaveGraph_SetSequentialVertexNum_value = 0;
    ForEachVertex (SaveGraph_SetSequentialVertexNum);

    fprintf (f,
      "digraph anything {\n"
		//      "size=\"7,10\"\n"
		"dpi=72\n"
      "edge [ decorate=1 fontsize=8 fontname=Helvetica labelfontname=Helvetica labelfontsize=8 ]\n"
      "node [ fontsize=12 fontname=\"Helvetica-Bold\" shape=ellipse ]\n"
      "subgraph cluster_components {\n" "  label=\"%s\"\n", v->name);

    GList *tmp;
    int i;

    for (tmp = v->childs, i = 0; tmp; tmp = tmp->next, i++)
    {
        struct Vertex *v2 = tmp->data;
        char *label = g_strdup (v2->name);
        char *ptr = strchr (label, '"');

        if (ptr)
            *ptr = 0;

        fprintf (f, " comp%d [label=\"%s\"]\n",  GPOINTER_TO_INT (v2->data2), label);

        g_free (label);
    }

    fprintf (f, "}\n");

    for (tmp = v->childs; tmp; tmp = tmp->next)
    {
        struct Vertex *v2 = tmp->data;
        GList *tmp2;

        for (tmp2 = v2->connectedVertices; tmp2; tmp2 = tmp2->next)
        {
            struct ConnectedVertex *con = tmp2->data;

            if (con->edge.source == v2)
            {
                switch (con->edge.direction)
                {
                case EdgeDirection_Undefined:
                case EdgeDirection_Sync:
                    fprintf (f,
                      "comp%d -> comp%d [ label=\"\" arrowhead=odot arrowtail=dot dir=forward headlabel=\"0\" taillabel=\"\" ]\n",
                       GPOINTER_TO_INT (con->edge.dest->data2),  GPOINTER_TO_INT (con->edge.source->data2));
                    break;
                case EdgeDirection_Push:
                    fprintf (f,
                      "comp%d -> comp%d [ label=\"\" arrowhead=normal arrowtail=dot dir=forward headlabel=\"0\" taillabel=\"\" ]\n",
                       GPOINTER_TO_INT (con->edge.dest->data2),  GPOINTER_TO_INT (con->edge.source->data2));
                    break;
                case EdgeDirection_Pull:
                    fprintf (f,
                      "comp%d -> comp%d [ label=\"\" arrowhead=odot arrowtail=normal dir=forward headlabel=\"0\" taillabel=\"\" ]\n",
                       GPOINTER_TO_INT (con->edge.dest->data2),  GPOINTER_TO_INT (con->edge.source->data2));
                    break;
                }

            }
        }
    }

    fprintf (f, "}\n");

    fclose (f);
}
*/

char *strdup_ApplyMaxWidth (const char *str, int length, int returnType)
{
    int nbChars = strlen (str);
    int nbLines = nbChars / length;

    if (nbLines == 0)
        return g_strdup (str);

    int remainder = nbChars - (nbLines * length);
    char *result = g_new (char, nbLines * (length + returnType) + remainder + 1);
    int i;

    for (i = 0; i < nbLines; i++)
    {
        memcpy (&result[i * (length + returnType)], &str[i * length], length);
        switch (returnType)
        {
        case 1:
            result[(i + 1) * (length + returnType) - 1] = '\n';
            break;
        case 2:
            result[(i + 1) * (length + returnType) - 2] = '\\';
            result[(i + 1) * (length + returnType) - 1] = 'n';
            break;
        }
    }

    memcpy (&result[nbLines * (length + returnType)], &str[nbLines * length], remainder);
    result[nbLines * (length + returnType) + remainder] = 0;

    return result;
}

gboolean UpdateVertexLabel (struct Vertex * v)
{
    if (v->isGroup)
    {
        if (v->label)
            g_free (v->label);

        char *betterName = NULL;

        if (!v->areSubgroupsExpanded)
        {
            // Count the number of not-hidden subgroups and change the name to "name (nbSubgroups)"
            int nbSubgroups = 0;
            int nbMODSubgroups = 0;
            GList *tmp;

            for (tmp = v->childs; tmp; tmp = tmp->next)
            {
                struct Vertex *v2 = tmp->data;

                if (v2->isGroup && !v2->isHidden)
                {
                    nbSubgroups++;
                    if (BEGINS_WITH (v2->name, "MOD_"))
                        nbMODSubgroups++;
                }
            }
            if (nbSubgroups >= 2 || (nbSubgroups - nbMODSubgroups) >= 1)
                betterName = g_strdup_printf ("%s (%d)", v->name, nbSubgroups);
        }

        if (v->areChildsVisible || v->areSubgroupsExpanded)
            v->label = strdup_ApplyMaxWidth (betterName ? : v->name, LINE_WIDTH_FOR_NON_EMPTY_GROUP, 1);
        else
            v->label = strdup_ApplyMaxWidth (betterName ? : v->name, LINE_WIDTH_FOR_EMPTY_GROUP, 1);

        if (betterName)
            g_free (betterName);
    }

    return TRUE;
}

void UpdateAllVertexLabels (void)
{
    ForEachVisibleVertexRecursiveAndPorts (NULL, UpdateVertexLabel, NULL);
}

void PrintEscapedString (FILE * f, char *str)
{
    for (; *str; str++)
    {
        switch (*str)
        {
        case '\n':
            fprintf (f, "\\n");
            break;
        case '"':
            fprintf (f, "\\\"");
            break;
        default:
            fputc (*str, f);
        }
    }
}

FILE *dotFile;
gboolean FullLayoutWithDot_PrintVertex1 (struct Vertex *vertex)
{
    char *label = vertex->label ? : vertex->name;

    if (vertex->isGroup)
    {
        /*
           char *label2 = strdup_ApplyMaxWidth (label,
           (vertex->areChildsVisible || vertex->areSubgroupsExpanded) ? LINE_WIDTH_FOR_NON_EMPTY_GROUP : LINE_WIDTH_FOR_EMPTY_GROUP, 2);
         */

        fprintf (dotFile, "subgraph \"cluster_%ld\" {\n" "  label=\"", (long) vertex);
        PrintEscapedString (dotFile, label);
        fprintf (dotFile, "\"\n");
        if (!vertex->areChildsVisible)
            fprintf (dotFile, "  comp%ld [label=\"\", id=fake]\n", (long) vertex);
        else
        {
            GList *tmp;

            for (tmp = vertex->childs; tmp; tmp = tmp->next)
            {
                struct Vertex *v = tmp->data;

                if (!v->isGroup && v->isHidden)
                {
                    fprintf (dotFile, "  comp%ld [label=\"\", id=fake]\n", (long) vertex);
                    break;
                }
            }
        }
    } else
    {
        fprintf (dotFile, " comp%ld [label=\"", (long) vertex /*->data*/ );
        PrintEscapedString (dotFile, label);
        fprintf (dotFile, "\"]\n");
    }

    return TRUE;
}

gboolean FullLayoutWithDot_PrintVertex2 (struct Vertex * vertex)
{
    if (vertex->isGroup)
        fprintf (dotFile, "}\n");
    return TRUE;
}

gboolean FullLayoutWithDot_PrintEdge (struct Edge * edge)
{
    switch (edge->direction)
    {
    case EdgeDirection_Undefined:
    case EdgeDirection_Sync:
        fprintf (dotFile, "comp%ld -> comp%ld [ id=%ld arrowhead=odot arrowtail=dot dir=forward headlabel=\"0\" taillabel=\"\" ]\n", (long) edge->source //->data
          , (long) edge->dest   //->data
          , (long) edge);
        break;
    case EdgeDirection_Push:
        fprintf (dotFile, "comp%ld -> comp%ld [ id=%ld arrowhead=normal arrowtail=dot dir=forward headlabel=\"0\" taillabel=\"\" ]\n", (long) edge->source //->data
          , (long) edge->dest   //->data
          , (long) edge);
        break;
    case EdgeDirection_Pull:
        fprintf (dotFile, "comp%ld -> comp%ld [ id=%ld arrowhead=odot arrowtail=normal dir=forward headlabel=\"0\" taillabel=\"\" ]\n", (long) edge->source //->data
          , (long) edge->dest   //->data
          , (long) edge);
        break;
    }

    return TRUE;
}

gboolean FullLayoutWithDot_PrintEdgeCon (struct ConnectedVertex * con)
{
    struct Edge *edge = &con->edge;

    switch (edge->direction)
    {
    case EdgeDirection_Undefined:
    case EdgeDirection_Sync:
        fprintf (dotFile, "comp%ld -> comp%ld [ id=%ld arrowhead=odot arrowtail=dot dir=forward headlabel=\"0\" taillabel=\"\" ]\n", (long) edge->source //->data
          , (long) edge->dest   //->data
          , (long) con);
        break;
    case EdgeDirection_Push:
        fprintf (dotFile, "comp%ld -> comp%ld [ id=%ld arrowhead=normal arrowtail=dot dir=forward headlabel=\"0\" taillabel=\"\" ]\n", (long) edge->source //->data
          , (long) edge->dest   //->data
          , (long) con);
        break;
    case EdgeDirection_Pull:
        fprintf (dotFile, "comp%ld -> comp%ld [ id=%ld arrowhead=odot arrowtail=normal dir=forward headlabel=\"0\" taillabel=\"\" ]\n", (long) edge->source //->data
          , (long) edge->dest   //->data
          , (long) con);
        break;
    }

    return TRUE;
}

void ExportDotFile (const char *filename)
{
    // Export tmp.dot
    dotFile = fopen (filename, "w");

    SaveGraph_SetSequentialVertexNum_value = 0;
    ForEachVertex (SaveGraph_SetSequentialVertexNum);

    fprintf (dotFile, "digraph anything {\n"
      //    "size=\"7,10\"\n"
      "dpi=72\n"
      "edge [ decorate=1 fontsize=8 fontname=Helvetica labelfontname=Helvetica labelfontsize=8 ]\n"
      "node [ fontsize=12 fontname=\"Helvetica-Bold\" shape=ellipse ]\n");

    ForEachVisibleVertexRecursiveAndPorts (NULL, FullLayoutWithDot_PrintVertex1, FullLayoutWithDot_PrintVertex2);

    //    ForEachEdge (FullLayoutWithDot_PrintEdge);
    ForEachVisibleEdgeAndPorts (FullLayoutWithDot_PrintEdgeCon);

    fprintf (dotFile, "}\n");

    fclose (dotFile);
}

double MoveGroupContent_X = 0;
double MoveGroupContent_Y = 0;
gboolean MoveGroupContent (struct Vertex *v)
{
    v->posX += MoveGroupContent_X;
    v->posY += MoveGroupContent_Y;

    v->rel_posX += MoveGroupContent_X;
    v->rel_posY += MoveGroupContent_Y;
    return TRUE;
}

int ParseDotFile (const char *filename, gboolean adjustGroupPositions, struct Vertex *mainGroup)
{
    FILE *f = fopen (filename, "r");

    if (!f)
        return 1;

    // Read dot positions
    struct Vertex *subgraphVertex = rootVertex;

    while (!feof (f))
    {
        char buf[10000];

        if (!fgets (buf, 10000, f))
            break;
        char *ptr = buf;

        while (*ptr == ' ' || *ptr == '\t')
            ptr++;
        if (BEGINS_WITH (ptr, "subgraph cluster_"))
        {
            ptr = strchr (ptr, '_') + 1;
            gpointer addr;

            sscanf (ptr, "%ld", (long *) &addr);
            subgraphVertex = (struct Vertex *) addr;
        } else if (BEGINS_WITH (ptr, "graph [bb=") || BEGINS_WITH (ptr, "bb="))
        {
            ptr = strchr (ptr, '"');
            if (!ptr)
                continue;
            ptr++;
            int xmin, ymin, xmax, ymax;

            if (sscanf (ptr, "%d,%d,%d,%d", &xmin, &ymin, &xmax, &ymax) == 4)
            {
                double old_posX = subgraphVertex->rel_posX;
                double old_posY = subgraphVertex->rel_posY;
                double old_sizeX = subgraphVertex->rel_sizeX;
                double old_sizeY = subgraphVertex->rel_sizeY;

                if (adjustGroupPositions && subgraphVertex != rootVertex && old_sizeX != 0 && old_sizeY != 0)
                {
                    //         ASSERT ((old_sizeX == subgraphVertex->rel_sizeX) && (old_sizeY == subgraphVertex->rel_sizeY));
                    double diffX = old_posX - ((double) xmin + (double) xmax) / 2;
                    double diffY = old_posY - ((double) -ymax + (double) -ymin) / 2;

                    if (diffX || diffY)
                    {
                        MoveGroupContent_X = -diffX;
                        MoveGroupContent_Y = -diffY;
                        ForEachVertexRecursive (subgraphVertex, MoveGroupContent, NULL);
                    }
                } else
                {
                    subgraphVertex->rel_posX = ((double) xmin + (double) xmax) / 2;
                    subgraphVertex->rel_posY = ((double) -ymax + (double) -ymin) / 2;
                    subgraphVertex->rel_sizeX = ((double) xmax - xmin) / 2;
                    subgraphVertex->rel_sizeY = ((double) ymax - ymin) / 2;
                }

                continue;
            }
            //     printf ("Found graph %d,%d,%d,%d\n", xmin, ymin, xmax, ymax);
        } else if (BEGINS_WITH (ptr, "comp"))
        {
            if (strstr (ptr, " -> comp"))
            {
                /*
                   int source, dest;
                   float x1, y1, x2, y2;

                   sscanf (ptr, "comp%d -> comp%d", &source, &dest);
                   char *ptr2 = strstr (ptr, "id=");
                   struct ConnectedVertex *con = (struct ConnectedVertex *) atoi (ptr2 + 3);
                   struct Edge *edge = &con->edge;

                   ptr = strstr (ptr, "pos=\"");
                   ptr += 5;
                   sscanf (ptr, "s,%f,%f e,%f,%f", &x1, &y1, &x2, &y2);
                 */
            } else
            {
                gpointer compptr;
                char label[1000];
                char *ptrlabel;
                int posX, posY;
                float width = 0, height = 0;

                if (strstr (ptr, "id=fake"))
                    continue;

                label[0] = 0;
                if (strstr (ptr, "label"))
                {
                    sscanf (ptr, "comp%ld", (long *) &compptr);
                    char *ptr2 = strstr (ptr, "label=\"");

                    if (ptr2)
                    {
                        ptr2 += 7;
                        char *ptr3 = strchr (ptr2, '"');

                        strncpy (label, ptr2, ptr3 - ptr2);
                        label[ptr3 - ptr2] = 0;
                        ptr2 = ptr3 + 1;
                        if (sscanf (ptr2, ", pos=\"%d,%d\", width=\"%f\", height=\"%f", &posX, &posY, &width, &height) != 4)
                            if (sscanf (ptr2, ", width=\"%f\", height=\"%f\", pos=\"%d,%d", &width, &height, &posX, &posY) != 4)
                                printf ("Warning: don't manage to parse (0) \"%s\"\n", ptr2);
                    } else
                    {
                        ptr2 = strchr (ptr, ' ');
                        if (sscanf (ptr2, " [label=%s pos=\"%d,%d\", width=\"%f\", height=\"%f", label, &posX, &posY, &width, &height) != 5)
                            if (sscanf (ptr2, " [label=%s width=\"%f\", height=\"%f\", pos=\"%d,%d", label, &width, &height, &posX, &posY) != 5)
                                printf ("Warning: don't manage to parse (1) \"%s\"\n", ptr2);
                        label[strlen (label) - 1] = 0;
                    }
                } else
                {
                    if (sscanf (ptr, "comp%ld [pos=\"%d,%d\", width=\"%f\", height=\"%f", (long *) &compptr, &posX, &posY, &width, &height) != 5)
                        if (sscanf (ptr, "comp%ld [width=\"%f\", height=\"%f\", pos=\"%d,%d", (long *) &compptr, &width, &height, &posX, &posY) != 5)
                            printf ("Warning: don't manage to parse (2) \"%s\"\n", ptr);
                    posX = posY = width = height = -1;
                    continue;
                }
                if (*label == '"')
                {
                    label[strlen (label) - 1] = 0;
                    ptrlabel = label + 1;
                } else
                    ptrlabel = label;
                //  printf ("Found component comp%ld label=\"%s\" (%d,%d) (%f,%f)\n", compptr, ptrlabel, posX, posY, 36*width, 36*height);

                struct Vertex *v = (struct Vertex *) compptr;

                v->rel_posX = (double) posX;
                v->rel_posY = (double) -posY;
                v->rel_sizeX = ((double) width) / 2 * DPI;
                v->rel_sizeY = ((double) height) / 2 * DPI;

                if (!width || !height)
                    printf ("Warning: width or height=0 in ParseDotFile\n");
            }
        }
    }

    fclose (f);
    return 0;
}

void FullLayoutWithDot_BottomUp (void);

void FullLayoutWithDot (void)
{

    if (layoutFilename)
    {
        LoadGraphOntoExistingGraph (layoutFilename);
        layoutFilename = 0;
    } else
        FullLayoutWithDot_BottomUp ();

    return;

    ExportDotFile ("tmp2.dot1");

    // Run dot
    remove ("tmp2.dot2");
    system ("dot -o tmp2.dot2 tmp2.dot1");

    libspring_LayoutProblem = ParseDotFile ("tmp2.dot2", FALSE, NULL);

    remove ("tmp2.dot1");
    remove ("tmp2.dot2");

    //    RedrawScreen ();
}

/****************************************/
GList *dotFiles = NULL;
GList *dotFilesVertices = NULL;
GList *dotFilesEdgeConLists = NULL;

gboolean FullLayoutWithDot_BottomUp_PrintEdgeCon_ForGroup (struct ConnectedVertex * con, struct Vertex * group)
{
    struct Edge *edge = &con->edge;

    struct Vertex *source = edge->source;
    struct Vertex *dest = edge->dest;

    if (source->parent != group)
    {
        struct Vertex *parent = source->parent;

        if (group == rootVertex)
            dest = visibleRootVertex;
        else
            while (parent)
            {
                if (parent->parent == group)
                {
                    source = parent;
                    goto ok1;
                }
                parent = parent->parent;
            }
    }
  ok1:

    if (dest->parent != group)
    {
        struct Vertex *parent = dest->parent;

        if (group == rootVertex)
            dest = visibleRootVertex;
        else
            while (parent)
            {
                if (parent->parent == group)
                {
                    dest = parent;
                    goto ok2;
                }
                parent = parent->parent;
            }
    }
  ok2:

    switch (edge->direction)
    {
    case EdgeDirection_Undefined:
    case EdgeDirection_Sync:
        if (useControlForLayout)
            fprintf (dotFile, "comp%ld -> comp%ld [ id=%ld arrowhead=odot arrowtail=dot dir=forward", (long) source, (long) dest, (long) con);
        else
            fprintf (dotFile, "comp%ld -> comp%ld [ id=%ld arrowhead=odot arrowtail=dot dir=none", (long) source, (long) dest, (long) con);
        break;
    case EdgeDirection_Push:
        fprintf (dotFile, "comp%ld -> comp%ld [ id=%ld arrowhead=normal arrowtail=dot dir=forward", (long) source, (long) dest, (long) con);
        break;
    case EdgeDirection_Pull:
        if (useControlForLayout)
            fprintf (dotFile, "comp%ld -> comp%ld [ id=%ld arrowhead=odot arrowtail=normal dir=forward", (long) source, (long) dest, (long) con);
        else
            fprintf (dotFile, "comp%ld -> comp%ld [ id=%ld arrowhead=odot arrowtail=normal dir=forward", (long) dest, (long) source, (long) con);
        break;
    }

    struct Edge *edge1 = con->edges->data;

    if (edge1->noweight)
        fprintf (dotFile, " weight=0");

    fprintf (dotFile, " headlabel=\"0\" taillabel=\"\" ]\n");

    return TRUE;
}

FILE *StartNewDotFile (struct Vertex * vertex)
{
    // Export tmp.dot
    char *filename = g_strdup_printf ("tmp%ld.dot1", (long) vertex);

    dotFile = fopen (filename, "w");
    g_free (filename);

    if (!vertex)
        vertex = rootVertex;

    SaveGraph_SetSequentialVertexNum_value = 0;
    ForEachVertex (SaveGraph_SetSequentialVertexNum);

    fprintf (dotFile, "digraph anything {\n"
      //    "size=\"7,10\"\n"
      "dpi=72\n"
      "edge [ decorate=1 fontsize=8 fontname=Helvetica labelfontname=Helvetica labelfontsize=8 ]\n"
      "node [ fontsize=12 fontname=\"Helvetica-Bold\" shape=ellipse ]\n");

    fprintf (dotFile, "subgraph \"cluster_%ld\" {\n" "  label=\"", (long) vertex);
    char *label = vertex ? (vertex->label ? : vertex->name) : "<rooT>";

    PrintEscapedString (dotFile, label);
    fprintf (dotFile, "\"\n");

    return dotFile;
}

void FinishDotFile (FILE * dotFile, gpointer id)
{
    fprintf (dotFile, "}\n");

    struct Vertex *currentGroup = dotFilesVertices->data;
    GList *tmp = dotFilesEdgeConLists->data;

    while (tmp)
    {
        struct ConnectedVertex *con = tmp->data;

        con->tmp_data = 0;
        tmp = tmp->next;
    }

    tmp = dotFilesEdgeConLists->data;
    while (tmp)
    {
        struct ConnectedVertex *con = tmp->data;

        if (con->tmp_data == 0)
            FullLayoutWithDot_BottomUp_PrintEdgeCon_ForGroup (con, currentGroup);
        con->tmp_data = 1;
        tmp = tmp->next;
    }

    fprintf (dotFile, "}\n");
    fclose (dotFile);

    char *command = g_strdup_printf ("dot -o tmp%ld.dot2 tmp%ld.dot1", (long) id, (long) id);

    // printf ("command=%s\n",command);
    system (command);
    g_free (command);

    /*
       command = g_strdup_printf ("dot -Tps -o tmp%d.ps tmp%d.dot1", id, id);
       printf ("command=%s\n",command);
       system (command);
       g_free (command);
     */

    char *filename = g_strdup_printf ("tmp%ld.dot2", (long) id);

    libspring_LayoutProblem = ParseDotFile (filename, TRUE, currentGroup);
    remove (filename);
    g_free (filename);

    filename = g_strdup_printf ("tmp%ld.dot1", (long) id);
    remove (filename);
    g_free (filename);
}

gboolean ExportDotFile_BottomUp_PrintVertex1 (struct Vertex *vertex)
{
    char *label = vertex->label ? : vertex->name;

    if (vertex->isGroup)
    {
        if (vertex->areChildsVisible || vertex->areSubgroupsExpanded)
        {
            dotFiles = g_list_prepend (dotFiles, StartNewDotFile (vertex));
            dotFilesVertices = g_list_prepend (dotFilesVertices, vertex);
            dotFilesEdgeConLists = g_list_prepend (dotFilesEdgeConLists, NULL);
            if (!vertex->areChildsVisible)
                fprintf (dotFile, "  comp%ld [label=\"\", id=fake]\n", (long) vertex);
        } else
        {
            dotFile = dotFiles->data;
            fprintf (dotFile, "subgraph \"cluster_%ld\" {\n" "  label=\"", (long) vertex);
            PrintEscapedString (dotFile, label);
            fprintf (dotFile, "\"\n");
            fprintf (dotFile, "  comp%ld [label=\"\", id=fake]\n", (long) vertex);
            fprintf (dotFile, "}\n");
        }
    } else
    {
        dotFile = dotFiles->data;
        fprintf (dotFile, " comp%ld [label=\"", (long) vertex /*->data*/ );
        PrintEscapedString (dotFile, label);
        fprintf (dotFile, "\"]\n");
    }

    //  struct Vertex *currentGroup = dotFilesVertices->data;
    GList *tmp;

    for (tmp = vertex->connectedVertices; tmp; tmp = tmp->next)
    {
        struct ConnectedVertex *con = tmp->data;

        dotFilesEdgeConLists->data = g_list_prepend (dotFilesEdgeConLists->data, con);
        //   FullLayoutWithDot_BottomUp_PrintEdgeCon_ForGroup (con, currentGroup);
    }

    return TRUE;
}

gboolean ExportDotFile_BottomUp_PrintVertex2 (struct Vertex * vertex)
{
    if (vertex->isGroup && (vertex->areChildsVisible || vertex->areSubgroupsExpanded))
    {
        char *label = vertex->label ? : vertex->name;

        dotFile = dotFiles->data;
        FinishDotFile (dotFile, vertex);
        dotFiles = g_list_remove_link (dotFiles, dotFiles);
        dotFilesVertices = g_list_remove_link (dotFilesVertices, dotFilesVertices);
        g_list_free ((GList *) dotFilesEdgeConLists->data);
        dotFilesEdgeConLists = g_list_remove_link (dotFilesEdgeConLists, dotFilesEdgeConLists);
        //        fprintf (dotFile, "}\n");

        dotFile = dotFiles->data;
        fprintf (dotFile, "subgraph \"cluster_%ld\" {\n  label=\"", (long) vertex);
        PrintEscapedString (dotFile, label);
        fprintf (dotFile, "\"\n");
        fprintf (dotFile,
          "  comp%ld [label=\"\", id=fake, width=%f, height=%f]\n", (long) vertex, vertex->rel_sizeX * 2 / DPI, vertex->rel_sizeY * 2 / DPI);
        fprintf (dotFile, "}\n");
    }
    return TRUE;
}

gboolean ExportDotFile_BottomUp_PrintEdge (struct Edge * edge)
{
    fprintf (dotFile, "comp%ld -> comp%ld [ id=%ld ", (long) edge->source, (long) edge->dest, (long) edge);

    switch (edge->direction)
    {
    case EdgeDirection_Undefined:
    case EdgeDirection_Sync:
        fprintf (dotFile, "arrowhead=odot arrowtail=dot dir=forward");
        break;
    case EdgeDirection_Push:
        fprintf (dotFile, "arrowhead=normal arrowtail=dot dir=forward");
        break;
    case EdgeDirection_Pull:
        fprintf (dotFile, "arrowhead=odot arrowtail=normal dir=forward");
        break;
    }

    if (edge->noweight)
        fprintf (dotFile, " weight=0");
    fprintf (dotFile, " headlabel=\"\" taillabel=\"\" ]\n");

    return TRUE;
}

gboolean ClearVertexPosAndSize (struct Vertex * vertex)
{
    vertex->rel_posX = 0;
    vertex->rel_posY = 0;
    vertex->rel_sizeX = 0;
    vertex->rel_sizeY = 0;
    return TRUE;
}

void FullLayoutWithDot_BottomUp (void)
{
    dotFiles = NULL;

    dotFiles = g_list_prepend (dotFiles, StartNewDotFile (NULL /*rootVertex */ ));
    dotFilesVertices = g_list_prepend (dotFilesVertices, rootVertex);
    dotFilesEdgeConLists = g_list_prepend (dotFilesEdgeConLists, NULL);

    ForEachVertex (ClearVertexPosAndSize);

    ForEachVisibleVertexRecursiveAndPorts (NULL, ExportDotFile_BottomUp_PrintVertex1, ExportDotFile_BottomUp_PrintVertex2);

    //    ForEachEdge (FullLayoutWithDot_PrintEdge);
    //    ForEachVisibleEdgeAndPorts (ExportDotFile_BottomUp_PrintEdgeCon);

    dotFile = dotFiles->data;
    FinishDotFile (dotFile, 0 /*(int)rootVertex */ );
    dotFiles = g_list_remove_link (dotFiles, dotFiles);
    dotFilesVertices = g_list_remove_link (dotFilesVertices, dotFilesVertices);
    g_list_free ((GList *) dotFilesEdgeConLists->data);
    dotFilesEdgeConLists = g_list_remove_link (dotFilesEdgeConLists, dotFilesEdgeConLists);

    if (dotFiles != NULL)
        printf ("Warning: dotFiles != NULL\n");
    if (dotFilesVertices != NULL)
        printf ("Warning: dotFilesVertices != NULL\n");
    if (dotFilesEdgeConLists != NULL)
        printf ("Warning: dotFilesEdgeConLists != NULL\n");
}

/****************************************/

FILE *Screenshot_File = NULL;
int ScreenShot_PrintEdge_PassNum;
extern int ChannelColorLimit1, ChannelColorLimit2;
gboolean ScreenShot_PrintVertex (struct Vertex *vertex)
{
    double x = GetVertexCoordX (vertex);
    double y = GetVertexCoordY (vertex);
    double sizeX = GetVertexSizeX (vertex);
    double sizeY = GetVertexSizeY (vertex);

    if (vertex->isGroup)
    {
        fprintf (Screenshot_File,
          //     "0 0 0 setrgbcolor\n"
          "%f %f %f %f rectstroke\n", (float) x - sizeX, (float) -(y - sizeY), (float) 2 * sizeX, (float) -2 * sizeY);
        //  DrawRectangle (x-sizeX, y-sizeY, sizeX*2, sizeY*2);
        //  DrawMultiLineText2 (x-sizeX + 5, y-sizeY + 5, vertex->label?:vertex->name, 0, 2, 12);
        fprintf (Screenshot_File,
          //     "0 0 0 setrgbcolor\n"
          "(%s) %f %f text_topleft\n", vertex->name, (float) x - sizeX, (float) -(y - sizeY));
    } else
    {
        fprintf (Screenshot_File,
          //     "0 0 0 setrgbcolor\n"
          "gsave %f %f translate %f %f scale 0 0 20 0 360 arc stroke grestore\n", (float) x, (float) -y, (float) sizeX / 20, (float) sizeY / 20);

        //  DrawText (x, y, vertex->label?:vertex->name, 1, 1, 11);
        fprintf (Screenshot_File,
          //     "1 0 0 setrgbcolor\n"
          "(%s) %f %f text_centre\n", vertex->label ? : vertex->name, (float) x, (float) -y);
    }

    return TRUE;
}

void PSPrintColorArrow (FILE * Screenshot_File, float x1, float y1, float x2, float y2)
{
    double length, dxnorm, dynorm;

    PrepareNormals (x1, y1, x2, y2, &length, &dxnorm, &dynorm);

    fprintf (Screenshot_File, "%f %f moveto %f %f lineto stroke\n", (x1), -(float) (y1), (float) (x2), -(float) (y2));
    fprintf (Screenshot_File, "%f %f moveto %f %f lineto stroke\n", (x2),
      -(float) (y2), (float) (x2 - 8 * (dxnorm - dynorm)), -(float) (y2 - 8 * (dxnorm + dynorm)));
    fprintf (Screenshot_File, "%f %f moveto %f %f lineto stroke\n", (x2),
      -(float) (y2), (float) (x2 - 8 * (dxnorm + dynorm)), -(float) (y2 + 8 * (dxnorm - dynorm)));
}

gboolean ScreenShot_PrintEdge (struct Edge *edge)
{
    struct Vertex *v1 = GetSourceVertex (edge)->deepestVisibleParent;
    struct Vertex *v2 = GetDestVertex (edge)->deepestVisibleParent;

    double length, dxnorm, dynorm;

    double centre_x1 = v1->posX;
    double centre_y1 = v1->posY;
    double centre_x2 = v2->posX;
    double centre_y2 = v2->posY;

    PrepareNormals (centre_x1, centre_y1, centre_x2, centre_y2, &length, &dxnorm, &dynorm);

    switch (ScreenShot_PrintEdge_PassNum)
    {
    case 1:
        if (length >= ChannelColorLimit2)
            ;
        else
            return TRUE;
        break;
    case 2:
        if (length >= ChannelColorLimit2)
            return TRUE;
        if (length >= ChannelColorLimit1)
            ;
        else
            return TRUE;
        break;
    case 3:
        if (length >= ChannelColorLimit2)
            return TRUE;
        if (length >= ChannelColorLimit1)
            return TRUE;
        else;
        break;
    default:
        printf ("Error ScreenShot_PassNum=%d\n", ScreenShot_PrintEdge_PassNum);
        exit (-1);
    }

    double k1, k2;

    if (!v1->isGroup)
    {                           // ellipse
        k1 = sqrt (1 / (SQ (dxnorm / v1->sizeX) + SQ (dynorm / v1->sizeY)));
    } else
    {                           // rectangle
        double k1a = ABS (v1->sizeX / dxnorm);
        double k1b = ABS (v1->sizeY / dynorm);

        if (k1a < k1b)
            k1 = k1a;
        else
            k1 = k1b;
    }

    if (!v2->isGroup)
    {                           // ellipse
        k2 = sqrt (1 / (SQ (dxnorm / v2->sizeX) + SQ (dynorm / v2->sizeY)));
    } else
    {                           // rectangle
        double k2a = ABS (v2->sizeX / dxnorm);
        double k2b = ABS (v2->sizeY / dynorm);

        if (k2a < k2b)
            k2 = k2a;
        else
            k2 = k2b;
    }

    double dx1a = dxnorm * (k1 + 4);
    double dy1a = dynorm * (k1 + 4);

    //double dx2a = dxnorm * (k2 + 4);
    //double dy2a = dynorm * (k2 + 4);
    double dx1b = dxnorm * (k1 + 8);
    double dy1b = dynorm * (k1 + 8);

    //double dx2b = dxnorm * (k2 + 8);
    //double dy2b = dynorm * (k2 + 8);
    //double dx1c = dxnorm * (k1 + 3);
    //double dy1c = dynorm * (k1 + 3);
    double dx2c = dxnorm * (k2 + 3);
    double dy2c = dynorm * (k2 + 3);

    //double dx1d = dxnorm * (k1 + 6);
    //double dy1d = dynorm * (k1 + 6);
    double dx2d = dxnorm * (k2 + 6);
    double dy2d = dynorm * (k2 + 6);

    /*
       int state = GetEdgeState (edge);
       gboolean isCurrentState = ((state & 128) == 128);
       int action = state & 127;
       if (action)
       ;//todo  color = GetColorGC (action);
     */

    // if (edge->selectionLevel) {
    //        DrawThickColorLine (GetColorGC (6)/*6=LightYellow for Selection*/, centre_x1, centre_y1, centre_x2, centre_y2, 16);
    // }

    switch (edge->direction)
    {
    case EdgeDirection_Undefined:
        //        DrawColorLine (color, centre_x1, centre_y1, centre_x2, centre_y2);
        fprintf (Screenshot_File, "%f %f moveto %f %f lineto stroke\n", (float) centre_x1, (float) -centre_y1, (float) centre_x2, (float) -centre_y2);
        break;
    case EdgeDirection_Sync:
        //        DrawColorLine (color, centre_x1+dx1b, centre_y1+dy1b, centre_x2-dx2d, centre_y2-dy2d);
        fprintf (Screenshot_File,
          "%f %f moveto %f %f lineto stroke\n", (float) centre_x1 + dx1b,
          (float) -(centre_y1 + dy1b), (float) centre_x2 - dx2d, (float) -(centre_y2 - dy2d));
        //        DrawFilledEllipse (centre_x1+dx1a, centre_y1+dy1a, 4, 4);
        fprintf (Screenshot_File,
          //     "0 0 0 setrgbcolor\n"
          "gsave %f %f translate %f %f scale 0 0 20 1 360 arc fill grestore\n",
          (float) centre_x1 + dx1a, -(float) (centre_y1 + dy1a), (float) .2, (float) .2);
        //        DrawEllipse (centre_x2-dx2c, centre_y2-dy2c, 3, 3);
        fprintf (Screenshot_File,
          //     "0 0 0 setrgbcolor\n"
          "gsave %f %f translate %f %f scale 0 0 20 0 360 arc stroke grestore\n",
          (float) centre_x2 - dx2c, -(float) (centre_y2 - dy2c), (float) .15, (float) .15);
        break;
    case EdgeDirection_Push:
        //        DrawColorArrow (color, centre_x1+dx1b, centre_y1+dy1b, centre_x2-dx2d, centre_y2-dy2d);
        PSPrintColorArrow (Screenshot_File, centre_x1 + dx1b, centre_y1 + dy1b, centre_x2 - dx2d, centre_y2 - dy2d);
        //        DrawFilledEllipse (centre_x1+dx1a, centre_y1+dy1a, 4, 4);
        fprintf (Screenshot_File,
          //     "0 0 0 setrgbcolor\n"
          "gsave %f %f translate %f %f scale 0 0 20 1 360 arc fill grestore\n",
          (float) centre_x1 + dx1a, -(float) (centre_y1 + dy1a), (float) .2, (float) .2);
        //        DrawEllipse (centre_x2-dx2c, centre_y2-dy2c, 3, 3);
        fprintf (Screenshot_File,
          //     "0 0 0 setrgbcolor\n"
          "gsave %f %f translate %f %f scale 0 0 20 0 360 arc stroke grestore\n",
          (float) centre_x2 - dx2c, -(float) (centre_y2 - dy2c), (float) .15, (float) .15);
        break;
    case EdgeDirection_Pull:
        //        DrawColorArrow (color, centre_x2-dx2d, centre_y2-dy2d, centre_x1+dx1b, centre_y1+dy1b);
        fprintf (Screenshot_File,
          "%f %f moveto %f %f lineto stroke\n", (float) centre_x2 - dx2d,
          -(float) (centre_y2 - dy2d), (float) centre_x1 + dx1b, -(float) (centre_y1 + dy1b));
        //        DrawFilledEllipse (centre_x1+dx1a, centre_y1+dy1a, 4, 4);
        fprintf (Screenshot_File,
          //     "0 0 0 setrgbcolor\n"
          "gsave %f %f translate %f %f scale 0 0 20 1 360 arc fill grestore\n",
          (float) centre_x1 + dx1a, -(float) (centre_y1 + dy1a), (float) .2, (float) .2);
        //        DrawEllipse (centre_x2-dx2c, centre_y2-dy2c, 3, 3);
        fprintf (Screenshot_File,
          //     "0 0 0 setrgbcolor\n"
          "gsave %f %f translate %f %f scale 0 0 20 0 360 arc stroke grestore\n",
          (float) centre_x2 - dx2c, -(float) (centre_y2 - dy2c), (float) .15, (float) .15);
        break;
    default:
        printf ("Error in ScreenShot_PrintEdge: invalid edge direction\n");
    }

    /*
       if (state)
       {
       char *value = GetEdgeValue (edge);

       if (value)
       {
       //            fprintf (stderr, "drawvalue %s\n", value);
       DrawColorText (color, (centre_x1 + centre_x2) / 2, (centre_y1 + centre_y2) / 2, value, 2, 1, 12);
       }
       }

       if (isCurrentState)
       {
       DrawThickColorLine (color, centre_x1, centre_y1, centre_x2, centre_y2, 4);

       char *text[4] = { "AD", "RU", "AU", "RD" };

       if (action >= 0 && action < 4)
       DrawColorText (GetColorGC (4), (centre_x1 + centre_x2) / 2, (centre_y1 + centre_y2) / 2, text[action], 0, 1, 12);
       else
       printf ("Unknown value in DrawEdge: action=%d\n", action);
       }
     */

    return TRUE;
}

gboolean ScreenShot_PrintEdgeCon (struct ConnectedVertex * con)
{
    GList *tmp;

    for (tmp = con->edges; tmp; tmp = tmp->next)
        ScreenShot_PrintEdge (tmp->data);

    return TRUE;
}

void ScreenShot (const char *filename)
{
    char buf[10000];

    getcwd (buf, 10000);
    printf ("Generating %s in %s\n", filename, buf);

    Screenshot_File = fopen (filename, "w");

    double A4_zoom = MIN ((20 * 72 / 2.54) / (rootVertex->sizeX * 2),
      (28.7 * 72 / 2.54) / (rootVertex->sizeY * 2));

    //    shift_X = -(x1 + x2 - (drawingArea_width / zoom)) / 2;
    //    shift_Y = -(y1 + y2 - (drawingArea_height / zoom)) / 2;

    fprintf (Screenshot_File,
      "%%!\n"
      "\n"
      "/Times-Roman findfont\n"
      "14 scalefont\n"
      "setfont\n"
      "\n"
      "/text_centre { moveto dup stringwidth pop 2 div neg -4 rmoveto show stroke } def\n"
      "/text_topleft { moveto 2 -14 rmoveto show stroke } def\n" "/scl %f def\n" "\n" "scl scl scale\n" "\n", A4_zoom);

    if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (gtk_object_get_data (GTK_OBJECT (MainWindow2), "ShowChansToggle"))))
    {
        ScreenShot_PrintEdge_PassNum = 1;
        fprintf (Screenshot_File, "0 setlinewidth\n" "0.6 0.6 0.6 setrgbcolor\n" "\n");
        ForEachVisibleEdgeAndPorts (ScreenShot_PrintEdgeCon);

        ScreenShot_PrintEdge_PassNum = 2;
        fprintf (Screenshot_File, "0 setlinewidth\n" "0.2 0.2 0.2 setrgbcolor\n" "\n");
        ForEachVisibleEdgeAndPorts (ScreenShot_PrintEdgeCon);

        ScreenShot_PrintEdge_PassNum = 3;
        fprintf (Screenshot_File, "0.01 scl div setlinewidth\n" "0.0 0.0 0.0 setrgbcolor\n" "\n");
        ForEachVisibleEdgeAndPorts (ScreenShot_PrintEdgeCon);
    }

    fprintf (Screenshot_File, "0.04 scl div setlinewidth\n" "0.0 0.0 0.0 setrgbcolor\n" "\n");
    ForEachVisibleVertexRecursiveAndPorts (NULL, ScreenShot_PrintVertex, NULL);

    fprintf (Screenshot_File, "showpage\n");

    fclose (Screenshot_File);
}

/****************************************/

gboolean isEdgeVisible (struct Edge *edge)
{
    if (!(edge->source && isVertexVisible (edge->source)))
        return FALSE;
    if (!(edge->dest && isVertexVisible (edge->dest)))
        return FALSE;

    return TRUE;
}

void MakeVertexVisible (struct Vertex *vertex)
{
    if (!isVertexVisible (vertex->parent))
        MakeVertexVisible (vertex->parent);

    if (vertex->isGroup)
    {
        if (!vertex->parent->areSubgroupsExpanded)
        {
            // Expand all subgroups and hide them, to show only the one we want
            vertex->parent->areSubgroupsExpanded = TRUE;
            GList *tmp;

            for (tmp = vertex->parent->childs; tmp; tmp = tmp->next)
            {
                struct Vertex *v = tmp->data;

                if (v->isGroup)
                    v->isHidden = TRUE;
            }
        }
        vertex->isHidden = FALSE;
    } else
    {
        if (!vertex->parent->areChildsVisible)
        {
            //Make all childs visible and hide them, to show only the one we want
            vertex->parent->areChildsVisible = TRUE;
            vertex->parent->areSubgroupsExpanded = TRUE; // childs can't be visible and subgroups not expanded
            GList *tmp;

            for (tmp = vertex->parent->childs; tmp; tmp = tmp->next)
            {
                struct Vertex *v = tmp->data;

                v->isHidden = TRUE;
            }
        }
        vertex->isHidden = FALSE;

        // If there's only one hidden vertex in the group: unhide it
        GList *tmp;
        struct Vertex *hiddenVertex = NULL;

        for (tmp = vertex->parent->childs; tmp; tmp = tmp->next)
        {
            struct Vertex *v = tmp->data;

            if (!v->isGroup && v->isHidden)
            {
                if (hiddenVertex)
                    goto moreThanOneHiddenVertex;
                else
                    hiddenVertex = v;
            }
        }
        if (hiddenVertex)
            MakeVertexVisible (hiddenVertex);
      moreThanOneHiddenVertex:;
    }
}

void MakeEdgeVisible (struct Edge *edge)
{
    printf ("MakeEdgeVisible %s -> %s\n", edge->source->name, edge->dest->name);
    if (edge->source)
        MakeVertexVisible (edge->source);
    if (edge->dest)
        MakeVertexVisible (edge->dest);
}
