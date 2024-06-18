/*  We start with the appropriate headers and then a function, printdir,
    which prints out the current directory.
    It will recurse for subdirectories, using the depth parameter is used for indentation.  */

// 引入必要的头文件
#include <unistd.h>   // 用于系统调用
#include <stdio.h>    // 标准输入输出
#include <dirent.h>   // 目录操作
#include <string.h>   // 字符串处理
#include <sys/stat.h> // 文件状态信息
#include <stdlib.h>   // 内存管理

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
  struct dirent *entry;
  while ((entry = readdir(dp)) != NULL)
  {
    // 获取文件状态
    struct stat statbuf;
    lstat(entry->d_name, &statbuf);

    // 如果是目录,但忽略 . 和 ..
    if (S_ISDIR(statbuf.st_mode))
    {
      if (strcmp(".", entry->d_name) == 0 ||
          strcmp("..", entry->d_name) == 0)
        continue;

      // 打印目录名,并递归调用 printdir
      printf("%*s%s/\n", depth, "", entry->d_name);
      printdir(entry->d_name, depth + 4);
    }
    else
    {
      // 打印文件名
      printf("%*s%s\n", depth, "", entry->d_name);
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
  char *topdir, pwd[2] = ".";
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
