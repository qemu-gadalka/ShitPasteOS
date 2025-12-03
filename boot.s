/* Minimal multiboot header only - no runtime code here. */
.section .multiboot
.align 4
.long 0x1BADB002
.long 0x00000003
.long -(0x1BADB002 + 0x00000003)

/* End of header */

    