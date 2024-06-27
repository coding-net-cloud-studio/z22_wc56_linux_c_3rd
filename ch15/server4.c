/* 程序 server4.c 的注释如下:

    该程序与我们的上一个服务器程序类似,显著的区别是增加了对 signal.h 头文件的包含.
    变量和创建及命名套接字的过程相同.
*/

#include <sys/types.h>   // 包含系统类型定义
#include <sys/socket.h>  // 包含套接字相关函数和类型定义
#include <stdio.h>       // 包含标准输入输出函数
#include <netinet/in.h>  // 包含 Internet 地址结构体定义
#include <signal.h>      // 包含信号处理相关定义
#include <unistd.h>      // 包含进程控制相关函数
#include <stdlib.h>      // 包含标准库函数

int main()
{
    int                server_sockfd, client_sockfd;  // 服务器套接字和客户端套接字的描述符
    int                server_len, client_len;        // 地址长度
    struct sockaddr_in server_address;                // 服务器地址结构体
    struct sockaddr_in client_address;                // 客户端地址结构体

    // 创建套接字,AF_INET 表示使用 IPv4,SOCK_STREAM 表示使用 TCP
    server_sockfd = socket(AF_INET, SOCK_STREAM, 0);

    // 初始化服务器地址结构体
    server_address.sin_family      = AF_INET;                 // 地址族为 IPv4
    server_address.sin_addr.s_addr = htonl(INADDR_ANY);       // 监听任意 IP 地址
    server_address.sin_port        = htons(9734);             // 端口号设置为 9734
    server_len                     = sizeof(server_address);  // 计算地址长度
    // 将套接字与地址绑定
    bind(server_sockfd, (struct sockaddr *)&server_address, server_len);

    // 创建连接队列,忽略子进程退出细节,等待客户端连接
    listen(server_sockfd, 5);

    // 设置信号处理,忽略 SIGCHLD 信号,防止僵尸进程产生
    signal(SIGCHLD, SIG_IGN);

    while (1)
    {
        char ch;

        // 打印服务器等待信息
        printf("server waiting\n");

        // 接受客户端连接
        client_len    = sizeof(client_address);
        client_sockfd = accept(server_sockfd,
                               (struct sockaddr *)&client_address,
                               &client_len);

        // 通过 fork 创建一个子进程来处理这个客户端的连接
        if (fork() == 0)
        {
            // 如果是子进程,现在可以读写客户端套接字了
            read(client_sockfd, &ch, 1);
            sleep(5);                      // 为了演示,延迟 5 秒
            ch++;                          // 将读取到的字符加 1
            write(client_sockfd, &ch, 1);  // 将结果写回客户端
            close(client_sockfd);          // 关闭客户端套接字
            exit(0);                       // 子进程结束
        }
        else
        {
            // 如果是父进程,关闭客户端套接字,不再参与处理
            close(client_sockfd);
        }
    }
}
