// 引入标准输入输出库
#include <stdio.h>
// 引入unistd库,用于获取命令行参数
#include <unistd.h>
// 引入stdlib库,用于程序退出
#include <stdlib.h>

// 定义_GNU_SOURCE宏,以便使用GNU扩展功能
#define _GNU_SOURCE
// 引入getopt库,用于解析命令行选项
#include <getopt.h>

// longopt.c 示例程序,用于解析命令行选项和参数

int main(int argc, char *argv[])
{
    // 定义一个变量来存储当前解析的选项
    int opt;

    // 定义一个结构体数组,用于描述长选项及其相关信息
    struct option longopts[] = {
        {"initialize", 0, NULL, 'i'},  // 初始化选项,无参数
        {"file", 1, NULL, 'f'},        // 文件选项,需要一个参数
        {"list", 0, NULL, 'l'},        // 列表选项,无参数
        {"restart", 0, NULL, 'r'},     // 重启选项,无参数
        {0, 0, 0, 0}};                 // 结束标记

    // 使用getopt_long函数循环解析命令行选项
    while ((opt = getopt_long(argc, argv, ":if:lr", longopts, NULL)) != -1)
    {
        // 根据解析出的选项执行相应操作
        switch (opt)
        {
        case 'i':  // 初始化选项
        case 'l':  // 列表选项
        case 'r':  // 重启选项
            printf("option: %c\n", opt);
            break;
        case 'f':  // 文件选项
            printf("filename: %s\n", optarg);
            break;
        case ':':  // 缺少参数的选项
            printf("option needs a value\n");
            break;
        case '?':  // 未知选项
            printf("unknown option: %c\n", optopt);
            break;
        }
    }

    // 循环处理剩余的参数
    for (; optind < argc; optind++)
        printf("argument: %s\n", argv[optind]);

    // 程序正常退出
    exit(0);
}
