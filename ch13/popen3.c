#include <unistd.h>  // 引入标准UNIX库,提供pipe和fork等函数
#include <stdlib.h>  // 引入标准库,提供exit函数
#include <stdio.h>   // 引入标准输入输出库,提供printf,popen和pclose函数
#include <string.h>  // 引入字符串处理库,提供memset函数

int main()
{
    FILE *read_fp;             // 定义一个FILE指针,用于存储popen打开的管道文件
    char  buffer[BUFSIZ + 1];  // 定义一个字符数组,用于存储从管道中读取的数据
    int   chars_read;          // 定义一个整数变量,用于存储每次从管道中读取的字符数

    memset(buffer, '\0', sizeof(buffer));  // 将buffer数组的所有元素初始化为'\0'
    read_fp = popen("ps ax", "r");         // 使用popen执行"ps ax"命令,并将结果以只读模式关联到管道
    if (read_fp != NULL)                   // 如果管道打开成功
    {
        chars_read = fread(buffer, sizeof(char), BUFSIZ / 10, read_fp);  // 从管道中读取最多BUFSIZ/10个字符到buffer中
        while (chars_read > 0)                                           // 当读取到的字符数大于0时,继续循环
        {
            buffer[chars_read - 1] = '\0';                                   // 将buffer的最后一个字符设置为'\0',以确保字符串正确结束
            printf("Reading %d: %s\n", BUFSIZ / 10, buffer);                 // 打印读取到的字符数和读取到的内容
            chars_read = fread(buffer, sizeof(char), BUFSIZ / 10, read_fp);  // 继续从管道中读取数据
        }
        pclose(read_fp);     // 关闭管道
        exit(EXIT_SUCCESS);  // 程序正常退出
    }
    exit(EXIT_FAILURE);  // 如果管道打开失败,程序异常退出
}
