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

#ifndef LPARSE_H
#define LPARSE_H

#include <stdio.h>
#include <glib.h>

/* TMPNode : node node type */
typedef enum
{ TMPSymbol, TMPString, /*TMPNumber, */ TMPNumberStr, TMPBoolean,
    TMPList
}
TMPType;

typedef struct TMPNode
{
    TMPType type;
    union
    {
        char *string;           /* String and Symbol and NumberStr */
        /*        int number; */
        gboolean boolean;
        GList *list;
    }
    body;
}
TMPNode, *PtrTMPNode;

#define TMP_NODE(node) ((PtrTMPNode)(node))

/* NewTMP{Symbol,...} : node constructors */
extern PtrTMPNode NewTMPSymbol (char *symbol, int count);
extern PtrTMPNode NewTMPString (char *string, int count);

/*extern PtrTMPNode NewTMPNumber (int number);*/
extern PtrTMPNode NewTMPNumberStr (char *string, int count);
extern PtrTMPNode NewTMPBoolean (gboolean boole);
extern PtrTMPNode NewTMPList (GList * list);

/* DeleteTMPNode : node destructor. Deallocates `node' and `node->body' members */
extern void DeleteTMPNode (PtrTMPNode node);

/* DeepCopyTMPNode : Recursively duplicate the node and its subnodes */
extern PtrTMPNode DeepCopyTMPNode (PtrTMPNode node);

/* PrintTMPNode : print a list node in a lisp like format onto stream `stream'.
	NB. This currently just prints on a single line. */
extern void PrintTMPNode (FILE * stream, PtrTMPNode node);

/* TMPNodeToString: like PrintTMPNode, but print to the given string buffer.  If the given buffer
	is NULL then allocate a fresh string.  returns `buffer' or the freshly allocated string */
extern char *TMPNodeToString (char *buffer, PtrTMPNode node);

/* TMPIsHeaded : returns TRUE if the given node is a list with a head symbol the same
	as headSymbol. eg. TMPIsHeaded ( "(myNode 10 20)", "myNode" ) returns TRUE.
	If headSymbol is NULL then return true if *any* symbol is the head of the list */
extern gboolean TMPIsHeaded (PtrTMPNode node, char *headSymbol);

/* ParseTMPNode : parse a TMP Node out of the currently open scanner,
	`inAList' should be FALSE if we are not expecting a ')' at the end of
	a list of items. */
extern PtrTMPNode ParseTMPNode (char **str, gboolean inAList, gboolean EOFiscorrect);

/* ParseCompleteFileAsTMPNode : open a new file and parse it. */
extern PtrTMPNode ParseCompleteFileAsTMPNode (char *filename);

#endif
