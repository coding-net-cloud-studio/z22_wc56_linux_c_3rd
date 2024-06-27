/*  Start with the usual includes and declarations.  */

// 引入套接字编程所需的头文件
#include <sys/socket.h>
// 引入网络通信相关的头文件
#include <netinet/in.h>
// 引入网络数据库查询相关的头文件
#include <netdb.h>
// 引入标准输入输出相关的头文件
#include <stdio.h>
// 引入进程控制相关的头文件,用于非阻塞操作
#include <unistd.h>
// 引入动态内存分配相关的头文件
#include <stdlib.h>

int main(int argc, char *argv[])
{
    // 定义字符指针变量host,用于存储服务器主机名
    char *host;

    // 定义整型变量sockfd,用于存储套接字的文件描述符
    int sockfd;

    // 定义整型变量len和result,分别用于存储地址长度和操作结果
    int len, result;

    // 定义结构体变量address,用于存储服务器的地址信息
    struct sockaddr_in address;

    // 定义结构体指针变量hostinfo,用于存储通过主机名获取的主机信息
    struct hostent *hostinfo;

    // 定义结构体指针变量servinfo,用于存储通过服务名获取的服务信息
    struct servent *servinfo;

    // 定义字符数组buffer,用于存储接收的数据缓冲区
    char buffer[128];

    // 如果命令行参数只有一个,即没有指定主机名,则默认使用"localhost"
    if (argc == 1)
        host = "localhost";
    else
        // 否则,使用命令行参数中提供的主机名
        host = argv[1];

    /* 使用gethostbyname()函数查找主机的地址信息
     * 如果找不到主机地址信息,报告错误并退出程序 */
    hostinfo = gethostbyname(host);
    if (!hostinfo)
    {
        // 输出错误信息到标准错误流
        fprintf(stderr, "no host: %s\n", host);
        // 退出程序并返回错误码1
        exit(1);
    }

    /* 检查主机上是否存在日间服务 */
    servinfo = getservbyname("daytime", "tcp");
    if (!servinfo)
    {
        fprintf(stderr, "没有日间服务\n");  // 输出错误信息到标准错误流
        exit(1);                            // 如果服务不存在,则退出程序
    }
    printf("日间端口是 %d\n", ntohs(servinfo->s_port));  // 打印日间服务的端口号

    /* 创建一个套接字 */
    sockfd = socket(AF_INET, SOCK_STREAM, 0);  // 创建一个IPv4的TCP套接字

    /* 构建用于连接的地址信息 */
    address.sin_family = AF_INET;                                    // 设置地址族为IPv4
    address.sin_port   = servinfo->s_port;                           // 设置端口号为日间服务的端口
    address.sin_addr   = *(struct in_addr *)*hostinfo->h_addr_list;  // 设置IP地址为主机信息中的第一个IP地址
    len                = sizeof(address);                            // 计算地址结构体的大小

    /* 连接并获得信息 */
    result = connect(sockfd, (struct sockaddr *)&address, len);  // 尝试连接到日间服务
    if (result == -1)
    {
        perror("oops: getdate");  // 如果连接失败,打印错误信息
        exit(1);                  // 并退出程序
    }

    result         = read(sockfd, buffer, sizeof(buffer));  // 从套接字读取数据到缓冲区
    buffer[result] = '\0';                                  // 在读取的数据后添加字符串结束符
    printf("读取 %d 字节: %s", result, buffer);             // 打印读取到的数据

    close(sockfd);  // 关闭套接字
    exit(0);        // 正常退出程序
}
