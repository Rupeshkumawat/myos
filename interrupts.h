#ifndef INTERRUPTS_H
#define INTERRUPTS_H

#include <stdint.h>

void idt_init(void);
void pic_remap(void);
void timer_init(uint32_t hz);
uint32_t timer_ticks(void);

#endif
