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

	`parameter.h'
	Breeze parameter handling, parsing from strings 

*/

#ifndef BALSA_SIM_PARAMETER_H
#define BALSA_SIM_PARAMETER_H

#include "balsasim/type.h"

typedef enum
{
    BalsaNumberParameter,
    BalsaStringParameter,
    BalsaTypeParameter
}
BalsaParameterNature;

/* BalsaParameter : descriminated union of allowable parameter natures */
typedef struct
{
    BalsaParameterNature nature;
    union
    {
        struct
        {
            FormatData *number;
            bool isNegated;     /* number is parsed as positive and this flag is
                                   set if the parameter number had a leading '-' to indicate that
                                   number.number should be negated once its correct length is known */
        }
        number;
        char *string;
        BalsaType *type;
    }
    info;
}
BalsaParameter;

/* BalsaParameterParseFromString : parse a parameter in one of the formats:
	"-?[0-9]+", "\"[^"]*\"" or "(breeze-type-decl)" from the given string */
extern BalsaParameter *BalsaParameterParseFromString (const char *string);

#endif
