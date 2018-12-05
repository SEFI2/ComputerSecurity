#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

#define DEVICE_FILE "/dev/hello"

ssize_t read_pipe(void *dest, void *src, size_t count) {
    int pipefd[2];
    ssize_t len;

    pipe(pipefd);
    len = write(pipefd[1], src, count);
    if(len != count) {
        printf("failed read_pipe (write) @ %p: %d %d\n",src, (int)len, errno);
        return len;
    }
    len = read(pipefd[0],dest,count);
    if(len != count) {
        printf("failed read_pipe (read) @ %p: %d %d\n",src, (int)len, errno);
        return len;
    }

    close(pipefd[0]);
    close(pipefd[1]);
}

void scan() {
    unsigned long* pt = (unsigned long*)0xdf6ba800;
    for(size_t i = 0; i < 5; i+=1) {
        unsigned long buf;
        read_pipe(&buf,pt+i,sizeof(unsigned long));
        printf("%p: %lx\n",pt+i,buf);
    }
}

int main(int argc, char* argv[]) {

    printf("scan before:\n");
    scan();
    int fd = open(DEVICE_FILE, O_RDWR, 0600);
    if (fd < 0) {
        printf("failed to open (1st) %s: %s(%d)\n",DEVICE_FILE,strerror(errno),errno);
        return 1;
    }
    /*
    int fd2 = open(DEVICE_FILE, O_RDWR, 0600);
    if (fd2 < 0) {
        printf("failed to open (2nd) %s: %s(%d)\n",DEVICE_FILE,strerror(errno),errno);
        return 1;
    }
    */
    char buffer[10];
    ssize_t rc = read(fd,buffer,10);

    printf("rc: %d\n",rc);
    if(rc < 0) {
        printf("failed to read (1st) %s: %s(%d)\n",DEVICE_FILE,strerror(errno),errno);
    }
    buffer[rc] = '\0';
    printf("read: %s\n",buffer);

    printf("scan after:\n");
    scan();


    close(fd);


    return 0;
}
