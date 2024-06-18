// 导入必要的头文件
#include <unistd.h>  // 提供对POSIX操作系统API的访问,包括文件I/O和进程控制
#include <stdlib.h>  // 提供程序退出的函数exit()
#include <stdio.h>   // 提供标准输入输出函数,如printf()
#include <string.h>  // 提供字符串处理函数,如memset()和sscanf()

// 程序的主入口点
int main(int argc, char *argv[])
{
    int  data_processed;      // 用于存储读取数据的字节数
    char buffer[BUFSIZ + 1];  // 创建一个缓冲区来存储从管道读取的数据,BUFSIZ是系统定义的缓冲区大小
    int  file_descriptor;     // 文件描述符,用于指定要读取的文件或管道

    // 使用memset函数将缓冲区初始化为全0
    memset(buffer, '\0', sizeof(buffer));

    // 使用sscanf函数从命令行参数中提取文件描述符的值
    // 使用sscanf函数从命令行参数argv[1]中解析一个整数
    // %d是格式说明符,用于读取整数
    // &file_descriptor是一个指针,指向一个整数变量,用于存储解析出的整数值
    sscanf(argv[1], "%d", &file_descriptor);

    // 使用read函数从指定的文件描述符读取数据到缓冲区
    // 从文件描述符中读取数据到缓冲区
    // file_descriptor: 已打开文件的描述符
    // buffer: 存储数据的缓冲区
    // BUFSIZ: 缓冲区的大小
    data_processed = read(file_descriptor, buffer, BUFSIZ);

    // 打印进程ID,读取的字节数和读取的数据
    printf("%d - read %d bytes: %s\n", getpid(), data_processed, buffer);

    // 程序成功执行后退出
    exit(EXIT_SUCCESS);
}
