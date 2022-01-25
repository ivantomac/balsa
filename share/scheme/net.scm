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
;;;	`net.scm'
;;;	.net netlist manipulation/parsing/generation
;;;

;;; net-session-file-list: list of netlist/other files generated in this session
;;;		useful for generating balsa-netlist output manifest files
(define net-session-file-list '())

;;; net-add-file-to-session-file-list: add a filename to net-session-file-list, mostly used
;;;		by the netlist output functions
(define net-add-file-to-session-file-list (lambda (file)
	(set! net-session-file-list (cons file net-session-file-list))
))

;;; Basic gate level netlist format .net:
;;;
;;; Auxiliary defns:
;;; <name> ::= any valid name string, [a-zA-Z_][a-zA-Z$0-9_]*.
;;; <cardinality> ::= N1
;;; <index> ::= N0
;;;
;;; Netlist .net file:
;;; <net-netlist> ::= <net-circuit-decl> *
;;; <net-circuit-decl> ::= (circuit <name> <net-ports> <net-nets> <net-instances> <net-optional> *)
;;; <net-ports> ::= (ports <net-port> *)
;;; <net-port> ::= (<name> <net-direction> <cardinality>)
;;; <net-direction> ::= input | output | inout | hiz
;;; <net-nets> ::= (nets <net-net> *)
;;; <net-net> ::= (<name> <cardinality>)
;;; <net-instances> ::= (instances <net-instance> *)
;;; <net-instance> ::= (instance <name> <net-instance-connections> <net-instance-name>?)
;;; <net-instance-name> ::= <name>
;;; <net-instance-connections> ::= ( <net-instance-connection> *)
;;; <net-instance-connection> ::= (<name> <index>)
;;;		| (<name> <index> <cardinality>)
;;;		| <name>
;;;		| <net-vector>
;;;		| unconnected
;;; <net-vector> ::= (vector <net-instance-connection> *)
;;; <net-optional> ::= (attributes <net-attribute> *)
;;; <net-attribute> ::= (<net-attribute-name> ...)
;;; <net-attribute-name> ::= <name>

;;; net-port-directions: valid port directions
(define net-port-directions '(input output input hiz))

;;; net-circuit-decl:{name,ports,nets,instances}: field accessors
(define net-circuit-decl:name cadr)
(define net-circuit-decl:ports caddr)
(define net-circuit-decl:nets cadddr)
(define net-circuit-decl:instances (nth 4))

;;; net-port:{name,direction,cardinality}
(define net-port:name car)
(define net-port:direction cadr)
(define net-port:cardinality caddr)

;;; net-net:{name,cardinality,options}: net names and widths, options are optional!
(define net-net:name car)
(define net-net:cardinality cadr)
(define net-net:options cddr)

;;; net-instance:{name,instance-connections}: field accessors
(define net-instance:name cadr)
(define net-instance:instance-connections caddr)
(define net-instance:instance-name (lambda (instance)
	(if (not (null? (cdddr instance))) (cadddr instance) #f)
))

;;; net-procedures:{net-map-name...} Access procedures for list returned by <cadsystem>-net-procedures
(define net-procedures:map-name car)
(define net-procedures:bundle-name cadr)
(define net-procedures:bus-suffix caddr)
(define net-procedures:write-netlist caddr)

;;; net-ports->nets: map a port list into a nets list eg. (ports ("a" input 8)) => (nets ("a" 8))
(define net-ports->nets (lambda (ports)
	(cons 'nets (map (lambda (port) (list (net-port:name port) (net-port:cardinality port))) (cdr ports)))
))

;;;;; Search procedures / ordering predicates

;;; net-net/port:name<=?: net or port name order sorting predicate
(define net-net/port:name<=? (lambda (l r)
	(string<=? (net-net:name l) (net-net:name r))
))

;;; net-net:name<=?: net name order sorting predicate
(define net-net:name<=? net-net/port:name<=?)

;;; net-port:name<=?: port name order sorting predicate
(define net-port:name<=? net-net/port:name<=?)

;;; net-find-net: find a net of the given name in a (nets ...) list
(define net-find-net (lambda (nets name)
	(assoc name (cdr nets))
))

;;; net-find-sorted-net: find the named net in a sorted vector of nets
(define net-find-sorted-net (lambda (nets name)
	(find-in-sorted-vector net-net:name<=? nets (list name))
))

;;; net-find-circuit: find a circuit in a list of circuit-decl's
(define net-find-circuit (lambda (circuits circuit-name)
	(find-headed-list-elem circuits 'circuit circuit-name)
))

;;; net-find-expected-circuit: find a circuit in a list of circuit-decl's, moan if there isn't one
(define net-find-expected-circuit (lambda (circuits circuit-name)
	(let ((circuit (find-headed-list-elem circuits 'circuit circuit-name)))
		(if (not circuit) (error "net-find-expected-circuit: can't find circuit `" circuit-name "'"))
		circuit
	)
))

;;; net-circuit-decl-cell-type: return the type of a circuit decl or "none" if no
;;;		cell type is specified.
(define net-circuit-decl-cell-type (lambda (circuit)
	(let ((cell-type (net-circuit-decl-find-attribute circuit 'cell-type)))
		(if cell-type (cadr cell-type) "none")
	)
))

;;; net-sorted-instance-name-list: return a sorted list of instance cell names
;;;		for the circuit, `circuit'
(define net-sorted-instance-name-list (lambda (circuit)
	(uniq string=? (merge-sort! string<=?
		(fold (lambda (instance ret)
			(if (eq? 'instance (car instance))
				(cons (net-instance:name instance) ret)
				ret
			)
		) '() (cdr (net-circuit-decl:instances circuit)))
	))
))

;;; net-instance-connections-use-net-name: does the net named `name' occur anywhere in
;;;		the given instance-connections.  Doesn't bother to check index or cardinality of
;;;		name usage.  Mostly useful for deciding when to place vcc/gnd components for
;;;		netlist formats with pseudo-global vcc/gnd nets.
(define net-instance-connections-use-net-name (lambda (instance-connections net-name)
	(let
		((instance-connection-uses-net-name (lambda (connection)
			(cond
				((string? connection) (string=? connection net-name))
				((pair? connection)	
					(if (eq? (car connection) 'vector)
						(net-instance-connections-use-net-name (cdr connection) net-name)
						(string=? (car connection) net-name)
					)
				)
				(else #f)
			)
		)))
		(let tail
			((connections instance-connections))
			(if (null? connections)
				#f
				(if (instance-connection-uses-net-name (car connections))
					#t ; not sure about tail optimisation
					(tail (cdr connections))
				)
			)
		)
	)
))

;;; net-circuit-decl-uses-net-name: as net-instance-connections-user-net-name but test all the
;;;		instances in `circuit-decl'
(define net-circuit-decl-uses-net-name (lambda (circuit-decl net-name)
	(let tail
		((instances (drop-head-symbol 'instances (net-circuit-decl:instances circuit-decl))))
		(if (null? instances)
			#f
			(if (and (eq? 'instance (caar instances))
				(net-instance-connections-use-net-name (net-instance:instance-connections (car instances)) net-name))
				#t
				(tail (cdr instances))
			)
		)
	)
))

;;; net-flatten-circuit-defns: returns a list of pairs of (circuit-defn . defineNdeclare)
;;;		for the circuits in `top-circuits' and all their depended-upon circuits.  Any circuit
;;;		found to have a circuit type named in `cell-types' is defined rather than
;;;		declared and its depended-upon circuits are also declared/defined.  Note that
;;;		the cell type list should have an element "none" so that untyped circuits can be output.
;;;		If a cell's type is not listed in `cell-types' then its dependencies are not considered.
;;;		Returns an ordered list of (declaration . defineNdeclare) elements.
;;;		If `cell-types' is #t then decend into all cells (and list them in the return value
;;;		as definable) irrespective of cell-type.
(define net-flatten-circuit-defns (lambda (top-circuits circuits cell-types)
	(let* ;;; already-declared: hash of circuit-name => (defn . defn-order)
		((number-of-buckets 32)
		 (already-declared (make-hash-table number-of-buckets))
		 ;;; instance-defns: return a list of the definitions of all those instances of
		 ;;;	the given circuit whose cells are not yet defined
		 (instance-defns (lambda (circuit)
			(reverse! (fold (lambda (circuit-name acc)
				(if (hash-ref already-declared circuit-name)
					acc
					(cons (net-find-expected-circuit circuits circuit-name) acc)
				)
			) '() (net-sorted-instance-name-list circuit)))
		 ))
		)
		(letrec ;;; descend-circuit-hierarchy: add to already-declared by decended the circuit
			;;; hierarchied for the circuits `top-circuits'
			((descend-circuit-hierarchy (lambda (top-circuits next-declaration-index)
				(fold (lambda (circuit index)
					(cond
						((hash-ref already-declared (net-circuit-decl:name circuit)) index)
						((or (eq? #t cell-types)
							(member (net-circuit-decl-cell-type circuit) cell-types)) ; is a stopping cell type
							(let ((next-declaration-index-after-subcells
								(descend-circuit-hierarchy (instance-defns circuit) index)))
								(hash-set! already-declared (net-circuit-decl:name circuit) (cons (cons circuit #t)
									next-declaration-index-after-subcells))
								(+ 1 next-declaration-index-after-subcells)
							)
						)
						(else
							(hash-set! already-declared (net-circuit-decl:name circuit) (cons (cons circuit #f) index))
							(+ 1 index)	
						)
					)
				) next-declaration-index top-circuits)
			)))
			(let*
				((number-of-decls (descend-circuit-hierarchy top-circuits 0))
				 (decls (make-vector number-of-decls #f))
				)
				; iterate over hash table reading out into a list
				; hash elements are (name . ((definition . declareNdefine) . defn-number))
				(hash-fold (lambda (key value prior) (vector-set! decls (cdr value) (car value)) #f) #f already-declared)
				(vector->list decls)
			)
		)	
	)
))

;;; net-smash-instance-connection: smash an instance connection into singleton
;;;		connections of the form ("name" index).  NB. "name" connections are assumed to be
;;;		singleton already, this is a normal Balsa netlist assumption.
(define net-smash-instance-connection (lambda (connection)
	(cond
		((string? connection) (list (list connection 0)))
		((pair? connection)
			(cond
				((string? (car connection))
					(case (length connection)
						((2) (list connection))
						;;; smash bits, in decending order 'cause Breeze prefers this
						((3) (map-integer-range-list (lambda (index) (list (car connection) index))
							(+ -1 (cadr connection) (caddr connection)) (cadr connection)))
						(else #f)
					)
				)
				((eq? 'vector (car connection))
					(apply append (map net-smash-instance-connection (cdr connection)))
				)
				(else #f)
			)
		)
		((eq? 'unconnected connection) 'unconnected)
		(else #f)
	)
))
