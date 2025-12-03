/* VGA test: draw a circle and display "test wm lol" at the top.
   This file replaces the previous WM and provides a simple visual
   confirmation test invoked by the `wm` shell command. Press ESC or
   Enter to exit back to the shell. */

#include "../includes/kernel.h"
#include "../includes/shell.h"

/* Draw a single "pixel" (a full block glyph) at text-cell coords */
static void vga_dot(int x, int y, uint8_t color) {
    /* Use a smaller, lighter glyph for a "dot" so the circles look finer */
    const unsigned char dot = 0xDB; /* full block for touching dots */
    if (x < 0 || x >= 80 || y < 0 || y >= 25) return;
    VGA_BUF[y*80 + x] = (uint16_t)((color << 8) | dot);
}

/* Small 5x7 bitmap font for a limited set of characters used here.
   Font stored as 5 columns of 7 bits (LSB top row). */
typedef struct { const unsigned char cols[5]; } font5x7;

/* Define an expanded 5x7 bitmap font for ASCII (A-Z, digits, punctuation)
   and a set of placeholder glyphs that will be used for Cyrillic letters.
   The font is stored as 5 columns (LSB = top row) per glyph. */
static const font5x7 FONT[] = {
    /* --- Latin uppercase A-Z (indexes 0..25) --- */
    /* A */ { {0x7E,0x11,0x11,0x11,0x7E} },
    /* B */ { {0x7F,0x49,0x49,0x49,0x36} },
    /* C */ { {0x3E,0x41,0x41,0x41,0x22} },
    /* D */ { {0x7F,0x41,0x41,0x22,0x1C} },
    /* E */ { {0x7F,0x49,0x49,0x49,0x41} },
    /* F */ { {0x7F,0x09,0x09,0x09,0x01} },
    /* G */ { {0x3E,0x41,0x49,0x49,0x7A} },
    /* H */ { {0x7F,0x08,0x08,0x08,0x7F} },
    /* I */ { {0x00,0x41,0x7F,0x41,0x00} },
    /* J */ { {0x20,0x40,0x41,0x3F,0x01} },
    /* K */ { {0x7F,0x08,0x14,0x22,0x41} },
    /* L */ { {0x7F,0x40,0x40,0x40,0x40} },
    /* M */ { {0x7F,0x02,0x04,0x02,0x7F} },
    /* N */ { {0x7F,0x04,0x08,0x10,0x7F} },
    /* O */ { {0x3E,0x41,0x41,0x41,0x3E} },
    /* P */ { {0x7F,0x09,0x09,0x09,0x06} },
    /* Q */ { {0x3E,0x41,0x51,0x21,0x5E} },
    /* R */ { {0x7F,0x09,0x19,0x29,0x46} },
    /* S */ { {0x46,0x49,0x49,0x49,0x31} },
    /* T */ { {0x01,0x01,0x7F,0x01,0x01} },
    /* U */ { {0x3F,0x40,0x40,0x40,0x3F} },
    /* V */ { {0x1F,0x20,0x40,0x20,0x1F} },
    /* W */ { {0x7F,0x20,0x10,0x20,0x7F} },
    /* X */ { {0x63,0x14,0x08,0x14,0x63} },
    /* Y */ { {0x07,0x08,0x70,0x08,0x07} },
    /* Z */ { {0x61,0x51,0x49,0x45,0x43} },

    /* space */ { {0x00,0x00,0x00,0x00,0x00} },

    /* digits 0..9 */
    /* 0 */ { {0x3E,0x45,0x49,0x51,0x3E} },
    /* 1 */ { {0x00,0x42,0x7F,0x40,0x00} },
    /* 2 */ { {0x21,0x43,0x45,0x49,0x31} },
    /* 3 */ { {0x42,0x41,0x51,0x69,0x46} },
    /* 4 */ { {0x18,0x14,0x12,0x7F,0x10} },
    /* 5 */ { {0x4F,0x49,0x49,0x49,0x31} },
    /* 6 */ { {0x3E,0x49,0x49,0x49,0x30} },
    /* 7 */ { {0x01,0x71,0x09,0x05,0x03} },
    /* 8 */ { {0x36,0x49,0x49,0x49,0x36} },
    /* 9 */ { {0x06,0x49,0x49,0x29,0x1E} },

    /* punctuation (colon, vertical bar, exclamation, dollar, hash, dot, comma, question) */
    /* ':' */ { {0x00,0x24,0x00,0x24,0x00} },
    /* '|' */ { {0x00,0x00,0x7E,0x00,0x00} },
    /* '!' */ { {0x00,0x00,0x5F,0x00,0x00} },
    /* '$' */ { {0x24,0x2A,0x7F,0x2A,0x12} },
    /* '#' */ { {0x14,0x7F,0x14,0x7F,0x14} },
    /* '.' */ { {0x00,0x00,0x60,0x60,0x00} },
    /* ',' */ { {0x00,0x80,0x60,0x60,0x00} },
    /* '?' */ { {0x02,0x01,0x59,0x09,0x06} },

    /* --- Cyrillic uppercase placeholders (U+0410..U+042F) ---
       We'll include simplified glyphs — many reuse Latin equivalents
       where shapes are visually similar. */
    /* U+0410 CYRILLIC CAPITAL LETTER A (А) maps to Latin A */
    { {0x7E,0x11,0x11,0x11,0x7E} }, /* А */
    /* Б */ { {0x3E,0x49,0x49,0x49,0x30} },
    /* В */ { {0x7F,0x49,0x49,0x49,0x36} },
    /* Г */ { {0x7F,0x01,0x01,0x01,0x01} },
    /* Д */ { {0x7E,0x11,0x11,0x11,0xFF} },
    /* Е */ { {0x7F,0x49,0x49,0x49,0x41} },
    /* Ж */ { {0x49,0x36,0x1C,0x36,0x49} },
    /* З */ { {0x22,0x41,0x49,0x49,0x36} },
    /* И */ { {0x7F,0x02,0x04,0x08,0x7F} },
    /* Й */ { {0x5E,0x01,0x02,0x04,0x7E} },
    /* К */ { {0x7F,0x08,0x14,0x22,0x41} },
    /* Л */ { {0x7E,0x11,0x11,0x11,0x7E} },
    /* М */ { {0x7F,0x02,0x04,0x02,0x7F} },
    /* Н */ { {0x7F,0x08,0x08,0x08,0x7F} },
    /* О */ { {0x3E,0x41,0x41,0x41,0x3E} },
    /* П */ { {0x7E,0x41,0x41,0x41,0x7E} },
    /* Р */ { {0x7F,0x09,0x09,0x09,0x06} },
    /* С */ { {0x3E,0x41,0x41,0x41,0x22} },
    /* Т */ { {0x01,0x01,0x7F,0x01,0x01} },
    /* У */ { {0x06,0x08,0x70,0x08,0x06} },
    /* Ф */ { {0x1C,0x3E,0x7F,0x3E,0x1C} },
    /* Х */ { {0x63,0x14,0x08,0x14,0x63} },
    /* Ц */ { {0x3E,0x41,0x41,0x41,0xFE} },
    /* Ч */ { {0x3E,0x41,0x41,0x41,0x3C} },
    /* Ш */ { {0x7F,0x40,0x40,0x40,0x7F} },
    /* Щ */ { {0x7F,0x40,0x40,0x40,0xFF} },
    /* Ъ */ { {0x7F,0x49,0x49,0x49,0x41} },
    /* Ы */ { {0x7F,0x40,0x4A,0x49,0x36} },
    /* Ь */ { {0x7F,0x40,0x40,0x40,0x20} },
    /* Э */ { {0x22,0x49,0x49,0x49,0x3E} },
    /* Ю */ { {0x7E,0x09,0x7E,0x49,0x7E} },
    /* Я */ { {0x7E,0x11,0x31,0x11,0x7E} }
};

/* Map ASCII letters to index in FONT. We only handle the letters we need
   (t,e,s,w,m,l,o) by mapping them to uppercase indices above; otherwise
   use space index 8.
*/
/* Convert a Unicode codepoint (ASCII or Cyrillic) to an index into FONT.
   If a lowercase ASCII or Cyrillic is provided it will be mapped to the
   uppercase glyph index. Unknown codepoints map to space. */
static int font_index(uint32_t cp) {
    /* ASCII letters */
    if (cp >= 'A' && cp <= 'Z') return (int)(cp - 'A');
    if (cp >= 'a' && cp <= 'z') return (int)(cp - 'a');

    /* space */
    if (cp == ' ') return 26;

    /* digits */
    if (cp >= '0' && cp <= '9') return 27 + (cp - '0');

    /* punctuation mapping */
    if (cp == ':') return 37;
    if (cp == '|') return 38;
    if (cp == '!') return 39;
    if (cp == '$') return 40;
    if (cp == '#') return 41;
    if (cp == '.') return 42;
    if (cp == ',') return 43;
    if (cp == '?') return 44;

    /* Cyrillic uppercase U+0410..U+042F map into the block starting at index 45 */
    if (cp >= 0x0410 && cp <= 0x042F) return 45 + (cp - 0x0410);
    /* Cyrillic lowercase U+0430..U+044F -> map to uppercase equivalents */
    if (cp >= 0x0430 && cp <= 0x044F) return 45 + (cp - 0x0430);

    /* Fallback to space */
    return 26; /* space and unknown */
}

/* Draw a character using dot pixels. x,y specify top-left of character
   in text-cell coordinates. scale increases the character pixel size. */
static void vga_dotted_char(int x, int y, uint32_t ch, uint8_t color, int scale) {
    int idx = font_index(ch);
    const font5x7 *f = &FONT[idx];
    for (int col = 0; col < 5; ++col) {
        unsigned char bits = f->cols[col];
        for (int row = 0; row < 7; ++row) {
            if (bits & (1 << row)) {
                int px = x + col * scale;
                int py = y + row * scale;
                /* draw scaled 1x1 or NxN dot block */
                for (int sx = 0; sx < scale; ++sx)
                    for (int sy = 0; sy < scale; ++sy)
                        vga_dot(px + sx, py + sy, color);
            }
        }
    }
}

static void vga_dotted_text(int x, int y, const char *s, uint8_t color, int scale) {
    int px = x;
    for (int i = 0; s[i]; ) {
        unsigned char b = (unsigned char)s[i];
        uint32_t cp = 0;
        int consumed = 1;
        if ((b & 0x80) == 0) {
            /* ASCII */
            cp = b;
            consumed = 1;
        } else if ((b & 0xE0) == 0xC0) {
            /* 2-byte UTF-8 */
            unsigned char b2 = (unsigned char)s[i+1];
            cp = ((b & 0x1F) << 6) | (b2 & 0x3F);
            consumed = 2;
        } else if ((b & 0xF0) == 0xE0) {
            /* 3-byte UTF-8 */
            unsigned char b2 = (unsigned char)s[i+1];
            unsigned char b3 = (unsigned char)s[i+2];
            cp = ((b & 0x0F) << 12) | ((b2 & 0x3F) << 6) | (b3 & 0x3F);
            consumed = 3;
        } else {
            /* unknown/unsupported sequences - treat as space */
            cp = ' ';
            consumed = 1;
        }

        vga_dotted_char(px, y, cp, color, scale);
        px += (6 * scale);
        i += consumed;
    }
}

/* Draw a circle outline using an approximation algorithm. */
/* Draw a filled circle using the dot grid so the dots touch evenly. */
/* Set a "subpixel" dot at double vertical resolution.
   y_sub is in half-row units (0..49). This lets us use upper/lower
   half-block characters to render smaller-looking dots while keeping
   a dense filled appearance. */
static void vga_subdot(int x, int y_sub, uint8_t color) {
    if (x < 0 || x >= 80) return;
    if (y_sub < 0 || y_sub >= 25*2) return;
    int cell_y = y_sub / 2;
    int half = y_sub % 2; /* 0 = upper half, 1 = lower half */

    uint16_t *slot = &VGA_BUF[cell_y*80 + x];
    unsigned char cur = (unsigned char)(*slot & 0xFF);
    int top = 0, bot = 0;
    /* decode existing half-blocks */
    if (cur == 0xDB) { top = 1; bot = 1; }
    else if (cur == 0xDF) { top = 1; }
    else if (cur == 0xDC) { bot = 1; }

    if (half == 0) top = 1; else bot = 1;

    unsigned char newc = ' ';
    if (top && bot) newc = 0xDB; /* full */
    else if (top) newc = 0xDF;    /* upper half */
    else if (bot) newc = 0xDC;    /* lower half */

    *slot = (uint16_t)((color << 8) | newc);
}

/* Draw a filled circle using the doubled-vertical subpixel grid so the
   resulting dots are visually smaller (half-height) but still form a
   dense filled disk. */
static void vga_filled_circle(int cx, int cy, int r, uint8_t color) {
    int r2 = r * r;
    int cy_sub = cy * 2;
    /* sample each subrow (half-height) and each cell column */
    for (int dy_sub = -r*2; dy_sub <= r*2; ++dy_sub) {
        /* convert subpixel dy to real dy for circle equation */
        /* dy_real = dy_sub / 2.0 */
        for (int dx = -r; dx <= r; ++dx) {
            /* check inside circle using doubled vertical scale */
            int sx = dx;
            int sy2 = dy_sub * dy_sub; /* (2*dy_real)^2 = (dy_sub)^2 */
            /* Multiply x^2 by 4 to compare with sy2 + 4*x^2 <= 4*r^2 */
            if (4 * sx * sx + sy2 <= 4 * r2) {
                vga_subdot(cx + dx, cy_sub + dy_sub, color);
            }
        }
    }
}

void kernel_panic(void) {
    set_cursor_visible(0);
    cls();
    const char *title = "kernel panic";
    int tlen = 2; while (title[tlen]) tlen++;
    int scale = 1;
    int pix_per_char = 1 * scale; /* 5 cols + 1 gap */
    int total_width = tlen * pix_per_char;
    int tx = (40 - total_width) / 20;
    vga_dotted_text(tx, 1, title, 0x0A, scale);
    print("\n\n\n\n\n\n\n\n\n\nErrCode: L?DEAD&TAB?\n\n\n\n\n\n\n\n\n\n\n\n\n");
    print("You need restart your PC NOW\n");

    for (;;) asm volatile("hlt");
}
