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

*/

#ifndef COMPONENTS_4PHASE_H
#define COMPONENTS_4PHASE_H

#include <balsasim/builtin.h>

//extern int WriteDebugAndAnimFiles;
extern FILE *TraceFile;
extern gboolean TraceAllChans;

/****************************************************/

/*
-- requp validated:

# b BrzAdapt_requp
# b BrzBinaryFunc_requp
# b BrzBinaryFuncConstR_requp
# b BrzCall_requp
# b BrzCallMux_requp
# b BrzCallDemux_requp
# b BrzCase_requp
# b BrzCaseFetch_requp
# b BrzCombine_requp
# b BrzCombineEqual_requp
# b BrzConcur_requp
# b BrzConstant_requp
# b BrzDecisionWait_requp
# b BrzEncode_requp
# b BrzFalseVariable_requp
# b BrzFetch_requp
# b BrzLoop_requp
# b BrzPassivatorPush_requp
# b BrzPassivePullPort_requp
# b BrzPassivePushPort_requp
# b BrzSequence_requp
# b BrzSlice_requp
# b BrzSynch_requp
# b BrzUnaryFunc_requp
# b BrzVariable_requp

# b BrzContinuePush_requp
# b BrzForkPush_requp
# b BrzSynchPush_requp

# b BrzArbiter_requp
# b BrzBar_requp
# b BrzContinue_requp
# b BrzFork_requp
# b BrzNullAdapt_requp
# b BrzSplit_requp
# b BrzSplitEqual_requp
# b BrzWhile_requp
# b BrzWireFork_requp

-- to be validated:

b BrzHalt_requp
b BrzActiveSyncPort_requp
b BrzPassiveSyncPort_requp
b BrzActivePushPort_requp
b BrzActivePullPort_requp

b BrzHaltPush_requp
b BrzPassivator_requp
b BrzSynchPull_requp

-- Not implemented yet:

b BrzCallActive_requp
b BrzCallDemuxPush_requp
b BrzMask_requp
*/

enum BinaryFuncType
{ NotEquals, LessThan, GreaterThan, LessOrEquals, GreaterOrEquals, Equals,
    Add, Subtract, ReverseSubtract, And, Or,
    Xor
};
enum UnaryFuncType
{ Invert, Negate };

void BrzAdapt (int num, char *args[]);
void BrzArbiter (int num, char *args[]);
void BrzBar (int num, char *args[]);
void BrzBinaryFunc (int num, char *args[]);
void BrzBinaryFuncPush (int num, char *args[]);
void BrzBinaryFuncConstR (int num, char *args[]);
void BrzBinaryFuncConstRPush (int num, char *args[]);
void BrzCall (int num, char *args[]);

void BrzCallDemux (int num, char *args[]);
void BrzCallDemuxPush (int num, char *args[]);
void BrzCallActive (int num, char *args[]);
void BrzCallMux (int num, char *args[]);
void BrzCase (int num, char *args[]);
void BrzCaseFetch (int num, char *args[]);
void BrzCombine (int num, char *args[]);
void BrzCombineEqual (int num, char *args[]);
void BrzConcur (int num, char *args[]);
void BrzConstant (int num, char *args[]);
void BrzContinue (int num, char *args[]);
void BrzContinuePush (int num, char *args[]);
void BrzDecisionWait (int num, char *args[]);
void BrzEncode (int num, char *args[]);
void BrzFalseVariable (int num, char *args[]);
void BrzPassiveEagerFalseVariable (int num, char *args[]);
void BrzActiveEagerFalseVariable (int num, char *args[]);
void BrzPassiveSyncEagerFalseVariable (int num, char *args[]);
void BrzPassiveEagerNullAdapt (int num, char *args[]);
void BrzActiveEagerNullAdapt (int num, char *args[]);
void BrzFetch (int num, char *args[]);
void BrzFork (int num, char *args[]);
void BrzForkPush (int num, char *args[]);
void BrzHalt (int num, char *args[]);
void BrzHaltPush (int num, char *args[]);
void BrzLoop (int num, char *args[]);

void BrzNullAdapt (int num, char *args[]);
void BrzPassivator (int num, char *args[]);
void BrzPassivatorPush (int num, char *args[]);
void BrzSequence (int num, char *args[]);
void BrzSlice (int num, char *args[]);
void BrzSplit (int num, char *args[]);
void BrzSynch (int num, char *args[]);
void BrzSynchPull (int num, char *args[]);
void BrzSynchPush (int num, char *args[]);
void BrzSplitEqual (int num, char *args[]);
void BrzUnaryFunc (int num, char *args[]);
void BrzUnaryFuncPush (int num, char *args[]);
void BrzVariable (int num, char *args[]);
void BrzInitVariable (int num, char *args[]);
void BrzWhile (int num, char *args[]);

void BrzWireFork (int num, char *args[]);

// Debug:
void BrzBuiltinVariable (int num, char *args[]);

// For Sam:
void BrzFetchPush (int num, char *args[]);
void BrzFetchReject (int num, char *args[]);

// New ports:

void BrzMemoryComponent (int num, char *args[]);

void BrzPassiveSyncPort2File (int num, char *args[]);

#ifdef MPI
void BrzMPISync (int num, char *args[]);
void BrzMPIPush (int num, char *args[]);
void BrzMPIPull (int num, char *args[]);
#endif

//#ifdef HLA
void BrzHLA_init (int num, char *args[]);
void BrzHLA_active_sync (int num, char *args[]);
//#endif

void BrzDebug_BeginFunc (char *funcname);
void BrzDebug_EndFunc (void);

void SetChanWidth (int numChan, int width);

unsigned int my_atoll (char *str);
struct callback_;
void WriteAnimationInfo_signalCallback (struct callback_ *cb);
void WriteAnimationInfo_signalCallback_OutOfOrder (struct callback_ *cb, unsigned long long time);
void WriteAnimationInfo_time (unsigned long long time);

void CloseBreezeSimulation (int type); // type= 0:normal finish 1:deadlock 2:error

/* InstantiateBuiltinFunction : instantiate a builtin function from a BuiltinFunction
	structure and the usual num/args pair used with the other component instantiation functions.
	NB. Need to update this if the function style in components_4phase.[ch] changes */
extern void InstantiateBuiltinFunction (BuiltinFunction * func, int num, char *args[]);

#endif
