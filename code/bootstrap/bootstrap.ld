 OUTPUT_ARCH( "riscv" )
 ENTRY( _start )
 SECTIONS
 {
 /* text: test code section */
 . = 0x0000; 
 .text :
 {
 *(.text)
 }
 /* data: Initialized data segment */
 .data :
 {
 *(.data)
 }
 /* End of uninitalized data segement */
 _end = .;
 }
 
