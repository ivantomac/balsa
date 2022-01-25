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

  `flatten.c'
  Flatten procedure/function calls

*/

#include <stdlib.h>
#include <string.h>
#include "flatten_breezefiles.h"
#include "flatten_flatten.h"
#include "libbreeze.h"

gboolean save_channel_numbers = FALSE; //TRUE; //FALSE;

int max_contextNum = 0;

#define TOPUP_MAX_CONTEXT(num) if (max_contextNum<num) max_contextNum=num;

/*
Algorithm:
 * Flatten lilian-complete-dump for the calls to parameterised procs and local procs (= calls that are already flattened by balsa-c at the comp&chan level) (file flattenparam.c):
  For each context:
   - Find the associated procedure-decl or procedure-param-alias-decl dump (it can be in another .lilian.breeze file)
     - If it is procedure-param-alias-decl, follow the alias and find the associated procedure-decl
     - If it comes from another file, flatten the lilian-complete-dump of this file (if not already been done before), and use the new procdecl dump
   - Duplicate it
   - Change the label "procedure-decl" into "instantiated-procedure-decl", for better readability
   - Change the position'context numbers to the correct value
   - Insert the new tree at the correct place, just after the fct call

 * Flatten the other calls (file flatten.c):
  For each function call (obtained by reading the component section)
   A) Flatten components&channels
    - Find the associated procdecl comps&chans (it can be in another file)
    - Flatten their breezeFile if not already been done
    - Duplicate them
    - Shift chan numbers in both of them
    - Recursively flatten the fct calls in this procedure
    - Insert the new chans in the caller
    - Insert the new comps in the caller, replacing the fct call
   B) Flatten lilian-complete-dump and contexts
    - Find the associated procdecldump&contexts (At this point they should already be fully flattened)
    - Duplicate them
    - Shift context numbers everywhere
    - Insert them in the caller, replacing the fct call

 * Remove every "not-instantiated" procedure decl from lilian-complete-dump (maybe useless, as anyway, no channel is going to reference these not-instantiated procedure decls, so they are going to be pruned by the visualisation system)
*/

#define BEGINS_WITH(x,y) (!strncmp(x,y,strlen(y)))

struct BreezePart *getPartByName_notRecursive (char *name, struct BreezeFile *breezeFile)
{
    GList *tmp;

    for (tmp = getBreezeParts (breezeFile); tmp; tmp = tmp->next)
    {
        struct BreezePart *part = (struct BreezePart *) tmp->data;

        if (!strcmp (getBreezePartName (part), name))
        {
            if (save_channel_numbers)
                saveChannelNumbers (part);
            return part;
        }
    }
    return NULL;
}

struct BreezePart *getPartByName (char *name, struct BreezeFile *breezeFile)
{
    struct BreezePart *foundPart;
    GList *tmp;

    // Step 1: Search in the file itself
    foundPart = getPartByName_notRecursive (name, breezeFile);
    if (foundPart)
        return foundPart;

    //Step 2: Search in the imported files
    for (tmp = getBreezeImports (breezeFile); tmp; tmp = tmp->next)
    {
        struct BreezeImport *import = tmp->data;
        char *importName = getBreezeImportName (import);
        struct BreezeFile *importedBreezeFile = getImportedBreezeFileByName (importName);

        if (importedBreezeFile)
        {
            foundPart = getPartByName (name, importedBreezeFile);
            if (foundPart)
                return foundPart;
        } else
            fprintf (stderr, "Warning: Imported breeze file not found: %s\n", importName);
    }

    return NULL;
}

void shiftChannelsInNode (GList * channelListNode, int shiftVal, int portChannels[], int nbPortChannels)
{
    GList *tmp;

    for (tmp = channelListNode; tmp; tmp = tmp->next)
    {
        PtrTMPNode node = (PtrTMPNode) tmp->data;

        if (node->type == TMPList)
            shiftChannelsInNode (node->body.list, shiftVal, portChannels, nbPortChannels);
        else if (node->type == TMPNumberStr)
        {
            int number = atoi (node->body.string);

            if (number <= nbPortChannels)
                number = portChannels[number - 1];
            else
                number += shiftVal;
            if (node->body.string)
                free (node->body.string);
            node->body.string = g_strdup_printf ("%d", number);
        }
    }
}

void recursiveFillNodePorts (PtrTMPNode node, int portChannels[], int *nbPortChannels)
{
    switch (node->type)
    {
    case TMPNumberStr:
        portChannels[*nbPortChannels] = atoi (node->body.string);
        (*nbPortChannels)++;
        break;

    case TMPList:
        {
            GList *tmp;

            for (tmp = node->body.list; tmp; tmp = tmp->next)
            {
                PtrTMPNode node = (PtrTMPNode) tmp->data;

                recursiveFillNodePorts (node, portChannels, nbPortChannels);
            }
        }
        break;

    default:
        fprintf (stderr, "Error in fillNodePorts: node should be of type TMPList or TMPNumberStr\n");
        exit (EXIT_FAILURE);
    }
}

void ShiftedCallcontextChannelPorts (GList * callcontexts, int shiftVal, int portChannels[], int nbPortChannels)
{
    GList *tmp;

    for (tmp = callcontexts; tmp; tmp = tmp->next)
    {
        struct BreezePartCallcontextsCallcontext *callcontext = tmp->data;
        GList *ports = getBreezePartCallcontextsCallcontextChannelList (callcontext);

        for (; ports; ports = ports->next)
        {
            long number = (long) ports->data;

            if (number <= nbPortChannels)
            {
                if (number >= 0)
                    number = portChannels[number - 1];
            } else
                number += shiftVal - nbPortChannels;

            ports->data = (gpointer) number;
        }
    }
}

GList *shiftedCopy (GList * components, int shiftVal, struct BreezePartComponentsComponent *callingComp, int portChannels[], int nbPortChannels)
{
    GList *newCompList = g_list_copy (components);
    GList *tmp;

#ifdef _DEBUG
    printf ("ShiftedCopy by %d for ", shiftVal);
    dumpBreezePartComponentsComponent (stdout, callingComp);
#endif

    for (tmp = newCompList; tmp; tmp = tmp->next)
    {
        struct BreezePartComponentsComponent *comp = (struct BreezePartComponentsComponent *) tmp->data;
        struct BreezePartComponentsComponent *newComp;
        PtrTMPNode compNode;

        newComp = DeepCopyBreezePartComponentsComponent (comp);
        tmp->data = newComp;

        compNode = getBreezePartComponentsComponentTMPNode (newComp);

        // Example: compNode = (component "$BrzEncode" (4 9 "8;7;3;4;5;6;2;1;0") ((2554 2559 2560 2563 2567 2558 2555 2557 2556) 3691))

        PtrTMPNode channelListNode = compNode->body.list->next->next->next->data;

        if (channelListNode->type != TMPList)
        {
            fprintf (stderr, "Error in shiftedCopy: channelListNode is not of type TMPList\n");
            exit (EXIT_FAILURE);
        }
        shiftChannelsInNode (channelListNode->body.list, shiftVal, portChannels, nbPortChannels);
    }

    return newCompList;
}

GList *ShiftedCopyChanContextNums (GList * chans, int contextShift)
{
    GList *newChanList = g_list_copy (chans);
    GList *tmp;

#ifdef _DEBUG
    fprintf (stderr, "ShiftedCopyChanContextNums by %d for a list of %d channels\n", contextShift, g_list_length (newChanList));
#endif

    for (tmp = newChanList; tmp; tmp = tmp->next)
    {
        struct BreezePartChannelsChannel *chan = tmp->data;
        struct BreezePartChannelsChannel *newChan = DeepCopyBreezePartChannelsChannel (chan);
        struct Position *pos = getBreezePartChannelsChannelPosition (newChan);

        tmp->data = newChan;
        pos->context += contextShift;
        TOPUP_MAX_CONTEXT (pos->context);
        setBreezePartChannelsChannelPosition (newChan, pos);
        // dumpBreezePartChannelsChannel (stderr, newChan);
    }

    return newChanList;
}

GList *ShiftedCopyCallcontextNums (GList * callcontexts, int contextShift)
{
    GList *newCallcontextList = g_list_copy (callcontexts);
    GList *tmp;

#ifdef _DEBUG
    fprintf (stderr, "ShiftedCopyCallcontextContextNums by %d for a list of %d callcontexts\n", contextShift, g_list_length (newCallcontextList));
#endif

    for (tmp = newCallcontextList; tmp; tmp = tmp->next)
    {
        struct BreezePartCallcontextsCallcontext *callcontext = tmp->data;
        struct BreezePartCallcontextsCallcontext *newCallcontext = DeepCopyBreezePartCallcontextsCallcontext (callcontext);
        int num = getBreezePartCallcontextsCallcontextNum (newCallcontext);
        int parentNum = getBreezePartCallcontextsCallcontextParentNum (newCallcontext);

        tmp->data = newCallcontext;
        /*
           struct Position *pos = getBreezePartCallcontextsCallcontextPosition (newCallcontext);
           pos->context += contextShift;
           TOPUP_MAX_CONTEXT (pos->context); 
         */

        setBreezePartCallcontextsCallcontextNum (newCallcontext, num + contextShift);
        setBreezePartCallcontextsCallcontextParentNum (newCallcontext, parentNum + contextShift);
        // dumpBreezePartCallcontextsCallcontext (stderr, newCallcontext);
    }
    return newCallcontextList;
}

void shiftContextNumsInPositionsOfTMPNode (PtrTMPNode baseNode, int contextShift)
{
    if (!baseNode)
        return;
    else
        switch (baseNode->type)
        {
        case TMPNumberStr:
        case TMPString:
        case TMPSymbol:
        case TMPBoolean:
            return;
        case TMPList:
            {
                GList *listIter = baseNode->body.list;

                if (listIter)
                {
                    PtrTMPNode firstElt = listIter->data;

                    if (firstElt->type == TMPSymbol
                      && !strcmp (firstElt->body.string, "at") && listIter->next
                      && listIter->next->next && listIter->next->next->next && listIter->next->next->next->next)
                    {
                        PtrTMPNode fifthElt = listIter->next->next->next->next->data;

                        if (fifthElt->type == TMPNumberStr)
                        {
                            int num = atoi (fifthElt->body.string);

                            //       if (num == oldBaseContextNum  ||  num == 0)
                            fifthElt->body.string = g_strdup_printf ("%d", num + contextShift);
                        }
                    }
                }

                while (listIter)
                {
                    shiftContextNumsInPositionsOfTMPNode (listIter->data, contextShift);
                    listIter = listIter->next;
                }
            }
        }
    return;
}

/*
PtrTMPNode FindTMPNodeCorrespondingToCallerComponent_inner (PtrTMPNode baseNode, struct Position *position, PtrTMPNode parentNode)
{
    if (!baseNode)
	return NULL;
    else
        switch (baseNode->type)
        {
        case TMPNumberStr:
        case TMPString:
        case TMPSymbol:
        case TMPBoolean:
	    return NULL;
        case TMPList:
            {
                GList *listIter = baseNode->body.list;

		if (listIter) {
		    PtrTMPNode firstElt = listIter->data;
		    if (firstElt->type==TMPSymbol && !strcmp(firstElt->body.string,"at") && listIter->next && listIter->next->next->next->next) {
			PtrTMPNode secondElt = listIter->next->data;
			PtrTMPNode thirdElt = listIter->next->next->data;
			PtrTMPNode fifthElt = listIter->next->next->next->next->data;

			if (secondElt->type==TMPNumberStr && thirdElt->type==TMPNumberStr) {
			    int line = atoi (secondElt->body.string);
			    int column = atoi (thirdElt->body.string);
			    int context = atoi (fifthElt->body.string);
			    if (line==position->line && column==position->column && context==position->context)
				return parentNode;
			}
		    }
		}

                while (listIter)
                {
		    PtrTMPNode res = FindTMPNodeCorrespondingToCallerComponent_inner (listIter->data, position, baseNode);
		    if (res)
			return res;
                    listIter = listIter->next;
                }
            }
        }
    return NULL;
}
PtrTMPNode FindTMPNodeCorrespondingToCallerComponent (struct BreezePart *part, struct BreezePartComponentsComponent *callerComp)
{
    PtrTMPNode nodeCallerComp = getBreezePartComponentsComponentTMPNode (callerComp);
    int numChan = atoi (((PtrTMPNode)((PtrTMPNode)nodeCallerComp->body.list->next->next->next->data)->body.list->data)->body.string);
    GList *chans = getBreezePartChannelsList(part);
    int numChan2 = g_list_length(chans)-numChan;
    GList *chanElt = g_list_nth (chans, numChan2);
    struct BreezePartChannelsChannel *chan = chanElt->data;
    struct Position *position = getBreezePartChannelsChannelPosition (chan);
    PtrTMPNode baseNode = getBreezeCallContextcompletedumpNode (getBreezePartFile (part));
    return FindTMPNodeCorrespondingToCallerComponent_inner (baseNode, position, NULL);
}
 */

/*
void insertCallWithTMPNodeAndChangeName (PtrTMPNode originalNode, PtrTMPNode newNode)
{
    GList *list;

    if (originalNode->type!=TMPList || newNode->type!=TMPList) {
	fprintf (stderr, "Error in insertWithTMPNode\n");
	exit (-1);
    }

    list = g_list_prepend (NULL, newNode);
    list = g_list_prepend (list, NewTMPList(originalNode->body.list));
    originalNode->body.list = list;
}
*/

/*
void FlattenNotParameterisedProcCall_LilianCompleteDump (struct BreezePart *part, struct BreezePartComponentsComponent *callerComp, int contextShift, struct BreezeFile *candidateFile)
{
    //    - Find the associated procdecldump&contexts (At this point they should already be fully flattened)
    char *name;
    PtrTMPNode procDump;

    name = getBreezePartComponentsComponentName (callerComp);
 retry_because_alias:
    procDump = getProcDumpByNameAndContextNum (name, -1, candidateFile, FALSE);

    if (!procDump) {
	fprintf (stderr, "Error in FlattenNotParameterisedProcCall_LilianCompleteDump: We don't find %s\n", name);
	return;
    }

    // If it is procedure-alias-decl, follow the alias and find the associated procedure-decl
    {
	PtrTMPNode label = procDump->body.list->data;
	if (!strcmp(label->body.string,"procedure-alias-decl") || !strcmp(label->body.string,"procedure-param-alias-decl")) {
	    PtrTMPNode newProcName = procDump->body.list->next->next->next->data;
	    if (debug) {
		fprintf (stderr, "Debug Info: FlattenNotParameterisedProcCall_LilianCompleteDump: alias detected. \"%s\" => \"%s\"\n", name, newProcName->body.string);
	    }
	    name = newProcName->body.string;
	    goto retry_because_alias;
	}
    }

    //    - Duplicate them
    PtrTMPNode newProcDump = DeepCopyTMPNode (procDump);

    //    - Shift context numbers everywhere
    shiftContextNumsInPositionsOfTMPNode (newProcDump, contextShift);

    PrintTMPNode (stderr, newProcDump);
    fprintf(stderr,"\n");

    //    - Insert them in the caller, replacing the fct call
    PtrTMPNode call = FindTMPNodeCorrespondingToCallerComponent (part, callerComp);
    if (call)
	insertWithTMPNode (call, newProcDump);
    else
	fprintf (stderr, "Error in FlattenNotParameterisedProcCall_LilianCompleteDump: TMPNode corresponding to caller component not found\n");
}
*/

void ChangeListOfPortsOfLastCallcontext (GList * callcontexts, int portChannels[], int nbPortChannels, int parentContextNum, int baseContextNum)
{
    struct BreezePartCallcontextsCallcontext *callcontext;
    GList *list = NULL;
    int i;

    GList *tmp;

    //    fprintf(stderr,"\n-----\n");
    for (tmp = g_list_last (callcontexts); tmp; tmp = tmp->prev)
    {
        callcontext = tmp->data;
        // dumpBreezePartCallcontextsCallcontext(stderr,callcontext);
        int parentNum = getBreezePartCallcontextsCallcontextParentNum (callcontext);

        if (parentNum == baseContextNum)
            break;
    }
    //    fprintf(stderr,"\n-----\n");

#ifdef _DEBUG
    fprintf (stderr, "ChangeListOfPortsOfLastCallcontext for %d ports and parentcontextnum=%d\n", nbPortChannels, parentContextNum);
#endif

    for (i = 0; i < nbPortChannels; i++)
        list = g_list_prepend (list, (gpointer) (long) portChannels[i]);
    setBreezePartCallcontextsCallcontextChannelList (callcontext, list);
    setBreezePartCallcontextsCallcontextParentNum (callcontext, parentContextNum);
}

gboolean FlattenNotParameterisedProcCall_CompsAndChans (struct BreezePart *part, struct BreezePartComponentsComponent *callerComp)
{
/* Extract the callerContextNum, useful later when position have been overwritten */
    GList *chans = getBreezePartChannelsList (part);
    PtrTMPNode callerNode = getBreezePartComponentsComponentTMPNode (callerComp);
    PtrTMPNode tmp = (PtrTMPNode) callerNode->body.list->next->next->next->data;
    int num = atoi (((PtrTMPNode) tmp->body.list->data)->body.string);
    struct BreezePartChannelsChannel *chan = g_list_nth (chans, g_list_length (chans) - num)->data;
    struct Position *pos = getBreezePartChannelsChannelPosition (chan);
    int callerContextNum = pos->context;

    //    - Find the associated procdecl comps&chans (it can be in another file)
    char *name = getBreezePartComponentsComponentName (callerComp);
    struct BreezePart *subPart = getPartByName (name, getBreezePartFile (part));
    gboolean ret = FALSE;
    gboolean partIsBuiltin = FALSE;

    if (subPart)
    {                           /* Determine if this subPart is a builtin function */
        GList *attributes = getBreezePartAttributesList (subPart);

        while (attributes)
        {
            char *attributeName = getBreezePartAttributesAttributeName ((struct BreezePartAttributesAttribute *) attributes->data);

            if (strcmp (attributeName, "is-builtin-function") == 0)
            {
                partIsBuiltin = TRUE;
                break;
            }
            attributes = g_list_next (attributes);
        }
    } else
    {
        /* FIXME, AB */
        fprintf (stderr, "Can't find part `%s', won't try to expand.\n", name);
        return FALSE;
        // exit (EXIT_FAILURE);
    }

#ifdef _DEBUG
    fprintf (stderr, "Flattening procedure call to %s\n", name);
#endif

    if (subPart && !partIsBuiltin)
    {
        //    - Flatten their breezeFile if not already been done
        // TODO;
        flattenProcedureCalls (subPart);

        // Look for the max context num in the part
        GList *tmp = getBreezePartCallcontextsList (part);

        for (; tmp; tmp = tmp->next)
        {
            struct BreezePartCallcontextsCallcontext *callcontext = tmp->data;
            int num = getBreezePartCallcontextsCallcontextNum (callcontext);

            TOPUP_MAX_CONTEXT (num);
        }

        //  Get the port list
        int portChannels[1000];
        int nbPortChannels = 0;
        PtrTMPNode callerNode = getBreezePartComponentsComponentTMPNode (callerComp);
        PtrTMPNode portListNode = callerNode->body.list->next->next->next->data;

        recursiveFillNodePorts (portListNode, portChannels, &nbPortChannels);

        //    - Duplicate channels
        //    - Shift chan numbers in both of them
        int shiftVal = g_list_length (getBreezePartChannelsList (part));
        GList *newComps = shiftedCopy (getBreezePartComponentsList (subPart),
          shiftVal - nbPortChannels, callerComp,
          portChannels,
          nbPortChannels);

        //    - Recursively flatten the fct calls in this procedure
        // TODO;

        //    - Insert the new comps in the caller, replacing the fct call
        newComps = g_list_concat (newComps, getBreezePartComponentsList (part));
        setBreezePartComponentsList (part, newComps);
        // TODO_replace_fctcall;

        // If we are doing the callcontext expansion, shift the context nums of the channels
        int contextShift = max_contextNum + 1;
        GList *newChans = ShiftedCopyChanContextNums (getBreezePartChannelsList (subPart),
          contextShift);

        // Skip the ports (memory leak, but we don't care)
        {
            GList *tmp = g_list_nth (newChans,
              g_list_length (newChans) - nbPortChannels - 1);

            if (tmp)
                tmp->next = 0;
            else
                newChans = NULL;
            //newChans = g_list_nth (newChans, nbPortChannels);
        }

        // Adjust the position of the channel which became the activation port of the subpart
        {
            GList *chans = getBreezePartChannelsList (part);
            struct BreezePartChannelsChannel *chan = g_list_nth (chans, g_list_length (chans) - portChannels[0])->data;

            GList *chans2 = getBreezePartChannelsList (subPart);
            struct BreezePartChannelsChannel *chan2 = g_list_last (chans2)->data;

            struct Position *pos = getBreezePartChannelsChannelPosition (chan2);

            pos = copyPosition (pos);
            pos->context += contextShift;
            TOPUP_MAX_CONTEXT (pos->context);

            setBreezePartChannelsChannelPosition (chan, copyPosition (pos));
        }

        //    - Insert the new chans in the caller
        //  breezePart->nbChans += bp->nbChans;
        // TOverify;
        setBreezePartChannelsList (part, g_list_concat (newChans, getBreezePartChannelsList (part)));

        // Shift the context nums of the callcontexts
        GList *newCallcontexts = ShiftedCopyCallcontextNums (getBreezePartCallcontextsList (subPart),
          contextShift);

        // Shift chan numbers of ports associated to the callcontexts
        ShiftedCallcontextChannelPorts (newCallcontexts, shiftVal, portChannels, nbPortChannels);

        // Get the first context's parentnum
        int mainContextNum = callerContextNum;

/*
        {
            GList *partCallcontexts = getBreezePartCallcontextsList (part);

            if (partCallcontexts)
            {
                struct BreezePartCallcontextsCallcontext *callcontext = g_list_last (partCallcontexts)->data;
//here is the crap
                mainContextNum = getBreezePartCallcontextsCallcontextNum (callcontext);

            GList *chans = getBreezePartChannelsList (part);
                PtrTMPNode callerNode = getBreezePartComponentsComponentTMPNode (callerComp);
                PtrTMPNode tmp = (PtrTMPNode)callerNode->body.list->next->next->next->data;
            int num = atoi (((PtrTMPNode)tmp->body.list->data)->body.string);
            struct BreezePartChannelsChannel *chan = g_list_nth (chans, g_list_length (chans) - num)->data;

            struct Position *pos = getBreezePartChannelsChannelPosition (chan);
                mainContextNum = pos->context;

            }
        }
*/

        if (newCallcontexts)
        {
            // Change the list of ports of the first callcontext, supposed to be the main one (which corresponds to the function being called)
            // And adjust the parent context num
            ChangeListOfPortsOfLastCallcontext (newCallcontexts, portChannels, nbPortChannels, mainContextNum, contextShift);

            // Insert the new callcontexts in the caller
            setBreezePartCallcontextsList (part, g_list_concat (newCallcontexts, getBreezePartCallcontextsList (part)));
        }

        ret = TRUE;             /* Have successfully flattened component */
    } else
    {
        if (!subPart)
        {
            fprintf (stderr, "Error: Breeze part not found: %s\n", name);
            exit (EXIT_FAILURE);
        }
    }

    return ret;
}

void FlattenNotParameterisedProcCalls (struct BreezePart *part)
{
    GList *componentsList = getBreezePartComponentsList (part);
    GList *tmp = g_list_last (componentsList);

    while (tmp)
    {
        struct BreezePartComponentsComponent *comp = tmp->data;
        char *name = getBreezePartComponentsComponentName (comp);

        if (!BEGINS_WITH (name, "$Brz") && !getBreezePartComponentsComponentIsUndeclared (comp))
        {
            gboolean haveFlattenedComponent = FlattenNotParameterisedProcCall_CompsAndChans (part, comp);

            //     FlattenNotParameterisedProcCall_LilianCompleteDump (part, comp);

            if (haveFlattenedComponent)
            {
                // Remove this component from the list
                GList *next = tmp->prev;

                componentsList = getBreezePartComponentsList (part);
                componentsList = g_list_remove_link (componentsList, tmp);
                setBreezePartComponentsList (part, componentsList);
                tmp = next;
                continue;
            }
        }

        tmp = tmp->prev;
    }
}

void flattenProcedureCalls (struct BreezePart *part)
{
    static GList *filesInProcess = NULL;
    struct BreezeFile *file = getBreezePartFile (part);

    /*
       GList *tmp;

       for (tmp=filesInProcess; tmp; tmp=tmp->next) {
       struct BreezeFile *f = tmp->data;
       if (f==file)
       return;
       }

     */
    filesInProcess = g_list_prepend (filesInProcess, file);

    FlattenNotParameterisedProcCalls (part);

    filesInProcess = g_list_remove (filesInProcess, file);
}
