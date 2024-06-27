/*  server5.c 文件内容注释如下: */

#include <sys/types.h>   // 包含系统类型定义
#include <sys/socket.h>  // 包含套接字相关定义
#include <stdio.h>       // 包含标准输入输出函数
#include <netinet/in.h>  // 包含网络地址结构定义
#include <sys/time.h>    // 包含时间结构定义,用于select
#include <sys/ioctl.h>   // 包含IO控制函数定义
#include <unistd.h>      // 包含系统调用函数定义
#include <stdlib.h>      // 包含标准库函数定义

int main()
{
    int                server_sockfd, client_sockfd;  // 服务器套接字和客户端套接字
    int                server_len, client_len;        // 地址长度
    struct sockaddr_in server_address;                // 服务器地址结构
    struct sockaddr_in client_address;                // 客户端地址结构
    int                result;                        // select函数的返回值
    fd_set             readfds, testfds;              // 文件描述符集合,用于select监控

    /* 创建并命名服务器的套接字 */
    server_sockfd = socket(AF_INET, SOCK_STREAM, 0);

    /* 初始化服务器地址结构 */
    // 设置服务器地址结构体,使用IPv4协议族
    server_address.sin_family = AF_INET;
    // 将服务器的IP地址设置为任意,允许来自任何IP的连接
    server_address.sin_addr.s_addr = htonl(INADDR_ANY);
    // 设置服务器监听的端口号为9734
    server_address.sin_port = htons(9734);
    // 获取服务器地址结构体的大小
    server_len = sizeof(server_address);

    /* 绑定套接字到本地地址和端口 */
    bind(server_sockfd, (struct sockaddr *)&server_address, server_len);

    /* 创建连接队列,并初始化readfds以监控来自server_sockfd的输入 */
    listen(server_sockfd, 5);

    /* 清空文件描述符集合 */
    FD_ZERO(&readfds);
    /* 将server_sockfd添加到文件描述符集合中 */
    FD_SET(server_sockfd, &readfds);

    /* 等待客户端连接和请求 */
    while (1)
    {
        char ch;     // 用于读取和写入的数据字符
        int  fd;     // 当前检查的文件描述符
        int  nread;  // 读取的字节数

        /* 复制readfds到testfds,用于select调用 */
        testfds = readfds;

        /* 等待活动发生,没有设置超时 */
        printf("server waiting\n");
        // 使用select函数来监视文件描述符的变化
        // FD_SETSIZE定义了文件描述符的最大数量
        // &testfds是一个指向fd_set类型的指针,用于存储我们关心的文件描述符集合
        // 接下来的两个参数是空指针,表示我们对写和异常文件描述符不感兴趣
        // 最后一个参数是空指针,表示我们没有超时时间,select会一直阻塞直到有事件发生
        result = select(FD_SETSIZE, &testfds, (fd_set *)0, (fd_set *)0, (struct timeval *)0);

        /* 如果select返回值小于1,则出错并退出 */
        if (result < 1)
        {
            perror("server5");
            exit(1);
        }

        /* 检查哪个文件描述符有活动 */
        for (fd = 0; fd < FD_SETSIZE; fd++)
        {
            if (FD_ISSET(fd, &testfds))
            {
                /* 如果是server_sockfd有活动,则是新的连接请求 */
                if (fd == server_sockfd)
                {
                    // 接受客户端连接请求,并获取客户端套接字的文件描述符
                    client_len = sizeof(client_address);
                    // 等待客户端连接请求,当有客户端连接时,返回一个新的套接字描述符client_sockfd
                    // server_sockfd 是监听套接字,用于监听传入的连接请求
                    // (struct sockaddr *)&client_address 是一个指向结构体的指针,用于存储客户端地址信息
                    // &client_len 是一个指向整数的指针,用于存储client_address结构体的大小
                    client_sockfd = accept(server_sockfd,
                                           (struct sockaddr *)&client_address,
                                           &client_len);

                    // 将新的客户端套接字添加到文件描述符集合中,以便在后续的轮询中能够检测到该套接字上的可读事件
                    FD_SET(client_sockfd, &readfds);

                    // 打印信息,显示已添加的客户端套接字的文件描述符
                    printf("adding client on fd %d\n", client_sockfd);
                }
                /* 如果不是server_sockfd,则是客户端活动 */
                else
                {
                    /* 获取可读字节数 */
                    // 使用ioctl函数来获取设备文件描述符fd的读缓冲区中可用的字节数
                    // FIONREAD宏用于查询读缓冲区的字节数
                    // nread是一个整数变量,用于存储可用的字节数
                    ioctl(fd, FIONREAD, &nread);

                    /* 如果没有数据,表示客户端已关闭连接 */
                    if (nread == 0)
                    {
                        close(fd);
                        /* 从文件描述符集合中移除该套接字 */
                        // 清除文件描述符fd在readfds集合中的位,表示不再监听这个文件描述符的读事件
                        FD_CLR(fd, &readfds);
                        // 打印信息,表明正在移除文件描述符为fd的客户端连接
                        printf("removing client on fd %d\n", fd);
                    }
                    /* 如果有数据,则处理客户端请求 */
                    else
                    {
                        // 从文件描述符 fd 中读取一个字符到变量 ch
                        read(fd, &ch, 1);
                        // 等待 5 秒
                        sleep(5);
                        // 打印服务客户端的文件描述符
                        printf("serving client on fd %d\n", fd);
                        // 将 ch 的值增加 1
                        ch++;
                        // 将 ch 的新值写回到文件描述符 fd
                        write(fd, &ch, 1);
                    }
                }
            }
        }
    }
}
