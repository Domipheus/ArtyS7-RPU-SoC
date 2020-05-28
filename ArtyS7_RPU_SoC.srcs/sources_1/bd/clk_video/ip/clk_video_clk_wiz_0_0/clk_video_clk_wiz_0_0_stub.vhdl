-- Copyright 1986-2018 Xilinx, Inc. All Rights Reserved.
-- --------------------------------------------------------------------------------
-- Tool Version: Vivado v.2018.1 (win64) Build 2188600 Wed Apr  4 18:40:38 MDT 2018
-- Date        : Fri Dec 14 14:49:06 2018
-- Host        : rainbowdash running 64-bit major release  (build 9200)
-- Command     : write_vhdl -force -mode synth_stub
--               c:/dev/RPU/ArtyS7-RPU-SoC-git/ArtyS7_RPU_SoC.srcs/sources_1/bd/clk_video/ip/clk_video_clk_wiz_0_0/clk_video_clk_wiz_0_0_stub.vhdl
-- Design      : clk_video_clk_wiz_0_0
-- Purpose     : Stub declaration of top-level module interface
-- Device      : xc7s50csga324-2
-- --------------------------------------------------------------------------------
library IEEE;
use IEEE.STD_LOGIC_1164.ALL;

entity clk_video_clk_wiz_0_0 is
  Port ( 
    clk_pixel : out STD_LOGIC;
    clk_5xpixel : out STD_LOGIC;
    clk_5xpixel_inv : out STD_LOGIC;
    reset : in STD_LOGIC;
    locked : out STD_LOGIC;
    clk_in1 : in STD_LOGIC
  );

end clk_video_clk_wiz_0_0;

architecture stub of clk_video_clk_wiz_0_0 is
attribute syn_black_box : boolean;
attribute black_box_pad_pin : string;
attribute syn_black_box of stub : architecture is true;
attribute black_box_pad_pin of stub : architecture is "clk_pixel,clk_5xpixel,clk_5xpixel_inv,reset,locked,clk_in1";
begin
end;
