all: layout layout-insecure

layout: layout.c
	gcc  -Wall -O0 $< -o $@

layout-insecure: layout.c
	gcc  -Wall -O0 -z execstack -fno-stack-protector $< -o $@


clean:
	rm -f layout layout-insecure
