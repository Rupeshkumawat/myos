CC = i686-linux-gnu-gcc
CFLAGS = -std=gnu99 -ffreestanding -O2 -Wall -Wextra -fno-pie -fno-stack-protector
LDFLAGS = -T linker.ld -ffreestanding -O2 -nostdlib -no-pie -Wl,--no-warn-rwx-segments

all: kernel.elf

boot.o: boot.asm
	nasm -f elf32 boot.asm -o boot.o

kernel.o: kernel.c
	$(CC) -c kernel.c -o kernel.o $(CFLAGS)

kernel.elf: boot.o kernel.o linker.ld
	$(CC) $(LDFLAGS) -o kernel.elf boot.o kernel.o

run: kernel.elf
	qemu-system-i386 -kernel kernel.elf

clean:
	rm -f *.o kernel.elf

.PHONY: all run clean
