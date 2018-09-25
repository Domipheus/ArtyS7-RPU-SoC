call ../env.bat

riscv-none-embed-gcc.exe -march=rv32i -mabi=ilp32 -fpic -nostdlib -nostartfiles -O2 -Wall -fno-inline -I../common -Tbootstrap.ld -o bootstrap.bin entry.S muldi3.S bootstrap.c

riscv-none-embed-objdump.exe -D bootstrap.bin > bootstrap_objdump.txt
riscv-none-embed-objdump.exe -s bootstrap.bin > bootstrap_dump.txt