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

#include <stdio.h>
#include <stdlib.h>
#include <gtk/gtk.h>

#define GTK_ENABLE_BROKEN
#include <gtk/gtktext.h>

#include <breeze/libbreeze.h>
#include "sourceview.h"
#include "main.h"
#include "drawing.h"
#include "structure.h"
#include "core.h"

static struct ViewProperties *sourceViewProperties = NULL;

static GtkToggleButton *ActivateThreadsCheckbutton = NULL;
static GtkToggleButton *ShowThreadSourceCheckbutton = NULL;
static GtkToggleButton *NotebookStyleCheckbutton = NULL;

static char *menuPopup_filename = NULL;
static int menuPopup_line = 0;
static int menuPopup_column = 0;

struct Breakpoint
{
    char *filename;
    int line;
};
GList *breakpoints = NULL;

static struct Position *SelectedChannelPosition = NULL;
void ShowAllChansPositionsForFile (char *filename);

gboolean IsSourceViewClosed (void)
{
    GtkWidget *sourceViewToggleButton = GTK_WIDGET (gtk_object_get_data (GTK_OBJECT (MainWindow2), "SourceViewToggleButton"));

    return !gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (sourceViewToggleButton));
}

void InitSourceViewWindow (void)
{
    ActivateThreadsCheckbutton = GTK_TOGGLE_BUTTON (gtk_object_get_data (GTK_OBJECT (SourceViewWindow), "ActivateThreadsCheckbutton"));
    ShowThreadSourceCheckbutton = GTK_TOGGLE_BUTTON (gtk_object_get_data (GTK_OBJECT (SourceViewWindow), "ShowThreadSourceCheckbutton"));
    NotebookStyleCheckbutton = GTK_TOGGLE_BUTTON (gtk_object_get_data (GTK_OBJECT (SourceViewWindow), "NotebookStyleCheckbutton"));
}

void ResetSourceView (void)
{
    GtkWidget *tree = GTK_WIDGET (gtk_object_get_data (GTK_OBJECT (SourceViewWindow), "ThreadsCTree"));

    gtk_clist_clear (GTK_CLIST (tree));
}

gboolean on_SourceViewWindow_delete_event (GtkWidget * widget, GdkEvent * event, gpointer user_data)
{
    GtkWidget *sourceViewToggleButton = GTK_WIDGET (gtk_object_get_data (GTK_OBJECT (MainWindow2), "SourceViewToggleButton"));

    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (sourceViewToggleButton), FALSE);
    return TRUE;
}

gboolean on_SourceView_text_button_press_event (GtkWidget * widget, GdkEventButton * event, gpointer user_data)
{
    //    printf("button pressed\n");
    char str[100];

    sprintf (str, "%d", GPOINTER_TO_INT (user_data));
    GtkWidget *entry = GTK_WIDGET (gtk_object_get_data (GTK_OBJECT (SourceViewWindow), "entry1"));

    gtk_entry_set_text (GTK_ENTRY (entry), str);

    return FALSE;
}

char *open_filenames[100];
GtkText *sourceTexts[100];
int nb_open_files = 0;
char file_buf[100][100000];
int file_size[100];

gboolean OnSourceTextMousePressed (GtkWidget * widget, GdkEventButton * event, gpointer user_data);

int getSourceTextNumForFile (char *filename, gboolean avoidRecursive)
{
    if (!filename || !*filename)
        return -1;

    int i;

    for (i = 0; i < nb_open_files; i++)
    {
        if (!strcmp (open_filenames[i], filename))
            return i;
    }

    // Check that the file exists
    FILE *f = fopen (filename, "r");

    if (!f)
        return -1;

    // Create New SourceText
    GtkWidget *SourceViewHBox = GTK_WIDGET (gtk_object_get_data (GTK_OBJECT (SourceViewWindow), "SourceViewHBox"));
    GtkWidget *SourceViewNotebook = GTK_WIDGET (gtk_object_get_data (GTK_OBJECT (SourceViewWindow), "SourceViewNotebook"));

    GtkWidget *SourceTextScrolledWindow = gtk_scrolled_window_new (NULL, NULL);

    gtk_widget_ref (SourceTextScrolledWindow);
    gtk_widget_show (SourceTextScrolledWindow);
    if (gtk_toggle_button_get_active (NotebookStyleCheckbutton))
    {
        GtkWidget *newLabel = gtk_label_new (filename);

        gtk_notebook_append_page (GTK_NOTEBOOK (SourceViewNotebook), SourceTextScrolledWindow, newLabel);
        gtk_notebook_set_page (GTK_NOTEBOOK (SourceViewNotebook), -1);
    } else
        gtk_box_pack_start (GTK_BOX (SourceViewHBox), SourceTextScrolledWindow, TRUE, TRUE, 0);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (SourceTextScrolledWindow), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_object_set_data (GTK_OBJECT (SourceTextScrolledWindow), "filename", g_strdup (filename));

    GtkWidget *SourceText = gtk_text_new (NULL, NULL);

    gtk_signal_connect (GTK_OBJECT (SourceText), "button_press_event", GTK_SIGNAL_FUNC (OnSourceTextMousePressed), NULL);

    gtk_widget_ref (SourceText);
    gtk_text_set_line_wrap (GTK_TEXT (SourceText), FALSE);
    gtk_container_add (GTK_CONTAINER (SourceTextScrolledWindow), SourceText);

    sourceTexts[i] = GTK_TEXT (SourceText);

    // Fill the new SourceText with the file context
    open_filenames[i] = g_strdup (filename);
    file_size[i] = fread (file_buf[i], 1, 100000, f);
    file_buf[i][file_size[i]] = 0;
    gtk_text_insert (GTK_TEXT (SourceText), NULL, NULL, NULL, file_buf[i], file_size[i]);
    gtk_widget_show (SourceText);
    fclose (f);

    nb_open_files++;

    if (!avoidRecursive
      && gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (gtk_object_get_data (GTK_OBJECT (SourceViewWindow), "AutoColorChanPosCheckbutton"))))
        ShowAllChansPositionsForFile (filename);

    return i;
}

void BreakAtDataActivity_ActivityDetected (struct Position *pos);

void SourceViewHighlightPosition (struct Position *pos, double *ret_x, double *ret_y, int color_num, char *valueStr, gboolean jumpToPos)
{
    int line = pos->line;
    int col = pos->column;
    char *filename = pos->filename;

    //    printf ("SourceViewHighlightPosition %s:%d:%d\n", filename, line, col);

    int sourceTextNum = getSourceTextNumForFile (filename, FALSE);

    if (sourceTextNum < 0)
        return;

    if (jumpToPos && gtk_toggle_button_get_active (NotebookStyleCheckbutton))
    {
        GtkWidget *SourceViewNotebook = GTK_WIDGET (gtk_object_get_data (GTK_OBJECT (SourceViewWindow), "SourceViewNotebook"));

        gtk_notebook_set_page (GTK_NOTEBOOK (SourceViewNotebook), sourceTextNum);
    }

    GtkText *sourceText = sourceTexts[sourceTextNum];

    if (!sourceText)
        return;

    GtkTextFont *current_font = sourceText->current_font;
    GdkFont *font = *(GdkFont **) current_font;
    double h = (double) (font->ascent + font->descent);
    double y = h * (line - 1) + (h / 2);
    char *ptr = file_buf[sourceTextNum];
    int i;

    for (i = 1; i < line; i++)
    {
        while (*ptr != '\n')
            ptr++;
        ptr++;
    }
    double x = (double) gdk_text_width (font, ptr, col - 1);
    int length = 0;
    char c;

    do
    {
        length++;
        c = ptr[col - 1 + length];
    }
    while (c != ' ' && c != '\t' && c != '\n' && c != '\0' && c != '(' && c != ')' && c != ',' && c != ';');
    double w = (double) gdk_text_width (font, ptr + col - 1, length);

    gtk_adjustment_set_value (sourceText->vadj, y - sourceText->vadj->page_size / 3);

    // scroll bar adjustments
    x -= sourceText->hadj->value;
    y -= sourceText->vadj->value;

    GtkText *text = sourceText;

    //    gdk_draw_arc (text->text_area, GetColorGC (color_num), FALSE, x-3, y-h/2, w+5, h, 0, 360*64);
    gdk_draw_rectangle (text->text_area, GetColorGC (valueStr ? 1 : color_num), FALSE, x - 3, y - h / 2, w + 5, h);

    // Draw data if present
    if (valueStr)
    {
        GtkTextFont *current_font = text->current_font;
        GdkFont *font = *(GdkFont **) current_font;

        gdk_draw_text (text->text_area, font, GetColorGC (1), x, y - h, valueStr, strlen (valueStr));

        if (valueStr[0] != '(')
            BreakAtDataActivity_ActivityDetected (pos);
    }
    // switch to "absolute" coordinates (relative to the window)
    x += GTK_WIDGET (sourceText)->allocation.x;
    y += GTK_WIDGET (sourceText)->allocation.y;

    if (ret_x)
        *ret_x = x;
    if (ret_y)
        *ret_y = y;
}

void SourceViewHighlightPositionPermanently (struct Position *pos, int color_num)
{
    int line = pos->line;
    int col = pos->column;
    char *filename = pos->filename;

    int sourceTextNum = getSourceTextNumForFile (filename, TRUE);

    if (sourceTextNum < 0)
        return;
    GtkText *sourceText = sourceTexts[sourceTextNum];

    if (!sourceText)
        return;

/*
    GtkTextFont *current_font = sourceText->current_font;
    GdkFont *font = *(GdkFont **) current_font;
    double h = (double) (font->ascent + font->descent);
    double y = h * (line - 1) + (h / 2);
*/
    char *ptr = file_buf[sourceTextNum];
    int i;

    for (i = 1; i < line; i++)
    {
        if (*ptr == 0)
            return;

        while (*ptr != '\n')
            ptr++;
        ptr++;
    }

//    double x = (double) gdk_text_width (font, ptr, col - 1);
    int length = 0;
    char c;

    do
    {
        length++;
        c = ptr[col - 1 + length];
    }
    while (c != ' ' && c != '\t' && c != '\n' && c != '\0' && c != '(' && c != ')' && c != ',' && c != ';');
//    double w = (double) gdk_text_width (font, ptr + col - 1, length);

    GtkText *text = sourceText;

    gtk_text_freeze (text);

    ptr += col - 1;
    gtk_text_set_point (text, ptr - file_buf[sourceTextNum]);
    gtk_text_forward_delete (text, length);

    GdkColor *fore, *back;

    {
        static int gdk_colours_initialised = FALSE;
        static GdkColor color[3];

        if (!gdk_colours_initialised)
        {
            gdk_colours_initialised = TRUE;
            GdkColormap *colours = gdk_colormap_get_system (); //gtk_widget_get_colormap (MessagesText);

            color[0].red = 0xffff;
            color[0].blue = 0xe000;
            color[0].green = 0xe000;
            if (!gdk_color_alloc (colours, &color[0]))
                g_error ("couldn't allocate colour");
            color[1].red = 0xe000;
            color[1].blue = 0xe000;
            color[1].green = 0xffff;
            if (!gdk_color_alloc (colours, &color[1]))
                g_error ("couldn't allocate colour");
            color[2].red = 0xe000;
            color[2].blue = 0xffff;
            color[2].green = 0xe000;
            if (!gdk_color_alloc (colours, &color[2]))
                g_error ("couldn't allocate colour");
        }
        fore = NULL;
        if (color_num >= 0 && color_num < 3)
            back = &color[color_num];
        else
            back = &color[0];
    }
    gtk_text_insert (text, NULL, fore, back, ptr, length);
    gtk_text_thaw (text);
}

static struct
{
    GtkCTreeNode *ctreeNode;
    int parent_num;
    GPtrArray *trace;
    struct Position *pos;
    char *valueStr;

    double tmp_x, tmp_y;
}
threads[10000];
static int num_last_thread = 0;

/**********************************************/

void DrawLineOverSourceTexts (GdkGC * gc, double x1, double y1, double x2, double y2)
{
    int i;

    for (i = 0; i < nb_open_files; i++)
    {
        double dx = GTK_WIDGET (sourceTexts[i])->allocation.x;
        double dy = GTK_WIDGET (sourceTexts[i])->allocation.y;

        gdk_draw_line (sourceTexts[i]->text_area, gc, x1 - dx, y1 - dy, x2 - dx, y2 - dy);
    }
}

void DrawBreakpointSign (struct Breakpoint *bp)
{
    int sourceTextNum = getSourceTextNumForFile (bp->filename, FALSE);
    GtkText *sourceText = sourceTexts[sourceTextNum];
    GtkTextFont *current_font = sourceText->current_font;
    GdkFont *font = *(GdkFont **) current_font;
    double h = (double) (font->ascent + font->descent);

    double y = (bp->line - 1) * h;

    // scroll bar adjustment
    y -= sourceText->vadj->value;

    gdk_draw_line (sourceText->text_area, GetColorGC (1), 2, y + h / 2, 2 + h, y + h / 2);
    gdk_draw_line (sourceText->text_area, GetColorGC (1), 2 + h / 2, y, 2 + h / 2, y + h);
    gdk_draw_line (sourceText->text_area, GetColorGC (1), 2, y, 2 + h, y + h);
    gdk_draw_line (sourceText->text_area, GetColorGC (1), 2 + h, y, 2, y + h);
}

void RedrawSourceView (void)
{
    if (IsSourceViewClosed ())
        return;

    GtkWidget *SourceViewHBox = GTK_WIDGET (gtk_object_get_data (GTK_OBJECT (SourceViewWindow), "SourceViewHBox"));

    gtk_widget_draw (SourceViewHBox, NULL);

    if (gtk_toggle_button_get_active (ShowThreadSourceCheckbutton))
    {
        // Draw thread highlights and lines
        int i;

        for (i = 1; i <= num_last_thread; i++)
        {
            if (threads[i].pos)
            {
                SourceViewHighlightPosition (threads[i].pos, &threads[i].tmp_x, &threads[i].tmp_y, 2, threads[i].valueStr, FALSE);
                int parent = threads[i].parent_num;

                if (parent > 0 && !gtk_toggle_button_get_active (NotebookStyleCheckbutton))
                    DrawLineOverSourceTexts (GetColorGC (4), threads[i].tmp_x, threads[i].tmp_y, threads[parent].tmp_x, threads[parent].tmp_y);
            }
        }
    }
    // Draw breakpoints
    GList *tmp;

    for (tmp = breakpoints; tmp; tmp = tmp->next)
    {
        struct Breakpoint *bp = tmp->data;

        DrawBreakpointSign (bp);
    }
}

static char *ShowAllChansPositions_Filename = NULL;
gboolean ShowAllChansPositionsButton_subfct (struct Edge *edge)
{
    struct Chan *chan = ((PtrEdgeInfo) edge->data)->chan;

    if (!chan)
        return TRUE;
    struct BreezePartChannelsChannel *channel = chan->channel;

    if (!channel)
        return TRUE;
    struct Position *pos = getBreezePartChannelsChannelPosition (channel);

    if (!pos)
        return TRUE;
    int color_num = chan->direction;

    if (ShowAllChansPositions_Filename != NULL)
    {
        char *filename = pos->filename;

        if (strcmp (filename, ShowAllChansPositions_Filename) != 0)
            return TRUE;
    }

    SourceViewHighlightPositionPermanently (pos, color_num);

    // Try to find a Variable component
    if (chan)
    {
        struct Comp *comp = chan->dest;

        if (comp)
        {
            struct BreezePartComponentsComponent *component = comp->component;

            if (component)
            {
                char *compname = getBreezePartComponentsComponentName (component);

                if (!strcmp (compname, "$BrzVariable"))
                {
                    pos = getBreezePartComponentsComponentPosition (component);
                    if (pos)
                        SourceViewHighlightPositionPermanently (pos, 0);
                }
            }
        }
    }

    return TRUE;
}

void on_ShowAllChansPositionsButton_clicked (GtkButton * button, gpointer user_data)
{
    Structure_GetRootVertex (); // init structure

    ShowAllChansPositions_Filename = NULL;
    ForEachEdge (ShowAllChansPositionsButton_subfct);
}

void ShowAllChansPositionsForFile (char *filename)
{
    Structure_GetRootVertex (); // init structure

    ShowAllChansPositions_Filename = filename;
    ForEachEdge (ShowAllChansPositionsButton_subfct);
}

void BreakAtDataActivity_ActivityDetected (struct Position *pos)
{
    static struct Position lastBreakedPos;

    // Check if the button "Break at any data activity" is activated
    GtkObject *checkbutton = gtk_object_get_data (GTK_OBJECT (SourceViewWindow),
      "BreakAtAnyDataActivityCheckButton");

    if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (checkbutton)))
        goto breakpoint_reached;

    // Check if a breakpoint has been reached
    GList *tmp;

    for (tmp = breakpoints; tmp; tmp = tmp->next)
    {
        struct Breakpoint *bp = tmp->data;

        if (!strcmp (bp->filename, pos->filename) && bp->line == pos->line)
            goto breakpoint_reached;
    }

    return;

  breakpoint_reached:
    if (pos->line == lastBreakedPos.line && pos->column == lastBreakedPos.column)
        return;
    lastBreakedPos = *pos;
    {
        GtkToggleButton *animateButton = GTK_TOGGLE_BUTTON (gtk_object_get_data (GTK_OBJECT (MainWindow2), "AnimateButton"));

        gtk_toggle_button_set_active (animateButton, FALSE);
    }
}

gboolean OnSourceTextMousePressed (GtkWidget * widget, GdkEventButton * event, gpointer user_data)
{
    if (event->button != 3)
        return FALSE;

    // find filename
    GtkText *sourceText = GTK_TEXT (widget);
    int sourceTextNum;

    for (sourceTextNum = 0; sourceTextNum < nb_open_files; sourceTextNum++)
    {
        if (sourceText == sourceTexts[sourceTextNum])
            break;
    }
    if (sourceTextNum == nb_open_files)
    {
        printf ("error 1 in OnSourceTextMousePressed\n");
        return FALSE;
    }

    menuPopup_filename = open_filenames[sourceTextNum];

    // scroll bar adjustments
    float x = event->x + sourceText->hadj->value;
    float y = event->y + sourceText->vadj->value;

    // find line
    GtkTextFont *current_font = sourceText->current_font;
    GdkFont *font = *(GdkFont **) current_font;
    double h = (double) (font->ascent + font->descent);
    int line = (y / h) + 1;

    menuPopup_line = line;

    // find column
    char *ptr = file_buf[sourceTextNum];
    int i;

    for (i = 1; i < line; i++)
    {
        while (*ptr != '\n')
            ptr++;
        ptr++;
    }
    menuPopup_column = 1;
    while (ptr[menuPopup_column - 1] != '\n')
    {
        if (gdk_text_width (font, ptr, menuPopup_column) > x)
            break;
        menuPopup_column++;
    }

    DEBUG_printf ("menuPopup click: x=%f y=%f, h=%f => line=%d column=%d file=%s\n", x, y, h, menuPopup_line, menuPopup_column, menuPopup_filename);

    GtkWidget *menu = (GtkWidget *) gtk_object_get_data (GTK_OBJECT (SourceViewWindow),
      "rightclickdropdownmenu_menu");

    gtk_menu_popup (GTK_MENU (menu), NULL, NULL, NULL, NULL, 0, 0);

    return FALSE;
}

void on_source_toggle_breakpoint_activate (GtkMenuItem * menuitem, gpointer user_data)
{
    GtkCList *breakpointsCList = GTK_CLIST (gtk_object_get_data (GTK_OBJECT (SourceViewWindow), "BreakpointsCList"));

    // check if existing breakpoint
    GList *tmp;
    int row = 0;

    for (tmp = breakpoints; tmp; tmp = tmp->next, row++)
    {
        struct Breakpoint *bp = tmp->data;

        if (!strcmp (menuPopup_filename, bp->filename) && menuPopup_line == bp->line)
        {
            // breakpoint found => remove it
            breakpoints = g_list_remove_link (breakpoints, tmp);

            // Remove from clist
            gtk_clist_remove (breakpointsCList, row);
            goto refresh;
        }
    }

    // breakpoint not found => add new one
    struct Breakpoint *bp = g_new0 (struct Breakpoint, 1);

    bp->filename = menuPopup_filename;
    bp->line = menuPopup_line;
    breakpoints = g_list_append (breakpoints, bp);

    // Add to clist
    char *text[3];

    text[0] = bp->filename;
    text[1] = g_strdup_printf ("%d", bp->line);
    text[2] = "";
    gtk_clist_append (breakpointsCList, text);
    g_free (text[1]);

  refresh:
    RedrawSourceView ();
}

static GList *on_source_select_channels_activate_ClosestEdgeList = NULL;

//static char *on_source_select_channels_activate_ClosestEdge_Filename = NULL;
static int on_source_select_channels_activate_ClosestEdge_line = 0;
static int on_source_select_channels_activate_ClosestEdge_column = 0;
gboolean on_source_select_channels_activate_FindClosestEdgeList (struct Edge *edge)
{
    struct Chan *chan = ((PtrEdgeInfo) edge->data)->chan;

    if (!chan)
        return TRUE;
    struct BreezePartChannelsChannel *channel = chan->channel;
    struct Position *pos = getBreezePartChannelsChannelPosition (channel);

    // Check for correct filename
    if (pos && pos->filename && strcmp (pos->filename, menuPopup_filename) != 0)
        return TRUE;

    // Look for better position
    if (pos->line > menuPopup_line)
        return TRUE;
    if (pos->line == menuPopup_line && pos->column > menuPopup_column)
        return TRUE;

    if (pos->line < on_source_select_channels_activate_ClosestEdge_line)
        return TRUE;
    if (pos->line == on_source_select_channels_activate_ClosestEdge_line && pos->column < on_source_select_channels_activate_ClosestEdge_column)
        return TRUE;

    if (!(pos->line == on_source_select_channels_activate_ClosestEdge_line && pos->column == on_source_select_channels_activate_ClosestEdge_column))
    {
        g_list_free (on_source_select_channels_activate_ClosestEdgeList);
        on_source_select_channels_activate_ClosestEdgeList = NULL;
        on_source_select_channels_activate_ClosestEdge_line = pos->line;
        on_source_select_channels_activate_ClosestEdge_column = pos->column;
    }

    on_source_select_channels_activate_ClosestEdgeList = g_list_prepend (on_source_select_channels_activate_ClosestEdgeList, edge);
    return TRUE;
}

void on_source_select_channels_activate (GtkMenuItem * menuitem, gpointer user_data)
{
    Core_ClearSelection ();

    on_source_select_channels_activate_ClosestEdge_line = 0;
    on_source_select_channels_activate_ClosestEdge_column = 0;
    ForEachEdge (on_source_select_channels_activate_FindClosestEdgeList);

    if (!on_source_select_channels_activate_ClosestEdge_line)
        return;

    GList *tmp = on_source_select_channels_activate_ClosestEdgeList;

    for (; tmp; tmp = tmp->next)
    {
        struct Edge *edge = tmp->data;
        struct Chan *chan = ((PtrEdgeInfo) edge->data)->chan;

        Core_SelectChannel (chan);
    }
}

void on_GotoSelectedChannelSourceButton_clicked (GtkButton * button, gpointer user_data)
{
    if (SelectedChannelPosition)
    {
        RedrawSourceView ();
        SourceViewHighlightPosition (SelectedChannelPosition, NULL, NULL, 1 /*red */ , NULL, TRUE);
    }
}

void on_NotebookStyleCheckbutton_toggled (GtkToggleButton * togglebutton, gpointer user_data)
{
    GtkWidget *SourceViewHBox = GTK_WIDGET (gtk_object_get_data (GTK_OBJECT (SourceViewWindow), "SourceViewHBox"));
    GtkWidget *SourceViewNotebook = GTK_WIDGET (gtk_object_get_data (GTK_OBJECT (SourceViewWindow), "SourceViewNotebook"));
    gboolean NotebookActive = gtk_toggle_button_get_active (togglebutton);
    int i;

    for (i = 0; i < nb_open_files; i++)
    {
        GtkWidget *SourceText = GTK_WIDGET (sourceTexts[i]);
        GtkWidget *SourceTextScrolledWindow = GTK_WIDGET (SourceText->parent);

        gtk_widget_ref (SourceTextScrolledWindow);
        gtk_container_remove (GTK_CONTAINER (SourceTextScrolledWindow->parent), SourceTextScrolledWindow);

        if (NotebookActive)
        {
            char *filename = gtk_object_get_data (GTK_OBJECT (SourceTextScrolledWindow),
              "filename");
            GtkWidget *newLabel = gtk_label_new (filename);

            gtk_notebook_append_page (GTK_NOTEBOOK (SourceViewNotebook), SourceTextScrolledWindow, newLabel);
        } else
            gtk_container_add (GTK_CONTAINER (SourceViewHBox), SourceTextScrolledWindow);

        gtk_widget_unref (SourceTextScrolledWindow);
    }

    if (NotebookActive)
    {
        gtk_widget_hide (SourceViewHBox);
        gtk_widget_show (SourceViewNotebook);
    } else
    {
        gtk_widget_show (SourceViewHBox);
        gtk_widget_hide (SourceViewNotebook);
    }
}

/***************************/

void SourceView_SimTraceTimeChanged_subfct (struct ControlThread *thread)
{
    DEBUG_printf ("  Thread %d, parent=%d, position=%s, data=%s\n", thread->num,
      thread->parentNum, thread->position ? strdup_printPosition (thread->position) : "(null)", thread->data);

    if (thread->position == NULL)
        return;

    GtkWidget *tree = GTK_WIDGET (gtk_object_get_data (GTK_OBJECT (SourceViewWindow), "ThreadsCTree"));
    GtkCTreeNode *node;
    GtkCTreeNode *parentNode;

    if (thread->parentNum > 0)
        parentNode = threads[thread->parentNum].ctreeNode;
    else
        parentNode = NULL;

    gchar *text[3];
    char buf[10];

    sprintf (buf, "%d", thread->num);
    text[0] = buf;
    text[1] = g_strdup_printf ("%d:%d %s", thread->position->line, thread->position->column, thread->position->filename);
    text[2] = thread->data ? : "";

    node = gtk_ctree_insert_node (GTK_CTREE (tree), parentNode, NULL, text, 0, NULL, NULL, NULL, NULL, FALSE, TRUE);
    threads[thread->num].ctreeNode = node;

    threads[thread->num].parent_num = thread->parentNum;
    //    threads[thread->num].trace = g_ptr_array_new ();
    threads[thread->num].pos = thread->position;
    threads[thread->num].valueStr = thread->data;

    if (thread->num > num_last_thread)
        num_last_thread = thread->num;
}

/***************************/

void SourceView_SimTraceTimeChanged (int length)
{
    DEBUG_printf ("SourceView_SimTraceTimeChanged (%d)\n", length);

    if (gtk_toggle_button_get_active (ActivateThreadsCheckbutton))
    {
        GtkWidget *tree = GTK_WIDGET (gtk_object_get_data (GTK_OBJECT (SourceViewWindow), "ThreadsCTree"));

        gtk_clist_clear (GTK_CLIST (tree));
        num_last_thread = 0;
        Core_Simtrace_ForEachThread (SourceView_SimTraceTimeChanged_subfct);
    }

    RedrawSourceView ();
}

void SourceView_SelectChannel (struct Chan *chan)
{
    DEBUG_printf ("SourceView_SelectChannel\n");

    struct Position *pos = getBreezePartChannelsChannelPosition (chan->channel);

    SelectedChannelPosition = pos;

    GtkEntry *SelectedChannelEntry = GTK_ENTRY (gtk_object_get_data (GTK_OBJECT (SourceViewWindow), "SelectedChannelEntry"));
    char *text = strdup_printPosition (pos);

    gtk_entry_set_text (SelectedChannelEntry, text);
    g_free (text);

    /* check GotoSelectedChannelSource */
    GtkWidget *gotoSelectedChannelSourceCheckButton = GTK_WIDGET (gtk_object_get_data (GTK_OBJECT (SourceViewWindow),
        "GotoSelectedChannelSourceCheckButton"));

    if (GTK_TOGGLE_BUTTON (gotoSelectedChannelSourceCheckButton)->active)
    {
        on_GotoSelectedChannelSourceButton_clicked (NULL, NULL);
        on_GotoSelectedChannelSourceButton_clicked (NULL, NULL);
    }
}

void SourceView_ExpandCollapseGroup (struct Vertex *v, int expandCollapseType)
{
    DEBUG_printf ("SourceView_ExpandCollapseGroup %d\n", expandCollapseType);
}

struct ViewProperties *SourceView_Init (void)
{
    sourceViewProperties = g_new0 (struct ViewProperties, 1);

    sourceViewProperties->SelectChannel = SourceView_SelectChannel;
    sourceViewProperties->ExpandCollapseGroup = (CALLBACK_voidstar_int) SourceView_ExpandCollapseGroup;
    sourceViewProperties->SimTraceTimeChanged = SourceView_SimTraceTimeChanged;
    return sourceViewProperties;
}

void SourceView_ToggleActivate (gboolean activate)
{
    if (activate)
    {
        //        SourceView_Initialise ();
        //        SourceView_UpdateAllStates ();
    }

    sourceViewProperties->enabled = activate;
}

void on_GoNextDataButton_clicked (GtkButton * button, gpointer user_data)
{
    GtkToggleButton *graphToggleButton = GTK_TOGGLE_BUTTON (gtk_object_get_data (GTK_OBJECT (MainWindow2), "GraphToggleButton"));
    GtkObject *checkbutton = gtk_object_get_data (GTK_OBJECT (SourceViewWindow),
      "BreakAtAnyDataActivityCheckButton");
    GtkToggleButton *animateButton = GTK_TOGGLE_BUTTON (gtk_object_get_data (GTK_OBJECT (MainWindow2), "AnimateButton"));

    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (graphToggleButton), TRUE);
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (checkbutton), TRUE);
    gtk_toggle_button_set_active (animateButton, TRUE);
}
