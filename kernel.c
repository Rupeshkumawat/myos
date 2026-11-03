void kernel_main(void) {
    volatile unsigned short *vga = (volatile unsigned short *)0xB8000;
    const char *msg = "Hello from my own OS!";

    for (int i = 0; i < 80 * 25; i++)
        vga[i] = (0x0F << 8) | ' ';

    for (int i = 0; msg[i] != '\0'; i++)
        vga[i] = (0x0F << 8) | msg[i];
}

