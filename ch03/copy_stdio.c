// 引入标准输入输出库
#include <stdio.h>
// 引入通用工具库
#include <stdlib.h>

int main()
{
    int   c;         // 声明一个整数变量c,用于存储从文件中读取的字符
    FILE *in, *out;  // 声明两个文件指针in和out,分别指向输入文件和输出文件

    in  = fopen("file.in", "r");   // 打开名为"file.in"的文件,以只读模式,并将文件指针赋给in
    out = fopen("file.out", "w");  // 打开名为"file.out"的文件,以写入模式,并将文件指针赋给out

    while ((c = fgetc(in)) != EOF)  // 当从in指向的文件中读取的字符不是文件结束符时,循环继续
        fputc(c, out);              // 将从in读取的字符写入到out指向的文件中

    exit(0);  // 退出程序,并返回状态码0,表示正常退出
}
