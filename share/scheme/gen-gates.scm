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
;;;	`gen-gates.scm'
;;;	Gate generation
;;;

(balsa-scheme-import 'gen)

; (brz-load-primitives)
; (brz-load-gates)

;;; gen-tree-invert-gate: return the inverted form of the given gate
(define gen-tree-invert-gate (lambda (gate)
	(case gate
		((and) 'nand)
		((nand) 'and)
		((or) 'nor)
		((nor) 'or)
		((connect) 'inv)
		((inv) 'connect)
	)
))

;;; gen-tree-single-input-gate: return the single input version of the
;;;		given gate.  This will typically be either `connect' or `inv'
(define gen-tree-single-input-gate (lambda (gate)
	(case gate
		((and or connect c-element) 'connect)
		((nand nor inv) 'inv)
	)
))

;;; gen-tree-first/rest-gate: to produce the many-input gate `gate' which type of
;;;		gate should we use in the first level of the tree and what logical function
;;;		should the remainder of the tree have. eg. and => ('nand . 'nor)
;;;		Supported gates: and nand or nor c-element
(define gen-tree-first/rest-gate (lambda (gate)
	(case gate
		((and) '(nand . nor)) ; and: nand->nor == and->and
		((nand) '(nand . or)) ; nand: nand->or == and->nand
		((or) '(nor . nand))  ; or: nor->nand == or->or
		((nor) '(nor . and))  ; nor: nor->and == or->nor
		((c-element) '(c-element . c-element)) ; c-element, non inverting tree
	)
))

;;; gen-make-many-input-gate: produce a list of (gate-type . singleton-slice-list) type gate descriptions required to
;;;		combine the input slices `input-wires' into the single wire slice `output-wire' by the logical function `gate'.
;;;		Internal wires are produced using the name `internal-wire-prefix' and are indexed based at
;;;		`internal-base-index' (`internal-wire-bundle-index' is the index portion of the produced internal wire slices).
;;;		The produced gates will consist of only gates with `max-gate-fan-in' inputs and fewer.
;;;		Supported gate functions are those supported by gen-tree-first/rest-gate
;;;		Returns a pair: (next-free-internal-wire-index . list-of-gates) giving the required gates and an indication of
;;;		the next internal wire number which can be used by another gate generator.
(define gen-make-many-input-gate (lambda
	(gate input-wires output-wire internal-wire-prefix internal-wire-bundle-index internal-base-index
		max-gate-fan-in)
	(let*
		((rank/rest-gate (gen-tree-first/rest-gate gate))
		 (first-gate-type (car rank/rest-gate))
		 (rest-gate-type (cdr rank/rest-gate))
		 ; fan-in: count of all bits in input slices
		 (fan-in (fold (lambda (l acc) (+ acc (gen-slice-cardinality l))) 0 input-wires))
		 ; common-gate...: number/fan-in... of the first rank multiinput gates
		 ; take away 2 from fan-in to allow for last gate stuff
		 (common-gates-count (quotient (- fan-in 2) max-gate-fan-in))
		 (common-gate-fan-in (min fan-in max-gate-fan-in)) 
		 ; fan-in of the residue gate/gates
		 (last-gate-fan-in (- fan-in (* common-gates-count common-gate-fan-in)))
		 (last-gates-count (cond
			((zero? last-gate-fan-in) 0)
			((<= last-gate-fan-in max-gate-fan-in) 1)
			((> last-gate-fan-in max-gate-fan-in) 2)
		 ))
		 (gate-count (+ common-gates-count last-gates-count))
		 ; input-wire/internal-wire/make-gate helper functions
		 (input-wire (lambda (index) (gen-extract-slices-bit-at-index input-wires index)))
		 (internal-wire (lambda (index) (list 'node internal-wire-prefix internal-wire-bundle-index
			(+ internal-base-index index) 1)))
		 (make-gate (lambda (type internal-output-index input-low-index input-count)
			(cons (if (= 1 input-count) (gen-tree-single-input-gate type) type)
				(cons (internal-wire internal-output-index) (map
					(lambda (input-index) (input-wire input-index))
					(.. input-low-index (+ input-low-index input-count -1))
				))
			)
		 ))
		 ; common `largest' gate used for the majority of inputs
		 (common-gates (map (lambda (index)
			(let ((base-input-index (* index common-gate-fan-in)))
				(make-gate first-gate-type index base-input-index common-gate-fan-in)
			)
		 ) (.. 0 (- common-gates-count 1))))
		 ; gates to combine the residue of the inputs
		 (last-gates ; have a close look at this
			(cond
				((zero? last-gate-fan-in) '())
				; =1 : this shouldn't happen (Hmm), assume we need a single input gate
				((= 1 last-gate-fan-in) (list (list (gen-tree-single-input-gate gate)
					(internal-wire (- gate-count 1)) (input-wire (- fan-in 1)))) )
				(else ; 1 or 2 gates
					(let*
						; fan-in of larger of the residue gates
						((large-gate-fan-in (if (= last-gates-count 1)
							last-gate-fan-in
							(- last-gate-fan-in 2)
						 ))
						 (large-gate (make-gate first-gate-type (- gate-count 1) (- fan-in large-gate-fan-in)
							large-gate-fan-in) )
						 (two-input-gate (delay (make-gate first-gate-type (- gate-count 2)
							(- fan-in last-gate-fan-in) 2)) )
						)
						(if (= last-gates-count 1) ; just one gate
							(list large-gate)
							(list (force two-input-gate) large-gate)
						)
					)
				)
			)
		 )
		 (gates (append common-gates last-gates))
		 (intermediate-slice (list 'node internal-wire-prefix 0 internal-base-index gate-count))
		)
		(if (= 1 gate-count)
			; combine in the output-wire
			(cons (+ -1 internal-base-index gate-count)	
				(list (cons
					(if (= 1 fan-in) ; single input gate equavalent to gate
						(gen-tree-single-input-gate gate)
						gate
					)
					(cons output-wire (cddar gates))
				))
			)
			(let
				((rest-of-gate (gen-make-many-input-gate
					rest-gate-type
					(list intermediate-slice) output-wire internal-wire-prefix internal-wire-bundle-index
					(+ internal-base-index gate-count)
					max-gate-fan-in
				)))
				(cons (car rest-of-gate) (append gates (cdr rest-of-gate)))
			)
		)
	)
))

;;; gen-abs-gate->simple-gates: make a list of simple gates (and,or... with realistic numbers of inputs) which
;;;		implement the given (connection expanded) abs gate. `internal...' identify the bundle name to use
;;;		for internal signals.  This procedure returns a pair (next-internal-wire-index . generated-gates)
(define gen-abs-gate->simple-gates (lambda (gate internal-wire-prefix internal-wire-bundle-index internal-base-index)
	(let
		((smash-slices (lambda (slices) (fold (lambda (slice outs)
			(append outs (gen-smash-simple-slice slice))) '() slices)
		 ))
		)
		(case (car gate)
			((and nand or nor c-element) ; make a gate tree
				(gen-make-many-input-gate (car gate) (cddr gate) (cadr gate)
					internal-wire-prefix internal-wire-bundle-index internal-base-index
					(if (eq? (car gate) 'c-element)
						tech-c-element-max-fan-in
						tech-gate-max-fan-in
					)
				)
			)
			((connect) ; buffers from input wire to each output wire
				(cons internal-base-index
					(map (lambda (out)
						(list 'connect out (cadr gate))
					) (smash-slices (cddr gate)))
				)
			)
			((vcc gnd)
				(cons internal-base-index
					(let ((rail (if (eqv? 'vcc (car gate)) tech-vcc-net-name tech-gnd-net-name))) 
						(map (lambda (out) (list 'connect out `(node ,rail 0 0 1))) (smash-slices (cdr gate)))
					)
				)
			)
			; non-tree gates
			((inv s-element latch latch-nenable
				tri-buffer tri-inv c-element2-sp keeper-inv mutex demux2 mux2 xor2 xnor2 adder)
				(cons internal-base-index (list gate)))
			; arbitrary helper cells
			((cell)
				(cons internal-base-index (list gate))
			)
			((constant)
				(let*
					((wires (smash-slices (cddr gate)))
					 (output-width (fold (lambda (slice card) (+ card (gen-slice-cardinality slice))) 0 (cddr gate)))
					 (value (cadr gate))
					 (extend-bit (->boolean (> 0 value)))
					 (abs-value (if extend-bit (+ (expt 2 output-width) value) value))
					 (bits (integer->binary-list abs-value output-width))
					)
					(cons internal-base-index
						(map (lambda (wire bit)
							(list 'connect wire (list 'node
								(if bit tech-vcc-net-name tech-gnd-net-name) 0 0 1))
						) wires bits)
					)
				)
			)
			((shdl shdl-repeat)
				(cons internal-base-index (list gate))
			)
			(else (cons internal-base-index '()))
		)
	)
))

;;; gen-simple-gate->default-gate: map simple back end gates into technology specific gates using the
;;;		`default' gate for each gate type.
(define gen-simple-gate->default-gate (lambda (gate)
	(case (car gate)
		((shdl shdl-repeat)
			gate
		)
		(else
			(let*
				((is-helper (eq? (car gate) 'cell)) ; is this gate a helper cell: (cell "helper-name" connections...)
				 (many-to-one-gate (memv (car gate) '(and nand or nor c-element)))
				 (gate-name (if is-helper (cadr gate)
					(case (car gate)
						((and) "and")
						((nand) "nand")
						((or) "or")
						((nor) "nor")
						((c-element) "c-element")
						((c-element2-sp) "c-element2-sp")
						((keeper-inv) "keeper-inv")
						((s-element) "s-element")
						((latch) "latch")
						((latch-nenable) "latch-nenable")
						((tri-buffer) "tri-buffer")
						((tri-inv) "tri-inv")
						((inv) "inv")
						((connect) "connect")
						((mutex) "mutex")
						((demux2) "demux2")
						((mux2) "mux2")
						((xor2) "xor2")
						((xnor2) "xnor2")
						((adder) "adder")
					 )
				 ))
				 (connections (if is-helper (cddr gate) (cdr gate)))
				 (full-gate-name (if many-to-one-gate
					(string-append gate-name (number->string (- (length gate) 2))) ; make (for example) and into and2
					gate-name
				 ))
				 (real-gate-description (assoc full-gate-name breeze-gate-mappings))
				 (default-gate-description
					(if real-gate-description
						(cadr real-gate-description)
						(error "gen-simple-gate->default-gate: can't find description for gate type `"
							full-gate-name "'")
					)
				 )
				)
				(let ; map pins from abs to real gate
					((mapped-pins (map (lambda (index)
						(if (eqv? 'unconnected index)
							'unconnected
							(list-ref connections index)
						)
					) (cdr default-gate-description))))
					(cons
						(car default-gate-description)
						mapped-pins
					)
				)
			)
		)
	)
))

(define gen-handle-connection (lambda (connection)
	(if (eqv? connection 'unconnected) ; handle unconnected ports, ret. 'unconnected
		'unconnected
		(let
			((name (apply tech-bundle-name (take connection 3)))
			 (portion (find-headed-list-elem all-portions (car connection) #f))
			)
			; if this is a singleton portion, just include the name
			(if (gen-slice-indexed? connection)
				(cons name ; (data/... name low-index [highIndex])
					(cons (gen-slice:low-bit-index connection)
						(if (= 1 (gen-slice:cardinality connection))
							'()
							(list (gen-slice:cardinality connection))
						)
					)
				)
				name
			)
		)
	)
))

;;; gen-abs-gate->net-gate: transform a gate of the format (name singleton-slice ...) into the
;;;		format used by .net files. ie. ('instance name (instance-connection ...))
;;;		NB. `name' may be either a symbol or string
(define gen-abs-gate->net-gate (lambda (gate)
	(case (car gate)
		((shdl shdl-repeat)
			(let
				((shdl-handle-connection (lambda (conn)
					(list 'tech-conn (gen-handle-connection conn))))
				)
				(list-replace-headed gate 'net-conns (lambda (conns)
					(if (= (length conns) 1)
						(shdl-handle-connection (car conns))
						(cons 'vector (map shdl-handle-connection conns))
					)
				))
			)
		)
		(else
			(list 'instance (if (symbol? (car gate)) (symbol->string (car gate)) (car gate))
				(map gen-handle-connection (cdr gate))
			)
		)
	)
))

;;; gen-brz-ports->net-ports: transform breeze file port structures to
;;;		a list of req/ack/data expanded ports suitable for inclusion in
;;;		a .net file
(define gen-brz-ports->net-ports (lambda (ports type-context buffered)
	(let*
		((unnamed-net-ports (fold (lambda (brz-port net-ports)
			(append net-ports
				(let*
					; portions: how many port fragements are there for this bundle?
					( ; low-index,count: port vector indices
					 (low-index (case (car brz-port)
						((sync-port port) 0)
						((arrayed-sync-port) (brz-arrayed-sync-port:low-index brz-port))
						((arrayed-port) (brz-arrayed-port:low-index brz-port))
					 ))
					 (count (case (car brz-port)
						((sync-port port) 1)
						((arrayed-sync-port) (brz-arrayed-sync-port:port-count brz-port))
						((arrayed-port) (brz-arrayed-port:port-count brz-port))
					 ))
					 (name (brz-port:name brz-port))
					 (sense (brz-port:sense brz-port))
					 (port-push/pull/sync
				 		(if (memv (car brz-port) '(port arrayed-port)) ; sync?
							(brz-port-push/pull (brz-port:sense brz-port) (brz-port:direction brz-port))
							'sync
				 		)
					 )
					 (portions (case port-push/pull/sync
						((push) tech-push-channel-portions)
						((pull) tech-pull-channel-portions)
						((sync) tech-sync-channel-portions)
					 ))
					 (cardinality-list (case port-push/pull/sync
						((sync) (map.. (lambda (i) 1) 1 count))
						(else 
							(let*
								((type (brz-port:type brz-port))
								 (width (brz-type-width (brz-port:type brz-port) type-context))
								)
								(case (car type)
									((variable-array-type)
										(brz-array-spec->width-list (brz-variable-array-type:spec-string type) 
											width (brz-variable-array-type:element-count type)))
									((variable-width-array-type)
										(brz-variable-width-array-type:widths type))
									(else
										(map.. (lambda (i) width) 1 count))
								)
							)
						)  ; cardinality	
					 ))
					)
					(fold (lambda (index cardinality port-fragments)
						(append port-fragments (reverse! (fold (lambda (portion port-fragments)
							(let ((width ((caddr portion) cardinality)))
								(if width
									(cons
										(list (list (car portion) name index) ; name
											(case (cadr portion) ; relative pin direction
												((push) (case sense ((active) 'output) ((passive) 'input)))
												((pull) (case sense ((active) 'input) ((passive) 'output)))
											)
											(if (not (zero? width))
												width
												1
											)
										)
										port-fragments
									)
									port-fragments
								)
							)
						) '() portions)))
					) '() (.. low-index (+ -1 low-index count)) cardinality-list)
				)
			)
	 	 ) '() ports))
		 (net-ports (map (lambda (port) (cons (apply tech-bundle-name (car port)) (cdr port))) unnamed-net-ports))
		)	
		(if buffered
			(cons 
				net-ports
				(map (lambda (port) ;; make channel 
					(let ((name-args (car port)))
						(list 
							(apply tech-bundle-name (car name-args) (string-append (cadr name-args) "_buf") (cddr name-args))
							(caddr port)
						)
					)
				) unnamed-net-ports)
			)
			net-ports
		)
	)
))

;; FIXME, think about making this cardinality conditional
;;; gen-net-port->net-connection: transform a net port into a suitable net connection
;;;		eg. (name input 4) => (name 0 4)
(define gen-net-port->net-connection (lambda (port)
	(list (net-port:name port) 0 (net-port:cardinality port))
))

;;; tech-mangle-parameters: combine brz parameters into a valid identifier suffix
(define tech-mangle-parameters (lambda (parameters)
	(build-string
		(map (lambda (param)
			(cond
				((string? param) (let ((name (tech-map-name param)))
					(string-append "s" (number->string (string-length name)) "_" name)
				))
				((number? param) (radix-string 10 param #f lowercase-radix-characters))
				((list? param) (let ((mapped-list (tech-map-name (->string param))))
					(string-append "l" (number->string (string-length mapped-list)) "_" mapped-list)
				))
				(else
					(error "Breeze parameters should only be strings, numbers, or lists: `" parameters "'") 
				)
				; ((boolean? param) (if param "bt" "bf"))
				; ((symbol? param) (case param
				;	((true) "bt") ((false) "bf")
				;	(else (tech-map-name (symbol->string param))) ; FIXME, deprecated
				; ))
			)
		) parameters)
		"_" ""
	)
))

;;; tech-mangle-hc-name: make a handshake component name
;;;		eg. ("Call" 2) => brz_call_2
(define tech-mangle-hc-name (lambda (name parameters)
	(string-append tech-brz-prefix (tech-map-name name) (tech-mangle-parameters parameters))
))

;;; tech-mangle-breeze-part-name: make a breeze part name suitable for the current tech
(define tech-mangle-breeze-part-name (lambda (name)
	(string-append tech-balsa-prefix (tech-map-name name))
))

;;; gen-extract-significant-parameters: returns a list of parameters taken from
;;;		the list `actual-parameters' for which the corresponding formal parameter in
;;;		`formal-parameters' does not have the 'not-used flag.
(define gen-extract-significant-parameters (lambda (formal-parameters actual-parameters)
	(reverse! (fold (lambda (formal actual params)
		(if (and (= 3 (length formal)) (eqv? (caddr formal) 'not-used))
			params
			(cons actual params)
		)
	) '() formal-parameters actual-parameters))
))
