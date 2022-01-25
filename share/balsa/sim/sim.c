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

	`sim.c'
	Dummy module so as not to make a special case of this module

*/

#include <stdlib.h>
#include <string.h>
#include "balsasim/builtin.h"
#include "balsasim/object.h"
#include "balsasim/bstring.h"

extern long long BalsaSimulationTimeValue (void);

/* function ForwardValuePrintMessage (parameter portName : String; value : String) is builtin : String */
static void ForwardValuePrintMessage (BuiltinFunction * function, BuiltinFunctionInstanceData * instance)
{
    BalsaString *string = BALSA_STRING (FormatDataGetBalsaObject (instance->arguments[0], 0)->data);

    char *str = malloc (string->length + 1);

    memcpy (str, string->string, string->length);
    str[string->length] = '\0';
    instance->userInstanceData = str;

    BalsaString *retString = NewBalsaString (string->string, string->length);

    SetBalsaObject (instance->objects[0], retString, (BalsaDestructor) BalsaStringUnref);
    FormatDataSetBalsaObject (instance->result, instance->objects[0], 0);
}

static void ForwardValuePrintMessage_ackdown (BuiltinFunction * function, BuiltinFunctionInstanceData * instance)
{
    long long simTime = BalsaSimulationTimeValue ();
    char *portname = instance->parameters[0]->info.string;
    char *str = instance->userInstanceData;

    if (BalsaSim_SimulationLog)
    {
        char *message = malloc (strlen (portname) + strlen (str) + 100);

        sprintf (message, "%lld: chan '%s' writing %s", simTime, portname, str);
        BalsaSim_SimulationLog (message, -1);
        free (message);
    } else
    {
        printf ("%lld: chan '%s' writing %s\n", simTime, portname, str);
    }

    free (str);
    instance->userInstanceData = NULL;
}

BALSA_SIM_REGISTER_BUILTIN_LIB (sim)
{
    BalsaSim_RegisterBuiltinFunctionWithAckdown ("ForwardValuePrintMessage", 1,
      1, ForwardValuePrintMessage, ForwardValuePrintMessage_ackdown, 64, (unsigned[])
      {
      64}
      , 1);
}
