//
// minimal-cells.net 
// Minimal cell library
//
// 24 May 2007, Will Toms
//
//


`timescale 1ns/1ps
`define gate_delay 0.090


module NOR2 (out, in0, in1);
  output out;
  input in0, in1;

  nor #(`gate_delay, `gate_delay) I0 (out, in0, in1);
endmodule
