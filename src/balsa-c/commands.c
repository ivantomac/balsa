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

	`commands.c'
	Command compilation handling functions
	
 */

#include <string.h>
#include "commands.h"
#include "contexts.h"
#include "Errors.h"
#include "flags.h"
#include "block.h"
#include "callcontexts.h"
#include "exprs.h"
#include "decls.h"
#include "Tree.h"

DEFINE_LIST_TYPE_CONSTRUCTOR (CommandAttributes, CommandAttributes) DEFINE_LIST_TYPE_LENGTHOF (CommandAttributes)
/* NoCommandAttributes : Default value, all NULL and No### */
CommandAttributes NoCommandAttributes;

/* Initialise */
void BeginCommands (void)
{
    NoCommandAttributes.components = NULL;
    NoCommandAttributes.accesses = NULL;
    NoCommandAttributes.wires = NULL;
    NoCommandAttributes.activation = NULL;
    NoCommandAttributes.permanent = false;
}

/* HandleContinueCommand : create a continue component and a wire */
CommandAttributes HandleContinueCommand (tPosition position)
{
    CommandAttributes ret;

    ret.activation = NewSyncWireWithAccesses (1, NULL, position);
    ret.wires = NewWireList (ret.activation, NULL);
    ret.components = NewComponentList (NewContinueComponent (position, ret.activation), NULL);
    ret.accesses = NULL;
    ret.permanent = false;      /* Continue always returns ack */
    return ret;
}

/* HandleHaltCommand : create a halt component and a wire */
CommandAttributes HandleHaltCommand (tPosition position)
{
    CommandAttributes ret;

    ret.activation = NewSyncWireWithAccesses (1, NULL, position);
    ret.wires = NewWireList (ret.activation, NULL);
    ret.components = NewComponentList (NewHaltComponent (position, ret.activation), NULL);
    ret.accesses = NULL;
    ret.permanent = true;       /* Halt never returns ack */
    return ret;
}

/* HandleSequentialCommand : handle the merging of accesses/components etc. on ; operator */
CommandAttributes HandleSequentialCommand (CommandAttributes left, CommandAttributes right, tPosition position)
{
    CommandAttributes ret = NoCommandAttributes;

    if (!left.activation || !right.activation)
        return ret;             /* Fail silently */
    /* LHS/RHS is just a continue */
    if (left.components && !CDR (left.components) && CAR (left.components)->nature == ContinueComponent)
        return right;
    if (right.components && !CDR (right.components) && CAR (right.components)->nature == ContinueComponent)
        return left;

    if (left.permanent)         /* Left is permanent => w don't execute RHS */
    {
        LOG_ERROR (PermanentLHSInSequentialCommand, NoIdent, position);
        return left;
    }
    ret.wires = AppendWireLists (CopyWireList (left.wires), right.wires);
    ret.components = AppendComponentLists (CopyComponentList (left.components), right.components);
    ret.accesses = SeriesCombineAccesses (left.accesses, right.accesses, &ret.components, &ret.wires, position);
    ret.permanent = right.permanent;

    /* Runs are generated as (SequentialCommand ; Command) */
    if (NwaySequenceOptimise && left.activation
      && left.activation->passiveComponent && left.activation->passiveComponent->nature == SequenceComponent)
    {
        ret.activation = left.activation;
        left.activation->passiveComponent->ports = AppendWireLists (left.activation->passiveComponent->ports, NewWireList (right.activation, NULL));
        /* Frig the right activation active component */
        UpdateWire (right.activation, true /* active */ ,
          left.activation->passiveComponent, LengthOfWireList (left.activation->passiveComponent->ports) - 1);
    } else
    {
        ret.activation = NewSyncWireWithAccesses (1, /* correct the BundleNumber in a second */ ret.accesses, position);
        ret.components =
          NewComponentList (NewSequenceComponent
          (position, ret.activation, left.activation, right.activation), ret.components);
        ret.wires = NewWireList (ret.activation, ret.wires);
    }
    /* resequence wires */
    RenumberWireList (ret.wires, false);
    return ret;
}

/* HandleParallelCommand : ditto de || operator */
CommandAttributes HandleParallelCommand (CommandAttributes left, CommandAttributes right, bool isPermissive, tPosition position)
{
    CommandAttributes ret = NoCommandAttributes;
    bool generateFORK = true;

    if (!left.activation || !right.activation)
        return ret;             /* Fail silently */
    /* LHS/RHS is just a continue */
    if (left.components && !CDR (left.components) && CAR (left.components)->nature == ContinueComponent)
        return right;
    if (right.components && !CDR (right.components) && CAR (right.components)->nature == ContinueComponent)
        return left;

    ret.wires = AppendWireLists (CopyWireList (left.wires), right.wires);
    ret.components = AppendComponentLists (CopyComponentList (left.components), right.components);
    ret.accesses = ParallelCombineAccesses (left.accesses, right.accesses, &ret.components, &ret.wires, position, &generateFORK, isPermissive);
    ret.permanent = left.permanent && right.permanent;
    /* Use a FORK if left and right are non communicative or if both are permanent */
    /* FIXME, have a good long look at this */
    generateFORK = /* generateFORK || */ ret.permanent;

    /* Runs are generated as (SequentialCommand ; Command) */
    if (NwayConcurOptimise && left.activation->passiveComponent &&
      left.activation &&
      (generateFORK ? (UseWireForks ? WireForkComponent : ForkComponent) : ConcurComponent) == left.activation->passiveComponent->nature)
    {
        ret.activation = left.activation;
        left.activation->passiveComponent->ports = AppendWireLists (left.activation->passiveComponent->ports, NewWireList (right.activation, NULL));
        /* Frig the right activation active component */
        UpdateWire (right.activation, true /* active */ ,
          left.activation->passiveComponent, LengthOfWireList (left.activation->passiveComponent->ports) - 1);
    } else
    {
        ret.activation = NewSyncWireWithAccesses (1, /* correct the BundleNumber in a second */ ret.accesses, position);
        ret.components =
          NewComponentList ((generateFORK
            ? (UseWireForks ? NewWireForkComponent :
              NewForkComponent) : NewConcurComponent) (position, ret.activation, left.activation, right.activation), ret.components);
        ret.wires = NewWireList (ret.activation, ret.wires);
    }
    RenumberWireList (ret.wires, false); /* resequence wires */
    return ret;
}

/* HandleLoopCommand : create a Loop component connected to command */
CommandAttributes HandleLoopCommand (CommandAttributes command, tPosition position)
{
    CommandAttributes ret = command;

    if (command.activation)     /* No activation, forget it */
    {
        if (command.permanent)  /* If command is permanent then the loop is redundant */
        {
            LOG_ERROR (LoopBodyIsPermanent, NoIdent, position);
            return command;
        }
        ret.activation = NewSyncWireWithAccesses (NextBundleNumber (command.wires), command.accesses, position);
        ret.components = NewComponentList (NewLoopComponent (position, ret.activation, command.activation), command.components);
        ret.wires = NewWireList (ret.activation, command.wires);
        ret.permanent = true;   /* Loop's are permanent */
    }
    return ret;
}

/* HandleSyncCommand : create an access on a sync channel */
CommandAttributes HandleSyncCommand (PtrContext context, ExprAttributes chanAttr, PtrInstance chanInstance, tPosition position)
{
    CommandAttributes ret = NoCommandAttributes;

    if (!chanInstance)
    {                           /* Fail silently */
    } else if (chanInstance->nature == VariableInstance /* Not A channel */
      || chanAttr.value.type->nature != SyncType)
        LOG_ERROR (NotASyncChannel, chanInstance->ident, position);
    else if (chanInstance->type->nature == ArrayedType)
        LOG_ERROR (NotExpectingAnArrayedChannel, NoIdent, position);
    else
    {
        /* Not a valid, resolved channel ? */
        if (chanAttr.activation)
        {                       /* V. simple, most of this is handled by the lvalue */
            /* Components, prob. don't need this but what the hell */
            ret.components = chanAttr.components;
            ret.wires = chanAttr.wires;
            ret.accesses = chanAttr.accesses;
            ret.activation = chanAttr.activation;
            ret.permanent = false;
        } else
        {                       /* Error already reported, I think */
        }
    }
    return ret;
}

/* ReduceGuardList : reduce a guard list by removing manifestly false guards, sets constantOneAttr
   to the first manifestly true guard command (if any, use CAR(constantOneAttr)), returns:
   -1 == CAR(constantOneAttr) contains attributes for a manifestly true guard,
   0 == all guards are manifestly false,
   > 1 == some guards (all non constant expressions) */
int
ReduceGuardList (PtrCommandAttributesList commands,
  PtrExprAttributesList exprs,
  PtrCommandAttributesList * newCommands, PtrExprAttributesList * newExprs, PtrCommandAttributesList * constantOneAttr, tPosition position)
{
    bool removeElement;         /* Remove the next guard element */

    *newCommands = NULL;
    *newExprs = NULL;
    *constantOneAttr = NULL;    /* set to a list element if a manifestly true element is found in the expr list */

    /* Traverse commands/exprs and remove the constant false's/look for constant 1's */
    /* NB. length(commands) == length(exprs), CAR(exprs).value.type =equiv= 1 bits */
    while (commands)
    {
        removeElement = false;
        if (CAR (exprs).value.value.baseValue) /* manifest constant */
        {
            if (mpz_sgn (CAR (exprs).value.value.baseValue) != 0) /* 1 */
            {
                if (*constantOneAttr) /* Already seen a true element */
                {
                    removeElement = true;
                } else
                    *constantOneAttr = commands;
            } else              /* else 0 */
                removeElement = true;
        }                       /* else Non constant */
        if (removeElement)
        {
            commands = CDR (commands);
            exprs = CDR (exprs);
            removeElement = false;
        } else
        {                       /* Don't remove, save the next pointer value then link this element onto the newLists */
            PtrCommandAttributesList tempNextCommand = CDR (commands);
            PtrExprAttributesList tempNextExpr = CDR (exprs);

            /* Reuse the elements from the old list in the new */
            *newCommands = CONS (commands, *newCommands);
            *newExprs = CONS (exprs, *newExprs);
            /* Move to next element */
            commands = tempNextCommand;
            exprs = tempNextExpr;
        }
    }
    if (*constantOneAttr)
    {
        (*constantOneAttr)->next = NULL;
        return -1;
    }
    return LengthOfCommandAttributesList (*newCommands);
}

/* PlaceActiveBuffer : place a sequencer, 2 fetch components and a variable to form a single place
   buffer with active input and output. Takes the width of the variable/internal channels
   from the width of the inputWire (the outputWire should be of the same width). */
void
PlaceActiveBuffer (PtrWire activation, PtrWire inputWire, PtrWire outputWire,
  PtrAccessList inputWireAccesses, PtrAccessList outputWireAccesses,
  Ptrchar bufferName, PtrWireList * wires, PtrComponentList * comps, tPosition position)
{
    BundleNumber bundle = (wires ? NextBundleNumber (*wires) : 1);

    /* Fetch (outFetchAct, outFetchIn, outputWire) */
    /* Fetch (inFetchAct, inputWire, inFetchOut) */
    /* Inserting variable accesses to be (semi, not don't consider inputWire/outputWire accesses!) honest */
    PtrWire inFetchOut = CopyAndRenumberWire (inputWire, bundle + 2, position); /* Variable (inFetchOut, outFetchIn) */
    PtrWire outFetchIn = CopyAndRenumberWire (inFetchOut, bundle + 3, position);
    PtrWire inFetchAct = NewSyncWire (bundle, position);
    PtrWire outFetchAct = NewSyncWire (bundle + 1, position);

	/* Make accesses on inFetchAct and outFetchOut correct */
    {
		PtrInstance variable = NewVariableInstance (MakeIdent1 (bufferName), NewNumericType (NoIdent, inputWire->width));
		PtrComponentList dummyComps = NULL;
		PtrWireList dummyWires = NULL;
		inFetchOut->isPull = false;
		outFetchIn->isPull = true;
		inFetchAct->accesses = ParallelCombineAccesses (
			NewAccessList (NewVariableAccess (variable, inFetchOut), NULL),
			inputWireAccesses, &dummyComps, &dummyWires, position, NULL, false
		);
		outFetchIn->accesses = ParallelCombineAccesses (
			NewAccessList (NewVariableAccess (variable, outFetchIn), NULL),
			outputWireAccesses, &dummyComps, &dummyWires, position, NULL, false
		);
	}

    /* and Sequence (activation, inFetchAct, outFetchAct) */
    *comps =
      NewComponentList (NewSequenceComponent (position, activation, inFetchAct, outFetchAct),
      NewComponentList (NewFetchComponent (position, false, inFetchAct, inputWire, inFetchOut),
      NewComponentList (NewFetchComponent (position, false, outFetchAct, outFetchIn, outputWire),
      NewComponentList (NewVariableComponent (position, StrDup (bufferName), 0, inputWire->width,
      	inFetchOut, NewWireList (outFetchIn, NULL)), *comps))));
    *wires = NewWireList (outFetchIn, NewWireList (inFetchOut, NewWireList (outFetchAct, NewWireList (inFetchAct, *wires))));
}

/* HandleIfCommand : handle the placement of an if or select component set */
CommandAttributes
HandleIfCommand (bool hasElse, PtrContext context,
  PtrCommandAttributesList commands, PtrExprAttributesList exprs, CommandAttributes elseCommand, tPosition position)
{
    PtrCommandAttributesList newCommands;
    PtrExprAttributesList newExprs;
    PtrCommandAttributesList constantOneAttr;

    int guardCount;

    /* This shouldn't be necessary any more (except for the constantOneAttr), but do it anyway */
    guardCount = ReduceGuardList (commands, exprs, &newCommands, &newExprs, &constantOneAttr, position);

    if (guardCount == -1)       /* Manifestly true single guard, else will fail */
        return CAR (constantOneAttr);
    else if (hasElse && !elseCommand.activation) /* else command is significant, fail if it failed */
        return NoCommandAttributes;
    else
    {
        if (guardCount == 0)    /* No guards, all failed therefore place a Continue (or use the else command) */
        {
            if (hasElse)
                return elseCommand;
            else
                return HandleContinueCommand (position);
        } else if (guardCount == 1) /* Single guard, use Select */
        {
            CommandAttributes thenCommand = CAR (newCommands);
            ExprAttributes expr = CAR (newExprs);
            CommandAttributes ret = thenCommand;
			PtrAccessList commandAccesses = NULL;
            bool exprAndCommandsAreDisjoint; /* Invalidated by the ParallelCombineAccesses call */

            if (hasElse)        /* Series combine then and else */
            {
                ret.components = AppendComponentLists (CopyComponentList (thenCommand.components), elseCommand.components);
                ret.wires = AppendWireLists (CopyWireList (thenCommand.wires), elseCommand.wires);
                commandAccesses = SeriesCombineAccesses (thenCommand.accesses, elseCommand.accesses, &ret.components, &ret.wires, position);
                ret.permanent = thenCommand.permanent && elseCommand.permanent;
            } else {
				commandAccesses = thenCommand.accesses;
			}
            /* Parallel combine commands with expression, use a variable if needs be, or an if? */
            exprAndCommandsAreDisjoint = !VariableParallelAccessConflictInAccessList (expr.accesses, commandAccesses);

            ret.components = AppendComponentLists (CopyComponentList (expr.components), ret.components);
            ret.wires = AppendWireLists (CopyWireList (expr.wires), ret.wires);
            ret.accesses = (exprAndCommandsAreDisjoint
              ? ParallelCombineAccesses (commandAccesses,
                expr.accesses,
                &ret.components,
                &ret.wires, position, NULL, false)
              : SeriesCombineAccesses (commandAccesses, expr.accesses, &ret.components, &ret.wires, position));
            ret.activation = NewSyncWireWithAccesses (1, /* Correct this later */ ret.accesses, position);
            ret.wires = NewWireList (ret.activation, ret.wires);
            /* Permanent should already be set */
            {                   /* Use a select */
                PtrWire fetchOut = NewWire (1 /* fix with renum */ , 1, BitTypeObj, 0,
                  false /* Push side of fetch */ , position);

                ret.wires = NewWireList (fetchOut, ret.wires);

                if (exprAndCommandsAreDisjoint) /* Just place a fetch */
                {
                    ret.components = NewComponentList (NewFetchComponent (position, false, ret.activation, expr.activation, fetchOut), ret.components); /* The fetch component nearest the select */
                } else          /* need a variable/seq/2xfetch */
                    PlaceActiveBuffer (ret.activation, expr.activation, fetchOut /* Onto select */ ,
					  expr.accesses,  commandAccesses, "guard", &ret.wires, &ret.components, position);

                if (hasElse)
                {
                    ret.components = NewComponentList (NewCaseComponent (position, StrDup ("0;1"), fetchOut, NewWireList ( /* 0 */ elseCommand.
                          activation, NewWireList ( /* 1 */
                            thenCommand.activation, NULL))), ret.components);
                } else
                {
                    ret.components = NewComponentList (NewCaseComponent (position, StrDup ("1"), fetchOut, NewWireList ( /* 1 */ thenCommand.
                          activation, NULL)), ret.components);
                }
            }
            RenumberWireList (ret.wires, false);
            return ret;
        } else
            /* guardCount > 1, use Bar/If */
        {
            /* Need to place bar tree, connect expr/command activations to tree, (||el combine expr's, ; combine commands and
               expr's combined accesses with combined commands.  ie. (; (append (; commands) (|| exprs))) */
            CommandAttributes ret = NoCommandAttributes;
            PtrExprAttributesList exprAttr = newExprs;
            PtrCommandAttributesList commandAttr = newCommands;
            PtrAccessList exprAccesses = NULL;
            PtrAccessList commandAccesses = NULL;
            PtrWire barExpr, barCommand;

            /* Parallel combine expressions, series combine commands */
            while (exprAttr)
            {
                ret.components = AppendComponentLists (ret.components,
                  AppendComponentLists (CopyComponentList (CAR (exprAttr).components), CopyComponentList (CAR (commandAttr).components)));
                ret.wires =
                  AppendWireLists (ret.wires, AppendWireLists (CopyWireList (CAR (exprAttr).wires), CopyWireList (CAR (commandAttr).wires)));
                exprAccesses = ParallelCombineAccesses (exprAccesses, CAR (exprAttr).accesses, &ret.components, &ret.wires, position, NULL, false);
                commandAccesses = SeriesCombineAccesses (commandAccesses, CAR (commandAttr).accesses, &ret.components, &ret.wires, position);
                commandAttr = CDR (commandAttr);
                exprAttr = CDR (exprAttr);
            }
            ret.accesses = SeriesCombineAccesses (exprAccesses, commandAccesses, &ret.components, &ret.wires, position);

            /* Mix in the else clause */
            if (hasElse)        /* Series combine then and else */
            {
                ret.components = AppendComponentLists (ret.components, elseCommand.components);
                ret.wires = AppendWireLists (ret.wires, elseCommand.wires);
                ret.accesses = SeriesCombineAccesses (ret.accesses, elseCommand.accesses, &ret.components, &ret.wires, position);
            }
            /* Place Bar component, barExpr, barCommand hole the boolean guard value and command activation wires */
            /* FIXME, permanence? */
            {
                /* Place a single Bar component for the expr/command sets */
                PtrWireList exprWires = NULL;
                PtrWireList commandWires = NULL;

                exprAttr = newExprs;
                commandAttr = newCommands;

                while (exprAttr) /* Iterate across exprs/commands, build expr and command wires lists */
                {
                    exprWires = NewWireList (CAR (exprAttr).activation, exprWires);
                    commandWires = NewWireList (CAR (commandAttr).activation, commandWires);
                    commandAttr = CDR (commandAttr);
                    exprAttr = CDR (exprAttr);
                }
                barExpr = NewWire (1, 1, BitTypeObj, 0, true /* Pull */ , position);
                barCommand = NewSyncWireWithAccesses (1, commandAccesses, position);
                ret.wires = NewWireList (barCommand, NewWireList (barExpr, ret.wires));

                ret.components = NewComponentList (NewBarComponent (position, barExpr, barCommand, exprWires, commandWires), ret.components);
            }
            /* Place select at top level (with variable to simulate if behaviour) */
            /* FIXME, check if ActiveBuffer is needed */
            {
                PtrWire selectIn = NewWire (1 /* fix with renum */ , 1, BitTypeObj, 0,
                  false /* Push side of fetch */ , position);

                ret.activation = NewSyncWireWithAccesses (1, /* Correct this later */ ret.accesses, position);
                ret.wires = NewWireList (ret.activation, NewWireList (selectIn, ret.wires));
                PlaceActiveBuffer (ret.activation, barExpr, selectIn, 
					exprAccesses, commandAccesses,	
					"guard", &ret.wires, &ret.components, position);

                if (hasElse)
                {
                    ret.components = NewComponentList (NewCaseComponent (position, StrDup ("0;1"), selectIn, NewWireList ( /* 0 */ elseCommand.
                          activation, NewWireList ( /* 1 */
                            barCommand, NULL))), ret.components);
                } else
                {
                    ret.components = NewComponentList (NewCaseComponent (position, StrDup ("1"), selectIn, NewWireList ( /* 1 */ barCommand, NULL)),
                      ret.components);
                }
            }
            RenumberWireList (ret.wires, false);
            return ret;
        }
    }

    return NoCommandAttributes;
}

/* HandleWhileCommand : handle the placement of a while component and the connection of guards */
CommandAttributes
HandleWhileCommand (WhileLoopNature loopType, struct Context * context,
  PtrCommandAttributesList commands, PtrExprAttributesList exprs, CommandAttributes command1, CommandAttributes command2, tPosition position)
{
    PtrCommandAttributesList newCommands;
    PtrExprAttributesList newExprs;
    PtrCommandAttributesList constantOneAttr;
    int guardCount;

    if (loopType == CommandWhileExprLoop) /* repeat while is an exception, has no real guards */
        return HandleCommandWhileExprCommand (CAR (exprs), command1, position);
    else
        /* This shouldn't be necessary any more (except for the constantOneAttr), but do it anyway */
        guardCount = ReduceGuardList (commands, exprs, &newCommands, &newExprs, &constantOneAttr, position);

    if (guardCount == -1)       /* Manifestly true single guard, else will fail */
    {
        switch (loopType)
        {
        case WhileGuardsLoop:
        case CommandWhileGuardsLoop:
            {
                CommandAttributes seqCommand = HandleSequentialCommand (command1, CAR (constantOneAttr),
                  position);

                return HandleLoopCommand (seqCommand, position);
            }
            break;
        case WhileGuardsAlsoLoop:
            {
                CommandAttributes seqCommand = HandleSequentialCommand (CAR (constantOneAttr), command1,
                  position);

                return HandleLoopCommand (seqCommand, position);
            }
            break;
        case CommandWhileGuardsAlsoLoop: /* Do: pre ; guardCommand ; post */
            {
                CommandAttributes seqCommand = HandleSequentialCommand (command1,
                  HandleSequentialCommand (CAR (constantOneAttr),
                    command2,
                    position),
                  position);

                return HandleLoopCommand (seqCommand, position);
            }
            break;
        default:
            break;
        }
    } else if ((loopType != WhileGuardsLoop && !command1.activation) || (loopType == CommandWhileGuardsAlsoLoop && !command2.activation)) /* test other commands */
        return NoCommandAttributes;
    else
    {
        if (guardCount == 0)    /* No guards, all failed therefore place a Continue (or use the else command) */
        {
            switch (loopType)
            {
            case WhileGuardsLoop:
            case WhileGuardsAlsoLoop:
                return HandleContinueCommand (position);
                break;
            case CommandWhileGuardsLoop:
            case CommandWhileGuardsAlsoLoop:
                return HandleLoopCommand (command1, position);
                break;
            default:
                break;
            }
        } else
            /* 1 or more guards */
        {
            /* Need to place bar tree, connect expr/command activations to tree, (||el combine expr's, ; combine commands and
               expr's combined accesses with combined commands.  ie. (; (append (; commands) (|| exprs))) */
            CommandAttributes ret = NoCommandAttributes;
            PtrExprAttributesList exprAttr = newExprs;
            PtrCommandAttributesList commandAttr = newCommands;
            PtrAccessList exprAccesses = NULL;
            PtrAccessList commandAccesses = NULL;
            PtrWire barExpr, barCommand;

            ret.components = NULL;
            ret.wires = NULL;
            /* Parallel combine expressions, series combine commands */
            while (exprAttr)
            {
                /* Give up if one of the commands was invalid */
                if (!CAR (commandAttr).activation)
                    return NoCommandAttributes;

                ret.components = AppendComponentLists (ret.components,
                  AppendComponentLists (CopyComponentList (CAR (exprAttr).components), CopyComponentList (CAR (commandAttr).components)));
                ret.wires =
                  AppendWireLists (ret.wires, AppendWireLists (CopyWireList (CAR (exprAttr).wires), CopyWireList (CAR (commandAttr).wires)));
                exprAccesses = ParallelCombineAccesses (exprAccesses, CAR (exprAttr).accesses, &ret.components, &ret.wires, position, NULL, false);
                commandAccesses = SeriesCombineAccesses (commandAccesses, CAR (commandAttr).accesses, &ret.components, &ret.wires, position);
                commandAttr = CDR (commandAttr);
                exprAttr = CDR (exprAttr);
            }
            /* Place Bar component, barExpr, barCommand hole the boolean guard value and command activation wires */

            /* FIXME, permanence? */
            if (guardCount == 1)
            {
                barExpr = CAR (newExprs).activation;
                barCommand = CAR (newCommands).activation;
            } else
            {
                /* Place a single Bar component for the expr/command sets */
                PtrWireList exprWires = NULL;
                PtrWireList commandWires = NULL;

                exprAttr = newExprs;
                commandAttr = newCommands;

                while (exprAttr) /* Iterate across exprs/commands, build expr and command wires lists */
                {
                    exprWires = NewWireList (CAR (exprAttr).activation, exprWires);
                    commandWires = NewWireList (CAR (commandAttr).activation, commandWires);
                    commandAttr = CDR (commandAttr);
                    exprAttr = CDR (exprAttr);
                }
                barExpr = NewWire (1, 1, BitTypeObj, 0, true /* Pull */ , position);
                barCommand = NewSyncWireWithAccesses (1, commandAccesses, position);
                ret.wires = NewWireList (barCommand, NewWireList (barExpr, ret.wires));
                ret.components = NewComponentList (NewBarComponent (position, barExpr, barCommand, exprWires, commandWires), ret.components);
            }
            /* Place select at top level, assume that we need the buffer, change while
               behaviour to latch? */
            {
                if (loopType != WhileGuardsLoop) /* Include command 1 */
                {
                    ret.wires = AppendWireLists (ret.wires, command1.wires);
                    ret.components = AppendComponentLists (ret.components, command1.components);
                    commandAccesses = SeriesCombineAccesses (commandAccesses, command1.accesses,
                    	&ret.components, &ret.wires, position);
                }

                if (loopType == CommandWhileGuardsAlsoLoop) /* Include command2 too */
                {
                    ret.wires = AppendWireLists (ret.wires, command2.wires);
                    ret.components = AppendComponentLists (ret.components, command2.components);
                    commandAccesses = SeriesCombineAccesses (commandAccesses, command2.accesses,
                    	&ret.components, &ret.wires, position);
                }

				ret.accesses = SeriesCombineAccesses (exprAccesses, commandAccesses, &ret.components, &ret.wires, position);
                ret.activation = NewSyncWireWithAccesses (1, /* Correct this later */ ret.accesses, position);
                ret.wires = NewWireList (ret.activation, ret.wires);

                switch (loopType)
                {
                case WhileGuardsLoop:
                    ret.components = NewComponentList (NewWhileComponent (position, ret.activation, barExpr, barCommand), ret.components);
                    break;
                case WhileGuardsAlsoLoop:
                    {
                        PtrWire doAct = NewSyncWireWithAccesses (1, commandAccesses, position); /* activation to Seq (barCommand, command1) */
                        PtrComponent sequenceComponent = NewSequenceComponent (position, doAct, barCommand,
                          command1.activation);

                        ret.components = NewComponentList (sequenceComponent,
                          NewComponentList (NewWhileComponent (position, ret.activation, barExpr, doAct), ret.components));
                        ret.wires = NewWireList (doAct, ret.wires);
                    }
                    break;
                case CommandWhileGuardsLoop:
                case CommandWhileGuardsAlsoLoop:
                    {
                        /* Place two sequences and a Call, one for the incoming activation and one for the bar activation */
                        PtrWire doAct = NewSyncWireWithAccesses (1, commandAccesses, position); /* activation to Seq (command12, barCommand) */
                        PtrWire whileAct = NewSyncWireWithAccesses (1, ret.accesses, position); /* activation to While */
                        PtrWire command1Act1 = NewSyncWireWithAccesses (1, command1.accesses, position); /* pre command1 activation */
                        PtrWire command1Act2 = NewSyncWireWithAccesses (1, command1.accesses, position); /* loop    "        "      */
                        PtrComponent preSequenceComponent = NewSequenceComponent (position, ret.activation, command1Act1,
                          whileAct);
                        PtrComponent callComponent = NewCallComponent (position, command1Act1, command1Act2,
                          command1.activation);

                        ret.components = NewComponentList (preSequenceComponent,
                          NewComponentList (callComponent, NewComponentList (NewWhileComponent (position, whileAct, barExpr, doAct), ret.components)));

                        ret.wires = NewWireList (doAct, NewWireList (whileAct, NewWireList (command1Act1, NewWireList (command1Act2, ret.wires))));

                        if (loopType == CommandWhileGuardsLoop) /* loopSeq is: guards ; command1 */
                        {
                            PtrComponent loopSequenceComponent = NewSequenceComponent (position, doAct, barCommand,
                              command1Act2);

                            ret.components = NewComponentList (loopSequenceComponent, ret.components);
                        } else
                        {       /* loopSeq is: guards ; command2 ; command1 */
                            PtrWire loopSequenceComponent2Act = NewSyncWireWithAccesses (1, commandAccesses, position);
                            PtrComponent loopSequenceComponent = NewSequenceComponent (position, doAct, barCommand,
                              loopSequenceComponent2Act);
                            PtrComponent loopSequenceComponent2 = NewSequenceComponent (position, loopSequenceComponent2Act,
                              command2.activation,
                              command1Act2);

                            ret.components = NewComponentList (loopSequenceComponent, NewComponentList (loopSequenceComponent2, ret.components));
                            ret.wires = NewWireList (loopSequenceComponent2Act, ret.wires);
                        }
                    }
                    break;
                default:
                    break;
                }
            }
            RenumberWireList (ret.wires, false);
            return ret;
        }
    }

    return NoCommandAttributes;
}

/* HandleCommandWhileExprCommand : handle compiling a loop ... while loop into a while construct */
CommandAttributes HandleCommandWhileExprCommand (ExprAttributes guard, CommandAttributes command, tPosition position)
{
    CommandAttributes ret = NoCommandAttributes;

    if (!TypeEquivalence (guard.value.type, BitTypeObj))
        LOG_ERROR (GuardExprMustHaveBitType, NoIdent, position);
    else if (!command.activation) /* The command must be valid */
    {
    } else if (guard.value.value.baseValue) /* Manifestly t/f guard */
    {
        if (guard.value.value.dontCares)
            LOG_ERROR (ImplicantNotAllowedHere, NoIdent, position);
        else if (mpz_sgn (guard.value.value.baseValue) == 0) /* false guard, just Command */
            ret = command;
        else                    /* true guard, just loop */
            ret = HandleLoopCommand (command, position);
    } else
    {                           /* Variable guard */
        /* Series combine guard and command */
        ret.components = AppendComponentLists (CopyComponentList (guard.components), command.components);
        ret.wires = AppendWireLists (CopyWireList (guard.wires), command.wires);
        ret.permanent = command.permanent;
        ret.accesses = SeriesCombineAccesses (guard.accesses, command.accesses, &ret.components, &ret.wires, position);
        {
			/* Need something like this:
			   Sequence (activation, firstAct, whileAct)
			   While (whileAct, guard.activation, secondAct)
			   Call (firstAct, secondAct, command.activation)
			 */
			PtrWire firstAct = NewSyncWireWithAccesses (1, command.accesses, position);
			PtrWire secondAct = NewSyncWireWithAccesses (1, command.accesses, position);
			PtrWire whileAct = NewSyncWireWithAccesses (1, ret.accesses, position);
			PtrWire activation = NewSyncWireWithAccesses (1, ret.accesses, position);
			PtrComponent sequenceComponent = NewSequenceComponent (position, activation, firstAct, whileAct);
			PtrComponent callComponent = NewCallComponent (position, firstAct, secondAct, command.activation);
			PtrComponent whileComponent = NewWhileComponent (position, whileAct, guard.activation, secondAct);

			/* Add new components ... */
			ret.wires = NewWireList (firstAct, NewWireList (secondAct,
				NewWireList (whileAct, NewWireList (activation, ret.wires))));
			ret.components = NewComponentList (sequenceComponent, NewComponentList (callComponent,
				NewComponentList (whileComponent, ret.components)));
			ret.activation = activation;
		}
    }
    return ret;
}

/* HandleAssignmentCommand : handle the placement of a fetch for the AssignmentCommand */
CommandAttributes HandleAssignmentCommand (ExprAttributes lvalue, PtrInstance lvalueInstance, ExprAttributes expr, tPosition position)
{
    CommandAttributes ret = NoCommandAttributes;

    if (!lvalueInstance)
    {
    }
    /* Fail silently */
    else if (lvalueInstance->nature != VariableInstance /* Not A channel */ )
        LOG_ERROR (ExpectingAVariableOnLHS, NoIdent, position);
    /* Fail if either lvalue or expr have no types */
    else if (expr.value.type != NoType && lvalue.value.type != NoType)
    {
        /* Type equivalence */
        if (!TypeEquivalence (expr.value.type, lvalue.value.type))
            LOG_ERROR (ExpressionsMustHaveSameType, NoIdent, position);
        /* A valid, resolved channel ? */
        else if (lvalue.activation) /* Yes */
        {
            PtrWire fetchActivate;

            if (!expr.activation)
            {
                TypedValueIsDCFree (expr.value, position);
                return NoCommandAttributes;
            }

            fetchActivate = NewSyncWire (1 /* fixed later */, position);
            ret.wires = NewWireList (fetchActivate, AppendWireLists (CopyWireList (lvalue.wires), expr.wires));
            ret.activation = fetchActivate;
            ret.components = AppendComponentLists (CopyComponentList (lvalue.components), expr.components);

            if (VariableParallelAccessConflictInAccessList (lvalue.accesses, expr.accesses))
            {
                char *auxName;
                int auxLength = 0;

                if (lvalue.activation->ident == NoIdent)
                    auxName = "aux";
                else
                {
                    const char *auxStr = PeekString (lvalue.activation->ident);

                    auxLength = strlen (auxStr) + 5;
                    auxName = NEW_ARRAY (char, auxLength);

                    sprintf (auxName, "aux:%s", auxStr);
                }
                PlaceActiveBuffer (fetchActivate, expr.activation, lvalue.activation,
					expr.accesses, lvalue.accesses, auxName, &ret.wires, &ret.components, position);
                ret.accesses = SeriesCombineAccesses (lvalue.accesses, expr.accesses, &ret.components, &ret.wires, position);
                if (auxLength)
                    FREE_ARRAY (char, auxLength, auxName);
            } else
            {
                ret.components = NewComponentList (NewFetchComponent (position, false, fetchActivate, expr.activation, lvalue.activation), ret.components);
                ret.accesses = ParallelCombineAccesses (lvalue.accesses, expr.accesses, &ret.components, &ret.wires, position, NULL, false);
            }
            RenumberWireList (ret.wires, true); /* resequence wires */
            ret.activation->accesses = ret.accesses;
        }
    }
    return ret;
}

/* HandleInputCommand : handle the placement of a fetch for the InputCommand for a variable
	on the right hand side */
CommandAttributes
HandleInputCommand (ExprAttributes chanAttr, PtrInstance chanInstance, ExprAttributes varAttr, PtrInstance varInstance, tPosition position)
{
    CommandAttributes ret = NoCommandAttributes;

    if (!chanInstance || !varInstance || !varAttr.activation)
    {                           /* Fail silently */
    } else if (chanInstance->nature == VariableInstance /* Not A channel */ )
        LOG_ERROR (ExpectingAReadableChannel, NoIdent, position);
    else if (varInstance->nature != VariableInstance)
        LOG_ERROR (ExpectingAVariableOnRHS, NoIdent, position);
    else if (chanInstance->type->nature == ArrayedType)
        LOG_ERROR (NotExpectingAnArrayedChannel, NoIdent, position);
    /* Fail if either lvalue or expr have no types */
    else if (chanAttr.value.type != NoType && varAttr.value.type != NoType)
    {
        /* Type equivalence */
        if (!TypeEquivalence (chanAttr.value.type, varAttr.value.type))
            LOG_ERROR (ExpressionsMustHaveSameType, NoIdent, position);
        /* Not a valid, resolved channel ? */
        else if (chanAttr.activation)
        {
            PtrWire fetchActivate = NewSyncWire (1 /* fixed later */, position);

            ret.wires = NewWireList (fetchActivate, AppendWireLists (CopyWireList (chanAttr.wires), varAttr.wires));
            ret.activation = fetchActivate;
            ret.components =
              NewComponentList (NewFetchComponent
              (position, false, fetchActivate, chanAttr.activation,
                varAttr.activation), AppendComponentLists (CopyComponentList (chanAttr.components), varAttr.components));
            ret.accesses = ParallelCombineAccesses (chanAttr.accesses, varAttr.accesses, &ret.components, &ret.wires, position, NULL, false);
            RenumberWireList (ret.wires, false); /* resequence wires */
            ret.activation->accesses = ret.accesses;
        } else
        {
            LOG_ERROR (ChannelIsUnresolved, NoIdent, position);
        }
    }
    return ret;
}

/* HandleInputToChannelCommand : handle the placement of a fetch for the InputCommand with a
	channel on the right hand side */
CommandAttributes
HandleInputToChannelCommand (ExprAttributes LHSAttr, PtrInstance LHSInstance, ExprAttributes RHSAttr, PtrInstance RHSInstance, tPosition position)
{
    CommandAttributes ret = NoCommandAttributes;

    if (!LHSInstance || !RHSInstance)
    {                           /* Fail silently */
    } else if (LHSInstance->nature == VariableInstance /* Not A channel */ )
        LOG_ERROR (ExpectingAReadableChannel, NoIdent, position); /* FIXME, better error messages */
    else if (RHSInstance->nature == VariableInstance)
        LOG_ERROR (ExpectingAWritableChannel, NoIdent, position);
    else if (LHSInstance->type->nature == ArrayedType || RHSInstance->type->nature == ArrayedType)
        LOG_ERROR (NotExpectingAnArrayedChannel, NoIdent, position);
    /* Fail if either lvalue or expr have no types */
    else if (LHSAttr.value.type != NoType && RHSAttr.value.type != NoType)
    {
        /* Type equivalence */
        if (!TypeEquivalence (LHSAttr.value.type, RHSAttr.value.type))
            LOG_ERROR (ExpressionsMustHaveSameType, NoIdent, position);
        /* Not valid, resolved channels ? */
        else if (LHSAttr.activation && RHSAttr.activation)
        {
            PtrWire fetchActivate = NewSyncWire (1 /* fixed later */, position);

            ret.wires = NewWireList (fetchActivate, AppendWireLists (CopyWireList (LHSAttr.wires), RHSAttr.wires));
            ret.activation = fetchActivate;
            ret.components =
              NewComponentList (NewFetchComponent
              (position, true, fetchActivate, LHSAttr.activation,
                RHSAttr.activation), AppendComponentLists (CopyComponentList (LHSAttr.components), RHSAttr.components));
            ret.accesses = ParallelCombineAccesses (LHSAttr.accesses, RHSAttr.accesses, &ret.components, &ret.wires, position, NULL, false);
            RenumberWireList (ret.wires, false); /* resequence wires */
            ret.activation->accesses = ret.accesses;
        } else
        {
            LOG_ERROR (ChannelIsUnresolved, NoIdent, position);
        }
    }
    return ret;
}

/* HandleOutputCommand : handle the placement of a fetch for the OutputCommand */
CommandAttributes HandleOutputCommand (ExprAttributes lvalue, PtrInstance lvalueInstance, ExprAttributes expr, tPosition position)
{
    CommandAttributes ret = NoCommandAttributes;

    if (!lvalueInstance)
    {
    }
    /* Fail silently */
    else if (lvalueInstance->nature == VariableInstance /* Not A channel */ )
        LOG_ERROR (ExpectingAWritableChannel, NoIdent, position);
    else if (lvalueInstance->type->nature == ArrayedType)
        LOG_ERROR (NotExpectingAnArrayedChannel, NoIdent, position);
    /* Fail if either lvalue or expr have no types */
    else if (expr.value.type != NoType && lvalue.value.type != NoType)
    {
        /* Type equivalence */
        if (!TypeEquivalence (expr.value.type, lvalue.value.type))
            LOG_ERROR (ExpressionsMustHaveSameType, NoIdent, position);
        /* Not a valid, resolved channel ? */
        else if (lvalue.activation)
        {
            PtrWire fetchActivate;

            if (!expr.activation)
            {
                TypedValueIsDCFree (expr.value, position);
                return NoCommandAttributes;
            }

            fetchActivate = NewSyncWire (1 /* fixed later */ , position);
            ret.wires = NewWireList (fetchActivate, AppendWireLists (CopyWireList (lvalue.wires), expr.wires));
            ret.activation = fetchActivate;
            ret.components =
              NewComponentList (NewFetchComponent
              (position, false, fetchActivate, expr.activation, lvalue.activation), AppendComponentLists (CopyComponentList (lvalue.components),
                expr.components));
            ret.accesses = ParallelCombineAccesses (lvalue.accesses, expr.accesses, &ret.components, &ret.wires, position, NULL, false);
            RenumberWireList (ret.wires, false); /* resequence wires */
            ret.activation->accesses = ret.accesses;
        } else
        {
            LOG_ERROR (ChannelIsUnresolved, NoIdent, position);
        }
    }
    return ret;
}

/* MakeFVsForChannelAccesses: make a set of FVs for a set of channel accesses in
	`guardChannelAccesses' whose `->inst' fields correspond to allocated read-only
	variables (for channel inputs).  The function also makes the transferrers for
	active-input communications, the Synch tree for synchronising FV `signal'
	outputs and NullAdapts and for inputs whose values are never used.
	Expects: *accesses to already contain a series combined set of accesses to the
		channel R/O vars.
	Other arguments: activeInput - processing an active-input command, not a select/arbitrate
		disallowSync - don't allow sync ``inputs''
		useEagerFVs = generate {Passive|Active|PassiveSync}EagerFalseVariable components instead of normal FVs
		accesses, components, wires - running CommandAttributes to be modified by func.
	Returns: a command activation wire, and (for active-inputs) an activation wire in
		`*activation' (if `activation' is not NULL) which is Forked to all the input
		transferrers.
*/
PtrWire
MakeFVsForChannelAccesses (PtrAccessList guardChannelAccesses,
  bool activeInput, bool disallowSync, bool useEagerFVs,
  PtrAccessList * accesses, PtrComponentList * components, PtrWireList * wires, PtrWire * activation, tPosition position)
{
    PtrWireList inputFetchActivations = NULL;
    int inputFetchActivationCount = 0;
    PtrWireList commandActivations = NULL;
    int commandActivationCount = 0;
    PtrWire completeCommandActivation = NULL;

    while (guardChannelAccesses)
    {
        PtrAccess guardChannelAccess = CAR (guardChannelAccesses);
        PtrWire fvarWriteWire = guardChannelAccess->info.channel.wire;

        /* CAR(guardChannelAccesses) should be a channel access */
        /* Filter accesses to this fvar into fvarAccess moding *accesses */
        PtrInstance thisChannel = guardChannelAccess->inst;
        Bits width = Abs (thisChannel->type->size);
        PtrAccess fvarAccess = (guardChannelAccess->info.channel.nature != ChannelPassiveWrite ? FilterOutAccessToInstance (accesses,
            thisChannel->selectionInfo.fvar) : NULL);

        /* For passive in/output and eager active input, the FV is connected directly to the channel,
           for active input in non-eager mode, the FV is attached to the output of a Fetch */
        if (activeInput && !useEagerFVs)
        {
            PtrWire fetchActivation = NewSyncWire (1 /* fixup later */ , position);

            fvarWriteWire = CopyWire (guardChannelAccess->info.channel.wire);
            fvarWriteWire->isPull = false; /* output of fetch */
            inputFetchActivations = NewWireList (fetchActivation, inputFetchActivations);
            inputFetchActivationCount++;

            *wires = NewWireList (fetchActivation, *wires);
            *wires = NewWireList (fvarWriteWire, *wires);
            *components =
              NewComponentList (NewFetchComponent (position, false, fetchActivation, guardChannelAccess->info.channel.wire, fvarWriteWire), *components);
        }

        if (disallowSync && thisChannel->type->size == 0) /* Sync */
        {
            LOG_ERROR (CannotUseSyncInEncloseCommand, NoIdent, position);
            return NULL;
        }

        /* Unlock the channel for the outside world */
        thisChannel->locked = false;
        /* Inputs/syncs: Make the FVAR components, connect read wires (using fvarAccess) */
        if (!fvarAccess || mpz_sgn (fvarAccess->info.variable.readMap) == 0)
            /* No accesses, use the FVAR input channel via a n->0 adapter to generate the
               completeCommandActivation, also for sync */
        {
            PtrWire partialCommandActivation = NULL; /* activation coming off back end of FVAR */

            if (thisChannel->type->size == 0) /* Sync */
            {                   /* No adapt, connect */
                if (!useEagerFVs)
                    partialCommandActivation = fvarWriteWire;
                else
                {
                    g_assert (!activeInput);

                    /* Prepare fvActivation wire for passiveSyncEagerFalseVariable */
                    PtrWire fvActivation = NewSyncWire (1 /* fixup later */ , position);

                    inputFetchActivations = NewWireList (fvActivation, inputFetchActivations);
                    inputFetchActivationCount++;
                    *wires = NewWireList (fvActivation, *wires);

                    /* Prepare commandActivation wire for passiveSyncEagerFalseVariable */
                    partialCommandActivation = NewSyncWire (1, position);
                    *wires = NewWireList (partialCommandActivation, *wires);

                    PtrComponent newComponent = NewPassiveSyncEagerFalseVariableComponent (position, fvActivation, fvarWriteWire, partialCommandActivation);

                    *components = NewComponentList (newComponent, *components);
                }
            } else
            {                   /* Null Adapt */
                PtrComponent newComponent;

                if (!useEagerFVs)
                {
                    /* Prepare partialCommandActivation wire for NullAdapt */
                    partialCommandActivation = NewSyncWire (1 /* fixup later */ , position);
                    *wires = NewWireList (partialCommandActivation, *wires);

                    newComponent = NewNullAdaptComponent (position, partialCommandActivation, fvarWriteWire);
                } else
                {
                    /* Prepare fvActivation wire for (passive or active) EagerFalseVariable */
                    PtrWire fvActivation = NewSyncWire (1 /* fixup later */ , position);

                    inputFetchActivations = NewWireList (fvActivation, inputFetchActivations);
                    inputFetchActivationCount++;
                    *wires = NewWireList (fvActivation, *wires);

                    /* Prepare commandActivation wire for (passive or active) EagerFalseVariable */
                    partialCommandActivation = NewSyncWire (1, position);
                    *wires = NewWireList (partialCommandActivation, *wires);

                    if (activeInput)
                        newComponent =
                          NewActiveEagerNullAdaptComponent (position,  /*0, thisChannel->type->size, */ fvActivation, fvarWriteWire, partialCommandActivation);
                    else
                        newComponent =
                          NewPassiveEagerNullAdaptComponent (position,  /*0, thisChannel->type->size, */ fvActivation, fvarWriteWire, partialCommandActivation);
                }
                *components = NewComponentList (newComponent, *components);
            }
            commandActivations = NewWireList (partialCommandActivation, commandActivations);
            commandActivationCount++;
        } else
        {
            /* Check variable accesses first, complain about non overlapping accesses,
               if we have a record type then reduce the read/write Maps to exclude
               the pad field */
            PtrAccess access = NULL;
            bool *partitions;
            PtrComponentList variables, variablesIter;

            /* Add a write access to the fvarAccess */
            access = CombineVariables (NewVariableAccess (thisChannel, fvarWriteWire), fvarAccess, position, false /* sequential */ , false);

            /* noVariableReads or balanced access */
            partitions = CreateVariablePartitions (access->info.variable.wires, width);
            variables =
              CreatePartitionedVariableComponents (NoIdent, partitions, width, true, useEagerFVs /* makeEagerFalseVariables */ ,
              activeInput /* eagerFVsAreActive */ , NULL, NoPosition);

            CreateWriteConnections (variables, access->info.variable.wires, wires, components, position);
            CreateReadConnections (variables, access->info.variable.wires->next, wires, components, position);

            variables = TrimDanglingVariables (variables);

            /* OK, make the probe connections */
            variablesIter = variables;
            while (variablesIter)
            {
                if ((CAR (variablesIter)->nature == FalseVariableComponent) || (CAR (variablesIter)->nature == PassiveEagerFalseVariableComponent)
                  || (CAR (variablesIter)->nature == ActiveEagerFalseVariableComponent))
                {
                    PtrWire partialCommandActivation = NewSyncWire (1, position);

                    if (CAR (variablesIter)->nature == FalseVariableComponent)
                    {
                        CDR (CAR (variablesIter)->ports)->body = partialCommandActivation;
                        UpdateWire (partialCommandActivation, true /* active */ , CAR (variablesIter), 1);
                    } else
                    {
                        CDDR (CAR (variablesIter)->ports)->body = partialCommandActivation;
                        UpdateWire (partialCommandActivation, true /* active */ , CAR (variablesIter), 2);
                    }
                    *wires = NewWireList (partialCommandActivation, *wires);
                    commandActivations = NewWireList (partialCommandActivation, commandActivations);
                    commandActivationCount++;

                    if (useEagerFVs)
                    {
                        PtrWire fetchActivation = NewSyncWire (1 /* fixup later */ , position);

                        CAR (variablesIter)->ports->body = fetchActivation;
                        UpdateWire (fetchActivation, false /* passive */ , CAR (variablesIter), 0);

                        inputFetchActivations = NewWireList (fetchActivation, inputFetchActivations);
                        inputFetchActivationCount++;

                        *wires = NewWireList (fetchActivation, *wires);
                    }
                }
                variablesIter = CDR (variablesIter);
            }

            *components = AppendComponentLists (variables, *components);
        }
        guardChannelAccesses = CDR (guardChannelAccesses);
    }

    /* Make activation Fork for active-input */
    if (inputFetchActivations)
    {
        if (inputFetchActivationCount == 1)
        {
            if (activation)
                *activation = CAR (inputFetchActivations);
        } else
        {                       /* Place a Fork */
            PtrWire inputActivation = NewSyncWire (1 /* later */ , position);
            PtrComponent fork = NewForkComponent (position, inputActivation, NULL, NULL);

            /* Update wires and modify Synch ports */
            fork->ports = NewWireList (inputActivation, inputFetchActivations);
            UpdateWires (inputFetchActivations, true /* active */ , fork, 1,
              -1);

            *components = NewComponentList (fork, *components);
            *wires = NewWireList (inputActivation, *wires);
            if (activation)
                *activation = inputActivation;
        }
    }

    /* Make command-activation-Synch for all cases */
    if (commandActivations)
    {
        if (commandActivationCount == 1)
        {
            completeCommandActivation = CAR (commandActivations);
        } else
        {                       /* Place a Synch */
            PtrComponent synch;

            completeCommandActivation = NewSyncWire (1 /* later */ , position);
            synch = NewSynchComponent (position, NULL, NULL, completeCommandActivation);

            /* Update wires and modify Synch ports */
            UpdateWires (commandActivations, false /* passive */ , synch, 0,
              -1);
            UpdateWire (completeCommandActivation, true /* active */ , synch,
              commandActivationCount);
            synch->ports = AppendWireLists (commandActivations, NewWireList (completeCommandActivation, NULL));

            *components = NewComponentList (synch, *components);
            *wires = NewWireList (completeCommandActivation, *wires);
        }
    }

    return completeCommandActivation;
}

/* HandleSelectAndArbitrateCommands : handle the joining of components/wires/accesses, resolution of
   FalseVariable accesses and placing of a DecisionWait, Arbiter for the select or arbitrate command
   doArbitrate == false -> select, doArbitrate == true -> arbitrate */
CommandAttributes
HandleSelectAndArbitrateCommands (PtrCommandAttributesList commands, PtrExprAttributesList guards, bool doArbitrate, tPosition position)
{
    CommandAttributes ret = NoCommandAttributes;
    PtrCommandAttributesList commandIter;
    PtrExprAttributesList guardIter = guards;
    int guardCount = LengthOfCommandAttributesList (commands);

    ret.permanent = true;
    /* Check for too many guards in arbitrate */
    if (doArbitrate && guardCount > 2)
    {
        LOG_ERROR (CantDoMoreThan2WayArbitrate, NoIdent, position);
        return NoCommandAttributes;
    }
    /* Series combine command accesses/wires/components */
    /* Parallel combine guard (FVAR write side) accesses */
    commandIter = commands;
    while (commandIter)
    {
        if (CAR (commandIter).activation && CAR (guardIter).accesses)
        {
            ret.components = AppendComponentLists (ret.components, CopyComponentList (CAR (commandIter).components));
            ret.wires = AppendWireLists (ret.wires, AppendWireLists (CopyWireList (CAR (commandIter).wires), CopyWireList (CAR (guardIter).wires)));
            ret.accesses = SeriesCombineAccesses (ret.accesses, CAR (commandIter).accesses, &ret.components, &ret.wires, position);
            ret.accesses = SeriesCombineAccesses (ret.accesses, CAR (guardIter).accesses, &ret.components, &ret.wires, position);
            /* Permanent unless any command isn't */
            if (!CAR (commandIter).permanent)
                ret.permanent = false;
        } else
            return ret;         /* Fall out if any commands are not complete */
        guardIter = CDR (guardIter);
        commandIter = CDR (commandIter);
    }
   	{ 
        PtrWireList commandActivations = NULL;

		guardIter = guards;
        while (guardIter)
        {
            PtrWire probe = NULL; /* Probe for this guard, join onto this */
            PtrAccessList guardChannelAccesses = CAR (guardIter).accesses;

            probe =
              MakeFVsForChannelAccesses (guardChannelAccesses, false, false, false /*eagerMode */ , &ret.accesses, &ret.components, &ret.wires, NULL,
              position);
            /* Add probe to commandActivations */
            commandActivations = NewWireList (probe, commandActivations);
            guardIter = CDR (guardIter);
        }

        /* Place command activations, dec wait, top level activation */
        {
            PtrCommandAttributesList commandsIter = commands;
            PtrWireList commandSideOfDWActivations = NULL; /* Activations on the command side of the dec wait */
            PtrWireList commandSideOfARBActivations = NULL;

            /* For ARB: guards -> ARB -> DW -> commands */
            /* Make a list of activations */
            while (commandsIter)
            {
                if (doArbitrate)
                {
                    PtrWire newARBtoDWwire = NewSyncWire (1 /* later */ , position);

                    commandSideOfARBActivations = NewWireList (newARBtoDWwire, commandSideOfARBActivations);
                }
                commandSideOfDWActivations = NewWireList (CAR (commandsIter).activation, commandSideOfDWActivations);
                commandsIter = CDR (commandsIter);
            }

            /* OK, place DW, ARB */
            ret.activation = NewSyncWireWithAccesses (1, ret.accesses, position);
            if (doArbitrate && guardCount > 1)
            {
                PtrWire tmp1 = NewSyncWire (1, position);
                PtrWire tmp2 = NewSyncWire (1, position);

                ret.wires = NewWireList (tmp1, NewWireList (tmp2, ret.wires));
                /* guards -> ARB -> DW -> commands */
                ret.components =
                  NewComponentList (NewArbiterComponent (position, CAR (commandActivations), CADR (commandActivations), tmp1, tmp2), ret.components);
                ret.components =
                  NewComponentList (NewDecisionWaitComponent
                  (position, ret.activation, NewWireList (tmp1, NewWireList (tmp2, NULL)), commandSideOfDWActivations), ret.components);
            } else
            {
                ret.components =
                  NewComponentList (NewDecisionWaitComponent (position, ret.activation, commandActivations, commandSideOfDWActivations), ret.components);
            }
            ret.wires = NewWireList (ret.activation, ret.wires);
        }
    }
    RenumberWireList (ret.wires, false); /* resequence wires */
    return ret;
}

/* HandleSelectBangCommand : handle the joining of components/wires/accesses and resolution of
   PassiveEagerFalseVariable accesses for the select! command */
CommandAttributes HandleSelectBangCommand (PtrCommandAttributesList commands, PtrExprAttributesList guards, tPosition position)
{
    CommandAttributes ret = NoCommandAttributes;
    int guardCount = LengthOfCommandAttributesList (commands);
    CommandAttributes command = CAR (commands);
    ExprAttributes guard = CAR (guards);

    /* Check for too many guards */
    if (guardCount > 1)
    {
        LOG_ERROR (CantDoMoreThan1WayEagerFalseVariable, NoIdent, position);
        return NoCommandAttributes;
    }

    /* Permanent unless command isn't */
    ret.permanent = command.permanent;

    /* Series combine command accesses/wires/components */
    /* Parallel combine guard (FVAR write side) accesses */
    if (command.activation && guard.accesses)
    {
        ret.components = AppendComponentLists (ret.components, CopyComponentList (command.components));
        ret.wires = AppendWireLists (ret.wires, AppendWireLists (CopyWireList (command.wires), CopyWireList (guard.wires)));
        ret.accesses = SeriesCombineAccesses (ret.accesses, command.accesses, &ret.components, &ret.wires, position);
        ret.accesses = SeriesCombineAccesses (ret.accesses, guard.accesses, &ret.components, &ret.wires, position);
    } else
        return ret;             /* Fall out if any commands are not complete */

    {
        PtrWire probe = NULL;   /* Probe for this guard, join onto this */
        PtrAccessList guardChannelAccesses = guard.accesses;

        ret.activation = NewSyncWire (1, position);

        probe = MakeFVsForChannelAccesses (guardChannelAccesses,
          false, false, true /*eagerMode */ , &ret.accesses, &ret.components, &ret.wires, &ret.activation, position);

        ret.activation->accesses = ret.accesses;

        PtrComponent mainEagerFVComp = probe->activeComponent;

        if (mainEagerFVComp)
        {                       /* Attempt at connecting the PassiveEagerFalseVariable to the command */
            /* Join the commandActivation to commandAttr.activation with a Fork1 */
            ret.components = NewComponentList (NewForkConnectorComponent (position, probe, command.activation), ret.components);
        }
    }
    RenumberWireList (ret.wires, false); /* resequence wires */
    return ret;
}

/* HandleInputEncloseCommand : handle the active input enclosure `channel -> then command end'
	command.  Replace the read-only variable references in the command accesses with access
	to synthesised FalseVariables on the guard channels. The eagerMode option is to handle
        the '->!' construct, which is an optimised '->' */
CommandAttributes HandleInputEncloseCommand (ExprAttributes guardChannelAttrs, CommandAttributes commandAttr, bool eagerMode, tPosition position)
{
    CommandAttributes ret = NoCommandAttributes;
    PtrWire commandActivation = NULL; /* activation from FV's to command */

    if (!commandAttr.activation || !guardChannelAttrs.accesses)
        return NoCommandAttributes;

    ret = commandAttr;          /* Start with the command */
    /* Series combine guard (FVAR write side) accesses */
    ret.wires = AppendWireLists (ret.wires, CopyWireList (guardChannelAttrs.wires));
    ret.accesses = SeriesCombineAccesses (ret.accesses, guardChannelAttrs.accesses, &ret.components, &ret.wires, position);

    commandActivation =
      MakeFVsForChannelAccesses (guardChannelAttrs.accesses, true /* active input */ , true /* disallow sync */ , eagerMode /* eagerFVs */ ,
      &ret.accesses, &ret.components, &ret.wires, &ret.activation, position);

    if (!commandActivation)     // Happens in case of input enclosure on a sync channel (Error correctly generated by MakeFVsForChannelAccesses)
        return ret;

    /* Join the commandActivation to commandAttr.activation with a Fork1 */
    ret.components = NewComponentList (NewForkConnectorComponent (position, commandActivation, commandAttr.activation), ret.components);

    return ret;
}

#define CASE_VALUE_STR_LEN (1024)

/* HandleCaseCommand : handle the command combination, component generations yada yada yada for
   the case command, hasElse enables the use of the arg elseAttr as an else clause. */
CommandAttributes
HandleCaseCommand (PtrCommandAttributesList commands,
  PtrImplicantListList implicantss,
  PtrImplicantList complementImplicants, ExprAttributes expr, bool hasElse, CommandAttributes elseAttr, tPosition position)
{
    CommandAttributes ret = NoCommandAttributes;
    PtrCommandAttributesList commIter = commands;
    PtrWireList commandActivations = NULL;
    PtrWireList commandActivationsTail = NULL;
	PtrAccessList commandAccesses = NULL;

    if (!expr.activation)
        return ret;             /* Fail silently */

    if (!hasElse && (!implicantss || !commands))
    {
        LOG_ERROR (CaseCommandHasNoValidGuards, NoIdent, position);
        return ret;
    }

    /* Single range, constant expr, must execute */
    if (expr.value.value.baseValue)
    {
        if (expr.value.value.dontCares)
        {
            LOG_ERROR (ImplicantNotAllowedHere, NoIdent, position);
            return NoCommandAttributes;
        }
        if (implicantss && !CDR (implicantss))
            return CAR (commands);
        if (hasElse)
            return elseAttr;
    }

    if (!hasElse && complementImplicants) /* has some uncovered values */
    {
        Implicant firstImplicant = CAR (complementImplicants);
        char caseValue[CASE_VALUE_STR_LEN];

        *caseValue = '\0';
        MakeTypedValueString (caseValue, NewTypedValue (firstImplicant.baseValue, expr.value.type));
        LOG_ERROR (SomeCaseValuesNotCovered, MakeIdent1 (caseValue), position);
    }

    /* Assume permanence, prove me wrong */
    ret.permanent = true;

    /* Combine else clause */
    if (hasElse && elseAttr.activation)
    {
        if (!complementImplicants) /* No uncovered values, else never used */
        {
            LOG_ERROR (AllCaseValuesCoveredIgnoringElse, NoIdent, position);
            hasElse = false;
        } else
            commIter = NewCommandAttributesList (elseAttr, commIter);
    }
    /* Combine commands */
    while (commIter)
    {
        if (CAR (commIter).activation)
        {                       /* Ignore NULL activation commands */
            ret.wires = AppendWireLists (CopyWireList (CAR (commIter).wires), ret.wires);
            ret.components = AppendComponentLists (CopyComponentList (CAR (commIter).components), ret.components);
            commandAccesses = SeriesCombineAccesses (CAR (commIter).accesses, commandAccesses, &ret.components, &ret.wires, position);
            ret.permanent = ret.permanent && CAR (commIter).permanent;

            if (!commandActivations)
                commandActivations = commandActivationsTail = NewWireList (CAR (commIter).activation, NULL);
            else
            {
                commandActivationsTail->next = NewWireList (CAR (commIter).activation, NULL);
                commandActivationsTail = CDR (commandActivationsTail);
            }
        }
        commIter = CDR (commIter);
    }

    /* Place Fetch and Case components */
    {
        /* Combine expr */
        /* Parallel combine commands with expression, use a variable if needs be */
        bool exprAndCommandsAreDisjoint = !VariableParallelAccessConflictInAccessList (expr.accesses,
          ret.accesses);

        ret.components = AppendComponentLists (CopyComponentList (expr.components), ret.components);
        ret.wires = AppendWireLists (CopyWireList (expr.wires), ret.wires);
        ret.accesses = (exprAndCommandsAreDisjoint
          ? ParallelCombineAccesses (commandAccesses, expr.accesses,
            &ret.components, &ret.wires,
            position, NULL, false)
		  : SeriesCombineAccesses (commandAccesses, expr.accesses, &ret.components, &ret.wires, position));

        /* Place components */
        ret.activation = NewSyncWireWithAccesses (1, /* Correct this later */ ret.accesses, position);
        ret.wires = NewWireList (ret.activation, ret.wires);
        {
            PtrWire fetchOut = CopyAndRenumberWire (expr.activation, 1, position);

            fetchOut->isPull = false; /* Make the wire push */

            ret.wires = NewWireList (fetchOut, ret.wires);

            if (exprAndCommandsAreDisjoint) /* Just place a fetch */
            {
                ret.components = NewComponentList (NewFetchComponent (position, false, ret.activation, expr.activation, fetchOut), ret.components); /* The fetch component nearest the case */
            } else
                /* need a variable/seq/2xfetch */
            {
                PlaceActiveBuffer (ret.activation, expr.activation, fetchOut /* Onto case */ ,
				  expr.accesses, commandAccesses,
                  "guard", &ret.wires, &ret.components, position);
            }

            ret.components =
              NewComponentList (NewCaseComponent
              (position, MakeImplicantCaseSpecification
                ((hasElse ?
                    NewImplicantListList (complementImplicants,
                      implicantss) : implicantss), WireWidth (fetchOut)), fetchOut, commandActivations), ret.components);
        }
    }
    RenumberWireList (ret.wires, false); /* resequence wires */
    return ret;
}

/* HandleUnSharedProcedureCallCommand : handle unshared procedure calls */
CommandAttributes HandleUnSharedProcedureCallCommand (PtrExprAttributesList arguments, PtrProcedure procedure, tPosition position)
{
    CommandAttributes ret = NoCommandAttributes;

    bool placeComponents = !procedure->isExternal && (FlattenProcedureCalls || procedure->context->depth > TopLevelContextDepth);

    /* Check that the arg list was the right length (or not cut short by other error) */
    if (LengthOfExprAttributesList (arguments) == procedure->portCount)
    {
        if (!placeComponents)
        {                       /* Place a ProcedureComponent then attach expressions to it */
            PtrComponent procComp = NewProcedureComponent (position, procedure->ident, procedure);
            PtrExprAttributesList attr = arguments;
            PtrInstanceList ports = procedure->ports;
            unsigned wireCount = 1;
            unsigned portArrayIndex = 1;
            PtrWireList wires = NULL;

            /* Copy some things from function to Handshake Component */
            procComp->param.procedure.parameters = NULL;
            procComp->param.procedure.portSpec = procedure->portSpec;
            {
                PtrInstance activatePort = NewChannelInstance (MakeIdent1 ("activate"), SyncTypeObj);

                procComp->param.procedure.ports = NewInstanceList (activatePort, procedure->ports);

                /* Make an activation port too */
                activatePort->nature = InputChannelInstance;
                activatePort->info.port.sense = PassivePortSense;
                activatePort->info.port.wire = NULL;
                activatePort->info.port.options = NULL;
                activatePort->info.port.position = NoPosition;
            }

            while (attr)
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
                if (CAR (ports)->type->nature == ArrayedType)
                {
                    if (portArrayIndex == CAR (ports)->type->info.arrayed.elementCount)
                    {
                        portArrayIndex = 1;
                        ports = CDR (ports);
                    } else
                        portArrayIndex++;
                } else
                    ports = CDR (ports);
                attr = CDR (attr);
            }
            /* Make up the activation wire */
            ret.activation = NewSyncWireWithAccesses (1 /* fix later */ , ret.accesses, position);
            UpdateWire (ret.activation, false /* passive */ , procComp, 0);
            /* Complete ProcedureComponent's wires */
            wires = NewWireList (ret.activation, wires);
            procComp->ports = wires;
            /* Add procComp to components, activation to wires */
            ret.wires = NewWireList (ret.activation, ret.wires);
            ret.components = NewComponentList (procComp, ret.components);
            ret.permanent = procedure->info.procedure.attributes.permanent;
            RenumberWireList (ret.wires, false); /* resequence wires */
        } else
        {
            PtrExprAttributesList attr = arguments;
            PtrInstanceList ports = procedure->ports;
            unsigned wireListLength;
            PtrWireArray portsInFunction;
            unsigned portArrayIndex = 1;
            unsigned portCount = 1; /* FIXME, check this, modified in anger 25/11/97 */

            /* Copy the whole deal */
            ret = DeepCopyCommandAttributes (procedure->info.procedure.attributes);

            /* CallContext: Add some debug information to the wires */
            CallContext_DuplicateWirePositions (ret.wires, procedure->ident, position, attr, procedure->info.procedure.attributes.wires);

            wireListLength = LengthOfWireList (ret.wires);
            /* Rely on the fact that procAttr's wires can be considered to be an array */
            portsInFunction = WIRE_LIST_TO_ARRAY (ret.wires, wireListLength);
            /* Remove wires from the placed procedure in favour of the activation on the argument */
            while (attr && ports)
            {
                /* Transfer connection from old procedure argument wire to activation of this argument */
                if (CAR (ports)->info.port.sense != PassivePortSense)
                {               /* Active port, connected to active component in procedure */
                    UpdateWire (CAR (attr).activation, true /* active */ ,
                      CAR (portsInFunction + portCount)->activeComponent, CAR (portsInFunction + portCount)->activePort);
                    SubstituteWireForWireInWireList (CAR
                      (portsInFunction + portCount)->activeComponent->ports, CAR (portsInFunction + portCount), CAR (attr).activation);
                } else
                {               /* Passive port, connected to passive component in procedure */
                    UpdateWire (CAR (attr).activation, false /* passive */ ,
                      CAR (portsInFunction + portCount)->passiveComponent, CAR (portsInFunction + portCount)->passivePort);
                    SubstituteWireForWireInWireList (CAR
                      (portsInFunction + portCount)->passiveComponent->ports, CAR (portsInFunction + portCount), CAR (attr).activation);
                }
                /* Mark the original port wire for deletion */
                MarkWireWithReplacement (CAR (portsInFunction + portCount), CAR (attr).activation);

                /* Combine the arguments wires/accesses/components */
                ret.wires = AppendWireLists (ret.wires, CAR (attr).wires);

                ret.accesses = ParallelCombineAccesses (ret.accesses, CAR (attr).accesses, &ret.components, &ret.wires, position, NULL, false);
                ret.components = AppendComponentLists (ret.components, CAR (attr).components);
                portCount++;
                if (CAR (ports)->type->nature == ArrayedType)
                {
                    if (portArrayIndex == CAR (ports)->type->info.arrayed.elementCount)
                    {
                        portArrayIndex = 1;
                        ports = CDR (ports);
                    } else
                        portArrayIndex++;
                } else
                    ports = CDR (ports);
                attr = CDR (attr);
            }
            /* Remove all the original ports from the function, numberOfPorts + 1 is the
               index of the first local wire: local -> ports -> activation in list order
               which is 0:act,1:port,2:port,...,numberOfPorts+1:local,numberOfPorts+2:local in the array */
            /* portsInFunction[numberOfPorts + 1].next = portsInFunction; */
            ret.wires = SweepWireList (ret.wires);
        }
        RenumberWireList (ret.wires, false);
    }
    return ret;
}

/* HandleSharedProcedureCallCommand : handle shared procedure calls */
CommandAttributes HandleSharedProcedureCallCommand (PtrExprAttributesList arguments, PtrProcedure procedure, tPosition position)
{
    CommandAttributes ret = NoCommandAttributes;

    if (arguments)
        LOG_ERROR (CannotHaveArgumentsToSharedProcedure, procedure->ident, position);
    else
    {
        /* Make an activation wire, include the accesses internal to the shaerd block */
        ret.activation = NewSyncWire (1, position);
        ret.wires = NewWireList (ret.activation, NULL);
        {                       /* The dummy wires and components are just for the parallel combine of accesses, which
                                   shouldn't produce any wires or components */
            PtrComponentList dummyComps = NULL;
            PtrWireList dummyWires = NULL;

            ret.accesses =
              ParallelCombineAccesses (NewAccessList
              (NewSharedCallAccess (procedure, ret.activation), NULL), procedure->unplacedAccesses, &dummyComps, &dummyWires, position, NULL, false);
            ret.activation->accesses = ret.accesses;
        }
        ret.permanent = procedure->info.procedure.attributes.permanent;
    }
    return ret;
}

/* HandleProcedureCallCommand : handle the placement of components/parts for a procedure call */
CommandAttributes HandleProcedureCallCommand (PtrExprAttributesList arguments, PtrProcedure procedure, tPosition position)
{
    CommandAttributes ret = NoCommandAttributes;

    if (procedure != NoProcedure)
    {
        switch (procedure->nature)
        {
        case SharedProcedure:
            ret = HandleSharedProcedureCallCommand (arguments, procedure, position);
            break;
        case UnSharedProcedure:
            ret = HandleUnSharedProcedureCallCommand (arguments, procedure, position);
            break;
        default:               /* Can't happen */
            break;
        }
    }
    return ret;
}

#define PRINT_STR_LEN (8192)
/* HandlePrintCommand : handle the `print Expr Exprs' command which is used for reporting
   parameterisation messages.  If `mustBeCompileTime' then default to `report' printing type
   if run time printing is the default and don't allow runtime printing */
CommandAttributes
HandlePrintCommand (PtrContext context, ExprAttributes expr, PtrExprAttributesList exprs, bool mustBeCompileTime, tPosition position)
{
    CommandAttributes ret = NoCommandAttributes;
    Ptrchar str = NEW_ARRAY (char, PRINT_STR_LEN); /* FIXME, fixed length buffer? */
    Ptrchar strTail;
    ErrorType errorType = DefaultPrintErrorType;

    if (expr.value.type == NoType) /* Problem with expr */
        return NoCommandAttributes; /* fail silently */

    /* An error report? */
    if (expr.value.type == ErrorTypeObj)
    {
        switch (mpz_get_ui (expr.value.value.baseValue))
        {
        case 4:
            errorType = ReportPrintCommand;
            break;
        case 3:
            errorType = WarningPrintCommand;
            break;
        case 2:
            errorType = ErrorPrintCommand;
            break;
        case 1:
            errorType = FatalPrintCommand;
            break;
        case 0:
            errorType = RuntimePrintCommand;
            break;
        }
    } else
        exprs = NewExprAttributesList (expr, exprs);

    if (mustBeCompileTime && errorType == RuntimePrintCommand)
        errorType = ReportPrintCommand;

    strTail = str;
    *str = '\0';

    if (errorType != RuntimePrintCommand)
    {
        while (exprs)
        {                       /* Not NoType, must be constant */
            if (CAR (exprs).value.type == StringTypeObj || CAR (exprs).value.value.baseValue)
            {
                strTail = MakeTypedValueString (strTail, CAR (exprs).value);
            } else
            {
                LOG_ERROR (ExpressionMustBeConstant, NoIdent, (CAR (exprs).activation ? CAR (exprs).activation->position : position));
            }
            strTail = EndOfString (strTail);
            exprs = CDR (exprs);
        }
        LOG_ERROR (errorType, MakeIdent1 (str), position);
        FREE_ARRAY (char, PRINT_STR_LEN, str);

        ret = HandleContinueCommand (position);
    } else
    {
        /* newExprs is a stacked up set of ExprAttributes of the calls to ToString or $BrzString
           suitable for processing into a StringAppend tree */
        PtrExprAttributesList newExprs = NULL;

        /* Check to see if we *must not* emit HCs */
        if (mustBeCompileTime)
        {
            LOG_ERROR (CannotReportRuntimeErrorHere, NoIdent, position);
            return NoCommandAttributes;
        }

        while (exprs)
        {
            if (CAR (exprs).value.value.baseValue)
            {
                strTail = MakeTypedValueString (strTail, CAR (exprs).value);
            } else if (CAR (exprs).activation)
            {
                ExprAttributes attr;

                if (str != strTail) /* Make Print for trailing text */
                {
                    newExprs = NewExprAttributesList (HandleStringExpr (StrDup (str), position), newExprs);

                    /* Reset accumulated string */
                    strTail = str;
                    *strTail = '\0';
                }

                /* Don't need to ToString a string */
                if (CAR (exprs).value.type == StringTypeObj)
                    attr = CAR (exprs);
                else
                {
                    /* Now handle the non constant string expr */
                    PtrType paramType = AliasType (CAR (exprs).value.type, MakeIdent1 ("X"));
                    tTree functionTree = CopyTree (ToStringFunction->info.parameterisedFunction.tree);

                    attr =
                      HandleFunctionCallExpr (NewExprAttributesList
                      (CAR (exprs), NULL),
                      SpecifyParameterisedBuiltinFunction
                      (context, ToStringFunction->ident,
                        ToStringFunction, functionTree,
                        NULL, NewTypeList (paramType, NULL), functionTree->BuiltinFunctionDecl.FormalPorts->FormalPort.next,
                        /* mExprFuncParam (position, mNullFuncParams (NoPosition),
                           mPrimedExpr (position, CAR (exprs))), */
                        NewComponentParameterList
                        (NewComponentParameter (TypeComponentParameter, paramType, paramType->ident, NoType), NULL), position), position);
                }

                newExprs = NewExprAttributesList (attr, newExprs);
            }
            strTail = EndOfString (strTail);
            exprs = CDR (exprs);
        }

        if (str != strTail)     /* Trailing text */
        {
            newExprs = NewExprAttributesList (HandleStringExpr (StrDup (str), position), newExprs);
        }

        {
            ExprAttributes stringAppendExpr;

            if (newExprs)
            {
                stringAppendExpr = CAR (newExprs);
                newExprs = CDR (newExprs);

                /* Iterate down newExprs and do StringAppend (e3 StringAppend (e2, e1))
                   for a newExprs of [e1, e2, e3].  Note that newExprs is reversed and so this
                   will result in a correctly ordered append */
                while (newExprs)
                {
                    stringAppendExpr =
                      HandleFunctionCallExpr (NewExprAttributesList
                      (CAR (newExprs), NewExprAttributesList (stringAppendExpr, NULL)), StringAppendFunction, position);

                    newExprs = CDR (newExprs);
                }
            } else
            {
                stringAppendExpr = HandleStringExpr ("", position);
            }

            /* Place a WriteMessage function and transfer it's result onto a sink
               need to find a prettier way of doing it */
            CommandAttributes writeMessageCall = HandleSinkCommand (HandleFunctionCallExpr (NewExprAttributesList (stringAppendExpr, NULL),
                WriteMessageFunction, position), position);

            ret.activation = writeMessageCall.activation;
            /* Add in expression to result */
            ret.wires = AppendWireLists (writeMessageCall.wires, ret.wires);
            ret.components = AppendComponentLists (writeMessageCall.components, ret.components);
            ret.accesses =
              ParallelCombineAccesses (writeMessageCall.accesses, ret.accesses,
              &ret.components, &ret.wires, position, NULL, false /* not permissive */ );
        }
        FREE_ARRAY (char, PRINT_STR_LEN, str);
    }
    return ret;
}

/* HandleSinkCommand : handle the `sink Expr Exprs' command which is used to call functions and
   ignore the return value */
CommandAttributes HandleSinkCommand (ExprAttributes expr, tPosition position)
{
    CommandAttributes ret = NoCommandAttributes;

    if (expr.value.type != NoType) /* Problem with expr */
    {
        ret.activation = NewSyncWireWithAccesses (1, expr.accesses, position);

        PtrWire continueInputWire = NewWire (1, ABS (expr.value.type->size), expr.value.type,
          0, false /* push */ , position);
        PtrComponent transferrer = NewFetchComponent (position, false, ret.activation, expr.activation,
          continueInputWire);
        PtrComponent continuePush = NewContinuePushComponent (position, continueInputWire);

        ret.wires = NewWireList (ret.activation, NewWireList (continueInputWire, expr.wires));
        ret.components = NewComponentList (transferrer, NewComponentList (continuePush, expr.components));
        ret.permanent = false;
        ret.accesses = expr.accesses;
    }

    return ret;
}

/* DeepCopyCommandAttributes : copy a command attribute set reproducing the
   wires (in order) and the components and all the interwoven connectivity */
CommandAttributes DeepCopyCommandAttributes (CommandAttributes attr)
{
    CommandAttributes ret;      /* 5 elements: components, accesses, wires, activation, permanent */
    PtrWireArray newWireArray;
    unsigned wireCount = LengthOfWireList (attr.wires);

    /* Array copy makes an array/list of wires where CAR(array[n-1]) is the wire n */
    newWireArray = ArrayDeepCopyWireList (attr.wires);
    ret.permanent = attr.permanent;
    ret.components = DeepCopyComponentList (attr.components); /* Need to fix up ports */
    ret.activation = newWireArray[0].body;
    ret.wires = WIRE_ARRAY_TO_LIST (newWireArray, wireCount); /* array/list duality ;) */
    ret.accesses = CopyAndRemapWiresOfAccessList (attr.accesses, newWireArray);

    /* Fix up component ports */
    {
        PtrComponentList compIter = ret.components;
        PtrComponentList origCompIter = attr.components; /* Original (pre copy) list iterator */

        while (compIter)
        {
            PtrWireList wires = CAR (compIter)->ports;

            while (wires)
            {
                BundleNumber wireIndex = CAR (wires)->bundleNo - 1;

                /* Fix up wire active/passive component */
                if (CAR (wires)->activeComponent == CAR (origCompIter))
                {
                    newWireArray[wireIndex].body->activeComponent = CAR (compIter);
                } else
                {
                    newWireArray[wireIndex].body->passiveComponent = CAR (compIter);
                }
                /* Replace the wire entry */
                wires->body = newWireArray[wireIndex].body;
                wires = CDR (wires);
            }
            origCompIter = CDR (origCompIter);
            compIter = CDR (compIter);
        }
    }
    return ret;
}

/* StrCommandAttributes{,List} : print functions */
void StrCommandAttributes (FILE * stream, CommandAttributes attr)
{
    fprintf (stream, "(COMPS ");
    StrPtrSBreezeComponentList (stream, attr.components, ",");
    fprintf (stream, ", ACCESSES ");
    StrPtrAccessList (stream, attr.accesses);
    fprintf (stream, ", WIRES ");
    StrPtrWireList (stream, attr.wires, true, ",", true);
    fprintf (stream, ", ACTIVATE ");
    StrPtrWire (stream, attr.activation);
    fprintf (stream, ", %sPERMANENT ", (attr.permanent ? "" : "NOT "));
    fprintf (stream, ")");
}

void StrPtrCommandAttributesList (FILE * stream, PtrCommandAttributesList list, char *separator)
{
    while (list)
    {
        StrCommandAttributes (stream, CAR (list));
        if (CDR (list))
            fprintf (stream, "%s", separator);
        list = CDR (list);
    }
}
