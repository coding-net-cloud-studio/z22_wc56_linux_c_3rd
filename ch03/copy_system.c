// 引入必要的头文件
#include <unistd.h>   // 用于文件I/O操作
#include <sys/stat.h> // 用于文件状态操作
#include <fcntl.h>    // 用于文件描述符操作
#include <stdlib.h>   // 用于程序退出

int main()
{
  char c;      // 用于存储读取的字符
  int in, out; // 文件描述符,in用于读取,out用于写入

  // 打开输入文件,只读模式
  in = open("file.in", O_RDONLY);
  // 打开输出文件,读写模式,如果不存在则创建,权限设置为用户可读写
  out = open("file.out", O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);

  // 当从输入文件成功读取一个字符时,将该字符写入输出文件
  while (read(in, &c, 1) == 1)
    write(out, &c, 1);

  // 关闭文件描述符并退出程序
  close(in);
  close(out);
  exit(0);
}
