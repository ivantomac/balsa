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

#ifndef BREEZESTRUCTS_H
#define BREEZESTRUCTS_H

#include "lparse.h"
#include "libbreeze.h"

struct BreezeFile;

struct BreezeImport
{
    char *name;
};

struct BreezeType
{
    char *contents;
    char *name;

    gboolean initialised;
    PtrTMPNode node;
};

struct BreezePartPortsPort
{
    char *contents;
    char *name;

    gboolean initialised;
    enum BreezePartPortsPortType type;
    gboolean isArray;
    int arrayIndexLow;
    int arraySize;
    PtrTMPNode node;
};

struct BreezePartPorts
{
    char *contents;

    gboolean initialised;
    GList *portList;
};

struct BreezePartAttributesAttribute
{
    char *contents;
    char *name;

    gboolean initialised;
    PtrTMPNode node;
};

struct BreezePartAttributes
{
    char *contents;

    gboolean initialised;
    GList *attributeList;
};

struct BreezePartChannelsChannel
{
    char *contents;

    gboolean initialised;
    enum BreezePartChannelsChannelType type;
    int width;
    struct Position *position;
    char *name;
    PtrTMPNode typeNode;
    GList *otherEntries;
};

struct BreezePartChannels
{
    char *contents;

    gboolean initialised;
    GList *channelList;
};

struct BreezePartComponentsComponent
{
    char *contents;
    char *name;
    gboolean isUndeclared;

    gboolean initialised;
    PtrTMPNode node;
    struct Position *position;
};

struct BreezePartComponents
{
    char *contents;

    gboolean initialised;
    GList *componentList;
};

struct BreezePartCallcontextsCallcontext
{
    char *contents;

    gboolean initialised;
    int num;
    char *name;
    struct Position *callerPosition;
    struct Position *calleePosition;
    int parentNum;
    GList *channelList;
};

struct BreezePartCallcontexts
{
    char *contents;

    gboolean initialised;
    GList *callcontextList;
};

struct BreezePart
{
    char *contents;
    char *name;
    struct BreezeFile *breezeFile;

    gboolean initialised;
    struct BreezePartPorts *ports;
    struct BreezePartAttributes *attributes;
    struct BreezePartChannels *channels;
    struct BreezePartComponents *components;
    struct BreezePartCallcontexts *callcontexts;

    GHashTable *callcontextTree_hashtable;
    GPtrArray *threadGroups;
};

struct BreezeCallContextcompletedump
{
    char *contents;

    gboolean initialised;
    PtrTMPNode node;
};

struct BreezeContextsContext
{
    char *contents;
    struct BreezeFile *breezeFile;

    gboolean initialised;
    int num;
    char *procName;
    struct Position *position;
    int parentContextNum;
};

struct BreezeContexts
{
    char *contents;
    struct BreezeFile *breezeFile;

    gboolean initialised;
    GList *contextList;
};

struct BreezeFile
{
    char *name;
    char *contents;
    PtrTMPNode node;

    GList *imports;
    GList *types;
    GList *parts;
    //    struct BreezeCallContextcompletedump *callContextcompletedump;
    struct BreezeContexts *contexts;
};

#endif
