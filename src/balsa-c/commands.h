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

     `commands.h'
     Command compilation handling functions 

   */

#ifndef COMMANDS_HEADER
#define COMMANDS_HEADER

#include <stdio.h>
#include "misclists.h"
#include "accesses.h"
#include "components.h"
#include "exprs.h"
#include "values.h"
#include "implicants.h"

/* Forward decls */
struct Context;
struct Procedure;

/* CommandAttributes : data elements connected to commands during compilation */
typedef struct CommandAttributes
{
    PtrComponentList components;
    PtrAccessList accesses;
    PtrWireList wires;
    PtrWire activation;         /* activation wire (sync, push) */
    bool permanent;             /* true: command never returns ack, false: does */
}
CommandAttributes;

DECLARE_LIST_TYPE (CommandAttributes, CommandAttributes)
DECLARE_LIST_TYPE_CONSTRUCTOR (CommandAttributes, CommandAttributes) DECLARE_LIST_TYPE_LENGTHOF (CommandAttributes)
/* NoCommandAttributes : Default value, all NULL and No### */
extern CommandAttributes NoCommandAttributes;

/* Initialise */
extern void BeginCommands (void);

/* HandleContinueCommand : create a continue component and a wire */
extern CommandAttributes HandleContinueCommand (tPosition position);

/* HandleHaltCommand : create a halt component and a wire */
extern CommandAttributes HandleHaltCommand (tPosition position);

/* HandleSequentialCommand : handle the merging of accesses/components etc. on ; operator */
extern CommandAttributes HandleSequentialCommand (CommandAttributes left, CommandAttributes right, tPosition position);

/* HandleParallelCommand : ditto de || operator */
extern CommandAttributes HandleParallelCommand (CommandAttributes left, CommandAttributes right, bool isPermissive, tPosition position);

/* HandleLoopCommand : create a Loop component connected to command */
extern CommandAttributes HandleLoopCommand (CommandAttributes command, tPosition position);

/* HandleSyncCommand : create an access on a sync channel */
extern CommandAttributes HandleSyncCommand (struct Context *context, ExprAttributes chanAttr, PtrInstance chanInstance, tPosition position);

/* WhileLoopNature : types of while loop */
typedef enum WhileLoopNature
{
    WhileGuardsLoop,            /* loop while <guards> end loop */
    WhileGuardsAlsoLoop,        /* loop while <guards> also <command> end */
    CommandWhileExprLoop,       /* loop <command> while <expr> end */
    CommandWhileGuardsLoop,     /* loop <command> while <guards> end */
    CommandWhileGuardsAlsoLoop, /* loop <command> while <guards> also <command> end */
}
WhileLoopNature;

/* HandleWhileCommand : handle the placement of a while component and the connection of guards */
extern CommandAttributes HandleWhileCommand (WhileLoopNature loopType,
  struct Context *context,
  PtrCommandAttributesList commands, PtrExprAttributesList exprs, CommandAttributes command1, CommandAttributes command2, tPosition position);

/* HandleCommandWhileExprCommand : handle compiling a loop ... while loop into a while construct */
extern CommandAttributes HandleCommandWhileExprCommand (ExprAttributes guard, CommandAttributes command, tPosition position);

/* HandleIfCommand : handle the placement of an if or select component set */
extern CommandAttributes HandleIfCommand (bool hasElse,
  struct Context *context, PtrCommandAttributesList commands, PtrExprAttributesList exprs, CommandAttributes elseCommand, tPosition position);

/* HandleAssignmentCommand : handle the placement of a fetch for the AssignmentCommand */
extern CommandAttributes HandleAssignmentCommand (ExprAttributes lvalue, PtrInstance lvalueInstance, ExprAttributes expr, tPosition position);

/* HandleInputCommand : handle the placement of a fetch for the InputCommand for a variable
	on the right hand side */
extern CommandAttributes HandleInputCommand (ExprAttributes chanAttr,
  PtrInstance chanInstance, ExprAttributes varAttr, PtrInstance varInstance, tPosition position);

/* HandleInputToChannelCommand : handle the placement of a fetch for the InputCommand with a
	channel on the right hand side */
extern CommandAttributes HandleInputToChannelCommand (ExprAttributes
  LHSAttr, PtrInstance LHSInstance, ExprAttributes RHSAttr, PtrInstance RHSInstance, tPosition position);

/* HandleOutputCommand : handle the placement of a fetch for the OutputCommand */
extern CommandAttributes HandleOutputCommand (ExprAttributes lvalue, PtrInstance lvalueInstance, ExprAttributes expr, tPosition position);

/* HandleSelectAndArbitrateCommands : handle the joining of components/wires/accesses, resolution of
   FalseVariable accesses and placing of a DecisionWait, Arbiter for the select or arbitrate command
   doArbitrate == false -> select, doArbitrate == true -> arbitrate */
extern CommandAttributes HandleSelectAndArbitrateCommands (PtrCommandAttributesList commands, PtrExprAttributesList guards, bool doArbitrate,
  tPosition position);

/* HandleSelectBangCommand : handle the joining of components/wires/accesses and resolution of
   PassiveEagerFalseVariable accesses for the select! command */
extern CommandAttributes HandleSelectBangCommand (PtrCommandAttributesList commands, PtrExprAttributesList guards, tPosition position);

/* HandleInputEncloseCommand : handle the active input enclosure `channel -> then command end'
	command.  Replace the read-only variable references in the command accesses with access
	to synthesised FalseVariables on the guard channels. The eagerMode option is to handle
        the '->!' construct, which is an optimised '->' */
extern CommandAttributes HandleInputEncloseCommand (ExprAttributes guardChannelAttrs, CommandAttributes commandAttr, bool eagerMode,
  tPosition position);

/* HandleArbitrateCommand : handle the joining of components/wires/accesses, resolution of
   FalseVariable accesses and placing of a DecisionWait for the select command */
extern CommandAttributes HandleArbitrateCommand (PtrCommandAttributesList commands, PtrExprAttributesList guards, tPosition position);

/* HandleCaseCommand : handle the command combination, component generations yada yada yada for
   the case command, hasElse enables the use of the arg elseAttr as an else clause. */
extern CommandAttributes HandleCaseCommand (PtrCommandAttributesList commands, PtrImplicantListList implicants, PtrImplicantList complementImplicants,
  ExprAttributes expr, bool hasElse, CommandAttributes elseAttr, tPosition position);

/* HandleProcedureCallCommand : handle the placement of components/parts for a procedure call */
extern CommandAttributes HandleProcedureCallCommand (PtrExprAttributesList arguments, struct Procedure *procedure, tPosition position);

/* HandlePrintCommand : handle the `print Expr Exprs' command which is used for reporting
   parameterisation messages.  If `mustBeCompileTime' then default to `report' printing type
   if run time printing is the default and don't allow runtime printing */
extern CommandAttributes HandlePrintCommand (struct Context *context,
  ExprAttributes expr, PtrExprAttributesList exprs, bool mustBeCompileTime, tPosition position);

/* HandleSinkCommand : handle the `sink Expr Exprs' command which is used to call functions and
   ignore the return value */
extern CommandAttributes HandleSinkCommand (ExprAttributes expr, tPosition position);

/* DeepCopyCommandAttributes : copy a command attribute set reproducing the
   wires (in order) and the components and all the interwoven connectivity */
extern CommandAttributes DeepCopyCommandAttributes (CommandAttributes attr);

/* StrCommandAttributes{,List} : print functions */
extern void StrCommandAttributes (FILE * stream, CommandAttributes attr);
extern void StrPtrCommandAttributesList (FILE * stream, PtrCommandAttributesList list, char *separator);

#endif /* COMMANDS_HEADER */
