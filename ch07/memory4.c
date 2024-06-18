#include <unistd.h>  // 引入unistd头文件,提供对POSIX操作系统API的访问
#include <stdlib.h>  // 引入stdlib头文件,提供内存分配和退出功能

#define ONE_K (1024)  // 定义一个宏,表示1KB的大小

int main()
{
    char *some_memory;  // 声明一个字符指针,用于存储分配的内存地址
    char *scan_ptr;     // 声明另一个字符指针,用于遍历分配的内存

    some_memory = (char *)malloc(ONE_K);  // 动态分配1KB的内存空间
    if (some_memory == NULL)              // 检查内存分配是否成功
        exit(EXIT_FAILURE);               // 如果分配失败,使用exit函数退出程序并返回失败状态码

    scan_ptr = some_memory;  // 将scan_ptr指向分配的内存起始位置
    while (1)                // 无限循环
    {
        *scan_ptr = '\0';  // 将当前指针位置的值设置为字符串结束符'\0'
        scan_ptr++;        // 指针向后移动一位
    }

    exit(EXIT_SUCCESS);  // 这行代码永远不会被执行,因为前面有一个无限循环
}
