/* $Id: Scanner.c,v 2.19 1993/08/18 17:28:53 grosch rel $ */

# define bool		char
# define true		1
# define false		0
# define StdIn		0
 
# include "BalsaScan.h"

# ifdef __cplusplus
extern "C" {
#    include "BalsaScanSource.h"
#    include "rSystem.h"
#    include "General.h"
#    include "DynArray.h"
#    include "Positions.h"
}
# else
#    include "BalsaScanSource.h"
#    include "rSystem.h"
#    include "General.h"
#    include "DynArray.h"
#    include "Positions.h"
# endif
 
# include <stdio.h>
# if defined __STDC__ | defined __cplusplus
#  include <stdlib.h>
# else
/* extern char * malloc ();
   extern void free (); */
# endif

# define yyTabSpace	8

# define yyStart(State)	{ yyPreviousStart = yyStartState; yyStartState = State; }
# define yyPrevious	{ yyStateRange s = yyStartState; \
	 		yyStartState = yyPreviousStart; yyPreviousStart = s; }
# define yyEcho		{ char * yyEnd = BalsaScan_TokenPtr + BalsaScan_TokenLength; \
			char yyCh = * yyEnd; * yyEnd = '\0'; \
	 		(void) fputs (BalsaScan_TokenPtr, stdout); * yyEnd = yyCh; }
# define yyLess(n)	{ yyChBufferIndex -= BalsaScan_TokenLength - n; BalsaScan_TokenLength = n; }
# define yyTab		yyLineStart -= yyTabSpace - 1 - ((unsigned char *) BalsaScan_TokenPtr - yyLineStart - 1) % yyTabSpace
# define yyTab1(a)	yyLineStart -= yyTabSpace - 1 - ((unsigned char *) BalsaScan_TokenPtr - yyLineStart + a - 1) % yyTabSpace
# define yyTab2(a,b)	yyLineStart -= yyTabSpace - 1 - ((unsigned char *) BalsaScan_TokenPtr - yyLineStart + a - 1) % yyTabSpace
# define yyEol(Column)	{ yyLineCount ++; yyLineStart = yyChBufferIndex - 1 - Column; }
# define output(c)	(void) putchar ((int) c)
# define unput(c)	* (-- yyChBufferIndex) = c

# define yyDNoState		0
# define yyFileStackSize	16
# define yyInitBufferSize	1024 * 8 + 256
# define yyFirstCh	(unsigned char) '\0'
# define yyLastCh	(unsigned char) '\177'
# define yyEolCh	(unsigned char) '\12'
# define yyEobCh	(unsigned char) '\177'
# define yyDStateCount	354
# define yyTableSize	1096
# define yyEobState	25
# define yyDefaultState	26
# define STD	1
# define CMNT	3
# define IMPORT	5
 
static void yyExit () { rExit (1); }

typedef unsigned short	yyStateRange;
typedef struct { yyStateRange yyCheck, yyNext; } yyCombType;
 
	char *		BalsaScan_TokenPtr	;
	short		BalsaScan_TokenLength	;
	BalsaScan_tScanAttribute	BalsaScan_Attribute	;
	void		(* BalsaScan_Exit) () = yyExit;
 
static	yyCombType	yyComb		[yyTableSize   + 1] = {{1, 0},
{0, 0},
{0, 0},
{0, 0},
{0, 0},
{0, 0},
{0, 0},
{0, 0},
{0, 0},
{1, 28},
{1, 27},
{0, 0},
{0, 0},
{0, 0},
{0, 0},
{0, 0},
{0, 0},
{0, 0},
{0, 0},
{0, 0},
{0, 0},
{0, 0},
{0, 0},
{0, 0},
{0, 0},
{0, 0},
{0, 0},
{0, 0},
{0, 0},
{0, 0},
{0, 0},
{0, 0},
{1, 29},
{75, 76},
{1, 8},
{1, 241},
{96, 97},
{1, 265},
{103, 104},
{1, 275},
{1, 244},
{1, 242},
{1, 266},
{1, 274},
{1, 277},
{1, 102},
{1, 248},
{1, 261},
{1, 21},
{1, 11},
{1, 11},
{1, 11},
{1, 11},
{1, 11},
{1, 11},
{1, 11},
{1, 11},
{1, 11},
{1, 98},
{1, 210},
{1, 100},
{1, 263},
{1, 258},
{1, 279},
{1, 237},
{23, 344},
{11, 11},
{11, 11},
{11, 11},
{11, 11},
{11, 11},
{11, 11},
{11, 11},
{11, 11},
{11, 11},
{11, 11},
{98, 99},
{244, 352},
{248, 249},
{13, 13},
{13, 13},
{13, 13},
{13, 13},
{13, 13},
{13, 13},
{13, 13},
{13, 13},
{14, 14},
{14, 14},
{258, 259},
{261, 262},
{1, 251},
{344, 345},
{1, 250},
{1, 264},
{21, 19},
{39, 336},
{1, 62},
{1, 119},
{1, 79},
{1, 304},
{1, 124},
{1, 46},
{41, 42},
{1, 108},
{1, 34},
{3, 28},
{3, 27},
{1, 91},
{1, 177},
{1, 217},
{1, 141},
{1, 57},
{11, 11},
{1, 211},
{1, 40},
{1, 128},
{21, 22},
{1, 186},
{1, 86},
{1, 252},
{5, 28},
{5, 27},
{1, 278},
{1, 95},
{1, 276},
{13, 13},
{21, 19},
{24, 25},
{3, 30},
{35, 36},
{36, 37},
{34, 132},
{37, 38},
{14, 14},
{38, 39},
{42, 43},
{3, 349},
{43, 44},
{34, 35},
{34, 147},
{44, 45},
{3, 346},
{40, 71},
{5, 29},
{34, 280},
{40, 41},
{40, 54},
{46, 77},
{21, 22},
{47, 48},
{48, 49},
{5, 354},
{49, 50},
{46, 47},
{50, 51},
{51, 52},
{5, 23},
{5, 33},
{52, 53},
{55, 56},
{58, 59},
{59, 60},
{40, 168},
{60, 61},
{64, 65},
{65, 66},
{58, 161},
{66, 67},
{67, 68},
{68, 69},
{69, 70},
{71, 72},
{72, 73},
{73, 74},
{74, 75},
{77, 78},
{5, 7},
{5, 7},
{5, 7},
{5, 7},
{5, 7},
{5, 7},
{5, 7},
{5, 7},
{5, 7},
{5, 7},
{5, 7},
{5, 7},
{5, 7},
{5, 7},
{5, 7},
{5, 7},
{5, 7},
{5, 7},
{5, 7},
{5, 7},
{5, 7},
{5, 7},
{5, 7},
{5, 7},
{5, 7},
{5, 7},
{5, 32},
{80, 81},
{5, 31},
{81, 82},
{5, 7},
{83, 84},
{5, 7},
{5, 7},
{5, 7},
{5, 7},
{5, 7},
{5, 7},
{5, 7},
{5, 7},
{5, 7},
{5, 7},
{5, 7},
{5, 7},
{5, 7},
{5, 7},
{5, 7},
{5, 7},
{5, 7},
{5, 7},
{5, 7},
{5, 7},
{5, 7},
{5, 7},
{5, 7},
{5, 7},
{5, 7},
{5, 7},
{7, 7},
{57, 133},
{84, 85},
{7, 7},
{7, 7},
{7, 7},
{7, 7},
{7, 7},
{7, 7},
{7, 7},
{7, 7},
{7, 7},
{7, 7},
{86, 87},
{87, 88},
{88, 89},
{89, 90},
{93, 94},
{57, 58},
{95, 96},
{7, 7},
{7, 7},
{7, 7},
{7, 7},
{7, 7},
{7, 7},
{7, 7},
{7, 7},
{7, 7},
{7, 7},
{7, 7},
{7, 7},
{7, 7},
{7, 7},
{7, 7},
{7, 7},
{7, 7},
{7, 7},
{7, 7},
{7, 7},
{7, 7},
{7, 7},
{7, 7},
{7, 7},
{7, 7},
{7, 7},
{105, 106},
{106, 107},
{108, 109},
{109, 110},
{7, 7},
{110, 111},
{7, 7},
{7, 7},
{7, 7},
{7, 7},
{7, 7},
{7, 7},
{7, 7},
{7, 7},
{7, 7},
{7, 7},
{7, 7},
{7, 7},
{7, 7},
{7, 7},
{7, 7},
{7, 7},
{7, 7},
{7, 7},
{7, 7},
{7, 7},
{7, 7},
{7, 7},
{7, 7},
{7, 7},
{7, 7},
{7, 7},
{8, 8},
{8, 8},
{8, 8},
{8, 8},
{8, 8},
{8, 8},
{8, 8},
{8, 8},
{8, 8},
{8, 8},
{112, 113},
{8, 8},
{8, 8},
{8, 8},
{8, 8},
{8, 8},
{8, 8},
{8, 8},
{8, 8},
{8, 8},
{8, 8},
{8, 8},
{8, 8},
{8, 8},
{8, 8},
{8, 8},
{8, 8},
{8, 8},
{8, 8},
{8, 8},
{8, 8},
{8, 8},
{8, 8},
{8, 8},
{8, 18},
{8, 8},
{8, 8},
{8, 8},
{8, 8},
{8, 8},
{8, 8},
{8, 8},
{8, 8},
{8, 8},
{8, 8},
{8, 8},
{8, 8},
{8, 8},
{8, 8},
{8, 8},
{8, 8},
{8, 8},
{8, 8},
{8, 8},
{8, 8},
{8, 8},
{8, 8},
{8, 8},
{8, 8},
{8, 8},
{8, 8},
{8, 8},
{8, 8},
{8, 8},
{8, 8},
{8, 8},
{8, 8},
{8, 8},
{8, 8},
{8, 8},
{8, 8},
{8, 8},
{8, 8},
{8, 8},
{8, 8},
{8, 8},
{8, 8},
{8, 8},
{8, 8},
{8, 8},
{8, 8},
{8, 8},
{8, 8},
{8, 8},
{8, 8},
{8, 8},
{8, 8},
{8, 8},
{8, 8},
{8, 8},
{8, 8},
{8, 8},
{8, 8},
{8, 8},
{8, 8},
{8, 8},
{8, 8},
{8, 8},
{8, 8},
{8, 8},
{8, 8},
{8, 8},
{8, 8},
{8, 8},
{8, 8},
{8, 8},
{8, 8},
{8, 8},
{8, 8},
{8, 8},
{8, 8},
{8, 8},
{8, 8},
{8, 8},
{8, 8},
{8, 8},
{8, 8},
{8, 8},
{8, 8},
{8, 8},
{8, 8},
{8, 8},
{8, 8},
{8, 8},
{8, 8},
{8, 8},
{8, 8},
{9, 9},
{9, 9},
{9, 9},
{9, 9},
{9, 9},
{9, 9},
{9, 9},
{9, 9},
{9, 9},
{9, 9},
{114, 115},
{9, 9},
{9, 9},
{9, 9},
{9, 9},
{9, 9},
{9, 9},
{9, 9},
{9, 9},
{9, 9},
{9, 9},
{9, 9},
{9, 9},
{9, 9},
{9, 9},
{9, 9},
{9, 9},
{9, 9},
{9, 9},
{9, 9},
{9, 9},
{9, 9},
{9, 9},
{9, 9},
{9, 9},
{9, 9},
{9, 9},
{9, 9},
{9, 9},
{9, 9},
{9, 9},
{9, 9},
{9, 9},
{9, 9},
{9, 9},
{9, 9},
{9, 9},
{9, 9},
{9, 9},
{9, 9},
{9, 9},
{9, 9},
{9, 9},
{9, 9},
{9, 9},
{9, 9},
{9, 9},
{9, 9},
{9, 9},
{9, 9},
{9, 9},
{9, 9},
{9, 9},
{9, 9},
{9, 9},
{9, 9},
{9, 9},
{9, 9},
{9, 9},
{9, 9},
{9, 9},
{9, 9},
{9, 9},
{9, 9},
{9, 9},
{9, 9},
{9, 9},
{9, 9},
{9, 9},
{9, 9},
{9, 9},
{9, 9},
{9, 9},
{9, 9},
{9, 9},
{9, 9},
{9, 9},
{9, 9},
{9, 9},
{9, 9},
{9, 9},
{9, 9},
{9, 9},
{9, 9},
{9, 9},
{9, 9},
{9, 9},
{9, 9},
{9, 9},
{9, 9},
{9, 9},
{9, 9},
{9, 9},
{9, 9},
{9, 9},
{9, 9},
{9, 9},
{9, 9},
{9, 9},
{9, 9},
{9, 9},
{9, 9},
{9, 9},
{9, 9},
{9, 9},
{9, 9},
{9, 9},
{9, 9},
{9, 9},
{9, 9},
{9, 9},
{9, 9},
{9, 9},
{9, 9},
{9, 9},
{9, 9},
{9, 9},
{12, 12},
{12, 12},
{12, 12},
{12, 12},
{12, 12},
{12, 12},
{12, 12},
{12, 12},
{12, 12},
{12, 12},
{113, 194},
{113, 114},
{115, 116},
{116, 117},
{117, 118},
{120, 121},
{121, 122},
{12, 12},
{12, 12},
{12, 12},
{12, 12},
{12, 12},
{12, 12},
{12, 12},
{12, 12},
{12, 12},
{12, 12},
{12, 12},
{12, 12},
{12, 12},
{12, 12},
{12, 12},
{12, 12},
{12, 12},
{12, 12},
{12, 12},
{12, 12},
{12, 12},
{12, 12},
{12, 12},
{12, 12},
{12, 12},
{12, 12},
{122, 123},
{124, 125},
{125, 126},
{124, 199},
{12, 12},
{126, 127},
{12, 12},
{12, 12},
{12, 12},
{12, 12},
{12, 12},
{12, 12},
{12, 12},
{12, 12},
{12, 12},
{12, 12},
{12, 12},
{12, 12},
{12, 12},
{12, 12},
{12, 12},
{12, 12},
{12, 12},
{12, 12},
{12, 12},
{12, 12},
{12, 12},
{12, 12},
{12, 12},
{12, 12},
{12, 12},
{12, 12},
{15, 15},
{15, 15},
{16, 16},
{16, 16},
{16, 16},
{16, 16},
{16, 16},
{16, 16},
{16, 16},
{16, 16},
{16, 16},
{16, 16},
{63, 64},
{100, 101},
{129, 130},
{15, 14},
{91, 239},
{16, 17},
{54, 230},
{16, 16},
{16, 16},
{16, 16},
{16, 16},
{16, 16},
{16, 16},
{54, 55},
{91, 92},
{102, 344},
{63, 227},
{100, 260},
{62, 156},
{92, 105},
{119, 120},
{128, 129},
{130, 131},
{92, 271},
{119, 234},
{54, 267},
{134, 135},
{62, 83},
{15, 14},
{62, 256},
{16, 17},
{92, 93},
{102, 103},
{62, 63},
{62, 243},
{15, 15},
{119, 220},
{16, 16},
{128, 281},
{16, 16},
{16, 16},
{16, 16},
{16, 16},
{16, 16},
{16, 16},
{135, 136},
{17, 17},
{17, 17},
{17, 17},
{17, 17},
{17, 17},
{17, 17},
{17, 17},
{17, 17},
{17, 17},
{17, 17},
{133, 134},
{133, 151},
{136, 137},
{137, 138},
{15, 14},
{138, 139},
{16, 17},
{17, 17},
{17, 17},
{17, 17},
{17, 17},
{17, 17},
{17, 17},
{79, 80},
{139, 140},
{142, 143},
{141, 226},
{143, 144},
{144, 145},
{145, 146},
{79, 171},
{147, 148},
{148, 149},
{149, 150},
{79, 284},
{151, 152},
{141, 317},
{79, 112},
{141, 255},
{152, 153},
{153, 154},
{141, 142},
{141, 245},
{154, 155},
{156, 157},
{157, 158},
{158, 159},
{17, 17},
{159, 160},
{17, 17},
{17, 17},
{17, 17},
{17, 17},
{17, 17},
{17, 17},
{20, 20},
{20, 20},
{20, 20},
{20, 20},
{20, 20},
{20, 20},
{20, 20},
{20, 20},
{20, 20},
{161, 162},
{162, 163},
{20, 20},
{20, 20},
{20, 20},
{20, 20},
{20, 20},
{20, 20},
{20, 20},
{20, 20},
{20, 20},
{20, 20},
{20, 20},
{20, 20},
{20, 20},
{20, 20},
{20, 20},
{20, 20},
{20, 20},
{20, 20},
{20, 20},
{20, 20},
{20, 20},
{20, 20},
{20, 20},
{20, 20},
{20, 20},
{20, 20},
{20, 20},
{20, 20},
{20, 20},
{163, 164},
{20, 20},
{20, 20},
{20, 20},
{20, 20},
{164, 165},
{20, 20},
{20, 20},
{20, 20},
{20, 20},
{20, 20},
{20, 20},
{20, 20},
{20, 20},
{20, 20},
{20, 20},
{20, 20},
{20, 20},
{20, 20},
{20, 20},
{20, 20},
{20, 20},
{20, 20},
{20, 20},
{20, 20},
{20, 20},
{20, 20},
{20, 20},
{20, 20},
{20, 20},
{20, 20},
{20, 20},
{20, 20},
{20, 20},
{20, 20},
{20, 20},
{20, 20},
{20, 20},
{20, 20},
{20, 20},
{20, 20},
{20, 20},
{20, 20},
{20, 20},
{20, 20},
{20, 20},
{20, 20},
{20, 20},
{20, 20},
{20, 20},
{20, 20},
{20, 20},
{20, 20},
{20, 20},
{20, 20},
{20, 20},
{20, 20},
{20, 20},
{20, 20},
{20, 20},
{20, 20},
{20, 20},
{20, 20},
{20, 20},
{20, 20},
{20, 20},
{20, 20},
{20, 20},
{20, 20},
{20, 20},
{20, 20},
{20, 20},
{20, 20},
{20, 20},
{20, 20},
{20, 20},
{20, 20},
{20, 20},
{20, 20},
{20, 20},
{20, 20},
{20, 20},
{20, 20},
{20, 20},
{20, 20},
{20, 20},
{20, 20},
{165, 166},
{166, 167},
{168, 169},
{169, 170},
{171, 172},
{172, 173},
{173, 174},
{174, 175},
{175, 176},
{177, 178},
{178, 179},
{179, 180},
{180, 181},
{181, 182},
{182, 183},
{183, 184},
{184, 185},
{186, 187},
{187, 238},
{188, 189},
{189, 190},
{190, 191},
{191, 192},
{192, 193},
{187, 188},
{194, 195},
{195, 196},
{196, 197},
{197, 198},
{199, 209},
{200, 201},
{201, 202},
{202, 203},
{203, 204},
{204, 205},
{205, 206},
{206, 207},
{207, 208},
{211, 212},
{212, 213},
{213, 214},
{214, 215},
{215, 216},
{217, 218},
{218, 219},
{220, 221},
{199, 200},
{221, 222},
{222, 223},
{223, 224},
{224, 225},
{227, 228},
{228, 229},
{217, 272},
{230, 231},
{231, 232},
{232, 233},
{234, 235},
{235, 236},
{239, 240},
{245, 246},
{246, 247},
{252, 253},
{253, 254},
{256, 257},
{267, 268},
{268, 269},
{269, 270},
{272, 273},
{281, 282},
{282, 283},
{284, 285},
{285, 286},
{286, 287},
{287, 288},
{288, 289},
{289, 290},
{290, 291},
{291, 292},
{292, 293},
{293, 294},
{294, 295},
{295, 296},
{296, 297},
{297, 298},
{298, 299},
{299, 300},
{300, 301},
{301, 302},
{302, 303},
{304, 305},
{305, 306},
{306, 307},
{307, 308},
{308, 309},
{309, 310},
{310, 311},
{311, 312},
{312, 313},
{313, 314},
{314, 315},
{315, 316},
{317, 318},
{318, 319},
{319, 320},
{320, 321},
{321, 322},
{322, 323},
{323, 324},
{324, 325},
{325, 326},
{326, 327},
{327, 328},
{328, 329},
{329, 330},
{330, 331},
{331, 332},
{332, 333},
{333, 334},
{334, 335},
{336, 337},
{337, 338},
{338, 339},
{339, 340},
{340, 341},
{341, 342},
{342, 343},
{346, 347},
{347, 348},
{349, 350},
{350, 351},
{352, 353},
{354, 352},
{0, 0},
{0, 0},
{0, 0},
{0, 0},
{0, 0},
{0, 0},
{0, 0},
{0, 0},
{0, 0},
{0, 0},
{0, 0},
{0, 0},
{0, 0},
{0, 0},
{0, 0},
{0, 0},
{0, 0},
{0, 0},
{0, 0},
{0, 0},
{0, 0},
{0, 0},
{0, 0},
{0, 0},
{0, 0},
{0, 0},
{0, 0},
{0, 0},
{0, 0},
{0, 0},
{0, 0},
{0, 0},
{0, 0},
{0, 0},
{0, 0},
{0, 0},
{0, 0},
{0, 0},
{0, 0},
{0, 0},
{0, 0},
{0, 0},
{0, 0},
{0, 0},
{0, 0},
{0, 0},
{0, 0},
{0, 0},
{0, 0},
{0, 0},
{0, 0},
{0, 0},
{0, 0},
{0, 0},
{0, 0},
{0, 0},
{0, 0},
{0, 0},
{0, 0},
{0, 0},
{0, 0},
{0, 0},
{0, 0},
{0, 0},
{0, 0},
{0, 0},
{0, 0},
{0, 0},
{0, 0},
{0, 0},
{0, 0},
{0, 0},
{0, 0},
{0, 0},
{0, 0},
{0, 0},
{0, 0},
{0, 0},
{0, 0},
{0, 0},
{0, 0},
{0, 0},
};
static	yyCombType *	yyBasePtr	[yyDStateCount + 1] = {& yyComb [0],
& yyComb [0],
& yyComb [0],
& yyComb [97],
& yyComb [0],
& yyComb [112],
& yyComb [0],
& yyComb [190],
& yyComb [313],
& yyComb [440],
& yyComb [0],
& yyComb [18],
& yyComb [519],
& yyComb [31],
& yyComb [39],
& yyComb [594],
& yyComb [596],
& yyComb [652],
& yyComb [0],
& yyComb [0],
& yyComb [755],
& yyComb [29],
& yyComb [0],
& yyComb [20],
& yyComb [1],
& yyComb [0],
& yyComb [0],
& yyComb [0],
& yyComb [0],
& yyComb [0],
& yyComb [0],
& yyComb [0],
& yyComb [0],
& yyComb [0],
& yyComb [30],
& yyComb [18],
& yyComb [20],
& yyComb [19],
& yyComb [19],
& yyComb [1],
& yyComb [42],
& yyComb [6],
& yyComb [22],
& yyComb [37],
& yyComb [41],
& yyComb [0],
& yyComb [37],
& yyComb [40],
& yyComb [52],
& yyComb [37],
& yyComb [50],
& yyComb [45],
& yyComb [49],
& yyComb [0],
& yyComb [557],
& yyComb [53],
& yyComb [0],
& yyComb [139],
& yyComb [56],
& yyComb [52],
& yyComb [48],
& yyComb [0],
& yyComb [573],
& yyComb [556],
& yyComb [60],
& yyComb [50],
& yyComb [54],
& yyComb [72],
& yyComb [54],
& yyComb [70],
& yyComb [0],
& yyComb [64],
& yyComb [72],
& yyComb [75],
& yyComb [59],
& yyComb [0],
& yyComb [0],
& yyComb [62],
& yyComb [0],
& yyComb [626],
& yyComb [89],
& yyComb [105],
& yyComb [0],
& yyComb [93],
& yyComb [126],
& yyComb [0],
& yyComb [144],
& yyComb [144],
& yyComb [142],
& yyComb [150],
& yyComb [0],
& yyComb [557],
& yyComb [574],
& yyComb [140],
& yyComb [0],
& yyComb [130],
& yyComb [3],
& yyComb [0],
& yyComb [15],
& yyComb [0],
& yyComb [610],
& yyComb [0],
& yyComb [624],
& yyComb [5],
& yyComb [0],
& yyComb [184],
& yyComb [174],
& yyComb [0],
& yyComb [186],
& yyComb [176],
& yyComb [170],
& yyComb [0],
& yyComb [213],
& yyComb [462],
& yyComb [345],
& yyComb [469],
& yyComb [463],
& yyComb [480],
& yyComb [0],
& yyComb [573],
& yyComb [479],
& yyComb [478],
& yyComb [500],
& yyComb [0],
& yyComb [503],
& yyComb [497],
& yyComb [514],
& yyComb [0],
& yyComb [571],
& yyComb [555],
& yyComb [566],
& yyComb [0],
& yyComb [0],
& yyComb [596],
& yyComb [583],
& yyComb [590],
& yyComb [611],
& yyComb [597],
& yyComb [614],
& yyComb [610],
& yyComb [0],
& yyComb [624],
& yyComb [609],
& yyComb [615],
& yyComb [611],
& yyComb [613],
& yyComb [0],
& yyComb [619],
& yyComb [615],
& yyComb [617],
& yyComb [0],
& yyComb [620],
& yyComb [634],
& yyComb [622],
& yyComb [642],
& yyComb [0],
& yyComb [628],
& yyComb [640],
& yyComb [628],
& yyComb [647],
& yyComb [0],
& yyComb [665],
& yyComb [664],
& yyComb [695],
& yyComb [683],
& yyComb [768],
& yyComb [782],
& yyComb [0],
& yyComb [774],
& yyComb [786],
& yyComb [0],
& yyComb [789],
& yyComb [777],
& yyComb [778],
& yyComb [788],
& yyComb [782],
& yyComb [0],
& yyComb [774],
& yyComb [784],
& yyComb [777],
& yyComb [789],
& yyComb [796],
& yyComb [799],
& yyComb [782],
& yyComb [782],
& yyComb [0],
& yyComb [802],
& yyComb [792],
& yyComb [796],
& yyComb [805],
& yyComb [805],
& yyComb [796],
& yyComb [804],
& yyComb [0],
& yyComb [791],
& yyComb [811],
& yyComb [799],
& yyComb [794],
& yyComb [0],
& yyComb [811],
& yyComb [803],
& yyComb [812],
& yyComb [800],
& yyComb [818],
& yyComb [800],
& yyComb [812],
& yyComb [807],
& yyComb [809],
& yyComb [0],
& yyComb [0],
& yyComb [0],
& yyComb [819],
& yyComb [822],
& yyComb [811],
& yyComb [809],
& yyComb [824],
& yyComb [0],
& yyComb [824],
& yyComb [807],
& yyComb [0],
& yyComb [822],
& yyComb [821],
& yyComb [814],
& yyComb [826],
& yyComb [822],
& yyComb [0],
& yyComb [0],
& yyComb [836],
& yyComb [813],
& yyComb [0],
& yyComb [826],
& yyComb [836],
& yyComb [838],
& yyComb [0],
& yyComb [823],
& yyComb [825],
& yyComb [0],
& yyComb [0],
& yyComb [0],
& yyComb [825],
& yyComb [0],
& yyComb [0],
& yyComb [0],
& yyComb [0],
& yyComb [32],
& yyComb [841],
& yyComb [829],
& yyComb [0],
& yyComb [32],
& yyComb [0],
& yyComb [0],
& yyComb [0],
& yyComb [833],
& yyComb [831],
& yyComb [0],
& yyComb [0],
& yyComb [846],
& yyComb [0],
& yyComb [28],
& yyComb [0],
& yyComb [0],
& yyComb [29],
& yyComb [0],
& yyComb [0],
& yyComb [0],
& yyComb [0],
& yyComb [0],
& yyComb [846],
& yyComb [837],
& yyComb [847],
& yyComb [0],
& yyComb [0],
& yyComb [834],
& yyComb [0],
& yyComb [0],
& yyComb [0],
& yyComb [0],
& yyComb [0],
& yyComb [0],
& yyComb [0],
& yyComb [0],
& yyComb [839],
& yyComb [851],
& yyComb [0],
& yyComb [842],
& yyComb [839],
& yyComb [854],
& yyComb [861],
& yyComb [859],
& yyComb [844],
& yyComb [862],
& yyComb [861],
& yyComb [860],
& yyComb [867],
& yyComb [858],
& yyComb [854],
& yyComb [870],
& yyComb [861],
& yyComb [858],
& yyComb [856],
& yyComb [858],
& yyComb [856],
& yyComb [855],
& yyComb [0],
& yyComb [861],
& yyComb [857],
& yyComb [879],
& yyComb [870],
& yyComb [866],
& yyComb [882],
& yyComb [873],
& yyComb [870],
& yyComb [868],
& yyComb [870],
& yyComb [868],
& yyComb [867],
& yyComb [0],
& yyComb [883],
& yyComb [875],
& yyComb [891],
& yyComb [889],
& yyComb [874],
& yyComb [892],
& yyComb [891],
& yyComb [890],
& yyComb [897],
& yyComb [888],
& yyComb [884],
& yyComb [900],
& yyComb [891],
& yyComb [888],
& yyComb [886],
& yyComb [888],
& yyComb [886],
& yyComb [885],
& yyComb [0],
& yyComb [895],
& yyComb [902],
& yyComb [883],
& yyComb [886],
& yyComb [895],
& yyComb [893],
& yyComb [908],
& yyComb [0],
& yyComb [51],
& yyComb [0],
& yyComb [964],
& yyComb [969],
& yyComb [0],
& yyComb [966],
& yyComb [967],
& yyComb [0],
& yyComb [968],
& yyComb [0],
& yyComb [969],
};
static	yyStateRange	yyDefault	[yyDStateCount + 1] = {0,
12,
1,
20,
3,
24,
5,
24,
24,
24,
24,
24,
24,
24,
15,
24,
24,
16,
0,
15,
24,
13,
16,
24,
0,
0,
0,
0,
0,
0,
20,
0,
0,
0,
12,
12,
12,
12,
12,
12,
12,
12,
12,
12,
12,
12,
12,
12,
12,
12,
12,
12,
12,
12,
12,
12,
12,
12,
12,
12,
12,
12,
12,
12,
12,
12,
12,
12,
12,
12,
12,
12,
12,
12,
12,
12,
0,
12,
12,
12,
12,
12,
12,
12,
12,
12,
12,
12,
12,
12,
12,
12,
12,
12,
12,
24,
24,
0,
24,
0,
24,
0,
23,
24,
0,
12,
12,
12,
12,
12,
12,
12,
12,
12,
12,
12,
12,
12,
12,
12,
12,
12,
12,
12,
12,
12,
12,
12,
12,
12,
12,
12,
12,
12,
12,
12,
12,
12,
12,
12,
12,
12,
12,
12,
12,
12,
12,
12,
12,
12,
12,
12,
12,
12,
12,
12,
12,
12,
12,
12,
12,
12,
12,
12,
12,
12,
12,
12,
12,
12,
12,
12,
12,
12,
12,
12,
12,
12,
12,
12,
12,
12,
12,
12,
12,
12,
12,
12,
12,
12,
12,
12,
12,
12,
12,
12,
12,
12,
12,
12,
12,
12,
12,
12,
12,
12,
12,
12,
12,
12,
0,
12,
12,
12,
12,
12,
12,
12,
12,
12,
12,
12,
12,
12,
12,
12,
12,
12,
12,
12,
12,
12,
12,
12,
12,
12,
12,
0,
12,
12,
12,
0,
0,
12,
24,
12,
12,
12,
24,
0,
0,
0,
12,
12,
12,
12,
12,
12,
24,
0,
0,
24,
0,
0,
0,
0,
0,
12,
12,
12,
12,
12,
12,
12,
0,
0,
0,
0,
0,
0,
12,
12,
12,
12,
12,
12,
12,
12,
12,
12,
12,
12,
12,
12,
12,
12,
12,
12,
12,
12,
12,
12,
12,
12,
12,
12,
12,
12,
12,
12,
12,
12,
12,
12,
12,
12,
12,
12,
12,
12,
12,
12,
12,
12,
12,
12,
12,
12,
12,
12,
12,
12,
12,
12,
12,
12,
12,
12,
12,
12,
12,
12,
12,
12,
9,
9,
10,
24,
0,
10,
24,
0,
24,
0,
24,
};
static	yyStateRange	yyEobTrans	[yyDStateCount + 1] = {0,
0,
0,
20,
20,
0,
0,
0,
8,
9,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
20,
0,
0,
0,
0,
0,
0,
0,
0,
0,
20,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
9,
9,
0,
0,
0,
0,
0,
0,
0,
0,
0,
};

static	yyStateRange	yyInitStateStack [yyInitBufferSize] = {0};
static	yyStateRange *	yyStateStack	= yyInitStateStack;
static	unsigned long	yyStateStackSize= yyInitBufferSize;
static	yyStateRange	yyStartState	= 0;
static	yyStateRange	yyPreviousStart	= 1;

static  short		yySourceFile	;
static	bool		yyEof		;
static	unsigned char *	yyChBufferPtr	;
static	unsigned char *	yyChBufferStart	;
static	unsigned long	yyChBufferSize	;
static	unsigned char *	yyChBufferIndex	= ((unsigned char *) yyComb) + 2; /* dirty trick */
static	int		yyBytesRead	;
static	int		yyLineCount	;
static	unsigned char *	yyLineStart	;

static	struct {
	short		yySourceFile	;
	bool		yyEof		;
	unsigned char *	yyChBufferPtr	;
	unsigned char *	yyChBufferStart	;
	unsigned long	yyChBufferSize	;
	unsigned char *	yyChBufferIndex	;
	int		yyBytesRead	;
	int		yyLineCount	;
	unsigned char *	yyLineStart	;
	} yyFileStack [yyFileStackSize + 1], * yyFileStackPtr = yyFileStack;

static	unsigned char	yyToLower	[] = {
'\0', '\1', '\2', '\3', '\4', '\5', '\6', '\7',
'\10', '\11', '\12', '\13', '\14', '\15', '\16', '\17',
'\20', '\21', '\22', '\23', '\24', '\25', '\26', '\27',
'\30', '\31', '\32', '\33', '\34', '\35', '\36', '\37',
' ', '!', '"', '#', '$', '%', '&', '\'', '(', ')', '*', '+', ',', '-', '.', '/',
'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', ':', ';', '<', '=', '>', '?',
'@', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o',
'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', '[', '\\', ']', '^', '_',
'`', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o',
'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', '{', '|', '}', '~', '\177',
'\200', '\201', '\202', '\203', '\204', '\205', '\206', '\207',
'\210', '\211', '\212', '\213', '\214', '\215', '\216', '\217',
'\220', '\221', '\222', '\223', '\224', '\225', '\226', '\227',
'\230', '\231', '\232', '\233', '\234', '\235', '\236', '\237',
'\240', '\241', '\242', '\243', '\244', '\245', '\246', '\247',
'\250', '\251', '\252', '\253', '\254', '\255', '\256', '\257',
'\260', '\261', '\262', '\263', '\264', '\265', '\266', '\267',
'\270', '\271', '\272', '\273', '\274', '\275', '\276', '\277',
'\300', '\301', '\302', '\303', '\304', '\305', '\306', '\307',
'\310', '\311', '\312', '\313', '\314', '\315', '\316', '\317',
'\320', '\321', '\322', '\323', '\324', '\325', '\326', '\327',
'\330', '\331', '\332', '\333', '\334', '\335', '\336', '\337',
'\340', '\341', '\342', '\343', '\344', '\345', '\346', '\347',
'\350', '\351', '\352', '\353', '\354', '\355', '\356', '\357',
'\360', '\361', '\362', '\363', '\364', '\365', '\366', '\367',
'\370', '\371', '\372', '\373', '\374', '\375', '\376', '\377',
};

static	unsigned char	yyToUpper	[] = {
'\0', '\1', '\2', '\3', '\4', '\5', '\6', '\7',
'\10', '\11', '\12', '\13', '\14', '\15', '\16', '\17',
'\20', '\21', '\22', '\23', '\24', '\25', '\26', '\27',
'\30', '\31', '\32', '\33', '\34', '\35', '\36', '\37',
' ', '!', '"', '#', '$', '%', '&', '\'', '(', ')', '*', '+', ',', '-', '.', '/',
'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', ':', ';', '<', '=', '>', '?',
'@', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O',
'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', '[', '\\', ']', '^', '_',
'`', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O',
'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', '{', '|', '}', '~', '\177',
'\200', '\201', '\202', '\203', '\204', '\205', '\206', '\207',
'\210', '\211', '\212', '\213', '\214', '\215', '\216', '\217',
'\220', '\221', '\222', '\223', '\224', '\225', '\226', '\227',
'\230', '\231', '\232', '\233', '\234', '\235', '\236', '\237',
'\240', '\241', '\242', '\243', '\244', '\245', '\246', '\247',
'\250', '\251', '\252', '\253', '\254', '\255', '\256', '\257',
'\260', '\261', '\262', '\263', '\264', '\265', '\266', '\267',
'\270', '\271', '\272', '\273', '\274', '\275', '\276', '\277',
'\300', '\301', '\302', '\303', '\304', '\305', '\306', '\307',
'\310', '\311', '\312', '\313', '\314', '\315', '\316', '\317',
'\320', '\321', '\322', '\323', '\324', '\325', '\326', '\327',
'\330', '\331', '\332', '\333', '\334', '\335', '\336', '\337',
'\340', '\341', '\342', '\343', '\344', '\345', '\346', '\347',
'\350', '\351', '\352', '\353', '\354', '\355', '\356', '\357',
'\360', '\361', '\362', '\363', '\364', '\365', '\366', '\367',
'\370', '\371', '\372', '\373', '\374', '\375', '\376', '\377',
};
 
static	void	yyInitialize	();
static	void	yyErrorMessage	ARGS((int yyErrorCode));
static	char	input		();
/* line 65 "Balsa.rex" */
 
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

 
int BalsaScan_GetToken ()
{
   register	yyStateRange	yyState;
   register	yyStateRange *	yyStatePtr;
   register	unsigned char * yyChBufferIndexReg;
   register	yyCombType * *	yyBasePtrReg = yyBasePtr;
/* line 109 "Balsa.rex" */
 BalsaScan_Attribute.Position.File = CurrentFile; 
   
yyBegin:
   yyState		= yyStartState;		/* initialize */
   yyStatePtr		= & yyStateStack [1];
   yyChBufferIndexReg 	= yyChBufferIndex;
   BalsaScan_TokenPtr	 	= (char *) yyChBufferIndexReg;
 
   /* ASSERT yyChBuffer [yyChBufferIndex] == first character */
 
yyContinue:		/* continue after sentinel or skipping blanks */
   for (;;) {		/* execute as many state transitions as possible */
					/* determine next state and get next character */
      register yyCombType * yyTablePtr = (yyBasePtrReg [yyState] + * yyChBufferIndexReg ++);
      if (yyTablePtr->yyCheck == yyState) {
	 yyState = yyTablePtr->yyNext;
	 * yyStatePtr ++ = yyState;		/* push state */
	 goto yyContinue;
      }
      yyChBufferIndexReg --;			/* reconsider character */
      if ((yyState = yyDefault [yyState]) == yyDNoState) break;
   }
 
   for (;;) {					/* search for last final state */
      BalsaScan_TokenLength = yyChBufferIndexReg - (unsigned char *) BalsaScan_TokenPtr;
      yyChBufferIndex = yyChBufferIndexReg;
switch (* -- yyStatePtr) {
case 353:;
/* line 135 "Balsa.rex" */
{ commentNesting++; preCommentStartState = yyStartState; yyStart (CMNT); 
} yyy1: goto yyBegin;
case 351:;
/* line 136 "Balsa.rex" */
{ commentNesting++; 
} yyy2: goto yyBegin;
case 348:;
/* line 137 "Balsa.rex" */
{ commentNesting--; if (!commentNesting) yyStart (preCommentStartState); 
} yyy3: goto yyBegin;
case 345:;
/* line 138 "Balsa.rex" */
{ if (!commentNesting) LOG_ERROR (BadCommentNesting, NoIdent, BalsaScan_Attribute.Position); 
} yyy4: goto yyBegin;
case 10:;
case 20:;
case 30:;
case 346:;
case 349:;
/* line 139 "Balsa.rex" */
{
} yyy5: goto yyBegin;
case 13:;
case 21:;
BalsaScan_Attribute.Position.Line   = yyLineCount;
BalsaScan_Attribute.Position.Column = (unsigned char *) BalsaScan_TokenPtr - yyLineStart;
/* line 141 "Balsa.rex" */
{
	BalsaScan_GetWord (lexeme);
	BalsaScan_Attribute.literal.value = ReadBasedMP_INTFromString (lexeme, 8, false);
	return 1;

} yyy6: goto yyBegin;
case 16:;
BalsaScan_Attribute.Position.Line   = yyLineCount;
BalsaScan_Attribute.Position.Column = (unsigned char *) BalsaScan_TokenPtr - yyLineStart;
/* line 146 "Balsa.rex" */
{
	BalsaScan_GetWord (lexeme);
	BalsaScan_Attribute.literal.value = ReadBasedMP_INTFromString (lexeme + 2, 16, false);
	return 1;

} yyy7: goto yyBegin;
case 15:;
BalsaScan_Attribute.Position.Line   = yyLineCount;
BalsaScan_Attribute.Position.Column = (unsigned char *) BalsaScan_TokenPtr - yyLineStart;
/* line 151 "Balsa.rex" */
{
	BalsaScan_GetWord (lexeme);
	BalsaScan_Attribute.literal.value = ReadBasedMP_INTFromString (lexeme + 2, 2, false);
	return 1;

} yyy8: goto yyBegin;
case 11:;
BalsaScan_Attribute.Position.Line   = yyLineCount;
BalsaScan_Attribute.Position.Column = (unsigned char *) BalsaScan_TokenPtr - yyLineStart;
/* line 156 "Balsa.rex" */
{
	BalsaScan_GetWord (lexeme);
	BalsaScan_Attribute.literal.value = ReadBasedMP_INTFromString (lexeme, 10, false);
	return 1;

} yyy9: goto yyBegin;
case 14:;
BalsaScan_Attribute.Position.Line   = yyLineCount;
BalsaScan_Attribute.Position.Column = (unsigned char *) BalsaScan_TokenPtr - yyLineStart;
/* line 162 "Balsa.rex" */
{
	BalsaScan_GetWord (lexeme);
	BalsaScan_Attribute.implicant.implicant = ReadImplicantFromString (lexeme + 2, 2); return 2;

} yyy10: goto yyBegin;
case 17:;
BalsaScan_Attribute.Position.Line   = yyLineCount;
BalsaScan_Attribute.Position.Column = (unsigned char *) BalsaScan_TokenPtr - yyLineStart;
/* line 166 "Balsa.rex" */
{
	BalsaScan_GetWord (lexeme);
	BalsaScan_Attribute.implicant.implicant = ReadImplicantFromString (lexeme + 2, 16); return 2;

} yyy11: goto yyBegin;
case 18:;
BalsaScan_Attribute.Position.Line   = yyLineCount;
BalsaScan_Attribute.Position.Column = (unsigned char *) BalsaScan_TokenPtr - yyLineStart;
/* line 171 "Balsa.rex" */
{ BalsaScan_Attribute.string.string = StrNDup (BalsaScan_TokenPtr + 1, BalsaScan_TokenLength - 2);
									return 4; 
} yyy12: goto yyBegin;
case 9:;
case 344:;
BalsaScan_Attribute.Position.Line   = yyLineCount;
BalsaScan_Attribute.Position.Column = (unsigned char *) BalsaScan_TokenPtr - yyLineStart;
/* line 174 "Balsa.rex" */
{ 
} yyy13: goto yyBegin;
case 28:;
/* line 175 "Balsa.rex" */
{ yyTab; 
} yyy14: goto yyBegin;
case 343:;
BalsaScan_Attribute.Position.Line   = yyLineCount;
BalsaScan_Attribute.Position.Column = (unsigned char *) BalsaScan_TokenPtr - yyLineStart;
/* line 177 "Balsa.rex" */
{ return 5; 
} yyy15: goto yyBegin;
case 335:;
BalsaScan_Attribute.Position.Line   = yyLineCount;
BalsaScan_Attribute.Position.Column = (unsigned char *) BalsaScan_TokenPtr - yyLineStart;
/* line 178 "Balsa.rex" */
{ return 6; 
} yyy16: goto yyBegin;
case 316:;
BalsaScan_Attribute.Position.Line   = yyLineCount;
BalsaScan_Attribute.Position.Column = (unsigned char *) BalsaScan_TokenPtr - yyLineStart;
/* line 179 "Balsa.rex" */
{ return 7; 
} yyy17: goto yyBegin;
case 303:;
BalsaScan_Attribute.Position.Line   = yyLineCount;
BalsaScan_Attribute.Position.Column = (unsigned char *) BalsaScan_TokenPtr - yyLineStart;
/* line 180 "Balsa.rex" */
{ return 8; 
} yyy18: goto yyBegin;
case 283:;
BalsaScan_Attribute.Position.Line   = yyLineCount;
BalsaScan_Attribute.Position.Column = (unsigned char *) BalsaScan_TokenPtr - yyLineStart;
/* line 181 "Balsa.rex" */
{ return 9; 
} yyy19: goto yyBegin;
case 280:;
BalsaScan_Attribute.Position.Line   = yyLineCount;
BalsaScan_Attribute.Position.Column = (unsigned char *) BalsaScan_TokenPtr - yyLineStart;
/* line 182 "Balsa.rex" */
{ return 10; 
} yyy20: goto yyBegin;
case 279:;
BalsaScan_Attribute.Position.Line   = yyLineCount;
BalsaScan_Attribute.Position.Column = (unsigned char *) BalsaScan_TokenPtr - yyLineStart;
/* line 183 "Balsa.rex" */
{ return 11; 
} yyy21: goto yyBegin;
case 278:;
BalsaScan_Attribute.Position.Line   = yyLineCount;
BalsaScan_Attribute.Position.Column = (unsigned char *) BalsaScan_TokenPtr - yyLineStart;
/* line 184 "Balsa.rex" */
{ return 12; 
} yyy22: goto yyBegin;
case 277:;
BalsaScan_Attribute.Position.Line   = yyLineCount;
BalsaScan_Attribute.Position.Column = (unsigned char *) BalsaScan_TokenPtr - yyLineStart;
/* line 185 "Balsa.rex" */
{ return 13; 
} yyy23: goto yyBegin;
case 276:;
BalsaScan_Attribute.Position.Line   = yyLineCount;
BalsaScan_Attribute.Position.Column = (unsigned char *) BalsaScan_TokenPtr - yyLineStart;
/* line 186 "Balsa.rex" */
{ return 14; 
} yyy24: goto yyBegin;
case 275:;
BalsaScan_Attribute.Position.Line   = yyLineCount;
BalsaScan_Attribute.Position.Column = (unsigned char *) BalsaScan_TokenPtr - yyLineStart;
/* line 187 "Balsa.rex" */
{ return 15; 
} yyy25: goto yyBegin;
case 102:;
BalsaScan_Attribute.Position.Line   = yyLineCount;
BalsaScan_Attribute.Position.Column = (unsigned char *) BalsaScan_TokenPtr - yyLineStart;
/* line 188 "Balsa.rex" */
{ return 16; 
} yyy26: goto yyBegin;
case 274:;
BalsaScan_Attribute.Position.Line   = yyLineCount;
BalsaScan_Attribute.Position.Column = (unsigned char *) BalsaScan_TokenPtr - yyLineStart;
/* line 189 "Balsa.rex" */
{ return 17; 
} yyy27: goto yyBegin;
case 273:;
BalsaScan_Attribute.Position.Line   = yyLineCount;
BalsaScan_Attribute.Position.Column = (unsigned char *) BalsaScan_TokenPtr - yyLineStart;
/* line 190 "Balsa.rex" */
{ return 18; 
} yyy28: goto yyBegin;
case 271:;
BalsaScan_Attribute.Position.Line   = yyLineCount;
BalsaScan_Attribute.Position.Column = (unsigned char *) BalsaScan_TokenPtr - yyLineStart;
/* line 191 "Balsa.rex" */
{ return 19; 
} yyy29: goto yyBegin;
case 270:;
BalsaScan_Attribute.Position.Line   = yyLineCount;
BalsaScan_Attribute.Position.Column = (unsigned char *) BalsaScan_TokenPtr - yyLineStart;
/* line 192 "Balsa.rex" */
{ return 20; 
} yyy30: goto yyBegin;
case 266:;
BalsaScan_Attribute.Position.Line   = yyLineCount;
BalsaScan_Attribute.Position.Column = (unsigned char *) BalsaScan_TokenPtr - yyLineStart;
/* line 193 "Balsa.rex" */
{ return 21; 
} yyy31: goto yyBegin;
case 261:;
BalsaScan_Attribute.Position.Line   = yyLineCount;
BalsaScan_Attribute.Position.Column = (unsigned char *) BalsaScan_TokenPtr - yyLineStart;
/* line 194 "Balsa.rex" */
{ return 22; 
} yyy32: goto yyBegin;
case 265:;
BalsaScan_Attribute.Position.Line   = yyLineCount;
BalsaScan_Attribute.Position.Column = (unsigned char *) BalsaScan_TokenPtr - yyLineStart;
/* line 195 "Balsa.rex" */
{ return 23; 
} yyy33: goto yyBegin;
case 264:;
BalsaScan_Attribute.Position.Line   = yyLineCount;
BalsaScan_Attribute.Position.Column = (unsigned char *) BalsaScan_TokenPtr - yyLineStart;
/* line 196 "Balsa.rex" */
{ return 24; 
} yyy34: goto yyBegin;
case 263:;
BalsaScan_Attribute.Position.Line   = yyLineCount;
BalsaScan_Attribute.Position.Column = (unsigned char *) BalsaScan_TokenPtr - yyLineStart;
/* line 197 "Balsa.rex" */
{ return 25; 
} yyy35: goto yyBegin;
case 262:;
BalsaScan_Attribute.Position.Line   = yyLineCount;
BalsaScan_Attribute.Position.Column = (unsigned char *) BalsaScan_TokenPtr - yyLineStart;
/* line 198 "Balsa.rex" */
{ return 26; 
} yyy36: goto yyBegin;
case 100:;
BalsaScan_Attribute.Position.Line   = yyLineCount;
BalsaScan_Attribute.Position.Column = (unsigned char *) BalsaScan_TokenPtr - yyLineStart;
/* line 199 "Balsa.rex" */
{ return 27; 
} yyy37: goto yyBegin;
case 258:;
BalsaScan_Attribute.Position.Line   = yyLineCount;
BalsaScan_Attribute.Position.Column = (unsigned char *) BalsaScan_TokenPtr - yyLineStart;
/* line 200 "Balsa.rex" */
{ return 28; 
} yyy38: goto yyBegin;
case 260:;
BalsaScan_Attribute.Position.Line   = yyLineCount;
BalsaScan_Attribute.Position.Column = (unsigned char *) BalsaScan_TokenPtr - yyLineStart;
/* line 201 "Balsa.rex" */
{ return 29; 
} yyy39: goto yyBegin;
case 259:;
BalsaScan_Attribute.Position.Line   = yyLineCount;
BalsaScan_Attribute.Position.Column = (unsigned char *) BalsaScan_TokenPtr - yyLineStart;
/* line 202 "Balsa.rex" */
{ return 30; 
} yyy40: goto yyBegin;
case 257:;
BalsaScan_Attribute.Position.Line   = yyLineCount;
BalsaScan_Attribute.Position.Column = (unsigned char *) BalsaScan_TokenPtr - yyLineStart;
/* line 203 "Balsa.rex" */
{ return 31; 
} yyy41: goto yyBegin;
case 255:;
BalsaScan_Attribute.Position.Line   = yyLineCount;
BalsaScan_Attribute.Position.Column = (unsigned char *) BalsaScan_TokenPtr - yyLineStart;
/* line 204 "Balsa.rex" */
{ return 32; 
} yyy42: goto yyBegin;
case 254:;
BalsaScan_Attribute.Position.Line   = yyLineCount;
BalsaScan_Attribute.Position.Column = (unsigned char *) BalsaScan_TokenPtr - yyLineStart;
/* line 205 "Balsa.rex" */
{ return 33; 
} yyy43: goto yyBegin;
case 248:;
BalsaScan_Attribute.Position.Line   = yyLineCount;
BalsaScan_Attribute.Position.Column = (unsigned char *) BalsaScan_TokenPtr - yyLineStart;
/* line 206 "Balsa.rex" */
{ return 34; 
} yyy44: goto yyBegin;
case 251:;
BalsaScan_Attribute.Position.Line   = yyLineCount;
BalsaScan_Attribute.Position.Column = (unsigned char *) BalsaScan_TokenPtr - yyLineStart;
/* line 207 "Balsa.rex" */
{ return 35; 
} yyy45: goto yyBegin;
case 250:;
BalsaScan_Attribute.Position.Line   = yyLineCount;
BalsaScan_Attribute.Position.Column = (unsigned char *) BalsaScan_TokenPtr - yyLineStart;
/* line 208 "Balsa.rex" */
{ return 36; 
} yyy46: goto yyBegin;
case 249:;
BalsaScan_Attribute.Position.Line   = yyLineCount;
BalsaScan_Attribute.Position.Column = (unsigned char *) BalsaScan_TokenPtr - yyLineStart;
/* line 209 "Balsa.rex" */
{ return 37; 
} yyy47: goto yyBegin;
case 247:;
BalsaScan_Attribute.Position.Line   = yyLineCount;
BalsaScan_Attribute.Position.Column = (unsigned char *) BalsaScan_TokenPtr - yyLineStart;
/* line 210 "Balsa.rex" */
{ return 38; 
} yyy48: goto yyBegin;
case 244:;
BalsaScan_Attribute.Position.Line   = yyLineCount;
BalsaScan_Attribute.Position.Column = (unsigned char *) BalsaScan_TokenPtr - yyLineStart;
/* line 211 "Balsa.rex" */
{ return 39; 
} yyy49: goto yyBegin;
case 243:;
BalsaScan_Attribute.Position.Line   = yyLineCount;
BalsaScan_Attribute.Position.Column = (unsigned char *) BalsaScan_TokenPtr - yyLineStart;
/* line 212 "Balsa.rex" */
{ return 40; 
} yyy50: goto yyBegin;
case 242:;
BalsaScan_Attribute.Position.Line   = yyLineCount;
BalsaScan_Attribute.Position.Column = (unsigned char *) BalsaScan_TokenPtr - yyLineStart;
/* line 213 "Balsa.rex" */
{ return 41; 
} yyy51: goto yyBegin;
case 241:;
BalsaScan_Attribute.Position.Line   = yyLineCount;
BalsaScan_Attribute.Position.Column = (unsigned char *) BalsaScan_TokenPtr - yyLineStart;
/* line 214 "Balsa.rex" */
{ return 42; 
} yyy52: goto yyBegin;
case 240:;
BalsaScan_Attribute.Position.Line   = yyLineCount;
BalsaScan_Attribute.Position.Column = (unsigned char *) BalsaScan_TokenPtr - yyLineStart;
/* line 215 "Balsa.rex" */
{ return 43; 
} yyy53: goto yyBegin;
case 238:;
BalsaScan_Attribute.Position.Line   = yyLineCount;
BalsaScan_Attribute.Position.Column = (unsigned char *) BalsaScan_TokenPtr - yyLineStart;
/* line 216 "Balsa.rex" */
{ return 44; 
} yyy54: goto yyBegin;
case 147:;
BalsaScan_Attribute.Position.Line   = yyLineCount;
BalsaScan_Attribute.Position.Column = (unsigned char *) BalsaScan_TokenPtr - yyLineStart;
/* line 217 "Balsa.rex" */
{ return 45; 
} yyy55: goto yyBegin;
case 237:;
BalsaScan_Attribute.Position.Line   = yyLineCount;
BalsaScan_Attribute.Position.Column = (unsigned char *) BalsaScan_TokenPtr - yyLineStart;
/* line 218 "Balsa.rex" */
{ return 46; 
} yyy56: goto yyBegin;
case 236:;
BalsaScan_Attribute.Position.Line   = yyLineCount;
BalsaScan_Attribute.Position.Column = (unsigned char *) BalsaScan_TokenPtr - yyLineStart;
/* line 219 "Balsa.rex" */
{ return 47; 
} yyy57: goto yyBegin;
case 233:;
BalsaScan_Attribute.Position.Line   = yyLineCount;
BalsaScan_Attribute.Position.Column = (unsigned char *) BalsaScan_TokenPtr - yyLineStart;
/* line 220 "Balsa.rex" */
{ return 48; 
} yyy58: goto yyBegin;
case 229:;
BalsaScan_Attribute.Position.Line   = yyLineCount;
BalsaScan_Attribute.Position.Column = (unsigned char *) BalsaScan_TokenPtr - yyLineStart;
/* line 221 "Balsa.rex" */
{ return 49; 
} yyy59: goto yyBegin;
case 226:;
BalsaScan_Attribute.Position.Line   = yyLineCount;
BalsaScan_Attribute.Position.Column = (unsigned char *) BalsaScan_TokenPtr - yyLineStart;
/* line 222 "Balsa.rex" */
{ return 50; 
} yyy60: goto yyBegin;
case 225:;
BalsaScan_Attribute.Position.Line   = yyLineCount;
BalsaScan_Attribute.Position.Column = (unsigned char *) BalsaScan_TokenPtr - yyLineStart;
/* line 223 "Balsa.rex" */
{ return 51; 
} yyy61: goto yyBegin;
case 219:;
BalsaScan_Attribute.Position.Line   = yyLineCount;
BalsaScan_Attribute.Position.Column = (unsigned char *) BalsaScan_TokenPtr - yyLineStart;
/* line 224 "Balsa.rex" */
{ return 52; 
} yyy62: goto yyBegin;
case 216:;
BalsaScan_Attribute.Position.Line   = yyLineCount;
BalsaScan_Attribute.Position.Column = (unsigned char *) BalsaScan_TokenPtr - yyLineStart;
/* line 225 "Balsa.rex" */
{ return 53; 
} yyy63: goto yyBegin;
case 98:;
BalsaScan_Attribute.Position.Line   = yyLineCount;
BalsaScan_Attribute.Position.Column = (unsigned char *) BalsaScan_TokenPtr - yyLineStart;
/* line 226 "Balsa.rex" */
{ return 54; 
} yyy64: goto yyBegin;
case 210:;
BalsaScan_Attribute.Position.Line   = yyLineCount;
BalsaScan_Attribute.Position.Column = (unsigned char *) BalsaScan_TokenPtr - yyLineStart;
/* line 227 "Balsa.rex" */
{ return 55; 
} yyy65: goto yyBegin;
case 209:;
BalsaScan_Attribute.Position.Line   = yyLineCount;
BalsaScan_Attribute.Position.Column = (unsigned char *) BalsaScan_TokenPtr - yyLineStart;
/* line 228 "Balsa.rex" */
{ return 56; 
} yyy66: goto yyBegin;
case 208:;
BalsaScan_Attribute.Position.Line   = yyLineCount;
BalsaScan_Attribute.Position.Column = (unsigned char *) BalsaScan_TokenPtr - yyLineStart;
/* line 229 "Balsa.rex" */
{ return 57; 
} yyy67: goto yyBegin;
case 198:;
BalsaScan_Attribute.Position.Line   = yyLineCount;
BalsaScan_Attribute.Position.Column = (unsigned char *) BalsaScan_TokenPtr - yyLineStart;
/* line 230 "Balsa.rex" */
{ return 58; 
} yyy68: goto yyBegin;
case 193:;
BalsaScan_Attribute.Position.Line   = yyLineCount;
BalsaScan_Attribute.Position.Column = (unsigned char *) BalsaScan_TokenPtr - yyLineStart;
/* line 231 "Balsa.rex" */
{ return 59; 
} yyy69: goto yyBegin;
case 185:;
BalsaScan_Attribute.Position.Line   = yyLineCount;
BalsaScan_Attribute.Position.Column = (unsigned char *) BalsaScan_TokenPtr - yyLineStart;
/* line 232 "Balsa.rex" */
{ return 60; 
} yyy70: goto yyBegin;
case 176:;
BalsaScan_Attribute.Position.Line   = yyLineCount;
BalsaScan_Attribute.Position.Column = (unsigned char *) BalsaScan_TokenPtr - yyLineStart;
/* line 233 "Balsa.rex" */
{ return 61; 
} yyy71: goto yyBegin;
case 170:;
BalsaScan_Attribute.Position.Line   = yyLineCount;
BalsaScan_Attribute.Position.Column = (unsigned char *) BalsaScan_TokenPtr - yyLineStart;
/* line 234 "Balsa.rex" */
{ return 62; 
} yyy72: goto yyBegin;
case 167:;
BalsaScan_Attribute.Position.Line   = yyLineCount;
BalsaScan_Attribute.Position.Column = (unsigned char *) BalsaScan_TokenPtr - yyLineStart;
/* line 235 "Balsa.rex" */
{ return 63; 
} yyy73: goto yyBegin;
case 160:;
BalsaScan_Attribute.Position.Line   = yyLineCount;
BalsaScan_Attribute.Position.Column = (unsigned char *) BalsaScan_TokenPtr - yyLineStart;
/* line 236 "Balsa.rex" */
{ return 64; 
} yyy74: goto yyBegin;
case 155:;
BalsaScan_Attribute.Position.Line   = yyLineCount;
BalsaScan_Attribute.Position.Column = (unsigned char *) BalsaScan_TokenPtr - yyLineStart;
/* line 237 "Balsa.rex" */
{ return 65; 
} yyy75: goto yyBegin;
case 150:;
BalsaScan_Attribute.Position.Line   = yyLineCount;
BalsaScan_Attribute.Position.Column = (unsigned char *) BalsaScan_TokenPtr - yyLineStart;
/* line 238 "Balsa.rex" */
{ return 66; 
} yyy76: goto yyBegin;
case 146:;
BalsaScan_Attribute.Position.Line   = yyLineCount;
BalsaScan_Attribute.Position.Column = (unsigned char *) BalsaScan_TokenPtr - yyLineStart;
/* line 239 "Balsa.rex" */
{ return 67; 
} yyy77: goto yyBegin;
case 140:;
BalsaScan_Attribute.Position.Line   = yyLineCount;
BalsaScan_Attribute.Position.Column = (unsigned char *) BalsaScan_TokenPtr - yyLineStart;
/* line 240 "Balsa.rex" */
{ return 68; 
} yyy78: goto yyBegin;
case 132:;
BalsaScan_Attribute.Position.Line   = yyLineCount;
BalsaScan_Attribute.Position.Column = (unsigned char *) BalsaScan_TokenPtr - yyLineStart;
/* line 241 "Balsa.rex" */
{ return 69; 
} yyy79: goto yyBegin;
case 131:;
BalsaScan_Attribute.Position.Line   = yyLineCount;
BalsaScan_Attribute.Position.Column = (unsigned char *) BalsaScan_TokenPtr - yyLineStart;
/* line 242 "Balsa.rex" */
{ return 70; 
} yyy80: goto yyBegin;
case 95:;
BalsaScan_Attribute.Position.Line   = yyLineCount;
BalsaScan_Attribute.Position.Column = (unsigned char *) BalsaScan_TokenPtr - yyLineStart;
/* line 243 "Balsa.rex" */
{ return 71; 
} yyy81: goto yyBegin;
case 127:;
BalsaScan_Attribute.Position.Line   = yyLineCount;
BalsaScan_Attribute.Position.Column = (unsigned char *) BalsaScan_TokenPtr - yyLineStart;
/* line 244 "Balsa.rex" */
{ return 72; 
} yyy82: goto yyBegin;
case 123:;
BalsaScan_Attribute.Position.Line   = yyLineCount;
BalsaScan_Attribute.Position.Column = (unsigned char *) BalsaScan_TokenPtr - yyLineStart;
/* line 245 "Balsa.rex" */
{ return 73; 
} yyy83: goto yyBegin;
case 118:;
BalsaScan_Attribute.Position.Line   = yyLineCount;
BalsaScan_Attribute.Position.Column = (unsigned char *) BalsaScan_TokenPtr - yyLineStart;
/* line 246 "Balsa.rex" */
{ return 74; 
} yyy84: goto yyBegin;
case 111:;
BalsaScan_Attribute.Position.Line   = yyLineCount;
BalsaScan_Attribute.Position.Column = (unsigned char *) BalsaScan_TokenPtr - yyLineStart;
/* line 247 "Balsa.rex" */
{ return 75; 
} yyy85: goto yyBegin;
case 107:;
BalsaScan_Attribute.Position.Line   = yyLineCount;
BalsaScan_Attribute.Position.Column = (unsigned char *) BalsaScan_TokenPtr - yyLineStart;
/* line 248 "Balsa.rex" */
{ return 76; 
} yyy86: goto yyBegin;
case 103:;
BalsaScan_Attribute.Position.Line   = yyLineCount;
BalsaScan_Attribute.Position.Column = (unsigned char *) BalsaScan_TokenPtr - yyLineStart;
/* line 249 "Balsa.rex" */
{ return 77; 
} yyy87: goto yyBegin;
case 104:;
BalsaScan_Attribute.Position.Line   = yyLineCount;
BalsaScan_Attribute.Position.Column = (unsigned char *) BalsaScan_TokenPtr - yyLineStart;
/* line 250 "Balsa.rex" */
{ return 78; 
} yyy88: goto yyBegin;
case 101:;
BalsaScan_Attribute.Position.Line   = yyLineCount;
BalsaScan_Attribute.Position.Column = (unsigned char *) BalsaScan_TokenPtr - yyLineStart;
/* line 251 "Balsa.rex" */
{ return 79; 
} yyy89: goto yyBegin;
case 99:;
BalsaScan_Attribute.Position.Line   = yyLineCount;
BalsaScan_Attribute.Position.Column = (unsigned char *) BalsaScan_TokenPtr - yyLineStart;
/* line 252 "Balsa.rex" */
{ return 80; 
} yyy90: goto yyBegin;
case 96:;
BalsaScan_Attribute.Position.Line   = yyLineCount;
BalsaScan_Attribute.Position.Column = (unsigned char *) BalsaScan_TokenPtr - yyLineStart;
/* line 253 "Balsa.rex" */
{ return 81; 
} yyy91: goto yyBegin;
case 97:;
BalsaScan_Attribute.Position.Line   = yyLineCount;
BalsaScan_Attribute.Position.Column = (unsigned char *) BalsaScan_TokenPtr - yyLineStart;
/* line 254 "Balsa.rex" */
{ return 82; 
} yyy92: goto yyBegin;
case 94:;
BalsaScan_Attribute.Position.Line   = yyLineCount;
BalsaScan_Attribute.Position.Column = (unsigned char *) BalsaScan_TokenPtr - yyLineStart;
/* line 255 "Balsa.rex" */
{ return 83; 
} yyy93: goto yyBegin;
case 90:;
BalsaScan_Attribute.Position.Line   = yyLineCount;
BalsaScan_Attribute.Position.Column = (unsigned char *) BalsaScan_TokenPtr - yyLineStart;
/* line 256 "Balsa.rex" */
{ return 84; 
} yyy94: goto yyBegin;
case 85:;
BalsaScan_Attribute.Position.Line   = yyLineCount;
BalsaScan_Attribute.Position.Column = (unsigned char *) BalsaScan_TokenPtr - yyLineStart;
/* line 257 "Balsa.rex" */
{ return 85; 
} yyy95: goto yyBegin;
case 82:;
BalsaScan_Attribute.Position.Line   = yyLineCount;
BalsaScan_Attribute.Position.Column = (unsigned char *) BalsaScan_TokenPtr - yyLineStart;
/* line 258 "Balsa.rex" */
{ return 86; 
} yyy96: goto yyBegin;
case 78:;
BalsaScan_Attribute.Position.Line   = yyLineCount;
BalsaScan_Attribute.Position.Column = (unsigned char *) BalsaScan_TokenPtr - yyLineStart;
/* line 259 "Balsa.rex" */
{ return 87; 
} yyy97: goto yyBegin;
case 75:;
BalsaScan_Attribute.Position.Line   = yyLineCount;
BalsaScan_Attribute.Position.Column = (unsigned char *) BalsaScan_TokenPtr - yyLineStart;
/* line 260 "Balsa.rex" */
{ return 88; 
} yyy98: goto yyBegin;
case 76:;
BalsaScan_Attribute.Position.Line   = yyLineCount;
BalsaScan_Attribute.Position.Column = (unsigned char *) BalsaScan_TokenPtr - yyLineStart;
/* line 261 "Balsa.rex" */
{ return 89; 
} yyy99: goto yyBegin;
case 70:;
BalsaScan_Attribute.Position.Line   = yyLineCount;
BalsaScan_Attribute.Position.Column = (unsigned char *) BalsaScan_TokenPtr - yyLineStart;
/* line 262 "Balsa.rex" */
{ return 90; 
} yyy100: goto yyBegin;
case 61:;
BalsaScan_Attribute.Position.Line   = yyLineCount;
BalsaScan_Attribute.Position.Column = (unsigned char *) BalsaScan_TokenPtr - yyLineStart;
/* line 263 "Balsa.rex" */
{ return 91; 
} yyy101: goto yyBegin;
case 56:;
BalsaScan_Attribute.Position.Line   = yyLineCount;
BalsaScan_Attribute.Position.Column = (unsigned char *) BalsaScan_TokenPtr - yyLineStart;
/* line 264 "Balsa.rex" */
{ return 92; 
} yyy102: goto yyBegin;
case 53:;
BalsaScan_Attribute.Position.Line   = yyLineCount;
BalsaScan_Attribute.Position.Column = (unsigned char *) BalsaScan_TokenPtr - yyLineStart;
/* line 265 "Balsa.rex" */
{ return 93; 
} yyy103: goto yyBegin;
case 45:;
BalsaScan_Attribute.Position.Line   = yyLineCount;
BalsaScan_Attribute.Position.Column = (unsigned char *) BalsaScan_TokenPtr - yyLineStart;
/* line 266 "Balsa.rex" */
{ return 94; 
} yyy104: goto yyBegin;
case 39:;
/* line 269 "Balsa.rex" */
{
	/* IMPORT is used to make keywords into identifiers inside import paths */
	yyStart (IMPORT);
	return 5;

} yyy105: goto yyBegin;
case 33:;
BalsaScan_Attribute.Position.Line   = yyLineCount;
BalsaScan_Attribute.Position.Column = (unsigned char *) BalsaScan_TokenPtr - yyLineStart;
/* line 275 "Balsa.rex" */
{ return 7; 
} yyy106: goto yyBegin;
case 32:;
BalsaScan_Attribute.Position.Line   = yyLineCount;
BalsaScan_Attribute.Position.Column = (unsigned char *) BalsaScan_TokenPtr - yyLineStart;
/* line 276 "Balsa.rex" */
{ return 6; 
} yyy107: goto yyBegin;
case 31:;
BalsaScan_Attribute.Position.Line   = yyLineCount;
BalsaScan_Attribute.Position.Column = (unsigned char *) BalsaScan_TokenPtr - yyLineStart;
/* line 277 "Balsa.rex" */
{ yyStart (STD); return 8; 
} yyy108: goto yyBegin;
case 7:;
BalsaScan_Attribute.Position.Line   = yyLineCount;
BalsaScan_Attribute.Position.Column = (unsigned char *) BalsaScan_TokenPtr - yyLineStart;
/* line 279 "Balsa.rex" */
{
	char nextChar = BalsaScan_TokenPtr[BalsaScan_TokenLength];
	BalsaScan_TokenPtr[BalsaScan_TokenLength] = '\0';
	BalsaScan_Attribute.ident.ident = MakeIdent1 (BalsaScan_TokenPtr);
	BalsaScan_TokenPtr[BalsaScan_TokenLength] = nextChar;
	return 3;

} yyy109: goto yyBegin;
case 12:;
case 34:;
case 35:;
case 36:;
case 37:;
case 38:;
case 40:;
case 41:;
case 42:;
case 43:;
case 44:;
case 46:;
case 47:;
case 48:;
case 49:;
case 50:;
case 51:;
case 52:;
case 54:;
case 55:;
case 57:;
case 58:;
case 59:;
case 60:;
case 62:;
case 63:;
case 64:;
case 65:;
case 66:;
case 67:;
case 68:;
case 69:;
case 71:;
case 72:;
case 73:;
case 74:;
case 77:;
case 79:;
case 80:;
case 81:;
case 83:;
case 84:;
case 86:;
case 87:;
case 88:;
case 89:;
case 91:;
case 92:;
case 93:;
case 105:;
case 106:;
case 108:;
case 109:;
case 110:;
case 112:;
case 113:;
case 114:;
case 115:;
case 116:;
case 117:;
case 119:;
case 120:;
case 121:;
case 122:;
case 124:;
case 125:;
case 126:;
case 128:;
case 129:;
case 130:;
case 133:;
case 134:;
case 135:;
case 136:;
case 137:;
case 138:;
case 139:;
case 141:;
case 142:;
case 143:;
case 144:;
case 145:;
case 148:;
case 149:;
case 151:;
case 152:;
case 153:;
case 154:;
case 156:;
case 157:;
case 158:;
case 159:;
case 161:;
case 162:;
case 163:;
case 164:;
case 165:;
case 166:;
case 168:;
case 169:;
case 171:;
case 172:;
case 173:;
case 174:;
case 175:;
case 177:;
case 178:;
case 179:;
case 180:;
case 181:;
case 182:;
case 183:;
case 184:;
case 186:;
case 187:;
case 188:;
case 189:;
case 190:;
case 191:;
case 192:;
case 194:;
case 195:;
case 196:;
case 197:;
case 199:;
case 200:;
case 201:;
case 202:;
case 203:;
case 204:;
case 205:;
case 206:;
case 207:;
case 211:;
case 212:;
case 213:;
case 214:;
case 215:;
case 217:;
case 218:;
case 220:;
case 221:;
case 222:;
case 223:;
case 224:;
case 227:;
case 228:;
case 230:;
case 231:;
case 232:;
case 234:;
case 235:;
case 239:;
case 245:;
case 246:;
case 252:;
case 253:;
case 256:;
case 267:;
case 268:;
case 269:;
case 272:;
case 281:;
case 282:;
case 284:;
case 285:;
case 286:;
case 287:;
case 288:;
case 289:;
case 290:;
case 291:;
case 292:;
case 293:;
case 294:;
case 295:;
case 296:;
case 297:;
case 298:;
case 299:;
case 300:;
case 301:;
case 302:;
case 304:;
case 305:;
case 306:;
case 307:;
case 308:;
case 309:;
case 310:;
case 311:;
case 312:;
case 313:;
case 314:;
case 315:;
case 317:;
case 318:;
case 319:;
case 320:;
case 321:;
case 322:;
case 323:;
case 324:;
case 325:;
case 326:;
case 327:;
case 328:;
case 329:;
case 330:;
case 331:;
case 332:;
case 333:;
case 334:;
case 336:;
case 337:;
case 338:;
case 339:;
case 340:;
case 341:;
case 342:;
BalsaScan_Attribute.Position.Line   = yyLineCount;
BalsaScan_Attribute.Position.Column = (unsigned char *) BalsaScan_TokenPtr - yyLineStart;
/* line 287 "Balsa.rex" */
{
	char nextChar = BalsaScan_TokenPtr[BalsaScan_TokenLength];
	BalsaScan_TokenPtr[BalsaScan_TokenLength] = '\0';
	BalsaScan_Attribute.ident.ident = MakeIdent1 (BalsaScan_TokenPtr);
	BalsaScan_TokenPtr[BalsaScan_TokenLength] = nextChar;
	return 3;

} yyy110: goto yyBegin;
case 29:;
{/* BlankAction */
while (* yyChBufferIndexReg ++ == ' ') ;
BalsaScan_TokenPtr = (char *) -- yyChBufferIndexReg;
yyState = yyStartState;
yyStatePtr = & yyStateStack [1];
goto yyContinue;
} yyy111: goto yyBegin;
case 27:;
{/* EolAction */
yyLineCount ++;
yyLineStart = yyChBufferIndexReg - 1;
} yyy112: goto yyBegin;
case 1:;
case 2:;
case 3:;
case 4:;
case 5:;
case 6:;
case 8:;
case 19:;
case 22:;
case 23:;
case 24:;
case 347:;
case 350:;
case 352:;
case 354:;
	 /* non final states */
	 yyChBufferIndexReg --;			/* return character */
	 break;
 
case 26:
	 BalsaScan_Attribute.Position.Line   = yyLineCount;
	 BalsaScan_Attribute.Position.Column = yyChBufferIndexReg - yyLineStart;
      /* BalsaScan_TokenLength   = 1; */
	 yyChBufferIndex = ++ yyChBufferIndexReg;
	 {
/* line 112 "Balsa.rex" */
 char badChar[2]; badChar[0] = *BalsaScan_TokenPtr; badChar[1] = '\0';
	LOG_ERROR (IllegalCharacter, MakeIdent1 (badChar), BalsaScan_Attribute.Position); 
	 }
	 goto yyBegin;
 
      case yyDNoState:				/* automatic initialization */
	 yyInitialize ();
	 yySourceFile = StdIn;
	 goto yyBegin;

case 25:
	 yyChBufferIndex = -- yyChBufferIndexReg; /* undo last state transition */
	 if (-- BalsaScan_TokenLength == 0) {		/* get previous state */
	    yyState = yyStartState;
	 } else {
	    yyState = * (yyStatePtr - 1);
	 }

	 if (yyChBufferIndex != & yyChBufferStart [yyBytesRead]) {
						/* end of buffer sentinel in buffer */
	    if ((yyState = yyEobTrans [yyState]) == yyDNoState) continue;
	    yyChBufferIndexReg ++;
	    * yyStatePtr ++ = yyState;		/* push state */
	    goto yyContinue;
	 }
						/* end of buffer reached */
	 {  /* copy initial part of token in front of the input buffer */
	    register char * yySource = BalsaScan_TokenPtr;
	    register char * yyTarget = BalsaScan_TokenPtr = (char *) & yyChBufferPtr [yyMaxAlign - BalsaScan_TokenLength % yyMaxAlign];
	    if (yySource != yyTarget) {
	       while (yySource < (char *) yyChBufferIndexReg) * yyTarget ++ = * yySource ++;
	       yyLineStart += (unsigned char *) yyTarget - yyChBufferStart - yyBytesRead;
	       yyChBufferStart = (unsigned char *) yyTarget;
	    } else {
	       yyChBufferStart = yyChBufferIndexReg;
	    }
	 }

	 if (! yyEof) {				/* read buffer and restart */
	    int yyChBufferFree = (int) Exp2 (Log2 (yyChBufferSize - 4 - yyMaxAlign - BalsaScan_TokenLength));
	    if (yyChBufferFree < yyChBufferSize / 8) {
	       register int yyDelta;
	       register unsigned char * yyOldChBufferPtr = yyChBufferPtr;
	       ExtendArray ((char * *) & yyChBufferPtr, & yyChBufferSize, sizeof (char));
	       if (yyChBufferPtr == NULL) yyErrorMessage (1);
	       yyDelta = yyChBufferPtr - yyOldChBufferPtr;
	       yyChBufferStart	+= yyDelta;
	       yyLineStart	+= yyDelta;
	       BalsaScan_TokenPtr	+= yyDelta;
	       yyChBufferFree = (int) Exp2 (Log2 (yyChBufferSize - 4 - yyMaxAlign - BalsaScan_TokenLength));
	       if (yyStateStackSize < yyChBufferSize) {
		  yyStateRange * yyOldStateStack = yyStateStack;
		  ExtendArray_FIXED ((char * *) & yyStateStack, & yyStateStackSize, sizeof (yyStateRange));
		  if (yyStateStack == NULL) yyErrorMessage (1);
		  yyStatePtr	+= yyStateStack - yyOldStateStack;
	       }
	    }
	    yyChBufferIndex = yyChBufferIndexReg = yyChBufferStart;
	    yyBytesRead = BalsaScan_GetLine (yySourceFile, (char *) yyChBufferIndex, yyChBufferFree);
	    if (yyBytesRead <= 0) { yyBytesRead = 0; yyEof = true; }
	    yyChBufferStart [yyBytesRead    ] = yyEobCh;
	    yyChBufferStart [yyBytesRead + 1] = '\0';
	    goto yyContinue;
	 }

	 if (BalsaScan_TokenLength == 0) {		/* end of file reached */
	    BalsaScan_Attribute.Position.Line   = yyLineCount;
	    BalsaScan_Attribute.Position.Column = yyChBufferIndexReg - yyLineStart;
	    BalsaScan_CloseFile ();
	    if (yyFileStackPtr == yyFileStack) {
/* line 116 "Balsa.rex" */
 if (yyStartState == CMNT) LOG_ERROR (UnexpectedEOFInComment, NoIdent, BalsaScan_Attribute.Position); 
	    }
	    if (yyFileStackPtr == yyFileStack) return BalsaScan_EofToken;
	    goto yyBegin;
	 }
	 break;
 
      default:
	 yyErrorMessage (0);
      }
   }
}
 
void BalsaScan_BeginFile
# if defined __STDC__ | defined __cplusplus
   (char * yyFileName)
# else
   (yyFileName) char * yyFileName;
# endif
   {
      yyInitialize ();
      yySourceFile = BalsaScan_BeginSource (yyFileName);
      if (yySourceFile < 0) yyErrorMessage (4);
   }

static void yyInitialize ()
   {
      if (yyFileStackPtr >= yyFileStack + yyFileStackSize) yyErrorMessage (2);
      yyFileStackPtr ++;			/* push file */
      yyFileStackPtr->yySourceFile	= yySourceFile		;
      yyFileStackPtr->yyEof		= yyEof			;
      yyFileStackPtr->yyChBufferPtr	= yyChBufferPtr		;
      yyFileStackPtr->yyChBufferStart	= yyChBufferStart	;
      yyFileStackPtr->yyChBufferSize	= yyChBufferSize	;
      yyFileStackPtr->yyChBufferIndex	= yyChBufferIndex	;
      yyFileStackPtr->yyBytesRead	= yyBytesRead		;
      yyFileStackPtr->yyLineCount	= yyLineCount		;
      yyFileStackPtr->yyLineStart	= yyLineStart		;
						/* initialize file state */
      yyChBufferSize	   = yyInitBufferSize;
      MakeArray ((char * *) & yyChBufferPtr, & yyChBufferSize, sizeof (char));
      if (yyChBufferPtr == NULL) yyErrorMessage (1);
      yyChBufferStart	   = & yyChBufferPtr [yyMaxAlign];
      yyChBufferStart [-1] = yyEolCh;		/* begin of line indicator */
      yyChBufferStart [ 0] = yyEobCh;		/* end of buffer sentinel */
      yyChBufferStart [ 1] = '\0';
      yyChBufferIndex	   = yyChBufferStart;
      yyEof		   = false;
      yyBytesRead	   = 0;
      yyLineCount	   = 1;
      yyLineStart	   = & yyChBufferStart [-1];
      if (yyStartState == 0) yyStartState = STD;
      yyStateStack [0]	   = yyDefaultState;	/* stack underflow sentinel */
   }

void BalsaScan_CloseFile ()
   {
      if (yyFileStackPtr == yyFileStack) yyErrorMessage (3);
      BalsaScan_CloseSource (yySourceFile);
      ReleaseArray ((char * *) & yyChBufferPtr, & yyChBufferSize, sizeof (char));
						/* pop file */
      yySourceFile	= yyFileStackPtr->yySourceFile		;
      yyEof		= yyFileStackPtr->yyEof			;
      yyChBufferPtr	= yyFileStackPtr->yyChBufferPtr		;
      yyChBufferStart	= yyFileStackPtr->yyChBufferStart	;
      yyChBufferSize	= yyFileStackPtr->yyChBufferSize	;
      yyChBufferIndex	= yyFileStackPtr->yyChBufferIndex	;
      yyBytesRead	= yyFileStackPtr->yyBytesRead		;
      yyLineCount	= yyFileStackPtr->yyLineCount		;
      yyLineStart	= yyFileStackPtr->yyLineStart		;
      yyFileStackPtr --;		
   }
 
int BalsaScan_GetWord
# if defined __STDC__ | defined __cplusplus
   (char * yyWord)
# else
   (yyWord) char * yyWord;
# endif
   {
      register char * yySource			= BalsaScan_TokenPtr;
      register char * yyTarget			= yyWord;
      register char * yyChBufferIndexReg	= (char *) yyChBufferIndex;
   
      do {				/* ASSERT word is not empty */
	 * yyTarget ++ = * yySource ++;
      } while (yySource < yyChBufferIndexReg);
      * yyTarget = '\0';
      return yyChBufferIndexReg - BalsaScan_TokenPtr;
   }
 
int BalsaScan_GetLower
# if defined __STDC__ | defined __cplusplus
   (char * yyWord)
# else
   (yyWord) char * yyWord;
# endif
   {
      register char * yySource			= BalsaScan_TokenPtr;
      register char * yyTarget			= yyWord;
      register char * yyChBufferIndexReg	= (char *) yyChBufferIndex;
   
      do {				/* ASSERT word is not empty */
	 * yyTarget ++ = yyToLower [* yySource ++];
      } while (yySource < yyChBufferIndexReg);
      * yyTarget = '\0';
      return yyChBufferIndexReg - BalsaScan_TokenPtr;
   }
 
int BalsaScan_GetUpper
# if defined __STDC__ | defined __cplusplus
   (char * yyWord)
# else
   (yyWord) char * yyWord;
# endif
   {
      register char * yySource			= BalsaScan_TokenPtr;
      register char * yyTarget			= yyWord;
      register char * yyChBufferIndexReg	= (char *) yyChBufferIndex;
   
      do {				/* ASSERT word is not empty */
	 * yyTarget ++ = yyToUpper [* yySource ++];
      } while (yySource < yyChBufferIndexReg);
      * yyTarget = '\0';
      return yyChBufferIndexReg - BalsaScan_TokenPtr;
   }
 
static char input ()
   {
      if (yyChBufferIndex == & yyChBufferStart [yyBytesRead]) {
	 if (! yyEof) {
	    yyLineStart -= yyBytesRead;
	    yyChBufferIndex = yyChBufferStart = yyChBufferPtr;
	    yyBytesRead = BalsaScan_GetLine (yySourceFile, (char *) yyChBufferIndex,
	       (int) Exp2 (Log2 (yyChBufferSize)));
	    if (yyBytesRead <= 0) { yyBytesRead = 0; yyEof = true; }
	    yyChBufferStart [yyBytesRead    ] = yyEobCh;
	    yyChBufferStart [yyBytesRead + 1] = '\0';
	 }
      }
      if (yyChBufferIndex == & yyChBufferStart [yyBytesRead]) return '\0';
      else return * yyChBufferIndex ++;
   }

void BalsaScan_BeginScanner ()
   {
   }
 
void BalsaScan_CloseScanner ()
   {
   }
 
static void yyErrorMessage
# if defined __STDC__ | defined __cplusplus
   (int yyErrorCode)
# else
   (yyErrorCode) int yyErrorCode;
# endif
   {
      WritePosition (stderr, BalsaScan_Attribute.Position);
      switch (yyErrorCode) {
      case 0: (void) fprintf (stderr, ": BalsaScan: internal error\n"); break;
      case 1: (void) fprintf (stderr, ": BalsaScan: out of memory\n"); break;
      case 2: (void) fprintf (stderr, ": BalsaScan: too many nested include files\n"); break;
      case 3: (void) fprintf (stderr, ": BalsaScan: file stack underflow (too many calls of BalsaScan_CloseFile)\n"); break;
      case 4: (void) fprintf (stderr, ": BalsaScan: cannot open input file\n"); break;
      }
      BalsaScan_Exit ();
   }
