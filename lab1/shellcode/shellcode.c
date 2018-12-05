#include <stdio.h>
#include <unistd.h>
extern char* environ[];

int main(int argc, char* argv[]) {
    char* argv_exec[] = {"/bin/sh",NULL};
    execve("/bin/sh",argv_exec,environ);
    return 0;
}
