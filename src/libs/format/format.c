/*
	The Balsa Formatted Printing Library, as used in:
	  The Balsa Asynchronous Hardware Synthesis System
	  The GTKWave electronic waveform viewer
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

	`format.h'
	Value formatting from spec. structure

*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <math.h>

#include "format/format.h"

/* NewFormat: make a new format specification array with elements
	initialised to FORMAT_END */
FormatElement *NewFormat (unsigned length)
{
    unsigned i;
    FormatElement *ret = malloc (sizeof (FormatElement) * (length + 1));

    for (i = 0; i < length + 1; i++)
        ret[i].nature = FORMAT_END;

    return ret;
}

/* FormatElementCount : returns the number of elements (excluding trailing FORMAT_ENDs) in the
	given Format */
unsigned FormatElementCount (Format f)
{
    unsigned ret = 0;

    while (f->nature != FORMAT_END)
    {
        f++;
        ret++;
    }
    return ret;
}

/* InsertFormatElement: extend the format specification array by inserting an element initialised to FORMAT_END. The input format must contain a FORMAT_END *only* as a last element */
FormatElement *InsertFormatElement (Format f, unsigned newAtPos)
{
    unsigned length = FormatElementCount (f) + 1;
    FormatElement *ret = realloc (f, sizeof (FormatElement) * (length + 1));

    unsigned i = length;

    do
    {
        i--;
        ret[i + 1] = ret[i];
    }
    while (i > newAtPos);

    ret[newAtPos].nature = FORMAT_END;
    return ret;
}

/* RemoveFormatElement: shorten the format specification array by removing an element. The input format must contain a FORMAT_END *only* as a last element */
FormatElement *RemoveFormatElement (Format f, unsigned removeAtPos)
{
    unsigned length = FormatElementCount (f) + 1;
    FormatElement *ret = realloc (f, sizeof (FormatElement) * (length - 1));

    unsigned i;

    for (i = removeAtPos; i < length - 2; i++)
        ret[i] = ret[i + 1];

    ret[length - 2].nature = FORMAT_END;
    return ret;
}

/* NewFormat...: populate the pointed-to format element */
/* NewFormatLiteral: ... literal string. If length is 0 then whole string is copied,
	otherwise, length *must* be <= length of string */
void NewFormatLiteral (FormatElement * ret, const char *string, unsigned length)
{
    char *dupString;

    if (length <= 0)
        length = strlen (string);

    dupString = malloc (length + 1);
    strncpy (dupString, string, length);
    dupString[length] = '\0';

    ret->nature = FORMAT_LITERAL;
    ret->info.literal.string = dupString;
    ret->info.literal.length = length;

    ret->position.offset = 0;
    ret->position.bitCount = 0;
}

void NewFormatNumber (FormatElement * ret, FormatPosition position)
{
    ret->info.number.showSign = false;
    ret->info.number.showLeadingZeroes = false;
    ret->nature = FORMAT_NUMBER;
    ret->position = position;
}

void NewFormatEnumeration (FormatElement * ret, FormatPosition position, int elementCount, FormatEnumerationElement * elements)
{
    int i;

    ret->nature = FORMAT_ENUMERATION;
    ret->info.enumeration.elementCount = elementCount;
    ret->info.enumeration.elements = elements;
    ret->position = position;

    ret->info.enumeration.longestElementNameLength = 0;
    for (i = 0; i < elementCount; i++)
    {
        int elementNameLength = strlen (elements[i].name);

        if (elementNameLength > ret->info.enumeration.longestElementNameLength)
            ret->info.enumeration.longestElementNameLength = elementNameLength;
    }
}
void NewFormatString (FormatElement * ret, FormatPosition position, char quoteCharacter, FormatGetFunction get, FormatPutFunction put)
{
    ret->nature = FORMAT_STRING;
    ret->position = position;
    ret->info.string.get = get;
    ret->info.string.put = put;
    ret->info.string.userData = NULL;
    ret->info.string.quoteCharacter = quoteCharacter;
}

/* FormatElementLength: returns the length (of the formatted string) of the given format element. */
static unsigned FormatElementLength (FormatElement * element, FormatRadixChoiceFunction radixChoiceFunction, void *reservedData, FormatData * data)
{
    int ret = 0;

    switch (element->nature)
    {
    case FORMAT_LITERAL:
        ret = element->info.literal.length;
        break;
    case FORMAT_NUMBER:
        {
            FormatRadixData **radices = radixChoiceFunction (0, reservedData);
            FormatRadixData *radixData = *radices;

            ret = FormatDataLengthInRadix (FormatBitCountToWordCount (element->position.bitCount), radixData->radix) + 1; /* 1 for the sign */
            if (radixData->separatorSpacing != 0)
                ret += ((ret - 1) / radixData->separatorSpacing);
            if (radixData->prefix)
                ret += strlen (radixData->prefix);
            if (radixData->suffix)
                ret += strlen (radixData->suffix);
        }
        break;
    case FORMAT_ENUMERATION:
        ret = FormatDataLengthInRadix (FormatBitCountToWordCount (element->position.bitCount), 10);
        if (element->info.enumeration.longestElementNameLength > ret)
            ret = element->info.enumeration.longestElementNameLength;
        break;
    case FORMAT_STRING:
        {
            char *string = (element->info.string.get ? element->info.string.get (FormatDataExtractPointer (data,
                  element->position.offset,
                  element->position.bitCount),
                element->info.string.userData) : "");

            ret = strlen (string);
            if (element->info.string.quoteCharacter)
                ret += 2;
        }
        break;
    default:
        ret = 0;
        break;
    }

    return ret;
}

/* FormatLength: returns the required allocated space to format using `format',
	plus one for the '\0' */
unsigned FormatLength (Format format, FormatRadixChoiceFunction radixChoiceFunction, void *reservedData, FormatData * data)
{
    unsigned length = 1;

    while (format->nature != FORMAT_END)
    {
        length += FormatElementLength (format, radixChoiceFunction, reservedData, data);
        format++;
    }

    return length;
}

/* EndOfString: skip to end of string, returns pointer to the '\0' */
static char *EndOfString (char *string)
{
    while (*string)
        string++;

    return string;
}

static FormatRadixData *FormatBareDecimalData[2] = { NULL, NULL };

/* FormatBareDecimalRadixChoice : most boring of all possible radix choice functions */
FormatRadixData **FormatBareDecimalRadixChoice (int prefferedRadix, void *reservedData)
{
    if (!FormatBareDecimalData[0])
        FormatBareDecimalData[0] = NewFormatRadixData (10, NULL, NULL, 0, 0);

    return FormatBareDecimalData;
}

/* PrintedLengthInRadix : max number of printed characters for an unsigned number of
	`bitCount' bits in radix `radix'.  If `radix' is negative then the number is
	considered to be one bit narrower (or 1 bit if this is less).
*/
unsigned PrintedLengthInRadix (unsigned bitCount, int radix)
{
    if (bitCount == 0)
        bitCount = 1;

    if (radix < 0)
    {
        bitCount--;
        if (bitCount == 0)
            bitCount = 1;
        radix = -radix;
    }

    /* OK, we should now have a correct number of bits, the return value should
       be ceiling (log_radix (2 ^ bitCount)))
       = ceiling ((ln (2 ^ bitCount) / ln radix))

       = ceiling ((log2 (2 ^ bitCount) / log2 radix))
       = ceiling (bitCount / log2 radix)
       = ceiling (bitCount / (ln radix / ln 2))
       = ceiling ((ln 2 * bitCount) / ln radix)
       = ceiling (bitCount * (ln 2 / ln radix)
     */

    return ceil (bitCount * (log (2) / log (radix)));
}

/* FormatPrint: print to a string (which must be large enough), the data value `data'
	with format `format'. `reservedData' is passed to the radix choice function when
	called. */
void FormatPrint (char *buffer, Format format, FormatData * data, FormatRadixChoiceFunction radixChoiceFunction, void *reservedData)
{
    char *bufferPtr = buffer;

    while (format->nature != FORMAT_END)
    {
        switch (format->nature)
        {
        case FORMAT_LITERAL:
            strcpy (bufferPtr, format->info.literal.string);
            bufferPtr = EndOfString (bufferPtr);
            break;
        case FORMAT_NUMBER:
            {
                int isSigned = format->info.number.showSign;
                FormatData *bitField = FormatDataExtractBitField (data, format->position.offset,
                  format->position.bitCount);
                FormatRadixData *radixData = *(radixChoiceFunction (0, reservedData));
                int signBit = (isSigned ? FormatDataExtractBit (bitField,
                    format->position.bitCount - 1) : 0);
                bool withLeadingZeroes = format->info.number.showLeadingZeroes || radixData->showLeadingZeroes;
                unsigned length = PrintedLengthInRadix (format->position.bitCount,
                  (isSigned && radixData->radix >= 0 ? -radixData->radix : radixData->radix));
                char *endBufferPtr;

                if (signBit)    /* Correct the bitField if we're signed */
                {
                    FormatData *newBitField = FormatDataNegate (bitField, format->position.bitCount);

                    strcpy (bufferPtr, "-");
                    bufferPtr = EndOfString (bufferPtr);
                    DeleteFormatData (bitField);
                    bitField = newBitField;
                }

                if (radixData->prefix)
                {
                    strcpy (bufferPtr, radixData->prefix);
                    bufferPtr = EndOfString (bufferPtr);
                }
                endBufferPtr = FormatDataFormatAsUInt (bufferPtr, bitField,
                  (radixData->radix < 0 ? -radixData->radix : radixData->radix), (withLeadingZeroes ? length : 0));
                /* Insert separators by counting backwards in the result string */
                if (radixData->separatorSpacing != 0)
                {
                    unsigned numberLength = ((unsigned) (endBufferPtr - bufferPtr));
                    unsigned separatorCount = (numberLength - 1) / radixData->separatorSpacing;
                    unsigned distanceToSeparator = radixData->separatorSpacing;
                    char *oldEndOfNumber = endBufferPtr - 1;
                    char *newEndOfNumber = oldEndOfNumber + separatorCount;

                    endBufferPtr += separatorCount;

                    while (separatorCount != 0)
                    {
                        if (distanceToSeparator == 0)
                        {
                            *newEndOfNumber = '_';
                            separatorCount--;
                            distanceToSeparator = radixData->separatorSpacing;
                        } else
                        {
                            *newEndOfNumber = *oldEndOfNumber;
                            oldEndOfNumber--;
                            distanceToSeparator--;
                        }
                        newEndOfNumber--;
                    }
                }
                bufferPtr = endBufferPtr;
                if (radixData->suffix)
                {
                    strcpy (bufferPtr, radixData->suffix);
                    bufferPtr = EndOfString (bufferPtr);
                }
                DeleteFormatData (bitField);
            }
            break;
        case FORMAT_ENUMERATION:
            {
                int i;
                bool foundElement = false;
                FormatData *bitField = FormatDataExtractBitField (data, format->position.offset,
                  format->position.bitCount);

                for (i = 0; i < format->info.enumeration.elementCount && !foundElement; i++)
                {
                    FormatEnumerationElement *element = format->info.enumeration.elements + i;

                    if (FormatDataEqual (bitField, element->value))
                    {
                        strcpy (bufferPtr, element->name);
                        bufferPtr = EndOfString (bufferPtr);
                        foundElement = true;
                    }
                }
                if (!foundElement)
                    bufferPtr = FormatDataFormatAsUInt (bufferPtr, bitField, 10, 0);

                DeleteFormatData (bitField);
            }
            break;
        case FORMAT_STRING:
            {
                char quoteCharacter = format->info.string.quoteCharacter;
                char *string = (format->info.string.get ? format->info.string.get (FormatDataExtractPointer (data,
                      format->position.offset,
                      format->position.bitCount),
                    format->info.string.userData) : "");

                if (quoteCharacter)
                {
                    *bufferPtr = quoteCharacter;
                    bufferPtr++;
                }
                strcpy (bufferPtr, string);
                bufferPtr = EndOfString (bufferPtr);
                if (quoteCharacter)
                {
                    *bufferPtr = quoteCharacter;
                    bufferPtr++;
                }
            }
            break;
        default:
            break;
        }
        format++;
    }

    *bufferPtr = '\0';
}

/* SkipWS : skip leading whitespace in string, really should make this part of something common */
static char *SkipWS (char *string)
{
    while (isspace (*string))
        string++;

    return string;
}

/* NullTerminatedPointerArrayLength : returns the length of an array of pointers
	in which the end of the array is indicated by a NULL entry.  The length doesn't
	include the NULL */
static unsigned NullTerminatedPointerArrayLength (void **array)
{
    unsigned ret = 0;

    if (array)
    {
        while (*array)
        {
            ret++;
            array++;
        }
    }

    return ret;
}

/* DigitCharacters : valid digits for radices 2-36.  Presented in
	reverse order so that digit validity can be determined by
	strchr (DigitCharacters - radix, toupper (digitChar)) */
static char *DigitCharacters = "zyxwvutsrqponmlkjihgfedcba9876543210_" + 36;

/* FormatScan: scan a string to the preallocated data value `data' with format `format'.
	Returns the pointer (in buffer) to the next character after the scanned data
	or NULL on failure to scan the whole format.
	`reservedData' is passed to the radix choice function when called.
	A parsed enumeration will always use the longest match. */
char *FormatScan (char *buffer, Format format, FormatData * data, FormatRadixChoiceFunction radixChoiceFunction, void *reservedData)
{
    int itemCount = 0;
    char *bufferPtr = buffer;

    while (format->nature != FORMAT_END && bufferPtr)
    {
        bufferPtr = SkipWS (bufferPtr);

        switch (format->nature)
        {
        case FORMAT_LITERAL:
            if (strncmp (bufferPtr, format->info.literal.string, format->info.literal.length) != 0)
                bufferPtr = NULL;
            else
            {
                itemCount++;
                bufferPtr += format->info.literal.length;
            }
            break;
        case FORMAT_NUMBER:
            {
                FormatRadixData **radices = radixChoiceFunction (0, reservedData);
                unsigned radixCount = NullTerminatedPointerArrayLength ((void **) radices);
                bool isNeg = false;
                FormatData *fd = NULL;
                char *matchedValue;
                int i;

                if (*bufferPtr == '-')
                {
                    bufferPtr++;
                    isNeg = true;
                }

                i = radixCount - 1;
                matchedValue = NULL;
                /* Find which radices match the given patterns, in reverse order */
                while (i >= 0 && !matchedValue)
                {
                    if (radices[i]->prefix)
                    {
                        unsigned prefixLength = strlen (radices[i]->prefix);

                        if (strncmp (bufferPtr, radices[i]->prefix, prefixLength) == 0)
                            matchedValue = bufferPtr + prefixLength;
                    } else
                        matchedValue = bufferPtr;

                    if (matchedValue)
                    {
                        char *allowedCharacters = DigitCharacters - radices[i]->radix;
                        char *suffixPtr = matchedValue;
                        unsigned suffixLength = 0;

                        /* First character *must* be a valid digit */
                        if (*suffixPtr && strchr (allowedCharacters, tolower (*suffixPtr)))
                        {
                            suffixPtr++;

                            /* Find the suffix */
                            while (*suffixPtr && strchr (allowedCharacters, tolower (*suffixPtr)))
                                suffixPtr++;

                            if (radices[i]->suffix) /* Check the suffix */
                            {
                                suffixLength = strlen (radices[i]->suffix);

                                if (strncmp (suffixPtr, radices[i]->suffix, suffixLength) != 0)
                                    matchedValue = NULL;
                            }
                        } else  /* failed to find a single valid digit */
                            matchedValue = NULL;

                        if (matchedValue)
                        {
                            fd = FormatDataParseUIntWithBuf (&matchedValue, radices[i]->radix);
                            bufferPtr = suffixPtr + suffixLength;
                        }
                    }
                    i--;
                }

                if (!fd)
                    bufferPtr = NULL; /* Error indicator */
                else
                {
                    if (isNeg)
                    {
                        FormatData *tmp = FormatDataNegate (fd, format->position.bitCount);

                        DeleteFormatData (fd);
                        fd = tmp;
                    }
                    FormatDataInsertBitField (data, fd, format->position.offset, format->position.bitCount);
                    DeleteFormatData (fd);
                }
                itemCount++;
            }
            break;
        case FORMAT_ENUMERATION:
            {
                int i;
                FormatEnumerationElement *match = NULL;
                unsigned matchLength = 0;

                for (i = 0; i < format->info.enumeration.elementCount; i++)
                {
                    FormatEnumerationElement *elt = &format->info.enumeration.elements[i];
                    unsigned length = strlen (elt->name);

                    if (strncmp (bufferPtr, elt->name, length) == 0)
                    {
                        if (length > matchLength)
                        {
                            match = elt;
                            matchLength = length;
                        }
                    }
                }

                if (match)
                {
                    FormatDataInsertBitField (data, match->value, format->position.offset, format->position.bitCount);
                    bufferPtr += matchLength;
                    itemCount++;
                } else
                    bufferPtr = NULL;
            }
            break;
        case FORMAT_STRING:
            {
                char *endOfString = NULL;
                char quoteCharacter = format->info.string.quoteCharacter;
                void *retPtr = FormatDataExtractPointer (data, format->position.offset,
                  format->position.bitCount);

                if (quoteCharacter != '\0')
                {
                    if (*bufferPtr == quoteCharacter)
                    {
                        bufferPtr++;
                        endOfString = bufferPtr;

                        /* String will be quote delimited */
                        while (*endOfString && *endOfString != quoteCharacter)
                            endOfString++;

                        if (*endOfString != quoteCharacter)
                            endOfString = NULL;
                    }
                } else
                {
                    endOfString = bufferPtr;

                    /* String will be space delimited */
                    while (*endOfString && !isspace (*endOfString))
                        endOfString++;
                }

                /* endOfString will be NULL on error or point to just after the last/the close quote
                   character of the string being parsed */
                if (endOfString)
                {
                    unsigned retLength = (unsigned) (endOfString - bufferPtr);
                    char *retString = malloc (retLength + 1);

                    strncpy (retString, bufferPtr, retLength);
                    retString[retLength] = '\0';
                    if (format->info.string.put)
                        retPtr = format->info.string.put (retPtr, retString, format->info.string.userData);
                    free (retString);

                    bufferPtr = endOfString + (quoteCharacter == '\0' ? 0 : 1);
                } else
                {
                    if (format->info.string.put)
                        retPtr = format->info.string.put (retPtr, "", format->info.string.userData);
                    bufferPtr = NULL;
                }

                if (retPtr)
                    FormatDataInsertPointer (data, retPtr, format->position.offset, format->position.bitCount);
            }
            break;
        default:
            break;
        }
        format++;
    }
    /* Do something with itemCount FIXME */

    if (bufferPtr)
        bufferPtr = SkipWS (bufferPtr);

    return bufferPtr;
}

/* CopyFormat : copy the format elements from `source' to `dest' formats, adding `offset'
	to each of the position offsets of the new target elements on the way.  Returns a pointer
	to the next un-copied position in dest.  Note that `dest' *must* be large enough to contain
	the whole of `source' */
FormatElement *CopyFormat (FormatElement * dest, FormatElement * source, unsigned offset)
{
    while (source->nature != FORMAT_END)
    {
        *dest = *source;
        dest->position.offset += offset;
        dest++;
        source++;
    }

    return dest;
}

/* {New,Delete,Copy}FormatRadixData : new/delete/copy radix data */
FormatRadixData *NewFormatRadixData (int radix, char *prefix, char *suffix, int showLeadingZeroes, int separatorSpacing)
{
    FormatRadixData *ret = malloc (sizeof (FormatRadixData));

    ret->radix = radix;
    ret->prefix = ret->suffix = NULL;
    FormatRadixDataSetPrefixSuffix (ret, (prefix && *prefix ? strdup (prefix) : NULL), (suffix && *suffix ? strdup (suffix) : NULL));
    ret->showLeadingZeroes = showLeadingZeroes;
    ret->separatorSpacing = separatorSpacing;

    return ret;
}

void DeleteFormatRadixData (FormatRadixData * radix_data)
{
    if (radix_data->prefix)
        free (radix_data->prefix);
    if (radix_data->suffix)
        free (radix_data->suffix);
    free (radix_data);
}

FormatRadixData *CopyFormatRadixData (FormatRadixData * radix_data)
{
    return NewFormatRadixData (radix_data->radix,
      radix_data->prefix, radix_data->suffix, radix_data->showLeadingZeroes, radix_data->separatorSpacing);
}

/* FormatRadixDataSetPrefixSuffix : set the prefix and/or suffix of a RadixData
	structure.  Only sets prefix if `prefix' is not NULL, ... suffix ... `suffix' */
void FormatRadixDataSetPrefixSuffix (FormatRadixData * radix, const char *prefix, const char *suffix)
{
    unsigned prefixSuffixLength = 0;

    if (!radix)
        return;

    if (prefix)
    {
        int prefixLength = strlen (prefix);

        if (radix->prefix)
            free (radix->prefix);
        radix->prefix = malloc (prefixLength + 1);
        strcpy (radix->prefix, prefix);
        prefixSuffixLength += prefixLength;
    }

    if (suffix)
    {
        int suffixLength = strlen (suffix);

        if (radix->suffix)
            free (radix->suffix);
        radix->suffix = malloc (suffixLength + 1);
        strcpy (radix->suffix, suffix);
        prefixSuffixLength += suffixLength;
    }

    radix->prefixSuffixLength = prefixSuffixLength;
}
