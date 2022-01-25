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

	`data.h'
	Word array bigint manipulation

*/

#ifndef FORMAT_DATA_HEADER
#define FORMAT_DATA_HEADER

#ifdef HAVE_CONFIG_H
# include <config.h>
# ifdef HAVE_STDINT_H
#  include <stdint.h>
#  define FORMAT_LIB_UINT64 uint64_t
# elif defined (HAVE_INTTYPES_H)
#  include <inttypes.h>
#  define FORMAT_LIB_UINT64 uint64_t
# else
#  define FORMAT_LIB_UINT64 unsigned long long
# endif
#else
# define FORMAT_LIB_UINT64 unsigned long long
#endif

#include <stdbool.h>

typedef FORMAT_LIB_UINT64 FormatUInt64;

typedef struct
{
    unsigned wordCount;
    unsigned words[];           /* Alloc more here to size `wordCount' */
}
FormatData;

/* NewFormatData : make a new, 0 initialised FormatData structure with `wordCount' words */
extern FormatData *NewFormatData (unsigned wordCount);

/* FormatDataSetWords : set each word in the given FormatData to the given word value */
extern void FormatDataSetWords (FormatData * data, unsigned word);

/* FormatBitCountToWordCount : returns the number of words needed to represent bitCount bits */
extern unsigned FormatBitCountToWordCount (unsigned bitCount);

/* NewFormatDataFromUnsignedInt : make a new format data array initialised from the
	unsigned int value */
extern FormatData *NewFormatDataFromUnsignedInt (unsigned wordCount, unsigned int value);

/* CopyFormatData : make a copy of a FormatData */
extern FormatData *CopyFormatData (FormatData * data);

/* CopyFormatDataWords : copy data from one FormatData to another (clipping to
	the wordCount of the destination */
extern void CopyFormatDataWords (FormatData * dest, FormatData * src);

/* DeleteFormatData : free a FormatData struct */
extern void DeleteFormatData (FormatData * data);

/* FormatDataLengthInRadix : returns the maximum number of characters
	needed to print a value with the number of words `wordCount'
	without sign/radix indication */
extern unsigned FormatDataLengthInRadix (unsigned wordCount, int radix);

/* FormatDataParseUInt : parse a format data value (as an unsigned
	int) out of a string */
extern FormatData *FormatDataParseUInt (const char *str, int radix);

/* FormatDataParseUIntWithBuf :  same as FormatDataParseUInt but modify the
   string pointer to point after the end of the parsed number */
extern FormatData *FormatDataParseUIntWithBuf (char **str, int radix);

/* FormatDataFormatAsUInt : format a FormatData as an unsigned integer (in
	decimal for the moment.  Formats into buffer and returns a pointer to
	the end of the formatted string.  Inserts leading zeroes to length `zeroedLength'
	if zeroedLength != 0 */
extern char *FormatDataFormatAsUInt (char *buffer, FormatData * data, int radix, unsigned zeroedLength);

/* FormatDataExtractBitField : extract a bit field from the given FormatData and
	return a freshly allocated FormatData containing that bit field.  Note that the
	newly allocated FormatData is only as large as it needs to be */
extern FormatData *FormatDataExtractBitField (FormatData * data, unsigned offset, unsigned bitCount);

/* FormatDataInsertBitField : insert data src into dest dst at the given offset and bitCount.
	src->wordCount must be >= FormatBitCountToWordCount (bitCount), insertion of the bit field
	must also not require dst to grow */
extern void FormatDataInsertBitField (FormatData * dst, FormatData * src, unsigned offset, unsigned bitCount);

/* FormatDataExtractPointer : extract a pointer value from a data word, the extracted pointer
	should be read from `bitCount' significant bits starting at offset `offset' */
extern void *FormatDataExtractPointer (FormatData * data, unsigned offset, unsigned bitCount);

/* FormatDataInsertPointer : insert a pointer into the given FormatData, making
	it occupy the bitfield [bitCount+offset-1:offset] */
extern void FormatDataInsertPointer (FormatData * data, void *pointer, unsigned offset, unsigned bitCount);

/* FormatDataApplyBitField : Map the given FormatData onto a bit field and
	return a freshly allocated FormatData. */
FormatData *FormatDataApplyBitField (FormatData * data, unsigned offset, unsigned bitCount);

/* FormatDataExtractBit : like ExtractBitField but only retreive a single bit
	into an int, nothing allocated */
extern int FormatDataExtractBit (FormatData * data, unsigned offset);

/* FormatDataNegate : negate a FormatData into a newly allocated FormatData
	(as it's possible for the result to be longer than the source).  `bitCount'
	specifies the length in bits of the result (ie. the sign bit will be in
	bit bitCount - 1.  May behave a bit loopy if any bits are set in `data'
	past bit bitCount - 1 */
extern FormatData *FormatDataNegate (FormatData * data, unsigned bitCount);

/* FormatDataBitwiseOr : compute the Or operation between the 2 arguments into
	a newly allocated FormatData
	(as it's possible for the result to be longer than the source). */
extern FormatData *FormatDataBitwiseOr (FormatData * data1, FormatData * data2);

/* FormatDataEqual : return true if two format datas have the same value */
extern bool FormatDataEqual (FormatData * data1, FormatData * data2);

/* FormatDataMultiplyThenAdd : multiply a FormatData by a small unsigned integer and then
	add another small integer, may reallocate `multiplicand' if it is too small to hold the
	result, hence the ** */
extern void FormatDataMultiplyThenAdd (FormatData ** multiplicand, unsigned multValue, unsigned addValue);

#endif
