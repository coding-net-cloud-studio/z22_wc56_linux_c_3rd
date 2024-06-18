// 引入unistd.h头文件,用于执行系统调用和POSIX标准库函数
#include <unistd.h>

// 引入stdlib.h头文件,用于执行通用工具函数,如内存管理,字符串转换等
#include <stdlib.h>

int main()
{
    // 定义一个字符数组作为缓冲区,用于存储读取的数据
    char buffer[128];

    // 定义一个整型变量,用于存储实际读取的字节数
    int nread;

    // 从标准输入(文件描述符为0)读取最多128个字节到缓冲区
    nread = read(0, buffer, 128);

    // 检查是否发生了读取错误
    if (nread == -1)
    {
        // 如果发生错误,向标准错误输出(文件描述符为2)写入错误信息
        write(2, "A read error has occurred\n", 26);
    }

    // 将读取到的数据写入标准输出(文件描述符为1),检查是否全部写入
    if ((write(1, buffer, nread)) != nread)
    {
        // 如果写入不完整,向标准错误输出写入错误信息
        write(2, "A write error has occurred\n", 27);
    }

    // 程序正常退出
    exit(0);
}
