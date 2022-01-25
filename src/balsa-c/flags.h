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

	`flags.h'
	Global compilation flags and options
	
 */

#ifndef FLAGS_HEADER
#define FLAGS_HEADER

#include "misc.h"
#include "Errors.h"

/* Self explanatory flags */
extern bool DoGenerateDebugFile;
extern bool DoGenerateBreezeFile;
extern bool DoGenerateSBreezeFile;
extern bool DoGenerateBasicImports;
extern bool DoOptimise;
extern bool DoSemantics;
extern bool DoBanner;
extern bool DoAndrewsMagicSwitch;
extern bool DoLAPsMagicSwitch;
extern bool DoLinkBreeze;
extern bool UseWireForks;
extern bool AllowSequentialSelection;
extern bool ReportImports;
extern bool FlattenOutputFile;
extern bool SourceDecorateErrorMessages;
extern bool Verbose;
extern bool DontReadBuiltin;
extern bool DontGenerateEagerFVs;
extern bool DontOptimiseSequencers;

/* File/Directories */
extern char *DebugFileName;
extern char *ImportFileSearchPath;
extern char *IntermediatePath;

/* Tab size */
extern unsigned TabDistance;

/* Optimisation flags */
extern bool NwaySequenceOptimise;
extern bool NwayConcurOptimise;
extern bool FlattenProcedureCalls;

/* Compilation options */
extern bool VariableReadSplit;
extern bool NoEagerInputs;
extern bool WarnMulticast;

extern ErrorType DefaultPrintErrorType;

/* Usage : print command usage, use argv0 as the invoked name of balsa-c */
extern void Usage (char *argv0);

/* FindTabSpacing : search the .exrc file (if any) to find an appropriate tab
   stop directive */
extern void FindTabSpacing (void);

/* MakeCommandLineString : create a string which is returned and also held internally in a static
   variable which consists of the space separated list of command line arguments passed to balsa-c
   (including argv[0]) */
extern char *MakeCommandLineString (char **argv, int argc);

/* ProcessSwitches : strip off switches leaving *argv == input_file, argc == 1 */
extern void ProcessSwitches (char ***argv, int *argc, char *programName);

#endif /* FLAGS_HEADER */
