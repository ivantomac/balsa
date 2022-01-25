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

#ifndef MISCLISTS_HEADER
#define MISCLISTS_HEADER

#include "lists.h"
#include "arith.h"
#include "Positions.h"
#include <Idents.h>

/* Lists of (tPosition x tIdent), (tPosition x int), (tIdent, PtrMP_INT), Ptrchar */
DECLARE_LIST_TYPE (struct
  {
  tIdent ident; tPosition position;}
  , Ident) DECLARE_LIST_TYPE (struct
  {
  int value; tPosition position;}
  , int) DECLARE_LIST_TYPE (struct
  {
  tIdent ident; PtrMP_INT value; tPosition position;}

  , Binding) DECLARE_LIST_TYPE (Ptrchar, String)
/* Declare a list reverser on IdentLists and intLists */
DECLARE_LIST_TYPE_REVERSE (Ident) DECLARE_LIST_TYPE_REVERSE (int) DECLARE_LIST_TYPE_APPEND (int) DECLARE_LIST_TYPE_CONSTRUCTOR (Ptrchar, String)
/* NewIdentList : create an IdentList element */
extern PtrIdentList NewIdentList (tIdent ident, tPosition position, PtrIdentList next);

/* NewintList : create an intList element */
extern PtrintList NewintList (int value, tPosition position, PtrintList next);

/* NewBindingList : create a BindingList (ident -> value mapping) element */
extern PtrBindingList NewBindingList (tIdent ident, PtrMP_INT value, tPosition position, PtrBindingList next);

/* StrPtrintList : write out a list of integers with the string separator between them */
extern void StrPtrintList (FILE * stream, PtrintList list, char *separator);

/* StrPtrIdentList : write out a list of identifiers with the string separator between them */
extern void StrPtrIdentList (FILE * stream, PtrIdentList list, char *separator);

/* StrPtrBindingList : write out a binding list with string separator between elements and
   string assignStr between ident and value */
extern void StrPtrBindingList (FILE * stream, PtrBindingList list, char *separator, char *assignStr);

/* LispLists are for representing the unconstrained lists as part of options lists in Breeze */
typedef enum
{
    SymbolLispNature, NumberLispNature, StringLispNature,
    BooleanLispNature, SublistLispNature
}
LispNature;

struct LispList;

typedef struct Lisp
{
    LispNature nature;
    union
    {
        tIdent symbol;
        PtrMP_INT number;
        tIdent string;
        bool boolean;
        struct LispList *sublist;
    }
    value;
}
Lisp;

DECLARE_LIST_TYPE (Lisp, Lisp) DECLARE_LIST_TYPE_CONSTRUCTOR (Lisp, Lisp) DECLARE_LIST_TYPE_REVERSE (Lisp) DECLARE_LIST_TYPE_COPY (Lisp)
/* StrPtrLispList : print a list of Lisp elements */
extern void StrPtrLispList (FILE * stream, PtrLispList list, Ptrchar separator, Ptrchar separator2);

/* NewLisp... : constructors for Lisp's and ListLists */
extern Lisp NewLispSymbol (tIdent value);
extern Lisp NewLispNumber (PtrMP_INT value);
extern Lisp NewLispString (tIdent value);
extern Lisp NewLispBoolean (bool value);
extern Lisp NewLispSublist (PtrLispList value);
extern PtrLispList NewLispList (Lisp body, PtrLispList next);

/* LispIsHeaded: returns true if the given Lisp node is a sublist
	with a symbol as its CAR that is also the same symbol as `symbol'
	(or any symbol if `symbol' is NoIdent) */
extern bool LispIsHeaded (Lisp node, tIdent symbol);

/* RemoveHeadedLispListElement: remove the first node from a PtrLispList
	with the head symbol `symbol'.  Returns true if an element was removed
	and the element body in `ret' (if non NULL).  `list' is also modified
	to remove the element */
extern bool RemoveHeadedLispListElement (PtrLispList * list, tIdent symbol, Lisp * ret);

#endif /* MISCLISTS_HEADER */
