// 导入必要的头文件
#include <unistd.h>     // 提供对POSIX操作系统API的访问,包括管道
#include <stdlib.h>     // 提供程序退出时的状态码定义和内存管理函数
#include <stdio.h>      // 提供标准输入输出功能
#include <sys/types.h>  // 提供数据类型定义,如mode_t
#include <sys/stat.h>   // 提供文件状态创建和更改的函数,如mkfifo

// 主函数
int main()
{
    // 使用mkfifo函数创建一个命名管道,路径为"/tmp/my_fifo",权限为0777
    // 0777表示所有用户都有读,写和执行权限
    int res = mkfifo("/tmp/my_fifo", 0777);

    // 检查mkfifo函数的返回值
    if (res == 0)
        // 如果创建成功,打印消息
        printf("FIFO created\n");

    // 退出程序并返回成功状态码
    exit(EXIT_SUCCESS);
}
