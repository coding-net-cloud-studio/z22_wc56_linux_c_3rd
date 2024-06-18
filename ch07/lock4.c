// 引入unistd头文件,提供对POSIX操作系统API的访问,包括文件I/O和进程控制功能
#include <unistd.h>
// 引入stdlib头文件,提供通用的函数,如内存分配,数学运算等
#include <stdlib.h>
// 引入stdio头文件,提供标准输入输出功能
#include <stdio.h>
// 引入fcntl头文件,提供文件控制和I/O操作的函数
#include <fcntl.h>

// 定义测试文件的路径
const char *test_file = "/tmp/test_lock";
// 定义尝试锁定的大小
#define SIZE_TO_TRY 5

// 显示锁信息的函数声明
void show_lock_info(struct flock *to_show);

int main()
{
  // 打开一个文件描述符,如果无法打开则输出错误并退出程序
  // 尝试以读写方式创建并打开一个名为test_file的文件
  int file_desc = open(test_file, O_RDWR | O_CREAT, 0666);
  // 检查文件是否成功打开,如果失败则输出错误信息并退出程序
  if (!file_desc)
  {
    // 输出错误信息到标准错误流,提示无法以读写方式打开文件
    fprintf(stderr, "无法以读写方式打开%s", test_file);
    // 以失败状态退出程序
    exit(EXIT_FAILURE);
  }

  // 遍历文件的不同区域进行锁测试
  for (int start_byte = 0; start_byte < 99; start_byte += SIZE_TO_TRY)
  {
    // 设置要测试的锁区域
    struct flock region_to_test;
    region_to_test.l_type = F_WRLCK;     // 设置写锁
    region_to_test.l_whence = SEEK_SET;  // 锁定的起始位置为文件开头
    region_to_test.l_start = start_byte; // 锁定的起始字节
    region_to_test.l_len = SIZE_TO_TRY;  // 锁定的长度
    region_to_test.l_pid = -1;           // 初始化进程ID为-1

    // 打印正在测试的写锁信息
    printf("测试F_WRLCK在从%d到%d的区域\n", start_byte, start_byte + SIZE_TO_TRY);

    // 测试文件上的锁
    // 使用fcntl系统调用检查文件锁的状态
    int res = fcntl(file_desc, F_GETLK, &region_to_test); // F_GETLK用于获取文件锁状态,file_desc是文件描述符,region_to_test是要测试的锁区域
    if (res == -1)                                        // 如果获取锁状态失败
    {
      fprintf(stderr, "F_GETLK失败\n"); // 向标准错误输出错误信息
      exit(EXIT_FAILURE);               // 以失败状态退出程序
    }
    // 如果返回的锁信息中的进程ID不为-1,说明锁会失败
    // 如果region_to_test.l_pid不等于-1,说明有其他进程持有锁或者尝试锁定
    if (region_to_test.l_pid != -1)
    {
      // 打印锁失败的信息,并显示锁的详细信息
      printf("锁会失败.F_GETLK返回: \n");
      show_lock_info(&region_to_test);
    }
    else
    {
      // 如果没有其他进程持有锁或尝试锁定,则当前进程可以成功获取写锁
      printf("F_WRLCK - 锁会成功\n");
    }

    // 重复测试,这次使用共享(读)锁
    region_to_test.l_type = F_RDLCK;     // 设置读锁
    region_to_test.l_whence = SEEK_SET;  // 锁定的起始位置为文件开头
    region_to_test.l_start = start_byte; // 锁定的起始字节
    region_to_test.l_len = SIZE_TO_TRY;  // 锁定的长度
    region_to_test.l_pid = -1;           // 初始化进程ID为-1

    // 打印正在测试的读锁信息
    printf("测试F_RDLCK在从%d到%d的区域\n", start_byte, start_byte + SIZE_TO_TRY);

    // 测试文件上的锁
    // 使用fcntl系统调用获取文件锁状态
    res = fcntl(file_desc, F_GETLK, &region_to_test);
    // 检查fcntl调用是否成功,如果失败则打印错误信息并退出程序
    if (res == -1)
    {
      // 打印"F_GETLK失败"到标准错误输出
      fprintf(stderr, "F_GETLK失败\n");
      // 以失败状态退出程序
      exit(EXIT_FAILURE);
    }
    // 如果返回的锁信息中的进程ID不为-1,说明锁会失败
    // 如果region_to_test.l_pid不等于-1,说明有其他进程持有锁或者存在其他问题,锁尝试会失败
    if (region_to_test.l_pid != -1)
    {
      // 打印锁失败的信息
      printf("锁会失败.F_GETLK返回:\n");
      // 显示锁的详细信息
      show_lock_info(&region_to_test);
    }
    else
    {
      // 如果没有其他进程持有锁,则尝试以共享锁模式(F_RDLCK)获取锁会成功
      printf("F_RDLCK - 锁会成功\n");
    }
  } // 结束遍历

  // 关闭文件描述符
  close(file_desc);
  // 程序成功执行
  exit(EXIT_SUCCESS);
}

// 显示锁信息的函数定义
void show_lock_info(struct flock *to_show)
{
  printf("\tl_type %d, ", to_show->l_type);      // 锁类型
  printf("l_whence %d, ", to_show->l_whence);    // 锁起始位置
  printf("l_start %d, ", (int)to_show->l_start); // 锁起始字节
  printf("l_len %d, ", (int)to_show->l_len);     // 锁长度
  printf("l_pid %d\n", to_show->l_pid);          // 进程ID
}
