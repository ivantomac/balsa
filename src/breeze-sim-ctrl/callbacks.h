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

#include <gtk/gtk.h>

#define GTK_ENABLE_BROKEN
#include <gtk/gtktree.h>

void on_GO_clicked (GtkButton * button, gpointer user_data);

gboolean on_drawingarea1_configure_event (GtkWidget * widget, GdkEventConfigure * event, gpointer user_data);

gboolean on_drawingarea1_expose_event (GtkWidget * widget, GdkEventExpose * event, gpointer user_data);

void on_handlebox_toggle_clicked (GtkButton * button, gpointer user_data);

void OnTraceTypeChanged (GtkMenuItem * menuitem, gpointer user_data);

void OnMenuTraceTypeChanged (GtkMenuItem * menuitem, gpointer user_data);

void OnButtonTraceTypeChanged (GtkButton * button, gpointer user_data);

void OnSimulationMenu_Run (GtkMenuItem * menuitem, gpointer user_data);

void on_PlayButton_clicked (GtkButton * button, gpointer user_data);

void OnMenuSlowSimulationSpeedDown (GtkMenuItem * menuitem, gpointer user_data);

gboolean on_SimulationSpeedScale_button_press_event (GtkWidget * widget, GdkEventButton * event, gpointer user_data);

void on_StopButton_clicked (GtkButton * button, gpointer user_data);

void on_PauseButton_clicked (GtkButton * button, gpointer user_data);

gboolean on_SimulationSpeedScale_drag_motion (GtkWidget * widget, GdkDragContext * drag_context, gint x, gint y, guint time, gpointer user_data);

gboolean on_SimulationSpeedScale_motion_notify_event (GtkWidget * widget, GdkEventMotion * event, gpointer user_data);

void on_SimulationSpeedScale_state_changed (GtkWidget * widget, GtkStateType state, gpointer user_data);

void on_SimulationSpeedScale_draw (GtkWidget * widget, GdkRectangle * area, gpointer user_data);

gboolean on_SimulationSpeedScale_event (GtkWidget * widget, GdkEvent * event, gpointer user_data);

void on_SimulationSpeedScale_realize (GtkWidget * widget, gpointer user_data);

void on_drawingarea2_draw (GtkWidget * widget, GdkRectangle * area, gpointer user_data);

gboolean on_drawingarea2_expose_event (GtkWidget * widget, GdkEventExpose * event, gpointer user_data);

gboolean on_drawingarea_led1_expose_event (GtkWidget * widget, GdkEventExpose * event, gpointer user_data);

gboolean on_drawingarea_led2_expose_event (GtkWidget * widget, GdkEventExpose * event, gpointer user_data);

gboolean on_led2_button_press_event (GtkWidget * widget, GdkEventButton * event, gpointer user_data);

void OnMenuTraceTypeChanged (GtkMenuItem * menuitem, gpointer user_data);

void OnSimulationMenu_Pause (GtkMenuItem * menuitem, gpointer user_data);

void OnSimulationMenu_Stop (GtkMenuItem * menuitem, gpointer user_data);

void OnMenuTraceTypeChanged (GtkMenuItem * menuitem, gpointer user_data);

void OnMenuTraceTypeChanged1 (GtkMenuItem * menuitem, gpointer user_data);

void OnMenuTraceTypeChanged2 (GtkMenuItem * menuitem, gpointer user_data);

void OnMenuTraceTypeChanged3 (GtkMenuItem * menuitem, gpointer user_data);

void KillAndRestartGTKWave (GtkButton * button, gpointer user_data);

void StartNewGTKWave (GtkButton * button, gpointer user_data);

void KillAndQuit (GtkButton * button, gpointer user_data);

void Quit (GtkButton * button, gpointer user_data);

gboolean OnQuit (GtkWidget * widget, GdkEvent * event, gpointer user_data);

void OnFileMenu_Quit (GtkMenuItem * menuitem, gpointer user_data);

void on_GTKWaveToggleButton (GtkButton * button, gpointer user_data);

gboolean on_drawingarea1_button_press_event (GtkWidget * widget, GdkEventButton * event, gpointer user_data);

gboolean on_drawingarea1_button_release_event (GtkWidget * widget, GdkEventButton * event, gpointer user_data);

gboolean on_drawingarea1_motion_notify_event (GtkWidget * widget, GdkEventMotion * event, gpointer user_data);

void on_ZoomPlus_clicked (GtkButton * button, gpointer user_data);

void on_ZoomMinus_clicked (GtkButton * button, gpointer user_data);

void on_ZoomFit_clicked (GtkButton * button, gpointer user_data);

void on_AnimateButton_toggled (GtkToggleButton * togglebutton, gpointer user_data);

void on_TimeSpin_changed (GtkEditable * editable, gpointer user_data);

void on_step_clicked (GtkButton * button, gpointer user_data);

void on_NoDroppingCheckbutton_toggled (GtkToggleButton * togglebutton, gpointer user_data);

void on_DontDropButton_clicked (GtkButton * button, gpointer user_data);

void on_DontDropToggle_toggled (GtkToggleButton * togglebutton, gpointer user_data);

gboolean on_ReadTraceSpeedScale_event (GtkWidget * widget, GdkEvent * event, gpointer user_data);

gboolean on_SourceView_text_button_press_event (GtkWidget * widget, GdkEventButton * event, gpointer user_data);

void on_button7_clicked (GtkButton * button, gpointer user_data);

gboolean on_SourceViewWindow_delete_event (GtkWidget * widget, GdkEvent * event, gpointer user_data);

void on_ZoomFit_pressed (GtkButton * button, gpointer user_data);

gboolean on_initSourceText_button_press_event (GtkWidget * widget, GdkEventButton * event, gpointer user_data);

void on_button8_clicked (GtkButton * button, gpointer user_data);

gboolean on_SourceViewHBox_configure_event (GtkWidget * widget, GdkEventConfigure * event, gpointer user_data);

gboolean on_SourceViewHBox_expose_event (GtkWidget * widget, GdkEventExpose * event, gpointer user_data);

void on_WarningButton_clicked (GtkButton * button, gpointer user_data);

void on_CallListTree_select_child (GtkTree * tree, GtkWidget * widget, gpointer user_data);

void on_CallListTree_selection_changed (GtkTree * tree, gpointer user_data);

void on_CallListTree_unselect_child (GtkTree * tree, GtkWidget * widget, gpointer user_data);

gboolean on_CallListTree_button_press_event (GtkWidget * widget, GdkEventButton * event, gpointer user_data);

void on_ShowAllChansPositionsButton_clicked (GtkButton * button, gpointer user_data);

void on_GoNextDataButton_clicked (GtkButton * button, gpointer user_data);

gboolean on_AnimateSpeedScale_event (GtkWidget * widget, GdkEvent * event, gpointer user_data);

void on_GoNextDataButton_clicked (GtkButton * button, gpointer user_data);

void on_ScreenShot_clicked (GtkButton * button, gpointer user_data);

void on_ShowChansToggle_toggled (GtkToggleButton * togglebutton, gpointer user_data);

void on_MaxDev_clicked (GtkButton * button, gpointer user_data);

void on_MaxDev_clicked (GtkButton * button, gpointer user_data);

void on_HalfDev_clicked (GtkButton * button, gpointer user_data);

void OnMenuStructureViewChanged (GtkMenuItem * menuitem, gpointer user_data);

void OnMenuBehaviourStructure_activate (GtkMenuItem * menuitem, gpointer user_data);

void OnMenuStructureViewChanged_activate (GtkMenuItem * menuitem, gpointer user_data);

void OnMenuStructureViewChanged (GtkMenuItem * menuitem, gpointer user_data);

void on_BehaviourDev_clicked (GtkButton * button, gpointer user_data);

void on_ShowWireforkTreeToggle_toggled (GtkToggleButton * togglebutton, gpointer user_data);

void on_BehaviourDev2_clicked (GtkButton * button, gpointer user_data);

void on_BehaviourDev3_clicked (GtkButton * button, gpointer user_data);

void on_BehaviourDev4_clicked (GtkButton * button, gpointer user_data);

void on_BehaviourDev5_clicked (GtkButton * button, gpointer user_data);

gboolean on_TimeLineBehaviourViewWindow_delete_event (GtkWidget * widget, GdkEvent * event, gpointer user_data);

gboolean on_TimeLineBehaviour_drawingarea_configure_event (GtkWidget * widget, GdkEventConfigure * event, gpointer user_data);

gboolean on_TimeLineBehaviour_drawingarea_expose_event (GtkWidget * widget, GdkEventExpose * event, gpointer user_data);

void on_ShowDebugToolbarToggle_toggled (GtkToggleButton * togglebutton, gpointer user_data);

void on_ShowTimeLineBehaviourToggle_toggled (GtkToggleButton * togglebutton, gpointer user_data);

void on_TimeLineBehaviourSpinButton_changed (GtkEditable * editable, gpointer user_data);

void on_TimeLineBehaviourZoomSpinButton_changed (GtkEditable * editable, gpointer user_data);

gboolean on_TimeLineBehaviour_drawingarea_button_press_event (GtkWidget * widget, GdkEventButton * event, gpointer user_data);

void on_NotebookStyleCheckbutton_toggled (GtkToggleButton * togglebutton, gpointer user_data);

void on_GotoSelectedChannelSourceButton_clicked (GtkButton * button, gpointer user_data);

void on_SelectionDeleteButton_clicked (GtkButton * button, gpointer user_data);

void on_SelectionShowHideButton_clicked (GtkButton * button, gpointer user_data);

void on_SelectionDilateButton_clicked (GtkButton * button, gpointer user_data);

void on_SelectionFollowButton_clicked (GtkButton * button, gpointer user_data);

void on_source_select_channels_activate (GtkMenuItem * menuitem, gpointer user_data);

void on_source_toggle_breakpoint_activate (GtkMenuItem * menuitem, gpointer user_data);

void on_SelectionRunUntilButton_clicked (GtkButton * button, gpointer user_data);

void on_GraphViewNextActionButton_clicked (GtkButton * button, gpointer user_data);

void TwoInstances_UsePrevious (GtkButton * button, gpointer user_data);

void TwoInstances_StartNew (GtkButton * button, gpointer user_data);

void TwoInstances_KillPreviousAndRestart (GtkButton * button, gpointer user_data);

void OnMenuGTKWaveViewTypeChanged (GtkMenuItem * menuitem, gpointer user_data);

void OnMenuGTKWaveViewTypeChanged (GtkMenuItem * menuitem, gpointer user_data);

void on_TimeSpin_next (GtkButton * button, gpointer user_data);

void on_TimeSpin_previous (GtkButton * button, gpointer user_data);

gboolean on_drawingarea1_key_press_event (GtkWidget * widget, GdkEventKey * event, gpointer user_data);

void on_SelectionCaptureToggleButton_toggled (GtkToggleButton * togglebutton, gpointer user_data);

void on_TimeLineBehaviourScreenshot_clicked (GtkButton * button, gpointer user_data);

void on_TimeLineBehaviourLivelockAnalysis_clicked (GtkButton * button, gpointer user_data);

void on_ChannelColorLimitSpinButton_changed (GtkEditable * editable, gpointer user_data);

void on_ShowChannelNamesToggle_toggled (GtkToggleButton * togglebutton, gpointer user_data);

gboolean on_drawingarea1_key_press_event (GtkWidget * widget, GdkEventKey * event, gpointer user_data);

gboolean on_drawingarea1_key_press_event (GtkWidget * widget, GdkEventKey * event, gpointer user_data);

gboolean on_drawingarea1_key_press_event (GtkWidget * widget, GdkEventKey * event, gpointer user_data);

gboolean on_key_press_event (GtkWidget * widget, GdkEventKey * event, gpointer user_data);

void on_SelectionChanNumSearchEntry_changed (GtkEditable * editable, gpointer user_data);

void on_SelectionChanNumSearchEntry_activate (GtkEditable * editable, gpointer user_data);

gboolean on_SelectionChanNumSearchEntry_key_press_event (GtkWidget * widget, GdkEventKey * event, gpointer user_data);

void on_NbGraphviewColumnsSpinButton_changed (GtkEditable * editable, gpointer user_data);

gboolean on_drawingarea1_motion_notify_event (GtkWidget * widget, GdkEventMotion * event, gpointer user_data);

gboolean on_drawingarea_configure_event (GtkWidget * widget, GdkEventConfigure * event, gpointer user_data);

gboolean on_drawingarea_configure_event (GtkWidget * widget, GdkEventConfigure * event, gpointer user_data);

gboolean on_drawingarea1_expose_event (GtkWidget * widget, GdkEventExpose * event, gpointer user_data);

gboolean on_drawingarea1_button_press_event (GtkWidget * widget, GdkEventButton * event, gpointer user_data);

gboolean on_drawingarea1_button_release_event (GtkWidget * widget, GdkEventButton * event, gpointer user_data);

gboolean on_drawingarea1_motion_notify_event (GtkWidget * widget, GdkEventMotion * event, gpointer user_data);

void on_ControlRadioButton_toggled (GtkToggleButton * togglebutton, gpointer user_data);

void on_DataRadioButton_toggled (GtkToggleButton * togglebutton, gpointer user_data);

void on_ControlRadioButton_toggled (GtkToggleButton * togglebutton, gpointer user_data);

void on_DataRadioButton_toggled (GtkToggleButton * togglebutton, gpointer user_data);

void on_ShowChannelValuesToggle_toggled (GtkToggleButton * togglebutton, gpointer user_data);

void OnDebugMenu_HighlightDeadlock (GtkMenuItem * menuitem, gpointer user_data);

void on_ShortcutsHelpTogglebutton_toggled (GtkToggleButton * togglebutton, gpointer user_data);

gboolean on_GraphviewShortcutsHelpWindow_delete_event (GtkWidget * widget, GdkEvent * event, gpointer user_data);

void on_FileMenu_SaveEnvironment_activate (GtkMenuItem * menuitem, gpointer user_data);

void OnFileMenu_SaveEnvironment (GtkMenuItem * menuitem, gpointer user_data);

void on_ControlLayout_Checkbutton_toggled (GtkToggleButton * togglebutton, gpointer user_data);

void on_Partitioning_clicked (GtkButton * button, gpointer user_data);

void on_ImportPartition_clicked (GtkButton * button, gpointer user_data);

void on_ExportNewBreze_clicked (GtkButton * button, gpointer user_data);

void on_PartitioningWithNegWeights_clicked (GtkButton * button, gpointer user_data);

void OnDebugMenu_HighlightCharliesSlowestPath (GtkMenuItem * menuitem, gpointer user_data);

void OnDebugMenu_TranslateCharlieToTraceFile (GtkMenuItem * menuitem, gpointer user_data);

void on_SelectPartitionCut_clicked (GtkButton * button, gpointer user_data);

void OnFileMenu_LoadEnvironment (GtkMenuItem * menuitem, gpointer user_data);
