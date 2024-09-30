// 导入unistd库,用于提供对POSIX操作系统API的访问,如read和write函数
#include <unistd.h>
// 导入stdlib库,用于提供内存管理,随机数生成等功能
#include <stdlib.h>
// 导入stdio库,用于提供标准输入输出功能,如printf和scanf函数
#include <stdio.h>
// 导入string库,用于处理字符串和内存相关的操作
#include <string.h>
// 导入fcntl库,用于提供文件控制和I/O操作的函数
#include <fcntl.h>
// 导入limits库,用于定义各种数据类型的最大值和最小值
#include <limits.h>
// 导入sys/types库,用于定义各种系统数据类型
#include <sys/types.h>
// 导入sys/stat库,用于提供文件状态信息
#include <sys/stat.h>

// 定义FIFO管道的名称,用于进程间通信
#define FIFO_NAME   "/tmp/my_fifo"
// 定义缓冲区大小,使用PIPE_BUF常量,确保数据的一致性
#define BUFFER_SIZE PIPE_BUF
// 定义一个十兆字节的常量,可能用于设置缓冲区大小或其他内存分配
#define TEN_MEG     (1024 * 1024 * 10)

int main()
{
    int  pipe_fd;                  // 管道文件描述符
    int  res;                      // 用于存储系统调用的返回值
    int  open_mode  = O_WRONLY;    // 以只写模式打开管道
    int  bytes_sent = 0;           // 已发送的字节数
    char buffer[BUFFER_SIZE + 1];  // 数据缓冲区

    // 检查管道是否存在,如果不存在则创建
    // 使用access函数检查FIFO_NAME指定的命名管道是否存在
    // 如果存在,access函数返回0;如果不存在,返回-1
    if (access(FIFO_NAME, F_OK) == -1)
    {
        // FIFO_NAME不存在,可能需要创建它或者处理错误情况
        res = mkfifo(FIFO_NAME, 0777);  // 创建命名管道,权限设置为0777
        if (res != 0)
        {
            fprintf(stderr, "无法创建管道 %s\n", FIFO_NAME);
            exit(EXIT_FAILURE);
        }
    }

    // 打印进程ID和打开管道的模式
    // 打印当前进程ID,并说明正在以只写模式打开管道
    printf("进程 %d 以O_WRONLY模式打开管道\n", getpid());
    // 尝试打开名为FIFO_NAME的管道文件,使用变量open_mode指定的模式
    pipe_fd = open(FIFO_NAME, open_mode);
    // 打印当前进程ID和打开管道的结果(文件描述符)
    printf("进程 %d 打开结果 %d\n", getpid(), pipe_fd);

    // 如果管道成功打开,则开始写入数据
    if (pipe_fd != -1)
    {
        // 循环写入数据直到达到10MB
        // 向管道写入数据,直到发送了指定数量的字节
        while (bytes_sent < TEN_MEG)
        {
            // 尝试向管道写入数据
            // 将缓冲区中的数据写入管道,BUFFER_SIZE定义了要写入的字节数
            // write函数返回实际写入的字节数,如果返回值小于BUFFER_SIZE,可能表示发生了错误或管道已满
            res = write(pipe_fd, buffer, BUFFER_SIZE);

            // 如果写入失败,打印错误信息并退出程序
            if (res == -1)
            {
                fprintf(stderr, "管道写入错误\n");
                exit(EXIT_FAILURE);
            }
            // 更新已发送的字节数
            bytes_sent += res;
        }

        (void)close(pipe_fd);  // 关闭管道文件描述符
    }
    else
    {
        exit(EXIT_FAILURE);  // 如果管道打开失败,则退出程序
    }

    // 打印进程结束信息
    printf("进程 %d 完成\n", getpid());
    exit(EXIT_SUCCESS);  // 正常退出程序
}
