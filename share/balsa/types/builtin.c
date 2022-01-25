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

	`builtin.c'
	Builtin functions needed to implement Balsa language operators

*/

#include "balsasim/object.h"
#include "balsasim/builtin.h"
#include "balsasim/bstring.h"
#include <stdlib.h>
#include <string.h>

/* String (parameter string : String) is builtin : String */
static void Builtin_String (BuiltinFunction * function, BuiltinFunctionInstanceData * instance)
{
    if (!instance->userInstanceData)
    {
        char *newString = BalsaSim_ConvertBackslashCharsToASCII (strdup (instance->parameters[0]->info.string));
        BalsaString *string = NewBalsaString (NULL, 0);
        BalsaObject *object = NewBalsaObject (string, (BalsaDestructor) BalsaStringUnref);

        string->string = string->allocatedString = newString;
        string->length = strlen (newString);
        BalsaObjectRef (object);
        FormatDataSetBalsaObject (instance->result, object, 0);

        instance->userInstanceData = object;
    }
}

/* StringAppend (str1, str2 : String) is builtin : String */
static void Builtin_StringAppend (BuiltinFunction * function, BuiltinFunctionInstanceData * instance)
{
    BalsaString *str1 = BALSA_STRING (FormatDataGetBalsaObject (instance->arguments[0], 0)->data);
    BalsaString *str2 = BALSA_STRING (FormatDataGetBalsaObject (instance->arguments[1], 0)->data);
    BalsaString *ret = NewBalsaString (NULL, 0);

    ret->length = str1->length + str2->length;
    ret->allocatedString = ret->string = malloc (ret->length + 1);

    memcpy (ret->string, str1->string, str1->length);
    /* This should put on the safety NUL */
    memcpy (ret->string + str1->length, str2->string, str2->length);

    SetBalsaObject (instance->objects[0], ret, (BalsaDestructor) BalsaStringUnref);
    FormatDataSetBalsaObject (instance->result, instance->objects[0], 0);
}

#define ABS(v) ((v) < 0 ? -(v) : (v))

/* ToString (parameter X : type; value : X) is builtin : String */
static void Builtin_ToString (BuiltinFunction * function, BuiltinFunctionInstanceData * instance)
{
    if (!instance->portWidthsAreResolved)
    {
        if (!instance->parameters[0]->info.type)
        {
            BALSA_SIM_PRINTF ("Invalid type in ToString\n");
            exit (EXIT_FAILURE);
        }

        instance->argumentWidths[0] = ABS (instance->parameters[0]->info.type->size);
    } else
    {
        BalsaType *type = instance->parameters[0]->info.type;
        BalsaString *ret;

        if (!type->format)
        {
            BalsaTypeMakeDefaultFormat (type);
            if (!type->format)
                BALSA_SIM_PRINTF ("Invalid type in ToString\n");
        }

        if (type->format)
        {
            int maxRetLength = FormatLength (type->format,
              FormatBareDecimalRadixChoice, NULL,
              instance->arguments[0]);

            ret = NewBalsaString (NULL, 0);

            /* ret->string is allowed to be oversized, use a common pool for this stuff? */
            ret->string = ret->allocatedString = malloc (maxRetLength);

            FormatPrint (ret->string, type->format, instance->arguments[0], FormatBareDecimalRadixChoice, NULL);

            /* find the actual formatted length, don't worry that we've over allocated
               ret->string, this will get sorted out when it is free'd */
            ret->length = strlen (ret->string);
        } else
            ret = NewBalsaString ("<invalid>", -1);
/*
		BALSA_SIM_PRINTF ("Passed type: ");
		PrintBalsaType (stdout, instance->parameters[0]->info.type, -1);
		BALSA_SIM_PRINTF ("\n");
*/

        SetBalsaObject (instance->objects[0], ret, (BalsaDestructor) BalsaStringUnref);
        FormatDataSetBalsaObject (instance->result, instance->objects[0], 0);
    }
}

/* WriteMessage (str : String) : bit : write a runtime printing message string, returns 1 */
static void Builtin_WriteMessage (BuiltinFunction * function, BuiltinFunctionInstanceData * instance)
{
    BalsaString *str1 = BALSA_STRING (FormatDataGetBalsaObject (instance->arguments[0], 0)->data);

    if (BalsaSim_SimulationLog)
    {
        BalsaSim_SimulationLog (str1->string, str1->length);
    } else
    {
        fwrite (str1->string, str1->length, 1, stdout);
        fprintf (stdout, "\n");
    }

    instance->result->words[0] = 1;
}

BALSA_SIM_REGISTER_BUILTIN_LIB (builtin)
{
    BalsaSim_RegisterBuiltinFunction ("String", 1, 0, Builtin_String, 64, NULL, 0);
    BalsaSim_RegisterBuiltinFunction ("StringAppend", 0, 2, Builtin_StringAppend, 64, (unsigned[])
      {
      64, 64}
      , 1);
    BalsaSim_RegisterBuiltinFunction ("ToString", 1, 1, Builtin_ToString, 64, (unsigned[])
      {
      0}
      , 1);
    BalsaSim_RegisterBuiltinFunction ("WriteMessage", 0, 1, Builtin_WriteMessage, 1, (unsigned[])
      {
      64}
      , 0);
    /* tWriteMessage is WriteMessage with 0 bit return */
    BalsaSim_RegisterBuiltinFunction ("tWriteMessage", 0, 1, Builtin_WriteMessage, 1, (unsigned[])
      {
      64}
      , 0);
}
