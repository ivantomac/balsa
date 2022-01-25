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
;;;	`misc-spans.scm'
;;;	Procedures for manipulating pairs (lower . upper) of integers representing ranges
;;;	of values and lists of those pairs 
;;;

;;; span:{lower,upper}: access functions
(define span:lower car)
(define span:upper cdr)

;;; span<=?: <= predicate for span pairs
(define span<=? (lambda (lhs rhs)
	(if (= (span:lower lhs) (span:lower rhs)) ; same lower bound
		(<= (span:upper lhs) (span:upper rhs))
		(<= (span:lower lhs) (span:lower rhs))
	)
))

;;; span<?: < predicate for span pairs (first span contains values which are all less than those in the
;;;		second span.
(define span<? (lambda (lhs rhs)
	(and
		(< (span:lower lhs) (span:lower rhs))
		(< (span:upper lhs) (span:upper rhs))
	)
))

;;; amalgamate-overlapping-spans: take a sorted span list and amalgamate adjacent spans
;;;		eg. ((1 . 1) (2 . 2) (4 . 4)) => ((1 . 2) (4. 4))
(define amalgamate-overlapping-spans (lambda (spans)
	(let*
		 ((last-span/final-spans (foldl-ma (lambda (span working-span final-spans)
			(cond
				((not working-span) (list span final-spans))
				((>= (+ 1 (span:upper working-span)) (span:lower span)) ; extend the current span
					(list (cons (span:lower working-span) (span:upper span)) final-spans)
				)
				(else ; new working span
					(list span (cons working-span final-spans))
				)
			)
		 ) spans #f '()))
		)
		(reverse (if (car last-span/final-spans)
			(apply cons last-span/final-spans)
			(cadr last-span/final-spans)
		))
	)
))

;;; merge-normalised-span-lists: merge two span lists into one by a simple merge
(define merge-normalised-span-lists (lambda (spans1 spans2)
	(merge! span<=? (copy-list spans1) (copy-list spans2))
))

;;; normalise-span-list: sort a span list by lower element then amalgamate overlapping spans
(define normalise-span-list (lambda (spans)
	(let
		((sorted-spans (merge-sort!	span<=? (copy-list spans))))
		(amalgamate-overlapping-spans sorted-spans)
	)
))

;;; flatten-span-list-list: turn a span list list into a span list containing the same values
(define flatten-span-list-list (lambda (spanss)
	(let
		((merged-spans (fold merge-normalised-span-lists '() spanss)))
		(amalgamate-overlapping-spans merged-spans)
	)
))

;;; span-list-complement: create a list of complementary spans so than `spans' union (span-list-complement spans)
;;;		= [0,max].
(define span-list-complement (lambda (spans max)
	(letrec
		((complement (lambda (start spans not-spans)
			(cond
				((null? spans) (if (<= start max)
					(cons (cons start max) not-spans) ; make a span [start,max]
					not-spans ; no end span
				))
				((> (span:lower (car spans)) start) ; make a span from [start,next.lower-1]
					(complement (+ 1 (span:upper (car spans))) (cdr spans)
						(cons (cons start (- (span:lower (car spans)) 1)) not-spans))
				)
				(else (complement (+ 1 (span:upper (car spans))) (cdr spans) not-spans)) ; no new span
			)
		)))
		(reverse (complement 0 spans '()))
	)
))
