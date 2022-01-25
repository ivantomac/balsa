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

	`commands.c'
	Wrapper for calling external progs/logging std{out,err}
*/

#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>
#include <glib.h>

#include "buffer.h"
#include "main.h"
#include "utils.h"

char *NullCommandArguments[] = { NULL };

void LogCommand (char *command, char **args)
{
    printfConsole (command);
    while (*args)
    {
        printfConsole_nobringtofront (" ");
        printfConsole_nobringtofront (*args);
        args++;
    }
    printfConsole ("\n");
}

void RunCommandWithoutOutput (char *command, char **args)
{
    /* FIXME check command */
    int childPid;               // outPipe[2], errPipe[2];

    LogCommand (command, args + 1);

    childPid = fork ();

    if (childPid == 0)
    { /* in the child process */
        char *comm = g_strjoinv (" ", args);

        system (comm);
        free (comm);
        _exit (0);
    }
}
