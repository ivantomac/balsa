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

	`technology.h'
	BALSATECH handling

*/

#ifndef TECHNOLOGY_HEADER
#define TECHNOLOGY_HEADER

#include <gtk/gtk.h>

typedef enum BalsaStyleOptionType_
{
    BalsaStyleOption_Number,
    BalsaStyleOption_String,
    BalsaStyleOption_Boolean,
    BalsaStyleOption_Enumeration
}
BalsaStyleOptionType;

struct BalsaTechnology_;

typedef struct BalsaStyleOption_
{
    char *name;
    char *description;
    BalsaStyleOptionType type;
    union
    {
        struct
        {
            GList *names;
            GList *descriptions;
        }
        enumeration;
    }
    options;
}
BalsaStyleOption, *PtrBalsaStyleOption;

typedef struct BalsaStyle_
{
    char *name;
    char *description;
    GList *allowedStyleOptions;
    GList *styleOptions;        /* specific style options */
}
BalsaStyle, *PtrBalsaStyle;

typedef struct BalsaTechnology_
{
    char *name;
    char *description;
    char *balsaNetlistOptions;  /* default balsa-netlist command line */
    GList *styles;              /* list of PtrBalsaStyle's */
    GList *styleOptions;        /* list of PtrBalsaStyleOption's */
}
BalsaTechnology, *PtrBalsaTechnology;

extern GList *BalsaTechnologies;

/* FindBalsaTechnologies: find all the available technologies and populate BalsaTechnologies */
extern gboolean FindBalsaTechnologies (char *techDirectory);

/* PopulateTechnologyChooser: populate the Glade generated framework
	for the BALSATECH technology chooser window */
extern void PopulateTechnologyChooser (GtkWidget * topLevel);

/* ParseBALSATECHValue: parse a BALSATECH string and set up the
	technology chooser appropriately */
void ParseBALSATECHValue (char *balsatech, GtkWidget * technologyChooserWindow);

#endif
