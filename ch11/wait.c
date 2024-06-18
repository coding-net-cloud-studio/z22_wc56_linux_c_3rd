// 引入系统头文件,用于进程控制
#include <sys/types.h>
// 引入wait相关头文件,用于等待子进程结束
#include <sys/wait.h>
// 引入unistd头文件,提供对POSIX操作系统API的访问
#include <unistd.h>
// 引入标准输入输出头文件,用于打印信息
#include <stdio.h>
// 引入标准库头文件,提供内存分配释放等功能
#include <stdlib.h>

int main()
{
  // 声明进程ID变量
  pid_t pid;
  // 声明消息字符串指针
  char *message;
  // 声明计数器变量
  int n;
  // 声明退出码变量
  int exit_code;

  // 打印程序启动信息
  printf("fork program starting\n");
  // 进行fork操作,创建子进程
  pid = fork();
  // 根据fork的结果进行不同的处理
  switch (pid)
  {
  case -1:
    // 如果fork失败,则退出程序
    exit(1);
  case 0:
    // 如果是子进程
    message = "这是子进程"; // 设置消息为子进程的信息
    n = 5;                  // 设置循环次数为5
    exit_code = 37;         // 设置子进程的退出码为37
    break;
  default:
    // 如果是父进程
    message = "这是父进程"; // 设置消息为父进程的信息
    n = 3;                  // 设置循环次数为3
    exit_code = 0;          // 设置父进程的退出码为0
    break;
  }

  // 循环打印消息并等待1秒
  for (; n > 0; n--)
  {
    puts(message);
    sleep(1);
  }

  // 父进程等待子进程结束
  if (pid)
  {
    int stat_val;
    pid_t child_pid;

    // 使用wait函数等待子进程结束,并获取子进程的状态值
    child_pid = wait(&stat_val);

    // 打印子进程结束的信息
    printf("子进程已结束:PID = %d\n", child_pid);
    // 判断子进程是否正常退出,并打印退出码
    if (WIFEXITED(stat_val))
      printf("子进程以代码 %d 退出\n", WEXITSTATUS(stat_val));
    else
      printf("子进程异常终止\n");
  }
  // 退出程序,使用之前设置的退出码
  exit(exit_code);
}
