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

#include "callcontexts.h"

//#define DEBUG_CALLCONTEXTS

GList *CallContextDecls = NULL;
GPtrArray *CallContexts = NULL;

CallContext *CallContext_Add (tIdent ident, tPosition callerPosition, tPosition calleePosition, GList * wireList)
{
    static int id = 0;
    CallContext *newCallContext;

    id++;
    if (!CallContexts)
    {
        CallContexts = g_ptr_array_new ();
        g_ptr_array_add (CallContexts, NULL);
    }

    newCallContext = g_new0 (CallContext, 1);
    newCallContext->calledProcIdent = ident;
    newCallContext->callerPosition = callerPosition;
    newCallContext->calleePosition = calleePosition;
    newCallContext->wireList = g_list_copy (wireList);

    static int seqnum = 0;

    seqnum++;
    newCallContext->seqnum = seqnum;

    g_ptr_array_add (CallContexts, newCallContext);

    return newCallContext;
}

CallContext *CallContext_AddCopy (CallContext * source)
{
    CallContext *newCallContext = CallContext_Add (source->calledProcIdent, source->callerPosition,
      source->calleePosition, source->wireList);

    newCallContext->parent = source->parent;
    newCallContext->callerPosition.CallContext = newCallContext->parent;
    return newCallContext;
}

void CallContext_SetParent (CallContext * callContext, CallContext * parent)
{
    while (callContext->parent && callContext->parent != parent)
    {
        callContext = callContext->parent;
    }

    if (callContext->parent == 0 && callContext != parent)
    {
        callContext->parent = parent;
        callContext->callerPosition.CallContext = callContext->parent;
    }
}

void CallContext_Print (FILE * stream)
{
    int i;

    for (i = 1; i < CallContexts->len; i++)
    {
        CallContext *callContext = g_ptr_array_index (CallContexts, i);

        fprintf (stream,
          "%d:%s (at %d %d %d) called from (%d %d %d), inside parent %d\n",
          i, PeekString (callContext->calledProcIdent),
          callContext->calleePosition.Line,
          callContext->calleePosition.Column,
          (int) (long) callContext->calleePosition.CallContext,
          callContext->callerPosition.Line,
          callContext->callerPosition.Column, (int) (long) callContext->callerPosition.CallContext, (int) (long) callContext->parent);
    }
}

void CallContext_Str (FILE * stream)
{
    int i;
    int nbUsedCallContexts = 0;

    if (CallContexts)
    {
        for (i = 1; i < CallContexts->len; i++)
        {
            CallContext *callContext = g_ptr_array_index (CallContexts, i);

            if (callContext->used)
            {
                nbUsedCallContexts++;
                while (callContext->parent)
                {
                    callContext = callContext->parent; //g_ptr_array_index (CallContexts, callContext->parent);
                    callContext->used = true;
                }
            }
        }
    }

    if (nbUsedCallContexts)
    {
        fprintf (stream, "(callContext-contexts\n");

        for (i = 1; i < CallContexts->len; i++)
        {
            CallContext *callContext = g_ptr_array_index (CallContexts, i);

            if (callContext->used)
            {
                fprintf (stream, "  (callContext-context %d \"%s\" ", i, PeekString (callContext->calledProcIdent));
                StrSBreezePosition (stream, callContext->callerPosition);
                fprintf (stream, " ");
                StrSBreezePosition (stream, callContext->calleePosition);
                fprintf (stream, " %d)\n", callContext->parent ? callContext->parent->seqnum : 0);
            }
        }

        fprintf (stream, ")\n");
    }
}

void CallContext_SetWirePositions (PtrWireList wires, PtrProcedure procedure, CallContext * id)
{
    PtrWire wire = CAR (wires);

    if (wire->position.CallContext == 0)
        wire->position.CallContext = id;
    else
        CallContext_SetParent (wire->position.CallContext, id);

    if (CDR (wires))
        CallContext_SetWirePositions (CDR (wires), procedure, id);
}

GList *CallContext_GetUsedList (PtrWireList wires)
{
    GList *list = NULL, *tmp;

    if (CDR (wires))
        list = CallContext_GetUsedList (CDR (wires));

    PtrWire wire = CAR (wires);
    CallContext *callContext = wire->position.CallContext;

    for (tmp = list; tmp; tmp = tmp->next)
    {
        CallContext *val = tmp->data;

        if (callContext == val)
            return list;
    }

    if (callContext)
        list = g_list_prepend (list, (void *) callContext);
    return list;
}

void CallContext_TranslateWires (PtrWireList wires, CallContext * oldCallContext, CallContext * newCallContext)
{
    if (!wires)
        return;
    PtrWire wire = CAR (wires);
    CallContext *callContext = wire->position.CallContext;

    //    printf("pos callc is %d\n", callContext);
    if (callContext == oldCallContext)
    {
        wire->position.CallContext = newCallContext;
        // printf("match!\n");
    }

    if (CDR (wires))
        CallContext_TranslateWires (CDR (wires), oldCallContext, newCallContext);
}

void CallContext_MultipleTranslateWires (PtrWireList wires, GList * callcontexts)
{
    GList *tmp = callcontexts;

    for (; tmp; tmp = tmp->next)
    {
        CallContext *callcontext = tmp->data;

#ifdef DEBUG_CALLCONTEXTS
        printf ("try %d -> %d on %x (%d)\n", callcontext->data, callcontext, wires, g_list_length (wires));
#endif
        CallContext_TranslateWires (wires, callcontext->data, callcontext);
    }
}

void CallContext_DuplicateWirePositions (PtrWireList wires, tIdent ident, tPosition position, PtrExprAttributesList attr_ports, PtrWireList oldWires)
{
    GList *usedCallContexts;
    GList *newCallContexts;
    GList *tmp;
    CallContext *mainCallContext;
    CallContext *callContext;

#ifdef DEBUG_CALLCONTEXTS
    printf ("duplicate\n");
#endif
    /* Get the list of the callContexts used to describe this piece of circuit */
    usedCallContexts = CallContext_GetUsedList (wires);
    newCallContexts = NULL;

    /* Find the id of the main callContext */
    mainCallContext = CAR (wires)->position.CallContext;

    if (!CallContexts)
        return;                 // flattened compilation problem needing to be fixed
    callContext = mainCallContext;
    if (!callContext)
        return;                 // flattened compilation problem needing to be fixed
    while (callContext->parent > 0)
    {
        mainCallContext = callContext->parent;
        callContext = mainCallContext;
    }

    /* Ports handling */
    // StrPtrWire (stdout, CAR (attr_ports).activation);
    // printf("\n");
    GList *newWireList = NULL;
    PtrWireList last = wires;

    while (CDR (last))
        last = CDR (last);
    newWireList = g_list_prepend (newWireList, CAR (last));
    /*
       printf("add to %d: %d: ", newWireList, (int)CAR(last));
       StrPtrWire (stdout, CAR (last));
       printf("\n");
     */

    while (attr_ports)
    {
        newWireList = g_list_prepend (newWireList, CAR (attr_ports).activation);
        /*
           printf("add to %d: %d: ", newWireList, (int)CAR (attr_ports).activation);
           StrPtrWire (stdout, CAR (attr_ports).activation);
           printf("\n");
         */
        attr_ports = CDR (attr_ports);
    }

    /* Translate each callContext into a new one and update the wires */
    /* The main callContext is not only translated: it's caller position is changed to the new caller */
    for (tmp = usedCallContexts; tmp; tmp = tmp->next)
    {
        CallContext *callContext = tmp->data;
        CallContext *newCallContext;

        // printf("Processing callcontext %s\n", PeekString(callContext->calledProcIdent));

        if (callContext == mainCallContext)
        {
            newCallContext = CallContext_Add (ident, position, NoPosition, newWireList);
        } else
        {
            newCallContext = CallContext_AddCopy (callContext);

            /* Translation of ports from pointing to old wires to pointing to new wires */
            // newCallContext->wireList = g_list_copy (newCallContext->wireList);

            GList *tmp2 = newCallContext->wireList;

            for (; tmp2; tmp2 = tmp2->next)
            {
                PtrWire port = tmp2->data;
                PtrWireList ptr1 = oldWires;
                PtrWireList ptr2 = wires;

                while (ptr1 && ptr2 && CAR (ptr1) != port)
                {
                    ptr1 = CDR (ptr1);
                    ptr2 = CDR (ptr2);
                }
                if (ptr1 && ptr2)
                {
                    tmp2->data = CAR (ptr2);
                    /*
                       printf("GOOOOOD: %d->%d: ", (int)port, (int)CAR(ptr2));
                       StrPtrWire (stdout, port);
                       printf("\n");
                     */
                }
                /*
                   else {
                   printf("OOOOOOOOOPS: %d: ", (int)port);
                   StrPtrWire (stdout, port);
                   printf("\n");
                   }
                 */
            }
        }

#ifdef DEBUG_CALLCONTEXTS
        printf ("translate %d->%d\n", (int) callContext, (int) newCallContext);
#endif
        newCallContexts = g_list_append (newCallContexts, (void *) newCallContext);
        CallContext_TranslateWires (wires, callContext, newCallContext);
    }

    for (tmp = newCallContexts; tmp; tmp = tmp->next)
    {
        CallContext *callContext = tmp->data;
        GList *tmp1, *tmp2;

        for (tmp1 = usedCallContexts, tmp2 = newCallContexts; tmp1; tmp1 = tmp1->next, tmp2 = tmp2->next)
        {
            CallContext *callContext1 = tmp1->data;
            CallContext *callContext2 = tmp2->data;

            if (callContext->parent == callContext1)
            {
                callContext->parent = callContext2;
                callContext->callerPosition.CallContext = callContext->parent;
            }
        }
    }
}

void CallContext_Str_Proc (FILE * stream, PtrProcedure proc)
{
    int i;
    int nbUsedCallContexts = 0;

    if (CallContexts)
    {
        for (i = 1; i < CallContexts->len; i++)
        {
            CallContext *callContext = g_ptr_array_index (CallContexts, i);

            if (callContext->used)
            {
                nbUsedCallContexts++;
                while (callContext->parent)
                {
                    callContext = callContext->parent;
                    callContext->used = true;
                }
            }
        }
    }

    if (nbUsedCallContexts)
    {
        fprintf (stream, "  (call-contexts\n");

        for (i = 1; i < CallContexts->len; i++)
        {
            CallContext *callContext = g_ptr_array_index (CallContexts, i);

            if (callContext->used)
            {
                fprintf (stream, "    (call-context %d \"%s\" ", callContext ? callContext->seqnum : 0, PeekString (callContext->calledProcIdent));
                StrSBreezePosition (stream, callContext->callerPosition);
                fprintf (stream, " ");
                StrSBreezePosition (stream, callContext->calleePosition);
                fprintf (stream, " %d", callContext->parent ? callContext->parent->seqnum : 0);

                int nbPorts = 0;
                GList *wireList = callContext->wireList;

                GList *tmp;

                for (tmp = wireList; tmp; tmp = tmp->next)
                {
                    //      PtrWire wire = tmp->data;
                    //      if (wire->bundleNo)
                    nbPorts++;
                }
                if (nbPorts)
                {
                    fprintf (stream, " (port-channels");
                    for (tmp = wireList; tmp; tmp = tmp->next)
                    {
                        PtrWire wire = tmp->data;

                        /*
                           printf("out:");
                           StrPtrWire (stdout, wire);
                           printf("\n");
                         */
                        if (wire->bundleNo)
                            fprintf (stream, " %d", wire->bundleNo);
                        else if (wire->replacementWire)
                            fprintf (stream, " %d", wire->replacementWire->bundleNo);
                        else
                            fprintf (stream, " -1");
#ifdef DEBUG_CALLCONTEXTS
                        fprintf (stream, "(%d)", (int) wire);
#endif
                    }
                    fprintf (stream, ")");
                }

                fprintf (stream, ")\n");
            }
            /*
               if (callContext->wireList) {
               PtrWire wire = callContext->wireList->data;
               printf("port (%d, %s, ", callContext->used, PeekString (callContext->calledProcIdent));
               //StrSBreezePosition (stdout, callContext->callerPosition);
               printf ("): ");
               StrPtrWire (stdout, wire);
               printf("\n");
               //  fprintf (stream, " (port-channels %d)", );
               }
             */
        }

        fprintf (stream, "  )\n");
    }
}

void CallContext_Str_Reset (void)
{
    if (CallContexts)
    {
        int i;

        for (i = 1; i < CallContexts->len; i++)
        {
            CallContext *callContext = g_ptr_array_index (CallContexts, i);

            callContext->used = false;
        }
    }
}

void CallContext_AfterOptimisation (void)
{
    if (CallContexts)
    {
        int i;

        for (i = 1; i < CallContexts->len; i++)
        {
            CallContext *callContext = g_ptr_array_index (CallContexts, i);
            GList *tmp = callContext->wireList;

            for (; tmp; tmp = tmp->next)
            {
                PtrWire wire = tmp->data;

                while (wire->bundleNo == 0 && wire->replacementWire)
                    wire = tmp->data = wire->replacementWire;
                wire->replacementWire = 0;
            }
        }
    }
}
