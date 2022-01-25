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

 */

#ifndef CALLCONTEXTS_HEADER
#define CALLCONTEXTS_HEADER

#include "wires.h"
#include "procedures.h"

extern GList *CallContextDecls;
extern GPtrArray *CallContexts;

typedef struct CallContext
{
    tIdent calledProcIdent;
    tPosition callerPosition;
    tPosition calleePosition;
    struct CallContext *parent;
    int seqnum;
    bool used;
    void *data;
    GList *wireList;
}
CallContext, *PtrCallContext;

typedef GList *PtrCallContextList;

extern CallContext *CallContext_Add (tIdent ident, tPosition callerPosition, tPosition calleePosition, GList * wireList);
extern CallContext *CallContext_AddCopy (CallContext * source);
void CallContext_SetParent (CallContext * callContext, CallContext * parent);
extern void CallContext_Print (FILE * stream);
extern void CallContext_Str (FILE * stream);
extern void CallContext_SetWirePositions (PtrWireList wires, PtrProcedure procedure, CallContext * id);
extern GList *CallContext_GetUsedList (PtrWireList wires);
extern void CallContext_TranslateWires (PtrWireList wires, CallContext * oldCallContext, CallContext * newCallContext);
extern void CallContext_MultipleTranslateWires (PtrWireList wires, GList * callcontexts);
extern void CallContext_DuplicateWirePositions (PtrWireList wires,
  tIdent ident, tPosition position, PtrExprAttributesList attr_ports, PtrWireList oldWires);
extern void CallContext_Str_Proc (FILE * stream, PtrProcedure proc);
extern void CallContext_Str_Reset (void);
extern void CallContext_AfterOptimisation (void);

#endif
