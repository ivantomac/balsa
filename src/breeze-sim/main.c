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

#include "simulate.h"
#include "breezesim_plugin.h"
#include "components_4phase.h"
#include "trace.h"
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <glib.h>
#include <gmodule.h>
#ifdef MPI
#include <mpi.h>
#endif

#include "balsasim/builtin.h"
#include "balsasim/bstring.h"
#include "balsasim/bfile.h"

int flush_delay = 1;
gboolean ForkGTKWave = FALSE;
char *TraceFileName = NULL;

unsigned IncludePathAllocatedLength = 0;
unsigned IncludePathLength = 0;
char **IncludePath = NULL;

struct BreezePart *breezePart = NULL;

gboolean ActAsLinker = FALSE;
char *LinkFileName = NULL;
gboolean NoDelayCoefs = FALSE;
int StopAtTime = 0;
gboolean StopAtTimeReached = FALSE;
gboolean HLA_activated = FALSE;

#ifdef MPI
int my_mpi_rank;
int MPI_arg1 = 0;
#endif

//#define SAMIPS_EXPERIMENTS
#ifdef SAMIPS_EXPERIMENTS
#include <sys/timeb.h>
struct timeb start_time, end_time;

void do_something (void)
{
    int i;

    for (i = 0; i < 1000; i++)
    {
        double j = sqrt ((double) i);

        j++;
    }
}
#endif

void CloseBreezeSimulation (int type) // type= 0:normal finish 1:deadlock 2:error
{
    char *message[] = { "finished", "deadlocked", "error" };

#if defined(EVENT_DRIVEN_SCHEDULER) || defined(REORDERED_EVENT_DRIVEN_SCHEDULER)
    Trace_CommitUntilTime (-1);
#endif

    if (TraceFile)
        fprintf (TraceFile, "\n$comment\nend\n$end\n");

    fflush (0);

    if (DebugFlag)
        printf ("%s%s at time: %lld\n\tlast activity at: %lld\n",
          StopAtTimeReached ? "Stopped_at_time & " : "", message[type], current_real_time, last_event_time);
    else
        printf ("Activity %s%s at time %lld\n", StopAtTimeReached ? "stopped_at_time & " : "", message[type], last_event_time);

#ifdef SAMIPS_EXPERIMENTS
    ftime (&end_time);
    fprintf (stderr, "%d %d => %d %d\n", end_time.time, end_time.millitm, end_time.time - start_time.time, end_time.millitm - start_time.millitm);
#endif

#if defined(MPI) && defined(REORDERED_EVENT_DRIVEN_SCHEDULER)
    /* Shut down MPI */
    printf ("%d sending KILLALL message\n", my_mpi_rank);
    MPI_Send (NULL, 0, MPI_INT, 2 /*dest */ , MPI_TAG__KILLALL, MPI_COMM_WORLD);
    MPI_Finalize ();
#endif

    if (type == 2)
    {
        printf ("Failed event time: %lld\n", current_real_time);
        exit (EXIT_FAILURE);
    } else
        exit (EXIT_SUCCESS);
}

static void BreezeTermination (struct comp *c, int port)
{
#if defined(EVENT_DRIVEN_SCHEDULER) || defined(REORDERED_EVENT_DRIVEN_SCHEDULER)
#else
    CloseBreezeSimulation (0);
#endif
}

static void signal_ALARM (int a)
{
    fflush (stdout);
    signal (SIGALRM, (void *) signal_ALARM);
    alarm (flush_delay);
}

static void signal_TERM (int a)
{
    printf ("Program killed\n");
    fflush (stdout);
    CloseBreezeSimulation (2);
}

/* ReplaceCharForCharInString : replace each `from' with a `to', returns the string */
static char *ReplaceCharForCharInString (char *string, char from, char to)
{
    char *ret = string;

    do
    {
        string = strchr (string, from);
        if (string)
        {
            *string = to;
            string++;
        }
    }
    while (string && *string);

    return ret;
}

/* FindBuiltinLibSharedLibrary : find the path of the shared library file
	for the named builtin library.  `paths' contains a GList of directories
	to search */
char *FindBuiltinLibSharedLibrary (const char *libraryName, GList * paths, bool reportErrors)
{
    char *dlFilename = NULL;
    FILE *laFile = NULL;
    char line[1024];
    char testFilename[PATH_MAX * 2 + 10]; /* 10 > strlen (".breeze") */
    char slashedLibraryName[strlen (libraryName) + 1];

    strcpy (slashedLibraryName, libraryName);
    ReplaceCharForCharInString (slashedLibraryName, '.', '/');

    while (paths && !laFile)
    {
        /* There must be a breeze file next to the .la */
        sprintf (testFilename, "%s/%s.breeze", (char *) paths->data, slashedLibraryName);
        if (access (testFilename, F_OK) == 0)
        {
            sprintf (testFilename, "%s/%s.la", (char *) paths->data, slashedLibraryName);
            laFile = fopen (testFilename, "rt");
        }
        errno = 0;

        paths = g_list_next (paths);
    }

    if (!laFile)
    {
        if (reportErrors)
            fprintf (stderr, "Can't find .la file for builtin library `%s'\n", libraryName);
        return NULL;
    }

    /* testFilename contains the correct path with the trailing file name,
       put a NUL at the first filename pos. to produce just a path */
    char *lastSlash = strrchr (testFilename, '/');

    if (lastSlash)
        lastSlash[1] = '\0';

    /* Find the dlFilename line */
    while (fgets (line, 1024, laFile))
    {
        if (strncmp (line, "dlname='", 8) == 0)
        {
            dlFilename = malloc (strlen (line) + strlen (testFilename) + 2 - 8);

            sprintf (dlFilename, "%s/%s", testFilename, line + 8);
            break;
        }
    }
    fclose (laFile);

    /* Trim the name to the last ' */
    if (dlFilename)
    {
        char *ptr = strrchr (dlFilename, '\'');

        if (ptr)
            *ptr = '\0';
    }

    return dlFilename;
}

/* LoadBuiltinLibrary : attempt to dynamically load a library of builtin functions from library `libraryName',
	returns true if a library was found.  If `reportErrors' is true then report the inability to find a .la
	file for the wanted library */
static bool LoadBuiltinLibrary (const char *libraryName, bool reportErrors)
{
    bool ret = false;
    GModule *handle = NULL;
    char *sharedLibraryFile = FindBuiltinLibSharedLibrary (libraryName, LibBreeze_GetIncludePathList (),
      reportErrors);
    char *libraryBaseName;

    /* From here on in the library is considered to only have a single component name:
       balsa.types.builtin -> builtin */
    {
        char *lastDot = strrchr (libraryName, '.');

        libraryBaseName = strdup ((lastDot ? lastDot + 1 : libraryName));
    }

    if (!g_module_supported ())
    {
        fprintf (stderr, "GModule error: %s\n", g_module_error ());
        g_free (libraryBaseName);
        return ret;
    }
    if (sharedLibraryFile)
        handle = g_module_open (sharedLibraryFile, G_MODULE_BIND_LAZY);

    if (!handle)
    {
        if (reportErrors)
        {
            fprintf (stderr, "Can't find and load module %s\n", sharedLibraryFile);
            if (sharedLibraryFile)
                fprintf (stderr, "GModule error2: %s\n", g_module_error ());
        }
    } else
    {
        /* Now try and find the init symbol, run that function and return the resulting struct */
        char *initFuncPrefix = "BalsaSim_BuiltinLibrary_";
        unsigned initFuncPrefixLength = strlen (initFuncPrefix);
        unsigned libraryBaseNameLength = strlen (libraryBaseName);
        char *initFuncName = malloc (initFuncPrefixLength + libraryBaseNameLength + 2);
        void (*initFunc) (void) = NULL;
        bool success;

        sprintf (initFuncName, "_%s%s", initFuncPrefix, libraryBaseName);
        success = g_module_symbol (handle, initFuncName + 1, (gpointer *) & initFunc);

        if (!initFunc || !success)
        {
            /* try BSD-style (?) _symbol */
            success = g_module_symbol (handle, initFuncName, (gpointer *) & initFunc);
            g_free (initFuncName);

            if (!initFunc || !success)
            {
                fprintf (stderr, "Can't find entry point for reader module for format `%s'\n", libraryName);
                g_free (libraryBaseName);
                return ret;
            }
        }
        g_free (initFuncName);

        if (initFunc)
        {
            initFunc ();
            ret = true;
        }
    }
    g_free (sharedLibraryFile);
    g_free (libraryBaseName);

    return ret;
}

/* LoadBuiltinLibrariesForImports : try and load builtin libraries for each of the given import lines */
static void LoadBuiltinLibrariesForImports ( /* list of struct BreezeImport * */ GList *
  imports)
{
    while (imports)
    {
        char *blockName = getBreezeImportName (imports->data);
        bool loadedLibrary;

        if (DebugFlag)
            fprintf (stderr, "Trying to find builtin library for block [%s]\n", blockName);

        loadedLibrary = LoadBuiltinLibrary (blockName, DebugFlag);

        if (DebugFlag)
            fprintf (stderr, "%s a builtin library\n", (loadedLibrary ? "Loaded" : "Didn't load"));

        imports = g_list_next (imports);
    }
}

/* BalsaSimulationTimeValue : required function to report simulation time to Balsa */
unsigned long long BalsaSimulationTimeValue (void)
{
    return current_real_time;
}

/* BalsaSimulationTime : required function to report simulation time to Balsa */
static void BalsaSimulationTime (BuiltinFunction * function, BuiltinFunctionInstanceData * instance)
{
    char *timeString = g_strdup_printf ("%llu", BalsaSimulationTimeValue ());
    BalsaString *ret = NewBalsaString (NULL, 0);

    ret->string = ret->allocatedString = timeString;
    ret->length = strlen (timeString);

    SetBalsaObject (instance->objects[0], ret, (BalsaDestructor) BalsaStringUnref);
    FormatDataSetBalsaObject (instance->result, instance->objects[0], 0);
}

/* BalsaSimulationStop : required function to stop simulation */
static void BalsaSimulationStop (BuiltinFunction * function, BuiltinFunctionInstanceData * instance)
{
	fprintf (stdout, "BalsaSimulationStop called\n");
	exit (EXIT_SUCCESS);
}

/* CommandLineArgs : name/value pair associations for command line args passed to the simulation */
static GHashTable *CommandLineArgs = NULL;

/* BalsaGetCommandLineArg : get the value of a name/value pair command line arg given the name */
static void BalsaGetCommandLineArg (BuiltinFunction * function, BuiltinFunctionInstanceData * instance)
{
    BalsaString *string = BALSA_STRING (FormatDataGetBalsaObject (instance->arguments[0], 0)->data);
    BalsaString *ret;
    char *name = g_strndup (string->string, string->length);
    char *value = g_hash_table_lookup (CommandLineArgs, name);

    g_free (name);

    ret = NewBalsaString ((value ? value : ""), -1);

    SetBalsaObject (instance->objects[0], ret, (BalsaDestructor) BalsaStringUnref);
    FormatDataSetBalsaObject (instance->result, instance->objects[0], 0);
}

/* InitBFile : play with the bfile functions so that the symbols here definately get exported by this binary */
static void InitBFile (void)
{
    int x = (int) BalsaFileReadable + (int) BalsaFileWritable;

    while (x)
        x = x >> 10;
    DeleteBalsaFile (GINT_TO_POINTER (x)); // x=0
}

/* BreezeSimSimulationLog : log function for WriteMessage */
#ifdef REORDERED_EVENT_DRIVEN_SCHEDULER
void BreezeSimSimulationLog_reordered_cb (struct comp *comp, int port)
{
    char *message = (char *) comp;

    printf ("%s\n", message);

    if (TraceFile)
    {
        fprintf (TraceFile, "\n$comment\nmessage-output \"");
        char *ptr;

        for (ptr = message; *ptr; ptr++)
        {
            if (*ptr == '"')
                fputc ('\\', TraceFile);
            fputc (*ptr, TraceFile);
        }
        fprintf (TraceFile, "\"\n$end\n");
    }
}
#endif
void BreezeSimSimulationLog (const char *message, int length)
{
    if (length == -1)
        length = strlen (message);

#ifdef REORDERED_EVENT_DRIVEN_SCHEDULER
    struct globalEventStruct *reordered_log_event = g_new0 (struct globalEventStruct, 1);
    struct callback_ *reordered_log_cb = g_new0 (struct callback_, 1);

    reordered_log_event->cb = reordered_log_cb;
    reordered_log_cb->fct = BreezeSimSimulationLog_reordered_cb;
    reordered_log_cb->comp = (struct comp *) g_strndup (message, length);
    reordered_log_cb->portnum = 0;

    InsertReorderedEventAtCurrentTime (reordered_log_event);

#else //#ifdef REORDERED_EVENT_DRIVEN_SCHEDULER
    /* fprintf (stdout, "LOG: "); */
    fwrite (message, length, 1, stdout);
    fprintf (stdout, "\n");

    if (TraceFile)
    {
        fprintf (TraceFile, "\n$comment\nmessage-output \"");
        int i;

        for (i = 0; i < length; i++)
        {
            char c = message[i];

            if (c == '"')
                fputc ('\\', TraceFile);
            fputc (c, TraceFile);
        }
        fprintf (TraceFile, "\"\n$end\n");

    }
#endif //#ifdef REORDERED_EVENT_DRIVEN_SCHEDULER
}

#ifdef MPI
int MPI_nb_msg_sent = 0;
int MPI_nb_msg_received = 0;
struct callback_ *WaitForIncomingMPIMessage (void)
{
    extern struct comp *MPI_comps[1000];

    struct MPI_message message;
    MPI_Status status, status2;

#ifdef REORDERED_MPI_EVENT_DRIVEN_SCHEDULER
    int flag = 0;

//3 printf ("%d probing for mpi message\n", my_mpi_rank);
    MPI_Iprobe (MPI_ANY_SOURCE, // receive from any sender
      MPI_ANY_TAG,              // any type of message
      MPI_COMM_WORLD,           // default communicator
      &flag, &status);          // info about the received message

    if (flag == 0)
    {
        // Reply to idle state query
        int *msg = malloc (2 * sizeof (int));

        msg[0] = MPI_nb_msg_sent;
        msg[1] = MPI_nb_msg_received;
//3  printf ("%d sending I_AM_IDLE[%d, %d] message\n", my_mpi_rank, msg[0], msg[1]);
        MPI_Send (msg, 2, MPI_INT, 2 /*dest */ , MPI_TAG__I_AM_IDLE, MPI_COMM_WORLD);
        MPI_nb_msg_sent = MPI_nb_msg_received = 0;
    }
#endif

#ifdef SAMIPS_EXPERIMENTS
    static int seq = 0;

    seq++;
    if (MPI_arg1 == 0)
        printf ("seq=%d\n", seq);
    else if (seq == MPI_arg1)
        CloseBreezeSimulation (0);
#endif

//3 printf ("%d waiting for mpi message\n", my_mpi_rank);
    MPI_Recv (&message,         // message buffer
      sizeof (struct MPI_message), MPI_BYTE, MPI_ANY_SOURCE, // receive from any sender
      MPI_ANY_TAG,              // any type of message
      MPI_COMM_WORLD,           // default communicator
      &status);                 // info about the received message

#ifdef REORDERED_MPI_EVENT_DRIVEN_SCHEDULER
    if (flag == 0)
    {
//3  printf ("%d sending I_AM_NOT_IDLE_ANYMORE message\n", my_mpi_rank);
        MPI_Send (NULL, 0, MPI_BYTE, 2 /*dest */ , MPI_TAG__I_AM_NOT_IDLE_ANYMORE, MPI_COMM_WORLD);
    }
#endif

    if (status.MPI_TAG == MPI_TAG__EVERYBODY_IS_IDLE)
    {
        return NULL;
    } else if (status.MPI_TAG == MPI_TAG__KILLALL)
    {
        printf ("%d: received KILLALL\n", my_mpi_rank);
        MPI_Send (NULL, 0, MPI_BYTE, 1 /*dest */ , MPI_TAG__KILLALL, MPI_COMM_WORLD);
//3  printf ("%d: Finalize\n", my_mpi_rank);
        MPI_Finalize ();
//3  printf ("%d: exit(0)\n", my_mpi_rank);
        fflush (0);
        exit (EXIT_SUCCESS);
    }
//3 printf ("%d received message (comp_num=%d, port=%d, event_type=%d, data_width=%d)\n", my_mpi_rank, message.MPI_comp_num, message.port, message.event_type, message.data_width);
    MPI_nb_msg_received++;
    {
        struct comp *c = MPI_comps[message.MPI_comp_num];

        if (message.data_width)
        {
            if (message.data_width <= (sizeof (int) * 8))
            {
                MPI_Recv (&c->chan[message.port]->data, sizeof (unsigned long), MPI_BYTE, status.MPI_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status2);
//3   printf ("received value=%d\n", (int)c->chan[message.port]->data);
            } else
            {
                char strtmp[10000];
                int len = 0;

                MPI_Recv (&len, 1, MPI_INT, status.MPI_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status2);
//3   printf ("received len=%d\n", len);
                MPI_Recv (strtmp, len, MPI_BYTE, status.MPI_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status2);
//3   printf ("received strtmp=%s\n", strtmp);
                mpz_ptr mp = (mpz_ptr) c->chan[message.port]->data;

                mpz_set_str (mp, strtmp, 16);
            }
        }

        current_real_time = message.timestamp;

        switch (message.event_type)
        {
        case REQUP:
            return &c->chan[message.port]->requp;
            break;
        case REQDOWN:
            return &c->chan[message.port]->reqdown;
            break;
        case ACKUP:
            return &c->chan[message.port]->ackup;
            break;
        case ACKDOWN:
            return &c->chan[message.port]->ackdown;
            break;
        }
    }

    return NULL;
}

void Synchroniser (void)
{
    int processes[2] = { 0, 0 };
    int nb_msg_sent[2] = { 0, 0 };
    int nb_msg_received[2] = { 0, 0 };

    int msg[2];
    MPI_Status status;

    while (1)
    {
//2  printf ("Synchroniser: waiting for message\n");
        MPI_Recv (&msg,         // message buffer
          2, MPI_INT, MPI_ANY_SOURCE, // receive from any sender
          MPI_ANY_TAG,          // any type of message
          MPI_COMM_WORLD,       // default communicator
          &status);             // info about the received message
//2  printf ("Synchroniser: received a message from %d\n", status.MPI_SOURCE);

        if (status.MPI_TAG == MPI_TAG__I_AM_IDLE)
        {
//2   printf ("Synchroniser: message from %d is I_AM_IDLE[%d,%d]\n", status.MPI_SOURCE, msg[0], msg[1]);
            int rank = status.MPI_SOURCE;

            if (rank < 0 || rank > 1)
                printf ("Synchroniser: Bad rank: %d\n", rank);
            if (processes[rank] == 1)
                printf ("Synchroniser: received twice I_AM_IDLE message from %d\n", rank);
            processes[rank] = 1;

            nb_msg_sent[rank] += msg[0];
            nb_msg_received[rank] += msg[1];
        } else if (status.MPI_TAG == MPI_TAG__I_AM_NOT_IDLE_ANYMORE)
        {
//2   printf ("Synchroniser: message from %d is I_AM_NOT_IDLE_ANYMORE\n", status.MPI_SOURCE);
            int rank = status.MPI_SOURCE;

            if (rank < 0 || rank > 1)
                printf ("Synchroniser: Bad rank: %d\n", rank);
            if (processes[rank] == 0)
                printf ("Synchroniser: received twice I_AM_NOT_IDLE_ANYMORE message from %d\n", rank);
            processes[rank] = 0;
        } else if (status.MPI_TAG == MPI_TAG__KILLALL)
        {
            printf ("Synchroniser: sending KILLALL\n");
            MPI_Send (NULL, 0, MPI_BYTE, 1 /*dest */ , MPI_TAG__KILLALL, MPI_COMM_WORLD);
//2   printf ("Synchroniser: Finalize\n");
            MPI_Finalize ();
//2   printf ("Synchroniser: exit(0)\n");
            exit (EXIT_SUCCESS);
        } else
        {
            printf ("Synchroniser: received BAD TAG %d\n", status.MPI_TAG);
        }

//2  printf ("Synchroniser: state= [%d, %d], process0:[%d, %d] process1:[%d, %d]\n", processes[0], processes[1], nb_msg_sent[0], nb_msg_received[0], nb_msg_sent[1], nb_msg_received[1]);

        if (processes[0] && processes[1] && nb_msg_sent[0] == nb_msg_received[1] && nb_msg_sent[1] == nb_msg_received[0])
        {
//2   printf ("%d sending EVERYBODY_IS_IDLE message\n", my_mpi_rank);
            MPI_Send (NULL, 0, MPI_BYTE, 0 /*dest */ , MPI_TAG__EVERYBODY_IS_IDLE, MPI_COMM_WORLD);
            nb_msg_sent[0] = nb_msg_received[0] = 0;
            nb_msg_sent[1] = nb_msg_received[1] = 0;
        }
    }
}
#endif // #ifdef MPI

const char *BreezeSimBanner = "\n"
  " |_ ._  _  _ __  _      _.._   [ breeze-sim: Breeze simulator ]\n" " |_)| `(-'(-' /_(-' - _/ ||||  (C) 2003, The University of Manchester\n";

int main (int argc, char **argv)
{
    int i;
    char *BreezeFileName = NULL;

#ifdef MPI
    // Initialize MPI
    MPI_Init (&argc, &argv);
    // Find out my identity in the default communicator
    MPI_Comm_rank (MPI_COMM_WORLD, &my_mpi_rank);
    if (my_mpi_rank == 2)
    {
        Synchroniser ();
        return 0;
    }
#ifdef SAMIPS_EXPERIMENTS
    ftime (&start_time);
    fprintf (stderr, "%d %d\n", start_time.time, start_time.millitm);
#endif
#endif // #ifdef MPI

    CommandLineArgs = g_hash_table_new (g_str_hash, g_str_equal);

    unsigned long long maxlong = ~((unsigned long long) 0);

    {                           /* Add the Balsa installation dir (or BALSAHOME/share) as the first path element */
        char *balsaHome = getenv ("BALSAHOME");
        char *shareDir;

        if (!balsaHome)
            balsaHome = BALSAHOME;

        shareDir = g_strconcat (balsaHome, "/share", NULL);
        LibBreeze_AddIncludePath (shareDir);
        g_free (shareDir);
    }

    /* Add the current directory as the secondpath element */
    char *currentDir = g_get_current_dir ();

    LibBreeze_AddIncludePath (currentDir);
    g_free (currentDir);

    InitBFile ();

    setvbuf (stdout, NULL, _IOLBF, 0);

    if (argc < 2)
    {
        printf ("%s\n", BreezeSimBanner);
        printf ("Usage: %s [<options>] file\n"
          " Options:\n"
          "  -I (--include) <dir>\n"
          "  --debug\n"
          "  --tracefile=<filename>\n"
          "  --traceallchans / --tracenamedportsonly\n"
          "  --vcdtracenamedportsonly\n"
	  "  -k (--keep-channel-numbers)\n" "  --link=<filename>\n" "  --stop-at-time=<integer>"
          "  --no-delay-coefs\n", *argv);
        exit (EXIT_FAILURE);
    }

    for (i = 1; i < argc; i++)
    {
	printf("arg:%s\n",argv[i]);
        if (BEGINS_WITH (argv[i], "-I") || BEGINS_WITH (argv[i], "--include"))
        {
            if (!argv[i + 1])
            {
                fprintf (stderr, "Usage: -I <dir>\n");
                exit (EXIT_FAILURE);
            }

            LibBreeze_AddIncludePath (argv[i + 1]);
            i++;
        } else if (BEGINS_WITH (argv[i], "-D") || BEGINS_WITH (argv[i], "--define"))
        {
            if (!argv[i + 1] || !argv[i + 2])
            {
                fprintf (stderr, "Usage: -D <name> <value>\n");
                exit (EXIT_FAILURE);
            }

            g_hash_table_insert (CommandLineArgs, argv[i + 1], argv[i + 2]);
            i += 2;
        } else if (BEGINS_WITH (argv[i], "--debug"))
        {
            DebugFlag = 1;
            BalsaDebug = 1;
        } else if (BEGINS_WITH (argv[i], "--tracefile="))
        {
#ifdef MPI
            if (my_mpi_rank == 0)
                TraceFileName = g_strdup (argv[i] + strlen ("--tracefile="));
            else
                TraceFileName = g_strdup_printf ("%s%d", argv[i] + strlen ("--tracefile="), my_mpi_rank);
#else
            TraceFileName = g_strdup (argv[i] + strlen ("--tracefile="));
#endif // #ifdef MPI
            TraceFile = fopen (TraceFileName, "w");
        } else if (BEGINS_WITH (argv[i], "--traceallchans"))
        {
            TraceAllChans = TRUE;
        } else if (BEGINS_WITH (argv[i], "--tracenamedportsonly"))
        {
            TraceNamedPortsOnly = TRUE;
        } else if (BEGINS_WITH (argv[i], "--vcdtracenamedportsonly"))
        {
            VcdTraceNamedPortsOnly = TRUE;
        } else if (BEGINS_WITH (argv[i], "--flush="))
        {
            flush_delay = atoi (argv[i] + strlen ("--flush="));
        } else if (BEGINS_WITH (argv[i], "--gtkwave"))
        {
            ForkGTKWave = TRUE;
        } else if (BEGINS_WITH (argv[i], "--ctrl-pipe"))
        {
            CtrlPipe = TRUE;
        } else if (BEGINS_WITH (argv[i], "-k") || BEGINS_WITH (argv[i], "--keep-channel-numbers"))
        {
            extern gboolean save_channel_numbers; // in libbreeze

            save_channel_numbers = TRUE;
        } else if (BEGINS_WITH (argv[i], "--link="))
        {
            ActAsLinker = TRUE;
            LinkFileName = g_strdup (argv[i] + strlen ("--link="));
        } else if (BEGINS_WITH (argv[i], "--stop-at-time="))
        {
            StopAtTime = atoi (argv[i] + strlen ("--stop-at-time="));
        } else if (BEGINS_WITH (argv[i], "--no-delay-coefs"))
        {
            NoDelayCoefs = TRUE;
        } else if (BEGINS_WITH (argv[i], "--version"))
        {
            fprintf (stderr, "breeze-sim CVS version: $Id: main.c,v 1.132 2004/10/14 22:18:06 janinl Exp $\n");
            exit (EXIT_SUCCESS);
#ifdef MPI
        } else if (BEGINS_WITH (argv[i], "--MPI-arg1="))
        {
            MPI_arg1 = atoi (argv[i] + strlen ("--MPI_arg1="));
#endif
        } else if (BEGINS_WITH (argv[i], "--hla"))
        {
            HLA_activated = TRUE;
            extern void LoadHLALibrary (void);
            LoadHLALibrary ();
        } else
        {
            BreezeFileName = argv[i];
        }
    }

    if (ENDS_WITH (BreezeFileName, ".breeze"))
        BreezeFileName[strlen (BreezeFileName) - strlen (".breeze")] = 0;

    if (ForkGTKWave)
    {
        int pid = fork ();

        if (pid == 0)
        {                       // child
            char *command = g_strdup_printf ("gtkwave %s", TraceFileName);

            system (command);
            exit (0);
        } else
            sleep (2);
    }

    if (flush_delay)
    {
        signal (SIGALRM, (void *) signal_ALARM);
        alarm (flush_delay);
    }

    signal (SIGTERM, (void *) signal_TERM);

    /***************************/

    struct BreezeFile *breezeStruct = breezeInitParse (BreezeFileName);

    if (breezeStruct == NULL)
    {
        fprintf (stderr, "Error (opening or parsing) in file %s.breeze\n", BreezeFileName);
        exit (EXIT_FAILURE);
    }

    GList *breezeParts = getBreezeParts (breezeStruct);

    if (breezeParts == NULL)
    {
        fprintf (stderr, "Error: No part defined in %s.breeze\n", BreezeFileName);
        exit (EXIT_FAILURE);
    } else
        breezePart = breezeParts->data;

    LibBreeze_FlattenBreezePart (breezePart);

    if (ActAsLinker)
    {
        FILE *f = fopen (LinkFileName, "w");

        dumpBreezeFile (f, breezeStruct);
        fclose (f);
        exit (EXIT_SUCCESS);
    }

    /* Register the mandatory builtins */
    BalsaSim_SimulationLog = BreezeSimSimulationLog;
    BalsaSim_RegisterBuiltinFunction ("BalsaSimulationTime", 0, 0, BalsaSimulationTime, BALSA_OBJECT_BITS, NULL, 1);
    BalsaSim_RegisterBuiltinFunction ("BalsaSimulationStop", 0, 0, BalsaSimulationStop, 1, NULL, 0);
    BalsaSim_RegisterBuiltinFunction ("BalsaGetCommandLineArg", 0, 1, BalsaGetCommandLineArg, BALSA_OBJECT_BITS, (unsigned[])
      {
      BALSA_OBJECT_BITS}
      , 1);

    /* Register all the builtin functions */
    LoadBuiltinLibrariesForImports (getBreezeImports (breezeStruct));

    BalsaSim_PrintF = printf;

    /***************************/

	{
		BalsaType *string = NewBalsaType (BalsaBuiltinType, "String", 64);
		BalsaInternType (string);
	}
    InitialiseBreezeTypes (getBreezeTypes (getBreezePartFile (breezePart)));
    InitialiseBreezeChannels (breezePart);
    InitialiseBreezeComponents (breezePart);

    InitialiseComponentDelayCoefs (breezePart);

    WriteAnimationInfo_time (0);

    InitialisePorts (breezePart);

    channel[1].ackdown.fct = BreezeTermination;

    /************ Ready to start ***************/
    if (CtrlPipe)
    {
        CtrlPipe_run = FALSE;
        fcntl (3, F_SETFL, fcntl (3, F_GETFL) & ~O_NONBLOCK);
        CtrlPipe_rununtil = -1;
        CtrlPipe_speed = 100000;
    }
#if defined(EVENT_DRIVEN_SCHEDULER) || defined(REORDERED_EVENT_DRIVEN_SCHEDULER)
    Trace_Initialise (BreezeFileName);
    while (1)
#else
    while (current_real_time - last_event_time <= time_span || HLA_activated)
#endif
    {
        /* Control pipe handling */
        if (CtrlPipe)
        {
            char command[10000];
            int pos = 0;
            char *nextcommand;
            int size;

            do
            {
                size = read (3, command + pos, 10000 - pos);
                if (CtrlPipe_run && size <= 0)
                {
                    if (CtrlPipe_speed == 0)
                    {
                        sleep (1);
                        continue;
                    } else
                        break;
                }
                if (size != -1)
                    command[pos + size] = 0;

              secondpass:
                nextcommand = strchr (command, '\n');
                if (!nextcommand)
                    continue;

                //  printf ("received %d:%s.\n",size,command);

                if (BEGINS_WITH (command, "setspeed "))
                {
                    CtrlPipe_speed = atoi (command + 9);
                } else if (BEGINS_WITH (command, "run "))
                {
                    CtrlPipe_rununtil = atoi (command + 4);

                    CtrlPipe_run = TRUE;
                    fcntl (3, F_SETFL, fcntl (3, F_GETFL) | O_NONBLOCK);
                } else if (BEGINS_WITH (command, "pause"))
                {
                    CtrlPipe_run = FALSE;
                    fcntl (3, F_SETFL, fcntl (3, F_GETFL) & ~O_NONBLOCK);
                } else if (BEGINS_WITH (command, "resume"))
                {
                    CtrlPipe_run = TRUE;
                    fcntl (3, F_SETFL, fcntl (3, F_GETFL) | O_NONBLOCK);
                } else if (BEGINS_WITH (command, "stop"))
                {
                    exit (EXIT_SUCCESS);
                }

                if (nextcommand[1])
                {
                    pos = strlen (nextcommand + 1);
                    memmove (command, nextcommand + 1, pos + 1);
                    goto secondpass;
                } else
                    pos = 0;
                //  printf("here %d\n",CtrlPipe_run);
            }
            while (CtrlPipe_run == FALSE || CtrlPipe_speed <= 0);

            if (CtrlPipe_speed < 100000)
            {
                fflush (NULL);
                /*
                   struct timespec req;
                   struct timespec rem;
                   req.tv_sec = 0;
                   req.tv_nsec = 999999999/CtrlPipe_speed;
                   nanosleep (&req, &rem);
                 */
                usleep (9999 / CtrlPipe_speed);
            }
        }

        /* End of Control pipe handling */

#if defined(EVENT_DRIVEN_SCHEDULER) || defined(REORDERED_EVENT_DRIVEN_SCHEDULER)
        struct callback_ *cb = 0;

#ifdef REORDERED_EVENT_DRIVEN_SCHEDULER
        // Exceptional situation where it is necessary to break the normal simulation flow:
        // If the sequential execution gets too long: the simulator is livelocking in a loop without executing parallel events
        // In this case, we fall back to an exhaustive search of the smallest timestamped event
#define TOO_LONG 1000
        if (sequential_execution_duration++ > TOO_LONG)
        {
#undef TOO_LONG
            sequential_execution_duration = 0;

            if (globalNextEvent_cb)
            {                   // Push the currently scheduled event in the queue, to simplify subsequent operations
                current_real_time = globalNextEvent_timestamp;
                InsertEventAtLaterTime (globalNextEvent_cb, 0, 0);
                globalNextEvent_cb = 0;
            }

            int i;
            int min_index = -1;
            long long min_time = maxlong;

            for (i = 0; i < globalEventArray_pos; i++)
            {
                if (globalEventArray[i].timestamp < min_time)
                {
                    min_time = globalEventArray[i].timestamp;
                    min_index = i;
                }
            }

            if (reordered_events_list && min_time > ((struct globalEventStruct *) reordered_events_list->data)->timestamp)
                goto break_flow_for_handling_reordered_events;
            else
            {
                globalEventArray_pos--;
                globalNextEvent_cb = globalEventArray[min_index].cb;
                globalNextEvent_timestamp = globalEventArray[min_index].timestamp;
                bcopy (&globalEventArray[min_index + 1], &globalEventArray[min_index],
                  sizeof (struct globalEventStruct) * (globalEventArray_pos - min_index));
                Trace_CommitUntilTime (globalNextEvent_timestamp);
            }
//    goto break_flow_for_handling_another_thread_in_normal_queue;
        }
#endif

        if (globalNextEvent_cb)
        {
            cb = globalNextEvent_cb;
            current_real_time = globalNextEvent_timestamp;
            globalNextEvent_cb = 0;
        } else if (globalEventArray_pos)
        {
            globalEventArray_pos--;
            cb = globalEventArray[globalEventArray_pos].cb;
            current_real_time = globalEventArray[globalEventArray_pos].timestamp;

            if (TraceFile)
            {
                //    fprintf (TraceFile, "---New Thread---\n");
                Trace_StartNewThread ();
            }
        }
#ifdef MPI
        // if no event and mpi is running, then need to check for incoming messages
        else if ((cb = WaitForIncomingMPIMessage ()) != NULL)
        {
        }
#endif // #ifdef MPI

#ifdef REORDERED_EVENT_DRIVEN_SCHEDULER
        else if (reordered_events_list)
        {
            struct globalEventStruct *event;

          break_flow_for_handling_reordered_events:
            sequential_execution_duration = 0;

            event = reordered_events_list->data;

            cb = event->cb;
            current_real_time = event->timestamp;
            reordered_events_list = g_list_remove_link (reordered_events_list, reordered_events_list);
            reordered_events_list_length--;
            if (TraceFile)
            {
                //    fprintf (TraceFile, "Careful!\n");
                fflush (TraceFile);
                Trace_StartNewThread ();
                Trace_CommitUntilTime (current_real_time);
            }
        }
#endif

        if (!cb)
            break;

        last_event_time = MAX (last_event_time, current_real_time);
        WriteAnimationInfo_signalCallback_OutOfOrder (cb, current_real_time);
#ifdef SAMIPS_EXPERIMENTS
        do_something ();
#endif // #ifdef SAMIPS_EXPERIMENTS
        cb->fct (cb->comp, cb->portnum);

#else //#if defined(EVENT_DRIVEN_SCHEDULER) || defined(REORDERED_EVENT_DRIVEN_SCHEDULER)

        int slot = current_real_time % time_span;

        //if (current_real_time % 100000 == 0) printf("!! %20lld %20lld %8d\n", current_real_time, last_event_time, time_span);
        if (signal_stack[slot].pos)
        {
            last_event_time = current_real_time;
            //            WriteAnimationInfo_time (current_real_time);
        }

      more_callbacks:
        while (signal_stack[slot].pos)
        {
            struct callback_ *cb;

            signal_stack[slot].pos--;
            cb = signal_stack[slot].cb[signal_stack[slot].pos];
            WriteAnimationInfo_signalCallback (cb);
#ifdef DELAYS_WITH_ERROR
            current_error = cb->error;
#endif
            cb->fct (cb->comp, cb->portnum);

            slot = current_real_time % time_span; // time_span could have been changed when cb is inserting events
        }

        if (HLA_activated) {
            extern int (*my_cosim__sync_callback) (int);
            if (!my_cosim__sync_callback (3))
                continue;

            slot = current_real_time % time_span; // time_span could have been changed when cb is inserting events
            if (signal_stack[slot].pos)
                goto more_callbacks;
        }

        current_real_time++;
#endif //#if defined(EVENT_DRIVEN_SCHEDULER) || defined(REORDERED_EVENT_DRIVEN_SCHEDULER)

        if (current_real_time >= maxlong)
        {
            fprintf (stderr, "Time overflow\n");
            CloseBreezeSimulation (1);
        }
    }

    CloseBreezeSimulation (0);

    return 0;
}
