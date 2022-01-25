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

#include "implicants.h"
#include "misc.h"
#include "types.h"
#include <string.h>
#include <ctype.h>

Implicant NoImplicant = { NULL, NULL };

DEFINE_LIST_TYPE_CONSTRUCTOR (Implicant, Implicant) DEFINE_LIST_TYPE_APPEND (Implicant) DEFINE_CONS_LIST_TYPE (ImplicantList)
/* NewImplicant : create a implicant from the given params */
Implicant NewImplicant (PtrMP_INT baseValue, PtrMP_INT dontCares)
{
    Implicant ret;

    ret.baseValue = baseValue;
    ret.dontCares = dontCares;
    return ret;
}

/* CopyImplicant : make a copy of an implicant, allocating new MP_INTs */
Implicant CopyImplicant (Implicant imp)
{
    Implicant ret;

    ret.baseValue = CopyMP_INT (imp.baseValue);
    ret.dontCares = (imp.dontCares ? CopyMP_INT (imp.dontCares) : NULL);

    return ret;
}

/* MakeSpanImplicantList : make a list of Implicants which corresponds to the
	range of the given Span */
PtrImplicantList MakeSpanImplicantList (Span span)
{
    PtrMP_INT min = NewMP_INT (0); /* Minimum value (0->n-1) eg. 00000010100101 */
    PtrMP_INT max = NewMP_INT (0); /* Maximum value (0->n-1) eg. 11111110100101 */
    PtrMP_INT start = NewMP_INT (0); /* Original start value */
    PtrMP_INT dcSpan = NewMP_INT (0); /* temporary */
    int bitNo;
    PtrImplicantList ret = NULL;

    ASSERT (span.boundingType);
    ASSERT (mpz_cmp (span.upperBound, span.lowerBound) >= 0); /* upperBound >= lowerBound */

    mpz_set (start, span.lowerBound);
    do
    {
        mpz_set (max, start);   /* Begin with the start value as max, min */
        mpz_set (min, start);

        bitNo = -1;

        /* Within range? */
        do
        {
            bitNo++;
            mpz_setbit (max, bitNo);
            mpz_clrbit (min, bitNo);
        }
        while (mpz_cmp (start, min) <= 0 && mpz_cmp (span.upperBound, max) >= 0);
        /* Min < start or Max > end therefore. bitNo LSBits are dc's */

        ret = NewImplicantList (NewImplicant (CopyMP_INT (start), MakeMaskForRange (bitNo, 0)), ret);

        mpz_set_ui (dcSpan, 0);
        mpz_setbit (dcSpan, bitNo); /* dcSpan := 1 << bitNo */
        mpz_add (start, start, dcSpan);
    }
    while (mpz_cmp (start, span.upperBound) <= 0); /* Until start > end */

    DeleteMP_INT (min);
    DeleteMP_INT (max);
    DeleteMP_INT (dcSpan);
    DeleteMP_INT (start);

    return ret;
}

#define IMPLICANT_SEPARATOR "m"

#define CASE_SPEC_STR_LEN (4096)
/* MakeImplicantCaseSpecification : return an ident in the format suitable
	for Case specifications from an ImplicantListList */
Ptrchar MakeImplicantCaseSpecification (PtrImplicantListList list, Bits width)
{
    int CaseSpecLength = CASE_SPEC_STR_LEN;
    Ptrchar buildStr = NEW_ARRAY (char, CaseSpecLength);
    Ptrchar buildStrPtr = buildStr;

    *buildStr = '\0';
    while (list)
    {
        PtrImplicantList subList = CAR (list);

        while (subList)
        {
            mpz_get_str (buildStrPtr, 10, FilterRangeFromMP_INT (CAR (subList).baseValue, width, 0));
            buildStrPtr = EndOfString (buildStrPtr);
            if (CAR (subList).dontCares && mpz_sgn (CAR (subList).dontCares) != 0)
            {
                strcat (buildStrPtr, IMPLICANT_SEPARATOR);
                mpz_get_str (buildStrPtr + 1, 10, FilterRangeFromMP_INT (CAR (subList).dontCares, width, 0));
                buildStrPtr = EndOfString (buildStrPtr);
            }
            if (CDR (subList))
                strcat (EndOfString (buildStrPtr), ",");
            buildStrPtr = EndOfString (buildStrPtr);
            subList = CDR (subList);

            if (buildStrPtr - buildStr > CaseSpecLength / 2)
            {
                CaseSpecLength *= 2;
                Ptrchar newBuildStr = EXTEND_ARRAY (buildStr, char, CaseSpecLength);

                if (newBuildStr != buildStr)
                {
                    buildStrPtr = newBuildStr + (buildStrPtr - buildStr);
                    buildStr = newBuildStr;
                }
            }
        }
        buildStrPtr = EndOfString (buildStrPtr);
        if (CDR (list))
            strcat (buildStrPtr, ";");
        buildStrPtr = EndOfString (buildStrPtr);
        list = CDR (list);
    }

    return buildStr;
}

/* ValueLiesInImplicant : return true if the given value is matched by in the given implicant */
bool ValueLiesInImplicant (Implicant implicant, PtrMP_INT value)
{
    bool ret = false;
    PtrMP_INT tmp = NewMP_INT (0);

    if (implicant.dontCares)    /* Otherwise care about all the bits */
    {
        mpz_com (tmp, implicant.dontCares);
        mpz_and (tmp, value, tmp);
    } else
    {
        mpz_set (tmp, value);
    }

    ret = mpz_cmp (tmp, implicant.baseValue) == 0;

    DeleteMP_INT (tmp);
    return ret;
}

/* ValueLiesInImplicantList : return true if the given value is matched by any of the given implicants */
bool ValueLiesInImplicantList (PtrImplicantList implicants, PtrMP_INT value)
{
    while (implicants)
    {
        if (ValueLiesInImplicant (CAR (implicants), value))
        {
            return true;
        }
        implicants = CDR (implicants);
    }
    return false;
}

/* ImplicantIsDisjointFromImplicant : return true if the given implicants cover none
	of the same values */
bool ImplicantIsDisjointFromImplicant (Implicant i1, Implicant i2)
{
    bool disjoint;
    PtrMP_INT tmpL = NewMP_INT (0);
    PtrMP_INT tmpR = NewMP_INT (0);

    if (i2.dontCares)
        mpz_and (tmpL, i1.baseValue, i2.dontCares);

    mpz_ior (tmpL, tmpL, i2.baseValue);

    if (i1.dontCares)
        mpz_and (tmpR, i2.baseValue, i1.dontCares);

    mpz_ior (tmpR, tmpR, i1.baseValue);

    disjoint = mpz_cmp (tmpL, tmpR) != 0;

    DeleteMP_INT (tmpL);
    DeleteMP_INT (tmpR);
    return disjoint;
}

/* ImplicantsAreDisjoint : returns true if all the implicants in the list are disjoint
	from all the others */
bool ImplicantsAreDisjoint (PtrImplicantList imps)
{
    bool disjoint = true;

    while (imps && disjoint)
    {
        PtrImplicantList restOfList = CDR (imps);

        while (restOfList && disjoint)
        {
            if (!ImplicantIsDisjointFromImplicant (CAR (imps), CAR (restOfList)))
                disjoint = false;
            restOfList = CDR (restOfList);
        }
        imps = CDR (imps);
    }

    return disjoint;
}

/* RemoveImplicantFromImplicant : returns a list of implicants which correspond to all
	the values matched in `l' but without any values matches by `r'. */
PtrImplicantList RemoveImplicantFromImplicant (Implicant l, Implicant r)
{
    PtrImplicantList ret = NULL;
    PtrMP_INT dontCares = (l.dontCares ? CopyMP_INT (l.dontCares) : NewMP_INT (0));
    Implicant result = CopyImplicant (l);
    int bitNo = 0;

    while (mpz_sgn (dontCares) != 0)
    {
        if (r.dontCares ? mpz_tstbit (r.dontCares, bitNo) : 0) /* right don't care, use left value */
            mpz_clrbit (dontCares, bitNo);
        else if (l.dontCares ? mpz_tstbit (l.dontCares, bitNo) : 0) /* left don't care, build fragments */
        {
            bool rBitSet = mpz_tstbit (r.baseValue, bitNo);
            Implicant newImp;

            /* remove the don't care */
            mpz_clrbit (result.dontCares, bitNo);
            newImp = CopyImplicant (result);

            if (rBitSet)        /* 1 in r -> 1 in result */
                mpz_setbit (result.baseValue, bitNo);
            else
            {                   /* 0 in r -> 1 in newImp */
                mpz_setbit (newImp.baseValue, bitNo);
            }
            ret = NewImplicantList (newImp, ret);
            mpz_clrbit (dontCares, bitNo);
        }

        bitNo++;
    }

    return ret;
}

/* RemoveImplicantFromImplicants : returns a list of implicants which correspond to all
	the values matched in `implicants' but without any values matches by `implicant'.
	*`disjoint' (if `disjoint' is not NULL) is reset if all of the implicants in `implicants' are
	disjoint from `implicant' */
PtrImplicantList RemoveImplicantFromImplicants (PtrImplicantList implicants, Implicant implicant, bool * disjoint)
{
    PtrImplicantList ret = NULL;
    bool allDisjoint = true;

    while (implicants)
    {
        if (ImplicantIsDisjointFromImplicant (CAR (implicants), implicant)) /* just include disjoint imps. */
        {
            ret = NewImplicantList (CAR (implicants), ret);
        } else
        {
            allDisjoint = false;
            ret = AppendImplicantLists (RemoveImplicantFromImplicant (CAR (implicants), implicant), ret);
        }

        implicants = CDR (implicants);
    }

    if (disjoint && !allDisjoint)
        *disjoint = false;
    return ret;
}

/* RemoveImplicantsFromImplicants : returns a list of implicants which correspond to all
	the values matched in `left' but without any values matches by `right'.
	*`disjoint' (if `disjoint' is not NULL) is reset if all of the implicants in `implicants' are
	disjoint from `implicant' */
PtrImplicantList RemoveImplicantsFromImplicants (PtrImplicantList left, PtrImplicantList right, bool * disjoint)
{
    PtrImplicantList ret = left;
    bool allDisjoint = true;

    while (right)
    {
        ret = RemoveImplicantFromImplicants (ret, CAR (right), &allDisjoint);
        right = CDR (right);
    }

    if (disjoint && !allDisjoint)
        *disjoint = false;
    return ret;
}

/* FindRangeOfImplicants : returns the number of bits required to represent
	all the implicants in the given list.  The returned number is negative if
	the range is a number of signed bits */
SignedBits FindRangeOfImplicants (PtrImplicantList imps)
{
    PtrMP_INT ORed = NewMP_INT (0);
    SignedBits impsRange;

    while (imps)
    {
        mpz_ior (ORed, ORed, CAR (imps).baseValue);
        if (CAR (imps).dontCares)
            mpz_ior (ORed, ORed, CAR (imps).dontCares);

        imps = CDR (imps);
    }
    impsRange = SmallestRangeToHoldValue (ORed);
    DeleteMP_INT (ORed);

    return impsRange;
}

/* TrimImplicants : trim baseValues and dontCaress in an implicant list using
	the provided mask. */
void TrimImplicants (PtrImplicantList imps, PtrMP_INT mask)
{
    /* Trim implicants by real range */
    while (imps)
    {
        mpz_and (CAR (imps).baseValue, CAR (imps).baseValue, mask);
        if (CAR (imps).dontCares)
            mpz_and (CAR (imps).dontCares, CAR (imps).dontCares, mask);

        imps = CDR (imps);
    }
}

/* DistributeMP_INTOverMask : distribute the bits of a given MP_INT over the
	bits set in the mask */
void DistributeMP_INTOverMask (PtrMP_INT dst, PtrMP_INT mask, PtrMP_INT value)
{
    Bits maskLength = mpz_popcount (mask);
    Bits bitNo = 0;
    Bits maskPosition = 0;

    while (bitNo < maskLength)
    {
        maskPosition = mpz_scan1 (mask, maskPosition);

        if (mpz_tstbit (value, bitNo))
            mpz_setbit (dst, maskPosition);
        else
            mpz_clrbit (dst, maskPosition);
        bitNo++;
        maskPosition++;
    }
}

/* NextMatchFromImplicantList : iterate over the values in a given implicant
	list, returning a new MP_INT each time and NULL after all the values in the
	implicant list have been exhausted.  Uses the variable `*count' as a temporary
	storage for iterator values and the list `*imps' is modified as implicants are
	exhausted.  Set `count' to point to a NULL MP_INT at first call */
PtrMP_INT NextMatchFromImplicantList (PtrImplicantList * imps, PtrMP_INT * count)
{
    PtrMP_INT ret = NULL;

    if (!*imps)                 /* Last iteration */
    {
        if (*count)
            DeleteMP_INT (*count);
        return NULL;
    }

    ret = CopyMP_INT (CAR (*imps).baseValue);

    if (!*count)                /* First iteration of each implicant */
    {                           /* Start at the last value and count down */
        *count = (CAR (*imps).dontCares ? CopyMP_INT (CAR (*imps).dontCares) : NewMP_INT (0));
    } else                      /* A middle iteration */
    {
        mpz_sub_ui (*count, *count, 1);
    }
    if (CAR (*imps).dontCares)
        DistributeMP_INTOverMask (ret, CAR (*imps).dontCares, *count);

    if (mpz_sgn (*count) == 0)
    {
        *imps = CDR (*imps);
        *count = NULL;
    }

    return ret;
}

/* ReadImplicantFromString : read a literal containing Xs and return an appropriate
	binary string corresponding to that literal, base can be either 2 or 16 */
Implicant ReadImplicantFromString (char *str, int base)
{
    char *strPtr;
    int shiftDist = 1;          /* shift distance for last addition */
    PtrMP_INT baseValue = NewMP_INT (0);
    PtrMP_INT dontCares = NewMP_INT (0);

    RemoveCharFromString (str, '_');
    strPtr = str;

    while (*strPtr)
    {
        char chr = toupper (*strPtr);

        strPtr++;
        mpz_mul_2exp (baseValue, baseValue, shiftDist);
        mpz_mul_2exp (dontCares, dontCares, shiftDist);

        if (base == 16)
        {
            shiftDist = 4;

            if (chr == 'X' || chr == '?')
                mpz_add_ui (dontCares, dontCares, 15);
            else
                mpz_add_ui (baseValue, baseValue, (isdigit (chr) ? chr - '0' : 10 + (chr - 'A')));
        } else
        {
            shiftDist = 1;
            switch (chr)
            {
            case '1':
                mpz_add_ui (baseValue, baseValue, 1);
                break;
            case 'X':
            case '?':
                mpz_add_ui (dontCares, dontCares, 1);
                break;
            }
        }
    }

    return NewImplicant (baseValue, dontCares);
}

/* MakeImplicantString,StrImplicant : write out an implicant in the form "`baseValue'm`dontCares'"
	where the `baseValue' is a bit mask specifying the locations of the 1s in the implicant
	and `dontCares' is a bit mask specifying the locations of the Xs in the implicant
	or 0b11100X... if inputForm is true */
Ptrchar MakeImplicantString (Implicant implicant, bool inputForm)
{
    char *ret = NULL;

    if (inputForm)
    {
        int printLength = Max ((implicant.dontCares ? mpz_sizeinbase (implicant.dontCares, 2) : 0),
          mpz_sizeinbase (implicant.baseValue, 2));
        int bitNo = 0;
        char *print = NEW_ARRAY (char, printLength + 1);
        char *printPtr = print + printLength - 1;

        print[printLength] = '\0';

        while (bitNo < printLength)
        {
            *printPtr = (mpz_tstbit (implicant.baseValue, bitNo) ?
              '1' : (implicant.dontCares ? (mpz_tstbit (implicant.dontCares, bitNo) ? 'X' : '0') : '0'));
            printPtr--;
            bitNo++;
        }
        ret = g_strconcat ("0b", print, NULL);
        FREE_ARRAY (char, printLength + 1, print);
    } else
    {
        int retLength = (implicant.dontCares ? mpz_sizeinbase (implicant.dontCares, 2) : 1) + mpz_sizeinbase (implicant.baseValue,
          2) + strlen (IMPLICANT_SEPARATOR) + 1;
        char *retPtr = ret;
        ret = NEW_ARRAY (char, retLength);

        mpz_get_str (ret, 10, implicant.baseValue);
        retPtr = EndOfString (ret);

        strcat (retPtr, IMPLICANT_SEPARATOR);
        retPtr = EndOfString (ret);

        if (implicant.dontCares)
            mpz_get_str (retPtr, 10, implicant.dontCares);
        else
            strcat (retPtr, "0");
    }
    return ret;
}

void StrImplicant (FILE * stream, Implicant implicant, bool inputForm)
{
    char *str = MakeImplicantString (implicant, inputForm);

    fprintf (stream, "%s", str);
    g_free (str);
}

void StrPtrImplicantList (FILE * stream, PtrImplicantList list, bool inputForm, char *separator)
{
    while (list)
    {
        StrImplicant (stream, CAR (list), inputForm);
        if (CDR (list))
            fprintf (stream, separator);

        list = CDR (list);
    }
}
