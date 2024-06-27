/* 引入必要的头文件并设置变量. */

#include <sys/types.h>   // 定义系统数据类型
#include <sys/socket.h>  // 包含套接字函数库
#include <stdio.h>       // 标准输入输出
#include <sys/un.h>      // UNIX域套接字结构体
#include <unistd.h>      // 包含unistd函数库
#include <stdlib.h>      // 包含stdlib函数库

int main()
{
    int                server_sockfd, client_sockfd;  // 服务器套接字和客户端套接字的描述符
    int                server_len, client_len;        // 地址长度
    struct sockaddr_un server_address;                // 服务器地址结构体
    struct sockaddr_un client_address;                // 客户端地址结构体

    /* 删除任何旧的套接字并为服务器创建一个未命名的套接字. */

    unlink("server_socket");                          // 如果存在同名套接字文件则删除
    server_sockfd = socket(AF_UNIX, SOCK_STREAM, 0);  // 创建一个UNIX域的流式套接字

    /* 为套接字命名. */

    server_address.sun_family = AF_UNIX;                                  // 设置地址族为UNIX域
    strcpy(server_address.sun_path, "server_socket");                     // 设置套接字的路径名
    server_len = sizeof(server_address);                                  // 计算服务器地址的长度
    bind(server_sockfd, (struct sockaddr *)&server_address, server_len);  // 将套接字与地址绑定

    /* 创建连接队列并等待客户端. */

    listen(server_sockfd, 5);  // 监听套接字,最大挂起连接数为5
    while (1)
    {
        char ch;

        printf("server waiting\n");  // 打印服务器等待信息

        /* 接受一个连接. */

        client_len = sizeof(client_address);  // 计算客户端地址长度

        // 接受客户端连接请求,返回一个新的套接字描述符client_sockfd
        // server_sockfd是监听套接字,用于等待客户端的连接请求
        // (struct sockaddr *)&client_address用于存储客户端地址信息
        // &client_len用于传入客户端地址结构体的大小,函数调用后返回实际大小
        client_sockfd = accept(server_sockfd, (struct sockaddr *)&client_address, &client_len);  // 接受客户端连接请求

        /* 现在我们可以通过client_sockfd读取/写入客户端. */

        read(client_sockfd, &ch, 1);   // 从客户端读取一个字符
        ch++;                          // 将读取的字符加1
        write(client_sockfd, &ch, 1);  // 将结果写回客户端
        close(client_sockfd);          // 关闭客户端套接字
    }
}
