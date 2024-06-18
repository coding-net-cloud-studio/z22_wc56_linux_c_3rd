#include <stdio.h> // 引入标准输入输出库

// 函数声明:bill 函数接受一个字符指针作为参数
void bill(char *arg)
{
  // 打印传入参数的值,格式化字符串中包含换行符
  printf("bill: 你传递了 %s\n", arg);
}
