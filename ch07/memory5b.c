#include <unistd.h>  // 用于进程控制,如sleep()函数
#include <stdlib.h>  // 用于动态内存管理,如malloc()和free()函数
#include <stdio.h>   // 用于输入输出,如printf()函数

int main()
{
    // 尝试从内存地址0读取数据,这是非法的,并可能导致程序崩溃
    char z = *(const char *)0;

    // 打印信息,但由于上一步可能导致的崩溃,这行代码可能不会执行
    printf("I read from location zero\n");  // 注意:字符串应该以双引号结束,这里缺少了一个双引号

    // 正常退出程序
    exit(EXIT_SUCCESS);
}
