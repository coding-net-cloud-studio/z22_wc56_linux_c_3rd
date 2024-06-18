// 引入标准输入输出库和通用工具库
#include <stdio.h>
#include <stdlib.h>

int main()
{
    // 创建一个字符数组用于存储临时文件名
    char tmpname[L_tmpnam];
    // 定义一个字符指针用于存储tmpnam函数返回的临时文件名
    char *filename;
    // 定义一个文件指针用于操作临时文件
    FILE *tmpfp;

    // 调用tmpnam函数生成一个唯一的临时文件名,并将结果存储在filename指针中
    filename = tmpnam(tmpname);

    // 打印生成的临时文件名
    printf("临时文件名是: %s\n", filename);

    // 调用tmpfile函数创建并打开一个临时文件,将文件指针存储在tmpfp中
    tmpfp = tmpfile();
    // 检查是否成功打开了临时文件
    if (tmpfp)
        // 如果成功打开,打印提示信息
        printf("已成功打开一个临时文件\n");
    else
        // 如果打开失败,使用perror输出错误信息
        perror("tmpfile");

    // 退出程序
    exit(0);
}
