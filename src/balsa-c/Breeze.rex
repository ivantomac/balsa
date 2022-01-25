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

	`Breeze.scan'
	Scanner for `Breeze'
	
	$Id: Breeze.scan,v 1.12 2003/02/08 19:39:44 bardslea Exp $
*/

SCANNER BreezeScan

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

}

GLOBAL
{ 
	void BreezeScan_ErrorAttribute
# if defined __STDC__ | defined __cplusplus
 (int Token, BreezeScan_tScanAttribute * pAttribute)
# else
 (Token, pAttribute) int Token; BreezeScan_tScanAttribute * pAttribute;
# endif
{
 pAttribute->Position = BreezeScan_Attribute.Position;
 switch (Token) {
 case /* boolean */ 1: 
 pAttribute->boolean.value = false;
 ;
 break;
 case /* breeze_literal */ 2: 
 pAttribute->breeze_literal.value = NewMP_INT (0);
 ;
 break;
 case /* breeze_keyword */ 3: 
 pAttribute->breeze_keyword.keyword = NoIdent;
 ;
 break;
 case /* breeze_ident */ 4: 
 pAttribute->breeze_ident.ident = NoIdent;
 ;
 break;
 }
}


	/* This is used as a scratch space for reading a lexeme */
	static char lexeme [1024];
}

LOCAL
{ BreezeScan_Attribute.Position.File = CurrentFile; }

DEFAULT
{ char badChar[2]; badChar[0] = *BreezeScan_TokenPtr; badChar[1] = '\0';
	LOG_ERROR (IllegalCharacter, MakeIdent1 (badChar), BreezeScan_Attribute.Position); }

DEFINE
digit = {0-9} .
decdigit = {_ 0-9} .

/*
nonzero = {1-9} .
bindigit = {_ 0-1} .
binxdigit = {_ 0-1 x X} .
octdigit = {_ 0-7} .
hexdigit = {_ 0-9 A-F a-f} .
hexxdigit = {_ 0-9 A-F a-f x X} .
*/

letter = {a-z A-Z _} .
breeze_identchar = - {\n \"} .

RULES

digit decdigit * : {
	BreezeScan_GetWord (lexeme);
	BreezeScan_Attribute.breeze_literal.value = ReadBasedMP_INTFromString (lexeme, 10, false);
	return 2;
}

"-" digit decdigit * : {
	BreezeScan_GetWord (lexeme);
	BreezeScan_Attribute.breeze_literal.value = ReadBasedMP_INTFromString (lexeme, 10, false);
	mpz_neg (BreezeScan_Attribute.breeze_literal.value, BreezeScan_Attribute.breeze_literal.value);
	return 2;
}

\" breeze_identchar * \"		: {
	BreezeScan_TokenPtr[BreezeScan_TokenLength - 1] = '\0';
	BreezeScan_Attribute.breeze_ident.ident = MakeIdent1 (BreezeScan_TokenPtr + 1);
	return 4;
}

\# t : {
	BreezeScan_Attribute.boolean.value = true;
	return 1;
}

\# f : {
	BreezeScan_Attribute.boolean.value = false;
	return 1;
}

";" ANY *					: { }
\t							:-{ yyTab; }

 \(	: { return 5; }
\i\m\p\o"r""t"	: { return 6; }
\)	: { return 7; }
"f"\i\l\e	: { return 8; }
\c\o"n"\s"t"\a"n""t"	: { return 9; }
"n"\a\m\e\d\-"t"\y\p\e	: { return 10; }
\a\l\i\a\s\-"t"\y\p\e	: { return 11; }
"n"\u\m\e"r"\i\c\-"t"\y\p\e	: { return 12; }
\a"r""r"\a\y\-"t"\y\p\e	: { return 13; }
"r"\e\c\o"r"\d\-"t"\y\p\e	: { return 14; }
\e"n"\u\m\e"r"\a"t"\i\o"n"\-"t"\y\p\e	: { return 15; }
"b"\u\i\l"t"\i"n"\-"t"\y\p\e	: { return 16; }
\i\m\p\l\i\c\a"n""t"	: { return 17; }
"t"\y\p\e	: { return 18; }
"b"\a\l\s\a	: { return 19; }
"t"\y\p\e\-\d\e\c\l	: { return 20; }
\a"t"	: { return 21; }
\e\x\i\s"t"\i"n"\g\-"t"\y\p\e	: { return 22; }
\l\i"t"\e"r"\a\l\-\e\x\p"r"	: { return 23; }
\i\d\e"n""t"\-\e\x\p"r"	: { return 24; }
\s"t""r"\i"n"\g\-\e\x\p"r"	: { return 25; }
\d\o"n""t"\-\c\a"r"\e\-\e\x\p"r"	: { return 26; }
\i\m\p\l\i\c\a"n""t"\-\e\x\p"r"	: { return 27; }
"n"\a\m\e\d\-\a\g\g"r"\-\c\o"n"\s\-\e\x\p"r"	: { return 28; }
\e\x\p"r"\s	: { return 29; }
\a\g\g"r"\-\c\o"n"\s\-\e\x\p"r"	: { return 30; }
\e"n"\u\m\-\e\l\e\m\-\e\x\p"r"	: { return 31; }
\u"n"\a"r"\y\-\e\x\p"r"	: { return 32; }
\s\i\z\e\o"f"\-\e\x\p"r"	: { return 33; }
"b"\i"n"\a"r"\y\-\e\x\p"r"	: { return 34; }
"r"\e\c\o"r"\d\-\e\l\e\m\-\e\x"t""r"\a\c"t"\-\e\x\p"r"	: { return 35; }
\a"r""r"\a\y\-\e\x"t""r"\a\c"t"\-\e\x\p"r"	: { return 36; }
\a"r""r"\a\y\-\s\l\i\c\e\-\e\x\p"r"	: { return 37; }
"r"\a"n"\g\e"1"	: { return 38; }
"r"\a"n"\g\e"2"	: { return 39; }
"t"\y\p\e\-"r"\a"n"\g\e	: { return 40; }
\a\s\-\e\x\p"r"	: { return 41; }
"b"\i"t"\-\a"r""r"\a\y\-\c\a\s"t"\-\e\x\p"r"	: { return 42; }
\l\e"t"\-\e\x\p"r"	: { return 43; }
"v"\a\l\-\d\e\c\l\s	: { return 44; }
"v"\a\l\-\d\e\c\l	: { return 45; }
"f"\u"n"\c"t"\i\o"n"\-\c\a\l\l\-\e\x\p"r"	: { return 46; }
"f"\u"n"\c"t"\i\o"n"\-\p\a"r"\a\m\s	: { return 47; }
\e\x\p"r"\-"f"\u"n"\c\-\p\a"r"\a\m	: { return 48; }
"t"\y\p\e\-"f"\u"n"\c\-\p\a"r"\a\m	: { return 49; }
\a"r""r"\a\y\-\a\p\p\e"n"\d\-\e\x\p"r"	: { return 50; }
"n"\e\w\-"t"\y\p\e	: { return 51; }
"r"\e\c\o"r"\d\-\e\l\e\m\s	: { return 52; }
"r"\e\c\o"r"\d\-\e\l\e\m	: { return 53; }
\i\d\e"n""t"\s	: { return 54; }
"b"\o\u"n"\d\e\d\-"r"\e\c\o"r"\d\-"t"\y\p\e	: { return 55; }
\e"n"\u\m\-\e\l\e\m\s	: { return 56; }
\e"n"\u\m\-\e\l\e\m	: { return 57; }
"v"\a\l\u\e\d\-\e"n"\u\m\-\e\l\e\m	: { return 58; }
"b"\o\u"n"\d\e\d\-\e"n"\u\m\e"r"\a"t"\i\o"n"\-"t"\y\p\e	: { return 59; }
"t"\y\p\e\d\-\c\o"n"\s"t"\a"n""t"\-\d\e\c\l	: { return 60; }
\u"n""t"\y\p\e\d\-\c\o"n"\s"t"\a"n""t"\-\d\e\c\l	: { return 61; }
"v"\a"r"\i\a"b"\l\e\-\d\e\c\l	: { return 62; }
\i"n"\i"t"\-"v"\a"r"\i\a"b"\l\e\-\d\e\c\l	: { return 63; }
\c\h\a"n""n"\e\l\-\d\e\c\l	: { return 64; }
\m\u\l"t"\i\c\a\s"t"	: { return 65; }
\a"r""r"\a\y\e\d\-\c\h\a"n""n"\e\l\-\d\e\c\l	: { return 66; }
\s\y"n"\c\-\d\e\c\l	: { return 67; }
\a"r""r"\a\y\e\d\-\s\y"n"\c\-\d\e\c\l	: { return 68; }
\p"r"\o\c\e\d\u"r"\e\-\d\e\c\l	: { return 69; }
"f"\o"r"\m\a\l\-\p\o"r""t"\s	: { return 70; }
"v"\a\l\u\e\-\p\o"r""t"	: { return 71; }
\p\o"r""t"	: { return 72; }
\d\e"f"\a\u\l"t"	: { return 73; }
\a\c"t"\i"v"\e	: { return 74; }
\p\a\s\s\i"v"\e	: { return 75; }
\i"n"\p\u"t"	: { return 76; }
\o\u"t"\p\u"t"	: { return 77; }
\a"r""r"\a\y\e\d\-\p\o"r""t"	: { return 78; }
\s\y"n"\c\-\p\o"r""t"	: { return 79; }
\a"r""r"\a\y\e\d\-\s\y"n"\c\-\p\o"r""t"	: { return 80; }
\p\a"r"\a\m\-\p\o"r""t"	: { return 81; }
"t"\y\p\e\-\p\a"r"\a\m\-\p\o"r""t"	: { return 82; }
\i"f"\-\p\o"r""t"\s	: { return 83; }
\p\o"r""t"\-\g\u\a"r"\d\s	: { return 84; }
\p\o"r""t"\-\g\u\a"r"\d	: { return 85; }
\i"f"\-\e\l\s\e\-\p\o"r""t"\s	: { return 86; }
"b"\l\o\c\k	: { return 87; }
\d\e\c\l\s	: { return 88; }
\c\o"n""t"\i"n"\u\e	: { return 89; }
\h\a\l"t"	: { return 90; }
\i\d\e"n""t"\-\l"v"\a\l\u\e	: { return 91; }
"r"\e\c\o"r"\d\-\e\l\e\m\-\l"v"\a\l\u\e	: { return 92; }
\a"r""r"\a\y\-\e\l\e\m\-\l"v"\a\l\u\e	: { return 93; }
\a"r""r"\a\y\-\s\l\i\c\e\-\l"v"\a\l\u\e	: { return 94; }
\a"r""r"\a\y\-\a\p\p\e"n"\d\-\l"v"\a\l\u\e	: { return 95; }
\a"r""r"\a\y\-\c\o"n"\s\-\l"v"\a\l\u\e	: { return 96; }
\l"v"\a\l\u\e\s	: { return 97; }
"b"\l\o\c\k\-\l"v"\a\l\u\e	: { return 98; }
\c\h\a"n""n"\e\l\-\l"v"\a\l\u\e	: { return 99; }
"r"\e"n"\a\m\e\d\-\c\h\a"n""n"\e\l\-\l"v"\a\l\u\e	: { return 100; }
\a\s\-\l"v"\a\l\u\e	: { return 101; }
"b"\i"t"\-\a"r""r"\a\y\-\c\a\s"t"\-\l"v"\a\l\u\e	: { return 102; }
\i"n"\p\u"t"\-\e"n"\c\l\o\s\e	: { return 103; }
\i"n"\p\u"t"\-\e"n"\c\l\o\s\e\-"b"\a"n"\g	: { return 104; }
\s\y"n"\c	: { return 105; }
\a\s\s\i\g"n"	: { return 106; }
"b"\l\o\c\k\-\c\o\m\m\a"n"\d	: { return 107; }
\s\e\q\u\e"n""t"\i\a\l	: { return 108; }
\p\a"r"\a\l\l\e\l	: { return 109; }
\p\e"r"\m\i\s\s\i"v"\e\-\p\a"r"\a\l\l\e\l	: { return 110; }
\l\o\o\p	: { return 111; }
\w\h\i\l\e\-\g\u\a"r"\d\s	: { return 112; }
\g\u\a"r"\d\s	: { return 113; }
\g\u\a"r"\d	: { return 114; }
\w\h\i\l\e\-\g\u\a"r"\d\s\-\a\l\s\o	: { return 115; }
\c\o\m\m\a"n"\d\-\w\h\i\l\e\-\e\x\p"r"	: { return 116; }
\c\o\m\m\a"n"\d\-\w\h\i\l\e\-\g\u\a"r"\d\s	: { return 117; }
\c\o\m\m\a"n"\d\-\w\h\i\l\e\-\g\u\a"r"\d\s\-\a\l\s\o	: { return 118; }
\i"f"	: { return 119; }
\i"f"\-\e\l\s\e	: { return 120; }
\c\a\s\e	: { return 121; }
\c\a\s\e\-\g\u\a"r"\d\s	: { return 122; }
\c\a\s\e\-\m\a"t"\c\h\-\g\u\a"r"\d	: { return 123; }
\c\a\s\e\-\m\a"t"\c\h\e\s	: { return 124; }
\c\a\s\e\-"r"\a"n"\g\e	: { return 125; }
\c\a\s\e\-\i\m\p\l\i\c\a"n""t"	: { return 126; }
"f"\o"r"\-\c\a\s\e\-\g\u\a"r"\d	: { return 127; }
\c\a\s\e\-\e\l\s\e	: { return 128; }
"f"\o"r"	: { return 129; }
\p"r"\o\c\e\d\u"r"\e\-\c\a\l\l	: { return 130; }
\p"r"\o\c\e\d\u"r"\e\-\p\a"r"\a\m\s	: { return 131; }
\e\x\p"r"\-\p"r"\o\c\-\p\a"r"\a\m	: { return 132; }
"t"\y\p\e\-\p"r"\o\c\-\p\a"r"\a\m	: { return 133; }
"b"\l\o\c\k\-\p"r"\o\c\-\p\a"r"\a\m	: { return 134; }
"v"\a"r"\-"r"\e\a\d\-\p"r"\o\c\-\p\a"r"\a\m	: { return 135; }
"v"\a"r"\-\w"r"\i"t"\e\-\p"r"\o\c\-\p\a"r"\a\m	: { return 136; }
\l\a"b"\e\l\l\e\d\-\c\o\m\m\a"n"\d	: { return 137; }
\s\e\l\e\c"t"	: { return 138; }
\c\h\a"n""n"\e\l\-\g\u\a"r"\d\s	: { return 139; }
\c\h\a"n""n"\e\l\-\g\u\a"r"\d	: { return 140; }
\s\e\l\e\c"t"\!	: { return 141; }
\a"r""b"\i"t""r"\a"t"\e	: { return 142; }
\p"r"\i"n""t"	: { return 143; }
\s\i"n"\k	: { return 144; }
\p"r"\o\c\e\d\u"r"\e\-\a\l\i\a\s\-\d\e\c\l	: { return 145; }
\p"r"\o\c\e\d\u"r"\e\-\p\a"r"\a\m\-\a\l\i\a\s\-\d\e\c\l	: { return 146; }
"t"\y\p\e\d\-"f"\u"n"\c"t"\i\o"n"\-\d\e\c\l	: { return 147; }
\u"n""t"\y\p\e\d\-"f"\u"n"\c"t"\i\o"n"\-\d\e\c\l	: { return 148; }
"b"\u\i\l"t"\i"n"\-"f"\u"n"\c"t"\i\o"n"\-\d\e\c\l	: { return 149; }
\s\h\a"r"\e\d\-\d\e\c\l	: { return 150; }
\i"f"\-\d\e\c\l\s	: { return 151; }
\d\e\c\l\-\g\u\a"r"\d\s	: { return 152; }
\d\e\c\l\-\g\u\a"r"\d	: { return 153; }
\i"f"\-\e\l\s\e\-\d\e\c\l\s	: { return 154; }
\p"r"\i"n""t"\-\d\e\c\l	: { return 155; }
"b""r"\e\e\z\e\-\p\a"r""t"	: { return 156; }
\p\o"r""t"\s	: { return 157; }
\a"t""t""r"\i"b"\u"t"\e\s	: { return 158; }
\c\h\a"n""n"\e\l\s	: { return 159; }
\p\u\s\h	: { return 160; }
\p\u\l\l	: { return 161; }
\c\o\m\p\o"n"\e"n""t"\s	: { return 162; }
\c\o\m\p\o"n"\e"n""t"	: { return 163; }
\u"n"\d\e\c\l\a"r"\e\d\-\c\o\m\p\o"n"\e"n""t"	: { return 164; }
\i\m\p\l\e\m\e"n""t"\s	: { return 165; }
\p\a"r"\a\m\e"t"\e"r"\s	: { return 166; }
\p\a"r"\a\m\e"t"\e"r"	: { return 167; }
"t"\y\p\e\-\p\a"r"\a\m\e"t"\e"r"	: { return 168; }
\c\a\l\l\-\c\o"n""t"\e\x"t"\s	: { return 169; }
\c\a\l\l\-\c\o"n""t"\e\x"t"	: { return 170; }

letter (letter | digit | \-) *	: {
	char nextChar = BreezeScan_TokenPtr[BreezeScan_TokenLength];
	BreezeScan_TokenPtr[BreezeScan_TokenLength] = '\0';
	BreezeScan_Attribute.breeze_keyword.keyword = MakeIdent1 (BreezeScan_TokenPtr);
	BreezeScan_TokenPtr[BreezeScan_TokenLength] = nextChar;
	return 3;
}
