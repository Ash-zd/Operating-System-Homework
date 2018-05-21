/**
 * * 0x02 利用linux的消息队列通信机制实现两个线程间的通信
 * 编写程序创建两个线程：sender 线程和 receive 线程，其中 sender 线程运行函数 sender()，它创建一个消息队列，
 * 然后，循环等待用户通过终端输入一串字符，将这串字符通过消息队列发送给 receiver 线程，直到用户输入“exit”为止；
 * 最后，它向 receiver 线程发送消息“end”，并且等待 receiver 的应答，等到应答消息后，将接收到的应答信息显示在终端屏幕上，
 * 删除相关消息队列，结束程序的运行。Receiver 线程运行 receive()，它通过消息队列接收来自sender 的消息，将消息显示在终端屏幕上，
 * 直至收到内容为“end”的消息为止，此时，它向 sender 发送一个应答消息“over”，结束程序的运行。
 * 使用无名信号量实现两个线程之间的同步与互斥。
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/msg.h>
#include <semaphore.h>
#include <string.h>
#include <pthread.h>
#include <sys/types.h>

#define MAXMESSAGE 1024

struct msg_st {
    long int msg_type;
    char text[MAXMESSAGE];
}message;

void sender();
void receiver();
sem_t *send_sem;
sem_t *receive_sem;


int main()
{
    pthread_t send, receive;  
    send_sem = sem_init(send_sem, 0, 0);
    receive_sem = sem_init(receive_sem, 0, 0);
    int msgid = msgget((key_t)1234, 0666 | IPC_CREAT);
    if (msgid == -1) {
        printf("msgget failed.\n");
        exit(-1);
    }
    if (pthread_create())
    

}

void sender() {
    struct msg_st sendMsg;
    sendMsg.msg_type = 1;
    int i = 0;
    while(i != 0) {
        sem_wait(send_sem);
        printf("Please input message.");
        scanf("%s", sendMsg.text);
        if (strcmp(sendMsg.text, "exit") == 0) {
            strcpy(sendMsg.text, "end");
            i = 1;
        }
        if (msgsnd(1234, &sendMsg, sizeof(struct msg_st), 0) == 0) {
            sem_post(receive_sem);
        }
        else {
            printf("message send failed.\n");
        }
    }
    sem_wait(send_sem);
    if (msgrcv(1234, &sendMsg, sizeof(struct msg_st), 0, 0) == -1) {
        printf("message receive failed.\n");
    } else {
        if (strcmp(sendMsg.text, "over") == 0) {
            printf("%s\n", sendMsg.text);
            printf("receive message done!\n");
        }
    }
    return;
}
void receiver() {
    struct msg_st recvMesg;
    recvMesg.msg_type = 1;
    
}