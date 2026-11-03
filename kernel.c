#include "vga.h"

void kernel_main(void) {
    vga_clear();

    vga_set_color(VGA_LIGHT_GREEN, VGA_BLACK);
    vga_puts("Hello from my own OS!\n");

    vga_set_color(VGA_WHITE, VGA_BLACK);
    vga_puts("Screen driver online.\n\n");

    vga_puts("Decimal: ");  vga_put_dec(12345);       vga_putc('\n');
    vga_puts("Negative: "); vga_put_dec(-42);         vga_putc('\n');
    vga_puts("Hex: ");      vga_put_hex(0xDEADBEEF);  vga_putc('\n');
    vga_putc('\n');

    vga_set_color(VGA_YELLOW, VGA_BLACK);
    for (int i = 1; i <= 30; i++) {
        vga_puts("Scroll test line ");
        vga_put_dec(i);
        vga_putc('\n');
    }

    vga_set_color(VGA_LIGHT_CYAN, VGA_BLACK);
    vga_puts("Done. Kernel idle.");
}
