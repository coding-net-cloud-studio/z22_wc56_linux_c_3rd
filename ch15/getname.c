/* 引入必要的头文件并声明变量 */
#include <netinet/in.h>  // 包含网络地址转换相关定义
#include <arpa/inet.h>   // 包含inet_ntoa函数定义
#include <unistd.h>      // 包含gethostname函数定义
#include <netdb.h>       // 包含gethostbyname函数定义
#include <stdio.h>       // 包含标准输入输出函数定义
#include <stdlib.h>      // 包含exit函数定义

int main(int argc, char *argv[])  // 主函数,接收命令行参数
{
    char           *host, **names, **addrs;  // 声明指针变量host, names, addrs
    struct hostent *hostinfo;                // 声明指向hostent结构体的指针

    /* 如果没有提供命令行参数,则默认查询本机主机名;否则使用命令行参数 */
    if (argc == 1)
    {
        char myname[256];          // 定义字符数组存储主机名
        gethostname(myname, 255);  // 获取本机主机名
        host = myname;             // 将获取的主机名赋值给host
    }
    else
        host = argv[1];  // 将命令行参数作为host

    /* 调用gethostbyname函数获取主机信息,如果未找到信息则报告错误并退出 */
    hostinfo = gethostbyname(host);
    if (!hostinfo)
    {
        fprintf(stderr, "cannot get info for host: %s\n", host);  // 输出错误信息到标准错误流
        exit(1);                                                  // 以非零状态码退出程序
    }

    /* 显示主机名和别名 */
    printf("results for host %s:\n", host);  // 输出主机名
    printf("Name: %s\n", hostinfo->h_name);  // 输出主机正式名称
    printf("Aliases:");                      // 输出别名提示
    names = hostinfo->h_aliases;             // 获取别名列表
    while (*names)                           // 遍历别名列表
    {
        printf(" %s", *names);  // 输出别名
        names++;                // 指向下一个别名
    }
    printf("\n");  // 输出换行符

    /* 如果主机不是IP主机,则警告并退出 */
    if (hostinfo->h_addrtype != AF_INET)
    {
        fprintf(stderr, "not an IP host!\n");  // 输出错误信息到标准错误流
        exit(1);                               // 以非零状态码退出程序
    }

    /* 否则,显示IP地址 */
    addrs = hostinfo->h_addr_list;  // 获取地址列表
    while (*addrs)                  // 遍历地址列表
    {
        printf(" %s", inet_ntoa(*(struct in_addr *)*addrs));  // 将网络地址转换为可读字符串并输出
        addrs++;                                              // 指向下一个地址
    }
    printf("\n");  // 输出换行符
    exit(0);       // 正常退出程序
}
