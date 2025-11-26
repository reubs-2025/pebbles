#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "../IO/PS2-keyboard.h"

#if defined(__linux__)
#error "You are not using a cross-compiler, you will most certainly run into trouble"
#endif

#if !defined(__i386__)
#error "This needs to be compiled with a ix86-elf compiler"
#endif

int chars;
int execute = 0;
char command[80] = "";
char name[10] = "reubs";
size_t strlen(const char* str);

static inline void hlt_forever() {
    for (;;) {
        __asm__ volatile ("hlt");
    }
}

void append(char c) {
    size_t len = strlen(command);
    if (len < sizeof(command) - 1) {
        command[len] = c;
        command[len + 1] = '\0';
    }
}

void reset_command(void) {
    command[0] = '\0';
}

int strcmp(const char* a, const char* b) {
    while (*a && (*a == *b)) {
        a++;
        b++;
    }
    return (unsigned char)*a - (unsigned char)*b;
}


enum vga_color {
    VGA_COLOR_BLACK = 0, VGA_COLOR_BLUE = 1, VGA_COLOR_GREEN = 2, VGA_COLOR_CYAN = 3,
    VGA_COLOR_RED = 4, VGA_COLOR_MAGENTA = 5, VGA_COLOR_BROWN = 6, VGA_COLOR_LIGHT_GREY = 7,
    VGA_COLOR_DARK_GREY = 8, VGA_COLOR_LIGHT_BLUE = 9, VGA_COLOR_LIGHT_GREEN = 10,
    VGA_COLOR_LIGHT_CYAN = 11, VGA_COLOR_LIGHT_RED = 12, VGA_COLOR_LIGHT_MAGENTA = 13,
    VGA_COLOR_LIGHT_BROWN = 14, VGA_COLOR_WHITE = 15,
};

static inline uint8_t vga_entry_color(enum vga_color fg, enum vga_color bg) {
    return fg | bg << 4;
}

static inline uint16_t vga_entry(unsigned char uc, uint8_t color) {
    return (uint16_t) uc | (uint16_t) color << 8;
}

size_t strlen(const char* str) {
    size_t len = 0;
    while (str[len]) len++;
    return len;
}

#define VGA_WIDTH   80
#define VGA_HEIGHT  25
#define VGA_MEMORY  0xB8000

size_t terminal_row;
size_t terminal_column;
uint8_t terminal_color;
uint16_t* terminal_buffer = (uint16_t*)VGA_MEMORY;

void terminal_initialize(void) {
    terminal_row = 0;
    terminal_column = 0;
    terminal_color = vga_entry_color(VGA_COLOR_LIGHT_BLUE, VGA_COLOR_BLACK);
    for (size_t y = 0; y < VGA_HEIGHT; y++) {
        for (size_t x = 0; x < VGA_WIDTH; x++) {
            const size_t index = y * VGA_WIDTH + x;
            terminal_buffer[index] = vga_entry(0x00, terminal_color);
        }
    }
}

void terminal_setcolor(uint8_t color) {
    terminal_color = color;
}

void terminal_up(void) {
    for (int row = 1; row < VGA_HEIGHT; row++) {
        for (int col = 0; col < VGA_WIDTH; col++) {
            terminal_buffer[(row - 1) * VGA_WIDTH + col] =
                terminal_buffer[row * VGA_WIDTH + col];
        }
    }
    for (int col = 0; col < VGA_WIDTH; col++) {
        terminal_buffer[(VGA_HEIGHT - 1) * VGA_WIDTH + col] =
            vga_entry(0x00, terminal_color);
    }
}

void terminal_putentryat(char c, uint8_t color, size_t x, size_t y) {
    const size_t index = y * VGA_WIDTH + x;
    terminal_buffer[index] = vga_entry(c, color);
}

void terminal_putchar(char c) {
    if (c != 0x0A) {
        terminal_putentryat(c, terminal_color, terminal_column, terminal_row);
        append(c);
        if (++terminal_column == VGA_WIDTH) {
            terminal_column = 0;
            if (++terminal_row == VGA_HEIGHT) {
                terminal_up();
                terminal_row = VGA_HEIGHT - 1;
            }
        }
    } else {
        terminal_column = 0;
        if (++terminal_row == VGA_HEIGHT) {
            terminal_up();
            terminal_row = VGA_HEIGHT - 1;
        }
        execute = 1;
    }
}


void terminal_write(const char* data, size_t size) {
    for (size_t i = 0; i < size; i++) terminal_putchar(data[i]);
}

void terminal_writestring(const char* data) {
    terminal_write(data, strlen(data));
}

void kernel_check() {
	if (death == 1) {
        terminal_color = vga_entry_color(VGA_COLOR_RED, VGA_COLOR_BLACK);
        terminal_writestring("VERY BAD. STOP NOW.");
        hlt_forever();
    }
    if (execute == 1) {
		if (strcmp(command, "whoami") == 0) {
			terminal_writestring("beans");
		}
	reset_command();
	}
}

void kernel_main(void) {
    terminal_initialize();
	
    ps2_init();
	
    terminal_writestring("ur in \n");
    while (1) {
		kernel_check();
		chars = get_chars();
		if (chars != 0) {
			terminal_putchar(chars);
		}
	}
}
