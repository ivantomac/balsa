//
// `example-cells.net' 
// Example cell library
//
// 9 Aug 2001, Andrew Bardsley
//
// $Id: example-cells.m4,v 1.4 2003/10/16 13:35:50 bardslea Exp $
//

changequote({, })dnl
define({for}, {ifelse(0,$3,,{pushdef({$1}, {$2})for_nest({$1},{$2},{$3},{$4})popdef({$1})})})dnl
define({for_nest}, {$4{}ifelse($1,{$3},, {define({$1}, incr($1))for_nest({$1},{$2},{$3},{$4})})})dnl

`timescale 1ns/1ps
`define gate_delay 0.150

define({sep_inputs}, {in{}$1{}for(I,incr($1),decr($2),{$3{}in{}I})})dnl
dnl
define({n_input_gate},{dnl
define({inputs}, {sep_inputs(0,$2,{, })})dnl
module $1{}$2 (out, inputs);
  output out;
  input inputs;

  reg out;

  always @(sep_inputs(0,$2,{ or }))
    $3
endmodule
})dnl
define({simple_n_input_gate},{dnl
define({inputs}, {sep_inputs(0,$2,{, })})dnl
module $1{}$2 (out, inputs);
  output out;
  input inputs;

  $3 {#}(`gate_delay, `gate_delay) I0 (out, inputs);
endmodule
})dnl

define({c_element},{n_input_gate($1, $2,{if (in0 == in1{}ifelse(2,$2,{)},dnl
{ for(I,2,decr($2),{& in0 == in{}I}))}) {#}`gate_delay out = in0;})})dnl
dnl
simple_n_input_gate(AND,2,and)
simple_n_input_gate(AND,3,and)
simple_n_input_gate(AND,4,and)
simple_n_input_gate(OR,2,or)
simple_n_input_gate(OR,3,or)
simple_n_input_gate(OR,4,or)
simple_n_input_gate(NAND,2,nand)
simple_n_input_gate(NAND,3,nand)
simple_n_input_gate(NAND,4,nand)
simple_n_input_gate(NOR,2,nor)
simple_n_input_gate(NOR,3,nor)
simple_n_input_gate(NOR,4,nor)
simple_n_input_gate(XOR,2,xor)
simple_n_input_gate(XNOR,2,xnor)

dnl c_element(C,2)
dnl c_element(C,3)

module MUTEX (inA, inB, outA, outB);
  input inA, inB;
  output outA, outB;
  reg outA, outB;
  integer lastSide;

  initial outA = 0;
  initial outB = 0;

  always begin
    wait (inA | inB);
    if ((inA & inB && (lastSide == 1)) || (inA & ~inB)) begin
      lastSide = 0;
      #`gate_delay outA = 1;
      wait (inA == 0);
      #`gate_delay outA = 0;
    end else begin
      lastSide = 1;
      #`gate_delay outB = 1;
      wait (inB == 0);
      #`gate_delay outB = 0;
    end
  end
endmodule

module BUF (out, in);
  output out;
  input in;

  assign out = in;
endmodule

module INV (out, in);
  output out;
  input in;

  not #(`gate_delay, `gate_delay) I0 (out, in);
endmodule

module LATCH (enable, in, out);
  input enable, in;
  output out;

  reg out;

  always @(enable or in)
    if (enable) #`gate_delay out = in;
endmodule

module MUX2 (out, in0, in1, sel);
  input in0, in1, sel;
  output out;

  reg out;

  always @(in0 or in1 or sel)
    #`gate_delay
      if (sel) out = in1;
      else out = in0;
endmodule

module NMUX2 (out, in0, in1, sel);
  input in0, in1, sel;
  output out;

  reg out;

  always @(in0 or in1 or sel)
    #`gate_delay
      if (sel) out = ! in1;
      else out = ! in0;
endmodule

module DEMUX2 (in, out0, out1, sel);
  input in, sel;
  output out0, out1;

  reg out0, out1;

  always @(in or sel)
  begin
    #`gate_delay out0 = in & ! sel;
    #`gate_delay out1 = in & sel;
  end
endmodule

module NKEEP (nout, in);
  input in;
  output nout;

  reg nout;

  always @(in)
    if (in !== 1'bz) #`gate_delay nout = ! in;
endmodule

module TRIBUF (enable, in, out);
  input enable, in;
  output out;

  reg out;

  always @(in or enable)
    if (enable) #`gate_delay out = in;
    else #`gate_delay out = 1'bz;
endmodule

module TRIINV (enable, in, out);
  input enable, in;
  output out;

  reg out;

  always @(in or enable)
    if (enable) #`gate_delay out = ! in;
    else #`gate_delay out = 1'bz;
endmodule

module C2 (out, in0, in1);
  output out;
  input in0, in1;
  wire n1, n2, n3;

  and #(`gate_delay, `gate_delay) I0 (n1, in0, in1);
  and #(`gate_delay, `gate_delay) I1 (n2, in0, out);
  and #(`gate_delay, `gate_delay) I2 (n3, in1, out);
  or #(`gate_delay, `gate_delay) I3 (out, n1, n2, n3);
endmodule

module C3 (out, in0, in1, in2);
  output out;
  input in0, in1, in2;
  wire n1;

  C2 I0 (n1, in0, in1);
  C2 I1 (out, n1, in2);
endmodule

module NC2P (out, ins, inp);
  input ins, inp;
  output out;

  reg out;

  initial
  	out = 1;

  always @(ins or inp)
  	if (! ins) #`gate_delay out = 1;
  	else if (ins & inp) #`gate_delay out = 0;
endmodule

module VDD (one);
  output one;
  supply1 vdd;

  assign one = vdd;
endmodule

module GND (zero);
  output zero;
  supply0 gnd;

  assign zero = gnd;
endmodule
