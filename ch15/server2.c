/* 引入必要的头文件并设置变量. */

#include <sys/types.h>   // 用于定义系统数据类型
#include <sys/socket.h>  // 用于套接字编程
#include <stdio.h>       // 标准输入输出
#include <netinet/in.h>  // 用于定义网络地址结构体
#include <arpa/inet.h>   // 用于进行IP地址转换
#include <unistd.h>      // 用于系统调用
#include <stdlib.h>      // 标准库函数

int main()
{
    int                server_sockfd, client_sockfd;  // 服务器套接字和客户端套接字
    int                server_len, client_len;        // 地址长度
    struct sockaddr_in server_address;                // 服务器地址结构体
    struct sockaddr_in client_address;                // 客户端地址结构体

    /* 为服务器创建一个未命名的套接字. */

    server_sockfd = socket(AF_INET, SOCK_STREAM, 0);

    /* 为套接字命名(绑定地址和端口). */

    server_address.sin_family      = AF_INET;                 // 使用IPv4
    server_address.sin_addr.s_addr = inet_addr("127.0.0.1");  // 本地回环地址
    server_address.sin_port        = 9734;                    // 端口号
    server_len                     = sizeof(server_address);  // 地址长度
    bind(server_sockfd, (struct sockaddr *)&server_address, server_len);

    /* 创建连接队列并等待客户端连接. */

    listen(server_sockfd, 5);  // 最大挂起连接数为5
    while (1)
    {
        char ch;

        printf("server waiting\n");  // 打印服务器等待信息

        /* 接受一个连接. */

        client_len    = sizeof(client_address);  // 客户端地址长度
        client_sockfd = accept(server_sockfd,    // 接受客户端连接
                               (struct sockaddr *)&client_address,
                               &client_len);

        /* 现在可以通过client_sockfd与客户端进行读写操作. */

        read(client_sockfd, &ch, 1);   // 从客户端读取一个字符
        ch++;                          // 将读取的字符加1
        write(client_sockfd, &ch, 1);  // 将结果写回客户端
        close(client_sockfd);          // 关闭客户端套接字
    }
}
