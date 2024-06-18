// 引入标准输入输出库
#include <stdio.h>
// 引入数学库,以使用sqrt函数计算平方根
#include <math.h>
// 引入断言库,用于在函数开始时检查参数有效性
#include <assert.h>
// 引入标准库,以使用exit函数正常退出程序
#include <stdlib.h>

// 自定义的平方根函数,接受一个double类型的参数x
double my_sqrt(double x)
{
    // 使用断言确保传入的参数非负,因为负数没有实数平方根
    assert(x >= 0.0);
    // 调用math库中的sqrt函数计算平方根并返回结果
    return sqrt(x);
}

// 主函数
int main()
{
    // 打印正数2的平方根
    printf("sqrt +2 = %g\n", my_sqrt(2.0));
    // 尝试打印负数-2的平方根,由于断言的作用,程序将在此处终止
    printf("sqrt -2 = %g\n", my_sqrt(-2.0));
    // 正常退出程序
    exit(0);
}
