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

	`pretty.h'
	KCT abstract syntax tree pretty functions to generate
	Balsa source in the lispy Breeze format
	
 */

#ifndef PRETTY_HEADER
#define PRETTY_HEADER

#include "Tree.h"

/* Stream for pretty print output */
extern FILE *PrettyPrintStream;

/* PrettyPrintNode: print a tree node to the given stream */
extern void PrettyPrintNode (tTree tree, unsigned tabCount);

/* PrettyPrintTabs: print `count' tabs.  A tab is set as two spaces */
extern void PrettyPrintTabs (unsigned count);

/* PrettyPrintProcedureDecl : print a procedure declaration */
extern void PrettyPrintProcedureDecl (tTree tree, unsigned tabCount);

/* PrettyPrintFunctionDecl : print a function declaration */
extern void PrettyPrintFunctionDecl (tTree tree, unsigned tabCount);

/* PrettyPrintBuiltinFunctionDecl : print a builtin function declaration */
extern void PrettyPrintBuiltinFunctionDecl (tTree tree, unsigned tabCount);

/* BeginPretty: do pretty printing initialisation */
extern void BeginPretty (void);

#endif /* PRETTY_HEADER */
