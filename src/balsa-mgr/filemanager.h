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

	`filemanager.h'

*/

#ifndef FILEMANAGER_HEADER
#define FILEMANAGER_HEADER

#include <gtk/gtk.h>

void FileManager_Initialize (void);
void FileManager_NewFile (void);
void FileManager_OpenFile (const char *filename);
void FileManager_OpenFileAtLine (const char *filename, int line);
void FileManager_OpenFileAtLineAndColumn (const char *filename, int line, int column);
void FileManager_ReOpenFile (void);
void FileManager_SaveFile (void);
void FileManager_SaveFileAs (void);
void FileManager_CloseFile (void);
void FileManager_CloseAllFiles (void);

char *FileManager_GetCurrentFileName (void);
char *FileManager_GetCurrentDisplayedName (void);
int FileManager_GetCurrentLineNumber (void);
void FileManager_SaveFileIfOpenedAndIfNeeded (char *filename);
void FileManager_SaveAllNamedFiles (void);
void FileManager_RefreshAllDisplayedNames (void);
void FileManager_SetDirtyFlagToCurrentFile (gboolean dirty);

gboolean FileManager_SignalTextActivate (GtkWidget * widget, gpointer data);

#endif
