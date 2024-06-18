#include <stdio.h>   // 引入标准输入输出库
#include <stdlib.h>  // 引入标准库,用于exit函数

int main()  // 主函数入口
{
#ifdef DEBUG                                             // 如果定义了DEBUG宏
    printf("编译时间: " __DATE__ " " __TIME__ "\n");     // 打印编译日期和时间
    printf("这是文件%s的第%d行\n", __FILE__, __LINE__);  // 打印当前文件和行号
#endif
    printf("hello world\n");  // 打印"hello world"到控制台
    exit(0);                  // 程序正常退出,返回状态码0
}
