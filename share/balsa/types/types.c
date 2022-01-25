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

	`types.balsa'
	Balsa type printing, comprehension function

*/

#include "balsasim/builtin.h"
#include <string.h>

/* LongestEnumerationElement (parameter X : type) is builtin : cardinal */
static void Types_LongestEnumerationElement (BuiltinFunction * function, BuiltinFunctionInstanceData * instance)
{
    if (!instance->userInstanceData)
    {
        BalsaType *type = instance->parameters[0]->info.type;
        unsigned nameLength = 0;
        int i;

        if (type->nature != BalsaEnumerationType)
            BALSA_SIM_PRINTF ("LongestEnumerationElement: type argument is not an enumerated type\n");
        else
        {
            for (i = 0; i < type->info.enumeration.elementCount; i++)
            {
                unsigned elementLength = strlen (type->info.enumeration.elementNames[i]);

                if (elementLength > nameLength)
                    nameLength = elementLength;
            }
        }

        instance->userInstanceData = (void *) nameLength;
    }
    instance->result->words[0] = (unsigned) instance->userInstanceData;
}

BALSA_SIM_REGISTER_BUILTIN_LIB (types)
{
    BalsaSim_RegisterBuiltinFunction ("LongestEnumerationElement", 1, 0, Types_LongestEnumerationElement, 32, NULL, 0);
}
