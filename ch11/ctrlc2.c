// 引入信号处理头文件
#include <signal.h>
// 引入标准输入输出头文件
#include <stdio.h>
// 引入unistd头文件,用于sleep函数
#include <unistd.h>

// 定义信号处理函数ouch,当接收到SIGINT信号时执行
void ouch(int sig)
{
  // 打印接收到的信号
  printf("OUCH! - 我收到了信号 %d\n", sig);
}

int main()
{
  // 定义信号动作结构体
  struct sigaction act;

  // 设置信号处理函数为ouch
  act.sa_handler = ouch;
  // 清空信号掩码集合
  sigemptyset(&act.sa_mask);
  // 设置信号动作标志为0,表示没有特殊标志
  act.sa_flags = 0;

  // 使用sigaction函数注册SIGINT信号的处理方式为act
  sigaction(SIGINT, &act, 0);

  // 无限循环,每秒打印一次"Hello World!"
  while (1)
  {
    printf("Hello World!\n");
    sleep(1);
  }
}
