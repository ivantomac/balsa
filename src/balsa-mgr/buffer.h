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

	`buffer.h'
	Line read buffer handling

*/

#ifndef BUFFER_HEADER
#define BUFFER_HEADER

#define READ_BUFFER_LEN (1024)

/* ReadBuffer : stream line read buffer */
typedef struct
{
    char buffer[READ_BUFFER_LEN];
    char *endPtr;
    int count;
}
ReadBuffer;

/* ResetReadBuffer : reset the count and endPtr in the given buffer */
extern void ResetReadBuffer (ReadBuffer * buff);

/* InitReadBuffer : initialise a new read buffer */
extern void InitReadBuffer (ReadBuffer * buff);

/* ReturnLineFromReadBuffer : read a line from the given ReadBuffer and return a pointer
	to that line (w/o \n). Modifies the given buffer and returns NULL if no full line
	is present in the buffer */
extern char *ReturnLineFromReadBuffer (ReadBuffer * buff);

/* ReadLineUsingReadBuffer : like ReturnLineFromReadBuffer but read from the given
	fd if more input is needed. If `completeLine' != NULL then sets *`completeLine'
	to TRUE if the read line was complete (orig. had a \n) and FALSE if the
	line is partial. */
extern char *ReadLineUsingReadBuffer (int fd, ReadBuffer * buff, gboolean * completeLine);

#endif
