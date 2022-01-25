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

	`types.h'
	Type datastructure for describing
	`balsa' compilable types.
	
 */

#ifndef TYPES_HEADER
#define TYPES_HEADER

#include <spans.h>
#include "misclists.h"

/* BeginTypes : Start up the type system */
extern void BeginTypes (void);

/* Scope : declaration scope for types/instances/procedures/parts */
typedef enum Scope
{
    ImportedScope = 1,
    TopLevelScope = 2,
    InnerScope = 4
}
Scope;

#define TOP_LEVEL_SCOPES (ImportedScope | TopLevelScope)
#define ALL_SCOPES (TOP_LEVEL_SCOPES | InnerScope)

struct Instance;
struct InstanceList;            /* Forward declaration - cyclic dependancy */

/* TypeNature : specifies which of the n type classes a given type falls into */
typedef enum TypeNature
{
    NumericType,                /* Regular balsa types */
    EnumerationType,
    RecordType,
    SyncType,
    ArrayType,                  /* Arrays, eg. channel a : array 8 of word */
    ArrayedType,                /* Arrayed, eg. array 8 of channel a : word */
    BuiltinType,                /* Special builtin type */
    ContextMarkerType,          /* Context mark in contexts */
    UnresolvableType            /* Bad type ! */
}
TypeNature;

/* Type : type linked to an identifier */
typedef struct Type
{
    TypeNature nature;
    union
    {
        struct
        {
            PtrBindingList enumBindings; /* Enumeration value bindings */
        }
        enumeration;
        struct
        {
            struct InstanceList *recordElements; /* Record elements in a list high->lower->lowest->NULL offsets */
            Bits unpaddedLength; /* Length without the pad field */
        }
        record;
        struct
        {
            struct Type *baseType; /* base type eg 4 bits in: variable : 4 bits [5] */
            Span range;         /* upper and lower bounds on elements, also index type 
                                   array has elements [lowerBound], [lowerBound+1] ... [upperBound] inclusive, lowerBound should be <= upperBound */
            unsigned elementCount; /* upperBound - lowerBound + 1 */
        }
        array;
        struct
        {
            struct Type *baseType; /* base type eg 4 bits in: variable : 4 bits [5] */
            struct InstanceList *arrayedElements; /* Elements of the arrayed type */
            Span range;         /* range of elements */
            unsigned elementCount; /* upperBound - lowerBound + 1 */
        }
        arrayed;
    }
    info;                       /* Type nature varient information */
    SignedBits size;            /* Number of bits in representation (and signedness if relevant) */
    struct Type *aliasing;      /* set if this is a type alias for the type in `aliasing' */
    Scope scope;                /* declaration scope */
    bool hasBuiltinElements;    /* true if this type has some builtin elements and so has restricted uses */
    tIdent ident;               /* Name of type */
}
Type, *PtrType;

/* Declare a list form for Type */
DECLARE_CONS_LIST_TYPE (Type)
extern PtrType NoType;          /* Untypable/not expecting a type */
extern PtrType SyncTypeObj;     /* Only one type of sync object */
extern PtrType StringTypeObj;   /* Strings ! */
extern PtrType ContextMarkerTypeObj; /* Element to use as a context marker */
extern PtrType BitTypeObj;      /* 1 bits - result of inequalities */
extern PtrType ErrorTypeObj;    /* BalsaError, error reporting enum. */

/* NewNumericType : Create a numeric type */
extern PtrType NewNumericType (tIdent ident, SignedBits size);

/* NewEnumerationType : ditto de enums */
extern PtrType NewEnumerationType (tIdent ident, SignedBits size, PtrBindingList bindings);

/* NewRecordType : fill in a record type */
extern PtrType NewRecordType (tIdent ident, Bits size, struct InstanceList *elements, Bits unpaddedLength);

/* NewArrayType : create a array over type baseType (having elements indexed from range.lowerBound to 
   range.upperBound on the type range.boundingType) */
extern PtrType NewArrayType (tIdent ident, PtrType baseType, Span range);

/* NewArrayedType : create a arrayed object list over type baseType (having elements indexed from 
   range.lowerBound to range.upperBound on the type range.boundingType) */
extern PtrType NewArrayedType (PtrType baseType, Span range, struct Instance *modelInstance);

/* NewBuiltinType : make new builtin type */
extern PtrType NewBuiltinType (void);

/* NewElementInitialisedArrayedType : create a arrayed type using the given instance list as the
	elements list.  `elementCount' should reflect the exact length of `elements' (or of the local portion
	of that list with a ContextMarkerInstance at ref. `elementCount') */
extern PtrType NewElementInitialisedArrayedType (tIdent ident, PtrType baseType, struct InstanceList *elements, unsigned elementCount);

/* CopyAnonType : copy a type, discarding it's name but sharing any linked structures within it (eg.
   record/enum elements list */
extern PtrType CopyAnonType (PtrType type);

/* AliasType: like CopyAnonType but name the new type and make it an alias of the old type */
extern PtrType AliasType (PtrType type, tIdent ident);

/* LookupEnumElemValue : lookup an enumeration element member, returns its value or NULL on failure */
extern PtrMP_INT LookupEnumElemValue (PtrType type, tIdent member);

/* LookupEnumElemNameByValue : lookup an enumeration element by value, returns the first
   matching element name or NoIdent on failure */
extern tIdent LookupEnumElemNameByValue (PtrType type, PtrMP_INT value);

/* TypeEquivalence : returns true if the 2 given types are equivalent, for
   numeric types this is by size and signedness, for other types it's
   a comparison of pointers (it's important not to copy types!), using
   the pointer is safe than using name-name equivalence as pointers are 
   unique w.r.t scope BTW. NULL pointers are equivalent ;) (array types are compared for
   baseType and size (number of elements) */
extern bool TypeEquivalence (PtrType typeA, PtrType typeB);

/* SubscriptTypeIsOK : returns true if the two given types are the same or if the the reqdType is
   numeric and the given boundingType is narrower or equal to the reqdType */
extern bool SubscriptTypeIsOK (PtrType reqdType, PtrType actualType);

/* RangeOfType : returns a span representing the range of values valid for this type,
   for numeric or enumerated types this is either [0,(2^n)-1] or [-2^(n-1),(2^(n-1))-1], for other classes of
   types [0,n-1] is returned where n = 2^type->size */
extern Span RangeOfType (PtrType type);

/* TypeIsInTypeList : returns true if the given type is present in the given type list,
	probably should do this with a generalised search function in lists.h */
extern bool TypeIsInTypeList (PtrType type, PtrTypeList types);

/* StrPtrTypeName displays the name of non-numeric types and [signed] <num> bits for numeric types */
extern void StrPtrTypeName (FILE * stream, PtrType type);

/* MakeTypeNameString : like StrPtrTypeName but into a string */
extern Ptrchar MakeTypeNameString (Ptrchar str, PtrType type);

/* StrPtrSBreeze{Type(List)?} : Print out types/type descriptions in the SBreeze format.
	If `printName' is true then print a (type "name" ...) wrapper around the type.
	If `expandToTopLevel' is true then only print subtype names of types which aren't
	ImportedScope or TopLevelScope declarations */
extern void StrPtrSBreezeType (FILE * stream, PtrType type, bool printName, bool expandToTopLevel);

/* StrPtrSBreezeTypeName displays the name of non-numeric types and [signed] <num> bits for numeric types */
extern void StrPtrSBreezeTypeName (FILE * stream, PtrType type);
extern void StrPtrSBreezeTypeList (FILE * stream, PtrTypeList types, bool onlyPrintLocals, Scope scopes, char *separator);

/* StrPtrBreezeTopLevelType : print out either the name of a type (for types with names and which are
	visible at the top level) or the type definition body for types without names (or not ...) */
extern void StrPtrBreezeTopLevelType (FILE * stream, PtrType type);

#endif /* TYPES_HEADER */
