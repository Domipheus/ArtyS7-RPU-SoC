// Copyright 1986-2018 Xilinx, Inc. All Rights Reserved.
// --------------------------------------------------------------------------------
// Tool Version: Vivado v.2018.1 (win64) Build 2188600 Wed Apr  4 18:40:38 MDT 2018
// Date        : Fri Jan 10 22:51:39 2020
// Host        : rainbowdash running 64-bit major release  (build 9200)
// Command     : write_verilog -force -mode synth_stub -rename_top clk_main_clk_wiz_0_0 -prefix
//               clk_main_clk_wiz_0_0_ clk_main_clk_wiz_0_0_stub.v
// Design      : clk_main_clk_wiz_0_0
// Purpose     : Stub declaration of top-level module interface
// Device      : xc7s50csga324-2
// --------------------------------------------------------------------------------

// This empty module with port declaration file causes synthesis tools to infer a black box for IP.
// The synthesis directives are for Synopsys Synplify support to prevent IO buffer insertion.
// Please paste the declaration into a Verilog source file or add the file as an additional source.
module clk_main_clk_wiz_0_0(clk_100MHz, clk_200MHz, clk_core, reset, locked, 
  clk_in1)
/* synthesis syn_black_box black_box_pad_pin="clk_100MHz,clk_200MHz,clk_core,reset,locked,clk_in1" */;
  output clk_100MHz;
  output clk_200MHz;
  output clk_core;
  input reset;
  output locked;
  input clk_in1;
endmodule
