#ifndef SHELL_H
#define SHELL_H

#include <stdint.h>
#include <stddef.h>
#include <stdarg.h>

#define CAPS_LOCK 0x3A
#define L_SHIFT 0x2A
#define R_SHIFT 0x36
#define L_WIN 0x5B
#define R_WIN 0x5C
#define L_SHIFT_RELEASE 0xAA
#define R_SHIFT_RELEASE 0xB6

extern int caps_lock;
extern int shift_pressed;
extern int mod_win;
extern int mod_win_recent;
extern const char scancode_to_ascii[128];
extern const char scancode_to_ascii_shift[128];

void calculator(void);
extern void UmnoLauncher(void);
extern void UmnoClicker(void);
extern uint8_t current_color; 
extern void print_clr(const char* s, uint8_t color);
extern void print(const char* s);
extern int get_char_blocking(void);
extern int get_char_nonblocking(void);
extern void reboot_hw(void);
extern void shutdown_hw(void);
extern void cls(void);
extern void print_int(int value);
extern void printf(const char* fmt, ...);
extern uint32_t total_ram;
void surfetch(void);
void setforeground(void);
void setbackground(void);
void wm_main(void);
extern void kernel_panic(void);
int strcmp(const char* a, const char* b);
static int strncmp(const char* s1, const char* s2, size_t n);
static char* strchr(const char* s, int c);
static int atoi(const char* str);
void UmnoLauncher(void);
void UmnoClicker(void);
extern void c_cls(void);
extern void kernel_panic(void);

extern int caps_lock;
extern int shift_pressed;
char to_lower(char c);
void to_lower_str(char *str);
char* strcpy(char* dest, const char* src);

#endif