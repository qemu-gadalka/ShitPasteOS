#include "includes/kernel.h"
#include "includes/gdt.h"
#include "includes/shell.h"
/* Global variables referenced across the kernel and shell. These were
    declared as extern in headers (e.g. includes/1.h/includes/2.h) but
    not defined, causing linker errors. Define and initialize them here. */

int caps_lock = 0;
int shift_pressed = 0;
int mod_win = 0;
int mod_win_recent = 0;
int cursor_visible = 1;
int escape_sequence = 0;

/* Core globals that must be defined in a single translation unit. */
size_t vga_pos = 0;
uint8_t current_color = VGA_COLOR;
uint32_t total_ram = 0;

unsigned char   inb(unsigned short port)
{
    unsigned char result;
        __asm__("in %%dx, %%al" : "=a" (result) : "d" (port));
    return (result);
}


void    outb(unsigned short port, unsigned char data)
{
        __asm__("out %%al, %%dx" : : "a" (data), "d" (port));
}


unsigned char   inw(unsigned short port)
{
    unsigned short result;
    __asm__("in %%dx, %%ax" : "=a" (result) : "d" (port));
    return (result);
}


void outw(unsigned short port, unsigned short data)
{
    __asm__("out %%ax, %%dx" : : "a" (data), "d" (port));
}

void insw(uint16_t port, void *addr, unsigned long count)
{
    asm volatile ("cld; rep insw"
                  : "+D" (addr), "+c" (count)
                  : "d" (port)
                  : "memory");
}

void outsw(uint16_t port, const void *addr, unsigned long count)
{
    asm volatile ("cld; rep outsw"
                  : "+S" (addr), "+c" (count)
                  : "d" (port)
                  : "memory");
}

void insb(uint16_t port, void *addr, unsigned long count) {
    asm volatile (
        "cld; rep insb"
        : "+D" (addr), "+c" (count)
        : "d" (port)
        : "memory"
    );
}

void update_cursor(void) {
    if (!cursor_visible) return;
    uint16_t cursor_pos = vga_pos;
    
    outb(VGA_CTRL, 0x0F);
    outb(VGA_DATA, (uint8_t)(cursor_pos & 0xFF));
    outb(VGA_CTRL, 0x0E);
    outb(VGA_DATA, (uint8_t)((cursor_pos >> 8) & 0xFF));
}

void set_cursor_visible(int visible) {
    cursor_visible = visible ? 1 : 0;
    if (cursor_visible) update_cursor();
    else {
        /* hide cursor by writing an impossible position (disable) */
        outb(VGA_CTRL, 0x0F);
        outb(VGA_DATA, (uint8_t)0xFF);
        outb(VGA_CTRL, 0x0E);
        outb(VGA_DATA, (uint8_t)0xFF);
    }
}

void set_color(uint8_t color) {
    current_color = color;
}

void putchar(char c) {
    if (c == '\b') {
        if (vga_pos > 0) {
            vga_pos--;
            VGA_BUF[vga_pos] = (uint16_t)((current_color << 8) | ' ');
        }
    } else if (c == '\n') {
        vga_pos += 80 - (vga_pos % 80);
        if (vga_pos >= 80*25) {
            for (int i = 0; i < 80*24; i++) {
                VGA_BUF[i] = VGA_BUF[i + 80];
            }
            for (int i = 80*24; i < 80*25; i++) {
                VGA_BUF[i] = (uint16_t)((current_color << 8) | ' ');
            }
            vga_pos = 80*24;
        }
    } else {
        if (vga_pos >= 80*25) {
            for (int i = 0; i < 80*24; i++) {
                VGA_BUF[i] = VGA_BUF[i + 80];
            }
            for (int i = 80*24; i < 80*25; i++) {
                VGA_BUF[i] = (uint16_t)((current_color << 8) | ' ');
            }
            vga_pos = 80*24;
        }
        VGA_BUF[vga_pos++] = (uint16_t)((current_color << 8) | c);
    }
    update_cursor();
}

void print_int(int value) {
    char buf[12];
    int pos = 0;
    
    if (value == 0) { 
        putchar('0'); 
        return; 
    }
    
    if (value < 0) {
        putchar('-');
        value = -value;
    }
    
    while (value > 0) {
        buf[pos++] = '0' + (value % 10);
        value /= 10;
    }
    
    for (int i = pos-1; i >= 0; i--) {
        putchar(buf[i]);
    }
}

void print(const char* s) {
    while (*s) {
        putchar(*s++);
    }
}

void printf(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);

    for (const char* p = fmt; *p; p++) {
        if (*p == '%') {
            p++;
            if (*p == 'd') {
                int n = va_arg(args, int);
                print_int(n);
            } else if (*p == 's') {
                char* str = va_arg(args, char*);
                print(str);
            } else if (*p == 'c') {
                char c = (char)va_arg(args, int);
                putchar(c);
            } else {
                putchar('%');
                putchar(*p);
            }
        } else {
            putchar(*p);
        }
    }

    va_end(args);
}

void cls(void) {
    uint16_t clear = (VGA_COLOR << 8) | ' ';
    for (int i = 0; i < 80*25; ++i) VGA_BUF[i] = clear;
    vga_pos = 0;
    current_color = VGA_COLOR;
    update_cursor();
}

void print_clr(const char* s, uint8_t color) {
    uint8_t old_color = current_color;
    set_color(color);
    print(s);
    set_color(old_color);
}

const char scancode_to_ascii[128] = {
    0,  27, '1','2','3','4','5','6','7','8','9','0','-','=','\b',
    '\t','q','w','e','r','t','y','u','i','o','p','[',']','\n', 0,
    'a','s','d','f','g','h','j','k','l',';','\'','`', 0,'\\','z','x',
    'c','v','b','n','m',',','.','/', 0,'*', 0,' ', 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

const char scancode_to_ascii_shift[128] = {
    0,  27, '!','@','#','$','%','^','&','*','(',')','_','+','\b',
    '\t','Q','W','E','R','T','Y','U','I','O','P','{','}','\n', 0,
    'A','S','D','F','G','H','J','K','L',':','"','~', 0,'|','Z','X',
    'C','V','B','N','M','<','>','?', 0,'*', 0,' ', 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

int get_scancode(void) {
    if (inb(PS2_STATUS) & 0x01) {
        return inb(PS2_DATA);
    }
    return 0;
}

int get_char_nonblocking(void) {
    int sc = get_scancode();
    if (!sc) return 0;
    
    if (escape_sequence) {
        escape_sequence = 0;
        switch (sc) {
            case 0x4B: return -1;
            case 0x4D: return -2;
            case 0x48: return -3;
            case 0x50: return -4;
            default: return 0;
        }
    }
    
    if (sc == 0xE0) {
        escape_sequence = 1;
        return 0;
    }
    
    if (sc == CAPS_LOCK) {
        caps_lock = !caps_lock;
        return 0;
    }
    
    if (sc == L_SHIFT || sc == R_SHIFT) {
        shift_pressed = 1;
        return 0;
    }
    
    if (sc == L_SHIFT_RELEASE || sc == R_SHIFT_RELEASE) {
        shift_pressed = 0;
        return 0;
    }

    /* Handle Windows (GUI) key: make press set mod_win, release clear it. */
    if (sc == L_WIN || sc == R_WIN) {
        mod_win = 1;
        mod_win_recent = 1; /* mark that win was pressed recently */
        return 0;
    }
    if (sc == (L_WIN | 0x80) || sc == (R_WIN | 0x80)) {
        mod_win = 0;
        return 0;
    }
    
    if (sc & 0x80) return 0;
    
    if (sc < 128) {
        char c;
        int use_shift = shift_pressed ^ caps_lock;
        
        if (use_shift) {
            c = scancode_to_ascii_shift[sc];
        } else {
            c = scancode_to_ascii[sc];
        }
        
        if (c) return (int)c;
    }
    return 0;
}

int get_char_blocking(void) {
    int c;
    do {
        c = get_char_nonblocking();
    } while (!c);
    return c;
}

void reboot_hw(void) {
    outb(0xCF9, 0x06);
    for (;;) asm volatile("hlt");
}

void shutdown_hw(void) {
    outw(0xB004, 0x2000);

    outw(0x604, 0x2000);

    outw(0x4004, 0x3400);

    while(1) {
        asm volatile ("hlt");
    }
}

void c_cls(void) {
    print("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");
}

void detect_memory(unsigned int magic, unsigned int addr) {
    
    if (magic != 0x2BADB002) {
        print_clr("ERROR: Invalid multiboot magic\n", 0x04);
        printf("Expected: 0x2BADB002, Got: 0x%x\n", magic);
        return;
    }

    struct multiboot_info* mbi = (struct multiboot_info*)addr;
    if (mbi->flags & 0x01) {
        print_clr("[  OK  ] ", 0x02);
        print("RAM Successfully received!\n");
        print(":: Converting KB to MB...\n");
        total_ram = (mbi->mem_lower + mbi->mem_upper) / 1015;
        print_clr("[  OK  ] ", 0x02);
        print("Succsesfull converting KB to MB!\n");
        printf(":: Lower memory: %d KB\n", mbi->mem_lower);
        printf(":: Upper memory: %d KB\n", mbi->mem_upper);
        printf(":: Total RAM: %d MB\n", total_ram);
    } else {
        print_clr("ERROR: No memory information from multiboot\n", 0x04);
    }
}

void _start(void) {
    unsigned int magic, addr;
    
    asm volatile (
        "mov %%eax, %0\n"
        "mov %%ebx, %1"
        : "=r"(magic), "=r"(addr)
    );
    
    kernel_main(magic, addr);
}

void kernel_main(unsigned int magic, unsigned int addr) {
    cls();
    print(":: SurteoKernel Booted Successfully!\n");
    
    print(":: Initing GDT...\n");
    gdt_init();
    heap_init(0x100000);

    char* test = kmalloc(64);
    print_clr("[  OK  ] ", 0x02);
    print("GDT Inited!\n");

    print(":: Detecting memory...\n");
    detect_memory(magic, addr);
    
    print(":: Initializing shell...\n");
    extern void shell_main(void);
    print_clr("[  OK  ] ", 0x02);
    print("Shell initialized!\n");

    print("\n:: Starting shell...\n");
    shell_main();

    for (;;) asm volatile("hlt");
}
