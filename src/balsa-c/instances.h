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

	`instances.h'
	Instances are channel's, sync's, variables
	and constant's; they occupy a common namespace
	
 */

#ifndef INSTANCES_HEADER
#define INSTANCES_HEADER

#include "misclists.h"
#include "values.h"
#include "types.h"
#include "wires.h"

/* BeginInstances : initialise instance handling */
extern void BeginInstances (void);

/* InstanceNature : for the many instance flavours */
typedef enum InstanceNature
{
    ConstantInstance,
    VariableInstance,
    ChannelInstance,
    /* Channels within a procedure ... */
    InputChannelInstance,
    OutputChannelInstance,
    /* Shared call dummy instance */
    SharedCallInstance,
    /* Shared expression */
    ValInstance,
    /* Record element */
    RecordElementInstance,
    ContextMarkerInstance,      /* Context mark in contexts */
    UnresolvableInstance        /* Used to form NoInstance */
}
InstanceNature;

/* PortSense : port sense enumeration for procedure ports */
typedef enum PortSense
{
    DefaultPortSense, ActivePortSense, PassivePortSense
}
PortSense;

/* Forward decl */
struct ComponentList;
struct AccessList;
struct ExprAttributes;

typedef struct Instance
{
    InstanceNature nature;
    union
    {
        struct
        {
            Implicant value;
        }
        constant;
        struct
        {
            bool readOnly;      /* Read only variables are locked channels */
            struct Instance *channelInstance; /* For a locked channel, the ref. to it */
            tPosition position; /* Declaration position */
            PtrMP_INT initialisedValue; /* Value to be loaded on activation, if NULL then this
                                           variable is not initialised */
        }
        variable;
        struct
        {
            PortSense sense;    /* Default == balsa, Active/Passive explicit balsa/breeze */
            PtrWire wire;       /* This should only be used on non arrayed instances */
            PtrLispList options; /* Port options in Breeze file */
            tPosition position; /* Declaration position */
            bool canMulticast;  /* Same rules as channel.canMulticast */
        }
        port;
        struct
        {
            bool canMulticast;  /* Try if this channel can be part of a multicast, note that
                                   this should usually be set unless you intend multicast error
                                   reporting to be used. */
        }
        channel;
        struct
        {
            unsigned offset;
        }
        recordElem;
        struct
        {
            PtrWire activation; /* Activation connected to components */
            PtrWire newActivation; /* Activation generated from accesses */
            struct ComponentList *components; /* Body of val */
            PtrWireList wires;  /* wires of the val */
            struct AccessList *accesses; /* accesses made by the val */
            TypedValue value;   /* value, type is repeated LOTS! */
        }
        val;
    }
    info;                       /* nature variant information */
    PtrType type;               /* Type of this instance */
    Scope scope;                /* declaration scope */
    tIdent ident;               /* Name of instance */
    bool locked;                /* transient property: true whilst in a select/arbitrate body */
    union
    {
        struct Instance *fvar;  /* false variable to which channel is locked iff locked == true */
        struct ExprAttributes *expr; /* expression for a passive output */
    }
    selectionInfo;              /* data used during input/output selection */
}
Instance, *PtrInstance;

/* Declare a list form for Instance */
DECLARE_CONS_LIST_TYPE (Instance)
extern PtrInstance NoInstance;  /* Untypable/not expecting a type */
extern PtrInstance ContextMarkerInstanceObj; /* Element to use as a context marker */

/* NewSharedCallInstance : create a shared call dummy instance */
PtrInstance NewSharedCallInstance (tIdent ident);

/* NewConstantInstance : create a constant */
extern PtrInstance NewConstantInstance (tIdent ident, TypedValue value);

/* New{Variable,Channel}Instance : Create new variable/channel (sync iff type == SyncTypeObj) */
extern PtrInstance NewVariableInstance (tIdent ident, PtrType type);
extern PtrInstance NewChannelInstance (tIdent ident, PtrType type);

/* NewRecordElementInstance : instance relating to an element of a record type */
PtrInstance NewRecordElementInstance (tIdent ident, PtrType type, unsigned offset);

/* NewValInstance : create a val instance from the given params */
extern PtrInstance NewValInstance (tIdent ident, PtrWire activation,
  struct ComponentList *components, PtrWireList wires, struct AccessList *accesses, TypedValue value);

/* LookupInstanceInInstanceList : look up the named instance in the given instance list 
   back to the first context marker iff localLookup is true */
extern PtrInstance LookupInstanceInInstanceList (PtrInstanceList instances, tIdent ident, bool localLookup);

/* InstanceIsLocal : inst is local in instance list insts */
extern bool InstanceIsLocal (PtrInstance inst, PtrInstanceList insts);

/* InstanceCanMulticast : true for channel and port instances which can take part in multicasts */
extern bool InstanceCanMulticast (PtrInstance inst);

/* StrPtrInstance : print out instances,instance descriptions. */
extern void StrPtrInstance (FILE * stream, PtrInstance instance);

#endif /* INSTANCES_HEADER */
