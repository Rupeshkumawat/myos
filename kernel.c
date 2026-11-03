#include <stdint.h>
#include "vga.h"
#include "gdt.h"
#include "interrupts.h"

void kernel_main(void) {
    vga_clear();

    vga_set_color(VGA_LIGHT_GREEN, VGA_BLACK);
    vga_puts("Hello from my own OS!\n");
    vga_set_color(VGA_WHITE, VGA_BLACK);

    gdt_init();      vga_puts("[ok] GDT loaded\n");
    idt_init();      vga_puts("[ok] IDT loaded\n");
    pic_remap();     vga_puts("[ok] PIC remapped\n");
    timer_init(100); vga_puts("[ok] Timer at 100 Hz\n\n");

    __asm__ volatile ("sti");   /* turn interrupts on */

    uint32_t last = 0;
    for (;;) {
        uint32_t seconds = timer_ticks() / 100;
        if (seconds != last) {
            last = seconds;
            vga_puts("Uptime: ");
            vga_put_dec((int)seconds);
            vga_puts(" s\n");
        }
        __asm__ volatile ("hlt");   /* sleep until the next interrupt */
    }
}
