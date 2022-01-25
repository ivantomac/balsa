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
;;;	`breeze2ps.scm'
;;;	Breeze -> Postscript HC graph generator
;;;

(balsa-scheme-import 'brz)
(balsa-scheme-import 'misc 'switches)
(balsa-scheme-import 'misc 'banners)

(define dot-cluster-name-to-style (lambda (cluster)
	(cond
		((initial-string cluster ":") => (lambda (bare-name) (cons "dotted" bare-name)))
		((initial-string cluster "_") => (lambda (bare-name) (cons "solid" bare-name)))
		((initial-string cluster "=") => (lambda (bare-name) (cons "dashed" bare-name)))
		(else (cons "invisible" cluster))
	)
))

;;; dot-print-ports: print dot(1) nodes for each of the ports in `ports'.
(define dot-print-ports (lambda (ports name chan-name-mapping)
	(let*
		((port-count (length ports))
		 (port-indices (.. 0 (- port-count 1)))
		 (name-matches (lambda (cluster) (string=? (car cluster) name)))
		 (clusters (map cdr (filter name-matches breeze2ps-port-clusters)))
		 (port-no-to-cluster (make-vector port-count ""))
		)
		(for-each (lambda (port port-no)
			(let ((cluster (find-headed-sub-list port 'cluster)))
				(if cluster (vector-set! port-no-to-cluster port-no (cadr cluster)))
			)
		) ports port-indices)
		(for-each (lambda (cluster)
			(for-each (lambda (port-name)
				(let
					((port-no (find-index-with-predicate ports
						(lambda (port) (string=? (brz-port:name port) port-name))
						0)))
					(vector-set! port-no-to-cluster port-no (car cluster))
				)
			) (cdr cluster))
		) clusters)
		; (if breeze2ps-box (print "subgraph ports {" #\newline))
		(fold (lambda (port cluster index)
			(let*
				((bare-name/style (dot-cluster-name-to-style cluster))
				 (bare-name (cdr bare-name/style))
				 (style (car bare-name/style))
				 (in-cluster (not (string=? bare-name "")))
				 ; (port-nos (cdr cluster))
				 (port-channel-index (let
				 	((port-channel (find-headed-sub-list port 'port-channel)))
				 	(if port-channel (hash-ref chan-name-mapping (cadr port-channel)) index)
				 ))
				)
				(if in-cluster
					(begin
						(print "subgraph cluster_" bare-name " {" #\newline)
						(print "label=\"" bare-name "\"" #\newline)
						(print "style=" style #\newline)
					)
				)
				(let
					((port-count (case (car port)
						((sync-port port)
							(print "port" (+ 1 port-channel-index) " [label=\"" (brz-port:name port)
								"\" " dot-port-style " ]" #\newline)
							1
						)
						((arrayed-sync-port)
							(let
								((low-index (brz-arrayed-sync-port:low-index port))
								 (port-count (brz-arrayed-sync-port:port-count port))
								 (port-name (brz-arrayed-sync-port:name port))
								)
								(for.. (lambda (i)
									(print "port" (+ 1 port-channel-index (- i low-index)) " [label=\"" port-name
										"[" (number->string i) "]\" " dot-port-style " ]" #\newline)
								) low-index (+ low-index port-count -1))
								port-count
							)
						)
						((arrayed-port)
							(let
								((low-index (brz-arrayed-port:low-index port))
								 (port-count (brz-arrayed-port:port-count port))
								 (port-name (brz-arrayed-port:name port))
								)
								(for.. (lambda (i)
									(print "port" (+ 1 port-channel-index (- i low-index)) " [label=\"" port-name
										"[" (number->string i) "]\" " dot-port-style " ]" #\newline)
								) low-index (+ low-index port-count -1))
								port-count
							)
						)
					)))
					(if in-cluster (print "}" #\newline))
					(+ index port-count)
				)
			)
		) 0 ports (vector->list port-no-to-cluster))
	)
))

;;; brz-specify-case-expr: evaluate a case expression called with value `switch-eval'
;;;		where `cases' is a list of (matches expr) pairs (or ('else expr)) and `eval-proc'
;;;		is a procedure capable of evaluating a sub-expression.  Returns #f if no other
;;;		option presents itself.
(define brz-specify-case-expr (lambda (switch-val cases eval-proc)
	(cond
		((null? cases) #f)
		((headed-list? (car cases) 'else) (eval-proc (cadar cases)))
		(else (let
			((match-values (map eval-proc (caar cases))))
			(if (member switch-val match-values)
				(eval-proc (cadar cases))
				(brz-specify-case-expr switch-val (cdr cases) eval-proc)
			)
		))
	)
))

;;; brz-specify-symbol-expr: specify the expressions used in the .abs files to
;;;		define the symbol used in the centre of the HCs on a printout.  This
;;;		procedure just returns a string.
(define brz-specify-symbol-expr (lambda (expr actual formal)
	; (let ((specify-expr (lambda (expr) (brz-specify-expression expr '() actual formal))))
	(let ((specify-expr (lambda (expr) (brz-specify-expression expr breeze-primitives-definitions actual formal))))
		(cond
			((headed-list? expr)
				(case (car expr)
					((centre-string)
						(fold (lambda (arg str)
							(let ((specified-arg (specify-expr arg)))
								(string-append str
									(if (string? specified-arg)
										specified-arg
										(->string specified-arg)
									)
								)
							)
						) "" (cdr expr))
					)
					(else "NO NAME")
				)
			)
			(else "")
		)
	)
))

(define dot-component-label (lambda (comp)
	(let ((primitive-part-name (brz-get-primitive-part-name (brz-component:name comp))))
		(if primitive-part-name
			(let ((part (brz-find-primitive primitive-part-name)))
				(brz-specify-symbol-expr (cadr (brz-primitive-part:symbol part))
					(brz-component:parameters comp) (brz-primitive-part:parameters part))
			)
			(brz-component:name comp)
		)
	)
))

(define dot-component-style (lambda (comp)
	(let
		((primitive-part-name (brz-get-primitive-part-name (brz-component:name comp))))
		(if primitive-part-name
			(let*
				((part (brz-find-primitive primitive-part-name))
				 (specify-expr (lambda (expr) (brz-specify-expression expr breeze-primitives-definitions
					(brz-component:parameters comp) (brz-primitive-part:parameters part))))
				 (shape (if part (find-headed-sub-list part 'shape) #f))
				 ; (aa (print "SHAPE " shape "\n"))
				 (shape-length (if shape (- (length shape) 1) 0))
				 ; (bb (print "/SHAPE\n"))
				)
				(if shape
					(string-append
						" shape=\"" (specify-expr (cadr shape)) "\" "
						(if (> shape-length 1)
							(build-separated-string (map (lambda (prop)
								(string-append (symbol->string (car prop)) "=" (->string
									(specify-expr (cadr prop)))))
								(cddr shape)) " ")
							""
						)
					)
					#f
				)
			)
			(if tech-dot-other-node-style
				tech-dot-other-node-style
				#f
			)
		)
	)
))

(define dot-print-components (lambda (name comps label comp-names)
	(if breeze2ps-box
		(begin
			(print "subgraph cluster_components {" #\newline)
			(print " label=\"" label "\"" #\newline)
		)
	)
	(let*
		((comp-count (length comps))
		 (comp-indices (.. 0 (- comp-count 1)))
		 (name-matches (lambda (cluster) (string=? (car cluster) name)))
		 (clusters (map cdr (filter name-matches breeze2ps-comp-clusters)))
		 (labels (apply append (map cdr (filter name-matches breeze2ps-comp-labels))))
		 (other-cluster (if breeze2ps-cluster-other-comps breeze2ps-cluster-other-comps ""))
		 (comp-no-to-cluster (make-vector comp-count other-cluster))
		)
		(for-each (lambda (comp comp-no)
			(let ((cluster (find-headed-sub-list comp 'cluster)))
				(if cluster (vector-set! comp-no-to-cluster comp-no (cadr cluster)))
			)
		) comps comp-indices)
		(for-each (lambda (cluster)
			(for-each (lambda (comp-no)
				(vector-set! comp-no-to-cluster comp-no (car cluster))
			) (cdr cluster))
		) clusters)
		(for-each (lambda (comp cluster index)
			(let*
				((bare-name/style (dot-cluster-name-to-style cluster))
				 (bare-name (cdr bare-name/style))
				 (style (car bare-name/style))
				 (in-cluster (not (string=? cluster "")))
				 (label (assoc index labels))
				 (label-is-inside (if label (initial-string (cdr label) "<") ""))
				 (label-is-outside (and label (not label-is-inside)))
				)
				(if in-cluster
					(begin
						(print "subgraph cluster_" bare-name " {" #\newline)
						(print "label=\"" bare-name "\"" #\newline)
						(print "style=" style #\newline)
					)
				)
				(if label-is-outside
					(let
						((label-colour "grey40"))
						(print "subgraph cluster_comp" index " { " #\newline)
						(print "style=invisible" #\newline "rank=same" #\newline)
						; (print "rankdir=LR" #\newline)
						(print "compLabel" index " [label=\"" (cdr label) "\" shape=box " ; fixedsize=true"
							" width=0.25 height=0.25 fontcolor=" label-colour " fontsize=12 pencolor=" label-colour
							" color=" label-colour "]" #\newline)
						; (print "comp" index ":e -> compLabel" index ":w [arrowhead=vee color=" label-colour
						(print "comp" index ":e -> compLabel" index ":w [dirType=back arrowtail=vee arrowhead=none "
							"color=" label-colour " constraint=false]" #\newline)
					)
				)

				(let*
					((label (dot-component-label comp))
					 ; (index-string (number->string index))
					 (index-string (->string-rep (vector-ref comp-names index)))
					 (comp-no-label (if breeze2ps-show-comp-nos
						(string-append "(" index-string ")")
						""
					 ))
					 (vert-record-tail (initial-string label "{<"))
					 (inside-label (if label-is-inside label-is-inside ""))
					 (full-label ; (if comp-no-label
					 	(if (and vert-record-tail (not (string=? comp-no-label "")) (string=? inside-label))
							(string-append comp-no-label inside-label "|{<" vert-record-tail)
							(string-append comp-no-label inside-label label)
						)
					 	; label
					 ); )
					)
					(print "comp" index " [label=\"" full-label "\"")
				)
				(let ((style (dot-component-style comp)))
					(if style (print style))
				)
				(print "]" #\newline)

				(if label-is-outside (print "}" #\newline))

				(if in-cluster (print "}" #\newline))
			)
		) comps (vector->list comp-no-to-cluster) comp-indices)
	)
	(if breeze2ps-box (print "}" #\newline))
))

(define dot-arc-label (lambda (chan-no chan chan-links comps channel-names)
	(string-append
		(if breeze2ps-chan-nos (vector-ref channel-names (- chan-no 1)) ; (string-append "C" (number->string chan-no) ": ")
			(brz-channel:name chan)))
))

(define dot-arc-end-port-no (lambda (passiveNactive chan-no chan chan-links comps)
	(let
		((comp-offset (if passiveNactive 0 2)))
		(if (eq? 'no-component (vector-ref chan-links comp-offset))
			#f
			(vector-ref chan-links (+ 1 comp-offset))
		)
	)
))

(define dot-arc-end-label (lambda (passiveNactive chan-no chan chan-links comps)
	(let
		((comp-offset (if passiveNactive 0 2)))
		(if (eq? 'no-component (vector-ref chan-links comp-offset))
			""
			(number->string (dot-arc-end-port-no passiveNactive chan-no chan chan-links comps))
		)
	)
))

(define dot-colours #("black" "brown" "red" "orange" "goldenrod" "green" "blue" "violet" "grey" "white"))

(define dot-make-default-edge-style (lambda (sense width)
	(let
		((arrow-style (case sense
			((sync) "arrowhead=odot arrowtail=dot dir=forward")
			((push) "arrowhead=normal arrowtail=dot dir=forward")
			((pull) "arrowhead=odot arrowtail=normal dir=back")
		 ))
		)
		(if width
			(let*
				((log-width (bit-length (if (eqv? sense 'sync) 0 width)))
				 (colour (vector-ref dot-colours (if (> log-width 8) 8 log-width)))
				)
				(string-append arrow-style " color=" colour)
			)
			arrow-style
		)
	)
))

; find-struct-port-index: find the (port-index . port-sub-index) for a given flat port-index
(define find-struct-port-index (lambda (comp-ports flat-port-index)
(let body
	((comp-ports comp-ports)
	 (flat-port-index flat-port-index)
	 (port-index 0)
	)
	(if (null? comp-ports)
		#f
		(if (pair? (car comp-ports))
			(if (< flat-port-index (length (car comp-ports)))
				(cons port-index flat-port-index)
				(body (cdr comp-ports) (- flat-port-index (length (car comp-ports))) (+ 1 port-index))
			)
			(if (= flat-port-index 0)
				(cons port-index #f)
				(body (cdr comp-ports) (- flat-port-index 1) (+ 1 port-index))
			)
		)
	)
)
))

(define dot-find-primitive (lambda (name)
	(let ((primitive-part-name (brz-get-primitive-part-name (brz-component:name name))))
		(if primitive-part-name
			(brz-find-primitive primitive-part-name)
			#f
		)
	)
))

;;; dot-comp-port : build a component name for passive/active connection to chan-no.  This can
;;;		include a port name if one was specified with (dot-port "name") in the component's primitive
;;;		description.  This can be used for drawing components with "record" shape.
(define dot-comp-port (lambda (passiveNactive chan-no chan-links comps)
	(let*
		((comp-index (vector-ref chan-links (if passiveNactive 0 2)))
		 (port-index (vector-ref chan-links (if passiveNactive 1 3)))
		 (comp (if (not (eqv? comp-index 'no-component)) (vector-ref comps comp-index) #f))
		 (port-name "w")
		 (isV (and comp (string=? "$BrzV" (brz-component:name comp))))
		 (struct-port-index (if comp
			(find-struct-port-index (brz-component:ports comp) port-index) (cons port-index 0)))
		 (primitive (if comp (dot-find-primitive comp) #f))
		)
		(if primitive
			(let*
				((port (list-ref (cdr (brz-primitive-part:ports primitive)) (car struct-port-index)))
				 (port-dot-name (find-headed-sub-list port 'dot-name))
				)
				(string-append "comp" (number->string comp-index)
					(if port-dot-name
						(string-append ":" (cadr port-dot-name)
							(if (cdr struct-port-index) (number->string (cdr struct-port-index)) "")
							; append second port-dot-name arg as port compass point
							(if (null? (cddr port-dot-name))
								""
								(string-append ":" (caddr port-dot-name))
							)
						)
						""
					)
				)
			)
			(string-append "comp" (number->string comp-index))
		)
	)
))

(define edge-colours #("black" "brown" "red" "orange" "goldenrod" "green" "blue" "violet" "grey" "white"))

(define dot-make-edge-style (lambda (sense width)
	(let*
		((log-width (if width (bit-length width) 0))
		 (colour (vector-ref edge-colours (if (> log-width 8) 8 log-width)))
		)
		(string-append "color=\"" colour "\"")
	)
))

(define dot-print-arcs (lambda (channels channel-links comps channel-names)
	(fold (lambda (chan index)
		(let*
			((chan-links (vector-ref channel-links index))
			 (width (brz-channel:width chan))
			 (head-port-no (dot-arc-end-port-no #t (+ 1 index) chan chan-links comps))
			 (tail-port-no (dot-arc-end-port-no #f (+ 1 index) chan chan-links comps))
			)
			(if (eq? 'no-component (vector-ref chan-links 2))
				(print "port" (+ 1 index))
				; (print "comp" (vector-ref chan-links 2))
				(print (dot-comp-port #f (+ 1 index) chan-links comps))
			)
			(print " -> ")
			(if (eq? 'no-component (vector-ref chan-links 0))
				(print "port" (+ 1 index))
				; (print "comp" (vector-ref chan-links 0))
				(print (dot-comp-port #t (+ 1 index) chan-links comps))
			)
			(print
				" [ "
				(if breeze2ps-chan-labels
					(let ((label (dot-arc-label (+ 1 index) chan chan-links comps channel-names)))
						(if (string=? label "")
							""
							(string-append "label=\"" label "\" ")
						)
					)
					""
				)
				(if breeze2ps-colour-chans
					(dot-make-edge-style (brz-channel:type chan) width)
					""
				)
				" "
				(if breeze2ps-wide-chans
					(let ((pen-width (max 1 (bit-length width))))
						(string-append "penwidth=" (number->string pen-width)
							" weight=" (number->string pen-width))
					)
					""
				)
				" "
				(if head-port-no (string-append "headportno=" (number->string head-port-no) " ") "")
				(if tail-port-no (string-append "tailportno=" (number->string tail-port-no) " ") "")
				(if breeze2ps-port-nos
					(string-append
						"headlabel=\"" (dot-arc-end-label #t (+ 1 index) chan chan-links comps) "\" " 
						"taillabel=\"" (dot-arc-end-label #f (+ 1 index) chan chan-links comps) "\" " 
					)
					""
				)
				"]" #\newline)
		) (+ 1 index)
	) 0 channels)
))

(define make-part-dot-code (lambda (procedure context)
	(let*
		((channel-count (length (brz-breeze-part:channels procedure)))
		 (linked-c-and-c (brz-link-channels-and-components
			(brz-breeze-part:channels procedure)
			(brz-breeze-part:components procedure) context)) ; Join channels to components
		 (channels (car linked-c-and-c))
		 (components (cadr linked-c-and-c))
		 (chan-name-mapping (list-ref linked-c-and-c 2))
		 (chan-names (list-ref linked-c-and-c 3))
		 (comp-names (list-ref linked-c-and-c 4))
		 (attributes (brz-breeze-part:attributes procedure))
		 (proc-line-no (assoc "line" (cdr attributes)))
		 (part-name (brz-breeze-part:name procedure))
		)
		(print "digraph anything {" #\newline)
		(print "outputorder=edgesfirst" #\newline)
		; (print "ordering=out" #\newline)
		; (print "rankdir=\"LR\"" #\newline)
		(if breeze2ps-ratio (print "ratio=" breeze2ps-ratio #\newline))
		(if breeze2ps-landscape
			(print "size=\"" (cadr breeze2ps-size) "," (car breeze2ps-size) "\"" #\newline "rotate=90" #\newline)
			(print "size=\"" (car breeze2ps-size) "," (cadr breeze2ps-size) "\"" #\newline)
		)
		(print "edge [ " tech-dot-default-edge-style " ]" #\newline)
		(if tech-dot-default-node-style
			(print "node [ " tech-dot-default-node-style " ]" #\newline)
			(print "node [ " dot-node-style " ]" #\newline)
		)
		(dot-print-ports (cdr (brz-breeze-part:ports procedure)) part-name chan-name-mapping)
		(dot-print-components (brz-breeze-part:name procedure) (cdr (brz-breeze-part:components procedure)) part-name
			comp-names)
		(dot-print-arcs (cdr (brz-breeze-part:channels procedure)) channels components chan-names)
		(print "}" #\newline)
	)
))

;;; breeze2ps-{no-banner,...}: command line switches
(define breeze2ps-no-banner #f)
(define breeze2ps-keep-dot-files #f)
(define breeze2ps-single-file #t)
(define breeze2ps-landscape #f)
(define breeze2ps-import-path balsa-search-path)
(define breeze2ps-tech #f)
(define breeze2ps-colour-chans #f)
(define breeze2ps-wide-chans #f)
(define breeze2ps-size '(7 10))
(define breeze2ps-box #t)
(define breeze2ps-port-nos #t)
(define breeze2ps-chan-nos #t)
(define breeze2ps-chan-labels #t)
(define breeze2ps-comp-clusters '())
(define breeze2ps-port-clusters '())
(define breeze2ps-cluster-other-comps #f)
(define breeze2ps-show-comp-nos #f)
(define breeze2ps-ratio #f)
(define breeze2ps-comp-labels '())

;;; breeze2ps-print-banner: print the breeze2ps banner
(define breeze2ps-print-banner (lambda ()
(make-program-banner "breeze2ps" "breeze2ps: Breeze -> Postscript Converter"
	"2000-2008, The University of Manchester")
))

;;; breeze2ps-usage: command usage
(define breeze2ps-usage (lambda ()
	(breeze2ps-print-banner)
	(error
		"version " balsa-version #\newline
		"usage: breeze2ps {<switch>}* <block/file-name>" #\newline #\newline
		"switches: -h or -?           - Display this message (--help)" #\newline
		"          -b                 - Don't print the breeze2ps banner (--no-banner)" #\newline
		"          -k                 - Keep intermediate .dot files (--keep-dot-files)" #\newline
		"          -s                 - Create separate files for each procedure (--separate-files)" #\newline
		"          -l                 - Produce a landscape plot (--landscape)" #\newline
		"          -p                 - Produce a portrait (default) plot (--portrait)" #\newline
		"          -I <directory>     - Add named directory to the import path (--import)" #\newline
		"          -X <tech>          - Set `technology/style/style-opts' to use.  (--technology)" #\newline
		"                               Technology will be set by ${BALSATECH} env. var. if -X is not passed" #\newline
		"          --a3               - Use A3 paper" #\newline
		"          --a4               - Use A3 paper (default)" #\newline
		"          -c                 - Colour channels according to log2 of width (--colour-chans)" #\newline
		"                               0:black, 1:brown, 2-4:red, 5-8:orange ..." #\newline 
		"          -w                 - Use arc width to signify channel width (--wide-chans)" #\newline
		"          --no-box           - Don't box procedure contents apart from ports" #\newline
		"          --no-port-nos      - Don't show ports numbers at ends of arcs" #\newline
		"          --no-chan-nos      - Don't show `C<no>:' prefix of channnel arcs" #\newline
		"          --no-chan-labels   - Don't show labels on arcs" #\newline
		"          --comp-nos         - Show component numbers at start of labels" #\newline
		"          --comp-labels <proc-name> (<comp-no>(=<label>)?,(<comp-no>(=<label>)?)*) - Label components." #\newline
		"                               If no label is given then the component number is used." #\newline
		"          --comp-cluster <proc-name> <cluster-name> <comp-no>(,<comp-no>)*) - Cluster components" #\newline
		"          --port-cluster <proc-name> <cluster-name> <port-name>(,<port-name>)*) - Cluster ports" #\newline
		"          --cluster-other-comps <cluster-name> - Cluster together all" #\newline
		"                               not otherwise clustered components into named cluster" #\newline
		"          --ratio            - Set output aspect ratio (y/x)" #\newline
		#\newline
		"          Prefix cluster names with:" #\newline
		"                 : - to produce a dotted box around components/ports" #\newline
		"                 = - to produce a dashed box around components/ports" #\newline
		"                 _ - to produce a solid box around components/ports" #\newline
	)
))

;;; breeze2ps-command-line-rules: command-line-args action rules
(define breeze2ps-command-line-rules `(
	(#\b "no-banner" 0 ,(lambda (args) (set! breeze2ps-no-banner #t)))
	(#\h "help" 0 ,(lambda (args) (breeze2ps-usage)))
	(#\? "help" 0 ,(lambda (args) (breeze2ps-usage)))
	(#\k "keep-dot-files" 0 ,(lambda (args) (set! breeze2ps-keep-dot-files #t)))
	(#\s "separate-files" 0 ,(lambda (args) (set! breeze2ps-single-file #f)))
	(#\l "landscape" 0 ,(lambda (args) (set! breeze2ps-landscape #t)))
	(#\p "portrait" 0 ,(lambda (args) (set! breeze2ps-landscape #f)))
	(#\I "import" 1 ,(lambda (args) (set! breeze2ps-import-path (append breeze2ps-import-path
		(list (car args))))))
    (#\X "technology" 1 ,(lambda (args) (set! breeze2ps-tech (car args))))
    (#\c "colour-chans" 0 ,(lambda (args) (set! breeze2ps-colour-chans #t)))
    (#\w "wide-chans" 0 ,(lambda (args) (set! breeze2ps-wide-chans #t)))
    (#\- "no-box" 0 ,(lambda (args) (set! breeze2ps-box #f)))
    (#\- "a4" 0 ,(lambda (args) (set! breeze2ps-size '(7 10))))
    (#\- "a3" 0 ,(lambda (args) (set! breeze2ps-size '(10 14))))
    (#\- "no-port-nos" 0 ,(lambda (args) (set! breeze2ps-port-nos #f)))
    (#\- "no-chan-nos" 0 ,(lambda (args) (set! breeze2ps-chan-nos #f)))
    (#\- "no-chan-labels" 0 ,(lambda (args) (set! breeze2ps-chan-labels #f)))
    (#\- "cluster-other-comps" 1 ,(lambda (args) (set! breeze2ps-cluster-other-comps (car args))))
    (#\- "comp-cluster" 3 ,(lambda (args)
    	(let ((comp-nos (map string->number (parse-string (caddr args) '(#\,)))))
    		(set! breeze2ps-comp-clusters (cons (cons* (car args) (cadr args) comp-nos)
    			breeze2ps-comp-clusters))
    	)
    ))
    (#\- "port-cluster" 3 ,(lambda (args)
    	(let ((port-names (parse-string (caddr args) '(#\,))))
    		(set! breeze2ps-port-clusters (cons (cons* (car args) (cadr args) port-names)
    			breeze2ps-port-clusters))
    	)
    ))
    (#\- "comp-nos" 0 ,(lambda (args) (set! breeze2ps-show-comp-nos #t)))
    (#\- "ratio" 1 ,(lambda (args) (set! breeze2ps-ratio (car args))))
    (#\- "comp-labels" 2 ,(lambda (args)
		(let ((labels
			(map (lambda (label-elem)
				(let*
					((label/value (parse-string label-elem '(#\=)))
					 (label (string->number (car label/value)))
					 (value (if (null? (cdr label/value)) (number->string label) (cadr label/value)))
					)	
					(cons label value)
				)
			) (parse-string (cadr args) '(#\,)))))
			(set! breeze2ps-comp-labels (cons (cons* (car args) labels)
				breeze2ps-comp-labels))
    	)
    ))
))

;;; breeze2ps-parse-command-line: parse switches from the given command line list, set
;;;		the breeze2ps-... globals and return the tail of the command line.
(define breeze2ps-parse-command-line (lambda (args)
	(if (null? args)
		(breeze2ps-usage)
		(let ((args-tail (parse-command-line "breeze2ps" breeze2ps-command-line-rules breeze2ps-usage args)))
			(if (/= 1 (length args-tail))
				(error "breeze2ps: expecting exactly one file name" #\newline)
				args-tail
			)
		)
	)
))

;;; dot-{port,edge,...}-style: dot(1) definitions for graph component styles
(define dot-port-style "style=bold shape=box")
(define dot-node-style "fontsize=12 fontname=\"Helvetica-Bold\" shape=ellipse")

;;; breeze2ps: main, reads from the file `filename'
(define breeze2ps (lambda (filename)
	; Allow use of common technology if technology not set.  Otherwise use the (possibly extended)
	; component set of the chosen technology
	(balsa-set-tech breeze2ps-tech)
	(if (string=? breeze-tech "common")
		(begin)
		(brz-get-technology)
	)
	(brz-load-primitives)
	; Set edge decoration procedure.  This can be overridden in startup.scm files
	(if (not tech-dot-make-edge-style)
		(set! tech-dot-make-edge-style dot-make-default-edge-style)
	)
	(if (not tech-dot-default-edge-style)
		(set! tech-dot-default-edge-style "fontsize=8 fontname=Helvetica labelfontname=Helvetica labelfontsize=8")
	)
	(let*
		((path/name/ext (find-filename filename "breeze" #f breeze2ps-import-path))
		 (path (car path/name/ext))
		 (name (cadr path/name/ext))
		 (decls/imports/imported-decls/visited-blocks (get-flattened-block
			(fold flip-string-append "" path/name/ext) "breeze" '() breeze2ps-import-path))
		 (decls (car decls/imports/imported-decls/visited-blocks))
		 (imports	
			(merge-sort! string<=? (cadr decls/imports/imported-decls/visited-blocks))
		 )
		 (imported-decls (caddr decls/imports/imported-decls/visited-blocks))
		 (context (append decls imported-decls))
		 ; AB 2008-07-25 remove checks to make importing experimental stuff easier
		 ; (parts (reverse! (filter brz-check-breeze-part? decls))) ; Breeze parts read form the input file
		 (parts (filter (lambda (part) (headed-list? part 'breeze-part)) decls))
		 (imported-types (filter brz-check-type-decl? imported-decls))
		 (local-types (filter brz-check-type-decl? decls))
		 (types (append local-types imported-types))
		 ; prune-file/run-dot : helper procedures for later stuff
		 (prune-file (lambda (filename)
			(if (not breeze2ps-keep-dot-files)
				(system (string-append "/bin/rm -f " filename))
			)
		 ))
		 (run-dot (lambda (dot-file ps-file)
			(system (string-append "dot -Tps " dot-file " > " ps-file))
		 ))
		)
		(if breeze2ps-single-file
			(let
				((dot-file (string-append filename ".dot"))
				 (ps-file (string-append filename ".ps"))
				)
				(with-output-to-file dot-file (lambda ()
					(for-each (lambda (proc) (make-part-dot-code proc context)) parts)
				))
				(run-dot dot-file ps-file)
				(prune-file dot-file)
			)
			(for-each (lambda (proc)
				(let*
					((procedure-file (string-append filename "-" (brz-breeze-part:name proc)))
					(dot-file (string-append procedure-file ".dot"))
					(ps-file (string-append procedure-file ".ps"))
					)
					(with-output-to-file dot-file (lambda () (make-part-dot-code proc context)))
					(run-dot dot-file ps-file)
					(prune-file dot-file)
				)
			) parts)
		)
	)
))

(top-level (lambda (args)
	(let
		((args-tail (breeze2ps-parse-command-line args)))
		(if (not breeze2ps-no-banner)
			(breeze2ps-print-banner)
		)
		(apply breeze2ps args-tail)
	)
) (cdr command-line-args))
