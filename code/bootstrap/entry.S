# Sets up a stack pointer to end of first 64KB block of BRAM.
# Jumps to a C entry point

.section .text

.globl _start
.globl c_entry_point

_start:

reset:
boot:
  # set up stack
  li   sp, 0x7ffc
  call c_entry_point

if:
  j if

