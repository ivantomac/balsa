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
;;;	`base.scm'
;;;	Base definitions for Guile Scheme environment for Balsa
;;;	share/scheme/base.scm.  Generated from base.scm.in by configure.
;;;

(debug-set! stack 0)

;;; slash-terminate-string: add a slash to the end of the given string
;;;		if it doesn't already have one
(define slash-terminate-string (lambda (str)
	(if (eqv? #\/ (string-ref str (- (string-length str) 1)))
		str
		(string-append str "/")
	)
))

;;; balsa-scheme-simple-import: if not already defined, this is how to load other files in the
;;;		scheme-src-dir directory.
(define balsa-scheme-simple-import (lambda (m)
	(let
		((balsa-env-home (getenv "BALSAHOME")))
		(if balsa-env-home
			(load (string-append balsa-env-home "/share/scheme/" m))
			(load (string-append "/home/amulinks/balsa/linux/4.0/share/scheme/" m))
		)
	)
))

;;; load miscellaneous library functions
(balsa-scheme-simple-import "misc.scm")
;;; and globals
(balsa-scheme-simple-import "globals.scm")

;;; balsa-scheme-import: find balsa scheme files from a path list of symbols.
;;;		If a single symbol is given then the file scheme-src-dir/name.scm is sought
;;;		For more than one symbol the file scheme-src-dir/1stsym/2ndsym/.../lastsym/1stsym-lastsym.scm
;;;		should be loaded.
(define balsa-scheme-import (lambda module-path
	(if (and balsa-loaded-modules (not (hash-ref balsa-loaded-modules module-path)))
		(let*
			((name-parts (map (lambda (sym) (string-downcase (symbol->string sym))) module-path))
			 (filename (string-append scheme-src-dir (build-separated-string name-parts "-") ".scm"))
			)
			(hash-set! balsa-loaded-modules module-path #t)
			(load filename)
		)
	)
))

;;; parse-and-set-tech-and-style: set the technology and style variables from the
;;;		BALSATECH string tech.
(define parse-and-set-tech-and-style (lambda (tech)
	(let*
		((breeze-tech-str-len (string-length tech))
		 (tech-slash-pos (strchr tech #\/))
		)
		(if tech-slash-pos
			(begin
				(let*
					((rest-of-tech-string (substring tech (+ 1 tech-slash-pos) breeze-tech-str-len))
					 (args-slash-pos (strchr rest-of-tech-string #\/))
					 (style (if args-slash-pos
						(substring rest-of-tech-string 0 args-slash-pos)
						rest-of-tech-string
					 ))
					 (style-options-string (if args-slash-pos
						(substring rest-of-tech-string (+ 1 args-slash-pos) (string-length rest-of-tech-string))
						#f
					 ))
					)
					(if (not (string=? style "")) (set! breeze-style style))
					(if style-options-string
						(set! breeze-style-options
							(reverse! (fold (lambda (arg args)
								(if (string=? arg "")
									args
									(let ((arg-parts (parse-string arg #\=)))
										(if (= 1 (length arg-parts))
											(cons (cons (car arg-parts) #t) args)
											(cons (cons (car arg-parts) (cadr arg-parts)) args)
										)
									)
								)
							) '() (parse-string style-options-string ":")))
						)
						(set! breeze-style-options '())
					)
				)
				(set! breeze-tech (substring tech 0 tech-slash-pos))
			)
			(set! breeze-tech tech)
		)
	)
))

;;; balsa-set-tech: set the balsa technology variable from either the
;;;		given argument, or if the argument is #f, from BALSATECH or to "common" if that isn't set
(define balsa-set-tech (lambda (tech)
	(parse-and-set-tech-and-style
		(if tech
			tech
			(let ((breeze-env-tech (getenv "BALSATECH")))
				(if breeze-env-tech breeze-env-tech "common")
			)
		)
	)
	(set! breeze-tech-dir (string-append data-dir "tech/" breeze-tech "/"))
	(set! breeze-style-dir (string-append data-dir "style/" breeze-style "/"))
	(set! breeze-primitives-file (string-append breeze-tech-dir "components.abs"))
	(set! breeze-gates-mapping-file (string-append breeze-tech-dir "gate-mappings.scm"))
))

;;; balsa-init: initialise Balsa system globals
(define balsa-init (lambda ()
	(let
		((balsa-env-home (getenv "BALSAHOME"))
		 (balsa-path-env (getenv "BALSAPATH"))
		)
		(set! balsa-loaded-modules (make-hash-table 10))
		(hash-set! balsa-loaded-modules '(misc) #t)
		(if balsa-env-home
			(begin
				(set! balsa-home (slash-terminate-string balsa-env-home))
				(set! data-dir (slash-terminate-string (string-append balsa-env-home "/share")))
				(set! scheme-src-dir (string-append data-dir "scheme/"))
			)
		)
		(set! breeze-primitives-figures-dir (string-append balsa-home "/doc/figures/components/"))
		(if balsa-path-env
			(set! balsa-search-path (parse-string balsa-path-env #\:))
			(if balsa-env-home
				(set! balsa-search-path (parse-string (string-append balsa-home "share") #\:))
			)
		)
		(set! balsa-search-path (cons "." balsa-search-path))
	)
))

