;;;
;;; `example'
;;; Example tech. description
;;;
;;; 6 Aug 2000, Andrew Bardsley
;;;

(net-signature-for-netlist-format 'verilog #t)

(set! breeze-gates-net-files '("example-cells" "balsa-cells"))
(set! breeze-primitives-file (string-append breeze-tech-dir "components.abs"))
(set! breeze-gates-mapping-file (string-append breeze-tech-dir "gate-mappings"))

;;; max. no. of inputs for and/or/nand/nor gates and c-elements
(set! tech-gate-max-fan-in 3)
(set! tech-c-element-max-fan-in 3)

;;; use name mapping to keep names less than ~48 chars
(set! tech-map-cell-name (net-simple-cell-name-mapping #f))
(set! tech-map-cell-name-import net-simple-cell-name-import)
(set! tech-map-cell-name-export net-simple-cell-name-export)
(set! tech-cell-name-max-length 48)

(set! tech-gnd-net-name "!gnd")
(set! tech-vcc-net-name "!vdd")

(set! tech-netlist-test-includes '("example-cells.v"))
