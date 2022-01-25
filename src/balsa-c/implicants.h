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

	`implicants.c'
	Implicants of the form: baseValue, dontCares as used in case statements
	and functions for working between implicants and spans.
	
 */

#ifndef IMPLICANTS_HEADER
#define IMPLICANTS_HEADER

#include "arith.h"
#include "spans.h"

typedef struct Implicant
{
    PtrMP_INT baseValue, dontCares;
}
Implicant;

DECLARE_LIST_TYPE (Implicant, Implicant)
DECLARE_CONS_LIST_TYPE (ImplicantList) DECLARE_LIST_TYPE_CONSTRUCTOR (Implicant, Implicant) DECLARE_LIST_TYPE_APPEND (Implicant)
extern Implicant NoImplicant;

/* NewImplicant : create a implicant from the given params */
extern Implicant NewImplicant (PtrMP_INT baseValue, PtrMP_INT dontCares);

/* CopyImplicant : make a copy of an implicant, allocating new MP_INTs */
extern Implicant CopyImplicant (Implicant imp);

/* MakeImplicantCaseSpecification : return an ident in the format suitable for
	Case specifications from an ImplicantListList */
extern Ptrchar MakeImplicantCaseSpecification (PtrImplicantListList list, Bits width);

/* MakeSpanImplicantList : make a list of Implicants which corresponds to the
	range of the given Span */
extern PtrImplicantList MakeSpanImplicantList (Span span);

/* ValueLiesInImplicant : return true if the given value is matched by in the given implicant */
extern bool ValueLiesInImplicant (Implicant implicant, PtrMP_INT value);

/* ValueLiesInImplicantList : return true if the given value is
	matched by any of the given implicants */
extern bool ValueLiesInImplicantList (PtrImplicantList implicants, PtrMP_INT value);

/* ImplicantIsDisjointFromImplicant : return true if the given implicants cover none
	of the same values */
extern bool ImplicantIsDisjointFromImplicant (Implicant i1, Implicant i2);

/* ImplicantsAreDisjoint : returns true if all the implicants in the list are disjoint
	from all the others */
extern bool ImplicantsAreDisjoint (PtrImplicantList imps);

/* RemoveImplicantFromImplicants : returns a list of implicants which correspond to all
	the values matched in `implicants' but without any values matches by `implicant'.
	*`disjoint' (if `disjoint' is not NULL) is reset if all of the implicants in `implicants' are
	disjoint from `implicant' */
extern PtrImplicantList RemoveImplicantFromImplicants (PtrImplicantList implicants, Implicant implicant, bool * disjoint);

/* RemoveImplicantsFromImplicants : returns a list of implicants which correspond to all
	the values matched in `left' but without any values matches by `right'.
	*`disjoint' (if `disjoint' is not NULL) is reset if all of the implicants in `implicants' are
	disjoint from `implicant' */
extern PtrImplicantList RemoveImplicantsFromImplicants (PtrImplicantList left, PtrImplicantList right, bool * disjoint);

/* NextMatchFromImplicantList : iterate over the values in a given implicant
	list, returning a new MP_INT each time and NULL after all the values in the
	implicant list have been exhausted.  Uses the variable `*count' as a temporary
	storage for iterator values and the list `*imps' is modified as implicants are
	exhausted.  Set `count' to point to a NULL MP_INT at first call */
extern PtrMP_INT NextMatchFromImplicantList (PtrImplicantList * imps, PtrMP_INT * count);

/* FindRangeOfImplicants : returns the number of bits required to represent
	all the implicants in the given list.  The returned number is negative if
	the range is a number of signed bits */
extern SignedBits FindRangeOfImplicants (PtrImplicantList imps);

/* TrimImplicants : trim baseValues and dontCaress in an implicant list using
	the provided mask. */
extern void TrimImplicants (PtrImplicantList imps, PtrMP_INT mask);

/* ReadImplicantFromString : read a literal containing Xs and return an appropriate
	binary string corresponding to that literal, base can be either 2 or 16 */
extern Implicant ReadImplicantFromString (char *str, int base);

/* MakeImplicantString,StrImplicant : write out an implicant in the form "`baseValue'm`dontCares'"
	where the `baseValue' is a bit mask specifying the locations of the 1s in the implicant
	and `dontCares' is a bit mask specifying the locations of the Xs in the implicant
	or 0b11100X... if inputForm is true */
extern Ptrchar MakeImplicantString (Implicant implicant, bool inputForm);
extern void StrImplicant (FILE * stream, Implicant implicant, bool inputForm);

extern void StrPtrImplicantList (FILE * stream, PtrImplicantList list, bool inputForm, char *separator);

#endif
