/* 我们的第一个程序是一个消费者.在头部之后,共享内存段(我们的共享内存结构的大小)是通过调用shmget创建的,
   其中指定了IPC_CREAT位. */

#include <unistd.h>  // 用于unistd相关的功能,如sleep
#include <stdlib.h>  // 用于exit函数
#include <stdio.h>   // 用于printf和fprintf
#include <string.h>  // 用于strncmp函数

#include <sys/shm.h>  // 用于共享内存相关的函数,如shmget, shmat, shmdt, shmctl

#include "shm_com.h"  // 假设这是包含shared_use_st结构定义的头文件

int main()
{
    int                   running       = 1;          // 控制循环的变量
    void                 *shared_memory = (void *)0;  // 初始化共享内存指针为NULL
    struct shared_use_st *shared_stuff;               // 指向共享内存中数据的指针
    int                   shmid;                      // 共享内存标识符

    srand((unsigned int)getpid());  // 初始化随机数种子

    // 创建或获取键值为1234的共享内存段,大小为shared_use_st结构的大小,权限为0666
    shmid = shmget((key_t)1234, sizeof(struct shared_use_st), 0666 | IPC_CREAT);

    if (shmid == -1)  // 检查shmget是否成功
    {
        fprintf(stderr, "shmget failed\n");  // 输出错误信息到标准错误
        exit(EXIT_FAILURE);                  // 如果失败,退出程序
    }

    // 将共享内存附加到程序地址空间
    shared_memory = shmat(shmid, (void *)0, 0);
    if (shared_memory == (void *)-1)  // 检查shmat是否成功
    {
        fprintf(stderr, "shmat failed\n");  // 输出错误信息到标准错误
        exit(EXIT_FAILURE);                 // 如果失败,退出程序
    }

    printf("Memory attached at %X\n", (int)shared_memory);  // 打印附加的共享内存地址

    // 将共享内存段分配给shared_stuff指针,然后打印written_by_you中的任何文本
    // 循环继续,直到在written_by_you中找到"end"
    // sleep调用强制消费者在其临界区等待,从而使生产者等待
    shared_stuff                 = (struct shared_use_st *)shared_memory;
    shared_stuff->written_by_you = 0;  // 初始化标志位
    while (running)
    {
        if (shared_stuff->written_by_you)  // 如果标志位被设置,表示有数据写入
        {
            printf("You wrote: %s", shared_stuff->some_text);     // 打印数据
            sleep(rand() % 4);                                    // 随机等待一段时间
            shared_stuff->written_by_you = 0;                     // 重置标志位
            if (strncmp(shared_stuff->some_text, "end", 3) == 0)  // 检查是否是结束信号
            {
                running = 0;  // 如果是结束信号,退出循环
            }
        }
    }

    // 最后,分离共享内存然后删除它
    if (shmdt(shared_memory) == -1)  // 检查shmdt是否成功
    {
        fprintf(stderr, "shmdt failed\n");  // 输出错误信息到标准错误
        exit(EXIT_FAILURE);                 // 如果失败,退出程序
    }

    if (shmctl(shmid, IPC_RMID, 0) == -1)  // 检查shmctl是否成功删除共享内存
    {
        fprintf(stderr, "shmctl(IPC_RMID) failed\n");  // 输出错误信息到标准错误
        exit(EXIT_FAILURE);                            // 如果失败,退出程序
    }

    exit(EXIT_SUCCESS);  // 正常退出程序
}
