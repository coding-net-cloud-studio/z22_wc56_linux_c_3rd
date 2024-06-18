// 引入时间库,用于获取当前时间
#include <time.h>

// 引入标准输入输出库,用于文件操作和打印输出
#include <stdio.h>

// 引入标准库,用于内存管理,字符串操作等
#include <stdlib.h>

// 引入time.h头文件以使用time函数和ctime函数
#include <time.h>

int main()
{
  // 声明一个time_t类型的变量,用于存储当前时间
  time_t timeval;

  // 获取当前时间,并将结果存储在timeval变量中
  (void)time(&timeval);

  // 将timeval转换为字符串格式,并打印输出当前日期和时间
  printf("当前日期是:%s", ctime(&timeval));

  // 程序正常结束,返回0
  exit(0);
}
