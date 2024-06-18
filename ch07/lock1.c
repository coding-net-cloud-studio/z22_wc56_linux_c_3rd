#include <unistd.h> // 引入标准UNIX库,用于系统调用
#include <stdlib.h> // 引入标准库,用于程序退出和错误处理
#include <stdio.h>  // 引入标准输入输出库,用于打印信息
#include <fcntl.h>  // 引入文件控制库,用于文件锁操作
#include <errno.h>  // 引入错误号定义库,用于获取错误信息

int main()
{
  int file_desc;  // 定义文件描述符变量
  int save_errno; // 定义保存错误码的变量

  // 尝试以读写方式创建并打开一个独占锁文件
  file_desc = open("/tmp/LCK.test", O_RDWR | O_CREAT | O_EXCL, 0444);
  // 如果打开失败,记录错误码并打印错误信息
  if (file_desc == -1)
  {
    save_errno = errno;
    printf("打开失败,错误码为 %d\n", save_errno);
  }
  // 如果打开成功,打印成功信息
  else
  {
    printf("打开成功\n");
  }
  // 程序正常退出
  exit(EXIT_SUCCESS);
}
