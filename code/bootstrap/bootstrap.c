/*
******************************************************************************
* Arty S7-50 RPU SoC Bootloader. This is the first code which runs on boot.
*
* It's very messy. When the hardware memory maps are more fixed this will be
* cleaned up.
*
* Code within this file is derived from many sources:
*   http://codeandlife.com/2012/04/02/simple-fat-and-sd-tutorial-part-1/
*   http://elm-chan.org/fsw/ff/00index_p.html
*   https://github.com/lowRISC/lowrisc-bbl
*
* Edits by Colin Riley @domipheus
*
******************************************************************************
*/
/*---------------------------------------------------------------------------/
/ This file contains content derived from lowrisc-bbl for Elf Loading.
/----------------------------------------------------------------------------/
* Copyright (c) 2013-2016, The Regents of the University of California
(Regents).
* All Rights Reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
* 1. Redistributions of source code must retain the above copyright
*    notice, this list of conditions and the following disclaimer.
* 2. Redistributions in binary form must reproduce the above copyright
*    notice, this list of conditions and the following disclaimer in the
*    documentation and/or other materials provided with the distribution.
* 3. Neither the name of the Regents nor the
*    names of its contributors may be used to endorse or promote products
*    derived from this software without specific prior written permission.
*
* IN NO EVENT SHALL REGENTS BE LIABLE TO ANY PARTY FOR DIRECT, INDIRECT,
* SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES, INCLUDING LOST PROFITS, ARISING
* OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF REGENTS HAS
* BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
* REGENTS SPECIFICALLY DISCLAIMS ANY WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
* THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
* PURPOSE. THE SOFTWARE AND ACCOMPANYING DOCUMENTATION, IF ANY, PROVIDED
* HEREUNDER IS PROVIDED "AS IS". REGENTS HAS NO OBLIGATION TO PROVIDE
* MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.
*/
/*---------------------------------------------------------------------------/
/ This file contains content derived from Petit FatFs for SD Fat32 Loading.
/----------------------------------------------------------------------------/
/ Petit FatFs module is an open source software to implement FAT file system to
/ small embedded systems. This is a free software and is opened for education,
/ research and commercial developments under license policy of following trems.
/
/  Copyright (C) 2014, ChaN, all right reserved.
/
/ * The Petit FatFs module is a free software and there is NO WARRANTY.
/ * No restriction on use. You can use, modify and redistribute it for
/   personal, non-profit or commercial use UNDER YOUR RESPONSIBILITY.
/ * Redistributions of source code must retain the above copyright notice.
/
/----------------------------------------------------------------------------*/
#include <rpu_system_artys7.h>
#define VERSION 0x0001000b


typedef uint16_t WORD;
typedef uint32_t DWORD;
typedef uint32_t UINT;
typedef uint8_t BYTE;

typedef uint32_t size_t;

typedef uint32_t Elf32_Addr; // Program address
typedef uint32_t Elf32_Off;  // File offset
typedef uint16_t Elf32_Half;
typedef uint32_t Elf32_Word;
typedef int32_t Elf32_Sword;

// e_ident size and indices.
enum {
  EI_MAG0 = 0,       // File identification index.
  EI_MAG1 = 1,       // File identification index.
  EI_MAG2 = 2,       // File identification index.
  EI_MAG3 = 3,       // File identification index.
  EI_CLASS = 4,      // File class.
  EI_DATA = 5,       // Data encoding.
  EI_VERSION = 6,    // File version.
  EI_OSABI = 7,      // OS/ABI identification.
  EI_ABIVERSION = 8, // ABI version.
  EI_PAD = 9,        // Start of padding bytes.
  EI_NIDENT = 16     // Number of bytes in e_ident.
};

enum {
  PT_NULL = 0,    // Unused segment.
  PT_LOAD = 1,    // Loadable segment.
  PT_DYNAMIC = 2, // Dynamic linking information.
  PT_INTERP = 3,  // Interpreter pathname.
  PT_NOTE = 4,    // Auxiliary information.
  PT_SHLIB = 5,   // Reserved.
  PT_PHDR = 6,    // The program header table itself.
  PT_TLS = 7,     // The thread-local storage template.
};

typedef struct Elf_Ehdr_s {
  unsigned char e_ident[EI_NIDENT]; // ELF Identification bytes
  Elf32_Half e_type;                // Type of file (see ET_* below)
  Elf32_Half e_machine;   // Required architecture for this file (see EM_*)
  Elf32_Word e_version;   // Must be equal to 1
  Elf32_Addr e_entry;     // Address to jump to in order to start program
  Elf32_Off e_phoff;      // Program header table's file offset, in bytes
  Elf32_Off e_shoff;      // Section header table's file offset, in bytes
  Elf32_Word e_flags;     // Processor-specific flags
  Elf32_Half e_ehsize;    // Size of ELF header, in bytes
  Elf32_Half e_phentsize; // Size of an entry in the program header table
  Elf32_Half e_phnum;     // Number of entries in the program header table
  Elf32_Half e_shentsize; // Size of an entry in the section header table
  Elf32_Half e_shnum;     // Number of entries in the section header table
  Elf32_Half e_shstrndx;  // Sect hdr table index of sect name string table
} __attribute((packed)) Elf_Ehdr;

// Program header for ELF32.
typedef struct Elf_Phdr_s {
  Elf32_Word p_type;   // Type of segment
  Elf32_Off p_offset;  // File offset where segment is located, in bytes
  Elf32_Addr p_vaddr;  // Virtual address of beginning of segment
  Elf32_Addr p_paddr;  // Physical address of beginning of segment (OS-specific)
  Elf32_Word p_filesz; // Num. of bytes in file image of segment (may be zero)
  Elf32_Word p_memsz;  // Num. of bytes in mem image of segment (may be zero)
  Elf32_Word p_flags;  // Segment flags
  Elf32_Word p_align;  // Segment alignment constraint
} __attribute((packed)) Elf_Phdr;

typedef struct {
  uintptr_t entry;
  uintptr_t first_user_vaddr;
  uintptr_t first_vaddr_after_user;
  uintptr_t load_offset;
} kernel_elf_info;

#define RISCV_PGSIZE 0x1000
#define MEGAPAGE_SIZE 0x1000

/* Status of Disk Functions */
typedef BYTE DSTATUS;
/* Results of Disk Functions */
typedef enum {
  RES_OK = 0, /* 0: Function succeeded */
  RES_ERROR,  /* 1: Disk error */
  RES_NOTRDY, /* 2: Not ready */
  RES_PARERR  /* 3: Invalid parameter */
} DRESULT;

DSTATUS disk_initialize(void);
DRESULT disk_readp(BYTE *buff, DWORD sector, UINT offser, UINT count);
DRESULT disk_writep(const BYTE *buff, DWORD sc);

#define STA_NOINIT 0x01 /* Drive not initialized */
#define STA_NODISK 0x02 /* No medium in the drive */

static int screen_row = 0;
static int screen_col = 0;
static unsigned char screen_pen = 0x0;


int strlen(const char *str) {
  int len = 0;

  while (*(str++) != '\0') {
    len++;
  }

  return len;
}

void screen_setpen(unsigned char pen) { screen_pen = pen; }

void screen_clear(unsigned char pen) {
  sys_charEntry_t *textBuffer = (sys_charEntry_t *)SYS_CONSOLE_TEXT_BUFFER_BEGIN;

  int i = 0;
  for (; i < (SYS_CONSOLE_TEXT_DEFAULT_COLS * SYS_CONSOLE_TEXT_DEFAULT_ROWS); i++) {
    textBuffer[i].character = ' ';
    textBuffer[i].colour = pen;
  }
}

void screen_setcursor(int x, int y) {
  screen_col = x;
  screen_row = y;

  if (screen_col == SYS_CONSOLE_TEXT_DEFAULT_COLS) {
    screen_col = 0;
    screen_row++;
  }
  if (screen_row == SYS_CONSOLE_TEXT_DEFAULT_ROWS) {
    screen_row = 0;
  }
}

void put_stringn(const char *text, uint32_t len) {
  sys_charEntry_t *textBuffer = (sys_charEntry_t *)SYS_CONSOLE_TEXT_BUFFER_BEGIN;

  int i = 0;
  for (; i < len; i++) {
    if (screen_col == SYS_CONSOLE_TEXT_DEFAULT_COLS) {
      screen_col = 0;
      screen_row++;
    }
    if (screen_row == SYS_CONSOLE_TEXT_DEFAULT_ROWS) {
      screen_row = 0;
    }
    if (text[i] == '\n') {
      screen_row++;
      screen_col = 0;
    } else {
      textBuffer[SYS_CONSOLE_TEXT_DEFAULT_COLS * screen_row + screen_col].character = text[i];
      textBuffer[SYS_CONSOLE_TEXT_DEFAULT_COLS * screen_row + screen_col].colour = screen_pen;
      screen_col++;
    }
  }
}

void put_string(const char *text) {
  int len = strlen(text);
  put_stringn(text, len);
}

void put_char(unsigned char b) {
  static char buff[2];
  buff[0] = b;
  buff[1] = '\0';
  put_string(buff);
}

void put_hexbyte(unsigned char b) {
  static char hexlookup[] = {'0', '1', '2', '3', '4', '5', '6', '7',
                             '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};

  static char buff[4];
  unsigned char b0 = b & 0xfu;
  unsigned char b1 = (b >> 4u) & 0xfu;
  buff[0] = hexlookup[b1];
  buff[1] = hexlookup[b0];
  buff[2] = '\0';

  put_string(buff);
}

void put_hexword(unsigned int i) {
  put_hexbyte(i >> 24);
  put_hexbyte(i >> 16);
  put_hexbyte(i >> 8);
  put_hexbyte(i);
}

////////////////////////////////////////////////////////// SD CARD SPI
/* Status of Disk Functions */
typedef BYTE DSTATUS;

/* MMC/SD command */
#define CMD0 (0)           /* GO_IDLE_STATE */
#define CMD1 (1)           /* SEND_OP_COND (MMC) */
#define ACMD41 (0x80 + 41) /* SEND_OP_COND (SDC) */
#define CMD8 (8)           /* SEND_IF_COND */
#define CMD9 (9)           /* SEND_CSD */
#define CMD10 (10)         /* SEND_CID */
#define CMD12 (12)         /* STOP_TRANSMISSION */
#define ACMD13 (0x80 + 13) /* SD_STATUS (SDC) */
#define CMD16 (16)         /* SET_BLOCKLEN */
#define CMD17 (17)         /* READ_SINGLE_BLOCK */
#define CMD18 (18)         /* READ_MULTIPLE_BLOCK */
#define CMD23 (23)         /* SET_BLOCK_COUNT (MMC) */
#define ACMD23 (0x80 + 23) /* SET_WR_BLK_ERASE_COUNT (SDC) */
#define CMD24 (24)         /* WRITE_BLOCK */
#define CMD25 (25)         /* WRITE_MULTIPLE_BLOCK */
#define CMD32 (32)         /* ERASE_ER_BLK_START */
#define CMD33 (33)         /* ERASE_ER_BLK_END */
#define CMD38 (38)         /* ERASE */
#define CMD55 (55)         /* APP_CMD */
#define CMD58 (58)         /* READ_OCR */

/* MMC card type flags (MMC_GET_TYPE) */
#define CT_MMC 0x01              /* MMC ver 3 */
#define CT_SD1 0x02              /* SD ver 1 */
#define CT_SD2 0x04              /* SD ver 2 */
#define CT_SDC (CT_SD1 | CT_SD2) /* SD */
#define CT_BLOCK 0x08            /* Block addressing */

/** status for card in the ready state */

#define R1_READY_STATE 0X00

/** status for card in the idle state */
#define R1_IDLE_STATE 0X01

/** status bit for illegal command */
#define R1_ILLEGAL_COMMAND 0X04

/** start data token for read or write single block*/
#define DATA_START_BLOCK 0XFE

/** stop token for write multiple blocks*/
#define STOP_TRAN_TOKEN 0XFD

/** start data token for write multiple blocks*/
#define WRITE_MULTIPLE_TOKEN 0XFC

/** mask for data response tokens after a write block operation */
#define DATA_RES_MASK 0X1F

/** write data accepted token */
#define DATA_RES_ACCEPTED 0X05

//------------------------------------------------------------------------------
typedef struct CID {
  // byte 0
  uint8_t mid; // Manufacturer ID
  // byte 1-2
  char oid[2]; // OEM/Application ID
  // byte 3-7
  char pnm[5]; // Product name
  // byte 8
  unsigned prv_m : 4; // Product revision n.m
  unsigned prv_n : 4;
  // byte 9-12
  uint32_t psn; // Product serial number
  // byte 13
  unsigned mdt_year_high : 4; // Manufacturing date
  unsigned reserved : 4;
  // byte 14
  unsigned mdt_month : 4;
  unsigned mdt_year_low : 4;
  // byte 15
  unsigned always1 : 1;
  unsigned crc : 7;
} cid_t;
//------------------------------------------------------------------------------
// CSD for version 1.00 cards
typedef struct CSDV1 {
  // byte 0
  unsigned reserved1 : 6;
  unsigned csd_ver : 2;
  // byte 1
  uint8_t taac;
  // byte 2
  uint8_t nsac;
  // byte 3
  uint8_t tran_speed;
  // byte 4
  uint8_t ccc_high;
  // byte 5
  unsigned read_bl_len : 4;
  unsigned ccc_low : 4;
  // byte 6
  unsigned c_size_high : 2;
  unsigned reserved2 : 2;
  unsigned dsr_imp : 1;
  unsigned read_blk_misalign : 1;
  unsigned write_blk_misalign : 1;
  unsigned read_bl_partial : 1;
  // byte 7
  uint8_t c_size_mid;
  // byte 8
  unsigned vdd_r_curr_max : 3;
  unsigned vdd_r_curr_min : 3;
  unsigned c_size_low : 2;
  // byte 9
  unsigned c_size_mult_high : 2;
  unsigned vdd_w_cur_max : 3;
  unsigned vdd_w_curr_min : 3;
  // byte 10
  unsigned sector_size_high : 6;
  unsigned erase_blk_en : 1;
  unsigned c_size_mult_low : 1;
  // byte 11
  unsigned wp_grp_size : 7;
  unsigned sector_size_low : 1;
  // byte 12
  unsigned write_bl_len_high : 2;
  unsigned r2w_factor : 3;
  unsigned reserved3 : 2;
  unsigned wp_grp_enable : 1;
  // byte 13
  unsigned reserved4 : 5;
  unsigned write_partial : 1;
  unsigned write_bl_len_low : 2;
  // byte 14
  unsigned reserved5 : 2;
  unsigned file_format : 2;
  unsigned tmp_write_protect : 1;
  unsigned perm_write_protect : 1;
  unsigned copy : 1;
  unsigned file_format_grp : 1;
  // byte 15
  unsigned always1 : 1;
  unsigned crc : 7;
} csd1_t;
//------------------------------------------------------------------------------
// CSD for version 2.00 cards
typedef struct CSDV2 {
  // byte 0
  unsigned reserved1 : 6;
  unsigned csd_ver : 2;
  // byte 1
  uint8_t taac;
  // byte 2
  uint8_t nsac;
  // byte 3
  uint8_t tran_speed;
  // byte 4
  uint8_t ccc_high;
  // byte 5
  unsigned read_bl_len : 4;
  unsigned ccc_low : 4;
  // byte 6
  unsigned reserved2 : 4;
  unsigned dsr_imp : 1;
  unsigned read_blk_misalign : 1;
  unsigned write_blk_misalign : 1;
  unsigned read_bl_partial : 1;
  // byte 7
  unsigned reserved3 : 2;
  unsigned c_size_high : 6;
  // byte 8
  uint8_t c_size_mid;
  // byte 9
  uint8_t c_size_low;
  // byte 10
  unsigned sector_size_high : 6;
  unsigned erase_blk_en : 1;
  unsigned reserved4 : 1;
  // byte 11
  unsigned wp_grp_size : 7;
  unsigned sector_size_low : 1;
  // byte 12
  unsigned write_bl_len_high : 2;
  unsigned r2w_factor : 3;
  unsigned reserved5 : 2;
  unsigned wp_grp_enable : 1;
  // byte 13
  unsigned reserved6 : 5;
  unsigned write_partial : 1;
  unsigned write_bl_len_low : 2;
  // byte 14
  unsigned reserved7 : 2;
  unsigned file_format : 2;
  unsigned tmp_write_protect : 1;
  unsigned perm_write_protect : 1;
  unsigned copy : 1;
  unsigned file_format_grp : 1;
  // byte 15
  unsigned always1 : 1;
  unsigned crc : 7;
} csd2_t;
//------------------------------------------------------------------------------
// union of old and new style CSD register
typedef union csd_u {
  csd1_t v1;
  csd2_t v2;
} csd_t;

#define SPI_M1_CONFIG_REG_ADDR 0xf0009300
#define SPI_M1_BUSY_REG_ADDR 0xf0009304
#define SPI_M1_DATA_REG_ADDR 0xf0009308
#define SPI_M1_CLKDIV_REG_ADDR 0xf000930c
#define SPI_M1_SELECT_REG_ADDR 0xf000930e

void *memcpy(void *dest, const void *src, uint32_t len) {
  const char *s = src;
  char *d = dest;

  while (d < (char *)(dest + len))
    *d++ = *s++;

  return dest;
}

void *memset(void *dest, int byte, uint32_t len) {

  char *d = dest;
  while (d < (char *)(dest + len)) {
    *d++ = byte;
  }
  return dest;
}

void spi_set_clkdivider(unsigned int divisor) {
  volatile unsigned int *spi_config_reg =
      (unsigned int *)SPI_M1_CLKDIV_REG_ADDR;

  *spi_config_reg = divisor;
}

void spi_set_deviceaddr(unsigned int device) {
  volatile unsigned int *spi_config_reg =
      (unsigned int *)SPI_M1_SELECT_REG_ADDR;

  *spi_config_reg = device;
}

/* Exchange a byte */
static BYTE xchg_spi(BYTE dat /* Data to send */
) {
  volatile unsigned int *spi_busy_reg = (unsigned int *)SPI_M1_BUSY_REG_ADDR;
  volatile unsigned int *spi_data_reg = (unsigned int *)SPI_M1_DATA_REG_ADDR;

  *spi_data_reg = (unsigned int)dat;

  while (*spi_busy_reg != 0)
    ;

  return *spi_data_reg;
}

/*-----------------------------------------------------------------------*/
/* Deselect card and release SPI                                         */
/*-----------------------------------------------------------------------*/

#define deselect() spi_set_deviceaddr(0)
#define select() spi_set_deviceaddr(1)

void dummyclocks(BYTE n) {
  for (; n; n--) {
    xchg_spi(0xFF); /* Send dummy clocks */
  }
}

static BYTE send_cmd(/* Return value: R1 resp (bit7==1:Failed to send) */
                     BYTE cmd, /* Command index */
                     DWORD arg /* Argument */
) {
  BYTE n, res;

  if (cmd & 0x80) { /* Send a CMD55 prior to ACMD<n> */
    cmd &= 0x7F;
    res = send_cmd(CMD55, 0);
    if (res > 1)
      return res;
  }

  /* Select the card and wait for ready except to stop multiple block read */
  if (cmd != CMD12) {
    deselect(); 
    select();
  }

  /* Send command packet */
  xchg_spi(0x40 | cmd);        /* Start + command index */
  xchg_spi((BYTE)(arg >> 24)); /* Argument[31..24] */
  xchg_spi((BYTE)(arg >> 16)); /* Argument[23..16] */
  xchg_spi((BYTE)(arg >> 8));  /* Argument[15..8] */
  xchg_spi((BYTE)arg);         /* Argument[7..0] */
  n = 0x01;                    /* Dummy CRC + Stop */
  if (cmd == CMD0)
    n = 0x95; /* Valid CRC for CMD0(0) */
  if (cmd == CMD8)
    n = 0x87; /* Valid CRC for CMD8(0x1AA) */
  xchg_spi(n);

  /* Receive command resp */
  if (cmd == CMD12)
    xchg_spi(0xFF); /* Diacard following one byte when CMD12 */
  n = 10;           /* Wait for response (10 bytes max) */
  do
    res = xchg_spi(0xFF);
  while ((res & 0x80) && --n);

  return res; /* Return received response */
}

BYTE CardType;

DSTATUS disk_initialize() {
  BYTE n, cmd, ty, ocr[4];

  CardType = 0;

  spi_set_clkdivider(1000);

  deselect();
  for (n = 10; n; n--)
    xchg_spi(0xFF); /* Send 80 dummy clocks */
                    // select();

  ty = 0;
  if (send_cmd(CMD0, 0) == 1) {       /* Put the card SPI/Idle state */
    UINT Timer1 = 1000000;            /* Initialization timeout = 1 sec */
                                      //	put_string(":0G");
    if (send_cmd(CMD8, 0x1AA) == 1) { /* SDv2? */
      for (n = 0; n < 4; n++)
        ocr[n] = xchg_spi(0xFF); /* Get 32 bit return value of R7 resp */
      if (ocr[2] == 0x01 &&
          ocr[3] == 0xAA) { /* Is the card supports vcc of 2.7-3.6V? */
        while (Timer1-- && send_cmd(ACMD41, 1UL << 30))
          ; /* Wait for end of initialization with ACMD41(HCS) */
        if (Timer1 && send_cmd(CMD58, 0) == 0) { /* Check CCS bit in the OCR */
          for (n = 0; n < 4; n++)
            ocr[n] = xchg_spi(0xFF);
          ty = (ocr[0] & 0x40) ? CT_SD2 | CT_BLOCK : CT_SD2; /* Card id SDv2 */
        }
      }
    } else {                          /* Not SDv2 card */
      if (send_cmd(ACMD41, 0) <= 1) { /* SDv1 or MMC? */
        ty = CT_SD1;
        cmd = ACMD41; /* SDv1 (ACMD41(0)) */
      } else {
        ty = CT_MMC;
        cmd = CMD1; /* MMCv3 (CMD1(0)) */
      }
      while (Timer1-- && send_cmd(cmd, 0))
        ; /* Wait for end of initialization */
      if (!Timer1 || send_cmd(CMD16, 512) != 0) /* Set block length: 512 */
        ty = 0;
    }
  }
  CardType = ty; /* Card type */
  deselect();

  if (ty) { /* OK */

    spi_set_clkdivider(200); // faster SPI speed
    return 0;
  }

  return STA_NOINIT;
}

DRESULT
disk_readp(BYTE *buff, /* Pointer to the read buffer (NULL:Read bytes are
                          forwarded to the stream) */
           DWORD lba,  /* Sector number (LBA) */
           UINT ofs,   /* Byte offset to read from (0..511) */
           UINT cnt    /* Number of bytes to read (ofs + cnt mus be <= 512) */
) {
  DRESULT res;
  BYTE rc;
  DWORD bc;

  res = RES_ERROR;
  if (send_cmd(CMD17, lba) == 0) { /* READ_SINGLE_BLOCK */

    bc = 160000;
    do { /* Wait for data packet */
      rc = xchg_spi(0xff);
    } while (rc == 0xFF && --bc);

    if (rc == 0xFE) { /* A data packet arrived */
      bc = 514 - ofs - cnt;

      /* Skip leading bytes */
      if (ofs) {
        do
          xchg_spi(0xff);
        while (--ofs);
      }

      /* Receive a part of the sector */
      if (buff) { /* Store data to the memory */
        do {
          // put_hexbyte(*buff++ = xchg_spi(0xff));
          *buff++ = xchg_spi(0xff);
        } while (--cnt);
      } else { /* Forward data to the outgoing stream (depends on the project)
                */
        do {
          put_hexbyte(xchg_spi(0xff));
        } while (--cnt);
      }

      /* Skip trailing bytes and CRC */
      do
        xchg_spi(0xff);
      while (--bc);

      res = RES_OK;
    }
  }

  deselect();
  xchg_spi(0xff);

  return res;
}

void error(uint8_t e) {
  put_string(" ERR:");
  put_hexbyte(e);
  while (1)
    ;
}

void load_kernel_elf(void *blob, size_t size, kernel_elf_info *info) {
  Elf_Ehdr *eh = blob;
  if (sizeof(*eh) > size ||
      !(eh->e_ident[0] == '\177' && eh->e_ident[1] == 'E' &&
        eh->e_ident[2] == 'L' && eh->e_ident[3] == 'F')) {
    put_string("\nELF header missing?");
 
  } else {
    put_string("\nELF header found...");
  }
 

  uintptr_t min_vaddr = -1, max_vaddr = 0;
  size_t phdr_size = eh->e_phnum * sizeof(Elf_Ehdr);
  Elf_Phdr *ph = blob + eh->e_phoff;

  if (eh->e_phoff + phdr_size > size) {
    put_string("\nblob too small");
    goto fail;
  }
  uint32_t first_free_paddr =
      0x0000;  

  put_string("\nProgram Headers:");
  for (int i = 0; i < eh->e_phnum; i++) { 
    put_string("\n  ");
    put_hexbyte(i);
    put_string(": t:");
    put_hexbyte(ph[i].p_type);
    put_string(" o:");
    put_hexword(ph[i].p_offset);
    put_string(" va:");
    put_hexword(ph[i].p_vaddr);
    put_string(" pa:");
    put_hexword(ph[i].p_paddr);
    put_string(" fz:");
    put_hexword(ph[i].p_filesz);
    put_string(" mz:");
    put_hexword(ph[i].p_memsz);
    if (ph[i].p_type == PT_LOAD && ph[i].p_memsz && ph[i].p_vaddr < min_vaddr) {
      min_vaddr = ph[i].p_vaddr;
    }
  }
  uintptr_t bias = first_free_paddr - min_vaddr;
  for (int i = eh->e_phnum - 1; i >= 0; i--) {
    if (ph[i].p_type == PT_LOAD && ph[i].p_memsz) {
      uintptr_t vaddr = ph[i].p_vaddr + bias;

      if (vaddr + ph[i].p_memsz > max_vaddr) {
        max_vaddr = vaddr + ph[i].p_memsz;
      }

      if (ph[i].p_offset + ph[i].p_filesz > size) {
        goto fail;
      }
      put_string("\nmemcpy( ");
      put_hexword(ph[i].p_vaddr); // vaddr);
      put_string(", ");
      put_hexword((unsigned int)(blob + ph[i].p_offset));
      put_string(", ");
      put_hexword(ph[i].p_filesz);
      put_string(") ");
      put_hexword(ph[i].p_offset);
      memcpy((void *)ph[i].p_vaddr, blob + ph[i].p_offset, ph[i].p_filesz);
 
    }
  }

  info->entry = eh->e_entry;
  info->load_offset = bias;
  info->first_user_vaddr = min_vaddr;
  info->first_vaddr_after_user =
      max_vaddr - bias; 
  return;

fail:
  error(0xde); 
}

void die(         /* Stop with dying message */
         DWORD rc /* FatFs return value */
) {
  put_string("Failed with rc=");
  put_hexbyte(rc >> 24);
  put_hexbyte(rc >> 16);
  put_hexbyte(rc >> 8);
  put_hexbyte(rc);
  for (;;)
    ;
}

BYTE buff[64];

typedef struct {
  unsigned char first_byte;
  unsigned char start_chs[3];
  unsigned char partition_type;
  unsigned char end_chs[3];
  unsigned long start_sector;
  unsigned long length_sectors;
} __attribute((packed)) PartitionTable;

typedef struct FAT32BootSector_s {
  uint8_t jmp[3];
  char oem[8];
  uint16_t sector_size;
  uint8_t sectors_per_cluster;
  uint16_t reserved_sectors;
  uint8_t number_of_fats;
  uint16_t root_dir_entries;
  uint16_t total_sectors_short; // if zero, later field is used
  uint8_t media_descriptor;
  uint16_t fat_size_sectors;
  uint16_t sectors_per_track;
  uint16_t number_of_heads;
  uint32_t hidden_sectors;
  uint32_t total_sectors_long;

  uint32_t f32_num_sectors_1fat;
  uint16_t f32_mirroring_flags;
  uint16_t f32_fs_version;
  uint32_t f32_cluster_for_root;
  uint16_t f32_sector_fsinfo;
  uint16_t f32_sector_backup_boot_sector;
  uint8_t f32_reserved[12];

  uint8_t drive_number;
  uint8_t current_head;
  uint8_t boot_signature;
  uint32_t volume_id;
  char volume_label[11];
  char fs_type[8];
} __attribute((packed)) FAT32BootSector_t;

typedef struct FileEntry_s {
  char name[11];
  uint8_t attrib;
  // uint8_t reserved[10];
  uint16_t ntres;
  uint16_t create_time;
  uint16_t create_date;
  uint16_t reserved;
  uint8_t cluster_hi_l;
  uint8_t cluster_hi_h;
  uint16_t mod_time;
  uint16_t mod_data;
  uint8_t cluster_low_l;
  uint8_t cluster_low_h;
  uint32_t size;
} __attribute((packed)) FileEntry_t;

#define IO_ADDR_LEDS 0xf0009000
#define MEM_BRAM_SCRATCH 0x18000
#define MEM_TEST_LENGTH 0x00fff

FAT32BootSector_t gBootSector;
PartitionTable gPartitions[1];
BYTE gSignature[2];
volatile uint32_t timer_count = 0;

#define IO_DDR3_CAL_COMPLETE 0xfdd30000
#define IO_DDR3_APP_RDY 0xfdd30004
#define IO_DDR3_APP_WDF_RDY 0xfdd30008

void BasicMemoryTest() {
  int offset = 0;
  int testpass = 1;
  unsigned int startpattern = 0x87654321;

  unsigned int pattern = 0;
  unsigned short pattern16 = 0;
  unsigned char pattern8 = 0;

  unsigned int *baseAddress32 = (unsigned int *)MEM_BRAM_SCRATCH;
  unsigned short *baseAddress16 = (unsigned short *)MEM_BRAM_SCRATCH;
  unsigned char *baseAddress8 = (unsigned char *)MEM_BRAM_SCRATCH;

  put_string("Memory Test...................\n");

  screen_setcursor(4, 4);
  put_string("32 bit ops............");

  for (pattern = startpattern, offset = 0; offset < MEM_TEST_LENGTH / 4;
       pattern++, offset++) {
    baseAddress32[offset] = pattern;
  }

  for (pattern = startpattern, offset = 0; offset < MEM_TEST_LENGTH / 4;
       pattern++, offset++) {
    if (baseAddress32[offset] != pattern) {
      testpass = 0;
      put_string("MEM 0x");
      put_hexword((unsigned int)&baseAddress32[offset]);
      put_string(" PATTERN 0x");
      put_hexword(pattern);
      put_string(" BAD\n");
      break;
    }
  }

  if (testpass) {
    screen_setcursor(25, 4);
    put_string(" PASS");
  } else {
    screen_setcursor(25, 4);
    put_string(" FAIL");
  }
  testpass = 1;

  screen_setcursor(4, 5);
  put_string("16 bit ops............");
  for (pattern16 = 1, offset = 0; offset < MEM_TEST_LENGTH / 2;
       pattern16++, offset++) {
    baseAddress16[offset] = pattern16;
  }

  for (pattern16 = 1, offset = 0; offset < MEM_TEST_LENGTH / 2;
       pattern16++, offset++) {
    if (baseAddress16[offset] != pattern16) {
      testpass = 0;
      put_string("          MEM 0x");
      put_hexword((unsigned int)&baseAddress16[offset]);
      put_string("=0x");
      put_hexword((unsigned int)baseAddress16[offset]);
      put_string(" PATTERN 0x");
      put_hexword(pattern16);
      put_string(" BAD\n");
      break;
    }
  }

  if (testpass) {
    screen_setcursor(25, 5);
    put_string(" PASS");
  } else {
    screen_setcursor(25, 5);
    put_string(" FAIL");
  }

  testpass = 1;

  screen_setcursor(4, 6);
  put_string("8 bit ops.............");
  for (pattern8 = 1, offset = 0; offset < MEM_TEST_LENGTH;
       pattern8++, offset++) {
    baseAddress8[offset] = pattern8;
  }

  for (pattern8 = 1, offset = 0; offset < MEM_TEST_LENGTH;
       pattern8++, offset++) {
    if (baseAddress8[offset] != pattern8) {
      testpass = 0;
      put_string("MEM 0x");
      put_hexword((unsigned int)&baseAddress8[offset]);
      put_string(" PATTERN 0x");
      put_hexword(pattern8);
      put_string(" BAD\n");
      break;
    }
  }

  if (testpass) {
    screen_setcursor(25, 6);
    put_string(" PASS");
  } else {
    screen_setcursor(25, 6);
    put_string(" FAIL");
  }
}

void InitAndCallSDBootElf() {

  typedef void (*entry_func_t)(void);

  unsigned int i = 0;
  put_string("\n\nInitializing SD...");

  int rowb = screen_row;
  int colb = screen_col;
  int failedonce = 0;
retryagain:
  while (STA_NOINIT == disk_initialize()) {
    if (failedonce == 0) {
      put_string(
          "\n  Failed to init. Please power cycle SD card unit to retry.\n");
      rowb = screen_row;
      colb = screen_col;
    }
    failedonce = 1;
    unsigned int i = 0xffffffff;
    while (i--) {
    }
    put_string(".");
  }

  screen_setcursor(colb, rowb);
  put_string("\n  Init Complete: ");
  if (CardType) {
    if (CardType & CT_SD1)
      put_string("SD");
    if (CardType & CT_SD2)
      put_string("SDHC");
    if (CardType & CT_BLOCK)
      put_string(", block addressing");

    if (RES_OK !=
        disk_readp((BYTE *)&gPartitions[0], 0, 0x1be, sizeof(gPartitions))) {
      put_string("\nRead of Partitions fail!\n");
      goto retryagain;
    }

    if (RES_OK != disk_readp((BYTE *)&gSignature[0], 0, 0x1fe, 2)) {
      put_string("\nRead of Signature fail!\n");
      goto retryagain;
    }

    put_string("\n\nBoot sector signature: 0x");
    put_hexbyte(gSignature[0]);
    put_hexbyte(gSignature[1]);
    put_string("\nPartition Table:");
    for (i = 0; i < 1; i++) {
      put_string("\n  ");
      put_hexbyte(i);
      put_string(") Type: 0x");
      put_hexbyte(gPartitions[i].partition_type);
      put_string(" Start Sector: 0x");
      put_hexword(gPartitions[i].start_sector);
      put_string(" Length: 0x");
      put_hexword(gPartitions[i].length_sectors);
    }
    if (gPartitions[0].start_sector != 0) {
      put_string("\nUsing partition 0...");
      disk_readp((void *)&gBootSector, gPartitions[0].start_sector, 0,
                 sizeof(gBootSector));
      put_string("\"  OEM: \"");
      put_stringn(&gBootSector.oem[0], 8);
      put_string("\"  TYPE: \"");
      put_stringn(&gBootSector.fs_type[0], 8);
      put_string("\"  Volume: \"");
      put_stringn(&gBootSector.volume_label[0], 11);
      put_string("\"\n");

      uint32_t cluster_begin_lba =
          gPartitions[0].start_sector + gBootSector.reserved_sectors +
          (gBootSector.number_of_fats * gBootSector.f32_num_sectors_1fat);
      uint32_t sectors_per_cluster = gBootSector.sectors_per_cluster;
      uint32_t root_dir_first_cluster = gBootSector.f32_cluster_for_root;

      uint32_t root_lba_addr =
          cluster_begin_lba +
          (root_dir_first_cluster - 2) * sectors_per_cluster;

      FileEntry_t files[16];
      
      disk_readp((void *)&files[0], root_lba_addr, 0, sizeof(files));

      put_string("\nRoot Files...");


      for (i = 0; i < 16; i++) {
        if (files[i].name[0] == '\0') {
          break;
        }
        if ((files[i].name[0] != 0xe5) && ((files[i].attrib & 0xf) != 0xf)) {
          put_string("\n ");

          uint32_t file_cluster = (((uint32_t)files[i].cluster_hi_h) << 24) |
                                  (((uint32_t)files[i].cluster_hi_l) << 16) |
                                  (((uint32_t)files[i].cluster_low_h) << 8) |
                                  ((uint32_t)files[i].cluster_low_l);

          put_hexword(file_cluster);
          put_string("  ");

          uint32_t file_lba_addr =
              cluster_begin_lba + (file_cluster - 2) * sectors_per_cluster;

          put_hexword(file_lba_addr);
          if ((files[i].attrib & 0x8) == 0x8) {
            put_string(" VOLUME: \"");
          } else if ((files[i].attrib & 0x10)) {
            put_string(" <dir>   \"");
          } else {
            put_string(" file    \"");
          }
          put_stringn(files[i].name, 8);
          if (files[i].name[9] != ' ') {
            put_string(".");
            put_stringn(&files[i].name[8], 3);
          }
          put_string("\" ");

          if (files[i].name[0] == 'B' && files[i].name[1] == 'O' &&
              files[i].name[2] == 'O' && files[i].name[3] == 'T') {
            put_string("\n");
            put_string("Reading BOOT into 0x");

            char *baseReadAddr = (char *)0x18000;
            put_hexword((unsigned int)baseReadAddr);
            put_string(".......");

            int numblocks = 32; // 16KB max boot elf size

            for (uint32_t block = 0; block < numblocks; block++) {
              disk_readp((void*)(baseReadAddr + (512u * block)), file_lba_addr + block, 0,
                         512u);
            }

            put_string("[OK]\n");

            put_string("Loading ELF from 0x");

            put_hexword((unsigned int)baseReadAddr);
            put_string(".......");

            kernel_elf_info info;
            load_kernel_elf(baseReadAddr, 512 * numblocks, &info);
            put_string("[OK]\n");

            put_string("\nBOOT init entry point: 0x");
            put_hexword(info.entry);

            entry_func_t boot_entry_point = 0;
            boot_entry_point = (entry_func_t)(info.entry);

            volatile unsigned int *ui_addr_leds = (unsigned int *)IO_ADDR_LEDS;
            int i = 0xfffff;
            while (i != 0) {
              *ui_addr_leds = (i--) >> 20;
            }
            put_string("\n\n 3...");
            i = 0xfffff;
            while (i != 0) {
              *ui_addr_leds = (i--) >> 20;
            }

            put_string("\n 2...");
            i = 0xfffff;
            while (i != 0) {
              *ui_addr_leds = (i--) >> 20;
            }

            put_string("\n 1...");

            i = 0xfffff;
            while (i != 0) {
              *ui_addr_leds = (i--) >> 20;
            }

            put_string("\n Transferring to SD BOOT...");
            (*boot_entry_point)();
            break;
          }
        }
      }
    }
  }

  // this function returning implied BOOT elf not found
}

void c_entry_point(void) {
  screen_clear(CREATE_PEN(0, COLOUR_BLUE, COLOUR_WHITE));
  screen_setpen(CREATE_PEN(0, COLOUR_BLUE, COLOUR_WHITE));
  screen_setcursor(0, 0);

  put_string("RPU Bootstrap firmware, version 0x");
  put_hexword(VERSION);
  put_string(".\n\n");

  BasicMemoryTest();
  InitAndCallSDBootElf();

  // should never get here.
  screen_setpen(CREATE_PEN(0, COLOUR_BLACK, COLOUR_RED));
  put_string("\n BOOT elf not found. \n");
}