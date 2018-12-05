#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
void target(void){
    printf("opps\n");
}

char global;
int main(int argc, char* argv[], char* envp[]) {

    int l = 0;
    volatile int* is_admin_stack = &l;
    int* is_admin_heap = (int*)malloc(sizeof(int));
    *is_admin_heap = 0;
    uint32_t stack;
    uint32_t heap;
    uint32_t ret;
    int pid = getpid();
    char buf[1024];
    sprintf(buf,"cat /proc/%d/maps",pid);

    printf("this is my memory layout:\n\n");
    printf("---------------------------\n");
    printf("addr range                perm  size       \n");
    system(buf);
    printf("---------------------------\n");



    printf("%p is @ stack\n",is_admin_stack);
    printf("%p is @ heap\n",is_admin_heap);
    printf("%p is @ global\n",&global);

    sscanf(argv[1],"%x",&stack); 
    sscanf(argv[2],"%x",&heap);    
    sscanf(argv[3],"%x",&ret);

    printf("%x\n",stack);
    printf("%x\n",heap);

    *(volatile int*)stack = 1;
    *(volatile int*)heap = 1;
    printf("is_admin_stack: %d\n",*is_admin_stack);
    printf("is_admin_heap: %d\n",*is_admin_heap);

    return 0;

}
