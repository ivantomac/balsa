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

#ifndef ICONS_HEADER
#define ICONS_HEADER

#include <gtk/gtk.h>

extern GdkBitmap *BalsaBlockMask;
extern GdkPixmap *BalsaBlockPixmap;
extern GdkBitmap *BalsaBlockWithExclamationMask;
extern GdkPixmap *BalsaBlockWithExclamationPixmap;
extern GdkBitmap *BreezeBlockMask;
extern GdkPixmap *BreezeBlockPixmap;

extern GdkPixmap *SyncPortPixmap;
extern GdkBitmap *SyncPortMask;
extern GdkPixmap *InputFilePortPixmap;
extern GdkPixmap *OutputFilePortPixmap;
extern GdkBitmap *InputFilePortMask;
extern GdkBitmap *OutputFilePortMask;
extern GdkPixmap *InputValuePortPixmap;
extern GdkPixmap *OutputStdoutPortPixmap;
extern GdkBitmap *InputValuePortMask;
extern GdkBitmap *OutputStdoutPortMask;
extern GdkPixmap *MemoryComponentPixmap;
extern GdkBitmap *MemoryComponentMask;
extern GdkPixmap *UndefinedPortPixmap;
extern GdkBitmap *UndefinedPortMask;

extern GdkPixmap *ProcedurePixmap;
extern GdkBitmap *ProcedureMask;
extern GdkPixmap *ImplementationPixmap;
extern GdkBitmap *ImplementationMask;
extern GdkPixmap *BuiltinLibPixmap;
extern GdkBitmap *BuiltinLibMask;
extern GdkPixmap *TickPixmap;
extern GdkBitmap *TickMask;
extern GdkPixmap *SelectedPixmap;
extern GdkBitmap *SelectedMask;
extern GdkPixmap *DirectoryPixmap;
extern GdkBitmap *DirectoryMask;

extern void InitIcons (void);

#endif
