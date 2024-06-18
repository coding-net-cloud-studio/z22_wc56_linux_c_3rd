/*  We start with the appropriate headers and then a function, printdir,
    which prints out the current directory.
    It will recurse for subdirectories, using the depth parameter is used for indentation.  */

// 引入必要的头文件
#include <unistd.h>   // 用于系统调用
#include <stdio.h>    // 标准输入输出
#include <dirent.h>   // 目录操作
#include <string.h>   // 字符串处理
#include <sys/stat.h> // 文件状态信息
#include <stdlib.h>   // 通用工具函数

// 打印目录结构的函数
void printdir(char *dir, int depth)
{
  // 打开目录
  DIR *dp = opendir(dir);
  if (dp == NULL)
  {
    // 如果无法打开目录,输出错误信息并返回
    fprintf(stderr, "无法打开目录: %s\n", dir);
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
      if (strcmp(".", entry->d_name) == 0 || strcmp("..", entry->d_name) == 0)
      {
        continue;
      }
      // 打印目录名,并在下一行递归调用 printdir
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

// 打印/home目录下的内容
int main()
{
  // 输出目录扫描提示信息
  printf("正在扫描 /home目录:\n");

  // 调用printdir函数,传入/home目录和初始深度为0
  printdir("/home", 0);

  // 输出扫描完成信息
  printf("扫描完成.\n");

  // 程序正常退出
  exit(0);
}
