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

	`flags.c'
	Global compilation flags and options
	
 */

#include "flags.h"
#include "misc.h"
#include "banner.h"
#include <unistd.h>
#include <pwd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

bool DoGenerateBreezeFile = true;
bool DoGenerateBasicImports = true;
bool DoOptimise = true;
bool DoSemantics = true;
bool DoBanner = true;
bool DoAndrewsMagicSwitch = false;
bool DoLAPsMagicSwitch = false;
bool DoLinkBreeze = false;
bool UseWireForks = true;
bool AllowSequentialSelection = false;
bool ReportImports = false;
bool FlattenOutputFile = false;
bool SourceDecorateErrorMessages = false;
bool Verbose = false;
bool DontReadBuiltin = false;
bool DontGenerateEagerFVs = false;
bool DontOptimiseSequencers = false;

char *ImportFileSearchPath = NULL;
char *IntermediatePath = NULL;

unsigned TabDistance = 1;

/* FIXME add command line options */
bool NwaySequenceOptimise = true;
bool NwayConcurOptimise = true;
bool FlattenProcedureCalls = false;

/* Compilation options */
bool VariableReadSplit = false;
bool NoEagerInputs = false;
bool WarnMulticast = true;

ErrorType DefaultPrintErrorType = RuntimePrintCommand;

/* Usage : display balsa-c info and usage on stderr then exit with errorcode -1 */
void Usage (char *argv0)
{
    fprintf (stderr, BALSA_C_BANNER);

    fprintf (stderr, "version %s\nusage: %s {<switch>}* <block/file-name>\n\n"
      "switches: -I <path>      - append <path> to import file path       (--import)\n"
      "          -e             - discard import path                     (--discard-import)\n"
      "          -o <directory> - directory for output intermediate files (--output)\n"
      "          -O             - DON'T optimise generated HC's           (--no-optimise)\n"
      "          -b             - inhibit banner                          (--no-banner)\n"
      "          -t <distance>  - tabs indent by <distance> places        (--tab)\n"
      "          -v             - be verbose                              (--verbose)\n"
      "          -c <option>    - compilation option                      (--compile-option)\n"
      "                           <option> can be:\n"
      "                             allow-sequential-selection -- allow the generation of the non-delay-insensitive\n"
      "                                                           BrzCallDemuxPush and BrzActive components\n"
      "                             no-sequencer-opt           -- don't optimise the contents of sequencers\n"
      "                           Deprecated code generation features:\n"
      "                             var-read-split             -- split variables on read bitfields as well as writes\n"
      "                             no-wire-forks              -- don't use the WireFork component as a replacement\n"
      "                                                           for permanent Forks\n"
      "                             no-eager-inputs            -- ignore !s on select and ->\n"
      "          --             - accept no more switches\n"
      "\n"
      "          -B             - don't generate a Breeze file                                (--no-breeze)\n"
      "          -F             - generate a flat Breeze file                                 (--flatten-files)\n"
      "          -i             - suppress `import [balsa.types.synthesis]' line in output    (--no-imports-in-output)\n"
      "          -j             - don't read [balsa.types.builtin] on startup                 (--no-builtin)\n"
      "          -p             - decorate error/warning messages with balsa-c source pos.    (--error-positions)\n"
      "          -M             - report a list of imported blocks on which this file depends (--depends)\n"
      "          -P <type>      - default print command behaviour.  Type can be:\n"
      "                             (runtime|report|error|warning|fatal)                      (--print-type)\n" "\n", VERSION, argv0);
    exit (-1);
}

/* FindTabSpacing : search the .exrc file (if any) to find an appropriate tab
   stop directive */
void FindTabSpacing (void)
{
    struct passwd *passEntry = getpwuid (getuid ());
    char tmp[1024];
    FILE *exrc;

    if (!passEntry)
        return;
    strcpy (tmp, passEntry->pw_dir);
    strcat (tmp, "/.exrc");

    exrc = fopen (tmp, "r");
    if (!exrc)
        return;

    while (fgets (tmp, 1024, exrc))
    {
        char *token1, *token2, *token3;

        token1 = strtok (tmp, " \t=");
        token2 = strtok (NULL, " \t=");
        token3 = strtok (NULL, " \t=");

        if (token1 && token2 && token3 && strcmp (token1, "set") == 0 && strcmp (token2, "ts") == 0)
        {
            TabDistance = atoi (token3);
        }
    }
}

/* MakeCommandLineString : create a string which is returned and also held internally in a static
   variable which consists of the space separated list of command line arguments passed to balsa-c
   (including argv[0]) */
char *MakeCommandLineString (char **argv, int argc)
{
    static char *ret = NULL;

    if (!ret)                   /* compose str */
    {
        int i;
        int stringAllocatedLength = argc + 1; /* One char for each space, one char for \0 */

        for (i = 0; i < argc; i++)
        {
            stringAllocatedLength += strlen (argv[i]);
        }

        ret = NEW_ARRAY (char, stringAllocatedLength);

        *ret = '\0';

        for (i = 0; i < argc; i++)
        {
            strcat (ret, argv[i]);
            strcat (ret, " ");
        }
    }
    return ret;
}

/* BalsaCommandLineSwitches : switches acceptable to balsa-c */
typedef enum
{
    LastSwitch,                 /* -- */
    BareHyphenSwitch,           /* - */
    ImportSwitch,
    DiscardImportSwitch,
    OutputFilenameSwitch,
    FlattenCallsSwitch,
    DontOptimiseSwitch,
    NoBannerSwitch,
    TabDistanceSwitch,
    VerboseSwitch,
    NoBreezeSwitch,
    FlattenFilesSwitch,
    NoImportInOutputSwitch,
    ErrorPositionSwitch,
    DependsSwitch,
    CompileOptionSwitch,
    PrintTypeSwitch,
    AndrewsMagicSwitch,
    LAPsMagicSwitch,
    LinkBreezeSwitch,
    NoBuiltinSwitch,
    NoEagerSwitch,
    NoSwitch
}
BalsaCommandLineSwitch;

typedef struct
{
    char shortName;
    char *longName;
    BalsaCommandLineSwitch switchNature;
}
CommandLineSwitch;

CommandLineSwitch BalsacSwitches[] = {
    {'-', "", LastSwitch},
    {'I', "import", ImportSwitch},
    {'e', "discard-import", DiscardImportSwitch},
    {'o', "output", OutputFilenameSwitch},
    {'f', "flatten-calls", FlattenCallsSwitch},
    {'O', "no-optimise", DontOptimiseSwitch},
    {'b', "no-banner", NoBannerSwitch},
    {'t', "tab", TabDistanceSwitch},
    {'v', "verbose", VerboseSwitch},
    {'B', "no-breeze", NoBreezeSwitch},
    {'F', "flatten-files", FlattenFilesSwitch},
    {'i', "no-imports-in-output", NoImportInOutputSwitch},
    {'p', "error-positions", ErrorPositionSwitch},
    {'M', "depends", DependsSwitch},
    {'c', "compile-option", CompileOptionSwitch},
    {'P', "print-type", PrintTypeSwitch},
    {'_', "ab", AndrewsMagicSwitch},
    {'_', "lap", LAPsMagicSwitch},
    {'l', "link-breeze", LinkBreezeSwitch},
    {'j', "no-builtin", NoBuiltinSwitch},
    {'_', "no-eager", NoEagerSwitch},
    {'\0', NULL, NoSwitch}
};

/* ParseSwitch : match the given switch against the list above and return the
	switchNature portion if there is a match or NoSwitch otherwise.  For
	switches of just "-", return BareHyphenSwitch */
BalsaCommandLineSwitch ParseSwitch (char *swtch)
{
    CommandLineSwitch *patterns = BalsacSwitches;

    if (strcmp (swtch, "-") == 0)
        return BareHyphenSwitch;

    if (swtch[0] != '-')
        return NoSwitch;        /* Not -... */
    if (swtch[1] == '-')        /* Long Switch */
    {
        while (patterns->switchNature != NoSwitch)
        {
            if (patterns->longName && strcmp (swtch + 2, patterns->longName) == 0)
                return patterns->switchNature;
            patterns++;
        }
    } else if (swtch[1] && swtch[2] == '\0') /* Short option */
    {
        while (patterns->switchNature != NoSwitch)
        {
            if (patterns->shortName != '_' && swtch[1] == patterns->shortName)
                return patterns->switchNature;
            patterns++;
        }
    }
    return NoSwitch;
}

/* ProcessSwitches : strip off switches leaving *argv == input_file, argc == 1 */
void ProcessSwitches (char ***argv, int *argc, char *programName)
{
    bool badExit = false;
    bool lastSwitch = false;

    while (!lastSwitch && *argc && ***argv == '-')
    {
        switch (ParseSwitch ((*argv)[0]))
        {
        case DontOptimiseSwitch:
            DoOptimise = false;
            break;
        case DependsSwitch:    /* list of imported blocks */
            DoBanner = false;
            DoSemantics = false;
            ReportImports = true;
            DoGenerateBreezeFile = false;
            break;
        case OutputFilenameSwitch: /* Intermediate file directory */
            if (*argc == 1)
                fprintf (stderr, "    -o switch requires a directory name argument\n");
            else
            {
                size_t IntermediatePathLength = strlen ((*argv)[1]);
                IntermediatePath = NEW_ARRAY (char, IntermediatePathLength + 2);

                strcpy (IntermediatePath, (*argv)[1]);
                if ((*argv)[1][IntermediatePathLength - 1] != '/')
                {
                    strcat (IntermediatePath, "/");
                }
                (*argc)--;
                (*argv)++;
            }
            break;
        case DiscardImportSwitch: /* Don't use BALSAPATH */
            ImportFileSearchPath = "";
            break;
        case ImportSwitch:     /* Add to import path */
            if (*argc == 1)
                fprintf (stderr, "    -I switch requires a path argument\n");
            else
            {
                char *newPath = (char *) Alloc (strlen (ImportFileSearchPath) + strlen ((*argv)[1]) + 2);

                sprintf (newPath, "%s:%s", ImportFileSearchPath, (*argv)[1]);
                /* Don't bother to deallocate original string - not worth the
                   hassle */
                ImportFileSearchPath = newPath;
                (*argc)--;
                (*argv)++;
            };
            break;
        case FlattenCallsSwitch:
            printf ("Warning: -f (--flatten-calls) option deprecated and removed\n");
            //            FlattenProcedureCalls = true;
            break;
        case FlattenFilesSwitch:
            FlattenOutputFile = true;
            break;
        case NoBannerSwitch:
            DoBanner = false;
            break;
        case ErrorPositionSwitch:
            SourceDecorateErrorMessages = true;
            break;
        case TabDistanceSwitch: /* Tab handling */
            if (*argc == 1)
                fprintf (stderr, "    -t switch requires a numeric argument\n");
            else
            {
                int tabSize = atoi ((*argv)[1]);

                if (tabSize < 0)
                    fprintf (stderr, "    -t switch requires a positive numeric argument\n");
                else
                    TabDistance = tabSize;

                fprintf (stderr, "Tab size %d\n", TabDistance);
                (*argc)--;
                (*argv)++;
            }
            break;
        case CompileOptionSwitch:
            if (*argc == 1)
                fprintf (stderr, "    -c switch requires an option name argument\n");
            else
            {
                char *option = (*argv)[1];

                if (strcmp (option, "var-read-split") == 0)
                    VariableReadSplit = true;
                else if (strcmp (option, "warn-multicast") == 0)
                {
                    /* Deprecated */
                    /* WarnMulticast = true; */
                } else if (strcmp (option, "no-wire-forks") == 0)
                    UseWireForks = false;
                else if (strcmp (option, "no-eager-inputs") == 0)
                    NoEagerInputs = true;
                else if (strcmp (option, "allow-sequential-selection") == 0)
                    AllowSequentialSelection = true;
                else if (strcmp (option, "no-sequencer-opt") == 0)
                    DontOptimiseSequencers = true;
                else
                    fprintf (stderr, "    -c invalid compile option `%s'", option);

                (*argc)--;
                (*argv)++;
            }
            break;
        case NoBreezeSwitch:   /* Don't generate Breeze */
            DoGenerateBreezeFile = false;
            break;
        case NoImportInOutputSwitch: /* Basic types import line */
            DoGenerateBasicImports = false;
            break;
        case PrintTypeSwitch:
            if (*argc == 1)
                fprintf (stderr, "    -P switch requires an argument\n");
            else
            {
                Ptrchar printType = (*argv)[1];

                if (strcmp (printType, "runtime") == 0)
                    DefaultPrintErrorType = RuntimePrintCommand;
                else if (strcmp (printType, "report") == 0)
                    DefaultPrintErrorType = ReportPrintCommand;
                else if (strcmp (printType, "error") == 0)
                    DefaultPrintErrorType = ErrorPrintCommand;
                else if (strcmp (printType, "warning") == 0)
                    DefaultPrintErrorType = WarningPrintCommand;
                else if (strcmp (printType, "fatal") == 0)
                    DefaultPrintErrorType = FatalPrintCommand;
                else
                    fprintf (stderr, "    -P switch only accepts (runtime|report|error|warning|fatal) argument\n");

                (*argc)--;
                (*argv)++;
            }
            break;
        case VerboseSwitch:    /* Verbose errors */
            Verbose = true;
            DoBanner = false;
            break;
        case LastSwitch:       /* End of switches */
        case BareHyphenSwitch: /* stdin as input file */
            lastSwitch = true;
            break;
        case AndrewsMagicSwitch:
            DoAndrewsMagicSwitch = true;
            break;
        case LAPsMagicSwitch:
            DoLAPsMagicSwitch = true;
            break;
        case NoBuiltinSwitch:
            DontReadBuiltin = true;
            break;
        case NoEagerSwitch:
            DontGenerateEagerFVs = true;
            break;
        case LinkBreezeSwitch:
            DoLinkBreeze = true;
            break;
        default:
            fprintf (stderr, "    invalid command line switch `%s'\n", **argv);
            badExit = true;
            lastSwitch = true;
            break;
        }
        (*argc)--;
        (*argv)++;
    }

    if (*argc != 1)
        badExit = true;
    if (badExit)
        Usage (programName);
}
