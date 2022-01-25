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

	`lparse.c'
	Lispy file parser

*/

#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#include "lparse.h"
#include "lscan.h"

/* NewTMP{Symbol,...} : node constructors */
PtrTMPNode NewTMPSymbol (char *symbol, int count)
{
    PtrTMPNode node = (PtrTMPNode) g_malloc (sizeof (TMPNode));
    char *newSymbol;

    if (count == -1)
        count = strlen (symbol);
    newSymbol = (char *) g_malloc (1 + count);
    strncpy (newSymbol, symbol, count);
    newSymbol[count] = '\0';

    node->type = TMPSymbol;
    node->body.string = newSymbol;

    return node;
}

PtrTMPNode NewTMPString (char *string, int count)
{
    PtrTMPNode node = NewTMPSymbol (string, count);

    node->type = TMPString;

    return node;
}

/*
PtrTMPNode NewTMPNumber (int number)
{
    PtrTMPNode node = (PtrTMPNode) g_malloc (sizeof (TMPNode));

    node->type = TMPNumber;
    node->body.number = number;

    return node;
}
*/
PtrTMPNode NewTMPNumberStr (char *string, int count)
{
    PtrTMPNode node = NewTMPSymbol (string, count);

    node->type = TMPNumberStr;

    return node;
}

PtrTMPNode NewTMPBoolean (gboolean boole)
{
    PtrTMPNode node = (PtrTMPNode) g_malloc (sizeof (TMPNode));

    node->type = TMPBoolean;
    node->body.boolean = boole;

    return node;
}

PtrTMPNode NewTMPList (GList * list)
{
    PtrTMPNode node = (PtrTMPNode) g_malloc (sizeof (TMPNode));

    node->type = TMPList;
    node->body.list = list;

    return node;
}

/* DeleteTMPNode : node destructor. Deallocates `node' and `node->body' members */
void DeleteTMPNode (PtrTMPNode node)
{
    if (!node)
        return;

    switch (node->type)
    {
        //    case TMPNumber:
    case TMPBoolean:
        break;
    case TMPNumberStr:
    case TMPSymbol:
    case TMPString:
        g_free (node->body.string);
        break;
    case TMPList:
        {
            GList *listIter = node->body.list;

            while (listIter)
            {
                DeleteTMPNode ((PtrTMPNode) listIter->data);
                listIter = listIter->next;
            }
            g_list_free (node->body.list);
        }
    }

    g_free (node);
}

/* DeepCopyTMPNode : Recursively duplicate the node and its subnodes */
PtrTMPNode DeepCopyTMPNode (PtrTMPNode node)
{
    if (!node)
        return NULL;

    switch (node->type)
    {
        /*
           case TMPNumber:
           return NewTMPNumber (node->body.number);
         */
    case TMPNumberStr:
        return NewTMPNumberStr (node->body.string, -1);
    case TMPBoolean:
        return NewTMPBoolean (node->body.boolean);
    case TMPSymbol:
        return NewTMPSymbol (node->body.string, -1);
    case TMPString:
        return NewTMPString (node->body.string, -1);
    case TMPList:
        {
            GList *newList = NULL;
            GList *tmp;

            for (tmp = g_list_last (node->body.list); tmp; tmp = tmp->prev)
            {
                PtrTMPNode subnode = (PtrTMPNode) tmp->data;

                newList = g_list_prepend (newList, DeepCopyTMPNode (subnode));
            }

            return NewTMPList (newList);
        }
    }

    return NULL;
}

/* PrintTMPNode: print a list node in a lisp like format onto stream `stream'.
	NB. This currently just prints on a single line. */
void PrintTMPNode (FILE * stream, PtrTMPNode node)
{
    if (!node)
        fprintf (stream, "NULL");
    else
    {
        switch (node->type)
        {
            /*
               case TMPNumber:
               fprintf (stream, "%d", node->body.number);
               break;
             */
        case TMPNumberStr:
            fprintf (stream, "%s", node->body.string);
            break;
        case TMPString:
            fprintf (stream, "\"%s\"", node->body.string);
            break;
        case TMPSymbol:
            fprintf (stream, "%s", node->body.string);
            break;
        case TMPBoolean:
            fprintf (stream, "#%c", (node->body.boolean ? 't' : 'f'));
            break;
        case TMPList:
            {
                GList *listIter = node->body.list;

                fprintf (stream, "(");

                while (listIter)
                {
                    PrintTMPNode (stream, listIter->data);
                    listIter = listIter->next;
                    if (listIter)
                        fprintf (stream, " ");
                }

                fprintf (stream, ")");
            }
            break;
        }
    }
}

/* TMPNodePrintedLength : (over) estimate the length of string
	needed to print the given TMPNode */
static unsigned TMPNodePrintedLength (TMPNode * node)
{
    unsigned ret = 0;

    if (node)
    {
        switch (node->type)
        {
        case TMPNumberStr:
        case TMPSymbol:
            ret += strlen (node->body.string);
            break;
        case TMPString:
            ret += strlen (node->body.string) + 2;
            break;
        case TMPBoolean:
            ret += 2;
            break;
        case TMPList:
            {
                GList *listIter = node->body.list;

                ret++;

                while (listIter)
                {
                    ret += TMPNodePrintedLength (listIter->data);
                    listIter = listIter->next;
                    if (listIter)
                        ret++;
                }

                ret++;
            }
            break;
        }
    }
    return ret;
}

/* EndOfString : oft-defined function, returns pointer to the NUL termination on the
	given string */
static char *EndOfString (char *string)
{
    while (*string)
        string++;

    return string;
}

/* TMPNodeToString: like PrintTMPNode, but print to the given string buffer.  If the given buffer
	is NULL then allocate a fresh string.  returns `buffer' or the freshly allocated string */
char *TMPNodeToString (char *buffer, PtrTMPNode node)
{
    char *bufferPtr;

    if (!buffer)
        buffer = malloc (TMPNodePrintedLength (node) + 10);

    bufferPtr = buffer;
    *bufferPtr = '\0';

    if (node)
    {
        switch (node->type)
        {
        case TMPNumberStr:
        case TMPSymbol:
            bufferPtr += sprintf (bufferPtr, "%s", node->body.string);
            break;
        case TMPString:
            bufferPtr += sprintf (bufferPtr, "\"%s\"", node->body.string);
            break;
        case TMPBoolean:
            bufferPtr += sprintf (bufferPtr, "#%c", (node->body.boolean ? 't' : 'f'));
            break;
        case TMPList:
            {
                GList *listIter = node->body.list;

                *bufferPtr = '(';
                bufferPtr++;
                *bufferPtr = '\0';

                while (listIter)
                {
                    bufferPtr = EndOfString (TMPNodeToString (bufferPtr, listIter->data));
                    listIter = listIter->next;
                    if (listIter)
                    {
                        *bufferPtr = ' ';
                        bufferPtr++;
                        *bufferPtr = '\0';
                    }
                }

                *bufferPtr = ')';
                bufferPtr++;
                *bufferPtr = '\0';
            }
            break;
        }
    }
    return buffer;
}

/* TMPIsHeaded : returns TRUE if the given node is a list with a head symbol the same
	as headSymbol. eg. TMPIsHeaded ( "(myNode 10 20)", "myNode" ) returns TRUE.
	If headSymbol is NULL then return true if *any* symbol is the head of the list */
gboolean TMPIsHeaded (PtrTMPNode node, char *headSymbol)
{
    return (node
      && node->type == TMPList
      && node->body.list
      && ((PtrTMPNode) (node->body.list->data))->type ==
      TMPSymbol && (!headSymbol || strcmp (((PtrTMPNode) (node->body.list->data))->body.string, headSymbol) == 0));
}

/* ParseTMPNode : parse a TMP Node out of a string,
	`inAList' should be FALSE if we are not expecting a ')' at the end of
	a list of items
	,`EOFiscorrect` should be TRUE if we want EOF to finish without error.*/
PtrTMPNode ParseTMPNode (char **str, gboolean inAList, gboolean EOFiscorrect)
{
    GTokenType tokenType;
    GList *list = NULL;

    if (*str == NULL)
        return NULL;

    do
    {
        tokenType = TMPScanner_GetNextToken (str);

        switch (tokenType)
        {
        case G_TOKEN_EOF:
            if (inAList)
            {
                if (EOFiscorrect)
                    return NewTMPList (g_list_reverse (list));
                else
                    fprintf (stderr, "ParseTMPNode: premature end of file in list\n");
            }
            return NULL;        /* EOF */
        case G_TOKEN_INT:      /* integer */
            {
                fprintf (stderr, "ParseTMPNode: G_TOKEN_INT should never occur\n");
                /*
                   PtrTMPNode num = NewTMPNumber (g_scanner_cur_value (scanner).v_int);

                   if (inAList)
                   list = g_list_prepend (list, (gpointer) num);
                   else
                   return num;
                 */
            }
            break;
        case G_TOKEN_IDENTIFIER: /* identifier (Symbol) or #f #t or number */
            {
                char *token = TMPScanner_token;

                if (*token >= '0' && *token <= '9')
                {
                    PtrTMPNode num = NewTMPNumberStr (token, TMPScanner_tokenlength);

                    if (inAList)
                        list = g_list_prepend (list, (gpointer) num);
                    else
                        return num;
                } else if (*token == '#')
                {
                    PtrTMPNode boole = NULL;

                    if (token[1] && !token[2])
                    {
                        switch (token[1])
                        {
                        case 't':
                            boole = NewTMPBoolean (TRUE);
                            break;
                        case 'f':
                            boole = NewTMPBoolean (FALSE);
                            break;
                        }
                    }
                    if (boole)
                    {
                        if (inAList)
                            list = g_list_prepend (list, (gpointer) boole);
                        else
                            return boole;
                    } else
                    {
                        fprintf (stderr, "ParseTMPNode: invalid symbol in file");
                        exit (EXIT_FAILURE);
                    }
                } else
                {
                    PtrTMPNode ident = NewTMPSymbol (TMPScanner_token,
                      TMPScanner_tokenlength);

                    if (inAList)
                        list = g_list_prepend (list, (gpointer) ident);
                    else
                        return ident;
                }
            }
            break;
        case G_TOKEN_STRING:   /* string */
            {
                PtrTMPNode str2 = NewTMPString (TMPScanner_token,
                  TMPScanner_tokenlength);

                if (inAList)
                    list = g_list_prepend (list, (gpointer) str2);
                else
                    return str2;
            }
            break;
        case G_TOKEN_LEFT_PAREN:
            if (inAList)
                list = g_list_prepend (list, ParseTMPNode (str, TRUE, FALSE));
            else
                return ParseTMPNode (str, TRUE, FALSE);
            break;

        case G_TOKEN_RIGHT_PAREN:
            if (inAList)
                return NewTMPList (g_list_reverse (list));
            else
                fprintf (stderr, "ParseTMPNode: unexpected ')' found\n");
            break;
        default:
            fprintf (stderr, "ParseTMPNode: unexpected token\n");
            return NULL;
            break;
        }
    }
    while (inAList);

    return NULL;
}

/* ParseCompleteFileAsTMPNode : open a new file and parse it. */
/*                                                               */
/* TODO: Find a better a way than copying the whole file to memory  */
PtrTMPNode ParseCompleteFileAsTMPNode (char *filename)
{
    FILE *f = fopen (filename, "rb");
    char *contents, *tmp;
    struct stat st;

    if (!f)
    {
        return NULL;
    }

    stat (filename, &st);
    contents = (char *) malloc (st.st_size + 1);
    int size = fread (contents, 1, st.st_size, f);

    if (size != st.st_size)
    {
        fprintf (stderr, "Error in reading %s\n", filename);
        return NULL;
    }
    contents[st.st_size] = 0;

    tmp = contents;
    PtrTMPNode node = ParseTMPNode (&tmp, FALSE, FALSE);

    free (contents);
    fclose (f);
    return node;
}
