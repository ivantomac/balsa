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
;;;	`balsa-list-builtins'
;;;	List builtin library (absolute paths) required for a given Breeze file
;;;

(balsa-scheme-import 'brz)
(balsa-scheme-import 'misc 'switches)
(balsa-scheme-import 'misc 'banners)

;;; list-builtins-print-banner: print the list-builtins banner
(define list-builtins-print-banner (lambda ()
	(make-program-banner "balsa-list-builtins" "balsa-list-builtins: Builtin library lister"
		"2003, The University of Manchester")
))

;;; list-builtins-usage: command usage
(define list-builtins-usage (lambda ()
	(list-builtins-print-banner)
	(error
		"version " balsa-version #\newline
		"usage: balsa-list-builtins {<switch>}* <block/file-name>" #\newline #\newline
		"switches: -h or -?           - Display this message (--help)" #\newline
		"          -I <directory>     - Add named directory to the import path (--import)" #\newline
	)
))

;;; list-builtins-{...}: command line switch values
(define list-builtins-import-path balsa-search-path)

;;; list-builtins-command-line-rules: command-line-args action rules
(define list-builtins-command-line-rules `(
	(#\h "help" 0 ,(lambda (args) (list-builtins-usage)))
	(#\? "help" 0 ,(lambda (args) (list-builtins-usage)))
	(#\I "import" 1 ,(lambda (args) (set! list-builtins-import-path (append list-builtins-import-path
		(list (car args))))))
))

;;; list-builtins-parse-command-line: parse switches from the given command line list, set
;;;		the list-builtins-... globals and return the tail of the command line.
(define list-builtins-parse-command-line (lambda (args)
	(if (null? args)
		(list-builtins-usage)
		(let ((args-tail (parse-command-line "balsa-list-builtins" list-builtins-command-line-rules list-builtins-usage args)))
			(if (/= 1 (length args-tail))
				(error "balsa-list-builtins: expecting exactly one file name" #\newline)
				args-tail
			)
		)
	)
))

;;; brz-find-builtin-path: find the path to the builtin library (if any) associated with
;;;		the given dotted path.  The argument must be a dotted path w/o the enclosing brackets,
;;;		uses the given import path to find builtin libs by looking for a .la file in each
;;;		directory and returns the path to the library w/o a file extension.  eg.
;;;		"balsa.types.builtin" -> "/home/bardslea/balsa/balsa/share/balsa/types/builtin"
;;;		Returns #f if no builtin is found.
(define brz-find-builtin-path (lambda (dotted-path import-path)
	(let
		((slashed-path (replace-char dotted-path #\. #\/)))
		(let loop
			((paths-to-try import-path))
			(if (null? paths-to-try)
				#f
				(let ((test-path (string-append (car paths-to-try) "/" slashed-path)))
					(if (file-exists? (string-append test-path ".la"))
						test-path
						(loop (cdr paths-to-try))
					)
				)
			)
		)
	)
))

;;; list-builtins: main, reads from the file `filename'
(define list-builtins (lambda (filename)
	(let*
		((path/name/ext (find-filename filename "breeze" #f list-builtins-import-path))
		 (path (car path/name/ext))
		 (name (cadr path/name/ext))
		 (decls/imports/imported-decls/visited-blocks (get-flattened-block
			(fold flip-string-append "" path/name/ext) "breeze" '() list-builtins-import-path))
		 (visited-blocks (cadddr decls/imports/imported-decls/visited-blocks))
		)
		(for-each (lambda (block)
			(let ((path (brz-find-builtin-path block list-builtins-import-path)))
				(if path (print path #\newline))
			)
		) visited-blocks)
	)
))

(top-level (lambda (args)
	(let
		((args-tail (list-builtins-parse-command-line args)))
		(apply list-builtins args-tail)
	)
) (cdr command-line-args))
