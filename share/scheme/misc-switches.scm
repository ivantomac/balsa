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
;;;	`misc-switches.scm'
;;;	Command line switches handling
;;;

;;; switch-rule:{short,long,argument-count,action}: field extractors
(define switch-rule:short-name car)
(define switch-rule:long-name cadr)
(define switch-rule:argument-count caddr)
(define switch-rule:action cadddr)

;;; find-short-switch-rule: find the rule for the -@ switch corresponding to @
(define find-short-switch-rule (lambda (rules rule-char)
	(find-with-predicate rules (lambda (rule char)
		(eqv? (switch-rule:short-name rule) char)) rule-char)
))

;;; find-long-switch-rule: find the rule for the --@@@@@ switch corresponding to @@@@@
(define find-long-switch-rule (lambda (rules rule-str)
	(find-with-predicate rules (lambda (rule str)
		(string=? (switch-rule:long-name rule) str)) rule-str)
))

;;; parse-command-line: parse switches from the given command line list using the rules given.
;;;		This procedure returns the remainder of the argument list.
;;;		The rules list should have elements of the form (switch-char long-name argument-count handling-procedure)
;;;		For example (-o or --output-filename <name>) '(#\o "output-filename" 1 (lambda (args) ...)).  `usage'
;;;		is called if the switch rules are violated.
;;;		The action procedure is called with the complete argument list (w/o the switch).
(define parse-command-line (lambda (program-name rules usage args)
	(if (or (null? args) (not (eqv? #\- (string-ref (car args) 0))))
		args
		(let*
			((require-n-args (lambda (required-switch-count)
				(if (< (length (cdr args)) required-switch-count)
					(error program-name ": expecting at least " (number->string required-switch-count)
						" switch arguments here" #\newline)
				)
			 ))
			 (switch (car args))
			 (switch-length (string-length switch))
			 (execute-rule (lambda (rule args)
				(begin
					(require-n-args (switch-rule:argument-count rule)) ; check arg count
					((switch-rule:action rule) args) ; execute the action
					(parse-command-line program-name rules usage (list-tail args
						(switch-rule:argument-count rule)))
				)
			 ))
			)
			(cond
				((= 1 switch-length) (usage)) ; no switch
				; long switch format --...
				((and (< 2 switch-length) (eqv? #\- (string-ref switch 1)))
					(let*
						((switch (substring switch 2 switch-length))
						 (switch-rule (find-long-switch-rule rules switch))
						)
						(if (not switch-rule)
							(usage)
							(execute-rule switch-rule (cdr args))
						)
					)
				)
				((eqv? #\- (string-ref switch 1)) (cdr args)) ; last arg
				(else ; short form, if characters remain make them part of the next arg if argument-count
					  ; /= 0 (making -labc equivalent to -l abc) or a second short switch if argument-count
					  ; = 0 (making -la equivalent to -l -a)
					(let ((switch-rule (find-short-switch-rule rules (string-ref switch 1))))
						(if (not switch-rule)
							(usage)
							(execute-rule switch-rule
								(if (= 2 switch-length)
									(cdr args) ; -@
									(cons ; create a new arg/switch
										(let ((rest-of-switch (substring switch 2 switch-length)))
											(if (zero? (switch-rule:argument-count switch-rule))
												(string-append "-" rest-of-switch) ; -laa => -l -aa
												rest-of-switch ; -laa => -l aa
											)	
										)
										(cdr args)
									)
								)
							)
						)
					)
				)
			)
		)
	)
))
