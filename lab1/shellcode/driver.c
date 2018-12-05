#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h> 
#include <fcntl.h>
int main(int argc, char* argv[]){
    int fd = open(argv[1],O_RDONLY);
    char buf[1024];
    read(fd,buf,1024);
    void (*vul)(void) = buf;
    (*vul)();
    return 0;
};
