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

	`operators.c'
	Func component operators
	
 */

#include "operators.h"

tIdent FirstOpIdent = 0;

Ptrchar OperatorNames[] = {
    "Negate", "Invert", "Add", "Subtract", "ReverseSubtract",
    "Equals", "NotEquals", "LessThan", "GreaterThan",
    "LessOrEquals", "GreaterOrEquals",
    "And", "Or", "Xor",
    "Log",
    "Multiply", "Divide", "Modulo",
    "Power",
    "INVALID"
};

Ptrchar OperatorSymbols[] = {
    "-", "not", "+", "-", "r-",
    "=", "/=", "<", ">", "<=", ">=",
    "and", "or", "xor",
    "log",
    "*", "/", "%",
    "^",
    "INVALID"
};

void BeginOperators (void)
{
    Operators op = NegateOperator;

    while (op <= InvalidOperator)
    {
        tIdent newIdent = MakeIdent1 (OperatorNames[op]);

        if (FirstOpIdent == 0)
            FirstOpIdent = newIdent;

        op++;
    }
}

#define OPERATOR_IDENT(op) ((FirstOpIdent) + (op))

/* MirrorOperators: AopB -> BopA, same operator for commutative operators, RevSub for Sub,
	mirrored inequality for comparisons */
Operators MirrorOperators[] = {
    NegateOperator, InvertOperator,
    AddOperator, ReverseSubtractOperator, SubtractOperator,
    EqualOperator, NotEqualOperator,
    GEOperator, LEOperator, GTOperator, LTOperator,
    AndOperator, OrOperator, XorOperator,
    InvalidOperator /* Array  append */ ,
    InvalidOperator /* Log */ ,
    MultiplyOperator, InvalidOperator /* Div */ , InvalidOperator /* Mod */ ,
    InvalidOperator /* Power */ ,
    InvalidOperator             /* Invalid operator ;) */
};

/* Find{Bin,Un}aryOperator: returns the operator enum value corresponding to the
	operator name in op or InvalidOperator if unsuccessful.  If `onlySynthesisable'
	is true then reject the names of operators which shouldn't appear in a part */
Operators FindBinaryOperator (tIdent op, bool onlySynthesisable)
{
    return ((op >= OPERATOR_IDENT (AddOperator) && op <= OPERATOR_IDENT (XorOperator)) /* synthesisable */
      || (!onlySynthesisable && op >= OPERATOR_IDENT (MultiplyOperator)
        && op <= OPERATOR_IDENT (PowerOperator)) ? op - FirstOpIdent : InvalidOperator);
}

Operators FindUnaryOperator (tIdent op, bool onlySynthesisable)
{
    return ((op >= OPERATOR_IDENT (NegateOperator) && op <= OPERATOR_IDENT (InvertOperator)) /* synthesisable */
      || (!onlySynthesisable && op == OPERATOR_IDENT (LogOperator)) ? op - FirstOpIdent : InvalidOperator);
}

/* StrOperators : print an operator name on the given stream */
void StrOperators (FILE * stream, Operators op)
{
    fprintf (stream, "\"%s\"", OperatorNames[(int) op]);
}
