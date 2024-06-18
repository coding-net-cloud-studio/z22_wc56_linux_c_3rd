#include <sys/types.h> // 引入系统类型定义,包括进程ID类型pid_t
#include <unistd.h>    // 引入unistd库,提供fork等系统调用函数
#include <stdio.h>     // 引入标准输入输出库,用于printf和perror函数
#include <stdlib.h>    // 引入标准库,提供exit函数

int main()
{
  pid_t pid;     // 定义一个pid_t类型的变量pid,用于存储fork函数的返回值
  char *message; // 定义一个字符指针变量message,用于存储要打印的消息
  int n;         // 定义一个整型变量n,用于控制循环次数

  printf("fork program starting\n"); // 打印程序启动信息
  pid = fork();                      // 调用fork函数创建子进程,并将返回值赋给pid
  switch (pid)                       // 根据fork的返回值进行判断
  {
  case -1:                         // 如果fork失败,返回-1
    perror("fork failed");         // 打印错误信息
    exit(1);                       // 以非零状态退出程序
  case 0:                          // 如果fork成功且当前是子进程(pid为0)
    message = "This is the child"; // 设置子进程的消息
    n = 3;                         // 设置子进程的循环次数
    break;
  default:                          // 如果fork成功且当前是父进程(pid不为0)
    message = "This is the parent"; // 设置父进程的消息
    n = 5;                          // 设置父进程的循环次数
    break;
  }

  for (; n > 0; n--)
  {                // 循环打印消息,直到n减到0
    puts(message); // 打印消息
    sleep(1);      // 等待1秒
  }
  exit(0); // 循环结束后以0状态退出程序
}
