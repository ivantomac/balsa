# ifndef yyBalsaScan
# define yyBalsaScan

/* $Id: Scanner.h,v 2.6 1992/08/07 15:29:41 grosch rel $ */

# if defined __STDC__ | defined __cplusplus
# define ARGS(parameters)	parameters
# else
# define ARGS(parameters)	()
# endif

/* line 29 "Balsa.rex" */

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

typedef struct { tPosition yyPos; PtrMP_INT value; } yyliteral;
typedef struct { tPosition yyPos; Implicant implicant; } yyimplicant;
typedef struct { tPosition yyPos; tIdent ident; } yyident;
typedef struct { tPosition yyPos; Ptrchar string; } yystring;

typedef union {
 tPosition Position;
 yyliteral literal;
 yyimplicant implicant;
 yyident ident;
 yystring string;
} BalsaScan_tScanAttribute;

extern void BalsaScan_ErrorAttribute ARGS((int Token, BalsaScan_tScanAttribute * pAttribute));


 
# define BalsaScan_EofToken	0
 
# ifdef lex_interface
#    define BalsaScan_GetToken	yylex
#    define BalsaScan_TokenLength	yyleng
# endif

extern	char *		BalsaScan_TokenPtr	;
extern	short		BalsaScan_TokenLength	;
extern	BalsaScan_tScanAttribute	BalsaScan_Attribute	;
extern	void		(* BalsaScan_Exit) ()	;
 
extern	void		BalsaScan_BeginScanner	();
extern	void		BalsaScan_BeginFile	ARGS ((char * yyFileName));
extern	int		BalsaScan_GetToken	();
extern	int		BalsaScan_GetWord		ARGS ((char * yyWord));
extern	int		BalsaScan_GetLower	ARGS ((char * yyWord));
extern	int		BalsaScan_GetUpper	ARGS ((char * yyWord));
extern	void		BalsaScan_CloseFile	();
extern	void		BalsaScan_CloseScanner	();

# endif
