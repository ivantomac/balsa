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

	`ports.c'
	Functions etc. to manipulate procedure/function/part ports
	
 */

#include "ports.h"
#include "Errors.h"
#include <ctype.h>
#include "Tree.h"

/* HandleChannelPorts : handle the declaration of a group of formal ports for
   a procedure / part or function, returns an augmented
   version of the incomingPorts */
PtrInstanceList
HandleChannelPorts (PtrIdentList idents, PtrType type,
  PortSense portSense, ProcedureArgsType portsType,
  bool isOutput, bool isArray, Span range,
  PtrInstanceList incomingPorts, PtrInstanceList * arrayedElements, unsigned *portCount, PtrLispList options, tPosition position)
{
    PtrInstanceList ports = incomingPorts;
    PtrIdentList ids = ReverseIdentList (idents);

    if (type == NoType || (isArray && range.boundingType == NoType))
        return incomingPorts;   /* Fail silently */

    /* OK, we have the right ports type */
    while (ids)
    {
        if (LookupInstanceInInstanceList (ports, CAR (ids).ident, true /* local */ ))
            LOG_ERROR (NonUniqueLocalName, CAR (ids).ident, CAR (ids).position);
        else /* Procedure with specified port sense ? */ if (portsType == ProcedurePorts
          && portSense != DefaultPortSense)
            LOG_ERROR (CannotSpecifyPortActiveOrPassiveInProc, CAR (ids).ident, CAR (ids).position);
        else /* Function ? */ if (portsType == FunctionArgs)
            LOG_ERROR (CannotPassPortsToFunction, CAR (ids).ident, CAR (ids).position);
        else
        {                       /* Generate instances */
            PtrInstance newPort = NewChannelInstance (CAR (ids).ident, type);

            newPort->nature = (isOutput ? OutputChannelInstance : InputChannelInstance);
            newPort->info.port.sense = portSense; /* Default is required for resolution
                                                     conflict testing, possibly */
            newPort->info.port.wire = NULL;
            newPort->info.port.position = position;
            newPort->info.port.options = (CDR (ids) ? CopyLispList (options) : options);
            newPort->info.port.canMulticast = true; /* FIXME */

            if (isArray)
            {
                newPort->type = NewArrayedType (type, range, newPort);
                /* Increment portCount */
                if (portCount)
                    *portCount += newPort->type->info.arrayed.elementCount;
                *arrayedElements = AppendInstanceLists (newPort->type->info.arrayed.arrayedElements, *arrayedElements);
            } else if (portCount)
                (*portCount)++;
            ports = NewInstanceList (newPort, ports);
        }
        ids = CDR (ids);
    }
    return ports;
}

/* HandleValuePorts : handle the declaration of value ports for functions, creates RO
   variable in a similar vein to those used with FalseVariable's */
PtrInstanceList HandleValuePorts (PtrIdentList idents, PtrType type, ProcedureArgsType portsType, PtrInstanceList incomingPorts, unsigned *portCount)
{
    PtrInstanceList ports = incomingPorts;
    PtrIdentList ids = ReverseIdentList (idents);

    if (type == NoType)
        return incomingPorts;   /* Fail silently */

    /* OK, we have the right ports type */
    while (ids)
    {
        if (LookupInstanceInInstanceList (ports, CAR (ids).ident, true /* local */ ))
            LOG_ERROR (NonUniqueLocalName, CAR (ids).ident, CAR (ids).position);
        else /* Not a function ? */ if (portsType != FunctionArgs)
            LOG_ERROR (CannotPassValuePortsToProcedure, CAR (ids).ident, CAR (ids).position);
        else
        {                       /* Generate instances */
            PtrInstance newPort = NewVariableInstance (CAR (ids).ident, type);

            newPort->info.variable.readOnly = true; /* Don't know if we need this flag, may be useful later */
            newPort->info.variable.channelInstance = NULL; /* readOnly, NULL channel == function arg! */

            if (portCount)
                (*portCount)++;
            ports = NewInstanceList (newPort, ports);
        }
        ids = CDR (ids);
    }
    return ports;
}

/* MakePortSpecString : build a spec string (like the component specs, eg. iOI)
   for a given port list, prepend preString to that spec list */
Ptrchar MakePortSpecString (PtrInstanceList ports, unsigned portListMaxLength)
{
    Ptrchar ret = NEW_ARRAY (char, portListMaxLength + 1);
    unsigned portNo = 0;

    while (ports)
    {
        char portNature = 'i';  /* Default to passive input */

        /* i, o or s */
        switch (CAR (ports)->nature)
        {
        case InputChannelInstance:
        case VariableInstance: /* function value ports */
            if (CAR (ports)->type == SyncTypeObj)
                portNature = 's';
            else
                portNature = 'i';
            break;
        case OutputChannelInstance:
            portNature = 'o';
            break;
        default:
            break;
        }
        /* active or passive */
        if (CAR (ports)->nature == VariableInstance || /* Expression ports are active */
          CAR (ports)->info.port.sense != PassivePortSense)
            portNature = toupper (portNature);

        if (CAR (ports)->type->nature == ArrayedType) /* Need to multiple place port char */
        {
            unsigned lastPortNo = portNo + CAR (ports)->type->info.arrayed.elementCount;

            for (; portNo < lastPortNo; portNo++)
            {
                ret[portNo] = portNature;
            }
        } else
        {
            ret[portNo] = portNature;
            portNo++;
        }
        ports = CDR (ports);
    }
    ret[portNo] = '\0';

    return ret;
}

/* ConvertExprToLvalue : convert an Expr into an Lvalue to use as a channel in an actual port list
	IdentExpr -> IdentLvalue
	ArrayExtractExpr -> ArrayElemLvalue
	ArraySliceExpr -> ArraySliceLvalue
	ArrayAppendExpr -> ArrayAppendLvalue
	AggregateConsExpr -> ArrayConsLvalue
*/
tTree ConvertExprToLvalue (tTree expr, PtrContext context)
{
    tTree newLvalue = NULL;

    switch (expr->Kind)
    {
    case kIdentExpr:
        newLvalue = mIdentLvalue (expr->Expr.position, expr->IdentExpr.ident);
        break;
    case kArrayExtractExpr:
        {
            tTree child = ConvertExprToLvalue (expr->ArrayExtractExpr.Array, context);

            if (child)
                newLvalue = mArrayElemLvalue (expr->Expr.position, child, expr->ArrayExtractExpr.Subscript);
        }
        break;
    case kArraySliceExpr:
        {
            tTree child = ConvertExprToLvalue (expr->ArraySliceExpr.Expr, context);

            if (child)
                newLvalue = mArraySliceLvalue (expr->Expr.position, child, expr->ArraySliceExpr.Range);
        }
        break;
    case kArrayAppendExpr:
        {
            tTree left = ConvertExprToLvalue (expr->ArrayAppendExpr.Left, context);
            tTree right = ConvertExprToLvalue (expr->ArrayAppendExpr.Right, context);

            if (left && right)
                newLvalue = mArrayAppendLvalue (expr->Expr.position, left, right);
        }
        break;
    case kAggregateConsExpr:
        {                       /* convert Exprs into Channels */
            tTree exprs = expr->AggregateConsExpr.ExprLists;
            tTree channelsHead = NoTree, channelsTail = NoTree;
            bool error = false;

            while (!error && !Tree_IsType (exprs, kNullExprLists))
            {
                /* FIXME, Linked blocks? */
                tTree subExpr = exprs->ExprList.CoercedExpr->CoercedExpr.Expr;

                if (subExpr->Kind == kArraySliceExpr || subExpr->Kind == kAggregateConsExpr || subExpr->Kind == kArrayAppendExpr)
                {
                    LOG_ERROR (ArrayedChannelsCannotBeMultidimensional, NoIdent, exprs->ExprLists.position);
                    error = true;
                } else
                {
                    tTree child = ConvertExprToLvalue (subExpr, context);
                    tTree newChannel = (child ? mLinkedChannel (exprs->ExprLists.position,
                        NoTree /* next */ , NoIdent,
                        child) : NULL);

                    if (!child)
                        error = true;
                    else if (channelsHead != NoTree)
                    {
                        channelsTail->LinkedChannel.next = newChannel;
                        channelsTail = newChannel;
                    } else
                        channelsHead = channelsTail = newChannel;
                }
                exprs = exprs->ExprList.next;
            }

            if (!error)
            {
                /* Finish off with a NullChannels */
                if (channelsHead != NoTree)
                    channelsTail->LinkedChannel.next = mNullLvalueLists (expr->Expr.position);
                else
                    channelsHead = channelsTail = mNullLvalueLists (expr->Expr.position);

                newLvalue = mArrayConsLvalue (expr->Expr.position, channelsHead);
            }
        }
        break;
    default:
        LOG_ERROR (ExpectingAChannel, NoIdent, expr->Expr.position);
        break;
    }

    if (newLvalue)              /* update common bits */
    {
        newLvalue->Lvalue.skip = false;
        newLvalue->Lvalue.context = context;
        newLvalue->Lvalue.expectingChannel = true;
        newLvalue->Lvalue.expectingEither = false;
        newLvalue->Lvalue.lockPassiveChannels = false;

        /* These attributes get set by the evaluator or caller to this function */
        newLvalue->Lvalue.isInput = false;
        newLvalue->Lvalue.isPassive = false;
        newLvalue->Lvalue.expectedBaseType = NoType;
    }
    return newLvalue;
}

/* ConvertCoercedExprToLinkedChannel : convert an actual-port-representing CoercedExpr tTree
	node into a LinkedChannel */
tTree ConvertCoercedExprToLinkedChannel (tTree expr, PtrContext context, PtrInstanceList formalPorts)
{
    tTree oldExpr = expr->CoercedExpr.Expr;
    tTree newLinkedChannel = NULL;
    tTree newLvalue = NULL;

    /* convert the Expr into an Lvalue */
    if (formalPorts)
    {
        newLvalue = ConvertExprToLvalue (oldExpr, context);

        if (newLvalue)
        {
            PtrInstance port = CAR (formalPorts);

            newLinkedChannel = mLinkedChannel (oldExpr->Expr.position, mNullLvalueLists (oldExpr->Expr.position), NoIdent, newLvalue);

            /* change the CoercedExpr into a LinkedChannel first */
            newLinkedChannel->LinkedChannel.context = context;
            newLinkedChannel->LinkedChannel.skip = false;
            newLinkedChannel->LinkedChannel.elements = NULL;

            /* setup type in LinkedChannel */
            newLinkedChannel->LinkedChannel.expectedType = port->type;
            newLinkedChannel->LinkedChannel.isInput = port->nature == InputChannelInstance;
            newLinkedChannel->LinkedChannel.isPassive = port->info.port.sense == PassivePortSense;
            newLinkedChannel->LinkedChannel.lockPassiveChannels = false;
            newLinkedChannel->LinkedChannel.isProcedurePorts = true;
            newLinkedChannel->LinkedChannel.allowArrayedChannels = true;
        }
    }

    /* check for null newLinkedChannel and replace with nullChannels */
    return (newLinkedChannel ? newLinkedChannel : mNullLvalueLists (oldExpr->Expr.position));
}

/* ConvertFunctionPortsToProcedureComponentPorts : convert the usual ValuePort
	list of a function to a set of passive input ports like those of the
	part for that function. */
PtrInstanceList ConvertFunctionPortsToProcedureComponentPorts (PtrInstanceList ports, PtrType returnType)
{
    PtrInstance activatePort = NewChannelInstance (MakeIdent1 ("activate"), returnType);
    PtrInstanceList ret = NewInstanceList (activatePort, NULL);

    /* Make an activation port too */
    activatePort->nature = OutputChannelInstance;
    activatePort->info.port.sense = PassivePortSense;
    activatePort->info.port.wire = NULL;
    activatePort->info.port.options = NULL;
    activatePort->info.port.position = NoPosition;

    while (ports)
    {
        PtrInstance port = CAR (ports);

        if (port->nature == VariableInstance)
        {
            PtrInstance newPort = NewChannelInstance (port->ident, port->type);

            newPort->nature = InputChannelInstance;
            newPort->info.port.sense = ActivePortSense;
            newPort->info.port.wire = NULL;
            newPort->info.port.options = NULL;
            newPort->info.port.position = NoPosition;
            ret = NewInstanceList (newPort, ret);
        } else
            ret = NewInstanceList (CAR (ports), ret);

        ports = CDR (ports);
    }

    return ReverseInstanceList (ret);
}

/* StrPortSense : write out the port sense (- default -), active, passive */
void StrPortSense (FILE * stream, PortSense sense)
{
    switch (sense)
    {
    case DefaultPortSense:
        break;
    case ActivePortSense:
        fprintf (stream, "active");
        break;
    case PassivePortSense:
        fprintf (stream, "passive");
        break;
    default:
        fprintf (stream, "(-- bad sense --)");
        break;
    }
}
