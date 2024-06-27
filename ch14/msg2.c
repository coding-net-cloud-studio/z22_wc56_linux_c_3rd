/* 发送者程序与 msg1.c 非常相似.在主设置中,删除 msg_to_receive 声明并将其替换为 buffer[BUFSIZ],
删除消息队列删除,并对运行循环进行以下更改.现在我们调用 msgsnd 来将输入的文本发送到队列中. */

#include<stdlib.h> // 用于标准库函数,如 exit
#include<stdio.h>  // 用于标准输入输出函数,如 printf, fgets
#include<string.h> // 用于字符串处理函数,如 strcpy
#include<errno.h>  // 用于获取系统调用的错误码
#include<unistd.h> // 用于系统调用,如 msgget, msgsnd

#include <sys/msg.h> // 用于消息队列操作

#define MAX_TEXT 512 // 定义最大文本长度

// 定义数据结构,用于发送消息
struct my_msg_st
{
    long int my_msg_type; // 消息类型
    char     some_text[MAX_TEXT]; // 存储文本内容
};

int main()
{
    int              running = 1; // 控制程序运行的标志
    struct my_msg_st some_data; // 创建消息结构体变量
    int              msgid; // 消息队列的标识符
    char             buffer[BUFSIZ]; // 用于存储用户输入的文本

    // 创建消息队列,如果不存在则创建,权限设置为 0666,IPC_CREAT 表示不存在则创建
    msgid = msgget((key_t)1234, 0666 | IPC_CREAT);

    // 如果消息队列创建失败,打印错误信息并退出程序
    if (msgid == -1)
    {
        // 打印错误信息到标准错误输出
        fprintf(stderr, "msgget failed with error: %d\n", errno);
        exit(EXIT_FAILURE);
    }

    // 程序主循环
    while (running)
    {
        // 打印提示行,让用户输入文本
        printf("Enter some text: ");

        // 读取用户输入的文本,fgets 会读取整行包括换行符
        fgets(buffer, BUFSIZ, stdin);

        // 设置消息类型为 1
        some_data.my_msg_type = 1;

        // 将用户输入的文本复制到消息结构体中
        strcpy(some_data.some_text, buffer);

        // 将消息发送到队列中,如果发送失败,打印错误信息并退出程序
        if (msgsnd(msgid, (void *)&some_data, MAX_TEXT, 0) == -1)
        {
            // 打印错误信息到标准错误输出
            fprintf(stderr, "msgsnd failed\n");
            exit(EXIT_FAILURE);
        }

        // 检查用户是否输入 "end",如果是则结束循环
        if (strncmp(buffer, "end", 3) == 0)
        {
            running = 0;
        }
    }

    // 程序成功执行完毕,正常退出
    exit(EXIT_SUCCESS);
}
