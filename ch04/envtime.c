// 引入所需的头文件
// 引入time.h头文件以使用time函数和sleep函数
#include <time.h>   // 时间相关的函数和类型
#include <stdio.h>  // 标准输入输出
#include <unistd.h> // 系统调用
#include <stdlib.h> // 通用工具函数

// 定义一个名为main的函数,该函数没有参数
// 并返回一个整数
int main()
{
  int i;
  time_t the_time;

  // 使用for循环,从1到10执行以下操作
  for (i = 1; i <= 10; i++)
  {
    // 获取当前时间,并将结果存储在the_time变量中
    the_time = time((time_t *)0);

    // 打印当前时间(以秒为单位)
    printf("The time is %ld\n", the_time);

    // 暂停程序执行2秒
    sleep(2);
  }

  // 程序正常退出
  exit(0);
}
