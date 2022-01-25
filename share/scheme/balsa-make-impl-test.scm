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
;;;	`balsa-make-impl-test.scm'
;;;	Test harness generator for implementation netlists
;;;

(balsa-scheme-import 'brz)
(balsa-scheme-import 'misc 'switches)
(balsa-scheme-import 'misc 'date)
(balsa-scheme-import 'misc 'banners)
(balsa-scheme-import 'brz 'tech)
(balsa-scheme-import 'net 'verilog)
(balsa-scheme-import 'gen 'gates)

;;; print-impl-test-header: print the fancy file header
(define print-impl-test-header (lambda ()
	(let
		((unix-name (let ((u-name (uname))) (if (vector? u-name) u-name
			#("unknown" "unknown" "unknown" "1" "unknown")))))
		(print "/*" #\newline)
		(print "    Balsa Verilog test harness" #\newline)
		(print "    Created: " (ctime) #\newline)
		(print "    By: " (vector-ref (getpw (getuid)) 0)
			"@" (vector-ref unix-name 1) " (" (vector-ref unix-name 0) ")" #\newline)
		(print "    With balsa-make-impl-test version: " balsa-version #\newline)
		(print "    Command: balsa-make-impl-test " impl-test-command-line-string #\newline)
		(print "*/" #\newline #\newline)
	)
))

;;; print-impl-test-simulation-header: print the simulation initialisation lines
(define print-impl-test-simulation-header (lambda ()
	(print "`define balsa_simulate" #\newline)
	(print "`define balsa_init_time " impl-test-reset-delay #\newline)
	(print "`define balsa_activate_delay " impl-test-activate-delay #\newline)
	(print "`define balsa_response_delay " impl-test-response-delay #\newline)
	(print "`define balsa_sampling_delay " impl-test-sampling-delay #\newline)
	(print "`define balsa_rtz_delay " impl-test-rtz-delay #\newline)
	(print "`define balsa_clock_high_delay " impl-test-clock-high-delay #\newline)
	(print "`define balsa_clock_low_delay " impl-test-clock-low-delay #\newline)
))

;;; impl-test-summarise-port: summarise port info, expanding arrayed ports.  Return
;;;		value is a list of (port arrayed-index . portions)
;;;		where `portions' is a list of (portion-name direction cardinality) for the expanded
;;;		portions of the port in which `direction' is (either input or output relative to
;;;		the part whose ports these are).  `port' is just a copy of the original port.
(define impl-test-summarise-port (lambda (port type-context)
	(let*
		((direction (brz-port-direction port))
		 (sense (brz-port:sense port))
		 (name (brz-port:name port))
		 (portions-for-port (lambda (port)
			(case (car port)
				((sync-port arrayed-sync-port) tech-sync-channel-portions)
				(else 
					(case (brz-port-push/pull sense direction)
						((push) tech-push-channel-portions)
						((pull) tech-pull-channel-portions)
					)
				)
			)
		 ))
	  	 (cardinality (brz-type-width (brz-port-type port) type-context))
		 (raw-portions (portions-for-port port))
		 (make-arrayed-port-portions (lambda (index)
		 	(cons* port index (map (lambda (portion) (list
		 		(tech-bundle-name (car portion) name index)
		 		(case (cadr portion)
		 			((push) (case sense ((active) 'output) ((passive) 'input)))
		 			((pull) (case sense ((active) 'input) ((passive) 'output)))
		 		)
		 		(let ((w ((caddr portion) cardinality))) (if (zero? w) 1 w))
		 	)) raw-portions))
		 ))
		)
		(map.. make-arrayed-port-portions 0 (- (brz-port-port-count port) 1))
	)
))

;;; print-impl-test-reset-initial: print the reset and activate raising initial block
(define print-impl-test-reset-initial (lambda (summarised-ports global-nets)
	(let*
		((activate-port-name (brz-port:name (car (car summarised-ports))))
		 (activate-req (tech-bundle-name 'req activate-port-name 0))
		 (activate-ack (tech-bundle-name 'ack activate-port-name 0))
		 (initialise (assoc "initialise" global-nets))
		 (clock (assoc "clock" global-nets))
		)
		(print "  initial begin" #\newline)
		(print "    running = 0;" #\newline)

		; For every input, assign 0 to that signal
		(for-each (lambda (port)
			(print "   ")
			(for-each (lambda (portion)
				(if (eq? 'input (cadr portion))
					(if (string=? "initialise" (car portion))
						(print " initialise = 1;")
						(print " " (car portion) " = 0;")
					)
				)
			) (cddr port))
			(newline)
		) summarised-ports)
		(for-each (lambda (portion)
			(print "   ")
			(if (eq? 'input (cadr portion))
				(if (string=? "initialise" (car portion))
					(print " initialise = 1;")
					(print " " (car portion) " = 0;")
				)
			)
			(newline)
		) global-nets)
		(print "    #`balsa_init_time;" #\newline)
		(if initialise 
			(begin
				(if clock (print "    @(posedge clock)" #\newline "  "))
				(print "    initialise = 0;" #\newline) 
			)
		)
		(print "    #`balsa_activate_delay;" #\newline)
		(if clock (print "    @(posedge clock)" #\newline "  "))
		(print "    running = 1;" #\newline)
		(print "    " activate-req " = 1;" #\newline)
		(if (and clock (string=? breeze-style "sync"))
			(print "    @(posedge clock)" #\newline "      " activate-req " = 0;" #\newline)
			(begin
				(print "    @(posedge " activate-ack ");" #\newline)
				(print "    #`balsa_response_delay;" #\newline)
				(print "    " activate-req " = 0;" #\newline)
				(print "    @(negedge " activate-ack ");" #\newline)
			)
		)
		(print "  end" #\newline)
	)
))

;;; print-impl-test-clock: print clock 
(define print-impl-test-clock (lambda (global-nets)
	(let
		((clocks (if (assoc "clock" global-nets) '("clock") '())))
		(for-each (lambda (clock)
			(print "  always begin" #\newline)
			(print "    " clock " = 0;" #\newline)
			(print "    #`balsa_clock_low_delay;" #\newline)
			(print "    " clock " = 1;" #\newline)
			(print "    #`balsa_clock_high_delay;" #\newline)
			(print "  end" #\newline #\newline)
		) clocks)
	)
))

;;; print-impl-test-defined-port-signals : print wire/reg lines for port decls.
(define print-impl-test-define-port-signals (lambda (summarised-ports global-nets)
	(for-each (lambda (port)
		(for-each (lambda (signal)
			(print "  " (if (eq? 'input (cadr signal)) "reg" "wire"))
			(if (< 1 (caddr signal))
				(print " [" (- (caddr signal) 1) ":0] ")
				(print " ")
			)
			(print (car signal) ";" #\newline)
		) (cddr port))
	) summarised-ports)
	; AB 2008-08-05 taken outside above loop
	(for-each (lambda (signal)
		(print "  " (if (eq? 'input (cadr signal)) "reg" "wire"))
		(if (< 1 (caddr signal))
			(print " [" (- (caddr signal) 1) ":0] ")
			(print " ")
		)
		(print (car signal) ";" #\newline)
	) global-nets)
))

;;; print-impl-test-port-drivers : print default port driving operations
(define print-impl-test-port-drivers (lambda (summarised-ports type-context running)
	(for-each (lambda (port-summary)
		(let*
			((index (cadr port-summary))
			 (port (car port-summary))
			 (is-sync (memq (car port) '(sync-port arrayed-sync-port)))
			 (direction (brz-port-direction port))
			 (name (brz-port:name port))
			 (fancy-name (string-append name
				(if (memq (car port) '(arrayed-sync-port arrayed-port))
					(string-append "[" (number->string index) "]")
					""
				)
			 ))
			 (sense (brz-port:sense port))
			 (cardinality (brz-type-width (brz-port-type port) type-context))
			 (bundle (lambda (portion) (tech-bundle-name portion name index)))
			 (wait (lambda (type) (print "    #`balsa_" type "_delay;" #\newline)))
			 (set (lambda (portion value) (print "    " (bundle portion) " = " value ";" #\newline))) 
			 (wait-term (lambda (term)
				(print "    wait (" term ");" #\newline)
			 ))
			 (wait-edge (lambda (portion edge)
				; (print "    @(" (if edge "pos" "neg") "edge " (bundle portion) ");" #\newline)
				(wait-term (string-append (if edge "" "!") (bundle portion)))
			 ))
			 (print-data (lambda (portion)
				(print "    $display (\"" fancy-name " = \", " (bundle portion) ");" #\newline)
			 ))
			)
			(print "  // Port: `" fancy-name "' " sense " " direction #\newline)
			(print "  initial @(posedge " running ")" #\newline)
			(print "  while (" running ")" #\newline "  begin" #\newline)
			(cond
				(is-sync (case sense
					((active)
						(wait-edge 'req #t)
						(wait "response")
						(set 'ack 1)
						(wait-edge 'req #f)
						(wait "rtz")
						(set 'ack 0)
					)
					((passive)
						(set 'req 1)
						(wait-edge 'ack #t)
						(wait "response")
						(set 'req 0)
						(wait-edge 'ack #f)
					)
				))
				((brz-style-is-single-rail? breeze-style)
					(case direction
						((input) (case sense
							((active)
								(wait-edge 'req #t)
								(wait "response")
								(set 'data 0)
								(wait "sampling")
								(set 'ack 1)
								(wait-edge 'req #f)
								(wait "rtz")
								(set 'ack 0)
							)
							((passive)
								(set 'data 0)
								(wait "sampling")
								(set 'req 1)
								(wait-edge 'ack #t)
								(wait "rtz")
								(set 'req 0)
								(wait-edge 'ack #f)
							)
						))
						((output) (case sense
							((active)
								(wait-edge 'req #t)
								(wait "response")
								(print-data 'data)
								(set 'ack 1)
								(wait-edge 'req #f)
								(wait "rtz")
								(set 'ack 0)
							)
							((passive)
								(set 'req 1)
								(wait-edge 'ack #t)
								(wait "response")
								(print-data 'data)
								(set 'req 0)
								(wait-edge 'ack #f)
							)
						))
					)
				)
				((string=? breeze-style "dual_b")
					(case direction
						((input) (case sense
							((active)
								(wait-edge 'req #t)
								(wait "response")
								(set 'ack1 0)
								(set 'ack0 "~0")
								(wait-edge 'req #f)
								(wait "rtz")
								(set 'ack1 0)
								(set 'ack0 0)
							)
							((passive)
								(set 'req1 0)
								(set 'req0 "~0")
								(wait-edge 'ack #t)
								(wait "rtz")
								(set 'req1 0)
								(set 'req0 0)
								(wait-edge 'ack #f)
							)
						))
						((output) (case sense
							((active)
								(wait-term (string-append "&(" (bundle 'req0) " | " (bundle 'req1) ")"))
								(wait "response")
								(print-data 'req1)
								(set 'ack 1)
								(wait-term (string-append "~&(" (bundle 'req0) " | " (bundle 'req1) ")"))
								(wait "rtz")
								(set 'ack 0)
							)
							((passive)
								(set 'req 1)
								(wait-term (string-append "&(" (bundle 'ack0) " | " (bundle 'ack1) ")"))
								(wait "response")
								(print-data 'ack1)
								(set 'req 0)
								(wait-term (string-append "~&(" (bundle 'ack0) " | " (bundle 'ack1) ")"))
							)
						))
					)
				)
				((string=? breeze-style "one_of_2_4")
					(let
						((width-odd (quotient (+ cardinality 1 2)))
						 (width-even (quotient cardinality 2))
						 (plural (> cardinality 1))
						)
						(case direction
							((input) (case sense
								((active)
									(wait-edge 'req #t)
									(wait "response")
									(if plural (set 'ack3 0))
									(if plural (set 'ack2 0))
									(set 'ack1 0)
									(set 'ack0 "~0")
									(wait-edge 'req #f)
									(wait "rtz")
									(if plural (set 'ack3 0))
									(if plural (set 'ack2 0))
									(set 'ack1 0)
									(set 'ack0 0)
								)
								((passive)
									(if plural (set 'req3 0))
									(if plural (set 'req2 0))
									(set 'req1 0)
									(set 'req0 "~0")
									(wait-edge 'ack #t)
									(wait "rtz")
									(if plural (set 'req3 0))
									(if plural (set 'req2 0))
									(set 'req1 0)
									(set 'req0 0)
									(wait-edge 'ack #f)
								)
							))
							((output) (case sense
								((active)
									(if plural
										(wait-term (string-append "&(" (bundle 'req0) " | " (bundle 'req1) " | " (bundle 'req2) " | " (bundle 'req3) ")"))
										(wait-term (string-append "&(" (bundle 'req0) " | " (bundle 'req1) ")"))
									)
									(wait "response")
									(print-data 'req1)
									(set 'ack 1)
									(if plural
										(wait-term (string-append "~&(" (bundle 'req0) " | " (bundle 'req1) " | " (bundle 'req2) " | " (bundle 'req3) ")"))
										(wait-term (string-append "~&(" (bundle 'req0) " | " (bundle 'req1) ")"))
									)
									(wait "rtz")
									(set 'ack 0)
								)
								((passive)
									(set 'req 1)
									(if plural
										(wait-term (string-append "&(" (bundle 'ack0) " | " (bundle 'ack1) " | " (bundle 'ack2) " | " (bundle 'ack3) ")"))
										(wait-term (string-append "&(" (bundle 'ack0) " | " (bundle 'ack1) ")"))
									)
									(wait "response")
									(print-data 'ack1)
									(set 'req 0)
									(if plural
										(wait-term (string-append "~&(" (bundle 'ack0) " | " (bundle 'ack1) " | " (bundle 'ack2) " | " (bundle 'ack3) ")"))
										(wait-term (string-append "~&(" (bundle 'ack0) " | " (bundle 'ack1) ")"))
									)
								)
							))
						)
					)
				)
				(else
					(print "// Style " breeze-style " not supported, put you own "
						(cadar summarised-ports) " driver" #\newline)
				)
			)
			(print "  end" #\newline #\newline)
		)
	) (cdr summarised-ports)) ; trim activate!
))

;;; print-impl-test-device-under-test : print the device under test instance with port
;;;		connections
(define print-impl-test-device-under-test (lambda (dut-name summarised-ports global-nets)
	(let
		((print-port (lambda (port)
			; (print (build-separated-string (map car (append (cddr port) global-nets)) ", ")) 
			(print (build-separated-string (map car (cddr port)) ", ")) 
		)))
		(print "  " dut-name " DUT (" #\newline "    ")
		(print-port (car summarised-ports)) ; activate
		(for-each (lambda (port)
			(print "," #\newline "    ");
			(print-port port)
		) (cdr summarised-ports))
		(if (not (null? global-nets))
			(print "," #\newline "    " (build-separated-string (map car global-nets) ", "))
		)
		(print #\newline "  );" #\newline)
	)
))

;;; print-impl-test-harness: dump the required test harness in Verilog onto stdout
(define print-impl-test-harness (lambda (dut-name dut-filename ports global-nets type-context)
	(let
		((summarised-ports (apply append (map
			(lambda (port) (impl-test-summarise-port port type-context)) ports))))
		(print-impl-test-header)
		(print-impl-test-simulation-header)
		(newline)
		(for-each (lambda (tech-include)
			(if (not (zero? (string-length tech-include)))
				(let*
					((file-in-tech (string-append breeze-tech-dir tech-include))
					 (file (cond
						; From the root
						((eq? (string-ref tech-include 0) #\/) tech-include)
						; From the tech
						((file-exists? file-in-tech) file-in-tech)
						; Otherwise don't qualify/this directory
						(else tech-include)
					 ))
					)
					(print "`include \"" file "\"" #\newline)
				)
			)
		) tech-netlist-test-includes)
		(print "`include \"" dut-filename "\"" #\newline)
		(newline)
		(print "`timescale 1ns/1ps" #\newline)
		(newline)
		(print "module " impl-test-top-level-name ";" #\newline)
		(print "  reg running;" #\newline)
		(print-impl-test-define-port-signals summarised-ports global-nets)
		(newline)
		(print-impl-test-reset-initial summarised-ports global-nets)
		(newline)
		(print-impl-test-clock global-nets)
		(print "  /* Balsa initialisation tasks */" #\newline)
		(if impl-test-dump-filename
			(begin
				(print "  initial begin" #\newline)
				(print "    $dumpfile (\"" impl-test-dump-filename "\");" #\newline)
				(print "    $dumpvars (" impl-test-dump-depth ", " impl-test-top-level-name ");" #\newline)
				(print "  end" #\newline)
			)
		)
		(if impl-test-print-types
			(begin
				(print "  initial begin" #\newline)
				(for-each (lambda (decl)
					(if (headed-list? decl 'type)
						(print "    $BalsaInternType (\"" (brz-type-decl:name decl) "\", \""
							(verilog-escape-string (->string (brz-type-decl:body decl))) "\");" #\newline)
					)
				) type-context)
				(print "  end" #\newline)
			)
		)
		(newline)
		(print-impl-test-port-drivers summarised-ports type-context "running")
		(print-impl-test-device-under-test dut-name summarised-ports global-nets)
		(print "endmodule" #\newline)
	)
))

;;; find global nets from netlist header
(define impl-test-global-nets (lambda (filename)
	(let
 		((input-file (open-input-file filename)))
		(reverse! (cadr (until (lambda (line signals) (string=? line "*/"))
			(lambda (line signals)
				(list
					(read-line input-file)
					(let
						((tokens (filter (lambda (str) (not (string=? str "")))
							(parse-string line '(#\tab #\space)))))
						(if (and (not (null? tokens)) (string=? (car tokens) "global-signal:"))
							(let
								((name (cadr tokens))
								 (direction (string->symbol (caddr tokens)))
								 (card (string->number (cadddr tokens)))
								)
								(cons (list name direction card) signals)
							)
							signals
						)
					)
				)
			)
			(read-line input-file) '()
		)))
	)
))

;;; impl-test-{no-banner,...}: command line switches
(define impl-test-no-banner #f)
(define impl-test-top-level-name "test") ; top level module (one above DUT) name
(define impl-test-reset-delay 100) ; balsa-sim-init period
(define impl-test-activate-delay 10) ; period after balsa-sim-init to activate req = 1
(define impl-test-response-delay 0.01)
(define impl-test-sampling-delay 0.01)
(define impl-test-rtz-delay 0.005)
(define impl-test-clock-high-delay 50)
(define impl-test-clock-low-delay 50)
(define impl-test-print-types #f)
(define impl-test-dump-filename #f)
(define impl-test-dump-depth 0)
(define impl-test-netlist-filename #f)
(define impl-test-command-line-string #f)
(define impl-test-import-path balsa-search-path)
(define impl-test-printed-banner #f)
(define impl-test-output-file-name #f)
(define impl-test-top-level-include-name #f)
(define impl-test-no-clobber #f)
(define impl-test-touch #f)
(define impl-test-tech #f)

;;; impl-test-print-banner: print the impl-test banner
(define impl-test-print-banner (lambda ()
	(if (not impl-test-printed-banner)
		(make-program-banner "balsa-make-impl-test" "Make test harness for impl. netlist"
			"2003-2008, The University of Manchester")
	)
	(set! impl-test-printed-banner #t)
))

;;; impl-test-usage: command usage
(define impl-test-usage (lambda ()
	(impl-test-print-banner)
	(error
		"version " balsa-version #\newline
		"usage: balsa-make-impl-test {<switch>}* <block/file-name> <top-procedure-name>" #\newline #\newline
		"switches: -h or -?           - Display this message (--help)" #\newline
		"          -b                 - Don't print the impl-test banner (--no-banner)" #\newline
		"          -I <directory>     - Add named directory to the import path to find" #\newline
		"                               blocks (--import)" #\newline
		"          -m <module-name>   - Test harness block name (i.e. the name of" #\newline
		"                               the block containing the instance of the" #\newline
		"                               top Balsa procedure) (--test-module-name)" #\newline
		"          -d <type> <time>   - Set one of the delay times (specified by <type>):" #\newline
		"                                 <type> (default delay) - use" #\newline
		"                                 reset (100 ns) - simulation reset time" #\newline 
		"                                 activate (100 ns) - reset release to activate" #\newline
		"                                 response (10 ns) - port driver response time" #\newline
		"                                 sampling (10 ns) - single rail sampling margin" #\newline
		"                                 rtz (5 ns) - return to zero delay" #\newline
		"                                 high (50 ns) - clock high period" #\newline
		"                                 low (50 ns) - clock low period" #\newline
		"                                 clock (100 ns) - clock period (50% duty)" #\newline
		"                                 (--delay)" #\newline
		"          -o <filename>      - Specify output file name (defaults to stdout, appropriate" #\newline
		"                                 file extension is added) (--output)" #\newline
		"          -i <filename>      - Specify what the implemented netlist file for" #\newline
		"                                 <block/file-name> should be. (--input)" #\newline
		"          -n                 - If -o was specified, don't overwrite output file" #\newline
		"                                 if it already exists (--no-clobber)" #\newline
		"          -t                 - Implies -n, touch(1) output file if it's older than input" #\newline
		"                                 file (--touch)" #\newline
		"          -T                 - Include a type interning `initial' block in the output file" #\newline
		"                                 (--types)" #\newline
		"          -D <filename>      - Dump trace activity to file <filename> (--dump)" #\newline
		"          --dump-depth <depth> - Dump trace activity to depth <depth>" #\newline
        "          -X <tech>          - Set `technology/style/style-opts' to use.  (--technology)" #\newline
        "                                 Technology will be set by ${BALSATECH} env. var. if -X is not passed" #\newline
	)
))

;;; delay-round: rounding function for delay periods.  Round to ceiling thousandth
(define delay-round (lambda (delay)
	(/ (ceiling (* delay 1000)) 1000)
))

;;; impl-test-command-line-rules: command-line-args action rules
(define impl-test-command-line-rules `(
	(#\b "no-banner" 0 ,(lambda (args) (set! impl-test-no-banner #t)))
	(#\m "test-module-name" 1 ,(lambda (args) (set! impl-test-top-level-name (car args))))
	(#\d "delay" 2 ,(lambda (args) (let ((time (delay-round (string->number (cadr args)))))
		(cond
			((string=? (car args) "reset") (set! impl-test-reset-delay time))
			((string=? (car args) "activate") (set! impl-test-activate-delay time))
			((string=? (car args) "response") (set! impl-test-response-delay time))
			((string=? (car args) "sampling") (set! impl-test-sampling-delay time))
			((string=? (car args) "rtz") (set! impl-test-rtz-delay time))
			((string=? (car args) "high") (set! impl-test-clock-high-delay time))
			((string=? (car args) "low") (set! impl-test-clock-low-delay time))
			((string=? (car args) "clock") (let
				((half-time (delay-round (/ time 2))))
				(set! impl-test-clock-high-delay half-time)
				(set! impl-test-clock-low-delay half-time)
			))
			(else (error "Don't know a delay type: " (car args)))
		)
	)))
	(#\I "import" 1 ,(lambda (args) (set! impl-test-import-path (append impl-test-import-path (list (car args))))))
	(#\i "input" 1 ,(lambda (args) (set! impl-test-top-level-include-name (car args))))
	(#\o "output" 1 ,(lambda (args) (set! impl-test-output-file-name (car args))))
	(#\n "no-clobber" 0 ,(lambda (args) (set! impl-test-no-clobber #t)))
	(#\t "touch" 0 ,(lambda (args) (set! impl-test-no-clobber #t) (set! impl-test-touch #t)))
	(#\T "types" 0 ,(lambda (args) (set! impl-test-print-types #t)))
	(#\D "dump" 1 ,(lambda (args) (set! impl-test-dump-filename (car args))))
	(#\_ "dump-depth" 1 ,(lambda (args) (set! impl-test-dump-depth (string->number (car args)))))
	(#\h "help" 0 ,(lambda (args) (impl-test-usage)))
	(#\? "help" 0 ,(lambda (args) (impl-test-usage)))
    (#\X "technology" 1 ,(lambda (args) (set! impl-test-tech (car args))))
))

;;; impl-test-parse-command-line: parse switches from the given command line list, set
;;;		the impl-test-... globals and return the tail of the command line.
(define impl-test-parse-command-line (lambda (args)
	(set! impl-test-command-line-string
		(build-separated-string (map bourne-escape-command-line-arg args) " "))
	(if (null? args)
		(impl-test-usage)
		(parse-command-line "impl-test" impl-test-command-line-rules impl-test-usage args)
	)
))

;;; impl-test: make a makefile for the top Balsa blocks `top-blocks'
(define impl-test (lambda args
	(if (/= (length args) 2) (impl-test-usage))
	(balsa-set-tech impl-test-tech)
	(brz-get-technology)
	(let*
		((block/file-name (car args))
		 (procedure-name (cadr args))
		 (path/name/ext (find-filename block/file-name "breeze" #f impl-test-import-path))
		 (absolute-path (apply string-append path/name/ext))
		 (filename-suffix (string-append "." tech-filename-suffix))
		 (output-file-name (if impl-test-output-file-name
			(if (final-string impl-test-output-file-name filename-suffix)
				impl-test-output-file-name (string-append impl-test-output-file-name filename-suffix))
			#f
		 ))
		)
		(if (and impl-test-no-clobber (file-exists? output-file-name))
			(begin
				(print-err "balsa-make-impl-test: output file exists, not regenerating" #\newline)
				(if impl-test-touch
					(if (file-ctime<=? absolute-path output-file-name)
						(print-err "balsa-make-impl-test: output file is new enough, not touch(1)ing" #\newline)
						(begin
							(print-err "balsa-make-impl-test: touch(1)ing output file" #\newline)
							(system (string-append "touch \"" output-file-name "\""))
						)
					)
				)
			)
			(let*
				((decls/imports/imported-decls/visited-blocks (get-flattened-block
					(apply string-append path/name/ext) "breeze" '() impl-test-import-path))
				 (decls (car decls/imports/imported-decls/visited-blocks))
				 (all-decls (append (caddr decls/imports/imported-decls/visited-blocks) decls))
				 (top-level-hdl-file-name-without-extension (if impl-test-top-level-include-name
					impl-test-top-level-include-name
					(string-append (car path/name/ext) (cadr path/name/ext) filename-suffix)
				 ))
				 (top-level-hdl-file-name
				   	(let	
					  	((base-name (if impl-test-top-level-include-name
							impl-test-top-level-include-name
							(string-append (car path/name/ext) (cadr path/name/ext))
						)))
				   		(string-append base-name (if (final-string base-name filename-suffix) "" filename-suffix))
					)
				 )
				 (global-ports (impl-test-global-nets top-level-hdl-file-name))
				 (dut-procedure (brz-find-breeze-part decls procedure-name))
				 (generate-test-harness (lambda ()
					(print-impl-test-harness (tech-mangle-breeze-part-name procedure-name) top-level-hdl-file-name 
						(cdr (brz-breeze-part:ports dut-procedure)) global-ports all-decls)
				 ))
				)
				;; (print (sim-block-name->path "[b]" "breeze" "v") #\newline)
				;;; FIXME, generalise
				(if (not dut-procedure)
					(error "balsa-make-impl-test: can't find procedure `" procedure-name "'" #\newline)
				)	
				;; Only Verilog is currently supported
				(if (not (eq? tech-netlist-format-name 'verilog))
					(error "balsa-make-impl-test: only Verilog is currently supported")
				)
				(if impl-test-output-file-name
					(with-output-to-file output-file-name generate-test-harness)
					(generate-test-harness)
				)
			)
		)
	)
))

(top-level (lambda (args)
	(let
		((args-tail (impl-test-parse-command-line args)))
		(if (not impl-test-no-banner)
			(impl-test-print-banner)
		)
		(apply impl-test args-tail)
	)
) (cdr command-line-args))
