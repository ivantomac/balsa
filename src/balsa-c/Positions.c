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

	`Positions.c'
	Derived from KCT reuse/Positions.c
	Added support for file path in positions
	
 */

#include "Positions.h"
#include "misclists.h"
#include "callcontexts.h"

/* Dummy Position */
tPosition NoPosition = { 0, 0, NULL, 0 };

/* Compare : -1 iff pos1 < pos2, 0 iff pos1 = pos2, 1 iff pos1 > pos2 */
int Compare (tPosition pos1, tPosition pos2)
{
    /* If pos1.File < pos2.File then it was MakeIdent'ed
       first and so should be reported first */
    if (pos1.File < pos2.File)
        return -1;
    if (pos1.File > pos2.File)
        return 1;
    if (!((pos1.Line == 0 && pos1.Column == 0) || (pos2.Line == 0 && pos2.Column == 0)))
    {
        if (pos1.Line < pos2.Line)
            return -1;
        if (pos1.Line > pos2.Line)
            return 1;
        if (pos1.Column < pos2.Column)
            return -1;
        if (pos1.Column > pos2.Column)
            return 1;
    }
    return 0;
}

/* WritePosition : write a position onto a stream */
void WritePosition (FILE * stream, tPosition pos)
{
    if (pos.File != NULL)
    {
        fprintf (stream, " -> ");
        StrPtrIdentList (stream, pos.File, " -> ");
    }
    if (pos.Line && pos.Column)
        fprintf (stream, "%d: (%d,%d)", pos.Line, pos.Line, pos.Column);
}

/* WriteErrorPosition : write a position onto a stream in a GCC like format,
   if the file list has the same value as the previous invocation don't reprint.
   Use format:  filename:line:column:
   or:          In file imported from filename_n:
   from ...
   filename:line:column:
   where the file list has a different value from previous invocation,
   where the file list is NULL just write --:line:column */
void WriteErrorPosition (FILE * stream, tPosition pos)
{
    static FILE *lastStream = NULL;
    static PtrIdentList lastIdentList = NULL;

    if (!pos.File)
    {
        if (pos.Line == 0)      /* line == 0, no idea where we are */
            fprintf (stream, "?:");
        else
            fprintf (stream, "?:%d:%d:", pos.Line, pos.Column);
        return;
    }
    /* Not same as before, at least one other file */
    if ((pos.File != lastIdentList || stream != lastStream) && CDR (pos.File))
    {
        fprintf (stream, "In file imported from ");
        StrPtrIdentList (stream, CDR (pos.File), ":\n                 from ");
        fprintf (stream, ":\n");
    }
    WriteIdent (stream, CAR (pos.File).ident);
    fputc (':', stream);
    if (pos.Line && pos.Column)
        fprintf (stream, "%d:%d:", pos.Line, pos.Column);

    lastStream = stream;
    lastIdentList = pos.File;
}

/* StrSBreezePosition : write a position to stream in the Breeze format */
extern void StrSBreezePosition (FILE * stream, tPosition pos)
{
    fprintf (stream, "(at %d %d \"", pos.Line, pos.Column);
    if (pos.File)
        WriteIdent (stream, CAR (pos.File).ident);
    fprintf (stream, "\" %d)", pos.CallContext ? ((CallContext *) pos.CallContext)->seqnum : 0);
    if (CallContexts)
    {
        CallContext *callContext = (CallContext *) pos.CallContext; //g_ptr_array_index (CallContexts, pos.CallContext);

        if (callContext)
            callContext->used = true;
    }
}
