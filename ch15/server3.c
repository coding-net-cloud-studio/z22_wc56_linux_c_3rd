/*  包含必要的头文件并设置变量.  */

#include <sys/types.h>   // 包含系统类型定义
#include <sys/socket.h>  // 包含套接字相关函数和类型定义
#include <stdio.h>       // 包含标准输入输出函数
#include <netinet/in.h>  // 包含网络地址结构体定义
#include <arpa/inet.h>   // 包含网络地址转换函数
#include <unistd.h>      // 包含进程控制函数
#include <stdlib.h>      // 包含内存管理函数

int main()
{
    int                server_sockfd, client_sockfd;  // 服务器套接字和客户端套接字的描述符
    int                server_len, client_len;        // 地址长度
    struct sockaddr_in server_address;                // 服务器地址结构体
    struct sockaddr_in client_address;                // 客户端地址结构体

    /*  移除任何旧套接字并为服务器创建一个未命名的套接字.  */

    server_sockfd = socket(AF_INET, SOCK_STREAM, 0);  // 创建一个TCP套接字

    /*  为套接字命名.  */

    server_address.sin_family      = AF_INET;                             // 设置地址族为IPv4
    server_address.sin_addr.s_addr = htonl(INADDR_ANY);                   // 设置IP地址为任意,允许任何IP连接
    server_address.sin_port        = htons(9734);                         // 设置监听端口为9734
    server_len                     = sizeof(server_address);              // 计算服务器地址长度
    bind(server_sockfd, (struct sockaddr *)&server_address, server_len);  // 绑定套接字到本地地址

    /*  创建连接队列并等待客户端.  */

    listen(server_sockfd, 5);  // 开始监听,最大挂起连接数为5
    while (1)
    {
        char ch;

        printf("server waiting\n");  // 打印服务器等待信息

        /*  接受一个连接.  */

        client_len    = sizeof(client_address);  // 计算客户端地址长度
        client_sockfd = accept(server_sockfd,    // 接受客户端连接请求
                               (struct sockaddr *)&client_address,
                               &client_len);

        /*  现在我们可以在client_sockfd上对客户端进行读写操作.  */

        read(client_sockfd, &ch, 1);   // 从客户端读取一个字符
        ch++;                          // 将读取的字符加1
        write(client_sockfd, &ch, 1);  // 将结果写回客户端
        close(client_sockfd);          // 关闭客户端套接字
    }
}
