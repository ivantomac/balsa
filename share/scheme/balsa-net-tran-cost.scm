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
;;;	`balsa-net-tran-cost.scm'
;;;	Netlist file cost estimator
;;;

(balsa-scheme-import 'misc 'switches)
(balsa-scheme-import 'misc 'banners)
(balsa-scheme-import 'net 'parser)
(balsa-scheme-import 'brz)
(balsa-scheme-import 'brz 'tech)

(define balsa-net-tran-cost-command-line (command-line))
(define prog-name (cadr balsa-net-tran-cost-command-line))

(define balsa-net-tran-cost-show-banner #t)
;(define balsa-net-tran-cost-show-messages 'all)
(define balsa-net-tran-cost-show-messages '(warning))
(define balsa-net-tran-cost-types '())
(define balsa-net-tran-cost-circuit-names '())
(define balsa-net-tran-cost-tech "example")

(define balsa-net-tran-cost-banner (lambda( )
	(make-program-banner 
		"balsa-net-tran-cost" 
		"balsa-net-tran-cost: Breeze transistor/area estimation" 
		"2001, The University of Manchester"
	)
))

;;; balsa-net-tran-cost-usage: command usage
(define balsa-net-tran-cost-usage (lambda ()
	(balsa-net-tran-cost-banner)
	(error
		"version " balsa-version #\newline
		"usage: balsa-net-tran-cost {<switch>}* {-c <cost-type>}* {-n <circuit-name>}* {<file>}*" #\newline #\newline
		"switches: -h or -?    - Display this message (--help)" #\newline
		"          -b          - Don't print the balsa-net-tran-cost banner (--no-banner)" #\newline
        "          -q          - Quiet. Don't display WARNING messages (dangerous) (--quiet)" #\newline
        "          -v          - Verbose. Display INFO messages. (--verbose)" #\newline
        "          -X <tech>   - Use the specified technology (default: example) (--technology)" #\newline
        "          -c <class>  - Calculate costs of type <class> (default: transistors) (--cost-type)" #\newline
        "                        using 'gates' gives a count of base cells (those w/o their own instances)" #\newline
        "          -n <name>   - Report statistics for the Balsa procedure <name> (--circuit-name)" #\newline
        #\newline
        "  If no circuit name is specified, all circuit names from all listed net files" #\newline
        "  will be reported. If no files are listed, " prog-name " will try to use the" #\newline
        "  name of the first circuit listed, with a \".net\" appended if it does not" #\newline
        "  already end in that." #\newline
	)
))

;;; balsa-net-tran-cost-command-line-rules: command-line-args action rules
(define balsa-net-tran-cost-command-line-rules `(
	(#\h "help" 0 ,(lambda (args) (balsa-net-tran-cost-usage)))
	(#\? "help" 0 ,(lambda (args) (balsa-net-tran-cost-usage)))
	(#\b "no-banner" 0 ,(lambda (args) (set! balsa-net-tran-cost-show-banner #f)))
	(#\q "quiet" 0 ,(lambda (args) (set! balsa-net-tran-cost-show-messages
		(delete 'warning balsa-net-tran-cost-show-messages))))
	(#\v "verbose" 0 ,(lambda (args) (set! balsa-net-tran-cost-show-messages
		(append balsa-net-tran-cost-show-messages '(info)))))
    (#\n "circuit-name" 1 ,(lambda (args) (set! balsa-net-tran-cost-circuit-names
    	(append balsa-net-tran-cost-circuit-names (list (car args))))))
    (#\X "technology" 1 ,(lambda (args) (set! balsa-net-tran-cost-tech (car args))))
	(#\c "cost-type" 1 ,(lambda (args) (set! balsa-net-tran-cost-types (append balsa-net-tran-cost-types 
		(list (string->symbol (string-downcase (car args))))))))
))

;;; balsa-net-tran-cost-parse-command-line: parse switches from the given command line list, set
;;;		the balsa-net-tran-cost-... globals and return the tail of the command line.
(define balsa-net-tran-cost-parse-command-line
	(lambda (args)
		(if (null? args)
			(balsa-net-tran-cost-usage)
			(parse-command-line "balsa-net-tran-cost"
				balsa-net-tran-cost-command-line-rules balsa-net-tran-cost-usage args)
		)
	)
)

(define balsa-net-tran-cost-circuit-initialise
	(lambda (cells-hash circuit)
		(let*
			((name (net-circuit-decl:name circuit))
			 (hash-val (hash-ref cells-hash name))
			)
			(if hash-val
				(if (not (equal? circuit hash-val))
					(if (member 'warning balsa-net-tran-cost-show-messages) 
						(print prog-name ": WARNING: attempted redefinition of circuit: " name #\newline)
					)
				)
				(hash-set! cells-hash name circuit)
			)
		)
	)
)

(define balsa-net-tran-cost-hash-circuits 
	(lambda (cells-hash circuit-list)
		(for-each (lambda (elem)
			(if (headed-list? elem 'circuit)
				(balsa-net-tran-cost-circuit-initialise cells-hash elem)
			)
		) circuit-list)
	)
)

;;; requires the hash cell-hash to be populated
(define balsa-net-tran-cost-find-circuit-cost
	(lambda (cell-hash cost-hash name type)
		(let*
			((cell (hash-ref cell-hash name)) ; its implementation
			 (cost (if cell (hash-ref cost-hash (list name type)) #f))
			 (base-cost (if (and cell (not cost)) (net-circuit-decl-find-attribute cell type) #f))
			 (base-cost2
			 	(if (or (not base-cost) (null? base-cost)) #f (if (null? (cdr base-cost)) #f (cadr base-cost))))
			 (instances (if (and cell (not cost) (not base-cost2)) (net-circuit-decl:instances cell) #f))
			 (instances2
			 	(if (or (not instances) (null? instances)) #f (if (null? (cdr instances)) #f (cdr instances))))
			)
			(if cell
				(if (not cost)
					(hash-set! cost-hash (list name type)
						(if base-cost2 base-cost2
							(if instances2
								(fold (lambda (instance acc) 
									(if (eqv? 'instance (car instance))
										(+ acc (balsa-net-tran-cost-find-circuit-cost
											cell-hash cost-hash (net-instance:name instance) type))
										acc
									)
								) 0 instances2)
								; AB 2008-08-29
								(if (eqv? type 'gates) 1 0)
							)
						)
					)
				)
				(error prog-name ": ERROR: unknown circuit: " name #\newline)
			)
		)

		(let ((cost (hash-ref cost-hash (list name type))))
			(if cost
				cost
				(let ()
					(if (member 'warning balsa-net-tran-cost-show-messages)
						(print prog-name ": WARNING: circuit has unknown " type " cost, defaulting to zero: "
							name #\newline)
					)
					0
				)
			)
		)
	)
)

(define balsa-net-tran-cost-load-netlist-file
	(lambda (cells-hash file)
		(let ((filename (if (final-string file ".net") file (string-append file ".net"))))
			(if (member 'info balsa-net-tran-cost-show-messages) 
				(print prog-name ": INFO: loading netlist (`" filename "') ... " #\newline)
			)
			(balsa-net-tran-cost-hash-circuits cells-hash (get-file filename))
			(if (member 'info balsa-net-tran-cost-show-messages) 
				(print prog-name ": INFO: loading netlist (`" filename "') ... done." #\newline)
			)
		)
	)
)

;;; the main event
(define balsa-net-tran-cost 
	(lambda ()
		(let*
			((balsa-net-tran-cost-file-names 
			 (balsa-net-tran-cost-parse-command-line (cddr balsa-net-tran-cost-command-line)))
			 (balsa-net-tran-cost-costs (make-hash-table 1))
			 (balsa-net-tran-cost-cells (make-hash-table 32))
			)
			(balsa-set-tech balsa-net-tran-cost-tech)

			(if balsa-net-tran-cost-show-banner (balsa-net-tran-cost-banner))

			(if (and (null? balsa-net-tran-cost-circuit-names) (null? balsa-net-tran-cost-file-names))
				(balsa-net-tran-cost-usage)
			)

			; (if (not breeze-tech) (balsa-set-tech "example"))

			(if (null? balsa-net-tran-cost-types)
				(set! balsa-net-tran-cost-types '(transistors))
			)

			(if (member 'info balsa-net-tran-cost-show-messages) 
				(print prog-name ": INFO: loading technology (`"
					(brz-pretty-format-tech-name) "') libraries ... " #\newline)
			)
			(brz-load-gates)
			(if (member 'info balsa-net-tran-cost-show-messages) 
				(print prog-name ": INFO: loading technology (`"
					(brz-pretty-format-tech-name) "') libraries ... done." #\newline)
			)
			(balsa-net-tran-cost-hash-circuits balsa-net-tran-cost-cells breeze-gate-defns)

			(let
				((files-to-load (if (null? balsa-net-tran-cost-file-names) (car circuit-names)
					balsa-net-tran-cost-file-names)))
				(for-each (lambda (file) (balsa-net-tran-cost-load-netlist-file balsa-net-tran-cost-cells file))
					files-to-load)
			)

			(let
				((cells-to-list
					(if (null? balsa-net-tran-cost-circuit-names)
						; (sort (map car (apply append (vector->list balsa-net-tran-cost-cells))) string<?)
						(sort (hash-fold (lambda (key value ret) (cons key ret)) '()
							balsa-net-tran-cost-cells) string<?)
						(map (lambda (name)
							(string-append tech-balsa-prefix 
								(let ((short-name (initial-string name tech-balsa-prefix)))
									(if short-name short-name name))
							)
						) balsa-net-tran-cost-circuit-names)
					)
				))
				(for-each (lambda (circuit-name)
					(if (initial-string circuit-name tech-balsa-prefix)
						(let
							((results (map (lambda (type) 
								(balsa-net-tran-cost-find-circuit-cost balsa-net-tran-cost-cells
									balsa-net-tran-cost-costs circuit-name type)
							) balsa-net-tran-cost-types)))

							(print (initial-string circuit-name tech-balsa-prefix) ": " (number->string (car results)))
							(map (lambda (n) (print ", " (number->string n))) (cdr results))
							(print #\newline)
						)
					)
				) cells-to-list)
			)
		)
	)
)

(balsa-net-tran-cost)

