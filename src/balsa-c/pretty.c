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

	`pretty.c'
	KCT abstract syntax tree pretty functions to generate
	Balsa source in lispy Breeze
	
 */

#include "pretty.h"
#include "operators.h"
#include "BalsaScanSource.h"

/* Output stream for pretty printing */
FILE *PrettyPrintStream = NULL;

/* NL,QUOTEDSTR,STR,IDENT,TABS,MP,ERROR: macros for printing various types to
	PrettyPrintStream */
/*	Apologies for the horribly unstructured nature of these but they make this file
	*much* smaller */
#define STR(str) fprintf (PrettyPrintStream, "%s", (str))
#define QUOTEDSTR(str) CHR ('"'); fprintf (PrettyPrintStream, "%s", (str)); CHR ('"')
#define CHR(chr) putc ((chr), PrettyPrintStream)
#define NL CHR ('\n')
#define SPACE CHR (' ')
#define OPEN CHR ('(')
#define CLOSE CHR (')')
#define IDENT(id) CHR ('"'); WriteIdent (PrettyPrintStream, (id)); CHR ('"')
#define TABS(count) PrettyPrintTabs (count)
#define MP(val) StrPtrMP_INT (PrettyPrintStream, (val))
#define IMPLICANT(val) StrImplicant (PrettyPrintStream, (val), true)
#define NODE(node,tabs) PrettyPrintNode ((node), tabCount + (tabs));

/* KEYWORD : This macro relies on the variable `tree' being local and on *all* non-terminals
	in the attribute grammar having a position parameter at the same place as Decls (which is
	an arbitrary choice of nonterminal) does. *Never* use this macro in a context where
	its multi-statement nature might be a problem */
#define KEYWORD(str) STR (str); SPACE; StrSBreezePosition (PrettyPrintStream, tree->Decls.position); SPACE;

#define ERROR fprintf (stderr, "Ouch! %s %d\n", __func__ , __LINE__)

/* PrettyPrintTabs: print `count' tabs.  A tab is set as two spaces */
void PrettyPrintTabs (unsigned count)
{
    putc ('\n', PrettyPrintStream);
    while (count > 0)
    {
        fprintf (PrettyPrintStream, "  ");
        count--;
    }
}

/* PrettyPrintPortSense : write out the port sense default, active, passive */
void PrettyPrintPortSense (FILE * stream, PortSense sense)
{
    switch (sense)
    {
    case DefaultPortSense:
        fprintf (stream, "default");
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

/* PrettyPrintProcedureDecl : print a procedure declaration */
void PrettyPrintProcedureDecl (tTree tree, unsigned tabCount)
{
    KEYWORD ("procedure-decl");
    IDENT (tree->ProcedureDecl.ident);
    TABS (tabCount + 1);
    NODE (tree->ProcedureDecl.FormalPorts, 1);
    TABS (tabCount + 1);
    NODE (tree->ProcedureDecl.Block, 1);
    TABS (tabCount);
}

/* PrettyPrintFunctionDecl : print a function declaration */
void PrettyPrintFunctionDecl (tTree tree, unsigned tabCount)
{
    bool typed = !Tree_IsType (tree->FunctionDecl.AType, kNullType);

    KEYWORD ((typed ? "typed-function-decl" : "untyped-function-decl"));
    IDENT (tree->FunctionDecl.ident);
    TABS (tabCount + 1);
    NODE (tree->FunctionDecl.FormalPorts, 1);
    TABS (tabCount + 1);
    NODE (tree->FunctionDecl.CoercedExpr, 1);
    if (typed)
    {
        TABS (tabCount + 1);
        NODE (tree->FunctionDecl.AType, 1);
    }
    TABS (tabCount);
}

/* PrettyPrintBuiltinFunctionDecl : print a builtin function declaration */
void PrettyPrintBuiltinFunctionDecl (tTree tree, unsigned tabCount)
{
    KEYWORD ("builtin-function-decl");
    IDENT (tree->BuiltinFunctionDecl.ident);
    TABS (tabCount + 1);
    NODE (tree->BuiltinFunctionDecl.FormalPorts, 1);
    TABS (tabCount + 1);
    NODE (tree->BuiltinFunctionDecl.AType, 1);
    TABS (tabCount);
}

/***** KCT Tree printing functions */

static void PrettyPrintBalsaProgram (tTree tree, unsigned tabCount)
{
    KEYWORD ("import");
    TABS (tabCount + 1);
    NODE (tree->BalsaProgram.Decls, 1);
}

/* PrettyPrintListNode : print list type node elements. `newline' specifies whether a newline
	and indentation should be used to separate list items (true) or just a space (false) */
static void PrettyPrintListNode (bool newline, tTree tree, unsigned tabCount)
{
#define NEXT(elemName) tree = (Tree_IsType (tree->elemName.next, kNull##elemName##s) \
	? NULL : tree->elemName.next)

    if (newline)
    {
        TABS (tabCount);
        tabCount++;
    }

    while (tree)
    {
        if (newline)
        {
            STR ("  ");
        }
        /* Add an extra indent */
        if (tree->Kind == kExprList) /* Let ExprList elements be `bare' */
        {
            NODE (tree->ExprList.CoercedExpr, 0); /* ironically, NODE will strip the CoercedExpr too */
            NEXT (ExprList);
        } else
        {
            OPEN;
            switch (tree->Kind)
            {
            case kConstantDecl:
                {
                    bool typed = !Tree_IsType (tree->ConstantDecl.AType, kNullType);

                    KEYWORD ((typed ? "typed-constant-decl" : "untyped-constant-decl"));
                    IDENT (tree->ConstantDecl.ident);
                    SPACE;
                    NODE (tree->ConstantDecl.CoercedExpr, 1);
                    if (typed)
                    {
                        SPACE;
                        NODE (tree->ConstantDecl.AType, 1);
                    }
                    NEXT (Decl);
                }
                break;
            case kVariableDecl:
                KEYWORD ("variable-decl");
                NODE (tree->VariableDecl.Idents, 1);
                SPACE;
                NODE (tree->VariableDecl.AType, 1);
                NEXT (Decl);
                break;
            case kInitVariableDecl:
                KEYWORD ("init-variable-decl");
                NODE (tree->InitVariableDecl.Idents, 1);
                SPACE;
                NODE (tree->InitVariableDecl.CoercedExpr, 1);
                SPACE;
                NODE (tree->InitVariableDecl.AType, 1);
                NEXT (Decl);
                break;
            case kChannelDecl:
                KEYWORD ("channel-decl");
                NODE (tree->ChannelDecl.Idents, 1);
                SPACE;
                NODE (tree->ChannelDecl.AType, 1);
                if (tree->ChannelDecl.multicast)
                {
                    SPACE;
                    STR ("multicast");
                }
                NEXT (Decl);
                break;
            case kChannelArrayDecl:
                KEYWORD ("arrayed-channel-decl");
                TABS (tabCount + 1);
                NODE (tree->ChannelArrayDecl.Idents, 1);
                SPACE;
                NODE (tree->ChannelArrayDecl.Range, 1);
                SPACE;
                NODE (tree->ChannelArrayDecl.AType, 1);
                if (tree->ChannelArrayDecl.multicast)
                {
                    SPACE;
                    STR ("multicast");
                }
                NEXT (Decl);
                break;
            case kSyncDecl:
                KEYWORD ("sync-decl");
                NODE (tree->SyncDecl.Idents, 1);
                if (tree->SyncDecl.multicast)
                {
                    SPACE;
                    STR ("multicast");
                }
                NEXT (Decl);
                break;
            case kSyncArrayDecl:
                KEYWORD ("arrayed-sync-decl");
                NODE (tree->SyncArrayDecl.Idents, 1);
                SPACE;
                NODE (tree->SyncArrayDecl.Range, 1);
                if (tree->SyncArrayDecl.multicast)
                {
                    SPACE;
                    STR ("multicast");
                }
                NEXT (Decl);
                break;
            case kProcedureDecl:
                PrettyPrintProcedureDecl (tree, tabCount); /* Special, need the function */
                NEXT (Decl);
                break;
            case kProcAliasDecl:
                {
                    bool hasParameters = !Tree_IsType (tree->ProcAliasDecl.ProcedureParams,
                      kNullProcParams);

                    KEYWORD ((hasParameters ? "procedure-param-alias-decl" : "procedure-alias-decl"));
                    IDENT (tree->ProcAliasDecl.newName);
                    SPACE;
                    IDENT (tree->ProcAliasDecl.oldName);
                    if (hasParameters)
                    {
                        SPACE;
                        NODE (tree->ProcAliasDecl.ProcedureParams, 1);
                    }
                }
                NEXT (Decl);
                break;
            case kFunctionDecl:
                PrettyPrintFunctionDecl (tree, tabCount);
                NEXT (Decl);
                break;
            case kBuiltinFunctionDecl:
                PrettyPrintBuiltinFunctionDecl (tree, tabCount);
                NEXT (Decl);
                break;
            case kSharedDecl:
                KEYWORD ("shared-decl");
                IDENT (tree->SharedDecl.ident);
                SPACE;
                NODE (tree->SharedDecl.Block, 1);
                NEXT (Decl);
                break;
            case kImportDecl:
                KEYWORD ("import-decl");
                NODE (tree->ImportDecl.Idents, 1);
                NEXT (Decl);
                break;
            case kTypeDecl:
                KEYWORD ("type-decl");
                IDENT (tree->TypeDecl.ident);
                SPACE;
                NODE (tree->TypeDecl.AType, 1);
                NEXT (Decl);
                break;
            case kPartDecl:
                /* Do nothing */
                NEXT (Decl);
                break;
            case kIfDecls:
                KEYWORD ("if-decls");
                TABS (tabCount + 1);
                NODE (tree->IfDecls.DeclGuards, 2);
                TABS (tabCount);
                NEXT (Decl);
                break;
            case kIfElseDecls:
                KEYWORD ("if-else-decls");
                TABS (tabCount + 1);
                NODE (tree->IfElseDecls.DeclGuards, 2);
                TABS (tabCount + 1);
                NODE (tree->IfElseDecls.Decls, 1);
                TABS (tabCount);
                NEXT (Decl);
                break;
            case kPrintDecl:
                KEYWORD ("print-decl");
                NODE (tree->PrintDecl.Expr, 1);
                SPACE;
                NODE (tree->PrintDecl.ExprLists, 1);
                TABS (tabCount);
                NEXT (Decl);
                break;
            case kValuePort:
                KEYWORD ("value-port");
                NODE (tree->ValuePort.Idents, 1);
                SPACE;
                NODE (tree->ValuePort.AType, 1);
                NEXT (FormalPort);
                break;
            case kParamPort:
                KEYWORD ("param-port");
                NODE (tree->ParamPort.Idents, 1);
                SPACE;
                NODE (tree->ParamPort.AType, 1);
                NEXT (FormalPort);
                break;
            case kTypeParamPort:
                KEYWORD ("type-param-port");
                NODE (tree->TypeParamPort.Idents, 1);
                NEXT (FormalPort);
                break;
            case kIfPorts:
                KEYWORD ("if-ports");
                TABS (tabCount + 1);
                NODE (tree->IfPorts.PortGuards, 2);
                TABS (tabCount);
                NEXT (FormalPort);
                break;
            case kIfElsePorts:
                KEYWORD ("if-else-ports");
                TABS (tabCount + 1);
                NODE (tree->IfElsePorts.PortGuards, 2);
                TABS (tabCount + 1);
                NODE (tree->IfElsePorts.FormalPorts, 1);
                TABS (tabCount);
                NEXT (FormalPort);
                break;
            case kChannelPort:
                KEYWORD ("port");
                PrettyPrintPortSense (PrettyPrintStream, tree->ChannelPort.portSense);
                SPACE;
                STR ((tree->ChannelPort.isOutput ? "output" : "input"));
                SPACE;
                NODE (tree->ChannelPort.Idents, 1);
                SPACE;
                NODE (tree->ChannelPort.AType, 1);
                NEXT (FormalPort);
                break;
            case kChannelPortArray:
                KEYWORD ("arrayed-port");
                PrettyPrintPortSense (PrettyPrintStream, tree->ChannelPortArray.portSense);
                SPACE;
                STR ((tree->ChannelPortArray.isOutput ? "output" : "input"));
                SPACE;
                NODE (tree->ChannelPortArray.Idents, 1);
                SPACE;
                NODE (tree->ChannelPortArray.Range, 1);
                SPACE;
                NODE (tree->ChannelPortArray.AType, 1);
                NEXT (FormalPort);
                break;
            case kSyncPort:
                KEYWORD ("sync-port");
                PrettyPrintPortSense (PrettyPrintStream, tree->SyncPort.portSense);
                SPACE;
                NODE (tree->SyncPort.Idents, 1);
                NEXT (FormalPort);
                break;
            case kSyncPortArray:
                KEYWORD ("arrayed-sync-port");
                PrettyPrintPortSense (PrettyPrintStream, tree->SyncPortArray.portSense);
                SPACE;
                NODE (tree->SyncPortArray.Idents, 1);
                SPACE;
                NODE (tree->SyncPortArray.Range, 1);
                NEXT (FormalPort);
                break;
            case kExprProcParam:
                KEYWORD ("expr-proc-param");
                NODE (tree->ExprProcParam.CoercedExpr, 0);
                NEXT (ProcParam);
                break;
            case kTypeProcParam:
                KEYWORD ("type-proc-param");
                NODE (tree->TypeProcParam.AType, 0);
                NEXT (ProcParam);
                break;
            case kExprFuncParam:
                KEYWORD ("expr-func-param");
                NODE (tree->ExprFuncParam.CoercedExpr, 0);
                NEXT (FuncParam);
                break;
            case kTypeFuncParam:
                KEYWORD ("type-func-param");
                NODE (tree->TypeFuncParam.AType, 0);
                NEXT (FuncParam);
                break;
            case kVarReadProcParam:
                KEYWORD ("var-read-proc-param");
                NODE (tree->VarReadProcParam.CoercedExpr, 0);
                NEXT (ProcParam);
                break;
            case kVarWriteProcParam:
                KEYWORD ("var-write-proc-param");
                NODE (tree->VarWriteProcParam.Lvalue, 0);
                NEXT (ProcParam);
                break;
            case kBlockProcParam:
                KEYWORD ("block-proc-param");
                NODE (tree->BlockProcParam.Block, 0);
                NEXT (ProcParam);
                break;
            case kGuard:
                KEYWORD ("guard");
                NODE (tree->Guard.Expr, 1);
                SPACE;
                NODE (tree->Guard.Command, 1);
                NEXT (Guard);
                break;
            case kCaseMatchGuard:
                KEYWORD ("case-match-guard");
                NODE (tree->CaseMatchGuard.CaseMatchs, 1);
                TABS (tabCount + 1);
                NODE (tree->CaseMatchGuard.Command, 1);
                TABS (tabCount);
                NEXT (CaseGuard);
                break;
            case kForCaseGuard:
                KEYWORD ("for-case-guard");
                IDENT (tree->ForCaseGuard.ident);
                SPACE;
                NODE (tree->ForCaseGuard.CaseMatchs, 1);
                TABS (tabCount + 1);
                NODE (tree->ForCaseGuard.Command, 1);
                TABS (tabCount);
                NEXT (CaseGuard);
                break;
            case kChannelGuard:
                KEYWORD ("channel-guard");
                NODE (tree->ChannelGuard.LvalueLists, 1);
                TABS (tabCount + 1);
                NODE (tree->ChannelGuard.Command, 1);
                TABS (tabCount);
                NEXT (ChannelGuard);
                break;
            case kPortGuard:
                KEYWORD ("port-guard");
                NODE (tree->PortGuard.Expr, 2);
                TABS (tabCount + 1);
                NODE (tree->PortGuard.FormalPorts, 2);
                TABS (tabCount);
                NEXT (PortGuard);
                break;
            case kDeclGuard:
                KEYWORD ("decl-guard");
                NODE (tree->DeclGuard.Expr, 2);
                TABS (tabCount + 1);
                NODE (tree->DeclGuard.Decls, 2);
                TABS (tabCount);
                NEXT (DeclGuard);
                break;
            case kIdent:
                IDENT (tree->Ident.ident);
                NEXT (Ident);
                break;
            case kLinkedChannel:
                {
                    bool rename = tree->LinkedChannel.ident != NoIdent;

                    KEYWORD ((rename ? "renamed-channel-lvalue" : "channel-lvalue"));
                    NODE (tree->LinkedChannel.Lvalue, 1);
                    if (rename)
                    {
                        SPACE;
                        IDENT (tree->LinkedChannel.ident);
                    }
                }
                NEXT (LvalueList);
                break;
            case kLinkedBlock:
                KEYWORD ("block-lvalue");
                NODE (tree->LinkedBlock.Block, 1);
                NEXT (LvalueList);
                break;
            case kCaseRange:
                KEYWORD ("case-range");
                NODE (tree->CaseRange.Range, 1);
                NEXT (CaseMatch);
                break;
            case kCaseImplicant:
                KEYWORD ("case-implicant");
                NODE (tree->CaseImplicant.Expr, 1);
                NEXT (CaseMatch);
                break;
            case kValuedEnumElem:
                KEYWORD ("valued-enum-elem");
                IDENT (tree->ValuedEnumElem.ident);
                SPACE;
                NODE (tree->ValuedEnumElem.CoercedExpr, 1);
                NEXT (EnumElem);
                break;
            case kDefaultValuedEnumElem:
                KEYWORD ("enum-elem");
                IDENT (tree->DefaultValuedEnumElem.ident);
                NEXT (EnumElem);
                break;
            case kRecordElem:
                KEYWORD ("record-elem");
                NODE (tree->RecordElem.Idents, 0);
                SPACE;
                NODE (tree->RecordElem.AType, 1);
                NEXT (RecordElem);
                break;
            case kValDecl:
                KEYWORD ("val-decl");
                IDENT (tree->ValDecl.ident);
                SPACE;
                NODE (tree->ValDecl.Expr, 1);
                NEXT (ValDecl);
                break;
            default:
                tree = NULL;
                break;
            }
            CLOSE;
        }
        if (newline)
            TABS (tabCount - 1);
        else if (tree)
            SPACE;
    }
}

/* PrettyPrintNode: print a tree node to the given stream */
void PrettyPrintNode (tTree tree, unsigned tabCount)
{
#define LIST PrettyPrintListNode (true, tree, tabCount);
#define NULLNODE(str) STR (str); SPACE; StrSBreezePosition (PrettyPrintStream, NoPosition);

    /* Ignored nodes are treated specially */

    switch (tree->Kind)
    {
    case kCoercedExpr:         /* CoercedExpr is an AG helper */
        NODE (tree->CoercedExpr.Expr, 0);
        return;
        break;
    default:
        break;
    }

    OPEN;
    switch (tree->Kind)
    {
    case kBalsaProgram:
        PrettyPrintBalsaProgram (tree, tabCount);
        break;
    case kConstantDecl:
    case kVariableDecl:
    case kInitVariableDecl:
    case kChannelDecl:
    case kChannelArrayDecl:
    case kSyncDecl:
    case kSyncArrayDecl:
    case kProcedureDecl:
    case kProcAliasDecl:
    case kFunctionDecl:
    case kSharedDecl:
    case kImportDecl:
    case kTypeDecl:
    case kPartDecl:
    case kIfDecls:
    case kIfElseDecls:
        KEYWORD ("decls");
        LIST;
        break;
    case kSpecifiedRange:
        {
            bool twoBounds = !Tree_IsType (tree->SpecifiedRange.Left, kNullExpr);

            KEYWORD ((twoBounds ? "range2" : "range1"));
            if (twoBounds)
            {
                NODE (tree->SpecifiedRange.Left, 0);
                SPACE;
            }
            NODE (tree->SpecifiedRange.Right, 0);
        }
        break;
    case kTypeRange:
        KEYWORD ("type-range");
        NODE (tree->TypeRange.AType, 0);
        break;
    case kExistingType:
        KEYWORD ("existing-type");
        IDENT (tree->ExistingType.ident);
        break;
    case kBuiltinType:
        KEYWORD ("builtin-type");
        break;
    case kArrayType:
        KEYWORD ("array-type");
        NODE (tree->ArrayType.Range, 0);
        SPACE;
        NODE (tree->ArrayType.AType, 0);
        break;
    case kNewType:
        KEYWORD ("new-type");
        NODE (tree->NewType.AType, 0);
        break;
    case kRecordType:
        {
            bool bounded = !Tree_IsType (tree->RecordType.AType, kNullType);

            KEYWORD ((bounded ? "bounded-record-type" : "record-type"));
            TABS (tabCount + 1);
            NODE (tree->RecordType.RecordElems, 1);
            TABS (tabCount);
            if (bounded)
            {
                SPACE;
                NODE (tree->RecordType.AType, 1);
            }
        }
        break;
    case kEnumType:
        {
            bool bounded = !Tree_IsType (tree->EnumType.AType, kNullType);

            KEYWORD ((bounded ? "bounded-enumeration-type" : "enumeration-type"));
            TABS (tabCount + 1);
            NODE (tree->EnumType.EnumElems, 1);
            TABS (tabCount);
            if (bounded)
            {
                SPACE;
                NODE (tree->EnumType.AType, 1);
            }
        }
        break;
    case kNumericType:
        KEYWORD ("numeric-type");
        STR ((tree->NumericType.signedness ? "#t " : "#f "));
        NODE (tree->NumericType.Expr, 0);
        break;
    case kLiteralExpr:
        KEYWORD ("literal-expr");
        MP (tree->LiteralExpr.literal);
        break;
    case kIdentExpr:
        KEYWORD ("ident-expr");
        IDENT (tree->IdentExpr.ident);
        break;
    case kDontCareExpr:
        STR ("dont-care-expr");
        SPACE;
        StrSBreezePosition (PrettyPrintStream, tree->Expr.position);
        break;
    case kImplicantExpr:
        KEYWORD ("implicant-expr");
        MP (tree->ImplicantExpr.implicant.baseValue);
        SPACE;
        MP (tree->ImplicantExpr.implicant.dontCares);
        break;
    case kStringExpr:
        KEYWORD ("string-expr");
        QUOTEDSTR (tree->StringExpr.string);
        break;
    case kAggregateConsExpr:
        {
            bool namedType = tree->AggregateConsExpr.ident != NoIdent;

            KEYWORD ((namedType ? "named-aggr-cons-expr" : "aggr-cons-expr"));
            if (namedType)
            {
                IDENT (tree->AggregateConsExpr.ident);
                SPACE;
            }
            NODE (tree->AggregateConsExpr.ExprLists, 0);
        }
        break;
    case kNamedEnumElemExpr:
        KEYWORD ("enum-elem-expr");
        IDENT (tree->NamedEnumElemExpr.typeName);
        SPACE;
        IDENT (tree->NamedEnumElemExpr.elemName);
        break;
    case kUnaryExpr:
        KEYWORD ("unary-expr");
        QUOTEDSTR (OperatorNames[tree->UnaryExpr.operation]);
        SPACE;
        NODE (tree->UnaryExpr.Expr, 1);
        break;
    case kBinaryExpr:
        KEYWORD ("binary-expr");
        QUOTEDSTR (OperatorNames[tree->BinaryExpr.operation]);
        SPACE;
        NODE (tree->BinaryExpr.Left, 1);
        SPACE;
        NODE (tree->BinaryExpr.Right, 1);
        break;
    case kRecordElemExtractExpr:
        KEYWORD ("record-elem-extract-expr");
        NODE (tree->RecordElemExtractExpr.Expr, 0);
        SPACE;
        IDENT (tree->RecordElemExtractExpr.ident);
        break;
    case kArrayExtractExpr:
        KEYWORD ("array-extract-expr");
        NODE (tree->ArrayExtractExpr.Array, 0);
        SPACE;
        NODE (tree->ArrayExtractExpr.Subscript, 0);
        break;
    case kArraySliceExpr:
        KEYWORD ("array-slice-expr");
        NODE (tree->ArraySliceExpr.Expr, 0);
        SPACE;
        NODE (tree->ArraySliceExpr.Range, 0);
        break;
    case kAsExpr:
        KEYWORD ("as-expr");
        NODE (tree->AsExpr.Expr, 0);
        SPACE;
        NODE (tree->AsExpr.AType, 0);
        break;
    case kBitArrayCastExpr:
        KEYWORD ("bit-array-cast-expr");
        NODE (tree->BitArrayCastExpr.Expr, 0);
        break;
    case kLetExpr:
        KEYWORD ("let-expr");
        TABS (tabCount + 1);
        NODE (tree->LetExpr.ValDecls, 1);
        TABS (tabCount + 1);
        NODE (tree->LetExpr.Expr, 1);
        TABS (tabCount);
        break;
    case kFunctionCallExpr:
        KEYWORD ("function-call-expr");
        IDENT (tree->FunctionCallExpr.ident);
        TABS (tabCount + 1);
        NODE (tree->FunctionCallExpr.FunctionParams, 1);
        TABS (tabCount);
        break;
    case kSizeofExpr:
        KEYWORD ("sizeof-expr");
        IDENT (tree->SizeofExpr.ident);
        break;
    case kArrayAppendExpr:
        KEYWORD ("array-append-expr");
        NODE (tree->ArrayAppendExpr.Left, 0);
        SPACE;
        NODE (tree->ArrayAppendExpr.Right, 0);
        break;
    case kValuePort:
    case kParamPort:
    case kTypeParamPort:
    case kIfPorts:
    case kIfElsePorts:
    case kChannelPort:
    case kChannelPortArray:
    case kSyncPort:
    case kSyncPortArray:
        KEYWORD ("formal-ports");
        LIST;
        break;
    case kBlock:
        KEYWORD ("block");
        TABS (tabCount + 1);
        NODE (tree->Block.Decls, 1);
        TABS (tabCount + 1);
        NODE (tree->Block.Command, 1);
        TABS (tabCount);
        break;
    case kContinueCommand:
        KEYWORD ("continue");
        break;
    case kHaltCommand:
        KEYWORD ("halt");
        break;
    case kInputCommand:
        KEYWORD ("input");
        NODE (tree->InputCommand.LHS, 0);
        SPACE;
        NODE (tree->InputCommand.RHS, 0);
        break;
    case kInputEncloseCommand:
        KEYWORD ("input-enclose");
        TABS (tabCount + 1);
        NODE (tree->InputEncloseCommand.LvalueLists, 1);
        TABS (tabCount + 1);
        NODE (tree->InputEncloseCommand.Command, 1);
        TABS (tabCount);
        break;
    case kInputEncloseBangCommand:
        KEYWORD ("input-enclose-bang");
        TABS (tabCount + 1);
        NODE (tree->InputEncloseBangCommand.LvalueLists, 1);
        TABS (tabCount + 1);
        NODE (tree->InputEncloseBangCommand.Command, 1);
        TABS (tabCount);
        break;
    case kOutputCommand:
        KEYWORD ("output");
        NODE (tree->OutputCommand.Lvalue, 0);
        SPACE;
        NODE (tree->OutputCommand.CoercedExpr, 0);
        break;
    case kSyncCommand:
        KEYWORD ("sync");
        NODE (tree->SyncCommand.Lvalue, 0);
        break;
    case kAssignmentCommand:
        KEYWORD ("assign");
        NODE (tree->AssignmentCommand.Lvalue, 0);
        SPACE;
        NODE (tree->AssignmentCommand.CoercedExpr, 0);
        break;
    case kBlockCommand:
        KEYWORD ("block-command");
        TABS (tabCount + 1);
        NODE (tree->BlockCommand.Block, 0);
        TABS (tabCount);
        break;
    case kSequentialCommand:
        KEYWORD ("sequential");
        TABS (tabCount + 1);
        NODE (tree->SequentialCommand.Left, 1);
        TABS (tabCount + 1);
        NODE (tree->SequentialCommand.Right, 1);
        TABS (tabCount);
        break;
    case kParallelCommand:
        {
            KEYWORD ((tree->ParallelCommand.isPermissive ? "permissive-parallel" : "parallel"));
            TABS (tabCount + 1);
            NODE (tree->ParallelCommand.Left, 1);
            TABS (tabCount + 1);
            NODE (tree->ParallelCommand.Right, 1);
            TABS (tabCount);
        }
        break;
    case kLoopCommand:
        KEYWORD ("loop");
        TABS (tabCount + 1);
        NODE (tree->LoopCommand.Command, 1);
        TABS (tabCount);
        break;
    case kWhileGuardsCommand:
        KEYWORD ("while-guards");
        TABS (tabCount + 1);
        NODE (tree->WhileGuardsCommand.Guards, 1);
        TABS (tabCount);
        break;
    case kWhileGuardsAlsoCommand:
        KEYWORD ("while-guards-also");
        TABS (tabCount + 1);
        NODE (tree->WhileGuardsAlsoCommand.Guards, 1);
        TABS (tabCount + 1);
        NODE (tree->WhileGuardsAlsoCommand.Command, 1);
        TABS (tabCount);
        break;
    case kCommandWhileExprCommand:
        KEYWORD ("command-while-expr");
        TABS (tabCount + 1);
        NODE (tree->CommandWhileExprCommand.Command, 1);
        TABS (tabCount + 1);
        NODE (tree->CommandWhileExprCommand.Expr, 1);
        TABS (tabCount);
        break;
    case kCommandWhileGuardsCommand:
        KEYWORD ("command-while-guards");
        TABS (tabCount + 1);
        NODE (tree->CommandWhileGuardsCommand.Command, 1);
        TABS (tabCount + 1);
        NODE (tree->CommandWhileGuardsCommand.Guards, 1);
        TABS (tabCount);
        break;
    case kCommandWhileGuardsAlsoCommand:
        KEYWORD ("command-while-guards-also");
        TABS (tabCount + 1);
        NODE (tree->CommandWhileGuardsAlsoCommand.FirstCommand, 1);
        TABS (tabCount + 1);
        NODE (tree->CommandWhileGuardsAlsoCommand.Guards, 1);
        TABS (tabCount + 1);
        NODE (tree->CommandWhileGuardsAlsoCommand.AlsoCommand, 1);
        TABS (tabCount);
        break;
    case kIfCommand:
        KEYWORD ("if");
        TABS (tabCount + 1);
        NODE (tree->IfCommand.Guards, 1);
        TABS (tabCount);
        break;
    case kIfElseCommand:
        KEYWORD ("if-else");
        TABS (tabCount + 1);
        NODE (tree->IfElseCommand.Guards, 1);
        TABS (tabCount + 1);
        NODE (tree->IfElseCommand.Command, 1);
        TABS (tabCount);
        break;
    case kCaseCommand:
        {
            bool hasElse = !Tree_IsType (tree->CaseCommand.Command, kNullCommand);

            KEYWORD ((hasElse ? "case-else" : "case"));
            NODE (tree->CaseCommand.Expr, 1);
            TABS (tabCount + 1);
            NODE (tree->CaseCommand.CaseGuards, 1);
            TABS (tabCount);
            if (hasElse)
            {
                TABS (tabCount);
                NODE (tree->CaseCommand.Command, 1);
            }
        }
        break;
    case kForCommand:
        KEYWORD ("for");
        if (tree->ForCommand.isParallel)
        {
            STR ((tree->ForCommand.isPermissive ? "permissive-parallel" : "parallel"));
        } else
            STR ("sequential");
        IDENT (tree->ForCommand.ident);
        SPACE;
        NODE (tree->ForCommand.Range, 1);
        TABS (tabCount + 1);
        NODE (tree->ForCommand.Command, 1);
        TABS (tabCount);
        break;
    case kProcedureCallCommonCommand:
        KEYWORD ("procedure-call");
        IDENT (tree->ProcedureCallCommonCommand.ident);
        TABS (tabCount + 1);
        NODE (tree->ProcedureCallCommonCommand.ProcedureParams, 1);
        TABS (tabCount);
        break;
    case kSelectCommand:
        KEYWORD ("select");
        NODE (tree->SelectCommand.ChannelGuards, 1);
        TABS (tabCount);
        break;
    case kSelectBangCommand:
        KEYWORD ("select!");
        NODE (tree->SelectCommand.ChannelGuards, 1);
        TABS (tabCount);
        break;
    case kArbitrateCommand:
        KEYWORD ("arbitrate");
        NODE (tree->ArbitrateCommand.ChannelGuards, 1);
        TABS (tabCount);
        break;
    case kPrintCommand:
        KEYWORD ("print");
        NODE (tree->PrintCommand.Expr, 1);
        SPACE;
        NODE (tree->PrintCommand.ExprLists, 1);
        break;
    case kSinkCommand:
        KEYWORD ("sink");
        NODE (tree->PrintCommand.Expr, 1);
        break;
    case kExprProcParam:
    case kTypeProcParam:
    case kVarReadProcParam:
    case kVarWriteProcParam:
    case kBlockProcParam:
        KEYWORD ("procedure-params");
        LIST;
        break;
    case kIdentLvalue:
        KEYWORD ("ident-lvalue");
        IDENT (tree->IdentLvalue.ident);
        break;
    case kRecordElemLvalue:
        KEYWORD ("record-elem-lvalue");
        NODE (tree->RecordElemLvalue.Lvalue, 0);
        SPACE;
        IDENT (tree->RecordElemLvalue.ident);
        break;
    case kArrayElemLvalue:
        KEYWORD ("array-elem-lvalue");
        NODE (tree->ArrayElemLvalue.Lvalue, 0);
        SPACE;
        NODE (tree->ArrayElemLvalue.CoercedExpr, 1);
        break;
    case kArraySliceLvalue:
        KEYWORD ("array-slice-lvalue");
        NODE (tree->ArraySliceLvalue.Lvalue, 0);
        SPACE;
        NODE (tree->ArraySliceLvalue.Range, 1);
        break;
    case kArrayAppendLvalue:
        KEYWORD ("array-append-lvalue");
        NODE (tree->ArrayAppendLvalue.Left, 0);
        SPACE;
        NODE (tree->ArrayAppendLvalue.Right, 0);
        break;
    case kArrayConsLvalue:
        KEYWORD ("array-cons-lvalue");
        NODE (tree->ArrayConsLvalue.LvalueLists, 0);
        break;
    case kAsLvalue:
        KEYWORD ("as-lvalue");
        NODE (tree->AsLvalue.Lvalue, 0);
        SPACE;
        NODE (tree->AsLvalue.AType, 1);
        break;
    case kBitArrayCastLvalue:
        KEYWORD ("bit-array-cast-lvalue");
        NODE (tree->BitArrayCastLvalue.Lvalue, 0);
        break;
    case kGuard:
        KEYWORD ("guards");
        LIST;
        break;
    case kCaseMatchGuard:
    case kForCaseGuard:
        KEYWORD ("case-guards");
        LIST;
        break;
    case kChannelGuard:
        KEYWORD ("channel-guards");
        LIST;
        break;
    case kPortGuard:
        KEYWORD ("port-guards");
        LIST;
        break;
    case kDeclGuard:
        KEYWORD ("decl-guards");
        LIST;
        break;
    case kIdent:
        STR ("idents");
        SPACE;
        StrSBreezePosition (PrettyPrintStream, tree->Decls.position);

        while (!Tree_IsType (tree, kNullIdents))
        {
            SPACE;
            IDENT (tree->Ident.ident);
            tree = tree->Ident.next;
        }
        break;
    case kLinkedChannel:
    case kLinkedBlock:
        KEYWORD ("lvalues");
        LIST;
        break;
    case kCaseRange:
    case kCaseImplicant:
        KEYWORD ("case-matches");
        LIST;
        break;
    case kValuedEnumElem:
    case kDefaultValuedEnumElem:
        KEYWORD ("enum-elems");
        LIST;
        break;
    case kRecordElem:
        KEYWORD ("record-elems");
        LIST;
        break;
    case kValDecl:
        KEYWORD ("val-decls");
        LIST;
        break;
    case kExprList:
        KEYWORD ("exprs");
        LIST;
        break;
    case kExprFuncParam:
    case kTypeFuncParam:
        KEYWORD ("function-params");
        LIST;
        break;
        /* Node typed which are allowed to appear as empty lists */
    case kNullDecls:
        NULLNODE ("decls");
        break;
    case kNullValDecls:
        NULLNODE ("val-decls");
        break;
    case kNullFormalPorts:
        NULLNODE ("formal-ports");
        break;
    case kNullProcParams:
        NULLNODE ("procedure-params");
        break;
    case kNullFuncParams:
        NULLNODE ("function-params");
        break;
    case kNullExprLists:
        NULLNODE ("exprs");
    default:
        break;
    }
    CLOSE;
}

/* BeginPretty: do pretty printing initialisation */
void BeginPretty (void)
{
    PrettyPrintStream = stderr;
}
