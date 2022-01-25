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

#include <string.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdlib.h>

#include "lscan.h"

char *TMPScanner_token = NULL;
int TMPScanner_tokenlength;
int TMPScanner_tokenAllocatedLength = 0;

void TMPScanner_NewToken (char *str, int length)
{
    if (length + 1 > TMPScanner_tokenAllocatedLength)
    {
        free (TMPScanner_token);
        TMPScanner_token = malloc (length + 1);
        TMPScanner_tokenAllocatedLength = length + 1;
    }

    memcpy (TMPScanner_token, str, length);
    TMPScanner_token[length] = 0;
    TMPScanner_tokenlength = length;
}

GTokenType TMPScanner_GetNextToken (char **str)
{
    char c;
    char *ptr = *str;

  nextchar:
    c = *ptr;
    ptr++;

    switch (c)
    {
    case ' ':
    case '\t':
    case '\r':
    case '\n':
        goto nextchar;

    case '(':
        *str = ptr;
        return G_TOKEN_LEFT_PAREN;

    case ')':
        *str = ptr;
        return G_TOKEN_RIGHT_PAREN;

    case '"':
        {
            char *beg = ptr;

            while (*ptr && *ptr != '"')
                ptr++;

            if (!*ptr)
            {
                *str = ptr;
                return G_TOKEN_EOF;
            }

            TMPScanner_NewToken (beg, ptr - beg);
            ptr++;
        }
        *str = ptr;
        return G_TOKEN_STRING;

    case ';':
        do
        {
            c = *ptr;
            ptr++;
            if (!c)
            {
                *str = ptr;
                return G_TOKEN_EOF;
            }
        }
        while (c != '\n');
        goto nextchar;

    case 0:
        *str = ptr;
        return G_TOKEN_EOF;
    }

    {
        char *beg = ptr - 1;

        for (;; ptr++)
        {
            switch (*ptr)
            {
            case '(':
            case ')':
            case '"':
            case ';':
            case ' ':
            case '\t':
            case '\r':
            case '\n':
            case 0:
                TMPScanner_NewToken (beg, ptr - beg);
                *str = ptr;
                return G_TOKEN_IDENTIFIER;
            }
        }
    }
}

char *getFirstKeywordPtr (char *ptr)
{
    while (*ptr == '(' || *ptr == ')' || *ptr == ' ' || *ptr == '\r' || *ptr == '\n' || *ptr == '\t')
    {
        if (*ptr == 0)
            return NULL;
        else
            ptr++;
    }
    return ptr;
}

char *getNthKeywordCopy (char *ptr, int n)
{
    char *str = ptr;

    for (;;)
    {
        GTokenType t = TMPScanner_GetNextToken (&str);

        switch (t)
        {
        case G_TOKEN_IDENTIFIER:
        case G_TOKEN_STRING:
            n--;
            if (n == 0)
                return g_strdup (TMPScanner_token);
            break;
        case G_TOKEN_EOF:
            return NULL;
        default:
            ;
        }
    }
}
char *getFirstKeywordCopy (char *ptr)
{
    return getNthKeywordCopy (ptr, 1);
}

char *getSecondKeywordCopy (char *ptr)
{
    return getNthKeywordCopy (ptr, 2);
}
