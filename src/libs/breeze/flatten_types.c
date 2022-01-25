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

#include <stdlib.h>
#include <string.h>

#include "flatten_breezefiles.h"
#include "flatten_types.h"
#include "libbreeze.h"
#include "../balsasim/object.h"

char *resolveType_toStr (PtrTMPNode node, int *widthPtr, gboolean resetIndex, struct BreezeFile *baseFile);

char *getTypeFormatByName_notRecursive (char *name, int *widthPtr, struct BreezeFile *breezeFile)
{
    GList *tmp;

    for (tmp = getBreezeTypes (breezeFile); tmp; tmp = tmp->next)
    {
        struct BreezeType *type = (struct BreezeType *) tmp->data;
        char *typeName = getBreezeTypeName (type);

        if (!strcmp (typeName, name))
        {
            PtrTMPNode typeNode = getBreezeTypeTMPNode (type);
            PtrTMPNode typeDeclNode = typeNode->body.list->next->next->data;
            char *typeStr = resolveType_toStr (typeDeclNode, widthPtr, FALSE, breezeFile);

            return typeStr;
        }
    }

    return NULL;
}

char *getTypeFormatByName (char *name, int *widthPtr, struct BreezeFile *breezeFile)
{
    char *foundType;
    GList *tmp;

    // Special case for string type
    if (strcmp (name, "String") == 0)
    {
        return g_strdup ("%!(63:0)ms");
    }
    // Step 1: Search in the file itself
    foundType = getTypeFormatByName_notRecursive (name, widthPtr, breezeFile);
    if (foundType)
        return foundType;

    //Step 2: Search in the imported files
    for (tmp = getBreezeImports (breezeFile); tmp; tmp = tmp->next)
    {
        struct BreezeImport *import = tmp->data;
        char *importName = getBreezeImportName (import);
        struct BreezeFile *importedBreezeFile = getImportedBreezeFileByName (importName);

        if (importedBreezeFile)
        {
            foundType = getTypeFormatByName (name, widthPtr, importedBreezeFile);
            if (foundType)
                return foundType;
        } else
            fprintf (stderr, "Warning: Breeze type not found: %s\n", importName);
    }

    return NULL;
}

char *resolveType_toStr (PtrTMPNode node, int *widthPtr, gboolean resetIndex, struct BreezeFile *baseFile)
{
    char *typeStr = NULL;
    static int typeIndex;

    if (resetIndex)
        typeIndex = 0;

    if (TMPIsHeaded (node, "numeric-type"))
    {
        int signedness = ((PtrTMPNode) (node->body.list->next->data))->body.boolean;
        int width = atoi (((PtrTMPNode) (node->body.list->next->next->data))->body.string);

        typeStr = g_strdup_printf ("%%!%s(%d:%d)mi", signedness ? "-" : "", width + typeIndex - 1, typeIndex);
        typeIndex += width;
        if (widthPtr)
            *widthPtr = width;
    }

    else if (TMPIsHeaded (node, "named-type"))
    {
        char *name = ((PtrTMPNode) (node->body.list->next->data))->body.string;

        typeStr = getTypeFormatByName (name, widthPtr, baseFile);
    }

    else if (TMPIsHeaded (node, "record-type"))
    {
        int width = atoi (((PtrTMPNode) (node->body.list->next->data))->body.string);
        GList *tmp;
        gboolean firstLoop = TRUE;
        char *tmpStr;

        typeStr = g_strdup ("{");

        for (tmp = node->body.list->next->next; tmp; tmp = tmp->next)
        {
            char *insideType;
            PtrTMPNode insideNode = (PtrTMPNode) ((PtrTMPNode) tmp->data)->body.list->next->data;

            insideType = resolveType_toStr (insideNode, widthPtr, FALSE, baseFile);
            tmpStr = g_strdup_printf ("%s%s%s", typeStr, firstLoop ? "" : ",", insideType);
            g_free (typeStr);
            g_free (insideType);
            typeStr = tmpStr;
            firstLoop = FALSE;
        }

        tmpStr = g_strdup_printf ("%s}", typeStr);
        g_free (typeStr);
        typeStr = tmpStr;
        if (widthPtr)
            *widthPtr = width;
    }

    else if (TMPIsHeaded (node, "enumeration-type"))
    {
        int signedness = ((PtrTMPNode) (node->body.list->next->data))->body.boolean;
        int width = atoi (((PtrTMPNode) (node->body.list->next->next->data))->body.string);
        GList *tmp;
        gboolean firstLoop = TRUE;
        char *tmpStr;
        int lastEnumNum = -1;

        typeStr = g_strdup_printf ("%%!%s(%d:%d)me(", signedness ? "-" : "", width + typeIndex - 1, typeIndex);

        for (tmp = node->body.list->next->next->next; tmp; tmp = tmp->next)
        {
            char *enumName = ((PtrTMPNode) ((PtrTMPNode) tmp->data)->body.list->data)->body.string;
            int enumNum = atoi (((PtrTMPNode) ((PtrTMPNode) tmp->data)->body.list->next->data)->body.string);

            if (enumNum == lastEnumNum + 1)
                tmpStr = g_strdup_printf ("%s%s%s", typeStr, firstLoop ? "" : ",", enumName);
            else
                tmpStr = g_strdup_printf ("%s%s%s=%d", typeStr, firstLoop ? "" : ",", enumName, enumNum);

            g_free (typeStr);
            typeStr = tmpStr;
            firstLoop = FALSE;
            lastEnumNum = enumNum;
        }

        tmpStr = g_strdup_printf ("%s)", typeStr);
        g_free (typeStr);
        typeStr = tmpStr;
        typeIndex += width;
        if (widthPtr)
            *widthPtr = width;
    }

    else if (TMPIsHeaded (node, "array-type"))
    {
        int n1 = atoi (((PtrTMPNode) (node->body.list->next->next->data))->body.string);
        int n2 = atoi (((PtrTMPNode) (node->body.list->next->next->next->data))->body.string);
        int i;
        int insideWidth = 0;

        // GList *tmp;
        gboolean firstLoop = TRUE;
        char *tmpStr;

        // int lastEnumNum = -1;
        typeStr = g_strdup_printf ("{");

        for (i = n1; i < n2; i++)
        {
            char *insideType;
            PtrTMPNode insideNode = (PtrTMPNode) node->body.list->next->data;

            insideType = resolveType_toStr (insideNode, &insideWidth, FALSE, baseFile);
            tmpStr = g_strdup_printf ("%s%s%s", typeStr, firstLoop ? "" : ",", insideType);
            g_free (typeStr);
            g_free (insideType);
            typeStr = tmpStr;
            firstLoop = FALSE;
        }

        tmpStr = g_strdup_printf ("%s}", typeStr);
        g_free (typeStr);
        typeStr = tmpStr;
        if (widthPtr)
            *widthPtr = insideWidth * (n2 - n1);
    }

    else if (TMPIsHeaded (node, "builtin-type"))
    {
        /* Don't bother to resolve builtin types, they should never need to be printed anyway */
        typeStr = g_strdup ("builtin-type");
        typeIndex += BALSA_OBJECT_BITS;
        if (widthPtr)
            *widthPtr = BALSA_OBJECT_BITS;
    }

    else
    {
        fprintf (stderr, "Error: Unknown symbol discovered in resolveType_toStr: %s\n", ((PtrTMPNode) (node->body.list->data))->body.string);
        exit (EXIT_FAILURE);
    }

    return typeStr;
}

PtrTMPNode resolveType (PtrTMPNode node, struct BreezeFile * baseFile)
{
    int width = 0;
    char *typeStr = resolveType_toStr (node, &width, TRUE, baseFile);

    if (!typeStr)
    {
        fprintf (stderr, "Error: Unresolved type ");
        PrintTMPNode (stderr, node);
        fprintf (stderr, "\n");
        return node;
    } else
    {
        GList *list = 0;

        list = g_list_prepend (list, NewTMPString (typeStr, -1));
        {
            char *tmpStr = g_strdup_printf ("%d", width);

            list = g_list_prepend (list, NewTMPNumberStr (tmpStr, -1));
            free (tmpStr);
        }
        list = g_list_prepend (list, NewTMPSymbol ("formatted-type", -1));
        return NewTMPList (list);
    }
}

void resolveTypes (struct BreezePart *part)
{
    GList *tmp;

    for (tmp = getBreezePartPortsList (part); tmp; tmp = tmp->next)
    {

        struct BreezePartPortsPort *port = (struct BreezePartPortsPort *) tmp->data;
        PtrTMPNode nodePort = getBreezePartPortsPortTMPNode (port);
        enum BreezePartPortsPortType portType = getBreezePartPortsPortType (port);

        switch (portType)
        {
        case PassiveOutputPort:
        case ActiveOutputPort:
        case PassiveInputPort:
        case ActiveInputPort:
            {
                PtrTMPNode typeNode = nodePort->body.list->next->next->next->next->data;
                PtrTMPNode newTypeNode = resolveType (typeNode, getBreezePartFile (part));

                nodePort->body.list->next->next->next->next->data = newTypeNode;

                setBreezePartPortsPortTMPNode (port, nodePort);
            }
            break;
        case PassiveSyncPort:
        case ActiveSyncPort:
            break;
        default:
            fprintf (stderr, "Error: Undefined port type in resolveTypes\n");
            exit (EXIT_FAILURE);
        }
    }

    for (tmp = getBreezePartComponentsList (part); tmp; tmp = tmp->next)
    {

        //        struct BreezePartComponentsComponent *comp = tmp->data;
        //        char *name = getBreezePartComponentsComponentName (comp);
        /*
           PtrTMPNode nodeComp = getBreezePartComponentsComponentTMPNode (comp);
           PtrTMPNode newNodeComp = DeepCopyTMPNode (nodeComp);
           PtrTMPNode nodeType = ((PtrTMPNode) newNodeComp->body.list->next->next->data)->body.list->next->data;
           PtrTMPNode newNodeType = resolveType (nodeType, getBreezePartFile (part));

           ((PtrTMPNode) newNodeComp->body.list->next->next->data)->body.list->next->data = newNodeType;
           setBreezePartComponentsComponentTMPNode (comp, newNodeComp);
         */
    }

    for (tmp = getBreezePartChannelsList (part); tmp; tmp = tmp->next)
    {

        struct BreezePartChannelsChannel *chan = tmp->data;
        PtrTMPNode typeNode = getBreezePartChannelsChannelTypeNode (chan);

        if (typeNode)
        {
            PtrTMPNode newTypeNode = DeepCopyTMPNode (typeNode);
            PtrTMPNode newNodeType2 = resolveType (newTypeNode, getBreezePartFile (part));

            setBreezePartChannelsChannelTypeNode (chan, newNodeType2);
        }
    }
}

void getCompleteTypeList (struct BreezeFile *breezeFile, GList ** imports, GList ** types)
{
    GList *tmp;

    for (tmp = getBreezeImports (breezeFile); tmp; tmp = tmp->next)
    {
        struct BreezeImport *import = tmp->data;
        char *importName = getBreezeImportName (import);

        GList *tmp = *imports;

        for (; tmp; tmp = tmp->next)
        {
            struct BreezeImport *import = tmp->data;
            char *tmpName = getBreezeImportName (import);

            if (strcmp (tmpName, importName) == 0)
                goto continue_loop2;
        }

        *imports = g_list_prepend (*imports, import);
        struct BreezeFile *importedBreezeFile = getImportedBreezeFileByName (importName);

        getCompleteTypeList (importedBreezeFile, imports, types);

      continue_loop2:;
    }

    *types = g_list_concat (*types, getBreezeTypes (breezeFile));
}
