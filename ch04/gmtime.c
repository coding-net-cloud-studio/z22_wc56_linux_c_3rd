// 引入所需的头文件
#include <time.h>   // 时间处理相关函数
#include <stdio.h>  // 标准输入输出函数
#include <stdlib.h> // 通用工具函数

int main()
{
  // 声明时间指针和时间变量
  struct tm *tm_ptr;
  time_t the_time;

  // 获取当前时间
  (void)time(&the_time);

  // 将时间转换为UTC时间结构
  tm_ptr = gmtime(&the_time);

  // 打印原始时间值
  printf("原始时间是 %ld\n", the_time);

  // 打印UTC时间信息
  printf("gmtime转换结果:\n");
  printf("日期:%02d/%02d/%02d\n",
         tm_ptr->tm_year, tm_ptr->tm_mon + 1, tm_ptr->tm_mday);
  printf("时间:%02d:%02d:%02d\n",
         tm_ptr->tm_hour, tm_ptr->tm_min, tm_ptr->tm_sec);

  // 程序正常退出
  exit(0);
}
