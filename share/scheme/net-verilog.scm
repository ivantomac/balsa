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
;;;	`net-verilog.scm'
;;;	Structural Verilog netlist generation
;;;

(balsa-scheme-import 'net)
(balsa-scheme-import 'misc 'date)

;;; verilog-netlist-signature: defaults for Verilog netlists
(define verilog-netlist-signature (lambda () (list
	verilog-write-netlist-file ; write-netlist-file
	"!vcc" ; vcc-net-name
	#f ; vcc-component
	"!gnd" ; gnd-net-name
	#f ; gnd-component
	`(nets ("vcc" 1) ("gnd" 1)) ; global nets
	verilog-map-name ; map-name
	verilog-bundle-name ; bundle-name
	"v" ; filename-suffix
	verilog-channel-name ; channel-name
	verilog-bus-suffix ; bus-suffix
	"Brz" ; brz-prefix
	"Balsa_" ; balsa-prefix
	#f ;;;;;; FIXME verilog-write-pin-file ; write-symbol-file
	"pins" ; symbol-filename-suffix
)))

;;; verilog-map-name: map a string of the form [_A-Za-z][_A-Za-z0-9]* into
;;;		a valid verilog (case sensitive) identifier.
;;;		eg. (verilog-map-name "_HelloWorld") => "__HelloWorld"
(define verilog-map-name (lambda (str)
	(apply string-append (map (lambda (char)
		(cond
			((char=? #\_ char) "__")
			((or (char-alphabetic? char) (char-numeric? char)) (string char))
			(else ; gimme hex
				(string-append "_" (radix-string 16 (remainder (char->integer char) 128) 2
					lowercase-radix-characters))
			)
		)
	) (string->list str)))
))

(define verilog-portion-suffix-mapping '(
	(node "n")
	(req "r")
	(ack "a")
	(data "d")
	(req0 "r0d")
	(req1 "r1d")
	(req2 "r2d")
	(req3 "r3d")
	(ack0 "a0d")
	(ack1 "a1d")
	(ack2 "a2d")
	(ack3 "a3d")
))
(define verilog-portion-suffix-reverse-mapping
	(map (lambda (elem) (apply rev-cons elem)) verilog-portion-suffix-mapping))

;;; verilog-portion-suffix: name suffix used for portion `portion'
(define verilog-portion-suffix (lambda (portion)
	(cadr (assv portion verilog-portion-suffix-mapping))
))

;;; verilog-split-bundle-name: take a bundle name something like "inp_4r" and
;;;		split into a list of (name index portion) where portion is an unchecked string
;;;		return #f if the bundle name is not valid in some way.
(define verilog-split-bundle-name (lambda (name)
	(let*
		((name-len (string-length name))
		 (last-_-pos (strrchr name #\_))
		)
		(if (and last-_-pos (/= last-_-pos (- name-len 1))) ; has a last _ and it isn't the last char
			(let
				((basename (substring name 0 last-_-pos))
				 (last-digit-index (if (char-numeric? (string-ref name (+ 1 last-_-pos)))
					(letrec ((last-digit-index (lambda (i)
						(if (= i name-len)
							(- i 1)
							(if (char-numeric? (string-ref name i))
								(last-digit-index (+ 1 i))
								(- i 1)
							)
						)
					))) (last-digit-index (+ 2 last-_-pos)))
					#f
				 ))
				)
				(if (and last-digit-index (/= last-digit-index (- name-len 1)))
					(list basename
						(string->number (substring name (+ 1 last-_-pos) (+ 1 last-digit-index)))
						(substring name (+ 1 last-digit-index) name-len)
					)
					#f
				)
			)
			#f
		)
	)
))

;;; verilog-bundle-name: transform a bundle name <name> which is index <index> in an arrayed
;;;		port or bundle (or #f if not arrayed) corresponding to the request/acknowledge/data portion
;;;		of that port. eg. (verilog-bundle-name 'req "inp" 4) => "inp_4r"
;;;		Case smashes names to lower case and produced identifiers which are always distinct
;;;		from the given name.
;;;		If the name begins with #\! then just return the name verbatim
(define verilog-bundle-name (lambda (portion name index)
	(if (eqv? (string-ref name 0) #\!)
		(substring name 1 (string-length name))
		(string-append
			(verilog-map-name name)
			(string #\_)
			(if index (number->string index) "")
			(verilog-portion-suffix portion)
		)
	)
))

;;; verilog-channel-name: give a name to portion `portion' of channel number `channel-no'
;;;		eg. (verilog-channel-name 'req 34) => "c34_r"
(define verilog-channel-name (lambda (portion channel-no)
	(string-append "c" (number->string channel-no) "_" (verilog-portion-suffix portion))
))

;;; verilog-bus-suffix: produce a valid suffix for a verilog vector signal (eg. a data portion
;;;		of a bundle) name. eg. (verilog-bus-suffix 4 3) => "[6:4]"
;;;		`low-index' and `width' specify the size of the bus slice, `cardinality' should
;;;		be the width of the whole bus from which this is a slice.  The slice is always
;;;		given in [(low-index+cardinality-1):low-index] form.
(define verilog-bus-suffix (lambda (low-index width cardinality)
	(if (= 1 cardinality)
		""
		(string-append
			(string #\[)
			(if (= 1 width)
				""
				(string-append (number->string (+ width low-index -1)) (string #\:))
			)
			(number->string low-index)
			(string #\])
		)
	)
))

;;; verilog-port-name-list: returns a string of suitably separated port names
;;;		the port names will have internal ",\n  " separation between channels and
;;;		", " separation between non-channel signals.  No external whitespace is returned.
(define verilog-port-name-list (lambda (ports map-name?)
	(let
		((map-func (if map-name? (lambda (port) (verilog-map-name (net-port:name port)))
			(lambda (port) (net-port:name port))))
		 (ports-newline-string (string-append (string #\newline) "  "))
		)
		(letrec
			((tail (lambda (ports prev-chan-name prev-chan-index ret)
				(if (null? ports)
					ret
					(let*
						((mapped-name (map-func (car ports)))
						 (split-name (verilog-split-bundle-name mapped-name))
						 ; same-line? if we're not talking channels or if the channel names match
						 (same-line? (and split-name (or (not prev-chan-name) (and
							(string=? (car split-name) prev-chan-name)
							(= (cadr split-name) prev-chan-index)
						 ))))
						 (new-prev-chan-name (cond
							(split-name (car split-name))
							(prev-chan-name "")
							(else #f)
						 ))
						 (new-prev-chan-index (if split-name (cadr split-name) 0))
						)
						(tail (cdr ports) new-prev-chan-name new-prev-chan-index
							(if ret ; ret is #f for the first channel only
								(string-append ret
									(string-append "," (if same-line? " " ports-newline-string))
									mapped-name
								)
								mapped-name
							)
						)
					)
				)
			)))
			(tail (drop-head-symbol 'ports ports) #f #f #f)
		)
	)
))

;;; verilog-port-direction-strings: strings to print for the various port dirs.
(define verilog-port-direction-strings '(
	(input . "input") (output . "output") (inout . "inout") (hiz . "buffer") ; FIXME
))

;;; verilog-print-port-decls: print port declarations eg. "input name", "input [5:0] name"
(define verilog-print-port-decls (lambda (out ports port-direction-strings)
	(for-each (lambda (port)
		(print-to-port out
			"  "
			(cdr (assv (net-port:direction port) port-direction-strings)) " "
			(if (= 1 (net-port:cardinality port))
				""
				(string-append "[" (number->string (- (net-port:cardinality port) 1)) ":0] ")
			)
			(net-port:name port) ";" #\newline;
		)
	) (drop-head-symbol 'ports ports))
))

;;; verilog-print-circuit-prototype: print a prototype for a component from the library
(define verilog-print-circuit-prototype (lambda (out circuit)
	(print-to-port out "module " (net-circuit-decl:name circuit))
	(if (not (null? (cdr (net-circuit-decl:ports circuit))))
		(print-to-port out " (" #\newline "  "
			(verilog-port-name-list (net-circuit-decl:ports circuit) #f) #\newline ");" #\newline)
		(print-to-port out ";" #\newline)
	)
	(verilog-print-port-decls out (net-circuit-decl:ports circuit) verilog-port-direction-strings)
	(print-to-port out "endmodule" #\newline #\newline)
))

;;; verilog-print-connection: print an instance connection
(define verilog-print-connection (lambda (out connection sorted-nets)
	(let*
		((vcc-net-name (verilog-bundle-name 'data tech-vcc-net-name 0)) ; bundle-name smashes global net prefices!
		 (gnd-net-name (verilog-bundle-name 'data tech-gnd-net-name 0))
		 (print-power-connection (lambda (name)
			(if (string? name)
				(if (or (string=? name vcc-net-name) (string=? name gnd-net-name))
					(begin (print-to-port out name) #t)
					#f
				)
				#f
			)
		)))
		(cond
			((string? connection)
				(if (not (print-power-connection connection))
					(print-to-port out connection)
				)
			) ; must be single bit wire, FIXME?
			((pair? connection)
				(cond
					((and (not (null? (cdr connection)))
						(eq? (car connection) 'vector))
						(print-to-port out "{")
						(verilog-print-connections out (cdr connection) sorted-nets) 
						(print-to-port out "}")
					)
					(else
						(let*
							((cardinality (if (= 2 (length connection)) 1 (caddr connection)))
							 ; AB 2008-08-24, don't print indexing on multibit connections across whole defn.
							 (net (net-find-sorted-net sorted-nets (car connection)))
							 (decl-cardinality (if net (net-net:cardinality net) 1))
							)
							(if (= 1 cardinality)
								(if (not (print-power-connection (car connection)))
									(if (= 1 decl-cardinality)
										(print-to-port out (car connection))
										(print-to-port out (car connection) "[" (cadr connection) "]")
									)
								)
								(if (and (= decl-cardinality cardinality) (= 0 (cadr connection)))
									(print-to-port out (car connection))
									(print-to-port out (car connection) "[" (+ cardinality -1 (cadr connection))
										":" (cadr connection) "]")
								)
							)
						)
					)
				)
			)
			((eq? connection 'unconnected) ; print nothing?
			)
		)
	)
))
;;; Print Connections updated to only print 50 connections on a line to prevent
;;; verilog simulators truncating really long lines

;;; verilog-print-connections: print a whole list of connections (with ", " separation)
(define verilog-print-connections (lambda (out connections sorted-nets)
	(letrec	
		((print-conn (lambda (connections index)
				(cond 
					((null? connections))
					((= 50 index)
						(begin
							(print-to-port out "," #\newline #\tab #\tab) ;; if over 50 connections print on next line
							(verilog-print-connection out (car connections) sorted-nets)
							(print-conn (cdr connections) 0)
						)
					)
					(else
						(begin
							(print-to-port out ", ")
							(verilog-print-connection out (car connections) sorted-nets)
							(print-conn (cdr connections) (+ index 1))
						)
					)
				)
		 ))
		)
		(verilog-print-connection out (car connections) sorted-nets)
		(print-conn (cdr connections) 1)
	)
))

;;; verilog-print-named-connections: print connections with .portname() format
(define verilog-print-named-connections (lambda (out connections sorted-nets ports)
	(letrec	
		((print-conn (lambda (connections ports index)
				(cond 
					((null? connections) (print-to-port out ")"))
					((= 50 index)
						(begin ;;(
							(print-to-port out ")," #\newline #\tab #\tab) ;; if over 50 connections print on next line
							(print-to-port out "." (net-port:name (car ports)) "(") ;;)
							(verilog-print-connection out (car connections) sorted-nets)
							(print-conn (cdr connections) (cdr ports) 0)
						)
					)
					(else
						(begin
							(print-to-port out "), ")
							(print-to-port out "." (net-port:name (car ports)) "(")
							(verilog-print-connection out (car connections) sorted-nets)
							(print-conn (cdr connections) (cdr ports) (+ index 1))
						)
					)
				)
		 ))
		)
		(print-to-port out "." (net-port:name (car ports)) "(")
		(verilog-print-connection out (car connections) sorted-nets)
		(print-conn (cdr connections) (cdr  ports) 1)
	)
))
	

;;; verilog-escape-string: escape " in given string
(define verilog-escape-string (lambda (string)
	; Do this the lazy way
	(list->string (reverse! (fold (lambda (char ret)
		(case char
			((#\") (cons #\" (cons #\\ ret)))
			((#\nl) (cons #\n (cons #\\ ret)))
			((#\`) (cons #\' ret))
			(else (cons char ret))
		)
	) '() (string->list string))))
))

;;; <shdl-expr> ::= string

;;; <shdl-term> ::= (wait-until-rising <expr>)
;;; 	| (wait-until-falling <expr>)
;;;		| (wait-for <expr>)
;;;		| (assign <expr> <expr>)
;;;		| (call <name> ( <expr> )* )
;;;     | (check-if (<expr> <expr> <expr>) <list-of-shdl-terms> [<list-of-shdl-term>]

;;; check-if defines an "if" Verilog statement
;;; first argument defines the condition (i.e. ("a_r" === 1))
;;; second argument is a list of shdl terms that are to be executed if condition holds (the "then" clause)
;;; third argument is optional, contains shdl terms executed if contition does not hold (the "else" clause)

;;; verilog-print-shdl-expr: print out expressions from the SHDL
(define verilog-print-shdl-expr (lambda (out expr tabs sorted-nets)
	(let
		((connection->string (lambda (conn . z)
			(let*
				((string-port (open-output-string))
				 (dummy (verilog-print-connection string-port conn sorted-nets))
				 (ret (get-output-string string-port))
				)
				(close string-port)
				ret
			)
		)))
		(print-to-port out (list-replace-headed expr 'tech-conn connection->string))
	)
))

;;; verilog-print-shdl-term: print out terms of the Balsa simple simulation HDL
(define verilog-print-shdl-term (lambda (out term tabs sorted-nets)
	(let
		((expr (lambda (e) (verilog-print-shdl-expr out e (string-append tabs "  ") sorted-nets))))
		(print-to-port out #\newline tabs)
		(case (car term)
			((wait-until-rising wait-until-falling)
				; (print-to-port out "@(" (if (eq? (car term) 'wait-until-rising) "posedge" "negedge") " "))
				(print-to-port out "wait ((")
				(expr (cadr term))
				(print-to-port out ") === " (if (eq? (car term) 'wait-until-rising) "1" "0") ");");
			)
			((wait-for)
				(print-to-port out "#")
				(expr (cadr term))
				(print-to-port out ";")
			)
			((assign)
				(expr (cadr term))
				(print-to-port out " = ");
				(expr (caddr term))
				(print-to-port out ";")
			)
			((check-if)
				(print-to-port out "if ((")
				(expr (car ((nth 1) term))) ;((cadr term))
				(print-to-port out ") ")
				(expr (cadr ((nth 1) term))) ;( === ")
				(print-to-port out " ")
				(expr (caddr ((nth 1) term)))
				; (expr ((nth 2) term))
				(print-to-port out ") begin")
				(for-each (lambda (n) 
					(verilog-print-shdl-term out n (string-append tabs "  ") sorted-nets)
				) ((nth 2) term))
				(if (not (null? (cdddr term)))
					(begin
						(print-to-port out #\newline tabs "end" #\newline)
						(print-to-port out tabs "else begin")
						(for-each (lambda (n) 
							(verilog-print-shdl-term out n (string-append tabs "  ") sorted-nets)
						) ((nth 3) term))
					)
				)
				(print-to-port out #\newline tabs "end")
			)
			((call)
				(let
					((args (cddr term)))
					(print-to-port out "$" (cadr term) " (")
					(if (not (null? args))
						(expr (car args))
					)
					(for-each (lambda (arg)
						(print-to-port out ", ")
						(expr arg)
					) (cdr args))
					(print-to-port out ");")
				)
			)
			((call-builtin-task)
				(print-to-port out "builtin_task (" (cadr term) ");")
			)
			(else
				(print-to-port out "/* unknown term */")
			)
		)
	)
))
	
;;; verilog-print-instance-line: print a component instance doing the right thing
;;;		for vectored connections. `nets' should be a vector of sorted nets relevant for
;;;		this circuit. `name' is used as the instance name if no instance-name
;;;		is specified in the netlist.
(define verilog-print-instance-line (lambda (out instance name sorted-nets)
	(case (car instance)
		((instance) (let
			((instance-name (let ((in (net-instance:instance-name instance))) (if in in name)))
			 (connect-by-name (hash-ref verilog-connect-by-name-cells (net-instance:name instance)))
			 (has-connections (not (null? (net-instance:instance-connections instance))))
			)
			(print-to-port out (net-instance:name instance) " " instance-name " (")
			(if has-connections
				(if connect-by-name ; Connect by name or position?
					(verilog-print-named-connections out (net-instance:instance-connections instance) sorted-nets
						(cdr (net-circuit-decl:ports connect-by-name)))
					(verilog-print-connections out (net-instance:instance-connections instance) sorted-nets)
				)
			)
			(print-to-port out ");" #\newline)
		))
		((shdl-repeat)
			(print-to-port out "always begin")
			(for-each (lambda (term)
				(verilog-print-shdl-term out term "    " sorted-nets)
			) (cdr instance))
			(print-to-port out #\newline "  end" #\newline)
		)
		((shdl)
			(print-to-port out "initial begin")
			(for-each (lambda (term)
				(verilog-print-shdl-term out term "    " sorted-nets)
			) (cdr instance))
			(print-to-port out #\newline "  end" #\newline)
		)
		((shdl-assign)
			(print-to-port out "assign ")
			(verilog-print-shdl-expr out (cadr instance) "    " sorted-nets)
			(print-to-port out " = ")
			(verilog-print-shdl-expr out (caddr instance) "    " sorted-nets)
			(print-to-port out ";" #\newline)
		)
		((task-for-builtin-call)
			(print-to-port out "task builtin_task;" #\newline)
			(print-to-port out "    input ack_state;")
			(for-each (lambda (term)
				(verilog-print-shdl-term out term "    " sorted-nets)
			) (cdr instance))
			(print-to-port out #\newline "  endtask" #\newline)
		)
	)
))

;;; verilog-print-circuit-definition: write a circuit definition with the ports, instances and module...endmodule
;;;		encapsulation
(define verilog-print-circuit-definition (lambda (out circuit)
	(let*
		((name (net-circuit-decl:name circuit))
		 (ports (net-circuit-decl:ports circuit))
		 (nets (net-circuit-decl:nets circuit))
		 (sorted-nets (list->vector (merge-sort net-net:name<=?
			(append! (cdr (net-ports->nets ports)) (cdr nets)))))
		 (vcc-net-name (verilog-bundle-name 'data tech-vcc-net-name 0)) ; bundle-name smashes global net prefices!
		 (gnd-net-name (verilog-bundle-name 'data tech-gnd-net-name 0))
		 (top-level-cell (if verilog-top-level-cell (string=? name verilog-top-level-cell) #f))
		 (uses-vcc? (net-circuit-decl-uses-net-name circuit vcc-net-name))
		 (uses-gnd? (net-circuit-decl-uses-net-name circuit gnd-net-name))
		 (initialisations (net-circuit-decl-find-attribute circuit 'simulation-initialise))
		)
		(print-to-port out "module " name)
		(if (not (null? (cdr (net-circuit-decl:ports circuit))))
			(print-to-port out " (" #\newline "  "
				(verilog-port-name-list (net-circuit-decl:ports circuit) #f) #\newline ");" #\newline)
			(print-to-port out ";" #\newline)
		)
		(verilog-print-port-decls out (net-circuit-decl:ports circuit) verilog-port-direction-strings)
		;;; print out top level wires
		(for-each (lambda (wire)
			(print-to-port out (if (memq 'assignable (net-net:options wire)) "  reg " "  wire ")
				(if (= 1 (net-net:cardinality wire))
					""
					(string-append "[" (number->string (- (net-net:cardinality wire) 1)) ":0] ")
				)
				(net-net:name wire) ";" #\newline
			)
		) (cdr nets))
		(if (or (and uses-gnd? (not verilog-top-level-cell)) top-level-cell) ;; only want to put supply declarations if not propagating to top-level
			(print-to-port out (if tech-gnd-component-name "  wire " "  supply0 ") gnd-net-name ";" #\newline)
		)
		(if (or (and uses-vcc? (not verilog-top-level-cell)) top-level-cell)
			(print-to-port out (if tech-vcc-component-name "  wire " "  supply1 ") vcc-net-name ";" #\newline)
		)
		;;; conditionally print initialisations
		(if (and verilog-simulation-initialise initialisations)
			(begin
				(print-to-port out
					#\newline
					"`ifdef balsa_simulate" #\newline
					"  initial begin" #\newline
				)
				(for-each (lambda (init)
					(print-to-port out "    force " (car init) " = " (cadr init) ";" #\newline)
				) (cdr initialisations))
				(print-to-port out
					"    #`balsa_init_time;" #\newline
				)
				(for-each (lambda (init)
					(print-to-port out "    release " (car init) ";" #\newline
						"    if (" (car init) " !== " (cadr init) ")" #\newline
						"        $display (\"module %m: signal " (car init)
							" not correctly initialised\");" #\newline
					)
				) (cdr initialisations))
				(print-to-port out
					"  end" #\newline
					"`endif" #\newline #\newline
				)
			)
		)
		;;; print the instances
		(if (and tech-gnd-component-name (or (and uses-gnd? (not verilog-top-level-cell)) top-level-cell))
			(print-to-port out "  " tech-gnd-component-name " " gnd-net-name "_cell_instance ("
				gnd-net-name ");" #\newline)
		)
		(if (and tech-vcc-component-name (or (and uses-vcc? (not verilog-top-level-cell)) top-level-cell))
			(print-to-port out "  " tech-vcc-component-name " " vcc-net-name "_cell_instance ("
				vcc-net-name ");" #\newline)
		)
		; (print-to-port out "  LOGIC0 gnd_circuit (gnd);" #\newline "  LOGIC1 vcc_circuit (vcc);" #\newline)
		(fold (lambda (instance instance-number)
			(let ((substs (verilog-cell-feedthrough-substitutions (net-instance:name instance))))
				(if (and substs verilog-replace-feedthroughs)
					;; handle feedthrough components
					(let ((connections (net-instance:instance-connections instance)))
						(for-each (lambda (pair)
							(print-to-port out "  assign ")
							(verilog-print-connection out (list-ref connections (cadr pair)) sorted-nets)
							(print-to-port out " = ")
							(verilog-print-connection out (list-ref connections (car pair)) sorted-nets)
							(print-to-port out ";" #\newline)
						) substs)
					)
					(begin
						(print-to-port out "  ")
						(verilog-print-instance-line
							out instance
							(string-append "I" (number->string instance-number)) ;; respect instance names?
							sorted-nets
						)
					)
				)
				(+ 1 instance-number)
			)
		) 0 (cdr (net-circuit-decl:instances circuit)))
		(print-to-port out "endmodule" #\newline #\newline)
	)
))

;;; verilog-maybe-print-circuit-definition: test some global flags to decide whether
;;;		to print the given circuit decl.  Also tests attributes attached to those circuits
;;;		to decide.
;;;		Rules:
;;;			No instances => print module with just port structure
;;;			not verilog-print-prototypes => don't even print port structures
;;;			Circuit has (cell-type ...) attribute included in verilog-included-cell-types =>
;;;				print the definition
(define verilog-maybe-print-circuit-definition (lambda (out circuit)
	(let*
		((any-instances? (not (null? (cdr (net-circuit-decl:instances circuit)))))
		 (print-definition? (and any-instances?
			(let ((cell-type (net-circuit-decl-cell-type circuit)))
				(and cell-type (member cell-type verilog-included-cell-types)))))
		 (print-prototype? (and verilog-print-prototypes (not print-definition?)))
		 (feedthrough-attr (net-circuit-decl-find-attribute circuit 'feedthrough))
		 (connect-by-name-attr (net-circuit-decl-find-attribute circuit 'connect-by-name))
		)
		; Note if this circuit is to be connected by name
		(if connect-by-name-attr
			(hash-set! verilog-connect-by-name-cells (net-circuit-decl:name circuit) circuit)
		)
		(if (not (member (net-circuit-decl:name circuit) verilog-excluded-cell-names))
			(if feedthrough-attr ; register this as a feedthrough cell
				(begin
					(if (not verilog-replace-feedthroughs) 
						(cond
							(print-definition?
								(verilog-print-circuit-definition out circuit)
							)
							(print-prototype?
								(verilog-print-circuit-prototype out circuit)
							)
						)
					)
					(hash-set! verilog-feedthrough-cells (net-circuit-decl:name circuit)
						(cdr feedthrough-attr)
					)
				)
				(begin
					(if print-prototype? (verilog-print-circuit-prototype out circuit))
					(if print-definition? (verilog-print-circuit-definition out circuit))
				)
			)
		)
	)
))

;;; verilog-write-pin-file-entry: write a pin file entry for the single circuit `circuit'
(define verilog-write-pin-file-entry (lambda (out circuit)
	; print-ports: if the next port name is different from this one (the basename of the port)
	;	or this is the last port then print out a pin line for the sense and direction
	;	indicated.  If it isn't then add to the pending-port-names and update the sense and
	;	direction as expected.
	(letrec
		((print-ports (lambda (ports pending-port-names sense direction)
			(let*
				((port-side (lambda (sense direction)
					(case direction
						((input) "left")
						((output) "right")
						((sync) (if (eq? 'passive sense) "top" "bottom"))
					)
				 ))
				 (print-pending-ports (lambda (pending-ports)
					(print-to-port out "pin_placement := " (net-circuit-decl:name circuit) ", "
						(port-side sense direction) ", "
						(build-separated-string (reverse (map cdr pending-ports)) ", ") #\newline
					)
				 ))
				)
				(if (null? ports)
					(print-pending-ports pending-port-names)
					(let*
						((port-name (net-port:name (car ports)))
						 (port-dir (net-port:direction (car ports)))
						 (name/index/portion (verilog-split-bundle-name port-name))
						 (basename (if name/index/portion (car name/index/portion) ""))
						 (portion (if name/index/portion (caddr name/index/portion) 'req))
						)
						(if (or (null? pending-port-names)
								(string=? (caar pending-port-names) basename))
							(print-ports (cdr ports) ; add port to pending list
								(cons (cons basename port-name) pending-port-names)
								(case portion
									((req) (if (eq? port-dir 'input) 'passive 'active))
									((ack) (if (eq? port-dir 'output) 'passive 'active))
									(else sense)
								)
								(if (not (memv portion '(req ack))) port-dir direction)
							)
							(begin
								(print-pending-ports pending-port-names)
								(print-ports ports '() 'passive 'sync)
							)
						)
					)	
				)
			)
		)))
		(print-ports (cdr (net-circuit-decl:ports circuit)) '() 'passive 'sync)
	)
))

;;; verilog-print-header: print a suitable file header
(define verilog-print-header (lambda (out filename)
	(let
		((unix-name (let ((u-name (uname))) (if (vector? u-name) u-name
			#("unknown" "unknown" "unknown" "1" "unknown")))))
		(print-to-port out
			"/*" #\newline
			"    `" filename "'" #\newline
			"    Balsa Verilog netlist file" #\newline
			"    Created: " (ctime (current-time)) #\newline
			"    By: " (vector-ref (getpw (getuid)) 0)
			"@" (vector-ref unix-name 1) " (" (vector-ref unix-name 0) ")" #\newline
			"    With net-verilog (balsa-netlist) version: " balsa-version #\newline
			"    Using technology: " (brz-pretty-format-tech-name) #\newline
            "    Command line : " command-line-args #\newline
		)
		(if verilog-simulation-initialise
			(print-to-port out
				#\newline
				"    Using `simulation-initialise'" #\newline
				"    You must set the following preprocessor directives to use this file:" #\newline
				"        balsa_simulate: set if you wish to initialise signal values during sim." #\newline
				"        balsa_init_time: duration of forced initialisation" #\newline
			)
		)
		(if verilog-global-nets
			(begin
				(print-to-port out
					#\newline
					"    Using `propagate-globals'" #\newline
				)
				(if (null? verilog-global-nets)
					(print-to-port out "    The design contains no global nets" #\newline)
					(print-to-port out "    The design contains the following global nets" #\newline)
				)
				(with-output-to-port out (lambda ()
					(for-each (lambda (port)
						(let
							((name (car port))
						 	 (direction (cadr port))
							 (width (caddr port))
							)
							(print #\tab #\tab "global-signal: " #\space (verilog-map-name name) 
								#\space direction #\space width #\newline)
						) 
					) verilog-global-nets)
				))
			)
		)
		(print-to-port out "*/" #\newline #\newline)
		(print-to-port out "`timescale 1ns/1ps" #\newline #\newline)
	)
))

;;; Features for netlist generation.
(define verilog-print-prototypes #t)
(define verilog-included-cell-types '())
(define verilog-excluded-cell-names '())
(define verilog-simulation-initialise #f)
(define verilog-replace-feedthroughs #f)
(define verilog-top-level-cell #f)
(define verilog-global-nets #f)

;;; Verilog feedthrough cells.  This netlister will make a note of all
;;;		the cells which are valid feedthroughs and cache a hash of
;;;		name => port-substitution-list for those cells. 
(define verilog-feedthrough-cells '())

;;; veilog-connect-by-name-cells: hash of circuit-names to circuit-defns
;;;		for circuits who prefer to be placed with named rather than positional
;;;		instance connections
(define verilog-connect-by-name-cells '())

;;; verilog-reset-feature: reset feature settings back to defaults
(define verilog-reset-features (lambda ()
	(set! verilog-print-prototypes #t)
	(set! verilog-included-cell-types '())
	(set! verilog-excluded-cell-names '())
	(set! verilog-simulation-initialise #f)
	(set! verilog-replace-feedthroughs #f)
	(set! verilog-top-level-cell #f)
	(set! verilog-connect-by-name-cells (make-hash-table 57))

	; This is just a convenient place to put this initialisation
	(set! verilog-feedthrough-cells (make-hash-table 57))
))

;;; verilog-cell-feedthrough-substitutions: returns a port substitution list
;;;		if this cell is a feedthrough cell or #f otherwise
(define verilog-cell-feedthrough-substitutions (lambda (cell-name)
	(hash-ref verilog-feedthrough-cells cell-name)
))

;;; verilog-write-netlist-file: write a netlist file `filename' for circuit(s) `top-circuits' using
;;;		`circuits' provides references for sub-circuit components. `top-circuits' can either
;;;		be a single circuit or a list of circuits.
;;;		Supports the following features:
;;;			(included-cell-types "type1" "type2" ...) => emit definitions of cells of these
;;;				types as well as the top level cells.
;;;			(emit-prototypes . bool) => choose whether to emit prototypes for used but not defined cells
;;;			(simulation-initialise) => respect simulation-initialise attributes and output behavioural
;;;				code to initialise signal values
;;;			(replace-feedthroughs) => remove feedthrough cells (those with feedthrough attributes) with
;;;				`assign' commands
(define verilog-write-netlist-file (lambda (circuits top-circuits design-name filename . features)
	(let* ((out (open-output-file filename)))
		(net-add-file-to-session-file-list filename)
		(verilog-reset-features)
		(for-each (lambda (feature)
			(if (not (pair? feature))
				(error "verilog-write-netlist-file: features must be (name . value) pairs")
				(case (car feature)
					((emit-prototypes) (set! verilog-print-prototypes (cdr feature)))
					((included-cell-types) (set! verilog-included-cell-types (cons "none" (cdr feature))))
					((excluded-cell-names) (set! verilog-excluded-cell-names (cdr feature)))
					((simulation-initialise) (set! verilog-simulation-initialise #t))
					((replace-feedthroughs) (set! verilog-replace-feedthroughs #t))
					((top-level-cell) (set! verilog-top-level-cell (string-append "Balsa_" (verilog-map-name (cadr feature)))))
					((global-nets) (set! verilog-global-nets (cdr feature)))
					(else (print-err "verilog-write-netlist-file: unrecognised feature `" feature "'" #\newline))
				)
			)
		) features)
		(verilog-print-header out filename)
		(let*
			((top-circuits-list (if (headed-list? top-circuits 'circuit) (list top-circuits) top-circuits))
			 (all-circuits (net-flatten-circuit-defns top-circuits-list circuits verilog-included-cell-types))
			 ;;; print-circuits: print prototype and defn circuits.
			 (print-circuits (lambda (port)
				(if tech-vcc-component-name
					(verilog-maybe-print-circuit-definition out
						(net-find-expected-circuit circuits tech-vcc-component-name))
				)
				(if tech-gnd-component-name
					(verilog-maybe-print-circuit-definition out
						(net-find-expected-circuit circuits tech-gnd-component-name))
				)
				(for-each (lambda (top-circuit/defn)
					(verilog-maybe-print-circuit-definition port (car top-circuit/defn))
				) all-circuits)
			 ))
			)
			(print-circuits out)	
		)
		(close-output-port out)
	)
))

;;; verilog-write-pin-file: write a pin defn. file for the circuits given in `circuits'
;;;		port types are determined from suffices.
(define verilog-write-pin-file (lambda (circuits filename)
	(let
		((out (open-output-file filename)))
		(net-add-file-to-session-file-list filename)
		(for-each (lambda (circuit)
			(verilog-write-pin-file-entry out circuit)
		) circuits)
		(close-output-port out)
	)
))
