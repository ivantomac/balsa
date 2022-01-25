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
;;;	`misc-banners.scm'
;;;	Make four line high ASCII art banners
;;;

;;; The font can be found in the banner-font file
(define banner-font #(
	(" " " " " " " ") (" " "|" "."  " ") ("  " "''" "  " "  ") ("    " "_||_" "_|L_" " || ") ("   " ",|-" "-|'" "   ")
	("  " "o/" "/o" "  ") ("  " "o " "O<" "  ") (" " "|" " " " ") ("  " ",'" "`."  "  ") ("  " "`."  ",'" "  ")
	("   " "   " ">|<" "   ") ("   " "_|_" " | " "   ") (" " " " "."  "'") ("   " "   " " - " "   ") (" " " " "."  " ")
	("  " " /" "/ " "  ") ("  " "/\\" "\\/" "  ") ("  " "'|" " |" "  ") ("  " "')" "/_" "  ") ("  " "')" ".)"  "  ")
	("  " "L|" " |" "  ") ("  " "['" "_)" "  ") ("  " "/`" "\\)" "  ") ("  " "`7" "/ " "  ") ("  " "()" "()" "  ")
	("  " "()" "/ " "  ") (" " "."  "."  " ") (" " "."  "."  "'") (" " "/" "\\" " ") ("  " "__" "--" "  ")
	(" " "\\" "/" " ") ("  " "`)" "! " "  ") ("   " ",-."  "|a/" "`- ") ("  ."  " /|" "/-|" "   ")
	("._ " "|_)" "|_)" "   ") (" _ " "/ `" "\\_," "   ") ("._ " "| \\" "|_/" "   ") ("._" "|_" "|_" "  ")
	("._" "|_" "| " "  ") (" _ " "/_`" "\\_|" "   ") (". ."  "|_|" "| |" "   ") ("."  "|" "|" " ")
	("___" "  |" "\\_/" "   ") (". ," "|_/" "| \\" "   ") (". " "| " "|_" "  ") (".  ."  "|\\/|" "|  |" "    ")
	(".  ."  "|\\ |" "| \\|" "    ") (" _ " "/ \\" "\\_/" "   ") ("._ " "|_)" "|  " "   ") (" _ " "/ \\" "\\_X" "   ")
	("._ " "|_)" "| \\" "   ") (" _ " "(_`" "._)" "   ") ("_._" " | " " | " "   ") (". ."  "| |" "\\_/" "   ")
	(". ," "| /" "|/ " "   ") (".  ."  "|  |" "|/\\|" "    ") (". ," "\\_/" "/ \\" "   ") (". ."  "\\_|" "._/" "   ")
	("__" " /" "/_" "  ") ("._" "| " "|_" "  ") ("  " "\\ " " \\" "  ") ("_."  " |" "_|" "  ") ("  " "/\\" "  " "  ")
	("  " "  " "  " "--") (" " "\\" " " " ") ("   " " _ " "(_\\" "   ") ("   " "|_ " "|_)" "   ")
	("   " " _ " "(_."  "   ") ("   " " _|" "(_|" "   ") ("   " " _ " "(-'" "   ") (" _" "[."  "| " "  ")
	("   " " _ " "(_|" " -'") ("   " "|_ " "| |" "   ") (" " "."  "|" " ") (" " "."  "|" "/") ("  " "|_" "|\\" "  ")
	(" " "|" "|" " ") ("   " "._ " "|||" "   ") ("   " "._ " "| |" "   ") ("   " " _ " "(_)" "   ")
	("   " "._ " "|_)" "|  ") ("   " " _."  "(_|" "  |") ("   " "._ " "| `" "   ") ("   " "  _" "_/ " "   ")
	("  " "|_" "|_" "  ") ("   " ". ."  "|_|" "   ") ("   " ". ," "|/ " "   ") ("     " ", . ," "|/|/ " "     ")
	("   " "_ _" "_/_" "   ") ("   " ". ."  "(_|" " -'") ("   " "__ " " /_" "   ") ("  " "_{" " {" "  ")
	(" " "|" "|" " ") ("  " "}_" "} " "  ") ("    " ",-_," "    " "    ") 
))

;;; char->bannerchar: map a character onto a 4tuple of strings from the banner font
(define char->bannerchar (lambda (char)
	(case char
		((#\newline) '("  " "  " "  " "  "))
		(else (vector-ref banner-font (- (char->integer char) (char->integer #\space))))
	)
))

;;; string->banner: map char->bannerchar across a string
(define string->asciiart (lambda (string)
	(fold
		(lambda (this tail) (map string-append tail this))
		'("" "" "" "")
		(map char->bannerchar (string->list string))
	)
))

;;; make-banner: Make a banner from the string `string'
(define make-banner (lambda (string)
	(line-print (string->asciiart string))
))

;;; make-program-banner: Make a banner from the string `string' with strings
;;;		shout1 and shout2 on the ends of the two main lines of the banner. eg.
;;;	(make-program-banner "balsa-md" "balsa-md Balsa Makefile generator"
;;;		"1997,98 Andrew Bardsley (bardsley@cs.man.ac.uk)") =>
;;;
;;;	|_  _ |  _ _    ._  _|  [ balsa-md Balsa Makefile generator ]
;;; |_)(_\|_/ (_\ - |||(_|  (C) 1997,98 Andrew Bardsley (bardsley@cs.man.ac.uk)
;;;
(define make-program-banner (lambda (string shout1 shout2)
	(for-each (lambda (line)
		(print-to-port (current-output-port) line #\newline)
	) (map string-append
		(list " " " " " " " ")
		(string->asciiart string)
		(list "  " "  [ " "  " "  ")
		(list "" shout1 (string-append "(C) " shout2) "")
		(list "" " ]" "" "")
	))
))

;;; make-sampler: Make a sampler of all the printable ASCII characters
(define make-sampler (lambda ()
	(let
		((sampler (make-string 193 #\space)))
		(do ((i 32 (+ 1 i))) ((= i 127) (make-banner sampler))
			(string-set! sampler (* 2 (- i 32)) (integer->char i))
		)
	)
))
