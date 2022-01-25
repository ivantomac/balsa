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

	`Errors.c'
	Error handling module, includes KCT
	look alikes for the parser.
	
 */

#include "Errors.h"
#include "misc.h"
#include "output.h"
#include "flags.h"
#include "lists.h"
#include <unistd.h>
#include <stdlib.h>

/* ErrorContext : context holding structure for error reports */
typedef struct ErrorContext
{
    Ptrchar str;
    int contextNumber;          /* Unique context id */
    tPosition position;
}
ErrorContext, *PtrErrorContext;

DECLARE_CONS_LIST_TYPE (ErrorContext);
DEFINE_CONS_LIST_TYPE (ErrorContext);

PtrErrorContextList GlobalErrorContext = NULL;

/* All the errors the compiler will produce */
Error BalsacErrors[] = {
    {NormalErrorClass, "syntax error", ""}, // SyntaxError
    {WarningErrorClass, "parser restarted", ""}, // RestartPoint
    {WarningErrorClass, "repair token inserted", ""}, // 
    {NormalErrorClass, "expected one of tokens", ""}, // 
    {FatalErrorClass, "can't open file", "for reading"}, // 
    {FatalErrorClass, "can't open file", "for writing"}, // 
    {NoteErrorClass, "processing", ""}, // 
    {NoteErrorClass, "miscellaneous", ""}, // 
    {NormalErrorClass, "illegal character", ""}, // 
    {NormalErrorClass, "unexpected end of file in comment", ""}, // UnexpectedEOFInComment
    {NormalErrorClass, "--) without matching (--", ""}, // 
    {NormalErrorClass, "identifier", "is not unique in local context"}, // 
    {NormalErrorClass, "expression must be constant", ""}, // 
    {NormalErrorClass, "type", "does not exist"}, // 
    {NormalErrorClass, "expression must have a numeric value", ""}, // 
    {NormalErrorClass, "type width must be in the range [1,INT_MAX]", ""}, // 
    {NormalErrorClass, "invalid type", ""}, // 
    {NormalErrorClass, "value of enumeration element",
      "exceeds range of `over' type, adjusting type"}, // 
    {NormalErrorClass, "record element",
      "causes record type size to exceed `over' type size"}, // 
    {NormalErrorClass, "identifier", "must be bound to a constant value"}, // IdentMustBeBoundToAConstantValue
    {NormalErrorClass, "expecting a record type", ""}, // 
    {NormalErrorClass, "expecting an enumeration type", ""}, // 
    {NormalErrorClass, "expecting an array type", ""}, // 
    {NormalErrorClass, "expecting an arrayed channel", ""}, // 
    {NormalErrorClass, "not expecting an arrayed channel", ""}, // 
    {NormalErrorClass, "expression list has fewer elements than expected", ""}, // 
    {NormalErrorClass, "expression list has more elements than expected", ""}, // 
    {NormalErrorClass, "parameter list has fewer elements than expected", ""}, // 
    {NormalErrorClass, "parameter list has more elements than expected", ""}, // 
    {NormalErrorClass, "",
      "is not an element of the required enumeration type"}, // NotAnEnumElem
    {NormalErrorClass, "", "is not an element of the required record type"}, // 
    {NormalErrorClass, "identifier", "has not been bound"}, // 
    {NormalErrorClass, "can't cast from builtin type", ""}, // 
    {NormalErrorClass, "can't declare a constant of a builtin type", ""}, // 
    {NormalErrorClass,
      "unbalanced channel usage; can't perform <read> ; <write> on channel", ""}, // 
    {NormalErrorClass,
      "unbalanced channel usage; can't perform <write> ; <read> on channel", ""}, // 
    {NormalErrorClass, "invalid channel usage; can't write to channel",
      "from two parallel threads"}, // 
    {WarningErrorClass, "possible channel write/write conflict on channel", ""}, // 
    {NormalErrorClass,
          "invalid variable usage; read/write conflict on variable",
      "from parallel threads"}, // 
    {WarningErrorClass,
          "possibly invalid variable usage; read/write conflict on variable",
      "from parallel threads"}, // VariableReadWriteConflictWarning
    {NormalErrorClass,
          "invalid variable usage; write/write conflict on variable",
      "from parallel threads"}, // 
    {WarningErrorClass,
          "possibly invalid variable usage; write/write conflict on variable",
      "from parallel threads"}, // 
    {NormalErrorClass, "can't perform calls on shared block", "in parallel"}, // 
    {NormalErrorClass, "channel", "can't be used in expression"}, // 
    {NormalErrorClass, "can't use a builtin type here", ""}, // 
    {NormalErrorClass, "can't use an array here", ""}, // 
    {NormalErrorClass, "invalid cast target type", ""}, // 
    {NormalErrorClass, "can't perform operations on builtin types", ""}, // 
    {NormalErrorClass,
      "left and right expressions must be of the same type nature", ""}, // 
    {NormalErrorClass, "can't synthesise the", "operator"}, // CannotSynthesiseOperator
    {NormalErrorClass, "can't apply operator",
      "to enumeration typed expressions"}, // 
    {NormalErrorClass, "can't apply operator", "to record typed expressions"}, // 
    {NormalErrorClass, "can't apply operator", "to array typed expressions"}, // 
    {NormalErrorClass, "left and right expressions must be of the same type",
      ""},                      // 
    {NormalErrorClass, "invalid type nature in a range bound value", ""}, // 
    {NormalErrorClass, "expecting an expression of type", ""}, // 
    {NormalErrorClass, "array has invalid (zero) size", ""}, // 
    {NormalErrorClass,
      "type for range must be either of numeric or enumeration nature", ""}, // 
    {NormalErrorClass,
      "type for expression must be either of numeric or enumeration nature", ""}, // 
    {NormalErrorClass,
      "array subscript must be of either numeric or enumerated type", ""}, // ExprMustBeNumericOrEnum
    {NormalErrorClass, "array subscript has the wrong type", ""}, // 
    {NormalErrorClass, "array subscript out of range", ""}, // 
    {NormalErrorClass, "array subscript cannot contain don't cares", ""}, // 
    {WarningErrorClass,
          "right hand side of `;' operator is unreachable, omitting right hand side",
      ""},                      // 
    {NormalErrorClass, "channel",
      "does not have exactly one source and at least one sink"}, // 
    {NormalErrorClass, "sync channel", "has fewer than 2 connections"}, // 
    {NormalErrorClass, "variable",
      "does not have matched read and write accesses"}, // 
    {WarningErrorClass,
      "loop body never terminates, omitting looping construct", ""}, // 
    {NormalErrorClass, "", "is not a `sync' channel"}, // 
    {NormalErrorClass, "case element label has wrong type", ""}, // WrongTypeInCaseElement
    {NormalErrorClass, "guard expression must have type 1 bits", ""}, // 
    {NormalErrorClass, "", "is neither a valid writable channel or variable"}, // 
    {NormalErrorClass, "expecting a readable channel", ""}, // 
    {NormalErrorClass, "expecting a writable channel", ""}, // 
    {NormalErrorClass, "expecting a variable on right hand side", ""}, // 
    {NormalErrorClass, "expecting a variable on left hand side", ""}, // 
    {NormalErrorClass, "expecting a writable variable", ""}, // 
    {NormalErrorClass,
      "`active' and `passive' can't be used with procedure port", ""}, // 
    {NormalErrorClass, "port", "is unused"}, // 
    {NormalErrorClass, "invalid range, can't declare iterator variable", ""}, // InvalidForRange
    {NormalErrorClass, "can't perform a non atomic access on channel", ""}, // 
    {NormalErrorClass, "constant initialiser value has wrong type", ""}, // 
    {NormalErrorClass, "expecting a channel reference", ""}, // 
    {WarningErrorClass,
          "making sequential use of arbitrate'd/select'ed channels is usually non-DI",
      ""},                      // 
    {NormalErrorClass, "making sequential use of arbitrate'd/select'ed channels is usually non-DI (specify the \"-c allow-sequential-selection\" compilation option to override)", ""}, //
    {NormalErrorClass, "can't write to or use",
      "as a channel within arbitrate/select body"}, // 
    {NormalErrorClass, "channel",
      "already appears in an arbitrate/select guard, can't use it here"}, // 
    {NormalErrorClass, "can't write to", "within arbitrate/select body"}, // 
    {NormalErrorClass,
      "can't mix selected/arbitrated and normal reads on a channel", ""}, // 
    {NormalErrorClass,
      "can't perform arbitration on more than 2 guard channels", ""}, // 
    {NormalErrorClass, "arrayed port element",
      "has different passive/active sense to other elements of the same port"}, // ArrayedPortElementsMustHaveSameSense
    {NormalErrorClass, "range values fall outside bounding type", ""}, // 
    {NormalErrorClass, "can't declare that here", ""}, // 
    {NormalErrorClass, "can't resolve channel", ""}, // 
    {NormalErrorClass, "can't determine type of aggregate cons. expression",
      ""},                      // 
    {NormalErrorClass,
          "can't perform array element extraction on arbitrary array expression",
      ""},                      // 
    {NormalErrorClass, "can't declare a multidimensional array type", ""}, // 
    {NormalErrorClass, "can't perform multicast read on channel",
      "which appears in the body of a shared block"}, // 
    {NormalErrorClass, "can't use local channel", "in a shared block"}, // 
    {NormalErrorClass, "can't have arguments to call to shared block", ""}, // 
    {WarningErrorClass, "shared block", "is unused"}, // SharedBlockNotUsed
    {NormalErrorClass, "first attribute of part",
      "must be either (is-procedure) or (is-function)"}, // 
    {NormalErrorClass, "attribute", "must have a numeric value"}, // 
    {NormalErrorClass, "attribute", "not recognised here"}, // 
    {NormalErrorClass, "expecting a channel count parameter for part", ""}, // 
    {NormalErrorClass, "invalid bundle number", ""}, // 
    {NormalErrorClass, "procedure can't have parameters", ""}, // 
    {NormalErrorClass, "non builtin functions can't have parameters", ""}, // 
    {NormalErrorClass, "too many parameters", ""}, // 
    {NormalErrorClass, "", "is not recognised as a breeze component"}, // 
    {NormalErrorClass, "wire not declared"}, // WireNotDeclared
    {NormalErrorClass, "can't use port",
      "as a function argument (only value arguments are allowed)"}, // 
    {NormalErrorClass, "can't use value argument",
      "as a procedure or part port"}, // 
    {NormalErrorClass, "function",
      "body expression has different type than expected"}, // 
    {NormalErrorClass, "function argument", "is unused"}, // 
    {NormalErrorClass, "", "is not the name of a procedure"}, // 
    {NormalErrorClass, "", "is not the name of a function"}, // 
    {NormalErrorClass, "case command has no valid guards", ""}, // 
    {NormalErrorClass, "case guard values are not disjoint", ""}, // 
    {WarningErrorClass, "some case guard values are not covered, these include",
      ""},                      // 
    {WarningErrorClass,
      "all case guard values covered, ignoring `else' command", ""}, // AllCaseValuesCoveredIgnoringElse
    {NormalErrorClass,
      "can't use a non constant expression as an arrayed channel index", ""}, // 
    {NormalErrorClass,
      "read/write conflict between index expression and variable write", ""}, // 
    {NormalErrorClass, "can't find the base 2 log of zero", ""}, // 
    {NormalErrorClass,
      "power `^' operator must have non-negative right hand argument", ""}, // 
    {NormalErrorClass, "parameters must be the first elements in the port list",
      ""},                      // 
    {NormalErrorClass, "", "is not the name of a parameterised procedure"}, // 
    {NormalErrorClass, "", "is not the name of a parameterised function"}, // 
    {NormalErrorClass, "expecting a parameter expression not a type", ""}, // 
    {NormalErrorClass, "expecting a parameter type not an expression", ""}, // 
    {NormalErrorClass, "block activations are not selectable channels", ""}, // BlockNotSelectable
    {NormalErrorClass, "block can only be given for an active sync port", ""}, // 
    {FatalErrorClass, "", ""},  // 
    {NormalErrorClass, "error:", ""}, // 
    {WarningErrorClass, "", ""}, // 
    {ReportErrorClass, "", ""}, // 
    {NoteErrorClass, "***you shouldn't see this ;)***", ""}, //  /* RuntimePrintCommand */
    {WarningErrorClass, "can't rename channels in procedure ports", ""}, // 
    {NormalErrorClass, "expecting an array type expression on left hand side",
      ""},                      // 
    {NormalErrorClass, "expecting an array type expression on right hand side",
      ""},                      // 
    {NormalErrorClass,
      "left and right expressions to @ must have the same base type", ""}, // ArrayAppendExprsMustHaveSameBaseType
    {NormalErrorClass, "can't rename arrayed channel", ""}, // 
    {NormalErrorClass, "can't slice a variable lvalue", ""}, // 
    {NormalErrorClass, "can't append variable lvalues", ""}, // 
    {NormalErrorClass, "can't construct variable array lvalues", ""}, // 
    {NormalErrorClass, "can't mix passive reads and writes on channel", ""}, // 
    {NormalErrorClass, "can't mix active and passive writes on channel", ""}, // 
    {NormalErrorClass, "can't sequence passive writes on channel", ""}, // 
    {NormalErrorClass, "vectored channels can't be multidimensional", ""}, // 
    {NormalErrorClass, "expecting a parameter type not a block", ""}, // 
    {NormalErrorClass, "expecting a writable variable or channel", ""}, // ExpectingAWritableVariableOrChannel
    {NormalErrorClass, "can't use sync channel in input enclosure command", ""}, // 
    {NormalErrorClass, "input command must have only one channel", ""}, // 
    {NormalErrorClass,
          "don't care containing matches can only be used for unsigned numeric cases",
      ""},                      // 
    {NormalErrorClass, "don't care containing match wider than case value", ""}, // 
    {NormalErrorClass, "problem reading part", "from file"}, // 
    {WarningErrorClass, "multicast formed on channel", ""}, // 
    {NormalErrorClass, "don't care containing values not allowed here", ""}, // 
    {NormalErrorClass,
      "can't mix don't care containing values and non constant exprs. here", ""}, // 
    {NormalErrorClass, "signed types can't have don't care bits", ""}, // 
    {NormalErrorClass, "errors in `for' iteration, giving up on `for'", ""}, // GivingUpOnFor
    {NormalErrorClass,
      "variable/expression access connected to invalid type/sense port. Must be active port.", ""}, // 
    {NormalErrorClass, "can't define a builtin function here", ""}, // 
    {NormalErrorClass, "only builtin functions can have parameters", ""}, // 
    {NormalErrorClass,
      "left and right hand args. to string append must be strings", ""}, // 
    {FatalErrorClass,
          "please check your BALSAHOME env. variable, [balsa.types.builtin] is not valid",
      ""},                      // 
    {NormalErrorClass, "initialised variables not yet supported", ""}, // 
    {NormalErrorClass, "functions can't have conditional arguments", ""}, // 
    {NormalErrorClass, "parameters can't be conditional", ""}, // 
    {NormalErrorClass, "can't emit a runtime print in this context", ""}, // 
    {NormalErrorClass, "can't perform select! with multiple guards", ""}, // CantDoMoreThan1WayEagerFalseVariable
    {NormalErrorClass, "width of type and lvalue must match", ""}
};

/* Number of errors in this run */
unsigned SyntaxErrors = 0;
unsigned WarningCount = 0;
unsigned ErrorCount = 0;

/* ErrorMessage{I} : report the four types of parse error using the proper functions */
void ErrorMessage (KCTError errorType, KCTError errorClass, tPosition position)
{
    if (errorType == xxSyntaxError)
        LOG_ERROR (SyntaxError, NoIdent, position);
    else
        LOG_ERROR (RestartPoint, NoIdent, position);
}

void ErrorMessageI (KCTError errorType, KCTError errorClass, tPosition position, KCTError infoClass, char *str)
{
    if (errorType == xxTokenInserted)
        LOG_ERROR (TokenInserted, MakeIdent1 (str), position);
    else
        LOG_ERROR (ExpectedTokens, MakeIdent1 (str), position);
}

/* LogError : `log' an error at the given position and of the given type, file and lineNo
   should contain the __FILE__ and __LINE__ values at point of invocation of LOG_ERROR.
   Function can also (optionally, NULL == don't bother) print out the function name of
   the error */
void LogError (ErrorType error, tIdent str, tPosition position, char *file, const char *function, int lineNo)
{
    static int lastContextNumber = -1;
    bool printErrorContext = true;

    if (SourceDecorateErrorMessages) /* Decorate error messages with balsa-c source position */
    {
        if (function)
            fprintf (stderr, "[%s:%s:%d] ", file, function, lineNo);
        else
            fprintf (stderr, "[%s:%d] ", file, lineNo);
    }

    WriteErrorPosition (stderr, position);
    switch (BalsacErrors[error].errorClass)
    {
    case ReportErrorClass:
        fprintf (stderr, " report: ");
        printErrorContext = false;
        break;
    case NoteErrorClass:
        fprintf (stderr, " note: ");
        break;
    case WarningErrorClass:
        fprintf (stderr, " warning: ");
        WarningCount++;
        break;
    case DeprecatedFeatureErrorClass:
        fprintf (stderr, " deprecation: ");
        WarningCount++;
        break;
    case FatalErrorClass:
        fprintf (stderr, " fatal: ");
        ErrorCount++;
        break;
    default:
        putc (' ', stderr);
        ErrorCount++;
        break;
    }
    if (str == NoIdent)
        fprintf (stderr, "%s\n", BalsacErrors[error].prefixString);
    else
    {
        if (*(BalsacErrors[error].prefixString) != '\0')
            fprintf (stderr, "%s ", BalsacErrors[error].prefixString);
        fprintf (stderr, "`%s' %s\n", PeekString (str), BalsacErrors[error].postfixString);
    }

    if (printErrorContext)
    {
        if (!GlobalErrorContext || lastContextNumber != CAR (GlobalErrorContext)->contextNumber)
        {
            PtrErrorContextList context = GlobalErrorContext;

            while (context)
            {
                fprintf (stderr, "  ");
                WriteErrorPosition (stderr, context->body->position);
                fprintf (stderr, " ");
                fprintf (stderr, "%s", context->body->str);
                context = CDR (context);
                fprintf (stderr, "\n");
            }
        } else
        {
            fprintf (stderr, "  (same context as previous message)\n");
        }

        if (GlobalErrorContext)
            lastContextNumber = CAR (GlobalErrorContext)->contextNumber;
    }

    if (BalsacErrors[error].errorClass == FatalErrorClass)
        ReportErrors ();
}

/* ReportErrors : call me at exit */
void ReportErrors (void)
{
    if (SyntaxErrors)
        fprintf (stderr, "\n*** %d syntax error%s\n", SyntaxErrors, (SyntaxErrors > 1 ? "s" : ""));
    else if (ErrorCount | WarningCount)
    {
        fprintf (stderr, "\n*** %d error%s, %d warning%s\n", ErrorCount, (ErrorCount != 1 ? "s" : ""), WarningCount, (WarningCount != 1 ? "s" : ""));
    }
    if (SyntaxErrors | ErrorCount)
    {
        if (DoGenerateBreezeFile)
            unlink (BreezeOutputFileName);
    }
    exit (SyntaxErrors | ErrorCount);
}

/* NewErrorContext : constructor */
PtrErrorContext NewErrorContext (Ptrchar str, tPosition position)
{
    PtrErrorContext ret = NEW (ErrorContext);
    static int contextNumber = 0;

    ret->str = str;
    ret->position = position;
    ret->contextNumber = contextNumber;

    contextNumber++;

    return ret;
}

/* PushErrorContext : push extra information about the context of any errors so that
	error reports can be more informative.  These get printed with indenting and positions
	on all reported errors */
void PushErrorContext (Ptrchar str, tPosition position)
{
    GlobalErrorContext = NewErrorContextList (NewErrorContext (str, position), GlobalErrorContext);
}

/* PopErrorContext : pop a single level of error context */
void PopErrorContext (void)
{
    PtrErrorContextList oldContext;

    if (GlobalErrorContext)
    {
        oldContext = GlobalErrorContext;
        GlobalErrorContext = CDR (GlobalErrorContext);

        Free (sizeof (ErrorContext), oldContext->body);
        Free (sizeof (ErrorContextList), oldContext);
    } else
        fprintf (stderr, "Problem with error context nesting\n");
}
