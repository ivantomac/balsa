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
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <glib.h>

#include "libbreeze.h"
#include "breezestructs.h"
#include "lscan.h"
#include "callcontexttree.h"
#include "threadgroups.h"

#define ENDS_WITH(x,y) ((strlen(x)>=strlen(y)) && !strcmp(x+strlen(x)-strlen(y),y))

struct BreezeImport *NewBreezeImport (char *ptr)
{
    struct BreezeImport *newBreezeImport = (struct BreezeImport *) calloc (1, sizeof (struct BreezeImport));

    newBreezeImport->name = getSecondKeywordCopy (ptr);
    return newBreezeImport;
}
struct BreezeType *NewBreezeType (char *ptr)
{
    struct BreezeType *newBreezeType = (struct BreezeType *) calloc (1, sizeof (struct BreezeType));

    newBreezeType->contents = ptr;
    newBreezeType->name = getSecondKeywordCopy (ptr);
    return newBreezeType;
}
struct BreezePart *NewBreezePart (char *ptr, struct BreezeFile *file)
{
    struct BreezePart *newBreezePart = (struct BreezePart *) calloc (1, sizeof (struct BreezePart));

    newBreezePart->contents = ptr;
    newBreezePart->name = getSecondKeywordCopy (ptr);
    newBreezePart->breezeFile = file;
    return newBreezePart;
}
struct BreezePartPorts *NewBreezePartPorts (char *ptr)
{
    struct BreezePartPorts *newBreezePartPorts = (struct BreezePartPorts *) calloc (1, sizeof (struct BreezePartPorts));

    newBreezePartPorts->contents = ptr;
    return newBreezePartPorts;
}
struct BreezePartPortsPort *NewBreezePartPortsPort (char *ptr)
{
    struct BreezePartPortsPort *newBreezePartPortsPort = (struct BreezePartPortsPort *) calloc (1,
      sizeof (struct BreezePartPortsPort));

    newBreezePartPortsPort->contents = ptr;
    newBreezePartPortsPort->name = getSecondKeywordCopy (ptr);
    return newBreezePartPortsPort;
}
struct BreezePartAttributes *NewBreezePartAttributes (char *ptr)
{
    struct BreezePartAttributes *newBreezePartAttributes = (struct BreezePartAttributes *) calloc (1,
      sizeof (struct BreezePartAttributes));

    newBreezePartAttributes->contents = ptr;
    return newBreezePartAttributes;
}
struct BreezePartAttributesAttribute *NewBreezePartAttributesAttribute (char *ptr)
{
    struct BreezePartAttributesAttribute *newBreezePartAttributesAttribute = (struct BreezePartAttributesAttribute *) calloc (1,
      sizeof (struct BreezePartAttributesAttribute));

    newBreezePartAttributesAttribute->contents = ptr;
    newBreezePartAttributesAttribute->name = getFirstKeywordCopy (ptr);
    return newBreezePartAttributesAttribute;
}
struct BreezePartChannels *NewBreezePartChannels (char *ptr)
{
    struct BreezePartChannels *newBreezePartChannels = (struct BreezePartChannels *) calloc (1,
      sizeof (struct BreezePartChannels));

    newBreezePartChannels->contents = ptr;
    return newBreezePartChannels;
}
struct BreezePartChannelsChannel *NewBreezePartChannelsChannel (char *ptr)
{
    struct BreezePartChannelsChannel *newBreezePartChannelsChannel = (struct BreezePartChannelsChannel *) calloc (1,
      sizeof (struct BreezePartChannelsChannel));

    newBreezePartChannelsChannel->contents = ptr;
    //    newBreezePartChannelsChannel->name = getSecondKeywordCopy (ptr);
    return newBreezePartChannelsChannel;
}
struct BreezePartComponents *NewBreezePartComponents (char *ptr)
{
    struct BreezePartComponents *newBreezePartComponents = (struct BreezePartComponents *) calloc (1,
      sizeof (struct BreezePartComponents));

    newBreezePartComponents->contents = ptr;
    return newBreezePartComponents;
}
struct BreezePartComponentsComponent *NewBreezePartComponentsComponent (char *ptr)
{
    struct BreezePartComponentsComponent *newBreezePartComponentsComponent = (struct BreezePartComponentsComponent *) calloc (1,
      sizeof (struct BreezePartComponentsComponent));

    newBreezePartComponentsComponent->contents = ptr;
    newBreezePartComponentsComponent->name = getSecondKeywordCopy (ptr);
    if (!strncmp (ptr, "undeclared-component", strlen ("undeclared-component")))
        newBreezePartComponentsComponent->isUndeclared = 1;
    return newBreezePartComponentsComponent;
}
struct BreezePartCallcontexts *NewBreezePartCallcontexts (char *ptr)
{
    struct BreezePartCallcontexts *newBreezePartCallcontexts = (struct BreezePartCallcontexts *) calloc (1,
      sizeof (struct BreezePartCallcontexts));

    newBreezePartCallcontexts->contents = ptr;
    return newBreezePartCallcontexts;
}
struct BreezePartCallcontextsCallcontext *NewBreezePartCallcontextsCallcontext (char *ptr)
{
    struct BreezePartCallcontextsCallcontext *newBreezePartCallcontextsCallcontext = (struct BreezePartCallcontextsCallcontext *) calloc (1,
      sizeof (struct BreezePartCallcontextsCallcontext));

    newBreezePartCallcontextsCallcontext->contents = ptr;
    //    newBreezePartCallcontextsCallcontext->name = getSecondKeywordCopy (ptr);
    return newBreezePartCallcontextsCallcontext;
}
struct BreezeContexts *NewBreezeContexts (char *ptr, struct BreezeFile *file)
{
    struct BreezeContexts *newBreezeContexts = (struct BreezeContexts *) calloc (1, sizeof (struct BreezeContexts));

    newBreezeContexts->contents = ptr;
    newBreezeContexts->breezeFile = file;
    return newBreezeContexts;
}
struct BreezeContextsContext *NewBreezeContextsContext (char *ptr, struct BreezeFile *file)
{
    struct BreezeContextsContext *newBreezeContextsContext = (struct BreezeContextsContext *) calloc (1,
      sizeof (struct BreezeContextsContext));

    newBreezeContextsContext->contents = ptr;
    newBreezeContextsContext->breezeFile = file;
    return newBreezeContextsContext;
}

/***********************************************************************/

typedef void (*FCT) (char *ptr, void *dataPtr);

void parseFirstLevelParentheses (char *ptr, FCT callback, void *dataPtr)
{
    //    char *ptr;
    int depth = 0;

    for ( /*ptr = file->contents */ ; *ptr; ptr++)
    {
        switch (*ptr)
        {
        case '(':
            depth++;
            if (depth == 1)
            {
                callback (ptr + 1, dataPtr);
            }
            break;

        case ')':
            depth--;
            if (depth <= 0)
            {
                if (depth == 0)
                    *ptr = 0;
                else            //if (depth < 0)
                    return;
            }
            break;

        case ';':
            do
                ptr++;
            while (*ptr && *ptr != '\n');
            if (*ptr == 0)
                return;
            continue;

        case '"':
            do
                ptr++;
            while (*ptr && *ptr != '"');
            if (*ptr == 0)
                return;
            continue;

        default:
            continue;
        }

    }
}

void parseBreezeFile_Callback (char *ptr, void *dataPtr)
{
    struct BreezeFile *file = (struct BreezeFile *) dataPtr;
    char *kw = getFirstKeywordCopy (ptr);

    if (!strcmp (kw, "import"))
        file->imports = g_list_prepend (file->imports, NewBreezeImport (getFirstKeywordPtr (ptr)));
    else if (!strcmp (kw, "type"))
        file->types = g_list_prepend (file->types, NewBreezeType (getFirstKeywordPtr (ptr)));
    else if (!strcmp (kw, "breeze-part"))
        file->parts = g_list_prepend (file->parts, NewBreezePart (getFirstKeywordPtr (ptr), file));
    else if (!strcmp (kw, "callContext-contexts"))
        file->contexts = NewBreezeContexts (getFirstKeywordPtr (ptr), file);

    else if (!strcmp (kw, "constant"))
    {
    }                           // do nothing
    else if (!strcmp (kw, "balsa"))
    {
    }                           // do nothing

    else
        g_warning ("unrecognised keyword: %s", kw);

    free (kw);
}

void parseBreezePart_Callback (char *ptr, void *dataPtr)
{
    struct BreezePart *part = (struct BreezePart *) dataPtr;
    char *kw = getFirstKeywordCopy (ptr);

    if (0 == strcmp (kw, "ports"))
        part->ports = NewBreezePartPorts (getFirstKeywordPtr (ptr));
    else if (0 == strcmp (kw, "attributes"))
        part->attributes = NewBreezePartAttributes (getFirstKeywordPtr (ptr));
    else if (0 == strcmp (kw, "channels"))
        part->channels = NewBreezePartChannels (getFirstKeywordPtr (ptr));
    else if (0 == strcmp (kw, "components"))
        part->components = NewBreezePartComponents (getFirstKeywordPtr (ptr));
    else if (0 == strcmp (kw, "call-contexts"))
        part->callcontexts = NewBreezePartCallcontexts (getFirstKeywordPtr (ptr));
    else
        g_warning ("unrecognised keyword: %s", kw);

    free (kw);
}

void parseBreezePartPorts_Callback (char *ptr, void *dataPtr)
{
    struct BreezePartPorts *ports = (struct BreezePartPorts *) dataPtr;
    char *kw = getFirstKeywordCopy (ptr);

    if (!strcmp (kw, "port"))
        ports->portList = g_list_prepend (ports->portList, NewBreezePartPortsPort (getFirstKeywordPtr (ptr)));
    else if (!strcmp (kw, "sync-port"))
        ports->portList = g_list_prepend (ports->portList, NewBreezePartPortsPort (getFirstKeywordPtr (ptr)));
    else if (!strcmp (kw, "arrayed-port"))
        ports->portList = g_list_prepend (ports->portList, NewBreezePartPortsPort (getFirstKeywordPtr (ptr)));
    else if (!strcmp (kw, "arrayed-sync-port"))
        ports->portList = g_list_prepend (ports->portList, NewBreezePartPortsPort (getFirstKeywordPtr (ptr)));
    else
        g_warning ("unrecognised port type: %s", kw);

    free (kw);
}

void parseBreezePartAttributes_Callback (char *ptr, void *dataPtr)
{
    struct BreezePartAttributes *attributes = (struct BreezePartAttributes *) dataPtr;

    attributes->attributeList = g_list_prepend (attributes->attributeList, NewBreezePartAttributesAttribute (getFirstKeywordPtr (ptr)));
}

void parseBreezePartChannels_Callback (char *ptr, void *dataPtr)
{
    struct BreezePartChannels *channels = (struct BreezePartChannels *) dataPtr;
    char *kw = getFirstKeywordCopy (ptr);

    if (!strcmp (kw, "sync"))
        channels->channelList = g_list_prepend (channels->channelList, NewBreezePartChannelsChannel (getFirstKeywordPtr (ptr)));
    else if (!strcmp (kw, "push"))
        channels->channelList = g_list_prepend (channels->channelList, NewBreezePartChannelsChannel (getFirstKeywordPtr (ptr)));
    else if (!strcmp (kw, "pull"))
        channels->channelList = g_list_prepend (channels->channelList, NewBreezePartChannelsChannel (getFirstKeywordPtr (ptr)));
    else
        g_warning ("unrecognised keyword: %s", kw);

    free (kw);
}

void parseBreezePartComponents_Callback (char *ptr, void *dataPtr)
{
    struct BreezePartComponents *components = (struct BreezePartComponents *) dataPtr;
    char *kw = getFirstKeywordCopy (ptr);

    //    printf("kw=%s\n",kw);

    if (!strcmp (kw, "component"))
        components->componentList = g_list_prepend (components->componentList, NewBreezePartComponentsComponent (getFirstKeywordPtr (ptr)));
    else if (!strcmp (kw, "undeclared-component"))
        components->componentList = g_list_prepend (components->componentList, NewBreezePartComponentsComponent (getFirstKeywordPtr (ptr)));
    else
        g_warning ("unrecognised keyword: %s", kw);

    free (kw);
}

void parseBreezePartCallcontexts_Callback (char *ptr, void *dataPtr)
{
    struct BreezePartCallcontexts *callcontexts = (struct BreezePartCallcontexts *) dataPtr;
    char *kw = getFirstKeywordCopy (ptr);

    if (!strcmp (kw, "call-context"))
        callcontexts->callcontextList =
          g_list_prepend (callcontexts->callcontextList, NewBreezePartCallcontextsCallcontext (getFirstKeywordPtr (ptr)));
    else
        g_warning ("unrecognised keyword: %s", kw);

    free (kw);
}

void parseBreezeContexts_Callback (char *ptr, void *dataPtr)
{
    struct BreezeContexts *contexts = (struct BreezeContexts *) dataPtr;
    char *kw = getFirstKeywordCopy (ptr);

    if (!strcmp (kw, "callContext-context"))
        contexts->contextList = g_list_prepend (contexts->contextList, NewBreezeContextsContext (getFirstKeywordPtr (ptr), contexts->breezeFile));
    else
        g_warning ("unrecognised keyword: %s", kw);

    free (kw);
}

/***********************************************************************/

void initialiseBreezePartPortsPort (struct BreezePartPortsPort *port)
{
    char *str = port->contents;

    port->node = ParseTMPNode (&str, TRUE, TRUE);

    char *syncport_port = ((PtrTMPNode) (port->node->body.list->data))->body.string;
    gboolean isSyncPort;

    if (!strcmp (syncport_port, "sync-port") || !strcmp (syncport_port, "arrayed-sync-port"))
        isSyncPort = TRUE;
    else if (!strcmp (syncport_port, "port") || !strcmp (syncport_port, "arrayed-port"))
        isSyncPort = FALSE;
    else
    {
        fprintf (stderr,
          "Error in initialiseBreezePartPortsPort: syncport_port (arg0) of port \"%s\" is \"%s\" but should be \"sync-port\" or \"port\"\n",
          port->name, syncport_port);
        exit (EXIT_FAILURE);
    }

    char *active_passive = ((PtrTMPNode) (port->node->body.list->next->next->data))->body.string;
    gboolean isActive;

    if (!strcmp (active_passive, "active"))
        isActive = TRUE;
    else if (!strcmp (active_passive, "passive"))
        isActive = FALSE;
    else
    {
        fprintf (stderr,
          "Error in initialiseBreezePartPortsPort: active_passive (arg2) of port \"%s\" is \"%s\" but should be \"active\" or \"passive\"\n",
          port->name, active_passive);
        exit (EXIT_FAILURE);
    }

    if (isSyncPort)
        port->type = isActive ? ActiveSyncPort : PassiveSyncPort;
    else
    {
        char *input_output = ((PtrTMPNode) (port->node->body.list->next->next->next->data))->body.string;
        gboolean isInput;

        if (!strcmp (input_output, "input"))
            isInput = TRUE;
        else if (!strcmp (input_output, "output"))
            isInput = FALSE;
        else
        {
            fprintf (stderr,
              "Error in initialiseBreezePartPortsPort: input_output (arg3) of port \"%s\" is \"%s\" but should be \"input\" or \"output\"\n",
              port->name, input_output);
            exit (EXIT_FAILURE);
        }

        if (isInput)
            port->type = isActive ? ActiveInputPort : PassiveInputPort;
        else
            port->type = isActive ? ActiveOutputPort : PassiveOutputPort;
    }

    if (!strcmp (syncport_port, "arrayed-port"))
    {
        port->isArray = TRUE;
        port->arrayIndexLow = atoi (((PtrTMPNode) (port->node->body.list->next->next->next->next->next->data))->body.string);
        port->arraySize = atoi (((PtrTMPNode) (port->node->body.list->next->next->next->next->next->next->data))->body.string);
    } else if (!strcmp (syncport_port, "arrayed-sync-port"))
    {
        port->isArray = TRUE;
        port->arrayIndexLow = atoi (((PtrTMPNode) (port->node->body.list->next->next->next->data))->body.string);
        port->arraySize = atoi (((PtrTMPNode) (port->node->body.list->next->next->next->next->data))->body.string);
        int i = 0;

        i++;
    }

    port->initialised = TRUE;
}

void initialiseBreezePartAttributesAttribute (struct BreezePartAttributesAttribute *attribute)
{
    char *str = attribute->contents;

    attribute->node = ParseTMPNode (&str, TRUE, TRUE);

    attribute->initialised = TRUE;
}

void initialiseBreezePartChannelsChannel (struct BreezePartChannelsChannel *channel)
{
    char *str = channel->contents;
    PtrTMPNode node = ParseTMPNode (&str, TRUE, TRUE);
    GList *args = node->body.list;

    {
        char *type_channel = ((PtrTMPNode) (args->data))->body.string;

        if (!strcmp (type_channel, "sync"))
            channel->type = SyncChannel;
        else if (!strcmp (type_channel, "push"))
            channel->type = PushChannel;
        else if (!strcmp (type_channel, "pull"))
            channel->type = PullChannel;
        else
        {
            fprintf (stderr, "Error in initialiseBreezePartChannelsChannel: type is \"%s\"\n", type_channel);
            exit (EXIT_FAILURE);
        }
        args = g_list_next (args);

        if (channel->type != SyncChannel)
        {
            channel->width = atoi (((PtrTMPNode) (args->data))->body.string);
            args = g_list_next (args);
        }

        for (; args; args = g_list_next (args))
        {
            if (TMPIsHeaded (args->data, "at"))
                channel->position = parsePositionNode (args->data);
            else if (TMPIsHeaded (args->data, "name"))
                channel->name = g_strdup (((PtrTMPNode) ((PtrTMPNode) (args->data))->body.list->next->data)->body.string);
            else if (TMPIsHeaded (args->data, "type"))
                channel->typeNode = ((PtrTMPNode) (args->data))->body.list->next->data;
            else
                channel->otherEntries = g_list_prepend (channel->otherEntries, DeepCopyTMPNode (args->data));
        }
    }
    free (node);

    channel->initialised = TRUE;
}

void initialiseBreezePartComponentsComponent (struct BreezePartComponentsComponent *comp)
{
    char *str = comp->contents;

    comp->node = ParseTMPNode (&str, TRUE, TRUE);

    GList *args = comp->node->body.list;

    for (; args; args = g_list_next (args))
    {
        if (TMPIsHeaded (args->data, "at"))
            comp->position = parsePositionNode (args->data);
    }

    comp->initialised = TRUE;
}

void initialiseBreezePartCallcontextsCallcontext (struct BreezePartCallcontextsCallcontext *callcontext)
{
    char *str = callcontext->contents;
    PtrTMPNode node = ParseTMPNode (&str, TRUE, TRUE);
    GList *args = node->body.list;

    {
        args = g_list_next (args);

        callcontext->num = atoi (((PtrTMPNode) (args->data))->body.string);
        args = g_list_next (args);

        callcontext->name = g_strdup (((PtrTMPNode) (args->data))->body.string);
        args = g_list_next (args);

        callcontext->callerPosition = parsePositionNode (args->data);
        args = g_list_next (args);

        callcontext->calleePosition = parsePositionNode (args->data);
        args = g_list_next (args);

        callcontext->parentNum = atoi (((PtrTMPNode) (args->data))->body.string);
        args = g_list_next (args);

        if (args && TMPIsHeaded (args->data, "port-channels"))
        {
            GList *subargs = ((PtrTMPNode) args->data)->body.list->next;

            while (subargs)
            {
                int channum = atoi (((PtrTMPNode) (subargs->data))->body.string);

                callcontext->channelList = g_list_prepend (callcontext->channelList, GINT_TO_POINTER (channum));
                subargs = g_list_next (subargs);
            }
        }
    }
    DeleteTMPNode (node);
    callcontext->initialised = TRUE;
}

void initialiseBreezeContextsContext (struct BreezeContextsContext *context)
{
    char *str = context->contents;
    PtrTMPNode node = ParseTMPNode (&str, TRUE, TRUE);
    GList *args = node->body.list;

    args = g_list_next (args);

    context->num = atoi (((PtrTMPNode) (args->data))->body.string);
    args = g_list_next (args);

    context->procName = g_strdup (((PtrTMPNode) (args->data))->body.string);
    args = g_list_next (args);

    context->position = parsePositionNode (args->data);
    args = g_list_next (args);

    context->parentContextNum = atoi (((PtrTMPNode) (args->data))->body.string);

    context->initialised = TRUE;
}

/***********************************************************************/

/* Initialising the parsing of a breeze file */
/*                                          */
/* TODO: Find a better a way than copying  */
/*      the whole file to memory          */
struct BreezeFile *breezeInitParse (const char *filename)
{
    struct BreezeFile *newBreezeFile = (struct BreezeFile *) calloc (1, sizeof (struct BreezeFile));

    char *breezeFilename;

    if (ENDS_WITH (filename, ".breeze"))
        breezeFilename = g_strdup (filename);
    else
        breezeFilename = g_strdup_printf ("%s.breeze", filename);

    FILE *f = fopen (breezeFilename, "rb");

    if (!f)
    {
        // fprintf (stderr, "Error in opening %s.breeze\n", filename);
        free (breezeFilename);
        return NULL;
    } else
    {
        struct stat st;

        stat (breezeFilename, &st);

        newBreezeFile->contents = (char *) malloc (st.st_size + 1);
        int size = fread (newBreezeFile->contents, 1, st.st_size, f);

        if (size != st.st_size)
        {
            fprintf (stderr, "Error in reading %s.breeze\n", filename);
            fclose (f);
            free (breezeFilename);
            return NULL;
        }
        fclose (f);
        newBreezeFile->contents[st.st_size] = 0;
        newBreezeFile->name = g_strdup (filename);
        free (breezeFilename);
    }

    parseFirstLevelParentheses (newBreezeFile->contents, parseBreezeFile_Callback, (void *) newBreezeFile);

    newBreezeFile->types = g_list_reverse (newBreezeFile->types);

    return newBreezeFile;
}

/* Accessing BreezeFile_ */
char *getBreezeFileName (struct BreezeFile *file)
{
    return file->name;
}

GList *getBreezeImports (struct BreezeFile * file)
{
    return file->imports;
}

GList *getBreezeTypes (struct BreezeFile * file)
{
    return file->types;
}

struct BreezeType *getBreezeTypeByName (struct BreezeFile *file, const char *name)
{
    fprintf (stderr, "TODO getBreezeTypeByName\n");
    return NULL;
}

GList *getBreezeParts (struct BreezeFile * file)
{
    return file->parts;
}

struct BreezePart *getBreezePartByName (struct BreezeFile *file, const char *name)
{
    GList *tmp;

    for (tmp = file->parts; tmp; tmp = tmp->next)
    {
        struct BreezePart *part = (struct BreezePart *) tmp->data;

        if (!strcmp (part->name, name))
            return part;
    }

    return NULL;
}

struct BreezePart *getBreezePartByName_searchImports (struct BreezeFile *file, const char *name)
{
    // function declared in flatten_breezefiles.c
    struct BreezeFile *getImportedBreezeFileByName (char *name);

    struct BreezePart *part = getBreezePartByName (file, name);

    if (part)
        return part;

    GList *tmp;

    for (tmp = getBreezeImports (file); tmp; tmp = tmp->next)
    {
        struct BreezeImport *import = tmp->data;
        char *importName = getBreezeImportName (import);
        struct BreezeFile *importedBreezeFile = getImportedBreezeFileByName (importName);

        part = getBreezePartByName_searchImports (importedBreezeFile, name);
        if (part)
            return part;
    }

    return NULL;
}

GList *getBreezeContextsList (struct BreezeFile * file)
{
    if (!file->contexts)
        return NULL;

    if (!file->contexts->initialised)
    {
        parseFirstLevelParentheses (file->contexts->contents, parseBreezeContexts_Callback, (void *) file->contexts);
        file->contexts->initialised = TRUE;
    }

    return file->contexts->contextList;
}

GList *getBreezeContextsList_OrderedByInclusion (struct BreezeFile * file)
{
    GList *list = getBreezeContextsList (file);
    GList *tmp1, *tmp2;

    for (tmp1 = list; tmp1; tmp1 = tmp1->next)
    {
        struct BreezeContextsContext *context1 = tmp1->data;
        int parent1 = getBreezeContextsContextParentContextNum (context1);

        for (tmp2 = g_list_next (tmp1); tmp2; tmp2 = tmp2->next)
        {
            struct BreezeContextsContext *context2 = tmp2->data;
            int num2 = getBreezeContextsContextNum (context2);

            if (parent1 == num2)
            {                   // Move context1 just after context2
                tmp1 = tmp1->prev;
                list = g_list_remove (list, context1);
                list = g_list_insert (list, context1, g_list_position (list, tmp2) + 1);
                break;
            }
        }
    }

    return list;
}

void setBreezeContextsList (struct BreezeFile *file, GList * list)
{
    if (!file->contexts)
        return;

    file->contexts->initialised = TRUE;
    file->contexts->contextList = list;
}

/* Accessing BreezeImport_ */
char *getBreezeImportName (struct BreezeImport *import)
{
    return import->name;
}

/* Accessing BreezeType_ */
char *getBreezeTypeName (struct BreezeType *type)
{
    return type->name;
}

PtrTMPNode getBreezeTypeTMPNode (struct BreezeType * type)
{
    if (!type->initialised)
    {
        char *str = type->contents;

        type->node = ParseTMPNode (&str, TRUE, TRUE);
        type->initialised = TRUE;
    }

    return type->node;
}

/* Accessing BreezePart_ */
char *getBreezePartName (struct BreezePart *part)
{
    return part->name;
}

struct BreezeFile *getBreezePartFile (struct BreezePart *part)
{
    return part->breezeFile;
}

GList *getBreezePartPortsList (struct BreezePart * part)
{
    if (!part->initialised)
    {
        parseFirstLevelParentheses (part->contents, parseBreezePart_Callback, (void *) part);
        part->initialised = TRUE;
    }

    if (!part->ports)
        return NULL;

    if (!part->ports->initialised)
    {
        parseFirstLevelParentheses (part->ports->contents, parseBreezePartPorts_Callback, (void *) part->ports);
        part->ports->initialised = TRUE;
    }

    return part->ports->portList;
}

GList *getBreezePartAttributesList (struct BreezePart * part)
{
    if (!part->initialised)
    {
        parseFirstLevelParentheses (part->contents, parseBreezePart_Callback, (void *) part);
        part->initialised = TRUE;
    }

    if (!part->attributes)
        return NULL;

    if (!part->attributes->initialised)
    {
        parseFirstLevelParentheses (part->attributes->contents, parseBreezePartAttributes_Callback, (void *) part->attributes);
        part->attributes->initialised = TRUE;
    }

    return part->attributes->attributeList;
}

GList *getBreezePartChannelsList (struct BreezePart * part)
{
    if (!part->initialised)
    {
        parseFirstLevelParentheses (part->contents, parseBreezePart_Callback, (void *) part);
        part->initialised = TRUE;
    }

    if (!part->channels)
        return NULL;

    if (!part->channels->initialised)
    {
        parseFirstLevelParentheses (part->channels->contents, parseBreezePartChannels_Callback, (void *) part->channels);
        part->channels->initialised = TRUE;
    }

    return part->channels->channelList;
}

GList *getBreezePartComponentsList (struct BreezePart * part)
{
    if (!part->initialised)
    {
        parseFirstLevelParentheses (part->contents, parseBreezePart_Callback, (void *) part);
        part->initialised = TRUE;
    }

    if (!part->components)
        return NULL;

    if (!part->components->initialised)
    {
        parseFirstLevelParentheses (part->components->contents, parseBreezePartComponents_Callback, (void *) part->components);
        part->components->initialised = TRUE;
    }

    return part->components->componentList;
}

GList *getBreezePartCallcontextsList (struct BreezePart * part)
{
    if (!part->initialised)
    {
        parseFirstLevelParentheses (part->contents, parseBreezePart_Callback, (void *) part);
        part->initialised = TRUE;
    }

    if (!part->callcontexts)
        return NULL;

    if (!part->callcontexts->initialised)
    {
        parseFirstLevelParentheses (part->callcontexts->contents, parseBreezePartCallcontexts_Callback, (void *) part->callcontexts);
        part->callcontexts->initialised = TRUE;
    }

    return part->callcontexts->callcontextList;
}

void setBreezePartChannelsList (struct BreezePart *part, GList * list)
{
    if (!part->initialised)
    {
        parseFirstLevelParentheses (part->contents, parseBreezePart_Callback, (void *) part);
        part->initialised = TRUE;
    }

    if (!part->channels)
    {
        part->channels = NewBreezePartChannels ("-- Initialised by setBreezePartChannelsList --");
    }

    part->channels->initialised = TRUE;
    part->channels->channelList = list;
}

void setBreezePartComponentsList (struct BreezePart *part, GList * list)
{
    if (!part->initialised)
    {
        parseFirstLevelParentheses (part->contents, parseBreezePart_Callback, (void *) part);
        part->initialised = TRUE;
    }

    if (!part->components)
    {
        part->components = NewBreezePartComponents ("-- Initialised by setBreezePartComponentsList --");
    }

    part->components->initialised = TRUE;
    part->components->componentList = list;
}

void setBreezePartCallcontextsList (struct BreezePart *part, GList * list)
{
    if (!part->initialised)
    {
        parseFirstLevelParentheses (part->contents, parseBreezePart_Callback, (void *) part);
        part->initialised = TRUE;
    }

    if (!part->callcontexts)
    {
        part->callcontexts = NewBreezePartCallcontexts ("-- Initialised by setBreezePartCallcontextsList --");
    }

    part->callcontexts->initialised = TRUE;
    part->callcontexts->callcontextList = list;
}

GHashTable *generateBreezePartCallcontextTree (struct BreezePart *part)
{
    if (!part->callcontextTree_hashtable)
        InitBreezePartCallcontextTree (part);

    return part->callcontextTree_hashtable;
}

GPtrArray *generateBreezePartThreadGroups (struct BreezePart * part)
{
    if (!part->threadGroups)
        InitBreezePartThreadGroups (part);

    return part->threadGroups;
}

/* Accessing BreezePartPortsPort_ */
char *getBreezePartPortsPortName (struct BreezePartPortsPort *port)
{
    return port->name;
}

enum BreezePartPortsPortType getBreezePartPortsPortType (struct BreezePartPortsPort *port)
{
    if (!port->initialised)
        initialiseBreezePartPortsPort (port);

    return port->type;
}

gboolean getBreezePartPortsPortIsArray (struct BreezePartPortsPort * port)
{
    if (!port->initialised)
        initialiseBreezePartPortsPort (port);

    return port->isArray;
}

int getBreezePartPortsPortArrayIndexLow (struct BreezePartPortsPort *port)
{
    if (!port->initialised)
        initialiseBreezePartPortsPort (port);

    return port->arrayIndexLow;
}

int getBreezePartPortsPortArraySize (struct BreezePartPortsPort *port)
{
    if (!port->initialised)
        initialiseBreezePartPortsPort (port);

    return port->arraySize;
}

PtrTMPNode getBreezePartPortsPortTMPNode (struct BreezePartPortsPort * port)
{
    if (!port->initialised)
        initialiseBreezePartPortsPort (port);

    return port->node;
}

void setBreezePartPortsPortTMPNode (struct BreezePartPortsPort *port, PtrTMPNode node)
{
    port->initialised = TRUE;
    port->node = node;
}

PtrTMPNode getBreezePartPortsPortDataTypeNode (struct BreezePartPortsPort *port)
{
    switch (getBreezePartPortsPortType (port))
    {
    case PassiveSyncPort:
    case ActiveSyncPort:
        return NULL;
    case PassiveOutputPort:
    case ActiveOutputPort:
    case PassiveInputPort:
    case ActiveInputPort:
        return getBreezePartPortsPortTMPNode (port)->body.list->next->next->next->next->data;
    default:
        g_warning ("port type not handled in switch statement");
    }
    return NULL;
}

/* Accessing BreezePartAttributesAttribute_ */
char *getBreezePartAttributesAttributeName (struct BreezePartAttributesAttribute *attribute)
{
    return attribute->name;
}

PtrTMPNode getBreezePartAttributesAttributeTMPNode (struct BreezePartAttributesAttribute * attribute)
{
    if (!attribute->initialised)
        initialiseBreezePartAttributesAttribute (attribute);

    return attribute->node;
}

void setBreezePartAttributesAttributeTMPNode (struct BreezePartAttributesAttribute *attribute, PtrTMPNode node)
{
    attribute->initialised = TRUE;
    attribute->node = node;
}

/* Accessing BreezePartChannelsChannel_ */
int getBreezePartChannelsChannelWidth (struct BreezePartChannelsChannel *channel)
{
    if (!channel->initialised)
        initialiseBreezePartChannelsChannel (channel);

    return channel->width;
}

char *getBreezePartChannelsChannelName (struct BreezePartChannelsChannel *channel)
{
    if (!channel->initialised)
        initialiseBreezePartChannelsChannel (channel);

    return channel->name;
}

struct Position *getBreezePartChannelsChannelPosition (struct BreezePartChannelsChannel *channel)
{
    if (!channel->initialised)
        initialiseBreezePartChannelsChannel (channel);

    return channel->position;
}

PtrTMPNode getBreezePartChannelsChannelTypeNode (struct BreezePartChannelsChannel * channel)
{
    if (!channel->initialised)
        initialiseBreezePartChannelsChannel (channel);

    return channel->typeNode;
}

enum BreezePartChannelsChannelType getBreezePartChannelsChannelType (struct BreezePartChannelsChannel *channel)
{
    if (!channel->initialised)
        initialiseBreezePartChannelsChannel (channel);

    return channel->type;
}

void setBreezePartChannelsChannelPosition (struct BreezePartChannelsChannel *channel, struct Position *position)
{
    if (!channel->initialised)
        initialiseBreezePartChannelsChannel (channel);

    channel->position = position;
}

void setBreezePartChannelsChannelName (struct BreezePartChannelsChannel *channel, char *name)
{
    if (!channel->initialised)
        initialiseBreezePartChannelsChannel (channel);

    if (channel->name)
        free (channel->name);
    channel->name = name;
}

void setBreezePartChannelsChannelTypeNode (struct BreezePartChannelsChannel *channel, PtrTMPNode typeNode)
{
    if (!channel->initialised)
        initialiseBreezePartChannelsChannel (channel);

    channel->typeNode = typeNode;
}

struct BreezePartChannelsChannel *DeepCopyBreezePartChannelsChannel (struct BreezePartChannelsChannel *chan)
{
    struct BreezePartChannelsChannel *newChan = (struct BreezePartChannelsChannel *) calloc (1,
      sizeof (struct BreezePartChannelsChannel));

    newChan->contents = chan->contents;
    newChan->initialised = chan->initialised;
    if (chan->initialised)
    {
        newChan->type = chan->type;
        newChan->width = chan->width;
        newChan->position = copyPosition (chan->position);
        if (chan->name)
            newChan->name = g_strdup (chan->name);
    }
    newChan->typeNode = DeepCopyTMPNode (chan->typeNode);
    if (chan->otherEntries)
    {
        GList *tmp = g_list_last (chan->otherEntries);

        for (; tmp; tmp = tmp->prev)
        {
            newChan->otherEntries = g_list_prepend (newChan->otherEntries, DeepCopyTMPNode ((PtrTMPNode) tmp->data));
        }
    }

    return newChan;
}

/* Accessing BreezePartComponentsComponent_ */
char *getBreezePartComponentsComponentName (struct BreezePartComponentsComponent *comp)
{
    return comp->name;
}

gboolean getBreezePartComponentsComponentIsUndeclared (struct BreezePartComponentsComponent * comp)
{
    return comp->isUndeclared;
}

PtrTMPNode getBreezePartComponentsComponentTMPNode (struct BreezePartComponentsComponent * comp)
{
    if (!comp->initialised)
        initialiseBreezePartComponentsComponent (comp);

    return comp->node;
}

void setBreezePartComponentsComponentTMPNode (struct BreezePartComponentsComponent *comp, PtrTMPNode node)
{
    comp->initialised = TRUE;
    comp->node = node;
}

struct Position *getBreezePartComponentsComponentPosition (struct BreezePartComponentsComponent *comp)
{
    if (!comp->initialised)
        initialiseBreezePartComponentsComponent (comp);

    return comp->position;
}

struct BreezePartComponentsComponent *DeepCopyBreezePartComponentsComponent (struct BreezePartComponentsComponent *comp)
{
    struct BreezePartComponentsComponent *newComp = (struct BreezePartComponentsComponent *) calloc (1,
      sizeof (struct BreezePartComponentsComponent));

    newComp->contents = comp->contents;
    newComp->name = g_strdup (comp->name);
    newComp->isUndeclared = comp->isUndeclared;
    newComp->initialised = comp->initialised;
    if (comp->initialised)
        newComp->node = DeepCopyTMPNode (comp->node);
    if (comp->position)
        newComp->position = copyPosition (comp->position);

    return newComp;
}

/* Accessing BreezePartCallcontextsCallcontext */
int getBreezePartCallcontextsCallcontextNum (struct BreezePartCallcontextsCallcontext *callcontext)
{
    if (!callcontext->initialised)
        initialiseBreezePartCallcontextsCallcontext (callcontext);

    return callcontext->num;
}

char *getBreezePartCallcontextsCallcontextName (struct BreezePartCallcontextsCallcontext *callcontext)
{
    if (!callcontext->initialised)
        initialiseBreezePartCallcontextsCallcontext (callcontext);

    return callcontext->name;
}

int getBreezePartCallcontextsCallcontextParentNum (struct BreezePartCallcontextsCallcontext *callcontext)
{
    if (!callcontext->initialised)
        initialiseBreezePartCallcontextsCallcontext (callcontext);

    return callcontext->parentNum;
}

GList *getBreezePartCallcontextsCallcontextChannelList (struct BreezePartCallcontextsCallcontext * callcontext)
{
    if (!callcontext->initialised)
        initialiseBreezePartCallcontextsCallcontext (callcontext);

    return callcontext->channelList;
}

void setBreezePartCallcontextsCallcontextNum (struct BreezePartCallcontextsCallcontext *callcontext, int num)
{
    if (!callcontext->initialised)
        initialiseBreezePartCallcontextsCallcontext (callcontext);

    callcontext->num = num;
}

void setBreezePartCallcontextsCallcontextName (struct BreezePartCallcontextsCallcontext *callcontext, char *name)
{
    if (!callcontext->initialised)
        initialiseBreezePartCallcontextsCallcontext (callcontext);

    if (callcontext->name)
        free (callcontext->name);
    callcontext->name = name;
}

void setBreezePartCallcontextsCallcontextParentNum (struct BreezePartCallcontextsCallcontext *callcontext, int num)
{
    if (!callcontext->initialised)
        initialiseBreezePartCallcontextsCallcontext (callcontext);

    callcontext->parentNum = num;
}

void setBreezePartCallcontextsCallcontextChannelList (struct BreezePartCallcontextsCallcontext *callcontext, GList * channelList)
{
    if (!callcontext->initialised)
        initialiseBreezePartCallcontextsCallcontext (callcontext);

    callcontext->channelList = channelList;
}

struct BreezePartCallcontextsCallcontext *DeepCopyBreezePartCallcontextsCallcontext (struct BreezePartCallcontextsCallcontext *callcontext)
{
    struct BreezePartCallcontextsCallcontext *newCallcontext = (struct BreezePartCallcontextsCallcontext *) calloc (1,
      sizeof (struct BreezePartCallcontextsCallcontext));

    newCallcontext->contents = callcontext->contents;
    newCallcontext->initialised = callcontext->initialised;
    if (callcontext->initialised)
    {
        newCallcontext->num = callcontext->num;
        newCallcontext->callerPosition = copyPosition (callcontext->callerPosition);
        newCallcontext->calleePosition = copyPosition (callcontext->calleePosition);
        if (callcontext->name)
            newCallcontext->name = g_strdup (callcontext->name);
        newCallcontext->parentNum = callcontext->parentNum;
        newCallcontext->channelList = g_list_copy (callcontext->channelList);
    }

    return newCallcontext;
}

/* Accessing BreezeContextsContext */
int getBreezeContextsContextNum (struct BreezeContextsContext *context)
{
    if (!context->initialised)
        initialiseBreezeContextsContext (context);

    return context->num;
}

char *getBreezeContextsContextProcName (struct BreezeContextsContext *context)
{
    if (!context->initialised)
        initialiseBreezeContextsContext (context);

    return context->procName;
}

struct Position *getBreezeContextsContextPosition (struct BreezeContextsContext *context)
{
    if (!context->initialised)
        initialiseBreezeContextsContext (context);

    return context->position;
}

int getBreezeContextsContextParentContextNum (struct BreezeContextsContext *context)
{
    if (!context->initialised)
        initialiseBreezeContextsContext (context);

    return context->parentContextNum;
}

/* Output */
void dumpBreezeFile (FILE * stream, struct BreezeFile *file)
{
    GList *tmp;

    fprintf (stream, "\n;;; Imports\n");
    for (tmp = file->imports; tmp; tmp = tmp->next)
    {
        struct BreezeImport *import = tmp->data;

        dumpBreezeImport (stream, import);
    }

    fprintf (stream, "\n;;; Types\n");
    for (tmp = file->types; tmp; tmp = tmp->next)
    {
        struct BreezeType *type = tmp->data;

        dumpBreezeType (stream, type);
    }

    fprintf (stream, "\n;;; Constants\n");

    fprintf (stream, "\n;;; Parts\n");
    for (tmp = file->parts; tmp; tmp = tmp->next)
    {
        struct BreezePart *part = tmp->data;

        dumpBreezePart (stream, part);
    }
}

void dumpBreezeImport (FILE * stream, struct BreezeImport *import)
{
    fprintf (stream, "(import \"%s\")\n", import->name);
}

void dumpBreezeType (FILE * stream, struct BreezeType *type)
{
    if (!type->initialised)
    {
        fprintf (stream, "(%s)\n", type->contents);
    } else
    {
        /* TODO: when breezeTypes can be modified, print the modified version */
        fprintf (stream, "(%s)\n", type->contents);
    }
}

void dumpBreezePart (FILE * stream, struct BreezePart *part)
{
    if (!part->initialised)
    {
        fprintf (stream, "(%s)\n", part->contents);
    } else
    {
        fprintf (stream, "(breeze-part \"%s\"\n", part->name);
        dumpBreezePartPorts (stream, part->ports);
        dumpBreezePartAttributes (stream, part->attributes);
        dumpBreezePartChannels (stream, part->channels);
        dumpBreezePartComponents (stream, part->components);
        if (part->callcontexts)
            dumpBreezePartCallcontexts (stream, part->callcontexts);
        fprintf (stream, ")\n");
    }
}

void dumpBreezePartPorts (FILE * stream, struct BreezePartPorts *ports)
{
    if (!ports->initialised)
    {
        fprintf (stream, "  (%s)\n", ports->contents);
    } else
    {
        GList *tmp;

        fprintf (stream, "  (ports\n");

        for (tmp = g_list_last (ports->portList); tmp; tmp = tmp->prev)
        {
            struct BreezePartPortsPort *port = (struct BreezePartPortsPort *) tmp->data;

            dumpBreezePartPortsPort (stream, port);
        }

        fprintf (stream, "  )\n");
    }
}

void dumpBreezePartPortsPort (FILE * stream, struct BreezePartPortsPort *port)
{
    if (!port->initialised)
    {
        fprintf (stream, "    (%s)\n", port->contents);
    } else
    {
        fprintf (stream, "    ");
        PrintTMPNode (stream, port->node);
        fprintf (stream, "\n");
    }
}

void dumpBreezePartAttributes (FILE * stream, struct BreezePartAttributes *attributes)
{
    if (!attributes->initialised)
    {
        fprintf (stream, "  (%s)\n", attributes->contents);
    } else
    {
        GList *tmp;

        fprintf (stream, "  (attributes\n");

        for (tmp = g_list_last (attributes->attributeList); tmp; tmp = tmp->prev)
        {
            struct BreezePartAttributesAttribute *attribute = (struct BreezePartAttributesAttribute *) tmp->data;

            dumpBreezePartAttributesAttribute (stream, attribute);
        }

        fprintf (stream, "  )\n");
    }
}

void dumpBreezePartAttributesAttribute (FILE * stream, struct BreezePartAttributesAttribute *attribute)
{
    if (!attribute->initialised)
    {
        fprintf (stream, "    (%s)\n", attribute->contents);
    } else
    {
        fprintf (stream, "    ");
        PrintTMPNode (stream, attribute->node);
        fprintf (stream, "\n");
    }
}

void dumpBreezePartChannels (FILE * stream, struct BreezePartChannels *channels)
{
    if (!channels->initialised)
    {
        fprintf (stream, "  (%s)\n", channels->contents);
    } else if (channels->channelList)
    {
        GList *tmp;

        fprintf (stream, "  (channels\n");

        int commentnum = 1;

        for (tmp = g_list_last (channels->channelList); tmp; tmp = tmp->prev)
        {
            struct BreezePartChannelsChannel *chan = (struct BreezePartChannelsChannel *) tmp->data;

            dumpBreezePartChannelsChannel (stream, chan, commentnum++);
        }

        fprintf (stream, "  )\n");
    }
}

void dumpBreezePartChannelsChannel (FILE * stream, struct BreezePartChannelsChannel *chan, int commentNum)
{
    if (!chan->initialised)
    {
        fprintf (stream, "    (%s)", chan->contents);
        goto end;
    } else
    {
        fprintf (stream, "    (");
        switch (chan->type)
        {
        case SyncChannel:
            fprintf (stream, "sync ");
            break;
        case PushChannel:
            fprintf (stream, "push %d ", chan->width);
            break;
        case PullChannel:
            fprintf (stream, "pull %d ", chan->width);
            break;
        }

        dumpPosition (stream, chan->position);

        if (chan->name)
            fprintf (stream, " (name \"%s\")", chan->name);

        if (chan->typeNode)
        {
            fprintf (stream, " (type ");
            PrintTMPNode (stream, chan->typeNode);
            fprintf (stream, ")");
        }

        if (chan->otherEntries)
        {
            GList *tmp = chan->otherEntries;

            for (; tmp; tmp = tmp->next)
            {
                fprintf (stream, " ");
                PrintTMPNode (stream, (PtrTMPNode) tmp->data);
            }
        }

        fprintf (stream, ")");
      end:
        if (commentNum > 0)
            fprintf (stream, " ; %d", commentNum);

        fprintf (stream, "\n"); //  ;;; TOCHECK dumpBreezePartChannelsChannel\n");
    }
}

void dumpBreezePartComponents (FILE * stream, struct BreezePartComponents *components)
{
    if (!components->initialised)
    {
        fprintf (stream, "  (%s)\n", components->contents);
    } else if (components->componentList)
    {
        GList *tmp;

        fprintf (stream, "  (components\n");

        int commentNum = 1;

        for (tmp = g_list_last (components->componentList); tmp; tmp = tmp->prev)
        {
            struct BreezePartComponentsComponent *comp = (struct BreezePartComponentsComponent *) tmp->data;

            dumpBreezePartComponentsComponent (stream, comp, commentNum++);
        }

        fprintf (stream, "  )\n");
    }
}

void dumpBreezePartComponentsComponent (FILE * stream, struct BreezePartComponentsComponent *comp, int commentNum)
{
    if (!comp->initialised)
    {
        fprintf (stream, "    (%s)", comp->contents);
    } else
    {
        fprintf (stream, "    ");
        PrintTMPNode (stream, comp->node);
    }

    if (commentNum > 0)
        fprintf (stream, " ; %d", commentNum);
    fprintf (stream, "\n");
}

void dumpBreezePartCallcontexts (FILE * stream, struct BreezePartCallcontexts *callcontexts)
{
    if (!callcontexts->initialised)
    {
        fprintf (stream, "  (%s)\n", callcontexts->contents);
    } else if (callcontexts->callcontextList)
    {
        GList *tmp;

        fprintf (stream, "  (call-contexts\n");

        for (tmp = g_list_last (callcontexts->callcontextList); tmp; tmp = tmp->prev)
        {
            struct BreezePartCallcontextsCallcontext *callcontext = (struct BreezePartCallcontextsCallcontext *) tmp->data;

            dumpBreezePartCallcontextsCallcontext (stream, callcontext);
        }

        fprintf (stream, "  )\n");
    }
}

void dumpBreezePartCallcontextsCallcontext (FILE * stream, struct BreezePartCallcontextsCallcontext *callcontext)
{
    if (!callcontext->initialised)
    {
        fprintf (stream, "    (%s)\n", callcontext->contents);
    } else
    {
        fprintf (stream, "    (call-context %d \"%s\" ", callcontext->num, callcontext->name);

        dumpPosition (stream, callcontext->callerPosition);
        fprintf (stream, " ");
        dumpPosition (stream, callcontext->calleePosition);
        fprintf (stream, " %d", callcontext->parentNum);

        if (callcontext->channelList)
        {
            GList *tmp;

            fprintf (stream, " (port-channels");
            for (tmp = g_list_last (callcontext->channelList); tmp; tmp = tmp->prev)
                fprintf (stream, " %d", GPOINTER_TO_INT (tmp->data));
            fprintf (stream, ")");
        }

        fprintf (stream, ")\n");
    }
}

void dumpBreezeContexts (FILE * stream, struct BreezeContexts *contexts)
{
    if (!contexts->initialised)
    {
        fprintf (stream, "  (%s)\n", contexts->contents);
    } else if (contexts->contextList)
    {
        dumpBreezeContextsList (stream, contexts->contextList);
    }
}

void dumpBreezeContextsList (FILE * stream, GList * contextsList)
{
    GList *tmp;

    fprintf (stream, "(callContext-contexts\n");

    for (tmp = g_list_last (contextsList); tmp; tmp = tmp->prev)
    {
        struct BreezeContextsContext *context = (struct BreezeContextsContext *) tmp->data;

        dumpBreezeContextsContext (stream, context);
    }

    fprintf (stream, ")\n");
}

void dumpBreezeContextsContext (FILE * stream, struct BreezeContextsContext *context)
{
    if (!context->initialised)
    {
        fprintf (stream, "  (%s)\n", context->contents);
    } else
    {
        fprintf (stream, "  (callContext-context %d \"%s\" ", context->num, context->procName);
        dumpPosition (stream, context->position);
        fprintf (stream, " %d)\n", context->parentContextNum);
    }
}

/* Positions */
struct Position *parsePositionNode (PtrTMPNode node)
{
    GList *list = node->body.list;
    struct Position *pos = g_new0 (struct Position, 1);

    pos->line = atoi (((PtrTMPNode) list->next->data)->body.string);
    pos->column = atoi (((PtrTMPNode) list->next->next->data)->body.string);
    if (list->next->next->next)
    {
        pos->filename = g_strdup (((PtrTMPNode) list->next->next->next->data)->body.string);
        if (list->next->next->next->next)
            pos->context = atoi (((PtrTMPNode) list->next->next->next->next->data)->body.string);
    }
    return pos;
}

struct Position *copyPosition (struct Position *position)
{
    struct Position *pos = g_new0 (struct Position, 1);

    pos->line = position->line;
    pos->column = position->column;
    pos->context = position->context;
    pos->filename = g_strdup (position->filename);
    return pos;
}

void dumpPosition (FILE * stream, struct Position *position)
{
    fprintf (stream, "(at %d %d \"%s\" %d)", position->line, position->column, position->filename ? : "", position->context);
}

char *strdup_printPosition (struct Position *position)
{
    return g_strdup_printf ("(at %d %d \"%s\" %d)", position->line, position->column, position->filename ? : "", position->context);
}

int comparePositions (struct Position *pos1, struct Position *pos2)
{
    if (pos1 && pos2)
    {
        if (pos1->line < pos2->line)
            return -1;
        if (pos1->line > pos2->line)
            return 1;
        if (pos1->column < pos2->column)
            return -1;
        if (pos1->column > pos2->column)
            return 1;
        return 0;
    } else
    {
        if (!pos1 && !pos2)
            return 0;
        if (pos1)
            return 2;
        return -2;
    }
}

int comparePositionsWithContexts (struct Position *pos1, struct Position *pos2)
{
    if (pos1 && pos2 && (pos1->filename == pos2->filename || (pos1->filename && pos2->filename && !strcmp (pos1->filename, pos2->filename))))
    {
        if (pos1->context < pos2->context)
            return -3;
        if (pos1->context > pos2->context)
            return 3;
        if (pos1->line < pos2->line)
            return -1;
        if (pos1->line > pos2->line)
            return 1;
        if (pos1->column < pos2->column)
            return -1;
        if (pos1->column > pos2->column)
            return 1;
        return 0;
    } else
    {
        if (!pos1 && !pos2)
            return 0;
        if (pos1)
            return 2;
        return -2;
    }
}

PtrTMPNode findNodeAtPosition_inner (PtrTMPNode baseNode, struct Position * position, PtrTMPNode parentNode)
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

                if (listIter)
                {
                    PtrTMPNode firstElt = listIter->data;

                    if (firstElt->type == TMPSymbol && !strcmp (firstElt->body.string, "at") && listIter->next && listIter->next->next->next->next)
                    {
                        PtrTMPNode secondElt = listIter->next->data;
                        PtrTMPNode thirdElt = listIter->next->next->data;
                        PtrTMPNode fifthElt = listIter->next->next->next->next->data;

                        if (secondElt->type == TMPNumberStr && thirdElt->type == TMPNumberStr)
                        {
                            int line = atoi (secondElt->body.string);
                            int column = atoi (thirdElt->body.string);
                            int context = atoi (fifthElt->body.string);

                            if (line == position->line && column == position->column && context == position->context)
                                return parentNode;
                        }
                    }
                }

                while (listIter)
                {
                    PtrTMPNode res = findNodeAtPosition_inner (listIter->data, position, baseNode);

                    if (res)
                        return res;
                    listIter = listIter->next;
                }
            }
        }
    return NULL;
}

PtrTMPNode findNodeAtPosition (PtrTMPNode baseNode, struct Position * position)
{
    return findNodeAtPosition_inner (baseNode, position, NULL);
}

PtrTMPNode findFunctionCallNodeInContext_inner (PtrTMPNode baseNode, char *name, int contextNum)
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

                if (listIter)
                {
                    PtrTMPNode firstElt = listIter->data;

                    if (firstElt->type == TMPSymbol
                      && !strcmp (firstElt->body.string, "function-call-expr") && listIter->next && listIter->next->next)
                    {
                        //      if (firstElt->type==TMPSymbol && !strcmp(firstElt->body.string,"function-call-expr") && listIter->next && listIter->next->next->next->next) {
                        PtrTMPNode secondElt = listIter->next->data;
                        PtrTMPNode thirdElt = listIter->next->next->data;

                        if (thirdElt->type == TMPString && !strcmp (thirdElt->body.string, name) && secondElt->type == TMPList)
                        {
                            GList *posList = secondElt->body.list;
                            PtrTMPNode contextElt = posList->next->next->next->next->data;
                            int context = atoi (contextElt->body.string);

                            if (context == contextNum)
                            {
                                fprintf (stderr, "Found something! : ");
                                PrintTMPNode (stderr, baseNode);
                                fprintf (stderr, "\n");
                                return baseNode;
                            }
                        }
                        /*
                           PtrTMPNode fifthElt = listIter->next->next->next->next->data;

                           if (secondElt->type==TMPNumberStr && thirdElt->type==TMPNumberStr) {
                           int line = atoi (secondElt->body.string);
                           int column = atoi (thirdElt->body.string);
                           int context = atoi (fifthElt->body.string);
                           if (line==position->line && column==position->column && context==position->context)
                           return parentNode;
                           }
                         */
                    }
                }

                while (listIter)
                {
                    PtrTMPNode res = findFunctionCallNodeInContext_inner (listIter->data, name,
                      contextNum);

                    if (res)
                        return res;
                    listIter = listIter->next;
                }
            }
        }
    return NULL;
}

PtrTMPNode findFunctionCallNodeInContext (PtrTMPNode baseNode, char *name, int contextNum)
{
    fprintf (stderr, "findFunctionCallNodeInContext \"%s\" in context %d\n", name, contextNum);
    return findFunctionCallNodeInContext_inner (baseNode, name, contextNum);
}

/* Other operations */

void free_key_only (gpointer key, gpointer value, gpointer user_data)
{
    free (key);
}

/* Operations on BreezePartCallcontexts */
void makeCallcontextNamesUnique (GList * list) // GList of struct BreezePartCallcontextsCallcontext *
{
    if (!list || g_list_length (list) < 2)
        return;

    // Every tuple (contextparent-contextname) must be unique
    GHashTable *hashtable = g_hash_table_new (g_str_hash, g_str_equal);

    GList *tmp;

    for (tmp = list; tmp; tmp = tmp->next)
    {
        struct BreezePartCallcontextsCallcontext *context = tmp->data;
        int parentNum = getBreezePartCallcontextsCallcontextParentNum (context);
        char *name = getBreezePartCallcontextsCallcontextName (context);
        char *key = g_strdup_printf ("%d:%s", parentNum, name);
        gpointer value = NULL;
        gpointer old_key;

        g_hash_table_lookup_extended (hashtable, (gconstpointer) key, &old_key, &value);

        if (value == NULL)
        {
            g_hash_table_insert (hashtable, (gpointer) key, (gpointer) context);
        } else
        {
            struct BreezePartCallcontextsCallcontext *prevContext = value;
            char *prevContextName = getBreezePartCallcontextsCallcontextName (prevContext);
            char *ptr = strrchr (prevContextName, '#');
            int prevOccurrence = 1;

            if (!ptr)
                setBreezePartCallcontextsCallcontextName (prevContext, g_strdup_printf ("%s#1", prevContextName));
            else
                prevOccurrence = atoi (ptr + 1);

            setBreezePartCallcontextsCallcontextName (context, g_strdup_printf ("%s#%d", name, prevOccurrence + 1));
            g_hash_table_remove (hashtable, (gpointer) key);
            free (old_key);
            g_hash_table_insert (hashtable, (gpointer) key, (gpointer) context);
        }
    }

    g_hash_table_foreach (hashtable, free_key_only, NULL);
    g_hash_table_destroy (hashtable);
}

struct BreezePartCallcontextsCallcontext *getBreezePartCallcontextsCallcontextByNum (struct BreezePart *breezePart, int context)
{
    GList *callcontexts = getBreezePartCallcontextsList (breezePart);

    for (; callcontexts; callcontexts = callcontexts->next)
    {
        struct BreezePartCallcontextsCallcontext *callcontext = callcontexts->data;
        int numContext = getBreezePartCallcontextsCallcontextNum (callcontext);

        if (numContext == context)
            return callcontext;
    }

    return NULL;
}

gboolean isBreezePartCallcontextsCallcontextNumChildOf (struct BreezePart * breezePart, int child, int parent)
{
    struct BreezePartCallcontextsCallcontext *cc_child = getBreezePartCallcontextsCallcontextByNum (breezePart, child);
    struct BreezePartCallcontextsCallcontext *cc_parent = getBreezePartCallcontextsCallcontextByNum (breezePart, parent);

    if (!cc_child || !cc_parent)
    {
        fprintf (stderr, "Error finding a callcontext in isBreezePartCallcontextsCallcontextNumChildOf\n");
        exit (-1);
    }

    while (cc_child)
    {
        if (cc_child == cc_parent)
            return TRUE;
        cc_child = getBreezePartCallcontextsCallcontextByNum (breezePart, cc_child->parentNum);
    }

    return FALSE;
}

/* Operations on BreezePartComponentsComponent */
char *generateBreezePartComponentsComponentNickname (struct BreezePartComponentsComponent *component)
{
    char *compname = getBreezePartComponentsComponentName (component);
    char *nickname;

    if (!strcmp (compname, "$BrzAdapt"))
        nickname = g_strdup ("Adapt");
    else if (!strcmp (compname, "$BrzCall"))
        nickname = g_strdup (">-");
    else if (!strcmp (compname, "$BrzCallMux"))
        nickname = g_strdup (">- ->");
    else if (!strcmp (compname, "$BrzCallDemux"))
        nickname = g_strdup (">- <-");
    else if (!strcmp (compname, "$BrzCase"))
        nickname = g_strdup ("@");
    else if (!strcmp (compname, "$BrzCaseFetch"))
        nickname = g_strdup ("@->");
    else if (!strcmp (compname, "$BrzCombine"))
        nickname = g_strdup ("<<>>");
    else if (!strcmp (compname, "$BrzCombineEqual"))
        nickname = g_strdup ("<<=>>");
    else if (!strcmp (compname, "$BrzConcur"))
        nickname = g_strdup ("||");
    else if (!strcmp (compname, "$BrzDecisionWait"))
        nickname = g_strdup ("DW");
    else if (!strcmp (compname, "$BrzEncode"))
        nickname = g_strdup ("Enc");
    else if (!strcmp (compname, "$BrzFalseVariable"))
        nickname = g_strdup ("FV");
    else if (!strcmp (compname, "$BrzPassiveEagerFalseVariable"))
        nickname = g_strdup ("peFV");
    else if (!strcmp (compname, "$BrzActiveEagerFalseVariable"))
        nickname = g_strdup ("aeFV");
    else if (!strcmp (compname, "$BrzPassiveSyncEagerFalseVariable"))
        nickname = g_strdup ("peFV");
    else if (!strcmp (compname, "$BrzPassiveEagerNullAdapt"))
        nickname = g_strdup ("peNA");
    else if (!strcmp (compname, "$BrzActiveEagerNullAdapt"))
        nickname = g_strdup ("aeNA");
    else if (!strcmp (compname, "$BrzFetch"))
        nickname = g_strdup ("->");
    else if (!strcmp (compname, "$BrzLoop"))
        nickname = g_strdup ("*");
    else if (!strcmp (compname, "$BrzPassivator"))
        nickname = g_strdup (".");
    else if (!strcmp (compname, "$BrzPassivatorPush"))
        nickname = g_strdup (".");
    else if (!strcmp (compname, "$BrzSynch"))
        nickname = g_strdup (".(s)");
    else if (!strcmp (compname, "$BrzSynchPush"))
        nickname = g_strdup (".(spush)");
    else if (!strcmp (compname, "$BrzSynchPull"))
        nickname = g_strdup (".(spull)");
    else if (!strcmp (compname, "$BrzFork"))
        nickname = g_strdup ("^");
    else if (!strcmp (compname, "$BrzForkPush"))
        nickname = g_strdup ("^");
    else if (!strcmp (compname, "$BrzSequence"))
        nickname = g_strdup (";");
    else if (!strcmp (compname, "$BrzSlice"))
        nickname = g_strdup ("8<");
    else if (!strcmp (compname, "$BrzContinuePush"))
        nickname = g_strdup ("run");
    else if (!strcmp (compname, "$BrzCallDemuxPush"))
        nickname = g_strdup ("-< -> !");
    else if (!strcmp (compname, "$BrzCallActive"))
        nickname = g_strdup ("-< !");
    else if (!strcmp (compname, "$BrzArbiter"))
        nickname = g_strdup ("Arb");
    else if (!strcmp (compname, "$BrzBar"))
        nickname = g_strdup ("[]");
    else if (!strcmp (compname, "$BrzContinue"))
        nickname = g_strdup ("run");
    else if (!strcmp (compname, "$BrzNullAdapt"))
        nickname = g_strdup ("NA");
    else if (!strcmp (compname, "$BrzSplit"))
        nickname = g_strdup (">><<");
    else if (!strcmp (compname, "$BrzSplitEqual"))
        nickname = g_strdup (">>=<<");
    else if (!strcmp (compname, "$BrzWhile"))
        nickname = g_strdup ("do");
    else if (!strcmp (compname, "$BrzWireFork"))
        nickname = g_strdup ("W^");
    else if (!strcmp (compname, "$BrzHalt"))
        nickname = g_strdup ("stop");
    else if (!strcmp (compname, "$BrzHaltPush"))
        nickname = g_strdup ("stop");
    else if (!strcmp (compname, "$BrzVariable"))
    {
        PtrTMPNode node = getBreezePartComponentsComponentTMPNode (component);
        char *varname = ((PtrTMPNode) ((PtrTMPNode) node->body.list->next->next->data)->body.list->next->next->data)->body.string;

        nickname = g_strdup (varname);
    } else if (!strcmp (compname, "$BrzInitVariable"))
    {
        PtrTMPNode node = getBreezePartComponentsComponentTMPNode (component);
        char *varname = ((PtrTMPNode) ((PtrTMPNode) node->body.list->next->next->data)->body.list->next->next->next->data)->body.string;

        nickname = g_strdup (varname);
    } else if (!strcmp (compname, "$BrzConstant"))
    {
        PtrTMPNode node = getBreezePartComponentsComponentTMPNode (component);
        char *value = ((PtrTMPNode) ((PtrTMPNode) node->body.list->next->next->data)->body.list->next->data)->body.string;

        nickname = g_strdup (value);
    } else if (!strcmp (compname, "$BrzBinaryFunc") || !strcmp (compname, "$BrzBinaryFuncPush"))
    {
        PtrTMPNode node = getBreezePartComponentsComponentTMPNode (component);
        char *op = ((PtrTMPNode) ((PtrTMPNode) node->body.list->next->next->data)->body.list->next->next->next->data)->body.string;

        if (!strcmp (op, "NotEquals"))
            nickname = g_strdup ("!=");
        else if (!strcmp (op, "LessThan"))
            nickname = g_strdup ("<");
        else if (!strcmp (op, "GreaterThan"))
            nickname = g_strdup (">");
        else if (!strcmp (op, "LessOrEquals"))
            nickname = g_strdup ("<=");
        else if (!strcmp (op, "GreaterOrEquals"))
            nickname = g_strdup (">=");
        else if (!strcmp (op, "Equals"))
            nickname = g_strdup ("==");
        else if (!strcmp (op, "Add"))
            nickname = g_strdup ("+");
        else if (!strcmp (op, "Subtract"))
            nickname = g_strdup ("-");
        else if (!strcmp (op, "ReverseSubtract"))
            nickname = g_strdup ("(-)");
        else if (!strcmp (op, "And"))
            nickname = g_strdup ("&");
        else if (!strcmp (op, "Or"))
            nickname = g_strdup ("|");
        else if (!strcmp (op, "Xor"))
            nickname = g_strdup ("^");
        else
            nickname = g_strdup ("BinaryFunc");
    } else if (!strcmp (compname, "$BrzBinaryFuncConstR") || !strcmp (compname, "$BrzBinaryFuncConstRPush"))
    {
        PtrTMPNode node = getBreezePartComponentsComponentTMPNode (component);
        char *op = ((PtrTMPNode) ((PtrTMPNode) node->body.list->next->next->data)->body.list->next->next->next->data)->body.string;

        if (!strcmp (op, "NotEquals"))
            nickname = g_strdup ("!=_#");
        else if (!strcmp (op, "LessThan"))
            nickname = g_strdup ("<_#");
        else if (!strcmp (op, "GreaterThan"))
            nickname = g_strdup (">_#");
        else if (!strcmp (op, "LessOrEquals"))
            nickname = g_strdup ("<=_#");
        else if (!strcmp (op, "GreaterOrEquals"))
            nickname = g_strdup (">=_#");
        else if (!strcmp (op, "Equals"))
            nickname = g_strdup ("==_#");
        else if (!strcmp (op, "Add"))
            nickname = g_strdup ("+_#");
        else if (!strcmp (op, "Subtract"))
            nickname = g_strdup ("-_#");
        else if (!strcmp (op, "ReverseSubtract"))
            nickname = g_strdup ("(-)_#");
        else if (!strcmp (op, "And"))
            nickname = g_strdup ("&_#");
        else if (!strcmp (op, "Or"))
            nickname = g_strdup ("|_#");
        else if (!strcmp (op, "Xor"))
            nickname = g_strdup ("^_#");
        else
            nickname = g_strdup ("BinaryFunc_#");
    } else if (!strcmp (compname, "$BrzUnaryFunc") || !strcmp (compname, "$BrzUnaryFuncPush"))
    {
        PtrTMPNode node = getBreezePartComponentsComponentTMPNode (component);
        char *op = ((PtrTMPNode) ((PtrTMPNode) node->body.list->next->next->data)->body.list->next->next->data)->body.string;

        if (!strcmp (op, "Negate"))
            nickname = g_strdup ("-");
        else if (!strcmp (op, "Invert"))
            nickname = g_strdup ("~");
        else
            nickname = g_strdup ("UnaryFunc_#");
    }
    else
        nickname = g_strdup (compname);

    return nickname;
}
