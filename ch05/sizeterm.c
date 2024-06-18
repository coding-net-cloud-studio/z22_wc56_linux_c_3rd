#include <stdio.h>   // 引入标准输入输出头文件
#include <term.h>    // 引入终端头文件
#include <curses.h>  // 引入curses库头文件
#include <stdlib.h>  // 引入标准库头文件

int main()
{
    int nrows, ncolumns;  // 声明两个整型变量,用于存储终端的行数和列数

    // 设置终端,使用当前环境变量,标准输出文件描述符,以及一个空指针作为选项参数
    // 此函数用于初始化终端,以便正确地获取终端信息
    setupterm(NULL, fileno(stdout), (int *)0);

    // 使用 tigetnum 函数获取终端的行数("lines")和列数("cols")
    // tigetnum函数通过指定的能力字符串来获取终端的数值能力
    nrows    = tigetnum("lines");
    ncolumns = tigetnum("cols");

    // 输出终端的行数和列数
    // printf函数用于格式化输出到标准输出设备
    printf("这个终端有 %d 列和 %d 行\n", ncolumns, nrows);

    // 退出程序,返回状态码 0 表示正常退出
    // exit函数用于结束程序,并返回一个状态码给操作系统
    exit(0);
}
