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
	Built-in function handling

*/

#include "builtin.h"
#include "list.h"
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include <unistd.h>
#include <errno.h>

int (*BalsaSim_PrintF) (const char *format, ...) = NULL;

void (*BalsaSim_SimulationLog) (const char *message, int length) = NULL;

static BalsaList *BalsaSim_BuiltinFunctions = NULL;

/* BalsaSim_FindBuiltinFunction : is the given name the name of a builtin function,
	returns BuiltinFunction structure if found */
BuiltinFunction *BalsaSim_FindBuiltinFunction (const char *name)
{
    BuiltinFunction *ret = NULL;
    BalsaList *functions = BalsaSim_BuiltinFunctions;

    while (functions && !ret)
    {
        if (strcmp (name, ((BuiltinFunction *) functions->data)->name) == 0)
            ret = functions->data;
        functions = functions->next;
    }
    return ret;
}

/* BalsaSim_RegisterBuiltinFunctionWithAckdown : register a builtin function's C implementation */
void
BalsaSim_RegisterBuiltinFunctionWithAckdown (const char *name,
  unsigned parameterCount,
  unsigned arity,
  BuiltinFunctionFunction function, BuiltinFunctionFunction function_ackdown, unsigned resultWidth, unsigned argumentWidths[], unsigned objectCount)
{
    BuiltinFunction *newFunction = calloc (1, sizeof (BuiltinFunction));

    newFunction->name = name;
    newFunction->arity = arity;
    newFunction->parameterCount = parameterCount;
    newFunction->function = function;
    newFunction->function_ackdown = function_ackdown;
    newFunction->resultWidth = resultWidth;
    newFunction->argumentWidths = malloc (sizeof (unsigned) * arity);
    newFunction->objectCount = objectCount;
    memcpy (newFunction->argumentWidths, argumentWidths, sizeof (unsigned) * arity);

    BALSA_SIM_PRINTF ("registered Balsa builtin function `%s'\n", name);

    BalsaSim_BuiltinFunctions = NewBalsaList (newFunction, BalsaSim_BuiltinFunctions);
}

/* BalsaSim_RegisterBuiltinFunction : register a builtin function's C implementation */
void
BalsaSim_RegisterBuiltinFunction (const char *name, unsigned parameterCount,
  unsigned arity, BuiltinFunctionFunction function, unsigned resultWidth, unsigned argumentWidths[], unsigned objectCount)
{
    BalsaSim_RegisterBuiltinFunctionWithAckdown (name, parameterCount, arity, function, NULL, resultWidth, argumentWidths, objectCount);
}

/* CheckPortWidthsAreResolved : returns true if all the port and result width for the
	given instanceData are resolved */
static bool CheckPortWidthsAreResolved (unsigned arity, BuiltinFunctionInstanceData * instance)
{
    int i;

    for (i = 0; i < arity; i++)
    {
        if (instance->argumentWidths[i] == 0)
            return false;
    }
    if (instance->resultWidth == 0)
        return false;

    return true;
}

/* MakeBuiltinFunctionInstanceResultAndArguments : fill in the result and argument
	FormatData elements for ports and result.  Complain if any widths are unresolved, and
	return false */
static bool MakeBuiltinFunctionInstanceResultAndArguments (unsigned arity, const char *functionName, BuiltinFunctionInstanceData * instance)
{
    int i;

    for (i = 0; i < arity; i++)
    {
        /* If an argument width is 0, then it depends on the parameters and so
           the argument field must be initialised by the function in question */
        if (instance->argumentWidths[i] != 0)
        {
            instance->arguments[i] = NewFormatData (FormatBitCountToWordCount (instance->argumentWidths[i]));
        } else
        {
            BALSA_SIM_PRINTF ("width of argument %d of function %s is unresolved\n", i + 1, functionName);
            return false;
        }
    }
    /* If the result width is 0, then " " */
    if (instance->resultWidth != 0)
    {
        instance->result = NewFormatData (FormatBitCountToWordCount (instance->resultWidth));
    } else
    {
        BALSA_SIM_PRINTF ("result with of function %s is unresolved\n", functionName);
        return false;
    }

    return true;
}

/* NewBuiltinFunctionInstanceData : make a new instance data and initialise the objects,
	result and arguments, the parameter list passed must have been initialised */
BuiltinFunctionInstanceData *NewBuiltinFunctionInstanceData (BuiltinFunction * function, BalsaParameter ** parameters)
{
    int i;
    BuiltinFunctionInstanceData *ret = malloc (sizeof (BuiltinFunctionInstanceData));

    ret->arguments = malloc (sizeof (FormatData *) * function->arity);
    ret->userInstanceData = NULL;
    ret->portWidthsAreResolved = true;
    ret->objectCount = function->objectCount;
    ret->parameters = parameters;
    ret->resultWidth = function->resultWidth;
    ret->argumentWidths = function->argumentWidths;

    /* Check to see if all the widths are resolved */
    ret->portWidthsAreResolved = CheckPortWidthsAreResolved (function->arity, ret);

    /* Do a dry run function call and resolve the port cardinalities */
    if (!ret->portWidthsAreResolved)
    {
        /* We can't share argumentWidth's with the BuiltinFunction, need instance specific list */
        size_t sizeOfArgumentWidths = sizeof (unsigned) * function->arity;

        ret->argumentWidths = malloc (sizeOfArgumentWidths);
        memcpy (ret->argumentWidths, function->argumentWidths, sizeOfArgumentWidths);

        /* Do the function dry run */
        function->function (function, ret);

        ret->portWidthsAreResolved = true;
    }
    /* Try to make remaining port FormatDatas */
    if (!MakeBuiltinFunctionInstanceResultAndArguments (function->arity, function->name, ret))
        exit (EXIT_FAILURE);

    /* Make the objects to hold BalsaObject results */
    ret->objects = malloc (sizeof (BalsaObject *) * ret->objectCount);
    for (i = 0; i < ret->objectCount; i++)
    {
        ret->objects[i] = NewBalsaObject (NULL, NULL);
        BalsaObjectRef (ret->objects[i]);
    }

    return ret;
}

/* BalsaSim_BuiltinFunctionRenewObjects : visit all the objects declared as part of a
	BuiltinFunction's instanceData and free the contents (if any) if their refCount's
	are 0.  Then make new BalsaObject's if necessary */
void BalsaSim_BuiltinFunctionRenewObjects (BuiltinFunction * function, BuiltinFunctionInstanceData * instanceData)
{
    int i;

/*
	fprintf (stderr, "HHH %d %s\n", instanceData->objectCount, function->name);
	for (i = 0; i < function->arity; i++)
		fprintf (stderr, "%d\n", instanceData->argumentWidths[i]);
*/

    for (i = 0; i < instanceData->objectCount; i++)
    {
        BalsaObject *object = instanceData->objects[i];

        /* Consider destroying object body */
        if (object->refCount <= 1)
        {
            if (object->destructor && object->data)
                object->destructor (object->data);
            object->data = NULL;
            object->refCount = 1;
        } else
        {
            object->refCount--; /* Set previous object adrift */
            instanceData->objects[i] = NewBalsaObject (NULL, NULL);
        }
    }

    instanceData->retryRequested = 0;
}

/* BalsaSim_ConvertBackslashCharsToASCII : convert the \\ and \n escape sequences in the given
	string to their usual ASCII characters */
char *BalsaSim_ConvertBackslashCharsToASCII (char *string)
{
    char *srcPtr = string;
    char *dstPtr = string;

    while (*srcPtr)
    {
        bool copyChar = true;

        if (*srcPtr == '\\')
        {
            if (srcPtr[1])
            {
                srcPtr++;
                switch (*srcPtr)
                {
                case 'n':
                    *dstPtr = '\n';
                    copyChar = false;
                    break;
                default:
                    break;
                }
            }
        }

        if (copyChar)
            *dstPtr = *srcPtr;

        srcPtr++;
        dstPtr++;
    }
    *dstPtr = '\0';
    return string;
}
