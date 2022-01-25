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
;;;	`brz.scm'
;;;	Parsed breeze file manipulation procedures
;;;

(balsa-scheme-import 'misc 'spans)
(balsa-scheme-import 'misc 'implicants)
(balsa-scheme-import 'brz 'parser)
(balsa-scheme-import 'brz 'tech)

;;;;; File name block name / string processing

;;; brz-trim-square-brackets: trim square brackets from around a block name
(define brz-trim-square-brackets (lambda (block)
	(if (char=? #\[ (string-ref block 0))
		(substring block 1 (- (string-length block) 1))
		block
	)
))

;;; brz-dotted-to-slashed-name: transform a dotted-path name into a slashed-path name
(define brz-dotted-to-slashed-name (lambda (name)
	(let
		((dot-to-slash (lambda (chr)
		 	(if (char=? #\. chr)
			    #\/
			    chr
			)
		)))
	 (list->string (map dot-to-slash (string->list name)))
	)
))

;;; brz-canonicalise-block-name: make sure a given block name has exactly one
;;;		set of enclosing square brackets
(define brz-canonicalise-block-name (lambda (block-name)
	(string-append "[" (brz-trim-square-brackets block-name) "]")
))

;;;;; Implicit Balsa library knowledge

;;; brz-library-balsa-blocks: blocks which are present in the $(BALSAHOME)/lib/{balsa,breeze}
;;;		hierachies and which should be the stopping level for rules by balsa-make-makefile.
;;;		NB. this list is ordered.
(define brz-library-balsa-blocks
	'("[balsa.parts.buffers]" "[balsa.parts.counters]"
	  "[balsa.sim.fileio]" "[balsa.sim.memory]" "[balsa.sim.portio]" "[balsa.sim.sim]" "[balsa.sim.string]"
	  "[balsa.types.basic]" "[balsa.types.builtin]" "[balsa.types.synthesis]" "[balsa.types.types]"
	)
)

;;;;; File handling / Primitives

;;; brz-get-flattened-breeze-block: find an breeze file from a given dotted-path/filename using get-block
(define brz-get-flattened-breeze-block (lambda (filename dotted-path?)
	(get-flattened-block
		(fold flip-string-append "" (find-filename filename "breeze" dotted-path? balsa-search-path))
		"breeze" '() balsa-search-path
	)
))

;;; brz-tech-build-include-filename: (include ...) filename construction building procedure
;;;		`extension' and `sub-directory' is applied to produce a procedure which will be used to parse
;;;		(include ...) lines (the ... bit). Giving #f for either option results in a procedure which doesn't use
;;;		extension/sub-directory offsets respectively.
;;;		Allowed forms:
;;;		(include "s1" "s2" ...) - read from file .../tech/this-tech/sub-directory/s1/s2.extension
;;;		(include 'tech "tech" "s1" "s2" ...) - read from file .../tech/some-tech/sub-directory/s1/s2.extension
;;;		(include 'style "style" "s1" "s2" ...) - read from file .../style/some-style/sub-directory/s1/s2.extension
(define brz-tech-build-include-filename (lambda (sub-directory extension)
	(let
		((tech-base-dir (string-append data-dir "tech/"))
		 (style-base-dir (string-append data-dir "style/"))
		 (sub-dir-string (if sub-directory (string-append sub-directory "/") ""))
		 (extension-string (if extension (string-append "." extension) ""))
		)
		(lambda (path-comps)
			(if (headed-list? path-comps)
				(case (car path-comps)
					((tech)
						(string-append tech-base-dir (cadr path-comps) "/" sub-dir-string
							(build-separated-string (cddr path-comps) "/") extension-string)
					)
					((style)
						(string-append style-base-dir (cadr path-comps) "/" sub-dir-string
							(build-separated-string (cddr path-comps) "/") extension-string)
					)
				)
				; all strings? Assume so
				(string-append breeze-tech-dir sub-dir-string
					(build-separated-string path-comps "/") extension-string)
			)
		)
	)
))

;;; brz-preprocess-file-contents: process include directives in the given list
;;;		using the filename building function (for the (include ...) contents)
(define brz-preprocess-file-contents (lambda (contents filename-build-func)
	(letrec
		((fold-inclusions (lambda (read-last-cell working-list)
			(if (null? working-list)
				read-last-cell ; required to allow resumption after inclusion
				(cond
					((not (pair? working-list))
						(set-cdr! read-last-cell working-list)
					)
					((headed-list? (car working-list) 'include)
						; patch read file onto the end of `read-last-cell' then run across doing
						; further inclusions. Afterwards past remainder of `working-list' onto the
						; tail of the included file.
						(let ((sub-file-contents (get-file (filename-build-func (cdar working-list)))))
							(set-cdr! read-last-cell sub-file-contents)
							(let ((inclusion-tail (fold-inclusions read-last-cell sub-file-contents)))
								(set-cdr! inclusion-tail (cdr working-list))
								(fold-inclusions inclusion-tail (cdr working-list))
							)
						)
					)
					; descend into headed lists
					((headed-list? (car working-list))
						(fold-inclusions (cons '() (car working-list)) (car working-list))
						(fold-inclusions (cdr read-last-cell) (cdr working-list))
					)
					(else ; do nothing, iterate
						(fold-inclusions (cdr read-last-cell) (cdr working-list))
					)
				)
			)
		)))
		(let
			((ret (cons '() contents)))
			; iterate over ret and insert included files, ret has a dummy head so that set-cdr! works
			(fold-inclusions ret contents)
			(cdr ret)
		)
	)
))

;;; brz-get-and-preprocess-file: get the given file's contents
(define brz-get-and-preprocess-file (lambda (filename filename-build-func)
	(brz-preprocess-file-contents (get-file filename) filename-build-func)
))

;;; brz-load-primitives: load the breeze primitives description list from the file breeze-primitives-file.
(define brz-load-primitives (lambda ()
	(if (not breeze-primitives)
		(let*
			((primitives-file-contents (brz-get-and-preprocess-file breeze-primitives-file
				(brz-tech-build-include-filename #f "abs")))
			 (primitive-parts (filter (lambda (elem)
			 	(headed-list? elem 'primitive-part)) primitives-file-contents))
			 (primitive-definitions ; specify definitions in order
				(fold (lambda (defn defines)
					(if (headed-list? defn)
						(case (car defn)
							((define) ; (define name value)
								(if (/= (length defn) 3)
									(error "brz-load-primitives: invalid define `" defn "'")
									(cons
										(list (cadr defn) (brz-specify-expression (caddr defn) defines '() '()))
										defines
									)
								)
							)
							((print) ; (print a b c ...)
								(print "brz-load-primitives: ")
								(for-each (lambda (expr)
									(write (brz-specify-expression expr defines '() '()))
									(print " ")
								) (cdr defn))
								(print #\newline)
								defines
							)
							(else defines)
						)
						defines
					)
				) `((style-options ,breeze-style-options)) primitives-file-contents)
			 )
			)
			(set! breeze-primitives primitive-parts)
			; tweak the defines for the lambdas
			(for-each (lambda (defn)
				(if (procedure? (cadr defn))
					(set-car! (cdr defn) ((cadr defn) 'new-defines primitive-definitions #f))
				)
			) primitive-definitions)
			(set! breeze-primitives-definitions primitive-definitions)
		)
	)
))

;;; brz-get-tech-file: get a file from the tech. tree using the same syntax as (include ...)
;;;		lines as the `file-path'
(define brz-get-tech-file (lambda file-path
	(let
		((process-file-path (brz-tech-build-include-filename #f #f)))
		(brz-get-and-preprocess-file (process-file-path file-path) process-file-path)
	)
))

;;; brz-pretty-format-tech-name: returns a string identifying the current technology
;;;		this will either be `tech/style' or `tech/style/style-options' depending on the
;;;		presence of style-options
(define brz-pretty-format-tech-name (lambda ()
	(string-append breeze-tech "/" breeze-style
		(if (null? breeze-style-options)
			""
			(string-append "/" (build-separated-string
				(map (lambda (arg) (if (eq? #t (cdr arg)) (car arg) (string-append (car arg) "=" (cdr arg))))
				breeze-style-options) ":"
			))
		)
	)
))

;;; brz-find-style-option: find the style option with the given (string) name. If the
;;;		option isn't present, return #f.  Returns #t for value-less options or the string value
;;;		of the option for valued options.
(define brz-find-style-option (lambda (name)
	(let ((option (find-with-predicate breeze-style-options (lambda (e) (string=? (car e) name)))))
		(if option
			(cdr option)
			#f
		)
	)
))

;;; brz-load-gates: load the target technology gates description list from the file breeze-gates-mapping-file
;;;		and the netlists for those cells from the files listed in the file
;;;		(string-append breeze-gates-net-files-dir "standard-cells.lst")
(define brz-load-gates (lambda ()
	(brz-get-technology)
	(if (not breeze-gate-mappings)
		(begin
			(set! breeze-gate-mappings (get-table-file breeze-gate-mappings breeze-gates-mapping-file))
			(let
				((net-file-list (if breeze-gates-net-files
					breeze-gates-net-files
					(get-file (string-append breeze-tech-dir "standard-cells.lst"))
				)))
				(set! breeze-gate-defns (fold (lambda (name circuits) 
					(append circuits (get-file (string-append breeze-tech-dir name ".net")))
				) '() net-file-list))
			)
		)
	)
))

;;; brz-find-primitive: find a breeze primitive description from the global
;;;		primitives list breeze-primitives
(define brz-find-primitive (lambda (name)
	(if breeze-primitives
		(let
			((primitive (find-headed-list-elem breeze-primitives 'primitive-part name)))
			(if (not primitive)
				(error (string-append "brz: can't find primitive: " name))
				primitive
			)
		)
		(error "brz: need to load breeze primitives descriptions")
	)
))

;;; brz-find-entity : make a find function for entity type `type'
(define brz-find-entity (lambda (type)
	(lambda (l name) (find-headed-list-elem l type name))
))

;;; brz-find-breeze-part: find a breeze-part in a list of same
(define brz-find-breeze-part (brz-find-entity 'breeze-part))

;;;;; Typing

;;; brz-type-context: type definitions for all the types which are valid in .breeze files
(define brz-type-context '(
	(type "bit" (numeric-type #f 1))
	(type "boolean" (enumeration-type #f 1 ("false" 0) ("true" 1)))
	(type "cardinal" (numeric-type #f 32))
	(type "UnaryOperator" (enumeration-type #f 1 ("Negate" 0) ("Invert" 1)))
	(type "BinaryOperator" (enumeration-type #f 4
		("Add" 0) ("Subtract" 1) ("ReverseSubtract" 2) ("Equals" 3)
		("NotEquals" 4) ("LessThan" 5) ("GreaterThan" 6) ("LessOrEquals" 7)
		("GreaterOrEquals" 8) ("And" 9) ("Or" 10) ("Xor" 11))
	)
))

;;; brz-type-width: get the width of the given type. Type may be any valid breeze
;;;		type (including a record or enumeration) sub-types look-ups refer to context
;;;		(which should be a list of type declarations), the first match being the reqd. type
(define brz-type-width (lambda (type context)
	(let
		((lookup-type (lambda (t)
			(let ((type (find-with-predicate context (lambda (x) (string=? t (brz-type-decl:name x))))))
				type
			)
		 ))
		 (self (lambda (type) (brz-type-width type context)))
		)
		(case (car type)
			((named-type) (self (brz-type-decl:body (lookup-type (brz-named-type:name type)))))
			((numeric-type) (brz-numeric-type:width type))
			((array-type) (* (brz-array-type:element-count type) (self (brz-array-type:type type))))
			((variable-array-type) (self (brz-variable-array-type:type type)))
			; ((variable-width-array-type) (error "variable-width-array-types don't have intrinsic widths"))
			((variable-width-array-type) (apply max (brz-variable-width-array-type:widths type)))
			((record-type) (brz-record-type:width type))
			((enumeration-type) (brz-enumeration-type:width type))
			((alias-type) (brz-type-width `(named-type ,(brz-alias-type:name type)) context))
			((builtin-type) 64)
			((sync-type) 0)
		)	
	)
))

;;; brz-is-named-type?: is the given type `type' the named type `name'
(define brz-is-named-type? (lambda (type name)
	(and (headed-list? type 'named-type) (string=? name (brz-named-type:name type)))
))

;;;;; Part Ports

;;; brz-port-push/pull: given a sense and direction returns 'push for a push channel and 'pull
;;;		for a pull channel
(define brz-port-push/pull (lambda (sense direction)
	(if (or (and (eqv? sense 'passive) (eqv? direction 'input))
		(and (eqv? sense 'active) (eqv? direction 'output)))
		'push
		'pull
	)
))

;;; brz-port=?: port name equality procedure
(define brz-port=? (lambda (l r) (string=? (brz-port:name l) (brz-port:name r))))

;;; brz-port-direction: return the direction, input/output/sync, of any given port
(define brz-port-direction (lambda (port)
	(case (car port)
		((port) (brz-port:direction port))
		((arrayed-port) (brz-arrayed-port:direction port))
		(else 'sync)
	)
))

;;; brz-port-port-count: returns the port count for sync-arrayed/arrayed ports and 1 for
;;;		other port types
(define brz-port-port-count (lambda (port)
	(case (car port)
		((arrayed-port) (brz-arrayed-port:port-count port))
		((arrayed-sync-port) (brz-arrayed-sync-port:port-count port))
		(else 1)
	)
))

;;; brz-port-low-index: returns the low port index for sync-arrayed/arrayed ports and 0 for
;;;		other port types
(define brz-port-low-index (lambda (port)
	(case (car port)
		((arrayed-port) (brz-arrayed-port:low-index port))
		((arrayed-sync-port) (brz-arrayed-sync-port:low-index port))
		(else 0)
	)
))

;;;;; Component Parameters

;;; brz-normalise-parameter-list: normalise the representation of a set of parameters
;;;		this includes: all symbols become strings, #t becomes "true" and #f becomes "false"
(define brz-normalise-parameter-list (lambda (parameters)
	(map (lambda (param)
		(cond
			((symbol? param) (symbol->string param))
			((boolean? param) (if param "true" "false"))
			(else param)
		)
	) parameters)
))

;;; brz-find-param-position: find the position of the parameter named `param-name' in the
;;;		headed parameter list param-spec.  Returns in index into param-spec if param-name
;;;		is found, #f if not found
(define brz-find-param-position (lambda (param-name formal-params)
	(let*
		((no-of-params (- (length formal-params) 1))
		 (pos-param-pairs (zip (.. 0 (- no-of-params 1)) (cdr formal-params)))
		 ; Return car of (position formal-params) pair if param-name is right
		 (pred (lambda (pos-param) (string=? param-name (brz-parameter:name (cadr pos-param))) ))
		 (match (find-with-predicate pos-param-pairs pred))
		)
		(if match
			(car match) ; just the position
			#f
		)
	)
))

;;; brz-make-expr-lambda-from-scheme-lambda: make a lambda suitable for
;;;		calling with the expression language from a scheme lambda
(define brz-make-lambda-from-scheme-lambda (lambda (scheme-lambda defines)
	(lambda (arguments actual formal)
		(if (eq? 'new-defines arguments)
			(brz-make-lambda-from-scheme-lambda scheme-lambda actual)
			(apply scheme-lambda arguments)
		)
	)
))

;;; brz-make-lambda-call-lambda: make a lambda for corresponding to a lambda defined
;;;		in brz-specify-expression.  `defines', `actual' and `formal' have the same meanings as in that procedure.
;;;		`arguments' is a list of arguments to the `defined-lambda' which have
;;;		already been specified.  Note that only `defines' and the lambda itself are required here.  The returned
;;;		lambda is supplied with the argument list and `actual' and `formal'
;;;		Supported lambda formats:
;;;			(lambda () expr1 expr2 ...)
;;;			(lambda (a1 a2 a3 ...) expr1 expr2)
;;;			(lambda args expr1 expr2 ...)
;;;			(lambda (a1 a2 a3 ... . rest-of-args) expr1 expr2 ...)
;;;		Note that if the lambda returned by this function is called using an `argument' of 'new-defines
;;;		then the `actual' argument is taken to be a new definitions binding list.  This allows lambdas
;;;		in let and global (define ...) statements to have their context redefined.   Applying the 'new-defines
;;;		`argument' returns a lambda similar to the called one ut with the new defines.
(define brz-make-lambda-call-lambda (lambda (defined-lambda defines)
	(if (not (cdr defined-lambda))
		(error "brz-make-lambda-call-lambda: empty lambda")
		(letrec
			; make-lambda: make the lambda call lambda using the given args-lambda, applied
			; (args-lambda arguments) to process the argument list.
			((make-lambda (lambda (args-lambda defines)
				(lambda (arguments actual formal)
					(if (eq? 'new-defines arguments)
						(make-lambda args-lambda actual)
						(fold (lambda (expr return-value)
							(brz-specify-expression expr (append (args-lambda arguments) defines) actual formal)
						) #f (cddr defined-lambda))
					)
				)
			)))
			(cond
				((symbol? (cadr defined-lambda)) ; (lambda symbol ...)
					(make-lambda (lambda (args) (list (list (cadr defined-lambda) args))) defines)
				)
				((null? (cadr defined-lambda)) ; (lambda () ...)
					(make-lambda (lambda (args)
						(if (not (null? args))
							(error "brz-make-lambda-call-lambda: lambda with no arguments passed `" args "'")
							'()
						)
					) defines)
				)
				((pair? (cadr defined-lambda)) ; All other forms
					(letrec
						((bind-defines (lambda (defines formal-args actual-args) ; bind lambda args
							(cond
								((null? formal-args)
									(if (not (null? actual-args))
										(error "brz-make-lambda-call-lambda: applied argument list too long")
									)
									defines
								)
								((pair? formal-args) ; step args
									(if (null? actual-args)
										(begin
											(error "brz-make-lambda-call-lambda: applied argument list too short")
											defines
										)
										(bind-defines (cons (list (car formal-args) (car actual-args)) defines)
											(cdr formal-args) (cdr actual-args))
									)
								)
								((symbol? formal-args)
									(cons (list formal-args actual-args) defines)
								)
								(else
									(error "brz-make-lambda-call-lambda: invalid (car formal-args) argument `"
										(car formal-args) "' in lambda `" lambda-name "'")
									defines
								)
							)
						)))
						(make-lambda (lambda (args)
							(bind-defines '() (cadr defined-lambda) args)
						) defines)
					)
				)
				(else (error "brz-make-lambda-call-lambda: invalid lambda defn. `" defined-lambda "'"))
			)
		)
	)
))

;;; brz-specify-let: handle (let ...) block expansion for brz-specify-expression
(define brz-specify-let (lambda (expr defines actual formal)
	(let ((specify-expr (lambda (expr defines) (brz-specify-expression expr defines actual formal))))
		(if (<= (length expr) 2)
			(error "brz-specify-let: invalid let block `" expr "`")
			(let
				((new-defines (fold (lambda (binding defines)
					(if (or (not (symbol? (car binding))) (/= (length binding) 2))
						(error "brz-specify-let: invalid binding `" binding "'")
						; allow lambda to see itself (if the rhs resolves to a lambda)
						(let*
							((specified-expr (specify-expr (cadr binding) defines))
							 (new-binding (list (car binding) specified-expr)) 
							 (new-bindings (cons new-binding defines))
							)
							(if (procedure? specified-expr)
								; rebind the lambda's defines to include itself
								(set-car! (cdr new-binding) (specified-expr 'new-defines new-bindings #f))
							)
							new-bindings
						)
					)
				 ) defines (cadr expr)))
				)
				; evaluate body expressions
				(fold (lambda (expr return-value)
					(specify-expr expr new-defines)
				) 'false (cddr expr))
			)
		)
	)
))

(define unique-name-counter 0)
(define unique-name (lambda (name)
	(set! unique-name-counter (+ 1 unique-name-counter))
	(string-append name (number->string unique-name-counter))
))

;;; brz-specify-expression: find the value of an expression by the application of the actual
;;;		parameters in `actual' in the context of the parameter specification `formal'
;;;		`defines' is a list of (name value) lists each of which defines a value to substitute
;;;		for each `name' symbol in the given expression
(define brz-specify-expression (lambda (expr defines actual formal)
	(let
		((self (lambda (sub-expr) (brz-specify-expression sub-expr defines actual formal))))
		(cond 
			((headed-list? expr)
				(case (car expr)
					((param) ; parameter from the actuals list
						(let
							((position (brz-find-param-position (cadr expr) formal)))
							(if (not position)
								(error "brz-specify-expression: can't find param `" (cadr expr) "'")
								; (self (list-ref actual position))
								; AB 2008-08-02
								(list-ref actual position)
							)
						)
					 )
					; complete-encoding? a case spec like span list list covers all values in the type
					; (caddr expr) bits
					((complete-encoding?) ; (complete-encoding? spec bit-count)
						(let*
							((spec (self (cadr expr)))
							 (bit-count (self (caddr expr)))
							 (implicantss (gen-make-case-spec-implicants (brz-parse-case-spec spec)))
							 (complement-implicants
								(remaining-implicants-from-implicantss implicantss bit-count))
							)
							(null? complement-implicants)
							;(->boolean (null? (span-list-complement (flatten-span-list-list
							;	(brz-parse-case-spec spec)) max-value)))
						)
					)
					((if) (if (self (cadr expr)) (self (caddr expr)) (self (cadddr expr))))
					((cond) ; (cond match ...) match ::= (cond expr ...) | (else expr ...)
						(let test
							((matches (cdr expr)))
							(if (null? matches)
								#f
								(let
									((match (car matches)))
									(if (or (not (pair? match)) (< (length match) 2))
										(error "brz-specify-espression: illegal cond term`" match "'")
										(if (or (eq? (car match) 'else)
												(self (car match)))
									 		(fold (lambda (expr acc) (self expr)) #f (cdr match))
											(test (cdr matches))
										)
									)
								)
							)
						)
					)
					((case)
						(let*
							((value (self (cadr expr)))
							(true-term (find-with-predicate (cddr expr) (lambda (term)
								(or (eq? 'else (car term)) (member value (car term)))
							)))
							)
							(if true-term
								; (self (cadr true-term))
								(fold (lambda (expr acc) (self expr)) #f (cdr true-term))
								(error "brz-specify-expression: must have a return value in case: `" expr "'")
							)
						)
					)
					((let) (brz-specify-let expr defines actual formal))
					((lambda) ; construct a lambda
						(if (or (< (length expr) 3) ; sanity check the lambda 
							(not (or (null? (cadr expr)) (symbol? (cadr expr)) (pair? (cadr expr)))))
							(error "brz-specify-expression: invalid lambda `" expr "'")
							(brz-make-lambda-call-lambda expr defines)
						)
					)
					((quote quasiquote) ; 'some-datum
						(if (/= (length expr) 2)
							(error "brz-specify-expression: invalid quoted datum `" expr "'")
							(letrec
								; foldl but also apply f to the (cdr (list-tail l)) if it isn't '()
								((foldl-not-list (lambda (f ret l)
									(cond
										((null? l) ret)
										((pair? l) (foldl-not-list f (f ret (car l)) (cdr l)))
										(else (f ret l))
									)
								 ))
								 (process-unquotes (lambda (ret term)
									(if (pair? term)
										(case (car term)
											((unquote)
												(if (/= (length expr) 2)
													(error "brz-specify-expression: invalid unquote expr. `" expr "'")
													(cons (self (cadr term)) ret)
												)
											)
											((unquote-splicing)
												(if (/= (length expr) 2)
													(error "brz-specify-expression: invalid unquote-splicing expr. `" expr "'")
													(let ((specified-arg (self (cadr term))))
														(if (list? specified-arg)
															(append! (reverse specified-arg) ret)	
															(error "brz-specify-expression: "
																"unquote-splicing arg. must be a list `" specified-arg "'")
														)
													)
												)
											)
											(else (cons (reverse! (foldl-not-list process-unquotes '() term)) ret))
										)
										(cons term ret)
									)
								 ))
								)
								(car (process-unquotes '() (cadr expr)))
							)
						)
					)
					((eval)
						(if (/= (length expr) 2)
							(error "brz-specify-expression: invalid eval expr. `" expr "'")
							(self (self (cadr expr)))
						)
					)
					(else ; A lambda or macro call
						(let*
							((is-macro (eq? 'macro (car expr)))
							 (is-apply (eq? 'apply (car expr)))
							 (defined-lambda
							 	(cond
							 		(is-macro
										(if (< (length expr) 2)
											(error "brz-specify-expression: invalid macro call `" expr "'")
											(self (cadr expr))
										)
									)
									(is-apply
										(if (< (length expr) 2)
											(error "brz-specify-expression: invalid apply call `" expr "'")
											(self (cadr expr))
										)
									)
							 		(else (self (car expr)))
							 	)
							 )
							 (lambda-name
							 	(cond
							 		(is-macro (cadr expr))
							 		(is-apply (cadr expr))
							 		(else (car expr))
							 	)
							 )
							)
							(if (procedure? defined-lambda)
								(if is-macro
									(let ((scope-rebound-lambda (defined-lambda 'new-defines defines #f)))
										(scope-rebound-lambda (cddr expr) actual formal) ; note lack of calls to self
									)
									(if is-apply
										(defined-lambda (apply cons* (map self (cddr expr))) actual formal)
										(defined-lambda (map self (cdr expr)) actual formal)
									)
								)
								(error "brz-specify-expression: not a valid lambda `" lambda-name "' (expands to `"
									defined-lambda "')")
							)
						)
					)
				)
			)
			((symbol? expr) ; true and false are no longer special, all symbols *must* be names of definitions
				(let ((sym-def (assv expr defines)))
					(if sym-def
						(cadr sym-def)
						(case expr
							((+ - * / and or not = /= > < >= <=
								quotient modulo min max expt pop-count bit-length
								find-set-bit find-clear-bit bit-set? bit-xor
								assoc cons cdr car cadr caar .. div
								list length reverse! sub-string pair?
								null? odd? print append make-string
								string-append string-set! string-length 
								substring note string?
								number->string string->number inexact->exact
								->string
								truncate log parse-case-spec
								unique-name fold mapi
								scheme-call scheme-eval
								) ; built in functions
								(let*
									((ffs (lambda (v i) (cdr (find-set-bit v i))))
									 (ffc (lambda (v i) (cdr (find-clear-bit v i))))
									 (pcs (lambda (str) (flatten-unheaded-list (brz-parse-case-spec str))))
									 (operator (case expr
										((+) +) ((-) -) ((*) *)
										((and) eager-and) ((or) eager-or) ((not) not)
										((=) equal?) ((/=) (lambda (l r) (not (equal? l r))))
										((>) >) ((<) <) ((>=) >=) ((<=) <=)
										((min) min) ((max) max)
										((expt) expt)
										((pop-count) logcount)
										((bit-length) bit-length)
										((/ quotient) quotient)
										((div) /)
										((modulo) modulo)
										((find-set-bit) ffs)
										((find-clear-bit) ffc)
										((bit-set?) logbit?)
										((bit-xor) logxor)
										((assoc) assoc)
										((cons) cons)
										((car) safe-car)
										((cdr) safe-cdr)
										((cadr) (lambda (arg) (safe-car (safe-cdr arg))))
										((caar) (lambda (arg) (safe-car (safe-car arg))))
										((..) ..)
										((list) list)
										((length) length)
										((reverse!) reverse!)
										((null?) null?)
										((odd?) odd?)
										((pair?) pair?)
										((parse-case-spec) pcs)
										((print) print)
										((append) append)
										((make-string) make-string)
										((string?) string?)
										((string-append) string-append)
										((string-set!) string-set!)
										((sub-string) sub-string)
										((string-length) string-length)
										((substring) substring)
										((note) note)
										((number->string) number->string)
										((string->number) string->number)
										((->string) ->string)
										((inexact->exact) inexact->exact)
										((truncate) truncate)
										((log) log)
										((unique-name) (lambda (name) (unique-name name)))
										((fold) (lambda (f . args)
											(apply fold (cons (lambda args
												(f args actual formal)
											) args))
										))
										((mapi) (lambda (f . args)
											(apply map (cons (lambda args
												(f args actual formal)
											) args))
										))
										((scheme-call) (lambda (f . args)
											(apply (primitive-eval f) args)
										))
										((scheme-eval) (lambda (expr)
											(primitive-eval expr)
										))
									)))
									(brz-make-lambda-from-scheme-lambda operator defines)
								)
							)
							(else (error "brz-specify-expression: `" expr "' is not a valid definition name"))
						)
					)
				)
			)
			((procedure? expr) expr) ; let lambdas fall through
			(else expr)
		)
	)
))

;;; brz-specify-type: expand expressions in type usage using `actual' and `formal' parameters
(define brz-specify-type (lambda (type actual formal)
	(let
		; 2008-07-03 AB, add primitives-definitions
		; ((expr (lambda (e) (brz-specify-expression e '() actual formal)))
		((expr (lambda (e) (brz-specify-expression e breeze-primitives-definitions actual formal)))
		 (self (lambda (t) (brz-specify-type t actual formal)))
		)
		(case (car type)
			((named-type) type) 
			((numeric-type)
				(list 'numeric-type (brz-numeric-type:signedness type) (expr (brz-numeric-type:width type))))
			((array-type) (list 'array-type (self (brz-array-type:type type)) (expr (brz-array-type:low-index type))
				(expr (brz-array-type:element-count type))))
			((variable-array-type) (list 'variable-array-type
				(self (brz-variable-array-type:type type))
				(expr (brz-variable-array-type:low-index type))
				(expr (brz-variable-array-type:element-count type))
				(expr (brz-variable-array-type:spec-string type))))
			((variable-width-array-type) (list 'variable-width-array-type
				(expr (brz-variable-width-array-type:low-index type))
				(expr (brz-variable-width-array-type:element-count type))
				(expr (brz-variable-width-array-type:widths type))))
		)
	)
))

;;; brz-parse-case-spec: parse a Breeze case specification (eg. "1,2,3,5..6;7;8;9") into
;;;		a list of lists of elements of the one of the forms:
;;;			(value `val'), (range `low' `high') or (implicant `value' `dcs')
;;;		representing the values which each term covers
(define brz-parse-case-spec (lambda (case-spec)
	(hier-map 2 (lambda (span)
		(cond
			((strchr span #\m) ; VALUEmDCS
				(let ((parsed (parse-string span "m")))
					(list 'implicant
						(string->number (car parsed))
						(string->number (cadr parsed))
					)
				)
			)
			((strchr span #\.) ; VALUE..VALUE
				(let ((parsed (parse-string span "..")))
					(list 'range
						(string->number (car parsed))
						(string->number (cadr parsed))
					)
				)
			)
			(else (list 'value (string->number span)))
		)	
	) (map (lambda (x) (parse-string x #\,)) (parse-string case-spec #\;)))
))

;;; brz-array-spec->width-list: parse a variable array specification string
;;; same format as case specification, except individual values refer to channel indices
;;; rather than values - returns a list of widths of the array
(define brz-array-spec->width-list (lambda (array-spec width elem-count)
	(let*
		((parse-elem (lambda (elem)
			(case (car elem)
				((value)
					(if (cadr elem) 1 width) ;; if false this is an empty declaration (e.g. ;;) and takes full width
				)
				((range)
					(let
						((start (cadr elem))
						 (end (caddr elem))
						)
						(- (max start end) (min start end) -1)
					)
				)
				((implicant)
					(logcount (cadr elem))
				)
			)
		 ))
		 (parsed-list (flatten-unheaded-list (brz-parse-case-spec array-spec)))
		)
		(append
			(map (lambda (elem) (parse-elem elem)) parsed-list)
			(if (= (length parsed-list) elem-count) '() (map.. (lambda (i) width) (length parsed-list) elem-count))
		)
	)
))

;;;;; Component Ports

;;; brz-specify-part-port: resolve parameters in part port defns using `actual' and `formal'
(define brz-specify-part-port (lambda (port actual formal)
	(case (car port) ; port type
		; Pass through un-arrayed sync ports
		((sync-port) port)
		; Specify type of non sync ports
		((port)
			(list
				'port
				(brz-port:name port)
				(brz-port:sense port)
				(brz-port:direction port)
				(brz-specify-type (brz-port:type port) actual formal)
			)
		)
		((arrayed-port) 
			(let*
				((low-index (brz-specify-expression
					(brz-arrayed-port:low-index port) '() actual formal))
				 (port-count (brz-specify-expression
					(brz-arrayed-port:port-count port) '() actual formal))
				)
				(list 
					'arrayed-port
					(brz-arrayed-port:name port)
					(brz-arrayed-port:sense port)
					(brz-arrayed-port:direction port)
					(brz-specify-type (brz-arrayed-port:type port) actual formal)
					low-index
					port-count
				)
			)
		)
		((arrayed-sync-port)
			(let*
				((low-index (brz-specify-expression
					(brz-arrayed-sync-port:low-index port) '() actual formal))
				 (port-count (brz-specify-expression
					(brz-arrayed-sync-port:port-count port) '() actual formal))
				)
				(list 
					'arrayed-sync-port
					(brz-arrayed-sync-port:name port)
					(brz-arrayed-sync-port:sense port)
					low-index
					port-count
				)
			)
		)
	)
))

;;; brz-expand-part-port: expand a port into a list of ports, one for
;;;	each of the indices of that port (ie. 'port -> 'port, 'arrayed-port -> list of 'port)
;;; N.B. NOT Updated to deal with Varaible port arrays - this function is currently only used
;;; in brz-sense-of-component-ports where only the sense of each port is returned and the size
;;; of individual ports is no required - if you wish to use this function elsewhere where size 
;;; is required you will need to update it to deal with variable port arrays
(define brz-expand-part-port (lambda (port)
	(case (car port)
		; Pass through unarrayed ports
		((port sync-port) (list port))
		((arrayed-port)
			(let*
				((low-index (brz-arrayed-port:low-index port))
				 (port-count (brz-arrayed-port:port-count port))
				 (type (brz-arrayed-port:type port))
				 (indices (integer-range-list (+ low-index port-count -1) low-index))
				)
				(map (lambda (index) (list 
					'port
					(brz-arrayed-port:name port)
					(brz-arrayed-port:sense port)
					(brz-arrayed-port:direction port)
					type
				)) indices )
			)
		)
		((arrayed-sync-port)
			(let*
				((low-index (brz-arrayed-sync-port:low-index port))
				 (port-count (brz-arrayed-sync-port:port-count port))
				 (indices (integer-range-list (+ low-index port-count -1) low-index))
				)
				(map (lambda (index) (list 
					'sync-port
					(brz-arrayed-sync-port:name port)
					(brz-arrayed-sync-port:sense port)
				)) indices )
			)
		)
	)
))

;;; brz-specify-part-ports: specify a list of ports
(define brz-specify-part-ports (lambda (ports actual formal)
	(map (lambda (port) (brz-specify-part-port port actual formal)) (cdr ports))
))

;;; brz-expand-and-specify-part-ports: specify then expand a list of ports
;;;		Note that this loses correct indices from expanded arrays
(define brz-expand-and-specify-part-ports (lambda (ports actual formal)
	(append-map! (lambda (port)
		(brz-expand-part-port (brz-specify-part-port port actual formal))
	) (drop-head-symbol 'ports ports))
))

;;; brz-get-primitive-part-name: Returns #f if the given part name is not a primitive (ie.
;;;		begin $Brz, or the remainder of the name with the $Brz prefix removed if it
;;;		is a primitive part name
(define brz-get-primitive-part-name (lambda (part-name)
	(if (and (> (string-length part-name) 4) (string=? (substring part-name 0 4) "$Brz"))
		(substring part-name 4 (string-length part-name))
		#f
	)
))

;;; brz-sense-of-component-ports: Returns a list of senses for each port in a given component
;;;		`context' is a list of breeze-parts which should be considered for matching against component-name
(define brz-sense-of-component-ports (lambda (comp context)
	(case (car comp)
		((component) (let*
			((component-name (brz-component:name comp))
			 (parameters (brz-component:parameters comp))
			 (primitive-part (if (brz-get-primitive-part-name component-name)
				(find-headed-list-elem breeze-primitives 'primitive-part (brz-get-primitive-part-name component-name))
				#f))
			 (part (if primitive-part
				primitive-part
				(find-headed-list-elem context 'breeze-part component-name)))
			)
			(if (not part)
				(begin (error "ERROR: Can't find component `" component-name "'") #f)
				(let
					((expanded-ports (if (eq? (car part) 'primitive-part) ; primitive-part?, expand ports
						(brz-expand-and-specify-part-ports
							(brz-primitive-part:ports part) parameters (brz-primitive-part:parameters part))
						(brz-expand-and-specify-part-ports
							(brz-breeze-part:ports part) '() '(parameters))
					)))
					(map brz-port:sense expanded-ports)
				)
			)
		))
		((undeclared-component) (let
			((expanded-ports (brz-expand-and-specify-part-ports 
				(find-headed-sub-list comp 'ports) '() '(parameters))))
			(map brz-port:sense expanded-ports)
		))
	)
))

;;;;; Component and Channel Decoration

;;; brz-link-channels-and-components: Takes a list of components and a list of part declarations
;;;		which are visible to (or occupy the immediate enclosing namespace of) this part's components.
;;;		Returns a pair consisting of (<channel-vector> <component-vector>) where each channel-vector
;;;		element is a 4tuple of #(pc pp ac ap) (passive port component number, port number,
;;;		active port component number, port number).  The component-vector is just a vectorised version
;;;		of the given components list.  Component numbers are indexed from 0, channels are indexed from
;;;		1 (as is usual in Balsa/balsa-c).  therefore: (vector-ref 0 channel-vector) is the 4tuple for
;;;		channel 1 (activation)
;;;     AB 2009-01-28: Now returns a list of (<channel-vector> <component-vector> <channel-name-to-number-mapping>
;;;			<channel-names> <component-names>)
;;;     With channel-vector and component-vector the same as above. <channel-name-to-number-mapping> can be used
;;;		to map component connection elements to <channel-vector> indices. <channel/component-names> are vectors
;;;		of channel/component names
(define brz-link-channels-and-components (lambda (channels components context)
	(let*
		; Make a four element vector to correspond to channel-array elements: pc pp ac ap
		; (for active/passive component/port indices)
		((channel-count (- (length channels) 1))
		 (channel-connections (make-vector channel-count))
		 (channel-name-to-number-mapping (make-hash-table))
		 (channel-names (make-vector channel-count))
		; Assign channels implicit numbers 1,2,3... (first element (at index 0) is null)
		 (comp-array (list->vector (cdr components))) ; Assign numbers to components too
		 (comp-count (vector-length comp-array))
		 (comp-names (make-vector comp-count))
		 (find-channel-number (lambda (name)
		 	(let ((num (hash-ref channel-name-to-number-mapping (->string-rep name))))
		 		(if num
		 			num
		 			(error "find-channel-number: unrecognised channel " name #\newline)
		 		)
		 	)
		 ))
		)
		(do ((chan-no 0 (+ 1 chan-no))) ((= chan-no channel-count) #f)
			(vector-set! channel-connections chan-no (vector 'no-component 'no-port 'no-component 'no-port)))
		; Build channel names
		(for.. (lambda (chan-index channel)
			(let
				((given-name (find-headed-sub-list channel 'net-name))
				 (chan-no (number->string (+ 1 chan-index)))
				)
				(hash-set! channel-name-to-number-mapping chan-no chan-index)
				(if given-name
					(begin
						(hash-set! channel-name-to-number-mapping (cadr given-name) chan-index)
						(vector-set! channel-names chan-index (cadr given-name))
					)
					(begin
						; (hash-set! channel-name-to-number-mapping (+ 1 chan-no) chan-no)
						(vector-set! channel-names chan-index chan-no)
					)
				)
			)
		) 0 (- channel-count 1) (cdr channels))
		; Build component names
		(for.. (lambda (comp-no comp)
			(let
				((given-name (find-headed-sub-list comp 'name)))
				(vector-set! comp-names comp-no (if given-name (cadr given-name) comp-no))
			)
		) 0 (- comp-count 1) (cdr components))
		; for i in 1 .. comp-count
		(do ((i 0 (+ 1 i))) ((= i comp-count) #f)
			; Need to know which ports of a component are active/passive
			(let*
				((comp (vector-ref comp-array i))
				 (comp-ports (list->vector
				 	(map find-channel-number (flatten-list (brz-component:ports comp)))))
				 (port-senses (list->vector (brz-sense-of-component-ports comp context)))
				 (port-senses-count (vector-length port-senses))
				)
				(do ((port-no 0 (+ 1 port-no))) ((= port-no port-senses-count) #f)
					(let
						((port-sense (vector-ref port-senses port-no))
						 ; (channel (vector-ref channel-connections (- (vector-ref comp-ports port-no) 1)))
						 (channel (vector-ref channel-connections (vector-ref comp-ports port-no)))
						)
						(case port-sense
							((passive)
								(if (eqv? 'no-component (vector-ref channel 0))
									(begin (vector-set! channel 0 i) (vector-set! channel 1 port-no))
									(print "invalid passive channel update "
										port-no " " comp " "
										(vector-ref comp-ports port-no) " "
										channel " " i #\newline)
								)
							)
							((active)
								(if (eqv? 'no-component (vector-ref channel 2))
									(begin (vector-set! channel 2 i) (vector-set! channel 3 port-no))
									(print "invalid active channel update "
										port-no " " comp " "
										(vector-ref comp-ports port-no) " "
										channel " " i #\newline
									)
								)
							)
						)
						#f
					)	
				)
			)
		)
		(list channel-connections comp-array channel-name-to-number-mapping channel-names comp-names)
	)
))

;;; brz-mark-component: change the head symbol of a component from 'component to 'no-component
;;;		marking the component as discard{ed,able}
(define brz-mark-component! (lambda (comp)
	(set-car! comp 'no-component)
	comp
))

;;; brz-component-marked?: is this component marked?
(define brz-component-marked? (lambda (comp)
	(eqv? 'no-component (car comp))
))

;;; brz-channel-tuple-append-flag!: append a symbol flag to a channel's 4tuple info vector.
;;;		Defined flags are:
;;;		'fixed) indicates that the channel should not be removed from a channel list/vector
;;;			even if it is connected to marked components
(define brz-channel-tuple-append-flag! (lambda (channels channel-no flag)
	(vector-set! channels (- channel-no 1)
		(list->vector (append
			(vector->list (vector-ref channels (- channel-no 1)))
			(list flag)
		))
	)
	channels
))

;;; brz-fix-channel!: append the symbol 'fixed to a channel's 4tuple info vector.  This is done
;;;		to indicate that the channel should not be removed even if it is connected to marked
;;;		components
(define brz-fix-channel! (lambda (channels channel-no)
	(brz-channel-tuple-append-flag! channels channel-no 'fixed)
))

;;; brz-component-remove-call-port!: remove the port connected to channel channel-no in the
;;;		Call/CallMux/CallDemux component at index call-comp-index in components.
;;;		If this removes all the arrayed ports from that component then mark the component
(define brz-component-remove-call-port! (lambda (components call-comp-index channel-no)
	(let*
		((call-comp (vector-ref components call-comp-index))
		 (call-comp-name (brz-component:name call-comp))
		 (call-comp-params (brz-component:parameters call-comp))
		 (call-comp-ports (brz-component:ports call-comp))
		 (is-call (string=? "$BrzCall" (cadr call-comp)))
		)
		(if (not (brz-component-marked? call-comp))
			(let*
		 		((trimmed-ports (filter (lambda (x) (not (= x channel-no))) (car call-comp-ports)))
				 (trimmed-port-count (length trimmed-ports))
				)
				(vector-set! components call-comp-index
					(list (if (zero? trimmed-port-count) 'no-component 'component) call-comp-name
						(if is-call ; We have more than one parameter: CallMux or CallDemux
							(list trimmed-port-count) ; call parameters
							(list (car call-comp-params) trimmed-port-count) ; mux/demux parameters
						)
						(list trimmed-ports (cadr call-comp-ports)) ; ports
					)
				)
			)
		)
	)
))

;;; brz-component-remove-encode-port!: remove the port connected to channel channel-no in the
;;;		Encode component at index call-comp-index in components.
;;;		If this removes all the arrayed ports from that component then mark the component
(define brz-component-remove-encode-port! (lambda (components encode-comp-index channel-no)
	(let*
		((encode-comp (vector-ref components encode-comp-index))
		 (encode-comp-name (brz-component:name encode-comp))
		 (encode-comp-params (brz-component:parameters encode-comp))
		 (encode-comp-ports (brz-component:ports encode-comp))
		 (input-port-count (cadr encode-comp-params))
		 (encode-spec (caddr encode-comp-params))
		)
		(if (not (brz-component-marked? encode-comp))
			(let trim-ports
				((ports (car encode-comp-ports))
				 (spec (parse-string encode-spec #\;))
				 (ret-ports '())
				 (ret-spec '())
				)
				(cond
					((null? ports) #f) ; remove nothing
					((= (car ports) channel-no)
						(vector-set! components encode-comp-index
							(list
								(if (= 1 input-port-count) 'no-component 'component)
								encode-comp-name
								(list ; parameters
									(car encode-comp-params)
									(- input-port-count 1)
									(build-separated-string (append! (reverse ret-spec) (cdr spec)) #\;)
								)
								(list ; ports
									(append! (reverse ret-ports) (cdr ports))
									(cadr encode-comp-ports)
								)
							)
						)
					)
					(else (trim-ports (cdr ports) (cdr spec)
						(cons (car ports) ret-ports) (cons (car spec) ret-spec)))
				)
			)
		)
	)
))

;;; brz-component-remove-var-read-port!: remove the port connected to channel channel-no
;;;		in the Variable component at index var-comp-index in the list components
(define brz-component-remove-var-read-port! (lambda (components var-comp-index channel-no)
	(let*
		((var-comp (vector-ref components var-comp-index))
		 (var-comp-name (brz-component:name var-comp))
		 (var-comp-params (brz-component:parameters var-comp))
		 (var-comp-ports (brz-component:ports var-comp))
		 (read-port-count (cadr var-comp-params))
		)
		(if (not (brz-component-marked? var-comp))
			(let
		 		((trimmed-ports (filter (lambda (x) (not (= x channel-no))) (cadr var-comp-ports))))
				(vector-set! components var-comp-index
					(list 'component var-comp-name
						(list (car var-comp-params) (length trimmed-ports)
								(caddr var-comp-params)) ; parameters
						(list (car var-comp-ports) trimmed-ports) ; ports
					)
				)
			)
		)
	)
))

;;; brz-component-remove-false-var-read-port!: like brz-component-remove-var-read-port! but for FalseVariable's
(define brz-component-remove-false-var-read-port! (lambda (components var-comp-index channel-no)
	(let*
		((var-comp (vector-ref components var-comp-index))
		 (var-comp-name (brz-component:name var-comp))
		 (var-comp-params (brz-component:parameters var-comp))
		 (var-comp-ports (brz-component:ports var-comp))
		 (read-port-count (cadr var-comp-params))
		)
		(if (not (brz-component-marked? var-comp))
			(let
		 		((trimmed-ports (filter (lambda (x) (not (= x channel-no))) (caddr var-comp-ports))))
				(vector-set! components var-comp-index
					(list 'component var-comp-name
						(list (car var-comp-params) (length trimmed-ports)) ; parameters
						(list (car var-comp-ports) (cadr var-comp-ports) trimmed-ports) ; ports
					)
				)
			)
		)
	)
))

;;; Dependency analysis

;;; brz-components/instances-dependencies : returns a (p u h) list (as used by
;;;		brz-breeze-part-flat-dependencies below) of dependee parts from a list
;;;		of 'component, 'undeclared-component or 'instances entries, noting that 'instances are always
;;;		(instance "name" (ports) ...) and 'components/undeclared components are always
;;;		({,undeclared-}component "name" (params) (ports) ...)
;;;		`all-parts' is for name lookups.
(define brz-components/instances-dependencies (lambda (components all-parts)
	(foldl-ma (lambda (component parts undeclareds hcs)
		(let ((name (cadr component)))
			(case (car component)
				((component) (let* ; search order is: $Brz == hc, other names: part, ERROR!
					((hc-name (brz-get-primitive-part-name name))
					 (part (if hc-name #f (brz-find-breeze-part all-parts name)))
					)
					(cond 
						(hc-name (let* ; is an HC
							((defn (brz-find-primitive hc-name))
							 (significant-parameters (gen-extract-significant-parameters
								 (cdr (brz-primitive-part:parameters defn))
								 (brz-component:parameters component)))
							)
							(list parts undeclareds (cons (cons hc-name significant-parameters) hcs))
						))
						(part (list (cons part parts) undeclareds hcs))
						(else
							(error "brz-components/instances-dependencies: unknown element `" name "'" #\newline)
							(list parts undeclareds hcs)
						)
					)
					; (cons (merge! string<=? (car parts/hcs) (list name)) (cdr parts/hcs))
				))
				((undeclared-component)
					(list
						parts
						(cons (cons* name (brz-component:parameters component) (brz-component:options component))
							undeclareds)
						hcs
					)
				)
				((instance) (list parts hcs))
			)
		)
	) components '() '() '())
))

;;; brz-breeze-part-flat-dependencies :
;;;		Undeclared Components and Handshake Components with parameters which need to be built in order to build
;;;		the top-level part `part'.  Returns a list (parts undeclareds hcs)
;;;		listing the parts which need to be built.  Each of these elements is a list
;;;		of names apart from the `undeclareds' and `hcs'.  The undeclareds in a list of lists of
;;;		(name parameters . options) where `options' is the trailing port/formal-parameters... list
;;;		present in the undeclared-component call
;;;		The `hcs' which is a list of list of (name . parameters) elements.
;;;		`visited' is a similar list of (p u h) for those parts already visited.
;;;		The `all-parts' list is provided for name lookups.
(define brz-breeze-part-flat-dependencies (lambda (visited all-parts part)
	(let*
		((no-deps '((() () ()) . #f))
		 (part-deps/this-dep (case (car part)
			((breeze-part)
				(if (brz-find-breeze-part (car visited) (brz-breeze-part:name part))
					no-deps
					(cons
						(brz-components/instances-dependencies
							(cdr (brz-breeze-part:components part)) all-parts)
						part
					)
				)
			)
			; FIXME, this shouldn't be necessary
		 ))
		 (part-deps (car part-deps/this-dep))
		 (this-dep (cdr part-deps/this-dep))
		 ; visit-parts : combine visited p/u/h of parts
		 (visit-parts (lambda (parts visited)
			(fold (lambda (part visited)
				(brz-breeze-part-flat-dependencies visited all-parts part)
			) visited parts)
		 ))
		 (visited-children-deps (visit-parts (car part-deps) visited))
 		 (all-visited
 		 	(list
		 		(if this-dep
					(cons this-dep (car visited-children-deps))
					(car visited-children-deps)
		 		)
				(if (null? (cadr part-deps))
					(cadr visited-children-deps)
					(cons (cadr part-deps) (cadr visited-children-deps))
				)
				(if (null? (caddr part-deps))
					(caddr visited-children-deps)
					(cons (caddr part-deps) (caddr visited-children-deps))
				)
			)
		 )
		)
		all-visited
	)
))

;;; brz-style-is-single-rail?: returns true for all single rail styles.  Use this procedure to prevent
;;;		four_b_rb/four_e_e... strings proliferating through the rest of the code
(define brz-style-is-single-rail? (lambda (style)
	(member style tech-single-rail-styles)
))

;;; brz-add-primitive-part-implementation: add an implementation of a primitive part to the
;;;		(already loaded) breeze-primitives.  The given implementation should be a list of the
;;;		form (style ...) and can include 'include directives
(define brz-add-primitive-part-implementation (lambda (part-name implementation)
	(brz-load-primitives)
	(append!
		(find-headed-sub-list (brz-find-primitive part-name) 'implementation)
		(list
			(brz-preprocess-file-contents implementation (brz-tech-build-include-filename #f "abs"))
		)
	)
))

;;; brz-add-primitive-part: add a whole new primitive part
(define brz-add-primitive-part (lambda (part)
	(brz-load-primitives)
	(set! breeze-primitives (cons part breeze-primitives))
))

;;; brz-add-primitive-define: add a new primitive definition.  These are usu. procedures for use in primitive part
;;;		definitions.
(define brz-add-primitive-define (lambda (defn)
	(brz-load-primitives)

	(if (and (headed-list? defn 'define) (= (length defn) 3))
		(set! breeze-primitives-definitions
			(cons 
				(list (cadr defn) (brz-specify-expression (caddr defn) breeze-primitives-definitions '() '()))
				breeze-primitives-definitions
			)
		)
		(error "brz-add-primitive-define: invalid define `" defn "'")
	)
))

;;; brz-print-primitive-parts: pretty print primitive parts (for debugging)
(define brz-print-primitive-parts (lambda ()
	(for-each (lambda (part)
		(write part)
		(newline)
	) breeze-primitives)
))
