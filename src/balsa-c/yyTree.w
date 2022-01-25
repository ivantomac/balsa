# ifndef beginint
# define beginint(a)		
# endif
# ifndef closeint
# define closeint(a)		
# endif
# ifndef readint
# define readint(a)		(void) fscanf (yyf, "%d", & a);
# endif
# ifndef writeint
# define writeint(a)		(void) fprintf (yyf, "%d", a);
# endif
# ifndef getint
# define getint(a)		yyGet ((char *) & a, sizeof (a));
# endif
# ifndef putint
# define putint(a)		yyPut ((char *) & a, sizeof (a));
# endif
# ifndef copyint
# define copyint(a, b)		
# endif
# ifndef equalint
# define equalint(a, b)		a == b
# endif
# ifndef beginunsigned
# define beginunsigned(a)	
# endif
# ifndef closeunsigned
# define closeunsigned(a)	
# endif
# ifndef readunsigned
# define readunsigned(a)	(void) fscanf (yyf, "%u", & a);
# endif
# ifndef writeunsigned
# define writeunsigned(a)	(void) fprintf (yyf, "%u", a);
# endif
# ifndef getunsigned
# define getunsigned(a)		yyGet ((char *) & a, sizeof (a));
# endif
# ifndef putunsigned
# define putunsigned(a)		yyPut ((char *) & a, sizeof (a));
# endif
# ifndef copyunsigned
# define copyunsigned(a, b)		
# endif
# ifndef equalunsigned
# define equalunsigned(a, b)	a == b
# endif
# ifndef beginbool
# define beginbool(a)		
# endif
# ifndef closebool
# define closebool(a)		
# endif
# ifndef readbool
# define readbool(a)		a = fgetc (yyf) == 'T';
# endif
# ifndef writebool
# define writebool(a)		(void) fputc (a ? 'T' : 'F', yyf);
# endif
# ifndef getbool
# define getbool(a)		yyGet ((char *) & a, sizeof (a));
# endif
# ifndef putbool
# define putbool(a)		yyPut ((char *) & a, sizeof (a));
# endif
# ifndef copybool
# define copybool(a, b)		
# endif
# ifndef equalbool
# define equalbool(a, b)	a == b
# endif
# ifndef begintIdent
# define begintIdent(a)		a = NoIdent;
# endif
# ifndef closetIdent
# define closetIdent(a)		
# endif
# ifndef readtIdent
# define readtIdent(a)		a = yyReadIdent ();
# endif
# ifndef writetIdent
# define writetIdent(a)		WriteIdent (yyf, a);
# endif
# ifndef gettIdent
# define gettIdent(a)		yyGetIdent (& a);
# endif
# ifndef puttIdent
# define puttIdent(a)		yyPutIdent (a);
# endif
# ifndef copytIdent
# define copytIdent(a, b)		
# endif
# ifndef equaltIdent
# define equaltIdent(a, b)	a == b
# endif
# ifndef begintPosition
# define begintPosition(a)	a = NoPosition;
# endif
# ifndef closetPosition
# define closetPosition(a)		
# endif
# ifndef readtPosition
# define readtPosition(a)	ReadPosition (yyf, & a);
# endif
# ifndef writetPosition
# define writetPosition(a)	WritePosition (yyf, a);
# endif
# ifndef gettPosition
# define gettPosition(a)	yyGet ((char *) & a, sizeof (a));
# endif
# ifndef puttPosition
# define puttPosition(a)	yyPut ((char *) & a, sizeof (a));
# endif
# ifndef copytPosition
# define copytPosition(a, b)		
# endif
# ifndef equaltPosition
# define equaltPosition(a, b)	Compare (a, b) == 0
# endif
# ifndef beginPtrLispList
# define beginPtrLispList(a)
# endif
# ifndef closePtrLispList
# define closePtrLispList(a)
# endif
# ifndef readPtrLispList
# define readPtrLispList(a)	yyReadHex ((unsigned char *) & a, sizeof (a));
# endif
# ifndef writePtrLispList
# define writePtrLispList(a)	yyWriteHex ((unsigned char *) & a, sizeof (a));
# endif
# ifndef getPtrLispList
# define getPtrLispList(a)	yyGet ((char *) & a, sizeof (a));
# endif
# ifndef putPtrLispList
# define putPtrLispList(a)	yyPut ((char *) & a, sizeof (a));
# endif
# ifndef copyPtrLispList
# define copyPtrLispList(a, b)
# endif
# ifndef equalPtrLispList
# define equalPtrLispList(a, b)	memcmp ((char *) & a, (char *) & b, sizeof (a)) == 0
# endif
# ifndef beginPtrWireArray
# define beginPtrWireArray(a)
# endif
# ifndef closePtrWireArray
# define closePtrWireArray(a)
# endif
# ifndef readPtrWireArray
# define readPtrWireArray(a)	yyReadHex ((unsigned char *) & a, sizeof (a));
# endif
# ifndef writePtrWireArray
# define writePtrWireArray(a)	yyWriteHex ((unsigned char *) & a, sizeof (a));
# endif
# ifndef getPtrWireArray
# define getPtrWireArray(a)	yyGet ((char *) & a, sizeof (a));
# endif
# ifndef putPtrWireArray
# define putPtrWireArray(a)	yyPut ((char *) & a, sizeof (a));
# endif
# ifndef copyPtrWireArray
# define copyPtrWireArray(a, b)
# endif
# ifndef equalPtrWireArray
# define equalPtrWireArray(a, b)	memcmp ((char *) & a, (char *) & b, sizeof (a)) == 0
# endif
# ifndef beginPtrCallContextList
# define beginPtrCallContextList(a)
# endif
# ifndef closePtrCallContextList
# define closePtrCallContextList(a)
# endif
# ifndef readPtrCallContextList
# define readPtrCallContextList(a)	yyReadHex ((unsigned char *) & a, sizeof (a));
# endif
# ifndef writePtrCallContextList
# define writePtrCallContextList(a)	yyWriteHex ((unsigned char *) & a, sizeof (a));
# endif
# ifndef getPtrCallContextList
# define getPtrCallContextList(a)	yyGet ((char *) & a, sizeof (a));
# endif
# ifndef putPtrCallContextList
# define putPtrCallContextList(a)	yyPut ((char *) & a, sizeof (a));
# endif
# ifndef copyPtrCallContextList
# define copyPtrCallContextList(a, b)
# endif
# ifndef equalPtrCallContextList
# define equalPtrCallContextList(a, b)	memcmp ((char *) & a, (char *) & b, sizeof (a)) == 0
# endif
# ifndef beginPtrMP_INT
# define beginPtrMP_INT(a)
# endif
# ifndef closePtrMP_INT
# define closePtrMP_INT(a)
# endif
# ifndef readPtrMP_INT
# define readPtrMP_INT(a)	yyReadHex ((unsigned char *) & a, sizeof (a));
# endif
# ifndef writePtrMP_INT
# define writePtrMP_INT(a)	yyWriteHex ((unsigned char *) & a, sizeof (a));
# endif
# ifndef getPtrMP_INT
# define getPtrMP_INT(a)	yyGet ((char *) & a, sizeof (a));
# endif
# ifndef putPtrMP_INT
# define putPtrMP_INT(a)	yyPut ((char *) & a, sizeof (a));
# endif
# ifndef copyPtrMP_INT
# define copyPtrMP_INT(a, b)
# endif
# ifndef equalPtrMP_INT
# define equalPtrMP_INT(a, b)	memcmp ((char *) & a, (char *) & b, sizeof (a)) == 0
# endif
# ifndef beginPtrconstchar
# define beginPtrconstchar(a)
# endif
# ifndef closePtrconstchar
# define closePtrconstchar(a)
# endif
# ifndef readPtrconstchar
# define readPtrconstchar(a)	yyReadHex ((unsigned char *) & a, sizeof (a));
# endif
# ifndef writePtrconstchar
# define writePtrconstchar(a)	yyWriteHex ((unsigned char *) & a, sizeof (a));
# endif
# ifndef getPtrconstchar
# define getPtrconstchar(a)	yyGet ((char *) & a, sizeof (a));
# endif
# ifndef putPtrconstchar
# define putPtrconstchar(a)	yyPut ((char *) & a, sizeof (a));
# endif
# ifndef copyPtrconstchar
# define copyPtrconstchar(a, b)
# endif
# ifndef equalPtrconstchar
# define equalPtrconstchar(a, b)	memcmp ((char *) & a, (char *) & b, sizeof (a)) == 0
# endif
# ifndef beginImplicant
# define beginImplicant(a)
# endif
# ifndef closeImplicant
# define closeImplicant(a)
# endif
# ifndef readImplicant
# define readImplicant(a)	yyReadHex ((unsigned char *) & a, sizeof (a));
# endif
# ifndef writeImplicant
# define writeImplicant(a)	yyWriteHex ((unsigned char *) & a, sizeof (a));
# endif
# ifndef getImplicant
# define getImplicant(a)	yyGet ((char *) & a, sizeof (a));
# endif
# ifndef putImplicant
# define putImplicant(a)	yyPut ((char *) & a, sizeof (a));
# endif
# ifndef copyImplicant
# define copyImplicant(a, b)
# endif
# ifndef equalImplicant
# define equalImplicant(a, b)	memcmp ((char *) & a, (char *) & b, sizeof (a)) == 0
# endif
# ifndef beginOperators
# define beginOperators(a)
# endif
# ifndef closeOperators
# define closeOperators(a)
# endif
# ifndef readOperators
# define readOperators(a)	yyReadHex ((unsigned char *) & a, sizeof (a));
# endif
# ifndef writeOperators
# define writeOperators(a)	yyWriteHex ((unsigned char *) & a, sizeof (a));
# endif
# ifndef getOperators
# define getOperators(a)	yyGet ((char *) & a, sizeof (a));
# endif
# ifndef putOperators
# define putOperators(a)	yyPut ((char *) & a, sizeof (a));
# endif
# ifndef copyOperators
# define copyOperators(a, b)
# endif
# ifndef equalOperators
# define equalOperators(a, b)	memcmp ((char *) & a, (char *) & b, sizeof (a)) == 0
# endif
# ifndef beginExprAttributes
# define beginExprAttributes(a)
# endif
# ifndef closeExprAttributes
# define closeExprAttributes(a)
# endif
# ifndef readExprAttributes
# define readExprAttributes(a)	yyReadHex ((unsigned char *) & a, sizeof (a));
# endif
# ifndef writeExprAttributes
# define writeExprAttributes(a)	yyWriteHex ((unsigned char *) & a, sizeof (a));
# endif
# ifndef getExprAttributes
# define getExprAttributes(a)	yyGet ((char *) & a, sizeof (a));
# endif
# ifndef putExprAttributes
# define putExprAttributes(a)	yyPut ((char *) & a, sizeof (a));
# endif
# ifndef copyExprAttributes
# define copyExprAttributes(a, b)
# endif
# ifndef equalExprAttributes
# define equalExprAttributes(a, b)	memcmp ((char *) & a, (char *) & b, sizeof (a)) == 0
# endif
# ifndef beginPortSense
# define beginPortSense(a)
# endif
# ifndef closePortSense
# define closePortSense(a)
# endif
# ifndef readPortSense
# define readPortSense(a)	yyReadHex ((unsigned char *) & a, sizeof (a));
# endif
# ifndef writePortSense
# define writePortSense(a)	yyWriteHex ((unsigned char *) & a, sizeof (a));
# endif
# ifndef getPortSense
# define getPortSense(a)	yyGet ((char *) & a, sizeof (a));
# endif
# ifndef putPortSense
# define putPortSense(a)	yyPut ((char *) & a, sizeof (a));
# endif
# ifndef copyPortSense
# define copyPortSense(a, b)
# endif
# ifndef equalPortSense
# define equalPortSense(a, b)	memcmp ((char *) & a, (char *) & b, sizeof (a)) == 0
# endif
# ifndef beginPtrintList
# define beginPtrintList(a)
# endif
# ifndef closePtrintList
# define closePtrintList(a)
# endif
# ifndef readPtrintList
# define readPtrintList(a)	yyReadHex ((unsigned char *) & a, sizeof (a));
# endif
# ifndef writePtrintList
# define writePtrintList(a)	yyWriteHex ((unsigned char *) & a, sizeof (a));
# endif
# ifndef getPtrintList
# define getPtrintList(a)	yyGet ((char *) & a, sizeof (a));
# endif
# ifndef putPtrintList
# define putPtrintList(a)	yyPut ((char *) & a, sizeof (a));
# endif
# ifndef copyPtrintList
# define copyPtrintList(a, b)
# endif
# ifndef equalPtrintList
# define equalPtrintList(a, b)	memcmp ((char *) & a, (char *) & b, sizeof (a)) == 0
# endif
# ifndef beginScope
# define beginScope(a)
# endif
# ifndef closeScope
# define closeScope(a)
# endif
# ifndef readScope
# define readScope(a)	yyReadHex ((unsigned char *) & a, sizeof (a));
# endif
# ifndef writeScope
# define writeScope(a)	yyWriteHex ((unsigned char *) & a, sizeof (a));
# endif
# ifndef getScope
# define getScope(a)	yyGet ((char *) & a, sizeof (a));
# endif
# ifndef putScope
# define putScope(a)	yyPut ((char *) & a, sizeof (a));
# endif
# ifndef copyScope
# define copyScope(a, b)
# endif
# ifndef equalScope
# define equalScope(a, b)	memcmp ((char *) & a, (char *) & b, sizeof (a)) == 0
# endif
# ifndef beginPtrContext
# define beginPtrContext(a)
# endif
# ifndef closePtrContext
# define closePtrContext(a)
# endif
# ifndef readPtrContext
# define readPtrContext(a)	yyReadHex ((unsigned char *) & a, sizeof (a));
# endif
# ifndef writePtrContext
# define writePtrContext(a)	yyWriteHex ((unsigned char *) & a, sizeof (a));
# endif
# ifndef getPtrContext
# define getPtrContext(a)	yyGet ((char *) & a, sizeof (a));
# endif
# ifndef putPtrContext
# define putPtrContext(a)	yyPut ((char *) & a, sizeof (a));
# endif
# ifndef copyPtrContext
# define copyPtrContext(a, b)
# endif
# ifndef equalPtrContext
# define equalPtrContext(a, b)	memcmp ((char *) & a, (char *) & b, sizeof (a)) == 0
# endif
# ifndef beginSpan
# define beginSpan(a)
# endif
# ifndef closeSpan
# define closeSpan(a)
# endif
# ifndef readSpan
# define readSpan(a)	yyReadHex ((unsigned char *) & a, sizeof (a));
# endif
# ifndef writeSpan
# define writeSpan(a)	yyWriteHex ((unsigned char *) & a, sizeof (a));
# endif
# ifndef getSpan
# define getSpan(a)	yyGet ((char *) & a, sizeof (a));
# endif
# ifndef putSpan
# define putSpan(a)	yyPut ((char *) & a, sizeof (a));
# endif
# ifndef copySpan
# define copySpan(a, b)
# endif
# ifndef equalSpan
# define equalSpan(a, b)	memcmp ((char *) & a, (char *) & b, sizeof (a)) == 0
# endif
# ifndef beginPtrType
# define beginPtrType(a)
# endif
# ifndef closePtrType
# define closePtrType(a)
# endif
# ifndef readPtrType
# define readPtrType(a)	yyReadHex ((unsigned char *) & a, sizeof (a));
# endif
# ifndef writePtrType
# define writePtrType(a)	yyWriteHex ((unsigned char *) & a, sizeof (a));
# endif
# ifndef getPtrType
# define getPtrType(a)	yyGet ((char *) & a, sizeof (a));
# endif
# ifndef putPtrType
# define putPtrType(a)	yyPut ((char *) & a, sizeof (a));
# endif
# ifndef copyPtrType
# define copyPtrType(a, b)
# endif
# ifndef equalPtrType
# define equalPtrType(a, b)	memcmp ((char *) & a, (char *) & b, sizeof (a)) == 0
# endif
# ifndef beginPtrImplicantList
# define beginPtrImplicantList(a)
# endif
# ifndef closePtrImplicantList
# define closePtrImplicantList(a)
# endif
# ifndef readPtrImplicantList
# define readPtrImplicantList(a)	yyReadHex ((unsigned char *) & a, sizeof (a));
# endif
# ifndef writePtrImplicantList
# define writePtrImplicantList(a)	yyWriteHex ((unsigned char *) & a, sizeof (a));
# endif
# ifndef getPtrImplicantList
# define getPtrImplicantList(a)	yyGet ((char *) & a, sizeof (a));
# endif
# ifndef putPtrImplicantList
# define putPtrImplicantList(a)	yyPut ((char *) & a, sizeof (a));
# endif
# ifndef copyPtrImplicantList
# define copyPtrImplicantList(a, b)
# endif
# ifndef equalPtrImplicantList
# define equalPtrImplicantList(a, b)	memcmp ((char *) & a, (char *) & b, sizeof (a)) == 0
# endif
# ifndef beginPtrBindingList
# define beginPtrBindingList(a)
# endif
# ifndef closePtrBindingList
# define closePtrBindingList(a)
# endif
# ifndef readPtrBindingList
# define readPtrBindingList(a)	yyReadHex ((unsigned char *) & a, sizeof (a));
# endif
# ifndef writePtrBindingList
# define writePtrBindingList(a)	yyWriteHex ((unsigned char *) & a, sizeof (a));
# endif
# ifndef getPtrBindingList
# define getPtrBindingList(a)	yyGet ((char *) & a, sizeof (a));
# endif
# ifndef putPtrBindingList
# define putPtrBindingList(a)	yyPut ((char *) & a, sizeof (a));
# endif
# ifndef copyPtrBindingList
# define copyPtrBindingList(a, b)
# endif
# ifndef equalPtrBindingList
# define equalPtrBindingList(a, b)	memcmp ((char *) & a, (char *) & b, sizeof (a)) == 0
# endif
# ifndef beginPtrInstanceList
# define beginPtrInstanceList(a)
# endif
# ifndef closePtrInstanceList
# define closePtrInstanceList(a)
# endif
# ifndef readPtrInstanceList
# define readPtrInstanceList(a)	yyReadHex ((unsigned char *) & a, sizeof (a));
# endif
# ifndef writePtrInstanceList
# define writePtrInstanceList(a)	yyWriteHex ((unsigned char *) & a, sizeof (a));
# endif
# ifndef getPtrInstanceList
# define getPtrInstanceList(a)	yyGet ((char *) & a, sizeof (a));
# endif
# ifndef putPtrInstanceList
# define putPtrInstanceList(a)	yyPut ((char *) & a, sizeof (a));
# endif
# ifndef copyPtrInstanceList
# define copyPtrInstanceList(a, b)
# endif
# ifndef equalPtrInstanceList
# define equalPtrInstanceList(a, b)	memcmp ((char *) & a, (char *) & b, sizeof (a)) == 0
# endif
# ifndef beginPtrInstance
# define beginPtrInstance(a)
# endif
# ifndef closePtrInstance
# define closePtrInstance(a)
# endif
# ifndef readPtrInstance
# define readPtrInstance(a)	yyReadHex ((unsigned char *) & a, sizeof (a));
# endif
# ifndef writePtrInstance
# define writePtrInstance(a)	yyWriteHex ((unsigned char *) & a, sizeof (a));
# endif
# ifndef getPtrInstance
# define getPtrInstance(a)	yyGet ((char *) & a, sizeof (a));
# endif
# ifndef putPtrInstance
# define putPtrInstance(a)	yyPut ((char *) & a, sizeof (a));
# endif
# ifndef copyPtrInstance
# define copyPtrInstance(a, b)
# endif
# ifndef equalPtrInstance
# define equalPtrInstance(a, b)	memcmp ((char *) & a, (char *) & b, sizeof (a)) == 0
# endif
# ifndef beginPtrSpanList
# define beginPtrSpanList(a)
# endif
# ifndef closePtrSpanList
# define closePtrSpanList(a)
# endif
# ifndef readPtrSpanList
# define readPtrSpanList(a)	yyReadHex ((unsigned char *) & a, sizeof (a));
# endif
# ifndef writePtrSpanList
# define writePtrSpanList(a)	yyWriteHex ((unsigned char *) & a, sizeof (a));
# endif
# ifndef getPtrSpanList
# define getPtrSpanList(a)	yyGet ((char *) & a, sizeof (a));
# endif
# ifndef putPtrSpanList
# define putPtrSpanList(a)	yyPut ((char *) & a, sizeof (a));
# endif
# ifndef copyPtrSpanList
# define copyPtrSpanList(a, b)
# endif
# ifndef equalPtrSpanList
# define equalPtrSpanList(a, b)	memcmp ((char *) & a, (char *) & b, sizeof (a)) == 0
# endif
# ifndef beginPtrSpanListList
# define beginPtrSpanListList(a)
# endif
# ifndef closePtrSpanListList
# define closePtrSpanListList(a)
# endif
# ifndef readPtrSpanListList
# define readPtrSpanListList(a)	yyReadHex ((unsigned char *) & a, sizeof (a));
# endif
# ifndef writePtrSpanListList
# define writePtrSpanListList(a)	yyWriteHex ((unsigned char *) & a, sizeof (a));
# endif
# ifndef getPtrSpanListList
# define getPtrSpanListList(a)	yyGet ((char *) & a, sizeof (a));
# endif
# ifndef putPtrSpanListList
# define putPtrSpanListList(a)	yyPut ((char *) & a, sizeof (a));
# endif
# ifndef copyPtrSpanListList
# define copyPtrSpanListList(a, b)
# endif
# ifndef equalPtrSpanListList
# define equalPtrSpanListList(a, b)	memcmp ((char *) & a, (char *) & b, sizeof (a)) == 0
# endif
# ifndef beginPtrWire
# define beginPtrWire(a)
# endif
# ifndef closePtrWire
# define closePtrWire(a)
# endif
# ifndef readPtrWire
# define readPtrWire(a)	yyReadHex ((unsigned char *) & a, sizeof (a));
# endif
# ifndef writePtrWire
# define writePtrWire(a)	yyWriteHex ((unsigned char *) & a, sizeof (a));
# endif
# ifndef getPtrWire
# define getPtrWire(a)	yyGet ((char *) & a, sizeof (a));
# endif
# ifndef putPtrWire
# define putPtrWire(a)	yyPut ((char *) & a, sizeof (a));
# endif
# ifndef copyPtrWire
# define copyPtrWire(a, b)
# endif
# ifndef equalPtrWire
# define equalPtrWire(a, b)	memcmp ((char *) & a, (char *) & b, sizeof (a)) == 0
# endif
# ifndef beginPtrAccess
# define beginPtrAccess(a)
# endif
# ifndef closePtrAccess
# define closePtrAccess(a)
# endif
# ifndef readPtrAccess
# define readPtrAccess(a)	yyReadHex ((unsigned char *) & a, sizeof (a));
# endif
# ifndef writePtrAccess
# define writePtrAccess(a)	yyWriteHex ((unsigned char *) & a, sizeof (a));
# endif
# ifndef getPtrAccess
# define getPtrAccess(a)	yyGet ((char *) & a, sizeof (a));
# endif
# ifndef putPtrAccess
# define putPtrAccess(a)	yyPut ((char *) & a, sizeof (a));
# endif
# ifndef copyPtrAccess
# define copyPtrAccess(a, b)
# endif
# ifndef equalPtrAccess
# define equalPtrAccess(a, b)	memcmp ((char *) & a, (char *) & b, sizeof (a)) == 0
# endif
# ifndef beginPtrExprAttributesList
# define beginPtrExprAttributesList(a)
# endif
# ifndef closePtrExprAttributesList
# define closePtrExprAttributesList(a)
# endif
# ifndef readPtrExprAttributesList
# define readPtrExprAttributesList(a)	yyReadHex ((unsigned char *) & a, sizeof (a));
# endif
# ifndef writePtrExprAttributesList
# define writePtrExprAttributesList(a)	yyWriteHex ((unsigned char *) & a, sizeof (a));
# endif
# ifndef getPtrExprAttributesList
# define getPtrExprAttributesList(a)	yyGet ((char *) & a, sizeof (a));
# endif
# ifndef putPtrExprAttributesList
# define putPtrExprAttributesList(a)	yyPut ((char *) & a, sizeof (a));
# endif
# ifndef copyPtrExprAttributesList
# define copyPtrExprAttributesList(a, b)
# endif
# ifndef equalPtrExprAttributesList
# define equalPtrExprAttributesList(a, b)	memcmp ((char *) & a, (char *) & b, sizeof (a)) == 0
# endif
# ifndef beginPtrIdentList
# define beginPtrIdentList(a)
# endif
# ifndef closePtrIdentList
# define closePtrIdentList(a)
# endif
# ifndef readPtrIdentList
# define readPtrIdentList(a)	yyReadHex ((unsigned char *) & a, sizeof (a));
# endif
# ifndef writePtrIdentList
# define writePtrIdentList(a)	yyWriteHex ((unsigned char *) & a, sizeof (a));
# endif
# ifndef getPtrIdentList
# define getPtrIdentList(a)	yyGet ((char *) & a, sizeof (a));
# endif
# ifndef putPtrIdentList
# define putPtrIdentList(a)	yyPut ((char *) & a, sizeof (a));
# endif
# ifndef copyPtrIdentList
# define copyPtrIdentList(a, b)
# endif
# ifndef equalPtrIdentList
# define equalPtrIdentList(a, b)	memcmp ((char *) & a, (char *) & b, sizeof (a)) == 0
# endif
# ifndef beginCommandAttributes
# define beginCommandAttributes(a)
# endif
# ifndef closeCommandAttributes
# define closeCommandAttributes(a)
# endif
# ifndef readCommandAttributes
# define readCommandAttributes(a)	yyReadHex ((unsigned char *) & a, sizeof (a));
# endif
# ifndef writeCommandAttributes
# define writeCommandAttributes(a)	yyWriteHex ((unsigned char *) & a, sizeof (a));
# endif
# ifndef getCommandAttributes
# define getCommandAttributes(a)	yyGet ((char *) & a, sizeof (a));
# endif
# ifndef putCommandAttributes
# define putCommandAttributes(a)	yyPut ((char *) & a, sizeof (a));
# endif
# ifndef copyCommandAttributes
# define copyCommandAttributes(a, b)
# endif
# ifndef equalCommandAttributes
# define equalCommandAttributes(a, b)	memcmp ((char *) & a, (char *) & b, sizeof (a)) == 0
# endif
# ifndef beginPtrchar
# define beginPtrchar(a)
# endif
# ifndef closePtrchar
# define closePtrchar(a)
# endif
# ifndef readPtrchar
# define readPtrchar(a)	yyReadHex ((unsigned char *) & a, sizeof (a));
# endif
# ifndef writePtrchar
# define writePtrchar(a)	yyWriteHex ((unsigned char *) & a, sizeof (a));
# endif
# ifndef getPtrchar
# define getPtrchar(a)	yyGet ((char *) & a, sizeof (a));
# endif
# ifndef putPtrchar
# define putPtrchar(a)	yyPut ((char *) & a, sizeof (a));
# endif
# ifndef copyPtrchar
# define copyPtrchar(a, b)
# endif
# ifndef equalPtrchar
# define equalPtrchar(a, b)	memcmp ((char *) & a, (char *) & b, sizeof (a)) == 0
# endif
# ifndef beginPtrCommandAttributesList
# define beginPtrCommandAttributesList(a)
# endif
# ifndef closePtrCommandAttributesList
# define closePtrCommandAttributesList(a)
# endif
# ifndef readPtrCommandAttributesList
# define readPtrCommandAttributesList(a)	yyReadHex ((unsigned char *) & a, sizeof (a));
# endif
# ifndef writePtrCommandAttributesList
# define writePtrCommandAttributesList(a)	yyWriteHex ((unsigned char *) & a, sizeof (a));
# endif
# ifndef getPtrCommandAttributesList
# define getPtrCommandAttributesList(a)	yyGet ((char *) & a, sizeof (a));
# endif
# ifndef putPtrCommandAttributesList
# define putPtrCommandAttributesList(a)	yyPut ((char *) & a, sizeof (a));
# endif
# ifndef copyPtrCommandAttributesList
# define copyPtrCommandAttributesList(a, b)
# endif
# ifndef equalPtrCommandAttributesList
# define equalPtrCommandAttributesList(a, b)	memcmp ((char *) & a, (char *) & b, sizeof (a)) == 0
# endif
# ifndef beginPtrImplicantListList
# define beginPtrImplicantListList(a)
# endif
# ifndef closePtrImplicantListList
# define closePtrImplicantListList(a)
# endif
# ifndef readPtrImplicantListList
# define readPtrImplicantListList(a)	yyReadHex ((unsigned char *) & a, sizeof (a));
# endif
# ifndef writePtrImplicantListList
# define writePtrImplicantListList(a)	yyWriteHex ((unsigned char *) & a, sizeof (a));
# endif
# ifndef getPtrImplicantListList
# define getPtrImplicantListList(a)	yyGet ((char *) & a, sizeof (a));
# endif
# ifndef putPtrImplicantListList
# define putPtrImplicantListList(a)	yyPut ((char *) & a, sizeof (a));
# endif
# ifndef copyPtrImplicantListList
# define copyPtrImplicantListList(a, b)
# endif
# ifndef equalPtrImplicantListList
# define equalPtrImplicantListList(a, b)	memcmp ((char *) & a, (char *) & b, sizeof (a)) == 0
# endif
# ifndef beginSignedBits
# define beginSignedBits(a)
# endif
# ifndef closeSignedBits
# define closeSignedBits(a)
# endif
# ifndef readSignedBits
# define readSignedBits(a)	yyReadHex ((unsigned char *) & a, sizeof (a));
# endif
# ifndef writeSignedBits
# define writeSignedBits(a)	yyWriteHex ((unsigned char *) & a, sizeof (a));
# endif
# ifndef getSignedBits
# define getSignedBits(a)	yyGet ((char *) & a, sizeof (a));
# endif
# ifndef putSignedBits
# define putSignedBits(a)	yyPut ((char *) & a, sizeof (a));
# endif
# ifndef copySignedBits
# define copySignedBits(a, b)
# endif
# ifndef equalSignedBits
# define equalSignedBits(a, b)	memcmp ((char *) & a, (char *) & b, sizeof (a)) == 0
# endif
# ifndef beginProcedureArgsType
# define beginProcedureArgsType(a)
# endif
# ifndef closeProcedureArgsType
# define closeProcedureArgsType(a)
# endif
# ifndef readProcedureArgsType
# define readProcedureArgsType(a)	yyReadHex ((unsigned char *) & a, sizeof (a));
# endif
# ifndef writeProcedureArgsType
# define writeProcedureArgsType(a)	yyWriteHex ((unsigned char *) & a, sizeof (a));
# endif
# ifndef getProcedureArgsType
# define getProcedureArgsType(a)	yyGet ((char *) & a, sizeof (a));
# endif
# ifndef putProcedureArgsType
# define putProcedureArgsType(a)	yyPut ((char *) & a, sizeof (a));
# endif
# ifndef copyProcedureArgsType
# define copyProcedureArgsType(a, b)
# endif
# ifndef equalProcedureArgsType
# define equalProcedureArgsType(a, b)	memcmp ((char *) & a, (char *) & b, sizeof (a)) == 0
# endif
# ifndef beginPtrProcedure
# define beginPtrProcedure(a)
# endif
# ifndef closePtrProcedure
# define closePtrProcedure(a)
# endif
# ifndef readPtrProcedure
# define readPtrProcedure(a)	yyReadHex ((unsigned char *) & a, sizeof (a));
# endif
# ifndef writePtrProcedure
# define writePtrProcedure(a)	yyWriteHex ((unsigned char *) & a, sizeof (a));
# endif
# ifndef getPtrProcedure
# define getPtrProcedure(a)	yyGet ((char *) & a, sizeof (a));
# endif
# ifndef putPtrProcedure
# define putPtrProcedure(a)	yyPut ((char *) & a, sizeof (a));
# endif
# ifndef copyPtrProcedure
# define copyPtrProcedure(a, b)
# endif
# ifndef equalPtrProcedure
# define equalPtrProcedure(a, b)	memcmp ((char *) & a, (char *) & b, sizeof (a)) == 0
# endif
# ifndef beginPtrComponentParameterList
# define beginPtrComponentParameterList(a)
# endif
# ifndef closePtrComponentParameterList
# define closePtrComponentParameterList(a)
# endif
# ifndef readPtrComponentParameterList
# define readPtrComponentParameterList(a)	yyReadHex ((unsigned char *) & a, sizeof (a));
# endif
# ifndef writePtrComponentParameterList
# define writePtrComponentParameterList(a)	yyWriteHex ((unsigned char *) & a, sizeof (a));
# endif
# ifndef getPtrComponentParameterList
# define getPtrComponentParameterList(a)	yyGet ((char *) & a, sizeof (a));
# endif
# ifndef putPtrComponentParameterList
# define putPtrComponentParameterList(a)	yyPut ((char *) & a, sizeof (a));
# endif
# ifndef copyPtrComponentParameterList
# define copyPtrComponentParameterList(a, b)
# endif
# ifndef equalPtrComponentParameterList
# define equalPtrComponentParameterList(a, b)	memcmp ((char *) & a, (char *) & b, sizeof (a)) == 0
# endif
# ifndef beginPtrComponent
# define beginPtrComponent(a)
# endif
# ifndef closePtrComponent
# define closePtrComponent(a)
# endif
# ifndef readPtrComponent
# define readPtrComponent(a)	yyReadHex ((unsigned char *) & a, sizeof (a));
# endif
# ifndef writePtrComponent
# define writePtrComponent(a)	yyWriteHex ((unsigned char *) & a, sizeof (a));
# endif
# ifndef getPtrComponent
# define getPtrComponent(a)	yyGet ((char *) & a, sizeof (a));
# endif
# ifndef putPtrComponent
# define putPtrComponent(a)	yyPut ((char *) & a, sizeof (a));
# endif
# ifndef copyPtrComponent
# define copyPtrComponent(a, b)
# endif
# ifndef equalPtrComponent
# define equalPtrComponent(a, b)	memcmp ((char *) & a, (char *) & b, sizeof (a)) == 0
# endif
# ifndef beginPtrComponentList
# define beginPtrComponentList(a)
# endif
# ifndef closePtrComponentList
# define closePtrComponentList(a)
# endif
# ifndef readPtrComponentList
# define readPtrComponentList(a)	yyReadHex ((unsigned char *) & a, sizeof (a));
# endif
# ifndef writePtrComponentList
# define writePtrComponentList(a)	yyWriteHex ((unsigned char *) & a, sizeof (a));
# endif
# ifndef getPtrComponentList
# define getPtrComponentList(a)	yyGet ((char *) & a, sizeof (a));
# endif
# ifndef putPtrComponentList
# define putPtrComponentList(a)	yyPut ((char *) & a, sizeof (a));
# endif
# ifndef copyPtrComponentList
# define copyPtrComponentList(a, b)
# endif
# ifndef equalPtrComponentList
# define equalPtrComponentList(a, b)	memcmp ((char *) & a, (char *) & b, sizeof (a)) == 0
# endif
# ifndef beginPtrTypeList
# define beginPtrTypeList(a)
# endif
# ifndef closePtrTypeList
# define closePtrTypeList(a)
# endif
# ifndef readPtrTypeList
# define readPtrTypeList(a)	yyReadHex ((unsigned char *) & a, sizeof (a));
# endif
# ifndef writePtrTypeList
# define writePtrTypeList(a)	yyWriteHex ((unsigned char *) & a, sizeof (a));
# endif
# ifndef getPtrTypeList
# define getPtrTypeList(a)	yyGet ((char *) & a, sizeof (a));
# endif
# ifndef putPtrTypeList
# define putPtrTypeList(a)	yyPut ((char *) & a, sizeof (a));
# endif
# ifndef copyPtrTypeList
# define copyPtrTypeList(a, b)
# endif
# ifndef equalPtrTypeList
# define equalPtrTypeList(a, b)	memcmp ((char *) & a, (char *) & b, sizeof (a)) == 0
# endif
# ifndef begintTree
# define begintTree(a)	a = NoTree;
# endif
# ifndef closetTree
# define closetTree(a)	yyChild = a; a = NoTree; yyReleaseTree (yyChild);
# endif
# ifndef readtTree
# define readtTree(a)	yyReadTree (a);
# endif
# ifndef writetTree
# define writetTree(a)	yyWriteTree (a);
# endif
# ifndef gettTree
# define gettTree(a)	yyGetTree (a);
# endif
# ifndef puttTree
# define puttTree(a)	yyPutTree (a);
# endif
# ifndef copytTree
# define copytTree(a, b)	yyCopyTree (b, & a);
# endif
# ifndef equaltTree
# define equaltTree(a, b)	IsEqualTree (a, b)
# endif
