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

	`Errors.h'
	Error handling module, includes KCT
	look alikes for the parser.
	
 */

#ifndef ERRORS_HEADER
#define ERRORS_HEADER

#include "Positions.h"
#include "misc.h"
#include <Idents.h>

/* Dummy type to placate the parser's error reporting */
typedef enum KCTError
{
    xxTokenInserted, xxRepair, xxSyntaxError, xxError,
    xxExpectedTokens, xxInformation, xxRestartPoint,
    xxString
}
KCTError;

/* Class of error, determines action to be taken */
typedef enum ErrorClass
{
    WarningErrorClass, NormalErrorClass, FatalErrorClass,
    ReportErrorClass, NoteErrorClass,
    DeprecatedFeatureErrorClass
}
ErrorClass;

/* Index into BalsacErrors */
typedef enum ErrorType
{
    SyntaxError,
    RestartPoint,
    TokenInserted,
    ExpectedTokens,
    CannotOpenFileToRead,
    CannotOpenFileToWrite,
    ProcessingFile,
    MiscNote,
    IllegalCharacter,
    UnexpectedEOFInComment,
    BadCommentNesting,
    NonUniqueLocalName,
    ExpressionMustBeConstant,
    NoTypeFound,
    ExpressionMustBeNumeric,
    TypeWidthOutOfRange,
    TypeIsNotValid,
    EnumElemExceedsOverType,
    RecordElemExceedsOverType,
    IdentMustBeBoundToAConstantValue,
    ExpectingARecordType,
    ExpectingAnEnumType,
    ExpectingAnArrayType,
    ExpectingAnArrayedChannel,
    NotExpectingAnArrayedChannel,
    ExprListTooShort,
    ExprListTooLong,
    ParameterListTooShort,
    ParameterListTooLong,
    NotAnEnumElem,
    NotARecordElem,
    UnboundIdentifier,
    CannotCastBuiltinType,
    CannotDeclareABuiltinTypeConstant,
    UnbalancedChannelLeftRead,
    UnbalancedChannelRightRead,
    CannotDoParallelChannelWrite,
    PossibleChannelWWConflict,
    VariableReadWriteConflict,
    VariableReadWriteConflictWarning,
    VariableWriteWriteConflict,
    VariableWriteWriteConflictWarning,
    CannotCallInParallel,
    ChannelsCantBeUsedInExpr,
    CannotUseBuiltinTypeHere,
    CannotUseArrayHere,
    InvalidAsTarget,
    CannotOperateOnBuiltinType,
    IncompatibleTypeNatures,
    CannotSynthesiseOperator,
    CannotApplyOperatorToEnum,
    CannotApplyOperatorToRecord,
    CannotApplyOperatorToArray,
    ExpressionsMustHaveSameType,
    InvalidRangeBoundType,
    ExpectingType,
    ArrayHasZeroSize,
    TypeRangeMustBeNumericOrEnum,
    SubscriptMustBeNumericOrEnum,
    ExprMustBeNumericOrEnum,
    SubscriptHasWrongType,
    SubscriptOutOfRange,
    SubscriptCannotBeImplicant,
    PermanentLHSInSequentialCommand,
    ChannelIsNotFunctional,
    SyncIsNotFunctional,
    MismatchedVariableAccesses,
    LoopBodyIsPermanent,
    NotASyncChannel,
    WrongTypeInCaseElement,
    GuardExprMustHaveBitType,
    IdentMustBeChannelOrVariable,
    ExpectingAReadableChannel,
    ExpectingAWritableChannel,
    ExpectingAVariableOnRHS,
    ExpectingAVariableOnLHS,
    ExpectingAWritableVariable,
    CannotSpecifyPortActiveOrPassiveInProc,
    PortIsUnused,
    InvalidForRange,
    CantPerformNonAtomicChannelAccess,
    ConstantHasWrongType,
    ExpectingAChannel,
    SerialUseOfPassiveChannelWarning,
    SerialUseOfPassiveChannelError,
    CannotAccessLockedChannel,
    ChannelAlreadyLocked,
    CannotWriteToLockedChannel,
    CannotMixReadNaturesForNonSync,
    CantDoMoreThan2WayArbitrate,
    ArrayedPortElementsMustHaveSameSense,
    RangeOutOfBounds,
    CannotDeclareThatHere,
    ChannelIsUnresolved,
    CannotDetermineTypeOfAggregate,
    CannotDoExprArrayExtract,
    CannotMakeMultidimensionalArray,
    CannotPerformMulticastOnChannelUsedInSharedBlock,
    CannotUseLocalChannelInSharedBlock,
    CannotHaveArgumentsToSharedProcedure,
    SharedBlockNotUsed,
    FirstPartAttributeMustBeIsSomething,
    AttributeMustHaveNumericValue,
    AttributeNotRecognised,
    ExpectingAChannelCountAttribute,
    InvalidBundleNumber,
    ProcedureCantHaveParameters,
    FunctionCantHaveParameters,
    TooManyParameters,
    InvalidBreezeComponent,
    WireNotDeclared,
    CannotPassPortsToFunction,
    CannotPassValuePortsToProcedure,
    FunctionExpressionHasWrongType,
    ArgumentIsUnused,
    ExpectingAProcedureName,
    ExpectingAFunctionName,
    CaseCommandHasNoValidGuards,
    CaseGuardsAreNotDisjoint,
    SomeCaseValuesNotCovered,
    AllCaseValuesCoveredIgnoringElse,
    CantDoNonConstArrayedChannelIndex,
    ConflictInLvalueIndex,
    CannotFindLogZero,
    PowerMustHaveNonNegativePowerArg,
    ParametersMustBeAtStartOfPorts,
    ExpectingAParameterisedProcedureName,
    ExpectingAParameterisedFunctionName,
    ExpectingAnExprParam,
    ExpectingATypeParam,
    BlockNotSelectable,
    BlockExpectsActiveSyncPort,
    FatalPrintCommand,
    ErrorPrintCommand,
    WarningPrintCommand,
    ReportPrintCommand,
    RuntimePrintCommand,
    ChannelRenamingNotAllowedInProcPorts,
    ExpectingAnArrayOnAppendLHS,
    ExpectingAnArrayOnAppendRHS,
    ArrayAppendExprsMustHaveSameBaseType,
    CannotRenameChannelArray,
    CannotSliceVariableLvalue,
    CannotAppendVariableLvalues,
    CannotConsVariableLvalues,
    CannotMixPassiveReadsAndWrites,
    CannotMixActiveAndPassiveWrites,
    CannotSequencePassiveWrites,
    ArrayedChannelsCannotBeMultidimensional,
    ExpectingAParameterNotABlock,
    ExpectingAWritableVariableOrChannel,
    CannotUseSyncInEncloseCommand,
    TooManyChannelsInInputCommand,
    ImplicantsOnlyForUNumericCases,
    ImplicantTooWide,
    ProblemReadingPart,
    MulticastWarning,
    ImplicantNotAllowedHere,
    CannotMixImplicantsAndNonConsts,
    SignedImplicantsNotAllowed,
    GivingUpOnFor,
    BadVarAccessActualParameter,
    CannotDeclareBuiltinFunctionHere,
    OnlyBuiltinFunctionsCanHaveParameters,
    BothStringAppendArgsMustBeStrings,
    BadBuiltinBlock,
    InitVarNotSupported,
    FunctionsCantHaveConditionalArguments,
    ParametersCantBeConditional,
    CannotReportRuntimeErrorHere,
    CantDoMoreThan1WayEagerFalseVariable,
    WrongWidthInAsLvalue
}
ErrorType;

/* Error response information */
typedef struct Error
{
    ErrorClass errorClass;
    char *prefixString, *postfixString;
}
Error;

/* The standard errors */
extern Error BalsacErrors[];

/* Count of errors in this run */
extern unsigned SyntaxErrors;
extern unsigned WarningCount;
extern unsigned ErrorCount;

/* ErrorMessage{I} : report the four types of parse error using the proper functions */
extern void ErrorMessage (KCTError errorType, KCTError errorClass, tPosition position);
extern void ErrorMessageI (KCTError errorType, KCTError errorClass, tPosition position, KCTError infoClass, char *str);

/* LogError : `log' an error at the given position and of the given type, file and lineNo
   should contain the __FILE__ and __LINE__ values at point of invocation of LOG_ERROR.
   Function can also (optionally, NULL == don't bother) print out the function name of
   the error */
extern void LogError (ErrorType error, tIdent str, tPosition position, char *file, const char *function, int lineNo);

/* PushErrorContext : push extra information about the context of any errors so that
	error reports can be more informative.  These get printed with indenting and positions
	on all reported errors */
extern void PushErrorContext (Ptrchar str, tPosition position);

/* PopErrorContext : pop a single level of error context */
extern void PopErrorContext (void);

/* LOG_ERROR : macro which calls LogError with the current file/position */
#define LOG_ERROR(e,s,p) (LogError ((e),(s),(p),__FILE__,__func__,__LINE__))

/* ReportErrors : call me at exit */
extern void ReportErrors (void);

#endif /* ERRORS_HEADER */
