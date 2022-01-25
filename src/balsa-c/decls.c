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

	`decls.c'
	Functions to handle declaration in
	the attribute grammar evaluator
	
 */

#include <limits.h>

#include "decls.h"
#include "Errors.h"
#include "output.h"
#include "optimise.h"
#include "flags.h"
#include "parts.h"
#include "ports.h"
#include "Tree.h"

#include "callcontexts.h"

/* TypeDeclaration : bind the name ident to the type type and add to the context contextIn returning
   the newly created context 
   Checks: validity of type, local uniqueness of ident */
PtrContext TypeDeclaration (PtrContext contextIn, tIdent ident, Scope scope, PtrType type, tPosition position)
{
    if (type == NoType) /* Sorted out further up tree */ ;
    else if (LookupType (contextIn, ident, true) != NoType)
        LOG_ERROR (NonUniqueLocalName, ident, position);
    else
    {
        /* Link the incoming type onto the type list of the outgoing context */
        type->ident = ident;
        type->scope = scope;
        return NewContext (contextIn->depth, NewTypeList (type, contextIn->types), contextIn->instances, contextIn->procedures);
    }
    return contextIn;
}

/* ConstantDeclaration : bind the name ident to the value value (with its type) and add it to the
   incoming context contextIn 
   Checks: validity of type, constness of value, local uniqueness of ident,
   iff type != NULL then checks value against this type */
PtrContext
ConstantDeclaration (PtrContext contextIn, tIdent ident, Scope scope, tPosition exprPosition, TypedValue value, PtrType type, tPosition position)
{
    if (!TypedValueIsConstant (value, position))
    {
    } else if (LookupInstance (contextIn, ident, true))
        LOG_ERROR (NonUniqueLocalName, ident, position);
    else if (type && !TypeEquivalence (type, value.type))
        LOG_ERROR (ConstantHasWrongType, NoIdent, position);
    else
    {
        PtrInstance newConstant = NewConstantInstance (ident, value);

        newConstant->scope = scope;

        return NewContext (contextIn->depth, contextIn->types, NewInstanceList (newConstant, contextIn->instances), contextIn->procedures);
    }
    return contextIn;
}

/* VariableDeclaration : bind each of the names in idents to the type type and add to the incoming
   context contextIn.  Each variable gets the initialised value given as an argument here (if
   not NULL).
   Checks: validity of type, local uniqueness of idents (against incoming context and against
   each other) */
PtrContext VariableDeclaration (PtrContext contextIn, PtrIdentList idents, Scope scope, PtrType type, PtrMP_INT initialisedValue, tPosition position)
{
    PtrContext ret = contextIn;

    if (type == NoType)
    {
        /* reported further up tree */
    } else if (contextIn->depth == TopLevelContextDepth)
        LOG_ERROR (CannotDeclareThatHere, NoIdent, position);
    else
    {
        ret = NewContext (contextIn->depth, contextIn->types, contextIn->instances, contextIn->procedures);

        while (idents)
        {
            if (LookupInstance (ret, CAR (idents).ident, true))
                LOG_ERROR (NonUniqueLocalName, (CAR (idents).ident), CAR (idents).position);
            else
            {
                PtrInstance newInstance = NewVariableInstance (CAR (idents).ident, type);

                newInstance->info.variable.position = position;
                newInstance->info.variable.initialisedValue = initialisedValue;

                newInstance->scope = scope;
                ret->instances = NewInstanceList (newInstance, ret->instances);
            }
            idents = CDR (idents);
        }
    }
    return ret;
}

/* ChannelDeclaration : similar to VariableDeclaration */
PtrContext ChannelDeclaration (PtrContext contextIn, PtrIdentList idents, Scope scope, PtrType type, bool canMulticast, tPosition position)
{
    PtrContext newContext = contextIn;

    if (type == NoType) /* reported further up tree */ ;
    else if (contextIn->depth == TopLevelContextDepth)
        LOG_ERROR (CannotDeclareThatHere, NoIdent, position);
    else
    {
        newContext = NewContext (contextIn->depth, contextIn->types, contextIn->instances, contextIn->procedures);

        while (idents)
        {
            if (LookupInstance (newContext, CAR (idents).ident, true))
                LOG_ERROR (NonUniqueLocalName, (CAR (idents).ident), CAR (idents).position);
            else
            {
                PtrInstance newInstance = NewChannelInstance (CAR (idents).ident, type);

                newInstance->scope = scope;
                newInstance->info.channel.canMulticast = (WarnMulticast ? canMulticast : true);

                newContext->instances = NewInstanceList (newInstance, newContext->instances);
            }
            idents = CDR (idents);
        }
    }
    return newContext;
}

/* ChannelArrayDeclaration : declare a channel array */
PtrContext ChannelArrayDeclaration (PtrContext contextIn, PtrIdentList idents, Scope scope, PtrType type, Span range, bool canMulticast)
{
    if (type == NoType) /* reported further up tree */ ;
    else if (range.boundingType == NoType); /* Report further up ... */
    else
    {
        PtrContext contextOut = NewContext (contextIn->depth, contextIn->types,
          contextIn->instances,
          contextIn->procedures);

        while (idents)
        {
            /* Create a new type for each instance */
            if (LookupInstance (contextOut, CAR (idents).ident, true))
                LOG_ERROR (NonUniqueLocalName, (CAR (idents).ident), CAR (idents).position);
            else
            {
                PtrInstance newArrayInst = NewChannelInstance (CAR (idents).ident, type);
                PtrType newArrayType;

                newArrayInst->info.channel.canMulticast = (WarnMulticast ? canMulticast : true);

                newArrayType = NewArrayedType (type, range, newArrayInst);
                newArrayInst->type = newArrayType;

                contextOut->instances = AppendInstanceLists
                  (newArrayType->info.arrayed.arrayedElements, NewInstanceList (newArrayInst, contextOut->instances));
            }
            idents = CDR (idents);
        }
        return contextOut;
    }
    return contextIn;
}

/* AddEnumElemToEnumType: add an element onto the end of the elementsTail list
   if useDefaultValue is set then bind ident to defaultValue, 
   otherwise use the expressionValue.value.  nextValue gets the value of this element 
   (either defaultValue or the expression value) + 1 
   Checks: validity of expressionValue iff useDefaultValue == true */
PtrType
AddEnumElemToEnumType (PtrType type, tIdent ident, PtrMP_INT defaultValue,
  PtrBindingList elementsTail,
  PtrBindingList * nextElementsTail,
  TypedValue exprValue, tPosition exprPosition, bool useDefaultValue, bool hasOverType, PtrMP_INT * nextValue, tPosition position)
{
    PtrBindingList newLink = NewBindingList (ident, defaultValue, position, NULL);
    bool dontInsertNewLink = false;

    *nextElementsTail = elementsTail;

    if (!useDefaultValue)
    {
        dontInsertNewLink = true;
        if (!TypedValueIsDCFreeConstant (exprValue, position))
            return NoType;
        else if (exprValue.type != type && exprValue.type->nature != NumericType)
        {
            LOG_ERROR (ExpressionMustBeNumeric, NoIdent, exprPosition);
            return NoType;
        } else
        {
            CAR (newLink).value = exprValue.value.baseValue;
            dontInsertNewLink = false;
        }
    }
    if (LookupEnumElemValue (type, ident))
    {
        LOG_ERROR (NonUniqueLocalName, ident, position);
        dontInsertNewLink = true;
    }
    if (!dontInsertNewLink)
    {
        SignedBits newElemSize = SmallestRangeToHoldValue (CAR (newLink).value);

        if (hasOverType && !RangeIsWiderOrEqual (type->size, newElemSize))
            LOG_ERROR (EnumElemExceedsOverType, ident, position);
        else
        {
            type->size = WidestRange (type->size, newElemSize);

            if (!elementsTail)
            {
                type->info.enumeration.enumBindings = newLink;
                *nextElementsTail = newLink;
            } else
                *nextElementsTail = CDR (CONS (elementsTail, newLink));
        }
    }
    *nextValue = IncPtrMP_INT (dontInsertNewLink ? defaultValue : CAR (newLink).value);
    return type;
}

/* MakeNumericType : make a numeric type of the given signednes and size
   Checks: Numericness/Positiveness/Constness of expr */
PtrType MakeNumericType (TypedValue expr, tPosition exprPosition, bool signedness, tPosition position)
{
    if (!TypedValueIsDCFreeConstant (expr, position))
        return NoType;

    if (expr.type->nature != NumericType)
        LOG_ERROR (ExpressionMustBeNumeric, NoIdent, exprPosition);
    else if (mpz_sgn (expr.value.baseValue) <= 0 /* 0 or Negative */  ||
      mpz_cmp_ui (expr.value.baseValue, INT_MAX) > 0)
        LOG_ERROR (TypeWidthOutOfRange, NoIdent, exprPosition);
    else
    {
        unsigned typeWidth = mpz_get_ui (expr.value.baseValue);

        return NewNumericType (MarkerIdent, (signedness ? -typeWidth : typeWidth));
    }
    return NoType;
}

/* AddRecordElemToRecordType: add the list of idents as record elements in the given record type,
   offset gives the initial offset of the first list element,
   iff (hasOverType == true) the size field of the record type is fixed, any overflowing this
   size (offset + size of elemType * lengthof(idents)) results in an error,
   else size of record type grows with new elements */
PtrType
AddRecordElemToRecordType (PtrType type, PtrIdentList idents,
  PtrType elemType, bool hasOverType, PtrInstanceList elementsTail, PtrInstanceList * nextElementsTail, tPosition position)
{
    *nextElementsTail = elementsTail;

    while (idents)
    {
        if (LookupInstanceInInstanceList (type->info.record.recordElements, CAR (idents).ident, true))
        {
            LOG_ERROR (NonUniqueLocalName, (CAR (idents).ident), position);
            break;
        } else
        {
            PtrInstanceList newInstance = NewInstanceList (NewRecordElementInstance (CAR (idents).ident, elemType,
                type->info.record.unpaddedLength), NULL);

            /* Calculate offset, raise errors on overflowing type */
            type->info.record.unpaddedLength += Abs (elemType->size);

            if (hasOverType)
            {
                if (type->info.record.unpaddedLength > type->size)
                {
                    LOG_ERROR (RecordElemExceedsOverType, (CAR (idents).ident), CAR (idents).position);
                    type->size = type->info.record.unpaddedLength;
                }
                /* else: Has over type but doesn't exceed size */
            } else
                type->size = type->info.record.unpaddedLength;

            if (!*nextElementsTail)
            {
                type->info.record.recordElements = newInstance;
                *nextElementsTail = newInstance;
            } else
                *nextElementsTail = CDR (CONS (*nextElementsTail, newInstance));
        }
        idents = CDR (idents);
    }
    if (elemType->hasBuiltinElements)
        type->hasBuiltinElements = true;

    return type;
}

/* HandleSpecifiedRange : handle range determination, if left == NULL then this is not a spanning range 
   (but 0 based), if expectedType == NoType then don't check range bounding type for consistency with
   the expectedType */
Span HandleSpecifiedRange (bool spanning, ExprAttributes left, ExprAttributes right, PtrType expectedType, tPosition position)
{
    Span ret = NoSpan;

    if (!TypedValueIsDCFreeConstant (right.value, position))
        return NoSpan;

    if (right.value.type->nature == ArrayType)
        LOG_ERROR (CannotUseArrayHere, NoIdent, position);
    else if (spanning)          /* Spanning range */
    {
        if (!TypedValueIsDCFreeConstant (left.value, position))
            return NoSpan;

        if (left.value.type->nature == ArrayType)
            LOG_ERROR (CannotUseArrayHere, NoIdent, position);
        else if (left.value.type->nature != right.value.type->nature)
            LOG_ERROR (IncompatibleTypeNatures, NoIdent, position);
        /* Can only do ranges over Numeric and Enumerated types */
        else if (left.value.type->nature != NumericType && left.value.type->nature != EnumerationType)
            LOG_ERROR (InvalidRangeBoundType, NoIdent, position);
        /* Must be same enumerated type */
        else if (left.value.type->nature == EnumerationType
          && left.value.type != right.value.type && (expectedType == NoType ? true : TypeEquivalence (left.value.type, expectedType)))
            LOG_ERROR (ExpectingType, (left.value.type->ident), position);
        else
        {
            /* Swap left/right if needed */
            if (mpz_cmp (left.value.value.baseValue, right.value.value.baseValue) < 0)
            {
                ret.upperBound = right.value.value.baseValue;
                ret.lowerBound = left.value.value.baseValue;
            } else
            {
                ret.lowerBound = right.value.value.baseValue;
                ret.upperBound = left.value.value.baseValue;
            }
            if (left.value.type->nature == EnumerationType)
                ret.boundingType = left.value.type;
            /* Numeric - calculate range type */
            else
                ret.boundingType =
                  NewNumericType (MarkerIdent, WidestRange (SmallestRangeToHoldValue (ret.lowerBound), SmallestRangeToHoldValue (ret.upperBound)));
        }
    } else
        /* 0 based range */
    {
        ret.lowerBound = NewMP_INT (0);
        ret.upperBound = right.value.value.baseValue;
        if (right.value.type->nature != NumericType)
            LOG_ERROR (InvalidRangeBoundType, NoIdent, position);
        else if (mpz_sgn (ret.upperBound) == 0)
            LOG_ERROR (ArrayHasZeroSize, NoIdent, position);
        else
        {
            /* Adjust upperbound to compensate for zero element */
            PtrMP_INT newUpper = NewMP_INT (0);

            mpz_sub_ui (newUpper, ret.upperBound, 1);
            ret.upperBound = newUpper;
            ret.boundingType = NewNumericType (MarkerIdent, SmallestRangeToHoldValue (ret.upperBound));
        }
    }

    if (expectedType != NoType)
    {
        if (left.value.type->nature != expectedType->nature)
        {
            LOG_ERROR (TypeIsNotValid, NoIdent, position);
            ret = NoSpan;
        } else if (!SubscriptTypeIsOK (expectedType, ret.boundingType))
        {
            LOG_ERROR (RangeOutOfBounds, NoIdent, position);
            ret = NoSpan;
        }
    }
    return ret;
}

/* HandleTypeRange : handle the creation of a range spanning a Numeric or Enum type,
   if expecetedType is non NULL (and non NoType) then check for subtype compliance with
   this type (ie. numeric type is narrower, enumeration type is the same) */
Span HandleTypeRange (PtrType type, PtrType expectedType, tPosition position)
{
    Span ret = NoSpan;

    if (type == NoType)
        LOG_ERROR (TypeIsNotValid, NoIdent, position);
    else if (type->nature != NumericType && type->nature != EnumerationType)
        LOG_ERROR (TypeRangeMustBeNumericOrEnum, NoIdent, position);
    else if (expectedType && expectedType != NoType && !SubscriptTypeIsOK (expectedType, type))
        LOG_ERROR (RangeOutOfBounds, NoIdent, position);
    else
    {
        ret.boundingType = type;
        ret.lowerBound = NewMP_INT (0);
        ret.upperBound = NewMP_INT (0);
        if (type->nature == NumericType)
        {
            if (type->size < 0) /* Signed, min: -2^(n-1), max: 2^(n-1) - 1 */
            {
                mpz_setbit (ret.lowerBound, Abs (type->size) - 1); /* tmp == 2^(n-1) */
                mpz_sub_ui (ret.upperBound, ret.lowerBound, 1); /* 2^(n-1) - 1 */
                mpz_neg (ret.lowerBound, ret.lowerBound); /* - 2^(n-1) */
            } else
            {                   /* Unsigned, min: 0, max: 2^n - 1 */
                mpz_setbit (ret.upperBound, Abs (type->size));
                mpz_sub_ui (ret.upperBound, ret.upperBound, 1);
            }
        } else
            /* Enumeration, find the lower and upper values */
        {
            PtrBindingList enumElems = type->info.enumeration.enumBindings;

            mpz_set (ret.lowerBound, CAR (enumElems).value);
            mpz_set (ret.upperBound, CAR (enumElems).value);
            enumElems = CDR (enumElems);

            while (enumElems)
            {
                if (mpz_cmp (CAR (enumElems).value, ret.lowerBound) < 0)
                    mpz_set (ret.lowerBound, CAR (enumElems).value);
                else if (mpz_cmp (CAR (enumElems).value, ret.upperBound) > 0)
                    mpz_set (ret.upperBound, CAR (enumElems).value);
                enumElems = CDR (enumElems);
            }
        }
    }
    return ret;
}

/* HandleArrayType : construct a new array type iff boundingType, baseType != NoType */
PtrType HandleArrayType (PtrContext context, PtrType baseType, Span range, tPosition position)
{
    PtrType ret = NoType;

    if (range.boundingType == NoType || baseType == NoType)
        LOG_ERROR (TypeIsNotValid, NoIdent, position);
    else if (baseType->nature == ArrayType)
        LOG_ERROR (CannotMakeMultidimensionalArray, NoIdent, position);
    else
        ret = NewArrayType (MarkerIdent, baseType, range);
    return ret;
}

/* ValDeclaration : create a new val */
PtrContext ValDeclaration (PtrContext contextIn, tIdent ident, ExprAttributes expr, tPosition position)
{
    PtrContext newContext = NewContext (contextIn->depth, contextIn->types,
      NewInstanceList (NewValInstance (ident,
          expr.activation,
          expr.components,
          expr.wires,
          expr.accesses,
          expr.value),
        contextIn->instances),
      contextIn->procedures);

    return newContext;
}

/* ProcedureDeclaration : declare a shared/unshared block/procedure */
PtrProcedure
ProcedureDeclaration (PtrContext contextIn, tIdent ident, bool shared,
  PtrInstanceList ports, unsigned portCount, PtrInstanceList extraInstances, PtrContext * blockContext, tPosition position)
{
    PtrProcedure procedure = NoProcedure;

    *blockContext = contextIn;

    if (LookupProcedure (contextIn, ident, true /* Local lookup */ ) !=
      NoProcedure)
        LOG_ERROR (NonUniqueLocalName, ident, position);
    else
    {
        procedure = NewProcedure (ident, (shared ? SharedProcedure : UnSharedProcedure), portCount, position);
        procedure->ports = ports;
        if (portCount != 0)
            procedure->portSpec = MakePortSpecString (ports, portCount);
        procedure->extraPorts = extraInstances; /* Ports which are port array elements */
        procedure->context = contextIn;
        /* Add ports to the block context */
        /* contextIn <- Marker <- Ports <- Local */
        /*            `<- Procedure <- next      */
        /* FIXME, copy extraInstances ? */
        *blockContext = AddContextMarker (contextIn);
        (*blockContext)->instances = AppendInstanceLists (CopyInstanceList (ports), AppendInstanceLists (extraInstances, (*blockContext)->instances));
    }
    return procedure;
}

/* ParameterisedProcedureDeclaration : declare a procedure with parameters */
PtrProcedure ParameterisedProcedureDeclaration (PtrContext contextIn, tIdent ident, union Tree_Node * tree, tPosition position)
{
    PtrProcedure procedure = NoProcedure;

    if (LookupProcedure (contextIn, ident, true /* Local lookup */ ) !=
      NoProcedure)
        LOG_ERROR (NonUniqueLocalName, ident, position);
    else
    {
        procedure = NewProcedure (ident, ParameterisedProcedure, 0, position);
        procedure->info.parameterisedProcedure.tree = tree;
        procedure->context = contextIn;
    }
    return procedure;
}

/* ParameterisedFunctionDeclaration : declare a function with parameters */
PtrProcedure ParameterisedFunctionDeclaration (ProcedureNature nature, PtrContext contextIn, tIdent ident, union Tree_Node * tree, tPosition position)
{
    PtrProcedure function = NoProcedure;

    if (LookupProcedure (contextIn, ident, true /* Local lookup */ ) !=
      NoProcedure)
        LOG_ERROR (NonUniqueLocalName, ident, position);
    else
    {
        function = NewProcedure (ident, nature, 0, position);
        function->info.parameterisedFunction.tree = tree;
        function->context = contextIn;
    }
    return function;
}

/* PushProcedureSpecifyErrorContext : push an error context for building function/procedure
	`procIdent' with the given parameters */
void
PushProcedureSpecifyErrorContext (tIdent procIdent, Ptrchar procedureNature,
  PtrInstanceList constantParams, PtrTypeList typeParams, tPosition position)
{
#define ARBITRARY_STRING_LENGTH_1 (8192)
    /* Arbitrary sized string, sorry FIXME */
    char *str = NEW_ARRAY (char, ARBITRARY_STRING_LENGTH_1);
    char *strPtr;
    PtrInstanceList constantParamsIter = constantParams;
    int paramCount = LengthOfInstanceList (constantParamsIter);
    PtrTypeList typeParamsIter = typeParams;
    bool someOfBothParamNatures = constantParams && typeParams;

    sprintf (str,
      "when expanding parameterised %s `%s'\n    with parameter%s: ", procedureNature, PeekString (procIdent), (paramCount > 1 ? "s" : ""));
    strPtr = EndOfString (str);

    while (constantParamsIter)
    {
        sprintf (strPtr, "%s = ", PeekString (CAR (constantParamsIter)->ident));
        strPtr =
          MakeTypedValueString (strPtr, NewTypedValue (CAR (constantParamsIter)->info.constant.value.baseValue, CAR (constantParamsIter)->type));
        if (CDR (constantParamsIter))
        {
            sprintf (strPtr, ", ");
            strPtr = EndOfString (str);
        }
        constantParamsIter = CDR (constantParamsIter);
    }

    if (someOfBothParamNatures)
    {
        sprintf (strPtr, ", ");
        strPtr = EndOfString (str);
    }

    while (typeParamsIter)
    {
        sprintf (strPtr, "%s = ", PeekString (CAR (typeParamsIter)->ident));
        strPtr = MakeTypeNameString (strPtr, CAR (typeParamsIter));
        if (CDR (typeParamsIter))
        {
            sprintf (strPtr, ", ");
            strPtr = EndOfString (str);
        }
        typeParamsIter = CDR (typeParamsIter);
    }

    PushErrorContext (StrDup (str), position);
    FREE_ARRAY (char, ARBITRARY_STRING_LENGTH_1, str);
}

/* SpecifyParameterisedProcedure : make a specified parameterised procedure.  The generated
   procedure will have the name MakeParameterisedProcedureName (procIdent).
   tree should be a copy of the parameterised procedure tree which should be decorated
   during the specifing action. */
PtrProcedure
SpecifyParameterisedProcedure (PtrContext contextIn, tIdent procIdent,
  PtrProcedure procedure, tTree procedureTree, PtrInstanceList constantParams, PtrTypeList typeParams, tTree ports, tPosition position)
{
    PtrProcedure ret = NoProcedure;

    if (procedure == NoProcedure || procedure->nature != ParameterisedProcedure)
        LOG_ERROR (ExpectingAParameterisedProcedureName, (procIdent), position);
    else if (!constantParams && !typeParams)
        return ret;             /* No parameters */
    else if (!ports)
        return ret;             /* No ports? */
    else
    {                           /* OK, make a procedure */
        /* A procedure, we need to:
           1. Add parameters to a new context derived from tree->context
           2. Visit the non parameter ports with a modified context
           3. Make the procedure
           4. Visit the Block with the port context
           5. Fill in the procedure
           6. Dealias the type parameters
         */
        yProcedureDecl tree = procedureTree->ProcedureDecl;
        PtrContext paramProcInputContext = tree.contextIn;
        PtrContext portInputContext = NewContext (paramProcInputContext->depth,
          AppendTypeLists (CopyTypeList (typeParams),
            paramProcInputContext->types),
          AppendInstanceLists (CopyInstanceList (constantParams),
            paramProcInputContext->instances),
          paramProcInputContext->procedures);
        tIdent specifiedName = MakeSpecifiedProcedureName (procIdent);

        /* 1a. Set a new error context */
        PushProcedureSpecifyErrorContext (procIdent, "procedure", constantParams, typeParams, position);

        /* 2. Visit the non parameters ports with a modified context */
        ports->FormalPorts.context = portInputContext;
        ports->FormalPorts.skip = false;
        ports->FormalPorts.portsType = ProcedurePorts;
        ports->FormalPorts.inConditionalPorts = false;
        VisitTreeNode (ports);
        /* 3. Make the procedure */
        ret = ProcedureDeclaration (portInputContext, specifiedName, false /* Unshared */ ,
          ports->FormalPorts.ports, ports->FormalPorts.portCount, ports->FormalPorts.extraPortInstances, &portInputContext, position);

        if (ret != NoProcedure)
        {                       /* 4. Visit the Block with the port context */
            tTree block = tree.Block;

            block->Block.context = AddContextMarker (portInputContext);
            block->Block.context->depth = contextIn->depth + 1;
            block->Block.skip = false;
            VisitTreeNode (block);
            /* 5. Fill in the procedure */
            ret->context = contextIn; /* Correct context for procedure */
            {
                PtrContext tmpContext = ProcedureDeclarationFillIn (ret, contextIn,
                  false /* Unshared */ ,
                  block->Block.attributes, position);

                /* Did we fill in the procedure correctly? */
                if (tmpContext == contextIn)
                    return NoProcedure;
            }
            /* 6. Dealiasing type params by changing names, still need aliasing support in
               TypeEquivalence but this is OK */
            while (typeParams)
            {
                if (CAR (typeParams) != NoType && CAR (typeParams)->aliasing != NoType)
                    CAR (typeParams)->ident = CAR (typeParams)->aliasing->ident;
                typeParams = CDR (typeParams);
            }
        }
        /* Free the tree. ... Second thoughts, don't bother */
        /* Pop the error context */
        PopErrorContext ();
    }
    return ret;
}

/* SpecifyParameterisedBuiltinFunction : like SpecifyParameterisedProcedure but for builtin functions.
   This will largely be the same but also involves handling builtin functions */
PtrProcedure
SpecifyParameterisedBuiltinFunction (PtrContext contextIn, tIdent funcIdent,
  PtrProcedure function,
  tTree functionTree,
  PtrInstanceList constantParams, PtrTypeList typeParams, tTree ports, PtrComponentParameterList componentParameters, tPosition position)
{
    PtrProcedure ret = NoProcedure;

    if (function == NoProcedure || function->nature != ParameterisedBuiltinFunction)
        LOG_ERROR (ExpectingAParameterisedFunctionName, (funcIdent), position);
    else if (!constantParams && !typeParams)
        return ret;             /* No parameters */
    else if (!ports)
        return ret;             /* No ports? */
    else
    {                           /* OK, make a function */
        /* A function, we need to:
           1. Add parameters to a new context derived from tree->context
           2. Visit the non parameter ports with a modified context
           3. Make the function
           4. Fill in the function
           5. Dealias the type parameters
         */
        yBuiltinFunctionDecl tree = functionTree->BuiltinFunctionDecl;
        PtrContext paramFuncInputContext = tree.contextIn;
        PtrContext portInputContext = NewContext (paramFuncInputContext->depth,
          AppendTypeLists (CopyTypeList (typeParams),
            paramFuncInputContext->types),
          AppendInstanceLists (CopyInstanceList (constantParams),
            paramFuncInputContext->instances),
          paramFuncInputContext->procedures);
        tIdent specifiedName = MakeSpecifiedProcedureName (funcIdent);

        /* 1a. Set a new error context */
        PushProcedureSpecifyErrorContext (funcIdent, "builtin function", constantParams, typeParams, position);

        /* 2. Visit the non parameters ports with a modified context */
        ports->FormalPorts.context = portInputContext;
        ports->FormalPorts.skip = false;
        ports->FormalPorts.portsType = FunctionArgs;
        ports->FormalPorts.inConditionalPorts = false;
        VisitTreeNode (ports);
        /* 3. Make the function */
        ret =
          FunctionDeclaration (portInputContext, specifiedName,
          ParameterisedBuiltinFunction, ports->FormalPorts.ports, ports->FormalPorts.portCount, &portInputContext, position);

        if (ret != NoProcedure)
        {                       /* 4. Visit the return type */
            tTree returnType = tree.AType;

            returnType->AType.context = AddContextMarker (portInputContext);
            returnType->AType.context->depth = contextIn->depth + 1;
            returnType->AType.skip = false;
            VisitTreeNode (returnType);

            /* If the return type is one of the parameter types, unalias it */
            if (TypeIsInTypeList (returnType->AType.type, typeParams))
                ret->info.parameterisedFunction.returnType = returnType->AType.type->aliasing;
            else
                ret->info.parameterisedFunction.returnType = returnType->AType.type;

            if (ret->info.parameterisedFunction.returnType == NoType)
            {
                LOG_ERROR (TypeIsNotValid, NoIdent, position);
                return NoProcedure;
            }

            componentParameters = ReverseComponentParameterList (componentParameters);
            ret->info.parameterisedFunction.parameters = componentParameters;

            /* 5. Dealiasing type params by changing names, still need aliasing support in
               TypeEquivalence but this is OK */
            while (typeParams)
            {
                if (CAR (typeParams) != NoType && CAR (typeParams)->aliasing != NoType)
                    CAR (typeParams)->ident = CAR (typeParams)->aliasing->ident;
                typeParams = CDR (typeParams);
            }
            ret->info.parameterisedFunction.ident = funcIdent;
        }
        /* Free the tree. ... Second thoughts, don't bother */
        /* Pop the error context */
        PopErrorContext ();
    }
    return ret;
}

/* FunctionDeclaration : declare a shared/unshared function */
PtrProcedure
FunctionDeclaration (PtrContext contextIn, tIdent ident,
  ProcedureNature nature, PtrInstanceList args, unsigned argCount, PtrContext * blockContext, tPosition position)
{
    PtrProcedure function = NoProcedure;

    *blockContext = contextIn;

    if (LookupProcedure (contextIn, ident, true /* Local lookup */ ) !=
      NoProcedure)
        LOG_ERROR (NonUniqueLocalName, ident, position);
    else
    {
        /* FIXME Need to worry about much more than this */
        function = NewProcedure (ident, nature, 0 /* port count FIXME? */ , position);
        function->ports = args;
        function->portCount = LengthOfInstanceList (args);
        if (argCount != 0)
            function->portSpec = MakePortSpecString (args, argCount);
        function->context = contextIn;
        /* Add args to the block context */
        /* contextIn <- Marker <- Ports <- Local */
        /*            `<- Procedure <- next      */
        *blockContext = AddContextMarker (contextIn);
        (*blockContext)->instances = AppendInstanceLists (CopyInstanceList (args), (*blockContext)->instances);
    }
    return function;
}

/* CheckAllPortsAreUsedAndModifySense : check that each port instance in the given list has the wire field set,
   only check count ports (or upto ports == NULL iff count == -1), also modify
   the sense of the ports to reflect the nature of the wire connected to them,
   if arrayed is true then require that the sense of all ports is the same */
bool CheckAllPortsAreUsedAndModifySense (PtrInstanceList ports, int count, bool isArrayed, tPosition position)
{
    bool allUsed = true;
    bool checkSense = false;
    PortSense sense = DefaultPortSense;

    while (ports && count)
    {
        /* Check the parts of a arrayed port */
        if (CAR (ports)->type->nature == ArrayedType)
        {
            allUsed = allUsed
              && CheckAllPortsAreUsedAndModifySense (CAR (ports)->type->info.
              arrayed.arrayedElements, CAR (ports)->type->info.arrayed.elementCount, true /* Arrayed */ ,
              position);
            /* Take the sense of the first element */
            CAR (ports)->info.port.sense = CAR (CAR (ports)->type->info.arrayed.arrayedElements)->info.port.sense;
        } else if (!CAR (ports)->info.port.wire) /* No connection */
        {
            LOG_ERROR (PortIsUnused, (CAR (ports)->ident), position);
            allUsed = false;
        } else                  /* OK, check the sense of the port */
        {
            /* check for active outputs not connected to a write channel */
            if (CAR (ports)->nature == OutputChannelInstance && CAR (ports)->info.port.wire->isPull)
                LOG_ERROR (ExpectingAReadableChannel, NoIdent, CAR (ports)->info.port.wire->position);

            /* make sure we don't have rogue passive outputs */
            if ((CAR (ports)->nature == InputChannelInstance &&
                !CAR (ports)->info.port.wire->isPull) || (CAR (ports)->nature == OutputChannelInstance && CAR (ports)->info.port.wire->isPull))
                CAR (ports)->info.port.sense = PassivePortSense;
            else
                CAR (ports)->info.port.sense = ActivePortSense;

            if (isArrayed)      /* Require same sense from all elements */
            {
                if (!checkSense)
                {
                    checkSense = true;
                    sense = CAR (ports)->info.port.sense;
                } else if (CAR (ports)->info.port.sense != sense)
                {
                    LOG_ERROR (ArrayedPortElementsMustHaveSameSense, (CAR (ports)->ident), position);
                }
            }
        }
        if (count > 0)
            count--;
        ports = CDR (ports);
    }
    return allUsed;
}

/* ResolveLocalPortAccesses : remove local port accesses from block access list and generate
   new list with only non local accesses, set wire field in port instances. */
PtrAccessList ResolveLocalPortAccesses (PtrAccessList blockAccesses, PtrInstanceList ports)
{
    /* Recurse down access list */
    if (blockAccesses)
        switch (CAR (blockAccesses)->inst->nature)
        {
        case InputChannelInstance:
        case OutputChannelInstance:
            if (InstanceIsLocal (CAR (blockAccesses)->inst, ports))
            {
                /* Set the wire field in the port */
                CAR (blockAccesses)->inst->info.port.wire = CAR (blockAccesses)->info.channel.wire;
                return ResolveLocalPortAccesses (CDR (blockAccesses), ports);
            }                   /* else fall through, sorry */
        default:
            return NewAccessList (CAR (blockAccesses), ResolveLocalPortAccesses (CDR (blockAccesses), ports));
            break;
        }
    return NULL;
}

/* ProcedureDeclarationFillIn : fill in procedure structure initialised by ProcedureDeclaration
   (or fail silently if procedure == NULL), perform consistency checks on block accesses to ports */
PtrContext
ProcedureDeclarationFillIn (PtrProcedure procedure, PtrContext contextIn, bool shared, CommandAttributes blockAttributes, tPosition position)
{
    PtrContext ret = contextIn;

    ASSERT (procedure);         /* Must be NoProcedure or else something useful */
    if (procedure != NoProcedure) {
        /* Resolve all the elements of the unshared procedure */
        /* 1: blockAttributes - components, accesses, wires, activation, permanent */
        procedure->info.procedure.attributes = blockAttributes;

        if (!shared)
        {
            /* Insist on an activation */
            if (!procedure->info.procedure.attributes.activation)
                return contextIn;

            /* CallContext: Add some debug information to the wires */
            CallContext *id = CallContext_Add (procedure->ident, position, NoPosition, NULL);

            CallContext_SetWirePositions (procedure->info.procedure.attributes.wires, procedure, id);

            /* Is this a short circuit component?, no components, only one access */
            if (!procedure->info.procedure.attributes.components &&
              procedure->info.procedure.attributes.accesses && !CDR (procedure->info.procedure.attributes.accesses))
            {
                PtrWire newActivation = NewSyncWire (1 /* fix later */ , position);

                procedure->info.procedure.attributes.wires = NewWireList (newActivation, procedure->info.procedure.attributes.wires);

                procedure->info.procedure.attributes.components =
                  NewComponentList (NewForkConnectorComponent (position, newActivation,
                    procedure->info.procedure.attributes.activation), procedure->info.procedure.attributes.components);
                procedure->info.procedure.attributes.activation = newActivation;
                procedure->info.procedure.attributes.accesses = NULL;
            }
            procedure->info.procedure.attributes.accesses =
              ResolveLocalPortAccesses (ResolveLocalPortAccesses (blockAttributes.accesses, procedure->ports), procedure->extraPorts);

            /* 2: check ports are all accessed, modify sense of ports to reflect active/passive */
            if (CheckAllPortsAreUsedAndModifySense (procedure->ports, -1 /* all ports */ , false,
                procedure->position))
            {
                ReassignPortWires (procedure);
                /* Set up context for next declaration */
                ret = NewContext (contextIn->depth, contextIn->types, contextIn->instances, NewProcedureList (procedure, contextIn->procedures));
            }
            /* 4: optimise the HC if that is required */
            if (DoOptimise && contextIn->depth == TopLevelContextDepth && ErrorCount == 0)
            {
                OptimiseProcedure (procedure, position);
                CallContext_AfterOptimisation ();
            }
        } else
            /* Make an unplaced copy of the block accesses */
        {
            /* Check that no non-port channel accesses were made */
            if (CheckAccessListForNonPortChannels (procedure->info.procedure.attributes.accesses))
            {
                procedure->unplacedAccesses = CopyAndUnplaceAccessList (procedure->info.procedure.attributes.accesses);
                /* Create a dummy instance for sequencing accesses in access lists */
                procedure->dummyInstance = NewSharedCallInstance (procedure->ident);
                ret = NewContext (contextIn->depth, contextIn->types, contextIn->instances, NewProcedureList (procedure, contextIn->procedures));
            }
        }
    }
    return ret;
}

/* MakeVariableInstancesIntoInputChannels : change the ports in the given list from variables into
   input channels */
void MakeVariableInstancesIntoInputChannels (PtrInstanceList ports)
{
    while (ports)
    {
        PtrInstance varInst = CAR (ports);

        /* Is this a function argument ? */
        if (varInst->info.variable.readOnly && !varInst->info.variable.channelInstance)
        {                       /* Modify instance in situ. */
            CAR (ports)->nature = InputChannelInstance;
            CAR (ports)->info.port.sense = ActivePortSense;
            CAR (ports)->info.port.wire = NULL;
            CAR (ports)->info.port.canMulticast = true;
        }
        ports = CDR (ports);
    }
}

/* FilterFunctionAccessesAndCreateInputChannelReadConnections : remove input channels from accesses and return
   remaining accesses, also create read trees from variable accesses with input channel instances and connect
   up to those channel's wires */
PtrAccessList
FilterFunctionAccessesAndCreateInputChannelReadConnections (PtrAccessList accesses, PtrWireList * wires, PtrComponentList * comps, tPosition position)
{
    PtrInstance inst;
    PtrAccessList next;

    if (accesses)
    {
        unsigned portWidth;

        inst = CAR (accesses)->inst;
        portWidth = Abs (inst->type->size);
        next = FilterFunctionAccessesAndCreateInputChannelReadConnections (CDR (accesses), wires, comps, position);

        if (inst->nature == InputChannelInstance)
        {
            /* Create masks and gather a list of wires to connect to the port */
            PtrWireList forkedWires = NULL;
            unsigned wireCount = 0;
            PtrWireList readWires = CAR (accesses)->info.variable.wires;

            while (readWires)
            {
                if (CAR (readWires)->width != portWidth) /* Place a mask */
                {
                    PtrWire portEndWire = NewWire (1 /* fix later */ , portWidth, inst->type,
                      0, true /* pull */ , position);

                    *wires = NewWireList (portEndWire, *wires);
                    *comps = NewComponentList (NewSliceComponent (position, CAR (readWires)->offset, CAR (readWires), portEndWire), *comps);
                    forkedWires = NewWireList (portEndWire, forkedWires);
                } else
                    forkedWires = NewWireList (CAR (readWires), forkedWires);
                wireCount++;
                readWires = CDR (readWires);
            }

            if (wireCount == 1)
                inst->info.port.wire = CAR (forkedWires);
            else
            {                   /* Create fork */
                PtrWire portWire = NewWire (1 /* fix later */ , portWidth, inst->type,
                  0, true /* pull */ , position);

                *wires = NewWireList (portWire, *wires);
                *comps = NewComponentList (NewSynchPullFromListComponent (position, forkedWires, portWire), *comps);
                inst->info.port.wire = portWire;
            }
            return next;
        } else
            return NewAccessList (CAR (accesses), next);
    }
    return NULL;
}

/* ConnectAndTransformFunctionPorts : take the variable accesses from a function expression and create
   a forked,adapted tree of connections onto a set of `active input' ports from those variable port accesses.
   Returns true iff we were successful in creating a function.  expectedType holds the type we expected the
   function to have with the `function name (args) = expr : type' format */
bool ConnectAndTransformFunctionPorts (PtrProcedure function, PtrType expectedType, tPosition position)
{
    /* Sanity check the function */
    ASSERT (function);

    /* Check activation, fail silently */
    if (!function->info.function.attributes.activation)
        return false;
    /* Check type against expectation */
    if (expectedType != NoType && !TypeEquivalence (expectedType, function->info.function.attributes.value.type))
        LOG_ERROR (FunctionExpressionHasWrongType, (function->ident), position);
    else
    {
        PtrInstanceList ports = function->ports;
        bool ret = true;

        /* Modify port natures to look like a procedure */
        MakeVariableInstancesIntoInputChannels (function->ports);
        /* Now the only accesses to channels in the access list instances are to this functions arguments */

        /* Traverse the accesses, make a list of external accesses and connect up internal ones, set the
           wire fields of the ports */
        function->info.function.attributes.accesses =
          FilterFunctionAccessesAndCreateInputChannelReadConnections
          (function->info.function.attributes.accesses,
          &function->info.function.attributes.wires, &function->info.function.attributes.components, position);
        while (ports)
        {
            if (!CAR (ports)->info.port.wire)
            {
                LOG_ERROR (ArgumentIsUnused, (CAR (ports)->ident), position);
                ret = false;
            }
            ports = CDR (ports);
        }
        return ret;
    }
    return false;
}

/* FunctionDeclarationFillIn : fill in procedure structure initialised by FunctionDeclaration
   (or fail silently if function == NULL), perform consistency checks on expression accesses to arguments
   and place argument spliting hardware */
PtrContext
FunctionDeclarationFillIn (PtrProcedure function, PtrContext contextIn, ExprAttributes exprAttributes, PtrType expectedType, tPosition position)
{
    PtrContext ret = contextIn;

    ASSERT (function);          /* Must be NoProcedure or else something useful */

    if (function != NoProcedure)
    {
        /* Resolve all the elements of the unshared function */
        /* 1: exprAttributes - components, accesses, wires, activation, permanent */
        function->info.function.attributes = exprAttributes;

        if (function->nature != SharedFunction)
        {
            /* Insist on an activation */
            if (!function->info.function.attributes.activation)
            {
                /* Do some type checking (and moaning) first */
                if (TypedValueIsNotBuiltin (exprAttributes.value, position))
                {
                    if (exprAttributes.value.value.dontCares)
                        LOG_ERROR (ImplicantNotAllowedHere, NoIdent, position);
                }
                return contextIn;
            }
            function->info.function.returnType = function->info.function.attributes.activation->type.type;

            /* CallContext: Add some debug information to the wires */
            CallContext *id = CallContext_Add (function->ident, position, NoPosition, NULL);

            CallContext_SetWirePositions (function->info.procedure.attributes.wires, function, id);

            /* 1a: change things about (add an adapter) if this is a feed through */
            if (function->portCount == 1 && LengthOfAccessList (function->info.function.attributes.accesses) == 1)
            {
                PtrAccess singleAccess = CAR (function->info.function.attributes.accesses);

                if (singleAccess->inst == CAR (function->ports) &&
                  LengthOfWireList (singleAccess->info.variable.wires) == 1 &&
                  CAR (singleAccess->info.variable.wires) == function->info.function.attributes.activation)
                {               /* this is a feedthrough */
                    PtrWire newAct = CopyWire (function->info.function.attributes.activation);
                    PtrWire oldAct = function->info.function.attributes.activation;

                    function->info.function.attributes.activation = newAct;

                    function->info.function.attributes.wires = NewWireList (newAct, function->info.function.attributes.wires);
                    function->info.function.attributes.components =
                      NewComponentList (NewAdaptComponent (position, false, false, newAct, oldAct), function->info.function.attributes.components);
                }
            }

            /* 2: place accesses to ports, transform ports into active input ports */
            if (!ConnectAndTransformFunctionPorts (function, expectedType, position))
                return ret;
            /* 3: reassign wire numbers */
            ReassignPortWires (function);
            /* 4: Set up context for next declaration */
            ret = NewContext (contextIn->depth, contextIn->types, contextIn->instances, NewProcedureList (function, contextIn->procedures));
            /* 5: optimise the HC if that is required */
            if (DoOptimise && contextIn->depth == TopLevelContextDepth)
                OptimiseProcedure (function, position);
        } else
            /* Make an unplaced copy of the block accesses */
        {
#if 0
/* FIXME */
            /* Check that no non-port channel accesses were made */
            if (CheckAccessListForNonPortChannels (function->attributes.accesses))
            {
                function->unplacedAccesses = CopyAndUnplaceAccessList (function->attributes.accesses);
                /* Create a dummy instance for sequencing accesses in access lists */
                function->dummyInstance = NewSharedCallInstance (function->ident);
                ret = NewContext (contextIn->depth, contextIn->types, contextIn->instances, NewProcedureList (function, contextIn->functions));
            }
#endif
        }
    }
    return ret;
}

/* BuiltinFunctionDeclarationFillIn : fill in procedure structure initialised by FunctionDeclaration
   (or fail silently if function == NULL), perform consistency checks */
PtrContext BuiltinFunctionDeclarationFillIn (PtrProcedure function, PtrContext contextIn, PtrType expectedType, tPosition position)
{
    PtrContext ret = contextIn;

    ASSERT (function);          /* Must be NoProcedure or else something useful */
    if (function != NoProcedure && expectedType != NoType)
    {
        /* Might need to do something later, placeholder */
        ret = NewContext (contextIn->depth, contextIn->types, contextIn->instances, NewProcedureList (function, contextIn->procedures));

        function->info.function.attributes = NoExprAttributes;
        function->info.function.attributes.value = NewTypedValue (NULL, expectedType);
        function->info.function.returnType = expectedType;
        function->ports = CDR (ConvertFunctionPortsToProcedureComponentPorts (function->ports, function->info.function.returnType));
        MakeWiresForBuiltinFunctionPorts (function->ports, &(function->info.function.attributes.wires), 1, function->portCount + 1, position);
    }
    return ret;
}

/* PartDeclaration : declare a procedure from a part in a breeze file */
PtrProcedure
PartDeclaration (PtrContext contextIn, tIdent ident, PtrInstanceList ports,
  unsigned portCount, PtrInstanceList extraInstances, PtrLispList attributes, tPosition position)
{
    PtrProcedure procedure = NULL;

    if (LookupProcedure (contextIn, ident, true /* Local lookup */ ) !=
      NoProcedure)
        LOG_ERROR (NonUniqueLocalName, (ident), position);
    else
    {
        procedure = NewAttributedProcedure (ident, attributes, portCount - 1, position);

        if (!procedure)
        {
            LOG_ERROR (ProblemReadingPart, ident, position);
            return NULL;
        }
        /* Decorate procedure */
        procedure->ports = ports;
        procedure->portSpec = MakePortSpecString (ports, portCount);
        procedure->extraPorts = extraInstances;
        procedure->context = contextIn;
    }

    return procedure;
}

/* PartDeclarationFillIn : fill in procedure structure initialised by PartDeclaration
   (or fail silently if procedure == NULL) */
PtrContext
PartDeclarationFillIn (PtrProcedure procedure, PtrContext contextIn,
  PtrWireArray localChannels, unsigned localChannelCount, PtrComponentList components, tPosition position)
{
    PtrContext ret = contextIn;
    bool isExternal = procedure->nature == BuiltinFunction;

    /* Fail silently if any of these are missing */
    if (procedure && (isExternal || (localChannels && components)))
    {
        if (!isExternal)        /* Non external procedures, add args */
        {
            PtrWireList wiresIter = WIRE_ARRAY_TO_LIST (localChannels, localChannelCount);

            /* Fixup types in wire list */
            while (wiresIter)
            {
                tTree tree = CAR (wiresIter)->type.tree;

                if (tree != NoTree)
                {
                    tree->AType.context = contextIn;
                    tree->AType.skip = false;
                    VisitTreeNode (tree);
                    CAR (wiresIter)->type.type = tree->AType.type;
                } else
                    CAR (wiresIter)->type.type = NoType;
                wiresIter = CDR (wiresIter);
            }

            if (procedure->nature == UnSharedProcedure)
            {
                procedure->info.procedure.attributes.wires = WIRE_ARRAY_TO_LIST (localChannels, localChannelCount);
                procedure->info.procedure.attributes.components = components;
                procedure->info.procedure.attributes.activation = localChannels[0].body; /* wire 1 */
            } else
            {
                procedure->info.function.attributes.wires = WIRE_ARRAY_TO_LIST (localChannels, localChannelCount);
                procedure->info.function.attributes.components = components;

                procedure->info.function.attributes.activation = localChannels[0].body; /* wire 1 */
                /* Pick up the type of the first (activation) port */
                procedure->info.function.attributes.value.type = CAR (procedure->ports)->type;
            }
            PlaceWiresInPortList (procedure->ports, localChannels, 1, LengthOfInstanceList (procedure->ports));
        }
        procedure->info.function.returnType = CAR (procedure->ports)->type;
        /* Remember activation options */
        procedure->activationOptions = CAR (procedure->ports)->info.port.options;
        procedure->ports = CDR (procedure->ports); /* Activation is implicit */
        ret = NewContext (contextIn->depth, contextIn->types, contextIn->instances, NewProcedureList (procedure, contextIn->procedures));
    }
    return ret;
}

/* PlaceWiresInPortList : place the wires firstWireNo ... into the given ports */
unsigned PlaceWiresInPortList (PtrInstanceList ports, PtrWireArray wires, unsigned firstWireNo, unsigned portCount)
{
    unsigned wireNo = firstWireNo;

    while (ports && portCount)
    {
        if (CAR (ports)->type->nature == ArrayedType)
            wireNo =
              PlaceWiresInPortList (CAR (ports)->type->info.arrayed.arrayedElements, wires, wireNo, CAR (ports)->type->info.arrayed.elementCount);
        else
        {
            CAR (ports)->info.port.wire = wires[firstWireNo - 1].body;
            wireNo++;
        }
        portCount--;
        ports = CDR (ports);
    }
    return wireNo;
}

/* MakeWiresForBuiltinFunctionPorts : make the wires (and add to the given wire list) for a builtin
	function's ports */
unsigned MakeWiresForBuiltinFunctionPorts (PtrInstanceList ports, PtrWireList * wires, unsigned firstPortNo, unsigned portCount, tPosition position)
{
    unsigned wireNo = firstPortNo;

    while (ports && portCount)
    {
        if (CAR (ports)->type->nature == ArrayedType)
            wireNo =
              MakeWiresForBuiltinFunctionPorts (CAR (ports)->type->info.arrayed.
              arrayedElements, wires, wireNo, CAR (ports)->type->info.arrayed.elementCount, position);
        else
        {
            PtrWire newWire;

            if (CAR (ports)->type->nature == SyncType)
                newWire = NewSyncWire (wireNo, position);
            else
                newWire = NewWire (wireNo, Abs (CAR (ports)->type->size), CAR (ports)->type, 0, true /* pull */ , position);

            CAR (ports)->info.port.wire = newWire;
            *wires = NewWireList (newWire, *wires);

            wireNo++;
        }
        portCount--;
        ports = CDR (ports);
    }
    return wireNo;
}
