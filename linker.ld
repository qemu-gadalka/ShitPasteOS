ENTRY(_start)

SECTIONS
{
    /* Multiboot header must appear within the first 8KB of the file so
       GRUB can find it. Place it at 0x100000 then put the header inside
       the start of the .text segment so it gets emitted early in the
       file (inside the first loadable segment). */
    . = 0x100000;

    .text : ALIGN(4) {
        /* Put the multiboot header at the beginning of .text so it
           lands early in the ELF image file (within the first 8KB). */
        *(.multiboot)
        *(.text)
    }

    .rodata : {
        *(.rodata)
    }

    .data : {
        *(.data)
    }

    .bss : {
        *(.bss)
    }

    /* Куча начинается после BSS */
    .heap : {
        . = ALIGN(4096);      /* Выравнивание по 4КБ */
        _heap_start = .;
        . = . + 0x400000;     /* 4 МБ для кучи */
        _heap_end = .;
    }
}
