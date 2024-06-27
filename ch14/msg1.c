/* 这里是接收方程序. */

#include <stdlib.h>  // 用于标准库函数,如exit
#include <stdio.h>   // 用于输入输出函数,如printf
#include <string.h>  // 用于字符串处理函数,如strncmp
#include <errno.h>   // 用于错误码定义
#include <unistd.h>  // 用于系统调用函数,如msgget

#include <sys/msg.h>  // 用于消息队列相关的函数和类型定义

// 定义消息结构体
struct my_msg_st
{
    long int my_msg_type;        // 消息类型
    char     some_text[BUFSIZ];  // 消息内容
};

int main()
{
    int              running = 1;         // 控制循环的变量
    int              msgid;               // 消息队列的标识符
    struct my_msg_st some_data;           // 存储接收到的消息
    long int         msg_to_receive = 0;  // 指定接收的消息类型

    // 首先,我们设置消息队列.
    msgid = msgget((key_t)1234, 0666 | IPC_CREAT);
    if (msgid == -1)  // 检查msgget是否成功
    {
        fprintf(stderr, "msgget failed with error: %d\n", errno);
        exit(EXIT_FAILURE);  // 如果失败,打印错误并退出
    }

    // 然后从队列中检索消息,直到遇到结束消息.
    // 最后,删除消息队列.
    while (running)
    {
        // 使用msgrcv接收消息
        if (msgrcv(msgid, (void *)&some_data, BUFSIZ, msg_to_receive, 0) == -1)
        {
            fprintf(stderr, "msgrcv failed with error: %d\n", errno);
            exit(EXIT_FAILURE);  // 如果接收失败,打印错误并退出
        }
        printf("You wrote: %s", some_data.some_text);  // 打印接收到的消息

        // 如果接收到的消息是"end",则结束循环
        if (strncmp(some_data.some_text, "end", 3) == 0)
        {
            running = 0;
        }
    }

    // 尝试删除消息队列
    if (msgctl(msgid, IPC_RMID, 0) == -1)
    {
        fprintf(stderr, "msgctl(IPC_RMID) failed\n");
        exit(EXIT_FAILURE);  // 如果删除失败,打印错误并退出
    }

    exit(EXIT_SUCCESS);  // 正常退出程序
}
