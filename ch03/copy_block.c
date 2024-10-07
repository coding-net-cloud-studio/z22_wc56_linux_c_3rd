#include <unistd.h>    // 引入系统调用库
#include <sys/stat.h>  // 引入文件状态库
#include <fcntl.h>     // 引入文件控制库
#include <stdlib.h>    // 引入通用工具库

int main()
{
    char block[1024];  // 定义一个大小为1024字节的字符数组,用于存储读取的数据块
    int  in, out;      // 定义两个整型变量,分别用于表示输入文件和输出文件的文件描述符
    int  nread;        // 定义一个整型变量,用于存储每次读取的字节数

    in  = open("file.in", O_RDONLY);                                // 打开名为"file.in"的文件,以只读模式,并将文件描述符赋值给变量in
    out = open("file.out", O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);  // 打开名为"file.out"的文件,以只写模式并创建文件(如果不存在),将文件描述符赋值给变量out,设置权限为所有者读写

    while ((nread = read(in, block, sizeof(block))) > 0)  // 当从输入文件中读取到的字节数大于0时,执行循环
        write(out, block, nread);                         // 将读取到的数据块写入输出文件

    exit(0);  // 退出程序,返回值为0表示正常退出
}
