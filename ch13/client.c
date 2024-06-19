// Here's the client, client.c. The first part of this program opens the server FIFO,
// if it already exists, as a file. It then gets its own process ID, which forms some
// of the data that will be sent to the server. The client FIFO is created, ready for
// the next section.

// 这是客户端程序 client.c.该程序的第一部分打开服务器
// FIFO(如果它已经存在)作为一个文件.然后它获取自己的进程 ID,
// 这构成了一部分将要发送给服务器的数据.
// 接下来,创建客户端 FIFO,为下一部分做准备.

// 客户端程序 client.c 的开始部分
// 引入客户端头文件
#include "client.h"
// 引入标准库中的字符处理函数头文件
#include <ctype.h>

int main()
{
    // 定义服务器和客户端 FIFO 的文件描述符
    int server_fifo_fd, client_fifo_fd;
    // 定义要发送给服务器的数据结构
    struct data_to_pass_st my_data;
    // 定义发送数据的次数
    int times_to_send;
    // 定义客户端 FIFO 的名称缓冲区
    char client_fifo[256];

    // 打开服务器 FIFO,如果它已经存在的话
    server_fifo_fd = open(SERVER_FIFO_NAME, O_WRONLY);
    // 如果打开失败,打印错误信息并退出程序
    if (server_fifo_fd == -1)
    {
        fprintf(stderr, "对不起,没有找到服务器\n");
        exit(EXIT_FAILURE);
    }

    // 获取当前进程的 PID,用于构造将要发送的数据
    my_data.client_pid = getpid();
    // 根据客户端 PID 构造客户端 FIFO 的名称
    sprintf(client_fifo, CLIENT_FIFO_NAME, my_data.client_pid);
    // 创建客户端 FIFO
    if (mkfifo(client_fifo, 0777) == -1)
    {
        // 如果创建失败,打印错误信息并退出程序
        fprintf(stderr, "对不起,无法创建 %s\n", client_fifo);
        exit(EXIT_FAILURE);
    }

    // For each of the five loops, the client data is sent to the server.
    // Then the client FIFO is opened (read-only, blocking mode) and the data read back.
    // Finally, the server FIFO is closed and the client FIFO removed from memory.

    // 对于这五个循环中的每一个,客户端数据都会被发送到服务器.
    // 然后以只读和阻塞模式打开客户端 FIFO,并读取回数据.
    // 最后,关闭服务器 FIFO,并从内存中移除客户端 FIFO.

    // 循环五次发送数据到服务器
    for (times_to_send = 0; times_to_send < 5; times_to_send++)
    {
        // 构造要发送的数据
        sprintf(my_data.some_data, "来自 %d 的问候", my_data.client_pid);
        // 打印发送的数据
        printf("%d 发送了 %s,", my_data.client_pid, my_data.some_data);
        // 将数据写入服务器 FIFO
        write(server_fifo_fd, &my_data, sizeof(my_data));
        // 以只读阻塞模式打开客户端 FIFO
        client_fifo_fd = open(client_fifo, O_RDONLY);
        // 如果成功打开,读取从服务器返回的数据
        if (client_fifo_fd != -1)
        {
            // 读取数据并检查是否成功
            // 从客户端管道文件描述符读取数据
            // 参数 client_fifo_fd 是打开的客户端管道的文件描述符
            // 参数 &my_data 是将要存储读取数据的变量的地址
            // 参数 sizeof(my_data) 指定要读取的数据的大小
            if (read(client_fifo_fd, &my_data, sizeof(my_data)) > 0)
            {
                // 打印接收到的数据
                printf("接收到:%s\n", my_data.some_data);
            }
            // 关闭客户端 FIFO 文件描述符
            close(client_fifo_fd);
        }
    }
    // 关闭服务器 FIFO 文件描述符
    close(server_fifo_fd);
    // 从系统中移除客户端 FIFO
    unlink(client_fifo);
    // 程序正常退出
    exit(EXIT_SUCCESS);
}
