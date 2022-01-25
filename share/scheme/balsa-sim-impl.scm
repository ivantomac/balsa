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
;;;	`balsa-sim-impl.scm'
;;;	Simulate an implementation
;;;

(balsa-scheme-import 'brz)
(balsa-scheme-import 'misc 'switches)
(balsa-scheme-import 'misc 'date)
(balsa-scheme-import 'misc 'banners)
(balsa-scheme-import 'brz 'tech)

;;; impl-sim-{no-banner,...}: command line switches
(define impl-sim-no-banner #f)
(define impl-sim-verbose #f)
(define impl-sim-breeze-file #f)
(define impl-sim-import-path balsa-search-path)
(define impl-sim-defines '())
(define impl-sim-tech #f)

;;; impl-sim-print-banner: print the test-harness banner
(define impl-sim-print-banner (lambda ()
	(make-program-banner "balsa-sim-impl" "balsa-sim-impl: Balsa test harness generator"
		"2003-2008, The University of Manchester")
))

;;; impl-sim-usage: command usage
(define impl-sim-usage (lambda ()
	(impl-sim-print-banner)
	(error
		"version " balsa-version #\newline
		"usage: balsa-sim-impl {<switch>}* <netlist-basename>" #\newline #\newline
		"switches: -h or -?        - Display this message (--help)" #\newline
		"          -b              - Don't print the test-harness banner (--no-banner)" #\newline
		"          -v              - Verbose (--verbose)" #\newline
		"          -B <block-name> - Identifies which Breeze file this netlist was" #\newline
		"                            made from so that builtin modules etc. can be" #\newline
		"                            picked up and passed to the simulation (--breeze)" #\newline
		"          -I <directory>     - Add named directory to the import path (--import)" #\newline
		"          -D <name> <value>  - Pass a name/value pair command line arg. to the" #\newline
		"                            simulation (--define)" #\newline
        "          -X <tech>       - Set `technology/style/style-opts' to use.  (--technology)" #\newline
        "                              Technology will be set by ${BALSATECH} env. var. if -X is not passed" #\newline
	)
))

;;; impl-sim-command-line-rules: command-line-args action rules
(define impl-sim-command-line-rules `(
	(#\b "no-banner" 0 ,(lambda (args) (set! impl-sim-no-banner #t)))
	(#\v "verbose" 0 ,(lambda (args) (set! impl-sim-verbose #t)))
	(#\B "breeze" 1 ,(lambda (args) (set! impl-sim-breeze-file (car args))))
	(#\h "help" 0 ,(lambda (args) (impl-sim-usage)))
	(#\? "help" 0 ,(lambda (args) (impl-sim-usage)))
	(#\I "import" 1 ,(lambda (args) (set! impl-sim-import-path (append! impl-sim-import-path
		(list (car args))))))
	(#\D "define" 2 ,(lambda (args) (set! impl-sim-defines (append! impl-sim-defines
		(list (cons (car args) (cadr args)))))))
    (#\X "technology" 1 ,(lambda (args) (set! impl-sim-tech (car args))))
))

;;; impl-sim-parse-command-line: parse switches from the given command line list, set
;;;		the impl-sim-... globals and return the tail of the command line.
(define impl-sim-parse-command-line (lambda (args)
	(if (null? args)
		(impl-sim-usage)
		(let ((args-tail (parse-command-line "balsa-sim-impl" impl-sim-command-line-rules impl-sim-usage args)))
			(if (/= 1 (length args-tail))
				(impl-sim-usage)
				args-tail
			)
		)
	)
))

;;; impl-sim : run the appropriate simulation program
(define impl-sim (lambda args
	(balsa-set-tech impl-sim-tech)
	(brz-get-technology)
	(if (or (not tech-netlist-format-name) (not (symbol? tech-netlist-format-name)))
		(error "balsa-impl-sim: the technology described by BALSATECH must choose a netlist format.")
	)
	(let*
		((netlist-format (symbol->string tech-netlist-format-name))
		 (sim-system (brz-find-style-option "sim"))
		 (defines (build-string (map (lambda (arg)
			(string-append (car arg) " " (bourne-escape-command-line-arg (cdr arg)))
		 ) impl-sim-defines) " -D " ""))
		 (simulation-command (string-append "balsa-" netlist-format "-sim "
		 	(if sim-system (string-append "-s " sim-system " ") "")
		 	(if impl-sim-breeze-file (string-append "-B " impl-sim-breeze-file " ") "")
		 	(build-string impl-sim-import-path "-I " " ")
			defines
		 ))
		)
		(if impl-sim-verbose (print (string-append simulation-command " "
			(build-separated-string (map bourne-escape-command-line-arg args) " ")) #\newline))
		(print "balsa-sim-impl: executing: " (string-append simulation-command " "
			(build-separated-string (map bourne-escape-command-line-arg args) " ")) #\newline)
		(system (string-append simulation-command " "
			(build-separated-string (map bourne-escape-command-line-arg args) " ")))
	)
))

(top-level (lambda (args)
	(let
		((args-tail (impl-sim-parse-command-line args)))
		(if (not impl-sim-no-banner)
			(impl-sim-print-banner)
		)
		(apply impl-sim args-tail)
	)
) (cdr command-line-args))
