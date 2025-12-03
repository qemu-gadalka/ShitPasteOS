#include "includes/gdt.h"

extern void gdt_flush(uint32_t gdt_ptr_addr);

static struct gdt_entry gdt[3];
static struct gdt_ptr gp;

static void gdt_set_gate(int num, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran) {
    gdt[num].base_low  = base & 0xFFFF;
    gdt[num].base_mid  = (base >> 16) & 0xFF;
    gdt[num].base_high = (base >> 24) & 0xFF;

    gdt[num].limit_low = limit & 0xFFFF;
    gdt[num].gran      = ((limit >> 16) & 0x0F);
    gdt[num].gran     |= gran & 0xF0;
    gdt[num].access    = access;
}

void gdt_init(void) {
    gp.limit = (sizeof(gdt) - 1);
    gp.base  = (uint32_t)&gdt;

    gdt_set_gate(0, 0, 0, 0, 0);                // null
    gdt_set_gate(1, 0, 0xFFFFF, 0x9A, 0xCF);    // kernel code
    gdt_set_gate(2, 0, 0xFFFFF, 0x92, 0xCF);    // kernel data

    gdt_flush((uint32_t)&gp);
}
