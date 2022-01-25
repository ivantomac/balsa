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

#ifndef MAIN_H
#define MAIN_H

#include <gtk/gtk.h>
#include <string.h>
#include <signal.h>

#define BEGINS_WITH(x,y) !strncmp(x,y,strlen(y))
#define ENDS_WITH(x,y) ((strlen(x)>=strlen(y)) && !strcmp(x+strlen(x)-strlen(y),y))
#define CONTAINS(x,y) (strstr(x,y)!=NULL)
#define ASSERT(x) {if (!(x)) {fprintf(stderr,"ASSERT failed\n"); raise(SIGUSR1);}}

extern char *projectName;
extern const char *layoutFilename;
extern char *partName;
extern GtkWidget *MainWindow1, *MainWindow2, *SourceViewWindow;
extern int signal_SIGCHLD_ignoreNext;

enum TraceType
{ TraceAllChans = 1, TracePortsOnly = 2 };
extern enum TraceType getTraceType (void);

enum GTKWaveViewType
{ GTKWaveViewAllTracedChans = 1, GTKWaveViewPortsOnly = 2, GTKWaveViewNamedPortsOnly = 3, GTKWaveViewNone = 4
};
extern enum GTKWaveViewType getGTKWaveViewType (void);

#endif
