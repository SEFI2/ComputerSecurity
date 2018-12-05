#include <stdio.h>
#include <string.h>


int main(int argc, char* argv[], char* envp[]){

    if(argc != 2) {
        printf("usage: ./a.out <input>\n");
        return 1;
    }

    volatile int is_admin = 2;
    char buf[4];
    char* c = argv[1];
    size_t i = 0;

    while(*c != '\0') {
        buf[i] = *c;
        i++;
        c++;
    }

    printf("is_admin: %d\n",is_admin);
    return 0;

}
