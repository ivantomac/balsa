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
;;;	`gen-mofn-espresso.scm'
;;;	Interface to Berkeley Espresso for m-of-n logic minimisation
;;;

(balsa-scheme-import 'misc 'implicants)
(balsa-scheme-import 'gen 'implicants)
(balsa-scheme-import 'gen 'espresso)

;;; gen-print-mofn-espresso-file: print out a suitable espresso input file for the given `implicantss'
;;;		with `width' bits for encoding/decoding.  Inputs are named "i"n and outputs "o"n in that file.
;;;		If `decodeNencode' is true then print a file from binary to one-hot, otherwise produce an encoding
;;;		from one-hot to binary using chosen implicant mapping file.
(define gen-print-mofn-espresso-file (lambda (implicantss width decodeNencode mapping-func map-args)
	(let*
		((imp-set-count (length implicantss))
		 (input-count (if decodeNencode width imp-set-count))
		 (output-count (if decodeNencode imp-set-count width))
		 (defines (car map-args))
		 (actual (cadr map-args))
		 (formal ((nth 2) map-args))
		 (specify-expr (lambda (expr) (brz-specify-expression expr defines actual formal)))
		)
		; header
		(print ".i " input-count #\newline ".o " output-count #\newline)
		(print ".ilb ")
		(for-each (lambda (input-no) (print #\i input-no #\space))
			(integer-range-list (- input-count 1) 0))
		(print #\newline ".ob ")
		(for-each (lambda (output-no) (print #\o output-no " "))
			(integer-range-list (- output-count 1) 0))
		(print #\newline ".p " imp-set-count #\newline)
		; implicants
		(for-each (lambda (decoded-pos/imp-list)
			(for-each (lambda (imp)
				(let*
					((express (list mapping-func imp width))
					 (decoded (gen-one-hot-string imp-set-count (car decoded-pos/imp-list)
						(if decodeNencode #\0 #\-)))
					 (str-imp (brz-specify-expression express defines actual formal))
					)
					(string-reverse! decoded)
					(for.. (lambda (index)
						(if decodeNencode
							(print (list-ref str-imp index) #\space decoded)
							(print decoded #\space (list-ref str-imp index))
						)
						(newline)
					) 0 (- (length str-imp) 1))
				)
			) (cadr decoded-pos/imp-list))
		) (zip (integer-range-list (- imp-set-count 1) 0) implicantss))
		(print ".e" #\newline)
	)
))

;;; gen-parse-mofn-dc-term: parse a string like [01-]+ into a list of pairs describing
;;;		the bit position and group position in that string (assuming
;;;		the string is bitwise little endian).
;;;		eg. "-11-1" => (((0 . 1)) ((1 . 0)) ((2 . 1))) for dual-rail group size two. 
;;; 	Will work for m-of-n codes and codes were the most significant bit is encoded differently
(define gen-parse-mofn-dc-term (lambda (term group-size)
	(let ((term-length (string-length term)))
		(letrec
			((parse-tail (lambda (position working-term)
				(let*
					((group-no (quotient position group-size))
					(odd-group (> (+ position group-size) term-length))
					(group-length (if odd-group
						(- term-length position)
						group-size
					))
					)
					(if (= position term-length)
						working-term
						(parse-tail (+ position group-length)
							(cons
								(fold (lambda (index res)
									(if (char=? (string-ref term (+ position index)) #\1) 
										(cons (cons group-no index) res)
										 res
									)
							  ) `() (integer-range-list 0 (- group-length 1)))
								working-term
							)
						)
					)
				)
			)))
			(reverse (parse-tail 0 '()))
		)
	)
))

;;; gen-make-mofn-sop-for-espresso-file: make an SOP implementation of the given espresso file.
;;;		The espresso file's pla lines should be of the form [01-]+ [01-]+ where the first
;;;		term is an input `AND' term (returned in the result as a list of (position . sense)
;;;		pairs specifying whihc input bits and of which sense are included in that term).
;;;		The second term on each line specifies the desired sense of the outputs when the AND
;;;		term is true.  Only 1's in the second term are significant to gen-make-sop-for-espresso-file
;;;		which produces a list or `or-terms' indexable by output bit position (little endian) each
;;;		element of which is a list of AND term indices for terms which can be or-ed together to form this output.
;;;		Returns a pair: (and-terms . or-terms).
;;;		eg. A file containing the lines:
;;;			1------- 0100
;;;			--1----- 0101
;;;			----1--- 0110
;;;			------1- 0111
;;;		produces the output (for dual-rail) =>
;;;		and-terms = ((((0 . 0))) (((1 . 0))) (((2 . 0))) (((3 . 0)))
;;;		or-terms  = (() (0 1 2 3) (2 3) (1 3))
;;;		Giving the effective expressions:
;;;			term0 = input3_1d
;;;			term1 = input2_1d
;;;			term2 = input1_1d
;;;			term3 = input0_1d
;;;			output0 = 0
;;;			output1 = 1
;;;			output2 = term2 | term3	
;;;			output3 = term1 | term3	
;;;		NB. an empty or-term corresponds to an output permanently connected low.  An or-term
;;;		containing all the input terms corresponds to an output permanently connected high.
(define gen-make-mofn-sop-for-espresso-file (lambda (filename group-size)
	(let*	
		((ic/oc/term-pairs (gen-read-espresso-file-terms filename))
		 (input-count (car ic/oc/term-pairs))
		 (output-count (cadr ic/oc/term-pairs))
		 (term-pairs (cddr ic/oc/term-pairs))
		 (output-terms (list->vector (map cadr term-pairs)))
		 (and-terms ; make a list of (input-pos.input-sense-list) pairs
			(map (lambda (term-pair) (gen-parse-mofn-dc-term (car term-pair) group-size)) term-pairs)
		 )
		 (or-terms
		 	(reverse! (fold (lambda (index res)
				(cons
					(filter (lambda (and-term-index)
								(eqv? #\1 (string-ref (vector-ref output-terms and-term-index) index))
							) (.. 0 (- (length and-terms) 1)))
					res
				)		
			) '() (.. 0 (- output-count 1)))) ;; need to reverse as "Consing" on the front
		))
		(list
			and-terms
			or-terms
		)
		;and->or-connections
	)
))
