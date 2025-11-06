#ifndef PS2_KEYBOARD_H
#define PS2_KEYBOARD_H

#define DATA_PORT   0x60
#define STATUS_PORT 0x64

extern int death;

static inline unsigned char inb(unsigned short port);
static inline void outb(unsigned short port, unsigned char value);

void send(unsigned char command);
unsigned char get(void);
void ps2_init(void);

#endif
