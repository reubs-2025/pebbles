#define DATA_PORT   0x60
#define STATUS_PORT 0x64

int death = 0;
int got;

char scanCodeToASCII[128] = {
    0,    // 0x00
    0,    // 0x01 ESC
    '1',  // 0x02
    '2',  // 0x03
    '3',  // 0x04
    '4',  // 0x05
    '5',  // 0x06
    '6',  // 0x07
    '7',  // 0x08
    '8',  // 0x09
    '9',  // 0x0A
    '0',  // 0x0B
    '-',  // 0x0C
    '=',  // 0x0D
    '\b', // 0x0E Backspace
    '\t', // 0x0F Tab
    'q',  // 0x10
    'w',  // 0x11
    'e',  // 0x12
    'r',  // 0x13
    't',  // 0x14
    'y',  // 0x15
    'u',  // 0x16
    'i',  // 0x17
    'o',  // 0x18
    'p',  // 0x19
    '[',  // 0x1A
    ']',  // 0x1B
    '\n', // 0x1C Enter
    0,    // 0x1D Ctrl
    'a',  // 0x1E
    's',  // 0x1F
    'd',  // 0x20
    'f',  // 0x21
    'g',  // 0x22
    'h',  // 0x23
    'j',  // 0x24
    'k',  // 0x25
    'l',  // 0x26
    ';',  // 0x27
    '\'', // 0x28
    '`',  // 0x29
    0,    // 0x2A Shift
    '\\', // 0x2B
    'z',  // 0x2C
    'x',  // 0x2D
    'c',  // 0x2E
    'v',  // 0x2F
    'b',  // 0x30
    'n',  // 0x31
    'm',  // 0x32
    ',',  // 0x33
    '.',  // 0x34
    '/',  // 0x35
    0,    // 0x36 Right Shift
    '*',  // 0x37 Keypad *
    0,    // 0x38 Alt
    ' ',  // 0x39 Space
};

static inline unsigned char inb(unsigned short port) {
    unsigned char ret;
    __asm__ volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

static inline void outb(unsigned short port, unsigned char value) {
    __asm__ volatile ("outb %0, %1" : : "a"(value), "Nd"(port));
}

void send(unsigned char command) {
    while (inb(STATUS_PORT) & 0x02);
    outb(DATA_PORT, command);
}

unsigned char get() {
    while (!(inb(STATUS_PORT) & 0x01));
    return inb(DATA_PORT);
}

void sndchk(unsigned char command) {
	send(command);
	got = get();
	while (got != 0xFA) {
		send(command);
		got = get();
	}
}

void ps2_init() {
    send(0xAD);
    send(0xA7);
    send(0x20);
    unsigned char data_byte = get();
    if (!(data_byte & 0x04)) {
        death = 1;
        return;
    }
    send(0x60);
    send(0x45);
    send(0xAA);
	
    int he = 0;
    data_byte = get();
    while (data_byte != 0x55 && he >= 1000000) {
        data_byte = get();
        he++;
    }
    if (he > 999999) {
        death = 1;
        return;
    }
    send(0xAE);
    send(0xFF);
    got = get();
    while (got == 0xFE) {
	    send(0xFF);
		got = get();
	}
	got = get();
	if (got != 0xAA) {
		death = 1;
	}
	sndchk(0xF4);
}

char get_chars() {
	got = get();
	if (got & 0x80) return 0;
	return scanCodeToASCII[got];
}
