#define DATA_PORT   0x60
#define STATUS_PORT 0x64

int death = 0;

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
}

char get_chars() {
	
}
