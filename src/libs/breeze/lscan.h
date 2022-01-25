/*
	The Balsa Asynchronous Hardware Synthesis System
	Copyright (C) 1995-2009 School of Computer Science
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

	`lparse.h'
	Lispy file parser

*/

#ifndef LSCAN_H
#define LSCAN_H

#include <glib.h>

extern char *TMPScanner_token;
extern int TMPScanner_tokenlength;
extern int TMPScanner_tokenAllocatedLength;

void TMPScanner_NewToken (char *str, int length);
GTokenType TMPScanner_GetNextToken (char **str);

char *getFirstKeywordPtr (char *ptr);
char *getFirstKeywordCopy (char *ptr);
char *getSecondKeywordCopy (char *ptr);

#endif
