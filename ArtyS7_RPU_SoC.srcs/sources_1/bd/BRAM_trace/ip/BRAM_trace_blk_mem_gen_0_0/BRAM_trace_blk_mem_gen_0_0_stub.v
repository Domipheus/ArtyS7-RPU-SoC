// Copyright 1986-2018 Xilinx, Inc. All Rights Reserved.
// --------------------------------------------------------------------------------
// Tool Version: Vivado v.2018.1 (win64) Build 2188600 Wed Apr  4 18:40:38 MDT 2018
// Date        : Tue Dec 11 12:46:48 2018
// Host        : rainbowdash running 64-bit major release  (build 9200)
// Command     : write_verilog -force -mode synth_stub
//               c:/dev/RPU/ArtyS7-RPU-SoC-git/ArtyS7_RPU_SoC.srcs/sources_1/bd/BRAM_trace/ip/BRAM_trace_blk_mem_gen_0_0/BRAM_trace_blk_mem_gen_0_0_stub.v
// Design      : BRAM_trace_blk_mem_gen_0_0
// Purpose     : Stub declaration of top-level module interface
// Device      : xc7s50csga324-2
// --------------------------------------------------------------------------------

// This empty module with port declaration file causes synthesis tools to infer a black box for IP.
// The synthesis directives are for Synopsys Synplify support to prevent IO buffer insertion.
// Please paste the declaration into a Verilog source file or add the file as an additional source.
(* x_core_info = "blk_mem_gen_v8_4_1,Vivado 2018.1" *)
module BRAM_trace_blk_mem_gen_0_0(clka, ena, wea, addra, dina, douta, clkb, enb, web, addrb, 
  dinb, doutb)
/* synthesis syn_black_box black_box_pad_pin="clka,ena,wea[0:0],addra[11:0],dina[63:0],douta[63:0],clkb,enb,web[0:0],addrb[11:0],dinb[63:0],doutb[63:0]" */;
  input clka;
  input ena;
  input [0:0]wea;
  input [11:0]addra;
  input [63:0]dina;
  output [63:0]douta;
  input clkb;
  input enb;
  input [0:0]web;
  input [11:0]addrb;
  input [63:0]dinb;
  output [63:0]doutb;
endmodule
