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

	`accesses.h'
	Type for handling unresolved accesses on channels/variables/shared code blocks
	
 */

#ifndef ACCESSES_HEADER
#define ACCESSES_HEADER

#include "wires.h"
#include "components.h"
#include "instances.h"
#include "arith.h"

/* Forward decl */
struct Procedure;
struct ExprAttributesList;

/* Types of access possible on a channel */
typedef enum ChannelAccessNature
{
    ChannelActiveRead,
    ChannelActiveWrite,
    ChannelPassiveRead,
    ChannelPassiveWrite
}
ChannelAccessNature;

/* Access : an Access describes a connection to a channel, variable or shared code block, each connection
   has an associated Wire and also an instance to which it is connected, A AccessList will not contain
   intermediate connections between components without 'named' instances, AccessList's are ordered
   in ascending inst pointer value (lowest inst) -> (lowest inst + n) -> ... -> (highest) -> NULL */
typedef struct Access
{
    PtrInstance inst;           /* Instance leads to ident and nature */
    union
    {
        /* Data for variable accesses */
        struct
        {
            PtrWireList wires;  /* connections to variable */
            PtrMP_INT readMap;  /* OR of all bit ranges on read connections */
            PtrMP_INT writeMap; /* ditto de write */
        }
        variable;
        /* This structure only needs to represent 1 channel access as channel accesses are 
           combined recursively during combinations */
        struct
        {
            PtrWire wire;       /* connection ! */
            ChannelAccessNature nature; /* access nature */
            bool functioningChannel; /* false: this channel does not have exactly one write and at lease one read,
                                        true: already have at least one read on the channel and exactly one write */
            bool unplaced;      /* Don't connect this access up, just check against other accesses in
                                   Parallel/Series combine functions */
            bool nonMulticast;  /* true -> don't allow multicast combinations to be formed with this access,
                                   ie. don't allow Parallel reads */
        }
        channel;
        /* This structure holds the desired activation port number for shared calls */
        struct
        {
            bool unplaced;      /* Usual meaning */
            struct Procedure *procedure; /* Procedure to which we are connecting */
            PtrWire wire;       /* Added activation port access */
        }
        shared;
        /* The activation port for shared val accesses */
        struct
        {
            PtrWire wire;       /* Added activation port access */
        }
        val;
    }
    info;                       /* Union of information structures for the two access flavours */
}
Access, *PtrAccess;

DECLARE_CONS_LIST_TYPE (Access)
/* NewAccess : create an access on instance inst */
extern PtrAccess NewAccess (PtrInstance inst);

/* NewChannelAccess : create an access element for channel access,
   NB. in any given accesslist a channel will only have one access */
extern PtrAccess NewChannelAccess (PtrInstance inst, ChannelAccessNature nature, PtrWire wire);

/* CopyAccess : shallow copy an access */
extern PtrAccess CopyAccess (PtrAccess access);

/* NewVariableAccess : create an access list element for variable access, this is for
   forming leaf not access lists. NB. this creates an access which accesses the range
   (width, offset) specified by the given wire (NB. if the wire is push then this is a write,
   otherwise a read) */
extern PtrAccess NewVariableAccess (PtrInstance inst, PtrWire wire);

/* NewSharedCallAccess : create a request to call procedure procedure */
extern PtrAccess NewSharedCallAccess (struct Procedure *procedure, PtrWire activationPort);

/* NewValAccess : create an access to a val declaration */
extern PtrAccess NewValAccess (PtrInstance valInstance, PtrWire activationPort);

/* CopyAndUnplaceAccessList : copy the access list provided but make all the entries unplaced
   (ie. no wires in variable access (only maps), channel accesses bear unplaced == true,
   nonMulticast == true */
extern PtrAccessList CopyAndUnplaceAccessList (PtrAccessList accesses);

/* ParallelCombineChannels : combine channel accesses left and right to give a new access and a
   component (which is placed on the front on the component list comps),
   report errors if they occur, using position as the error point */
extern PtrAccess ParallelCombineChannels (PtrAccess left,
  PtrAccess right, PtrComponentList * comps, PtrWireList * wires, bool isPermissive, tPosition position);

/* SeriesCombineChannels : Combine channel accesses in series, if ignorePassive is true then allow
   series PassiveReads if one side is unplaced */
extern PtrAccess SeriesCombineChannels (PtrAccess left, PtrAccess right,
  PtrComponentList * comps, PtrWireList * wires, bool ignorePassive, tPosition position);

/* CombineVariables : combine variable accesses (analoguous to CombineChannels), variable
   accesses are not actually combined but the read and write access lists are compared
   and checked for read/write and write/write conflicts if parallelNotSerial is true,
   ie. parallelNotSerial == true : Disallow similtaneous conflicting accesses
   parallelNotSerial == false : Allow left and right accesses to 'conflict' */
extern PtrAccess CombineVariables (PtrAccess left, PtrAccess right, tPosition position, bool parallelNotSerial, bool isPermissive);

/* ParallelCombineAccesses : join two lists of accessess together 
   Adds to the list of components in comp, and wires in wires 
   NB if canGenerateFORK != NULL && left and right accesss 
   communicate with each other (R x W, W x R) then *canGenerateFORK gets false
   (to use this initialise *canGenerateFORK to true)
   If `isPermissive' is true then allow parallel W/W and W/R variable conflicts and W/W channel conflicts */
extern PtrAccessList ParallelCombineAccesses (PtrAccessList left,
  PtrAccessList right, PtrComponentList * comps, PtrWireList * wires, tPosition position, bool * canGenerateFORK, bool isPermissive);

/* SeriesCombineAccesses : similar to parallel combine but without the FORK generation */
extern PtrAccessList SeriesCombineAccesses (PtrAccessList left,
  PtrAccessList right, PtrComponentList * comps, PtrWireList * wires, tPosition position);

/* SharedCombineAccesses : same as SeriesCombine but don't moan about series accesses to passive
   channels where one access is unplaced */
extern PtrAccessList SharedCombineAccesses (PtrAccessList left,
  PtrAccessList right, PtrComponentList * comps, PtrWireList * wires, tPosition position);

/* FilterOutAccessToInstance : remove instance accesses from the given AccessList
   to the given instance, returning the filtered out access */
extern PtrAccess FilterOutAccessToInstance (PtrAccessList * accesses, PtrInstance inst);

/* LookupVarAccess : returns access structure if wire is a variable access in the AccessList accesses */
extern PtrAccess LookupVarAccess (PtrAccessList accesses, PtrWire wire);

/* UpdateVarAccessMap : update one or other of the bit maps in a variable access. isRead == true => update readMap,
   isRead == false => update writeMap */
extern void UpdateVarAccessMap (PtrAccess varAccess, bool isRead);

/* VariableParallelAccessConflictInAccessList : As above but iterate across a pair of access lists */
extern bool VariableParallelAccessConflictInAccessList (PtrAccessList left, PtrAccessList right);

/* MakeReadOnlyVariablesFromInstances : take the given list of attributes and replicate the channel instances
   referred to (making them read only variables).  Iterate down idents too and use those
   names for the read only variables (if not NoIdent).  Only makes variables for accesses in
   `attributes' of nature `accessNature' */
extern PtrInstanceList MakeReadOnlyVariablesFromInstances (struct
  ExprAttributesList *attributes, PtrIdentList idents, ChannelAccessNature accessNature);

/* CopyAndRemapWiresOfAccessList : copy an access list (including copies of all MP_INT maps)
   and change all the wires to those from the wire array/list wireArray */
extern PtrAccessList CopyAndRemapWiresOfAccessList (PtrAccessList accesses, PtrWireArray wireArray);

/* CheckAccessListForNonPortChannels : raise an error if the given accesslist contains references
   to local channels, used to check shared blocks for illegal channel ops, returns true if
   no errors were generated */
extern bool CheckAccessListForNonPortChannels (PtrAccessList accesses);

/* CheckWAWandWAR : return TRUE if a WAW or a WAR access occurs. Return FALSE otherwise */
extern bool CheckWAWandWAR (PtrAccessList left, PtrAccessList right);

/* FindChannelsFromAccessList : returns a list of channels from all the accesses in the given list */
extern PtrInstanceList FindChannelsFromAccessList (PtrAccessList accesses);

/* StrPtrAccess : print out an individual access */
extern void StrPtrAccess (FILE * stream, PtrAccess access);

/* StrPtrAccessList : print out an access list */
extern void StrPtrAccessList (FILE * stream, PtrAccessList list);

#endif /* ACCESSES_HEADER */
