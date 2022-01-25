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
;;;	`gen-undeclared.scm'
;;;	`gen' stage of balsa-netlist back end - Undeclared component circuit-decl creation
;;;

(balsa-scheme-import 'net 'parser)
(balsa-scheme-import 'gen 'gates)

;;; gen-undeclared-is-builtin-function? : do the set of options passed imply that
;;;		the undeclared-component from which they come is a parameterised builtin function
(define gen-undeclared-is-builtin-function? (lambda (options)
	(let ((implements-option (find-headed-sub-list options 'implements)))
		(and implements-option (< 2 (length implements-option))
			(string=? (cadr implements-option) "is-builtin-function"))
	)
))

;;; gen-make-builtin-function-netlist: make a netlist for a (possibly parameterised)
;;;		builtin function.  `name' will be the mangled name of this function with the parameters
;;;		encoded in it.  `unparameterised-name' is the name of the function this parameterised
;;;		instance was based on.
(define gen-make-builtin-function-netlist (lambda (name unparameterised-name params ports type-context)
	(let*
		((ports (drop-head-symbol 'ports ports))
		 (input-ports (cdr ports))
		 (port-nets (gen-brz-ports->net-ports ports type-context #f))
		 (gate-map (lambda (gate) (gen-abs-gate->net-gate (gen-simple-gate->default-gate gate))))
		 (buffer (lambda (from to) (gate-map (list 'connect to from))))
		 (argument-names (map brz-port:name input-ports))
		 (param-count (length params))
		 (arg-count (length input-ports))
		 (bundle (lambda (portion name) (tech-bundle-name portion name 0)))
		 (stringify (lambda (str) (string-append "\"" str "\"")))
		 (activate-width (brz-type-width (brz-port:type (car ports)) type-context))
		 (escape-param (lambda (param)
		 	(stringify (verilog-escape-string
				(if (string? param)
					(stringify param)
					(->string param)
				)
			))
		 ))
		 (supported-styles `("dual_b" "one_of_2_4" ,tech-single-rail-styles))
		)
		(cond
			; FIXME, move this functionality into styles
			((string=? "sync" breeze-style)
				(list 'circuit name
					(cons 'ports
						(append
							port-nets
							(list (list "clock" 'input 1))
						)
					)
					(cons* 'nets
						(list (bundle 'ack "activate") 1 'assignable)
						(list (bundle 'data "activate") activate-width 'assignable)
						(if (/= arg-count 0)
							(list (list (bundle 'data "acks") arg-count 'assignable))
							'()
						)
					)
					(cons 'instances
						(append
							(map (lambda (name)
								(buffer (list 'req "activate" 0) (list 'req name 0))
							) argument-names)
							(if (/= 0 arg-count)
								(list (list 'assign (bundle 'data "acks") 0))
								'()
							)
							(list
								(list 'task-for-builtin-call
									(cons* 'call "BalsaBuiltin"
										(append!
											(cons*
												"ack_state"
												(stringify unparameterised-name) param-count
												(map escape-param params)
											)
											(map (lambda (port) (bundle 'data (brz-port:name port))) ports)
										)
									)
								)
								(list 'shdl
									(list 'assign (bundle 'data "activate") 0)
								)
								(list 'shdl
									(list 'assign (bundle 'ack "activate") 0)
								)
								(let
									((pre (list (list 'wait-until-rising "clock")))
									 (ack-check (map (lambda (port-num)
										(list 'check-if (list (bundle 'ack
											(brz-port:name (list-ref ports port-num))) "==" 1)
											(list
												(list 'assign
													;(string-append (bundle 'data "acks") "["
													;	(number->string (- port-num 1)) "]")
													; `(net-conns (slice ,(- port-num 1) 1 (data "acks")))
													`(net-conns ((data "acks" 0 ,(- port-num 1) 1)))
													1
												)
											)
											(list)
										)
									 ) (.. 1 arg-count)))
									 (trigger
										(let
											((condition (if (/= 0 arg-count)
											 (list (bundle 'data "acks") "===" (- (expt 2 arg-count) 1))
											 (list (bundle 'req "activate") "===" 1)
											)))
											(list
												(list 'check-if condition
													(cons*
														(list 'call-builtin-task 0)
														(list 'wait-for "`balsa_response_delay")
														(list 'assign (bundle 'ack "activate") 1)
														(if (/= 0 arg-count)
															(list (list 'assign (bundle 'data "acks") 0))
															'()
														)
													)
													(list
														(list 'wait-for "`balsa_response_delay")
														(list 'assign (bundle 'ack "activate") 0)
													)
												)
											)
										)
									 )
									 (post (list (list 'wait-until-falling "clock")))
									)
									(gate-map (cons 'shdl-repeat
										(append pre ack-check trigger post)
									))
								)
							)
						)
					)
					(list 'attributes
						(list 'global-ports "clock")
					)
				)
			)
			((brz-style-is-single-rail? breeze-style)
				(let
					((args-up (if (zero? arg-count)
						(bundle 'req "activate")
						(build-separated-string (map (lambda (port)
							(bundle 'ack (brz-port:name port))) input-ports) " & ")
					 ))
					 (args-down (if (zero? arg-count)
						(bundle 'req "activate")
						(build-separated-string (map (lambda (port)
							(bundle 'ack (brz-port:name port))) input-ports) " | ")
					 ))
					)
					(list 'circuit name
						(cons 'ports port-nets)
						(list 'nets
							(list (bundle 'ack "activate") 1 'assignable)
							(list (bundle 'data "activate") activate-width 'assignable)
						)
						(cons 'instances
							(append
								(map (lambda (name)
									(buffer (list 'req "activate" 0) (list 'req name 0))
								) argument-names)
								(list
									(list 'task-for-builtin-call
										(cons* 'call "BalsaBuiltin"
											(append!
												(cons*
													"ack_state"
													(stringify unparameterised-name) param-count
													(map escape-param params)
												)
												(map (lambda (port) (bundle 'data (brz-port:name port))) ports)
											)
										)
									)
									(list 'shdl
										(list 'assign (bundle 'data "activate") 0)
									)
									(list 'shdl-repeat
										(list 'assign (bundle 'ack "activate") 0)
										(list 'wait-until-rising args-up)
										(list 'wait-for "`balsa_sampling_delay")

										(list 'call-builtin-task 0)
										(list 'wait-for "`balsa_response_delay")
										(list 'assign (bundle 'ack "activate") 1)
										(list 'wait-for "`balsa_rtz_delay")
										(list 'wait-until-falling args-down)
										(list 'call-builtin-task 1)
									)
								)
							)
							; (list 'call unparameterised-name (map net-net:name port-nets)
							;	(cons 'parameters params) (list 'guard "done"))
						)
					)
				)
			)
			((string=? "dual_b" breeze-style)
				(let
					((args-up (if (zero? arg-count)
						(bundle 'req "activate")
						(build-separated-string (map (lambda (port)
							(let
								((port-name (brz-port:name port)))
								(string-append "(& (" (bundle 'ack0 port-name) " | " (bundle 'ack1 port-name)
									"))")
							)
						) input-ports) " & ")
					 ))
					 (args-down (if (zero? arg-count)
						(bundle 'req "activate")
						(build-separated-string (map (lambda (port)
							(let ((port-name (brz-port:name port)))
								(string-append "(| (" (bundle 'ack0 port-name) " | " (bundle 'ack1 port-name)
									"))")
							)
						) input-ports) " | ")
					 ))
					)
					(list 'circuit name
						(cons 'ports port-nets)
						(list 'nets
							(list (bundle 'ack0 "activate") activate-width 'assignable)
							(list (bundle 'ack1 "activate") activate-width 'assignable)
						)
						(cons 'instances
							(append
								(map (lambda (name)
									(buffer (list 'req "activate" 0) (list 'req name 0))
								) argument-names)
								(list
									(list 'task-for-builtin-call
										(cons* 'call "BalsaBuiltin"
											(append!
												(cons*
													"ack_state"
													(stringify unparameterised-name) param-count
													(map escape-param params)
												)
												(map (lambda (port) (bundle 'ack1 (brz-port:name port))) ports)
											)
										)
									)
									(list 'shdl-repeat
										(list 'assign (bundle 'ack0 "activate") 0)
										(list 'assign (bundle 'ack1 "activate") 0)
										(list 'wait-until-rising args-up)
										(list 'wait-for "`balsa_sampling_delay")
										(list 'call-builtin-task 0)
										(list 'wait-for "`balsa_response_delay")
										(list 'assign (bundle 'ack0 "activate")
											(string-append "~ " (bundle 'ack1 "activate")))
										(list 'wait-for "`balsa_rtz_delay")
										(list 'wait-until-falling args-down)
										(list 'call-builtin-task 1)
									)
								)
							)
							; (list 'call unparameterised-name (map net-net:name port-nets)
							;	(cons 'parameters params) (list 'guard "done"))
						)
					)
				)
			)
			((string=? "one_of_2_4" breeze-style)
				(let*
					((args-up (if (zero? arg-count)
						(bundle 'req "activate")
						(build-separated-string (map (lambda (port)
							(let 
								((port-name (brz-port:name port))
								 (width (brz-type-width (brz-port:type port) type-context))
								)
								(if (= width 1)
								 (string-append "(& (" (bundle 'ack0 port-name) " | " (bundle 'ack1 port-name) "))")
								 (string-append "(& (" (bundle 'ack0 port-name) " | " (bundle 'ack1 port-name) " | "
								 	(bundle 'ack2 port-name) " | " (bundle 'ack3 port-name) "))")
								)
							)
						) input-ports) " & ")
					 ))
					 (args-down (if (zero? arg-count)
						(bundle 'req "activate")
						(build-separated-string (map (lambda (port)
							(let 
								((port-name (brz-port:name port))
								 (width (brz-type-width (brz-port:type port) type-context))
								)
								(if (= width 1)
									(string-append "(| (" (bundle 'ack0 port-name) " | " (bundle 'ack1 port-name) "))")
									(string-append "(| (" (bundle 'ack0 port-name) " | " (bundle 'ack1 port-name) " | "
										(bundle 'ack2 port-name) " | " (bundle 'ack3 port-name) "))")
								)
							)
						) input-ports) " | ")
					 ))
					 (encode-nodes (map (lambda (port)
					 	(let
							((port-name (brz-port:name port))
							 (width (brz-type-width (brz-port:type port) type-context))
							)
							(list (bundle 'node port-name) width)
						)
					 ) input-ports))
					 (encode-gates (fold (lambda (port gates)
					 		(let*
								((port-name (brz-port:name port))
								 (width (brz-type-width (brz-port:type port) type-context))
								 (plural? (> width 1))
								 (width-odd (quotient (+ width 1) 2))
								 (width-even (quotient width 2))
								)
								(append gates
									(fold (lambda (index gates)
										(append gates
											(list
												(list 'or (list 'node port-name 0 (* index 2) 1) (list 'ack1 port-name 0 index 1) (list 'ack3 port-name 0 index 1))
												(list 'or (list 'node port-name 0 (+ (* index 2) 1) 1) (list 'ack2 port-name 0 index 1) (list 'ack3 port-name 0 index 1))
											)
										)
									) '() (.. 0 (- width-even 1)))
									(if (odd? width)
								 	(list (list 'connect (list 'node port-name 0 (- width 1) 1) (list 'ack1 port-name 0 width-even 1)))
										'()
									)
								)
							)
					 ) '() input-ports))
					 (decode-nodes
					 	(append
							(list
								(list (bundle 'node "activate0") activate-width 'assignable)
								(list (bundle 'node "activate1") activate-width 'assignable)
							)
						 	(if (> activate-width 1)
								(let
									((width-odd (quotient (+ activate-width 1) 2))
									 (width-even (quotient activate-width 2))
									)
									(list
										(list (bundle 'ack0 "activate") width-odd)
										(list (bundle 'ack1 "activate") width-odd)
										(list (bundle 'ack2 "activate") width-even)
										(list (bundle 'ack3 "activate") width-even)
									)
								)
								(list
									(list (bundle 'ack0 "activate") 1)
									(list (bundle 'ack1 "activate") 1)
								)
							)
						)
					 )
					 (decode-gates
					 	(if (= activate-width 1)
							(list
								'(connect (ack0 "activate" 0 0 1) (node "activate0" 0 0 1))
								'(connect (ack1 "activate" 0 0 1) (node "activate1" 0 0 1)) 							
							)
							(let
								((width-odd (quotient (+ activate-width 1) 2))
								 (width-even (quotient activate-width 2))
								)
								(append
									(fold (lambda (index gates)
										(append gates
											(list
												(list 'c-element (list 'ack0 "activate" 0 index 1) (list 'node "activate0" 0 (* index 2) 1) (list 'node "activate0" 0 (+ (* index 2) 1) 1))
												(list 'c-element (list 'ack1 "activate" 0 index 1) (list 'node "activate1" 0 (* index 2) 1) (list 'node "activate0" 0 (+ (* index 2) 1) 1))
												(list 'c-element (list 'ack2 "activate" 0 index 1) (list 'node "activate0" 0 (* index 2) 1) (list 'node "activate1" 0 (+ (* index 2) 1) 1))
												(list 'c-element (list 'ack3 "activate" 0 index 1) (list 'node "activate1" 0 (* index 2) 1) (list 'node "activate1" 0 (+ (* index 2) 1) 1))
											)
										)
									) '() (.. 0 (- width-even 1)))
									(if (odd? activate-width)
										(list
											(list 'connect (list 'ack0 "activate" 0 (- width 1) 1) (list 'node "activate0" 0 width-even 1))
											(list 'connect (list 'ack1 "activate" 0 (- width 1) 1) (list 'node "activate1" 0 width-even 1))   
										)
										'()
									)
								)
							)
					 	)
					 )
					)
					(list 'circuit name
						(cons 'ports port-nets)
						(cons 'nets
							(append
								encode-nodes
								decode-nodes
							)
						)
						(cons 'instances
							(append
								(map (lambda (name)
									(buffer (list 'req "activate" 0) (list 'req name 0))
								) argument-names)
								(map (lambda (gate)
									(gen-abs-gate->net-gate (gen-simple-gate->default-gate gate))
								) (append encode-gates decode-gates))
								(list
									(list 'task-for-builtin-call
										(cons* 'call "BalsaBuiltin"
											(append!
												(cons*
													"ack_state"
													(stringify unparameterised-name) param-count
													(map escape-param params)
												)
												(cons (bundle 'node "activate1") (map (lambda (port) (bundle 'node (brz-port:name port))) input-ports))
											)
										)
									)
									
									(list 'shdl-repeat
										(list 'assign (bundle 'node "activate0") 0)
										(list 'assign (bundle 'node "activate1") 0)
										(list 'wait-until-rising args-up)
										(list 'wait-for "`balsa_sampling_delay")
										(list 'call-builtin-task 0)
										(list 'wait-for "`balsa_response_delay")
										(list 'assign (bundle 'node "activate0")
											(string-append "~ " (bundle 'node "activate1")))
										(list 'wait-for "`balsa_rtz_delay")
										(list 'wait-until-falling args-down)
										(list 'call-builtin-task 1)
									)
								)
							)
							; (list 'call unparameterised-name (map net-net:name port-nets)
							;	(cons 'parameters params) (list 'guard "done"))
						)
					)
				)
			)
			(else
				(error "gen-make-builtin-function-netlist: only " (build-separated-string supported-styles " ")
					" currently supported for builtin functions" #\newline)
			)
		)
	)
))

;;; tech-mangle-builtin-function-name: make a builtin function circuit-decl name
;;;		eg. ("String" "Hello") => Balsa_String_s5_Hello
(define tech-mangle-builtin-function-name (lambda (name parameters)
	(string-append tech-balsa-prefix (tech-map-name name) (tech-mangle-parameters parameters))
))

;;; gen-make-undeclared-component-netlist: make a .net netlist for the named undeclared component
;;;		currently this only works for components which implement builtin-functions
(define gen-make-undeclared-component-netlist (lambda (name params options type-context)
	(if (not (gen-undeclared-is-builtin-function? options))
		(error "gen-make-undeclared-component-netlist: can't make anything other than builtin functions")
	)
	(list (gen-make-builtin-function-netlist
		(tech-map-cell-name (tech-mangle-builtin-function-name name params)) name
		params (find-headed-sub-list options 'ports)
		type-context
	))
))

;;; gen-make-builtin-function-part-netlist: analogue of gen-make-breeze-part-netlist for unparameterised
;;;		parts which implement builtin functions
(define gen-make-builtin-function-part-netlist (lambda (part all-types)
	(gen-make-builtin-function-netlist
		(tech-map-cell-name (tech-mangle-breeze-part-name (brz-breeze-part:name part))) (brz-breeze-part:name part)
		'() (brz-breeze-part:ports part)
		all-types
	)
))
