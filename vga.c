#include "vga.h"

#define VGA_WIDTH  80
#define VGA_HEIGHT 25
#define VGA_MEMORY ((volatile unsigned short *)0xB8000)

static int cursor_x = 0;
static int cursor_y = 0;
static unsigned char color = 0x0F;  /* white on black */

static inline void outb(unsigned short port, unsigned char val) {
    __asm__ volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
}

static unsigned short entry(char c) {
    return ((unsigned short)color << 8) | (unsigned char)c;
}

static void update_cursor(void) {
    unsigned short pos = cursor_y * VGA_WIDTH + cursor_x;
    outb(0x3D4, 0x0F);
    outb(0x3D5, (unsigned char)(pos & 0xFF));
    outb(0x3D4, 0x0E);
    outb(0x3D5, (unsigned char)((pos >> 8) & 0xFF));
}

void vga_set_color(unsigned char fg, unsigned char bg) {
    color = (unsigned char)((bg << 4) | (fg & 0x0F));
}

void vga_clear(void) {
    for (int i = 0; i < VGA_WIDTH * VGA_HEIGHT; i++)
        VGA_MEMORY[i] = entry(' ');
    cursor_x = 0;
    cursor_y = 0;
    update_cursor();
}

static void scroll(void) {
    for (int y = 1; y < VGA_HEIGHT; y++)
        for (int x = 0; x < VGA_WIDTH; x++)
            VGA_MEMORY[(y - 1) * VGA_WIDTH + x] = VGA_MEMORY[y * VGA_WIDTH + x];
    for (int x = 0; x < VGA_WIDTH; x++)
        VGA_MEMORY[(VGA_HEIGHT - 1) * VGA_WIDTH + x] = entry(' ');
    cursor_y = VGA_HEIGHT - 1;
}

void vga_putc(char c) {
    if (c == '\n') {
        cursor_x = 0;
        cursor_y++;
    } else if (c == '\r') {
        cursor_x = 0;
    } else if (c == '\b') {
        if (cursor_x > 0) {
            cursor_x--;
            VGA_MEMORY[cursor_y * VGA_WIDTH + cursor_x] = entry(' ');
        }
    } else if (c == '\t') {
        cursor_x = (cursor_x + 8) & ~7;
    } else {
        VGA_MEMORY[cursor_y * VGA_WIDTH + cursor_x] = entry(c);
        cursor_x++;
    }

    if (cursor_x >= VGA_WIDTH) {
        cursor_x = 0;
        cursor_y++;
    }
    if (cursor_y >= VGA_HEIGHT)
        scroll();
    update_cursor();
}

void vga_puts(const char *s) {
    for (int i = 0; s[i] != '\0'; i++)
        vga_putc(s[i]);
}

void vga_put_dec(int n) {
    char buf[12];
    int i = 0;
    unsigned int u;

    if (n < 0) {
        vga_putc('-');
        u = 0u - (unsigned int)n;
    } else {
        u = (unsigned int)n;
    }
    if (u == 0) {
        vga_putc('0');
        return;
    }
    while (u > 0) {
        buf[i++] = (char)('0' + (u % 10));
        u /= 10;
    }
    while (i > 0)
        vga_putc(buf[--i]);
}

void vga_put_hex(unsigned int n) {
    const char *digits = "0123456789ABCDEF";
    vga_puts("0x");
    for (int i = 28; i >= 0; i -= 4)
        vga_putc(digits[(n >> i) & 0xF]);
}
