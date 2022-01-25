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

	`file.h'
	Project file details structures

*/

#ifndef FILE_HEADER
#define FILE_HEADER

#include <glib.h>
#include <gtk/gtk.h>

/* BalsaFileNature : what sort of file is this */
typedef enum BalsaFileNature
{
    BalsaFile_File
}
BalsaFileNature;

struct BuiltinLib;
struct BalsaTest;

typedef struct BalsaFile
{
    BalsaFileNature nature;
    char *name;                 /* complete path to the file */
    union
    {
        struct
        {
            GList *testFixtures; /* test fixtures for this file as a top level file */
            GList *implementations;
            struct BuiltinLib *builtinLib;
        }
        balsaFile;
    }
    info;
}
BalsaFile, *PtrBalsaFile;

typedef struct Implementation
{
    char *name;
    char *balsaNetlistOptions;
    char *makeImplTestOptions;
    char *technology;
    char *DumpFile;
    BalsaFile *matchingBalsaFile;
    struct BalsaTest *matchingBalsaTest;
}
Implementation, *PtrImplementation;

typedef struct BuiltinLib
{
    char *name;
    char *compileOptions;
    GList *sourceFilenames;
    BalsaFile *matchingBalsaFile;
}
BuiltinLib, *PtrBuiltinLib;

//#ifdef NOT_DEFINED
typedef enum BalsaPort2Nature
{
    BalsaPort2_Sync, BalsaPort2_Input, BalsaPort2_Output
}
BalsaPort2Nature, *PtrBalsaPort2Nature;

typedef struct BalsaPort2
{
    BalsaPort2Nature nature;
    char *name;
}
BalsaPort2, *PtrBalsaPort2;

//#endif

typedef enum BalsaTestComponentNature
{
    BalsaTestComponent_Sync, BalsaTestComponent_InputFromFile,
    BalsaTestComponent_InputFromValue,
    BalsaTestComponent_OutputToFile, BalsaTestComponent_OutputToStdout,
    BalsaTestComponent_Memory,
    BalsaTestComponent_Undefined
}
BalsaTestComponentNature, *PtrBalsaTestComponentNature;

typedef struct BalsaTestComponent
{
    BalsaTestComponentNature nature;
    GList *portNames;
    char *value;                /* filename bound to (if any) or value to use */
    //    GList *parameters;
}
BalsaTestComponent, *PtrBalsaTestComponent;

typedef struct BalsaTest
{
    char *name;                 /* name of test fixture */
    char *procName;             /* name of top level procedure */
    //    GList *ports;               /* list of ports, port->file/value mappings */
    GList *testComponents;      /* list of test components (in replacement to the list of ports) */
    GList *implementations;
    BalsaFile *matchingBalsaFile;
    char *commandLineOptions;
}
BalsaTest, *PtrBalsaTest;

typedef struct Define
{
    char *name;
    char *value;
}
Define, *PtrDefine;

struct BalsaProject;            /* forward decl. */

/* BALSA_... : Cast into ... (from gpointer) */
#define BALSA_TESTCOMPONENT(ptr) ((PtrBalsaTestComponent)(ptr))
#define BALSA_TEST(ptr) ((PtrBalsaTest)(ptr))
#define BALSA_FILE(ptr) ((PtrBalsaFile)(ptr))
#define IMPLEMENTATION(ptr) ((PtrImplementation)(ptr))
#define BUILTINLIB(ptr) ((PtrBuiltinLib)(ptr))

/* NewBalsaFile : make a new project file unconnected to a real file */
extern PtrBalsaFile NewBalsaFile (void);
extern PtrBalsaFile NewBalsaFileWithName (const char *filename);

/* NewImplementation : make a new implementation */
extern PtrImplementation NewImplementation (void);
extern PtrImplementation NewImplementationWithName (const char *name);

/* NewBuiltinLib : make a new builtin library structure */
extern PtrBuiltinLib NewBuiltinLib (void);
extern PtrBuiltinLib NewBuiltinLibWithName (const char *name);

/* NewBalsaPort : make a new port */
extern PtrBalsaPort2 NewBalsaPort2 (BalsaPort2Nature nature, char *name);

/* NewBalsaTestComponent : make a new testComponent */
extern PtrBalsaTestComponent NewBalsaTestComponent (BalsaTestComponentNature nature);

/* NewBalsaTest : make a new test fixture with no ports */
extern PtrBalsaTest NewBalsaTest (const char *name);

extern PtrDefine NewDefine (char *name, char *value);

/* Delete... : destructors */
extern void DeleteBalsaTestComponent (PtrBalsaTestComponent testComponent);
extern void DeleteBalsaTest (PtrBalsaTest test);
extern void DeleteBalsaFile (PtrBalsaFile file);

#endif
