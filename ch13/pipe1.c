#include <unistd.h>  // 引入unistd头文件,提供对POSIX操作系统API的访问,包括管道操作
#include <stdlib.h>  // 引入stdlib头文件,提供程序退出时的状态码定义和内存管理函数
#include <stdio.h>   // 引入stdio头文件,提供标准输入输出功能
#include <string.h>  // 引入string头文件,提供字符串处理函数

int main()
{
    int        data_processed;       // 定义一个整型变量用于存储读写操作处理的数据量
    int        file_pipes[2];        // 定义一个整型数组用于存储管道的两个文件描述符
    const char some_data[] = "123";  // 定义一个常量字符数组作为要写入管道的数据
    char       buffer[BUFSIZ + 1];   // 定义一个字符数组用于存储从管道读取的数据,BUFSIZ是系统定义的最大缓冲区大小

    memset(buffer, '\0', sizeof(buffer));  // 使用memset函数将buffer数组的所有元素初始化为'\0'

    // 尝试创建管道,如果成功,file_pipes数组将包含读端和写端的文件描述符
    if (pipe(file_pipes) == 0)
    {
        // 将some_data数组中的数据写入管道的写端,data_processed返回实际写入的字节数
        data_processed = write(file_pipes[1], some_data, strlen(some_data));
        // 打印实际写入的字节数
        printf("Wrote %d bytes\n", data_processed);

        // 从管道的读端读取数据到buffer数组中,data_processed返回实际读取的字节数
        data_processed = read(file_pipes[0], buffer, BUFSIZ);
        // 打印实际读取的字节数和读取到的数据
        printf("Read %d bytes: %s\n", data_processed, buffer);

        // 程序执行成功,退出时返回EXIT_SUCCESS
        exit(EXIT_SUCCESS);
    }
    // 如果管道创建失败,程序退出时返回EXIT_FAILURE
    exit(EXIT_FAILURE);
}
