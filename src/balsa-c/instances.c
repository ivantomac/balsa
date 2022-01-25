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

	`instances.c'
	Instances are channel's, sync's, variables
	and constant's; they occupy a common namespace
	
 */

#include "instances.h"
#include "components.h"
#include "accesses.h"
#include "ports.h"

/* BeginInstances : initialise instance handling */
void BeginInstances (void)
{
    NoInstance = NEW (Instance);
    NoInstance->ident = MarkerIdent;
    NoInstance->type = NoType;
    NoInstance->nature = UnresolvableInstance;
    NoInstance->locked = false;
    NoInstance->selectionInfo.fvar = NULL;
    NoInstance->scope = ImportedScope;

    ContextMarkerInstanceObj = NEW (Instance);
    ContextMarkerInstanceObj->ident = MarkerIdent;
    ContextMarkerInstanceObj->type = NoType;
    ContextMarkerInstanceObj->nature = ContextMarkerInstance;
    ContextMarkerInstanceObj->locked = false;
    ContextMarkerInstanceObj->selectionInfo.fvar = NULL;
    ContextMarkerInstanceObj->scope = ImportedScope;
}

PtrInstance NoInstance = NULL;  /* Untypable/not expecting a type */
PtrInstance ContextMarkerInstanceObj = NULL; /* Element to use as a context marker */

DEFINE_CONS_LIST_TYPE (Instance)
/* NewSharedCallInstance : create a shared call dummy instance */
PtrInstance NewSharedCallInstance (tIdent ident)
{
    PtrInstance instance = NEW (Instance);

    instance->ident = ident;
    instance->nature = SharedCallInstance;
    instance->scope = ImportedScope;
    return instance;
}

/* NewConstantInstance : create a constant */
PtrInstance NewConstantInstance (tIdent ident, TypedValue value)
{
    PtrInstance instance = NEW (Instance);

    instance->ident = ident;
    instance->type = value.type;
    instance->nature = ConstantInstance;
    instance->info.constant.value = value.value;
    instance->locked = false;
    instance->selectionInfo.fvar = NULL;
    instance->scope = ImportedScope;
    return instance;
}

/* LookupInstanceInInstanceList : look up the named instance in the given instance list 
   back to the first context marker iff localLookup is true */
PtrInstance LookupInstanceInInstanceList (PtrInstanceList instances, tIdent ident, bool localLookup)
{
    /* FOR_EACH (instances, (localLookup ? CAR (instances)->nature != ContextMarkerInstance : true), ...) */
    while (instances && (localLookup ? CAR (instances)->nature != ContextMarkerInstance : true))
    {
        if (CAR (instances)->ident == ident)
            return CAR (instances);
        instances = CDR (instances);
    }
    return NULL;
}

/* New{Variable,Channel}Instance : Create new variable/channel (sync iff type == SyncTypeObj) */
PtrInstance NewVariableInstance (tIdent ident, PtrType type)
{
    PtrInstance instance = NEW (Instance);

    instance->ident = ident;
    instance->type = type;
    instance->nature = VariableInstance;
    instance->locked = false;
    instance->selectionInfo.fvar = NULL;
    instance->info.variable.readOnly = false;
    instance->info.variable.channelInstance = NULL;
    instance->info.variable.position = NoPosition;
    instance->info.variable.initialisedValue = NULL;
    instance->scope = ImportedScope;
    return instance;
}

PtrInstance NewChannelInstance (tIdent ident, PtrType type)
{
    PtrInstance instance = NEW (Instance);

    instance->ident = ident;
    instance->type = type;
    instance->nature = ChannelInstance;
    instance->locked = false;
    instance->selectionInfo.fvar = NULL;
    instance->scope = ImportedScope;
    /* Just to make sure ... */
    instance->info.channel.canMulticast = true;
    return instance;
}

/* NewRecordElementInstance : instance relating to an element of a record type */
PtrInstance NewRecordElementInstance (tIdent ident, PtrType type, unsigned offset)
{
    PtrInstance instance = NEW (Instance);

    instance->ident = ident;
    instance->type = type;
    instance->nature = RecordElementInstance;
    instance->info.recordElem.offset = offset;
    instance->locked = false;
    instance->selectionInfo.fvar = NULL;
    instance->scope = ImportedScope;
    return instance;
}

/* NewValInstance : create a val instance from the given params */
PtrInstance
NewValInstance (tIdent ident, PtrWire activation, PtrComponentList components, PtrWireList wires, PtrAccessList accesses, TypedValue value)
{
    PtrInstance instance = NEW (Instance);

    instance->ident = ident;
    instance->nature = ValInstance;
    instance->type = value.type;
    instance->info.val.activation = activation;
    instance->info.val.components = components;
    instance->info.val.wires = wires;
    instance->info.val.value = value;
    instance->info.val.accesses = accesses;
    instance->locked = false;
    instance->selectionInfo.fvar = NULL;
    instance->scope = ImportedScope;
    return instance;
}

/* InstanceIsLocal : inst is local in instance list insts */
bool InstanceIsLocal (PtrInstance inst, PtrInstanceList insts)
{
    /* FOR_EACH (insts, CAR (insts)->nature != ContextMarkerInstance, ...) */
    while (insts && CAR (insts)->nature != ContextMarkerInstance)
    {
        if (CAR (insts) == inst)
            return true;
        insts = CDR (insts);
    }
    return false;
}

/* InstanceCanMulticast : true for channel and port instances which can take part in multicasts */
bool InstanceCanMulticast (PtrInstance inst)
{
    bool ret = false;

    switch (inst->nature)
    {
    case ChannelInstance:
        ret = inst->info.channel.canMulticast;
        break;
    case InputChannelInstance:
    case OutputChannelInstance:
        ret = inst->info.port.canMulticast;
        break;
    default:
        break;
    }

    return ret;
}

/* StrNameColonType : write out `prefix ident : type' */
void StrNameColonType (FILE * stream, char *prefix, tIdent ident, PtrType type)
{
    if (prefix)
        fprintf (stream, "%s ", prefix);
    WriteIdent (stream, ident);
    fprintf (stream, " : ");
    StrPtrTypeName (stream, type);
}

/* StrPtrChannelInstance : print out a channel instance, taking note of sync and
   arrayed types */
void StrPtrChannelInstance (FILE * stream, PtrInstance instance)
{
    PtrType type = instance->type;

    if (instance->type->nature == ArrayedType)
    {
        fprintf (stream, "array ");
        StrSpan (stream, instance->type->info.arrayed.range, true /* long form */ );
        fprintf (stream, " of ");
        type = instance->type->info.arrayed.baseType;
    }
    if (instance->nature != ChannelInstance)
    {
        StrPortSense (stream, instance->info.port.sense);
        putc (' ', stream);
    }
    if (type->nature == SyncType)
    {
        fprintf (stream, "sync ");
        WriteIdent (stream, instance->ident);
    } else
    {
        StrNameColonType (stream,
          (instance->nature == ChannelInstance ? "channel" : (instance->nature == InputChannelInstance ? "input" : "output")), instance->ident, type);
    }
    if (instance->locked)
        fprintf (stream, " (-- locked --) ");
}

/* StrPtr{Instance(List)?} : Print out instances,instance descriptions. */
void StrPtrInstance (FILE * stream, PtrInstance instance)
{
    if (instance)
    {
        switch (instance->nature)
        {
        case ConstantInstance:
            fprintf (stream, "constant ");
            WriteIdent (stream, instance->ident);
            fprintf (stream, " = (");
            if (instance->info.constant.value.dontCares)
                StrImplicant (stream, instance->info.constant.value, true);
            else
                StrPtrMP_INT (stream, instance->info.constant.value.baseValue);
            fprintf (stream, " as ");
            StrPtrTypeName (stream, instance->type);
            putc (')', stream);
            break;
        case RecordElementInstance:
            StrNameColonType (stream, NULL, instance->ident, instance->type);
            break;
        case VariableInstance:
            StrNameColonType (stream, "variable", instance->ident, instance->type);
            if (instance->info.variable.readOnly)
                fprintf (stream, " (-- read only --)");
            break;
        case ChannelInstance:
        case InputChannelInstance:
        case OutputChannelInstance:
            StrPtrChannelInstance (stream, instance);
            break;
        case ValInstance:
            fprintf (stream, "val ");
            WriteIdent (stream, instance->ident);
            fprintf (stream, " = ( ? as ");
            StrPtrTypeName (stream, instance->type);
            fprintf (stream, ")");
            break;
        case ContextMarkerInstance:
            fprintf (stream, "-- context-marker");
            break;
        case UnresolvableInstance:
            fprintf (stream, "NoInstance");
            break;
        default:
            break;
        }
    } else
        fprintf (stream, "NULL");
}
