obj-m += hello.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean


ins: hello.ko
	sudo insmod hello.ko && sudo chown vagrant /dev/hello

rm:
	sudo rmmod hello
