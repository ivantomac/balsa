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

#ifndef LIBSPRING_H
#define LIBSPRING_H

#include <glib.h>
#include <gtk/gtk.h>

extern GPtrArray *edges;
extern gboolean libspring_LayoutProblem;
extern gboolean useControlForLayout;

struct ThreadGroup
{
    int num;
    GList *childs;
};

struct Vertex
{
    char *label;                // Text displayed on the screen
    char *name;                 // Initial name of the real thing: procedure name, breeze component name, ...
    int type;
    void *data;
    struct Vertex *parent;
    struct ThreadGroup *threadgroup;

    gboolean isGroup;
    // 4 group properties:
    gboolean isShrunk;          // indicates if the group is not developed. // NOT USED ANYMORE
    gboolean areSubgroupsExpanded;
    gboolean areChildsVisible;
    gboolean isHidden;
    GList *childs;

    double rel_posX;
    double rel_posY;
    double rel_sizeX;
    double rel_sizeY;
    int color;

    GtkCTreeNode *hierarchyWindowNode;
    struct Position *balsaPosition;

    // Values refreshed every time a group is developed/shrunk
    struct Vertex *deepestVisibleParent;
    GList *connectedVertices;   // List of struct ConnectedVertex
    double circle_containing_childs_sizeX;
    double circle_containing_childs_sizeY;
    double posX;
    double posY;
    double posZ;
    double sizeX;
    double sizeY;

    void *data2;
};

enum EdgeDirection
{ EdgeDirection_Undefined, EdgeDirection_Sync, EdgeDirection_Pull,
    EdgeDirection_Push
};

#define EDGE_TYPE_WIREFORK_TREE 1
#define EDGE_TYPE_MODULE_MAINLOOP 2

struct Edge
{
    int type;                   // 0=normal, 1=wirefork channel, 2=inside the main loop of a module
    int selectionLevel;
    struct Vertex *source;
    struct Vertex *dest;
    enum EdgeDirection direction;
    int noweight;

    void *data;
};

struct ConnectedVertex
{
    GList *edges;
    struct Edge edge;
    int weight;
    int overlapping_edges_num;
    int visibleVertices;        // 1:source vertex is visible, 2:dest vertex is visible, 3:both

    int tmp_data;
};

extern struct Vertex *rootVertex;

#define isVertexVisible(v) (!v->isHidden && (!v->parent || (v->parent==v->parent->deepestVisibleParent && v->parent->areChildsVisible)))

struct Vertex *AddNewVertex (const char *name, gboolean isGroup, struct Vertex *parent);
struct Vertex *AddNewVertexWithData (const char *name, gboolean isGroup, struct Vertex *parent, void *data);
struct Edge *AddNewEdge (struct Vertex *, struct Vertex *);

void reset_libspring (void);
void ComputeThreadGroups ();
void InitialiseVertexSizeRecursively (struct Vertex *);
void InitialiseVertexPositionRecursively (struct Vertex *, double x, double y);
void ComputeRealVertexSizeAndPositionRecursively (struct Vertex *);

typedef gboolean (*CALLBACK_VERTEX) (struct Vertex *);
typedef gboolean (*CALLBACK_EDGE) (struct Edge *);
typedef gboolean (*CALLBACK_EDGECON) (struct ConnectedVertex *);
typedef int (*COMPARISON_FCT) (void *, void *);

// Accessing Vertices
void ForEachVertex (CALLBACK_VERTEX);
void ForEachVisibleVertex (CALLBACK_VERTEX);
void ForEachVertexRecursive (struct Vertex *, CALLBACK_VERTEX fct_begin, CALLBACK_VERTEX fct_end);
void ForEachVisibleVertexRecursive (struct Vertex *, CALLBACK_VERTEX fct_begin, CALLBACK_VERTEX fct_end);
void ForEachVisibleVertexRecursiveAndPorts (struct Vertex *, CALLBACK_VERTEX fct_begin, CALLBACK_VERTEX fct_end);
void ForEachVertexConnectedTo (struct Vertex *, CALLBACK_VERTEX);
GList *GetEdgesConnectedTo (struct Vertex *);
double GetVertexCoordX (struct Vertex *);
double GetVertexCoordY (struct Vertex *);
double GetVertexSizeX (struct Vertex *);
double GetVertexSizeY (struct Vertex *);
double GetVertexRelSizeX (struct Vertex *);
double GetVertexRelSizeY (struct Vertex *);
char *GetVertexName (struct Vertex *);
void SetVertexName (struct Vertex *, char *name);
void *GetVertexData (struct Vertex *);
void SetVertexData (struct Vertex *, void *data);

struct Vertex *SearchVertex (COMPARISON_FCT, void *);
void DestroyVertex (struct Vertex *);

void ExpandGroupVertex (struct Vertex *);
void ShrinkGroupVertex (struct Vertex *);

void RefreshAllConnectedVertices (void);
void RecursivelyRefreshDeepestExpandedParent (struct Vertex *vertex, gboolean isVisible
  /*parentIsExpanded */ ,
  struct Vertex *deepestVisibleParent);

// Accessing Edges
void ForEachEdge (CALLBACK_EDGE);
void ForEachVisibleEdge (CALLBACK_EDGECON);
void ForEachVisibleEdgeAndPorts (CALLBACK_EDGECON);

//void ForEachVisibleLevelEdge (CALLBACK_EDGE);
void DestroyEdge (struct Edge *edge);
void DestroyEdgeWithoutFree (struct Edge *edge);
extern enum EdgeDirection SearchAndDestroyEdge_DestroyedEdgeDirection;
gboolean SearchAndDestroyEdge (struct Vertex *, struct Vertex *);
struct Vertex *GetSourceVertex (struct Edge *);
struct Vertex *GetDestVertex (struct Edge *);

void PruneEmptyGroups (struct Vertex *);
void PruneGroupsWithOneGroupElement (struct Vertex *);
void PruneColoredVertices (struct Vertex *);

//void InitialiseLevelEdges (void);

void UpdateVertexPositions (void);

void SaveGraph (const char *filename);
void LoadGraph (const char *filename);

void ExportDotFile (const char *filename);
void LayoutWithDot (struct Vertex *);
void FullLayoutWithDot (void);

#define LINE_WIDTH_FOR_NON_EMPTY_GROUP 20
#define LINE_WIDTH_FOR_EMPTY_GROUP 5
char *strdup_ApplyMaxWidth (const char *str, int length, int returnType);
gboolean UpdateVertexLabel (struct Vertex *v);
void UpdateAllVertexLabels (void);

void SetVisibleRootVertex (struct Vertex *v);

gboolean isEdgeVisible (struct Edge *edge);
void MakeEdgeVisible (struct Edge *edge);

void ScreenShot (const char *filename);

extern double MoveGroupContent_X;
extern double MoveGroupContent_Y;
gboolean MoveGroupContent (struct Vertex *v);

#endif
