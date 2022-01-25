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
;;; `brz-tech.scm'
;;; Setup technology options
;;;

(balsa-scheme-import 'net 'parser)

;;; Procedures to return the size of the portion according to the various protocols
;;; Return 0 if single wire, n if full width, and #f if not used.

;;; width-one: sync portions
(define width-one (lambda (width) 0))

;;; width-n: full width bundled data and dual rail
(define width-n id)

;;; width-dualrail/1of4: req0, req1 etc, dual rail/one of four  
(define width-dualrail/1of4 (lambda (width) (quotient (+ width 1) 2)))

;;; width-1of4: req2, req3 one of four
(define width-1of4 (lambda (width) 
	(let 
		((slice-width (quotient width 2)))
		(if (zero? slice-width) 
			#f
			slice-width
		)
	)
))

(define balsa-style-load-scheme (lambda (file)
	(let
		((style-description-file (string-append breeze-style-dir file ".scm")))
		(if (file-exists? style-description-file)
			(load style-description-file)
			(error "brz-get-technology: invalid style `" breeze-style "', can't find file `" breeze-style-dir
				file ".scm'")
		)
	)
))

(define brz-get-technology (lambda ()
	(if (not tech)
		(begin
			(let
				((tech-description-file (string-append breeze-tech-dir "startup.scm")))
				(if (file-exists? tech-description-file)
					(load tech-description-file)
					(error "brz-get-technology: invalid technology `" breeze-tech "', can't find file `"
						breeze-tech-dir "startup.scm'")
				)
			)
			(balsa-style-load-scheme "startup")
			; always include node in portions.
			(set! tech-node-portions `((node push ,width-n)))
			(set! tech-net-portions `((net push ,width-one)))
			(set! all-portions 
				(append tech-sync-channel-portions tech-push-channel-portions 
					tech-pull-channel-portions tech-node-portions tech-net-portions))
			(set! tech #t)
		)
	)
))
