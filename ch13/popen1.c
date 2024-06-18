#include <unistd.h>  // 引入unistd头文件,提供对POSIX操作系统API的访问
#include <stdlib.h>  // 引入stdlib头文件,提供程序退出时的状态码定义和内存管理函数
#include <stdio.h>   // 引入stdio头文件,提供标准输入输出功能
#include <string.h>  // 引入string头文件,提供字符串处理函数

int main()
{
    FILE *read_fp;             // 定义一个FILE指针,用于存储popen打开的文件流
    char  buffer[BUFSIZ + 1];  // 定义一个字符数组,用于存储从命令执行结果中读取的数据
    int   chars_read;          // 定义一个整数变量,用于存储实际读取的字符数

    // 使用memset函数初始化buffer数组为'\0',确保字符串以空字符结尾
    memset(buffer, '\0', sizeof(buffer));

    // 使用popen函数执行"uname -a"命令,并将结果作为文件流打开,存储在read_fp中
    read_fp = popen("uname -a", "r");

    // 检查read_fp是否为NULL,如果不为NULL,则继续执行
    if (read_fp != NULL)
    {
        // 使用fread函数从read_fp指向的文件流中读取数据到buffer中
        chars_read = fread(buffer, sizeof(char), BUFSIZ, read_fp);

        // 如果读取的字符数大于0,则打印输出结果
        if (chars_read > 0)
        {
            printf("输出结果为:-\n%s\n", buffer);
        }

        // 使用pclose函数关闭由popen打开的文件流
        pclose(read_fp);

        // 程序成功执行,退出时返回EXIT_SUCCESS
        exit(EXIT_SUCCESS);
    }

    // 如果read_fp为NULL,说明popen执行失败,程序退出时返回EXIT_FAILURE
    exit(EXIT_FAILURE);
}
