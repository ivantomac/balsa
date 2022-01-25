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

	`values.c'
	Type to describe typed values
	
 */

#include <string.h>
#include "values.h"
#include "instances.h"
#include "Errors.h"

TypedValue NoValue;             /* Useful for floccinaucinihilipilification */

DEFINE_LIST_TYPE_CONSTRUCTOR (TypedValue, TypedValue)
/* BeginValues : initialise */
void BeginValues (void)
{
    NoValue = NewTypedValue (NULL, NoType);
}

/* NewTypedValueImplicant : constructor */
TypedValue NewTypedValueImplicant (Implicant val, PtrType type)
{
    TypedValue ret;

    ret.value = val;
    ret.type = type;
    return ret;
}

/* NewTypedValue : constructor */
TypedValue NewTypedValue (PtrMP_INT val, PtrType type)
{
    TypedValue ret;

    ret.value = NewImplicant (val, NULL);
    ret.type = type;
    return ret;
}

/* NewBuiltinValue : make a new builtin value from the given pointer (in
	balsa-c this is just the pointer, in simulation this will include a
	reference count */
PtrMP_INT NewBuiltinValue (void *pointer)
{
#if SIZEOF_VOID_P == 8
    PtrMP_INT ret = NewMP_INT (0);

    mpz_set_ui (ret, (((guint64) pointer) & 0xFFFFFFFF00000000) >> 32);
    mpz_mul_2exp (ret, ret, 32);
    mpz_add_ui (ret, ret, ((guint32) pointer) & 0xFFFFFFFF);
#else
    PtrMP_INT ret = NewMP_INT (0);

    mpz_set_ui (ret, (guint32) pointer);
#endif

    return ret;
}

/* GetBuiltinValuePointer : get the pointer to a value of a builtin type */
void *GetBuiltinValuePointer (PtrMP_INT value)
{
    PtrMP_INT pointer31to0 = FilterRangeFromMP_INT (value, 32, 0);

#if SIZEOF_VOID_P == 8
    PtrMP_INT pointer63to32 = FilterRangeFromMP_INT (value, 32, 32);
    guint64 ret = (((guint64) mpz_get_ui (pointer63to32)) << 32) + mpz_get_ui (pointer31to0);

    DeleteMP_INT (pointer63to32);
#else
    guint32 ret = mpz_get_ui (pointer31to0);
#endif

    DeleteMP_INT (pointer31to0);

    return (void *) ret;
}

/* NewTypedValueBuiltin : constructor for opaque (simulator/compiler) builtin typed values */
TypedValue NewTypedValueBuiltin (void *pointer, PtrType type)
{
    TypedValue ret;

    ret.value = NewImplicant (NewBuiltinValue (pointer), NULL);
    ret.type = type;
    return ret;
}

/* MakeValueString : append a representation of a given value onto
   the given string `str' (str should point to the end of the existing string).
   Returns the tail of the modified string.
   NumericType -> number
   EnumerationType -> firstMatchingElementName or (number as typeName) for non matching values
   RecordType -> {elem0,elem1,elem2,elem3}
   ArrayType -> {elem0,elem1,elem2}
   StringTypeObj -> string
 */
Ptrchar MakeTypedValueString (Ptrchar str, TypedValue value)
{
    Ptrchar strTail;

    strTail = EndOfString (str); /* OK, OK, making sure */

    /* Not a constant? */
    if (!value.value.baseValue)
        return strTail;

    /* What type class is this? */
    switch (value.type->nature)
    {
    case NumericType:
        if (value.value.dontCares)
        {
            char *impString = MakeImplicantString (value.value, true);

            strcpy (strTail, impString);
            g_free (impString);
        } else if (value.type->size < 0) /* Need to sign extend */
        {
            PtrMP_INT sgnExtVal = CopyMP_INT (value.value.baseValue);

            RecoverSign (sgnExtVal, -(value.type->size));

            mpz_get_str (strTail, 10, sgnExtVal);
        } else
            mpz_get_str (strTail, 10, value.value.baseValue);
        break;
    case BuiltinType:
        if (value.type == StringTypeObj)
            strcat (strTail, GetBuiltinValuePointer (value.value.baseValue));
        else
            strcat (strTail, "<builtin>");
        break;
    case EnumerationType:
        {
            tIdent elementName = LookupEnumElemNameByValue (value.type, value.value.baseValue);

            if (elementName == NoIdent || value.value.dontCares) /* no element name, produce (number as typeName) */
            {
                sprintf (strTail, "(");
                strTail++;

                if (value.value.dontCares)
                {
                    char *impString = MakeImplicantString (value.value, true);

                    strcpy (strTail, impString);
                    g_free (impString);
                } else
                    mpz_get_str (strTail, 10, value.value.baseValue);
                strTail = EndOfString (strTail);
                sprintf (strTail, " as %s)", PeekString (value.type->ident));
            } else
                sprintf (strTail, "%s", PeekString (elementName));
        }
        break;
    case RecordType:
        {
            PtrInstanceList elements = value.type->info.record.recordElements;
            unsigned offset = 0;

            sprintf (strTail, "{");
            strTail++;
            while (elements)    /* print the elements one by one */
            {
                int elementSize = Abs (CAR (elements)->type->size);
                PtrMP_INT baseValue = FilterRangeFromMP_INT (value.value.baseValue, elementSize,
                  offset);
                PtrMP_INT dontCares = (value.value.dontCares ? FilterRangeFromMP_INT (value.value.dontCares,
                    elementSize,
                    offset) : NULL);
                TypedValue elemValue = (dontCares && mpz_sgn (dontCares) != 0 ? NewTypedValueImplicant (NewImplicant (baseValue,
                      dontCares),
                    CAR (elements)->type) : NewTypedValue (baseValue,
                    CAR (elements)->type));

                strTail = MakeTypedValueString (strTail, elemValue);
                DeleteMP_INT (baseValue);
                if (dontCares)
                    DeleteMP_INT (dontCares);

                offset += Abs (CAR (elements)->type->size);
                if (CDR (elements))
                {
                    strTail[0] = ',';
                    strTail[1] = '\0';
                    strTail++;
                }
                elements = CDR (elements);
            }
            sprintf (strTail, "}");
        }
        break;
    case ArrayType:
        {
            PtrType baseType = value.type->info.array.baseType;
            unsigned baseWidth = Abs (baseType->size);
            unsigned elementCount = value.type->info.array.elementCount;
            unsigned offset = 0;

            sprintf (strTail, "{");
            strTail++;
            for (elementCount = value.type->info.array.elementCount; elementCount != 0; elementCount--)
            {
                PtrMP_INT baseValue = FilterRangeFromMP_INT (value.value.baseValue, baseWidth,
                  offset);
                PtrMP_INT dontCares = (value.value.dontCares ? FilterRangeFromMP_INT (value.value.dontCares,
                    baseWidth, offset) : NULL);
                TypedValue elemValue = (dontCares && mpz_sgn (dontCares) != 0 ? NewTypedValueImplicant (NewImplicant (baseValue,
                      dontCares),
                    baseType) : NewTypedValue (baseValue, baseType));

                strTail = MakeTypedValueString (strTail, elemValue);
                DeleteMP_INT (baseValue);
                if (dontCares)
                    DeleteMP_INT (dontCares);

                offset += baseWidth;
                if (elementCount != 1)
                {
                    strTail[0] = ',';
                    strTail[1] = '\0';
                    strTail++;
                }
            }
            sprintf (strTail, "}");
        }
        break;
        /* Unhandled types */
    case SyncType:
    case ArrayedType:
    case ContextMarkerType:
    case UnresolvableType:
        break;
    }

    return EndOfString (strTail);
}

/* TypedValueIsNotBuiltin : raise an error and return false if the given value
	is of a builtin type, true otherwise */
/* TypedValueIsConstant : raise an error and return false if the given value
	is not a builtin typed constant */
/* TypedValueIsDCFreeConstant : raise an error and return false if the given value
	is a non builtin typed constant with no don't care bits */
/* TypedValueIsDCFree : like TypedValueIsDCFreeConstant but moan only moan about builtin,
	invalid type and DC freeness, don't moan about constness */
bool TypedValueIsNotBuiltin (TypedValue val, tPosition position)
{
    if (val.type == NoType)
    {
        LOG_ERROR (TypeIsNotValid, NoIdent, position);
        return false;
    }
    /* if (val.type->nature == BuiltinType) */
    if (val.type->hasBuiltinElements)
    {
        LOG_ERROR (CannotUseBuiltinTypeHere, NoIdent, position);
        return false;
    }
    return true;
}

bool TypedValueIsConstant (TypedValue val, tPosition position)
{
    if (!TypedValueIsNotBuiltin (val, position))
        return false;
    if (!val.value.baseValue)
    {
        LOG_ERROR (ExpressionMustBeConstant, NoIdent, position);
        return false;
    }
    return true;
}

bool TypedValueIsDCFreeConstant (TypedValue val, tPosition position)
{
    if (!TypedValueIsConstant (val, position))
        return false;
    if (val.value.dontCares)
    {
        LOG_ERROR (ImplicantNotAllowedHere, NoIdent, position);
        return false;
    }
    return true;
}

bool TypedValueIsDCFree (TypedValue val, tPosition position)
{
    if (!TypedValueIsNotBuiltin (val, position))
        return false;
    if (val.value.dontCares)
    {
        LOG_ERROR (ImplicantNotAllowedHere, NoIdent, position);
        return false;
    }
    return true;
}
