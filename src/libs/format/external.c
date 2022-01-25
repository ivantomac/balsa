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

	`external.c'
	External (string) representation of formats.
	Note that this is all due to be replaced with some better, non printf-like
	system

*/

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

#include "format/external.h"

/* SkipWS : skip leading whitespace in string, really should make this part of something common */
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

/* FormatPrintEscapedString : print the given string with the escaping rules:
	'\', all non printable characters (!isprint(chr)) and the characters in
	the escapableChars string are printed as \xXX hex escape codes (with *exactly* 2 hex digits),
	`length' specifies the true length of the source string (if non NUL terminated, or contains
	NUL) or -1 indicating that strlen (string) is the true string length */
void FormatPrintEscapedString (FILE * stream, const char *string, int length, const char *escapableChars)
{
    if (length < 0)
        length = strlen (string);

    while (length > 0)
    {
        if (!isprint (*string) || *string == '\\' || (escapableChars && strchr (escapableChars, *string)))
        {
            fprintf (stream, "\\x%02X", *string & 0xFF);
        } else
        {
            putc (*string, stream);
        }
        string++;
        length--;
    }
}

/* FormatPrintEscapedStringToString : Same as FormatPrintEscapedString but writes the output
   to the buffer 'destString'.
   Returns the number of chars written.
   You need to be careful to allocate a large enough buffer when calling this function! */
int FormatPrintEscapedStringToString (char *destString, const char *string, int length, const char *escapableChars)
{
    char *ptr = destString;

    if (length < 0)
        length = strlen (string);

    while (length > 0)
    {
        if (!isprint (*string) || *string == '\\' || (escapableChars && strchr (escapableChars, *string)))
        {
            ptr += sprintf (ptr, "\\x%02X", *string & 0xFF);
        } else
        {
            *ptr = *string;
            ptr++;
        }
        string++;
        length--;
    }

    *ptr = 0;
    return (ptr - destString);
}

/* FormatDeescapeString : modify the given string to reverse the escaping effects of FormatPrintEscapedString.
	Returns the number of characters in the final string (not including the NUL termination) */
int FormatDeescapeString (char *string)
{
    char *srcPtr = string;
    char *dstPtr = string;
    int charCount = 0;

    while (*srcPtr)
    {
        char srcChar = *srcPtr;

        srcPtr++;

        if (srcChar == '\\')
        {
            char escapeChar = *srcPtr;

            srcPtr++;

            switch (escapeChar)
            {
            case 'x':
                if (isxdigit (srcPtr[0]) && isxdigit (srcPtr[1]))
                {
                    char hex[3];

                    hex[2] = '\0';
                    strncpy (hex, srcPtr, 2);
                    srcPtr += 2;
                    srcChar = strtoul (hex, NULL, 16);
                } else
                    srcChar = '?';
                break;
                /* Handle a few more common escape codes too just in case */
            case '\\':
                srcChar = '\\';
                break;
            case 'r':
                srcChar = '\r';
                break;
            case 'n':
                srcChar = '\n';
                break;
            default:           /* Ignore escaping */
                srcChar = '?';
                break;
            }
        }

        /* Copy a character over */
        *dstPtr = srcChar;
        dstPtr++;
        charCount++;
    }
    *dstPtr = '\0';

    return charCount;
}

/* FormatExternalParse... : parse the named type of content out of the
	current format where *format should point to a valid component of
	that type of content (eg. a digit for Natural parsing).  Returns
	the updated `format' pointer and sets `*value' to the value of the
	content found in the string */
static char *FormatExternalParseNatural (const char *format, unsigned *value)
{
    char *endPtr = (char *) format;
    unsigned ret = strtoul (format, &endPtr, 10);

    if (value)
        *value = ret;
    return endPtr;
}

/* ...Position : parse an (index:index) bitfield position */
static char *FormatExternalParsePosition (const char *format, FormatPosition * value)
{
    const char *endPtr = format;

    if (*endPtr != '(')
    {
        fprintf (stderr, "Position first char must be `('\n");
        return (char *) endPtr;
    } else
    {
        unsigned firstIndex, secondIndex;

        endPtr = FormatExternalParseNatural (endPtr + 1, &firstIndex);
        if (*endPtr != ':')
        {
            fprintf (stderr, "Expecting position index separator `:'\n");
            return (char *) endPtr;
        }
        endPtr = FormatExternalParseNatural (endPtr + 1, &secondIndex);
        if (*endPtr != ')')
        {
            fprintf (stderr, "Position last char must be `)'\n");
            return (char *) endPtr;
        }
        endPtr++;

        if (value)
        {                       /* Bother to set result */
            if (firstIndex > secondIndex)
            {
                value->offset = secondIndex;
                value->bitCount = (firstIndex - secondIndex) + 1;
            } else
            {
                value->offset = firstIndex;
                value->bitCount = (secondIndex - firstIndex) + 1;
            }
        }
    }

    return (char *) endPtr;
}

/* ...List...Position : parse an `offset bitCount' bitfield position */
static char *FormatListExternalParsePosition (const char *format, FormatPosition * value)
{
    const char *endPtr = format;
    unsigned offset, bitCount;

    endPtr = SkipWS (FormatExternalParseNatural (endPtr, &offset));
    endPtr = SkipWS (FormatExternalParseNatural (endPtr, &bitCount));

    if (value)
    {                           /* Bother to set result */
        value->offset = offset;
        value->bitCount = bitCount;
    }

    return (char *) endPtr;
}

/* FormatExternalParseEnumeration : parse and add an enumeration description
	from the given format (must start with the '(' which comes after the
	enumeration 'e').  Returns the tail of the format string */
static char *FormatExternalParseEnumeration (FormatElement * ret, char *format)
{
    /* First check for brackets and commas */
    char *formatPtr = format;
    int elementCount = 1, i;
    int longestElementName = 0;
    FormatEnumerationElement *elements = NULL;
    FormatData *nextElementValue = NewFormatDataFromUnsignedInt (FormatBitCountToWordCount (ret->position.bitCount), 0);

    if (*formatPtr != '(')
    {
        fprintf (stderr, "Expecting `(' in enumeration\n");
        return formatPtr;
    }
    {                           /* Check for strict alternation of commas and '='s */
        while (*formatPtr && *formatPtr != ')')
        {
            if (*formatPtr == ',')
                elementCount++;

            formatPtr++;
        }
    }

    /* We can now initialise the return structure */
    ret->nature = FORMAT_ENUMERATION;
    ret->info.enumeration.elementCount = elementCount;
    ret->info.enumeration.elements = elements = malloc (sizeof (FormatEnumerationElement) * elementCount);

    formatPtr = format + 1;

    for (i = 0; i < elementCount; i++)
    {
        char *endOfName = formatPtr;
        int nameLength;

        /* Find the end of the name */
        while (!strchr ("=,)", *endOfName))
            endOfName++;

        nameLength = endOfName - formatPtr;
        if (nameLength > longestElementName)
            longestElementName = nameLength;

        elements->name = malloc (nameLength + 1);
        strncpy (elements->name, formatPtr, nameLength);
        elements->name[nameLength] = '\0';

        if (*endOfName == '=')  /* name=value */
        {
            bool isNeg = false;

            formatPtr = endOfName + 1;
            if (*formatPtr == '-')
            {
                isNeg = true;
                formatPtr++;
            }
            elements->value = FormatDataParseUInt (formatPtr, 10);
            if (isNeg)
            {
                FormatData *negatedValue = FormatDataNegate (elements->value, ret->position.bitCount);

                DeleteFormatData (elements->value);
                elements->value = negatedValue;
            }
            DeleteFormatData (nextElementValue);
            nextElementValue = CopyFormatData (elements->value);
        } else
        {
            formatPtr = endOfName;
            elements->value = CopyFormatData (nextElementValue);
        }
        /* next element may be current one + 1 */
        FormatDataMultiplyThenAdd (&nextElementValue, 1, 1);
        /* Seek end of value */
        while (*formatPtr != ',' && *formatPtr != ')')
            formatPtr++;
        /* Go past the [),] */
        formatPtr++;

        elements++;
    }
    ret->info.enumeration.longestElementNameLength = longestElementName;

    DeleteFormatData (nextElementValue);

    return formatPtr;
}

#define INITIAL_FORMAT_LENGTH (4)

/* FormatExternalToInternal : make an internal representation format structure
	from an external format string */
Format FormatExternalToInternal (char *format)
{
    static Format tmpFormat = NULL;
    static int tmpFormatAllocatedLength = 0;
    int formatLength = 0;

    if (!tmpFormat)
    {
        tmpFormatAllocatedLength = INITIAL_FORMAT_LENGTH;
        tmpFormat = NewFormat (tmpFormatAllocatedLength);
    }

    while (*format)
    {
        char *startFormat = format;

        while (*format && *format != '%')
            format++;
        /* Did we find any literal string stuff first */
        if ((format - startFormat) > 0)
        {
            NewFormatLiteral (tmpFormat + formatLength, startFormat, format - startFormat);
            formatLength++;
        }

        if (*format == '%')     /* Just make sure */
        {
            FormatPosition position = { 0, 0 };
            int strippedLeadingFlags = 0;
            int showSign = 0;

            format++;

            while (!strippedLeadingFlags)
            {
                /* Strip leading ! and - */
                switch (*format)
                {
                case '!':
                case '-':
                    showSign = 1;
                    format++;
                    break;
                default:
                    strippedLeadingFlags = 1;
                    break;
                }

                if (!*format)
                {
                    fprintf (stderr, "Incomplete format string\n");
                    return NULL;
                }
            }

            /* Leading position */
            if (*format == '(')
                format = FormatExternalParsePosition (format, &position);

            /* Discard m prefix */
            if (*format == 'm')
                format++;

            switch (*format)
            {
            case 'n':          /* Number */
            case 'i':
                NewFormatNumber (tmpFormat + formatLength, position);
                tmpFormat[formatLength].info.number.showSign = showSign;
                formatLength++;
                format++;
                break;
            case 'e':          /* Enumeration */
                tmpFormat[formatLength].position = position;
                format = FormatExternalParseEnumeration (tmpFormat + formatLength, format + 1);
                formatLength++;
                break;
            default:
                fprintf (stderr, "Unrecognised format character `%c'\n", *format);
                return NULL;
                break;
            case '\0':
                fprintf (stderr, "Incomplete format string\n");
                return NULL;
                break;
            }
        }

        /* Extend tmpFormat array if necessary */
        if (formatLength >= (tmpFormatAllocatedLength - 1))
        {
            tmpFormatAllocatedLength *= 2;
            tmpFormat = realloc (tmpFormat, sizeof (FormatElement) * tmpFormatAllocatedLength);
        }
    }

    /* Make a copy of the used portion of the tmpFormat as the result value */
    {
        Format newFormat = NewFormat (formatLength + 1);

        memcpy (newFormat, tmpFormat, formatLength * sizeof (FormatElement));
        return newFormat;
    }
}

/* FormatListExternalToInternal : make an internal representation format structure
	from an external format string in the new lispy format, use `getString' and `putString'
	functions for strings and `quoteChar' whereever a quote character is required. */
Format FormatListExternalToInternal (const char *string, FormatGetFunction getString, FormatPutFunction putString, char quoteChar)
{
    const char *stringPtr = SkipWS (string);
    Format ret = NULL;

#define EXPECTING(thing) do { \
	fprintf (stderr, "%s:%d: expecting " thing " at `%s' index %d (%c)\n", \
		__func__, __LINE__, string, (int) (stringPtr - string), *stringPtr); \
		if (ret) free (ret); \
		return NULL; \
	} while (0)

    if (*stringPtr == '(')      /* ) */
    {
        const char *symbolPtr = stringPtr + 1;
        const char *endSymbolPtr;
        unsigned symbolLength = SymbolLength (symbolPtr);
        unsigned formatLength;
        FormatElement *retPtr;

        /* Parse out (format <count> ...) */
        if (!SymbolEqual (symbolPtr, "format", symbolLength))
            EXPECTING ("format");

        stringPtr = SkipWS (symbolPtr + symbolLength);

        if (!isdigit (*stringPtr))
            EXPECTING ("a format length count");

        endSymbolPtr = FormatExternalParseNatural (stringPtr, &formatLength);
        if (formatLength < 0)
            EXPECTING ("a format length count >= 0");
        stringPtr = SkipWS (endSymbolPtr);

        ret = retPtr = NewFormat (formatLength);

        while (*stringPtr && *stringPtr != /* ( */ ')')
        {
            switch (*stringPtr)
            {
            case '\"':         /* Literal string */
                {
                    const char *closeQuotePtr = strchr (stringPtr + 1, '\"');

                    if (!closeQuotePtr)
                        EXPECTING ("a matching close quote for");

                    NewFormatLiteral (retPtr, stringPtr + 1, (closeQuotePtr - stringPtr) - 1);
                    retPtr->info.literal.length = FormatDeescapeString (retPtr->info.literal.string);

                    retPtr++;
                    stringPtr = closeQuotePtr + 1;
                }
                break;
            case '(':          /* Some more complicated element */
                symbolPtr = stringPtr + 1;
                symbolLength = SymbolLength (symbolPtr);
                stringPtr = SkipWS (symbolPtr + symbolLength);
                stringPtr = FormatListExternalParsePosition (stringPtr, &(retPtr->position));

                if (SymbolEqual (symbolPtr, "number", symbolLength))
                {
                    NewFormatNumber (retPtr, retPtr->position);

                    /* Parse out the option symbols */
                    while (isalpha (*stringPtr))
                    {
                        if (strncmp (stringPtr, "sign", 4) == 0)
                        {
                            retPtr->info.number.showSign = true;
                            stringPtr += 4;
                        } else if (strncmp (stringPtr, "zero", 4) == 0)
                        {
                            retPtr->info.number.showLeadingZeroes = true;
                            stringPtr += 4;
                        }
                        stringPtr = SkipWS (stringPtr);
                    }
                    retPtr++;
                } else if (SymbolEqual (symbolPtr, "enum", symbolLength))
                {
                    /* Expecting an element count */
                    if (!isdigit (*stringPtr))
                        EXPECTING ("an element count in (enum ...) format element");
                    {
                        int elementCount = atoi (stringPtr);
                        int elementIndex = 0;
                        FormatEnumerationElement *elements = malloc (sizeof (FormatEnumerationElement) * elementCount);
                        FormatEnumerationElement *elementPtr = elements;

                        /* Skip number */
                        while (isdigit (*stringPtr))
                            stringPtr++;
                        stringPtr = SkipWS (stringPtr);

                        while (*stringPtr == '(' && elementIndex < elementCount) /* ) */
                        {
                            stringPtr = SkipWS (stringPtr + 1);
                            /* Now expecting `"name" value' */
                            if (*stringPtr == '\"')
                            {
                                stringPtr++;
                                char *endQuote = strchr (stringPtr, '\"');

                                if (endQuote)
                                {
                                    int nameLength = endQuote - stringPtr;

                                    elementPtr->name = malloc (nameLength + 1);
                                    elementPtr->name[nameLength] = '\0';
                                    strncpy (elementPtr->name, stringPtr, nameLength);
                                    FormatDeescapeString (elementPtr->name);

                                    /* Parse the value and skip to the close paren */
                                    stringPtr = SkipWS (endQuote + 1);
                                    elementPtr->value = FormatDataParseUInt (stringPtr, 10);

                                    /* Skip to close paren */
                                    /* ( */ while (*stringPtr
                                      && *stringPtr != ')')
                                        stringPtr++;

                                    if (!stringPtr)
                                        EXPECTING ("close parenthesis on enum element");
                                    else
                                        stringPtr++;

                                    elementPtr++;
                                    elementIndex++;
                                } else
                                    EXPECTING ("close quote on enum element name");
                            }
                            stringPtr = SkipWS (stringPtr);
                        }

                        /* Fill the remaining elements (if any) with NULL, NULL */
                        while (elementIndex < elementCount)
                        {
                            elementPtr->name = "";
                            elementPtr->value = NULL;

                            elementPtr++;
                            elementIndex++;
                        }
                        NewFormatEnumeration (retPtr, retPtr->position, elementCount, elements);
                        retPtr++;
                    }
                } else if (SymbolEqual (symbolPtr, "string", symbolLength))
                {
                    NewFormatString (retPtr, retPtr->position, quoteChar, getString, putString);
                    retPtr++;
                } else
                {
                    EXPECTING ("a valid format type name");
                }
                if (*stringPtr != /* ( */ ')')
                    /* ( */ EXPECTING (")");
                stringPtr++;
                break;
            default:
                EXPECTING ("`\"' or `('"); /* ) */
                break;
            }
            stringPtr = SkipWS (stringPtr);
        }

        if (*stringPtr != /* ( */ ')')
            /* ( */ EXPECTING (")");
    } else
    {
        EXPECTING ("(");        /* ) */
    }

#undef EXPECTING

    return ret;
}

/* FormatPrintFormat : print a format using the lispy external representation for the format */
void FormatPrintFormat (FILE * stream, Format format)
{
    FormatElement *formatPtr;
    unsigned formatLength = 0;

    if (!format)
        return;

    formatPtr = format;
    while (formatPtr->nature != FORMAT_END)
    {
        formatLength++;
        formatPtr++;
    }

    formatPtr = format;
    fprintf (stream, "(format %d", formatLength); /* ) */

    while (formatPtr->nature != FORMAT_END)
    {
        switch (formatPtr->nature)
        {
        case FORMAT_LITERAL:
            fprintf (stream, " \"");
            FormatPrintEscapedString (stream, formatPtr->info.literal.string, -1, "\"");
            putc ('\"', stream);
            break;
        case FORMAT_NUMBER:
            fprintf (stream, " (number %d %d%s%s)", formatPtr->position.offset,
              formatPtr->position.bitCount,
              (formatPtr->info.number.showSign ? " sign" : ""), (formatPtr->info.number.showLeadingZeroes ? " zero" : ""));
            break;
        case FORMAT_ENUMERATION:
            {
                int i;
                FormatEnumerationElement *elements = formatPtr->info.enumeration.elements;
                char *value = malloc (FormatDataLengthInRadix ((formatPtr->position.bitCount / FORMAT_BITS_PER_INT) + 1,
                    10) + 2);

                fprintf (stream, " (enum %d %d %d", formatPtr->position.offset, formatPtr->position.bitCount, formatPtr->info.enumeration.elementCount); /* ) */
                for (i = 0; i < formatPtr->info.enumeration.elementCount; i++)
                {
                    FormatDataFormatAsUInt (value, elements[i].value, 10, 0);
                    fprintf (stream, " (\""); /* ) */
                    FormatPrintEscapedString (stream, elements[i].name, -1, "\"");
                    /* ( */ fprintf (stream, "\" %s)", value);
                }
                /* ( */ fprintf (stream, ")");
                free (value);
            }
            break;
        case FORMAT_STRING:
            /* Note that the get and put functions are not dumped.  These must be filled in when the
               FormatListExternalToInternal function is run */
            fprintf (stream, " (string %d %d)", formatPtr->position.offset, formatPtr->position.bitCount);
            break;
        default:
            break;
        }
        formatPtr++;
    }
    fprintf (stream, /* ( */ ")");
}
