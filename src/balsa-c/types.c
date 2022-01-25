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

	`types.c'
	Type construction for contexts
	
 */

#include "types.h"
#include "instances.h"
#include <string.h>

/* NoType : Untypable type marker, safer than using bomb type NULL's */
PtrType NoType = NULL;
PtrType SyncTypeObj = NULL;
PtrType StringTypeObj = NULL;
PtrType ContextMarkerTypeObj = NULL;
PtrType BitTypeObj = NULL;
PtrType ErrorTypeObj = NULL;

/* BeginTypes : Start up the type system */
void BeginTypes (void)
{
    NoType = NEW (Type);
    NoType->ident = MarkerIdent;
    NoType->nature = UnresolvableType;
    NoType->aliasing = NoType;
    NoType->size = 0;
    NoType->scope = ImportedScope;
    NoType->hasBuiltinElements = false;

    SyncTypeObj = NEW (Type);
    SyncTypeObj->ident = MakeIdent1 ("<sync>");
    SyncTypeObj->nature = SyncType;
    SyncTypeObj->aliasing = NoType;
    SyncTypeObj->size = 0;
    SyncTypeObj->scope = ImportedScope;
    SyncTypeObj->hasBuiltinElements = false;

    ContextMarkerTypeObj = NEW (Type);
    ContextMarkerTypeObj->ident = MarkerIdent;
    ContextMarkerTypeObj->nature = ContextMarkerType;
    ContextMarkerTypeObj->aliasing = NoType;
    ContextMarkerTypeObj->size = 0;
    ContextMarkerTypeObj->scope = ImportedScope;
    ContextMarkerTypeObj->hasBuiltinElements = false;

    /* BitTypeObj, StringTypeObj and ErrorTypeObj initialised in BeginContexts */
}

DEFINE_CONS_LIST_TYPE (Type)
/* NewNumericType : Create a numeric type */
PtrType NewNumericType (tIdent ident, SignedBits size)
{
    PtrType type = NEW (Type);

    type->ident = ident;
    type->nature = NumericType;
    type->aliasing = NoType;
    type->size = size;
    type->scope = InnerScope;
    type->hasBuiltinElements = false;
    return type;
}

/* NewEnumerationType : ditto de enums */
PtrType NewEnumerationType (tIdent ident, SignedBits size, PtrBindingList bindings)
{
    PtrType type = NEW (Type);

    type->ident = ident;
    type->nature = EnumerationType;
    type->aliasing = NoType;
    type->size = size;
    type->info.enumeration.enumBindings = bindings;
    type->scope = InnerScope;
    type->hasBuiltinElements = false;
    return type;
}

/* NewRecordType : fill in a record type */
PtrType NewRecordType (tIdent ident, Bits size, PtrInstanceList elements, Bits unpaddedLength)
{
    PtrType type = NEW (Type);
    PtrInstanceList elementsIter = elements;

    type->ident = ident;
    type->nature = RecordType;
    type->aliasing = NoType;
    type->size = size;
    type->info.record.unpaddedLength = unpaddedLength;
    type->info.record.recordElements = elements;
    type->scope = InnerScope;

    type->hasBuiltinElements = false;

    while (elementsIter)
    {
        if (CAR (elementsIter)->type->hasBuiltinElements)
        {
            type->hasBuiltinElements = true;
            break;
        }
        elementsIter = CDR (elementsIter);
    }
    return type;
}

/* NewArrayType : create a array over type baseType (having elements indexed from range.lowerBound to range.upperBound on the type range.boundingType) */
PtrType NewArrayType (tIdent ident, PtrType baseType, Span range)
{
    PtrType type = NEW (Type);
    mpz_t count;

    mpz_init (count);
    type->ident = ident;
    type->nature = ArrayType;
    type->info.array.baseType = baseType;
    type->info.array.range = range;
    mpz_sub (count, range.upperBound, range.lowerBound);
    type->info.array.elementCount = mpz_get_ui (count) + 1;
    mpz_clear (count);
    type->aliasing = NoType;
    /* type->size is the complete length of elements, only really useful for variables of this type */
    type->size = type->info.array.elementCount * Abs (type->info.array.baseType->size);
    type->scope = InnerScope;
    type->hasBuiltinElements = baseType->hasBuiltinElements;
    return type;
}

/* NewArrayedType : create a arrayed object list over type baseType (having elements indexed from 
   range.lowerBound to range.upperBound on the type range.boundingType). */
PtrType NewArrayedType (PtrType baseType, Span range, PtrInstance modelInstance)
{
    PtrType type = NEW (Type);
    mpz_t count;
    PtrMP_INT i = NewMP_INT (0);
    PtrInstanceList elements = NULL;
    const char *identStr = PeekString (modelInstance->ident);
    unsigned identLength = strlen (identStr);

    mpz_init (count);
    type->ident = MarkerIdent;
    type->nature = ArrayedType;
    type->info.arrayed.baseType = baseType;
    type->info.arrayed.range = range;
    mpz_sub (count, range.upperBound, range.lowerBound);
    type->info.arrayed.elementCount = mpz_get_ui (count) + 1;
    mpz_clear (count);
    type->aliasing = NoType;
    /* This is not really a type, so don't recognise this */
    type->size = 0;
    type->scope = InnerScope;
    type->hasBuiltinElements = baseType->hasBuiltinElements;

    mpz_set (i, range.upperBound); /* Iterate over range elements to construct type */
    for (; mpz_cmp (i, range.lowerBound) >= 0; mpz_sub_ui (i, i, 1))
    {
        unsigned nameLength = identLength + 4 /* [ ] */  + mpz_sizeinbase (i, 10);
        char *newIdent = NEW_ARRAY (char, nameLength);
        PtrInstance newElement = NEW (Instance);

        *newElement = *modelInstance; /* Copy the model instance */
        /* Generate an individual name */
        sprintf (newIdent, "%s[", identStr);
        mpz_get_str (newIdent + identLength + 1, 10, i);
        strcat (newIdent, "]");
        newElement->ident = MakeIdent1 (newIdent);
        elements = NewInstanceList (newElement, elements);
        FREE_ARRAY (char, nameLength, newIdent);
    }
    type->info.arrayed.arrayedElements = elements;
    DeleteMP_INT (i);
    return type;
}

/* NewBuiltinType : make new builtin type */
PtrType NewBuiltinType (void)
{
    PtrType type = NEW (Type);

    type->ident = MarkerIdent;
    type->nature = BuiltinType;
    type->aliasing = NoType;
    type->size = 64;            /* A pointer! */
    type->scope = InnerScope;
    type->hasBuiltinElements = true;

    return type;
}

/* NewElementInitialisedArrayedType : create a arrayed type using the given instance list as the
	elements list.  `elementCount' should reflect the exact length of `elements' (or of the local portion
	of that list with a ContextMarkerInstance at ref. `elementCount') */
PtrType NewElementInitialisedArrayedType (tIdent ident, PtrType baseType, PtrInstanceList elements, unsigned elementCount)
{
    PtrType ret = NEW (Type);
    PtrMP_INT maxElementCount = NewMP_INT (elementCount - 1);

    ret->ident = ident;
    ret->nature = ArrayedType;
    ret->info.arrayed.baseType = baseType;
    ret->info.arrayed.range = NewSpan (NewMP_INT (0), maxElementCount, NewNumericType (MarkerIdent, SmallestRangeToHoldValue (maxElementCount)));
    ret->info.arrayed.elementCount = elementCount;
    ret->aliasing = NoType;
    /* This is not really a type, so don't recognise this */
    ret->size = 0;
    ret->info.arrayed.arrayedElements = elements;
    ret->scope = InnerScope;
    ret->hasBuiltinElements = baseType->hasBuiltinElements;

    return ret;
}

/* CopyAnonType : copy a type, discarding it's name but sharing any linked structures within it (eg.
   record/enum elements list) */
PtrType CopyAnonType (PtrType type)
{
    PtrType newType = NEW (Type);

    *newType = *type;
    newType->ident = MarkerIdent;
    return newType;
}

/* AliasType: like CopyAnonType but name the new type and make it an alias of the old type */
PtrType AliasType (PtrType type, tIdent ident)
{
    PtrType newType = CopyAnonType (type);

    /* Aliasing is transitive */
    if (newType->aliasing == NoType)
        newType->aliasing = type;

    newType->ident = ident;
    return newType;
}

/* LookupEnumElemValue : lookup an enumeration element member, returns its value or NULL on failure */
PtrMP_INT LookupEnumElemValue (PtrType type, tIdent member)
{
    PtrBindingList bindings = type->info.enumeration.enumBindings;

    /* FOR_EACH (bindings, true, ...) */
    while (bindings)
    {
        if (CAR (bindings).ident == member)
            return CAR (bindings).value;
        bindings = CDR (bindings);
    }
    return NULL;
}

/* LookupEnumElemNameByValue : lookup an enumeration element by value, returns the first
   matching element name or NoIdent on failure */
tIdent LookupEnumElemNameByValue (PtrType type, PtrMP_INT value)
{
    PtrBindingList bindings = type->info.enumeration.enumBindings;

    /* FOR_EACH (bindings, true, ...) */
    while (bindings)
    {
        if (mpz_cmp (CAR (bindings).value, value) == 0)
            return CAR (bindings).ident;
        bindings = CDR (bindings);
    }
    return NoIdent;
}

/* TypeEquivalence : returns true if the 2 given types are equivalent, for
   numeric types this is by size and signedness, for other types it's
   a comparison of pointers (it's important not to copy types!), using
   the pointer is safe than using name-name equivalence as pointers are 
   unique w.r.t scope BTW. NULL pointers are equivalent ;) (array types are compared for
   baseType and size (number of elements) */
bool TypeEquivalence (PtrType typeA, PtrType typeB)
{
    if (!typeA && !typeB)
        return true;
    if (typeA->nature == NumericType && typeB->nature == NumericType)
        return typeA->size == typeB->size;
    if (typeA->nature == ArrayType && typeB->nature == ArrayType)
        return TypeEquivalence (typeA->info.array.baseType,
          typeB->info.array.baseType) && typeA->info.array.elementCount == typeB->info.array.elementCount;
    if (typeA->nature == ArrayedType && typeB->nature == ArrayedType)
        return TypeEquivalence (typeA->info.arrayed.baseType,
          typeB->info.arrayed.baseType) && typeA->info.arrayed.elementCount == typeB->info.arrayed.elementCount;
    else
        return (typeA->aliasing == NoType ? typeA : typeA->aliasing) == (typeB->aliasing == NoType ? typeB : typeB->aliasing);
}

/* SubscriptTypeIsOK : returns true if the two given types are the same or if the the reqdType is
   numeric and the given boundingType is narrower or equal to the reqdType */
bool SubscriptTypeIsOK (PtrType reqdType, PtrType actualType)
{
    return TypeEquivalence (reqdType, actualType)
      || (reqdType->nature == NumericType && actualType->nature == NumericType && RangeIsWiderOrEqual (reqdType->size, actualType->size));
}

/* RangeOfType : returns a span representing the range of values valid for this type,
   for numeric or enumerated types this is either [0,(2^n)-1] or [-2^(n-1),(2^(n-1))-1], for other classes of
   types [0,n-1] is returned where n = 2^type->size */
Span RangeOfType (PtrType type)
{
    if (type->size > 0)
        return NewSpan (NewMP_INT (0), MakeMaskForRange (type->size, 0), type);
    else
    {
        PtrMP_INT upper = MakeMaskForRange ((-type->size) - 1, 0); /* 2^(n-1)-1 */
        PtrMP_INT lower = NewMP_INT (0);

        mpz_neg (lower, upper);
        mpz_add_ui (lower, lower, 1);

        return NewSpan (lower, upper, type);
    }
    return NoSpan;
}

/* StrPtrTypeName : print out the name, or `n bits' spec for the given type */
void StrPtrTypeName (FILE * stream, PtrType type)
{
    if (type)
    {
        switch (type->nature)
        {
        case ContextMarkerType:
            fprintf (stream, "-- context-marker");
            break;
        case NumericType:
            StrSignedBits (stream, type->size);
            break;
        case ArrayType:
            fprintf (stream, "array ");
            StrSpan (stream, type->info.array.range, true);
            fprintf (stream, " of ");
            StrPtrTypeName (stream, type->info.array.baseType);
            break;
        default:
            if (type->aliasing != NoType) /* Print the root name */
                WriteIdent (stream, (type->aliasing != NoType ? type->aliasing->ident : type->ident));
            break;
        }
    } else
        fprintf (stream, "NULL");
}

/* MakeTypeNameString : like StrPtrTypeName but into a string */
Ptrchar MakeTypeNameString (Ptrchar str, PtrType type)
{
    Ptrchar strTail;

    strTail = EndOfString (str); /* OK, OK, making sure */

    if (type)
    {
        switch (type->nature)
        {
        case ContextMarkerType:
            strcat (strTail, "-- context-marker");
            break;
        case NumericType:
            sprintf (strTail, "%d %sbits", Abs (type->size), (type->size < 0 ? "signed " : ""));
            break;
        case ArrayType:
            sprintf (strTail, "array %ld .. %ld of ", mpz_get_si (type->info.array.range.lowerBound), mpz_get_si (type->info.array.range.upperBound));
            strTail = EndOfString (strTail);
            MakeTypeNameString (strTail, type->info.array.baseType);
            break;
        default:
            if (type->aliasing != NoType) /* Print the root name */
                strcat (strTail, PeekString (type->aliasing != NoType ? type->aliasing->ident : type->ident));
            break;
        }
    }

    return EndOfString (strTail);
}

/* TypeIsInTypeList : returns true if the given type is present in the given type list,
	probably should do this with a generalised search function in lists.h */
bool TypeIsInTypeList (PtrType type, PtrTypeList types)
{
    bool ret = false;

    while (types && !ret)
    {
        if (CAR (types) == type)
            ret = true;

        types = CDR (types);
    }

    return ret;
}

/* StrPtrSBreezeTypeName : like StrPtrTypeName but use the SBreeze format (named-type "name"),
   (numeric-type signedness size) or (array type lowIndex elementCount) */
void StrPtrSBreezeTypeName (FILE * stream, PtrType type)
{
    if (type)
    {
        if (type->nature != ContextMarkerType)
        {
            if (type->nature == NumericType)
                fprintf (stream, "(numeric-type %s %d)", SCHEME_BOOL_STRING (type->size < 0), Abs (type->size));
            else if (type->nature == ArrayType)
            {
                PtrMP_INT arraySize = NewMP_INT (0);

                mpz_sub (arraySize, type->info.array.range.upperBound, type->info.array.range.lowerBound);
                mpz_add_ui (arraySize, arraySize, 1);

                fprintf (stream, "(array-type ");
                StrPtrSBreezeTypeName (stream, type->info.array.baseType);
                putc (' ', stream);
                StrPtrMP_INT (stream, type->info.array.range.lowerBound);
                putc (' ', stream);
                StrPtrMP_INT (stream, arraySize);
                if (type->info.array.range.boundingType->nature == EnumerationType)
                {
                    putc (' ', stream);
                    StrPtrSBreezeTypeName (stream, type->info.array.range.boundingType);
                }
                putc (')', stream);
            } else
            {
                fprintf (stream, "(named-type \"");
                if (type->aliasing != NoType) /* Print the root name */
                    WriteIdent (stream, type->aliasing->ident);
                else
                    WriteIdent (stream, type->ident);
                fprintf (stream, "\")");
            }
        } else
            fprintf (stream, ";;; context-marker");
    } else
        fprintf (stream, "(invalid-type)");
}

/* StrPtrSBreeze{Type(List)?} : Print out types/type descriptions in the SBreeze format.
	If `printName' is true then print a (type "name" ...) wrapper around the type.
	If `expandToTopLevel' is true then only print subtype names of types which aren't
	ImportedScope or TopLevelScope declarations */
void StrPtrSBreezeType (FILE * stream, PtrType type, bool printName, bool expandToTopLevel)
{
    if (type)
    {
        if (type->nature != ContextMarkerType)
        {
            if (printName)
            {
                fprintf (stream, "(type \"");
                WriteIdent (stream, type->ident);
                fprintf (stream, "\" ");
            }
            switch (type->nature)
            {
            case NumericType:
            case ArrayType:
                /* Careful here if the SBreeze use suddenly becomes embeddable and
                   this doesn't work! */
                StrPtrSBreezeTypeName (stream, type);
                break;
            case BuiltinType:
                fprintf (stream, "(builtin-type)");
                break;
            case EnumerationType:
                if (!expandToTopLevel && type->aliasing != NoType)
                {
                    fprintf (stream, "(alias-type \"");
                    WriteIdent (stream, type->aliasing->ident);
                    fprintf (stream, "\")");
                } else
                {
                    fprintf (stream, "(enumeration-type %s %d (\"", SCHEME_BOOL_STRING (type->size < 0), Abs (type->size));
                    StrPtrBindingList (stream, type->info.enumeration.enumBindings, ") (\"", "\" ");
                    fprintf (stream, "))");
                }
                break;
            case RecordType:
                if (!expandToTopLevel && type->aliasing != NoType)
                {
                    fprintf (stream, "(alias-type \"");
                    WriteIdent (stream, type->aliasing->ident);
                    fprintf (stream, "\")");
                } else
                {
                    fprintf (stream, "(record-type %d ", Abs (type->size));
                    {
                        PtrInstanceList elems = type->info.record.recordElements;

                        /* FOR_EACH (elems, CAR (elems)->nature == RecordElementInstance, ...) */
                        while (elems && CAR (elems)->nature == RecordElementInstance)
                        {
                            fprintf (stream, "(\"");
                            WriteIdent (stream, CAR (elems)->ident);
                            fprintf (stream, "\" ");
                            if (expandToTopLevel && !(CAR (elems)->type->scope & TOP_LEVEL_SCOPES))
                                StrPtrSBreezeType (stream, CAR (elems)->type, false, expandToTopLevel);
                            else
                                StrPtrSBreezeTypeName (stream, CAR (elems)->type);
                            fprintf (stream, ") ");
                            elems = CDR (elems);
                        }
                    }
                    fprintf (stream, ")");
                }
                break;
            case ArrayedType:
                fprintf (stream, "(arrayed)");
                break;
            case SyncType:
                fprintf (stream, "(sync)");
                break;
            case UnresolvableType:
                fprintf (stream, "(no-type)");
                break;
            case ContextMarkerType:
                break;
            }
            if (printName)
                putc (')', stream);
        } else
            fprintf (stream, ";;; context-marker");
    } else
        fprintf (stream, "()");
}

void StrPtrSBreezeTypeList (FILE * stream, PtrTypeList types, bool onlyPrintLocals, Scope scopes, char *separator)
{
    if (types && (!onlyPrintLocals || CAR (types)->nature != ContextMarkerType))
    {
        if (CDR (types) && (onlyPrintLocals ? CADR (types)->nature != ContextMarkerType : true))
        {
            StrPtrSBreezeTypeList (stream, CDR (types), onlyPrintLocals, scopes, separator);
        }
        if ((int) CAR (types)->scope & (int) scopes)
        {
            fprintf (stream, "%s", separator);
            StrPtrSBreezeType (stream, CAR (types), true, false);
        }
    }
}

/* StrPtrBreezeTopLevelType : print out either the name of a type (for types with names and which are
	visible at the top level) or the type definition body for types without names (or not ...) */
void StrPtrBreezeTopLevelType (FILE * stream, PtrType type)
{
    if (!(type->scope & TOP_LEVEL_SCOPES))
        StrPtrSBreezeType (stream, type, false, true);
    else
        StrPtrSBreezeTypeName (stream, type);
}
