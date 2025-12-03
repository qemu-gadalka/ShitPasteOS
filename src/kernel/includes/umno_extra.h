#include <stdint.h>
#include <stddef.h>
#include <stdarg.h>
#include <string.h>
#include <limits.h>

void UmnoLauncher(void);
void UmnoClicker(void);

extern void shell_main(void);
extern void print(const char* s);
extern void printf(const char* fmt, ...);
extern void cls(void);
extern int get_char_nonblocking(void);
extern int get_char_blocking(void);
extern int strcmp(const char* a, const char* b);
void UmnoTetris(void);
extern void kernel_panic(void);