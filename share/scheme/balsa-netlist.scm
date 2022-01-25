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
;;;	`balsa-netlist.scm'
;;;	Breeze -> Netlist generator, all manner of netlist functions
;;;

(balsa-scheme-import 'brz)
(balsa-scheme-import 'brz 'tech)
(balsa-scheme-import 'net 'parser)
(balsa-scheme-import 'net 'modify)
(balsa-scheme-import 'gen 'hcs)
(balsa-scheme-import 'misc 'switches)
(balsa-scheme-import 'misc 'banners)
(balsa-scheme-import 'gen 'undeclared)
(balsa-scheme-import 'net 'insert-buffers)

;;; balsa-netlist-{no-banner,...}: command line switches
(define balsa-netlist-no-banner #f)
(define balsa-netlist-force #f)
(define balsa-netlist-verbose #f)
(define balsa-netlist-test-comp #f) ;; Test abs component descriptions
(define balsa-netlist-other-netlists '())
(define balsa-netlist-make-cad-netlist #t)
(define balsa-netlist-read-old-cell-names #t)
(define balsa-netlist-features '((replace-feedthroughs) (emit-prototypes . #f)))
(define balsa-included-cell-types '("helper"))
(define balsa-excluded-cell-names '())
(define balsa-netlist-import-path balsa-search-path)
(define balsa-netlist-file-list-filename #f)
(define balsa-netlist-build-all-parts #f)
(define balsa-netlist-log-stream #f)
(define balsa-netlist-log-file-name #f)
(define balsa-netlist-propagate-globals #t)
(define balsa-netlist-top-level-cell #f)
(define balsa-netlist-tech #f)
(define balsa-netlist-output-file #f)
;;; added for inserting buffers
(define balsa-netlist-insert-buffers #f)
(define balsa-netlist-basename #f)
(define balsa-netlist-with-completions #f)
(define balsa-netlist-with-errors #f)

;;; balsa-netlist-print-banner: pose, pose a bit more
(define balsa-netlist-print-banner (lambda ()
	(make-program-banner "balsa-netlist" "balsa-netlist: Netlist generator"
		"1999-2008, The University of Manchester")
))

;;; balsa-netlist-usage: command usage
(define balsa-netlist-usage (lambda ()
	(balsa-netlist-print-banner)
	(error
		"version " balsa-version #\newline
		"usage: balsa-netlist {<switch>}* <block/file-name>" #\newline #\newline
		"switches: -h or -?    - Display this message (--help)" #\newline
		"          -b          - Don't print the balsa-netlist banner (--no-banner)" #\newline
		"          -v          - Be verbose, print cell names as they are produced (--verbose)" #\newline
		"          -c          - Don't try to make an EDA system native netlist (--no-cad-netlist)" #\newline
		"          -m          - Don't read in old cell name mappings from the .map file" #\newline
		"                        (--no-old-cell-names)" #\newline
		"          -n <format> - Dump a netlist in the given format (verilog, ...)" #\newline
		"                        as well as any other scheduled netlist writes, several -n can be" #\newline
		"                        used (--make-other-netlist)" #\newline
		"                        NB. Name mapping/mangling occurs when the internal netlist is" #\newline
		"                        generated, all of these additional netlists will contain names" #\newline
		"                        mapped to work with the default format" #\newline
		"          -i <type>   - Add cell type <type> to the list of cell types to netlist.  If no" #\newline
		"                        additional cell types are given, then only the netlist" #\newline
		"                        definitions for Balsa cells are emitted (--include-cell-type)" #\newline
		"                        Cell type `helper' is in the included cell type list by default" #\newline
        "          -x <cellname> - Exclude the cell <cellname> from the generated netlist" #\newline
        "                        No definition or prototype will be emitted (--exclude-cell)" #\newline
		"          -I <directory> - Add named directory to the Breeze import path (--import)" #\newline
		"          -t <component> <args> - create test component (--test-component)" #\newline
		"          -l <filename> - Make a list of generated files in file <filename> (--file-list)" #\newline
		"          -a          - Emit definitions for all parts found even if the top level" #\newline
		"                        block doesn't need them (--all-parts)" #\newline
		"          -s          - Insert simulation initialisation code in netlist formats which" #\newline
		"                        support this option (--simulation-initialise)" #\newline
		"          -L <filename> - write a log of balsa-netlist messages to file <filename> (--log)" #\newline
		"          -e <top-level> - Produce encounter compatible netlist (--encounter)" #\newline
        "          -B          - Automatic buffer insertion (--insert-buffers)" #\newline
        "          --verbose-bi - Verbose automatic buffer insertion" #\newline
        "          -X <tech>   - Set `technology/style/style-opts' to use.  (--technology)" #\newline
        "                        Technology will be set by ${BALSATECH} env. var. if -X is not passed" #\newline
        "          --basename  - Output file name prefix for netlists, etc." #\newline
        "          -o          - Output netlist file name (--output-file)" #\newline
		"          --no-replace-feedthroughs - Don't replace feedthrough cells with netlist appropriate" #\newline
		"                        aliases" #\newline
        #\newline
        "deprecated switches [new behaviour]:" #\newline
		"          -g          - Propagate global ports on cells (--propagate-globals) [always on]" #\newline
		"          -d/-p       - Don't/do print prototypes for undefined cells. [never needed] " #\newline
		"                        (--no-prototypes/--prototypes)" #\newline 
		"          -f          - Replace feedthrough cells with netlist appropriate aliases [on by default]" #\newline
		"                        (--replace-feedthroughs)" #\newline
	)
))

;;; balsa-netlist-command-line-rules: command-line-args action rules
(define balsa-netlist-command-line-rules `(
	(#\b "no-banner" 0 ,(lambda (args) (set! balsa-netlist-no-banner #t)))
	(#\n "make-other-netlist" 1 ,(lambda (args) (set! balsa-netlist-other-netlists
		(append balsa-netlist-other-netlists (list (car args))))))
	(#\c "no-cad-netlist" 0 ,(lambda (args) (set! balsa-netlist-make-cad-netlist #f)))
	(#\h "help" 0 ,(lambda (args) (balsa-netlist-usage)))
	(#\? "help" 0 ,(lambda (args) (balsa-netlist-usage)))
	(#\v "verbose" 0 ,(lambda (args) (set! balsa-netlist-verbose #t)))
	(#\m "no-old-cell-names" 0 ,(lambda (args) (set! balsa-netlist-read-old-cell-names #f)))
	(#\i "include-cell-type" 1 ,(lambda (args) (set! balsa-included-cell-types
		(cons (car args) balsa-included-cell-types))))
	(#\x "exclude-cell" 1 ,(lambda (args) (set! balsa-excluded-cell-names
		(cons (car args) balsa-excluded-cell-names))))
	(#\I "import" 1 ,(lambda (args) (set! balsa-netlist-import-path (append balsa-netlist-import-path (list (car args))))))
	(#\t "test-component" 0 ,(lambda (args) (set! balsa-netlist-test-comp #t)))
	(#\a "all-parts" 0 ,(lambda (args) (set! balsa-netlist-build-all-parts #t)))
	(#\l "file-list" 1 ,(lambda (args) (set! balsa-netlist-file-list-filename (car args))))
	(#\s "simulation-initialise" 0 ,(lambda (args) (set! balsa-netlist-features (cons
		'(simulation-initialise) balsa-netlist-features))))
	(#\f "replace-feedthroughs" 0 ,(lambda (args) #f))
	(#\L "log" 1 ,(lambda (args)
		(set! balsa-netlist-log-file-name (car args))
		(set! balsa-netlist-log-stream (open-output-file balsa-netlist-log-file-name))
	))
	(#\e "encounter" 1 ,(lambda (args) (set! balsa-netlist-top-level-cell (car args))  (set! balsa-netlist-features (cons
		`(top-level-cell ,(car args)) balsa-netlist-features)))) ;; feature of netlists - allows for handling of global gnd/vcc
	(#\_ "no-replace-feedthroughs" 0 ,(lambda (args)
		(set! balsa-netlist-features
			(filter (lambda (feature) (not (headed-list? feature 'replace-feedthroughs))) balsa-netlist-features))
		(print "FEATURES " balsa-netlist-features)
	))
    (#\B "insert-buffers" 0 ,(lambda (args) (set! balsa-netlist-insert-buffers #t)))
    (#\_ "verbose-bi" 0 ,(lambda (args) (set! balsa-netlist-insert-buffers #t) (set! balsa-insert-b-verbose #t)))
    (#\X "technology" 1 ,(lambda (args) (set! balsa-netlist-tech (car args))))
	;; Deprecated
	(#\d "no-prototypes" 0 ,(lambda (args) #f))
	(#\p "prototypes" 0 ,(lambda (args) #f))
	(#\g "propagate-globals" 0 ,(lambda (args) #f))
	(#\o "output-file" 1 ,(lambda (args) (set! balsa-netlist-output-file (car args))))
	(#\_ "basename" 1 ,(lambda (args) (set! balsa-netlist-basename (car args))))
	(#\_ "with-completions" 0 ,(lambda (args) (set! balsa-netlist-with-completions #t)))
	(#\_ "with-errors" 0 ,(lambda (args) (set! balsa-netlist-with-errors #t)))
))

;;; balsa-netlist-parse-command-line: parse switches from the given command line list, set
;;;		the balsa-netlist-... globals and return the tail of the command line.
(define balsa-netlist-parse-command-line (lambda (args)
	(if (null? args)
		(balsa-netlist-usage)
		(let
			((args-tail (parse-command-line "balsa-netlist"
				balsa-netlist-command-line-rules balsa-netlist-usage args))
			)
			(if (and (/= 1 (length args-tail)) (not balsa-netlist-test-comp))
			(balsa-netlist-usage)
			args-tail
			)
		)
	)
))

(define note (lambda args
	(if balsa-netlist-log-stream (apply print-to-port balsa-netlist-log-stream args))
	(apply print args)
))

;;; backslash-escape-string : replace NL in string with \n
(define backslash-escape-string (lambda (string)
	(list->string (reverse! (fold (lambda (char ret)
		(if (char=? char #\nl)
			(cons #\n (cons #\\ ret))
			(cons char ret)
		)
	) '() (string->list string))))
))

(define pretty-hc-parameters (lambda (params)
	(build-separated-string (map (lambda (param)
		(cond
			((number? param) (number->string param))
			((symbol? param) (symbol->string param))
			((string? param)
				(string-append "\"" (backslash-escape-string param) "\"")
			)
			(else (->string param))
		)
	) params) " ")
))

;;; balsa-netlist-normalise-features: remove repeated elements from the feature list
(define balsa-netlist-normalise-features (lambda ()
	(set! balsa-netlist-features
		(cons*
			(cons 'included-cell-types balsa-included-cell-types)
			(cons 'excluded-cell-names balsa-excluded-cell-names)
			(unsorted-uniq (lambda (a b) (eq? (car a) (car b))) balsa-netlist-features)
		)
	)
))

(define balsa-netlist-write-file-list-file (lambda ()
	(note "writing file list to file: `" balsa-netlist-file-list-filename "'" #\newline)
	(with-output-to-file balsa-netlist-file-list-filename (lambda ()
		(for-each (lambda (filename)
			(print filename #\newline)
		) net-session-file-list)
		(if balsa-netlist-log-stream
			(print balsa-netlist-log-file-name #\newline)
		)
		;;; (print balsa-netlist-file-list-filename #\newline)
	))
))

;;; balsa-netlist-convert: convert a .net netlist into the EDA native format
(define balsa-netlist-convert (lambda (filename)
	(balsa-set-tech balsa-netlist-tech)
	(brz-load-gates)
	(let*
		((base-filename (let ((root (final-string filename ".net"))) (if root root filename)))
		 (netlist (get-file filename))
		 (top-level-block (car (last-pair netlist)))
		 (netlist-filename (if balsa-netlist-output-file
		 	balsa-netlist-output-file
		 	(string-append base-filename "." tech-filename-suffix)
		 ))
		)
		(note "using technology: `" (brz-pretty-format-tech-name) "'" #\newline)
		(note "processing Balsa netlist: `" filename "'" #\newline)
		(note #\newline)

		(note "making netlists" #\newline)

		(balsa-netlist-normalise-features)

		(if balsa-netlist-make-cad-netlist (begin
			(note "writing EDA native netlist to file: `" netlist-filename "'" #\newline)
			(apply tech-write-netlist-file (cons* netlist netlist
				(net-circuit-decl:name top-level-block) netlist-filename balsa-netlist-features))
		))

		(if (not (null? balsa-netlist-other-netlists))
			(for-each (lambda (format)
				(let*
					((signature (net-signature-for-netlist-format format #f))
					 (filename (string-append base-filename "." (net-signature:filename-suffix signature)))
					)
					(note "writing " format " netlist to file: `" filename "'" #\newline)
					(apply (net-signature:write-netlist-file signature)
						(cons* netlist top-level-block (net-circuit-decl:name top-level-block) filename
						balsa-netlist-features))
				)
			) balsa-netlist-other-netlists)
		)

		(note "finished writing netlist" #\newline)
		(if balsa-netlist-file-list-filename (balsa-netlist-write-file-list-file))
		(if balsa-netlist-log-stream (close-port balsa-netlist-log-stream))
	)
))

;;; convert-test-param: converts input test params into relevent type.
(define convert-test-param (lambda (a-param f-param)
	(cond 
		((/= (length (cadr f-param)) 1) ;; If not string
			(let*
				((param-type-dec (cadr f-param)) ;;parameter-type-description
				(param-type (car param-type-dec))) ;; actual type
				(cond
					((eqv? param-type 'numeric-type) (string->number a-param))
					((eqv? param-type 'other-type) ; added 2008-07-07 AB
						(let*
							((string-port (open-input-string a-param))
							 (parsed-param (read string-port))
							)
							(close-port string-port)
							parsed-param
						)
					)
					((eqv? param-type 'named-type) 
						(let*
							((param-type-name (cadr param-type-dec))
							(get-type-decl (lambda (elem) (string=? param-type-name (brz-type-decl:name elem))))
							(type-decl (find-with-predicate brz-type-context get-type-decl))
							(type-type (car (brz-type-decl:body type-decl))))
							(if (string=? "boolean" param-type-name)
								a-param ; removed symbol conv. 2002-05-06 AB
								(cond
									((eqv? type-type 'numeric-type) (string->number a-param))
									((eqv? type-type 'enumeration-type) a-param)
									(else a-param) ; removed symbol conv. 2002-05-06 AB
								)
							)
						)
					)
				)
			)
		)
		(else a-param)
	)
))

;;; balsa-netlist-test: generate netlists for the given HC Component
(define balsa-netlist-test (lambda (args)
	(balsa-set-tech balsa-netlist-tech)
	(brz-load-gates)
	(brz-load-primitives)
	(let*
		((part-name (car args))
		 (part-params (cdr args))
		 (part-primitive (brz-find-primitive part-name))
		 (formal-parameters
			(let*
				((fps (cdr (brz-primitive-part:parameters part-primitive)))
				 (is-significant-formal-param? (lambda (param) (not (memv 'not-used (cddr param)))))
				)
				(if (/= (length fps) (length part-params))
					(filter is-significant-formal-param? fps)
					fps
				)
		 	)
		 )
		 (param-list (map convert-test-param part-params formal-parameters))
		 (netlist (gen-make-hc-netlist part-name param-list brz-type-context))
		 (defn (append breeze-gate-defns netlist))
		 (globals (if balsa-netlist-propagate-globals
		 	(begin
				(note "propagating global signals" #\newline)
				(balsa-netlist-add-globals! defn)
			)
		 ))
		 (rails (if balsa-netlist-top-level-cell
			(begin
				(note "propagating global rails" #\newline)
			 	(balsa-netlist-add-global-rails! defn)
			)
		 ))
		 (netlist-filename (if balsa-netlist-output-file
		 	balsa-netlist-output-file
		 	(string-append part-name "." tech-filename-suffix)
		 ))
		 (celllist-filename (string-append part-name ".lst"))
		 (symbol-filename (if tech-write-symbol-file (string-append part-name "." tech-symbol-filename-suffix) #f))
		 (cell-name-mapping-file (string-append breeze-tech ".map"))
		)
		(note "using technology: `" (brz-pretty-format-tech-name) "'" #\newline)
		(note "processing component: `" part-name "'")
		(note " (" (pretty-hc-parameters param-list) ")" #\newline)
		(if (and balsa-netlist-read-old-cell-names (file-exists? cell-name-mapping-file) tech-map-cell-name-import)
			(begin
				(note "reading cell name mapping file: `" cell-name-mapping-file "'" #\newline)
				(tech-map-cell-name-import cell-name-mapping-file)
			)
		)
		
		(balsa-netlist-normalise-features)

		(if balsa-netlist-make-cad-netlist (begin
			(note "writing EDA native netlist to file: `" netlist-filename "'" #\newline)
			(apply tech-write-netlist-file
				(cons* defn netlist #f netlist-filename balsa-netlist-features))
		))

		(if (not (null? balsa-netlist-other-netlists))
			(for-each (lambda (format)
				(let*
					((signature (net-signature-for-netlist-format format #f))
					 (filename (string-append part-name "." (net-signature:filename-suffix signature)))
					)
					(note "writing " format " netlist to file: `" filename "'" #\newline)
					(apply (net-signature:write-netlist-file signature)
						(cons* defn netlist #f filename balsa-netlist-features))
				)
			) balsa-netlist-other-netlists)
		)

		(if symbol-filename (begin
			(note "writing symbol description file: `" symbol-filename "'" #\newline)
			(verilog-write-pin-file netlist symbol-filename)
		))

		(note "writing cell list to file: `" celllist-filename "'" #\newline)
		(net-add-file-to-session-file-list celllist-filename)
		(with-output-to-file celllist-filename (lambda ()
			(for-each (lambda (cell)
				(print (net-circuit-decl:name cell) #\newline)
			) netlist)
		))
		(note "finished writing netlist" #\newline)
		
		(if tech-map-cell-name-export
			(begin
				(note "writing cell name mapping file: `" cell-name-mapping-file "'" #\newline)
				(tech-map-cell-name-export cell-name-mapping-file)
			)
		)
	)
))

;;; balsa-netlist-generate: generate netlists for the given Balsa module
(define balsa-netlist-generate (lambda (filename)
	(balsa-set-tech balsa-netlist-tech)
	(brz-load-gates)
	(brz-load-primitives)
	(let*
		; "my_proc" =eg=> "Balsamy_proc"
		(;(mangled-proc-name (string-append tech-balsa-prefix (tech-map-name proc-name)))
		 ; top-level/imported-blocks/imported-decls/visited-blocks
		 (path/name/ext (find-filename filename "breeze" #f balsa-netlist-import-path))
		 (flat-path (apply string-append path/name/ext))
		 (t/i/i-d/v (begin
		 	(note "loading file: `" flat-path "'" #\newline)
		 	(get-flattened-block flat-path "breeze" '() balsa-netlist-import-path)
		 ))
		 (top-level-decls (car t/i/i-d/v))
		 (import-blocks (cadr t/i/i-d/v))
		 (imported-decls (caddr t/i/i-d/v))
		 (visited-blocks (cadddr t/i/i-d/v))
		 ; is-breeze-part?: is the given decl a breeze-part
		 (is-breeze-part? (lambda (decl) (headed-list? decl 'breeze-part)))
		 (local-parts (filter is-breeze-part? top-level-decls))
		 (imported-parts (filter is-breeze-part? imported-decls))
		 (all-parts (append local-parts imported-parts))
		 ; flat-dependencies: dependency pair (parts-in-order . hcs) for all parts in local scope
		 (flat-dependencies (begin
		 	(note "determining dependencies" #\newline)
		 	(fold (lambda (part visited)
				(brz-breeze-part-flat-dependencies visited all-parts part)
			) '(() () ()) (if balsa-netlist-build-all-parts all-parts local-parts))
		 ))
		 (parts-to-build (reverse (car flat-dependencies)))
		 (not-equal? (lambda (a b) (not (equal? a b))))
		 (undeclareds-to-build
		 	(uniq equal? (merge-sort! object<=?
				(concatenate! (list-ref flat-dependencies 1))))
		 )
		 (hcs-to-build
		 	(uniq equal? (merge-sort! object<=?
		 		(concatenate! (list-ref flat-dependencies 2))))
		 )
		 (is-type-decl? (lambda (decl) (headed-list? decl 'type)))
		 (all-types (append (filter is-type-decl? top-level-decls) (filter is-type-decl? imported-decls)))
		 (base-name (if balsa-netlist-basename balsa-netlist-basename (cadr path/name/ext)))
		 (netlist-filename (if balsa-netlist-output-file
		 	balsa-netlist-output-file
		 	(string-append base-name "." tech-filename-suffix)
		 ))
		 (celllist-filename (string-append base-name ".lst"))
		 (symbol-filename (if tech-write-symbol-file (string-append base-name "." tech-symbol-filename-suffix) #f))
		 (cell-name-mapping-file (string-append breeze-tech ".map"))
		 ; report-required-parts : pretty print function to report parts to be built
		 (report-required-parts (lambda (name find-procedure parts)
			(if (not (null? parts)) (begin
				(note "required " name " (in order, [top-level]):" #\newline)
				(for-each (lambda (part)
					(let ((part-name (cadr part)))
						(if (find-procedure local-parts part-name)
							(note "[" part-name "]")
							(note " " part-name)
						)
						(note #\newline)
					)
				) parts)
				(note #\newline)
			))
		 ))
		)

		(note "using technology: `" (brz-pretty-format-tech-name) "'" #\newline)
		(note "processing file/block: `" filename "'" #\newline)
		(report-required-parts "Breeze parts" brz-find-breeze-part parts-to-build)
		(note "making these HCs:")
		(fold (lambda (hc last-hc-type)
			(if (not (string=? last-hc-type (car hc)))
				(note #\newline (car hc) ":")
			)
			(note " (" (pretty-hc-parameters (cdr hc)) ")")
			(car hc)
		) "" hcs-to-build)
		(note #\newline "finished composing HCs" #\newline)

		(if (not (null? undeclareds-to-build)) (begin
			(note #\newline "making these parameterised builtin functions:" #\newline)
			(for-each (lambda (undeclared)
				(if (not (gen-undeclared-is-builtin-function? (cddr undeclared)))
					(error "balsa-netlist: undeclared-component `" (car undeclared)
						"' must implement builtin-functions" #\newline)
					(print (car undeclared) " (" (pretty-hc-parameters (cadr undeclared)) ")" #\newline)
				)
			) undeclareds-to-build)
			(note #\newline)
		))

		(if (and balsa-netlist-read-old-cell-names (file-exists? cell-name-mapping-file) tech-map-cell-name-import)
			(begin
				(note "reading cell name mapping file: `" cell-name-mapping-file "'" #\newline)
				(tech-map-cell-name-import cell-name-mapping-file)
			)
		)
		(note #\newline)
		(let*
			; make-netlist-types : do fancy progress bar netlist construction
			((make-netlist-types (lambda (name sources source-name-proc build-proc)
				(if (null? sources)
					(begin (note "No " name " netlists to build" #\newline) '())
					(begin
						(note "making " name " netlists (" (length sources) " to build)" #\newline)
						(let ((netlists (foldl-with-progress-bar (lambda (acc elem) (append! acc (build-proc elem)))
							(if balsa-netlist-verbose source-name-proc #f) note '() sources)))
							(note #\newline)
							(if (not balsa-netlist-verbose) (note #\newline))
							netlists
						)
					)
				)
			 ))
			 (hc-netlists (make-netlist-types "HC" hcs-to-build
				(lambda (hc) (string-append (car hc) ": " (->string (cdr hc))))
				(lambda (hc) (gen-make-hc-netlist
					(car hc) (cdr hc) brz-type-context))
			 ))
			 (undeclared-netlists (make-netlist-types "undeclared component" undeclareds-to-build
				(lambda (undeclared) (string-append (car undeclared) ": " (pretty-hc-parameters (cadr undeclared))))
				(lambda (undeclared) (gen-make-undeclared-component-netlist
					(car undeclared) (cadr undeclared) (cddr undeclared) all-types))
			 ))
			 (part-netlists (make-netlist-types "Breeze part" parts-to-build brz-breeze-part:name
				(lambda (part) (list
					(if (find-headed-sub-list (brz-breeze-part:attributes part) 'is-builtin-function)
						(gen-make-builtin-function-part-netlist part all-types)
						(gen-make-breeze-part-netlist part all-types ;; buffer top-level ports
							(and balsa-netlist-top-level-cell
								(string=? balsa-netlist-top-level-cell (brz-breeze-part:name part)))
							balsa-netlist-with-completions
							balsa-netlist-with-errors
						)
					)
				))
			 ))
			 (final-netlist (append hc-netlists
			 	undeclared-netlists
			 	part-netlists
			 ))
             (all-defns (if balsa-netlist-insert-buffers
             	(append breeze-gate-defns (net-insert-buffers final-netlist))
                (append breeze-gate-defns final-netlist)
             ))
			)
			(if balsa-netlist-propagate-globals
				(let
					((global-signals (balsa-netlist-add-globals! all-defns))
					)
					(begin
			 			(note "propagating global signals" #\newline)
			 			(set! balsa-netlist-features (cons (cons 'global-nets global-signals) balsa-netlist-features))
			 		)
				)
			)
			(if balsa-netlist-top-level-cell
				(begin
			 		(note "propagating global rails" #\newline)
			 		(balsa-netlist-add-global-rails! all-defns)
			 	)
			)
			(balsa-netlist-normalise-features)

			(if (not (null? balsa-netlist-other-netlists))
				(for-each (lambda (format)
					(let*
						((signature (net-signature-for-netlist-format format #f))
					 	 (filename (string-append base-name "." (net-signature:filename-suffix signature)))
						)
						(note "writing " format " netlist to file: `" filename "'" #\newline)
						(apply (net-signature:write-netlist-file signature)
							(cons* all-defns final-netlist #f filename balsa-netlist-features))
					)
				) balsa-netlist-other-netlists)
			)

			(if balsa-netlist-make-cad-netlist (begin
				(note "writing EDA native netlist to file: `" netlist-filename "'" #\newline)
				(apply tech-write-netlist-file
					(cons* all-defns final-netlist #f netlist-filename balsa-netlist-features))
			))

			(if symbol-filename (begin
				(note "writing symbol description file: `" symbol-filename "'" #\newline)
				(verilog-write-pin-file final-netlist symbol-filename)
			))

			(note "writing cell list to file: `" celllist-filename "'" #\newline)
			(net-add-file-to-session-file-list celllist-filename)
			(with-output-to-file celllist-filename (lambda ()
				(for-each (lambda (cell)
					(print (net-circuit-decl:name cell) #\newline)
				) final-netlist)
			))
			(note "finished writing netlist" #\newline)
		)
		(if tech-map-cell-name-export
			(begin
				(note "writing cell name mapping file: `" cell-name-mapping-file "'" #\newline)
				(net-add-file-to-session-file-list cell-name-mapping-file)
				(tech-map-cell-name-export cell-name-mapping-file)
			)
		)
		(if balsa-netlist-file-list-filename (balsa-netlist-write-file-list-file))
		(if balsa-netlist-log-stream (close-port balsa-netlist-log-stream))
	)
))

(top-level (lambda (args)
	(let
		((args-tail (balsa-netlist-parse-command-line args)))
		(if (not balsa-netlist-no-banner)
			(balsa-netlist-print-banner)
		)
		(set! net-session-file-list '())
		(cond
			((null? args-tail) (balsa-netlist-usage))
			(balsa-netlist-test-comp (balsa-netlist-test args-tail))
			((final-string (car args-tail) ".net") (apply balsa-netlist-convert args-tail))
			(else (apply balsa-netlist-generate args-tail))
		)
	)
) (cdr command-line-args))
