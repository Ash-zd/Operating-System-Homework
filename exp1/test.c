#define _GNU_SOURCE
#include <stdio.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/resource.h>
#include <stdlib.h>
#define MAX_RT_PRIO 100
#define PRIO_TO_NICE(_prio) ((_prio) - MAX_RT_PRIO - 20) 
#define __NR_mysyscall 334
int main() {
    int pid, flag, nicevalue;
    int prio = 0, nice = 0;
    //printf("Please input the pid\n");
    //scanf("%d", &pid);
    pid = getpid();
    printf("Please input the flag.\n");
    scanf("%d", &flag);
    printf("Please input the nicevalue\n");
    scanf("%d", &nicevalue);
    if (syscall(__NR_mysyscall, pid, flag, nicevalue, (void *)&prio, (void *)&nice) == 0) {
        printf("Done! prio = %d, nice = %d\n", prio + 100, nice);
    } else {
        printf("System call error!\n");
        exit(-1);
    }
    while(1);
    return 0;
}
