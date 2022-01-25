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
;;;	`net-connections.scm'
;;;	Net connection table generating/manipulating procedures
;;;

(balsa-scheme-import 'brz)
(balsa-scheme-import 'net)

; (brz-load-gates)

;;; net-connection:{name,nature,cardinality,connections}: field accessors
(define net-connection:name car)
(define net-connection:nature cadr)
(define net-connection:cardinality caddr)
(define net-connection:connections cadddr)
(define net-connection:port/net-index (nth 4))

;;; net-conn-conn:{instance-index,connection-index,wire-index}: field accessors for
;;;		connection net-connection:connections vector entries.
(define net-conn-conn:instance-index car)
(define net-conn-conn:connection-index cadr)
(define net-conn-conn:wire-index caddr)

;;; net-connection<=?: ordering predicate for net-connections.
(define net-connection<=? net-net/port:name<=?)

;;; net-find-sorted-connection: vector search procedure for net-connection vectors.
(define net-find-sorted-connection (lambda (cs name)
	(find-in-sorted-vector net-connection<=? cs (list name))
))

;;; net-find-sorted-connection-index: vector search procedure for net-connection vectors.
(define net-find-sorted-connection-index (lambda (cs name)
	(find-index-in-sorted-vector net-connection<=? cs (list name))
))

;;; net-connection-any-connections?: return true if their are any connections
;;;		to this net-connection entry for a net
(define net-connection-any-connections? (lambda (conn)
	(let ((conn-conn (net-connection:connections conn)))
		(let check ((index 0))
			(if (= index (vector-length conn-conn))
				#f
				(let ((conn-conn-conn (vector-ref conn-conn index)))
					(if (or (null? conn-conn-conn)
						(and (null? (cdr conn-conn-conn)) (null? (car conn-conn-conn))))
						(check (+ 1 index))
						#t
					)
				)
			)
		)
	)
))

;;; net-connection-is-to-port?: is a connection a signal connected to a port
(define net-connection-is-to-port? (lambda (connection)
	(memq (net-connection:nature connection) net-port-directions)
))

;;; net-connection-table-add-connection!: fill in the c-table entry for a given instance-connection
;;;		adding a triple (instance-index connection-index wire-index) (from type conn-conn) to the head of the
;;;		connection-table's list for the nets named in the connection.
;;;		`instance-index' may be any token used to identify the instance. `connection-index'
;;;		should be the position in the instance connection list of this index. `first-wire-index'
;;;		should be set to 0 for invocations of net-connection-table-add-connection! which refer to
;;;		complete connections; this parameter is used in recursive invocation to give a base wire index
;;;		for vector connection elements.  The `wire-index' field in the set triple will usu. contain the
;;;		`first-wire-index'.  This procedure returns a count of the number of wires present in
;;;		the given connection.
(define net-connection-table-add-connection! (lambda
	(connection-table instance-index instance-connection connection-index first-wire-index)
	(let	
		;;; add-connection!: add a connection the table (instance-index connection-index connection-wire-index)
		;;;		for element `net-wire-index' of the net connection `connection'
		((add-connection! (lambda (connection net-wire-index connection-wire-index)
			(vector-set! (net-connection:connections connection) net-wire-index
				(cons (list instance-index connection-index connection-wire-index)
				(vector-ref (net-connection:connections connection) net-wire-index))
			)
		)))
		(cond
			((pair? instance-connection)
				(cond
					; vector list - iterate down connections
					((eq? (car instance-connection) 'vector)
						(+ 1 (- (fold (lambda (first-index connection)
							(+ first-index
								(net-connection-table-add-connection! connection-table instance-index connection
									connection-index first-index)
							)
						) first-wire-index (cdr instance-connection)) first-wire-index))
					)
					; (name index)
					((= 2 (length instance-connection))
						(add-connection! (net-find-sorted-connection connection-table (car instance-connection))
							(cadr instance-connection) first-wire-index)
						1 ; one wire processed
					)
					(else ; (name index cardinality)
						(let*
							((index (cadr instance-connection))
							 (cardinality (caddr instance-connection))
							 (connection (net-find-sorted-connection connection-table (car instance-connection)))
							)
							(for.. (lambda (wire-index)
								(add-connection! connection wire-index (+ first-wire-index (- wire-index index)))
							) index (+ cardinality index -1))
							cardinality ; n wires
						)
					)
				)
			)
			; set at index 0
			((string? instance-connection)
				(add-connection! (net-find-sorted-connection connection-table instance-connection)
					0 first-wire-index)
				1 ; one wire processed
			)
			((eq? instance-connection 'unconnected) 1)
			(else (error "net-connection-table-add-connection!: invalid instance-connection `" instance-connection "'"))
		)
	)
))

;;; net-connection-table-add-connections!: add a given list of instance-connections to a given
;;;		connection-table.  These connections have 0 based incrementing connection-indices.
(define net-connection-table-add-connections!
	(lambda (connection-table instance-index instance-connections first-index)
	(fold (lambda (connection index)
		(net-connection-table-add-connection! instance-index connection-table connection index 0)
		(+ 1 index)
	) 0 instance-connections)
))

;;; net-make-connection-table: make a table of net->instance connection mapping for the given
;;;		circuit-decl.  This will be a sorted vector of ``net-connection''s (name nature cardinality connections)
;;;		for each of the ports and nets in the circuit-decl. `name' and `cardinality' match those
;;;		of the net/port, `nature' is 'net for nets and the relevant direction for ports, `connections'
;;;		will be a vector (of length cardinality) of lists of connections for each of the indices of
;;;		that net.  Each connection entry is a triple as described in the comments for
;;;		net-connection-table-add-connection!.
(define net-make-connection-table (lambda (circuit-decl)
	(let*
		((ports (cdr (net-circuit-decl:ports circuit-decl)))
		 (nets (cdr (net-circuit-decl:nets circuit-decl)))
		 (numbered-ports (map cons ports (.. 0 (- (length ports) 1))))
		 (numbered-nets (map cons nets (.. 0 (- (length nets) 1))))
		 (nets/ports<=? (lambda (n1 n2) (string<=? (caar n1) (caar n2))))
		 (sorted-ports (merge-sort nets/ports<=? numbered-ports))
		 (sorted-nets (merge-sort nets/ports<=? numbered-nets))
		 (sorted-globals (if (null? tech-global-nets) '() (merge-sort net-net:name<=? (cdr tech-global-nets))))
		 (connection-table
			(list->vector (merge! net-net/port:name<=?
				(merge! net-net/port:name<=?
					(map (lambda (p/i) (let ((p (car p/i)) (i (cdr p/i)))
						(list
							(net-port:name p) (net-port:direction p) (net-port:cardinality p)
							(make-vector (net-port:cardinality p) '())
							i
						)
					)) sorted-ports)
					(map (lambda (g)
						(list
							(net-net:name g) 'global (net-net:cardinality g)
							(make-vector (net-net:cardinality g) '())
							#f
						)
					) sorted-globals)
				)
				(map (lambda (n/i) (let ((n (car n/i)) (i (cdr n/i)))
					(list
						(net-net:name n) 'net (net-net:cardinality n)
						(make-vector (net-net:cardinality n) '())
						i
					)
				)) sorted-nets)
			))
		 )
		 (instances (cdr (net-circuit-decl:instances circuit-decl)))
		)
		(fold (lambda (instance index)
			(net-connection-table-add-connections! index connection-table
				(net-instance:instance-connections instance) 0)
			(+ 1 index)
		) 0 instances)
		connection-table
	)	
))
