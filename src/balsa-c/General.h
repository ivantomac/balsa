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

	`General.h'
	Misc defns. like libreuse but using libglib.
	
 */

#ifndef GENERAL_HEADER
#define GENERAL_HEADER

#include <glib.h>

/* extern unsigned int Log2 (unsigned long x), NB. this is |_ log2 x _| */
#define Log2(x) \
	(g_bit_storage ((x) + 1) - 1)

/* extern unsigned long Exp2 (unsigned int x) */
#define Exp2(x) \
	(1 << (x))

struct AlignmentExample
{
    char imSmall;
    double imBig;
};

#define yyMaxAlign (sizeof (struct AlignmentExample) - sizeof (double))

extern long yyAlignMasks[];

#endif /* GENERAL_HEADER */
