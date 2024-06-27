/* 第二个程序是生产者,允许我们为客户端输入数据.
它与shm1.c非常相似,看起来像这样. */

#include <unistd.h>  // 用于sleep函数
#include <stdlib.h>  // 用于exit函数
#include <stdio.h>   // 用于printf和fgets函数
#include <string.h>  // 用于strncpy函数

#include <sys/shm.h>  // 用于共享内存函数

#include "shm_com.h"  // 包含共享内存结构定义

int main()
{
    int                   running       = 1;          // 控制生产者的运行状态
    void                 *shared_memory = (void *)0;  // 共享内存的指针初始化为NULL
    struct shared_use_st *shared_stuff;               // 指向共享内存中数据的指针
    char                  buffer[BUFSIZ];             // 用于存储用户输入的文本缓冲区
    int                   shmid;                      // 共享内存标识符

    // 使用key_t类型的1234作为键值,创建或获取一个大小为sizeof(struct shared_use_st)的共享内存段
    shmid = shmget((key_t)1234, sizeof(struct shared_use_st), 0666 | IPC_CREAT);

    // 如果shmget失败,打印错误信息并退出程序
    if (shmid == -1)
    {
        fprintf(stderr, "shmget failed\n");
        exit(EXIT_FAILURE);
    }

    // 将共享内存附加到进程地址空间
    shared_memory = shmat(shmid, (void *)0, 0);
    // 如果shmat失败,打印错误信息并退出程序
    if (shared_memory == (void *)-1)
    {
        fprintf(stderr, "shmat failed\n");
        exit(EXIT_FAILURE);
    }

    printf("Memory attached at %X\n", (int)shared_memory);  // 打印共享内存的地址

    // 将共享内存的指针转换为正确的类型
    shared_stuff = (struct shared_use_st *)shared_memory;
    // 当生产者仍在运行时,循环继续
    while (running)
    {
        // 如果共享内存中的标志位表明消费者正在写入,则生产者等待
        while (shared_stuff->written_by_you == 1)
        {
            sleep(1);                           // 等待一秒钟
            printf("waiting for client...\n");  // 打印等待信息
        }
        printf("Enter some text: ");   // 提示用户输入文本
        fgets(buffer, BUFSIZ, stdin);  // 从标准输入读取文本到缓冲区

        // 将用户输入的文本复制到共享内存中,但不超过TEXT_SZ的大小
        strncpy(shared_stuff->some_text, buffer, TEXT_SZ);
        shared_stuff->written_by_you = 1;  // 设置标志位,表明生产者已经写入

        // 如果用户输入的是"end",则停止生产者
        if (strncmp(buffer, "end", 3) == 0)
        {
            running = 0;
        }
    }

    // 将共享内存从进程地址空间中分离
    if (shmdt(shared_memory) == -1)
    {
        fprintf(stderr, "shmdt failed\n");
        exit(EXIT_FAILURE);
    }
    exit(EXIT_SUCCESS);  // 正常退出程序
}
