// 引入所需的头文件
#include <sys/utsname.h>  // 用于获取系统信息
#include <unistd.h>       // 用于执行shell命令
#include <stdio.h>        // 用于输入输出
#include <stdlib.h>       // 用于内存管理

// 主函数,用于获取和打印主机信息
int main()
{
    // 定义一个字符数组,用于存储计算机名
    char computer[256];
    // 定义一个结构体变量,用于存储系统相关信息
    struct utsname uts;

    // 尝试获取计算机名和系统信息
    if (gethostname(computer, 255) != 0 || uname(&uts) < 0)
    {
        // 如果获取失败,输出错误信息并退出程序
        fprintf(stderr, "无法获取主机信息\n");
        exit(1);
    }

    // 打印计算机名
    printf("计算机主机名是 %s\n", computer);
    // 打印系统名称和硬件架构
    printf("系统是 %s 在 %s 硬件上运行\n", uts.sysname, uts.machine);
    // 打印节点名
    printf("节点名是 %s\n", uts.nodename);
    // 打印版本信息和发行版信息
    printf("版本是 %s, %s\n", uts.release, uts.version);

    // 程序正常结束
    exit(0);
}
