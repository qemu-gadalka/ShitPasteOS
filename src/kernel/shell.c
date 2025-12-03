#include "includes/shell.h"

char to_lower(char c) {
    if (c >= 'A' && c <= 'Z') {
        return c + ('a' - 'A');
    }
    return c;
}

void to_lower_str(char *str) {
    for (int i = 0; str[i]; i++) {
        str[i] = to_lower(str[i]);
    }
}

void setbackground(void) {
    char buf[64];
    int color_code;
    
    while (1) {
        print("setbackground> ");
        size_t pos = 0;

        while (1) {
            int c = get_char_blocking();
            
            if (c > 0) {
                if (c == '\r' || c == '\n') {
                    print("\n");
                    break;
                } else if (c == '\b' || c == 127) {
                    if (pos > 0) {
                        pos--;
                        print("\b \b");
                    }
                } else {
                    if (pos < sizeof(buf)-1) {
                        buf[pos++] = (char)c;
                        char s[2] = { (char)c, 0 };
                        print(s);
                    }
                }
            }
        }

        buf[pos] = 0;
        
        if (pos == 0) {
            /* Empty prompt: continue waiting for input. WM is started
               explicitly via the `wm` command to avoid accidental
               re-entry loops when modifiers are noisy. */
            continue;
        }

        char lower_buf[64];
        strcpy(lower_buf, buf);
        to_lower_str(lower_buf);

        if (strcmp(lower_buf, "help") == 0) {
            print("setbackground help: \n");
            print("    0 -- Black\n");
            print("    1 -- Blue\n");
            print("    2 -- Green\n");
            print("    3 -- Cyan\n");
            print("    4 -- Red\n");
            print("    5 -- Magenta\n");
            print("    6 -- Brown (Or Orange)\n");
            print("    7 -- Light Gray\n");
            print("    8 -- Dark Gray\n");
            print("    9 -- Light Blue\n");
            print("    10 -- Light Green\n");
            print("    11 -- Light Cyan\n");
            print("    12 -- Light Red\n");
            print("    13 -- Light Magenta\n");
            print("    14 -- Yellow\n");
            print("    15 -- White\n");
        }
        else if (strcmp(lower_buf, "exit") == 0) {
            break;
        }
        else {
            color_code = atoi(buf);
            if (color_code >= 0 && color_code <= 15) {
                current_color = (current_color & 0x0F) | ((uint8_t)color_code << 4);
                printf("Background color set to %d\n", color_code);
            } else {
                print("Invalid color code. Use 0-15 or 'help' for options.\n");
            }
        }
    }
}

void setforeground(void) {
    char buf[64];
    int color_code;
    
    while (1) {
        print("setforeground> ");
        size_t pos = 0;

        while (1) {
            int c = get_char_blocking();
            
            if (c > 0) {
                if (c == '\r' || c == '\n') {
                    print("\n");
                    break;
                } else if (c == '\b' || c == 127) {
                    if (pos > 0) {
                        pos--;
                        print("\b \b");
                    }
                } else {
                    if (pos < sizeof(buf)-1) {
                        buf[pos++] = (char)c;
                        char s[2] = { (char)c, 0 };
                        print(s);
                    }
                }
            }
        }

        buf[pos] = 0;
        
        if (pos == 0) {
            continue;
        }

        char lower_buf[64];
        strcpy(lower_buf, buf);
        to_lower_str(lower_buf);

        if (strcmp(lower_buf, "help") == 0) {
            print("setforeground help: \n");
            print("    0 -- Black\n");
            print("    1 -- Blue\n");
            print("    2 -- Green\n");
            print("    3 -- Cyan\n");
            print("    4 -- Red\n");
            print("    5 -- Magenta\n");
            print("    6 -- Brown (Or Orange)\n");
            print("    7 -- Light Gray\n");
            print("    8 -- Dark Gray\n");
            print("    9 -- Light Blue\n");
            print("    10 -- Light Green\n");
            print("    11 -- Light Cyan\n");
            print("    12 -- Light Red\n");
            print("    13 -- Light Magenta\n");
            print("    14 -- Yellow\n");
            print("    15 -- White\n");
        }
        else if (strcmp(lower_buf, "exit") == 0) {
            break;
        }
        else {
            color_code = atoi(buf);
            if (color_code >= 0 && color_code <= 15) {
                current_color = (current_color & 0xF0) | (uint8_t)color_code;
                printf("Foreground color set to %d\n", color_code);
            } else {
                print("Invalid color code. Use 0-15 or 'help' for options.\n");
            }
        }
    }
}

void surfetch(void) {
    print("|\\---/|    Kernel: 0.7, OS: 0.8\n");
    printf("| o_o |    TOTAL RAM: %d MB\n", total_ram);
    print(" \\_^_/     ");
    print_clr("V", 0x02);
    print_clr("G", 0x03);
    print_clr("A", 0x04);
    print_clr("_", 0x05);
    print_clr("C", 0x06);
    print_clr("O", 0x07);
    print_clr("L", 0x08);
    print_clr("O", 0x09);
    print_clr("R", 0x0A);
    print_clr(":", 0x07);
    print(" ");
    printf("%d", current_color & 0x0F);
    
    if (caps_lock) {
        print_clr(" [CAPS]", 0x0E);
    }
    print("\n\n");
}

void calculator(void) {
    char input[100];
    int num1, num2, result;
    char op;

    print("Thanks Umno for Calculator!\n\n");
    print(" /\\_/\\  \n");
    print("( o.o )  [123]\n");
    print(" > ^ <  /|\\\n\n");
    print("Type 'help' for instructions, 'exit' to quit.\n\n");
    
    if (caps_lock) {
        print_clr("Caps Lock is ON\n", 0x0E);
    }

    while (1) {
        print("[xpression]$ ");
        
        size_t pos = 0;
        while (1) {
            int c = get_char_blocking();
            
            if (c > 0) {
                if (c == '\r' || c == '\n') {
                    print("\n");
                    break;
                } else if (c == '\b' || c == 127) {
                    if (pos > 0) {
                        pos--;
                        print("\b \b");
                    }
                } else if (c == 27) { // ESC
                    print("\nExiting...\n");
                    return;
                } else {
                    if (pos < sizeof(input)-1) {
                        input[pos++] = (char)c;
                        char s[2] = { (char)c, 0 };
                        print(s);
                    }
                }
            }
        }

        input[pos] = 0;
        
        if (pos == 0) {
            continue;
        }

        char lower_input[100];
        strcpy(lower_input, input);
        to_lower_str(lower_input);

        if (strcmp(lower_input, "help") == 0) {
            print("Instructions:\n");
            print("- Enter expressions like: 2 + 3, 4 * 5, etc.\n");
            print("- Supported operators: +, -, *, /\n");
            print("- Press ESC or type 'exit' to quit.\n");
            continue;
        } else if (strcmp(lower_input, "exit") == 0) {
            print("Exiting calculator...\n");
            break;
        }

        char *token = input;
        char *num1_str = token;
        
        while (*token && *token != ' ') token++;
        if (*token == ' ') {
            *token = '\0';
            token++;
        }
        
        op = *token;
        token++;
        
        while (*token && *token == ' ') token++;
        
        char *num2_str = token;
        
        num1 = atoi(num1_str);
        num2 = atoi(num2_str);

        if (num1_str[0] == '\0' || num2_str[0] == '\0') {
            print("Invalid input! Use format: number operator number\n");
            continue;
        }

        switch(op) {
            case '+': 
                result = num1 + num2; 
                break;
            case '-': 
                result = num1 - num2; 
                break;
            case '*': 
                result = num1 * num2; 
                break;
            case '/':
                if(num2 == 0) {
                    print("Error: division by zero!\n");
                    continue;
                }
                result = num1 / num2;
                break;
            default:
                print("Invalid operator! Use +, -, *, or /\n");
                continue;
        }

        printf("%d %c %d = %d\n", num1, op, num2, result);
    }
}

void shell_main(void) {
    print_clr("\nWelcome to Surteo!\n\n", 0x03);

    char buf[64];
    
    while (1) {
        print("[Surteo@OS]$ ");
        size_t pos = 0;

        while (1) {
            int c = get_char_blocking();
            
            if (c > 0) {
                if (c == '\r' || c == '\n') {
                    print("\n");
                    break;
                } else if (c == '\b' || c == 127) {
                    if (pos > 0) {
                        pos--;
                        print("\b \b");
                    }
                } else {
                    if (pos < sizeof(buf)-1) {
                        buf[pos++] = (char)c;
                        char s[2] = { (char)c, 0 };
                        print(s);
                    }
                }
            }
        }

        buf[pos] = 0;
        
        if (pos == 0) {
            continue;
        }

        char lower_buf[64];
        strcpy(lower_buf, buf);
        to_lower_str(lower_buf);

        if (strcmp(lower_buf, "help") == 0) {
            print("Help: \n    clear - Clears the screen\n    reboot - Reboots your PC\n    shutdown - Shutdowns your PC\n    surfetch - Info about your PC\n    setforeground - Change font color\n    setbackground - Change background color\n    calc - Calculator by Umno\n    umno - Umno projects writed for SurteoOS!\n\n");
        } 

        else if (strcmp(lower_buf, "clear") == 0) {
            cls();
        } 

        else if (strcmp(lower_buf, "reboot") == 0) {
            print("Rebooting...\n");
            reboot_hw();
        } 

        else if (strcmp(lower_buf, "shutdown") == 0) {
            print("Shutting down...\n");
            shutdown_hw();
        } 

        else if (strcmp(lower_buf, "calc") == 0) {
            calculator();
        }

        else if (strcmp(lower_buf, "surfetch") == 0) {
            surfetch();
        } 

        else if (strcmp(lower_buf, "setforeground") == 0) {
            setforeground();
        } 

        else if (strcmp(lower_buf, "setbackground") == 0) {
            setbackground();
        } 
        
        else if(strcmp(buf, "\t") == 0) {
            kernel_panic();
        }

        else if(strcmp(lower_buf, "umno") == 0) {
            UmnoLauncher();
        }

        else {
            print("Unknown command: ");
            print(buf);
            print("\n");
        }
    }
}

char* strcpy(char* dest, const char* src) {
    char* ptr = dest;
    while (*src) {
        *dest++ = *src++;
    }
    *dest = 0;
    return ptr;
}

int strcmp(const char* a, const char* b) {
    while (*a && (*a == *b)) { a++; b++; }
    return (*(unsigned char*)a) - (*(unsigned char*)b);
}

static int strncmp(const char* s1, const char* s2, size_t n) {
    while (n && *s1 && (*s1 == *s2)) {
        s1++;
        s2++;
        n--;
    }
    if (n == 0) return 0;
    return (*(unsigned char*)s1 - *(unsigned char*)s2);
}

static char* strchr(const char* s, int c) {
    while (*s) {
        if (*s == c) return (char*)s;
        s++;
    }
    return 0;
}

static int atoi(const char* str) {
    int result = 0;
    while (*str >= '0' && *str <= '9') {
        result = result * 10 + (*str - '0');
        str++;
    }
    return result;
}