//  1  Include the header files for all the functions we're going to be using in this program.

// 引入系统类型,资源限制,时间结构体,进程控制以及标准输入输出库
#include <sys/types.h>
#include <sys/resource.h>
#include <sys/time.h>
#include <unistd.h>

// 引入标准输入输出和数学库
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// 2  The void function writes a string to a temporary file 10000 times and then performs some arithmetic
// to generate load on the CPU.

// 从提供的代码片段来看,`limits.c` 文件中的注释描述了一个功能:一个空函数会向临时文件中写入一个字符串10000次,然后执行一些算术运算以在CPU上产生负载.

// 打开一个临时文件用于写入
void work()
{
    FILE  *f;
    int    i;
    double x = 4.5;

    // 创建一个临时文件指针
    f = tmpfile();
    // 循环10000次进行文件写入
    for (i = 0; i < 10000; i++)
    {
        // 向临时文件中写入数据
        fprintf(f, "Do some output\n");
        // 检查文件写入是否出错
        if (ferror(f))
        {
            // 如果出错,向标准错误流输出错误信息并退出程序
            fprintf(stderr, "Error writing to temporary file\n");
            exit(1);
        }
    }
    // 循环1000000次进行计算
    for (i = 0; i < 1000000; i++)
        // 计算x的值
        x = log(x * x + 3.21);
}

//  3  The main function calls work and then uses the getrusage function to discover how much CPU time it has used. It displays this information on screen.

int main()
{
    struct rusage r_usage;
    struct rlimit r_limit;
    int           priority;

    // 调用工作函数以产生CPU使用率
    work();

    // 获取当前进程的资源使用情况
    getrusage(RUSAGE_SELF, &r_usage);

    // 打印CPU使用时间:用户时间和系统时间
    printf("CPU usage: User = %ld.%06ld, System = %ld.%06ld\n",
           r_usage.ru_utime.tv_sec,
           r_usage.ru_utime.tv_usec,
           r_usage.ru_stime.tv_sec,
           r_usage.ru_stime.tv_usec);

    // 获取当前进程的优先级
    priority = getpriority(PRIO_PROCESS, getpid());
    // 打印当前优先级
    printf("Current priority = %d\n", priority);

    // 获取当前进程的文件大小限制
    getrlimit(RLIMIT_FSIZE, &r_limit);
    // 打印当前文件大小限制:软限制和硬限制
    printf("Current FSIZE limit: soft = %ld, hard = %ld\n",
           r_limit.rlim_cur,
           r_limit.rlim_max);

    // 设置新的文件大小限制
    r_limit.rlim_cur = 2048;
    r_limit.rlim_max = 4096;
    printf("Setting a 2K file size limit\n");
    setrlimit(RLIMIT_FSIZE, &r_limit);

    // 再次调用工作函数,由于设置了文件大小限制,可能会失败
    work();

    exit(0);
}
