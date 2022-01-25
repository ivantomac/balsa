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

	`file.c'
	Project file details structures

*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "file.h"
#include "project.h"
#include "paths.h"
#include "utils.h"

/* NewBalsaFile : make a new project file unconnected to a real file */
PtrBalsaFile NewBalsaFile (void)
{
    PtrBalsaFile file = BALSA_FILE (g_malloc (sizeof (BalsaFile)));

    file->name = "";
    file->nature = BalsaFile_File;
    file->info.balsaFile.testFixtures = NULL;
    file->info.balsaFile.implementations = NULL;
    file->info.balsaFile.builtinLib = NULL;
    return file;
}

PtrBalsaFile NewBalsaFileWithName (const char *filename)
{
    PtrBalsaFile file = NewBalsaFile ();

    if (filename[0] == '/')
        file->name = GetWithoutPath (filename);
    else
        file->name = g_strdup (filename);

    if ((strlen (file->name) > 6) && !strcmp (file->name + strlen (file->name) - 6, ".balsa"))
    {
        file->name = (char *) GetWithoutExtension (file->name);
        return file;
    } else
    {
        printfConsole ("Not a .balsa file\n");
        //free(file) and properties.
        return NULL;
    }
}

/* NewImplementation : make a new test impl */
PtrImplementation NewImplementation (void)
{
    PtrImplementation impl = IMPLEMENTATION (g_new0 (Implementation, 1));

    impl->name = "";
    impl->balsaNetlistOptions = "";
    impl->makeImplTestOptions = "";
    impl->technology = "";
    impl->matchingBalsaFile = 0;
    impl->matchingBalsaTest = 0;
    return impl;
}

PtrImplementation NewImplementationWithName (const char *name)
{
    PtrImplementation impl = NewImplementation ();

    impl->name = g_strdup (name);
    return impl;
}

/* NewBuiltinLib : make a new test builtin lib */
PtrBuiltinLib NewBuiltinLib (void)
{
    PtrBuiltinLib lib = BUILTINLIB (g_malloc (sizeof (BuiltinLib)));

    lib->name = "";
    lib->compileOptions = "";
    lib->sourceFilenames = NULL;
    lib->matchingBalsaFile = 0;
    return lib;
}

PtrBuiltinLib NewBuiltinLibWithName (const char *name)
{
    PtrBuiltinLib lib = NewBuiltinLib ();

    lib->name = g_strdup (name);
    return lib;
}

PtrDefine NewDefine (char *name, char *value)
{
    PtrDefine def = (PtrDefine) (g_malloc (sizeof (Define)));

    def->name = g_strdup (name);
    def->value = g_strdup (value);
    return def;
}

/* NewBalsaPort : make a new port */
PtrBalsaPort2 NewBalsaPort2 (BalsaPort2Nature nature, char *name)
{
    PtrBalsaPort2 port2 = g_new0 (BalsaPort2, 1);

    port2->nature = nature;
    port2->name = g_strdup (name);
    return port2;
}

PtrBalsaTestComponent NewBalsaTestComponent (BalsaTestComponentNature nature)
{
    PtrBalsaTestComponent testComponent = g_new0 (BalsaTestComponent, 1);

    testComponent->nature = nature;
    return testComponent;
}

/* NewBalsaTest : make a new test fixture with no ports */
PtrBalsaTest NewBalsaTest (const char *name)
{
    PtrBalsaTest test = BALSA_TEST (g_malloc (sizeof (BalsaTest)));

    test->name = g_strdup (name);
    test->procName = NULL;
    test->testComponents = NULL;
    test->implementations = NULL;
    test->matchingBalsaFile = NULL;
    test->commandLineOptions = NULL;
    return test;
}

/* Delete... : destructors */
void DeleteBalsaTestComponent (PtrBalsaTestComponent testComponent)
{
    if (testComponent)
    {
        //        if (testComponent->name)
        //            g_free (testComponent->name);
        if (testComponent->value)
            g_free (testComponent->value);
        g_free (testComponent);
    }
}

void DeleteImplementation (PtrImplementation impl)
{
    if (impl)
    {
        if (impl->name)
            g_free (impl->name);
    }
}

void DeleteBuiltinLib (PtrBuiltinLib lib)
{
    if (lib)
    {
        if (lib->name)
            g_free (lib->name);
    }
}

void DeleteBalsaTest (PtrBalsaTest test)
{
    if (test)
    {
        if (test->name)
            g_free (test->name);
        if (test->procName)
            g_free (test->procName);
        g_list_foreach (test->testComponents, (GFunc) DeleteBalsaTestComponent, NULL);
        g_list_free (test->testComponents);
        g_list_foreach (test->implementations, (GFunc) DeleteImplementation, NULL);
        g_list_free (test->implementations);
    }
}

void DeleteBalsaFile (PtrBalsaFile file)
{
    if (file)
    {
        if (file->name)
            g_free (file->name);
        if (file->nature == BalsaFile_File)
        {
            g_list_foreach (file->info.balsaFile.testFixtures, (GFunc) DeleteBalsaTest, NULL);
            g_list_free (file->info.balsaFile.testFixtures);

            g_list_foreach (file->info.balsaFile.implementations, (GFunc) DeleteImplementation, NULL);

            if (file->info.balsaFile.builtinLib)
                DeleteBuiltinLib (file->info.balsaFile.builtinLib);

            g_list_free (file->info.balsaFile.implementations);
        }
    }
}
