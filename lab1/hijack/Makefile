PROG=hijack

all: $(PROG) driver

$(PROG): $(PROG).c
	gcc -g -O0 -Wall -z execstack -fno-stack-protector \
		-Wno-unused-but-set-variable\
		$< -o $@
driver: driver.c
	gcc -Wall $< -o $@
clean:
	rm -f ${PROG} driver
test:
	./hijack aaaa
