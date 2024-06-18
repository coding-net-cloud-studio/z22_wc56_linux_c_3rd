/*  alarm.c 文件中,首个函数 ding 模拟了一个闹钟.  */

#include <signal.h>  // 引入信号处理头文件
#include <stdio.h>   // 引入标准输入输出头文件
#include <unistd.h>  // 引入unistd头文件,用于提供对POSIX操作系统API的访问
#include <stdlib.h>  // 引入标准库头文件,提供如exit等函数

static int alarm_fired = 0;  // 静态变量,用于标记闹钟是否响起

// 信号处理函数,当接收到SIGALRM信号时执行
void ding(int sig)
{
    alarm_fired = 1;  // 将闹钟响标志设置为1
}

/* 主函数中,我们让子进程等待五秒后向其父进程发送SIGALRM信号. */

int main()
{
    pid_t pid;  // 声明进程ID变量

    printf("alarm application starting\n");  // 打印程序启动信息

    pid = fork();  // 创建子进程
    switch (pid)
    {
    case -1:
        /* 创建失败 */
        perror("fork failed");  // 打印错误信息
        exit(1);                // 以错误码退出程序
    case 0:
        /* 子进程 */
        sleep(5);                  // 等待5秒
        kill(getppid(), SIGALRM);  // 向父进程发送SIGALRM信号
        exit(0);                   // 子进程正常退出
    }

    /* 父进程通过调用signal函数来捕获SIGALRM信号,然后等待信号的到来. */

    printf("waiting for alarm to go off\n");  // 打印等待闹钟响的信息
    (void)signal(SIGALRM, ding);              // 设置SIGALRM信号的处理函数为ding

    pause();                // 使进程挂起,直到接收到信号
    if (alarm_fired)        // 检查闹钟是否响起
        printf("Ding!\n");  // 打印闹钟响的信息

    printf("done\n");  // 打印程序结束信息
    exit(0);           // 程序正常退出
}
