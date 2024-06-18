/* 程序useupper.c接受一个文件名作为参数,并在调用不正确时返回错误. */

#include <unistd.h>  // 包含unistd头文件以使用freopen函数
#include <stdio.h>   // 包含stdio头文件以使用fprintf和perror函数
#include <stdlib.h>  // 包含stdlib头文件以使用exit函数

int main(int argc, char *argv[])  // 主函数,接受命令行参数
{
    char *filename;  // 声明一个字符指针变量用于存储文件名

    // 检查命令行参数的数量是否为2(包括程序名本身)
    if (argc != 2)
    {
        fprintf(stderr, "用法:useupper 文件名\n");  // 如果参数数量不正确,输出错误信息
        exit(1);                                    // 并退出程序,返回错误码1
    }

    filename = argv[1];  // 将命令行第二个参数赋值给filename变量

    /* 完成上述操作后,我们重新打开标准输入,再次检查是否有任何错误,
       然后使用execl调用upper程序. */

    // 尝试将标准输入重定向到指定的文件,如果失败则输出错误信息并退出程序
    if (!freopen(filename, "r", stdin))
    {
        fprintf(stderr, "无法将stdin重定向到文件 %s\n", filename);
        exit(2);  // 返回错误码2
    }

    execl("./upper", "upper", 0);  // 使用execl函数调用upper程序,替换当前进程

    /* 不要忘记execl会替换当前进程;
       如果没有错误,剩余的行不会被执行. */

    perror("无法执行./upper");  // 如果execl调用失败,输出错误信息
    exit(3);                    // 并退出程序,返回错误码3
}
