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

#ifndef LIBBREEZE_H
#define LIBBREEZE_H

#include <glib.h>
#include "lparse.h"
#include "callcontexttree.h"
#include "threadgroups.h"
#include "flatten.h"

struct Position;

enum BreezePartPortsPortType
{
    PassiveSyncPort, ActiveSyncPort,
    PassiveOutputPort, ActiveOutputPort,
    PassiveInputPort, ActiveInputPort
};

enum BreezePartChannelsChannelType
{
    SyncChannel, PushChannel, PullChannel
};

struct BreezeFile;
struct BreezeImport;
struct BreezeType;
struct BreezePart;
struct BreezePartPorts;
struct BreezePartPortsPort;
struct BreezePartAttributes;
struct BreezePartAttributesAttribute;
struct BreezePartChannels;
struct BreezePartChannelsChannel;
struct BreezePartComponents;
struct BreezePartComponentsComponent;
struct BreezePartCallcontexts;
struct BreezePartCallcontextsCallcontext;
struct BreezeContexts;
struct BreezeContextsContext;

/* Initialising the parsing of a breeze file */
struct BreezeFile *breezeInitParse (const char *filename);

/* Accessing BreezeFile */
char *getBreezeFileName (struct BreezeFile *);
GList *getBreezeImports (struct BreezeFile *);
GList *getBreezeTypes (struct BreezeFile *);
struct BreezeType *getBreezeTypeByName (struct BreezeFile *, const char *name);
GList *getBreezeParts (struct BreezeFile *);
struct BreezePart *getBreezePartByName (struct BreezeFile *, const char *name);
struct BreezePart *getBreezePartByName_searchImports (struct BreezeFile *file, const char *name);
GList *getBreezeContextsList (struct BreezeFile *);
GList *getBreezeContextsList_OrderedByInclusion (struct BreezeFile *);
void setBreezeContextsList (struct BreezeFile *, GList *);

/* Accessing BreezeImport */
char *getBreezeImportName (struct BreezeImport *);

/* Accessing BreezeType */
char *getBreezeTypeName (struct BreezeType *);
PtrTMPNode getBreezeTypeTMPNode (struct BreezeType *);

/* Accessing BreezePart */
char *getBreezePartName (struct BreezePart *);
struct BreezeFile *getBreezePartFile (struct BreezePart *);
GList *getBreezePartPortsList (struct BreezePart *);
GList *getBreezePartAttributesList (struct BreezePart *);
GList *getBreezePartChannelsList (struct BreezePart *);
GList *getBreezePartComponentsList (struct BreezePart *);
GList *getBreezePartCallcontextsList (struct BreezePart *);
void setBreezePartChannelsList (struct BreezePart *, GList *);
void setBreezePartComponentsList (struct BreezePart *, GList *);
void setBreezePartCallcontextsList (struct BreezePart *, GList *);
GHashTable *generateBreezePartCallcontextTree (struct BreezePart *);
GPtrArray *generateBreezePartThreadGroups (struct BreezePart *);

/* Accessing BreezePartPortsPort */
char *getBreezePartPortsPortName (struct BreezePartPortsPort *);
enum BreezePartPortsPortType getBreezePartPortsPortType (struct BreezePartPortsPort *);
gboolean getBreezePartPortsPortIsArray (struct BreezePartPortsPort *);
int getBreezePartPortsPortArrayIndexLow (struct BreezePartPortsPort *);
int getBreezePartPortsPortArraySize (struct BreezePartPortsPort *);
PtrTMPNode getBreezePartPortsPortTMPNode (struct BreezePartPortsPort *);
void setBreezePartPortsPortTMPNode (struct BreezePartPortsPort *, PtrTMPNode);
PtrTMPNode getBreezePartPortsPortDataTypeNode (struct BreezePartPortsPort *);

/* Accessing BreezePartAttributesAttribute_ */
char *getBreezePartAttributesAttributeName (struct BreezePartAttributesAttribute *attribute);
PtrTMPNode getBreezePartAttributesAttributeTMPNode (struct BreezePartAttributesAttribute *attribute);
void setBreezePartAttributesAttributeTMPNode (struct BreezePartAttributesAttribute *attribute, PtrTMPNode node);

/* Accessing BreezePartChannelsChannel */
int getBreezePartChannelsChannelWidth (struct BreezePartChannelsChannel *);
char *getBreezePartChannelsChannelName (struct BreezePartChannelsChannel *);
struct Position *getBreezePartChannelsChannelPosition (struct BreezePartChannelsChannel *);
PtrTMPNode getBreezePartChannelsChannelTypeNode (struct BreezePartChannelsChannel *);
enum BreezePartChannelsChannelType getBreezePartChannelsChannelType (struct BreezePartChannelsChannel *);
void setBreezePartChannelsChannelPosition (struct BreezePartChannelsChannel *, struct Position *);
void setBreezePartChannelsChannelName (struct BreezePartChannelsChannel *, char *);
void setBreezePartChannelsChannelTypeNode (struct BreezePartChannelsChannel *, PtrTMPNode);
struct BreezePartChannelsChannel *DeepCopyBreezePartChannelsChannel (struct BreezePartChannelsChannel *);

/* Accessing BreezePartComponentsComponent */
char *getBreezePartComponentsComponentName (struct BreezePartComponentsComponent *);
gboolean getBreezePartComponentsComponentIsUndeclared (struct BreezePartComponentsComponent *);
PtrTMPNode getBreezePartComponentsComponentTMPNode (struct BreezePartComponentsComponent *);
void setBreezePartComponentsComponentTMPNode (struct BreezePartComponentsComponent *, PtrTMPNode);
struct Position *getBreezePartComponentsComponentPosition (struct BreezePartComponentsComponent *);
struct BreezePartComponentsComponent *DeepCopyBreezePartComponentsComponent (struct BreezePartComponentsComponent *);

/* Accessing BreezePartCallcontextsCallcontext */
int getBreezePartCallcontextsCallcontextNum (struct BreezePartCallcontextsCallcontext *);
char *getBreezePartCallcontextsCallcontextName (struct BreezePartCallcontextsCallcontext *);
int getBreezePartCallcontextsCallcontextParentNum (struct BreezePartCallcontextsCallcontext *);
GList *getBreezePartCallcontextsCallcontextChannelList (struct BreezePartCallcontextsCallcontext *);
void setBreezePartCallcontextsCallcontextNum (struct BreezePartCallcontextsCallcontext *, int);
void setBreezePartCallcontextsCallcontextName (struct BreezePartCallcontextsCallcontext *, char *);
void setBreezePartCallcontextsCallcontextParentNum (struct BreezePartCallcontextsCallcontext *, int);
void setBreezePartCallcontextsCallcontextChannelList (struct BreezePartCallcontextsCallcontext *, GList *);
struct BreezePartCallcontextsCallcontext *DeepCopyBreezePartCallcontextsCallcontext (struct BreezePartCallcontextsCallcontext *);

/* Accessing BreezeContextsContext */
int getBreezeContextsContextNum (struct BreezeContextsContext *);
char *getBreezeContextsContextProcName (struct BreezeContextsContext *);
struct Position *getBreezeContextsContextPosition (struct BreezeContextsContext *context);
int getBreezeContextsContextParentContextNum (struct BreezeContextsContext *);
PtrTMPNode getBreezeContextsContextAssociatedCallNode (struct BreezeContextsContext *);

/* Output */
void dumpBreezeFile (FILE * stream, struct BreezeFile *);
void dumpBreezeImport (FILE * stream, struct BreezeImport *);
void dumpBreezeType (FILE * stream, struct BreezeType *);
void dumpBreezePart (FILE * stream, struct BreezePart *);
void dumpBreezePartPorts (FILE * stream, struct BreezePartPorts *);
void dumpBreezePartPortsPort (FILE * stream, struct BreezePartPortsPort *);
void dumpBreezePartAttributes (FILE * stream, struct BreezePartAttributes *attributes);
void dumpBreezePartAttributesAttribute (FILE * stream, struct BreezePartAttributesAttribute *);
void dumpBreezePartChannels (FILE * stream, struct BreezePartChannels *);
void dumpBreezePartChannelsChannel (FILE * stream, struct BreezePartChannelsChannel *, int commentNum);
void dumpBreezePartComponents (FILE * stream, struct BreezePartComponents *);
void dumpBreezePartComponentsComponent (FILE * stream, struct BreezePartComponentsComponent *, int commentNum);
void dumpBreezePartCallcontexts (FILE * stream, struct BreezePartCallcontexts *);
void dumpBreezePartCallcontextsCallcontext (FILE * stream, struct BreezePartCallcontextsCallcontext *);
void dumpBreezeContexts (FILE * stream, struct BreezeContexts *);
void dumpBreezeContextsList (FILE * stream, GList *);
void dumpBreezeContextsContext (FILE * stream, struct BreezeContextsContext *);

/* Other operations */

/* Operations on BreezePartCallcontexts */
void makeCallcontextNamesUnique (GList *); // GList of struct BreezePartCallcontextsCallcontext *
gboolean isBreezePartCallcontextsCallcontextNumChildOf (struct BreezePart *, int child, int parent);

/* Operations on BreezePartComponentsComponent */
char *generateBreezePartComponentsComponentNickname (struct BreezePartComponentsComponent *);

/* Positions */
struct Position
{
    int line;
    int column;
    int context;
    char *filename;             // an ident would be better
};

struct Position *parsePositionNode (PtrTMPNode);
struct Position *copyPosition (struct Position *);
void dumpPosition (FILE * stream, struct Position *);
char *strdup_printPosition (struct Position *);
int comparePositions (struct Position *, struct Position *);
int comparePositionsWithContexts (struct Position *, struct Position *);
PtrTMPNode findNodeAtPosition (PtrTMPNode, struct Position *);
PtrTMPNode findFunctionCallNodeInContext (PtrTMPNode, char *, int);

#endif
