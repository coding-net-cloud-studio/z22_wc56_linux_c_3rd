// 引入必要的头文件
#include <stdlib.h>  // 用于环境变量操作和内存管理
#include <stdio.h>   // 用于输入输出操作

// 引入环境变量声明
extern char **environ;

int main()
{
    // 获取当前进程的环境变量列表
    char **env = environ;

    // 遍历环境变量列表,直到遇到空指针(表示列表结束)
    while (*env)
    {
        // 打印当前环境变量的值
        printf("%s\n", *env);
        // 移动到下一个环境变量
        env++;
    }
    // 程序正常退出
    exit(0);
}
