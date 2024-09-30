/* 1  The first few lines after the declaration of main ensure that the program, environ.c,
has been called correctly. */

/* 1 主函数声明后的前几行确保程序 environ.c 被正确调用. */

#include <stdlib.h>  // 用于动态内存管理
#include <stdio.h>   // 用于输入输出功能
#include <string.h>  // 用于字符串处理

// 主函数,接收命令行参数并处理环境变量操作
int main(int argc, char *argv[])
{
    // 定义一个指向字符的指针变量 var,用于存储环境变量名
    char *var;
    // 定义一个指向字符的指针变量 value,用于存储环境变量的值
    char *value;

    // 检查命令行参数数量,必须是1个或3个
    if (argc == 1 || argc > 3)
    {
        // 如果参数数量不正确,输出提示信息并退出程序
        fprintf(stderr, "用法:environ var [value]\n");
        exit(1);
    }

    // 获取环境变量的值
    // 从命令行参数中获取环境变量名
    var = argv[1];
    // 获取指定环境变量的值
    value = getenv(var);

    // 输出环境变量的当前值,如果存在的话
    if (value)
        printf("变量 %s 的值为 %s\n", var, value);
    else
        printf("变量 %s 没有值\n", var);

    // 如果有第二个参数,表示要设置环境变量的值
    if (argc == 3)
    {
        char *string;
        value = argv[2];
        // 分配内存并构造 name=value 格式的字符串
        string = malloc(strlen(var) + strlen(value) + 2);
        if (!string)
        {
            // 如果内存分配失败,输出错误信息并退出程序
            fprintf(stderr, "内存不足\n");
            // 程序异常退出
            exit(1);
        }
        // 拼接字符串
        // 将变量var的内容复制到字符串string中
        strcpy(string, var);

        // 在string的末尾追加等号'='
        strcat(string, "=");

        // 在string的末尾追加变量value的内容
        strcat(string, value);

        // 输出即将设置的环境变量字符串
        printf("调用 putenv 函数:%s\n", string);
        // 设置环境变量
        if (putenv(string) != 0)
        {
            // 如果设置失败,输出错误信息并释放内存后退出程序
            fprintf(stderr, "putenv 失败\n");
            // 释放内存
            free(string);
            // 程序异常退出
            exit(1);
        }

        // 再次获取环境变量的值以查看新设置的值
        value = getenv(var);
        // 输出新设置的环境变量值
        if (value)
            printf("变量 %s 的新值为 %s\n", var, value);
        else
            printf("变量 %s 的新值为空?\n", var);
    }
    // 程序正常结束
    exit(0);
}
