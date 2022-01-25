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
;;;	`balsa-netlist.scm'
;;;	Breeze -> Netlist generator, all manner of netlist functions
;;;

(balsa-scheme-import 'brz)
(balsa-scheme-import 'brz 'tech)
(balsa-scheme-import 'net 'parser)
(balsa-scheme-import 'gen 'hcs)
(balsa-scheme-import 'misc 'switches)
(balsa-scheme-import 'misc 'banners)

(define note print-err)
(define balsa-make-cells #f)
(define balsa-helper-no-banner #f)

(define balsa-helper-print-banner (lambda () 
	(make-program-banner "balsa-make-helpers" "balsa-make-helpers: Cell Generator" "2002, The University of Manchester")	 
))

(define balsa-helper-usage (lambda ()
	(balsa-helper-print-banner)
	(error
		"version " balsa-version #\newline
		"usage: balsa-make-helpers  <helper-cells> <gate-mappings> [<helper-mappings> [<helper-defns>]]" #\newline #\newline
		"balsa-make-helpers:  generates a new helper-cells file for a technology existing abs descriptions" #\newline
		"The helper-cells file icontains the definitions of the new cells generated in the process and the gate-" #\newline
		"mappings file contains the gate-mappings of these cells. These must be concatenated with the existing helper" #\newline
		" cell and gate-mappings files in the technology. The remaining two optional args determine new helper-" #\newline
		"mappings and helper-defns file to override those currently in the common technology" #\newline #\newline
		"-b" " don't print banner message" #\newline
		"-t" " make test cells: <input-file-name> <output-file-name>" #\newline
	)
))

;;; balsa-make-helpers-command-line-rules: command-line-args action rules
(define balsa-make-helpers-command-line-rules `(
	(#\t "test" 0 ,(lambda (args) (set! balsa-make-cells #t)))
	(#\b "no-banner" 0 ,(lambda (args) (set! balsa-helper-no-banner #t)))
))

;;; balsa-make-helpers-parse-command-line: parse switches from the given command line list, set
;;;		the balsa-netlist-... globals and return the tail of the command line.
(define balsa-make-helpers-parse-command-line (lambda (args)
	(if (null? args)
		(balsa-helper-usage)
		(let
			((args-tail (parse-command-line "balsa-make-helpers"
				balsa-make-helpers-command-line-rules balsa-helper-usage args))
			)
			(if (and (> 2 (length args-tail)) (not balsa-make-cells))
				(balsa-helper-usage)
				args-tail
			)
		)
	)
))

;;; simple-gate->default-gate: map simple back end gates into technology specific gates using the
;;;		`default' gate for each gate type.
(define simple-gate->default-gate (lambda (gate gate-mappings)
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
				 (real-gate-description (assoc full-gate-name gate-mappings))
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

(define abs-gate->net-gate (lambda (gate)
	(case (car gate)
		((shdl shdl-repeat)
			gate
		)
		(else
			(list 'instance (if (symbol? (car gate)) (symbol->string (car gate)) (car gate))
				(map (lambda (connection)
					(if (eqv? connection 'unconnected) ; handle unconnected ports, ret. 'unconnected
						'unconnected
						(let*
							((net? (eqv? 'net (car connection)))
							 (name (if net? 
								(cond 
									((string=? (cadr connection) "vcc") (if tech-vcc-component-name "vcc" tech-vcc-net-name))
									((string=? (cadr connection) "gnd") (if tech-gnd-component-name "gnd" tech-gnd-net-name))
									(else (cadr connection))
								)
					  		(apply tech-bundle-name (take connection 3))
							 ))
							 (portion (find-headed-list-elem all-portions (car connection) #f))
							)
							; if this is a singleton portion, just include the name
							(if (gen-slice-indexed? connection)
								(cons name ; (data/... name low-index [high-index])
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
				) (cdr gate))
			)
		)
	)
))

(define helper:defines (lambda (helper) (find-headed-list-elem helper 'defines #f)))
(define helper:ports (lambda (helper) (find-headed-list-elem helper 'ports #f)))
(define helper:nodes (lambda (helper) (find-headed-list-elem helper 'nodes #f)))
(define helper:nets (lambda (helper) (find-headed-list-elem helper 'nets #f)))
(define helper:gates (lambda (helper) (find-headed-list-elem helper 'gates #f)))
(define helper:attributes (lambda (helper) (find-headed-list-elem helper 'attributes #f)))

;;; expand a connection to a list of net names in the target technology
(define gen-connection->net-nets (lambda (connection bindings defines)
	(let*
		((get-slices (lambda (connection)
			(let
				((slices (gen-expand-slices (list connection) bindings defines '() '(parameters))))
				(fold (lambda (slice outs)
					(append outs (gen-smash-simple-slice slice))) '() slices)
			 
		 	)
		 ))
		)
		;;; cheat using abs-gate which returns gate as 'instance name slices
		(cddr (abs-gate->net-gate (cons 'connect (flatten-unheaded-list (get-slices connection)))))
	)
))

;;; parse attributes just need to map tech-names for each net to target technology
(define gen-parse-attributes (lambda (attributes bindings defines)
	(let
		((specify-expr (lambda (expr) (brz-specify-expression expr defines '() '(parameters))))
		)
		(reverse! (fold (lambda (attribute ret)
			(let ((type (car attribute)))
				(cons
					(case type
						((simulation-initialise) ;; (simulation-initialise port/value list)
							(cons
								type
								(fold (lambda (port/value ret)
									(let* ;; can set a value to vector using this method
										((ports (apply append (gen-connection->net-nets (car port/value) bindings defines)))
										 (value (specify-expr (cadr port/value)))
										)
										(append ret
											(map.. (lambda (index port)
												(list port (if (logbit? index value) 1 0))
											) 0 (- (length ports) 1) ports)
										)
									)
								) '() (cdr attribute))
							)
						)
						((global-ports) ;; (global-ports port-list)
							(cons
								type
								(apply append (flatten-unheaded-list (map (lambda (port) (gen-connection->net-nets port bindings defines)) (cdr attribute))))
							)
						)
						((feedthrough) ;; (feedthrough conection-list)
							(cons*
								type
								(map (lambda (connection) ;; connection is a pair of port orders
									(map specify-expr connection)
								) (cdr attribute))
							)
						)
						(else
							(cons
								type
								(map (specify-expr (cdr attribute)))
							)
						)
					)
					ret
				)
			)
		) '() attributes))
	)
))

(define gen-instances-use-net-name (lambda (instances net-name)
	(letrec
		((instances-use-net-name (lambda (instances)
			(if (null? instances)
				#f
				(if (and (eq? 'instance (caar instances))
						(net-instance-connections-use-net-name (net-instance:instance-connections (car instances)) net-name))
					#t
					(instances-use-net-name (cdr instances))
				)
			)
		 ))
		)
		(instances-use-net-name instances)
	)
))

(define gen-make-helper-netlist (lambda (helper name gate-mappings)
	(let*
		((defines
			(let*
				((defines (helper:defines helper)))
				(if defines
					(fold (lambda (def existing-defs)
						(cons (list (car def)
							(brz-specify-expression (cadr def) existing-defs '() '(parameters)))
							existing-defs
						)
					) '() (cdr defines))
					'()
				)
		 	)
		 )
		 (specify-expr (lambda (expr) (brz-specify-expression expr defines '() '(parameters))))
		 (ports 
			(let*
				((port-list (helper:ports helper))
				(prts (map (lambda (port)
					(list (list-ref port 0) (list-ref port 1) (specify-expr (list-ref port 2)))
				) (cdr port-list))))
				(append '(ports) prts)
			)
		 )
		 (defined-nets (let ((nets (helper:nets helper)))
		 	(if nets (cdr nets) '())
		 ))
		 (nodes (reverse! (fold (lambda (node acc)
		 	(let
				((card (specify-expr (cadr node))))
				(if (not (zero? card))
					(cons
						(cons* (car node) card (map specify-expr (cddr node)))
						acc
					)
					acc
				)
			)
		 ) '() (cdr (helper:nodes helper)))))
		 (abs-gates (cdr (helper:gates helper)))
		 (port-bindings (map (lambda (port)
			(list (car port) (list-ref port 2) 0 1)
		 ) (cdr ports)))
		 (bindings (append port-bindings nodes '(("gnd" 1 0 1) ("unconnected" 1 0 1))))
	 	 (new-bindings/expanded-abs-gates (cdr
			(fold (lambda (abs-gate ret)
				(let ; call self on each gate and carry over the internal-name-index
					((i/b/g (gen-expand-abs-gate abs-gate  defines '() '(parameters) bindings "int" (car ret))))
					(if i/b/g
						(cons* (car i/b/g) (append (cadr i/b/g) (cadr ret)) (append (cddr i/b/g) (cddr ret)))
						ret
					)
				)
			) '(0 () . ()) abs-gates)
		 ))
		 (new-bindings (car new-bindings/expanded-abs-gates))
		 (expanded-abs-gates (cdr new-bindings/expanded-abs-gates))
		 (expanded-nodes (append (gen-bundle-bindings->net-nets nodes) defined-nets))
		 (last-internal-index/default-gates (foldl-ma (lambda (gate internal-base-index prev-gates)
			(let*
				((next-internal/gates (gen-abs-gate->simple-gates gate "internal" 0 internal-base-index))
				 (gates 
				 	(letrec
						((app-sim-gate (lambda (res gte)
							(if (null? gte)
								res
								(app-sim-gate
									(cons 
										(simple-gate->default-gate (car gte) gate-mappings)
										res
									)
									(cdr gte)
								)
							)
						)))
						(app-sim-gate '() (cdr next-internal/gates))))
				)
				(list (car next-internal/gates) (append prev-gates (map abs-gate->net-gate gates)))
			)
		 ) expanded-abs-gates 0 '()))
		 (nets (if (zero? (car last-internal-index/default-gates))
			expanded-nodes
			(cons (list (tech-bundle-name 'node "internal" 0) (car last-internal-index/default-gates))
				expanded-nodes
			)
		 ))
		 (instances (cadr last-internal-index/default-gates))
		 (vcc/gnd-nets
		 	`(nets
				,@nets
				,@(if (and tech-gnd-component-name (gen-instances-use-net-name instances "gnd"))
					'(("gnd" 1))
					'()
				)
				,@(if (and tech-vcc-component-name (gen-instances-use-net-name instances "vcc"))
						'(("vcc" 1))
						'()
					)
			)
		 )
		 (vcc/gnd-gates
		 		`(instances
					,@instances
					,@(if (and tech-gnd-component-name (gen-instances-use-net-name instances "gnd"))
						`((instance ,tech-gnd-component-name ("gnd")))
						'()
					)
					,@(if (and tech-vcc-component-name (gen-instances-use-net-name instances "vcc"))
						`((instance ,tech-vcc-component-name ("vcc")))
						'()
					)
				)
		 )
		 (attributes
			(let* 
				((attr (helper:attributes helper))
				 (new-attr (if attr (cons 'attributes (gen-parse-attributes (cdr attr) (append bindings new-bindings) defines)) '(attributes)))
				)
				(if balsa-make-cells
					 (if attr (list new-attr) '())
					 (list (append new-attr '((cell-type "helper"))))
				)
			)
		 )
		)
		`(circuit ,name
			,ports
			,vcc/gnd-nets
			,vcc/gnd-gates
			,@attributes
		)
	)
))

;;; Produce new balsa-cells defns list and update gate-mappings with new cells
(define gen-expand-generic-cells (lambda (cell-defns cell-map gate-defns)
	(let*
		((cell-mappings (if cell-map cell-map '())) 
		 (gate-mappings/defns (foldl-ma (lambda (defn new-gate-mappings new-gate-defns)
			(let*
				((mapping-desc (assoc (cadr defn) cell-mappings))
				 (mssg (if (and cell-map (not mapping-desc))
					(note "No Helper-Mapping for '" (cadr defn) "'" " using default name" #\newline)))
				 (cell-name (if mapping-desc (cadr mapping-desc) (cadr defn)))
				 (gate-description (assoc cell-name new-gate-mappings))
				 (circuit-name ;; If mapping use that name else use helper-mapping
				 	(if gate-description 
						(caadr gate-description)
						(if mapping-desc ((nth 2) mapping-desc) (cadr defn))
				 	)
				 )
				 (gate-defn (if gate-description
				 	(net-find-expected-circuit new-gate-defns circuit-name)
					(gen-make-helper-netlist defn circuit-name new-gate-mappings)
				 ))
				 (gate-desc (if gate-description ;; create mapping entry
					gate-description
					(let*
						((ports (helper:ports defn))
						 (attributes (helper:attributes defn))
						 (global-ports (if attributes (find-headed-list-elem attributes 'global-ports #f) attributes))
						 (port-count
							(if global-ports
								(- (length (cdr ports)) (length (cdr global-ports)))
								(length (cdr ports))
							)
						 )
						)
						(list cell-name (cons circuit-name (.. 0 (- port-count 1))) (list 1 circuit-name))
				 	)
				 ))
				)
				(if gate-description 
						(list
							new-gate-mappings
							new-gate-defns
						)
						(list
							(cons gate-desc new-gate-mappings)
							(cons gate-defn new-gate-defns)
						)
				)
			)
		 ) cell-defns breeze-gate-mappings gate-defns))
		)
		(fold (lambda (mapping defn new-mappings/gate-defns)
				(if (assoc (car mapping) breeze-gate-mappings)
					new-mappings/gate-defns
					(list 
						(cons mapping (car new-mappings/gate-defns))
						(cons defn (cadr new-mappings/gate-defns))
					)
				)
			) '(() ()) (car gate-mappings/defns) (cadr gate-mappings/defns))
	)
))

;; Create new helpers.net and gate-mappings in current directory from generic descriptions
(define gen-helper-test (lambda (args)
	(balsa-set-tech #f)
	(brz-load-primitives)
	(brz-load-gates)
	(note #\newline "using technology: '" (brz-pretty-format-tech-name) "'" #\newline)
	(let*
		((cell-file-name (cadr args))
		(helper-cells-out (open-output-file cell-file-name))
		(helper-cells 
			(brz-get-and-preprocess-file (car args)
				(brz-tech-build-include-filename "" "abs")
			)
		)
		(print-cell-header (net-print-header helper-cells-out "Pete's Test Cells" cell-file-name))
		(new-defns (gen-expand-generic-cells helper-cells #f '()))
		(cells (for-each (lambda (defn)
			(net-print-circuit-decl helper-cells-out defn)
			) (cadr new-defns)))
		)
		(note "writing cell definitions to '" cell-file-name "'" #\newline)
		(close-output-port helper-cells-out)
	)
))

;; Create new helpers.net and gate-mappings in current directory from generic descriptions
(define gen-new-helper-cells (lambda (args)
	(balsa-set-tech #f)
	(brz-load-primitives)
	(brz-load-gates)
	(note #\newline "using technology: '" (brz-pretty-format-tech-name) "'" #\newline)
	(let*
		((modified-args (filter (lambda (elem) (not (string=? (substring elem 0 1) "-"))) args))
		 (cell-file-name (car modified-args))
		 (mapping-file-name (cadr modified-args))
		 (args-length (length modified-args))
		 (helper-mappings-file-name (if (> args-length 2)
			(caddr modified-args) (string-append data-dir "tech/common/helper-mappings")))
		 (helper-cells-file-name (if (> args-length 3)
			(cadddr modified-args) (string-append data-dir "tech/common/helper-cells.abs")))
		 (helper-cells-out (open-output-file cell-file-name))
		 (gate-mappings-out (open-output-file mapping-file-name))
		 (helper-cells 
			(brz-get-and-preprocess-file helper-cells-file-name
				(brz-tech-build-include-filename "" "abs"))
		 )
		 (helper-mappings 
			(brz-get-and-preprocess-file helper-mappings-file-name
				(brz-tech-build-include-filename "" #f))
		 )
		 (cells-header (string-append breeze-tech " Balsa helper cells"))
		 (print-cell-header (net-print-header helper-cells-out cells-header cell-file-name))
		 (new-defns (gen-expand-generic-cells helper-cells helper-mappings breeze-gate-defns))
		 (mappings (with-output-to-port gate-mappings-out
			(lambda ()
				(for-each (lambda (mapping)
					(simple-print  mapping "" '())
					(print-to-port gate-mappings-out #\newline)
			) (car new-defns)))
		 ))
		 (cells (for-each (lambda (defn)
			
			(net-print-circuit-decl helper-cells-out defn)
			) (cadr new-defns)))
		 )
		 (note "writing cell definitions to '" cell-file-name "'" #\newline)
		 (note "writing gate mappings to '" mapping-file-name "'" #\newline)
		 (close-output-port gate-mappings-out)
		 (close-output-port helper-cells-out)
	)
))
		
(top-level (lambda (args)
	(let
		((args-tail (balsa-make-helpers-parse-command-line args)))
		(if (not balsa-helper-no-banner)
			(balsa-helper-print-banner)
		)
		(cond
			((null? args-tail) (balsa-helper-usage))
			(balsa-make-cells (gen-helper-test args-tail))
			(else (gen-new-helper-cells args))
		)
	)
) (cdr command-line-args))

