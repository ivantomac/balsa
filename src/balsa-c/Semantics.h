# ifndef yySemantics
# define yySemantics

# if defined __STDC__ | defined __cplusplus
# define ARGS(parameters)	parameters
# else
# define ARGS(parameters)	()
# endif

# include "Tree.h"


extern void Semantics ARGS((tTree yyt));
extern void BeginSemantics ();
extern void CloseSemantics ();

# endif
