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

	`arith.c'
	Multiprecision bitwise arithmetic functions, for use with GNU MP
	package of multiprecision arithmetic routines.
	
 */

#include "arith.h"

/* SmallestRangeToHoldValue : Returns the number of bits required to represent
   a given value. (-ve result for signed constants)
   NB: For an input value of 0, the output type is unsigned, 0 bits. 
   This allows 0's to be compiled out easily.
 */
SignedBits SmallestRangeToHoldValue (PtrMP_INT value)
{
    Bits unsignedLength = mpz_sizeinbase (value, 2);
    bool isNegative = mpz_sgn (value) < 0;
    SignedBits ret = unsignedLength;

    /* Zero */
    if (mpz_sgn (value) == 0)
        ret = 1;
    else if (isNegative)
    {
        mpz_t negativeLimit;

        /* Make negativeLimit equal to -(1 << (unsignedLength-1))
           is the most negative number that can be represented
           without an extra sign bit */
        mpz_init (negativeLimit);

        /* for unsigned length         XXXXXXXX */
        mpz_setbit (negativeLimit, unsignedLength - 1); /* eg 00000000 10000000 */
        mpz_neg (negativeLimit, negativeLimit); /* eg 11111111 10000000 */

        if (mpz_cmp (value, negativeLimit) < 0)
            ret++;              /* Add sign bit */

        mpz_clear (negativeLimit);

        return -ret;
    }
    return ret;
}

/* MinimumValueOfRange : return the minimum value for a type with size size */
PtrMP_INT MinimumValueOfRange (SignedBits size)
{
    PtrMP_INT ret;

    if (size > 0)
        ret = NewMP_INT (0);
    else
    {
        ret = NewMP_INT (1);
        mpz_mul_2exp (ret, ret, (-size) - 1);
        mpz_neg (ret, ret);
    }
    return ret;
}

/* MaximumValueOfRange : return the maximum value for a type with size size */
PtrMP_INT MaximumValueOfRange (SignedBits size)
{
    if (size < 0)
        size = (-size) - 1;
    return MakeMaskForRange (size, 0);
}

/* WidestRange : returns the narrowest range which can contain range1 & range2 */
SignedBits WidestRange (SignedBits range1, SignedBits range2)
{
    unsigned NegMax = 0;        /* Largest signed range */
    unsigned PosMax = 0;        /* Largest unsigned range */

    if (range1 < 0)
    {
        NegMax = -range1;
        PosMax = -range1;
    } else
        PosMax = range1;
    if (range2 < 0)
    {
        NegMax = Max (NegMax, -range2);
        PosMax = Max (NegMax, -range2);
    } else
        PosMax = Max (PosMax, range2);

    return (NegMax ? -Max (NegMax, PosMax) : PosMax);
}

/* RangeIsWiderOrEqual : true <= range2 can be contained in range1 */
bool RangeIsWiderOrEqual (SignedBits range1, SignedBits range2)
{
    /* ?,- : range1 must be signed if range2 is (reject +,-) */
    if (range2 < 0 && range1 >= 0)
        return false;
    /* -,- or +,+ : range1 must have >= bits than range2 */
    if ((range1 < 0) == (range2 < 0))
        if (Abs (range1) >= Abs (range2))
            return true;
    /* -,+ : range1 must be > range2 (to accomodate sign) */
    if (Abs (range1) > Abs (range2))
        return true;
    return false;
}

/* RecoverSign : sign extend a MP_INT with the bit width-1 */
void RecoverSign (PtrMP_INT val, Bits width)
{
    mpz_setbit (val, width);

    if (mpz_scan1 (val, width - 1) == width - 1)
    {
        PtrMP_INT bitMask = MakeMaskForRange (width, 0);

        mpz_com (bitMask, bitMask); /* 00001111 -> 11110000 */
        mpz_ior (val, val, bitMask);
        DeleteMP_INT (bitMask);
    } else
        mpz_clrbit (val, width);
}

/* NewMP_INT : Allocate a new MP_INT, initialise and set to given value */
PtrMP_INT NewMP_INT (long initialValue)
{
    PtrMP_INT ret = NEW (MP_INT);

    mpz_init_set_si (ret, initialValue);
    return ret;
}

/* CopyMP_INT : Allocate a new MP_INT, initialise to value of initialValue */
PtrMP_INT CopyMP_INT (PtrMP_INT initialValue)
{
    PtrMP_INT ret = NEW (MP_INT);

    mpz_init_set (ret, initialValue);
    return ret;
}

/* DeleteMP_INT : Deallocate a NEWed MP_INT (clear it first) */
void DeleteMP_INT (PtrMP_INT integer)
{
    mpz_clear (integer);
    FREE_ARRAY (MP_INT, 1, integer);
}

/* IncPtrMP_INT : Add one to a copy of the given PtrMP_INT */
PtrMP_INT IncPtrMP_INT (PtrMP_INT inVal)
{
    PtrMP_INT newInt = NewMP_INT (0);

    mpz_add_ui (newInt, inVal, 1);
    return newInt;
}

/* NegPtrMP_INT : Return the negated value of inVal */
PtrMP_INT NegPtrMP_INT (PtrMP_INT inVal)
{
    PtrMP_INT newInt = NewMP_INT (0);

    mpz_neg (newInt, inVal);
    return newInt;
}

/* MaxMP_INT : maximum of two MP_INT's */
PtrMP_INT MaxMP_INT (PtrMP_INT a, PtrMP_INT b)
{
    bool aGTb = mpz_cmp (a, b) > 0;

    return (aGTb ? a : b);
}

/* MinMP_INT : minimum of two MP_INT's */
PtrMP_INT MinMP_INT (PtrMP_INT a, PtrMP_INT b)
{
    bool aGTb = mpz_cmp (a, b) > 0;

    return (aGTb ? b : a);
}

/* StrPtrMP_INT : Output an MP_INT in decimal */
void StrPtrMP_INT (FILE * stream, PtrMP_INT a)
{
    if (!a)
        fprintf (stream, "NULL");
    else
        mpz_out_str (stream, 10, a);
}

/* StrFilterMask : write a quoted binary number from the value filter (of width bits) 
   filter should be positive */
void StrFilterMask (FILE * stream, PtrMP_INT filter, Bits width)
{
    int leadingZeros = width - mpz_sizeinbase (filter, 2);

    putc ('"', stream);
    for (; leadingZeros > 0; leadingZeros--)
        putc ('0', stream);
    mpz_out_str (stream, 2, filter);
    putc ('"', stream);
}

/* MakeMaskForRange : make an MP_INT which has bits set in all bit positions given by the
   range [offset + width - 1, offset] */
PtrMP_INT MakeMaskForRange (Bits width, Bits offset)
{
    PtrMP_INT mask = NewMP_INT (0);
    mpz_t tmp;

    mpz_init (tmp);

    /* The mask is (((1 << width) - 1) << offset)) */

    mpz_setbit (tmp, width);
    mpz_sub_ui (tmp, tmp, 1);   /* tmp = (1 << width) - 1 */
    mpz_mul_2exp (mask, tmp, offset); /* mask = ((1 << width - 1) << offset */

    mpz_clear (tmp);

    return mask;
}

/* FilterRangeFromMP_INT : make a mask for width and offset, AND the given value with the mask
   and return that ANDed value shifted right offset bits */
PtrMP_INT FilterRangeFromMP_INT (PtrMP_INT value, Bits width, Bits offset)
{
    PtrMP_INT mask = MakeMaskForRange (width, offset);

    mpz_and (mask, value, mask); /* mask= value & mask */
    mpz_div_2exp (mask, mask, offset); /* mask >>= offset */

    return mask;
}

/* ReadBasedMP_INTFromString : Shortens the actions definitions for literal reading */
PtrMP_INT ReadBasedMP_INTFromString (char *str, int base, bool negative)
{
    PtrMP_INT ret = NewMP_INT (0);

    RemoveCharFromString (str, '_');
    mpz_init_set_str (ret, str, base);
    if (negative)
        mpz_neg (ret, ret);

    return ret;
}
