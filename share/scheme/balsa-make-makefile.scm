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
;;;	`balsa-make-makefile.scm'
;;;	Balsa "make-depend" Makefile generating script
;;;

(balsa-scheme-import 'brz)
(balsa-scheme-import 'misc 'switches)
(balsa-scheme-import 'misc 'date)
(balsa-scheme-import 'misc 'banners)

;;; brz-rule:{target,dependencies}: field extractors
(define brz-rule:target car)
(define brz-rule:dependencies cdr)

;;; rule<=?: ordering predicate on rules
(define rule<=? (lambda (l r)
	(string<=? (brz-rule:target l) (brz-rule:target r))
))

;;; brz-make-balsa-import-path: make a list of `-I' for balsa-c from a given path list
(define brz-make-balsa-import-path (lambda (path)
	(build-string path "-I " " ")
))

;;; brz-list-imported-blocks: returns a list of blocks imported by the Balsa block
;;;		`block'.  NB. This procedure uses Balsa not Breeze blocks and uses balsa-c
;;;		to find the import list from the Balsa format input files.
(define brz-list-imported-blocks (lambda (block)
	(let*
		((temp-file "./balsa-make-makefile-deps")
		 (rv (system (string-append "balsa-c " (brz-make-balsa-import-path balsa-make-makefile-import-path)
			" -M '" block "' > " temp-file)))
		 (ret (if (zero? rv)
			(car (get-file temp-file))
			(error "brz-list-imported-blocks: can't expand dependencies for block `" block "'")
		 ))
		)
		(system (string-append "rm " temp-file))
		(map (lambda (name) (string-append "[" name "]")) ret)
		;;; ret
	)
))

;;; brz-make-rule-set: add to the list of rules `rules' rules which describe the dependencies
;;;		inherant in the blocks named in the list `to-visit'.  `visited' contains a list of
;;;		already visited blocks which should not be re-visited duirn rules construction.  If
;;;		`dotted-path?' is true then the first (typically the only) element of `to-visit' is
;;;		treated as a dotted path rather than a file path or a square bracketted dotted path.
;;;		Typical use example: (brz-make-rule-set '("mytopfile") brz-library-balsa-blocks '() #f)
;;;		NB. `to-visit' should be ordered
(define brz-make-rule-set (lambda (to-visit visited rules dotted-path?)
	(if (null? to-visit)
		(reverse! rules)
		(let*
			((imports (brz-list-imported-blocks (car to-visit))))
			(brz-make-rule-set ; Add to to-visit, visited, rules
				(union string<=? (cdr to-visit) (differ string<=? (merge-sort string<=? imports) visited))
				(union string<=? (list (car to-visit)) visited)
				(cons (cons (car to-visit) imports) rules)
				#t
			)
		)
	)
))

;;; brz-make-top-rule-set: make a rule set for the top level files `top-files'
(define brz-make-top-rule-set (lambda top-files
	(brz-make-rule-set (merge-sort string<=? top-files) brz-library-balsa-blocks '() #f)
))

;;; brz-flatten-dependencies: make a new list of dependencies which is a flattened version of the
;;;		input dep. list. A flattened dependency list has each target for which a rule exists in
;;;		that rule set expanded to its terminal (non-rule) targets.
;;;		`rules' should contain a rule set for the flattening and `dependencies' should be a
;;;		dependency list for this rule.
;;;		NB. for a single rule r this procedure should be called:
;;;			(brz-flatten-dependencies rules (brz-rule:dependencies rule) '())
(define brz-flatten-dependencies (lambda (rules dependencies visited-deps)
	(if (null? dependencies)
		(unsorted-uniq string=? visited-deps)
		(let
			((sub-rule (assoc (car dependencies) rules))
			 (next (lambda (new-dependencies) (brz-flatten-dependencies rules new-dependencies
				(cons (car dependencies) visited-deps)
			 )))
			)
			(if sub-rule
				(next (unsorted-uniq string=? (append (cdr dependencies) (cdr sub-rule))))
				(next (cdr dependencies))
			)
		)
	)
))

;;; brz-flatten-rule-set: flatten an entire rules set list
(define brz-flatten-rule-set (lambda (rules)
	(map (lambda (rule)
		(cons (brz-rule:target rule)
			(brz-flatten-dependencies rules (brz-rule:dependencies rule) '())
		)
	) rules)
))

;;; brz-block-name->path: find a path/filename to a given block type and return a path which
;;;		has $(BALSAHOME) substituted for initial path components which
;;;		match their current values and an extension as per `target-ext'.  Blocks in the
;;;		current directory have no prefixing path.
;;;		`source-ext' is the extension for the block type to be found.
(define brz-block-name->path (lambda (dotted-path source-ext target-ext)
	(let*
		((balsa-home balsa-home) ; allow options?
		 (path/name/ext (find-filename dotted-path source-ext #t balsa-make-makefile-import-path
			'dotted-path-in-same-dir))
		 (path (car path/name/ext))
		 (name (cadr path/name/ext))
		)
		(string-append (cond
			((string=? path "./") "")
			(else path)
		) name (if (not target-ext) "" (string-append "." target-ext)))
	)
))

;;; print-make-header: print the fancy header ont the make file
(define print-make-header (lambda ()
	(let
		((unix-name (let ((u-name (uname))) (if (vector? u-name) u-name
			#("unknown" "unknown" "unknown" "1" "unknown")))))
		(print "### Balsa Makefile" #\newline)
		(print "### Created: " (ctime) #\newline)
		(print "### By: " (vector-ref (getpw (getuid)) 0)
			"@" (vector-ref unix-name 1) " (" (vector-ref unix-name 0) ")" #\newline)
		(print "### With balsa-make-makefile version: " balsa-version #\newline)
		(print "### Command: balsa-make-makefile " balsa-make-makefile-command-line-string #\newline #\newline)
	)
))

;;; print-variable-definition: print a single variable definition in the form `NAME = value' #\newline
;;;		unless the variable name is pre-defined/undefined.
;;;		NB. If the name of the variable is 'comment then just print the value as a comment; this is usefull
;;;		for inserting the ### Programs / ### Options separators in the makefile with the default rules.
;;;		If the var. name is 'newline then just print a newline.
(define print-variable-definition (lambda (name value)
	(cond
		((eq? 'comment name) (print "### " value #\newline))
		((eq? 'newline name) (newline))
		((member name balsa-make-makefile-undefined-variables)
			(print "# -- " name " left undefined" #\newline)
		)
		((assoc name balsa-make-makefile-defined-variables)
			(print "# -- " name " defined by user" #\newline)
		)
		((string=? "BALSAIMPORTPATH" name) ; handle exception for BALSAIMPORTPATH
			; cut the first (length balsa-search-path) elements from balsa-make-makefile-import-path
			(simple-print-variable-definition name (brz-make-balsa-import-path
				(list-tail balsa-make-makefile-import-path (length balsa-search-path))))
		)
		(else (simple-print-variable-definition name value))
	)
))

;;; simple-print-variable-definition: as above but don't respect -U/-D or 'comment/'newline
(define simple-print-variable-definition (lambda (name value)
	(print name " = ")
	(print (build-separated-string (map bourne-escape-command-line-arg (parse-string value " ")) " ") #\newline)
))

;;; default-variable-definitions: variable definitions which need not be computed but may
;;;		be overridded at the command line to balsa-make-makefile.
;;;		This is a (name value) 2 elem. list suitable for passing to print-variable-definition
;;;		and for assoc'ing
(define default-variable-definitions '(
	(comment "Programs")
	("BALSAC" "balsa-c")
	("BALSAMAKETEST" "balsa-make-test -b")
	("BALSAMAKEIMPLTEST" "balsa-make-impl-test -b")
	("BALSAMAKEBUILTIN" "balsa-make-builtin-lib")
	("BALSASIMIMPL" "balsa-sim-impl -b")
	("BALSANETLIST" "balsa-netlist")
	("BREEZE2PS" "breeze2ps")
	("BREEZESIM" "breeze-sim")
	("BREEZESIMCTRL" "breeze-sim-ctrl")
	("BALSAWAVE" "balsa-wave")
	("MAKE" "make")
	("RM" "rm -f")
	("RM_R" "rm -rf")
	(newline "")
	(comment "Options")
	("BALSAIMPORTPATH" "")
	("BALSACOPTS_COMPLETE" "-b $(BALSAIMPORTPATH) $(BALSACOPTS)")
	("BALSANETLISTOPTS" "$(BALSAIMPORTPATH)")
	(newline "")
	(comment "Implementation technology")
))

;;; test-fixture-variables: variables which are specific to a test harness and so have a 
;;;		`_testHarnessName' suffix when placed.
(define test-fixture-variables '(
	"SIMARGS"
))

;;; implementation-variables: variables generated for each implementation with
;;;		similar name structures to test-fixture-variables
(define implementation-variables '(
	"IMPLPRE" "IMPLOPTS" "IMPLTECH"
))

;;; print-feature-variables: print out variables specific to a list of Makefile
;;;		features (ie. test fixtures or implementations) using the list of variable
;;;		prefices `feature-variables' and the given comment prefix.
;;;		NB. The name of feature must be in (car `feature')
(define print-feature-variables (lambda (features feature-variables comment-prefix)
	(for-each (lambda (feature)
		(newline)
		(print "### " comment-prefix " " (car feature) " variables" #\newline)
		(for-each (lambda (variable)
			(print-variable-definition (string-append variable "_" (replace-char (car feature) #\- #\_)) "")
		) feature-variables)
	) features)
))

;;; print-make-definitions: print the variable/suffix rule definitions block
(define print-make-definitions (lambda ()
	(for-each (lambda (var-defn) (apply print-variable-definition var-defn)) default-variable-definitions)
	; Print SIMARGS
	(print-feature-variables balsa-make-makefile-test-fixtures
		test-fixture-variables "Test fixture simulation variables")
	; Print the IMPLOPTS_<name> variable
	(print-feature-variables balsa-make-makefile-implementations implementation-variables "Implementation")
	; Print user (re)defined variables
	(if (not (null? balsa-make-makefile-defined-variables))
		(begin
			(newline)
			(print "### User defined variables" #\newline)
			(for-each (lambda (var-defn) (apply simple-print-variable-definition var-defn))
				(reverse balsa-make-makefile-defined-variables))
		)
	)
	(newline)
	(print "### Suffix rules" #\newline)
	(print ".SUFFIXES: .balsa .breeze .ps" #\newline)
	(newline)
	(print ".breeze.ps:" #\newline)
	(print #\ht "$(BREEZE2PS) $(BALSAIMPORTPATH) $*" #\newline)
	(print ".balsa.breeze:" #\newline)
	(print #\ht "$(BALSAC) $(BALSACOPTS_COMPLETE) $*" #\newline)

	(balsa-make-makefile-add-to-target-help "suffix rule .ps" "Generate a PostScript graph of a Breeze file")
	(balsa-make-makefile-add-to-target-help "suffix rule .breeze" "Compile Balsa to Breeze")
))

;;; print-balsa-rules: print `target: dependencies' rules for only those
;;;		rules in the given `rules' which have dependencies.  This procedure makes
;;;		breeze targets from breeze dependencies and produces no actions as these are
;;;		handled by the balsa->breeze suffix rule.
(define print-balsa-rules (lambda (rules)
	(let
		((block-names (lambda (blocks ext)
		 	(map (lambda (n) (brz-block-name->path n "balsa" ext)) blocks)
		)))
		(for-each (lambda (rule)
			(if (not (null? (brz-rule:dependencies rule)))
				(print (brz-block-name->path (brz-rule:target rule) "balsa" "breeze") ": "
					(build-separated-string (block-names (brz-rule:dependencies rule) "breeze") " ")
					#\newline
				)
			)
		) rules)
	)
))

;;; print-breezesim-rules: print breezesim `target: dependencies' rules.
(define print-breezesim-rules (lambda (rules)
	(let
		((block-names (lambda (blocks ext)
		 	(map (lambda (n) (brz-block-name->path n "balsa" ext)) blocks)
		)))
		(for-each (lambda (rule)
			(print (brz-block-name->path (brz-rule:target rule) "balsa" "breezesim") ": "
				(build-separated-string (block-names (brz-rule:dependencies rule) "breeze") " ")
				#\newline 
				#\ht "breeze-link "
				(build-separated-string (block-names (append (brz-rule:dependencies rule)
					(list (brz-rule:target rule))) "") " ")
				#\newline #\newline
			)
		) rules)
	)
))

;;; print-test-fixture: print out the make(1) rules for a single test fixture using
;;;		breeze-sim and the test harness generation using balsa-make-test
(define print-test-fixture (lambda (fixture flat-rules)
	(let*
		((name (car fixture))
		 (block-name (cadr fixture))
		 (third-name (caddr fixture)) ; proc-name or file-name
		 (breeze-block-name (brz-block-name->path block-name "balsa" "breeze")) ; expanded block name
		 (block-rule (find-with-predicate flat-rules (lambda (r) (string=? (brz-rule:target r) block-name))))
		 (test-fixture-file-name (string-append "test-" name))
		 (pre-rule (string-append "sim-pre-" name))
		 (sim-rule (string-append "sim-" name))
		 (sim-win-rule (string-append "sim-win-" name))
		)	
		(if (not block-rule)
			(error "balsa-make-makefile: can't find a block with name `" block-name
				"'. Are you using filenames/dotted-paths consistently?")
		)
        ; breezesim test harness rules:

		(print test-fixture-file-name ".balsa: " breeze-block-name " Project"#\newline)
		(print #\ht "$(BALSAMAKETEST) $(BALSAIMPORTPATH) -p . " name #\newline)
		(newline)

		(print pre-rule ": " test-fixture-file-name ".breeze" #\newline)
		(newline)

		(balsa-make-makefile-add-phony (list pre-rule sim-rule sim-win-rule))

		(balsa-make-makefile-add-to-target-help (string-append sim-rule ", " sim-win-rule)
			(string-append "Simulate procedure " third-name " in block " block-name))

		(print sim-rule ": " pre-rule #\newline)
		(print #\ht "$(BREEZESIM) $(BALSAIMPORTPATH) $(SIMARGS_" name ") "
			test-fixture-file-name #\newline)
		(newline)

		(print sim-win-rule ": " pre-rule #\newline)
		(print #\ht "$(BREEZESIMCTRL) $(BALSAIMPORTPATH) $(SIMARGS_" name ") "
			test-fixture-file-name #\newline)
		(newline)
	)
))

;;; print-test-fixtures: print out test-fixture and test-fixtures rules
(define print-test-fixtures (lambda (top-blocks top-breeze-files flat-rules)
	(for-each (lambda (fixture) ; '(name block proc-name)
		(print-test-fixture fixture flat-rules)
	) balsa-make-makefile-test-fixtures)
))

;;; balsa-make-makefile-print-help-rule: pretty print usage help
(define balsa-make-makefile-print-help-rule (lambda ()
	(let
		((max-rule-name-length (fold (lambda (rule len)
			(let ((rule-name-length (string-length (car rule))))
				(max rule-name-length len)
			)
		 ) 0 balsa-make-makefile-target-help))
		 (max-explanation-length (fold (lambda (rule len)
			(let ((rule-explanation-length (string-length (cdr rule))))
				(max rule-explanation-length len)
			)
		 ) 0 balsa-make-makefile-target-help))
		 (sorted-rules (merge-sort (lambda (l r) (string<=? (car l) (car r))) balsa-make-makefile-target-help))
		)
		(print "help:")
		(newline)
		(print #\ht "@echo 'Target " (make-string (- max-rule-name-length 6) #\space) "Meaning'")
		(newline)
		(print #\ht "@echo '" (make-string max-rule-name-length #\-) " "
			(make-string max-explanation-length #\-) "'")
		(newline)
		(for-each (lambda (rule)
			(print #\ht "@echo '" (string-append (car rule)
				(make-string (- max-rule-name-length (string-length (car rule))) #\space)
				" " (cdr rule)) "'")
			(newline)
		) sorted-rules)
		(print #\ht "@echo")
		(newline)
		(newline)
	)
))

;;; print-misc-rules: print rules for make file regeneration and cleaning up generated files.
(define print-misc-rules (lambda (top-blocks all-blocks)
	(let*
		((block->file (lambda (n) (brz-block-name->path n "balsa" "")))
		 (files (map block->file all-blocks))
		 (dotted-files (map (lambda (n) (string-append (brz-trim-square-brackets n) ".")) all-blocks))
		 (top-files (map block->file top-blocks))
		)
		(print "depend:" #\newline)
		(print #\ht "balsa-make-makefile " balsa-make-makefile-command-line-string #\newline)
		(newline)
		(print "clean:" #\newline)

		(for-each (lambda (clean)
        	(print #\ht "$(RM)")
			(for-each (lambda (file) (print " " file)) clean)
       		(newline)
		) balsa-make-makefile-clean-rules)

		(for-each (lambda (file-type)
			(print #\ht "$(RM)" (build-string files " " file-type) #\newline)
		) '("breeze" "ps"))

		(for-each (lambda (fixture)
			(print #\ht "$(RM)")
			(for-each (lambda (file-type)
				(print " test-" (car fixture) file-type)
			) '(".breeze"))
			(newline)
		) balsa-make-makefile-test-fixtures)

		; Remove implementation generated files
		(for-each (lambda (impl)
			(let*
				((impl-name (car impl))
				 (list-file (string-append "impl-" impl-name ".lst"))
				)
				(print #\ht "$(RM) impl-" impl-name ".log" #\newline)
				(print #\ht "if test -s " list-file "; then \\" #\newline)
				(print #\ht "cat " list-file " | while read file; do $(RM) $$file; done; \\" #\newline)
				(print #\ht "$(RM) " list-file "; \\" #\newline)
				(print #\ht "fi" #\newline)
				(balsa-make-makefile-add-to-target-help (string-append "impl-" impl-name) (string-append
					"Implement block " (cadr impl) " using rules " impl-name))
			)
		) balsa-make-makefile-implementations)
		(newline)
		(print "very-clean: clean" #\newline)

		(for-each (lambda (clean)
        	(print #\ht "$(RM)")
			(for-each (lambda (file) (print " " file)) clean)
        	(newline)
		) balsa-make-makefile-very-clean-rules)

		; automagically generated test harness files
		(for-each (lambda (fixture)
			(print #\ht "$(RM) test-" (car fixture) ".balsa" #\newline)
		) balsa-make-makefile-test-fixtures)
		(for-each (lambda (fixture)
			(print #\ht "$(RM)")
			(for-each (lambda (file-type)
				(print " test-" (car fixture) file-type)
			) '(".hhh"))
            (newline)
		) balsa-make-makefile-test-fixtures)
        (print #\ht "$(RM_R) .libs" #\newline)
		(newline)
		(print "ps:" (build-string top-files " " "ps"))
		(newline)
		(newline)
		(print "cost:" (build-string top-files " " "breeze"))
		(newline)
		(for-each (lambda (top-file)
			(print #\ht "breeze-cost " top-file "breeze")
			(newline)
		) top-files)
		(newline)
		(balsa-make-makefile-add-to-target-help "clean" "Remove auto-generated files (except test harnesses)")
		(balsa-make-makefile-add-to-target-help "very-clean" "Remove all auto-generated files")
		(balsa-make-makefile-add-to-target-help "cost" "Estimate Breeze block costs")
		(balsa-make-makefile-add-to-target-help "ps" "Generate PostScript graphs for a Breeze files")
		(balsa-make-makefile-print-help-rule)
	)
))

;;; print-implementation: print balsa-make-makefile rules for a single implementation
(define print-implementation (lambda (impl)
	(let*
		((name (car impl))
		 (var-name (replace-char name #\- #\_))
		 (block-name (cadr impl))
		 (new-block-name (caddr impl))
		 (breeze-block-name (brz-block-name->path block-name "balsa" "breeze")) ; expanded block name
		 (impl-rule-name (string-append "impl-" name))
		 (new-breeze-name (if new-block-name (brz-block-name->path new-block-name "balsa" "breeze") #f))
		 (new-block-name-arg (if new-block-name
		 	(string-append " --basename " (brz-block-name->path new-block-name "balsa" #f) " ")
		 	""
		 ))
		)
		(print impl-rule-name " " impl-rule-name ".log " impl-rule-name ".lst: "
			breeze-block-name #\newline)
		(print #\ht "$(BALSANETLIST) --technology '$(IMPLTECH_" var-name ")' \\" #\nl #\ht
			"    $(BALSANETLISTOPTS) $(IMPLPRE_" var-name ") $(IMPLOPTS_" var-name ")"
			new-block-name-arg "'" block-name "'" #\newline)
		(newline)
		(balsa-make-makefile-add-phony impl-rule-name)
	)
))

;;; print-simulation: print balsa-make-makefile rules for a single simulation
(define print-simulation (lambda (sim)
	(let*
		((test-name (car sim))
		 (impl-name (cadr sim))
		 (test-impl impl-name)
		 (var-name (replace-char test-impl #\- #\_))
		 (pre-rule (string-append "sim-pre-" test-impl))
		 (sim-rule (string-append "sim-" test-impl))
		)
		(print "sim-pre-" test-impl ": impl-" test-impl ".log" #\newline)
		(print #\ht "$(BALSAMAKEIMPLTEST) --technology $(IMPLTECH_" var-name ") \\" #\nl #\ht
			   "    $(BALSAIMPORTPATH) $(IMPLTEST_" var-name ") $(IMPLTESTDUMP_" var-name ") -t -T -o test-" test-impl "-top -i impl-" test-impl " test-" test-name
			" balsa" #\newline)
		(newline)
		(print "sim-" test-impl ": sim-pre-" test-impl #\newline)
		(print #\ht "$(BALSASIMIMPL) --technology $(IMPLTECH_" var-name ") \\" #\nl #\ht)
		(print #\ht "    $(BALSAIMPORTPATH) -B test-" test-name " $(SIMARGS_" test-name ") "
			"test-" test-impl "-top" #\newline)
		(newline)
		(balsa-make-makefile-add-phony (list pre-rule sim-rule))
		(balsa-make-makefile-add-clean #t (string-append "test-" test-impl "-top.*"))
		(balsa-make-makefile-add-clean #f (string-append "impl-" test-impl ".breeze"))
		(balsa-make-makefile-add-clean #t (string-append "${IMPLTESTDUMPFILENAME_" var-name "}"))
		(balsa-make-makefile-add-to-target-help (string-append "sim-" test-impl) (string-append
			"Simulate implementation " impl-name " in the same way as test " test-name))
	)
))

;;; print-builtin: print balsa-make-makefile rules for a user written builtin lib
(define print-builtin (lambda (builtin)
	(let*
		((builtin-name (car builtin))
		 (builtin-sources (cadr builtin))
		 (sources-string (build-separated-string builtin-sources " "))
		)
		(print builtin-name ".la: " sources-string #\newline)
		(print #\ht "$(BALSAMAKEBUILTIN) " builtin-name " " sources-string #\newline)
		(print builtin-name ".breeze: " builtin-name ".la" #\newline)
		(newline)
		(balsa-make-makefile-add-to-target-help (string-append builtin-name ".la")
			(string-append "Build builtin library " builtin-name " from " sources-string))
		(balsa-make-makefile-add-clean #t (string-append builtin-name ".la"))
		(balsa-make-makefile-add-clean #t (string-append builtin-name ".so*"))
		(balsa-make-makefile-add-clean #t (string-append builtin-name ".a"))
		(balsa-make-makefile-add-clean #t (string-append builtin-name ".o"))
		(balsa-make-makefile-add-clean #t (string-append builtin-name ".lo"))
	)
))

;;; print-phony-rules: print a list of phony rules
(define print-phony-rules (lambda (phonies)
	(print ".PHONY:")
	(for-each (lambda (phony) (print " " phony)) phonies)
	(newline)
))

;;; balsa-make-makefile-{no-banner,...}: command line switches
(define balsa-make-makefile-no-banner #f)
(define balsa-make-makefile-makefile-name "Makefile")
(define balsa-make-makefile-test-fixtures '()) ; list of (name block-name proc-name) triples
; (name block-name file-name) triples for predefined balsa-make-test test fixtures
(define balsa-make-makefile-command-line-string #f)
(define balsa-make-makefile-defined-variables '())
(define balsa-make-makefile-undefined-variables '())
(define balsa-make-makefile-project-name "project")
(define balsa-make-makefile-files '())
(define balsa-make-makefile-import-path balsa-search-path)
(define balsa-make-makefile-implementations '()) ; list of (name block-name real-block-name)
(define balsa-make-makefile-builtins '()) ; list of (name c-files)
(define balsa-make-makefile-simulations '()) ; list of (test-name impl-name)
(define balsa-make-makefile-simulation-system "interpreter")
(define balsa-make-makefile-phony-rules '(("depend" "clean" "very-clean" "ps" "cost" "help")))
(define balsa-make-makefile-clean-rules '())
(define balsa-make-makefile-very-clean-rules '())

;;; balsa-make-makefile-add-phony: add a single, or a list of phony rules
(define balsa-make-makefile-add-phony (lambda (phonies)
	(set! balsa-make-makefile-phony-rules
		(cons (if (pair? phonies) phonies (list phonies)) balsa-make-makefile-phony-rules))
))

;;; balsa-make-makefile-add-clean: add a single, or a list of clean or very clean rules rules
(define balsa-make-makefile-add-clean (lambda (very? phonies)
	(if very?
		(set! balsa-make-makefile-very-clean-rules
			(cons (if (pair? phonies) phonies (list phonies)) balsa-make-makefile-very-clean-rules))
		(set! balsa-make-makefile-clean-rules
			(cons (if (pair? phonies) phonies (list phonies)) balsa-make-makefile-clean-rules))
	)
))

(define balsa-make-makefile-printed-banner #f)

;;; balsa-make-makefile-print-banner: print the balsa-make-makefile banner
(define balsa-make-makefile-print-banner (lambda ()
	(if (not balsa-make-makefile-printed-banner)
		(make-program-banner "balsa-make-makefile" "balsa-make-makefile: Makefile generator"
			"2003-2008, The University of Manchester")
	)
	(set! balsa-make-makefile-printed-banner #t)
))

;;; balsa-make-makefile-usage: command usage
(define balsa-make-makefile-usage (lambda ()
	(balsa-make-makefile-print-banner)
	(error
		"version " balsa-version #\newline
		"usage: balsa-make-makefile {<switch>}* (must include -p)" #\newline #\newline
		"switches: -o <file-name>     - Name of target makefile (--output-file)" #\newline
		"          -h or -?           - Display this message (--help)" #\newline
		"          -b                 - Don't print the balsa-make-makefile banner (--no-banner)" #\newline
		"          -U <variable>      - Don't define the named variable in the generated file (--undefine)" #\newline
		"          -D <var> <value>   - Define the variable <var> with value <value> (--define)" #\newline
		"          -p <directory>     - Read the Project file out of the given directory and use" #\newline
		"                               that as the basis of the Makefile (--project)" #\newline
	)
))

;;; balsa-make-makefile-add-test-fixture: add a test fixture defn. to the global balsa-make-makefile-test-fixtures
;;;		list.  Canonicalise the given block name
(define balsa-make-makefile-add-test-fixture (lambda (test-name block-name proc-name)
	(cond
		((assoc test-name balsa-make-makefile-test-fixtures)
			(error "balsa-make-makefile: test fixture `" test-name "' already defined"))
		(else
			(set! balsa-make-makefile-test-fixtures
				(cons
					(list
						test-name
						(brz-canonicalise-block-name block-name)
						proc-name
					)
					balsa-make-makefile-test-fixtures
				)
			)
		)
	)
))

;;; balsa-make-makefile-add-implementation: add a scheduled run of balsa-netlist under the
;;;		rule impl-`impl-name' to a list of such runs.  This run of balsa-netlist will
;;;		be on block `block-name'.  Canonicalise the given block name
(define balsa-make-makefile-add-implementation (lambda (impl-name block-name new-block-name)
	(cond
		((assoc impl-name balsa-make-makefile-implementations)
			(error "balsa-make-makefile: implementation `" impl-name "' already defined"))
		(else
			(set! balsa-make-makefile-implementations
				(cons
					(list
						impl-name
						(brz-canonicalise-block-name block-name)
						(if new-block-name
							(brz-canonicalise-block-name new-block-name)
							#f
						)
					)
					balsa-make-makefile-implementations
				)
			)
			; generate log file impl-<name>.log
			(balsa-make-makefile-define-variable
				(string-append "IMPLPRE_" impl-name)
				(string-append "-l impl-" impl-name ".lst -L impl-" impl-name ".log")
			)
		)
	)
))

;;; balsa-make-makefile-add-simulation: add a scheduled simulation run (using a
;;;		simulator other than breeze-sim, through balsa-sim-impl)
;;;		rule sim-`test-name'-`impl-name' to a list of such runs.
(define balsa-make-makefile-add-simulation (lambda (test-name impl-name)
	(cond
		((assoc impl-name balsa-make-makefile-simulations)
			(error "balsa-make-makefile: simulation `" test-name "-" impl-name "' already defined"))
		(else
			(set! balsa-make-makefile-simulations
				(cons
					(list
						test-name
						impl-name
					)
					balsa-make-makefile-simulations
				)
			)
			(balsa-make-makefile-define-variable
				(string-append "IMPLTEST_" impl-name)
				""
			)
			(balsa-make-makefile-define-variable
				(string-append "IMPLTESTDUMP_" impl-name)
				""
			)
		)
	)
))

;;; balsa-make-makefile-define-variable: add a definition for a variable in balsa-make-makefile-defined-variable
(define balsa-make-makefile-define-variable (lambda (name value)
	(let ((var-name (replace-char name #\- #\_)))
		(if (not (assoc var-name balsa-make-makefile-defined-variables))
			(set! balsa-make-makefile-defined-variables
				(cons (list var-name value) balsa-make-makefile-defined-variables)
			)
		)
	)
))

;;; balsa-make-makefile-set-simulation-system: check for a valid argument and set balsa-make-makefile-simulation-system
(define valid-simulation-systems '("breezesim"))
(define balsa-make-makefile-set-simulation-system (lambda (name)
	(if (member name valid-simulation-systems)
		(set! balsa-make-makefile-simulation-system name)
		(error "balsa-make-makefile: invalid simulation system. Valid values:" valid-simulation-systems)
	)
))

;;; balsa-make-makefile-target-help: pairs of (target-name . explanation) for each target made in this
;;;		Makefile dumped as the make target `help'
(define balsa-make-makefile-target-help '())

;;; balsa-make-makefile-add-to-target-help: add a single target to balsa-make-makefile-target-help
(define balsa-make-makefile-add-to-target-help (lambda (target help-string)
	(set! balsa-make-makefile-target-help (cons (cons target help-string) balsa-make-makefile-target-help))
))

;;; balsa-make-makefile-command-line-rules: command-line-args action rules
(define balsa-make-makefile-command-line-rules `(
	(#\o "output-file" 1 ,(lambda (args) (set! balsa-make-makefile-makefile-name (car args))))
	(#\b "no-banner" 0 ,(lambda (args) (set! balsa-make-makefile-no-banner #t)))
	(#\U "undefine" 1 ,(lambda (args) 
		(set! balsa-make-makefile-undefined-variables (cons (car args) balsa-make-makefile-undefined-variables))
	))
	(#\D "define" 2 ,(lambda (args) 
		(balsa-make-makefile-define-variable (car args) (cadr args))
	))
	(#\p "project" 1 ,(lambda (args) (balsa-make-makefile-read-project-file (car args))))
	(#\h "help" 0 ,(lambda (args) (balsa-make-makefile-usage)))
	(#\? "help" 0 ,(lambda (args) (balsa-make-makefile-usage)))
))

;;; balsa-make-makefile-parse-command-line: parse switches from the given command line list, set
;;;		the balsa-make-makefile-... globals and return the tail of the command line.
(define balsa-make-makefile-parse-command-line (lambda (args)
	(set! balsa-make-makefile-command-line-string
		(build-separated-string (map bourne-escape-command-line-arg args) " "))
	(if (null? args)
		(balsa-make-makefile-usage)
		(if (not (null? (parse-command-line "balsa-make-makefile" balsa-make-makefile-command-line-rules
			balsa-make-makefile-usage args)))
			(balsa-make-makefile-usage)
		)
	)
))

;;; balsa-make-makefile-read-project-file:test: read a test fixture desc. from the project file element test
;;;		This procedure also adds any implementations created by this test to the implementation list
(define balsa-make-makefile-read-project-file:test (lambda (file test)
	(let*
		((syntax-error (lambda () (error "balsa-make-makefile-read-project-file:test: error in Project file")))
		 (test-name (cadr test))
		 (proc-name (caddr test))
		 (fixture (list test-name file proc-name))
		 (implementations (find-headed-sub-list test 'implementations))
		 (command-line-options (find-headed-sub-list test 'command-line-options))
		)
		; Ports are left upto balsa-make-test to work on but implementations within tests are resolved here
		(if implementations
			(for-each (lambda (impl)
				(let
					((impl (balsa-make-makefile-read-project-file:implementation
						(string-append "test-" test-name) impl test-name)))
					(if impl (begin
						(apply balsa-make-makefile-add-implementation impl)
						(balsa-make-makefile-add-simulation test-name (car impl))
					))
				)
			) (cdr implementations))
		)
		(if command-line-options
			(balsa-make-makefile-define-variable (string-append "SIMARGS_" test-name)
				(build-separated-string (cdr command-line-options) " "))
		)
		fixture
	)
))

;;; balsa-make-makefile-read-project-file:implementation: read an implementation desc. from the project
;;;		file element impl.  If prefix isn't #f then call the implementation "prefix-(cadr impl)"
(define balsa-make-makefile-read-project-file:implementation (lambda (file impl prefix)
	(let*
		((syntax-error (lambda () (error "balsa-make-makefile-read-project-file:implementation: error in Project file")))
		 (impl-name (if prefix (string-append prefix "-" (cadr impl)) (cadr impl)))
		 (var-name (replace-char impl-name #\- #\_))
		 (implementation (list impl-name file (if prefix (string-append "impl-" impl-name) #f)))
		)
		(for-each (lambda (elem)
			(if (headed-list? elem)
				(case (car elem)
					((technology)
						(if (/= 2 (length elem))
							(syntax-error)
							(balsa-make-makefile-define-variable
								(string-append "IMPLTECH_" impl-name)
								(cadr elem)
							)
						)
					)
					((balsa-netlist-options)
						(if (/= 2 (length elem))
							(syntax-error)
							(balsa-make-makefile-define-variable
								(string-append "IMPLOPTS_" impl-name)
								(cadr elem)
							)
						)
					)
					((make-impl-test-options)
						(if (/= 2 (length elem))
							(syntax-error)
							(balsa-make-makefile-define-variable
								(string-append "IMPLTEST_" impl-name)
								(cadr elem)
							)
						)
					)
					((dump-file)
						(if (/= 2 (length elem))
							(syntax-error)
                            (begin
                              (balsa-make-makefile-define-variable
								(string-append "IMPLTESTDUMPFILENAME_" impl-name)
								(cadr elem)
							  )
							  (balsa-make-makefile-define-variable
								(string-append "IMPLTESTDUMP_" impl-name)
								; FIXME, insert orig. variable value
								(string-append "-D ${IMPLTESTDUMPFILENAME_" var-name "}")
							  )
                            )
						)
					)
				)
				(syntax-error)
			)
		) (cddr impl))
		implementation
	)
))

;;; balsa-make-makefile-read-project-file:file: read a file description from the project file element file
;;;		and update the global variables
(define balsa-make-makefile-read-project-file:file (lambda (file)
	(let
		((syntax-error (lambda () (error "balsa-make-makefile-read-project-file:file: error in Project file")))
		 (add-file (lambda (filename)
			(set! balsa-make-makefile-files (cons (brz-canonicalise-block-name filename) balsa-make-makefile-files))
		 ))
		)
		(if (string? file)
			(if (not (find-filename file "balsa" #t balsa-make-makefile-import-path))
				(error "balsa-make-makefile-read-project-file: invalid block name `" (cadr file) "'")
				(add-file file)
			)
			(if (headed-list? file 'top-level)
				(if (find-filename (cadr file) "balsa" #t balsa-make-makefile-import-path)
					(begin
						(add-file (cadr file))
						(for-each (lambda (file-elem)
							(if (headed-list? file-elem)
								(case (car file-elem)
									((test) ; test fixture desc.
										(let
											((test (balsa-make-makefile-read-project-file:test
												(cadr file) file-elem)))
											(if test
												(apply balsa-make-makefile-add-test-fixture test)
											)
										)
									)
									((lard-test-harness)
										(error "balsa-make-makefile: LARD test fixtures don't work in Balsa anymore. Sorry.")
									)
									((implementation) ; balsa-netlist implementation
										(let
											((impl (balsa-make-makefile-read-project-file:implementation
												(cadr file) file-elem (cadr file))))
											(if impl
												(apply balsa-make-makefile-add-implementation impl)
											)
										)
									)
									((builtin-lib)
										(balsa-make-makefile-read-project-file:builtin-lib (cadr file) file-elem)
									)
								)
								(syntax-error)
							)
						) (cddr file)) 
					)
					(error "balsa-make-makefile-read-project-file: invalid block name `" (cadr file) "'") 
				)
				(syntax-error)
			)
		)
	)
))

;;; balsa-make-makefile-read-project-file:builtin-lib: read a builtin-lib desc. from the project
;;;		file element impl.  This directly adds to balsa-make-makefiles-builtins
(define balsa-make-makefile-read-project-file:builtin-lib (lambda (file builtin)
	(let*
		((syntax-error (lambda () (error "balsa-make-makefile-read-project-file:builtin-lib: error in Project file")))
		 (builtin-name (cadr builtin))
		)
		(for-each (lambda (elem)
			(if (headed-list? elem)
				(case (car elem)
					((source-files)
						(set! balsa-make-makefile-builtins
							(cons
								(list builtin-name (cdr elem))
								balsa-make-makefile-builtins
							)
						)
						;(balsa-make-makefile-define-variable
						;	(string-append "BUILTINSRC_" builtin-name)
						;	(build-separated-string (cdr elem) " ")
						;)
					)
				)
				(syntax-error)
			)
		) (cddr builtin))
	)
))

;;; balsa-make-makefile-read-project-file: read the file Project from the given directory
;;;		and fill the global variables from that file
(define balsa-make-makefile-read-project-file (lambda (directory)
	(let*
		((project (car (get-file (string-append directory "/Project"))))
		 (syntax-error (lambda () (error "balsa-make-makefile-read-project-file: error in Project file `"
			directory "/Project'")
		 ))
		)
		(chdir directory) ; change to that directory
		(if (headed-list? project 'balsa-project)
			(for-each (lambda (elem)
				(if (headed-list? elem)
					(case (car elem)
						((name) (set! balsa-make-makefile-project-name (cadr elem)))
						; directory, ignore
						((import-path)
							(balsa-make-makefile-define-variable "BALSAIMPORTPATH"
								(brz-make-balsa-import-path (cdr elem)))
							(set! balsa-make-makefile-import-path (append balsa-search-path (cdr elem)))
						)
						((files) (for-each balsa-make-makefile-read-project-file:file (cdr elem)))
						((technology)
							(print-err "balsa-make-makefile: use of (technology ...) "
								"node at top of Project file is deprecated" #\newline)
						)
						((simulation-system)
							(balsa-make-makefile-set-simulation-system (cadr elem))
						)
						((defines)
							(for-each (lambda (elem)
								(if (/= 3 (length elem)) (syntax-error))
								(balsa-make-makefile-define-variable (cadr elem) (caddr elem))
							) (cdr elem))
						)
					)
					(syntax-error)
				)
			) (cdr project))
			(syntax-error)
		)
	)
))

;;; brz-balsa-make-makefile: make a makefile for the top Balsa blocks `top-blocks'
(define balsa-make-makefile (lambda top-blocks
	; (if (null? top-blocks) (balsa-make-makefile-usage))
	(let*
		((top-blocks-w/o-ext
			(map (lambda (block)
				(let ((trimmed (final-string block ".balsa")))
					(if (strchr block #\/)
						(error "balsa-make-makefile: `" block
							"' use a dotted-path for a Balsa block in a different directory")
					)
					(brz-canonicalise-block-name (if trimmed trimmed block))
				)
			) top-blocks)
		 )
		 (rules (apply brz-make-top-rule-set top-blocks-w/o-ext))
		 (filtered-rules (map (lambda (rule)
			(cons (brz-rule:target rule)
				(filter (lambda (e) (not (member e brz-library-balsa-blocks))) (brz-rule:dependencies rule))
			)
		 ) rules))
		 (flat-rules (brz-flatten-rule-set rules))
		 (block-names (lambda (blocks ext)
		 	(map (lambda (n) (brz-block-name->path n "balsa" ext)) blocks)
		 ))
		 (top-breeze-files (block-names top-blocks-w/o-ext "breeze"))
		)
		(with-output-to-file balsa-make-makefile-makefile-name (lambda ()
			(print-make-header)
			(print "### Must use a Bourne shell" #\newline)
			(print "SHELL = /bin/sh" #\newline)
			(newline)
			(print-make-definitions)
			(newline)
			(print "all: " (build-string top-breeze-files "" " "))
			(newline)
			(print "### Balsa rules" #\newline)
			(print-balsa-rules filtered-rules)
			(newline)
			(print "### Test harness rules" #\newline)
			(print-test-fixtures top-blocks-w/o-ext top-breeze-files flat-rules)
			(print "### Implementation rules" #\newline)
			(for-each (lambda (impl) (print-implementation impl)) balsa-make-makefile-implementations)
			(print "### Implementation simulations" #\newline)
			(for-each (lambda (impl) (print-simulation impl)) balsa-make-makefile-simulations)
			(if (not (null? balsa-make-makefile-builtins))
				(begin
					(print "### Builtin libs" #\newline)
					(for-each (lambda (builtin) (print-builtin builtin)) balsa-make-makefile-builtins)
				)
			)
			(print "### Miscellaneous rules" #\newline)
			(for-each (lambda (phonies) (print-phony-rules phonies)) balsa-make-makefile-phony-rules)
			(newline)
			(print-misc-rules top-blocks-w/o-ext (fold (lambda (rule blocks)
				(union string<=? blocks (merge-sort string<=? rule))
			) '() filtered-rules))
		))
	)
))

(top-level (lambda (args)
	(balsa-make-makefile-parse-command-line args)
	(if (not balsa-make-makefile-no-banner)
		(balsa-make-makefile-print-banner)
	)
	(apply balsa-make-makefile balsa-make-makefile-files)
) (cdr command-line-args))
