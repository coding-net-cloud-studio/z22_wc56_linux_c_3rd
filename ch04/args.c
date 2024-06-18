// 引入标准输入输出库
#include <stdio.h>
// 引入通用工具库
#include <stdlib.h>

// 程序入口函数
int main(int argc, char *argv[])
{
    // 初始化参数变量
    int arg;

    // 遍历所有传递给程序的参数
    for (arg = 0; arg < argc; arg++)
    {
        // 如果参数以'-'开头,则认为是选项
        if (argv[arg][0] == '-')
        {
            // 打印出选项内容(去掉开头的'-')
            printf("选项: %s\n", argv[arg] + 1);
        }
        else
        {
            // 如果参数不是选项,则打印出参数的序号和内容
            printf("参数 %d: %s\n", arg, argv[arg]);
        }
    }
    // 程序正常退出
    exit(0);
}
