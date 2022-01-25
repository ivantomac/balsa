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

	`misclists.h'
	Functions for manipulating lists/trees of miscellaneous types 
	
 */

#include "misclists.h"
#include "arith.h"

DEFINE_LIST_TYPE_CONSTRUCTOR (Ptrchar, String) DEFINE_LIST_TYPE_REVERSE (Ident) DEFINE_LIST_TYPE_REVERSE (int) DEFINE_LIST_TYPE_APPEND (int)
/* NewIdentList : create an IdentList element */
PtrIdentList NewIdentList (tIdent ident, tPosition position, PtrIdentList next)
{
    PtrIdentList elem = NEW (IdentList);

    CAR (elem).ident = ident;
    CAR (elem).position = position;
    elem->next = next;
    return elem;
}

/* NewintList : create an intList element */
PtrintList NewintList (int value, tPosition position, PtrintList next)
{
    PtrintList elem = NEW (intList);

    CAR (elem).value = value;
    CAR (elem).position = position;
    elem->next = next;
    return elem;
}

/* NewBindingList : create a BindingList (ident -> value mapping) element */
extern PtrBindingList NewBindingList (tIdent ident, PtrMP_INT value, tPosition position, PtrBindingList next)
{
    PtrBindingList elem = NEW (BindingList);

    CAR (elem).ident = ident;
    CAR (elem).value = value;
    CAR (elem).position = position;
    elem->next = next;
    return elem;
}

/* StrPtrintList : write out a list of integers with the string separator between them */
void StrPtrintList (FILE * stream, PtrintList list, char *separator)
{
    /* FOR_EACH (list, true, ...) */
    while (list)
    {
        fprintf (stream, "%d", CAR (list).value);
        if (CDR (list))
            fprintf (stream, "%s", separator);
        list = CDR (list);
    }
}

/* StrPtrIdentList : write out a list of identifiers with the string separator between them */
void StrPtrIdentList (FILE * stream, PtrIdentList list, char *separator)
{
    /* FOR_EACH (list, true, ...) */
    while (list)
    {
        WriteIdent (stream, CAR (list).ident);
        if (CDR (list))
            fprintf (stream, "%s", separator);
        list = CDR (list);
    }
}

/* StrPtrBindingList : write out a binding list with string separator between elements and
   string assignStr between ident and value */
void StrPtrBindingList (FILE * stream, PtrBindingList list, char *separator, char *assignStr)
{
    /* FOR_EACH (list, true, ...) */
    while (list)
    {
        WriteIdent (stream, CAR (list).ident);
        if (CAR (list).value)
        {
            fprintf (stream, "%s", assignStr);
            StrPtrMP_INT (stream, CAR (list).value);
        }
        if (CDR (list))
            fprintf (stream, "%s", separator);
        list = CDR (list);
    }
}

DEFINE_LIST_TYPE_CONSTRUCTOR (Lisp, Lisp) DEFINE_LIST_TYPE_REVERSE (Lisp) DEFINE_LIST_TYPE_COPY (Lisp)
/* StrLisp : print a single Lisp node */
void StrLisp (FILE * stream, Lisp lisp, Ptrchar separator)
{
    switch (lisp.nature)
    {
    case SymbolLispNature:
        WriteIdent (stream, lisp.value.symbol);
        break;
    case NumberLispNature:
        StrPtrMP_INT (stream, lisp.value.number);
        break;
    case StringLispNature:
        putc ('"', stream);
        /* FIXME, escaping */
        WriteIdent (stream, lisp.value.string);
        putc ('"', stream);
        break;
    case BooleanLispNature:
        fprintf (stream, "%s", SCHEME_BOOL_STRING (lisp.value.boolean));
        break;
    case SublistLispNature:
        putc ('(', stream);
        /* Once we're more than one list in just replecate the separator */
        StrPtrLispList (stream, lisp.value.sublist, separator, separator);
        putc (')', stream);
        break;
    }
}

/* StrPtrLispList : print a list of Lisp elements */
void StrPtrLispList (FILE * stream, PtrLispList list, Ptrchar separator, Ptrchar separator2)
{
    while (list)
    {
        StrLisp (stream, CAR (list), separator2);

        if (CDR (list))
            fprintf (stream, "%s", separator);
        list = CDR (list);
    }
}

/* NewLisp... : constructors for Lisp's and ListLists */
Lisp NewLispSymbol (tIdent value)
{
    Lisp ret;

    ret.nature = SymbolLispNature;
    ret.value.symbol = value;

    return ret;
}

Lisp NewLispNumber (PtrMP_INT value)
{
    Lisp ret;

    ret.nature = NumberLispNature;
    ret.value.number = value;

    return ret;
}

Lisp NewLispString (tIdent value)
{
    Lisp ret;

    ret.nature = StringLispNature;
    ret.value.string = value;

    return ret;
}

Lisp NewLispBoolean (bool value)
{
    Lisp ret;

    ret.nature = BooleanLispNature;
    ret.value.boolean = value;

    return ret;
}

Lisp NewLispSublist (PtrLispList value)
{
    Lisp ret;

    ret.nature = SublistLispNature;
    ret.value.sublist = value;

    return ret;
}

/* LispIsHeaded: returns true if the given Lisp node is a sublist
	with a symbol as its CAR that is also the same symbol as `symbol'
	(or any symbol if `symbol' is NoIdent) */
bool LispIsHeaded (Lisp node, tIdent symbol)
{
    return (node.nature == SublistLispNature &&
      node.value.sublist &&
      CAR (node.value.sublist).nature == SymbolLispNature && (symbol == NoIdent || symbol == CAR (node.value.sublist).value.symbol));
}

/* RemoveHeadedLispListElement: remove the first node from a PtrLispList
	with the head symbol `symbol'.  Returns true if an element was removed
	and the element body in `ret' (if non NULL).  `list' is also modified
	to remove the element */
bool RemoveHeadedLispListElement (PtrLispList * list, tIdent symbol, Lisp * ret)
{
    PtrLispList prev = NULL;
    PtrLispList iter = *list;
    bool returnStatus = false;

    while (iter && !returnStatus)
    {
        if (LispIsHeaded (CAR (iter), symbol))
        {
            if (prev)           /* not first element? */
                prev->next = CDR (iter);
            {                   /* 1st element hit, modify *list */
                *list = CDR (iter);
            }

            if (ret)
                *ret = CAR (iter);
            returnStatus = true;
        }
        prev = iter;
        iter = CDR (iter);
    }

    return returnStatus;
}
