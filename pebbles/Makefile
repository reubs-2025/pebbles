all:
	i686-elf-as src/core/boot.s -o bin/boot.o
	i686-elf-gcc -c src/core/kernel.c -o bin/kernel.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra
	i686-elf-gcc -c src/IO/PS2-keyboard.c -o bin/PS2-keyboard.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra
	i686-elf-gcc -T src/linker.ld -o pebbles.bin -ffreestanding -O2 -nostdlib bin/boot.o bin/kernel.o bin/PS2-keyboard.o -lgcc
	cp pebbles.bin isodir/boot/pebbles.bin
	rm pebbles.bin
	cp src/grub.cfg isodir/boot/grub/grub.cfg
	grub-mkrescue -o pebbles.iso isodir
	
boot:
	qemu-system-i386 -cdrom pebbles.iso

clean:
	rm -r isodir/*
	mkdir isodir/boot/grub/
	rm -r bin/*
