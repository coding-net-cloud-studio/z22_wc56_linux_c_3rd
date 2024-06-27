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
    int                sockfd;    // 客户端套接字描述符
    int                len;       // 地址长度
    struct sockaddr_in address;   // 服务器地址结构体
    int                result;    // 连接结果
    char               ch = 'A';  // 要发送的字符

    /*  为客户端创建一个套接字.  */

    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    /*  为套接字命名,与服务器协商一致.  */

    address.sin_family      = AF_INET;                 // 使用IPv4协议族
    address.sin_addr.s_addr = inet_addr("127.0.0.1");  // 设置服务器IP地址为本地回环地址
    address.sin_port        = htons(9734);             // 设置服务器端口号
    len                     = sizeof(address);         // 计算地址结构体大小

    /*  现在将我们的套接字连接到服务器的套接字.  */

    result = connect(sockfd, (struct sockaddr *)&address, len);

    if (result == -1)
    {
        perror("oops: client3");  // 如果连接失败,打印错误信息
        exit(1);                  // 并退出程序
    }

    /*  现在我们可以通过sockfd进行读写操作.  */

    write(sockfd, &ch, 1);                  // 向服务器发送字符
    read(sockfd, &ch, 1);                   // 从服务器读取字符
    printf("char from server = %c\n", ch);  // 打印从服务器接收到的字符
    close(sockfd);                          // 关闭套接字
    exit(0);                                // 正常退出程序
}
