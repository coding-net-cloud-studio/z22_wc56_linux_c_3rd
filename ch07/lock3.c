#include <unistd.h>  // 包含unistd.h头文件,该头文件提供了对POSIX操作系统API的访问
#include <stdlib.h>  // 包含stdlib.h头文件,提供了一系列用于程序初始化,终止以及内存分配的函数
#include <stdio.h>   // 包含stdio.h头文件,提供了标准输入输出的功能
#include <fcntl.h>   // 包含fcntl.h头文件,提供了对文件描述符的操作,如设置文件状态标志,复制文件描述符等

// 定义测试文件的路径
const char *test_file = "/tmp/test_lock";

int main()
{
    int          file_desc;            // 文件描述符
    int          byte_count;           // 用于循环写入文件的计数器
    char        *byte_to_write = "A";  // 要写入文件的字符
    struct flock region_1;             // 定义第一个锁区域
    struct flock region_2;             // 定义第二个锁区域
    int          res;                  // 用于存储锁操作的返回值

    // 打开一个文件描述符,读写模式,如果文件不存在则创建,权限为0666
    // 打开或创建一个文件用于读写
    // test_file: 要打开或创建的文件名
    // O_RDWR | O_CREAT: 打开模式,表示以读写方式打开文件,如果不存在则创建
    // 0666: 文件权限,表示所有用户都有读写权限
    file_desc = open(test_file, O_RDWR | O_CREAT, 0666);

    if (!file_desc)
    {
        // 如果无法打开文件,输出错误信息并退出程序
        fprintf(stderr, "无法打开 %s 进行读写\n", test_file);
        exit(EXIT_FAILURE);
    }

    // 向文件中写入数据
    // 以下代码段用于向文件描述符file_desc写入byte_to_write的内容,共写入100次.
    // 注意:这里使用了(void)来避免未使用的返回值警告.

    for (byte_count = 0; byte_count < 100; byte_count++)  // 循环100次
    {
        (void)write(file_desc, byte_to_write, 1);  // 写入一个字节的数据到文件
    }

    // 设置第一个锁区域,共享锁,从第10个字节到第30个字节
    // 设置region_1为读锁
    region_1.l_type   = F_RDLCK;   // 锁定类型为读锁
    region_1.l_whence = SEEK_SET;  // 锁定起始位置相对于文件开始
    region_1.l_start  = 10;        // 锁定起始偏移量为10
    region_1.l_len    = 20;        // 锁定长度为20字节

    // 设置第二个锁区域,排他锁,从第40个字节到第50个字节
    // 设置region_2的锁定类型为写锁(F_WRLCK)
    // 这意味着在指定的文件区域内,其他进程不能进行读或写操作
    region_2.l_type = F_WRLCK;

    // 设置region_2的起始位置相对于文件开头的偏移量为SEEK_SET,即从文件开头开始计算
    region_2.l_whence = SEEK_SET;

    // 设置region_2的锁定区域的起始字节为40
    region_2.l_start = 40;

    // 设置region_2的锁定区域的长度为10个字节
    region_2.l_len = 10;

    // 锁定文件
    printf("进程 %d 正在锁定文件\n", getpid());
    // 尝试设置第一个锁区域
    // 使用fcntl函数尝试对文件描述符file_desc指定的文件区域region_1进行锁定
    // F_SETLK用于设置文件的锁定状态,如果区域已被其他进程锁定,则返回-1
    res = fcntl(file_desc, F_SETLK, &region_1);
    if (res == -1)
        // 如果返回值为-1,表示锁定失败,输出错误信息到标准错误流
        fprintf(stderr, "无法锁定区域 1\n");

    // 尝试设置第二个锁区域
    // 使用fcntl函数尝试对文件描述符file_desc指定的文件区域region_2进行锁定
    // F_SETLK是fcntl的一个命令,用于设置文件锁
    // 如果res等于-1,表示锁定失败,通过fprintf将错误信息输出到标准错误流stderr
    res = fcntl(file_desc, F_SETLK, &region_2);
    if (res == -1)
        fprintf(stderr, "无法锁定区域 2\n");

    // 等待一段时间
    sleep(60);

    // 关闭文件并退出程序
    // 打印当前进程ID和正在关闭文件的消息
    printf("进程 %d 正在关闭文件\n", getpid());

    // 关闭文件描述符file_desc,释放与之关联的系统资源
    close(file_desc);

    // 退出当前进程,EXIT_SUCCESS为成功退出的状态码
    exit(EXIT_SUCCESS);
}
