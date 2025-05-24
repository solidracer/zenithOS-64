cc = x86_64-elf-gcc
ld = x86_64-elf-ld
as = nasm -felf64

cflags = -ffreestanding -fno-stack-protector -fno-strict-aliasing -Wstrict-prototypes -I./include -Wall -Wextra -std=gnu99 -g -O2 -mcmodel=kernel
ldflags = -nostdlib -T linker.ld
asflags = -g

bootflags = -serial stdio -enable-kvm -cpu host -m 8G -no-reboot -audiodev pa,id=speaker -machine pcspk-audiodev=speaker

target = build/kernel.elf

src_c = $(wildcard src/*.c)
src_st= $(wildcard src/start/*.c)
src_s = $(wildcard src/asm/*.s)
obj = $(patsubst src/%.c,build/%.o,$(src_c)) \
	  $(patsubst src/asm/%.s,build/asm_%.o,$(src_s)) \
	  $(patsubst src/start/%.c,build/start_%.o,$(src_st))

all: $(target)

./build:
	@mkdir -p build

$(target): ./build $(obj)
	$(ld) $(ldflags) $(obj) $(shell $(cc) -print-libgcc-file-name) -o $(target)

build/%.o: src/%.c
	$(cc) $(cflags) -c $< -o $@

build/start_%.o: src/start/%.c
	$(cc) $(cflags) -c $< -o $@
	objcopy --rename-section .text=.boot.text --rename-section .data=.boot.data --rename-section .bss=.boot.bss $@

build/asm_%.o: src/asm/%.s
	$(as) $(asflags) $< -o $@

boot: $(target)
	uefi-run -d bootloader/main.efi -b /usr/share/edk2/x64/OVMF.4m.fd -f $(target) -- $(bootflags)

clean:
	@rm -rf build

.PHONY: clean all boot
