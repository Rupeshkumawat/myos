CC = i686-linux-gnu-gcc
CFLAGS = -std=gnu99 -ffreestanding -O2 -Wall -Wextra -fno-pie -fno-stack-protector
LDFLAGS = -T linker.ld -ffreestanding -O2 -nostdlib -no-pie -Wl,--no-warn-rwx-segments

OBJS = boot.o stubs.o kernel.o vga.o gdt.o interrupts.o

all: kernel.elf

%.o: %.asm
	nasm -f elf32 $< -o $@

%.o: %.c $(wildcard *.h)
	$(CC) -c $< -o $@ $(CFLAGS)

kernel.elf: $(OBJS) linker.ld
	$(CC) $(LDFLAGS) -o $@ $(OBJS)

run: kernel.elf
	qemu-system-i386 -kernel kernel.elf

clean:
	rm -f *.o kernel.elf

.PHONY: all run clean
