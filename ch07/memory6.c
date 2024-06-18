// 引入标准库函数,用于动态内存分配和释放
#include <stdlib.h>
// 引入标准输入输出库函数,用于程序的输入输出操作
#include <stdio.h>

// 定义一个常量 ONE_K,表示1KB的大小
#define ONE_K (1024)

int main()
{
  // 声明一个字符指针变量 some_memory
  char *some_memory;
  // 初始化退出码为失败
  int exit_code = EXIT_FAILURE;

  // 使用 malloc 函数动态分配 1KB 的内存空间给 some_memory 指针
  some_memory = (char *)malloc(ONE_K);
  // 检查分配的内存是否为空(即是否分配成功)
  if (some_memory != NULL)
  {
    // 如果内存分配成功,则释放这块内存
    free(some_memory);
    // 打印内存分配并释放成功的消息
    printf("内存已分配并已再次释放\n");
    // 将退出码设置为成功
    exit_code = EXIT_SUCCESS;
  }
  // 退出程序,并返回退出码
  exit(exit_code);
}
