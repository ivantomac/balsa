# ifndef yyBalsa
# define yyBalsa

/* $Id: Parser.h,v 2.1 1992/08/07 15:28:42 grosch rel $ */

/* line 143 "Balsa.lalr" */



# ifdef yacc_interface
# define Balsa			yyparse
# define yyInitStackSize	YYMAXDEPTH
# endif

extern	char *	Balsa_TokenName [];

extern	int	Balsa	();
extern	void	CloseBalsa	();

# endif
