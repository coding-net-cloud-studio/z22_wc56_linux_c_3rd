#include <stdlib.h>  // 引入标准库,提供exit函数
#include <stdio.h>   // 引入标准输入输出库,提供printf函数

int main()  // 主函数,程序的入口点
{
    printf("Running ps with system\n");  // 打印正在运行ps命令的信息
    system("ps ax &");                   // 使用system函数执行ps ax命令,&表示在后台运行
    printf("Done.\n");                   // 打印完成信息
    exit(0);                             // 程序正常退出,返回状态码0
}
