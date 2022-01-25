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
;;;	`gen-espresso.scm'
;;;	Interface to Berkeley Espresso for logic minimisation
;;;

(balsa-scheme-import 'misc 'implicants)
(balsa-scheme-import 'gen 'implicants)

;;; gen-one-hot-string: make a binary string of length `width' with a single #\1
;;;		at position `pos' and char `dont-care' in other bit positions
(define gen-one-hot-string (lambda (width pos dont-care)
	(let
		((str (make-string width dont-care)))
		(string-set! str pos #\1)
		str
	)
))

;;; gen-print-espresso-file: print out a suitable espresso input file for the given `implicantss'
;;;		with `width' bits for encoding/decoding.  Inputs are named "i"n and outputs "o"n in that file.
;;;		If `decodeNencode' is true then print a file from binary to one-hot, otherwise produce an encoding
;;;		from one-hot to binary.
(define gen-print-espresso-file (lambda (implicantss width decodeNencode)
	(let*
		((imp-set-count (length implicantss))
		 (input-count (if decodeNencode width imp-set-count))
		 (output-count (if decodeNencode imp-set-count width))
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
				(let
					((decoded (gen-one-hot-string imp-set-count (car decoded-pos/imp-list)
						(if decodeNencode #\0 #\-)))
					 (str-imp (implicant->string imp width))
					)
					(string-reverse! str-imp)
					(if decodeNencode
						(print str-imp #\space decoded)
						(print decoded #\space str-imp)
					)
				)
				(newline)
			) (cadr decoded-pos/imp-list))
		) (zip (integer-range-list (- imp-set-count 1) 0) implicantss))
		(print ".e" #\newline)
	)
))

;;; gen-parse-dc-term: parse a string like [01-]+ into a list of pairs describing
;;;		the position and sense of 0s and 1s in that string (assuming
;;;		the string is bitwise big endian).
;;;		eg. "01-1" => ((3 . #f) (2 . #t) (0 . #t))
(define gen-parse-dc-term (lambda (term)
	(let ((term-length (string-length term)))
		(letrec
			((parse-tail (lambda (index working-term) ; test bits in reverse order to string pos.
				(if (< index 0)
					working-term
					(parse-tail (- index 1)
						(case (string-ref term index)
							((#\-) working-term)
							((#\0 #\1) (cons
								; report index as a bit pos. not string pos.
								(cons (- term-length (+ 1 index)) (eqv? (string-ref term index) #\1))
								working-term
							))
						)
					)
				)
			)))
			(parse-tail (- term-length 1) '())
		)
	)
))

;;; gen-read-espresso-file-terms: read out lines from an espresso output (or input) file
;;;		which start without a dot.  Split those lines and return the fragements as a string list list.
;;;		Returns (input-count output-count . fragmented-term-lines)
(define gen-read-espresso-file-terms (lambda (filename)
	(let*
		((contents (get-file-with-reader filename read-lines))
		 (input-count 0)
		 (output-count 0)
		 (terms (filter-map
			(lambda (line) (if (not (eqv? (string-ref line 0) #\.))
				(parse-string line " ")
				(let ((parsed-line (parse-string line " ")))
					(cond
						((string=? (car parsed-line) ".i")
							(set! input-count (string->number (cadr parsed-line)))
						)
						((string=? (car parsed-line) ".o")
							(set! output-count (string->number (cadr parsed-line)))
						)
					)
					#f
				)
			))
			contents
		 ))
		)
		(cons input-count (cons output-count terms))
	)
))

;;; gen-make-sop-for-espresso-file: make an SOP implementation of the given espresso file.
;;;		The espresso file's pla lines should be of the form [01-]+ [01-]+ where the first
;;;		term is an input `AND' term (returned in the result as a list of (position . sense)
;;;		pairs specifying whihc input bits and of which sense are included in that term).
;;;		The second term on each line specifies the desired sense of the outputs when the AND
;;;		term is true.  Only 1's in the second term are significant to gen-make-sop-for-espresso-file
;;;		which produces a list or `or-terms' indexable by output bit position (little endian) each
;;;		element of which is a list of AND term indices for terms which can be or-ed together to form this output.
;;;		Returns a pair: (and-terms . or-terms).
;;;		eg. A file containing the lines:
;;;			1--- 0100
;;;			-1-- 0101
;;;			--1- 0110
;;;			---1 0111
;;;		produces the output =>
;;;		and-terms = (((3 . #t)) ((2 . #t)) ((1 . #t)) ((0 . #t)))
;;;		or-terms  = (() (0 1 2 3) (2 3) (1 3))
;;;		Giving the effective expressions:
;;;			term0 = input3
;;;			term1 = input2
;;;			term2 = input1
;;;			term3 = input0
;;;			output0 = 0
;;;			output1 = 1
;;;			output2 = term2 | term3	
;;;			output3 = term1 | term3	
;;;		NB. an empty or-term corresponds to an output permanently connected low.  An or-term
;;;		containing all the input terms corresponds to an output permanently connected high.
(define gen-make-sop-for-espresso-file (lambda (filename)
	(let*	
		((ic/oc/term-pairs (gen-read-espresso-file-terms filename))
		 (input-count (car ic/oc/term-pairs))
		 (output-count (cadr ic/oc/term-pairs))
		 (term-pairs (cddr ic/oc/term-pairs))
		 (and-terms ; make a list of (input-pos.input-sense-list) pairs
			(map (lambda (term-pair) (gen-parse-dc-term (car term-pair))) term-pairs)
		 )
		 (output-terms (list->vector (map cadr term-pairs)))
		)
		(cons
			and-terms
			(letrec
				; output-term: find all the terms which have a one in the output bit at index output-index
				((output-term (lambda (output-index or-terms)
					(if (< output-index 0)
						or-terms
						(output-term (- output-index 1) (cons 
							(filter (lambda (and-term-index)
								(eqv? #\1 (string-ref (vector-ref output-terms and-term-index) output-index))
							) (.. 0 (- (length and-terms) 1)))
							or-terms
						))
					)
				)))
				(output-term (- output-count 1) '())
			)
		)
		;and->or-connections
	)
))
