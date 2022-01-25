# define DEP(a, b) a
# define SELF yyt
# include "Semantics.h"
# line 55 "Balsa.ag"

  static void yyVisit1 (tTree node);

  /* Export the tree node visiting routine */
  void VisitTreeNode (tTree node)
  {
  	yyVisit1 (node);
  }

# line 326 "Balsa.ag"
 


static char yyb;

char * yyCheck1 = & Tree_module_does_not_match_evaluator_module_25160;
char * yyCheck2 = & generate_Tree_module_without_option_0;

static void yyVisit1 ARGS((register tTree yyt));

void Semantics
# if defined __STDC__ | defined __cplusplus
 (tTree yyt)
# else
 (yyt) tTree yyt;
# endif
{ yyVisit1 (yyt); }

static void yyVisit1
# if defined __STDC__ | defined __cplusplus
 (register tTree yyt)
# else
 (yyt) register tTree yyt;
# endif
{

 switch (yyt->Kind) {

case kBalsaProgram:
# line 524 "Balsa.ag"

	yyt->BalsaProgram.Decls->Decls.scope = TopLevelScope;
# line 523 "Balsa.ag"

	yyt->BalsaProgram.Decls->Decls.skip = false;
# line 526 "Balsa.ag"


	yyt->BalsaProgram.Decls->Decls.contextIn = AddContextMarker (PredefinedContext);
yyVisit1 (yyt->BalsaProgram.Decls);
# line 527 "Balsa.ag"
 yyt->BalsaProgram.context = TopLevelContext = yyt->BalsaProgram.Decls->Decls.contextOut; 
break;

case kDecls:
yyt->Decls.contextOut=yyt->Decls.contextIn;
break;

case kNullDecls:
yyt->NullDecls.contextOut=yyt->NullDecls.contextIn;
break;

case kDecl:
yyt->Decl.next->Decls.scope=yyt->Decl.scope;
yyt->Decl.next->Decls.skip=yyt->Decl.skip;
yyt->Decl.next->Decls.contextIn=yyt->Decl.contextIn;
yyVisit1 (yyt->Decl.next);
yyt->Decl.contextOut=yyt->Decl.next->Decls.contextOut;
break;

case kImportDecl:
yyt->ImportDecl.next->Decls.scope=yyt->ImportDecl.scope;
yyt->ImportDecl.next->Decls.skip=yyt->ImportDecl.skip;
# line 533 "Balsa.ag"

		yyt->ImportDecl.next->Decls.contextIn = yyt->ImportDecl.contextIn;

		if (! yyt->ImportDecl.skip)
		{
			Ptrchar path = MakePathFromDottedPath (yyt->ImportDecl.Idents, '.');

			yyt->ImportDecl.next->Decls.contextIn = HandleImport (path, yyt->ImportDecl.position, yyt->ImportDecl.contextIn, yyt->ImportDecl.scope);
		}	
	
yyVisit1 (yyt->ImportDecl.next);
yyt->ImportDecl.contextOut=yyt->ImportDecl.next->Decls.contextOut;
yyVisit1 (yyt->ImportDecl.Idents);
break;

case kFileDecl:
yyt->FileDecl.next->Decls.scope=yyt->FileDecl.scope;
yyt->FileDecl.next->Decls.skip=yyt->FileDecl.skip;
yyt->FileDecl.next->Decls.contextIn=yyt->FileDecl.contextIn;
yyVisit1 (yyt->FileDecl.next);
yyt->FileDecl.contextOut=yyt->FileDecl.next->Decls.contextOut;
yyVisit1 (yyt->FileDecl.Idents);
break;

case kTypeDecl:
yyt->TypeDecl.next->Decls.scope=yyt->TypeDecl.scope;
yyt->TypeDecl.AType->AType.skip=yyt->TypeDecl.skip;
# line 548 "Balsa.ag"

	yyt->TypeDecl.AType->AType.context = yyt->TypeDecl.contextIn;
yyVisit1 (yyt->TypeDecl.AType);
yyt->TypeDecl.next->Decls.skip=yyt->TypeDecl.skip;
# line 545 "Balsa.ag"

	yyt->TypeDecl.next->Decls.contextIn = (yyt->TypeDecl.skip || yyt->TypeDecl.AType->AType.type == NoType ? yyt->TypeDecl.contextIn : TypeDeclaration (yyt->TypeDecl.contextIn, yyt->TypeDecl.ident, yyt->TypeDecl.scope,
		
		(yyt->TypeDecl.AType->Kind == kExistingType ? AliasType (yyt->TypeDecl.AType->AType.type, NoIdent) : yyt->TypeDecl.AType->AType.type), yyt->TypeDecl.position));
yyVisit1 (yyt->TypeDecl.next);
yyt->TypeDecl.contextOut=yyt->TypeDecl.next->Decls.contextOut;
break;

case kConstantDecl:
yyt->ConstantDecl.next->Decls.scope=yyt->ConstantDecl.scope;
yyt->ConstantDecl.AType->AType.skip=yyt->ConstantDecl.skip;
# line 554 "Balsa.ag"
 yyt->ConstantDecl.AType->AType.context = yyt->ConstantDecl.contextIn;
yyVisit1 (yyt->ConstantDecl.AType);
yyt->ConstantDecl.CoercedExpr->CoercedExpr.skip=yyt->ConstantDecl.skip;
# line 556 "Balsa.ag"

	yyt->ConstantDecl.CoercedExpr->CoercedExpr.allowChannels = false;
# line 555 "Balsa.ag"

	yyt->ConstantDecl.CoercedExpr->CoercedExpr.expectedType = (yyt->ConstantDecl.AType->Kind == kNullType ? NoType : yyt->ConstantDecl.AType->AType.type);
# line 554 "Balsa.ag"

	yyt->ConstantDecl.CoercedExpr->CoercedExpr.context = yyt->ConstantDecl.contextIn;
yyVisit1 (yyt->ConstantDecl.CoercedExpr);
yyt->ConstantDecl.next->Decls.skip=yyt->ConstantDecl.skip;
# line 551 "Balsa.ag"
 
	yyt->ConstantDecl.next->Decls.contextIn =
		(yyt->ConstantDecl.skip ? yyt->ConstantDecl.contextIn : ConstantDeclaration (yyt->ConstantDecl.contextIn, yyt->ConstantDecl.ident, yyt->ConstantDecl.scope, yyt->ConstantDecl.CoercedExpr->CoercedExpr.position,
			(yyt->ConstantDecl.CoercedExpr->CoercedExpr.attributes).value, (yyt->ConstantDecl.AType->Kind == kNullType ? NULL : yyt->ConstantDecl.AType->AType.type), yyt->ConstantDecl.position));
yyVisit1 (yyt->ConstantDecl.next);
yyt->ConstantDecl.contextOut=yyt->ConstantDecl.next->Decls.contextOut;
break;

case kVariableDecl:
yyt->VariableDecl.next->Decls.scope=yyt->VariableDecl.scope;
yyt->VariableDecl.AType->AType.skip=yyt->VariableDecl.skip;
# line 561 "Balsa.ag"

	yyt->VariableDecl.AType->AType.context = yyt->VariableDecl.contextIn;
yyVisit1 (yyt->VariableDecl.AType);
yyVisit1 (yyt->VariableDecl.Idents);
yyt->VariableDecl.next->Decls.skip=yyt->VariableDecl.skip;
# line 559 "Balsa.ag"

	yyt->VariableDecl.next->Decls.contextIn = (yyt->VariableDecl.skip ? yyt->VariableDecl.contextIn :
		VariableDeclaration (yyt->VariableDecl.contextIn, yyt->VariableDecl.Idents->Idents.idents, yyt->VariableDecl.scope, yyt->VariableDecl.AType->AType.type, NULL, yyt->VariableDecl.position));
yyVisit1 (yyt->VariableDecl.next);
yyt->VariableDecl.contextOut=yyt->VariableDecl.next->Decls.contextOut;
break;

case kInitVariableDecl:
yyt->InitVariableDecl.next->Decls.scope=yyt->InitVariableDecl.scope;
yyt->InitVariableDecl.AType->AType.skip=yyt->InitVariableDecl.skip;
# line 582 "Balsa.ag"
 yyt->InitVariableDecl.AType->AType.context = yyt->InitVariableDecl.contextIn;
yyVisit1 (yyt->InitVariableDecl.AType);
yyt->InitVariableDecl.CoercedExpr->CoercedExpr.skip=yyt->InitVariableDecl.skip;
# line 584 "Balsa.ag"

	yyt->InitVariableDecl.CoercedExpr->CoercedExpr.allowChannels = false;
# line 583 "Balsa.ag"

	yyt->InitVariableDecl.CoercedExpr->CoercedExpr.expectedType = (yyt->InitVariableDecl.AType->Kind == kNullType ? NoType : yyt->InitVariableDecl.AType->AType.type);
# line 582 "Balsa.ag"

	yyt->InitVariableDecl.CoercedExpr->CoercedExpr.context = yyt->InitVariableDecl.contextIn;
yyVisit1 (yyt->InitVariableDecl.CoercedExpr);
yyVisit1 (yyt->InitVariableDecl.Idents);
yyt->InitVariableDecl.next->Decls.skip=yyt->InitVariableDecl.skip;
# line 564 "Balsa.ag"

		yyt->InitVariableDecl.next->Decls.contextIn = yyt->InitVariableDecl.contextIn;

		LOG_ERROR (InitVarNotSupported, NoIdent, yyt->InitVariableDecl.position);
		yyt->InitVariableDecl.skip = true;

		if (! yyt->InitVariableDecl.skip)
		{
			if (! TypedValueIsDCFreeConstant ((yyt->InitVariableDecl.CoercedExpr->CoercedExpr.attributes).value, yyt->InitVariableDecl.position))
			{  }
			else if (! TypeEquivalence (yyt->InitVariableDecl.AType->AType.type, (yyt->InitVariableDecl.CoercedExpr->CoercedExpr.attributes).value.type))
				LOG_ERROR (TypeIsNotValid, NoIdent, yyt->InitVariableDecl.position);
			else {
				yyt->InitVariableDecl.next->Decls.contextIn = VariableDeclaration (yyt->InitVariableDecl.contextIn, yyt->InitVariableDecl.Idents->Idents.idents,
					yyt->InitVariableDecl.scope, yyt->InitVariableDecl.AType->AType.type, (yyt->InitVariableDecl.CoercedExpr->CoercedExpr.attributes).value.value.baseValue, yyt->InitVariableDecl.position);
			}
		}
	
yyVisit1 (yyt->InitVariableDecl.next);
yyt->InitVariableDecl.contextOut=yyt->InitVariableDecl.next->Decls.contextOut;
break;

case kChannelDecl:
yyt->ChannelDecl.next->Decls.scope=yyt->ChannelDecl.scope;
yyt->ChannelDecl.AType->AType.skip=yyt->ChannelDecl.skip;
# line 589 "Balsa.ag"

	yyt->ChannelDecl.AType->AType.context = yyt->ChannelDecl.contextIn;
yyVisit1 (yyt->ChannelDecl.AType);
yyVisit1 (yyt->ChannelDecl.Idents);
yyt->ChannelDecl.next->Decls.skip=yyt->ChannelDecl.skip;
# line 587 "Balsa.ag"

	yyt->ChannelDecl.next->Decls.contextIn = (yyt->ChannelDecl.skip ? yyt->ChannelDecl.contextIn :
		ChannelDeclaration (yyt->ChannelDecl.contextIn, yyt->ChannelDecl.Idents->Idents.idents, yyt->ChannelDecl.scope, yyt->ChannelDecl.AType->AType.type, yyt->ChannelDecl.multicast, yyt->ChannelDecl.position));
yyVisit1 (yyt->ChannelDecl.next);
yyt->ChannelDecl.contextOut=yyt->ChannelDecl.next->Decls.contextOut;
break;

case kChannelArrayDecl:
yyt->ChannelArrayDecl.next->Decls.scope=yyt->ChannelArrayDecl.scope;
# line 595 "Balsa.ag"

	yyt->ChannelArrayDecl.Range->Range.isSpan = false;
# line 594 "Balsa.ag"

	yyt->ChannelArrayDecl.Range->Range.expectedType = NoType;
yyt->ChannelArrayDecl.Range->Range.skip=yyt->ChannelArrayDecl.skip;
# line 593 "Balsa.ag"
 
	yyt->ChannelArrayDecl.Range->Range.context = yyt->ChannelArrayDecl.contextIn;
yyVisit1 (yyt->ChannelArrayDecl.Range);
yyt->ChannelArrayDecl.AType->AType.skip=yyt->ChannelArrayDecl.skip;
# line 592 "Balsa.ag"
 
	yyt->ChannelArrayDecl.AType->AType.context = yyt->ChannelArrayDecl.contextIn;
yyVisit1 (yyt->ChannelArrayDecl.AType);
yyVisit1 (yyt->ChannelArrayDecl.Idents);
yyt->ChannelArrayDecl.next->Decls.skip=yyt->ChannelArrayDecl.skip;
# line 596 "Balsa.ag"
 
	yyt->ChannelArrayDecl.next->Decls.contextIn = (yyt->ChannelArrayDecl.skip ? yyt->ChannelArrayDecl.contextIn :
		ChannelArrayDeclaration (yyt->ChannelArrayDecl.contextIn, yyt->ChannelArrayDecl.Idents->Idents.idents, yyt->ChannelArrayDecl.scope, yyt->ChannelArrayDecl.AType->AType.type, yyt->ChannelArrayDecl.Range->Range.range, yyt->ChannelArrayDecl.multicast));
yyVisit1 (yyt->ChannelArrayDecl.next);
yyt->ChannelArrayDecl.contextOut=yyt->ChannelArrayDecl.next->Decls.contextOut;
break;

case kSyncDecl:
yyt->SyncDecl.next->Decls.scope=yyt->SyncDecl.scope;
yyVisit1 (yyt->SyncDecl.Idents);
yyt->SyncDecl.next->Decls.skip=yyt->SyncDecl.skip;
# line 600 "Balsa.ag"

	yyt->SyncDecl.next->Decls.contextIn = (yyt->SyncDecl.skip ? yyt->SyncDecl.contextIn :
		ChannelDeclaration (yyt->SyncDecl.contextIn, yyt->SyncDecl.Idents->Idents.idents, yyt->SyncDecl.scope, SyncTypeObj, yyt->SyncDecl.multicast, yyt->SyncDecl.position));
yyVisit1 (yyt->SyncDecl.next);
yyt->SyncDecl.contextOut=yyt->SyncDecl.next->Decls.contextOut;
break;

case kSyncArrayDecl:
yyt->SyncArrayDecl.next->Decls.scope=yyt->SyncArrayDecl.scope;
# line 606 "Balsa.ag"

	yyt->SyncArrayDecl.Range->Range.isSpan = false;
# line 605 "Balsa.ag"

	yyt->SyncArrayDecl.Range->Range.expectedType = NoType;
yyt->SyncArrayDecl.Range->Range.skip=yyt->SyncArrayDecl.skip;
# line 604 "Balsa.ag"
 
	yyt->SyncArrayDecl.Range->Range.context = yyt->SyncArrayDecl.contextIn;
yyVisit1 (yyt->SyncArrayDecl.Range);
yyVisit1 (yyt->SyncArrayDecl.Idents);
yyt->SyncArrayDecl.next->Decls.skip=yyt->SyncArrayDecl.skip;
# line 607 "Balsa.ag"
 
	yyt->SyncArrayDecl.next->Decls.contextIn = (yyt->SyncArrayDecl.skip ? yyt->SyncArrayDecl.contextIn :
		ChannelArrayDeclaration (yyt->SyncArrayDecl.contextIn, yyt->SyncArrayDecl.Idents->Idents.idents, yyt->SyncArrayDecl.scope, SyncTypeObj, yyt->SyncArrayDecl.Range->Range.range, yyt->SyncArrayDecl.multicast));
yyVisit1 (yyt->SyncArrayDecl.next);
yyt->SyncArrayDecl.contextOut=yyt->SyncArrayDecl.next->Decls.contextOut;
break;

case kProcedureDecl:
yyt->ProcedureDecl.next->Decls.scope=yyt->ProcedureDecl.scope;
# line 633 "Balsa.ag"
 
	yyt->ProcedureDecl.FormalPorts->FormalPorts.inConditionalPorts = false;
# line 632 "Balsa.ag"

	yyt->ProcedureDecl.FormalPorts->FormalPorts.portsType = ProcedurePorts;
yyt->ProcedureDecl.FormalPorts->FormalPorts.skip=yyt->ProcedureDecl.skip;
# line 631 "Balsa.ag"

	yyt->ProcedureDecl.FormalPorts->FormalPorts.context = yyt->ProcedureDecl.contextIn;
yyVisit1 (yyt->ProcedureDecl.FormalPorts);
# line 613 "Balsa.ag"
 
		yyt->ProcedureDecl.Block->Block.skip = yyt->ProcedureDecl.skip;
		yyt->ProcedureDecl.Block->Block.context = yyt->ProcedureDecl.contextIn;
		yyt->ProcedureDecl.procedure = NULL;

		if (!yyt->ProcedureDecl.skip)
		{
			if (yyt->ProcedureDecl.FormalPorts->FormalPorts.hasParameters) 
			{
				yyt->ProcedureDecl.Block->Block.skip = true;
				yyt->ProcedureDecl.procedure = ParameterisedProcedureDeclaration (yyt->ProcedureDecl.contextIn, yyt->ProcedureDecl.ident, yyt, yyt->ProcedureDecl.position);		
			} else {
				yyt->ProcedureDecl.procedure = ProcedureDeclaration (yyt->ProcedureDecl.contextIn, yyt->ProcedureDecl.ident, false , yyt->ProcedureDecl.FormalPorts->FormalPorts.ports,
					yyt->ProcedureDecl.FormalPorts->FormalPorts.portCount, yyt->ProcedureDecl.FormalPorts->FormalPorts.extraPortInstances, &yyt->ProcedureDecl.Block->Block.context, yyt->ProcedureDecl.position); 
			}
			yyt->ProcedureDecl.procedure->scope = yyt->ProcedureDecl.scope;
		}
	
yyVisit1 (yyt->ProcedureDecl.Block);
yyt->ProcedureDecl.next->Decls.skip=yyt->ProcedureDecl.skip;
# line 635 "Balsa.ag"

		yyt->ProcedureDecl.next->Decls.contextIn = yyt->ProcedureDecl.contextIn;
		if (!yyt->ProcedureDecl.skip)
		{
			if (yyt->ProcedureDecl.FormalPorts->FormalPorts.hasParameters && yyt->ProcedureDecl.procedure != NoProcedure)
			{
				yyt->ProcedureDecl.next->Decls.contextIn = NewContext (yyt->ProcedureDecl.contextIn->depth, yyt->ProcedureDecl.contextIn->types,
					yyt->ProcedureDecl.contextIn->instances, NewProcedureList (yyt->ProcedureDecl.procedure, yyt->ProcedureDecl.contextIn->procedures));
				yyt->ProcedureDecl.contextIn = yyt->ProcedureDecl.next->Decls.contextIn; 
			} else {
				yyt->ProcedureDecl.next->Decls.contextIn = ProcedureDeclarationFillIn (yyt->ProcedureDecl.procedure, yyt->ProcedureDecl.contextIn,
				false , yyt->ProcedureDecl.Block->Block.attributes, yyt->ProcedureDecl.position);
			}
		}
	
yyVisit1 (yyt->ProcedureDecl.next);
yyt->ProcedureDecl.contextOut=yyt->ProcedureDecl.next->Decls.contextOut;
break;

case kProcAliasDecl:
yyt->ProcAliasDecl.next->Decls.scope=yyt->ProcAliasDecl.scope;
# line 653 "Balsa.ag"

	yyt->ProcAliasDecl.ProcedureParams->ProcedureParams.parameterContext = yyt->ProcAliasDecl.contextIn;
# line 657 "Balsa.ag"

	yyt->ProcAliasDecl.ProcedureParams->ProcedureParams.noPorts = true;
# line 660 "Balsa.ag"

		yyt->ProcAliasDecl.ProcedureParams->ProcedureParams.procedureTree = NULL;
		yyt->ProcAliasDecl.ProcedureParams->ProcedureParams.formalParams = NULL;
		yyt->ProcAliasDecl.ProcedureParams->ProcedureParams.idents = NULL;
		yyt->ProcAliasDecl.ProcedureParams->ProcedureParams.procedureIn = (yyt->ProcAliasDecl.skip ? NoProcedure : LookupProcedure
			(yyt->ProcAliasDecl.contextIn, yyt->ProcAliasDecl.oldName, false  ));
		yyt->ProcAliasDecl.ProcedureParams->ProcedureParams.skip = yyt->ProcAliasDecl.skip;

		if (! yyt->ProcAliasDecl.skip)
		{
			if (! yyt->ProcAliasDecl.ProcedureParams->ProcedureParams.procedureIn ||
				yyt->ProcAliasDecl.ProcedureParams->ProcedureParams.procedureIn == NoProcedure)
			{
				LOG_ERROR (ExpectingAProcedureName, yyt->ProcAliasDecl.oldName, yyt->ProcAliasDecl.position);
				yyt->ProcAliasDecl.ProcedureParams->ProcedureParams.skip = true;
			} else {
				switch (yyt->ProcAliasDecl.ProcedureParams->ProcedureParams.procedureIn->nature)
				{
					case UnSharedProcedure: 
						break;
					case ParameterisedProcedure:
						{
							tTree tree = yyt->ProcAliasDecl.ProcedureParams->ProcedureParams.procedureIn->info.parameterisedProcedure.tree;
							tTree oldNext = tree->Decl.next;
						
							tree->Decl.next = mNullDecls (yyt->ProcAliasDecl.position);
							yyt->ProcAliasDecl.ProcedureParams->ProcedureParams.procedureTree = CopyTree (tree);
							tree->Decl.next = oldNext;					

							yyt->ProcAliasDecl.ProcedureParams->ProcedureParams.formalParams =
								yyt->ProcAliasDecl.ProcedureParams->ProcedureParams.procedureTree->ProcedureDecl.FormalPorts;
							yyt->ProcAliasDecl.ProcedureParams->ProcedureParams.idents = (yyt->ProcAliasDecl.ProcedureParams->ProcedureParams.formalParams->Kind == kParamPort
								?  yyt->ProcAliasDecl.ProcedureParams->ProcedureParams.formalParams->ParamPort.Idents->Idents.idents
								: yyt->ProcAliasDecl.ProcedureParams->ProcedureParams.formalParams->TypeParamPort.Idents->Idents.idents);
						}
						break;
					default:
						LOG_ERROR (ExpectingAProcedureName, yyt->ProcAliasDecl.ProcedureParams->ProcedureParams.procedureIn->ident, yyt->ProcAliasDecl.position);
						yyt->ProcAliasDecl.ProcedureParams->ProcedureParams.skip = true;
						break;
				}
			}
		}
	
# line 656 "Balsa.ag"

	yyt->ProcAliasDecl.ProcedureParams->ProcedureParams.typeParams = NULL;
# line 655 "Balsa.ag"

	yyt->ProcAliasDecl.ProcedureParams->ProcedureParams.constantParams = NULL;
# line 654 "Balsa.ag"

	yyt->ProcAliasDecl.ProcedureParams->ProcedureParams.formalPorts = NULL;
# line 652 "Balsa.ag"

	yyt->ProcAliasDecl.ProcedureParams->ProcedureParams.context = yyt->ProcAliasDecl.contextIn;
yyVisit1 (yyt->ProcAliasDecl.ProcedureParams);
yyt->ProcAliasDecl.next->Decls.skip=yyt->ProcAliasDecl.skip;
# line 704 "Balsa.ag"

		yyt->ProcAliasDecl.next->Decls.contextIn = yyt->ProcAliasDecl.contextIn;

		if (! yyt->ProcAliasDecl.skip && yyt->ProcAliasDecl.ProcedureParams->ProcedureParams.procedureOut != NoProcedure)
		{
			PtrProcedure realProc = CopyPtrProcedure (yyt->ProcAliasDecl.ProcedureParams->ProcedureParams.procedureOut);
			realProc->ident = yyt->ProcAliasDecl.newName;
			realProc->scope = yyt->ProcAliasDecl.scope;
			yyt->ProcAliasDecl.next->Decls.contextIn = NewContext (yyt->ProcAliasDecl.contextIn->depth,
				yyt->ProcAliasDecl.contextIn->types,
				yyt->ProcAliasDecl.contextIn->instances,
				NewProcedureList (realProc, yyt->ProcAliasDecl.contextIn->procedures)
			);
		}
	
yyVisit1 (yyt->ProcAliasDecl.next);
yyt->ProcAliasDecl.contextOut=yyt->ProcAliasDecl.next->Decls.contextOut;
break;

case kFunctionDecl:
yyt->FunctionDecl.next->Decls.scope=yyt->FunctionDecl.scope;
yyt->FunctionDecl.AType->AType.skip=yyt->FunctionDecl.skip;
# line 757 "Balsa.ag"

	yyt->FunctionDecl.AType->AType.context = yyt->FunctionDecl.contextIn;
yyVisit1 (yyt->FunctionDecl.AType);
yyt->FunctionDecl.CoercedExpr->CoercedExpr.skip=yyt->FunctionDecl.skip;
# line 738 "Balsa.ag"

	yyt->FunctionDecl.CoercedExpr->CoercedExpr.allowChannels = false;
# line 737 "Balsa.ag"
 
	
	yyt->FunctionDecl.CoercedExpr->CoercedExpr.expectedType = (yyt->FunctionDecl.AType->Kind == kNullType ? NoType : yyt->FunctionDecl.AType->AType.type);
# line 759 "Balsa.ag"

	yyt->FunctionDecl.FormalPorts->FormalPorts.inConditionalPorts = false;
# line 758 "Balsa.ag"

	yyt->FunctionDecl.FormalPorts->FormalPorts.portsType = FunctionArgs;
yyt->FunctionDecl.FormalPorts->FormalPorts.skip=yyt->FunctionDecl.skip;
# line 756 "Balsa.ag"

	yyt->FunctionDecl.FormalPorts->FormalPorts.context = yyt->FunctionDecl.contextIn;
yyVisit1 (yyt->FunctionDecl.FormalPorts);
# line 739 "Balsa.ag"

		yyt->FunctionDecl.CoercedExpr->CoercedExpr.context = yyt->FunctionDecl.contextIn;
		yyt->FunctionDecl.function = NoProcedure;

		if (!yyt->FunctionDecl.skip)
		{
			if (yyt->FunctionDecl.FormalPorts->FormalPorts.hasParameters) 
			{
				LOG_ERROR (FunctionCantHaveParameters, yyt->FunctionDecl.ident, yyt->FunctionDecl.position);
			} else {
				yyt->FunctionDecl.function = FunctionDeclaration (yyt->FunctionDecl.contextIn, yyt->FunctionDecl.ident, UnSharedFunction,
					yyt->FunctionDecl.FormalPorts->FormalPorts.ports, yyt->FunctionDecl.FormalPorts->FormalPorts.portCount, &yyt->FunctionDecl.CoercedExpr->CoercedExpr.context, yyt->FunctionDecl.position);
			}
			if (yyt->FunctionDecl.function != NoProcedure)
				yyt->FunctionDecl.function->scope = yyt->FunctionDecl.scope;
		}
	
yyVisit1 (yyt->FunctionDecl.CoercedExpr);
yyt->FunctionDecl.next->Decls.skip=yyt->FunctionDecl.skip;
# line 760 "Balsa.ag"

	yyt->FunctionDecl.next->Decls.contextIn = (yyt->FunctionDecl.skip ? yyt->FunctionDecl.contextIn :
		FunctionDeclarationFillIn (yyt->FunctionDecl.function, yyt->FunctionDecl.contextIn,
			yyt->FunctionDecl.CoercedExpr->CoercedExpr.attributes, (yyt->FunctionDecl.AType->Kind == kNullType ? NoType : yyt->FunctionDecl.AType->AType.type), yyt->FunctionDecl.position));
yyVisit1 (yyt->FunctionDecl.next);
yyt->FunctionDecl.contextOut=yyt->FunctionDecl.next->Decls.contextOut;
break;

case kBuiltinFunctionDecl:
# line 788 "Balsa.ag"

	yyt->BuiltinFunctionDecl.FormalPorts->FormalPorts.context = yyt->BuiltinFunctionDecl.contextIn;
yyt->BuiltinFunctionDecl.FormalPorts->FormalPorts.skip=yyt->BuiltinFunctionDecl.skip;
# line 792 "Balsa.ag"

	yyt->BuiltinFunctionDecl.FormalPorts->FormalPorts.inConditionalPorts = false;
# line 791 "Balsa.ag"

	yyt->BuiltinFunctionDecl.FormalPorts->FormalPorts.portsType = FunctionArgs;
yyVisit1 (yyt->BuiltinFunctionDecl.FormalPorts);
# line 766 "Balsa.ag"

		PtrContext bodyContext;
		yyt->BuiltinFunctionDecl.function = NoProcedure;

		if (!yyt->BuiltinFunctionDecl.skip)
		{
			if (yyt->BuiltinFunctionDecl.scope == InnerScope)
				LOG_ERROR (CannotDeclareBuiltinFunctionHere, yyt->BuiltinFunctionDecl.ident, yyt->BuiltinFunctionDecl.position);
			else {
				if (yyt->BuiltinFunctionDecl.FormalPorts->FormalPorts.hasParameters) 
				{
					yyt->BuiltinFunctionDecl.function = ParameterisedFunctionDeclaration (ParameterisedBuiltinFunction,
						yyt->BuiltinFunctionDecl.contextIn, yyt->BuiltinFunctionDecl.ident, yyt, yyt->BuiltinFunctionDecl.position);		
				} else {
					yyt->BuiltinFunctionDecl.function = FunctionDeclaration (yyt->BuiltinFunctionDecl.contextIn, yyt->BuiltinFunctionDecl.ident, BuiltinFunction,
						yyt->BuiltinFunctionDecl.FormalPorts->FormalPorts.ports, yyt->BuiltinFunctionDecl.FormalPorts->FormalPorts.portCount, &bodyContext, yyt->BuiltinFunctionDecl.position);
				}
				if (yyt->BuiltinFunctionDecl.function != NoProcedure)
					yyt->BuiltinFunctionDecl.function->scope = yyt->BuiltinFunctionDecl.scope;
			}
		}
	
# line 790 "Balsa.ag"

	yyt->BuiltinFunctionDecl.AType->AType.skip = (yyt->BuiltinFunctionDecl.skip || yyt->BuiltinFunctionDecl.FormalPorts->FormalPorts.hasParameters);
# line 789 "Balsa.ag"

	yyt->BuiltinFunctionDecl.AType->AType.context = yyt->BuiltinFunctionDecl.contextIn;
yyVisit1 (yyt->BuiltinFunctionDecl.AType);
yyt->BuiltinFunctionDecl.next->Decls.skip=yyt->BuiltinFunctionDecl.skip;
yyt->BuiltinFunctionDecl.next->Decls.scope=yyt->BuiltinFunctionDecl.scope;
# line 793 "Balsa.ag"

		yyt->BuiltinFunctionDecl.next->Decls.contextIn = yyt->BuiltinFunctionDecl.contextIn;
		if (!yyt->BuiltinFunctionDecl.skip)
		{
			if (yyt->BuiltinFunctionDecl.FormalPorts->FormalPorts.hasParameters)
			{
				yyt->BuiltinFunctionDecl.next->Decls.contextIn = NewContext (yyt->BuiltinFunctionDecl.contextIn->depth, yyt->BuiltinFunctionDecl.contextIn->types,
					yyt->BuiltinFunctionDecl.contextIn->instances, NewProcedureList (yyt->BuiltinFunctionDecl.function, yyt->BuiltinFunctionDecl.contextIn->procedures));
				yyt->BuiltinFunctionDecl.contextIn = yyt->BuiltinFunctionDecl.next->Decls.contextIn; 
			} else {
				yyt->BuiltinFunctionDecl.next->Decls.contextIn = BuiltinFunctionDeclarationFillIn (yyt->BuiltinFunctionDecl.function, yyt->BuiltinFunctionDecl.contextIn, yyt->BuiltinFunctionDecl.AType->AType.type, yyt->BuiltinFunctionDecl.position);
			}
		}
	
yyVisit1 (yyt->BuiltinFunctionDecl.next);
yyt->BuiltinFunctionDecl.contextOut=yyt->BuiltinFunctionDecl.next->Decls.contextOut;
break;

case kSharedDecl:
yyt->SharedDecl.next->Decls.scope=yyt->SharedDecl.scope;
yyt->SharedDecl.Block->Block.skip=yyt->SharedDecl.skip;
# line 721 "Balsa.ag"

		yyt->SharedDecl.Block->Block.context = yyt->SharedDecl.contextIn;
		yyt->SharedDecl.procedure = NULL;
	
		if (!yyt->SharedDecl.skip)
		{
			yyt->SharedDecl.procedure = ProcedureDeclaration (yyt->SharedDecl.contextIn, yyt->SharedDecl.ident, true , NULL, 
				0, NULL, &yyt->SharedDecl.Block->Block.context, yyt->SharedDecl.position); 
			yyt->SharedDecl.procedure->scope = yyt->SharedDecl.scope;
		}
	
yyVisit1 (yyt->SharedDecl.Block);
yyt->SharedDecl.next->Decls.skip=yyt->SharedDecl.skip;
# line 732 "Balsa.ag"

	yyt->SharedDecl.next->Decls.contextIn = (yyt->SharedDecl.skip ? yyt->SharedDecl.contextIn : ProcedureDeclarationFillIn (yyt->SharedDecl.procedure, yyt->SharedDecl.contextIn, true ,
		yyt->SharedDecl.Block->Block.attributes, yyt->SharedDecl.position));
yyVisit1 (yyt->SharedDecl.next);
yyt->SharedDecl.contextOut=yyt->SharedDecl.next->Decls.contextOut;
break;

case kPartDecl:
yyt->PartDecl.next->Decls.scope=yyt->PartDecl.scope;
# line 2998 "Balsa.ag"


	yyt->PartDecl.Comps->Comps.componentsIn = NULL;
yyt->PartDecl.Comps->Comps.channelCount=yyt->PartDecl.channelCount;
yyt->PartDecl.Comps->Comps.channels=yyt->PartDecl.channels;
# line 2996 "Balsa.ag"
  
	yyt->PartDecl.FormalPorts->FormalPorts.inConditionalPorts = false;
# line 2995 "Balsa.ag"

	yyt->PartDecl.FormalPorts->FormalPorts.portsType = PartPorts;
yyt->PartDecl.FormalPorts->FormalPorts.skip=yyt->PartDecl.skip;
# line 2994 "Balsa.ag"


	yyt->PartDecl.FormalPorts->FormalPorts.context = yyt->PartDecl.contextIn;
yyVisit1 (yyt->PartDecl.FormalPorts);
# line 2982 "Balsa.ag"

		yyt->PartDecl.Comps->Comps.skip = false;

		yyt->PartDecl.procedure = (yyt->PartDecl.skip ? NULL : PartDeclaration (yyt->PartDecl.contextIn, yyt->PartDecl.ident, yyt->PartDecl.FormalPorts->FormalPorts.ports,
			yyt->PartDecl.FormalPorts->FormalPorts.portCount, yyt->PartDecl.FormalPorts->FormalPorts.extraPortInstances, yyt->PartDecl.attributes, yyt->PartDecl.position));
		if (yyt->PartDecl.procedure)
			yyt->PartDecl.procedure->scope = yyt->PartDecl.scope;
		else {
			yyt->PartDecl.Comps->Comps.skip = true;
		}
	
# line 2999 "Balsa.ag"

	yyt->PartDecl.Comps->Comps.context = yyt->PartDecl.contextIn;
yyVisit1 (yyt->PartDecl.Comps);
yyt->PartDecl.next->Decls.skip=yyt->PartDecl.skip;
# line 2978 "Balsa.ag"
 
	yyt->PartDecl.next->Decls.contextIn = PartDeclarationFillIn (yyt->PartDecl.procedure, yyt->PartDecl.contextIn, yyt->PartDecl.channels, yyt->PartDecl.channelCount,
		yyt->PartDecl.Comps->Comps.componentsOut, yyt->PartDecl.position);
yyVisit1 (yyt->PartDecl.next);
yyt->PartDecl.contextOut=yyt->PartDecl.next->Decls.contextOut;
break;

case kIfDecls:
yyt->IfDecls.next->Decls.scope=yyt->IfDecls.scope;
yyt->IfDecls.DeclGuards->DeclGuards.skip=yyt->IfDecls.skip;
# line 3004 "Balsa.ag"

	yyt->IfDecls.DeclGuards->DeclGuards.contextIn = yyt->IfDecls.contextIn;
yyt->IfDecls.DeclGuards->DeclGuards.scope=yyt->IfDecls.scope;
yyVisit1 (yyt->IfDecls.DeclGuards);
yyt->IfDecls.next->Decls.skip=yyt->IfDecls.skip;
# line 3007 "Balsa.ag"

		yyt->IfDecls.next->Decls.contextIn = yyt->IfDecls.contextIn;

		if (! yyt->IfDecls.skip)
		{
			if (yyt->IfDecls.DeclGuards->DeclGuards.foundTrueGuard)
				yyt->IfDecls.next->Decls.contextIn = yyt->IfDecls.DeclGuards->DeclGuards.contextOut;
		}
	
yyVisit1 (yyt->IfDecls.next);
# line 3005 "Balsa.ag"

	yyt->IfDecls.contextOut = yyt->IfDecls.next->Decls.contextOut;
break;

case kIfElseDecls:
yyt->IfElseDecls.next->Decls.scope=yyt->IfElseDecls.scope;
yyt->IfElseDecls.DeclGuards->DeclGuards.skip=yyt->IfElseDecls.skip;
# line 3018 "Balsa.ag"

	yyt->IfElseDecls.DeclGuards->DeclGuards.contextIn = yyt->IfElseDecls.contextIn;
yyt->IfElseDecls.DeclGuards->DeclGuards.scope=yyt->IfElseDecls.scope;
yyVisit1 (yyt->IfElseDecls.DeclGuards);
# line 3021 "Balsa.ag"

	yyt->IfElseDecls.Decls->Decls.skip = yyt->IfElseDecls.skip || yyt->IfElseDecls.DeclGuards->DeclGuards.foundTrueGuard;
# line 3019 "Balsa.ag"

	yyt->IfElseDecls.Decls->Decls.contextIn = yyt->IfElseDecls.contextIn;
yyt->IfElseDecls.Decls->Decls.scope=yyt->IfElseDecls.scope;
yyVisit1 (yyt->IfElseDecls.Decls);
yyt->IfElseDecls.next->Decls.skip=yyt->IfElseDecls.skip;
# line 3023 "Balsa.ag"

		yyt->IfElseDecls.next->Decls.contextIn = yyt->IfElseDecls.contextIn;

		if (! yyt->IfElseDecls.skip)
		{
			if (yyt->IfElseDecls.DeclGuards->DeclGuards.foundTrueGuard)
			{
				yyt->IfElseDecls.next->Decls.contextIn = yyt->IfElseDecls.DeclGuards->DeclGuards.contextOut;
			} else {
			  
				yyt->IfElseDecls.next->Decls.contextIn = yyt->IfElseDecls.Decls->Decls.contextOut;
			}
		}
	
yyVisit1 (yyt->IfElseDecls.next);
# line 3020 "Balsa.ag"

	yyt->IfElseDecls.contextOut = yyt->IfElseDecls.next->Decls.contextOut;
break;

case kPrintDecl:
yyt->PrintDecl.next->Decls.scope=yyt->PrintDecl.scope;
# line 3044 "Balsa.ag"

	yyt->PrintDecl.ExprLists->ExprLists.expectConstants = false;
# line 3046 "Balsa.ag"

	yyt->PrintDecl.ExprLists->ExprLists.elements = NULL;
# line 3045 "Balsa.ag"
 
	yyt->PrintDecl.ExprLists->ExprLists.expectedType = NoType;
yyt->PrintDecl.ExprLists->ExprLists.skip=yyt->PrintDecl.skip;
# line 3041 "Balsa.ag"

	yyt->PrintDecl.ExprLists->ExprLists.context = yyt->PrintDecl.contextIn;
yyVisit1 (yyt->PrintDecl.ExprLists);
# line 3042 "Balsa.ag"

	yyt->PrintDecl.Expr->Expr.allowChannels = false;
# line 3043 "Balsa.ag"

	yyt->PrintDecl.Expr->Expr.expectedType = ErrorTypeObj;
yyt->PrintDecl.Expr->Expr.skip=yyt->PrintDecl.skip;
# line 3040 "Balsa.ag"

	yyt->PrintDecl.Expr->Expr.context = yyt->PrintDecl.contextIn;
yyVisit1 (yyt->PrintDecl.Expr);
yyt->PrintDecl.next->Decls.skip=yyt->PrintDecl.skip;
# line 3049 "Balsa.ag"

		yyt->PrintDecl.next->Decls.contextIn = yyt->PrintDecl.contextIn;

		if (!yyt->PrintDecl.skip)
			HandlePrintCommand (yyt->PrintDecl.contextIn, yyt->PrintDecl.Expr->Expr.attributes, yyt->PrintDecl.ExprLists->ExprLists.attributes, true
				, yyt->PrintDecl.position);
	
yyVisit1 (yyt->PrintDecl.next);
# line 3047 "Balsa.ag"

	yyt->PrintDecl.contextOut = yyt->PrintDecl.next->Decls.contextOut;
break;

case kNullDecl:
yyt->NullDecl.next->Decls.scope=yyt->NullDecl.scope;
yyt->NullDecl.next->Decls.skip=yyt->NullDecl.skip;
yyt->NullDecl.next->Decls.contextIn=yyt->NullDecl.contextIn;
yyVisit1 (yyt->NullDecl.next);
yyt->NullDecl.contextOut=yyt->NullDecl.next->Decls.contextOut;
break;

case kRange:
# line 810 "Balsa.ag"
 yyt->Range.range = NoSpan;
break;

case kSpecifiedRange:
yyt->SpecifiedRange.Left->Expr.context=yyt->SpecifiedRange.context;
# line 815 "Balsa.ag"
 
	yyt->SpecifiedRange.Left->Expr.allowChannels = false;
# line 830 "Balsa.ag"

	yyt->SpecifiedRange.Left->Expr.expectedType = yyt->SpecifiedRange.expectedType;
yyt->SpecifiedRange.Left->Expr.skip=yyt->SpecifiedRange.skip;
yyVisit1 (yyt->SpecifiedRange.Left);
# line 816 "Balsa.ag"

	yyt->SpecifiedRange.Right->Expr.allowChannels = false;
yyt->SpecifiedRange.Right->Expr.context=yyt->SpecifiedRange.context;
# line 831 "Balsa.ag"
 
		yyt->SpecifiedRange.Right->Expr.expectedType = NoType;

		if (!yyt->SpecifiedRange.skip)
		{
			if (yyt->SpecifiedRange.Left->Kind == kNullExpr) yyt->SpecifiedRange.Right->Expr.expectedType = yyt->SpecifiedRange.expectedType;
			else if ((yyt->SpecifiedRange.Left->Expr.attributes).value.type->nature == EnumerationType)
			
				yyt->SpecifiedRange.Right->Expr.expectedType = (yyt->SpecifiedRange.Left->Expr.attributes).value.type;
		}
	
yyt->SpecifiedRange.Right->Expr.skip=yyt->SpecifiedRange.skip;
yyVisit1 (yyt->SpecifiedRange.Right);
# line 817 "Balsa.ag"

		yyt->SpecifiedRange.range = NoSpan;
	
		if (!yyt->SpecifiedRange.skip)
		{
			
			yyt->SpecifiedRange.range = HandleSpecifiedRange (yyt->SpecifiedRange.Left->Kind != kNullExpr || yyt->SpecifiedRange.isSpan, 
				(yyt->SpecifiedRange.Left->Kind == kNullExpr ? yyt->SpecifiedRange.Right->Expr.attributes : yyt->SpecifiedRange.Left->Expr.attributes), 
				yyt->SpecifiedRange.Right->Expr.attributes, yyt->SpecifiedRange.expectedType, (yyt->SpecifiedRange.Left->Kind == kNullExpr ? yyt->SpecifiedRange.Right->Expr.position : yyt->SpecifiedRange.position)); 
		}
	
break;

case kTypeRange:
yyt->TypeRange.AType->AType.context=yyt->TypeRange.context;
yyt->TypeRange.AType->AType.skip=yyt->TypeRange.skip;
yyVisit1 (yyt->TypeRange.AType);
# line 812 "Balsa.ag"
 
	yyt->TypeRange.range = (yyt->TypeRange.skip ? NoSpan : HandleTypeRange (yyt->TypeRange.AType->AType.type, yyt->TypeRange.expectedType, yyt->TypeRange.position));
break;

case kAType:
# line 846 "Balsa.ag"
 yyt->AType.type = NoType;
break;

case kNullType:
# line 846 "Balsa.ag"
 yyt->NullType.type = NoType;
break;

case kNumericType:
yyt->NumericType.Expr->Expr.context=yyt->NumericType.context;
# line 850 "Balsa.ag"
 
	yyt->NumericType.Expr->Expr.allowChannels = false;
# line 849 "Balsa.ag"
 
	yyt->NumericType.Expr->Expr.expectedType = NoType;
yyt->NumericType.Expr->Expr.skip=yyt->NumericType.skip;
yyVisit1 (yyt->NumericType.Expr);
# line 848 "Balsa.ag"
 
	yyt->NumericType.type = (yyt->NumericType.skip ? NoType : MakeNumericType ((yyt->NumericType.Expr->Expr.attributes).value, yyt->NumericType.Expr->Expr.position, yyt->NumericType.signedness, yyt->NumericType.position));
break;

case kExistingType:
# line 852 "Balsa.ag"
 yyt->ExistingType.type = (yyt->ExistingType.skip ? NoType :
	ExistingTypeLookup (yyt->ExistingType.context, yyt->ExistingType.ident, yyt->ExistingType.position));
break;

case kBuiltinType:
# line 855 "Balsa.ag"
 yyt->BuiltinType.type = (yyt->BuiltinType.skip ? NoType : NewBuiltinType ());
break;

case kArrayType:
yyt->ArrayType.AType->AType.context=yyt->ArrayType.context;
yyt->ArrayType.AType->AType.skip=yyt->ArrayType.skip;
yyVisit1 (yyt->ArrayType.AType);
# line 858 "Balsa.ag"

	yyt->ArrayType.Range->Range.isSpan = false;
# line 858 "Balsa.ag"
 yyt->ArrayType.Range->Range.expectedType = NoType;
yyt->ArrayType.Range->Range.skip=yyt->ArrayType.skip;
yyt->ArrayType.Range->Range.context=yyt->ArrayType.context;
yyVisit1 (yyt->ArrayType.Range);
# line 856 "Balsa.ag"
 yyt->ArrayType.type = (yyt->ArrayType.skip ? NoType :
	HandleArrayType (yyt->ArrayType.context, yyt->ArrayType.AType->AType.type, yyt->ArrayType.Range->Range.range, yyt->ArrayType.position));
break;

case kNewType:
yyt->NewType.AType->AType.context=yyt->NewType.context;
yyt->NewType.AType->AType.skip=yyt->NewType.skip;
yyVisit1 (yyt->NewType.AType);
# line 860 "Balsa.ag"
 yyt->NewType.type = (yyt->NewType.AType->AType.type == NoType ? NoType : CopyAnonType (yyt->NewType.AType->AType.type));
break;

case kRecordType:
yyt->RecordType.RecordElems->RecordElems.context=yyt->RecordType.context;
yyt->RecordType.RecordElems->RecordElems.skip=yyt->RecordType.skip;
yyt->RecordType.AType->AType.skip=yyt->RecordType.skip;
yyt->RecordType.AType->AType.context=yyt->RecordType.context;
yyVisit1 (yyt->RecordType.AType);
# line 865 "Balsa.ag"

	yyt->RecordType.RecordElems->RecordElems.elementsTail = NULL;
# line 862 "Balsa.ag"

	yyt->RecordType.RecordElems->RecordElems.hasOverType = !(yyt->RecordType.AType->Kind == kNullType);
# line 863 "Balsa.ag"

	yyt->RecordType.RecordElems->RecordElems.typeIn = (yyt->RecordType.skip ? NoType : NewRecordType (MarkerIdent,
		(yyt->RecordType.RecordElems->RecordElems.hasOverType ? Abs (CheckAndSizeOfType(yyt->RecordType.AType->AType.type, yyt->RecordType.AType->AType.position)) : 0), NULL, 0));
yyVisit1 (yyt->RecordType.RecordElems);
# line 861 "Balsa.ag"
 yyt->RecordType.type = yyt->RecordType.RecordElems->RecordElems.typeOut;
break;

case kEnumType:
yyt->EnumType.EnumElems->EnumElems.context=yyt->EnumType.context;
yyt->EnumType.EnumElems->EnumElems.skip=yyt->EnumType.skip;
yyt->EnumType.AType->AType.skip=yyt->EnumType.skip;
yyt->EnumType.AType->AType.context=yyt->EnumType.context;
yyVisit1 (yyt->EnumType.AType);
# line 872 "Balsa.ag"

	yyt->EnumType.EnumElems->EnumElems.elementsTail = NULL;
# line 869 "Balsa.ag"
 
	yyt->EnumType.EnumElems->EnumElems.hasOverType = !(yyt->EnumType.AType->Kind == kNullType);
# line 870 "Balsa.ag"

	yyt->EnumType.EnumElems->EnumElems.typeIn = (yyt->EnumType.skip ? NoType : NewEnumerationType (MarkerIdent, 
		(yyt->EnumType.EnumElems->EnumElems.hasOverType ? CheckAndSizeOfType(yyt->EnumType.AType->AType.type, yyt->EnumType.AType->AType.position) : 0), NULL));
# line 868 "Balsa.ag"

	yyt->EnumType.EnumElems->EnumElems.elementValue = NewMP_INT (0);
yyVisit1 (yyt->EnumType.EnumElems);
# line 867 "Balsa.ag"
 yyt->EnumType.type = yyt->EnumType.EnumElems->EnumElems.typeOut;
break;

case kCoercedExpr:
yyt->CoercedExpr.Expr->Expr.context=yyt->CoercedExpr.context;
# line 914 "Balsa.ag"

	yyt->CoercedExpr.Expr->Expr.allowChannels = false;
yyt->CoercedExpr.Expr->Expr.expectedType=yyt->CoercedExpr.expectedType;
yyt->CoercedExpr.Expr->Expr.skip=yyt->CoercedExpr.skip;
yyVisit1 (yyt->CoercedExpr.Expr);
# line 915 "Balsa.ag"

		yyt->CoercedExpr.attributes = yyt->CoercedExpr.Expr->Expr.attributes;
		
		if (yyt->CoercedExpr.expectedType && yyt->CoercedExpr.expectedType->nature == NumericType &&
			yyt->CoercedExpr.attributes.value.type && yyt->CoercedExpr.attributes.value.type->nature == NumericType &&
			yyt->CoercedExpr.attributes.value.value.baseValue)
		{
			if (RangeIsWiderOrEqual (yyt->CoercedExpr.expectedType->size,
				SmallestRangeToHoldValue (yyt->CoercedExpr.attributes.value.value.baseValue)))
			{ 
				yyt->CoercedExpr.attributes.value.type = yyt->CoercedExpr.expectedType;
				if (yyt->CoercedExpr.attributes.activation)
					yyt->CoercedExpr.attributes.activation->width = Abs (yyt->CoercedExpr.expectedType->size);
			}
		}
	
yyt->CoercedExpr.position=yyt->CoercedExpr.Expr->Expr.position;
break;

case kExpr:
# line 933 "Balsa.ag"
  yyt->Expr.attributes = NoExprAttributes;
break;

case kNullExpr:
# line 933 "Balsa.ag"
  yyt->NullExpr.attributes = NoExprAttributes;
break;

case kLiteralExpr:
# line 935 "Balsa.ag"

	yyt->LiteralExpr.attributes = (yyt->LiteralExpr.skip ? NoExprAttributes : HandleLiteralExpr (yyt->LiteralExpr.literal, yyt->LiteralExpr.expectedType, yyt->LiteralExpr.position));
break;

case kIdentExpr:
# line 938 "Balsa.ag"

	yyt->IdentExpr.attributes = (yyt->IdentExpr.skip ? NoExprAttributes : HandleIdentExpr (yyt->IdentExpr.context, yyt->IdentExpr.ident, yyt->IdentExpr.expectedType,
		yyt->IdentExpr.allowChannels, yyt->IdentExpr.position));
break;

case kStringExpr:
# line 942 "Balsa.ag"
 
	yyt->StringExpr.attributes = (yyt->StringExpr.skip ? NoExprAttributes : HandleStringExpr (yyt->StringExpr.string, yyt->StringExpr.position));
break;

case kImplicantExpr:
# line 945 "Balsa.ag"

	yyt->ImplicantExpr.attributes = (yyt->ImplicantExpr.skip ? NoExprAttributes : HandleImplicantExpr (yyt->ImplicantExpr.implicant, yyt->ImplicantExpr.expectedType, yyt->ImplicantExpr.position));
break;

case kDontCareExpr:
# line 948 "Balsa.ag"

	yyt->DontCareExpr.attributes = (yyt->DontCareExpr.skip ? NoExprAttributes : HandleDontCareExpr (yyt->DontCareExpr.expectedType, yyt->DontCareExpr.position));
break;

case kAggregateConsExpr:
yyt->AggregateConsExpr.ExprLists->ExprLists.context=yyt->AggregateConsExpr.context;
yyt->AggregateConsExpr.ExprLists->ExprLists.skip=yyt->AggregateConsExpr.skip;
# line 951 "Balsa.ag"
  
	yyt->AggregateConsExpr.ExprLists->ExprLists.expectConstants = false;
# line 952 "Balsa.ag"
 
		yyt->AggregateConsExpr.ExprLists->ExprLists.expectedType = NoType;
		yyt->AggregateConsExpr.ExprLists->ExprLists.elements = NULL;
		yyt->AggregateConsExpr.actualType = NoType;
	
		if (!yyt->AggregateConsExpr.skip)
		{
			yyt->AggregateConsExpr.ExprLists->ExprLists.elements = GetAggregateConsExprElements (yyt->AggregateConsExpr.context, yyt->AggregateConsExpr.ident, yyt->AggregateConsExpr.expectedType, &yyt->AggregateConsExpr.actualType, yyt->AggregateConsExpr.position); 
			
			yyt->AggregateConsExpr.ExprLists->ExprLists.expectedType = (yyt->AggregateConsExpr.actualType->nature == RecordType ? NULL :
				(yyt->AggregateConsExpr.actualType->nature == ArrayType ? yyt->AggregateConsExpr.actualType->info.array.baseType : NULL));
		}
	
yyVisit1 (yyt->AggregateConsExpr.ExprLists);
# line 965 "Balsa.ag"

		yyt->AggregateConsExpr.attributes = NoExprAttributes;
		
		if (! yyt->AggregateConsExpr.skip)
		{
			yyt->AggregateConsExpr.attributes = HandleAggregateConsExpr (yyt->AggregateConsExpr.actualType, 
				yyt->AggregateConsExpr.ident != NoIdent ,
				false , yyt->AggregateConsExpr.ExprLists->ExprLists.attributes, yyt->AggregateConsExpr.position); 
		}
	
break;

case kNamedEnumElemExpr:
# line 977 "Balsa.ag"

	yyt->NamedEnumElemExpr.attributes = (yyt->NamedEnumElemExpr.skip ? NoExprAttributes : HandleNamedEnumElemExpr (yyt->NamedEnumElemExpr.context, yyt->NamedEnumElemExpr.typeName, yyt->NamedEnumElemExpr.elemName, yyt->NamedEnumElemExpr.position));
break;

case kUnaryExpr:
yyt->UnaryExpr.Expr->Expr.context=yyt->UnaryExpr.context;
yyt->UnaryExpr.Expr->Expr.allowChannels=yyt->UnaryExpr.allowChannels;
# line 1039 "Balsa.ag"

	yyt->UnaryExpr.Expr->Expr.expectedType = NoType;
yyt->UnaryExpr.Expr->Expr.skip=yyt->UnaryExpr.skip;
yyVisit1 (yyt->UnaryExpr.Expr);
# line 1040 "Balsa.ag"

	yyt->UnaryExpr.attributes = (yyt->UnaryExpr.skip ? NoExprAttributes : HandleUnaryExpr (yyt->UnaryExpr.context, yyt->UnaryExpr.operation, yyt->UnaryExpr.Expr->Expr.attributes, yyt->UnaryExpr.position));
break;

case kBinaryExpr:
yyt->BinaryExpr.Left->Expr.context=yyt->BinaryExpr.context;
yyt->BinaryExpr.Left->Expr.allowChannels=yyt->BinaryExpr.allowChannels;
# line 1043 "Balsa.ag"

	yyt->BinaryExpr.Left->Expr.expectedType = NoType;
yyt->BinaryExpr.Left->Expr.skip=yyt->BinaryExpr.skip;
yyVisit1 (yyt->BinaryExpr.Left);
yyt->BinaryExpr.Right->Expr.allowChannels=yyt->BinaryExpr.allowChannels;
yyt->BinaryExpr.Right->Expr.context=yyt->BinaryExpr.context;
# line 1044 "Balsa.ag"
 
		if ((yyt->BinaryExpr.Left->Expr.attributes).value.type->nature == EnumerationType ||
			(yyt->BinaryExpr.Left->Expr.attributes).value.type->nature == RecordType)
			yyt->BinaryExpr.Right->Expr.expectedType = (yyt->BinaryExpr.Left->Expr.attributes).value.type;
		else	yyt->BinaryExpr.Right->Expr.expectedType = NoType;
	
yyt->BinaryExpr.Right->Expr.skip=yyt->BinaryExpr.skip;
yyVisit1 (yyt->BinaryExpr.Right);
# line 1050 "Balsa.ag"

	yyt->BinaryExpr.attributes = (yyt->BinaryExpr.skip ? NoExprAttributes :
		HandleBinaryExpr (yyt->BinaryExpr.context, yyt->BinaryExpr.operation, yyt->BinaryExpr.Left->Expr.attributes, yyt->BinaryExpr.Right->Expr.attributes, yyt->BinaryExpr.position));
break;

case kRecordElemExtractExpr:
yyt->RecordElemExtractExpr.Expr->Expr.context=yyt->RecordElemExtractExpr.context;
yyt->RecordElemExtractExpr.Expr->Expr.allowChannels=yyt->RecordElemExtractExpr.allowChannels;
# line 980 "Balsa.ag"

	yyt->RecordElemExtractExpr.Expr->Expr.expectedType = NoType;
yyt->RecordElemExtractExpr.Expr->Expr.skip=yyt->RecordElemExtractExpr.skip;
yyVisit1 (yyt->RecordElemExtractExpr.Expr);
# line 982 "Balsa.ag"
 
	yyt->RecordElemExtractExpr.attributes = (yyt->RecordElemExtractExpr.skip ? NoExprAttributes :
		HandleRecordElemExtractExpr (yyt->RecordElemExtractExpr.context, yyt->RecordElemExtractExpr.ident, yyt->RecordElemExtractExpr.Expr->Expr.attributes, yyt->RecordElemExtractExpr.position));
break;

case kArrayExtractExpr:
yyt->ArrayExtractExpr.Array->Expr.context=yyt->ArrayExtractExpr.context;
# line 986 "Balsa.ag"
 
	
	yyt->ArrayExtractExpr.Array->Expr.allowChannels = true;
# line 987 "Balsa.ag"

	yyt->ArrayExtractExpr.Array->Expr.expectedType = NoType;
yyt->ArrayExtractExpr.Array->Expr.skip=yyt->ArrayExtractExpr.skip;
yyVisit1 (yyt->ArrayExtractExpr.Array);
yyt->ArrayExtractExpr.Subscript->CoercedExpr.skip=yyt->ArrayExtractExpr.skip;
yyt->ArrayExtractExpr.Subscript->CoercedExpr.allowChannels=yyt->ArrayExtractExpr.allowChannels;
yyt->ArrayExtractExpr.Subscript->CoercedExpr.context=yyt->ArrayExtractExpr.context;
# line 988 "Balsa.ag"
 
	yyt->ArrayExtractExpr.Subscript->CoercedExpr.expectedType = 
		(yyt->ArrayExtractExpr.skip ? NoType :
		((yyt->ArrayExtractExpr.Array->Expr.attributes).value.type->nature == ArrayType ?
			(yyt->ArrayExtractExpr.Array->Expr.attributes).value.type->info.array.range.boundingType :
		((yyt->ArrayExtractExpr.Array->Expr.attributes).value.type->nature == ArrayedType ?
			(yyt->ArrayExtractExpr.Array->Expr.attributes).value.type->info.arrayed.range.boundingType : NoType)));
yyVisit1 (yyt->ArrayExtractExpr.Subscript);
# line 994 "Balsa.ag"

	yyt->ArrayExtractExpr.attributes = (yyt->ArrayExtractExpr.skip ? NoExprAttributes :
		HandleArrayExtractExpr (yyt->ArrayExtractExpr.context, yyt->ArrayExtractExpr.Array->Expr.attributes, yyt->ArrayExtractExpr.Subscript->CoercedExpr.attributes, yyt->ArrayExtractExpr.position));
break;

case kArraySliceExpr:
yyt->ArraySliceExpr.Expr->Expr.context=yyt->ArraySliceExpr.context;
yyt->ArraySliceExpr.Expr->Expr.allowChannels=yyt->ArraySliceExpr.allowChannels;
yyt->ArraySliceExpr.Expr->Expr.expectedType=yyt->ArraySliceExpr.expectedType;
yyt->ArraySliceExpr.Expr->Expr.skip=yyt->ArraySliceExpr.skip;
yyVisit1 (yyt->ArraySliceExpr.Expr);
# line 998 "Balsa.ag"
 
	yyt->ArraySliceExpr.Range->Range.isSpan = true;
yyt->ArraySliceExpr.Range->Range.skip=yyt->ArraySliceExpr.skip;
yyt->ArraySliceExpr.Range->Range.context=yyt->ArraySliceExpr.context;
# line 999 "Balsa.ag"
 
	yyt->ArraySliceExpr.Range->Range.expectedType = (yyt->ArraySliceExpr.skip ? NoType : ((yyt->ArraySliceExpr.Expr->Expr.attributes).value.type->nature == ArrayType ?
		(yyt->ArraySliceExpr.Expr->Expr.attributes).value.type->info.array.range.boundingType : NoType));
yyVisit1 (yyt->ArraySliceExpr.Range);
# line 1001 "Balsa.ag"

	yyt->ArraySliceExpr.attributes = (yyt->ArraySliceExpr.skip ? NoExprAttributes :
		HandleArraySliceExpr (yyt->ArraySliceExpr.context, yyt->ArraySliceExpr.Expr->Expr.attributes, yyt->ArraySliceExpr.Range->Range.range, false , yyt->ArraySliceExpr.position));
break;

case kAsExpr:
yyt->AsExpr.Expr->Expr.context=yyt->AsExpr.context;
yyt->AsExpr.Expr->Expr.allowChannels=yyt->AsExpr.allowChannels;
# line 1032 "Balsa.ag"

	yyt->AsExpr.Expr->Expr.expectedType = NoType;
yyt->AsExpr.Expr->Expr.skip=yyt->AsExpr.skip;
yyVisit1 (yyt->AsExpr.Expr);
yyt->AsExpr.AType->AType.skip=yyt->AsExpr.skip;
yyt->AsExpr.AType->AType.context=yyt->AsExpr.context;
yyVisit1 (yyt->AsExpr.AType);
# line 1033 "Balsa.ag"

	yyt->AsExpr.attributes = (yyt->AsExpr.skip ? NoExprAttributes : HandleAsExpr (yyt->AsExpr.AType->AType.type, yyt->AsExpr.Expr->Expr.attributes, yyt->AsExpr.position));
break;

case kBitArrayCastExpr:
yyt->BitArrayCastExpr.Expr->Expr.context=yyt->BitArrayCastExpr.context;
yyt->BitArrayCastExpr.Expr->Expr.allowChannels=yyt->BitArrayCastExpr.allowChannels;
yyt->BitArrayCastExpr.Expr->Expr.expectedType=yyt->BitArrayCastExpr.expectedType;
yyt->BitArrayCastExpr.Expr->Expr.skip=yyt->BitArrayCastExpr.skip;
yyVisit1 (yyt->BitArrayCastExpr.Expr);
# line 1036 "Balsa.ag"

	yyt->BitArrayCastExpr.attributes = (yyt->BitArrayCastExpr.skip ? NoExprAttributes : HandleBitArrayCastExpr (yyt->BitArrayCastExpr.Expr->Expr.attributes, yyt->BitArrayCastExpr.position));
break;

case kLetExpr:
# line 1054 "Balsa.ag"

	yyt->LetExpr.ValDecls->ValDecls.contextIn = AddContextMarker(yyt->LetExpr.context);
yyt->LetExpr.ValDecls->ValDecls.skip=yyt->LetExpr.skip;
yyVisit1 (yyt->LetExpr.ValDecls);
# line 1055 "Balsa.ag"

	yyt->LetExpr.Expr->Expr.context = yyt->LetExpr.ValDecls->ValDecls.contextOut;
yyt->LetExpr.Expr->Expr.allowChannels=yyt->LetExpr.allowChannels;
yyt->LetExpr.Expr->Expr.expectedType=yyt->LetExpr.expectedType;
yyt->LetExpr.Expr->Expr.skip=yyt->LetExpr.skip;
yyVisit1 (yyt->LetExpr.Expr);
# line 1057 "Balsa.ag"

		yyt->LetExpr.attributes = yyt->LetExpr.Expr->Expr.attributes;
		
		if (!yyt->LetExpr.skip)
		{
			yyt->LetExpr.attributes.accesses = ResolveLocalInstanceAccesses (yyt->LetExpr.ValDecls->ValDecls.contextOut, 
				&yyt->LetExpr.attributes.wires, &yyt->LetExpr.attributes.components, yyt->LetExpr.attributes.accesses, NULL, yyt->LetExpr.position);
		}
	
break;

case kFunctionCallExpr:
# line 1070 "Balsa.ag"

	yyt->FunctionCallExpr.FunctionParams->FunctionParams.params = NULL;
yyt->FunctionCallExpr.FunctionParams->FunctionParams.context=yyt->FunctionCallExpr.context;
# line 1075 "Balsa.ag"

		yyt->FunctionCallExpr.FunctionParams->FunctionParams.functionTree = NULL;
		yyt->FunctionCallExpr.FunctionParams->FunctionParams.formalParams = NULL;
		yyt->FunctionCallExpr.FunctionParams->FunctionParams.formalPorts = NULL;
		yyt->FunctionCallExpr.FunctionParams->FunctionParams.idents = NULL;
		yyt->FunctionCallExpr.FunctionParams->FunctionParams.functionIn = (yyt->FunctionCallExpr.skip ? NoProcedure : LookupProcedure
			(yyt->FunctionCallExpr.context, yyt->FunctionCallExpr.ident, false  ));
		yyt->FunctionCallExpr.FunctionParams->FunctionParams.skip = yyt->FunctionCallExpr.skip;

		if (!yyt->FunctionCallExpr.skip)
		{
			if (! yyt->FunctionCallExpr.FunctionParams->FunctionParams.functionIn ||
				yyt->FunctionCallExpr.FunctionParams->FunctionParams.functionIn == NoProcedure)
			{
				LOG_ERROR (ExpectingAFunctionName, yyt->FunctionCallExpr.ident, yyt->FunctionCallExpr.position);
				yyt->FunctionCallExpr.FunctionParams->FunctionParams.skip = true;
			} else {
				switch (yyt->FunctionCallExpr.FunctionParams->FunctionParams.functionIn->nature)
				{
					case SharedFunction:
					case UnSharedFunction:
					case BuiltinFunction:
						yyt->FunctionCallExpr.FunctionParams->FunctionParams.formalPorts = yyt->FunctionCallExpr.FunctionParams->FunctionParams.functionIn->ports;
						break;
					case ParameterisedBuiltinFunction:
						{
							tTree tree = yyt->FunctionCallExpr.FunctionParams->FunctionParams.functionIn->info.parameterisedFunction.tree;
							tTree oldNext = tree->Decl.next;
							
							tree->Decl.next = mNullDecls (yyt->FunctionCallExpr.position);
							yyt->FunctionCallExpr.FunctionParams->FunctionParams.functionTree = CopyTree (tree);
							tree->Decl.next = oldNext;					

							yyt->FunctionCallExpr.FunctionParams->FunctionParams.formalParams =
								yyt->FunctionCallExpr.FunctionParams->FunctionParams.functionTree->BuiltinFunctionDecl.FormalPorts;
							yyt->FunctionCallExpr.FunctionParams->FunctionParams.idents = (yyt->FunctionCallExpr.FunctionParams->FunctionParams.formalParams->Kind == kParamPort
								?  yyt->FunctionCallExpr.FunctionParams->FunctionParams.formalParams->ParamPort.Idents->Idents.idents
								: yyt->FunctionCallExpr.FunctionParams->FunctionParams.formalParams->TypeParamPort.Idents->Idents.idents);
						}
						break;
					default:
						LOG_ERROR (ExpectingAFunctionName, yyt->FunctionCallExpr.ident, yyt->FunctionCallExpr.position);
						break;
				}
			}
		}
	
# line 1075 "Balsa.ag"
# line 1068 "Balsa.ag"

	yyt->FunctionCallExpr.FunctionParams->FunctionParams.constantParams = NULL;
# line 1069 "Balsa.ag"

	yyt->FunctionCallExpr.FunctionParams->FunctionParams.typeParams = NULL;
# line 1071 "Balsa.ag"

	yyt->FunctionCallExpr.FunctionParams->FunctionParams.parameterContext = yyt->FunctionCallExpr.context;
yyVisit1 (yyt->FunctionCallExpr.FunctionParams);
# line 1122 "Balsa.ag"

		yyt->FunctionCallExpr.attributes = NoExprAttributes;

		if (! yyt->FunctionCallExpr.skip && yyt->FunctionCallExpr.FunctionParams->FunctionParams.functionOut != NoProcedure)
			yyt->FunctionCallExpr.attributes = HandleFunctionCallExpr (yyt->FunctionCallExpr.FunctionParams->FunctionParams.actualPorts, yyt->FunctionCallExpr.FunctionParams->FunctionParams.functionOut, yyt->FunctionCallExpr.position); 
	
break;

case kSizeofExpr:
# line 1130 "Balsa.ag"
{
		PtrType type = LookupType (yyt->SizeofExpr.context, yyt->SizeofExpr.ident, false );

		yyt->SizeofExpr.attributes = (type == NoType ? NoExprAttributes
			: HandleLiteralExpr (NewMP_INT (Abs (type->size)), yyt->SizeofExpr.expectedType, yyt->SizeofExpr.position));
	}
break;

case kArrayAppendExpr:
yyt->ArrayAppendExpr.Left->Expr.context=yyt->ArrayAppendExpr.context;
yyt->ArrayAppendExpr.Left->Expr.allowChannels=yyt->ArrayAppendExpr.allowChannels;
# line 1006 "Balsa.ag"

	yyt->ArrayAppendExpr.Left->Expr.expectedType = yyt->ArrayAppendExpr.expectedType;
yyt->ArrayAppendExpr.Left->Expr.skip=yyt->ArrayAppendExpr.skip;
yyVisit1 (yyt->ArrayAppendExpr.Left);
yyt->ArrayAppendExpr.Right->Expr.allowChannels=yyt->ArrayAppendExpr.allowChannels;
yyt->ArrayAppendExpr.Right->Expr.context=yyt->ArrayAppendExpr.context;
# line 1007 "Balsa.ag"
 
	yyt->ArrayAppendExpr.Right->Expr.expectedType = yyt->ArrayAppendExpr.Left->Expr.attributes.value.type;
yyt->ArrayAppendExpr.Right->Expr.skip=yyt->ArrayAppendExpr.skip;
yyVisit1 (yyt->ArrayAppendExpr.Right);
# line 1009 "Balsa.ag"

		yyt->ArrayAppendExpr.attributes = NoExprAttributes;

		if (!yyt->ArrayAppendExpr.skip)
		{
			if (yyt->ArrayAppendExpr.Left->Expr.attributes.value.type->nature != ArrayType)
				LOG_ERROR (ExpectingAnArrayOnAppendLHS, NoIdent, yyt->ArrayAppendExpr.position);
			else if (yyt->ArrayAppendExpr.Right->Expr.attributes.value.type->nature != ArrayType)
				LOG_ERROR (ExpectingAnArrayOnAppendRHS, NoIdent, yyt->ArrayAppendExpr.position);
			else if (!TypeEquivalence (yyt->ArrayAppendExpr.Left->Expr.attributes.value.type->info.array.baseType,
				yyt->ArrayAppendExpr.Right->Expr.attributes.value.type->info.array.baseType))
				LOG_ERROR (ArrayAppendExprsMustHaveSameBaseType, NoIdent, yyt->ArrayAppendExpr.position);
			else if (yyt->ArrayAppendExpr.Left->Expr.attributes.value.type != NoType && yyt->ArrayAppendExpr.Right->Expr.attributes.value.type != NoType)
			{
				yyt->ArrayAppendExpr.attributes = HandleAggregateConsExpr (yyt->ArrayAppendExpr.Left->Expr.attributes.value.type, false
					, true, 
					NewExprAttributesList (yyt->ArrayAppendExpr.Left->Expr.attributes, NewExprAttributesList (yyt->ArrayAppendExpr.Right->Expr.attributes, NULL)),
					yyt->ArrayAppendExpr.position); 
			}
		}
	
break;

case kPrimedExpr:
# line 1139 "Balsa.ag"

	
	yyt->PrimedExpr.attributes = yyt->PrimedExpr.results;
break;

case kFormalPorts:
# line 2215 "Balsa.ag"
 yyt->FormalPorts.ports = NULL;
# line 2215 "Balsa.ag"
 yyt->FormalPorts.hasParameters = false;
# line 2215 "Balsa.ag"
 yyt->FormalPorts.extraPortInstances = NULL;
# line 2215 "Balsa.ag"
 yyt->FormalPorts.portCount = 0;
break;

case kNullFormalPorts:
# line 2215 "Balsa.ag"
 yyt->NullFormalPorts.ports = NULL;
# line 2215 "Balsa.ag"
 yyt->NullFormalPorts.hasParameters = false;
# line 2215 "Balsa.ag"
 yyt->NullFormalPorts.extraPortInstances = NULL;
# line 2215 "Balsa.ag"
 yyt->NullFormalPorts.portCount = 0;
break;

case kFormalPort:
# line 2215 "Balsa.ag"
 yyt->FormalPort.ports = NULL;
yyt->FormalPort.next->FormalPorts.inConditionalPorts=yyt->FormalPort.inConditionalPorts;
yyt->FormalPort.next->FormalPorts.portsType=yyt->FormalPort.portsType;
yyt->FormalPort.next->FormalPorts.skip=yyt->FormalPort.skip;
yyt->FormalPort.next->FormalPorts.context=yyt->FormalPort.context;
yyVisit1 (yyt->FormalPort.next);
# line 2217 "Balsa.ag"

		if (yyt->FormalPort.next->FormalPorts.hasParameters) LOG_ERROR (ParametersMustBeAtStartOfPorts, NoIdent, yyt->FormalPort.position);
		yyt->FormalPort.hasParameters = false;
	
# line 2215 "Balsa.ag"
 yyt->FormalPort.extraPortInstances = NULL;
# line 2215 "Balsa.ag"
 yyt->FormalPort.portCount = 0;
break;

case kValuePort:
yyt->ValuePort.next->FormalPorts.context=yyt->ValuePort.context;
yyt->ValuePort.next->FormalPorts.skip=yyt->ValuePort.skip;
yyt->ValuePort.next->FormalPorts.inConditionalPorts=yyt->ValuePort.inConditionalPorts;
yyt->ValuePort.next->FormalPorts.portsType=yyt->ValuePort.portsType;
yyVisit1 (yyt->ValuePort.next);
yyt->ValuePort.AType->AType.skip=yyt->ValuePort.skip;
yyt->ValuePort.AType->AType.context=yyt->ValuePort.context;
yyVisit1 (yyt->ValuePort.AType);
yyVisit1 (yyt->ValuePort.Idents);
# line 2223 "Balsa.ag"

		yyt->ValuePort.portCount = yyt->ValuePort.next->FormalPorts.portCount;
		yyt->ValuePort.extraPortInstances = yyt->ValuePort.next->FormalPorts.extraPortInstances;  
		yyt->ValuePort.ports = (yyt->ValuePort.skip ? yyt->ValuePort.next->FormalPorts.ports : HandleValuePorts (yyt->ValuePort.Idents->Idents.idents, yyt->ValuePort.AType->AType.type, yyt->ValuePort.portsType,
			yyt->ValuePort.next->FormalPorts.ports, &yyt->ValuePort.portCount)); 
	
# line 2217 "Balsa.ag"
		if (yyt->ValuePort.next->FormalPorts.hasParameters) LOG_ERROR (ParametersMustBeAtStartOfPorts, NoIdent, yyt->ValuePort.position);
		yyt->ValuePort.hasParameters = false;
	
break;

case kParamPort:
yyt->ParamPort.next->FormalPorts.context=yyt->ParamPort.context;
# line 2232 "Balsa.ag"

	yyt->ParamPort.next->FormalPorts.skip = true;
yyt->ParamPort.next->FormalPorts.inConditionalPorts=yyt->ParamPort.inConditionalPorts;
yyt->ParamPort.next->FormalPorts.portsType=yyt->ParamPort.portsType;
yyVisit1 (yyt->ParamPort.next);
yyVisit1 (yyt->ParamPort.Idents);
# line 2235 "Balsa.ag"

		
		yyt->ParamPort.Idents->Idents.idents;
		yyt->ParamPort.ports = yyt->ParamPort.next->FormalPorts.ports;
		yyt->ParamPort.portCount = yyt->ParamPort.next->FormalPorts.portCount;
		yyt->ParamPort.extraPortInstances = yyt->ParamPort.next->FormalPorts.extraPortInstances;

		if (yyt->ParamPort.inConditionalPorts)
			LOG_ERROR (ParametersCantBeConditional, NoIdent, yyt->ParamPort.position);
	
# line 2233 "Balsa.ag"

	yyt->ParamPort.AType->AType.skip = true;
yyt->ParamPort.AType->AType.context=yyt->ParamPort.context;
yyVisit1 (yyt->ParamPort.AType);
# line 2234 "Balsa.ag"

	yyt->ParamPort.hasParameters = true;
break;

case kTypeParamPort:
yyt->TypeParamPort.next->FormalPorts.context=yyt->TypeParamPort.context;
# line 2247 "Balsa.ag"

	yyt->TypeParamPort.next->FormalPorts.skip = true;
yyt->TypeParamPort.next->FormalPorts.inConditionalPorts=yyt->TypeParamPort.inConditionalPorts;
yyt->TypeParamPort.next->FormalPorts.portsType=yyt->TypeParamPort.portsType;
yyVisit1 (yyt->TypeParamPort.next);
yyVisit1 (yyt->TypeParamPort.Idents);
# line 2249 "Balsa.ag"

		
		yyt->TypeParamPort.Idents->Idents.idents;
		yyt->TypeParamPort.ports = yyt->TypeParamPort.next->FormalPorts.ports;
		yyt->TypeParamPort.portCount = yyt->TypeParamPort.next->FormalPorts.portCount;
		yyt->TypeParamPort.extraPortInstances = yyt->TypeParamPort.next->FormalPorts.extraPortInstances;

		if (yyt->TypeParamPort.inConditionalPorts)
			LOG_ERROR (ParametersCantBeConditional, NoIdent, yyt->TypeParamPort.position);
	
# line 2248 "Balsa.ag"

	yyt->TypeParamPort.hasParameters = true;
break;

case kChannelPort:
yyt->ChannelPort.next->FormalPorts.context=yyt->ChannelPort.context;
yyt->ChannelPort.next->FormalPorts.skip=yyt->ChannelPort.skip;
yyt->ChannelPort.next->FormalPorts.inConditionalPorts=yyt->ChannelPort.inConditionalPorts;
yyt->ChannelPort.next->FormalPorts.portsType=yyt->ChannelPort.portsType;
yyVisit1 (yyt->ChannelPort.next);
yyt->ChannelPort.AType->AType.skip=yyt->ChannelPort.skip;
yyt->ChannelPort.AType->AType.context=yyt->ChannelPort.context;
yyVisit1 (yyt->ChannelPort.AType);
yyVisit1 (yyt->ChannelPort.Idents);
# line 2261 "Balsa.ag"

		yyt->ChannelPort.portCount = yyt->ChannelPort.next->FormalPorts.portCount;
		yyt->ChannelPort.extraPortInstances = yyt->ChannelPort.next->FormalPorts.extraPortInstances; 
		yyt->ChannelPort.ports = (yyt->ChannelPort.skip ? yyt->ChannelPort.next->FormalPorts.ports : HandleChannelPorts (yyt->ChannelPort.Idents->Idents.idents, yyt->ChannelPort.AType->AType.type, yyt->ChannelPort.portSense, yyt->ChannelPort.portsType, 
			yyt->ChannelPort.isOutput, false , NoSpan, yyt->ChannelPort.next->FormalPorts.ports, &yyt->ChannelPort.extraPortInstances, &yyt->ChannelPort.portCount,
			yyt->ChannelPort.options, yyt->ChannelPort.position));
	
# line 2217 "Balsa.ag"

		if (yyt->ChannelPort.next->FormalPorts.hasParameters) LOG_ERROR (ParametersMustBeAtStartOfPorts, NoIdent, yyt->ChannelPort.position);
		yyt->ChannelPort.hasParameters = false;
	
break;

case kChannelPortArray:
yyt->ChannelPortArray.next->FormalPorts.context=yyt->ChannelPortArray.context;
yyt->ChannelPortArray.next->FormalPorts.skip=yyt->ChannelPortArray.skip;
yyt->ChannelPortArray.next->FormalPorts.inConditionalPorts=yyt->ChannelPortArray.inConditionalPorts;
yyt->ChannelPortArray.next->FormalPorts.portsType=yyt->ChannelPortArray.portsType;
yyVisit1 (yyt->ChannelPortArray.next);
# line 2281 "Balsa.ag"
 
	yyt->ChannelPortArray.Range->Range.isSpan = false;
# line 2282 "Balsa.ag"
 
	yyt->ChannelPortArray.Range->Range.expectedType = NoType;
yyt->ChannelPortArray.Range->Range.skip=yyt->ChannelPortArray.skip;
yyt->ChannelPortArray.Range->Range.context=yyt->ChannelPortArray.context;
yyVisit1 (yyt->ChannelPortArray.Range);
yyt->ChannelPortArray.AType->AType.skip=yyt->ChannelPortArray.skip;
yyt->ChannelPortArray.AType->AType.context=yyt->ChannelPortArray.context;
yyVisit1 (yyt->ChannelPortArray.AType);
yyVisit1 (yyt->ChannelPortArray.Idents);
# line 2283 "Balsa.ag"

		yyt->ChannelPortArray.extraPortInstances = yyt->ChannelPortArray.next->FormalPorts.extraPortInstances;
		yyt->ChannelPortArray.portCount = yyt->ChannelPortArray.next->FormalPorts.portCount;
		yyt->ChannelPortArray.ports = (yyt->ChannelPortArray.skip ? yyt->ChannelPortArray.next->FormalPorts.ports : HandleChannelPorts (yyt->ChannelPortArray.Idents->Idents.idents, yyt->ChannelPortArray.AType->AType.type, yyt->ChannelPortArray.portSense, yyt->ChannelPortArray.portsType, 
			yyt->ChannelPortArray.isOutput, true , yyt->ChannelPortArray.Range->Range.range, yyt->ChannelPortArray.next->FormalPorts.ports, &yyt->ChannelPortArray.extraPortInstances, &yyt->ChannelPortArray.portCount,
			yyt->ChannelPortArray.options, yyt->ChannelPortArray.position)); 
	
# line 2217 "Balsa.ag"

		if (yyt->ChannelPortArray.next->FormalPorts.hasParameters) LOG_ERROR (ParametersMustBeAtStartOfPorts, NoIdent, yyt->ChannelPortArray.position);
		yyt->ChannelPortArray.hasParameters = false;
	
break;

case kSyncPort:
yyt->SyncPort.next->FormalPorts.context=yyt->SyncPort.context;
yyt->SyncPort.next->FormalPorts.skip=yyt->SyncPort.skip;
yyt->SyncPort.next->FormalPorts.inConditionalPorts=yyt->SyncPort.inConditionalPorts;
yyt->SyncPort.next->FormalPorts.portsType=yyt->SyncPort.portsType;
yyVisit1 (yyt->SyncPort.next);
yyVisit1 (yyt->SyncPort.Idents);
# line 2271 "Balsa.ag"

		yyt->SyncPort.portCount = yyt->SyncPort.next->FormalPorts.portCount;
		yyt->SyncPort.extraPortInstances = yyt->SyncPort.next->FormalPorts.extraPortInstances; 
		yyt->SyncPort.ports = (yyt->SyncPort.skip ? yyt->SyncPort.next->FormalPorts.ports :
			HandleChannelPorts (yyt->SyncPort.Idents->Idents.idents, SyncTypeObj, yyt->SyncPort.portSense, yyt->SyncPort.portsType, false ,
			false , NoSpan, yyt->SyncPort.next->FormalPorts.ports, &yyt->SyncPort.extraPortInstances, &yyt->SyncPort.portCount,
			yyt->SyncPort.options, yyt->SyncPort.position)); 
	
# line 2217 "Balsa.ag"

		if (yyt->SyncPort.next->FormalPorts.hasParameters) LOG_ERROR (ParametersMustBeAtStartOfPorts, NoIdent, yyt->SyncPort.position);
		yyt->SyncPort.hasParameters = false;
	
break;

case kSyncPortArray:
yyt->SyncPortArray.next->FormalPorts.context=yyt->SyncPortArray.context;
yyt->SyncPortArray.next->FormalPorts.skip=yyt->SyncPortArray.skip;
yyt->SyncPortArray.next->FormalPorts.inConditionalPorts=yyt->SyncPortArray.inConditionalPorts;
yyt->SyncPortArray.next->FormalPorts.portsType=yyt->SyncPortArray.portsType;
yyVisit1 (yyt->SyncPortArray.next);
# line 2292 "Balsa.ag"
 
	yyt->SyncPortArray.Range->Range.isSpan = false;
# line 2293 "Balsa.ag"
 
	yyt->SyncPortArray.Range->Range.expectedType = NoType;
yyt->SyncPortArray.Range->Range.skip=yyt->SyncPortArray.skip;
yyt->SyncPortArray.Range->Range.context=yyt->SyncPortArray.context;
yyVisit1 (yyt->SyncPortArray.Range);
yyVisit1 (yyt->SyncPortArray.Idents);
# line 2294 "Balsa.ag"

		yyt->SyncPortArray.extraPortInstances = yyt->SyncPortArray.next->FormalPorts.extraPortInstances; 
		yyt->SyncPortArray.portCount = yyt->SyncPortArray.next->FormalPorts.portCount;
		yyt->SyncPortArray.ports = (yyt->SyncPortArray.skip ? yyt->SyncPortArray.next->FormalPorts.ports :
			HandleChannelPorts (yyt->SyncPortArray.Idents->Idents.idents, SyncTypeObj, yyt->SyncPortArray.portSense, yyt->SyncPortArray.portsType, false ,
			true , yyt->SyncPortArray.Range->Range.range, yyt->SyncPortArray.next->FormalPorts.ports, &yyt->SyncPortArray.extraPortInstances, &yyt->SyncPortArray.portCount,
			yyt->SyncPortArray.options, yyt->SyncPortArray.position)); 
	
# line 2217 "Balsa.ag"

		if (yyt->SyncPortArray.next->FormalPorts.hasParameters) LOG_ERROR (ParametersMustBeAtStartOfPorts, NoIdent, yyt->SyncPortArray.position);
		yyt->SyncPortArray.hasParameters = false;
	
break;

case kIfPorts:
yyt->IfPorts.next->FormalPorts.context=yyt->IfPorts.context;
yyt->IfPorts.next->FormalPorts.skip=yyt->IfPorts.skip;
yyt->IfPorts.next->FormalPorts.inConditionalPorts=yyt->IfPorts.inConditionalPorts;
yyt->IfPorts.next->FormalPorts.portsType=yyt->IfPorts.portsType;
yyVisit1 (yyt->IfPorts.next);
yyt->IfPorts.PortGuards->PortGuards.portsType=yyt->IfPorts.portsType;
yyt->IfPorts.PortGuards->PortGuards.skip=yyt->IfPorts.skip;
yyt->IfPorts.PortGuards->PortGuards.context=yyt->IfPorts.context;
yyVisit1 (yyt->IfPorts.PortGuards);
# line 2304 "Balsa.ag"

		yyt->IfPorts.portCount = yyt->IfPorts.next->FormalPorts.portCount;
		yyt->IfPorts.extraPortInstances = yyt->IfPorts.next->FormalPorts.extraPortInstances; 
		yyt->IfPorts.ports = yyt->IfPorts.next->FormalPorts.ports;
		yyt->IfPorts.hasParameters = yyt->IfPorts.next->FormalPorts.hasParameters;

		if (! yyt->IfPorts.skip)
		{
			if (yyt->IfPorts.PortGuards->PortGuards.ports)
			{
				yyt->IfPorts.portCount = yyt->IfPorts.PortGuards->PortGuards.portCount + yyt->IfPorts.next->FormalPorts.portCount;
				yyt->IfPorts.extraPortInstances = AppendInstanceLists (CopyInstanceList (yyt->IfPorts.PortGuards->PortGuards.extraPortInstances),
					yyt->IfPorts.next->FormalPorts.extraPortInstances);
				yyt->IfPorts.ports = AppendInstanceLists (CopyInstanceList (yyt->IfPorts.PortGuards->PortGuards.ports), yyt->IfPorts.next->FormalPorts.ports);
				yyt->IfPorts.hasParameters = yyt->IfPorts.PortGuards->PortGuards.hasParameters || yyt->IfPorts.next->FormalPorts.hasParameters;
			}
		}
	
break;

case kIfElsePorts:
yyt->IfElsePorts.next->FormalPorts.context=yyt->IfElsePorts.context;
yyt->IfElsePorts.next->FormalPorts.skip=yyt->IfElsePorts.skip;
yyt->IfElsePorts.next->FormalPorts.inConditionalPorts=yyt->IfElsePorts.inConditionalPorts;
yyt->IfElsePorts.next->FormalPorts.portsType=yyt->IfElsePorts.portsType;
yyVisit1 (yyt->IfElsePorts.next);
yyt->IfElsePorts.FormalPorts->FormalPorts.inConditionalPorts=yyt->IfElsePorts.inConditionalPorts;
yyt->IfElsePorts.FormalPorts->FormalPorts.portsType=yyt->IfElsePorts.portsType;
yyt->IfElsePorts.FormalPorts->FormalPorts.skip=yyt->IfElsePorts.skip;
yyt->IfElsePorts.FormalPorts->FormalPorts.context=yyt->IfElsePorts.context;
yyVisit1 (yyt->IfElsePorts.FormalPorts);
yyt->IfElsePorts.PortGuards->PortGuards.portsType=yyt->IfElsePorts.portsType;
yyt->IfElsePorts.PortGuards->PortGuards.skip=yyt->IfElsePorts.skip;
yyt->IfElsePorts.PortGuards->PortGuards.context=yyt->IfElsePorts.context;
yyVisit1 (yyt->IfElsePorts.PortGuards);
# line 2327 "Balsa.ag"

		yyt->IfElsePorts.portCount = yyt->IfElsePorts.next->FormalPorts.portCount;
		yyt->IfElsePorts.extraPortInstances = yyt->IfElsePorts.next->FormalPorts.extraPortInstances; 
		yyt->IfElsePorts.ports = yyt->IfElsePorts.next->FormalPorts.ports;
		yyt->IfElsePorts.hasParameters = yyt->IfElsePorts.next->FormalPorts.hasParameters;

		if (! yyt->IfElsePorts.skip)
		{
			if (yyt->IfElsePorts.PortGuards->PortGuards.ports)
			{
				yyt->IfElsePorts.portCount = yyt->IfElsePorts.PortGuards->PortGuards.portCount + yyt->IfElsePorts.next->FormalPorts.portCount;
				yyt->IfElsePorts.extraPortInstances = AppendInstanceLists (CopyInstanceList (yyt->IfElsePorts.PortGuards->PortGuards.extraPortInstances),
					yyt->IfElsePorts.next->FormalPorts.extraPortInstances);
				yyt->IfElsePorts.ports = AppendInstanceLists (CopyInstanceList (yyt->IfElsePorts.PortGuards->PortGuards.ports), yyt->IfElsePorts.next->FormalPorts.ports);
				yyt->IfElsePorts.hasParameters = yyt->IfElsePorts.PortGuards->PortGuards.hasParameters || yyt->IfElsePorts.next->FormalPorts.hasParameters;
			} else {
			  
				yyt->IfElsePorts.portCount = yyt->IfElsePorts.FormalPorts->FormalPorts.portCount + yyt->IfElsePorts.next->FormalPorts.portCount;
				yyt->IfElsePorts.extraPortInstances = AppendInstanceLists (CopyInstanceList (yyt->IfElsePorts.FormalPorts->FormalPorts.extraPortInstances),
					yyt->IfElsePorts.next->FormalPorts.extraPortInstances);
				yyt->IfElsePorts.ports = AppendInstanceLists (CopyInstanceList (yyt->IfElsePorts.FormalPorts->FormalPorts.ports), yyt->IfElsePorts.next->FormalPorts.ports);
				yyt->IfElsePorts.hasParameters = yyt->IfElsePorts.FormalPorts->FormalPorts.hasParameters || yyt->IfElsePorts.next->FormalPorts.hasParameters;
			}
		}
	
break;

case kBlock:
# line 1996 "Balsa.ag"

	yyt->Block.Decls->Decls.scope = InnerScope;
yyt->Block.Decls->Decls.skip=yyt->Block.skip;
# line 1995 "Balsa.ag"

	yyt->Block.Decls->Decls.contextIn = yyt->Block.context;
yyVisit1 (yyt->Block.Decls);
yyt->Block.Command->Command.skip=yyt->Block.skip;
# line 1997 "Balsa.ag"
 
	yyt->Block.Command->Command.context = yyt->Block.Decls->Decls.contextOut;
yyVisit1 (yyt->Block.Command);
# line 1998 "Balsa.ag"

		yyt->Block.attributes = yyt->Block.Command->Command.attributes;
		
		if (!yyt->Block.skip)
		{
			PtrWireList initWires = NULL;

			yyt->Block.attributes.accesses = ResolveLocalSharedCallAccesses (yyt->Block.Decls->Decls.contextOut,
				&yyt->Block.attributes.wires, &yyt->Block.attributes.components, yyt->Block.attributes.accesses, yyt->Block.position);
			yyt->Block.attributes.accesses = ResolveLocalInstanceAccesses (yyt->Block.Decls->Decls.contextOut, 
				&yyt->Block.attributes.wires, &yyt->Block.attributes.components, yyt->Block.attributes.accesses, &initWires, yyt->Block.position);

			if ((yyt->Block.Command->Command.attributes).activation && initWires)
			{
				yyt->Block.attributes.activation = SequenceBlockInitialisationAndCommand ((yyt->Block.Command->Command.attributes).activation,
				  &yyt->Block.attributes.wires, &yyt->Block.attributes.components, initWires, yyt->Block.position);
			}
		}
	
break;

case kParameters:
yyt->Parameters.paramNoOut=yyt->Parameters.paramNoIn;
break;

case kNullParameters:
yyt->NullParameters.paramNoOut=yyt->NullParameters.paramNoIn;
break;

case kParameter:
yyt->Parameter.next->Parameters.context=yyt->Parameter.context;
yyt->Parameter.next->Parameters.component=yyt->Parameter.component;
yyt->Parameter.next->Parameters.parameters=yyt->Parameter.parameters;
yyt->Parameter.next->Parameters.paramNoIn=yyt->Parameter.paramNoIn;
yyt->Parameter.next->Parameters.skip=yyt->Parameter.skip;
yyVisit1 (yyt->Parameter.next);
yyt->Parameter.paramNoOut=yyt->Parameter.next->Parameters.paramNoOut;
break;

case kNumberParameter:
yyt->NumberParameter.next->Parameters.context=yyt->NumberParameter.context;
yyt->NumberParameter.next->Parameters.component=yyt->NumberParameter.component;
yyt->NumberParameter.next->Parameters.parameters=yyt->NumberParameter.parameters;
# line 3060 "Balsa.ag"

		yyt->NumberParameter.next->Parameters.paramNoIn = yyt->NumberParameter.paramNoIn + 1;
		if (! yyt->NumberParameter.skip) HandleParameter ("i#", yyt->NumberParameter.paramNoIn, yyt->NumberParameter.component, (void *) yyt->NumberParameter.literal, yyt->NumberParameter.context,
			yyt->NumberParameter.parameters, yyt->NumberParameter.position);
	
yyt->NumberParameter.next->Parameters.skip=yyt->NumberParameter.skip;
yyVisit1 (yyt->NumberParameter.next);
yyt->NumberParameter.paramNoOut=yyt->NumberParameter.next->Parameters.paramNoOut;
break;

case kStringParameter:
yyt->StringParameter.next->Parameters.context=yyt->StringParameter.context;
yyt->StringParameter.next->Parameters.component=yyt->StringParameter.component;
yyt->StringParameter.next->Parameters.parameters=yyt->StringParameter.parameters;
# line 3067 "Balsa.ag"

		yyt->StringParameter.next->Parameters.paramNoIn = yyt->StringParameter.paramNoIn + 1;
		if (! yyt->StringParameter.skip) HandleParameter ("BUb$", yyt->StringParameter.paramNoIn, yyt->StringParameter.component, (void *) yyt->StringParameter.ident, yyt->StringParameter.context,
			yyt->StringParameter.parameters, yyt->StringParameter.position);
	
yyt->StringParameter.next->Parameters.skip=yyt->StringParameter.skip;
yyVisit1 (yyt->StringParameter.next);
yyt->StringParameter.paramNoOut=yyt->StringParameter.next->Parameters.paramNoOut;
break;

case kTypeParameter:
yyt->TypeParameter.next->Parameters.context=yyt->TypeParameter.context;
yyt->TypeParameter.AType->AType.skip=yyt->TypeParameter.skip;
yyt->TypeParameter.AType->AType.context=yyt->TypeParameter.context;
yyVisit1 (yyt->TypeParameter.AType);
yyt->TypeParameter.next->Parameters.component=yyt->TypeParameter.component;
yyt->TypeParameter.next->Parameters.parameters=yyt->TypeParameter.parameters;
# line 3074 "Balsa.ag"

		yyt->TypeParameter.next->Parameters.paramNoIn = yyt->TypeParameter.paramNoIn + 1;
		if (! yyt->TypeParameter.skip) HandleParameter ("t", yyt->TypeParameter.paramNoIn, yyt->TypeParameter.component, (void *) yyt->TypeParameter.AType->AType.type, yyt->TypeParameter.context,
			yyt->TypeParameter.parameters, yyt->TypeParameter.position);
	
yyt->TypeParameter.next->Parameters.skip=yyt->TypeParameter.skip;
yyVisit1 (yyt->TypeParameter.next);
yyt->TypeParameter.paramNoOut=yyt->TypeParameter.next->Parameters.paramNoOut;
break;

case kBreezeParameters:
# line 3082 "Balsa.ag"
 yyt->BreezeParameters.parameters = NULL;
break;

case kNullBreezeParameters:
# line 3082 "Balsa.ag"
 yyt->NullBreezeParameters.parameters = NULL;
break;

case kBreezeParameter:
# line 3082 "Balsa.ag"
 yyt->BreezeParameter.parameters = NULL;
yyt->BreezeParameter.next->BreezeParameters.skip=yyt->BreezeParameter.skip;
yyt->BreezeParameter.next->BreezeParameters.context=yyt->BreezeParameter.context;
yyVisit1 (yyt->BreezeParameter.next);
break;

case kBreezeExprParameter:
yyt->BreezeExprParameter.next->BreezeParameters.context=yyt->BreezeExprParameter.context;
yyt->BreezeExprParameter.next->BreezeParameters.skip=yyt->BreezeExprParameter.skip;
yyVisit1 (yyt->BreezeExprParameter.next);
yyt->BreezeExprParameter.AType->AType.skip=yyt->BreezeExprParameter.skip;
yyt->BreezeExprParameter.AType->AType.context=yyt->BreezeExprParameter.context;
yyVisit1 (yyt->BreezeExprParameter.AType);
# line 3084 "Balsa.ag"

		yyt->BreezeExprParameter.parameters = yyt->BreezeExprParameter.next->BreezeParameters.parameters;

		if (! yyt->BreezeExprParameter.skip)
		{
			yyt->BreezeExprParameter.parameters = NewComponentParameterList (NewComponentParameter
				((yyt->BreezeExprParameter.AType->AType.type == StringTypeObj ? StringComponentParameter : NumberComponentParameter),
				NULL, yyt->BreezeExprParameter.ident, yyt->BreezeExprParameter.AType->AType.type), yyt->BreezeExprParameter.next->BreezeParameters.parameters);
		}
	
break;

case kBreezeTypeParameter:
yyt->BreezeTypeParameter.next->BreezeParameters.context=yyt->BreezeTypeParameter.context;
yyt->BreezeTypeParameter.next->BreezeParameters.skip=yyt->BreezeTypeParameter.skip;
yyVisit1 (yyt->BreezeTypeParameter.next);
# line 3096 "Balsa.ag"

		yyt->BreezeTypeParameter.parameters = yyt->BreezeTypeParameter.next->BreezeParameters.parameters;

		if (! yyt->BreezeTypeParameter.skip)
		{
			yyt->BreezeTypeParameter.parameters = NewComponentParameterList (NewComponentParameter
				(TypeComponentParameter, NULL, yyt->BreezeTypeParameter.ident, NoType), yyt->BreezeTypeParameter.next->BreezeParameters.parameters);
		}
	
break;

case kComps:
yyt->Comps.componentsOut=yyt->Comps.componentsIn;
break;

case kNullComps:
yyt->NullComps.componentsOut=yyt->NullComps.componentsIn;
break;

case kComp:
yyt->Comp.next->Comps.context=yyt->Comp.context;
yyt->Comp.next->Comps.componentsIn=yyt->Comp.componentsIn;
yyt->Comp.next->Comps.channelCount=yyt->Comp.channelCount;
yyt->Comp.next->Comps.channels=yyt->Comp.channels;
yyt->Comp.next->Comps.skip=yyt->Comp.skip;
yyVisit1 (yyt->Comp.next);
yyt->Comp.componentsOut=yyt->Comp.next->Comps.componentsOut;
break;

case kNormalComp:
yyt->NormalComp.next->Comps.context=yyt->NormalComp.context;
# line 3109 "Balsa.ag"

	yyt->NormalComp.Parameters->Parameters.component = NewComponentFromComponentName (yyt->NormalComp.context, yyt->NormalComp.ident, yyt->NormalComp.internal, yyt->NormalComp.position);
# line 3111 "Balsa.ag"

	yyt->NormalComp.Parameters->Parameters.parameters = NULL;
# line 3110 "Balsa.ag"
 
	yyt->NormalComp.Parameters->Parameters.paramNoIn = 0;
yyt->NormalComp.Parameters->Parameters.skip=yyt->NormalComp.skip;
yyt->NormalComp.Parameters->Parameters.context=yyt->NormalComp.context;
yyVisit1 (yyt->NormalComp.Parameters);
# line 3113 "Balsa.ag"

		yyt->NormalComp.Parameters->Parameters.paramNoOut; 
		yyt->NormalComp.next->Comps.componentsIn = (yyt->NormalComp.skip ? NULL : HandleComp (yyt->NormalComp.componentsIn, yyt->NormalComp.Parameters->Parameters.component, yyt->NormalComp.channelNos,
			yyt->NormalComp.channels, yyt->NormalComp.position, yyt->NormalComp.options));
	
yyt->NormalComp.next->Comps.channelCount=yyt->NormalComp.channelCount;
yyt->NormalComp.next->Comps.channels=yyt->NormalComp.channels;
yyt->NormalComp.next->Comps.skip=yyt->NormalComp.skip;
yyVisit1 (yyt->NormalComp.next);
yyt->NormalComp.componentsOut=yyt->NormalComp.next->Comps.componentsOut;
break;

case kUndeclaredComp:
yyt->UndeclaredComp.next->Comps.context=yyt->UndeclaredComp.context;
# line 3122 "Balsa.ag"

	yyt->UndeclaredComp.FormalPorts->FormalPorts.inConditionalPorts = false;
# line 3121 "Balsa.ag"

	yyt->UndeclaredComp.FormalPorts->FormalPorts.portsType = PartPorts;
yyt->UndeclaredComp.FormalPorts->FormalPorts.skip=yyt->UndeclaredComp.skip;
yyt->UndeclaredComp.FormalPorts->FormalPorts.context=yyt->UndeclaredComp.context;
yyVisit1 (yyt->UndeclaredComp.FormalPorts);
yyt->UndeclaredComp.BreezeParameters->BreezeParameters.skip=yyt->UndeclaredComp.skip;
yyt->UndeclaredComp.BreezeParameters->BreezeParameters.context=yyt->UndeclaredComp.context;
yyVisit1 (yyt->UndeclaredComp.BreezeParameters);
# line 3123 "Balsa.ag"

		yyt->UndeclaredComp.Parameters->Parameters.component = NewComponent (yyt->UndeclaredComp.position, ProcedureComponent);
		yyt->UndeclaredComp.Parameters->Parameters.component->param.procedure.ident = yyt->UndeclaredComp.ident;
	
# line 3128 "Balsa.ag"

	yyt->UndeclaredComp.Parameters->Parameters.parameters = yyt->UndeclaredComp.BreezeParameters->BreezeParameters.parameters;
# line 3127 "Balsa.ag"

	yyt->UndeclaredComp.Parameters->Parameters.paramNoIn = 0;
yyt->UndeclaredComp.Parameters->Parameters.skip=yyt->UndeclaredComp.skip;
yyt->UndeclaredComp.Parameters->Parameters.context=yyt->UndeclaredComp.context;
yyVisit1 (yyt->UndeclaredComp.Parameters);
# line 3130 "Balsa.ag"

		yyt->UndeclaredComp.Parameters->Parameters.paramNoOut; 
		yyt->UndeclaredComp.next->Comps.componentsIn = (yyt->UndeclaredComp.skip ? NULL : HandleUndeclaredComp (yyt->UndeclaredComp.componentsIn, yyt->UndeclaredComp.Parameters->Parameters.component, yyt->UndeclaredComp.channelNos,
			yyt->UndeclaredComp.channels, yyt->UndeclaredComp.position, yyt->UndeclaredComp.componentType, yyt->UndeclaredComp.baseComponentName, yyt->UndeclaredComp.BreezeParameters->BreezeParameters.parameters,
			yyt->UndeclaredComp.FormalPorts->FormalPorts.ports, yyt->UndeclaredComp.options));
	
yyt->UndeclaredComp.next->Comps.channelCount=yyt->UndeclaredComp.channelCount;
yyt->UndeclaredComp.next->Comps.channels=yyt->UndeclaredComp.channels;
yyt->UndeclaredComp.next->Comps.skip=yyt->UndeclaredComp.skip;
yyVisit1 (yyt->UndeclaredComp.next);
yyt->UndeclaredComp.componentsOut=yyt->UndeclaredComp.next->Comps.componentsOut;
break;

case kCommand:
# line 1143 "Balsa.ag"
  yyt->Command.attributes = NoCommandAttributes;
break;

case kNullCommand:
# line 1143 "Balsa.ag"
  yyt->NullCommand.attributes = NoCommandAttributes;
break;

case kContinueCommand:
# line 1145 "Balsa.ag"
 yyt->ContinueCommand.attributes = HandleContinueCommand (yyt->ContinueCommand.position);
break;

case kHaltCommand:
# line 1146 "Balsa.ag"
 yyt->HaltCommand.attributes = HandleHaltCommand (yyt->HaltCommand.position);
break;

case kInputCommand:
yyt->InputCommand.LHS->Lvalue.context=yyt->InputCommand.context;
# line 1306 "Balsa.ag"
 
	yyt->InputCommand.LHS->Lvalue.expectedBaseType = NoType;
# line 1312 "Balsa.ag"

	yyt->InputCommand.LHS->Lvalue.lockPassiveChannels = false;
# line 1311 "Balsa.ag"

	yyt->InputCommand.LHS->Lvalue.isPassive = false;
# line 1310 "Balsa.ag"

	yyt->InputCommand.LHS->Lvalue.isInput = true;
# line 1308 "Balsa.ag"
 yyt->InputCommand.LHS->Lvalue.expectingEither = false;
# line 1308 "Balsa.ag"
 
	yyt->InputCommand.LHS->Lvalue.expectingChannel = true;
yyt->InputCommand.LHS->Lvalue.skip=yyt->InputCommand.skip;
yyVisit1 (yyt->InputCommand.LHS);
# line 1307 "Balsa.ag"
 
	yyt->InputCommand.RHS->Lvalue.expectedBaseType = NoType;
# line 1312 "Balsa.ag"
 yyt->InputCommand.RHS->Lvalue.lockPassiveChannels = false;
# line 1311 "Balsa.ag"
 yyt->InputCommand.RHS->Lvalue.isPassive = false;
# line 1310 "Balsa.ag"
 yyt->InputCommand.RHS->Lvalue.isInput = false;
# line 1309 "Balsa.ag"
 yyt->InputCommand.RHS->Lvalue.expectingEither = true;
# line 1309 "Balsa.ag"

	yyt->InputCommand.RHS->Lvalue.expectingChannel = false;
yyt->InputCommand.RHS->Lvalue.skip=yyt->InputCommand.skip;
yyt->InputCommand.RHS->Lvalue.context=yyt->InputCommand.context;
yyVisit1 (yyt->InputCommand.RHS);
# line 1313 "Balsa.ag"

	yyt->InputCommand.attributes = (yyt->InputCommand.skip || ! yyt->InputCommand.LHS->Lvalue.instance || ! yyt->InputCommand.RHS->Lvalue.instance ? NoCommandAttributes :
		(yyt->InputCommand.RHS->Lvalue.instance->nature == VariableInstance
		? HandleInputCommand (yyt->InputCommand.LHS->Lvalue.attributes, yyt->InputCommand.LHS->Lvalue.instance,
			FinaliseLvalue (yyt->InputCommand.RHS->Lvalue.attributes, yyt->InputCommand.RHS->Lvalue.instance, yyt->InputCommand.RHS->Lvalue.partitions, yyt->InputCommand.RHS->Lvalue.indices,
			yyt->InputCommand.RHS->Lvalue.indexWire, yyt->InputCommand.RHS->Lvalue.access, yyt->InputCommand.position), yyt->InputCommand.RHS->Lvalue.instance, yyt->InputCommand.position)
		: HandleInputToChannelCommand (yyt->InputCommand.LHS->Lvalue.attributes, yyt->InputCommand.LHS->Lvalue.instance, yyt->InputCommand.RHS->Lvalue.attributes, yyt->InputCommand.RHS->Lvalue.instance, yyt->InputCommand.position)
		));
break;

case kInputEncloseCommand:
yyt->InputEncloseCommand.LvalueLists->LvalueLists.context=yyt->InputEncloseCommand.context;
# line 1322 "Balsa.ag"
 
	yyt->InputEncloseCommand.LvalueLists->LvalueLists.allowArrayedChannels = false;
# line 1328 "Balsa.ag"
 
	yyt->InputEncloseCommand.LvalueLists->LvalueLists.isProcedurePorts = false;
# line 1325 "Balsa.ag"
 
	yyt->InputEncloseCommand.LvalueLists->LvalueLists.lockPassiveChannels = false;
# line 1324 "Balsa.ag"
 
	yyt->InputEncloseCommand.LvalueLists->LvalueLists.isPassive = false;
# line 1323 "Balsa.ag"

	yyt->InputEncloseCommand.LvalueLists->LvalueLists.isInput = true;
# line 1326 "Balsa.ag"
 
	yyt->InputEncloseCommand.LvalueLists->LvalueLists.elements = NULL;
# line 1327 "Balsa.ag"
 
	yyt->InputEncloseCommand.LvalueLists->LvalueLists.expectedType = NoType;
yyt->InputEncloseCommand.LvalueLists->LvalueLists.skip=yyt->InputEncloseCommand.skip;
yyVisit1 (yyt->InputEncloseCommand.LvalueLists);
yyt->InputEncloseCommand.Command->Command.skip=yyt->InputEncloseCommand.skip;
# line 1330 "Balsa.ag"


	yyt->InputEncloseCommand.Command->Command.context = (yyt->InputEncloseCommand.skip ? yyt->InputEncloseCommand.context :
		NewContext (yyt->InputEncloseCommand.context->depth + 1, yyt->InputEncloseCommand.context->types, 
			AppendInstanceLists (MakeReadOnlyVariablesFromInstances (yyt->InputEncloseCommand.LvalueLists->LvalueLists.attributes,
				yyt->InputEncloseCommand.LvalueLists->LvalueLists.idents, ChannelActiveRead), yyt->InputEncloseCommand.context->instances), yyt->InputEncloseCommand.context->procedures));
yyVisit1 (yyt->InputEncloseCommand.Command);
# line 1335 "Balsa.ag"


	yyt->InputEncloseCommand.attributes = (yyt->InputEncloseCommand.skip ? NoCommandAttributes :
		HandleInputEncloseCommand (FlattenDisjointChannelAccessList (yyt->InputEncloseCommand.LvalueLists->LvalueLists.attributes, yyt->InputEncloseCommand.position), 
			yyt->InputEncloseCommand.Command->Command.attributes, false, yyt->InputEncloseCommand.position));
break;

case kInputEncloseBangCommand:
yyt->InputEncloseBangCommand.LvalueLists->LvalueLists.context=yyt->InputEncloseBangCommand.context;
# line 1340 "Balsa.ag"
 
	yyt->InputEncloseBangCommand.LvalueLists->LvalueLists.allowArrayedChannels = false;
# line 1346 "Balsa.ag"
 
	yyt->InputEncloseBangCommand.LvalueLists->LvalueLists.isProcedurePorts = false;
# line 1343 "Balsa.ag"
 
	yyt->InputEncloseBangCommand.LvalueLists->LvalueLists.lockPassiveChannels = false;
# line 1342 "Balsa.ag"
 
	yyt->InputEncloseBangCommand.LvalueLists->LvalueLists.isPassive = false;
# line 1341 "Balsa.ag"

	yyt->InputEncloseBangCommand.LvalueLists->LvalueLists.isInput = true;
# line 1344 "Balsa.ag"
 
	yyt->InputEncloseBangCommand.LvalueLists->LvalueLists.elements = NULL;
# line 1345 "Balsa.ag"
 
	yyt->InputEncloseBangCommand.LvalueLists->LvalueLists.expectedType = NoType;
yyt->InputEncloseBangCommand.LvalueLists->LvalueLists.skip=yyt->InputEncloseBangCommand.skip;
yyVisit1 (yyt->InputEncloseBangCommand.LvalueLists);
yyt->InputEncloseBangCommand.Command->Command.skip=yyt->InputEncloseBangCommand.skip;
# line 1348 "Balsa.ag"


	yyt->InputEncloseBangCommand.Command->Command.context = (yyt->InputEncloseBangCommand.skip ? yyt->InputEncloseBangCommand.context :
		NewContext (yyt->InputEncloseBangCommand.context->depth + 1, yyt->InputEncloseBangCommand.context->types, 
			AppendInstanceLists (MakeReadOnlyVariablesFromInstances (yyt->InputEncloseBangCommand.LvalueLists->LvalueLists.attributes,
				yyt->InputEncloseBangCommand.LvalueLists->LvalueLists.idents, ChannelActiveRead), yyt->InputEncloseBangCommand.context->instances), yyt->InputEncloseBangCommand.context->procedures));
yyVisit1 (yyt->InputEncloseBangCommand.Command);
# line 1353 "Balsa.ag"


	yyt->InputEncloseBangCommand.attributes = (yyt->InputEncloseBangCommand.skip ? NoCommandAttributes :
		HandleInputEncloseCommand (FlattenDisjointChannelAccessList (yyt->InputEncloseBangCommand.LvalueLists->LvalueLists.attributes, yyt->InputEncloseBangCommand.position), 
			yyt->InputEncloseBangCommand.Command->Command.attributes, ! DontGenerateEagerFVs, yyt->InputEncloseBangCommand.position));
break;

case kOutputCommand:
yyt->OutputCommand.Lvalue->Lvalue.context=yyt->OutputCommand.context;
# line 1358 "Balsa.ag"
 
	yyt->OutputCommand.Lvalue->Lvalue.expectedBaseType = NoType;
# line 1363 "Balsa.ag"

	yyt->OutputCommand.Lvalue->Lvalue.lockPassiveChannels = false;
# line 1362 "Balsa.ag"
 yyt->OutputCommand.Lvalue->Lvalue.isPassive = false;
# line 1362 "Balsa.ag"

	yyt->OutputCommand.Lvalue->Lvalue.isInput = false;
# line 1359 "Balsa.ag"
 yyt->OutputCommand.Lvalue->Lvalue.expectingEither = false;
# line 1359 "Balsa.ag"
 
	yyt->OutputCommand.Lvalue->Lvalue.expectingChannel = true;
yyt->OutputCommand.Lvalue->Lvalue.skip=yyt->OutputCommand.skip;
yyVisit1 (yyt->OutputCommand.Lvalue);
yyt->OutputCommand.CoercedExpr->CoercedExpr.skip=yyt->OutputCommand.skip;
# line 1361 "Balsa.ag"
 
	yyt->OutputCommand.CoercedExpr->CoercedExpr.allowChannels = false;
yyt->OutputCommand.CoercedExpr->CoercedExpr.context=yyt->OutputCommand.context;
# line 1360 "Balsa.ag"

	yyt->OutputCommand.CoercedExpr->CoercedExpr.expectedType = (yyt->OutputCommand.Lvalue->Lvalue.attributes).value.type;
yyVisit1 (yyt->OutputCommand.CoercedExpr);
# line 1364 "Balsa.ag"

	yyt->OutputCommand.attributes = (yyt->OutputCommand.skip ? NoCommandAttributes : 
		HandleOutputCommand (yyt->OutputCommand.Lvalue->Lvalue.attributes, yyt->OutputCommand.Lvalue->Lvalue.instance, yyt->OutputCommand.CoercedExpr->CoercedExpr.attributes, yyt->OutputCommand.position));
break;

case kSyncCommand:
yyt->SyncCommand.Lvalue->Lvalue.context=yyt->SyncCommand.context;
# line 1160 "Balsa.ag"

	yyt->SyncCommand.Lvalue->Lvalue.expectedBaseType = NoType;
# line 1162 "Balsa.ag"
 yyt->SyncCommand.Lvalue->Lvalue.lockPassiveChannels = false;
# line 1162 "Balsa.ag"
 yyt->SyncCommand.Lvalue->Lvalue.isPassive = false;
# line 1162 "Balsa.ag"

	yyt->SyncCommand.Lvalue->Lvalue.isInput = true;
# line 1161 "Balsa.ag"
 yyt->SyncCommand.Lvalue->Lvalue.expectingEither = false;
# line 1161 "Balsa.ag"
 
	yyt->SyncCommand.Lvalue->Lvalue.expectingChannel = true;
yyt->SyncCommand.Lvalue->Lvalue.skip=yyt->SyncCommand.skip;
yyVisit1 (yyt->SyncCommand.Lvalue);
# line 1163 "Balsa.ag"

	yyt->SyncCommand.attributes = (yyt->SyncCommand.skip ? NoCommandAttributes :
		HandleSyncCommand (yyt->SyncCommand.context, yyt->SyncCommand.Lvalue->Lvalue.attributes, yyt->SyncCommand.Lvalue->Lvalue.instance, yyt->SyncCommand.position));
break;

case kAssignmentCommand:
yyt->AssignmentCommand.Lvalue->Lvalue.context=yyt->AssignmentCommand.context;
# line 1292 "Balsa.ag"
 
	yyt->AssignmentCommand.Lvalue->Lvalue.expectedBaseType = NoType;
# line 1296 "Balsa.ag"
 yyt->AssignmentCommand.Lvalue->Lvalue.lockPassiveChannels = false;
# line 1296 "Balsa.ag"
 yyt->AssignmentCommand.Lvalue->Lvalue.isPassive = false;
# line 1296 "Balsa.ag"

	yyt->AssignmentCommand.Lvalue->Lvalue.isInput = false;
# line 1295 "Balsa.ag"
 yyt->AssignmentCommand.Lvalue->Lvalue.expectingEither = false;
# line 1295 "Balsa.ag"

	yyt->AssignmentCommand.Lvalue->Lvalue.expectingChannel = false;
yyt->AssignmentCommand.Lvalue->Lvalue.skip=yyt->AssignmentCommand.skip;
yyVisit1 (yyt->AssignmentCommand.Lvalue);
yyt->AssignmentCommand.CoercedExpr->CoercedExpr.skip=yyt->AssignmentCommand.skip;
# line 1294 "Balsa.ag"

	yyt->AssignmentCommand.CoercedExpr->CoercedExpr.allowChannels = false;
yyt->AssignmentCommand.CoercedExpr->CoercedExpr.context=yyt->AssignmentCommand.context;
# line 1293 "Balsa.ag"
 
	yyt->AssignmentCommand.CoercedExpr->CoercedExpr.expectedType = (yyt->AssignmentCommand.Lvalue->Lvalue.attributes).value.type;
yyVisit1 (yyt->AssignmentCommand.CoercedExpr);
# line 1297 "Balsa.ag"

	yyt->AssignmentCommand.attributes = (yyt->AssignmentCommand.skip ? NoCommandAttributes :
		HandleAssignmentCommand (
		FinaliseLvalue (yyt->AssignmentCommand.Lvalue->Lvalue.attributes, yyt->AssignmentCommand.Lvalue->Lvalue.instance, yyt->AssignmentCommand.Lvalue->Lvalue.partitions,
		yyt->AssignmentCommand.Lvalue->Lvalue.indices, yyt->AssignmentCommand.Lvalue->Lvalue.indexWire, yyt->AssignmentCommand.Lvalue->Lvalue.access, yyt->AssignmentCommand.position), yyt->AssignmentCommand.Lvalue->Lvalue.instance,
		yyt->AssignmentCommand.CoercedExpr->CoercedExpr.attributes, yyt->AssignmentCommand.position)
	);
break;

case kBlockCommand:
# line 1155 "Balsa.ag"
 yyt->BlockCommand.Block->Block.context = AddContextMarker (yyt->BlockCommand.context);
yyt->BlockCommand.Block->Block.skip=yyt->BlockCommand.skip;
yyVisit1 (yyt->BlockCommand.Block);
# line 1155 "Balsa.ag"
 yyt->BlockCommand.attributes = yyt->BlockCommand.Block->Block.attributes;
break;

case kSequentialCommand:
yyt->SequentialCommand.Left->Command.skip=yyt->SequentialCommand.skip;
yyt->SequentialCommand.Left->Command.context=yyt->SequentialCommand.context;
yyVisit1 (yyt->SequentialCommand.Left);
yyt->SequentialCommand.Right->Command.skip=yyt->SequentialCommand.skip;
yyt->SequentialCommand.Right->Command.context=yyt->SequentialCommand.context;
yyVisit1 (yyt->SequentialCommand.Right);
# line 1148 "Balsa.ag"

	yyt->SequentialCommand.attributes = (yyt->SequentialCommand.skip ? NoCommandAttributes :
		HandleSequentialCommand (yyt->SequentialCommand.Left->Command.attributes, yyt->SequentialCommand.Right->Command.attributes, yyt->SequentialCommand.position));
break;

case kParallelCommand:
yyt->ParallelCommand.Left->Command.skip=yyt->ParallelCommand.skip;
yyt->ParallelCommand.Left->Command.context=yyt->ParallelCommand.context;
yyVisit1 (yyt->ParallelCommand.Left);
yyt->ParallelCommand.Right->Command.skip=yyt->ParallelCommand.skip;
yyt->ParallelCommand.Right->Command.context=yyt->ParallelCommand.context;
yyVisit1 (yyt->ParallelCommand.Right);
# line 1152 "Balsa.ag"

	yyt->ParallelCommand.attributes = (yyt->ParallelCommand.skip ? NoCommandAttributes :
		HandleParallelCommand (yyt->ParallelCommand.Left->Command.attributes, yyt->ParallelCommand.Right->Command.attributes, yyt->ParallelCommand.isPermissive, yyt->ParallelCommand.position));
break;

case kLoopCommand:
yyt->LoopCommand.Command->Command.skip=yyt->LoopCommand.skip;
yyt->LoopCommand.Command->Command.context=yyt->LoopCommand.context;
yyVisit1 (yyt->LoopCommand.Command);
# line 1157 "Balsa.ag"

	yyt->LoopCommand.attributes = (yyt->LoopCommand.skip ? NoCommandAttributes : HandleLoopCommand (yyt->LoopCommand.Command->Command.attributes, yyt->LoopCommand.position));
break;

case kWhileGuardsCommand:
yyt->WhileGuardsCommand.Guards->Guards.context=yyt->WhileGuardsCommand.context;
yyt->WhileGuardsCommand.Guards->Guards.skip=yyt->WhileGuardsCommand.skip;
yyVisit1 (yyt->WhileGuardsCommand.Guards);
# line 1167 "Balsa.ag"
 
	yyt->WhileGuardsCommand.attributes = (yyt->WhileGuardsCommand.skip ? NoCommandAttributes :
		HandleWhileCommand (WhileGuardsLoop, yyt->WhileGuardsCommand.context, yyt->WhileGuardsCommand.Guards->Guards.commandAttributes, yyt->WhileGuardsCommand.Guards->Guards.exprAttributes,
			NoCommandAttributes, NoCommandAttributes, yyt->WhileGuardsCommand.position));
break;

case kWhileGuardsAlsoCommand:
yyt->WhileGuardsAlsoCommand.Guards->Guards.context=yyt->WhileGuardsAlsoCommand.context;
yyt->WhileGuardsAlsoCommand.Guards->Guards.skip=yyt->WhileGuardsAlsoCommand.skip;
yyVisit1 (yyt->WhileGuardsAlsoCommand.Guards);
yyt->WhileGuardsAlsoCommand.Command->Command.skip=yyt->WhileGuardsAlsoCommand.skip;
yyt->WhileGuardsAlsoCommand.Command->Command.context=yyt->WhileGuardsAlsoCommand.context;
yyVisit1 (yyt->WhileGuardsAlsoCommand.Command);
# line 1189 "Balsa.ag"
 
	yyt->WhileGuardsAlsoCommand.attributes = (yyt->WhileGuardsAlsoCommand.skip ? NoCommandAttributes :
		HandleWhileCommand (WhileGuardsAlsoLoop, yyt->WhileGuardsAlsoCommand.context, yyt->WhileGuardsAlsoCommand.Guards->Guards.commandAttributes, yyt->WhileGuardsAlsoCommand.Guards->Guards.exprAttributes,
			yyt->WhileGuardsAlsoCommand.Command->Command.attributes, NoCommandAttributes, yyt->WhileGuardsAlsoCommand.position));
break;

case kCommandWhileExprCommand:
yyt->CommandWhileExprCommand.Command->Command.skip=yyt->CommandWhileExprCommand.skip;
yyt->CommandWhileExprCommand.Command->Command.context=yyt->CommandWhileExprCommand.context;
yyVisit1 (yyt->CommandWhileExprCommand.Command);
# line 1172 "Balsa.ag"
 
	yyt->CommandWhileExprCommand.Expr->Expr.allowChannels = false;
# line 1173 "Balsa.ag"

	yyt->CommandWhileExprCommand.Expr->Expr.expectedType = BitTypeObj;
yyt->CommandWhileExprCommand.Expr->Expr.skip=yyt->CommandWhileExprCommand.skip;
yyt->CommandWhileExprCommand.Expr->Expr.context=yyt->CommandWhileExprCommand.context;
yyVisit1 (yyt->CommandWhileExprCommand.Expr);
# line 1174 "Balsa.ag"

	yyt->CommandWhileExprCommand.attributes = (yyt->CommandWhileExprCommand.skip ? NoCommandAttributes :
		HandleWhileCommand (CommandWhileExprLoop, yyt->CommandWhileExprCommand.context, NULL, NewExprAttributesList (yyt->CommandWhileExprCommand.Expr->Expr.attributes, NULL),
			yyt->CommandWhileExprCommand.Command->Command.attributes, NoCommandAttributes, yyt->CommandWhileExprCommand.position));
break;

case kCommandWhileGuardsCommand:
yyt->CommandWhileGuardsCommand.Command->Command.skip=yyt->CommandWhileGuardsCommand.skip;
yyt->CommandWhileGuardsCommand.Command->Command.context=yyt->CommandWhileGuardsCommand.context;
yyVisit1 (yyt->CommandWhileGuardsCommand.Command);
yyt->CommandWhileGuardsCommand.Guards->Guards.skip=yyt->CommandWhileGuardsCommand.skip;
yyt->CommandWhileGuardsCommand.Guards->Guards.context=yyt->CommandWhileGuardsCommand.context;
yyVisit1 (yyt->CommandWhileGuardsCommand.Guards);
# line 1179 "Balsa.ag"
 
	yyt->CommandWhileGuardsCommand.attributes = (yyt->CommandWhileGuardsCommand.skip ? NoCommandAttributes :
		HandleWhileCommand (CommandWhileGuardsLoop, yyt->CommandWhileGuardsCommand.context, yyt->CommandWhileGuardsCommand.Guards->Guards.commandAttributes, yyt->CommandWhileGuardsCommand.Guards->Guards.exprAttributes,
			yyt->CommandWhileGuardsCommand.Command->Command.attributes, NoCommandAttributes, yyt->CommandWhileGuardsCommand.position));
break;

case kCommandWhileGuardsAlsoCommand:
yyt->CommandWhileGuardsAlsoCommand.FirstCommand->Command.skip=yyt->CommandWhileGuardsAlsoCommand.skip;
yyt->CommandWhileGuardsAlsoCommand.FirstCommand->Command.context=yyt->CommandWhileGuardsAlsoCommand.context;
yyVisit1 (yyt->CommandWhileGuardsAlsoCommand.FirstCommand);
yyt->CommandWhileGuardsAlsoCommand.AlsoCommand->Command.skip=yyt->CommandWhileGuardsAlsoCommand.skip;
yyt->CommandWhileGuardsAlsoCommand.AlsoCommand->Command.context=yyt->CommandWhileGuardsAlsoCommand.context;
yyVisit1 (yyt->CommandWhileGuardsAlsoCommand.AlsoCommand);
yyt->CommandWhileGuardsAlsoCommand.Guards->Guards.skip=yyt->CommandWhileGuardsAlsoCommand.skip;
yyt->CommandWhileGuardsAlsoCommand.Guards->Guards.context=yyt->CommandWhileGuardsAlsoCommand.context;
yyVisit1 (yyt->CommandWhileGuardsAlsoCommand.Guards);
# line 1184 "Balsa.ag"
 
	yyt->CommandWhileGuardsAlsoCommand.attributes = (yyt->CommandWhileGuardsAlsoCommand.skip ? NoCommandAttributes :
		HandleWhileCommand (CommandWhileGuardsAlsoLoop, yyt->CommandWhileGuardsAlsoCommand.context, yyt->CommandWhileGuardsAlsoCommand.Guards->Guards.commandAttributes, yyt->CommandWhileGuardsAlsoCommand.Guards->Guards.exprAttributes,
			yyt->CommandWhileGuardsAlsoCommand.FirstCommand->Command.attributes, yyt->CommandWhileGuardsAlsoCommand.AlsoCommand->Command.attributes, yyt->CommandWhileGuardsAlsoCommand.position));
break;

case kIfCommand:
yyt->IfCommand.Guards->Guards.context=yyt->IfCommand.context;
yyt->IfCommand.Guards->Guards.skip=yyt->IfCommand.skip;
yyVisit1 (yyt->IfCommand.Guards);
# line 1194 "Balsa.ag"
 
	yyt->IfCommand.attributes = (yyt->IfCommand.skip ? NoCommandAttributes :
		HandleIfCommand (false, yyt->IfCommand.context, yyt->IfCommand.Guards->Guards.commandAttributes, yyt->IfCommand.Guards->Guards.exprAttributes,
			NoCommandAttributes, yyt->IfCommand.position));
break;

case kIfElseCommand:
yyt->IfElseCommand.Guards->Guards.context=yyt->IfElseCommand.context;
yyt->IfElseCommand.Guards->Guards.skip=yyt->IfElseCommand.skip;
yyVisit1 (yyt->IfElseCommand.Guards);
# line 1202 "Balsa.ag"

		yyt->IfElseCommand.Command->Command.skip = yyt->IfElseCommand.skip;

		if (! yyt->IfElseCommand.skip)
		{
			if (yyt->IfElseCommand.Guards->Guards.exprAttributes && DetermineGuardNature (CAR (yyt->IfElseCommand.Guards->Guards.exprAttributes),
				false, false , yyt->IfElseCommand.position) == TrueGuardNature)
				yyt->IfElseCommand.Command->Command.skip = true;
		}
	
yyt->IfElseCommand.Command->Command.context=yyt->IfElseCommand.context;
yyVisit1 (yyt->IfElseCommand.Command);
# line 1199 "Balsa.ag"
 
	yyt->IfElseCommand.attributes = (yyt->IfElseCommand.skip ? NoCommandAttributes :
		HandleIfCommand (! yyt->IfElseCommand.Command->Command.skip, yyt->IfElseCommand.context, yyt->IfElseCommand.Guards->Guards.commandAttributes, yyt->IfElseCommand.Guards->Guards.exprAttributes,
			yyt->IfElseCommand.Command->Command.attributes, yyt->IfElseCommand.position));
break;

case kCaseCommand:
yyt->CaseCommand.Expr->Expr.context=yyt->CaseCommand.context;
# line 1370 "Balsa.ag"

	yyt->CaseCommand.Expr->Expr.allowChannels = false;
# line 1371 "Balsa.ag"

	yyt->CaseCommand.Expr->Expr.expectedType = NoType;
yyt->CaseCommand.Expr->Expr.skip=yyt->CaseCommand.skip;
yyVisit1 (yyt->CaseCommand.Expr);
yyt->CaseCommand.CaseGuards->CaseGuards.context=yyt->CaseCommand.context;
# line 1373 "Balsa.ag"
 
	yyt->CaseCommand.CaseGuards->CaseGuards.maxRange = (yyt->CaseCommand.Expr->Expr.attributes).value.type->size;
# line 1375 "Balsa.ag"

	
	yyt->CaseCommand.CaseGuards->CaseGuards.switchValue = (yyt->CaseCommand.Expr->Expr.attributes).value.value.baseValue;
# line 1372 "Balsa.ag"

	yyt->CaseCommand.CaseGuards->CaseGuards.switchType = (yyt->CaseCommand.Expr->Expr.attributes).value.type;
yyt->CaseCommand.CaseGuards->CaseGuards.skip=yyt->CaseCommand.skip;
yyVisit1 (yyt->CaseCommand.CaseGuards);
# line 1376 "Balsa.ag"

	yyt->CaseCommand.Command->Command.skip = yyt->CaseCommand.skip || yyt->CaseCommand.CaseGuards->CaseGuards.foundTrueGuard;
yyt->CaseCommand.Command->Command.context=yyt->CaseCommand.context;
yyVisit1 (yyt->CaseCommand.Command);
# line 1377 "Balsa.ag"

		yyt->CaseCommand.attributes = NoCommandAttributes;

		if (! yyt->CaseCommand.skip)
		{
			if (yyt->CaseCommand.CaseGuards->CaseGuards.switchValue && ! yyt->CaseCommand.CaseGuards->CaseGuards.foundTrueGuard) 
			{
				yyt->CaseCommand.attributes = ((yyt->CaseCommand.Command->Kind == kNullCommand)
					? HandleContinueCommand (yyt->CaseCommand.position)
					: yyt->CaseCommand.Command->Command.attributes
				);
			} else {
				yyt->CaseCommand.attributes = HandleCaseCommand (yyt->CaseCommand.CaseGuards->CaseGuards.commandAttributes, yyt->CaseCommand.CaseGuards->CaseGuards.implicantss,
					yyt->CaseCommand.CaseGuards->CaseGuards.complementImplicants, yyt->CaseCommand.Expr->Expr.attributes,
					yyt->CaseCommand.Command->Kind != kNullCommand ,
					(yyt->CaseCommand.Command->Kind == kNullCommand ? NoCommandAttributes : yyt->CaseCommand.Command->Command.attributes),
					yyt->CaseCommand.position
				);
			}
		}	
	
break;

case kForCommand:
yyt->ForCommand.Range->Range.context=yyt->ForCommand.context;
yyt->ForCommand.Range->Range.skip=yyt->ForCommand.skip;
# line 1215 "Balsa.ag"

	yyt->ForCommand.Range->Range.isSpan = true;
# line 1216 "Balsa.ag"
 
	yyt->ForCommand.Range->Range.expectedType = NoType;
yyVisit1 (yyt->ForCommand.Range);
yyt->ForCommand.Command->Command.skip=yyt->ForCommand.skip;
# line 1259 "Balsa.ag"

		yyt->ForCommand.iterator = NULL;
		yyt->ForCommand.Command->Command.context = AddContextMarker (yyt->ForCommand.context);
		yyt->ForCommand.errorContextString = "";
		yyt->ForCommand.valueStringPtr = "";
		yyt->ForCommand.lastErrorCount = ErrorCount;

		if (!yyt->ForCommand.skip)
		{
			if ((yyt->ForCommand.Range->Range.range).boundingType == NoType)
				LOG_ERROR (InvalidForRange, yyt->ForCommand.ident, yyt->ForCommand.position);
			else {
				yyt->ForCommand.iterator = NewConstantInstance (yyt->ForCommand.ident, 
					NewTypedValue (CopyMP_INT ((yyt->ForCommand.Range->Range.range).lowerBound), (yyt->ForCommand.Range->Range.range).boundingType));
				yyt->ForCommand.errorContextString = NEW_ARRAY (char, 8192);
				yyt->ForCommand.valueStringPtr = yyt->ForCommand.errorContextString;
	
				sprintf (yyt->ForCommand.valueStringPtr, "in `for %s' loop at iteration %s = ", (yyt->ForCommand.isParallel ? "||" : ";"),
					PeekString (yyt->ForCommand.ident));
				yyt->ForCommand.valueStringPtr = EndOfString (yyt->ForCommand.valueStringPtr);
				
				*yyt->ForCommand.valueStringPtr = '\0';
				MakeTypedValueString (yyt->ForCommand.valueStringPtr, NewTypedValue (
					(yyt->ForCommand.Range->Range.range).lowerBound, (yyt->ForCommand.Range->Range.range).boundingType));
				PushErrorContext (yyt->ForCommand.errorContextString, yyt->ForCommand.position);
				(yyt->ForCommand.Command->Command.context)->instances = NewInstanceList (yyt->ForCommand.iterator, yyt->ForCommand.context->instances);
			}
		}
	
yyVisit1 (yyt->ForCommand.Command);
# line 1218 "Balsa.ag"
 
		yyt->ForCommand.attributes = yyt->ForCommand.Command->Command.attributes;

		if (!yyt->ForCommand.skip && yyt->ForCommand.iterator)
		{ 
			PtrMP_INT i = yyt->ForCommand.iterator->info.constant.value.baseValue; 
			PtrMP_INT originalIteratorValue = CopyMP_INT (i);

			if (yyt->ForCommand.lastErrorCount != ErrorCount)
				LOG_ERROR (GivingUpOnFor, NoIdent, yyt->ForCommand.position);

			PopErrorContext (); 
			mpz_add_ui (i, i, 1); 
			for (; mpz_cmp (i, (yyt->ForCommand.Range->Range.range).upperBound) <= 0 && yyt->ForCommand.lastErrorCount == ErrorCount; mpz_add_ui (i, i, 1))
			{
				yyt->ForCommand.lastErrorCount = ErrorCount;
				
				*yyt->ForCommand.valueStringPtr = '\0';
				MakeTypedValueString (yyt->ForCommand.valueStringPtr, NewTypedValue (
					i, (yyt->ForCommand.Range->Range.range).boundingType));
				PushErrorContext (yyt->ForCommand.errorContextString, yyt->ForCommand.position);
				yyVisit1  (yyt->ForCommand.Command);
				PopErrorContext ();
				if (yyt->ForCommand.isParallel)
				{
					yyt->ForCommand.attributes = HandleParallelCommand (yyt->ForCommand.attributes, yyt->ForCommand.Command->Command.attributes, yyt->ForCommand.isPermissive, yyt->ForCommand.position);
				} else {
					yyt->ForCommand.attributes = HandleSequentialCommand (yyt->ForCommand.attributes, yyt->ForCommand.Command->Command.attributes, yyt->ForCommand.position);
				}

				if (yyt->ForCommand.lastErrorCount != ErrorCount)
				{
					LOG_ERROR (GivingUpOnFor, NoIdent, yyt->ForCommand.position);
					break;
				}
			}
			FREE_ARRAY (char, 8192, yyt->ForCommand.errorContextString);
			mpz_set (i, originalIteratorValue);
			DeleteMP_INT (originalIteratorValue);
		}
	
break;

case kProcedureCallCommonCommand:
# line 1891 "Balsa.ag"

	yyt->ProcedureCallCommonCommand.ProcedureParams->ProcedureParams.context = yyt->ProcedureCallCommonCommand.context;
# line 1892 "Balsa.ag"

	yyt->ProcedureCallCommonCommand.ProcedureParams->ProcedureParams.parameterContext = yyt->ProcedureCallCommonCommand.context;
# line 1893 "Balsa.ag"

	yyt->ProcedureCallCommonCommand.ProcedureParams->ProcedureParams.noPorts = false;
# line 1896 "Balsa.ag"

		yyt->ProcedureCallCommonCommand.ProcedureParams->ProcedureParams.procedureTree = NULL;
		yyt->ProcedureCallCommonCommand.ProcedureParams->ProcedureParams.formalParams = NULL;
		yyt->ProcedureCallCommonCommand.ProcedureParams->ProcedureParams.formalPorts = NULL;
		yyt->ProcedureCallCommonCommand.ProcedureParams->ProcedureParams.idents = NULL;
		yyt->ProcedureCallCommonCommand.ProcedureParams->ProcedureParams.procedureIn = (yyt->ProcedureCallCommonCommand.skip ? NoProcedure : LookupProcedure
			(yyt->ProcedureCallCommonCommand.context, yyt->ProcedureCallCommonCommand.ident, false  ));
		yyt->ProcedureCallCommonCommand.ProcedureParams->ProcedureParams.skip = yyt->ProcedureCallCommonCommand.skip;

		if (! yyt->ProcedureCallCommonCommand.skip)
		{
			if (! yyt->ProcedureCallCommonCommand.ProcedureParams->ProcedureParams.procedureIn ||
				yyt->ProcedureCallCommonCommand.ProcedureParams->ProcedureParams.procedureIn == NoProcedure)
			{
				LOG_ERROR (ExpectingAProcedureName, yyt->ProcedureCallCommonCommand.ident, yyt->ProcedureCallCommonCommand.position);
				yyt->ProcedureCallCommonCommand.ProcedureParams->ProcedureParams.skip = true;
			} else {
				switch (yyt->ProcedureCallCommonCommand.ProcedureParams->ProcedureParams.procedureIn->nature)
				{
					case SharedProcedure:
					case UnSharedProcedure:
						yyt->ProcedureCallCommonCommand.ProcedureParams->ProcedureParams.formalPorts = yyt->ProcedureCallCommonCommand.ProcedureParams->ProcedureParams.procedureIn->ports;
						break;
					case ParameterisedProcedure:
						{
							tTree tree = yyt->ProcedureCallCommonCommand.ProcedureParams->ProcedureParams.procedureIn->info.parameterisedProcedure.tree;
							tTree oldNext = tree->Decl.next;
						
							tree->Decl.next = mNullDecls (yyt->ProcedureCallCommonCommand.position);
							yyt->ProcedureCallCommonCommand.ProcedureParams->ProcedureParams.procedureTree = CopyTree (tree);
							tree->Decl.next = oldNext;					

							yyt->ProcedureCallCommonCommand.ProcedureParams->ProcedureParams.formalParams =
								yyt->ProcedureCallCommonCommand.ProcedureParams->ProcedureParams.procedureTree->ProcedureDecl.FormalPorts;
							yyt->ProcedureCallCommonCommand.ProcedureParams->ProcedureParams.idents = (yyt->ProcedureCallCommonCommand.ProcedureParams->ProcedureParams.formalParams->Kind == kParamPort
								?  yyt->ProcedureCallCommonCommand.ProcedureParams->ProcedureParams.formalParams->ParamPort.Idents->Idents.idents
								: yyt->ProcedureCallCommonCommand.ProcedureParams->ProcedureParams.formalParams->TypeParamPort.Idents->Idents.idents);
						}
						break;
					default:
						LOG_ERROR (ExpectingAProcedureName, yyt->ProcedureCallCommonCommand.ProcedureParams->ProcedureParams.procedureIn->ident, yyt->ProcedureCallCommonCommand.position);
						yyt->ProcedureCallCommonCommand.ProcedureParams->ProcedureParams.skip = true;
						break;
				}
			}
		}
	
# line 1890 "Balsa.ag"

	yyt->ProcedureCallCommonCommand.ProcedureParams->ProcedureParams.typeParams = NULL;
# line 1889 "Balsa.ag"
 
	yyt->ProcedureCallCommonCommand.ProcedureParams->ProcedureParams.constantParams = NULL;
yyVisit1 (yyt->ProcedureCallCommonCommand.ProcedureParams);
# line 1943 "Balsa.ag"

		yyt->ProcedureCallCommonCommand.attributes = (yyt->ProcedureCallCommonCommand.skip || yyt->ProcedureCallCommonCommand.ProcedureParams->ProcedureParams.procedureOut == NoProcedure ? NoCommandAttributes :
			HandleProcedureCallCommand (yyt->ProcedureCallCommonCommand.ProcedureParams->ProcedureParams.actualPorts, yyt->ProcedureCallCommonCommand.ProcedureParams->ProcedureParams.procedureOut, yyt->ProcedureCallCommonCommand.position)); 
	
break;

case kSelectCommand:
yyt->SelectCommand.ChannelGuards->ChannelGuards.context=yyt->SelectCommand.context;
yyt->SelectCommand.ChannelGuards->ChannelGuards.skip=yyt->SelectCommand.skip;
yyVisit1 (yyt->SelectCommand.ChannelGuards);
# line 1950 "Balsa.ag"

	yyt->SelectCommand.attributes = (yyt->SelectCommand.skip ? NoCommandAttributes :
		HandleSelectAndArbitrateCommands (yyt->SelectCommand.ChannelGuards->ChannelGuards.commandAttributes, 
			yyt->SelectCommand.ChannelGuards->ChannelGuards.guardAttributes, false , yyt->SelectCommand.position));
break;

case kSelectBangCommand:
yyt->SelectBangCommand.ChannelGuards->ChannelGuards.context=yyt->SelectBangCommand.context;
yyt->SelectBangCommand.ChannelGuards->ChannelGuards.skip=yyt->SelectBangCommand.skip;
yyVisit1 (yyt->SelectBangCommand.ChannelGuards);
# line 1955 "Balsa.ag"

	yyt->SelectBangCommand.attributes = (yyt->SelectBangCommand.skip ? NoCommandAttributes :
		( DontGenerateEagerFVs ?
			HandleSelectAndArbitrateCommands (yyt->SelectBangCommand.ChannelGuards->ChannelGuards.commandAttributes, 
				yyt->SelectBangCommand.ChannelGuards->ChannelGuards.guardAttributes, false , yyt->SelectBangCommand.position) :
			HandleSelectBangCommand (yyt->SelectBangCommand.ChannelGuards->ChannelGuards.commandAttributes, 
				yyt->SelectBangCommand.ChannelGuards->ChannelGuards.guardAttributes, yyt->SelectBangCommand.position)));
break;

case kArbitrateCommand:
yyt->ArbitrateCommand.ChannelGuards->ChannelGuards.context=yyt->ArbitrateCommand.context;
yyt->ArbitrateCommand.ChannelGuards->ChannelGuards.skip=yyt->ArbitrateCommand.skip;
yyVisit1 (yyt->ArbitrateCommand.ChannelGuards);
# line 1963 "Balsa.ag"

	yyt->ArbitrateCommand.attributes = (yyt->ArbitrateCommand.skip ? NoCommandAttributes :
		HandleSelectAndArbitrateCommands (yyt->ArbitrateCommand.ChannelGuards->ChannelGuards.commandAttributes, 
			yyt->ArbitrateCommand.ChannelGuards->ChannelGuards.guardAttributes, true , yyt->ArbitrateCommand.position));
break;

case kPrintCommand:
yyt->PrintCommand.Expr->Expr.context=yyt->PrintCommand.context;
# line 1968 "Balsa.ag"

	yyt->PrintCommand.Expr->Expr.allowChannels = false;
# line 1969 "Balsa.ag"

	yyt->PrintCommand.Expr->Expr.expectedType = ErrorTypeObj;
yyt->PrintCommand.Expr->Expr.skip=yyt->PrintCommand.skip;
yyVisit1 (yyt->PrintCommand.Expr);
# line 1970 "Balsa.ag"

	yyt->PrintCommand.ExprLists->ExprLists.expectConstants = false;
# line 1972 "Balsa.ag"

	yyt->PrintCommand.ExprLists->ExprLists.elements = NULL;
# line 1971 "Balsa.ag"
 
	yyt->PrintCommand.ExprLists->ExprLists.expectedType = NoType;
yyt->PrintCommand.ExprLists->ExprLists.skip=yyt->PrintCommand.skip;
yyt->PrintCommand.ExprLists->ExprLists.context=yyt->PrintCommand.context;
yyVisit1 (yyt->PrintCommand.ExprLists);
# line 1974 "Balsa.ag"

		yyt->PrintCommand.attributes = NoCommandAttributes;

		if (!yyt->PrintCommand.skip)
			yyt->PrintCommand.attributes = HandlePrintCommand (yyt->PrintCommand.context, yyt->PrintCommand.Expr->Expr.attributes, yyt->PrintCommand.ExprLists->ExprLists.attributes, false
				, yyt->PrintCommand.position);
	
break;

case kSinkCommand:
yyt->SinkCommand.Expr->Expr.context=yyt->SinkCommand.context;
# line 1983 "Balsa.ag"

	yyt->SinkCommand.Expr->Expr.allowChannels = false;
# line 1984 "Balsa.ag"

	yyt->SinkCommand.Expr->Expr.expectedType = ErrorTypeObj;
yyt->SinkCommand.Expr->Expr.skip=yyt->SinkCommand.skip;
yyVisit1 (yyt->SinkCommand.Expr);
# line 1986 "Balsa.ag"

		yyt->SinkCommand.attributes = NoCommandAttributes;

		if (!yyt->SinkCommand.skip)
			yyt->SinkCommand.attributes = HandleSinkCommand (yyt->SinkCommand.Expr->Expr.attributes, yyt->SinkCommand.position);
	
break;

case kProcedureParams:
# line 1402 "Balsa.ag"

	yyt->ProcedureParams.actualPorts = NULL;
yyt->ProcedureParams.procedureOut=yyt->ProcedureParams.procedureIn;
break;

case kNullProcParams:
# line 1405 "Balsa.ag"

		yyt->NullProcParams.actualPorts = NULL;

		if (! yyt->NullProcParams.skip)
		{
			if (yyt->NullProcParams.formalParams)
				LOG_ERROR (ParameterListTooShort, NoIdent, yyt->NullProcParams.position);
			else if (yyt->NullProcParams.formalPorts && ! yyt->NullProcParams.noPorts)
				LOG_ERROR (ExprListTooShort, NoIdent, yyt->NullProcParams.position);
		}	
	
yyt->NullProcParams.procedureOut=yyt->NullProcParams.procedureIn;
break;

case kProcParam:
# line 1402 "Balsa.ag"

	yyt->ProcParam.actualPorts = NULL;
yyt->ProcParam.next->ProcedureParams.parameterContext=yyt->ProcParam.parameterContext;
yyt->ProcParam.next->ProcedureParams.noPorts=yyt->ProcParam.noPorts;
yyt->ProcParam.next->ProcedureParams.procedureTree=yyt->ProcParam.procedureTree;
yyt->ProcParam.next->ProcedureParams.procedureIn=yyt->ProcParam.procedureIn;
yyt->ProcParam.next->ProcedureParams.idents=yyt->ProcParam.idents;
yyt->ProcParam.next->ProcedureParams.typeParams=yyt->ProcParam.typeParams;
yyt->ProcParam.next->ProcedureParams.constantParams=yyt->ProcParam.constantParams;
yyt->ProcParam.next->ProcedureParams.formalPorts=yyt->ProcParam.formalPorts;
yyt->ProcParam.next->ProcedureParams.formalParams=yyt->ProcParam.formalParams;
yyt->ProcParam.next->ProcedureParams.skip=yyt->ProcParam.skip;
yyt->ProcParam.next->ProcedureParams.context=yyt->ProcParam.context;
yyVisit1 (yyt->ProcParam.next);
yyt->ProcParam.procedureOut=yyt->ProcParam.next->ProcedureParams.procedureOut;
break;

case kExprProcParam:
yyt->ExprProcParam.next->ProcedureParams.context=yyt->ExprProcParam.context;
# line 1418 "Balsa.ag"

		
	
# line 1422 "Balsa.ag"

		yyt->ExprProcParam.typeIfTypeName = NoType;

		if (Tree_IsType (yyt->ExprProcParam.CoercedExpr, kCoercedExpr))
		{
			yyt->ExprProcParam.CoercedExpr->CoercedExpr.expectedType = NoType;
			yyt->ExprProcParam.CoercedExpr->CoercedExpr.allowChannels = false;
			yyt->ExprProcParam.CoercedExpr->CoercedExpr.skip = yyt->ExprProcParam.skip; 
			yyt->ExprProcParam.CoercedExpr->CoercedExpr.context = yyt->ExprProcParam.context;
		}

		if (! yyt->ExprProcParam.skip)
		{
			if (yyt->ExprProcParam.formalParams)
			{
				if (Tree_IsType (yyt->ExprProcParam.formalParams, kParamPort))
				{
					yyt->ExprProcParam.formalParams->ParamPort.AType->AType.context = yyt->ExprProcParam.parameterContext;
					yyt->ExprProcParam.formalParams->ParamPort.AType->AType.skip = false;
					VisitTreeNode (yyt->ExprProcParam.formalParams->ParamPort.AType);

					yyt->ExprProcParam.CoercedExpr->CoercedExpr.expectedType = yyt->ExprProcParam.formalParams->ParamPort.AType->AType.type;
				}

				if (Tree_IsType (yyt->ExprProcParam.formalParams, kTypeParamPort))
				{
					
					if (Tree_IsType (yyt->ExprProcParam.CoercedExpr, kCoercedExpr) &&
						Tree_IsType (yyt->ExprProcParam.CoercedExpr->CoercedExpr.Expr, kIdentExpr))
					{
						tTree expr = yyt->ExprProcParam.CoercedExpr->CoercedExpr.Expr;
						yyt->ExprProcParam.typeIfTypeName = ExistingTypeLookup (yyt->ExprProcParam.context, expr->IdentExpr.ident, yyt->ExprProcParam.position);
						
						if (yyt->ExprProcParam.typeIfTypeName == NoType)
						{
							LOG_ERROR (ExpectingATypeParam, NoIdent, yyt->ExprProcParam.position);
						} else yyt->ExprProcParam.CoercedExpr->CoercedExpr.skip = true;
					} else LOG_ERROR (ExpectingATypeParam, NoIdent, yyt->ExprProcParam.position);
				}
			} else {
				if (yyt->ExprProcParam.noPorts)
					LOG_ERROR (ExprListTooLong, NoIdent, yyt->ExprProcParam.position);
				else {
					
					if (! Tree_IsType (yyt->ExprProcParam.CoercedExpr, kLinkedChannel))
						yyt->ExprProcParam.CoercedExpr = ConvertCoercedExprToLinkedChannel (yyt->ExprProcParam.CoercedExpr,
							yyt->ExprProcParam.context, yyt->ExprProcParam.formalPorts);
				}
			}
		}
	
yyVisit1 (yyt->ExprProcParam.CoercedExpr);
# line 1476 "Balsa.ag"

		yyt->ExprProcParam.next->ProcedureParams.formalParams = NULL;
		yyt->ExprProcParam.next->ProcedureParams.formalPorts = yyt->ExprProcParam.formalPorts;
		yyt->ExprProcParam.next->ProcedureParams.constantParams = yyt->ExprProcParam.constantParams;
		yyt->ExprProcParam.next->ProcedureParams.typeParams = yyt->ExprProcParam.typeParams;
		yyt->ExprProcParam.next->ProcedureParams.idents = yyt->ExprProcParam.idents;
		yyt->ExprProcParam.next->ProcedureParams.procedureIn = yyt->ExprProcParam.procedureIn;
		yyt->ExprProcParam.next->ProcedureParams.parameterContext = yyt->ExprProcParam.parameterContext;

		if (! yyt->ExprProcParam.skip)
		{
			if (yyt->ExprProcParam.formalParams)
			{
				tTree remainingPorts = NULL;

				if (yyt->ExprProcParam.typeIfTypeName == NoType) 
				{
					if (yyt->ExprProcParam.CoercedExpr->CoercedExpr.expectedType != NoType)
					{
						remainingPorts = HandleExprParam (yyt->ExprProcParam.formalParams, &yyt->ExprProcParam.next->ProcedureParams.parameterContext,
							&yyt->ExprProcParam.next->ProcedureParams.idents, &yyt->ExprProcParam.next->ProcedureParams.constantParams, NULL,
							yyt->ExprProcParam.CoercedExpr->CoercedExpr.attributes.value, yyt->ExprProcParam.CoercedExpr->CoercedExpr.position);
					}
				} else {
					remainingPorts = HandleTypeParam (yyt->ExprProcParam.formalParams, &yyt->ExprProcParam.next->ProcedureParams.parameterContext,
						&yyt->ExprProcParam.next->ProcedureParams.idents, &yyt->ExprProcParam.next->ProcedureParams.typeParams, NULL, yyt->ExprProcParam.typeIfTypeName,
						yyt->ExprProcParam.CoercedExpr->CoercedExpr.position);
				}

				if (! Tree_IsType (remainingPorts, kParamPort)
					&& ! Tree_IsType (remainingPorts, kTypeParamPort))
				{
					
					yyt->ExprProcParam.next->ProcedureParams.procedureIn = SpecifyParameterisedProcedure (yyt->ExprProcParam.context, yyt->ExprProcParam.procedureIn->ident,
						yyt->ExprProcParam.procedureIn, yyt->ExprProcParam.procedureTree,
						yyt->ExprProcParam.next->ProcedureParams.constantParams, yyt->ExprProcParam.next->ProcedureParams.typeParams, remainingPorts, yyt->ExprProcParam.position);

					yyt->ExprProcParam.next->ProcedureParams.formalPorts = yyt->ExprProcParam.next->ProcedureParams.procedureIn->ports;
					yyt->ExprProcParam.next->ProcedureParams.formalParams = NULL;
				} else {
					yyt->ExprProcParam.next->ProcedureParams.formalParams = remainingPorts;
				}
			} else if (yyt->ExprProcParam.formalPorts)
				yyt->ExprProcParam.next->ProcedureParams.formalPorts = CDR (yyt->ExprProcParam.formalPorts);
			else LOG_ERROR (ExprListTooLong, NoIdent, yyt->ExprProcParam.position);
		}
	
yyt->ExprProcParam.next->ProcedureParams.noPorts=yyt->ExprProcParam.noPorts;
yyt->ExprProcParam.next->ProcedureParams.procedureTree=yyt->ExprProcParam.procedureTree;
yyt->ExprProcParam.next->ProcedureParams.skip=yyt->ExprProcParam.skip;
yyVisit1 (yyt->ExprProcParam.next);
# line 1524 "Balsa.ag"

		yyt->ExprProcParam.actualPorts = yyt->ExprProcParam.next->ProcedureParams.actualPorts;

		if (! yyt->ExprProcParam.skip && Tree_IsType (yyt->ExprProcParam.CoercedExpr, kLinkedChannel))
		{
			
			if (yyt->ExprProcParam.CoercedExpr->LinkedChannel.attributes)
				yyt->ExprProcParam.actualPorts = AppendExprAttributesLists (yyt->ExprProcParam.CoercedExpr->LinkedChannel.attributes, yyt->ExprProcParam.next->ProcedureParams.actualPorts);
			else yyt->ExprProcParam.actualPorts = yyt->ExprProcParam.next->ProcedureParams.actualPorts;
		}
	
yyt->ExprProcParam.procedureOut=yyt->ExprProcParam.next->ProcedureParams.procedureOut;
break;

case kTypeProcParam:
yyt->TypeProcParam.next->ProcedureParams.context=yyt->TypeProcParam.context;
yyt->TypeProcParam.AType->AType.skip=yyt->TypeProcParam.skip;
yyt->TypeProcParam.AType->AType.context=yyt->TypeProcParam.context;
yyVisit1 (yyt->TypeProcParam.AType);
# line 1538 "Balsa.ag"

		yyt->TypeProcParam.next->ProcedureParams.parameterContext = yyt->TypeProcParam.parameterContext;
		yyt->TypeProcParam.next->ProcedureParams.formalParams = NULL;
		yyt->TypeProcParam.next->ProcedureParams.formalPorts = yyt->TypeProcParam.formalPorts;
		yyt->TypeProcParam.next->ProcedureParams.constantParams = yyt->TypeProcParam.constantParams;
		yyt->TypeProcParam.next->ProcedureParams.typeParams = yyt->TypeProcParam.typeParams;
		yyt->TypeProcParam.next->ProcedureParams.idents = yyt->TypeProcParam.idents;
		yyt->TypeProcParam.next->ProcedureParams.procedureIn = yyt->TypeProcParam.procedureIn;

		if (! yyt->TypeProcParam.skip)
		{
			if (yyt->TypeProcParam.formalParams)
			{
				tTree remainingPorts = HandleTypeParam (yyt->TypeProcParam.formalParams, &yyt->TypeProcParam.next->ProcedureParams.parameterContext,
					&yyt->TypeProcParam.next->ProcedureParams.idents,&yyt->TypeProcParam.next->ProcedureParams.typeParams, NULL, yyt->TypeProcParam.AType->AType.type,
					yyt->TypeProcParam.AType->AType.position);

				if (! Tree_IsType (remainingPorts, kParamPort)
					&& ! Tree_IsType (remainingPorts, kTypeParamPort))
				{
					
					yyt->TypeProcParam.next->ProcedureParams.procedureIn = SpecifyParameterisedProcedure (yyt->TypeProcParam.context, yyt->TypeProcParam.procedureIn->ident,
						yyt->TypeProcParam.procedureIn, yyt->TypeProcParam.procedureTree,
						yyt->TypeProcParam.next->ProcedureParams.constantParams, yyt->TypeProcParam.next->ProcedureParams.typeParams, remainingPorts, yyt->TypeProcParam.position);

					yyt->TypeProcParam.next->ProcedureParams.formalPorts = yyt->TypeProcParam.next->ProcedureParams.procedureIn->ports;
					yyt->TypeProcParam.next->ProcedureParams.formalParams = NULL;
				} else {
					yyt->TypeProcParam.next->ProcedureParams.formalParams = remainingPorts;
				}
			} else if (yyt->TypeProcParam.noPorts)
				LOG_ERROR (ExprListTooLong, NoIdent, yyt->TypeProcParam.position);
		}
	
yyt->TypeProcParam.next->ProcedureParams.noPorts=yyt->TypeProcParam.noPorts;
yyt->TypeProcParam.next->ProcedureParams.procedureTree=yyt->TypeProcParam.procedureTree;
yyt->TypeProcParam.next->ProcedureParams.skip=yyt->TypeProcParam.skip;
yyVisit1 (yyt->TypeProcParam.next);
# line 1573 "Balsa.ag"


	yyt->TypeProcParam.actualPorts = yyt->TypeProcParam.next->ProcedureParams.actualPorts;
yyt->TypeProcParam.procedureOut=yyt->TypeProcParam.next->ProcedureParams.procedureOut;
break;

case kVarReadProcParam:
yyt->VarReadProcParam.next->ProcedureParams.context=yyt->VarReadProcParam.context;
# line 1590 "Balsa.ag"

		yyt->VarReadProcParam.next->ProcedureParams.formalParams = NULL;
		yyt->VarReadProcParam.next->ProcedureParams.formalPorts = yyt->VarReadProcParam.formalPorts;
		yyt->VarReadProcParam.next->ProcedureParams.constantParams = yyt->VarReadProcParam.constantParams;
		yyt->VarReadProcParam.next->ProcedureParams.typeParams = yyt->VarReadProcParam.typeParams;
		yyt->VarReadProcParam.next->ProcedureParams.idents = yyt->VarReadProcParam.idents;
		yyt->VarReadProcParam.next->ProcedureParams.procedureIn = yyt->VarReadProcParam.procedureIn;
		yyt->VarReadProcParam.next->ProcedureParams.parameterContext = yyt->VarReadProcParam.parameterContext;

		if (! yyt->VarReadProcParam.skip)
		{
			if (yyt->VarReadProcParam.formalParams)
			{
				
				(void) GetNextBalsaParam (&(yyt->VarReadProcParam.next->ProcedureParams.formalParams), NULL, &(yyt->VarReadProcParam.next->ProcedureParams.idents), yyt->VarReadProcParam.position);
				LOG_ERROR (BadVarAccessActualParameter, NoIdent, yyt->VarReadProcParam.position);
			} else if (yyt->VarReadProcParam.noPorts)
				LOG_ERROR (ExprListTooLong, NoIdent, yyt->VarReadProcParam.position);
			else if (yyt->VarReadProcParam.formalPorts)
				yyt->VarReadProcParam.next->ProcedureParams.formalPorts = CDR (yyt->VarReadProcParam.formalPorts);
			else LOG_ERROR (ExprListTooLong, NoIdent, yyt->VarReadProcParam.position);
		}
	
yyt->VarReadProcParam.next->ProcedureParams.noPorts=yyt->VarReadProcParam.noPorts;
yyt->VarReadProcParam.next->ProcedureParams.procedureTree=yyt->VarReadProcParam.procedureTree;
yyt->VarReadProcParam.next->ProcedureParams.skip=yyt->VarReadProcParam.skip;
yyVisit1 (yyt->VarReadProcParam.next);
yyt->VarReadProcParam.CoercedExpr->CoercedExpr.skip=yyt->VarReadProcParam.skip;
# line 1576 "Balsa.ag"

	yyt->VarReadProcParam.CoercedExpr->CoercedExpr.allowChannels = false;
# line 1577 "Balsa.ag"

		yyt->VarReadProcParam.CoercedExpr->CoercedExpr.expectedType = NoType;

		if (! yyt->VarReadProcParam.skip)
		{
			if (! yyt->VarReadProcParam.procedureIn)
				LOG_ERROR (BadVarAccessActualParameter, NoIdent, yyt->VarReadProcParam.position);
			else if (yyt->VarReadProcParam.formalPorts)
				yyt->VarReadProcParam.CoercedExpr->CoercedExpr.expectedType = CAR (yyt->VarReadProcParam.formalPorts)->type;
		}
	
yyt->VarReadProcParam.CoercedExpr->CoercedExpr.context=yyt->VarReadProcParam.context;
yyVisit1 (yyt->VarReadProcParam.CoercedExpr);
# line 1614 "Balsa.ag"

		yyt->VarReadProcParam.actualPorts = yyt->VarReadProcParam.next->ProcedureParams.actualPorts;

		if (! yyt->VarReadProcParam.skip)
		{
			if (! TypeEquivalence (yyt->VarReadProcParam.CoercedExpr->CoercedExpr.attributes.value.type, CAR (yyt->VarReadProcParam.formalPorts)->type))
				LOG_ERROR (TypeIsNotValid, NoIdent, yyt->VarReadProcParam.position);
			else if (CAR (yyt->VarReadProcParam.formalPorts)->info.port.sense != ActivePortSense)
				LOG_ERROR (BadVarAccessActualParameter, NoIdent, yyt->VarReadProcParam.position);
			else yyt->VarReadProcParam.actualPorts = NewExprAttributesList (yyt->VarReadProcParam.CoercedExpr->CoercedExpr.attributes, yyt->VarReadProcParam.next->ProcedureParams.actualPorts);
		}
	
yyt->VarReadProcParam.procedureOut=yyt->VarReadProcParam.next->ProcedureParams.procedureOut;
break;

case kVarWriteProcParam:
yyt->VarWriteProcParam.next->ProcedureParams.context=yyt->VarWriteProcParam.context;
# line 1633 "Balsa.ag"

		yyt->VarWriteProcParam.next->ProcedureParams.formalParams = NULL;
		yyt->VarWriteProcParam.next->ProcedureParams.formalPorts = yyt->VarWriteProcParam.formalPorts;
		yyt->VarWriteProcParam.next->ProcedureParams.constantParams = yyt->VarWriteProcParam.constantParams;
		yyt->VarWriteProcParam.next->ProcedureParams.typeParams = yyt->VarWriteProcParam.typeParams;
		yyt->VarWriteProcParam.next->ProcedureParams.idents = yyt->VarWriteProcParam.idents;
		yyt->VarWriteProcParam.next->ProcedureParams.procedureIn = yyt->VarWriteProcParam.procedureIn;
		yyt->VarWriteProcParam.next->ProcedureParams.parameterContext = yyt->VarWriteProcParam.parameterContext;

		if (! yyt->VarWriteProcParam.skip)
		{
			if (yyt->VarWriteProcParam.formalParams)
			{
				
				(void) GetNextBalsaParam (&(yyt->VarWriteProcParam.next->ProcedureParams.formalParams), NULL, &(yyt->VarWriteProcParam.next->ProcedureParams.idents), yyt->VarWriteProcParam.position);
				LOG_ERROR (BadVarAccessActualParameter, NoIdent, yyt->VarWriteProcParam.position);
			} else if (yyt->VarWriteProcParam.noPorts)
				LOG_ERROR (ExprListTooLong, NoIdent, yyt->VarWriteProcParam.position);
			else if (yyt->VarWriteProcParam.formalPorts)
				yyt->VarWriteProcParam.next->ProcedureParams.formalPorts = CDR (yyt->VarWriteProcParam.formalPorts);
			else LOG_ERROR (ExprListTooLong, NoIdent, yyt->VarWriteProcParam.position);
		}
	
yyt->VarWriteProcParam.next->ProcedureParams.noPorts=yyt->VarWriteProcParam.noPorts;
yyt->VarWriteProcParam.next->ProcedureParams.procedureTree=yyt->VarWriteProcParam.procedureTree;
yyt->VarWriteProcParam.next->ProcedureParams.skip=yyt->VarWriteProcParam.skip;
yyVisit1 (yyt->VarWriteProcParam.next);
# line 1628 "Balsa.ag"

	yyt->VarWriteProcParam.Lvalue->Lvalue.expectedBaseType = NoType;
# line 1630 "Balsa.ag"
 yyt->VarWriteProcParam.Lvalue->Lvalue.lockPassiveChannels = false;
# line 1630 "Balsa.ag"
 yyt->VarWriteProcParam.Lvalue->Lvalue.isPassive = false;
# line 1630 "Balsa.ag"

	yyt->VarWriteProcParam.Lvalue->Lvalue.isInput = false;
# line 1629 "Balsa.ag"
 yyt->VarWriteProcParam.Lvalue->Lvalue.expectingEither = false;
# line 1629 "Balsa.ag"
 
	yyt->VarWriteProcParam.Lvalue->Lvalue.expectingChannel = false;
yyt->VarWriteProcParam.Lvalue->Lvalue.skip=yyt->VarWriteProcParam.skip;
yyt->VarWriteProcParam.Lvalue->Lvalue.context=yyt->VarWriteProcParam.context;
yyVisit1 (yyt->VarWriteProcParam.Lvalue);
# line 1657 "Balsa.ag"

		yyt->VarWriteProcParam.actualPorts = yyt->VarWriteProcParam.next->ProcedureParams.actualPorts;

		if (! yyt->VarWriteProcParam.skip)
		{
			ExprAttributes varAttrs = FinaliseLvalue (yyt->VarWriteProcParam.Lvalue->Lvalue.attributes, yyt->VarWriteProcParam.Lvalue->Lvalue.instance, yyt->VarWriteProcParam.Lvalue->Lvalue.partitions,
				yyt->VarWriteProcParam.Lvalue->Lvalue.indices, yyt->VarWriteProcParam.Lvalue->Lvalue.indexWire, yyt->VarWriteProcParam.Lvalue->Lvalue.access, yyt->VarWriteProcParam.position);

			if (! TypeEquivalence (varAttrs.value.type, CAR (yyt->VarWriteProcParam.formalPorts)->type))
				LOG_ERROR (TypeIsNotValid, NoIdent, yyt->VarWriteProcParam.position);
			else yyt->VarWriteProcParam.actualPorts = NewExprAttributesList (varAttrs, yyt->VarWriteProcParam.next->ProcedureParams.actualPorts);
		}
	
yyt->VarWriteProcParam.procedureOut=yyt->VarWriteProcParam.next->ProcedureParams.procedureOut;
break;

case kBlockProcParam:
yyt->BlockProcParam.next->ProcedureParams.context=yyt->BlockProcParam.context;
yyt->BlockProcParam.next->ProcedureParams.parameterContext=yyt->BlockProcParam.parameterContext;
yyt->BlockProcParam.next->ProcedureParams.noPorts=yyt->BlockProcParam.noPorts;
yyt->BlockProcParam.next->ProcedureParams.procedureTree=yyt->BlockProcParam.procedureTree;
yyt->BlockProcParam.next->ProcedureParams.procedureIn=yyt->BlockProcParam.procedureIn;
yyt->BlockProcParam.next->ProcedureParams.idents=yyt->BlockProcParam.idents;
yyt->BlockProcParam.next->ProcedureParams.typeParams=yyt->BlockProcParam.typeParams;
yyt->BlockProcParam.next->ProcedureParams.constantParams=yyt->BlockProcParam.constantParams;
# line 1673 "Balsa.ag"

		yyt->BlockProcParam.next->ProcedureParams.formalPorts = yyt->BlockProcParam.formalPorts;

		if (! yyt->BlockProcParam.skip)
		{
			if (yyt->BlockProcParam.formalParams)
				LOG_ERROR (ExpectingAParameterNotABlock, NoIdent, yyt->BlockProcParam.Block->Block.position);
			else if (yyt->BlockProcParam.formalPorts)
			{
				PtrInstance port = CAR (yyt->BlockProcParam.formalPorts);
				if (port->type != SyncTypeObj ||
					port->info.port.sense == PassivePortSense)
					LOG_ERROR (BlockExpectsActiveSyncPort, NoIdent, yyt->BlockProcParam.Block->Block.position);

				yyt->BlockProcParam.next->ProcedureParams.formalPorts = CDR (yyt->BlockProcParam.formalPorts);
			}	
		}
	
yyt->BlockProcParam.next->ProcedureParams.formalParams=yyt->BlockProcParam.formalParams;
yyt->BlockProcParam.next->ProcedureParams.skip=yyt->BlockProcParam.skip;
yyVisit1 (yyt->BlockProcParam.next);
yyt->BlockProcParam.Block->Block.skip=yyt->BlockProcParam.skip;
# line 1672 "Balsa.ag"

	yyt->BlockProcParam.Block->Block.context = AddContextMarker (yyt->BlockProcParam.context);
yyVisit1 (yyt->BlockProcParam.Block);
# line 1692 "Balsa.ag"

		yyt->BlockProcParam.actualPorts = yyt->BlockProcParam.next->ProcedureParams.actualPorts;

		if (! yyt->BlockProcParam.skip && yyt->BlockProcParam.Block->Block.attributes.activation)
		{
			ExprAttributes attr;

			attr.components = yyt->BlockProcParam.Block->Block.attributes.components;
			attr.accesses = yyt->BlockProcParam.Block->Block.attributes.accesses;
			attr.wires = yyt->BlockProcParam.Block->Block.attributes.wires;
			attr.activation = yyt->BlockProcParam.Block->Block.attributes.activation;
			attr.value = NoValue;
			attr.value.type = SyncTypeObj;

			yyt->BlockProcParam.actualPorts = NewExprAttributesList (attr, yyt->BlockProcParam.next->ProcedureParams.actualPorts);
		}
	
yyt->BlockProcParam.procedureOut=yyt->BlockProcParam.next->ProcedureParams.procedureOut;
break;

case kFunctionParams:
# line 1713 "Balsa.ag"

	yyt->FunctionParams.actualPorts = NULL;
yyt->FunctionParams.functionOut=yyt->FunctionParams.functionIn;
break;

case kNullFuncParams:
# line 1716 "Balsa.ag"

		yyt->NullFuncParams.actualPorts = NULL;

		if (! yyt->NullFuncParams.skip)
		{
			if (yyt->NullFuncParams.formalParams)
				LOG_ERROR (ParameterListTooShort, NoIdent, yyt->NullFuncParams.position);
			else if (yyt->NullFuncParams.formalPorts)
				LOG_ERROR (ExprListTooShort, NoIdent, yyt->NullFuncParams.position);
		}	
	
yyt->NullFuncParams.functionOut=yyt->NullFuncParams.functionIn;
break;

case kFuncParam:
# line 1713 "Balsa.ag"

	yyt->FuncParam.actualPorts = NULL;
yyt->FuncParam.next->FunctionParams.functionIn=yyt->FuncParam.functionIn;
yyt->FuncParam.next->FunctionParams.params=yyt->FuncParam.params;
yyt->FuncParam.next->FunctionParams.parameterContext=yyt->FuncParam.parameterContext;
yyt->FuncParam.next->FunctionParams.functionTree=yyt->FuncParam.functionTree;
yyt->FuncParam.next->FunctionParams.idents=yyt->FuncParam.idents;
yyt->FuncParam.next->FunctionParams.typeParams=yyt->FuncParam.typeParams;
yyt->FuncParam.next->FunctionParams.constantParams=yyt->FuncParam.constantParams;
yyt->FuncParam.next->FunctionParams.formalPorts=yyt->FuncParam.formalPorts;
yyt->FuncParam.next->FunctionParams.formalParams=yyt->FuncParam.formalParams;
yyt->FuncParam.next->FunctionParams.skip=yyt->FuncParam.skip;
yyt->FuncParam.next->FunctionParams.context=yyt->FuncParam.context;
yyVisit1 (yyt->FuncParam.next);
yyt->FuncParam.functionOut=yyt->FuncParam.next->FunctionParams.functionOut;
break;

case kExprFuncParam:
yyt->ExprFuncParam.next->FunctionParams.context=yyt->ExprFuncParam.context;
# line 1729 "Balsa.ag"

		
	
# line 1733 "Balsa.ag"

		yyt->ExprFuncParam.typeIfTypeName = NoType;
		yyt->ExprFuncParam.CoercedExpr->CoercedExpr.expectedType = NoType;
		yyt->ExprFuncParam.CoercedExpr->CoercedExpr.allowChannels = false;
		yyt->ExprFuncParam.CoercedExpr->CoercedExpr.skip = yyt->ExprFuncParam.skip;

		if (! yyt->ExprFuncParam.skip)
		{
			if (yyt->ExprFuncParam.formalParams)
			{
				if (Tree_IsType (yyt->ExprFuncParam.formalParams, kParamPort))
				{
					yyt->ExprFuncParam.formalParams->ParamPort.AType->AType.context = yyt->ExprFuncParam.parameterContext;
					yyt->ExprFuncParam.formalParams->ParamPort.AType->AType.skip = false;
					VisitTreeNode (yyt->ExprFuncParam.formalParams->ParamPort.AType);
					yyt->ExprFuncParam.CoercedExpr->CoercedExpr.expectedType = yyt->ExprFuncParam.formalParams->ParamPort.AType->AType.type;
				}

				if (Tree_IsType (yyt->ExprFuncParam.formalParams, kTypeParamPort))
				{
					
					if (Tree_IsType (yyt->ExprFuncParam.CoercedExpr->CoercedExpr.Expr, kIdentExpr))
					{
						tTree expr = yyt->ExprFuncParam.CoercedExpr->CoercedExpr.Expr;
						yyt->ExprFuncParam.typeIfTypeName = ExistingTypeLookup (yyt->ExprFuncParam.context, expr->IdentExpr.ident, yyt->ExprFuncParam.position);
						
						if (yyt->ExprFuncParam.typeIfTypeName == NoType)
						{
							LOG_ERROR (ExpectingATypeParam, NoIdent, yyt->ExprFuncParam.position);
						} else yyt->ExprFuncParam.CoercedExpr->CoercedExpr.skip = true;
					} else LOG_ERROR (ExpectingATypeParam, NoIdent, yyt->ExprFuncParam.position);
				}
			} else if (yyt->ExprFuncParam.formalPorts)
				yyt->ExprFuncParam.CoercedExpr->CoercedExpr.expectedType = CAR (yyt->ExprFuncParam.formalPorts)->type;
		}
	
yyt->ExprFuncParam.CoercedExpr->CoercedExpr.context=yyt->ExprFuncParam.context;
yyVisit1 (yyt->ExprFuncParam.CoercedExpr);
yyt->ExprFuncParam.next->FunctionParams.functionIn=yyt->ExprFuncParam.functionIn;
# line 1770 "Balsa.ag"

		
	
# line 1774 "Balsa.ag"

		yyt->ExprFuncParam.next->FunctionParams.formalParams = NULL;
		yyt->ExprFuncParam.next->FunctionParams.formalPorts = yyt->ExprFuncParam.formalPorts;
		yyt->ExprFuncParam.next->FunctionParams.constantParams = yyt->ExprFuncParam.constantParams;
		yyt->ExprFuncParam.next->FunctionParams.typeParams = yyt->ExprFuncParam.typeParams;
		yyt->ExprFuncParam.next->FunctionParams.params = yyt->ExprFuncParam.params;
		yyt->ExprFuncParam.next->FunctionParams.idents = yyt->ExprFuncParam.idents;
		yyt->ExprFuncParam.next->FunctionParams.parameterContext = yyt->ExprFuncParam.parameterContext;

		if (! yyt->ExprFuncParam.skip)
		{
			if (yyt->ExprFuncParam.formalParams)
			{
				tTree remainingPorts = NULL;

				if (yyt->ExprFuncParam.typeIfTypeName == NoType) 
				{
					if (yyt->ExprFuncParam.CoercedExpr->CoercedExpr.expectedType != NoType)
					{
						remainingPorts = HandleExprParam (yyt->ExprFuncParam.formalParams, &yyt->ExprFuncParam.next->FunctionParams.parameterContext,
							&yyt->ExprFuncParam.next->FunctionParams.idents, &yyt->ExprFuncParam.next->FunctionParams.constantParams, &yyt->ExprFuncParam.next->FunctionParams.params,
							yyt->ExprFuncParam.CoercedExpr->CoercedExpr.attributes.value, yyt->ExprFuncParam.CoercedExpr->CoercedExpr.position);
					}
				} else {
					remainingPorts = HandleTypeParam (yyt->ExprFuncParam.formalParams, &yyt->ExprFuncParam.next->FunctionParams.parameterContext,
						&yyt->ExprFuncParam.next->FunctionParams.idents, &yyt->ExprFuncParam.next->FunctionParams.typeParams, &yyt->ExprFuncParam.next->FunctionParams.params, yyt->ExprFuncParam.typeIfTypeName,
						yyt->ExprFuncParam.CoercedExpr->CoercedExpr.position);
				}

				if (! Tree_IsType (remainingPorts, kParamPort)
					&& ! Tree_IsType (remainingPorts, kTypeParamPort))
				{
					
					if (yyt->ExprFuncParam.functionIn->nature == ParameterisedBuiltinFunction)
					{
						yyt->ExprFuncParam.next->FunctionParams.functionIn = SpecifyParameterisedBuiltinFunction (yyt->ExprFuncParam.context, yyt->ExprFuncParam.functionIn->ident,
							yyt->ExprFuncParam.functionIn, yyt->ExprFuncParam.functionTree,
							yyt->ExprFuncParam.next->FunctionParams.constantParams, yyt->ExprFuncParam.next->FunctionParams.typeParams, remainingPorts,
							yyt->ExprFuncParam.next->FunctionParams.params, yyt->ExprFuncParam.position);
					} else {
						LOG_ERROR (OnlyBuiltinFunctionsCanHaveParameters, NoIdent, yyt->ExprFuncParam.position);
					}

					yyt->ExprFuncParam.next->FunctionParams.formalPorts = yyt->ExprFuncParam.next->FunctionParams.functionIn->ports;
					yyt->ExprFuncParam.next->FunctionParams.formalParams = NULL;
				} else {
					yyt->ExprFuncParam.next->FunctionParams.formalParams = remainingPorts;
				}
			} else if (yyt->ExprFuncParam.formalPorts)
			{
				if (yyt->ExprFuncParam.CoercedExpr->CoercedExpr.expectedType != NoType &&
					!TypeEquivalence ((yyt->ExprFuncParam.CoercedExpr->CoercedExpr.attributes).value.type, yyt->ExprFuncParam.CoercedExpr->CoercedExpr.expectedType)) 
					LOG_ERROR (TypeIsNotValid, NoIdent, yyt->ExprFuncParam.CoercedExpr->CoercedExpr.position);  

				yyt->ExprFuncParam.next->FunctionParams.formalPorts = CDR (yyt->ExprFuncParam.formalPorts);
			}
			else LOG_ERROR (ExprListTooLong, NoIdent, yyt->ExprFuncParam.position);
		}
	
yyt->ExprFuncParam.next->FunctionParams.functionTree=yyt->ExprFuncParam.functionTree;
yyt->ExprFuncParam.next->FunctionParams.skip=yyt->ExprFuncParam.skip;
yyVisit1 (yyt->ExprFuncParam.next);
# line 1834 "Balsa.ag"

		yyt->ExprFuncParam.actualPorts = yyt->ExprFuncParam.next->FunctionParams.actualPorts;

		if (! yyt->ExprFuncParam.skip && yyt->ExprFuncParam.formalPorts)
			yyt->ExprFuncParam.actualPorts = NewExprAttributesList (yyt->ExprFuncParam.CoercedExpr->CoercedExpr.attributes, yyt->ExprFuncParam.next->FunctionParams.actualPorts);
	
yyt->ExprFuncParam.functionOut=yyt->ExprFuncParam.next->FunctionParams.functionOut;
break;

case kTypeFuncParam:
yyt->TypeFuncParam.next->FunctionParams.context=yyt->TypeFuncParam.context;
yyt->TypeFuncParam.AType->AType.skip=yyt->TypeFuncParam.skip;
yyt->TypeFuncParam.AType->AType.context=yyt->TypeFuncParam.context;
yyVisit1 (yyt->TypeFuncParam.AType);
yyt->TypeFuncParam.next->FunctionParams.functionIn=yyt->TypeFuncParam.functionIn;
yyt->TypeFuncParam.next->FunctionParams.params=yyt->TypeFuncParam.params;
# line 1843 "Balsa.ag"

		yyt->TypeFuncParam.next->FunctionParams.parameterContext = yyt->TypeFuncParam.parameterContext;
		yyt->TypeFuncParam.next->FunctionParams.formalParams = NULL;
		yyt->TypeFuncParam.next->FunctionParams.formalPorts = yyt->TypeFuncParam.formalPorts;
		yyt->TypeFuncParam.next->FunctionParams.constantParams = yyt->TypeFuncParam.constantParams;
		yyt->TypeFuncParam.next->FunctionParams.typeParams = yyt->TypeFuncParam.typeParams;
		yyt->TypeFuncParam.next->FunctionParams.idents = yyt->TypeFuncParam.idents;

		if (! yyt->TypeFuncParam.skip)
		{
			if (yyt->TypeFuncParam.formalParams)
			{
				tTree remainingPorts = NULL;

				remainingPorts = HandleTypeParam (yyt->TypeFuncParam.formalParams, &yyt->TypeFuncParam.next->FunctionParams.parameterContext,
					&yyt->TypeFuncParam.next->FunctionParams.idents, &yyt->TypeFuncParam.next->FunctionParams.typeParams, &yyt->TypeFuncParam.next->FunctionParams.params, yyt->TypeFuncParam.AType->AType.type,
					yyt->TypeFuncParam.AType->AType.position);

				if (! Tree_IsType (remainingPorts, kParamPort)
					&& ! Tree_IsType (remainingPorts, kTypeParamPort))
				{
					
					if (yyt->TypeFuncParam.functionIn->nature == ParameterisedBuiltinFunction)
					{
						yyt->TypeFuncParam.next->FunctionParams.functionIn = SpecifyParameterisedBuiltinFunction (yyt->TypeFuncParam.context, yyt->TypeFuncParam.functionIn->ident,
							yyt->TypeFuncParam.functionIn, yyt->TypeFuncParam.functionTree,
							yyt->TypeFuncParam.next->FunctionParams.constantParams, yyt->TypeFuncParam.next->FunctionParams.typeParams, remainingPorts,
							yyt->TypeFuncParam.next->FunctionParams.params, yyt->TypeFuncParam.position);
					} else {
						LOG_ERROR (OnlyBuiltinFunctionsCanHaveParameters, NoIdent, yyt->TypeFuncParam.position);
					}

					yyt->TypeFuncParam.next->FunctionParams.formalPorts = yyt->TypeFuncParam.next->FunctionParams.functionIn->ports;
					yyt->TypeFuncParam.next->FunctionParams.formalParams = NULL;
				} else {
					yyt->TypeFuncParam.next->FunctionParams.formalParams = remainingPorts;
				}
			}
		}
	
yyt->TypeFuncParam.next->FunctionParams.functionTree=yyt->TypeFuncParam.functionTree;
yyt->TypeFuncParam.next->FunctionParams.skip=yyt->TypeFuncParam.skip;
yyVisit1 (yyt->TypeFuncParam.next);
# line 1884 "Balsa.ag"


	yyt->TypeFuncParam.actualPorts = yyt->TypeFuncParam.next->FunctionParams.actualPorts;
yyt->TypeFuncParam.functionOut=yyt->TypeFuncParam.next->FunctionParams.functionOut;
break;

case kLvalue:
# line 2020 "Balsa.ag"
 yyt->Lvalue.attributes = NoExprAttributes;
# line 2021 "Balsa.ag"
 yyt->Lvalue.access = NULL;
# line 2021 "Balsa.ag"
 yyt->Lvalue.indexWire = NULL;
# line 2021 "Balsa.ag"

	yyt->Lvalue.indices = NULL;
# line 2020 "Balsa.ag"
 yyt->Lvalue.partitions = NULL;
# line 2020 "Balsa.ag"
 yyt->Lvalue.instance = NULL;
break;

case kIdentLvalue:
# line 2023 "Balsa.ag"

		yyt->IdentLvalue.instance = NULL;
		yyt->IdentLvalue.partitions = NULL;
		yyt->IdentLvalue.indices = NULL;
		yyt->IdentLvalue.indexWire = NULL;
		yyt->IdentLvalue.access = NULL;
		yyt->IdentLvalue.attributes = NoExprAttributes;

		if (! yyt->IdentLvalue.skip)
		{
			yyt->IdentLvalue.instance = LookupInstance (yyt->IdentLvalue.context, yyt->IdentLvalue.ident, false );

			if (!yyt->IdentLvalue.instance)
				LOG_ERROR (UnboundIdentifier, yyt->IdentLvalue.ident, yyt->IdentLvalue.position);
			else {
				bool isAVariable = yyt->IdentLvalue.instance->nature == VariableInstance;
				bool isAChannel = yyt->IdentLvalue.instance->nature == OutputChannelInstance ||
					yyt->IdentLvalue.instance->nature == InputChannelInstance ||
					yyt->IdentLvalue.instance->nature == ChannelInstance;

				if (isAVariable && (yyt->IdentLvalue.expectingEither || ! yyt->IdentLvalue.expectingChannel))
					yyt->IdentLvalue.attributes = HandleIdentLvalue (yyt->IdentLvalue.context, &yyt->IdentLvalue.instance, &yyt->IdentLvalue.partitions, &yyt->IdentLvalue.access, yyt->IdentLvalue.position);
				else if (isAChannel && (yyt->IdentLvalue.expectingEither || yyt->IdentLvalue.expectingChannel))
					yyt->IdentLvalue.attributes = HandleIdentChannel (yyt->IdentLvalue.context, &yyt->IdentLvalue.instance, yyt->IdentLvalue.isInput, yyt->IdentLvalue.isPassive, yyt->IdentLvalue.lockPassiveChannels, yyt->IdentLvalue.position);
				else { 
					yyt->IdentLvalue.instance = NULL;
					if (yyt->IdentLvalue.expectingChannel)
						LOG_ERROR (ExpectingAChannel, NoIdent, yyt->IdentLvalue.position);
					else if (! yyt->IdentLvalue.expectingEither)
						LOG_ERROR (ExpectingAWritableVariable, NoIdent, yyt->IdentLvalue.position);
					else LOG_ERROR (ExpectingAWritableVariableOrChannel, NoIdent, yyt->IdentLvalue.position);
				};
			}
		}
	
break;

case kRecordElemLvalue:
yyt->RecordElemLvalue.Lvalue->Lvalue.context=yyt->RecordElemLvalue.context;
yyt->RecordElemLvalue.Lvalue->Lvalue.expectedBaseType=yyt->RecordElemLvalue.expectedBaseType;
yyt->RecordElemLvalue.Lvalue->Lvalue.lockPassiveChannels=yyt->RecordElemLvalue.lockPassiveChannels;
yyt->RecordElemLvalue.Lvalue->Lvalue.isPassive=yyt->RecordElemLvalue.isPassive;
yyt->RecordElemLvalue.Lvalue->Lvalue.isInput=yyt->RecordElemLvalue.isInput;
yyt->RecordElemLvalue.Lvalue->Lvalue.expectingEither=yyt->RecordElemLvalue.expectingEither;
yyt->RecordElemLvalue.Lvalue->Lvalue.expectingChannel=yyt->RecordElemLvalue.expectingChannel;
yyt->RecordElemLvalue.Lvalue->Lvalue.skip=yyt->RecordElemLvalue.skip;
yyVisit1 (yyt->RecordElemLvalue.Lvalue);
# line 2125 "Balsa.ag"

		yyt->RecordElemLvalue.instance = NULL;
		yyt->RecordElemLvalue.partitions = NULL;
		yyt->RecordElemLvalue.attributes = NoExprAttributes;

		if (! yyt->RecordElemLvalue.skip && yyt->RecordElemLvalue.Lvalue->Lvalue.instance)
		{
			if (yyt->RecordElemLvalue.Lvalue->Lvalue.instance->nature == VariableInstance)
			{
				yyt->RecordElemLvalue.attributes = HandleRecordElemLvalue (yyt->RecordElemLvalue.Lvalue->Lvalue.attributes,
					yyt->RecordElemLvalue.Lvalue->Lvalue.instance, yyt->RecordElemLvalue.Lvalue->Lvalue.partitions, yyt->RecordElemLvalue.ident, &yyt->RecordElemLvalue.instance, &yyt->RecordElemLvalue.partitions, yyt->RecordElemLvalue.Lvalue->Lvalue.access, yyt->RecordElemLvalue.position);
			} else LOG_ERROR (ExpectingAVariableOnLHS, NoIdent, yyt->RecordElemLvalue.position);
		}
	
# line 2123 "Balsa.ag"

	yyt->RecordElemLvalue.access = yyt->RecordElemLvalue.Lvalue->Lvalue.access;
# line 2124 "Balsa.ag"

	yyt->RecordElemLvalue.indexWire = yyt->RecordElemLvalue.Lvalue->Lvalue.indexWire;
# line 2122 "Balsa.ag"

	yyt->RecordElemLvalue.indices = yyt->RecordElemLvalue.Lvalue->Lvalue.indices;
break;

case kArrayElemLvalue:
yyt->ArrayElemLvalue.Lvalue->Lvalue.context=yyt->ArrayElemLvalue.context;
yyt->ArrayElemLvalue.Lvalue->Lvalue.expectedBaseType=yyt->ArrayElemLvalue.expectedBaseType;
yyt->ArrayElemLvalue.Lvalue->Lvalue.lockPassiveChannels=yyt->ArrayElemLvalue.lockPassiveChannels;
yyt->ArrayElemLvalue.Lvalue->Lvalue.isPassive=yyt->ArrayElemLvalue.isPassive;
yyt->ArrayElemLvalue.Lvalue->Lvalue.isInput=yyt->ArrayElemLvalue.isInput;
yyt->ArrayElemLvalue.Lvalue->Lvalue.expectingEither=yyt->ArrayElemLvalue.expectingEither;
yyt->ArrayElemLvalue.Lvalue->Lvalue.expectingChannel=yyt->ArrayElemLvalue.expectingChannel;
yyt->ArrayElemLvalue.Lvalue->Lvalue.skip=yyt->ArrayElemLvalue.skip;
yyVisit1 (yyt->ArrayElemLvalue.Lvalue);
yyt->ArrayElemLvalue.CoercedExpr->CoercedExpr.skip=yyt->ArrayElemLvalue.skip;
# line 2066 "Balsa.ag"

	yyt->ArrayElemLvalue.CoercedExpr->CoercedExpr.allowChannels = false;
yyt->ArrayElemLvalue.CoercedExpr->CoercedExpr.context=yyt->ArrayElemLvalue.context;
# line 2060 "Balsa.ag"
 
	yyt->ArrayElemLvalue.CoercedExpr->CoercedExpr.expectedType = 
		((yyt->ArrayElemLvalue.Lvalue->Lvalue.attributes).value.type->nature == ArrayType 
			?  (yyt->ArrayElemLvalue.Lvalue->Lvalue.attributes).value.type->info.array.range.boundingType 
			: ((yyt->ArrayElemLvalue.Lvalue->Lvalue.attributes).value.type->nature == ArrayedType 
				? (yyt->ArrayElemLvalue.Lvalue->Lvalue.attributes).value.type->info.arrayed.range.boundingType
				: NoType));
yyVisit1 (yyt->ArrayElemLvalue.CoercedExpr);
# line 2068 "Balsa.ag"

		yyt->ArrayElemLvalue.instance = NULL;
		yyt->ArrayElemLvalue.attributes = (yyt->ArrayElemLvalue.skip || ! yyt->ArrayElemLvalue.Lvalue->Lvalue.instance ? NoExprAttributes :
			(yyt->ArrayElemLvalue.Lvalue->Lvalue.instance->nature == VariableInstance
			?	HandleArrayElemLvalue (yyt->ArrayElemLvalue.context, yyt->ArrayElemLvalue.Lvalue->Lvalue.attributes, yyt->ArrayElemLvalue.Lvalue->Lvalue.instance, yyt->ArrayElemLvalue.Lvalue->Lvalue.partitions,
					yyt->ArrayElemLvalue.Lvalue->Lvalue.indices, yyt->ArrayElemLvalue.Lvalue->Lvalue.indexWire, yyt->ArrayElemLvalue.CoercedExpr->CoercedExpr.attributes, &yyt->ArrayElemLvalue.instance, &yyt->ArrayElemLvalue.partitions, &yyt->ArrayElemLvalue.indices,
					&yyt->ArrayElemLvalue.indexWire, yyt->ArrayElemLvalue.Lvalue->Lvalue.access, yyt->ArrayElemLvalue.position)
			:	HandleArrayedElemChannel (yyt->ArrayElemLvalue.context, yyt->ArrayElemLvalue.Lvalue->Lvalue.attributes, yyt->ArrayElemLvalue.Lvalue->Lvalue.instance, yyt->ArrayElemLvalue.CoercedExpr->CoercedExpr.attributes,
					&yyt->ArrayElemLvalue.instance, yyt->ArrayElemLvalue.isInput, yyt->ArrayElemLvalue.isPassive, yyt->ArrayElemLvalue.position)
			));
	
# line 2067 "Balsa.ag"

	yyt->ArrayElemLvalue.access = yyt->ArrayElemLvalue.Lvalue->Lvalue.access;
break;

case kArraySliceLvalue:
yyt->ArraySliceLvalue.Lvalue->Lvalue.context=yyt->ArraySliceLvalue.context;
yyt->ArraySliceLvalue.Lvalue->Lvalue.expectedBaseType=yyt->ArraySliceLvalue.expectedBaseType;
yyt->ArraySliceLvalue.Lvalue->Lvalue.lockPassiveChannels=yyt->ArraySliceLvalue.lockPassiveChannels;
yyt->ArraySliceLvalue.Lvalue->Lvalue.isPassive=yyt->ArraySliceLvalue.isPassive;
yyt->ArraySliceLvalue.Lvalue->Lvalue.isInput=yyt->ArraySliceLvalue.isInput;
yyt->ArraySliceLvalue.Lvalue->Lvalue.expectingEither=yyt->ArraySliceLvalue.expectingEither;
yyt->ArraySliceLvalue.Lvalue->Lvalue.expectingChannel=yyt->ArraySliceLvalue.expectingChannel;
yyt->ArraySliceLvalue.Lvalue->Lvalue.skip=yyt->ArraySliceLvalue.skip;
yyVisit1 (yyt->ArraySliceLvalue.Lvalue);
# line 2081 "Balsa.ag"
 
	yyt->ArraySliceLvalue.Range->Range.isSpan = false;
yyt->ArraySliceLvalue.Range->Range.skip=yyt->ArraySliceLvalue.skip;
yyt->ArraySliceLvalue.Range->Range.context=yyt->ArraySliceLvalue.context;
# line 2082 "Balsa.ag"
 
	yyt->ArraySliceLvalue.Range->Range.expectedType = 
		((yyt->ArraySliceLvalue.Lvalue->Lvalue.attributes).value.type->nature == ArrayType 
			?  (yyt->ArraySliceLvalue.Lvalue->Lvalue.attributes).value.type->info.array.range.boundingType 
			: ((yyt->ArraySliceLvalue.Lvalue->Lvalue.attributes).value.type->nature == ArrayedType 
				? (yyt->ArraySliceLvalue.Lvalue->Lvalue.attributes).value.type->info.arrayed.range.boundingType
				: NoType));
yyVisit1 (yyt->ArraySliceLvalue.Range);
# line 2089 "Balsa.ag"

		yyt->ArraySliceLvalue.instance = NULL;

		if (! yyt->ArraySliceLvalue.skip && yyt->ArraySliceLvalue.Lvalue->Lvalue.instance)
		{
			if (yyt->ArraySliceLvalue.Lvalue->Lvalue.instance->nature == VariableInstance)
				LOG_ERROR (CannotSliceVariableLvalue, NoIdent, yyt->ArraySliceLvalue.position);
			else {
				yyt->ArraySliceLvalue.attributes = HandleArrayedSliceChannel (yyt->ArraySliceLvalue.context, yyt->ArraySliceLvalue.Lvalue->Lvalue.attributes, yyt->ArraySliceLvalue.Lvalue->Lvalue.instance,
					yyt->ArraySliceLvalue.Range->Range.range, &yyt->ArraySliceLvalue.instance, yyt->ArraySliceLvalue.isInput, yyt->ArraySliceLvalue.isPassive, yyt->ArraySliceLvalue.position);
			}
		}
	
# line 2088 "Balsa.ag"

	yyt->ArraySliceLvalue.access = yyt->ArraySliceLvalue.Lvalue->Lvalue.access;
break;

case kArrayAppendLvalue:
yyt->ArrayAppendLvalue.Left->Lvalue.context=yyt->ArrayAppendLvalue.context;
yyt->ArrayAppendLvalue.Left->Lvalue.expectedBaseType=yyt->ArrayAppendLvalue.expectedBaseType;
yyt->ArrayAppendLvalue.Left->Lvalue.lockPassiveChannels=yyt->ArrayAppendLvalue.lockPassiveChannels;
yyt->ArrayAppendLvalue.Left->Lvalue.isPassive=yyt->ArrayAppendLvalue.isPassive;
yyt->ArrayAppendLvalue.Left->Lvalue.isInput=yyt->ArrayAppendLvalue.isInput;
yyt->ArrayAppendLvalue.Left->Lvalue.expectingEither=yyt->ArrayAppendLvalue.expectingEither;
yyt->ArrayAppendLvalue.Left->Lvalue.expectingChannel=yyt->ArrayAppendLvalue.expectingChannel;
yyt->ArrayAppendLvalue.Left->Lvalue.skip=yyt->ArrayAppendLvalue.skip;
yyVisit1 (yyt->ArrayAppendLvalue.Left);
yyt->ArrayAppendLvalue.Right->Lvalue.expectedBaseType=yyt->ArrayAppendLvalue.expectedBaseType;
yyt->ArrayAppendLvalue.Right->Lvalue.lockPassiveChannels=yyt->ArrayAppendLvalue.lockPassiveChannels;
yyt->ArrayAppendLvalue.Right->Lvalue.isPassive=yyt->ArrayAppendLvalue.isPassive;
yyt->ArrayAppendLvalue.Right->Lvalue.isInput=yyt->ArrayAppendLvalue.isInput;
yyt->ArrayAppendLvalue.Right->Lvalue.expectingEither=yyt->ArrayAppendLvalue.expectingEither;
yyt->ArrayAppendLvalue.Right->Lvalue.expectingChannel=yyt->ArrayAppendLvalue.expectingChannel;
yyt->ArrayAppendLvalue.Right->Lvalue.skip=yyt->ArrayAppendLvalue.skip;
yyt->ArrayAppendLvalue.Right->Lvalue.context=yyt->ArrayAppendLvalue.context;
yyVisit1 (yyt->ArrayAppendLvalue.Right);
# line 2105 "Balsa.ag"

		yyt->ArrayAppendLvalue.instance = NULL;

		if (! yyt->ArrayAppendLvalue.skip && yyt->ArrayAppendLvalue.Left->Lvalue.instance && yyt->ArrayAppendLvalue.Right->Lvalue.instance)
		{
			if (yyt->ArrayAppendLvalue.Left->Lvalue.instance->nature == VariableInstance ||
				yyt->ArrayAppendLvalue.Right->Lvalue.instance->nature == VariableInstance)
				LOG_ERROR (CannotAppendVariableLvalues, NoIdent, yyt->ArrayAppendLvalue.position);
			else {
				yyt->ArrayAppendLvalue.attributes = HandleArrayedAppendChannels (yyt->ArrayAppendLvalue.context, yyt->ArrayAppendLvalue.Left->Lvalue.attributes, yyt->ArrayAppendLvalue.Left->Lvalue.instance,
					yyt->ArrayAppendLvalue.Right->Lvalue.attributes, yyt->ArrayAppendLvalue.Right->Lvalue.instance,
					&yyt->ArrayAppendLvalue.instance, yyt->ArrayAppendLvalue.isInput, yyt->ArrayAppendLvalue.isPassive, yyt->ArrayAppendLvalue.position);
			}
		}
	
# line 2104 "Balsa.ag"
 
	yyt->ArrayAppendLvalue.access = yyt->ArrayAppendLvalue.Left->Lvalue.access;
break;

case kArrayConsLvalue:
yyt->ArrayConsLvalue.LvalueLists->LvalueLists.context=yyt->ArrayConsLvalue.context;
# line 2159 "Balsa.ag"

	yyt->ArrayConsLvalue.LvalueLists->LvalueLists.allowArrayedChannels = false;
# line 2160 "Balsa.ag"

	yyt->ArrayConsLvalue.LvalueLists->LvalueLists.isProcedurePorts = true;
yyt->ArrayConsLvalue.LvalueLists->LvalueLists.lockPassiveChannels=yyt->ArrayConsLvalue.lockPassiveChannels;
yyt->ArrayConsLvalue.LvalueLists->LvalueLists.isPassive=yyt->ArrayConsLvalue.isPassive;
yyt->ArrayConsLvalue.LvalueLists->LvalueLists.isInput=yyt->ArrayConsLvalue.isInput;
# line 2161 "Balsa.ag"
 
	yyt->ArrayConsLvalue.LvalueLists->LvalueLists.elements = NULL;
# line 2163 "Balsa.ag"

	
	yyt->ArrayConsLvalue.LvalueLists->LvalueLists.expectedType = yyt->ArrayConsLvalue.expectedBaseType;
yyt->ArrayConsLvalue.LvalueLists->LvalueLists.skip=yyt->ArrayConsLvalue.skip;
yyVisit1 (yyt->ArrayConsLvalue.LvalueLists);
# line 2141 "Balsa.ag"

		yyt->ArrayConsLvalue.attributes = NoExprAttributes; 
		yyt->ArrayConsLvalue.instance = NULL;

		if (!yyt->ArrayConsLvalue.skip)
		{
			if (! (yyt->ArrayConsLvalue.expectingEither || yyt->ArrayConsLvalue.expectingChannel))
				LOG_ERROR (CannotConsVariableLvalues, NoIdent, yyt->ArrayConsLvalue.position);
			else if (yyt->ArrayConsLvalue.LvalueLists->LvalueLists.attributes)
			{
				if (yyt->ArrayConsLvalue.LvalueLists->LvalueLists.expectedType != NoType)
				{ 
					yyt->ArrayConsLvalue.attributes = HandleArrayedConsChannels (yyt->ArrayConsLvalue.context, yyt->ArrayConsLvalue.LvalueLists->LvalueLists.expectedType,
						yyt->ArrayConsLvalue.LvalueLists->LvalueLists.attributes, &yyt->ArrayConsLvalue.instance, yyt->ArrayConsLvalue.isInput, yyt->ArrayConsLvalue.isPassive, yyt->ArrayConsLvalue.position);
				}
			}
		}
	
# line 2021 "Balsa.ag"
 yyt->ArrayConsLvalue.access = NULL;
break;

case kAsLvalue:
yyt->AsLvalue.Lvalue->Lvalue.context=yyt->AsLvalue.context;
yyt->AsLvalue.Lvalue->Lvalue.expectedBaseType=yyt->AsLvalue.expectedBaseType;
yyt->AsLvalue.Lvalue->Lvalue.lockPassiveChannels=yyt->AsLvalue.lockPassiveChannels;
yyt->AsLvalue.Lvalue->Lvalue.isPassive=yyt->AsLvalue.isPassive;
yyt->AsLvalue.Lvalue->Lvalue.isInput=yyt->AsLvalue.isInput;
yyt->AsLvalue.Lvalue->Lvalue.expectingEither=yyt->AsLvalue.expectingEither;
yyt->AsLvalue.Lvalue->Lvalue.expectingChannel=yyt->AsLvalue.expectingChannel;
yyt->AsLvalue.Lvalue->Lvalue.skip=yyt->AsLvalue.skip;
yyVisit1 (yyt->AsLvalue.Lvalue);
yyt->AsLvalue.AType->AType.skip=yyt->AsLvalue.skip;
yyt->AsLvalue.AType->AType.context=yyt->AsLvalue.context;
yyVisit1 (yyt->AsLvalue.AType);
# line 2172 "Balsa.ag"

		yyt->AsLvalue.attributes = NoExprAttributes;

		if (!yyt->AsLvalue.skip)
		{
			if (yyt->AsLvalue.Lvalue->Lvalue.attributes.value.type != NoType && yyt->AsLvalue.AType->AType.type != NoType)
			{
				if (ABS (yyt->AsLvalue.AType->AType.type->size) != ABS (yyt->AsLvalue.Lvalue->Lvalue.attributes.value.type->size))
    				LOG_ERROR (WrongWidthInAsLvalue, NoIdent, yyt->AsLvalue.position);
    			else {
					yyt->AsLvalue.attributes = yyt->AsLvalue.Lvalue->Lvalue.attributes;
					yyt->AsLvalue.attributes.value.type = yyt->AsLvalue.AType->AType.type;
				}
			}
		}
	
# line 2167 "Balsa.ag"

	yyt->AsLvalue.access = yyt->AsLvalue.Lvalue->Lvalue.access;
# line 2168 "Balsa.ag"

	yyt->AsLvalue.indexWire = yyt->AsLvalue.Lvalue->Lvalue.indexWire;
# line 2166 "Balsa.ag"

	yyt->AsLvalue.indices = yyt->AsLvalue.Lvalue->Lvalue.indices;
# line 2169 "Balsa.ag"

	yyt->AsLvalue.partitions = yyt->AsLvalue.Lvalue->Lvalue.partitions;
# line 2170 "Balsa.ag"

	yyt->AsLvalue.instance = yyt->AsLvalue.Lvalue->Lvalue.instance;
break;

case kBitArrayCastLvalue:
yyt->BitArrayCastLvalue.Lvalue->Lvalue.context=yyt->BitArrayCastLvalue.context;
yyt->BitArrayCastLvalue.Lvalue->Lvalue.expectedBaseType=yyt->BitArrayCastLvalue.expectedBaseType;
yyt->BitArrayCastLvalue.Lvalue->Lvalue.lockPassiveChannels=yyt->BitArrayCastLvalue.lockPassiveChannels;
yyt->BitArrayCastLvalue.Lvalue->Lvalue.isPassive=yyt->BitArrayCastLvalue.isPassive;
yyt->BitArrayCastLvalue.Lvalue->Lvalue.isInput=yyt->BitArrayCastLvalue.isInput;
yyt->BitArrayCastLvalue.Lvalue->Lvalue.expectingEither=yyt->BitArrayCastLvalue.expectingEither;
yyt->BitArrayCastLvalue.Lvalue->Lvalue.expectingChannel=yyt->BitArrayCastLvalue.expectingChannel;
yyt->BitArrayCastLvalue.Lvalue->Lvalue.skip=yyt->BitArrayCastLvalue.skip;
yyVisit1 (yyt->BitArrayCastLvalue.Lvalue);
# line 2196 "Balsa.ag"

		yyt->BitArrayCastLvalue.attributes = NoExprAttributes;

		if (!yyt->BitArrayCastLvalue.skip)
		{
			if (yyt->BitArrayCastLvalue.Lvalue->Lvalue.attributes.value.type != NoType)
			{
				Bits typeWidth = Abs (yyt->BitArrayCastLvalue.Lvalue->Lvalue.attributes.value.type->size);
				PtrMP_INT typeWidthMinus1 = NewMP_INT (typeWidth - 1);
				
				yyt->BitArrayCastLvalue.attributes = yyt->BitArrayCastLvalue.Lvalue->Lvalue.attributes;
				yyt->BitArrayCastLvalue.attributes.value.type = NewArrayType (MarkerIdent, BitTypeObj, NewSpan (NewMP_INT (0),
					typeWidthMinus1, NewNumericType (MarkerIdent, SmallestRangeToHoldValue (typeWidthMinus1))));
			}
		}
	
# line 2191 "Balsa.ag"

	yyt->BitArrayCastLvalue.access = yyt->BitArrayCastLvalue.Lvalue->Lvalue.access;
# line 2192 "Balsa.ag"

	yyt->BitArrayCastLvalue.indexWire = yyt->BitArrayCastLvalue.Lvalue->Lvalue.indexWire;
# line 2190 "Balsa.ag"

	yyt->BitArrayCastLvalue.indices = yyt->BitArrayCastLvalue.Lvalue->Lvalue.indices;
# line 2193 "Balsa.ag"

	yyt->BitArrayCastLvalue.partitions = yyt->BitArrayCastLvalue.Lvalue->Lvalue.partitions;
# line 2194 "Balsa.ag"

	yyt->BitArrayCastLvalue.instance = yyt->BitArrayCastLvalue.Lvalue->Lvalue.instance;
break;

case kGuards:
# line 2357 "Balsa.ag"

	yyt->Guards.exprAttributes = NULL;
# line 2356 "Balsa.ag"
 
	yyt->Guards.commandAttributes = NULL;
break;

case kNullGuards:
# line 2357 "Balsa.ag"
	yyt->NullGuards.exprAttributes = NULL;
# line 2356 "Balsa.ag"
 
	yyt->NullGuards.commandAttributes = NULL;
break;

case kGuard:
yyt->Guard.next->Guards.context=yyt->Guard.context;
yyt->Guard.Expr->Expr.context=yyt->Guard.context;
# line 2360 "Balsa.ag"
 
	yyt->Guard.Expr->Expr.allowChannels = false;
# line 2361 "Balsa.ag"

	yyt->Guard.Expr->Expr.expectedType = BitTypeObj;
yyt->Guard.Expr->Expr.skip=yyt->Guard.skip;
yyVisit1 (yyt->Guard.Expr);
# line 2363 "Balsa.ag"
 
		yyt->Guard.Command->Command.skip = yyt->Guard.skip;
		yyt->Guard.next->Guards.skip = yyt->Guard.skip;
	
		if (! yyt->Guard.skip)
		{
			switch (DetermineGuardNature (yyt->Guard.Expr->Expr.attributes, true, false ,
				yyt->Guard.position))
			{
				case TrueGuardNature:
					yyt->Guard.next->Guards.skip = true;
					break;
				case FalseGuardNature:
					yyt->Guard.Command->Command.skip = true;
					break;
				case NonConstantGuardNature:
					break;
			}
		}
	
yyt->Guard.Command->Command.context=yyt->Guard.context;
yyVisit1 (yyt->Guard.Command);
yyVisit1 (yyt->Guard.next);
# line 2387 "Balsa.ag"

		yyt->Guard.commandAttributes = yyt->Guard.next->Guards.commandAttributes;
		yyt->Guard.exprAttributes = yyt->Guard.next->Guards.exprAttributes;

		if (! yyt->Guard.Command->Command.skip)
		{
			yyt->Guard.commandAttributes = NewCommandAttributesList (yyt->Guard.Command->Command.attributes, yyt->Guard.commandAttributes);
			yyt->Guard.exprAttributes = NewExprAttributesList (yyt->Guard.Expr->Expr.attributes, yyt->Guard.exprAttributes);
		}
	
break;

case kPortGuards:
# line 2401 "Balsa.ag"

	yyt->PortGuards.ports = NULL;
# line 2404 "Balsa.ag"

	yyt->PortGuards.hasParameters = false;
# line 2402 "Balsa.ag"
 
	yyt->PortGuards.extraPortInstances = NULL;
# line 2403 "Balsa.ag"

	yyt->PortGuards.portCount = 0;
break;

case kNullPortGuards:
# line 2401 "Balsa.ag"
	yyt->NullPortGuards.ports = NULL;
# line 2404 "Balsa.ag"
	yyt->NullPortGuards.hasParameters = false;
# line 2402 "Balsa.ag"
	yyt->NullPortGuards.extraPortInstances = NULL;
# line 2403 "Balsa.ag"

	yyt->NullPortGuards.portCount = 0;
break;

case kPortGuard:
yyt->PortGuard.next->PortGuards.context=yyt->PortGuard.context;
yyt->PortGuard.next->PortGuards.portsType=yyt->PortGuard.portsType;
# line 2408 "Balsa.ag"

	yyt->PortGuard.Expr->Expr.allowChannels = false;
# line 2407 "Balsa.ag"

	yyt->PortGuard.Expr->Expr.expectedType = BitTypeObj;
yyt->PortGuard.Expr->Expr.skip=yyt->PortGuard.skip;
yyt->PortGuard.Expr->Expr.context=yyt->PortGuard.context;
yyVisit1 (yyt->PortGuard.Expr);
# line 2411 "Balsa.ag"
 
		yyt->PortGuard.FormalPorts->FormalPorts.skip = yyt->PortGuard.skip;
		yyt->PortGuard.next->PortGuards.skip = yyt->PortGuard.skip;

		if (! yyt->PortGuard.skip)
		{
			switch (DetermineGuardNature (yyt->PortGuard.Expr->Expr.attributes, true, true ,
				yyt->PortGuard.position))
			{
				case TrueGuardNature:
					yyt->PortGuard.next->PortGuards.skip = true;
					break;
				case FalseGuardNature:
					yyt->PortGuard.FormalPorts->FormalPorts.skip = true;
					break;
				case NonConstantGuardNature:
					break;
			}
		}
	
yyVisit1 (yyt->PortGuard.next);
# line 2410 "Balsa.ag"


	yyt->PortGuard.FormalPorts->FormalPorts.inConditionalPorts = true;
yyt->PortGuard.FormalPorts->FormalPorts.portsType=yyt->PortGuard.portsType;
yyt->PortGuard.FormalPorts->FormalPorts.context=yyt->PortGuard.context;
yyVisit1 (yyt->PortGuard.FormalPorts);
# line 2432 "Balsa.ag"

		yyt->PortGuard.ports = yyt->PortGuard.next->PortGuards.ports;
		yyt->PortGuard.portCount = yyt->PortGuard.next->PortGuards.portCount;
		yyt->PortGuard.extraPortInstances = yyt->PortGuard.next->PortGuards.extraPortInstances;
		yyt->PortGuard.hasParameters = yyt->PortGuard.next->PortGuards.hasParameters;

		if (yyt->PortGuard.portsType == FunctionArgs)
			LOG_ERROR (FunctionsCantHaveConditionalArguments, NoIdent, yyt->PortGuard.position);
		else if (! yyt->PortGuard.FormalPorts->FormalPorts.skip)
		{
			yyt->PortGuard.portCount = yyt->PortGuard.FormalPorts->FormalPorts.portCount;
			yyt->PortGuard.extraPortInstances = yyt->PortGuard.FormalPorts->FormalPorts.extraPortInstances;
			yyt->PortGuard.ports = yyt->PortGuard.FormalPorts->FormalPorts.ports;
			yyt->PortGuard.hasParameters = yyt->PortGuard.FormalPorts->FormalPorts.hasParameters;
		}
	
break;

case kDeclGuards:
# line 2452 "Balsa.ag"

	yyt->DeclGuards.foundTrueGuard = false;
yyt->DeclGuards.contextOut=yyt->DeclGuards.contextIn;
break;

case kNullDeclGuards:
# line 2452 "Balsa.ag"

	yyt->NullDeclGuards.foundTrueGuard = false;
yyt->NullDeclGuards.contextOut=yyt->NullDeclGuards.contextIn;
break;

case kDeclGuard:
# line 2452 "Balsa.ag"

	yyt->DeclGuard.foundTrueGuard = false;
# line 2456 "Balsa.ag"

	yyt->DeclGuard.Expr->Expr.allowChannels = false;
# line 2455 "Balsa.ag"

	yyt->DeclGuard.Expr->Expr.expectedType = BitTypeObj;
yyt->DeclGuard.Expr->Expr.skip=yyt->DeclGuard.skip;
# line 2457 "Balsa.ag"

	yyt->DeclGuard.Expr->Expr.context = yyt->DeclGuard.contextIn;
yyVisit1 (yyt->DeclGuard.Expr);
# line 2460 "Balsa.ag"
 
		yyt->DeclGuard.Decls->Decls.skip = yyt->DeclGuard.skip;
		yyt->DeclGuard.next->DeclGuards.skip = yyt->DeclGuard.skip;

		if (! yyt->DeclGuard.skip)
		{
			switch (DetermineGuardNature (yyt->DeclGuard.Expr->Expr.attributes, true, true ,
				yyt->DeclGuard.position))
			{
				case TrueGuardNature:
					yyt->DeclGuard.next->DeclGuards.skip = true;
					break;
				case FalseGuardNature:
					yyt->DeclGuard.Decls->Decls.skip = true;
					break;
				case NonConstantGuardNature:
					break;
			}
		}
	
# line 2458 "Balsa.ag"

	yyt->DeclGuard.Decls->Decls.contextIn = yyt->DeclGuard.contextIn;
yyt->DeclGuard.Decls->Decls.scope=yyt->DeclGuard.scope;
yyVisit1 (yyt->DeclGuard.Decls);
yyt->DeclGuard.next->DeclGuards.contextIn=yyt->DeclGuard.contextIn;
yyt->DeclGuard.next->DeclGuards.scope=yyt->DeclGuard.scope;
yyVisit1 (yyt->DeclGuard.next);
# line 2481 "Balsa.ag"

		yyt->DeclGuard.contextOut = yyt->DeclGuard.next->DeclGuards.contextOut;
		yyt->DeclGuard.foundTrueGuard = yyt->DeclGuard.next->DeclGuards.foundTrueGuard;

		if (! yyt->DeclGuard.Decls->Decls.skip)
		{
			yyt->DeclGuard.contextOut = yyt->DeclGuard.Decls->Decls.contextOut;
			yyt->DeclGuard.foundTrueGuard = true;
		}
	
break;

case kCaseGuards:
# line 2498 "Balsa.ag"

		yyt->CaseGuards.complementImplicants = NULL;
		yyt->CaseGuards.implicantMask = NewMP_INT (-1);

		if (! yyt->CaseGuards.skip)
		{ 
			yyt->CaseGuards.implicantMask = MakeMaskForRange (Abs (yyt->CaseGuards.maxRange), 0);
			yyt->CaseGuards.complementImplicants = NewImplicantList (NewImplicant (NewMP_INT (0), yyt->CaseGuards.implicantMask), NULL );
		}
	
# line 2497 "Balsa.ag"
 
	yyt->CaseGuards.foundTrueGuard = false;
# line 2496 "Balsa.ag"

	yyt->CaseGuards.implicantss = NULL;
# line 2495 "Balsa.ag"
 
	yyt->CaseGuards.commandAttributes = NULL;
break;

case kNullCaseGuards:
# line 2498 "Balsa.ag"
		yyt->NullCaseGuards.complementImplicants = NULL;
		yyt->NullCaseGuards.implicantMask = NewMP_INT (-1);

		if (! yyt->NullCaseGuards.skip)
		{ 
			yyt->NullCaseGuards.implicantMask = MakeMaskForRange (Abs (yyt->NullCaseGuards.maxRange), 0);
			yyt->NullCaseGuards.complementImplicants = NewImplicantList (NewImplicant (NewMP_INT (0), yyt->NullCaseGuards.implicantMask), NULL );
		}
	
# line 2497 "Balsa.ag"
	yyt->NullCaseGuards.foundTrueGuard = false;
# line 2496 "Balsa.ag"
	yyt->NullCaseGuards.implicantss = NULL;
# line 2495 "Balsa.ag"
 
	yyt->NullCaseGuards.commandAttributes = NULL;
break;

case kCaseGuard:
# line 2498 "Balsa.ag"
		yyt->CaseGuard.complementImplicants = NULL;
		yyt->CaseGuard.implicantMask = NewMP_INT (-1);

		if (! yyt->CaseGuard.skip)
		{ 
			yyt->CaseGuard.implicantMask = MakeMaskForRange (Abs (yyt->CaseGuard.maxRange), 0);
			yyt->CaseGuard.complementImplicants = NewImplicantList (NewImplicant (NewMP_INT (0), yyt->CaseGuard.implicantMask), NULL );
		}
	
# line 2497 "Balsa.ag"
	yyt->CaseGuard.foundTrueGuard = false;
# line 2496 "Balsa.ag"
	yyt->CaseGuard.implicantss = NULL;
# line 2495 "Balsa.ag"
 
	yyt->CaseGuard.commandAttributes = NULL;
yyt->CaseGuard.next->CaseGuards.context=yyt->CaseGuard.context;
yyt->CaseGuard.next->CaseGuards.skip=yyt->CaseGuard.skip;
yyt->CaseGuard.next->CaseGuards.switchType=yyt->CaseGuard.switchType;
yyt->CaseGuard.next->CaseGuards.switchValue=yyt->CaseGuard.switchValue;
yyt->CaseGuard.next->CaseGuards.maxRange=yyt->CaseGuard.maxRange;
yyVisit1 (yyt->CaseGuard.next);
break;

case kCaseMatchGuard:
# line 2498 "Balsa.ag"
		yyt->CaseMatchGuard.complementImplicants = NULL;
		yyt->CaseMatchGuard.implicantMask = NewMP_INT (-1);

		if (! yyt->CaseMatchGuard.skip)
		{ 
			yyt->CaseMatchGuard.implicantMask = MakeMaskForRange (Abs (yyt->CaseMatchGuard.maxRange), 0);
			yyt->CaseMatchGuard.complementImplicants = NewImplicantList (NewImplicant (NewMP_INT (0), yyt->CaseMatchGuard.implicantMask), NULL );
		}
	
# line 2512 "Balsa.ag"

	yyt->CaseMatchGuard.CaseMatchs->CaseMatchs.isSpan = true;
# line 2513 "Balsa.ag"
 
	yyt->CaseMatchGuard.CaseMatchs->CaseMatchs.allowOverlappingMatches = true;
# line 2511 "Balsa.ag"
 
	
	yyt->CaseMatchGuard.CaseMatchs->CaseMatchs.expectedType = (yyt->CaseMatchGuard.switchValue && yyt->CaseMatchGuard.switchType->nature == NumericType ? NoType : yyt->CaseMatchGuard.switchType);
yyt->CaseMatchGuard.CaseMatchs->CaseMatchs.skip=yyt->CaseMatchGuard.skip;
yyt->CaseMatchGuard.CaseMatchs->CaseMatchs.context=yyt->CaseMatchGuard.context;
yyVisit1 (yyt->CaseMatchGuard.CaseMatchs);
# line 2536 "Balsa.ag"
 
		yyt->CaseMatchGuard.next->CaseGuards.maxRange = yyt->CaseMatchGuard.maxRange;

		if (! yyt->CaseMatchGuard.skip)
		{
			SignedBits impsRange = FindRangeOfImplicants (yyt->CaseMatchGuard.CaseMatchs->CaseMatchs.implicants);
			yyt->CaseMatchGuard.next->CaseGuards.maxRange = WidestRange (impsRange, yyt->CaseMatchGuard.maxRange);
		}
	
yyt->CaseMatchGuard.next->CaseGuards.switchValue=yyt->CaseMatchGuard.switchValue;
yyt->CaseMatchGuard.next->CaseGuards.switchType=yyt->CaseMatchGuard.switchType;
yyt->CaseMatchGuard.next->CaseGuards.skip=yyt->CaseMatchGuard.skip;
yyt->CaseMatchGuard.next->CaseGuards.context=yyt->CaseMatchGuard.context;
yyVisit1 (yyt->CaseMatchGuard.next);
# line 2515 "Balsa.ag"

		
		yyt->CaseMatchGuard.Command->Command.skip = yyt->CaseMatchGuard.skip;
		yyt->CaseMatchGuard.foundTrueGuard = yyt->CaseMatchGuard.next->CaseGuards.foundTrueGuard;

		if (! yyt->CaseMatchGuard.skip)
		{
			if (! yyt->CaseMatchGuard.CaseMatchs->CaseMatchs.implicants)
				yyt->CaseMatchGuard.Command->Command.skip = true;
					
			else if (yyt->CaseMatchGuard.switchValue)
			{
				
				if (!ValueLiesInImplicantList (yyt->CaseMatchGuard.CaseMatchs->CaseMatchs.implicants, yyt->CaseMatchGuard.switchValue))
					yyt->CaseMatchGuard.Command->Command.skip = true;
				else yyt->CaseMatchGuard.foundTrueGuard = true;
			}
		}
	
# line 2549 "Balsa.ag"

		yyt->CaseMatchGuard.implicantss = yyt->CaseMatchGuard.next->CaseGuards.implicantss;
		yyt->CaseMatchGuard.complementImplicants = yyt->CaseMatchGuard.next->CaseGuards.complementImplicants;

		if (! yyt->CaseMatchGuard.skip)
		{
			
			TrimImplicants (yyt->CaseMatchGuard.CaseMatchs->CaseMatchs.implicants, yyt->CaseMatchGuard.implicantMask);

			if (RemoveImplicantsFromImplicants (yyt->CaseMatchGuard.CaseMatchs->CaseMatchs.implicants, yyt->CaseMatchGuard.next->CaseGuards.complementImplicants, NULL))
				LOG_ERROR (CaseGuardsAreNotDisjoint, NoIdent, yyt->CaseMatchGuard.Command->Command.position);

			yyt->CaseMatchGuard.complementImplicants = RemoveImplicantsFromImplicants (yyt->CaseMatchGuard.next->CaseGuards.complementImplicants,
				yyt->CaseMatchGuard.CaseMatchs->CaseMatchs.implicants, NULL);
		}

		if (! yyt->CaseMatchGuard.Command->Command.skip)
		{
			yyt->CaseMatchGuard.implicantss = NewImplicantListList (yyt->CaseMatchGuard.CaseMatchs->CaseMatchs.implicants, yyt->CaseMatchGuard.next->CaseGuards.implicantss);
		}
	
yyt->CaseMatchGuard.Command->Command.context=yyt->CaseMatchGuard.context;
yyVisit1 (yyt->CaseMatchGuard.Command);
# line 2547 "Balsa.ag"


	
	yyt->CaseMatchGuard.commandAttributes = (yyt->CaseMatchGuard.Command->Command.skip ? yyt->CaseMatchGuard.next->CaseGuards.commandAttributes :
		NewCommandAttributesList (yyt->CaseMatchGuard.Command->Command.attributes, yyt->CaseMatchGuard.next->CaseGuards.commandAttributes));
break;

case kForCaseGuard:
# line 2498 "Balsa.ag"
		yyt->ForCaseGuard.complementImplicants = NULL;
		yyt->ForCaseGuard.implicantMask = NewMP_INT (-1);

		if (! yyt->ForCaseGuard.skip)
		{ 
			yyt->ForCaseGuard.implicantMask = MakeMaskForRange (Abs (yyt->ForCaseGuard.maxRange), 0);
			yyt->ForCaseGuard.complementImplicants = NewImplicantList (NewImplicant (NewMP_INT (0), yyt->ForCaseGuard.implicantMask), NULL );
		}
	
# line 2574 "Balsa.ag"

	yyt->ForCaseGuard.CaseMatchs->CaseMatchs.isSpan = true;
# line 2575 "Balsa.ag"
 
	yyt->ForCaseGuard.CaseMatchs->CaseMatchs.allowOverlappingMatches = false;
# line 2573 "Balsa.ag"
 
	
	yyt->ForCaseGuard.CaseMatchs->CaseMatchs.expectedType = (yyt->ForCaseGuard.switchValue && yyt->ForCaseGuard.switchType->nature == NumericType ? NoType : yyt->ForCaseGuard.switchType);
yyt->ForCaseGuard.CaseMatchs->CaseMatchs.skip=yyt->ForCaseGuard.skip;
yyt->ForCaseGuard.CaseMatchs->CaseMatchs.context=yyt->ForCaseGuard.context;
yyVisit1 (yyt->ForCaseGuard.CaseMatchs);
# line 2598 "Balsa.ag"
 
		yyt->ForCaseGuard.next->CaseGuards.maxRange = yyt->ForCaseGuard.maxRange;

		if (! yyt->ForCaseGuard.skip)
		{
			SignedBits impsRange = FindRangeOfImplicants (yyt->ForCaseGuard.CaseMatchs->CaseMatchs.implicants);
			yyt->ForCaseGuard.next->CaseGuards.maxRange = WidestRange (impsRange, yyt->ForCaseGuard.maxRange);
		}
	
yyt->ForCaseGuard.next->CaseGuards.switchValue=yyt->ForCaseGuard.switchValue;
yyt->ForCaseGuard.next->CaseGuards.switchType=yyt->ForCaseGuard.switchType;
yyt->ForCaseGuard.next->CaseGuards.skip=yyt->ForCaseGuard.skip;
yyt->ForCaseGuard.next->CaseGuards.context=yyt->ForCaseGuard.context;
yyVisit1 (yyt->ForCaseGuard.next);
# line 2577 "Balsa.ag"

		
		yyt->ForCaseGuard.Command->Command.skip = yyt->ForCaseGuard.skip;
		yyt->ForCaseGuard.foundTrueGuard = yyt->ForCaseGuard.next->CaseGuards.foundTrueGuard;

		if (! yyt->ForCaseGuard.skip)
		{
			if (! yyt->ForCaseGuard.CaseMatchs->CaseMatchs.implicants)
				yyt->ForCaseGuard.Command->Command.skip = true;
					
			else if (yyt->ForCaseGuard.switchValue)
			{
				
				if (!ValueLiesInImplicantList (yyt->ForCaseGuard.CaseMatchs->CaseMatchs.implicants, yyt->ForCaseGuard.switchValue))
					yyt->ForCaseGuard.Command->Command.skip = true;
				else yyt->ForCaseGuard.foundTrueGuard = true;
			}
		}
	
# line 2609 "Balsa.ag"

		yyt->ForCaseGuard.iterator = NULL;
		yyt->ForCaseGuard.implicants = yyt->ForCaseGuard.CaseMatchs->CaseMatchs.implicants;
		yyt->ForCaseGuard.countValue = NULL;
		yyt->ForCaseGuard.Command->Command.context = AddContextMarker (yyt->ForCaseGuard.context);
		yyt->ForCaseGuard.errorContextString = "";
		yyt->ForCaseGuard.valueStringPtr = "";
		yyt->ForCaseGuard.lastErrorCount = ErrorCount;

		if (! yyt->ForCaseGuard.Command->Command.skip)
		{
			if (yyt->ForCaseGuard.foundTrueGuard)
			{ 
				yyt->ForCaseGuard.iterator = NewConstantInstance (yyt->ForCaseGuard.ident, 
					NewTypedValue (yyt->ForCaseGuard.switchValue, yyt->ForCaseGuard.switchType));
			} else {
				yyt->ForCaseGuard.iterator = NewConstantInstance (yyt->ForCaseGuard.ident, 
					NewTypedValue (NextMatchFromImplicantList (&yyt->ForCaseGuard.implicants, &yyt->ForCaseGuard.countValue), yyt->ForCaseGuard.switchType));
				yyt->ForCaseGuard.errorContextString = NEW_ARRAY (char, 8192);
				yyt->ForCaseGuard.valueStringPtr = yyt->ForCaseGuard.errorContextString;
		
				sprintf (yyt->ForCaseGuard.valueStringPtr, "in `case for' loop at iteration %s = ", PeekString (yyt->ForCaseGuard.ident));
				yyt->ForCaseGuard.valueStringPtr = EndOfString (yyt->ForCaseGuard.valueStringPtr);
				
				*yyt->ForCaseGuard.valueStringPtr = '\0';
				MakeTypedValueString (yyt->ForCaseGuard.valueStringPtr, NewTypedValue (
					CAR (yyt->ForCaseGuard.CaseMatchs->CaseMatchs.implicants).baseValue,
					(yyt->ForCaseGuard.switchType->nature == NumericType ? NewNumericType (NoIdent, yyt->ForCaseGuard.maxRange)
						: yyt->ForCaseGuard.switchType)));
				PushErrorContext (yyt->ForCaseGuard.errorContextString, yyt->ForCaseGuard.position);
			}
			(yyt->ForCaseGuard.Command->Command.context)->instances = NewInstanceList (yyt->ForCaseGuard.iterator, yyt->ForCaseGuard.context->instances);
		}
	
yyVisit1 (yyt->ForCaseGuard.Command);
# line 2644 "Balsa.ag"

		yyt->ForCaseGuard.commandAttributes = yyt->ForCaseGuard.next->CaseGuards.commandAttributes;
		yyt->ForCaseGuard.implicantss = yyt->ForCaseGuard.next->CaseGuards.implicantss;
		yyt->ForCaseGuard.complementImplicants = yyt->ForCaseGuard.next->CaseGuards.complementImplicants;

		if (! yyt->ForCaseGuard.Command->Command.skip)
		{
			if (yyt->ForCaseGuard.foundTrueGuard)
			{ 
				yyt->ForCaseGuard.commandAttributes = NewCommandAttributesList (yyt->ForCaseGuard.Command->Command.attributes, yyt->ForCaseGuard.next->CaseGuards.commandAttributes);
			} else {
				
				TrimImplicants (yyt->ForCaseGuard.CaseMatchs->CaseMatchs.implicants, yyt->ForCaseGuard.implicantMask);

				
				if (yyt->ForCaseGuard.iterator)
				{ 
					PtrMP_INT i = yyt->ForCaseGuard.iterator->info.constant.value.baseValue; 
					PtrMP_INT originalIteratorValue = CopyMP_INT (i);
					Implicant iImplicant;
					PtrImplicantList iImplicantList;
					PopErrorContext (); 
					iImplicant = NewImplicant (i, NewMP_INT (0));
					iImplicantList = NewImplicantList (iImplicant, NULL);

					if (RemoveImplicantsFromImplicants (iImplicantList, yyt->ForCaseGuard.next->CaseGuards.complementImplicants, NULL))
						LOG_ERROR (CaseGuardsAreNotDisjoint, NoIdent, yyt->ForCaseGuard.Command->Command.position);
					else if (yyt->ForCaseGuard.lastErrorCount != ErrorCount)
						LOG_ERROR (GivingUpOnFor, NoIdent, yyt->ForCaseGuard.position);
					else {
						
						yyt->ForCaseGuard.implicantss = NewImplicantListList (iImplicantList, yyt->ForCaseGuard.next->CaseGuards.implicantss);
						yyt->ForCaseGuard.complementImplicants = RemoveImplicantFromImplicants (yyt->ForCaseGuard.next->CaseGuards.complementImplicants,
							iImplicant, NULL);
						yyt->ForCaseGuard.commandAttributes = NewCommandAttributesList (yyt->ForCaseGuard.Command->Command.attributes, yyt->ForCaseGuard.next->CaseGuards.commandAttributes);

						for (i = NextMatchFromImplicantList (&yyt->ForCaseGuard.implicants, &yyt->ForCaseGuard.countValue); i;
							i = NextMatchFromImplicantList (&yyt->ForCaseGuard.implicants, &yyt->ForCaseGuard.countValue))
						{
							yyt->ForCaseGuard.lastErrorCount = ErrorCount;
							
							yyt->ForCaseGuard.iterator->info.constant.value.baseValue = i;
							*yyt->ForCaseGuard.valueStringPtr = '\0';
							MakeTypedValueString (yyt->ForCaseGuard.valueStringPtr, NewTypedValue (
								i, (yyt->ForCaseGuard.switchType->nature == NumericType ? NewNumericType (NoIdent, yyt->ForCaseGuard.maxRange)
									: yyt->ForCaseGuard.switchType)));
							PushErrorContext (yyt->ForCaseGuard.errorContextString, yyt->ForCaseGuard.position);
							VisitTreeNode  (yyt->ForCaseGuard.Command);
							PopErrorContext ();

							
							if (yyt->ForCaseGuard.Command->Command.attributes.activation)
							{
								iImplicant = NewImplicant (i, NewMP_INT (0));
								iImplicantList = NewImplicantList (iImplicant, NULL);

								if (RemoveImplicantsFromImplicants (iImplicantList, yyt->ForCaseGuard.complementImplicants, NULL))
								{
									LOG_ERROR (CaseGuardsAreNotDisjoint, NoIdent, yyt->ForCaseGuard.Command->Command.position);
									break;
								}
								yyt->ForCaseGuard.implicantss = NewImplicantListList (NewImplicantList (iImplicant, NULL), yyt->ForCaseGuard.implicantss);
								yyt->ForCaseGuard.complementImplicants = RemoveImplicantFromImplicants (yyt->ForCaseGuard.complementImplicants,
									iImplicant, NULL);
								yyt->ForCaseGuard.commandAttributes = NewCommandAttributesList (yyt->ForCaseGuard.Command->Command.attributes, yyt->ForCaseGuard.commandAttributes);
							} else {
								DeleteMP_INT (yyt->ForCaseGuard.iterator->info.constant.value.baseValue);
							}
							if (yyt->ForCaseGuard.lastErrorCount != ErrorCount)
							{
								LOG_ERROR (GivingUpOnFor, NoIdent, yyt->ForCaseGuard.position);
								break;
							}
						}
					}
					FREE_ARRAY (char, 8192, yyt->ForCaseGuard.errorContextString);
					yyt->ForCaseGuard.iterator->info.constant.value.baseValue = originalIteratorValue;
				}
			}
		}
	
break;

case kChannelGuards:
# line 2729 "Balsa.ag"
 yyt->ChannelGuards.guardAttributes = NULL;
# line 2729 "Balsa.ag"
 yyt->ChannelGuards.commandAttributes = NULL;
break;

case kNullChannelGuards:
# line 2729 "Balsa.ag"
 yyt->NullChannelGuards.guardAttributes = NULL;
# line 2729 "Balsa.ag"
 yyt->NullChannelGuards.commandAttributes = NULL;
break;

case kChannelGuard:
yyt->ChannelGuard.next->ChannelGuards.context=yyt->ChannelGuard.context;
yyt->ChannelGuard.next->ChannelGuards.skip=yyt->ChannelGuard.skip;
yyVisit1 (yyt->ChannelGuard.next);
# line 2731 "Balsa.ag"
 
	yyt->ChannelGuard.LvalueLists->LvalueLists.allowArrayedChannels = false;
# line 2737 "Balsa.ag"
 
	yyt->ChannelGuard.LvalueLists->LvalueLists.isProcedurePorts = false;
# line 2734 "Balsa.ag"
 
	yyt->ChannelGuard.LvalueLists->LvalueLists.lockPassiveChannels = true;
# line 2733 "Balsa.ag"
 
	yyt->ChannelGuard.LvalueLists->LvalueLists.isPassive = true;
# line 2732 "Balsa.ag"

	yyt->ChannelGuard.LvalueLists->LvalueLists.isInput = true;
# line 2735 "Balsa.ag"
 
	yyt->ChannelGuard.LvalueLists->LvalueLists.elements = NULL;
# line 2736 "Balsa.ag"
 
	yyt->ChannelGuard.LvalueLists->LvalueLists.expectedType = NoType;
yyt->ChannelGuard.LvalueLists->LvalueLists.skip=yyt->ChannelGuard.skip;
yyt->ChannelGuard.LvalueLists->LvalueLists.context=yyt->ChannelGuard.context;
yyVisit1 (yyt->ChannelGuard.LvalueLists);
# line 2738 "Balsa.ag"

		yyt->ChannelGuard.guardAttributes = (yyt->ChannelGuard.skip ? NULL
			: NewExprAttributesList (
				FlattenDisjointChannelAccessList (yyt->ChannelGuard.LvalueLists->LvalueLists.attributes, yyt->ChannelGuard.position), 
				yyt->ChannelGuard.next->ChannelGuards.guardAttributes));
	
yyt->ChannelGuard.Command->Command.skip=yyt->ChannelGuard.skip;
# line 2744 "Balsa.ag"

	yyt->ChannelGuard.Command->Command.context = (yyt->ChannelGuard.skip ? yyt->ChannelGuard.context
		: NewContext (yyt->ChannelGuard.context->depth + 1, yyt->ChannelGuard.context->types, 
			AppendInstanceLists (MakeReadOnlyVariablesFromInstances (yyt->ChannelGuard.LvalueLists->LvalueLists.attributes,
				yyt->ChannelGuard.LvalueLists->LvalueLists.idents, ChannelPassiveRead), yyt->ChannelGuard.context->instances), yyt->ChannelGuard.context->procedures));
yyVisit1 (yyt->ChannelGuard.Command);
# line 2748 "Balsa.ag"

	yyt->ChannelGuard.commandAttributes = (yyt->ChannelGuard.skip ? NULL
		: NewCommandAttributesList (yyt->ChannelGuard.Command->Command.attributes, yyt->ChannelGuard.next->ChannelGuards.commandAttributes));
break;

case kIdents:
# line 2973 "Balsa.ag"
  yyt->Idents.idents = NULL;
break;

case kNullIdents:
# line 2973 "Balsa.ag"
  yyt->NullIdents.idents = NULL;
break;

case kIdent:
yyVisit1 (yyt->Ident.next);
# line 2974 "Balsa.ag"
 yyt->Ident.idents = NewIdentList (yyt->Ident.ident, yyt->Ident.position, yyt->Ident.next->Idents.idents);
break;

case kExprLists:
# line 2753 "Balsa.ag"
 
	 
	
	if (!yyt->ExprLists.expectedType && yyt->ExprLists.elements) LOG_ERROR (ExprListTooShort, NoIdent, yyt->ExprLists.position); 
	yyt->ExprLists.attributes = NULL; 
break;

case kNullExprLists:
# line 2753 "Balsa.ag"
 
	 
	
	if (!yyt->NullExprLists.expectedType && yyt->NullExprLists.elements) LOG_ERROR (ExprListTooShort, NoIdent, yyt->NullExprLists.position); 
	yyt->NullExprLists.attributes = NULL; 
break;

case kExprList:
yyt->ExprList.next->ExprLists.context=yyt->ExprList.context;
yyt->ExprList.next->ExprLists.skip=yyt->ExprList.skip;
yyt->ExprList.next->ExprLists.expectConstants=yyt->ExprList.expectConstants;
# line 2780 "Balsa.ag"

		if (yyt->ExprList.skip) yyt->ExprList.next->ExprLists.elements = NULL;
		else {
			if (!yyt->ExprList.expectedType) {
				yyt->ExprList.next->ExprLists.elements = CDR (yyt->ExprList.elements); 
				if (!yyt->ExprList.elements) LOG_ERROR (ExprListTooLong, NoIdent, yyt->ExprList.position);
			} 
			else yyt->ExprList.next->ExprLists.elements = NULL;
		}
	
yyt->ExprList.next->ExprLists.expectedType=yyt->ExprList.expectedType;
yyVisit1 (yyt->ExprList.next);
yyt->ExprList.CoercedExpr->CoercedExpr.skip=yyt->ExprList.skip;
# line 2779 "Balsa.ag"

	yyt->ExprList.CoercedExpr->CoercedExpr.allowChannels = false;
# line 2778 "Balsa.ag"

	
	yyt->ExprList.CoercedExpr->CoercedExpr.expectedType = (yyt->ExprList.expectedType ? yyt->ExprList.expectedType : (yyt->ExprList.elements ? CAR(yyt->ExprList.elements)->type : NoType));
yyt->ExprList.CoercedExpr->CoercedExpr.context=yyt->ExprList.context;
yyVisit1 (yyt->ExprList.CoercedExpr);
# line 2763 "Balsa.ag"

		yyt->ExprList.attributes = yyt->ExprList.next->ExprLists.attributes; 

		if (!yyt->ExprList.skip)
		{
			if (yyt->ExprList.CoercedExpr->CoercedExpr.expectedType != NoType &&
				!TypeEquivalence ((yyt->ExprList.CoercedExpr->CoercedExpr.attributes).value.type, yyt->ExprList.CoercedExpr->CoercedExpr.expectedType)) 
				LOG_ERROR (TypeIsNotValid, NoIdent, yyt->ExprList.CoercedExpr->CoercedExpr.position);  
			else if (yyt->ExprList.expectConstants && !(yyt->ExprList.CoercedExpr->CoercedExpr.attributes).value.value.baseValue
				&& yyt->ExprList.CoercedExpr->Kind != kStringExpr)
				LOG_ERROR (ExpressionMustBeConstant, NoIdent, yyt->ExprList.CoercedExpr->CoercedExpr.position);
			else yyt->ExprList.attributes = NewExprAttributesList (yyt->ExprList.CoercedExpr->CoercedExpr.attributes, yyt->ExprList.next->ExprLists.attributes);
		}
	
break;

case kLvalueLists:
# line 2795 "Balsa.ag"
 
		 
		
		if (!yyt->LvalueLists.expectedType && yyt->LvalueLists.elements) LOG_ERROR (ExprListTooShort, NoIdent, yyt->LvalueLists.position); 
		yyt->LvalueLists.attributes = NULL;
	
# line 2802 "Balsa.ag"

	yyt->LvalueLists.idents = NULL;
break;

case kNullLvalueLists:
# line 2795 "Balsa.ag"
		if (!yyt->NullLvalueLists.expectedType && yyt->NullLvalueLists.elements) LOG_ERROR (ExprListTooShort, NoIdent, yyt->NullLvalueLists.position); 
		yyt->NullLvalueLists.attributes = NULL;
	
# line 2802 "Balsa.ag"

	yyt->NullLvalueLists.idents = NULL;
break;

case kLvalueList:
# line 2795 "Balsa.ag"
		if (!yyt->LvalueList.expectedType && yyt->LvalueList.elements) LOG_ERROR (ExprListTooShort, NoIdent, yyt->LvalueList.position); 
		yyt->LvalueList.attributes = NULL;
	
yyt->LvalueList.next->LvalueLists.allowArrayedChannels=yyt->LvalueList.allowArrayedChannels;
yyt->LvalueList.next->LvalueLists.isProcedurePorts=yyt->LvalueList.isProcedurePorts;
yyt->LvalueList.next->LvalueLists.lockPassiveChannels=yyt->LvalueList.lockPassiveChannels;
yyt->LvalueList.next->LvalueLists.isPassive=yyt->LvalueList.isPassive;
yyt->LvalueList.next->LvalueLists.isInput=yyt->LvalueList.isInput;
yyt->LvalueList.next->LvalueLists.elements=yyt->LvalueList.elements;
yyt->LvalueList.next->LvalueLists.expectedType=yyt->LvalueList.expectedType;
yyt->LvalueList.next->LvalueLists.skip=yyt->LvalueList.skip;
yyt->LvalueList.next->LvalueLists.context=yyt->LvalueList.context;
yyVisit1 (yyt->LvalueList.next);
# line 2802 "Balsa.ag"

	yyt->LvalueList.idents = NULL;
break;

case kLinkedBlock:
yyt->LinkedBlock.next->LvalueLists.context=yyt->LinkedBlock.context;
yyt->LinkedBlock.next->LvalueLists.allowArrayedChannels=yyt->LinkedBlock.allowArrayedChannels;
yyt->LinkedBlock.next->LvalueLists.isProcedurePorts=yyt->LinkedBlock.isProcedurePorts;
yyt->LinkedBlock.next->LvalueLists.lockPassiveChannels=yyt->LinkedBlock.lockPassiveChannels;
yyt->LinkedBlock.next->LvalueLists.isPassive=yyt->LinkedBlock.isPassive;
yyt->LinkedBlock.next->LvalueLists.isInput=yyt->LinkedBlock.isInput;
# line 2905 "Balsa.ag"

	yyt->LinkedBlock.next->LvalueLists.elements = CDR (yyt->LinkedBlock.elements);
yyt->LinkedBlock.next->LvalueLists.expectedType=yyt->LinkedBlock.expectedType;
yyt->LinkedBlock.next->LvalueLists.skip=yyt->LinkedBlock.skip;
yyVisit1 (yyt->LinkedBlock.next);
yyt->LinkedBlock.Block->Block.skip=yyt->LinkedBlock.skip;
# line 2881 "Balsa.ag"

	yyt->LinkedBlock.Block->Block.context = AddContextMarker (yyt->LinkedBlock.context);
yyVisit1 (yyt->LinkedBlock.Block);
# line 2882 "Balsa.ag"

		yyt->LinkedBlock.attributes = yyt->LinkedBlock.next->LvalueLists.attributes; 

		if (!yyt->LinkedBlock.skip)
		{
			if (! yyt->LinkedBlock.isProcedurePorts) 
				LOG_ERROR (BlockNotSelectable, NoIdent, yyt->LinkedBlock.position);
			else if (yyt->LinkedBlock.elements && (CAR (yyt->LinkedBlock.elements)->type != SyncTypeObj || 
				CAR (yyt->LinkedBlock.elements)->info.port.sense == PassivePortSense))
				LOG_ERROR (BlockExpectsActiveSyncPort, NoIdent, yyt->LinkedBlock.position);
			else if (yyt->LinkedBlock.Block->Block.attributes.activation)
			{
				ExprAttributes attr;
				attr.components = yyt->LinkedBlock.Block->Block.attributes.components;
				attr.accesses = yyt->LinkedBlock.Block->Block.attributes.accesses;
				attr.wires = yyt->LinkedBlock.Block->Block.attributes.wires;
				attr.activation = yyt->LinkedBlock.Block->Block.attributes.activation;
				attr.value = NoValue;
				attr.value.type = SyncTypeObj;
				yyt->LinkedBlock.attributes = NewExprAttributesList (attr, yyt->LinkedBlock.next->LvalueLists.attributes);
			}
		}
	
# line 2880 "Balsa.ag"

	yyt->LinkedBlock.idents = NewIdentList (NoIdent, yyt->LinkedBlock.position, yyt->LinkedBlock.next->LvalueLists.idents);
break;

case kLinkedChannel:
yyt->LinkedChannel.next->LvalueLists.context=yyt->LinkedChannel.context;
# line 2849 "Balsa.ag"

		yyt->LinkedChannel.Lvalue->Lvalue.isInput = yyt->LinkedChannel.isInput;
		yyt->LinkedChannel.Lvalue->Lvalue.isPassive = yyt->LinkedChannel.isPassive;
		yyt->LinkedChannel.next->LvalueLists.elements = CDR (yyt->LinkedChannel.elements); 
		yyt->LinkedChannel.Lvalue->Lvalue.expectedBaseType = (yyt->LinkedChannel.expectedType && yyt->LinkedChannel.expectedType->nature == ArrayedType
			? yyt->LinkedChannel.expectedType->info.arrayed.baseType : NoType);

		if (!yyt->LinkedChannel.skip)
		{
			if (!yyt->LinkedChannel.expectedType) 
			{
				if (yyt->LinkedChannel.elements)
				{
					ASSERT (CAR (yyt->LinkedChannel.elements));
					yyt->LinkedChannel.Lvalue->Lvalue.expectedBaseType = (CAR (yyt->LinkedChannel.elements)->type->nature == ArrayedType
						? CAR (yyt->LinkedChannel.elements)->type->info.arrayed.baseType
						: (CAR (yyt->LinkedChannel.elements)->type->nature == ArrayType
							? CAR (yyt->LinkedChannel.elements)->type->info.array.baseType
							: NoType));
					yyt->LinkedChannel.Lvalue->Lvalue.isInput = CAR (yyt->LinkedChannel.elements)->nature == InputChannelInstance;
					yyt->LinkedChannel.Lvalue->Lvalue.isPassive = CAR (yyt->LinkedChannel.elements)->info.port.sense == PassivePortSense; 
				} else
				{ 
					yyt->LinkedChannel.Lvalue->Lvalue.isPassive = false; yyt->LinkedChannel.Lvalue->Lvalue.isInput = false; yyt->LinkedChannel.next->LvalueLists.elements = NULL;
					LOG_ERROR (ExprListTooLong, NoIdent, yyt->LinkedChannel.position); 
				}
			}
		}
	
yyt->LinkedChannel.next->LvalueLists.allowArrayedChannels=yyt->LinkedChannel.allowArrayedChannels;
yyt->LinkedChannel.next->LvalueLists.isProcedurePorts=yyt->LinkedChannel.isProcedurePorts;
yyt->LinkedChannel.next->LvalueLists.lockPassiveChannels=yyt->LinkedChannel.lockPassiveChannels;
yyt->LinkedChannel.next->LvalueLists.isPassive=yyt->LinkedChannel.isPassive;
yyt->LinkedChannel.next->LvalueLists.isInput=yyt->LinkedChannel.isInput;
yyt->LinkedChannel.next->LvalueLists.expectedType=yyt->LinkedChannel.expectedType;
yyt->LinkedChannel.next->LvalueLists.skip=yyt->LinkedChannel.skip;
yyVisit1 (yyt->LinkedChannel.next);
yyt->LinkedChannel.Lvalue->Lvalue.lockPassiveChannels=yyt->LinkedChannel.lockPassiveChannels;
# line 2848 "Balsa.ag"
 yyt->LinkedChannel.Lvalue->Lvalue.expectingEither = false;
# line 2848 "Balsa.ag"

	
	yyt->LinkedChannel.Lvalue->Lvalue.expectingChannel = true;
yyt->LinkedChannel.Lvalue->Lvalue.skip=yyt->LinkedChannel.skip;
yyt->LinkedChannel.Lvalue->Lvalue.context=yyt->LinkedChannel.context;
yyVisit1 (yyt->LinkedChannel.Lvalue);
# line 2805 "Balsa.ag"

		yyt->LinkedChannel.attributes = yyt->LinkedChannel.next->LvalueLists.attributes; 
		yyt->LinkedChannel.idents = yyt->LinkedChannel.next->LvalueLists.idents;

		if (!yyt->LinkedChannel.skip)
		{
			if (yyt->LinkedChannel.isProcedurePorts && yyt->LinkedChannel.ident != NoIdent) 
				LOG_ERROR (ChannelRenamingNotAllowedInProcPorts, NoIdent, yyt->LinkedChannel.position);
			else if (yyt->LinkedChannel.Lvalue->Lvalue.instance && ((yyt->LinkedChannel.Lvalue->Lvalue.instance)->nature == VariableInstance || 
				(yyt->LinkedChannel.Lvalue->Lvalue.instance)->nature == ConstantInstance))
				LOG_ERROR (ExpectingAChannel, NoIdent, yyt->LinkedChannel.Lvalue->Lvalue.position);
			
			else if ((yyt->LinkedChannel.elements || (yyt->LinkedChannel.expectedType && yyt->LinkedChannel.expectedType != NoType)) &&
				!TypeEquivalence ((yyt->LinkedChannel.Lvalue->Lvalue.attributes).value.type,
				(yyt->LinkedChannel.elements ? CAR (yyt->LinkedChannel.elements)->type : (yyt->LinkedChannel.expectedType ? yyt->LinkedChannel.expectedType : NoType))))
			{
				LOG_ERROR (TypeIsNotValid, NoIdent, yyt->LinkedChannel.Lvalue->Lvalue.position);  
				
			}
			else if ((yyt->LinkedChannel.Lvalue->Lvalue.attributes).value.type->nature == ArrayedType && !yyt->LinkedChannel.allowArrayedChannels)
				LOG_ERROR (NotExpectingAnArrayedChannel, NoIdent, yyt->LinkedChannel.position);
			else {
				if ((yyt->LinkedChannel.Lvalue->Lvalue.attributes).value.type->nature == ArrayedType)
				{
					if (yyt->LinkedChannel.ident != NoIdent)
						LOG_ERROR (CannotRenameChannelArray, NoIdent, yyt->LinkedChannel.position);
					else { 
						yyt->LinkedChannel.attributes = AppendExprAttributesLists (MakeAttributesListForChannelInstancesAccesses
							(yyt->LinkedChannel.Lvalue->Lvalue.attributes.value.type->info.arrayed.arrayedElements,
							yyt->LinkedChannel.Lvalue->Lvalue.attributes.value.type->info.arrayed.elementCount, yyt->LinkedChannel.Lvalue->Lvalue.isInput,
							yyt->LinkedChannel.Lvalue->Lvalue.isPassive, yyt->LinkedChannel.Lvalue->Lvalue.lockPassiveChannels, yyt->LinkedChannel.position), yyt->LinkedChannel.next->LvalueLists.attributes);
					}
				} else {
					yyt->LinkedChannel.idents = NewIdentList ((yyt->LinkedChannel.ident == NoIdent ?
						(yyt->LinkedChannel.Lvalue->Lvalue.instance == NULL || yyt->LinkedChannel.Lvalue->Lvalue.instance == NoInstance 
							? NoIdent : yyt->LinkedChannel.Lvalue->Lvalue.instance->ident)
						: yyt->LinkedChannel.ident), yyt->LinkedChannel.position, yyt->LinkedChannel.next->LvalueLists.idents);
					yyt->LinkedChannel.attributes = NewExprAttributesList (yyt->LinkedChannel.Lvalue->Lvalue.attributes, yyt->LinkedChannel.next->LvalueLists.attributes);
				}
			}
		}
	
break;

case kCaseMatchs:
# line 2909 "Balsa.ag"
  yyt->CaseMatchs.implicants = NULL;
break;

case kNullCaseMatchs:
# line 2909 "Balsa.ag"
  yyt->NullCaseMatchs.implicants = NULL;
break;

case kCaseMatch:
# line 2909 "Balsa.ag"
  yyt->CaseMatch.implicants = NULL;
yyt->CaseMatch.next->CaseMatchs.isSpan=yyt->CaseMatch.isSpan;
yyt->CaseMatch.next->CaseMatchs.allowOverlappingMatches=yyt->CaseMatch.allowOverlappingMatches;
yyt->CaseMatch.next->CaseMatchs.expectedType=yyt->CaseMatch.expectedType;
yyt->CaseMatch.next->CaseMatchs.skip=yyt->CaseMatch.skip;
yyt->CaseMatch.next->CaseMatchs.context=yyt->CaseMatch.context;
yyVisit1 (yyt->CaseMatch.next);
break;

case kCaseRange:
yyt->CaseRange.next->CaseMatchs.context=yyt->CaseRange.context;
yyt->CaseRange.next->CaseMatchs.skip=yyt->CaseRange.skip;
yyt->CaseRange.next->CaseMatchs.isSpan=yyt->CaseRange.isSpan;
yyt->CaseRange.next->CaseMatchs.allowOverlappingMatches=yyt->CaseRange.allowOverlappingMatches;
yyt->CaseRange.next->CaseMatchs.expectedType=yyt->CaseRange.expectedType;
yyVisit1 (yyt->CaseRange.next);
yyt->CaseRange.Range->Range.isSpan=yyt->CaseRange.isSpan;
yyt->CaseRange.Range->Range.expectedType=yyt->CaseRange.expectedType;
yyt->CaseRange.Range->Range.skip=yyt->CaseRange.skip;
yyt->CaseRange.Range->Range.context=yyt->CaseRange.context;
yyVisit1 (yyt->CaseRange.Range);
# line 2911 "Balsa.ag"

		yyt->CaseRange.implicants = yyt->CaseRange.next->CaseMatchs.implicants;
	
		if (!yyt->CaseRange.skip && yyt->CaseRange.Range->Range.range.lowerBound)
		{
			
			if (yyt->CaseRange.expectedType == NoType && yyt->CaseRange.Range->Range.range.boundingType->nature != NumericType)
				LOG_ERROR (ExpressionMustBeNumeric, NoIdent, yyt->CaseRange.position);
			else {
				PtrImplicantList newImps = MakeSpanImplicantList (yyt->CaseRange.Range->Range.range);
				yyt->CaseRange.implicants = AppendImplicantLists (newImps, yyt->CaseRange.next->CaseMatchs.implicants);

				if (! yyt->CaseRange.allowOverlappingMatches && ! ImplicantsAreDisjoint (yyt->CaseRange.implicants))
				{
					LOG_ERROR (CaseGuardsAreNotDisjoint, NoIdent, yyt->CaseRange.position);
					yyt->CaseRange.implicants = yyt->CaseRange.next->CaseMatchs.implicants;
				}
			}
		}
	
break;

case kCaseImplicant:
yyt->CaseImplicant.next->CaseMatchs.context=yyt->CaseImplicant.context;
yyt->CaseImplicant.next->CaseMatchs.skip=yyt->CaseImplicant.skip;
yyt->CaseImplicant.next->CaseMatchs.isSpan=yyt->CaseImplicant.isSpan;
yyt->CaseImplicant.next->CaseMatchs.allowOverlappingMatches=yyt->CaseImplicant.allowOverlappingMatches;
yyt->CaseImplicant.next->CaseMatchs.expectedType=yyt->CaseImplicant.expectedType;
yyVisit1 (yyt->CaseImplicant.next);
# line 2933 "Balsa.ag"

	yyt->CaseImplicant.Expr->Expr.allowChannels = false;
yyt->CaseImplicant.Expr->Expr.expectedType=yyt->CaseImplicant.expectedType;
yyt->CaseImplicant.Expr->Expr.skip=yyt->CaseImplicant.skip;
yyt->CaseImplicant.Expr->Expr.context=yyt->CaseImplicant.context;
yyVisit1 (yyt->CaseImplicant.Expr);
# line 2934 "Balsa.ag"

		yyt->CaseImplicant.implicants = yyt->CaseImplicant.next->CaseMatchs.implicants;

		if (! yyt->CaseImplicant.skip)	
		{
			if (yyt->CaseImplicant.expectedType != NoType)
			{
				if (!TypeEquivalence (yyt->CaseImplicant.expectedType, yyt->CaseImplicant.Expr->Expr.attributes.value.type))
					LOG_ERROR (TypeIsNotValid, NoIdent, yyt->CaseImplicant.position);
				else yyt->CaseImplicant.implicants = NewImplicantList (yyt->CaseImplicant.Expr->Expr.attributes.value.value, yyt->CaseImplicant.next->CaseMatchs.implicants);
			} else { 
				if (yyt->CaseImplicant.Expr->Expr.attributes.value.type->nature != NumericType)
					LOG_ERROR (ExpressionMustBeNumeric, NoIdent, yyt->CaseImplicant.position);
				if (!yyt->CaseImplicant.Expr->Expr.attributes.value.value.baseValue)
					LOG_ERROR (ExpressionMustBeConstant, NoIdent, yyt->CaseImplicant.position);
				else if (yyt->CaseImplicant.expectedType->size < 0 && yyt->CaseImplicant.Expr->Expr.attributes.value.value.dontCares) 
					LOG_ERROR (ImplicantsOnlyForUNumericCases, NoIdent, yyt->CaseImplicant.position);
				else yyt->CaseImplicant.implicants = NewImplicantList (yyt->CaseImplicant.Expr->Expr.attributes.value.value, yyt->CaseImplicant.next->CaseMatchs.implicants);
			}

			if (! yyt->CaseImplicant.allowOverlappingMatches && ! ImplicantsAreDisjoint (yyt->CaseImplicant.implicants))
			{
				LOG_ERROR (CaseGuardsAreNotDisjoint, NoIdent, yyt->CaseImplicant.position);
				yyt->CaseImplicant.implicants = yyt->CaseImplicant.next->CaseMatchs.implicants;
			}
		}
	
break;

case kEnumElems:
yyt->EnumElems.typeOut=yyt->EnumElems.typeIn;
break;

case kNullEnumElems:
yyt->NullEnumElems.typeOut=yyt->NullEnumElems.typeIn;
break;

case kEnumElem:
yyt->EnumElem.next->EnumElems.context=yyt->EnumElem.context;
yyt->EnumElem.next->EnumElems.skip=yyt->EnumElem.skip;
yyt->EnumElem.next->EnumElems.elementsTail=yyt->EnumElem.elementsTail;
yyt->EnumElem.next->EnumElems.hasOverType=yyt->EnumElem.hasOverType;
yyt->EnumElem.next->EnumElems.typeIn=yyt->EnumElem.typeIn;
yyt->EnumElem.next->EnumElems.elementValue=yyt->EnumElem.elementValue;
yyVisit1 (yyt->EnumElem.next);
yyt->EnumElem.typeOut=yyt->EnumElem.next->EnumElems.typeOut;
break;

case kDefaultValuedEnumElem:
yyt->DefaultValuedEnumElem.next->EnumElems.context=yyt->DefaultValuedEnumElem.context;
yyt->DefaultValuedEnumElem.next->EnumElems.skip=yyt->DefaultValuedEnumElem.skip;
yyt->DefaultValuedEnumElem.next->EnumElems.hasOverType=yyt->DefaultValuedEnumElem.hasOverType;
# line 877 "Balsa.ag"

		yyt->DefaultValuedEnumElem.next->EnumElems.typeIn = NoType;
		if (! yyt->DefaultValuedEnumElem.skip)
		{
			yyt->DefaultValuedEnumElem.next->EnumElems.typeIn = AddEnumElemToEnumType (yyt->DefaultValuedEnumElem.typeIn, yyt->DefaultValuedEnumElem.ident, yyt->DefaultValuedEnumElem.elementValue, 
				yyt->DefaultValuedEnumElem.elementsTail, &(yyt->DefaultValuedEnumElem.next->EnumElems.elementsTail), NoValue,
				yyt->DefaultValuedEnumElem.position, true, yyt->DefaultValuedEnumElem.hasOverType, &(yyt->DefaultValuedEnumElem.next->EnumElems.elementValue), yyt->DefaultValuedEnumElem.position);
		}
	
yyVisit1 (yyt->DefaultValuedEnumElem.next);
yyt->DefaultValuedEnumElem.typeOut=yyt->DefaultValuedEnumElem.next->EnumElems.typeOut;
break;

case kValuedEnumElem:
yyt->ValuedEnumElem.next->EnumElems.context=yyt->ValuedEnumElem.context;
yyt->ValuedEnumElem.next->EnumElems.skip=yyt->ValuedEnumElem.skip;
yyt->ValuedEnumElem.CoercedExpr->CoercedExpr.skip=yyt->ValuedEnumElem.skip;
# line 889 "Balsa.ag"
 
	yyt->ValuedEnumElem.CoercedExpr->CoercedExpr.allowChannels = false;
# line 888 "Balsa.ag"
 
	yyt->ValuedEnumElem.CoercedExpr->CoercedExpr.expectedType = yyt->ValuedEnumElem.typeIn;
yyt->ValuedEnumElem.CoercedExpr->CoercedExpr.context=yyt->ValuedEnumElem.context;
yyVisit1 (yyt->ValuedEnumElem.CoercedExpr);
yyt->ValuedEnumElem.next->EnumElems.hasOverType=yyt->ValuedEnumElem.hasOverType;
# line 890 "Balsa.ag"

		yyt->ValuedEnumElem.next->EnumElems.typeIn = NoType;
		if (! yyt->ValuedEnumElem.skip)
		{
			yyt->ValuedEnumElem.next->EnumElems.typeIn = AddEnumElemToEnumType (yyt->ValuedEnumElem.typeIn, yyt->ValuedEnumElem.ident, yyt->ValuedEnumElem.elementValue, 
				yyt->ValuedEnumElem.elementsTail, &(yyt->ValuedEnumElem.next->EnumElems.elementsTail), (yyt->ValuedEnumElem.CoercedExpr->CoercedExpr.attributes).value, 
				yyt->ValuedEnumElem.CoercedExpr->CoercedExpr.position, false, yyt->ValuedEnumElem.hasOverType, &(yyt->ValuedEnumElem.next->EnumElems.elementValue), yyt->ValuedEnumElem.position);
		}
	
yyVisit1 (yyt->ValuedEnumElem.next);
yyt->ValuedEnumElem.typeOut=yyt->ValuedEnumElem.next->EnumElems.typeOut;
break;

case kRecordElems:
yyt->RecordElems.typeOut=yyt->RecordElems.typeIn;
break;

case kNullRecordElems:
yyt->NullRecordElems.typeOut=yyt->NullRecordElems.typeIn;
break;

case kRecordElem:
yyt->RecordElem.next->RecordElems.context=yyt->RecordElem.context;
yyt->RecordElem.next->RecordElems.skip=yyt->RecordElem.skip;
yyt->RecordElem.AType->AType.skip=yyt->RecordElem.skip;
yyt->RecordElem.AType->AType.context=yyt->RecordElem.context;
yyVisit1 (yyt->RecordElem.AType);
yyVisit1 (yyt->RecordElem.Idents);
yyt->RecordElem.next->RecordElems.hasOverType=yyt->RecordElem.hasOverType;
# line 902 "Balsa.ag"

		yyt->RecordElem.next->RecordElems.typeIn = NoType;
		if (! yyt->RecordElem.skip)
		{
			yyt->RecordElem.next->RecordElems.typeIn = AddRecordElemToRecordType (yyt->RecordElem.typeIn, yyt->RecordElem.Idents->Idents.idents, yyt->RecordElem.AType->AType.type, yyt->RecordElem.hasOverType, 
				yyt->RecordElem.elementsTail, &(yyt->RecordElem.next->RecordElems.elementsTail), yyt->RecordElem.position); 
		}
	
yyVisit1 (yyt->RecordElem.next);
yyt->RecordElem.typeOut=yyt->RecordElem.next->RecordElems.typeOut;
break;

case kValDecls:
yyt->ValDecls.contextOut=yyt->ValDecls.contextIn;
break;

case kNullValDecls:
yyt->NullValDecls.contextOut=yyt->NullValDecls.contextIn;
break;

case kValDecl:
# line 2967 "Balsa.ag"
 yyt->ValDecl.Expr->Expr.context = yyt->ValDecl.contextIn;
# line 2966 "Balsa.ag"

	yyt->ValDecl.Expr->Expr.allowChannels = false;
# line 2967 "Balsa.ag"

	yyt->ValDecl.Expr->Expr.expectedType = NoType;
yyt->ValDecl.Expr->Expr.skip=yyt->ValDecl.skip;
yyVisit1 (yyt->ValDecl.Expr);
# line 2968 "Balsa.ag"

	yyt->ValDecl.next->ValDecls.contextIn = (yyt->ValDecl.skip ? yyt->ValDecl.contextIn : ValDeclaration (yyt->ValDecl.contextIn, yyt->ValDecl.ident, yyt->ValDecl.Expr->Expr.attributes, yyt->ValDecl.position));
yyt->ValDecl.next->ValDecls.skip=yyt->ValDecl.skip;
yyVisit1 (yyt->ValDecl.next);
yyt->ValDecl.contextOut=yyt->ValDecl.next->ValDecls.contextOut;
break;
 default: ;
 }
}

void BeginSemantics ()
{
# line 66 "Balsa.ag"


# line 329 "Balsa.ag"
 
}

void CloseSemantics ()
{
}
