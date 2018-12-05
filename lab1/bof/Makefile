all: bof driver

bof: bof.c
	gcc -Wall -z execstack -fno-stack-protector \
		-Wno-unused-but-set-variable \
		$< -o $@
driver: driver.c
	gcc -Wall $< -o $@
clean:
	rm -f bof driver
