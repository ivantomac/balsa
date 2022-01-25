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

#ifndef CALLCONTEXTTREE_H
#define CALLCONTEXTTREE_H

extern gboolean libbreeze_TEMPORARY_FLAG_threadgroups;
extern gboolean libbreeze_TEMPORARY_FLAG_behaviourgroups;

struct CallcontextTreeItem;
struct Comp;
struct Chan;

struct CallcontextTreeItem
{
    int type;                   //0=real callcontext, 1=MainLoop, 2=MODule, 3=MODule+mainLoop
    char *altname;
    struct BreezePartCallcontextsCallcontext *callcontext;
    struct CallcontextTreeItem *parent;
    GList *childs;              // list of struct CallcontextTreeItem*
    GList *comps;               // list of struct Comp
    GList *chans;               // list of struct Chan
    GList *ports;               // list of struct Port

    void *data;
    gboolean processed;
};

enum ChanDirection
{ ChanDirection_Sync, ChanDirection_Pull, ChanDirection_Push,
    ChanDirection_Unknown
};
struct Chan
{
    int num;
    struct BreezePartChannelsChannel *channel;
    //    enum ChanType { } type;
    struct CallcontextTreeItem *callcontext;
    struct Comp *source;
    struct Comp *dest;
    enum ChanDirection direction;

    void *data;
};

struct Comp
{
    int num;
    struct BreezePartComponentsComponent *component;
    struct Chan *mainChannel;
    GList *channels;
    struct ThreadGroupsItem *threadGroup;

    void *data;
};

//TMP
struct HashedPortStruct
{
    int port;
    struct BreezePartChannelsChannel *chan;
    int callcontext_source;
    int callcontext_dest;
};
extern GHashTable *ports_hashtable;

extern GPtrArray *allChans;     // array of struct Chan*

//\TMP

struct BreezePart;
void InitBreezePartCallcontextTree (struct BreezePart *part);

void callcontexts_hashtable_foreachWithParent (GHFunc fct);
void CallcontextTree_ForeachTopDown (GHashTable * hash_table, GHFunc func, gpointer user_data);

#endif
