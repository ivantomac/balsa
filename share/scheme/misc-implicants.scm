;;;
;;; The Balsa Asynchronous Hardware Synthesis System
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
;;;	`misc-implicants.scm'
;;;	Procedures for manipulating implicants (base-value . dont-cares)
;;;

;;; implicant:{base-value,dont-cares}: access functions
(define implicant:base-value car)
(define implicant:dont-cares cdr)

;;; NB. These procedures require the procedures: lognot, logand, logior and bit-extract
;;;   to be defined

;;; zero-bottom-bits: returns `val' with the bottom `n' bits replaced by zeroes
(define zero-bottom-bits (lambda (val n)
	(* (expt 2 n) (shift-right val n))
))

;;; make-implicant-list-for-range: Returns a list of implicants
;;;	  to represent the range [start,end]. eg. list element (12 . 3) represents the implicant 11--
(define make-implicant-list-for-range (lambda (start end)
	(letrec
		; find the implicants from base to end and add to the implicants list (tail recurs.)
		((find-implicants (lambda (base implicants)
			(letrec
				; count the number of don't cares which we can safely insert here
				((count-dont-cares (lambda (mask bitNo)
					(if (or (> base (zero-bottom-bits base (+ 1 bitNo)))
							(< end (logior mask base))) ; range too big
						bitNo
						(count-dont-cares (logior 1 (ash mask 1)) (+ 1 bitNo)) ; extend mask, continue
					)
				)))
				(if (> base end) ; past the end of range?
					implicants
					(let
						((bitNo (count-dont-cares 1 0)))
						(find-implicants (+ base (ash 1 bitNo))
							(cons (cons base (- (expt 2 bitNo) 1)) implicants))
					)
				)
			)
		)))
		(find-implicants start '())
	)
))

;;; implicant->string: make a bitwise little-endian of an implicant using - as don't care
(define implicant->string (lambda (implicant width)
	(let
		((ret (make-string width #\0)))
		(for.. (lambda (index)
			(cond
				((logbit? index (implicant:dont-cares implicant))
					(string-set! ret index #\-))
				((logbit? index (implicant:base-value implicant))
					(string-set! ret index #\1))
			)
		) 0 (- width 1))
		ret
	)
))

;;; complement-implicant: complement a given implicant in a field of `width'
;;;		bits. 0 -> 1, 1 -> 0, X -> X
;;;	NB. requires that dc bits in implicant are 0
(define complement-implicant (lambda (implicant width)
	(let
		((value (implicant:base-value implicant))
		 (dcs (implicant:dont-cares implicant))
		 (mask (- (expt 2 width) 1))
		)
		(cons (- mask (logand value mask) (logand dcs mask)) dcs)
	)
))

;;; remove-implicant-from-implicant: remove all the values covered by implicant `r'
;;;		from implicant `l' and return a list of new implicants corresponding to that
;;;		smaller set of values.  `l' and `r' must not be disjoint.
(define remove-implicant-from-implicant (lambda (l r)
	(letrec
		((tail (lambda (v remaining-dont-cares bit-no ret)
			(if (zero? remaining-dont-cares)
				ret
				(cond
					((logbit? bit-no (implicant:dont-cares r)) ; don't care to the right
						(tail v (bit-clear remaining-dont-cares bit-no) (+ 1 bit-no) ret)
					)
					((logbit? bit-no (implicant:dont-cares l)) ; don't care to the left
						(tail
							(cons
								(if (logbit? bit-no (implicant:base-value r)) ; 1 in r -> 1 in l
									(bit-set (implicant:base-value v) bit-no)
									(implicant:base-value v)
								)
								(bit-clear (implicant:dont-cares v) bit-no)
							)
							(bit-clear remaining-dont-cares bit-no)
							(+ 1 bit-no)
							(cons
								(cons
									(if (not (logbit? bit-no (implicant:base-value r))) ; 0 -> 1 in l
										(bit-set (implicant:base-value v) bit-no)
										(implicant:base-value v)
									)
									(bit-clear (implicant:dont-cares v) bit-no)
								)
								ret
							)
						)
					)
					(else (tail v remaining-dont-cares (+ 1 bit-no) ret))
				)
			)
		)))
		(tail l (implicant:dont-cares l) 0 '())
	)
))

;;; implicants-disjoint?: returns #t if `l' and `r' cover none of the same values
(define implicants-disjoint? (lambda (l r)
	(/=
		(logior (implicant:base-value r)
			(logand (implicant:base-value l) (implicant:dont-cares r)))
		(logior (implicant:base-value l)
			(logand (implicant:base-value r) (implicant:dont-cares l)))
	)
))

;;; remove-implicant-from-implicants: as above but work with a list of implicants
(define remove-implicant-from-implicants (lambda (is i)
	(fold (lambda (working-imp ret)
		(append ret
			(if (implicants-disjoint? working-imp i)
				(list working-imp)
				(remove-implicant-from-implicant working-imp i)
			)
		)
	) '() is)
))

;;; remove-implicants-from-implicants: ditto but remove all the right hand implicants
(define remove-implicants-from-implicants (lambda (ls rs)
	(fold (lambda (r ret)
		(remove-implicant-from-implicants ret r)
	) ls rs)
))

;;; remaining-implicants: find a set of implicants which cover all the values
;;;		in a field of `width' bits not yet covered by `implicants'
(define remaining-implicants (lambda (implicants width)
	(remove-implicants-from-implicants
		(list (cons 0 (- (expt 2 width) 1)))
		implicants
	)
))

;;; remaining-implicants-from-implicantss: like above but process a list of lists
;;;		of implicants
(define remaining-implicants-from-implicantss (lambda (implicantss width)
	(fold (lambda (imps complement-imps)
		(remove-implicants-from-implicants complement-imps imps)
	) (list (cons 0 (- (expt 2 width) 1))) implicantss)
))

(define enlarge-imp (lambda (i l)
	(let*
		((i-w/o-l-dcs (logand (implicant:base-value i) (lognot (implicant:dont-cares l))))
		 (i-xor-l (logxor i-w/o-l-dcs (implicant:base-value l)))
		 (i-xor-l-minus-1 (- i-xor-l 1))
		 (exactly-one-diff-between-i-and-l
		 	(zero? (logand i-xor-l i-xor-l-minus-1))
		 )
		)
		(if exactly-one-diff-between-i-and-l
			(cons
				(logand (implicant:base-value i) (lognot i-xor-l))
				(logior (implicant:dont-cares i) i-xor-l)
			)
			i
		)
	)
))

