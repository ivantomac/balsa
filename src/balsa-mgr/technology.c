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

	`technology.c'
	BALSATECH handling

*/

#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>

#include "technology.h"
#include <breeze/lparse.h>
#include "main.h"
#include "project.h"
#include "utils.h"
#include "support.h"

GList *BalsaTechnologies = NULL;

/* ParseBalsaStyleOption: parse a (style-option ...) node */
static PtrBalsaStyleOption ParseBalsaStyleOption (PtrTMPNode styleOption, PtrBalsaTechnology tech)
{
    PtrBalsaStyleOption ret;
    GList *contentsList;
    int contentsListLength;

    if (!TMPIsHeaded (styleOption, "style-option"))
    {
        printfConsole ("invalid element in style-options list, skipping\n");
        return NULL;
    }
    ret = g_new (BalsaStyleOption, 1);

    contentsList = g_list_next (styleOption->body.list);
    ret->name = g_strdup (TMP_NODE (contentsList->data)->body.string);
    ret->description = "";
    ret->type = BalsaStyleOption_Boolean;
    contentsListLength = g_list_length (contentsList);

    if (contentsListLength < 2 || contentsListLength > 3)
    {
        printfConsole ("invalid element in style-options list, skipping\n");
        return NULL;
    }

    ret->name = g_strdup (TMP_NODE (contentsList->data)->body.string);
    if (contentsListLength == 3) /* has description */
        ret->description = g_strdup (TMP_NODE (g_list_nth (contentsList, 2)->data)->body.string);

    if (TMPIsHeaded (TMP_NODE (g_list_nth (contentsList, 1)->data), NULL))
    {                           /* any head symbol */
        GList *subList = TMP_NODE (g_list_nth (contentsList, 1)->data)->body.list;
        char *headSymbol = TMP_NODE (subList->data)->body.string;

        if (strcmp (headSymbol, "boolean") == 0) /* #t, #f */
            ret->type = BalsaStyleOption_Boolean;
        else if (strcmp (headSymbol, "number") == 0) /* integers */
            ret->type = BalsaStyleOption_Number;
        else if (strcmp (headSymbol, "string") == 0) /* string */
            ret->type = BalsaStyleOption_String;
        else if (strcmp (headSymbol, "enumeration") == 0) /* choice of string */
        {
            ret->type = BalsaStyleOption_Enumeration;
            ret->options.enumeration.names = NULL;
            ret->options.enumeration.descriptions = NULL;

            subList = g_list_next (subList);
            while (subList)
            {
                if (TMP_NODE (subList->data)->type == TMPList)
                {
                    GList *subSubList = TMP_NODE (subList->data)->body.list;

                    ret->options.enumeration.names =
                      g_list_append (ret->options.enumeration.names, g_strdup (TMP_NODE (subSubList->data)->body.string));
                    ret->options.enumeration.descriptions =
                      g_list_append (ret->options.enumeration.descriptions,
                      (subSubList->next ? g_strdup (TMP_NODE (g_list_next (subSubList)->data)->body.string) : ""));
                } else
                {
                    printfConsole ("invalid enumeration element `");
                    PrintTMPNode (stderr, TMP_NODE (contentsList->data));
                    printfConsole ("\n");
                    return NULL;
                }
                subList = g_list_next (subList);
            }
        } else
        {
            printfConsole_s ("invalid style-option type `%s'", headSymbol);
            printfConsole_s ("in style-option `%s', skipping\n", ret->name);
            return NULL;
        }
    }

    return ret;
}

/* ParseBalsaStyle: parse a (style ...) node */
static PtrBalsaStyle ParseBalsaStyle (PtrTMPNode style, PtrBalsaTechnology tech)
{
    PtrBalsaStyle ret;
    GList *contentsList;

    if (!TMPIsHeaded (style, "style"))
    {
        printfConsole ("invalid element in styles list, skipping\n");
        return NULL;
    }
    ret = g_new (BalsaStyle, 1);

    contentsList = g_list_next (style->body.list);

    ret->name = g_strdup (TMP_NODE (contentsList->data)->body.string);
    ret->description = "";
    ret->allowedStyleOptions = NULL;
    ret->styleOptions = NULL;

    contentsList = g_list_next (contentsList);
    while (contentsList)
    {
        if (TMPIsHeaded (TMP_NODE (contentsList->data), NULL))
        {                       /* any head symbol */
            GList *subList = TMP_NODE (contentsList->data)->body.list;
            char *headSymbol = TMP_NODE (subList->data)->body.string;

            if (strcmp (headSymbol, "description") == 0)
            {
                ret->description = g_strdup (TMP_NODE (g_list_next (subList)->data)->body.string);
            } else if (strcmp (headSymbol, "allowed-style-options") == 0)
            {
                subList = g_list_next (subList);
                while (subList)
                {
                    if (TMP_NODE (subList->data)->type == TMPString)
                    {
                        ret->allowedStyleOptions = g_list_append (ret->allowedStyleOptions, g_strdup (TMP_NODE (subList->data)->body.string));
                    }
                    subList = g_list_next (subList);
                }
            } else if (strcmp (headSymbol, "style-options") == 0)
            {
                subList = g_list_next (subList);
                while (subList)
                {
                    PtrBalsaStyleOption styleOption = ParseBalsaStyleOption (TMP_NODE (subList->data), tech);

                    if (styleOption)
                        ret->styleOptions = g_list_append (ret->styleOptions, styleOption);
                    subList = g_list_next (subList);
                }
            } else
            {
                printfConsole_s ("unknown tech. file style entry `%s', skipping\n", headSymbol);
            }
        } else
        {
            printfConsole ("invalid style entry: `");
            PrintTMPNode (stderr, TMP_NODE (contentsList->data));
            printfConsole ("'\n");
            return NULL;
        }
        contentsList = g_list_next (contentsList);
    }

    return ret;
}

/* ParseBalsaTechFile: parse a tech. file and populate a single BalsaTechnology */
static PtrBalsaTechnology ParseBalsaTechFile (PtrTMPNode contents)
{
    PtrBalsaTechnology ret = g_new (BalsaTechnology, 1);
    GList *contentsList = g_list_next (contents->body.list); /* (balsa-mgr-technology ... -> ("name" ... */

    ret->name = g_strdup (TMP_NODE (contentsList->data)->body.string);
    ret->description = "";
    ret->styles = NULL;
    ret->styleOptions = NULL;
    ret->balsaNetlistOptions = NULL;

    contentsList = g_list_next (contentsList);
    while (contentsList)
    {
        if (TMPIsHeaded (TMP_NODE (contentsList->data), NULL))
        {                       /* any head symbol */
            GList *subList = TMP_NODE (contentsList->data)->body.list;
            char *headSymbol = TMP_NODE (subList->data)->body.string;

            if (strcmp (headSymbol, "description") == 0)
            {
                ret->description = g_strdup (TMP_NODE (g_list_next (subList)->data)->body.string);
            } else if (strcmp (headSymbol, "styles") == 0)
            {
                subList = g_list_next (subList);
                while (subList)
                {
                    PtrBalsaStyle style = ParseBalsaStyle (TMP_NODE (subList->data), ret);

                    if (style)
                        ret->styles = g_list_append (ret->styles, style);
                    subList = g_list_next (subList);
                }
            } else if (strcmp (headSymbol, "style-options") == 0)
            {
                subList = g_list_next (subList);
                while (subList)
                {
                    PtrBalsaStyleOption styleOption = ParseBalsaStyleOption (TMP_NODE (subList->data), ret);

                    if (styleOption)
                        ret->styleOptions = g_list_append (ret->styleOptions, styleOption);
                    subList = g_list_next (subList);
                }
            } else if (strcmp (headSymbol, "balsa-netlist-options") == 0)
            {
                ret->balsaNetlistOptions = g_strdup (TMP_NODE (g_list_next (subList)->data)->body.string);
            } else
            {
                printfConsole_s ("unknown tech. file entry `%s', skipping\n", headSymbol);
            }
        } else
        {
            printfConsole ("invalid balsa-mgr-technology entry: `");
            PrintTMPNode (stderr, TMP_NODE (contentsList->data));
            printfConsole ("'\n");
            return NULL;
        }
        contentsList = g_list_next (contentsList);
    }

    return ret;
}

/* ReadBalsaTechFile: open and check the (balsa-mgr-tech ...)'ness of a balsa-mgr tech file */
static PtrTMPNode ReadBalsaTechFile (char *filename)
{
    PtrTMPNode contents = ParseCompleteFileAsTMPNode (filename);

    if (contents)
    {
        if (!TMPIsHeaded (contents, "balsa-mgr-technology"))
        {
            printfConsole_s ("`%s' is not a valid balsa-mgr technology file\n", filename);
            contents = NULL;
        }
    }
    return contents;
}

/* TechnolgyNameCompare: ordering function for technologies, based on ASCII order of names */
int TechnologyNameCompare (PtrBalsaTechnology t1, PtrBalsaTechnology t2)
{
    return (t1 && t2 ? strcmp (t1->name, t2->name) : 0);
}

/* FindBalsaTechnologies: find all the available technologies and populate BalsaTechnologies */
gboolean FindBalsaTechnologies (char *techDirectory)
{
    DIR *directory = opendir (techDirectory);
    struct dirent *dirEntry;
    int techDirNameLength = strlen (techDirectory);

    char *balsaMgrConfigFilename = "balsa-mgr.cfg";
    int balsaMgrConfigFilenameLength = strlen (balsaMgrConfigFilename);

    if (!directory)
    {
        printfConsole_s ("cannot open tech directory `%s'\n", techDirectory);
        return FALSE;
    }

    do
    {
        dirEntry = readdir (directory);

        if (dirEntry && dirEntry->d_name[0] != '.') /* don't process dot files */
        {
            int dirNameLength = techDirNameLength + strlen (dirEntry->d_name) + 1;
            char *dirName = g_new (char, dirNameLength + 1);
            struct stat statBuffer;

            sprintf (dirName, "%s/%s", techDirectory, dirEntry->d_name);

            /* techDir/dirEntry is a directory and techDir/dirEntry/balsa-mgr.cfg is a
               readable file */
            if (stat (dirName, &statBuffer) == 0 && S_ISDIR (statBuffer.st_mode))
            {
                char *configFilename = g_new (char,
                  dirNameLength + balsaMgrConfigFilenameLength + 2);

                sprintf (configFilename, "%s/%s", dirName, balsaMgrConfigFilename);
                stat (configFilename, &statBuffer);

                if (stat (configFilename, &statBuffer) == 0 && S_ISREG (statBuffer.st_mode))
                {
                    PtrTMPNode contents = ReadBalsaTechFile (configFilename);

                    if (contents)
                    {
                        PtrBalsaTechnology tech = ParseBalsaTechFile (contents);

                        if (tech)
                        {
                            BalsaTechnologies = g_list_insert_sorted (BalsaTechnologies, (gpointer) tech, (GCompareFunc) TechnologyNameCompare);
                        }
                        DeleteTMPNode (contents);
                    }
                }
                g_free (configFilename);
            }
            g_free (dirName);
        }
    }
    while (dirEntry);

    if (directory)
        closedir (directory);

    return TRUE;
}

/* FindStyleOptionInStyleOptionList: find a style option entry by name in a list of style options */
static PtrBalsaStyleOption FindStyleOptionInStyleOptionList (GList * styleOptions, char *styleOptionName)
{
    while (styleOptions)
    {
        if (strcmp (((PtrBalsaStyle) styleOptions->data)->name, styleOptionName) == 0)
            return (PtrBalsaStyleOption) styleOptions->data;
        styleOptions = g_list_next (styleOptions);
    }

    return NULL;
}

/* FindStyleOptionInStyleOrTech : find a style option entry in the given style, or
	if that style doesn't have that specific option, in the technology's style options */
static PtrBalsaStyleOption FindStyleOptionInStyleOrTech (PtrBalsaStyle style, PtrBalsaTechnology tech, char *styleOptionName)
{
    PtrBalsaStyleOption ret = FindStyleOptionInStyleOptionList (style->styleOptions, styleOptionName);

    if (!ret)
    {
        ret = FindStyleOptionInStyleOptionList (tech->styleOptions, styleOptionName);
    }

    return ret;
}

/* MakeStyleOptionRowActive: set the little tick in column 0 of a row and set the
	row data to TRUE if active is TRUE, otherwise clear the pixmap and set the
	row data to FALSE */
static void MakeStyleOptionRowActive (GtkCList * clist, int row, gboolean active)
{
    if (active)
        gtk_clist_set_pixmap (clist, row, 0, TickPixmap, TickMask);
    else
        gtk_clist_set_text (clist, row, 0, "");

    gtk_clist_set_row_data (clist, row, GINT_TO_POINTER (active));
}

static void SelectTechnology (GtkMenuItem * menuItem, PtrBalsaTechnology tech);
static void SelectStyle (GtkMenuItem * menuItem, PtrBalsaStyle style);

/* UpdateBALSATECHEntry: update the BALSATECH variable */
static void UpdateBALSATECHEntry (GtkWidget * technologyChooserWindow)
{
    GtkWidget *BALSATECHEntry = LookupWidget (technologyChooserWindow,
        "BALSATECHEntry");
    GtkCList *styleOptionCList = GTK_CLIST (LookupWidget (technologyChooserWindow, "StyleOptionsCList"));
    PtrBalsaTechnology selectedTechnology = (PtrBalsaTechnology) gtk_object_get_data
    	(GTK_OBJECT (technologyChooserWindow), "SelectedTechnology");
    PtrBalsaStyle selectedStyle = (PtrBalsaStyle) gtk_object_get_data
    	(GTK_OBJECT (technologyChooserWindow), "SelectedStyle");

    char balsatech[1024];
    int row;
    GtkCList *clist = GTK_CLIST (styleOptionCList);
    gboolean hadAtLeastOneStyleOption = FALSE;

    /* Build up the BALSATECH string */
    sprintf (balsatech, "%s/%s/", selectedTechnology->name, selectedStyle->name);

    for (row = 0; row < clist->rows; row++)
    {
        if ((gboolean) GPOINTER_TO_INT (gtk_clist_get_row_data (clist, row))) /* enabled option */
        {
            char *optionName, *optionValue;
            PtrBalsaStyleOption option;

            if (hadAtLeastOneStyleOption)
                strcat (balsatech, ":");
            hadAtLeastOneStyleOption = TRUE;

            gtk_clist_get_text (clist, row, 1, &optionName);
            option = FindStyleOptionInStyleOrTech (selectedStyle, selectedTechnology, optionName);

            strcat (balsatech, option->name);
            switch (option->type)
            {
            case BalsaStyleOption_Boolean:
                break;
            case BalsaStyleOption_Enumeration:
            case BalsaStyleOption_Number:
            case BalsaStyleOption_String:
                gtk_clist_get_text (clist, row, 2, &optionValue);
                strcat (balsatech, "=");
                strcat (balsatech, optionValue);
                break;
            }
        }
    }
    gtk_entry_set_text (GTK_ENTRY (BALSATECHEntry), balsatech);
}

/* StyleOptionValueChanged: callback while typing style option value to
	copy value to table */
void OnStyleOptionValueChanged (GtkEditable * editable, void * _unused_)
{
	GtkWidget *technologyChooserWindow = LookupWidget (GTK_WIDGET (editable), "ImplementationOptionsDialogue");
    GtkWidget *styleOptionValueEntry = LookupWidget (technologyChooserWindow, "StyleOptionValueEntry");
    GtkWidget *styleOptionValueMenu = LookupWidget (technologyChooserWindow, "StyleOptionValueMenu");
    GtkCList *styleOptionCList = GTK_CLIST (LookupWidget (technologyChooserWindow, "StyleOptionsCList"));
    int selectedStyleOptionRow = GPOINTER_TO_INT (gtk_object_get_data
    	(GTK_OBJECT (technologyChooserWindow), "SelectedStyleOptionRow"));

    if (GTK_OBJECT_TYPE (editable) == GTK_TYPE_ENTRY)
    {                           /* Handle entry value changers */
        gtk_clist_set_text (styleOptionCList, selectedStyleOptionRow, 2,
        	gtk_entry_get_text (GTK_ENTRY (styleOptionValueEntry)));
    } else
    {
		GtkWidget *menu = gtk_option_menu_get_menu (GTK_OPTION_MENU (styleOptionValueMenu));
		GtkWidget *active = gtk_menu_get_active (GTK_MENU (menu));

		if (active)
		{
            gtk_entry_set_text (GTK_ENTRY (styleOptionValueEntry), gtk_menu_item_get_label (GTK_MENU_ITEM (active)));
		}
    }
    MakeStyleOptionRowActive (styleOptionCList, selectedStyleOptionRow, TRUE);

    UpdateBALSATECHEntry (technologyChooserWindow);
}

static void InsertText (GtkTextBuffer * text, char *string, gboolean highlight)
{
    GtkTextIter endOfBuffer;
    gtk_text_buffer_get_end_iter (text, &endOfBuffer);
    // gtk_text_insert (text, NULL, (highlight ? &Red : NULL), NULL, string, strlen (string));
    gtk_text_buffer_insert (text, &endOfBuffer, string, -1);
}

/* UpdateTechDescription: update the GtkText describing the selected tech. */
static void UpdateTechDescription (GtkWidget * technologyChooserWindow)
{
    GtkCList *styleOptionCList = GTK_CLIST (LookupWidget (technologyChooserWindow, "StyleOptionsCList"));
    GtkWidget *technologyDescriptionText = LookupWidget (technologyChooserWindow, "TechnologyDescriptionText");

//    GtkWidget *styleMenu = LookupWidget (technologyChooserWindow, "StyleMenu"));
    int selectedStyleOptionRow = GPOINTER_TO_INT (gtk_object_get_data
    	(GTK_OBJECT (technologyChooserWindow), "SelectedStyleOptionRow"));
    PtrBalsaTechnology selectedTechnology = (PtrBalsaTechnology) gtk_object_get_data
    	(GTK_OBJECT (technologyChooserWindow), "SelectedTechnology");
    PtrBalsaStyle selectedStyle = (PtrBalsaStyle) gtk_object_get_data
    	(GTK_OBJECT (technologyChooserWindow), "SelectedStyle");

    int row;
    GtkCList *clist = GTK_CLIST (styleOptionCList);
    GtkTextBuffer *text = gtk_text_view_get_buffer (GTK_TEXT_VIEW (technologyDescriptionText));

    /* Clear text */
    gtk_text_buffer_set_text (text, "", 0);
    /* gtk_text_set_point (text, 0); gtk_text_forward_delete (text, gtk_text_get_length (text)); */

    InsertText (text, selectedTechnology->name, TRUE);
    InsertText (text, ": ", TRUE);
    InsertText (text, selectedTechnology->description, FALSE);
    InsertText (text, "\n", FALSE);
    InsertText (text, selectedStyle->name, TRUE);
    InsertText (text, ": ", TRUE);
    InsertText (text, selectedStyle->description, FALSE);
    InsertText (text, "\n", FALSE);

    for (row = 0; row < clist->rows; row++)
    {
        if ((gboolean) GPOINTER_TO_INT (gtk_clist_get_row_data (clist, row)) || selectedStyleOptionRow == row) /* enabled option */
        {
            PtrBalsaStyleOption option;
            char *optionName;

            gtk_clist_get_text (clist, row, 1, &optionName);
            option = FindStyleOptionInStyleOrTech (selectedStyle, selectedTechnology, optionName);
            InsertText (text, option->name, TRUE);
            InsertText (text, ": ", TRUE);
            InsertText (text, option->description, FALSE);
            InsertText (text, "\n", FALSE);
        }
    }
}

/* OnSelectStyleOption: callback for style option row select */
void OnSelectStyleOption (GtkWidget * technologyChooserWindow, gint row, gint column, GdkEvent * event)
{
    GtkCList *clist = GTK_CLIST (LookupWidget (technologyChooserWindow, "StyleOptionsCList"));
    GtkWidget *nameEntry = LookupWidget (technologyChooserWindow, "StyleOptionNameEntry");
    GtkWidget *styleOptionValueEntry = LookupWidget (technologyChooserWindow, "StyleOptionValueEntry");
    GtkWidget *styleOptionValueMenu = LookupWidget (technologyChooserWindow, "StyleOptionValueMenu");
    PtrBalsaTechnology selectedTechnology = (PtrBalsaTechnology) gtk_object_get_data
    	(GTK_OBJECT (technologyChooserWindow), "SelectedTechnology");
    PtrBalsaStyle selectedStyle = (PtrBalsaStyle) gtk_object_get_data
    	(GTK_OBJECT (technologyChooserWindow), "SelectedStyle");

    char *name = 0, *value = 0;
    PtrBalsaStyleOption styleOption;
    GtkWidget *menu = gtk_menu_new ();

    /* Get the name and value from the table and fill in the name entry */
    // SelectedStyleOptionRow = row;
    gtk_object_set_data (GTK_OBJECT (technologyChooserWindow), "SelectedStyleOptionRow", GINT_TO_POINTER (row));
    gtk_clist_get_text (clist, row, 1, &name);
    gtk_clist_get_text (clist, row, 2, &value);
    gtk_entry_set_text (GTK_ENTRY (nameEntry), name);
    styleOption = FindStyleOptionInStyleOrTech (selectedStyle, selectedTechnology, name);

    /* Block entry updates so that option-menu -> entry copies don't cause two
       callback calls and the entry box can be blanked without fear of updating
       the clist */
    gtk_signal_handler_block_by_func (GTK_OBJECT (styleOptionValueEntry), (GtkSignalFunc) OnStyleOptionValueChanged, NULL);

    switch (styleOption->type)
    {
    case BalsaStyleOption_Enumeration:
        {
            GList *enumElems = styleOption->options.enumeration.names;
            int selectedItemIndex = -1;
            int index = 0;

            /* char *value = ""; */

            /* Setup enumElems menu (rebuild each time?) */
            while (enumElems)
            {
                GtkWidget *menuItem = gtk_menu_item_new_with_label ((char *) enumElems->data);

                /* if (index == 0) value = (char *) enumElems->data; */
                if (strcmp ((char *) enumElems->data, value) == 0)
                {
                    selectedItemIndex = index;
                    value = (char *) enumElems->data;
                }

                gtk_widget_show (menuItem);
                gtk_menu_append (GTK_MENU (menu), menuItem);
                gtk_signal_connect (GTK_OBJECT (menuItem), "activate", GTK_SIGNAL_FUNC (OnStyleOptionValueChanged),
                	technologyChooserWindow);
                enumElems = g_list_next (enumElems);
                index++;
            }
            if (selectedItemIndex >= 0)
            {
                gtk_menu_set_active (GTK_MENU (menu), selectedItemIndex);
            }
            gtk_entry_set_text (GTK_ENTRY (styleOptionValueEntry), value);
            gtk_widget_set_sensitive (styleOptionValueEntry, FALSE);
            gtk_widget_set_sensitive (styleOptionValueMenu, TRUE);
            break;
        }
    case BalsaStyleOption_Number:
    case BalsaStyleOption_String:
        {
            GtkWidget *menuItem = gtk_menu_item_new_with_label (" ");

            gtk_widget_show (menuItem);
            gtk_menu_append (GTK_MENU (menu), menuItem);
            gtk_entry_set_text (GTK_ENTRY (styleOptionValueEntry), value);
            gtk_widget_set_sensitive (styleOptionValueEntry, TRUE);
            gtk_widget_set_sensitive (styleOptionValueMenu, FALSE);
            break;
        }
    case BalsaStyleOption_Boolean:
        gtk_widget_set_sensitive (styleOptionValueEntry, FALSE);
        gtk_widget_set_sensitive (styleOptionValueMenu, FALSE);
        gtk_entry_set_text (GTK_ENTRY (styleOptionValueEntry), "");
        break;
    }
    gtk_option_menu_set_menu (GTK_OPTION_MENU (styleOptionValueMenu), menu);
    /* Unblock entry updates */
    gtk_signal_handler_unblock_by_func (GTK_OBJECT (styleOptionValueEntry), (GtkSignalFunc) OnStyleOptionValueChanged, NULL);

    if (column == 0)
    {
        MakeStyleOptionRowActive (clist, row, !(gboolean) GPOINTER_TO_INT (gtk_clist_get_row_data (clist, row)));
        UpdateBALSATECHEntry (technologyChooserWindow);
    }
    UpdateTechDescription (technologyChooserWindow);
}


/* ParseBALSATECHValue: parse a BALSATECH string and set up the
	technology chooser appropriately */
void ParseBALSATECHValue (char *balsatech, GtkWidget * technologyChooserWindow)
{
    GtkWidget *technologyMenu = LookupWidget (technologyChooserWindow, "TechnologyMenu");
    GtkCList *styleOptionCList = GTK_CLIST (LookupWidget (technologyChooserWindow, "StyleOptionsCList"));
    GtkWidget *styleMenu = LookupWidget (technologyChooserWindow, "StyleMenu");

    char *firstSlash, *secondSlash;
    char *techName = "", *styleName = NULL;
    PtrBalsaTechnology tech;
    PtrBalsaStyle style;

    firstSlash = strchr (balsatech, '/');
    secondSlash = (firstSlash ? strchr (firstSlash + 1, '/') : NULL);

    if (firstSlash)             /* "technology/..." */
    {
        int techLength = firstSlash - balsatech;

        techName = g_new (char, techLength + 1);

        strncpy (techName, balsatech, techLength);
        techName[techLength] = '\0';

        if (secondSlash)        /* "technology/style?/options" */
        {
            int styleLength = secondSlash - firstSlash - 1;

            if (styleLength > 0)
            {
                styleName = g_new (char, styleLength + 1);

                strncpy (styleName, firstSlash + 1, styleLength);
                styleName[styleLength] = '\0';
            }
        } else
        {                       /* "technology/style" */
            if (*(firstSlash + 1)) /* Is a char after '/' */
            {
                styleName = g_strdup (firstSlash + 1);
            }
        }
    } else
    {                           /* "technology" */
        techName = g_strdup (balsatech);
        styleName = NULL;
    }

    {                           /* Set technology */
        GList *techs = BalsaTechnologies;
        GList *menuItems = gtk_container_children (GTK_CONTAINER (GTK_OPTION_MENU (technologyMenu)->menu));
        int index = 0;

        while (techs && menuItems)
        {
            tech = (PtrBalsaTechnology) techs->data;

            if (strcmp (tech->name, techName) == 0)
            {
                gtk_option_menu_set_history (GTK_OPTION_MENU (technologyMenu), index);
                SelectTechnology (GTK_MENU_ITEM (menuItems->data), tech);
                break;
            }

            techs = g_list_next (techs);
            menuItems = g_list_next (menuItems);
            index++;
        }
        if (!techs)             /* didn't find technology */
        {
            printfConsole_s ("technology string `%s' is not valid\n", balsatech);
            return;
        }
    }

    if (styleName)
    {                           /* Set style */
        GList *styles = tech->styles;
        GList *menuItems = gtk_container_children (GTK_CONTAINER (GTK_OPTION_MENU (styleMenu)->menu));
        int index = 0;

        while (styles && menuItems)
        {
            style = (PtrBalsaStyle) styles->data;

            if (strcmp (style->name, styleName) == 0)
            {
                gtk_option_menu_set_history (GTK_OPTION_MENU (styleMenu), index);
                SelectStyle (GTK_MENU_ITEM (menuItems->data), style);
                break;
            }

            styles = g_list_next (styles);
            index++;
        }
        if (!styles)            /* didn't find technology */
            printfConsole_s ("technology string `%s' is not valid\n", balsatech);
    }

    if (secondSlash)            /* Has some options? */
    {
        char *optionPtr = secondSlash + 1;
        char *endOfOption = strchr (optionPtr, ':');

//        char *option;

        while (optionPtr && *optionPtr)
        {
            if (!endOfOption)   /* last option */
            {
                endOfOption = optionPtr;
                while (*endOfOption)
                    endOfOption++; /* set to EOS */
            }

            if (*optionPtr && *optionPtr != ':') /* not last or empty */
            {
                int optionLength = endOfOption - optionPtr;
                char *optionStr = g_new (char, optionLength + 1);
                PtrBalsaStyleOption option;
                char *valuePtr;

                /* Get the option string */
                strncpy (optionStr, optionPtr, optionLength);
                optionStr[optionLength] = '\0';
                valuePtr = strchr (optionStr, '=');
                if (valuePtr)
                {
                    *valuePtr = '\0';
                    valuePtr++;
                }
                option = FindStyleOptionInStyleOrTech (style, tech, optionStr);

                if (option)     /* found option in technology, need to find the row */
                {
                    int optionRow;

                    for (optionRow = 0; optionRow < styleOptionCList->rows; optionRow++)
                    {
                        char *optionName;

                        gtk_clist_get_text (styleOptionCList, optionRow, 1, &optionName);

                        if (strcmp (optionName, optionStr) == 0)
                        {
                            MakeStyleOptionRowActive (styleOptionCList, optionRow, TRUE);
                            gtk_clist_set_text (styleOptionCList, optionRow, 2, (valuePtr ? valuePtr : ""));
                            if (optionRow == 0)
                                OnSelectStyleOption (technologyChooserWindow, optionRow, -1, NULL);
                            UpdateBALSATECHEntry (technologyChooserWindow);
                        }
                    }
                }

                g_free (optionStr);
            }

            optionPtr = (*endOfOption ? endOfOption + 1 : NULL);
            if (optionPtr)
                endOfOption = strchr (optionPtr, ':');
        }
    }

    g_free (techName);
    if (styleName)
        g_free (styleName);
}

/* SelectStyle: handle populating technologyChooserWindow's style-options
	view after a style is selected */
static void SelectStyle (GtkMenuItem * menuItem, PtrBalsaStyle style)
{
    GtkWidget *technologyChooserWindow = gtk_object_get_data (GTK_OBJECT (menuItem), "TechnologyChooserWindow");
    PtrBalsaTechnology selectedTechnology = (PtrBalsaTechnology) gtk_object_get_data
    	(GTK_OBJECT (technologyChooserWindow), "SelectedTechnology");
    GtkCList *clist = GTK_CLIST (LookupWidget (technologyChooserWindow, "StyleOptionsCList"));

    GList *allowedStyleOptions = style->allowedStyleOptions;

    gtk_object_set_data (GTK_OBJECT (technologyChooserWindow), "SelectedStyle", (gpointer) style);

    gtk_clist_clear (clist);
    gtk_clist_set_reorderable (clist, FALSE);

    if (!allowedStyleOptions)   /* No style options, clear entry boxes */
    {
        GtkWidget *styleOptionValueEntry = LookupWidget (technologyChooserWindow,
            "StyleOptionValueEntry");
        GtkWidget *styleOptionValueMenu = LookupWidget (technologyChooserWindow,
            "StyleOptionValueMenu");
        GtkWidget *styleOptionNameEntry = LookupWidget (technologyChooserWindow,
            "StyleOptionNameEntry");

        gtk_widget_set_sensitive (styleOptionValueEntry, FALSE);
        gtk_widget_set_sensitive (styleOptionValueMenu, FALSE);
        gtk_entry_set_text (GTK_ENTRY (styleOptionValueEntry), "");
        gtk_entry_set_text (GTK_ENTRY (styleOptionNameEntry), "");
        gtk_option_menu_set_menu (GTK_OPTION_MENU (styleOptionValueMenu), gtk_menu_new ());
    }

    while (allowedStyleOptions)
    {
        PtrBalsaStyleOption styleOption = FindStyleOptionInStyleOrTech (style, selectedTechnology,
          (char *) allowedStyleOptions->data);
        char *fields[3];

        fields[0] = "";
        fields[1] = styleOption->name;

        switch (styleOption->type)
        {
        case BalsaStyleOption_Number:
        case BalsaStyleOption_String:
            fields[2] = "";     /* Consider storing this stuff somewhere? */
            break;
        case BalsaStyleOption_Boolean:
            fields[2] = "";
            break;
        case BalsaStyleOption_Enumeration:
            fields[2] = (char *) styleOption->options.enumeration.names->data;
            break;
        }
        gtk_clist_append (clist, fields);
        /* Use row data to contain activity info.  */
        gtk_clist_set_row_data (clist, clist->rows - 1, (gpointer) FALSE);

        allowedStyleOptions = g_list_next (allowedStyleOptions);
    }
    UpdateBALSATECHEntry (technologyChooserWindow);
    UpdateTechDescription (technologyChooserWindow);
}

/* SelectTechnology: handle populating technologyChooserWindow after a
	technology is selected */
static void SelectTechnology (GtkMenuItem * menuItem, PtrBalsaTechnology tech)
{
    GtkWidget *technologyChooserWindow = gtk_object_get_data (GTK_OBJECT (menuItem), "TechnologyChooserWindow");
    GtkWidget *styleMenu = LookupWidget (technologyChooserWindow, "StyleMenu");

    GList *styles = tech->styles;
    GtkWidget *menu = gtk_menu_new ();
    GtkWidget *firstMenuItem = NULL;

    // SelectedTechnology = tech;
    gtk_object_set_data (GTK_OBJECT (technologyChooserWindow), "SelectedTechnology", (gpointer) tech);

    /* Setup styles menu (rebuild each time?) */
    while (styles)
    {
        GtkWidget *menuItem = gtk_menu_item_new_with_label (((PtrBalsaTechnology) styles->data)->name);

        if (!firstMenuItem)
            firstMenuItem = menuItem;

        gtk_widget_show (menuItem);
        gtk_menu_append (GTK_MENU (menu), menuItem);
        gtk_signal_connect (GTK_OBJECT (menuItem), "activate", GTK_SIGNAL_FUNC (SelectStyle), (PtrBalsaStyle) styles->data);
        gtk_object_set_data (GTK_OBJECT (menuItem), "TechnologyChooserWindow", technologyChooserWindow);
        styles = g_list_next (styles);
    }
    gtk_option_menu_set_menu (GTK_OPTION_MENU (styleMenu), menu);

    if (firstMenuItem)
        SelectStyle (GTK_MENU_ITEM (firstMenuItem), (PtrBalsaStyle) tech->styles->data);
}

/* PopulateTechnologyChooser: populate the Glade generated framework
	for the BALSATECH technology chooser window */
void PopulateTechnologyChooser (GtkWidget * technologyChooserWindow)
{
    GtkWidget *technologyMenu = LookupWidget (technologyChooserWindow,
        "TechnologyMenu");

    GtkWidget *menu = gtk_menu_new ();
    GList *techs = BalsaTechnologies;
    GtkWidget *firstMenuItem = NULL;

    while (techs)
    {
        GtkWidget *menuItem;
        PtrBalsaTechnology tech = (PtrBalsaTechnology) techs->data;

        /* Add a menu item to the technology OptionMenu */
        menuItem = gtk_menu_item_new_with_label (tech->name);
        gtk_widget_show (menuItem);
        gtk_menu_append (GTK_MENU (menu), menuItem);
        gtk_signal_connect (GTK_OBJECT (menuItem), "activate", GTK_SIGNAL_FUNC (SelectTechnology), tech);
        gtk_object_set_data (GTK_OBJECT (menuItem), "TechnologyChooserWindow", technologyChooserWindow);
        techs = g_list_next (techs);
        if (!firstMenuItem)
            firstMenuItem = menuItem;
    }

    if (firstMenuItem)          /* Some technologies */
    {
        gtk_option_menu_set_menu (GTK_OPTION_MENU (technologyMenu), menu);
        SelectTechnology (GTK_MENU_ITEM (firstMenuItem), (PtrBalsaTechnology) BalsaTechnologies->data);
    }
}
