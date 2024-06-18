// 引入必要的头文件
#include <stdio.h>  // 用于标准输入输出
#include <term.h>   // 用于终端控制
#include <curses.h> // 用于图形化界面控制
#include <stdlib.h> // 用于程序退出等操作

int main()
{
  // 设置终端为未列出模式,使用标准输出文件描述符,并传递空指针作为选项参数
  setupterm("unlisted", fileno(stdout), (int *)0);

  // 打印完成信息
  printf("Done.\n");

  // 退出程序,返回状态码0表示成功
  exit(0);
}
