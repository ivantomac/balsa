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
;;;	`balsa-make-test.scm'
;;;	Balsa test harness generator
;;;

(balsa-scheme-import 'brz)
(balsa-scheme-import 'misc 'switches)
(balsa-scheme-import 'misc 'banners)
(balsa-scheme-import 'gen 'implicants)

;;; print-test-harness-header: print the fancy header
(define print-test-harness-header (lambda ()
	(let
		((unix-name (let ((u-name (uname))) (if (vector? u-name) u-name
			#("unknown" "unknown" "unknown" "1" "unknown")))))
		(print "(--" #\newline)
		(print "    Balsa test harness" #\newline)
		(print "    Created: " (ctime) #\newline)
		(print "    By: " (vector-ref (getpw (getuid)) 0)
			"@" (vector-ref unix-name 1) " (" (vector-ref unix-name 0) ")" #\newline)
		(print "    With balsa-make-test version: " balsa-version #\newline)
		(print "    Command: balsa-make-test " balsa-test-command-line-string #\newline)
		(print "--)" #\newline #\newline)
	)
))

;;; print-balsa-type-name : print a Breeze type which can be used as a port (the sort
;;;		of types which are printed with StrTypeName in balsa-c) */
(define print-balsa-type-name (lambda (type)
	(case (car type)
		((numeric-type)
			(print (caddr type) (if (cadr type) "signed" "") " bits")
		)
		((array-type)
			(print "array " (cadr type)) ;; FIXME
		)
		((named-type)
			(print (cadr type))
		)
	)
))

;;; type-is-string?: returns true if the argument is of the form (named-type "String" ...)
(define type-is-string? (lambda (type)
	(and (pair? type) (eq? (car type) 'named-type) (pair? (cdr type)) (string=? (cadr type) "String"))
))

;;; print-test-harness-channels : print Balsa channel decls. for each of the given ports
(define print-test-harness-channels (lambda (ports)
	(for-each (lambda (port)
		(let ((name (brz-port:name port)))
			(print "  ")
			(case (car port)
				((sync-port)
					(print "sync " name)
					(newline)
				)
				((port)
					(print "channel " name " : ")
					(print-balsa-type-name (brz-port:type port))
					(newline)
				)
				((arrayed-port)
					(print "array " (brz-arrayed-port:port-count port) " of channel " name " : ")
					(print-balsa-type-name (brz-port:type port))
					(newline)
				)
				((sync-arrayed-port)
					(print "array " (brz-arrayed-sync-port:port-count port) " of sync " name #\newline)
				)
			)
		)
	) ports)
))

;;; print-test-harness-test-descriptions : print the attached commands for each test description line
(define print-test-harness-test-descriptions (lambda (tests ports)
	(let ((find-port (lambda (name) (find-with-predicate ports (lambda (e) (string=? name (brz-port:name e))))))
		  (unarrayed-name (lambda (name) (car (parse-string name #\[))))
		  )
		(for-each (lambda (test)
			(print " ||" #\newline "  ")
			(case (car test)
				((sync-port)
					(let ((name (cadr test)))
						(print "BalsaPrintSyncPortActivity (\"" name "\", " name ")")
					)
				)
				((input-port) (let*
					((name (cadr test))
					 (type (brz-port:type (find-port (unarrayed-name name))))
					 (source (caddr test))
					 (source-func (cdr (assoc source
					 	'((value . "BalsaInputPortFromValue") (file . "BalsaInputPortFromFile")))))
					)
					(print source-func " (")
					(print-balsa-type-name type)
					(print ", \"" name "\", ")

					(case source
						((value)
							(print "<- ")
							(let
								((value (list-ref test 3)))
							  (if (type-is-string? type)
								  (print "\"" value "\", ")
								  (begin
									(print "(" (if (string=? value "") "0" value) " as ")
									(print-balsa-type-name type)
									(print "), ")
								  )
							  )
							)
						)
						((file)
							(print "<- FileOpen (\"" (list-ref test 3) "\", read), ")
						)
					)

					(print name ")")
				))
				((output-port) (let*
					((name (cadr test))
					 (port (find-port (unarrayed-name name)))
					 (dest (caddr test))
					 (dest-func (cdr (assoc dest 
					 	'((log . "BalsaOutputPortToLog") (file . "BalsaOutputPortToFile")))))
					 (format-list (cddddr test))
					)
					(print dest-func)
					(if (>= (length format-list) 3) (print "WithFormat"))
					(print " (");
					(if (eq? #f port)
						(begin
						  (print-err "Error: Port '" name "' defined in the Project test but not in the associated Balsa procedure." #\newline)
						  (set! balsa-fatal-errors-detected #t)
						)
						(print-balsa-type-name (brz-port:type port))
					)
					(print ", \"" name "\", ")

					(if (>= (length format-list) 3) (print (car format-list) ", " (cadr format-list) ", " (caddr format-list) ", "))
					(case dest
						((file)
							(print "<- FileOpen (\"" (list-ref test 3) "\", write), ")
						)
					)

					(print name)
					(print ")")
				))
				(else
					(print "(-- Unknown test type `" (car test) "' --)")
				)
			)
		) tests)
	)
))

;;; balsa-test-make-test-harness : print out a Balsa test harness
(define balsa-test-make-test-harness (lambda (test)
	(let*
		((name (balsa-test-test:name test))
		 (dut-block-file-name (apply string-append (find-filename name "breeze" #t balsa-test-import-path)))
		 (dut-name (balsa-test-test:dut-name test))
		 (file-name (balsa-test-test:file-name test))
		 (ports (balsa-test-test:ports test))
		 (tests (balsa-test-test:descriptions test))
		 (output-file-name (if balsa-test-output-file-name
			balsa-test-output-file-name file-name))
		 (project-file-name "Project")
		)
		(if (and balsa-test-no-clobber (file-exists? output-file-name) (file-ctime<=? project-file-name output-file-name))
			(begin
				(print-err "balsa-make-test: output file exists, not regenerating" #\newline)
				(if balsa-test-touch
					(if (file-ctime<=? dut-block-file-name output-file-name)
						(print-err "balsa-make-test: output file is new enough, not touch(1)ing" #\newline)
						(begin
							(print-err "balsa-make-test: touch(1)ing output file" #\newline)
							(system (string-append "touch \"" output-file-name "\""))
						)
					)
				)
				(exit 0)
			)
			(with-output-to-file output-file-name
				(lambda ()
					(print-test-harness-header)
					(print "import [balsa.sim.portio]" #\newline #\newline)
					(print "import [" name "]" #\newline #\newline)
					(print "procedure " balsa-test-top-procedure-name #\newline "is" #\newline)
					(print-test-harness-channels ports)
					(print "begin" #\newline)
					(print "  " dut-name " (" (build-separated-string (map brz-port:name ports) ", ") ")")
					(print-test-harness-test-descriptions tests ports)
					(newline)
					(print "end" #\newline)
				)
			)
		)
		(if (eq? balsa-fatal-errors-detected #t)
			(begin
			  (print-err "Deleting output file" #\newline)
			  (system (string-append "rm \"" output-file-name "\""))
			  (exit 1)
			)
		)
	)
))

;;; dot-to-underscores : a.b.c -> a_b_c
(define dots-to-underscores (lambda (string)
	(list->string (map (lambda (chr)
		(if (eq? chr #\.) #\_ chr)
	) (string->list string)))
))

;;; balsa-test-make-default-port-description : make the default Project file like
;;;		test description for a port. Returns #f to mean no description
(define balsa-test-make-default-port-description (lambda (port)
	(let ((name (brz-port:name port)))
		(case (car port)
			((sync-port) 
				(list 'sync-port name)
			)
			((port)
				(case (brz-port:direction port)
					((input) (list 'input-port name 'value "0"))
					((output) (list 'output-port name 'log "0" ""))
					(else #f)
				)
			)
			((arrayed-port)
			 (map (lambda (index)
					(case (brz-port:direction port)
					  ((input) (list 'input-port (string-append name "[" (number->string index) "]") 'value "0"))
					  ((output) (list 'output-port (string-append name "[" (number->string index) "]") 'log "0" ""))
					  (else #f)
					  )
					)(integer-range-list 0 (- (brz-arrayed-port:port-count port) 1)))
			)
			((sync-arrayed-port)
				#f ; FIXME
			)
			(else #f)
		)
	)
))

;;; balsa-test-find-used-ports : returns a list of all the ports used in the given test descriptions
(define balsa-test-find-used-ports (lambda (test-descriptions)
	(fold (lambda (test names)
		(case (car test)
			((input-port output-port sync-port) (cons (car (parse-string (cadr test) #\[)) names))
			(else names)
		)
	) '() test-descriptions)
))

;;; balsa-test-make-test-structure : load a file and make a test for the named procedure using
;;;		the test descriptions given.
(define balsa-test-make-test-structure (lambda (filename part-name port-descriptions)
	(let*
		((path/name/ext (find-filename filename "breeze" #t balsa-test-import-path))
		 (path (car path/name/ext))
		 (name (cadr path/name/ext))
		 (decls/imports/imported-decls/visited-blocks (get-flattened-block
			(fold flip-string-append "" path/name/ext) "breeze" '() balsa-test-import-path))
		 (decls (car decls/imports/imported-decls/visited-blocks))
		 (imported-decls (caddr decls/imports/imported-decls/visited-blocks))
		 (part (find-headed-list-elem decls 'breeze-part part-name))
		 (imported-types (filter brz-check-type-decl? imported-decls))
		 (local-types (filter brz-check-type-decl? decls))
		 (types (append local-types imported-types))
		 (ports (if part (cddr (brz-breeze-part:ports part)) '()))
		 (used-port-names (balsa-test-find-used-ports port-descriptions))
		 ; new-port-descriptions : filled in defaults
		 (port-descs (fold (lambda (port descs)
		 	(if (member (brz-port:name port) used-port-names)
		 		descs
				(let ((new-desc (balsa-test-make-default-port-description port)))
					(if new-desc (case (car port) ((arrayed-port) (append new-desc descs)) (else (cons new-desc descs))) descs)
				)
		 	)
		 ) '() ports))
		)
		(if (not part)
			(error "balsa-make-test: can't find part `" part-name "' in file `" filename "'" #\newline)
			(list
				name
				(brz-breeze-part:name part)
				(string-append "test-" (dots-to-underscores name) "_" part-name ".balsa")
				ports
				(append! port-descs port-descriptions)
			)
		)
	)
))

;;; balsa-test-make-test-from-project : make a test structure from part of a Project file
(define balsa-test-make-test-from-project (lambda (project-dir test-name)
	(let*
		((project-file-name (string-append project-dir "/Project")))
		(if (not (file-exists? project-file-name))
			(error "balsa-make-test: can't read project file `" project-file-name "'" #\newline)
			(let
				((project-file-contents (get-file project-file-name)))
				(if (and (not (null? project-file-contents))
					(not (headed-list? (car project-file-contents) 'balsa-project)))
					(error "balsa-make-test: file `" project-file-name "' must contain a Balsa project" #\newline)
					(let*
						((files (find-headed-list-elem (cdar project-file-contents) 'files #f))
						 ; make a list of (top-level-name . test) for all the tests which match test-name
						 (tests (fold (lambda (top-level tests)
						 	(if (headed-list? top-level 'top-level #f)
						 		(fold (lambda (test tests)
									(if (and (headed-list? test 'test) (>= (length test) 3)
											(string? (cadr test)) (string=? (cadr test) test-name))
										(cons (cons (cadr top-level) test) tests)
										tests
									)
						 		) tests (cdr top-level))
						 		tests
						 	)
						 ) '() files))
						 (test (if (/= 1 (length tests))
						 	(error "balsa-make-test: can't find exactly one test named `" test-name "'" #\newline)
						 	(car tests)
						 ))
						 (test-balsa-file-dotted-path (car test))
						 (test-procedure-name (cadddr test))
						 (test-ports (let ((ports (find-headed-sub-list test 'ports)))
						 	(if ports (cdr ports) '())
						 ))
						 (default-test (balsa-test-make-test-structure test-balsa-file-dotted-path
						 	test-procedure-name test-ports))
						)
						(list
							test-balsa-file-dotted-path
							test-procedure-name
							(string-append "test-" test-name ".balsa")
							(balsa-test-test:ports default-test)
							(balsa-test-test:descriptions default-test)
						)
					)
				)
				; (test (balsa-test-make-test-structure filename part-name descs))
			)
		)
	)
))

;;; 
(define balsa-fatal-errors-detected #f)

;;; balsa-test-test:... : access functions for test description
(define balsa-test-test:name car)
(define balsa-test-test:dut-name cadr)
(define balsa-test-test:file-name caddr)
(define balsa-test-test:ports (nth 3))
;;; handled-ports : list of pairs of (port-name index) identifying which
;;;		ports have valid descriptions in :descriptions
(define balsa-test-test:descriptions (nth 4))

;;; balsa-test-{no-banner,...}: command line switches
(define balsa-test-no-banner #f)
(define balsa-test-output-file-name #f)
(define balsa-test-import-path balsa-search-path)
(define balsa-test-command-line-string "")
(define balsa-test-top-procedure-name "balsa")
(define balsa-test-no-clobber #f)
(define balsa-test-touch #f)
;;; balsa-test-type: type of test to create, (car balsa-test-type) should be one of
;;;		'default or 'project
(define balsa-test-type #f)

;;; balsa-test-print-banner: print the test-harness banner
(define balsa-test-print-banner (lambda ()
	(make-program-banner "balsa-make-test" "balsa-make-test: Balsa test harness generator"
		"2003-2008, The University of Manchester")
))

;;; balsa-test-usage: command usage
(define balsa-test-usage (lambda ()
	(balsa-test-print-banner)
	(error
		"version " balsa-version #\newline
		"usage: balsa-make-test (-p ... | -d ... ) {<switch>}*" #\newline #\newline
		"switches: -h or -?           - Display this message (--help)" #\newline
		"          -b                 - Don't print the test-harness banner (--no-banner)" #\newline
		"          -o <file-name>     - Output test harness file base name (--output-file)" #\newline
		"          -I <directory>     - Add named directory to the import path to find" #\newline
		"                               blocks (--import)" #\newline
		"          -T <name>          - Use a different top procedure name than `balsa_top'" #\newline
		"                               in the output file (--test-name)" #\newline
		"          -d <block-name> <procedure-name> - Produce a default test harness for" #\newline
		"                               the named procedure in the named file (--default)" #\newline
		"          -p <project-dir> <test-name> - Make a test harness (named \"sim_<test-name>.balsa\"" #\newline
		"                               for test <test-name> is the named project (--project)" #\newline
		"          -n                 - Don't regenerate output if file already exists" #\newline
		"                               (--no-clobber)" #\newline
		"          -t                 - Implies -n, touch(1) output file if it's older than input" #\newline
		"                               file (--touch)" #\newline
	)
))

;;; balsa-test-command-line-rules: command-line-args action rules
(define balsa-test-command-line-rules `(
	(#\o "output-file" 1 ,(lambda (args) (set! balsa-test-output-file-name (car args))))
	(#\b "no-banner" 0 ,(lambda (args) (set! balsa-test-no-banner #t)))
	(#\I "import" 1 ,(lambda (args) (set! balsa-test-import-path (append balsa-test-import-path (list (car args))))))
	(#\T "test-name" 1 ,(lambda (args) (set! balsa-test-top-procedure-name (car args))))
	(#\d "default" 2 ,(lambda (args) (set! balsa-test-type (list 'default (car args) (cadr args)))))
	(#\p "project" 2 ,(lambda (args) (set! balsa-test-type (list 'project (car args) (cadr args)))))
	(#\n "no-clobber" 0 ,(lambda (args) (set! balsa-test-no-clobber #t)))
	(#\t "touch" 0 ,(lambda (args) (set! balsa-test-no-clobber #t) (set! balsa-test-touch #t)))
	(#\h "help" 0 ,(lambda (args) (balsa-test-usage)))
	(#\? "help" 0 ,(lambda (args) (balsa-test-usage)))
))

;;; balsa-test-parse-command-line: parse switches from the given command line list, set
;;;		the balsa-test-... globals and return the tail of the command line.
(define balsa-test-parse-command-line (lambda (args)
	(set! balsa-test-command-line-string
		(build-separated-string (map bourne-escape-command-line-arg args) " "))
	(if (null? args)
		(balsa-test-usage)
		(let ((args-tail (parse-command-line "balsa-make-test" balsa-test-command-line-rules balsa-test-usage args)))
			(if (or (/= 0 (length args-tail)) (not balsa-test-type))
				(begin
					(print-err "balsa-make-test: use one of -p or -d to make a test harness" #\newline)
					(balsa-test-usage)
				)
				args-tail
			)
		)
	)
))

;;; balsa-test : top level
(define balsa-test (lambda ()
	(balsa-set-tech "common")
	(brz-load-primitives)
	(let
		((test (case (car balsa-test-type)
			((default) (balsa-test-make-test-structure (cadr balsa-test-type) (caddr balsa-test-type) '()))
			((project) (balsa-test-make-test-from-project (cadr balsa-test-type) (caddr balsa-test-type)))
		)))
		(if test (balsa-test-make-test-harness test))
	)
))

(top-level (lambda (args)
	(let
		((args-tail (balsa-test-parse-command-line args)))
		(if (not balsa-test-no-banner)
			(balsa-test-print-banner)
		)
		(apply balsa-test args-tail)
	)
) (cdr command-line-args))
