#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

int main(int argc, char *argv[]) {

    int fd = open("lock", O_CREAT | O_RDWR, S_IRWXU);
    if (fd == -1)
        printf("error open lock\n");


    FILE *file;
    file = fdopen(fd, "w");
//    mx_lockfile(fd);
    flockfile(file);

    if(ftrylockfile(file) != 0) {
        printf("error lock2");
        return -1;
    }

    printf("write\n");
    int a = write(fd, "00000", 6);
    printf("return write=%d\n", a);
    sleep(20);
return 0;
}


