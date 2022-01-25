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

	`operators.h'
	Func component operators
	
 */

#ifndef OPERATORS_HEADER
#define OPERATORS_HEADER

#include "misc.h"

/* Operators : Func operations, synthesisable and non-synthesisable */
typedef enum Operators
{
    /* Synthesisable operators */
    NegateOperator, InvertOperator, /* Unary */
    AddOperator, SubtractOperator, ReverseSubtractOperator, /* Additive */
    EqualOperator, NotEqualOperator, /* Equality */
    LTOperator, GTOperator, LEOperator, GEOperator, /* Ordering/Inequalities */
    AndOperator, OrOperator, XorOperator, /* Logical */
    /* Internal compiler operators */
    LogOperator,                /* Log to base 2 */
    MultiplyOperator, DivideOperator, ModuloOperator, /* Multiplicative */
    PowerOperator,              /* Binary expontential */
    InvalidOperator             /* Error! */
}
Operators;

/* Names of the operators in breeze/DEBUG */
extern Ptrchar OperatorNames[];

/* balsa symbols for operators */
extern Ptrchar OperatorSymbols[];

/* MirrorOperators: AopB -> BopA, same operator for commutative operators, RevSub for Sub,
	mirrored inequality for comparisons */
extern Operators MirrorOperators[];

/* Find{Bin,Un}aryOperator: returns the operator enum value corresponding to the
	operator name in op or InvalidOperator if unsuccessful.  If `onlySynthesisable'
	is true then reject the names of operators which shouldn't appear in a part */
extern Operators FindBinaryOperator (tIdent op, bool onlySynthesisable);
extern Operators FindUnaryOperator (tIdent op, bool onlySynthesisable);

/* StrOperators : print an operator name on the given stream */
extern void StrOperators (FILE * stream, Operators op);

extern void BeginOperators (void);

#endif /* OPERATORS_HEADER */
