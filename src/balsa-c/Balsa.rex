/*
	The Balsa Asynchronous Hardware Synthesis System
	Copyright (C) 1995-2003 Department of Computer Science
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

	`Balsa.scan'
	Scanner for `balsa'
	
	$Id: Balsa.scan,v 1.10 2004/05/25 14:21:04 janinl Exp $
*/

SCANNER BalsaScan

EXPORT
{
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

}

GLOBAL
{ 
	static unsigned commentNesting = 0; 
	static unsigned preCommentStartState = 0;

	void BalsaScan_ErrorAttribute
# if defined __STDC__ | defined __cplusplus
 (int Token, BalsaScan_tScanAttribute * pAttribute)
# else
 (Token, pAttribute) int Token; BalsaScan_tScanAttribute * pAttribute;
# endif
{
 pAttribute->Position = BalsaScan_Attribute.Position;
 switch (Token) {
 case /* literal */ 1: 
 pAttribute->literal.value = NewMP_INT (0);
 ;
 break;
 case /* implicant */ 2: 
 pAttribute->implicant.implicant = NoImplicant;
 ;
 break;
 case /* ident */ 3: 
 pAttribute->ident.ident = NoIdent;
 ;
 break;
 case /* string */ 4: 
 pAttribute->string.string = "";
 ;
 break;
 }
}


	/* This is used as a scratch space for reading a lexeme */
	static char lexeme [1024];

	int hasEof = 0;

/* Hack to perform correct tab handling */
#undef yyTab
#define yyTab		yyLineStart -= TabDistance - 1 -((unsigned char *)BalsaScan_TokenPtr-yyLineStart-1) % TabDistance
}

LOCAL
{ BalsaScan_Attribute.Position.File = CurrentFile; }

DEFAULT
{ char badChar[2]; badChar[0] = *BalsaScan_TokenPtr; badChar[1] = '\0';
	LOG_ERROR (IllegalCharacter, MakeIdent1 (badChar), BalsaScan_Attribute.Position); }

EOF
{ if (yyStartState == CMNT) LOG_ERROR (UnexpectedEOFInComment, NoIdent, BalsaScan_Attribute.Position); }

DEFINE
digit = {0-9} .
decdigit = {_ 0-9} .
nonzero = {1-9} .
bindigit = {_ 0-1} .
binxdigit = {_ 0-1 x X ?} .
octdigit = {_ 0-7} .
hexdigit = {_ 0-9 A-F a-f} .
hexxdigit = {_ 0-9 A-F a-f x X ?} .
letter = {a-z A-Z _} .
stringchar = - {\n \"} .
hyphen = {\-} .

START CMNT IMPORT

RULES

NOT #CMNT#	"(--"			:-{ commentNesting++; preCommentStartState = yyStartState; yyStart (CMNT); }
#CMNT#		"(--"			:-{ commentNesting++; }
#CMNT#		"--)"			:-{ commentNesting--; if (!commentNesting) yyStart (preCommentStartState); }
NOT #CMNT#	"--)"			:-{ if (!commentNesting) LOG_ERROR (BadCommentNesting, NoIdent, BalsaScan_Attribute.Position); }
#CMNT#		"(" | "-" | - {\-(\t\n} + :-{} 

#STD#	0 octdigit *			: {
	BalsaScan_GetWord (lexeme);
	BalsaScan_Attribute.literal.value = ReadBasedMP_INTFromString (lexeme, 8, false);
	return 1;
}
#STD#	0 {xX} hexdigit +		: {
	BalsaScan_GetWord (lexeme);
	BalsaScan_Attribute.literal.value = ReadBasedMP_INTFromString (lexeme + 2, 16, false);
	return 1;
}
#STD#	0 {bB} bindigit +		: {
	BalsaScan_GetWord (lexeme);
	BalsaScan_Attribute.literal.value = ReadBasedMP_INTFromString (lexeme + 2, 2, false);
	return 1;
}
#STD#	nonzero decdigit *		: {
	BalsaScan_GetWord (lexeme);
	BalsaScan_Attribute.literal.value = ReadBasedMP_INTFromString (lexeme, 10, false);
	return 1;
}

#STD#	0 {bB} bindigit * {xX?} binxdigit * : {
	BalsaScan_GetWord (lexeme);
	BalsaScan_Attribute.implicant.implicant = ReadImplicantFromString (lexeme + 2, 2); return 2;
}
#STD#	0 {xX} hexdigit * {xX?} hexxdigit * : {
	BalsaScan_GetWord (lexeme);
	BalsaScan_Attribute.implicant.implicant = ReadImplicantFromString (lexeme + 2, 16); return 2;
}

#STD#	\" stringchar * \"		: { BalsaScan_Attribute.string.string = StrNDup (BalsaScan_TokenPtr + 1, BalsaScan_TokenLength - 2);
									return 4; }

#STD, IMPORT#	"--" ANY *		: { }
\t								:-{ yyTab; }

 #STD#import_keyword	: { return 5; }
 #STD#open_brace_in_import	: { return 6; }
 #STD#dot_in_import	: { return 7; }
 #STD#close_brace_in_import	: { return 8; }
 #STD#"t"\y\p\e	: { return 9; }
 #STD#\i\s	: { return 10; }
 #STD#\?	: { return 11; }
 #STD#\{	: { return 12; }
 #STD#\,	: { return 13; }
 #STD#\}	: { return 14; }
 #STD#\'	: { return 15; }
 #STD#\-	: { return 16; }
 #STD#\+	: { return 17; }
 #STD#"n"\o"t"	: { return 18; }
 #STD#\l\o\g	: { return 19; }
 #STD#\s\i\z\e\o"f"	: { return 20; }
 #STD#\*	: { return 21; }
 #STD#\/	: { return 22; }
 #STD#\%	: { return 23; }
 #STD#\^	: { return 24; }
 #STD#\=	: { return 25; }
 #STD#\/\=	: { return 26; }
 #STD#\<	: { return 27; }
 #STD#\>	: { return 28; }
 #STD#\<\=	: { return 29; }
 #STD#\>\=	: { return 30; }
 #STD#\a"n"\d	: { return 31; }
 #STD#\o"r"	: { return 32; }
 #STD#\x\o"r"	: { return 33; }
 #STD#\.	: { return 34; }
 #STD#\[	: { return 35; }
 #STD#\]	: { return 36; }
 #STD#\.\.	: { return 37; }
 #STD#\o"v"\e"r"	: { return 38; }
 #STD#\(	: { return 39; }
 #STD#\a\s	: { return 40; }
 #STD#\)	: { return 41; }
 #STD#\#	: { return 42; }
 #STD#\l\e"t"	: { return 43; }
 #STD#"v"\a\l	: { return 44; }
 #STD#\i"n"	: { return 45; }
 #STD#\@	: { return 46; }
 #STD#"b"\i"t"\s	: { return 47; }
 #STD#\s\i\g"n"\e\d	: { return 48; }
 #STD#\a"r""r"\a\y	: { return 49; }
 #STD#\o"f"	: { return 50; }
 #STD#"b"\u\i\l"t"\i"n"	: { return 51; }
 #STD#"n"\e\w	: { return 52; }
 #STD#"r"\e\c\o"r"\d	: { return 53; }
 #STD#\:	: { return 54; }
 #STD#\;	: { return 55; }
 #STD#\e"n"\d	: { return 56; }
 #STD#\e"n"\u\m\e"r"\a"t"\i\o"n"	: { return 57; }
 #STD#\c\o"n"\s"t"\a"n""t"	: { return 58; }
 #STD#"v"\a"r"\i\a"b"\l\e	: { return 59; }
 #STD#\m\u\l"t"\i\c\a\s"t"	: { return 60; }
 #STD#\c\h\a"n""n"\e\l	: { return 61; }
 #STD#\s\y"n"\c	: { return 62; }
 #STD#\p"r"\o\c\e\d\u"r"\e	: { return 63; }
 #STD#\a\c"t"\i"v"\e	: { return 64; }
 #STD#\p\a\s\s\i"v"\e	: { return 65; }
 #STD#\i"n"\p\u"t"	: { return 66; }
 #STD#\o\u"t"\p\u"t"	: { return 67; }
 #STD#\p\a"r"\a\m\e"t"\e"r"	: { return 68; }
 #STD#\i"f"	: { return 69; }
 #STD#"t"\h\e"n"	: { return 70; }
 #STD#\|	: { return 71; }
 #STD#\e\l\s\e	: { return 72; }
 #STD#"b"\e\g\i"n"	: { return 73; }
 #STD#\c\o"n""t"\i"n"\u\e	: { return 74; }
 #STD#\h\a\l"t"	: { return 75; }
 #STD#\l\o\c\a\l	: { return 76; }
 #STD#\-\>	: { return 77; }
 #STD#\-\>\!	: { return 78; }
 #STD#\<\-	: { return 79; }
 #STD#\:\=	: { return 80; }
 #STD#\|\|	: { return 81; }
 #STD#\|\|\!	: { return 82; }
 #STD#\l\o\o\p	: { return 83; }
 #STD#\w\h\i\l\e	: { return 84; }
 #STD#\a\l\s\o	: { return 85; }
 #STD#\c\a\s\e	: { return 86; }
 #STD#"f"\o"r"	: { return 87; }
 #STD#\s\e\l\e\c"t"	: { return 88; }
 #STD#\s\e\l\e\c"t"\!	: { return 89; }
 #STD#\a"r""b"\i"t""r"\a"t"\e	: { return 90; }
 #STD#\p"r"\i"n""t"	: { return 91; }
 #STD#\s\i"n"\k	: { return 92; }
 #STD#"f"\u"n"\c"t"\i\o"n"	: { return 93; }
 #STD#\s\h\a"r"\e\d	: { return 94; }


#STD#	"import" :- {
	/* IMPORT is used to make keywords into identifiers inside import paths */
	yyStart (IMPORT);
	return 5;
}

#IMPORT# "." : { return 7; }
#IMPORT# "[" : { return 6; }
#IMPORT# "]" : { yyStart (STD); return 8; }

#IMPORT#	letter (letter | digit | hyphen) *	: {
	char nextChar = BalsaScan_TokenPtr[BalsaScan_TokenLength];
	BalsaScan_TokenPtr[BalsaScan_TokenLength] = '\0';
	BalsaScan_Attribute.ident.ident = MakeIdent1 (BalsaScan_TokenPtr);
	BalsaScan_TokenPtr[BalsaScan_TokenLength] = nextChar;
	return 3;
}

#STD#	letter (letter | digit) *	: {
	char nextChar = BalsaScan_TokenPtr[BalsaScan_TokenLength];
	BalsaScan_TokenPtr[BalsaScan_TokenLength] = '\0';
	BalsaScan_Attribute.ident.ident = MakeIdent1 (BalsaScan_TokenPtr);
	BalsaScan_TokenPtr[BalsaScan_TokenLength] = nextChar;
	return 3;
}
