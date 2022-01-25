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

	`buffer.c'
	Line read buffer handling

*/

#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <glib.h>

#include "buffer.h"

/* ResetReadBuffer : reset the count and endPtr in the given buffer */
void ResetReadBuffer (ReadBuffer * buff)
{
    buff->count = 0;
    buff->endPtr = buff->buffer;
}

/* InitReadBuffer : initialise a new read buffer */
void InitReadBuffer (ReadBuffer * buff)
{
    ResetReadBuffer (buff);
    *(buff->buffer) = '\0';
}

/* ReturnLineFromReadBuffer : read a line from the given ReadBuffer and return a pointer
	to that line (w/o \n). Modifies the given buffer and returns NULL if no full line
	is present in the buffer. */
char *ReturnLineFromReadBuffer (ReadBuffer * buff)
{
    char *oldEndPtr, *nextEnd;

    if (buff->count == 0)
        return NULL;
    oldEndPtr = buff->endPtr;
    nextEnd = strchr (oldEndPtr, '\n');

    if (nextEnd)
    {
        *nextEnd = '\0';        /* get rid of the \n */
        buff->endPtr = nextEnd + 1;
    } else
        oldEndPtr = NULL;

    return oldEndPtr;
}

/* ReadLineUsingReadBuffer : like ReturnLineFromReadBuffer but read from the given
	fd if more input is needed. If `completeLine' != NULL then sets *`completeLine'
	to TRUE if the read line was complete (orig. had a \n) and FALSE if the
	line is partial. */
char *ReadLineUsingReadBuffer (int fd, ReadBuffer * buff, gboolean * completeLine)
{
    char *line = ReturnLineFromReadBuffer (buff);

    if (line)
    {                           /* have a line in the buffer */
        if (completeLine)
            *completeLine = TRUE; /* RLFRB always returns full lines */
        return line;
    } else
    {
        int readCount;

        if (buff->count != 0)
        {                       /* have some chars in the buffer *//* copy to start of buffer, read rest of buffer */
            buff->count -= buff->endPtr - buff->buffer;
            memmove (buff->buffer, buff->endPtr, buff->count);
            buff->endPtr = buff->buffer;
            buff->buffer[buff->count] = '\0';
        }
        /* chars just in buffer[0..count-1] */
        do
        {
            readCount = read (fd, buff->buffer + buff->count, (READ_BUFFER_LEN - 1) - buff->count);
        }
        while (readCount < 0 && errno == EINTR);

        if (readCount <= 0)
        {
            if (completeLine)
                *completeLine = FALSE;
            if (buff->count == 0)
            {                   /* No chars in buffer */
                return NULL;    /* EOF */
            } else
            {
                ResetReadBuffer (buff);
                return buff->buffer; /* partial line, EOF next time around */
            }
        } else
        {
            buff->count += readCount;
            buff->buffer[buff->count] = '\0'; /* ensure null termination */

            line = ReturnLineFromReadBuffer (buff);
            if (completeLine)
                *completeLine = (line ? TRUE : FALSE);

            if (line)
                return line;
            else if (buff->count == (READ_BUFFER_LEN - 1))
            {                   /* partial line */
                ResetReadBuffer (buff);
                return buff->buffer;
            } else
            {
                return ReadLineUsingReadBuffer (fd, buff, completeLine); /* try to read again */
            }
        }
    }
}
