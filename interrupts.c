#include "interrupts.h"
#include "io.h"
#include "vga.h"

/* Matches the order the assembly pushes things onto the stack */
struct regs {
    uint32_t ds;
    uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;
    uint32_t int_no, err_code;
    uint32_t eip, cs, eflags;
};

struct idt_entry {
    uint16_t base_low;
    uint16_t sel;
    uint8_t  always0;
    uint8_t  flags;
    uint16_t base_high;
} __attribute__((packed));

struct idt_ptr {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed));

static struct idt_entry idt[256];
static struct idt_ptr idtp;
static volatile uint32_t ticks = 0;

extern void idt_load(uint32_t ptr);
extern uint32_t isr_stub_table[];

static const char *exception_names[32] = {
    "Divide by zero", "Debug", "Non-maskable interrupt", "Breakpoint",
    "Overflow", "Bound range exceeded", "Invalid opcode", "Device not available",
    "Double fault", "Coprocessor segment overrun", "Invalid TSS",
    "Segment not present", "Stack-segment fault", "General protection fault",
    "Page fault", "Reserved", "x87 floating-point", "Alignment check",
    "Machine check", "SIMD floating-point", "Virtualization", "Control protection",
    "Reserved", "Reserved", "Reserved", "Reserved", "Reserved", "Reserved",
    "Hypervisor injection", "VMM communication", "Security exception", "Reserved"
};

static void idt_set(int n, uint32_t handler) {
    idt[n].base_low  = handler & 0xFFFF;
    idt[n].sel       = 0x08;   /* kernel code segment */
    idt[n].always0   = 0;
    idt[n].flags     = 0x8E;   /* present, ring 0, 32-bit interrupt gate */
    idt[n].base_high = (handler >> 16) & 0xFFFF;
}

void idt_init(void) {
    idtp.limit = sizeof(idt) - 1;
    idtp.base  = (uint32_t)&idt;

    for (int i = 0; i < 48; i++)
        idt_set(i, isr_stub_table[i]);

    idt_load((uint32_t)&idtp);
}

void pic_remap(void) {
    outb(0x20, 0x11);  /* start initialization of both PICs */
    outb(0xA0, 0x11);
    outb(0x21, 0x20);  /* master: IRQs 0-7  -> interrupts 32-39 */
    outb(0xA1, 0x28);  /* slave:  IRQs 8-15 -> interrupts 40-47 */
    outb(0x21, 0x04);  /* tell master there is a slave on IRQ2 */
    outb(0xA1, 0x02);
    outb(0x21, 0x01);  /* 8086 mode */
    outb(0xA1, 0x01);

    outb(0x21, 0xFE);  /* enable only IRQ0 (the timer) for now */
    outb(0xA1, 0xFF);
}

void timer_init(uint32_t hz) {
    uint32_t divisor = 1193180 / hz;
    outb(0x43, 0x36);
    outb(0x40, divisor & 0xFF);
    outb(0x40, (divisor >> 8) & 0xFF);
}

uint32_t timer_ticks(void) {
    return ticks;
}

/* Called from assembly for every interrupt */
void isr_handler(struct regs *r) {
    if (r->int_no < 32) {
        vga_set_color(VGA_WHITE, VGA_RED);
        vga_puts("\nCPU EXCEPTION: ");
        vga_puts(exception_names[r->int_no]);
        vga_puts("\nInterrupt: ");  vga_put_dec((int)r->int_no);
        vga_puts("  Error code: "); vga_put_hex(r->err_code);
        vga_puts("\nEIP: ");        vga_put_hex(r->eip);
        vga_puts("\nSystem halted.");
        for (;;)
            __asm__ volatile ("cli; hlt");
    }

    if (r->int_no == 32)
        ticks++;

    /* Tell the PIC(s) we're done so they send more interrupts */
    if (r->int_no >= 40)
        outb(0xA0, 0x20);
    outb(0x20, 0x20);
}
