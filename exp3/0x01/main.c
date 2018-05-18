/* 0x01 实现一个管道通信程序
 * 由父进程创建一个管道，然后再创建 3 个子进程，并由这三个子进程利用管道与父进程之间进行通信：
 * 子进程发送信息，父进程等三个子进程全部发完消息后再接收信息。通信的具体内容可根据自己的需要随意设计，
 * 要求能试验阻塞型读写过程中的各种情况，测试管道的默认大小，并且要求利用 Posix 信号量机制实现进程间对管道的互斥访问。
 * 运行程序，观察各种情况下，进程实际读写的字节数以及进程阻塞唤醒的情况。
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <limits.h>
#include <string.h>
#include <semaphore.h> 
#include <sys/wait.h>

#define BUFSIZE PIPE_BUF
int fd[2];//fd[0]=读管道 fd[1]=写管道
sem_t *sem = NULL;

void error_quit(char *s) {
    printf("%s", s);
    exit(-1);
}

void pipe_write(int id) {
    close(fd[0]);
    sem_wait(sem);//申请POSIX信号量
    char message[64];
    printf("This is child %d, pid %d, pipe write init.\n", id + 1, getpid());
    sprintf(message, "This is child %d, pid %d, pipe write init.\n", id + 1, getpid());
    int ret = write(fd[1], "pipe write!", BUFSIZE);
    if (ret != -1) {
        printf("child %d pipe write done!\n", id + 1);
    } 
    else {
        printf("child %d pipe write failed!\n", id + 1);
    }
    sleep(1);
    sem_post(sem);//释放POSIX信号量
    return;
}

int main()
{
    int pid[3]; //3个子进程
    
    char buf[BUFSIZE];
    int len;

    //父进程创建管道
    if (pipe(fd) < 0) {
        error_quit("pipe failed!\n");
    }
    if (sem_init(sem, 1, 0) < 0) {
        error_quit("sem_init error!\n");
    }
    //fork子进程
    if ((pid[0] = fork()) < 0) {//子进程1 创建
        error_quit("fork failed!\n");//创建失败
    }
    if (pid[0] == 0) {//子进程1 写
        pipe_write(0);
    }
    else {
        if ((pid[1] = fork()) < 0) {//子进程2 创建
            error_quit("fork failed!\n");//失败
        }
        if (pid[1] == 0) {//子进程2 写
            pipe_write(1);
        }
        else {
            if ((pid[2] = fork()) < 0) {//子进程3 创建
                error_quit("fork failed!\n");//失败
            }
            if (pid[2] == 0) {//子进程3 写
                pipe_write(2);
            }
            else {//父进程 等待子进程结束
                wait(NULL);
                wait(NULL);
                wait(NULL);
                close(fd[1]);//子进程关闭写通道 父进程读管道数据
                printf("father process %d read:\n", getpid());
                int len;
                for (int i = 0; i < 3; i++) {
                    len = read(fd[0], buf, BUFSIZE);//从管道读数据
                    if (len != -1) {
                        write(STDOUT_FILENO, buf, len);//将读出数据写入到标准输出流
                    } 
                    else {
                        error_quit("process failed when read a pipe.\n");
                    }
                }
            }
        }
    }
    sem_destroy(sem);
    return 0;
}