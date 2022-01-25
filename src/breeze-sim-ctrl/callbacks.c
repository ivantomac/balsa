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

#include <stdio.h>

#include "callbacks.h"
#include "interface.h"
#include "support.h"

/*
void
on_GO_clicked                          (GtkButton       *button,
                                        gpointer         user_data)
{

}

gboolean
on_drawingarea1_configure_event        (GtkWidget       *widget,
                                        GdkEventConfigure *event,
                                        gpointer         user_data)
{

  return FALSE;
}

gboolean
on_drawingarea1_expose_event           (GtkWidget       *widget,
                                        GdkEventExpose  *event,
                                        gpointer         user_data)
{

  return FALSE;
}

void
OnTraceTypeChanged                     (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}

void
OnMenuTraceTypeChanged                 (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}

void
OnButtonTraceTypeChanged               (GtkButton       *button,
                                        gpointer         user_data)
{

}

void
OnMenuSlowSimulationSpeedDown          (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}

void
OnSimulationMenu_Run                   (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}

void
on_PlayButton_clicked                  (GtkButton       *button,
                                        gpointer         user_data)
{

}

void
on_StopButton_clicked                  (GtkButton       *button,
                                        gpointer         user_data)
{

}

void
on_PauseButton_clicked                 (GtkButton       *button,
                                        gpointer         user_data)
{

}

gboolean
on_SimulationSpeedScale_event          (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data)
{
    printf ("event\n");

  return FALSE;
}

void
on_ShowAllChansPositionsButton_clicked (GtkButton       *button,
                                        gpointer         user_data)
{

}

void
on_GoNextDataButton_clicked            (GtkButton       *button,
                                        gpointer         user_data)
{

}

gboolean
on_AnimateSpeedScale_event             (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data)
{

  return FALSE;
}

void
on_GoNextDataButton_clicked            (GtkButton       *button,
                                        gpointer         user_data)
{

}

void
on_ScreenShot_clicked                  (GtkButton       *button,
                                        gpointer         user_data)
{

}

void
on_ShowChansToggle_toggled             (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{

}

void
on_MaxDev_clicked                      (GtkButton       *button,
                                        gpointer         user_data)
{

}

void
on_HalfDev_clicked                     (GtkButton       *button,
                                        gpointer         user_data)
{

}

void
on_BehaviourDev_clicked                (GtkButton       *button,
                                        gpointer         user_data)
{

}

void
on_BehaviourDev2_clicked               (GtkButton       *button,
                                        gpointer         user_data)
{

}

void
on_BehaviourDev3_clicked               (GtkButton       *button,
                                        gpointer         user_data)
{

}

void
on_BehaviourDev4_clicked               (GtkButton       *button,
                                        gpointer         user_data)
{

}

void
on_BehaviourDev5_clicked               (GtkButton       *button,
                                        gpointer         user_data)
{

}

void
on_ShowWireforkTreeToggle_toggled      (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{

}

void
on_ShowDebugToolbarToggle_toggled      (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{
}

void
on_ShowTimeLineBehaviourToggle_toggled (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{

}

gboolean
on_TimeLineBehaviour_drawingarea_configure_event
                                        (GtkWidget       *widget,
                                        GdkEventConfigure *event,
                                        gpointer         user_data)
{

  return FALSE;
}

gboolean
on_TimeLineBehaviour_drawingarea_expose_event
                                        (GtkWidget       *widget,
                                        GdkEventExpose  *event,
                                        gpointer         user_data)
{

  return FALSE;
}

gboolean
on_TimeLineBehaviourViewWindow_delete_event
                                        (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data)
{

  return FALSE;
}

void
on_TimeLineBehaviourSpinButton_changed (GtkEditable     *editable,
                                        gpointer         user_data)
{

}

void
on_TimeLineBehaviourZoomSpinButton_changed
                                        (GtkEditable     *editable,
                                        gpointer         user_data)
{

}

gboolean
on_TimeLineBehaviour_drawingarea_button_press_event
                                        (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data)
{

  return FALSE;
}

void
on_GotoSelectedChannelSourceButton_clicked
                                        (GtkButton       *button,
                                        gpointer         user_data)
{

}

void
on_NotebookStyleCheckbutton_toggled    (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{

}

void
on_SelectionDeleteButton_clicked       (GtkButton       *button,
                                        gpointer         user_data)
{

}

void
on_SelectionShowHideButton_clicked     (GtkButton       *button,
                                        gpointer         user_data)
{

}

void
on_SelectionDilateButton_clicked       (GtkButton       *button,
                                        gpointer         user_data)
{

}

void
on_SelectionFollowButton_clicked       (GtkButton       *button,
                                        gpointer         user_data)
{

}

void
on_source_select_channels_activate     (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}

void
on_source_toggle_breakpoint_activate   (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}

void
on_SelectionRunUntilButton_clicked     (GtkButton       *button,
                                        gpointer         user_data)
{

}

void
on_GraphViewNextActionButton_clicked   (GtkButton       *button,
                                        gpointer         user_data)
{

}

void
TwoInstances_UsePrevious               (GtkButton       *button,
                                        gpointer         user_data)
{

}

void
TwoInstances_StartNew                  (GtkButton       *button,
                                        gpointer         user_data)
{

}

void
TwoInstances_KillPreviousAndRestart    (GtkButton       *button,
                                        gpointer         user_data)
{

}

void
on_TimeSpin_next                       (GtkButton       *button,
                                        gpointer         user_data)
{

}

void
on_TimeSpin_previous                   (GtkButton       *button,
                                        gpointer         user_data)
{

}

void
on_TimeLineBehaviourScreenshot_clicked (GtkButton       *button,
                                        gpointer         user_data)
{

}

void
on_TimeLineBehaviourLivelockAnalysis_clicked
                                        (GtkButton       *button,
                                        gpointer         user_data)
{

}

void
on_ChannelColorLimitSpinButton_changed (GtkEditable     *editable,
                                        gpointer         user_data)
{

}

void
on_ShowChannelNamesToggle_toggled      (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{

}

gboolean
on_key_press_event                     (GtkWidget       *widget,
                                        GdkEventKey     *event,
                                        gpointer         user_data)
{

  return FALSE;
}

void
on_SelectionChanNumSearchEntry_activate
                                        (GtkEditable     *editable,
                                        gpointer         user_data)
{

}

void
on_NbGraphviewColumnsSpinButton_changed
                                        (GtkEditable     *editable,
                                        gpointer         user_data)
{

}

void
on_ControlRadioButton_toggled          (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{

}

void
on_DataRadioButton_toggled             (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{

}

void
on_ShowChannelValuesToggle_toggled     (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{

}

void
OnDebugMenu_HighlightDeadlock          (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}

void
OnFileMenu_SaveEnvironment             (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}

void
on_ControlLayout_Checkbutton_toggled   (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{

}

*/

void OnMenuStructureViewChanged (GtkMenuItem * menuitem, gpointer user_data)
{
}

void OnMenuGTKWaveViewTypeChanged (GtkMenuItem * menuitem, gpointer user_data)
{

}
