#include <stdlib.h> // 引入标准库,提供exit函数
#include <stdio.h>  // 引入标准输入输出库,提供printf函数

int main() // 主函数,程序的入口点
{
  printf("Running ps with system\n"); // 打印信息,说明正在执行ps命令
  system("ps ax");                    // 使用system函数调用shell命令"ps ax",列出所有运行中的进程
  printf("Done.\n");                  // 打印信息,说明ps命令执行完毕
  exit(0);                            // 程序正常退出,返回状态码0
}
