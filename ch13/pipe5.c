#include <unistd.h>  // 引入标准Unix库,用于进程间通信和系统调用
#include <stdlib.h>  // 引入标准库,用于内存管理和退出程序
#include <stdio.h>   // 引入标准输入输出库,用于文件操作和错误报告
#include <string.h>  // 引入字符串处理库,用于处理C风格字符串

int main()
{
    int        data_processed;       // 用于存储写入管道的数据量
    int        file_pipes[2];        // 创建一个管道数组,包含读端和写端
    const char some_data[] = "123";  // 要写入管道的数据
    pid_t      fork_result;          // 存储fork函数的返回值

    // 创建管道
    if (pipe(file_pipes) == 0)
    {
        // 创建子进程
        fork_result = fork();
        // 检查fork是否失败
        if (fork_result == (pid_t)-1)
        {
            fprintf(stderr, "Fork failure");  // 如果fork失败,打印错误信息到标准错误流
            exit(EXIT_FAILURE);               // 并退出程序
        }

        // 子进程逻辑
        if (fork_result == (pid_t)0)
        {
            close(0);              // 关闭标准输入
            dup(file_pipes[0]);    // 将管道的读端复制到标准输入
            close(file_pipes[0]);  // 关闭管道的读端
            close(file_pipes[1]);  // 关闭管道的写端

            // 执行od命令,以字符格式显示从管道读取的数据
            // 使用execlp函数调用od命令,用于显示文件的原始数据
            // 参数"od"是命令名,"-c"是指定以字符格式输出
            // 最后一个参数为NULL,表示参数列表的结束
            execlp("od", "od", "-c", (char *)0);
            exit(EXIT_FAILURE);  // 如果execlp失败,退出程序
        }
        // 父进程逻辑
        else
        {
            close(file_pipes[0]);  // 关闭管道的读端
            // 将数据写入管道的写端
            // 使用write系统调用将some_data的内容写入到file_pipes[1]这个管道的写端
            // file_pipes[1]是管道的写端描述符
            // some_data是要写入的数据指针
            // strlen(some_data)指定要写入数据的字节数
            data_processed = write(file_pipes[1], some_data, strlen(some_data));
            close(file_pipes[1]);  // 关闭管道的写端
            // 打印父进程ID和写入管道的字节数
            printf("%d - wrote %d bytes\n", (int)getpid(), data_processed);
        }
    }
    exit(EXIT_SUCCESS);  // 程序正常退出
}
