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

	`balsa-c.c'
	main function / compiler invocation
	
 */

#include "misc.h"
#include "contexts.h"
#include "Tree.h"
#include "BalsaScanSource.h"
#include "output.h"
#include "operators.h"
#include "BalsaScan.h"
#include "Balsa.h"
#include "Semantics.h"
#include "Errors.h"
#include "flags.h"
#include "components.h"
#include "parts.h"
#include "banner.h"
#include "pretty.h"
#include "callcontexts.h"
#include <Idents.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>
#include <stdio.h>

/* StartUp : start up the compiler, initialise all the sub parts */
void StartUp (void)
{
    BeginMisc ();               /* My initialisation */
    BeginComponents ();
    BeginOperators ();
    BeginParts ();
    BeginTypes ();
    BeginSpans ();
    BeginValues ();
    BeginInstances ();
    BeginWires ();
    BeginProcedures ();
    BeginExprs ();
    BeginCommands ();
    BeginPretty ();
}

/* Catch SIGSEGV signals, to print a proper error message when
   we get a stack overflow. This problem is a big one due to the
   recursive parsing library used.
   Here, we need to allocate a replacement stack to replace the
   overflowed one when the SIGSEGV signal is raised.
*/
static void signal_SIGSEGV (int signo, siginfo_t * info, void *uc)
{
    fprintf (stderr,
      "Segmentation fault!\n"
      "This problem can sometimes be solved by increasing your stack limit (try \"ulimit -s unlimited\").\n"
      "Otherwise, it's a real bug... we would be very grateful if you can submit it.\n");

    // Try to remove the badly generated breeze file
    if (BreezeOutputFileName)
        remove (BreezeOutputFileName);

    exit (1);
}
void Catch_SIGSEGV (void)
{
    stack_t altstack;
    char *stack;
    int rc;
    struct sigaction sa;

    stack = malloc (SIGSTKSZ);
    altstack.ss_sp = stack;
    altstack.ss_flags = 0;
    altstack.ss_size = SIGSTKSZ;
    rc = sigaltstack (&altstack, NULL);
    if (rc < 0)
    {
        perror ("sigaltstack");
        exit (1);
    }

    sa.sa_sigaction = signal_SIGSEGV;
    sigemptyset (&sa.sa_mask);
    sa.sa_flags = SA_SIGINFO | SA_ONSTACK;

    sigaction (SIGSEGV, &sa, NULL);
}

int main (int argc, char **argv)
{
    Ptrchar *args = argv + 1;
    Ptrchar blockName;
    int argCount = argc - 1;
    Ptrchar fileName;

    if (argc == 1)
        Usage (*argv);

    Catch_SIGSEGV ();

    /* Resolve path - Use BALSAPATH as base path - if BALSAPATH not present or is empty
       then fall back on DEFAULT_BALSAPATH */
    ImportFileSearchPath = getenv ("BALSAPATH");
    IntermediatePath = NULL;    /* getenv ("BREEZE_OUT_PATH"); */

    if (!ImportFileSearchPath || !*ImportFileSearchPath)
    {
        /* If BALSAHOME is set then use that as the basis for the incluse path */
        char *balsaHome = getenv ("BALSAHOME");

        if (balsaHome && *balsaHome)
        {
            ImportFileSearchPath = NEW_ARRAY (char, strlen (balsaHome) + 20);

            sprintf (ImportFileSearchPath, "%s/share:.", balsaHome);
        } else
            ImportFileSearchPath = DEFAULT_BALSAPATH;
    }
    /* if (!IntermediatePath || !*IntermediatePath) IntermediatePath = "./"; */

    StartUp ();

    FindTabSpacing ();
    MakeCommandLineString (argv, argc);
    ProcessSwitches (&args, &argCount, *argv);
    if (DoBanner)
        printf (BALSA_C_BANNER);

    BeginContexts ();           /* Load balsa.types.builtin */

    fileName = args[0];

    if (strcmp (fileName, "-") != 0)
    {
        /* Process filename */
        /* FILE *dummy = fopen (args[0], "r"); */
        bool foundFile = false;
        size_t fileNameLength = strlen (fileName);

        if (*fileName == '[' && fileName[fileNameLength - 1] == ']')
        {                       /* This is a dotted path */
            fileName = StrNDup (args[0] + 1, fileNameLength - 2);

            SubstituteCharInString (fileName, '.', '/');

            blockName = strrchr (fileName, '/');
            if (!blockName)
                blockName = fileName;
            else
                blockName++;    /* Skip slash */

            fileName = ExpandPathToFile (fileName, "balsa");

            foundFile = !!fileName;
        } else
        {                       /* A file */
            struct stat fileStat;

            stat (fileName, &fileStat);

            if (stat (fileName, &fileStat) != 0 /* Can't open file */
              || ((fileStat.st_mode & S_IFMT) != S_IFREG))
            {
                fileName = NEW_ARRAY (char, strlen (fileName) + 7);

                strcpy (fileName, args[0]);
                strcat (fileName, ".balsa");

                foundFile = stat (fileName, &fileStat) == 0 && ((fileStat.st_mode & S_IFMT) == S_IFREG);
            } else
                foundFile = true;

            if (foundFile)      /* Split name for the blockName */
            {
                char *name, *extension;

                SplitFilename (StrDup (fileName), &name, &blockName, &extension);
            }
        }

        if (foundFile)
        {
            char *iter;

            /* Correct the output path to coincide with input location */
            if (!IntermediatePath || !*IntermediatePath)
            {
                IntermediatePath = StrDup (fileName);
                iter = strrchr (IntermediatePath, '/');
                /* Place a NULL at the last slash */
                if (iter)
                {
                    *iter = '\0';
                } else
                {
                    IntermediatePath = ".";
                }
            }
        } else
            LOG_ERROR (CannotOpenFileToRead, MakeIdent1 (args[0]), NoPosition);

        BalsaScan_BeginFile (fileName);
    } else
        LOG_ERROR (ProcessingFile, MakeIdent1 ("stdin"), NoPosition);

    if (Verbose)
    {
        fprintf (stderr, "-- balsa-c version %s\n", VERSION);
        fprintf (stderr, "-- import file search path: %s\n", ImportFileSearchPath);
        if (strcmp (fileName, "-") == 0)
            fprintf (stderr, "-- processing standard input\n");
        else
            fprintf (stderr, "-- processing file: %s\n", fileName);
    }

    if (DoGenerateBreezeFile)
    {
        BreezeOutputFile = CreateOutputFile (blockName, "breeze", &BreezeOutputFileName);
        WriteOutputFilePreamble (BreezeOutputFile, "Breeze intermediate file (list format)", ";;;");
        fprintf (BreezeOutputFile, ";;; Imports\n");
        if (DoGenerateBasicImports)
        {
            fprintf (BreezeOutputFile, "(import \"balsa.types.builtin\")\n");
            fprintf (BreezeOutputFile, "(import \"balsa.types.synthesis\")\n");
        }
    }

    if (ReportImports)
        printf ("( ");
    /* Parse (and list import files) */
    SyntaxErrors = Balsa ();
    if (ReportImports)
        printf (")\n");

    /* Semantics */
    if (SyntaxErrors == 0 && DoSemantics)
    {
        BeginSemantics ();
        Semantics (TreeRoot);

        if (DoGenerateBreezeFile)
        {
            StrPtrSBreezeContext (BreezeOutputFile, TopLevelContext, !FlattenOutputFile, (FlattenOutputFile ? TOP_LEVEL_SCOPES : TopLevelScope));

            fprintf (BreezeOutputFile, "\n;;; EOF\n");
            fclose (BreezeOutputFile);
        }
    }

    ReportErrors ();
    return 0;                   /* For the compiler */
}
