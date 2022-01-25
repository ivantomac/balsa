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
;;;	`brz-parser.scm'
;;;	Parse from a Scheme formatted Breeze file
;;;

;;; brz-check*?: brz-check procedures check the given argument
;;;    for compliance with the named patterm.  Warnings are
;;;    given for headed list objects which have a correct initial
;;;    symbol but an incorrect structure

;;; brz-param:value: field accessor
(define brz-param:value cadr)

;;; brz-check-param?: ('param name)
(define brz-check-param? (lambda (value)
	(and
		(headed-list? value 'param)
		(warning-and "param"
			`(,(= 2 (length value)) "format: (param name)")
		)
		(warning-and "param"
			`(,(string? (brz-param:value value)) "expecting a parameter name")
		)
	)
))

;;; brz-check-literal?: string, symbol or integer
(define brz-check-literal? (lambda (literal)
	(or
		(symbol? literal)
		(string? literal)
		(integer? literal)
	)
))

;;; brz-check-value?: string, symbol, integer, param
(define brz-check-value? (lambda (value)
	(or
		(brz-check-literal? value)
		(brz-check-param? value)
	)
))

;;; brz-named-type:name: field accessor
(define brz-named-type:name cadr)
;;; brz-alias-type:name: field accessors
(define brz-alias-type:name cadr)
;;; brz-numeric-type:{signedness,width}: field accessors
(define brz-numeric-type:signedness cadr)
(define brz-numeric-type:width caddr)
;;; brz-array-type:{type,low-index,element-count}: field accessors
(define brz-array-type:type cadr)
(define brz-array-type:low-index caddr)
(define brz-array-type:element-count cadddr)
;;; brz-variable-array-type:{original-type,low-index,element-count,spec-string}
(define brz-variable-array-type:type cadr)
(define brz-variable-array-type:low-index caddr)
(define brz-variable-array-type:element-count cadddr)
(define brz-variable-array-type:spec-string (nth 4)) 
;;; brz-variable-width-array-type:{low-index,element-count,widths}
(define brz-variable-width-array-type:low-index cadr)
(define brz-variable-width-array-type:element-count caddr)
(define brz-variable-width-array-type:widths cadddr)

;;; brz-check-type?: Is this a valid named/anonymous type
(define brz-check-type? (lambda (type)
	(let
		((brz-check-named-type? (lambda ()
			(and
				(headed-list? type 'named-type)
				(warning-and "named-type"
					`(,(= 2 (length type)) "format: (named-type name)")
				)
				(warning-and "named-type"
					`(,(string? (brz-named-type:name type)) "expecting a type name")
				)
			)
		 ))
		 (brz-check-alias-type? (lambda ()
			(and
				(headed-list? type 'alias-type)
				(warning-and "alias-type"
					`(,(= 2 (length type)) "format: (alias-type name)")
				)
				(warning-and "alias-type"
					`(,(string? (brz-alias-type:name type)) "expecting a type name")
				)
			)
		 ))
		 (brz-check-numeric-type? (lambda ()
			(and
				(headed-list? type 'numeric-type)
				(warning-and "numeric-type"
					`(,(= 3 (length type)) "format: (numeric-type signedness width)")
				)
				(warning-and "numeric-type"
					`(,(boolean? (brz-numeric-type:signedness type)) "expecting a boolean signedness value")
					`(,(brz-check-value? (brz-numeric-type:width type)) "expecting an integer width") 
				)
			)
		 ))
		 (brz-check-array-type? (lambda ()
			(and
				(headed-list? type 'array-type)
				(warning-and "array-type"
					`(,(= 4 (length type)) "format: (array-type type low-index elementCount)")
				)
				(warning-and "array-type"
					`(,(brz-check-type?  (brz-array-type:type type)) "expecting a valid type")
					`(,(brz-check-value? (brz-array-type:low-index type)) "expecting an integer low-index") 
					`(,(brz-check-value? (brz-array-type:element-count type)) "expecting an integer elementCount") 
				)
			)
		 ))
		 (brz-check-variable-array-type? (lambda ()
			(and
				(headed-list? type 'variable-array-type)
				(warning-and "variable-array-type"
					`(,(= 5 (length type)) "format: (variable-array-type type low-index elementCount spec-string)")
				)
				(warning-and "variable-array-type"
					`(,(brz-check-type?  (brz-variable-array-type:type type)) "expecting a valid type")
					`(,(brz-check-value? (brz-variable-array-type:low-index type)) "expecting an integer low-index") 
					`(,(brz-check-value? (brz-variable-array-type:element-count type)) "expecting an integer elementCount")
					`(,(string? (brz-variable-array-type:spec-string type)) "expecting specification string")
				)
			)
		 ))
		 (brz-check-variable-width-array-type? (lambda ()
			(and
				(headed-list? type 'variable-width-array-type)
				(warning-and "variable-width-array-type"
					`(,(= 5 (length type)) "format: (variable-width-array-type type low-index elementCount spec-string)")
				)
				(warning-and "variable-width-array-type"
					`(,(brz-check-value? (brz-variable-width-array-type:low-index type)) "expecting an integer low-index") 
					`(,(brz-check-value? (brz-variable-width-array-type:element-count type)) "expecting an integer elementCount")
					`(,(list? (brz-variable-array-type:spec-string type)) "expecting list of widths")
				)
			)
		 ))
		 (brz-check-string-type? (lambda ()
			(headed-list? type 'string)
		 ))
		)
		(or
			(brz-check-named-type?)
			(brz-check-alias-type?)
			(brz-check-numeric-type?)
			(brz-check-array-type?)
			(brz-check-string-type?)
			; (brz-check-variable-array-type?)
			; (brz-check-variable-width-array-type?)
		)
	)
))

;;; brz-parameter:{name,type}: field accessors
(define brz-parameter:name car)
(define brz-parameter:type cadr)

;;; brz-check-parameters?: ('parameters (name value)* ) ?
(define brz-check-parameters? (lambda (parameters)
	(headed-uniform-list? parameters
		'parameters
		(lambda (param)
			(and
				(pair? param)
				(warning-and "parameter"
					`(,(<= 2 (length param)) "format (name type ...)")
				)
				(warning-and "parameter"
					`(,(string? (car param)) "expecting a parameter name")
					`(,(brz-check-type? (cadr param)) "expecting a valid type")
				)
			)
		)
	)
))

;;; brz-port-{sense,direction}?: Is the argument either passive or active, input or output
(define brz-port-sense? (lambda (sense)
	(not (not (memv sense '(passive active))))
))
(define brz-port-direction? (lambda (direction)
	(not (not (memv direction '(input output))))
))

;;; brz-port-type: return the type field of port/arrayed-port ports and '(sync-type)
;;;		for sync and arrayed sync ports
(define brz-port-type (lambda (port)
	(case (car port)
		((port) (brz-port:type port))
		((arrayed-port) (brz-arrayed-port:type port))
		((sync-port arrayed-sync-port) '(sync-type))
	)
))

;;; brz-port:{name,sense,direction,type}: field accessors
(define brz-port:name cadr)
(define brz-port:sense caddr)
(define brz-port:direction cadddr)
(define brz-port:type (nth 4))
;;; brz-sync-port:{name,sense}: field accessors
(define brz-sync-port:name cadr)
(define brz-sync-port:sense caddr)
;;; brz-arrayed-port:{name,sense,direction,type,low-index,port-count}: field accessors
(define brz-arrayed-port:name cadr)
(define brz-arrayed-port:sense caddr)
(define brz-arrayed-port:direction cadddr)
(define brz-arrayed-port:type (nth 4))
(define brz-arrayed-port:low-index (nth 5))
(define brz-arrayed-port:port-count (nth 6))
;;; brz-arrayed-sync-port:{name,sense,low-index,port-count}: field accessors
(define brz-arrayed-sync-port:name cadr)
(define brz-arrayed-sync-port:sense caddr)
(define brz-arrayed-sync-port:low-index cadddr)
(define brz-arrayed-sync-port:port-count (nth 4))

;;; brz-check-port?: Is port a valid Breeze port?
(define brz-check-port? (lambda (port)
	(let	
		((brz-check-normal-port? (lambda (port)
			(and
				(headed-list? port 'port)
				(warning-and "port"
					`(,(>= (length port) 5) "format: (port name sense direction type [options ...])") 
				)
				(warning-and "port"
					`(,(string? (brz-port:name port)) "expecting a port name")
					`(,(brz-port-sense? (brz-port:sense port)) "expecting a port sense")
					`(,(brz-port-direction? (brz-port:direction port)) "expecting a port direction")
					`(,(brz-check-type? (brz-port:type port)) "expecting a valid type")
				)
			)
		 ))
		 (brz-check-sync-port? (lambda (port)
			(and
				(headed-list? port 'sync-port)
				(warning-and "sync-port"
					`(,(<= 3 (length port)) "format: (sync-port name sense ...)")
				)
				(warning-and "sync-port"
					`(,(string? (brz-sync-port:name port)) "expecting a port name")
					`(,(brz-port-sense? (brz-sync-port:sense port)) "expecting a port sense")
				)
			)
		 ))
		 (brz-check-arrayed-port? (lambda (port)
			(and
				(headed-list? port 'arrayed-port)
				(warning-and "arrayed-port"
					`(,(<= 7 (length port)) "format: (arrayed-port name sense direction type low-index port-count ...)")
				)
				(warning-and "arrayed-port"
					`(,(string? (brz-arrayed-port:name port)) "expecting a port name")
					`(,(brz-port-sense? (brz-arrayed-port:sense port)) "expecting a port sense")
					`(,(brz-port-direction? (brz-arrayed-port:direction port)) "expecting a port direction")
					`(,(brz-check-type? (brz-arrayed-port:type port)) "expecting a valid type")
					`(,(brz-check-value? (brz-arrayed-port:low-index port)) "expecting a low-index value")
					`(,(brz-check-value? (brz-arrayed-port:port-count port)) "expecting a port-count value")
				)
			)
		 ))
		 (brz-check-arrayed-sync-port? (lambda (port)
			(and
				(headed-list? port 'arrayed-sync-port)
				(warning-and "arrayed-sync-port"
					`(,(<= 5 (length port)) "format: (arrayed-sync-port name sense low-index port-count ...)")
				)
				(warning-and "arrayed-sync-port"
					`(,(string? (brz-arrayed-sync-port:name port)) "expecting a port name")
					`(,(brz-port-sense? (brz-arrayed-sync-port:sense port)) "expecting a port sense")
					`(,(brz-check-value? (brz-arrayed-sync-port:low-index port)) "expecting a low-index value")
					`(,(brz-check-value? (brz-arrayed-sync-port:port-count port)) "expecting a port-count value")
				)
			)
		 ))
		)
		(or
			(brz-check-sync-port? port)
			(brz-check-normal-port? port)
			(brz-check-arrayed-port? port)
			(brz-check-arrayed-sync-port? port)
		)
	)
))

;;; brz-check-ports?: ('ports port* )?
(define brz-check-ports? (lambda (ports)
	(headed-uniform-list? ports 'ports brz-check-port?)
))

;;; brz-check-actual-port?: Is port a (positive integer | list of positive integers)
(define brz-check-actual-port? (lambda (port)
	(or
		(positive-integer? port) ; single port
		(and
			(list? port) ; arrayed port
			(apply eager-and (map positive-integer? port))
		)
	)
))

;;; brz-component:{name,parameters,ports}: field accessors
(define brz-component:name cadr)
(define brz-component:parameters caddr)
(define brz-component:ports cadddr)
(define brz-component:options (lambda (c) (list-tail c 4)))

;;; brz-check-component?: Is component a correctly formed component
(define brz-check-component? (lambda (component)
	(and
		(headed-list? component 'component)
		(warning-and "check-component"
			`(,(>= (length component) 4) "format: (component name parameters ports [options ...])")
		)
		(warning-and "check-component"
			`(,(string? (brz-component:name component)) "expecting a component name")
			`(,(apply eager-and (map brz-check-literal? (brz-component:parameters component)))
				"expecting a list of parameters")
			`(,(apply eager-and (map brz-check-actual-port? (brz-component:ports component)))
				"expecting a list of ports") 
		)
	)
))

;;; brz-check-components?: ('components component* )?
(define brz-check-components? (lambda (components)
	(headed-uniform-list? components 'components brz-check-component?)
))

;;; brz-channel:type: 'sync, 'push, 'pull channel type
(define brz-channel:type car)
;;; brz-channel:width: field accessor (for push and pull channels)
(define brz-channel:width cadr)
;;; brz-channel:name: get (optional) name of channel, returns "" for no name
(define brz-channel:name (lambda (chan)
	(let ((name-sublist (find-headed-sub-list chan 'name)))
		(if name-sublist (cadr name-sublist) "")
	)
))
;;; brz-channel:position: return the position of a channel
;;;		(as the tail of the `(at ...)' list in the channel options or
;;;		#f if there is no position
(define brz-channel:position (lambda  (chan)
	(let ((at-sublist (find-headed-sub-list chan 'at)))
		(if at-sublist (cdr at-sublist) #f)
	)
))

;;; brz-check-channel?: Is channel (sync) or (push n) or (pull n)
(define brz-check-channel? (lambda (channel)
	(let ((channel-length (length channel)))
		(or
			(headed-list? channel 'sync)
			(and
				(headed-list? channel 'push)
				(warning-and "check-channel"
					`(,(<= 2 channel-length) "format: (push width ...)")
				)
				(warning-and "check-channel"
					`(,(positive-integer? (brz-channel:width channel)) "expecting a positive integer width")
				)
			)
			(and
				(headed-list? channel 'pull)
				(warning-and "check-channel"
					`(,(<= 2 channel-length) "format: (pull width ...)")
				)
				(warning-and "check-channel"
					`(,(positive-integer? (brz-channel:width channel)) "expecting a positive integer width")
				)
			)
		)
	)
))

;;; brz-check-channels?: ('channels channel* )?
(define brz-check-channels? (lambda (channels)
	(headed-uniform-list? channels 'channels brz-check-channel?)
))

;;; brz-primitive-part:{name,parameters,ports}: field accessors
(define brz-primitive-part:name cadr)
(define brz-primitive-part:parameters caddr)
(define brz-primitive-part:ports cadddr)
(define brz-primitive-part:symbol (nth 4))
(define brz-primitive-part:implementation (nth 5))

;;; brz-implementation-style:{name,nodes,gates}: field accessors for (implementation (style ...)) parts
(define brz-implementation-style:name cadr)
(define brz-implementation-style:nodes (lambda (t) (find-headed-list-elem t 'nodes #f)))
(define brz-implementation-style:gates (lambda (t) (find-headed-list-elem t 'gates #f)))
(define brz-implementation-style:connections (lambda (t) (find-headed-list-elem t 'connections #f)))
(define brz-implementation-style:defines (lambda (t) (find-headed-list-elem t 'defines #f)))

;;; brz-check-primitive-part?: ('primitive-part parameters ports)
(define brz-check-primitive-part? (lambda (part)
	(and
		(headed-list? part 'primitive-part)
		(warning-and "primitive-part"
			`(,(<= 4 (length part)) "format: (primitive-part name parameters ports)")
		)
		(warning-and "primitive-part"
			`(,(string? (brz-primitive-part:name part)) "expecting a primitive-part name")
			`(,(brz-check-parameters? (brz-primitive-part:parameters part)) "expecting a list of parameters")
			`(,(brz-check-ports? (brz-primitive-part:ports part)) "expecting a list of ports")
		)
	)
))

;;; brz-attribute:{name,value}: field accessors
(define brz-attribute:name car)
(define brz-attribute:value cadr)

;;; brz-check-attribute?: Is attr one of (symbol) or (symbol value)
(define brz-check-attribute? (lambda (attr)
	(and
		(pair? attr)
		(symbol? (brz-attribute:name attr))
	)
))

;;; brz-check-attributes?: Is attrs a list of attributes ('attributes attribute*)
(define brz-check-attributes? (lambda (attrs)
	(headed-uniform-list? attrs 'attributes brz-check-attribute?)
))

;;; brz-breeze-part:{name,parameters,ports}: field accessors
(define brz-breeze-part:name cadr)
(define brz-breeze-part:ports caddr)
(define brz-breeze-part:attributes cadddr)
(define brz-breeze-part:channels (nth 4))
(define brz-breeze-part:components (nth 5))

;;; brz-check-breeze-part?: ('breeze-part ports)
(define brz-check-breeze-part? (lambda (part)
	(and
		(headed-list? part 'breeze-part)
		(warning-and "breeze-part"
			`(,(<= 5 (length part)) "format: (breeze-part name ports attributes channels components)")
		)
		(warning-and "breeze-part"
			`(,(string? (brz-breeze-part:name part)) "expecting a part name")
			`(,(brz-check-ports? (brz-breeze-part:ports part)) "expecting a list of ports")
			`(,(brz-check-attributes? (brz-breeze-part:attributes part)) "expecting a list of attributes")
			`(,(brz-check-channels? (brz-breeze-part:channels part)) "expecting a list of channels")
			'(,(brz-check-components? (brz-breeze-part:components part)) "expecting a list of components")
		)
	)
))

;;; brz-enumeration-type:{signedness,width,bindings}: field accessors
(define brz-enumeration-type:signedness cadr)
(define brz-enumeration-type:width caddr)
(define brz-enumeration-type:bindings cdddr)
;;; brz-binding:{name,value}: field accessors
(define brz-binding:name car)
(define brz-binding:value cadr)
;;; brz-record-type:{width,fields}: field accessors
(define brz-record-type:width cadr)
(define brz-record-type:fields cddr)
;;; brz-record-field:{name,type}: field accessors
(define brz-record-field:name car)
(define brz-record-field:type cadr)

;;; brz-check-type-decl-body?: Is type-decl-body a valid type description
(define brz-check-type-decl-body? (lambda (type-decl-body)
	(let
		((brz-check-enumeration-type-decl? (lambda ()
			(and
				(headed-list? type-decl-body 'enumeration-type)
				(warning-and "enumeration-type"
					`(,(<= 4 (length type-decl-body)) "format: (enumeration-type signedness width (name value)*)")
				)
				(warning-and "enumeration-type"
					`(,(boolean? (brz-enumeration-type:signedness type-decl-body))
						"expecting a boolean `signedness' value")
					`(,(positive-integer? (brz-enumeration-type:width type-decl-body)) "expecting a type width")
					`(,(apply eager-and (map 
						(lambda (element)
							(and
								(list? element)
								(= 2 (length element))
								(string? (brz-binding:name element))
								(integer? (brz-binding:value element))
							)
						)
						(brz-enumeration-type:bindings type-decl-body))) "some list elements are non compliant")
				)
			)
		 ))
		 (brz-check-record-type-decl? (lambda ()
			(and
				(headed-list? type-decl-body 'record-type)
				(warning-and "record-type"
					`(,(<= 3 (length type-decl-body)) "format: (record-type width (name type)*)")
				)
				(warning-and "record-type"
					`(,(positive-integer? (brz-record-type:width type-decl-body)) "expecting a type width")
					`(,(apply eager-and (map 
						(lambda (element)
							(and
								(list? element)
								(= 2 (length element))
								(string? (brz-record-field:name element))
								(brz-check-type? (brz-record-field:type element))
							)
						)
						(brz-record-type:fields type-decl-body))) "some list elements are non compliant")
				)
			)
		 ))
		 (brz-check-builtin-type-decl? (lambda ()
			(headed-list? type-decl-body 'builtin-type)
		 ))
		)
		(or
			(brz-check-type? type-decl-body)
			(brz-check-enumeration-type-decl?)
			(brz-check-record-type-decl?)
			(brz-check-builtin-type-decl?)
		)
	)
))

;;; brz-context-marker?: Is this a (redundant) context-marker decl.?
(define brz-context-marker? (lambda (decl)
	(headed-list? decl 'context-marker)
))

;;; brz-type-decl:{name,body}: field accessors
(define brz-type-decl:name cadr)
(define brz-type-decl:body caddr)

;;; brz-check-type-decl?: Is type-decl a valid type declaration?
(define brz-check-type-decl? (lambda (type-decl)
	(and
		(headed-list? type-decl 'type)
		(warning-and "type-decl"
			`(,(= 3 (length type-decl)) "format: (type name type-decl-body)")
		)
		(warning-and "type-decl"
			`(,(string? (brz-type-decl:name type-decl)) "expecting a type name")
			`(,(brz-check-type-decl-body? (brz-type-decl:body type-decl)) "expecting a type-decl-body")
		)
	)
))

;;; brz-constant-decl:{name,value,type}: field accessors
(define brz-constant-decl:name cadr)
(define brz-constant-decl:value caddr)
(define brz-constant-decl:type cadddr)

;;; brz-check-constant-decl?: Is constant-decl a valid constant declaration?
(define brz-check-constant-decl? (lambda (constant-decl)
	(and
		(headed-list? constant-decl 'constant)
		(warning-and "constant-decl"
			`(,(= 4 (length constant-decl)) "format: (constant name value type)")
		)
		(warning-and "constant-decl"
			`(,(string? (brz-constant-decl:name constant-decl)) "expecting a constant name")
			`(,(integer? (brz-constant-decl:value constant-decl)) "expecting an integer value")
			`(,(brz-check-type? (brz-constant-decl:type constant-decl)) "expecting a type")
		)
	)
))

;;; brz-check-decl?: Is decl a valid declaration (w/o context markers)
(define brz-check-decl? (lambda (decl)
	(or
		(brz-check-breeze-part? decl)
		(brz-check-constant-decl? decl)
		(brz-check-type-decl? decl)
	)
))

;;; read-and-check-breeze-file: Read in a breeze file and returns its declarations
(define read-and-check-breeze-file (lambda (filename)
	(let*
		((file (open-input-file filename))
		 (unchecked-list (read-list file))
		 (marker-free-list (filter (lambda (e) (not (brz-context-marker? e))) unchecked-list))
		)
		(if (apply eager-and (map brz-check-decl? marker-free-list)) ; All elements OK
			marker-free-list
			#f
		)
	)
))

;;; read-and-check-primitive-part-file: Read in the primitive parts from the given file
(define read-and-check-primitive-part-file (lambda (filename)
	(let*
		((file (open-input-file filename))
		 (unchecked-list (read-list file))
		)
		; All elements are primitive parts?
		(if (apply eager-and (map brz-check-primitive-part? unchecked-list))
			unchecked-list
			#f
		)
	)
))
