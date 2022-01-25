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

#include "threadgroups.h"
#include "libbreeze.h"
#include "breezestructs.h"
#include "callcontexttree.h"

//#define DEBUG
#ifdef DEBUG
#include <signal.h>
#define ASSERT(x) {if (!(x)) {fprintf(stderr,"ASSERT failed\n"); raise(SIGUSR1);}}
#else
#define ASSERT(x)
#endif

void InitBreezePartThreadGroups (struct BreezePart *breezePart)
{
    breezePart->threadGroups = g_ptr_array_new ();

    //    ReadChans (breezePart);
    //    ReadComps (breezePart);
}

void ThreadGroups_Foreach (struct BreezePart *breezePart, GHFunc func, gpointer user_data)
{
    unsigned int i;

    for (i = 0; i < breezePart->threadGroups->len; i++)
    {
        struct ThreadGroupsItem *threadGroup = g_ptr_array_index (breezePart->threadGroups, i);

        func (threadGroup, user_data, 0);
    }
}
