#include <unistd.h> // 引入unistd头文件,提供对POSIX操作系统API的访问
#include <stdio.h>  // 引入stdio头文件,提供标准输入输出功能
#include <stdlib.h> // 引入stdlib头文件,提供程序退出等函数

int main()
{
  // 打印提示信息,表明即将运行ps命令
  printf("Running ps with execlp\n");

  // 使用execlp函数执行ps命令,"ps"是命令名,"ax"是命令参数
  // execlp函数会替换当前进程映像,因此"Done."将不会被执行
  execlp("ps", "ps", "ax", 0);

  // 这行代码实际上不会被执行,因为execlp已经替换了进程映像
  // 如果execlp失败,则会执行此处的代码
  printf("Done.\n");

  // 程序正常退出
  exit(0);
}
