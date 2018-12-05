all: driver shellcode.bin injection driver2 driver3

driver: driver.c
	gcc -z execstack -fno-stack-protector \
		-Wno-incompatible-pointer-types $< -o $@

shellcode.bin: shellcode.S
	gcc -c $< -o shellcode.o
	objcopy -S -O binary -j .text shellcode.o $@




driver2: driver2.c
	gcc -z execstack -fno-stack-protector $< -o $@


injection: injection.c
	gcc -O0 -g -z execstack -fno-stack-protector $< -o $@

driver3: driver3.c
	gcc -z execstack -fno-stack-protector \
		-Wno-incompatible-pointer-types $< -o $@




clean:
	rm -f shellcode.bin driver shellcode.o injection driver2 driver3
