#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
int main(void) {
    /*int fd = open("/home/kadyr/secret",O_RDONLY);
    char buf[1024];
    int rc = read(fd,buf,1024);
    printf("%s\n",buf);*/
	int res = remove ("/home/kadyr/secret");
	printf("Delete status: %d\n", res);
				
    return 0;
}
