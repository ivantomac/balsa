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

#include "format/external.h"
#include "format/data.h"

void ScanTest (void)
{
    /* scan test */

    char s[] = "123foobar36";   /* 0x7B 0x24 */

    Format f = NewFormat (3);
    FormatPosition p1 = { 12, 64 };
    FormatPosition p2 = { 8, 2 };
    FormatPosition p3 = { 0, 4 };

    FormatEnumerationElement elts[3];

    elts[0].name = "foo";
    elts[0].value = FormatDataParseUInt ("1", 10);
    elts[1].name = "bar";
    elts[1].value = FormatDataParseUInt ("2", 10);
    elts[2].name = "foobar";
    elts[2].value = FormatDataParseUInt ("3", 10);

    NewFormatNumber (f + 0, p1);
// NewFormatLiteral (f + 1, " ", 0);
    NewFormatEnumeration (f + 1, p2, 3, elts);
    NewFormatNumber (f + 2, p3);

    FormatData *fd = NewFormatData (3);
    int ret = FormatScan (s, f, &fd, FormatBareDecimalRadixChoice, NULL);

    fprintf (stderr, "ret=%d\nfdsize=%d\nfd[0]=%d\nfd[1]=%d\nfd[2]=%d\n", ret, fd->wordCount, fd->words[0], fd->words[1], fd->words[2]);
    FormatDataFormatAsUInt (s, fd, 16, 0);
    fprintf (stderr, "`%s'\n", s);
}

main ()
{
    Format f = NewFormat (3);
    FormatData *fd = NewFormatDataFromUnsignedInt (3, 0);
    char *s;
    FormatPosition p1 = { 4, 64 };
    FormatPosition p2 = { 4, 4 };

    NewFormatNumber (f + 0, p1);
    NewFormatLiteral (f + 1, "Hello", 0);
    NewFormatNumber (f + 2, p2);

    s = malloc (1000);          // malloc (FormatLength (f, FormatBareDecimalRadixChoice, NULL));
    fd->words[0] = 0x76543210;
    fd->words[1] = 0xFEDCBA98;
    fd->words[2] = 0x76543210;

    FormatPrint (s, f, fd, FormatBareDecimalRadixChoice, NULL);
    fprintf (stderr, "`%s'\n", s);

    // Format f2 = FormatExternalToInternal ("{{%(67:4)n}}");
    Format f2 = FormatExternalToInternal ("{{kfsklfj%(1:0)n");

    s = malloc (FormatLength (f2, FormatBareDecimalRadixChoice, NULL, NULL));
    FormatPrint (s, f2, fd, FormatBareDecimalRadixChoice, NULL);
    fprintf (stderr, "`%s'\n", s);

    Format f3 = FormatExternalToInternal ("%(63:0)n");

    s = malloc (FormatLength (f3, FormatBareDecimalRadixChoice, NULL, NULL));
    FormatPrint (s, f3, FormatDataParseUInt ("74823478347890234", 10), FormatBareDecimalRadixChoice, NULL);
    fprintf (stderr, "`%s'\n", s);

    s = malloc (1000);
    FormatDataFormatAsUInt (s, FormatDataParseUInt ("0", 10), 10, 0);
    fprintf (stderr, "`%s'\n", s);
    FormatDataFormatAsUInt (s, FormatDataParseUInt ("102787", 10), 10, 0);
    fprintf (stderr, "`%s'\n", s);
    FormatDataFormatAsUInt (s, FormatDataParseUInt ("FFFFFFFFFFFF", 16), 10, 0);
    fprintf (stderr, "`%s'\n", s);
    FormatDataFormatAsUInt (s, FormatDataParseUInt ("FFFF5FFFFFFF", 16), 16, 0);
    fprintf (stderr, "`%s'\n", s);
    FormatDataFormatAsUInt (s, FormatDataParseUInt ("1027873473897480097347", 10), 10, 0);
    fprintf (stderr, "`%s'\n\n", s);

    ScanTest ();

    FormatData *d1, *d2;

    d1 = FormatDataParseUInt ("010111110101", 2);
    FormatDataInsertBitField (d1, FormatDataParseUInt ("0101", 2), 4, 4);
    FormatDataFormatAsUInt (s, d1, 2, 0);
    fprintf (stderr, "`%s'\n\n", s);

    // d1 = FormatDataParseUInt ("0123456789ABCDEF0123456789ABCDEF", 16);
    d2 = FormatDataParseUInt ("012345678901234567", 16);

    int i;

    for (i = 0; i <= 24; i += 4)
    {
        d1 = FormatDataParseUInt ("DDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDD", 16);
        FormatDataInsertBitField (d1, d2, i, 72);
        FormatDataFormatAsUInt (s, d1, 16, 0);
        fprintf (stderr, "`%s'\n", s);
    }
}
