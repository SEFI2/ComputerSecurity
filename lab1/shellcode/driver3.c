#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h> 
#include <fcntl.h>
int main(int argc, char* argv[]){
    char buf[1024];
    read(STDIN_FILENO,buf,1024);
    void (*vul)(void) = buf;
    (*vul)();
    return 0;
};
