// 引入unistd.h头文件,提供对POSIX操作系统API的访问,如read(), write()等系统调用
#include <unistd.h>
// 引入stdlib.h头文件,提供通用的函数,如malloc(), free(), exit()等
#include <stdlib.h>
// 引入stdio.h头文件,提供标准输入输出功能,如printf(), scanf()等
#include <stdio.h>

int main()
{
    // 分配一个指向空地址的指针
    char *some_memory = (char *)0;

    // 试图从空地址读取内存,这将导致未定义行为
    printf("试图从空地址读取内存: %s\n", some_memory);

    // 试图向空地址写入数据,这同样会导致未定义行为
    sprintf(some_memory, "试图向空地址写入数据\n");

    // 程序成功退出
    exit(EXIT_SUCCESS);
}
