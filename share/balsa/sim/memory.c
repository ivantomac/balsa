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

	`memory.c'
	Balsa memory model

*/

#include "balsasim/builtin.h"
#include "balsasim/object.h"
#include "balsasim/bstring.h"
#include <stdlib.h>
#include <string.h>

#define PAGE_ADR_BITS 8

typedef struct Memory_Page_
{
    struct Memory_Page_ *next;
    unsigned *address;
    unsigned *data;
}
Memory_Page;

typedef struct Memory_Descriptor_
{
    bool initialised;
    unsigned pageSize;
    unsigned adrMask;
    Memory_Page *data;
    Memory_Page *MRU;
}
Memory_Descriptor;

static void Memory_Init (Memory_Descriptor * desc)
{
    desc->initialised = true;
    desc->pageSize = (1 << PAGE_ADR_BITS);
    desc->adrMask = (desc->pageSize - 1);
    desc->data = NULL;
    desc->MRU = NULL;
}

static void Memory_Free (Memory_Descriptor * desc)
{
    if (!desc)
        return;

    while (desc->data)
    {
        Memory_Page *page = desc->data;

        desc->data = page->next;
        free (page->data);
        free (page->address);
        free (page);
    }
    free (desc);
}

static bool Memory_AddressInPage (Memory_Descriptor * desc, FormatData * adr)
{
    if (!desc->MRU)
    {
        return false;
    } else if ((adr->words[0] | desc->adrMask) != desc->MRU->address[0])
    {
        return false;
    } else if ((adr->wordCount > 1) && (memcmp (&(adr->words[1]), &(desc->MRU->address[1]), ((adr->wordCount - 1) * sizeof (unsigned)))))
    {
        return false;
    } else
    {
        return true;
    }
}

static bool Memory_PageFind (Memory_Descriptor * desc, FormatData * adr)
{
    if (Memory_AddressInPage (desc, adr))
        return true;
    else
    {
        Memory_Page *old_page = desc->MRU;

        desc->MRU = desc->data;
        while (desc->MRU)
            if (Memory_AddressInPage (desc, adr))
                return true;
            else
                desc->MRU = desc->MRU->next;

        desc->MRU = old_page;
        return false;
    }
}

static void Memory_MakeNewPage (Memory_Descriptor * desc, FormatData * adr, FormatData * data)
{
    desc->MRU = malloc (sizeof (Memory_Page));
    desc->MRU->next = desc->data;
    desc->data = desc->MRU;

    desc->MRU->data = malloc (data->wordCount * sizeof (unsigned) * desc->pageSize);
    desc->MRU->address = malloc (adr->wordCount * sizeof (unsigned));

    desc->MRU->address[0] = adr->words[0] | desc->adrMask;
    if (adr->wordCount > 1)
        memcpy (&(desc->MRU->address[1]), &(adr->words[1]), ((adr->wordCount - 1) * sizeof (unsigned)));

    memset (desc->MRU->data, 0, (data->wordCount * sizeof (unsigned) * desc->pageSize));
}

/* BalsaMemoryNew is builtin : BalsaMemory */
static void Memory_BalsaMemoryNew (BuiltinFunction * function, BuiltinFunctionInstanceData * instance)
{
    Memory_Descriptor *desc = malloc (sizeof (Memory_Descriptor));

    desc->initialised = false;
    SetBalsaObject (instance->objects[0], desc, (BalsaDestructor) Memory_Free);
    FormatDataSetBalsaObject (instance->result, instance->objects[0], 0);
}

/* BalsaMemory{Read,Write} : simple access functions
	function BalsaMemoryRead (parameter params : BalsaMemoryParams;
		memory : BalsaMemory; address : params.addressWidth bits) is builtin : params.dataWidth bits */
static void Memory_BalsaMemoryRead (BuiltinFunction * function, BuiltinFunctionInstanceData * instance)
{
    if (!instance->portWidthsAreResolved)
    {
        FormatData *addrWidth = FormatDataExtractBitField (instance->parameters[0]->info.number.number, 0, 32);
        FormatData *dataWidth = FormatDataExtractBitField (instance->parameters[0]->info.number.number, 32, 32);

        instance->resultWidth = dataWidth->words[0];
        instance->argumentWidths[1] = addrWidth->words[0];
        DeleteFormatData (addrWidth);
        DeleteFormatData (dataWidth);
    } else
    {
        BalsaObject *descObj = FormatDataGetBalsaObject (instance->arguments[0], 0);
        Memory_Descriptor *desc = descObj->data;
        FormatData *adrFd = instance->arguments[1];
        FormatData *dataFd = instance->result;

        if (!desc->initialised)
            Memory_Init (desc);

        if (Memory_PageFind (desc, adrFd))
            memcpy (dataFd->words,
              &(desc->MRU->data[(adrFd->words[0] & desc->adrMask) * dataFd->wordCount]), (dataFd->wordCount * sizeof (unsigned)));
        else
            FormatDataSetWords (dataFd, 0);
    }
}

/* BalsaMemory{Read,Write} : simple access functions
	function BalsaMemoryWrite (parameter params : BalsaMemoryParams;
		memory : BalsaMemory; address : params.addressWidth bits; data : params.dataWidth bits) is builtin : BalsaMemory */
static void Memory_BalsaMemoryWrite (BuiltinFunction * function, BuiltinFunctionInstanceData * instance)
{
    if (!instance->portWidthsAreResolved)
    {
        FormatData *addrWidth = FormatDataExtractBitField (instance->parameters[0]->info.number.number, 0, 32);
        FormatData *dataWidth = FormatDataExtractBitField (instance->parameters[0]->info.number.number, 32, 32);

        instance->argumentWidths[1] = addrWidth->words[0];
        instance->argumentWidths[2] = dataWidth->words[0];

        DeleteFormatData (addrWidth);
        DeleteFormatData (dataWidth);
    } else
    {
        BalsaObject *descObj = FormatDataGetBalsaObject (instance->arguments[0], 0);
        Memory_Descriptor *desc = descObj->data;
        FormatData *adrFd = instance->arguments[1];
        FormatData *dataFd = instance->arguments[2];

        if (!desc->initialised)
            Memory_Init (desc);

        if (!Memory_PageFind (desc, adrFd))
            Memory_MakeNewPage (desc, adrFd, dataFd);

        memcpy (&(desc->MRU->data[(adrFd->words[0] & desc->adrMask) * dataFd->wordCount]), dataFd->words, (dataFd->wordCount * sizeof (unsigned)));

        SetBalsaObject (descObj, desc, (BalsaDestructor) Memory_Free);
        FormatDataSetBalsaObject (instance->result, descObj, 0);
    }
}

BALSA_SIM_REGISTER_BUILTIN_LIB (memory)
{
    BalsaSim_RegisterBuiltinFunction ("BalsaMemoryNew", 0, 0, Memory_BalsaMemoryNew, 64, NULL, 1);
    BalsaSim_RegisterBuiltinFunction ("BalsaMemoryRead", 1, 2, Memory_BalsaMemoryRead, 0, (unsigned[])
      {
      64, 0}
      , 0);
    BalsaSim_RegisterBuiltinFunction ("BalsaMemoryWrite", 1, 3, Memory_BalsaMemoryWrite, 64, (unsigned[])
      {
      64, 0, 0}
      , 0);
}
