/*
	The Balsa Formatted Printing Library, as used in:
	  The Balsa Asynchronous Hardware Synthesis System
	  The GTKWave electronic waveform viewer
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

#include <stdio.h>
#include <stdlib.h>
#include "format/format.h"

FormatRadixData *radices[4];

FormatRadixData **MyRadixChoice (int radix, void *data)
{
    return radices;
}

main ()
{
    radices[0] = NewFormatRadixData (10, "", "", 0, 0);
    radices[1] = NewFormatRadixData (16, "0x", "", 0, 0);
    radices[2] = NewFormatRadixData (8, "0x", "x", 0, 0);
    radices[3] = NULL;

    FormatData *fd = NewFormatData (1);

    Format f = NewFormat (1);

    NewFormatNumber (f, (FormatPosition)
      {
      0, 32}
    );

    FormatPrintFormat (stderr, f);
    fprintf (stderr, "\n");

    char *remaining = FormatScan ("0x76x0x1", f, fd, MyRadixChoice, NULL);

    fprintf (stderr, "%08X `%s'\n", fd->words[0], remaining);

    fd = FormatDataParseUInt ("171798691890", 10);
    fprintf (stderr, "T %u %u\n", fd->words[0], fd->words[1]);

    FormatPrintFormat (stderr, FormatListExternalToInternal ("(format 2 \"Hello\" (number 0 32))"));
    FormatPrintFormat (stderr, FormatListExternalToInternal ("(format 3 \"Hello\" (number 0 32) (enum 33 32 2 (\"a\" 10) (\"b\" 20)))"));
}
