BOOTCODE is a riscv-compliance test runner. It jumps to all .elf files in the /CTS folder, pulling out the test result signature, before comparing it to the reference results in the .ref files.

Place in root of SD card and it will run. Outputs to text console.

sd:/BOOTCODE
sd:/CTS/*many tests comprising .elf and .ref*
