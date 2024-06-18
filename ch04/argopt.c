// 引入标准输入输出库
#include <stdio.h>
// 引入unistd库,用于系统调用
#include <unistd.h>
// 引入stdlib库,用于动态内存管理
#include <stdlib.h>

// 定义主函数,接收命令行参数
int main(int argc, char *argv[])
{
    // 初始化getopt函数的返回值
    int opt;

    // 使用while循环处理命令行选项
    while ((opt = getopt(argc, argv, ":if:lr")) != -1)
    {
        // 根据getopt返回的选项字符进行处理
        switch (opt)
        {
        // 处理'-i', '-l', '-r'选项
        case 'i':
        case 'l':
        case 'r':
            printf("option: %c\n", opt);
            break;
        // 处理'-f'选项,并获取其参数值
        case 'f':
            printf("filename: %s\n", optarg);
            break;
        // 当选项需要参数但未提供时,打印提示信息
        case ':':
            printf("option needs a value\n");
            break;
        // 当遇到未知选项时,打印提示信息
        case '?':
            printf("unknown option: %c\n", optopt);
            break;
        }
    }

    // 处理非选项参数
    for (; optind < argc; optind++)
        printf("argument: %s\n", argv[optind]);

    // 程序正常退出
    exit(0);
}
