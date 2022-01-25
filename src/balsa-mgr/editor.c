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

	`editor.c'
	Interface between filemanager.c and the Scintilla editor
*/

#include <stdio.h>
#include <glib.h>

#include "editor.h"
#include "filemanager.h"

#define PLAT_GTK 1
#include "Scintilla.h"
#include "ScintillaWidget.h"
#include "SciLexer.h"

#define SendEditor(m,w,l) scintilla_send_message ((ScintillaObject*)editor, m, (uptr_t)w, (sptr_t)l)

void EditorNotifySignal (GtkWidget * editor, gint wParam, gpointer lParam, gpointer data)
{
    struct SCNotification *notification = lParam;

    switch (notification->nmhdr.code)
    {
    case SCN_MARGINCLICK:
        if (notification->margin == 2)
        {
            int position = notification->position;
            int line_number = SendEditor (SCI_LINEFROMPOSITION, position, 0);

            SendEditor (SCI_TOGGLEFOLD, line_number, 0);
        }
        break;

    case SCN_MODIFIED:
        FileManager_SetDirtyFlagToCurrentFile (TRUE);
        FileManager_SignalTextActivate (editor, 0);
       	break;
    }
}

void initialise_editor (GtkWidget * editor)
{
    SendEditor (SCI_STYLECLEARALL, 0, 0);
    //        SendEditor (SCI_SETLEXER, SCLEX_VERILOG, 0);
    SendEditor (SCI_SETLEXERLANGUAGE, 0, "balsa");

    // Keywords
    SendEditor (SCI_SETKEYWORDS, 0,
      "if then else end type is constant variable channel array of sync procedure is function import shared print begin end multicast builtin new record enumeration over val as local case select arbitrate for in forseq loop print");

    // Operators
    SendEditor (SCI_SETKEYWORDS, 1, "let or xor and not log sizeof -> <-");

    // Attributes
    SendEditor (SCI_SETKEYWORDS, 2, "active passive input output parameter");

    // Std Functions
    SendEditor (SCI_SETKEYWORDS, 3, "bits byte unsigned signed");

    SendEditor (SCI_STYLESETFORE, SCE_V_DEFAULT, 0x000000);
    SendEditor (SCI_STYLESETFORE, SCE_V_COMMENT, 0x808080);
    SendEditor (SCI_STYLESETITALIC, SCE_V_COMMENT, 1);
    SendEditor (SCI_STYLESETFORE, SCE_V_COMMENTLINE, 0x808080);
    SendEditor (SCI_STYLESETITALIC, SCE_V_COMMENTLINE, 1);
    SendEditor (SCI_STYLESETFORE, SCE_V_COMMENTLINEBANG, 0x808080);
    SendEditor (SCI_STYLESETITALIC, SCE_V_COMMENTLINEBANG, 1);
    SendEditor (SCI_STYLESETFORE, SCE_V_NUMBER, 0xc000c0);
    SendEditor (SCI_STYLESETFORE, SCE_V_WORD, 0x800000);
    //        SendEditor (SCI_STYLESETBOLD, SCE_V_WORD, 1);
    SendEditor (SCI_STYLESETFORE, SCE_V_WORD2, 0x0000c0);
    SendEditor (SCI_STYLESETFORE, SCE_V_WORD3, 0x008000);
    SendEditor (SCI_STYLESETFORE, SCE_V_USER, 0x008080);
    SendEditor (SCI_STYLESETBOLD, SCE_V_USER, 1);

    SendEditor (SCI_STYLESETFORE, SCE_V_STRING, 0xff00ff);
    SendEditor (SCI_STYLESETFORE, SCE_V_PREPROCESSOR, 0x008080);
    SendEditor (SCI_STYLESETFORE, SCE_V_OPERATOR, 0x800080);
    SendEditor (SCI_STYLESETFORE, SCE_V_IDENTIFIER, 0x000000);
    SendEditor (SCI_STYLESETFORE, SCE_V_STRINGEOL, 0x0000ff);

    SendEditor (SCI_SETPROPERTY, "fold", "1");
    SendEditor (SCI_SETPROPERTY, "fold.comment", "1");
    SendEditor (SCI_SETPROPERTY, "fold.compact", "0");
    SendEditor (SCI_SETPROPERTY, "fold.at.else", "1");
    SendEditor (SCI_SETPROPERTY, "fold.at.Begin", "1");
    SendEditor (SCI_SETPROPERTY, "fold.at.Parenthese", "1");

    SendEditor (SCI_SETMARGINWIDTHN, 0, 30);
    SendEditor (SCI_SETMARGINWIDTHN, 1, 0);
    SendEditor (SCI_SETMARGINWIDTHN, 2, 15);
    SendEditor (SCI_SETFOLDFLAGS, 16, 0); // 16 = Draw line below if not expanded

    SendEditor (SCI_SETMARGINTYPEN, 2, SC_MARGIN_SYMBOL);
    SendEditor (SCI_SETMARGINMASKN, 2, SC_MASK_FOLDERS);
    SendEditor (SCI_SETMARGINSENSITIVEN, 2, 1);

    // Plus for contracted folders, minus for expanded
    SendEditor (SCI_MARKERDEFINE, SC_MARKNUM_FOLDER, SC_MARK_PLUS);
    SendEditor (SCI_MARKERDEFINE, SC_MARKNUM_FOLDEROPEN, SC_MARK_MINUS);
    SendEditor (SCI_MARKERDEFINE, SC_MARKNUM_FOLDEREND, SC_MARK_EMPTY);
    SendEditor (SCI_MARKERDEFINE, SC_MARKNUM_FOLDERMIDTAIL, SC_MARK_EMPTY);
    SendEditor (SCI_MARKERDEFINE, SC_MARKNUM_FOLDEROPENMID, SC_MARK_EMPTY);
    SendEditor (SCI_MARKERDEFINE, SC_MARKNUM_FOLDERSUB, SC_MARK_EMPTY);
    SendEditor (SCI_MARKERDEFINE, SC_MARKNUM_FOLDERTAIL, SC_MARK_EMPTY);

    SendEditor (SCI_SETWRAPMODE, 1, 0);
    SendEditor (SCI_SETWRAPVISUALFLAGS, SC_WRAPVISUALFLAG_END, 0);

    SendEditor (SCI_SETEOLMODE, SC_EOL_LF, 0);
}

GtkWidget *create_new_editor (void)
{
    GtkWidget *editor = scintilla_new ();

    initialise_editor (editor);
    SendEditor (SCI_SETMODEVENTMASK, SC_MOD_INSERTTEXT | SC_MOD_DELETETEXT, 0);
    gtk_signal_connect (GTK_OBJECT (editor), SCINTILLA_NOTIFY, (GtkSignalFunc) EditorNotifySignal, NULL);
    gtk_signal_connect (GTK_OBJECT (editor), "button_press_event", GTK_SIGNAL_FUNC (FileManager_SignalTextActivate), NULL);
    return editor;
}

int Editor_LoadFile (GtkWidget * editor, char *filename)
{
    FILE *f = fopen (filename, "r");

    if (!f)
        return 0;

    char buf[1001];
    int size;

    /* Clear the current text */
    SendEditor (SCI_CLEARALL, 0, 0);

    /* Load the file text.c into the text window */
    while ((size = fread (buf, 1, 1000, f)))
    {
        SendEditor (SCI_ADDTEXT, size, buf);
    }

    SendEditor (SCI_GOTOPOS, 0, 0);
    SendEditor (SCI_EMPTYUNDOBUFFER, 0, 0);

    fclose (f);
    return 1;
}

int Editor_SaveBuffer (GtkWidget * editor, char *filename)
{
    FILE *f = fopen (filename, "w");

    if (!f)
        return 0;

    int size = SendEditor (SCI_GETLENGTH, 0, 0);
    char *buf = g_new (char, size + 1);

    SendEditor (SCI_GETTEXT, size + 1, buf);

    fwrite (buf, 1, size, f);
    g_free (buf);
    fclose (f);

    return 1;
}

void Editor_GotoAndHighlight_LineColumn (GtkWidget * editor, int line, int column)
{
    int pos1 = SendEditor (SCI_POSITIONFROMLINE, line - 1, 0);
    int pos2 = SendEditor (SCI_POSITIONFROMLINE, line, 0);

    SendEditor (SCI_SETSEL, pos2, pos1 + column);

    int linesOnScreen = SendEditor (SCI_LINESONSCREEN, 0, 0);
    int firstVisibleLine = SendEditor (SCI_GETFIRSTVISIBLELINE, 0, 0);
    int Yoffset = line - (firstVisibleLine + linesOnScreen / 3);

    SendEditor (SCI_SETXOFFSET, 0, 0);
    if (Yoffset)
        SendEditor (SCI_LINESCROLL, 0, Yoffset);
}
