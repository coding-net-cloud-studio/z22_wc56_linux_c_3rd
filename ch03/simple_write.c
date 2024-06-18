// 引入unistd.h头文件,用于执行I/O操作
#include <unistd.h>
// 引入stdlib.h头文件,用于执行动态内存管理相关函数
#include <stdlib.h>

// simple_write.c 程序,用于向标准输出(文件描述符1)写入数据

int main()
{
  // 尝试向文件描述符1(标准输出)写入字符串 "这里是一些数据",长度为18字节
  if ((write(1, "Here is some data\n", 18)) != 18)
    // 如果写入失败,则向文件描述符2(标准错误)写入错误信息 "在文件描述符1上发生了写错误",长度为46字节
    write(2, "A write error has occurred on file descriptor 1\n", 46);

  // 程序正常退出
  exit(0);
}

