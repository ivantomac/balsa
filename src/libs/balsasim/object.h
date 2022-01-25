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

	`object.h'
	Balsa object representation

*/

#ifndef BALSA_SIM_OBJECT_H
#define BALSA_SIM_OBJECT_H

#include "format/data.h"
#include <stdio.h>
#include <stdbool.h>

/* BalsaDebug : set >0 to turn on object debugging */
extern int BalsaDebug;

/* BalsaDestructor : object->data destructor */
typedef void (*BalsaDestructor) (void *data);

/* BalsaObject : Balsa reference counted object type, always passed by reference */
typedef struct
{
	int magic; // 2007
    int refCount;
    void *data;
    BalsaDestructor destructor;
}
BalsaObject;

#define BALSA_OBJECT(ptr) ((BalsaObject *)(ptr))

/* Width of a BalsaObject when passed on a channel/into a Balsa variable */
#define BALSA_OBJECT_BITS (64)

/* Note that where a BalsaObject is passed around, it should *never* be NULL,
	but object->data can be NULL to indicate an uninitialised object */

/* NewBalsaObject : create a BalsaObject with a 0 reference count */
extern BalsaObject *NewBalsaObject (void *data, BalsaDestructor destructor);

/* SetBalsaObject : like NewBalsaObject but the BalsaObject structure is
	given and already allocated */
extern void SetBalsaObject (BalsaObject * object, void *data, BalsaDestructor destructor);

/* BalsaObjectRef : increment the given object's reference count, please
	always use this function to do this as this will help debugging.  Returns
	the object (for convenience) */
extern BalsaObject *BalsaObjectRef (BalsaObject * object);

/* BalsaObjectUnref : decrement an object's reference count and
	conditionally delete the object and it's data.  Returns false if the
	object was actually deleted and true if it wasn't */
extern bool BalsaObjectUnref (BalsaObject * object);

/* AssertBalsaObject : complain if the passed object and its data is NULL
	returns argument object for convenience */
extern BalsaObject *AssertBalsaObject (BalsaObject * object);

/* FormatData{Get,Set}BalsaObject : get/set a pointer to a BalsaObject from a FormatData at the given
	bit offset */
extern BalsaObject *FormatDataGetBalsaObject (FormatData * data, unsigned offset);
extern void FormatDataSetBalsaObject (FormatData * data, BalsaObject * object, unsigned offset);

#endif
