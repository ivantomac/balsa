;;;
;;; The Balsa Asynchronous Hardware Synthesis System
;;; Copyright (C) 1995-2003 Department of Computer Science
;;; The University of Manchester, Oxford Road, Manchester, UK, M13 9PL
;;; 
;;; This program is free software; you can redistribute it and/or modify
;;; it under the terms of the GNU General Public License as published by
;;; the Free Software Foundation; either version 2 of the License, or
;;; (at your option) any later version.
;;; 
;;; This program is distributed in the hope that it will be useful,
;;; but WITHOUT ANY WARRANTY; without even the implied warranty of
;;; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
;;; GNU General Public License for more details.
;;; 
;;; You should have received a copy of the GNU General Public License
;;; along with this program; if not, write to the Free Software
;;; Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
;;;
;;; 'net-insert-buffers.scm' 
;;; Automatic buffer insertion for balsa-netlist netlist generator
;;;
;;; anotates components with input and output load information if this attribute is missing
;;;


(define balsa-insert-b-verbose #f)
(define balsa-insert-b-log-stream #f)    ;; for future log file generation
(define balsa-insert-b-log-file-name #f)
(define balsa-remove-aliases tech-remove-aliases)

;; drive factor of xN drive output (must be an integer > 0)
(define x-drive-factor tech-x-drive-factor)
;; Max load threshold for buffer insertion
;; anything above this drive load will be buffered
(define load-threshold tech-load-threshold)
;; types of signals to be buffered:
;; i-port : signal belongs to an input port
;; o-instance : signal belongs to an output instance
(define signal-types (list 'i-port 'o-instance))
;; load represented by an hc input port to be accounted into an output port total load
(define hc-out-port-load 0)
;;

;; globals
(define gate-not-found 0)
(define do-not-change 0)
(define total-signals-buffered 0)
(define total-instances-updated 0)
(define total-buffers-inserted (make-hash-table 5))
(define total-alias-removed 0)
(define total-alias-not-removed 0)
(define list-of-circuits '())
(define my-net '())
(define real-gates-f '())
(define helper-mappings-f '())
(define all-mappings-f '())
(define real-gates '())
(define helper-mappings '())
(define all-mappings '())

;; default real buffer name
(define default-real-buf-name "")
;; logical buffer cell name
(define default-logical-buf-name "")

(define real-buffers "")

;; List of breeze parts in the net
(define Brz-circuit-list '())
(define Balsa-circuit-list '())
;; List of basic parts in the net
(define simple-circuit-list '())
(define simple-parts (make-hash-table 31))
(define total-Brz-circuits 0)
(define counter-Brz 0)
(define highest-drive-load (list 0 '()))
(define analised-circuit "")

(define v-note (lambda args (if balsa-insert-b-verbose (apply print args))))

(define print-nl
    (lambda x
        (for-each v-note  x)
        (v-note #\nl)
))

;;
;; net-display-circuit-decl: pretty display a net-circuit-decl on screen
;;
(define net-display-circuit-decl (lambda (circuit)
    (if (null? circuit)
        (print-nl "; NULL circuit")
        (let    ((new-tabs "    "))
                (print "(circuit \"" (net-circuit-decl:name circuit) "\"")
                ((lambda ()
                    (for-each (lambda (elem)
                        (print #\newline new-tabs)
                        (simple-print elem new-tabs '(circuit instances ports nets))
                    ) (cddr circuit))
                ))
                (print #\newline ")" #\newline #\newline)
        )
    )
))

;;; returns an integer describing guile's version
(define guile-version (+ (* 100 (string->number (major-version))) (string->number (minor-version))))

;;; added for compatibility with old versions of guile interpreter (<1.8.0)
(define balsa-hash-clear! (lambda (ht) 
    (if (< guile-version 108) 
        (hash-fold (lambda (k v x) (hash-remove! ht k)) "" ht) 
        (hash-clear! ht))
))

;;; added for compatibility with old versions of guile interpreter (<1.8.0)
(define balsa-hash-for-each (lambda (proc ht) 
    (if (< guile-version 180) 
        (hash-fold (lambda (k v x) (proc k v)) "" ht) 
        (hash-for-each proc ht))
))

;;; added for compatibility with old versions of guile interpreter (<1.8.0)
(define int-ceiling (lambda (x) (inexact->exact (ceiling x))))

;; checks if given key exists in a hash table
(define key-exist (lambda (key htable)
        (pair? (hash-get-handle htable key))))
;;
;;
;; list of input ports in the circuit net
(define ci-list '())
;; list of output ports
(define co-list '())
;; list of the outputs of all instances in the circuit definition
(define io-list (make-hash-table 511))
;; list of the inputs of all instances in the circuit
(define ii-list (make-hash-table 511))
;; table of instance-output -> total-load for each output whose
;; total-load >= load threshold
(define ip-load (make-hash-table 51))
(define io-load (make-hash-table 51))
;;
(define load-list '())
;;
(define alias-hash (make-hash-table 127))
(define rev-alias-hash (make-hash-table 255))
(define de-aliased-list '())
;;
;;; logical buffer ports definition
(define buff-first-port '())
;;; reference index for the output pin of logical buffer
(define buff-output-ref-index '())
;; left-to-right logical buffer?
(define l-r-buff? '())
;;
(define net-find-instance (lambda (instances name)
    (find-headed-list-elem instances 'instance name))
)


;;
(define net-insert-circuit (lambda (netlist circuit)
        (append! netlist (list circuit))
))

;; get the type of node associated with a connection
;; (input, output, internal)
(define get-node-type (lambda (node port-list)
    (let ((in-port-list (assoc (safe-car node) port-list)))
         (if (pair? in-port-list)
             (net-port:direction in-port-list)
             'internal
        )
    )
))
;;
(define circuit-insert-port (lambda (circuit port)
    (if (null? (cdr (net-circuit-decl:ports circuit)))
        (append! (net-circuit-decl:ports circuit) (list port))
        (append! (cdr (net-circuit-decl:ports circuit)) (list port))
    )
))
;;
(define circuit-insert-net (lambda (circuit net)
    (if (null? (cdr (net-circuit-decl:nets circuit)))
        (append! (net-circuit-decl:nets circuit) (list net))
        (append! (cdr (net-circuit-decl:nets circuit)) (list net))
    )
))
;;
(define circuit-insert-instance (lambda (circuit i-name i-conns)
    
    (let
        ((connections
            (if (and (> (string-length i-name) 14) (string=? (substring i-name 0 13) "insertedBTree"))
                i-conns
                (let
                   ((gate-mapping (cadr (assoc "buf" breeze-gate-mappings))))
                   (map (lambda (port-index) (list-ref i-conns port-index)) (cdr gate-mapping))
                )
            )
        ))
        (if (null? (cdr (net-circuit-decl:instances circuit)))
            (append! (net-circuit-decl:instances circuit) (list (list 'instance i-name connections)))
           (append! (cdr (net-circuit-decl:instances circuit)) (list (list 'instance i-name connections)))
        )
    )
))
;;
(define circuit-insert-attribute (lambda (circuit a-name a-value)
    (if (list? (net-circuit-decl:attributes circuit))
        (append! (cdr (net-circuit-decl:attributes circuit)) (list (list a-name a-value)))
        (append! circuit (list (list 'attributes (list a-name a-value))))
    )
))
;;
;;
(define circuit-remove-instance (lambda (circuit instance)
    (list-set! circuit 4
               (append (list 'instances) (delete instance (cdr (net-circuit-decl:instances circuit)))))
))
;
(define circuit-rename-instance (lambda (circuit instance-ref new-name)
    (list-set! 
        (cdr (list-ref (cdr(net-circuit-decl:instances circuit)) instance-ref)) 0 new-name)
))
;
;
(define circuit-replace-instance-conn (lambda (circuit instance-ref conn-ref new-conn)
    (list-set! 
            (net-instance:instance-connections
                (list-ref (cdr(net-circuit-decl:instances circuit)) instance-ref)) conn-ref new-conn)
))
;
;

(define is-removable? (lambda (net args)
    (if (null? args)
        #t
        (begin
              ;;(print-nl "    checking " net " against " args)
              (if (equal? (caar args) (safe-car net))
                  #f 
                  (is-removable? net (cdr args))
              )
        )
    )
))

(define removable-net (lambda (net conns)
    (if (pair? net)
        (is-removable? net conns)
        #t
    )
))


(define removable-buff? (lambda (conns n-r-nodes)
    (not (or (pair? (member (car conns) n-r-nodes)) (pair? (member (cdr conns) n-r-nodes))))
)) 

;; remove  buffs instances given in list 
;;
(define safe-remove-buff (lambda (circuit list-of-buffs n-r-nodes)
    (for-each (lambda (buff)
        (if (removable-buff? (net-instance:instance-connections buff) n-r-nodes) 
            (circuit-remove-instance circuit buff))) list-of-buffs)
))

;;
(define circuit-remove-aliases (lambda (circuit rev-alias-hash nr-conns)
   ;; (display "#\n buffdef=")
   ;; (display (cadr (net-circuit-decl:ports (net-find-circuit all-mappings "buff"))))
    (let* ((instance-ref 0)
           (conn-ref 0)
           (instances (cdr (net-circuit-decl:instances circuit))))
           (for-each (lambda (instance)
                (set! conn-ref 0)
                (for-each (lambda (conn)
                    (if (key-exist conn rev-alias-hash)
                        (if (removable-net conn nr-conns)
                            ;; maker sure to only change the inputs of buff instances!!!
                            (if (not (and (equal? default-logical-buf-name (net-instance:name instance))
                                          (equal? conn-ref buff-output-ref-index))) 
                                (begin
                                    (circuit-replace-instance-conn 
                                        circuit instance-ref conn-ref (hash-ref rev-alias-hash conn))
                                    (set! total-alias-removed (inc total-alias-removed))
                                )
                            )
                            (set! total-alias-not-removed (inc total-alias-not-removed))
                           ;; (print-nl "Not able to remove: " conn " in instance: " (net-instance:name instance))
                        )
                    )
                    (set! conn-ref (inc conn-ref))) (net-instance:instance-connections instance)
                )
                (set! instance-ref (inc instance-ref))) instances)
    )
))





;; access the name of the default real gate of the first gate in a list de gate mappings
;; where each entry is (abs-gate-name default-real-gate . weighted-real-gates)
;;(see gate-mappings format)
(define default-real-gate:name caadar)
;;
;;
;; access the list of weighted gates in a entry from gate-mappings 
;; where each entry is (abs-gate-name default-real-gate . weighted-real-gates)
;;(see gate-mappings format)
(define weighted-real-gates cddr)
;;
;;
(define get-stronger-gate (lambda (weight w-gate-list previous-gate)
    (if (null? w-gate-list)
        (list previous-gate gate-not-found) ;stronger gate not found return gate with highest drive weight available
        (if (<= weight (caar w-gate-list))
            w-gate-list
            (get-stronger-gate weight (cdr w-gate-list) (car w-gate-list))
        )
    )
))

;; find a gate definition in real-gates
(define find-gate-def (lambda (gate-name gate-list)
    ;;(print "finding " gate-name " in " (safe-car gate-list) #\nl)
    (if (null? gate-list)
        (begin
            (note "\n Error: net-insert-buffers:find-gate-def: can't find gate definition for " gate-name #\nl)
            (quit 1)
        )
        (if (equal? gate-name (safe-car (assoc gate-name 
                              (map reverse (weighted-real-gates (car gate-list))))))
            (car gate-list)
            (find-gate-def gate-name (cdr gate-list))
        )
    )
))
;; find the drive buffer definition  in real-gates
;; finds the sublist whose car is the string cell-name
;; Every gate-mapping file must have this name for drive buffers defn sublist
;;
(define find-buf-def (lambda (cell-name gate-list)
    (if (null? gate-list)
        '() ;; this point should never be reached !!!!
        (begin
		(print-nl "Checking " (car gate-list)  " for " cell-name #\nl)
		(if (equal? cell-name (caar gate-list))
            (car gate-list)
            (find-buf-def cell-name (cdr gate-list))
        )
		)
    )
))


;(define b-tree-circuit (list 'circuit "" (list 'ports) (list 'nets) (list 'instances)))

(define b-tree-circuit (list 'circuit "" '(ports) '(nets) '(instances)))
    

;; calculates port-load attribute for helper cell
;; generates a headed list ('port-load port0-load port1-load ... portN-load)
;; with the loads for each port. It is expected that all helper cells have
;; only ports with cardinality = 1, however, if portI has cardI > 1, the entry
;; in the port load list will be a list with the load for each smashed port. 
;; The generated list is added as an attribute  to the circuit
;;
(define helper-cell-add-port-load-attr (lambda (circuit)
  (print-nl "calculating load for helper " circuit)
  (if (net-check-instances? (net-circuit-decl:instances circuit)) ; only do it for helper cells consisting of instances only
      (begin 
        (calculate-loads circuit) ;; updates ii-list & ci-list
        ;(balsa-hash-for-each (lambda (i j) (print-nl "[" i "]" j)) ii-list)
        ;(print-nl "ci-list=" ci-list)
        (let* ((pin-load (list '()))
               (port-list ci-list))
            (for-each (lambda (port)
                (let* ((smashed-port '())
                       (s-port-loads '())
                       (port-name (net-port:name port))
                       (port-card (net-port:cardinality port))
                       (node-type (get-node-type port ci-list))
                       (port-ref (list port-name 0 port-card)))
                      (if (> port-card 1) ;; NOTE: normally helpers don't have cardinality > 1
                          (begin
                            (set! smashed-port (net-smash-instance-connection port-ref))
                            (for-each (lambda (s-port)
                                (if (key-exist s-port ii-list)
                                    (set! s-port-loads 
                                        (append s-port-loads (car (hash-ref ii-list s-port)))) 
                                    (set! s-port-loads (append s-port-loads 0))
                                )) smashed-port)
                          ) 
                          (if (key-exist port-name  ii-list)
                              (set! s-port-loads (car (hash-ref ii-list port-name))) 
                              (set! s-port-loads 0)
                          )
                      )
                      (set! pin-load (append pin-load (list s-port-loads)))
                )) port-list)
            ;; append the generated list
			(circuit-insert-attribute circuit 'port-load (cdr pin-load)) ;; get rid of empty list at the begining
;;            (cdr pin-load)
        )
      )
      (begin
         (let* 
		 	((port-list (cdr (net-circuit-decl:ports circuit)))
             (pin-load (foldl (lambda (pload pinport)
                (let* ((port-name (net-port:name pinport))
                       (port-card (net-port:cardinality pinport))
                       (port-ref (list port-name 0 port-card))
					   (s-port-loads (foldl (lambda (s-p-l s-port)
                        (cons 0 s-p-l)
                    	) '() (net-smash-instance-connection port-ref)))
                      )
                      (append pload s-port-loads)
				)
             ) '() port-list))
			)
            ;; append the generated list
            (circuit-insert-attribute circuit 'port-load pin-load) ;; get rid of empty list at the begining
        )
      )
  )
))

(define add-load-attribute (lambda (circuit)
    (if (is-helper-cell? circuit)
        (helper-cell-add-port-load-attr circuit)
        (let* ((pin-load (list '()))
               (port-list (cdr (net-circuit-decl:ports circuit))))
              (for-each (lambda (port)
                (if (eqv? 'input (net-port:direction port))
                    (set! pin-load (append pin-load (list 1)))
                    (set! pin-load (append pin-load (list 0)))
                )) port-list)
              (circuit-insert-attribute circuit 'port-load (cdr pin-load)) ;; get rid of empty list at the begining
       )
    )
))

;; returns a description of a buffer-tree as a list of stages (from top to root):
;; (stage_1 stage_2 ... stage_N) where stage_i = (buffer-type total-buffers)
;; and buffertype = (output-drive gate-name)

;; generates a copy of a simple circuit for a gate but with the name of a stronger gate
;; with same function as found in tech map list (gate-mappings)

(define stronger-gate-circuit-defn (lambda (gate-name strong-gate-name)
    (let* ((gate-defn (list-copy (net-find-circuit all-mappings gate-name))))
        ;;access the name and change it (quick and easy way for me to do it)
          (list-set! gate-defn 1 strong-gate-name)
          gate-defn
    )
))

(define find-in-simple-parts (lambda (part-name)
    (let* ((part (hash-ref simple-parts part-name)))
          (if (eqv? #f part) ;; part not found, look into all-mappings (hopefully it will be there :))
              (begin
                (set! part (list-copy (net-find-circuit all-mappings part-name))) ;find it
                (set! simple-circuit-list (append simple-circuit-list (list part)))  ; save it
                (hash-create-handle! simple-parts part-name part)
              )
           )
           ;;(print-nl "in looking for " part-name ", found: " part)
           part
    )
))

;; creates a buffer tree description for the given total load and the given
;; max-drive-avail buffer. initially stages must be set to 0 and tree must be the empty list
;; returns a list of buffer-type and number of buffers require per stage
;;(from top to root)
;; ( ((buff-type) number-of-buffers) ((buff-type) number-of-buffers) ...)
;; where (buff-type) is (drive buff-name) as described in gate-mappings
;;
(define make-buffer-tree (lambda (total-load max-drive-avail stages tree)
    ;;(set! max-drive-avail 1)
    (let* ((buffers-needed (int-ceiling (/ total-load (* x-drive-factor (caar max-drive-avail)))))
          (lower-normalised-load (int-ceiling (/ buffers-needed x-drive-factor)))
          (lower-branch (get-stronger-gate lower-normalised-load real-buffers '())))
          ;(print-nl "buff needed:" buffers-needed " norm-load:" lower-normalised-load " lower-branch:" lower-branch)
           (if (>= x-drive-factor buffers-needed)
               (begin
                    ;;(print-nl "looking for " (cadar max-drive-avail))
                    (if (not (key-exist (cadar max-drive-avail) simple-parts))
                        (let ((gate-circuit (net-find-circuit all-mappings (cadar max-drive-avail))))
                             (set! simple-circuit-list (append simple-circuit-list (list gate-circuit)))
                             (hash-create-handle! simple-parts (cdar max-drive-avail) gate-circuit)
                             ;;(print-nl "--inserted " (cadar max-drive-avail) " " gate-circuit " -- " max-drive-avail)
                        )
                    )
                    (append tree (list (list (car max-drive-avail) buffers-needed)))
                ) 
                (begin
                    (set! tree (append tree (list (list (car max-drive-avail) buffers-needed))))
                    (make-buffer-tree buffers-needed lower-branch (+ 1 stages) tree)
                )
            )
          )
    )
) 

;; creates a buffer-tree circuit from a buffer-tree description
(define create-buffer-tree-circuit (lambda (tree)
    ;;(print-nl " entered creating buffer tree ")
    (let* ((total-stages (length tree))
           (this-stage 0)
           (i-port '())
           (o-port '())
           (tree-name (string-append "insertedBTree_" (number->string (cadar tree))))
           (tree-circuit (list 'circuit tree-name (list 'ports) (list 'nets) (list 'instances)))
           (prev-net-decl (lambda (tc)
                (safe-car (reverse (cdr (net-circuit-decl:nets tc))))))
           (prev-prev-net-decl (lambda (tc) 
                (if (null? (prev-net-decl tc))
                    '()
                    (safe-car (safe-cdr (reverse (cdr (net-circuit-decl:nets tc))))))))
           (next-out-port (lambda (p-num tc stage-num)
                (cond ((= stage-num 0) (list "out" p-num)) ;; top of tree (outputs)
                      ((= stage-num (- total-stages 1)) ;; last stage? (root of tree reached)
                       (let ((p-net (prev-net-decl tc)))
                            (list (net-net:name p-net) (modulo p-num (net-net:cardinality p-net)))
                      ))
                      (else                                ;; inner branches
                         (let ((p-net (prev-prev-net-decl tc)))
                              (list (net-net:name p-net) (modulo p-num (net-net:cardinality p-net)))
                      ))
                )))
           (next-in-port (lambda (p-num tc is-root-stage)
                (let ((p-p-net (prev-net-decl tc)))
                ;;(print-nl "previous net:" p-p-net)
                     (if is-root-stage 
                         "in"
                         (list (net-net:name p-p-net) (modulo p-num (net-net:cardinality p-p-net)))
                     )
                 )))
           (insert-stage (lambda (stage stage-num is-root-stage)
                (let insert-buffer ((bufnum 0))
                    (if (> (cadr stage) bufnum) ; more buffers to insert?
                        (begin

                            ;;(print-nl "creating stage " stage-num " buffer " bufnum) 
                            (if (and (> total-stages 1) (not is-root-stage) (= 0 bufnum)) ;; insert node (only once)
                                (let ((net-name (string-append "internal_" (number->string stage-num) "n")))
                                     (circuit-insert-net 
                                     tree-circuit (list net-name (cadr (list-ref tree (+ 1 stage-num)))))
                                )
                            )
                            (set! o-port (next-out-port bufnum tree-circuit stage-num))
                            (set! i-port (next-in-port bufnum tree-circuit is-root-stage))
                            (circuit-insert-instance tree-circuit (cadar stage) (list o-port i-port))
                            (insert-buffer (+ 1 bufnum))
                         )
                    )
                 )
                )
            ))
           (if (key-exist tree-name simple-parts) ;; Tree already calculate, don't bother to do it again
               (begin
                    ;;(print-nl "using already created tree " tree-name)
                    ;(print-nl (hash-ref simple-parts tree-name))
                    (list (hash-ref simple-parts tree-name))
               )
               (begin 
                    (circuit-insert-port tree-circuit (list "out" 'output (cadar tree)))   
                    (circuit-insert-port tree-circuit (list "in" 'input 1))
                    (for-each (lambda (stage)
                            (insert-stage stage this-stage (= this-stage (- total-stages 1)))
                            (set! this-stage (+ 1 this-stage))) tree)
                    ;;FIXME: does not take into account real input load of root buffer, assumes 1. 
                    (circuit-insert-attribute tree-circuit 'port-load (list 0 1))
                    (list tree-circuit)
                )
            )
)))

;; add to total-buffers-inserted the buffers needed to create btree
;; btree= (((buff-type) number-of-buffers) ((buff-type) number-of-buffers) ...)

(define accumulate-buffers (lambda (btree)
    (for-each (lambda (buffer)
        (let ((key (string-append "buf x" (number->string (caar buffer))))
              (value (cadr buffer))
             )
             (if (key-exist key total-buffers-inserted)
                 (let ((prev-value (hash-ref total-buffers-inserted key)))
                      (hash-set! total-buffers-inserted key  (+ prev-value value) )
                 )
             )
             (hash-create-handle! total-buffers-inserted key value))
    ) btree)
))


;; find in real-gates list (from gate-mappings) if there is another gate
;; implementing the same function but with stronger output
;; otherwise create a buffer tree
(define find-stronger-gate (lambda (total-load gate-name gate-list)
    (if (equal? gate-name default-logical-buf-name)
        (set! gate-name default-real-buf-name)
    )
    (let ((gate-map (find-gate-def gate-name gate-list))
          (norm-load (int-ceiling (/ total-load x-drive-factor))))
          (print-nl " gate def for " gate-name " is: " gate-map )
        (if (null? gate-map)
            'gate-not-found:unexpected-error
            (let* ((stronger-gate (get-stronger-gate norm-load (weighted-real-gates gate-map) '()))
                  ;(max-avail (car stronger-gate)))
                 )
                 (print-nl "stronger gate found is " stronger-gate)
                 (if (equal? (safe-car (cdr stronger-gate)) gate-not-found) ; This means that a stronger gate was not found
                     (begin 
                        (set! gate-name default-real-buf-name)
                        (set! gate-map (find-gate-def gate-name gate-list))
                        (set! stronger-gate (get-stronger-gate norm-load (weighted-real-gates gate-map) '()))
                        (let* ((myTree (make-buffer-tree total-load stronger-gate 0 '()))
                             (my-tree-circuit '()))
                            (print-nl "inserting tree for " total-load " loads") 
                            (print-nl myTree)
                            (set! my-tree-circuit (car (create-buffer-tree-circuit myTree)))
                            ;(net-display-circuit-decl my-tree-circuit)
                            (string-append "[buffer tree " (net-circuit-decl:name my-tree-circuit) " inserted]")
                            (accumulate-buffers myTree)
                            (cons #f my-tree-circuit)
                        )
                    )
                    (cons #t stronger-gate)
                )
             )
        )
    )
))


;; connection load is the 5th element in list (iname i-number c-number node-type port-load)
(define conn-load (lambda (conn-values)
    (list-ref conn-values 4)
))

;;
;;connection type is the 4th element in list (iname i-number c-number node-type port-load)
;; input : connected to an input port of HC
;; output: connected to an output port of HC
;; internal : internal node
(define conn-type (lambda (conn-values)
    (list-ref conn-values 3)
))
;;
;; inserts or updates a value in ii-list or io-list
;; updates the total load
;; key = connection (might be a list (name index card) or just a name)
;; value = attributes of the connection
;; value format is (iname i-number c-number port-load type)
;; type can be internal, input or output
;;
(define insert-update-value (lambda (key value htable)
    ;;(print-nl "probing with " key " AND " value)
    (if (not (or (equal? "vcc" (safe-car key)) (equal? "vdd" (safe-car key)) (equal? "gnd" (safe-car key))))
            (if (key-exist key htable)
                (let ((prev-val (hash-ref htable key)))
                    ;;(print-nl "adding one to " port)
                    (hash-set! htable key 
                        (cons (+ (conn-load value) (car prev-val)) (append (cdr prev-val) (list value))))
                )
                (begin
                    ;;(print-nl " creating new entry : " (cons 1 (list value)))
                    (hash-create-handle! htable key (cons (conn-load value) (list value)))
                )
            )
    )
))
(define out-ports-with-internal-conns 0)
(define out-ports-buffered 0)
(define in-ports-buffered 0)
;;
;; selects the signals that need to be buffered from the ii-list (list of inputs)
;; generates a load-list with entries like
;; (node . (total-load node-type list-of-connections))
;; where node is a triple = (instance-name instance-index connection-index)
;;
(define select-signals (lambda (port l-and-conn)
(let* ((total-load (car l-and-conn))
      (connections (cdr l-and-conn)))
      ;; if the node is an instance output, add the internal load to the node:
      (if (key-exist port io-list)
          (let ((out-load (conn-load (cadr (hash-ref io-list port)))))
               (set! total-load (+ total-load out-load))
          )
      )
      ;; add the hc output port load to total loads, if necessary
      (if (eqv? 'output (get-node-type port ci-list))
          (begin
            ;;(print-nl "ADDING PORT LOAD initial=" total-load " new=" (+ total-load hc-out-port-load))
            (set! total-load (+ total-load hc-out-port-load))
            (set! out-ports-with-internal-conns (+ 1 out-ports-with-internal-conns))
          )
      )
      ;;(print-nl " net node: "port " type=" (get-node-type port ci-list) " total load=" total-load " connections=" connections) 
    (if (> total-load load-threshold)
        (begin
        ;;(print-nl port " located in ip-list? " is-input-port)
            (cond ((eqv? 'input (get-node-type port ci-list)) (set! in-ports-buffered (+ 1 in-ports-buffered)))
                  ((eqv? 'output (get-node-type port ci-list)) (set! out-ports-buffered (+ 1 out-ports-buffered)))              
            )
            (if (eqv? 'input (get-node-type port ci-list)) 
                (begin
                    ;;(print-nl " -- Node: " port " => " total-load " loads (input port)")
                    (hash-create-handle! ip-load port 
                        (cons total-load (append (list (list "input" (safe-car port))) (cdr l-and-conn))))
                )
                (let ((i-name-and-ref (hash-ref io-list port)))
                     ;;(print-nl " -- Node: " port " => " total-load " loads (instance output) " i-name-and-ref " " l-and-conn)
                     (hash-create-handle! io-load port (cons total-load (append (cdr i-name-and-ref) (cdr l-and-conn))))
                )
            )
            (if (< (car highest-drive-load) total-load)
                   (set! highest-drive-load (list total-load port analised-circuit)))

        )
    )))
)

;;
;; generates a list of nodes for a tree buffer (to be used in assing-loads
;;
;; list format is ((((n-name 0)) loads-connected) (((n-name 1)) loads-connected)...)
;; initially, loads connected=0
(define gen-list-avail (lambda (n-name total list-avail n)
    ;(print-nl "called with " list-avail) 
    (if (> total n)
        (gen-list-avail n-name total 
                        (append list-avail (list (list (list (list n-name n)) 0)))
                        (+ n 1))
        list-avail
    )
))
;;
;; Evenly distributes the signals given in the list conns between the outputs of a tree of buffers
;; returns a list of elements (((tree-output)(conn1)(conn2)..(connN)) output-load))
;; where (tree-output) = (node-name index)
;; conns is a list of (connN) elements where
;; (conn) = (instance-name instance-index connection-index connection-type pin-load)
;;
(define assign-balanced-loads (lambda (n-name total-ports conns)
    (let* ((s-conns (merge-sort (lambda (a b) (>= (conn-load a) (conn-load b))) conns))
           (avail-conns (gen-list-avail n-name total-ports '() 0)))
          (for-each (lambda (conn)
               (append! (caar avail-conns) (list conn))
               (set-car! avail-conns (list (caar avail-conns) (+ (cadar avail-conns) (conn-load conn))))  
               (set! avail-conns (merge-sort (lambda (a b) (<= (cadr a) (cadr b))) avail-conns))
               ) s-conns)
          avail-conns
    )
))




;; insert "tree" in "circuit" on "node". This implies:
;;  - adding a new net node named '("buf_tree_N" tree-outputs)', where N is a global
;;    counter of trees inserted and tree-outputs is the number of outputs of "tree"
;;  - changing all connections in the connections list to '("buff_tree_N" i)'
;;    ( i= [0..tree-outputs - 1] balancing the loads of all tree outputs)
;;  - connect the input of the tree to the output to be buffered
;;  - insert a new instance (the "tree")
;;
;; NB: the template of the tree circuit has already been inserted in the
;; list of basic parts by the caller
;;
;; connections = ((instance-name instance-index output-index type port-load) (instance-name instance-index input-index type port-load)...)
;;
(define insert-tree (lambda (circuit tree N node connections . single-buffer)
        (let* ((node-name (if (null? single-buffer)
                              (string-append "buf_tree_" ( number->string N) "n")
                              (string-append "in_buf" ( number->string N) "n")))
              (total-outputs (cadr (cdadr (net-circuit-decl:ports tree))))
              (input-list (cdr connections))
              (new-instance-name (net-circuit-decl:name tree))
              (instance-conn (list node-name 0 total-outputs))
              (node-counter 0)
              (output-counter 0)
              (output-nodes (assign-balanced-loads node-name total-outputs (copy-list input-list))))
             ; insert new node in nets
             (circuit-insert-net circuit (list node-name total-outputs))
             ; change connections in list of connections to the appropriate buffer tree output
              (for-each (lambda (bunch)
                (let ((new-conn (caar bunch)))
                    (for-each (lambda (connection)
                        (let* ((instance-index (cadr connection))
                               (conn-index (caddr connection)))
                               (circuit-replace-instance-conn circuit instance-index conn-index new-conn)
                        )) (cdar bunch))
                )) output-nodes)
             (set! instance-conn (append (list instance-conn) (list node)))
;;
             (circuit-insert-instance circuit new-instance-name instance-conn)
        )
))

;;
;; connections = ((instance-name instance-index output-index) (instance-name instance-index input-index)...)
;;
(define change-instance-name (lambda (circuit connections new-name)
        (let ((instance-index (cadar connections)))
             (circuit-rename-instance circuit instance-index new-name)
             (print-nl "changed instance at index: " instance-index " to " new-name)
        )
))
;;
;; Given the information in the input-port loads and internal output loads
;; insert buffers or change components where required
;;
(define insert-buffers (lambda (circuit ip-load-list io-load-list)
    (let* ((tree-number 0)
          (check-i-ports (lambda (node l-and-conn)
                (v-note "Finding a suitable buffer for " node)
                (let* ((total-load (car l-and-conn))
                       (connections (cdr l-and-conn)) 
                       (replacement (find-stronger-gate total-load default-real-buf-name real-gates)))
                      (if (eqv? 'circuit (cadr replacement))
                          (print-nl "... tree of buffers found is: " (net-circuit-decl:name (cdr replacement)))
                          (begin
                            (print-nl "... buffer found is: " (cadadr replacement))
                            (accumulate-buffers (list (list (list (caadr replacement)) 1)))
                          )
                      )
                     (if (car replacement) ; single buffer insertion required
                         (let ((part-name (cadar (cdr replacement))))
                               (if (not (key-exist part-name simple-parts))
                                   (let ((gate-circuit (net-find-circuit all-mappings part-name)))
                                        (set! simple-circuit-list (append simple-circuit-list (list gate-circuit)))
                                        (hash-create-handle! simple-parts part-name gate-circuit)
                                        (print-nl ":appending buffer" part-name "=>" gate-circuit)
                                    )
                                )
                                (insert-tree circuit (net-find-circuit all-mappings part-name) tree-number node connections 'single)
                                (set! tree-number (+ 1 tree-number))
                                (set! total-signals-buffered (+ 1 total-signals-buffered))
                         ) ; replacement is a tree of buffers 
                         (let ((part-name (net-circuit-decl:name (cdr replacement))))
                                 (if (not (key-exist part-name simple-parts))
                                     (let ((gate-circuit (cdr replacement)))
                                          (set! simple-circuit-list (append simple-circuit-list (list gate-circuit)))
                                          (hash-create-handle! simple-parts part-name gate-circuit)
                                          (print-nl ":appending tree " part-name "=>" gate-circuit)
                                     )
                                 )
                                 (insert-tree circuit (cdr replacement) tree-number node connections)
                                 (set! tree-number (+ 1 tree-number))
                                 (set! total-signals-buffered (+ 1 total-signals-buffered))
                         )
                     )
                )))
           (check-o-ports (lambda (node l-and-conn) 
                (v-note "Finding replacement for output of gate " (caadr l-and-conn))
                (let* ((total-load (car l-and-conn))
                       (connections (cdr l-and-conn)) 
                       (replacement (find-stronger-gate total-load (caar connections) real-gates)))
                      (if (eqv? 'circuit (cadr replacement))
                          (print-nl "... replacement found is: " (net-circuit-decl:name (cdr replacement)))
                          (print-nl "... replacement found is: " (cadadr replacement))
                      )
                      (if (car replacement) ; just a change of name required
                          (let ((part-name (cadar (cdr replacement))))
                               (if (not (key-exist part-name simple-parts))
                                   (let ((gate-circuit (net-find-circuit all-mappings part-name)))
                                        (if (eqv? #f gate-circuit)
                                            (begin 
                                                (display #\newline )
                                                (display (string-append "Unexpected error: in "  
                                                                        analised-circuit
                                                                        ": not circuit definition found for instance " 
                                                                        part-name))
                                                (display #\newline )
                                                (quit)
                                            )
                                        )
                                        (set! simple-circuit-list (append! simple-circuit-list (list gate-circuit)))
                                        (hash-create-handle! simple-parts part-name gate-circuit)
                                        (print-nl ":appending " part-name "=>" gate-circuit)
                                    )
                                )
                                (if (not (string=? part-name (caar connections))) ;;need to really change the name?
                                    (begin
                                        (change-instance-name circuit connections part-name)
                                        (set! total-instances-updated (+ 1 total-instances-updated))
                                 (set! total-signals-buffered (+ 1 total-signals-buffered))
                                    )
                                )
                           ) ; replacement is a tree of buffers 
                            (let ((part-name (net-circuit-decl:name (cdr replacement))))
                                 (if (not (key-exist part-name simple-parts))
                                     (let ((gate-circuit (cdr replacement)))
                                          (set! simple-circuit-list (append! simple-circuit-list (list gate-circuit)))
                                          (hash-create-handle! simple-parts part-name gate-circuit)
                                          (print-nl ":appending tree" part-name "=>" gate-circuit)
                                     )
                                 )
                                 (insert-tree circuit (cdr replacement) tree-number node connections)
                                 (set! tree-number (+ 1 tree-number))
                                 (set! total-signals-buffered (+ 1 total-signals-buffered))
                           )
                        )
                ))))
                (balsa-hash-for-each check-i-ports ip-load-list)
                (balsa-hash-for-each check-o-ports io-load-list)
)))

;;; create list of connections
(define buff-index-list '())

(define get-buff-conn-list  (lambda (circuit)
    (let* ((instances (cdr (net-circuit-decl:instances circuit)))
           (out-port-list '()) 
           (other-conn-list '())
           (instance-index 0) 
           (buff-conn-list '())
           (not-connected-to-output-port (lambda (connections out-port-list)
                (let ((connected #f))
                    (for-each (lambda (p) 
                        (set! connected (or (pair? (assoc (safe-car (car connections)) out-port-list))))
                    ) connections)
                    (not connected)
                ))))     ;;(not (pair? (assoc (safe-car(car connections)) out-port-list))))))
           (for-each (lambda (p) 
                (if (eq? (net-port:direction p) 'output)
                    (set! out-port-list (append out-port-list (list p)))
                 )) (cdr (net-circuit-decl:ports circuit)))
           (for-each (lambda (instance)
                    (if (and (equal? default-logical-buf-name (net-instance:name instance))
                             (not-connected-to-output-port (net-instance:instance-connections instance) out-port-list))
                        (begin
                            (set! buff-conn-list 
                                (append buff-conn-list (list (net-instance:instance-connections instance))))
                            (set! buff-index-list (append buff-index-list (list instance-index)))
                        )
                        (for-each (lambda (conn)
                            (if (list? conn)
                                (if (equal? (length conn) 3)
                                    (set! other-conn-list 
                                        (append other-conn-list (list conn)))
                                )
                            )) (net-instance:instance-connections instance))
                    )
                    (if (and (equal? default-logical-buf-name (net-instance:name instance))
                             (not (not-connected-to-output-port (net-instance:instance-connections instance) out-port-list)))
                        (set! total-alias-not-removed (inc total-alias-not-removed))
                    )
                    (set! instance-index (+ 1 instance-index))) instances)
    (cons buff-conn-list other-conn-list)
    )
))


(define deep-de-alias (lambda (connections)
(let* ((updated-aliases (copy-list connections)))
    (for-each (lambda (conn)
       (if (and (key-exist conn alias-hash) (not (member conn de-aliased-list)))
            (begin
                (set! de-aliased-list (append de-aliased-list (list conn)))
                (set! updated-aliases (append updated-aliases (deep-de-alias (hash-ref alias-hash conn))))
                (hash-remove! alias-hash conn)
           )
        )) connections)
    updated-aliases)
))

(define de-alias-connections (lambda (alias-hash)
    (balsa-hash-for-each (lambda (input outputs)
        ;;(print-nl "checking " input " => " outputs)
        (if (not (member input de-aliased-list))
            (hash-set! alias-hash input (deep-de-alias outputs))
        )) alias-hash)
))

;; Remove the aliases
(define remove-aliases (lambda (circuit)
    (balsa-hash-clear! alias-hash)
    (balsa-hash-clear! rev-alias-hash)
    (set! de-aliased-list '())
    (set! buff-index-list '())
    (let* ((buff-conns (get-buff-conn-list circuit))
           (other-conns (cdr buff-conns))
           (not-removable-nodes '()))
          (set! buff-conns (copy-list (car buff-conns))) 
          ;;; buff-conns is a list of buffer connections)
          (for-each (lambda (conn)
                (let ((input  (if l-r-buff? (car conn) (cadr conn)))
                      (output (if l-r-buff? (cadr conn) (car conn))))
                     (cond ((key-exist input alias-hash)
                            (hash-set! alias-hash input (append (hash-ref alias-hash input) (list output))))
                           ((key-exist output alias-hash)
                            (begin
                                (hash-create-handle! 
                                    alias-hash input (append (hash-ref alias-hash output) (list output)))
                                (hash-remove! alias-hash output)
                            ))
                           (else
                            (hash-create-handle! alias-hash input (list output))
                           )
                     ))) buff-conns)
        (de-alias-connections alias-hash)
        (balsa-hash-for-each (lambda (node aliases)
            (for-each (lambda (alias)
                (hash-create-handle! rev-alias-hash alias node)) aliases)) alias-hash)
        (circuit-remove-aliases circuit rev-alias-hash other-conns)
    )
))

(define get-port-loads (lambda (iname ports)
    (let ((port-loads (net-circuit-decl-find-attribute (find-in-simple-parts iname) 'port-load)))
         (if (or (equal? #f port-loads) (equal? '() port-loads)) (begin
             (set! port-loads '())
             (for-each (lambda (port) 
                (if (eqv? 'input (net-port:direction port))
                    (set! port-loads (append port-loads (list 1)))
                    (set! port-loads (append port-loads (list 0)))
                )) ports)
             port-loads)
            (cadr port-loads)
    ))
))
;; 
;; Find all input ports, inputs and outputs of all instances in a circuit
;; uses global variables ci-list (list of input ports), 
;; io-list (list of instance inputs) and ii-list (list of instance outputs)
;;
(define calculate-loads (lambda(circuit)
     (set! ci-list '())
     (set! load-list '())
     (balsa-hash-clear! io-list)
     (balsa-hash-clear! ii-list)
     (balsa-hash-clear! ip-load)
     (balsa-hash-clear! io-load)
     (if balsa-remove-aliases
        (remove-aliases circuit)
     )
     (let ((ports (cdr (net-circuit-decl:ports circuit)))
           (instances (cdr (net-circuit-decl:instances circuit)))
           (i-number 0) ;; index of instance in the list of instances
           (c-number 0)
          ) ;; index of connection in the list of connections
          (set! ci-list ports) ;; all input and output ports
          (for-each (lambda (i)
            (if (eq? (car i) 'instance) 
                ;;(net-display-circuit-decl i)
                (let* ((iname (net-instance:name i))
                       (circuit-instance (find-in-simple-parts iname))
                       (all-ports (cdr (net-circuit-decl:ports circuit-instance))) 
                       (global-ports
                            (let ((g-p (net-circuit-decl-find-attribute circuit-instance 'global-ports)))
                                 (if (not g-p) '() (cdr g-p))
                            )
                        )
                        (temp-ports (reverse (foldl (lambda (port-list port) 
                                        (let ((elem (member (car port) global-ports)))
                                            (if elem
                                                port-list
                                                (cons port port-list)
                                            )
                                        )
                                     ) '() all-ports)))
                       (conns (net-instance:instance-connections i))
                        ;;; Global Variables have to be last declared ports
                       (port-loads (take (get-port-loads iname temp-ports) (length temp-ports)))
                      )
                      ;; if the output of a logical buffer (buff) is connected to an internal node, 
                      ;; its input load should be set to zero when balsa-remove-aliases is active
                      (if (and balsa-remove-aliases (equal? default-logical-buf-name iname))
                          (if (eqv? 'internal (get-node-type (car conns) ci-list))
                              (set! port-loads (list 0 0))
                      ))
                      ;;(print-nl "")
                      ;;(print-nl "instance= " iname)
                      ;;(print-nl "global-ports= " global-ports)
                      ;;(print-nl "temports =" temp-ports)
                      ;;(print-nl "portloads=" port-loads)
                      ;;(print-nl "connections=" conns)
                      (for-each (lambda (conn port port-load)
                        ;(if (not (eq? 'unconnectedx conn))
                            (let* ((smashed-conn '())
                                   (node-type (get-node-type conn ports))
                                   (pin-load (safe-car port-load)))
                                  (if (pair? conn)
                                      (set! smashed-conn (net-smash-instance-connection conn))
                                      (set! smashed-conn (list conn))
                                  )
                                  ;(print-nl conn " smashed = " smashed-conn)  
                                  ;(print-nl "---port dir = " (net-port:direction port))
                                  (if (eq? (net-port:direction port) 'output) ;; is it an instance output
                                    ;; save each output and its instance name and number (order) in list
                                     ;;   (set! io-list (append io-list (list (list conn iname i-number c-number))))
                                    (for-each (lambda (conn) 
                                                (insert-update-value conn (list iname i-number c-number node-type pin-load) io-list))
                                              smashed-conn)
                                    ;;   (set! ii-list (append ii-list (list (list conn iname i-number c-number))))
                                    (for-each (lambda (conn) 
                                                (insert-update-value conn (list iname i-number c-number node-type pin-load) ii-list))
                                              smashed-conn)
                                 )
                            );)
                            (set! c-number (+ c-number 1)) 
                                ) conns  temp-ports port-loads)
                      (set! i-number (+ i-number 1))
                      (set! c-number 0)
                      ;;;(print-nl "done!")
                ))) instances)
      )
))

(define find-ci-io (lambda (circuit)
    (print-nl #\newline "calculating loads...")
    (set! counter-Brz (+ 1 counter-Brz))
    (set! analised-circuit (net-circuit-decl:name circuit))
    (print-nl "Checking circuit " counter-Brz "/" total-Brz-circuits " :" analised-circuit)
    (calculate-loads circuit)
    (balsa-hash-for-each select-signals ii-list)
    (insert-buffers circuit ip-load io-load)
))


;; Insert a buffer tree definition in real gates list to allow safe recursive
;; processing of a .net file
(define insert-btree-def-in-real-gates (lambda (btree)
    (let ((tree-name (net-circuit-decl:name btree)))
         (if (eqv? #f (assoc tree-name real-gates)) ;; not defined yet?
              (let* ((out-buf-name (cadadr (net-circuit-decl:instances btree))) ;; name of top buffer in tree
                     (out-buf-drive (cadr (assoc out-buf-name (map reverse real-buffers))))
                     ;; define "real" tree gate to insert. NOTE dummy pin mapping 
                     (real-tree (list tree-name (list tree-name 0 1) (list out-buf-drive tree-name))))
                     (set! real-gates (append real-gates (list real-tree)))
              )
         )
    )
))



;; Checks if the circuit is a helper cell
(define is-helper-cell? (lambda (circuit)
    (let ((cell-type (net-circuit-decl-find-attribute circuit 'cell-type)))
         (if cell-type
             (string=? (cadr cell-type) "helper")
             #f
         )
    )
))
;;

;; Classify simple parts, handshake (Brz) componentes and Balsa modules
;; simple-circuit-list holds a list of simple part definitions (instances used within a HC)
;; Brz-circuit-list holds the list of all HC components used in the Balsa modules
;;
;; 25-01-07 : sort circuits also adds load attributes to the circuit
;;
(define sort-circuits (lambda (circuit)
      (begin
      ;;(print-nl "classifying " (net-circuit-decl:name circuit))
      ;;(note ".")
      (let  ((cname (net-circuit-decl:name circuit)))
            (cond ((string=? "Brz" (substring cname 0 3))
                        (set! Brz-circuit-list (append Brz-circuit-list (list circuit)))
                  )
                  ((string=? "Bal" (substring cname 0 3))
                        (set! Balsa-circuit-list (append Balsa-circuit-list (list circuit)))
                  )
                  (else
                    (if (not (key-exist cname simple-parts))
                        (begin
                            ;; if it is an inserted tree of buffers, handle it special to allow safe recursive
                            ;; processing: insert its definition in real-gates list
                            (if (string=? "ins" (substring cname 0 3))
                                (insert-btree-def-in-real-gates circuit)
                                (if (not (net-circuit-decl-find-attribute circuit 'port-load))
                                    (add-load-attribute circuit)
                                )
                            ) 
                            (set! simple-circuit-list (append simple-circuit-list (list circuit)))
                            (hash-create-handle! simple-parts cname circuit)
                        )
                            ;(print-nl ":appending to simple parts:" cname "=>" (hash-ref simple-parts cname)))
                    )
                  )
            )
      )
)))



(define total-of-circuits 0)
(define my-net-buffered '())

;; buff is a basic part in almost all circuits
;; this function add buff description and its base instance to
;; the top of the netlist (needed when compiled without including helpers)
;;
(define add-buff-to-simple-parts (lambda (buff-name)
    (let ((buff-circuit (list-copy (net-find-circuit all-mappings buff-name))))
         ;(print-nl (cadadr (net-circuit-decl:instances buff-circuit)))
         (if (> (length (net-circuit-decl:instances buff-circuit)) 1)
             (let* ((base-instance-name (cadadr (net-circuit-decl:instances buff-circuit)))
                    (base-instance (list-copy (net-find-circuit all-mappings base-instance-name))))
                    (add-load-attribute base-instance) 
                   ;;(print-nl "base-instance-name = " base-instance-name)
                   ;;(net-display-circuit-decl base-instance)
                   (set! simple-circuit-list (append simple-circuit-list (list base-instance)))  ; save it
                   (hash-create-handle! simple-parts base-instance-name base-instance)
             )
         )
         (add-load-attribute buff-circuit)
         (set! simple-circuit-list (append simple-circuit-list (list buff-circuit)))  ; save it
         (hash-create-handle! simple-parts buff-name buff-circuit)
    )
   ; (quit)
))

(define insert-b-init-vars (lambda (breeze-tech-dir)
    (begin
        (set! real-gates-f  breeze-gates-mapping-file)
        (for-each (lambda (file)
            (let ((net-file (string-append breeze-tech-dir file ".net")))
                 (set! all-mappings (append (if (file-exists? net-file)
                                            (get-file (string-append net-file))
                                          '()) all-mappings)
                 )
            )) breeze-gates-net-files)
        (set! real-gates (if (file-exists? real-gates-f) 
                             (get-file real-gates-f)
                            '()))
        (set! default-real-buf-name (caadr (find-buf-def "buf" real-gates)))
        (set! default-logical-buf-name (caadr (find-buf-def "connect" real-gates)))
        (set! real-buffers (weighted-real-gates (find-buf-def "buf" real-gates)))
        (set! x-drive-factor tech-x-drive-factor)
        (set! load-threshold tech-load-threshold)
        ;;; logical buffer ports definition
        (set! buff-first-port (cadr (net-circuit-decl:ports (net-find-circuit all-mappings default-logical-buf-name))))
        ;;; reference index for the output pin of logical buffer
        (set! buff-output-ref-index 
            (if (equal? 'output (net-port:direction buff-first-port))
             0
             1
            )
        )
        ;; left-to-right logical buffer?
        (set! l-r-buff? (equal? buff-output-ref-index 1))
)))

;;; main insertion buffer function (must be applied to final-netlist)
;;;
(define net-insert-buffers ( lambda (my-net)
    (begin
        (insert-b-init-vars breeze-tech-dir)
        ;; insert definition of logical buffer (buff) and its dependencies in top of net
        (add-buff-to-simple-parts default-logical-buf-name)
        ;; add load attributes to all helper cells, do not attempt to remove aliases inside helpers
        (set! balsa-remove-aliases #f)
        (for-each add-load-attribute all-mappings)
        (set! balsa-remove-aliases tech-remove-aliases)
        (for-each sort-circuits my-net)
        (set! total-Brz-circuits (length Brz-circuit-list)) 
        (note "Inserting buffers in " total-Brz-circuits " Breeze circuits:"#\nl
              "Doing analisis & insertion with output drive factor = " 
               x-drive-factor ", load threshold = " load-threshold #\nl #\nl
              "    Processing circuits..." #\nl)
        (for-each find-ci-io Brz-circuit-list)
        (note #\nl " Buffer insertion summary:"
              #\nl 
              "    Highest-drive-load      : " (car highest-drive-load) " in "
                   (cadr highest-drive-load) " of circuit " (cddr highest-drive-load) #\nl
              "    Updated instances       : " total-instances-updated #\nl
              "    Insertions made         : " total-signals-buffered  #\nl
              "    Total updates+insertions: " (+ total-instances-updated total-signals-buffered)  #\nl
              "    Buffers added to circuit by type:" #\nl)
        (balsa-hash-for-each (lambda (btype total) (note "     - " btype ": " (inexact->exact total) #\nl)) total-buffers-inserted) 
        (note "    Output ports buffered   : " out-ports-buffered #\nl
              "    Input ports buffered    : " in-ports-buffered #\nl
              "    Aliases removed         : " total-alias-removed #\nl
              "    Aliases not removed     : " total-alias-not-removed #\nl #\nl)
        (append simple-circuit-list Brz-circuit-list Balsa-circuit-list)
    )
))
