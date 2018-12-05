obj-m += syscall.o
obj-m += vfshook.o


all:	
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
	echo "asd" > /home/kadyr/malicious
clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean


