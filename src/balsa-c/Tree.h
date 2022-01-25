# ifndef yyTree
# define yyTree

extern char Tree_module_does_not_match_evaluator_module_25160;
extern char generate_Tree_module_without_option_0;

# if defined __STDC__ | defined __cplusplus
# define ARGS(parameters)	parameters
# else
# define ARGS(parameters)	()
# endif

# include <stdio.h>

# ifndef bool
# define bool char
# endif
# define NoTree (tTree) 0L
# define kBalsaProgram 1
# define kDecls 2
# define kNullDecls 3
# define kDecl 4
# define kImportDecl 5
# define kFileDecl 6
# define kTypeDecl 7
# define kConstantDecl 8
# define kVariableDecl 9
# define kInitVariableDecl 10
# define kChannelDecl 11
# define kChannelArrayDecl 12
# define kSyncDecl 13
# define kSyncArrayDecl 14
# define kProcedureDecl 15
# define kProcAliasDecl 16
# define kFunctionDecl 17
# define kBuiltinFunctionDecl 18
# define kSharedDecl 19
# define kPartDecl 20
# define kIfDecls 21
# define kIfElseDecls 22
# define kPrintDecl 23
# define kNullDecl 24
# define kRange 25
# define kSpecifiedRange 26
# define kTypeRange 27
# define kAType 28
# define kNullType 29
# define kNumericType 30
# define kExistingType 31
# define kBuiltinType 32
# define kArrayType 33
# define kNewType 34
# define kRecordType 35
# define kEnumType 36
# define kCoercedExpr 37
# define kExpr 38
# define kNullExpr 39
# define kLiteralExpr 40
# define kIdentExpr 41
# define kStringExpr 42
# define kImplicantExpr 43
# define kDontCareExpr 44
# define kAggregateConsExpr 45
# define kNamedEnumElemExpr 46
# define kUnaryExpr 47
# define kBinaryExpr 48
# define kRecordElemExtractExpr 49
# define kArrayExtractExpr 50
# define kArraySliceExpr 51
# define kAsExpr 52
# define kBitArrayCastExpr 53
# define kLetExpr 54
# define kFunctionCallExpr 55
# define kSizeofExpr 56
# define kArrayAppendExpr 57
# define kPrimedExpr 58
# define kFormalPorts 59
# define kNullFormalPorts 60
# define kFormalPort 61
# define kValuePort 62
# define kParamPort 63
# define kTypeParamPort 64
# define kChannelPort 65
# define kChannelPortArray 66
# define kSyncPort 67
# define kSyncPortArray 68
# define kIfPorts 69
# define kIfElsePorts 70
# define kBlock 71
# define kParameters 72
# define kNullParameters 73
# define kParameter 74
# define kNumberParameter 75
# define kStringParameter 76
# define kTypeParameter 77
# define kBreezeParameters 78
# define kNullBreezeParameters 79
# define kBreezeParameter 80
# define kBreezeExprParameter 81
# define kBreezeTypeParameter 82
# define kComps 83
# define kNullComps 84
# define kComp 85
# define kNormalComp 86
# define kUndeclaredComp 87
# define kCommand 88
# define kNullCommand 89
# define kContinueCommand 90
# define kHaltCommand 91
# define kInputCommand 92
# define kInputEncloseCommand 93
# define kInputEncloseBangCommand 94
# define kOutputCommand 95
# define kSyncCommand 96
# define kAssignmentCommand 97
# define kBlockCommand 98
# define kSequentialCommand 99
# define kParallelCommand 100
# define kLoopCommand 101
# define kWhileGuardsCommand 102
# define kWhileGuardsAlsoCommand 103
# define kCommandWhileExprCommand 104
# define kCommandWhileGuardsCommand 105
# define kCommandWhileGuardsAlsoCommand 106
# define kIfCommand 107
# define kIfElseCommand 108
# define kCaseCommand 109
# define kForCommand 110
# define kProcedureCallCommonCommand 111
# define kSelectCommand 112
# define kSelectBangCommand 113
# define kArbitrateCommand 114
# define kPrintCommand 115
# define kSinkCommand 116
# define kProcedureParams 117
# define kNullProcParams 118
# define kProcParam 119
# define kExprProcParam 120
# define kTypeProcParam 121
# define kVarReadProcParam 122
# define kVarWriteProcParam 123
# define kBlockProcParam 124
# define kFunctionParams 125
# define kNullFuncParams 126
# define kFuncParam 127
# define kExprFuncParam 128
# define kTypeFuncParam 129
# define kLvalue 130
# define kIdentLvalue 131
# define kRecordElemLvalue 132
# define kArrayElemLvalue 133
# define kArraySliceLvalue 134
# define kArrayAppendLvalue 135
# define kArrayConsLvalue 136
# define kAsLvalue 137
# define kBitArrayCastLvalue 138
# define kGuards 139
# define kNullGuards 140
# define kGuard 141
# define kPortGuards 142
# define kNullPortGuards 143
# define kPortGuard 144
# define kDeclGuards 145
# define kNullDeclGuards 146
# define kDeclGuard 147
# define kCaseGuards 148
# define kNullCaseGuards 149
# define kCaseGuard 150
# define kCaseMatchGuard 151
# define kForCaseGuard 152
# define kChannelGuards 153
# define kNullChannelGuards 154
# define kChannelGuard 155
# define kIdents 156
# define kNullIdents 157
# define kIdent 158
# define kExprLists 159
# define kNullExprLists 160
# define kExprList 161
# define kLvalueLists 162
# define kNullLvalueLists 163
# define kLvalueList 164
# define kLinkedBlock 165
# define kLinkedChannel 166
# define kCaseMatchs 167
# define kNullCaseMatchs 168
# define kCaseMatch 169
# define kCaseRange 170
# define kCaseImplicant 171
# define kEnumElems 172
# define kNullEnumElems 173
# define kEnumElem 174
# define kDefaultValuedEnumElem 175
# define kValuedEnumElem 176
# define kRecordElems 177
# define kNullRecordElems 178
# define kRecordElem 179
# define kValDecls 180
# define kNullValDecls 181
# define kValDecl 182

typedef unsigned char Tree_tKind;
typedef unsigned short Tree_tMark;
typedef unsigned short Tree_tLabel;
typedef union Tree_Node * tTree;
typedef void (* Tree_tProcTree) ARGS((tTree));
# line 31 "Balsa.ag"

#include <string.h>
#include "decls.h"
#include "exprs.h"
#include "lvalues.h"
#include "channels.h"
#include "commands.h"
#include "block.h"
#include "ports.h"
#include "parts.h"
#include "implicants.h"
#include "Errors.h"
#include "pretty.h"
#include "callcontexts.h"

#include "BreezeScanSource.h"
#include "BreezeScan.h"

extern void VisitTreeNode (tTree node);


# ifndef Tree_NodeHead
# define Tree_NodeHead
# endif
typedef struct { Tree_tKind yyKind; Tree_tMark yyMark; Tree_NodeHead } Tree_tNodeHead;
typedef struct { Tree_tNodeHead yyHead; tTree Decls; PtrContext context; } yBalsaProgram;
typedef struct { Tree_tNodeHead yyHead; tPosition position; Scope scope; PtrContext contextIn; PtrContext contextOut; bool skip; } yDecls;
typedef struct { Tree_tNodeHead yyHead; tPosition position; Scope scope; PtrContext contextIn; PtrContext contextOut; bool skip; } yNullDecls;
typedef struct { Tree_tNodeHead yyHead; tPosition position; Scope scope; PtrContext contextIn; PtrContext contextOut; bool skip; tTree next; } yDecl;
typedef struct { Tree_tNodeHead yyHead; tPosition position; Scope scope; PtrContext contextIn; PtrContext contextOut; bool skip; tTree next; tTree Idents; } yImportDecl;
typedef struct { Tree_tNodeHead yyHead; tPosition position; Scope scope; PtrContext contextIn; PtrContext contextOut; bool skip; tTree next; tTree Idents; } yFileDecl;
typedef struct { Tree_tNodeHead yyHead; tPosition position; Scope scope; PtrContext contextIn; PtrContext contextOut; bool skip; tTree next; tIdent ident; tTree AType; } yTypeDecl;
typedef struct { Tree_tNodeHead yyHead; tPosition position; Scope scope; PtrContext contextIn; PtrContext contextOut; bool skip; tTree next; tIdent ident; tTree CoercedExpr; tTree AType; } yConstantDecl;
typedef struct { Tree_tNodeHead yyHead; tPosition position; Scope scope; PtrContext contextIn; PtrContext contextOut; bool skip; tTree next; tTree Idents; tTree AType; } yVariableDecl;
typedef struct { Tree_tNodeHead yyHead; tPosition position; Scope scope; PtrContext contextIn; PtrContext contextOut; bool skip; tTree next; tTree Idents; tTree CoercedExpr; tTree AType; } yInitVariableDecl;
typedef struct { Tree_tNodeHead yyHead; tPosition position; Scope scope; PtrContext contextIn; PtrContext contextOut; bool skip; tTree next; tTree Idents; tTree AType; bool multicast; } yChannelDecl;
typedef struct { Tree_tNodeHead yyHead; tPosition position; Scope scope; PtrContext contextIn; PtrContext contextOut; bool skip; tTree next; tTree Idents; tTree AType; tTree Range; bool multicast; } yChannelArrayDecl;
typedef struct { Tree_tNodeHead yyHead; tPosition position; Scope scope; PtrContext contextIn; PtrContext contextOut; bool skip; tTree next; tTree Idents; bool multicast; } ySyncDecl;
typedef struct { Tree_tNodeHead yyHead; tPosition position; Scope scope; PtrContext contextIn; PtrContext contextOut; bool skip; tTree next; tTree Idents; tTree Range; bool multicast; } ySyncArrayDecl;
typedef struct { Tree_tNodeHead yyHead; tPosition position; Scope scope; PtrContext contextIn; PtrContext contextOut; bool skip; tTree next; tIdent ident; tTree FormalPorts; tTree Block; PtrProcedure procedure; } yProcedureDecl;
typedef struct { Tree_tNodeHead yyHead; tPosition position; Scope scope; PtrContext contextIn; PtrContext contextOut; bool skip; tTree next; tIdent newName; tIdent oldName; tTree ProcedureParams; } yProcAliasDecl;
typedef struct { Tree_tNodeHead yyHead; tPosition position; Scope scope; PtrContext contextIn; PtrContext contextOut; bool skip; tTree next; tIdent ident; tTree FormalPorts; tTree CoercedExpr; tTree AType; PtrProcedure function; } yFunctionDecl;
typedef struct { Tree_tNodeHead yyHead; tPosition position; Scope scope; PtrContext contextIn; PtrContext contextOut; bool skip; tTree next; tIdent ident; tTree FormalPorts; tTree AType; PtrProcedure function; } yBuiltinFunctionDecl;
typedef struct { Tree_tNodeHead yyHead; tPosition position; Scope scope; PtrContext contextIn; PtrContext contextOut; bool skip; tTree next; tIdent ident; tTree Block; PtrProcedure procedure; } ySharedDecl;
typedef struct { Tree_tNodeHead yyHead; tPosition position; Scope scope; PtrContext contextIn; PtrContext contextOut; bool skip; tTree next; tIdent ident; PtrLispList attributes; tTree FormalPorts; PtrWireArray channels; unsigned channelCount; tTree Comps; PtrCallContextList callcontexts; PtrProcedure procedure; } yPartDecl;
typedef struct { Tree_tNodeHead yyHead; tPosition position; Scope scope; PtrContext contextIn; PtrContext contextOut; bool skip; tTree next; tTree DeclGuards; } yIfDecls;
typedef struct { Tree_tNodeHead yyHead; tPosition position; Scope scope; PtrContext contextIn; PtrContext contextOut; bool skip; tTree next; tTree DeclGuards; tTree Decls; } yIfElseDecls;
typedef struct { Tree_tNodeHead yyHead; tPosition position; Scope scope; PtrContext contextIn; PtrContext contextOut; bool skip; tTree next; tTree Expr; tTree ExprLists; } yPrintDecl;
typedef struct { Tree_tNodeHead yyHead; tPosition position; Scope scope; PtrContext contextIn; PtrContext contextOut; bool skip; tTree next; } yNullDecl;
typedef struct { Tree_tNodeHead yyHead; tPosition position; PtrContext context; bool skip; Span range; PtrType expectedType; bool isSpan; } yRange;
typedef struct { Tree_tNodeHead yyHead; tPosition position; PtrContext context; bool skip; Span range; PtrType expectedType; bool isSpan; tTree Left; tTree Right; } ySpecifiedRange;
typedef struct { Tree_tNodeHead yyHead; tPosition position; PtrContext context; bool skip; Span range; PtrType expectedType; bool isSpan; tTree AType; } yTypeRange;
typedef struct { Tree_tNodeHead yyHead; tPosition position; PtrContext context; bool skip; PtrType type; } yAType;
typedef struct { Tree_tNodeHead yyHead; tPosition position; PtrContext context; bool skip; PtrType type; } yNullType;
typedef struct { Tree_tNodeHead yyHead; tPosition position; PtrContext context; bool skip; PtrType type; bool signedness; tTree Expr; } yNumericType;
typedef struct { Tree_tNodeHead yyHead; tPosition position; PtrContext context; bool skip; PtrType type; tIdent ident; } yExistingType;
typedef struct { Tree_tNodeHead yyHead; tPosition position; PtrContext context; bool skip; PtrType type; } yBuiltinType;
typedef struct { Tree_tNodeHead yyHead; tPosition position; PtrContext context; bool skip; PtrType type; tTree AType; tTree Range; } yArrayType;
typedef struct { Tree_tNodeHead yyHead; tPosition position; PtrContext context; bool skip; PtrType type; tTree AType; } yNewType;
typedef struct { Tree_tNodeHead yyHead; tPosition position; PtrContext context; bool skip; PtrType type; tTree RecordElems; tTree AType; } yRecordType;
typedef struct { Tree_tNodeHead yyHead; tPosition position; PtrContext context; bool skip; PtrType type; tTree EnumElems; tTree AType; } yEnumType;
typedef struct { Tree_tNodeHead yyHead; tTree Expr; PtrContext context; bool skip; PtrType expectedType; ExprAttributes attributes; bool allowChannels; tPosition position; } yCoercedExpr;
typedef struct { Tree_tNodeHead yyHead; tPosition position; PtrContext context; bool skip; PtrType expectedType; bool allowChannels; ExprAttributes attributes; } yExpr;
typedef struct { Tree_tNodeHead yyHead; tPosition position; PtrContext context; bool skip; PtrType expectedType; bool allowChannels; ExprAttributes attributes; } yNullExpr;
typedef struct { Tree_tNodeHead yyHead; tPosition position; PtrContext context; bool skip; PtrType expectedType; bool allowChannels; ExprAttributes attributes; PtrMP_INT literal; } yLiteralExpr;
typedef struct { Tree_tNodeHead yyHead; tPosition position; PtrContext context; bool skip; PtrType expectedType; bool allowChannels; ExprAttributes attributes; tIdent ident; } yIdentExpr;
typedef struct { Tree_tNodeHead yyHead; tPosition position; PtrContext context; bool skip; PtrType expectedType; bool allowChannels; ExprAttributes attributes; Ptrconstchar string; } yStringExpr;
typedef struct { Tree_tNodeHead yyHead; tPosition position; PtrContext context; bool skip; PtrType expectedType; bool allowChannels; ExprAttributes attributes; Implicant implicant; } yImplicantExpr;
typedef struct { Tree_tNodeHead yyHead; tPosition position; PtrContext context; bool skip; PtrType expectedType; bool allowChannels; ExprAttributes attributes; } yDontCareExpr;
typedef struct { Tree_tNodeHead yyHead; tPosition position; PtrContext context; bool skip; PtrType expectedType; bool allowChannels; ExprAttributes attributes; tIdent ident; tTree ExprLists; PtrType actualType; } yAggregateConsExpr;
typedef struct { Tree_tNodeHead yyHead; tPosition position; PtrContext context; bool skip; PtrType expectedType; bool allowChannels; ExprAttributes attributes; tIdent typeName; tIdent elemName; } yNamedEnumElemExpr;
typedef struct { Tree_tNodeHead yyHead; tPosition position; PtrContext context; bool skip; PtrType expectedType; bool allowChannels; ExprAttributes attributes; Operators operation; tTree Expr; } yUnaryExpr;
typedef struct { Tree_tNodeHead yyHead; tPosition position; PtrContext context; bool skip; PtrType expectedType; bool allowChannels; ExprAttributes attributes; Operators operation; tTree Left; tTree Right; } yBinaryExpr;
typedef struct { Tree_tNodeHead yyHead; tPosition position; PtrContext context; bool skip; PtrType expectedType; bool allowChannels; ExprAttributes attributes; tTree Expr; tIdent ident; } yRecordElemExtractExpr;
typedef struct { Tree_tNodeHead yyHead; tPosition position; PtrContext context; bool skip; PtrType expectedType; bool allowChannels; ExprAttributes attributes; tTree Array; tTree Subscript; } yArrayExtractExpr;
typedef struct { Tree_tNodeHead yyHead; tPosition position; PtrContext context; bool skip; PtrType expectedType; bool allowChannels; ExprAttributes attributes; tTree Expr; tTree Range; } yArraySliceExpr;
typedef struct { Tree_tNodeHead yyHead; tPosition position; PtrContext context; bool skip; PtrType expectedType; bool allowChannels; ExprAttributes attributes; tTree Expr; tTree AType; } yAsExpr;
typedef struct { Tree_tNodeHead yyHead; tPosition position; PtrContext context; bool skip; PtrType expectedType; bool allowChannels; ExprAttributes attributes; tTree Expr; } yBitArrayCastExpr;
typedef struct { Tree_tNodeHead yyHead; tPosition position; PtrContext context; bool skip; PtrType expectedType; bool allowChannels; ExprAttributes attributes; tTree ValDecls; tTree Expr; } yLetExpr;
typedef struct { Tree_tNodeHead yyHead; tPosition position; PtrContext context; bool skip; PtrType expectedType; bool allowChannels; ExprAttributes attributes; tIdent ident; tTree FunctionParams; } yFunctionCallExpr;
typedef struct { Tree_tNodeHead yyHead; tPosition position; PtrContext context; bool skip; PtrType expectedType; bool allowChannels; ExprAttributes attributes; tIdent ident; } ySizeofExpr;
typedef struct { Tree_tNodeHead yyHead; tPosition position; PtrContext context; bool skip; PtrType expectedType; bool allowChannels; ExprAttributes attributes; tTree Left; tTree Right; } yArrayAppendExpr;
typedef struct { Tree_tNodeHead yyHead; tPosition position; PtrContext context; bool skip; PtrType expectedType; bool allowChannels; ExprAttributes attributes; ExprAttributes results; } yPrimedExpr;
typedef struct { Tree_tNodeHead yyHead; tPosition position; PtrContext context; bool skip; ProcedureArgsType portsType; bool inConditionalPorts; PtrInstanceList ports; unsigned portCount; PtrInstanceList extraPortInstances; bool hasParameters; } yFormalPorts;
typedef struct { Tree_tNodeHead yyHead; tPosition position; PtrContext context; bool skip; ProcedureArgsType portsType; bool inConditionalPorts; PtrInstanceList ports; unsigned portCount; PtrInstanceList extraPortInstances; bool hasParameters; } yNullFormalPorts;
typedef struct { Tree_tNodeHead yyHead; tPosition position; PtrContext context; bool skip; ProcedureArgsType portsType; bool inConditionalPorts; PtrInstanceList ports; unsigned portCount; PtrInstanceList extraPortInstances; bool hasParameters; tTree next; } yFormalPort;
typedef struct { Tree_tNodeHead yyHead; tPosition position; PtrContext context; bool skip; ProcedureArgsType portsType; bool inConditionalPorts; PtrInstanceList ports; unsigned portCount; PtrInstanceList extraPortInstances; bool hasParameters; tTree next; tTree Idents; tTree AType; } yValuePort;
typedef struct { Tree_tNodeHead yyHead; tPosition position; PtrContext context; bool skip; ProcedureArgsType portsType; bool inConditionalPorts; PtrInstanceList ports; unsigned portCount; PtrInstanceList extraPortInstances; bool hasParameters; tTree next; tTree Idents; tTree AType; } yParamPort;
typedef struct { Tree_tNodeHead yyHead; tPosition position; PtrContext context; bool skip; ProcedureArgsType portsType; bool inConditionalPorts; PtrInstanceList ports; unsigned portCount; PtrInstanceList extraPortInstances; bool hasParameters; tTree next; tTree Idents; } yTypeParamPort;
typedef struct { Tree_tNodeHead yyHead; tPosition position; PtrContext context; bool skip; ProcedureArgsType portsType; bool inConditionalPorts; PtrInstanceList ports; unsigned portCount; PtrInstanceList extraPortInstances; bool hasParameters; tTree next; tTree Idents; tTree AType; PortSense portSense; bool isOutput; PtrLispList options; } yChannelPort;
typedef struct { Tree_tNodeHead yyHead; tPosition position; PtrContext context; bool skip; ProcedureArgsType portsType; bool inConditionalPorts; PtrInstanceList ports; unsigned portCount; PtrInstanceList extraPortInstances; bool hasParameters; tTree next; tTree Idents; tTree AType; PortSense portSense; bool isOutput; tTree Range; PtrLispList options; } yChannelPortArray;
typedef struct { Tree_tNodeHead yyHead; tPosition position; PtrContext context; bool skip; ProcedureArgsType portsType; bool inConditionalPorts; PtrInstanceList ports; unsigned portCount; PtrInstanceList extraPortInstances; bool hasParameters; tTree next; tTree Idents; PortSense portSense; PtrLispList options; } ySyncPort;
typedef struct { Tree_tNodeHead yyHead; tPosition position; PtrContext context; bool skip; ProcedureArgsType portsType; bool inConditionalPorts; PtrInstanceList ports; unsigned portCount; PtrInstanceList extraPortInstances; bool hasParameters; tTree next; tTree Idents; PortSense portSense; tTree Range; PtrLispList options; } ySyncPortArray;
typedef struct { Tree_tNodeHead yyHead; tPosition position; PtrContext context; bool skip; ProcedureArgsType portsType; bool inConditionalPorts; PtrInstanceList ports; unsigned portCount; PtrInstanceList extraPortInstances; bool hasParameters; tTree next; tTree PortGuards; } yIfPorts;
typedef struct { Tree_tNodeHead yyHead; tPosition position; PtrContext context; bool skip; ProcedureArgsType portsType; bool inConditionalPorts; PtrInstanceList ports; unsigned portCount; PtrInstanceList extraPortInstances; bool hasParameters; tTree next; tTree PortGuards; tTree FormalPorts; } yIfElsePorts;
typedef struct { Tree_tNodeHead yyHead; tPosition position; tTree Decls; tTree Command; PtrContext context; bool skip; CommandAttributes attributes; } yBlock;
typedef struct { Tree_tNodeHead yyHead; tPosition position; PtrContext context; bool skip; unsigned paramNoIn; unsigned paramNoOut; PtrComponentParameterList parameters; PtrComponent component; } yParameters;
typedef struct { Tree_tNodeHead yyHead; tPosition position; PtrContext context; bool skip; unsigned paramNoIn; unsigned paramNoOut; PtrComponentParameterList parameters; PtrComponent component; } yNullParameters;
typedef struct { Tree_tNodeHead yyHead; tPosition position; PtrContext context; bool skip; unsigned paramNoIn; unsigned paramNoOut; PtrComponentParameterList parameters; PtrComponent component; tTree next; } yParameter;
typedef struct { Tree_tNodeHead yyHead; tPosition position; PtrContext context; bool skip; unsigned paramNoIn; unsigned paramNoOut; PtrComponentParameterList parameters; PtrComponent component; tTree next; PtrMP_INT literal; } yNumberParameter;
typedef struct { Tree_tNodeHead yyHead; tPosition position; PtrContext context; bool skip; unsigned paramNoIn; unsigned paramNoOut; PtrComponentParameterList parameters; PtrComponent component; tTree next; tIdent ident; } yStringParameter;
typedef struct { Tree_tNodeHead yyHead; tPosition position; PtrContext context; bool skip; unsigned paramNoIn; unsigned paramNoOut; PtrComponentParameterList parameters; PtrComponent component; tTree next; tTree AType; } yTypeParameter;
typedef struct { Tree_tNodeHead yyHead; tPosition position; PtrContext context; bool skip; PtrComponentParameterList parameters; } yBreezeParameters;
typedef struct { Tree_tNodeHead yyHead; tPosition position; PtrContext context; bool skip; PtrComponentParameterList parameters; } yNullBreezeParameters;
typedef struct { Tree_tNodeHead yyHead; tPosition position; PtrContext context; bool skip; PtrComponentParameterList parameters; tTree next; } yBreezeParameter;
typedef struct { Tree_tNodeHead yyHead; tPosition position; PtrContext context; bool skip; PtrComponentParameterList parameters; tTree next; tIdent ident; tTree AType; } yBreezeExprParameter;
typedef struct { Tree_tNodeHead yyHead; tPosition position; PtrContext context; bool skip; PtrComponentParameterList parameters; tTree next; tIdent ident; } yBreezeTypeParameter;
typedef struct { Tree_tNodeHead yyHead; tPosition position; PtrContext context; bool skip; PtrWireArray channels; unsigned channelCount; PtrComponentList componentsIn; PtrComponentList componentsOut; } yComps;
typedef struct { Tree_tNodeHead yyHead; tPosition position; PtrContext context; bool skip; PtrWireArray channels; unsigned channelCount; PtrComponentList componentsIn; PtrComponentList componentsOut; } yNullComps;
typedef struct { Tree_tNodeHead yyHead; tPosition position; PtrContext context; bool skip; PtrWireArray channels; unsigned channelCount; PtrComponentList componentsIn; PtrComponentList componentsOut; tTree next; } yComp;
typedef struct { Tree_tNodeHead yyHead; tPosition position; PtrContext context; bool skip; PtrWireArray channels; unsigned channelCount; PtrComponentList componentsIn; PtrComponentList componentsOut; tTree next; tIdent ident; bool internal; tTree Parameters; PtrintList channelNos; PtrLispList options; } yNormalComp;
typedef struct { Tree_tNodeHead yyHead; tPosition position; PtrContext context; bool skip; PtrWireArray channels; unsigned channelCount; PtrComponentList componentsIn; PtrComponentList componentsOut; tTree next; tIdent ident; tTree Parameters; PtrintList channelNos; tIdent componentType; tIdent baseComponentName; tTree BreezeParameters; tTree FormalPorts; PtrLispList options; } yUndeclaredComp;
typedef struct { Tree_tNodeHead yyHead; tPosition position; PtrContext context; bool skip; CommandAttributes attributes; } yCommand;
typedef struct { Tree_tNodeHead yyHead; tPosition position; PtrContext context; bool skip; CommandAttributes attributes; } yNullCommand;
typedef struct { Tree_tNodeHead yyHead; tPosition position; PtrContext context; bool skip; CommandAttributes attributes; } yContinueCommand;
typedef struct { Tree_tNodeHead yyHead; tPosition position; PtrContext context; bool skip; CommandAttributes attributes; } yHaltCommand;
typedef struct { Tree_tNodeHead yyHead; tPosition position; PtrContext context; bool skip; CommandAttributes attributes; tTree LHS; tTree RHS; } yInputCommand;
typedef struct { Tree_tNodeHead yyHead; tPosition position; PtrContext context; bool skip; CommandAttributes attributes; tTree LvalueLists; tTree Command; } yInputEncloseCommand;
typedef struct { Tree_tNodeHead yyHead; tPosition position; PtrContext context; bool skip; CommandAttributes attributes; tTree LvalueLists; tTree Command; } yInputEncloseBangCommand;
typedef struct { Tree_tNodeHead yyHead; tPosition position; PtrContext context; bool skip; CommandAttributes attributes; tTree Lvalue; tTree CoercedExpr; } yOutputCommand;
typedef struct { Tree_tNodeHead yyHead; tPosition position; PtrContext context; bool skip; CommandAttributes attributes; tTree Lvalue; } ySyncCommand;
typedef struct { Tree_tNodeHead yyHead; tPosition position; PtrContext context; bool skip; CommandAttributes attributes; tTree Lvalue; tTree CoercedExpr; } yAssignmentCommand;
typedef struct { Tree_tNodeHead yyHead; tPosition position; PtrContext context; bool skip; CommandAttributes attributes; tTree Block; } yBlockCommand;
typedef struct { Tree_tNodeHead yyHead; tPosition position; PtrContext context; bool skip; CommandAttributes attributes; tTree Left; tTree Right; } ySequentialCommand;
typedef struct { Tree_tNodeHead yyHead; tPosition position; PtrContext context; bool skip; CommandAttributes attributes; bool isPermissive; tTree Left; tTree Right; } yParallelCommand;
typedef struct { Tree_tNodeHead yyHead; tPosition position; PtrContext context; bool skip; CommandAttributes attributes; tTree Command; } yLoopCommand;
typedef struct { Tree_tNodeHead yyHead; tPosition position; PtrContext context; bool skip; CommandAttributes attributes; tTree Guards; } yWhileGuardsCommand;
typedef struct { Tree_tNodeHead yyHead; tPosition position; PtrContext context; bool skip; CommandAttributes attributes; tTree Guards; tTree Command; } yWhileGuardsAlsoCommand;
typedef struct { Tree_tNodeHead yyHead; tPosition position; PtrContext context; bool skip; CommandAttributes attributes; tTree Command; tTree Expr; } yCommandWhileExprCommand;
typedef struct { Tree_tNodeHead yyHead; tPosition position; PtrContext context; bool skip; CommandAttributes attributes; tTree Command; tTree Guards; } yCommandWhileGuardsCommand;
typedef struct { Tree_tNodeHead yyHead; tPosition position; PtrContext context; bool skip; CommandAttributes attributes; tTree FirstCommand; tTree Guards; tTree AlsoCommand; } yCommandWhileGuardsAlsoCommand;
typedef struct { Tree_tNodeHead yyHead; tPosition position; PtrContext context; bool skip; CommandAttributes attributes; tTree Guards; } yIfCommand;
typedef struct { Tree_tNodeHead yyHead; tPosition position; PtrContext context; bool skip; CommandAttributes attributes; tTree Guards; tTree Command; } yIfElseCommand;
typedef struct { Tree_tNodeHead yyHead; tPosition position; PtrContext context; bool skip; CommandAttributes attributes; tTree Expr; tTree CaseGuards; tTree Command; } yCaseCommand;
typedef struct { Tree_tNodeHead yyHead; tPosition position; PtrContext context; bool skip; CommandAttributes attributes; bool isParallel; bool isPermissive; tIdent ident; tTree Range; tTree Command; PtrInstance iterator; int lastErrorCount; Ptrchar errorContextString; Ptrchar valueStringPtr; } yForCommand;
typedef struct { Tree_tNodeHead yyHead; tPosition position; PtrContext context; bool skip; CommandAttributes attributes; tIdent ident; tTree ProcedureParams; } yProcedureCallCommonCommand;
typedef struct { Tree_tNodeHead yyHead; tPosition position; PtrContext context; bool skip; CommandAttributes attributes; tTree ChannelGuards; } ySelectCommand;
typedef struct { Tree_tNodeHead yyHead; tPosition position; PtrContext context; bool skip; CommandAttributes attributes; tTree ChannelGuards; } ySelectBangCommand;
typedef struct { Tree_tNodeHead yyHead; tPosition position; PtrContext context; bool skip; CommandAttributes attributes; tTree ChannelGuards; } yArbitrateCommand;
typedef struct { Tree_tNodeHead yyHead; tPosition position; PtrContext context; bool skip; CommandAttributes attributes; tTree Expr; tTree ExprLists; } yPrintCommand;
typedef struct { Tree_tNodeHead yyHead; tPosition position; PtrContext context; bool skip; CommandAttributes attributes; tTree Expr; } ySinkCommand;
typedef struct { Tree_tNodeHead yyHead; tPosition position; PtrContext context; bool skip; PtrExprAttributesList actualPorts; tTree formalParams; PtrInstanceList formalPorts; PtrInstanceList constantParams; PtrTypeList typeParams; PtrIdentList idents; PtrProcedure procedureIn; PtrProcedure procedureOut; tTree procedureTree; bool noPorts; PtrContext parameterContext; } yProcedureParams;
typedef struct { Tree_tNodeHead yyHead; tPosition position; PtrContext context; bool skip; PtrExprAttributesList actualPorts; tTree formalParams; PtrInstanceList formalPorts; PtrInstanceList constantParams; PtrTypeList typeParams; PtrIdentList idents; PtrProcedure procedureIn; PtrProcedure procedureOut; tTree procedureTree; bool noPorts; PtrContext parameterContext; } yNullProcParams;
typedef struct { Tree_tNodeHead yyHead; tPosition position; PtrContext context; bool skip; PtrExprAttributesList actualPorts; tTree formalParams; PtrInstanceList formalPorts; PtrInstanceList constantParams; PtrTypeList typeParams; PtrIdentList idents; PtrProcedure procedureIn; PtrProcedure procedureOut; tTree procedureTree; bool noPorts; PtrContext parameterContext; tTree next; } yProcParam;
typedef struct { Tree_tNodeHead yyHead; tPosition position; PtrContext context; bool skip; PtrExprAttributesList actualPorts; tTree formalParams; PtrInstanceList formalPorts; PtrInstanceList constantParams; PtrTypeList typeParams; PtrIdentList idents; PtrProcedure procedureIn; PtrProcedure procedureOut; tTree procedureTree; bool noPorts; PtrContext parameterContext; tTree next; tTree CoercedExpr; PtrType typeIfTypeName; } yExprProcParam;
typedef struct { Tree_tNodeHead yyHead; tPosition position; PtrContext context; bool skip; PtrExprAttributesList actualPorts; tTree formalParams; PtrInstanceList formalPorts; PtrInstanceList constantParams; PtrTypeList typeParams; PtrIdentList idents; PtrProcedure procedureIn; PtrProcedure procedureOut; tTree procedureTree; bool noPorts; PtrContext parameterContext; tTree next; tTree AType; } yTypeProcParam;
typedef struct { Tree_tNodeHead yyHead; tPosition position; PtrContext context; bool skip; PtrExprAttributesList actualPorts; tTree formalParams; PtrInstanceList formalPorts; PtrInstanceList constantParams; PtrTypeList typeParams; PtrIdentList idents; PtrProcedure procedureIn; PtrProcedure procedureOut; tTree procedureTree; bool noPorts; PtrContext parameterContext; tTree next; tTree CoercedExpr; } yVarReadProcParam;
typedef struct { Tree_tNodeHead yyHead; tPosition position; PtrContext context; bool skip; PtrExprAttributesList actualPorts; tTree formalParams; PtrInstanceList formalPorts; PtrInstanceList constantParams; PtrTypeList typeParams; PtrIdentList idents; PtrProcedure procedureIn; PtrProcedure procedureOut; tTree procedureTree; bool noPorts; PtrContext parameterContext; tTree next; tTree Lvalue; } yVarWriteProcParam;
typedef struct { Tree_tNodeHead yyHead; tPosition position; PtrContext context; bool skip; PtrExprAttributesList actualPorts; tTree formalParams; PtrInstanceList formalPorts; PtrInstanceList constantParams; PtrTypeList typeParams; PtrIdentList idents; PtrProcedure procedureIn; PtrProcedure procedureOut; tTree procedureTree; bool noPorts; PtrContext parameterContext; tTree next; tTree Block; } yBlockProcParam;
typedef struct { Tree_tNodeHead yyHead; tPosition position; PtrContext context; bool skip; PtrExprAttributesList actualPorts; tTree formalParams; PtrInstanceList formalPorts; PtrInstanceList constantParams; PtrTypeList typeParams; PtrComponentParameterList params; PtrIdentList idents; PtrProcedure functionIn; PtrProcedure functionOut; tTree functionTree; PtrContext parameterContext; } yFunctionParams;
typedef struct { Tree_tNodeHead yyHead; tPosition position; PtrContext context; bool skip; PtrExprAttributesList actualPorts; tTree formalParams; PtrInstanceList formalPorts; PtrInstanceList constantParams; PtrTypeList typeParams; PtrComponentParameterList params; PtrIdentList idents; PtrProcedure functionIn; PtrProcedure functionOut; tTree functionTree; PtrContext parameterContext; } yNullFuncParams;
typedef struct { Tree_tNodeHead yyHead; tPosition position; PtrContext context; bool skip; PtrExprAttributesList actualPorts; tTree formalParams; PtrInstanceList formalPorts; PtrInstanceList constantParams; PtrTypeList typeParams; PtrComponentParameterList params; PtrIdentList idents; PtrProcedure functionIn; PtrProcedure functionOut; tTree functionTree; PtrContext parameterContext; tTree next; } yFuncParam;
typedef struct { Tree_tNodeHead yyHead; tPosition position; PtrContext context; bool skip; PtrExprAttributesList actualPorts; tTree formalParams; PtrInstanceList formalPorts; PtrInstanceList constantParams; PtrTypeList typeParams; PtrComponentParameterList params; PtrIdentList idents; PtrProcedure functionIn; PtrProcedure functionOut; tTree functionTree; PtrContext parameterContext; tTree next; tTree CoercedExpr; PtrType typeIfTypeName; } yExprFuncParam;
typedef struct { Tree_tNodeHead yyHead; tPosition position; PtrContext context; bool skip; PtrExprAttributesList actualPorts; tTree formalParams; PtrInstanceList formalPorts; PtrInstanceList constantParams; PtrTypeList typeParams; PtrComponentParameterList params; PtrIdentList idents; PtrProcedure functionIn; PtrProcedure functionOut; tTree functionTree; PtrContext parameterContext; tTree next; tTree AType; } yTypeFuncParam;
typedef struct { Tree_tNodeHead yyHead; tPosition position; PtrContext context; bool skip; ExprAttributes attributes; bool expectingChannel; bool expectingEither; bool isInput; bool isPassive; bool lockPassiveChannels; PtrInstance instance; PtrSpanList partitions; PtrSpanListList indices; PtrWire indexWire; PtrAccess access; PtrType expectedBaseType; } yLvalue;
typedef struct { Tree_tNodeHead yyHead; tPosition position; PtrContext context; bool skip; ExprAttributes attributes; bool expectingChannel; bool expectingEither; bool isInput; bool isPassive; bool lockPassiveChannels; PtrInstance instance; PtrSpanList partitions; PtrSpanListList indices; PtrWire indexWire; PtrAccess access; PtrType expectedBaseType; tIdent ident; } yIdentLvalue;
typedef struct { Tree_tNodeHead yyHead; tPosition position; PtrContext context; bool skip; ExprAttributes attributes; bool expectingChannel; bool expectingEither; bool isInput; bool isPassive; bool lockPassiveChannels; PtrInstance instance; PtrSpanList partitions; PtrSpanListList indices; PtrWire indexWire; PtrAccess access; PtrType expectedBaseType; tTree Lvalue; tIdent ident; } yRecordElemLvalue;
typedef struct { Tree_tNodeHead yyHead; tPosition position; PtrContext context; bool skip; ExprAttributes attributes; bool expectingChannel; bool expectingEither; bool isInput; bool isPassive; bool lockPassiveChannels; PtrInstance instance; PtrSpanList partitions; PtrSpanListList indices; PtrWire indexWire; PtrAccess access; PtrType expectedBaseType; tTree Lvalue; tTree CoercedExpr; } yArrayElemLvalue;
typedef struct { Tree_tNodeHead yyHead; tPosition position; PtrContext context; bool skip; ExprAttributes attributes; bool expectingChannel; bool expectingEither; bool isInput; bool isPassive; bool lockPassiveChannels; PtrInstance instance; PtrSpanList partitions; PtrSpanListList indices; PtrWire indexWire; PtrAccess access; PtrType expectedBaseType; tTree Lvalue; tTree Range; } yArraySliceLvalue;
typedef struct { Tree_tNodeHead yyHead; tPosition position; PtrContext context; bool skip; ExprAttributes attributes; bool expectingChannel; bool expectingEither; bool isInput; bool isPassive; bool lockPassiveChannels; PtrInstance instance; PtrSpanList partitions; PtrSpanListList indices; PtrWire indexWire; PtrAccess access; PtrType expectedBaseType; tTree Left; tTree Right; } yArrayAppendLvalue;
typedef struct { Tree_tNodeHead yyHead; tPosition position; PtrContext context; bool skip; ExprAttributes attributes; bool expectingChannel; bool expectingEither; bool isInput; bool isPassive; bool lockPassiveChannels; PtrInstance instance; PtrSpanList partitions; PtrSpanListList indices; PtrWire indexWire; PtrAccess access; PtrType expectedBaseType; tTree LvalueLists; } yArrayConsLvalue;
typedef struct { Tree_tNodeHead yyHead; tPosition position; PtrContext context; bool skip; ExprAttributes attributes; bool expectingChannel; bool expectingEither; bool isInput; bool isPassive; bool lockPassiveChannels; PtrInstance instance; PtrSpanList partitions; PtrSpanListList indices; PtrWire indexWire; PtrAccess access; PtrType expectedBaseType; tTree Lvalue; tTree AType; } yAsLvalue;
typedef struct { Tree_tNodeHead yyHead; tPosition position; PtrContext context; bool skip; ExprAttributes attributes; bool expectingChannel; bool expectingEither; bool isInput; bool isPassive; bool lockPassiveChannels; PtrInstance instance; PtrSpanList partitions; PtrSpanListList indices; PtrWire indexWire; PtrAccess access; PtrType expectedBaseType; tTree Lvalue; } yBitArrayCastLvalue;
typedef struct { Tree_tNodeHead yyHead; tPosition position; PtrContext context; bool skip; PtrExprAttributesList exprAttributes; PtrCommandAttributesList commandAttributes; } yGuards;
typedef struct { Tree_tNodeHead yyHead; tPosition position; PtrContext context; bool skip; PtrExprAttributesList exprAttributes; PtrCommandAttributesList commandAttributes; } yNullGuards;
typedef struct { Tree_tNodeHead yyHead; tPosition position; PtrContext context; bool skip; PtrExprAttributesList exprAttributes; PtrCommandAttributesList commandAttributes; tTree next; tTree Expr; tTree Command; } yGuard;
typedef struct { Tree_tNodeHead yyHead; tPosition position; PtrContext context; bool skip; ProcedureArgsType portsType; PtrInstanceList ports; unsigned portCount; PtrInstanceList extraPortInstances; bool hasParameters; } yPortGuards;
typedef struct { Tree_tNodeHead yyHead; tPosition position; PtrContext context; bool skip; ProcedureArgsType portsType; PtrInstanceList ports; unsigned portCount; PtrInstanceList extraPortInstances; bool hasParameters; } yNullPortGuards;
typedef struct { Tree_tNodeHead yyHead; tPosition position; PtrContext context; bool skip; ProcedureArgsType portsType; PtrInstanceList ports; unsigned portCount; PtrInstanceList extraPortInstances; bool hasParameters; tTree next; tTree Expr; tTree FormalPorts; } yPortGuard;
typedef struct { Tree_tNodeHead yyHead; tPosition position; Scope scope; PtrContext contextIn; PtrContext contextOut; bool skip; bool foundTrueGuard; } yDeclGuards;
typedef struct { Tree_tNodeHead yyHead; tPosition position; Scope scope; PtrContext contextIn; PtrContext contextOut; bool skip; bool foundTrueGuard; } yNullDeclGuards;
typedef struct { Tree_tNodeHead yyHead; tPosition position; Scope scope; PtrContext contextIn; PtrContext contextOut; bool skip; bool foundTrueGuard; tTree next; tTree Expr; tTree Decls; } yDeclGuard;
typedef struct { Tree_tNodeHead yyHead; tPosition position; PtrContext context; bool skip; PtrImplicantListList implicantss; PtrImplicantList complementImplicants; PtrCommandAttributesList commandAttributes; PtrType switchType; PtrMP_INT switchValue; SignedBits maxRange; PtrMP_INT implicantMask; bool foundTrueGuard; } yCaseGuards;
typedef struct { Tree_tNodeHead yyHead; tPosition position; PtrContext context; bool skip; PtrImplicantListList implicantss; PtrImplicantList complementImplicants; PtrCommandAttributesList commandAttributes; PtrType switchType; PtrMP_INT switchValue; SignedBits maxRange; PtrMP_INT implicantMask; bool foundTrueGuard; } yNullCaseGuards;
typedef struct { Tree_tNodeHead yyHead; tPosition position; PtrContext context; bool skip; PtrImplicantListList implicantss; PtrImplicantList complementImplicants; PtrCommandAttributesList commandAttributes; PtrType switchType; PtrMP_INT switchValue; SignedBits maxRange; PtrMP_INT implicantMask; bool foundTrueGuard; tTree next; } yCaseGuard;
typedef struct { Tree_tNodeHead yyHead; tPosition position; PtrContext context; bool skip; PtrImplicantListList implicantss; PtrImplicantList complementImplicants; PtrCommandAttributesList commandAttributes; PtrType switchType; PtrMP_INT switchValue; SignedBits maxRange; PtrMP_INT implicantMask; bool foundTrueGuard; tTree next; tTree CaseMatchs; tTree Command; } yCaseMatchGuard;
typedef struct { Tree_tNodeHead yyHead; tPosition position; PtrContext context; bool skip; PtrImplicantListList implicantss; PtrImplicantList complementImplicants; PtrCommandAttributesList commandAttributes; PtrType switchType; PtrMP_INT switchValue; SignedBits maxRange; PtrMP_INT implicantMask; bool foundTrueGuard; tTree next; tIdent ident; tTree CaseMatchs; tTree Command; PtrInstance iterator; PtrMP_INT countValue; int lastErrorCount; PtrImplicantList implicants; Ptrchar errorContextString; Ptrchar valueStringPtr; } yForCaseGuard;
typedef struct { Tree_tNodeHead yyHead; tPosition position; PtrContext context; bool skip; PtrExprAttributesList guardAttributes; PtrCommandAttributesList commandAttributes; } yChannelGuards;
typedef struct { Tree_tNodeHead yyHead; tPosition position; PtrContext context; bool skip; PtrExprAttributesList guardAttributes; PtrCommandAttributesList commandAttributes; } yNullChannelGuards;
typedef struct { Tree_tNodeHead yyHead; tPosition position; PtrContext context; bool skip; PtrExprAttributesList guardAttributes; PtrCommandAttributesList commandAttributes; tTree next; tTree LvalueLists; tTree Command; } yChannelGuard;
typedef struct { Tree_tNodeHead yyHead; tPosition position; PtrIdentList idents; } yIdents;
typedef struct { Tree_tNodeHead yyHead; tPosition position; PtrIdentList idents; } yNullIdents;
typedef struct { Tree_tNodeHead yyHead; tPosition position; PtrIdentList idents; tTree next; tIdent ident; } yIdent;
typedef struct { Tree_tNodeHead yyHead; tPosition position; PtrContext context; bool skip; PtrType expectedType; PtrInstanceList elements; PtrExprAttributesList attributes; bool expectConstants; } yExprLists;
typedef struct { Tree_tNodeHead yyHead; tPosition position; PtrContext context; bool skip; PtrType expectedType; PtrInstanceList elements; PtrExprAttributesList attributes; bool expectConstants; } yNullExprLists;
typedef struct { Tree_tNodeHead yyHead; tPosition position; PtrContext context; bool skip; PtrType expectedType; PtrInstanceList elements; PtrExprAttributesList attributes; bool expectConstants; tTree next; tTree CoercedExpr; } yExprList;
typedef struct { Tree_tNodeHead yyHead; tPosition position; PtrContext context; bool skip; PtrExprAttributesList attributes; PtrType expectedType; PtrInstanceList elements; PtrIdentList idents; bool isInput; bool isPassive; bool lockPassiveChannels; int isProcedurePorts; int allowArrayedChannels; } yLvalueLists;
typedef struct { Tree_tNodeHead yyHead; tPosition position; PtrContext context; bool skip; PtrExprAttributesList attributes; PtrType expectedType; PtrInstanceList elements; PtrIdentList idents; bool isInput; bool isPassive; bool lockPassiveChannels; int isProcedurePorts; int allowArrayedChannels; } yNullLvalueLists;
typedef struct { Tree_tNodeHead yyHead; tPosition position; PtrContext context; bool skip; PtrExprAttributesList attributes; PtrType expectedType; PtrInstanceList elements; PtrIdentList idents; bool isInput; bool isPassive; bool lockPassiveChannels; int isProcedurePorts; int allowArrayedChannels; tTree next; } yLvalueList;
typedef struct { Tree_tNodeHead yyHead; tPosition position; PtrContext context; bool skip; PtrExprAttributesList attributes; PtrType expectedType; PtrInstanceList elements; PtrIdentList idents; bool isInput; bool isPassive; bool lockPassiveChannels; int isProcedurePorts; int allowArrayedChannels; tTree next; tTree Block; } yLinkedBlock;
typedef struct { Tree_tNodeHead yyHead; tPosition position; PtrContext context; bool skip; PtrExprAttributesList attributes; PtrType expectedType; PtrInstanceList elements; PtrIdentList idents; bool isInput; bool isPassive; bool lockPassiveChannels; int isProcedurePorts; int allowArrayedChannels; tTree next; tIdent ident; tTree Lvalue; } yLinkedChannel;
typedef struct { Tree_tNodeHead yyHead; tPosition position; PtrContext context; bool skip; PtrImplicantList implicants; PtrType expectedType; bool allowOverlappingMatches; bool isSpan; } yCaseMatchs;
typedef struct { Tree_tNodeHead yyHead; tPosition position; PtrContext context; bool skip; PtrImplicantList implicants; PtrType expectedType; bool allowOverlappingMatches; bool isSpan; } yNullCaseMatchs;
typedef struct { Tree_tNodeHead yyHead; tPosition position; PtrContext context; bool skip; PtrImplicantList implicants; PtrType expectedType; bool allowOverlappingMatches; bool isSpan; tTree next; } yCaseMatch;
typedef struct { Tree_tNodeHead yyHead; tPosition position; PtrContext context; bool skip; PtrImplicantList implicants; PtrType expectedType; bool allowOverlappingMatches; bool isSpan; tTree next; tTree Range; } yCaseRange;
typedef struct { Tree_tNodeHead yyHead; tPosition position; PtrContext context; bool skip; PtrImplicantList implicants; PtrType expectedType; bool allowOverlappingMatches; bool isSpan; tTree next; tTree Expr; } yCaseImplicant;
typedef struct { Tree_tNodeHead yyHead; tPosition position; PtrContext context; bool skip; PtrMP_INT elementValue; PtrType typeIn; PtrType typeOut; bool hasOverType; PtrBindingList elementsTail; } yEnumElems;
typedef struct { Tree_tNodeHead yyHead; tPosition position; PtrContext context; bool skip; PtrMP_INT elementValue; PtrType typeIn; PtrType typeOut; bool hasOverType; PtrBindingList elementsTail; } yNullEnumElems;
typedef struct { Tree_tNodeHead yyHead; tPosition position; PtrContext context; bool skip; PtrMP_INT elementValue; PtrType typeIn; PtrType typeOut; bool hasOverType; PtrBindingList elementsTail; tTree next; tIdent ident; } yEnumElem;
typedef struct { Tree_tNodeHead yyHead; tPosition position; PtrContext context; bool skip; PtrMP_INT elementValue; PtrType typeIn; PtrType typeOut; bool hasOverType; PtrBindingList elementsTail; tTree next; tIdent ident; } yDefaultValuedEnumElem;
typedef struct { Tree_tNodeHead yyHead; tPosition position; PtrContext context; bool skip; PtrMP_INT elementValue; PtrType typeIn; PtrType typeOut; bool hasOverType; PtrBindingList elementsTail; tTree next; tIdent ident; tTree CoercedExpr; } yValuedEnumElem;
typedef struct { Tree_tNodeHead yyHead; tPosition position; PtrContext context; bool skip; PtrType typeIn; PtrType typeOut; bool hasOverType; PtrInstanceList elementsTail; } yRecordElems;
typedef struct { Tree_tNodeHead yyHead; tPosition position; PtrContext context; bool skip; PtrType typeIn; PtrType typeOut; bool hasOverType; PtrInstanceList elementsTail; } yNullRecordElems;
typedef struct { Tree_tNodeHead yyHead; tPosition position; PtrContext context; bool skip; PtrType typeIn; PtrType typeOut; bool hasOverType; PtrInstanceList elementsTail; tTree next; tTree Idents; tTree AType; } yRecordElem;
typedef struct { Tree_tNodeHead yyHead; tPosition position; PtrContext contextIn; PtrContext contextOut; bool skip; } yValDecls;
typedef struct { Tree_tNodeHead yyHead; tPosition position; PtrContext contextIn; PtrContext contextOut; bool skip; } yNullValDecls;
typedef struct { Tree_tNodeHead yyHead; tPosition position; PtrContext contextIn; PtrContext contextOut; bool skip; tTree next; tIdent ident; tTree Expr; } yValDecl;

union Tree_Node {
 Tree_tKind Kind;
 Tree_tNodeHead yyHead;
 yBalsaProgram BalsaProgram;
 yDecls Decls;
 yNullDecls NullDecls;
 yDecl Decl;
 yImportDecl ImportDecl;
 yFileDecl FileDecl;
 yTypeDecl TypeDecl;
 yConstantDecl ConstantDecl;
 yVariableDecl VariableDecl;
 yInitVariableDecl InitVariableDecl;
 yChannelDecl ChannelDecl;
 yChannelArrayDecl ChannelArrayDecl;
 ySyncDecl SyncDecl;
 ySyncArrayDecl SyncArrayDecl;
 yProcedureDecl ProcedureDecl;
 yProcAliasDecl ProcAliasDecl;
 yFunctionDecl FunctionDecl;
 yBuiltinFunctionDecl BuiltinFunctionDecl;
 ySharedDecl SharedDecl;
 yPartDecl PartDecl;
 yIfDecls IfDecls;
 yIfElseDecls IfElseDecls;
 yPrintDecl PrintDecl;
 yNullDecl NullDecl;
 yRange Range;
 ySpecifiedRange SpecifiedRange;
 yTypeRange TypeRange;
 yAType AType;
 yNullType NullType;
 yNumericType NumericType;
 yExistingType ExistingType;
 yBuiltinType BuiltinType;
 yArrayType ArrayType;
 yNewType NewType;
 yRecordType RecordType;
 yEnumType EnumType;
 yCoercedExpr CoercedExpr;
 yExpr Expr;
 yNullExpr NullExpr;
 yLiteralExpr LiteralExpr;
 yIdentExpr IdentExpr;
 yStringExpr StringExpr;
 yImplicantExpr ImplicantExpr;
 yDontCareExpr DontCareExpr;
 yAggregateConsExpr AggregateConsExpr;
 yNamedEnumElemExpr NamedEnumElemExpr;
 yUnaryExpr UnaryExpr;
 yBinaryExpr BinaryExpr;
 yRecordElemExtractExpr RecordElemExtractExpr;
 yArrayExtractExpr ArrayExtractExpr;
 yArraySliceExpr ArraySliceExpr;
 yAsExpr AsExpr;
 yBitArrayCastExpr BitArrayCastExpr;
 yLetExpr LetExpr;
 yFunctionCallExpr FunctionCallExpr;
 ySizeofExpr SizeofExpr;
 yArrayAppendExpr ArrayAppendExpr;
 yPrimedExpr PrimedExpr;
 yFormalPorts FormalPorts;
 yNullFormalPorts NullFormalPorts;
 yFormalPort FormalPort;
 yValuePort ValuePort;
 yParamPort ParamPort;
 yTypeParamPort TypeParamPort;
 yChannelPort ChannelPort;
 yChannelPortArray ChannelPortArray;
 ySyncPort SyncPort;
 ySyncPortArray SyncPortArray;
 yIfPorts IfPorts;
 yIfElsePorts IfElsePorts;
 yBlock Block;
 yParameters Parameters;
 yNullParameters NullParameters;
 yParameter Parameter;
 yNumberParameter NumberParameter;
 yStringParameter StringParameter;
 yTypeParameter TypeParameter;
 yBreezeParameters BreezeParameters;
 yNullBreezeParameters NullBreezeParameters;
 yBreezeParameter BreezeParameter;
 yBreezeExprParameter BreezeExprParameter;
 yBreezeTypeParameter BreezeTypeParameter;
 yComps Comps;
 yNullComps NullComps;
 yComp Comp;
 yNormalComp NormalComp;
 yUndeclaredComp UndeclaredComp;
 yCommand Command;
 yNullCommand NullCommand;
 yContinueCommand ContinueCommand;
 yHaltCommand HaltCommand;
 yInputCommand InputCommand;
 yInputEncloseCommand InputEncloseCommand;
 yInputEncloseBangCommand InputEncloseBangCommand;
 yOutputCommand OutputCommand;
 ySyncCommand SyncCommand;
 yAssignmentCommand AssignmentCommand;
 yBlockCommand BlockCommand;
 ySequentialCommand SequentialCommand;
 yParallelCommand ParallelCommand;
 yLoopCommand LoopCommand;
 yWhileGuardsCommand WhileGuardsCommand;
 yWhileGuardsAlsoCommand WhileGuardsAlsoCommand;
 yCommandWhileExprCommand CommandWhileExprCommand;
 yCommandWhileGuardsCommand CommandWhileGuardsCommand;
 yCommandWhileGuardsAlsoCommand CommandWhileGuardsAlsoCommand;
 yIfCommand IfCommand;
 yIfElseCommand IfElseCommand;
 yCaseCommand CaseCommand;
 yForCommand ForCommand;
 yProcedureCallCommonCommand ProcedureCallCommonCommand;
 ySelectCommand SelectCommand;
 ySelectBangCommand SelectBangCommand;
 yArbitrateCommand ArbitrateCommand;
 yPrintCommand PrintCommand;
 ySinkCommand SinkCommand;
 yProcedureParams ProcedureParams;
 yNullProcParams NullProcParams;
 yProcParam ProcParam;
 yExprProcParam ExprProcParam;
 yTypeProcParam TypeProcParam;
 yVarReadProcParam VarReadProcParam;
 yVarWriteProcParam VarWriteProcParam;
 yBlockProcParam BlockProcParam;
 yFunctionParams FunctionParams;
 yNullFuncParams NullFuncParams;
 yFuncParam FuncParam;
 yExprFuncParam ExprFuncParam;
 yTypeFuncParam TypeFuncParam;
 yLvalue Lvalue;
 yIdentLvalue IdentLvalue;
 yRecordElemLvalue RecordElemLvalue;
 yArrayElemLvalue ArrayElemLvalue;
 yArraySliceLvalue ArraySliceLvalue;
 yArrayAppendLvalue ArrayAppendLvalue;
 yArrayConsLvalue ArrayConsLvalue;
 yAsLvalue AsLvalue;
 yBitArrayCastLvalue BitArrayCastLvalue;
 yGuards Guards;
 yNullGuards NullGuards;
 yGuard Guard;
 yPortGuards PortGuards;
 yNullPortGuards NullPortGuards;
 yPortGuard PortGuard;
 yDeclGuards DeclGuards;
 yNullDeclGuards NullDeclGuards;
 yDeclGuard DeclGuard;
 yCaseGuards CaseGuards;
 yNullCaseGuards NullCaseGuards;
 yCaseGuard CaseGuard;
 yCaseMatchGuard CaseMatchGuard;
 yForCaseGuard ForCaseGuard;
 yChannelGuards ChannelGuards;
 yNullChannelGuards NullChannelGuards;
 yChannelGuard ChannelGuard;
 yIdents Idents;
 yNullIdents NullIdents;
 yIdent Ident;
 yExprLists ExprLists;
 yNullExprLists NullExprLists;
 yExprList ExprList;
 yLvalueLists LvalueLists;
 yNullLvalueLists NullLvalueLists;
 yLvalueList LvalueList;
 yLinkedBlock LinkedBlock;
 yLinkedChannel LinkedChannel;
 yCaseMatchs CaseMatchs;
 yNullCaseMatchs NullCaseMatchs;
 yCaseMatch CaseMatch;
 yCaseRange CaseRange;
 yCaseImplicant CaseImplicant;
 yEnumElems EnumElems;
 yNullEnumElems NullEnumElems;
 yEnumElem EnumElem;
 yDefaultValuedEnumElem DefaultValuedEnumElem;
 yValuedEnumElem ValuedEnumElem;
 yRecordElems RecordElems;
 yNullRecordElems NullRecordElems;
 yRecordElem RecordElem;
 yValDecls ValDecls;
 yNullValDecls NullValDecls;
 yValDecl ValDecl;
};

extern tTree TreeRoot;
extern unsigned long Tree_HeapUsed;
extern char * Tree_PoolFreePtr, * Tree_PoolMaxPtr;
extern unsigned short Tree_NodeSize [182 + 1];
extern char * Tree_NodeName [182 + 1];

extern void (* Tree_Exit) ();
extern tTree Tree_Alloc ();
extern tTree MakeTree ARGS((Tree_tKind yyKind));
extern bool Tree_IsType ARGS((register tTree yyt, register Tree_tKind yyKind));

extern tTree mBalsaProgram ARGS((tTree pDecls));
extern tTree mDecls ARGS((tPosition pposition));
extern tTree mNullDecls ARGS((tPosition pposition));
extern tTree mDecl ARGS((tPosition pposition, tTree pnext));
extern tTree mImportDecl ARGS((tPosition pposition, tTree pnext, tTree pIdents));
extern tTree mFileDecl ARGS((tPosition pposition, tTree pnext, tTree pIdents));
extern tTree mTypeDecl ARGS((tPosition pposition, tTree pnext, tIdent pident, tTree pAType));
extern tTree mConstantDecl ARGS((tPosition pposition, tTree pnext, tIdent pident, tTree pCoercedExpr, tTree pAType));
extern tTree mVariableDecl ARGS((tPosition pposition, tTree pnext, tTree pIdents, tTree pAType));
extern tTree mInitVariableDecl ARGS((tPosition pposition, tTree pnext, tTree pIdents, tTree pCoercedExpr, tTree pAType));
extern tTree mChannelDecl ARGS((tPosition pposition, tTree pnext, tTree pIdents, tTree pAType, bool pmulticast));
extern tTree mChannelArrayDecl ARGS((tPosition pposition, tTree pnext, tTree pIdents, tTree pAType, tTree pRange, bool pmulticast));
extern tTree mSyncDecl ARGS((tPosition pposition, tTree pnext, tTree pIdents, bool pmulticast));
extern tTree mSyncArrayDecl ARGS((tPosition pposition, tTree pnext, tTree pIdents, tTree pRange, bool pmulticast));
extern tTree mProcedureDecl ARGS((tPosition pposition, tTree pnext, tIdent pident, tTree pFormalPorts, tTree pBlock));
extern tTree mProcAliasDecl ARGS((tPosition pposition, tTree pnext, tIdent pnewName, tIdent poldName, tTree pProcedureParams));
extern tTree mFunctionDecl ARGS((tPosition pposition, tTree pnext, tIdent pident, tTree pFormalPorts, tTree pCoercedExpr, tTree pAType));
extern tTree mBuiltinFunctionDecl ARGS((tPosition pposition, tTree pnext, tIdent pident, tTree pFormalPorts, tTree pAType));
extern tTree mSharedDecl ARGS((tPosition pposition, tTree pnext, tIdent pident, tTree pBlock));
extern tTree mPartDecl ARGS((tPosition pposition, tTree pnext, tIdent pident, PtrLispList pattributes, tTree pFormalPorts, PtrWireArray pchannels, unsigned pchannelCount, tTree pComps, PtrCallContextList pcallcontexts));
extern tTree mIfDecls ARGS((tPosition pposition, tTree pnext, tTree pDeclGuards));
extern tTree mIfElseDecls ARGS((tPosition pposition, tTree pnext, tTree pDeclGuards, tTree pDecls));
extern tTree mPrintDecl ARGS((tPosition pposition, tTree pnext, tTree pExpr, tTree pExprLists));
extern tTree mNullDecl ARGS((tPosition pposition, tTree pnext));
extern tTree mRange ARGS((tPosition pposition));
extern tTree mSpecifiedRange ARGS((tPosition pposition, tTree pLeft, tTree pRight));
extern tTree mTypeRange ARGS((tPosition pposition, tTree pAType));
extern tTree mAType ARGS((tPosition pposition));
extern tTree mNullType ARGS((tPosition pposition));
extern tTree mNumericType ARGS((tPosition pposition, bool psignedness, tTree pExpr));
extern tTree mExistingType ARGS((tPosition pposition, tIdent pident));
extern tTree mBuiltinType ARGS((tPosition pposition));
extern tTree mArrayType ARGS((tPosition pposition, tTree pAType, tTree pRange));
extern tTree mNewType ARGS((tPosition pposition, tTree pAType));
extern tTree mRecordType ARGS((tPosition pposition, tTree pRecordElems, tTree pAType));
extern tTree mEnumType ARGS((tPosition pposition, tTree pEnumElems, tTree pAType));
extern tTree mCoercedExpr ARGS((tTree pExpr));
extern tTree mExpr ARGS((tPosition pposition));
extern tTree mNullExpr ARGS((tPosition pposition));
extern tTree mLiteralExpr ARGS((tPosition pposition, PtrMP_INT pliteral));
extern tTree mIdentExpr ARGS((tPosition pposition, tIdent pident));
extern tTree mStringExpr ARGS((tPosition pposition, Ptrconstchar pstring));
extern tTree mImplicantExpr ARGS((tPosition pposition, Implicant pimplicant));
extern tTree mDontCareExpr ARGS((tPosition pposition));
extern tTree mAggregateConsExpr ARGS((tPosition pposition, tIdent pident, tTree pExprLists));
extern tTree mNamedEnumElemExpr ARGS((tPosition pposition, tIdent ptypeName, tIdent pelemName));
extern tTree mUnaryExpr ARGS((tPosition pposition, Operators poperation, tTree pExpr));
extern tTree mBinaryExpr ARGS((tPosition pposition, Operators poperation, tTree pLeft, tTree pRight));
extern tTree mRecordElemExtractExpr ARGS((tPosition pposition, tTree pExpr, tIdent pident));
extern tTree mArrayExtractExpr ARGS((tPosition pposition, tTree pArray, tTree pSubscript));
extern tTree mArraySliceExpr ARGS((tPosition pposition, tTree pExpr, tTree pRange));
extern tTree mAsExpr ARGS((tPosition pposition, tTree pExpr, tTree pAType));
extern tTree mBitArrayCastExpr ARGS((tPosition pposition, tTree pExpr));
extern tTree mLetExpr ARGS((tPosition pposition, tTree pValDecls, tTree pExpr));
extern tTree mFunctionCallExpr ARGS((tPosition pposition, tIdent pident, tTree pFunctionParams));
extern tTree mSizeofExpr ARGS((tPosition pposition, tIdent pident));
extern tTree mArrayAppendExpr ARGS((tPosition pposition, tTree pLeft, tTree pRight));
extern tTree mPrimedExpr ARGS((tPosition pposition, ExprAttributes presults));
extern tTree mFormalPorts ARGS((tPosition pposition));
extern tTree mNullFormalPorts ARGS((tPosition pposition));
extern tTree mFormalPort ARGS((tPosition pposition, tTree pnext));
extern tTree mValuePort ARGS((tPosition pposition, tTree pnext, tTree pIdents, tTree pAType));
extern tTree mParamPort ARGS((tPosition pposition, tTree pnext, tTree pIdents, tTree pAType));
extern tTree mTypeParamPort ARGS((tPosition pposition, tTree pnext, tTree pIdents));
extern tTree mChannelPort ARGS((tPosition pposition, tTree pnext, tTree pIdents, tTree pAType, PortSense pportSense, bool pisOutput, PtrLispList poptions));
extern tTree mChannelPortArray ARGS((tPosition pposition, tTree pnext, tTree pIdents, tTree pAType, PortSense pportSense, bool pisOutput, tTree pRange, PtrLispList poptions));
extern tTree mSyncPort ARGS((tPosition pposition, tTree pnext, tTree pIdents, PortSense pportSense, PtrLispList poptions));
extern tTree mSyncPortArray ARGS((tPosition pposition, tTree pnext, tTree pIdents, PortSense pportSense, tTree pRange, PtrLispList poptions));
extern tTree mIfPorts ARGS((tPosition pposition, tTree pnext, tTree pPortGuards));
extern tTree mIfElsePorts ARGS((tPosition pposition, tTree pnext, tTree pPortGuards, tTree pFormalPorts));
extern tTree mBlock ARGS((tPosition pposition, tTree pDecls, tTree pCommand));
extern tTree mParameters ARGS((tPosition pposition));
extern tTree mNullParameters ARGS((tPosition pposition));
extern tTree mParameter ARGS((tPosition pposition, tTree pnext));
extern tTree mNumberParameter ARGS((tPosition pposition, tTree pnext, PtrMP_INT pliteral));
extern tTree mStringParameter ARGS((tPosition pposition, tTree pnext, tIdent pident));
extern tTree mTypeParameter ARGS((tPosition pposition, tTree pnext, tTree pAType));
extern tTree mBreezeParameters ARGS((tPosition pposition));
extern tTree mNullBreezeParameters ARGS((tPosition pposition));
extern tTree mBreezeParameter ARGS((tPosition pposition, tTree pnext));
extern tTree mBreezeExprParameter ARGS((tPosition pposition, tTree pnext, tIdent pident, tTree pAType));
extern tTree mBreezeTypeParameter ARGS((tPosition pposition, tTree pnext, tIdent pident));
extern tTree mComps ARGS((tPosition pposition));
extern tTree mNullComps ARGS((tPosition pposition));
extern tTree mComp ARGS((tPosition pposition, tTree pnext));
extern tTree mNormalComp ARGS((tPosition pposition, tTree pnext, tIdent pident, bool pinternal, tTree pParameters, PtrintList pchannelNos, PtrLispList poptions));
extern tTree mUndeclaredComp ARGS((tPosition pposition, tTree pnext, tIdent pident, tTree pParameters, PtrintList pchannelNos, tIdent pcomponentType, tIdent pbaseComponentName, tTree pBreezeParameters, tTree pFormalPorts, PtrLispList poptions));
extern tTree mCommand ARGS((tPosition pposition));
extern tTree mNullCommand ARGS((tPosition pposition));
extern tTree mContinueCommand ARGS((tPosition pposition));
extern tTree mHaltCommand ARGS((tPosition pposition));
extern tTree mInputCommand ARGS((tPosition pposition, tTree pLHS, tTree pRHS));
extern tTree mInputEncloseCommand ARGS((tPosition pposition, tTree pLvalueLists, tTree pCommand));
extern tTree mInputEncloseBangCommand ARGS((tPosition pposition, tTree pLvalueLists, tTree pCommand));
extern tTree mOutputCommand ARGS((tPosition pposition, tTree pLvalue, tTree pCoercedExpr));
extern tTree mSyncCommand ARGS((tPosition pposition, tTree pLvalue));
extern tTree mAssignmentCommand ARGS((tPosition pposition, tTree pLvalue, tTree pCoercedExpr));
extern tTree mBlockCommand ARGS((tPosition pposition, tTree pBlock));
extern tTree mSequentialCommand ARGS((tPosition pposition, tTree pLeft, tTree pRight));
extern tTree mParallelCommand ARGS((tPosition pposition, bool pisPermissive, tTree pLeft, tTree pRight));
extern tTree mLoopCommand ARGS((tPosition pposition, tTree pCommand));
extern tTree mWhileGuardsCommand ARGS((tPosition pposition, tTree pGuards));
extern tTree mWhileGuardsAlsoCommand ARGS((tPosition pposition, tTree pGuards, tTree pCommand));
extern tTree mCommandWhileExprCommand ARGS((tPosition pposition, tTree pCommand, tTree pExpr));
extern tTree mCommandWhileGuardsCommand ARGS((tPosition pposition, tTree pCommand, tTree pGuards));
extern tTree mCommandWhileGuardsAlsoCommand ARGS((tPosition pposition, tTree pFirstCommand, tTree pGuards, tTree pAlsoCommand));
extern tTree mIfCommand ARGS((tPosition pposition, tTree pGuards));
extern tTree mIfElseCommand ARGS((tPosition pposition, tTree pGuards, tTree pCommand));
extern tTree mCaseCommand ARGS((tPosition pposition, tTree pExpr, tTree pCaseGuards, tTree pCommand));
extern tTree mForCommand ARGS((tPosition pposition, bool pisParallel, bool pisPermissive, tIdent pident, tTree pRange, tTree pCommand));
extern tTree mProcedureCallCommonCommand ARGS((tPosition pposition, tIdent pident, tTree pProcedureParams));
extern tTree mSelectCommand ARGS((tPosition pposition, tTree pChannelGuards));
extern tTree mSelectBangCommand ARGS((tPosition pposition, tTree pChannelGuards));
extern tTree mArbitrateCommand ARGS((tPosition pposition, tTree pChannelGuards));
extern tTree mPrintCommand ARGS((tPosition pposition, tTree pExpr, tTree pExprLists));
extern tTree mSinkCommand ARGS((tPosition pposition, tTree pExpr));
extern tTree mProcedureParams ARGS((tPosition pposition));
extern tTree mNullProcParams ARGS((tPosition pposition));
extern tTree mProcParam ARGS((tPosition pposition, tTree pnext));
extern tTree mExprProcParam ARGS((tPosition pposition, tTree pnext, tTree pCoercedExpr));
extern tTree mTypeProcParam ARGS((tPosition pposition, tTree pnext, tTree pAType));
extern tTree mVarReadProcParam ARGS((tPosition pposition, tTree pnext, tTree pCoercedExpr));
extern tTree mVarWriteProcParam ARGS((tPosition pposition, tTree pnext, tTree pLvalue));
extern tTree mBlockProcParam ARGS((tPosition pposition, tTree pnext, tTree pBlock));
extern tTree mFunctionParams ARGS((tPosition pposition));
extern tTree mNullFuncParams ARGS((tPosition pposition));
extern tTree mFuncParam ARGS((tPosition pposition, tTree pnext));
extern tTree mExprFuncParam ARGS((tPosition pposition, tTree pnext, tTree pCoercedExpr));
extern tTree mTypeFuncParam ARGS((tPosition pposition, tTree pnext, tTree pAType));
extern tTree mLvalue ARGS((tPosition pposition));
extern tTree mIdentLvalue ARGS((tPosition pposition, tIdent pident));
extern tTree mRecordElemLvalue ARGS((tPosition pposition, tTree pLvalue, tIdent pident));
extern tTree mArrayElemLvalue ARGS((tPosition pposition, tTree pLvalue, tTree pCoercedExpr));
extern tTree mArraySliceLvalue ARGS((tPosition pposition, tTree pLvalue, tTree pRange));
extern tTree mArrayAppendLvalue ARGS((tPosition pposition, tTree pLeft, tTree pRight));
extern tTree mArrayConsLvalue ARGS((tPosition pposition, tTree pLvalueLists));
extern tTree mAsLvalue ARGS((tPosition pposition, tTree pLvalue, tTree pAType));
extern tTree mBitArrayCastLvalue ARGS((tPosition pposition, tTree pLvalue));
extern tTree mGuards ARGS((tPosition pposition));
extern tTree mNullGuards ARGS((tPosition pposition));
extern tTree mGuard ARGS((tPosition pposition, tTree pnext, tTree pExpr, tTree pCommand));
extern tTree mPortGuards ARGS((tPosition pposition));
extern tTree mNullPortGuards ARGS((tPosition pposition));
extern tTree mPortGuard ARGS((tPosition pposition, tTree pnext, tTree pExpr, tTree pFormalPorts));
extern tTree mDeclGuards ARGS((tPosition pposition));
extern tTree mNullDeclGuards ARGS((tPosition pposition));
extern tTree mDeclGuard ARGS((tPosition pposition, tTree pnext, tTree pExpr, tTree pDecls));
extern tTree mCaseGuards ARGS((tPosition pposition));
extern tTree mNullCaseGuards ARGS((tPosition pposition));
extern tTree mCaseGuard ARGS((tPosition pposition, tTree pnext));
extern tTree mCaseMatchGuard ARGS((tPosition pposition, tTree pnext, tTree pCaseMatchs, tTree pCommand));
extern tTree mForCaseGuard ARGS((tPosition pposition, tTree pnext, tIdent pident, tTree pCaseMatchs, tTree pCommand));
extern tTree mChannelGuards ARGS((tPosition pposition));
extern tTree mNullChannelGuards ARGS((tPosition pposition));
extern tTree mChannelGuard ARGS((tPosition pposition, tTree pnext, tTree pLvalueLists, tTree pCommand));
extern tTree mIdents ARGS((tPosition pposition));
extern tTree mNullIdents ARGS((tPosition pposition));
extern tTree mIdent ARGS((tPosition pposition, tTree pnext, tIdent pident));
extern tTree mExprLists ARGS((tPosition pposition));
extern tTree mNullExprLists ARGS((tPosition pposition));
extern tTree mExprList ARGS((tPosition pposition, tTree pnext, tTree pCoercedExpr));
extern tTree mLvalueLists ARGS((tPosition pposition));
extern tTree mNullLvalueLists ARGS((tPosition pposition));
extern tTree mLvalueList ARGS((tPosition pposition, tTree pnext));
extern tTree mLinkedBlock ARGS((tPosition pposition, tTree pnext, tTree pBlock));
extern tTree mLinkedChannel ARGS((tPosition pposition, tTree pnext, tIdent pident, tTree pLvalue));
extern tTree mCaseMatchs ARGS((tPosition pposition));
extern tTree mNullCaseMatchs ARGS((tPosition pposition));
extern tTree mCaseMatch ARGS((tPosition pposition, tTree pnext));
extern tTree mCaseRange ARGS((tPosition pposition, tTree pnext, tTree pRange));
extern tTree mCaseImplicant ARGS((tPosition pposition, tTree pnext, tTree pExpr));
extern tTree mEnumElems ARGS((tPosition pposition));
extern tTree mNullEnumElems ARGS((tPosition pposition));
extern tTree mEnumElem ARGS((tPosition pposition, tTree pnext, tIdent pident));
extern tTree mDefaultValuedEnumElem ARGS((tPosition pposition, tTree pnext, tIdent pident));
extern tTree mValuedEnumElem ARGS((tPosition pposition, tTree pnext, tIdent pident, tTree pCoercedExpr));
extern tTree mRecordElems ARGS((tPosition pposition));
extern tTree mNullRecordElems ARGS((tPosition pposition));
extern tTree mRecordElem ARGS((tPosition pposition, tTree pnext, tTree pIdents, tTree pAType));
extern tTree mValDecls ARGS((tPosition pposition));
extern tTree mNullValDecls ARGS((tPosition pposition));
extern tTree mValDecl ARGS((tPosition pposition, tTree pnext, tIdent pident, tTree pExpr));

extern tTree ReverseTree ARGS((tTree yyt));
extern void ForallTree ARGS((tTree yyt, Tree_tProcTree yyProc));
extern tTree CopyTree ARGS((tTree yyt));
extern void BeginTree ();
extern void CloseTree ();

# endif
