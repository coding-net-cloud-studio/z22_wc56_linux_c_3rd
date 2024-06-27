/*  Start with the usual includes and declarations.  */

// 导入必要的头文件以支持网络编程和系统调用
#include <sys/socket.h>  // 包含套接字函数库
#include <netinet/in.h>  // 包含互联网地址结构体和相关定义
#include <netdb.h>       // 包含域名解析函数库
#include <stdio.h>       // 包含标准输入输出函数库
#include <unistd.h>      // 包含进程控制函数库
#include <stdlib.h>      // 包含内存分配和释放函数库

int main(int argc, char *argv[])
{
    // 定义字符指针变量host,用于存储目标主机名
    char *host;
    // 定义整型变量sockfd,用于存储套接字文件描述符
    int sockfd;
    // 定义整型变量len和result,分别用于存储地址长度和操作结果
    int len, result;
    // 定义结构体变量address,用于存储目标主机的地址信息
    struct sockaddr_in address;
    // 定义结构体指针变量hostinfo,用于存储目标主机的信息
    struct hostent *hostinfo;
    // 定义结构体指针变量servinfo,用于存储服务信息
    struct servent *servinfo;
    // 定义字符数组buffer,用于存储接收的数据
    char buffer[128];

    // 如果命令行参数只有一个,即没有指定主机名,则默认使用"localhost"
    if (argc == 1)
        host = "localhost";
    // 否则,使用命令行参数中指定的主机名
    else
        host = argv[1];

    /* 获取主机地址,如果没有找到则报告错误. */

    // 使用gethostbyname函数尝试获取主机的地址信息
    hostinfo = gethostbyname(host);

    // 如果gethostbyname返回的是空指针,表示没有找到主机地址
    if (!hostinfo)
    {
        // 向标准错误输出流打印错误信息,并退出程序,返回值为1表示程序异常退出
        fprintf(stderr, "no host: %s\n", host);
        exit(1);
    }

    /* 检查主机上是否存在日间服务 */
    servinfo = getservbyname("daytime", "udp");
    if (!servinfo)
    {
        fprintf(stderr, "没有日间服务\n");  // 输出错误信息到标准错误流
        exit(1);                            // 如果服务不存在,则退出程序
    }
    printf("日间端口是 %d\n", ntohs(servinfo->s_port));  // 打印日间服务的端口号

    /* 创建一个UDP套接字 */
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    /* 构建用于sendto/recvfrom的地址结构 */
    address.sin_family = AF_INET;                                    // 设置地址族为IPv4
    address.sin_port   = servinfo->s_port;                           // 设置端口为日间服务的端口
    address.sin_addr   = *(struct in_addr *)*hostinfo->h_addr_list;  // 设置IP地址为主机信息中的第一个IP地址
    len                = sizeof(address);                            // 计算地址结构的大小

    /* 向服务器发送请求并接收响应 */
    // 使用sendto函数发送数据包
    // sockfd: 已创建的UDP套接字文件描述符
    // buffer: 指向要发送数据的指针
    // 1: 要发送的数据长度(字节)
    // 0: 标志位,此处使用0,表示普通数据发送
    // (struct sockaddr *)&address: 目标地址结构体的指针
    // len: 目标地址结构体的大小(字节)
    result = sendto(sockfd, buffer, 1, 0, (struct sockaddr *)&address, len);  // 发送一个字节的数据
    // 接收数据包
    // sockfd: 已经创建的UDP套接字文件描述符
    // buffer: 用于存储接收到的数据的缓冲区
    // sizeof(buffer): 缓冲区的大小
    // 0: 表示接收数据时不设置任何选项
    // (struct sockaddr *)&address: 用于接收数据包的来源地址信息
    // &len: 是一个指向地址结构大小的变量的指针,用于接收实际地址长度
    result         = recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr *)&address, &len);  // 接收服务器的响应
    buffer[result] = '\0';                                                                            // 确保字符串以空字符结尾
    printf("读取了 %d 字节: %s", result, buffer);                                                     // 打印接收到的数据

    close(sockfd);  // 关闭套接字
    exit(0);        // 正常退出程序
}
