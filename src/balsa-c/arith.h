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

	`arith.h'
	Multiprecision bitwise arithmetic functions, for use with GNU MP
	package of multiprecision arithmetic routines.
	
 */

#ifndef ARITHMETIC_HEADER
#define ARITHMETIC_HEADER

#include "misc.h"
#include <stdio.h>
#include <gmp.h>

/* PtrMP_INT : Pointer to an MP_INT */
typedef MP_INT *PtrMP_INT;

/* SmallestRangeToHoldValue : Returns the number of bits required to represent
   a given value. (-ve result for signed constants)
   NB: For an input value of 0, the output type is unsigned, 0 bits. 
   This allows 0's to be compiled out easily.  */
extern SignedBits SmallestRangeToHoldValue (PtrMP_INT value);

/* MinimumValueOfRange : return the minimum value for a type with size size */
extern PtrMP_INT MinimumValueOfRange (SignedBits size);

/* MaximumValueOfRange : return the maximum value for a type with size size */
extern PtrMP_INT MaximumValueOfRange (SignedBits size);

/* WidestRange : returns the narrowest range which can contain range1 & range2 */
extern SignedBits WidestRange (SignedBits range1, SignedBits range2);

/* RangeIsWiderOrEqual : true <= range2 can be contained in range1 */
extern bool RangeIsWiderOrEqual (SignedBits range1, SignedBits range2);

/* RecoverSign : sign extend a MP_INT with the bit width-1 */
extern void RecoverSign (PtrMP_INT val, Bits width);

/* NewMP_INT : Allocate space for an MP_INT, and set it to given value */
extern PtrMP_INT NewMP_INT (long initialValue);

/* CopyMP_INT : Allocate a new MP_INT, initialise to value of initialValue */
extern PtrMP_INT CopyMP_INT (PtrMP_INT initialValue);

/* DeleteMP_INT : Deallocate a NEWed MP_INT (clear it first) */
extern void DeleteMP_INT (PtrMP_INT integer);

/* IncPtrMP_INT : Add one to a copy of the given PtrMP_INT */
extern PtrMP_INT IncPtrMP_INT (PtrMP_INT inVal);

/* NegPtrMP_INT : Return the negated value of inVal */
extern PtrMP_INT NegPtrMP_INT (PtrMP_INT inVal);

/* MaxMP_INT : maximum of two MP_INT's */
extern PtrMP_INT MaxMP_INT (PtrMP_INT a, PtrMP_INT b);

/* MinMP_INT : minimum of two MP_INT's */
extern PtrMP_INT MinMP_INT (PtrMP_INT a, PtrMP_INT b);

/* StrPtrMP_INT : Output an MP_INT in decimal */
extern void StrPtrMP_INT (FILE * stream, PtrMP_INT a);

/* StrFilterMask : write a quoted binary number from the value filter (of width bits) 
   filter should be positive */
extern void StrFilterMask (FILE * stream, PtrMP_INT filter, Bits width);

/* MakeMaskForRange : make an MP_INT which has bits set in all bit positions given by the
   range [offset + width - 1, offset] */
extern PtrMP_INT MakeMaskForRange (Bits width, Bits offset);

/* FilterRangeFromMP_INT : make a mask for width and offset, AND the given value with the mask
   and return that ANDed value shifted right offset bits */
extern PtrMP_INT FilterRangeFromMP_INT (PtrMP_INT value, Bits width, Bits offset);

/* ReadBasedMP_INTFromString : Shortens the actions definitions for literal reading */
extern PtrMP_INT ReadBasedMP_INTFromString (char *str, int base, bool negative);

#endif /* ARITHMETIC_HEADER */
