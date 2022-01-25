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

	`data.c'
	Word array bigint manipulation

*/

#include <stdio.h>

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <assert.h>

#include "format/data.h"
#include "format/format.h"

/* NewFormatData : make a new, 0 initialised FormatData structure with `wordCount' words */
FormatData *NewFormatData (unsigned wordCount)
{
    FormatData *ret = malloc (sizeof (FormatData) + wordCount * sizeof (unsigned));

    ret->wordCount = wordCount;
    FormatDataSetWords (ret, 0);

    return ret;
}

/* FormatBitCountToWordCount : returns the number of words needed to represent bitCount bits */
unsigned FormatBitCountToWordCount (unsigned bitCount)
{
    return (bitCount + (sizeof (unsigned) * 8) - 1) / (sizeof (unsigned) * 8);
}

/* FormatWordCountToBitCount : returns the number of bits contained in wordCount words */
unsigned FormatWordCountToBitCount (unsigned wordCount)
{
    return wordCount * (sizeof (unsigned) * 8);
}

/* FormatDataSetWords : set each word in the given FormatData to the given word value */
void FormatDataSetWords (FormatData * data, unsigned word)
{
    unsigned i;

    for (i = 0; i < data->wordCount; i++)
        data->words[i] = word;
}

/* NewFormatDataFromUnsignedInt : make a new format data array initialised from the
	unsigned int value */
FormatData *NewFormatDataFromUnsignedInt (unsigned wordCount, unsigned int value)
{
    unsigned i;
    FormatData *ret = NewFormatData (wordCount);

    ret->words[0] = value;
    for (i = 1; i < wordCount; i++)
        ret->words[i] = 0;

    return ret;
}

/* DeleteFormatData : free a FormatData struct */
void DeleteFormatData (FormatData * data)
{
    free (data);
}

/* CopyFormatDataWords : copy data from one FormatData to another (clipping to
	the wordCount of the destination */
void CopyFormatDataWords (FormatData * dest, FormatData * src)
{
    unsigned wordCount;

    if (dest->wordCount < src->wordCount)
        wordCount = dest->wordCount;
    else
        wordCount = src->wordCount;

    memcpy (dest->words, src->words, sizeof (unsigned) * wordCount);
    if (wordCount < dest->wordCount)
    {
        int i;

        for (i = wordCount; i < dest->wordCount; i++)
            dest->words[i] = 0;
    }
}

/* CopyFormatData : copy a FormatData */
FormatData *CopyFormatData (FormatData * data)
{
    FormatData *ret = NewFormatData (data->wordCount);

    CopyFormatDataWords (ret, data);

    return ret;
}

/* FormatDataMultiplyThenAdd : multiply a FormatData by a small unsigned integer and then
	add another small integer, may reallocate `multiplicand' if it is too small to hold the
	result, hence the ** */
void FormatDataMultiplyThenAdd (FormatData ** multiplicand, unsigned multValue, unsigned addValue)
{
    FormatUInt64 accum = addValue;
    FormatData *data = *multiplicand;
    unsigned i = 0;

    do
    {
        accum += data->words[i] * (FormatUInt64) multValue;
        data->words[i] = (unsigned) accum;
        accum = (unsigned) (accum >> FORMAT_BITS_PER_INT);
        i++;
    }
    while (i < data->wordCount);

    if (accum != 0)
    {
        data->wordCount++;
        data = realloc (data, sizeof (FormatData) + sizeof (int) * data->wordCount);

        data->words[data->wordCount - 1] = accum;

        *multiplicand = data;
    }
}

/* FormatDataDivide : divide a FormatData by a small integer.  Also returns the (small integer)
	remainder in `*remainder' if `remainder' isn't NULL */
static void FormatDataDivide (FormatData * data, unsigned divValue, unsigned *remainder)
{
    unsigned workingRemainder = 0;
    int i;

    for (i = data->wordCount - 1; i >= 0; i--)
    {
        FormatUInt64 dividend = (((FormatUInt64) workingRemainder) << FORMAT_BITS_PER_INT) + data->words[i];

        data->words[i] = dividend / divValue;
        workingRemainder = dividend % divValue;
    }

    if (remainder)
        *remainder = workingRemainder;
}

/* FormatData{DigitValueToChar,CharToDigitValue} : digit char to from ASCII value for
	bases upto 36, return '?' and -1 respectively on error */
static int FormatDataCharToDigitValue (char chr)
{
    int ret = -1;

    if (isdigit (chr))
        ret = chr - '0';
    else
    {
        chr = toupper (chr);
        if (chr >= 'A' && chr <= 'Z')
            ret = 10 + chr - 'A';
    }

    return ret;
}
static char FormatDataDigitValueToChar (unsigned digit)
{
    char ret = '?';

    if (digit < 10)
        ret = digit + '0';
    else
        ret = (digit - 10) + 'A';

    return ret;
}

/* FormatDataParseUIntWithBuf :  same as FormatDataParseUInt but modify the
   string pointer to point after the end of the parsed number */
FormatData *FormatDataParseUIntWithBuf (char **str, int radix)
{
    FormatData *ret = NewFormatDataFromUnsignedInt (1, 0);
    int digitValue;
    bool validDigitChar;

    do
    {
        if (**str != '_')
        {
            digitValue = FormatDataCharToDigitValue (**str);
            validDigitChar = (digitValue != -1 && digitValue < radix);

            if (validDigitChar)
                FormatDataMultiplyThenAdd (&ret, radix, digitValue);
        } else
            validDigitChar = (**str != '\0');

        (*str)++;
    }
    while (**str && validDigitChar);

    return ret;
}

/* FormatDataParseUInt : parse a format data value (as an unsigned
	int) out of a string */
FormatData *FormatDataParseUInt (const char *str, int radix)
{
    char *strPtr = (char *) str;

    return FormatDataParseUIntWithBuf (&strPtr, radix);
}

static bool FormatDataIsZero (FormatData * data)
{
    int i;

    for (i = 0; i < data->wordCount; i++)
    {
        if (data->words[i] != 0)
            return false;
    }

    return true;
}

/* FormatDataLengthInRadix : returns the maximum number of characters
	needed to print a value with the number of words `wordCount'
	without sign/radix indication */
unsigned FormatDataLengthInRadix (unsigned wordCount, int radix)
{
    /* Assume we need: wordCount * log_radix MAX_INT chars
       == wordCount * (log2 MAX_INT / log2 radix)
       == 32 wordCount / (log10 radix / log10 2)
       == 10 wordCount / log10 radix
     */
    return (10.0 * wordCount) / log10 (radix);
}

#ifdef DEBUG
static void FormatDataDebugPrint (FILE * stream, FormatData * data)
{
    unsigned i;

    fprintf (stream, "words[0..%d]:", data->wordCount - 1);
    for (i = 0; i < data->wordCount; i++)
        fprintf (stream, " %u", data->words[i]);
}
#endif

/* FormatDataFormatAsUInt : format a FormatData as an unsigned integer (in
	decimal for the moment).  Formats into buffer and returns a pointer to
	the end of the formatted string.  Inserts leading zeroes to length `zeroedLength'
	if zeroedLength != 0 */
char *FormatDataFormatAsUInt (char *buffer, FormatData * data, int radix, unsigned zeroedLength)
{
    int bufferReqdLength = FormatDataLengthInRadix (data->wordCount, radix);
    char *bufferPtr = buffer + bufferReqdLength;
    int writtenLength = 0;

    if (FormatDataIsZero (data))
    {
        bufferPtr--;
        *bufferPtr = '0';
        writtenLength++;
    } else
    {
        FormatData *quotient = CopyFormatData (data);

        while (!FormatDataIsZero (quotient))
        {
            unsigned digit;

            bufferPtr--;

            FormatDataDivide (quotient, radix, &digit);
            *bufferPtr = FormatDataDigitValueToChar (digit);
            writtenLength++;
        }

        DeleteFormatData (quotient);

    }

    /* Now insert the zeroes. */
    if (zeroedLength < writtenLength)
        zeroedLength = 0;
    else
        zeroedLength -= writtenLength;

    /* Copy the `real' digits to the right place */
    memmove (buffer + zeroedLength, bufferPtr, writtenLength);
    /* Insert the zeroes */
    memset (buffer, '0', zeroedLength);

    buffer[writtenLength + zeroedLength] = '\0';

    return buffer + writtenLength + zeroedLength;
}

/* FormatDataExtractBitField : extract a bit field from the given FormatData and
	return a freshly allocated FormatData containing that bit field.  Note that the
	newly allocated FormatData is only as large as it needs to be */
FormatData *FormatDataExtractBitField (FormatData * data, unsigned offset, unsigned bitCount)
{
    unsigned returnWordCount = (bitCount + FORMAT_BITS_PER_INT - 1) / FORMAT_BITS_PER_INT;
    FormatData *ret = NewFormatData (returnWordCount);
    unsigned sourceIndex = offset / FORMAT_BITS_PER_INT;
    unsigned destIndex = 0;
    unsigned sourceOffset = offset % FORMAT_BITS_PER_INT;
    unsigned lastWordBitCount = bitCount % FORMAT_BITS_PER_INT;
    unsigned *source = data->words + sourceIndex;
    unsigned *dest = ret->words;

    while (destIndex < returnWordCount && sourceIndex < data->wordCount)
    {
        if (sourceOffset == 0)
            *dest = *source;
        else
        {
            *dest = (*source >> sourceOffset) + (sourceIndex + 1 < data->wordCount ? (source[1] << (FORMAT_BITS_PER_INT - sourceOffset)) : 0);
        }

        destIndex++;
        sourceIndex++;
        dest++; // Changed from *dest++ and *source++ 2007-04-05, check AB
        source++;
    }

    while (destIndex < returnWordCount)
    {
        destIndex++;
        *dest = 0;
        dest++;
    }

    if (lastWordBitCount != 0)
        ret->words[returnWordCount - 1] &= (1 << lastWordBitCount) - 1;

    return ret;
}

/* FormatDataInsertBitField : insert data src into dest dst at the given offset and bitCount.
	src->wordCount should be >= FormatBitCountToWordCount (bitCount) if performance is needed
        insertion of the bit field must also not require dst to grow */
void FormatDataInsertBitField (FormatData * dst, FormatData * src, unsigned offset, unsigned bitCount)
{
    unsigned *dest = dst->words + offset / FORMAT_BITS_PER_INT;
    unsigned *source = src->words;
    unsigned bitOffset = offset % FORMAT_BITS_PER_INT;

    /* Special fix, if src->wordcount is < FormatBitCountToWordCount (bitCount):
       Clear bitCount bits and reduce bitCount to a correct value */
    if (src->wordCount < FormatBitCountToWordCount (bitCount))
    {
        FormatData *fd = NewFormatData (FormatBitCountToWordCount (bitCount));

        FormatDataInsertBitField (dst, fd, offset, bitCount);
        bitCount = FormatWordCountToBitCount (src->wordCount);
    }

    /* Special case for small bitCounts as they will require a mask with 0s on both ends */
    if (bitCount < FORMAT_BITS_PER_INT)
    {
        unsigned mask = (1 << bitCount) - 1;

        *dest = (*dest & ~(mask << bitOffset)) | ((*source & mask) << bitOffset);
    } else if (bitOffset == 0)  /* Quick version for 0 offset */
    {
        /* Copy the whole words first */
        while (bitCount >= FORMAT_BITS_PER_INT)
        {
            bitCount -= FORMAT_BITS_PER_INT;
            *dest = *source;
            source++;
            dest++;
        }

        /* Then do the last < FORMAT_BITS_PER_INT bits */
        if (bitCount)
        {
            unsigned mask = (1 << bitCount) - 1;

            *dest = (*dest & ~mask) | (*source & mask);
        }
    } else
    {
        int bitsLeft = bitCount;
        unsigned carry = (*dest & ((1 << bitOffset) - 1));
        unsigned remainingBitCount;

        /* Complete destination overwriting */
        while (bitsLeft >= FORMAT_BITS_PER_INT)
        {
            *dest = (*source << bitOffset) | carry;
            carry = *source >> (FORMAT_BITS_PER_INT - bitOffset);
            bitsLeft -= FORMAT_BITS_PER_INT;
            dest++;
            source++;
        }

        remainingBitCount = bitsLeft + bitOffset;

        if (remainingBitCount >= FORMAT_BITS_PER_INT)
        {
            unsigned nextWordBitCount = remainingBitCount - FORMAT_BITS_PER_INT;

            *dest = carry | *source << bitOffset;
            dest++;
            *dest &= ~((1 << nextWordBitCount) - 1);
            *dest |= (*source >> (bitsLeft - nextWordBitCount)) & ((1 << nextWordBitCount) - 1);
        } else
        {
            *dest &= ~((1 << remainingBitCount) - 1);
            *dest |= carry;
            if (bitsLeft != 0)
                *dest |= ((*source & ((1 << bitsLeft) - 1)) << bitOffset);
        }
    }
}

/* FormatDataExtractPointer : extract a pointer value from a data word, the extracted pointer
	should be read from `bitCount' significant bits starting at offset `offset' */
void *FormatDataExtractPointer (FormatData * data, unsigned offset, unsigned bitCount)
{
    FormatData *bitField;
    void *ret = NULL;

    /* Clip to native pointer length */
    if (bitCount > FORMAT_BITS_PER_VOID_P)
        bitCount = FORMAT_BITS_PER_VOID_P;
    bitField = FormatDataExtractBitField (data, offset, bitCount);

    if (FORMAT_BITS_PER_VOID_P > FORMAT_BITS_PER_INT)
    {
        assert (bitField->wordCount == 2);
        ret = (void *) (((FormatUInt64) bitField->words[1] << FORMAT_BITS_PER_INT) + bitField->words[0]);
    } else
    {
        ret = (void *) bitField->words[0];
    }

    DeleteFormatData (bitField);
    return ret;
}

/* FormatDataInsertPointer : insert a pointer into the given FormatData, making
	it occupy the bitfield [bitCount+offset-1:offset] */
void FormatDataInsertPointer (FormatData * data, void *pointer, unsigned offset, unsigned bitCount)
{
    FormatData *pointerAsFormatData = NewFormatDataFromUnsignedInt (FormatBitCountToWordCount (bitCount), 0);

    if (FORMAT_BITS_PER_VOID_P > FORMAT_BITS_PER_INT)
    {
        unsigned *pointerAsUnsigned = (unsigned *) &pointer;

        assert (pointerAsFormatData->wordCount == 2);

        pointerAsFormatData->words[0] = pointerAsUnsigned[0];
        pointerAsFormatData->words[1] = pointerAsUnsigned[1];
    } else
    {
        pointerAsFormatData->words[0] = (unsigned) pointer;
        pointerAsFormatData->words[1] = 0;
    }

    FormatDataInsertBitField (data, pointerAsFormatData, offset, bitCount);
    DeleteFormatData (pointerAsFormatData);
}

/* FormatDataApplyBitField : Map the given FormatData onto a bit field and
	return a freshly allocated FormatData. */
FormatData *FormatDataApplyBitField (FormatData * data, unsigned offset, unsigned bitCount)
{
    unsigned returnWordCount = (offset + bitCount + FORMAT_BITS_PER_INT - 1) / FORMAT_BITS_PER_INT;
    FormatData *ret = NewFormatData (returnWordCount);
    unsigned destIndex = offset / FORMAT_BITS_PER_INT;
    unsigned sourceIndex = 0;
    unsigned destOffset = offset % FORMAT_BITS_PER_INT;
    unsigned lastWordBitCount = (offset + bitCount) % FORMAT_BITS_PER_INT;
    unsigned *source = data->words;
    unsigned *dest = ret->words + destIndex;

    while (destIndex < returnWordCount && sourceIndex < data->wordCount)
    {
        if (destOffset == 0)
            *dest = *source;
        else
        {
            *dest = (*source << destOffset) + (sourceIndex - 1 > 0 ? (source[-1] >> (FORMAT_BITS_PER_INT - destOffset)) : 0);
        }

        destIndex++;
        sourceIndex++;
        dest++; // Changed from *dest++ and *source++ 2007-04-05, check AB
        source++;
    }

    while (destIndex < returnWordCount)
    {
        destIndex++;
        *dest = 0;
        dest++;
    }

    if (lastWordBitCount != 0)
        ret->words[ret->wordCount - 1] &= (1 << lastWordBitCount) - 1;

    return ret;
}

/* FormatDataExtractBit : like ExtractBitField but only retreive a single bit
	into an int, nothing allocated */
int FormatDataExtractBit (FormatData * data, unsigned offset)
{
    return (data->words[offset / FORMAT_BITS_PER_INT] >> (offset % FORMAT_BITS_PER_INT)) & 1;
}

/* FormatDataNegate : negate a FormatData into a newly allocated FormatData
	(as it's possible for the result to be longer than the source).  `bitCount'
	specifies the length in bits of the result (ie. the sign bit will be in
	bit bitCount - 1.  May behave a bit loopy if any bits are set in `data'
	past bit bitCount - 1 */
FormatData *FormatDataNegate (FormatData * data, unsigned bitCount)
{
    unsigned retWordCount = (bitCount + FORMAT_BITS_PER_INT - 1) / FORMAT_BITS_PER_INT;
    FormatData *ret = NewFormatData (retWordCount);
    unsigned srcWordCount = data->wordCount;
    unsigned i = 0;
    unsigned carry = 1;

    if (retWordCount < srcWordCount)
        srcWordCount = retWordCount;

    while (i < srcWordCount)
    {
        ret->words[i] = (~data->words[i]) + carry;
        /* 0 with carry of 1 is the only combination which can give a subsequent carry:
           ~0 + 1 == 0xFFFFFFFF + 1 == 0x100000000 */
        carry = (data->words[i] == 0 && carry ? 1 : 0);
        i++;
    }

    /* Subsequent words are ~0 + carry, as ~0 + 1 = 0x100000000, ~0 + 0 = 0xFFFFFFFF,
       input and output carry must be the same, so we can just use the results
       inplace of the carry */
    carry = (carry ? 0 : -1);

    /* Fill the remaining result words */
    while (i < retWordCount)
    {
        ret->words[i] = carry;
        i++;
    }

    /* Now crop the last word if necessary */
    {
        unsigned lastWordBitCount = bitCount % FORMAT_BITS_PER_INT;

        if (lastWordBitCount != 0)
            ret->words[retWordCount - 1] &= (1 << lastWordBitCount) - 1;
    }

    return ret;
}

/* FormatDataBitwiseOr : compute the Or operation between the 2 arguments into
	a newly allocated FormatData
	(as it's possible for the result to be longer than the source). */
FormatData *FormatDataBitwiseOr (FormatData * data1, FormatData * data2)
{
    unsigned retWordCount = (data1->wordCount > data2->wordCount) ? data1->wordCount : data2->wordCount;
    FormatData *ret = NewFormatData (retWordCount);
    unsigned i;

    for (i = 0; i < retWordCount; i++)
    {
        if (i < data1->wordCount)
            ret->words[i] = data1->words[i];
        else
            ret->words[i] = 0;

        if (i < data2->wordCount)
            ret->words[i] |= data2->words[i];
    }

    return ret;
}

/* FormatDataEqual : return true if two format datas have the same value */
bool FormatDataEqual (FormatData * data1, FormatData * data2)
{
    bool equal = true;
    unsigned *shortData;
    unsigned *longData;
    unsigned shortLength;
    unsigned longLength;

    /* Sort the short and long data out, then compare upto the length of the
       short data, then check the remainder of the long word to make sure it's 0 */
    if (data1->wordCount < data2->wordCount)
    {
        shortData = data1->words;
        longData = data2->words;
        shortLength = data1->wordCount;
        longLength = data2->wordCount;
    } else
    {
        shortData = data2->words;
        longData = data1->words;
        shortLength = data2->wordCount;
        longLength = data1->wordCount;
    }

    int i = 0;

    while (i < shortLength && equal)
    {
        if (*shortData != *longData)
            equal = false;
        i++;
        shortData++;
        longData++;
    }
    while (i < longLength && equal)
    {
        if (*longData != 0)
            equal = false;
        i++;
        longData++;
    }

    return equal;
}
