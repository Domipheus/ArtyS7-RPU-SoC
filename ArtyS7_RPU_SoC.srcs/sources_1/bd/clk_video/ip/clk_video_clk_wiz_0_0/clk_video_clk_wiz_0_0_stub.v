// Copyright 1986-2018 Xilinx, Inc. All Rights Reserved.
// --------------------------------------------------------------------------------
// Tool Version: Vivado v.2018.1 (win64) Build 2188600 Wed Apr  4 18:40:38 MDT 2018
// Date        : Fri Dec 14 14:49:06 2018
// Host        : rainbowdash running 64-bit major release  (build 9200)
// Command     : write_verilog -force -mode synth_stub
//               c:/dev/RPU/ArtyS7-RPU-SoC-git/ArtyS7_RPU_SoC.srcs/sources_1/bd/clk_video/ip/clk_video_clk_wiz_0_0/clk_video_clk_wiz_0_0_stub.v
// Design      : clk_video_clk_wiz_0_0
// Purpose     : Stub declaration of top-level module interface
// Device      : xc7s50csga324-2
// --------------------------------------------------------------------------------

// This empty module with port declaration file causes synthesis tools to infer a black box for IP.
// The synthesis directives are for Synopsys Synplify support to prevent IO buffer insertion.
// Please paste the declaration into a Verilog source file or add the file as an additional source.
module clk_video_clk_wiz_0_0(clk_pixel, clk_5xpixel, clk_5xpixel_inv, reset, 
  locked, clk_in1)
/* synthesis syn_black_box black_box_pad_pin="clk_pixel,clk_5xpixel,clk_5xpixel_inv,reset,locked,clk_in1" */;
  output clk_pixel;
  output clk_5xpixel;
  output clk_5xpixel_inv;
  input reset;
  output locked;
  input clk_in1;
endmodule
