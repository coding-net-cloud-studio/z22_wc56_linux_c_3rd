// 引入unistd头文件,提供对POSIX操作系统API的访问,如read(), write()等系统调用
#include <unistd.h>
// 引入stdlib头文件,提供通用的函数,如malloc(), free()等内存管理函数
#include <stdlib.h>
// 引入stdio头文件,提供标准输入输出功能,如printf(), scanf()等
#include <stdio.h>
// 引入string头文件,提供字符串处理函数,如strcpy(), strcmp()等
#include <string.h>
// 引入fcntl头文件,提供文件控制选项,用于设置文件的打开模式和访问权限
#include <fcntl.h>
// 引入limits头文件,提供数值限制,如INT_MAX等
#include <limits.h>
// 引入sys/types头文件,提供基本系统数据类型,如pid_t, size_t等
#include <sys/types.h>
// 引入sys/stat头文件,提供文件状态信息,如文件大小,权限等
#include <sys/stat.h>

// 定义一个命名管道的名称,用于进程间通信
#define FIFO_NAME   "/tmp/my_fifo"

// 定义缓冲区大小,使用PIPE_BUF常量,确保数据写入时的原子性
#define BUFFER_SIZE PIPE_BUF

int main()
{
    // 声明管道文件描述符,结果变量和打开模式
    int pipe_fd;
    int res;
    int open_mode = O_RDONLY;
    // 声明缓冲区数组和读取字节数变量
    char buffer[BUFFER_SIZE + 1];
    int  bytes_read = 0;

    // 清空缓冲区内容
    // 使用memset函数将buffer数组的所有元素初始化为'\0'(空字符)
    // memset函数的第一个参数是指向要填充的内存区域的指针
    // 第二个参数是要填充的值,这里是'\0',即空字符
    // 第三个参数是要填充的字节数,这里使用sizeof(buffer)来确定buffer数组的总字节数
    memset(buffer, '\0', sizeof(buffer));

    // 打印进程ID和以只读模式打开FIFO的信息
    printf("进程 %d 以O_RDONLY模式打开FIFO\n", getpid());
    // 打开FIFO并获取文件描述符
    // 打开名为FIFO_NAME的管道,open_mode决定了打开模式(只读,只写或读写)
    // 如果FIFO不存在,且程序没有以写模式打开,将会阻塞;如果以写模式打开,则会出错
    // 成功返回文件描述符,失败返回-1
    pipe_fd = open(FIFO_NAME, open_mode);
    // 打印进程ID和打开结果
    printf("进程 %d 结果 %d\n", getpid(), pipe_fd);

    // 如果文件描述符不为-1,表示成功打开FIFO
    if (pipe_fd != -1)
    {
        // 循环读取FIFO中的数据
        do
        {
            // 读取数据到缓冲区
            // 从管道文件描述符中读取数据
            // 参数 pipe_fd 是打开的管道的文件描述符
            // 参数 buffer 是用来存储读取数据的缓冲区
            // 参数 BUFFER_SIZE 是缓冲区的大小,定义在文件的其他部分
            // 返回值 res 表示实际读取的字节数,如果出错则为负数
            res = read(pipe_fd, buffer, BUFFER_SIZE);
            // 累加读取的字节数
            bytes_read += res;
        } while (res > 0);
        // 关闭管道文件描述符
        (void)close(pipe_fd);
    }
    else
    {
        // 如果打开FIFO失败,退出程序并返回失败状态码
        exit(EXIT_FAILURE);
    }

    // 打印进程ID和读取的总字节数
    printf("进程 %d 完成,读取了%d字节\n", getpid(), bytes_read);
    // 退出程序并返回成功状态码
    exit(EXIT_SUCCESS);
}
