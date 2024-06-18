#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>

// 定义测试文件的路径
const char *test_file = "/tmp/test_lock";

int main()
{
  int file_desc;             // 文件描述符
  int byte_count;            // 用于循环写入文件的计数器
  char *byte_to_write = "A"; // 要写入文件的字符
  struct flock region_1;     // 定义第一个锁区域
  struct flock region_2;     // 定义第二个锁区域
  int res;                   // 用于存储锁操作的返回值

  // 打开一个文件描述符,读写模式,如果文件不存在则创建,权限为0666
  file_desc = open(test_file, O_RDWR | O_CREAT, 0666);
  if (!file_desc)
  {
    // 如果无法打开文件,输出错误信息并退出程序
    fprintf(stderr, "无法打开 %s 进行读写\n", test_file);
    exit(EXIT_FAILURE);
  }

  // 向文件中写入数据
  for (byte_count = 0; byte_count < 100; byte_count++)
  {
    (void)write(file_desc, byte_to_write, 1);
  }

  // 设置第一个锁区域,共享锁,从第10个字节到第30个字节
  region_1.l_type = F_RDLCK;
  region_1.l_whence = SEEK_SET;
  region_1.l_start = 10;
  region_1.l_len = 20;

  // 设置第二个锁区域,排他锁,从第40个字节到第50个字节
  region_2.l_type = F_WRLCK;
  region_2.l_whence = SEEK_SET;
  region_2.l_start = 40;
  region_2.l_len = 10;

  // 锁定文件
  printf("进程 %d 正在锁定文件\n", getpid());
  // 尝试设置第一个锁区域
  res = fcntl(file_desc, F_SETLK, &region_1);
  if (res == -1)
    fprintf(stderr, "无法锁定区域 1\n");
  // 尝试设置第二个锁区域
  res = fcntl(file_desc, F_SETLK, &region_2);
  if (res == -1)
    fprintf(stderr, "无法锁定区域 2\n");

  // 等待一段时间
  sleep(60);

  // 关闭文件并退出程序
  printf("进程 %d 正在关闭文件\n", getpid());
  close(file_desc);
  exit(EXIT_SUCCESS);
}
