#include <stdio.h>
#include <sys/utsname.h>

int main(void){
    struct utsname info;

    if(uname(&info) == -1){
        perror("Error uname");
        return 1;
    }

    printf("%s, %s, %s, %s, %s\n", info.sysname, info.nodename, info.release, info.version, info.machine);

    return 0;
}