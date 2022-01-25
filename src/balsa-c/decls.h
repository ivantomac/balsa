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

	`decls.h'
	Functions to handle declaration in
	the attribute grammar evaluator
	
 */

#ifndef DECLS_HEADER
#define DECLS_HEADER

#include "misc.h"
#include "contexts.h"
#include "values.h"
#include "misclists.h"
#include "exprs.h"
#include "commands.h"

/* TypeDeclaration : bind the name ident to the type type and add to the context contextIn returning
   the newly created context 
   Checks: validity of type, local uniqueness of ident */
extern PtrContext TypeDeclaration (PtrContext contextIn, tIdent ident, Scope scope, PtrType type, tPosition position);

/* ConstantDeclaration : bind the name ident to the value value (with its type) and add it to the
   incoming context contextIn 
   Checks: validity of type, constness of value, local uniqueness of ident,
   iff type != NULL then checks value against this type */
extern PtrContext ConstantDeclaration (PtrContext contextIn, tIdent ident,
  Scope scope, tPosition exprPosition, TypedValue value, PtrType type, tPosition position);

/* VariableDeclaration : bind each of the names in idents to the type type and add to the incoming
   context contextIn.  Each variable gets the initialised value given as an argument here (if
   not NULL).
   Checks: validity of type, local uniqueness of idents (against incoming context and against
   each other) */
extern PtrContext VariableDeclaration (PtrContext contextIn,
  PtrIdentList idents, Scope scope, PtrType type, PtrMP_INT initialisedValue, tPosition position);

/* ChannelDeclaration : similar to VariableDeclaration */
extern PtrContext ChannelDeclaration (PtrContext contextIn, PtrIdentList idents, Scope scope, PtrType type, bool isMulticast, tPosition position);

/* ChannelArrayDeclaration : declare a channel array */
extern PtrContext ChannelArrayDeclaration (PtrContext contextIn, PtrIdentList idents, Scope scope, PtrType type, Span range, bool isMulticast);

/* AddEnumElemToEnumType: add an element onto the end of the elementsTail list
   if useDefaultValue is set then bind ident to defaultValue, 
   otherwise use the expressionValue.value.  nextValue gets the value of this element 
   (either defaultValue or the expression value) + 1 
   Checks: validity of expressionValue iff useDefaultValue == true */
extern PtrType AddEnumElemToEnumType (PtrType type, tIdent ident,
  PtrMP_INT defaultValue,
  PtrBindingList elementsTail,
  PtrBindingList * nextElementsTail,
  TypedValue exprValue, tPosition exprPosition, bool useDefaultValue, bool hasOverType, PtrMP_INT * nextValue, tPosition position);

/* MakeNumericType : make a numeric type of the given signednes and size
   Checks: Numericness/Positiveness/Constness of expr */
extern PtrType MakeNumericType (TypedValue expr, tPosition exprPosition, bool signedness, tPosition position);

/* AddRecordElemToRecordType: add the list of idents as record elements in the given record type,
   offset gives the initial offset of the first list element,
   iff (hasOverType == true) the size field of the record type is fixed, any overflowing this
   size (offset + size of elemType * lengthof(idents)) results in an error,
   else size of record type grows with new elements */
extern PtrType AddRecordElemToRecordType (PtrType type, PtrIdentList idents,
  PtrType elemType, bool hasOverType, PtrInstanceList elementsTail, PtrInstanceList * nextElementsTail, tPosition position);

/* HandleSpecifiedRange : handle range determination, if left == NULL then this is not a spanning range 
   (but 0 based), if expectedType == NoType then don't check range bounding type for consistency with
   the expectedType */
extern Span HandleSpecifiedRange (bool spanning, ExprAttributes left, ExprAttributes right, PtrType expectedType, tPosition position);

/* HandleTypeRange : handle the creation of a range spanning a Numeric or Enum type,
   if expecetedType is non NULL (and non NoType) then check for subtype compliance with
   this type (ie. numeric type is narrower, enumeration type is the same) */
extern Span HandleTypeRange (PtrType type, PtrType expectedType, tPosition position);

/* HandleArrayType : construct a new array type iff boundingType, baseType != NoType */
extern PtrType HandleArrayType (PtrContext context, PtrType baseType, Span range, tPosition position);

/* ValDeclaration : create a new val */
extern PtrContext ValDeclaration (PtrContext contextIn, tIdent ident, ExprAttributes expr, tPosition position);

/* ProcedureDeclaration : declare a shared/unshared block/procedure */
extern PtrProcedure ProcedureDeclaration (PtrContext contextIn, tIdent ident,
  bool shared, PtrInstanceList ports, unsigned portCount, PtrInstanceList extraInstances, PtrContext * blockContext, tPosition position);

/* ParameterisedProcedureDeclaration : declare a procedure with parameters */
extern PtrProcedure ParameterisedProcedureDeclaration (PtrContext contextIn, tIdent ident, union Tree_Node *tree, tPosition position);

/* ParameterisedFunctionDeclaration : declare a function with parameters */
extern PtrProcedure ParameterisedFunctionDeclaration (ProcedureNature nature,
  PtrContext contextIn, tIdent ident, union Tree_Node *tree, tPosition position);

/* SpecifyParameterisedProcedure : make a specified parameterised procedure.  The generated
   procedure will have the name MakeParameterisedProcedureName (procIdent).
   tree should be a copy of the parameterised procedure tree which should be decorated
   during the specifing action. */
extern PtrProcedure SpecifyParameterisedProcedure (PtrContext contextIn,
  tIdent procIdent,
  PtrProcedure procedure,
  union Tree_Node *procedureTree, PtrInstanceList constantParams, PtrTypeList typeParams, union Tree_Node *ports, tPosition position);

/* SpecifyParameterisedBuiltinFunction : like SpecifyParameterisedProcedure but for functions.
   This will largely be the same but also involves handling builtin functions */
extern PtrProcedure SpecifyParameterisedBuiltinFunction (PtrContext contextIn,
  tIdent funcIdent,
  PtrProcedure
  function,
  union Tree_Node
  *functionTree,
  PtrInstanceList constantParams, PtrTypeList typeParams, union Tree_Node *ports, PtrComponentParameterList componentParameters, tPosition position);

/* FunctionDeclaration : declare a shared/unshared function */
extern PtrProcedure FunctionDeclaration (PtrContext contextIn, tIdent ident,
  ProcedureNature nature, PtrInstanceList args, unsigned argCount, PtrContext * blockContext, tPosition position);

/* ProcedureDeclarationFillIn : fill in procedure structure initialised by ProcedureDeclaration
   (or fail silently if procedure == NULL), perform consistency checks on block accesses to ports */
extern PtrContext ProcedureDeclarationFillIn (PtrProcedure procedure,
  PtrContext contextIn, bool shared, CommandAttributes blockAttributes, tPosition position);

/* FunctionDeclarationFillIn : fill in procedure structure initialised by FunctionDeclaration
   (or fail silently if function == NULL), perform consistency checks on expression accesses to arguments
   and place argument spliting hardware */
extern PtrContext FunctionDeclarationFillIn (PtrProcedure function,
  PtrContext contextIn, ExprAttributes exprAttributes, PtrType expectedType, tPosition position);

/* BuiltinFunctionDeclarationFillIn : fill in procedure structure initialised by FunctionDeclaration
   (or fail silently if function == NULL), perform consistency checks */
extern PtrContext BuiltinFunctionDeclarationFillIn (PtrProcedure function, PtrContext contextIn, PtrType expectedType, tPosition position);

/* PartDeclaration : declare a procedure from a part in a breeze file */
extern PtrProcedure PartDeclaration (PtrContext contextIn, tIdent ident,
  PtrInstanceList ports, unsigned portCount, PtrInstanceList extraInstances, PtrLispList attributes, tPosition position);

/* PartDeclarationFillIn : fill in procedure structure initialised by PartDeclaration
   (or fail sinlently if procedure == NULL) */
extern PtrContext PartDeclarationFillIn (PtrProcedure procedure,
  PtrContext contextIn, PtrWireArray localChannels, unsigned localChannelCount, PtrComponentList components, tPosition position);

/* PlaceWiresInPortList : place the wires firstWireNo ... into the given ports */
extern unsigned PlaceWiresInPortList (PtrInstanceList ports, PtrWireArray wires, unsigned firstWireNo, unsigned portCount);

/* MakeWiresForBuiltinFunctionPorts : make the wires (and add to the given wire list) for a builtin
	function's ports */
extern unsigned MakeWiresForBuiltinFunctionPorts (PtrInstanceList ports,
  PtrWireList * wires, unsigned firstPortNo, unsigned portCount, tPosition position);

#endif /* DECLS_HEADER */
