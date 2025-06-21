#include <unistd.h>
#include <stdio.h>

int main(void){
    
    printf("%ld, %ld, %ld, %ld, %ld, %ld, %ld, %ld\n", sysconf(_SC_CLK_TCK), sysconf(_SC_CHILD_MAX), sysconf(_SC_OPEN_MAX), sysconf(_SC_PAGESIZE), sysconf(_SC_ARG_MAX), pathconf("/home", _PC_LINK_MAX), pathconf("/home", _PC_PATH_MAX), pathconf("/home", _PC_NAME_MAX));

    return 0;
}