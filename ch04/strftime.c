// 引入需要的头文件和宏定义
#define _XOPEN_SOURCE // 为了使用strptime函数,glibc2需要这个宏定义
#include <time.h>     // 时间相关的函数和类型
#include <stdio.h>    // 标准输入输出函数
#include <stdlib.h>   // 标准库函数
#include <string.h>   // 字符串处理函数

// 引入必要的头文件
#include <time.h>
#include <stdio.h>
#include <string.h>

int main()
{
  // 声明时间指针和结构体,以及时间变量
  struct tm *tm_ptr, timestruct;
  time_t the_time;

  // 声明字符数组和结果指针
  char buf[256];
  char *result;

  // 获取当前时间
  (void)time(&the_time);
  // 将时间转换为本地时间
  tm_ptr = localtime(&the_time);

  // 使用strftime格式化时间并存储在buf中
  strftime(buf, 256, "%A %d %B, %I:%S %p", tm_ptr);

  // 打印格式化后的时间
  printf("strftime gives: %s\n", buf);

  // 复制一个日期和时间字符串到buf中
  strcpy(buf, "Thu 26 July 2007, 17:53 will do fine");

  // 打印将要解析的日期和时间字符串
  printf("calling strptime with: %s\n", buf);

  // 准备tm_ptr指向timestruct
  tm_ptr = &timestruct;

  // 使用strptime解析日期和时间字符串
  result = strptime(buf, "%a %d %b %Y, %R", tm_ptr);

  // 打印strptime解析到的位置
  printf("strptime consumed up to: %s\n", result);

  // 打印解析后的日期和时间
  printf("strptime gives:\n");
  printf("date: %02d/%02d/%02d\n",
         tm_ptr->tm_year % 100, tm_ptr->tm_mon + 1, tm_ptr->tm_mday);
  printf("time: %02d:%02d\n",
         tm_ptr->tm_hour, tm_ptr->tm_min);

  // 程序正常退出
  exit(0);
}
