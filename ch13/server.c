// 引入客户端头文件,用于定义客户端相关的函数和数据结构
#include "client.h"
// 引入标准库ctype.h,提供字符处理功能
#include <ctype.h>

int main()
{
    // 创建服务器管道并打开它进行读取
    // 声明服务器管道文件描述符和客户端管道文件描述符
    int server_fifo_fd, client_fifo_fd;

    // 声明一个结构体变量,用于在服务器和客户端之间传递数据
    struct data_to_pass_st my_data;

    // 声明一个整型变量,用于存储读取操作的返回值
    int read_res;

    // 声明一个字符数组,用于存储客户端管道的名称
    char client_fifo[256];

    // 声明一个字符指针,用于临时操作字符串
    char *tmp_char_ptr;

    // 创建命名管道,权限设置为0777(可读写执行)
    mkfifo(SERVER_FIFO_NAME, 0777);
    // 以只读模式打开服务器管道
    server_fifo_fd = open(SERVER_FIFO_NAME, O_RDONLY);
    // 如果打开失败,打印错误信息并退出程序
    if (server_fifo_fd == -1)
    {
        fprintf(stderr, "服务器管道创建失败\n");
        exit(EXIT_FAILURE);
    }

    // 等待10秒,以便客户端排队等待演示
    sleep(10);

    // 读取客户端发送的数据
    do
    {
        // 从服务器管道中读取数据
        // 从服务器管道文件描述符读取数据
        // 参数 server_fifo_fd 是打开的服务器管道的文件描述符
        // 参数 &my_data 是将要存储读取数据的变量的地址
        // 参数 sizeof(my_data) 指定要读取的数据的大小
        read_res = read(server_fifo_fd, &my_data, sizeof(my_data));

        // 如果读取成功
        if (read_res > 0)
        {
            // 将接收到的数据中的字符转换为大写
            tmp_char_ptr = my_data.some_data;
            while (*tmp_char_ptr)
            {
                *tmp_char_ptr = toupper(*tmp_char_ptr);
                tmp_char_ptr++;
            }
            // 根据客户端PID构造客户端管道名称
            sprintf(client_fifo, CLIENT_FIFO_NAME, my_data.client_pid);

            // 以只写阻塞模式打开客户端管道
            client_fifo_fd = open(client_fifo, O_WRONLY);
            // 如果打开成功,将处理后的数据写回客户端
            if (client_fifo_fd != -1)
            {
                // 向指定的客户端FIFO管道写入数据
                // client_fifo_fd: 已打开的客户端FIFO管道文件描述符
                // my_data: 要发送的数据结构体指针
                // sizeof(my_data): 要发送数据的字节大小
                write(client_fifo_fd, &my_data, sizeof(my_data));
                // 关闭客户端管道
                close(client_fifo_fd);
            }
        }
    } while (read_res > 0);  // 继续读取直到没有更多数据

    // 关闭服务器管道并删除它
    close(server_fifo_fd);
    unlink(SERVER_FIFO_NAME);

    // 程序正常退出
    exit(EXIT_SUCCESS);
}
