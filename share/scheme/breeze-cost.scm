;;;
;;;	The Balsa Asynchronous Hardware Synthesis System
;;;	Copyright (C) 1995-2008 School of Computer Science
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
;;;	`breezecost.scm'
;;;	Breeze file cost estimator
;;;

(balsa-scheme-import 'brz)
(balsa-scheme-import 'misc 'switches)
(balsa-scheme-import 'misc 'banners)

;;; breeze-cost-{files,...}: command line args
(define breeze-cost-files '())
(define breeze-cost-no-banner #f)
(define breeze-cost-printed-banner #f)
(define breeze-cost-multiplier 0)
(define breeze-cost-unit #f)
(define breeze-cost-threshold 0)

(define breeze-cost-print-banner (lambda ()
	(if (not breeze-cost-printed-banner)
		(make-program-banner "breeze-cost" "breeze-cost: Breeze cost estimation"
			"1998-2008, The University of Manchester")
	)
	(set! breeze-cost-printed-banner #t)
))

;;; breeze-cost-usage: command usage
(define breeze-cost-usage (lambda ()
	(breeze-cost-print-banner)
	(error
		"version " balsa-version #\newline
		"usage: breeze-cost {<switch>}* <breeze-file> ..." #\newline #\newline
		"switches: -b                 - Don't print the breeze-cost banner (--no-banner)" #\newline
		"          -t <threshold>     - Cost threshold for printing components (--threshold)" #\newline
		"          -m <multiplier>    - Multiplier to get 'real' component costs (--multiplier)" #\newline
		"          -u <unit>          - Unit name of 'real' cost (--unit)" #\newline
	)
))

;;; breeze-cost-command-line-rules: command-line-args action rules
(define breeze-cost-command-line-rules `(
	(#\t "threshold" 1 ,(lambda (args) (set! breeze-cost-threshold (string->number (car args)))))
	(#\m "multiplier" 1 ,(lambda (args) (set! breeze-cost-multiplier (string->number (car args)))))
	(#\u "unit" 1 ,(lambda (args) (set! breeze-cost-unit (car args))))
	(#\b "no-banner" 0 ,(lambda (args) (set! breeze-cost-no-banner #t)))
	(#\h "help" 0 ,(lambda (args) (breeze-cost-usage)))
	(#\? "help" 0 ,(lambda (args) (breeze-cost-usage)))
))

;;; breeze-cost-parse-command-line: parse switches from the given command line list, set
;;;		the breeze-cost-... globals and return the tail of the command line.
(define breeze-cost-parse-command-line (lambda (args)
	(if (null? args)
		(breeze-cost-usage)
		(begin
			(set! breeze-cost-files (parse-command-line "breeze-cost" breeze-cost-command-line-rules
				breeze-cost-usage args))
			(if (null? breeze-cost-files)
				(breeze-cost-usage)
			)
		)
	)
))

;;; cost: Gate cost function
(define cost (lambda (gate-name)
	(case gate-name
		((AO21) 20)
		((AOI21) 17.25)
		((AOI22) 19.75)
		((OA21) 21.25)
		((OAI21) 18.75)
		((OAI22) 22.5)
		((and2) 15.5)
		((and3) 21.75)
		((and4) 25.5)
		((demux12) 29.25)
		((inv1) 9)
		((inv2) 9)
		((inv3) 9)
		((inv4) 9)
		((inv5) 9.5)
		((inv6) 17)
		((inv7) 17)
		((inv8) 17)
		((majoirty) 24.25)
		((mux21) 25)
		((nand2) 12)
		((nand3) 17.5)
		((nand4) 21)
		((nor2) 11.75)
		((nor3) 17.5)
		((nor4) 20)
		((or2) 16)
		((or3) 21.5)
		((or4) 24.75)
		((tri_inv) 14.5)
		((xor2) 20.25)
		((xor_dp) 20.25)
		((xnor2) 23.5)
		((DFF) 37)
		((lp_mullc2) 22.25)
		((lp_mullc2R) 28)
		((lp_asy_mullc_bbn) 24)
		((lp_asy_mullc_bnn) 24)
		((lp_asy_mullc_bnnn) 27.5)
		((lp_asy_mullc_nb) 21.375)
		((lp_asy_mullc_nppbR) 28)
		((lp_asy_mullc_npbR) 28)
		((lp_asy_mullc_pb) 21)
		((lp_asy_mullc_ppbR) 30)
		((neDFF) 51.875)
		((neDFFR) 57)
		((peDFF) 51.875)
		((peDFFR) 57)
		((srff_H) 22)
		((srff_L) 22)
		((static_asy_mullc_nb) 20.25)
		((static_asy_mullc_pb) 20)
		((svensson_n) 24.75)
		((t_latch_comp) 25)
		((t_latch_compR) 28.75)
		((t_latchR) 33.25)
		((tri_latch) 24.75)
		((tri_latchR) 27.75)
	
		((mutex) 30) ; ?
	)
))

;;; cost-fixed: useful for parts
(define cost-fixed (lambda (cost)
	(lambda args cost)
))

(define zero-cost (cost-fixed 0))
(define unknown-cost zero-cost)
(define cost-s-element (+ (cost 'lp_mullc2) (cost 'nor2) (cost 'and2)))
(define cost-call2 (+ (cost 'or2) (* 2 (cost 'lp_mullc2))))
(define cost-n-way-call (lambda (n) (* (- n 1) cost-call2)))
(define cost-n-way-mux (lambda (n) (* (- n 1) (+ (cost 'inv1) (cost 'mux21)))))
(define cost-loop (+ (cost 'inv1) (cost 'nor2)))
(define cost-while (+ cost-s-element cost-loop (cost 'demux12)))
(define cost-while-else (+ cost-while (cost 'or2)))

;;; cost-and/cost-or: Cost of simple and/or (completion) trees, NB. no nand/nor
;;;		optimisation yet.
(define cost-and-modulo (lambda (count)
	(case count
		((0 1) 0)
		((2) (cost 'and2))
		((3) (cost 'and3))
	)
))

(define cost-or-modulo (lambda (count)
	(case count
		((0 1) 0)
		((2) (cost 'or2))
		((3) (cost 'or3))
	)
))


(define cost-or (lambda (count)
	(+ (cost-or-modulo (modulo count 3))
		(* (/ count 3) (cost 'or3))
	)
))

(define cost-and (lambda (count)
	(+ (cost-and-modulo (modulo count 3))
		(* (/ count 3) (cost 'and3))
	)
))

(define cost-unary-func (lambda (outputWidth inputWidth op inputIsSigned)
	(cond 
		((string=? op "Negate") 0 ) ; FIXME
		((string=? op "Invert") (* (max outputWidth inputWidth) (cost 'inv1)))
	)
))

(define cost-binary-func (lambda (outputWidth inputAWidth inputBWidth
	op outputIsSigned inputAIsSigned inputBIsSigned)
	(let*
		((minimumWidth (min outputWidth inputAWidth inputBWidth))
		 (maximumWidth (min outputWidth (max inputAWidth inputBWidth)))
		 ; cost-full-adder: cost of st_adder
		 (cost-full-adder (+ (* 2 (cost 'mux21)) (* 2 (cost 'xor2)) (* 2 (cost 'inv1)) (cost 'nor2)))
		 ; cost-half-adder: cost of fouraddsgnext
		 (cost-half-adder (+ (* 2 (cost 'xor2)) (* 2 (cost 'and3)) (cost 'inv1)))
		 (cost-adder (lambda () 
			(+
				(* cost-full-adder minimumWidth)
				; FIXME, optimise for lower cost if not sign extending one input?
				(* cost-half-adder (- maximumWidth minimumWidth))
				; FIXME, output sign extension is free
				(cost-and maximumWidth)
			)
		 ))
		)
		(cond
			((string=? op "Equals") (+ (cost 'inv1) (cost-and maximumWidth) (* maximumWidth (cost 'xor2))))
			((string=? op "NotEquals") (+ (cost-and maximumWidth) (* maximumWidth (cost 'xor2))))
			((string=? op "And") (* minimumWidth (cost 'and2)))
			((string=? op "Or") (* minimumWidth (cost 'or2)))
			((string=? op "Xor") (* minimumWidth (cost 'xor2)))
			((string=? op "Add") (cost-adder))
			((string=? op "Subtract") (cost-adder))
			((string=? op "ReverseSubtract") (cost-adder))
			((string=? op "LessThan") 0)
			((string=? op "GreaterThan") 0)
			((string=? op "LessOrEquals") 0)
			((string=? op "GreaterOrEquals") 0)
		)
	)
))

;;; Use a very simple cost estimate, assume all minterms are expanded
(define cost-case (lambda (inputWidth outputCount specification)
	(let
		((noOfMinterms (expt 2 inputWidth)))
		(+ (* inputWidth (cost 'demux12)) ; Single rail -> dual rail
			(cost-or noOfMinterms)
			(* noOfMinterms (cost-and inputWidth) )
		)
	)
))

(define cost-function-association `(
	("$BrzFetch" ,zero-cost)
	("$BrzSplit" ,zero-cost)
	("$BrzSplitEqual" ,zero-cost)
	("$BrzCombine" ,zero-cost)
	("$BrzCombineEqual" ,zero-cost)
	("$BrzConstant" ,zero-cost)

	("$BrzContinue" ,zero-cost)
	("$BrzContinuePush" ,zero-cost)
	("$BrzHalt" ,zero-cost)
	("$BrzHaltPush" ,zero-cost)

	("$BrzMask" ,zero-cost)
	("$BrzSlice" ,zero-cost)
	("$BrzAdapt" ,zero-cost)
	("$BrzNullAdapt" ,zero-cost)
	("$BrzFalseVariable" ,zero-cost)
	("$BrzPassiveEagerFalseVariable" ,zero-cost)
	("$BrzPassiveSyncEagerFalseVariable" ,zero-cost)
	("$BrzActiveEagerFalseVariable" ,zero-cost)
	("$BrzPassiveEagerNullAdapt" ,zero-cost)
	("$BrzActiveEagerNullAdapt" ,zero-cost)

	("$BrzWireFork" ,zero-cost)

	("$BrzBuiltinVariable" ,zero-cost)
	("$BrzString" ,zero-cost)

	("$BrzLoop" ,(lambda (p) cost-loop))
	("$BrzSequence" ,(lambda (p) (* (- (car p) 1) cost-s-element)))
	("$BrzSequencePull" ,(lambda (p) cost-s-element))
	("$BrzConcur" ,(lambda (p) (+ (* (car p) cost-s-element) (* (- (car p) 1) (cost 'lp_mullc2)))))
	("$BrzVariable" ,(lambda (p) (* (car p) (cost 'svensson_n))))

	("$BrzCall" ,(lambda (p) (cost-n-way-call (car p))))
	("$BrzCallActive" ,(lambda (p) (cost-n-way-call (car p))))
	("$BrzCallMux" ,(lambda (p) (+ (cost-n-way-call (cadr p)) (* (car p) (cost-n-way-mux (cadr p))))))
	("$BrzCallDemux" ,(lambda (p) (cost-n-way-call (cadr p))))
	("$BrzCallDemuxPush" ,(lambda (p) (cost-n-way-call (cadr p))))
	("$BrzSynch" ,(lambda (p) (* (- (car p) 1) (cost 'lp_mullc2))))

	("$BrzSynchPush" ,(lambda (p) (* (+ 1 (cadr p)) (cost 'lp_mullc2))))
	("$BrzSynchPull" ,(lambda (p) (* (cadr p) (cost 'lp_mullc2))))
	("$BrzFork" ,(lambda (p) (* (- (car p) 1) (cost 'lp_mullc2))))
	("$BrzForkPush" ,(lambda (p) (* (- (cadr p) 1) (cost 'lp_mullc2))))

	("$BrzUnaryFunc" ,(lambda (p) (apply cost-unary-func p)))
	("$BrzBinaryFuncConstR" ,(lambda (p) (apply cost-binary-func (take p 7))))
	("$BrzBinaryFunc" ,(lambda (p) (apply cost-binary-func p)))
	("$BrzBar" ,unknown-cost)

	("$BrzWhile" ,(lambda (p) cost-while))
	("$BrzDecisionWait" ,(lambda (p) (+ (* (car p) (cost 'lp_mullc2)) (* (- (car p) 1) (cost 'or2)))))
	("$BrzArbiter" ,(lambda (p) (+ (* 2 (+ (cost 'or2) (cost 'and2))) (cost 'mutex))))

	("$BrzCase" ,(lambda (p) (apply cost-case p)))
	("$BrzCaseFetch" ,(lambda (p) (+ (* (car p) (cost-n-way-mux (caddr p))) (apply cost-case (cdr p)))))
	("$BrzEncode" ,zero-cost) ; FIXME
	("$BrzPassivator" ,(lambda (p) (cost 'lp_mullc2)))
	("$BrzPassivatorPush" ,(lambda (p) (* (cadr p) (cost 'lp_mullc2))))
))

;;; component-cost: Returns the cost of the given component.  `part-costs' is an association
;;;		list containing the costs of previously handled parts.
(define component-cost (lambda (comp part-costs)
	(if (not (brz-check-component? comp))
		0
		(let*
			((name (brz-component:name comp))
			 (name-cost-func-pair
				(let ((hc (assoc name cost-function-association)))
					(if hc
						hc
						(assoc name part-costs)
					)
				)
			 )
			)
			(if name-cost-func-pair
				((cadr name-cost-func-pair) (brz-component:parameters comp))
				(let
					((part-cost (assoc name part-costs)))
					(if part-cost
						(cdr part-cost)
						(begin (print "not a component: " comp #\newline) 0)
					)
				)
			)
		)
	)
))

;;; component-list->cost-list: Returns a list of (cost component) elements for the given components
(define component-list->cost-list (lambda (decls part-costs)
	(map (lambda (decl)
		(list (component-cost decl part-costs) decl)
	) decls)
))

;;; part-cost: calculate (and display) the cost of a part.  Uses `part-costs' as costs for
;;;		previously defined parts and returns an augmented part-costs.
(define part-cost (lambda (part part-costs)
	(print "Part: " (brz-breeze-part:name part) #\newline)
	(let*
		((cost-list (component-list->cost-list (cdr (brz-breeze-part:components part)) part-costs))
		 (sorted-cost-list (merge-sort! (lambda (l r) (<= (car l) (car r))) (copy-list cost-list)))
		 (total-cost (apply + (map car cost-list)))
		)
		(for-each (lambda (x)
			(if (>= (car x) breeze-cost-threshold)
				(begin (write x) (newline))
			)
		) sorted-cost-list)
		(print #\newline "Total cost: " total-cost #\newline)
		(if (not (zero? breeze-cost-multiplier))
			(begin
				(print "Cost in scaled units: " (* breeze-cost-multiplier total-cost))
				(if breeze-cost-unit
					(print " " breeze-cost-unit)
				)
				(print #\newline)
			)
		)
		(print #\newline)
		;; return
		(cons (list (brz-breeze-part:name part) (cost-fixed total-cost)) part-costs)
	)
))

;;; breeze-cost: estimate cost of breeze file filename.
(define breeze-cost (lambda (filename)
	(let*
		((decls/imports/imported-decls/visited-blocks (brz-get-flattened-breeze-block filename #f))
		 (all-decls (append
			(caddr decls/imports/imported-decls/visited-blocks)
		 	(car decls/imports/imported-decls/visited-blocks)))
		 (parts (filter brz-check-breeze-part? all-decls))
		 (part-costs (fold (lambda (part part-costs) (part-cost part part-costs)) '() parts))
		)
		(print "Total costs:" #\newline)
		(for-each (lambda (cost-pair)
			(let ((cost ((cadr cost-pair))))
				(print (car cost-pair) " = " cost " ")
				(if (not (zero? breeze-cost-multiplier))
					(begin
						(print "(" (* breeze-cost-multiplier cost))
						(if breeze-cost-unit
							(print " " breeze-cost-unit)
						)
						(print ")")
					)
				)
				(print #\newline)
			)
		) part-costs)
	)
	(print #\newline)
))

(top-level (lambda (args)
	(breeze-cost-parse-command-line args)
	(if (not breeze-cost-no-banner)
		(breeze-cost-print-banner)
	)
	(for-each breeze-cost breeze-cost-files)
) (cdr command-line-args))
