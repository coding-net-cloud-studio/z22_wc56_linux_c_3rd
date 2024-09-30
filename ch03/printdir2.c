/*  We start with the appropriate headers and then a function, printdir,
    which prints out the current directory.
    It will recurse for subdirectories, using the depth parameter is used for indentation.  */

/*  我们从适当的头文件开始,然后是一个函数 printdir,
    它用于打印当前目录.
    如果存在子目录,它将递归地打印,使用 depth 参数来进行缩进.  */

#include <unistd.h>    // 用于系统调用
#include <stdio.h>     // 标准输入输出
#include <dirent.h>    // 目录操作
#include <string.h>    // 字符串处理
#include <sys/stat.h>  // 文件状态信息
#include <stdlib.h>    // 内存管理

// 打印目录结构的函数
void printdir(char *dir, int depth)
{
    // 打开目录
    DIR *dp;
    if ((dp = opendir(dir)) == NULL)
    {
        // 如果无法打开目录,输出错误信息并返回
        fprintf(stderr, "cannot open directory: %s\n", dir);
        return;
    }

    // 切换到目录
    chdir(dir);

    // 读取目录条目
    // 读取目录项
    struct dirent *entry;
    while ((entry = readdir(dp)) != NULL)
    {
        // 获取文件状态
        struct stat statbuf;
        lstat(entry->d_name, &statbuf);  // 使用 lstat 获取文件或目录的状态信息

        // 如果是目录,但忽略 . 和 ..
        if (S_ISDIR(statbuf.st_mode))  // 检查是否为目录
        {
            if (strcmp(".", entry->d_name) == 0 ||  // 忽略当前目录 "."
                strcmp("..", entry->d_name) == 0)   // 忽略上级目录 ".."
                continue;

            // 打印目录名,并递归调用 printdir
            printf("%*s%s/\n", depth, "", entry->d_name);  // 打印目录名,并加上前缀空格
            printdir(entry->d_name, depth + 4);            // 递归调用 printdir 函数,增加缩进深度
        }
        else
        {
            // 打印文件名
            printf("%*s%s\n", depth, "", entry->d_name);  // 打印文件名,并加上前缀空格
        }
    }

    // 回到上一级目录
    chdir("..");

    // 关闭目录
    closedir(dp);
}

/*  Now we move onto the main function.  */

// 打印目录结构的程序
int main(int argc, char *argv[])
{
    // 如果命令行参数为空,则设置当前目录为顶级目录

    // 定义指向顶级目录的指针和当前工作目录的字符数组
    char *topdir, pwd[2] = ".";

    // 检查命令行参数的数量
    // 如果参数数量不是2(程序名和顶级目录),则使用当前工作目录作为顶级目录
    // 否则,使用命令行提供的第一个参数作为顶级目录
    if (argc != 2)
        topdir = pwd;
    else
        topdir = argv[1];

    // 打印正在扫描的目录
    printf("正在扫描目录 %s\n", topdir);

    // 调用printdir函数递归地打印目录结构
    printdir(topdir, 0);

    // 打印扫描完成信息
    printf("扫描完成.\n");

    // 退出程序
    exit(0);
}
