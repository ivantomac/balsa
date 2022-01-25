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

	`string.c'
	Balsa string handling

*/

#include "balsasim/builtin.h"
#include "balsasim/object.h"
#include "balsasim/bstring.h"
#include "format/format.h"
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

/* StringLength (string : String) : cardinal */
static void String_StringLength (BuiltinFunction * function, BuiltinFunctionInstanceData * instance)
{
    BalsaObject *string = FormatDataGetBalsaObject (instance->arguments[0], 0);

    instance->result->words[0] = BALSA_STRING (string->data)->length;
}

#define ABS(v) ((v) < 0 ? -(v) : (v))

/* FromString (parameter X : type; source : String; remainder : String) is builtin : X */
static void String_FromString (BuiltinFunction * function, BuiltinFunctionInstanceData * instance)
{
    if (!instance->portWidthsAreResolved)
    {
        BalsaType *returnType = instance->parameters[0]->info.type;

        if (!returnType)
        {
            BALSA_SIM_PRINTF ("Invalid type in FromString\n");
            exit (EXIT_FAILURE);
        }

        instance->resultWidth = ABS (returnType->size);
        instance->objectCount = BalsaTypeBuiltinCount (returnType);
    } else
    {
        BalsaType *type = instance->parameters[0]->info.type;
        BalsaString *sourceStr = BALSA_STRING (FormatDataGetBalsaObject (instance->arguments[0], 0)->data);
        BalsaObject *remainder = FormatDataGetBalsaObject (instance->arguments[1], 0);
        BalsaString *remainderStr = BALSA_STRING (remainder->data);
        bool returnEmptyString = false;

        if (!type->format)
        {
            BalsaTypeMakeDefaultFormat (type);
            if (!type->format)
            {
                BALSA_SIM_PRINTF ("Invalid type in FromString\n");
                returnEmptyString = true;
            }
        }

        if (!returnEmptyString)
        {
            char lastChar = sourceStr->string[sourceStr->length];
            char *remainderCharPtr;

            sourceStr->string[sourceStr->length] = '\0';

            BalsaTypePackBalsaObjectsIntoFormatData (instance->result, instance->parameters[0]->info.type, instance->objects);
            remainderCharPtr = FormatScan (sourceStr->string, type->format, instance->result, BalsaRadixChoiceFunction, NULL);
            sourceStr->string[sourceStr->length] = lastChar;

            if (!remainderCharPtr)
            {
                BALSA_SIM_PRINTF ("Failed to parse value from string in FromString\n");
                returnEmptyString = true;
            } else
            {
                /* remainder will be a substring of argument */
                remainder->data = NewBalsaSubString (sourceStr, remainderCharPtr, -1);
                BalsaStringUnref (remainderStr);
            }
        }

        /* Something went wrong, return an empty string in remainder and set
           return value to 0 */
        if (returnEmptyString)
        {
            BalsaStringUnref (remainderStr);
            remainder->data = NewBalsaString ("", 0);
            FormatDataSetWords (instance->result, 0);
        }
    }
}

/* SimpleRadixChoice : radix choice function for NumberToString, simply
	passes through defined radix choice data */
static FormatRadixData **SimpleRadixChoice (unsigned radix, FormatRadixData * radixData)
{
    static FormatRadixData *radices[] = { NULL, NULL };

    radices[0] = radixData;

    return radices;
}

/* CheckRadix : check that a radix value passed into one of the To/From functions is valid,
	exit the program if it isn't */
static void CheckRadix (unsigned radix)
{
    if (radix > 36 || radix < 2)
    {
        BALSA_SIM_PRINTF ("Invalid radix in NumberToString (%d), must be between 2 and 36\n", radix);
        exit (EXIT_FAILURE);
    }
}

/* NumberToString (parameter width : cardinal; value : width bits; radix : 5 bits; showLeadingZeroes : bit) is builtin : String */
static void String_NumberToString (BuiltinFunction * function, BuiltinFunctionInstanceData * instance)
{
    unsigned width = ABS (instance->parameters[0]->info.type->size);

    if (!instance->portWidthsAreResolved)
    {
        instance->argumentWidths[0] = width;
        if ((instance->parameters[0]->info.type->nature != BalsaNumericType) && (instance->parameters[0]->info.type->nature != BalsaRecordType))
        {
            BALSA_SIM_PRINTF ("Type parameter to NumberToString must be a numeric type\n");
            exit (EXIT_FAILURE);
        }
    } else
    {
        FormatData *value = instance->arguments[0];
        unsigned radix = instance->arguments[1]->words[0];
        unsigned underscoreSpacing = instance->arguments[2]->words[0];
        bool showLeadingZeroes = instance->arguments[3]->words[0];
        BalsaType *type = instance->parameters[0]->info.type;

        CheckRadix (radix);

        Format format = NewFormat (1);
        FormatRadixData *radixData = NewFormatRadixData (radix, "", "", showLeadingZeroes,
          underscoreSpacing);
        char *buffer;

        /* Format the number into buffer */
        NewFormatNumber (format, (FormatPosition)
          {
          0, width}
        );
        format->info.number.showSign = type->size < 0;
        buffer = malloc (FormatLength (format, (FormatRadixChoiceFunction) SimpleRadixChoice, radixData, value) + 1);
        FormatPrint (buffer, format, value, (FormatRadixChoiceFunction) SimpleRadixChoice, radixData);
        DeleteFormatRadixData (radixData);
        free (format);

        /* Make the return styring */
        BalsaString *string = NewBalsaString (NULL, 0);

        string->string = string->allocatedString = buffer;
        string->length = strlen (buffer);

        SetBalsaObject (instance->objects[0], string, (BalsaDestructor) BalsaStringUnref);
        FormatDataSetBalsaObject (instance->result, instance->objects[0], 0);
    }
}

/* NumberFromString (parameter width : cardinal; source : String; radix : 6 bits) is builtin : width bits */
static void String_NumberFromString (BuiltinFunction * function, BuiltinFunctionInstanceData * instance)
{
    unsigned width = ABS (instance->parameters[0]->info.type->size);

    if (!instance->portWidthsAreResolved)
    {
        instance->resultWidth = width;
        if (instance->parameters[0]->info.type->nature != BalsaNumericType)
        {
            BALSA_SIM_PRINTF ("Type parameter to NumberFromString must be a numeric type\n");
            exit (EXIT_FAILURE);
        }
    } else
    {
        BalsaString *string = BALSA_STRING (FormatDataGetBalsaObject (instance->arguments[0], 0)->data);
        unsigned radix = instance->arguments[1]->words[0];

        CheckRadix (radix);

        /* Remember the last + 1 character of the given string, substritute with
           '/0' to make FormatScan not process characters beyond the end of the string */
        char lastStringChar = string->string[string->length];
        Format format = NewFormat (1);
        FormatRadixData *radixData = NewFormatRadixData (radix, "", "", 0, 0);

        /* Parse number from source string */
        NewFormatNumber (format, (FormatPosition)
          {
          0, width}
        );

        /* Temporarily place a NULL at the end of string */
        string->string[string->length] = '\0';
        if (!FormatScan (string->string, format, instance->result, (FormatRadixChoiceFunction) SimpleRadixChoice, radixData))
        {                       /* Set result to 0 and moan on failure */
            BALSA_SIM_PRINTF ("NumberFromString couldn't parse `%s' in radix %d\n", string->string, radix);
            FormatDataSetWords (instance->result, 0);
        }
        DeleteFormatRadixData (radixData);
        free (format);

        /* Restore string's last + 1 character */
        string->string[string->length] = lastStringChar;
    }
}

/* SubString (string : String; index : cardinal; length : cardinal) is builtin : String */
static void String_SubString (BuiltinFunction * function, BuiltinFunctionInstanceData * instance)
{
    BalsaString *string = BALSA_STRING (FormatDataGetBalsaObject (instance->arguments[0], 0)->data);
    BalsaString *retString;
    unsigned index = instance->arguments[1]->words[0];
    unsigned length = instance->arguments[2]->words[0];

    if (length == 0 || index >= string->length)
    {                           /* Return empty string */
        retString = NewBalsaString ("", 0);
    } else
    {
        /* Adjust length to clip string if requested substring extends beyond end of source string */
        if ((length + index) > string->length)
            length = string->length - index;

        retString = NewBalsaSubString (string, string->string + index, length);
    }

    SetBalsaObject (instance->objects[0], retString, (BalsaDestructor) BalsaStringUnref);
    FormatDataSetBalsaObject (instance->result, instance->objects[0], 0);
}

/* StringEqual (str1, str2 : String) is builtin : bit */
static void String_StringEqual (BuiltinFunction * function, BuiltinFunctionInstanceData * instance)
{
    BalsaString *str1 = BALSA_STRING (FormatDataGetBalsaObject (instance->arguments[0], 0)->data);
    BalsaString *str2 = BALSA_STRING (FormatDataGetBalsaObject (instance->arguments[1], 0)->data);

    instance->result->words[0] = (str1->length == str2->length && memcmp (str1->string, str2->string, str1->length) == 0 ? 1 : 0);
}

/* SkipWS : the usual */
static char *SkipWS (char *string)
{
    while (isspace (*string))
        string++;

    return string;
}

/* CountNonWS : count the number of leading, non-whitespace characters in the given string */
static unsigned CountNonWS (char *string)
{
    unsigned ret = 0;

    while (*string && !isspace (*string))
    {
        string++;
        ret++;
    }

    return ret;
}

/* TokenFromString (string : String; remainder : String) is builtin : String */
static void String_TokenFromString (BuiltinFunction * function, BuiltinFunctionInstanceData * instance)
{
    BalsaString *string = BALSA_STRING (FormatDataGetBalsaObject (instance->arguments[0], 0)->data);
    BalsaObject *remainderObj = FormatDataGetBalsaObject (instance->arguments[1], 0);
    BalsaString *remainderStr = BALSA_STRING (remainderObj->data);
    BalsaString *retString;
    char *tokenBegin;

    /* Do the usual NUL insertion after string so that C string functions work */
    char lastChar = string->string[string->length];

    string->string[string->length] = '\0';

    tokenBegin = SkipWS (string->string);

    if (*tokenBegin)
    {
        unsigned tokenLength = CountNonWS (tokenBegin);

        retString = NewBalsaSubString (string, tokenBegin, tokenLength);
        remainderObj->data = NewBalsaSubString (string, SkipWS (tokenBegin + tokenLength), -1);
    } else
    {
        /* Return an empty string on failure */
        retString = NewBalsaString ("", 0);
        remainderObj->data = NewBalsaString ("", 0);
    }

    /* Restore the last source string character */
    string->string[string->length] = lastChar;
    BalsaStringUnref (remainderStr);

    SetBalsaObject (instance->objects[0], retString, (BalsaDestructor) BalsaStringUnref);
    FormatDataSetBalsaObject (instance->result, instance->objects[0], 0);
}

/* StringGetToken (line : String) is builtin : TokenTail */
static void String_StringGetToken (BuiltinFunction * function, BuiltinFunctionInstanceData * instance)
{
    BalsaString *string = BALSA_STRING (FormatDataGetBalsaObject (instance->arguments[0], 0)->data);
    /*
    BalsaObject *remainderObj = FormatDataGetBalsaObject (instance->arguments[1], 0);
    BalsaString *remainderStr = BALSA_STRING (remainderObj->data);
    */
    BalsaString *retString;
    BalsaString *retRest;
    char *tokenBegin;

    /* Do the usual NUL insertion after string so that C string functions work */
    char lastChar = string->string[string->length];

    string->string[string->length] = '\0';

    tokenBegin = SkipWS (string->string);

    if (*tokenBegin)
    {
        unsigned tokenLength = CountNonWS (tokenBegin);

        retString = NewBalsaSubString (string, tokenBegin, tokenLength);
        retRest = NewBalsaSubString (string, SkipWS (tokenBegin + tokenLength), -1);
    } else
    {
        /* Return an empty string on failure */
        retString = NewBalsaString ("", 0);
        retRest = NewBalsaString ("", 0);
    }

    /* Restore the last source string character */
    string->string[string->length] = lastChar;
    // BalsaStringUnref (remainderStr);

    SetBalsaObject (instance->objects[0], retString, (BalsaDestructor) BalsaStringUnref);
    SetBalsaObject (instance->objects[1], retRest, (BalsaDestructor) BalsaStringUnref);
    FormatDataSetBalsaObject (instance->result, instance->objects[0], 0);
    FormatDataSetBalsaObject (instance->result, instance->objects[1], 64);
}

/* StringToPrintable (string : String) is builtin : String */
static void String_StringToPrintable (BuiltinFunction * function, BuiltinFunctionInstanceData * instance)
{
    BalsaString *string = BALSA_STRING (FormatDataGetBalsaObject (instance->arguments[0], 0)->data);
    BalsaString *retString;
	int retLength = string->length;
	int srcIndex, dstIndex;
	static char *const hexChars = "0123456789ABCDEF";

	for (srcIndex = 0; srcIndex < string->length; srcIndex ++)
	{
		if (! isprint (string->string[srcIndex]))
			retLength += 3; /* <HH> length minus orig. character */
	}

	retString = NewBalsaString (NULL, 0);
	retString->allocatedString = retString->string = malloc (retLength + 1);
	retString->string[retLength] = '\0';
	retString->length = retLength;

	dstIndex = 0;
	for (srcIndex = 0; srcIndex < string->length; srcIndex ++)
	{
		char srcChar = string->string[srcIndex];

		if (isprint (srcChar))
		{
			retString->string[dstIndex] = srcChar;
			dstIndex ++;
		} else {
			retString->string[dstIndex + 0] = '<';
			retString->string[dstIndex + 1] = hexChars[(int) ((srcChar >> 4) & 0xF)];
			retString->string[dstIndex + 2] = hexChars[(int) ((srcChar >> 0) & 0xF)];
			retString->string[dstIndex + 3] = '>';
			dstIndex += 4;
		}
	}

    SetBalsaObject (instance->objects[0], retString, (BalsaDestructor) BalsaStringUnref);
    FormatDataSetBalsaObject (instance->result, instance->objects[0], 0);
}

/* String{Down,Up}case (string : String) is builtin : String */
static void String_StringDowncase (BuiltinFunction * function, BuiltinFunctionInstanceData * instance)
{
    BalsaString *string = BALSA_STRING (FormatDataGetBalsaObject (instance->arguments[0], 0)->data);
    BalsaString *retString;

	retString = NewBalsaString (NULL, 0);
	retString->allocatedString = retString->string = malloc (string->length + 1);
	retString->length = string->length;

	strcpy (retString->string, string->string);
	{
		char *ptr = retString->string;
		while (*ptr)
		{
			*ptr = tolower (*ptr);
			ptr ++;
		}
	}

    SetBalsaObject (instance->objects[0], retString, (BalsaDestructor) BalsaStringUnref);
    FormatDataSetBalsaObject (instance->result, instance->objects[0], 0);
}
static void String_StringUpcase (BuiltinFunction * function, BuiltinFunctionInstanceData * instance)
{
    BalsaString *string = BALSA_STRING (FormatDataGetBalsaObject (instance->arguments[0], 0)->data);
    BalsaString *retString;

	retString = NewBalsaString (NULL, 0);
	retString->allocatedString = retString->string = malloc (string->length + 1);
	retString->length = string->length;

	strcpy (retString->string, string->string);
	{
		char *ptr = retString->string;
		while (*ptr)
		{
			*ptr = toupper (*ptr);
			ptr ++;
		}
	}

    SetBalsaObject (instance->objects[0], retString, (BalsaDestructor) BalsaStringUnref);
    FormatDataSetBalsaObject (instance->result, instance->objects[0], 0);
}

/* RepeatString (str : String; n : cardinal) is builtin : String */
static void String_RepeatString (BuiltinFunction * function, BuiltinFunctionInstanceData * instance)
{
    BalsaString *string = BALSA_STRING (FormatDataGetBalsaObject (instance->arguments[0], 0)->data);
    unsigned repeatCount = instance->arguments[1]->words[0];
    BalsaString *retString;

    if (repeatCount == 0)
    {                           /* Return empty string */
        retString = NewBalsaString ("", 0);
    } else
    {
        int i;
        int sourceStringLength = string->length;
        char *retStringPtr;

        retString = NewBalsaString (NULL, 0);
        retString->length = repeatCount * sourceStringLength;
        retString->string = retString->allocatedString = retStringPtr = malloc (retString->length + 1);

        for (i = 0; i < repeatCount; i++)
        {
            memcpy (retStringPtr, string->string, sourceStringLength);
            retStringPtr += sourceStringLength;
        }
        *retStringPtr = '\0';
    }

    SetBalsaObject (instance->objects[0], retString, (BalsaDestructor) BalsaStringUnref);
    FormatDataSetBalsaObject (instance->result, instance->objects[0], 0);
}

typedef enum
{ StringJustification_left, StringJustification_right }
StringJustification;

/* FitStringToWidth (str : String; width : cardinal; justification : StringJustification) is builtin : String */
static void String_FitStringToWidth (BuiltinFunction * function, BuiltinFunctionInstanceData * instance)
{
    BalsaString *string = BALSA_STRING (FormatDataGetBalsaObject (instance->arguments[0], 0)->data);
    unsigned width = instance->arguments[1]->words[0];
    StringJustification justification = instance->arguments[2]->words[0];
    BalsaString *retString;
    unsigned sourceStringWidth = string->length;

    if (sourceStringWidth >= width)
    {                           /* Whole string or clip, use a sub string */
        retString = NewBalsaSubString (string,
          (justification == StringJustification_left ? string->string : string->string + (sourceStringWidth - width)), width);
    } else
    {                           /* Need to pad */
        int padWidth = width - sourceStringWidth;

        retString = NewBalsaString (NULL, 0);
        retString->length = width;
        retString->string = retString->allocatedString = malloc (width + 1);
        retString->string[width] = '\0';

        if (justification == StringJustification_left)
        {
            memcpy (retString->string, string->string, sourceStringWidth);
            memset (retString->string + sourceStringWidth, ' ', padWidth);
        } else
        {                       /* Right justified */
            memset (retString->string, ' ', padWidth);
            memcpy (retString->string + padWidth, string->string, sourceStringWidth);
        }
    }

    SetBalsaObject (instance->objects[0], retString, (BalsaDestructor) BalsaStringUnref);
    FormatDataSetBalsaObject (instance->result, instance->objects[0], 0);
}

/* Chr (value : byte) is builtin : String */
static void String_Chr (BuiltinFunction * function, BuiltinFunctionInstanceData * instance)
{
    char character = instance->arguments[0]->words[0];

    SetBalsaObject (instance->objects[0], NewBalsaString (&character, 1), (BalsaDestructor) BalsaStringUnref);
    FormatDataSetBalsaObject (instance->result, instance->objects[0], 0);
}

/* Ord (char : String) is builtin : byte */
static void String_Ord (BuiltinFunction * function, BuiltinFunctionInstanceData * instance)
{
    BalsaString *string = BALSA_STRING (FormatDataGetBalsaObject (instance->arguments[0], 0)->data);

    instance->result->words[0] = (string->length > 0 ? string->string[0] : 0);
}

BALSA_SIM_REGISTER_BUILTIN_LIB (string)
{
    BalsaSim_RegisterBuiltinFunction ("StringLength", 0, 1, String_StringLength, 32, (unsigned[])
      {
      64}
      , 0);
    BalsaSim_RegisterBuiltinFunction ("SubString", 0, 3, String_SubString, 64, (unsigned[])
      {
      64, 32, 32}
      , 1);
    BalsaSim_RegisterBuiltinFunction ("StringEqual", 0, 2, String_StringEqual, 1, (unsigned[])
      {
      64, 64}
      , 0);
    BalsaSim_RegisterBuiltinFunction ("FromString", 1, 2, String_FromString, 0, (unsigned[])
      {
      64, 64}
      , 0);
    BalsaSim_RegisterBuiltinFunction ("RepeatString", 0, 2, String_RepeatString, 64, (unsigned[])
      {
      64, 32}
      , 1);
    BalsaSim_RegisterBuiltinFunction ("FitStringToWidth", 0, 3, String_FitStringToWidth, 64, (unsigned[])
      {
      64, 32, 1}
      , 1);
    BalsaSim_RegisterBuiltinFunction ("NumberToString", 1, 4, String_NumberToString, 64, (unsigned[])
      {
      0, 6, 8, 1}
      , 1);
    BalsaSim_RegisterBuiltinFunction ("NumberFromString", 1, 2, String_NumberFromString, 0, (unsigned[])
      {
      64, 6}
      , 0);
    BalsaSim_RegisterBuiltinFunction ("TokenFromString", 0, 2, String_TokenFromString, 64, (unsigned[])
      {
      64, 64}
      , 1);
    BalsaSim_RegisterBuiltinFunction ("Chr", 0, 1, String_Chr, 64, (unsigned[])
      {
      8}
      , 1);
    BalsaSim_RegisterBuiltinFunction ("Ord", 0, 1, String_Ord, 8, (unsigned[])
      {
      64}
      , 0);
    BalsaSim_RegisterBuiltinFunction ("StringToPrintable", 0, 1, String_StringToPrintable, 64, (unsigned[])
      {
      64}
      , 1);
    BalsaSim_RegisterBuiltinFunction ("StringGetToken", 0, 1, String_StringGetToken, 128, (unsigned []) {64}, 2); 
    BalsaSim_RegisterBuiltinFunction ("StringDowncase", 0, 1, String_StringDowncase, 64, (unsigned[])
      {
      64}
      , 1);
    BalsaSim_RegisterBuiltinFunction ("StringUpcase", 0, 1, String_StringUpcase, 64, (unsigned[])
      {
      64}
      , 1);
}

