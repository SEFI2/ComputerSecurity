#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h> 
#include <fcntl.h>

extern char* environ[];

volatile int flag = 2;


size_t i;
char* c;
int fd;
int len;
int main(int argc, char* argv[]) {

    char buf[4];
    char bufshell[1024];
    i = 0;
    c = argv[1];
    fd = open(argv[2],O_RDONLY);
    len = read(fd,bufshell,1024);

    while(*c != '\0') {
        buf[i] = *c;
        i++;
        c++;
    }

    printf("I'm done\n");


    return 0;
}
