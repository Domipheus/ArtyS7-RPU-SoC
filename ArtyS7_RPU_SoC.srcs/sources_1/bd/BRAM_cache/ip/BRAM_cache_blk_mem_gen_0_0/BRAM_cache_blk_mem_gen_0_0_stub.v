// Copyright 1986-2018 Xilinx, Inc. All Rights Reserved.
// --------------------------------------------------------------------------------
// Tool Version: Vivado v.2018.1 (win64) Build 2188600 Wed Apr  4 18:40:38 MDT 2018
// Date        : Wed Aug 26 11:29:17 2020
// Host        : rainbowdash running 64-bit major release  (build 9200)
// Command     : write_verilog -force -mode synth_stub
//               c:/dev/RPU/ArtyS7-RPU-SoC-git/ArtyS7_RPU_SoC.srcs/sources_1/bd/BRAM_cache/ip/BRAM_cache_blk_mem_gen_0_0/BRAM_cache_blk_mem_gen_0_0_stub.v
// Design      : BRAM_cache_blk_mem_gen_0_0
// Purpose     : Stub declaration of top-level module interface
// Device      : xc7s50csga324-2
// --------------------------------------------------------------------------------

// This empty module with port declaration file causes synthesis tools to infer a black box for IP.
// The synthesis directives are for Synopsys Synplify support to prevent IO buffer insertion.
// Please paste the declaration into a Verilog source file or add the file as an additional source.
(* x_core_info = "blk_mem_gen_v8_4_1,Vivado 2018.1" *)
module BRAM_cache_blk_mem_gen_0_0(clka, ena, wea, addra, dina, douta, clkb, enb, web, addrb, 
  dinb, doutb)
/* synthesis syn_black_box black_box_pad_pin="clka,ena,wea[0:0],addra[6:0],dina[255:0],douta[255:0],clkb,enb,web[0:0],addrb[6:0],dinb[255:0],doutb[255:0]" */;
  input clka;
  input ena;
  input [0:0]wea;
  input [6:0]addra;
  input [255:0]dina;
  output [255:0]douta;
  input clkb;
  input enb;
  input [0:0]web;
  input [6:0]addrb;
  input [255:0]dinb;
  output [255:0]doutb;
endmodule
