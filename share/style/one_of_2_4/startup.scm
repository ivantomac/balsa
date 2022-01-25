;;;
;;;	The Balsa Asynchronous Hardware Synthesis System
;;;	Copyright (C) 1995-2003 Department of Computer Science
;;;	The University of Manchester, Oxford Road, Manchester, UK, M13 9PL
;;;	
;;;	This program is free software; you can redistribute it and/or modify
;;;	it under the terms of the GNU General Public License as published by
;;;	the Free Software Foundation; either version 2 of the License, or
;;;	(at your option) any later version.
;;;	
;;;	This program is distributed in the hope that it will be useful,
;;;	but WITHOUT ANY WARRANTY; without even the implied warranty of
;;;	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
;;;	GNU General Public License for more details.
;;;	
;;;	You should have received a copy of the GNU General Public License
;;;	along with this program; if not, write to the Free Software
;;;	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
;;;
;;;	`startup.scm'
;;; one_of_2_4 style startup
;;;

(set! tech-sync-channel-portions 
	`((req push ,width-one) (ack pull ,width-one)))

(set! tech-push-channel-portions 
	`((req0 push ,width-dualrail/1of4) (req1 push ,width-dualrail/1of4) 
		(req2 push ,width-1of4) (req3 push ,width-1of4) (ack pull ,width-one)))

(set! tech-pull-channel-portions 
	`((req push ,width-one) (ack0 pull ,width-dualrail/1of4)
		(ack1 pull ,width-dualrail/1of4) (ack2 pull ,width-1of4) (ack3 pull ,width-1of4)))

(for-each (lambda (component)
   	(if (list? component) ; (name from-name from-style)
		(brz-add-primitive-part-implementation (car component)
			`(style "one_of_2_4" (include style ,(caddr component) ,(cadr component)))
		)
		(brz-add-primitive-part-implementation component
			`(style "one_of_2_4" (include style "one_of_2_4" ,component))
		)
    )
) '(
    "ActiveEagerFalseVariable"
    "ActiveEagerNullAdapt"
    "Adapt"
    ("Arbiter" "Arbiter" "four_b_rb")
    ("Bar" "Bar" "dual_b")
    "BinaryFunc"
    "BinaryFuncConstR"
    ; "BinaryFuncConstRPush"
    ; "BinaryFuncPush"
    ("Call" "Call" "four_b_rb")
    ("CallActive" "CallActive" "four_b_rb")
    "CallDemux"
    "CallDemuxPush"
    "CallMux"
    "Case"
    "CaseFetch"
    "Combine"
    "CombineEqual"
    ("Concur" "Concur" "four_b_rb")
    "Constant"
    ("Continue" "Continue" "four_b_rb")
    "ContinuePush"
    ("DecisionWait" "DecisionWait" "four_b_rb")
    "Encode"
    "FalseVariable"
    "Fetch"
    ("Fork" "Fork" "four_b_rb")
    "ForkPush"
    ("Halt" "Halt" "four_b_rb")
    "HaltPush"
    ("Loop" "Loop" "four_b_rb")
    "NullAdapt"
    ("Passivator" "Passivator" "four_b_rb")
    "PassivatorPush"
    "PassiveEagerFalseVariable"
    "PassiveEagerNullAdapt"
    "PassiveSyncEagerFalseVariable"
    ("Sequence" "Sequence" "four_b_rb")
    "Slice"
    "Split"
    "SplitEqual"
    ("Synch" "Synch" "four_b_rb")
    "SynchPull"
    "SynchPush"
    "UnaryFunc"
    ; "UnaryFuncPush"
    "Variable"
    ("While" "While" "dual_b")
    ("WireFork" "WireFork" "four_b_rb")
    "BuiltinVariable"
))
