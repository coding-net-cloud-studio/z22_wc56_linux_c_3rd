/*  包含必要的头文件并设置变量.  */

#include <sys/types.h>   // 用于定义系统数据类型
#include <sys/socket.h>  // 包含套接字相关函数和类型
#include <stdio.h>       // 标准输入输出
#include <sys/un.h>      // 包含UNIX域套接字相关定义
#include <unistd.h>      // 包含进程控制相关函数
#include <stdlib.h>      // 包含内存分配和释放函数

int main()
{
    int                sockfd;    // 客户端套接字描述符
    int                len;       // 地址长度
    struct sockaddr_un address;   // UNIX域套接字地址结构体
    int                result;    // 连接结果
    char               ch = 'A';  // 要发送给服务器的字符

    /*  为客户端创建一个套接字.  */

    sockfd = socket(AF_UNIX, SOCK_STREAM, 0);

    /*  按照与服务器约定的名称命名套接字.  */

    address.sun_family = AF_UNIX;               // 设置地址族为UNIX域
    strcpy(address.sun_path, "server_socket");  // 设置套接字路径名
    len = sizeof(address);                      // 计算地址结构体的大小

    /*  现在将我们的套接字连接到服务器的套接字.  */

    result = connect(sockfd, (struct sockaddr *)&address, len);

    if (result == -1)
    {
        perror("oops: client1");  // 如果连接失败,打印错误信息
        exit(1);                  // 并退出程序
    }

    /*  现在我们可以通过sockfd进行读写操作.  */

    write(sockfd, &ch, 1);                  // 向服务器发送字符
    read(sockfd, &ch, 1);                   // 从服务器读取字符
    printf("char from server = %c\n", ch);  // 打印从服务器接收的字符
    close(sockfd);                          // 关闭套接字
    exit(0);                                // 正常退出程序
}
