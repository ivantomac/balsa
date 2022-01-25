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

	`lvalues.c'
	Lvalue expression handling functions
	
 */

#include "lvalues.h"
#include "Errors.h"

/* HandleIdentLvalue : resolve an instance representing a writable variable and
   generate the appropriate access hardware.  Invalidates (makes NULL) *instance
   on error */
ExprAttributes HandleIdentLvalue (PtrContext context, PtrInstance * instance, PtrSpanList * partitions, PtrAccess * access, tPosition position)
{
    ExprAttributes ret = NoExprAttributes;
    PtrInstance thisInstance = *instance;

    *instance = NULL;           /* Assume the worst */

    /* Check variable for read onlyness */
    if (thisInstance->info.variable.readOnly)
        LOG_ERROR (CannotWriteToLockedChannel, thisInstance->ident, position);
    else
    {
        /* Generate a single span */
        *partitions = NewSpanList (NewSpan (NewMP_INT (0), NewMP_INT (Abs (thisInstance->type->size) - 1), NoType), NULL);

        /* Make up the access */
        *access = NewAccess (thisInstance);
        (*access)->info.variable.wires = NULL;
        (*access)->info.variable.readMap = NewMP_INT (0);
        (*access)->info.variable.writeMap = MakeMaskForRange (Abs (thisInstance->type->size), 0);

        ret.accesses = NULL;
		/* AB 2008-08-11 */
        ret.activation = NewSyncWire (0, NoPosition);
        NameWire (ret.activation, thisInstance->ident);
        ret.value.type = thisInstance->type;
        ret.wires = NULL;

        *instance = thisInstance;
    }

    return ret;
}

/* HandleArrayElemLvalue : resolve a array access on a variable, remain quiet
   if an error was reported `upstream', *instance will contain on exit the same things as in
   HandleIdentLvalue. */
ExprAttributes
HandleArrayElemLvalue (PtrContext context,
  ExprAttributes lhsAttr, PtrInstance lhsInstance,
  PtrSpanList lhsPartitions, PtrSpanListList lhsIndices,
  PtrWire lhsIndexWire, ExprAttributes expr,
  PtrInstance * instance, PtrSpanList * partitions, PtrSpanListList * indices, PtrWire * indexWire, PtrAccess access, tPosition position)
{
    ExprAttributes ret = NoExprAttributes;
    Span lhsRange = (lhsAttr.value.type->nature == ArrayType ? lhsAttr.value.type->info.array.range : NoSpan);

    *instance = NULL;
    *partitions = NULL;
    *indices = NULL;
    *indexWire = NULL;

    if (!lhsInstance)
    {
        return ret;
    }
    /* Bow out now if previous Lvalue failed */
    else if (lhsAttr.value.type->nature != ArrayType)
        LOG_ERROR (ExpectingAnArrayType, NoIdent, position);
    else if (!SubscriptTypeIsOK (lhsRange.boundingType, expr.value.type))
        LOG_ERROR (SubscriptHasWrongType, NoIdent, position);
    else if (!expr.value.value.baseValue) /* Non-const */
    {
        PtrMP_INT indexIter = NewMP_INT (0);
        unsigned elementLength = Abs (lhsAttr.value.type->info.array.baseType->size);
        PtrMP_INT lowSubscript, highSubscript;
        PtrMP_INT lowArraySubscript, highArraySubscript;
        PtrMP_INT lowElementNumber, highElementNumber;
        PtrMP_INT rawLowElementNumber, rawHighElementNumber;

        lowSubscript = MinimumValueOfRange (expr.value.type->size);
        highSubscript = MaximumValueOfRange (expr.value.type->size);
        lowArraySubscript = lhsAttr.value.type->info.array.range.lowerBound;
        highArraySubscript = lhsAttr.value.type->info.array.range.upperBound;

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

        /* Split each partition element into so many indexed chunks */
        {
            PtrSpanList head = NewSpanList (NoSpan, NULL);
            PtrSpanList tail = head;

            /* FOR_EACH (lhsPartitions, true, ...) */
            while (lhsPartitions)
            {
                for (mpz_set (indexIter, lowElementNumber); mpz_cmp (indexIter, highElementNumber) <= 0; mpz_add_ui (indexIter, indexIter, 1))
                {
                    PtrMP_INT lower = NewMP_INT (0);
                    PtrMP_INT upper = NewMP_INT (0);

                    /* Add the low index offset to the elementLength * this element number */
                    mpz_add_ui (lower, lower, elementLength * mpz_get_si (indexIter));
                    mpz_add_ui (upper, lower, elementLength - 1);
                    tail->next = NewSpanList (NewSpan (lower, upper, CAR (lhsPartitions).boundingType), NULL);
                    tail = CDR (tail);
                }
                lhsPartitions = CDR (lhsPartitions);
            }
            *partitions = CDR (head);
        }

        /* Make up indices for this indexing round */
        {
            unsigned previousIndexWidth = 0;
            PtrMP_INT thisIndexMask = MakeMaskForRange (Abs (expr.value.type->size), 0);

            ret = expr;         /* Start with expression attributes */
            ret.activation = NULL;
            ret.value.type = lhsAttr.value.type->info.array.baseType;
            ret.components = AppendComponentLists (CopyComponentList (lhsAttr.components), expr.components);
            ret.wires = AppendWireLists (CopyWireList (lhsAttr.wires), expr.wires);
            ret.accesses = ParallelCombineAccesses (lhsAttr.accesses, ret.accesses, &ret.components, &ret.wires, position, NULL, false);
            *indexWire = expr.activation;

            if (lhsIndexWire)   /* Have we already seen indexing before? */
            {
                /* Combine the old wire and the new index wire */
                PtrWire newIndexWire = NewWire (1, /* fixed later */
                  lhsIndexWire->width + Abs (ret.value.type->size),
                  NoType, 0, true /* pull */ ,
                  position);

                ret.wires = NewWireList (newIndexWire, ret.wires);
                ret.components = NewComponentList (NewCombineComponent (position, newIndexWire, expr.activation, lhsIndexWire), ret.components);
                *indexWire = newIndexWire;
                previousIndexWidth = lhsIndexWire->width;
            } else
                /* Make up a list with a single index if no indexing has happened before */
            {
                lhsIndices = NewSpanListList (NewSpanList (NewSpan (NewMP_INT (0), NewMP_INT (0), NoType), NULL), NULL);
            }

            {                   /* Run along old indices, create new ones */
                PtrSpanListList head = NewSpanListList (NewSpanList (NoSpan, NULL), NULL);
                PtrSpanListList tail = head;
                PtrMP_INT tmp = NewMP_INT (0);

                /* FOR_EACH (lhsIndices, true, ...) */
                while (lhsIndices)
                {
                    for (mpz_set (indexIter, rawLowElementNumber);
                      mpz_cmp (indexIter, rawHighElementNumber) <= 0; mpz_add_ui (indexIter, indexIter, 1))
                    {
                        PtrMP_INT lower = CopyMP_INT (CAR (CAR (lhsIndices)).lowerBound);
                        PtrMP_INT upper = CopyMP_INT (CAR (CAR (lhsIndices)).upperBound);

                        /* Shift previousIndexWidth places to the left and add current index */
                        mpz_mul_2exp (lower, lower, previousIndexWidth);
                        mpz_and (tmp, indexIter, thisIndexMask);
                        mpz_add (lower, lower, tmp);
                        /* And with upper */
                        mpz_mul_2exp (upper, upper, previousIndexWidth);
                        mpz_and (tmp, indexIter, thisIndexMask);
                        mpz_add (upper, upper, tmp);
                        tail->next = NewSpanListList (NewSpanList (NewSpan (lower, upper, CAR (CAR (lhsIndices)).boundingType), NULL), NULL);
                        tail = CDR (tail);
                    }
                    lhsIndices = CDR (lhsIndices);
                }
                DeleteMP_INT (tmp);
                *indices = CDR (head);
            }
            DeleteMP_INT (thisIndexMask);
        }
        *instance = lhsInstance;
        RenumberWireList (ret.wires, false);
    } else
        /* Constant index */
    {
        if (expr.value.value.dontCares)
            LOG_ERROR (ImplicantNotAllowedHere, NoIdent, position);
        else if (mpz_cmp (expr.value.value.baseValue, lhsRange.lowerBound) < 0 || mpz_cmp (expr.value.value.baseValue, lhsRange.upperBound) > 0)
            LOG_ERROR (SubscriptOutOfRange, NoIdent, position);
        else if (lhsInstance->info.variable.readOnly)
            LOG_ERROR (CannotWriteToLockedChannel, lhsInstance->ident, position);
        else
        {
            PtrMP_INT elementNumber = NewMP_INT (0);
            unsigned elementLength, elementOffset;

            elementLength = Abs (lhsAttr.value.type->info.array.baseType->size);

            mpz_sub (elementNumber, expr.value.value.baseValue, lhsAttr.value.type->info.array.range.lowerBound);
            /* elementOffset is the bit wise offset of this element into the array */
            elementOffset = mpz_get_si (elementNumber) * elementLength;

            {                   /* Restrict the lhsPartitions to this array element */
                PtrSpanList head = NewSpanList (NoSpan, NULL); /* sacrificial head */
                PtrSpanList tail = head;

                /* FOR_EACH (lhsPartitions, true, ...) */
                while (lhsPartitions)
                {
                    PtrMP_INT lower = CopyMP_INT (CAR (lhsPartitions).lowerBound);
                    PtrMP_INT upper = NewMP_INT (0);

                    mpz_add_ui (lower, lower, elementOffset);
                    mpz_add_ui (upper, lower, elementLength - 1);
                    tail->next = NewSpanList (NewSpan (lower, upper, CAR (lhsPartitions).boundingType), NULL);
                    tail = CDR (tail);
                    lhsPartitions = CDR (lhsPartitions);
                }
                *partitions = CDR (head);
            }

            ret = lhsAttr;

			/* AB 2008-08-11 */
			if (ret.activation)
			{
				ret.activation->ident = MakeIdent1 (
					g_strdup_printf ("%s[%lu]", PeekString (ret.activation->ident), mpz_get_ui (expr.value.value.baseValue)));
			}
            ret.value.type = lhsAttr.value.type->info.array.baseType;
            *instance = lhsInstance;
            *indexWire = lhsIndexWire;
            *indices = lhsIndices;
        }
    }
    /* Reconstruct accesses, check for conflicts */
    access->info.variable.writeMap = MakeMaskFromSpanList (*partitions);
    if (VariableParallelAccessConflictInAccessList (NewAccessList (access, NULL), ret.accesses))
    {
        LOG_ERROR (ConflictInLvalueIndex, NoIdent, position);
        *instance = NULL;       /* Invalidate this step */
    }
    return ret;
}

/* HandleRecordElemLvalue : handle the assignment into variable record elements */
ExprAttributes
HandleRecordElemLvalue (ExprAttributes lhsAttr, PtrInstance lhsInstance,
  PtrSpanList lhsPartitions, tIdent elem, PtrInstance * instance, PtrSpanList * partitions, PtrAccess access, tPosition position)
{
    ExprAttributes ret = NoExprAttributes;
    PtrInstance element;

    *instance = NULL;
    *partitions = NULL;

    if (!lhsInstance)
    {
        return ret;
    }
    /* Bow out now if previous Lvalue failed */
    else if (lhsAttr.value.type->nature != RecordType)
        LOG_ERROR (ExpectingARecordType, NoIdent, position);
    else
    {
        element = LookupInstanceInInstanceList (lhsAttr.value.type->info.record.recordElements, elem, false);

        if (!element)
            LOG_ERROR (NotARecordElem, elem, position);
        else if (lhsInstance->info.variable.readOnly)
            LOG_ERROR (CannotWriteToLockedChannel, lhsInstance->ident, position);
        else
        {
            /* Restrict each partition in lhsPartitions to just the record element and return that list */
            PtrSpanList head = NewSpanList (NoSpan, NULL); /* sacrificial head */
            PtrSpanList tail = head;

            /* FOR_EACH (lhsPartitions, true, ...) */
            while (lhsPartitions)
            {
                PtrMP_INT lower = CopyMP_INT (CAR (lhsPartitions).lowerBound);
                PtrMP_INT upper = NewMP_INT (0);

                mpz_add_ui (lower, lower, element->info.recordElem.offset);
                mpz_add_ui (upper, lower, Abs (element->type->size) - 1);
                tail->next = NewSpanList (NewSpan (lower, upper, CAR (lhsPartitions).boundingType), NULL);
                tail = CDR (tail);
                lhsPartitions = CDR (lhsPartitions);
            }

            *partitions = CDR (head);
            *instance = lhsInstance;
            ret = lhsAttr;
			/* AB 2008-08-11 */
			if (ret.activation)
			{
				ret.activation->ident = MakeIdent1 (
					g_strdup_printf ("%s.%s", PeekString (ret.activation->ident), PeekString (elem)));
			}
            ret.value.type = element->type;
        }
    }
    access->info.variable.writeMap = MakeMaskFromSpanList (*partitions);
    if (VariableParallelAccessConflictInAccessList (NewAccessList (access, NULL), ret.accesses))
    {
        LOG_ERROR (ConflictInLvalueIndex, NoIdent, position);
        *instance = NULL;       /* Invalidate this step */
    }
    return ret;
}

/* FinaliseLvalue : place the hardware for an lvalue.  Creates a wire for non
   array accesses, a whole load of hardware (Case component, transferrers ...) for array accesses */
ExprAttributes
FinaliseLvalue (ExprAttributes attributes, PtrInstance instance,
  PtrSpanList partitions, PtrSpanListList indices, PtrWire indexWire, PtrAccess access, tPosition position)
{
    ExprAttributes ret = NoExprAttributes;

	/* AB 2008-08-11 */
	tIdent name;
	if (attributes.activation)
		name = attributes.activation->ident;
	else name = instance->ident;
	attributes.activation = NULL;

    if (!instance)
    {
    } else
    {
        /* Start with the index attributes */
        ret = attributes;
        /* Make a write wire */
        if (indexWire)          /* We have some form of indexing, place a tonne of hardware */
        {
            Bits elementWidth = Abs (ret.value.type->size);

            /* Make the write wire */
            PtrWire writeWire = NewWire (1 /* fix later */ , elementWidth, ret.value.type,
              0 /* offset */ , false /* push */ , position);
            PtrWire probeWire = NewSyncWire (1 /* fix later */ , position);
            PtrWire indexTransferrerOut = CopyAndRenumberWire (indexWire, 1 /* fixed later */ , position);
            PtrWireList transferrerActivations = NULL;
            PtrWireList transferrerInWires = NULL;
            PtrWireList transferrerOutWires = NULL;
            PtrSpanList iter = partitions;

            /* Make the index transferrer output a push wire */
            indexTransferrerOut->isPull = false;
            ret.activation = writeWire;
            ret.wires = NewWireList (writeWire, NewWireList (probeWire, NewWireList (indexTransferrerOut, ret.wires)));

            /* Create the variable accesses, transferrers and transferrer activations */
            /* FOR_EACH (iter, true, ...) */
            while (iter)
            {
                Bits lower = mpz_get_ui (CAR (iter).lowerBound);
                PtrWire act = NewSyncWire (1 /* fix later */ , position);
                PtrWire inp = NewWire (1 /* fix later */ , elementWidth, ret.value.type, 0,
                  true /* pull */ ,
                  position);
                PtrWire out = NewWire (1 /* fix later */ , elementWidth, ret.value.type,
                  lower, false /* push */ ,
                  position);

                ret.components = NewComponentList (NewFetchComponent (position, false, act, inp, out), ret.components);
                ret.wires = NewWireList (act, NewWireList (inp, NewWireList (out, ret.wires)));
                transferrerActivations = NewWireList (act, transferrerActivations);
                transferrerInWires = NewWireList (inp, transferrerInWires);
                transferrerOutWires = NewWireList (out, transferrerOutWires);
                iter = CDR (iter);
            }
            /* Place the False Variable */
            ret.components = NewComponentList (NewFalseVariableComponent (position, 0, elementWidth, writeWire, probeWire, transferrerInWires), ret.components);
            /* Place the Case component */
            ret.components =
              NewComponentList (NewCaseComponent
              (position, MakeSpanCaseSpecification
                (ReverseSpanListList (indices),
                  WireWidth (indexTransferrerOut)),
                indexTransferrerOut, transferrerActivations),
              NewComponentList (NewFetchComponent (position, false, probeWire, indexWire, indexTransferrerOut), ret.components));

            access->info.variable.wires = transferrerOutWires;
            RenumberWireList (ret.wires, false);
        } else
        {                       /* Simple access, should have a single partitions element */
            PtrWire writeWire = NewWire (NextBundleNumber (ret.wires), Abs (ret.value.type->size),
              ret.value.type,
              mpz_get_ui (CAR (partitions).lowerBound) /* offset */ ,
              false /* push */ , position);

			/* AB 2008-08-11 */
            NameWire (writeWire, name);

            ret.activation = writeWire;
            ret.wires = NewWireList (writeWire, ret.wires);
            access->info.variable.wires = NewWireList (writeWire, NULL);
        }
        /* Connect access to the access list */
        ret.accesses = ParallelCombineAccesses (NewAccessList (access, NULL), ret.accesses, &ret.components, &ret.wires, position, NULL, false);
        ret.activation->accesses = ret.accesses;
    }
    return ret;
}
