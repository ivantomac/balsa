;;;
;;; `xilinx'
;;;	Generic Xilinx tech. description
;;;
;;;	2004-06-02, Sam Taylor
;;;	2002-10-15, Andrew Bardsley
;;;

(net-signature-for-netlist-format 'verilog #t)

;;; max. no. of inputs for and/or/nand/nor gates and c-elements
(set! tech-gate-max-fan-in 4)
(set! tech-c-element-max-fan-in 3)

(set! tech-map-cell-name (net-simple-cell-name-mapping #f)) ;;; mapping
(set! tech-cell-name-max-length 64)

(set! tech-gnd-component-name "GND")
(set! tech-vcc-component-name "VCC")
(set! breeze-gates-net-files '("xilinx-cells" "balsa-cells"))
(set! breeze-gates-mapping-file (string-append breeze-tech-dir "gate-mappings"))

(set! tech-netlist-test-includes '("xilinx.cells"))

(set! breeze-style-options (cons*
	'("mux" . "gates")
	'("latch" . "edge-dff")
	breeze-style-options
))
