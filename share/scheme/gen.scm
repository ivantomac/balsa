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
;;;	`gen.scm'
;;;	`gen' stage of balsa-netlist back end - `.abs' file format language
;;;

(balsa-scheme-import 'gen 'espresso)
(balsa-scheme-import 'gen 'mofn-espresso)
(balsa-scheme-import 'brz 'tech)

; (brz-get-technology)

;;; gen-bundle-binding:{name,cardinality,low-index,count}: field accessors for bundle binding list
;;;		declarations. (name cardinality low-index count) referring to an array of bundles each of width `width'
;;;		(== type `cardinality' bits) with indices [low-index,low-index+count-1]
(define gen-bundle-binding:name car)
(define gen-bundle-binding:cardinality cadr)
(define gen-bundle-binding:low-index caddr)
(define gen-bundle-binding:count cadddr)
(define gen-bundle-binding:width-list (lambda (bundle)
	(if (>= (length bundle) 5)
		(list-ref bundle 4)
		#f
	)
))
(define gen-bundle-binding:flags (lambda (bundle)
	(let ((raw-flags (find-headed-sub-list bundle 'flags)))
		(if raw-flags (cdr raw-flags) '())
	)
))

;;; gen-slice:{type,name,index,low-bit-index,cardinality}: field accessors for simple connections to
;;;		bundle req/ack/data elements Simple slices have one of the forms:
;;;			(req "name" index)
;;;			(req0 "name" index low-bit-index cardinality)
;;;			(req1 "name" index low-bit-index cardinality)
;;;			(ack "name" index)
;;;			(ack0 "name" index low-bit-index cardinality)
;;;			(ack1 "name" index low-bit-index cardinality)
;;;			(data "name" index low-bit-index cardinality)
;;;		Referring to the request, acknowledge and data components of bundle `index' of an arrayed bundle.
;;;		For the data varient the slice covers bits [low-bit-index+cardinality-1:low-bit-index]
(define gen-slice:type car)
(define gen-slice:name cadr)
(define gen-slice:index caddr)
(define gen-slice:low-bit-index cadddr)
(define gen-slice:cardinality (nth 4))
(define gen-slice-indexed? (lambda (slice) (> (length slice) 3)))

;;; gen-brz-ports->gen-bundle-binding: make a bundles declaration list from a list of specified ports
;;;		un-arrayed ports become arrays of one element with low-index of 0 and count of 0
;;;		eg. (port "inp" passive input (numeric-type #f 5)) => ("inp" 5 0 0).  Sync ports get a
;;;		cardinality of 0 and arrayed ports set all four elements of each gen-bundles properly.
(define gen-brz-ports->gen-bundle-bindings (lambda (specified-ports type-context)
	(map (lambda (port)
		(case (car port)
			((port)
				(list (brz-port:name port) (brz-type-width (brz-port:type port) type-context) 0 0)
			)
			((arrayed-port)
				(let*
					((type (brz-arrayed-port:type port))
					 (width (brz-type-width type '()))
					 (variable-array-widths 
					 	(case (car type)
					 		((variable-array-type)
								(list (brz-array-spec->width-list (brz-variable-array-type:spec-string type) width
									(brz-variable-array-type:element-count type))))
							((variable-width-array-type)
								(list (brz-variable-width-array-type:widths type)))
							(else '())
						)
					 )
					)
					`(,(brz-arrayed-port:name port) ,width
						,(brz-arrayed-port:low-index port) ,(brz-arrayed-port:port-count port) ,@variable-array-widths)
				)
			)
			((sync-port)
				(list (brz-sync-port:name port) 0 0 0)
			)
			((arrayed-sync-port)
				(list (brz-arrayed-sync-port:name port) 0
					(brz-arrayed-sync-port:low-index port) (brz-arrayed-sync-port:port-count port))
			)
		)
	) specified-ports)
))

;;; gen-portion-width: given a portion description eg. '(ack3 pull width-procedure) apply `width-procedure'
;;;		to the given width and return the normalised width of that portion.  ie. if width-procedure returns
;;;		#f then return 0, 0 then return 1, any other number then return that number.  Useful for finding real
;;;		portion widths.
(define gen-portion-width (lambda (portion width)
	(let ((portion-width-ret ((caddr portion) width))) ; FIXME, sanity check on portion?
		(cond
			((not portion-width-ret) 0)
			((zero? portion-width-ret) 1)
			(else portion-width-ret)
		)
	)
))

;;; gen-find-bundle-binding: find a bundle of a given name in a list of bundle bindings
(define gen-find-bundle-binding assoc)

;;; gen-expand-bundles: expand gen-bundles terms into a list of (bundle "name" index)
;;;		simple terms.  (each ...) and (bundles ...) forms are expanded in ascending bundle index order
;;;		Bindings is a list of ("name" cardinality low-index count) bundle array declarations to refer to.
;;;		`actual' and `formal' are the actual and formal params. for the defn. in which these bundles occur
(define gen-expand-bundles (lambda (bundles bindings defines actual formal)
	(flatten-unheaded-list (map (lambda (bundle)
		(cond
			((string? bundle) ; "name": a single bundle
				(list 'bundle bundle 0)
			)
			((headed-list? bundle)
				(let
					((specify-expr (lambda (expr) (brz-specify-expression expr defines actual formal))))
					(case (car bundle)
						((each) ; (each "name")
							(let*
								((binding (gen-find-bundle-binding (cadr bundle) bindings))
								 (low-index (gen-bundle-binding:low-index binding))
								 (count (if (zero? (gen-bundle-binding:count binding))
									1 (gen-bundle-binding:count binding)))
								)
								(map (lambda (i) (list 'bundle (cadr bundle) i))
									(.. low-index (+ low-index count -1)))
							)
						)
						((bundle) ; (bundle "name" index)
							(list 'bundle (cadr bundle) (specify-expr (caddr bundle)))
						)
						((bundles) ; (bundles "name" low-index count)
							(let*
								((low-index (specify-expr (caddr bundle)))
								 (high-index (+ low-index -1 (specify-expr (cadddr bundle))))
								)
								(map (lambda (i) (list 'bundle (cadr bundle) i))
									(.. low-index high-index))
							)
						)
						((bundles-at-indices) ; (bundles-at-indices "name" indices)
							(let*
								((indices (specify-expr (caddr bundle)))
								)
								(map (lambda (i) (list 'bundle (cadr bundle) i))
									indices)
							)
						)
					)
				)
			)
			; FIXME, error reporting
		)
	) bundles))
))

;;; gen-expand-slices: expand a list of abs-connection terms (req/ack/data <gen-bundles> ...)
;;;		into a list of lists of simple slices.  NB. these slices should be combined in left->right
;;;		least->most significant manner.  `actual' and `formal' are the actual and formal params.
;;;		for the defn. in which these conns. occur, `defines' is a list of symbol bindings for expressions
(define gen-expand-slices (lambda (conns bindings defines actual formal)
	(let
		((specify-expr (lambda (expr) (brz-specify-expression expr defines actual formal))))
		(append-map! (lambda (conn)
			(if (memv (car conn) all-portion-names)  ;req/ack/data simple bundle->connection mappings
				(let*
					((expanded (gen-expand-bundles (cdr conn) bindings defines actual formal)))
					(map (lambda (simple-bundle)
						; portion expansion.  Check if this is a full-width portion
						(let* 
							((bundles (gen-find-bundle-binding (cadr simple-bundle) bindings))
							 (flt (if (not bundles)
							 	(print "Error with bundle: `" (cadr simple-bundle) "'" #\newline bindings #\newline)))
							 (bundle-index (caddr simple-bundle))
							 (binding-width-list (gen-bundle-binding:width-list bundles))
							 (portion (find-headed-list-elem all-portions (car conn) #f))
							 (width ((caddr portion)
							 	(if binding-width-list
									(list-ref binding-width-list bundle-index)
							 		(gen-bundle-binding:cardinality bundles)
								)
							 ))
							)
							(list 	
								(if (zero? width)
									 (list (car conn) (cadr simple-bundle) (caddr simple-bundle))
									 (list (car conn) (cadr simple-bundle) (caddr simple-bundle) 0 width)
								)
							)
						)
					) expanded)
				)
				(case (car conn)
					((filter) ; (filter mask input-slice), filter from a single slice
						(let
							((input-slices (gen-expand-slices (cddr conn) bindings defines actual formal)))	
							(if (or (/= 1 (length input-slices)) (/= 1 (length (car input-slices))))
								(error "gen: `filter' slice operator usage: (filter mask single-slice)")
							)
							(let*
								((slice (caar input-slices))
								 (low-index (if (gen-slice-indexed? slice) (gen-slice:low-bit-index slice) 0))
								 (cardinality (gen-slice-cardinality slice))
								 (mask-bits (integer->binary-list (specify-expr (cadr conn)) cardinality))
								 ; make-slice: make a slice with appropriate req/ack/data symbol
								)
								(list (car (foldl-ma (lambda (bit in-slices index)
									(list (if bit
										(append in-slices
											(list (gen-extract-slice-bit-at-index slice index)))
										in-slices
									) (+ 1 index) )
								) mask-bits '() low-index)))
							)
						)
					)
					((combine) ; (combine slices ...) 
						(let
							((expanded (gen-expand-slices (cdr conn) bindings defines actual formal)))
							(list (concatenate expanded))
						)
					)
					((smash) ; (smash slices ...)
						(let*
							((expanded (gen-expand-slices (cdr conn) bindings defines actual formal))
							 (smashed
								(append-map! (lambda (slice)
									 (map list (gen-smash-simple-slice slice)) 
								) (flatten-unheaded-list expanded))
							 )
							)
							smashed
						)
					)
					((slice) ; (slice low-bit-index cardinality single-slice ...) 
						(let*
							((input-slices
								(gen-expand-slices (cdddr conn) bindings defines actual formal)
							))
							(for-each (lambda (slice)
								(if (or (/= 1 (length slice)) #f ) ; (/= 1 (length (car slice)))
									(error "gen: `slices' slice operator usage:"
										"(slice low-bit-index cardinality single-slice ...)")
								)
							) input-slices)
							; FIXME test bounds
							(append-map! (lambda (input-slices)
								(let*
									((slice (car input-slices))
									 (low-bit-index (specify-expr (cadr conn)))
									 (cardinality (specify-expr (caddr conn)))
									)
									(if (gen-slice-indexed? slice)
										(list (list (list
											(gen-slice:type slice) (cadr slice) (caddr slice)
												(+ low-bit-index (cadddr slice)) cardinality)
										))
										(error "gen: `slice' slice operator only works on indexable slices")
									)
								)
							) input-slices)
						)
					)
					((dup) ; (dup count slices ...) '(dup 3 A B) => A B A B A B
						(let
							((expanded-slices (gen-expand-slices (cddr conn) bindings defines actual formal)))	
							(concatenate (dup expanded-slices (specify-expr (cadr conn))))
						)
					)
					((dup-each) ; (dup-each count slices ...) '(dup-each 3 A B) => A A A B B B
						(let
							((expanded-slices (gen-expand-slices (cddr conn) bindings defines actual formal))
							 (count (specify-expr (cadr conn)))
							)
							(append-map! (lambda (slice) (dup slice count)) expanded-slices)
						)
					)
					(else (error "gen: invalid slice operation `" conn "'"))
				)
			)
		) conns)
	)
))

;;; gen-smash-simple-slice: smash a simple slices into singleton fragments
(define gen-smash-simple-slice (lambda (slice)
	(if (gen-slice-indexed? slice)
	 	(map (lambda (index)
			(list (car slice) (gen-slice:name slice) (gen-slice:index slice) index 1)
		) (.. (gen-slice:low-bit-index slice) (+ -1 (gen-slice:low-bit-index slice)
			(gen-slice:cardinality slice))))
		(list slice)
	)
))

;;; gen-slice-cardinality: get the cardinality of a simple (ie. single element of expansion with
;;;		gen-expand-slices) slice. 1 for req/ack, n for data ...
(define gen-slice-cardinality (lambda (slice)
	(if (gen-slice-indexed? slice)
		(gen-slice:cardinality slice)
		1
	)
))

;;; gen-slices-cardinality: get the cardinality of a list of slices combined 
(define gen-slices-cardinality (lambda (slices)
	(fold (lambda (slice cardinality) (+ cardinality (gen-slice-cardinality slice))) 0 slices)
))

;;; gen-extract-slice-bit-at-index: make a singleton slice from `slice' using the bit at index
;;;		`index' into that slice
(define gen-extract-slice-bit-at-index (lambda (slice index)
	(if (gen-slice-indexed? slice)
		(list (gen-slice:type slice) (gen-slice:name slice) (gen-slice:index slice)
			(+ (gen-slice:low-bit-index slice) index) 1) ; this is alright for dual 1of4 because
			; only using single slice req0 etc, mot whole bit 
		slice
	)
))

;;; gen-extract-slices-bit-at-index: extract a singleton slice from
;;;		a given slice list (which represents a `combine' type slice).  The single
;;;		bit coming from the slice which lies across the given index.  Slice lists
;;;		are little endian as always.
(define gen-extract-slices-bit-at-index (lambda (slices index)
	(caddr (while (lambda (current-index slices result) (not result))
		(lambda (current-index slices result)
			; bit is in current slice
			(let*
				((this-width (gen-slice-cardinality (car slices)))
				 (next-index (+ current-index this-width))
				)
				(if (> next-index index)
					(list 0 '() (gen-extract-slice-bit-at-index (car slices) (- index current-index)))
					(list next-index (cdr slices) #f)
				)
			)
		)
		0 slices #f
	)) 
))

;;; gen-make-decoder/encoder: make binary<->unary en/decoder for the given terms.
;;;		Returns (internal-name-index' new-bindings . gates).  `new-bindings' will be a list of
;;;		.abs file format bindings for internal nodes to the generated netlist fragment described by `gates'.
;;;		If decodeNencode is true then generate a decoder (binary->one-hot), otherwise generate an encoder
;;;		(one-hot->binary).  `and-gate' is the name of the element (and/c-element) to be used for the and plane
(define gen-make-decoder/encoder (lambda
	(spec false-slices true-slices output-slices internal-name-prefix internal-name-index decodeNencode and-gate compl)
	(let*
		((smash-slices (lambda (slices)
			(append-map gen-smash-simple-slice slices)
		 ))
		 (false-bits (smash-slices false-slices)) ; smashed true/complement/output slices
		 (true-bits (smash-slices true-slices))
		 (output-bits (smash-slices output-slices))
		 (input-width (length true-bits))
		 (output-width (length output-bits))
		 (implicant-width (if decodeNencode input-width output-width))
		 ; spec parsed into an implicant list list
		 (implicantss (gen-make-case-spec-implicants (brz-parse-case-spec spec)))
		 (adjusted-implicantss (if compl
			(hier-map 2 (lambda (imp)
				(complement-implicant imp implicant-width)
			) implicantss)
			implicantss
		 )) 
		 (complement-implicants (if (not decodeNencode)
			'() ; encoder, no `else' clause
			; (remaining-implicants adjusted-implicants implicant-width)
			(remaining-implicants-from-implicantss adjusted-implicantss implicant-width)
		 ))
		 ; final-implicants: final parsed spec with else at end (if any)
		 (final-implicantss (if (null? complement-implicants)
			adjusted-implicantss
			(append! adjusted-implicantss (list complement-implicants))
		 ))
		 (file-name-prefix (if decodeNencode
			"tmp-espresso-decode"
			"tmp-espresso-encode"
		 ))
		 (input-file-name (string-append file-name-prefix "-input"))
		 (output-file-name (string-append file-name-prefix "-output"))
		 ; and-terms/or-terms: espresso optimised expressions for outputs
		 (and-terms/or-terms (begin
			(with-output-to-file input-file-name (lambda ()
				(gen-print-espresso-file final-implicantss
					(if decodeNencode input-width output-width) decodeNencode)))
			(system (string-append "balsa-espresso " input-file-name " " output-file-name))
			(let ((sop (gen-make-sop-for-espresso-file output-file-name)))
				(system (string-append "/bin/rm -f " input-file-name " " output-file-name))
				sop
			)
		 ))
		 ; term-count: number of terms in the espresso output
		 (term-count (length (car and-terms/or-terms)))
		 ; internal-name: name of internal wires
		 (internal-name (string-append internal-name-prefix (number->string internal-name-index)))
		 ; internal-wire: generate a singleton slice for internal-name[index]
		 (internal-wire (lambda (index)
			(list 'node internal-name 0 index 1)
		 ))
		 ; and-term-gates: gates to generate and-terms
		 (and-term-gates (map (lambda (term index)
			(cons and-gate (cons (internal-wire index)
				(map (lambda (terminal)
					(list-ref (if (cdr terminal) true-bits false-bits) (car terminal))
				) term)
			))
		 ) (car and-terms/or-terms) (.. 0 (- term-count 1))))
		 ; or-term-gates: gates for or-terms to outputs
		 (or-term-gates (map (lambda (and-term-index-list output-index)
			(cond
				((null? and-term-index-list) ; 0
					(list 'connect
						(list 'node tech-gnd-net-name 0 0 1)
						(list-ref output-bits output-index)
					)
				)
				(else (cons 'or (cons (list-ref output-bits output-index)
					(map internal-wire and-term-index-list))))
			)
		 ) (cdr and-terms/or-terms) (integer-range-list (- output-width 1) 0)))
		)
		(cons
			(+ 1 internal-name-index)
			(cons (if (not (zero? term-count)) (list (list internal-name term-count 0 1)) '())
			(append! and-term-gates or-term-gates)
		))
	)
))

;;; gen-make-mofn-decoder/encoder: make binary<->unary en/decoder for the given terms.
;;; 	This version makes m-of-n style sutible for all m-of-n codes, but requires a mapping-function
;;; 	to map implicants to required encoding.
;;;		Returns (internal-name-index' new-bindings . gates).  `new-bindings' will be a list of
;;;		.abs file format bindings for internal nodes to the generated netlist fragment described by `gates'.
;;;		If decodeNencode is true then generate a decoder (binary->one-hot), otherwise generate an encoder
;;;		(one-hot->binary).  `and-gate' is the name of the element (and/c-element) to be used for the and plane
(define gen-make-mofn-decoder/encoder (lambda
	(spec input-slices output-slices width internal-name-prefix internal-name-index decodeNencode and-gate dims mapping-function exp-args)
	(let*
		((smash-slices (lambda (slices)
			(append-map gen-smash-simple-slice slices)
		 ))
		 (num-input-slices (length input-slices))
		 (num-output-slices (length output-slices))
		 (input-bits (map smash-slices input-slices)) ; smashed slices
		 (output-bits (map smash-slices output-slices))
		 (implicant-width (apply + (map length (if decodeNencode input-bits output-bits))))
		 (input-width (if decodeNencode implicant-width (length (car input-slices)))) ;;Input width if decoder = to implicant width
		 (output-width (if decodeNencode (length (car output-slices)) implicant-width)) ;; Output width if encdoer = to implicant width
		 ; spec parsed into an implicant list list
		 (implicantss (gen-make-case-spec-implicants (brz-parse-case-spec spec)))
		 (complement-implicants (if (not decodeNencode)
			'() ; encoder, no `else' clause
			; (remaining-implicants adjusted-implicants implicant-width)
			(remaining-implicants-from-implicantss implicantss width)
		 ))
		 ; final-implicants: final parsed spec with else at end (if any)
		 (final-implicantss (if (null? complement-implicants)
			implicantss
			(append! implicantss (list complement-implicants))
		 ))
		 (file-name-prefix (if decodeNencode
			"tmp-espresso-decode"
			"tmp-espresso-encode"
		 ))
		 (input-file-name (string-append file-name-prefix "-input"))
		 (output-file-name (string-append file-name-prefix "-output"))
		 ; and-terms/or-terms: espresso optimised expressions for outputs
		(and-terms/or-terms (begin
			(with-output-to-file input-file-name (lambda ()
				(gen-print-mofn-espresso-file final-implicantss
					implicant-width decodeNencode mapping-function exp-args))) ;; pass in implicant width as more useful, can obtain the bit width in mapping function
			(if dims
				(system (string-append "cat " input-file-name " > " output-file-name))
				(system (string-append "balsa-espresso " input-file-name " " output-file-name))
			)
			(let ((sop (gen-make-mofn-sop-for-espresso-file output-file-name num-input-slices)))
				(system (string-append "/bin/rm -f " input-file-name " " output-file-name))
				sop
			)
		 ))
		 ; term-count: number of terms in the espresso output
		 (term-count (length (car and-terms/or-terms)))
		 ; internal-name: name of internal wires
		 (internal-name (string-append internal-name-prefix (number->string internal-name-index)))
		 ; internal-wire: generate a singleton slice for internal-name[index]
		 (internal-wire (lambda (index)
			(list 'node internal-name 0 index 1)
		 ))
		; and-term-gates: gates to generate and-terms
		 (and-term-gates (map (lambda (term index)
				(cons and-gate (cons (internal-wire index)
					(fold (lambda (terminal res)
						(if (null? terminal)
							res
							(append res
								(map (lambda (terminus)
									(list-ref (list-ref input-bits (cdr terminus)) (car terminus))
								) terminal)
							)
						)
					) '() term)
				))
	 		) (car and-terms/or-terms) (.. 0 (- term-count 1))))
		 ; or-term-gates: gates for or-terms to outputs
		(or-term-gates (letrec
				((make-or (lambda (portion-index output-index tail res)
					(if (null? tail)
						res
						(make-or
							(modulo (+ portion-index 1) num-output-slices)
							(+ output-index 1)
							(cdr tail)
							(cons
								(cond
									((null? (car tail)) ; 0
										(list 'connect
											(list 'node tech-gnd-net-name 0 0 1)
											(list-ref (list-ref output-bits  portion-index) (quotient output-index num-output-slices))
										)
									)
									(else 
										(cons 'or 
										(cons (list-ref (list-ref output-bits portion-index) (quotient output-index num-output-slices))
										(map internal-wire (car tail)))))
								)
								res
							)
						)
					)
				)))
				(make-or 0 0 (cadr and-terms/or-terms) '())
			))
		)
		(cons
			(+ 1 internal-name-index)
			(cons (if (not (zero? term-count)) (list (list internal-name term-count 0 1)) '())
			(append! and-term-gates or-term-gates)
		))
	)
))

;;; gen-expand-abs-gate: expand an abstract gate with slice/bundle connections
;;;		into a list of singleton-ported gates with simple slice list connections.
;;;		`bindings' is a list of bundle bindings identifying ports and local bundles which
;;;		are included in the connections to the given gate.  Returns a list of
;;;		(internal-node-index new-bindings . singleton-ported-gates) where `new-bindings' is a
;;;		list of bindings in the same format as `bindings' for signals which are internal to gate expansions.
;;;		The returned `internal-node-index' is the next index which can safely be used to name an internal
;;;		node (ie. all the nodes in this gate expansion are between internal-name-prefix(pre) and
;;;		internal-name-index(post) - 1
;;;		The names of internal signals are derived from the prefix `internal-name-prefix' and the
;;;		index `internal-node-index'(pre) and should not be accessed from other gates than those in the
;;;		current expansion.
;;;		Can return #f indicating that no components were generated
(define gen-expand-abs-gate (lambda
	(gate defines actual-parameters formal-parameters bindings internal-name-prefix internal-name-index)
	(let*
	 	((specify-expr (lambda (expr) (brz-specify-expression expr defines actual-parameters formal-parameters)))
		 (call-macro (lambda (macro args)
			((macro 'new-defines defines #f) args actual-parameters formal-parameters)	
		 ))
		 ; expand slice/bundle connections to a list of lists of simple slices
		 (expand-connections (lambda (connections)
			(gen-expand-slices connections bindings defines actual-parameters formal-parameters)
		 ))
		 ; All the connections must be the same width
		 (check-connection-widths (lambda (connections)
			(let*
			 	((widths (map (lambda (slice-part-list)
					(gen-slices-cardinality slice-part-list)
				 ) connections))
				 (first-width (car widths))
				)
				(if (not (null? (filter (lambda (w) (/= w first-width)) widths))) 
					(error "gen: slices are of unequal sizes" #\newline #\ht "gate: " gate
						#\newline #\ht "widths: " widths)
				)
				first-width
			)
		 ))
		 ; make-singleton-gates: expand wide inputs into many gates of singleton inputs
		 (make-singleton-gates (lambda (gate-type gate-arguments width connections)
			(map (lambda (index)
				(append (cons gate-type gate-arguments)
					(map (lambda (slices)
						(gen-extract-slices-bit-at-index slices index)
					) connections)
				)
			) (.. 0 (- width 1)))
		 ))
		 ; no-new-bindings: wrapper for gates only return values
		 (no-new-bindings (lambda (gates) (cons internal-name-index (cons '() gates))))
		 ; null-return: no bindings/no gates
		 (null-return (cons internal-name-index (cons '() '())))
		 ; self{,-fold}: recurse on single gate/list
		 (self (lambda (gate) (gen-expand-abs-gate gate defines actual-parameters formal-parameters bindings
			internal-name-prefix internal-name-index)))
		 (self-fold (lambda (gates)
			(fold (lambda (abs-gate ret)
				(let ; call self on each gate and carry over the internal-name-index: now changed to pass new bindings
					((i/b/g (gen-expand-abs-gate abs-gate defines actual-parameters formal-parameters (append bindings (cadr ret))
						internal-name-prefix (car ret))
					))
					(if i/b/g
						(cons* (car i/b/g) (append (cadr i/b/g) (cadr ret)) (append (cddr i/b/g) (cddr ret)))
						ret
					)
				)
			) null-return gates)
		 ))
		)
		(if (not (pair? gate))
			(error "gen: invalid abstract gate form `" gate "'")
			(case (car gate)
				((width)
					(print-err (check-connection-widths (expand-connections (cdr gate))) #\newline)
					null-return
				)
				; argument-less gates with singleton ports
				((c-element s-element latch latch-nenable tri-buffer tri-inv connect or and nand nor
					c-element2-sp mutex gnd vcc xor2 xnor2 inv keeper-inv demux2 mux2 adder)
					(let*
						((abs-conns (expand-connections (cdr gate)))
						 (width (check-connection-widths abs-conns))
						)
						(no-new-bindings (make-singleton-gates (car gate) '() width abs-conns))
					)
				)	
				; helper cell instances
				((cell)
					(let*
						((abs-conns (expand-connections (cddr gate)))
						 (width (check-connection-widths abs-conns))
						)
						(no-new-bindings
							(make-singleton-gates (car gate) (list (specify-expr (cadr gate))) width abs-conns)
						)
					)
				)	
				; cell instance with possibly non-1 widths (ie. these can't be arrayed)
				((unarrayed-cell)
					(let* ((abs-conns (expand-connections (cddr gate))))
						(no-new-bindings
							(list (cons* 'cell (specify-expr (cadr gate)) (concatenate abs-conns)))
						)
					)
				)
				; (constant value slice)
				((constant)
					(let*
						((abs-conns (expand-connections (cddr gate)))
						 (width (check-connection-widths abs-conns))
						 (value (specify-expr (cadr gate)))
						)
						(no-new-bindings (list (cons 'constant (cons value (concatenate abs-conns)))))
					)
				)
				; (decode options spec false-input-slice true-input-slice out-slices)
				;	Make a Case component's decode gates taking true and complement dual rail inputs
				; (encode options spec false-input-slice true-input-slice out-slice)
				;  Make an Encode one-hot -> binary encoding gates
				; options is either a symbol indicating the type of encoder/decoder to produce
				; or a list of (non-conflicting) options.  Currently there are only three options:
				;	and-or - normal, optimised sum of products (AND/OR) implementation (default)
				;	c-or - c-element/OR implementation 
				;   compl - complements each term of spec before generating the encoder/decoder
				; FIXME The arguments to m-of-n mapping include lists of slice lists
				;; this relies on the fact that expand connections produces a single list of slices,
				;; if changes to the way expand connections produces slices this may break...
				((decode encode)
					(let
						((options (if (symbol? (cadr gate)) (list (cadr gate)) (cadr gate))))
						(if (memv 'm-of-n-mapping options) ;; Perform m-of-n-mapping
							(let*
								((input-slices (map (lambda (inp-slice)
									(concatenate (expand-connections (list inp-slice)))) (list-ref gate 4)))
								 (output-slices (map (lambda (out-slice)
								 	(concatenate (expand-connections (list out-slice)))) (list-ref gate 5)))
								 (imp-width (specify-expr (list-ref gate 3)))
								 ;; mapping function must be last in option list
								 (map-fun (list-ref options (- (length options) 1)))
								 (spec (specify-expr (caddr gate)))
								)
								(gen-make-mofn-decoder/encoder spec input-slices output-slices
									imp-width internal-name-prefix internal-name-index
									(eq? 'decode (car gate))
									(if (memv 'c-or options)
										'c-element
										'and
									)
									(memv 'dims options)
									map-fun 
									(list defines actual-parameters formal-parameters)
								)
							)
							(let
								((false-slices (concatenate (expand-connections (list (list-ref gate 3)))))
								 (true-slices (concatenate (expand-connections (list (list-ref gate 4)))))
						 		 (output-slices (concatenate (expand-connections (list-tail gate 5))))
							 	 (options (if (symbol? (cadr gate)) (list (cadr gate)) (cadr gate)))
								)
								(gen-make-decoder/encoder
									(specify-expr (caddr gate))
									false-slices true-slices output-slices
									internal-name-prefix internal-name-index
									(eq? 'decode (car gate))
									(if (memv 'c-or options)
										'c-element
										'and
									)
									(memv 'compl options)
								)
							)
						)
					)
				)
				((node-decl)  ;; (node-decl name card low-index count [widths])
					(let*
						((name (specify-expr (cadr gate)))
						 (card (specify-expr (caddr gate)))
						 (low-index (specify-expr (cadddr gate)))
						 (count (specify-expr (list-ref gate 4)))
						 (width-list 
						 	(if (> (length gate) 5)
						 		(list (specify-expr (list-ref gate 5)))
						 		'()
						 	)
						 )
						 (new-node (cons* name card low-index count width-list))
						)
						(if (zero? card)
							null-return
							(cons internal-name-index (cons (list new-node) '()))
						)
					)
				)
				((assignable-node-decl)  ;; (assignable-node-decl name card low-index count [widths])
					(let*
						((name (specify-expr (cadr gate)))
						 (card (specify-expr (caddr gate)))
						 (low-index (specify-expr (cadddr gate)))
						 (count (specify-expr (list-ref gate 4)))
						 (width-list 
						 	(if (> (length gate) 5)
						 		(list (specify-expr (list-ref gate 5)))
						 		(make-list count card)
						 	)
						 )
						 (new-node (list name card low-index count width-list '(flags assignable)))
						)
						(if (zero? card)
							null-return
							(cons internal-name-index (cons (list new-node) '()))
						)
					)
				)
				; (cond ...) Conditionals!
				((cond)
					(let
						((true-term (find-with-predicate (cdr gate) (lambda (term)
							(or (eq? 'else (car term)) (specify-expr (car term)))
						))))
						(if true-term
							(self-fold (cdr true-term))
							null-return
						)
					)
				)
				; (if condition consequence alternative)
				((if)
					(if (specify-expr (cadr gate))
						(self (caddr gate))
						(if (not (null? (cdddr gate))) (self (cadddr gate)) #f)
					)
				)
				((print)
					(print "debugging print: ")
					(for-each (lambda (expr)
						(print (specify-expr expr) " ")
					) (cdr gate))
					(print #\newline)
					null-return
				)
				((print-conn)
					(print "debugging print: ")
					(for-each (lambda (expr)
						(print (expand-connections (list expr)) " ")
					) (cdr gate))
					(print #\newline)
				)
				((raw-print)
					(print "raw debugging print: ")
					(for-each (lambda (expr)
						(write expr)
						(print " ")
					) (cdr gate))
					(print #\newline)
					null-return
				)
				; (case value terms ...) equal? compared alternatives
				((case)
					(let*
						((value (specify-expr (cadr gate)))
						 (true-term (find-with-predicate (cddr gate) (lambda (term)
							(or (eq? 'else (car term)) (member value (car term)))
						 )))
						)
						(if true-term
							(self-fold (cdr true-term))
							null-return
						)
					)
				)
				; (gates ...) many gates as one
				((gates) (self-fold (cdr gate)))
				; (macro macro-expr args ...) call a macro and do self and the result
				((macro)
					(if (< (length gate) 2)
						(error "gen: invalid macro call `" gate "'")
						(let
							((macro (specify-expr (cadr gate)))
							 (args (cddr gate))
							)
							(if (not (procedure? macro))
								(error "gen: `" macro "' is not a lambda in macro call")
								(self (call-macro macro args))
							)
						)
					)
				)
				((shdl shdl-repeat shdl-assign)
					(let
						((subst-connection (lambda (conn) (list 'net-conns 
							(concatenate (expand-connections (list conn)))))))
						(no-new-bindings
							(list
								(list-replace-headed
									(list-replace-headed gate 'abs-expr specify-expr)
									'abs-conn
									subst-connection
								)
							)
						)
					)
				)
				(else (error "gen: invalid abstract gate `" gate "'"))
			)
		)
	)
))
