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

#ifndef BALSA_SIM_BUILTIN_H
#define BALSA_SIM_BUILTIN_H

#include "balsasim/object.h"
#include "balsasim/parameter.h"

struct BuiltinFunction_;
struct BuiltinFunctionInstanceData_;

/* BuiltinFunctionFunction : function type for builtin function body.  Arguments and
	results are to be found in `arguments' */
typedef void (*BuiltinFunctionFunction) (struct BuiltinFunction_ * function, struct BuiltinFunctionInstanceData_ * instance);

/* BalsaSim_SimulationLog : print behaviour for WriteMessage in Balsa */
extern void (*BalsaSim_SimulationLog) (const char *message, int length);

/* BuiltinFunction : builtin function info, construct using RegisterBuiltinFunction */
typedef struct BuiltinFunction_
{
    const char *name;
    BuiltinFunctionFunction function;
    BuiltinFunctionFunction function_ackdown;
    unsigned arity;             /* number of ports, *not* parameters and ports */
    unsigned parameterCount;
    unsigned *argumentWidths;
    unsigned resultWidth;
    unsigned objectCount;       /* objects passed back from this function which this
                                   library will allocate and renew in the instance data for the user's convenience */
}
BuiltinFunction;

/* BuiltinFunctionInstanceData : structure containing argument/result structure for
	builtin functions both with and without parameters */
typedef struct BuiltinFunctionInstanceData_
{
    bool portWidthsAreResolved; /* if false during a function call then some or all
                                   of the argumentWidths, resultWidth need to be filled in (ie. this
                                   function was parameterised).  This first call will then be a dry run
                                   to let that parameterisation take place. */
    unsigned *argumentWidths;   /* elements are set to 0 if uninitialised */
    unsigned resultWidth;       /* " " */
    unsigned objectCount;       /* This can be redefined by functions which have unresolved
                                   ports as their object counts may vary by port types.  Use BalsaTypeBuiltinCount */
    FormatData **arguments;
    FormatData *result;
    BalsaObject **objects;
    BalsaParameter **parameters;
    bool retryRequested;        /* If true, the builtin function call needs to be rescheduled at a later time */

    void *userInstanceData;     /* user settable instance data, set to NULL initially */
}
BuiltinFunctionInstanceData;

/* BalsaSim_FindBuiltinFunction : is the given name the name of a builtin function,
	returns BuiltinFunction structure if found */
extern BuiltinFunction *BalsaSim_FindBuiltinFunction (const char *name);

/* BalsaSim_BuiltinFunctionRenewObjects : visit all the objects declared as part of a
	BuiltinFunction's instanceData and free the contents (if any) if their refCount's
	are 0.  Then make new BalsaObject's if necessary */
extern void BalsaSim_BuiltinFunctionRenewObjects (BuiltinFunction * function, BuiltinFunctionInstanceData * instanceData);

/* NewBuiltinFunctionInstanceData : make a new instance data and initialise the objects,
	result and arguments, the parameter list passed must have been initialised */
extern BuiltinFunctionInstanceData *NewBuiltinFunctionInstanceData (BuiltinFunction * function, BalsaParameter ** parameters);

/* BalsaSim_RegisterBuiltinFunctionWithAckdown : register a builtin function's C implementation */
void BalsaSim_RegisterBuiltinFunctionWithAckdown (const char *name,
  unsigned parameterCount,
  unsigned arity,
  BuiltinFunctionFunction function, BuiltinFunctionFunction function_ackdown, unsigned resultWidth, unsigned argumentWidths[], unsigned objectCount);

/* BalsaSim_RegisterBuiltinFunction : register a builtin function's C implementation */
extern void BalsaSim_RegisterBuiltinFunction (const char *name,
  unsigned parameterCount, unsigned arity, BuiltinFunctionFunction function, unsigned resultWidth, unsigned argumentWidths[], unsigned objectCount);

/* BalsaSim_PrintF : printf to use for error/warning messages during user functions.
	use the macro BALSA_SIM_PRINTF in calls to this function */
extern int (*BalsaSim_PrintF) (const char *format, ...);

#define BALSA_SIM_PRINTF(...) do { if (BalsaSim_PrintF) BalsaSim_PrintF (__VA_ARGS__); } while (0)

/* BALSA_SIM_REGISTER_BUILTIN_LIB : macro that creates function declaration head for the
	externally-visible initialisation function which must be present in each builtin library */
#define BALSA_SIM_REGISTER_BUILTIN_LIB(libname) extern void BalsaSim_BuiltinLibrary_##libname (void)

/* BalsaSim_ConvertBackslashCharsToASCII : convert the \\ and \n escape sequences in the given
	string to their usual ASCII characters */
extern char *BalsaSim_ConvertBackslashCharsToASCII (char *string);

#endif
