# include "Tree.h"
# define yyALLOC(ptr, size)	if ((ptr = (tTree) Tree_PoolFreePtr) >= (tTree) Tree_PoolMaxPtr) \
  ptr = Tree_Alloc (); \
  Tree_PoolFreePtr += size;
# define yyFREE(ptr, size)	
# ifdef __cplusplus
extern "C" {
# include "rSystem.h"
# include "General.h"
# include "rMemory.h"
# include "DynArray.h"
# include "StringMem.h"
# include "Idents.h"
# include "Sets.h"
# include "Positions.h"
}
# else
# include "rSystem.h"
# include "General.h"
# include "rMemory.h"
# include "DynArray.h"
# include "StringMem.h"
# include "Idents.h"
# include "Sets.h"
# include "Positions.h"
# endif

char Tree_module_does_not_match_evaluator_module_25160;
char generate_Tree_module_without_option_0;

# ifdef getchar
# undef getchar
# endif
# ifdef putchar
# undef putchar
# endif
# include "yyTree.w"

static void yyExit () { rExit (1); }

void (* Tree_Exit) () = yyExit;

# define yyBlockSize 20480

typedef struct yysBlock {
 char yyBlock [yyBlockSize];
 struct yysBlock * yySuccessor;
} yytBlock, * yytBlockPtr;

tTree TreeRoot;
unsigned long Tree_HeapUsed = 0;

static yytBlockPtr yyBlockList	= (yytBlockPtr) NoTree;
char * Tree_PoolFreePtr	= (char *) NoTree;
char * Tree_PoolMaxPtr	= (char *) NoTree;
static unsigned short yyMaxSize	= 0;
unsigned short Tree_NodeSize [182 + 1] = { 0,
 sizeof (yBalsaProgram),
 sizeof (yDecls),
 sizeof (yNullDecls),
 sizeof (yDecl),
 sizeof (yImportDecl),
 sizeof (yFileDecl),
 sizeof (yTypeDecl),
 sizeof (yConstantDecl),
 sizeof (yVariableDecl),
 sizeof (yInitVariableDecl),
 sizeof (yChannelDecl),
 sizeof (yChannelArrayDecl),
 sizeof (ySyncDecl),
 sizeof (ySyncArrayDecl),
 sizeof (yProcedureDecl),
 sizeof (yProcAliasDecl),
 sizeof (yFunctionDecl),
 sizeof (yBuiltinFunctionDecl),
 sizeof (ySharedDecl),
 sizeof (yPartDecl),
 sizeof (yIfDecls),
 sizeof (yIfElseDecls),
 sizeof (yPrintDecl),
 sizeof (yNullDecl),
 sizeof (yRange),
 sizeof (ySpecifiedRange),
 sizeof (yTypeRange),
 sizeof (yAType),
 sizeof (yNullType),
 sizeof (yNumericType),
 sizeof (yExistingType),
 sizeof (yBuiltinType),
 sizeof (yArrayType),
 sizeof (yNewType),
 sizeof (yRecordType),
 sizeof (yEnumType),
 sizeof (yCoercedExpr),
 sizeof (yExpr),
 sizeof (yNullExpr),
 sizeof (yLiteralExpr),
 sizeof (yIdentExpr),
 sizeof (yStringExpr),
 sizeof (yImplicantExpr),
 sizeof (yDontCareExpr),
 sizeof (yAggregateConsExpr),
 sizeof (yNamedEnumElemExpr),
 sizeof (yUnaryExpr),
 sizeof (yBinaryExpr),
 sizeof (yRecordElemExtractExpr),
 sizeof (yArrayExtractExpr),
 sizeof (yArraySliceExpr),
 sizeof (yAsExpr),
 sizeof (yBitArrayCastExpr),
 sizeof (yLetExpr),
 sizeof (yFunctionCallExpr),
 sizeof (ySizeofExpr),
 sizeof (yArrayAppendExpr),
 sizeof (yPrimedExpr),
 sizeof (yFormalPorts),
 sizeof (yNullFormalPorts),
 sizeof (yFormalPort),
 sizeof (yValuePort),
 sizeof (yParamPort),
 sizeof (yTypeParamPort),
 sizeof (yChannelPort),
 sizeof (yChannelPortArray),
 sizeof (ySyncPort),
 sizeof (ySyncPortArray),
 sizeof (yIfPorts),
 sizeof (yIfElsePorts),
 sizeof (yBlock),
 sizeof (yParameters),
 sizeof (yNullParameters),
 sizeof (yParameter),
 sizeof (yNumberParameter),
 sizeof (yStringParameter),
 sizeof (yTypeParameter),
 sizeof (yBreezeParameters),
 sizeof (yNullBreezeParameters),
 sizeof (yBreezeParameter),
 sizeof (yBreezeExprParameter),
 sizeof (yBreezeTypeParameter),
 sizeof (yComps),
 sizeof (yNullComps),
 sizeof (yComp),
 sizeof (yNormalComp),
 sizeof (yUndeclaredComp),
 sizeof (yCommand),
 sizeof (yNullCommand),
 sizeof (yContinueCommand),
 sizeof (yHaltCommand),
 sizeof (yInputCommand),
 sizeof (yInputEncloseCommand),
 sizeof (yInputEncloseBangCommand),
 sizeof (yOutputCommand),
 sizeof (ySyncCommand),
 sizeof (yAssignmentCommand),
 sizeof (yBlockCommand),
 sizeof (ySequentialCommand),
 sizeof (yParallelCommand),
 sizeof (yLoopCommand),
 sizeof (yWhileGuardsCommand),
 sizeof (yWhileGuardsAlsoCommand),
 sizeof (yCommandWhileExprCommand),
 sizeof (yCommandWhileGuardsCommand),
 sizeof (yCommandWhileGuardsAlsoCommand),
 sizeof (yIfCommand),
 sizeof (yIfElseCommand),
 sizeof (yCaseCommand),
 sizeof (yForCommand),
 sizeof (yProcedureCallCommonCommand),
 sizeof (ySelectCommand),
 sizeof (ySelectBangCommand),
 sizeof (yArbitrateCommand),
 sizeof (yPrintCommand),
 sizeof (ySinkCommand),
 sizeof (yProcedureParams),
 sizeof (yNullProcParams),
 sizeof (yProcParam),
 sizeof (yExprProcParam),
 sizeof (yTypeProcParam),
 sizeof (yVarReadProcParam),
 sizeof (yVarWriteProcParam),
 sizeof (yBlockProcParam),
 sizeof (yFunctionParams),
 sizeof (yNullFuncParams),
 sizeof (yFuncParam),
 sizeof (yExprFuncParam),
 sizeof (yTypeFuncParam),
 sizeof (yLvalue),
 sizeof (yIdentLvalue),
 sizeof (yRecordElemLvalue),
 sizeof (yArrayElemLvalue),
 sizeof (yArraySliceLvalue),
 sizeof (yArrayAppendLvalue),
 sizeof (yArrayConsLvalue),
 sizeof (yAsLvalue),
 sizeof (yBitArrayCastLvalue),
 sizeof (yGuards),
 sizeof (yNullGuards),
 sizeof (yGuard),
 sizeof (yPortGuards),
 sizeof (yNullPortGuards),
 sizeof (yPortGuard),
 sizeof (yDeclGuards),
 sizeof (yNullDeclGuards),
 sizeof (yDeclGuard),
 sizeof (yCaseGuards),
 sizeof (yNullCaseGuards),
 sizeof (yCaseGuard),
 sizeof (yCaseMatchGuard),
 sizeof (yForCaseGuard),
 sizeof (yChannelGuards),
 sizeof (yNullChannelGuards),
 sizeof (yChannelGuard),
 sizeof (yIdents),
 sizeof (yNullIdents),
 sizeof (yIdent),
 sizeof (yExprLists),
 sizeof (yNullExprLists),
 sizeof (yExprList),
 sizeof (yLvalueLists),
 sizeof (yNullLvalueLists),
 sizeof (yLvalueList),
 sizeof (yLinkedBlock),
 sizeof (yLinkedChannel),
 sizeof (yCaseMatchs),
 sizeof (yNullCaseMatchs),
 sizeof (yCaseMatch),
 sizeof (yCaseRange),
 sizeof (yCaseImplicant),
 sizeof (yEnumElems),
 sizeof (yNullEnumElems),
 sizeof (yEnumElem),
 sizeof (yDefaultValuedEnumElem),
 sizeof (yValuedEnumElem),
 sizeof (yRecordElems),
 sizeof (yNullRecordElems),
 sizeof (yRecordElem),
 sizeof (yValDecls),
 sizeof (yNullValDecls),
 sizeof (yValDecl),
};
char * Tree_NodeName [182 + 1] = {
 "NoTree",
 "BalsaProgram",
 "Decls",
 "NullDecls",
 "Decl",
 "ImportDecl",
 "FileDecl",
 "TypeDecl",
 "ConstantDecl",
 "VariableDecl",
 "InitVariableDecl",
 "ChannelDecl",
 "ChannelArrayDecl",
 "SyncDecl",
 "SyncArrayDecl",
 "ProcedureDecl",
 "ProcAliasDecl",
 "FunctionDecl",
 "BuiltinFunctionDecl",
 "SharedDecl",
 "PartDecl",
 "IfDecls",
 "IfElseDecls",
 "PrintDecl",
 "NullDecl",
 "Range",
 "SpecifiedRange",
 "TypeRange",
 "AType",
 "NullType",
 "NumericType",
 "ExistingType",
 "BuiltinType",
 "ArrayType",
 "NewType",
 "RecordType",
 "EnumType",
 "CoercedExpr",
 "Expr",
 "NullExpr",
 "LiteralExpr",
 "IdentExpr",
 "StringExpr",
 "ImplicantExpr",
 "DontCareExpr",
 "AggregateConsExpr",
 "NamedEnumElemExpr",
 "UnaryExpr",
 "BinaryExpr",
 "RecordElemExtractExpr",
 "ArrayExtractExpr",
 "ArraySliceExpr",
 "AsExpr",
 "BitArrayCastExpr",
 "LetExpr",
 "FunctionCallExpr",
 "SizeofExpr",
 "ArrayAppendExpr",
 "PrimedExpr",
 "FormalPorts",
 "NullFormalPorts",
 "FormalPort",
 "ValuePort",
 "ParamPort",
 "TypeParamPort",
 "ChannelPort",
 "ChannelPortArray",
 "SyncPort",
 "SyncPortArray",
 "IfPorts",
 "IfElsePorts",
 "Block",
 "Parameters",
 "NullParameters",
 "Parameter",
 "NumberParameter",
 "StringParameter",
 "TypeParameter",
 "BreezeParameters",
 "NullBreezeParameters",
 "BreezeParameter",
 "BreezeExprParameter",
 "BreezeTypeParameter",
 "Comps",
 "NullComps",
 "Comp",
 "NormalComp",
 "UndeclaredComp",
 "Command",
 "NullCommand",
 "ContinueCommand",
 "HaltCommand",
 "InputCommand",
 "InputEncloseCommand",
 "InputEncloseBangCommand",
 "OutputCommand",
 "SyncCommand",
 "AssignmentCommand",
 "BlockCommand",
 "SequentialCommand",
 "ParallelCommand",
 "LoopCommand",
 "WhileGuardsCommand",
 "WhileGuardsAlsoCommand",
 "CommandWhileExprCommand",
 "CommandWhileGuardsCommand",
 "CommandWhileGuardsAlsoCommand",
 "IfCommand",
 "IfElseCommand",
 "CaseCommand",
 "ForCommand",
 "ProcedureCallCommonCommand",
 "SelectCommand",
 "SelectBangCommand",
 "ArbitrateCommand",
 "PrintCommand",
 "SinkCommand",
 "ProcedureParams",
 "NullProcParams",
 "ProcParam",
 "ExprProcParam",
 "TypeProcParam",
 "VarReadProcParam",
 "VarWriteProcParam",
 "BlockProcParam",
 "FunctionParams",
 "NullFuncParams",
 "FuncParam",
 "ExprFuncParam",
 "TypeFuncParam",
 "Lvalue",
 "IdentLvalue",
 "RecordElemLvalue",
 "ArrayElemLvalue",
 "ArraySliceLvalue",
 "ArrayAppendLvalue",
 "ArrayConsLvalue",
 "AsLvalue",
 "BitArrayCastLvalue",
 "Guards",
 "NullGuards",
 "Guard",
 "PortGuards",
 "NullPortGuards",
 "PortGuard",
 "DeclGuards",
 "NullDeclGuards",
 "DeclGuard",
 "CaseGuards",
 "NullCaseGuards",
 "CaseGuard",
 "CaseMatchGuard",
 "ForCaseGuard",
 "ChannelGuards",
 "NullChannelGuards",
 "ChannelGuard",
 "Idents",
 "NullIdents",
 "Ident",
 "ExprLists",
 "NullExprLists",
 "ExprList",
 "LvalueLists",
 "NullLvalueLists",
 "LvalueList",
 "LinkedBlock",
 "LinkedChannel",
 "CaseMatchs",
 "NullCaseMatchs",
 "CaseMatch",
 "CaseRange",
 "CaseImplicant",
 "EnumElems",
 "NullEnumElems",
 "EnumElem",
 "DefaultValuedEnumElem",
 "ValuedEnumElem",
 "RecordElems",
 "NullRecordElems",
 "RecordElem",
 "ValDecls",
 "NullValDecls",
 "ValDecl",
};
static Tree_tKind yyTypeRange [182 + 1] = { 0,
 kBalsaProgram,
 kNullDecl,
 kNullDecls,
 kNullDecl,
 kImportDecl,
 kFileDecl,
 kTypeDecl,
 kConstantDecl,
 kVariableDecl,
 kInitVariableDecl,
 kChannelDecl,
 kChannelArrayDecl,
 kSyncDecl,
 kSyncArrayDecl,
 kProcedureDecl,
 kProcAliasDecl,
 kFunctionDecl,
 kBuiltinFunctionDecl,
 kSharedDecl,
 kPartDecl,
 kIfDecls,
 kIfElseDecls,
 kPrintDecl,
 kNullDecl,
 kTypeRange,
 kSpecifiedRange,
 kTypeRange,
 kEnumType,
 kNullType,
 kNumericType,
 kExistingType,
 kBuiltinType,
 kArrayType,
 kNewType,
 kRecordType,
 kEnumType,
 kCoercedExpr,
 kPrimedExpr,
 kNullExpr,
 kLiteralExpr,
 kIdentExpr,
 kStringExpr,
 kImplicantExpr,
 kDontCareExpr,
 kAggregateConsExpr,
 kNamedEnumElemExpr,
 kUnaryExpr,
 kBinaryExpr,
 kRecordElemExtractExpr,
 kArrayExtractExpr,
 kArraySliceExpr,
 kAsExpr,
 kBitArrayCastExpr,
 kLetExpr,
 kFunctionCallExpr,
 kSizeofExpr,
 kArrayAppendExpr,
 kPrimedExpr,
 kIfElsePorts,
 kNullFormalPorts,
 kIfElsePorts,
 kValuePort,
 kParamPort,
 kTypeParamPort,
 kChannelPort,
 kChannelPortArray,
 kSyncPort,
 kSyncPortArray,
 kIfPorts,
 kIfElsePorts,
 kBlock,
 kTypeParameter,
 kNullParameters,
 kTypeParameter,
 kNumberParameter,
 kStringParameter,
 kTypeParameter,
 kBreezeTypeParameter,
 kNullBreezeParameters,
 kBreezeTypeParameter,
 kBreezeExprParameter,
 kBreezeTypeParameter,
 kUndeclaredComp,
 kNullComps,
 kUndeclaredComp,
 kNormalComp,
 kUndeclaredComp,
 kSinkCommand,
 kNullCommand,
 kContinueCommand,
 kHaltCommand,
 kInputCommand,
 kInputEncloseCommand,
 kInputEncloseBangCommand,
 kOutputCommand,
 kSyncCommand,
 kAssignmentCommand,
 kBlockCommand,
 kSequentialCommand,
 kParallelCommand,
 kLoopCommand,
 kWhileGuardsCommand,
 kWhileGuardsAlsoCommand,
 kCommandWhileExprCommand,
 kCommandWhileGuardsCommand,
 kCommandWhileGuardsAlsoCommand,
 kIfCommand,
 kIfElseCommand,
 kCaseCommand,
 kForCommand,
 kProcedureCallCommonCommand,
 kSelectCommand,
 kSelectBangCommand,
 kArbitrateCommand,
 kPrintCommand,
 kSinkCommand,
 kBlockProcParam,
 kNullProcParams,
 kBlockProcParam,
 kExprProcParam,
 kTypeProcParam,
 kVarReadProcParam,
 kVarWriteProcParam,
 kBlockProcParam,
 kTypeFuncParam,
 kNullFuncParams,
 kTypeFuncParam,
 kExprFuncParam,
 kTypeFuncParam,
 kBitArrayCastLvalue,
 kIdentLvalue,
 kRecordElemLvalue,
 kArrayElemLvalue,
 kArraySliceLvalue,
 kArrayAppendLvalue,
 kArrayConsLvalue,
 kAsLvalue,
 kBitArrayCastLvalue,
 kGuard,
 kNullGuards,
 kGuard,
 kPortGuard,
 kNullPortGuards,
 kPortGuard,
 kDeclGuard,
 kNullDeclGuards,
 kDeclGuard,
 kForCaseGuard,
 kNullCaseGuards,
 kForCaseGuard,
 kCaseMatchGuard,
 kForCaseGuard,
 kChannelGuard,
 kNullChannelGuards,
 kChannelGuard,
 kIdent,
 kNullIdents,
 kIdent,
 kExprList,
 kNullExprLists,
 kExprList,
 kLinkedChannel,
 kNullLvalueLists,
 kLinkedChannel,
 kLinkedBlock,
 kLinkedChannel,
 kCaseImplicant,
 kNullCaseMatchs,
 kCaseImplicant,
 kCaseRange,
 kCaseImplicant,
 kValuedEnumElem,
 kNullEnumElems,
 kValuedEnumElem,
 kDefaultValuedEnumElem,
 kValuedEnumElem,
 kRecordElem,
 kNullRecordElems,
 kRecordElem,
 kValDecl,
 kNullValDecls,
 kValDecl,
};

tTree Tree_Alloc ()
{
 register yytBlockPtr yyBlockPtr = yyBlockList;
 register int i;

 if (yyMaxSize == 0)
  for (i = 1; i <= 182; i ++) {
   Tree_NodeSize [i] = (Tree_NodeSize [i] + yyMaxAlign - 1) & yyAlignMasks [yyMaxAlign];
   yyMaxSize = Max (Tree_NodeSize [i], yyMaxSize);
  }
 yyBlockList = (yytBlockPtr) Alloc (sizeof (yytBlock));
 yyBlockList->yySuccessor = yyBlockPtr;
 Tree_PoolFreePtr = yyBlockList->yyBlock;
 Tree_PoolMaxPtr = Tree_PoolFreePtr + yyBlockSize - yyMaxSize + 1;
 Tree_HeapUsed += yyBlockSize;
 return (tTree) Tree_PoolFreePtr;
}

tTree MakeTree
# if defined __STDC__ | defined __cplusplus
 (Tree_tKind yyKind)
# else
 (yyKind) Tree_tKind yyKind;
# endif
{
 register tTree yyt;
 yyALLOC (yyt, Tree_NodeSize [yyKind])
 yyt->Kind = yyKind;
 yyt->yyHead.yyMark = 0;
 return yyt;
}

bool Tree_IsType
# if defined __STDC__ | defined __cplusplus
 (register tTree yyt, register Tree_tKind yyKind)
# else
 (yyt, yyKind) register tTree yyt; register Tree_tKind yyKind;
# endif
{
 return yyt != NoTree && yyKind <= yyt->Kind && yyt->Kind <= yyTypeRange [yyKind];
}


tTree mBalsaProgram
# if defined __STDC__ | defined __cplusplus
(tTree pDecls)
# else
(pDecls)
tTree pDecls;
# endif
{
 register tTree yyt;
 yyALLOC (yyt, Tree_NodeSize [kBalsaProgram])
 yyt->Kind = kBalsaProgram;
 yyt->yyHead.yyMark = 0;
 yyt->BalsaProgram.Decls = pDecls;
 beginPtrContext(yyt->BalsaProgram.context)
 return yyt;
}

tTree mDecls
# if defined __STDC__ | defined __cplusplus
(tPosition pposition)
# else
(pposition)
tPosition pposition;
# endif
{
 register tTree yyt;
 yyALLOC (yyt, Tree_NodeSize [kDecls])
 yyt->Kind = kDecls;
 yyt->yyHead.yyMark = 0;
 yyt->Decls.position = pposition;
 beginScope(yyt->Decls.scope)
 beginPtrContext(yyt->Decls.contextIn)
 beginPtrContext(yyt->Decls.contextOut)
 beginbool(yyt->Decls.skip)
 return yyt;
}

tTree mNullDecls
# if defined __STDC__ | defined __cplusplus
(tPosition pposition)
# else
(pposition)
tPosition pposition;
# endif
{
 register tTree yyt;
 yyALLOC (yyt, Tree_NodeSize [kNullDecls])
 yyt->Kind = kNullDecls;
 yyt->yyHead.yyMark = 0;
 yyt->NullDecls.position = pposition;
 beginScope(yyt->NullDecls.scope)
 beginPtrContext(yyt->NullDecls.contextIn)
 beginPtrContext(yyt->NullDecls.contextOut)
 beginbool(yyt->NullDecls.skip)
 return yyt;
}

tTree mDecl
# if defined __STDC__ | defined __cplusplus
(tPosition pposition, tTree pnext)
# else
(pposition, pnext)
tPosition pposition;
tTree pnext;
# endif
{
 register tTree yyt;
 yyALLOC (yyt, Tree_NodeSize [kDecl])
 yyt->Kind = kDecl;
 yyt->yyHead.yyMark = 0;
 yyt->Decl.position = pposition;
 beginScope(yyt->Decl.scope)
 beginPtrContext(yyt->Decl.contextIn)
 beginPtrContext(yyt->Decl.contextOut)
 beginbool(yyt->Decl.skip)
 yyt->Decl.next = pnext;
 return yyt;
}

tTree mImportDecl
# if defined __STDC__ | defined __cplusplus
(tPosition pposition, tTree pnext, tTree pIdents)
# else
(pposition, pnext, pIdents)
tPosition pposition;
tTree pnext;
tTree pIdents;
# endif
{
 register tTree yyt;
 yyALLOC (yyt, Tree_NodeSize [kImportDecl])
 yyt->Kind = kImportDecl;
 yyt->yyHead.yyMark = 0;
 yyt->ImportDecl.position = pposition;
 beginScope(yyt->ImportDecl.scope)
 beginPtrContext(yyt->ImportDecl.contextIn)
 beginPtrContext(yyt->ImportDecl.contextOut)
 beginbool(yyt->ImportDecl.skip)
 yyt->ImportDecl.next = pnext;
 yyt->ImportDecl.Idents = pIdents;
 return yyt;
}

tTree mFileDecl
# if defined __STDC__ | defined __cplusplus
(tPosition pposition, tTree pnext, tTree pIdents)
# else
(pposition, pnext, pIdents)
tPosition pposition;
tTree pnext;
tTree pIdents;
# endif
{
 register tTree yyt;
 yyALLOC (yyt, Tree_NodeSize [kFileDecl])
 yyt->Kind = kFileDecl;
 yyt->yyHead.yyMark = 0;
 yyt->FileDecl.position = pposition;
 beginScope(yyt->FileDecl.scope)
 beginPtrContext(yyt->FileDecl.contextIn)
 beginPtrContext(yyt->FileDecl.contextOut)
 beginbool(yyt->FileDecl.skip)
 yyt->FileDecl.next = pnext;
 yyt->FileDecl.Idents = pIdents;
 return yyt;
}

tTree mTypeDecl
# if defined __STDC__ | defined __cplusplus
(tPosition pposition, tTree pnext, tIdent pident, tTree pAType)
# else
(pposition, pnext, pident, pAType)
tPosition pposition;
tTree pnext;
tIdent pident;
tTree pAType;
# endif
{
 register tTree yyt;
 yyALLOC (yyt, Tree_NodeSize [kTypeDecl])
 yyt->Kind = kTypeDecl;
 yyt->yyHead.yyMark = 0;
 yyt->TypeDecl.position = pposition;
 beginScope(yyt->TypeDecl.scope)
 beginPtrContext(yyt->TypeDecl.contextIn)
 beginPtrContext(yyt->TypeDecl.contextOut)
 beginbool(yyt->TypeDecl.skip)
 yyt->TypeDecl.next = pnext;
 yyt->TypeDecl.ident = pident;
 yyt->TypeDecl.AType = pAType;
 return yyt;
}

tTree mConstantDecl
# if defined __STDC__ | defined __cplusplus
(tPosition pposition, tTree pnext, tIdent pident, tTree pCoercedExpr, tTree pAType)
# else
(pposition, pnext, pident, pCoercedExpr, pAType)
tPosition pposition;
tTree pnext;
tIdent pident;
tTree pCoercedExpr;
tTree pAType;
# endif
{
 register tTree yyt;
 yyALLOC (yyt, Tree_NodeSize [kConstantDecl])
 yyt->Kind = kConstantDecl;
 yyt->yyHead.yyMark = 0;
 yyt->ConstantDecl.position = pposition;
 beginScope(yyt->ConstantDecl.scope)
 beginPtrContext(yyt->ConstantDecl.contextIn)
 beginPtrContext(yyt->ConstantDecl.contextOut)
 beginbool(yyt->ConstantDecl.skip)
 yyt->ConstantDecl.next = pnext;
 yyt->ConstantDecl.ident = pident;
 yyt->ConstantDecl.CoercedExpr = pCoercedExpr;
 yyt->ConstantDecl.AType = pAType;
 return yyt;
}

tTree mVariableDecl
# if defined __STDC__ | defined __cplusplus
(tPosition pposition, tTree pnext, tTree pIdents, tTree pAType)
# else
(pposition, pnext, pIdents, pAType)
tPosition pposition;
tTree pnext;
tTree pIdents;
tTree pAType;
# endif
{
 register tTree yyt;
 yyALLOC (yyt, Tree_NodeSize [kVariableDecl])
 yyt->Kind = kVariableDecl;
 yyt->yyHead.yyMark = 0;
 yyt->VariableDecl.position = pposition;
 beginScope(yyt->VariableDecl.scope)
 beginPtrContext(yyt->VariableDecl.contextIn)
 beginPtrContext(yyt->VariableDecl.contextOut)
 beginbool(yyt->VariableDecl.skip)
 yyt->VariableDecl.next = pnext;
 yyt->VariableDecl.Idents = pIdents;
 yyt->VariableDecl.AType = pAType;
 return yyt;
}

tTree mInitVariableDecl
# if defined __STDC__ | defined __cplusplus
(tPosition pposition, tTree pnext, tTree pIdents, tTree pCoercedExpr, tTree pAType)
# else
(pposition, pnext, pIdents, pCoercedExpr, pAType)
tPosition pposition;
tTree pnext;
tTree pIdents;
tTree pCoercedExpr;
tTree pAType;
# endif
{
 register tTree yyt;
 yyALLOC (yyt, Tree_NodeSize [kInitVariableDecl])
 yyt->Kind = kInitVariableDecl;
 yyt->yyHead.yyMark = 0;
 yyt->InitVariableDecl.position = pposition;
 beginScope(yyt->InitVariableDecl.scope)
 beginPtrContext(yyt->InitVariableDecl.contextIn)
 beginPtrContext(yyt->InitVariableDecl.contextOut)
 beginbool(yyt->InitVariableDecl.skip)
 yyt->InitVariableDecl.next = pnext;
 yyt->InitVariableDecl.Idents = pIdents;
 yyt->InitVariableDecl.CoercedExpr = pCoercedExpr;
 yyt->InitVariableDecl.AType = pAType;
 return yyt;
}

tTree mChannelDecl
# if defined __STDC__ | defined __cplusplus
(tPosition pposition, tTree pnext, tTree pIdents, tTree pAType, bool pmulticast)
# else
(pposition, pnext, pIdents, pAType, pmulticast)
tPosition pposition;
tTree pnext;
tTree pIdents;
tTree pAType;
bool pmulticast;
# endif
{
 register tTree yyt;
 yyALLOC (yyt, Tree_NodeSize [kChannelDecl])
 yyt->Kind = kChannelDecl;
 yyt->yyHead.yyMark = 0;
 yyt->ChannelDecl.position = pposition;
 beginScope(yyt->ChannelDecl.scope)
 beginPtrContext(yyt->ChannelDecl.contextIn)
 beginPtrContext(yyt->ChannelDecl.contextOut)
 beginbool(yyt->ChannelDecl.skip)
 yyt->ChannelDecl.next = pnext;
 yyt->ChannelDecl.Idents = pIdents;
 yyt->ChannelDecl.AType = pAType;
 yyt->ChannelDecl.multicast = pmulticast;
 return yyt;
}

tTree mChannelArrayDecl
# if defined __STDC__ | defined __cplusplus
(tPosition pposition, tTree pnext, tTree pIdents, tTree pAType, tTree pRange, bool pmulticast)
# else
(pposition, pnext, pIdents, pAType, pRange, pmulticast)
tPosition pposition;
tTree pnext;
tTree pIdents;
tTree pAType;
tTree pRange;
bool pmulticast;
# endif
{
 register tTree yyt;
 yyALLOC (yyt, Tree_NodeSize [kChannelArrayDecl])
 yyt->Kind = kChannelArrayDecl;
 yyt->yyHead.yyMark = 0;
 yyt->ChannelArrayDecl.position = pposition;
 beginScope(yyt->ChannelArrayDecl.scope)
 beginPtrContext(yyt->ChannelArrayDecl.contextIn)
 beginPtrContext(yyt->ChannelArrayDecl.contextOut)
 beginbool(yyt->ChannelArrayDecl.skip)
 yyt->ChannelArrayDecl.next = pnext;
 yyt->ChannelArrayDecl.Idents = pIdents;
 yyt->ChannelArrayDecl.AType = pAType;
 yyt->ChannelArrayDecl.Range = pRange;
 yyt->ChannelArrayDecl.multicast = pmulticast;
 return yyt;
}

tTree mSyncDecl
# if defined __STDC__ | defined __cplusplus
(tPosition pposition, tTree pnext, tTree pIdents, bool pmulticast)
# else
(pposition, pnext, pIdents, pmulticast)
tPosition pposition;
tTree pnext;
tTree pIdents;
bool pmulticast;
# endif
{
 register tTree yyt;
 yyALLOC (yyt, Tree_NodeSize [kSyncDecl])
 yyt->Kind = kSyncDecl;
 yyt->yyHead.yyMark = 0;
 yyt->SyncDecl.position = pposition;
 beginScope(yyt->SyncDecl.scope)
 beginPtrContext(yyt->SyncDecl.contextIn)
 beginPtrContext(yyt->SyncDecl.contextOut)
 beginbool(yyt->SyncDecl.skip)
 yyt->SyncDecl.next = pnext;
 yyt->SyncDecl.Idents = pIdents;
 yyt->SyncDecl.multicast = pmulticast;
 return yyt;
}

tTree mSyncArrayDecl
# if defined __STDC__ | defined __cplusplus
(tPosition pposition, tTree pnext, tTree pIdents, tTree pRange, bool pmulticast)
# else
(pposition, pnext, pIdents, pRange, pmulticast)
tPosition pposition;
tTree pnext;
tTree pIdents;
tTree pRange;
bool pmulticast;
# endif
{
 register tTree yyt;
 yyALLOC (yyt, Tree_NodeSize [kSyncArrayDecl])
 yyt->Kind = kSyncArrayDecl;
 yyt->yyHead.yyMark = 0;
 yyt->SyncArrayDecl.position = pposition;
 beginScope(yyt->SyncArrayDecl.scope)
 beginPtrContext(yyt->SyncArrayDecl.contextIn)
 beginPtrContext(yyt->SyncArrayDecl.contextOut)
 beginbool(yyt->SyncArrayDecl.skip)
 yyt->SyncArrayDecl.next = pnext;
 yyt->SyncArrayDecl.Idents = pIdents;
 yyt->SyncArrayDecl.Range = pRange;
 yyt->SyncArrayDecl.multicast = pmulticast;
 return yyt;
}

tTree mProcedureDecl
# if defined __STDC__ | defined __cplusplus
(tPosition pposition, tTree pnext, tIdent pident, tTree pFormalPorts, tTree pBlock)
# else
(pposition, pnext, pident, pFormalPorts, pBlock)
tPosition pposition;
tTree pnext;
tIdent pident;
tTree pFormalPorts;
tTree pBlock;
# endif
{
 register tTree yyt;
 yyALLOC (yyt, Tree_NodeSize [kProcedureDecl])
 yyt->Kind = kProcedureDecl;
 yyt->yyHead.yyMark = 0;
 yyt->ProcedureDecl.position = pposition;
 beginScope(yyt->ProcedureDecl.scope)
 beginPtrContext(yyt->ProcedureDecl.contextIn)
 beginPtrContext(yyt->ProcedureDecl.contextOut)
 beginbool(yyt->ProcedureDecl.skip)
 yyt->ProcedureDecl.next = pnext;
 yyt->ProcedureDecl.ident = pident;
 yyt->ProcedureDecl.FormalPorts = pFormalPorts;
 yyt->ProcedureDecl.Block = pBlock;
 beginPtrProcedure(yyt->ProcedureDecl.procedure)
 return yyt;
}

tTree mProcAliasDecl
# if defined __STDC__ | defined __cplusplus
(tPosition pposition, tTree pnext, tIdent pnewName, tIdent poldName, tTree pProcedureParams)
# else
(pposition, pnext, pnewName, poldName, pProcedureParams)
tPosition pposition;
tTree pnext;
tIdent pnewName;
tIdent poldName;
tTree pProcedureParams;
# endif
{
 register tTree yyt;
 yyALLOC (yyt, Tree_NodeSize [kProcAliasDecl])
 yyt->Kind = kProcAliasDecl;
 yyt->yyHead.yyMark = 0;
 yyt->ProcAliasDecl.position = pposition;
 beginScope(yyt->ProcAliasDecl.scope)
 beginPtrContext(yyt->ProcAliasDecl.contextIn)
 beginPtrContext(yyt->ProcAliasDecl.contextOut)
 beginbool(yyt->ProcAliasDecl.skip)
 yyt->ProcAliasDecl.next = pnext;
 yyt->ProcAliasDecl.newName = pnewName;
 yyt->ProcAliasDecl.oldName = poldName;
 yyt->ProcAliasDecl.ProcedureParams = pProcedureParams;
 return yyt;
}

tTree mFunctionDecl
# if defined __STDC__ | defined __cplusplus
(tPosition pposition, tTree pnext, tIdent pident, tTree pFormalPorts, tTree pCoercedExpr, tTree pAType)
# else
(pposition, pnext, pident, pFormalPorts, pCoercedExpr, pAType)
tPosition pposition;
tTree pnext;
tIdent pident;
tTree pFormalPorts;
tTree pCoercedExpr;
tTree pAType;
# endif
{
 register tTree yyt;
 yyALLOC (yyt, Tree_NodeSize [kFunctionDecl])
 yyt->Kind = kFunctionDecl;
 yyt->yyHead.yyMark = 0;
 yyt->FunctionDecl.position = pposition;
 beginScope(yyt->FunctionDecl.scope)
 beginPtrContext(yyt->FunctionDecl.contextIn)
 beginPtrContext(yyt->FunctionDecl.contextOut)
 beginbool(yyt->FunctionDecl.skip)
 yyt->FunctionDecl.next = pnext;
 yyt->FunctionDecl.ident = pident;
 yyt->FunctionDecl.FormalPorts = pFormalPorts;
 yyt->FunctionDecl.CoercedExpr = pCoercedExpr;
 yyt->FunctionDecl.AType = pAType;
 beginPtrProcedure(yyt->FunctionDecl.function)
 return yyt;
}

tTree mBuiltinFunctionDecl
# if defined __STDC__ | defined __cplusplus
(tPosition pposition, tTree pnext, tIdent pident, tTree pFormalPorts, tTree pAType)
# else
(pposition, pnext, pident, pFormalPorts, pAType)
tPosition pposition;
tTree pnext;
tIdent pident;
tTree pFormalPorts;
tTree pAType;
# endif
{
 register tTree yyt;
 yyALLOC (yyt, Tree_NodeSize [kBuiltinFunctionDecl])
 yyt->Kind = kBuiltinFunctionDecl;
 yyt->yyHead.yyMark = 0;
 yyt->BuiltinFunctionDecl.position = pposition;
 beginScope(yyt->BuiltinFunctionDecl.scope)
 beginPtrContext(yyt->BuiltinFunctionDecl.contextIn)
 beginPtrContext(yyt->BuiltinFunctionDecl.contextOut)
 beginbool(yyt->BuiltinFunctionDecl.skip)
 yyt->BuiltinFunctionDecl.next = pnext;
 yyt->BuiltinFunctionDecl.ident = pident;
 yyt->BuiltinFunctionDecl.FormalPorts = pFormalPorts;
 yyt->BuiltinFunctionDecl.AType = pAType;
 beginPtrProcedure(yyt->BuiltinFunctionDecl.function)
 return yyt;
}

tTree mSharedDecl
# if defined __STDC__ | defined __cplusplus
(tPosition pposition, tTree pnext, tIdent pident, tTree pBlock)
# else
(pposition, pnext, pident, pBlock)
tPosition pposition;
tTree pnext;
tIdent pident;
tTree pBlock;
# endif
{
 register tTree yyt;
 yyALLOC (yyt, Tree_NodeSize [kSharedDecl])
 yyt->Kind = kSharedDecl;
 yyt->yyHead.yyMark = 0;
 yyt->SharedDecl.position = pposition;
 beginScope(yyt->SharedDecl.scope)
 beginPtrContext(yyt->SharedDecl.contextIn)
 beginPtrContext(yyt->SharedDecl.contextOut)
 beginbool(yyt->SharedDecl.skip)
 yyt->SharedDecl.next = pnext;
 yyt->SharedDecl.ident = pident;
 yyt->SharedDecl.Block = pBlock;
 beginPtrProcedure(yyt->SharedDecl.procedure)
 return yyt;
}

tTree mPartDecl
# if defined __STDC__ | defined __cplusplus
(tPosition pposition, tTree pnext, tIdent pident, PtrLispList pattributes, tTree pFormalPorts, PtrWireArray pchannels, unsigned pchannelCount, tTree pComps, PtrCallContextList pcallcontexts)
# else
(pposition, pnext, pident, pattributes, pFormalPorts, pchannels, pchannelCount, pComps, pcallcontexts)
tPosition pposition;
tTree pnext;
tIdent pident;
PtrLispList pattributes;
tTree pFormalPorts;
PtrWireArray pchannels;
unsigned pchannelCount;
tTree pComps;
PtrCallContextList pcallcontexts;
# endif
{
 register tTree yyt;
 yyALLOC (yyt, Tree_NodeSize [kPartDecl])
 yyt->Kind = kPartDecl;
 yyt->yyHead.yyMark = 0;
 yyt->PartDecl.position = pposition;
 beginScope(yyt->PartDecl.scope)
 beginPtrContext(yyt->PartDecl.contextIn)
 beginPtrContext(yyt->PartDecl.contextOut)
 beginbool(yyt->PartDecl.skip)
 yyt->PartDecl.next = pnext;
 yyt->PartDecl.ident = pident;
 yyt->PartDecl.attributes = pattributes;
 yyt->PartDecl.FormalPorts = pFormalPorts;
 yyt->PartDecl.channels = pchannels;
 yyt->PartDecl.channelCount = pchannelCount;
 yyt->PartDecl.Comps = pComps;
 yyt->PartDecl.callcontexts = pcallcontexts;
 beginPtrProcedure(yyt->PartDecl.procedure)
 return yyt;
}

tTree mIfDecls
# if defined __STDC__ | defined __cplusplus
(tPosition pposition, tTree pnext, tTree pDeclGuards)
# else
(pposition, pnext, pDeclGuards)
tPosition pposition;
tTree pnext;
tTree pDeclGuards;
# endif
{
 register tTree yyt;
 yyALLOC (yyt, Tree_NodeSize [kIfDecls])
 yyt->Kind = kIfDecls;
 yyt->yyHead.yyMark = 0;
 yyt->IfDecls.position = pposition;
 beginScope(yyt->IfDecls.scope)
 beginPtrContext(yyt->IfDecls.contextIn)
 beginPtrContext(yyt->IfDecls.contextOut)
 beginbool(yyt->IfDecls.skip)
 yyt->IfDecls.next = pnext;
 yyt->IfDecls.DeclGuards = pDeclGuards;
 return yyt;
}

tTree mIfElseDecls
# if defined __STDC__ | defined __cplusplus
(tPosition pposition, tTree pnext, tTree pDeclGuards, tTree pDecls)
# else
(pposition, pnext, pDeclGuards, pDecls)
tPosition pposition;
tTree pnext;
tTree pDeclGuards;
tTree pDecls;
# endif
{
 register tTree yyt;
 yyALLOC (yyt, Tree_NodeSize [kIfElseDecls])
 yyt->Kind = kIfElseDecls;
 yyt->yyHead.yyMark = 0;
 yyt->IfElseDecls.position = pposition;
 beginScope(yyt->IfElseDecls.scope)
 beginPtrContext(yyt->IfElseDecls.contextIn)
 beginPtrContext(yyt->IfElseDecls.contextOut)
 beginbool(yyt->IfElseDecls.skip)
 yyt->IfElseDecls.next = pnext;
 yyt->IfElseDecls.DeclGuards = pDeclGuards;
 yyt->IfElseDecls.Decls = pDecls;
 return yyt;
}

tTree mPrintDecl
# if defined __STDC__ | defined __cplusplus
(tPosition pposition, tTree pnext, tTree pExpr, tTree pExprLists)
# else
(pposition, pnext, pExpr, pExprLists)
tPosition pposition;
tTree pnext;
tTree pExpr;
tTree pExprLists;
# endif
{
 register tTree yyt;
 yyALLOC (yyt, Tree_NodeSize [kPrintDecl])
 yyt->Kind = kPrintDecl;
 yyt->yyHead.yyMark = 0;
 yyt->PrintDecl.position = pposition;
 beginScope(yyt->PrintDecl.scope)
 beginPtrContext(yyt->PrintDecl.contextIn)
 beginPtrContext(yyt->PrintDecl.contextOut)
 beginbool(yyt->PrintDecl.skip)
 yyt->PrintDecl.next = pnext;
 yyt->PrintDecl.Expr = pExpr;
 yyt->PrintDecl.ExprLists = pExprLists;
 return yyt;
}

tTree mNullDecl
# if defined __STDC__ | defined __cplusplus
(tPosition pposition, tTree pnext)
# else
(pposition, pnext)
tPosition pposition;
tTree pnext;
# endif
{
 register tTree yyt;
 yyALLOC (yyt, Tree_NodeSize [kNullDecl])
 yyt->Kind = kNullDecl;
 yyt->yyHead.yyMark = 0;
 yyt->NullDecl.position = pposition;
 beginScope(yyt->NullDecl.scope)
 beginPtrContext(yyt->NullDecl.contextIn)
 beginPtrContext(yyt->NullDecl.contextOut)
 beginbool(yyt->NullDecl.skip)
 yyt->NullDecl.next = pnext;
 return yyt;
}

tTree mRange
# if defined __STDC__ | defined __cplusplus
(tPosition pposition)
# else
(pposition)
tPosition pposition;
# endif
{
 register tTree yyt;
 yyALLOC (yyt, Tree_NodeSize [kRange])
 yyt->Kind = kRange;
 yyt->yyHead.yyMark = 0;
 yyt->Range.position = pposition;
 beginPtrContext(yyt->Range.context)
 beginbool(yyt->Range.skip)
 beginSpan(yyt->Range.range)
 beginPtrType(yyt->Range.expectedType)
 beginbool(yyt->Range.isSpan)
 return yyt;
}

tTree mSpecifiedRange
# if defined __STDC__ | defined __cplusplus
(tPosition pposition, tTree pLeft, tTree pRight)
# else
(pposition, pLeft, pRight)
tPosition pposition;
tTree pLeft;
tTree pRight;
# endif
{
 register tTree yyt;
 yyALLOC (yyt, Tree_NodeSize [kSpecifiedRange])
 yyt->Kind = kSpecifiedRange;
 yyt->yyHead.yyMark = 0;
 yyt->SpecifiedRange.position = pposition;
 beginPtrContext(yyt->SpecifiedRange.context)
 beginbool(yyt->SpecifiedRange.skip)
 beginSpan(yyt->SpecifiedRange.range)
 beginPtrType(yyt->SpecifiedRange.expectedType)
 beginbool(yyt->SpecifiedRange.isSpan)
 yyt->SpecifiedRange.Left = pLeft;
 yyt->SpecifiedRange.Right = pRight;
 return yyt;
}

tTree mTypeRange
# if defined __STDC__ | defined __cplusplus
(tPosition pposition, tTree pAType)
# else
(pposition, pAType)
tPosition pposition;
tTree pAType;
# endif
{
 register tTree yyt;
 yyALLOC (yyt, Tree_NodeSize [kTypeRange])
 yyt->Kind = kTypeRange;
 yyt->yyHead.yyMark = 0;
 yyt->TypeRange.position = pposition;
 beginPtrContext(yyt->TypeRange.context)
 beginbool(yyt->TypeRange.skip)
 beginSpan(yyt->TypeRange.range)
 beginPtrType(yyt->TypeRange.expectedType)
 beginbool(yyt->TypeRange.isSpan)
 yyt->TypeRange.AType = pAType;
 return yyt;
}

tTree mAType
# if defined __STDC__ | defined __cplusplus
(tPosition pposition)
# else
(pposition)
tPosition pposition;
# endif
{
 register tTree yyt;
 yyALLOC (yyt, Tree_NodeSize [kAType])
 yyt->Kind = kAType;
 yyt->yyHead.yyMark = 0;
 yyt->AType.position = pposition;
 beginPtrContext(yyt->AType.context)
 beginbool(yyt->AType.skip)
 beginPtrType(yyt->AType.type)
 return yyt;
}

tTree mNullType
# if defined __STDC__ | defined __cplusplus
(tPosition pposition)
# else
(pposition)
tPosition pposition;
# endif
{
 register tTree yyt;
 yyALLOC (yyt, Tree_NodeSize [kNullType])
 yyt->Kind = kNullType;
 yyt->yyHead.yyMark = 0;
 yyt->NullType.position = pposition;
 beginPtrContext(yyt->NullType.context)
 beginbool(yyt->NullType.skip)
 beginPtrType(yyt->NullType.type)
 return yyt;
}

tTree mNumericType
# if defined __STDC__ | defined __cplusplus
(tPosition pposition, bool psignedness, tTree pExpr)
# else
(pposition, psignedness, pExpr)
tPosition pposition;
bool psignedness;
tTree pExpr;
# endif
{
 register tTree yyt;
 yyALLOC (yyt, Tree_NodeSize [kNumericType])
 yyt->Kind = kNumericType;
 yyt->yyHead.yyMark = 0;
 yyt->NumericType.position = pposition;
 beginPtrContext(yyt->NumericType.context)
 beginbool(yyt->NumericType.skip)
 beginPtrType(yyt->NumericType.type)
 yyt->NumericType.signedness = psignedness;
 yyt->NumericType.Expr = pExpr;
 return yyt;
}

tTree mExistingType
# if defined __STDC__ | defined __cplusplus
(tPosition pposition, tIdent pident)
# else
(pposition, pident)
tPosition pposition;
tIdent pident;
# endif
{
 register tTree yyt;
 yyALLOC (yyt, Tree_NodeSize [kExistingType])
 yyt->Kind = kExistingType;
 yyt->yyHead.yyMark = 0;
 yyt->ExistingType.position = pposition;
 beginPtrContext(yyt->ExistingType.context)
 beginbool(yyt->ExistingType.skip)
 beginPtrType(yyt->ExistingType.type)
 yyt->ExistingType.ident = pident;
 return yyt;
}

tTree mBuiltinType
# if defined __STDC__ | defined __cplusplus
(tPosition pposition)
# else
(pposition)
tPosition pposition;
# endif
{
 register tTree yyt;
 yyALLOC (yyt, Tree_NodeSize [kBuiltinType])
 yyt->Kind = kBuiltinType;
 yyt->yyHead.yyMark = 0;
 yyt->BuiltinType.position = pposition;
 beginPtrContext(yyt->BuiltinType.context)
 beginbool(yyt->BuiltinType.skip)
 beginPtrType(yyt->BuiltinType.type)
 return yyt;
}

tTree mArrayType
# if defined __STDC__ | defined __cplusplus
(tPosition pposition, tTree pAType, tTree pRange)
# else
(pposition, pAType, pRange)
tPosition pposition;
tTree pAType;
tTree pRange;
# endif
{
 register tTree yyt;
 yyALLOC (yyt, Tree_NodeSize [kArrayType])
 yyt->Kind = kArrayType;
 yyt->yyHead.yyMark = 0;
 yyt->ArrayType.position = pposition;
 beginPtrContext(yyt->ArrayType.context)
 beginbool(yyt->ArrayType.skip)
 beginPtrType(yyt->ArrayType.type)
 yyt->ArrayType.AType = pAType;
 yyt->ArrayType.Range = pRange;
 return yyt;
}

tTree mNewType
# if defined __STDC__ | defined __cplusplus
(tPosition pposition, tTree pAType)
# else
(pposition, pAType)
tPosition pposition;
tTree pAType;
# endif
{
 register tTree yyt;
 yyALLOC (yyt, Tree_NodeSize [kNewType])
 yyt->Kind = kNewType;
 yyt->yyHead.yyMark = 0;
 yyt->NewType.position = pposition;
 beginPtrContext(yyt->NewType.context)
 beginbool(yyt->NewType.skip)
 beginPtrType(yyt->NewType.type)
 yyt->NewType.AType = pAType;
 return yyt;
}

tTree mRecordType
# if defined __STDC__ | defined __cplusplus
(tPosition pposition, tTree pRecordElems, tTree pAType)
# else
(pposition, pRecordElems, pAType)
tPosition pposition;
tTree pRecordElems;
tTree pAType;
# endif
{
 register tTree yyt;
 yyALLOC (yyt, Tree_NodeSize [kRecordType])
 yyt->Kind = kRecordType;
 yyt->yyHead.yyMark = 0;
 yyt->RecordType.position = pposition;
 beginPtrContext(yyt->RecordType.context)
 beginbool(yyt->RecordType.skip)
 beginPtrType(yyt->RecordType.type)
 yyt->RecordType.RecordElems = pRecordElems;
 yyt->RecordType.AType = pAType;
 return yyt;
}

tTree mEnumType
# if defined __STDC__ | defined __cplusplus
(tPosition pposition, tTree pEnumElems, tTree pAType)
# else
(pposition, pEnumElems, pAType)
tPosition pposition;
tTree pEnumElems;
tTree pAType;
# endif
{
 register tTree yyt;
 yyALLOC (yyt, Tree_NodeSize [kEnumType])
 yyt->Kind = kEnumType;
 yyt->yyHead.yyMark = 0;
 yyt->EnumType.position = pposition;
 beginPtrContext(yyt->EnumType.context)
 beginbool(yyt->EnumType.skip)
 beginPtrType(yyt->EnumType.type)
 yyt->EnumType.EnumElems = pEnumElems;
 yyt->EnumType.AType = pAType;
 return yyt;
}

tTree mCoercedExpr
# if defined __STDC__ | defined __cplusplus
(tTree pExpr)
# else
(pExpr)
tTree pExpr;
# endif
{
 register tTree yyt;
 yyALLOC (yyt, Tree_NodeSize [kCoercedExpr])
 yyt->Kind = kCoercedExpr;
 yyt->yyHead.yyMark = 0;
 yyt->CoercedExpr.Expr = pExpr;
 beginPtrContext(yyt->CoercedExpr.context)
 beginbool(yyt->CoercedExpr.skip)
 beginPtrType(yyt->CoercedExpr.expectedType)
 beginExprAttributes(yyt->CoercedExpr.attributes)
 beginbool(yyt->CoercedExpr.allowChannels)
 begintPosition(yyt->CoercedExpr.position)
 return yyt;
}

tTree mExpr
# if defined __STDC__ | defined __cplusplus
(tPosition pposition)
# else
(pposition)
tPosition pposition;
# endif
{
 register tTree yyt;
 yyALLOC (yyt, Tree_NodeSize [kExpr])
 yyt->Kind = kExpr;
 yyt->yyHead.yyMark = 0;
 yyt->Expr.position = pposition;
 beginPtrContext(yyt->Expr.context)
 beginbool(yyt->Expr.skip)
 beginPtrType(yyt->Expr.expectedType)
 beginbool(yyt->Expr.allowChannels)
 beginExprAttributes(yyt->Expr.attributes)
 return yyt;
}

tTree mNullExpr
# if defined __STDC__ | defined __cplusplus
(tPosition pposition)
# else
(pposition)
tPosition pposition;
# endif
{
 register tTree yyt;
 yyALLOC (yyt, Tree_NodeSize [kNullExpr])
 yyt->Kind = kNullExpr;
 yyt->yyHead.yyMark = 0;
 yyt->NullExpr.position = pposition;
 beginPtrContext(yyt->NullExpr.context)
 beginbool(yyt->NullExpr.skip)
 beginPtrType(yyt->NullExpr.expectedType)
 beginbool(yyt->NullExpr.allowChannels)
 beginExprAttributes(yyt->NullExpr.attributes)
 return yyt;
}

tTree mLiteralExpr
# if defined __STDC__ | defined __cplusplus
(tPosition pposition, PtrMP_INT pliteral)
# else
(pposition, pliteral)
tPosition pposition;
PtrMP_INT pliteral;
# endif
{
 register tTree yyt;
 yyALLOC (yyt, Tree_NodeSize [kLiteralExpr])
 yyt->Kind = kLiteralExpr;
 yyt->yyHead.yyMark = 0;
 yyt->LiteralExpr.position = pposition;
 beginPtrContext(yyt->LiteralExpr.context)
 beginbool(yyt->LiteralExpr.skip)
 beginPtrType(yyt->LiteralExpr.expectedType)
 beginbool(yyt->LiteralExpr.allowChannels)
 beginExprAttributes(yyt->LiteralExpr.attributes)
 yyt->LiteralExpr.literal = pliteral;
 return yyt;
}

tTree mIdentExpr
# if defined __STDC__ | defined __cplusplus
(tPosition pposition, tIdent pident)
# else
(pposition, pident)
tPosition pposition;
tIdent pident;
# endif
{
 register tTree yyt;
 yyALLOC (yyt, Tree_NodeSize [kIdentExpr])
 yyt->Kind = kIdentExpr;
 yyt->yyHead.yyMark = 0;
 yyt->IdentExpr.position = pposition;
 beginPtrContext(yyt->IdentExpr.context)
 beginbool(yyt->IdentExpr.skip)
 beginPtrType(yyt->IdentExpr.expectedType)
 beginbool(yyt->IdentExpr.allowChannels)
 beginExprAttributes(yyt->IdentExpr.attributes)
 yyt->IdentExpr.ident = pident;
 return yyt;
}

tTree mStringExpr
# if defined __STDC__ | defined __cplusplus
(tPosition pposition, Ptrconstchar pstring)
# else
(pposition, pstring)
tPosition pposition;
Ptrconstchar pstring;
# endif
{
 register tTree yyt;
 yyALLOC (yyt, Tree_NodeSize [kStringExpr])
 yyt->Kind = kStringExpr;
 yyt->yyHead.yyMark = 0;
 yyt->StringExpr.position = pposition;
 beginPtrContext(yyt->StringExpr.context)
 beginbool(yyt->StringExpr.skip)
 beginPtrType(yyt->StringExpr.expectedType)
 beginbool(yyt->StringExpr.allowChannels)
 beginExprAttributes(yyt->StringExpr.attributes)
 yyt->StringExpr.string = pstring;
 return yyt;
}

tTree mImplicantExpr
# if defined __STDC__ | defined __cplusplus
(tPosition pposition, Implicant pimplicant)
# else
(pposition, pimplicant)
tPosition pposition;
Implicant pimplicant;
# endif
{
 register tTree yyt;
 yyALLOC (yyt, Tree_NodeSize [kImplicantExpr])
 yyt->Kind = kImplicantExpr;
 yyt->yyHead.yyMark = 0;
 yyt->ImplicantExpr.position = pposition;
 beginPtrContext(yyt->ImplicantExpr.context)
 beginbool(yyt->ImplicantExpr.skip)
 beginPtrType(yyt->ImplicantExpr.expectedType)
 beginbool(yyt->ImplicantExpr.allowChannels)
 beginExprAttributes(yyt->ImplicantExpr.attributes)
 yyt->ImplicantExpr.implicant = pimplicant;
 return yyt;
}

tTree mDontCareExpr
# if defined __STDC__ | defined __cplusplus
(tPosition pposition)
# else
(pposition)
tPosition pposition;
# endif
{
 register tTree yyt;
 yyALLOC (yyt, Tree_NodeSize [kDontCareExpr])
 yyt->Kind = kDontCareExpr;
 yyt->yyHead.yyMark = 0;
 yyt->DontCareExpr.position = pposition;
 beginPtrContext(yyt->DontCareExpr.context)
 beginbool(yyt->DontCareExpr.skip)
 beginPtrType(yyt->DontCareExpr.expectedType)
 beginbool(yyt->DontCareExpr.allowChannels)
 beginExprAttributes(yyt->DontCareExpr.attributes)
 return yyt;
}

tTree mAggregateConsExpr
# if defined __STDC__ | defined __cplusplus
(tPosition pposition, tIdent pident, tTree pExprLists)
# else
(pposition, pident, pExprLists)
tPosition pposition;
tIdent pident;
tTree pExprLists;
# endif
{
 register tTree yyt;
 yyALLOC (yyt, Tree_NodeSize [kAggregateConsExpr])
 yyt->Kind = kAggregateConsExpr;
 yyt->yyHead.yyMark = 0;
 yyt->AggregateConsExpr.position = pposition;
 beginPtrContext(yyt->AggregateConsExpr.context)
 beginbool(yyt->AggregateConsExpr.skip)
 beginPtrType(yyt->AggregateConsExpr.expectedType)
 beginbool(yyt->AggregateConsExpr.allowChannels)
 beginExprAttributes(yyt->AggregateConsExpr.attributes)
 yyt->AggregateConsExpr.ident = pident;
 yyt->AggregateConsExpr.ExprLists = pExprLists;
 beginPtrType(yyt->AggregateConsExpr.actualType)
 return yyt;
}

tTree mNamedEnumElemExpr
# if defined __STDC__ | defined __cplusplus
(tPosition pposition, tIdent ptypeName, tIdent pelemName)
# else
(pposition, ptypeName, pelemName)
tPosition pposition;
tIdent ptypeName;
tIdent pelemName;
# endif
{
 register tTree yyt;
 yyALLOC (yyt, Tree_NodeSize [kNamedEnumElemExpr])
 yyt->Kind = kNamedEnumElemExpr;
 yyt->yyHead.yyMark = 0;
 yyt->NamedEnumElemExpr.position = pposition;
 beginPtrContext(yyt->NamedEnumElemExpr.context)
 beginbool(yyt->NamedEnumElemExpr.skip)
 beginPtrType(yyt->NamedEnumElemExpr.expectedType)
 beginbool(yyt->NamedEnumElemExpr.allowChannels)
 beginExprAttributes(yyt->NamedEnumElemExpr.attributes)
 yyt->NamedEnumElemExpr.typeName = ptypeName;
 yyt->NamedEnumElemExpr.elemName = pelemName;
 return yyt;
}

tTree mUnaryExpr
# if defined __STDC__ | defined __cplusplus
(tPosition pposition, Operators poperation, tTree pExpr)
# else
(pposition, poperation, pExpr)
tPosition pposition;
Operators poperation;
tTree pExpr;
# endif
{
 register tTree yyt;
 yyALLOC (yyt, Tree_NodeSize [kUnaryExpr])
 yyt->Kind = kUnaryExpr;
 yyt->yyHead.yyMark = 0;
 yyt->UnaryExpr.position = pposition;
 beginPtrContext(yyt->UnaryExpr.context)
 beginbool(yyt->UnaryExpr.skip)
 beginPtrType(yyt->UnaryExpr.expectedType)
 beginbool(yyt->UnaryExpr.allowChannels)
 beginExprAttributes(yyt->UnaryExpr.attributes)
 yyt->UnaryExpr.operation = poperation;
 yyt->UnaryExpr.Expr = pExpr;
 return yyt;
}

tTree mBinaryExpr
# if defined __STDC__ | defined __cplusplus
(tPosition pposition, Operators poperation, tTree pLeft, tTree pRight)
# else
(pposition, poperation, pLeft, pRight)
tPosition pposition;
Operators poperation;
tTree pLeft;
tTree pRight;
# endif
{
 register tTree yyt;
 yyALLOC (yyt, Tree_NodeSize [kBinaryExpr])
 yyt->Kind = kBinaryExpr;
 yyt->yyHead.yyMark = 0;
 yyt->BinaryExpr.position = pposition;
 beginPtrContext(yyt->BinaryExpr.context)
 beginbool(yyt->BinaryExpr.skip)
 beginPtrType(yyt->BinaryExpr.expectedType)
 beginbool(yyt->BinaryExpr.allowChannels)
 beginExprAttributes(yyt->BinaryExpr.attributes)
 yyt->BinaryExpr.operation = poperation;
 yyt->BinaryExpr.Left = pLeft;
 yyt->BinaryExpr.Right = pRight;
 return yyt;
}

tTree mRecordElemExtractExpr
# if defined __STDC__ | defined __cplusplus
(tPosition pposition, tTree pExpr, tIdent pident)
# else
(pposition, pExpr, pident)
tPosition pposition;
tTree pExpr;
tIdent pident;
# endif
{
 register tTree yyt;
 yyALLOC (yyt, Tree_NodeSize [kRecordElemExtractExpr])
 yyt->Kind = kRecordElemExtractExpr;
 yyt->yyHead.yyMark = 0;
 yyt->RecordElemExtractExpr.position = pposition;
 beginPtrContext(yyt->RecordElemExtractExpr.context)
 beginbool(yyt->RecordElemExtractExpr.skip)
 beginPtrType(yyt->RecordElemExtractExpr.expectedType)
 beginbool(yyt->RecordElemExtractExpr.allowChannels)
 beginExprAttributes(yyt->RecordElemExtractExpr.attributes)
 yyt->RecordElemExtractExpr.Expr = pExpr;
 yyt->RecordElemExtractExpr.ident = pident;
 return yyt;
}

tTree mArrayExtractExpr
# if defined __STDC__ | defined __cplusplus
(tPosition pposition, tTree pArray, tTree pSubscript)
# else
(pposition, pArray, pSubscript)
tPosition pposition;
tTree pArray;
tTree pSubscript;
# endif
{
 register tTree yyt;
 yyALLOC (yyt, Tree_NodeSize [kArrayExtractExpr])
 yyt->Kind = kArrayExtractExpr;
 yyt->yyHead.yyMark = 0;
 yyt->ArrayExtractExpr.position = pposition;
 beginPtrContext(yyt->ArrayExtractExpr.context)
 beginbool(yyt->ArrayExtractExpr.skip)
 beginPtrType(yyt->ArrayExtractExpr.expectedType)
 beginbool(yyt->ArrayExtractExpr.allowChannels)
 beginExprAttributes(yyt->ArrayExtractExpr.attributes)
 yyt->ArrayExtractExpr.Array = pArray;
 yyt->ArrayExtractExpr.Subscript = pSubscript;
 return yyt;
}

tTree mArraySliceExpr
# if defined __STDC__ | defined __cplusplus
(tPosition pposition, tTree pExpr, tTree pRange)
# else
(pposition, pExpr, pRange)
tPosition pposition;
tTree pExpr;
tTree pRange;
# endif
{
 register tTree yyt;
 yyALLOC (yyt, Tree_NodeSize [kArraySliceExpr])
 yyt->Kind = kArraySliceExpr;
 yyt->yyHead.yyMark = 0;
 yyt->ArraySliceExpr.position = pposition;
 beginPtrContext(yyt->ArraySliceExpr.context)
 beginbool(yyt->ArraySliceExpr.skip)
 beginPtrType(yyt->ArraySliceExpr.expectedType)
 beginbool(yyt->ArraySliceExpr.allowChannels)
 beginExprAttributes(yyt->ArraySliceExpr.attributes)
 yyt->ArraySliceExpr.Expr = pExpr;
 yyt->ArraySliceExpr.Range = pRange;
 return yyt;
}

tTree mAsExpr
# if defined __STDC__ | defined __cplusplus
(tPosition pposition, tTree pExpr, tTree pAType)
# else
(pposition, pExpr, pAType)
tPosition pposition;
tTree pExpr;
tTree pAType;
# endif
{
 register tTree yyt;
 yyALLOC (yyt, Tree_NodeSize [kAsExpr])
 yyt->Kind = kAsExpr;
 yyt->yyHead.yyMark = 0;
 yyt->AsExpr.position = pposition;
 beginPtrContext(yyt->AsExpr.context)
 beginbool(yyt->AsExpr.skip)
 beginPtrType(yyt->AsExpr.expectedType)
 beginbool(yyt->AsExpr.allowChannels)
 beginExprAttributes(yyt->AsExpr.attributes)
 yyt->AsExpr.Expr = pExpr;
 yyt->AsExpr.AType = pAType;
 return yyt;
}

tTree mBitArrayCastExpr
# if defined __STDC__ | defined __cplusplus
(tPosition pposition, tTree pExpr)
# else
(pposition, pExpr)
tPosition pposition;
tTree pExpr;
# endif
{
 register tTree yyt;
 yyALLOC (yyt, Tree_NodeSize [kBitArrayCastExpr])
 yyt->Kind = kBitArrayCastExpr;
 yyt->yyHead.yyMark = 0;
 yyt->BitArrayCastExpr.position = pposition;
 beginPtrContext(yyt->BitArrayCastExpr.context)
 beginbool(yyt->BitArrayCastExpr.skip)
 beginPtrType(yyt->BitArrayCastExpr.expectedType)
 beginbool(yyt->BitArrayCastExpr.allowChannels)
 beginExprAttributes(yyt->BitArrayCastExpr.attributes)
 yyt->BitArrayCastExpr.Expr = pExpr;
 return yyt;
}

tTree mLetExpr
# if defined __STDC__ | defined __cplusplus
(tPosition pposition, tTree pValDecls, tTree pExpr)
# else
(pposition, pValDecls, pExpr)
tPosition pposition;
tTree pValDecls;
tTree pExpr;
# endif
{
 register tTree yyt;
 yyALLOC (yyt, Tree_NodeSize [kLetExpr])
 yyt->Kind = kLetExpr;
 yyt->yyHead.yyMark = 0;
 yyt->LetExpr.position = pposition;
 beginPtrContext(yyt->LetExpr.context)
 beginbool(yyt->LetExpr.skip)
 beginPtrType(yyt->LetExpr.expectedType)
 beginbool(yyt->LetExpr.allowChannels)
 beginExprAttributes(yyt->LetExpr.attributes)
 yyt->LetExpr.ValDecls = pValDecls;
 yyt->LetExpr.Expr = pExpr;
 return yyt;
}

tTree mFunctionCallExpr
# if defined __STDC__ | defined __cplusplus
(tPosition pposition, tIdent pident, tTree pFunctionParams)
# else
(pposition, pident, pFunctionParams)
tPosition pposition;
tIdent pident;
tTree pFunctionParams;
# endif
{
 register tTree yyt;
 yyALLOC (yyt, Tree_NodeSize [kFunctionCallExpr])
 yyt->Kind = kFunctionCallExpr;
 yyt->yyHead.yyMark = 0;
 yyt->FunctionCallExpr.position = pposition;
 beginPtrContext(yyt->FunctionCallExpr.context)
 beginbool(yyt->FunctionCallExpr.skip)
 beginPtrType(yyt->FunctionCallExpr.expectedType)
 beginbool(yyt->FunctionCallExpr.allowChannels)
 beginExprAttributes(yyt->FunctionCallExpr.attributes)
 yyt->FunctionCallExpr.ident = pident;
 yyt->FunctionCallExpr.FunctionParams = pFunctionParams;
 return yyt;
}

tTree mSizeofExpr
# if defined __STDC__ | defined __cplusplus
(tPosition pposition, tIdent pident)
# else
(pposition, pident)
tPosition pposition;
tIdent pident;
# endif
{
 register tTree yyt;
 yyALLOC (yyt, Tree_NodeSize [kSizeofExpr])
 yyt->Kind = kSizeofExpr;
 yyt->yyHead.yyMark = 0;
 yyt->SizeofExpr.position = pposition;
 beginPtrContext(yyt->SizeofExpr.context)
 beginbool(yyt->SizeofExpr.skip)
 beginPtrType(yyt->SizeofExpr.expectedType)
 beginbool(yyt->SizeofExpr.allowChannels)
 beginExprAttributes(yyt->SizeofExpr.attributes)
 yyt->SizeofExpr.ident = pident;
 return yyt;
}

tTree mArrayAppendExpr
# if defined __STDC__ | defined __cplusplus
(tPosition pposition, tTree pLeft, tTree pRight)
# else
(pposition, pLeft, pRight)
tPosition pposition;
tTree pLeft;
tTree pRight;
# endif
{
 register tTree yyt;
 yyALLOC (yyt, Tree_NodeSize [kArrayAppendExpr])
 yyt->Kind = kArrayAppendExpr;
 yyt->yyHead.yyMark = 0;
 yyt->ArrayAppendExpr.position = pposition;
 beginPtrContext(yyt->ArrayAppendExpr.context)
 beginbool(yyt->ArrayAppendExpr.skip)
 beginPtrType(yyt->ArrayAppendExpr.expectedType)
 beginbool(yyt->ArrayAppendExpr.allowChannels)
 beginExprAttributes(yyt->ArrayAppendExpr.attributes)
 yyt->ArrayAppendExpr.Left = pLeft;
 yyt->ArrayAppendExpr.Right = pRight;
 return yyt;
}

tTree mPrimedExpr
# if defined __STDC__ | defined __cplusplus
(tPosition pposition, ExprAttributes presults)
# else
(pposition, presults)
tPosition pposition;
ExprAttributes presults;
# endif
{
 register tTree yyt;
 yyALLOC (yyt, Tree_NodeSize [kPrimedExpr])
 yyt->Kind = kPrimedExpr;
 yyt->yyHead.yyMark = 0;
 yyt->PrimedExpr.position = pposition;
 beginPtrContext(yyt->PrimedExpr.context)
 beginbool(yyt->PrimedExpr.skip)
 beginPtrType(yyt->PrimedExpr.expectedType)
 beginbool(yyt->PrimedExpr.allowChannels)
 beginExprAttributes(yyt->PrimedExpr.attributes)
 yyt->PrimedExpr.results = presults;
 return yyt;
}

tTree mFormalPorts
# if defined __STDC__ | defined __cplusplus
(tPosition pposition)
# else
(pposition)
tPosition pposition;
# endif
{
 register tTree yyt;
 yyALLOC (yyt, Tree_NodeSize [kFormalPorts])
 yyt->Kind = kFormalPorts;
 yyt->yyHead.yyMark = 0;
 yyt->FormalPorts.position = pposition;
 beginPtrContext(yyt->FormalPorts.context)
 beginbool(yyt->FormalPorts.skip)
 beginProcedureArgsType(yyt->FormalPorts.portsType)
 beginbool(yyt->FormalPorts.inConditionalPorts)
 beginPtrInstanceList(yyt->FormalPorts.ports)
 beginunsigned(yyt->FormalPorts.portCount)
 beginPtrInstanceList(yyt->FormalPorts.extraPortInstances)
 beginbool(yyt->FormalPorts.hasParameters)
 return yyt;
}

tTree mNullFormalPorts
# if defined __STDC__ | defined __cplusplus
(tPosition pposition)
# else
(pposition)
tPosition pposition;
# endif
{
 register tTree yyt;
 yyALLOC (yyt, Tree_NodeSize [kNullFormalPorts])
 yyt->Kind = kNullFormalPorts;
 yyt->yyHead.yyMark = 0;
 yyt->NullFormalPorts.position = pposition;
 beginPtrContext(yyt->NullFormalPorts.context)
 beginbool(yyt->NullFormalPorts.skip)
 beginProcedureArgsType(yyt->NullFormalPorts.portsType)
 beginbool(yyt->NullFormalPorts.inConditionalPorts)
 beginPtrInstanceList(yyt->NullFormalPorts.ports)
 beginunsigned(yyt->NullFormalPorts.portCount)
 beginPtrInstanceList(yyt->NullFormalPorts.extraPortInstances)
 beginbool(yyt->NullFormalPorts.hasParameters)
 return yyt;
}

tTree mFormalPort
# if defined __STDC__ | defined __cplusplus
(tPosition pposition, tTree pnext)
# else
(pposition, pnext)
tPosition pposition;
tTree pnext;
# endif
{
 register tTree yyt;
 yyALLOC (yyt, Tree_NodeSize [kFormalPort])
 yyt->Kind = kFormalPort;
 yyt->yyHead.yyMark = 0;
 yyt->FormalPort.position = pposition;
 beginPtrContext(yyt->FormalPort.context)
 beginbool(yyt->FormalPort.skip)
 beginProcedureArgsType(yyt->FormalPort.portsType)
 beginbool(yyt->FormalPort.inConditionalPorts)
 beginPtrInstanceList(yyt->FormalPort.ports)
 beginunsigned(yyt->FormalPort.portCount)
 beginPtrInstanceList(yyt->FormalPort.extraPortInstances)
 beginbool(yyt->FormalPort.hasParameters)
 yyt->FormalPort.next = pnext;
 return yyt;
}

tTree mValuePort
# if defined __STDC__ | defined __cplusplus
(tPosition pposition, tTree pnext, tTree pIdents, tTree pAType)
# else
(pposition, pnext, pIdents, pAType)
tPosition pposition;
tTree pnext;
tTree pIdents;
tTree pAType;
# endif
{
 register tTree yyt;
 yyALLOC (yyt, Tree_NodeSize [kValuePort])
 yyt->Kind = kValuePort;
 yyt->yyHead.yyMark = 0;
 yyt->ValuePort.position = pposition;
 beginPtrContext(yyt->ValuePort.context)
 beginbool(yyt->ValuePort.skip)
 beginProcedureArgsType(yyt->ValuePort.portsType)
 beginbool(yyt->ValuePort.inConditionalPorts)
 beginPtrInstanceList(yyt->ValuePort.ports)
 beginunsigned(yyt->ValuePort.portCount)
 beginPtrInstanceList(yyt->ValuePort.extraPortInstances)
 beginbool(yyt->ValuePort.hasParameters)
 yyt->ValuePort.next = pnext;
 yyt->ValuePort.Idents = pIdents;
 yyt->ValuePort.AType = pAType;
 return yyt;
}

tTree mParamPort
# if defined __STDC__ | defined __cplusplus
(tPosition pposition, tTree pnext, tTree pIdents, tTree pAType)
# else
(pposition, pnext, pIdents, pAType)
tPosition pposition;
tTree pnext;
tTree pIdents;
tTree pAType;
# endif
{
 register tTree yyt;
 yyALLOC (yyt, Tree_NodeSize [kParamPort])
 yyt->Kind = kParamPort;
 yyt->yyHead.yyMark = 0;
 yyt->ParamPort.position = pposition;
 beginPtrContext(yyt->ParamPort.context)
 beginbool(yyt->ParamPort.skip)
 beginProcedureArgsType(yyt->ParamPort.portsType)
 beginbool(yyt->ParamPort.inConditionalPorts)
 beginPtrInstanceList(yyt->ParamPort.ports)
 beginunsigned(yyt->ParamPort.portCount)
 beginPtrInstanceList(yyt->ParamPort.extraPortInstances)
 beginbool(yyt->ParamPort.hasParameters)
 yyt->ParamPort.next = pnext;
 yyt->ParamPort.Idents = pIdents;
 yyt->ParamPort.AType = pAType;
 return yyt;
}

tTree mTypeParamPort
# if defined __STDC__ | defined __cplusplus
(tPosition pposition, tTree pnext, tTree pIdents)
# else
(pposition, pnext, pIdents)
tPosition pposition;
tTree pnext;
tTree pIdents;
# endif
{
 register tTree yyt;
 yyALLOC (yyt, Tree_NodeSize [kTypeParamPort])
 yyt->Kind = kTypeParamPort;
 yyt->yyHead.yyMark = 0;
 yyt->TypeParamPort.position = pposition;
 beginPtrContext(yyt->TypeParamPort.context)
 beginbool(yyt->TypeParamPort.skip)
 beginProcedureArgsType(yyt->TypeParamPort.portsType)
 beginbool(yyt->TypeParamPort.inConditionalPorts)
 beginPtrInstanceList(yyt->TypeParamPort.ports)
 beginunsigned(yyt->TypeParamPort.portCount)
 beginPtrInstanceList(yyt->TypeParamPort.extraPortInstances)
 beginbool(yyt->TypeParamPort.hasParameters)
 yyt->TypeParamPort.next = pnext;
 yyt->TypeParamPort.Idents = pIdents;
 return yyt;
}

tTree mChannelPort
# if defined __STDC__ | defined __cplusplus
(tPosition pposition, tTree pnext, tTree pIdents, tTree pAType, PortSense pportSense, bool pisOutput, PtrLispList poptions)
# else
(pposition, pnext, pIdents, pAType, pportSense, pisOutput, poptions)
tPosition pposition;
tTree pnext;
tTree pIdents;
tTree pAType;
PortSense pportSense;
bool pisOutput;
PtrLispList poptions;
# endif
{
 register tTree yyt;
 yyALLOC (yyt, Tree_NodeSize [kChannelPort])
 yyt->Kind = kChannelPort;
 yyt->yyHead.yyMark = 0;
 yyt->ChannelPort.position = pposition;
 beginPtrContext(yyt->ChannelPort.context)
 beginbool(yyt->ChannelPort.skip)
 beginProcedureArgsType(yyt->ChannelPort.portsType)
 beginbool(yyt->ChannelPort.inConditionalPorts)
 beginPtrInstanceList(yyt->ChannelPort.ports)
 beginunsigned(yyt->ChannelPort.portCount)
 beginPtrInstanceList(yyt->ChannelPort.extraPortInstances)
 beginbool(yyt->ChannelPort.hasParameters)
 yyt->ChannelPort.next = pnext;
 yyt->ChannelPort.Idents = pIdents;
 yyt->ChannelPort.AType = pAType;
 yyt->ChannelPort.portSense = pportSense;
 yyt->ChannelPort.isOutput = pisOutput;
 yyt->ChannelPort.options = poptions;
 return yyt;
}

tTree mChannelPortArray
# if defined __STDC__ | defined __cplusplus
(tPosition pposition, tTree pnext, tTree pIdents, tTree pAType, PortSense pportSense, bool pisOutput, tTree pRange, PtrLispList poptions)
# else
(pposition, pnext, pIdents, pAType, pportSense, pisOutput, pRange, poptions)
tPosition pposition;
tTree pnext;
tTree pIdents;
tTree pAType;
PortSense pportSense;
bool pisOutput;
tTree pRange;
PtrLispList poptions;
# endif
{
 register tTree yyt;
 yyALLOC (yyt, Tree_NodeSize [kChannelPortArray])
 yyt->Kind = kChannelPortArray;
 yyt->yyHead.yyMark = 0;
 yyt->ChannelPortArray.position = pposition;
 beginPtrContext(yyt->ChannelPortArray.context)
 beginbool(yyt->ChannelPortArray.skip)
 beginProcedureArgsType(yyt->ChannelPortArray.portsType)
 beginbool(yyt->ChannelPortArray.inConditionalPorts)
 beginPtrInstanceList(yyt->ChannelPortArray.ports)
 beginunsigned(yyt->ChannelPortArray.portCount)
 beginPtrInstanceList(yyt->ChannelPortArray.extraPortInstances)
 beginbool(yyt->ChannelPortArray.hasParameters)
 yyt->ChannelPortArray.next = pnext;
 yyt->ChannelPortArray.Idents = pIdents;
 yyt->ChannelPortArray.AType = pAType;
 yyt->ChannelPortArray.portSense = pportSense;
 yyt->ChannelPortArray.isOutput = pisOutput;
 yyt->ChannelPortArray.Range = pRange;
 yyt->ChannelPortArray.options = poptions;
 return yyt;
}

tTree mSyncPort
# if defined __STDC__ | defined __cplusplus
(tPosition pposition, tTree pnext, tTree pIdents, PortSense pportSense, PtrLispList poptions)
# else
(pposition, pnext, pIdents, pportSense, poptions)
tPosition pposition;
tTree pnext;
tTree pIdents;
PortSense pportSense;
PtrLispList poptions;
# endif
{
 register tTree yyt;
 yyALLOC (yyt, Tree_NodeSize [kSyncPort])
 yyt->Kind = kSyncPort;
 yyt->yyHead.yyMark = 0;
 yyt->SyncPort.position = pposition;
 beginPtrContext(yyt->SyncPort.context)
 beginbool(yyt->SyncPort.skip)
 beginProcedureArgsType(yyt->SyncPort.portsType)
 beginbool(yyt->SyncPort.inConditionalPorts)
 beginPtrInstanceList(yyt->SyncPort.ports)
 beginunsigned(yyt->SyncPort.portCount)
 beginPtrInstanceList(yyt->SyncPort.extraPortInstances)
 beginbool(yyt->SyncPort.hasParameters)
 yyt->SyncPort.next = pnext;
 yyt->SyncPort.Idents = pIdents;
 yyt->SyncPort.portSense = pportSense;
 yyt->SyncPort.options = poptions;
 return yyt;
}

tTree mSyncPortArray
# if defined __STDC__ | defined __cplusplus
(tPosition pposition, tTree pnext, tTree pIdents, PortSense pportSense, tTree pRange, PtrLispList poptions)
# else
(pposition, pnext, pIdents, pportSense, pRange, poptions)
tPosition pposition;
tTree pnext;
tTree pIdents;
PortSense pportSense;
tTree pRange;
PtrLispList poptions;
# endif
{
 register tTree yyt;
 yyALLOC (yyt, Tree_NodeSize [kSyncPortArray])
 yyt->Kind = kSyncPortArray;
 yyt->yyHead.yyMark = 0;
 yyt->SyncPortArray.position = pposition;
 beginPtrContext(yyt->SyncPortArray.context)
 beginbool(yyt->SyncPortArray.skip)
 beginProcedureArgsType(yyt->SyncPortArray.portsType)
 beginbool(yyt->SyncPortArray.inConditionalPorts)
 beginPtrInstanceList(yyt->SyncPortArray.ports)
 beginunsigned(yyt->SyncPortArray.portCount)
 beginPtrInstanceList(yyt->SyncPortArray.extraPortInstances)
 beginbool(yyt->SyncPortArray.hasParameters)
 yyt->SyncPortArray.next = pnext;
 yyt->SyncPortArray.Idents = pIdents;
 yyt->SyncPortArray.portSense = pportSense;
 yyt->SyncPortArray.Range = pRange;
 yyt->SyncPortArray.options = poptions;
 return yyt;
}

tTree mIfPorts
# if defined __STDC__ | defined __cplusplus
(tPosition pposition, tTree pnext, tTree pPortGuards)
# else
(pposition, pnext, pPortGuards)
tPosition pposition;
tTree pnext;
tTree pPortGuards;
# endif
{
 register tTree yyt;
 yyALLOC (yyt, Tree_NodeSize [kIfPorts])
 yyt->Kind = kIfPorts;
 yyt->yyHead.yyMark = 0;
 yyt->IfPorts.position = pposition;
 beginPtrContext(yyt->IfPorts.context)
 beginbool(yyt->IfPorts.skip)
 beginProcedureArgsType(yyt->IfPorts.portsType)
 beginbool(yyt->IfPorts.inConditionalPorts)
 beginPtrInstanceList(yyt->IfPorts.ports)
 beginunsigned(yyt->IfPorts.portCount)
 beginPtrInstanceList(yyt->IfPorts.extraPortInstances)
 beginbool(yyt->IfPorts.hasParameters)
 yyt->IfPorts.next = pnext;
 yyt->IfPorts.PortGuards = pPortGuards;
 return yyt;
}

tTree mIfElsePorts
# if defined __STDC__ | defined __cplusplus
(tPosition pposition, tTree pnext, tTree pPortGuards, tTree pFormalPorts)
# else
(pposition, pnext, pPortGuards, pFormalPorts)
tPosition pposition;
tTree pnext;
tTree pPortGuards;
tTree pFormalPorts;
# endif
{
 register tTree yyt;
 yyALLOC (yyt, Tree_NodeSize [kIfElsePorts])
 yyt->Kind = kIfElsePorts;
 yyt->yyHead.yyMark = 0;
 yyt->IfElsePorts.position = pposition;
 beginPtrContext(yyt->IfElsePorts.context)
 beginbool(yyt->IfElsePorts.skip)
 beginProcedureArgsType(yyt->IfElsePorts.portsType)
 beginbool(yyt->IfElsePorts.inConditionalPorts)
 beginPtrInstanceList(yyt->IfElsePorts.ports)
 beginunsigned(yyt->IfElsePorts.portCount)
 beginPtrInstanceList(yyt->IfElsePorts.extraPortInstances)
 beginbool(yyt->IfElsePorts.hasParameters)
 yyt->IfElsePorts.next = pnext;
 yyt->IfElsePorts.PortGuards = pPortGuards;
 yyt->IfElsePorts.FormalPorts = pFormalPorts;
 return yyt;
}

tTree mBlock
# if defined __STDC__ | defined __cplusplus
(tPosition pposition, tTree pDecls, tTree pCommand)
# else
(pposition, pDecls, pCommand)
tPosition pposition;
tTree pDecls;
tTree pCommand;
# endif
{
 register tTree yyt;
 yyALLOC (yyt, Tree_NodeSize [kBlock])
 yyt->Kind = kBlock;
 yyt->yyHead.yyMark = 0;
 yyt->Block.position = pposition;
 yyt->Block.Decls = pDecls;
 yyt->Block.Command = pCommand;
 beginPtrContext(yyt->Block.context)
 beginbool(yyt->Block.skip)
 beginCommandAttributes(yyt->Block.attributes)
 return yyt;
}

tTree mParameters
# if defined __STDC__ | defined __cplusplus
(tPosition pposition)
# else
(pposition)
tPosition pposition;
# endif
{
 register tTree yyt;
 yyALLOC (yyt, Tree_NodeSize [kParameters])
 yyt->Kind = kParameters;
 yyt->yyHead.yyMark = 0;
 yyt->Parameters.position = pposition;
 beginPtrContext(yyt->Parameters.context)
 beginbool(yyt->Parameters.skip)
 beginunsigned(yyt->Parameters.paramNoIn)
 beginunsigned(yyt->Parameters.paramNoOut)
 beginPtrComponentParameterList(yyt->Parameters.parameters)
 beginPtrComponent(yyt->Parameters.component)
 return yyt;
}

tTree mNullParameters
# if defined __STDC__ | defined __cplusplus
(tPosition pposition)
# else
(pposition)
tPosition pposition;
# endif
{
 register tTree yyt;
 yyALLOC (yyt, Tree_NodeSize [kNullParameters])
 yyt->Kind = kNullParameters;
 yyt->yyHead.yyMark = 0;
 yyt->NullParameters.position = pposition;
 beginPtrContext(yyt->NullParameters.context)
 beginbool(yyt->NullParameters.skip)
 beginunsigned(yyt->NullParameters.paramNoIn)
 beginunsigned(yyt->NullParameters.paramNoOut)
 beginPtrComponentParameterList(yyt->NullParameters.parameters)
 beginPtrComponent(yyt->NullParameters.component)
 return yyt;
}

tTree mParameter
# if defined __STDC__ | defined __cplusplus
(tPosition pposition, tTree pnext)
# else
(pposition, pnext)
tPosition pposition;
tTree pnext;
# endif
{
 register tTree yyt;
 yyALLOC (yyt, Tree_NodeSize [kParameter])
 yyt->Kind = kParameter;
 yyt->yyHead.yyMark = 0;
 yyt->Parameter.position = pposition;
 beginPtrContext(yyt->Parameter.context)
 beginbool(yyt->Parameter.skip)
 beginunsigned(yyt->Parameter.paramNoIn)
 beginunsigned(yyt->Parameter.paramNoOut)
 beginPtrComponentParameterList(yyt->Parameter.parameters)
 beginPtrComponent(yyt->Parameter.component)
 yyt->Parameter.next = pnext;
 return yyt;
}

tTree mNumberParameter
# if defined __STDC__ | defined __cplusplus
(tPosition pposition, tTree pnext, PtrMP_INT pliteral)
# else
(pposition, pnext, pliteral)
tPosition pposition;
tTree pnext;
PtrMP_INT pliteral;
# endif
{
 register tTree yyt;
 yyALLOC (yyt, Tree_NodeSize [kNumberParameter])
 yyt->Kind = kNumberParameter;
 yyt->yyHead.yyMark = 0;
 yyt->NumberParameter.position = pposition;
 beginPtrContext(yyt->NumberParameter.context)
 beginbool(yyt->NumberParameter.skip)
 beginunsigned(yyt->NumberParameter.paramNoIn)
 beginunsigned(yyt->NumberParameter.paramNoOut)
 beginPtrComponentParameterList(yyt->NumberParameter.parameters)
 beginPtrComponent(yyt->NumberParameter.component)
 yyt->NumberParameter.next = pnext;
 yyt->NumberParameter.literal = pliteral;
 return yyt;
}

tTree mStringParameter
# if defined __STDC__ | defined __cplusplus
(tPosition pposition, tTree pnext, tIdent pident)
# else
(pposition, pnext, pident)
tPosition pposition;
tTree pnext;
tIdent pident;
# endif
{
 register tTree yyt;
 yyALLOC (yyt, Tree_NodeSize [kStringParameter])
 yyt->Kind = kStringParameter;
 yyt->yyHead.yyMark = 0;
 yyt->StringParameter.position = pposition;
 beginPtrContext(yyt->StringParameter.context)
 beginbool(yyt->StringParameter.skip)
 beginunsigned(yyt->StringParameter.paramNoIn)
 beginunsigned(yyt->StringParameter.paramNoOut)
 beginPtrComponentParameterList(yyt->StringParameter.parameters)
 beginPtrComponent(yyt->StringParameter.component)
 yyt->StringParameter.next = pnext;
 yyt->StringParameter.ident = pident;
 return yyt;
}

tTree mTypeParameter
# if defined __STDC__ | defined __cplusplus
(tPosition pposition, tTree pnext, tTree pAType)
# else
(pposition, pnext, pAType)
tPosition pposition;
tTree pnext;
tTree pAType;
# endif
{
 register tTree yyt;
 yyALLOC (yyt, Tree_NodeSize [kTypeParameter])
 yyt->Kind = kTypeParameter;
 yyt->yyHead.yyMark = 0;
 yyt->TypeParameter.position = pposition;
 beginPtrContext(yyt->TypeParameter.context)
 beginbool(yyt->TypeParameter.skip)
 beginunsigned(yyt->TypeParameter.paramNoIn)
 beginunsigned(yyt->TypeParameter.paramNoOut)
 beginPtrComponentParameterList(yyt->TypeParameter.parameters)
 beginPtrComponent(yyt->TypeParameter.component)
 yyt->TypeParameter.next = pnext;
 yyt->TypeParameter.AType = pAType;
 return yyt;
}

tTree mBreezeParameters
# if defined __STDC__ | defined __cplusplus
(tPosition pposition)
# else
(pposition)
tPosition pposition;
# endif
{
 register tTree yyt;
 yyALLOC (yyt, Tree_NodeSize [kBreezeParameters])
 yyt->Kind = kBreezeParameters;
 yyt->yyHead.yyMark = 0;
 yyt->BreezeParameters.position = pposition;
 beginPtrContext(yyt->BreezeParameters.context)
 beginbool(yyt->BreezeParameters.skip)
 beginPtrComponentParameterList(yyt->BreezeParameters.parameters)
 return yyt;
}

tTree mNullBreezeParameters
# if defined __STDC__ | defined __cplusplus
(tPosition pposition)
# else
(pposition)
tPosition pposition;
# endif
{
 register tTree yyt;
 yyALLOC (yyt, Tree_NodeSize [kNullBreezeParameters])
 yyt->Kind = kNullBreezeParameters;
 yyt->yyHead.yyMark = 0;
 yyt->NullBreezeParameters.position = pposition;
 beginPtrContext(yyt->NullBreezeParameters.context)
 beginbool(yyt->NullBreezeParameters.skip)
 beginPtrComponentParameterList(yyt->NullBreezeParameters.parameters)
 return yyt;
}

tTree mBreezeParameter
# if defined __STDC__ | defined __cplusplus
(tPosition pposition, tTree pnext)
# else
(pposition, pnext)
tPosition pposition;
tTree pnext;
# endif
{
 register tTree yyt;
 yyALLOC (yyt, Tree_NodeSize [kBreezeParameter])
 yyt->Kind = kBreezeParameter;
 yyt->yyHead.yyMark = 0;
 yyt->BreezeParameter.position = pposition;
 beginPtrContext(yyt->BreezeParameter.context)
 beginbool(yyt->BreezeParameter.skip)
 beginPtrComponentParameterList(yyt->BreezeParameter.parameters)
 yyt->BreezeParameter.next = pnext;
 return yyt;
}

tTree mBreezeExprParameter
# if defined __STDC__ | defined __cplusplus
(tPosition pposition, tTree pnext, tIdent pident, tTree pAType)
# else
(pposition, pnext, pident, pAType)
tPosition pposition;
tTree pnext;
tIdent pident;
tTree pAType;
# endif
{
 register tTree yyt;
 yyALLOC (yyt, Tree_NodeSize [kBreezeExprParameter])
 yyt->Kind = kBreezeExprParameter;
 yyt->yyHead.yyMark = 0;
 yyt->BreezeExprParameter.position = pposition;
 beginPtrContext(yyt->BreezeExprParameter.context)
 beginbool(yyt->BreezeExprParameter.skip)
 beginPtrComponentParameterList(yyt->BreezeExprParameter.parameters)
 yyt->BreezeExprParameter.next = pnext;
 yyt->BreezeExprParameter.ident = pident;
 yyt->BreezeExprParameter.AType = pAType;
 return yyt;
}

tTree mBreezeTypeParameter
# if defined __STDC__ | defined __cplusplus
(tPosition pposition, tTree pnext, tIdent pident)
# else
(pposition, pnext, pident)
tPosition pposition;
tTree pnext;
tIdent pident;
# endif
{
 register tTree yyt;
 yyALLOC (yyt, Tree_NodeSize [kBreezeTypeParameter])
 yyt->Kind = kBreezeTypeParameter;
 yyt->yyHead.yyMark = 0;
 yyt->BreezeTypeParameter.position = pposition;
 beginPtrContext(yyt->BreezeTypeParameter.context)
 beginbool(yyt->BreezeTypeParameter.skip)
 beginPtrComponentParameterList(yyt->BreezeTypeParameter.parameters)
 yyt->BreezeTypeParameter.next = pnext;
 yyt->BreezeTypeParameter.ident = pident;
 return yyt;
}

tTree mComps
# if defined __STDC__ | defined __cplusplus
(tPosition pposition)
# else
(pposition)
tPosition pposition;
# endif
{
 register tTree yyt;
 yyALLOC (yyt, Tree_NodeSize [kComps])
 yyt->Kind = kComps;
 yyt->yyHead.yyMark = 0;
 yyt->Comps.position = pposition;
 beginPtrContext(yyt->Comps.context)
 beginbool(yyt->Comps.skip)
 beginPtrWireArray(yyt->Comps.channels)
 beginunsigned(yyt->Comps.channelCount)
 beginPtrComponentList(yyt->Comps.componentsIn)
 beginPtrComponentList(yyt->Comps.componentsOut)
 return yyt;
}

tTree mNullComps
# if defined __STDC__ | defined __cplusplus
(tPosition pposition)
# else
(pposition)
tPosition pposition;
# endif
{
 register tTree yyt;
 yyALLOC (yyt, Tree_NodeSize [kNullComps])
 yyt->Kind = kNullComps;
 yyt->yyHead.yyMark = 0;
 yyt->NullComps.position = pposition;
 beginPtrContext(yyt->NullComps.context)
 beginbool(yyt->NullComps.skip)
 beginPtrWireArray(yyt->NullComps.channels)
 beginunsigned(yyt->NullComps.channelCount)
 beginPtrComponentList(yyt->NullComps.componentsIn)
 beginPtrComponentList(yyt->NullComps.componentsOut)
 return yyt;
}

tTree mComp
# if defined __STDC__ | defined __cplusplus
(tPosition pposition, tTree pnext)
# else
(pposition, pnext)
tPosition pposition;
tTree pnext;
# endif
{
 register tTree yyt;
 yyALLOC (yyt, Tree_NodeSize [kComp])
 yyt->Kind = kComp;
 yyt->yyHead.yyMark = 0;
 yyt->Comp.position = pposition;
 beginPtrContext(yyt->Comp.context)
 beginbool(yyt->Comp.skip)
 beginPtrWireArray(yyt->Comp.channels)
 beginunsigned(yyt->Comp.channelCount)
 beginPtrComponentList(yyt->Comp.componentsIn)
 beginPtrComponentList(yyt->Comp.componentsOut)
 yyt->Comp.next = pnext;
 return yyt;
}

tTree mNormalComp
# if defined __STDC__ | defined __cplusplus
(tPosition pposition, tTree pnext, tIdent pident, bool pinternal, tTree pParameters, PtrintList pchannelNos, PtrLispList poptions)
# else
(pposition, pnext, pident, pinternal, pParameters, pchannelNos, poptions)
tPosition pposition;
tTree pnext;
tIdent pident;
bool pinternal;
tTree pParameters;
PtrintList pchannelNos;
PtrLispList poptions;
# endif
{
 register tTree yyt;
 yyALLOC (yyt, Tree_NodeSize [kNormalComp])
 yyt->Kind = kNormalComp;
 yyt->yyHead.yyMark = 0;
 yyt->NormalComp.position = pposition;
 beginPtrContext(yyt->NormalComp.context)
 beginbool(yyt->NormalComp.skip)
 beginPtrWireArray(yyt->NormalComp.channels)
 beginunsigned(yyt->NormalComp.channelCount)
 beginPtrComponentList(yyt->NormalComp.componentsIn)
 beginPtrComponentList(yyt->NormalComp.componentsOut)
 yyt->NormalComp.next = pnext;
 yyt->NormalComp.ident = pident;
 yyt->NormalComp.internal = pinternal;
 yyt->NormalComp.Parameters = pParameters;
 yyt->NormalComp.channelNos = pchannelNos;
 yyt->NormalComp.options = poptions;
 return yyt;
}

tTree mUndeclaredComp
# if defined __STDC__ | defined __cplusplus
(tPosition pposition, tTree pnext, tIdent pident, tTree pParameters, PtrintList pchannelNos, tIdent pcomponentType, tIdent pbaseComponentName, tTree pBreezeParameters, tTree pFormalPorts, PtrLispList poptions)
# else
(pposition, pnext, pident, pParameters, pchannelNos, pcomponentType, pbaseComponentName, pBreezeParameters, pFormalPorts, poptions)
tPosition pposition;
tTree pnext;
tIdent pident;
tTree pParameters;
PtrintList pchannelNos;
tIdent pcomponentType;
tIdent pbaseComponentName;
tTree pBreezeParameters;
tTree pFormalPorts;
PtrLispList poptions;
# endif
{
 register tTree yyt;
 yyALLOC (yyt, Tree_NodeSize [kUndeclaredComp])
 yyt->Kind = kUndeclaredComp;
 yyt->yyHead.yyMark = 0;
 yyt->UndeclaredComp.position = pposition;
 beginPtrContext(yyt->UndeclaredComp.context)
 beginbool(yyt->UndeclaredComp.skip)
 beginPtrWireArray(yyt->UndeclaredComp.channels)
 beginunsigned(yyt->UndeclaredComp.channelCount)
 beginPtrComponentList(yyt->UndeclaredComp.componentsIn)
 beginPtrComponentList(yyt->UndeclaredComp.componentsOut)
 yyt->UndeclaredComp.next = pnext;
 yyt->UndeclaredComp.ident = pident;
 yyt->UndeclaredComp.Parameters = pParameters;
 yyt->UndeclaredComp.channelNos = pchannelNos;
 yyt->UndeclaredComp.componentType = pcomponentType;
 yyt->UndeclaredComp.baseComponentName = pbaseComponentName;
 yyt->UndeclaredComp.BreezeParameters = pBreezeParameters;
 yyt->UndeclaredComp.FormalPorts = pFormalPorts;
 yyt->UndeclaredComp.options = poptions;
 return yyt;
}

tTree mCommand
# if defined __STDC__ | defined __cplusplus
(tPosition pposition)
# else
(pposition)
tPosition pposition;
# endif
{
 register tTree yyt;
 yyALLOC (yyt, Tree_NodeSize [kCommand])
 yyt->Kind = kCommand;
 yyt->yyHead.yyMark = 0;
 yyt->Command.position = pposition;
 beginPtrContext(yyt->Command.context)
 beginbool(yyt->Command.skip)
 beginCommandAttributes(yyt->Command.attributes)
 return yyt;
}

tTree mNullCommand
# if defined __STDC__ | defined __cplusplus
(tPosition pposition)
# else
(pposition)
tPosition pposition;
# endif
{
 register tTree yyt;
 yyALLOC (yyt, Tree_NodeSize [kNullCommand])
 yyt->Kind = kNullCommand;
 yyt->yyHead.yyMark = 0;
 yyt->NullCommand.position = pposition;
 beginPtrContext(yyt->NullCommand.context)
 beginbool(yyt->NullCommand.skip)
 beginCommandAttributes(yyt->NullCommand.attributes)
 return yyt;
}

tTree mContinueCommand
# if defined __STDC__ | defined __cplusplus
(tPosition pposition)
# else
(pposition)
tPosition pposition;
# endif
{
 register tTree yyt;
 yyALLOC (yyt, Tree_NodeSize [kContinueCommand])
 yyt->Kind = kContinueCommand;
 yyt->yyHead.yyMark = 0;
 yyt->ContinueCommand.position = pposition;
 beginPtrContext(yyt->ContinueCommand.context)
 beginbool(yyt->ContinueCommand.skip)
 beginCommandAttributes(yyt->ContinueCommand.attributes)
 return yyt;
}

tTree mHaltCommand
# if defined __STDC__ | defined __cplusplus
(tPosition pposition)
# else
(pposition)
tPosition pposition;
# endif
{
 register tTree yyt;
 yyALLOC (yyt, Tree_NodeSize [kHaltCommand])
 yyt->Kind = kHaltCommand;
 yyt->yyHead.yyMark = 0;
 yyt->HaltCommand.position = pposition;
 beginPtrContext(yyt->HaltCommand.context)
 beginbool(yyt->HaltCommand.skip)
 beginCommandAttributes(yyt->HaltCommand.attributes)
 return yyt;
}

tTree mInputCommand
# if defined __STDC__ | defined __cplusplus
(tPosition pposition, tTree pLHS, tTree pRHS)
# else
(pposition, pLHS, pRHS)
tPosition pposition;
tTree pLHS;
tTree pRHS;
# endif
{
 register tTree yyt;
 yyALLOC (yyt, Tree_NodeSize [kInputCommand])
 yyt->Kind = kInputCommand;
 yyt->yyHead.yyMark = 0;
 yyt->InputCommand.position = pposition;
 beginPtrContext(yyt->InputCommand.context)
 beginbool(yyt->InputCommand.skip)
 beginCommandAttributes(yyt->InputCommand.attributes)
 yyt->InputCommand.LHS = pLHS;
 yyt->InputCommand.RHS = pRHS;
 return yyt;
}

tTree mInputEncloseCommand
# if defined __STDC__ | defined __cplusplus
(tPosition pposition, tTree pLvalueLists, tTree pCommand)
# else
(pposition, pLvalueLists, pCommand)
tPosition pposition;
tTree pLvalueLists;
tTree pCommand;
# endif
{
 register tTree yyt;
 yyALLOC (yyt, Tree_NodeSize [kInputEncloseCommand])
 yyt->Kind = kInputEncloseCommand;
 yyt->yyHead.yyMark = 0;
 yyt->InputEncloseCommand.position = pposition;
 beginPtrContext(yyt->InputEncloseCommand.context)
 beginbool(yyt->InputEncloseCommand.skip)
 beginCommandAttributes(yyt->InputEncloseCommand.attributes)
 yyt->InputEncloseCommand.LvalueLists = pLvalueLists;
 yyt->InputEncloseCommand.Command = pCommand;
 return yyt;
}

tTree mInputEncloseBangCommand
# if defined __STDC__ | defined __cplusplus
(tPosition pposition, tTree pLvalueLists, tTree pCommand)
# else
(pposition, pLvalueLists, pCommand)
tPosition pposition;
tTree pLvalueLists;
tTree pCommand;
# endif
{
 register tTree yyt;
 yyALLOC (yyt, Tree_NodeSize [kInputEncloseBangCommand])
 yyt->Kind = kInputEncloseBangCommand;
 yyt->yyHead.yyMark = 0;
 yyt->InputEncloseBangCommand.position = pposition;
 beginPtrContext(yyt->InputEncloseBangCommand.context)
 beginbool(yyt->InputEncloseBangCommand.skip)
 beginCommandAttributes(yyt->InputEncloseBangCommand.attributes)
 yyt->InputEncloseBangCommand.LvalueLists = pLvalueLists;
 yyt->InputEncloseBangCommand.Command = pCommand;
 return yyt;
}

tTree mOutputCommand
# if defined __STDC__ | defined __cplusplus
(tPosition pposition, tTree pLvalue, tTree pCoercedExpr)
# else
(pposition, pLvalue, pCoercedExpr)
tPosition pposition;
tTree pLvalue;
tTree pCoercedExpr;
# endif
{
 register tTree yyt;
 yyALLOC (yyt, Tree_NodeSize [kOutputCommand])
 yyt->Kind = kOutputCommand;
 yyt->yyHead.yyMark = 0;
 yyt->OutputCommand.position = pposition;
 beginPtrContext(yyt->OutputCommand.context)
 beginbool(yyt->OutputCommand.skip)
 beginCommandAttributes(yyt->OutputCommand.attributes)
 yyt->OutputCommand.Lvalue = pLvalue;
 yyt->OutputCommand.CoercedExpr = pCoercedExpr;
 return yyt;
}

tTree mSyncCommand
# if defined __STDC__ | defined __cplusplus
(tPosition pposition, tTree pLvalue)
# else
(pposition, pLvalue)
tPosition pposition;
tTree pLvalue;
# endif
{
 register tTree yyt;
 yyALLOC (yyt, Tree_NodeSize [kSyncCommand])
 yyt->Kind = kSyncCommand;
 yyt->yyHead.yyMark = 0;
 yyt->SyncCommand.position = pposition;
 beginPtrContext(yyt->SyncCommand.context)
 beginbool(yyt->SyncCommand.skip)
 beginCommandAttributes(yyt->SyncCommand.attributes)
 yyt->SyncCommand.Lvalue = pLvalue;
 return yyt;
}

tTree mAssignmentCommand
# if defined __STDC__ | defined __cplusplus
(tPosition pposition, tTree pLvalue, tTree pCoercedExpr)
# else
(pposition, pLvalue, pCoercedExpr)
tPosition pposition;
tTree pLvalue;
tTree pCoercedExpr;
# endif
{
 register tTree yyt;
 yyALLOC (yyt, Tree_NodeSize [kAssignmentCommand])
 yyt->Kind = kAssignmentCommand;
 yyt->yyHead.yyMark = 0;
 yyt->AssignmentCommand.position = pposition;
 beginPtrContext(yyt->AssignmentCommand.context)
 beginbool(yyt->AssignmentCommand.skip)
 beginCommandAttributes(yyt->AssignmentCommand.attributes)
 yyt->AssignmentCommand.Lvalue = pLvalue;
 yyt->AssignmentCommand.CoercedExpr = pCoercedExpr;
 return yyt;
}

tTree mBlockCommand
# if defined __STDC__ | defined __cplusplus
(tPosition pposition, tTree pBlock)
# else
(pposition, pBlock)
tPosition pposition;
tTree pBlock;
# endif
{
 register tTree yyt;
 yyALLOC (yyt, Tree_NodeSize [kBlockCommand])
 yyt->Kind = kBlockCommand;
 yyt->yyHead.yyMark = 0;
 yyt->BlockCommand.position = pposition;
 beginPtrContext(yyt->BlockCommand.context)
 beginbool(yyt->BlockCommand.skip)
 beginCommandAttributes(yyt->BlockCommand.attributes)
 yyt->BlockCommand.Block = pBlock;
 return yyt;
}

tTree mSequentialCommand
# if defined __STDC__ | defined __cplusplus
(tPosition pposition, tTree pLeft, tTree pRight)
# else
(pposition, pLeft, pRight)
tPosition pposition;
tTree pLeft;
tTree pRight;
# endif
{
 register tTree yyt;
 yyALLOC (yyt, Tree_NodeSize [kSequentialCommand])
 yyt->Kind = kSequentialCommand;
 yyt->yyHead.yyMark = 0;
 yyt->SequentialCommand.position = pposition;
 beginPtrContext(yyt->SequentialCommand.context)
 beginbool(yyt->SequentialCommand.skip)
 beginCommandAttributes(yyt->SequentialCommand.attributes)
 yyt->SequentialCommand.Left = pLeft;
 yyt->SequentialCommand.Right = pRight;
 return yyt;
}

tTree mParallelCommand
# if defined __STDC__ | defined __cplusplus
(tPosition pposition, bool pisPermissive, tTree pLeft, tTree pRight)
# else
(pposition, pisPermissive, pLeft, pRight)
tPosition pposition;
bool pisPermissive;
tTree pLeft;
tTree pRight;
# endif
{
 register tTree yyt;
 yyALLOC (yyt, Tree_NodeSize [kParallelCommand])
 yyt->Kind = kParallelCommand;
 yyt->yyHead.yyMark = 0;
 yyt->ParallelCommand.position = pposition;
 beginPtrContext(yyt->ParallelCommand.context)
 beginbool(yyt->ParallelCommand.skip)
 beginCommandAttributes(yyt->ParallelCommand.attributes)
 yyt->ParallelCommand.isPermissive = pisPermissive;
 yyt->ParallelCommand.Left = pLeft;
 yyt->ParallelCommand.Right = pRight;
 return yyt;
}

tTree mLoopCommand
# if defined __STDC__ | defined __cplusplus
(tPosition pposition, tTree pCommand)
# else
(pposition, pCommand)
tPosition pposition;
tTree pCommand;
# endif
{
 register tTree yyt;
 yyALLOC (yyt, Tree_NodeSize [kLoopCommand])
 yyt->Kind = kLoopCommand;
 yyt->yyHead.yyMark = 0;
 yyt->LoopCommand.position = pposition;
 beginPtrContext(yyt->LoopCommand.context)
 beginbool(yyt->LoopCommand.skip)
 beginCommandAttributes(yyt->LoopCommand.attributes)
 yyt->LoopCommand.Command = pCommand;
 return yyt;
}

tTree mWhileGuardsCommand
# if defined __STDC__ | defined __cplusplus
(tPosition pposition, tTree pGuards)
# else
(pposition, pGuards)
tPosition pposition;
tTree pGuards;
# endif
{
 register tTree yyt;
 yyALLOC (yyt, Tree_NodeSize [kWhileGuardsCommand])
 yyt->Kind = kWhileGuardsCommand;
 yyt->yyHead.yyMark = 0;
 yyt->WhileGuardsCommand.position = pposition;
 beginPtrContext(yyt->WhileGuardsCommand.context)
 beginbool(yyt->WhileGuardsCommand.skip)
 beginCommandAttributes(yyt->WhileGuardsCommand.attributes)
 yyt->WhileGuardsCommand.Guards = pGuards;
 return yyt;
}

tTree mWhileGuardsAlsoCommand
# if defined __STDC__ | defined __cplusplus
(tPosition pposition, tTree pGuards, tTree pCommand)
# else
(pposition, pGuards, pCommand)
tPosition pposition;
tTree pGuards;
tTree pCommand;
# endif
{
 register tTree yyt;
 yyALLOC (yyt, Tree_NodeSize [kWhileGuardsAlsoCommand])
 yyt->Kind = kWhileGuardsAlsoCommand;
 yyt->yyHead.yyMark = 0;
 yyt->WhileGuardsAlsoCommand.position = pposition;
 beginPtrContext(yyt->WhileGuardsAlsoCommand.context)
 beginbool(yyt->WhileGuardsAlsoCommand.skip)
 beginCommandAttributes(yyt->WhileGuardsAlsoCommand.attributes)
 yyt->WhileGuardsAlsoCommand.Guards = pGuards;
 yyt->WhileGuardsAlsoCommand.Command = pCommand;
 return yyt;
}

tTree mCommandWhileExprCommand
# if defined __STDC__ | defined __cplusplus
(tPosition pposition, tTree pCommand, tTree pExpr)
# else
(pposition, pCommand, pExpr)
tPosition pposition;
tTree pCommand;
tTree pExpr;
# endif
{
 register tTree yyt;
 yyALLOC (yyt, Tree_NodeSize [kCommandWhileExprCommand])
 yyt->Kind = kCommandWhileExprCommand;
 yyt->yyHead.yyMark = 0;
 yyt->CommandWhileExprCommand.position = pposition;
 beginPtrContext(yyt->CommandWhileExprCommand.context)
 beginbool(yyt->CommandWhileExprCommand.skip)
 beginCommandAttributes(yyt->CommandWhileExprCommand.attributes)
 yyt->CommandWhileExprCommand.Command = pCommand;
 yyt->CommandWhileExprCommand.Expr = pExpr;
 return yyt;
}

tTree mCommandWhileGuardsCommand
# if defined __STDC__ | defined __cplusplus
(tPosition pposition, tTree pCommand, tTree pGuards)
# else
(pposition, pCommand, pGuards)
tPosition pposition;
tTree pCommand;
tTree pGuards;
# endif
{
 register tTree yyt;
 yyALLOC (yyt, Tree_NodeSize [kCommandWhileGuardsCommand])
 yyt->Kind = kCommandWhileGuardsCommand;
 yyt->yyHead.yyMark = 0;
 yyt->CommandWhileGuardsCommand.position = pposition;
 beginPtrContext(yyt->CommandWhileGuardsCommand.context)
 beginbool(yyt->CommandWhileGuardsCommand.skip)
 beginCommandAttributes(yyt->CommandWhileGuardsCommand.attributes)
 yyt->CommandWhileGuardsCommand.Command = pCommand;
 yyt->CommandWhileGuardsCommand.Guards = pGuards;
 return yyt;
}

tTree mCommandWhileGuardsAlsoCommand
# if defined __STDC__ | defined __cplusplus
(tPosition pposition, tTree pFirstCommand, tTree pGuards, tTree pAlsoCommand)
# else
(pposition, pFirstCommand, pGuards, pAlsoCommand)
tPosition pposition;
tTree pFirstCommand;
tTree pGuards;
tTree pAlsoCommand;
# endif
{
 register tTree yyt;
 yyALLOC (yyt, Tree_NodeSize [kCommandWhileGuardsAlsoCommand])
 yyt->Kind = kCommandWhileGuardsAlsoCommand;
 yyt->yyHead.yyMark = 0;
 yyt->CommandWhileGuardsAlsoCommand.position = pposition;
 beginPtrContext(yyt->CommandWhileGuardsAlsoCommand.context)
 beginbool(yyt->CommandWhileGuardsAlsoCommand.skip)
 beginCommandAttributes(yyt->CommandWhileGuardsAlsoCommand.attributes)
 yyt->CommandWhileGuardsAlsoCommand.FirstCommand = pFirstCommand;
 yyt->CommandWhileGuardsAlsoCommand.Guards = pGuards;
 yyt->CommandWhileGuardsAlsoCommand.AlsoCommand = pAlsoCommand;
 return yyt;
}

tTree mIfCommand
# if defined __STDC__ | defined __cplusplus
(tPosition pposition, tTree pGuards)
# else
(pposition, pGuards)
tPosition pposition;
tTree pGuards;
# endif
{
 register tTree yyt;
 yyALLOC (yyt, Tree_NodeSize [kIfCommand])
 yyt->Kind = kIfCommand;
 yyt->yyHead.yyMark = 0;
 yyt->IfCommand.position = pposition;
 beginPtrContext(yyt->IfCommand.context)
 beginbool(yyt->IfCommand.skip)
 beginCommandAttributes(yyt->IfCommand.attributes)
 yyt->IfCommand.Guards = pGuards;
 return yyt;
}

tTree mIfElseCommand
# if defined __STDC__ | defined __cplusplus
(tPosition pposition, tTree pGuards, tTree pCommand)
# else
(pposition, pGuards, pCommand)
tPosition pposition;
tTree pGuards;
tTree pCommand;
# endif
{
 register tTree yyt;
 yyALLOC (yyt, Tree_NodeSize [kIfElseCommand])
 yyt->Kind = kIfElseCommand;
 yyt->yyHead.yyMark = 0;
 yyt->IfElseCommand.position = pposition;
 beginPtrContext(yyt->IfElseCommand.context)
 beginbool(yyt->IfElseCommand.skip)
 beginCommandAttributes(yyt->IfElseCommand.attributes)
 yyt->IfElseCommand.Guards = pGuards;
 yyt->IfElseCommand.Command = pCommand;
 return yyt;
}

tTree mCaseCommand
# if defined __STDC__ | defined __cplusplus
(tPosition pposition, tTree pExpr, tTree pCaseGuards, tTree pCommand)
# else
(pposition, pExpr, pCaseGuards, pCommand)
tPosition pposition;
tTree pExpr;
tTree pCaseGuards;
tTree pCommand;
# endif
{
 register tTree yyt;
 yyALLOC (yyt, Tree_NodeSize [kCaseCommand])
 yyt->Kind = kCaseCommand;
 yyt->yyHead.yyMark = 0;
 yyt->CaseCommand.position = pposition;
 beginPtrContext(yyt->CaseCommand.context)
 beginbool(yyt->CaseCommand.skip)
 beginCommandAttributes(yyt->CaseCommand.attributes)
 yyt->CaseCommand.Expr = pExpr;
 yyt->CaseCommand.CaseGuards = pCaseGuards;
 yyt->CaseCommand.Command = pCommand;
 return yyt;
}

tTree mForCommand
# if defined __STDC__ | defined __cplusplus
(tPosition pposition, bool pisParallel, bool pisPermissive, tIdent pident, tTree pRange, tTree pCommand)
# else
(pposition, pisParallel, pisPermissive, pident, pRange, pCommand)
tPosition pposition;
bool pisParallel;
bool pisPermissive;
tIdent pident;
tTree pRange;
tTree pCommand;
# endif
{
 register tTree yyt;
 yyALLOC (yyt, Tree_NodeSize [kForCommand])
 yyt->Kind = kForCommand;
 yyt->yyHead.yyMark = 0;
 yyt->ForCommand.position = pposition;
 beginPtrContext(yyt->ForCommand.context)
 beginbool(yyt->ForCommand.skip)
 beginCommandAttributes(yyt->ForCommand.attributes)
 yyt->ForCommand.isParallel = pisParallel;
 yyt->ForCommand.isPermissive = pisPermissive;
 yyt->ForCommand.ident = pident;
 yyt->ForCommand.Range = pRange;
 yyt->ForCommand.Command = pCommand;
 beginPtrInstance(yyt->ForCommand.iterator)
 beginint(yyt->ForCommand.lastErrorCount)
 beginPtrchar(yyt->ForCommand.errorContextString)
 beginPtrchar(yyt->ForCommand.valueStringPtr)
 return yyt;
}

tTree mProcedureCallCommonCommand
# if defined __STDC__ | defined __cplusplus
(tPosition pposition, tIdent pident, tTree pProcedureParams)
# else
(pposition, pident, pProcedureParams)
tPosition pposition;
tIdent pident;
tTree pProcedureParams;
# endif
{
 register tTree yyt;
 yyALLOC (yyt, Tree_NodeSize [kProcedureCallCommonCommand])
 yyt->Kind = kProcedureCallCommonCommand;
 yyt->yyHead.yyMark = 0;
 yyt->ProcedureCallCommonCommand.position = pposition;
 beginPtrContext(yyt->ProcedureCallCommonCommand.context)
 beginbool(yyt->ProcedureCallCommonCommand.skip)
 beginCommandAttributes(yyt->ProcedureCallCommonCommand.attributes)
 yyt->ProcedureCallCommonCommand.ident = pident;
 yyt->ProcedureCallCommonCommand.ProcedureParams = pProcedureParams;
 return yyt;
}

tTree mSelectCommand
# if defined __STDC__ | defined __cplusplus
(tPosition pposition, tTree pChannelGuards)
# else
(pposition, pChannelGuards)
tPosition pposition;
tTree pChannelGuards;
# endif
{
 register tTree yyt;
 yyALLOC (yyt, Tree_NodeSize [kSelectCommand])
 yyt->Kind = kSelectCommand;
 yyt->yyHead.yyMark = 0;
 yyt->SelectCommand.position = pposition;
 beginPtrContext(yyt->SelectCommand.context)
 beginbool(yyt->SelectCommand.skip)
 beginCommandAttributes(yyt->SelectCommand.attributes)
 yyt->SelectCommand.ChannelGuards = pChannelGuards;
 return yyt;
}

tTree mSelectBangCommand
# if defined __STDC__ | defined __cplusplus
(tPosition pposition, tTree pChannelGuards)
# else
(pposition, pChannelGuards)
tPosition pposition;
tTree pChannelGuards;
# endif
{
 register tTree yyt;
 yyALLOC (yyt, Tree_NodeSize [kSelectBangCommand])
 yyt->Kind = kSelectBangCommand;
 yyt->yyHead.yyMark = 0;
 yyt->SelectBangCommand.position = pposition;
 beginPtrContext(yyt->SelectBangCommand.context)
 beginbool(yyt->SelectBangCommand.skip)
 beginCommandAttributes(yyt->SelectBangCommand.attributes)
 yyt->SelectBangCommand.ChannelGuards = pChannelGuards;
 return yyt;
}

tTree mArbitrateCommand
# if defined __STDC__ | defined __cplusplus
(tPosition pposition, tTree pChannelGuards)
# else
(pposition, pChannelGuards)
tPosition pposition;
tTree pChannelGuards;
# endif
{
 register tTree yyt;
 yyALLOC (yyt, Tree_NodeSize [kArbitrateCommand])
 yyt->Kind = kArbitrateCommand;
 yyt->yyHead.yyMark = 0;
 yyt->ArbitrateCommand.position = pposition;
 beginPtrContext(yyt->ArbitrateCommand.context)
 beginbool(yyt->ArbitrateCommand.skip)
 beginCommandAttributes(yyt->ArbitrateCommand.attributes)
 yyt->ArbitrateCommand.ChannelGuards = pChannelGuards;
 return yyt;
}

tTree mPrintCommand
# if defined __STDC__ | defined __cplusplus
(tPosition pposition, tTree pExpr, tTree pExprLists)
# else
(pposition, pExpr, pExprLists)
tPosition pposition;
tTree pExpr;
tTree pExprLists;
# endif
{
 register tTree yyt;
 yyALLOC (yyt, Tree_NodeSize [kPrintCommand])
 yyt->Kind = kPrintCommand;
 yyt->yyHead.yyMark = 0;
 yyt->PrintCommand.position = pposition;
 beginPtrContext(yyt->PrintCommand.context)
 beginbool(yyt->PrintCommand.skip)
 beginCommandAttributes(yyt->PrintCommand.attributes)
 yyt->PrintCommand.Expr = pExpr;
 yyt->PrintCommand.ExprLists = pExprLists;
 return yyt;
}

tTree mSinkCommand
# if defined __STDC__ | defined __cplusplus
(tPosition pposition, tTree pExpr)
# else
(pposition, pExpr)
tPosition pposition;
tTree pExpr;
# endif
{
 register tTree yyt;
 yyALLOC (yyt, Tree_NodeSize [kSinkCommand])
 yyt->Kind = kSinkCommand;
 yyt->yyHead.yyMark = 0;
 yyt->SinkCommand.position = pposition;
 beginPtrContext(yyt->SinkCommand.context)
 beginbool(yyt->SinkCommand.skip)
 beginCommandAttributes(yyt->SinkCommand.attributes)
 yyt->SinkCommand.Expr = pExpr;
 return yyt;
}

tTree mProcedureParams
# if defined __STDC__ | defined __cplusplus
(tPosition pposition)
# else
(pposition)
tPosition pposition;
# endif
{
 register tTree yyt;
 yyALLOC (yyt, Tree_NodeSize [kProcedureParams])
 yyt->Kind = kProcedureParams;
 yyt->yyHead.yyMark = 0;
 yyt->ProcedureParams.position = pposition;
 beginPtrContext(yyt->ProcedureParams.context)
 beginbool(yyt->ProcedureParams.skip)
 beginPtrExprAttributesList(yyt->ProcedureParams.actualPorts)
 begintTree(yyt->ProcedureParams.formalParams)
 beginPtrInstanceList(yyt->ProcedureParams.formalPorts)
 beginPtrInstanceList(yyt->ProcedureParams.constantParams)
 beginPtrTypeList(yyt->ProcedureParams.typeParams)
 beginPtrIdentList(yyt->ProcedureParams.idents)
 beginPtrProcedure(yyt->ProcedureParams.procedureIn)
 beginPtrProcedure(yyt->ProcedureParams.procedureOut)
 begintTree(yyt->ProcedureParams.procedureTree)
 beginbool(yyt->ProcedureParams.noPorts)
 beginPtrContext(yyt->ProcedureParams.parameterContext)
 return yyt;
}

tTree mNullProcParams
# if defined __STDC__ | defined __cplusplus
(tPosition pposition)
# else
(pposition)
tPosition pposition;
# endif
{
 register tTree yyt;
 yyALLOC (yyt, Tree_NodeSize [kNullProcParams])
 yyt->Kind = kNullProcParams;
 yyt->yyHead.yyMark = 0;
 yyt->NullProcParams.position = pposition;
 beginPtrContext(yyt->NullProcParams.context)
 beginbool(yyt->NullProcParams.skip)
 beginPtrExprAttributesList(yyt->NullProcParams.actualPorts)
 begintTree(yyt->NullProcParams.formalParams)
 beginPtrInstanceList(yyt->NullProcParams.formalPorts)
 beginPtrInstanceList(yyt->NullProcParams.constantParams)
 beginPtrTypeList(yyt->NullProcParams.typeParams)
 beginPtrIdentList(yyt->NullProcParams.idents)
 beginPtrProcedure(yyt->NullProcParams.procedureIn)
 beginPtrProcedure(yyt->NullProcParams.procedureOut)
 begintTree(yyt->NullProcParams.procedureTree)
 beginbool(yyt->NullProcParams.noPorts)
 beginPtrContext(yyt->NullProcParams.parameterContext)
 return yyt;
}

tTree mProcParam
# if defined __STDC__ | defined __cplusplus
(tPosition pposition, tTree pnext)
# else
(pposition, pnext)
tPosition pposition;
tTree pnext;
# endif
{
 register tTree yyt;
 yyALLOC (yyt, Tree_NodeSize [kProcParam])
 yyt->Kind = kProcParam;
 yyt->yyHead.yyMark = 0;
 yyt->ProcParam.position = pposition;
 beginPtrContext(yyt->ProcParam.context)
 beginbool(yyt->ProcParam.skip)
 beginPtrExprAttributesList(yyt->ProcParam.actualPorts)
 begintTree(yyt->ProcParam.formalParams)
 beginPtrInstanceList(yyt->ProcParam.formalPorts)
 beginPtrInstanceList(yyt->ProcParam.constantParams)
 beginPtrTypeList(yyt->ProcParam.typeParams)
 beginPtrIdentList(yyt->ProcParam.idents)
 beginPtrProcedure(yyt->ProcParam.procedureIn)
 beginPtrProcedure(yyt->ProcParam.procedureOut)
 begintTree(yyt->ProcParam.procedureTree)
 beginbool(yyt->ProcParam.noPorts)
 beginPtrContext(yyt->ProcParam.parameterContext)
 yyt->ProcParam.next = pnext;
 return yyt;
}

tTree mExprProcParam
# if defined __STDC__ | defined __cplusplus
(tPosition pposition, tTree pnext, tTree pCoercedExpr)
# else
(pposition, pnext, pCoercedExpr)
tPosition pposition;
tTree pnext;
tTree pCoercedExpr;
# endif
{
 register tTree yyt;
 yyALLOC (yyt, Tree_NodeSize [kExprProcParam])
 yyt->Kind = kExprProcParam;
 yyt->yyHead.yyMark = 0;
 yyt->ExprProcParam.position = pposition;
 beginPtrContext(yyt->ExprProcParam.context)
 beginbool(yyt->ExprProcParam.skip)
 beginPtrExprAttributesList(yyt->ExprProcParam.actualPorts)
 begintTree(yyt->ExprProcParam.formalParams)
 beginPtrInstanceList(yyt->ExprProcParam.formalPorts)
 beginPtrInstanceList(yyt->ExprProcParam.constantParams)
 beginPtrTypeList(yyt->ExprProcParam.typeParams)
 beginPtrIdentList(yyt->ExprProcParam.idents)
 beginPtrProcedure(yyt->ExprProcParam.procedureIn)
 beginPtrProcedure(yyt->ExprProcParam.procedureOut)
 begintTree(yyt->ExprProcParam.procedureTree)
 beginbool(yyt->ExprProcParam.noPorts)
 beginPtrContext(yyt->ExprProcParam.parameterContext)
 yyt->ExprProcParam.next = pnext;
 yyt->ExprProcParam.CoercedExpr = pCoercedExpr;
 beginPtrType(yyt->ExprProcParam.typeIfTypeName)
 return yyt;
}

tTree mTypeProcParam
# if defined __STDC__ | defined __cplusplus
(tPosition pposition, tTree pnext, tTree pAType)
# else
(pposition, pnext, pAType)
tPosition pposition;
tTree pnext;
tTree pAType;
# endif
{
 register tTree yyt;
 yyALLOC (yyt, Tree_NodeSize [kTypeProcParam])
 yyt->Kind = kTypeProcParam;
 yyt->yyHead.yyMark = 0;
 yyt->TypeProcParam.position = pposition;
 beginPtrContext(yyt->TypeProcParam.context)
 beginbool(yyt->TypeProcParam.skip)
 beginPtrExprAttributesList(yyt->TypeProcParam.actualPorts)
 begintTree(yyt->TypeProcParam.formalParams)
 beginPtrInstanceList(yyt->TypeProcParam.formalPorts)
 beginPtrInstanceList(yyt->TypeProcParam.constantParams)
 beginPtrTypeList(yyt->TypeProcParam.typeParams)
 beginPtrIdentList(yyt->TypeProcParam.idents)
 beginPtrProcedure(yyt->TypeProcParam.procedureIn)
 beginPtrProcedure(yyt->TypeProcParam.procedureOut)
 begintTree(yyt->TypeProcParam.procedureTree)
 beginbool(yyt->TypeProcParam.noPorts)
 beginPtrContext(yyt->TypeProcParam.parameterContext)
 yyt->TypeProcParam.next = pnext;
 yyt->TypeProcParam.AType = pAType;
 return yyt;
}

tTree mVarReadProcParam
# if defined __STDC__ | defined __cplusplus
(tPosition pposition, tTree pnext, tTree pCoercedExpr)
# else
(pposition, pnext, pCoercedExpr)
tPosition pposition;
tTree pnext;
tTree pCoercedExpr;
# endif
{
 register tTree yyt;
 yyALLOC (yyt, Tree_NodeSize [kVarReadProcParam])
 yyt->Kind = kVarReadProcParam;
 yyt->yyHead.yyMark = 0;
 yyt->VarReadProcParam.position = pposition;
 beginPtrContext(yyt->VarReadProcParam.context)
 beginbool(yyt->VarReadProcParam.skip)
 beginPtrExprAttributesList(yyt->VarReadProcParam.actualPorts)
 begintTree(yyt->VarReadProcParam.formalParams)
 beginPtrInstanceList(yyt->VarReadProcParam.formalPorts)
 beginPtrInstanceList(yyt->VarReadProcParam.constantParams)
 beginPtrTypeList(yyt->VarReadProcParam.typeParams)
 beginPtrIdentList(yyt->VarReadProcParam.idents)
 beginPtrProcedure(yyt->VarReadProcParam.procedureIn)
 beginPtrProcedure(yyt->VarReadProcParam.procedureOut)
 begintTree(yyt->VarReadProcParam.procedureTree)
 beginbool(yyt->VarReadProcParam.noPorts)
 beginPtrContext(yyt->VarReadProcParam.parameterContext)
 yyt->VarReadProcParam.next = pnext;
 yyt->VarReadProcParam.CoercedExpr = pCoercedExpr;
 return yyt;
}

tTree mVarWriteProcParam
# if defined __STDC__ | defined __cplusplus
(tPosition pposition, tTree pnext, tTree pLvalue)
# else
(pposition, pnext, pLvalue)
tPosition pposition;
tTree pnext;
tTree pLvalue;
# endif
{
 register tTree yyt;
 yyALLOC (yyt, Tree_NodeSize [kVarWriteProcParam])
 yyt->Kind = kVarWriteProcParam;
 yyt->yyHead.yyMark = 0;
 yyt->VarWriteProcParam.position = pposition;
 beginPtrContext(yyt->VarWriteProcParam.context)
 beginbool(yyt->VarWriteProcParam.skip)
 beginPtrExprAttributesList(yyt->VarWriteProcParam.actualPorts)
 begintTree(yyt->VarWriteProcParam.formalParams)
 beginPtrInstanceList(yyt->VarWriteProcParam.formalPorts)
 beginPtrInstanceList(yyt->VarWriteProcParam.constantParams)
 beginPtrTypeList(yyt->VarWriteProcParam.typeParams)
 beginPtrIdentList(yyt->VarWriteProcParam.idents)
 beginPtrProcedure(yyt->VarWriteProcParam.procedureIn)
 beginPtrProcedure(yyt->VarWriteProcParam.procedureOut)
 begintTree(yyt->VarWriteProcParam.procedureTree)
 beginbool(yyt->VarWriteProcParam.noPorts)
 beginPtrContext(yyt->VarWriteProcParam.parameterContext)
 yyt->VarWriteProcParam.next = pnext;
 yyt->VarWriteProcParam.Lvalue = pLvalue;
 return yyt;
}

tTree mBlockProcParam
# if defined __STDC__ | defined __cplusplus
(tPosition pposition, tTree pnext, tTree pBlock)
# else
(pposition, pnext, pBlock)
tPosition pposition;
tTree pnext;
tTree pBlock;
# endif
{
 register tTree yyt;
 yyALLOC (yyt, Tree_NodeSize [kBlockProcParam])
 yyt->Kind = kBlockProcParam;
 yyt->yyHead.yyMark = 0;
 yyt->BlockProcParam.position = pposition;
 beginPtrContext(yyt->BlockProcParam.context)
 beginbool(yyt->BlockProcParam.skip)
 beginPtrExprAttributesList(yyt->BlockProcParam.actualPorts)
 begintTree(yyt->BlockProcParam.formalParams)
 beginPtrInstanceList(yyt->BlockProcParam.formalPorts)
 beginPtrInstanceList(yyt->BlockProcParam.constantParams)
 beginPtrTypeList(yyt->BlockProcParam.typeParams)
 beginPtrIdentList(yyt->BlockProcParam.idents)
 beginPtrProcedure(yyt->BlockProcParam.procedureIn)
 beginPtrProcedure(yyt->BlockProcParam.procedureOut)
 begintTree(yyt->BlockProcParam.procedureTree)
 beginbool(yyt->BlockProcParam.noPorts)
 beginPtrContext(yyt->BlockProcParam.parameterContext)
 yyt->BlockProcParam.next = pnext;
 yyt->BlockProcParam.Block = pBlock;
 return yyt;
}

tTree mFunctionParams
# if defined __STDC__ | defined __cplusplus
(tPosition pposition)
# else
(pposition)
tPosition pposition;
# endif
{
 register tTree yyt;
 yyALLOC (yyt, Tree_NodeSize [kFunctionParams])
 yyt->Kind = kFunctionParams;
 yyt->yyHead.yyMark = 0;
 yyt->FunctionParams.position = pposition;
 beginPtrContext(yyt->FunctionParams.context)
 beginbool(yyt->FunctionParams.skip)
 beginPtrExprAttributesList(yyt->FunctionParams.actualPorts)
 begintTree(yyt->FunctionParams.formalParams)
 beginPtrInstanceList(yyt->FunctionParams.formalPorts)
 beginPtrInstanceList(yyt->FunctionParams.constantParams)
 beginPtrTypeList(yyt->FunctionParams.typeParams)
 beginPtrComponentParameterList(yyt->FunctionParams.params)
 beginPtrIdentList(yyt->FunctionParams.idents)
 beginPtrProcedure(yyt->FunctionParams.functionIn)
 beginPtrProcedure(yyt->FunctionParams.functionOut)
 begintTree(yyt->FunctionParams.functionTree)
 beginPtrContext(yyt->FunctionParams.parameterContext)
 return yyt;
}

tTree mNullFuncParams
# if defined __STDC__ | defined __cplusplus
(tPosition pposition)
# else
(pposition)
tPosition pposition;
# endif
{
 register tTree yyt;
 yyALLOC (yyt, Tree_NodeSize [kNullFuncParams])
 yyt->Kind = kNullFuncParams;
 yyt->yyHead.yyMark = 0;
 yyt->NullFuncParams.position = pposition;
 beginPtrContext(yyt->NullFuncParams.context)
 beginbool(yyt->NullFuncParams.skip)
 beginPtrExprAttributesList(yyt->NullFuncParams.actualPorts)
 begintTree(yyt->NullFuncParams.formalParams)
 beginPtrInstanceList(yyt->NullFuncParams.formalPorts)
 beginPtrInstanceList(yyt->NullFuncParams.constantParams)
 beginPtrTypeList(yyt->NullFuncParams.typeParams)
 beginPtrComponentParameterList(yyt->NullFuncParams.params)
 beginPtrIdentList(yyt->NullFuncParams.idents)
 beginPtrProcedure(yyt->NullFuncParams.functionIn)
 beginPtrProcedure(yyt->NullFuncParams.functionOut)
 begintTree(yyt->NullFuncParams.functionTree)
 beginPtrContext(yyt->NullFuncParams.parameterContext)
 return yyt;
}

tTree mFuncParam
# if defined __STDC__ | defined __cplusplus
(tPosition pposition, tTree pnext)
# else
(pposition, pnext)
tPosition pposition;
tTree pnext;
# endif
{
 register tTree yyt;
 yyALLOC (yyt, Tree_NodeSize [kFuncParam])
 yyt->Kind = kFuncParam;
 yyt->yyHead.yyMark = 0;
 yyt->FuncParam.position = pposition;
 beginPtrContext(yyt->FuncParam.context)
 beginbool(yyt->FuncParam.skip)
 beginPtrExprAttributesList(yyt->FuncParam.actualPorts)
 begintTree(yyt->FuncParam.formalParams)
 beginPtrInstanceList(yyt->FuncParam.formalPorts)
 beginPtrInstanceList(yyt->FuncParam.constantParams)
 beginPtrTypeList(yyt->FuncParam.typeParams)
 beginPtrComponentParameterList(yyt->FuncParam.params)
 beginPtrIdentList(yyt->FuncParam.idents)
 beginPtrProcedure(yyt->FuncParam.functionIn)
 beginPtrProcedure(yyt->FuncParam.functionOut)
 begintTree(yyt->FuncParam.functionTree)
 beginPtrContext(yyt->FuncParam.parameterContext)
 yyt->FuncParam.next = pnext;
 return yyt;
}

tTree mExprFuncParam
# if defined __STDC__ | defined __cplusplus
(tPosition pposition, tTree pnext, tTree pCoercedExpr)
# else
(pposition, pnext, pCoercedExpr)
tPosition pposition;
tTree pnext;
tTree pCoercedExpr;
# endif
{
 register tTree yyt;
 yyALLOC (yyt, Tree_NodeSize [kExprFuncParam])
 yyt->Kind = kExprFuncParam;
 yyt->yyHead.yyMark = 0;
 yyt->ExprFuncParam.position = pposition;
 beginPtrContext(yyt->ExprFuncParam.context)
 beginbool(yyt->ExprFuncParam.skip)
 beginPtrExprAttributesList(yyt->ExprFuncParam.actualPorts)
 begintTree(yyt->ExprFuncParam.formalParams)
 beginPtrInstanceList(yyt->ExprFuncParam.formalPorts)
 beginPtrInstanceList(yyt->ExprFuncParam.constantParams)
 beginPtrTypeList(yyt->ExprFuncParam.typeParams)
 beginPtrComponentParameterList(yyt->ExprFuncParam.params)
 beginPtrIdentList(yyt->ExprFuncParam.idents)
 beginPtrProcedure(yyt->ExprFuncParam.functionIn)
 beginPtrProcedure(yyt->ExprFuncParam.functionOut)
 begintTree(yyt->ExprFuncParam.functionTree)
 beginPtrContext(yyt->ExprFuncParam.parameterContext)
 yyt->ExprFuncParam.next = pnext;
 yyt->ExprFuncParam.CoercedExpr = pCoercedExpr;
 beginPtrType(yyt->ExprFuncParam.typeIfTypeName)
 return yyt;
}

tTree mTypeFuncParam
# if defined __STDC__ | defined __cplusplus
(tPosition pposition, tTree pnext, tTree pAType)
# else
(pposition, pnext, pAType)
tPosition pposition;
tTree pnext;
tTree pAType;
# endif
{
 register tTree yyt;
 yyALLOC (yyt, Tree_NodeSize [kTypeFuncParam])
 yyt->Kind = kTypeFuncParam;
 yyt->yyHead.yyMark = 0;
 yyt->TypeFuncParam.position = pposition;
 beginPtrContext(yyt->TypeFuncParam.context)
 beginbool(yyt->TypeFuncParam.skip)
 beginPtrExprAttributesList(yyt->TypeFuncParam.actualPorts)
 begintTree(yyt->TypeFuncParam.formalParams)
 beginPtrInstanceList(yyt->TypeFuncParam.formalPorts)
 beginPtrInstanceList(yyt->TypeFuncParam.constantParams)
 beginPtrTypeList(yyt->TypeFuncParam.typeParams)
 beginPtrComponentParameterList(yyt->TypeFuncParam.params)
 beginPtrIdentList(yyt->TypeFuncParam.idents)
 beginPtrProcedure(yyt->TypeFuncParam.functionIn)
 beginPtrProcedure(yyt->TypeFuncParam.functionOut)
 begintTree(yyt->TypeFuncParam.functionTree)
 beginPtrContext(yyt->TypeFuncParam.parameterContext)
 yyt->TypeFuncParam.next = pnext;
 yyt->TypeFuncParam.AType = pAType;
 return yyt;
}

tTree mLvalue
# if defined __STDC__ | defined __cplusplus
(tPosition pposition)
# else
(pposition)
tPosition pposition;
# endif
{
 register tTree yyt;
 yyALLOC (yyt, Tree_NodeSize [kLvalue])
 yyt->Kind = kLvalue;
 yyt->yyHead.yyMark = 0;
 yyt->Lvalue.position = pposition;
 beginPtrContext(yyt->Lvalue.context)
 beginbool(yyt->Lvalue.skip)
 beginExprAttributes(yyt->Lvalue.attributes)
 beginbool(yyt->Lvalue.expectingChannel)
 beginbool(yyt->Lvalue.expectingEither)
 beginbool(yyt->Lvalue.isInput)
 beginbool(yyt->Lvalue.isPassive)
 beginbool(yyt->Lvalue.lockPassiveChannels)
 beginPtrInstance(yyt->Lvalue.instance)
 beginPtrSpanList(yyt->Lvalue.partitions)
 beginPtrSpanListList(yyt->Lvalue.indices)
 beginPtrWire(yyt->Lvalue.indexWire)
 beginPtrAccess(yyt->Lvalue.access)
 beginPtrType(yyt->Lvalue.expectedBaseType)
 return yyt;
}

tTree mIdentLvalue
# if defined __STDC__ | defined __cplusplus
(tPosition pposition, tIdent pident)
# else
(pposition, pident)
tPosition pposition;
tIdent pident;
# endif
{
 register tTree yyt;
 yyALLOC (yyt, Tree_NodeSize [kIdentLvalue])
 yyt->Kind = kIdentLvalue;
 yyt->yyHead.yyMark = 0;
 yyt->IdentLvalue.position = pposition;
 beginPtrContext(yyt->IdentLvalue.context)
 beginbool(yyt->IdentLvalue.skip)
 beginExprAttributes(yyt->IdentLvalue.attributes)
 beginbool(yyt->IdentLvalue.expectingChannel)
 beginbool(yyt->IdentLvalue.expectingEither)
 beginbool(yyt->IdentLvalue.isInput)
 beginbool(yyt->IdentLvalue.isPassive)
 beginbool(yyt->IdentLvalue.lockPassiveChannels)
 beginPtrInstance(yyt->IdentLvalue.instance)
 beginPtrSpanList(yyt->IdentLvalue.partitions)
 beginPtrSpanListList(yyt->IdentLvalue.indices)
 beginPtrWire(yyt->IdentLvalue.indexWire)
 beginPtrAccess(yyt->IdentLvalue.access)
 beginPtrType(yyt->IdentLvalue.expectedBaseType)
 yyt->IdentLvalue.ident = pident;
 return yyt;
}

tTree mRecordElemLvalue
# if defined __STDC__ | defined __cplusplus
(tPosition pposition, tTree pLvalue, tIdent pident)
# else
(pposition, pLvalue, pident)
tPosition pposition;
tTree pLvalue;
tIdent pident;
# endif
{
 register tTree yyt;
 yyALLOC (yyt, Tree_NodeSize [kRecordElemLvalue])
 yyt->Kind = kRecordElemLvalue;
 yyt->yyHead.yyMark = 0;
 yyt->RecordElemLvalue.position = pposition;
 beginPtrContext(yyt->RecordElemLvalue.context)
 beginbool(yyt->RecordElemLvalue.skip)
 beginExprAttributes(yyt->RecordElemLvalue.attributes)
 beginbool(yyt->RecordElemLvalue.expectingChannel)
 beginbool(yyt->RecordElemLvalue.expectingEither)
 beginbool(yyt->RecordElemLvalue.isInput)
 beginbool(yyt->RecordElemLvalue.isPassive)
 beginbool(yyt->RecordElemLvalue.lockPassiveChannels)
 beginPtrInstance(yyt->RecordElemLvalue.instance)
 beginPtrSpanList(yyt->RecordElemLvalue.partitions)
 beginPtrSpanListList(yyt->RecordElemLvalue.indices)
 beginPtrWire(yyt->RecordElemLvalue.indexWire)
 beginPtrAccess(yyt->RecordElemLvalue.access)
 beginPtrType(yyt->RecordElemLvalue.expectedBaseType)
 yyt->RecordElemLvalue.Lvalue = pLvalue;
 yyt->RecordElemLvalue.ident = pident;
 return yyt;
}

tTree mArrayElemLvalue
# if defined __STDC__ | defined __cplusplus
(tPosition pposition, tTree pLvalue, tTree pCoercedExpr)
# else
(pposition, pLvalue, pCoercedExpr)
tPosition pposition;
tTree pLvalue;
tTree pCoercedExpr;
# endif
{
 register tTree yyt;
 yyALLOC (yyt, Tree_NodeSize [kArrayElemLvalue])
 yyt->Kind = kArrayElemLvalue;
 yyt->yyHead.yyMark = 0;
 yyt->ArrayElemLvalue.position = pposition;
 beginPtrContext(yyt->ArrayElemLvalue.context)
 beginbool(yyt->ArrayElemLvalue.skip)
 beginExprAttributes(yyt->ArrayElemLvalue.attributes)
 beginbool(yyt->ArrayElemLvalue.expectingChannel)
 beginbool(yyt->ArrayElemLvalue.expectingEither)
 beginbool(yyt->ArrayElemLvalue.isInput)
 beginbool(yyt->ArrayElemLvalue.isPassive)
 beginbool(yyt->ArrayElemLvalue.lockPassiveChannels)
 beginPtrInstance(yyt->ArrayElemLvalue.instance)
 beginPtrSpanList(yyt->ArrayElemLvalue.partitions)
 beginPtrSpanListList(yyt->ArrayElemLvalue.indices)
 beginPtrWire(yyt->ArrayElemLvalue.indexWire)
 beginPtrAccess(yyt->ArrayElemLvalue.access)
 beginPtrType(yyt->ArrayElemLvalue.expectedBaseType)
 yyt->ArrayElemLvalue.Lvalue = pLvalue;
 yyt->ArrayElemLvalue.CoercedExpr = pCoercedExpr;
 return yyt;
}

tTree mArraySliceLvalue
# if defined __STDC__ | defined __cplusplus
(tPosition pposition, tTree pLvalue, tTree pRange)
# else
(pposition, pLvalue, pRange)
tPosition pposition;
tTree pLvalue;
tTree pRange;
# endif
{
 register tTree yyt;
 yyALLOC (yyt, Tree_NodeSize [kArraySliceLvalue])
 yyt->Kind = kArraySliceLvalue;
 yyt->yyHead.yyMark = 0;
 yyt->ArraySliceLvalue.position = pposition;
 beginPtrContext(yyt->ArraySliceLvalue.context)
 beginbool(yyt->ArraySliceLvalue.skip)
 beginExprAttributes(yyt->ArraySliceLvalue.attributes)
 beginbool(yyt->ArraySliceLvalue.expectingChannel)
 beginbool(yyt->ArraySliceLvalue.expectingEither)
 beginbool(yyt->ArraySliceLvalue.isInput)
 beginbool(yyt->ArraySliceLvalue.isPassive)
 beginbool(yyt->ArraySliceLvalue.lockPassiveChannels)
 beginPtrInstance(yyt->ArraySliceLvalue.instance)
 beginPtrSpanList(yyt->ArraySliceLvalue.partitions)
 beginPtrSpanListList(yyt->ArraySliceLvalue.indices)
 beginPtrWire(yyt->ArraySliceLvalue.indexWire)
 beginPtrAccess(yyt->ArraySliceLvalue.access)
 beginPtrType(yyt->ArraySliceLvalue.expectedBaseType)
 yyt->ArraySliceLvalue.Lvalue = pLvalue;
 yyt->ArraySliceLvalue.Range = pRange;
 return yyt;
}

tTree mArrayAppendLvalue
# if defined __STDC__ | defined __cplusplus
(tPosition pposition, tTree pLeft, tTree pRight)
# else
(pposition, pLeft, pRight)
tPosition pposition;
tTree pLeft;
tTree pRight;
# endif
{
 register tTree yyt;
 yyALLOC (yyt, Tree_NodeSize [kArrayAppendLvalue])
 yyt->Kind = kArrayAppendLvalue;
 yyt->yyHead.yyMark = 0;
 yyt->ArrayAppendLvalue.position = pposition;
 beginPtrContext(yyt->ArrayAppendLvalue.context)
 beginbool(yyt->ArrayAppendLvalue.skip)
 beginExprAttributes(yyt->ArrayAppendLvalue.attributes)
 beginbool(yyt->ArrayAppendLvalue.expectingChannel)
 beginbool(yyt->ArrayAppendLvalue.expectingEither)
 beginbool(yyt->ArrayAppendLvalue.isInput)
 beginbool(yyt->ArrayAppendLvalue.isPassive)
 beginbool(yyt->ArrayAppendLvalue.lockPassiveChannels)
 beginPtrInstance(yyt->ArrayAppendLvalue.instance)
 beginPtrSpanList(yyt->ArrayAppendLvalue.partitions)
 beginPtrSpanListList(yyt->ArrayAppendLvalue.indices)
 beginPtrWire(yyt->ArrayAppendLvalue.indexWire)
 beginPtrAccess(yyt->ArrayAppendLvalue.access)
 beginPtrType(yyt->ArrayAppendLvalue.expectedBaseType)
 yyt->ArrayAppendLvalue.Left = pLeft;
 yyt->ArrayAppendLvalue.Right = pRight;
 return yyt;
}

tTree mArrayConsLvalue
# if defined __STDC__ | defined __cplusplus
(tPosition pposition, tTree pLvalueLists)
# else
(pposition, pLvalueLists)
tPosition pposition;
tTree pLvalueLists;
# endif
{
 register tTree yyt;
 yyALLOC (yyt, Tree_NodeSize [kArrayConsLvalue])
 yyt->Kind = kArrayConsLvalue;
 yyt->yyHead.yyMark = 0;
 yyt->ArrayConsLvalue.position = pposition;
 beginPtrContext(yyt->ArrayConsLvalue.context)
 beginbool(yyt->ArrayConsLvalue.skip)
 beginExprAttributes(yyt->ArrayConsLvalue.attributes)
 beginbool(yyt->ArrayConsLvalue.expectingChannel)
 beginbool(yyt->ArrayConsLvalue.expectingEither)
 beginbool(yyt->ArrayConsLvalue.isInput)
 beginbool(yyt->ArrayConsLvalue.isPassive)
 beginbool(yyt->ArrayConsLvalue.lockPassiveChannels)
 beginPtrInstance(yyt->ArrayConsLvalue.instance)
 beginPtrSpanList(yyt->ArrayConsLvalue.partitions)
 beginPtrSpanListList(yyt->ArrayConsLvalue.indices)
 beginPtrWire(yyt->ArrayConsLvalue.indexWire)
 beginPtrAccess(yyt->ArrayConsLvalue.access)
 beginPtrType(yyt->ArrayConsLvalue.expectedBaseType)
 yyt->ArrayConsLvalue.LvalueLists = pLvalueLists;
 return yyt;
}

tTree mAsLvalue
# if defined __STDC__ | defined __cplusplus
(tPosition pposition, tTree pLvalue, tTree pAType)
# else
(pposition, pLvalue, pAType)
tPosition pposition;
tTree pLvalue;
tTree pAType;
# endif
{
 register tTree yyt;
 yyALLOC (yyt, Tree_NodeSize [kAsLvalue])
 yyt->Kind = kAsLvalue;
 yyt->yyHead.yyMark = 0;
 yyt->AsLvalue.position = pposition;
 beginPtrContext(yyt->AsLvalue.context)
 beginbool(yyt->AsLvalue.skip)
 beginExprAttributes(yyt->AsLvalue.attributes)
 beginbool(yyt->AsLvalue.expectingChannel)
 beginbool(yyt->AsLvalue.expectingEither)
 beginbool(yyt->AsLvalue.isInput)
 beginbool(yyt->AsLvalue.isPassive)
 beginbool(yyt->AsLvalue.lockPassiveChannels)
 beginPtrInstance(yyt->AsLvalue.instance)
 beginPtrSpanList(yyt->AsLvalue.partitions)
 beginPtrSpanListList(yyt->AsLvalue.indices)
 beginPtrWire(yyt->AsLvalue.indexWire)
 beginPtrAccess(yyt->AsLvalue.access)
 beginPtrType(yyt->AsLvalue.expectedBaseType)
 yyt->AsLvalue.Lvalue = pLvalue;
 yyt->AsLvalue.AType = pAType;
 return yyt;
}

tTree mBitArrayCastLvalue
# if defined __STDC__ | defined __cplusplus
(tPosition pposition, tTree pLvalue)
# else
(pposition, pLvalue)
tPosition pposition;
tTree pLvalue;
# endif
{
 register tTree yyt;
 yyALLOC (yyt, Tree_NodeSize [kBitArrayCastLvalue])
 yyt->Kind = kBitArrayCastLvalue;
 yyt->yyHead.yyMark = 0;
 yyt->BitArrayCastLvalue.position = pposition;
 beginPtrContext(yyt->BitArrayCastLvalue.context)
 beginbool(yyt->BitArrayCastLvalue.skip)
 beginExprAttributes(yyt->BitArrayCastLvalue.attributes)
 beginbool(yyt->BitArrayCastLvalue.expectingChannel)
 beginbool(yyt->BitArrayCastLvalue.expectingEither)
 beginbool(yyt->BitArrayCastLvalue.isInput)
 beginbool(yyt->BitArrayCastLvalue.isPassive)
 beginbool(yyt->BitArrayCastLvalue.lockPassiveChannels)
 beginPtrInstance(yyt->BitArrayCastLvalue.instance)
 beginPtrSpanList(yyt->BitArrayCastLvalue.partitions)
 beginPtrSpanListList(yyt->BitArrayCastLvalue.indices)
 beginPtrWire(yyt->BitArrayCastLvalue.indexWire)
 beginPtrAccess(yyt->BitArrayCastLvalue.access)
 beginPtrType(yyt->BitArrayCastLvalue.expectedBaseType)
 yyt->BitArrayCastLvalue.Lvalue = pLvalue;
 return yyt;
}

tTree mGuards
# if defined __STDC__ | defined __cplusplus
(tPosition pposition)
# else
(pposition)
tPosition pposition;
# endif
{
 register tTree yyt;
 yyALLOC (yyt, Tree_NodeSize [kGuards])
 yyt->Kind = kGuards;
 yyt->yyHead.yyMark = 0;
 yyt->Guards.position = pposition;
 beginPtrContext(yyt->Guards.context)
 beginbool(yyt->Guards.skip)
 beginPtrExprAttributesList(yyt->Guards.exprAttributes)
 beginPtrCommandAttributesList(yyt->Guards.commandAttributes)
 return yyt;
}

tTree mNullGuards
# if defined __STDC__ | defined __cplusplus
(tPosition pposition)
# else
(pposition)
tPosition pposition;
# endif
{
 register tTree yyt;
 yyALLOC (yyt, Tree_NodeSize [kNullGuards])
 yyt->Kind = kNullGuards;
 yyt->yyHead.yyMark = 0;
 yyt->NullGuards.position = pposition;
 beginPtrContext(yyt->NullGuards.context)
 beginbool(yyt->NullGuards.skip)
 beginPtrExprAttributesList(yyt->NullGuards.exprAttributes)
 beginPtrCommandAttributesList(yyt->NullGuards.commandAttributes)
 return yyt;
}

tTree mGuard
# if defined __STDC__ | defined __cplusplus
(tPosition pposition, tTree pnext, tTree pExpr, tTree pCommand)
# else
(pposition, pnext, pExpr, pCommand)
tPosition pposition;
tTree pnext;
tTree pExpr;
tTree pCommand;
# endif
{
 register tTree yyt;
 yyALLOC (yyt, Tree_NodeSize [kGuard])
 yyt->Kind = kGuard;
 yyt->yyHead.yyMark = 0;
 yyt->Guard.position = pposition;
 beginPtrContext(yyt->Guard.context)
 beginbool(yyt->Guard.skip)
 beginPtrExprAttributesList(yyt->Guard.exprAttributes)
 beginPtrCommandAttributesList(yyt->Guard.commandAttributes)
 yyt->Guard.next = pnext;
 yyt->Guard.Expr = pExpr;
 yyt->Guard.Command = pCommand;
 return yyt;
}

tTree mPortGuards
# if defined __STDC__ | defined __cplusplus
(tPosition pposition)
# else
(pposition)
tPosition pposition;
# endif
{
 register tTree yyt;
 yyALLOC (yyt, Tree_NodeSize [kPortGuards])
 yyt->Kind = kPortGuards;
 yyt->yyHead.yyMark = 0;
 yyt->PortGuards.position = pposition;
 beginPtrContext(yyt->PortGuards.context)
 beginbool(yyt->PortGuards.skip)
 beginProcedureArgsType(yyt->PortGuards.portsType)
 beginPtrInstanceList(yyt->PortGuards.ports)
 beginunsigned(yyt->PortGuards.portCount)
 beginPtrInstanceList(yyt->PortGuards.extraPortInstances)
 beginbool(yyt->PortGuards.hasParameters)
 return yyt;
}

tTree mNullPortGuards
# if defined __STDC__ | defined __cplusplus
(tPosition pposition)
# else
(pposition)
tPosition pposition;
# endif
{
 register tTree yyt;
 yyALLOC (yyt, Tree_NodeSize [kNullPortGuards])
 yyt->Kind = kNullPortGuards;
 yyt->yyHead.yyMark = 0;
 yyt->NullPortGuards.position = pposition;
 beginPtrContext(yyt->NullPortGuards.context)
 beginbool(yyt->NullPortGuards.skip)
 beginProcedureArgsType(yyt->NullPortGuards.portsType)
 beginPtrInstanceList(yyt->NullPortGuards.ports)
 beginunsigned(yyt->NullPortGuards.portCount)
 beginPtrInstanceList(yyt->NullPortGuards.extraPortInstances)
 beginbool(yyt->NullPortGuards.hasParameters)
 return yyt;
}

tTree mPortGuard
# if defined __STDC__ | defined __cplusplus
(tPosition pposition, tTree pnext, tTree pExpr, tTree pFormalPorts)
# else
(pposition, pnext, pExpr, pFormalPorts)
tPosition pposition;
tTree pnext;
tTree pExpr;
tTree pFormalPorts;
# endif
{
 register tTree yyt;
 yyALLOC (yyt, Tree_NodeSize [kPortGuard])
 yyt->Kind = kPortGuard;
 yyt->yyHead.yyMark = 0;
 yyt->PortGuard.position = pposition;
 beginPtrContext(yyt->PortGuard.context)
 beginbool(yyt->PortGuard.skip)
 beginProcedureArgsType(yyt->PortGuard.portsType)
 beginPtrInstanceList(yyt->PortGuard.ports)
 beginunsigned(yyt->PortGuard.portCount)
 beginPtrInstanceList(yyt->PortGuard.extraPortInstances)
 beginbool(yyt->PortGuard.hasParameters)
 yyt->PortGuard.next = pnext;
 yyt->PortGuard.Expr = pExpr;
 yyt->PortGuard.FormalPorts = pFormalPorts;
 return yyt;
}

tTree mDeclGuards
# if defined __STDC__ | defined __cplusplus
(tPosition pposition)
# else
(pposition)
tPosition pposition;
# endif
{
 register tTree yyt;
 yyALLOC (yyt, Tree_NodeSize [kDeclGuards])
 yyt->Kind = kDeclGuards;
 yyt->yyHead.yyMark = 0;
 yyt->DeclGuards.position = pposition;
 beginScope(yyt->DeclGuards.scope)
 beginPtrContext(yyt->DeclGuards.contextIn)
 beginPtrContext(yyt->DeclGuards.contextOut)
 beginbool(yyt->DeclGuards.skip)
 beginbool(yyt->DeclGuards.foundTrueGuard)
 return yyt;
}

tTree mNullDeclGuards
# if defined __STDC__ | defined __cplusplus
(tPosition pposition)
# else
(pposition)
tPosition pposition;
# endif
{
 register tTree yyt;
 yyALLOC (yyt, Tree_NodeSize [kNullDeclGuards])
 yyt->Kind = kNullDeclGuards;
 yyt->yyHead.yyMark = 0;
 yyt->NullDeclGuards.position = pposition;
 beginScope(yyt->NullDeclGuards.scope)
 beginPtrContext(yyt->NullDeclGuards.contextIn)
 beginPtrContext(yyt->NullDeclGuards.contextOut)
 beginbool(yyt->NullDeclGuards.skip)
 beginbool(yyt->NullDeclGuards.foundTrueGuard)
 return yyt;
}

tTree mDeclGuard
# if defined __STDC__ | defined __cplusplus
(tPosition pposition, tTree pnext, tTree pExpr, tTree pDecls)
# else
(pposition, pnext, pExpr, pDecls)
tPosition pposition;
tTree pnext;
tTree pExpr;
tTree pDecls;
# endif
{
 register tTree yyt;
 yyALLOC (yyt, Tree_NodeSize [kDeclGuard])
 yyt->Kind = kDeclGuard;
 yyt->yyHead.yyMark = 0;
 yyt->DeclGuard.position = pposition;
 beginScope(yyt->DeclGuard.scope)
 beginPtrContext(yyt->DeclGuard.contextIn)
 beginPtrContext(yyt->DeclGuard.contextOut)
 beginbool(yyt->DeclGuard.skip)
 beginbool(yyt->DeclGuard.foundTrueGuard)
 yyt->DeclGuard.next = pnext;
 yyt->DeclGuard.Expr = pExpr;
 yyt->DeclGuard.Decls = pDecls;
 return yyt;
}

tTree mCaseGuards
# if defined __STDC__ | defined __cplusplus
(tPosition pposition)
# else
(pposition)
tPosition pposition;
# endif
{
 register tTree yyt;
 yyALLOC (yyt, Tree_NodeSize [kCaseGuards])
 yyt->Kind = kCaseGuards;
 yyt->yyHead.yyMark = 0;
 yyt->CaseGuards.position = pposition;
 beginPtrContext(yyt->CaseGuards.context)
 beginbool(yyt->CaseGuards.skip)
 beginPtrImplicantListList(yyt->CaseGuards.implicantss)
 beginPtrImplicantList(yyt->CaseGuards.complementImplicants)
 beginPtrCommandAttributesList(yyt->CaseGuards.commandAttributes)
 beginPtrType(yyt->CaseGuards.switchType)
 beginPtrMP_INT(yyt->CaseGuards.switchValue)
 beginSignedBits(yyt->CaseGuards.maxRange)
 beginPtrMP_INT(yyt->CaseGuards.implicantMask)
 beginbool(yyt->CaseGuards.foundTrueGuard)
 return yyt;
}

tTree mNullCaseGuards
# if defined __STDC__ | defined __cplusplus
(tPosition pposition)
# else
(pposition)
tPosition pposition;
# endif
{
 register tTree yyt;
 yyALLOC (yyt, Tree_NodeSize [kNullCaseGuards])
 yyt->Kind = kNullCaseGuards;
 yyt->yyHead.yyMark = 0;
 yyt->NullCaseGuards.position = pposition;
 beginPtrContext(yyt->NullCaseGuards.context)
 beginbool(yyt->NullCaseGuards.skip)
 beginPtrImplicantListList(yyt->NullCaseGuards.implicantss)
 beginPtrImplicantList(yyt->NullCaseGuards.complementImplicants)
 beginPtrCommandAttributesList(yyt->NullCaseGuards.commandAttributes)
 beginPtrType(yyt->NullCaseGuards.switchType)
 beginPtrMP_INT(yyt->NullCaseGuards.switchValue)
 beginSignedBits(yyt->NullCaseGuards.maxRange)
 beginPtrMP_INT(yyt->NullCaseGuards.implicantMask)
 beginbool(yyt->NullCaseGuards.foundTrueGuard)
 return yyt;
}

tTree mCaseGuard
# if defined __STDC__ | defined __cplusplus
(tPosition pposition, tTree pnext)
# else
(pposition, pnext)
tPosition pposition;
tTree pnext;
# endif
{
 register tTree yyt;
 yyALLOC (yyt, Tree_NodeSize [kCaseGuard])
 yyt->Kind = kCaseGuard;
 yyt->yyHead.yyMark = 0;
 yyt->CaseGuard.position = pposition;
 beginPtrContext(yyt->CaseGuard.context)
 beginbool(yyt->CaseGuard.skip)
 beginPtrImplicantListList(yyt->CaseGuard.implicantss)
 beginPtrImplicantList(yyt->CaseGuard.complementImplicants)
 beginPtrCommandAttributesList(yyt->CaseGuard.commandAttributes)
 beginPtrType(yyt->CaseGuard.switchType)
 beginPtrMP_INT(yyt->CaseGuard.switchValue)
 beginSignedBits(yyt->CaseGuard.maxRange)
 beginPtrMP_INT(yyt->CaseGuard.implicantMask)
 beginbool(yyt->CaseGuard.foundTrueGuard)
 yyt->CaseGuard.next = pnext;
 return yyt;
}

tTree mCaseMatchGuard
# if defined __STDC__ | defined __cplusplus
(tPosition pposition, tTree pnext, tTree pCaseMatchs, tTree pCommand)
# else
(pposition, pnext, pCaseMatchs, pCommand)
tPosition pposition;
tTree pnext;
tTree pCaseMatchs;
tTree pCommand;
# endif
{
 register tTree yyt;
 yyALLOC (yyt, Tree_NodeSize [kCaseMatchGuard])
 yyt->Kind = kCaseMatchGuard;
 yyt->yyHead.yyMark = 0;
 yyt->CaseMatchGuard.position = pposition;
 beginPtrContext(yyt->CaseMatchGuard.context)
 beginbool(yyt->CaseMatchGuard.skip)
 beginPtrImplicantListList(yyt->CaseMatchGuard.implicantss)
 beginPtrImplicantList(yyt->CaseMatchGuard.complementImplicants)
 beginPtrCommandAttributesList(yyt->CaseMatchGuard.commandAttributes)
 beginPtrType(yyt->CaseMatchGuard.switchType)
 beginPtrMP_INT(yyt->CaseMatchGuard.switchValue)
 beginSignedBits(yyt->CaseMatchGuard.maxRange)
 beginPtrMP_INT(yyt->CaseMatchGuard.implicantMask)
 beginbool(yyt->CaseMatchGuard.foundTrueGuard)
 yyt->CaseMatchGuard.next = pnext;
 yyt->CaseMatchGuard.CaseMatchs = pCaseMatchs;
 yyt->CaseMatchGuard.Command = pCommand;
 return yyt;
}

tTree mForCaseGuard
# if defined __STDC__ | defined __cplusplus
(tPosition pposition, tTree pnext, tIdent pident, tTree pCaseMatchs, tTree pCommand)
# else
(pposition, pnext, pident, pCaseMatchs, pCommand)
tPosition pposition;
tTree pnext;
tIdent pident;
tTree pCaseMatchs;
tTree pCommand;
# endif
{
 register tTree yyt;
 yyALLOC (yyt, Tree_NodeSize [kForCaseGuard])
 yyt->Kind = kForCaseGuard;
 yyt->yyHead.yyMark = 0;
 yyt->ForCaseGuard.position = pposition;
 beginPtrContext(yyt->ForCaseGuard.context)
 beginbool(yyt->ForCaseGuard.skip)
 beginPtrImplicantListList(yyt->ForCaseGuard.implicantss)
 beginPtrImplicantList(yyt->ForCaseGuard.complementImplicants)
 beginPtrCommandAttributesList(yyt->ForCaseGuard.commandAttributes)
 beginPtrType(yyt->ForCaseGuard.switchType)
 beginPtrMP_INT(yyt->ForCaseGuard.switchValue)
 beginSignedBits(yyt->ForCaseGuard.maxRange)
 beginPtrMP_INT(yyt->ForCaseGuard.implicantMask)
 beginbool(yyt->ForCaseGuard.foundTrueGuard)
 yyt->ForCaseGuard.next = pnext;
 yyt->ForCaseGuard.ident = pident;
 yyt->ForCaseGuard.CaseMatchs = pCaseMatchs;
 yyt->ForCaseGuard.Command = pCommand;
 beginPtrInstance(yyt->ForCaseGuard.iterator)
 beginPtrMP_INT(yyt->ForCaseGuard.countValue)
 beginint(yyt->ForCaseGuard.lastErrorCount)
 beginPtrImplicantList(yyt->ForCaseGuard.implicants)
 beginPtrchar(yyt->ForCaseGuard.errorContextString)
 beginPtrchar(yyt->ForCaseGuard.valueStringPtr)
 return yyt;
}

tTree mChannelGuards
# if defined __STDC__ | defined __cplusplus
(tPosition pposition)
# else
(pposition)
tPosition pposition;
# endif
{
 register tTree yyt;
 yyALLOC (yyt, Tree_NodeSize [kChannelGuards])
 yyt->Kind = kChannelGuards;
 yyt->yyHead.yyMark = 0;
 yyt->ChannelGuards.position = pposition;
 beginPtrContext(yyt->ChannelGuards.context)
 beginbool(yyt->ChannelGuards.skip)
 beginPtrExprAttributesList(yyt->ChannelGuards.guardAttributes)
 beginPtrCommandAttributesList(yyt->ChannelGuards.commandAttributes)
 return yyt;
}

tTree mNullChannelGuards
# if defined __STDC__ | defined __cplusplus
(tPosition pposition)
# else
(pposition)
tPosition pposition;
# endif
{
 register tTree yyt;
 yyALLOC (yyt, Tree_NodeSize [kNullChannelGuards])
 yyt->Kind = kNullChannelGuards;
 yyt->yyHead.yyMark = 0;
 yyt->NullChannelGuards.position = pposition;
 beginPtrContext(yyt->NullChannelGuards.context)
 beginbool(yyt->NullChannelGuards.skip)
 beginPtrExprAttributesList(yyt->NullChannelGuards.guardAttributes)
 beginPtrCommandAttributesList(yyt->NullChannelGuards.commandAttributes)
 return yyt;
}

tTree mChannelGuard
# if defined __STDC__ | defined __cplusplus
(tPosition pposition, tTree pnext, tTree pLvalueLists, tTree pCommand)
# else
(pposition, pnext, pLvalueLists, pCommand)
tPosition pposition;
tTree pnext;
tTree pLvalueLists;
tTree pCommand;
# endif
{
 register tTree yyt;
 yyALLOC (yyt, Tree_NodeSize [kChannelGuard])
 yyt->Kind = kChannelGuard;
 yyt->yyHead.yyMark = 0;
 yyt->ChannelGuard.position = pposition;
 beginPtrContext(yyt->ChannelGuard.context)
 beginbool(yyt->ChannelGuard.skip)
 beginPtrExprAttributesList(yyt->ChannelGuard.guardAttributes)
 beginPtrCommandAttributesList(yyt->ChannelGuard.commandAttributes)
 yyt->ChannelGuard.next = pnext;
 yyt->ChannelGuard.LvalueLists = pLvalueLists;
 yyt->ChannelGuard.Command = pCommand;
 return yyt;
}

tTree mIdents
# if defined __STDC__ | defined __cplusplus
(tPosition pposition)
# else
(pposition)
tPosition pposition;
# endif
{
 register tTree yyt;
 yyALLOC (yyt, Tree_NodeSize [kIdents])
 yyt->Kind = kIdents;
 yyt->yyHead.yyMark = 0;
 yyt->Idents.position = pposition;
 beginPtrIdentList(yyt->Idents.idents)
 return yyt;
}

tTree mNullIdents
# if defined __STDC__ | defined __cplusplus
(tPosition pposition)
# else
(pposition)
tPosition pposition;
# endif
{
 register tTree yyt;
 yyALLOC (yyt, Tree_NodeSize [kNullIdents])
 yyt->Kind = kNullIdents;
 yyt->yyHead.yyMark = 0;
 yyt->NullIdents.position = pposition;
 beginPtrIdentList(yyt->NullIdents.idents)
 return yyt;
}

tTree mIdent
# if defined __STDC__ | defined __cplusplus
(tPosition pposition, tTree pnext, tIdent pident)
# else
(pposition, pnext, pident)
tPosition pposition;
tTree pnext;
tIdent pident;
# endif
{
 register tTree yyt;
 yyALLOC (yyt, Tree_NodeSize [kIdent])
 yyt->Kind = kIdent;
 yyt->yyHead.yyMark = 0;
 yyt->Ident.position = pposition;
 beginPtrIdentList(yyt->Ident.idents)
 yyt->Ident.next = pnext;
 yyt->Ident.ident = pident;
 return yyt;
}

tTree mExprLists
# if defined __STDC__ | defined __cplusplus
(tPosition pposition)
# else
(pposition)
tPosition pposition;
# endif
{
 register tTree yyt;
 yyALLOC (yyt, Tree_NodeSize [kExprLists])
 yyt->Kind = kExprLists;
 yyt->yyHead.yyMark = 0;
 yyt->ExprLists.position = pposition;
 beginPtrContext(yyt->ExprLists.context)
 beginbool(yyt->ExprLists.skip)
 beginPtrType(yyt->ExprLists.expectedType)
 beginPtrInstanceList(yyt->ExprLists.elements)
 beginPtrExprAttributesList(yyt->ExprLists.attributes)
 beginbool(yyt->ExprLists.expectConstants)
 return yyt;
}

tTree mNullExprLists
# if defined __STDC__ | defined __cplusplus
(tPosition pposition)
# else
(pposition)
tPosition pposition;
# endif
{
 register tTree yyt;
 yyALLOC (yyt, Tree_NodeSize [kNullExprLists])
 yyt->Kind = kNullExprLists;
 yyt->yyHead.yyMark = 0;
 yyt->NullExprLists.position = pposition;
 beginPtrContext(yyt->NullExprLists.context)
 beginbool(yyt->NullExprLists.skip)
 beginPtrType(yyt->NullExprLists.expectedType)
 beginPtrInstanceList(yyt->NullExprLists.elements)
 beginPtrExprAttributesList(yyt->NullExprLists.attributes)
 beginbool(yyt->NullExprLists.expectConstants)
 return yyt;
}

tTree mExprList
# if defined __STDC__ | defined __cplusplus
(tPosition pposition, tTree pnext, tTree pCoercedExpr)
# else
(pposition, pnext, pCoercedExpr)
tPosition pposition;
tTree pnext;
tTree pCoercedExpr;
# endif
{
 register tTree yyt;
 yyALLOC (yyt, Tree_NodeSize [kExprList])
 yyt->Kind = kExprList;
 yyt->yyHead.yyMark = 0;
 yyt->ExprList.position = pposition;
 beginPtrContext(yyt->ExprList.context)
 beginbool(yyt->ExprList.skip)
 beginPtrType(yyt->ExprList.expectedType)
 beginPtrInstanceList(yyt->ExprList.elements)
 beginPtrExprAttributesList(yyt->ExprList.attributes)
 beginbool(yyt->ExprList.expectConstants)
 yyt->ExprList.next = pnext;
 yyt->ExprList.CoercedExpr = pCoercedExpr;
 return yyt;
}

tTree mLvalueLists
# if defined __STDC__ | defined __cplusplus
(tPosition pposition)
# else
(pposition)
tPosition pposition;
# endif
{
 register tTree yyt;
 yyALLOC (yyt, Tree_NodeSize [kLvalueLists])
 yyt->Kind = kLvalueLists;
 yyt->yyHead.yyMark = 0;
 yyt->LvalueLists.position = pposition;
 beginPtrContext(yyt->LvalueLists.context)
 beginbool(yyt->LvalueLists.skip)
 beginPtrExprAttributesList(yyt->LvalueLists.attributes)
 beginPtrType(yyt->LvalueLists.expectedType)
 beginPtrInstanceList(yyt->LvalueLists.elements)
 beginPtrIdentList(yyt->LvalueLists.idents)
 beginbool(yyt->LvalueLists.isInput)
 beginbool(yyt->LvalueLists.isPassive)
 beginbool(yyt->LvalueLists.lockPassiveChannels)
 beginint(yyt->LvalueLists.isProcedurePorts)
 beginint(yyt->LvalueLists.allowArrayedChannels)
 return yyt;
}

tTree mNullLvalueLists
# if defined __STDC__ | defined __cplusplus
(tPosition pposition)
# else
(pposition)
tPosition pposition;
# endif
{
 register tTree yyt;
 yyALLOC (yyt, Tree_NodeSize [kNullLvalueLists])
 yyt->Kind = kNullLvalueLists;
 yyt->yyHead.yyMark = 0;
 yyt->NullLvalueLists.position = pposition;
 beginPtrContext(yyt->NullLvalueLists.context)
 beginbool(yyt->NullLvalueLists.skip)
 beginPtrExprAttributesList(yyt->NullLvalueLists.attributes)
 beginPtrType(yyt->NullLvalueLists.expectedType)
 beginPtrInstanceList(yyt->NullLvalueLists.elements)
 beginPtrIdentList(yyt->NullLvalueLists.idents)
 beginbool(yyt->NullLvalueLists.isInput)
 beginbool(yyt->NullLvalueLists.isPassive)
 beginbool(yyt->NullLvalueLists.lockPassiveChannels)
 beginint(yyt->NullLvalueLists.isProcedurePorts)
 beginint(yyt->NullLvalueLists.allowArrayedChannels)
 return yyt;
}

tTree mLvalueList
# if defined __STDC__ | defined __cplusplus
(tPosition pposition, tTree pnext)
# else
(pposition, pnext)
tPosition pposition;
tTree pnext;
# endif
{
 register tTree yyt;
 yyALLOC (yyt, Tree_NodeSize [kLvalueList])
 yyt->Kind = kLvalueList;
 yyt->yyHead.yyMark = 0;
 yyt->LvalueList.position = pposition;
 beginPtrContext(yyt->LvalueList.context)
 beginbool(yyt->LvalueList.skip)
 beginPtrExprAttributesList(yyt->LvalueList.attributes)
 beginPtrType(yyt->LvalueList.expectedType)
 beginPtrInstanceList(yyt->LvalueList.elements)
 beginPtrIdentList(yyt->LvalueList.idents)
 beginbool(yyt->LvalueList.isInput)
 beginbool(yyt->LvalueList.isPassive)
 beginbool(yyt->LvalueList.lockPassiveChannels)
 beginint(yyt->LvalueList.isProcedurePorts)
 beginint(yyt->LvalueList.allowArrayedChannels)
 yyt->LvalueList.next = pnext;
 return yyt;
}

tTree mLinkedBlock
# if defined __STDC__ | defined __cplusplus
(tPosition pposition, tTree pnext, tTree pBlock)
# else
(pposition, pnext, pBlock)
tPosition pposition;
tTree pnext;
tTree pBlock;
# endif
{
 register tTree yyt;
 yyALLOC (yyt, Tree_NodeSize [kLinkedBlock])
 yyt->Kind = kLinkedBlock;
 yyt->yyHead.yyMark = 0;
 yyt->LinkedBlock.position = pposition;
 beginPtrContext(yyt->LinkedBlock.context)
 beginbool(yyt->LinkedBlock.skip)
 beginPtrExprAttributesList(yyt->LinkedBlock.attributes)
 beginPtrType(yyt->LinkedBlock.expectedType)
 beginPtrInstanceList(yyt->LinkedBlock.elements)
 beginPtrIdentList(yyt->LinkedBlock.idents)
 beginbool(yyt->LinkedBlock.isInput)
 beginbool(yyt->LinkedBlock.isPassive)
 beginbool(yyt->LinkedBlock.lockPassiveChannels)
 beginint(yyt->LinkedBlock.isProcedurePorts)
 beginint(yyt->LinkedBlock.allowArrayedChannels)
 yyt->LinkedBlock.next = pnext;
 yyt->LinkedBlock.Block = pBlock;
 return yyt;
}

tTree mLinkedChannel
# if defined __STDC__ | defined __cplusplus
(tPosition pposition, tTree pnext, tIdent pident, tTree pLvalue)
# else
(pposition, pnext, pident, pLvalue)
tPosition pposition;
tTree pnext;
tIdent pident;
tTree pLvalue;
# endif
{
 register tTree yyt;
 yyALLOC (yyt, Tree_NodeSize [kLinkedChannel])
 yyt->Kind = kLinkedChannel;
 yyt->yyHead.yyMark = 0;
 yyt->LinkedChannel.position = pposition;
 beginPtrContext(yyt->LinkedChannel.context)
 beginbool(yyt->LinkedChannel.skip)
 beginPtrExprAttributesList(yyt->LinkedChannel.attributes)
 beginPtrType(yyt->LinkedChannel.expectedType)
 beginPtrInstanceList(yyt->LinkedChannel.elements)
 beginPtrIdentList(yyt->LinkedChannel.idents)
 beginbool(yyt->LinkedChannel.isInput)
 beginbool(yyt->LinkedChannel.isPassive)
 beginbool(yyt->LinkedChannel.lockPassiveChannels)
 beginint(yyt->LinkedChannel.isProcedurePorts)
 beginint(yyt->LinkedChannel.allowArrayedChannels)
 yyt->LinkedChannel.next = pnext;
 yyt->LinkedChannel.ident = pident;
 yyt->LinkedChannel.Lvalue = pLvalue;
 return yyt;
}

tTree mCaseMatchs
# if defined __STDC__ | defined __cplusplus
(tPosition pposition)
# else
(pposition)
tPosition pposition;
# endif
{
 register tTree yyt;
 yyALLOC (yyt, Tree_NodeSize [kCaseMatchs])
 yyt->Kind = kCaseMatchs;
 yyt->yyHead.yyMark = 0;
 yyt->CaseMatchs.position = pposition;
 beginPtrContext(yyt->CaseMatchs.context)
 beginbool(yyt->CaseMatchs.skip)
 beginPtrImplicantList(yyt->CaseMatchs.implicants)
 beginPtrType(yyt->CaseMatchs.expectedType)
 beginbool(yyt->CaseMatchs.allowOverlappingMatches)
 beginbool(yyt->CaseMatchs.isSpan)
 return yyt;
}

tTree mNullCaseMatchs
# if defined __STDC__ | defined __cplusplus
(tPosition pposition)
# else
(pposition)
tPosition pposition;
# endif
{
 register tTree yyt;
 yyALLOC (yyt, Tree_NodeSize [kNullCaseMatchs])
 yyt->Kind = kNullCaseMatchs;
 yyt->yyHead.yyMark = 0;
 yyt->NullCaseMatchs.position = pposition;
 beginPtrContext(yyt->NullCaseMatchs.context)
 beginbool(yyt->NullCaseMatchs.skip)
 beginPtrImplicantList(yyt->NullCaseMatchs.implicants)
 beginPtrType(yyt->NullCaseMatchs.expectedType)
 beginbool(yyt->NullCaseMatchs.allowOverlappingMatches)
 beginbool(yyt->NullCaseMatchs.isSpan)
 return yyt;
}

tTree mCaseMatch
# if defined __STDC__ | defined __cplusplus
(tPosition pposition, tTree pnext)
# else
(pposition, pnext)
tPosition pposition;
tTree pnext;
# endif
{
 register tTree yyt;
 yyALLOC (yyt, Tree_NodeSize [kCaseMatch])
 yyt->Kind = kCaseMatch;
 yyt->yyHead.yyMark = 0;
 yyt->CaseMatch.position = pposition;
 beginPtrContext(yyt->CaseMatch.context)
 beginbool(yyt->CaseMatch.skip)
 beginPtrImplicantList(yyt->CaseMatch.implicants)
 beginPtrType(yyt->CaseMatch.expectedType)
 beginbool(yyt->CaseMatch.allowOverlappingMatches)
 beginbool(yyt->CaseMatch.isSpan)
 yyt->CaseMatch.next = pnext;
 return yyt;
}

tTree mCaseRange
# if defined __STDC__ | defined __cplusplus
(tPosition pposition, tTree pnext, tTree pRange)
# else
(pposition, pnext, pRange)
tPosition pposition;
tTree pnext;
tTree pRange;
# endif
{
 register tTree yyt;
 yyALLOC (yyt, Tree_NodeSize [kCaseRange])
 yyt->Kind = kCaseRange;
 yyt->yyHead.yyMark = 0;
 yyt->CaseRange.position = pposition;
 beginPtrContext(yyt->CaseRange.context)
 beginbool(yyt->CaseRange.skip)
 beginPtrImplicantList(yyt->CaseRange.implicants)
 beginPtrType(yyt->CaseRange.expectedType)
 beginbool(yyt->CaseRange.allowOverlappingMatches)
 beginbool(yyt->CaseRange.isSpan)
 yyt->CaseRange.next = pnext;
 yyt->CaseRange.Range = pRange;
 return yyt;
}

tTree mCaseImplicant
# if defined __STDC__ | defined __cplusplus
(tPosition pposition, tTree pnext, tTree pExpr)
# else
(pposition, pnext, pExpr)
tPosition pposition;
tTree pnext;
tTree pExpr;
# endif
{
 register tTree yyt;
 yyALLOC (yyt, Tree_NodeSize [kCaseImplicant])
 yyt->Kind = kCaseImplicant;
 yyt->yyHead.yyMark = 0;
 yyt->CaseImplicant.position = pposition;
 beginPtrContext(yyt->CaseImplicant.context)
 beginbool(yyt->CaseImplicant.skip)
 beginPtrImplicantList(yyt->CaseImplicant.implicants)
 beginPtrType(yyt->CaseImplicant.expectedType)
 beginbool(yyt->CaseImplicant.allowOverlappingMatches)
 beginbool(yyt->CaseImplicant.isSpan)
 yyt->CaseImplicant.next = pnext;
 yyt->CaseImplicant.Expr = pExpr;
 return yyt;
}

tTree mEnumElems
# if defined __STDC__ | defined __cplusplus
(tPosition pposition)
# else
(pposition)
tPosition pposition;
# endif
{
 register tTree yyt;
 yyALLOC (yyt, Tree_NodeSize [kEnumElems])
 yyt->Kind = kEnumElems;
 yyt->yyHead.yyMark = 0;
 yyt->EnumElems.position = pposition;
 beginPtrContext(yyt->EnumElems.context)
 beginbool(yyt->EnumElems.skip)
 beginPtrMP_INT(yyt->EnumElems.elementValue)
 beginPtrType(yyt->EnumElems.typeIn)
 beginPtrType(yyt->EnumElems.typeOut)
 beginbool(yyt->EnumElems.hasOverType)
 beginPtrBindingList(yyt->EnumElems.elementsTail)
 return yyt;
}

tTree mNullEnumElems
# if defined __STDC__ | defined __cplusplus
(tPosition pposition)
# else
(pposition)
tPosition pposition;
# endif
{
 register tTree yyt;
 yyALLOC (yyt, Tree_NodeSize [kNullEnumElems])
 yyt->Kind = kNullEnumElems;
 yyt->yyHead.yyMark = 0;
 yyt->NullEnumElems.position = pposition;
 beginPtrContext(yyt->NullEnumElems.context)
 beginbool(yyt->NullEnumElems.skip)
 beginPtrMP_INT(yyt->NullEnumElems.elementValue)
 beginPtrType(yyt->NullEnumElems.typeIn)
 beginPtrType(yyt->NullEnumElems.typeOut)
 beginbool(yyt->NullEnumElems.hasOverType)
 beginPtrBindingList(yyt->NullEnumElems.elementsTail)
 return yyt;
}

tTree mEnumElem
# if defined __STDC__ | defined __cplusplus
(tPosition pposition, tTree pnext, tIdent pident)
# else
(pposition, pnext, pident)
tPosition pposition;
tTree pnext;
tIdent pident;
# endif
{
 register tTree yyt;
 yyALLOC (yyt, Tree_NodeSize [kEnumElem])
 yyt->Kind = kEnumElem;
 yyt->yyHead.yyMark = 0;
 yyt->EnumElem.position = pposition;
 beginPtrContext(yyt->EnumElem.context)
 beginbool(yyt->EnumElem.skip)
 beginPtrMP_INT(yyt->EnumElem.elementValue)
 beginPtrType(yyt->EnumElem.typeIn)
 beginPtrType(yyt->EnumElem.typeOut)
 beginbool(yyt->EnumElem.hasOverType)
 beginPtrBindingList(yyt->EnumElem.elementsTail)
 yyt->EnumElem.next = pnext;
 yyt->EnumElem.ident = pident;
 return yyt;
}

tTree mDefaultValuedEnumElem
# if defined __STDC__ | defined __cplusplus
(tPosition pposition, tTree pnext, tIdent pident)
# else
(pposition, pnext, pident)
tPosition pposition;
tTree pnext;
tIdent pident;
# endif
{
 register tTree yyt;
 yyALLOC (yyt, Tree_NodeSize [kDefaultValuedEnumElem])
 yyt->Kind = kDefaultValuedEnumElem;
 yyt->yyHead.yyMark = 0;
 yyt->DefaultValuedEnumElem.position = pposition;
 beginPtrContext(yyt->DefaultValuedEnumElem.context)
 beginbool(yyt->DefaultValuedEnumElem.skip)
 beginPtrMP_INT(yyt->DefaultValuedEnumElem.elementValue)
 beginPtrType(yyt->DefaultValuedEnumElem.typeIn)
 beginPtrType(yyt->DefaultValuedEnumElem.typeOut)
 beginbool(yyt->DefaultValuedEnumElem.hasOverType)
 beginPtrBindingList(yyt->DefaultValuedEnumElem.elementsTail)
 yyt->DefaultValuedEnumElem.next = pnext;
 yyt->DefaultValuedEnumElem.ident = pident;
 return yyt;
}

tTree mValuedEnumElem
# if defined __STDC__ | defined __cplusplus
(tPosition pposition, tTree pnext, tIdent pident, tTree pCoercedExpr)
# else
(pposition, pnext, pident, pCoercedExpr)
tPosition pposition;
tTree pnext;
tIdent pident;
tTree pCoercedExpr;
# endif
{
 register tTree yyt;
 yyALLOC (yyt, Tree_NodeSize [kValuedEnumElem])
 yyt->Kind = kValuedEnumElem;
 yyt->yyHead.yyMark = 0;
 yyt->ValuedEnumElem.position = pposition;
 beginPtrContext(yyt->ValuedEnumElem.context)
 beginbool(yyt->ValuedEnumElem.skip)
 beginPtrMP_INT(yyt->ValuedEnumElem.elementValue)
 beginPtrType(yyt->ValuedEnumElem.typeIn)
 beginPtrType(yyt->ValuedEnumElem.typeOut)
 beginbool(yyt->ValuedEnumElem.hasOverType)
 beginPtrBindingList(yyt->ValuedEnumElem.elementsTail)
 yyt->ValuedEnumElem.next = pnext;
 yyt->ValuedEnumElem.ident = pident;
 yyt->ValuedEnumElem.CoercedExpr = pCoercedExpr;
 return yyt;
}

tTree mRecordElems
# if defined __STDC__ | defined __cplusplus
(tPosition pposition)
# else
(pposition)
tPosition pposition;
# endif
{
 register tTree yyt;
 yyALLOC (yyt, Tree_NodeSize [kRecordElems])
 yyt->Kind = kRecordElems;
 yyt->yyHead.yyMark = 0;
 yyt->RecordElems.position = pposition;
 beginPtrContext(yyt->RecordElems.context)
 beginbool(yyt->RecordElems.skip)
 beginPtrType(yyt->RecordElems.typeIn)
 beginPtrType(yyt->RecordElems.typeOut)
 beginbool(yyt->RecordElems.hasOverType)
 beginPtrInstanceList(yyt->RecordElems.elementsTail)
 return yyt;
}

tTree mNullRecordElems
# if defined __STDC__ | defined __cplusplus
(tPosition pposition)
# else
(pposition)
tPosition pposition;
# endif
{
 register tTree yyt;
 yyALLOC (yyt, Tree_NodeSize [kNullRecordElems])
 yyt->Kind = kNullRecordElems;
 yyt->yyHead.yyMark = 0;
 yyt->NullRecordElems.position = pposition;
 beginPtrContext(yyt->NullRecordElems.context)
 beginbool(yyt->NullRecordElems.skip)
 beginPtrType(yyt->NullRecordElems.typeIn)
 beginPtrType(yyt->NullRecordElems.typeOut)
 beginbool(yyt->NullRecordElems.hasOverType)
 beginPtrInstanceList(yyt->NullRecordElems.elementsTail)
 return yyt;
}

tTree mRecordElem
# if defined __STDC__ | defined __cplusplus
(tPosition pposition, tTree pnext, tTree pIdents, tTree pAType)
# else
(pposition, pnext, pIdents, pAType)
tPosition pposition;
tTree pnext;
tTree pIdents;
tTree pAType;
# endif
{
 register tTree yyt;
 yyALLOC (yyt, Tree_NodeSize [kRecordElem])
 yyt->Kind = kRecordElem;
 yyt->yyHead.yyMark = 0;
 yyt->RecordElem.position = pposition;
 beginPtrContext(yyt->RecordElem.context)
 beginbool(yyt->RecordElem.skip)
 beginPtrType(yyt->RecordElem.typeIn)
 beginPtrType(yyt->RecordElem.typeOut)
 beginbool(yyt->RecordElem.hasOverType)
 beginPtrInstanceList(yyt->RecordElem.elementsTail)
 yyt->RecordElem.next = pnext;
 yyt->RecordElem.Idents = pIdents;
 yyt->RecordElem.AType = pAType;
 return yyt;
}

tTree mValDecls
# if defined __STDC__ | defined __cplusplus
(tPosition pposition)
# else
(pposition)
tPosition pposition;
# endif
{
 register tTree yyt;
 yyALLOC (yyt, Tree_NodeSize [kValDecls])
 yyt->Kind = kValDecls;
 yyt->yyHead.yyMark = 0;
 yyt->ValDecls.position = pposition;
 beginPtrContext(yyt->ValDecls.contextIn)
 beginPtrContext(yyt->ValDecls.contextOut)
 beginbool(yyt->ValDecls.skip)
 return yyt;
}

tTree mNullValDecls
# if defined __STDC__ | defined __cplusplus
(tPosition pposition)
# else
(pposition)
tPosition pposition;
# endif
{
 register tTree yyt;
 yyALLOC (yyt, Tree_NodeSize [kNullValDecls])
 yyt->Kind = kNullValDecls;
 yyt->yyHead.yyMark = 0;
 yyt->NullValDecls.position = pposition;
 beginPtrContext(yyt->NullValDecls.contextIn)
 beginPtrContext(yyt->NullValDecls.contextOut)
 beginbool(yyt->NullValDecls.skip)
 return yyt;
}

tTree mValDecl
# if defined __STDC__ | defined __cplusplus
(tPosition pposition, tTree pnext, tIdent pident, tTree pExpr)
# else
(pposition, pnext, pident, pExpr)
tPosition pposition;
tTree pnext;
tIdent pident;
tTree pExpr;
# endif
{
 register tTree yyt;
 yyALLOC (yyt, Tree_NodeSize [kValDecl])
 yyt->Kind = kValDecl;
 yyt->yyHead.yyMark = 0;
 yyt->ValDecl.position = pposition;
 beginPtrContext(yyt->ValDecl.contextIn)
 beginPtrContext(yyt->ValDecl.contextOut)
 beginbool(yyt->ValDecl.skip)
 yyt->ValDecl.next = pnext;
 yyt->ValDecl.ident = pident;
 yyt->ValDecl.Expr = pExpr;
 return yyt;
}

typedef tTree * yyPtrtTree;

static FILE * yyf;

static void yyMark
# if defined __STDC__ | defined __cplusplus
 (register tTree yyt)
# else
 (yyt) register tTree yyt;
# endif
{
 for (;;) {
  if (yyt == NoTree || ++ yyt->yyHead.yyMark > 1) return;

  switch (yyt->Kind) {
case kBalsaProgram:
yyt = yyt->BalsaProgram.Decls; break;
case kDecl:
yyt = yyt->Decl.next; break;
case kImportDecl:
yyMark (yyt->ImportDecl.Idents);
yyt = yyt->ImportDecl.next; break;
case kFileDecl:
yyMark (yyt->FileDecl.Idents);
yyt = yyt->FileDecl.next; break;
case kTypeDecl:
yyMark (yyt->TypeDecl.AType);
yyt = yyt->TypeDecl.next; break;
case kConstantDecl:
yyMark (yyt->ConstantDecl.CoercedExpr);
yyMark (yyt->ConstantDecl.AType);
yyt = yyt->ConstantDecl.next; break;
case kVariableDecl:
yyMark (yyt->VariableDecl.Idents);
yyMark (yyt->VariableDecl.AType);
yyt = yyt->VariableDecl.next; break;
case kInitVariableDecl:
yyMark (yyt->InitVariableDecl.Idents);
yyMark (yyt->InitVariableDecl.CoercedExpr);
yyMark (yyt->InitVariableDecl.AType);
yyt = yyt->InitVariableDecl.next; break;
case kChannelDecl:
yyMark (yyt->ChannelDecl.Idents);
yyMark (yyt->ChannelDecl.AType);
yyt = yyt->ChannelDecl.next; break;
case kChannelArrayDecl:
yyMark (yyt->ChannelArrayDecl.Idents);
yyMark (yyt->ChannelArrayDecl.AType);
yyMark (yyt->ChannelArrayDecl.Range);
yyt = yyt->ChannelArrayDecl.next; break;
case kSyncDecl:
yyMark (yyt->SyncDecl.Idents);
yyt = yyt->SyncDecl.next; break;
case kSyncArrayDecl:
yyMark (yyt->SyncArrayDecl.Idents);
yyMark (yyt->SyncArrayDecl.Range);
yyt = yyt->SyncArrayDecl.next; break;
case kProcedureDecl:
yyMark (yyt->ProcedureDecl.FormalPorts);
yyMark (yyt->ProcedureDecl.Block);
yyt = yyt->ProcedureDecl.next; break;
case kProcAliasDecl:
yyMark (yyt->ProcAliasDecl.ProcedureParams);
yyt = yyt->ProcAliasDecl.next; break;
case kFunctionDecl:
yyMark (yyt->FunctionDecl.FormalPorts);
yyMark (yyt->FunctionDecl.CoercedExpr);
yyMark (yyt->FunctionDecl.AType);
yyt = yyt->FunctionDecl.next; break;
case kBuiltinFunctionDecl:
yyMark (yyt->BuiltinFunctionDecl.FormalPorts);
yyMark (yyt->BuiltinFunctionDecl.AType);
yyt = yyt->BuiltinFunctionDecl.next; break;
case kSharedDecl:
yyMark (yyt->SharedDecl.Block);
yyt = yyt->SharedDecl.next; break;
case kPartDecl:
yyMark (yyt->PartDecl.FormalPorts);
yyMark (yyt->PartDecl.Comps);
yyt = yyt->PartDecl.next; break;
case kIfDecls:
yyMark (yyt->IfDecls.DeclGuards);
yyt = yyt->IfDecls.next; break;
case kIfElseDecls:
yyMark (yyt->IfElseDecls.DeclGuards);
yyMark (yyt->IfElseDecls.Decls);
yyt = yyt->IfElseDecls.next; break;
case kPrintDecl:
yyMark (yyt->PrintDecl.Expr);
yyMark (yyt->PrintDecl.ExprLists);
yyt = yyt->PrintDecl.next; break;
case kNullDecl:
yyt = yyt->NullDecl.next; break;
case kSpecifiedRange:
yyMark (yyt->SpecifiedRange.Left);
yyt = yyt->SpecifiedRange.Right; break;
case kTypeRange:
yyt = yyt->TypeRange.AType; break;
case kNumericType:
yyt = yyt->NumericType.Expr; break;
case kArrayType:
yyMark (yyt->ArrayType.AType);
yyt = yyt->ArrayType.Range; break;
case kNewType:
yyt = yyt->NewType.AType; break;
case kRecordType:
yyMark (yyt->RecordType.RecordElems);
yyt = yyt->RecordType.AType; break;
case kEnumType:
yyMark (yyt->EnumType.EnumElems);
yyt = yyt->EnumType.AType; break;
case kCoercedExpr:
yyt = yyt->CoercedExpr.Expr; break;
case kAggregateConsExpr:
yyt = yyt->AggregateConsExpr.ExprLists; break;
case kUnaryExpr:
yyt = yyt->UnaryExpr.Expr; break;
case kBinaryExpr:
yyMark (yyt->BinaryExpr.Left);
yyt = yyt->BinaryExpr.Right; break;
case kRecordElemExtractExpr:
yyt = yyt->RecordElemExtractExpr.Expr; break;
case kArrayExtractExpr:
yyMark (yyt->ArrayExtractExpr.Array);
yyt = yyt->ArrayExtractExpr.Subscript; break;
case kArraySliceExpr:
yyMark (yyt->ArraySliceExpr.Expr);
yyt = yyt->ArraySliceExpr.Range; break;
case kAsExpr:
yyMark (yyt->AsExpr.Expr);
yyt = yyt->AsExpr.AType; break;
case kBitArrayCastExpr:
yyt = yyt->BitArrayCastExpr.Expr; break;
case kLetExpr:
yyMark (yyt->LetExpr.ValDecls);
yyt = yyt->LetExpr.Expr; break;
case kFunctionCallExpr:
yyt = yyt->FunctionCallExpr.FunctionParams; break;
case kArrayAppendExpr:
yyMark (yyt->ArrayAppendExpr.Left);
yyt = yyt->ArrayAppendExpr.Right; break;
case kFormalPort:
yyt = yyt->FormalPort.next; break;
case kValuePort:
yyMark (yyt->ValuePort.Idents);
yyMark (yyt->ValuePort.AType);
yyt = yyt->ValuePort.next; break;
case kParamPort:
yyMark (yyt->ParamPort.Idents);
yyMark (yyt->ParamPort.AType);
yyt = yyt->ParamPort.next; break;
case kTypeParamPort:
yyMark (yyt->TypeParamPort.Idents);
yyt = yyt->TypeParamPort.next; break;
case kChannelPort:
yyMark (yyt->ChannelPort.Idents);
yyMark (yyt->ChannelPort.AType);
yyt = yyt->ChannelPort.next; break;
case kChannelPortArray:
yyMark (yyt->ChannelPortArray.Idents);
yyMark (yyt->ChannelPortArray.AType);
yyMark (yyt->ChannelPortArray.Range);
yyt = yyt->ChannelPortArray.next; break;
case kSyncPort:
yyMark (yyt->SyncPort.Idents);
yyt = yyt->SyncPort.next; break;
case kSyncPortArray:
yyMark (yyt->SyncPortArray.Idents);
yyMark (yyt->SyncPortArray.Range);
yyt = yyt->SyncPortArray.next; break;
case kIfPorts:
yyMark (yyt->IfPorts.PortGuards);
yyt = yyt->IfPorts.next; break;
case kIfElsePorts:
yyMark (yyt->IfElsePorts.PortGuards);
yyMark (yyt->IfElsePorts.FormalPorts);
yyt = yyt->IfElsePorts.next; break;
case kBlock:
yyMark (yyt->Block.Decls);
yyt = yyt->Block.Command; break;
case kParameter:
yyt = yyt->Parameter.next; break;
case kNumberParameter:
yyt = yyt->NumberParameter.next; break;
case kStringParameter:
yyt = yyt->StringParameter.next; break;
case kTypeParameter:
yyMark (yyt->TypeParameter.AType);
yyt = yyt->TypeParameter.next; break;
case kBreezeParameter:
yyt = yyt->BreezeParameter.next; break;
case kBreezeExprParameter:
yyMark (yyt->BreezeExprParameter.AType);
yyt = yyt->BreezeExprParameter.next; break;
case kBreezeTypeParameter:
yyt = yyt->BreezeTypeParameter.next; break;
case kComp:
yyt = yyt->Comp.next; break;
case kNormalComp:
yyMark (yyt->NormalComp.next);
yyt = yyt->NormalComp.Parameters; break;
case kUndeclaredComp:
yyMark (yyt->UndeclaredComp.next);
yyMark (yyt->UndeclaredComp.Parameters);
yyMark (yyt->UndeclaredComp.BreezeParameters);
yyt = yyt->UndeclaredComp.FormalPorts; break;
case kInputCommand:
yyMark (yyt->InputCommand.LHS);
yyt = yyt->InputCommand.RHS; break;
case kInputEncloseCommand:
yyMark (yyt->InputEncloseCommand.LvalueLists);
yyt = yyt->InputEncloseCommand.Command; break;
case kInputEncloseBangCommand:
yyMark (yyt->InputEncloseBangCommand.LvalueLists);
yyt = yyt->InputEncloseBangCommand.Command; break;
case kOutputCommand:
yyMark (yyt->OutputCommand.Lvalue);
yyt = yyt->OutputCommand.CoercedExpr; break;
case kSyncCommand:
yyt = yyt->SyncCommand.Lvalue; break;
case kAssignmentCommand:
yyMark (yyt->AssignmentCommand.Lvalue);
yyt = yyt->AssignmentCommand.CoercedExpr; break;
case kBlockCommand:
yyt = yyt->BlockCommand.Block; break;
case kSequentialCommand:
yyMark (yyt->SequentialCommand.Left);
yyt = yyt->SequentialCommand.Right; break;
case kParallelCommand:
yyMark (yyt->ParallelCommand.Left);
yyt = yyt->ParallelCommand.Right; break;
case kLoopCommand:
yyt = yyt->LoopCommand.Command; break;
case kWhileGuardsCommand:
yyt = yyt->WhileGuardsCommand.Guards; break;
case kWhileGuardsAlsoCommand:
yyMark (yyt->WhileGuardsAlsoCommand.Guards);
yyt = yyt->WhileGuardsAlsoCommand.Command; break;
case kCommandWhileExprCommand:
yyMark (yyt->CommandWhileExprCommand.Command);
yyt = yyt->CommandWhileExprCommand.Expr; break;
case kCommandWhileGuardsCommand:
yyMark (yyt->CommandWhileGuardsCommand.Command);
yyt = yyt->CommandWhileGuardsCommand.Guards; break;
case kCommandWhileGuardsAlsoCommand:
yyMark (yyt->CommandWhileGuardsAlsoCommand.FirstCommand);
yyMark (yyt->CommandWhileGuardsAlsoCommand.Guards);
yyt = yyt->CommandWhileGuardsAlsoCommand.AlsoCommand; break;
case kIfCommand:
yyt = yyt->IfCommand.Guards; break;
case kIfElseCommand:
yyMark (yyt->IfElseCommand.Guards);
yyt = yyt->IfElseCommand.Command; break;
case kCaseCommand:
yyMark (yyt->CaseCommand.Expr);
yyMark (yyt->CaseCommand.CaseGuards);
yyt = yyt->CaseCommand.Command; break;
case kForCommand:
yyMark (yyt->ForCommand.Range);
yyt = yyt->ForCommand.Command; break;
case kProcedureCallCommonCommand:
yyt = yyt->ProcedureCallCommonCommand.ProcedureParams; break;
case kSelectCommand:
yyt = yyt->SelectCommand.ChannelGuards; break;
case kSelectBangCommand:
yyt = yyt->SelectBangCommand.ChannelGuards; break;
case kArbitrateCommand:
yyt = yyt->ArbitrateCommand.ChannelGuards; break;
case kPrintCommand:
yyMark (yyt->PrintCommand.Expr);
yyt = yyt->PrintCommand.ExprLists; break;
case kSinkCommand:
yyt = yyt->SinkCommand.Expr; break;
case kProcParam:
yyt = yyt->ProcParam.next; break;
case kExprProcParam:
yyMark (yyt->ExprProcParam.CoercedExpr);
yyt = yyt->ExprProcParam.next; break;
case kTypeProcParam:
yyMark (yyt->TypeProcParam.AType);
yyt = yyt->TypeProcParam.next; break;
case kVarReadProcParam:
yyMark (yyt->VarReadProcParam.CoercedExpr);
yyt = yyt->VarReadProcParam.next; break;
case kVarWriteProcParam:
yyMark (yyt->VarWriteProcParam.Lvalue);
yyt = yyt->VarWriteProcParam.next; break;
case kBlockProcParam:
yyMark (yyt->BlockProcParam.Block);
yyt = yyt->BlockProcParam.next; break;
case kFuncParam:
yyt = yyt->FuncParam.next; break;
case kExprFuncParam:
yyMark (yyt->ExprFuncParam.CoercedExpr);
yyt = yyt->ExprFuncParam.next; break;
case kTypeFuncParam:
yyMark (yyt->TypeFuncParam.AType);
yyt = yyt->TypeFuncParam.next; break;
case kRecordElemLvalue:
yyt = yyt->RecordElemLvalue.Lvalue; break;
case kArrayElemLvalue:
yyMark (yyt->ArrayElemLvalue.Lvalue);
yyt = yyt->ArrayElemLvalue.CoercedExpr; break;
case kArraySliceLvalue:
yyMark (yyt->ArraySliceLvalue.Lvalue);
yyt = yyt->ArraySliceLvalue.Range; break;
case kArrayAppendLvalue:
yyMark (yyt->ArrayAppendLvalue.Left);
yyt = yyt->ArrayAppendLvalue.Right; break;
case kArrayConsLvalue:
yyt = yyt->ArrayConsLvalue.LvalueLists; break;
case kAsLvalue:
yyMark (yyt->AsLvalue.Lvalue);
yyt = yyt->AsLvalue.AType; break;
case kBitArrayCastLvalue:
yyt = yyt->BitArrayCastLvalue.Lvalue; break;
case kGuard:
yyMark (yyt->Guard.Expr);
yyMark (yyt->Guard.Command);
yyt = yyt->Guard.next; break;
case kPortGuard:
yyMark (yyt->PortGuard.Expr);
yyMark (yyt->PortGuard.FormalPorts);
yyt = yyt->PortGuard.next; break;
case kDeclGuard:
yyMark (yyt->DeclGuard.Expr);
yyMark (yyt->DeclGuard.Decls);
yyt = yyt->DeclGuard.next; break;
case kCaseGuard:
yyt = yyt->CaseGuard.next; break;
case kCaseMatchGuard:
yyMark (yyt->CaseMatchGuard.CaseMatchs);
yyMark (yyt->CaseMatchGuard.Command);
yyt = yyt->CaseMatchGuard.next; break;
case kForCaseGuard:
yyMark (yyt->ForCaseGuard.CaseMatchs);
yyMark (yyt->ForCaseGuard.Command);
yyt = yyt->ForCaseGuard.next; break;
case kChannelGuard:
yyMark (yyt->ChannelGuard.LvalueLists);
yyMark (yyt->ChannelGuard.Command);
yyt = yyt->ChannelGuard.next; break;
case kIdent:
yyt = yyt->Ident.next; break;
case kExprList:
yyMark (yyt->ExprList.CoercedExpr);
yyt = yyt->ExprList.next; break;
case kLvalueList:
yyt = yyt->LvalueList.next; break;
case kLinkedBlock:
yyMark (yyt->LinkedBlock.Block);
yyt = yyt->LinkedBlock.next; break;
case kLinkedChannel:
yyMark (yyt->LinkedChannel.Lvalue);
yyt = yyt->LinkedChannel.next; break;
case kCaseMatch:
yyt = yyt->CaseMatch.next; break;
case kCaseRange:
yyMark (yyt->CaseRange.Range);
yyt = yyt->CaseRange.next; break;
case kCaseImplicant:
yyMark (yyt->CaseImplicant.Expr);
yyt = yyt->CaseImplicant.next; break;
case kEnumElem:
yyt = yyt->EnumElem.next; break;
case kDefaultValuedEnumElem:
yyt = yyt->DefaultValuedEnumElem.next; break;
case kValuedEnumElem:
yyMark (yyt->ValuedEnumElem.CoercedExpr);
yyt = yyt->ValuedEnumElem.next; break;
case kRecordElem:
yyMark (yyt->RecordElem.Idents);
yyMark (yyt->RecordElem.AType);
yyt = yyt->RecordElem.next; break;
case kValDecl:
yyMark (yyt->ValDecl.Expr);
yyt = yyt->ValDecl.next; break;
  default: return;
  }
 }
}

# define yyNil	0374
# define yyNoLabel	0375
# define yyLabelDef	0376
# define yyLabelUse	0377

tTree ReverseTree
# if defined __STDC__ | defined __cplusplus
 (tTree yyOld)
# else
 (yyOld) tTree yyOld;
# endif
{
 register tTree yyNew, yyNext, yyTail;
 yyNew = yyOld;
 yyTail = yyOld;
 for (;;) {
  switch (yyOld->Kind) {
case kDecl: yyNext = yyOld->Decl.next; yyOld->Decl.next = yyNew; break;
case kImportDecl: yyNext = yyOld->ImportDecl.next; yyOld->ImportDecl.next = yyNew; break;
case kFileDecl: yyNext = yyOld->FileDecl.next; yyOld->FileDecl.next = yyNew; break;
case kTypeDecl: yyNext = yyOld->TypeDecl.next; yyOld->TypeDecl.next = yyNew; break;
case kConstantDecl: yyNext = yyOld->ConstantDecl.next; yyOld->ConstantDecl.next = yyNew; break;
case kVariableDecl: yyNext = yyOld->VariableDecl.next; yyOld->VariableDecl.next = yyNew; break;
case kInitVariableDecl: yyNext = yyOld->InitVariableDecl.next; yyOld->InitVariableDecl.next = yyNew; break;
case kChannelDecl: yyNext = yyOld->ChannelDecl.next; yyOld->ChannelDecl.next = yyNew; break;
case kChannelArrayDecl: yyNext = yyOld->ChannelArrayDecl.next; yyOld->ChannelArrayDecl.next = yyNew; break;
case kSyncDecl: yyNext = yyOld->SyncDecl.next; yyOld->SyncDecl.next = yyNew; break;
case kSyncArrayDecl: yyNext = yyOld->SyncArrayDecl.next; yyOld->SyncArrayDecl.next = yyNew; break;
case kProcedureDecl: yyNext = yyOld->ProcedureDecl.next; yyOld->ProcedureDecl.next = yyNew; break;
case kProcAliasDecl: yyNext = yyOld->ProcAliasDecl.next; yyOld->ProcAliasDecl.next = yyNew; break;
case kFunctionDecl: yyNext = yyOld->FunctionDecl.next; yyOld->FunctionDecl.next = yyNew; break;
case kBuiltinFunctionDecl: yyNext = yyOld->BuiltinFunctionDecl.next; yyOld->BuiltinFunctionDecl.next = yyNew; break;
case kSharedDecl: yyNext = yyOld->SharedDecl.next; yyOld->SharedDecl.next = yyNew; break;
case kPartDecl: yyNext = yyOld->PartDecl.next; yyOld->PartDecl.next = yyNew; break;
case kIfDecls: yyNext = yyOld->IfDecls.next; yyOld->IfDecls.next = yyNew; break;
case kIfElseDecls: yyNext = yyOld->IfElseDecls.next; yyOld->IfElseDecls.next = yyNew; break;
case kPrintDecl: yyNext = yyOld->PrintDecl.next; yyOld->PrintDecl.next = yyNew; break;
case kNullDecl: yyNext = yyOld->NullDecl.next; yyOld->NullDecl.next = yyNew; break;
case kFormalPort: yyNext = yyOld->FormalPort.next; yyOld->FormalPort.next = yyNew; break;
case kValuePort: yyNext = yyOld->ValuePort.next; yyOld->ValuePort.next = yyNew; break;
case kParamPort: yyNext = yyOld->ParamPort.next; yyOld->ParamPort.next = yyNew; break;
case kTypeParamPort: yyNext = yyOld->TypeParamPort.next; yyOld->TypeParamPort.next = yyNew; break;
case kChannelPort: yyNext = yyOld->ChannelPort.next; yyOld->ChannelPort.next = yyNew; break;
case kChannelPortArray: yyNext = yyOld->ChannelPortArray.next; yyOld->ChannelPortArray.next = yyNew; break;
case kSyncPort: yyNext = yyOld->SyncPort.next; yyOld->SyncPort.next = yyNew; break;
case kSyncPortArray: yyNext = yyOld->SyncPortArray.next; yyOld->SyncPortArray.next = yyNew; break;
case kIfPorts: yyNext = yyOld->IfPorts.next; yyOld->IfPorts.next = yyNew; break;
case kIfElsePorts: yyNext = yyOld->IfElsePorts.next; yyOld->IfElsePorts.next = yyNew; break;
case kParameter: yyNext = yyOld->Parameter.next; yyOld->Parameter.next = yyNew; break;
case kNumberParameter: yyNext = yyOld->NumberParameter.next; yyOld->NumberParameter.next = yyNew; break;
case kStringParameter: yyNext = yyOld->StringParameter.next; yyOld->StringParameter.next = yyNew; break;
case kTypeParameter: yyNext = yyOld->TypeParameter.next; yyOld->TypeParameter.next = yyNew; break;
case kBreezeParameter: yyNext = yyOld->BreezeParameter.next; yyOld->BreezeParameter.next = yyNew; break;
case kBreezeExprParameter: yyNext = yyOld->BreezeExprParameter.next; yyOld->BreezeExprParameter.next = yyNew; break;
case kBreezeTypeParameter: yyNext = yyOld->BreezeTypeParameter.next; yyOld->BreezeTypeParameter.next = yyNew; break;
case kProcParam: yyNext = yyOld->ProcParam.next; yyOld->ProcParam.next = yyNew; break;
case kExprProcParam: yyNext = yyOld->ExprProcParam.next; yyOld->ExprProcParam.next = yyNew; break;
case kTypeProcParam: yyNext = yyOld->TypeProcParam.next; yyOld->TypeProcParam.next = yyNew; break;
case kVarReadProcParam: yyNext = yyOld->VarReadProcParam.next; yyOld->VarReadProcParam.next = yyNew; break;
case kVarWriteProcParam: yyNext = yyOld->VarWriteProcParam.next; yyOld->VarWriteProcParam.next = yyNew; break;
case kBlockProcParam: yyNext = yyOld->BlockProcParam.next; yyOld->BlockProcParam.next = yyNew; break;
case kFuncParam: yyNext = yyOld->FuncParam.next; yyOld->FuncParam.next = yyNew; break;
case kExprFuncParam: yyNext = yyOld->ExprFuncParam.next; yyOld->ExprFuncParam.next = yyNew; break;
case kTypeFuncParam: yyNext = yyOld->TypeFuncParam.next; yyOld->TypeFuncParam.next = yyNew; break;
case kGuard: yyNext = yyOld->Guard.next; yyOld->Guard.next = yyNew; break;
case kPortGuard: yyNext = yyOld->PortGuard.next; yyOld->PortGuard.next = yyNew; break;
case kDeclGuard: yyNext = yyOld->DeclGuard.next; yyOld->DeclGuard.next = yyNew; break;
case kCaseGuard: yyNext = yyOld->CaseGuard.next; yyOld->CaseGuard.next = yyNew; break;
case kCaseMatchGuard: yyNext = yyOld->CaseMatchGuard.next; yyOld->CaseMatchGuard.next = yyNew; break;
case kForCaseGuard: yyNext = yyOld->ForCaseGuard.next; yyOld->ForCaseGuard.next = yyNew; break;
case kChannelGuard: yyNext = yyOld->ChannelGuard.next; yyOld->ChannelGuard.next = yyNew; break;
case kIdent: yyNext = yyOld->Ident.next; yyOld->Ident.next = yyNew; break;
case kExprList: yyNext = yyOld->ExprList.next; yyOld->ExprList.next = yyNew; break;
case kLvalueList: yyNext = yyOld->LvalueList.next; yyOld->LvalueList.next = yyNew; break;
case kLinkedBlock: yyNext = yyOld->LinkedBlock.next; yyOld->LinkedBlock.next = yyNew; break;
case kLinkedChannel: yyNext = yyOld->LinkedChannel.next; yyOld->LinkedChannel.next = yyNew; break;
case kCaseMatch: yyNext = yyOld->CaseMatch.next; yyOld->CaseMatch.next = yyNew; break;
case kCaseRange: yyNext = yyOld->CaseRange.next; yyOld->CaseRange.next = yyNew; break;
case kCaseImplicant: yyNext = yyOld->CaseImplicant.next; yyOld->CaseImplicant.next = yyNew; break;
case kEnumElem: yyNext = yyOld->EnumElem.next; yyOld->EnumElem.next = yyNew; break;
case kDefaultValuedEnumElem: yyNext = yyOld->DefaultValuedEnumElem.next; yyOld->DefaultValuedEnumElem.next = yyNew; break;
case kValuedEnumElem: yyNext = yyOld->ValuedEnumElem.next; yyOld->ValuedEnumElem.next = yyNew; break;
case kRecordElem: yyNext = yyOld->RecordElem.next; yyOld->RecordElem.next = yyNew; break;
case kValDecl: yyNext = yyOld->ValDecl.next; yyOld->ValDecl.next = yyNew; break;
  default: goto yyExit;
  }
  yyNew = yyOld;
  yyOld = yyNext;
 }
yyExit:
 switch (yyTail->Kind) {
case kDecl: yyTail->Decl.next = yyOld; break;
case kImportDecl: yyTail->ImportDecl.next = yyOld; break;
case kFileDecl: yyTail->FileDecl.next = yyOld; break;
case kTypeDecl: yyTail->TypeDecl.next = yyOld; break;
case kConstantDecl: yyTail->ConstantDecl.next = yyOld; break;
case kVariableDecl: yyTail->VariableDecl.next = yyOld; break;
case kInitVariableDecl: yyTail->InitVariableDecl.next = yyOld; break;
case kChannelDecl: yyTail->ChannelDecl.next = yyOld; break;
case kChannelArrayDecl: yyTail->ChannelArrayDecl.next = yyOld; break;
case kSyncDecl: yyTail->SyncDecl.next = yyOld; break;
case kSyncArrayDecl: yyTail->SyncArrayDecl.next = yyOld; break;
case kProcedureDecl: yyTail->ProcedureDecl.next = yyOld; break;
case kProcAliasDecl: yyTail->ProcAliasDecl.next = yyOld; break;
case kFunctionDecl: yyTail->FunctionDecl.next = yyOld; break;
case kBuiltinFunctionDecl: yyTail->BuiltinFunctionDecl.next = yyOld; break;
case kSharedDecl: yyTail->SharedDecl.next = yyOld; break;
case kPartDecl: yyTail->PartDecl.next = yyOld; break;
case kIfDecls: yyTail->IfDecls.next = yyOld; break;
case kIfElseDecls: yyTail->IfElseDecls.next = yyOld; break;
case kPrintDecl: yyTail->PrintDecl.next = yyOld; break;
case kNullDecl: yyTail->NullDecl.next = yyOld; break;
case kFormalPort: yyTail->FormalPort.next = yyOld; break;
case kValuePort: yyTail->ValuePort.next = yyOld; break;
case kParamPort: yyTail->ParamPort.next = yyOld; break;
case kTypeParamPort: yyTail->TypeParamPort.next = yyOld; break;
case kChannelPort: yyTail->ChannelPort.next = yyOld; break;
case kChannelPortArray: yyTail->ChannelPortArray.next = yyOld; break;
case kSyncPort: yyTail->SyncPort.next = yyOld; break;
case kSyncPortArray: yyTail->SyncPortArray.next = yyOld; break;
case kIfPorts: yyTail->IfPorts.next = yyOld; break;
case kIfElsePorts: yyTail->IfElsePorts.next = yyOld; break;
case kParameter: yyTail->Parameter.next = yyOld; break;
case kNumberParameter: yyTail->NumberParameter.next = yyOld; break;
case kStringParameter: yyTail->StringParameter.next = yyOld; break;
case kTypeParameter: yyTail->TypeParameter.next = yyOld; break;
case kBreezeParameter: yyTail->BreezeParameter.next = yyOld; break;
case kBreezeExprParameter: yyTail->BreezeExprParameter.next = yyOld; break;
case kBreezeTypeParameter: yyTail->BreezeTypeParameter.next = yyOld; break;
case kProcParam: yyTail->ProcParam.next = yyOld; break;
case kExprProcParam: yyTail->ExprProcParam.next = yyOld; break;
case kTypeProcParam: yyTail->TypeProcParam.next = yyOld; break;
case kVarReadProcParam: yyTail->VarReadProcParam.next = yyOld; break;
case kVarWriteProcParam: yyTail->VarWriteProcParam.next = yyOld; break;
case kBlockProcParam: yyTail->BlockProcParam.next = yyOld; break;
case kFuncParam: yyTail->FuncParam.next = yyOld; break;
case kExprFuncParam: yyTail->ExprFuncParam.next = yyOld; break;
case kTypeFuncParam: yyTail->TypeFuncParam.next = yyOld; break;
case kGuard: yyTail->Guard.next = yyOld; break;
case kPortGuard: yyTail->PortGuard.next = yyOld; break;
case kDeclGuard: yyTail->DeclGuard.next = yyOld; break;
case kCaseGuard: yyTail->CaseGuard.next = yyOld; break;
case kCaseMatchGuard: yyTail->CaseMatchGuard.next = yyOld; break;
case kForCaseGuard: yyTail->ForCaseGuard.next = yyOld; break;
case kChannelGuard: yyTail->ChannelGuard.next = yyOld; break;
case kIdent: yyTail->Ident.next = yyOld; break;
case kExprList: yyTail->ExprList.next = yyOld; break;
case kLvalueList: yyTail->LvalueList.next = yyOld; break;
case kLinkedBlock: yyTail->LinkedBlock.next = yyOld; break;
case kLinkedChannel: yyTail->LinkedChannel.next = yyOld; break;
case kCaseMatch: yyTail->CaseMatch.next = yyOld; break;
case kCaseRange: yyTail->CaseRange.next = yyOld; break;
case kCaseImplicant: yyTail->CaseImplicant.next = yyOld; break;
case kEnumElem: yyTail->EnumElem.next = yyOld; break;
case kDefaultValuedEnumElem: yyTail->DefaultValuedEnumElem.next = yyOld; break;
case kValuedEnumElem: yyTail->ValuedEnumElem.next = yyOld; break;
case kRecordElem: yyTail->RecordElem.next = yyOld; break;
case kValDecl: yyTail->ValDecl.next = yyOld; break;
 default: ;
 }
 return yyNew;
}

void ForallTree
# if defined __STDC__ | defined __cplusplus
 (tTree yyt, Tree_tProcTree yyProc)
# else
 (yyt, yyProc) tTree yyt; Tree_tProcTree yyProc;
# endif
{
 for (;;) {
  tTree yyyt;
  if ((yyyt = yyt) == NoTree) return;
  switch (yyt->Kind) {
case kDecl: yyt = yyt->Decl.next; break;
case kImportDecl: yyt = yyt->ImportDecl.next; break;
case kFileDecl: yyt = yyt->FileDecl.next; break;
case kTypeDecl: yyt = yyt->TypeDecl.next; break;
case kConstantDecl: yyt = yyt->ConstantDecl.next; break;
case kVariableDecl: yyt = yyt->VariableDecl.next; break;
case kInitVariableDecl: yyt = yyt->InitVariableDecl.next; break;
case kChannelDecl: yyt = yyt->ChannelDecl.next; break;
case kChannelArrayDecl: yyt = yyt->ChannelArrayDecl.next; break;
case kSyncDecl: yyt = yyt->SyncDecl.next; break;
case kSyncArrayDecl: yyt = yyt->SyncArrayDecl.next; break;
case kProcedureDecl: yyt = yyt->ProcedureDecl.next; break;
case kProcAliasDecl: yyt = yyt->ProcAliasDecl.next; break;
case kFunctionDecl: yyt = yyt->FunctionDecl.next; break;
case kBuiltinFunctionDecl: yyt = yyt->BuiltinFunctionDecl.next; break;
case kSharedDecl: yyt = yyt->SharedDecl.next; break;
case kPartDecl: yyt = yyt->PartDecl.next; break;
case kIfDecls: yyt = yyt->IfDecls.next; break;
case kIfElseDecls: yyt = yyt->IfElseDecls.next; break;
case kPrintDecl: yyt = yyt->PrintDecl.next; break;
case kNullDecl: yyt = yyt->NullDecl.next; break;
case kFormalPort: yyt = yyt->FormalPort.next; break;
case kValuePort: yyt = yyt->ValuePort.next; break;
case kParamPort: yyt = yyt->ParamPort.next; break;
case kTypeParamPort: yyt = yyt->TypeParamPort.next; break;
case kChannelPort: yyt = yyt->ChannelPort.next; break;
case kChannelPortArray: yyt = yyt->ChannelPortArray.next; break;
case kSyncPort: yyt = yyt->SyncPort.next; break;
case kSyncPortArray: yyt = yyt->SyncPortArray.next; break;
case kIfPorts: yyt = yyt->IfPorts.next; break;
case kIfElsePorts: yyt = yyt->IfElsePorts.next; break;
case kParameter: yyt = yyt->Parameter.next; break;
case kNumberParameter: yyt = yyt->NumberParameter.next; break;
case kStringParameter: yyt = yyt->StringParameter.next; break;
case kTypeParameter: yyt = yyt->TypeParameter.next; break;
case kBreezeParameter: yyt = yyt->BreezeParameter.next; break;
case kBreezeExprParameter: yyt = yyt->BreezeExprParameter.next; break;
case kBreezeTypeParameter: yyt = yyt->BreezeTypeParameter.next; break;
case kProcParam: yyt = yyt->ProcParam.next; break;
case kExprProcParam: yyt = yyt->ExprProcParam.next; break;
case kTypeProcParam: yyt = yyt->TypeProcParam.next; break;
case kVarReadProcParam: yyt = yyt->VarReadProcParam.next; break;
case kVarWriteProcParam: yyt = yyt->VarWriteProcParam.next; break;
case kBlockProcParam: yyt = yyt->BlockProcParam.next; break;
case kFuncParam: yyt = yyt->FuncParam.next; break;
case kExprFuncParam: yyt = yyt->ExprFuncParam.next; break;
case kTypeFuncParam: yyt = yyt->TypeFuncParam.next; break;
case kGuard: yyt = yyt->Guard.next; break;
case kPortGuard: yyt = yyt->PortGuard.next; break;
case kDeclGuard: yyt = yyt->DeclGuard.next; break;
case kCaseGuard: yyt = yyt->CaseGuard.next; break;
case kCaseMatchGuard: yyt = yyt->CaseMatchGuard.next; break;
case kForCaseGuard: yyt = yyt->ForCaseGuard.next; break;
case kChannelGuard: yyt = yyt->ChannelGuard.next; break;
case kIdent: yyt = yyt->Ident.next; break;
case kExprList: yyt = yyt->ExprList.next; break;
case kLvalueList: yyt = yyt->LvalueList.next; break;
case kLinkedBlock: yyt = yyt->LinkedBlock.next; break;
case kLinkedChannel: yyt = yyt->LinkedChannel.next; break;
case kCaseMatch: yyt = yyt->CaseMatch.next; break;
case kCaseRange: yyt = yyt->CaseRange.next; break;
case kCaseImplicant: yyt = yyt->CaseImplicant.next; break;
case kEnumElem: yyt = yyt->EnumElem.next; break;
case kDefaultValuedEnumElem: yyt = yyt->DefaultValuedEnumElem.next; break;
case kValuedEnumElem: yyt = yyt->ValuedEnumElem.next; break;
case kRecordElem: yyt = yyt->RecordElem.next; break;
case kValDecl: yyt = yyt->ValDecl.next; break;
  default: return;
  }
  yyProc (yyyt);
 }
}

# define yyInitOldToNewStoreSize 32

typedef struct { tTree yyOld, yyNew; } yytOldToNew;
static unsigned long yyOldToNewStoreSize = yyInitOldToNewStoreSize;
static yytOldToNew yyOldToNewStore [yyInitOldToNewStoreSize];
static yytOldToNew * yyOldToNewStorePtr = yyOldToNewStore;
static int yyOldToNewCount;

static void yyStoreOldToNew
# if defined __STDC__ | defined __cplusplus
 (tTree yyOld, tTree yyNew)
# else
 (yyOld, yyNew) tTree yyOld, yyNew;
# endif
{
 if (++ yyOldToNewCount == yyOldToNewStoreSize)
  ExtendArray ((char * *) & yyOldToNewStorePtr, & yyOldToNewStoreSize, sizeof (yytOldToNew));
 yyOldToNewStorePtr [yyOldToNewCount].yyOld = yyOld;
 yyOldToNewStorePtr [yyOldToNewCount].yyNew = yyNew;
}

static tTree yyMapOldToNew
# if defined __STDC__ | defined __cplusplus
 (tTree yyOld)
# else
 (yyOld) tTree yyOld;
# endif
{
 register int yyi;
 for (yyi = 1; yyi <= yyOldToNewCount; yyi ++)
  if (yyOldToNewStorePtr [yyi].yyOld == yyOld) return yyOldToNewStorePtr [yyi].yyNew;
 return NoTree;
}

static void yyCopyTree
# if defined __STDC__ | defined __cplusplus
 (tTree yyt, yyPtrtTree yyNew)
# else
 (yyt, yyNew) tTree yyt; yyPtrtTree yyNew;
# endif
{
 for (;;) {
  if (yyt == NoTree) { * yyNew = NoTree; return; }
  if (yyt->yyHead.yyMark == 0) { * yyNew = yyMapOldToNew (yyt); return; }
  yyALLOC (* yyNew, Tree_NodeSize [yyt->Kind])
  if (yyt->yyHead.yyMark > 1) { yyStoreOldToNew (yyt, * yyNew); }
  yyt->yyHead.yyMark = 0;

  switch (yyt->Kind) {
case kBalsaProgram: (* yyNew)->BalsaProgram = yyt->BalsaProgram;
copyPtrContext ((* yyNew)->BalsaProgram.context, yyt->BalsaProgram.context)
yyt = yyt->BalsaProgram.Decls;
yyNew = & (* yyNew)->BalsaProgram.Decls; break;
case kDecls: (* yyNew)->Decls = yyt->Decls;
copytPosition ((* yyNew)->Decls.position, yyt->Decls.position)
copyScope ((* yyNew)->Decls.scope, yyt->Decls.scope)
copyPtrContext ((* yyNew)->Decls.contextIn, yyt->Decls.contextIn)
copyPtrContext ((* yyNew)->Decls.contextOut, yyt->Decls.contextOut)
copybool ((* yyNew)->Decls.skip, yyt->Decls.skip)
return;
case kNullDecls: (* yyNew)->NullDecls = yyt->NullDecls;
copytPosition ((* yyNew)->NullDecls.position, yyt->NullDecls.position)
copyScope ((* yyNew)->NullDecls.scope, yyt->NullDecls.scope)
copyPtrContext ((* yyNew)->NullDecls.contextIn, yyt->NullDecls.contextIn)
copyPtrContext ((* yyNew)->NullDecls.contextOut, yyt->NullDecls.contextOut)
copybool ((* yyNew)->NullDecls.skip, yyt->NullDecls.skip)
return;
case kDecl: (* yyNew)->Decl = yyt->Decl;
copytPosition ((* yyNew)->Decl.position, yyt->Decl.position)
copyScope ((* yyNew)->Decl.scope, yyt->Decl.scope)
copyPtrContext ((* yyNew)->Decl.contextIn, yyt->Decl.contextIn)
copyPtrContext ((* yyNew)->Decl.contextOut, yyt->Decl.contextOut)
copybool ((* yyNew)->Decl.skip, yyt->Decl.skip)
yyt = yyt->Decl.next;
yyNew = & (* yyNew)->Decl.next; break;
case kImportDecl: (* yyNew)->ImportDecl = yyt->ImportDecl;
copytPosition ((* yyNew)->ImportDecl.position, yyt->ImportDecl.position)
copyScope ((* yyNew)->ImportDecl.scope, yyt->ImportDecl.scope)
copyPtrContext ((* yyNew)->ImportDecl.contextIn, yyt->ImportDecl.contextIn)
copyPtrContext ((* yyNew)->ImportDecl.contextOut, yyt->ImportDecl.contextOut)
copybool ((* yyNew)->ImportDecl.skip, yyt->ImportDecl.skip)
copytTree ((* yyNew)->ImportDecl.Idents, yyt->ImportDecl.Idents)
yyt = yyt->ImportDecl.next;
yyNew = & (* yyNew)->ImportDecl.next; break;
case kFileDecl: (* yyNew)->FileDecl = yyt->FileDecl;
copytPosition ((* yyNew)->FileDecl.position, yyt->FileDecl.position)
copyScope ((* yyNew)->FileDecl.scope, yyt->FileDecl.scope)
copyPtrContext ((* yyNew)->FileDecl.contextIn, yyt->FileDecl.contextIn)
copyPtrContext ((* yyNew)->FileDecl.contextOut, yyt->FileDecl.contextOut)
copybool ((* yyNew)->FileDecl.skip, yyt->FileDecl.skip)
copytTree ((* yyNew)->FileDecl.Idents, yyt->FileDecl.Idents)
yyt = yyt->FileDecl.next;
yyNew = & (* yyNew)->FileDecl.next; break;
case kTypeDecl: (* yyNew)->TypeDecl = yyt->TypeDecl;
copytPosition ((* yyNew)->TypeDecl.position, yyt->TypeDecl.position)
copyScope ((* yyNew)->TypeDecl.scope, yyt->TypeDecl.scope)
copyPtrContext ((* yyNew)->TypeDecl.contextIn, yyt->TypeDecl.contextIn)
copyPtrContext ((* yyNew)->TypeDecl.contextOut, yyt->TypeDecl.contextOut)
copybool ((* yyNew)->TypeDecl.skip, yyt->TypeDecl.skip)
copytIdent ((* yyNew)->TypeDecl.ident, yyt->TypeDecl.ident)
copytTree ((* yyNew)->TypeDecl.AType, yyt->TypeDecl.AType)
yyt = yyt->TypeDecl.next;
yyNew = & (* yyNew)->TypeDecl.next; break;
case kConstantDecl: (* yyNew)->ConstantDecl = yyt->ConstantDecl;
copytPosition ((* yyNew)->ConstantDecl.position, yyt->ConstantDecl.position)
copyScope ((* yyNew)->ConstantDecl.scope, yyt->ConstantDecl.scope)
copyPtrContext ((* yyNew)->ConstantDecl.contextIn, yyt->ConstantDecl.contextIn)
copyPtrContext ((* yyNew)->ConstantDecl.contextOut, yyt->ConstantDecl.contextOut)
copybool ((* yyNew)->ConstantDecl.skip, yyt->ConstantDecl.skip)
copytIdent ((* yyNew)->ConstantDecl.ident, yyt->ConstantDecl.ident)
copytTree ((* yyNew)->ConstantDecl.CoercedExpr, yyt->ConstantDecl.CoercedExpr)
copytTree ((* yyNew)->ConstantDecl.AType, yyt->ConstantDecl.AType)
yyt = yyt->ConstantDecl.next;
yyNew = & (* yyNew)->ConstantDecl.next; break;
case kVariableDecl: (* yyNew)->VariableDecl = yyt->VariableDecl;
copytPosition ((* yyNew)->VariableDecl.position, yyt->VariableDecl.position)
copyScope ((* yyNew)->VariableDecl.scope, yyt->VariableDecl.scope)
copyPtrContext ((* yyNew)->VariableDecl.contextIn, yyt->VariableDecl.contextIn)
copyPtrContext ((* yyNew)->VariableDecl.contextOut, yyt->VariableDecl.contextOut)
copybool ((* yyNew)->VariableDecl.skip, yyt->VariableDecl.skip)
copytTree ((* yyNew)->VariableDecl.Idents, yyt->VariableDecl.Idents)
copytTree ((* yyNew)->VariableDecl.AType, yyt->VariableDecl.AType)
yyt = yyt->VariableDecl.next;
yyNew = & (* yyNew)->VariableDecl.next; break;
case kInitVariableDecl: (* yyNew)->InitVariableDecl = yyt->InitVariableDecl;
copytPosition ((* yyNew)->InitVariableDecl.position, yyt->InitVariableDecl.position)
copyScope ((* yyNew)->InitVariableDecl.scope, yyt->InitVariableDecl.scope)
copyPtrContext ((* yyNew)->InitVariableDecl.contextIn, yyt->InitVariableDecl.contextIn)
copyPtrContext ((* yyNew)->InitVariableDecl.contextOut, yyt->InitVariableDecl.contextOut)
copybool ((* yyNew)->InitVariableDecl.skip, yyt->InitVariableDecl.skip)
copytTree ((* yyNew)->InitVariableDecl.Idents, yyt->InitVariableDecl.Idents)
copytTree ((* yyNew)->InitVariableDecl.CoercedExpr, yyt->InitVariableDecl.CoercedExpr)
copytTree ((* yyNew)->InitVariableDecl.AType, yyt->InitVariableDecl.AType)
yyt = yyt->InitVariableDecl.next;
yyNew = & (* yyNew)->InitVariableDecl.next; break;
case kChannelDecl: (* yyNew)->ChannelDecl = yyt->ChannelDecl;
copytPosition ((* yyNew)->ChannelDecl.position, yyt->ChannelDecl.position)
copyScope ((* yyNew)->ChannelDecl.scope, yyt->ChannelDecl.scope)
copyPtrContext ((* yyNew)->ChannelDecl.contextIn, yyt->ChannelDecl.contextIn)
copyPtrContext ((* yyNew)->ChannelDecl.contextOut, yyt->ChannelDecl.contextOut)
copybool ((* yyNew)->ChannelDecl.skip, yyt->ChannelDecl.skip)
copytTree ((* yyNew)->ChannelDecl.Idents, yyt->ChannelDecl.Idents)
copytTree ((* yyNew)->ChannelDecl.AType, yyt->ChannelDecl.AType)
copybool ((* yyNew)->ChannelDecl.multicast, yyt->ChannelDecl.multicast)
yyt = yyt->ChannelDecl.next;
yyNew = & (* yyNew)->ChannelDecl.next; break;
case kChannelArrayDecl: (* yyNew)->ChannelArrayDecl = yyt->ChannelArrayDecl;
copytPosition ((* yyNew)->ChannelArrayDecl.position, yyt->ChannelArrayDecl.position)
copyScope ((* yyNew)->ChannelArrayDecl.scope, yyt->ChannelArrayDecl.scope)
copyPtrContext ((* yyNew)->ChannelArrayDecl.contextIn, yyt->ChannelArrayDecl.contextIn)
copyPtrContext ((* yyNew)->ChannelArrayDecl.contextOut, yyt->ChannelArrayDecl.contextOut)
copybool ((* yyNew)->ChannelArrayDecl.skip, yyt->ChannelArrayDecl.skip)
copytTree ((* yyNew)->ChannelArrayDecl.Idents, yyt->ChannelArrayDecl.Idents)
copytTree ((* yyNew)->ChannelArrayDecl.AType, yyt->ChannelArrayDecl.AType)
copytTree ((* yyNew)->ChannelArrayDecl.Range, yyt->ChannelArrayDecl.Range)
copybool ((* yyNew)->ChannelArrayDecl.multicast, yyt->ChannelArrayDecl.multicast)
yyt = yyt->ChannelArrayDecl.next;
yyNew = & (* yyNew)->ChannelArrayDecl.next; break;
case kSyncDecl: (* yyNew)->SyncDecl = yyt->SyncDecl;
copytPosition ((* yyNew)->SyncDecl.position, yyt->SyncDecl.position)
copyScope ((* yyNew)->SyncDecl.scope, yyt->SyncDecl.scope)
copyPtrContext ((* yyNew)->SyncDecl.contextIn, yyt->SyncDecl.contextIn)
copyPtrContext ((* yyNew)->SyncDecl.contextOut, yyt->SyncDecl.contextOut)
copybool ((* yyNew)->SyncDecl.skip, yyt->SyncDecl.skip)
copytTree ((* yyNew)->SyncDecl.Idents, yyt->SyncDecl.Idents)
copybool ((* yyNew)->SyncDecl.multicast, yyt->SyncDecl.multicast)
yyt = yyt->SyncDecl.next;
yyNew = & (* yyNew)->SyncDecl.next; break;
case kSyncArrayDecl: (* yyNew)->SyncArrayDecl = yyt->SyncArrayDecl;
copytPosition ((* yyNew)->SyncArrayDecl.position, yyt->SyncArrayDecl.position)
copyScope ((* yyNew)->SyncArrayDecl.scope, yyt->SyncArrayDecl.scope)
copyPtrContext ((* yyNew)->SyncArrayDecl.contextIn, yyt->SyncArrayDecl.contextIn)
copyPtrContext ((* yyNew)->SyncArrayDecl.contextOut, yyt->SyncArrayDecl.contextOut)
copybool ((* yyNew)->SyncArrayDecl.skip, yyt->SyncArrayDecl.skip)
copytTree ((* yyNew)->SyncArrayDecl.Idents, yyt->SyncArrayDecl.Idents)
copytTree ((* yyNew)->SyncArrayDecl.Range, yyt->SyncArrayDecl.Range)
copybool ((* yyNew)->SyncArrayDecl.multicast, yyt->SyncArrayDecl.multicast)
yyt = yyt->SyncArrayDecl.next;
yyNew = & (* yyNew)->SyncArrayDecl.next; break;
case kProcedureDecl: (* yyNew)->ProcedureDecl = yyt->ProcedureDecl;
copytPosition ((* yyNew)->ProcedureDecl.position, yyt->ProcedureDecl.position)
copyScope ((* yyNew)->ProcedureDecl.scope, yyt->ProcedureDecl.scope)
copyPtrContext ((* yyNew)->ProcedureDecl.contextIn, yyt->ProcedureDecl.contextIn)
copyPtrContext ((* yyNew)->ProcedureDecl.contextOut, yyt->ProcedureDecl.contextOut)
copybool ((* yyNew)->ProcedureDecl.skip, yyt->ProcedureDecl.skip)
copytIdent ((* yyNew)->ProcedureDecl.ident, yyt->ProcedureDecl.ident)
copytTree ((* yyNew)->ProcedureDecl.FormalPorts, yyt->ProcedureDecl.FormalPorts)
copytTree ((* yyNew)->ProcedureDecl.Block, yyt->ProcedureDecl.Block)
copyPtrProcedure ((* yyNew)->ProcedureDecl.procedure, yyt->ProcedureDecl.procedure)
yyt = yyt->ProcedureDecl.next;
yyNew = & (* yyNew)->ProcedureDecl.next; break;
case kProcAliasDecl: (* yyNew)->ProcAliasDecl = yyt->ProcAliasDecl;
copytPosition ((* yyNew)->ProcAliasDecl.position, yyt->ProcAliasDecl.position)
copyScope ((* yyNew)->ProcAliasDecl.scope, yyt->ProcAliasDecl.scope)
copyPtrContext ((* yyNew)->ProcAliasDecl.contextIn, yyt->ProcAliasDecl.contextIn)
copyPtrContext ((* yyNew)->ProcAliasDecl.contextOut, yyt->ProcAliasDecl.contextOut)
copybool ((* yyNew)->ProcAliasDecl.skip, yyt->ProcAliasDecl.skip)
copytIdent ((* yyNew)->ProcAliasDecl.newName, yyt->ProcAliasDecl.newName)
copytIdent ((* yyNew)->ProcAliasDecl.oldName, yyt->ProcAliasDecl.oldName)
copytTree ((* yyNew)->ProcAliasDecl.ProcedureParams, yyt->ProcAliasDecl.ProcedureParams)
yyt = yyt->ProcAliasDecl.next;
yyNew = & (* yyNew)->ProcAliasDecl.next; break;
case kFunctionDecl: (* yyNew)->FunctionDecl = yyt->FunctionDecl;
copytPosition ((* yyNew)->FunctionDecl.position, yyt->FunctionDecl.position)
copyScope ((* yyNew)->FunctionDecl.scope, yyt->FunctionDecl.scope)
copyPtrContext ((* yyNew)->FunctionDecl.contextIn, yyt->FunctionDecl.contextIn)
copyPtrContext ((* yyNew)->FunctionDecl.contextOut, yyt->FunctionDecl.contextOut)
copybool ((* yyNew)->FunctionDecl.skip, yyt->FunctionDecl.skip)
copytIdent ((* yyNew)->FunctionDecl.ident, yyt->FunctionDecl.ident)
copytTree ((* yyNew)->FunctionDecl.FormalPorts, yyt->FunctionDecl.FormalPorts)
copytTree ((* yyNew)->FunctionDecl.CoercedExpr, yyt->FunctionDecl.CoercedExpr)
copytTree ((* yyNew)->FunctionDecl.AType, yyt->FunctionDecl.AType)
copyPtrProcedure ((* yyNew)->FunctionDecl.function, yyt->FunctionDecl.function)
yyt = yyt->FunctionDecl.next;
yyNew = & (* yyNew)->FunctionDecl.next; break;
case kBuiltinFunctionDecl: (* yyNew)->BuiltinFunctionDecl = yyt->BuiltinFunctionDecl;
copytPosition ((* yyNew)->BuiltinFunctionDecl.position, yyt->BuiltinFunctionDecl.position)
copyScope ((* yyNew)->BuiltinFunctionDecl.scope, yyt->BuiltinFunctionDecl.scope)
copyPtrContext ((* yyNew)->BuiltinFunctionDecl.contextIn, yyt->BuiltinFunctionDecl.contextIn)
copyPtrContext ((* yyNew)->BuiltinFunctionDecl.contextOut, yyt->BuiltinFunctionDecl.contextOut)
copybool ((* yyNew)->BuiltinFunctionDecl.skip, yyt->BuiltinFunctionDecl.skip)
copytIdent ((* yyNew)->BuiltinFunctionDecl.ident, yyt->BuiltinFunctionDecl.ident)
copytTree ((* yyNew)->BuiltinFunctionDecl.FormalPorts, yyt->BuiltinFunctionDecl.FormalPorts)
copytTree ((* yyNew)->BuiltinFunctionDecl.AType, yyt->BuiltinFunctionDecl.AType)
copyPtrProcedure ((* yyNew)->BuiltinFunctionDecl.function, yyt->BuiltinFunctionDecl.function)
yyt = yyt->BuiltinFunctionDecl.next;
yyNew = & (* yyNew)->BuiltinFunctionDecl.next; break;
case kSharedDecl: (* yyNew)->SharedDecl = yyt->SharedDecl;
copytPosition ((* yyNew)->SharedDecl.position, yyt->SharedDecl.position)
copyScope ((* yyNew)->SharedDecl.scope, yyt->SharedDecl.scope)
copyPtrContext ((* yyNew)->SharedDecl.contextIn, yyt->SharedDecl.contextIn)
copyPtrContext ((* yyNew)->SharedDecl.contextOut, yyt->SharedDecl.contextOut)
copybool ((* yyNew)->SharedDecl.skip, yyt->SharedDecl.skip)
copytIdent ((* yyNew)->SharedDecl.ident, yyt->SharedDecl.ident)
copytTree ((* yyNew)->SharedDecl.Block, yyt->SharedDecl.Block)
copyPtrProcedure ((* yyNew)->SharedDecl.procedure, yyt->SharedDecl.procedure)
yyt = yyt->SharedDecl.next;
yyNew = & (* yyNew)->SharedDecl.next; break;
case kPartDecl: (* yyNew)->PartDecl = yyt->PartDecl;
copytPosition ((* yyNew)->PartDecl.position, yyt->PartDecl.position)
copyScope ((* yyNew)->PartDecl.scope, yyt->PartDecl.scope)
copyPtrContext ((* yyNew)->PartDecl.contextIn, yyt->PartDecl.contextIn)
copyPtrContext ((* yyNew)->PartDecl.contextOut, yyt->PartDecl.contextOut)
copybool ((* yyNew)->PartDecl.skip, yyt->PartDecl.skip)
copytIdent ((* yyNew)->PartDecl.ident, yyt->PartDecl.ident)
copyPtrLispList ((* yyNew)->PartDecl.attributes, yyt->PartDecl.attributes)
copytTree ((* yyNew)->PartDecl.FormalPorts, yyt->PartDecl.FormalPorts)
copyPtrWireArray ((* yyNew)->PartDecl.channels, yyt->PartDecl.channels)
copyunsigned ((* yyNew)->PartDecl.channelCount, yyt->PartDecl.channelCount)
copytTree ((* yyNew)->PartDecl.Comps, yyt->PartDecl.Comps)
copyPtrCallContextList ((* yyNew)->PartDecl.callcontexts, yyt->PartDecl.callcontexts)
copyPtrProcedure ((* yyNew)->PartDecl.procedure, yyt->PartDecl.procedure)
yyt = yyt->PartDecl.next;
yyNew = & (* yyNew)->PartDecl.next; break;
case kIfDecls: (* yyNew)->IfDecls = yyt->IfDecls;
copytPosition ((* yyNew)->IfDecls.position, yyt->IfDecls.position)
copyScope ((* yyNew)->IfDecls.scope, yyt->IfDecls.scope)
copyPtrContext ((* yyNew)->IfDecls.contextIn, yyt->IfDecls.contextIn)
copyPtrContext ((* yyNew)->IfDecls.contextOut, yyt->IfDecls.contextOut)
copybool ((* yyNew)->IfDecls.skip, yyt->IfDecls.skip)
copytTree ((* yyNew)->IfDecls.DeclGuards, yyt->IfDecls.DeclGuards)
yyt = yyt->IfDecls.next;
yyNew = & (* yyNew)->IfDecls.next; break;
case kIfElseDecls: (* yyNew)->IfElseDecls = yyt->IfElseDecls;
copytPosition ((* yyNew)->IfElseDecls.position, yyt->IfElseDecls.position)
copyScope ((* yyNew)->IfElseDecls.scope, yyt->IfElseDecls.scope)
copyPtrContext ((* yyNew)->IfElseDecls.contextIn, yyt->IfElseDecls.contextIn)
copyPtrContext ((* yyNew)->IfElseDecls.contextOut, yyt->IfElseDecls.contextOut)
copybool ((* yyNew)->IfElseDecls.skip, yyt->IfElseDecls.skip)
copytTree ((* yyNew)->IfElseDecls.DeclGuards, yyt->IfElseDecls.DeclGuards)
copytTree ((* yyNew)->IfElseDecls.Decls, yyt->IfElseDecls.Decls)
yyt = yyt->IfElseDecls.next;
yyNew = & (* yyNew)->IfElseDecls.next; break;
case kPrintDecl: (* yyNew)->PrintDecl = yyt->PrintDecl;
copytPosition ((* yyNew)->PrintDecl.position, yyt->PrintDecl.position)
copyScope ((* yyNew)->PrintDecl.scope, yyt->PrintDecl.scope)
copyPtrContext ((* yyNew)->PrintDecl.contextIn, yyt->PrintDecl.contextIn)
copyPtrContext ((* yyNew)->PrintDecl.contextOut, yyt->PrintDecl.contextOut)
copybool ((* yyNew)->PrintDecl.skip, yyt->PrintDecl.skip)
copytTree ((* yyNew)->PrintDecl.Expr, yyt->PrintDecl.Expr)
copytTree ((* yyNew)->PrintDecl.ExprLists, yyt->PrintDecl.ExprLists)
yyt = yyt->PrintDecl.next;
yyNew = & (* yyNew)->PrintDecl.next; break;
case kNullDecl: (* yyNew)->NullDecl = yyt->NullDecl;
copytPosition ((* yyNew)->NullDecl.position, yyt->NullDecl.position)
copyScope ((* yyNew)->NullDecl.scope, yyt->NullDecl.scope)
copyPtrContext ((* yyNew)->NullDecl.contextIn, yyt->NullDecl.contextIn)
copyPtrContext ((* yyNew)->NullDecl.contextOut, yyt->NullDecl.contextOut)
copybool ((* yyNew)->NullDecl.skip, yyt->NullDecl.skip)
yyt = yyt->NullDecl.next;
yyNew = & (* yyNew)->NullDecl.next; break;
case kRange: (* yyNew)->Range = yyt->Range;
copytPosition ((* yyNew)->Range.position, yyt->Range.position)
copyPtrContext ((* yyNew)->Range.context, yyt->Range.context)
copybool ((* yyNew)->Range.skip, yyt->Range.skip)
copySpan ((* yyNew)->Range.range, yyt->Range.range)
copyPtrType ((* yyNew)->Range.expectedType, yyt->Range.expectedType)
copybool ((* yyNew)->Range.isSpan, yyt->Range.isSpan)
return;
case kSpecifiedRange: (* yyNew)->SpecifiedRange = yyt->SpecifiedRange;
copytPosition ((* yyNew)->SpecifiedRange.position, yyt->SpecifiedRange.position)
copyPtrContext ((* yyNew)->SpecifiedRange.context, yyt->SpecifiedRange.context)
copybool ((* yyNew)->SpecifiedRange.skip, yyt->SpecifiedRange.skip)
copySpan ((* yyNew)->SpecifiedRange.range, yyt->SpecifiedRange.range)
copyPtrType ((* yyNew)->SpecifiedRange.expectedType, yyt->SpecifiedRange.expectedType)
copybool ((* yyNew)->SpecifiedRange.isSpan, yyt->SpecifiedRange.isSpan)
copytTree ((* yyNew)->SpecifiedRange.Left, yyt->SpecifiedRange.Left)
yyt = yyt->SpecifiedRange.Right;
yyNew = & (* yyNew)->SpecifiedRange.Right; break;
case kTypeRange: (* yyNew)->TypeRange = yyt->TypeRange;
copytPosition ((* yyNew)->TypeRange.position, yyt->TypeRange.position)
copyPtrContext ((* yyNew)->TypeRange.context, yyt->TypeRange.context)
copybool ((* yyNew)->TypeRange.skip, yyt->TypeRange.skip)
copySpan ((* yyNew)->TypeRange.range, yyt->TypeRange.range)
copyPtrType ((* yyNew)->TypeRange.expectedType, yyt->TypeRange.expectedType)
copybool ((* yyNew)->TypeRange.isSpan, yyt->TypeRange.isSpan)
yyt = yyt->TypeRange.AType;
yyNew = & (* yyNew)->TypeRange.AType; break;
case kAType: (* yyNew)->AType = yyt->AType;
copytPosition ((* yyNew)->AType.position, yyt->AType.position)
copyPtrContext ((* yyNew)->AType.context, yyt->AType.context)
copybool ((* yyNew)->AType.skip, yyt->AType.skip)
copyPtrType ((* yyNew)->AType.type, yyt->AType.type)
return;
case kNullType: (* yyNew)->NullType = yyt->NullType;
copytPosition ((* yyNew)->NullType.position, yyt->NullType.position)
copyPtrContext ((* yyNew)->NullType.context, yyt->NullType.context)
copybool ((* yyNew)->NullType.skip, yyt->NullType.skip)
copyPtrType ((* yyNew)->NullType.type, yyt->NullType.type)
return;
case kNumericType: (* yyNew)->NumericType = yyt->NumericType;
copytPosition ((* yyNew)->NumericType.position, yyt->NumericType.position)
copyPtrContext ((* yyNew)->NumericType.context, yyt->NumericType.context)
copybool ((* yyNew)->NumericType.skip, yyt->NumericType.skip)
copyPtrType ((* yyNew)->NumericType.type, yyt->NumericType.type)
copybool ((* yyNew)->NumericType.signedness, yyt->NumericType.signedness)
yyt = yyt->NumericType.Expr;
yyNew = & (* yyNew)->NumericType.Expr; break;
case kExistingType: (* yyNew)->ExistingType = yyt->ExistingType;
copytPosition ((* yyNew)->ExistingType.position, yyt->ExistingType.position)
copyPtrContext ((* yyNew)->ExistingType.context, yyt->ExistingType.context)
copybool ((* yyNew)->ExistingType.skip, yyt->ExistingType.skip)
copyPtrType ((* yyNew)->ExistingType.type, yyt->ExistingType.type)
copytIdent ((* yyNew)->ExistingType.ident, yyt->ExistingType.ident)
return;
case kBuiltinType: (* yyNew)->BuiltinType = yyt->BuiltinType;
copytPosition ((* yyNew)->BuiltinType.position, yyt->BuiltinType.position)
copyPtrContext ((* yyNew)->BuiltinType.context, yyt->BuiltinType.context)
copybool ((* yyNew)->BuiltinType.skip, yyt->BuiltinType.skip)
copyPtrType ((* yyNew)->BuiltinType.type, yyt->BuiltinType.type)
return;
case kArrayType: (* yyNew)->ArrayType = yyt->ArrayType;
copytPosition ((* yyNew)->ArrayType.position, yyt->ArrayType.position)
copyPtrContext ((* yyNew)->ArrayType.context, yyt->ArrayType.context)
copybool ((* yyNew)->ArrayType.skip, yyt->ArrayType.skip)
copyPtrType ((* yyNew)->ArrayType.type, yyt->ArrayType.type)
copytTree ((* yyNew)->ArrayType.AType, yyt->ArrayType.AType)
yyt = yyt->ArrayType.Range;
yyNew = & (* yyNew)->ArrayType.Range; break;
case kNewType: (* yyNew)->NewType = yyt->NewType;
copytPosition ((* yyNew)->NewType.position, yyt->NewType.position)
copyPtrContext ((* yyNew)->NewType.context, yyt->NewType.context)
copybool ((* yyNew)->NewType.skip, yyt->NewType.skip)
copyPtrType ((* yyNew)->NewType.type, yyt->NewType.type)
yyt = yyt->NewType.AType;
yyNew = & (* yyNew)->NewType.AType; break;
case kRecordType: (* yyNew)->RecordType = yyt->RecordType;
copytPosition ((* yyNew)->RecordType.position, yyt->RecordType.position)
copyPtrContext ((* yyNew)->RecordType.context, yyt->RecordType.context)
copybool ((* yyNew)->RecordType.skip, yyt->RecordType.skip)
copyPtrType ((* yyNew)->RecordType.type, yyt->RecordType.type)
copytTree ((* yyNew)->RecordType.RecordElems, yyt->RecordType.RecordElems)
yyt = yyt->RecordType.AType;
yyNew = & (* yyNew)->RecordType.AType; break;
case kEnumType: (* yyNew)->EnumType = yyt->EnumType;
copytPosition ((* yyNew)->EnumType.position, yyt->EnumType.position)
copyPtrContext ((* yyNew)->EnumType.context, yyt->EnumType.context)
copybool ((* yyNew)->EnumType.skip, yyt->EnumType.skip)
copyPtrType ((* yyNew)->EnumType.type, yyt->EnumType.type)
copytTree ((* yyNew)->EnumType.EnumElems, yyt->EnumType.EnumElems)
yyt = yyt->EnumType.AType;
yyNew = & (* yyNew)->EnumType.AType; break;
case kCoercedExpr: (* yyNew)->CoercedExpr = yyt->CoercedExpr;
copyPtrContext ((* yyNew)->CoercedExpr.context, yyt->CoercedExpr.context)
copybool ((* yyNew)->CoercedExpr.skip, yyt->CoercedExpr.skip)
copyPtrType ((* yyNew)->CoercedExpr.expectedType, yyt->CoercedExpr.expectedType)
copyExprAttributes ((* yyNew)->CoercedExpr.attributes, yyt->CoercedExpr.attributes)
copybool ((* yyNew)->CoercedExpr.allowChannels, yyt->CoercedExpr.allowChannels)
copytPosition ((* yyNew)->CoercedExpr.position, yyt->CoercedExpr.position)
yyt = yyt->CoercedExpr.Expr;
yyNew = & (* yyNew)->CoercedExpr.Expr; break;
case kExpr: (* yyNew)->Expr = yyt->Expr;
copytPosition ((* yyNew)->Expr.position, yyt->Expr.position)
copyPtrContext ((* yyNew)->Expr.context, yyt->Expr.context)
copybool ((* yyNew)->Expr.skip, yyt->Expr.skip)
copyPtrType ((* yyNew)->Expr.expectedType, yyt->Expr.expectedType)
copybool ((* yyNew)->Expr.allowChannels, yyt->Expr.allowChannels)
copyExprAttributes ((* yyNew)->Expr.attributes, yyt->Expr.attributes)
return;
case kNullExpr: (* yyNew)->NullExpr = yyt->NullExpr;
copytPosition ((* yyNew)->NullExpr.position, yyt->NullExpr.position)
copyPtrContext ((* yyNew)->NullExpr.context, yyt->NullExpr.context)
copybool ((* yyNew)->NullExpr.skip, yyt->NullExpr.skip)
copyPtrType ((* yyNew)->NullExpr.expectedType, yyt->NullExpr.expectedType)
copybool ((* yyNew)->NullExpr.allowChannels, yyt->NullExpr.allowChannels)
copyExprAttributes ((* yyNew)->NullExpr.attributes, yyt->NullExpr.attributes)
return;
case kLiteralExpr: (* yyNew)->LiteralExpr = yyt->LiteralExpr;
copytPosition ((* yyNew)->LiteralExpr.position, yyt->LiteralExpr.position)
copyPtrContext ((* yyNew)->LiteralExpr.context, yyt->LiteralExpr.context)
copybool ((* yyNew)->LiteralExpr.skip, yyt->LiteralExpr.skip)
copyPtrType ((* yyNew)->LiteralExpr.expectedType, yyt->LiteralExpr.expectedType)
copybool ((* yyNew)->LiteralExpr.allowChannels, yyt->LiteralExpr.allowChannels)
copyExprAttributes ((* yyNew)->LiteralExpr.attributes, yyt->LiteralExpr.attributes)
copyPtrMP_INT ((* yyNew)->LiteralExpr.literal, yyt->LiteralExpr.literal)
return;
case kIdentExpr: (* yyNew)->IdentExpr = yyt->IdentExpr;
copytPosition ((* yyNew)->IdentExpr.position, yyt->IdentExpr.position)
copyPtrContext ((* yyNew)->IdentExpr.context, yyt->IdentExpr.context)
copybool ((* yyNew)->IdentExpr.skip, yyt->IdentExpr.skip)
copyPtrType ((* yyNew)->IdentExpr.expectedType, yyt->IdentExpr.expectedType)
copybool ((* yyNew)->IdentExpr.allowChannels, yyt->IdentExpr.allowChannels)
copyExprAttributes ((* yyNew)->IdentExpr.attributes, yyt->IdentExpr.attributes)
copytIdent ((* yyNew)->IdentExpr.ident, yyt->IdentExpr.ident)
return;
case kStringExpr: (* yyNew)->StringExpr = yyt->StringExpr;
copytPosition ((* yyNew)->StringExpr.position, yyt->StringExpr.position)
copyPtrContext ((* yyNew)->StringExpr.context, yyt->StringExpr.context)
copybool ((* yyNew)->StringExpr.skip, yyt->StringExpr.skip)
copyPtrType ((* yyNew)->StringExpr.expectedType, yyt->StringExpr.expectedType)
copybool ((* yyNew)->StringExpr.allowChannels, yyt->StringExpr.allowChannels)
copyExprAttributes ((* yyNew)->StringExpr.attributes, yyt->StringExpr.attributes)
copyPtrconstchar ((* yyNew)->StringExpr.string, yyt->StringExpr.string)
return;
case kImplicantExpr: (* yyNew)->ImplicantExpr = yyt->ImplicantExpr;
copytPosition ((* yyNew)->ImplicantExpr.position, yyt->ImplicantExpr.position)
copyPtrContext ((* yyNew)->ImplicantExpr.context, yyt->ImplicantExpr.context)
copybool ((* yyNew)->ImplicantExpr.skip, yyt->ImplicantExpr.skip)
copyPtrType ((* yyNew)->ImplicantExpr.expectedType, yyt->ImplicantExpr.expectedType)
copybool ((* yyNew)->ImplicantExpr.allowChannels, yyt->ImplicantExpr.allowChannels)
copyExprAttributes ((* yyNew)->ImplicantExpr.attributes, yyt->ImplicantExpr.attributes)
copyImplicant ((* yyNew)->ImplicantExpr.implicant, yyt->ImplicantExpr.implicant)
return;
case kDontCareExpr: (* yyNew)->DontCareExpr = yyt->DontCareExpr;
copytPosition ((* yyNew)->DontCareExpr.position, yyt->DontCareExpr.position)
copyPtrContext ((* yyNew)->DontCareExpr.context, yyt->DontCareExpr.context)
copybool ((* yyNew)->DontCareExpr.skip, yyt->DontCareExpr.skip)
copyPtrType ((* yyNew)->DontCareExpr.expectedType, yyt->DontCareExpr.expectedType)
copybool ((* yyNew)->DontCareExpr.allowChannels, yyt->DontCareExpr.allowChannels)
copyExprAttributes ((* yyNew)->DontCareExpr.attributes, yyt->DontCareExpr.attributes)
return;
case kAggregateConsExpr: (* yyNew)->AggregateConsExpr = yyt->AggregateConsExpr;
copytPosition ((* yyNew)->AggregateConsExpr.position, yyt->AggregateConsExpr.position)
copyPtrContext ((* yyNew)->AggregateConsExpr.context, yyt->AggregateConsExpr.context)
copybool ((* yyNew)->AggregateConsExpr.skip, yyt->AggregateConsExpr.skip)
copyPtrType ((* yyNew)->AggregateConsExpr.expectedType, yyt->AggregateConsExpr.expectedType)
copybool ((* yyNew)->AggregateConsExpr.allowChannels, yyt->AggregateConsExpr.allowChannels)
copyExprAttributes ((* yyNew)->AggregateConsExpr.attributes, yyt->AggregateConsExpr.attributes)
copytIdent ((* yyNew)->AggregateConsExpr.ident, yyt->AggregateConsExpr.ident)
copyPtrType ((* yyNew)->AggregateConsExpr.actualType, yyt->AggregateConsExpr.actualType)
yyt = yyt->AggregateConsExpr.ExprLists;
yyNew = & (* yyNew)->AggregateConsExpr.ExprLists; break;
case kNamedEnumElemExpr: (* yyNew)->NamedEnumElemExpr = yyt->NamedEnumElemExpr;
copytPosition ((* yyNew)->NamedEnumElemExpr.position, yyt->NamedEnumElemExpr.position)
copyPtrContext ((* yyNew)->NamedEnumElemExpr.context, yyt->NamedEnumElemExpr.context)
copybool ((* yyNew)->NamedEnumElemExpr.skip, yyt->NamedEnumElemExpr.skip)
copyPtrType ((* yyNew)->NamedEnumElemExpr.expectedType, yyt->NamedEnumElemExpr.expectedType)
copybool ((* yyNew)->NamedEnumElemExpr.allowChannels, yyt->NamedEnumElemExpr.allowChannels)
copyExprAttributes ((* yyNew)->NamedEnumElemExpr.attributes, yyt->NamedEnumElemExpr.attributes)
copytIdent ((* yyNew)->NamedEnumElemExpr.typeName, yyt->NamedEnumElemExpr.typeName)
copytIdent ((* yyNew)->NamedEnumElemExpr.elemName, yyt->NamedEnumElemExpr.elemName)
return;
case kUnaryExpr: (* yyNew)->UnaryExpr = yyt->UnaryExpr;
copytPosition ((* yyNew)->UnaryExpr.position, yyt->UnaryExpr.position)
copyPtrContext ((* yyNew)->UnaryExpr.context, yyt->UnaryExpr.context)
copybool ((* yyNew)->UnaryExpr.skip, yyt->UnaryExpr.skip)
copyPtrType ((* yyNew)->UnaryExpr.expectedType, yyt->UnaryExpr.expectedType)
copybool ((* yyNew)->UnaryExpr.allowChannels, yyt->UnaryExpr.allowChannels)
copyExprAttributes ((* yyNew)->UnaryExpr.attributes, yyt->UnaryExpr.attributes)
copyOperators ((* yyNew)->UnaryExpr.operation, yyt->UnaryExpr.operation)
yyt = yyt->UnaryExpr.Expr;
yyNew = & (* yyNew)->UnaryExpr.Expr; break;
case kBinaryExpr: (* yyNew)->BinaryExpr = yyt->BinaryExpr;
copytPosition ((* yyNew)->BinaryExpr.position, yyt->BinaryExpr.position)
copyPtrContext ((* yyNew)->BinaryExpr.context, yyt->BinaryExpr.context)
copybool ((* yyNew)->BinaryExpr.skip, yyt->BinaryExpr.skip)
copyPtrType ((* yyNew)->BinaryExpr.expectedType, yyt->BinaryExpr.expectedType)
copybool ((* yyNew)->BinaryExpr.allowChannels, yyt->BinaryExpr.allowChannels)
copyExprAttributes ((* yyNew)->BinaryExpr.attributes, yyt->BinaryExpr.attributes)
copyOperators ((* yyNew)->BinaryExpr.operation, yyt->BinaryExpr.operation)
copytTree ((* yyNew)->BinaryExpr.Left, yyt->BinaryExpr.Left)
yyt = yyt->BinaryExpr.Right;
yyNew = & (* yyNew)->BinaryExpr.Right; break;
case kRecordElemExtractExpr: (* yyNew)->RecordElemExtractExpr = yyt->RecordElemExtractExpr;
copytPosition ((* yyNew)->RecordElemExtractExpr.position, yyt->RecordElemExtractExpr.position)
copyPtrContext ((* yyNew)->RecordElemExtractExpr.context, yyt->RecordElemExtractExpr.context)
copybool ((* yyNew)->RecordElemExtractExpr.skip, yyt->RecordElemExtractExpr.skip)
copyPtrType ((* yyNew)->RecordElemExtractExpr.expectedType, yyt->RecordElemExtractExpr.expectedType)
copybool ((* yyNew)->RecordElemExtractExpr.allowChannels, yyt->RecordElemExtractExpr.allowChannels)
copyExprAttributes ((* yyNew)->RecordElemExtractExpr.attributes, yyt->RecordElemExtractExpr.attributes)
copytIdent ((* yyNew)->RecordElemExtractExpr.ident, yyt->RecordElemExtractExpr.ident)
yyt = yyt->RecordElemExtractExpr.Expr;
yyNew = & (* yyNew)->RecordElemExtractExpr.Expr; break;
case kArrayExtractExpr: (* yyNew)->ArrayExtractExpr = yyt->ArrayExtractExpr;
copytPosition ((* yyNew)->ArrayExtractExpr.position, yyt->ArrayExtractExpr.position)
copyPtrContext ((* yyNew)->ArrayExtractExpr.context, yyt->ArrayExtractExpr.context)
copybool ((* yyNew)->ArrayExtractExpr.skip, yyt->ArrayExtractExpr.skip)
copyPtrType ((* yyNew)->ArrayExtractExpr.expectedType, yyt->ArrayExtractExpr.expectedType)
copybool ((* yyNew)->ArrayExtractExpr.allowChannels, yyt->ArrayExtractExpr.allowChannels)
copyExprAttributes ((* yyNew)->ArrayExtractExpr.attributes, yyt->ArrayExtractExpr.attributes)
copytTree ((* yyNew)->ArrayExtractExpr.Array, yyt->ArrayExtractExpr.Array)
yyt = yyt->ArrayExtractExpr.Subscript;
yyNew = & (* yyNew)->ArrayExtractExpr.Subscript; break;
case kArraySliceExpr: (* yyNew)->ArraySliceExpr = yyt->ArraySliceExpr;
copytPosition ((* yyNew)->ArraySliceExpr.position, yyt->ArraySliceExpr.position)
copyPtrContext ((* yyNew)->ArraySliceExpr.context, yyt->ArraySliceExpr.context)
copybool ((* yyNew)->ArraySliceExpr.skip, yyt->ArraySliceExpr.skip)
copyPtrType ((* yyNew)->ArraySliceExpr.expectedType, yyt->ArraySliceExpr.expectedType)
copybool ((* yyNew)->ArraySliceExpr.allowChannels, yyt->ArraySliceExpr.allowChannels)
copyExprAttributes ((* yyNew)->ArraySliceExpr.attributes, yyt->ArraySliceExpr.attributes)
copytTree ((* yyNew)->ArraySliceExpr.Expr, yyt->ArraySliceExpr.Expr)
yyt = yyt->ArraySliceExpr.Range;
yyNew = & (* yyNew)->ArraySliceExpr.Range; break;
case kAsExpr: (* yyNew)->AsExpr = yyt->AsExpr;
copytPosition ((* yyNew)->AsExpr.position, yyt->AsExpr.position)
copyPtrContext ((* yyNew)->AsExpr.context, yyt->AsExpr.context)
copybool ((* yyNew)->AsExpr.skip, yyt->AsExpr.skip)
copyPtrType ((* yyNew)->AsExpr.expectedType, yyt->AsExpr.expectedType)
copybool ((* yyNew)->AsExpr.allowChannels, yyt->AsExpr.allowChannels)
copyExprAttributes ((* yyNew)->AsExpr.attributes, yyt->AsExpr.attributes)
copytTree ((* yyNew)->AsExpr.Expr, yyt->AsExpr.Expr)
yyt = yyt->AsExpr.AType;
yyNew = & (* yyNew)->AsExpr.AType; break;
case kBitArrayCastExpr: (* yyNew)->BitArrayCastExpr = yyt->BitArrayCastExpr;
copytPosition ((* yyNew)->BitArrayCastExpr.position, yyt->BitArrayCastExpr.position)
copyPtrContext ((* yyNew)->BitArrayCastExpr.context, yyt->BitArrayCastExpr.context)
copybool ((* yyNew)->BitArrayCastExpr.skip, yyt->BitArrayCastExpr.skip)
copyPtrType ((* yyNew)->BitArrayCastExpr.expectedType, yyt->BitArrayCastExpr.expectedType)
copybool ((* yyNew)->BitArrayCastExpr.allowChannels, yyt->BitArrayCastExpr.allowChannels)
copyExprAttributes ((* yyNew)->BitArrayCastExpr.attributes, yyt->BitArrayCastExpr.attributes)
yyt = yyt->BitArrayCastExpr.Expr;
yyNew = & (* yyNew)->BitArrayCastExpr.Expr; break;
case kLetExpr: (* yyNew)->LetExpr = yyt->LetExpr;
copytPosition ((* yyNew)->LetExpr.position, yyt->LetExpr.position)
copyPtrContext ((* yyNew)->LetExpr.context, yyt->LetExpr.context)
copybool ((* yyNew)->LetExpr.skip, yyt->LetExpr.skip)
copyPtrType ((* yyNew)->LetExpr.expectedType, yyt->LetExpr.expectedType)
copybool ((* yyNew)->LetExpr.allowChannels, yyt->LetExpr.allowChannels)
copyExprAttributes ((* yyNew)->LetExpr.attributes, yyt->LetExpr.attributes)
copytTree ((* yyNew)->LetExpr.ValDecls, yyt->LetExpr.ValDecls)
yyt = yyt->LetExpr.Expr;
yyNew = & (* yyNew)->LetExpr.Expr; break;
case kFunctionCallExpr: (* yyNew)->FunctionCallExpr = yyt->FunctionCallExpr;
copytPosition ((* yyNew)->FunctionCallExpr.position, yyt->FunctionCallExpr.position)
copyPtrContext ((* yyNew)->FunctionCallExpr.context, yyt->FunctionCallExpr.context)
copybool ((* yyNew)->FunctionCallExpr.skip, yyt->FunctionCallExpr.skip)
copyPtrType ((* yyNew)->FunctionCallExpr.expectedType, yyt->FunctionCallExpr.expectedType)
copybool ((* yyNew)->FunctionCallExpr.allowChannels, yyt->FunctionCallExpr.allowChannels)
copyExprAttributes ((* yyNew)->FunctionCallExpr.attributes, yyt->FunctionCallExpr.attributes)
copytIdent ((* yyNew)->FunctionCallExpr.ident, yyt->FunctionCallExpr.ident)
yyt = yyt->FunctionCallExpr.FunctionParams;
yyNew = & (* yyNew)->FunctionCallExpr.FunctionParams; break;
case kSizeofExpr: (* yyNew)->SizeofExpr = yyt->SizeofExpr;
copytPosition ((* yyNew)->SizeofExpr.position, yyt->SizeofExpr.position)
copyPtrContext ((* yyNew)->SizeofExpr.context, yyt->SizeofExpr.context)
copybool ((* yyNew)->SizeofExpr.skip, yyt->SizeofExpr.skip)
copyPtrType ((* yyNew)->SizeofExpr.expectedType, yyt->SizeofExpr.expectedType)
copybool ((* yyNew)->SizeofExpr.allowChannels, yyt->SizeofExpr.allowChannels)
copyExprAttributes ((* yyNew)->SizeofExpr.attributes, yyt->SizeofExpr.attributes)
copytIdent ((* yyNew)->SizeofExpr.ident, yyt->SizeofExpr.ident)
return;
case kArrayAppendExpr: (* yyNew)->ArrayAppendExpr = yyt->ArrayAppendExpr;
copytPosition ((* yyNew)->ArrayAppendExpr.position, yyt->ArrayAppendExpr.position)
copyPtrContext ((* yyNew)->ArrayAppendExpr.context, yyt->ArrayAppendExpr.context)
copybool ((* yyNew)->ArrayAppendExpr.skip, yyt->ArrayAppendExpr.skip)
copyPtrType ((* yyNew)->ArrayAppendExpr.expectedType, yyt->ArrayAppendExpr.expectedType)
copybool ((* yyNew)->ArrayAppendExpr.allowChannels, yyt->ArrayAppendExpr.allowChannels)
copyExprAttributes ((* yyNew)->ArrayAppendExpr.attributes, yyt->ArrayAppendExpr.attributes)
copytTree ((* yyNew)->ArrayAppendExpr.Left, yyt->ArrayAppendExpr.Left)
yyt = yyt->ArrayAppendExpr.Right;
yyNew = & (* yyNew)->ArrayAppendExpr.Right; break;
case kPrimedExpr: (* yyNew)->PrimedExpr = yyt->PrimedExpr;
copytPosition ((* yyNew)->PrimedExpr.position, yyt->PrimedExpr.position)
copyPtrContext ((* yyNew)->PrimedExpr.context, yyt->PrimedExpr.context)
copybool ((* yyNew)->PrimedExpr.skip, yyt->PrimedExpr.skip)
copyPtrType ((* yyNew)->PrimedExpr.expectedType, yyt->PrimedExpr.expectedType)
copybool ((* yyNew)->PrimedExpr.allowChannels, yyt->PrimedExpr.allowChannels)
copyExprAttributes ((* yyNew)->PrimedExpr.attributes, yyt->PrimedExpr.attributes)
copyExprAttributes ((* yyNew)->PrimedExpr.results, yyt->PrimedExpr.results)
return;
case kFormalPorts: (* yyNew)->FormalPorts = yyt->FormalPorts;
copytPosition ((* yyNew)->FormalPorts.position, yyt->FormalPorts.position)
copyPtrContext ((* yyNew)->FormalPorts.context, yyt->FormalPorts.context)
copybool ((* yyNew)->FormalPorts.skip, yyt->FormalPorts.skip)
copyProcedureArgsType ((* yyNew)->FormalPorts.portsType, yyt->FormalPorts.portsType)
copybool ((* yyNew)->FormalPorts.inConditionalPorts, yyt->FormalPorts.inConditionalPorts)
copyPtrInstanceList ((* yyNew)->FormalPorts.ports, yyt->FormalPorts.ports)
copyunsigned ((* yyNew)->FormalPorts.portCount, yyt->FormalPorts.portCount)
copyPtrInstanceList ((* yyNew)->FormalPorts.extraPortInstances, yyt->FormalPorts.extraPortInstances)
copybool ((* yyNew)->FormalPorts.hasParameters, yyt->FormalPorts.hasParameters)
return;
case kNullFormalPorts: (* yyNew)->NullFormalPorts = yyt->NullFormalPorts;
copytPosition ((* yyNew)->NullFormalPorts.position, yyt->NullFormalPorts.position)
copyPtrContext ((* yyNew)->NullFormalPorts.context, yyt->NullFormalPorts.context)
copybool ((* yyNew)->NullFormalPorts.skip, yyt->NullFormalPorts.skip)
copyProcedureArgsType ((* yyNew)->NullFormalPorts.portsType, yyt->NullFormalPorts.portsType)
copybool ((* yyNew)->NullFormalPorts.inConditionalPorts, yyt->NullFormalPorts.inConditionalPorts)
copyPtrInstanceList ((* yyNew)->NullFormalPorts.ports, yyt->NullFormalPorts.ports)
copyunsigned ((* yyNew)->NullFormalPorts.portCount, yyt->NullFormalPorts.portCount)
copyPtrInstanceList ((* yyNew)->NullFormalPorts.extraPortInstances, yyt->NullFormalPorts.extraPortInstances)
copybool ((* yyNew)->NullFormalPorts.hasParameters, yyt->NullFormalPorts.hasParameters)
return;
case kFormalPort: (* yyNew)->FormalPort = yyt->FormalPort;
copytPosition ((* yyNew)->FormalPort.position, yyt->FormalPort.position)
copyPtrContext ((* yyNew)->FormalPort.context, yyt->FormalPort.context)
copybool ((* yyNew)->FormalPort.skip, yyt->FormalPort.skip)
copyProcedureArgsType ((* yyNew)->FormalPort.portsType, yyt->FormalPort.portsType)
copybool ((* yyNew)->FormalPort.inConditionalPorts, yyt->FormalPort.inConditionalPorts)
copyPtrInstanceList ((* yyNew)->FormalPort.ports, yyt->FormalPort.ports)
copyunsigned ((* yyNew)->FormalPort.portCount, yyt->FormalPort.portCount)
copyPtrInstanceList ((* yyNew)->FormalPort.extraPortInstances, yyt->FormalPort.extraPortInstances)
copybool ((* yyNew)->FormalPort.hasParameters, yyt->FormalPort.hasParameters)
yyt = yyt->FormalPort.next;
yyNew = & (* yyNew)->FormalPort.next; break;
case kValuePort: (* yyNew)->ValuePort = yyt->ValuePort;
copytPosition ((* yyNew)->ValuePort.position, yyt->ValuePort.position)
copyPtrContext ((* yyNew)->ValuePort.context, yyt->ValuePort.context)
copybool ((* yyNew)->ValuePort.skip, yyt->ValuePort.skip)
copyProcedureArgsType ((* yyNew)->ValuePort.portsType, yyt->ValuePort.portsType)
copybool ((* yyNew)->ValuePort.inConditionalPorts, yyt->ValuePort.inConditionalPorts)
copyPtrInstanceList ((* yyNew)->ValuePort.ports, yyt->ValuePort.ports)
copyunsigned ((* yyNew)->ValuePort.portCount, yyt->ValuePort.portCount)
copyPtrInstanceList ((* yyNew)->ValuePort.extraPortInstances, yyt->ValuePort.extraPortInstances)
copybool ((* yyNew)->ValuePort.hasParameters, yyt->ValuePort.hasParameters)
copytTree ((* yyNew)->ValuePort.Idents, yyt->ValuePort.Idents)
copytTree ((* yyNew)->ValuePort.AType, yyt->ValuePort.AType)
yyt = yyt->ValuePort.next;
yyNew = & (* yyNew)->ValuePort.next; break;
case kParamPort: (* yyNew)->ParamPort = yyt->ParamPort;
copytPosition ((* yyNew)->ParamPort.position, yyt->ParamPort.position)
copyPtrContext ((* yyNew)->ParamPort.context, yyt->ParamPort.context)
copybool ((* yyNew)->ParamPort.skip, yyt->ParamPort.skip)
copyProcedureArgsType ((* yyNew)->ParamPort.portsType, yyt->ParamPort.portsType)
copybool ((* yyNew)->ParamPort.inConditionalPorts, yyt->ParamPort.inConditionalPorts)
copyPtrInstanceList ((* yyNew)->ParamPort.ports, yyt->ParamPort.ports)
copyunsigned ((* yyNew)->ParamPort.portCount, yyt->ParamPort.portCount)
copyPtrInstanceList ((* yyNew)->ParamPort.extraPortInstances, yyt->ParamPort.extraPortInstances)
copybool ((* yyNew)->ParamPort.hasParameters, yyt->ParamPort.hasParameters)
copytTree ((* yyNew)->ParamPort.Idents, yyt->ParamPort.Idents)
copytTree ((* yyNew)->ParamPort.AType, yyt->ParamPort.AType)
yyt = yyt->ParamPort.next;
yyNew = & (* yyNew)->ParamPort.next; break;
case kTypeParamPort: (* yyNew)->TypeParamPort = yyt->TypeParamPort;
copytPosition ((* yyNew)->TypeParamPort.position, yyt->TypeParamPort.position)
copyPtrContext ((* yyNew)->TypeParamPort.context, yyt->TypeParamPort.context)
copybool ((* yyNew)->TypeParamPort.skip, yyt->TypeParamPort.skip)
copyProcedureArgsType ((* yyNew)->TypeParamPort.portsType, yyt->TypeParamPort.portsType)
copybool ((* yyNew)->TypeParamPort.inConditionalPorts, yyt->TypeParamPort.inConditionalPorts)
copyPtrInstanceList ((* yyNew)->TypeParamPort.ports, yyt->TypeParamPort.ports)
copyunsigned ((* yyNew)->TypeParamPort.portCount, yyt->TypeParamPort.portCount)
copyPtrInstanceList ((* yyNew)->TypeParamPort.extraPortInstances, yyt->TypeParamPort.extraPortInstances)
copybool ((* yyNew)->TypeParamPort.hasParameters, yyt->TypeParamPort.hasParameters)
copytTree ((* yyNew)->TypeParamPort.Idents, yyt->TypeParamPort.Idents)
yyt = yyt->TypeParamPort.next;
yyNew = & (* yyNew)->TypeParamPort.next; break;
case kChannelPort: (* yyNew)->ChannelPort = yyt->ChannelPort;
copytPosition ((* yyNew)->ChannelPort.position, yyt->ChannelPort.position)
copyPtrContext ((* yyNew)->ChannelPort.context, yyt->ChannelPort.context)
copybool ((* yyNew)->ChannelPort.skip, yyt->ChannelPort.skip)
copyProcedureArgsType ((* yyNew)->ChannelPort.portsType, yyt->ChannelPort.portsType)
copybool ((* yyNew)->ChannelPort.inConditionalPorts, yyt->ChannelPort.inConditionalPorts)
copyPtrInstanceList ((* yyNew)->ChannelPort.ports, yyt->ChannelPort.ports)
copyunsigned ((* yyNew)->ChannelPort.portCount, yyt->ChannelPort.portCount)
copyPtrInstanceList ((* yyNew)->ChannelPort.extraPortInstances, yyt->ChannelPort.extraPortInstances)
copybool ((* yyNew)->ChannelPort.hasParameters, yyt->ChannelPort.hasParameters)
copytTree ((* yyNew)->ChannelPort.Idents, yyt->ChannelPort.Idents)
copytTree ((* yyNew)->ChannelPort.AType, yyt->ChannelPort.AType)
copyPortSense ((* yyNew)->ChannelPort.portSense, yyt->ChannelPort.portSense)
copybool ((* yyNew)->ChannelPort.isOutput, yyt->ChannelPort.isOutput)
copyPtrLispList ((* yyNew)->ChannelPort.options, yyt->ChannelPort.options)
yyt = yyt->ChannelPort.next;
yyNew = & (* yyNew)->ChannelPort.next; break;
case kChannelPortArray: (* yyNew)->ChannelPortArray = yyt->ChannelPortArray;
copytPosition ((* yyNew)->ChannelPortArray.position, yyt->ChannelPortArray.position)
copyPtrContext ((* yyNew)->ChannelPortArray.context, yyt->ChannelPortArray.context)
copybool ((* yyNew)->ChannelPortArray.skip, yyt->ChannelPortArray.skip)
copyProcedureArgsType ((* yyNew)->ChannelPortArray.portsType, yyt->ChannelPortArray.portsType)
copybool ((* yyNew)->ChannelPortArray.inConditionalPorts, yyt->ChannelPortArray.inConditionalPorts)
copyPtrInstanceList ((* yyNew)->ChannelPortArray.ports, yyt->ChannelPortArray.ports)
copyunsigned ((* yyNew)->ChannelPortArray.portCount, yyt->ChannelPortArray.portCount)
copyPtrInstanceList ((* yyNew)->ChannelPortArray.extraPortInstances, yyt->ChannelPortArray.extraPortInstances)
copybool ((* yyNew)->ChannelPortArray.hasParameters, yyt->ChannelPortArray.hasParameters)
copytTree ((* yyNew)->ChannelPortArray.Idents, yyt->ChannelPortArray.Idents)
copytTree ((* yyNew)->ChannelPortArray.AType, yyt->ChannelPortArray.AType)
copyPortSense ((* yyNew)->ChannelPortArray.portSense, yyt->ChannelPortArray.portSense)
copybool ((* yyNew)->ChannelPortArray.isOutput, yyt->ChannelPortArray.isOutput)
copytTree ((* yyNew)->ChannelPortArray.Range, yyt->ChannelPortArray.Range)
copyPtrLispList ((* yyNew)->ChannelPortArray.options, yyt->ChannelPortArray.options)
yyt = yyt->ChannelPortArray.next;
yyNew = & (* yyNew)->ChannelPortArray.next; break;
case kSyncPort: (* yyNew)->SyncPort = yyt->SyncPort;
copytPosition ((* yyNew)->SyncPort.position, yyt->SyncPort.position)
copyPtrContext ((* yyNew)->SyncPort.context, yyt->SyncPort.context)
copybool ((* yyNew)->SyncPort.skip, yyt->SyncPort.skip)
copyProcedureArgsType ((* yyNew)->SyncPort.portsType, yyt->SyncPort.portsType)
copybool ((* yyNew)->SyncPort.inConditionalPorts, yyt->SyncPort.inConditionalPorts)
copyPtrInstanceList ((* yyNew)->SyncPort.ports, yyt->SyncPort.ports)
copyunsigned ((* yyNew)->SyncPort.portCount, yyt->SyncPort.portCount)
copyPtrInstanceList ((* yyNew)->SyncPort.extraPortInstances, yyt->SyncPort.extraPortInstances)
copybool ((* yyNew)->SyncPort.hasParameters, yyt->SyncPort.hasParameters)
copytTree ((* yyNew)->SyncPort.Idents, yyt->SyncPort.Idents)
copyPortSense ((* yyNew)->SyncPort.portSense, yyt->SyncPort.portSense)
copyPtrLispList ((* yyNew)->SyncPort.options, yyt->SyncPort.options)
yyt = yyt->SyncPort.next;
yyNew = & (* yyNew)->SyncPort.next; break;
case kSyncPortArray: (* yyNew)->SyncPortArray = yyt->SyncPortArray;
copytPosition ((* yyNew)->SyncPortArray.position, yyt->SyncPortArray.position)
copyPtrContext ((* yyNew)->SyncPortArray.context, yyt->SyncPortArray.context)
copybool ((* yyNew)->SyncPortArray.skip, yyt->SyncPortArray.skip)
copyProcedureArgsType ((* yyNew)->SyncPortArray.portsType, yyt->SyncPortArray.portsType)
copybool ((* yyNew)->SyncPortArray.inConditionalPorts, yyt->SyncPortArray.inConditionalPorts)
copyPtrInstanceList ((* yyNew)->SyncPortArray.ports, yyt->SyncPortArray.ports)
copyunsigned ((* yyNew)->SyncPortArray.portCount, yyt->SyncPortArray.portCount)
copyPtrInstanceList ((* yyNew)->SyncPortArray.extraPortInstances, yyt->SyncPortArray.extraPortInstances)
copybool ((* yyNew)->SyncPortArray.hasParameters, yyt->SyncPortArray.hasParameters)
copytTree ((* yyNew)->SyncPortArray.Idents, yyt->SyncPortArray.Idents)
copyPortSense ((* yyNew)->SyncPortArray.portSense, yyt->SyncPortArray.portSense)
copytTree ((* yyNew)->SyncPortArray.Range, yyt->SyncPortArray.Range)
copyPtrLispList ((* yyNew)->SyncPortArray.options, yyt->SyncPortArray.options)
yyt = yyt->SyncPortArray.next;
yyNew = & (* yyNew)->SyncPortArray.next; break;
case kIfPorts: (* yyNew)->IfPorts = yyt->IfPorts;
copytPosition ((* yyNew)->IfPorts.position, yyt->IfPorts.position)
copyPtrContext ((* yyNew)->IfPorts.context, yyt->IfPorts.context)
copybool ((* yyNew)->IfPorts.skip, yyt->IfPorts.skip)
copyProcedureArgsType ((* yyNew)->IfPorts.portsType, yyt->IfPorts.portsType)
copybool ((* yyNew)->IfPorts.inConditionalPorts, yyt->IfPorts.inConditionalPorts)
copyPtrInstanceList ((* yyNew)->IfPorts.ports, yyt->IfPorts.ports)
copyunsigned ((* yyNew)->IfPorts.portCount, yyt->IfPorts.portCount)
copyPtrInstanceList ((* yyNew)->IfPorts.extraPortInstances, yyt->IfPorts.extraPortInstances)
copybool ((* yyNew)->IfPorts.hasParameters, yyt->IfPorts.hasParameters)
copytTree ((* yyNew)->IfPorts.PortGuards, yyt->IfPorts.PortGuards)
yyt = yyt->IfPorts.next;
yyNew = & (* yyNew)->IfPorts.next; break;
case kIfElsePorts: (* yyNew)->IfElsePorts = yyt->IfElsePorts;
copytPosition ((* yyNew)->IfElsePorts.position, yyt->IfElsePorts.position)
copyPtrContext ((* yyNew)->IfElsePorts.context, yyt->IfElsePorts.context)
copybool ((* yyNew)->IfElsePorts.skip, yyt->IfElsePorts.skip)
copyProcedureArgsType ((* yyNew)->IfElsePorts.portsType, yyt->IfElsePorts.portsType)
copybool ((* yyNew)->IfElsePorts.inConditionalPorts, yyt->IfElsePorts.inConditionalPorts)
copyPtrInstanceList ((* yyNew)->IfElsePorts.ports, yyt->IfElsePorts.ports)
copyunsigned ((* yyNew)->IfElsePorts.portCount, yyt->IfElsePorts.portCount)
copyPtrInstanceList ((* yyNew)->IfElsePorts.extraPortInstances, yyt->IfElsePorts.extraPortInstances)
copybool ((* yyNew)->IfElsePorts.hasParameters, yyt->IfElsePorts.hasParameters)
copytTree ((* yyNew)->IfElsePorts.PortGuards, yyt->IfElsePorts.PortGuards)
copytTree ((* yyNew)->IfElsePorts.FormalPorts, yyt->IfElsePorts.FormalPorts)
yyt = yyt->IfElsePorts.next;
yyNew = & (* yyNew)->IfElsePorts.next; break;
case kBlock: (* yyNew)->Block = yyt->Block;
copytPosition ((* yyNew)->Block.position, yyt->Block.position)
copytTree ((* yyNew)->Block.Decls, yyt->Block.Decls)
copyPtrContext ((* yyNew)->Block.context, yyt->Block.context)
copybool ((* yyNew)->Block.skip, yyt->Block.skip)
copyCommandAttributes ((* yyNew)->Block.attributes, yyt->Block.attributes)
yyt = yyt->Block.Command;
yyNew = & (* yyNew)->Block.Command; break;
case kParameters: (* yyNew)->Parameters = yyt->Parameters;
copytPosition ((* yyNew)->Parameters.position, yyt->Parameters.position)
copyPtrContext ((* yyNew)->Parameters.context, yyt->Parameters.context)
copybool ((* yyNew)->Parameters.skip, yyt->Parameters.skip)
copyunsigned ((* yyNew)->Parameters.paramNoIn, yyt->Parameters.paramNoIn)
copyunsigned ((* yyNew)->Parameters.paramNoOut, yyt->Parameters.paramNoOut)
copyPtrComponentParameterList ((* yyNew)->Parameters.parameters, yyt->Parameters.parameters)
copyPtrComponent ((* yyNew)->Parameters.component, yyt->Parameters.component)
return;
case kNullParameters: (* yyNew)->NullParameters = yyt->NullParameters;
copytPosition ((* yyNew)->NullParameters.position, yyt->NullParameters.position)
copyPtrContext ((* yyNew)->NullParameters.context, yyt->NullParameters.context)
copybool ((* yyNew)->NullParameters.skip, yyt->NullParameters.skip)
copyunsigned ((* yyNew)->NullParameters.paramNoIn, yyt->NullParameters.paramNoIn)
copyunsigned ((* yyNew)->NullParameters.paramNoOut, yyt->NullParameters.paramNoOut)
copyPtrComponentParameterList ((* yyNew)->NullParameters.parameters, yyt->NullParameters.parameters)
copyPtrComponent ((* yyNew)->NullParameters.component, yyt->NullParameters.component)
return;
case kParameter: (* yyNew)->Parameter = yyt->Parameter;
copytPosition ((* yyNew)->Parameter.position, yyt->Parameter.position)
copyPtrContext ((* yyNew)->Parameter.context, yyt->Parameter.context)
copybool ((* yyNew)->Parameter.skip, yyt->Parameter.skip)
copyunsigned ((* yyNew)->Parameter.paramNoIn, yyt->Parameter.paramNoIn)
copyunsigned ((* yyNew)->Parameter.paramNoOut, yyt->Parameter.paramNoOut)
copyPtrComponentParameterList ((* yyNew)->Parameter.parameters, yyt->Parameter.parameters)
copyPtrComponent ((* yyNew)->Parameter.component, yyt->Parameter.component)
yyt = yyt->Parameter.next;
yyNew = & (* yyNew)->Parameter.next; break;
case kNumberParameter: (* yyNew)->NumberParameter = yyt->NumberParameter;
copytPosition ((* yyNew)->NumberParameter.position, yyt->NumberParameter.position)
copyPtrContext ((* yyNew)->NumberParameter.context, yyt->NumberParameter.context)
copybool ((* yyNew)->NumberParameter.skip, yyt->NumberParameter.skip)
copyunsigned ((* yyNew)->NumberParameter.paramNoIn, yyt->NumberParameter.paramNoIn)
copyunsigned ((* yyNew)->NumberParameter.paramNoOut, yyt->NumberParameter.paramNoOut)
copyPtrComponentParameterList ((* yyNew)->NumberParameter.parameters, yyt->NumberParameter.parameters)
copyPtrComponent ((* yyNew)->NumberParameter.component, yyt->NumberParameter.component)
copyPtrMP_INT ((* yyNew)->NumberParameter.literal, yyt->NumberParameter.literal)
yyt = yyt->NumberParameter.next;
yyNew = & (* yyNew)->NumberParameter.next; break;
case kStringParameter: (* yyNew)->StringParameter = yyt->StringParameter;
copytPosition ((* yyNew)->StringParameter.position, yyt->StringParameter.position)
copyPtrContext ((* yyNew)->StringParameter.context, yyt->StringParameter.context)
copybool ((* yyNew)->StringParameter.skip, yyt->StringParameter.skip)
copyunsigned ((* yyNew)->StringParameter.paramNoIn, yyt->StringParameter.paramNoIn)
copyunsigned ((* yyNew)->StringParameter.paramNoOut, yyt->StringParameter.paramNoOut)
copyPtrComponentParameterList ((* yyNew)->StringParameter.parameters, yyt->StringParameter.parameters)
copyPtrComponent ((* yyNew)->StringParameter.component, yyt->StringParameter.component)
copytIdent ((* yyNew)->StringParameter.ident, yyt->StringParameter.ident)
yyt = yyt->StringParameter.next;
yyNew = & (* yyNew)->StringParameter.next; break;
case kTypeParameter: (* yyNew)->TypeParameter = yyt->TypeParameter;
copytPosition ((* yyNew)->TypeParameter.position, yyt->TypeParameter.position)
copyPtrContext ((* yyNew)->TypeParameter.context, yyt->TypeParameter.context)
copybool ((* yyNew)->TypeParameter.skip, yyt->TypeParameter.skip)
copyunsigned ((* yyNew)->TypeParameter.paramNoIn, yyt->TypeParameter.paramNoIn)
copyunsigned ((* yyNew)->TypeParameter.paramNoOut, yyt->TypeParameter.paramNoOut)
copyPtrComponentParameterList ((* yyNew)->TypeParameter.parameters, yyt->TypeParameter.parameters)
copyPtrComponent ((* yyNew)->TypeParameter.component, yyt->TypeParameter.component)
copytTree ((* yyNew)->TypeParameter.AType, yyt->TypeParameter.AType)
yyt = yyt->TypeParameter.next;
yyNew = & (* yyNew)->TypeParameter.next; break;
case kBreezeParameters: (* yyNew)->BreezeParameters = yyt->BreezeParameters;
copytPosition ((* yyNew)->BreezeParameters.position, yyt->BreezeParameters.position)
copyPtrContext ((* yyNew)->BreezeParameters.context, yyt->BreezeParameters.context)
copybool ((* yyNew)->BreezeParameters.skip, yyt->BreezeParameters.skip)
copyPtrComponentParameterList ((* yyNew)->BreezeParameters.parameters, yyt->BreezeParameters.parameters)
return;
case kNullBreezeParameters: (* yyNew)->NullBreezeParameters = yyt->NullBreezeParameters;
copytPosition ((* yyNew)->NullBreezeParameters.position, yyt->NullBreezeParameters.position)
copyPtrContext ((* yyNew)->NullBreezeParameters.context, yyt->NullBreezeParameters.context)
copybool ((* yyNew)->NullBreezeParameters.skip, yyt->NullBreezeParameters.skip)
copyPtrComponentParameterList ((* yyNew)->NullBreezeParameters.parameters, yyt->NullBreezeParameters.parameters)
return;
case kBreezeParameter: (* yyNew)->BreezeParameter = yyt->BreezeParameter;
copytPosition ((* yyNew)->BreezeParameter.position, yyt->BreezeParameter.position)
copyPtrContext ((* yyNew)->BreezeParameter.context, yyt->BreezeParameter.context)
copybool ((* yyNew)->BreezeParameter.skip, yyt->BreezeParameter.skip)
copyPtrComponentParameterList ((* yyNew)->BreezeParameter.parameters, yyt->BreezeParameter.parameters)
yyt = yyt->BreezeParameter.next;
yyNew = & (* yyNew)->BreezeParameter.next; break;
case kBreezeExprParameter: (* yyNew)->BreezeExprParameter = yyt->BreezeExprParameter;
copytPosition ((* yyNew)->BreezeExprParameter.position, yyt->BreezeExprParameter.position)
copyPtrContext ((* yyNew)->BreezeExprParameter.context, yyt->BreezeExprParameter.context)
copybool ((* yyNew)->BreezeExprParameter.skip, yyt->BreezeExprParameter.skip)
copyPtrComponentParameterList ((* yyNew)->BreezeExprParameter.parameters, yyt->BreezeExprParameter.parameters)
copytIdent ((* yyNew)->BreezeExprParameter.ident, yyt->BreezeExprParameter.ident)
copytTree ((* yyNew)->BreezeExprParameter.AType, yyt->BreezeExprParameter.AType)
yyt = yyt->BreezeExprParameter.next;
yyNew = & (* yyNew)->BreezeExprParameter.next; break;
case kBreezeTypeParameter: (* yyNew)->BreezeTypeParameter = yyt->BreezeTypeParameter;
copytPosition ((* yyNew)->BreezeTypeParameter.position, yyt->BreezeTypeParameter.position)
copyPtrContext ((* yyNew)->BreezeTypeParameter.context, yyt->BreezeTypeParameter.context)
copybool ((* yyNew)->BreezeTypeParameter.skip, yyt->BreezeTypeParameter.skip)
copyPtrComponentParameterList ((* yyNew)->BreezeTypeParameter.parameters, yyt->BreezeTypeParameter.parameters)
copytIdent ((* yyNew)->BreezeTypeParameter.ident, yyt->BreezeTypeParameter.ident)
yyt = yyt->BreezeTypeParameter.next;
yyNew = & (* yyNew)->BreezeTypeParameter.next; break;
case kComps: (* yyNew)->Comps = yyt->Comps;
copytPosition ((* yyNew)->Comps.position, yyt->Comps.position)
copyPtrContext ((* yyNew)->Comps.context, yyt->Comps.context)
copybool ((* yyNew)->Comps.skip, yyt->Comps.skip)
copyPtrWireArray ((* yyNew)->Comps.channels, yyt->Comps.channels)
copyunsigned ((* yyNew)->Comps.channelCount, yyt->Comps.channelCount)
copyPtrComponentList ((* yyNew)->Comps.componentsIn, yyt->Comps.componentsIn)
copyPtrComponentList ((* yyNew)->Comps.componentsOut, yyt->Comps.componentsOut)
return;
case kNullComps: (* yyNew)->NullComps = yyt->NullComps;
copytPosition ((* yyNew)->NullComps.position, yyt->NullComps.position)
copyPtrContext ((* yyNew)->NullComps.context, yyt->NullComps.context)
copybool ((* yyNew)->NullComps.skip, yyt->NullComps.skip)
copyPtrWireArray ((* yyNew)->NullComps.channels, yyt->NullComps.channels)
copyunsigned ((* yyNew)->NullComps.channelCount, yyt->NullComps.channelCount)
copyPtrComponentList ((* yyNew)->NullComps.componentsIn, yyt->NullComps.componentsIn)
copyPtrComponentList ((* yyNew)->NullComps.componentsOut, yyt->NullComps.componentsOut)
return;
case kComp: (* yyNew)->Comp = yyt->Comp;
copytPosition ((* yyNew)->Comp.position, yyt->Comp.position)
copyPtrContext ((* yyNew)->Comp.context, yyt->Comp.context)
copybool ((* yyNew)->Comp.skip, yyt->Comp.skip)
copyPtrWireArray ((* yyNew)->Comp.channels, yyt->Comp.channels)
copyunsigned ((* yyNew)->Comp.channelCount, yyt->Comp.channelCount)
copyPtrComponentList ((* yyNew)->Comp.componentsIn, yyt->Comp.componentsIn)
copyPtrComponentList ((* yyNew)->Comp.componentsOut, yyt->Comp.componentsOut)
yyt = yyt->Comp.next;
yyNew = & (* yyNew)->Comp.next; break;
case kNormalComp: (* yyNew)->NormalComp = yyt->NormalComp;
copytPosition ((* yyNew)->NormalComp.position, yyt->NormalComp.position)
copyPtrContext ((* yyNew)->NormalComp.context, yyt->NormalComp.context)
copybool ((* yyNew)->NormalComp.skip, yyt->NormalComp.skip)
copyPtrWireArray ((* yyNew)->NormalComp.channels, yyt->NormalComp.channels)
copyunsigned ((* yyNew)->NormalComp.channelCount, yyt->NormalComp.channelCount)
copyPtrComponentList ((* yyNew)->NormalComp.componentsIn, yyt->NormalComp.componentsIn)
copyPtrComponentList ((* yyNew)->NormalComp.componentsOut, yyt->NormalComp.componentsOut)
copytTree ((* yyNew)->NormalComp.next, yyt->NormalComp.next)
copytIdent ((* yyNew)->NormalComp.ident, yyt->NormalComp.ident)
copybool ((* yyNew)->NormalComp.internal, yyt->NormalComp.internal)
copyPtrintList ((* yyNew)->NormalComp.channelNos, yyt->NormalComp.channelNos)
copyPtrLispList ((* yyNew)->NormalComp.options, yyt->NormalComp.options)
yyt = yyt->NormalComp.Parameters;
yyNew = & (* yyNew)->NormalComp.Parameters; break;
case kUndeclaredComp: (* yyNew)->UndeclaredComp = yyt->UndeclaredComp;
copytPosition ((* yyNew)->UndeclaredComp.position, yyt->UndeclaredComp.position)
copyPtrContext ((* yyNew)->UndeclaredComp.context, yyt->UndeclaredComp.context)
copybool ((* yyNew)->UndeclaredComp.skip, yyt->UndeclaredComp.skip)
copyPtrWireArray ((* yyNew)->UndeclaredComp.channels, yyt->UndeclaredComp.channels)
copyunsigned ((* yyNew)->UndeclaredComp.channelCount, yyt->UndeclaredComp.channelCount)
copyPtrComponentList ((* yyNew)->UndeclaredComp.componentsIn, yyt->UndeclaredComp.componentsIn)
copyPtrComponentList ((* yyNew)->UndeclaredComp.componentsOut, yyt->UndeclaredComp.componentsOut)
copytTree ((* yyNew)->UndeclaredComp.next, yyt->UndeclaredComp.next)
copytIdent ((* yyNew)->UndeclaredComp.ident, yyt->UndeclaredComp.ident)
copytTree ((* yyNew)->UndeclaredComp.Parameters, yyt->UndeclaredComp.Parameters)
copyPtrintList ((* yyNew)->UndeclaredComp.channelNos, yyt->UndeclaredComp.channelNos)
copytIdent ((* yyNew)->UndeclaredComp.componentType, yyt->UndeclaredComp.componentType)
copytIdent ((* yyNew)->UndeclaredComp.baseComponentName, yyt->UndeclaredComp.baseComponentName)
copytTree ((* yyNew)->UndeclaredComp.BreezeParameters, yyt->UndeclaredComp.BreezeParameters)
copyPtrLispList ((* yyNew)->UndeclaredComp.options, yyt->UndeclaredComp.options)
yyt = yyt->UndeclaredComp.FormalPorts;
yyNew = & (* yyNew)->UndeclaredComp.FormalPorts; break;
case kCommand: (* yyNew)->Command = yyt->Command;
copytPosition ((* yyNew)->Command.position, yyt->Command.position)
copyPtrContext ((* yyNew)->Command.context, yyt->Command.context)
copybool ((* yyNew)->Command.skip, yyt->Command.skip)
copyCommandAttributes ((* yyNew)->Command.attributes, yyt->Command.attributes)
return;
case kNullCommand: (* yyNew)->NullCommand = yyt->NullCommand;
copytPosition ((* yyNew)->NullCommand.position, yyt->NullCommand.position)
copyPtrContext ((* yyNew)->NullCommand.context, yyt->NullCommand.context)
copybool ((* yyNew)->NullCommand.skip, yyt->NullCommand.skip)
copyCommandAttributes ((* yyNew)->NullCommand.attributes, yyt->NullCommand.attributes)
return;
case kContinueCommand: (* yyNew)->ContinueCommand = yyt->ContinueCommand;
copytPosition ((* yyNew)->ContinueCommand.position, yyt->ContinueCommand.position)
copyPtrContext ((* yyNew)->ContinueCommand.context, yyt->ContinueCommand.context)
copybool ((* yyNew)->ContinueCommand.skip, yyt->ContinueCommand.skip)
copyCommandAttributes ((* yyNew)->ContinueCommand.attributes, yyt->ContinueCommand.attributes)
return;
case kHaltCommand: (* yyNew)->HaltCommand = yyt->HaltCommand;
copytPosition ((* yyNew)->HaltCommand.position, yyt->HaltCommand.position)
copyPtrContext ((* yyNew)->HaltCommand.context, yyt->HaltCommand.context)
copybool ((* yyNew)->HaltCommand.skip, yyt->HaltCommand.skip)
copyCommandAttributes ((* yyNew)->HaltCommand.attributes, yyt->HaltCommand.attributes)
return;
case kInputCommand: (* yyNew)->InputCommand = yyt->InputCommand;
copytPosition ((* yyNew)->InputCommand.position, yyt->InputCommand.position)
copyPtrContext ((* yyNew)->InputCommand.context, yyt->InputCommand.context)
copybool ((* yyNew)->InputCommand.skip, yyt->InputCommand.skip)
copyCommandAttributes ((* yyNew)->InputCommand.attributes, yyt->InputCommand.attributes)
copytTree ((* yyNew)->InputCommand.LHS, yyt->InputCommand.LHS)
yyt = yyt->InputCommand.RHS;
yyNew = & (* yyNew)->InputCommand.RHS; break;
case kInputEncloseCommand: (* yyNew)->InputEncloseCommand = yyt->InputEncloseCommand;
copytPosition ((* yyNew)->InputEncloseCommand.position, yyt->InputEncloseCommand.position)
copyPtrContext ((* yyNew)->InputEncloseCommand.context, yyt->InputEncloseCommand.context)
copybool ((* yyNew)->InputEncloseCommand.skip, yyt->InputEncloseCommand.skip)
copyCommandAttributes ((* yyNew)->InputEncloseCommand.attributes, yyt->InputEncloseCommand.attributes)
copytTree ((* yyNew)->InputEncloseCommand.LvalueLists, yyt->InputEncloseCommand.LvalueLists)
yyt = yyt->InputEncloseCommand.Command;
yyNew = & (* yyNew)->InputEncloseCommand.Command; break;
case kInputEncloseBangCommand: (* yyNew)->InputEncloseBangCommand = yyt->InputEncloseBangCommand;
copytPosition ((* yyNew)->InputEncloseBangCommand.position, yyt->InputEncloseBangCommand.position)
copyPtrContext ((* yyNew)->InputEncloseBangCommand.context, yyt->InputEncloseBangCommand.context)
copybool ((* yyNew)->InputEncloseBangCommand.skip, yyt->InputEncloseBangCommand.skip)
copyCommandAttributes ((* yyNew)->InputEncloseBangCommand.attributes, yyt->InputEncloseBangCommand.attributes)
copytTree ((* yyNew)->InputEncloseBangCommand.LvalueLists, yyt->InputEncloseBangCommand.LvalueLists)
yyt = yyt->InputEncloseBangCommand.Command;
yyNew = & (* yyNew)->InputEncloseBangCommand.Command; break;
case kOutputCommand: (* yyNew)->OutputCommand = yyt->OutputCommand;
copytPosition ((* yyNew)->OutputCommand.position, yyt->OutputCommand.position)
copyPtrContext ((* yyNew)->OutputCommand.context, yyt->OutputCommand.context)
copybool ((* yyNew)->OutputCommand.skip, yyt->OutputCommand.skip)
copyCommandAttributes ((* yyNew)->OutputCommand.attributes, yyt->OutputCommand.attributes)
copytTree ((* yyNew)->OutputCommand.Lvalue, yyt->OutputCommand.Lvalue)
yyt = yyt->OutputCommand.CoercedExpr;
yyNew = & (* yyNew)->OutputCommand.CoercedExpr; break;
case kSyncCommand: (* yyNew)->SyncCommand = yyt->SyncCommand;
copytPosition ((* yyNew)->SyncCommand.position, yyt->SyncCommand.position)
copyPtrContext ((* yyNew)->SyncCommand.context, yyt->SyncCommand.context)
copybool ((* yyNew)->SyncCommand.skip, yyt->SyncCommand.skip)
copyCommandAttributes ((* yyNew)->SyncCommand.attributes, yyt->SyncCommand.attributes)
yyt = yyt->SyncCommand.Lvalue;
yyNew = & (* yyNew)->SyncCommand.Lvalue; break;
case kAssignmentCommand: (* yyNew)->AssignmentCommand = yyt->AssignmentCommand;
copytPosition ((* yyNew)->AssignmentCommand.position, yyt->AssignmentCommand.position)
copyPtrContext ((* yyNew)->AssignmentCommand.context, yyt->AssignmentCommand.context)
copybool ((* yyNew)->AssignmentCommand.skip, yyt->AssignmentCommand.skip)
copyCommandAttributes ((* yyNew)->AssignmentCommand.attributes, yyt->AssignmentCommand.attributes)
copytTree ((* yyNew)->AssignmentCommand.Lvalue, yyt->AssignmentCommand.Lvalue)
yyt = yyt->AssignmentCommand.CoercedExpr;
yyNew = & (* yyNew)->AssignmentCommand.CoercedExpr; break;
case kBlockCommand: (* yyNew)->BlockCommand = yyt->BlockCommand;
copytPosition ((* yyNew)->BlockCommand.position, yyt->BlockCommand.position)
copyPtrContext ((* yyNew)->BlockCommand.context, yyt->BlockCommand.context)
copybool ((* yyNew)->BlockCommand.skip, yyt->BlockCommand.skip)
copyCommandAttributes ((* yyNew)->BlockCommand.attributes, yyt->BlockCommand.attributes)
yyt = yyt->BlockCommand.Block;
yyNew = & (* yyNew)->BlockCommand.Block; break;
case kSequentialCommand: (* yyNew)->SequentialCommand = yyt->SequentialCommand;
copytPosition ((* yyNew)->SequentialCommand.position, yyt->SequentialCommand.position)
copyPtrContext ((* yyNew)->SequentialCommand.context, yyt->SequentialCommand.context)
copybool ((* yyNew)->SequentialCommand.skip, yyt->SequentialCommand.skip)
copyCommandAttributes ((* yyNew)->SequentialCommand.attributes, yyt->SequentialCommand.attributes)
copytTree ((* yyNew)->SequentialCommand.Left, yyt->SequentialCommand.Left)
yyt = yyt->SequentialCommand.Right;
yyNew = & (* yyNew)->SequentialCommand.Right; break;
case kParallelCommand: (* yyNew)->ParallelCommand = yyt->ParallelCommand;
copytPosition ((* yyNew)->ParallelCommand.position, yyt->ParallelCommand.position)
copyPtrContext ((* yyNew)->ParallelCommand.context, yyt->ParallelCommand.context)
copybool ((* yyNew)->ParallelCommand.skip, yyt->ParallelCommand.skip)
copyCommandAttributes ((* yyNew)->ParallelCommand.attributes, yyt->ParallelCommand.attributes)
copybool ((* yyNew)->ParallelCommand.isPermissive, yyt->ParallelCommand.isPermissive)
copytTree ((* yyNew)->ParallelCommand.Left, yyt->ParallelCommand.Left)
yyt = yyt->ParallelCommand.Right;
yyNew = & (* yyNew)->ParallelCommand.Right; break;
case kLoopCommand: (* yyNew)->LoopCommand = yyt->LoopCommand;
copytPosition ((* yyNew)->LoopCommand.position, yyt->LoopCommand.position)
copyPtrContext ((* yyNew)->LoopCommand.context, yyt->LoopCommand.context)
copybool ((* yyNew)->LoopCommand.skip, yyt->LoopCommand.skip)
copyCommandAttributes ((* yyNew)->LoopCommand.attributes, yyt->LoopCommand.attributes)
yyt = yyt->LoopCommand.Command;
yyNew = & (* yyNew)->LoopCommand.Command; break;
case kWhileGuardsCommand: (* yyNew)->WhileGuardsCommand = yyt->WhileGuardsCommand;
copytPosition ((* yyNew)->WhileGuardsCommand.position, yyt->WhileGuardsCommand.position)
copyPtrContext ((* yyNew)->WhileGuardsCommand.context, yyt->WhileGuardsCommand.context)
copybool ((* yyNew)->WhileGuardsCommand.skip, yyt->WhileGuardsCommand.skip)
copyCommandAttributes ((* yyNew)->WhileGuardsCommand.attributes, yyt->WhileGuardsCommand.attributes)
yyt = yyt->WhileGuardsCommand.Guards;
yyNew = & (* yyNew)->WhileGuardsCommand.Guards; break;
case kWhileGuardsAlsoCommand: (* yyNew)->WhileGuardsAlsoCommand = yyt->WhileGuardsAlsoCommand;
copytPosition ((* yyNew)->WhileGuardsAlsoCommand.position, yyt->WhileGuardsAlsoCommand.position)
copyPtrContext ((* yyNew)->WhileGuardsAlsoCommand.context, yyt->WhileGuardsAlsoCommand.context)
copybool ((* yyNew)->WhileGuardsAlsoCommand.skip, yyt->WhileGuardsAlsoCommand.skip)
copyCommandAttributes ((* yyNew)->WhileGuardsAlsoCommand.attributes, yyt->WhileGuardsAlsoCommand.attributes)
copytTree ((* yyNew)->WhileGuardsAlsoCommand.Guards, yyt->WhileGuardsAlsoCommand.Guards)
yyt = yyt->WhileGuardsAlsoCommand.Command;
yyNew = & (* yyNew)->WhileGuardsAlsoCommand.Command; break;
case kCommandWhileExprCommand: (* yyNew)->CommandWhileExprCommand = yyt->CommandWhileExprCommand;
copytPosition ((* yyNew)->CommandWhileExprCommand.position, yyt->CommandWhileExprCommand.position)
copyPtrContext ((* yyNew)->CommandWhileExprCommand.context, yyt->CommandWhileExprCommand.context)
copybool ((* yyNew)->CommandWhileExprCommand.skip, yyt->CommandWhileExprCommand.skip)
copyCommandAttributes ((* yyNew)->CommandWhileExprCommand.attributes, yyt->CommandWhileExprCommand.attributes)
copytTree ((* yyNew)->CommandWhileExprCommand.Command, yyt->CommandWhileExprCommand.Command)
yyt = yyt->CommandWhileExprCommand.Expr;
yyNew = & (* yyNew)->CommandWhileExprCommand.Expr; break;
case kCommandWhileGuardsCommand: (* yyNew)->CommandWhileGuardsCommand = yyt->CommandWhileGuardsCommand;
copytPosition ((* yyNew)->CommandWhileGuardsCommand.position, yyt->CommandWhileGuardsCommand.position)
copyPtrContext ((* yyNew)->CommandWhileGuardsCommand.context, yyt->CommandWhileGuardsCommand.context)
copybool ((* yyNew)->CommandWhileGuardsCommand.skip, yyt->CommandWhileGuardsCommand.skip)
copyCommandAttributes ((* yyNew)->CommandWhileGuardsCommand.attributes, yyt->CommandWhileGuardsCommand.attributes)
copytTree ((* yyNew)->CommandWhileGuardsCommand.Command, yyt->CommandWhileGuardsCommand.Command)
yyt = yyt->CommandWhileGuardsCommand.Guards;
yyNew = & (* yyNew)->CommandWhileGuardsCommand.Guards; break;
case kCommandWhileGuardsAlsoCommand: (* yyNew)->CommandWhileGuardsAlsoCommand = yyt->CommandWhileGuardsAlsoCommand;
copytPosition ((* yyNew)->CommandWhileGuardsAlsoCommand.position, yyt->CommandWhileGuardsAlsoCommand.position)
copyPtrContext ((* yyNew)->CommandWhileGuardsAlsoCommand.context, yyt->CommandWhileGuardsAlsoCommand.context)
copybool ((* yyNew)->CommandWhileGuardsAlsoCommand.skip, yyt->CommandWhileGuardsAlsoCommand.skip)
copyCommandAttributes ((* yyNew)->CommandWhileGuardsAlsoCommand.attributes, yyt->CommandWhileGuardsAlsoCommand.attributes)
copytTree ((* yyNew)->CommandWhileGuardsAlsoCommand.FirstCommand, yyt->CommandWhileGuardsAlsoCommand.FirstCommand)
copytTree ((* yyNew)->CommandWhileGuardsAlsoCommand.Guards, yyt->CommandWhileGuardsAlsoCommand.Guards)
yyt = yyt->CommandWhileGuardsAlsoCommand.AlsoCommand;
yyNew = & (* yyNew)->CommandWhileGuardsAlsoCommand.AlsoCommand; break;
case kIfCommand: (* yyNew)->IfCommand = yyt->IfCommand;
copytPosition ((* yyNew)->IfCommand.position, yyt->IfCommand.position)
copyPtrContext ((* yyNew)->IfCommand.context, yyt->IfCommand.context)
copybool ((* yyNew)->IfCommand.skip, yyt->IfCommand.skip)
copyCommandAttributes ((* yyNew)->IfCommand.attributes, yyt->IfCommand.attributes)
yyt = yyt->IfCommand.Guards;
yyNew = & (* yyNew)->IfCommand.Guards; break;
case kIfElseCommand: (* yyNew)->IfElseCommand = yyt->IfElseCommand;
copytPosition ((* yyNew)->IfElseCommand.position, yyt->IfElseCommand.position)
copyPtrContext ((* yyNew)->IfElseCommand.context, yyt->IfElseCommand.context)
copybool ((* yyNew)->IfElseCommand.skip, yyt->IfElseCommand.skip)
copyCommandAttributes ((* yyNew)->IfElseCommand.attributes, yyt->IfElseCommand.attributes)
copytTree ((* yyNew)->IfElseCommand.Guards, yyt->IfElseCommand.Guards)
yyt = yyt->IfElseCommand.Command;
yyNew = & (* yyNew)->IfElseCommand.Command; break;
case kCaseCommand: (* yyNew)->CaseCommand = yyt->CaseCommand;
copytPosition ((* yyNew)->CaseCommand.position, yyt->CaseCommand.position)
copyPtrContext ((* yyNew)->CaseCommand.context, yyt->CaseCommand.context)
copybool ((* yyNew)->CaseCommand.skip, yyt->CaseCommand.skip)
copyCommandAttributes ((* yyNew)->CaseCommand.attributes, yyt->CaseCommand.attributes)
copytTree ((* yyNew)->CaseCommand.Expr, yyt->CaseCommand.Expr)
copytTree ((* yyNew)->CaseCommand.CaseGuards, yyt->CaseCommand.CaseGuards)
yyt = yyt->CaseCommand.Command;
yyNew = & (* yyNew)->CaseCommand.Command; break;
case kForCommand: (* yyNew)->ForCommand = yyt->ForCommand;
copytPosition ((* yyNew)->ForCommand.position, yyt->ForCommand.position)
copyPtrContext ((* yyNew)->ForCommand.context, yyt->ForCommand.context)
copybool ((* yyNew)->ForCommand.skip, yyt->ForCommand.skip)
copyCommandAttributes ((* yyNew)->ForCommand.attributes, yyt->ForCommand.attributes)
copybool ((* yyNew)->ForCommand.isParallel, yyt->ForCommand.isParallel)
copybool ((* yyNew)->ForCommand.isPermissive, yyt->ForCommand.isPermissive)
copytIdent ((* yyNew)->ForCommand.ident, yyt->ForCommand.ident)
copytTree ((* yyNew)->ForCommand.Range, yyt->ForCommand.Range)
copyPtrInstance ((* yyNew)->ForCommand.iterator, yyt->ForCommand.iterator)
copyint ((* yyNew)->ForCommand.lastErrorCount, yyt->ForCommand.lastErrorCount)
copyPtrchar ((* yyNew)->ForCommand.errorContextString, yyt->ForCommand.errorContextString)
copyPtrchar ((* yyNew)->ForCommand.valueStringPtr, yyt->ForCommand.valueStringPtr)
yyt = yyt->ForCommand.Command;
yyNew = & (* yyNew)->ForCommand.Command; break;
case kProcedureCallCommonCommand: (* yyNew)->ProcedureCallCommonCommand = yyt->ProcedureCallCommonCommand;
copytPosition ((* yyNew)->ProcedureCallCommonCommand.position, yyt->ProcedureCallCommonCommand.position)
copyPtrContext ((* yyNew)->ProcedureCallCommonCommand.context, yyt->ProcedureCallCommonCommand.context)
copybool ((* yyNew)->ProcedureCallCommonCommand.skip, yyt->ProcedureCallCommonCommand.skip)
copyCommandAttributes ((* yyNew)->ProcedureCallCommonCommand.attributes, yyt->ProcedureCallCommonCommand.attributes)
copytIdent ((* yyNew)->ProcedureCallCommonCommand.ident, yyt->ProcedureCallCommonCommand.ident)
yyt = yyt->ProcedureCallCommonCommand.ProcedureParams;
yyNew = & (* yyNew)->ProcedureCallCommonCommand.ProcedureParams; break;
case kSelectCommand: (* yyNew)->SelectCommand = yyt->SelectCommand;
copytPosition ((* yyNew)->SelectCommand.position, yyt->SelectCommand.position)
copyPtrContext ((* yyNew)->SelectCommand.context, yyt->SelectCommand.context)
copybool ((* yyNew)->SelectCommand.skip, yyt->SelectCommand.skip)
copyCommandAttributes ((* yyNew)->SelectCommand.attributes, yyt->SelectCommand.attributes)
yyt = yyt->SelectCommand.ChannelGuards;
yyNew = & (* yyNew)->SelectCommand.ChannelGuards; break;
case kSelectBangCommand: (* yyNew)->SelectBangCommand = yyt->SelectBangCommand;
copytPosition ((* yyNew)->SelectBangCommand.position, yyt->SelectBangCommand.position)
copyPtrContext ((* yyNew)->SelectBangCommand.context, yyt->SelectBangCommand.context)
copybool ((* yyNew)->SelectBangCommand.skip, yyt->SelectBangCommand.skip)
copyCommandAttributes ((* yyNew)->SelectBangCommand.attributes, yyt->SelectBangCommand.attributes)
yyt = yyt->SelectBangCommand.ChannelGuards;
yyNew = & (* yyNew)->SelectBangCommand.ChannelGuards; break;
case kArbitrateCommand: (* yyNew)->ArbitrateCommand = yyt->ArbitrateCommand;
copytPosition ((* yyNew)->ArbitrateCommand.position, yyt->ArbitrateCommand.position)
copyPtrContext ((* yyNew)->ArbitrateCommand.context, yyt->ArbitrateCommand.context)
copybool ((* yyNew)->ArbitrateCommand.skip, yyt->ArbitrateCommand.skip)
copyCommandAttributes ((* yyNew)->ArbitrateCommand.attributes, yyt->ArbitrateCommand.attributes)
yyt = yyt->ArbitrateCommand.ChannelGuards;
yyNew = & (* yyNew)->ArbitrateCommand.ChannelGuards; break;
case kPrintCommand: (* yyNew)->PrintCommand = yyt->PrintCommand;
copytPosition ((* yyNew)->PrintCommand.position, yyt->PrintCommand.position)
copyPtrContext ((* yyNew)->PrintCommand.context, yyt->PrintCommand.context)
copybool ((* yyNew)->PrintCommand.skip, yyt->PrintCommand.skip)
copyCommandAttributes ((* yyNew)->PrintCommand.attributes, yyt->PrintCommand.attributes)
copytTree ((* yyNew)->PrintCommand.Expr, yyt->PrintCommand.Expr)
yyt = yyt->PrintCommand.ExprLists;
yyNew = & (* yyNew)->PrintCommand.ExprLists; break;
case kSinkCommand: (* yyNew)->SinkCommand = yyt->SinkCommand;
copytPosition ((* yyNew)->SinkCommand.position, yyt->SinkCommand.position)
copyPtrContext ((* yyNew)->SinkCommand.context, yyt->SinkCommand.context)
copybool ((* yyNew)->SinkCommand.skip, yyt->SinkCommand.skip)
copyCommandAttributes ((* yyNew)->SinkCommand.attributes, yyt->SinkCommand.attributes)
yyt = yyt->SinkCommand.Expr;
yyNew = & (* yyNew)->SinkCommand.Expr; break;
case kProcedureParams: (* yyNew)->ProcedureParams = yyt->ProcedureParams;
copytPosition ((* yyNew)->ProcedureParams.position, yyt->ProcedureParams.position)
copyPtrContext ((* yyNew)->ProcedureParams.context, yyt->ProcedureParams.context)
copybool ((* yyNew)->ProcedureParams.skip, yyt->ProcedureParams.skip)
copyPtrExprAttributesList ((* yyNew)->ProcedureParams.actualPorts, yyt->ProcedureParams.actualPorts)
copytTree ((* yyNew)->ProcedureParams.formalParams, yyt->ProcedureParams.formalParams)
copyPtrInstanceList ((* yyNew)->ProcedureParams.formalPorts, yyt->ProcedureParams.formalPorts)
copyPtrInstanceList ((* yyNew)->ProcedureParams.constantParams, yyt->ProcedureParams.constantParams)
copyPtrTypeList ((* yyNew)->ProcedureParams.typeParams, yyt->ProcedureParams.typeParams)
copyPtrIdentList ((* yyNew)->ProcedureParams.idents, yyt->ProcedureParams.idents)
copyPtrProcedure ((* yyNew)->ProcedureParams.procedureIn, yyt->ProcedureParams.procedureIn)
copyPtrProcedure ((* yyNew)->ProcedureParams.procedureOut, yyt->ProcedureParams.procedureOut)
copytTree ((* yyNew)->ProcedureParams.procedureTree, yyt->ProcedureParams.procedureTree)
copybool ((* yyNew)->ProcedureParams.noPorts, yyt->ProcedureParams.noPorts)
copyPtrContext ((* yyNew)->ProcedureParams.parameterContext, yyt->ProcedureParams.parameterContext)
return;
case kNullProcParams: (* yyNew)->NullProcParams = yyt->NullProcParams;
copytPosition ((* yyNew)->NullProcParams.position, yyt->NullProcParams.position)
copyPtrContext ((* yyNew)->NullProcParams.context, yyt->NullProcParams.context)
copybool ((* yyNew)->NullProcParams.skip, yyt->NullProcParams.skip)
copyPtrExprAttributesList ((* yyNew)->NullProcParams.actualPorts, yyt->NullProcParams.actualPorts)
copytTree ((* yyNew)->NullProcParams.formalParams, yyt->NullProcParams.formalParams)
copyPtrInstanceList ((* yyNew)->NullProcParams.formalPorts, yyt->NullProcParams.formalPorts)
copyPtrInstanceList ((* yyNew)->NullProcParams.constantParams, yyt->NullProcParams.constantParams)
copyPtrTypeList ((* yyNew)->NullProcParams.typeParams, yyt->NullProcParams.typeParams)
copyPtrIdentList ((* yyNew)->NullProcParams.idents, yyt->NullProcParams.idents)
copyPtrProcedure ((* yyNew)->NullProcParams.procedureIn, yyt->NullProcParams.procedureIn)
copyPtrProcedure ((* yyNew)->NullProcParams.procedureOut, yyt->NullProcParams.procedureOut)
copytTree ((* yyNew)->NullProcParams.procedureTree, yyt->NullProcParams.procedureTree)
copybool ((* yyNew)->NullProcParams.noPorts, yyt->NullProcParams.noPorts)
copyPtrContext ((* yyNew)->NullProcParams.parameterContext, yyt->NullProcParams.parameterContext)
return;
case kProcParam: (* yyNew)->ProcParam = yyt->ProcParam;
copytPosition ((* yyNew)->ProcParam.position, yyt->ProcParam.position)
copyPtrContext ((* yyNew)->ProcParam.context, yyt->ProcParam.context)
copybool ((* yyNew)->ProcParam.skip, yyt->ProcParam.skip)
copyPtrExprAttributesList ((* yyNew)->ProcParam.actualPorts, yyt->ProcParam.actualPorts)
copytTree ((* yyNew)->ProcParam.formalParams, yyt->ProcParam.formalParams)
copyPtrInstanceList ((* yyNew)->ProcParam.formalPorts, yyt->ProcParam.formalPorts)
copyPtrInstanceList ((* yyNew)->ProcParam.constantParams, yyt->ProcParam.constantParams)
copyPtrTypeList ((* yyNew)->ProcParam.typeParams, yyt->ProcParam.typeParams)
copyPtrIdentList ((* yyNew)->ProcParam.idents, yyt->ProcParam.idents)
copyPtrProcedure ((* yyNew)->ProcParam.procedureIn, yyt->ProcParam.procedureIn)
copyPtrProcedure ((* yyNew)->ProcParam.procedureOut, yyt->ProcParam.procedureOut)
copytTree ((* yyNew)->ProcParam.procedureTree, yyt->ProcParam.procedureTree)
copybool ((* yyNew)->ProcParam.noPorts, yyt->ProcParam.noPorts)
copyPtrContext ((* yyNew)->ProcParam.parameterContext, yyt->ProcParam.parameterContext)
yyt = yyt->ProcParam.next;
yyNew = & (* yyNew)->ProcParam.next; break;
case kExprProcParam: (* yyNew)->ExprProcParam = yyt->ExprProcParam;
copytPosition ((* yyNew)->ExprProcParam.position, yyt->ExprProcParam.position)
copyPtrContext ((* yyNew)->ExprProcParam.context, yyt->ExprProcParam.context)
copybool ((* yyNew)->ExprProcParam.skip, yyt->ExprProcParam.skip)
copyPtrExprAttributesList ((* yyNew)->ExprProcParam.actualPorts, yyt->ExprProcParam.actualPorts)
copytTree ((* yyNew)->ExprProcParam.formalParams, yyt->ExprProcParam.formalParams)
copyPtrInstanceList ((* yyNew)->ExprProcParam.formalPorts, yyt->ExprProcParam.formalPorts)
copyPtrInstanceList ((* yyNew)->ExprProcParam.constantParams, yyt->ExprProcParam.constantParams)
copyPtrTypeList ((* yyNew)->ExprProcParam.typeParams, yyt->ExprProcParam.typeParams)
copyPtrIdentList ((* yyNew)->ExprProcParam.idents, yyt->ExprProcParam.idents)
copyPtrProcedure ((* yyNew)->ExprProcParam.procedureIn, yyt->ExprProcParam.procedureIn)
copyPtrProcedure ((* yyNew)->ExprProcParam.procedureOut, yyt->ExprProcParam.procedureOut)
copytTree ((* yyNew)->ExprProcParam.procedureTree, yyt->ExprProcParam.procedureTree)
copybool ((* yyNew)->ExprProcParam.noPorts, yyt->ExprProcParam.noPorts)
copyPtrContext ((* yyNew)->ExprProcParam.parameterContext, yyt->ExprProcParam.parameterContext)
copytTree ((* yyNew)->ExprProcParam.CoercedExpr, yyt->ExprProcParam.CoercedExpr)
copyPtrType ((* yyNew)->ExprProcParam.typeIfTypeName, yyt->ExprProcParam.typeIfTypeName)
yyt = yyt->ExprProcParam.next;
yyNew = & (* yyNew)->ExprProcParam.next; break;
case kTypeProcParam: (* yyNew)->TypeProcParam = yyt->TypeProcParam;
copytPosition ((* yyNew)->TypeProcParam.position, yyt->TypeProcParam.position)
copyPtrContext ((* yyNew)->TypeProcParam.context, yyt->TypeProcParam.context)
copybool ((* yyNew)->TypeProcParam.skip, yyt->TypeProcParam.skip)
copyPtrExprAttributesList ((* yyNew)->TypeProcParam.actualPorts, yyt->TypeProcParam.actualPorts)
copytTree ((* yyNew)->TypeProcParam.formalParams, yyt->TypeProcParam.formalParams)
copyPtrInstanceList ((* yyNew)->TypeProcParam.formalPorts, yyt->TypeProcParam.formalPorts)
copyPtrInstanceList ((* yyNew)->TypeProcParam.constantParams, yyt->TypeProcParam.constantParams)
copyPtrTypeList ((* yyNew)->TypeProcParam.typeParams, yyt->TypeProcParam.typeParams)
copyPtrIdentList ((* yyNew)->TypeProcParam.idents, yyt->TypeProcParam.idents)
copyPtrProcedure ((* yyNew)->TypeProcParam.procedureIn, yyt->TypeProcParam.procedureIn)
copyPtrProcedure ((* yyNew)->TypeProcParam.procedureOut, yyt->TypeProcParam.procedureOut)
copytTree ((* yyNew)->TypeProcParam.procedureTree, yyt->TypeProcParam.procedureTree)
copybool ((* yyNew)->TypeProcParam.noPorts, yyt->TypeProcParam.noPorts)
copyPtrContext ((* yyNew)->TypeProcParam.parameterContext, yyt->TypeProcParam.parameterContext)
copytTree ((* yyNew)->TypeProcParam.AType, yyt->TypeProcParam.AType)
yyt = yyt->TypeProcParam.next;
yyNew = & (* yyNew)->TypeProcParam.next; break;
case kVarReadProcParam: (* yyNew)->VarReadProcParam = yyt->VarReadProcParam;
copytPosition ((* yyNew)->VarReadProcParam.position, yyt->VarReadProcParam.position)
copyPtrContext ((* yyNew)->VarReadProcParam.context, yyt->VarReadProcParam.context)
copybool ((* yyNew)->VarReadProcParam.skip, yyt->VarReadProcParam.skip)
copyPtrExprAttributesList ((* yyNew)->VarReadProcParam.actualPorts, yyt->VarReadProcParam.actualPorts)
copytTree ((* yyNew)->VarReadProcParam.formalParams, yyt->VarReadProcParam.formalParams)
copyPtrInstanceList ((* yyNew)->VarReadProcParam.formalPorts, yyt->VarReadProcParam.formalPorts)
copyPtrInstanceList ((* yyNew)->VarReadProcParam.constantParams, yyt->VarReadProcParam.constantParams)
copyPtrTypeList ((* yyNew)->VarReadProcParam.typeParams, yyt->VarReadProcParam.typeParams)
copyPtrIdentList ((* yyNew)->VarReadProcParam.idents, yyt->VarReadProcParam.idents)
copyPtrProcedure ((* yyNew)->VarReadProcParam.procedureIn, yyt->VarReadProcParam.procedureIn)
copyPtrProcedure ((* yyNew)->VarReadProcParam.procedureOut, yyt->VarReadProcParam.procedureOut)
copytTree ((* yyNew)->VarReadProcParam.procedureTree, yyt->VarReadProcParam.procedureTree)
copybool ((* yyNew)->VarReadProcParam.noPorts, yyt->VarReadProcParam.noPorts)
copyPtrContext ((* yyNew)->VarReadProcParam.parameterContext, yyt->VarReadProcParam.parameterContext)
copytTree ((* yyNew)->VarReadProcParam.CoercedExpr, yyt->VarReadProcParam.CoercedExpr)
yyt = yyt->VarReadProcParam.next;
yyNew = & (* yyNew)->VarReadProcParam.next; break;
case kVarWriteProcParam: (* yyNew)->VarWriteProcParam = yyt->VarWriteProcParam;
copytPosition ((* yyNew)->VarWriteProcParam.position, yyt->VarWriteProcParam.position)
copyPtrContext ((* yyNew)->VarWriteProcParam.context, yyt->VarWriteProcParam.context)
copybool ((* yyNew)->VarWriteProcParam.skip, yyt->VarWriteProcParam.skip)
copyPtrExprAttributesList ((* yyNew)->VarWriteProcParam.actualPorts, yyt->VarWriteProcParam.actualPorts)
copytTree ((* yyNew)->VarWriteProcParam.formalParams, yyt->VarWriteProcParam.formalParams)
copyPtrInstanceList ((* yyNew)->VarWriteProcParam.formalPorts, yyt->VarWriteProcParam.formalPorts)
copyPtrInstanceList ((* yyNew)->VarWriteProcParam.constantParams, yyt->VarWriteProcParam.constantParams)
copyPtrTypeList ((* yyNew)->VarWriteProcParam.typeParams, yyt->VarWriteProcParam.typeParams)
copyPtrIdentList ((* yyNew)->VarWriteProcParam.idents, yyt->VarWriteProcParam.idents)
copyPtrProcedure ((* yyNew)->VarWriteProcParam.procedureIn, yyt->VarWriteProcParam.procedureIn)
copyPtrProcedure ((* yyNew)->VarWriteProcParam.procedureOut, yyt->VarWriteProcParam.procedureOut)
copytTree ((* yyNew)->VarWriteProcParam.procedureTree, yyt->VarWriteProcParam.procedureTree)
copybool ((* yyNew)->VarWriteProcParam.noPorts, yyt->VarWriteProcParam.noPorts)
copyPtrContext ((* yyNew)->VarWriteProcParam.parameterContext, yyt->VarWriteProcParam.parameterContext)
copytTree ((* yyNew)->VarWriteProcParam.Lvalue, yyt->VarWriteProcParam.Lvalue)
yyt = yyt->VarWriteProcParam.next;
yyNew = & (* yyNew)->VarWriteProcParam.next; break;
case kBlockProcParam: (* yyNew)->BlockProcParam = yyt->BlockProcParam;
copytPosition ((* yyNew)->BlockProcParam.position, yyt->BlockProcParam.position)
copyPtrContext ((* yyNew)->BlockProcParam.context, yyt->BlockProcParam.context)
copybool ((* yyNew)->BlockProcParam.skip, yyt->BlockProcParam.skip)
copyPtrExprAttributesList ((* yyNew)->BlockProcParam.actualPorts, yyt->BlockProcParam.actualPorts)
copytTree ((* yyNew)->BlockProcParam.formalParams, yyt->BlockProcParam.formalParams)
copyPtrInstanceList ((* yyNew)->BlockProcParam.formalPorts, yyt->BlockProcParam.formalPorts)
copyPtrInstanceList ((* yyNew)->BlockProcParam.constantParams, yyt->BlockProcParam.constantParams)
copyPtrTypeList ((* yyNew)->BlockProcParam.typeParams, yyt->BlockProcParam.typeParams)
copyPtrIdentList ((* yyNew)->BlockProcParam.idents, yyt->BlockProcParam.idents)
copyPtrProcedure ((* yyNew)->BlockProcParam.procedureIn, yyt->BlockProcParam.procedureIn)
copyPtrProcedure ((* yyNew)->BlockProcParam.procedureOut, yyt->BlockProcParam.procedureOut)
copytTree ((* yyNew)->BlockProcParam.procedureTree, yyt->BlockProcParam.procedureTree)
copybool ((* yyNew)->BlockProcParam.noPorts, yyt->BlockProcParam.noPorts)
copyPtrContext ((* yyNew)->BlockProcParam.parameterContext, yyt->BlockProcParam.parameterContext)
copytTree ((* yyNew)->BlockProcParam.Block, yyt->BlockProcParam.Block)
yyt = yyt->BlockProcParam.next;
yyNew = & (* yyNew)->BlockProcParam.next; break;
case kFunctionParams: (* yyNew)->FunctionParams = yyt->FunctionParams;
copytPosition ((* yyNew)->FunctionParams.position, yyt->FunctionParams.position)
copyPtrContext ((* yyNew)->FunctionParams.context, yyt->FunctionParams.context)
copybool ((* yyNew)->FunctionParams.skip, yyt->FunctionParams.skip)
copyPtrExprAttributesList ((* yyNew)->FunctionParams.actualPorts, yyt->FunctionParams.actualPorts)
copytTree ((* yyNew)->FunctionParams.formalParams, yyt->FunctionParams.formalParams)
copyPtrInstanceList ((* yyNew)->FunctionParams.formalPorts, yyt->FunctionParams.formalPorts)
copyPtrInstanceList ((* yyNew)->FunctionParams.constantParams, yyt->FunctionParams.constantParams)
copyPtrTypeList ((* yyNew)->FunctionParams.typeParams, yyt->FunctionParams.typeParams)
copyPtrComponentParameterList ((* yyNew)->FunctionParams.params, yyt->FunctionParams.params)
copyPtrIdentList ((* yyNew)->FunctionParams.idents, yyt->FunctionParams.idents)
copyPtrProcedure ((* yyNew)->FunctionParams.functionIn, yyt->FunctionParams.functionIn)
copyPtrProcedure ((* yyNew)->FunctionParams.functionOut, yyt->FunctionParams.functionOut)
copytTree ((* yyNew)->FunctionParams.functionTree, yyt->FunctionParams.functionTree)
copyPtrContext ((* yyNew)->FunctionParams.parameterContext, yyt->FunctionParams.parameterContext)
return;
case kNullFuncParams: (* yyNew)->NullFuncParams = yyt->NullFuncParams;
copytPosition ((* yyNew)->NullFuncParams.position, yyt->NullFuncParams.position)
copyPtrContext ((* yyNew)->NullFuncParams.context, yyt->NullFuncParams.context)
copybool ((* yyNew)->NullFuncParams.skip, yyt->NullFuncParams.skip)
copyPtrExprAttributesList ((* yyNew)->NullFuncParams.actualPorts, yyt->NullFuncParams.actualPorts)
copytTree ((* yyNew)->NullFuncParams.formalParams, yyt->NullFuncParams.formalParams)
copyPtrInstanceList ((* yyNew)->NullFuncParams.formalPorts, yyt->NullFuncParams.formalPorts)
copyPtrInstanceList ((* yyNew)->NullFuncParams.constantParams, yyt->NullFuncParams.constantParams)
copyPtrTypeList ((* yyNew)->NullFuncParams.typeParams, yyt->NullFuncParams.typeParams)
copyPtrComponentParameterList ((* yyNew)->NullFuncParams.params, yyt->NullFuncParams.params)
copyPtrIdentList ((* yyNew)->NullFuncParams.idents, yyt->NullFuncParams.idents)
copyPtrProcedure ((* yyNew)->NullFuncParams.functionIn, yyt->NullFuncParams.functionIn)
copyPtrProcedure ((* yyNew)->NullFuncParams.functionOut, yyt->NullFuncParams.functionOut)
copytTree ((* yyNew)->NullFuncParams.functionTree, yyt->NullFuncParams.functionTree)
copyPtrContext ((* yyNew)->NullFuncParams.parameterContext, yyt->NullFuncParams.parameterContext)
return;
case kFuncParam: (* yyNew)->FuncParam = yyt->FuncParam;
copytPosition ((* yyNew)->FuncParam.position, yyt->FuncParam.position)
copyPtrContext ((* yyNew)->FuncParam.context, yyt->FuncParam.context)
copybool ((* yyNew)->FuncParam.skip, yyt->FuncParam.skip)
copyPtrExprAttributesList ((* yyNew)->FuncParam.actualPorts, yyt->FuncParam.actualPorts)
copytTree ((* yyNew)->FuncParam.formalParams, yyt->FuncParam.formalParams)
copyPtrInstanceList ((* yyNew)->FuncParam.formalPorts, yyt->FuncParam.formalPorts)
copyPtrInstanceList ((* yyNew)->FuncParam.constantParams, yyt->FuncParam.constantParams)
copyPtrTypeList ((* yyNew)->FuncParam.typeParams, yyt->FuncParam.typeParams)
copyPtrComponentParameterList ((* yyNew)->FuncParam.params, yyt->FuncParam.params)
copyPtrIdentList ((* yyNew)->FuncParam.idents, yyt->FuncParam.idents)
copyPtrProcedure ((* yyNew)->FuncParam.functionIn, yyt->FuncParam.functionIn)
copyPtrProcedure ((* yyNew)->FuncParam.functionOut, yyt->FuncParam.functionOut)
copytTree ((* yyNew)->FuncParam.functionTree, yyt->FuncParam.functionTree)
copyPtrContext ((* yyNew)->FuncParam.parameterContext, yyt->FuncParam.parameterContext)
yyt = yyt->FuncParam.next;
yyNew = & (* yyNew)->FuncParam.next; break;
case kExprFuncParam: (* yyNew)->ExprFuncParam = yyt->ExprFuncParam;
copytPosition ((* yyNew)->ExprFuncParam.position, yyt->ExprFuncParam.position)
copyPtrContext ((* yyNew)->ExprFuncParam.context, yyt->ExprFuncParam.context)
copybool ((* yyNew)->ExprFuncParam.skip, yyt->ExprFuncParam.skip)
copyPtrExprAttributesList ((* yyNew)->ExprFuncParam.actualPorts, yyt->ExprFuncParam.actualPorts)
copytTree ((* yyNew)->ExprFuncParam.formalParams, yyt->ExprFuncParam.formalParams)
copyPtrInstanceList ((* yyNew)->ExprFuncParam.formalPorts, yyt->ExprFuncParam.formalPorts)
copyPtrInstanceList ((* yyNew)->ExprFuncParam.constantParams, yyt->ExprFuncParam.constantParams)
copyPtrTypeList ((* yyNew)->ExprFuncParam.typeParams, yyt->ExprFuncParam.typeParams)
copyPtrComponentParameterList ((* yyNew)->ExprFuncParam.params, yyt->ExprFuncParam.params)
copyPtrIdentList ((* yyNew)->ExprFuncParam.idents, yyt->ExprFuncParam.idents)
copyPtrProcedure ((* yyNew)->ExprFuncParam.functionIn, yyt->ExprFuncParam.functionIn)
copyPtrProcedure ((* yyNew)->ExprFuncParam.functionOut, yyt->ExprFuncParam.functionOut)
copytTree ((* yyNew)->ExprFuncParam.functionTree, yyt->ExprFuncParam.functionTree)
copyPtrContext ((* yyNew)->ExprFuncParam.parameterContext, yyt->ExprFuncParam.parameterContext)
copytTree ((* yyNew)->ExprFuncParam.CoercedExpr, yyt->ExprFuncParam.CoercedExpr)
copyPtrType ((* yyNew)->ExprFuncParam.typeIfTypeName, yyt->ExprFuncParam.typeIfTypeName)
yyt = yyt->ExprFuncParam.next;
yyNew = & (* yyNew)->ExprFuncParam.next; break;
case kTypeFuncParam: (* yyNew)->TypeFuncParam = yyt->TypeFuncParam;
copytPosition ((* yyNew)->TypeFuncParam.position, yyt->TypeFuncParam.position)
copyPtrContext ((* yyNew)->TypeFuncParam.context, yyt->TypeFuncParam.context)
copybool ((* yyNew)->TypeFuncParam.skip, yyt->TypeFuncParam.skip)
copyPtrExprAttributesList ((* yyNew)->TypeFuncParam.actualPorts, yyt->TypeFuncParam.actualPorts)
copytTree ((* yyNew)->TypeFuncParam.formalParams, yyt->TypeFuncParam.formalParams)
copyPtrInstanceList ((* yyNew)->TypeFuncParam.formalPorts, yyt->TypeFuncParam.formalPorts)
copyPtrInstanceList ((* yyNew)->TypeFuncParam.constantParams, yyt->TypeFuncParam.constantParams)
copyPtrTypeList ((* yyNew)->TypeFuncParam.typeParams, yyt->TypeFuncParam.typeParams)
copyPtrComponentParameterList ((* yyNew)->TypeFuncParam.params, yyt->TypeFuncParam.params)
copyPtrIdentList ((* yyNew)->TypeFuncParam.idents, yyt->TypeFuncParam.idents)
copyPtrProcedure ((* yyNew)->TypeFuncParam.functionIn, yyt->TypeFuncParam.functionIn)
copyPtrProcedure ((* yyNew)->TypeFuncParam.functionOut, yyt->TypeFuncParam.functionOut)
copytTree ((* yyNew)->TypeFuncParam.functionTree, yyt->TypeFuncParam.functionTree)
copyPtrContext ((* yyNew)->TypeFuncParam.parameterContext, yyt->TypeFuncParam.parameterContext)
copytTree ((* yyNew)->TypeFuncParam.AType, yyt->TypeFuncParam.AType)
yyt = yyt->TypeFuncParam.next;
yyNew = & (* yyNew)->TypeFuncParam.next; break;
case kLvalue: (* yyNew)->Lvalue = yyt->Lvalue;
copytPosition ((* yyNew)->Lvalue.position, yyt->Lvalue.position)
copyPtrContext ((* yyNew)->Lvalue.context, yyt->Lvalue.context)
copybool ((* yyNew)->Lvalue.skip, yyt->Lvalue.skip)
copyExprAttributes ((* yyNew)->Lvalue.attributes, yyt->Lvalue.attributes)
copybool ((* yyNew)->Lvalue.expectingChannel, yyt->Lvalue.expectingChannel)
copybool ((* yyNew)->Lvalue.expectingEither, yyt->Lvalue.expectingEither)
copybool ((* yyNew)->Lvalue.isInput, yyt->Lvalue.isInput)
copybool ((* yyNew)->Lvalue.isPassive, yyt->Lvalue.isPassive)
copybool ((* yyNew)->Lvalue.lockPassiveChannels, yyt->Lvalue.lockPassiveChannels)
copyPtrInstance ((* yyNew)->Lvalue.instance, yyt->Lvalue.instance)
copyPtrSpanList ((* yyNew)->Lvalue.partitions, yyt->Lvalue.partitions)
copyPtrSpanListList ((* yyNew)->Lvalue.indices, yyt->Lvalue.indices)
copyPtrWire ((* yyNew)->Lvalue.indexWire, yyt->Lvalue.indexWire)
copyPtrAccess ((* yyNew)->Lvalue.access, yyt->Lvalue.access)
copyPtrType ((* yyNew)->Lvalue.expectedBaseType, yyt->Lvalue.expectedBaseType)
return;
case kIdentLvalue: (* yyNew)->IdentLvalue = yyt->IdentLvalue;
copytPosition ((* yyNew)->IdentLvalue.position, yyt->IdentLvalue.position)
copyPtrContext ((* yyNew)->IdentLvalue.context, yyt->IdentLvalue.context)
copybool ((* yyNew)->IdentLvalue.skip, yyt->IdentLvalue.skip)
copyExprAttributes ((* yyNew)->IdentLvalue.attributes, yyt->IdentLvalue.attributes)
copybool ((* yyNew)->IdentLvalue.expectingChannel, yyt->IdentLvalue.expectingChannel)
copybool ((* yyNew)->IdentLvalue.expectingEither, yyt->IdentLvalue.expectingEither)
copybool ((* yyNew)->IdentLvalue.isInput, yyt->IdentLvalue.isInput)
copybool ((* yyNew)->IdentLvalue.isPassive, yyt->IdentLvalue.isPassive)
copybool ((* yyNew)->IdentLvalue.lockPassiveChannels, yyt->IdentLvalue.lockPassiveChannels)
copyPtrInstance ((* yyNew)->IdentLvalue.instance, yyt->IdentLvalue.instance)
copyPtrSpanList ((* yyNew)->IdentLvalue.partitions, yyt->IdentLvalue.partitions)
copyPtrSpanListList ((* yyNew)->IdentLvalue.indices, yyt->IdentLvalue.indices)
copyPtrWire ((* yyNew)->IdentLvalue.indexWire, yyt->IdentLvalue.indexWire)
copyPtrAccess ((* yyNew)->IdentLvalue.access, yyt->IdentLvalue.access)
copyPtrType ((* yyNew)->IdentLvalue.expectedBaseType, yyt->IdentLvalue.expectedBaseType)
copytIdent ((* yyNew)->IdentLvalue.ident, yyt->IdentLvalue.ident)
return;
case kRecordElemLvalue: (* yyNew)->RecordElemLvalue = yyt->RecordElemLvalue;
copytPosition ((* yyNew)->RecordElemLvalue.position, yyt->RecordElemLvalue.position)
copyPtrContext ((* yyNew)->RecordElemLvalue.context, yyt->RecordElemLvalue.context)
copybool ((* yyNew)->RecordElemLvalue.skip, yyt->RecordElemLvalue.skip)
copyExprAttributes ((* yyNew)->RecordElemLvalue.attributes, yyt->RecordElemLvalue.attributes)
copybool ((* yyNew)->RecordElemLvalue.expectingChannel, yyt->RecordElemLvalue.expectingChannel)
copybool ((* yyNew)->RecordElemLvalue.expectingEither, yyt->RecordElemLvalue.expectingEither)
copybool ((* yyNew)->RecordElemLvalue.isInput, yyt->RecordElemLvalue.isInput)
copybool ((* yyNew)->RecordElemLvalue.isPassive, yyt->RecordElemLvalue.isPassive)
copybool ((* yyNew)->RecordElemLvalue.lockPassiveChannels, yyt->RecordElemLvalue.lockPassiveChannels)
copyPtrInstance ((* yyNew)->RecordElemLvalue.instance, yyt->RecordElemLvalue.instance)
copyPtrSpanList ((* yyNew)->RecordElemLvalue.partitions, yyt->RecordElemLvalue.partitions)
copyPtrSpanListList ((* yyNew)->RecordElemLvalue.indices, yyt->RecordElemLvalue.indices)
copyPtrWire ((* yyNew)->RecordElemLvalue.indexWire, yyt->RecordElemLvalue.indexWire)
copyPtrAccess ((* yyNew)->RecordElemLvalue.access, yyt->RecordElemLvalue.access)
copyPtrType ((* yyNew)->RecordElemLvalue.expectedBaseType, yyt->RecordElemLvalue.expectedBaseType)
copytIdent ((* yyNew)->RecordElemLvalue.ident, yyt->RecordElemLvalue.ident)
yyt = yyt->RecordElemLvalue.Lvalue;
yyNew = & (* yyNew)->RecordElemLvalue.Lvalue; break;
case kArrayElemLvalue: (* yyNew)->ArrayElemLvalue = yyt->ArrayElemLvalue;
copytPosition ((* yyNew)->ArrayElemLvalue.position, yyt->ArrayElemLvalue.position)
copyPtrContext ((* yyNew)->ArrayElemLvalue.context, yyt->ArrayElemLvalue.context)
copybool ((* yyNew)->ArrayElemLvalue.skip, yyt->ArrayElemLvalue.skip)
copyExprAttributes ((* yyNew)->ArrayElemLvalue.attributes, yyt->ArrayElemLvalue.attributes)
copybool ((* yyNew)->ArrayElemLvalue.expectingChannel, yyt->ArrayElemLvalue.expectingChannel)
copybool ((* yyNew)->ArrayElemLvalue.expectingEither, yyt->ArrayElemLvalue.expectingEither)
copybool ((* yyNew)->ArrayElemLvalue.isInput, yyt->ArrayElemLvalue.isInput)
copybool ((* yyNew)->ArrayElemLvalue.isPassive, yyt->ArrayElemLvalue.isPassive)
copybool ((* yyNew)->ArrayElemLvalue.lockPassiveChannels, yyt->ArrayElemLvalue.lockPassiveChannels)
copyPtrInstance ((* yyNew)->ArrayElemLvalue.instance, yyt->ArrayElemLvalue.instance)
copyPtrSpanList ((* yyNew)->ArrayElemLvalue.partitions, yyt->ArrayElemLvalue.partitions)
copyPtrSpanListList ((* yyNew)->ArrayElemLvalue.indices, yyt->ArrayElemLvalue.indices)
copyPtrWire ((* yyNew)->ArrayElemLvalue.indexWire, yyt->ArrayElemLvalue.indexWire)
copyPtrAccess ((* yyNew)->ArrayElemLvalue.access, yyt->ArrayElemLvalue.access)
copyPtrType ((* yyNew)->ArrayElemLvalue.expectedBaseType, yyt->ArrayElemLvalue.expectedBaseType)
copytTree ((* yyNew)->ArrayElemLvalue.Lvalue, yyt->ArrayElemLvalue.Lvalue)
yyt = yyt->ArrayElemLvalue.CoercedExpr;
yyNew = & (* yyNew)->ArrayElemLvalue.CoercedExpr; break;
case kArraySliceLvalue: (* yyNew)->ArraySliceLvalue = yyt->ArraySliceLvalue;
copytPosition ((* yyNew)->ArraySliceLvalue.position, yyt->ArraySliceLvalue.position)
copyPtrContext ((* yyNew)->ArraySliceLvalue.context, yyt->ArraySliceLvalue.context)
copybool ((* yyNew)->ArraySliceLvalue.skip, yyt->ArraySliceLvalue.skip)
copyExprAttributes ((* yyNew)->ArraySliceLvalue.attributes, yyt->ArraySliceLvalue.attributes)
copybool ((* yyNew)->ArraySliceLvalue.expectingChannel, yyt->ArraySliceLvalue.expectingChannel)
copybool ((* yyNew)->ArraySliceLvalue.expectingEither, yyt->ArraySliceLvalue.expectingEither)
copybool ((* yyNew)->ArraySliceLvalue.isInput, yyt->ArraySliceLvalue.isInput)
copybool ((* yyNew)->ArraySliceLvalue.isPassive, yyt->ArraySliceLvalue.isPassive)
copybool ((* yyNew)->ArraySliceLvalue.lockPassiveChannels, yyt->ArraySliceLvalue.lockPassiveChannels)
copyPtrInstance ((* yyNew)->ArraySliceLvalue.instance, yyt->ArraySliceLvalue.instance)
copyPtrSpanList ((* yyNew)->ArraySliceLvalue.partitions, yyt->ArraySliceLvalue.partitions)
copyPtrSpanListList ((* yyNew)->ArraySliceLvalue.indices, yyt->ArraySliceLvalue.indices)
copyPtrWire ((* yyNew)->ArraySliceLvalue.indexWire, yyt->ArraySliceLvalue.indexWire)
copyPtrAccess ((* yyNew)->ArraySliceLvalue.access, yyt->ArraySliceLvalue.access)
copyPtrType ((* yyNew)->ArraySliceLvalue.expectedBaseType, yyt->ArraySliceLvalue.expectedBaseType)
copytTree ((* yyNew)->ArraySliceLvalue.Lvalue, yyt->ArraySliceLvalue.Lvalue)
yyt = yyt->ArraySliceLvalue.Range;
yyNew = & (* yyNew)->ArraySliceLvalue.Range; break;
case kArrayAppendLvalue: (* yyNew)->ArrayAppendLvalue = yyt->ArrayAppendLvalue;
copytPosition ((* yyNew)->ArrayAppendLvalue.position, yyt->ArrayAppendLvalue.position)
copyPtrContext ((* yyNew)->ArrayAppendLvalue.context, yyt->ArrayAppendLvalue.context)
copybool ((* yyNew)->ArrayAppendLvalue.skip, yyt->ArrayAppendLvalue.skip)
copyExprAttributes ((* yyNew)->ArrayAppendLvalue.attributes, yyt->ArrayAppendLvalue.attributes)
copybool ((* yyNew)->ArrayAppendLvalue.expectingChannel, yyt->ArrayAppendLvalue.expectingChannel)
copybool ((* yyNew)->ArrayAppendLvalue.expectingEither, yyt->ArrayAppendLvalue.expectingEither)
copybool ((* yyNew)->ArrayAppendLvalue.isInput, yyt->ArrayAppendLvalue.isInput)
copybool ((* yyNew)->ArrayAppendLvalue.isPassive, yyt->ArrayAppendLvalue.isPassive)
copybool ((* yyNew)->ArrayAppendLvalue.lockPassiveChannels, yyt->ArrayAppendLvalue.lockPassiveChannels)
copyPtrInstance ((* yyNew)->ArrayAppendLvalue.instance, yyt->ArrayAppendLvalue.instance)
copyPtrSpanList ((* yyNew)->ArrayAppendLvalue.partitions, yyt->ArrayAppendLvalue.partitions)
copyPtrSpanListList ((* yyNew)->ArrayAppendLvalue.indices, yyt->ArrayAppendLvalue.indices)
copyPtrWire ((* yyNew)->ArrayAppendLvalue.indexWire, yyt->ArrayAppendLvalue.indexWire)
copyPtrAccess ((* yyNew)->ArrayAppendLvalue.access, yyt->ArrayAppendLvalue.access)
copyPtrType ((* yyNew)->ArrayAppendLvalue.expectedBaseType, yyt->ArrayAppendLvalue.expectedBaseType)
copytTree ((* yyNew)->ArrayAppendLvalue.Left, yyt->ArrayAppendLvalue.Left)
yyt = yyt->ArrayAppendLvalue.Right;
yyNew = & (* yyNew)->ArrayAppendLvalue.Right; break;
case kArrayConsLvalue: (* yyNew)->ArrayConsLvalue = yyt->ArrayConsLvalue;
copytPosition ((* yyNew)->ArrayConsLvalue.position, yyt->ArrayConsLvalue.position)
copyPtrContext ((* yyNew)->ArrayConsLvalue.context, yyt->ArrayConsLvalue.context)
copybool ((* yyNew)->ArrayConsLvalue.skip, yyt->ArrayConsLvalue.skip)
copyExprAttributes ((* yyNew)->ArrayConsLvalue.attributes, yyt->ArrayConsLvalue.attributes)
copybool ((* yyNew)->ArrayConsLvalue.expectingChannel, yyt->ArrayConsLvalue.expectingChannel)
copybool ((* yyNew)->ArrayConsLvalue.expectingEither, yyt->ArrayConsLvalue.expectingEither)
copybool ((* yyNew)->ArrayConsLvalue.isInput, yyt->ArrayConsLvalue.isInput)
copybool ((* yyNew)->ArrayConsLvalue.isPassive, yyt->ArrayConsLvalue.isPassive)
copybool ((* yyNew)->ArrayConsLvalue.lockPassiveChannels, yyt->ArrayConsLvalue.lockPassiveChannels)
copyPtrInstance ((* yyNew)->ArrayConsLvalue.instance, yyt->ArrayConsLvalue.instance)
copyPtrSpanList ((* yyNew)->ArrayConsLvalue.partitions, yyt->ArrayConsLvalue.partitions)
copyPtrSpanListList ((* yyNew)->ArrayConsLvalue.indices, yyt->ArrayConsLvalue.indices)
copyPtrWire ((* yyNew)->ArrayConsLvalue.indexWire, yyt->ArrayConsLvalue.indexWire)
copyPtrAccess ((* yyNew)->ArrayConsLvalue.access, yyt->ArrayConsLvalue.access)
copyPtrType ((* yyNew)->ArrayConsLvalue.expectedBaseType, yyt->ArrayConsLvalue.expectedBaseType)
yyt = yyt->ArrayConsLvalue.LvalueLists;
yyNew = & (* yyNew)->ArrayConsLvalue.LvalueLists; break;
case kAsLvalue: (* yyNew)->AsLvalue = yyt->AsLvalue;
copytPosition ((* yyNew)->AsLvalue.position, yyt->AsLvalue.position)
copyPtrContext ((* yyNew)->AsLvalue.context, yyt->AsLvalue.context)
copybool ((* yyNew)->AsLvalue.skip, yyt->AsLvalue.skip)
copyExprAttributes ((* yyNew)->AsLvalue.attributes, yyt->AsLvalue.attributes)
copybool ((* yyNew)->AsLvalue.expectingChannel, yyt->AsLvalue.expectingChannel)
copybool ((* yyNew)->AsLvalue.expectingEither, yyt->AsLvalue.expectingEither)
copybool ((* yyNew)->AsLvalue.isInput, yyt->AsLvalue.isInput)
copybool ((* yyNew)->AsLvalue.isPassive, yyt->AsLvalue.isPassive)
copybool ((* yyNew)->AsLvalue.lockPassiveChannels, yyt->AsLvalue.lockPassiveChannels)
copyPtrInstance ((* yyNew)->AsLvalue.instance, yyt->AsLvalue.instance)
copyPtrSpanList ((* yyNew)->AsLvalue.partitions, yyt->AsLvalue.partitions)
copyPtrSpanListList ((* yyNew)->AsLvalue.indices, yyt->AsLvalue.indices)
copyPtrWire ((* yyNew)->AsLvalue.indexWire, yyt->AsLvalue.indexWire)
copyPtrAccess ((* yyNew)->AsLvalue.access, yyt->AsLvalue.access)
copyPtrType ((* yyNew)->AsLvalue.expectedBaseType, yyt->AsLvalue.expectedBaseType)
copytTree ((* yyNew)->AsLvalue.Lvalue, yyt->AsLvalue.Lvalue)
yyt = yyt->AsLvalue.AType;
yyNew = & (* yyNew)->AsLvalue.AType; break;
case kBitArrayCastLvalue: (* yyNew)->BitArrayCastLvalue = yyt->BitArrayCastLvalue;
copytPosition ((* yyNew)->BitArrayCastLvalue.position, yyt->BitArrayCastLvalue.position)
copyPtrContext ((* yyNew)->BitArrayCastLvalue.context, yyt->BitArrayCastLvalue.context)
copybool ((* yyNew)->BitArrayCastLvalue.skip, yyt->BitArrayCastLvalue.skip)
copyExprAttributes ((* yyNew)->BitArrayCastLvalue.attributes, yyt->BitArrayCastLvalue.attributes)
copybool ((* yyNew)->BitArrayCastLvalue.expectingChannel, yyt->BitArrayCastLvalue.expectingChannel)
copybool ((* yyNew)->BitArrayCastLvalue.expectingEither, yyt->BitArrayCastLvalue.expectingEither)
copybool ((* yyNew)->BitArrayCastLvalue.isInput, yyt->BitArrayCastLvalue.isInput)
copybool ((* yyNew)->BitArrayCastLvalue.isPassive, yyt->BitArrayCastLvalue.isPassive)
copybool ((* yyNew)->BitArrayCastLvalue.lockPassiveChannels, yyt->BitArrayCastLvalue.lockPassiveChannels)
copyPtrInstance ((* yyNew)->BitArrayCastLvalue.instance, yyt->BitArrayCastLvalue.instance)
copyPtrSpanList ((* yyNew)->BitArrayCastLvalue.partitions, yyt->BitArrayCastLvalue.partitions)
copyPtrSpanListList ((* yyNew)->BitArrayCastLvalue.indices, yyt->BitArrayCastLvalue.indices)
copyPtrWire ((* yyNew)->BitArrayCastLvalue.indexWire, yyt->BitArrayCastLvalue.indexWire)
copyPtrAccess ((* yyNew)->BitArrayCastLvalue.access, yyt->BitArrayCastLvalue.access)
copyPtrType ((* yyNew)->BitArrayCastLvalue.expectedBaseType, yyt->BitArrayCastLvalue.expectedBaseType)
yyt = yyt->BitArrayCastLvalue.Lvalue;
yyNew = & (* yyNew)->BitArrayCastLvalue.Lvalue; break;
case kGuards: (* yyNew)->Guards = yyt->Guards;
copytPosition ((* yyNew)->Guards.position, yyt->Guards.position)
copyPtrContext ((* yyNew)->Guards.context, yyt->Guards.context)
copybool ((* yyNew)->Guards.skip, yyt->Guards.skip)
copyPtrExprAttributesList ((* yyNew)->Guards.exprAttributes, yyt->Guards.exprAttributes)
copyPtrCommandAttributesList ((* yyNew)->Guards.commandAttributes, yyt->Guards.commandAttributes)
return;
case kNullGuards: (* yyNew)->NullGuards = yyt->NullGuards;
copytPosition ((* yyNew)->NullGuards.position, yyt->NullGuards.position)
copyPtrContext ((* yyNew)->NullGuards.context, yyt->NullGuards.context)
copybool ((* yyNew)->NullGuards.skip, yyt->NullGuards.skip)
copyPtrExprAttributesList ((* yyNew)->NullGuards.exprAttributes, yyt->NullGuards.exprAttributes)
copyPtrCommandAttributesList ((* yyNew)->NullGuards.commandAttributes, yyt->NullGuards.commandAttributes)
return;
case kGuard: (* yyNew)->Guard = yyt->Guard;
copytPosition ((* yyNew)->Guard.position, yyt->Guard.position)
copyPtrContext ((* yyNew)->Guard.context, yyt->Guard.context)
copybool ((* yyNew)->Guard.skip, yyt->Guard.skip)
copyPtrExprAttributesList ((* yyNew)->Guard.exprAttributes, yyt->Guard.exprAttributes)
copyPtrCommandAttributesList ((* yyNew)->Guard.commandAttributes, yyt->Guard.commandAttributes)
copytTree ((* yyNew)->Guard.Expr, yyt->Guard.Expr)
copytTree ((* yyNew)->Guard.Command, yyt->Guard.Command)
yyt = yyt->Guard.next;
yyNew = & (* yyNew)->Guard.next; break;
case kPortGuards: (* yyNew)->PortGuards = yyt->PortGuards;
copytPosition ((* yyNew)->PortGuards.position, yyt->PortGuards.position)
copyPtrContext ((* yyNew)->PortGuards.context, yyt->PortGuards.context)
copybool ((* yyNew)->PortGuards.skip, yyt->PortGuards.skip)
copyProcedureArgsType ((* yyNew)->PortGuards.portsType, yyt->PortGuards.portsType)
copyPtrInstanceList ((* yyNew)->PortGuards.ports, yyt->PortGuards.ports)
copyunsigned ((* yyNew)->PortGuards.portCount, yyt->PortGuards.portCount)
copyPtrInstanceList ((* yyNew)->PortGuards.extraPortInstances, yyt->PortGuards.extraPortInstances)
copybool ((* yyNew)->PortGuards.hasParameters, yyt->PortGuards.hasParameters)
return;
case kNullPortGuards: (* yyNew)->NullPortGuards = yyt->NullPortGuards;
copytPosition ((* yyNew)->NullPortGuards.position, yyt->NullPortGuards.position)
copyPtrContext ((* yyNew)->NullPortGuards.context, yyt->NullPortGuards.context)
copybool ((* yyNew)->NullPortGuards.skip, yyt->NullPortGuards.skip)
copyProcedureArgsType ((* yyNew)->NullPortGuards.portsType, yyt->NullPortGuards.portsType)
copyPtrInstanceList ((* yyNew)->NullPortGuards.ports, yyt->NullPortGuards.ports)
copyunsigned ((* yyNew)->NullPortGuards.portCount, yyt->NullPortGuards.portCount)
copyPtrInstanceList ((* yyNew)->NullPortGuards.extraPortInstances, yyt->NullPortGuards.extraPortInstances)
copybool ((* yyNew)->NullPortGuards.hasParameters, yyt->NullPortGuards.hasParameters)
return;
case kPortGuard: (* yyNew)->PortGuard = yyt->PortGuard;
copytPosition ((* yyNew)->PortGuard.position, yyt->PortGuard.position)
copyPtrContext ((* yyNew)->PortGuard.context, yyt->PortGuard.context)
copybool ((* yyNew)->PortGuard.skip, yyt->PortGuard.skip)
copyProcedureArgsType ((* yyNew)->PortGuard.portsType, yyt->PortGuard.portsType)
copyPtrInstanceList ((* yyNew)->PortGuard.ports, yyt->PortGuard.ports)
copyunsigned ((* yyNew)->PortGuard.portCount, yyt->PortGuard.portCount)
copyPtrInstanceList ((* yyNew)->PortGuard.extraPortInstances, yyt->PortGuard.extraPortInstances)
copybool ((* yyNew)->PortGuard.hasParameters, yyt->PortGuard.hasParameters)
copytTree ((* yyNew)->PortGuard.Expr, yyt->PortGuard.Expr)
copytTree ((* yyNew)->PortGuard.FormalPorts, yyt->PortGuard.FormalPorts)
yyt = yyt->PortGuard.next;
yyNew = & (* yyNew)->PortGuard.next; break;
case kDeclGuards: (* yyNew)->DeclGuards = yyt->DeclGuards;
copytPosition ((* yyNew)->DeclGuards.position, yyt->DeclGuards.position)
copyScope ((* yyNew)->DeclGuards.scope, yyt->DeclGuards.scope)
copyPtrContext ((* yyNew)->DeclGuards.contextIn, yyt->DeclGuards.contextIn)
copyPtrContext ((* yyNew)->DeclGuards.contextOut, yyt->DeclGuards.contextOut)
copybool ((* yyNew)->DeclGuards.skip, yyt->DeclGuards.skip)
copybool ((* yyNew)->DeclGuards.foundTrueGuard, yyt->DeclGuards.foundTrueGuard)
return;
case kNullDeclGuards: (* yyNew)->NullDeclGuards = yyt->NullDeclGuards;
copytPosition ((* yyNew)->NullDeclGuards.position, yyt->NullDeclGuards.position)
copyScope ((* yyNew)->NullDeclGuards.scope, yyt->NullDeclGuards.scope)
copyPtrContext ((* yyNew)->NullDeclGuards.contextIn, yyt->NullDeclGuards.contextIn)
copyPtrContext ((* yyNew)->NullDeclGuards.contextOut, yyt->NullDeclGuards.contextOut)
copybool ((* yyNew)->NullDeclGuards.skip, yyt->NullDeclGuards.skip)
copybool ((* yyNew)->NullDeclGuards.foundTrueGuard, yyt->NullDeclGuards.foundTrueGuard)
return;
case kDeclGuard: (* yyNew)->DeclGuard = yyt->DeclGuard;
copytPosition ((* yyNew)->DeclGuard.position, yyt->DeclGuard.position)
copyScope ((* yyNew)->DeclGuard.scope, yyt->DeclGuard.scope)
copyPtrContext ((* yyNew)->DeclGuard.contextIn, yyt->DeclGuard.contextIn)
copyPtrContext ((* yyNew)->DeclGuard.contextOut, yyt->DeclGuard.contextOut)
copybool ((* yyNew)->DeclGuard.skip, yyt->DeclGuard.skip)
copybool ((* yyNew)->DeclGuard.foundTrueGuard, yyt->DeclGuard.foundTrueGuard)
copytTree ((* yyNew)->DeclGuard.Expr, yyt->DeclGuard.Expr)
copytTree ((* yyNew)->DeclGuard.Decls, yyt->DeclGuard.Decls)
yyt = yyt->DeclGuard.next;
yyNew = & (* yyNew)->DeclGuard.next; break;
case kCaseGuards: (* yyNew)->CaseGuards = yyt->CaseGuards;
copytPosition ((* yyNew)->CaseGuards.position, yyt->CaseGuards.position)
copyPtrContext ((* yyNew)->CaseGuards.context, yyt->CaseGuards.context)
copybool ((* yyNew)->CaseGuards.skip, yyt->CaseGuards.skip)
copyPtrImplicantListList ((* yyNew)->CaseGuards.implicantss, yyt->CaseGuards.implicantss)
copyPtrImplicantList ((* yyNew)->CaseGuards.complementImplicants, yyt->CaseGuards.complementImplicants)
copyPtrCommandAttributesList ((* yyNew)->CaseGuards.commandAttributes, yyt->CaseGuards.commandAttributes)
copyPtrType ((* yyNew)->CaseGuards.switchType, yyt->CaseGuards.switchType)
copyPtrMP_INT ((* yyNew)->CaseGuards.switchValue, yyt->CaseGuards.switchValue)
copySignedBits ((* yyNew)->CaseGuards.maxRange, yyt->CaseGuards.maxRange)
copyPtrMP_INT ((* yyNew)->CaseGuards.implicantMask, yyt->CaseGuards.implicantMask)
copybool ((* yyNew)->CaseGuards.foundTrueGuard, yyt->CaseGuards.foundTrueGuard)
return;
case kNullCaseGuards: (* yyNew)->NullCaseGuards = yyt->NullCaseGuards;
copytPosition ((* yyNew)->NullCaseGuards.position, yyt->NullCaseGuards.position)
copyPtrContext ((* yyNew)->NullCaseGuards.context, yyt->NullCaseGuards.context)
copybool ((* yyNew)->NullCaseGuards.skip, yyt->NullCaseGuards.skip)
copyPtrImplicantListList ((* yyNew)->NullCaseGuards.implicantss, yyt->NullCaseGuards.implicantss)
copyPtrImplicantList ((* yyNew)->NullCaseGuards.complementImplicants, yyt->NullCaseGuards.complementImplicants)
copyPtrCommandAttributesList ((* yyNew)->NullCaseGuards.commandAttributes, yyt->NullCaseGuards.commandAttributes)
copyPtrType ((* yyNew)->NullCaseGuards.switchType, yyt->NullCaseGuards.switchType)
copyPtrMP_INT ((* yyNew)->NullCaseGuards.switchValue, yyt->NullCaseGuards.switchValue)
copySignedBits ((* yyNew)->NullCaseGuards.maxRange, yyt->NullCaseGuards.maxRange)
copyPtrMP_INT ((* yyNew)->NullCaseGuards.implicantMask, yyt->NullCaseGuards.implicantMask)
copybool ((* yyNew)->NullCaseGuards.foundTrueGuard, yyt->NullCaseGuards.foundTrueGuard)
return;
case kCaseGuard: (* yyNew)->CaseGuard = yyt->CaseGuard;
copytPosition ((* yyNew)->CaseGuard.position, yyt->CaseGuard.position)
copyPtrContext ((* yyNew)->CaseGuard.context, yyt->CaseGuard.context)
copybool ((* yyNew)->CaseGuard.skip, yyt->CaseGuard.skip)
copyPtrImplicantListList ((* yyNew)->CaseGuard.implicantss, yyt->CaseGuard.implicantss)
copyPtrImplicantList ((* yyNew)->CaseGuard.complementImplicants, yyt->CaseGuard.complementImplicants)
copyPtrCommandAttributesList ((* yyNew)->CaseGuard.commandAttributes, yyt->CaseGuard.commandAttributes)
copyPtrType ((* yyNew)->CaseGuard.switchType, yyt->CaseGuard.switchType)
copyPtrMP_INT ((* yyNew)->CaseGuard.switchValue, yyt->CaseGuard.switchValue)
copySignedBits ((* yyNew)->CaseGuard.maxRange, yyt->CaseGuard.maxRange)
copyPtrMP_INT ((* yyNew)->CaseGuard.implicantMask, yyt->CaseGuard.implicantMask)
copybool ((* yyNew)->CaseGuard.foundTrueGuard, yyt->CaseGuard.foundTrueGuard)
yyt = yyt->CaseGuard.next;
yyNew = & (* yyNew)->CaseGuard.next; break;
case kCaseMatchGuard: (* yyNew)->CaseMatchGuard = yyt->CaseMatchGuard;
copytPosition ((* yyNew)->CaseMatchGuard.position, yyt->CaseMatchGuard.position)
copyPtrContext ((* yyNew)->CaseMatchGuard.context, yyt->CaseMatchGuard.context)
copybool ((* yyNew)->CaseMatchGuard.skip, yyt->CaseMatchGuard.skip)
copyPtrImplicantListList ((* yyNew)->CaseMatchGuard.implicantss, yyt->CaseMatchGuard.implicantss)
copyPtrImplicantList ((* yyNew)->CaseMatchGuard.complementImplicants, yyt->CaseMatchGuard.complementImplicants)
copyPtrCommandAttributesList ((* yyNew)->CaseMatchGuard.commandAttributes, yyt->CaseMatchGuard.commandAttributes)
copyPtrType ((* yyNew)->CaseMatchGuard.switchType, yyt->CaseMatchGuard.switchType)
copyPtrMP_INT ((* yyNew)->CaseMatchGuard.switchValue, yyt->CaseMatchGuard.switchValue)
copySignedBits ((* yyNew)->CaseMatchGuard.maxRange, yyt->CaseMatchGuard.maxRange)
copyPtrMP_INT ((* yyNew)->CaseMatchGuard.implicantMask, yyt->CaseMatchGuard.implicantMask)
copybool ((* yyNew)->CaseMatchGuard.foundTrueGuard, yyt->CaseMatchGuard.foundTrueGuard)
copytTree ((* yyNew)->CaseMatchGuard.CaseMatchs, yyt->CaseMatchGuard.CaseMatchs)
copytTree ((* yyNew)->CaseMatchGuard.Command, yyt->CaseMatchGuard.Command)
yyt = yyt->CaseMatchGuard.next;
yyNew = & (* yyNew)->CaseMatchGuard.next; break;
case kForCaseGuard: (* yyNew)->ForCaseGuard = yyt->ForCaseGuard;
copytPosition ((* yyNew)->ForCaseGuard.position, yyt->ForCaseGuard.position)
copyPtrContext ((* yyNew)->ForCaseGuard.context, yyt->ForCaseGuard.context)
copybool ((* yyNew)->ForCaseGuard.skip, yyt->ForCaseGuard.skip)
copyPtrImplicantListList ((* yyNew)->ForCaseGuard.implicantss, yyt->ForCaseGuard.implicantss)
copyPtrImplicantList ((* yyNew)->ForCaseGuard.complementImplicants, yyt->ForCaseGuard.complementImplicants)
copyPtrCommandAttributesList ((* yyNew)->ForCaseGuard.commandAttributes, yyt->ForCaseGuard.commandAttributes)
copyPtrType ((* yyNew)->ForCaseGuard.switchType, yyt->ForCaseGuard.switchType)
copyPtrMP_INT ((* yyNew)->ForCaseGuard.switchValue, yyt->ForCaseGuard.switchValue)
copySignedBits ((* yyNew)->ForCaseGuard.maxRange, yyt->ForCaseGuard.maxRange)
copyPtrMP_INT ((* yyNew)->ForCaseGuard.implicantMask, yyt->ForCaseGuard.implicantMask)
copybool ((* yyNew)->ForCaseGuard.foundTrueGuard, yyt->ForCaseGuard.foundTrueGuard)
copytIdent ((* yyNew)->ForCaseGuard.ident, yyt->ForCaseGuard.ident)
copytTree ((* yyNew)->ForCaseGuard.CaseMatchs, yyt->ForCaseGuard.CaseMatchs)
copytTree ((* yyNew)->ForCaseGuard.Command, yyt->ForCaseGuard.Command)
copyPtrInstance ((* yyNew)->ForCaseGuard.iterator, yyt->ForCaseGuard.iterator)
copyPtrMP_INT ((* yyNew)->ForCaseGuard.countValue, yyt->ForCaseGuard.countValue)
copyint ((* yyNew)->ForCaseGuard.lastErrorCount, yyt->ForCaseGuard.lastErrorCount)
copyPtrImplicantList ((* yyNew)->ForCaseGuard.implicants, yyt->ForCaseGuard.implicants)
copyPtrchar ((* yyNew)->ForCaseGuard.errorContextString, yyt->ForCaseGuard.errorContextString)
copyPtrchar ((* yyNew)->ForCaseGuard.valueStringPtr, yyt->ForCaseGuard.valueStringPtr)
yyt = yyt->ForCaseGuard.next;
yyNew = & (* yyNew)->ForCaseGuard.next; break;
case kChannelGuards: (* yyNew)->ChannelGuards = yyt->ChannelGuards;
copytPosition ((* yyNew)->ChannelGuards.position, yyt->ChannelGuards.position)
copyPtrContext ((* yyNew)->ChannelGuards.context, yyt->ChannelGuards.context)
copybool ((* yyNew)->ChannelGuards.skip, yyt->ChannelGuards.skip)
copyPtrExprAttributesList ((* yyNew)->ChannelGuards.guardAttributes, yyt->ChannelGuards.guardAttributes)
copyPtrCommandAttributesList ((* yyNew)->ChannelGuards.commandAttributes, yyt->ChannelGuards.commandAttributes)
return;
case kNullChannelGuards: (* yyNew)->NullChannelGuards = yyt->NullChannelGuards;
copytPosition ((* yyNew)->NullChannelGuards.position, yyt->NullChannelGuards.position)
copyPtrContext ((* yyNew)->NullChannelGuards.context, yyt->NullChannelGuards.context)
copybool ((* yyNew)->NullChannelGuards.skip, yyt->NullChannelGuards.skip)
copyPtrExprAttributesList ((* yyNew)->NullChannelGuards.guardAttributes, yyt->NullChannelGuards.guardAttributes)
copyPtrCommandAttributesList ((* yyNew)->NullChannelGuards.commandAttributes, yyt->NullChannelGuards.commandAttributes)
return;
case kChannelGuard: (* yyNew)->ChannelGuard = yyt->ChannelGuard;
copytPosition ((* yyNew)->ChannelGuard.position, yyt->ChannelGuard.position)
copyPtrContext ((* yyNew)->ChannelGuard.context, yyt->ChannelGuard.context)
copybool ((* yyNew)->ChannelGuard.skip, yyt->ChannelGuard.skip)
copyPtrExprAttributesList ((* yyNew)->ChannelGuard.guardAttributes, yyt->ChannelGuard.guardAttributes)
copyPtrCommandAttributesList ((* yyNew)->ChannelGuard.commandAttributes, yyt->ChannelGuard.commandAttributes)
copytTree ((* yyNew)->ChannelGuard.LvalueLists, yyt->ChannelGuard.LvalueLists)
copytTree ((* yyNew)->ChannelGuard.Command, yyt->ChannelGuard.Command)
yyt = yyt->ChannelGuard.next;
yyNew = & (* yyNew)->ChannelGuard.next; break;
case kIdents: (* yyNew)->Idents = yyt->Idents;
copytPosition ((* yyNew)->Idents.position, yyt->Idents.position)
copyPtrIdentList ((* yyNew)->Idents.idents, yyt->Idents.idents)
return;
case kNullIdents: (* yyNew)->NullIdents = yyt->NullIdents;
copytPosition ((* yyNew)->NullIdents.position, yyt->NullIdents.position)
copyPtrIdentList ((* yyNew)->NullIdents.idents, yyt->NullIdents.idents)
return;
case kIdent: (* yyNew)->Ident = yyt->Ident;
copytPosition ((* yyNew)->Ident.position, yyt->Ident.position)
copyPtrIdentList ((* yyNew)->Ident.idents, yyt->Ident.idents)
copytIdent ((* yyNew)->Ident.ident, yyt->Ident.ident)
yyt = yyt->Ident.next;
yyNew = & (* yyNew)->Ident.next; break;
case kExprLists: (* yyNew)->ExprLists = yyt->ExprLists;
copytPosition ((* yyNew)->ExprLists.position, yyt->ExprLists.position)
copyPtrContext ((* yyNew)->ExprLists.context, yyt->ExprLists.context)
copybool ((* yyNew)->ExprLists.skip, yyt->ExprLists.skip)
copyPtrType ((* yyNew)->ExprLists.expectedType, yyt->ExprLists.expectedType)
copyPtrInstanceList ((* yyNew)->ExprLists.elements, yyt->ExprLists.elements)
copyPtrExprAttributesList ((* yyNew)->ExprLists.attributes, yyt->ExprLists.attributes)
copybool ((* yyNew)->ExprLists.expectConstants, yyt->ExprLists.expectConstants)
return;
case kNullExprLists: (* yyNew)->NullExprLists = yyt->NullExprLists;
copytPosition ((* yyNew)->NullExprLists.position, yyt->NullExprLists.position)
copyPtrContext ((* yyNew)->NullExprLists.context, yyt->NullExprLists.context)
copybool ((* yyNew)->NullExprLists.skip, yyt->NullExprLists.skip)
copyPtrType ((* yyNew)->NullExprLists.expectedType, yyt->NullExprLists.expectedType)
copyPtrInstanceList ((* yyNew)->NullExprLists.elements, yyt->NullExprLists.elements)
copyPtrExprAttributesList ((* yyNew)->NullExprLists.attributes, yyt->NullExprLists.attributes)
copybool ((* yyNew)->NullExprLists.expectConstants, yyt->NullExprLists.expectConstants)
return;
case kExprList: (* yyNew)->ExprList = yyt->ExprList;
copytPosition ((* yyNew)->ExprList.position, yyt->ExprList.position)
copyPtrContext ((* yyNew)->ExprList.context, yyt->ExprList.context)
copybool ((* yyNew)->ExprList.skip, yyt->ExprList.skip)
copyPtrType ((* yyNew)->ExprList.expectedType, yyt->ExprList.expectedType)
copyPtrInstanceList ((* yyNew)->ExprList.elements, yyt->ExprList.elements)
copyPtrExprAttributesList ((* yyNew)->ExprList.attributes, yyt->ExprList.attributes)
copybool ((* yyNew)->ExprList.expectConstants, yyt->ExprList.expectConstants)
copytTree ((* yyNew)->ExprList.CoercedExpr, yyt->ExprList.CoercedExpr)
yyt = yyt->ExprList.next;
yyNew = & (* yyNew)->ExprList.next; break;
case kLvalueLists: (* yyNew)->LvalueLists = yyt->LvalueLists;
copytPosition ((* yyNew)->LvalueLists.position, yyt->LvalueLists.position)
copyPtrContext ((* yyNew)->LvalueLists.context, yyt->LvalueLists.context)
copybool ((* yyNew)->LvalueLists.skip, yyt->LvalueLists.skip)
copyPtrExprAttributesList ((* yyNew)->LvalueLists.attributes, yyt->LvalueLists.attributes)
copyPtrType ((* yyNew)->LvalueLists.expectedType, yyt->LvalueLists.expectedType)
copyPtrInstanceList ((* yyNew)->LvalueLists.elements, yyt->LvalueLists.elements)
copyPtrIdentList ((* yyNew)->LvalueLists.idents, yyt->LvalueLists.idents)
copybool ((* yyNew)->LvalueLists.isInput, yyt->LvalueLists.isInput)
copybool ((* yyNew)->LvalueLists.isPassive, yyt->LvalueLists.isPassive)
copybool ((* yyNew)->LvalueLists.lockPassiveChannels, yyt->LvalueLists.lockPassiveChannels)
copyint ((* yyNew)->LvalueLists.isProcedurePorts, yyt->LvalueLists.isProcedurePorts)
copyint ((* yyNew)->LvalueLists.allowArrayedChannels, yyt->LvalueLists.allowArrayedChannels)
return;
case kNullLvalueLists: (* yyNew)->NullLvalueLists = yyt->NullLvalueLists;
copytPosition ((* yyNew)->NullLvalueLists.position, yyt->NullLvalueLists.position)
copyPtrContext ((* yyNew)->NullLvalueLists.context, yyt->NullLvalueLists.context)
copybool ((* yyNew)->NullLvalueLists.skip, yyt->NullLvalueLists.skip)
copyPtrExprAttributesList ((* yyNew)->NullLvalueLists.attributes, yyt->NullLvalueLists.attributes)
copyPtrType ((* yyNew)->NullLvalueLists.expectedType, yyt->NullLvalueLists.expectedType)
copyPtrInstanceList ((* yyNew)->NullLvalueLists.elements, yyt->NullLvalueLists.elements)
copyPtrIdentList ((* yyNew)->NullLvalueLists.idents, yyt->NullLvalueLists.idents)
copybool ((* yyNew)->NullLvalueLists.isInput, yyt->NullLvalueLists.isInput)
copybool ((* yyNew)->NullLvalueLists.isPassive, yyt->NullLvalueLists.isPassive)
copybool ((* yyNew)->NullLvalueLists.lockPassiveChannels, yyt->NullLvalueLists.lockPassiveChannels)
copyint ((* yyNew)->NullLvalueLists.isProcedurePorts, yyt->NullLvalueLists.isProcedurePorts)
copyint ((* yyNew)->NullLvalueLists.allowArrayedChannels, yyt->NullLvalueLists.allowArrayedChannels)
return;
case kLvalueList: (* yyNew)->LvalueList = yyt->LvalueList;
copytPosition ((* yyNew)->LvalueList.position, yyt->LvalueList.position)
copyPtrContext ((* yyNew)->LvalueList.context, yyt->LvalueList.context)
copybool ((* yyNew)->LvalueList.skip, yyt->LvalueList.skip)
copyPtrExprAttributesList ((* yyNew)->LvalueList.attributes, yyt->LvalueList.attributes)
copyPtrType ((* yyNew)->LvalueList.expectedType, yyt->LvalueList.expectedType)
copyPtrInstanceList ((* yyNew)->LvalueList.elements, yyt->LvalueList.elements)
copyPtrIdentList ((* yyNew)->LvalueList.idents, yyt->LvalueList.idents)
copybool ((* yyNew)->LvalueList.isInput, yyt->LvalueList.isInput)
copybool ((* yyNew)->LvalueList.isPassive, yyt->LvalueList.isPassive)
copybool ((* yyNew)->LvalueList.lockPassiveChannels, yyt->LvalueList.lockPassiveChannels)
copyint ((* yyNew)->LvalueList.isProcedurePorts, yyt->LvalueList.isProcedurePorts)
copyint ((* yyNew)->LvalueList.allowArrayedChannels, yyt->LvalueList.allowArrayedChannels)
yyt = yyt->LvalueList.next;
yyNew = & (* yyNew)->LvalueList.next; break;
case kLinkedBlock: (* yyNew)->LinkedBlock = yyt->LinkedBlock;
copytPosition ((* yyNew)->LinkedBlock.position, yyt->LinkedBlock.position)
copyPtrContext ((* yyNew)->LinkedBlock.context, yyt->LinkedBlock.context)
copybool ((* yyNew)->LinkedBlock.skip, yyt->LinkedBlock.skip)
copyPtrExprAttributesList ((* yyNew)->LinkedBlock.attributes, yyt->LinkedBlock.attributes)
copyPtrType ((* yyNew)->LinkedBlock.expectedType, yyt->LinkedBlock.expectedType)
copyPtrInstanceList ((* yyNew)->LinkedBlock.elements, yyt->LinkedBlock.elements)
copyPtrIdentList ((* yyNew)->LinkedBlock.idents, yyt->LinkedBlock.idents)
copybool ((* yyNew)->LinkedBlock.isInput, yyt->LinkedBlock.isInput)
copybool ((* yyNew)->LinkedBlock.isPassive, yyt->LinkedBlock.isPassive)
copybool ((* yyNew)->LinkedBlock.lockPassiveChannels, yyt->LinkedBlock.lockPassiveChannels)
copyint ((* yyNew)->LinkedBlock.isProcedurePorts, yyt->LinkedBlock.isProcedurePorts)
copyint ((* yyNew)->LinkedBlock.allowArrayedChannels, yyt->LinkedBlock.allowArrayedChannels)
copytTree ((* yyNew)->LinkedBlock.Block, yyt->LinkedBlock.Block)
yyt = yyt->LinkedBlock.next;
yyNew = & (* yyNew)->LinkedBlock.next; break;
case kLinkedChannel: (* yyNew)->LinkedChannel = yyt->LinkedChannel;
copytPosition ((* yyNew)->LinkedChannel.position, yyt->LinkedChannel.position)
copyPtrContext ((* yyNew)->LinkedChannel.context, yyt->LinkedChannel.context)
copybool ((* yyNew)->LinkedChannel.skip, yyt->LinkedChannel.skip)
copyPtrExprAttributesList ((* yyNew)->LinkedChannel.attributes, yyt->LinkedChannel.attributes)
copyPtrType ((* yyNew)->LinkedChannel.expectedType, yyt->LinkedChannel.expectedType)
copyPtrInstanceList ((* yyNew)->LinkedChannel.elements, yyt->LinkedChannel.elements)
copyPtrIdentList ((* yyNew)->LinkedChannel.idents, yyt->LinkedChannel.idents)
copybool ((* yyNew)->LinkedChannel.isInput, yyt->LinkedChannel.isInput)
copybool ((* yyNew)->LinkedChannel.isPassive, yyt->LinkedChannel.isPassive)
copybool ((* yyNew)->LinkedChannel.lockPassiveChannels, yyt->LinkedChannel.lockPassiveChannels)
copyint ((* yyNew)->LinkedChannel.isProcedurePorts, yyt->LinkedChannel.isProcedurePorts)
copyint ((* yyNew)->LinkedChannel.allowArrayedChannels, yyt->LinkedChannel.allowArrayedChannels)
copytIdent ((* yyNew)->LinkedChannel.ident, yyt->LinkedChannel.ident)
copytTree ((* yyNew)->LinkedChannel.Lvalue, yyt->LinkedChannel.Lvalue)
yyt = yyt->LinkedChannel.next;
yyNew = & (* yyNew)->LinkedChannel.next; break;
case kCaseMatchs: (* yyNew)->CaseMatchs = yyt->CaseMatchs;
copytPosition ((* yyNew)->CaseMatchs.position, yyt->CaseMatchs.position)
copyPtrContext ((* yyNew)->CaseMatchs.context, yyt->CaseMatchs.context)
copybool ((* yyNew)->CaseMatchs.skip, yyt->CaseMatchs.skip)
copyPtrImplicantList ((* yyNew)->CaseMatchs.implicants, yyt->CaseMatchs.implicants)
copyPtrType ((* yyNew)->CaseMatchs.expectedType, yyt->CaseMatchs.expectedType)
copybool ((* yyNew)->CaseMatchs.allowOverlappingMatches, yyt->CaseMatchs.allowOverlappingMatches)
copybool ((* yyNew)->CaseMatchs.isSpan, yyt->CaseMatchs.isSpan)
return;
case kNullCaseMatchs: (* yyNew)->NullCaseMatchs = yyt->NullCaseMatchs;
copytPosition ((* yyNew)->NullCaseMatchs.position, yyt->NullCaseMatchs.position)
copyPtrContext ((* yyNew)->NullCaseMatchs.context, yyt->NullCaseMatchs.context)
copybool ((* yyNew)->NullCaseMatchs.skip, yyt->NullCaseMatchs.skip)
copyPtrImplicantList ((* yyNew)->NullCaseMatchs.implicants, yyt->NullCaseMatchs.implicants)
copyPtrType ((* yyNew)->NullCaseMatchs.expectedType, yyt->NullCaseMatchs.expectedType)
copybool ((* yyNew)->NullCaseMatchs.allowOverlappingMatches, yyt->NullCaseMatchs.allowOverlappingMatches)
copybool ((* yyNew)->NullCaseMatchs.isSpan, yyt->NullCaseMatchs.isSpan)
return;
case kCaseMatch: (* yyNew)->CaseMatch = yyt->CaseMatch;
copytPosition ((* yyNew)->CaseMatch.position, yyt->CaseMatch.position)
copyPtrContext ((* yyNew)->CaseMatch.context, yyt->CaseMatch.context)
copybool ((* yyNew)->CaseMatch.skip, yyt->CaseMatch.skip)
copyPtrImplicantList ((* yyNew)->CaseMatch.implicants, yyt->CaseMatch.implicants)
copyPtrType ((* yyNew)->CaseMatch.expectedType, yyt->CaseMatch.expectedType)
copybool ((* yyNew)->CaseMatch.allowOverlappingMatches, yyt->CaseMatch.allowOverlappingMatches)
copybool ((* yyNew)->CaseMatch.isSpan, yyt->CaseMatch.isSpan)
yyt = yyt->CaseMatch.next;
yyNew = & (* yyNew)->CaseMatch.next; break;
case kCaseRange: (* yyNew)->CaseRange = yyt->CaseRange;
copytPosition ((* yyNew)->CaseRange.position, yyt->CaseRange.position)
copyPtrContext ((* yyNew)->CaseRange.context, yyt->CaseRange.context)
copybool ((* yyNew)->CaseRange.skip, yyt->CaseRange.skip)
copyPtrImplicantList ((* yyNew)->CaseRange.implicants, yyt->CaseRange.implicants)
copyPtrType ((* yyNew)->CaseRange.expectedType, yyt->CaseRange.expectedType)
copybool ((* yyNew)->CaseRange.allowOverlappingMatches, yyt->CaseRange.allowOverlappingMatches)
copybool ((* yyNew)->CaseRange.isSpan, yyt->CaseRange.isSpan)
copytTree ((* yyNew)->CaseRange.Range, yyt->CaseRange.Range)
yyt = yyt->CaseRange.next;
yyNew = & (* yyNew)->CaseRange.next; break;
case kCaseImplicant: (* yyNew)->CaseImplicant = yyt->CaseImplicant;
copytPosition ((* yyNew)->CaseImplicant.position, yyt->CaseImplicant.position)
copyPtrContext ((* yyNew)->CaseImplicant.context, yyt->CaseImplicant.context)
copybool ((* yyNew)->CaseImplicant.skip, yyt->CaseImplicant.skip)
copyPtrImplicantList ((* yyNew)->CaseImplicant.implicants, yyt->CaseImplicant.implicants)
copyPtrType ((* yyNew)->CaseImplicant.expectedType, yyt->CaseImplicant.expectedType)
copybool ((* yyNew)->CaseImplicant.allowOverlappingMatches, yyt->CaseImplicant.allowOverlappingMatches)
copybool ((* yyNew)->CaseImplicant.isSpan, yyt->CaseImplicant.isSpan)
copytTree ((* yyNew)->CaseImplicant.Expr, yyt->CaseImplicant.Expr)
yyt = yyt->CaseImplicant.next;
yyNew = & (* yyNew)->CaseImplicant.next; break;
case kEnumElems: (* yyNew)->EnumElems = yyt->EnumElems;
copytPosition ((* yyNew)->EnumElems.position, yyt->EnumElems.position)
copyPtrContext ((* yyNew)->EnumElems.context, yyt->EnumElems.context)
copybool ((* yyNew)->EnumElems.skip, yyt->EnumElems.skip)
copyPtrMP_INT ((* yyNew)->EnumElems.elementValue, yyt->EnumElems.elementValue)
copyPtrType ((* yyNew)->EnumElems.typeIn, yyt->EnumElems.typeIn)
copyPtrType ((* yyNew)->EnumElems.typeOut, yyt->EnumElems.typeOut)
copybool ((* yyNew)->EnumElems.hasOverType, yyt->EnumElems.hasOverType)
copyPtrBindingList ((* yyNew)->EnumElems.elementsTail, yyt->EnumElems.elementsTail)
return;
case kNullEnumElems: (* yyNew)->NullEnumElems = yyt->NullEnumElems;
copytPosition ((* yyNew)->NullEnumElems.position, yyt->NullEnumElems.position)
copyPtrContext ((* yyNew)->NullEnumElems.context, yyt->NullEnumElems.context)
copybool ((* yyNew)->NullEnumElems.skip, yyt->NullEnumElems.skip)
copyPtrMP_INT ((* yyNew)->NullEnumElems.elementValue, yyt->NullEnumElems.elementValue)
copyPtrType ((* yyNew)->NullEnumElems.typeIn, yyt->NullEnumElems.typeIn)
copyPtrType ((* yyNew)->NullEnumElems.typeOut, yyt->NullEnumElems.typeOut)
copybool ((* yyNew)->NullEnumElems.hasOverType, yyt->NullEnumElems.hasOverType)
copyPtrBindingList ((* yyNew)->NullEnumElems.elementsTail, yyt->NullEnumElems.elementsTail)
return;
case kEnumElem: (* yyNew)->EnumElem = yyt->EnumElem;
copytPosition ((* yyNew)->EnumElem.position, yyt->EnumElem.position)
copyPtrContext ((* yyNew)->EnumElem.context, yyt->EnumElem.context)
copybool ((* yyNew)->EnumElem.skip, yyt->EnumElem.skip)
copyPtrMP_INT ((* yyNew)->EnumElem.elementValue, yyt->EnumElem.elementValue)
copyPtrType ((* yyNew)->EnumElem.typeIn, yyt->EnumElem.typeIn)
copyPtrType ((* yyNew)->EnumElem.typeOut, yyt->EnumElem.typeOut)
copybool ((* yyNew)->EnumElem.hasOverType, yyt->EnumElem.hasOverType)
copyPtrBindingList ((* yyNew)->EnumElem.elementsTail, yyt->EnumElem.elementsTail)
copytIdent ((* yyNew)->EnumElem.ident, yyt->EnumElem.ident)
yyt = yyt->EnumElem.next;
yyNew = & (* yyNew)->EnumElem.next; break;
case kDefaultValuedEnumElem: (* yyNew)->DefaultValuedEnumElem = yyt->DefaultValuedEnumElem;
copytPosition ((* yyNew)->DefaultValuedEnumElem.position, yyt->DefaultValuedEnumElem.position)
copyPtrContext ((* yyNew)->DefaultValuedEnumElem.context, yyt->DefaultValuedEnumElem.context)
copybool ((* yyNew)->DefaultValuedEnumElem.skip, yyt->DefaultValuedEnumElem.skip)
copyPtrMP_INT ((* yyNew)->DefaultValuedEnumElem.elementValue, yyt->DefaultValuedEnumElem.elementValue)
copyPtrType ((* yyNew)->DefaultValuedEnumElem.typeIn, yyt->DefaultValuedEnumElem.typeIn)
copyPtrType ((* yyNew)->DefaultValuedEnumElem.typeOut, yyt->DefaultValuedEnumElem.typeOut)
copybool ((* yyNew)->DefaultValuedEnumElem.hasOverType, yyt->DefaultValuedEnumElem.hasOverType)
copyPtrBindingList ((* yyNew)->DefaultValuedEnumElem.elementsTail, yyt->DefaultValuedEnumElem.elementsTail)
copytIdent ((* yyNew)->DefaultValuedEnumElem.ident, yyt->DefaultValuedEnumElem.ident)
yyt = yyt->DefaultValuedEnumElem.next;
yyNew = & (* yyNew)->DefaultValuedEnumElem.next; break;
case kValuedEnumElem: (* yyNew)->ValuedEnumElem = yyt->ValuedEnumElem;
copytPosition ((* yyNew)->ValuedEnumElem.position, yyt->ValuedEnumElem.position)
copyPtrContext ((* yyNew)->ValuedEnumElem.context, yyt->ValuedEnumElem.context)
copybool ((* yyNew)->ValuedEnumElem.skip, yyt->ValuedEnumElem.skip)
copyPtrMP_INT ((* yyNew)->ValuedEnumElem.elementValue, yyt->ValuedEnumElem.elementValue)
copyPtrType ((* yyNew)->ValuedEnumElem.typeIn, yyt->ValuedEnumElem.typeIn)
copyPtrType ((* yyNew)->ValuedEnumElem.typeOut, yyt->ValuedEnumElem.typeOut)
copybool ((* yyNew)->ValuedEnumElem.hasOverType, yyt->ValuedEnumElem.hasOverType)
copyPtrBindingList ((* yyNew)->ValuedEnumElem.elementsTail, yyt->ValuedEnumElem.elementsTail)
copytIdent ((* yyNew)->ValuedEnumElem.ident, yyt->ValuedEnumElem.ident)
copytTree ((* yyNew)->ValuedEnumElem.CoercedExpr, yyt->ValuedEnumElem.CoercedExpr)
yyt = yyt->ValuedEnumElem.next;
yyNew = & (* yyNew)->ValuedEnumElem.next; break;
case kRecordElems: (* yyNew)->RecordElems = yyt->RecordElems;
copytPosition ((* yyNew)->RecordElems.position, yyt->RecordElems.position)
copyPtrContext ((* yyNew)->RecordElems.context, yyt->RecordElems.context)
copybool ((* yyNew)->RecordElems.skip, yyt->RecordElems.skip)
copyPtrType ((* yyNew)->RecordElems.typeIn, yyt->RecordElems.typeIn)
copyPtrType ((* yyNew)->RecordElems.typeOut, yyt->RecordElems.typeOut)
copybool ((* yyNew)->RecordElems.hasOverType, yyt->RecordElems.hasOverType)
copyPtrInstanceList ((* yyNew)->RecordElems.elementsTail, yyt->RecordElems.elementsTail)
return;
case kNullRecordElems: (* yyNew)->NullRecordElems = yyt->NullRecordElems;
copytPosition ((* yyNew)->NullRecordElems.position, yyt->NullRecordElems.position)
copyPtrContext ((* yyNew)->NullRecordElems.context, yyt->NullRecordElems.context)
copybool ((* yyNew)->NullRecordElems.skip, yyt->NullRecordElems.skip)
copyPtrType ((* yyNew)->NullRecordElems.typeIn, yyt->NullRecordElems.typeIn)
copyPtrType ((* yyNew)->NullRecordElems.typeOut, yyt->NullRecordElems.typeOut)
copybool ((* yyNew)->NullRecordElems.hasOverType, yyt->NullRecordElems.hasOverType)
copyPtrInstanceList ((* yyNew)->NullRecordElems.elementsTail, yyt->NullRecordElems.elementsTail)
return;
case kRecordElem: (* yyNew)->RecordElem = yyt->RecordElem;
copytPosition ((* yyNew)->RecordElem.position, yyt->RecordElem.position)
copyPtrContext ((* yyNew)->RecordElem.context, yyt->RecordElem.context)
copybool ((* yyNew)->RecordElem.skip, yyt->RecordElem.skip)
copyPtrType ((* yyNew)->RecordElem.typeIn, yyt->RecordElem.typeIn)
copyPtrType ((* yyNew)->RecordElem.typeOut, yyt->RecordElem.typeOut)
copybool ((* yyNew)->RecordElem.hasOverType, yyt->RecordElem.hasOverType)
copyPtrInstanceList ((* yyNew)->RecordElem.elementsTail, yyt->RecordElem.elementsTail)
copytTree ((* yyNew)->RecordElem.Idents, yyt->RecordElem.Idents)
copytTree ((* yyNew)->RecordElem.AType, yyt->RecordElem.AType)
yyt = yyt->RecordElem.next;
yyNew = & (* yyNew)->RecordElem.next; break;
case kValDecls: (* yyNew)->ValDecls = yyt->ValDecls;
copytPosition ((* yyNew)->ValDecls.position, yyt->ValDecls.position)
copyPtrContext ((* yyNew)->ValDecls.contextIn, yyt->ValDecls.contextIn)
copyPtrContext ((* yyNew)->ValDecls.contextOut, yyt->ValDecls.contextOut)
copybool ((* yyNew)->ValDecls.skip, yyt->ValDecls.skip)
return;
case kNullValDecls: (* yyNew)->NullValDecls = yyt->NullValDecls;
copytPosition ((* yyNew)->NullValDecls.position, yyt->NullValDecls.position)
copyPtrContext ((* yyNew)->NullValDecls.contextIn, yyt->NullValDecls.contextIn)
copyPtrContext ((* yyNew)->NullValDecls.contextOut, yyt->NullValDecls.contextOut)
copybool ((* yyNew)->NullValDecls.skip, yyt->NullValDecls.skip)
return;
case kValDecl: (* yyNew)->ValDecl = yyt->ValDecl;
copytPosition ((* yyNew)->ValDecl.position, yyt->ValDecl.position)
copyPtrContext ((* yyNew)->ValDecl.contextIn, yyt->ValDecl.contextIn)
copyPtrContext ((* yyNew)->ValDecl.contextOut, yyt->ValDecl.contextOut)
copybool ((* yyNew)->ValDecl.skip, yyt->ValDecl.skip)
copytIdent ((* yyNew)->ValDecl.ident, yyt->ValDecl.ident)
copytTree ((* yyNew)->ValDecl.Expr, yyt->ValDecl.Expr)
yyt = yyt->ValDecl.next;
yyNew = & (* yyNew)->ValDecl.next; break;
  default: ;
  }
 }
}

tTree CopyTree
# if defined __STDC__ | defined __cplusplus
 (tTree yyt)
# else
 (yyt) tTree yyt;
# endif
{
 tTree yyNew;
 yyMark (yyt);
 yyOldToNewCount = 0;
 yyCopyTree (yyt, & yyNew);
 return yyNew;
}

void BeginTree ()
{
}

void CloseTree ()
{
}
