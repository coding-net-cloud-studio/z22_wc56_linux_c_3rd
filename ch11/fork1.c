// 引入 sys/types.h 头文件,提供进程和用户相关的数据类型定义
#include <sys/types.h>
// 引入 unistd.h 头文件,提供 Unix 标准函数定义,如 fork()
#include <unistd.h>
// 引入 stdio.h 头文件,提供标准输入输出函数定义,如 printf()
#include <stdio.h>
// 引入 stdlib.h 头文件,提供程序退出函数定义,如 exit()
#include <stdlib.h>

// 主函数
int main()
{
    // 声明一个进程ID变量,用于存储fork()函数的返回值
    pid_t pid;
    // 声明一个字符指针变量,用于存储要打印的消息
    char *message;
    // 声明一个整型变量,用于控制循环次数
    int n;

    // 打印程序启动信息
    printf("fork program starting\n");
    // 调用fork()函数创建一个新进程,并将返回值赋给pid变量
    pid = fork();

    // 根据fork()的返回值进行不同的处理
    switch (pid)
    {
    case -1:  // 如果fork()失败
        // 打印错误信息
        perror("fork failed");
        // 退出程序并返回错误码1
        exit(1);
    case 0:  // 如果是子进程
        // 设置子进程的消息
        message = "这是子进程";
        // 设置子进程的循环次数
        n = 5;
        break;
    default:  // 如果是父进程
        // 设置父进程的消息
        message = "这是父进程";
        // 设置父进程的循环次数
        n = 3;
        break;
    }

    // 循环打印消息
    for (; n > 0; n--)
    {
        // 打印消息
        puts(message);
        // 等待1秒
        sleep(1);
    }
    // 退出程序并返回成功码0
    exit(0);
}
