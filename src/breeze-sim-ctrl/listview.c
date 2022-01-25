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

#include "listview.h"
#include "structure.h"
#include "libspring.h"
#include "main.h"
#include <breeze/libbreeze.h>
#include <gtk/gtk.h>

#define GTK_ENABLE_BROKEN
#include <gtk/gtktreeitem.h>

static struct ViewProperties *listViewProperties = NULL;

//static GtkTree *listViewTree = NULL;

/***************************/

static gboolean on_calllistExpandCollapse_active = FALSE;
void on_calllistExpandCollapse (GtkTreeItem * tree_item, void *user_data)
{
    if (!on_calllistExpandCollapse_active)
        return;

    DEBUG_printf ("on_calllistExpandCollapse %d\n", GPOINTER_TO_INT (user_data));

    struct Vertex *v = gtk_object_get_data (GTK_OBJECT (tree_item), "Vertex");

    if (!v)
    {
        fprintf (stderr, "Error on_calllistExpandCollapse: v == 0\n");
        return;
    }

    Core_ExpandCollapseGroup (v, GPOINTER_TO_INT (user_data));
}

/*
gboolean ListView_InitialiseList_subfct (struct Vertex *vertex)
{
    if (vertex->isGroup)
    {
        GtkWidget *frame = gtk_frame_new (NULL);

        gtk_widget_show (frame);

        GtkWidget *insideTree = gtk_tree_new ();

        gtk_tree_set_selection_mode (GTK_TREE (insideTree), GTK_SELECTION_MULTIPLE);
        gtk_widget_show (insideTree);
        GtkWidget *internalItem = gtk_tree_item_new_with_label (vertex->name);

        gtk_widget_show (internalItem);
        gtk_tree_append (GTK_TREE (insideTree), internalItem);
        gtk_object_set_data (GTK_OBJECT (internalItem), "Vertex", vertex);
        gtk_signal_connect (GTK_OBJECT (internalItem), "expand", GTK_SIGNAL_FUNC (on_calllistExpandCollapse),
          (gpointer) 0);
        gtk_signal_connect (GTK_OBJECT (internalItem), "collapse", GTK_SIGNAL_FUNC (on_calllistExpandCollapse),
          (gpointer) 1);

        GtkWidget *subtree = gtk_tree_new ();

        gtk_widget_show (subtree);
        gtk_tree_item_set_subtree (GTK_TREE_ITEM (internalItem), subtree);

        GtkWidget *subtree_ports;

        {
            // Create Ports section
            GtkWidget *item_ports = gtk_tree_item_new_with_label ("ports");

            gtk_widget_show (item_ports);
            gtk_tree_append (GTK_TREE (subtree), item_ports);

            subtree_ports = gtk_tree_new ();
            gtk_widget_show (subtree_ports);
            gtk_tree_item_set_subtree (GTK_TREE_ITEM (item_ports), subtree_ports);
            gtk_tree_item_collapse (GTK_TREE_ITEM (item_ports));

            // Create Chans section
            GtkWidget *item_chans = gtk_tree_item_new_with_label ("chans");

            gtk_widget_show (item_chans);
            gtk_tree_append (GTK_TREE (subtree), item_chans);

            GtkWidget *subtree_chans = gtk_tree_new ();

            gtk_widget_show (subtree_chans);
            gtk_tree_item_set_subtree (GTK_TREE_ITEM (item_chans), subtree_chans);
            gtk_tree_item_collapse (GTK_TREE_ITEM (item_chans));

            // Create Comps section
            GtkWidget *item_comps = gtk_tree_item_new_with_label ("comps");

            gtk_widget_show (item_comps);
            gtk_tree_append (GTK_TREE (subtree), item_comps);

            GtkWidget *subtree_comps = gtk_tree_new ();

            gtk_widget_show (subtree_comps);
            gtk_tree_item_set_subtree (GTK_TREE_ITEM (item_comps), subtree_comps);
            gtk_tree_item_collapse (GTK_TREE_ITEM (item_comps));

            // Fill in Chans
#ifdef 0
               GList *chans = hashedStruct->chans;

               for (; chans; chans = chans->next)
               {
               struct Chan *chan = chans->data;
               struct BreezePartChannelsChannel *channel = chan->channel;

               char *channame = getBreezePartChannelsChannelName (channel);

               char *name = g_strdup_printf ("%s", channame);
               GtkWidget *subitem = gtk_tree_item_new_with_label (name);

               gtk_widget_show (subitem);
               gtk_tree_prepend (GTK_TREE (subtree_chans), subitem);
               g_free (name);
               }
#endif
        }

#ifdef 0
           {                           // Fill in Ports
           GList *ports = hashedStruct->ports;

           for (; ports; ports = ports->next)
           {
           struct HashedPortStruct *hashedPortStruct = ports->data;

           char *buf;
           char *channame = NULL;
           char *channame_part2 = NULL;

           if (hashedPortStruct->chan)
           {
           channame = getBreezePartChannelsChannelName (hashedPortStruct->chan);
           if (channame && *channame)
           {
           channame = g_strdup (channame);
           if (*channame == ':')
           {
           channame_part2 = channame;
           channame = NULL;
           } else
           {
           channame_part2 = strstr (channame, "_[");
           if (channame_part2)
           {
           *channame_part2 = 0;
           channame_part2++;
           }
           }
           }
           }
           if (channame && *channame)
           {
           buf = g_strdup_printf ("%s%s%s", channame, channame_part2 ? " " : "", channame_part2 ? : "");
           } else
           {
           buf = g_strdup_printf ("%s%s", channame_part2 ? " " : "", channame_part2 ? : "");
           channame = NULL;
           }

           {
           GtkWidget *subitem = gtk_tree_item_new_with_label (buf);

           gtk_widget_show (subitem);
           gtk_tree_append (GTK_TREE (subtree_ports), subitem);
           }

           g_free (buf);
           g_free (channame);
           }
           }
#endif

        gtk_container_add (GTK_CONTAINER (frame), insideTree);

        GtkWidget *listViewItem = gtk_tree_item_new ();

        struct VertexInfo *vertexInfo = (struct VertexInfo *) GetVertexData (vertex);

        vertexInfo->infoForListView.listViewItem = listViewItem;
        vertexInfo->infoForListView.listViewInternalItem = internalItem;
        gtk_container_add (GTK_CONTAINER (listViewItem), frame);
        gtk_widget_show (listViewItem);
        gtk_object_set_data (GTK_OBJECT (listViewItem), "Vertex", vertex);
        gtk_signal_connect (GTK_OBJECT (listViewItem), "expand", GTK_SIGNAL_FUNC (on_calllistExpandCollapse),
          (gpointer) 2);
        gtk_signal_connect (GTK_OBJECT (listViewItem), "collapse", GTK_SIGNAL_FUNC (on_calllistExpandCollapse),
          (gpointer) 3);

        if (vertex->parent)
        {
            struct VertexInfo *parentVertexInfo = (struct VertexInfo *) GetVertexData (vertex->parent);

            if (!parentVertexInfo->infoForListView.listViewSubTree)
            {
                parentVertexInfo->infoForListView.listViewSubTree = gtk_tree_new ();
                gtk_tree_item_set_subtree (GTK_TREE_ITEM (parentVertexInfo->infoForListView.listViewItem),
                  parentVertexInfo->infoForListView.listViewSubTree);
                gtk_tree_item_expand (GTK_TREE_ITEM (parentVertexInfo->infoForListView.listViewItem));
            }
            gtk_tree_append (GTK_TREE (parentVertexInfo->infoForListView.listViewSubTree),
              vertexInfo->infoForListView.listViewItem);
        } else
            gtk_tree_append (listViewTree, vertexInfo->infoForListView.listViewItem);
    }
    return TRUE;
}
*/

void ListView_InitialiseList (void)
{
    static gboolean initialised = FALSE;

    if (initialised)
        return;
    else
        initialised = TRUE;

    //    listViewTree = gtk_object_get_data (GTK_OBJECT (MainWindow2), "CallListTree");

    //    ForEachVertexRecursive (Structure_GetRootVertex (), ListView_InitialiseList_subfct, NULL);
}

/***************************/

#define gtk_tree_item_expand2(x) { if (!(x)->expanded) gtk_tree_item_expand(x); }
#define gtk_tree_item_collapse2(x) { if ((x)->expanded) gtk_tree_item_collapse(x); }

gboolean ListView_UpdateVertexState (struct Vertex *vertex)
{
    if (vertex->isGroup)
    {
        gboolean save = on_calllistExpandCollapse_active;

        on_calllistExpandCollapse_active = FALSE;

        struct VertexInfo *vertexInfo = (struct VertexInfo *) GetVertexData (vertex);

        if (vertex->areChildsVisible)
        {
            gtk_tree_item_expand2 (GTK_TREE_ITEM (vertexInfo->infoForListView.listViewInternalItem));
        } else
        {
            gtk_tree_item_collapse2 (GTK_TREE_ITEM (vertexInfo->infoForListView.listViewInternalItem));
        }

        if (vertex->areSubgroupsExpanded)
        {
            gtk_tree_item_expand2 (GTK_TREE_ITEM (vertexInfo->infoForListView.listViewItem));
        } else
        {
            gtk_tree_item_collapse2 (GTK_TREE_ITEM (vertexInfo->infoForListView.listViewItem));
        }

        on_calllistExpandCollapse_active = save;
    }

    return TRUE;
}

void ListView_UpdateAllStates (void)
{
    //    ForEachVertexRecursive (Structure_GetRootVertex (), ListView_UpdateVertexState, NULL);
}

gboolean on_MaxDev_clicked_subfct (struct Vertex *v)
{
    if (v->isGroup)
    {
        //  Core_ExpandCollapseGroup (vertex, 0);
        v->areSubgroupsExpanded = TRUE;
        v->areChildsVisible = TRUE;
    }

    return TRUE;
}

void on_MaxDev_clicked (GtkButton * button, gpointer user_data)
{
    ForEachVertexRecursive (Structure_GetRootVertex (), on_MaxDev_clicked_subfct, NULL);
    Core_ExpandCollapseUpdateAllGroups ();
}

gboolean on_HalfDev_clicked_subfct (struct Vertex *v)
{
    if (v->isGroup)
    {
        //  Core_ExpandCollapseGroup (vertex, 0);
        v->areSubgroupsExpanded = TRUE;
        v->areChildsVisible = FALSE;
    }

    return TRUE;
}

void on_HalfDev_clicked (GtkButton * button, gpointer user_data)
{
    ForEachVertexRecursive (Structure_GetRootVertex (), on_HalfDev_clicked_subfct, NULL);
    Core_ExpandCollapseUpdateAllGroups ();
}

gboolean on_BehaviourDev_clicked_subfct (struct Vertex *v)
{
    if (v->isGroup)
    {
        v->areSubgroupsExpanded = FALSE;
        v->areChildsVisible = FALSE;
        if (BEGINS_WITH (v->name, "MOD_"))
        {
            struct Vertex *parent = v->parent;
            gboolean firstParentAlreadyHadOneVisibleChild = (v->parent->isHidden == FALSE);

            while (parent)
            {
                parent->areSubgroupsExpanded = TRUE;
                parent->isHidden = FALSE;
                parent = parent->parent;
            }
            v->parent->areSubgroupsExpanded = firstParentAlreadyHadOneVisibleChild;
            v->isHidden = FALSE;
        } else
            v->isHidden = TRUE;
    }

    return TRUE;
}

gboolean on_BehaviourDev_clicked_subfct_ReduceTreeSize (struct Vertex * v)
{
    if (v->isGroup && v->areSubgroupsExpanded)
    {
        // If all visible childs are called "MOD_*", i.e. don't have name, then reduce the tree
        GList *tmp = v->childs;
        int nbGoodSubgroups = 0;
        int nbMODSubgroups = 0;

        for (; tmp; tmp = tmp->next)
        {
            struct Vertex *v2 = tmp->data;

            if (v2->isGroup && !v2->isHidden)
            {
                if (BEGINS_WITH (v2->name, "MOD_"))
                    nbMODSubgroups++;
                else
                {
                    nbGoodSubgroups++;
                    return TRUE;
                }
            }
        }
        v->areSubgroupsExpanded = FALSE;
    }

    return TRUE;
}

gboolean on_BehaviourDev_clicked_subfct_ReduceTreeSizeMore (struct Vertex * v)
{
    if (v->isGroup && v->areSubgroupsExpanded)
    {
        // If all visible childs are called "MOD_*", i.e. don't have name, then reduce the tree
        GList *tmp = v->childs;
        int nbGoodSubgroups = 0;
        int nbMODSubgroups = 0;

        for (; tmp; tmp = tmp->next)
        {
            struct Vertex *v2 = tmp->data;

            if (v2->isGroup && !v2->isHidden)
            {
                if (BEGINS_WITH (v2->name, "MOD_"))
                    nbMODSubgroups++;
                else
                {
                    nbGoodSubgroups++;
                }
            }
        }
        if (nbMODSubgroups >= 2 && nbGoodSubgroups < nbMODSubgroups)
            v->areSubgroupsExpanded = FALSE;
    }

    return TRUE;
}

gboolean on_BehaviourDev_clicked_subfct_ReduceTreeSizeMore2 (struct Vertex * v)
{
    if (v->isGroup && v->areSubgroupsExpanded)
    {
        // If all visible childs are called "MOD_*", i.e. don't have name, then reduce the tree
        GList *tmp = v->childs;
        int nbGoodSubgroups = 0;
        int nbMODSubgroups = 0;

        for (; tmp; tmp = tmp->next)
        {
            struct Vertex *v2 = tmp->data;

            if (v2->isGroup && !v2->isHidden)
            {
                if (BEGINS_WITH (v2->name, "MOD_"))
                    nbMODSubgroups++;
                else
                {
                    nbGoodSubgroups++;
                }
            }
        }
        if (nbMODSubgroups >= 1 && nbGoodSubgroups <= nbMODSubgroups)
            v->areSubgroupsExpanded = FALSE;
    }

    return TRUE;
}

gboolean on_BehaviourDev_clicked_subfct_ReduceTreeSizeMore3 (struct Vertex * v)
{
    if (v->isGroup && v->areSubgroupsExpanded)
    {
        // If all visible childs are called "MOD_*", i.e. don't have name, then reduce the tree
        GList *tmp = v->childs;
        int nbGoodSubgroups = 0;
        int nbMODSubgroups = 0;

        for (; tmp; tmp = tmp->next)
        {
            struct Vertex *v2 = tmp->data;

            if (v2->isGroup && !v2->isHidden)
            {
                if (BEGINS_WITH (v2->name, "MOD_") || CONTAINS (v2->name, "buf") || CONTAINS (v2->name, "Buf"))
                    nbMODSubgroups++;
                else
                {
                    nbGoodSubgroups++;
                }
            }
        }
        if (nbMODSubgroups >= 1 && nbGoodSubgroups <= nbMODSubgroups)
            v->areSubgroupsExpanded = FALSE;
    }

    return TRUE;
}

void on_BehaviourDev_clicked (GtkButton * button, gpointer user_data)
{
    ForEachVertexRecursive (Structure_GetRootVertex (), on_BehaviourDev_clicked_subfct, NULL);
    Core_ExpandCollapseUpdateAllGroups ();
}

void on_BehaviourDev2_clicked (GtkButton * button, gpointer user_data)
{
    ForEachVertexRecursive (Structure_GetRootVertex (), on_BehaviourDev_clicked_subfct, NULL);
    ForEachVertexRecursive (Structure_GetRootVertex (), on_BehaviourDev_clicked_subfct_ReduceTreeSize, NULL);
    Core_ExpandCollapseUpdateAllGroups ();
}

void on_BehaviourDev3_clicked (GtkButton * button, gpointer user_data)
{
    ForEachVertexRecursive (Structure_GetRootVertex (), on_BehaviourDev_clicked_subfct, NULL);
    ForEachVertexRecursive (Structure_GetRootVertex (), on_BehaviourDev_clicked_subfct_ReduceTreeSizeMore, NULL);
    Core_ExpandCollapseUpdateAllGroups ();
}

void on_BehaviourDev4_clicked (GtkButton * button, gpointer user_data)
{
    ForEachVertexRecursive (Structure_GetRootVertex (), on_BehaviourDev_clicked_subfct, NULL);
    ForEachVertexRecursive (Structure_GetRootVertex (), on_BehaviourDev_clicked_subfct_ReduceTreeSizeMore2, NULL);
    Core_ExpandCollapseUpdateAllGroups ();
}

void on_BehaviourDev5_clicked (GtkButton * button, gpointer user_data)
{
    ForEachVertexRecursive (Structure_GetRootVertex (), on_BehaviourDev_clicked_subfct, NULL);
    ForEachVertexRecursive (Structure_GetRootVertex (), on_BehaviourDev_clicked_subfct_ReduceTreeSizeMore3, NULL);
    Core_ExpandCollapseUpdateAllGroups ();
}

/***************************/

void ListView_SelectChannel (struct Chan *chan)
{
    DEBUG_printf ("ListView_SelectChannel\n");
}

void ListView_ExpandCollapseGroup (struct Vertex *v, int expandCollapseType)
{
    DEBUG_printf ("ListView_ExpandCollapseGroup %d\n", expandCollapseType);

    ListView_UpdateVertexState (v);
}

void ListView_ExpandCollapseUpdateAllGroups (void)
{
    DEBUG_printf ("ListView_ExpandCollapseUpdateAllGroups\n");

    ListView_UpdateAllStates ();
}

struct ViewProperties *ListView_Init (void)
{
    listViewProperties = g_new0 (struct ViewProperties, 1);

    listViewProperties->SelectChannel = ListView_SelectChannel;
    listViewProperties->ExpandCollapseGroup = (CALLBACK_voidstar_int) ListView_ExpandCollapseGroup;
    listViewProperties->ExpandCollapseUpdateAllGroups = (CALLBACK_void) ListView_ExpandCollapseUpdateAllGroups;
    return listViewProperties;
}

void ListView_ToggleActivate (gboolean activate)
{
    if (activate)
    {
        ListView_InitialiseList ();
        ListView_UpdateAllStates ();
        on_calllistExpandCollapse_active = TRUE;
    }

    listViewProperties->enabled = activate;
}
