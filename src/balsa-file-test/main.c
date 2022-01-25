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

	`main.c'
	balsa-file-test: file age tests like bash/csh test command
	
 */

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>

/* Usage : print comamnd usage and quit with non-zero return value */
static void Usage (void)
{
    fprintf (stderr,
      "                  _\n"
      " |_  _ |  _ _    [..| _    |_ _   _|_  [ balsa-file-test: file comparison ]\n"
      " |_)(_\\|_/ (_\\ - | ||(-' - |_(-'_/ |_  (C) 2003, The University of Manchester\n\n");
    fprintf (stderr,
      "balsa-file-test <file1> <relation> <file2> {<switch>}*\n\n"
      "where <relation> is one of:\n"
      "    -ot - returns 0 when file1 is older than file2, returns 1 otherwise\n"
      "          (--older-than)\n"
      "    -nt - returns 0 when file1 is newer than file2, returns 1 otherwise\n"
      "          (--newer-than)\n" "switches include:\n"
      "    -t  - return 0 if either file does not exist (--true)\n" "    -f  - return 1 if either file does not exist (--false)\n" "\n");
    exit (1);
}

static bool IgnoreNonExistantFiles = false;
static int NonExistantFileReturnValue = 0;

/* ProcessSwitches : set globals from trailing command line switches */
void ProcessSwitches (char **switches, int switchCount)
{
    int switchNo = 0;

    while (switchNo < switchCount)
    {
        if (strcmp (*switches, "-t") == 0 || strcmp (*switches, "--true") == 0)
        {
            IgnoreNonExistantFiles = true;
            NonExistantFileReturnValue = 0;
        } else if (strcmp (*switches, "-f") == 0 || strcmp (*switches, "--false") == 0)
        {
            IgnoreNonExistantFiles = true;
            NonExistantFileReturnValue = 1;
        } else
        {
            fprintf (stderr, "balsa-file-test: unrecognised switch `%s'\n", *switches);
            Usage ();
        }
        switches++;
        switchNo++;
    }
}

int main (int argc, char **argv)
{
    int ret = 1;
    struct stat file1Stat, file2Stat;
    char *file1, *file2, *relation;

    if (argc < 4)
        Usage ();
    file1 = argv[1];
    file2 = argv[3];
    relation = argv[2];

    ProcessSwitches (argv + 4, argc - 4);

    if (stat (file1, &file1Stat) != 0)
    {
        if (IgnoreNonExistantFiles)
            ret = NonExistantFileReturnValue;
        else
            fprintf (stderr, "balsa-file-test: file `%s' does not exist\n", file1);
    } else if (stat (file2, &file2Stat) != 0)
    {
        if (IgnoreNonExistantFiles)
            ret = NonExistantFileReturnValue;
        else
            fprintf (stderr, "balsa-file-test: file `%s' does not exist\n", file2);
    } else
    {
        if (strcmp (relation, "-ot") == 0 || strcmp (relation, "--older-than") == 0)
            ret = (file1Stat.st_ctime < file2Stat.st_ctime ? 0 : 1);
        else if (strcmp (relation, "-nt") == 0 || strcmp (relation, "--newer-than") == 0)
            ret = (file1Stat.st_ctime > file2Stat.st_ctime ? 0 : 1);
        else
        {
            fprintf (stderr, "balsa-file-test: unrecognised relation `%s'\n", relation);
            Usage ();
        }
    }

    return ret;
}
