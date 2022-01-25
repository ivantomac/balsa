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

	`spans.c'
	Value `spans', ranges of [lower,upper] integers
	over a specified type.
	
 */

#include "types.h"
#include <string.h>

Span NoSpan = { NULL, NULL, NULL };

/* BeginSpans : Start up the slicin' 'n' dicin' system */
void BeginSpans (void)
{
    NoSpan.boundingType = NoType;
}

DEFINE_LIST_TYPE_CONSTRUCTOR (Span, Span) DEFINE_CONS_LIST_TYPE (SpanList)
/* NewSpan : create a span from the given params */
Span NewSpan (PtrMP_INT lower, PtrMP_INT upper, PtrType boundingType)
{
    Span ret;

    ret.upperBound = upper;
    ret.lowerBound = lower;
    ret.boundingType = boundingType;
    return ret;
}

#define CASE_SPEC_STR_LEN (4096)
/* MakeSpanCaseSpecification : return an ident in the format suitable for
	Case specifications from a SpanListList */
Ptrchar MakeSpanCaseSpecification (PtrSpanListList list, Bits width)
{
    int CaseSpecLength = CASE_SPEC_STR_LEN;
    Ptrchar buildStr = NEW_ARRAY (char, CaseSpecLength);

    *buildStr = '\0';
    /* FOR_EACH (list, true, ...) */
    while (list && true)
    {
        PtrSpanList subList = CAR (list);

        /* FOR_EACH (subList, true, ...) */
        while (subList && true)
        {
            mpz_get_str (EndOfString (buildStr), 10, FilterRangeFromMP_INT (CAR (subList).lowerBound, width, 0));
            if (mpz_cmp (CAR (subList).lowerBound, CAR (subList).upperBound) != 0)
            {
                strcat (buildStr, "..");
                mpz_get_str (EndOfString (buildStr), 10, FilterRangeFromMP_INT (CAR (subList).upperBound, width, 0));
            }
            if (CDR (subList))
                strcat (buildStr, ",");
            if (strlen (buildStr) > CaseSpecLength / 2)
            {
                CaseSpecLength *= 2;
                buildStr = EXTEND_ARRAY (buildStr, char, CaseSpecLength);
            }
            subList = CDR (subList);
        }
        if (CDR (list))
            strcat (buildStr, ";");
        list = CDR (list);
    }

    return buildStr;
}

/* MakeCaseSpecificationAcrossSpan : return an ident in the format suitable for Case
   specifications with each element a value from the range defined by the given span in
   reverse order */
Ptrchar MakeCaseSpecificationAcrossSpan (Span span, Bits width)
{
    int CaseSpecLength = CASE_SPEC_STR_LEN;
    Ptrchar buildStr = NEW_ARRAY (char, CaseSpecLength);
    PtrMP_INT iter = CopyMP_INT (span.upperBound);

    *buildStr = '\0';
    for (; mpz_cmp (iter, span.lowerBound) >= 0; mpz_sub_ui (iter, iter, 1))
    {
        mpz_get_str (buildStr + strlen (buildStr), 10, FilterRangeFromMP_INT (iter, width, 0));
        strcat (buildStr, ";");

        if (strlen (buildStr) > CaseSpecLength / 2)
        {
            CaseSpecLength *= 2;
            buildStr = EXTEND_ARRAY (buildStr, char, CaseSpecLength);
        }
    }
    /* strip the trailing ; */
    buildStr[strlen (buildStr) - 1] = '\0';

    DeleteMP_INT (iter);

    return buildStr;
}

/* MergeNormalisedSpanLists : merge two normalised span lists, resulting a list
   which is normalised (sorted by lowerBound value) */
PtrSpanList MergeNormalisedSpanLists (PtrSpanList list1, PtrSpanList list2)
{
    PtrSpanList mergedListHead = NewSpanList (NoSpan, NULL);
    PtrSpanList mergedListTail = mergedListHead;

    for (; list1 && list2; mergedListTail = CDR (mergedListTail))
    {
        if (mpz_cmp (CAR (list1).lowerBound, CAR (list2).lowerBound) < 0)
        {
            mergedListTail->next = NewSpanList (CAR (list1), NULL);
            list1 = CDR (list1);
        } else
        {
            mergedListTail->next = NewSpanList (CAR (list2), NULL);
            list2 = CDR (list2);
        }
    }
    mergedListTail->next = NULL;
    /* Combine tails */
    if (list1)
        mergedListTail->next = list1;
    else if (list2)
        mergedListTail->next = list2;

    return CDR (mergedListHead);
}

/* AmalgamateOverlappingAdjacentSpans : take a sorted (on lowerBound value)
   list of spans and amalgamate adjacent overlapping cells.
   if disjoint is not NULL then set it to true if no overlapping cells were found,
   false if list elements are not disjoint */
PtrSpanList AmalgamateOverlappingAdjacentSpans (PtrSpanList sortedList, bool * disjoint)
{
    PtrMP_INT tmp = NewMP_INT (0);
    PtrSpanList amalgamatedList, amalgamatedListHead;

    if (!sortedList)
        return NULL;
    if (disjoint)
        *disjoint = true;
    amalgamatedList = NewSpanList (NoSpan, sortedList);
    amalgamatedListHead = amalgamatedList;

    while (CDR (amalgamatedList) && CDDR (amalgamatedList))
    {
        mpz_add_ui (tmp, CADR (amalgamatedList).upperBound, 1);
        /* Is the end of this span into the next one (or abutting) */
        if (mpz_cmp (tmp, CADDR (amalgamatedList).lowerBound) >= 0)
        {
            if (disjoint &&     /* spans really are overlapping, not just abutting */
              mpz_cmp (CADR (amalgamatedList).upperBound, CADDR (amalgamatedList).lowerBound) >= 0)
                *disjoint = false;
            CADR (amalgamatedList).upperBound = CADDR (amalgamatedList).upperBound;
            amalgamatedList->next->next = CDDDR (amalgamatedList);
        } else
        {
            amalgamatedList = CDR (amalgamatedList);
        }
    }
    amalgamatedList->next->next = NULL;
    DeleteMP_INT (tmp);
    return CDR (amalgamatedListHead);
}

/* CombineNormalisedSpanLists : combine two normalised span lists into a single
   list by merging and normalising the lists.  If disjoint is not NULL then
   set it to reflect whether the two lists are disjoint */
PtrSpanList CombineNormalisedSpanLists (PtrSpanList list1, PtrSpanList list2, bool * disjoint)
{
    return AmalgamateOverlappingAdjacentSpans (MergeNormalisedSpanLists (list1, list2), disjoint);
}

/* NormaliseSpanList : order a span list on the lowerBound value, remove redundant elements (those
   covered by other spans, amalgamate consecutive elements, change boundingType for all elements
   to that given */
PtrSpanList NormaliseSpanList (PtrSpanList list, PtrType boundingType)
{
    PtrSpanList sortedList;

    if (!list)
        return NULL;            /* Skip */

    sortedList = NewSpanList (NoSpan, NewSpanList (CAR (list), NULL));

    list = CDR (list);
    /* FOR_EACH (list, true, ...) */
    while (list && true)        /* Insertion sort the list by the lowerBound values */
    {
        PtrSpanList lowerElements = sortedList;
        bool inserted = false;

        /* FOR_EACH (lowerElements, !inserted, ...) */
        while (lowerElements && !inserted)
        {
            /* Last span */
            if (!CDR (lowerElements))
            {
                lowerElements->next = NewSpanList (CAR (list), NULL);
                inserted = true;
            }
            /* Insert lowerBound <= lowerBound of next list element */
            if (mpz_cmp (CAR (list).lowerBound, CADR (lowerElements).lowerBound) <= 0)
            {
                lowerElements->next = NewSpanList (CAR (list), CDR (lowerElements));
                inserted = true;
            }
            lowerElements = CDR (lowerElements);
        }
        list = CDR (list);
    }

    sortedList = AmalgamateOverlappingAdjacentSpans (CDR (sortedList), NULL);

    /* Set type field */
    {
        PtrSpanList typeCorrectedList = sortedList;

        /* FOR_EACH (typeCorrectedList, true, ...) */
        while (typeCorrectedList && true)
        {
            CAR (typeCorrectedList).boundingType = boundingType;
            typeCorrectedList = CDR (typeCorrectedList);
        }
    }

    return sortedList;
}

/* MakeComplementarySpanList : given a normalised span list generate a complement to that span
   list (ie. such that all values in the given span lower->upper bound range are covered but the
   two lists (input and complement) are disjoint. NB. boundingSpan must express a range which is
   greater or equal to that of the existing list */
PtrSpanList MakeComplementarySpanList (PtrSpanList list, Span boundingSpan)
{
    PtrSpanList complementList;
    PtrSpanList complementTail;
    PtrMP_INT runningMinimum = NewMP_INT (0);

    if (!list || !boundingSpan.boundingType)
        return NULL;            /* Skip */

    complementList = NewSpanList (NoSpan, NULL); /* Sacrificial head */
    complementTail = complementList;

    mpz_set (runningMinimum, boundingSpan.lowerBound);

    /* FOR_EACH (list, true, ...) */
    while (list && true)
    {
        if (mpz_cmp (CAR (list).lowerBound, runningMinimum) > 0)
        {
            PtrMP_INT lowerBoundMinus1 = NewMP_INT (0);

            mpz_sub_ui (lowerBoundMinus1, CAR (list).lowerBound, 1);
            complementTail->next = NewSpanList (NewSpan (CopyMP_INT (runningMinimum), lowerBoundMinus1, boundingSpan.boundingType), NULL);
            complementTail = CDR (complementTail);
        }
        mpz_add_ui (runningMinimum, CAR (list).upperBound, 1);
        list = CDR (list);
    }
    /* Last element, upto boundingSpan.upperBound */
    if (mpz_cmp (runningMinimum, boundingSpan.upperBound) <= 0)
    {
        complementTail->next = NewSpanList (NewSpan (CopyMP_INT (runningMinimum), boundingSpan.upperBound, boundingSpan.boundingType), NULL);
        complementTail = CDR (complementTail);
    }
    DeleteMP_INT (runningMinimum);
    return CDR (complementList);
}

/* MakeBitStringWithNDontCares : print val into a newly allocated string of length bits+1
   where the first bits-dcCount bits are from val's most significant bits-dcCount bits and
   the lower dcCount bits are all -'s. NB. char at index n in the returned string represents
   bit bits-(n+1), ie. 1001--- represents the span [72,79], also val MUST be unsigned */
Ptrchar MakeBitStringWithNDontCares (unsigned bits, PtrMP_INT val, unsigned dcCount)
{
    unsigned size = mpz_sizeinbase (val, 2);
    Ptrchar str = NEW_ARRAY (char, bits + 1);
    unsigned getStrStart = bits - size;
    int count;

    /* val is unsigned */
    assert (mpz_sgn (val) >= 0);
    /* string is greater or equal in length to printed val */
    assert (bits >= size);

    /* Zero the zero padded initial bits */
    for (count = 0; count < getStrStart; count++)
        str[count] = '0';
    /* insert val */
    mpz_get_str (str + getStrStart, 2, val);
    /* - out last dcCount bits */
    for (count = bits - dcCount; count < bits; count++)
        str[count] = '-';
    str[bits] = '\0';

    return str;
}

/* MakeMaskFromSpanList : make a mask containing 1's for each value covered in the ranges given in
   the span list given as an argument */
PtrMP_INT MakeMaskFromSpanList (PtrSpanList spans)
{
    PtrMP_INT ret = NewMP_INT (0);

    /* FOR_EACH (spans, true, ...) */
    while (spans && true)
    {
        Bits lowerBound = mpz_get_ui (CAR (spans).lowerBound);
        PtrMP_INT thisMask = MakeMaskForRange (1 + mpz_get_ui (CAR (spans).upperBound) - lowerBound, lowerBound);

        mpz_ior (ret, ret, thisMask);
        DeleteMP_INT (thisMask);
        spans = CDR (spans);
    }
    return ret;
}

/* StrSpan : write out a span in the form (lowerBound as boundingType) .. (upperBound as boundingType)
   where lower and upper bound are numeric, or as lowerBound..upperBound iff longForm == false */
void StrSpan (FILE * stream, Span span, bool longForm)
{
    if (longForm)
    {
        fprintf (stream, "(");
        StrPtrMP_INT (stream, span.lowerBound);
        fprintf (stream, " as ");
        StrPtrTypeName (stream, span.boundingType);
        fprintf (stream, ")..(");
        StrPtrMP_INT (stream, span.upperBound);
        fprintf (stream, " as ");
        StrPtrTypeName (stream, span.boundingType);
        fprintf (stream, ")");
    } else
    {
        StrPtrMP_INT (stream, span.lowerBound);
        fprintf (stream, "..");
        StrPtrMP_INT (stream, span.upperBound);
    }
}

void StrPtrSpanList (FILE * stream, PtrSpanList list, bool longForm, char *separator)
{
    /* FOR_EACH (list, true, ...) */
    while (list && true)
    {
        StrSpan (stream, CAR (list), longForm);
        if (CDR (list))
            fprintf (stream, separator);
        list = CDR (list);
    }
}

void StrPtrSpanListList (FILE * stream, PtrSpanListList list, bool longForm, char *listListSep, char *listSep)
{
    /* FOR_EACH (list, true, ...) */
    while (list && true)
    {
        StrPtrSpanList (stream, CAR (list), longForm, listSep);
        if (CDR (list))
            fprintf (stream, listListSep);
        list = CDR (list);
    }
}
