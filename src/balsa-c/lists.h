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

	`lists.h'
	Generic list manipulating macros/functions
	
 */

#ifndef LISTS_HEADER
#define LISTS_HEADER

#include "misc.h"

/* --- Common cons cell type (for lists which have a body parameter which is just a pointer) */

/* A ConsList is a CAR, CDR cell with pointers for both elements.  PtrConsList objects are
   pointers to lists of ConsList cells.  PtrConsArray objects are pointers to the first
   element to ConsList's stored as arrays */

typedef struct ConsList
{
    void *body;
    struct ConsList *next;
}
ConsList, *PtrConsList, *PtrConsArray;

/* CONS_ARRAY_TO_LIST, CONS_LIST_TO_ARRAY : convert to and from list and array types */
#define CONS_ARRAY_TO_LIST(array,size) ((PtrConsList)((array)+((size)-1)))
#define CONS_LIST_TO_ARRAY(list,size) ((PtrConsArray)((list)-((size)-1)))

/* --- List manipulation primitives for non Cons cell lists declarations */

/* DECLARE_LIST_TYPE : for a type foo creates types fooList and PtrfooList which are a structure
   and a pointer to that structure */
#define DECLARE_LIST_TYPE(type,name) \
typedef struct name##List { \
	type body; struct name##List *next; \
} name##List, *Ptr##name##List;

/* DECLARE_LIST_TYPE_CONSTRUCTOR : define a constructor to the declared list type, takes same
   arguments as DECLARE_LIST_TYPE, produces a function New##name##List with two arguments
   of type type and Ptr##name##List returning a Ptr##name##List, this creates a cons cell
   for type type and places the given arguments in the body and the next link fields respectively */
#define DECLARE_LIST_TYPE_CONSTRUCTOR(type,name) \
extern Ptr##name##List New##name##List (type body, Ptr##name##List nextCell);

/* DECLARE_LIST_TYPE_APPEND : define a function Append##name##Lists which concatenates two lists
   of type Ptr##name##List by changing the next field in the last link of the first list */
#define DECLARE_LIST_TYPE_APPEND(name) \
extern Ptr##name##List Append##name##Lists (Ptr##name##List list1, Ptr##name##List list2);

/* DECLARE_LIST_TYPE_COPY : define a function Copy##name##List which copies the elements of the
   given list into a new list */
#define DECLARE_LIST_TYPE_COPY(name) \
extern Ptr##name##List Copy##name##List (Ptr##name##List list);

/* DECLARE_LIST_TYPE_LENGTHOF : define a function LengthOf##name##List which returns the length of the given list */
#define DECLARE_LIST_TYPE_LENGTHOF(name) \
extern unsigned LengthOf##name##List (Ptr##name##List list);

/* DECLARE_LIST_TYPE_REVERSE : define a function to reverse lists of type name##List */
#define DECLARE_LIST_TYPE_REVERSE(name) \
extern Ptr##name##List Reverse##name##List (Ptr##name##List list);

/* DECLARE_LIST_TYPE_NTHCELLOF : define a function to return the Nth list element's cons cell,
   if elem is negative then return element -elem'th to last element (-1 gives the last element) */
#define DECLARE_LIST_TYPE_NTHCELLOF(name) \
extern Ptr##name##List NthCellOf##name##List (Ptr##name##List list, int elem);

/* DECLARE_LIST_TYPE_NTHOF : define a function to return the Nth list element,
   this function requires NthCellOf... */
#define DECLARE_LIST_TYPE_NTHOF(name) \
extern Ptr##name NthOf##name##List (Ptr##name##List list, int elem);

/* --- List manipulation primitives, definitions */

/* DEFINE_LIST_TYPE_CONSTRUCTOR */
#define DEFINE_LIST_TYPE_CONSTRUCTOR(type,name) \
Ptr##name##List New##name##List (type body, Ptr##name##List nextCell) \
{ \
	Ptr##name##List ret = NEW (name##List); \
	CAR(ret) = body; \
	ret->next = nextCell; \
	return ret; \
}

/* DEFINE_LIST_TYPE_APPEND */
#define DEFINE_LIST_TYPE_APPEND(name) \
Ptr##name##List Append##name##Lists (Ptr##name##List list1, Ptr##name##List list2) \
{ \
	Ptr##name##List list = list1; \
	FOR_EACH (list, true, if (!CDR(list)) { list->next = list2; return list1; } ) \
	return list2; \
}

/* DEFINE_LIST_TYPE_COPY */
#define DEFINE_LIST_TYPE_COPY(name) \
Ptr##name##List Copy##name##List (Ptr##name##List list) \
{ if (list) return New##name##List (CAR(list), Copy##name##List (CDR(list))); return NULL; }

/* DEFINE_LIST_TYPE_LENGTHOF */
#define DEFINE_LIST_TYPE_LENGTHOF(name) \
unsigned LengthOf##name##List (Ptr##name##List list) \
{ \
	unsigned length = 0; \
	FOR_EACH (list, true, length ++; ) \
	return length; \
}

/* DEFINE_LIST_TYPE_REVERSE */
#define DEFINE_LIST_TYPE_REVERSE(name) \
Ptr##name##List ReverseTail##name##List (Ptr##name##List stub, Ptr##name##List list) \
{ \
	Ptr##name##List next = CDR(list); \
	return (list ? ReverseTail##name##List (CONS (list /* head */, stub), next) : stub); \
} \
Ptr##name##List Reverse##name##List (Ptr##name##List list) \
{ return ReverseTail##name##List (NULL, list); }

/* DEFINE_LIST_TYPE_NTHOF */
#define DEFINE_LIST_TYPE_NTHOF(name) \
Ptr##name NthOf##name##List (Ptr##name##List list, int elem) \
{ \
	Ptr##name##List ret = NthCellOf##name##List (list, elem); \
	return (ret ? CAR (ret) : NULL); \
}

/* DEFINE_LIST_TYPE_NTHCELLOF */
#define DEFINE_LIST_TYPE_NTHCELLOF(name) \
Ptr##name##List NthCellOf##name##List (Ptr##name##List list, int elem) \
{ \
	unsigned elemNo = (elem < 0 ? LengthOf##name##List (list) + elem : elem); \
	FOR_EACH (list, elemNo, elemNo--; ) \
	return list; \
}

/* --- Cons cell mapulation / list traversal primitives */

/* CONS : add a head element onto the front of list tail and return the new list 
   iff head == NULL then just return tail */
#define CONS(head,tail) ((head) ? ((head)->next = (tail), (head)) : (tail))

/* CAR : head of list, NB list must not be NULL */
#define CAR(list) ((list)->body)
/* CDR : tail of list, returns NULL on NULL list */
#define CDR(list) ((list) ? (list)->next : NULL)

/* CA*D*R : combinations of CAR and CDR */
#define CADR(list) (CAR(CDR(list)))
#define CDDR(list) (CDR(CDR(list)))
#define CADDR(list) (CAR(CDR(CDR(list))))
#define CDDDR(list) (CDR(CDR(CDR(list))))

/* FOR_EACH : iterate across the given list, terminating at the end of the list of when
   pred ceases to be true */
#define FOR_EACH(list,pred,body) while ((list) && (pred)) { body; (list) = CDR(list); }

/* --- Locally defined functions, cons cell primitives as functions */

/* PrepareConsArray : make an array (length len) of correctly (reverse 0 <- 1 <- n-1) linked
   PtrConsList cons cells with the body element set to NULL */
extern PtrConsArray PrepareConsArray (unsigned len);

/* ConsListTail : returns the last element of the given list, or NULL for an empty list */
extern PtrConsList ConsListTail (PtrConsList list);

/* AppendConsLists : append the second cons list to the first */
extern PtrConsList AppendConsLists (PtrConsList list1, PtrConsList list2);

/* NewConsList : create a new cons cell initialised with the given car and cdr */
extern PtrConsList NewConsList (void *car, PtrConsList cdr);

/* RawArrayCopyConsList : copy a cons cell list into an array, given the
   correct length of the list */
extern PtrConsArray RawArrayCopyConsList (PtrConsList list, unsigned listLength);

/* ArrayCopyConsList : copy a list of cons cells into an array */
extern PtrConsArray ArrayCopyConsList (PtrConsList list);

/* CopyConsList : copy a list of cons cells into a new list */
extern PtrConsList CopyConsList (PtrConsList list);

/* LengthOfConsList : find the length of a cons cell list */
extern unsigned LengthOfConsList (PtrConsList list);

/* RawArrayReverseConsList : reverse a given list, create an array, copy elements in reverse
   order and return head */
extern PtrConsArray RawArrayReverseConsList (PtrConsList list, unsigned listLength);

/* ArrayReverseConsList : reverse a given list, create an array, copy elements in reverse
   order and return head */
extern PtrConsArray ArrayReverseConsList (PtrConsList list);

/* ReverseConsList : reverse a given list, create an array, copy elements in reverse
   order and return head */
extern PtrConsArray ReverseConsList (PtrConsList list);

/* NthCellOfConsList : return the nth cell of a given list, if the
   index is negative then work from the back of the list */
extern PtrConsList NthCellOfConsList (PtrConsList list, int elem);

/* NthOfConsList : return the nth cells car element, indexing as NthCellOfConsList */
extern void *NthOfConsList (PtrConsList list, int elem);

/* CopyConsCellFunctionType : function prototype for cons cell to cons cell functions */
typedef PtrConsList (*CopyConsCellFunctionType) (PtrConsList);

/* RawArrayDeepCopyConsList : copy a cons cell list by copying each element, need to know
   the function required in the copy and the length of the list */
extern PtrConsArray RawArrayDeepCopyConsList (PtrConsList list, CopyConsCellFunctionType copyFunction, unsigned listLength);

/* ArrayDeepCopyConsList : copy a cons list into an array using the copyFunction to copy each element */
extern PtrConsArray ArrayDeepCopyConsList (PtrConsList list, CopyConsCellFunctionType copyFunction);

/* DeepCopyConsList : copy a cons list into a new list using the copyFunction to copy each element */
extern PtrConsList DeepCopyConsList (PtrConsList list, CopyConsCellFunctionType copyFunction);

/* --- Macros for cons cell type list function declarations */

/* DECLARE_CONS_LIST_TYPE : declare a type and functions for a cons list type */
#define DECLARE_CONS_LIST_TYPE(name) \
typedef struct name##List { \
	Ptr##name body; struct name##List *next; \
} name##List, *Ptr##name##List, *Ptr##name##Array; \
extern Ptr##name##List (*New##name##List)(Ptr##name,Ptr##name##List); \
extern Ptr##name##List (*Append##name##Lists)(Ptr##name##List,Ptr##name##List); \
extern Ptr##name##List (*Copy##name##List)(Ptr##name##List); \
extern Ptr##name##List (*ArrayCopy##name##List)(Ptr##name##List); \
extern unsigned (*LengthOf##name##List)(Ptr##name##List); \
extern Ptr##name##List (*Reverse##name##List)(Ptr##name##List); \
extern Ptr##name (*NthOf##name##List)(Ptr##name##List,int); \
extern Ptr##name##List (*NthCellOf##name##List)(Ptr##name##List,int);

/* DECLARE_CONS_LIST_DEEP_COPY : declare a function to deep copy the given list */
#define DECLARE_CONS_LIST_DEEP_COPY(name) \
extern Ptr##name##List DeepCopy##name##List (Ptr##name##List list);

/* DECLARE_CONS_LIST_ARRAY_DEEP_COPY : declare a function to deep copy the given list into an array */
#define DECLARE_CONS_LIST_ARRAY_DEEP_COPY(name) \
extern Ptr##name##Array ArrayDeepCopy##name##List (Ptr##name##List list);

/* --- Macros for cons cell type list function definitions */

/* DEFINE_CONS_LIST_TYPE : define the type and functions for a cons list type */
#define DEFINE_CONS_LIST_TYPE(name) \
Ptr##name##List (*New##name##List)(Ptr##name,Ptr##name##List) \
  = (Ptr##name##List (*)(Ptr##name,Ptr##name##List)) NewConsList; \
Ptr##name##List (*Append##name##Lists)(Ptr##name##List,Ptr##name##List) \
  = (Ptr##name##List (*)(Ptr##name##List,Ptr##name##List)) AppendConsLists; \
Ptr##name##List (*Copy##name##List)(Ptr##name##List) \
  = (Ptr##name##List (*)(Ptr##name##List)) CopyConsList; \
Ptr##name##List (*ArrayCopy##name##List)(Ptr##name##List) \
  = (Ptr##name##List (*)(Ptr##name##List)) ArrayCopyConsList; \
unsigned (*LengthOf##name##List)(Ptr##name##List) \
  = (unsigned (*)(Ptr##name##List)) LengthOfConsList; \
Ptr##name##List (*Reverse##name##List)(Ptr##name##List) \
  = (Ptr##name##List (*)(Ptr##name##List)) ReverseConsList; \
Ptr##name (*NthOf##name##List)(Ptr##name##List,int) \
  = (Ptr##name (*)(Ptr##name##List,int)) NthOfConsList; \
Ptr##name##List (*NthCellOf##name##List)(Ptr##name##List,int) \
  = (Ptr##name##List (*)(Ptr##name##List,int)) NthCellOfConsList;

/* DEFINE_CONS_LIST_DEEP_COPY */
#define DEFINE_CONS_LIST_DEEP_COPY(name) \
Ptr##name##List DeepCopy##name##List (Ptr##name##List list) \
{ return (Ptr##name##List) DeepCopyConsList ((PtrConsList) list, (CopyConsCellFunctionType) Copy##name); }

/* DEFINE_CONS_LIST_ARRAY_DEEP_COPY */
#define DEFINE_CONS_LIST_ARRAY_DEEP_COPY(name) \
Ptr##name##Array ArrayDeepCopy##name##List (Ptr##name##List list) \
{ return (Ptr##name##Array) ArrayDeepCopyConsList ((PtrConsList) list, (CopyConsCellFunctionType) Copy##name); }

#endif /* LISTS_HEADER */
