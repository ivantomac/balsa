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
;;;	`gen-hcs.scm'
;;;	`gen' stage of balsa-netlist back end - Handshake component generation
;;;

(balsa-scheme-import 'net 'parser)
(balsa-scheme-import 'net 'connections)
(balsa-scheme-import 'gen 'gates)

;;; gen-bundle-bindings->net-nets: map a list of specified gen-bundle-bindings
;;;		into a list of net-net elements
(define gen-bundle-bindings->net-nets (lambda (bindings)
	(append-map!
		(lambda (node)
			(let
				((name (gen-bundle-binding:name node))
				 (cardinality (gen-bundle-binding:cardinality node))
				 (low-index (gen-bundle-binding:low-index node))
				 (count (gen-bundle-binding:count node))
				 (bundle-width-list (gen-bundle-binding:width-list node))
				 (flags (gen-bundle-binding:flags node))
				)
				; (print name " " low-index " " count " " bundle-width-list #\newline)
				(if bundle-width-list
					(reverse! (fold (lambda (index card nets)
						(if (zero? card)
							nets
							(cons (cons* (tech-bundle-name 'node name index) card flags) nets)
						)
					) '() (.. low-index (+ -1 low-index count)) bundle-width-list))
					(map (lambda (index)
						; FIXME, check connections, req/ack
						(cons* (tech-bundle-name 'node name index) cardinality flags)
					) (.. low-index (+ -1 low-index count)))
				)
			)
		) bindings
	)
))

;;; gen-make-hc-netlist: make a .net netlist for the named handshake component
;;;		(a generating description of which should be present in the primitives description file)
;;;		by applying the given parameters.
(define gen-make-hc-netlist (lambda (name params type-context)
	(let*
		((component (brz-find-primitive name)) 
		 (parameters (brz-normalise-parameter-list params))
		 ; formal-parameters: if the actual parameters list for this part is shorter
		 ;	than the formal parameters list then assume that only significant parameters
		 ;	were specified and shorten the formal-parameters list appropriately.
		 (formal-parameters
			(let 
				((fps (brz-primitive-part:parameters component))
				 (is-significant-formal-param? (lambda (param) (not (memv 'not-used (cddr param)))))
				)
				(if (/= (length (cdr fps)) (length parameters)) ;; get rid of 'params header
					(cons 'parameters (filter is-significant-formal-param? (cdr fps)))
					fps
				)
		 	)
		 )
		 (ports (brz-primitive-part:ports component))
		 (specified-ports (brz-specify-part-ports ports parameters formal-parameters))
		 (implementations (brz-primitive-part:implementation component))
		; Find the appropriate technology (allow 'technology and 'style)
		 (tech-style (find-headed-list-elem (cdr implementations) 'style breeze-style))
		 (tech (if tech-style tech-style
			 (find-headed-list-elem (cdr implementations) 'technology breeze-style)))
		 (error-check-1 (if (not tech)
		 	(error (string-append "gen-make-hc-netlist: can't find component definition for `"
		 		name "' in style `" breeze-style "'"))
		 ))
		 (nodes (brz-implementation-style:nodes tech))
		; Add a definition for the current component name
		 (pre-defines-1 (cons 
			(list 'component-name (brz-primitive-part:name component))
			breeze-primitives-definitions
		 ))
		 (defines (let ((defines (brz-implementation-style:defines tech)))
			(if defines
				(fold (lambda (def existing-defs)
					(cons (list (car def)
						(brz-specify-expression (cadr def) existing-defs parameters formal-parameters))
						existing-defs
					)
				) pre-defines-1 (cdr defines))
				pre-defines-1
			)
		 ))
		; specify-expr: specify an expression using the current parameters
		 (specify-expr (lambda (expr) (brz-specify-expression expr defines parameters formal-parameters)))
		 (specified-nodes (reverse! (fold (lambda (node acc)
		 	(let*
				((node-name (specify-expr (car node)))
				 (card (specify-expr (cadr node)))
				 (low-index (specify-expr (caddr node)))
				 (count (specify-expr (cadddr node)))
				 (width-list 
					(if (> (length node) 4)
						; AB 2008-08-06 (list (map specify-expr (list-ref node 4)))
						(list (specify-expr (list-ref node 4)))
						'()
					)
				 )
				 (new-node (cons* node-name card low-index count width-list))
				)
				(if (not (zero? card))
					(cons
						(cons* (car node) card low-index count width-list)
						acc
					)
					acc
				)
			)
		 ) '() (cdr nodes))))
		 (abs-gates (brz-implementation-style:gates tech))
		 (no-abs-gates (or (not abs-gates) (null? (cdr abs-gates))))
		 (abs-connections (brz-implementation-style:connections tech))
		 (no-abs-connections (or (not abs-connections) (null? (cdr abs-connections))))
		 (port-bindings (gen-brz-ports->gen-bundle-bindings specified-ports type-context))
		 (bindings (append port-bindings specified-nodes))
		; pre-defines: predefined definitions.  HC entry
		; defns (eg. lambdas with prebound contexts)
		; FIXME, combine these with defines-1 some time
		 (pre-defines-2
			(list
				(list 'normalise-slices
					(brz-make-lambda-from-scheme-lambda (lambda slices (concatenate! (gen-expand-slices
						(flatten-unheaded-list slices) bindings defines parameters formal-parameters)))
						breeze-primitives-definitions)
				)
			)				
		 )
		 (all-defines (append pre-defines-2 defines))
		; new-bindings/expanded-abs-gates: expanded gates and internal nodes generated therein
		 (new-bindings/expanded-abs-gates (cdr
			(fold (lambda (abs-gate ret)
				(let ; call self on each gate and carry over the internal-name-index
					((i/b/g (gen-expand-abs-gate abs-gate all-defines parameters formal-parameters bindings
						"int" (car ret))))
					(if i/b/g
						(cons* (car i/b/g) (append (cadr i/b/g) (cadr ret)) (append (cddr i/b/g) (cddr ret)))
						ret
					)
				)
			) '(0 () . ()) (cdr abs-gates))
		 ))
		 (new-bindings (car new-bindings/expanded-abs-gates))
		 (expanded-abs-gates (cdr new-bindings/expanded-abs-gates))
		; connection-gates: gates and new nodes generated for the (connections ...) desc.
		 (connection-gates (if no-abs-connections '()
			(fold (lambda (abs-gate ret)
				(let ((i/b/g (gen-expand-abs-gate abs-gate defines parameters formal-parameters
					bindings "int" abs-gate)))
					(if i/b/g
						(let*
							((expanded-abs-gates (cddr i/b/g))
							 (simple-gates (append-map
								(lambda (abs-gate) (cdr (gen-abs-gate->simple-gates abs-gate "" 0 0)))
								expanded-abs-gates
							 ))
							 (default-gates (map
							 	(lambda (gate) (gen-abs-gate->net-gate (gen-simple-gate->default-gate gate)))
								simple-gates))
							)
							(append! ret default-gates)
						)
						ret
					)
				)
			) '() (cdr abs-connections))
		 ))
		 ; FIXME, connection-bindings should be null
		; expanded-nodes: node defns. within the gates cell
		 (expanded-nodes (gen-bundle-bindings->net-nets (append specified-nodes new-bindings)))
		 ; Generate default target technology gates for the circuit.  Use the name `internal' for internal nodes.
		 (last-internal-index/default-gates (foldl-ma (lambda (gate internal-base-index prev-gates)
			(let*
				((next-internal/gates (gen-abs-gate->simple-gates gate "internal" 0 internal-base-index))
			 	 (gates (map gen-simple-gate->default-gate (cdr next-internal/gates)))
				)
				(list (car next-internal/gates) (append prev-gates (map gen-abs-gate->net-gate gates)))
			)
		 ) expanded-abs-gates 0 '()))
		 ; net-{ports,nets}: ports and nets in final Brz netlist
		 (net-ports (gen-brz-ports->net-ports specified-ports type-context #f))
		 (net-nets (if (zero? (car last-internal-index/default-gates))
			expanded-nodes
			(cons (list (tech-bundle-name 'node "internal" 0) (car last-internal-index/default-gates))
				expanded-nodes
			)
		 ))
		 (mangled-name (tech-map-cell-name (tech-mangle-hc-name name parameters)))
		 (make-circuit-decl (lambda (name ports nets instances)
			(list 'circuit name (cons 'ports ports) (cons 'nets nets) (cons 'instances instances))
		 ))
		)
		(list (make-circuit-decl mangled-name net-ports net-nets
			(append
				(cadr last-internal-index/default-gates)
				connection-gates
			)
		))
	)
))

;;; gen-make-hc-header: make a .net netlist for the named handshake component.
;;;		This netlist should have only name and ports fields set and be the minimum necessary
;;;		netlist to define the external interface of the component.
(define gen-make-hc-header (lambda (name mangled-name parameters type-context)
	(let*
		((component (brz-find-primitive name)) 
		 (formal-parameters (brz-primitive-part:parameters component))
		 (ports (brz-primitive-part:ports component))
		 (specified-ports (brz-specify-part-ports ports parameters formal-parameters))
		)
		(list 'circuit
			mangled-name
			(cons 'ports (gen-brz-ports->net-ports specified-ports type-context #f))
			(list 'nets) (list 'instances)
		)
	)
))

;;; gen-make-hc-other-files: make miscellaneous/EDA system
;;;		dependent files.  `filename-stub' will be a directory/filename w/o
;;;		an extension
(define gen-make-hc-other-files (lambda
	(name mangled-name parameters type-context filename-stub)
	(let*
		((component (brz-find-primitive name)) 
		 (formal-parameters (brz-primitive-part:parameters component))
		 (ports (brz-primitive-part:ports component))
		 (specified-ports (brz-specify-part-ports ports parameters formal-parameters))
		)
		(if tech-write-symbol-file
			(tech-write-symbol-file name mangled-name specified-ports filename-stub)
		)
	)
))

;;; gen-brz-ports->name/index-list: make a vector of (portname . portVectorIndex) from a list of
;;;		brz ports.  Single ports (port/sync-port) become single vector elements and arrayed ports
;;;		become elements from index 0 upto port-count-1. If buffered is true appends "_buf" to each
;;; 	name to allow top-level ports to be buffered.
(define gen-brz-ports->name/index-list (lambda (ports buffered)
	(letrec
		((get-name (lambda (port) (if buffered (string-append (brz-port:name port) "_buf") (brz-port:name port))))
		 (make-arrayed-ports (lambda (name low-index port-count tail-of-list)
			(let tail
				((index low-index)
				 (ports tail-of-list)
				)
				(if (= index (+ low-index port-count))
					ports
					(tail (+ 1 index) (cons (cons name index) ports))
				)
			)
		)))
		(let
			((port-list
				(reverse! (fold (lambda (port names/indices)
					(case (car port)
						((sync-port port) (cons (cons (get-name port) 0) names/indices))
						((arrayed-port)
							(make-arrayed-ports (get-name port)
								(brz-arrayed-port:low-index port) (brz-arrayed-port:port-count port) names/indices)
						)
						((arrayed-sync-port)
							(make-arrayed-ports (get-name port)
								(brz-arrayed-sync-port:low-index port) (brz-arrayed-sync-port:port-count port)
									names/indices)
						)
					)
				) '() (if (headed-list? 'ports ports) (cdr ports) ports)))
			))
			(list->vector port-list)
		)
	)
))

;;; gen-buffer-ports: produce a list of net buffer instances to buffer each port. Port-nets is a list
;;; 	of .net port descriptions and channel-nets is a list of .net channel descriptions for the internal
;;; 	buffered channels
(define gen-buffer-ports (lambda (port-nets channel-nets)
	(let*
		((gate-desc (cadr (assoc "buf" breeze-gate-mappings)))
		 (buffer (car gate-desc))
		 (mapping (cdr gate-desc))
		 (mapping-fn (lambda (ports)
			(map (lambda (index)
				(if (eqv? 'unconnected index) 'unconnected (list-ref ports index))
			) mapping)	
		 ))
		)
		(fold (lambda (port channel instances)
			(let
				((port-name (net-port:name port))
				 (channel-name (net-net:name channel))
				 (direction (net-port:direction port))
				 (width (net-port:cardinality port))
			  	)
				(append instances
					(map.. (lambda (index)
						(list 'instance buffer
							(mapping-fn 
								(case direction ;; buffer - dst, src
									((input) 
										(list (list channel-name index) (list port-name index))
									)
									((output) 
										(list (list port-name index) (list channel-name index))
									)
									(else ;;; uh, don't know what to do with inouts -lets treat them as inputs...
										(list (list channel-name index) (list port-name index))
									)
								)
							)
						)
					) 0 (- width 1))
				)			 
			)
		) '() port-nets channel-nets)
	)
))

;;; gen-make-breeze-part-netlist: make a .net netlist for a given breeze part which expresses the
;;;		same handshake component connectivity
(define gen-make-breeze-part-netlist (lambda (procedure type-context buffered with-completion-signals with-error-signals)
	(let*
		((channels (brz-breeze-part:channels procedure))
		 (channel-count (length (cdr channels)))
		 (ports (cdr (brz-breeze-part:ports procedure)))
		 (port-/channel-nets (gen-brz-ports->net-ports ports type-context buffered))
		 (port-nets (if buffered (car port-/channel-nets) port-/channel-nets))
		 (buffered-channel-nets (if buffered (cdr port-/channel-nets) '()))
		 ; vector of (port-name . index) pairs for each real port eg.
		 ; sync a; array 2 of sync b => #((a . 0) (b . 0) (b . 1))
		 (unvectored-ports (gen-brz-ports->name/index-list ports buffered))
		 (buffered-instances (if buffered (gen-buffer-ports port-nets buffered-channel-nets) '()))
		 (port-count (vector-length unvectored-ports))
		 (port-indices (let ((vec (make-vector channel-count #f)))
		 	(for-each (lambda (port-no)
		 		(vector-set! vec (- port-no 1) port-no)
		 	) (.. 1 port-count))
		 	vec
		 ))
		 (channel-name/index (list->vector (map (lambda (channel channel-no)
		 	(let ((port-no (vector-ref port-indices (- channel-no 1))))
				(if port-no
					(vector-ref unvectored-ports (- port-no 1))
					(cons 
						(let ((defined-name (find-headed-sub-list channel 'net-name)))
							(if defined-name
								(cadr defined-name)
								(string-append "c" (number->string channel-no))
							)
						)
						#f
					)
				)
			)
		 ) (cdr channels) (.. 1 channel-count))))
		 (channel-name->number-hash
		 	(let ((hash (make-hash-table)))
		 		(for-each (lambda (channel-no channel)
		 			(let ((name/index (vector-ref channel-name/index (- channel-no 1))))
		 				(hash-set! hash (car name/index) channel-no)
		 				; Insert net-names as well as port names into hash
						(let ((defined-name (find-headed-sub-list channel 'net-name)))
							(if defined-name (hash-set! hash (cadr defined-name) channel-no))
						)
		 			)
		 		) (.. 1 channel-count) (cdr channels))
		 		hash
		 	)
		 )
		 (channel-name->number (lambda (channel-no)
		 	(cond
		 		((or (string? channel-no) (symbol? channel-no))
		 			(let*
		 				((channel-no-string (->string-rep channel-no))
		 				 (real-channel-no (hash-ref channel-name->number-hash channel-no-string))
		 				)
		 				(if (not real-channel-no)
		 					(error (string-append "gen-make-breeze-part-netlist: unrecognised channel `"
		 						channel-no-string "'"))
		 					real-channel-no
		 				)
		 			)
		 		)
		 		((integer? channel-no) channel-no)
		 		(else
		 			(error (string-append "gen-make-breeze-part-netlist: unrecognised channel `"
		 				(->string channel-no) "'"))
		 		)
			)
		 ))
		 ; channel-name: a wrapper to allow us to keep port names.  Uses channel-name/index for data
		 (channel-name (lambda (portion channel-no)
		 	(let*
		 		((real-channel-no (channel-name->number channel-no))
				 (name/index (vector-ref channel-name/index (- real-channel-no 1))))
				(tech-bundle-name portion (car name/index) (cdr name/index))
		 	)
		 ))
		 (portion-for-channel-sense (lambda (sense)
			(case sense
				((push) tech-push-channel-portions)
				((pull) tech-pull-channel-portions)
				((sync) tech-sync-channel-portions)
			)
		 ))
		 (non-port-channels (list-tail (cdr channels) port-count))
		 ; channel-nets: local channels expanded into (nets ...) elements
		 (channel-nets (car (foldl-ma (lambda (channel nets index)
			(list
				(let*
					((portions (portion-for-channel-sense (car channel)))
					 (channel-has-width (and (> (length channel) 1) (number? (cadr channel))))
					 ; turn portion lists into channel wires
					 (processed-portions
						(reverse! (fold (lambda (portion acc)
							(let
								((width (if channel-has-width
									((caddr portion) (cadr channel))
									((caddr portion) 1)
								)))
								(if width 
									(cons (list (channel-name (car portion) index) (if (zero? width) 1 width)) acc)
									acc
								)
							)
						) '() portions))
					 )
					)
					(append! processed-portions nets)
				)
				(+ 1 index)
			)
		 ) non-port-channels '() (+ 1 port-count))))
		 ; channel-cardinalities: a vector of all the channel widths
		 (channel-cardinalities (list->vector (map (lambda (channel)
			(case (car channel) ((push pull) (cadr channel)) ((sync) 0))) (cdr channels))))
		 ; channel-sense : a vector of all the channel senses
		 (channel-senses (list->vector (map car (cdr channels))))
		 ; channel-portions: connections for channel number channel-no
		 (channel-portions (lambda (channel-no)
			(let*
		 		((real-channel-no (channel-name->number channel-no))
				 (cardinality (vector-ref channel-cardinalities (- real-channel-no 1)))
				 (portions (portion-for-channel-sense (vector-ref channel-senses (- real-channel-no 1))))
				)
				(reverse! (fold (lambda (portion acc)
	 				(let
	   					((width ((caddr portion) cardinality)))
	   					(cond
							((not width) acc)
							((zero? width) (cons (channel-name (car portion) channel-no) acc))
							(else (cons (list (channel-name (car portion) channel-no) 0 width) acc))
						)
					)
				) '() portions))
			)
		 ))
		 (instances (fold (lambda (hc hc-list)
			(let*
				((instance-name (find-headed-sub-list hc 'name))
				 (instance-props (if instance-name (list (cadr instance-name)) '()))
				 (name (brz-get-primitive-part-name (cadr hc))))
				(if name
					(let* ; primitive parts
						((hc-defn (brz-find-primitive name))
						 (formal-parameters (brz-primitive-part:parameters hc-defn))
						 (actual-parameters (brz-normalise-parameter-list (caddr hc)))
						 (significant-parameters (gen-extract-significant-parameters
							(cdr formal-parameters) actual-parameters))
						 (mangled-name (tech-map-cell-name (tech-mangle-hc-name name significant-parameters)))
						 (flat-ports (flatten-list (cadddr hc)))
						)
						(cons (cons* 'instance
							mangled-name
							(append-map! channel-portions flat-ports)
							instance-props
						) hc-list)
					)
					(case (car hc)
						((component)
							(let* ; Non primitive part, (component name () ports)
								((mangled-name (tech-map-cell-name (tech-mangle-breeze-part-name (cadr hc))))
								 (flat-ports (flatten-unheaded-list (cadddr hc)))
								)
								(cons (cons* 'instance
									mangled-name
									(append-map! channel-portions flat-ports)
									instance-props
								) hc-list)
							)
						)
						((undeclared-component)
							(let* ; Non primitive part, (undeclared-component some-name params actual-ports
								; (ports formal-ports) ...)
								((mangled-name
									(tech-map-cell-name (tech-mangle-builtin-function-name (cadr hc) (caddr hc))))
								 (flat-ports (flatten-unheaded-list (cadddr hc)))
								)
								(cons (cons* 'instance
									mangled-name
									(append-map! channel-portions flat-ports)
									instance-props
								) hc-list)
							)
						)
					)
				)
		 	)
		 ) '() (cdr (brz-breeze-part:components procedure))))
		 ; (with-completion-signals #t)
		 (completion-net-name (lambda (channel-no) (string-append "c" (number->string channel-no) "comp")))
		 (completion-nets (if with-completion-signals
		 	(map (lambda (i) (list (completion-net-name i) 1)) (.. 1 channel-count))
		 	'()
		 ))
		 (error-net-name (lambda (channel-no) (string-append "c" (number->string channel-no) "error")))
		 (error-nets (if with-error-signals
		 	; (map (lambda (i) (list (error-net-name i) 1)) (.. 1 channel-count))
		 	; '()
		 	(reverse! (fold (lambda (i nets)
		 		; (print channel-senses "\n")
		 		; (print channels  "\n")
				(if (/= 0 (vector-ref channel-cardinalities (- i 1)))
					(cons (list (error-net-name i) 1) nets)
					nets
		 		)
		 	) '() (.. 1 channel-count)))
		 	'()
		 ))
		 (completion-instances (if with-completion-signals
		 	(fold (lambda (channel-no instances)
		 		; (print channel-senses "\n")
		 		; (print channels  "\n")
		 		(case (vector-ref channel-senses (- channel-no 1))
		 			((push)
		 				(cons
		 					(list 'shdl-assign
		 						; (list 'wait-until-rising (channel-name 'req1 channel-no))
		 						(completion-net-name channel-no)
								(if (= 0 (vector-ref channel-cardinalities (- channel-no 1)))
									(channel-name 'req channel-no)
									(string-append "&(" (channel-name 'req0 channel-no)
										" | " (channel-name 'req1 channel-no) ")")
								)
		 					)
		 					instances
		 				)
		 			)
		 			(else instances)
		 		)
		 	) '() (.. 1 channel-count))
		 	'()
		 ))
		 (error-instances (if with-error-signals
		 	(fold (lambda (channel-no instances)
		 		; (print channel-senses "\n")
		 		; (print channels  "\n")
		 		(case (vector-ref channel-senses (- channel-no 1))
		 			((push)
						(if (/= 0 (vector-ref channel-cardinalities (- channel-no 1)))
							(cons
								(list 'shdl-assign
									; (list 'wait-until-rising (channel-name 'req1 channel-no))
									(error-net-name channel-no)
									(string-append "|(" (channel-name 'req0 channel-no)
										" & " (channel-name 'req1 channel-no) ")")
								)
								instances
							)
							instances
		 				)
		 			)
		 			(else instances)
		 		)
		 	) '() (.. 1 channel-count))
		 	'()
		 ))
		)
		(list 'circuit
			(tech-map-cell-name (tech-mangle-breeze-part-name (brz-breeze-part:name procedure)))
			(cons 'ports port-nets)
			(cons 'nets (append completion-nets error-nets buffered-channel-nets channel-nets))
			(cons 'instances (append (reverse! instances) buffered-instances completion-instances error-instances))
		)
	)
))
