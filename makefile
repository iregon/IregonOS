# Part 1
# sudo apt-get install g++ binutils libc6-dev-i386

# Part 2
# sudo apt-get install VirtualBox grub-legacy xorriso

GCCPARAMS = -m32 -Iinclude -fno-use-cxa-atexit -nostdlib -fno-builtin -fno-rtti -fno-exceptions -fno-leading-underscore -Wno-write-strings -Wpointer-arith
ASPARAMS = --32
LDPARAMS = -melf_i386 -s

objects = obj/loader.o \
          obj/gdt.o \
          obj/memorymanagement.o \
          obj/drivers/driver.o \
          obj/hardwarecommunication/port.o \
          obj/hardwarecommunication/interruptstubs.o \
          obj/hardwarecommunication/interrupts.o \
          obj/syscalls.o \
          obj/multitasking.o \
          obj/drivers/amd_am79c973.o \
          obj/hardwarecommunication/pci.o \
          obj/drivers/keyboard.o \
          obj/drivers/mouse.o \
          obj/drivers/vga.o \
          obj/drivers/ata.o \
          obj/gui/widget.o \
          obj/gui/window.o \
          obj/gui/desktop.o \
          obj/net/etherframe.o \
          obj/net/arp.o \
          obj/kernel.o


# BEGIN generation object files
obj/%.o: src/%.cpp
	mkdir -p $(@D)
	gcc $(GCCPARAMS) -c -o $@ $<

obj/%.o: src/%.s
	mkdir -p $(@D)
	as $(ASPARAMS) -o $@ $<
# END

kernel.bin: linker.ld $(objects)
	ld $(LDPARAMS) -T $< -o $@ $(objects)

# BEGIN generation of iso image
iregonos.iso: kernel.bin
	mkdir iso
	mkdir iso/boot
	mkdir iso/boot/grub
	cp kernel.bin iso/boot/kernel.bin
	echo 'set timeout=0'                      > iso/boot/grub/grub.cfg
	echo 'set default=0'                     >> iso/boot/grub/grub.cfg
	echo ''                                  >> iso/boot/grub/grub.cfg
	echo 'menuentry "Iregon OS" {'           >> iso/boot/grub/grub.cfg
	echo '  multiboot /boot/kernel.bin'      >> iso/boot/grub/grub.cfg
	echo '  boot'                            >> iso/boot/grub/grub.cfg
	echo '}'                                 >> iso/boot/grub/grub.cfg
	grub-mkrescue --output=iregonos.iso iso
	rm -rf iso
# END

# BEGIN launch iso on virtualbox's virtual machine
run: iregonos.iso
	(killall VirtualBoxVM && sleep 5) || true
	VirtualBoxVM --startvm 'Iregon OS' &
# END

# BEGIN copy bin file into boot folder 
install: kernel.bin
	sudo cp $< /boot/kernel.bin
# END

.PHONY: clean
clean:
	rm -rf obj kernel.bin kernel.iso
