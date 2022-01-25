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

	`values.h'
	Type to describe typed values
	
 */

#ifndef VALUES_HEADER
#define VALUES_HEADER

#include "types.h"
#include "implicants.h"
#include "arith.h"
#include "Positions.h"
#include <stdio.h>

/* Value x Type combo */
typedef struct TypedValue
{
    Implicant value;            /* In normal (non implicant) use, the dontCares element *must* be NULL */
    PtrType type;
}
TypedValue;

DECLARE_LIST_TYPE (TypedValue, TypedValue) DECLARE_LIST_TYPE_CONSTRUCTOR (TypedValue, TypedValue)
/* NoValue : NULL,NoType */
extern TypedValue NoValue;

/* BeginValues : initialise */
extern void BeginValues (void);

/* NewTypedValue : constructor */
extern TypedValue NewTypedValue (PtrMP_INT val, PtrType type);

/* NewTypedValueImplicant : constructor */
extern TypedValue NewTypedValueImplicant (Implicant val, PtrType type);

/* NewBuiltinValue : make a new builtin value from the given pointer (in
	balsa-c this is just the pointer, in simulation this will include a
	reference count */
extern PtrMP_INT NewBuiltinValue (void *pointer);

/* GetBuiltinValuePointer : get the pointer to a value of a builtin type */
extern void *GetBuiltinValuePointer (PtrMP_INT value);

/* NewTypedValueBuiltin : constructor for opaque (simulator/compiler) builtin typed values */
extern TypedValue NewTypedValueBuiltin (void *pointer, PtrType type);

/* MakeTypedValueString : append a representation of a given value onto
   the given string `str' (str should point to the end of the existing string).
   Returns the tail of the modified string. */
extern Ptrchar MakeTypedValueString (Ptrchar str, TypedValue value);

/* TypedValueIsNotBuiltin : raise an error and return false if the given value
	is of a builtin type, true otherwise */
extern bool TypedValueIsNotBuiltin (TypedValue val, tPosition position);

/* TypedValueIsConstant : raise an error and return false if the given value
	is not a builtin typed constant */
extern bool TypedValueIsConstant (TypedValue val, tPosition position);

/* TypedValueIsDCFreeConstant : raise an error and return false if the given value
	is a non builtin typed constant with no don't care bits */
extern bool TypedValueIsDCFreeConstant (TypedValue val, tPosition position);

/* TypedValueIsDCFree : like TypedValueIsDCFreeConstant but moan only moan about builtin,
	invalid type and DC freeness, don't moan about constness */
extern bool TypedValueIsDCFree (TypedValue val, tPosition position);

#endif /* VALUES_HEADER */
