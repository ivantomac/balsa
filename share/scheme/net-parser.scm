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
;;;	`net-parser.scm'
;;;	Parsers for various netlist and pseudo-netlist formats
;;;

(balsa-scheme-import 'net)
(balsa-scheme-import 'misc 'date)

;;;;; Name mapping

;;; net-simple-cell-name-mapping: a simple name cropping procedure for cell-names.  Call this
;;;		with an indication of the preferred case (UPPER/lower) for the current tech and you'll
;;;		get back the mapping procedure.
(define net-simple-cell-name-mapping (lambda (upper-case)
	(lambda (unmapped-name)
		(let ((max-unmapped-name-length (- tech-cell-name-max-length 6)))
			(if (<= (string-length unmapped-name) max-unmapped-name-length)
				unmapped-name
				(let*
					((mapped-name-stub (substring unmapped-name 0 max-unmapped-name-length))
					(old-entry (assoc unmapped-name tech-map-cell-name-old-names))
					(entry (if old-entry
						old-entry
						(assoc unmapped-name tech-map-cell-name-new-names)
					))
					)
					(if entry ; already have this name
						(cdr entry)
						(let ((mapped-name (string-append mapped-name-stub (if upper-case "_M" "_m")
							(number->string (+ 1 tech-map-cell-name-count)) (if upper-case "M" "m"))))
							(set! tech-map-cell-name-new-names (cons
								(cons unmapped-name mapped-name) tech-map-cell-name-new-names))
							(set! tech-map-cell-name-count (+ 1 tech-map-cell-name-count))
							mapped-name
						)
					)
				)
			)
		)
	)
))

;;; net-simple-cell-name-import: import mappings for cell names
(define net-simple-cell-name-import (lambda (filename)
	(let ((imported-names (get-file filename)))
		(set! tech-map-cell-name-count (car imported-names))
		(set! tech-map-cell-name-old-names (cdr imported-names))
	)
))

;;; net-simple-cell-name-export: export mappings for cell names
(define net-simple-cell-name-export (lambda (filename)
	(let*
		((out (open-output-file filename))
		 (print-mappings (lambda (mappings)
		 	(for-each (lambda (mapping)
		 		(print-to-port out "(\"" (car mapping) "\" . \"" (cdr mapping) "\")" #\newline)
		 	) mappings)
		 ))
		)
		(net-print-header out "Balsa cell name mapping file" filename)
		(print-to-port out ";;; current mapping count" #\newline tech-map-cell-name-count #\newline #\newline)
		(print-to-port out ";;; current mappings " #\newline)
		(print-mappings tech-map-cell-name-old-names)
		(print-mappings tech-map-cell-name-new-names)
		(close-output-port out)
	)
))

;;;;; Netlist signatures

;;; net-signature:{write-netlist-file ...} netlist operations signature list access procedures
(define net-signature:write-netlist-file car)
(define net-signature:vcc-net-name cadr)
(define net-signature:vcc-component-name caddr)
(define net-signature:gnd-net-name (nth 3))
(define net-signature:gnd-component-name (nth 4))
(define net-signature:global-nets (nth 5))
(define net-signature:map-name (nth 6))
(define net-signature:bundle-name (nth 7))
(define net-signature:filename-suffix (nth 8))
(define net-signature:channel-name (nth 9))
(define net-signature:bus-suffix (nth 10))
(define net-signature:brz-prefix (nth 11))
(define net-signature:balsa-prefix (nth 12))
(define net-signature:write-symbol-file (nth 13))
(define net-signature:symbol-filename-suffix (nth 14))

;;; net-signature-for-netlist-format: return the netlist features signature list
;;;		for a given netlist format, this function imports the required files and will set
;;;		the global tech-... variables to the netlist defaults if set-to-default-tech? is
;;;		true.
(define net-signature-for-netlist-format (lambda (netlist-format set-to-default-tech?)
	(let*
		((netlist-format-name (if (symbol? netlist-format)
			netlist-format (string->symbol (string-downcase netlist-format))))
		 (signature (case netlist-format-name
			((verilog)
				(balsa-scheme-import 'net 'verilog)
				(verilog-netlist-signature)
			)
			((net) net-netlist-signature)
			(else (error "net: don't recognise netlist format `" netlist-format-name "'"))
		 ))
		)
		(if set-to-default-tech? (begin
			(set! tech-netlist-format-name netlist-format-name)
			(set! tech-write-netlist-file (net-signature:write-netlist-file signature))
			(set! tech-vcc-net-name (net-signature:vcc-net-name signature))
			(set! tech-vcc-component-name (net-signature:vcc-component-name signature))
			(set! tech-gnd-net-name (net-signature:gnd-net-name signature))
			(set! tech-gnd-component-name (net-signature:gnd-component-name signature))
			(set! tech-global-nets (net-signature:global-nets signature))
			(set! tech-map-name (net-signature:map-name signature))
			(set! tech-bundle-name (net-signature:bundle-name signature))
			(set! tech-filename-suffix (net-signature:filename-suffix signature))
			(set! tech-channel-name (net-signature:channel-name signature))
			(set! tech-bus-suffix (net-signature:bus-suffix signature))
			(set! tech-brz-prefix (net-signature:brz-prefix signature))
			(set! tech-balsa-prefix (net-signature:balsa-prefix signature))
			(set! tech-write-symbol-file (net-signature:write-symbol-file signature))
			(set! tech-symbol-filename-suffix (net-signature:symbol-filename-suffix signature))
		))
		signature
	)
))

;;;;; Netlist I/O

;;; net-read-netlist-file: read a list of circuit-decls from the named file
(define net-read-netlist-file (lambda (filename)
	(let
		((decls (get-file filename)))
		(cond 
			((not decls) #f)
			((net-check-circuit-decls? decls) decls)
			(else (error "net-read-netlist-file: problem with netlist file" #\newline))
		)
	)
))

;;; net-print-header: print a suitable file header
(define net-print-header (lambda (out type filename)
	(let
		((unix-name (let ((u-name (uname))) (if (vector? u-name) u-name
			#("unknown" "unknown" "unknown" "1" "unknown")))))
		(print-to-port out
			";;; `" filename "'" #\newline
			";;; " type #\newline
			";;; Created: " (ctime (current-time)) #\newline
			";;; By: " (vector-ref (getpw (getuid)) 0)
			"@" (vector-ref unix-name 1) " (" (vector-ref unix-name 0) ")" #\newline
			";;; With net-net version: " balsa-version #\newline #\newline
		)
	)
))

;;; net-print-circuit-decl: pretty print a net-circuit-decl
(define net-print-circuit-decl (lambda (port circuit)
	(if (null? circuit)
		(print-to-port port "; NULL circuit")
		(let ((new-tabs "    "))
			(print-to-port port "(circuit \"" (net-circuit-decl:name circuit) "\"")
			(with-output-to-port port (lambda ()
				(for-each (lambda (elem)
					(print-to-port port #\newline new-tabs)
					(simple-print elem new-tabs '(circuit instances ports nets))
				) (cddr circuit))
			))
			(print-to-port port #\newline ")" #\newline #\newline)
		)
	)
))

;;; net-write-netlist-file: write a .net netlist file `filename' for circuit(s) `top-circuits'
;;;		`circuits' provides references for sub-circuit components.
;;;		`top-circuit-name' is ignored (the circuits in top-circuits use their own names)
;;;		net-write-netlist-file is a prototype for the EDA system native netlist write functions.
;;;		`features' (if given) is a list of a (symbol . value) pairs with additional (possibly
;;;		netlist format specific) features.
;;;		Supports features included-cell-types and excluded-cell-names
(define net-write-netlist-file (lambda (circuits top-circuits top-circuit-name filename . features)
	(let*
		((out (open-output-file filename))
		 (included-cell-types '("none"))
		 (excluded-cell-names '())
		)
		(net-add-file-to-session-file-list filename)
		(for-each (lambda (feature)
			(if (not (pair? feature))
				(error "net-net-write-netlist-file: features must be (name . value) pairs")
				(case (car feature)
					((included-cell-types) (set! included-cell-types (cons "none" (cdr feature))))
					((excluded-cell-names) (set! excluded-cell-names (cdr feature)))
					(else (print-err "net-net-write-netlist-file: unrecognised feature `" feature "'" #\newline))
				)
			)
		) features)
		(net-print-header out "Balsa .net netlist file" filename)
		(let*
			((top-circuits-list (if (headed-list? top-circuits 'circuit) (list top-circuits) top-circuits))
			 (all-circuits (net-flatten-circuit-defns top-circuits-list circuits included-cell-types))
			)
			(for-each (lambda (top-circuit/defn)
				(let*
					((circuit (car top-circuit/defn))
					 (cell-type (net-circuit-decl-cell-type circuit))
					)
					(if (and cell-type (member cell-type included-cell-types)
						(not (member (net-circuit-decl:name circuit) excluded-cell-names)))
						(net-print-circuit-decl out circuit)
					)
				)
			) all-circuits)
		)
		(close-output-port out)
	)
))

;;; net-netlist-signature: defaults for .net netlists
(define net-netlist-signature (list
	net-write-netlist-file ; write-netlist-file
	"vcc" ; vcc-net-name
	"" ; vcc-component-name
	"gnd" ; gnd-net-name
	"" ; gnd-component-name
	'() ; global-nets
	id ; map-name
	#f ; bundle-name (portion:{req,ack,data...} name:string index:int) => string
	"net" ; filename-suffix
	#f ; channel-name (portion:{req,ack,data...} channel-no:int) => string
	#f ; bus-suffix (low-index:int width:int cardinality:int) => string
	"brz" ; brz-prefix
	"balsa" ; balsa-prefix
	#f ; write-symbol-file
	"" ; symbol-filename-suffix
))

;;; net-check*?: net-check procedures check the given argument
;;;    for compliance with the named patterm.  Warnings are
;;;    given for headed list objects which have a correct initial
;;;    symbol but an incorrect structure

;;; net-check-direction?: input/output or inout
(define net-check-direction? (lambda (dir)
	(memq dir '(input output inout hiz))
))

;;; net-check-port?: a single port
(define net-check-port? (lambda (port)
	(and
		(warning-and "net-port"
			`(,(= 3 (length port)) "format: (name direction cardinality)")
		)
		(warning-and "net-port"
			`(,(string? (net-port:name port)) "expecting a port name")
			`(,(net-check-direction? (net-port:direction port)) "expecting a port direction")
			`(,(integer? (net-port:cardinality port)) "expecting a port cardinality")
		)
	)
))

;;; net-check-ports?: port list
(define net-check-ports? (lambda (ports)
	(headed-uniform-list? ports 'ports net-check-port?)
))

;;; net-check-net?: name/cardinality pair
(define net-check-net? (lambda (net)
	(and
		(warning-and "net-net"
			`(,(= 2 (length net)) "format: (name cardinality)")
		)
		(warning-and "net-net"
			`(,(string? (net-net:name net)) "expecting a net name")
			`(,(integer? (net-net:cardinality net)) "expecting a net cardinality")
		)
	)
))

;;; net-check-nets?: list of local nets
(define net-check-nets? (lambda (nets)
	(headed-uniform-list? nets 'nets net-check-net?)
))

;;; net-check-vector?: a vector of connections
(define net-check-vector? (lambda (vector)
	(headed-uniform-list? vector 'vector net-check-instance-connection?)
))

;;; net-check-instance-connection?: a single instance port connection
(define net-check-instance-connection? (lambda (connection)
	(or
		(net-check-vector? connection)
		(eqv? connection 'unconnected)
		(string? connection)
		(and
			(pair? connection)
			(let ((len (length connection)))
				(and
					(<= 2 len)
					(>= 3 len)
					(warning-and "instance-connection"
						`(,(string? (car connection)) "expecting a net name")
						`(,(integer? (cadr connection)) "expecting an index")
					)
					(if (= 3 len)
						(warning-and "instance-connection"
							`(,(integer? (caddr connection)) "expecting a connection width")
						)
						#t
					)
				)
			)
		)
	)
))

;;; net-check-instance-connections?: connections to an instance
(define net-check-instance-connections? (lambda (connections)
	(apply eager-and (map net-check-instance-connection? connections))
))

;;; net-check-instance?: a component instance
(define net-check-instance? (lambda (instance)
	(and
		(headed-list? instance 'instance)
		(warning-and "net-instance"
			`(,(<= 3 (length instance)) "format: (instance name instance-connections [instance-name])")	
		)
		(warning-and "net-instance"
			`(,(string? (net-instance:name instance)) "expecting a gate name")
			`(,(net-check-instance-connections? (net-instance:instance-connections instance))
				"expecting a list of net connections")
			`(,(or (null? (cdddr instance)) (string? (net-instance:instance-name instance)))
				"expecting an instance name")
		)
	)
))

;;; net-check-instances?: list of instances
(define net-check-instances? (lambda (instances)
	(headed-uniform-list? instances 'instances net-check-instance?)
))

;;; net-check-circuit-decl?: Is this a circuit declaration?
(define net-check-circuit-decl? (lambda (circuit)
	(and
		(headed-list? circuit 'circuit)
		(warning-and "circuit-decl"
			`(,(<= 5 (length circuit)) "format: (circuit name ports nets instances)")
		)
		(warning-and "circuit-decl"
			`(,(string? (net-circuit-decl:name circuit)) "expecting a circuit name")
			`(,(net-check-ports? (net-circuit-decl:ports circuit)) "expecting a list of port declarations")
			`(,(net-check-nets? (net-circuit-decl:nets circuit)) "expecting a list of ports")
			`(,(net-check-instances? (net-circuit-decl:instances circuit)) "expecting a list of instances")
		)
	)
))

;;; net-check-circuit-decls?: A list of circuit decls.
(define net-check-circuit-decls? (lambda (circuits)
	(apply eager-and (map net-check-circuit-decl? circuits))
))

;;; net-circuit-decl:attributes: Returns the attributes list for the given circuit-decl
;;;		or (attributes) if there are no attributes
(define net-circuit-decl:attributes (lambda (circuit)
	(find-headed-sub-list circuit 'attributes)
))

;;; net-circuit-decl-find-attribute: Returns the attribute name/value of attribute
;;;		`attribute' for the given circuit-decl or #f if that attribute is not present.
;;;		eg. (attributes (a 10)) returns '(a 10)
(define net-circuit-decl-find-attribute (lambda (circuit attribute)
	(find-headed-sub-list (net-circuit-decl:attributes circuit) attribute)
))
