// 引入unistd头文件,提供对POSIX操作系统API的访问,如read(), write()等系统调用
#include <unistd.h>
// 引入stdlib头文件,提供通用的函数,如malloc(), free()等
#include <stdlib.h>
// 引入stdio头文件,提供标准输入输出功能,如printf(), scanf()等
#include <stdio.h>
// 引入string头文件,提供字符串处理函数,如strcpy(), strcmp()等
#include <string.h>
// 引入fcntl头文件,提供文件控制选项,用于设置文件的打开模式和访问权限
#include <fcntl.h>
// 引入sys/types头文件,定义了各种数据类型,常用于底层系统编程
#include <sys/types.h>
// 引入sys/stat头文件,提供了对文件状态信息的访问,如文件大小,权限等
#include <sys/stat.h>

// 定义FIFO的名称
#define FIFO_NAME "/tmp/my_fifo"

int main(int argc, char *argv[])
{
    int res;            // 用于存储函数调用的结果
    int open_mode = 0;  // 用于设置FIFO的打开模式
    int i;

    // 检查命令行参数的数量是否正确
    if (argc < 2)
    {
        // 如果参数不足,打印使用说明并退出程序
        fprintf(stderr, "用法: %s <O_RDONLY O_WRONLY O_NONBLOCK的某种组合>\n", *argv);
        exit(EXIT_FAILURE);
    }

    // 根据命令行参数设置open_mode的值
    // 以下代码段用于解析命令行参数,并设置文件打开模式
    for (i = 1; i < argc; i++)  // 从第二个参数开始遍历命令行参数
    {
        if (strncmp(*++argv, "O_RDONLY", 8) == 0)   // 检查参数是否为只读模式
            open_mode |= O_RDONLY;                  // 设置只读模式标志
        if (strncmp(*argv, "O_WRONLY", 8) == 0)     // 检查参数是否为只写模式
            open_mode |= O_WRONLY;                  // 设置只写模式标志
        if (strncmp(*argv, "O_NONBLOCK", 10) == 0)  // 检查参数是否为非阻塞模式
            open_mode |= O_NONBLOCK;                // 设置非阻塞模式标志
    }

    // 检查FIFO是否存在,如果不存在则创建它
    // 检查FIFO文件是否存在
    if (access(FIFO_NAME, F_OK) == -1)
    {
        // 如果不存在,则创建FIFO文件
        res = mkfifo(FIFO_NAME, 0777);
        if (res != 0)
        {
            // 如果无法创建FIFO,打印错误信息并退出程序
            fprintf(stderr, "无法创建fifo %s\n", FIFO_NAME);
            exit(EXIT_FAILURE);
        }
    }

    // 打印进程ID和打开FIFO的信息
    printf("进程 %d 打开FIFO\n", getpid());
    res = open(FIFO_NAME, open_mode);
    // 打印进程ID和打开FIFO的结果
    printf("进程 %d 结果 %d\n", getpid(), res);
    // 等待5秒
    sleep(5);
    // 如果FIFO成功打开,则关闭它
    if (res != -1)
        (void)close(res);
    // 打印进程结束信息
    printf("进程 %d 结束\n", getpid());
    // 程序正常退出
    exit(EXIT_SUCCESS);
}
