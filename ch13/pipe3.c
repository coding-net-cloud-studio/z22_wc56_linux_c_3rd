#include <unistd.h>  // 引入unistd头文件,用于pipe和fork等系统调用
#include <stdlib.h>  // 引入stdlib头文件,用于exit函数
#include <stdio.h>   // 引入stdio头文件,用于printf和fprintf函数
#include <string.h>  // 引入string头文件,用于memset和strlen函数

int main()
{
    int        data_processed;       // 用于存储写入管道的数据量
    int        file_pipes[2];        // 定义一个包含两个整数的数组,用于存储管道的两个文件描述符
    const char some_data[] = "123";  // 定义一个常量字符数组,用于存储要写入管道的数据
    char       buffer[BUFSIZ + 1];   // 定义一个字符数组,用于存储管道的文件描述符
    pid_t      fork_result;          // 定义一个pid_t类型的变量,用于存储fork函数的返回值

    memset(buffer, '\0', sizeof(buffer));  // 将buffer数组的所有元素初始化为'\0'

    // 创建管道,如果成功则file_pipes数组将包含管道的读写文件描述符
    if (pipe(file_pipes) == 0)
    {
        // 调用fork创建子进程,fork_result将包含子进程的PID或-1(失败)
        fork_result = fork();
        // 如果fork失败,打印错误信息并退出程序
        // 检查fork()函数的返回值
        if (fork_result == (pid_t)-1)
        {
            // 如果fork()失败,向标准错误输出错误信息
            fprintf(stderr, "Fork failure");
            // 退出程序并返回失败状态码
            exit(EXIT_FAILURE);
        }

        // 如果是子进程
        if (fork_result == 0)
        {
            // 将管道的读文件描述符转换为字符串格式,并存储在buffer中
            sprintf(buffer, "%d", file_pipes[0]);
            // 使用execl函数启动另一个程序"pipe4",并将管道的读文件描述符作为参数传递
            // 使用execl函数执行另一个程序"pipe4"
            // execl函数的第一个参数是程序名,第二个参数是程序启动时使用的命令行参数
            // buffer变量作为命令行参数传递给"pipe4"程序
            // 最后一个参数是NULL,表示参数列表的结束
            (void)execl("pipe4", "pipe4", buffer, (char *)0);
            // 如果execl执行失败,退出程序
            exit(EXIT_FAILURE);
        }
        else
        {
            // 如果是父进程,向管道的写文件描述符写入数据
            // 在pipe3.c文件中,以下代码的作用是将some_data数组中的数据写入到管道file_pipes[1]中.
            // write()函数用于将数据写入文件描述符,这里是管道的写端.
            // file_pipes[1]是管道的写端文件描述符,some_data是要写入的数据,strlen(some_data)指定要写入数据的字节数.
            // data_processed变量接收实际写入的字节数.

            data_processed = write(file_pipes[1], some_data, strlen(some_data));
            // 打印父进程ID和写入管道的字节数
            printf("%d - wrote %d bytes\n", getpid(), data_processed);
        }
    }
    // 程序正常退出
    exit(EXIT_SUCCESS);
}
