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
;;;	`misc.scm'
;;;	Miscellaneous I/O, typing and list manipulator procedures
;;;

;;;;; Miscellaneous procedures
(if (or (> (string->number (major-version)) 1) (>= (string->number (minor-version)) 8))
	(use-modules (ice-9 rdelim)))

(use-modules (srfi srfi-1))

;;; id: identity procedure
(define id (lambda (arg) arg))

;;; positive-integer?: Is the argument an integer greater than 0 
(define positive-integer? (lambda (x) (and (integer? x) (positive? x))))

;;; inc,dec: add/subtract one to/from x
(define inc (lambda (x) (+ x 1)))
(define dec (lambda (x) (- x 1)))

;;; /=: doesn't equal!
(define /= (lambda l (not (apply = l))))

;;; ->boolean: #f => #f, <anything-else> => #t
(define ->boolean (lambda (x) (if x #t #f)))

;;; logbit? - work around for guile logbit function to allow checking
;;; of bit no.s above 32
(define logbit? (lambda (bit val)
	(not (zero? (logand val (expt 2 bit))))
))

;;; logtest - work around for guile logtest function to allow checking
;;; of large words
(define logtest (lambda (j k)
	(not (zero? (logand j k)))
))

; Headed lists are used as the basis for most typing in this
; project. A headed list is just a list with a symbol as its
; first element which is used to symbolise its type eg:
; 
; (port "a" passive input (numeric-type #f 8))
;
; is a headed list representing a balsa port structure with elements
; "a" (port name), passive (sense), input (direction), (numeric-type #f 8)
; which is another headed list representing a numeric type `8 bits'

;;;;; List handling procedures

;;; headed-list?: Returns true if the given object l is a list with the symbol head-symbol
;;;    as its first element.  If no `head-symbol' is given then returns true if
;;;	   `l' passes pair? and symbol? on its car element
(define headed-list? (lambda args
	(let
		((l (car args))
		 (head-symbol (if (not (null? (cdr args))) (cadr args) #f))
		)
		(if head-symbol
			(and (pair? l) (eq? head-symbol (car l)))
			(and (pair? l) (symbol? (car l)))
		)
	)
))

;;; drop-head-symbol: drop a head symbol from a list if it is equal to head-symbol, if
;;;		the given head-symbol is #f then drop any symbol that happens to be there
(define drop-head-symbol (lambda (head-symbol l)
	(if (and (not (null? l)) (symbol? (car l)))
		(if (or (not head-symbol) (eq? (car l) head-symbol))
			(cdr l)
			l
		)
		l
	)
))

;;; headed-uniform-list?: Return true if the given list has the
;;;    form (head-symbol element_1 element_2 ...) where head-symbol
;;;    is the same symbol as that given in the argument head-symbol
;;;    and each of element_n satisfies the predicate element-predicate.
(define headed-uniform-list? (lambda (l head-symbol element-predicate)
	(and
		(headed-list? l head-symbol)
		(warning-and "uniform-list"
			`(,(apply eager-and (map element-predicate (cdr l))) ,head-symbol " list has non compliant elements")
		)
	)
)) 

;;; flatten-unheaded-list: flatten the given list by appending sublists which are not headed
;;;    eg. (flatten-unheaded-list '((1 2) (4 5) (symbol 4 5))) => '(1 2 4 5 (symbol 4 5))
(define flatten-unheaded-list (lambda (l)
	(let loop
		((l l)
		 (ret '())
		)
		(if (null? l)
			(reverse! ret)
			(loop
				(cdr l)
				(if (and (pair? (car l)) (not (symbol? (caar l))))
					(append! (reverse (car l)) ret)
					(cons (car l) ret)
				)
			)
		)
	)
))

;;; flatten-list: flatten through list structure
(define flatten-list (lambda (l)
	(let loop
		((l l)
		 (ret '())
		)
		(if (null? l)
			(reverse! ret)
			(loop
				(cdr l)
				(if (pair? (car l))
					(append! (reverse (car l)) ret)
					(cons (car l) ret)
				)
			)
		)
	)
))

;;; {list,vector,}-nth: make a procedure to extract the nth element from a list
(define nth (lambda (n) (lambda (l) (list-ref l n))))
(define list-nth nth)
(define vector-nth (lambda (n) (lambda (l) (vector-ref l n))))

;;; member-with-predicate: Like member, but using a user provided predicate
;;;		The predicate is passed the arguments (elem . args) where elem is the
;;;		list element in question; this is useful for doing equality member's
;;;		w/o creating a lambda eg: (member-with-predicate '("a" "b" "c") string=? "a")
(define member-with-predicate (lambda (elems predicate . args)
	(if (null? elems)
		#f
		(if (apply predicate (car elems) args)
			elems
			(apply member-with-predicate (cdr elems) predicate args)
		)
	)
))

;;; member-index-with-predicate: Like member, but using a user provided predicate and
;;;		returns a pair (index element) when the element is found indicating the index in elems
;;;		at which the predicate was satisfied.  index is the index of the first list element
(define member-index-with-predicate (lambda (elems predicate index . args)
	(if (null? elems)
		#f
		(if (apply predicate (car elems) args)
			(list index elems)
			(apply member-index-with-predicate (cdr elems) predicate (+ 1 index) args)
		)
	)
))

;;; find-with-predicate: Returns the car of member-with-predicate ... or #f if no match is found
(define find-with-predicate (lambda (elems predicate . args)
	(safe-car (apply member-with-predicate elems predicate args))
))

;;; find-index-with-predicate: Returns the car of member-index-with-predicate ... or #f if no match is found
(define find-index-with-predicate (lambda (elems predicate index . args)
	(safe-car (apply member-index-with-predicate elems predicate index args))
))

;;; find-index-in-sorted-vector: find the index of the first matching element (by (and (<= key elem) (<= elem key)))
;;;		in the given vector.  The vector should be ordered under <=.  This procedure will return the
;;;		vector index where an element matches and #f othwerwise.
(define find-index-in-sorted-vector (lambda (<= vec key)
	(letrec
		((find-with-len (lambda (index len)
			(case len
				((0) #f)
				((1) (if (and (<= key (vector-ref vec index)) (<= (vector-ref vec index) key))
					index #f))
				(else
					(let
						((half-len (quotient len 2)))
						(if (<= (vector-ref vec (+ index half-len)) key)
							(find-with-len (+ index half-len) (- len half-len)) ; upper half
							(find-with-len index half-len) ; lower half
						)
					)
				)
			)
		)))
		(find-with-len 0 (vector-length vec))
	)
))

;;; find-in-sorted-vector: find the first element in a vector sorted least..most using the <= comparison
;;;		which matches `key'
(define find-in-sorted-vector (lambda (<= vec key)
	(let
		((index (find-index-in-sorted-vector <= vec key)))
		(if index (vector-ref vec index) #f)
	)
))

;;; type-ordering: return a small int usable for ordering elements based on their type
(define type-ordering (lambda (thing)
	(cond
		((null? thing) 0)
		((boolean? thing) 1)
		((string? thing) 2)
		((number? thing) 3)
		((symbol? thing) 4)
		((pair? thing) 5)
	)
))

;;; object<=?: ordering function on lists with only
;;;		string, integer and symbol elements and for which elements pairwise type
;;;		match between the lists.
(define object<=? (lambda (hc1 hc2)
	(let
		((val1 (type-ordering hc1))
		 (val2 (type-ordering hc2))
		)
		(if (= val1 val2)
			(cond
				((null? hc1) #t)
				((boolean? hc1) (or (eq? hc1 hc2) (not hc1)))
				((string? hc1) (string<=? hc1 hc2))
				((number? hc1) (<= hc1 hc2))
				((symbol? hc1) (string<=? (symbol->string hc1) (symbol->string hc2)))
				((pair? hc1)
					(if (equal? (car hc1) (car hc2))
						(object<=? (cdr hc1) (cdr hc2))
						(object<=? (car hc1) (car hc2))
					)
				)
			)
			(<= val1 val2)
		)
	)
))

;;; find-headed-list-elem: Find a headed-list element of the required type and name in the given list
;;;		if name is #f then don't bother to match the name (no name need exist)
(define find-headed-list-elem (lambda (headed-lists head-symbol name)
	(find-with-predicate headed-lists
		; list with (head-symbol name ...)
		(lambda (x) (and (headed-list? x head-symbol) (or (not name) (equal? (cadr x) name))))
	)
))

;;; find-headed-sub-list: Find a sub list of the given list `l' which has the
;;;		head symbol `head-symbol'
;;;		returns the sublist if found or #f in any other case.
;;;		eg. (find-headed-sub-list '(a (b #f) (c #t)) 'b) => '(b #f)
(define find-headed-sub-list (lambda (l head-symbol)
	(if (not (pair? l))
		#f
		(if (headed-list? (car l) head-symbol)
			(car l)
			(find-headed-sub-list (cdr l) head-symbol)
		)
	)
))

;;; eager-and: Eager and procedure
(define eager-and (lambda args
	(if (null? args)
		#t
		(if (car args)
			(apply eager-and (cdr args))
			#f
		)
	)
))

;;; eager-or: Eager or procedure
(define eager-or (lambda args
	(if (null? args)
		#f
		(if (car args)
			#t
			(apply eager-or (cdr args))
		)
	)
))

;;; warning-and: gives a warning of the form `prefix: message' for those arguments
;;;    which are pairs of (boolean . message) and in which boolean is false
(define warning-and (lambda (prefix . conditions)
	(let
		((emit-warning (lambda (condition)
			(if (pair? condition)
				(if (not (car condition))
					(begin
						(display prefix)
						(display ": ")
						(for-each display (cdr condition))
						(newline)
						#f
					)
					#t
				)
				condition
			)
		)))
		(apply eager-and (map emit-warning conditions))
	)
))

;;; print: write the arguments to the current-output-port
;;;   in the order they are given
(define print (lambda l
	(for-each display l)
))

;;; print-to-port: like print but to the given port
(define print-to-port (lambda (port . l)
	(for-each (lambda (elem) (display elem port)) l)
))

;;; simple-print: print out the given data structure with a little
;;;   sensible formatting, l is the structure to print (typically a list)
;;;   tabs is a string to prepend to printed lines, listable-head-symbols is
;;;   a list of all symbols which should be recognised as type distinguishing
;;;   symbols when found at the head of lists.  Passing #t in lieu of a list
;;;   of symbols as listable-head-symbols results in all headed lists being
;;;   formatted 
(define simple-print (lambda (l tabs listable-head-symbols)
	(if (and
			(pair? l)
			(or
				(eqv? listable-head-symbols #t)
				(and
					(symbol? (car l))
					(cdr l)
					(memq (car l) listable-head-symbols)
				)
			)
		)
		(begin
			(display "(") ;)
			(write (car l))
			(for-each (lambda (x)
				(begin
					(newline)
					(display "    ")
					(display tabs)
					(simple-print x (string-append "    " tabs) listable-head-symbols)
				)
			) (cdr l))
			(newline)
			(display tabs) ;(
			(display ")")
		)
		(if (list? l)
			(begin
				(display "(") ;)
				(if (not (null? l))
					(begin
						(write (car l))
						(for-each (lambda (x)
							(display " ")
							(simple-print x tabs listable-head-symbols))
						(cdr l))
					)
				)
				(display ")")
			)
			(write l)
		)
	)
))

;;; safe-car: if argument is a pair then return (car arg) otherwise return the argument
(define safe-car (lambda (arg)
	(if (pair? arg) (car arg) arg)
))

;;; safe-cdr: if argument is a pair then return (cdr arg) otherwise return the argument
(define safe-cdr (lambda (arg)
	(if (pair? arg) (cdr arg) arg)
))

;;; foldl: accumlate to the left, work on head items of given lists
(define foldl (lambda (f acc . ls)
	(if (null? (car ls)) ; ls must be of the same length
		acc
		(apply foldl f (apply f acc (map car ls)) (map cdr ls))
	)
))

;;; foldl-ma: like foldl but pass multiple accumulators.  Function `f' is applied
;;;		with list item at end.  `f' must return a list of the same length as the
;;;		number of accumulators.  foldl-ma returns the list of final accumulators.
;;;		eg. (foldl-ma (lambda (val add sub) (list (+ add val) (- sub val))) '(1 2 3) 0 0)
;;;		=> '(6 -6)
(define foldl-ma (lambda (f l . accs)
	(if (null? l)
		accs
		(apply foldl-ma f (cdr l) (apply f (car l) accs))
	)
))

;;; hier-map: apply map to elemenets of a tree at depth depth. eg.
;;;		(hier-map 2 <+1> '((1) (2 3 4) (5 6)) ==> ((2) (3 4 5) (6 7))
(define hier-map (lambda (depth f l)
	(if (= depth 1)
		(map f l)
		(map (lambda (next-level) (hier-map (- depth 1) f next-level)) l)
	)
))

;;; foldl-with-progress-bar : do fancy progress bar processing of the
;;;		given list `l' with procedure `p'.  If `name-proc' is set to #f
;;;		then a progress bar is shown, otherwise `name-proc' is invoked to
;;;		print an informative string corresponding to the element of `l'
;;;		currently being processed.  `p' is passed (acc element-of-l) and
;;;		must return a modified acc.  Uses procedure `print' to print with.
(define foldl-with-progress-bar (lambda (p name-proc print acc l)
	(let*
		((count (length l))
		 (elements-per-ding (if (< count 50) 1 (quotient count 50)))
		)
		(if (not name-proc)
			(begin
				(print "0% |" (make-string (quotient count elements-per-ding) #\-) "| 100%" #\newline)
				(print "    ")
			)
		)
		(car (foldl-ma (lambda (elem acc ding-counter)
			(if name-proc
				(print (name-proc elem) #\newline)
				(if (>= 1 ding-counter) (print "="))
			)
			(list
				(p acc elem)
				(if (>= 1 ding-counter) elements-per-ding (- ding-counter 1))
			)
		) l '() elements-per-ding))
	)
))

;;; union: set union. Merge two lists (which are sorted least to most element by the function <=) into
;;;		a single list removing elements which are equal under the function =
(define union (lambda (<= set1 set2)
	(letrec 
		((union-tail (lambda (set1 set2 ret)
			(cond
				((null? set1) (append! (reverse! ret) set2))
				((null? set2) (append! (reverse! ret) set1))
				(else
					(if (<= (car set1) (car set2))
						(if (<= (car set2) (car set1))
							(union-tail (cdr set1) (cdr set2) (cons (car set1) ret)) ; set1 = set2
							(union-tail (cdr set1) set2 (cons (car set1) ret)) ; set1 < set2
						)
						(union-tail set1 (cdr set2) (cons (car set2) ret)) ; set1 > set2
					)
				)
			)
		)))
		(union-tail set1 set2 '())
	)
))

;;; differ: set difference. Remove elements of set s2 from set s1 using <= as a comparison predicate
(define differ (lambda (<= set1 set2)
	(letrec 
		((differ-tail (lambda (set1 set2 ret)
			(cond
				((null? set1) (reverse! ret)) ; remove no more elements
				((null? set2) (append! (reverse! ret) set1)) ; no elements left to prune
				(else
					(if (<= (car set1) (car set2))
						(if (<= (car set2) (car set1))
							(differ-tail (cdr set1) (cdr set2) ret) ; set1 = set2
							(differ-tail (cdr set1) set2 (cons (car set1) ret)) ; set1 < set2
						)
						(differ-tail set1 (cdr set2) ret) ; set1 > set2
					)
				)
			)
		)))
		(differ-tail set1 set2 '())
	)
))

;;; union-numbers: union on sets of numbers
(define union-numbers (lambda (set1 set2)
	(union <= set1 set2)
))

;;; differ-numbers: difference on sets of numbers
(define differ-numbers (lambda (set1 set2)
	(differ <= set1 set2)
))

;;; uniq: remove adjacent elements from a list which match by predicate =
(define uniq (lambda (= l)
	(letrec
		((uniq-tail (lambda (l first ret)
			(if (null? l)
				(reverse! (cons first ret))
				(if (= (car l) first)
					(uniq-tail (cdr l) first ret)
					(uniq-tail (cdr l) (car l) (cons first ret))
				)
			)
		)))
		(if (null? l)
			'()
			(uniq-tail (cdr l) (car l) '())
		)
	)
))

;;; unsorted-uniq: remove repeated elements from the given list.  Only the first
;;;		occurence will be kept. eg. '(1 2 3 5 6 1 4) => '(1 2 3 4 5 6 4). O(n^2)
(define unsorted-uniq (lambda (= l)
	(reverse! (fold (lambda (elem ret)
		(if (find-with-predicate ret (lambda (v) (= v elem)))
			ret
			(cons elem ret)
		)
	) '() l))
))

;;; dup: make a list of `n' occurences of `elem'
(define dup (lambda (elem n)
	(letrec
		((dup-tail (lambda (n ret)
			(if (zero? n)
				ret
				(dup-tail (- n 1) (cons elem ret))
			)
		 ))
		)
		(dup-tail n '())
	)
))

;;; merge!: merge two lists (sorted least to most element by the function <=) into
;;;		a single sorted list
(define merge! (lambda (<= l1 l2)
	(letrec
		((merge-tail! (lambda (l1 l2 tail)
			(cond
				((null? l1) (set-cdr! tail l2))
				((null? l2) (set-cdr! tail l1))
				((<= (car l1) (car l2)) (merge-tail! (cdr l1) l2 (begin (set-cdr! tail l1) (cdr tail))))
				(else (merge-tail! l1 (cdr l2) (begin (set-cdr! tail l2) (cdr tail))))
			)
		)))
		(let
			((head (cons #f '()))) ; sacrificial head cell
			(merge-tail! l1 l2 head)
			(cdr head)
		)
	)
))

;;; merge-sort!: sort a list (in accending order by the function <=) by a divide and merge
;;;		method
(define merge-sort! (lambda (<= l)
	(if (or (null? l) (null? (cdr l)))
		l
		(let*
			((half-length (quotient (length l) 2))
			 (first-half-last-cell (list-tail l (- half-length 1)))
			 (second-half (cdr first-half-last-cell))
			)
			(set-cdr! first-half-last-cell '())
			(merge! <= (merge-sort! <= l) (merge-sort! <= second-half))
		)
	)
))

;;; merge-sort: referentially transparent merge sort
(define merge-sort (lambda (<= l)
	(merge-sort! <= (copy-list l))
))

;;; rev-cons: cons but with car and cdr reversed, useful in folds
(define rev-cons (lambda (cdr car)
	(cons car cdr)
))

;;; copy-list: copy a list to a new list
(define copy-list (lambda (l)
	(append l '())
))

;;; zip: combine lists into a single list of elements in the same position in each
;;;		list eg. a,b -> (a1 b1) (a2 b2) ...
;(define zip (lambda ls
;	(apply map list ls)
;))

;;; ..: generate a list of integers
(define .. (lambda (low high)
	(letrec
		((..-tail (lambda (i is)
			(if (< i low)
				is
				(..-tail (- i 1) (cons i is))
			)
		)))
		(..-tail high '())
	)
))

;;; integer-range-list: like .. but expands to a list from `first' to `last' in either
;;;		assending or descending order
(define integer-range-list (lambda (first last)
	(if (> first last)
		(reverse! (.. last first))
		(.. first last)
	)
))

;;; for..: execute the procedure on each value from low to high calling procedure
;;;		with (apply procedure elem args)
;;; now traverses any list in args as well. equivalent to 
;;; (apply for-each procedure (.. low high) args)
(define for.. (lambda (procedure low high . args)
	(if (<= low high)
		(begin
			(apply procedure low (map car args))
			(apply for.. procedure (+ 1 low) high (map cdr args))
		)
	)
))

;;; map..: like for.. but make a list of return values
(define map.. (lambda (procedure low high . args)
	(letrec
		((tail (lambda (val ret args)
			(if (<= val high)
				(tail
					(+ 1 val)
					(cons (apply procedure val (map car args)) ret)
					(if (null? args) '() (map cdr args))
				)
				ret
			)
		)))
		(reverse! (tail low '() args))
	)
))

;;; map-integer-range-list: like map.. but allowing ascending
;;;		and descending integer ranges
(define map-integer-range-list (lambda (procedure first last . args)
	(let
		((end-test (if (> first last)
			(lambda (v) (< v last))
			(lambda (v) (> v last))
		 ))
		 (iterate (if (> first last)
			(lambda (i) (- i 1))
			(lambda (i) (+ i 1))
		 ))
		)
		(letrec
			((tail (lambda (val ret)
				(if (end-test val)
					ret
					(tail
						(iterate val)
						(cons (apply procedure val args) ret)
					)
				)
			)))
			(reverse! (tail first '()))
		)
	)
))

;;;;; Iteration operators (I can't get the hang of do ;)

;;; while: apply body to args while (apply pred args) remains true.  The return value
;;;		of body is used as args for the next call to body. eg.
;;;		(while (lambda (count) (< count 10)) (lambda (count) (print count #\,) `(,(+ 1 count))) 0)) =>
;;;		0,1,2,3,4,5,6,7,8,9, (return value '(10))
(define while (lambda (pred body . args)
	(if (apply pred args)
		(apply while pred body (apply body args))
		args
	)
))

;;; until: like while but execute until pred becomes true
(define until (lambda (pred body . args)
	(apply while (lambda args (not (apply pred args))) body args)
))

;;;;; String handling

(define flip-string-append (lambda (r l) (string-append l r)))

;;; strdchr: find a character in the given string, starting at the given `start-index'
;;;		and going in the specified direction (#t for forwards, #f for backwards)
(define strdchr (lambda (str char start-index direction)
	(let*
		((next (if direction inc dec))
		 (string-len (delay (string-length str)))
		 (first-index (if (negative? start-index) (+ (force string-len) start-index) start-index))
		 (terminate? (if direction ; terminate predicate
			(lambda (index) (>= index (force string-len)))
			(lambda (index) (< index 0))
		 ))
		)
		(letrec
			((find-char (lambda (index)
				(if (terminate? index)
					#f
					(if (eqv? (string-ref str index) char)
						index
						(find-char (next index))
					)
				)
			)))
			(find-char first-index)
		)
	)
))

;;; strchr: find a character's first position in a string (or #f if not found), if a
;;;		third argument is given this is taken to be a starting index
;;;		eg. (strchr "Hollo" #\c 3) => 4.  Negative starting indices count from the end
;;;		of the string (-1 => last char (- (string-len str) 1))
(define strchr (lambda (str char . args)
	(strdchr str char (if (null? args) 0 (car args)) #t)
))

;;; strrchr: like strchr but search the string backwards
(define strrchr (lambda (str char . args)
	(strdchr str char (if (null? args) -1 (car args)) #f)
))

;;; replace-char: replace each occurence of character `from' with character `to' in given string,
;;;		generating a new string
(define replace-char (lambda (str from to)
	(let
		((new-str (string-copy str)))
		(for.. (lambda (index)
			(if (eq? from (string-ref str index))
				(string-set! new-str index to)
			)
		) 0 (- (string-length str) 1))
		new-str
	)
))

;;; list-replace-headed: replace elements of the given list of the form (tag ...) by applying f to the ...
;;;		and substituting the result into the orig. list
(define list-replace-headed (lambda (l tag f)
	(cond
		((pair? l)
			(if (eqv? (car l) tag)
				(apply f (cdr l))
				(cons (list-replace-headed (car l) tag f) (list-replace-headed (cdr l) tag f))
			)
		)
		(else l)
	)
))

;;; chars-in-string?: return #t if any of the characters in the list `chars' (which could also
;;;		be a string) occurs in the string `str' 
(define chars-in-string? (lambda (str chars)
	(let ((chars-list (if (string? chars) (string->list chars) chars)))
		(cond
			((null? chars) #f)
			((strchr str (car chars-list)) #t)
			(else (chars-in-string? str (cdr chars-list)))
		)
	)
))

;;; parse-string: reduce a string `str' into a list of elements which were delimited by `delim'
;;;		in str.  delim can be a character, a list of characters (which are equivalent) or a string
;;;		representing a multicharacter delimter.  eg. (parse-string "1,2,3,;6;7;;" '(#\; #\,)) returns
;;;		("1" "2" "3" "" "6" "7" "" "").  Delimiters separate elements and so delimiters at the start
;;;		and end of the string denote empty fields.
(define parse-string (lambda (str delim)
	(let*
		((delim-len (if (string? delim) (string-length delim) 1)) ; length of delimiter str.
		 (delim-test ; test for delimiter, a character, a string or a list of characters
			(cond
				((char? delim) (lambda (from) (eq? delim (string-ref str from))))
				((list? delim) (lambda (from) (memq (string-ref str from) delim)))
				((string? delim) (lambda (from) (string=? (substring str from (+ from delim-len)) delim)))
			)
		 )
		 (str-len (string-length str))
		 (undelimited-len (+ 1 (- str-len delim-len)))
		)
		(letrec
			((parse-string-tail (lambda (from to fragments)
				(if (>= to undelimited-len)
					(cons (substring str from str-len) fragments) ; Last fragment
					; Is a delimiter? (or the end of the string)
					(let
						((to-after-delim (+ delim-len to)))
						(if (delim-test to)
							(parse-string-tail to-after-delim to-after-delim
								(cons (substring str from to) fragments))
							(parse-string-tail from (+ 1 to) fragments)
						)	
					)
				)
			)))
			(reverse! (parse-string-tail 0 0 '()))
		)
	)
))

;;; escape-string: escape "s in a string to \"
(define escape-string (lambda (from)
	(let*
		((len (string-length from))
		 (to (make-string (* len 2)))
		 (to-length (fold (lambda (from-index to-index)
		 	(let ((chr (string-ref from from-index)))
		 		(case chr
					((#\")
						(string-set! to to-index #\\)
						(string-set! to (+ 1 to-index) #\")
						(+ 2 to-index)
					)
					(else
						(string-set! to to-index chr)
						(+ 1 to-index)
					)
				)
			)
	 	 ) 0 (.. 0 (- len 1))))
	 	)
	 	(substring to 0 to-length)
	)
))

;;; ->string: string representation of the given obj. Strings are printed with double quotes,
;;;		numbers with number->string, booleans with #f and #t, lists as `(a b c)' type thingies.
;;;		vectors as `#(a b c)'.
(define ->string (lambda (obj)
	(cond
		((eq? #f obj) "#f")
		((eq? #t obj) "#t")
		((string? obj) (string-append "\"" (escape-string obj) "\""))
		((number? obj) (number->string obj))
		((symbol? obj) (symbol->string obj))
		((null? obj) "()")
		((list? obj) (string-append "(" (build-separated-string (map ->string obj) " ") ")"))
		((pair? obj) (string-append "(" (->string (car obj)) " . " (->string (cdr obj)) ")"))
		((vector? obj) (string-append "#" (vector->list obj)))
		((char? obj) (string obj))
		(else (error "Can't stringigy: " obj))
	)
))

;;; ->string-rep: like ->string but don't quote strings
(define ->string-rep (lambda (obj)
	(if (string? obj) obj (->string obj))
))

;;; build-string: take a list of strings and append them using `pre' and `post' as separators
;;;		around each element
(define build-string (lambda (l pre post)
	(fold (lambda (elem acc) (string-append acc pre elem post)) "" l)
))

;;; build-separated-string: like build string but use a separator in lieu of pre/post strings
(define build-separated-string (lambda (l separator)
	(if (null? l)
		""
		(string-append (car l) (build-string (cdr l) separator ""))
	)
))

;;; split-line: split a string into a list of strings of maximum length `max-line-length'
;;;		(plus the line continuation string `continuation-string'). `endNstart' specifies whether
;;;		the continuation string should be at the end of the continued line or the start of the next
(define split-line (lambda (line max-line-length continuation-string endNstart)
	(let*
		((line-length (string-length line)))
		(letrec
			((split-line-tail (lambda (start-index ret pending-prefix-string)
				(if (>= (+ start-index max-line-length) line-length) ; last line
					(if (zero? start-index)
						(list line)
						(reverse (cons (string-append pending-prefix-string
							(substring line start-index line-length)) ret))
					)
					(let
						((new-start-index (+ start-index max-line-length))
						 (line-segment (substring line start-index (+ start-index max-line-length)))
						)
						(if endNstart
							(split-line-tail new-start-index
								(cons (string-append pending-prefix-string line-segment continuation-string) ret)
								"" )
							(split-line-tail new-start-index
								(cons (string-append pending-prefix-string line-segment) ret)
								continuation-string)
						)
					)
				)
			)))
			(split-line-tail 0 '() "")
		)
	)
))

;;; overwrite-end-of-string: returns a string conposed of the start of `str1' and
;;;		the whole of `str2' having the same length as `str1'. eg. "HelloWorld" "ooly" => "HelloWooly"
(define overwrite-end-of-string (lambda (str1 str2)
	(string-append (substring str1 0 (- (string-length str1) (string-length str2))) str2)
))

;;; ...radix-characters: characters used to represent numbers 0...35
(define uppercase-radix-characters '#(#\0 #\1 #\2 #\3 #\4 #\5 #\6 #\7 #\8 #\9
#\A #\B #\C #\D #\E #\F #\G #\H #\I #\J #\K #\L #\M #\N #\O #\P #\Q #\R #\S #\T #\U #\V #\W #\X #\Y #\Z))
(define lowercase-radix-characters '#(#\0 #\1 #\2 #\3 #\4 #\5 #\6 #\7 #\8 #\9
#\a #\b #\c #\d #\e #\f #\g #\h #\i #\j #\k #\l #\m #\n #\o #\p #\q #\r #\s #\t #\u #\v #\w #\x #\y #\z))

;;; radix-string: returns a string representing the given natural value in the given radix.  If
;;;		`digit-count' is not #f then zero pads the string to that number of digit-count in length.
;;;		NB. `radix' can be between 2 and 36.  If a fourth argument is given this is used as the
;;;		character vector from which digits should be drawn.
(define radix-string (lambda (radix value digit-count . alphabet)	
	(let
		((alphabet (if (null? alphabet) uppercase-radix-characters (car alphabet))))
		(if (and (zero? value) (or (not digit-count) (< 0 digit-count)))
			(make-string (if digit-count digit-count 1) (vector-ref alphabet 0))
			(list->string (caddr (until
				(lambda (value digit-count digits) (if digit-count (zero? digit-count) (zero? value)))
				(lambda (value digit-count digits)
					(list
						(quotient value radix)
						(if digit-count (- digit-count 1) #f)
						(cons (vector-ref alphabet (modulo value radix)) digits)
					)
				)
				value digit-count '()
			)))
		)
	)
))

;;; binary-string: returns a string representing the given natural value in binary in a width bit
;;;   wide field.  The returned string is zero padded and bitwise little endian "1 ... 2^n-1 2^n"
(define binary-string (lambda (value width)
	(string-reverse! (radix-string 2 value width))
))

;;; initial-string: if `str2' matches the front end of string `str1' then return the rest of the
;;;		string `str1' w/o that front section.  Otherwise returns #f.
(define initial-string (lambda (str1 str2)
	(let
		((len1 (string-length str1))
		 (len2 (string-length str2))
		)
		(if (> len2 len1)
			#f
			(if (string=? str2 (substring str1 0 len2))
				(substring str1 len2 len1)
				#f
			)		
		)
	)
))

;;; final-string: if `str2' matches the back end of string `str1' then return the front portion
;;;		of string `str1' w/o that back section.  Otherwise returns #f.
(define final-string (lambda (str1 str2)
	(let
		((len1 (string-length str1))
		 (len2 (string-length str2))
		)
		(if (> len2 len1)
			#f
			(if (string=? str2 (substring str1 (- len1 len2) len1))
				(substring str1 0 (- len1 len2))
				#f
			)		
		)
	)
))

;;;;; Bit twiddling

;;; bit-length: returns the length (in bits) of the given number, 0 for 0
(define bit-length (lambda (v)
	(inexact->exact (ceiling (/ (log (+ 1 v)) (log 2))))
))

;;; integer->binary-list: Return a list of booleans (of length width) which represent
;;;		the given value `value' in binary.  The returned list is bitwise little endian
;;;		ie. the value at (list-ref n ...) represents the 2^n bit place
(define integer->binary-list (lambda (value width)
	(letrec
		((i->bl-tail (lambda (value width result)
			(if (zero? width)
				result
				(i->bl-tail (quotient value 2) (- width 1)
					(cons (= 1 (modulo value 2)) result))
			)
		)))
		(reverse! (i->bl-tail value width '()))
	)
))

;;; integer->word-list: produce a list of integers representing the given integer when
;;;		split into `word-size' bit long words.  The returned list is word-wise little endian.
;;;		If `word-count' is given then the returned list will have exactly `word-count' words
;;;		(padded in the most significant words with 0).
(define integer->word-list (lambda (value word-size . word-count)
	(let
		((word-count (if (null? word-count) #f (car word-count)))
		 (two-pow-word-size (expt 2 word-size))
		)
		(letrec
			((integer->word-list-tail (lambda (value words word-count)
				(if (zero? (if word-count word-count value))
					words
					(integer->word-list-tail (quotient value two-pow-word-size)
						(cons (remainder value two-pow-word-size) words)
						(if word-count (- word-count 1) #f)
					)
				)
			)))
			(reverse! (integer->word-list-tail value '() word-count))
		)
	)
))

;;; shift-right: shift the gievn number `distance' places right
(define shift-right (lambda (v distance)
	(if (zero? distance)
		v
		(shift-right (quotient v 2) (- distance 1))
	)
))

;;; find-set-bit: find the index of the first set bit after (or including) the given index
;;;		returns a pair of (right-shifted-val-residue . first-set-bit-index)
;;;		first-set-bit-index is #f if no set bit in present.
(define find-set-bit (lambda (v index)
	(let
		((shifted-v (shift-right v index)))
		(letrec
			((self (lambda (v index)
				(let ((bottom-bit (modulo v 2)))
					(cond
						((zero? v) (cons v #f))
						((zero? bottom-bit) (self (quotient v 2) (+ 1 index)))
						(else (cons (quotient v 2) index))
					)
				)
			)))
			(self shifted-v index)
		)
	)
))

;;; find-clear-bit: find the index of the first clear (0) bit after (or including) the given index
;;;		returns a pair of (right-shifted-val-residue . first-clear-bit-index)
(define find-clear-bit (lambda (v index)
	(let
		((shifted-v (shift-right v index)))
		(letrec
			((self (lambda (v index)
				(let ((bottom-bit (modulo v 2)))
					(if (zero? bottom-bit)
						(cons (quotient v 2) index)
						(self (quotient v 2) (+ 1 index))
					)
				)
			)))
			(self shifted-v index)
		)
	)
))

;;; logbit?: a version of this operator that actually works
(define logbit? (lambda (bit val)
	(not (zero? (logand val (expt 2 bit))))
))

;;; bit-{set,clear}: return a modified version of `v' with the bit
;;;		at index `bit-no' set or cleared.
(define bit-set (lambda (v bit-no)
	(logior v (expt 2 bit-no))
))
(define bit-clear (lambda (v bit-no)
	(if (logbit? bit-no v)
		(- v (expt 2 bit-no))
		v
	)
))

;;; extract-mask-indices: find a group of contiguous sequence bits in the value `v'.  Returns a pair
;;;		(LS-set-bit-index . no-of-set-bits) representing that sequence of bits.  eg.
;;;		(extract-mask-indices #b0001111100) returns (2 . 5).  Returns #f if there is more than one such
;;;		sequence of set bits in v.
(define extract-mask-indices (lambda (v)
	(let*
		((one-residue/index (find-set-bit v 0))
		 (zero-residue/index (find-clear-bit v (+ 1 (cdr one-residue/index))))
		)
		(if (zero? (car zero-residue/index))
			(cons (cdr one-residue/index) (- (cdr zero-residue/index) (cdr one-residue/index)))
			#f
		)
	)
))

;;; Error reporting and top level exiting

;;; *top-level-exit*: top level continuation
(define *top-level-exit* (lambda (n) (exit)))

;;; *top-level-return*: value returned by last top-level
(define *top-level-return* '())

;;; top-level: top level procedure
(define top-level (lambda (f . args)
    (let ((main_function_went_ok 0))
	    (set! *top-level-return*
		    (call-with-current-continuation (lambda (exit)
			    (set! *top-level-exit* exit)
				(apply f args)
				(set! main_function_went_ok 1)
			)))
		(if (= main_function_went_ok 0)
			(exit 1)
		)
	)
))

;;; print-err: print to stderr
(define print-err (lambda l
	(apply print-to-port (current-error-port) l)
))

;;; error: report an error ;)
(define error (lambda l
	(apply print-err l)
	(print-err #\newline)
	(*top-level-exit* 1)
))

;;; debug: print if debugging
;;; (define debugging #f)
;;; (define debug (lambda l
;;;	   (if debugging (apply print-err l))
;;; ))

;;;;; File handling

;;; file-ctime<=?: returns true if file `file-name2' doesn't exist or if
;;;		file `file-name1' is older or exactly the same age as than `file-name2'
(define file-ctime<=? (lambda (file-name1 file-name2)
	(if (not (file-exists? file-name2))
		#t
		(<= (stat:ctime (stat file-name1)) (stat:ctime (stat file-name2)))
	)
))

;;; read-list: read the given port until eof. Returns a list of objects read
(define read-list (lambda (port)
	(let 
		; Sacrificial head cell
		((head-cell (cons #f '())))
		(letrec
			((read-list-tail (lambda (tail)
				(let
					; Read object from port
					((object (read port)))
					(if (eof-object? object)
						; Return the head of the list
						(cdr head-cell)	
						(begin
							(set-cdr! tail (cons object '()))
							; append new element
							(read-list-tail (cdr tail))
						)
					)
				)
			)))
			(read-list-tail head-cell)
		)
	)
))

;;; read-lines: read a list of lines from the given port until EOF is hit
(define read-lines (lambda (port)
	(let loop
		((lines '()))
		(let
			((line (read-line port)))
			(if (eof-object? line)
				(reverse! lines)
				(loop (cons line lines))
			)
		)
	)
))

;;; split-filename: split a given filename into a triple of (path filename extension)
;;;		eg. (split-filename "/usr/local/bin/guile.a") => ("/usr/local/bin/" "guile" ".a")
(define split-filename (lambda (filename)
	(let*
		((split-name (parse-string filename #\/))
		 (split-name-part-count (length split-name))
		 (last-path-part (list-ref split-name (- split-name-part-count 1)))
		 (dot-split-name (parse-string last-path-part #\.))
		 (dot-split-name-part-count (length dot-split-name))
		 (path (if (< split-name-part-count 2) ""
			(build-string (take split-name (- split-name-part-count 1)) "" "/")
		 ))
		 (name (if (< dot-split-name-part-count 2) last-path-part
			(build-separated-string (take dot-split-name (- dot-split-name-part-count 1)) ".")
		 ))
		 (extension (if (< dot-split-name-part-count 2) ""
			(string-append "." (list-ref dot-split-name (- dot-split-name-part-count 1)))
		 ))
		)
		(list path name extension)
	)
))

;;; find-filename: find a file given either a dotted path or an explicit filename (with or
;;;		without an extension).  The filename is assumed to be a dotted-path if
;;;		enclosed in square brackets or if dotted-path? is true.  Returns a triple of
;;;		(prefix-path filename extension) eg. ("/home/bardslea/project/balsa/lib/balsa/types/" "basic" ".sbreeze")
;;;		`prefix-path' will be "" where an explicit filename was given, extension is "" where the filename
;;;		has no extension and has a prefixing "." where there is an extension.
;;;		If 'dotted-path-in-same-dir is passed as part of `options' then raise no error if a dotted pathed
;;;		file cannot be found, assume its in the current directory ie. ("" "name" ".extension")
(define find-filename (lambda (filename extension dotted-path? search-path . options)
	(let*
		((dotted-path-in-same-dir (and (not (null? options)) (memv 'dotted-path-in-same-dir options)))
		 (filename-len (string-length filename))
		 (bracketted-filename ; filename has square brackets around it
			(and (eqv? (string-ref filename 0) #\[) (eqv? (string-ref filename (- filename-len 1)) #\])))
		)
		(if (or dotted-path? bracketted-filename) ; this is a dotted path
			(let*
				; "a.b.c" => ("a" "b" "c") => "/a/b/c.ext"
				((dotted-block-name (parse-string
					(if bracketted-filename (substring filename 1 (- filename-len 1)) filename) "."))
		 		 (slashed-block-name
					(let
						((s-name (fold (lambda (x acc) (string-append acc "/" x)) "" dotted-block-name)))
						(if (and extension (not (string=? "" extension)))
							(string-append s-name "." extension) s-name) 
					)
		 		 )
				 (block-part-count (length dotted-block-name))
		 		 (file-exists (lambda (path) (file-exists? (string-append path slashed-block-name))))
		 		 (found-path (find-with-predicate search-path file-exists))
				)
				(if found-path
					(list
						(string-append found-path (build-string
							(take dotted-block-name (- block-part-count 1)) "/" "") "/")
						(list-ref dotted-block-name (- block-part-count 1))
						(string-append "." extension)
					)
					(if dotted-path-in-same-dir
						(list "" (list-ref dotted-block-name (- block-part-count 1))
							(string-append "." extension))
						(error "find-filename: cannot open block with dotted path `" filename "'")
					)
				)
			)
			(let
				((path/name/ext (split-filename filename))
				 (cant-find-file (lambda ()
					(error "find-filename: cannot open file " filename)
				 ))
				)
				(if (file-exists? filename)
					path/name/ext
					(if (and extension (not (string=? "" extension)))
						(let
							((extended-filename (string-append filename "." extension)))
							(if (file-exists? extended-filename)
								(split-filename extended-filename)
								(cant-find-file)
							)
						)
						(cant-find-file)
					)
				)
			)
		)
	)
))

;;; get-flattened-block: get all the declarations in the file specified by filename (this is a real file name
;;;		not a dotted path).  Follow imports in this file to produce a list of visited files and declarations
;;;		Returns a quadruple of lists (top-level-decls imported-blocks imported-decls visited-blocks).
;;;		`stopping-blocks' is a list of dotted paths which have already been visited and
;;;		should not be visited by this procedure.  This procedure will work for sbreeze and net files
;;;		and any other simple list formatted file with (import "dotted-path") style imports and
;;;		import idempotency.
(define get-flattened-block (lambda (filename extension stopping-blocks search-path)
	(if (not filename)
		(error "get-block: Can't find top-level file")
		(let*
			((top-decls (read-list (open-input-file filename))) ; All top level decls
			 ; just the names of imported blocks (as dotted paths)
			 (imported-blocks (map cadr (filter (lambda (e) (headed-list? e 'import)) top-decls)))
			 ; iterate across the files to import, call get-block recursively
			 (null/visited/imported-top-decls
				(while (lambda (blocks-to-visit visited-blocks top-decls) (not (null? blocks-to-visit)))
					(lambda (blocks-to-visit visited-blocks top-decls)
						(cons (cdr blocks-to-visit)
							(if (member (car blocks-to-visit) visited-blocks) ; been here before
								(list visited-blocks top-decls)
								(let
									((import-filename (fold flip-string-append ""
										(find-filename (car blocks-to-visit) extension #t search-path))))
									(if (not import-filename)
										(error "get-block: Can't find file for ." extension " block: "
											(car blocks-to-visit))
										(let
											; Recurse through blocks-to-visit, accumulate visited-blocks
											((top-decls/import-names/imported/visited
												(get-flattened-block import-filename extension
												visited-blocks search-path)))
											(list 
												(cons (car blocks-to-visit)
													(cadddr top-decls/import-names/imported/visited)
												)
												(append top-decls
													(caddr top-decls/import-names/imported/visited)
													(car top-decls/import-names/imported/visited)
												)
											)
										)
									)
								)
							)
						)
					)
					imported-blocks stopping-blocks '()
				)
			 )
			)
			(list
				top-decls
				imported-blocks
				(caddr null/visited/imported-top-decls)
				(cadr null/visited/imported-top-decls)
			)
		)
	)
))

;;; get-table-file: test the value `global' for #f, if so open the file `filename' and use
;;;		read-list to get its contents (return the list).  If global is not #f, return global.
;;;		This is used to load global tables when wrapped in something like:
;;;		(set! *global-table* (get-table-file *global-table* "filename"))
;;;		The table will be read only once however many times this procedure is called as long as
;;;		the global variable is initialised with #f.
(define get-table-file (lambda (global filename)
	(if (not global) ; Have we already loaded?
		(get-file filename)
		global
	)
))

;;; get-file-with-reader: get a file contents using `reader' as a whole file read procedure.
;;;		This procedure provides a useful wrapper for read-lines and read-list.
(define get-file-with-reader (lambda (filename reader)
	(if (file-exists? filename)
		(let*
			((file (open-input-file filename))
			 (contents (reader file))
			)
			(close-port file)
			contents
		)
		(error "get-file: cannot open file `" filename "'")
	)
))

;;; get-file: get file contents using read-list
(define get-file (lambda (filename)
	(get-file-with-reader filename read-list)
))

;;;;; Time

;;; time: time a thunk in seconds
(define time (lambda (thunk)
	(let
		((start-time (current-time)))
		(thunk)
		(print-err "Took " (- (current-time) start-time) "s" #\newline)
	)
))

;;;;; Shell interface, command line escaping etc.

(define bourne-command-line-sensitive-chars '(#\space #\; #\* #\[ #\]))
;;; bourne-escape-command-line-arg: `Escape' command line strings containing characters which
;;;		the shell is sensitive to.
(define bourne-escape-command-line-arg (lambda (str)
	(if (chars-in-string? str bourne-command-line-sensitive-chars)
		(string-append "'" str "'")
		str
	)
))
