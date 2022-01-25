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

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gtk/gtk.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <dirent.h>

#include "support.h"
#include "interface.h"
#include "support.h"
#include "main.h"
#include "mainwindow2.h"
#include "simulation.h"
#include "sourceview.h"

extern gboolean save_channel_numbers; // in libbreeze

char *projectName = NULL;
char *projectNameWithPath = NULL;
char *pathName;
const char *layoutFilename = NULL;
char *scriptFilename = NULL;
char *partName = NULL;

GtkWidget *MainWindow1, *MainWindow2, *SourceViewWindow;

char *pidFilename = NULL;
int previousPid = 0;

enum TraceType traceType = TraceAllChans; //TraceNamedPortsOnly
int signal_SIGCHLD_ignoreNext = 0;

void main_finish_initialisation (void);

void signal_SIGCHLD (int a)
{
    extern int breezesimPid, gtkwavePid;
    int status;

    //    fprintf(stderr,"SIGCHLD %d\n",a);

    if (signal_SIGCHLD_ignoreNext)
    {
        signal_SIGCHLD_ignoreNext--;
        // printf ("ignored\n");
        return;
    }
    //    int pid = wait (&status);
    int pid = waitpid (0, &status, WNOHANG);

    if (!pid)
        return;

    //    printf("child died %d (%d %d)\n", pid, breezesimPid, gtkwavePid);

    if (WIFEXITED (status) || WIFSIGNALED (status))
    {
        if (breezesimPid == pid)
        {
            GrayControlIcons (TRUE, FALSE, TRUE);
            simulationIsRunning = FALSE;
            UpdateLEDs ();
            breezesimPid = 0;
        } else if (gtkwavePid == pid)
        {
            gtkwaveIsRunning = FALSE;
            UpdateLEDs ();
            gtkwavePid = 0;
        }

        if (WIFSIGNALED (status) && (pid == breezesimPid || pid == gtkwavePid))
        {
            GtkWidget *msg = create_InfoProcessCrashed ();

            gtk_widget_show (msg);
        }
    }
}

void signal_SIGUSR1 (int a)
{
    //    fprintf(stderr,"received SIGUSR1\n");
    if (MainWindow2)
    {
        gtk_widget_show (MainWindow2);
        gdk_window_raise (MainWindow2->window);
    }
}

void signal_SIGQUIT (int a)
{
//    fprintf(stderr,"received SIGQUIT\n");
    gtk_main_quit ();
}

int CheckForSameSimulationRunning (void)
{
    const char *homedir = g_get_home_dir ();
    char dirname[] = ".breeze-sim-ctrl";

    if (homedir)
        pathName = g_strdup_printf ("%s/%s", homedir, dirname);
    else
        pathName = g_strdup (dirname);

    // Search for a running simulation with the same name
    DIR *d = opendir (pathName);

    if (d)
    {
        struct dirent *ent;
        int length = strlen (projectNameWithPath);
        char *buf = (char *) malloc (length + 1);

        while ((ent = readdir (d)) != 0)
        {
            if (ent->d_name && ent->d_name[0] && ent->d_name[0] >= '0' && ent->d_name[0] <= '9')
            {
                char *fullName = g_strdup_printf ("%s/%s", pathName, ent->d_name);
                FILE *f = fopen (fullName, "r");

                if (!f)
                    continue;
                int l = fread (buf, 1, length + 1, f);

                if (l == length && !strncmp (buf, projectNameWithPath, length))
                {
                    previousPid = atoi (ent->d_name);

                    if (kill (previousPid, 0 /*SIGUSR1 */ ) == -1)
                    {
                        fprintf (stderr, "Incorrect pid lock detected and removed (%s).\n", fullName);
                        remove (fullName);
                        free (fullName);
                        continue;
                    }
                    free (buf);
                    return -1;
                }
                free (fullName);
            }
        }
        free (buf);
        closedir (d);
    } else
        mkdir (pathName, S_IRWXU);

    return 0;
}

void TryParsingScript (char *filename, int pass)
{
    FILE *f = fopen (filename, "r");

    if (!f)
        return;

    char buf[100];

    fgets (buf, 100, f);
    if (BEGINS_WITH (buf, "-- breeze-sim-ctrl script"))
    {
        if (pass == 1)
            scriptFilename = filename;

        while (!feof (f) && fgets (buf, 100, f))
        {
            char *ptr = strchr (buf, '\n');

            if (ptr)
                *ptr = 0;
            char *arg2 = strchr (buf, ' ') + 1;

            if (BEGINS_WITH (buf, "use-breeze-file "))
            {
                if (pass == 1)
                    projectName = g_strdup (arg2);
            } else if (BEGINS_WITH (buf, "use-layout-file "))
            {
                if (pass == 1)
                    layoutFilename = g_strdup (arg2);
            } else if (BEGINS_WITH (buf, "resize-window "))
            {
                if (pass == 2)
                {
                    int width, height;

                    width = atoi (arg2);
                    arg2 = strchr (arg2, ' ') + 1;
                    height = atoi (arg2);
                    gtk_window_set_default_size (GTK_WINDOW (MainWindow2), width, height);
                }
            } else if (BEGINS_WITH (buf, "open-graph-view"))
            {
                if (pass == 2)
                {
                    GtkToggleButton *graphToggleButton = GTK_TOGGLE_BUTTON (gtk_object_get_data (GTK_OBJECT (MainWindow2),
                        "GraphToggleButton"));

                    gtk_toggle_button_set_active (graphToggleButton, TRUE);
                }
            } else
            {
                if (pass == 1)
                    fprintf (stderr, "Undefined command in breeze-sim-ctrl script: %s\n", buf);
            }
        }
    }

    fclose (f);
}

int main (int argc, char *argv[])
{
//  gtk_set_locale ();
    gtk_init (&argc, &argv);

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

    setvbuf (stdout, NULL, _IOLBF, 0);

    if (argc < 2)
    {
        printf ("Usage: %s [<options>] file\n"
          " All options are parsed, used if recognised, and passed to breeze-sim.\n"
          " Recognised options:\n"
          "  -I (--include) <dir>\n" "  -k (--keep-channel-numbers)\n" "  -l (--layout) <filename>\n" "  -p (--part) <part name>\n", *argv);
        exit (EXIT_FAILURE);
    }

    int i;

    for (i = 1; i < argc - 1; i++)
    {
        if (BEGINS_WITH (argv[i], "-I") || BEGINS_WITH (argv[i], "--include"))
        {
            if (!argv[i + 1])
            {
                fprintf (stderr, "Usage: -I <dir>\n");
                exit (EXIT_FAILURE);
            }

            LibBreeze_AddIncludePath (argv[i + 1]);
            i++;
        } else if (BEGINS_WITH (argv[i], "-k") || BEGINS_WITH (argv[i], "--keep-channel-numbers"))
        {
            save_channel_numbers = TRUE;
        } else if (BEGINS_WITH (argv[i], "-l") || BEGINS_WITH (argv[i], "--layout"))
        {
            if (!argv[i + 1])
            {
                fprintf (stderr, "Usage: -l (--layout) <filename>\n");
                exit (EXIT_FAILURE);
            }

            layoutFilename = g_strdup (argv[i + 1]);
            i++;
        } else if (BEGINS_WITH (argv[i], "-p") || BEGINS_WITH (argv[i], "--part"))
        {
            if (!argv[i + 1])
            {
                fprintf (stderr, "Usage: -p (--part) <part name>\n");
                exit (EXIT_FAILURE);
            }

            partName = g_strdup (argv[i + 1]);
            i++;
        }
    }

    projectName = g_strdup (argv[argc - 1]);

    TryParsingScript (projectName, 1);

    if (ENDS_WITH (projectName, ".breeze"))
        projectName[strlen (projectName) - strlen (".breeze")] = 0;

    projectNameWithPath = g_strdup_printf ("%s/%s", currentDir, projectName);
    g_free (currentDir);

    breezesim_passedArgs = g_new0 (char *, argc - 2);
    memcpy (breezesim_passedArgs, argv + 1, (argc - 2) * sizeof (char *));
    breezesim_passedArgCount = argc - 2;

    if (CheckForSameSimulationRunning ())
    {
        GtkWidget *msg = create_BreezeSimCtrlAlreadyRunning ();

        gtk_widget_show (msg);
        pidFilename = 0;
    } else
        main_finish_initialisation ();

    gtk_main ();

    if (pidFilename)
        remove (pidFilename);

    return 0;
}

void main_finish_initialisation (void)
{
    int pid = getpid ();

    pidFilename = g_strdup_printf ("%s/%d", pathName, pid);

    FILE *f = fopen (pidFilename, "w");

    fprintf (f, "%s", projectNameWithPath);
    fclose (f);

    {                           /* Handle pixmap startup */
        GtkWidget *sample_window = gtk_window_new (GTK_WINDOW_TOPLEVEL);

        breezesimctrl_register_pixmaps (sample_window);
        gtk_widget_destroy (sample_window);
    }

    MainWindow2 = create_MainWindow2 ();
    gtk_widget_hide (GTK_WIDGET (gtk_object_get_data (GTK_OBJECT (MainWindow2), "PauseButton")));
    gtk_widget_show (MainWindow2);

    SourceViewWindow = create_SourceViewWindow ();
    InitSourceViewWindow ();

    InitMainWindow2 ();

    {                           // set Window's title
        char *name = g_strdup_printf ("Breeze-sim-ctrl <%s>", projectName);

        gtk_window_set_title (GTK_WINDOW (MainWindow2), name);
    }

    if (scriptFilename)
        TryParsingScript (scriptFilename, 2);

    signal (SIGCHLD, (void *) signal_SIGCHLD);
    signal (SIGUSR1, (void *) signal_SIGUSR1);
    signal (SIGQUIT, (void *) signal_SIGQUIT);

    signal_SIGCHLD_ignoreNext = 0;

    //  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (gtk_object_get_data (GTK_OBJECT (MainWindow2), "SourceViewToggleButton")), TRUE);
}

void TwoInstances_UsePrevious (GtkButton * button, gpointer user_data)
{
    kill (previousPid, SIGUSR1);
    exit (EXIT_SUCCESS);
}

void TwoInstances_StartNew (GtkButton * button, gpointer user_data)
{
    GtkWidget *msg = gtk_widget_get_toplevel (GTK_WIDGET (button));

    gtk_widget_destroy (msg);
    main_finish_initialisation ();
}

void TwoInstances_KillPreviousAndRestart (GtkButton * button, gpointer user_data)
{
    kill (previousPid, SIGQUIT);

    GtkWidget *msg = gtk_widget_get_toplevel (GTK_WIDGET (button));

    gtk_widget_destroy (msg);
    main_finish_initialisation ();
}
