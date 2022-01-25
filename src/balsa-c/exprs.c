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

	`exprs.c'
	Expression handling functions
	
 */

#include "exprs.h"
#include "commands.h"
#include "contexts.h"
#include "flags.h"
#include "Errors.h"
#include "ports.h"
#include "parts.h"
#include "Tree.h"
#include <string.h>

DEFINE_LIST_TYPE_CONSTRUCTOR (ExprAttributes, ExprAttributes)
DEFINE_LIST_TYPE_LENGTHOF (ExprAttributes) DEFINE_LIST_TYPE_APPEND (ExprAttributes) DEFINE_LIST_TYPE_REVERSE (ExprAttributes)
ExprAttributes NoExprAttributes;

/* Initialise */
void BeginExprs (void)
{
    NoExprAttributes.components = NULL;
    NoExprAttributes.accesses = NULL;
    NoExprAttributes.wires = NULL;
    NoExprAttributes.activation = NULL;
    NoExprAttributes.value.value.baseValue = NULL;
    NoExprAttributes.value.type = NoType;
}

/* DetermineGuardNature : is the given expression a good guard value, is it a
	constant 1 or 0, or does it not have a constant value */
GuardNature DetermineGuardNature (ExprAttributes guard, bool checkType, bool mustBeConst, tPosition position)
{
    GuardNature ret = FalseGuardNature;

    if (checkType && !TypeEquivalence (guard.value.type, BitTypeObj))
        /* Check type of expression, must be the same as expected */
    {
        LOG_ERROR (GuardExprMustHaveBitType, NoIdent, position);
    } else
    {
        if (guard.value.value.dontCares) /* Don't allow don't cares */
            LOG_ERROR (ImplicantNotAllowedHere, NoIdent, position);
        else if (guard.value.value.baseValue) /* Constant guard */
            ret = (mpz_sgn (guard.value.value.baseValue) == 0 ? FalseGuardNature : TrueGuardNature);
        else if (mustBeConst)
            LOG_ERROR (ExpressionMustBeConstant, NoIdent, position);
        else
            ret = NonConstantGuardNature;
    }
    return ret;
}

/* CheckAndSizeOfType : wierd function to check a type for NoType'ness and return 0, else
   returns the SignedBits size of the type type */
SignedBits CheckAndSizeOfType (PtrType type, tPosition position)
{
    if (type == NoType)
        LOG_ERROR (TypeIsNotValid, NoIdent, position);
    else
        return type->size;
    return 0;
}

/* ExistingTypeLookup : lookup a type and complain if it doesn't exist.
   Returns NoType if type did not exist in the given context */
PtrType ExistingTypeLookup (PtrContext context, tIdent ident, tPosition position)
{
    PtrType type = LookupType (context, ident, false /* not just local */ );

    if (type == NoType)
        LOG_ERROR (NoTypeFound, ident, position);
    return type;
}

/* HandleLiteralExpr : create constant component for literals, handles implicit numeric cast too */
ExprAttributes HandleLiteralExpr (PtrMP_INT literal, PtrType expectedType, tPosition position)
{
    SignedBits requiredRange = SmallestRangeToHoldValue (literal);
    PtrWire newWire;
    ExprAttributes ret = NoExprAttributes;

    /* Implement implicit cast */
    if (expectedType && expectedType->nature == NumericType && RangeIsWiderOrEqual (expectedType->size, requiredRange))
        ret.value = NewTypedValue (literal, expectedType);
    else
        ret.value = NewTypedValue (literal, NewNumericType (MarkerIdent, requiredRange));

    newWire = NewWire (1, Abs (ret.value.type->size), ret.value.type, 0, true /* Pull */ , position);
    ret.components = NewComponentList (NewConstantComponent (position, ret.value.value.baseValue, newWire), NULL);
    ret.wires = NewWireList (newWire, NULL);
    ret.activation = newWire;
    return ret;
}

/* HandleStringExpr : create constant string component for string literals */
ExprAttributes HandleStringExpr (Ptrconstchar string, tPosition position)
{
    ExprAttributes ret = NoExprAttributes;

    /* The copy tree may not be necessary, but safety first */
    tTree functionTree = CopyTree (StringFunction->info.parameterisedFunction.tree);
    tIdent stringIdent = MakeIdent1 ("string");
    PtrInstance stringInst = NewConstantInstance (stringIdent,
      NewTypedValueBuiltin ((void *) string, StringTypeObj));

    ret = HandleFunctionCallExpr (NULL,
      SpecifyParameterisedBuiltinFunction
      (PredefinedContext, StringFunction->ident,
        StringFunction, functionTree,
        NewInstanceList (stringInst, NULL), NULL,
        functionTree->BuiltinFunctionDecl.
        FormalPorts->FormalPort.next,
        NewComponentParameterList (NewComponentParameter (StringComponentParameter, (void *) string, stringIdent, NoType), NULL), position), position);
    ret.value = NewTypedValueBuiltin ((void *) string, StringTypeObj);

    return ret;
}

/* HandleIdentExpr : resolve an identifier from either instances or
   enumeration element values (iff expectedType has nature EnumerationType,
   returns type and value of identifier.)
   If `allowChannels' is true then return channel accesses for channel matching idents.
   Returned value attribute has value NULL and type NoType on error */
ExprAttributes HandleIdentExpr (PtrContext context, tIdent ident, PtrType expectedType, bool allowChannels, tPosition position)
{
    ExprAttributes ret = NoExprAttributes;

    if (expectedType->nature == EnumerationType) /* Lookup enum elem */
        ret.value = NewTypedValue (LookupEnumElemValue (expectedType, ident), expectedType);
    if (!ret.value.value.baseValue)
    {
        PtrInstance inst = LookupInstance (context, ident, false);

        ret.value.type = NoType; /* Just incase Enum didn't work */
        if (!inst)
            LOG_ERROR (UnboundIdentifier, ident, position);
        else
            switch (inst->nature)
            {
            case ConstantInstance:
                ret.value = NewTypedValueImplicant (CopyImplicant (inst->info.constant.value), inst->type);
                break;
            case VariableInstance:
                {
                    /* Create a new wire for / access on variable read */
                    PtrWire newWire = NewWire (1,
                      Abs (inst->type->size), inst->type,
                      0, true /* Pull read */ , position);

                    newWire->ident = inst->ident;

                    ret.wires = NewWireList (newWire, NULL);
                    ret.value.type = inst->type;
                    ret.accesses = NewAccessList (NewVariableAccess (inst, newWire), NULL);
                    ret.activation = newWire;
                }
                break;
            case ValInstance:
            	{
                    /* Create a new wire for result */
                    PtrWire newWire = NewWire (1,
                      Abs (inst->type->size), inst->type,
                      0, true /* Pull read */ , position);

                    newWire->ident = inst->ident;

                    ret.wires = NewWireList (newWire, NULL);
                    ret.value.type = inst->type;
                    ret.accesses = NewAccessList (NewValAccess (inst, newWire), NULL);
                    ret.activation = newWire;
            	}
            	break;
            default:           /* Channel accesses */
                if (allowChannels)
                {
                    ret.wires = NULL;
                    ret.value.type = inst->type;
                    /* Just construct an access to tunnel back to the calling process. */
                    ret.accesses = NewAccessList (NewChannelAccess (inst, ChannelActiveRead, NULL), NULL);
                    ret.activation = NULL;
                } else
                    LOG_ERROR (ChannelsCantBeUsedInExpr, ident, position);
                break;
            }
    }
    if (ret.value.value.baseValue) /* Add constant if we have a value */
    {
        if (ret.value.type == StringTypeObj)
        {
            ret = HandleStringExpr (GetBuiltinValuePointer (ret.value.value.baseValue), position);
        } else
        {
            PtrWire newWire = NewWire (1, Abs (ret.value.type->size), ret.value.type, 0,
              true /* Pull from constant */ , position);

            ret.components = NewComponentList (NewConstantComponent (position, ret.value.value.baseValue, newWire), NULL);
            ret.wires = NewWireList (newWire, NULL);
            ret.activation = newWire;
        }
    }
    return ret;
}

/* HandleImplicantExpr : create an implicant, allowing for promotion as with literals */
ExprAttributes HandleImplicantExpr (Implicant implicant, PtrType expectedType, tPosition position)
{
    SignedBits requiredRange = SmallestRangeToHoldValue (implicant.baseValue);
    PtrWire newWire;
    ExprAttributes ret = NoExprAttributes;

    /* Implement implicit cast */
    if (expectedType && expectedType->nature == NumericType && RangeIsWiderOrEqual (expectedType->size, requiredRange))
        ret.value = NewTypedValueImplicant (implicant, expectedType);
    else
        ret.value = NewTypedValueImplicant (implicant, NewNumericType (MarkerIdent, requiredRange));

    /* Normalise dontCares */
    if (implicant.dontCares && mpz_sgn (implicant.dontCares) == 0)
        implicant.dontCares = NULL;

    if (!implicant.dontCares)   /* Allow dontCares to be empty */
    {
        newWire = NewWire (1, Abs (ret.value.type->size), ret.value.type, 0, true /* Pull */ , position);
        ret.components = NewComponentList (NewConstantComponent (position, ret.value.value.baseValue, newWire), NULL);
        ret.wires = NewWireList (newWire, NULL);
        ret.activation = newWire;
    }
    return ret;
}

/* HandleDontCareExpr : create a dont care, similar to implicant generation but consists entirely of dontCare bits */
ExprAttributes HandleDontCareExpr (PtrType expectedType, tPosition position)
{
    ExprAttributes ret = NoExprAttributes;
    Implicant retValue;

    if (!expectedType || expectedType == NoType)
        LOG_ERROR (ImplicantNotAllowedHere, NoIdent, position);
    else if (expectedType->size < 0)
        LOG_ERROR (SignedImplicantsNotAllowed, NoIdent, position);
    else
    {
        retValue.baseValue = NewMP_INT (0);
        retValue.dontCares = MakeMaskForRange (Abs (expectedType->size), 0);
        ret.value = NewTypedValueImplicant (retValue, expectedType);
    }

    return ret;
}

/* MakeConstantExprAttributes : make a Constant components attached to a wire as an ExprAttributes */
ExprAttributes MakeConstantExprAttributes (PtrMP_INT value, PtrType type, tPosition position)
{
    ExprAttributes ret = NoExprAttributes;
    PtrWire newWire = NewWire (1, Abs (type->size), type, 0, true /* Pull */ , position);

    ret.wires = NewWireList (newWire, NULL);
    ret.components = NewComponentList (NewConstantComponent (position, value, newWire), NULL);
    ret.activation = newWire;
    ret.value = NewTypedValue (value, type);

    return ret;
}

/* GetAggregateConsExprElements : lookup elements of type type, if this is NULL then use
   the expectedType unless it is not a record or array type. Complain if the typing is wrong
   and return NULL, actualType gets the real type of the expression */
PtrInstanceList GetAggregateConsExprElements (struct Context * context, tIdent ident, PtrType expectedType, PtrType * actualType, tPosition position)
{
    PtrType aggregateType;

    *actualType = NoType;

    if (ident == NoIdent)
        aggregateType = expectedType;
    else if ((aggregateType = LookupType (context, ident, false /* not just local */ )) == NoType)
        LOG_ERROR (NoTypeFound, ident, position);

    /* aggregateType should be a valid type here */
    if (aggregateType != NoType)
    {
        switch (aggregateType->nature)
        {
        case RecordType:
            *actualType = aggregateType;
            return aggregateType->info.record.recordElements;
            break;
        case ArrayType:
            *actualType = aggregateType;
            return NULL;
            break;
        default:               /* Fall through to error report */
            break;
        }
    }
    LOG_ERROR (CannotDetermineTypeOfAggregate, NoIdent, position);
    return NULL;
}

/* CombineComponentTree : Create a chain of combine components linking all the inputs in 
   inputs into a single bundle (returned in activation),
   NB. inputs is ordered lsw -> next_lsw -> ... -> msw -> NULL,
   prepends newly created wires (internal wires + activation) to wires */
PtrComponentList
CombineComponentTree (PtrWireList inputs, Bits totalWidth, PtrType finalType, PtrWire * activation, PtrWireList * wires, tPosition position)
{
    PtrComponentList components = NULL;
    PtrWire hangingPullWire;
    unsigned combinedWidth = 0;

    if (!inputs)
    {
        *activation = NULL;
        return NULL;
    }                           /* No wires */
    if (!CDR (inputs))
    {
        *activation = CAR (inputs);
        return NULL;
    }
    /* One wire */
    /* n wires */
    *activation = NewWire (NextBundleNumber (*wires), totalWidth, finalType, 0, true /* pull */ , position);
    *wires = NewWireList (*activation, *wires);
    hangingPullWire = *activation;

    while (inputs && CDR (inputs))
    {
        if (CDDR (inputs))      /* More than 2 wires left (incl. this one) */
        {
            PtrWire newHangingPullWire = NewWire (NextBundleNumber (*wires),
              hangingPullWire->width - CAR (inputs)->width, NoType,
              0 /* offset not used here */ , true, position);

            components = NewComponentList (NewCombineComponent (position, hangingPullWire, CAR (inputs), newHangingPullWire), components);
            *wires = NewWireList (newHangingPullWire, *wires);
            hangingPullWire = newHangingPullWire;
            combinedWidth += CAR (inputs)->width;
        } else
        {                       /* Last 2 wires */
            components = NewComponentList (NewCombineComponent (position, hangingPullWire, CAR (inputs), CADR (inputs)), components);
            combinedWidth += CAR (inputs)->width + CADR (inputs)->width;
        }
        inputs = CDR (inputs);
    }
    return components;
}

/* HandleAggregateConsExpr : reduce an expression list into a record/array type constant or a wire connected to a
   Combine component.  If appendNcons is true then the expressions are treated as array fragments rather than
   record/array elements. */
ExprAttributes HandleAggregateConsExpr (PtrType actualType, bool checkArraySize, bool appendNcons, PtrExprAttributesList exprs, tPosition position)
{
    ExprAttributes ret = NoExprAttributes;
    PtrWireList combineActivations = NULL;

    /* Pending constant, if we encounter a constant add its value to the end of this MP_INT and
       bunk up pendingConstantWidth. Only generate a constant if !CDR(exprActs) or
       CAR(exprValues).value.value.baseValue == NULL */
    PtrMP_INT pendingConstant = NewMP_INT (0);
    PtrMP_INT pendingDontCares = NewMP_INT (0);
    Bits pendingConstantWidth = 0;

    /* Check length of array element list for cons exprs. */
    if (!appendNcons && actualType->nature == ArrayType)
    {
        unsigned listLength = LengthOfExprAttributesList (exprs);

        if (checkArraySize)
        {
            unsigned reqdLength = actualType->info.array.elementCount;

            if (listLength < reqdLength)
                LOG_ERROR (ExprListTooShort, NoIdent, position);
            if (listLength > reqdLength)
                LOG_ERROR (ExprListTooShort, NoIdent, position);
        } else if (actualType->info.array.elementCount != listLength)
        {
            PtrMP_INT maxElementCount = NewMP_INT (listLength - 1);

            actualType =
              NewArrayType (MarkerIdent, actualType->info.array.baseType,
              NewSpan (NewMP_INT (0), maxElementCount, NewNumericType (MarkerIdent, SmallestRangeToHoldValue (maxElementCount))));
        }
    }
    /* Find real size, make a type for actualType from left type */
    if (appendNcons && exprs && CAR (exprs).value.type->nature == ArrayType)
    {
        PtrType baseType = CAR (exprs).value.type->info.array.baseType;
        PtrExprAttributesList exprIter = CDR (exprs);
        unsigned elementCount = CAR (exprs).value.type->info.array.elementCount;
        PtrMP_INT maxElementCount;

        while (exprIter)
        {
            if (CAR (exprIter).value.type->nature == ArrayType)
                elementCount += CAR (exprIter).value.type->info.array.elementCount;
            exprIter = CDR (exprIter);
        }
        maxElementCount = NewMP_INT (elementCount - 1);

        actualType =
          NewArrayType (MarkerIdent, baseType,
          NewSpan (NewMP_INT (0), maxElementCount, NewNumericType (MarkerIdent, SmallestRangeToHoldValue (maxElementCount))));
    }
    /* Construction of record and array typed aggregates is v. similar */
    while (exprs)
    {
        if (CAR (exprs).value.value.baseValue && !CAR (exprs).value.type->hasBuiltinElements) /* Has a value */
        {                       /* Add to the pending constant */
            PtrMP_INT maskedBaseValue = FilterRangeFromMP_INT (CAR (exprs).value.value.baseValue,
              Abs (CAR (exprs).value.type->size), 0);
            PtrMP_INT maskedDontCares = (CAR (exprs).value.value.dontCares ? FilterRangeFromMP_INT (CAR (exprs).value.value.dontCares,
                Abs (CAR (exprs).value.type->size),
                0) : NULL);

            if (pendingConstantWidth) /* Silly optimisation */
            {
                mpz_mul_2exp (maskedBaseValue, maskedBaseValue, pendingConstantWidth);
                mpz_ior (pendingConstant, pendingConstant, maskedBaseValue);

                if (maskedDontCares)
                {
                    mpz_mul_2exp (maskedDontCares, maskedDontCares, pendingConstantWidth);
                    mpz_ior (pendingDontCares, pendingDontCares, maskedDontCares);
                }
            } else
            {
                mpz_set (pendingConstant, maskedBaseValue);
                if (maskedDontCares)
                    mpz_set (pendingDontCares, maskedDontCares);
            }
            pendingConstantWidth += Abs (CAR (exprs).value.type->size);
            DeleteMP_INT (maskedBaseValue);
        } else                  /* No value */
        {
            if (!CAR (exprs).activation) /* Give up here if no activation */
                return ret;
            if (pendingConstantWidth > 0)
            {
                PtrWire newWire;

                if (mpz_sgn (pendingDontCares) != 0) /* Some don't care bits */
                {
                    LOG_ERROR (CannotMixImplicantsAndNonConsts, NoIdent, position);
                    return ret;
                }
                /* Add a constant */
                newWire = NewWire (NextBundleNumber (ret.wires), pendingConstantWidth, NoType, 0, true /* Pull */ , position);
                ret.components = NewComponentList (NewConstantComponent (position, pendingConstant, newWire), ret.components);
                ret.wires = NewWireList (newWire, ret.wires);
                combineActivations = AppendWireLists (combineActivations, NewWireList (newWire, NULL));
                pendingConstantWidth = 0;
                pendingConstant = NewMP_INT (0);
                pendingDontCares = NewMP_INT (0);
            }
            /* Add this activation port */
            /* Add the components accesses .. of CAR(exprs) */
            ret.wires = AppendWireLists (ret.wires, CAR (exprs).wires);
            ret.components = AppendComponentLists (ret.components, CAR (exprs).components);
            ret.accesses = ParallelCombineAccesses (ret.accesses, CAR (exprs).accesses, &ret.components, &ret.wires, position, NULL, false);
            RenumberWireList (ret.wires, false); /* resequence wires */
            combineActivations = AppendWireLists (combineActivations, NewWireList (CAR (exprs).activation, NULL));
        }
        exprs = CDR (exprs);
    }
    /* Handle record over types */
    if (actualType->nature == RecordType && actualType->info.record.unpaddedLength != Abs (actualType->size))
    {
        pendingConstantWidth = Abs (actualType->size);
    }
    /* Either combineActivations == NULL and therefore pendingConstant/pendingConstantWidth give us a value,
       or else combineActivations contains the activation ports for a Combine (and we need to sort out the
       last part of list was a constant problem) */
    if (!combineActivations)
    {
        if (mpz_sgn (pendingDontCares) != 0) /* Some don't cares */
        {
            ret.value = NewTypedValueImplicant (NewImplicant (pendingConstant, pendingDontCares), actualType);
        } else
        {
            /* Add a constant component (just in case we needed one) */
            PtrWire newWire = NewWire (1, pendingConstantWidth, NoType, 0, true /* Pull */ ,
              position);

            ret.components = NewComponentList (NewConstantComponent (position, pendingConstant, newWire), NULL);
            ret.wires = NewWireList (newWire, NULL);
            ret.value = NewTypedValue (pendingConstant, actualType);
            ret.activation = newWire;
        }
    } else
    {
        if (pendingConstantWidth) /* An outstanding constant */
        {
            PtrWire newWire;

            if (mpz_sgn (pendingDontCares) != 0) /* Some don't care bits */
            {
                LOG_ERROR (CannotMixImplicantsAndNonConsts, NoIdent, position);
                return ret;
            }

            newWire = NewWire (NextBundleNumber (ret.wires), pendingConstantWidth, NoType, 0, true /* Pull */ , position);
            ret.components = NewComponentList (NewConstantComponent (position, pendingConstant, newWire), ret.components);
            ret.wires = NewWireList (newWire, ret.wires);
            combineActivations = AppendWireLists (combineActivations, NewWireList (newWire, NULL));
        }
        /* combine activations should now be clean */
        if (LengthOfWireList (combineActivations) == 1) /* Single element, */
        {
            ret.activation = CAR (combineActivations); /* Just connect */
        } else
        {                       /* Need a combine tree */
            ret.components =
              AppendComponentLists (CombineComponentTree
              (combineActivations, Abs (actualType->size), actualType, &ret.activation, &ret.wires, position), ret.components);
        }
        ret.value = NewTypedValue (NULL, actualType);
    }
    return ret;
}

/* HandleNamedEnumElemExpr : lookup enumeration element enumName in type typeName, raises
   errors on any problems, return NULL,NoType on error */
ExprAttributes HandleNamedEnumElemExpr (PtrContext context, tIdent typeName, tIdent enumName, tPosition position)
{
    ExprAttributes ret = NoExprAttributes;
    PtrType enumType = LookupType (context, typeName, false /* not just local */ );

    if (enumType == NoType || enumType->nature != EnumerationType)
        LOG_ERROR (ExpectingAnEnumType, NoIdent, position);
    else
    {
        PtrMP_INT enumElemValue = LookupEnumElemValue (enumType, enumName);

        if (!enumElemValue)
            LOG_ERROR (NotAnEnumElem, enumName, position);
        else
            ret = MakeConstantExprAttributes (enumElemValue, enumType, position);
    }
    return ret;
}

/* HandleRecordElemExtractExpr : extract the named element value from the expression expr
   if exprValue has no type then moan and return NULL, NoType in value */
ExprAttributes HandleRecordElemExtractExpr (PtrContext context, tIdent elemName, ExprAttributes expr, tPosition position)
{
    ExprAttributes ret = NoExprAttributes;

    if (expr.value.type == NoType)
        LOG_ERROR (TypeIsNotValid, NoIdent, position);
    if (expr.value.type->nature != RecordType)
        LOG_ERROR (ExpectingARecordType, NoIdent, position);
    else
    {
        PtrInstance recordElem = LookupInstanceInInstanceList (expr.value.type->info.record.recordElements, elemName,
          false /* global */ );
        SignedBits elementSize;
        Bits elementOffset;

        if (!recordElem)
            LOG_ERROR (NotARecordElem, elemName, position);
        else
        {
            elementOffset = recordElem->info.recordElem.offset;
            elementSize = recordElem->type->size;

            if (expr.value.value.baseValue) /* Is constant */
            {
                PtrMP_INT filtered = FilterRangeFromMP_INT (expr.value.value.baseValue,
                  Abs (elementSize), elementOffset);
                PtrMP_INT filteredDontCares = (expr.value.value.dontCares ? FilterRangeFromMP_INT (expr.value.value.dontCares,
                    Abs (elementSize),
                    elementOffset) : NULL);

                if (elementSize < 0) /* Sex the result if the target is signed */
                    RecoverSign (filtered, Abs (elementSize));

                /* Have some don't cares? */
                if (filteredDontCares && mpz_sgn (filteredDontCares) != 0)
                {
                    ret.value = NewTypedValueImplicant (NewImplicant (filtered, filteredDontCares), recordElem->type);
                } else
                {
                    /* Using the expr return comps/wires ... */
                    PtrWire newWire = NewWire (NextBundleNumber (expr.wires), Abs (elementSize),
                      recordElem->type, 0,
                      true /* Pull from constant */ , position);

                    ret.components = NewComponentList (NewConstantComponent (position, filtered, newWire), ret.components);
                    ret.wires = NewWireList (newWire, expr.wires);
                    ret.activation = newWire;
                    ret.accesses = expr.accesses;
                    ret.value = NewTypedValue (filtered, recordElem->type);
                }

                return ret;
            } else
                /* Non constant */
            {
                PtrAccess readConnection;

                /* Skip if we have no expr activation */
                if (expr.activation)
                {
                    /* was the result of expr connected to a variable? */
                    if ((readConnection = LookupVarAccess (expr.accesses, expr.activation)))
                    {
                        /* Update the wire, update the access readMap */
                        expr.activation->offset += elementOffset;
                        expr.activation->width = Abs (elementSize);
                        expr.activation->position = position;
                        expr.activation->type.type = recordElem->type;
                        UpdateVarAccessMap (readConnection, true /* readMap */ );

						/* AB 2008-08-11 */
						expr.activation->ident = MakeIdent1 (g_strdup_printf ("%s.%s",
							PeekString (expr.activation->ident), PeekString (elemName)));

                        ret.components = expr.components;
                        ret.wires = expr.wires;
                        ret.activation = expr.activation;
                    } else
                    {
                        PtrWire newWire = NewWire (NextBundleNumber (expr.wires),
                          Abs (elementSize),
                          recordElem->type, elementOffset,
                          true /* Pull */ , position);

                        ret.components = NewComponentList (NewSliceComponent (position, elementOffset, newWire, expr.activation), expr.components);
                        ret.wires = NewWireList (newWire, expr.wires);
                        ret.activation = newWire;
                    }
                }
                ret.accesses = expr.accesses;
                ret.value = NewTypedValue (NULL, recordElem->type);
                return ret;
            }
        }
    }
    return ret;
}

/* HandleArrayExtractExpr : handle extraction of an element from a array variable access */
ExprAttributes HandleArrayExtractExpr (PtrContext context, ExprAttributes array, ExprAttributes subscript, tPosition position)
{
    ExprAttributes ret = NoExprAttributes;

    if (array.value.type->nature == ArrayedType) /* channel and it's arrayed! */
    {
        if (subscript.value.type->nature != NumericType && subscript.value.type->nature != EnumerationType)
            LOG_ERROR (SubscriptMustBeNumericOrEnum, NoIdent, position);
        else if (!SubscriptTypeIsOK (array.value.type->info.arrayed.range.boundingType, subscript.value.type))
            LOG_ERROR (SubscriptHasWrongType, NoIdent, position);
        else if (subscript.value.value.baseValue)
        {
            Span range = array.value.type->info.arrayed.range;

            if (subscript.value.value.dontCares)
            {
                LOG_ERROR (SubscriptCannotBeImplicant, NoIdent, position);
                return NoExprAttributes;
            }

            if (mpz_cmp (subscript.value.value.baseValue, range.lowerBound) < 0 || mpz_cmp (subscript.value.value.baseValue, range.upperBound) > 0)
                LOG_ERROR (SubscriptOutOfRange, NoIdent, position);
            else
            {
                /* Make a `name[index]' ident */
                tIdent ident = CAR (array.accesses)->inst->ident;
                tIdent identLength = strlen (PeekString (ident));
                tIdent numberedIdent;
                PtrMP_INT index = subscript.value.value.baseValue;
                unsigned nameLength = identLength + 4 /* [ ] */  + mpz_sizeinbase (index, 10);
                char *newIdent = NEW_ARRAY (char, nameLength);

                sprintf (newIdent, "%s[", PeekString (ident));
                mpz_get_str (newIdent + identLength + 1, 10, index);
                strcat (newIdent, "]");

                numberedIdent = MakeIdent1 (newIdent);
                FREE_ARRAY (char, nameLength, newIdent);

                {
                    PtrInstance inst = LookupInstance (context, numberedIdent, false);

                    if (!inst || inst->nature != VariableInstance)
                        LOG_ERROR (ExpectingAReadableChannel, NoIdent, position); /* The right error? */
                    else
                    {
                        /* Create a new wire for / access on variable read */
                        PtrWire newWire = NewWire (1,
                          Abs (inst->type->size),
                          inst->type, 0,
                          true /* Pull read */ ,
                          position);

                        newWire->ident = inst->ident;

                        ret.wires = NewWireList (newWire, NULL);
                        ret.value.type = inst->type;
                        ret.accesses = NewAccessList (NewVariableAccess (inst, newWire), NULL);
                        ret.activation = newWire;
                    }
                }
            }
        } else
            LOG_ERROR (CantDoNonConstArrayedChannelIndex, NoIdent, position);
    } else if (array.accesses &&
      (CAR (array.accesses)->inst->nature == ChannelInstance ||
        CAR (array.accesses)->inst->nature == InputChannelInstance || CAR (array.accesses)->inst->nature == OutputChannelInstance))
        LOG_ERROR (ChannelsCantBeUsedInExpr, CAR (array.accesses)->inst->ident, position);
    else if (array.value.type->nature != ArrayType)
        LOG_ERROR (ExpectingAnArrayType, NoIdent, position);
    else if (subscript.value.type->nature != NumericType && subscript.value.type->nature != EnumerationType)
        LOG_ERROR (SubscriptMustBeNumericOrEnum, NoIdent, position);
    else if (!SubscriptTypeIsOK (array.value.type->info.array.range.boundingType, subscript.value.type))
        LOG_ERROR (SubscriptHasWrongType, NoIdent, position); /* FIXME, more info */
    else
    {                           /* Right subscript type, right array */
        if (subscript.value.value.baseValue) /* Constant subscript */
        {
            ret =
              HandleArraySliceExpr (context, array,
              NewSpan (subscript.value.value.baseValue, subscript.value.value.baseValue, subscript.value.type), true
              /* single element return type ie: a[5] for a : array 10 of 8 bits will have type 8 bits not array 1 of 8 bits */
              , position);
        } else
        {                       /* Non-const, mucho hardware */
            /* Check for compliance of index and subscript */
            PtrAccess arrayAccess;
            PtrMP_INT indexIter = NewMP_INT (0);
            PtrWireList inputWires = NULL;
            unsigned elementLength = Abs (array.value.type->info.array.baseType->size);
            PtrWire outWire = NewWire (1 /* fix later */ , elementLength,
              array.value.type->info.array.baseType, 0,
              true /* pull */ , position);
            PtrMP_INT lowSubscript, highSubscript;
            PtrMP_INT lowArraySubscript, highArraySubscript;
            PtrMP_INT lowElementNumber, highElementNumber;
            PtrMP_INT rawLowElementNumber, rawHighElementNumber;

            lowSubscript = MinimumValueOfRange (subscript.value.type->size);
            highSubscript = MaximumValueOfRange (subscript.value.type->size);
            lowArraySubscript = array.value.type->info.array.range.lowerBound;
            highArraySubscript = array.value.type->info.array.range.upperBound;

            /* Which is the lowest numbered element that can be extracted ? */
            rawLowElementNumber = MaxMP_INT (lowSubscript, lowArraySubscript);
            /* Last index number */
            rawHighElementNumber = MinMP_INT (highSubscript, highArraySubscript);

            if (mpz_cmp (rawLowElementNumber, rawHighElementNumber) > 0)
            {
                LOG_ERROR (SubscriptOutOfRange, NoIdent, position);
                return NoExprAttributes;
            }

            /* Normalise indices to 0 */
            lowElementNumber = CopyMP_INT (rawLowElementNumber);
            mpz_sub (lowElementNumber, lowElementNumber, lowArraySubscript);
            highElementNumber = CopyMP_INT (rawHighElementNumber);
            mpz_sub (highElementNumber, highElementNumber, lowArraySubscript);

            /* Start with the subscript components, wires and accesses */
            ret = subscript;
            ret.wires = NewWireList (outWire, ret.wires);
            ret.activation = outWire;
            ret.value = NewTypedValue (NULL, array.value.type->info.array.baseType);

            /* Reset indexIter */
            mpz_set (indexIter, lowElementNumber);
            /* Add components/accesses and wires to implement choice */
            if (array.value.value.baseValue) /* Constant array eg. X{1,2,4,9}[n] */
            {
                unsigned offset = elementLength * mpz_get_ui (lowElementNumber);

                /* lowerBound .. upperBound */
                for (; mpz_cmp (indexIter, highElementNumber) <= 0; mpz_add_ui (indexIter, indexIter, 1))
                {
                    /* Filter out the appropriate value */
                    PtrMP_INT filtered = FilterRangeFromMP_INT (array.value.value.baseValue,
                      elementLength, offset);

                    inputWires = NewWireList (NewWire (1 /* fix later */ , elementLength,
                        array.value.type->info.array.baseType, offset, true /* read var, pull */ ,
                        position), inputWires);
                    ret.components = NewComponentList (NewConstantComponent (position, filtered, CAR (inputWires)), ret.components);
                    offset += elementLength;
                }
            }
            /* array.activation is connected to the access to a variable eg. a[b] */
            else if ((arrayAccess = LookupVarAccess (array.accesses, array.activation)))
            {
                /* FIXME, this won't work on arbitrary array types, only for
                   variable reads ie:  {a,b,c}[d] won't work */
                PtrAccess inputVariableAccess = NULL;
                unsigned offset = elementLength * mpz_get_ui (lowElementNumber);

                /* lowerBound .. upperBound */
                for (; mpz_cmp (indexIter, highElementNumber) <= 0; mpz_add_ui (indexIter, indexIter, 1))
                {
                    inputWires = NewWireList (NewWire (1 /* fix later */ , elementLength,
                        array.value.type->info.array.baseType, offset, true /* read var, pull */ ,
                        position), inputWires);
                    if (!inputVariableAccess)
                        inputVariableAccess = NewVariableAccess (arrayAccess->inst, CAR (inputWires));
                    else
                    {
                        inputVariableAccess =
                          CombineVariables (NewVariableAccess
                          (arrayAccess->inst, CAR (inputWires)), inputVariableAccess, position, true /* || */ , false);
                    }
                    offset += elementLength;
                }
                /* inputVariableAccess will replace arrayAccess as the variable access */
                ret.accesses =
                  ParallelCombineAccesses (NewAccessList
                  (inputVariableAccess, NULL), ret.accesses, &ret.components, &ret.wires, position, NULL, false);
            } else
                /* Filtered expression choice eg. X{a,b,c}[n] where all of a b c and n are expressions */
            {
                /* FIXME */
                LOG_ERROR (CannotDoExprArrayExtract, NoIdent, position);
                return ret;
            }
            ret.wires = AppendWireLists (CopyWireList (inputWires), ret.wires);
            ret.components = NewComponentList (NewCaseFetchComponent (position, 
                /* FIXME, need to restrict this to just the range asked for */
                MakeCaseSpecificationAcrossSpan
                (NewSpan
                  (rawLowElementNumber,
                    rawHighElementNumber, NoType), WireWidth (subscript.activation)), outWire, subscript.activation, inputWires), ret.components);
            /* Do we need to renumber here? */
            RenumberWireList (ret.wires, false);
        }
    }
    return ret;
}

/* HandleArraySliceExpr : handle extraction of a constant slice from a array variable access */
ExprAttributes HandleArraySliceExpr (PtrContext context, ExprAttributes array, Span range, bool singleElement, tPosition position)
{
    ExprAttributes ret = NoExprAttributes;

    if (range.boundingType == NoType)
        return ret;             /* Fail silently */
    else if (array.value.type->nature != ArrayType)
        LOG_ERROR (ExpectingAnArrayType, NoIdent, position);
    /* For enum's, must be the same type, for numeric types range must be <= size of array boundingType */
    else if (!SubscriptTypeIsOK (array.value.type->info.array.range.boundingType, range.boundingType))
        LOG_ERROR (SubscriptHasWrongType, NoIdent, position); /* FIXME, more info */
    else
    {                           /* Right range type, right array */
        /* Field extract city */
        if (mpz_cmp
          (range.lowerBound,
            array.value.type->info.array.range.lowerBound) < 0 || mpz_cmp (range.upperBound, array.value.type->info.array.range.upperBound) > 0)
            LOG_ERROR (SubscriptOutOfRange, NoIdent, position);
        /* Constant / Variable extract */
        else
        {                       /* Work out mask Length/Offset */
            PtrMP_INT tmp = NewMP_INT (0);

            /* elementLength hold the length of a single element, elementLength * elementCount
               is the length of the entire span */
            unsigned elementLength = Abs (array.value.type->info.array.baseType->size);
            unsigned elementOffset, elementCount;

            /* Calculate number of elements and offset of first element */
            mpz_sub (tmp, range.lowerBound, array.value.type->info.array.range.lowerBound);
            elementOffset = mpz_get_ui (tmp) * elementLength;
            mpz_sub (tmp, range.upperBound, range.lowerBound);
            elementCount = mpz_get_ui (tmp) + 1;

            DeleteMP_INT (tmp);

            if (array.value.value.baseValue)
            {                   /* Constants */
                ret = array;
                ret.value.value.baseValue = FilterRangeFromMP_INT (array.value.value.baseValue, elementLength * elementCount, elementOffset);

                if (array.value.value.dontCares)
                {
                    ret.value.value.dontCares = FilterRangeFromMP_INT (array.value.value.dontCares, elementLength * elementCount, elementOffset);
                    if (mpz_sgn (ret.value.value.dontCares) == 0)
                        ret.value.value.dontCares = NULL;
                } else
                    ret.value.value.dontCares = NULL;

                /* Interesting approach, check this some time FIXME */
                if (array.activation)
                    CAR (ret.components)->param.constant.value = ret.value.value.baseValue;
            } else
            {                   /* Variables */
                PtrAccess readConnection = LookupVarAccess (array.accesses, array.activation);

                /* was the result of expr connected to a variable? */
                if (readConnection)
                {
                    /* Just fiddle with the existing activation (from array) */
                    ret = array;
                    ret.activation->width = elementLength * elementCount;
                    ret.activation->offset += elementOffset;
                    UpdateVarAccessMap (readConnection, true /* readMap */ );
                } else
                    /* Need to use a mask */
                {
                    PtrWire newWire = NewWire (NextBundleNumber (array.wires),
                      elementLength * elementCount,
                      NoType, elementOffset,
                      true /* Pull */ , position);

                    ret.components = NewComponentList (NewSliceComponent (position, elementOffset, newWire, array.activation), array.components);
                    ret.wires = NewWireList (newWire, array.wires);
                    ret.activation = newWire;
                }
				/* AB 2008-08-11 */
				ret.activation->ident = MakeIdent1 ((singleElement
					?  g_strdup_printf ("%s[%lu]", PeekString (array.activation->ident), mpz_get_ui (range.lowerBound))
					:  g_strdup_printf ("%s[%lu..%lu]", PeekString (array.activation->ident),
						mpz_get_ui (range.lowerBound), mpz_get_ui (range.upperBound))));

                ret.accesses = array.accesses;
                ret.value.value.baseValue = NULL;
            }
            if (singleElement)
                ret.value.type = array.value.type->info.array.baseType;
            else
                ret.value.type = NewArrayType (NoIdent, array.value.type->info.array.baseType, range);
            if (ret.activation)
                ret.activation->type.type = ret.value.type;
        }
    }
    return ret;
}

/* HandleAsExpr : handle explicit casts, if expr has a value (constant) then generate a new
   constant components otherwise place the appropriate adapter */
ExprAttributes HandleAsExpr (PtrType asType, ExprAttributes expr, tPosition position)
{
    ExprAttributes ret = NoExprAttributes;

    if (asType == NoType)
        LOG_ERROR (InvalidAsTarget, NoIdent, position);
    else if (expr.value.type == NoType)
        LOG_ERROR (TypeIsNotValid, NoIdent, position);
    else if (expr.value.type->hasBuiltinElements || asType->hasBuiltinElements)
        LOG_ERROR (CannotCastBuiltinType, NoIdent, position);
    else
    {
        /* AB 2008-03-10, sign extend signed numeric to numeric casts.  All others are clipped to
           	Abs (expr.value.type->size) */
        bool targetSigned = asType->size < 0;
    	bool signExtend = expr.value.type->size < 0 && asType->nature == NumericType;

        ret = expr;             /* Passing on expr? */
        ret.value.type = asType;

        if (expr.value.value.baseValue) /* Constant */
        {
            /* ret.value.value.baseValue = FilterRangeFromMP_INT (expr.value.value.baseValue, Abs (asType->size), 0); */

            if (expr.value.value.dontCares)
            {
                PtrMP_INT filteredDontCares = FilterRangeFromMP_INT (expr.value.value.dontCares,
                  Abs (asType->size), 0);

                if (mpz_sgn (filteredDontCares) == 0)
                    ret.value.value.dontCares = NULL;
                else
                    ret.value.value.dontCares = filteredDontCares;
            }

            if (signExtend)
            {
                RecoverSign (ret.value.value.baseValue, Abs (expr.value.type->size));
                /* If the target type isn't signed then only extend as far as Abs (asType->size) */
                if (! targetSigned)
            		ret.value.value.baseValue = FilterRangeFromMP_INT (expr.value.value.baseValue, Abs (asType->size), 0);

                if (ret.value.value.dontCares)
                {
                    LOG_ERROR (SignedImplicantsNotAllowed, NoIdent, position);
                    return NoExprAttributes;
                }
            } else {
            	/* AB 2008-03-10, this may not be strictly necessary but being cautious here as I can't remember if
            		expr.values can be negative */
            	ret.value.value.baseValue = FilterRangeFromMP_INT (expr.value.value.baseValue, Abs (expr.value.type->size), 0);
            }
        } else
            ret.value.value.baseValue = NULL;

        /* Place the target components if type sizes are different */
        if (Abs (expr.value.type->size) != Abs (asType->size))
        {
            if (expr.value.value.baseValue) /* Constant */
            {
                /* The value should convey sign (in an MP_INT) */
                PtrWire newWire = NewWire (1, Abs (asType->size), asType, 0, true /* Pull */ ,
                  position);

                ret.components = NewComponentList (NewConstantComponent (position, expr.value.value.baseValue, newWire), NULL);
                ret.accesses = NULL;
                ret.activation = newWire;
                ret.wires = NewWireList (newWire, NULL);
            } else
                /* Non constant */
            {
                PtrWire newWire = NewWire (NextBundleNumber (expr.wires),
                  Abs (asType->size), asType, 0,
                  true /* Pull */ , position);

                ret.components = NewComponentList (NewAdaptComponent (position, signExtend, signExtend,
                    newWire,    /* Target wire */
                    expr.activation /* Source wire */
                  ), expr.components);
                ret.activation = newWire;
                ret.wires = NewWireList (newWire, expr.wires);
            }
        }
        else { // source and dest type sizes are equal, but make sure they have the same nature
            if (expr.activation->type.type->nature != asType->nature)
                expr.activation->type.type = asType;
        }
    }
    return ret;
}

/* HandleBitArrayCastExpr : handle to-bit-array casts, if expr has a value (constant) which is
   signed then also truncate this value to the type width */
ExprAttributes HandleBitArrayCastExpr (ExprAttributes expr, tPosition position)
{
    ExprAttributes ret = NoExprAttributes;

    if (expr.value.type == NoType)
        LOG_ERROR (TypeIsNotValid, NoIdent, position);
    else if (expr.value.type->hasBuiltinElements)
        LOG_ERROR (CannotCastBuiltinType, NoIdent, position);
    else
    {
        Bits typeWidth = Abs (expr.value.type->size);
        PtrMP_INT typeWidthMinus1 = NewMP_INT (typeWidth - 1);

        ret = expr;
        ret.value.type = NewArrayType (MarkerIdent, BitTypeObj,
          NewSpan (NewMP_INT (0), typeWidthMinus1, NewNumericType (MarkerIdent, SmallestRangeToHoldValue (typeWidthMinus1))));

        if (expr.value.value.baseValue) /* Truncate a constant */
            ret.value.value.baseValue = FilterRangeFromMP_INT (expr.value.value.baseValue, typeWidth, 0);
    }
    return ret;
}

/* CalculateUnaryOpValue : calculate the value and type of unary operation op applied to value val,
   (iff val.value.baseValue) otherwise returns just the type, returns NoValue on error */
TypedValue CalculateUnaryOpValue (Operators op, TypedValue val, tPosition position)
{
    TypedValue ret = NoValue;

    if (val.type->nature != NumericType)
        LOG_ERROR (ExpressionMustBeNumeric, NoIdent, position);
    else
    {
        ret.type = val.type;    /* Fallback */

        if (val.value.baseValue) /* Constant */
        {
            ret.value.baseValue = NewMP_INT (0);
            switch (op)
            {
            case NegateOperator:
                mpz_neg (ret.value.baseValue, val.value.baseValue);
                /* NB. the type determination is slightly different here */
                ret.type = NewNumericType (MarkerIdent, SmallestRangeToHoldValue (ret.value.baseValue));
                break;
            case InvertOperator:
                {
                    PtrMP_INT filteredResult;

                    mpz_com (ret.value.baseValue, val.value.baseValue);
                    filteredResult = FilterRangeFromMP_INT (ret.value.baseValue, Abs (val.type->size), 0);
                    /* Sign extend for a signed type */
                    if (val.type->size < 0)
                        RecoverSign (filteredResult, Abs (val.type->size));
                    DeleteMP_INT (ret.value.baseValue);
                    ret.value.baseValue = filteredResult;
                }
                break;
            case LogOperator:
                if (!val.value.baseValue)
                {
                    LOG_ERROR (ExpressionMustBeConstant, NoIdent, position);
                    ret.type = NoType;
                } else if (mpz_sgn (val.value.baseValue) == 0) /* log 0 is invalid */
                {
                    LOG_ERROR (CannotFindLogZero, NoIdent, position);
                    ret.type = NoType;
                } else if (mpz_cmp_ui (val.value.baseValue, 1) == 0) /* log 1 == 0 */
                {
                    ret.value.baseValue = NewMP_INT (0);
                    ret.type = BitTypeObj;
                } else
                {
                    ret.value.baseValue = CopyMP_INT (val.value.baseValue);
                    /* sizeinbase (|val| - 1) */
                    mpz_abs (ret.value.baseValue, ret.value.baseValue);
                    mpz_sub_ui (ret.value.baseValue, ret.value.baseValue, 1);
                    ret.value.baseValue = NewMP_INT (mpz_sizeinbase (ret.value.baseValue, 2));
                    ret.type = NewNumericType (MarkerIdent, SmallestRangeToHoldValue (ret.value.baseValue));
                }
                break;
            default:
                break;
            }
        } else if (op == NegateOperator && val.type->size > 0) /* Negate an unsigned non constant */
            /* unsigned n bits -> signed n+1 bits */
            ret.type = NewNumericType (MarkerIdent, -(val.type->size + 1));
    }
    return ret;
}

/* EvaluateNumericConstantBinaryOps : handle the numeric x numeric operators on constants */
TypedValue EvaluateNumericConstantBinaryOps (Operators op, PtrMP_INT left, PtrMP_INT right, tPosition position)
{
    TypedValue val = NoValue;

    val.value.baseValue = NewMP_INT (0);

    switch (op)
    {
    case AddOperator:
        mpz_add (val.value.baseValue, left, right);
        break;
    case SubtractOperator:
        mpz_sub (val.value.baseValue, left, right);
        break;
    case MultiplyOperator:
        mpz_mul (val.value.baseValue, left, right);
        break;
    case DivideOperator:
        mpz_div (val.value.baseValue, left, right);
        break;
    case ModuloOperator:
        mpz_mod (val.value.baseValue, left, right);
        break;
    case PowerOperator:
        if (mpz_sgn (right) == -1) /* Don't allow n^-m */
        {
            LOG_ERROR (PowerMustHaveNonNegativePowerArg, NoIdent, position);
            val.value.baseValue = NewMP_INT (0);
        } else
        {
            mpz_pow_ui (val.value.baseValue, left, mpz_get_ui (right));
        }
        break;
    default:
        break;
    }
    val.type = NewNumericType (MarkerIdent, SmallestRangeToHoldValue (val.value.baseValue));
    return val;
}

/* EvaluateEqualityOps : test for equality, not equals */
TypedValue EvaluateEqualityOps (Operators op, PtrMP_INT left, PtrMP_INT right)
{
    TypedValue val = NoValue;

    val.type = NewNumericType (MarkerIdent, 1);

    switch (op)
    {
    case EqualOperator:
        val.value.baseValue = NewMP_INT (mpz_cmp (left, right) == 0);
        break;
    case NotEqualOperator:
        val.value.baseValue = NewMP_INT (mpz_cmp (left, right) != 0);
        break;
    default:
        break;
    }
    return val;
}

/* EvaluateEqualityOps : handle constant ops on the other inequality operators */
TypedValue EvaluateInequalityOps (Operators op, PtrMP_INT left, PtrMP_INT right)
{
    TypedValue val = NoValue;

    val.type = BitTypeObj;      /* Usual type */

    switch (op)
    {
    case LTOperator:
        val.value.baseValue = NewMP_INT (mpz_cmp (left, right) < 0);
        break;
    case GTOperator:
        val.value.baseValue = NewMP_INT (mpz_cmp (left, right) > 0);
        break;
    case LEOperator:
        val.value.baseValue = NewMP_INT (mpz_cmp (left, right) <= 0);
        break;
    case GEOperator:
        val.value.baseValue = NewMP_INT (mpz_cmp (left, right) >= 0);
        break;
    case AndOperator:
        val.value.baseValue = NewMP_INT (0);
        mpz_and (val.value.baseValue, left, right);
        val.type = NewNumericType (MarkerIdent, SmallestRangeToHoldValue (val.value.baseValue));
        break;
    case OrOperator:
        val.value.baseValue = NewMP_INT (0);
        mpz_ior (val.value.baseValue, left, right);
        val.type = NewNumericType (MarkerIdent, SmallestRangeToHoldValue (val.value.baseValue));
        break;
    case XorOperator:
        val.value.baseValue = NewMP_INT (0);
        mpz_eor (val.value.baseValue, left, right);
        val.type = NewNumericType (MarkerIdent, SmallestRangeToHoldValue (val.value.baseValue));
        break;
    default:
        break;
    }
    return val;
}

/* TypeNumericBinaryOps : calculate type of numeric op on non constant */
PtrType TypeNumericBinaryOps (Operators op, TypedValue left, TypedValue right)
{
    PtrType type = NoType;

    switch (op)
    {
    case AddOperator:
    case SubtractOperator:
        {
            /* width1 is the width and sign1 is the signedness of the widest type */
            Bits width1, width2;
            bool sign1, sign2;
            Bits width;
            bool sign;

            if (Abs (left.type->size) >= Abs (right.type->size))
            {
                width1 = Abs (left.type->size);
                sign1 = left.type->size < 0;
                width2 = Abs (right.type->size);
                sign2 = right.type->size < 0;
            } else
            {
                width2 = Abs (left.type->size);
                sign2 = left.type->size < 0;
                width1 = Abs (right.type->size);
                sign1 = right.type->size < 0;
            }

            sign = sign1 || sign2;

            /* One signed one not (One to extend signed range, one for the new sign bit)
               when unsigned is larger or equal to signed */
            if (sign1 != sign2 && ((!sign1 && width1 >= width2) || (!sign2 && width1 == width2)))
                width = width1 + 2;
            /* Otherwise just need one extra bit */
            else
                width = width1 + 1;

            type = NewNumericType (MarkerIdent, sign ? -width : width);
            break;
        }
    case MultiplyOperator:
    case DivideOperator:
    case ModuloOperator:
    case PowerOperator:
        /* Cannot compile these operators */
        type = NoType;
        break;
    case AndOperator:
    case OrOperator:
    case XorOperator:
        {
            Bits width = Max (Abs (left.type->size), Abs (right.type->size));

            /* Use widest width of left and right types, result is signed only if both
               left and right hand sides are signed */
            type = NewNumericType (MarkerIdent, left.type->size < 0 && right.type->size < 0 ? -width : width);
        }
        break;
    default:
        break;
    }
    return type;
}

/* CalculateBinaryOpValue : calculate type (and value for left=right=constant) of result
   of binary operation, make no assumptions about left/right types other than
   that they are valid */
TypedValue CalculateBinaryOpValue (Operators op, TypedValue left, TypedValue right, tPosition position)
{
    /* Start with no valid type or value */
    TypedValue val = NoValue;

    /* Invalid types */
    if (left.type == NoType || right.type == NoType)
        LOG_ERROR (TypeIsNotValid, NoIdent, position);
    /* Disallow string operations */
    else if (left.type->hasBuiltinElements && right.type->hasBuiltinElements)
        LOG_ERROR (CannotOperateOnBuiltinType, NoIdent, position);
    /* Check first that left and right hand sides match in nature */
    else if (left.type->nature != right.type->nature)
        LOG_ERROR (IncompatibleTypeNatures, NoIdent, position);
    /* Numeric left and right */
    else if (left.type->nature == NumericType)
    {
        /* Left and right HAVE numeric values, calculate type based on function of values */
        if (left.value.baseValue && right.value.baseValue)
        {
            /* Constants */
            switch (op)
            {
            case AddOperator:
            case SubtractOperator:
            case MultiplyOperator:
            case DivideOperator:
            case ModuloOperator:
            case PowerOperator:
                val = EvaluateNumericConstantBinaryOps (op, left.value.baseValue, right.value.baseValue, position);
                break;
            case EqualOperator:
            case NotEqualOperator:
                val = EvaluateEqualityOps (op, left.value.baseValue, right.value.baseValue);
                break;
            case LTOperator:
            case LEOperator:
            case GTOperator:
            case GEOperator:
            case AndOperator:
            case OrOperator:
            case XorOperator:
                val = EvaluateInequalityOps (op, left.value.baseValue, right.value.baseValue);
                break;
            default:
                break;
            }
        } else
        {
            switch (op)
            {
            case AddOperator:
            case SubtractOperator:
            case MultiplyOperator:
            case DivideOperator:
            case ModuloOperator:
            case AndOperator:
            case OrOperator:
            case XorOperator:
            case PowerOperator:
                val.type = TypeNumericBinaryOps (op, left, right);
                if (val.type == NoType)
                    LOG_ERROR (CannotSynthesiseOperator, MakeIdent1 (OperatorSymbols[op]), position);
                break;
            case EqualOperator:
            case NotEqualOperator:
            case LTOperator:
            case LEOperator:
            case GTOperator:
            case GEOperator:
                val.type = BitTypeObj;
                break;
            default:
                break;
            }
        }
    }
    /* Enumeration left and right */
    else if (left.type->nature == EnumerationType)
    {
        /* Left and right must have the same type */
        if (left.type != right.type)
            LOG_ERROR (ExpressionsMustHaveSameType, NoIdent, position);
        /* Check operator first - enumerations can only have relational operators applied to them */
        else
            switch (op)
            {
            case EqualOperator:
            case NotEqualOperator:
            case LTOperator:
            case LEOperator:
            case GTOperator:
            case GEOperator:
                if (left.value.baseValue && right.value.baseValue) /* Constant expression */
                {
                    if (op == EqualOperator || op == NotEqualOperator)
                        val = EvaluateEqualityOps (op, left.value.baseValue, right.value.baseValue);
                    else
                        val = EvaluateInequalityOps (op, left.value.baseValue, right.value.baseValue);
                } else
                    val.type = BitTypeObj;
                break;
            default:           /* Other ops */
                LOG_ERROR (CannotApplyOperatorToEnum, MakeIdent1 (OperatorSymbols[op]), position);
                break;
            }
    }
    /* Record or Array left and right */
    else if (left.type->nature == RecordType || left.type->nature == ArrayType)
    {
        /* Left and right must have equivalent types */
        if (!TypeEquivalence (left.type, right.type))
            LOG_ERROR (ExpressionsMustHaveSameType, NoIdent, position);
        /* Check operator first - records and arrays can only have = and != applied to them */
        else
            switch (op)
            {
            case EqualOperator:
            case NotEqualOperator:
                /* Constant expression */
                if (left.value.baseValue && right.value.baseValue)
                    val = EvaluateEqualityOps (op, left.value.baseValue, right.value.baseValue);
                else
                    val.type = BitTypeObj;
                break;
            default:           /* Other ops */
                LOG_ERROR ((left.type->nature == RecordType
                    ? CannotApplyOperatorToRecord : CannotApplyOperatorToArray), MakeIdent1 (OperatorSymbols[op]), position);
                break;
            }

    } else
        LOG_ERROR (TypeIsNotValid, NoIdent, position);
    return val;
}

/* Handle{Un,Bin}aryExpr : handle the unary and binary operators, doing constant folding and
   Func component placement */
ExprAttributes HandleUnaryExpr (PtrContext context, Operators operation, ExprAttributes expr, tPosition position)
{
    ExprAttributes ret = NoExprAttributes;

    if (!TypedValueIsDCFree (expr.value, position))
    {
    } else
    {
        ret.value = CalculateUnaryOpValue (operation, expr.value, position);

        if (ret.value.type != NoType) /* No errors in UnaryOp calc. */
        {
            if (expr.value.value.baseValue) /* Expression a constant result */
            {
                if (ret.value.value.baseValue) /* made a constant result OK */
                {
                    PtrWire newWire = NewWire (1, Abs (ret.value.type->size), ret.value.type,
                      0, true /* Pull */ , position);

                    ret.wires = NewWireList (newWire, NULL);
                    ret.components = NewComponentList (NewConstantComponent (position, ret.value.value.baseValue, newWire), NULL);
                    ret.activation = newWire;
                }
            } else
            {                   /* Non constant */
                PtrWire newWire = NewWire (NextBundleNumber (expr.wires),
                  Abs (ret.value.type->size),
                  ret.value.type, 0, true /* Pull */ , position);

                ret.wires = NewWireList (newWire, expr.wires);
                ret.accesses = expr.accesses;
                ret.components = NewComponentList (NewUnaryFuncComponent (position, operation, expr.value.type->size < 0 /* signed? */ ,
                    newWire, expr.activation), expr.components);
                ret.activation = newWire;
            }
        }
    }
    return ret;
}

ExprAttributes HandleBinaryExpr (PtrContext context, Operators operation, ExprAttributes left, ExprAttributes right, tPosition position)
{
    ExprAttributes ret = NoExprAttributes;

    /* Treat + as string append for strings */
    if (left.value.type == StringTypeObj || right.value.type == StringTypeObj)
    {
        if (left.value.type != StringTypeObj || right.value.type != StringTypeObj)
            LOG_ERROR (BothStringAppendArgsMustBeStrings, NoIdent, position);
        else
        {
            /* Constant string append */
            if (left.value.value.baseValue && right.value.value.baseValue)
            {
                char *leftString = GetBuiltinValuePointer (left.value.value.baseValue);
                char *rightString = GetBuiltinValuePointer (right.value.value.baseValue);
                char *concatString = g_strconcat (leftString, rightString, NULL);

                ret = HandleStringExpr (concatString, position);
            } else
            {
                ret = HandleFunctionCallExpr (NewExprAttributesList (left, NewExprAttributesList (right, NULL)), StringAppendFunction, position);
            }
        }
    } else if (TypedValueIsDCFree (left.value, position) && TypedValueIsDCFree (right.value, position))
    {
        bool leftIsConstant = left.value.value.baseValue != NULL;
        bool rightIsConstant = right.value.value.baseValue != NULL;
        bool someConstant = leftIsConstant || rightIsConstant;
        bool placeComponents = true; /* set to false to force placing of BinaryFunc component */

        ret.value = CalculateBinaryOpValue (operation, left.value, right.value, position);
        if (ret.value.type == NoType)
            return ret;         /* CalculateBinaryOpValue returned an error */

        if (leftIsConstant && rightIsConstant) /* Constant therefore result should be */
        {
            PtrWire newWire = NewWire (1, Abs (ret.value.type->size), ret.value.type, 0,
              true /* Pull */ , position);

            ret.wires = NewWireList (newWire, NULL);
            ret.components = NewComponentList (NewConstantComponent (position, ret.value.value.baseValue, newWire), NULL);
            ret.activation = newWire;
            placeComponents = false;
        } else if (someConstant)
        {
            ExprAttributes constAttr = (leftIsConstant ? left : right);
            ExprAttributes nonConstAttr = (leftIsConstant ? right : left);

            if (Abs (left.value.type->size) == 1 && Abs (right.value.type->size) == 1)
            {                   /* One bit operations, FIXME could make this more general */
                bool constIsZero = mpz_sgn (constAttr.value.value.baseValue) == 0;

                switch (operation)
                {
                case AndOperator:
                    if (constIsZero)
                        ret = MakeConstantExprAttributes (NewMP_INT (0), BitTypeObj, position);
                    else
                        ret = nonConstAttr;
                    placeComponents = false;
                    break;
                case OrOperator:
                    if (constIsZero)
                        ret = nonConstAttr;
                    else
                        ret = MakeConstantExprAttributes (NewMP_INT (1), BitTypeObj, position);
                    placeComponents = false;
                    break;
                case EqualOperator:
                case NotEqualOperator:
                    {
                        bool needInvert = (operation == NotEqualOperator) ^ constIsZero;
                        PtrType oldType = nonConstAttr.value.type;

                        /* Temporarily twiddle the type of the nonConst attributes */
                        nonConstAttr.value.type = BitTypeObj;
                        ret = (needInvert ? HandleUnaryExpr (context, InvertOperator, nonConstAttr, position) : nonConstAttr);
                        nonConstAttr.value.type = oldType;
                        ret.value.type = BitTypeObj;
                        placeComponents = false;
                    }
                    break;
                default:
                    break;
                }
            }
        }

        if (placeComponents)
        {                       /* Non constant, need to join left and right */
            PtrWire newWire;

            ret.wires = AppendWireLists (CopyWireList (left.wires), right.wires);
            ret.components = AppendComponentLists (CopyComponentList (left.components), right.components);
            ret.accesses = ParallelCombineAccesses (left.accesses, right.accesses, &ret.components, &ret.wires, position, NULL, false);
            RenumberWireList (ret.wires, false); /* resequence left wires */
            newWire = NewWire (NextBundleNumber (ret.wires), Abs (ret.value.type->size), ret.value.type, 0, true /* Pull */ , position);
            ret.wires = NewWireList (newWire, ret.wires); /* Add to ret.wires */
            ret.components = NewComponentList (NewBinaryFuncComponent (position, operation, ret.value.type->size < 0, left.value.type->size < 0, right.value.type->size < 0, /* RES,L,R signed? */
                newWire, left.activation, right.activation), ret.components);
            ret.activation = newWire;
        }
    }
    return ret;
}

/* HandleUnSharedFunctionCallExpr : handle unshared function calls */
ExprAttributes HandleUnSharedFunctionCallExpr (PtrExprAttributesList arguments, PtrProcedure function, tPosition position)
{
    ExprAttributes ret = NoExprAttributes;
    bool placeComponents = function->nature != BuiltinFunction && (FlattenProcedureCalls || function->context->depth > TopLevelContextDepth);
    unsigned numberOfPorts = LengthOfInstanceList (function->ports);

    /* Check that the arg list was the right length (or not cut short by other error) */
    if (LengthOfExprAttributesList (arguments) == numberOfPorts)
    {
        if (!placeComponents)
        {                       /* Place a ProcedureComponent then attach expressions to it */
            PtrWireList wires = NULL;
            PtrComponent procComp = NewProcedureComponent (position, function->ident, function);
            PtrExprAttributesList attr = arguments;
            PtrInstanceList ports = function->ports;
            unsigned wireCount = 1;

            /* Copy some things from function to Handshake Component */
            procComp->param.procedure.parameters = NULL;
            procComp->param.procedure.portSpec = function->portSpec;
            procComp->param.procedure.ports = ConvertFunctionPortsToProcedureComponentPorts (function->ports, function->info.function.returnType);

            while (attr && ports)
            {
                if (!CAR (attr).activation)
                    break;
                else
                {
                    /* Make list of argument activations */
                    wires = AppendWireLists (wires, NewWireList (CAR (attr).activation, NULL));
                    /* Update the wires, might as well keep this extended form with passive and active ports */
                    UpdateWire (CAR (attr).activation, CAR (ports)->info.port.sense != PassivePortSense, procComp, wireCount);
                    /* Combine the arguments wires/accesses/components */
                    ret.wires = AppendWireLists (ret.wires, CAR (attr).wires);
                    ret.accesses = ParallelCombineAccesses (ret.accesses, CAR (attr).accesses, &ret.components, &ret.wires, position, NULL, false);
                    ret.components = AppendComponentLists (ret.components, CAR (attr).components);
                    wireCount++;
                }
                ports = CDR (ports);
                attr = CDR (attr);
            }
            /* Make up the activation wire */
            ret.activation = NewWire (1 /* fix later */ ,
              Abs (function->info.function.returnType->size), function->info.function.returnType, 0, true /* pull */ , position);
            UpdateWire (ret.activation, false /* passive */ , procComp, 0);
            /* Complete ProcedureComponent's wires */
            wires = NewWireList (ret.activation, wires);
            procComp->ports = wires;
            /* Add procComp to components, activation to wires */
            ret.wires = NewWireList (ret.activation, ret.wires);
            ret.components = NewComponentList (procComp, ret.components);
            ret.value = NewTypedValue (NULL, function->info.function.returnType);
            RenumberWireList (ret.wires, false); /* resequence wires */
        } else
        {
            PtrExprAttributesList attr = arguments;
            PtrInstanceList ports = function->ports;
            unsigned wireListLength;
            PtrWireArray portsInFunction;
            unsigned portCount = 1; /* FIXME, even more confused (see Procedure analogue) */

            /* Copy the whole deal */
            ret = DeepCopyExprAttributes (function->info.function.attributes);
            wireListLength = LengthOfWireList (ret.wires);
            /* Rely on the fact that procAttr's wires can be considered to be an array */
            portsInFunction = WIRE_LIST_TO_ARRAY (ret.wires, wireListLength);

            /* Remove wires from the placed function in favour of the activation on the argument */
            while (attr && ports)
            {
                /* Transfer connection from old function argument wire to activation of this argument */
                if (CAR (ports)->info.port.sense != PassivePortSense)
                {               /* Active port, connected to active component in function */
                    UpdateWire (CAR (attr).activation, true /* active */ ,
                      CAR (portsInFunction + portCount)->activeComponent, CAR (portsInFunction + portCount)->activePort);
                    SubstituteWireForWireInWireList (CAR
                      (portsInFunction + portCount)->activeComponent->ports, CAR (portsInFunction + portCount), CAR (attr).activation);
                } else
                {               /* Passive port, connected to passive component in function */
                    UpdateWire (CAR (attr).activation, false /* passive */ ,
                      CAR (portsInFunction + portCount)->passiveComponent, CAR (portsInFunction + portCount)->passivePort);
                    SubstituteWireForWireInWireList (CAR
                      (portsInFunction + portCount)->passiveComponent->ports, CAR (portsInFunction + portCount), CAR (attr).activation);
                }
                /* Mark the original port wire for deletion */
                MarkWire (CAR (portsInFunction + portCount));
                /* Combine the arguments wires/accesses/components */
                ret.wires = AppendWireLists (ret.wires, CAR (attr).wires);
                ret.accesses = ParallelCombineAccesses (ret.accesses, CAR (attr).accesses, &ret.components, &ret.wires, position, NULL, false);
                ret.components = AppendComponentLists (ret.components, CAR (attr).components);
                portCount++;
                ports = CDR (ports);
                attr = CDR (attr);
            }
            /* Remove all the original ports from the function */
            ret.wires = SweepWireList (ret.wires);
            RenumberWireList (ret.wires, false);
        }
    }
    return ret;
}

/* HandleSharedFunctionCallExpr : handle shared function calls */
ExprAttributes HandleSharedFunctionCallExpr (PtrExprAttributesList arguments, PtrProcedure function, tPosition position)
{
#if 0

/* FIXME */
    ExprAttributes ret = NoExprAttributes;

    if (arguments)
        LOG_ERROR (CannotHaveArgumentsToSharedFunction, function->ident, position);
    else
    {
        /* Make an activation wire, include the accesses internal to the shaerd block */
        ret.activation = NewSyncWire (1, NULL, position);
        ret.wires = NewWireList (ret.activation, NULL);
        {                       /* The dummy wires and components are just for the parallel combine of accesses, which
                                   shouldn't produce any wires or components */
            PtrComponentList dummyComps = NULL;
            PtrWireList dummyWires = NULL;

            ret.accesses =
              ParallelCombineAccesses (NewAccessList
              (NewSharedCallAccess (function, ret.activation), NULL), function->unplacedAccesses, &dummyComps, &dummyWires, position, NULL, false);
        }
        ret.activation->accesses = ret.accesses;
        ret.permanent = function->info.function.attributes.permanent;
    }
    return ret;
#endif
    return NoExprAttributes;
}

/* HandleParameterisedBuiltinFunctionCallExpr : handle unshared function calls */
ExprAttributes HandleParameterisedBuiltinFunctionCallExpr (PtrExprAttributesList arguments, PtrProcedure function, tPosition position)
{
    ExprAttributes ret = NoExprAttributes;
    unsigned numberOfPorts = LengthOfInstanceList (function->ports);

    /* Check that the arg list was the right length (or not cut short by other error) */
    if (LengthOfExprAttributesList (arguments) == numberOfPorts)
    {
        PtrWireList wires = NULL;
        PtrComponent procComp = NewProcedureComponent (position, function->info.parameterisedFunction.ident,
          function);
        PtrExprAttributesList attr = arguments;
        PtrInstanceList ports = function->ports;
        PtrType returnType = function->info.parameterisedFunction.returnType;
        unsigned wireCount = 1;

        /* Copy some things from function to Handshake Component */
        procComp->param.procedure.parameters = function->info.parameterisedFunction.parameters;
        procComp->param.procedure.portSpec = function->portSpec;
        procComp->param.procedure.ports = ConvertFunctionPortsToProcedureComponentPorts (function->ports, returnType);
        procComp->param.procedure.componentType = IsBuiltinFunctionIdent;
        procComp->param.procedure.baseComponentName = function->info.parameterisedFunction.ident;

        while (attr && ports)
        {
            if (!CAR (attr).activation)
                break;
            else
            {
                /* Make list of argument activations */
                wires = AppendWireLists (wires, NewWireList (CAR (attr).activation, NULL));
                /* Update the wires, might as well keep this extended form with passive and active ports */
                UpdateWire (CAR (attr).activation, CAR (ports)->info.port.sense != PassivePortSense, procComp, wireCount);
                /* Combine the arguments wires/accesses/components */
                ret.wires = AppendWireLists (ret.wires, CAR (attr).wires);
                ret.accesses = ParallelCombineAccesses (ret.accesses, CAR (attr).accesses, &ret.components, &ret.wires, position, NULL, false);
                ret.components = AppendComponentLists (ret.components, CAR (attr).components);
                wireCount++;
            }
            ports = CDR (ports);
            attr = CDR (attr);
        }
        /* Make up the activation wire */
        ret.activation = NewWire (1 /* fix later */ ,
          Abs (returnType->size), returnType, 0, true /* pull */ , position);
        UpdateWire (ret.activation, false /* passive */ , procComp, 0);
        /* Complete ProcedureComponent's wires */
        wires = NewWireList (ret.activation, wires);
        procComp->ports = wires;
        /* Add procComp to components, activation to wires */
        ret.wires = NewWireList (ret.activation, ret.wires);
        ret.components = NewComponentList (procComp, ret.components);
        ret.value = NewTypedValue (NULL, returnType);
        RenumberWireList (ret.wires, false); /* resequence wires */
    }
    return ret;
}

/* HandleFunctionCallExpr : handle the placement of components/parts for a function call */
ExprAttributes HandleFunctionCallExpr (PtrExprAttributesList arguments, PtrProcedure function, tPosition position)
{
    ExprAttributes ret = NoExprAttributes;

    if (function != NoProcedure)
    {
        switch (function->nature)
        {
        case SharedFunction:
            ret = HandleSharedFunctionCallExpr (arguments, function, position);
            break;
        case BuiltinFunction:
        case UnSharedFunction:
            ret = HandleUnSharedFunctionCallExpr (arguments, function, position);
            break;
        case ParameterisedBuiltinFunction:
            ret = HandleParameterisedBuiltinFunctionCallExpr (arguments, function, position);
            break;
        default:               /* Can't happen */
            break;
        }
    }
    return ret;
}

/* DeepCopyExprAttributes : use DeepCopyCommandAttributes to copy attributes of an expression */
ExprAttributes DeepCopyExprAttributes (ExprAttributes attr)
{
    ExprAttributes ret;
    CommandAttributes temp;

    temp.permanent = false;
    temp.components = attr.components;
    temp.activation = attr.activation;
    temp.wires = attr.wires;
    temp.accesses = attr.accesses;

    temp = DeepCopyCommandAttributes (temp);

    ret.components = temp.components;
    ret.activation = temp.activation;
    ret.wires = temp.wires;
    ret.accesses = temp.accesses;
    ret.value = attr.value;
    return ret;
}
