// 引入unistd.h头文件,提供对POSIX操作系统API的访问,如read(), write()等系统调用
#include <unistd.h>
// 引入stdlib.h头文件,提供通用的函数,如exit(), malloc(), free()等
#include <stdlib.h>
// 引入stdio.h头文件,提供标准输入输出功能,如printf(), scanf()等
#include <stdio.h>
// 引入string.h头文件,提供字符串处理函数,如strcpy(), strcmp()等
#include <string.h>

int main()
{
    int        data_processed;       // 用于存储读写操作处理的数据量
    int        file_pipes[2];        // 定义一个管道数组,包含两个描述符,用于父子进程间的通信
    const char some_data[] = "123";  // 要写入管道的数据
    char       buffer[BUFSIZ + 1];   // 用于从管道读取数据的缓冲区
    pid_t      fork_result;          // 存储fork操作的结果

    memset(buffer, '\0', sizeof(buffer));  // 清空缓冲区

    // 创建管道
    if (pipe(file_pipes) == 0)
    {
        // 进行fork操作
        fork_result = fork();
        // 检查fork是否成功
        if (fork_result == -1)
        {
            fprintf(stderr, "Fork failure");  // 如果fork失败,输出错误信息
            exit(EXIT_FAILURE);               // 并退出程序
        }

        // 如果fork成功,且fork_result为0,表示当前在子进程中
        if (fork_result == 0)
        {
            // NOTE 在子进程中
            // 从管道的读端读取数据
            data_processed = read(file_pipes[0], buffer, BUFSIZ);
            // 输出读取到的数据及其长度
            printf("Read %d bytes: %s\n", data_processed, buffer);
            // 子进程结束,返回成功状态码
            exit(EXIT_SUCCESS);
        }
        // 否则,当前在父进程中
        else
        {
            // NOTE 在父进程中
            // 向管道的写端写入数据
            data_processed = write(file_pipes[1], some_data, strlen(some_data));
            // 输出写入的数据长度
            printf("Wrote %d bytes\n", data_processed);
        }
    }
    // 程序结束,返回成功状态码
    exit(EXIT_SUCCESS);
}
