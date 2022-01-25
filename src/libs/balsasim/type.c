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

#include "type.h"
#include "list.h"
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include "format/dictionary.h"
#include "format/external.h"
#include "balsasim/bstring.h"

/* NewBalsaType : make a new type with some obvious initialisations */
BalsaType *NewBalsaType (BalsaTypeNature nature, const char *name, int size)
{
    BalsaType *ret = malloc (sizeof (BalsaType));

    ret->nature = nature;
    ret->name = (name ? strdup (name) : NULL);
    ret->size = size;
    ret->format = NULL;

    return ret;
}

/* BalsaTypeAlias : make an alias type for an existing (usually named) type */
BalsaType *BalsaTypeAlias (const char *newName, BalsaType * type)
{
    BalsaType *ret = NewBalsaType (BalsaAliasType, newName, type->size);

    ret->info.alias.type = type;

    return ret;
}

/* DeleteBalsaType : type destructor, if deleteSubTypes is true then delete
	all bounding/element/record element sub types of this type */
void DeleteBalsaType (BalsaType * type, bool deleteSubTypes)
{
    int i;

    if (!type)
        return;

    switch (type->nature)
    {
    case BalsaEnumerationType:
        for (i = 0; i < type->info.enumeration.elementCount; i++)
        {
            free (type->info.enumeration.elementNames[i]);
            DeleteFormatData (type->info.enumeration.elementValues[i]);
        }
        free (type->info.enumeration.elementNames);
        free (type->info.enumeration.elementValues);
        break;
    case BalsaRecordType:
        for (i = 0; i < type->info.record.elementCount; i++)
        {
            free (type->info.record.elementNames[i]);
            if (deleteSubTypes)
                DeleteBalsaType (type->info.record.elementTypes[i], true);
        }
        free (type->info.record.elementNames);
        free (type->info.record.elementTypes);
        break;
    case BalsaArrayType:
        if (deleteSubTypes)
        {
            DeleteBalsaType (type->info.array.elementType, true);
            DeleteBalsaType (type->info.array.boundingType, true);
        }
        break;
    case BalsaAliasType:
        if (deleteSubTypes)
            DeleteBalsaType (type->info.alias.type, true);
        break;
    default:
        break;
    }

    if (type->name)
        free (type->name);
    free (type);
}

/* SkipWS : skip leading whitespace */
static const char *SkipWS (const char *string)
{
    while (isspace (*string))
        string++;

    return string;
}

/* SymbolLength : returns the number of characters (including the one at *string)
	which make up the next symbol in the given string.  A symbol here is
	[-#:alpha:_0-9][^:whitespace:()"].  Returns 0 if *string isn't a valid first character */
static unsigned SymbolLength (const char *string)
{
    const char *stringPtr = string;

    if (strchr ("#_-", *stringPtr) || isalnum (*stringPtr))
    {
        stringPtr++;

        while (*stringPtr && !isspace (*stringPtr) && strchr ("()\"", *stringPtr) == NULL)
            stringPtr++;
    }

    return (unsigned) (stringPtr - string);
}

/* SymbolEqual : symbol1 and symbol2 contain the same string for the first MIN (strlen (symbol2),
	symbol2Length) characters and strlen (symbol2) must equal symbol2 length. Usually called
	with SymbolEqual (someBuffer, "a-fixed-symbol", significantCharCountInBuffer) */
static bool SymbolEqual (const char *symbol1, const char *symbol2, unsigned symbol1Length)
{
    unsigned symbol2Length = strlen (symbol2);

    return symbol2Length == symbol1Length && strncmp (symbol1, symbol2, symbol1Length) == 0;
}

#define ABS(integer) (integer < 0 ? - integer : integer)

/* BalsaTypeParseFromString : parse a Balsa type in the Breeze format
	from a string skipping whitespace.  Returns the pointer to the first
	character beyond the end of the first type defn. in the string or NULL
	on error, puts it's result in *type only on success.
	index is used for error reporting and should usually be set to 0 */
const char *BalsaTypeParseFromString (const char *string, unsigned index, BalsaType ** type)
{
    const char *stringPtr = SkipWS (string);
    const char *newStringPtr;
    BalsaType *ret = NULL;
    unsigned symbolLength;

#define EXPECTING(thing) do { \
	fprintf (stderr, "%s:%d: expecting " thing " at `%s' index %d (%c)\n", \
		__func__, __LINE__, string, (int) (stringPtr - string), *stringPtr); \
		return NULL; \
	} while (0)

    stringPtr = SkipWS (stringPtr);
    if (*stringPtr == '(')      /* ) */
    {
        const char *symbolPtr = stringPtr + 1;
        bool isAlias;

        symbolLength = SymbolLength (symbolPtr);
        stringPtr = SkipWS (symbolPtr + symbolLength);

        isAlias = SymbolEqual (symbolPtr, "alias-type", symbolLength);

        if (isAlias || SymbolEqual (symbolPtr, "named-type", symbolLength))
        {
            char *typeName;

            if (*stringPtr != '"')
                EXPECTING ("`\"'");
            stringPtr++;
            symbolLength = SymbolLength (stringPtr);
            typeName = malloc (symbolLength + 1);
            strncpy (typeName, stringPtr, symbolLength);
            typeName[symbolLength] = '\0';
            stringPtr += symbolLength;
            if (*stringPtr != '"')
                EXPECTING ("`\"'");
            stringPtr = SkipWS (stringPtr + 1);

            ret = BalsaLookupInternedType (typeName);

            if (!ret)
            {
                stringPtr -= symbolLength + 1; /* Report error at start of name */
                EXPECTING ("valid type name");
            }

            if (isAlias)
                ret = BalsaTypeAlias (NULL, ret);

            free (typeName);
        } else if (SymbolEqual (symbolPtr, "numeric-type", symbolLength))
        {
            symbolLength = SymbolLength (stringPtr);
            bool isSigned = SymbolEqual (stringPtr, "#t", symbolLength);
            unsigned length;

            if (!isSigned && !SymbolEqual (stringPtr, "#f", symbolLength))
                EXPECTING ("signedness");
            stringPtr = SkipWS (stringPtr + symbolLength);

            symbolLength = SymbolLength (stringPtr);
            if (!isdigit (*stringPtr))
                EXPECTING ("bitwise type length");
            length = strtoul (stringPtr, NULL, 10);
            stringPtr = stringPtr + symbolLength;

            ret = NewBalsaType (BalsaNumericType, NULL, (isSigned ? -length : length));
        } else if (SymbolEqual (symbolPtr, "enumeration-type", symbolLength))
        {
            symbolLength = SymbolLength (stringPtr);
            bool isSigned = SymbolEqual (stringPtr, "#t", symbolLength);
            unsigned length;
            BalsaList *elementNames = NULL;
            BalsaList *elementValues = NULL;

            if (!isSigned && !SymbolEqual (stringPtr, "#f", symbolLength))
                EXPECTING ("signedness");
            stringPtr = SkipWS (stringPtr + symbolLength);

            symbolLength = SymbolLength (stringPtr);
            if (!isdigit (*stringPtr))
                EXPECTING ("bitwise type length");
            length = strtoul (stringPtr, NULL, 10);
            stringPtr = SkipWS (stringPtr + symbolLength);

            ret = NewBalsaType (BalsaEnumerationType, NULL, (isSigned ? -length : length));

            while (*stringPtr != ')')
            {
                bool elementIsNegate = false;
                char *elementName;
                FormatData *elementValue;

                if (*stringPtr != '(')
                    EXPECTING ("`('");
                stringPtr = SkipWS (stringPtr + 1);

                if (*stringPtr != '"')
                    EXPECTING ("`\"'");
                stringPtr++;
                symbolLength = SymbolLength (stringPtr);
                if (!(isalnum (*stringPtr) || *stringPtr == '_'))
                    EXPECTING ("element name");
                elementName = malloc (symbolLength + 1);
                strncpy (elementName, stringPtr, symbolLength);
                elementName[symbolLength] = '\0';
                stringPtr += symbolLength;
                if (*stringPtr != '"')
                    EXPECTING ("`\"'");
                stringPtr = SkipWS (stringPtr + 1);

                /* Parse a multi-precision (possibly negative) integer */
                if (*stringPtr == '-')
                {
                    elementIsNegate = true;
                    stringPtr++;
                }
                symbolLength = SymbolLength (stringPtr);
                elementValue = FormatDataParseUInt (stringPtr, 10);
                if (!elementValue)
                    EXPECTING ("element type");
                if (elementIsNegate)
                {
                    FormatData *negElementValue = elementValue;

                    elementValue = FormatDataNegate (negElementValue, length);
                    DeleteFormatData (negElementValue);
                }
                stringPtr = SkipWS (stringPtr + symbolLength);

                elementNames = NewBalsaList (elementName, elementNames);
                elementValues = NewBalsaList (elementValue, elementValues);

                if (*stringPtr != ')')
                    EXPECTING ("`)'");
                stringPtr = SkipWS (stringPtr + 1);
            }

            elementNames = BalsaListReverse (elementNames);
            elementValues = BalsaListReverse (elementValues);

            ret->info.enumeration.elementNames = (char **) BalsaListToArray (elementNames, &(ret->info.enumeration.elementCount));
            ret->info.enumeration.elementValues = (FormatData **) BalsaListToArray (elementValues, NULL);

            BalsaListDelete (elementNames);
            BalsaListDelete (elementValues);

        } else if (SymbolEqual (symbolPtr, "record-type", symbolLength))
        {
            unsigned length;
            BalsaList *elementNames = NULL;
            BalsaList *elementTypes = NULL;

            symbolLength = SymbolLength (stringPtr);
            if (!isdigit (*stringPtr))
                EXPECTING ("bitwise type length");
            length = strtoul (stringPtr, NULL, 10);
            stringPtr = SkipWS (stringPtr + symbolLength);

            ret = NewBalsaType (BalsaRecordType, NULL, length);

            while (*stringPtr != ')')
            {
                char *elementName;
                BalsaType *elementType;

                if (*stringPtr != '(')
                    EXPECTING ("`('");
                stringPtr = SkipWS (stringPtr + 1);

                if (*stringPtr != '"')
                    EXPECTING ("`\"'");
                stringPtr++;
                symbolLength = SymbolLength (stringPtr);
                if (!(isalnum (*stringPtr) || *stringPtr == '_'))
                    EXPECTING ("element name");
                elementName = malloc (symbolLength + 1);
                strncpy (elementName, stringPtr, symbolLength);
                elementName[symbolLength] = '\0';
                stringPtr += symbolLength;
                if (*stringPtr != '"')
                    EXPECTING ("`\"'");
                stringPtr = SkipWS (stringPtr + 1);

                newStringPtr = BalsaTypeParseFromString (stringPtr, index + (unsigned) (stringPtr - string), &elementType);
                if (!newStringPtr)
                    EXPECTING ("element type");
                stringPtr = SkipWS (newStringPtr);

                elementNames = NewBalsaList (elementName, elementNames);
                elementTypes = NewBalsaList (elementType, elementTypes);

                if (*stringPtr != ')')
                    EXPECTING ("`)'");
                stringPtr = SkipWS (stringPtr + 1);
            }
            elementNames = BalsaListReverse (elementNames);
            elementTypes = BalsaListReverse (elementTypes);

            ret->info.record.elementNames = (char **) BalsaListToArray (elementNames, &(ret->info.record.elementCount));
            ret->info.record.elementTypes = (BalsaType **) BalsaListToArray (elementTypes, NULL);

            BalsaListDelete (elementNames);
            BalsaListDelete (elementTypes);
        } else if (SymbolEqual (symbolPtr, "array-type", symbolLength))
        {
            BalsaType *elementType;
            BalsaType *boundingType;
            int lowIndex;
            unsigned elementCount;

            newStringPtr = BalsaTypeParseFromString (stringPtr, index + (unsigned) (stringPtr - string), &elementType);
            if (!newStringPtr)
                EXPECTING ("element type");
            stringPtr = SkipWS (newStringPtr);

            symbolLength = SymbolLength (stringPtr);
            if (!isdigit (*stringPtr) && *stringPtr != '-')
                EXPECTING ("low index");
            lowIndex = strtoul (stringPtr, NULL, 10);
            stringPtr = SkipWS (stringPtr + symbolLength);

            symbolLength = SymbolLength (stringPtr);
            if (!isdigit (*stringPtr))
                EXPECTING ("element count");
            elementCount = strtoul (stringPtr, NULL, 10);
            stringPtr = SkipWS (stringPtr + symbolLength);

            ret = NewBalsaType (BalsaArrayType, NULL, elementCount * ABS (elementType->size));
            ret->info.array.elementCount = elementCount;
            ret->info.array.elementType = elementType;
            ret->info.array.boundingType = boundingType;
            ret->info.array.lowIndex = lowIndex;
        } else if (SymbolEqual (symbolPtr, "builtin-type", symbolLength))
        {
            ret = NewBalsaType (BalsaBuiltinType, NULL, BALSA_OBJECT_BITS);
        } else
        {
            stringPtr = symbolPtr;
            EXPECTING ("*-type");
        }

        /* ( */ if (*stringPtr != ')')
            /* ( */ EXPECTING ("`)'");
        stringPtr++;
    } else
        EXPECTING ("`('");      /* ) */

#undef EXPECTING

    if (type)
        *type = ret;
    return stringPtr;
}

/* PrintBalsaType : print a BalsaType in the Breeze format,
	if the type has a name then print (named-type "name") unless expandDepth is non 0
	in which case structure of subtypes upto depth `expandDepth' (the given type is
	at depth 1) are shown rather than the names.  If expandDepth is -1 then the
	type is fully expanded */
void PrintBalsaType (FILE * stream, const BalsaType * type, int expandDepth)
{
    if (!type)
        fprintf (stream, "(null)");
    else
    {
        int subTypeDepth = (expandDepth > 0 ? expandDepth - 1 : expandDepth);

        putc ('(', stream);

        if (type->name && expandDepth == 0)
            fprintf (stream, "named-type \"%s\"", type->name);
        else
        {
            switch (type->nature)
            {
            case BalsaNumericType:
                fprintf (stream, "numeric-type #%c %u", (type->size < 0 ? 't' : 'f'), ABS (type->size));
                break;
            case BalsaEnumerationType:
                {
                    int i;
                    unsigned elementCount = type->info.enumeration.elementCount;
                    unsigned typeBitCount = ABS (type->size);

                    fprintf (stream, "enumeration-type #%c %u", (type->size < 0 ? 't' : 'f'), typeBitCount);

                    for (i = 0; i < elementCount; i++)
                    {
                        FormatData *elementValue = type->info.enumeration.elementValues[i];
                        int signBit = (type->size < 0 ? FormatDataExtractBit (elementValue,
                            typeBitCount - 1) : 0);
                        unsigned elementValueLength = FormatDataLengthInRadix (elementValue->wordCount, 10);
                        char *buffer = malloc (elementValueLength + 1);

                        buffer[elementValueLength] = '\0';
                        fprintf (stream, " (\"%s\" ", type->info.enumeration.elementNames[i]);

                        if (signBit) /* Correct the bitField if we're signed */
                        {
                            FormatData *negatedValue = FormatDataNegate (elementValue, typeBitCount);

                            putc ('-', stream);
                            FormatDataFormatAsUInt (buffer, negatedValue, 10, 0);
                            DeleteFormatData (negatedValue);
                        } else
                        {
                            FormatDataFormatAsUInt (buffer, elementValue, 10, 0);
                        }
                        fprintf (stream, "%s)", buffer);
                        free (buffer);
                    }

                    putc (')', stream);
                }
                break;
            case BalsaRecordType:
                {
                    int i;
                    unsigned elementCount = type->info.record.elementCount;

                    fprintf (stream, "record-type %u", ABS (type->size));

                    for (i = 0; i < elementCount; i++)
                    {
                        fprintf (stream, " (\"%s\" ", type->info.record.elementNames[i]);
                        PrintBalsaType (stream, type->info.record.elementTypes[i], subTypeDepth);
                        putc (')', stream);
                    }

                    putc (')', stream);
                }
                break;
            case BalsaArrayType:
                fprintf (stream, "array-type ");
                PrintBalsaType (stream, type->info.array.elementType, subTypeDepth);
                fprintf (stream, " %d %u ", type->info.array.lowIndex, type->info.array.elementCount);
                PrintBalsaType (stream, type->info.array.boundingType, subTypeDepth);
                break;
            case BalsaBuiltinType:
                fprintf (stream, "builtin-type");
                break;
            case BalsaAliasType:
                fprintf (stream, "alias-type \"%s\"", type->info.alias.type->name);
                break;
            }
        }

        putc (')', stream);
    }
}

static FormatDictionary *BalsaInternedTypes = NULL;

/* BalsaInternType : make a type known by a particular name (the ->name of the given type) */
void BalsaInternType (BalsaType * type)
{
    if (!BalsaInternedTypes)
        BalsaInternedTypes = NewFormatDictionary ();

    if (type && type->name)
        FormatDictionaryInsert (BalsaInternedTypes, type->name, type);
    else
        fprintf (stderr, "BalsaInternType: can't intern NULL type, or type with empty name\n");
}

/* BalsaLookupInternedType : lookup an interned type by name */
BalsaType *BalsaLookupInternedType (const char *name)
{
    return (BalsaType *) FormatDictionaryLookup (BalsaInternedTypes, name);

}

/* BalsaTypeDefaultFormatLength : number of format elements required for the given type */
static unsigned BalsaTypeDefaultFormatLength (const BalsaType * type)
{
    unsigned ret = 0;

    if (type->format)
        ret = FormatElementCount (type->format);
    else
    {
        switch (type->nature)
        {
        case BalsaNumericType:
            ret = 1;            /* Change this if the radix changes */
            break;
        case BalsaEnumerationType:
            ret = 1;
            break;
        case BalsaRecordType:
            /* "{" "elemType0" "," ... "elemTypeN-1" "}" */
            ret = 1;
            int i;

            for (i = 0; i < type->info.record.elementCount; i++)
                ret += 1 + BalsaTypeDefaultFormatLength (type->info.record.elementTypes[i]);
            break;
        case BalsaArrayType:
            /* "{" "elem" "," ... "elem" "}" */
            ret = 1 + (1 + BalsaTypeDefaultFormatLength (type->info.array.elementType)) * type->info.array.elementCount;
            break;
        case BalsaAliasType:
            ret += BalsaTypeDefaultFormatLength (type->info.alias.type);
            break;
        case BalsaBuiltinType:
            /* Special case for strings */
            if (strcmp (type->name, "String") == 0)
                ret = 1;
            else
                ret = 2;
            break;
        }
    }

    return ret;
}

/* BalsaObjectStringGetCharPtr : get a plain char * from a BalsaObject which contains a BalsaString */
static char *BalsaObjectStringGetCharPtr (BalsaObject * object)
{
    char *ret = NULL;
    BalsaString *string = (object ? BALSA_STRING (object->data) : NULL);

    if (string)
        ret = string->string;

    return ret;
}

/* BalsaObjectStringPutBalsaString : replace a string in the given object with a new string */
static BalsaObject *BalsaObjectStringPutBalsaString (BalsaObject * object, char *string)
{
    if (!object)
    {
        fprintf (stderr, "BalsaObjectStringPutBalsaString: target object is not allocated\n");
        exit (EXIT_FAILURE);
    }

    if (object->data)
        BalsaStringUnref (object->data);

    object->data = NewBalsaString (string, -1);

    return object;
}

/* BalsaTypeMakeDefaultFormatBody : body function of BalsaTypeMakeDefaultFormat, make
	format into a preallocated array (format) */
static void BalsaTypeMakeDefaultFormatBody (const BalsaType * type, FormatElement * format)
{
    int i;
    FormatPosition position;

    position.offset = 0;
    position.bitCount = ABS (type->size); /* usual case for whole type printing */

    if (type->format)           /* Copy the elements of the format rather than making a new one.  This is
                                   really just a fallback in case this function is called when type->format is already set
                                   to allow a new format to be created */
    {
        format = CopyFormat (format, type->format, 0);
    } else
    {
        switch (type->nature)
        {
        case BalsaNumericType:
            NewFormatNumber (format, position);
            format->info.number.showSign = type->size < 0;
            format++;
            break;
        case BalsaEnumerationType:
            {
                unsigned elementCount = type->info.enumeration.elementCount;
                FormatEnumerationElement *elements = malloc (sizeof (FormatEnumerationElement) * elementCount);
                char **elementNames = type->info.enumeration.elementNames;
                FormatData **elementValues = type->info.enumeration.elementValues;

                for (i = 0; i < elementCount; i++)
                {
                    elements[i].name = strdup (elementNames[i]);
                    elements[i].value = CopyFormatData (elementValues[i]);
                }
                NewFormatEnumeration (format, position, elementCount, elements);
                format++;
            }
            break;
        case BalsaRecordType:
            {
                unsigned elementCount = type->info.record.elementCount;
                unsigned offset = 0;

                /* "{" "elemType0" "," ... "elemTypeN-1" "}" */
                NewFormatLiteral (format, "{", 1);
                format++;

                for (i = 0; i < elementCount; i++)
                {
                    BalsaType *elementType = type->info.record.elementTypes[i];

                    BalsaTypeMakeDefaultFormat (elementType);
                    format = CopyFormat (format, elementType->format, offset);

                    offset += ABS (elementType->size);

                    NewFormatLiteral (format, (i == elementCount - 1 ? "}" : ","), 1);
                    format++;
                }
            }
            break;
        case BalsaArrayType:
            {
                unsigned elementCount = type->info.array.elementCount;
                unsigned offset = 0;
                BalsaType *elementType = type->info.array.elementType;

                /* "{" "elem" "," ... "elem" "}" */
                NewFormatLiteral (format, "{", 1);
                format++;

                BalsaTypeMakeDefaultFormat (elementType);

                for (i = 0; i < elementCount; i++)
                {
                    format = CopyFormat (format, elementType->format, offset);
                    offset += ABS (elementType->size);

                    NewFormatLiteral (format, (i == elementCount - 1 ? "}" : ","), 1);
                    format++;
                }
            }
            break;
        case BalsaAliasType:
            BalsaTypeMakeDefaultFormat (type->info.alias.type);
            format = CopyFormat (format, type->info.alias.type->format, 0);
            break;
        case BalsaBuiltinType:
            /* Special case for strings */
            if (strcmp (type->name, "String") == 0)
            {
                NewFormatString (format, position, '\"',
                  (FormatGetFunction) BalsaObjectStringGetCharPtr, (FormatPutFunction) BalsaObjectStringPutBalsaString);
                format++;
            } else
            {
                NewFormatLiteral (format, "builtin: ", 0);
                format++;
                NewFormatNumber (format, position);
                format++;
            }
            break;
        default:
            break;
        }
    }
}

/* BalsaTypeMakeDefaultFormat : make a default printing format for the given type,
	this can then be used to print values of that type.  Returns the default format
	in type->format. */
void BalsaTypeMakeDefaultFormat (BalsaType * type)
{
    if (type && !type->format)
    {
        Format newFormat = NewFormat (BalsaTypeDefaultFormatLength (type));

        BalsaTypeMakeDefaultFormatBody (type, newFormat);
        type->format = newFormat;
    }
}

static FormatRadixData **BalsaRadices = NULL;

/* BalsaRadixChoiceFunction : radix choice function for Balsa radix representations */
FormatRadixData **BalsaRadixChoiceFunction (int radix, void *data)
{
    if (!BalsaRadices)
    {
        BalsaRadices = malloc (5 * sizeof (FormatRadixData *));
        BalsaRadices[0] = NewFormatRadixData (10, "", "", 0, 0);
        BalsaRadices[1] = NewFormatRadixData (8, "0", "", 0, 0);
        BalsaRadices[2] = NewFormatRadixData (2, "0b", "", 0, 0);
        BalsaRadices[3] = NewFormatRadixData (16, "0x", "", 0, 0);
        BalsaRadices[4] = NULL;
    }

    BalsaRadices[0]->radix = (radix == 0 ? 10 : radix); /* Default radix entry */

    return BalsaRadices;
}

/* BalsaTypeBuiltinCount : returns the number of builtin-type typed objects in the type `type' */
unsigned BalsaTypeBuiltinCount (const BalsaType * type)
{
    unsigned ret = 0;
    int i;

    if (type)
    {
        switch (type->nature)
        {
        case BalsaEnumerationType:
        case BalsaNumericType:
            /* 0 builtins */
            break;
        case BalsaBuiltinType:
            ret = 1;
            break;
        case BalsaRecordType:
            for (i = 0; i < type->info.record.elementCount; i++)
                ret += BalsaTypeBuiltinCount (type->info.record.elementTypes[i]);
            break;
        case BalsaArrayType:
            ret = type->info.array.elementCount * BalsaTypeBuiltinCount (type->info.array.elementType);
            break;
        case BalsaAliasType:
            ret = BalsaTypeBuiltinCount (type->info.alias.type);
            break;
        }
    }
    return ret;
}

static BalsaObject **PackBalsaObjectsIntoFormatDataBody (FormatData * data, BalsaType * type, BalsaObject ** objects, unsigned offset)
{
    int i;

    if (type)
    {
        switch (type->nature)
        {
        case BalsaEnumerationType:
        case BalsaNumericType:
            break;
        case BalsaBuiltinType:
            FormatDataInsertPointer (data, *objects, offset, BALSA_OBJECT_BITS);
            objects++;
            break;
        case BalsaRecordType:
            for (i = 0; i < type->info.record.elementCount; i++)
            {
                BalsaType *elementType = type->info.record.elementTypes[i];

                objects = PackBalsaObjectsIntoFormatDataBody (data, elementType, objects, offset);
                offset += ABS (elementType->size);
            }
            break;
        case BalsaArrayType:
            for (i = 0; i < type->info.array.elementCount; i++)
            {
                BalsaType *elementType = type->info.array.elementType;

                objects = PackBalsaObjectsIntoFormatDataBody (data, elementType, objects, offset);
                offset += ABS (elementType->size);
            }
            break;
        case BalsaAliasType:
            objects = PackBalsaObjectsIntoFormatDataBody (data, type->info.alias.type, objects, offset);
            break;
        }
    }
    return objects;
}

/* BalsaTypePackBalsaObjectsIntoFormatData : pack pointers to the given BalsaObjects into the
	given FormatData at position corresponding to the builtin types in the given type.
	This can be used to initialise a FormatData to pass to FormatScan */
void BalsaTypePackBalsaObjectsIntoFormatData (FormatData * data, BalsaType * type, BalsaObject * objects[])
{
    PackBalsaObjectsIntoFormatDataBody (data, type, objects, 0);
}

/* PrintBalsaTypeDataBody : body function for PrintBalsaTypeData, this accumulates the strings
	in the given type in the array `strings', with their bit offsets in `offsets'.  Returns the
	number of discovered strings (so filling strings and offsets up to offset return-value - 1) and
	also clears the bits in `fd' which contain the pointers to the type's strings */
static int
PrintBalsaTypeDataBody (const BalsaType * type, FormatData * fd, unsigned offset, unsigned stringCount, BalsaString ** strings, unsigned *offsets)
{
    if (type)
    {
        switch (type->nature)
        {
        case BalsaNumericType:
        case BalsaEnumerationType:
            break;
        case BalsaRecordType:
            {
                int i;
                unsigned elementCount = type->info.record.elementCount;
                unsigned endOffset = offset + ABS (type->size);

                for (i = 0; i < elementCount; i++)
                {
                    BalsaType *elementType = type->info.record.elementTypes[i];

                    stringCount = PrintBalsaTypeDataBody (elementType, fd, offset, stringCount, strings, offsets);
                    offset += ABS (elementType->size);
                }
                offset = endOffset; /* Need to do this incase the record is padded */
            }
            break;
        case BalsaArrayType:
            {
                int i;
                unsigned elementCount = type->info.record.elementCount;

                for (i = 0; i < elementCount; i++)
                {
                    BalsaType *elementType = type->info.array.elementType;

                    stringCount = PrintBalsaTypeDataBody (elementType, fd, offset, stringCount, strings, offsets);
                    offset += ABS (elementType->size);
                }
            }
            break;
        case BalsaBuiltinType:
            {
                if (strcmp (type->name, "String") == 0)
                {
                    BalsaString *str1 = BALSA_STRING (FormatDataGetBalsaObject (fd, offset)->data);

                    strings[stringCount] = str1;
                    offsets[stringCount] = offset;

                    // Clears in fd the bits corresponding to the object pointer
                    static FormatData *zeroObject = 0;

                    if (!zeroObject)
                        zeroObject = NewFormatData (FormatBitCountToWordCount (BALSA_OBJECT_BITS));
                    FormatDataInsertBitField (fd, zeroObject, offset, BALSA_OBJECT_BITS);

                    offset += BALSA_OBJECT_BITS;
                    stringCount++;
                } else
                {
                    char *str = malloc (strlen (type->name) + strlen ("(builtin \\\"\\\")") + 1);

                    sprintf (str, "(builtin \\\"%s\\\")", type->name);

                    BalsaString *str1 = NewBalsaString (str, -1);

                    strings[stringCount] = str1;
                    offsets[stringCount] = offset;

                    // Clears in fd the bits corresponding to the object pointer
                    static FormatData *zeroObject = 0;

                    if (!zeroObject)
                        zeroObject = NewFormatData (FormatBitCountToWordCount (BALSA_OBJECT_BITS));
                    FormatDataInsertBitField (fd, zeroObject, offset, BALSA_OBJECT_BITS);

                    offset += BALSA_OBJECT_BITS;
                    stringCount++;
                }
            }
            break;
        case BalsaAliasType:
            stringCount = PrintBalsaTypeDataBody (type->info.alias.type, fd, offset, stringCount, strings, offsets);
            break;
        }
    }
    return stringCount;
}

/* PrintBalsaTypeData : print a data value given its type definition, for use in hhh files */
void PrintBalsaTypeData (FILE * stream, const BalsaType * type, FormatData * fd)
{
    /* There are more builtin values than strings so it's safe to use arrays of this size */
    unsigned builtinCount = BalsaTypeBuiltinCount (type);
    BalsaString **strings = malloc (sizeof (BalsaString *) * builtinCount);
    unsigned *stringOffsets = malloc (sizeof (unsigned) * builtinCount);
    unsigned stringCount = PrintBalsaTypeDataBody (type, fd, 0, 0, strings, stringOffsets);
    char *valBuffer = malloc (FormatDataLengthInRadix (fd->wordCount, 16) + 1);
    int i;

    FormatDataFormatAsUInt (valBuffer, fd, 16, 0);
    fprintf (stream, "0x%s ", valBuffer);

    for (i = 0; i < stringCount; i++)
    {
        fprintf (stream, " (string %d \"", stringOffsets[i]); /* ) */
        FormatPrintEscapedString (stream, strings[i]->string, strings[i]->length, "\"\n");
        /* ( */ fprintf (stream, "\")");
    }

    free (stringOffsets);
    free (strings);
    free (valBuffer);
}

/* PrintBalsaTypeDataIntoNewString : print a data value given its type definition,
                                     in a newly allocated string, for use in hhh files */
char *PrintBalsaTypeDataIntoNewString (const BalsaType * type, FormatData * fd)
{
    /* There are more builtin values than strings so it's safe to use arrays of this size */
    unsigned builtinCount = BalsaTypeBuiltinCount (type);
    BalsaString **strings = malloc (sizeof (BalsaString *) * builtinCount);
    unsigned *stringOffsets = malloc (sizeof (unsigned) * builtinCount);
    unsigned stringCount = PrintBalsaTypeDataBody (type, fd, 0, 0, strings, stringOffsets);
    int valBufferLength = FormatDataLengthInRadix (fd->wordCount, 16);
    int length = 0;
    int i;

    length += 3 + valBufferLength;
    for (i = 0; i < stringCount; i++)
        length += 11 + 16 + 4 * strings[i]->length + 2;

    char *result = malloc (length + 1);
    char *ptr = result;

    strcpy (ptr, "0x");
    ptr += 2;

    FormatDataFormatAsUInt (ptr, fd, 16, 0);
    while (*ptr)
        ptr++;

    for (i = 0; i < stringCount; i++)
    {
        ptr += sprintf (ptr, " (string %d \"", stringOffsets[i]);
        ptr += FormatPrintEscapedStringToString (ptr, strings[i]->string, strings[i]->length, "\"\n");
        //      strncpy (ptr, strings[i]->string, strings[i]->length);
        //      ptr += strings[i]->length;
        strcpy (ptr, "\")");
        ptr += 2;
    }

    free (stringOffsets);
    free (strings);

    return result;
}
