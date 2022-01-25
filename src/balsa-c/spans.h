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

	`spans.h'
	Value `spans', ranges of [lower,upper] integers
	over a specified type.
	
 */

#ifndef SPANS_HEADER
#define SPANS_HEADER

#include "arith.h"
#include "misclists.h"

/* BeginSpans : Start up the slicin' 'n' dicin' system */
extern void BeginSpans (void);

struct Type;                    /* Forward decl */

/* Span : a type representing a range of elements (bits/array elems) not
   necessarily based at 0 */
typedef struct Span
{
    PtrMP_INT lowerBound, upperBound; /* upper and lower element indices 
                                         (inclusive, also upperBound >= lowerBound) */
    struct Type *boundingType;  /* Type calculated to hold numeric range or enumeration type
                                   over which the span cuts */
}
Span;

DECLARE_LIST_TYPE (Span, Span) DECLARE_CONS_LIST_TYPE (SpanList) DECLARE_LIST_TYPE_CONSTRUCTOR (Span, Span)
extern Span NoSpan;             /* Span with NULL values and NoType */

/* NewSpan : create a span from the given params */
extern Span NewSpan (PtrMP_INT lower, PtrMP_INT upper, struct Type *boundingType);

/* MakeSpanCaseSpecification : return an ident in the format suitable for
	Case specifications from a SpanListList */
extern Ptrchar MakeSpanCaseSpecification (PtrSpanListList list, Bits width);

/* MakeCaseSpecificationAcrossSpan : return an ident in the format suitable for Case
   specifications with each element a value from the range defined by the given span in
   reverse order */
extern Ptrchar MakeCaseSpecificationAcrossSpan (Span span, Bits width);

/* NormaliseSpanList : order a span list on the lowerBound value, remove redundant elements (those
   covered by other spans, amalgamate consecutive elements, change boundingType for all elements
   to that given */
extern PtrSpanList NormaliseSpanList (PtrSpanList list, struct Type *boundingType);

/* CombineNormalisedSpanLists : combine two normalised span lists into a single
   list by merging and normalising the lists.  If disjoint is not NULL then
   set it to reflect whether the two lists are disjoint */
extern PtrSpanList CombineNormalisedSpanLists (PtrSpanList list1, PtrSpanList list2, bool * disjoint);

/* MakeComplementarySpanList : given a normalised span list generate a complement to that span
   list (ie. such that all values in the given span lower->upper bound range are covered but the
   two lists (input and complement) are disjoint. NB. boundingSpan must express a range which is
   greater or equal to that of the existing list */
extern PtrSpanList MakeComplementarySpanList (PtrSpanList list, Span boundingSpan);

/* MakeMaskFromSpanList : make a mask containing 1's for each value covered in the ranges given in
   the span list given as an argument */
extern PtrMP_INT MakeMaskFromSpanList (PtrSpanList spans);

/* StrSpan : write out a span in the form (lowerBound as boundingType) .. (upperBound as boundingType)
   where lower and upper bound are numeric, or as lowerBound..upperBound iff longForm == false */
extern void StrSpan (FILE * stream, Span span, bool longForm);
extern void StrPtrSpanList (FILE * stream, PtrSpanList list, bool longForm, char *separator);
extern void StrPtrSpanListList (FILE * stream, PtrSpanListList list, bool longForm, char *listListSep, char *listSep);

#endif /* SPANS_HEADER */
