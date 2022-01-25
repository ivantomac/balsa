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
;;; four_e_e style startup
;;;

(set! tech-sync-channel-portions
	`((req push ,width-one) (ack pull ,width-one)))

(set! tech-push-channel-portions 
	`((req push ,width-one) (ack pull ,width-one) (data push ,width-n)))

(set! tech-pull-channel-portions 
	`((req push ,width-one) (ack pull ,width-one) (data pull ,width-n)))

(for-each (lambda (component)
   	(if (list? component) ; (name from-name from-style)
		(brz-add-primitive-part-implementation (car component)
			`(style "four_e_e" (include style ,(caddr component) ,(cadr component)))
		)
		(brz-add-primitive-part-implementation component
			`(style "four_e_e" (include style "four_e_e" ,component))
		)
    )
) '(
    ("ActiveEagerFalseVariable" "FalseVariable" "four_e_e")
    ("ActiveEagerNullAdapt" "NullAdapt" "four_e_e")
    ("Adapt" "Adapt" "four_b_rb")
    "Arbiter"
    "Bar"
    ("BinaryFunc" "BinaryFunc" "four_b_rb")
    ("BinaryFuncConstR" "BinaryFuncConstR" "four_b_rb")
    ("BinaryFuncConstRPush" "BinaryFuncConstR" "four_b_rb")
    ("BinaryFuncPush" "BinaryFunc" "four_b_rb")
    "Call"
    ("CallActive" "CallActive" "four_b_rb")
    "CallDemux"
    "CallDemuxPush"
    "CallMux"
    "Case"
    "CaseFetch"
    ("Combine" "Combine" "four_b_rb")
    ("CombineEqual" "CombineEqual" "four_b_rb")
   	"Concur"
    ("Constant" "Constant" "four_b_rb")
    ("Continue" "Continue" "four_b_rb")
    ("ContinuePush" "ContinuePush" "four_b_rb")
    "DecisionWait"
    "Encode"
    "FalseVariable"
    "Fetch"
    ("Fork" "Fork" "four_b_rb")
    ("ForkPush" "ForkPush" "four_b_rb")
    ("Halt" "Halt" "four_b_rb")
    ("HaltPush" "HaltPush" "four_b_rb")
    ("Loop" "Loop" "four_b_rb")
    "NullAdapt"
    ("Passivator" "Passivator" "four_b_rb")
    "PassivatorPush"
    ("PassiveEagerFalseVariable" "FalseVariable" "four_e_e")
    ("PassiveEagerNullAdapt" "NullAdapt" "four_e_e")
    ("PassiveSyncEagerFalseVariable" "FalseVariable" "four_e_e")
    "Sequence"
    ("Slice" "Slice" "four_b_rb")
    ("Split" "Split" "four_b_rb")
    ("SplitEqual" "SplitEqual" "four_b_rb")
    ("Synch" "Synch" "four_b_rb")
    ("SynchPull" "SynchPull" "four_b_rb")
    ("SynchPush" "SynchPush" "four_b_rb")
    ("UnaryFunc" "UnaryFunc" "four_b_rb")
    ("UnaryFuncPush" "UnaryFunc" "four_b_rb")
    "Variable"
    "While"
    ("WireFork" "WireFork" "four_b_rb")
    "BuiltinVariable"
))
