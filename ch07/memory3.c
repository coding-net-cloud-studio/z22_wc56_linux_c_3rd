// 引入unistd.h头文件,提供对POSIX操作系统API的访问,如read(), write()等系统调用
#include <unistd.h>
// 引入stdlib.h头文件,提供一些通用的函数,如malloc(), free(), exit()等
#include <stdlib.h>
// 引入stdio.h头文件,提供标准输入输出功能,如printf(), scanf()等
#include <stdio.h>

// 定义一个常量ONE_K,表示1KB的大小
#define ONE_K (1024)

int main()
{
    // 声明一个字符指针变量some_memory
    char *some_memory;
    // 定义一个整型变量size_to_allocate,初始化为1KB
    int size_to_allocate = ONE_K;
    // 定义两个整型变量megs_obtained和ks_obtained,用于记录分配的内存大小
    int megs_obtained = 0;
    int ks_obtained   = 0;

    // 无限循环,持续分配内存
    while (1)
    {
        // 内层循环,每次分配1KB内存,直到分配了1024KB(即1MB)
        for (ks_obtained = 0; ks_obtained < 1024; ks_obtained++)
        {
            // 使用malloc函数动态分配size_to_allocate大小的内存
            some_memory = (char *)malloc(size_to_allocate);
            // 如果分配失败,返回错误并退出程序
            if (some_memory == NULL)
                exit(EXIT_FAILURE);
            // 在分配的内存中写入"Hello World"字符串
            sprintf(some_memory, "Hello World");
            // 注意:这里没有释放分配的内存,会导致内存泄漏
        }
        // 每分配完1MB内存,megs_obtained自增1
        megs_obtained++;
        // 打印当前已分配的内存大小(以MB为单位)
        printf("Now allocated %d Megabytes\n", megs_obtained);
    }
    // 程序实际上不会执行到这里,因为前面是无限循环
    exit(EXIT_SUCCESS);
}
