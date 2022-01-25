# ifndef yyBreezeScan
# define yyBreezeScan

/* $Id: Scanner.h,v 2.6 1992/08/07 15:29:41 grosch rel $ */

# if defined __STDC__ | defined __cplusplus
# define ARGS(parameters)	parameters
# else
# define ARGS(parameters)	()
# endif

/* line 29 "Breeze.rex" */

#include "arith.h"
#include "flags.h"
#include "Positions.h"
#include "Errors.h"
#include "implicants.h"
#include "misc.h"
#include "output.h"
#include <Idents.h>
#include <stdlib.h>
#include <ctype.h>

	# if defined __STDC__ | defined __cplusplus
# define ARGS(parameters)	parameters
# else
# define ARGS(parameters)	()
# endif

typedef struct { tPosition yyPos; bool value; } yyboolean;
typedef struct { tPosition yyPos; PtrMP_INT value; } yybreeze_literal;
typedef struct { tPosition yyPos; tIdent keyword; } yybreeze_keyword;
typedef struct { tPosition yyPos; tIdent ident; } yybreeze_ident;

typedef union {
 tPosition Position;
 yyboolean boolean;
 yybreeze_literal breeze_literal;
 yybreeze_keyword breeze_keyword;
 yybreeze_ident breeze_ident;
} BreezeScan_tScanAttribute;

extern void BreezeScan_ErrorAttribute ARGS((int Token, BreezeScan_tScanAttribute * pAttribute));


 
# define BreezeScan_EofToken	0
 
# ifdef lex_interface
#    define BreezeScan_GetToken	yylex
#    define BreezeScan_TokenLength	yyleng
# endif

extern	char *		BreezeScan_TokenPtr	;
extern	short		BreezeScan_TokenLength	;
extern	BreezeScan_tScanAttribute	BreezeScan_Attribute	;
extern	void		(* BreezeScan_Exit) ()	;
 
extern	void		BreezeScan_BeginScanner	();
extern	void		BreezeScan_BeginFile	ARGS ((char * yyFileName));
extern	int		BreezeScan_GetToken	();
extern	int		BreezeScan_GetWord		ARGS ((char * yyWord));
extern	int		BreezeScan_GetLower	ARGS ((char * yyWord));
extern	int		BreezeScan_GetUpper	ARGS ((char * yyWord));
extern	void		BreezeScan_CloseFile	();
extern	void		BreezeScan_CloseScanner	();

# endif
