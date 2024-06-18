#include <unistd.h>  // 引入标准Unix库,用于提供对POSIX操作系统API的访问
#include <stdlib.h>  // 引入标准库,提供如exit等函数
#include <stdio.h>   // 引入标准输入输出库,提供如printf,FILE等
#include <string.h>  // 引入字符串处理库,提供如sprintf,strlen等函数

int main()
{
    FILE *write_fp;            // 声明一个FILE指针,用于后续的管道操作
    char  buffer[BUFSIZ + 1];  // 定义一个缓冲区,用于存储要写入管道的数据,BUFSIZ是系统定义的缓冲区大小

    // 使用sprintf将字符串格式化到buffer中
    sprintf(buffer, "Once upon a time, there was...\n");  // 注意这里缺少了双引号的闭合,应该是sprintf(buffer, "Once upon a time, there was...");
    write_fp = popen("od -c", "w");                       // 使用popen创建一个管道,"od -c"命令以字符方式显示文件内容,"w"表示以写模式打开管道
    if (write_fp != NULL)                                 // 检查管道是否成功创建
    {
        fwrite(buffer, sizeof(char), strlen(buffer), write_fp);  // 将buffer中的数据写入管道
        pclose(write_fp);                                        // 关闭管道
        exit(EXIT_SUCCESS);                                      // 程序执行成功,退出状态码为0
    }
    exit(EXIT_FAILURE);  // 如果管道创建失败,程序执行失败,退出状态码为非0
}
