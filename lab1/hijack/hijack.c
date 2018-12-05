#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <sys/mman.h>
extern char* environ[];


int shellcode(void) {
    char* argv_exec[] = {"/bin/sh",NULL};
    execve("/bin/sh",argv_exec,environ);
    return 0;
}

size_t i = 0;
char* c;
volatile int flag = 2;


int main(int argc, char* argv[]) {
	
    char buf[8];
    c = argv[1];
    while(*c != '\0') {
        buf[i] = *c;
        i++;
        c++;
    }

    if (flag == 0)
        shellcode();
    else
      printf("I'm done\n");


    return 0;
}
