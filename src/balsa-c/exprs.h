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

	`exprs.h'
	Expression handling functions
	
 */

#ifndef EXPRS_HEADER
#define EXPRS_HEADER

#include <stdio.h>
#include "misclists.h"
#include "accesses.h"
#include "components.h"
#include "values.h"

struct Context;                 /* forward declare contexts. */
struct Procedure;

/* ExprAttributes : data elements connected to expressions during compilation */
typedef struct ExprAttributes
{
    PtrComponentList components;
    PtrAccessList accesses;
    PtrWireList wires;
    PtrWire activation;         /* result wire */
    TypedValue value;
}
ExprAttributes, *PtrExprAttributes;

/* Declare list of expr attrs. and list of list of ... */
DECLARE_LIST_TYPE (ExprAttributes, ExprAttributes)
DECLARE_LIST_TYPE_CONSTRUCTOR (ExprAttributes, ExprAttributes)
DECLARE_LIST_TYPE_LENGTHOF (ExprAttributes) DECLARE_LIST_TYPE_APPEND (ExprAttributes) DECLARE_LIST_TYPE_REVERSE (ExprAttributes)
/* NoExprAttributes : Default value, all NULL and No### */
extern ExprAttributes NoExprAttributes;

/* Guard outcomes, as returned by DetermineGuardNature */
typedef enum GuardNature
{
    FalseGuardNature, TrueGuardNature, NonConstantGuardNature
}
GuardNature;

/* Initialise */
extern void BeginExprs (void);

/* DetermineGuardNature : is the given expression a good guard value, is it a
	constant 1 or 0, or does it not have a constant value */
extern GuardNature DetermineGuardNature (ExprAttributes guard, bool checkType, bool mustBeConst, tPosition position);

/* CheckAndSizeOfType : wierd function to check a type for NoType'ness and return 0, else
   returns the SignedBits size of the type type */
extern SignedBits CheckAndSizeOfType (PtrType type, tPosition position);

/* ExistingTypeLookup : lookup a type and complain if it doesn't exist.
   Returns NoType if type did not exist in the given context */
extern PtrType ExistingTypeLookup (struct Context *context, tIdent ident, tPosition position);

/* HandleLiteralExpr : create constant component for literals, handles implicit numeric cast too */
extern ExprAttributes HandleLiteralExpr (PtrMP_INT literal, PtrType expectedType, tPosition position);

/* HandleStringExpr : create constant string component for string literals */
extern ExprAttributes HandleStringExpr (Ptrconstchar string, tPosition position);

/* HandleIdentExpr : resolve an identifier from either instances or
   enumeration element values (iff expectedType has nature EnumerationType,
   returns type and value of identifier.
   If `allowChannels' is true then return channel accesses for channel matching idents.
   Returned value attribute has value NULL and type NoType on error */
extern ExprAttributes HandleIdentExpr (struct Context *context, tIdent ident, PtrType expectedType, bool allowChannels, tPosition position);

/* HandleImplicantExpr : create an implicant, allowing for promotion as with literals */
extern ExprAttributes HandleImplicantExpr (Implicant implicant, PtrType expectedType, tPosition position);

/* HandleDontCareExpr : create a dont care, similar to implicant generation but consists entirely of dontCare bits */
extern ExprAttributes HandleDontCareExpr (PtrType expectedType, tPosition position);

/* GetAggregateConsExprElements : lookup elements of type ident, if this is NoIdent then use
   the expectedType unless it is not a record or array type. Complain if the typing is wrong
   and return NULL, actualType gets the real type of the expression */
extern PtrInstanceList GetAggregateConsExprElements (struct Context
  *context, tIdent ident, PtrType expectedType, PtrType * actualType, tPosition position);

/* HandleAggregateConsExpr : reduce an expression list into a record/array type constant or a wire connected to a
   Combine component.  If appendNcons is true then the expressions are treated as array fragments rather than
   record/array elements. */
extern ExprAttributes HandleAggregateConsExpr (PtrType actualType,
  bool checkArraySize, bool appendNcons, PtrExprAttributesList exprs, tPosition position);

/* HandleNamedEnumElemExpr : lookup enumeration element enumName in type typeName, raises
   errors on any problems, return NULL,NoType on error */
extern ExprAttributes HandleNamedEnumElemExpr (struct Context *context, tIdent typeName, tIdent enumName, tPosition position);

/* HandleRecordElemExtractExpr : extract the named element value from the expression expr
   if exprValue has no type then moan and return NULL, NoType in value */
extern ExprAttributes HandleRecordElemExtractExpr (struct Context *context, tIdent elemName, ExprAttributes expr, tPosition position);

/* HandleArrayExtractExpr : handle extraction of an element from a array variable access */
extern ExprAttributes HandleArrayExtractExpr (struct Context *context, ExprAttributes array, ExprAttributes subscript, tPosition position);

/* HandleArraySliceExpr : handle extraction of a constant slice from a array variable access */
extern ExprAttributes HandleArraySliceExpr (struct Context *context, ExprAttributes array, Span range, bool singleElement, tPosition position);

/* HandleAsExpr : handle explicit casts, if expr has a value (constant) then generate a new
   constant components otherwise place the appropriate adapter */
extern ExprAttributes HandleAsExpr (PtrType asType, ExprAttributes expr, tPosition position);

/* HandleBitArrayCastExpr : handle to-bit-array casts, if expr has a value (constant) which is
   signed then also truncate this value to the type width */
extern ExprAttributes HandleBitArrayCastExpr (ExprAttributes expr, tPosition position);

/* Handle{Un,Bin}aryExpr : handle the unary and binary operators, doing constant folding and
   Func component placement */
extern ExprAttributes HandleUnaryExpr (struct Context *context, Operators operation, ExprAttributes expr, tPosition position);
extern ExprAttributes HandleBinaryExpr (struct Context *context, Operators operation, ExprAttributes left, ExprAttributes right, tPosition position);

/* HandleFunctionCallExpr : handle the placement of components/parts for a function call */
extern ExprAttributes HandleFunctionCallExpr (PtrExprAttributesList arguments, struct Procedure *function, tPosition position);

/* DeepCopyExprAttributes : use DeepCopyCommandAttributes to copy attributes of an expression */
extern ExprAttributes DeepCopyExprAttributes (ExprAttributes attr);

#endif /* EXPRS_HEADER */
