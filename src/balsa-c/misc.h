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

	`misc.h'
	Miscellaneous definitions
	
 */

#ifndef MISC_HEADER
#define MISC_HEADER

#include "../../config.h"

#include "Idents.h"
#include "rMemory.h"
#include <sys/types.h>
#include <stdio.h>
#include <assert.h>

/* bool, I know, I know, everyone defines it, */
#ifndef bool
#define bool char
#endif

#ifndef false
#define false (0)
#endif

#ifndef true
#define true (!false)
#endif

/* ASSERT : assert macro, removable */
#define ASSERT(pred) assert(pred)

/* Bits : A type to represent numbers of bits */
typedef unsigned Bits;

/* SignedBits : like Bits but -8 means 8 signed bits */
typedef int SignedBits;

/* BundleNumber : unique number for a channel bundle */
typedef unsigned BundleNumber;
typedef BundleNumber *PtrBundleNumber;

/* Invalid bundle */
#define NoBundle (0)

/* Integer pointers */
typedef int *Ptrint;
typedef unsigned *Ptrunsigned;
typedef char *Ptrchar;
typedef const char *Ptrconstchar;
typedef void *Ptrvoid;
typedef bool *Ptrbool;

/* Identifier 'Marker' */
extern tIdent MarkerIdent;

/* OFFSETOF : offsetof for the uneducated OS */
#define OFFSETOF(type,member) ((size_t) &((Ptr##type)0)->member)

/* {SCHEME}_BOOL_STRING : Convert a bool to a true/false string */
#define BOOL_STRING(x) ((x)? "true" : "false")
#define BOOL_RAW_STRING(x) ((x)? "\"true\"" : "\"false\"")
#define SCHEME_BOOL_STRING(x) ((x)? "#t" : "#f")

/* Create a new object of type type */
#define NEW(type) ((Ptr##type) Alloc (sizeof (type)))
#define NEW_ARRAY(type,n) ((Ptr##type) Alloc ((n) * sizeof (type)))
#define FREE_ARRAY(type,n,ptr) Free ((n) * sizeof (type), (char*)(ptr))
#define EXTEND_ARRAY(oldptr,type,n) ((Ptr##type) Realloc (oldptr, (n) * sizeof (type)))

/* BeginMisc : initialise misc module */
extern void BeginMisc (void);

/* Abs : Absolute value of argument - saves including the maths lib */
extern unsigned Abs (int x);

/* Max : maximum of two ints */
extern int Max (int a, int b);

/* IsInteger : returns true if string begins with -<digit> or <digit> */
extern bool IsInteger (char *string);

/* StrBundleNumber : print a BundleNumber */
extern void StrBundleNumber (FILE * stream, BundleNumber bundle);

/* EndOfString : return a pointer to the NULL termination on string str */
extern char *EndOfString (char *str);

/* StrDup : like strdup but using NEW_ARRAY, StrNDup is similar but only copies n char's */
extern char *StrDup (char *str);
extern char *StrNDup (char *str, size_t n);

/* SubstituteCharInString : swap one char for another in the given string */
extern void SubstituteCharInString (char *str, char fromChar, char toChar);

/* RemoveCharFromString : remove occurences of the character chr from the string str */
extern char *RemoveCharFromString (char *str, char chr);

/* StrSignedBits : show a SignedBit's as though it were a numeric type declaration
   ie. <num> [signed] bits */
extern void StrSignedBits (FILE * stream, SignedBits bits);

#endif /* MISC_HEADER */
