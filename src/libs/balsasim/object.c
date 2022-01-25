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

	`object.c'
	Balsa object representation

*/

#include "object.h"
#include <stdlib.h>

int BalsaDebug = 0;

/* Note that where a BalsaObject is passed around, it should *never* be NULL,
	but object->data can be NULL to indicate an uninitialised object */

int BalsaObjectCount = 0;

const int BalsaMagic = 0x12345678;

/* NewBalsaObject : create a BalsaObject with a 0 reference count */
BalsaObject *NewBalsaObject (void *data, BalsaDestructor destructor)
{
    BalsaObject *ret = malloc (sizeof (BalsaObject));

    ret->magic = BalsaMagic;

    SetBalsaObject (ret, data, destructor);
    if (BalsaDebug)
        BalsaObjectCount++;

    if (BalsaDebug)
    {
        fprintf (stderr, "NewBalsaObject: %p\n", ret);
    }

    return ret;
}

/* SetBalsaObject : like NewBalsaObject but the BalsaObject structure is
	given and already allocated */
void SetBalsaObject (BalsaObject * object, void *data, BalsaDestructor destructor)
{
    object->data = data;
    object->refCount = 1;
    object->destructor = destructor;

    if (BalsaDebug)
    {
        fprintf (stderr, "SetBalsaObject: %p %p %p\n", object, data, destructor);
    }
}

/* BalsaObjectRef : increment the given object's reference count, please
	always use this function to do this as this will help debugging.  Returns
	the object (for convenience) */
BalsaObject *BalsaObjectRef (BalsaObject * object)
{
    object->refCount++;

    if (BalsaDebug)
    {
        fprintf (stderr, "BalsaObjectRef: %p refCount: %d\n", object, object->refCount);
    }

    return object;
}

/* BalsaObjectUnref : decrement an object's reference count and
	conditionally delete the object and it's data.  Returns false if the
	object was actually deleted and true if it wasn't */
bool BalsaObjectUnref (BalsaObject * object)
{
    object->refCount--;

    if (BalsaDebug)
    {
        fprintf (stderr, "BalsaObjectUnref: %p refCount: %d %s\n", object, object->refCount, (object->refCount <= 0 ? "*deleted*" : ""));
    }

    if (object->refCount <= 0)
    {
        if (object->destructor)
        {
            if (BalsaDebug)
            {
                BalsaObjectCount--;
                fprintf (stderr, "BalsaObjectUnref: %p *calling destructor*, %d left\n", object, BalsaObjectCount);
            }
            object->destructor (object->data);
        } else
            free (object->data);

        free (object);
        object = NULL;
    }

    return object;
}

/* AssertBalsaObject : complain if the passed object and its data is NULL
	returns argument object for convenience */
BalsaObject *AssertBalsaObject (BalsaObject * object)
{
    if (!object || !object->data || object->magic != BalsaMagic)
    {
        fprintf (stderr, __FILE__ ":%d: FATAL: uninitialised BalsaObject %p%s\n", __LINE__,
        	object, (object->magic != BalsaMagic ? " (bad magic)" : " (bad pointer)"));
        // exit (EXIT_FAILURE);
        *((char *) 0) = 0;
    }
    return object;
}

/* FormatData{Get,Set}BalsaObject : get/set a pointer to a BalsaObject from a FormatData at the given
	bit offset */
BalsaObject *FormatDataGetBalsaObject (FormatData * data, unsigned offset)
{
    BalsaObject *ret = AssertBalsaObject (FormatDataExtractPointer (data, offset, BALSA_OBJECT_BITS));

    return ret;
}

void FormatDataSetBalsaObject (FormatData * data, BalsaObject * object, unsigned offset)
{
    AssertBalsaObject (object);
    FormatDataInsertPointer (data, object, offset, BALSA_OBJECT_BITS);
}
