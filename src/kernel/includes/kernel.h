#ifndef KERNEL_H
#define KERNEL_H

#include <stdint.h>
#include <stddef.h>
#include <stdarg.h>

#define VGA_BUF ((volatile uint16_t*)0xB8000)
#define VGA_COLOR 0x07
#define PS2_STATUS 0x64
#define PS2_DATA   0x60
#define VGA_CTRL 0x3D4
#define VGA_DATA 0x3D5

/* -------- Structs -------- */
struct vbe_info {
    uint16_t mode;
    uint16_t width;
    uint16_t height;
    uint16_t bpp;
    uint32_t framebuffer;
};

struct vbe_mode_info {
    uint16_t attributes;
    uint8_t  window_a;
    uint8_t  window_b;
    uint16_t granularity;
    uint16_t window_size;
    uint16_t segment_a;
    uint16_t segment_b;
    uint32_t win_func_ptr;
    uint16_t pitch;
    uint16_t width;
    uint16_t height;
    uint8_t  w_char;
    uint8_t  y_char;
    uint8_t  planes;
    uint8_t  bpp;
    uint8_t  banks;
    uint8_t  memory_model;
    uint8_t  bank_size;
    uint8_t  image_pages;
    uint8_t  reserved0;
    uint8_t  red_mask;
    uint8_t  red_position;
    uint8_t  green_mask;
    uint8_t  green_position;
    uint8_t  blue_mask;
    uint8_t  blue_position;
    uint8_t  reserved_mask;
    uint8_t  reserved_position;
    uint8_t  direct_color_attributes;
    uint32_t framebuffer;
    uint32_t off_screen_mem_off;
    uint16_t off_screen_mem_size;
    uint8_t  reserved1[206];
} __attribute__((packed));

struct multiboot_info {
    uint32_t flags;
    uint32_t mem_lower;
    uint32_t mem_upper;
    uint32_t boot_device;
    uint32_t cmdline;
    uint32_t mods_count;
    uint32_t mods_addr;

    /* VBE info */
    uint16_t vbe_mode;
    uint32_t vbe_mode_info;
};

/* -------- Functions -------- */
void print_clr(const char* s, uint8_t color);
void print(const char* s);
void cls(void);
int get_char_nonblocking(void);
int get_char_blocking(void);
void reboot_hw(void);
void shutdown_hw(void);
void update_cursor(void);
void set_cursor_visible(int visible);
void set_color(uint8_t color);
void printf(const char* fmt, ...);
void print_int(int value);
void putchar(char c);
void shell_main(void);
void detect_memory(unsigned int magic, unsigned int addr);
void kernel_main(unsigned int magic, unsigned int addr);
void c_cls(void);
void detect_vbe(struct multiboot_info* mbi);
void heap_init(uint32_t start);
void* kmalloc(uint32_t size);
extern void gdt_init(void);

/* -------- Global vars (definitions go in one C file only) -------- */
extern size_t vga_pos;

extern const char scancode_to_ascii[128];
extern const char scancode_to_ascii_shift[128];

#endif
