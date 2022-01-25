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

	`type.c'
	Balsa type and parameter representation

*/

#ifndef BALSA_SIM_TYPE_H
#define BALSA_SIM_TYPE_H

#include "balsasim/object.h"
#include "format/format.h"
#include <stdbool.h>
#include <stdio.h>

typedef enum
{
    BalsaNumericType,
    BalsaEnumerationType,
    BalsaRecordType,
    BalsaArrayType,
    BalsaBuiltinType,
    BalsaAliasType
}
BalsaTypeNature;

/* BalsaType : Balsa representable types */
typedef struct BalsaType_
{
    BalsaTypeNature nature;
    char *name;                 /* Type name or NULL for anonymous type */
    int size;                   /* abs (size) is the size of this type, sign of this
                                   corresponds (for numeric and enumeration types) to the sign of this type */
    Format format;              /* Default printing format, usu. NULL */
    union
    {
        struct
        {
            unsigned elementCount; /* size of elementNames, elementValues arrays, must not be 0 */
            char **elementNames; /* elements are not necessarily ordered */
            FormatData **elementValues;
        }
        enumeration;
        struct
        {
            unsigned elementCount; /* size of elementNames, elementValues arrays, must not be 0 */
            char **elementNames; /* elementNames[0] is the name of the low-order element */
            struct BalsaType_ **elementTypes;
            unsigned unpaddedLength; /* total length of elements w/o pad field */
        }
        record;
        struct
        {
            unsigned elementCount; /* != 0 */
            struct BalsaType_ *elementType;
            struct BalsaType_ *boundingType; /* Type of indices */
            int lowIndex;       /* Array is [lowIndex .. lowIndex + elementCount - 1] */
        }
        array;
        struct
        {
            struct BalsaType_ *type;
        }
        alias;
    }
    info;
}
BalsaType;

/* NewBalsaType : make a new type with some obvious initialisations */
extern BalsaType *NewBalsaType (BalsaTypeNature nature, const char *name, int size);

/* DeleteBalsaType : type destructor, if deleteSubTypes is true then delete
	all bounding/element/record element sub types of this type */
extern void DeleteBalsaType (BalsaType * type, bool deleteSubTypes);

/* BalsaTypeAlias : make an alias type for an existing (usually named) type */
extern BalsaType *BalsaTypeAlias (const char *newName, BalsaType * type);

/* BalsaInternType : make a type known by a particular name (the ->name of the given type) */
extern void BalsaInternType (BalsaType * type);

/* BalsaLookupInternedType : lookup an interned type by name */
extern BalsaType *BalsaLookupInternedType (const char *name);

/* BalsaTypeParseFromString : parse a Balsa type in the Breeze format
	from a string skipping whitespace.  Returns the pointer to the first
	character beyond the end of the first type defn. in the string or NULL
	on error, puts it's result in *type only on success
	index is used for error reporting and should usually be set to 0 */
extern const char *BalsaTypeParseFromString (const char *string, unsigned index, BalsaType ** type);

/* PrintBalsaType : print a BalsaType in the Breeze format,
	if the type has a name then print (named-type "name") unless expandDepth is non 0
	in which case structure of subtypes upto depth `expandDepth' (the given type is
	at depth 1) are shown rather than the names.  If expandDepth is -1 then the
	type is fully expanded */
extern void PrintBalsaType (FILE * stream, const BalsaType * type, int expandDepth);

/* BalsaTypeMakeDefaultFormat : make a default printing format for the given type,
	this can then be used to print values of that type.  Returns the default format
	in type->format. */
extern void BalsaTypeMakeDefaultFormat (BalsaType * type);

/* BalsaRadixChoiceFunction : radix choice function for Balsa radix representations */
extern FormatRadixData **BalsaRadixChoiceFunction (int radix, void *data);

/* BalsaTypeBuiltinCount : returns the number of builtin-type typed objects in the type `type' */
extern unsigned BalsaTypeBuiltinCount (const BalsaType * type);

/* BalsaTypePackBalsaObjectsIntoFormatData : pack pointers to the given BalsaObjects into the
	given FormatData at position corresponding to the builtin types in the given type.
	This can be used to initialise a FormatData to pass to FormatScan */
extern void BalsaTypePackBalsaObjectsIntoFormatData (FormatData * data, BalsaType * type, BalsaObject * objects[]);

/* PrintBalsaTypeData : print a data value given its type definition, for use in hhh files */
extern void PrintBalsaTypeData (FILE * stream, const BalsaType * type, FormatData * fd);

/* PrintBalsaTypeDataIntoNewString : print a data value given its type definition,
                                     in a newly allocated string, for use in hhh files */
char *PrintBalsaTypeDataIntoNewString (const BalsaType * type, FormatData * fd);

#endif
