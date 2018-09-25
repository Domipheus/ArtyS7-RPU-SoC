// RPU ArtyS7-50 SoC system header.
// 
// Copyright 2018 Colin Riley
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef __RPU_SYSTEM_ARTYS7_H__
#define __RPU_SYSTEM_ARTYS7_H__

#include <stdint.h>

/******************************************************************************
 *  Textual console I/O
 *  
 *  Default driver of HDMI out at reset. Works akin to old text mode interfaces
 *  A default font is loaded automatically. Writing to the text buffer using
 *  rows and columns 2D addressing allows each screen character to be 
 *  written using the sys_charEntry_t structure.
 *
 *****************************************************************************/

// Memory interface to textual console display
#define SYS_CONSOLE_TEXT_BUFFER_BEGIN 0x10000
#define SYS_CONSOLE_FONT_BUFFER_BEGIN 0x20000 //unsupported

// Default console dimensions
#define SYS_CONSOLE_TEXT_DEFAULT_ROWS 45
#define SYS_CONSOLE_TEXT_DEFAULT_COLS 160

// Text modifiers
#define COLOUR_BLACK        0
#define COLOUR_BLUE         1
#define COLOUR_GREEN        2
#define COLOUR_CYAN         3
#define COLOUR_RED          4
#define COLOUR_MAGENTA      5
#define COLOUR_BROWN        6
#define COLOUR_LIGHTGRAY    7
#define COLOUR_DARKGRAY     8
#define COLOUR_LIGHTBLUE    9
#define COLOUR_LIGHTGREEN   10
#define COLOUR_LIGHTCYAN    11
#define COLOUR_LIGHTRED     12
#define COLOUR_LIGHTMAGENTA 13
#define COLOUR_YELLOW       14
#define COLOUR_WHITE        15

// Helper for creating character colour code
//     (blink) | (Background Colour) | (Foreground Colour)
//      1 bit  |     3 bits (0-7)    |    4 bits (0-15)
#define CREATE_PEN(BLINK, BG, FG) (((BLINK)<<7) | (((BG)&7)<<4) | (FG&15))

typedef struct sys_charEntry_s
{
  uint8_t colour;
  int8_t           character;
} sys_charEntry_t;

/******************************************************************************
 *  SPI Master interface 1 I/O
 *  
 *  Operates out of PMOD JD, with 4 chip select outputs.
 *
 *****************************************************************************/

#define SPI_M1_CONFIG_REG_ADDR 0xf0009300
#define SPI_M1_BUSY_REG_ADDR   0xf0009304
#define SPI_M1_DATA_REG_ADDR   0xf0009308
#define SPI_M1_CLKDIV_REG_ADDR 0xf000930c
#define SPI_M1_SELECT_REG_ADDR 0xf000930e /// this isnt right - should be a 16 bit write?

/******************************************************************************
 *  Counter memory locations and structure definitions
 *  
 *  Counts for clock details, performance counters, etc.
 *
 *****************************************************************************/

#define IO_COUNTER_CLOCK_START         0xFF001000
#define IO_COUNTER_CLOCK_CYCLES_CORE   0xFF001000  
#define IO_COUNTER_CLOCK_CYCLES_100MHZ 0xFF001004
#define IO_COUNTER_CLOCK_FREQ_CORE     0xFF001008
#define IO_COUNTER_CLOCK_FREQ_MEM      0xFF00100C
#define IO_COUNTER_CLOCK_FREQ_SOC      0xFF001010

#define IO_COUNTER_DDR3_START                       0xFF001100
#define IO_COUNTER_DDR3_READ_REQ                    0xFF001100
#define IO_COUNTER_DDR3_WRITE_REQ                   0xFF001104
#define IO_COUNTER_DDR3_CMDREADY_READ_WAIT_CYCLES   0xFF001108
#define IO_COUNTER_DDR3_READ_WAIT_CYCLES            0xFF00110C
#define IO_COUNTER_DDR3_CMDREADY_WRITE_WAIT_CYCLES  0xFF001110
#define IO_COUNTER_DDR3_WRITE_WAIT_CYCLES           0xFF001114

#define MEM_DDR3_START 0x10000000
#define MEM_BRAM_START 0x00000000

#endif //__RPU_SYSTEM_ARTYS7_H__
