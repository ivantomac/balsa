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
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "interface.h"
#include "main.h"
#include "mainwindow2.h"
#include "drawing.h"
#include "simtrace.h"

int breezesimPid = 0;
int gtkwavePid = 0;

int gtkwave_pipe1[2];
int gtkwave_pipe2[2];

char **breezesim_passedArgs = NULL;
int breezesim_passedArgCount = 0;

int breezesim_pipe[2];
gboolean simulationIsRunning = FALSE;
gboolean simulationIsPaused = FALSE;
gboolean gtkwaveIsRunning = FALSE;

void Simulation_Pause (void);
void Simulation_Play (void);

void Simulation_SimLengthChanged (int length)
{
}

void GrayControlIcons (gboolean play, gboolean stop, gboolean toggletraces)
{
    if (play)
    {
        gtk_widget_show (GTK_WIDGET (gtk_object_get_data (GTK_OBJECT (MainWindow2), "PlayButton")));
        gtk_widget_hide (GTK_WIDGET (gtk_object_get_data (GTK_OBJECT (MainWindow2), "PauseButton")));
    } else
    {
        gtk_widget_hide (GTK_WIDGET (gtk_object_get_data (GTK_OBJECT (MainWindow2), "PlayButton")));
        gtk_widget_show (GTK_WIDGET (gtk_object_get_data (GTK_OBJECT (MainWindow2), "PauseButton")));
    }
    gtk_widget_set_sensitive (GTK_WIDGET (gtk_object_get_data (GTK_OBJECT (MainWindow2), "StopButton")), stop);
    gtk_widget_set_sensitive (GTK_WIDGET (gtk_object_get_data (GTK_OBJECT (MainWindow2), "SimulationMenu_Run")), play);
    gtk_widget_set_sensitive (GTK_WIDGET (gtk_object_get_data (GTK_OBJECT (MainWindow2), "SimulationMenu_Stop")), stop);
    gtk_widget_set_sensitive (GTK_WIDGET (gtk_object_get_data (GTK_OBJECT (MainWindow2), "SimulationMenu_Pause")), !play);
    gtk_widget_set_sensitive (GTK_WIDGET (gtk_object_get_data (GTK_OBJECT (MainWindow2), "toggleTraceAllChannels")), toggletraces);
    gtk_widget_set_sensitive (GTK_WIDGET (gtk_object_get_data (GTK_OBJECT (MainWindow2), "toggleTracePortsOnly")), toggletraces);
    gtk_widget_set_sensitive (GTK_WIDGET (gtk_object_get_data (GTK_OBJECT (MainWindow2), "toggleTraceNamedPortsOnly")), toggletraces);
    gtk_widget_set_sensitive (GTK_WIDGET (gtk_object_get_data (GTK_OBJECT (MainWindow2), "menuTraceAllChannels")), toggletraces);
    gtk_widget_set_sensitive (GTK_WIDGET (gtk_object_get_data (GTK_OBJECT (MainWindow2), "menuTracePortsOnly")), toggletraces);
}

void UpdateLEDs (void)
{
    GtkWidget *led1 = GTK_WIDGET (gtk_object_get_data (GTK_OBJECT (MainWindow2), "led1"));

    gtk_widget_queue_draw (led1);

    {
        GtkWidget *toggle = (gtk_object_get_data (GTK_OBJECT (MainWindow2), "GTKWaveToggleButton"));

        gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (toggle), gtkwaveIsRunning);
    }
}

void SendSetspeedCommand (gboolean force)
{                               // set speed
    static gfloat oldfvalue = -1;
    GtkWidget *scale = gtk_object_get_data (GTK_OBJECT (MainWindow2), "SimulationSpeedScale");

    //AB gfloat fvalue = GTK_RANGE (scale)->old_value;
    gfloat fvalue = gtk_range_get_value (GTK_RANGE (scale));

    if ((simulationIsRunning && oldfvalue != fvalue) || force)
    {
        oldfvalue = fvalue;
        int value = (int) pow (10, fvalue);
        char *command = g_strdup_printf ("setspeed %d\n", value);

        // printf("%s\n",command);
        write (breezesim_pipe[1], command, strlen (command));
        g_free (command);
    }
}

/*
void gtkwave_input_received (gpointer data, int source, GdkInputCondition condition)
{
  char buf[10000];
  int size = read (source, buf, 10000);

  buf[size] = 0;
  printf ("received:%s.\n",buf);
}
*/
void gtkwave_send_command (char *command)
{
    return; // balsa-wave desactivated

    //  printf ("sent:%s.\n",command);
    write (gtkwave_pipe1[1], command, strlen (command));
    //  gdk_input_add (gtkwave_pipe2[0], GDK_INPUT_READ, gtkwave_input_received, NULL);

    char buf[10000];
    int size = read (gtkwave_pipe2[0], buf, 10000);

    buf[size] = 0;
    //  printf ("received:%s.\n",buf);
}

void gtkwave_send_initchannel_commands (void)
{
    return; // balsa-wave desactivated

    enum GTKWaveViewType viewType = getGTKWaveViewType ();

    if (viewType == GTKWaveViewNone)
        return;

    char *hhhfilename = g_strdup_printf ("%s.vcd", projectName);
    FILE *f = fopen (hhhfilename, "r");
    char buf[10000], *ptr;

    while (1)
    {
        ptr = fgets (buf, 10000, f);
        if (!ptr || feof (f))
            break;
        if (BEGINS_WITH (buf, "init-channel "))
        {
            int channum = atoi (buf + strlen ("init-channel "));

            if (viewType != GTKWaveViewAllTracedChans)
            {
                ptr = strrchr (buf, ' ');
                if (!ptr || strcmp (ptr, " isPort\n") != 0)
                    continue;

                if (viewType == GTKWaveViewNamedPortsOnly)
                {
                    char *name = strchr (buf, '\"');

                    if (!name)
                        continue;
                    ptr = strchr (name + 1, '\"');
                    if (!ptr || *(ptr - 1) == '.')
                        continue;
                }

            }
            char *command = g_strdup_printf ("add-trace-with-key %d\n", channum);

            gtkwave_send_command (command);
            g_free (command);

        } else if (BEGINS_WITH (buf, "time 0"))
            break;
    }

    gtkwave_send_command ("zoom fit\n");
    gtkwave_send_command ("redraw\n");
}

void Launch_GTKWave (int delay)
{
    if (simulationIsRunning)
        Simulation_Pause ();

    if (gtkwaveIsRunning && gtkwavePid)
    {
        GtkWidget *GTKWaveRunningDialogue = create_GTKWaveRunningDialogue ();

        gtk_widget_show (GTKWaveRunningDialogue);
        return;
    }

    char *hhhfilename = g_strdup_printf ("%s.vcd", projectName);

    if (access (hhhfilename, R_OK) != 0)
    {
        GtkWidget *NoTraceFileDialogue = create_NoTraceFileDialogue ();

        gtk_widget_show (NoTraceFileDialogue);
        return;
    }

    pipe (gtkwave_pipe1);
    pipe (gtkwave_pipe2);

    gtkwavePid = fork ();
    if (gtkwavePid == 0)
    {
        signal (SIGCHLD, NULL);

        dup2 (gtkwave_pipe1[0], 3);
        dup2 (gtkwave_pipe2[1], 4);
        close (gtkwave_pipe1[1]);
        close (gtkwave_pipe2[0]);

        if (delay)
            sleep (delay);

        execlp ("gtkwave", "gtkwave", hhhfilename, NULL);

        // If we reach here, then it means that gtkwave is not installed
        fprintf (stderr, "Error: GTKWave is not installed\n");
        sleep (1);
        _exit (0);
    }

    //    close (gtkwave_pipe1[0]);
    //    close (gtkwave_pipe2[1]);

    gtkwave_send_command ("hello\n");
    gtkwave_send_initchannel_commands ();

    gtkwaveIsRunning = TRUE;
    UpdateLEDs ();

    if (simulationIsRunning)
        Simulation_Play ();
}

void KillGTKWave (void)
{
    //    printf("killing %d (not %d)\n", gtkwavePid, breezesimPid);
    kill (gtkwavePid, SIGKILL);
    gtkwaveIsRunning = FALSE;
    gtkwavePid = 0;
}

void KillAndRestartGTKWave (GtkButton * button, gpointer user_data)
{
    GtkWidget *dialogue = gtk_widget_get_toplevel (GTK_WIDGET (button));

    gtk_widget_hide (dialogue);

    KillGTKWave ();
    Launch_GTKWave (0);
}

void StartNewGTKWave (GtkButton * button, gpointer user_data)
{
    GtkWidget *dialogue = gtk_widget_get_toplevel (GTK_WIDGET (button));

    gtk_widget_hide (dialogue);

    gtkwaveIsRunning = FALSE;
    gtkwavePid = 0;
    Launch_GTKWave (0);
}

void Simulation_Run (void)
{
    GrayControlIcons (FALSE, TRUE, FALSE);
    pipe (breezesim_pipe);

    breezesimPid = fork ();
    if (breezesimPid == 0)
    {
        signal (SIGCHLD, NULL);

        close (3);
        dup2 (breezesim_pipe[0], 3);

        // char *command = g_strdup_printf ("breeze-sim --ctrl-pipe %s%s--tracefile=%s.hhh %s", (traceType==TraceAllChans)?"--traceallchans ":"", (traceType==TraceNamedPortsOnly)?"--tracenamedportsonly ":"", projectName, projectName);
        //printf ("Launching %s\n", command);
        char *tracefilearg = g_strdup_printf ("--tracefile=%s.vcd", projectName);

#define BREEZESIM_MAX_ARGS 10   /* This doesn't include the arguments passed from the command-line of breeze-sim-ctrl to breeze-sim */

        char **argv = g_new0 (char *, breezesim_passedArgCount + BREEZESIM_MAX_ARGS);
        int argc = 0;

        argv[argc++] = "breeze-sim";
        argv[argc++] = "--ctrl-pipe";

        switch (getTraceType ())
        {
        case TraceAllChans:
            argv[argc++] = "--traceallchans";
            break;
        case TracePortsOnly:
            break;
            //        case TraceNamedPortsOnly:
            //            argv[argc++] = "--tracenamedportsonly";
            //            break;
        }

	argv[argc++] = "--vcdtracenamedportsonly";

        memcpy (argv + argc, breezesim_passedArgs, breezesim_passedArgCount * sizeof (char *));
        argc += breezesim_passedArgCount;
        argv[argc++] = tracefilearg;
        argv[argc++] = projectName;
        argv[argc++] = NULL;

/*
        int i = 0;
        while (argv[i])
        {
            fprintf (stderr, "argv[%d]=%s.\n", i, argv[i]);
            i++;
        }
*/
        execvp ("breeze-sim", argv);
    }
    //    Launch_GTKWave (1);

    SendSetspeedCommand (TRUE);

    write (breezesim_pipe[1], "run 1\n", 8);

    simulationIsRunning = TRUE;
    simulationIsPaused = FALSE;
    UpdateLEDs ();
}

void Simulation_Play (void)
{
    if (simulationIsRunning)
    {
        write (breezesim_pipe[1], "resume\n", 7);
        GrayControlIcons (FALSE, TRUE, FALSE);
        simulationIsPaused = FALSE;
        UpdateLEDs ();
    } else
    {
        ResetReadTrace (TRUE);
        Simulation_Run ();
    }

    //    if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (gtk_object_get_data (GTK_OBJECT (MainWindow2),
    //            "GraphToggleButton"))))
    //        StartReadTraceTimer1 ();
}

void Simulation_Pause (void)
{
    write (breezesim_pipe[1], "pause\n", 6);
    GrayControlIcons (TRUE, TRUE, FALSE);
    simulationIsPaused = TRUE;
    UpdateLEDs ();

    //    StopReadTraceTimer1 ();
}

void Simulation_Stop (void)
{
    write (breezesim_pipe[1], "stop\n", 5);
    //    GrayControlIcons (TRUE, FALSE, FALSE);

    //    StopReadTraceTimer1 ();
}

void OnSimulationMenu_Run (GtkMenuItem * menuitem, gpointer user_data)
{
    Simulation_Play ();
}

void on_PlayButton_clicked (GtkButton * button, gpointer user_data)
{
    Simulation_Play ();
}

void OnSimulationMenu_Pause (GtkMenuItem * menuitem, gpointer user_data)
{
    Simulation_Pause ();
}

void on_PauseButton_clicked (GtkButton * button, gpointer user_data)
{
    Simulation_Pause ();
}

void OnSimulationMenu_Stop (GtkMenuItem * menuitem, gpointer user_data)
{
    Simulation_Stop ();

}

void on_StopButton_clicked (GtkButton * button, gpointer user_data)
{
    Simulation_Stop ();
}

gboolean on_SimulationSpeedScale_event (GtkWidget * widget, GdkEvent * event, gpointer user_data)
{
    SendSetspeedCommand (FALSE);
    return FALSE;
}

gboolean on_drawingarea_led1_expose_event (GtkWidget * widget, GdkEventExpose * event, gpointer user_data)
{
    int state = 0;

    if (simulationIsRunning)
    {
        if (simulationIsPaused)
            state = 2;
        else
            state = 1;
    }
    gdk_draw_rectangle (widget->window,
      widget->style->bg_gc[GTK_WIDGET_STATE (widget)], TRUE, event->area.x, event->area.y, event->area.width, event->area.height);
    gdk_draw_arc (widget->window, GetColorGC (state + 1), TRUE,
      (widget->allocation.width - 7) / 2, (widget->allocation.height - 7) / 2, 7, 7, 0, 360 * 64);
    return FALSE;
}

void on_GTKWaveToggleButton (GtkButton * button, gpointer user_data)
{
    if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (button)))
    {
        if (!gtkwaveIsRunning)
            Launch_GTKWave (0);
    } else
    {
        if (gtkwaveIsRunning)
            KillGTKWave ();
    }
}

void KillThemAll (void)
{
    if (simulationIsRunning)
        kill (breezesimPid, SIGKILL);
    if (gtkwaveIsRunning)
        kill (gtkwavePid, SIGKILL);
    gtk_main_quit ();
}
