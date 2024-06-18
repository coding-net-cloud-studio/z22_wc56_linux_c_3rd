// 引入unistd.h头文件,提供对POSIX操作系统API的访问,如read(), write()等系统调用
#include <unistd.h>
// 引入stdlib.h头文件,提供通用的函数,如malloc(), free(), exit()等
#include <stdlib.h>
// 引入stdio.h头文件,提供标准输入输出功能,如printf(), scanf()等
#include <stdio.h>
// 定义一个兆字节的大小
#define A_MEGABYTE (1024 * 1024)
// 定义物理内存的大小,可以根据需要调整这个数值
#define PHY_MEM_MEGS 128

int main()
{
  // 分配内存的指针
  char *some_memory;
  // 定义每次尝试分配的内存大小为一个兆字节
  size_t size_to_allocate = A_MEGABYTE;
  // 用于记录成功分配的内存大小(以兆字节计)
  int megs_obtained = 0;

  // 当分配的内存小于设定值的两倍时,继续循环
  while (megs_obtained < (PHY_MEM_MEGS * 2))
  {
    // 尝试分配指定大小的内存
    some_memory = (char *)malloc(size_to_allocate);
    // 如果分配成功
    if (some_memory != NULL)
    {
      // 增加已分配内存的计数
      megs_obtained++;
      // 在分配的内存中写入字符串 "Hello World"
      sprintf(some_memory, "Hello World");
      // 打印当前分配的内存内容以及已分配的内存大小
      printf("%s - 现在分配了 %d 兆字节\n", some_memory, megs_obtained);
    }
    else
    {
      // 如果分配失败,退出程序并返回失败状态
      exit(EXIT_FAILURE);
    }
  }
  // 程序正常结束,返回成功状态
  exit(EXIT_SUCCESS);
}
