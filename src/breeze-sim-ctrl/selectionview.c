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

#include "selectionview.h"
#include "structure.h"
#include "libspring.h"
#include "simtrace.h"
#include "main.h"
#include <breeze/libbreeze.h>
#include <gtk/gtk.h>

static struct ViewProperties *selectionViewProperties = NULL;
static GtkList *selectionViewList = NULL;
GList *selectedChans = NULL;

/***************************/

/*
void SelectionView_UpdateList (void)
{
}
*/

/***************************/

void on_SelectionDeleteButton_clicked (GtkButton * button, gpointer user_data)
{
    while (selectionViewList->selection)
    {
        struct Chan *chan = gtk_object_get_data (GTK_OBJECT (selectionViewList->selection->data),
          "Chan");

        Core_SelectChannel (chan);
    }
}

void on_SelectionShowHideButton_clicked (GtkButton * button, gpointer user_data)
{
    gboolean haveDoneSomething = FALSE;
    GList *tmp;

    for (tmp = selectionViewList->selection; tmp; tmp = tmp->next)
    {
        struct Chan *chan = gtk_object_get_data (GTK_OBJECT (tmp->data), "Chan");
        struct Edge *edge = chan->data;

        if (!isEdgeVisible (edge))
        {
            MakeEdgeVisible (edge);
            haveDoneSomething = TRUE;
        }
    }

    if (haveDoneSomething)
        Core_ExpandCollapseUpdateAllGroups ();
}

void GetDilatationList_AddCompChannels (GList ** p_result, GList * selection, struct Comp *comp);
GList *GetDilatationList (GList * selection)
{
    GList *result = NULL;
    GList *tmp;

    for (tmp = selection; tmp; tmp = tmp->next)
    {
        struct Chan *chan = tmp->data;

        GetDilatationList_AddCompChannels (&result, selection, chan->source);
        GetDilatationList_AddCompChannels (&result, selection, chan->dest);
    }
    return result;
}
void GetDilatationList_AddCompChannels (GList ** p_result, GList * selection, struct Comp *comp)
{
    if (comp)
    {
        GList *tmp2;

        for (tmp2 = comp->channels; tmp2; tmp2 = tmp2->next)
            if (!g_list_find (*p_result, tmp2->data) && !g_list_find (selection, tmp2->data))
                *p_result = g_list_prepend (*p_result, tmp2->data);
    }
}

void on_SelectionDilateButton_clicked (GtkButton * button, gpointer user_data)
{
    GList *selection = NULL;
    GList *tmp;

    for (tmp = selectionViewList->selection; tmp; tmp = tmp->next)
    {
        struct Chan *chan = gtk_object_get_data (GTK_OBJECT (tmp->data), "Chan");

        selection = g_list_prepend (selection, chan);
    }

    GList *dilatation = GetDilatationList (selection);

    for (tmp = dilatation; tmp; tmp = tmp->next)
    {
        struct Chan *otherChan = tmp->data;

        if (otherChan && !g_list_find (selectedChans, otherChan))
            Core_SelectChannel (otherChan);
    }

    g_list_free (dilatation);
    g_list_free (selection);
}

void on_SelectionRunUntilButton_clicked (GtkButton * button, gpointer user_data)
{
    if (!selectionViewList->selection)
        return;

    GList *selection = NULL;
    GList *tmp;

    for (tmp = selectionViewList->selection; tmp; tmp = tmp->next)
    {
        struct Chan *chan = gtk_object_get_data (GTK_OBJECT (tmp->data), "Chan");

        selection = g_list_prepend (selection, chan);
    }

    GList *activatedChans = Core_SimTrace_RunUntilActivityInChanList (selection);

    g_list_free (activatedChans);
    g_list_free (selection);
}

void on_SelectionFollowButton_clicked (GtkButton * button, gpointer user_data)
{
    /*
       -- Algo --
       If no activity in selected channels
       Then Run until first activity in selected chans and return
       Else:
       Run until next activity in selected chans U their dilatation
       Add unselected activated channel(s) to the selection
     */

    if (!selectionViewList->selection)
        return;

    GList *selection = NULL;
    GList *tmp;

    for (tmp = selectionViewList->selection; tmp; tmp = tmp->next)
    {
        struct Chan *chan = gtk_object_get_data (GTK_OBJECT (tmp->data), "Chan");

        selection = g_list_prepend (selection, chan);
    }

    for (tmp = selection; tmp; tmp = tmp->next)
    {
        struct Chan *chan = tmp->data;
        struct Edge *edge = chan->data;
        int state = GetEdgeState (edge);

        if (state)
            goto activityDetectedInSelection;
    }

    // No activity in selected channels => Run until first activity in selected chans and return
    GList *activatedChans = Core_SimTrace_RunUntilActivityInChanList (selection);

    g_list_free (activatedChans);
    g_list_free (selection);
    return;

  activityDetectedInSelection:
    {
        // Run until next activity in selected chans U their dilatation
        GList *dilatation = GetDilatationList (selection);
        GList *both = g_list_concat (g_list_copy (selection), g_list_copy (dilatation));
        GList *activatedChans = Core_SimTrace_RunUntilActivityInChanList (both);

        // Add unselected activated channel(s) to the selection
        for (tmp = activatedChans; tmp; tmp = tmp->next)
        {
            struct Chan *chan = tmp->data;
            struct Edge *edge = chan->data;
            int state = GetEdgeState (edge);

            if (state & 127)
            {
                if (edge->selectionLevel == 0)
                    Core_SelectChannel (chan);
            } else
            {
                if (edge->selectionLevel != 0)
                    Core_SelectChannel (chan);
            }
        }
        g_list_free (activatedChans);
    }
}

void on_SelectionCaptureToggleButton_toggled (GtkToggleButton * togglebutton, gpointer user_data)
{
    gboolean active = gtk_toggle_button_get_active (togglebutton);

    Core_SetContinuousMouseSelectionCapture (active);
}

gboolean SelectionChanNumSearchEntry_subfct (struct Edge *edge)
{
    GtkEditable *editable = GTK_EDITABLE (gtk_object_get_data (GTK_OBJECT (MainWindow2), "SelectionChanNumSearchEntry"));
    char *str = gtk_editable_get_chars (editable, 0, -1);

    PtrEdgeInfo edgeInfo = edge->data;

    if (edgeInfo)
    {
        struct Chan *chanStruct = edgeInfo->chan;

        if (chanStruct)
        {
            struct BreezePartChannelsChannel *breezePartChannelsChannel = chanStruct->channel;

            if (breezePartChannelsChannel)
            {
                char *channelName = getBreezePartChannelsChannelName (breezePartChannelsChannel);

                if (channelName)
                {
                    if (ENDS_WITH (channelName, str))
                    {
                        Core_SelectChannel (chanStruct);
                    }
                }
            }
        }
    }
    return TRUE;
}

void on_SelectionChanNumSearchEntry_activate (GtkEditable * editable, gpointer user_data)
{
    ForEachEdge (SelectionChanNumSearchEntry_subfct);
}

/***************************/

void SelectionView_SelectChannel (struct Chan *chan)
{
    DEBUG_printf ("SelectionView_SelectChannel\n");

    GList *elt = g_list_find (selectedChans, chan);

    if (elt)
    {
        int num = g_list_position (selectedChans, elt);

        selectedChans = g_list_remove_link (selectedChans, elt);
        gtk_list_clear_items (selectionViewList, num, num + 1);
    } else
    {
        selectedChans = g_list_append (selectedChans, chan);

        struct BreezePartChannelsChannel *channel = chan->channel;
        struct Position *pos = getBreezePartChannelsChannelPosition (channel);
        char *channelName = getBreezePartChannelsChannelName (channel);
        char *text;

        if (channelName)
            text = g_strdup_printf ("%d \"%s\": %s @%d:%d", chan->num, channelName, pos->filename, pos->line, pos->column);
        else
            text = g_strdup_printf ("%d: %s @%d:%d", chan->num, pos->filename, pos->line, pos->column);

        GtkWidget *item = gtk_list_item_new_with_label (text);

        gtk_object_set_data (GTK_OBJECT (item), "Chan", chan);
        gtk_widget_show (item);
        g_free (text);

        GList *items = g_list_append (NULL, item);

        gtk_list_append_items (selectionViewList, items);

        int size = g_list_length (selectionViewList->children);

        gtk_list_select_item (selectionViewList, size - 1);
    }
}

void SelectionView_ClearSelection (void)
{
    while (selectionViewList->children)
    {
        struct Chan *chan = gtk_object_get_data (GTK_OBJECT (selectionViewList->children->data),
          "Chan");

        Core_SelectChannel (chan);
    }
}

struct ViewProperties *SelectionView_Init (void)
{
    selectionViewList = gtk_object_get_data (GTK_OBJECT (MainWindow2), "SelectionViewList");

    selectionViewProperties = g_new0 (struct ViewProperties, 1);

    selectionViewProperties->SelectChannel = SelectionView_SelectChannel;
    selectionViewProperties->ClearSelection = SelectionView_ClearSelection;
    return selectionViewProperties;
}

void SelectionView_ToggleActivate (gboolean activate)
{
    if (activate)
        ;                       //        SelectionView_UpdateList ();

    selectionViewProperties->enabled = activate;
}
