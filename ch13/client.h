// 引入必要的头文件
#include <unistd.h>     // 提供POSIX操作系统API的访问,如read(), write()等
#include <stdlib.h>     // 提供内存管理函数,如malloc(), free()等
#include <stdio.h>      // 提供标准输入输出函数,如printf(), scanf()等
#include <string.h>     // 提供字符串处理函数,如strcpy(), strlen()等
#include <fcntl.h>      // 提供文件控制选项,用于设置管道的非阻塞模式
#include <limits.h>     // 提供一些数值限制,如PATH_MAX
#include <sys/types.h>  // 提供数据类型定义,如pid_t
#include <sys/stat.h>   // 提供文件状态信息,用于创建和检查文件或目录

// 定义服务器端和客户端管道的名称
#define SERVER_FIFO_NAME "/tmp/serv_fifo"    // 服务器端管道名称
#define CLIENT_FIFO_NAME "/tmp/cli_%d_fifo"  // 客户端管道名称,%d将被替换为客户端PID

// 定义缓冲区大小
#define BUFFER_SIZE      20  // 数据缓冲区的大小

// 定义一个结构体,用于在进程间传递数据
struct data_to_pass_st
{
    pid_t client_pid;                  // 客户端进程ID
    char  some_data[BUFFER_SIZE - 1];  // 用于存储要传递的数据的数组
};
