#include <stdio.h>  // 引入标准输入输出库
#include <ctype.h>  // 引入字符处理库,用于toupper函数
#include <stdlib.h> // 引入标准库,用于exit函数

int main() // 主函数入口
{
    int ch; // 声明一个整型变量ch,用于存储从输入流中读取的字符
    while ((ch = getchar()) != EOF)
    {                         // 当从输入流中读取的字符不是文件结束符时,执行循环体
        putchar(toupper(ch)); // 将读取到的字符转换为大写后输出
    }
    exit(0); // 程序正常退出,返回状态码0
}
