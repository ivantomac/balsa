# ifndef yyBreeze
# define yyBreeze

/* $Id: Parser.h,v 2.1 1992/08/07 15:28:42 grosch rel $ */

/* line 189 "Breeze.lalr" */



# ifdef yacc_interface
# define Breeze			yyparse
# define yyInitStackSize	YYMAXDEPTH
# endif

extern	char *	Breeze_TokenName [];

extern	int	Breeze	();
extern	void	CloseBreeze	();

# endif
