/* $Id: Parser.c,v 2.15 1993/09/13 12:35:16 grosch rel $ */

# define bool		char
# define true		1
# define false		0

# include "Breeze.h"

# ifdef __cplusplus
extern "C" {
#    include "rMemory.h"
#    include "DynArray.h"
#    include "Sets.h"
#    include "Errors.h"
#    include <string.h>
#    ifndef BCOPY
#       include <memory.h>
#    endif
}
# else
#    include "rMemory.h"
#    include "DynArray.h"
#    include "Sets.h"
#    include "Errors.h"
#    ifndef BCOPY
#       include <memory.h>
#    endif
# endif

# if defined __STDC__ | defined __cplusplus
# define ARGS(parameters)	parameters
# else
# define ARGS(parameters)	()
# endif

# ifdef lex_interface
#    define BreezeScan_GetToken	yylex
     extern int yylex ();
#    ifndef AttributeDef
#	include "Positions.h"
        typedef struct { tPosition Position; } BreezeScan_tScanAttribute;
        BreezeScan_tScanAttribute	BreezeScan_Attribute = {{ 0, 0 }};
#    endif
#    ifndef ErrorAttributeDef
#	define BreezeScan_ErrorAttribute(Token, RepairAttribute)
#    endif
#    ifndef yyGetAttribute
#	define yyGetAttribute(yyAttrStackPtr, Attribute) * yyAttrStackPtr = yylval
#    endif
# else
#    include "BreezeScan.h"
#    ifndef yyGetAttribute
#	define yyGetAttribute(yyAttrStackPtr, Attribute) (yyAttrStackPtr)->Scan = Attribute
#    endif
# endif

/* line 2 "Breeze.lalr" */


#include "BreezeScan.h"
#include "Tree.h"
#include "BreezeScanSource.h"
#include "arith.h"
#include "ports.h"
#include "implicants.h"
#include "wires.h"
#include "callcontexts.h"
#include <string.h>

  tIdent nameIdent;


#include "Tree.h"

typedef struct { tTree Tree; } yybreeze_file;
typedef struct { tTree Tree; } yybreeze_decls;
typedef struct { tTree Tree; } yybreeze_decl;
typedef struct { tTree Tree; } yybreeze_type;
typedef struct { tTree Tree; } yybreeze_record_elems;
typedef struct { tTree Tree; } yybreeze_enum_elems;
typedef struct { tTree Tree; } yypart_component;
typedef struct { tTree Tree; } yybreeze_param_decls;
typedef struct { tTree Tree; } yybreeze_param_decl;
typedef struct { tTree Tree; } yycomp_parameter;
typedef struct { tTree Tree; } yypart_port;
typedef struct { bool isPull; } yypart_channel_sense;
typedef struct { PtrintList channelNos; } yychannel_numbers;
typedef struct { tTree Tree; } yypart_components;
typedef struct { PtrCallContextList callcontexts; } yypart_optional_callcontexts;
typedef struct { PtrCallContextList callcontexts; } yypart_callcontexts;
typedef struct { tTree Tree; } yypart_ports;
typedef struct { PtrWireList channels; } yypart_channels;
typedef struct { tTree Tree; } yycomp_parameters;
typedef struct { tPosition Position; tTree type; PtrLispList list; } yybreeze_options;
typedef struct { tPosition Position; tTree type; PtrLispList tokens; } yybreeze_option;
typedef struct { PtrLispList tokens; } yylisp_tokens;
typedef struct { Lisp token; } yylisp_token;
typedef struct { tPosition Position; } yyposition;
typedef struct { tTree Tree; } yydecl;
typedef struct { bool multicast; } yychannel_options;
typedef struct { tTree Tree; } yyrange;
typedef struct { tTree Tree; } yytype;
typedef struct { tTree Tree; } yyval_decls;
typedef struct { tTree Tree; } yyval_decls_body;
typedef struct { tTree Tree; } yyval_decl;
typedef struct { tTree Tree; } yyexpr;
typedef struct { PortSense portSense; } yyport_sense;
typedef struct { bool isOutput; } yyport_direction;
typedef struct { tTree Tree; } yyport;
typedef struct { tTree Tree; } yyblock;
typedef struct { tTree Tree; } yycommand;
typedef struct { tTree Tree; } yyprocedure_params;
typedef struct { tTree Tree; } yyprocedure_params_body;
typedef struct { tTree Tree; } yyprocedure_param;
typedef struct { tTree Tree; } yyfunction_params;
typedef struct { tTree Tree; } yyfunction_params_body;
typedef struct { tTree Tree; } yyfunction_param;
typedef struct { bool isParallel; bool isPermissive; } yypar_seq;
typedef struct { tTree Tree; } yyguard;
typedef struct { tTree Tree; } yyport_guard;
typedef struct { tTree Tree; } yydecl_guard;
typedef struct { tTree Tree; } yychannel_guard;
typedef struct { tTree Tree; } yycase_guard;
typedef struct { tTree Tree; } yylvalue;
typedef struct { tTree Tree; } yydecls;
typedef struct { tTree Tree; } yydecls_body;
typedef struct { tTree Tree; } yyidents;
typedef struct { tTree Tree; } yyidents_body;
typedef struct { tTree Tree; } yyexprs;
typedef struct { tTree Tree; } yyexprs_body;
typedef struct { tTree Tree; } yylvalues;
typedef struct { tTree Tree; } yylvalues_body;
typedef struct { tTree Tree; } yycase_match_elem;
typedef struct { tTree Tree; } yycase_matches;
typedef struct { tTree Tree; } yycase_matches_body;
typedef struct { tTree Tree; } yyformal_ports;
typedef struct { tTree Tree; } yyformal_ports_body;
typedef struct { tTree Tree; } yyguards;
typedef struct { tTree Tree; } yyguards_body;
typedef struct { tTree Tree; } yyport_guards;
typedef struct { tTree Tree; } yyport_guards_body;
typedef struct { tTree Tree; } yydecl_guards;
typedef struct { tTree Tree; } yydecl_guards_body;
typedef struct { tTree Tree; } yycase_guards;
typedef struct { tTree Tree; } yycase_guards_body;
typedef struct { tTree Tree; } yychannel_guards;
typedef struct { tTree Tree; } yychannel_guards_body;
typedef struct { tTree Tree; } yyrecord_elems;
typedef struct { tTree Tree; } yyrecord_elems_body;
typedef struct { tTree Tree; } yyenum_elems;
typedef struct { tTree Tree; } yyenum_elems_body;
typedef struct { tTree Tree; } yyenum_elem;
typedef struct { tTree Tree; } yyrecord_elem;
typedef struct { tTree Tree; } yyident;
typedef struct { tTree Tree; } yylinked_expr;
typedef struct { tTree Tree; } yylinked_lvalue;

typedef union {
 BreezeScan_tScanAttribute Scan;
 yybreeze_file breeze_file;
 yybreeze_decls breeze_decls;
 yybreeze_decl breeze_decl;
 yybreeze_type breeze_type;
 yybreeze_record_elems breeze_record_elems;
 yybreeze_enum_elems breeze_enum_elems;
 yypart_component part_component;
 yybreeze_param_decls breeze_param_decls;
 yybreeze_param_decl breeze_param_decl;
 yycomp_parameter comp_parameter;
 yypart_port part_port;
 yypart_channel_sense part_channel_sense;
 yychannel_numbers channel_numbers;
 yypart_components part_components;
 yypart_optional_callcontexts part_optional_callcontexts;
 yypart_callcontexts part_callcontexts;
 yypart_ports part_ports;
 yypart_channels part_channels;
 yycomp_parameters comp_parameters;
 yybreeze_options breeze_options;
 yybreeze_option breeze_option;
 yylisp_tokens lisp_tokens;
 yylisp_token lisp_token;
 yyposition position;
 yydecl decl;
 yychannel_options channel_options;
 yyrange range;
 yytype type;
 yyval_decls val_decls;
 yyval_decls_body val_decls_body;
 yyval_decl val_decl;
 yyexpr expr;
 yyport_sense port_sense;
 yyport_direction port_direction;
 yyport port;
 yyblock block;
 yycommand command;
 yyprocedure_params procedure_params;
 yyprocedure_params_body procedure_params_body;
 yyprocedure_param procedure_param;
 yyfunction_params function_params;
 yyfunction_params_body function_params_body;
 yyfunction_param function_param;
 yypar_seq par_seq;
 yyguard guard;
 yyport_guard port_guard;
 yydecl_guard decl_guard;
 yychannel_guard channel_guard;
 yycase_guard case_guard;
 yylvalue lvalue;
 yydecls decls;
 yydecls_body decls_body;
 yyidents idents;
 yyidents_body idents_body;
 yyexprs exprs;
 yyexprs_body exprs_body;
 yylvalues lvalues;
 yylvalues_body lvalues_body;
 yycase_match_elem case_match_elem;
 yycase_matches case_matches;
 yycase_matches_body case_matches_body;
 yyformal_ports formal_ports;
 yyformal_ports_body formal_ports_body;
 yyguards guards;
 yyguards_body guards_body;
 yyport_guards port_guards;
 yyport_guards_body port_guards_body;
 yydecl_guards decl_guards;
 yydecl_guards_body decl_guards_body;
 yycase_guards case_guards;
 yycase_guards_body case_guards_body;
 yychannel_guards channel_guards;
 yychannel_guards_body channel_guards_body;
 yyrecord_elems record_elems;
 yyrecord_elems_body record_elems_body;
 yyenum_elems enum_elems;
 yyenum_elems_body enum_elems_body;
 yyenum_elem enum_elem;
 yyrecord_elem record_elem;
 yyident ident;
 yylinked_expr linked_expr;
 yylinked_lvalue linked_lvalue;
} tParsAttribute;


# if defined lex_interface & ! defined yylvalDef
     tParsAttribute yylval;
# endif

# ifndef yyInitStackSize
#    define yyInitStackSize	100
# endif
# define yyNoState		0

# define yyFirstTerminal	0
# define yyLastTerminal		170
# define yyTableMax		728
# define yyNTableMax		660
# define yyFirstReadState	1
# define yyLastReadState	707
# define yyFirstReadReduceState	708
# define yyLastReadReduceState	935
# define yyFirstReduceState	936
# define yyLastReduceState	1184
# define yyStartState		1
# define yyStopState		936

# define yyFirstFinalState	yyFirstReadReduceState

typedef unsigned short	yyStateRange	;
typedef unsigned short	yySymbolRange	;
typedef struct	{ yyStateRange Check, Next; } yyTCombType ;

	char *		Breeze_TokenName	[yyLastTerminal + 2] = {
"_EndOfFile",
"boolean",
"breeze_literal",
"breeze_keyword",
"breeze_ident",
"(",
"import",
")",
"file",
"constant",
"named-type",
"alias-type",
"numeric-type",
"array-type",
"record-type",
"enumeration-type",
"builtin-type",
"implicant",
"type",
"balsa",
"type-decl",
"at",
"existing-type",
"literal-expr",
"ident-expr",
"string-expr",
"dont-care-expr",
"implicant-expr",
"named-aggr-cons-expr",
"exprs",
"aggr-cons-expr",
"enum-elem-expr",
"unary-expr",
"sizeof-expr",
"binary-expr",
"record-elem-extract-expr",
"array-extract-expr",
"array-slice-expr",
"range1",
"range2",
"type-range",
"as-expr",
"bit-array-cast-expr",
"let-expr",
"val-decls",
"val-decl",
"function-call-expr",
"function-params",
"expr-func-param",
"type-func-param",
"array-append-expr",
"new-type",
"record-elems",
"record-elem",
"idents",
"bounded-record-type",
"enum-elems",
"enum-elem",
"valued-enum-elem",
"bounded-enumeration-type",
"typed-constant-decl",
"untyped-constant-decl",
"variable-decl",
"init-variable-decl",
"channel-decl",
"multicast",
"arrayed-channel-decl",
"sync-decl",
"arrayed-sync-decl",
"procedure-decl",
"formal-ports",
"value-port",
"port",
"default",
"active",
"passive",
"input",
"output",
"arrayed-port",
"sync-port",
"arrayed-sync-port",
"param-port",
"type-param-port",
"if-ports",
"port-guards",
"port-guard",
"if-else-ports",
"block",
"decls",
"continue",
"halt",
"ident-lvalue",
"record-elem-lvalue",
"array-elem-lvalue",
"array-slice-lvalue",
"array-append-lvalue",
"array-cons-lvalue",
"lvalues",
"block-lvalue",
"channel-lvalue",
"renamed-channel-lvalue",
"as-lvalue",
"bit-array-cast-lvalue",
"input-enclose",
"input-enclose-bang",
"sync",
"assign",
"block-command",
"sequential",
"parallel",
"permissive-parallel",
"loop",
"while-guards",
"guards",
"guard",
"while-guards-also",
"command-while-expr",
"command-while-guards",
"command-while-guards-also",
"if",
"if-else",
"case",
"case-guards",
"case-match-guard",
"case-matches",
"case-range",
"case-implicant",
"for-case-guard",
"case-else",
"for",
"procedure-call",
"procedure-params",
"expr-proc-param",
"type-proc-param",
"block-proc-param",
"var-read-proc-param",
"var-write-proc-param",
"labelled-command",
"select",
"channel-guards",
"channel-guard",
"select!",
"arbitrate",
"print",
"sink",
"procedure-alias-decl",
"procedure-param-alias-decl",
"typed-function-decl",
"untyped-function-decl",
"builtin-function-decl",
"shared-decl",
"if-decls",
"decl-guards",
"decl-guard",
"if-else-decls",
"print-decl",
"breeze-part",
"ports",
"attributes",
"channels",
"push",
"pull",
"components",
"component",
"undeclared-component",
"implements",
"parameters",
"parameter",
"type-parameter",
"call-contexts",
"call-context",
""
};
static	yyTCombType	yyTComb		[yyTableMax + 1] = {
{1, 938},
{2, 937},
{6, 7},
{10, 11},
{4, 5},
{1, 938},
{2, 3},
{3, 4},
{5, 708},
{3, 6},
{3, 9},
{7, 8},
{8, 709},
{9, 10},
{13, 14},
{14, 710},
{15, 16},
{16, 711},
{3, 40},
{3, 45},
{3, 48},
{12, 13},
{12, 15},
{12, 17},
{12, 20},
{12, 25},
{12, 31},
{12, 38},
{17, 18},
{18, 19},
{19, 712},
{21, 22},
{22, 23},
{23, 713},
{24, 714},
{25, 26},
{26, 955},
{27, 28},
{26, 955},
{27, 716},
{28, 29},
{30, 715},
{31, 32},
{32, 33},
{33, 957},
{34, 35},
{33, 957},
{34, 718},
{35, 36},
{36, 37},
{37, 717},
{38, 719},
{39, 720},
{40, 41},
{41, 42},
{42, 43},
{44, 721},
{45, 46},
{47, 722},
{48, 49},
{49, 50},
{51, 52},
{52, 53},
{53, 54},
{55, 56},
{54, 55},
{56, 723},
{52, 725},
{54, 724},
{57, 58},
{61, 62},
{59, 60},
{59, 169},
{59, 173},
{59, 189},
{59, 204},
{65, 66},
{66, 726},
{68, 69},
{69, 727},
{71, 72},
{59, 206},
{72, 728},
{74, 729},
{76, 77},
{77, 78},
{78, 730},
{80, 81},
{82, 83},
{84, 1143},
{85, 731},
{84, 1143},
{86, 732},
{89, 733},
{91, 92},
{92, 93},
{93, 734},
{95, 96},
{97, 735},
{99, 100},
{49, 221},
{49, 226},
{49, 230},
{49, 234},
{49, 239},
{100, 736},
{49, 244},
{49, 250},
{49, 254},
{49, 259},
{59, 209},
{102, 103},
{105, 737},
{108, 109},
{59, 212},
{109, 738},
{113, 739},
{120, 740},
{59, 216},
{63, 64},
{63, 67},
{63, 70},
{63, 73},
{63, 75},
{63, 79},
{124, 741},
{63, 87},
{63, 90},
{63, 94},
{63, 98},
{63, 101},
{63, 106},
{63, 110},
{63, 114},
{117, 118},
{117, 121},
{117, 125},
{63, 129},
{63, 133},
{63, 136},
{127, 742},
{128, 743},
{63, 149},
{132, 744},
{135, 745},
{137, 138},
{63, 164},
{138, 139},
{140, 1039},
{141, 142},
{140, 1039},
{141, 747},
{142, 143},
{144, 145},
{146, 746},
{148, 748},
{150, 151},
{3, 597},
{151, 152},
{152, 153},
{154, 1115},
{155, 156},
{154, 1115},
{155, 751},
{156, 157},
{156, 160},
{159, 749},
{162, 750},
{163, 752},
{167, 753},
{168, 754},
{172, 755},
{175, 176},
{177, 178},
{178, 179},
{181, 182},
{183, 756},
{184, 757},
{186, 758},
{187, 759},
{188, 760},
{191, 192},
{193, 194},
{194, 195},
{194, 198},
{49, 547},
{49, 551},
{49, 556},
{49, 562},
{49, 567},
{49, 572},
{49, 576},
{196, 197},
{197, 761},
{49, 588},
{49, 592},
{199, 200},
{201, 762},
{202, 763},
{203, 764},
{205, 765},
{207, 208},
{208, 766},
{211, 767},
{213, 175},
{215, 768},
{217, 191},
{219, 769},
{220, 770},
{222, 223},
{225, 771},
{227, 228},
{229, 772},
{233, 773},
{238, 774},
{243, 776},
{249, 777},
{253, 778},
{257, 1024},
{258, 779},
{260, 261},
{262, 263},
{264, 1154},
{265, 266},
{264, 1154},
{265, 796},
{266, 267},
{266, 271},
{270, 780},
{276, 786},
{283, 787},
{287, 788},
{292, 789},
{266, 277},
{266, 284},
{266, 288},
{266, 293},
{266, 297},
{266, 300},
{296, 790},
{298, 181},
{266, 312},
{299, 791},
{302, 303},
{304, 305},
{305, 306},
{309, 792},
{310, 793},
{311, 794},
{313, 302},
{315, 795},
{317, 318},
{320, 321},
{322, 1137},
{323, 797},
{322, 1137},
{325, 326},
{325, 375},
{330, 331},
{331, 798},
{334, 335},
{328, 329},
{328, 332},
{328, 336},
{328, 340},
{328, 344},
{328, 348},
{335, 799},
{339, 800},
{325, 379},
{325, 381},
{328, 366},
{328, 370},
{343, 801},
{347, 802},
{350, 351},
{257, 775},
{352, 353},
{353, 354},
{353, 357},
{353, 360},
{356, 803},
{359, 804},
{325, 383},
{325, 387},
{325, 391},
{325, 394},
{325, 398},
{325, 401},
{325, 405},
{325, 409},
{325, 413},
{325, 416},
{362, 363},
{363, 805},
{325, 428},
{325, 432},
{325, 436},
{325, 440},
{325, 445},
{325, 448},
{325, 452},
{364, 806},
{365, 807},
{369, 808},
{372, 809},
{374, 810},
{378, 811},
{325, 481},
{325, 486},
{325, 492},
{380, 812},
{382, 813},
{386, 814},
{390, 815},
{393, 816},
{397, 817},
{325, 516},
{325, 520},
{400, 818},
{404, 819},
{325, 532},
{325, 535},
{325, 538},
{325, 542},
{408, 820},
{412, 821},
{415, 822},
{418, 419},
{420, 421},
{421, 422},
{425, 823},
{426, 824},
{427, 825},
{431, 826},
{435, 827},
{439, 828},
{444, 829},
{447, 830},
{449, 418},
{451, 831},
{455, 456},
{457, 458},
{458, 459},
{461, 462},
{463, 464},
{467, 832},
{458, 474},
{464, 465},
{464, 468},
{470, 833},
{471, 834},
{473, 835},
{475, 476},
{476, 461},
{478, 836},
{479, 837},
{480, 838},
{483, 455},
{485, 839},
{487, 840},
{487, 841},
{487, 842},
{488, 489},
{489, 117},
{491, 843},
{493, 494},
{495, 496},
{497, 1107},
{498, 499},
{497, 1107},
{498, 849},
{499, 500},
{499, 503},
{499, 506},
{499, 509},
{499, 512},
{502, 844},
{505, 845},
{508, 846},
{511, 847},
{513, 328},
{514, 848},
{515, 850},
{517, 518},
{519, 851},
{522, 523},
{524, 525},
{525, 526},
{527, 350},
{528, 325},
{529, 852},
{530, 853},
{531, 854},
{534, 855},
{536, 522},
{537, 856},
{541, 857},
{544, 858},
{545, 859},
{546, 860},
{548, 549},
{549, 550},
{550, 861},
{552, 553},
{553, 554},
{554, 495},
{555, 862},
{557, 558},
{561, 863},
{563, 564},
{566, 864},
{568, 569},
{569, 262},
{570, 59},
{571, 865},
{573, 574},
{574, 317},
{575, 866},
{578, 579},
{580, 581},
{581, 582},
{585, 867},
{586, 868},
{587, 869},
{589, 578},
{590, 320},
{591, 870},
{593, 63},
{594, 82},
{595, 871},
{596, 872},
{597, 598},
{598, 599},
{599, 600},
{601, 602},
{602, 603},
{607, 880},
{608, 609},
{610, 878},
{612, 877},
{601, 615},
{601, 624},
{601, 628},
{614, 879},
{615, 616},
{617, 784},
{617, 785},
{619, 620},
{620, 621},
{623, 881},
{624, 625},
{627, 882},
{608, 613},
{628, 629},
{629, 781},
{629, 782},
{629, 783},
{630, 631},
{631, 632},
{634, 883},
{635, 636},
{636, 637},
{637, 638},
{639, 640},
{640, 641},
{641, 642},
{642, 985},
{643, 644},
{642, 985},
{643, 650},
{644, 645},
{646, 884},
{647, 648},
{649, 887},
{650, 651},
{651, 652},
{652, 977},
{653, 654},
{652, 977},
{653, 694},
{654, 655},
{654, 666},
{655, 656},
{656, 657},
{658, 659},
{659, 660},
{661, 664},
{663, 891},
{665, 892},
{666, 667},
{667, 668},
{668, 988},
{670, 671},
{668, 988},
{668, 988},
{669, 888},
{668, 988},
{669, 889},
{671, 974},
{672, 890},
{669, 670},
{671, 974},
{672, 662},
{671, 974},
{672, 673},
{673, 674},
{674, 675},
{675, 676},
{676, 677},
{677, 678},
{678, 679},
{679, 680},
{680, 961},
{681, 682},
{680, 961},
{681, 688},
{682, 683},
{682, 686},
{683, 684},
{684, 12},
{685, 893},
{686, 687},
{687, 894},
{688, 689},
{689, 690},
{644, 885},
{644, 886},
{690, 601},
{691, 692},
{692, 990},
{693, 608},
{692, 990},
{693, 895},
{694, 695},
{695, 696},
{694, 979},
{696, 981},
{697, 698},
{696, 981},
{697, 897},
{698, 699},
{699, 700},
{700, 701},
{702, 51},
{703, 704},
{704, 995},
{704, 995},
{704, 995},
{704, 995},
{704, 995},
{706, 898},
{704, 995},
{705, 873},
{705, 874},
{705, 875},
{705, 876},
{705, 611},
{707, 936},
{705, 896},
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
{0, 0},
{0, 0},
{0, 0},
{0, 0},
{0, 0},
};
static	unsigned short	yyNComb		[yyNTableMax - yyLastTerminal] = {
2,
707,
935,
39,
21,
24,
30,
44,
47,
596,
27,
34,
912,
606,
607,
614,
619,
622,
273,
279,
274,
280,
286,
57,
61,
65,
68,
220,
168,
71,
74,
76,
80,
84,
899,
88,
91,
86,
89,
95,
97,
99,
102,
104,
105,
107,
108,
111,
112,
113,
115,
116,
128,
119,
120,
122,
123,
124,
126,
127,
130,
131,
132,
134,
135,
137,
85,
147,
140,
141,
901,
144,
146,
900,
148,
150,
163,
154,
155,
902,
158,
159,
161,
162,
165,
166,
167,
170,
171,
172,
174,
188,
177,
187,
906,
180,
185,
183,
184,
904,
903,
186,
905,
190,
203,
193,
202,
908,
196,
199,
201,
907,
205,
207,
210,
211,
213,
214,
215,
217,
218,
219,
222,
224,
225,
227,
229,
231,
232,
233,
235,
236,
237,
238,
240,
241,
242,
243,
245,
246,
247,
248,
249,
251,
252,
253,
255,
256,
257,
258,
260,
316,
264,
265,
911,
268,
269,
270,
272,
275,
276,
278,
281,
282,
283,
285,
287,
289,
290,
291,
292,
294,
295,
296,
298,
299,
301,
311,
304,
909,
307,
308,
309,
910,
313,
314,
315,
546,
319,
324,
322,
323,
545,
327,
373,
310,
330,
333,
334,
337,
338,
339,
341,
342,
343,
345,
346,
347,
349,
365,
352,
364,
914,
355,
356,
358,
359,
361,
362,
913,
367,
368,
369,
371,
372,
374,
376,
377,
378,
380,
382,
384,
385,
386,
388,
389,
390,
392,
393,
395,
396,
397,
399,
400,
402,
403,
404,
406,
407,
408,
410,
411,
412,
414,
415,
417,
427,
420,
915,
423,
424,
425,
916,
429,
430,
431,
433,
434,
435,
437,
438,
439,
441,
442,
426,
443,
444,
446,
447,
449,
450,
451,
453,
454,
480,
457,
919,
460,
472,
463,
917,
471,
466,
467,
469,
470,
918,
473,
475,
477,
479,
478,
920,
482,
483,
484,
485,
487,
488,
490,
491,
493,
515,
497,
498,
921,
501,
502,
504,
505,
507,
508,
510,
511,
513,
514,
517,
519,
521,
531,
524,
922,
527,
528,
529,
923,
533,
534,
536,
537,
539,
540,
541,
543,
544,
548,
552,
555,
530,
557,
559,
560,
561,
563,
565,
566,
568,
570,
571,
573,
575,
577,
587,
580,
924,
583,
584,
585,
925,
589,
590,
591,
593,
594,
595,
635,
604,
605,
610,
927,
586,
928,
926,
612,
617,
618,
623,
626,
627,
932,
630,
633,
634,
931,
639,
643,
647,
646,
649,
653,
934,
658,
929,
661,
663,
665,
669,
672,
681,
933,
685,
691,
693,
706,
697,
702,
930,
703,
705,
0,
0,
0,
0,
0,
0,
932,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
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
static	yyTCombType *	yyTBasePtr	[yyLastReadState + 1] = { 0,
& yyTComb [0],
& yyTComb [1],
& yyTComb [1],
& yyTComb [0],
& yyTComb [1],
& yyTComb [0],
& yyTComb [7],
& yyTComb [5],
& yyTComb [9],
& yyTComb [1],
& yyTComb [0],
& yyTComb [11],
& yyTComb [10],
& yyTComb [8],
& yyTComb [12],
& yyTComb [10],
& yyTComb [27],
& yyTComb [27],
& yyTComb [23],
& yyTComb [0],
& yyTComb [29],
& yyTComb [30],
& yyTComb [26],
& yyTComb [27],
& yyTComb [33],
& yyTComb [31],
& yyTComb [32],
& yyTComb [36],
& yyTComb [0],
& yyTComb [34],
& yyTComb [41],
& yyTComb [41],
& yyTComb [39],
& yyTComb [40],
& yyTComb [44],
& yyTComb [47],
& yyTComb [43],
& yyTComb [44],
& yyTComb [45],
& yyTComb [49],
& yyTComb [52],
& yyTComb [53],
& yyTComb [0],
& yyTComb [49],
& yyTComb [53],
& yyTComb [0],
& yyTComb [51],
& yyTComb [54],
& yyTComb [40],
& yyTComb [0],
& yyTComb [40],
& yyTComb [60],
& yyTComb [61],
& yyTComb [61],
& yyTComb [62],
& yyTComb [59],
& yyTComb [65],
& yyTComb [0],
& yyTComb [59],
& yyTComb [0],
& yyTComb [69],
& yyTComb [0],
& yyTComb [96],
& yyTComb [0],
& yyTComb [74],
& yyTComb [70],
& yyTComb [0],
& yyTComb [74],
& yyTComb [72],
& yyTComb [0],
& yyTComb [76],
& yyTComb [75],
& yyTComb [0],
& yyTComb [76],
& yyTComb [0],
& yyTComb [82],
& yyTComb [83],
& yyTComb [79],
& yyTComb [0],
& yyTComb [83],
& yyTComb [0],
& yyTComb [59],
& yyTComb [0],
& yyTComb [84],
& yyTComb [83],
& yyTComb [85],
& yyTComb [0],
& yyTComb [0],
& yyTComb [86],
& yyTComb [0],
& yyTComb [90],
& yyTComb [91],
& yyTComb [89],
& yyTComb [0],
& yyTComb [93],
& yyTComb [0],
& yyTComb [91],
& yyTComb [0],
& yyTComb [95],
& yyTComb [98],
& yyTComb [0],
& yyTComb [107],
& yyTComb [0],
& yyTComb [0],
& yyTComb [105],
& yyTComb [0],
& yyTComb [0],
& yyTComb [109],
& yyTComb [108],
& yyTComb [0],
& yyTComb [0],
& yyTComb [0],
& yyTComb [109],
& yyTComb [0],
& yyTComb [0],
& yyTComb [0],
& yyTComb [96],
& yyTComb [0],
& yyTComb [0],
& yyTComb [110],
& yyTComb [0],
& yyTComb [0],
& yyTComb [0],
& yyTComb [118],
& yyTComb [0],
& yyTComb [0],
& yyTComb [133],
& yyTComb [134],
& yyTComb [0],
& yyTComb [0],
& yyTComb [0],
& yyTComb [136],
& yyTComb [0],
& yyTComb [0],
& yyTComb [137],
& yyTComb [0],
& yyTComb [140],
& yyTComb [103],
& yyTComb [0],
& yyTComb [143],
& yyTComb [144],
& yyTComb [107],
& yyTComb [0],
& yyTComb [149],
& yyTComb [0],
& yyTComb [147],
& yyTComb [0],
& yyTComb [148],
& yyTComb [0],
& yyTComb [152],
& yyTComb [153],
& yyTComb [112],
& yyTComb [0],
& yyTComb [155],
& yyTComb [156],
& yyTComb [116],
& yyTComb [0],
& yyTComb [0],
& yyTComb [159],
& yyTComb [0],
& yyTComb [0],
& yyTComb [160],
& yyTComb [161],
& yyTComb [0],
& yyTComb [0],
& yyTComb [0],
& yyTComb [162],
& yyTComb [163],
& yyTComb [0],
& yyTComb [0],
& yyTComb [0],
& yyTComb [164],
& yyTComb [0],
& yyTComb [0],
& yyTComb [120],
& yyTComb [0],
& yyTComb [168],
& yyTComb [121],
& yyTComb [0],
& yyTComb [0],
& yyTComb [121],
& yyTComb [0],
& yyTComb [172],
& yyTComb [170],
& yyTComb [0],
& yyTComb [171],
& yyTComb [172],
& yyTComb [173],
& yyTComb [0],
& yyTComb [0],
& yyTComb [125],
& yyTComb [0],
& yyTComb [177],
& yyTComb [126],
& yyTComb [0],
& yyTComb [188],
& yyTComb [186],
& yyTComb [0],
& yyTComb [192],
& yyTComb [0],
& yyTComb [190],
& yyTComb [191],
& yyTComb [192],
& yyTComb [0],
& yyTComb [193],
& yyTComb [0],
& yyTComb [197],
& yyTComb [195],
& yyTComb [0],
& yyTComb [0],
& yyTComb [196],
& yyTComb [0],
& yyTComb [199],
& yyTComb [0],
& yyTComb [198],
& yyTComb [0],
& yyTComb [201],
& yyTComb [0],
& yyTComb [200],
& yyTComb [201],
& yyTComb [0],
& yyTComb [205],
& yyTComb [0],
& yyTComb [0],
& yyTComb [203],
& yyTComb [0],
& yyTComb [207],
& yyTComb [0],
& yyTComb [205],
& yyTComb [0],
& yyTComb [0],
& yyTComb [0],
& yyTComb [206],
& yyTComb [0],
& yyTComb [0],
& yyTComb [0],
& yyTComb [0],
& yyTComb [207],
& yyTComb [0],
& yyTComb [0],
& yyTComb [0],
& yyTComb [0],
& yyTComb [208],
& yyTComb [0],
& yyTComb [0],
& yyTComb [0],
& yyTComb [0],
& yyTComb [0],
& yyTComb [209],
& yyTComb [0],
& yyTComb [0],
& yyTComb [0],
& yyTComb [210],
& yyTComb [0],
& yyTComb [0],
& yyTComb [0],
& yyTComb [211],
& yyTComb [212],
& yyTComb [0],
& yyTComb [216],
& yyTComb [0],
& yyTComb [151],
& yyTComb [0],
& yyTComb [217],
& yyTComb [218],
& yyTComb [155],
& yyTComb [0],
& yyTComb [0],
& yyTComb [0],
& yyTComb [221],
& yyTComb [0],
& yyTComb [0],
& yyTComb [0],
& yyTComb [0],
& yyTComb [0],
& yyTComb [222],
& yyTComb [0],
& yyTComb [0],
& yyTComb [0],
& yyTComb [0],
& yyTComb [0],
& yyTComb [0],
& yyTComb [223],
& yyTComb [0],
& yyTComb [0],
& yyTComb [0],
& yyTComb [224],
& yyTComb [0],
& yyTComb [0],
& yyTComb [0],
& yyTComb [0],
& yyTComb [225],
& yyTComb [0],
& yyTComb [0],
& yyTComb [0],
& yyTComb [232],
& yyTComb [0],
& yyTComb [235],
& yyTComb [235],
& yyTComb [0],
& yyTComb [0],
& yyTComb [159],
& yyTComb [0],
& yyTComb [239],
& yyTComb [160],
& yyTComb [0],
& yyTComb [0],
& yyTComb [0],
& yyTComb [239],
& yyTComb [240],
& yyTComb [241],
& yyTComb [0],
& yyTComb [244],
& yyTComb [0],
& yyTComb [243],
& yyTComb [0],
& yyTComb [164],
& yyTComb [0],
& yyTComb [0],
& yyTComb [164],
& yyTComb [0],
& yyTComb [248],
& yyTComb [247],
& yyTComb [0],
& yyTComb [180],
& yyTComb [0],
& yyTComb [0],
& yyTComb [170],
& yyTComb [0],
& yyTComb [254],
& yyTComb [252],
& yyTComb [0],
& yyTComb [0],
& yyTComb [256],
& yyTComb [260],
& yyTComb [0],
& yyTComb [0],
& yyTComb [0],
& yyTComb [261],
& yyTComb [0],
& yyTComb [0],
& yyTComb [0],
& yyTComb [266],
& yyTComb [0],
& yyTComb [0],
& yyTComb [0],
& yyTComb [267],
& yyTComb [0],
& yyTComb [0],
& yyTComb [178],
& yyTComb [0],
& yyTComb [272],
& yyTComb [180],
& yyTComb [0],
& yyTComb [0],
& yyTComb [274],
& yyTComb [0],
& yyTComb [0],
& yyTComb [275],
& yyTComb [0],
& yyTComb [0],
& yyTComb [289],
& yyTComb [287],
& yyTComb [295],
& yyTComb [296],
& yyTComb [0],
& yyTComb [0],
& yyTComb [0],
& yyTComb [297],
& yyTComb [0],
& yyTComb [0],
& yyTComb [298],
& yyTComb [0],
& yyTComb [299],
& yyTComb [0],
& yyTComb [0],
& yyTComb [0],
& yyTComb [300],
& yyTComb [0],
& yyTComb [304],
& yyTComb [0],
& yyTComb [305],
& yyTComb [0],
& yyTComb [0],
& yyTComb [0],
& yyTComb [306],
& yyTComb [0],
& yyTComb [0],
& yyTComb [0],
& yyTComb [307],
& yyTComb [0],
& yyTComb [0],
& yyTComb [308],
& yyTComb [0],
& yyTComb [0],
& yyTComb [0],
& yyTComb [309],
& yyTComb [0],
& yyTComb [0],
& yyTComb [312],
& yyTComb [0],
& yyTComb [0],
& yyTComb [0],
& yyTComb [313],
& yyTComb [0],
& yyTComb [0],
& yyTComb [0],
& yyTComb [318],
& yyTComb [0],
& yyTComb [0],
& yyTComb [0],
& yyTComb [319],
& yyTComb [0],
& yyTComb [0],
& yyTComb [320],
& yyTComb [0],
& yyTComb [0],
& yyTComb [215],
& yyTComb [0],
& yyTComb [324],
& yyTComb [216],
& yyTComb [0],
& yyTComb [0],
& yyTComb [0],
& yyTComb [324],
& yyTComb [325],
& yyTComb [326],
& yyTComb [0],
& yyTComb [0],
& yyTComb [0],
& yyTComb [327],
& yyTComb [0],
& yyTComb [0],
& yyTComb [0],
& yyTComb [328],
& yyTComb [0],
& yyTComb [0],
& yyTComb [0],
& yyTComb [329],
& yyTComb [0],
& yyTComb [0],
& yyTComb [0],
& yyTComb [0],
& yyTComb [330],
& yyTComb [0],
& yyTComb [0],
& yyTComb [331],
& yyTComb [0],
& yyTComb [334],
& yyTComb [0],
& yyTComb [333],
& yyTComb [0],
& yyTComb [0],
& yyTComb [0],
& yyTComb [219],
& yyTComb [0],
& yyTComb [337],
& yyTComb [220],
& yyTComb [0],
& yyTComb [0],
& yyTComb [220],
& yyTComb [0],
& yyTComb [340],
& yyTComb [223],
& yyTComb [0],
& yyTComb [0],
& yyTComb [339],
& yyTComb [0],
& yyTComb [0],
& yyTComb [343],
& yyTComb [344],
& yyTComb [0],
& yyTComb [345],
& yyTComb [0],
& yyTComb [349],
& yyTComb [349],
& yyTComb [0],
& yyTComb [348],
& yyTComb [349],
& yyTComb [350],
& yyTComb [0],
& yyTComb [0],
& yyTComb [353],
& yyTComb [0],
& yyTComb [352],
& yyTComb [0],
& yyTComb [252],
& yyTComb [359],
& yyTComb [359],
& yyTComb [0],
& yyTComb [358],
& yyTComb [0],
& yyTComb [362],
& yyTComb [0],
& yyTComb [236],
& yyTComb [0],
& yyTComb [363],
& yyTComb [364],
& yyTComb [240],
& yyTComb [0],
& yyTComb [0],
& yyTComb [370],
& yyTComb [0],
& yyTComb [0],
& yyTComb [371],
& yyTComb [0],
& yyTComb [0],
& yyTComb [372],
& yyTComb [0],
& yyTComb [0],
& yyTComb [373],
& yyTComb [0],
& yyTComb [376],
& yyTComb [375],
& yyTComb [376],
& yyTComb [0],
& yyTComb [380],
& yyTComb [0],
& yyTComb [378],
& yyTComb [0],
& yyTComb [0],
& yyTComb [247],
& yyTComb [0],
& yyTComb [382],
& yyTComb [248],
& yyTComb [0],
& yyTComb [384],
& yyTComb [385],
& yyTComb [384],
& yyTComb [385],
& yyTComb [386],
& yyTComb [0],
& yyTComb [0],
& yyTComb [387],
& yyTComb [0],
& yyTComb [390],
& yyTComb [389],
& yyTComb [0],
& yyTComb [0],
& yyTComb [0],
& yyTComb [390],
& yyTComb [0],
& yyTComb [0],
& yyTComb [391],
& yyTComb [392],
& yyTComb [393],
& yyTComb [0],
& yyTComb [397],
& yyTComb [398],
& yyTComb [396],
& yyTComb [0],
& yyTComb [400],
& yyTComb [401],
& yyTComb [401],
& yyTComb [400],
& yyTComb [0],
& yyTComb [404],
& yyTComb [0],
& yyTComb [0],
& yyTComb [0],
& yyTComb [402],
& yyTComb [0],
& yyTComb [406],
& yyTComb [0],
& yyTComb [0],
& yyTComb [404],
& yyTComb [0],
& yyTComb [408],
& yyTComb [408],
& yyTComb [409],
& yyTComb [408],
& yyTComb [0],
& yyTComb [412],
& yyTComb [412],
& yyTComb [411],
& yyTComb [0],
& yyTComb [0],
& yyTComb [267],
& yyTComb [0],
& yyTComb [415],
& yyTComb [268],
& yyTComb [0],
& yyTComb [0],
& yyTComb [0],
& yyTComb [415],
& yyTComb [416],
& yyTComb [417],
& yyTComb [0],
& yyTComb [420],
& yyTComb [421],
& yyTComb [420],
& yyTComb [0],
& yyTComb [423],
& yyTComb [424],
& yyTComb [423],
& yyTComb [424],
& yyTComb [428],
& yyTComb [428],
& yyTComb [277],
& yyTComb [0],
& yyTComb [363],
& yyTComb [432],
& yyTComb [0],
& yyTComb [0],
& yyTComb [0],
& yyTComb [0],
& yyTComb [430],
& yyTComb [435],
& yyTComb [0],
& yyTComb [432],
& yyTComb [0],
& yyTComb [433],
& yyTComb [0],
& yyTComb [437],
& yyTComb [441],
& yyTComb [0],
& yyTComb [370],
& yyTComb [0],
& yyTComb [446],
& yyTComb [447],
& yyTComb [0],
& yyTComb [0],
& yyTComb [443],
& yyTComb [447],
& yyTComb [0],
& yyTComb [0],
& yyTComb [445],
& yyTComb [450],
& yyTComb [382],
& yyTComb [456],
& yyTComb [457],
& yyTComb [0],
& yyTComb [0],
& yyTComb [453],
& yyTComb [454],
& yyTComb [457],
& yyTComb [305],
& yyTComb [0],
& yyTComb [457],
& yyTComb [460],
& yyTComb [307],
& yyTComb [462],
& yyTComb [463],
& yyTComb [366],
& yyTComb [0],
& yyTComb [465],
& yyTComb [471],
& yyTComb [0],
& yyTComb [467],
& yyTComb [470],
& yyTComb [314],
& yyTComb [472],
& yyTComb [473],
& yyTComb [318],
& yyTComb [479],
& yyTComb [479],
& yyTComb [0],
& yyTComb [478],
& yyTComb [481],
& yyTComb [0],
& yyTComb [480],
& yyTComb [0],
& yyTComb [481],
& yyTComb [0],
& yyTComb [482],
& yyTComb [486],
& yyTComb [486],
& yyTComb [490],
& yyTComb [494],
& yyTComb [488],
& yyTComb [497],
& yyTComb [498],
& yyTComb [501],
& yyTComb [342],
& yyTComb [504],
& yyTComb [505],
& yyTComb [503],
& yyTComb [506],
& yyTComb [346],
& yyTComb [508],
& yyTComb [509],
& yyTComb [350],
& yyTComb [515],
& yyTComb [515],
& yyTComb [514],
& yyTComb [518],
& yyTComb [516],
& yyTComb [519],
& yyTComb [368],
& yyTComb [523],
& yyTComb [522],
& yyTComb [525],
& yyTComb [526],
& yyTComb [529],
& yyTComb [366],
& yyTComb [532],
& yyTComb [533],
& yyTComb [371],
& yyTComb [540],
& yyTComb [539],
& yyTComb [0],
& yyTComb [539],
& yyTComb [543],
& yyTComb [545],
& yyTComb [552],
& yyTComb [544],
& yyTComb [558],
};
static	unsigned short*	yyNBasePtr	[yyLastReadState + 1] = { 0,
& yyNComb [-171],
& yyNComb [-171],
& yyNComb [-171],
& yyNComb [-171],
& yyNComb [-171],
& yyNComb [-171],
& yyNComb [-171],
& yyNComb [-171],
& yyNComb [-171],
& yyNComb [-171],
& yyNComb [-171],
& yyNComb [-171],
& yyNComb [-171],
& yyNComb [-171],
& yyNComb [-171],
& yyNComb [-171],
& yyNComb [-171],
& yyNComb [-171],
& yyNComb [-171],
& yyNComb [-170],
& yyNComb [-171],
& yyNComb [-171],
& yyNComb [-169],
& yyNComb [-171],
& yyNComb [-171],
& yyNComb [-171],
& yyNComb [-171],
& yyNComb [-171],
& yyNComb [-168],
& yyNComb [-171],
& yyNComb [-171],
& yyNComb [-171],
& yyNComb [-171],
& yyNComb [-171],
& yyNComb [-171],
& yyNComb [-171],
& yyNComb [-171],
& yyNComb [-171],
& yyNComb [-171],
& yyNComb [-171],
& yyNComb [-171],
& yyNComb [-171],
& yyNComb [-167],
& yyNComb [-171],
& yyNComb [-171],
& yyNComb [-166],
& yyNComb [-171],
& yyNComb [-166],
& yyNComb [-171],
& yyNComb [-171],
& yyNComb [-171],
& yyNComb [-171],
& yyNComb [-171],
& yyNComb [-171],
& yyNComb [-171],
& yyNComb [-171],
& yyNComb [-171],
& yyNComb [-171],
& yyNComb [-171],
& yyNComb [-170],
& yyNComb [-171],
& yyNComb [-171],
& yyNComb [-171],
& yyNComb [-169],
& yyNComb [-171],
& yyNComb [-171],
& yyNComb [-168],
& yyNComb [-171],
& yyNComb [-171],
& yyNComb [-165],
& yyNComb [-171],
& yyNComb [-171],
& yyNComb [-164],
& yyNComb [-171],
& yyNComb [-163],
& yyNComb [-171],
& yyNComb [-171],
& yyNComb [-171],
& yyNComb [-162],
& yyNComb [-171],
& yyNComb [-171],
& yyNComb [-171],
& yyNComb [-161],
& yyNComb [-171],
& yyNComb [-165],
& yyNComb [-171],
& yyNComb [-159],
& yyNComb [-170],
& yyNComb [-171],
& yyNComb [-158],
& yyNComb [-171],
& yyNComb [-171],
& yyNComb [-171],
& yyNComb [-155],
& yyNComb [-171],
& yyNComb [-159],
& yyNComb [-171],
& yyNComb [-153],
& yyNComb [-171],
& yyNComb [-171],
& yyNComb [-152],
& yyNComb [-171],
& yyNComb [-156],
& yyNComb [-155],
& yyNComb [-171],
& yyNComb [-149],
& yyNComb [-153],
& yyNComb [-171],
& yyNComb [-171],
& yyNComb [-147],
& yyNComb [-151],
& yyNComb [-150],
& yyNComb [-171],
& yyNComb [-144],
& yyNComb [-148],
& yyNComb [-150],
& yyNComb [-171],
& yyNComb [-141],
& yyNComb [-145],
& yyNComb [-171],
& yyNComb [-139],
& yyNComb [-143],
& yyNComb [-142],
& yyNComb [-171],
& yyNComb [-136],
& yyNComb [-139],
& yyNComb [-171],
& yyNComb [-171],
& yyNComb [-134],
& yyNComb [-138],
& yyNComb [-136],
& yyNComb [-171],
& yyNComb [-131],
& yyNComb [-135],
& yyNComb [-171],
& yyNComb [-129],
& yyNComb [-145],
& yyNComb [-171],
& yyNComb [-126],
& yyNComb [-142],
& yyNComb [-143],
& yyNComb [-171],
& yyNComb [-123],
& yyNComb [-171],
& yyNComb [-127],
& yyNComb [-171],
& yyNComb [-125],
& yyNComb [-171],
& yyNComb [-119],
& yyNComb [-171],
& yyNComb [-138],
& yyNComb [-171],
& yyNComb [-117],
& yyNComb [-148],
& yyNComb [-148],
& yyNComb [-171],
& yyNComb [-114],
& yyNComb [-118],
& yyNComb [-171],
& yyNComb [-112],
& yyNComb [-115],
& yyNComb [-171],
& yyNComb [-171],
& yyNComb [-110],
& yyNComb [-114],
& yyNComb [-113],
& yyNComb [-171],
& yyNComb [-171],
& yyNComb [-107],
& yyNComb [-114],
& yyNComb [-109],
& yyNComb [-171],
& yyNComb [-104],
& yyNComb [-118],
& yyNComb [-171],
& yyNComb [-102],
& yyNComb [-156],
& yyNComb [-171],
& yyNComb [-99],
& yyNComb [-104],
& yyNComb [-171],
& yyNComb [-97],
& yyNComb [-137],
& yyNComb [-136],
& yyNComb [-97],
& yyNComb [-171],
& yyNComb [-148],
& yyNComb [-171],
& yyNComb [-91],
& yyNComb [-106],
& yyNComb [-171],
& yyNComb [-89],
& yyNComb [-145],
& yyNComb [-171],
& yyNComb [-86],
& yyNComb [-171],
& yyNComb [-171],
& yyNComb [-85],
& yyNComb [-171],
& yyNComb [-89],
& yyNComb [-171],
& yyNComb [-141],
& yyNComb [-171],
& yyNComb [-82],
& yyNComb [-171],
& yyNComb [-81],
& yyNComb [-171],
& yyNComb [-171],
& yyNComb [-80],
& yyNComb [-83],
& yyNComb [-171],
& yyNComb [-78],
& yyNComb [-92],
& yyNComb [-80],
& yyNComb [-171],
& yyNComb [-75],
& yyNComb [-90],
& yyNComb [-77],
& yyNComb [-171],
& yyNComb [-171],
& yyNComb [-72],
& yyNComb [-171],
& yyNComb [-76],
& yyNComb [-74],
& yyNComb [-171],
& yyNComb [-69],
& yyNComb [-171],
& yyNComb [-73],
& yyNComb [-171],
& yyNComb [-67],
& yyNComb [-72],
& yyNComb [-69],
& yyNComb [-171],
& yyNComb [-64],
& yyNComb [-69],
& yyNComb [-67],
& yyNComb [-65],
& yyNComb [-171],
& yyNComb [-60],
& yyNComb [-65],
& yyNComb [-62],
& yyNComb [-64],
& yyNComb [-171],
& yyNComb [-56],
& yyNComb [-61],
& yyNComb [-62],
& yyNComb [-57],
& yyNComb [-59],
& yyNComb [-171],
& yyNComb [-51],
& yyNComb [-56],
& yyNComb [-56],
& yyNComb [-171],
& yyNComb [-48],
& yyNComb [-53],
& yyNComb [-54],
& yyNComb [-52],
& yyNComb [-171],
& yyNComb [-44],
& yyNComb [-171],
& yyNComb [-52],
& yyNComb [-171],
& yyNComb [-42],
& yyNComb [-90],
& yyNComb [-61],
& yyNComb [-171],
& yyNComb [-39],
& yyNComb [-44],
& yyNComb [-41],
& yyNComb [-171],
& yyNComb [-36],
& yyNComb [-171],
& yyNComb [-171],
& yyNComb [-41],
& yyNComb [-38],
& yyNComb [-171],
& yyNComb [-33],
& yyNComb [-170],
& yyNComb [-170],
& yyNComb [-38],
& yyNComb [-39],
& yyNComb [-34],
& yyNComb [-171],
& yyNComb [-29],
& yyNComb [-167],
& yyNComb [-34],
& yyNComb [-171],
& yyNComb [-27],
& yyNComb [-21],
& yyNComb [-31],
& yyNComb [-32],
& yyNComb [-171],
& yyNComb [-23],
& yyNComb [-28],
& yyNComb [-25],
& yyNComb [-171],
& yyNComb [-20],
& yyNComb [-25],
& yyNComb [-171],
& yyNComb [-18],
& yyNComb [-39],
& yyNComb [-171],
& yyNComb [-16],
& yyNComb [-50],
& yyNComb [-171],
& yyNComb [-14],
& yyNComb [-18],
& yyNComb [-21],
& yyNComb [-171],
& yyNComb [-46],
& yyNComb [-171],
& yyNComb [-10],
& yyNComb [-31],
& yyNComb [-17],
& yyNComb [-171],
& yyNComb [-17],
& yyNComb [-171],
& yyNComb [-6],
& yyNComb [-18],
& yyNComb [-171],
& yyNComb [-4],
& yyNComb [-43],
& yyNComb [-163],
& yyNComb [-25],
& yyNComb [-171],
& yyNComb [-1],
& yyNComb [-24],
& yyNComb [-171],
& yyNComb [2],
& yyNComb [-171],
& yyNComb [-171],
& yyNComb [3],
& yyNComb [-20],
& yyNComb [-171],
& yyNComb [-171],
& yyNComb [5],
& yyNComb [-18],
& yyNComb [2],
& yyNComb [-171],
& yyNComb [8],
& yyNComb [-15],
& yyNComb [2],
& yyNComb [-171],
& yyNComb [11],
& yyNComb [-12],
& yyNComb [-11],
& yyNComb [-171],
& yyNComb [14],
& yyNComb [-10],
& yyNComb [-171],
& yyNComb [16],
& yyNComb [-28],
& yyNComb [-171],
& yyNComb [19],
& yyNComb [10],
& yyNComb [-171],
& yyNComb [21],
& yyNComb [-2],
& yyNComb [-171],
& yyNComb [23],
& yyNComb [0],
& yyNComb [-171],
& yyNComb [-171],
& yyNComb [-21],
& yyNComb [-171],
& yyNComb [26],
& yyNComb [3],
& yyNComb [24],
& yyNComb [-171],
& yyNComb [29],
& yyNComb [6],
& yyNComb [-171],
& yyNComb [7],
& yyNComb [-171],
& yyNComb [32],
& yyNComb [9],
& yyNComb [29],
& yyNComb [-171],
& yyNComb [35],
& yyNComb [-171],
& yyNComb [36],
& yyNComb [-171],
& yyNComb [37],
& yyNComb [13],
& yyNComb [16],
& yyNComb [-171],
& yyNComb [40],
& yyNComb [16],
& yyNComb [19],
& yyNComb [-171],
& yyNComb [43],
& yyNComb [20],
& yyNComb [-171],
& yyNComb [45],
& yyNComb [22],
& yyNComb [42],
& yyNComb [-171],
& yyNComb [48],
& yyNComb [39],
& yyNComb [-171],
& yyNComb [50],
& yyNComb [28],
& yyNComb [29],
& yyNComb [-171],
& yyNComb [53],
& yyNComb [31],
& yyNComb [32],
& yyNComb [-171],
& yyNComb [56],
& yyNComb [34],
& yyNComb [35],
& yyNComb [-171],
& yyNComb [59],
& yyNComb [37],
& yyNComb [-171],
& yyNComb [61],
& yyNComb [36],
& yyNComb [-171],
& yyNComb [63],
& yyNComb [30],
& yyNComb [-171],
& yyNComb [65],
& yyNComb [61],
& yyNComb [44],
& yyNComb [-171],
& yyNComb [34],
& yyNComb [-171],
& yyNComb [69],
& yyNComb [44],
& yyNComb [48],
& yyNComb [-171],
& yyNComb [72],
& yyNComb [50],
& yyNComb [69],
& yyNComb [-171],
& yyNComb [75],
& yyNComb [53],
& yyNComb [51],
& yyNComb [-171],
& yyNComb [78],
& yyNComb [56],
& yyNComb [55],
& yyNComb [59],
& yyNComb [-171],
& yyNComb [83],
& yyNComb [58],
& yyNComb [-171],
& yyNComb [85],
& yyNComb [60],
& yyNComb [64],
& yyNComb [-171],
& yyNComb [88],
& yyNComb [84],
& yyNComb [63],
& yyNComb [-171],
& yyNComb [91],
& yyNComb [53],
& yyNComb [-171],
& yyNComb [93],
& yyNComb [56],
& yyNComb [-171],
& yyNComb [95],
& yyNComb [49],
& yyNComb [-171],
& yyNComb [98],
& yyNComb [91],
& yyNComb [-171],
& yyNComb [100],
& yyNComb [96],
& yyNComb [-171],
& yyNComb [55],
& yyNComb [80],
& yyNComb [-171],
& yyNComb [104],
& yyNComb [-171],
& yyNComb [67],
& yyNComb [84],
& yyNComb [-171],
& yyNComb [69],
& yyNComb [-171],
& yyNComb [109],
& yyNComb [105],
& yyNComb [84],
& yyNComb [89],
& yyNComb [-171],
& yyNComb [113],
& yyNComb [86],
& yyNComb [-171],
& yyNComb [107],
& yyNComb [93],
& yyNComb [-171],
& yyNComb [117],
& yyNComb [-171],
& yyNComb [107],
& yyNComb [-171],
& yyNComb [119],
& yyNComb [90],
& yyNComb [90],
& yyNComb [-171],
& yyNComb [122],
& yyNComb [118],
& yyNComb [-171],
& yyNComb [124],
& yyNComb [121],
& yyNComb [-171],
& yyNComb [126],
& yyNComb [117],
& yyNComb [-171],
& yyNComb [128],
& yyNComb [124],
& yyNComb [-171],
& yyNComb [130],
& yyNComb [107],
& yyNComb [-171],
& yyNComb [-171],
& yyNComb [132],
& yyNComb [-171],
& yyNComb [110],
& yyNComb [-171],
& yyNComb [134],
& yyNComb [106],
& yyNComb [-171],
& yyNComb [136],
& yyNComb [100],
& yyNComb [-171],
& yyNComb [138],
& yyNComb [114],
& yyNComb [117],
& yyNComb [-171],
& yyNComb [104],
& yyNComb [-171],
& yyNComb [142],
& yyNComb [114],
& yyNComb [-171],
& yyNComb [144],
& yyNComb [116],
& yyNComb [-171],
& yyNComb [146],
& yyNComb [142],
& yyNComb [134],
& yyNComb [-171],
& yyNComb [149],
& yyNComb [145],
& yyNComb [-171],
& yyNComb [-171],
& yyNComb [-171],
& yyNComb [151],
& yyNComb [-171],
& yyNComb [-171],
& yyNComb [-171],
& yyNComb [152],
& yyNComb [-171],
& yyNComb [-171],
& yyNComb [142],
& yyNComb [-171],
& yyNComb [155],
& yyNComb [-171],
& yyNComb [147],
& yyNComb [152],
& yyNComb [154],
& yyNComb [-171],
& yyNComb [159],
& yyNComb [-171],
& yyNComb [151],
& yyNComb [156],
& yyNComb [-171],
& yyNComb [162],
& yyNComb [-171],
& yyNComb [154],
& yyNComb [160],
& yyNComb [-171],
& yyNComb [165],
& yyNComb [-171],
& yyNComb [156],
& yyNComb [-171],
& yyNComb [167],
& yyNComb [156],
& yyNComb [-171],
& yyNComb [169],
& yyNComb [134],
& yyNComb [-171],
& yyNComb [171],
& yyNComb [167],
& yyNComb [160],
& yyNComb [-171],
& yyNComb [138],
& yyNComb [-171],
& yyNComb [175],
& yyNComb [164],
& yyNComb [164],
& yyNComb [-171],
& yyNComb [178],
& yyNComb [174],
& yyNComb [166],
& yyNComb [-171],
& yyNComb [-171],
& yyNComb [-171],
& yyNComb [-171],
& yyNComb [-171],
& yyNComb [199],
& yyNComb [-171],
& yyNComb [-171],
& yyNComb [187],
& yyNComb [186],
& yyNComb [-161],
& yyNComb [-163],
& yyNComb [185],
& yyNComb [-171],
& yyNComb [183],
& yyNComb [185],
& yyNComb [188],
& yyNComb [185],
& yyNComb [-159],
& yyNComb [-171],
& yyNComb [-171],
& yyNComb [195],
& yyNComb [194],
& yyNComb [-158],
& yyNComb [-171],
& yyNComb [-171],
& yyNComb [-157],
& yyNComb [209],
& yyNComb [185],
& yyNComb [-171],
& yyNComb [198],
& yyNComb [211],
& yyNComb [185],
& yyNComb [-171],
& yyNComb [201],
& yyNComb [-171],
& yyNComb [-171],
& yyNComb [217],
& yyNComb [215],
& yyNComb [185],
& yyNComb [203],
& yyNComb [-171],
& yyNComb [-171],
& yyNComb [217],
& yyNComb [185],
& yyNComb [-171],
& yyNComb [-171],
& yyNComb [217],
& yyNComb [-171],
& yyNComb [204],
& yyNComb [220],
& yyNComb [185],
& yyNComb [-171],
& yyNComb [221],
& yyNComb [185],
& yyNComb [-171],
& yyNComb [-171],
& yyNComb [220],
& yyNComb [215],
& yyNComb [-171],
& yyNComb [-171],
& yyNComb [-171],
& yyNComb [218],
& yyNComb [228],
& yyNComb [-171],
& yyNComb [219],
& yyNComb [-171],
& yyNComb [220],
& yyNComb [-171],
& yyNComb [228],
& yyNComb [185],
& yyNComb [-171],
& yyNComb [-171],
& yyNComb [223],
& yyNComb [228],
& yyNComb [-171],
& yyNComb [223],
& yyNComb [-171],
& yyNComb [-171],
& yyNComb [-171],
& yyNComb [-171],
& yyNComb [-171],
& yyNComb [-171],
& yyNComb [-171],
& yyNComb [-171],
& yyNComb [222],
& yyNComb [222],
& yyNComb [-171],
& yyNComb [-171],
& yyNComb [236],
& yyNComb [-171],
& yyNComb [-171],
& yyNComb [-171],
& yyNComb [-171],
& yyNComb [-171],
& yyNComb [235],
& yyNComb [203],
& yyNComb [235],
& yyNComb [185],
& yyNComb [233],
& yyNComb [-171],
& yyNComb [221],
& yyNComb [-171],
& yyNComb [-171],
& yyNComb [-171],
& yyNComb [-171],
& yyNComb [221],
& yyNComb [223],
& yyNComb [-171],
& yyNComb [223],
& yyNComb [185],
& yyNComb [-171],
& yyNComb [-171],
};
static	unsigned short	yyDefault	[yyLastReadState + 1] = { 0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
684,
0,
0,
0,
0,
0,
0,
0,
0,
684,
0,
0,
684,
0,
0,
0,
0,
0,
684,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
684,
0,
0,
684,
0,
0,
0,
702,
0,
0,
0,
0,
0,
0,
0,
570,
0,
702,
0,
593,
0,
702,
0,
0,
702,
0,
0,
702,
0,
0,
702,
0,
702,
0,
0,
0,
702,
0,
594,
0,
702,
0,
593,
0,
702,
594,
0,
702,
0,
0,
0,
702,
0,
593,
0,
702,
0,
0,
702,
0,
593,
593,
0,
702,
593,
0,
0,
702,
593,
593,
0,
702,
593,
489,
0,
702,
593,
0,
702,
593,
593,
0,
702,
570,
0,
0,
702,
593,
570,
0,
702,
593,
0,
702,
0,
0,
702,
0,
0,
0,
702,
0,
593,
0,
593,
0,
702,
0,
0,
0,
702,
0,
0,
0,
702,
593,
0,
702,
570,
0,
0,
702,
593,
593,
0,
0,
702,
489,
570,
0,
702,
213,
0,
702,
0,
0,
702,
298,
0,
702,
0,
183,
570,
0,
177,
0,
702,
217,
0,
702,
0,
0,
702,
0,
0,
702,
0,
593,
0,
193,
0,
702,
0,
702,
0,
0,
702,
570,
0,
702,
0,
570,
0,
702,
0,
570,
0,
0,
702,
0,
593,
570,
0,
702,
0,
593,
0,
702,
298,
570,
0,
702,
298,
593,
570,
0,
702,
298,
570,
257,
0,
702,
298,
489,
570,
257,
0,
702,
298,
257,
0,
702,
298,
489,
0,
0,
702,
0,
569,
0,
702,
0,
0,
0,
702,
298,
570,
0,
702,
629,
617,
298,
570,
0,
702,
629,
617,
298,
489,
570,
0,
702,
629,
298,
0,
702,
629,
298,
489,
0,
702,
298,
570,
0,
702,
0,
0,
702,
313,
0,
702,
0,
0,
702,
593,
569,
0,
304,
0,
702,
0,
569,
0,
574,
0,
702,
590,
0,
702,
0,
48,
528,
0,
702,
513,
0,
702,
0,
0,
702,
513,
0,
0,
702,
513,
593,
0,
702,
513,
489,
0,
702,
513,
513,
0,
702,
527,
0,
702,
0,
0,
702,
574,
0,
702,
513,
0,
702,
513,
0,
0,
352,
0,
702,
513,
570,
0,
702,
513,
0,
513,
0,
702,
513,
593,
0,
702,
0,
702,
0,
702,
527,
528,
0,
702,
527,
528,
0,
702,
513,
0,
702,
513,
593,
0,
702,
574,
0,
702,
528,
528,
0,
702,
528,
528,
0,
702,
528,
528,
0,
702,
528,
0,
702,
449,
0,
702,
0,
0,
702,
593,
528,
0,
420,
0,
702,
449,
528,
0,
702,
528,
593,
0,
702,
528,
449,
0,
702,
528,
449,
528,
0,
702,
449,
0,
702,
0,
528,
0,
702,
593,
483,
0,
702,
0,
0,
702,
476,
0,
702,
0,
0,
702,
489,
0,
702,
593,
0,
463,
528,
0,
702,
0,
0,
528,
0,
457,
0,
702,
593,
0,
528,
0,
702,
0,
0,
0,
528,
0,
702,
0,
554,
0,
702,
0,
0,
0,
702,
593,
0,
702,
570,
0,
702,
574,
0,
702,
593,
0,
702,
0,
0,
0,
702,
0,
528,
0,
702,
536,
0,
702,
0,
0,
702,
0,
0,
0,
524,
0,
702,
536,
0,
702,
0,
0,
702,
593,
594,
0,
702,
593,
0,
0,
0,
702,
0,
0,
0,
702,
0,
0,
0,
0,
702,
0,
569,
593,
570,
0,
702,
0,
569,
593,
0,
702,
0,
0,
0,
0,
702,
0,
0,
0,
702,
589,
0,
702,
0,
0,
702,
593,
590,
0,
580,
0,
702,
0,
0,
0,
702,
0,
0,
0,
0,
0,
0,
0,
690,
0,
0,
629,
617,
684,
692,
693,
51,
704,
705,
704,
705,
684,
0,
0,
629,
0,
684,
0,
0,
684,
692,
693,
0,
629,
692,
693,
0,
0,
0,
0,
684,
692,
693,
690,
0,
0,
692,
693,
0,
0,
0,
0,
0,
692,
693,
0,
692,
693,
0,
0,
0,
0,
0,
0,
0,
668,
669,
0,
671,
672,
671,
672,
692,
693,
0,
0,
0,
684,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
690,
0,
0,
0,
0,
0,
0,
0,
0,
0,
702,
0,
0,
0,
0,
0,
0,
};
static	unsigned char	yyLength	[yyLastReduceState - yyFirstReduceState + 1] = {
2,
1,
0,
2,
4,
5,
6,
7,
5,
4,
21,
4,
4,
5,
6,
7,
5,
6,
3,
0,
5,
0,
5,
11,
24,
0,
2,
5,
4,
1,
1,
1,
6,
8,
11,
9,
1,
1,
0,
2,
4,
0,
2,
0,
4,
0,
10,
1,
2,
0,
6,
5,
0,
2,
0,
2,
1,
4,
4,
0,
2,
1,
1,
1,
1,
3,
3,
7,
5,
6,
7,
6,
6,
7,
7,
8,
6,
7,
7,
6,
7,
8,
7,
7,
6,
5,
6,
6,
0,
1,
5,
6,
5,
5,
6,
6,
5,
5,
6,
5,
6,
4,
5,
0,
2,
6,
5,
5,
5,
4,
6,
6,
5,
6,
6,
5,
7,
6,
6,
6,
6,
5,
6,
6,
6,
1,
1,
1,
1,
1,
6,
8,
9,
6,
7,
6,
5,
5,
6,
6,
4,
4,
6,
6,
6,
6,
5,
6,
5,
6,
6,
6,
5,
5,
6,
6,
6,
7,
5,
6,
6,
7,
8,
6,
6,
5,
5,
5,
6,
5,
5,
0,
2,
5,
5,
5,
5,
5,
5,
0,
2,
5,
5,
1,
1,
1,
6,
6,
6,
6,
6,
7,
5,
6,
6,
6,
6,
5,
6,
5,
5,
0,
2,
5,
1,
2,
5,
0,
2,
5,
1,
2,
5,
5,
5,
1,
2,
5,
0,
2,
5,
1,
2,
5,
1,
2,
5,
1,
2,
5,
1,
2,
5,
1,
2,
5,
1,
2,
5,
1,
2,
5,
6,
6,
1,
1,
5,
5,
6,
};
static	yySymbolRange	yyLeftHandSide	[yyLastReduceState - yyFirstReduceState + 1] = {
253,
172,
171,
171,
173,
173,
173,
173,
173,
173,
173,
174,
174,
174,
174,
174,
174,
174,
174,
181,
181,
182,
182,
185,
185,
186,
186,
187,
187,
188,
188,
188,
190,
190,
190,
190,
192,
192,
184,
184,
184,
179,
179,
180,
180,
193,
193,
176,
176,
178,
178,
178,
183,
183,
177,
177,
196,
196,
196,
195,
195,
197,
197,
197,
197,
197,
194,
194,
194,
175,
175,
175,
175,
175,
175,
175,
175,
175,
175,
175,
175,
175,
175,
175,
175,
175,
175,
175,
201,
201,
202,
202,
202,
198,
198,
198,
198,
198,
198,
198,
198,
198,
212,
211,
211,
213,
199,
199,
199,
199,
199,
199,
199,
199,
199,
199,
199,
199,
199,
199,
199,
199,
199,
199,
199,
189,
189,
189,
191,
191,
215,
215,
215,
215,
215,
215,
215,
215,
215,
204,
217,
217,
217,
217,
217,
217,
217,
217,
217,
217,
217,
217,
217,
217,
217,
217,
217,
217,
217,
217,
217,
217,
217,
217,
217,
217,
217,
217,
217,
217,
205,
224,
224,
225,
225,
225,
225,
225,
214,
226,
226,
227,
227,
222,
222,
222,
228,
229,
230,
231,
233,
233,
218,
218,
218,
218,
218,
218,
218,
218,
207,
234,
234,
200,
235,
235,
208,
237,
237,
219,
239,
239,
241,
241,
232,
242,
242,
203,
243,
243,
220,
244,
244,
216,
245,
245,
206,
246,
246,
221,
247,
247,
223,
248,
248,
209,
249,
249,
210,
251,
251,
252,
252,
250,
236,
238,
240,
240,
240,
};
static	yySymbolRange	yyContinuation	[yyLastReadState + 1] = { 0,
0,
0,
6,
4,
7,
2,
4,
7,
4,
2,
5,
16,
4,
7,
4,
7,
1,
2,
7,
5,
2,
2,
7,
7,
2,
5,
7,
4,
5,
7,
1,
2,
5,
7,
4,
2,
7,
7,
7,
4,
2,
2,
5,
7,
4,
5,
7,
5,
145,
5,
21,
7,
2,
7,
2,
7,
4,
5,
16,
5,
1,
5,
26,
5,
2,
7,
5,
4,
7,
5,
4,
7,
5,
7,
5,
2,
2,
7,
5,
4,
5,
29,
5,
5,
7,
7,
5,
5,
7,
5,
4,
4,
7,
5,
4,
5,
7,
5,
4,
7,
5,
4,
5,
5,
7,
5,
5,
4,
7,
5,
5,
5,
7,
5,
5,
5,
38,
5,
5,
7,
5,
5,
5,
7,
5,
5,
7,
7,
5,
5,
5,
7,
5,
5,
7,
5,
5,
44,
5,
5,
7,
45,
5,
4,
5,
7,
5,
7,
5,
4,
5,
47,
5,
5,
7,
48,
5,
5,
7,
5,
5,
7,
7,
5,
5,
5,
7,
7,
5,
5,
5,
7,
5,
5,
52,
5,
5,
53,
5,
5,
54,
5,
4,
7,
5,
7,
7,
7,
5,
5,
56,
5,
5,
57,
5,
4,
7,
5,
4,
5,
7,
7,
7,
5,
7,
5,
4,
7,
5,
5,
7,
5,
5,
5,
7,
5,
5,
5,
7,
7,
5,
4,
5,
5,
7,
5,
4,
5,
7,
5,
5,
5,
7,
5,
5,
5,
5,
7,
5,
5,
5,
7,
7,
5,
5,
5,
5,
7,
7,
5,
5,
7,
7,
5,
5,
5,
7,
7,
5,
4,
5,
70,
5,
5,
7,
82,
5,
5,
5,
7,
5,
73,
76,
5,
5,
7,
5,
73,
76,
5,
5,
5,
7,
5,
73,
5,
7,
5,
73,
5,
5,
7,
5,
5,
5,
7,
5,
5,
7,
5,
5,
84,
5,
5,
85,
5,
5,
5,
7,
7,
7,
5,
5,
5,
7,
5,
87,
5,
5,
88,
5,
5,
7,
5,
89,
5,
5,
91,
5,
4,
7,
5,
5,
4,
7,
5,
5,
5,
7,
5,
5,
5,
7,
5,
5,
5,
7,
5,
5,
97,
5,
5,
99,
5,
5,
7,
5,
5,
7,
5,
5,
4,
7,
7,
7,
5,
5,
5,
7,
5,
5,
7,
5,
7,
5,
5,
5,
7,
5,
7,
5,
7,
5,
5,
5,
7,
5,
5,
5,
7,
5,
5,
7,
5,
5,
5,
7,
5,
5,
7,
5,
5,
5,
7,
5,
5,
5,
7,
5,
5,
5,
7,
5,
5,
7,
5,
5,
113,
5,
5,
114,
5,
5,
5,
7,
7,
7,
5,
5,
5,
7,
5,
5,
5,
7,
5,
5,
5,
7,
5,
5,
5,
5,
7,
5,
5,
7,
5,
5,
5,
7,
5,
5,
5,
122,
5,
5,
123,
5,
5,
124,
5,
5,
126,
5,
5,
7,
5,
5,
7,
7,
5,
7,
5,
4,
5,
5,
7,
7,
7,
5,
5,
5,
5,
7,
5,
108,
4,
5,
5,
7,
5,
4,
5,
131,
5,
5,
7,
132,
5,
5,
7,
5,
5,
7,
5,
5,
7,
5,
5,
7,
5,
5,
7,
7,
5,
4,
5,
7,
5,
5,
139,
5,
5,
140,
5,
5,
5,
7,
7,
7,
5,
5,
7,
5,
5,
7,
5,
5,
5,
7,
5,
5,
7,
7,
7,
5,
4,
4,
7,
5,
4,
4,
5,
7,
5,
4,
5,
5,
5,
7,
5,
4,
5,
5,
7,
5,
4,
5,
5,
7,
5,
4,
5,
7,
5,
5,
152,
5,
5,
153,
5,
5,
5,
7,
7,
7,
5,
5,
5,
7,
5,
5,
5,
7,
7,
4,
5,
157,
5,
79,
4,
73,
76,
5,
5,
7,
21,
1,
7,
1,
7,
5,
7,
4,
73,
76,
5,
2,
2,
5,
5,
7,
4,
73,
5,
7,
4,
73,
2,
2,
5,
5,
7,
7,
5,
158,
5,
7,
5,
159,
5,
7,
105,
5,
7,
2,
5,
7,
5,
162,
5,
7,
163,
4,
5,
2,
7,
5,
2,
7,
2,
7,
5,
7,
4,
5,
2,
7,
5,
2,
7,
5,
165,
4,
4,
7,
5,
166,
5,
7,
168,
4,
5,
7,
4,
7,
5,
157,
5,
7,
5,
7,
7,
169,
5,
7,
170,
2,
4,
5,
5,
2,
1,
7,
7,
0,
};
static	unsigned short	yyFinalToProd	[yyLastReadReduceState - yyFirstReadReduceState + 1] = {
940,
941,
947,
948,
949,
950,
951,
956,
952,
958,
953,
954,
942,
943,
944,
1003,
1004,
1002,
1042,
1043,
1044,
1045,
1046,
1142,
1047,
1048,
1049,
1050,
1051,
1052,
1053,
1054,
1026,
1027,
1028,
1055,
1056,
1057,
1041,
1038,
1058,
1117,
1118,
1114,
1059,
1060,
1030,
1031,
1180,
1139,
1179,
1171,
1033,
1177,
1178,
1174,
1035,
1037,
1029,
1032,
1034,
1036,
1005,
1006,
1007,
1008,
1009,
1025,
1010,
1011,
1012,
1013,
1066,
1061,
1062,
1063,
1064,
1065,
1067,
1068,
1069,
1070,
1071,
1072,
1123,
1159,
1073,
1074,
1153,
1136,
1128,
1129,
1130,
1131,
1132,
1182,
1183,
1184,
1145,
1133,
1134,
1135,
1078,
1081,
1076,
1077,
1079,
1080,
1082,
1083,
1084,
1085,
1086,
1087,
1088,
1122,
1156,
1089,
1090,
1091,
1092,
1093,
1094,
1095,
1148,
1149,
1150,
1126,
1127,
1165,
1096,
1097,
1119,
1120,
1121,
1098,
1109,
1110,
1111,
1112,
1113,
1106,
1099,
1100,
1125,
1168,
1101,
1102,
1103,
1104,
1105,
1075,
1014,
1015,
1016,
1017,
1018,
1019,
1020,
1124,
1162,
1021,
1022,
1023,
945,
999,
1000,
997,
998,
1001,
994,
993,
969,
970,
968,
971,
987,
972,
973,
986,
965,
966,
975,
976,
959,
963,
964,
960,
982,
980,
946,
1181,
1144,
1040,
1116,
1141,
1140,
1173,
1172,
1176,
1175,
1160,
1161,
1155,
1138,
1147,
1146,
1157,
1158,
1151,
1152,
1166,
1167,
1108,
1169,
1170,
1163,
1164,
996,
992,
991,
967,
989,
984,
983,
962,
978,
939,
};

static	void	yyErrorRecovery		ARGS((yySymbolRange * yyTerminal, yyStateRange * yyStateStack, unsigned long yyStackSize, short yyStackPtr));
static	void	yyComputeContinuation	ARGS((yyStateRange * yyStack, unsigned long yyStackSize, short yyStackPtr, tSet * yyContinueSet));
static	bool	yyIsContinuation	ARGS((yySymbolRange yyTerminal, yyStateRange * yyStateStack, unsigned long yyStackSize, short yyStackPtr));
static	void	yyComputeRestartPoints	ARGS((yyStateRange * yyStateStack, unsigned long yyStackSize, short yyStackPtr, tSet * yyRestartSet));
static	yyStateRange yyNext		ARGS((yyStateRange yyState, yySymbolRange yySymbol));
static	void	BeginBreeze		();

int Breeze ()
   {
      register	yyStateRange	yyState		;
      register	long		yyTerminal	;
      register	yyStateRange *	yyStateStackPtr ;
      register	tParsAttribute *yyAttrStackPtr	;
      register	bool		yyIsRepairing	;
		unsigned long	yyStateStackSize= yyInitStackSize;
		unsigned long	yyAttrStackSize = yyInitStackSize;
		yyStateRange *	yyStateStack	;
		tParsAttribute* yyAttributeStack;
		tParsAttribute	yySynAttribute	;	/* synthesized attribute */
      register	yyStateRange *	yyEndOfStack	;
		int		yyErrorCount	= 0;
   
/* line 192 "Breeze.lalr" */



      BeginBreeze ();
      yyState		= yyStartState;
      yyTerminal	= BreezeScan_GetToken ();
      MakeArray ((char * *) & yyStateStack, & yyStateStackSize, sizeof (yyStateRange));
      MakeArray ((char * *) & yyAttributeStack, & yyAttrStackSize, sizeof (tParsAttribute));
      yyEndOfStack	= & yyStateStack [yyStateStackSize];
      yyStateStackPtr	= yyStateStack;
      yyAttrStackPtr	= yyAttributeStack;
      yyIsRepairing	= false;

   ParseLoop:
      for (;;) {
	 if (yyStateStackPtr >= yyEndOfStack) {
	    int yyyStateStackPtr= yyStateStackPtr - yyStateStack;
	    int yyyAttrStackPtr	= yyAttrStackPtr - yyAttributeStack;
	    ExtendArray ((char * *) & yyStateStack, & yyStateStackSize, sizeof (yyStateRange));
	    ExtendArray ((char * *) & yyAttributeStack, & yyAttrStackSize, sizeof (tParsAttribute));
	    yyStateStackPtr	= yyStateStack + yyyStateStackPtr;
	    yyAttrStackPtr	= yyAttributeStack + yyyAttrStackPtr;
	    yyEndOfStack	= & yyStateStack [yyStateStackSize];
	 }
	 * yyStateStackPtr = yyState;

   TermTrans:
	 for (;;) {	/* SPEC State = Next (State, Terminal); terminal transition */
	    register yyStateRange * yyTCombPtr;

	    yyTCombPtr = (yyStateRange *) (yyTBasePtr [yyState] + yyTerminal);
	    if (* yyTCombPtr ++ == yyState) { yyState = * yyTCombPtr; break; }
	    if ((yyState = yyDefault [yyState]) != yyNoState) goto TermTrans;

							/* syntax error */
	    if (! yyIsRepairing) {			/* report and recover */
	       yySymbolRange yyyTerminal = yyTerminal;

	       yyErrorCount ++;
	       yyErrorRecovery (& yyyTerminal, yyStateStack, yyStateStackSize, yyStateStackPtr - yyStateStack);
	       yyTerminal = yyyTerminal;
	       yyIsRepairing = true;
	    }
	    yyState = * yyStateStackPtr;
	    for (;;) {
	       if (yyNext (yyState, (yySymbolRange) yyTerminal) == yyNoState) { /* repair */
		  yySymbolRange		yyRepairToken;
		  BreezeScan_tScanAttribute	yyRepairAttribute;
	    
		  yyRepairToken = yyContinuation [yyState];
		  yyState = yyNext (yyState, yyRepairToken);
		  if (yyState <= yyLastReadReduceState) {	/* read or read reduce ? */
		     BreezeScan_ErrorAttribute ((int) yyRepairToken, & yyRepairAttribute);
		     ErrorMessageI (xxTokenInserted, xxRepair, BreezeScan_Attribute.Position,
			xxString, Breeze_TokenName [yyRepairToken]);
		     if (yyState >= yyFirstFinalState) {	/* avoid second push */
			yyState = yyFinalToProd [yyState - yyFirstReadReduceState];
		     }
		     yyGetAttribute (yyAttrStackPtr ++, yyRepairAttribute);
		     * ++ yyStateStackPtr = yyState;
		  }
		  if (yyState >= yyFirstFinalState) goto Final; /* final state ? */
	       } else {
		  yyState = yyNext (yyState, (yySymbolRange) yyTerminal);
		  goto Final;
	       }
	    }
	 }

   Final:
	 if (yyState >= yyFirstFinalState) {		/* final state ? */
	    if (yyState <= yyLastReadReduceState) {	/* read reduce ? */
	       yyStateStackPtr ++;
	       yyGetAttribute (yyAttrStackPtr ++, BreezeScan_Attribute);
	       yyTerminal = BreezeScan_GetToken ();
	       yyIsRepairing = false;
	    }

	    for (;;) {
	       /* register long yyNonterminal;		   left-hand side */
# define yyNonterminal yyState

switch (yyState) {
case 936: /* _0000_ : breeze_file _EndOfFile .*/
  ReleaseArray ((char * *) & yyStateStack, & yyStateStackSize, sizeof (yyStateRange));
  ReleaseArray ((char * *) & yyAttributeStack, & yyAttrStackSize, sizeof (tParsAttribute));
  return yyErrorCount;

case 937: /* breeze_file : breeze_decls .*/
  yyStateStackPtr -= 1; yyAttrStackPtr -= 1; yyNonterminal = 172; {
/* line 383 "Breeze.lalr" */

		yySynAttribute.breeze_file.Tree = Breeze_TreeRoot = ReverseTree (yyAttrStackPtr [1-1].breeze_decls.Tree);
	;

;

} break;
case 938: /* breeze_decls : .*/
  yyStateStackPtr -= 0; yyAttrStackPtr -= 0; yyNonterminal = 171; {
/* line 389 "Breeze.lalr" */
 yySynAttribute.breeze_decls.Tree = mNullDecls (NoPosition);
 ;

} break;
case 939:
case 935: /* breeze_decls : breeze_decls breeze_decl .*/
  yyStateStackPtr -= 2; yyAttrStackPtr -= 2; yyNonterminal = 171; {
/* line 392 "Breeze.lalr" */
 yyAttrStackPtr [2-1].breeze_decl.Tree->Decl.next = yyAttrStackPtr [1-1].breeze_decls.Tree; yySynAttribute.breeze_decls.Tree = yyAttrStackPtr [2-1].breeze_decl.Tree; ;
 ;

} break;
case 940:
case 708: /* breeze_decl : '(' 'import' breeze_ident ')' .*/
  yyStateStackPtr -= 4; yyAttrStackPtr -= 4; yyNonterminal = 173; {
/* line 395 "Breeze.lalr" */
 yySynAttribute.breeze_decl.Tree = mImportDecl (NoPosition, NoTree,
	mIdent (NoPosition, mNullIdents (NoPosition), yyAttrStackPtr [3-1].Scan.breeze_ident.ident));

;

} break;
case 941:
case 709: /* breeze_decl : '(' 'file' breeze_literal breeze_ident ')' .*/
  yyStateStackPtr -= 5; yyAttrStackPtr -= 5; yyNonterminal = 173; {
/* line 400 "Breeze.lalr" */
 yySynAttribute.breeze_decl.Tree = mFileDecl (NoPosition, NoTree,
	mIdent (NoPosition, mNullIdents (NoPosition), yyAttrStackPtr [4-1].Scan.breeze_ident.ident));

;

} break;
case 942:
case 720: /* breeze_decl : '(' 'constant' breeze_ident breeze_literal breeze_type ')' .*/
  yyStateStackPtr -= 6; yyAttrStackPtr -= 6; yyNonterminal = 173; {
/* line 405 "Breeze.lalr" */
 yySynAttribute.breeze_decl.Tree = mConstantDecl (NoPosition, NoTree,
	yyAttrStackPtr [3-1].Scan.breeze_ident.ident, mCoercedExpr (mAsExpr (NoPosition, mLiteralExpr (NoPosition, yyAttrStackPtr [4-1].Scan.breeze_literal.value),
	yyAttrStackPtr [5-1].breeze_type.Tree)), yyAttrStackPtr [5-1].breeze_type.Tree);

;

} break;
case 943:
case 721: /* breeze_decl : '(' 'implicant' breeze_ident breeze_literal breeze_literal breeze_type ')' .*/
  yyStateStackPtr -= 7; yyAttrStackPtr -= 7; yyNonterminal = 173; {
/* line 411 "Breeze.lalr" */
 yySynAttribute.breeze_decl.Tree = mConstantDecl (NoPosition, NoTree,
	yyAttrStackPtr [3-1].Scan.breeze_ident.ident, mCoercedExpr (mAsExpr (NoPosition, mImplicantExpr (NoPosition,
	NewImplicant (yyAttrStackPtr [4-1].Scan.breeze_literal.value, yyAttrStackPtr [5-1].Scan.breeze_literal.value)), yyAttrStackPtr [6-1].breeze_type.Tree)), yyAttrStackPtr [6-1].breeze_type.Tree);

;

} break;
case 944:
case 722: /* breeze_decl : '(' 'type' breeze_ident breeze_type ')' .*/
  yyStateStackPtr -= 5; yyAttrStackPtr -= 5; yyNonterminal = 173; {
/* line 417 "Breeze.lalr" */
 yySynAttribute.breeze_decl.Tree = mTypeDecl (NoPosition, NoTree, yyAttrStackPtr [3-1].Scan.breeze_ident.ident, yyAttrStackPtr [4-1].breeze_type.Tree);
 ;

} break;
case 945:
case 872: /* breeze_decl : '(' 'balsa' decl ')' .*/
  yyStateStackPtr -= 4; yyAttrStackPtr -= 4; yyNonterminal = 173; {
/* line 420 "Breeze.lalr" */
 yySynAttribute.breeze_decl.Tree  =  yyAttrStackPtr [3-1].decl.Tree;
 ;

} break;
case 946:
case 898: /* breeze_decl : '(' 'breeze-part' breeze_ident '(' 'ports' part_ports ')' '(' 'attributes' breeze_options ')' '(' 'channels' part_channels ')' '(' 'components' part_components ')' part_optional_callcontexts ')' .*/
  yyStateStackPtr -= 21; yyAttrStackPtr -= 21; yyNonterminal = 173; {
/* line 423 "Breeze.lalr" */

		unsigned channelCount;
		GList *callcontexts = yyAttrStackPtr [20-1].part_optional_callcontexts.callcontexts;
		CallContext_MultipleTranslateWires (yyAttrStackPtr [14-1].part_channels.channels, callcontexts);
		PtrWireArray channels = ConvertWireListToArray (yyAttrStackPtr [14-1].part_channels.channels, &channelCount);
		yySynAttribute.breeze_decl.Tree = mPartDecl (yyAttrStackPtr [10-1].breeze_options.Position, NoTree, yyAttrStackPtr [3-1].Scan.breeze_ident.ident,
			ReverseLispList (yyAttrStackPtr [10-1].breeze_options.list), ReverseTree (yyAttrStackPtr [6-1].part_ports.Tree),
			channels, channelCount, yyAttrStackPtr [18-1].part_components.Tree, callcontexts);
	;

;

} break;
case 947:
case 710: /* breeze_type : '(' 'named-type' breeze_ident ')' .*/
  yyStateStackPtr -= 4; yyAttrStackPtr -= 4; yyNonterminal = 174; {
/* line 435 "Breeze.lalr" */
 yySynAttribute.breeze_type.Tree = mExistingType (NoPosition, yyAttrStackPtr [3-1].Scan.breeze_ident.ident);
 ;

} break;
case 948:
case 711: /* breeze_type : '(' 'alias-type' breeze_ident ')' .*/
  yyStateStackPtr -= 4; yyAttrStackPtr -= 4; yyNonterminal = 174; {
/* line 438 "Breeze.lalr" */
 yySynAttribute.breeze_type.Tree = mExistingType (NoPosition, yyAttrStackPtr [3-1].Scan.breeze_ident.ident);
 ;

} break;
case 949:
case 712: /* breeze_type : '(' 'numeric-type' boolean breeze_literal ')' .*/
  yyStateStackPtr -= 5; yyAttrStackPtr -= 5; yyNonterminal = 174; {
/* line 441 "Breeze.lalr" */
 yySynAttribute.breeze_type.Tree = mNumericType (NoPosition, yyAttrStackPtr [3-1].Scan.boolean.value,
	mLiteralExpr (NoPosition, yyAttrStackPtr [4-1].Scan.breeze_literal.value));
 ;

} break;
case 950:
case 713: /* breeze_type : '(' 'array-type' breeze_type breeze_literal breeze_literal ')' .*/
  yyStateStackPtr -= 6; yyAttrStackPtr -= 6; yyNonterminal = 174; {
/* line 445 "Breeze.lalr" */

		PtrMP_INT upperIndex = CopyMP_INT (yyAttrStackPtr [4-1].Scan.breeze_literal.value);
		mpz_add (upperIndex, upperIndex, yyAttrStackPtr [5-1].Scan.breeze_literal.value);
		mpz_sub_ui (upperIndex, upperIndex, 1);
		yySynAttribute.breeze_type.Tree = mArrayType (NoPosition, yyAttrStackPtr [3-1].breeze_type.Tree, mSpecifiedRange (NoPosition,
			mLiteralExpr (NoPosition, yyAttrStackPtr [4-1].Scan.breeze_literal.value),
			mLiteralExpr (NoPosition, upperIndex)));
	;

;

} break;
case 951:
case 714: /* breeze_type : '(' 'array-type' breeze_type breeze_literal breeze_literal breeze_type ')' .*/
  yyStateStackPtr -= 7; yyAttrStackPtr -= 7; yyNonterminal = 174; {
/* line 456 "Breeze.lalr" */

		PtrMP_INT upperIndex = CopyMP_INT (yyAttrStackPtr [4-1].Scan.breeze_literal.value);
		mpz_add (upperIndex, upperIndex, yyAttrStackPtr [5-1].Scan.breeze_literal.value);
		mpz_sub_ui (upperIndex, upperIndex, 1);
		yySynAttribute.breeze_type.Tree = mArrayType (NoPosition, yyAttrStackPtr [3-1].breeze_type.Tree, mSpecifiedRange (NoPosition,
			mAsExpr (NoPosition, mLiteralExpr (NoPosition, yyAttrStackPtr [4-1].Scan.breeze_literal.value), yyAttrStackPtr [6-1].breeze_type.Tree),
			mAsExpr (NoPosition, mLiteralExpr (NoPosition, upperIndex), yyAttrStackPtr [6-1].breeze_type.Tree)));
	;

;

} break;
case 952:
case 716: /* breeze_type : '(' 'record-type' breeze_literal breeze_record_elems ')' .*/
  yyStateStackPtr -= 5; yyAttrStackPtr -= 5; yyNonterminal = 174; {
/* line 467 "Breeze.lalr" */
 yySynAttribute.breeze_type.Tree = mRecordType (NoPosition, ReverseTree (yyAttrStackPtr [4-1].breeze_record_elems.Tree),
	mNumericType (NoPosition, false, mLiteralExpr (NoPosition, yyAttrStackPtr [3-1].Scan.breeze_literal.value)));
 ;

} break;
case 953:
case 718: /* breeze_type : '(' 'enumeration-type' boolean breeze_literal breeze_enum_elems ')' .*/
  yyStateStackPtr -= 6; yyAttrStackPtr -= 6; yyNonterminal = 174; {
/* line 471 "Breeze.lalr" */
 yySynAttribute.breeze_type.Tree = mEnumType (NoPosition, ReverseTree (yyAttrStackPtr [5-1].breeze_enum_elems.Tree),
	mNumericType (NoPosition, yyAttrStackPtr [3-1].Scan.boolean.value, mLiteralExpr (NoPosition, yyAttrStackPtr [4-1].Scan.breeze_literal.value)));
 ;

} break;
case 954:
case 719: /* breeze_type : '(' 'builtin-type' ')' .*/
  yyStateStackPtr -= 3; yyAttrStackPtr -= 3; yyNonterminal = 174; {
/* line 475 "Breeze.lalr" */
 yySynAttribute.breeze_type.Tree = mBuiltinType (NoPosition);
 ;

} break;
case 955: /* breeze_record_elems : .*/
  yyStateStackPtr -= 0; yyAttrStackPtr -= 0; yyNonterminal = 181; {
/* line 478 "Breeze.lalr" */
 yySynAttribute.breeze_record_elems.Tree = mNullRecordElems (NoPosition);
 ;

} break;
case 956:
case 715: /* breeze_record_elems : breeze_record_elems '(' breeze_ident breeze_type ')' .*/
  yyStateStackPtr -= 5; yyAttrStackPtr -= 5; yyNonterminal = 181; {
/* line 481 "Breeze.lalr" */

	yySynAttribute.breeze_record_elems.Tree = mRecordElem (NoPosition, yyAttrStackPtr [1-1].breeze_record_elems.Tree, mIdent (NoPosition, mNullIdents (NoPosition),
		yyAttrStackPtr [3-1].Scan.breeze_ident.ident), yyAttrStackPtr [4-1].breeze_type.Tree);

;

} break;
case 957: /* breeze_enum_elems : .*/
  yyStateStackPtr -= 0; yyAttrStackPtr -= 0; yyNonterminal = 182; {
/* line 487 "Breeze.lalr" */
 yySynAttribute.breeze_enum_elems.Tree = mNullEnumElems (NoPosition);
 ;

} break;
case 958:
case 717: /* breeze_enum_elems : breeze_enum_elems '(' breeze_ident breeze_literal ')' .*/
  yyStateStackPtr -= 5; yyAttrStackPtr -= 5; yyNonterminal = 182; {
/* line 490 "Breeze.lalr" */
 yySynAttribute.breeze_enum_elems.Tree = mValuedEnumElem (NoPosition, yyAttrStackPtr [1-1].breeze_enum_elems.Tree,
	yyAttrStackPtr [3-1].Scan.breeze_ident.ident, mCoercedExpr (mLiteralExpr (NoPosition, yyAttrStackPtr [4-1].Scan.breeze_literal.value)));

;

} break;
case 959:
case 892: /* part_component : '(' 'component' breeze_ident '(' comp_parameters ')' '(' channel_numbers ')' breeze_options ')' .*/
  yyStateStackPtr -= 11; yyAttrStackPtr -= 11; yyNonterminal = 185; {
/* line 495 "Breeze.lalr" */

		Ptrconstchar name = PeekString (yyAttrStackPtr [3-1].Scan.breeze_ident.ident);

		yySynAttribute.part_component.Tree = mNormalComp (yyAttrStackPtr [10-1].breeze_options.Position, NoTree, yyAttrStackPtr [3-1].Scan.breeze_ident.ident, (*name == '$') ,
			ReverseTree (yyAttrStackPtr [5-1].comp_parameters.Tree), ReverseintList (yyAttrStackPtr [8-1].channel_numbers.channelNos),
			ReverseLispList (yyAttrStackPtr [10-1].breeze_options.list));
	;

;

} break;
case 960:
case 895: /* part_component : '(' 'undeclared-component' breeze_ident '(' comp_parameters ')' '(' channel_numbers ')' '(' 'implements' breeze_ident breeze_ident ')' '(' 'parameters' breeze_param_decls ')' '(' 'ports' part_ports ')' breeze_options ')' .*/
  yyStateStackPtr -= 24; yyAttrStackPtr -= 24; yyNonterminal = 185; {
/* line 505 "Breeze.lalr" */

		yySynAttribute.part_component.Tree = mUndeclaredComp (yyAttrStackPtr [23-1].breeze_options.Position, NoTree, yyAttrStackPtr [3-1].Scan.breeze_ident.ident,
			ReverseTree (yyAttrStackPtr [5-1].comp_parameters.Tree), ReverseintList (yyAttrStackPtr [8-1].channel_numbers.channelNos),
			yyAttrStackPtr [12-1].Scan.breeze_ident.ident, yyAttrStackPtr [13-1].Scan.breeze_ident.ident,
			ReverseTree (yyAttrStackPtr [17-1].breeze_param_decls.Tree),
			ReverseTree (yyAttrStackPtr [21-1].part_ports.Tree),
			ReverseLispList (yyAttrStackPtr [23-1].breeze_options.list)
		);
	;

;

} break;
case 961: /* breeze_param_decls : .*/
  yyStateStackPtr -= 0; yyAttrStackPtr -= 0; yyNonterminal = 186; {
/* line 517 "Breeze.lalr" */
 yySynAttribute.breeze_param_decls.Tree = mNullBreezeParameters (NoPosition);
 ;

} break;
case 962:
case 933: /* breeze_param_decls : breeze_param_decls breeze_param_decl .*/
  yyStateStackPtr -= 2; yyAttrStackPtr -= 2; yyNonterminal = 186; {
/* line 520 "Breeze.lalr" */
 yyAttrStackPtr [2-1].breeze_param_decl.Tree->BreezeParameter.next = yyAttrStackPtr [1-1].breeze_param_decls.Tree;
	yySynAttribute.breeze_param_decls.Tree = yyAttrStackPtr [2-1].breeze_param_decl.Tree; ;
 ;

} break;
case 963:
case 893: /* breeze_param_decl : '(' 'parameter' breeze_ident breeze_type ')' .*/
  yyStateStackPtr -= 5; yyAttrStackPtr -= 5; yyNonterminal = 187; {
/* line 524 "Breeze.lalr" */
 yySynAttribute.breeze_param_decl.Tree = mBreezeExprParameter (NoPosition, NoTree, yyAttrStackPtr [3-1].Scan.breeze_ident.ident, yyAttrStackPtr [4-1].breeze_type.Tree);
 ;

} break;
case 964:
case 894: /* breeze_param_decl : '(' 'type-parameter' breeze_ident ')' .*/
  yyStateStackPtr -= 4; yyAttrStackPtr -= 4; yyNonterminal = 187; {
/* line 527 "Breeze.lalr" */
 yySynAttribute.breeze_param_decl.Tree = mBreezeTypeParameter (NoPosition, NoTree, yyAttrStackPtr [3-1].Scan.breeze_ident.ident);
 ;

} break;
case 965:
case 888: /* comp_parameter : breeze_literal .*/
  yyStateStackPtr -= 1; yyAttrStackPtr -= 1; yyNonterminal = 188; {
/* line 530 "Breeze.lalr" */
 yySynAttribute.comp_parameter.Tree = mNumberParameter (yyAttrStackPtr [1-1].Scan.Position, NoTree, yyAttrStackPtr [1-1].Scan.breeze_literal.value);
 ;

} break;
case 966:
case 889: /* comp_parameter : breeze_ident .*/
  yyStateStackPtr -= 1; yyAttrStackPtr -= 1; yyNonterminal = 188; {
/* line 533 "Breeze.lalr" */
 yySynAttribute.comp_parameter.Tree = mStringParameter (yyAttrStackPtr [1-1].Scan.Position, NoTree, yyAttrStackPtr [1-1].Scan.breeze_ident.ident);
 ;

} break;
case 967:
case 929: /* comp_parameter : breeze_type .*/
  yyStateStackPtr -= 1; yyAttrStackPtr -= 1; yyNonterminal = 188; {
/* line 536 "Breeze.lalr" */
 yySynAttribute.comp_parameter.Tree = mTypeParameter (yyAttrStackPtr [1-1].breeze_type.Tree->AType.position, NoTree, yyAttrStackPtr [1-1].breeze_type.Tree);
 ;

} break;
case 968:
case 882: /* part_port : '(' 'sync-port' breeze_ident port_sense breeze_options ')' .*/
  yyStateStackPtr -= 6; yyAttrStackPtr -= 6; yyNonterminal = 190; {
/* line 539 "Breeze.lalr" */
 yySynAttribute.part_port.Tree = mSyncPort (yyAttrStackPtr [5-1].breeze_options.Position, NoTree, mIdent (NoPosition, mNullIdents (NoPosition),
	yyAttrStackPtr [3-1].Scan.breeze_ident.ident), yyAttrStackPtr [4-1].port_sense.portSense, ReverseLispList (yyAttrStackPtr [5-1].breeze_options.list));
 ;

} break;
case 969:
case 880: /* part_port : '(' 'port' breeze_ident port_sense port_direction breeze_type breeze_options ')' .*/
  yyStateStackPtr -= 8; yyAttrStackPtr -= 8; yyNonterminal = 190; {
/* line 543 "Breeze.lalr" */
 yySynAttribute.part_port.Tree = mChannelPort (yyAttrStackPtr [7-1].breeze_options.Position, NoTree, mIdent (NoPosition, mNullIdents (NoPosition),
	yyAttrStackPtr [3-1].Scan.breeze_ident.ident), yyAttrStackPtr [6-1].breeze_type.Tree, yyAttrStackPtr [4-1].port_sense.portSense, yyAttrStackPtr [5-1].port_direction.isOutput,
	ReverseLispList (yyAttrStackPtr [7-1].breeze_options.list));
 ;

} break;
case 970:
case 881: /* part_port : '(' 'arrayed-port' breeze_ident port_sense port_direction breeze_type breeze_literal breeze_literal breeze_type breeze_options ')' .*/
  yyStateStackPtr -= 11; yyAttrStackPtr -= 11; yyNonterminal = 190; {
/* line 548 "Breeze.lalr" */

		PtrMP_INT upperIndex = CopyMP_INT (yyAttrStackPtr [7-1].Scan.breeze_literal.value);
		mpz_add (upperIndex, upperIndex, yyAttrStackPtr [8-1].Scan.breeze_literal.value);
		mpz_sub_ui (upperIndex, upperIndex, 1);

		yySynAttribute.part_port.Tree = mChannelPortArray (yyAttrStackPtr [10-1].breeze_options.Position, NoTree, mIdent (NoPosition, mNullIdents (NoPosition), yyAttrStackPtr [3-1].Scan.breeze_ident.ident),
			yyAttrStackPtr [6-1].breeze_type.Tree, yyAttrStackPtr [4-1].port_sense.portSense, yyAttrStackPtr [5-1].port_direction.isOutput,
			mSpecifiedRange (NoPosition,
				mAsExpr (NoPosition, mLiteralExpr (NoPosition, yyAttrStackPtr [7-1].Scan.breeze_literal.value), yyAttrStackPtr [9-1].breeze_type.Tree),
				mAsExpr (NoPosition, mLiteralExpr (NoPosition, upperIndex), yyAttrStackPtr [9-1].breeze_type.Tree)),
			ReverseLispList (yyAttrStackPtr [10-1].breeze_options.list));
	;

;

} break;
case 971:
case 883: /* part_port : '(' 'arrayed-sync-port' breeze_ident port_sense breeze_literal breeze_literal breeze_type breeze_options ')' .*/
  yyStateStackPtr -= 9; yyAttrStackPtr -= 9; yyNonterminal = 190; {
/* line 563 "Breeze.lalr" */

		PtrMP_INT upperIndex = CopyMP_INT (yyAttrStackPtr [5-1].Scan.breeze_literal.value);
		mpz_add (upperIndex, upperIndex, yyAttrStackPtr [6-1].Scan.breeze_literal.value);
		mpz_sub_ui (upperIndex, upperIndex, 1);

		yySynAttribute.part_port.Tree = mSyncPortArray (yyAttrStackPtr [8-1].breeze_options.Position, NoTree, mIdent (NoPosition, mNullIdents (NoPosition), yyAttrStackPtr [3-1].Scan.breeze_ident.ident),
			yyAttrStackPtr [4-1].port_sense.portSense,
			mSpecifiedRange (NoPosition,
				mAsExpr (NoPosition, mLiteralExpr (NoPosition, yyAttrStackPtr [5-1].Scan.breeze_literal.value), yyAttrStackPtr [7-1].breeze_type.Tree),
				mAsExpr (NoPosition, mLiteralExpr (NoPosition, upperIndex), yyAttrStackPtr [7-1].breeze_type.Tree)),
			ReverseLispList (yyAttrStackPtr [8-1].breeze_options.list));
	;

;

} break;
case 972:
case 885: /* part_channel_sense : 'push' .*/
  yyStateStackPtr -= 1; yyAttrStackPtr -= 1; yyNonterminal = 192; {
/* line 578 "Breeze.lalr" */
 yySynAttribute.part_channel_sense.isPull = false;
 ;

} break;
case 973:
case 886: /* part_channel_sense : 'pull' .*/
  yyStateStackPtr -= 1; yyAttrStackPtr -= 1; yyNonterminal = 192; {
/* line 581 "Breeze.lalr" */
 yySynAttribute.part_channel_sense.isPull = true;
 ;

} break;
case 974: /* channel_numbers : .*/
  yyStateStackPtr -= 0; yyAttrStackPtr -= 0; yyNonterminal = 184; {
/* line 584 "Breeze.lalr" */
 yySynAttribute.channel_numbers.channelNos = NULL;
 ;

} break;
case 975:
case 890: /* channel_numbers : channel_numbers breeze_literal .*/
  yyStateStackPtr -= 2; yyAttrStackPtr -= 2; yyNonterminal = 184; {
/* line 587 "Breeze.lalr" */
 yySynAttribute.channel_numbers.channelNos = NewintList (mpz_get_ui (yyAttrStackPtr [2-1].Scan.breeze_literal.value), yyAttrStackPtr [2-1].Scan.Position, yyAttrStackPtr [1-1].channel_numbers.channelNos);
 ;

} break;
case 976:
case 891: /* channel_numbers : channel_numbers '(' channel_numbers ')' .*/
  yyStateStackPtr -= 4; yyAttrStackPtr -= 4; yyNonterminal = 184; {
/* line 590 "Breeze.lalr" */
 yySynAttribute.channel_numbers.channelNos = AppendintLists (yyAttrStackPtr [3-1].channel_numbers.channelNos, yyAttrStackPtr [1-1].channel_numbers.channelNos);
 ;

} break;
case 977: /* part_components : .*/
  yyStateStackPtr -= 0; yyAttrStackPtr -= 0; yyNonterminal = 179; {
/* line 593 "Breeze.lalr" */
 yySynAttribute.part_components.Tree = mNullComps (NoPosition);
 ;

} break;
case 978:
case 934: /* part_components : part_components part_component .*/
  yyStateStackPtr -= 2; yyAttrStackPtr -= 2; yyNonterminal = 179; {
/* line 596 "Breeze.lalr" */
 yyAttrStackPtr [2-1].part_component.Tree->Comp.next = yyAttrStackPtr [1-1].part_components.Tree; yySynAttribute.part_components.Tree = yyAttrStackPtr [2-1].part_component.Tree; ;
 ;

} break;
case 979: /* part_optional_callcontexts : .*/
  yyStateStackPtr -= 0; yyAttrStackPtr -= 0; yyNonterminal = 180; {
/* line 599 "Breeze.lalr" */
 yySynAttribute.part_optional_callcontexts.callcontexts = NULL;
 ;

} break;
case 980:
case 897: /* part_optional_callcontexts : '(' 'call-contexts' part_callcontexts ')' .*/
  yyStateStackPtr -= 4; yyAttrStackPtr -= 4; yyNonterminal = 180; {
/* line 602 "Breeze.lalr" */
 yySynAttribute.part_optional_callcontexts.callcontexts  =  yyAttrStackPtr [3-1].part_callcontexts.callcontexts;
 ;

} break;
case 981: /* part_callcontexts : .*/
  yyStateStackPtr -= 0; yyAttrStackPtr -= 0; yyNonterminal = 193; {
/* line 605 "Breeze.lalr" */
 yySynAttribute.part_callcontexts.callcontexts = NULL;
 ;

} break;
case 982:
case 896: /* part_callcontexts : part_callcontexts '(' 'call-context' breeze_literal breeze_ident position position breeze_literal lisp_tokens ')' .*/
  yyStateStackPtr -= 10; yyAttrStackPtr -= 10; yyNonterminal = 193; {
/* line 608 "Breeze.lalr" */

		    PtrCallContext callcontext = CallContext_Add (yyAttrStackPtr [5-1].Scan.breeze_ident.ident, yyAttrStackPtr [6-1].position.Position, yyAttrStackPtr [7-1].position.Position, NULL);
		    callcontext->data = (PtrCallContext) mpz_get_ui (yyAttrStackPtr [4-1].Scan.breeze_literal.value);
		    yySynAttribute.part_callcontexts.callcontexts = g_list_prepend (yyAttrStackPtr [1-1].part_callcontexts.callcontexts, callcontext);
	;

;

} break;
case 983:
case 932: /* part_ports : part_port .*/
  yyStateStackPtr -= 1; yyAttrStackPtr -= 1; yyNonterminal = 176; {
/* line 616 "Breeze.lalr" */
 yyAttrStackPtr [1-1].part_port.Tree->FormalPort.next = mNullFormalPorts (NoPosition); yySynAttribute.part_ports.Tree = yyAttrStackPtr [1-1].part_port.Tree; ;
 ;

} break;
case 984:
case 931: /* part_ports : part_ports part_port .*/
  yyStateStackPtr -= 2; yyAttrStackPtr -= 2; yyNonterminal = 176; {
/* line 619 "Breeze.lalr" */
 yyAttrStackPtr [2-1].part_port.Tree->FormalPort.next = yyAttrStackPtr [1-1].part_ports.Tree; yySynAttribute.part_ports.Tree = yyAttrStackPtr [2-1].part_port.Tree; ;
 ;

} break;
case 985: /* part_channels : .*/
  yyStateStackPtr -= 0; yyAttrStackPtr -= 0; yyNonterminal = 178; {
/* line 622 "Breeze.lalr" */
 yySynAttribute.part_channels.channels = NULL;
 ;

} break;
case 986:
case 887: /* part_channels : part_channels '(' part_channel_sense breeze_literal breeze_options ')' .*/
  yyStateStackPtr -= 6; yyAttrStackPtr -= 6; yyNonterminal = 178; {
/* line 625 "Breeze.lalr" */

		Lisp nameNode;
		PtrWire wire = NewWire (0 , mpz_get_ui (yyAttrStackPtr [4-1].Scan.breeze_literal.value),
			NoType, 
			0, yyAttrStackPtr [3-1].part_channel_sense.isPull, yyAttrStackPtr [5-1].breeze_options.Position);
		wire->type.tree = yyAttrStackPtr [5-1].breeze_options.type;

		wire->options = ReverseLispList (yyAttrStackPtr [5-1].breeze_options.list);
		if (RemoveHeadedLispListElement (&wire->options, nameIdent, &nameNode))
			wire->ident = CAR (CDR (nameNode.value.sublist)).value.string;
		yySynAttribute.part_channels.channels = NewWireList (wire, yyAttrStackPtr [1-1].part_channels.channels);
	;

;

} break;
case 987:
case 884: /* part_channels : part_channels '(' 'sync' breeze_options ')' .*/
  yyStateStackPtr -= 5; yyAttrStackPtr -= 5; yyNonterminal = 178; {
/* line 640 "Breeze.lalr" */

		Lisp nameNode;
		PtrWire wire = NewSyncWire (0 , yyAttrStackPtr [4-1].breeze_options.Position);
		wire->type.tree = NoTree;

		wire->options = ReverseLispList (yyAttrStackPtr [4-1].breeze_options.list);
		if (RemoveHeadedLispListElement (&wire->options, nameIdent, &nameNode))
			wire->ident = CAR (CDR (nameNode.value.sublist)).value.string;
		yySynAttribute.part_channels.channels = NewWireList (wire, yyAttrStackPtr [1-1].part_channels.channels);
	;

;

} break;
case 988: /* comp_parameters : .*/
  yyStateStackPtr -= 0; yyAttrStackPtr -= 0; yyNonterminal = 183; {
/* line 653 "Breeze.lalr" */
 yySynAttribute.comp_parameters.Tree = mNullParameters (NoPosition);
 ;

} break;
case 989:
case 930: /* comp_parameters : comp_parameters comp_parameter .*/
  yyStateStackPtr -= 2; yyAttrStackPtr -= 2; yyNonterminal = 183; {
/* line 656 "Breeze.lalr" */
 yyAttrStackPtr [2-1].comp_parameter.Tree->Parameter.next = yyAttrStackPtr [1-1].comp_parameters.Tree; yySynAttribute.comp_parameters.Tree = yyAttrStackPtr [2-1].comp_parameter.Tree; ;
 ;

} break;
case 990: /* breeze_options : .*/
  yyStateStackPtr -= 0; yyAttrStackPtr -= 0; yyNonterminal = 177; {
/* line 659 "Breeze.lalr" */
 yySynAttribute.breeze_options.Position = NoPosition;
 yySynAttribute.breeze_options.type = NULL;
 yySynAttribute.breeze_options.list = NULL;
 ;

} break;
case 991:
case 928: /* breeze_options : breeze_options breeze_option .*/
  yyStateStackPtr -= 2; yyAttrStackPtr -= 2; yyNonterminal = 177; {
/* line 664 "Breeze.lalr" */

	yySynAttribute.breeze_options.Position = (yyAttrStackPtr [2-1].breeze_option.Position.Line != 0 ? yyAttrStackPtr [2-1].breeze_option.Position : yyAttrStackPtr [1-1].breeze_options.Position);

	yySynAttribute.breeze_options.list = (yyAttrStackPtr [2-1].breeze_option.tokens
		? NewLispList (NewLispSublist (yyAttrStackPtr [2-1].breeze_option.tokens), yyAttrStackPtr [1-1].breeze_options.list)
		: yyAttrStackPtr [1-1].breeze_options.list
	);

	yySynAttribute.breeze_options.type = (yyAttrStackPtr [2-1].breeze_option.type == NoTree ? yyAttrStackPtr [2-1].breeze_option.type : yyAttrStackPtr [1-1].breeze_options.type);

;

} break;
case 992:
case 927: /* breeze_option : position .*/
  yyStateStackPtr -= 1; yyAttrStackPtr -= 1; yyNonterminal = 196; {
/* line 676 "Breeze.lalr" */

	yySynAttribute.breeze_option.Position  =  yyAttrStackPtr [1-1].position.Position;

	yySynAttribute.breeze_option.type = NoTree;

	yySynAttribute.breeze_option.tokens = NULL;

;

} break;
case 993:
case 879: /* breeze_option : '(' 'type' breeze_type ')' .*/
  yyStateStackPtr -= 4; yyAttrStackPtr -= 4; yyNonterminal = 196; {
/* line 685 "Breeze.lalr" */

	yySynAttribute.breeze_option.Position = NoPosition;

	yySynAttribute.breeze_option.type  =  yyAttrStackPtr [3-1].breeze_type.Tree;

	yySynAttribute.breeze_option.tokens = NULL;

;

} break;
case 994:
case 878: /* breeze_option : '(' breeze_keyword lisp_tokens ')' .*/
  yyStateStackPtr -= 4; yyAttrStackPtr -= 4; yyNonterminal = 196; {
/* line 694 "Breeze.lalr" */

	yySynAttribute.breeze_option.Position = NoPosition;

	yySynAttribute.breeze_option.type = NoTree;

	yySynAttribute.breeze_option.tokens = NewLispList (NewLispSymbol (yyAttrStackPtr [2-1].Scan.breeze_keyword.keyword), ReverseLispList (yyAttrStackPtr [3-1].lisp_tokens.tokens));

;

} break;
case 995: /* lisp_tokens : .*/
  yyStateStackPtr -= 0; yyAttrStackPtr -= 0; yyNonterminal = 195; {
/* line 703 "Breeze.lalr" */
 yySynAttribute.lisp_tokens.tokens = NULL;
 ;

} break;
case 996:
case 926: /* lisp_tokens : lisp_tokens lisp_token .*/
  yyStateStackPtr -= 2; yyAttrStackPtr -= 2; yyNonterminal = 195; {
/* line 706 "Breeze.lalr" */
 yySynAttribute.lisp_tokens.tokens = NewLispList (yyAttrStackPtr [2-1].lisp_token.token, yyAttrStackPtr [1-1].lisp_tokens.tokens);
 ;

} break;
case 997:
case 875: /* lisp_token : breeze_keyword .*/
  yyStateStackPtr -= 1; yyAttrStackPtr -= 1; yyNonterminal = 197; {
/* line 709 "Breeze.lalr" */
 yySynAttribute.lisp_token.token = NewLispSymbol (yyAttrStackPtr [1-1].Scan.breeze_keyword.keyword);
 ;

} break;
case 998:
case 876: /* lisp_token : breeze_ident .*/
  yyStateStackPtr -= 1; yyAttrStackPtr -= 1; yyNonterminal = 197; {
/* line 712 "Breeze.lalr" */
 yySynAttribute.lisp_token.token = NewLispString (yyAttrStackPtr [1-1].Scan.breeze_ident.ident);
 ;

} break;
case 999:
case 873: /* lisp_token : boolean .*/
  yyStateStackPtr -= 1; yyAttrStackPtr -= 1; yyNonterminal = 197; {
/* line 715 "Breeze.lalr" */
 yySynAttribute.lisp_token.token = NewLispBoolean (yyAttrStackPtr [1-1].Scan.boolean.value);
 ;

} break;
case 1000:
case 874: /* lisp_token : breeze_literal .*/
  yyStateStackPtr -= 1; yyAttrStackPtr -= 1; yyNonterminal = 197; {
/* line 718 "Breeze.lalr" */
 yySynAttribute.lisp_token.token = NewLispNumber (yyAttrStackPtr [1-1].Scan.breeze_literal.value);
 ;

} break;
case 1001:
case 877: /* lisp_token : '(' lisp_tokens ')' .*/
  yyStateStackPtr -= 3; yyAttrStackPtr -= 3; yyNonterminal = 197; {
/* line 721 "Breeze.lalr" */
 yySynAttribute.lisp_token.token = NewLispSublist (ReverseLispList (yyAttrStackPtr [2-1].lisp_tokens.tokens));
 ;

} break;
case 1002:
case 725: /* position : '(' 'at' ')' .*/
  yyStateStackPtr -= 3; yyAttrStackPtr -= 3; yyNonterminal = 194; {
/* line 724 "Breeze.lalr" */
 yySynAttribute.position.Position = NoPosition;
 ;

} break;
case 1003:
case 723: /* position : '(' 'at' breeze_literal breeze_literal breeze_ident breeze_literal ')' .*/
  yyStateStackPtr -= 7; yyAttrStackPtr -= 7; yyNonterminal = 194; {
/* line 727 "Breeze.lalr" */

		yySynAttribute.position.Position.Line = mpz_get_ui (yyAttrStackPtr [3-1].Scan.breeze_literal.value);
		yySynAttribute.position.Position.Column = mpz_get_ui (yyAttrStackPtr [4-1].Scan.breeze_literal.value);
		yySynAttribute.position.Position.File = NewIdentList (yyAttrStackPtr [5-1].Scan.breeze_ident.ident, NoPosition, NULL);
		yySynAttribute.position.Position.CallContext = (CallContext *) mpz_get_ui (yyAttrStackPtr [6-1].Scan.breeze_literal.value);
	;

;

} break;
case 1004:
case 724: /* position : '(' 'at' breeze_literal breeze_literal ')' .*/
  yyStateStackPtr -= 5; yyAttrStackPtr -= 5; yyNonterminal = 194; {
/* line 736 "Breeze.lalr" */

		yySynAttribute.position.Position.Line = mpz_get_ui (yyAttrStackPtr [3-1].Scan.breeze_literal.value);
		yySynAttribute.position.Position.Column = mpz_get_ui (yyAttrStackPtr [4-1].Scan.breeze_literal.value);
		yySynAttribute.position.Position.File = NULL;
		yySynAttribute.position.Position.CallContext = 0;
	;

;

} break;
case 1005:
case 770: /* decl : '(' 'type-decl' position breeze_ident type ')' .*/
  yyStateStackPtr -= 6; yyAttrStackPtr -= 6; yyNonterminal = 175; {
/* line 745 "Breeze.lalr" */
 yySynAttribute.decl.Tree = mTypeDecl (yyAttrStackPtr [3-1].position.Position, NoTree, yyAttrStackPtr [4-1].Scan.breeze_ident.ident, yyAttrStackPtr [5-1].type.Tree);
 ;

} break;
case 1006:
case 771: /* decl : '(' 'typed-constant-decl' position breeze_ident expr type ')' .*/
  yyStateStackPtr -= 7; yyAttrStackPtr -= 7; yyNonterminal = 175; {
/* line 748 "Breeze.lalr" */
 yySynAttribute.decl.Tree = mConstantDecl (yyAttrStackPtr [3-1].position.Position, NoTree,
	yyAttrStackPtr [4-1].Scan.breeze_ident.ident, mCoercedExpr (yyAttrStackPtr [5-1].expr.Tree), yyAttrStackPtr [6-1].type.Tree);
 ;

} break;
case 1007:
case 772: /* decl : '(' 'untyped-constant-decl' position breeze_ident expr ')' .*/
  yyStateStackPtr -= 6; yyAttrStackPtr -= 6; yyNonterminal = 175; {
/* line 752 "Breeze.lalr" */
 yySynAttribute.decl.Tree = mConstantDecl (yyAttrStackPtr [3-1].position.Position, NoTree,
	yyAttrStackPtr [4-1].Scan.breeze_ident.ident, mCoercedExpr (yyAttrStackPtr [5-1].expr.Tree), mNullType (NoPosition));
 ;

} break;
case 1008:
case 773: /* decl : '(' 'variable-decl' position idents type ')' .*/
  yyStateStackPtr -= 6; yyAttrStackPtr -= 6; yyNonterminal = 175; {
/* line 756 "Breeze.lalr" */
 yySynAttribute.decl.Tree = mVariableDecl (yyAttrStackPtr [3-1].position.Position, NoTree,
	ReverseTree (yyAttrStackPtr [4-1].idents.Tree), yyAttrStackPtr [5-1].type.Tree);
 ;

} break;
case 1009:
case 774: /* decl : '(' 'init-variable-decl' position idents expr type ')' .*/
  yyStateStackPtr -= 7; yyAttrStackPtr -= 7; yyNonterminal = 175; {
/* line 760 "Breeze.lalr" */
 yySynAttribute.decl.Tree = mInitVariableDecl (yyAttrStackPtr [3-1].position.Position, NoTree,
	ReverseTree (yyAttrStackPtr [4-1].idents.Tree), mCoercedExpr (yyAttrStackPtr [5-1].expr.Tree), yyAttrStackPtr [6-1].type.Tree);
 ;

} break;
case 1010:
case 776: /* decl : '(' 'channel-decl' position idents type channel_options ')' .*/
  yyStateStackPtr -= 7; yyAttrStackPtr -= 7; yyNonterminal = 175; {
/* line 764 "Breeze.lalr" */
 yySynAttribute.decl.Tree = mChannelDecl (yyAttrStackPtr [3-1].position.Position, NoTree,
	ReverseTree (yyAttrStackPtr [4-1].idents.Tree), yyAttrStackPtr [5-1].type.Tree, yyAttrStackPtr [6-1].channel_options.multicast);
 ;

} break;
case 1011:
case 777: /* decl : '(' 'arrayed-channel-decl' position idents range type channel_options ')' .*/
  yyStateStackPtr -= 8; yyAttrStackPtr -= 8; yyNonterminal = 175; {
/* line 768 "Breeze.lalr" */
 yySynAttribute.decl.Tree = mChannelArrayDecl (yyAttrStackPtr [3-1].position.Position, NoTree,
	ReverseTree (yyAttrStackPtr [4-1].idents.Tree), yyAttrStackPtr [6-1].type.Tree, yyAttrStackPtr [5-1].range.Tree, yyAttrStackPtr [7-1].channel_options.multicast);
 ;

} break;
case 1012:
case 778: /* decl : '(' 'sync-decl' position idents channel_options ')' .*/
  yyStateStackPtr -= 6; yyAttrStackPtr -= 6; yyNonterminal = 175; {
/* line 772 "Breeze.lalr" */
 yySynAttribute.decl.Tree = mSyncDecl (yyAttrStackPtr [3-1].position.Position, NoTree, ReverseTree (yyAttrStackPtr [4-1].idents.Tree), yyAttrStackPtr [5-1].channel_options.multicast);
 ;

} break;
case 1013:
case 779: /* decl : '(' 'arrayed-sync-decl' position idents range channel_options ')' .*/
  yyStateStackPtr -= 7; yyAttrStackPtr -= 7; yyNonterminal = 175; {
/* line 775 "Breeze.lalr" */
 yySynAttribute.decl.Tree = mSyncArrayDecl (yyAttrStackPtr [3-1].position.Position, NoTree, ReverseTree (yyAttrStackPtr [4-1].idents.Tree),
	yyAttrStackPtr [5-1].range.Tree, yyAttrStackPtr [6-1].channel_options.multicast);
 ;

} break;
case 1014:
case 860: /* decl : '(' 'procedure-decl' position breeze_ident formal_ports block ')' .*/
  yyStateStackPtr -= 7; yyAttrStackPtr -= 7; yyNonterminal = 175; {
/* line 779 "Breeze.lalr" */
 yySynAttribute.decl.Tree = mProcedureDecl (yyAttrStackPtr [3-1].position.Position, NoTree, yyAttrStackPtr [4-1].Scan.breeze_ident.ident,
	ReverseTree (yyAttrStackPtr [5-1].formal_ports.Tree), yyAttrStackPtr [6-1].block.Tree);
 ;

} break;
case 1015:
case 861: /* decl : '(' 'procedure-alias-decl' position breeze_ident breeze_ident ')' .*/
  yyStateStackPtr -= 6; yyAttrStackPtr -= 6; yyNonterminal = 175; {
/* line 783 "Breeze.lalr" */
 yySynAttribute.decl.Tree = mProcAliasDecl (yyAttrStackPtr [3-1].position.Position, NoTree,
	yyAttrStackPtr [4-1].Scan.breeze_ident.ident, yyAttrStackPtr [5-1].Scan.breeze_ident.ident, mNullProcParams (yyAttrStackPtr [3-1].position.Position));
 ;

} break;
case 1016:
case 862: /* decl : '(' 'procedure-param-alias-decl' position breeze_ident breeze_ident procedure_params ')' .*/
  yyStateStackPtr -= 7; yyAttrStackPtr -= 7; yyNonterminal = 175; {
/* line 787 "Breeze.lalr" */
 yySynAttribute.decl.Tree = mProcAliasDecl (yyAttrStackPtr [3-1].position.Position, NoTree,
	yyAttrStackPtr [4-1].Scan.breeze_ident.ident, yyAttrStackPtr [5-1].Scan.breeze_ident.ident, ReverseTree (yyAttrStackPtr [6-1].procedure_params.Tree));
 ;

} break;
case 1017:
case 863: /* decl : '(' 'typed-function-decl' position breeze_ident formal_ports expr type ')' .*/
  yyStateStackPtr -= 8; yyAttrStackPtr -= 8; yyNonterminal = 175; {
/* line 791 "Breeze.lalr" */
 yySynAttribute.decl.Tree = mFunctionDecl (yyAttrStackPtr [3-1].position.Position, NoTree, yyAttrStackPtr [4-1].Scan.breeze_ident.ident,
	ReverseTree (yyAttrStackPtr [5-1].formal_ports.Tree), mCoercedExpr (yyAttrStackPtr [6-1].expr.Tree), yyAttrStackPtr [7-1].type.Tree);
 ;

} break;
case 1018:
case 864: /* decl : '(' 'untyped-function-decl' position breeze_ident formal_ports expr ')' .*/
  yyStateStackPtr -= 7; yyAttrStackPtr -= 7; yyNonterminal = 175; {
/* line 795 "Breeze.lalr" */
 yySynAttribute.decl.Tree = mFunctionDecl (yyAttrStackPtr [3-1].position.Position, NoTree, yyAttrStackPtr [4-1].Scan.breeze_ident.ident,
	ReverseTree (yyAttrStackPtr [5-1].formal_ports.Tree), mCoercedExpr (yyAttrStackPtr [6-1].expr.Tree), mNullType (NoPosition));
 ;

} break;
case 1019:
case 865: /* decl : '(' 'builtin-function-decl' position breeze_ident formal_ports type ')' .*/
  yyStateStackPtr -= 7; yyAttrStackPtr -= 7; yyNonterminal = 175; {
/* line 799 "Breeze.lalr" */
 yySynAttribute.decl.Tree = mBuiltinFunctionDecl (yyAttrStackPtr [3-1].position.Position, NoTree, yyAttrStackPtr [4-1].Scan.breeze_ident.ident,
	ReverseTree (yyAttrStackPtr [5-1].formal_ports.Tree), yyAttrStackPtr [6-1].type.Tree);
 ;

} break;
case 1020:
case 866: /* decl : '(' 'shared-decl' position breeze_ident block ')' .*/
  yyStateStackPtr -= 6; yyAttrStackPtr -= 6; yyNonterminal = 175; {
/* line 803 "Breeze.lalr" */
 yySynAttribute.decl.Tree = mSharedDecl (yyAttrStackPtr [3-1].position.Position, NoTree, yyAttrStackPtr [4-1].Scan.breeze_ident.ident, yyAttrStackPtr [5-1].block.Tree);

} break;
case 1021:
case 869: /* decl : '(' 'if-decls' position decl_guards ')' .*/
  yyStateStackPtr -= 5; yyAttrStackPtr -= 5; yyNonterminal = 175; {
/* line 805 "Breeze.lalr" */
 yySynAttribute.decl.Tree = mIfDecls (yyAttrStackPtr [3-1].position.Position, NoTree, ReverseTree (yyAttrStackPtr [4-1].decl_guards.Tree));
 ;

} break;
case 1022:
case 870: /* decl : '(' 'if-else-decls' position decl_guards decls ')' .*/
  yyStateStackPtr -= 6; yyAttrStackPtr -= 6; yyNonterminal = 175; {
/* line 808 "Breeze.lalr" */
 yySynAttribute.decl.Tree = mIfElseDecls (yyAttrStackPtr [3-1].position.Position, NoTree, ReverseTree (yyAttrStackPtr [4-1].decl_guards.Tree), ReverseTree (yyAttrStackPtr [5-1].decls.Tree));
 ;

} break;
case 1023:
case 871: /* decl : '(' 'print-decl' position expr exprs ')' .*/
  yyStateStackPtr -= 6; yyAttrStackPtr -= 6; yyNonterminal = 175; {
/* line 811 "Breeze.lalr" */
 yySynAttribute.decl.Tree = mPrintDecl (yyAttrStackPtr [3-1].position.Position, NoTree, yyAttrStackPtr [4-1].expr.Tree, ReverseTree (yyAttrStackPtr [5-1].exprs.Tree));
 ;

} break;
case 1024: /* channel_options : .*/
  yyStateStackPtr -= 0; yyAttrStackPtr -= 0; yyNonterminal = 201; {
/* line 814 "Breeze.lalr" */
 yySynAttribute.channel_options.multicast = false;
 ;

} break;
case 1025:
case 775: /* channel_options : 'multicast' .*/
  yyStateStackPtr -= 1; yyAttrStackPtr -= 1; yyNonterminal = 201; {
/* line 817 "Breeze.lalr" */
 yySynAttribute.channel_options.multicast = true;
 ;

} break;
case 1026:
case 740: /* range : '(' 'range1' position expr ')' .*/
  yyStateStackPtr -= 5; yyAttrStackPtr -= 5; yyNonterminal = 202; {
/* line 820 "Breeze.lalr" */
 yySynAttribute.range.Tree = mSpecifiedRange (yyAttrStackPtr [3-1].position.Position, mNullExpr (yyAttrStackPtr [3-1].position.Position), yyAttrStackPtr [4-1].expr.Tree);
 ;

} break;
case 1027:
case 741: /* range : '(' 'range2' position expr expr ')' .*/
  yyStateStackPtr -= 6; yyAttrStackPtr -= 6; yyNonterminal = 202; {
/* line 823 "Breeze.lalr" */
 yySynAttribute.range.Tree = mSpecifiedRange (yyAttrStackPtr [3-1].position.Position, yyAttrStackPtr [4-1].expr.Tree, yyAttrStackPtr [5-1].expr.Tree);
 ;

} break;
case 1028:
case 742: /* range : '(' 'type-range' position type ')' .*/
  yyStateStackPtr -= 5; yyAttrStackPtr -= 5; yyNonterminal = 202; {
/* line 826 "Breeze.lalr" */
 yySynAttribute.range.Tree = mTypeRange (yyAttrStackPtr [3-1].position.Position, yyAttrStackPtr [4-1].type.Tree);
 ;

} break;
case 1029:
case 766: /* type : '(' 'existing-type' position breeze_ident ')' .*/
  yyStateStackPtr -= 5; yyAttrStackPtr -= 5; yyNonterminal = 198; {
/* line 829 "Breeze.lalr" */
 yySynAttribute.type.Tree = mExistingType (yyAttrStackPtr [4-1].Scan.Position, yyAttrStackPtr [4-1].Scan.breeze_ident.ident);
 ;

} break;
case 1030:
case 754: /* type : '(' 'numeric-type' position boolean expr ')' .*/
  yyStateStackPtr -= 6; yyAttrStackPtr -= 6; yyNonterminal = 198; {
/* line 832 "Breeze.lalr" */
 yySynAttribute.type.Tree = mNumericType (yyAttrStackPtr [3-1].position.Position, yyAttrStackPtr [4-1].Scan.boolean.value, yyAttrStackPtr [5-1].expr.Tree);
 ;

} break;
case 1031:
case 755: /* type : '(' 'array-type' position range type ')' .*/
  yyStateStackPtr -= 6; yyAttrStackPtr -= 6; yyNonterminal = 198; {
/* line 835 "Breeze.lalr" */
 yySynAttribute.type.Tree = mArrayType (yyAttrStackPtr [3-1].position.Position, yyAttrStackPtr [5-1].type.Tree, yyAttrStackPtr [4-1].range.Tree);
 ;

} break;
case 1032:
case 767: /* type : '(' 'new-type' position type ')' .*/
  yyStateStackPtr -= 5; yyAttrStackPtr -= 5; yyNonterminal = 198; {
/* line 838 "Breeze.lalr" */
 yySynAttribute.type.Tree = mNewType (yyAttrStackPtr [3-1].position.Position, yyAttrStackPtr [4-1].type.Tree);
 ;

} break;
case 1033:
case 760: /* type : '(' 'record-type' position record_elems ')' .*/
  yyStateStackPtr -= 5; yyAttrStackPtr -= 5; yyNonterminal = 198; {
/* line 841 "Breeze.lalr" */
 yySynAttribute.type.Tree = mRecordType (yyAttrStackPtr [3-1].position.Position, ReverseTree (yyAttrStackPtr [4-1].record_elems.Tree), mNullType (yyAttrStackPtr [3-1].position.Position));
 ;

} break;
case 1034:
case 768: /* type : '(' 'bounded-record-type' position record_elems type ')' .*/
  yyStateStackPtr -= 6; yyAttrStackPtr -= 6; yyNonterminal = 198; {
/* line 844 "Breeze.lalr" */
 yySynAttribute.type.Tree = mRecordType (yyAttrStackPtr [3-1].position.Position, ReverseTree (yyAttrStackPtr [4-1].record_elems.Tree), yyAttrStackPtr [5-1].type.Tree);
 ;

} break;
case 1035:
case 764: /* type : '(' 'enumeration-type' position enum_elems ')' .*/
  yyStateStackPtr -= 5; yyAttrStackPtr -= 5; yyNonterminal = 198; {
/* line 847 "Breeze.lalr" */
 yySynAttribute.type.Tree = mEnumType (yyAttrStackPtr [3-1].position.Position, ReverseTree (yyAttrStackPtr [4-1].enum_elems.Tree), mNullType (yyAttrStackPtr [3-1].position.Position));
 ;

} break;
case 1036:
case 769: /* type : '(' 'bounded-enumeration-type' position enum_elems type ')' .*/
  yyStateStackPtr -= 6; yyAttrStackPtr -= 6; yyNonterminal = 198; {
/* line 850 "Breeze.lalr" */
 yySynAttribute.type.Tree = mEnumType (yyAttrStackPtr [3-1].position.Position, ReverseTree (yyAttrStackPtr [4-1].enum_elems.Tree), yyAttrStackPtr [5-1].type.Tree);
 ;

} break;
case 1037:
case 765: /* type : '(' 'builtin-type' position ')' .*/
  yyStateStackPtr -= 4; yyAttrStackPtr -= 4; yyNonterminal = 198; {
/* line 853 "Breeze.lalr" */
 yySynAttribute.type.Tree = mBuiltinType (yyAttrStackPtr [3-1].position.Position);
 ;

} break;
case 1038:
case 747: /* val_decls : '(' 'val-decls' position val_decls_body ')' .*/
  yyStateStackPtr -= 5; yyAttrStackPtr -= 5; yyNonterminal = 212; {
/* line 856 "Breeze.lalr" */
 yySynAttribute.val_decls.Tree  =  yyAttrStackPtr [4-1].val_decls_body.Tree;
 ;

} break;
case 1039: /* val_decls_body : .*/
  yyStateStackPtr -= 0; yyAttrStackPtr -= 0; yyNonterminal = 211; {
/* line 859 "Breeze.lalr" */
 yySynAttribute.val_decls_body.Tree = mNullValDecls (NoPosition);
 ;

} break;
case 1040:
case 901: /* val_decls_body : val_decls_body val_decl .*/
  yyStateStackPtr -= 2; yyAttrStackPtr -= 2; yyNonterminal = 211; {
/* line 862 "Breeze.lalr" */
 yyAttrStackPtr [2-1].val_decl.Tree->ValDecl.next = yyAttrStackPtr [1-1].val_decls_body.Tree; yySynAttribute.val_decls_body.Tree = yyAttrStackPtr [2-1].val_decl.Tree; ;
 ;

} break;
case 1041:
case 746: /* val_decl : '(' 'val-decl' position breeze_ident expr ')' .*/
  yyStateStackPtr -= 6; yyAttrStackPtr -= 6; yyNonterminal = 213; {
/* line 865 "Breeze.lalr" */
 yySynAttribute.val_decl.Tree = mValDecl (yyAttrStackPtr [3-1].position.Position, NoTree, yyAttrStackPtr [4-1].Scan.breeze_ident.ident, yyAttrStackPtr [5-1].expr.Tree);
 ;

} break;
case 1042:
case 726: /* expr : '(' 'literal-expr' position breeze_literal ')' .*/
  yyStateStackPtr -= 5; yyAttrStackPtr -= 5; yyNonterminal = 199; {
/* line 868 "Breeze.lalr" */
 yySynAttribute.expr.Tree = mLiteralExpr (yyAttrStackPtr [3-1].position.Position, yyAttrStackPtr [4-1].Scan.breeze_literal.value);
 ;

} break;
case 1043:
case 727: /* expr : '(' 'ident-expr' position breeze_ident ')' .*/
  yyStateStackPtr -= 5; yyAttrStackPtr -= 5; yyNonterminal = 199; {
/* line 871 "Breeze.lalr" */
 yySynAttribute.expr.Tree = mIdentExpr (yyAttrStackPtr [3-1].position.Position, yyAttrStackPtr [4-1].Scan.breeze_ident.ident);
 ;

} break;
case 1044:
case 728: /* expr : '(' 'string-expr' position breeze_ident ')' .*/
  yyStateStackPtr -= 5; yyAttrStackPtr -= 5; yyNonterminal = 199; {
/* line 874 "Breeze.lalr" */
 yySynAttribute.expr.Tree = mStringExpr (yyAttrStackPtr [3-1].position.Position, PeekString (yyAttrStackPtr [4-1].Scan.breeze_ident.ident));
 ;

} break;
case 1045:
case 729: /* expr : '(' 'dont-care-expr' position ')' .*/
  yyStateStackPtr -= 4; yyAttrStackPtr -= 4; yyNonterminal = 199; {
/* line 877 "Breeze.lalr" */
 yySynAttribute.expr.Tree = mDontCareExpr (yyAttrStackPtr [3-1].position.Position);
 ;

} break;
case 1046:
case 730: /* expr : '(' 'implicant-expr' position breeze_literal breeze_literal ')' .*/
  yyStateStackPtr -= 6; yyAttrStackPtr -= 6; yyNonterminal = 199; {
/* line 880 "Breeze.lalr" */
 yySynAttribute.expr.Tree = mImplicantExpr (yyAttrStackPtr [3-1].position.Position, NewImplicant (yyAttrStackPtr [4-1].Scan.breeze_literal.value, yyAttrStackPtr [5-1].Scan.breeze_literal.value));
 ;

} break;
case 1047:
case 732: /* expr : '(' 'named-aggr-cons-expr' position breeze_ident exprs ')' .*/
  yyStateStackPtr -= 6; yyAttrStackPtr -= 6; yyNonterminal = 199; {
/* line 883 "Breeze.lalr" */
 yySynAttribute.expr.Tree = mAggregateConsExpr (yyAttrStackPtr [3-1].position.Position, yyAttrStackPtr [4-1].Scan.breeze_ident.ident, ReverseTree (yyAttrStackPtr [5-1].exprs.Tree));
 ;

} break;
case 1048:
case 733: /* expr : '(' 'aggr-cons-expr' position exprs ')' .*/
  yyStateStackPtr -= 5; yyAttrStackPtr -= 5; yyNonterminal = 199; {
/* line 886 "Breeze.lalr" */
 yySynAttribute.expr.Tree = mAggregateConsExpr (yyAttrStackPtr [3-1].position.Position, NoIdent, ReverseTree (yyAttrStackPtr [4-1].exprs.Tree));
 ;

} break;
case 1049:
case 734: /* expr : '(' 'enum-elem-expr' position breeze_ident breeze_ident ')' .*/
  yyStateStackPtr -= 6; yyAttrStackPtr -= 6; yyNonterminal = 199; {
/* line 889 "Breeze.lalr" */
 yySynAttribute.expr.Tree = mNamedEnumElemExpr (yyAttrStackPtr [3-1].position.Position, yyAttrStackPtr [4-1].Scan.breeze_ident.ident, yyAttrStackPtr [5-1].Scan.breeze_ident.ident);
 ;

} break;
case 1050:
case 735: /* expr : '(' 'unary-expr' position breeze_ident expr ')' .*/
  yyStateStackPtr -= 6; yyAttrStackPtr -= 6; yyNonterminal = 199; {
/* line 892 "Breeze.lalr" */
 yySynAttribute.expr.Tree = mUnaryExpr (yyAttrStackPtr [3-1].position.Position, FindUnaryOperator (yyAttrStackPtr [4-1].Scan.breeze_ident.ident, false),
	yyAttrStackPtr [5-1].expr.Tree);
 ;

} break;
case 1051:
case 736: /* expr : '(' 'sizeof-expr' position breeze_ident ')' .*/
  yyStateStackPtr -= 5; yyAttrStackPtr -= 5; yyNonterminal = 199; {
/* line 896 "Breeze.lalr" */
 yySynAttribute.expr.Tree = mSizeofExpr (yyAttrStackPtr [3-1].position.Position, yyAttrStackPtr [4-1].Scan.breeze_ident.ident);
 ;

} break;
case 1052:
case 737: /* expr : '(' 'binary-expr' position breeze_ident expr expr ')' .*/
  yyStateStackPtr -= 7; yyAttrStackPtr -= 7; yyNonterminal = 199; {
/* line 899 "Breeze.lalr" */
 yySynAttribute.expr.Tree = mBinaryExpr (yyAttrStackPtr [3-1].position.Position, FindBinaryOperator (yyAttrStackPtr [4-1].Scan.breeze_ident.ident, false), yyAttrStackPtr [5-1].expr.Tree, yyAttrStackPtr [6-1].expr.Tree);

;

} break;
case 1053:
case 738: /* expr : '(' 'record-elem-extract-expr' position expr breeze_ident ')' .*/
  yyStateStackPtr -= 6; yyAttrStackPtr -= 6; yyNonterminal = 199; {
/* line 903 "Breeze.lalr" */
 yySynAttribute.expr.Tree = mRecordElemExtractExpr (yyAttrStackPtr [3-1].position.Position, yyAttrStackPtr [4-1].expr.Tree, yyAttrStackPtr [5-1].Scan.breeze_ident.ident);
 ;

} break;
case 1054:
case 739: /* expr : '(' 'array-extract-expr' position expr expr ')' .*/
  yyStateStackPtr -= 6; yyAttrStackPtr -= 6; yyNonterminal = 199; {
/* line 906 "Breeze.lalr" */
 yySynAttribute.expr.Tree = mArrayExtractExpr (yyAttrStackPtr [3-1].position.Position, yyAttrStackPtr [4-1].expr.Tree, mCoercedExpr(yyAttrStackPtr [5-1].expr.Tree));
 ;

} break;
case 1055:
case 743: /* expr : '(' 'array-slice-expr' position expr range ')' .*/
  yyStateStackPtr -= 6; yyAttrStackPtr -= 6; yyNonterminal = 199; {
/* line 909 "Breeze.lalr" */
 yySynAttribute.expr.Tree = mArraySliceExpr (yyAttrStackPtr [3-1].position.Position, yyAttrStackPtr [4-1].expr.Tree, yyAttrStackPtr [5-1].range.Tree);
 ;

} break;
case 1056:
case 744: /* expr : '(' 'as-expr' position expr type ')' .*/
  yyStateStackPtr -= 6; yyAttrStackPtr -= 6; yyNonterminal = 199; {
/* line 912 "Breeze.lalr" */
 yySynAttribute.expr.Tree = mAsExpr (yyAttrStackPtr [3-1].position.Position, yyAttrStackPtr [4-1].expr.Tree, yyAttrStackPtr [5-1].type.Tree);
 ;

} break;
case 1057:
case 745: /* expr : '(' 'bit-array-cast-expr' position expr ')' .*/
  yyStateStackPtr -= 5; yyAttrStackPtr -= 5; yyNonterminal = 199; {
/* line 915 "Breeze.lalr" */
 yySynAttribute.expr.Tree = mBitArrayCastExpr (yyAttrStackPtr [3-1].position.Position, yyAttrStackPtr [4-1].expr.Tree);
 ;

} break;
case 1058:
case 748: /* expr : '(' 'let-expr' position val_decls expr ')' .*/
  yyStateStackPtr -= 6; yyAttrStackPtr -= 6; yyNonterminal = 199; {
/* line 918 "Breeze.lalr" */
 yySynAttribute.expr.Tree = mLetExpr (yyAttrStackPtr [3-1].position.Position, ReverseTree (yyAttrStackPtr [4-1].val_decls.Tree), yyAttrStackPtr [5-1].expr.Tree);
 ;

} break;
case 1059:
case 752: /* expr : '(' 'function-call-expr' position breeze_ident function_params ')' .*/
  yyStateStackPtr -= 6; yyAttrStackPtr -= 6; yyNonterminal = 199; {
/* line 921 "Breeze.lalr" */
 yySynAttribute.expr.Tree = mFunctionCallExpr (yyAttrStackPtr [3-1].position.Position, yyAttrStackPtr [4-1].Scan.breeze_ident.ident, ReverseTree (yyAttrStackPtr [5-1].function_params.Tree));
 ;

} break;
case 1060:
case 753: /* expr : '(' 'array-append-expr' position expr expr ')' .*/
  yyStateStackPtr -= 6; yyAttrStackPtr -= 6; yyNonterminal = 199; {
/* line 924 "Breeze.lalr" */
 yySynAttribute.expr.Tree = mArrayAppendExpr (yyAttrStackPtr [3-1].position.Position, yyAttrStackPtr [4-1].expr.Tree, yyAttrStackPtr [5-1].expr.Tree);
 ;

} break;
case 1061:
case 781: /* port_sense : 'default' .*/
  yyStateStackPtr -= 1; yyAttrStackPtr -= 1; yyNonterminal = 189; {
/* line 927 "Breeze.lalr" */
 yySynAttribute.port_sense.portSense = DefaultPortSense;
  ;

} break;
case 1062:
case 782: /* port_sense : 'active' .*/
  yyStateStackPtr -= 1; yyAttrStackPtr -= 1; yyNonterminal = 189; {
/* line 930 "Breeze.lalr" */
 yySynAttribute.port_sense.portSense = ActivePortSense;
 ;

} break;
case 1063:
case 783: /* port_sense : 'passive' .*/
  yyStateStackPtr -= 1; yyAttrStackPtr -= 1; yyNonterminal = 189; {
/* line 933 "Breeze.lalr" */
 yySynAttribute.port_sense.portSense = PassivePortSense;
 ;

} break;
case 1064:
case 784: /* port_direction : 'input' .*/
  yyStateStackPtr -= 1; yyAttrStackPtr -= 1; yyNonterminal = 191; {
/* line 936 "Breeze.lalr" */
 yySynAttribute.port_direction.isOutput = false;
 ;

} break;
case 1065:
case 785: /* port_direction : 'output' .*/
  yyStateStackPtr -= 1; yyAttrStackPtr -= 1; yyNonterminal = 191; {
/* line 939 "Breeze.lalr" */
 yySynAttribute.port_direction.isOutput = true;
 ;

} break;
case 1066:
case 780: /* port : '(' 'value-port' position idents type ')' .*/
  yyStateStackPtr -= 6; yyAttrStackPtr -= 6; yyNonterminal = 215; {
/* line 942 "Breeze.lalr" */
 yySynAttribute.port.Tree = mValuePort (yyAttrStackPtr [3-1].position.Position, NoTree, ReverseTree (yyAttrStackPtr [4-1].idents.Tree), yyAttrStackPtr [5-1].type.Tree);
 ;

} break;
case 1067:
case 786: /* port : '(' 'port' position port_sense port_direction idents type ')' .*/
  yyStateStackPtr -= 8; yyAttrStackPtr -= 8; yyNonterminal = 215; {
/* line 945 "Breeze.lalr" */
 yySynAttribute.port.Tree = mChannelPort (yyAttrStackPtr [3-1].position.Position, NoTree, ReverseTree (yyAttrStackPtr [6-1].idents.Tree), yyAttrStackPtr [7-1].type.Tree,
	yyAttrStackPtr [4-1].port_sense.portSense, yyAttrStackPtr [5-1].port_direction.isOutput, NULL);
 ;

} break;
case 1068:
case 787: /* port : '(' 'arrayed-port' position port_sense port_direction idents range type ')' .*/
  yyStateStackPtr -= 9; yyAttrStackPtr -= 9; yyNonterminal = 215; {
/* line 949 "Breeze.lalr" */
 yySynAttribute.port.Tree = mChannelPortArray (yyAttrStackPtr [3-1].position.Position, NoTree, ReverseTree (yyAttrStackPtr [6-1].idents.Tree), yyAttrStackPtr [8-1].type.Tree,
	yyAttrStackPtr [4-1].port_sense.portSense, yyAttrStackPtr [5-1].port_direction.isOutput, yyAttrStackPtr [7-1].range.Tree, NULL);
 ;

} break;
case 1069:
case 788: /* port : '(' 'sync-port' position port_sense idents ')' .*/
  yyStateStackPtr -= 6; yyAttrStackPtr -= 6; yyNonterminal = 215; {
/* line 953 "Breeze.lalr" */
 yySynAttribute.port.Tree = mSyncPort (yyAttrStackPtr [3-1].position.Position, NoTree, ReverseTree (yyAttrStackPtr [5-1].idents.Tree), yyAttrStackPtr [4-1].port_sense.portSense,
	NULL);
 ;

} break;
case 1070:
case 789: /* port : '(' 'arrayed-sync-port' position port_sense idents range ')' .*/
  yyStateStackPtr -= 7; yyAttrStackPtr -= 7; yyNonterminal = 215; {
/* line 957 "Breeze.lalr" */
 yySynAttribute.port.Tree = mSyncPortArray (yyAttrStackPtr [3-1].position.Position, NoTree, ReverseTree (yyAttrStackPtr [5-1].idents.Tree),
	yyAttrStackPtr [4-1].port_sense.portSense, yyAttrStackPtr [6-1].range.Tree, NULL);
 ;

} break;
case 1071:
case 790: /* port : '(' 'param-port' position idents type ')' .*/
  yyStateStackPtr -= 6; yyAttrStackPtr -= 6; yyNonterminal = 215; {
/* line 961 "Breeze.lalr" */
 yySynAttribute.port.Tree = mParamPort (yyAttrStackPtr [3-1].position.Position, NoTree, ReverseTree (yyAttrStackPtr [4-1].idents.Tree), yyAttrStackPtr [5-1].type.Tree);
 ;

} break;
case 1072:
case 791: /* port : '(' 'type-param-port' position idents ')' .*/
  yyStateStackPtr -= 5; yyAttrStackPtr -= 5; yyNonterminal = 215; {
/* line 964 "Breeze.lalr" */
 yySynAttribute.port.Tree = mTypeParamPort (yyAttrStackPtr [3-1].position.Position, NoTree, ReverseTree (yyAttrStackPtr [4-1].idents.Tree));
 ;

} break;
case 1073:
case 794: /* port : '(' 'if-ports' position port_guards ')' .*/
  yyStateStackPtr -= 5; yyAttrStackPtr -= 5; yyNonterminal = 215; {
/* line 967 "Breeze.lalr" */
 yySynAttribute.port.Tree = mIfPorts (yyAttrStackPtr [3-1].position.Position, NoTree, ReverseTree (yyAttrStackPtr [4-1].port_guards.Tree));
 ;

} break;
case 1074:
case 795: /* port : '(' 'if-else-ports' position port_guards formal_ports ')' .*/
  yyStateStackPtr -= 6; yyAttrStackPtr -= 6; yyNonterminal = 215; {
/* line 970 "Breeze.lalr" */
 yySynAttribute.port.Tree = mIfElsePorts (yyAttrStackPtr [3-1].position.Position, NoTree, ReverseTree (yyAttrStackPtr [4-1].port_guards.Tree),
	ReverseTree (yyAttrStackPtr [5-1].formal_ports.Tree));
 ;

} break;
case 1075:
case 859: /* block : '(' 'block' position decls command ')' .*/
  yyStateStackPtr -= 6; yyAttrStackPtr -= 6; yyNonterminal = 204; {
/* line 974 "Breeze.lalr" */
 yySynAttribute.block.Tree = mBlock (yyAttrStackPtr [3-1].position.Position, ReverseTree (yyAttrStackPtr [4-1].decls.Tree), yyAttrStackPtr [5-1].command.Tree);
 ;

} break;
case 1076:
case 812: /* command : '(' 'continue' position ')' .*/
  yyStateStackPtr -= 4; yyAttrStackPtr -= 4; yyNonterminal = 217; {
/* line 977 "Breeze.lalr" */
 yySynAttribute.command.Tree = mContinueCommand (yyAttrStackPtr [3-1].position.Position);
 ;

} break;
case 1077:
case 813: /* command : '(' 'halt' position ')' .*/
  yyStateStackPtr -= 4; yyAttrStackPtr -= 4; yyNonterminal = 217; {
/* line 980 "Breeze.lalr" */
 yySynAttribute.command.Tree = mHaltCommand (yyAttrStackPtr [3-1].position.Position);
 ;

} break;
case 1078:
case 810: /* command : '(' 'input' position lvalue lvalue ')' .*/
  yyStateStackPtr -= 6; yyAttrStackPtr -= 6; yyNonterminal = 217; {
/* line 983 "Breeze.lalr" */
 yySynAttribute.command.Tree = mInputCommand (yyAttrStackPtr [3-1].position.Position, yyAttrStackPtr [4-1].lvalue.Tree, yyAttrStackPtr [5-1].lvalue.Tree);
 ;

} break;
case 1079:
case 814: /* command : '(' 'input-enclose' position lvalues command ')' .*/
  yyStateStackPtr -= 6; yyAttrStackPtr -= 6; yyNonterminal = 217; {
/* line 986 "Breeze.lalr" */
 yySynAttribute.command.Tree = mInputEncloseCommand (yyAttrStackPtr [3-1].position.Position, yyAttrStackPtr [4-1].lvalues.Tree, yyAttrStackPtr [5-1].command.Tree);
 ;

} break;
case 1080:
case 815: /* command : '(' 'input-enclose-bang' position lvalues command ')' .*/
  yyStateStackPtr -= 6; yyAttrStackPtr -= 6; yyNonterminal = 217; {
/* line 989 "Breeze.lalr" */
 yySynAttribute.command.Tree = mInputEncloseBangCommand (yyAttrStackPtr [3-1].position.Position, yyAttrStackPtr [4-1].lvalues.Tree, yyAttrStackPtr [5-1].command.Tree);
 ;

} break;
case 1081:
case 811: /* command : '(' 'output' position lvalue expr ')' .*/
  yyStateStackPtr -= 6; yyAttrStackPtr -= 6; yyNonterminal = 217; {
/* line 992 "Breeze.lalr" */
 yySynAttribute.command.Tree = mOutputCommand (yyAttrStackPtr [3-1].position.Position, yyAttrStackPtr [4-1].lvalue.Tree, mCoercedExpr (yyAttrStackPtr [5-1].expr.Tree));
 ;

} break;
case 1082:
case 816: /* command : '(' 'sync' position lvalue ')' .*/
  yyStateStackPtr -= 5; yyAttrStackPtr -= 5; yyNonterminal = 217; {
/* line 995 "Breeze.lalr" */
 yySynAttribute.command.Tree = mSyncCommand (yyAttrStackPtr [3-1].position.Position, yyAttrStackPtr [4-1].lvalue.Tree);
 ;

} break;
case 1083:
case 817: /* command : '(' 'assign' position lvalue expr ')' .*/
  yyStateStackPtr -= 6; yyAttrStackPtr -= 6; yyNonterminal = 217; {
/* line 998 "Breeze.lalr" */
 yySynAttribute.command.Tree = mAssignmentCommand (yyAttrStackPtr [3-1].position.Position, yyAttrStackPtr [4-1].lvalue.Tree, mCoercedExpr (yyAttrStackPtr [5-1].expr.Tree));
 ;

} break;
case 1084:
case 818: /* command : '(' 'block-command' position block ')' .*/
  yyStateStackPtr -= 5; yyAttrStackPtr -= 5; yyNonterminal = 217; {
/* line 1001 "Breeze.lalr" */
 yySynAttribute.command.Tree = mBlockCommand (yyAttrStackPtr [3-1].position.Position, yyAttrStackPtr [4-1].block.Tree);
 ;

} break;
case 1085:
case 819: /* command : '(' 'sequential' position command command ')' .*/
  yyStateStackPtr -= 6; yyAttrStackPtr -= 6; yyNonterminal = 217; {
/* line 1004 "Breeze.lalr" */
 yySynAttribute.command.Tree = mSequentialCommand (yyAttrStackPtr [3-1].position.Position, yyAttrStackPtr [4-1].command.Tree, yyAttrStackPtr [5-1].command.Tree);
 ;

} break;
case 1086:
case 820: /* command : '(' 'parallel' position command command ')' .*/
  yyStateStackPtr -= 6; yyAttrStackPtr -= 6; yyNonterminal = 217; {
/* line 1007 "Breeze.lalr" */
 yySynAttribute.command.Tree = mParallelCommand (yyAttrStackPtr [3-1].position.Position, false, yyAttrStackPtr [4-1].command.Tree, yyAttrStackPtr [5-1].command.Tree);
 ;

} break;
case 1087:
case 821: /* command : '(' 'permissive-parallel' position command command ')' .*/
  yyStateStackPtr -= 6; yyAttrStackPtr -= 6; yyNonterminal = 217; {
/* line 1010 "Breeze.lalr" */
 yySynAttribute.command.Tree = mParallelCommand (yyAttrStackPtr [3-1].position.Position, true, yyAttrStackPtr [4-1].command.Tree, yyAttrStackPtr [5-1].command.Tree);
 ;

} break;
case 1088:
case 822: /* command : '(' 'loop' position command ')' .*/
  yyStateStackPtr -= 5; yyAttrStackPtr -= 5; yyNonterminal = 217; {
/* line 1013 "Breeze.lalr" */
 yySynAttribute.command.Tree = mLoopCommand (yyAttrStackPtr [3-1].position.Position, yyAttrStackPtr [4-1].command.Tree);
 ;

} break;
case 1089:
case 825: /* command : '(' 'while-guards' position guards ')' .*/
  yyStateStackPtr -= 5; yyAttrStackPtr -= 5; yyNonterminal = 217; {
/* line 1016 "Breeze.lalr" */
 yySynAttribute.command.Tree = mWhileGuardsCommand (yyAttrStackPtr [3-1].position.Position, ReverseTree (yyAttrStackPtr [4-1].guards.Tree));
 ;

} break;
case 1090:
case 826: /* command : '(' 'while-guards-also' position guards command ')' .*/
  yyStateStackPtr -= 6; yyAttrStackPtr -= 6; yyNonterminal = 217; {
/* line 1019 "Breeze.lalr" */
 yySynAttribute.command.Tree = mWhileGuardsAlsoCommand (yyAttrStackPtr [3-1].position.Position, ReverseTree (yyAttrStackPtr [4-1].guards.Tree), yyAttrStackPtr [5-1].command.Tree);
 ;

} break;
case 1091:
case 827: /* command : '(' 'command-while-expr' position command expr ')' .*/
  yyStateStackPtr -= 6; yyAttrStackPtr -= 6; yyNonterminal = 217; {
/* line 1022 "Breeze.lalr" */
 yySynAttribute.command.Tree = mCommandWhileExprCommand (yyAttrStackPtr [3-1].position.Position, yyAttrStackPtr [4-1].command.Tree, yyAttrStackPtr [5-1].expr.Tree);
 ;

} break;
case 1092:
case 828: /* command : '(' 'command-while-guards' position command guards ')' .*/
  yyStateStackPtr -= 6; yyAttrStackPtr -= 6; yyNonterminal = 217; {
/* line 1025 "Breeze.lalr" */
 yySynAttribute.command.Tree = mCommandWhileGuardsCommand (yyAttrStackPtr [3-1].position.Position, yyAttrStackPtr [4-1].command.Tree, ReverseTree (yyAttrStackPtr [5-1].guards.Tree));
 ;

} break;
case 1093:
case 829: /* command : '(' 'command-while-guards-also' position command guards command ')' .*/
  yyStateStackPtr -= 7; yyAttrStackPtr -= 7; yyNonterminal = 217; {
/* line 1028 "Breeze.lalr" */
 yySynAttribute.command.Tree = mCommandWhileGuardsAlsoCommand (yyAttrStackPtr [3-1].position.Position, yyAttrStackPtr [4-1].command.Tree, ReverseTree (yyAttrStackPtr [5-1].guards.Tree),
	yyAttrStackPtr [6-1].command.Tree);
 ;

} break;
case 1094:
case 830: /* command : '(' 'if' position guards ')' .*/
  yyStateStackPtr -= 5; yyAttrStackPtr -= 5; yyNonterminal = 217; {
/* line 1032 "Breeze.lalr" */
 yySynAttribute.command.Tree = mIfCommand (yyAttrStackPtr [3-1].position.Position, ReverseTree (yyAttrStackPtr [4-1].guards.Tree));
 ;

} break;
case 1095:
case 831: /* command : '(' 'if-else' position guards command ')' .*/
  yyStateStackPtr -= 6; yyAttrStackPtr -= 6; yyNonterminal = 217; {
/* line 1035 "Breeze.lalr" */
 yySynAttribute.command.Tree = mIfElseCommand (yyAttrStackPtr [3-1].position.Position, ReverseTree (yyAttrStackPtr [4-1].guards.Tree), yyAttrStackPtr [5-1].command.Tree);
 ;

} break;
case 1096:
case 838: /* command : '(' 'case' position expr case_guards ')' .*/
  yyStateStackPtr -= 6; yyAttrStackPtr -= 6; yyNonterminal = 217; {
/* line 1038 "Breeze.lalr" */
 yySynAttribute.command.Tree = mCaseCommand (yyAttrStackPtr [3-1].position.Position, yyAttrStackPtr [4-1].expr.Tree,
	ReverseTree (yyAttrStackPtr [5-1].case_guards.Tree), mNullCommand (yyAttrStackPtr [3-1].position.Position));
 ;

} break;
case 1097:
case 839: /* command : '(' 'case-else' position expr case_guards command ')' .*/
  yyStateStackPtr -= 7; yyAttrStackPtr -= 7; yyNonterminal = 217; {
/* line 1042 "Breeze.lalr" */
 yySynAttribute.command.Tree = mCaseCommand (yyAttrStackPtr [3-1].position.Position, yyAttrStackPtr [4-1].expr.Tree, ReverseTree (yyAttrStackPtr [5-1].case_guards.Tree), yyAttrStackPtr [6-1].command.Tree);
 ;

} break;
case 1098:
case 843: /* command : '(' 'for' position par_seq breeze_ident range command ')' .*/
  yyStateStackPtr -= 8; yyAttrStackPtr -= 8; yyNonterminal = 217; {
/* line 1045 "Breeze.lalr" */
 yySynAttribute.command.Tree = mForCommand (yyAttrStackPtr [3-1].position.Position, yyAttrStackPtr [4-1].par_seq.isParallel, yyAttrStackPtr [4-1].par_seq.isPermissive, yyAttrStackPtr [5-1].Scan.breeze_ident.ident,
	yyAttrStackPtr [6-1].range.Tree, yyAttrStackPtr [7-1].command.Tree);
 ;

} break;
case 1099:
case 850: /* command : '(' 'procedure-call' position breeze_ident procedure_params ')' .*/
  yyStateStackPtr -= 6; yyAttrStackPtr -= 6; yyNonterminal = 217; {
/* line 1049 "Breeze.lalr" */
 yySynAttribute.command.Tree = mProcedureCallCommonCommand (yyAttrStackPtr [3-1].position.Position, yyAttrStackPtr [4-1].Scan.breeze_ident.ident, ReverseTree (yyAttrStackPtr [5-1].procedure_params.Tree));
 ;

} break;
case 1100:
case 851: /* command : '(' 'labelled-command' position breeze_ident command ')' .*/
  yyStateStackPtr -= 6; yyAttrStackPtr -= 6; yyNonterminal = 217; {
/* line 1052 "Breeze.lalr" */
 yySynAttribute.command.Tree  =  yyAttrStackPtr [5-1].command.Tree;
 ;

} break;
case 1101:
case 854: /* command : '(' 'select' position channel_guards ')' .*/
  yyStateStackPtr -= 5; yyAttrStackPtr -= 5; yyNonterminal = 217; {
/* line 1055 "Breeze.lalr" */
 yySynAttribute.command.Tree = mSelectCommand (yyAttrStackPtr [3-1].position.Position, yyAttrStackPtr [4-1].channel_guards.Tree);
 ;

} break;
case 1102:
case 855: /* command : '(' 'select!' position channel_guards ')' .*/
  yyStateStackPtr -= 5; yyAttrStackPtr -= 5; yyNonterminal = 217; {
/* line 1058 "Breeze.lalr" */
 yySynAttribute.command.Tree = mSelectBangCommand (yyAttrStackPtr [3-1].position.Position, yyAttrStackPtr [4-1].channel_guards.Tree);
 ;

} break;
case 1103:
case 856: /* command : '(' 'arbitrate' position channel_guards ')' .*/
  yyStateStackPtr -= 5; yyAttrStackPtr -= 5; yyNonterminal = 217; {
/* line 1061 "Breeze.lalr" */
 yySynAttribute.command.Tree = mArbitrateCommand (yyAttrStackPtr [3-1].position.Position, yyAttrStackPtr [4-1].channel_guards.Tree);
 ;

} break;
case 1104:
case 857: /* command : '(' 'print' position expr exprs ')' .*/
  yyStateStackPtr -= 6; yyAttrStackPtr -= 6; yyNonterminal = 217; {
/* line 1064 "Breeze.lalr" */
 yySynAttribute.command.Tree = mPrintCommand (yyAttrStackPtr [3-1].position.Position, yyAttrStackPtr [4-1].expr.Tree, ReverseTree (yyAttrStackPtr [5-1].exprs.Tree));
 ;

} break;
case 1105:
case 858: /* command : '(' 'sink' position expr ')' .*/
  yyStateStackPtr -= 5; yyAttrStackPtr -= 5; yyNonterminal = 217; {
/* line 1067 "Breeze.lalr" */
 yySynAttribute.command.Tree = mSinkCommand (yyAttrStackPtr [3-1].position.Position, yyAttrStackPtr [4-1].expr.Tree);
 ;

} break;
case 1106:
case 849: /* procedure_params : '(' 'procedure-params' position procedure_params_body ')' .*/
  yyStateStackPtr -= 5; yyAttrStackPtr -= 5; yyNonterminal = 205; {
/* line 1070 "Breeze.lalr" */
 yySynAttribute.procedure_params.Tree  =  yyAttrStackPtr [4-1].procedure_params_body.Tree;
 ;

} break;
case 1107: /* procedure_params_body : .*/
  yyStateStackPtr -= 0; yyAttrStackPtr -= 0; yyNonterminal = 224; {
/* line 1073 "Breeze.lalr" */
 yySynAttribute.procedure_params_body.Tree = mNullProcParams (NoPosition);
 ;

} break;
case 1108:
case 921: /* procedure_params_body : procedure_params_body procedure_param .*/
  yyStateStackPtr -= 2; yyAttrStackPtr -= 2; yyNonterminal = 224; {
/* line 1076 "Breeze.lalr" */
 yyAttrStackPtr [2-1].procedure_param.Tree->ProcParam.next = yyAttrStackPtr [1-1].procedure_params_body.Tree;
	yySynAttribute.procedure_params_body.Tree = yyAttrStackPtr [2-1].procedure_param.Tree; ;
 ;

} break;
case 1109:
case 844: /* procedure_param : '(' 'expr-proc-param' position expr ')' .*/
  yyStateStackPtr -= 5; yyAttrStackPtr -= 5; yyNonterminal = 225; {
/* line 1080 "Breeze.lalr" */
 yySynAttribute.procedure_param.Tree = mExprProcParam (yyAttrStackPtr [3-1].position.Position, NoTree, mCoercedExpr (yyAttrStackPtr [4-1].expr.Tree));
 ;

} break;
case 1110:
case 845: /* procedure_param : '(' 'type-proc-param' position type ')' .*/
  yyStateStackPtr -= 5; yyAttrStackPtr -= 5; yyNonterminal = 225; {
/* line 1083 "Breeze.lalr" */
 yySynAttribute.procedure_param.Tree = mTypeProcParam (yyAttrStackPtr [3-1].position.Position, NoTree, yyAttrStackPtr [4-1].type.Tree);
 ;

} break;
case 1111:
case 846: /* procedure_param : '(' 'block-proc-param' position block ')' .*/
  yyStateStackPtr -= 5; yyAttrStackPtr -= 5; yyNonterminal = 225; {
/* line 1086 "Breeze.lalr" */
 yySynAttribute.procedure_param.Tree = mBlockProcParam (yyAttrStackPtr [3-1].position.Position, NoTree, yyAttrStackPtr [4-1].block.Tree);
 ;

} break;
case 1112:
case 847: /* procedure_param : '(' 'var-read-proc-param' position expr ')' .*/
  yyStateStackPtr -= 5; yyAttrStackPtr -= 5; yyNonterminal = 225; {
/* line 1089 "Breeze.lalr" */
 yySynAttribute.procedure_param.Tree = mVarReadProcParam (yyAttrStackPtr [3-1].position.Position, NoTree, mCoercedExpr (yyAttrStackPtr [4-1].expr.Tree));
 ;

} break;
case 1113:
case 848: /* procedure_param : '(' 'var-write-proc-param' position lvalue ')' .*/
  yyStateStackPtr -= 5; yyAttrStackPtr -= 5; yyNonterminal = 225; {
/* line 1092 "Breeze.lalr" */
 yySynAttribute.procedure_param.Tree = mVarWriteProcParam (yyAttrStackPtr [3-1].position.Position, NoTree, yyAttrStackPtr [4-1].lvalue.Tree);
 ;

} break;
case 1114:
case 751: /* function_params : '(' 'function-params' position function_params_body ')' .*/
  yyStateStackPtr -= 5; yyAttrStackPtr -= 5; yyNonterminal = 214; {
/* line 1095 "Breeze.lalr" */
 yySynAttribute.function_params.Tree  =  yyAttrStackPtr [4-1].function_params_body.Tree;
 ;

} break;
case 1115: /* function_params_body : .*/
  yyStateStackPtr -= 0; yyAttrStackPtr -= 0; yyNonterminal = 226; {
/* line 1098 "Breeze.lalr" */
 yySynAttribute.function_params_body.Tree = mNullFuncParams (NoPosition);
 ;

} break;
case 1116:
case 902: /* function_params_body : function_params_body function_param .*/
  yyStateStackPtr -= 2; yyAttrStackPtr -= 2; yyNonterminal = 226; {
/* line 1101 "Breeze.lalr" */
 yyAttrStackPtr [2-1].function_param.Tree->FuncParam.next = yyAttrStackPtr [1-1].function_params_body.Tree;
	yySynAttribute.function_params_body.Tree = yyAttrStackPtr [2-1].function_param.Tree; ;
 ;

} break;
case 1117:
case 749: /* function_param : '(' 'expr-func-param' position expr ')' .*/
  yyStateStackPtr -= 5; yyAttrStackPtr -= 5; yyNonterminal = 227; {
/* line 1105 "Breeze.lalr" */
 yySynAttribute.function_param.Tree = mExprFuncParam (yyAttrStackPtr [3-1].position.Position, NoTree, mCoercedExpr (yyAttrStackPtr [4-1].expr.Tree));
 ;

} break;
case 1118:
case 750: /* function_param : '(' 'type-func-param' position type ')' .*/
  yyStateStackPtr -= 5; yyAttrStackPtr -= 5; yyNonterminal = 227; {
/* line 1108 "Breeze.lalr" */
 yySynAttribute.function_param.Tree = mTypeFuncParam (yyAttrStackPtr [3-1].position.Position, NoTree, yyAttrStackPtr [4-1].type.Tree);
 ;

} break;
case 1119:
case 840: /* par_seq : 'sequential' .*/
  yyStateStackPtr -= 1; yyAttrStackPtr -= 1; yyNonterminal = 222; {
/* line 1111 "Breeze.lalr" */
 yySynAttribute.par_seq.isParallel = false;
 yySynAttribute.par_seq.isPermissive = false;
 ;

} break;
case 1120:
case 841: /* par_seq : 'parallel' .*/
  yyStateStackPtr -= 1; yyAttrStackPtr -= 1; yyNonterminal = 222; {
/* line 1115 "Breeze.lalr" */
 yySynAttribute.par_seq.isParallel = true;
 yySynAttribute.par_seq.isPermissive = false;
 ;

} break;
case 1121:
case 842: /* par_seq : 'permissive-parallel' .*/
  yyStateStackPtr -= 1; yyAttrStackPtr -= 1; yyNonterminal = 222; {
/* line 1119 "Breeze.lalr" */
 yySynAttribute.par_seq.isParallel = true;
 yySynAttribute.par_seq.isPermissive = true;
 ;

} break;
case 1122:
case 823: /* guard : '(' 'guard' position expr command ')' .*/
  yyStateStackPtr -= 6; yyAttrStackPtr -= 6; yyNonterminal = 228; {
/* line 1123 "Breeze.lalr" */
 yySynAttribute.guard.Tree = mGuard (yyAttrStackPtr [3-1].position.Position, NoTree, yyAttrStackPtr [4-1].expr.Tree, yyAttrStackPtr [5-1].command.Tree);
 ;

} break;
case 1123:
case 792: /* port_guard : '(' 'port-guard' position expr formal_ports ')' .*/
  yyStateStackPtr -= 6; yyAttrStackPtr -= 6; yyNonterminal = 229; {
/* line 1126 "Breeze.lalr" */
 yySynAttribute.port_guard.Tree = mPortGuard (yyAttrStackPtr [3-1].position.Position, NoTree, yyAttrStackPtr [4-1].expr.Tree, ReverseTree (yyAttrStackPtr [5-1].formal_ports.Tree));
 ;

} break;
case 1124:
case 867: /* decl_guard : '(' 'decl-guard' position expr decls ')' .*/
  yyStateStackPtr -= 6; yyAttrStackPtr -= 6; yyNonterminal = 230; {
/* line 1129 "Breeze.lalr" */
 yySynAttribute.decl_guard.Tree = mDeclGuard (yyAttrStackPtr [3-1].position.Position, NoTree, yyAttrStackPtr [4-1].expr.Tree, ReverseTree (yyAttrStackPtr [5-1].decls.Tree));
 ;

} break;
case 1125:
case 852: /* channel_guard : '(' 'channel-guard' position lvalues command ')' .*/
  yyStateStackPtr -= 6; yyAttrStackPtr -= 6; yyNonterminal = 231; {
/* line 1132 "Breeze.lalr" */
 yySynAttribute.channel_guard.Tree = mChannelGuard (yyAttrStackPtr [3-1].position.Position, NoTree, ReverseTree (yyAttrStackPtr [4-1].lvalues.Tree), yyAttrStackPtr [5-1].command.Tree);
 ;

} break;
case 1126:
case 835: /* case_guard : '(' 'case-match-guard' position case_matches command ')' .*/
  yyStateStackPtr -= 6; yyAttrStackPtr -= 6; yyNonterminal = 233; {
/* line 1135 "Breeze.lalr" */
 yySynAttribute.case_guard.Tree = mCaseMatchGuard (yyAttrStackPtr [3-1].position.Position, NoTree, yyAttrStackPtr [4-1].case_matches.Tree, yyAttrStackPtr [5-1].command.Tree);
 ;

} break;
case 1127:
case 836: /* case_guard : '(' 'for-case-guard' position breeze_ident case_matches command ')' .*/
  yyStateStackPtr -= 7; yyAttrStackPtr -= 7; yyNonterminal = 233; {
/* line 1138 "Breeze.lalr" */
 yySynAttribute.case_guard.Tree = mForCaseGuard (yyAttrStackPtr [3-1].position.Position, NoTree, yyAttrStackPtr [4-1].Scan.breeze_ident.ident, yyAttrStackPtr [5-1].case_matches.Tree, yyAttrStackPtr [6-1].command.Tree);
 ;

} break;
case 1128:
case 798: /* lvalue : '(' 'ident-lvalue' position breeze_ident ')' .*/
  yyStateStackPtr -= 5; yyAttrStackPtr -= 5; yyNonterminal = 218; {
/* line 1141 "Breeze.lalr" */
 yySynAttribute.lvalue.Tree = mIdentLvalue (yyAttrStackPtr [3-1].position.Position, yyAttrStackPtr [4-1].Scan.breeze_ident.ident);
 ;

} break;
case 1129:
case 799: /* lvalue : '(' 'record-elem-lvalue' position lvalue breeze_ident ')' .*/
  yyStateStackPtr -= 6; yyAttrStackPtr -= 6; yyNonterminal = 218; {
/* line 1144 "Breeze.lalr" */
 yySynAttribute.lvalue.Tree = mRecordElemLvalue (yyAttrStackPtr [3-1].position.Position, yyAttrStackPtr [4-1].lvalue.Tree, yyAttrStackPtr [5-1].Scan.breeze_ident.ident);
 ;

} break;
case 1130:
case 800: /* lvalue : '(' 'array-elem-lvalue' position lvalue expr ')' .*/
  yyStateStackPtr -= 6; yyAttrStackPtr -= 6; yyNonterminal = 218; {
/* line 1147 "Breeze.lalr" */
 yySynAttribute.lvalue.Tree = mArrayElemLvalue (yyAttrStackPtr [3-1].position.Position, yyAttrStackPtr [4-1].lvalue.Tree, mCoercedExpr (yyAttrStackPtr [5-1].expr.Tree));
 ;

} break;
case 1131:
case 801: /* lvalue : '(' 'array-slice-lvalue' position lvalue range ')' .*/
  yyStateStackPtr -= 6; yyAttrStackPtr -= 6; yyNonterminal = 218; {
/* line 1150 "Breeze.lalr" */
 yySynAttribute.lvalue.Tree = mArraySliceLvalue (yyAttrStackPtr [3-1].position.Position, yyAttrStackPtr [4-1].lvalue.Tree, yyAttrStackPtr [5-1].range.Tree);
 ;

} break;
case 1132:
case 802: /* lvalue : '(' 'array-append-lvalue' position lvalue lvalue ')' .*/
  yyStateStackPtr -= 6; yyAttrStackPtr -= 6; yyNonterminal = 218; {
/* line 1153 "Breeze.lalr" */
 yySynAttribute.lvalue.Tree = mArrayAppendLvalue (yyAttrStackPtr [3-1].position.Position, yyAttrStackPtr [4-1].lvalue.Tree, yyAttrStackPtr [5-1].lvalue.Tree);
 ;

} break;
case 1133:
case 807: /* lvalue : '(' 'array-cons-lvalue' position lvalues ')' .*/
  yyStateStackPtr -= 5; yyAttrStackPtr -= 5; yyNonterminal = 218; {
/* line 1156 "Breeze.lalr" */
 yySynAttribute.lvalue.Tree = mArrayConsLvalue (yyAttrStackPtr [3-1].position.Position, ReverseTree (yyAttrStackPtr [4-1].lvalues.Tree));
 ;

} break;
case 1134:
case 808: /* lvalue : '(' 'as-lvalue' position lvalue type ')' .*/
  yyStateStackPtr -= 6; yyAttrStackPtr -= 6; yyNonterminal = 218; {
/* line 1159 "Breeze.lalr" */
 yySynAttribute.lvalue.Tree = mAsLvalue (yyAttrStackPtr [3-1].position.Position, yyAttrStackPtr [4-1].lvalue.Tree, yyAttrStackPtr [5-1].type.Tree);
 ;

} break;
case 1135:
case 809: /* lvalue : '(' 'bit-array-cast-lvalue' position lvalue ')' .*/
  yyStateStackPtr -= 5; yyAttrStackPtr -= 5; yyNonterminal = 218; {
/* line 1162 "Breeze.lalr" */
 yySynAttribute.lvalue.Tree = mBitArrayCastLvalue (yyAttrStackPtr [3-1].position.Position, yyAttrStackPtr [4-1].lvalue.Tree);
 ;

} break;
case 1136:
case 797: /* decls : '(' 'decls' position decls_body ')' .*/
  yyStateStackPtr -= 5; yyAttrStackPtr -= 5; yyNonterminal = 207; {
/* line 1165 "Breeze.lalr" */
 yySynAttribute.decls.Tree  =  yyAttrStackPtr [4-1].decls_body.Tree;
 ;

} break;
case 1137: /* decls_body : .*/
  yyStateStackPtr -= 0; yyAttrStackPtr -= 0; yyNonterminal = 234; {
/* line 1168 "Breeze.lalr" */
 yySynAttribute.decls_body.Tree = mNullDecls (NoPosition);
 ;

} break;
case 1138:
case 912: /* decls_body : decls_body decl .*/
  yyStateStackPtr -= 2; yyAttrStackPtr -= 2; yyNonterminal = 234; {
/* line 1171 "Breeze.lalr" */
 yyAttrStackPtr [2-1].decl.Tree->Decl.next = yyAttrStackPtr [1-1].decls_body.Tree; yySynAttribute.decls_body.Tree = yyAttrStackPtr [2-1].decl.Tree; ;
 ;

} break;
case 1139:
case 757: /* idents : '(' 'idents' position idents_body ')' .*/
  yyStateStackPtr -= 5; yyAttrStackPtr -= 5; yyNonterminal = 200; {
/* line 1174 "Breeze.lalr" */
 yySynAttribute.idents.Tree  =  yyAttrStackPtr [4-1].idents_body.Tree;
 ;

} break;
case 1140:
case 904: /* idents_body : ident .*/
  yyStateStackPtr -= 1; yyAttrStackPtr -= 1; yyNonterminal = 235; {
/* line 1177 "Breeze.lalr" */
 yyAttrStackPtr [1-1].ident.Tree->Ident.next = mNullIdents (NoPosition); yySynAttribute.idents_body.Tree = yyAttrStackPtr [1-1].ident.Tree;;

} break;
case 1141:
case 903: /* idents_body : idents_body ident .*/
  yyStateStackPtr -= 2; yyAttrStackPtr -= 2; yyNonterminal = 235; {
/* line 1179 "Breeze.lalr" */
 yyAttrStackPtr [2-1].ident.Tree->Ident.next = yyAttrStackPtr [1-1].idents_body.Tree; yySynAttribute.idents_body.Tree = yyAttrStackPtr [2-1].ident.Tree; ;
 ;

} break;
case 1142:
case 731: /* exprs : '(' 'exprs' position exprs_body ')' .*/
  yyStateStackPtr -= 5; yyAttrStackPtr -= 5; yyNonterminal = 208; {
/* line 1182 "Breeze.lalr" */
 yySynAttribute.exprs.Tree  =  yyAttrStackPtr [4-1].exprs_body.Tree;
 ;

} break;
case 1143: /* exprs_body : .*/
  yyStateStackPtr -= 0; yyAttrStackPtr -= 0; yyNonterminal = 237; {
/* line 1185 "Breeze.lalr" */
 yySynAttribute.exprs_body.Tree = mNullExprLists (NoPosition);
 ;

} break;
case 1144:
case 900: /* exprs_body : exprs_body linked_expr .*/
  yyStateStackPtr -= 2; yyAttrStackPtr -= 2; yyNonterminal = 237; {
/* line 1188 "Breeze.lalr" */
 yyAttrStackPtr [2-1].linked_expr.Tree->ExprList.next = yyAttrStackPtr [1-1].exprs_body.Tree; yySynAttribute.exprs_body.Tree = yyAttrStackPtr [2-1].linked_expr.Tree; ;
 ;

} break;
case 1145:
case 806: /* lvalues : '(' 'lvalues' position lvalues_body ')' .*/
  yyStateStackPtr -= 5; yyAttrStackPtr -= 5; yyNonterminal = 219; {
/* line 1191 "Breeze.lalr" */
 yySynAttribute.lvalues.Tree  =  yyAttrStackPtr [4-1].lvalues_body.Tree;
 ;

} break;
case 1146:
case 914: /* lvalues_body : linked_lvalue .*/
  yyStateStackPtr -= 1; yyAttrStackPtr -= 1; yyNonterminal = 239; {
/* line 1194 "Breeze.lalr" */
 yyAttrStackPtr [1-1].linked_lvalue.Tree->LvalueList.next =
	mNullLvalueLists (NoPosition); yySynAttribute.lvalues_body.Tree = yyAttrStackPtr [1-1].linked_lvalue.Tree; ;
 ;

} break;
case 1147:
case 913: /* lvalues_body : lvalues_body linked_lvalue .*/
  yyStateStackPtr -= 2; yyAttrStackPtr -= 2; yyNonterminal = 239; {
/* line 1198 "Breeze.lalr" */
 yyAttrStackPtr [2-1].linked_lvalue.Tree->LvalueList.next = yyAttrStackPtr [1-1].lvalues_body.Tree;
	yySynAttribute.lvalues_body.Tree = yyAttrStackPtr [2-1].linked_lvalue.Tree; ;
 ;

} break;
case 1148:
case 832: /* case_match_elem : '(' 'case-range' position range ')' .*/
  yyStateStackPtr -= 5; yyAttrStackPtr -= 5; yyNonterminal = 241; {
/* line 1202 "Breeze.lalr" */
 yySynAttribute.case_match_elem.Tree = mCaseRange (yyAttrStackPtr [3-1].position.Position, NoTree, yyAttrStackPtr [4-1].range.Tree);
 ;

} break;
case 1149:
case 833: /* case_match_elem : '(' 'case-implicant' position expr ')' .*/
  yyStateStackPtr -= 5; yyAttrStackPtr -= 5; yyNonterminal = 241; {
/* line 1205 "Breeze.lalr" */
 yySynAttribute.case_match_elem.Tree = mCaseImplicant (yyAttrStackPtr [3-1].position.Position, NoTree, yyAttrStackPtr [4-1].expr.Tree) ;
 ;

} break;
case 1150:
case 834: /* case_matches : '(' 'case-matches' position case_matches_body ')' .*/
  yyStateStackPtr -= 5; yyAttrStackPtr -= 5; yyNonterminal = 232; {
/* line 1208 "Breeze.lalr" */
 yySynAttribute.case_matches.Tree  =  yyAttrStackPtr [4-1].case_matches_body.Tree;
 ;

} break;
case 1151:
case 917: /* case_matches_body : case_match_elem .*/
  yyStateStackPtr -= 1; yyAttrStackPtr -= 1; yyNonterminal = 242; {
/* line 1211 "Breeze.lalr" */
 yyAttrStackPtr [1-1].case_match_elem.Tree->CaseMatch.next =
	mNullCaseMatchs (NoPosition); yySynAttribute.case_matches_body.Tree = yyAttrStackPtr [1-1].case_match_elem.Tree; ;
 ;

} break;
case 1152:
case 918: /* case_matches_body : case_matches_body case_match_elem .*/
  yyStateStackPtr -= 2; yyAttrStackPtr -= 2; yyNonterminal = 242; {
/* line 1215 "Breeze.lalr" */
 yyAttrStackPtr [2-1].case_match_elem.Tree->CaseRange.next = yyAttrStackPtr [1-1].case_matches_body.Tree;
	yySynAttribute.case_matches_body.Tree = yyAttrStackPtr [2-1].case_match_elem.Tree; ;
 ;

} break;
case 1153:
case 796: /* formal_ports : '(' 'formal-ports' position formal_ports_body ')' .*/
  yyStateStackPtr -= 5; yyAttrStackPtr -= 5; yyNonterminal = 203; {
/* line 1219 "Breeze.lalr" */
 yySynAttribute.formal_ports.Tree  =  yyAttrStackPtr [4-1].formal_ports_body.Tree;
 ;

} break;
case 1154: /* formal_ports_body : .*/
  yyStateStackPtr -= 0; yyAttrStackPtr -= 0; yyNonterminal = 243; {
/* line 1222 "Breeze.lalr" */
 yySynAttribute.formal_ports_body.Tree = mNullFormalPorts (NoPosition);
 ;

} break;
case 1155:
case 911: /* formal_ports_body : formal_ports_body port .*/
  yyStateStackPtr -= 2; yyAttrStackPtr -= 2; yyNonterminal = 243; {
/* line 1225 "Breeze.lalr" */
 yyAttrStackPtr [2-1].port.Tree->FormalPort.next = yyAttrStackPtr [1-1].formal_ports_body.Tree; yySynAttribute.formal_ports_body.Tree = yyAttrStackPtr [2-1].port.Tree; ;
 ;

} break;
case 1156:
case 824: /* guards : '(' 'guards' position guards_body ')' .*/
  yyStateStackPtr -= 5; yyAttrStackPtr -= 5; yyNonterminal = 220; {
/* line 1228 "Breeze.lalr" */
 yySynAttribute.guards.Tree  =  yyAttrStackPtr [4-1].guards_body.Tree;
 ;

} break;
case 1157:
case 915: /* guards_body : guard .*/
  yyStateStackPtr -= 1; yyAttrStackPtr -= 1; yyNonterminal = 244; {
/* line 1231 "Breeze.lalr" */
 yyAttrStackPtr [1-1].guard.Tree->Guard.next = mNullGuards (NoPosition); yySynAttribute.guards_body.Tree = yyAttrStackPtr [1-1].guard.Tree; ;
 ;

} break;
case 1158:
case 916: /* guards_body : guards_body guard .*/
  yyStateStackPtr -= 2; yyAttrStackPtr -= 2; yyNonterminal = 244; {
/* line 1234 "Breeze.lalr" */
 yyAttrStackPtr [2-1].guard.Tree->Guard.next = yyAttrStackPtr [1-1].guards_body.Tree; yySynAttribute.guards_body.Tree = yyAttrStackPtr [2-1].guard.Tree; ;
 ;

} break;
case 1159:
case 793: /* port_guards : '(' 'port-guards' position port_guards_body ')' .*/
  yyStateStackPtr -= 5; yyAttrStackPtr -= 5; yyNonterminal = 216; {
/* line 1237 "Breeze.lalr" */
 yySynAttribute.port_guards.Tree  =  yyAttrStackPtr [4-1].port_guards_body.Tree;
 ;

} break;
case 1160:
case 909: /* port_guards_body : port_guard .*/
  yyStateStackPtr -= 1; yyAttrStackPtr -= 1; yyNonterminal = 245; {
/* line 1240 "Breeze.lalr" */
 yyAttrStackPtr [1-1].port_guard.Tree->PortGuard.next = mNullPortGuards (NoPosition); yySynAttribute.port_guards_body.Tree = yyAttrStackPtr [1-1].port_guard.Tree; ;
 ;

} break;
case 1161:
case 910: /* port_guards_body : port_guards_body port_guard .*/
  yyStateStackPtr -= 2; yyAttrStackPtr -= 2; yyNonterminal = 245; {
/* line 1243 "Breeze.lalr" */
 yyAttrStackPtr [2-1].port_guard.Tree->PortGuard.next = yyAttrStackPtr [1-1].port_guards_body.Tree; yySynAttribute.port_guards_body.Tree = yyAttrStackPtr [2-1].port_guard.Tree; ;
 ;

} break;
case 1162:
case 868: /* decl_guards : '(' 'decl-guards' position decl_guards_body ')' .*/
  yyStateStackPtr -= 5; yyAttrStackPtr -= 5; yyNonterminal = 206; {
/* line 1246 "Breeze.lalr" */
 yySynAttribute.decl_guards.Tree  =  yyAttrStackPtr [4-1].decl_guards_body.Tree;
 ;

} break;
case 1163:
case 924: /* decl_guards_body : decl_guard .*/
  yyStateStackPtr -= 1; yyAttrStackPtr -= 1; yyNonterminal = 246; {
/* line 1249 "Breeze.lalr" */
 yyAttrStackPtr [1-1].decl_guard.Tree->DeclGuard.next = mNullDeclGuards (NoPosition); yySynAttribute.decl_guards_body.Tree = yyAttrStackPtr [1-1].decl_guard.Tree; ;
 ;

} break;
case 1164:
case 925: /* decl_guards_body : decl_guards_body decl_guard .*/
  yyStateStackPtr -= 2; yyAttrStackPtr -= 2; yyNonterminal = 246; {
/* line 1252 "Breeze.lalr" */
 yyAttrStackPtr [2-1].decl_guard.Tree->DeclGuard.next = yyAttrStackPtr [1-1].decl_guards_body.Tree; yySynAttribute.decl_guards_body.Tree = yyAttrStackPtr [2-1].decl_guard.Tree; ;
 ;

} break;
case 1165:
case 837: /* case_guards : '(' 'case-guards' position case_guards_body ')' .*/
  yyStateStackPtr -= 5; yyAttrStackPtr -= 5; yyNonterminal = 221; {
/* line 1255 "Breeze.lalr" */
 yySynAttribute.case_guards.Tree  =  yyAttrStackPtr [4-1].case_guards_body.Tree;
 ;

} break;
case 1166:
case 919: /* case_guards_body : case_guard .*/
  yyStateStackPtr -= 1; yyAttrStackPtr -= 1; yyNonterminal = 247; {
/* line 1258 "Breeze.lalr" */
 yyAttrStackPtr [1-1].case_guard.Tree->CaseGuard.next = mNullCaseGuards (NoPosition); yySynAttribute.case_guards_body.Tree = yyAttrStackPtr [1-1].case_guard.Tree; ;
 ;

} break;
case 1167:
case 920: /* case_guards_body : case_guards_body case_guard .*/
  yyStateStackPtr -= 2; yyAttrStackPtr -= 2; yyNonterminal = 247; {
/* line 1261 "Breeze.lalr" */
 yyAttrStackPtr [2-1].case_guard.Tree->CaseGuard.next = yyAttrStackPtr [1-1].case_guards_body.Tree; yySynAttribute.case_guards_body.Tree = yyAttrStackPtr [2-1].case_guard.Tree; ;
 ;

} break;
case 1168:
case 853: /* channel_guards : '(' 'channel-guards' position channel_guards_body ')' .*/
  yyStateStackPtr -= 5; yyAttrStackPtr -= 5; yyNonterminal = 223; {
/* line 1264 "Breeze.lalr" */
 yySynAttribute.channel_guards.Tree  =  yyAttrStackPtr [4-1].channel_guards_body.Tree;
 ;

} break;
case 1169:
case 922: /* channel_guards_body : channel_guard .*/
  yyStateStackPtr -= 1; yyAttrStackPtr -= 1; yyNonterminal = 248; {
/* line 1267 "Breeze.lalr" */
 yyAttrStackPtr [1-1].channel_guard.Tree->ChannelGuard.next = mNullChannelGuards (NoPosition);
	yySynAttribute.channel_guards_body.Tree = yyAttrStackPtr [1-1].channel_guard.Tree; ;
 ;

} break;
case 1170:
case 923: /* channel_guards_body : channel_guards_body channel_guard .*/
  yyStateStackPtr -= 2; yyAttrStackPtr -= 2; yyNonterminal = 248; {
/* line 1271 "Breeze.lalr" */
 yyAttrStackPtr [2-1].channel_guard.Tree->ChannelGuard.next = yyAttrStackPtr [1-1].channel_guards_body.Tree;
	yySynAttribute.channel_guards_body.Tree = yyAttrStackPtr [2-1].channel_guard.Tree; ;
 ;

} break;
case 1171:
case 759: /* record_elems : '(' 'record-elems' position record_elems_body ')' .*/
  yyStateStackPtr -= 5; yyAttrStackPtr -= 5; yyNonterminal = 209; {
/* line 1275 "Breeze.lalr" */
 yySynAttribute.record_elems.Tree  =  yyAttrStackPtr [4-1].record_elems_body.Tree;
 ;

} break;
case 1172:
case 906: /* record_elems_body : record_elem .*/
  yyStateStackPtr -= 1; yyAttrStackPtr -= 1; yyNonterminal = 249; {
/* line 1278 "Breeze.lalr" */
 yyAttrStackPtr [1-1].record_elem.Tree->RecordElem.next = mNullRecordElems (NoPosition);
	yySynAttribute.record_elems_body.Tree = yyAttrStackPtr [1-1].record_elem.Tree; ;
 ;

} break;
case 1173:
case 905: /* record_elems_body : record_elems_body record_elem .*/
  yyStateStackPtr -= 2; yyAttrStackPtr -= 2; yyNonterminal = 249; {
/* line 1282 "Breeze.lalr" */
 yyAttrStackPtr [2-1].record_elem.Tree->RecordElem.next = yyAttrStackPtr [1-1].record_elems_body.Tree;
	yySynAttribute.record_elems_body.Tree = yyAttrStackPtr [2-1].record_elem.Tree; ;
 ;

} break;
case 1174:
case 763: /* enum_elems : '(' 'enum-elems' position enum_elems_body ')' .*/
  yyStateStackPtr -= 5; yyAttrStackPtr -= 5; yyNonterminal = 210; {
/* line 1286 "Breeze.lalr" */
 yySynAttribute.enum_elems.Tree  =  yyAttrStackPtr [4-1].enum_elems_body.Tree;
 ;

} break;
case 1175:
case 908: /* enum_elems_body : enum_elem .*/
  yyStateStackPtr -= 1; yyAttrStackPtr -= 1; yyNonterminal = 251; {
/* line 1289 "Breeze.lalr" */
 yyAttrStackPtr [1-1].enum_elem.Tree->EnumElem.next = mNullEnumElems (NoPosition); yySynAttribute.enum_elems_body.Tree = yyAttrStackPtr [1-1].enum_elem.Tree; ;
 ;

} break;
case 1176:
case 907: /* enum_elems_body : enum_elems_body enum_elem .*/
  yyStateStackPtr -= 2; yyAttrStackPtr -= 2; yyNonterminal = 251; {
/* line 1292 "Breeze.lalr" */
 yyAttrStackPtr [2-1].enum_elem.Tree->EnumElem.next = yyAttrStackPtr [1-1].enum_elems_body.Tree; yySynAttribute.enum_elems_body.Tree = yyAttrStackPtr [2-1].enum_elem.Tree; ;
 ;

} break;
case 1177:
case 761: /* enum_elem : '(' 'enum-elem' position breeze_ident ')' .*/
  yyStateStackPtr -= 5; yyAttrStackPtr -= 5; yyNonterminal = 252; {
/* line 1295 "Breeze.lalr" */
 yySynAttribute.enum_elem.Tree = mDefaultValuedEnumElem (yyAttrStackPtr [4-1].Scan.Position, NoTree, yyAttrStackPtr [4-1].Scan.breeze_ident.ident);
 ;

} break;
case 1178:
case 762: /* enum_elem : '(' 'valued-enum-elem' position breeze_ident expr ')' .*/
  yyStateStackPtr -= 6; yyAttrStackPtr -= 6; yyNonterminal = 252; {
/* line 1298 "Breeze.lalr" */
 yySynAttribute.enum_elem.Tree = mValuedEnumElem (yyAttrStackPtr [4-1].Scan.Position, NoTree, yyAttrStackPtr [4-1].Scan.breeze_ident.ident,
	mCoercedExpr (yyAttrStackPtr [5-1].expr.Tree));
 ;

} break;
case 1179:
case 758: /* record_elem : '(' 'record-elem' position idents type ')' .*/
  yyStateStackPtr -= 6; yyAttrStackPtr -= 6; yyNonterminal = 250; {
/* line 1302 "Breeze.lalr" */
 yySynAttribute.record_elem.Tree = mRecordElem (yyAttrStackPtr [3-1].position.Position, NoTree, ReverseTree (yyAttrStackPtr [4-1].idents.Tree), yyAttrStackPtr [5-1].type.Tree);
 ;

} break;
case 1180:
case 756: /* ident : breeze_ident .*/
  yyStateStackPtr -= 1; yyAttrStackPtr -= 1; yyNonterminal = 236; {
/* line 1305 "Breeze.lalr" */
 yySynAttribute.ident.Tree = mIdent (yyAttrStackPtr [1-1].Scan.Position, NoTree, yyAttrStackPtr [1-1].Scan.breeze_ident.ident);
 ;

} break;
case 1181:
case 899: /* linked_expr : expr .*/
  yyStateStackPtr -= 1; yyAttrStackPtr -= 1; yyNonterminal = 238; {
/* line 1308 "Breeze.lalr" */
 yySynAttribute.linked_expr.Tree = mExprList (NoPosition, NoTree, mCoercedExpr (yyAttrStackPtr [1-1].expr.Tree));
 ;

} break;
case 1182:
case 803: /* linked_lvalue : '(' 'block-lvalue' position block ')' .*/
  yyStateStackPtr -= 5; yyAttrStackPtr -= 5; yyNonterminal = 240; {
/* line 1311 "Breeze.lalr" */
 yySynAttribute.linked_lvalue.Tree = mLinkedBlock (yyAttrStackPtr [3-1].position.Position, NoTree, yyAttrStackPtr [4-1].block.Tree);
 ;

} break;
case 1183:
case 804: /* linked_lvalue : '(' 'channel-lvalue' position lvalue ')' .*/
  yyStateStackPtr -= 5; yyAttrStackPtr -= 5; yyNonterminal = 240; {
/* line 1314 "Breeze.lalr" */
 yySynAttribute.linked_lvalue.Tree = mLinkedChannel (yyAttrStackPtr [3-1].position.Position, NoTree, NoIdent, yyAttrStackPtr [4-1].lvalue.Tree);
 ;

} break;
case 1184:
case 805: /* linked_lvalue : '(' 'renamed-channel-lvalue' position lvalue breeze_ident ')' .*/
  yyStateStackPtr -= 6; yyAttrStackPtr -= 6; yyNonterminal = 240; {
/* line 1317 "Breeze.lalr" */
 yySynAttribute.linked_lvalue.Tree = mLinkedChannel (yyAttrStackPtr [3-1].position.Position, NoTree, yyAttrStackPtr [5-1].Scan.breeze_ident.ident, yyAttrStackPtr [4-1].lvalue.Tree);
 ;

} break;
}

	       /* SPEC State = Next (Top (), Nonterminal); nonterminal transition */
	       yyState = * (yyNBasePtr [* yyStateStackPtr ++] + yyNonterminal);
	       * yyAttrStackPtr ++ = yySynAttribute;
	       if (yyState < yyFirstFinalState) goto ParseLoop;	/* read reduce ? */
	    } 

	 } else {					/* read */
	    yyStateStackPtr ++;
	    yyGetAttribute (yyAttrStackPtr ++, BreezeScan_Attribute);
	    yyTerminal = BreezeScan_GetToken ();
	    yyIsRepairing = false;
	 }
      }
   }

static void yyErrorRecovery
# if defined __STDC__ | defined __cplusplus
   (yySymbolRange * yyTerminal, yyStateRange * yyStateStack, unsigned long yyStackSize, short yyStackPtr)
# else
   (yyTerminal, yyStateStack, yyStackSize, yyStackPtr)
   yySymbolRange *	yyTerminal	;
   yyStateRange *	yyStateStack	;
   unsigned long	yyStackSize	;
   short		yyStackPtr	;
# endif
   {
      bool	yyTokensSkipped	;
      tSet	yyContinueSet	;
      tSet	yyRestartSet	;
      int	yyLength = 0	;
      char	yyContinueString [256];

   /* 1. report an error */
      ErrorMessage (xxSyntaxError, xxError, BreezeScan_Attribute.Position);

   /* 2. report the set of expected terminal symbols */
      MakeSet (& yyContinueSet, (short) yyLastTerminal);
      yyComputeContinuation (yyStateStack, yyStackSize, yyStackPtr, & yyContinueSet);
      yyContinueString [0] = '\0';
      while (! IsEmpty (& yyContinueSet)) {
	 char * yyTokenString = Breeze_TokenName [Extract (& yyContinueSet)];
	 if ((yyLength += strlen (yyTokenString) + 1) >= 256) break;
	 (void) strcat (yyContinueString, yyTokenString);
	 (void) strcat (yyContinueString, " ");
      }
      ErrorMessageI (xxExpectedTokens, xxInformation, BreezeScan_Attribute.Position,
	 xxString, yyContinueString);
      ReleaseSet (& yyContinueSet);

   /* 3. compute the set of terminal symbols for restart of the parse */
      MakeSet (& yyRestartSet, (short) yyLastTerminal);
      yyComputeRestartPoints (yyStateStack, yyStackSize, yyStackPtr, & yyRestartSet);

   /* 4. skip terminal symbols until a restart point is reached */
      yyTokensSkipped = false;
      while (! IsElement (* yyTerminal, & yyRestartSet)) {
	 * yyTerminal = BreezeScan_GetToken ();
	 yyTokensSkipped = true;
      }
      ReleaseSet (& yyRestartSet);

   /* 5. report the restart point */
      if (yyTokensSkipped) {
	 ErrorMessage (xxRestartPoint, xxInformation, BreezeScan_Attribute.Position);
      }
   }

/*
   compute the set of terminal symbols that can be accepted (read)
   in a given stack configuration (eventually after reduce actions)
*/

static void yyComputeContinuation
# if defined __STDC__ | defined __cplusplus
   (yyStateRange * yyStack, unsigned long yyStackSize, short yyStackPtr, tSet * yyContinueSet)
# else
   (yyStack, yyStackSize, yyStackPtr, yyContinueSet)
   yyStateRange *	yyStack		;
   unsigned long	yyStackSize	;
   short		yyStackPtr	;
   tSet *		yyContinueSet	;
# endif
   {
      register yySymbolRange	yyTerminal;
      register yyStateRange	yyState = yyStack [yyStackPtr];

      AssignEmpty (yyContinueSet);
      for (yyTerminal = yyFirstTerminal; yyTerminal <= yyLastTerminal; yyTerminal ++) {
	 if (yyNext (yyState, yyTerminal) != yyNoState &&
	    yyIsContinuation (yyTerminal, yyStack, yyStackSize, yyStackPtr)) {
	    Include (yyContinueSet, (short) yyTerminal);
	 }
      }
   }

/*
   check whether a given terminal symbol can be accepted (read)
   in a certain stack configuration (eventually after reduce actions)
*/

static bool yyIsContinuation
# if defined __STDC__ | defined __cplusplus
   (yySymbolRange yyTerminal, yyStateRange * yyStateStack, unsigned long yyStackSize, short yyStackPtr)
# else
   (yyTerminal, yyStateStack, yyStackSize, yyStackPtr)
   yySymbolRange	yyTerminal	;
   yyStateRange *	yyStateStack	;
   unsigned long	yyStackSize	;
   short		yyStackPtr	;
# endif
   {
      register yyStateRange	yState		;
      register yySymbolRange	yyNonterminal	;
	       yyStateRange *	yyStack		;
   
      MakeArray ((char * *) & yyStack, & yyStackSize, sizeof (yyStateRange));	/* pass Stack by value */
# ifdef BCOPY
      bcopy ((char *) yyStateStack, (char *) yyStack, (int) sizeof (yyStateRange) * (yyStackPtr + 1));
# else
      (void) memcpy ((char *) yyStack, (char *) yyStateStack, (int) sizeof (yyStateRange) * (yyStackPtr + 1));
# endif

      yState = yyStack [yyStackPtr];
      for (;;) {
	 yyStack [yyStackPtr] = yState;
	 yState = yyNext (yState, yyTerminal);
	 if (yState == yyNoState) {
	    ReleaseArray ((char * *) & yyStack, & yyStackSize, sizeof (yyStateRange));
	    return false;
	 }
	 if (yState <= yyLastReadReduceState) {		/* read or read reduce ? */
	    ReleaseArray ((char * *) & yyStack, & yyStackSize, sizeof (yyStateRange));
	    return true;
	 }

	 for (;;) {					/* reduce */
	    if (yState == yyStopState) {
	       ReleaseArray ((char * *) & yyStack, & yyStackSize, sizeof (yyStateRange));
	       return true;
	    } else { 
	       yyStackPtr -= yyLength [yState - yyFirstReduceState];
	       yyNonterminal = yyLeftHandSide [yState - yyFirstReduceState];
	    }

	    yState = yyNext (yyStack [yyStackPtr], yyNonterminal);
	    if (yyStackPtr >= yyStackSize) {
	       ExtendArray ((char * *) & yyStack, & yyStackSize, sizeof (yyStateRange));
	    }
	    yyStackPtr ++;
	    if (yState < yyFirstFinalState) break;	/* read nonterminal ? */
	    yState = yyFinalToProd [yState - yyFirstReadReduceState];	/* read reduce */
	 }
      }
   }

/*
   compute a set of terminal symbols that can be used to restart
   parsing in a given stack configuration. we simulate parsing until
   end of file using a suffix program synthesized by the function
   Continuation. All symbols acceptable in the states reached during
   the simulation can be used to restart parsing.
*/

static void yyComputeRestartPoints
# if defined __STDC__ | defined __cplusplus
   (yyStateRange * yyStateStack, unsigned long yyStackSize, short yyStackPtr, tSet * yyRestartSet)
# else
   (yyStateStack, yyStackSize, yyStackPtr, yyRestartSet)
   yyStateRange *	yyStateStack	;
   unsigned long	yyStackSize	;
   short		yyStackPtr	;
   tSet *		yyRestartSet	;
# endif
   {
      register yyStateRange	yState		;
      register yySymbolRange	yyNonterminal	;
	       yyStateRange *	yyStack		;
	       tSet		yyContinueSet	;
   
      MakeArray ((char * *) & yyStack, & yyStackSize, sizeof (yyStateRange)); /* pass Stack by value */
# ifdef BCOPY
      bcopy ((char *) yyStateStack, (char *) yyStack, (int) sizeof (yyStateRange) * (yyStackPtr + 1));
# else
      (void) memcpy ((char *) yyStack, (char *) yyStateStack, (int) sizeof (yyStateRange) * (yyStackPtr + 1));
# endif

      MakeSet (& yyContinueSet, (short) yyLastTerminal);
      AssignEmpty (yyRestartSet);
      yState = yyStack [yyStackPtr];

      for (;;) {
	 if (yyStackPtr >= yyStackSize) {
	    ExtendArray ((char * *) & yyStack, & yyStackSize, sizeof (yyStateRange));
	 }
	 yyStack [yyStackPtr] = yState;
	 yyComputeContinuation (yyStack, yyStackSize, yyStackPtr, & yyContinueSet);
	 Union (yyRestartSet, & yyContinueSet);
	 yState = yyNext (yState, yyContinuation [yState]);

	 if (yState >= yyFirstFinalState) {		/* final state ? */
	    if (yState <= yyLastReadReduceState) {	/* read reduce ? */
	       yyStackPtr ++;
	       yState = yyFinalToProd [yState - yyFirstReadReduceState];
	    }

	    for (;;) {					/* reduce */
	       if (yState == yyStopState) {
		  ReleaseSet (& yyContinueSet);
		  ReleaseArray ((char * *) & yyStack, & yyStackSize, sizeof (yyStateRange));
		  return;
	       } else { 
		  yyStackPtr -= yyLength [yState - yyFirstReduceState];
		  yyNonterminal = yyLeftHandSide [yState - yyFirstReduceState];
	       }

	       yState = yyNext (yyStack [yyStackPtr], yyNonterminal);
	       yyStackPtr ++;
	       if (yState < yyFirstFinalState) break;	/* read nonterminal ? */
	       yState = yyFinalToProd [yState - yyFirstReadReduceState]; /* read reduce */
	    }
	 } else {					/* read */
	    yyStackPtr ++;
	 }
      }
   }

/* access the parse table:   Next : State x Symbol -> Action */

static yyStateRange yyNext
# if defined __STDC__ | defined __cplusplus
   (yyStateRange yyState, yySymbolRange yySymbol)
# else
   (yyState, yySymbol) yyStateRange yyState; yySymbolRange yySymbol;
# endif
   {
      register yyTCombType * yyTCombPtr;

      if (yySymbol <= yyLastTerminal) {
	 for (;;) {
	    yyTCombPtr = yyTBasePtr [yyState] + yySymbol;
	    if (yyTCombPtr->Check != yyState) {
	       if ((yyState = yyDefault [yyState]) == yyNoState) return yyNoState;
	    } else {
	       return yyTCombPtr->Next;
	    }
	 }
      } else {
	return * (yyNBasePtr [yyState] + yySymbol);
      }
   }

static void BeginBreeze ()
   {
/* line 195 "Breeze.lalr" */


	BreezeScan_BeginScanner();
	nameIdent = MakeIdent1 ("name");


   }

void CloseBreeze ()
   {
/* line 202 "Breeze.lalr" */


   }
