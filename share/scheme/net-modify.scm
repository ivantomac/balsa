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
;;;	`net-modify.scm'
;;;	Netlist in-place modification procedures
;;;

;;; net-circuit-decl-remove-instances!: remove the instance list from a cell (in place)
;;;		and replace it with (instances)
(define net-circuit-decl-remove-instances! (lambda (cell)
	(let* ((instances (net-circuit-decl:instances cell)))
		(set-cdr! instances '())
	)
))

;;; balsa-netlist-add-globals!: add global ports to netlist cells - assumes top level cell is last cell in the list
(define balsa-netlist-add-globals! (lambda (netlist)
	(let*
		((globals (make-hash-table 57)))
		(unsorted-uniq (lambda (p1 p2) (string=? (net-port:name p1) (net-port:name p2)))
			(fold (lambda (cell signals)
				(let ((global-ports (net-circuit-decl-find-attribute cell 'global-ports)))
					(if global-ports ; this is the start of a global signal
						(let*
							((global-port-list (map (lambda (port-name) 
								(net-find-net (net-circuit-decl:ports cell) port-name)
							 ) (cdr global-ports)))
							 (update-hash (hash-set! globals (net-circuit-decl:name cell) global-port-list))
							)
							(append signals global-port-list)
						)
						(let ; instances have any globals?
							((all-instance-globals (fold (lambda (instance new-globals)
								(let ((instance-globals (hash-ref globals (net-instance:name instance))))
									(if instance-globals
										(begin ; add connections
											(set-cdr! (last-pair (net-instance:instance-connections instance))
												(map car instance-globals))
											(unsorted-uniq (lambda (p1 p2)
												(string=? (net-port:name p1) (net-port:name p2)))
												(append instance-globals new-globals))
										)
										new-globals
									)
								)
							) '() (cdr (net-circuit-decl:instances cell)))))
							(if (not (null? all-instance-globals))
								(begin ; munge the ports of this cell and make a note
									(hash-set! globals (net-circuit-decl:name cell) all-instance-globals)
									(set-cdr! (last-pair (net-circuit-decl:ports cell)) all-instance-globals)
								)
							)
							all-instance-globals
						)
					)
				)
			) '() netlist)
		)
	)
))

;;; balsa-netlist-add-global-rails!: add global rails to netlist cells -adds
;;; gnd and vdd nets to any circuits that need them, except for the top-level cell
;;; which will have supply0 and supply1.
(define balsa-netlist-add-global-rails! (lambda (netlist)
	(let*
		((globals (make-hash-table 157))
		 (top-level-cell (tech-map-cell-name (tech-mangle-breeze-part-name balsa-netlist-top-level-cell)))
		 (vcc-net-name (tech-bundle-name 'data tech-vcc-net-name 0)) ; bundle-name smashes global net prefices!
		 (gnd-net-name (tech-bundle-name 'data tech-gnd-net-name 0))
		 (vcc-port-decl (list (list vcc-net-name 'inout 1)))
		 (gnd-port-decl (list (list  gnd-net-name 'inout 1)))
		)
		(for-each (lambda (cell)
			(let*
				((cell-name (net-circuit-decl:name cell))
				 (top-level (string=? cell-name top-level-cell))
				 (uses-vcc (if (net-circuit-decl-uses-net-name cell vcc-net-name) vcc-port-decl '()))
				 (uses-gnd (if (net-circuit-decl-uses-net-name cell gnd-net-name) gnd-port-decl '()))
				 (rails (append uses-vcc uses-gnd))
				 (update-hash
					 (if (not (or (null? rails) top-level))
							(hash-set! globals cell-name rails)
					 )
				 )
				 (all-instance-globals (fold (lambda (instance new-globals)
					(if (eqv? (car instance) 'instance)
						(let 
							((instance-globals (hash-ref globals (net-instance:name instance))))
							(if instance-globals
								(begin ; add connections
									(set-cdr! (last-pair (net-instance:instance-connections instance))
									(map car instance-globals))
									(unsorted-uniq (lambda (p1 p2)
									(string=? (net-port:name p1) (net-port:name p2)))
									(append instance-globals new-globals))
								)
								new-globals
							)
						)
						new-globals
					)
				 ) rails (cdr (net-circuit-decl:instances cell))))
				)
				(if (not (or (null? all-instance-globals) top-level)) ;; leave top-level cell alone as will have supply0 and supply1
					(begin ; munge the ports of this cell and make a note
						(hash-set! globals cell-name all-instance-globals)
						(set-cdr! (last-pair (net-circuit-decl:ports cell)) all-instance-globals)
					)
				)
			)
		) netlist)
	)
))

