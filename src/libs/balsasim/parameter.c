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

	`parameter.c'
	Breeze parameter handling, parsing from strings 

*/

#include "parameter.h"
#include <stdlib.h>
#include <string.h>

/* BalsaParameterParseFromString : parse a parameter in one of the formats:
	"-?[0-9]+", "\"[^"]*\"" or "(breeze-type-decl)" from the given string */
BalsaParameter *BalsaParameterParseFromString (const char *string)
{
    BalsaParameter *ret = NULL;

    if (string)
    {
        ret = malloc (sizeof (BalsaParameter));

        switch (*string)
        {
        case '\"':             /* string */
            string++;
            ret->nature = BalsaStringParameter;
            {
                unsigned stringLength = strlen (string);
                char *trailingQuote = strchr (string, '\"');

                if (trailingQuote)
                    stringLength--;

                ret->info.string = malloc (stringLength + 1);
                strncpy (ret->info.string, string, stringLength);
                ret->info.string[stringLength] = '\0';
            }
            break;
        case '(':              /* type */
            ret->nature = BalsaTypeParameter;
            ret->info.type = NULL;
            BalsaTypeParseFromString (string, 0, &(ret->info.type));
            break;
        default:               /* number */
            if (string[1] && string[1] == '-')
            {
                ret->info.number.isNegated = true;
                string++;
            } else
                ret->info.number.isNegated = false;

            ret->nature = BalsaNumberParameter;
            ret->info.number.number = FormatDataParseUInt (string, 10);
            break;
        }
    }
    return ret;
}
