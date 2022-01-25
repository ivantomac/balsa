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

#ifndef FORMAT_HEADER
#define FORMAT_HEADER

#include "format/data.h"

typedef enum
{
    FORMAT_END,
    FORMAT_LITERAL,
    FORMAT_NUMBER,
    FORMAT_ENUMERATION,
    FORMAT_STRING
}
FormatElementNature;

typedef struct
{
    unsigned offset, bitCount;
}
FormatPosition;

/* FormatRadixData : data relating to printing of
	numbers of a particular radix. */
typedef struct
{
    int radix;
    char *prefix, *suffix;      /* Prefix and suffix strings */
    unsigned prefixSuffixLength; /* Combined length of prefix and suffix */
    int showLeadingZeroes:1;    /* Print leading zeroes upto word length */
    unsigned separatorSpacing;  /* 0 means no separators, count from LSdigit */
}
FormatRadixData;

typedef struct
{
    char *name;
    FormatData *value;
}
FormatEnumerationElement;

/* Format{Get,Put}Function : extraction and insertion function types for formats which
	refer to managed objects.  The meaning of the void *s in these functions is specific to
	the format type and the object type being referred to.
	FormatGetFunction turns a pointer to an object (which started life as a component of a
		FormatData) into a suitable pointer for printing (eg. a char * for FORMAT_STRING)
	FormatPutFunction performs an assignment of value into the object pointed to by
		`object'.  Returns `object' (or a new object) which must then be packed back into the
		FormatData we're scanning to.
	`userData' is passed from the `userData' field in the relevant portion of the
	format structure as the last argument of both `get' and `put' functions */
typedef void *(*FormatGetFunction) (void *object, void *userData);
typedef void *(*FormatPutFunction) (void *object, void *value, void *userData);

/* A format specification is an array of FormatElements
	ending in an element with nature == FORMAT_END */
typedef struct
{
    FormatElementNature nature;
    union
    {
        struct
        {
            char *string;
            unsigned length;    /* for convenience, must be strlen (string) */
        }
        literal;
        struct
        {
            bool showSign:1;    /* Use a sign prefix for negative numbers/treat the number as signed */
            bool showLeadingZeroes:1; /* ored with radix data to choose whether to print leading 0s */
        }
        number;
        struct
        {
            int elementCount;
            int longestElementNameLength; /* for FormatLength */
            FormatEnumerationElement *elements;
        }
        enumeration;
        struct
        {                       /* Managed object data, get and put are functions to turn a pointer into a value to
                                   print/turn a scanned value into an object (or do an assignment) */
            FormatGetFunction get;
            FormatPutFunction put;
            char quoteCharacter; /* NUL for no quote */
            void *userData;
        }
        string;
    }
    info;
    FormatPosition position;
}
FormatElement;

typedef FormatElement *Format;

#define FORMAT_BITS_PER_INT (sizeof (int) * 8)
#define FORMAT_BITS_PER_VOID_P (sizeof (int) * 8)

/* FormatRadixChoiceFunction : prototype for functions which select radix features
	based on a preferred radix provided.  If `preferredRadix' is 0 then no
	particular radix has been selected and the default may apply.  */
typedef FormatRadixData **(*FormatRadixChoiceFunction) (int preferredRadix, void *reservedData);

/* NewFormat: make a new format specification array with elements
	initialised to FORMAT_END */
extern FormatElement *NewFormat (unsigned length);

/* FormatElementCount : returns the number of elements (excluding trailing FORMAT_ENDs) in the
	given Format */
extern unsigned FormatElementCount (Format f);

/* InsertFormatElement: extend the format specification array by inserting an element initialised to FORMAT_END. The input format must contain a FORMAT_END *only* as a last element */
extern FormatElement *InsertFormatElement (Format f, unsigned newAtPos);

/* RemoveFormatElement: shorten the format specification array by removing an element. The input format must contain a FORMAT_END *only* as a last element */
extern FormatElement *RemoveFormatElement (Format f, unsigned removeAtPos);

/* NewFormat...: populate the pointed-to format element */
/* NewFormatLiteral: ... literal string. If length is 0 then whole string is copied,
	otherwise, length *must* be >= length of string */
/* NewFormatString: the given get and put functions should be used when printing or
	scanning a string.  if quoteCharacter is NUL then strings will be unquoted */
extern void NewFormatLiteral (FormatElement * ret, const char *string, unsigned length);
extern void NewFormatNumber (FormatElement * ret, FormatPosition position);
extern void NewFormatEnumeration (FormatElement * ret, FormatPosition position, int elementCount, FormatEnumerationElement * elements);
extern void NewFormatString (FormatElement * ret, FormatPosition position, char quoteCharacter, FormatGetFunction get, FormatPutFunction put);

/* FormatBareDecimalRadixChoice : most boring of all possible radix choice functions */
extern FormatRadixData **FormatBareDecimalRadixChoice (int prefferedRadix, void *reservedData);

/* FormatLength: returns the required allocated space to format using `format',
	plus one for the '\0' */
extern unsigned FormatLength (Format format, FormatRadixChoiceFunction radixChoiceFunction, void *reservedData, FormatData * data);

/* FormatPrint: print to a string (which must be large enough), the data value `data'
	with format `format'. `reservedData' is passed to the radix choice function when
	called. */
extern void FormatPrint (char *buffer, Format format, FormatData * data, FormatRadixChoiceFunction radixChoiceFunction, void *reservedData);

/* FormatScan: scan a string to the preallocated data value `data' with format `format'.
	Returns the pointer (in buffer) to the next character after the scanned data.
	or NULL on failure to scan the whole format.
	`reservedData' is passed to the radix choice function when called.
	A parsed enumeration will always use the longest match. */
extern char *FormatScan (char *buffer, Format format, FormatData * data, FormatRadixChoiceFunction radixChoiceFunction, void *reservedData);

/* CopyFormat : copy the format elements from `source' to `dest' formats, adding `offset'
	to each of the position offsets of the new target elements on the way.  Returns a pointer
	to the next un-copied position in dest.  Note that `dest' *must* be large enough to contain
	the whole of `source' */
extern FormatElement *CopyFormat (FormatElement * dest, FormatElement * source, unsigned offset);

/* {New,Delete,Copy}FormatRadixData : new/delete/copy radix data */
extern FormatRadixData *NewFormatRadixData (int radix, char *prefix, char *suffix, int showLeadingZeroes, int separatorSpacing);
extern void DeleteFormatRadixData (FormatRadixData * radix_data);
extern FormatRadixData *CopyFormatRadixData (FormatRadixData * radix_data);

/* FormatRadixDataSetPrefixSuffix : set the prefix and/or suffix of a RadixData
	structure.  Only sets prefix if `prefix' is not NULL, ... suffix ... `suffix' */
extern void FormatRadixDataSetPrefixSuffix (FormatRadixData * radix, const char *prefix, const char *suffix);

#endif
