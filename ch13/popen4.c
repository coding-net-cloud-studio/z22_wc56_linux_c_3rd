#include <unistd.h>  // 引入标准Unix库,用于提供POSIX操作系统API
#include <stdlib.h>  // 引入标准库,用于提供内存分配,数学运算等功能
#include <stdio.h>   // 引入标准输入输出库,用于文件操作和格式化输入输出
#include <string.h>  // 引入字符串处理库,用于处理C风格字符串

int main()
{
    FILE *read_fp;             // 定义一个FILE指针,用于存储popen打开的文件流
    char  buffer[BUFSIZ + 1];  // 定义一个字符数组,用于存储从文件中读取的数据
    int   chars_read;          // 定义一个整型变量,用于存储每次读取的字符数

    memset(buffer, '\0', sizeof(buffer));          // 将buffer数组的所有元素初始化为'\0'
    read_fp = popen("cat popen*.c | wc -l", "r");  // 使用popen执行shell命令,并打开结果文件流
    if (read_fp != NULL)                           // 如果文件流打开成功
    {
        chars_read = fread(buffer, sizeof(char), BUFSIZ, read_fp);  // 从文件流中读取数据到buffer
        while (chars_read > 0)                                      // 当读取到的字符数大于0时,继续循环
        {
            buffer[chars_read - 1] = '\0';                              // 将buffer最后一个字符设置为'\0',确保字符串以null字符结尾
            printf("Reading: %s\n", buffer);                            // 打印读取到的内容
            chars_read = fread(buffer, sizeof(char), BUFSIZ, read_fp);  // 继续从文件流中读取数据
        }
        pclose(read_fp);     // 关闭popen打开的文件流
        exit(EXIT_SUCCESS);  // 程序正常退出
    }
    exit(EXIT_FAILURE);  // 如果文件流打开失败,程序异常退出
}
