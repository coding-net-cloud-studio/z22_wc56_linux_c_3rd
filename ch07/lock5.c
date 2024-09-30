
#include <unistd.h>  // 用于进程控制,如sleep()函数
#include <stdlib.h>  // 用于内存管理,如malloc()和free()函数
#include <stdio.h>   // 用于输入输出,如printf()函数
#include <fcntl.h>   // 用于文件控制,如open(),close()函数

// 定义测试文件的路径
const char *test_file = "/tmp/test_lock";

int main()
{
    int          file_desc;       // 文件描述符
    struct flock region_to_lock;  // 用于锁定文件区域的锁结构体
    int          res;             // 用于存储fcntl函数的返回值

    // 打开一个文件描述符,读写模式,如果文件不存在则创建,权限为0666
    file_desc = open(test_file, O_RDWR | O_CREAT, 0666);
    // 检查文件是否成功打开
    if (!file_desc)
    {
        // 如果无法打开文件,向标准错误输出错误信息
        fprintf(stderr, "无法打开 %s 进行读/写\n", test_file);
        // 退出程序并返回失败状态码
        exit(EXIT_FAILURE);
    }

    // 初始化锁结构体,尝试读取锁定文件的一个区域
    region_to_lock.l_type   = F_RDLCK;   // 读锁
    region_to_lock.l_whence = SEEK_SET;  // 从文件开头开始
    region_to_lock.l_start  = 10;        // 锁定起始位置
    region_to_lock.l_len    = 5;         // 锁定长度
    printf("进程 %d, 尝试 F_RDLCK, 区域 %d 到 %d\n", getpid(), (int)region_to_lock.l_start, (int)(region_to_lock.l_start + region_to_lock.l_len));
    res = fcntl(file_desc, F_SETLK, &region_to_lock);  // 设置锁
    if (res == -1)
    {
        // 如果设置锁失败,输出失败信息
        printf("进程 %d - 未能锁定区域\n", getpid());
    }
    else
    {
        // 如果成功锁定,输出成功信息
        printf("进程 %d - 获得锁定区域\n", getpid());
    }

    // 解锁之前锁定的区域
    region_to_lock.l_type = F_UNLCK;                                     // 解锁
    res                   = fcntl(file_desc, F_SETLK, &region_to_lock);  // 设置解锁
    // 输出解锁结果
    printf("进程 %d, 尝试 F_UNLCK, 区域 %d 到 %d\n", getpid(), (int)region_to_lock.l_start, (int)(region_to_lock.l_start + region_to_lock.l_len));
    if (res == -1)
    {
        printf("进程 %d - 未能解锁区域\n", getpid());
    }
    else
    {
        printf("进程 %d - 解锁区域\n", getpid());
    }

    // 尝试解锁一个更大的区域,覆盖之前的锁定和未锁定的部分
    region_to_lock.l_start = 0;                                           // 起始位置改为0
    region_to_lock.l_len   = 50;                                          // 长度改为50
    res                    = fcntl(file_desc, F_SETLK, &region_to_lock);  // 设置解锁
    // 输出解锁结果
    printf("进程 %d, 尝试 F_UNLCK, 区域 %d 到 %d\n", getpid(), (int)region_to_lock.l_start, (int)(region_to_lock.l_start + region_to_lock.l_len));
    if (res == -1)
    {
        printf("进程 %d - 未能解锁区域\n", getpid());
    }
    else
    {
        printf("进程 %d - 解锁区域\n", getpid());
    }

    // 尝试对一个新区域加写锁
    region_to_lock.l_type  = F_WRLCK;                                     // 写锁
    region_to_lock.l_start = 16;                                          // 新的起始位置
    region_to_lock.l_len   = 5;                                           // 新的长度
    res                    = fcntl(file_desc, F_SETLK, &region_to_lock);  // 设置锁
    // 输出加锁结果
    printf("进程 %d, 尝试 F_WRLCK, 区域 %d 到 %d\n", getpid(), (int)region_to_lock.l_start, (int)(region_to_lock.l_start + region_to_lock.l_len));
    if (res == -1)
    {
        printf("进程 %d - 未能锁定区域\n", getpid());
    }
    else
    {
        printf("进程 %d - 获得锁定区域\n", getpid());
    }

    // 尝试在另一个区域加读锁
    region_to_lock.l_type  = F_RDLCK;                                     // 读锁
    region_to_lock.l_start = 40;                                          // 另一个起始位置
    region_to_lock.l_len   = 10;                                          // 另一个长度
    res                    = fcntl(file_desc, F_SETLK, &region_to_lock);  // 设置锁
    // 输出加锁结果
    printf("进程 %d, 尝试 F_RDLCK, 区域 %d 到 %d\n", getpid(), (int)region_to_lock.l_start, (int)(region_to_lock.l_start + region_to_lock.l_len));
    if (res == -1)
    {
        printf("进程 %d - 未能锁定区域\n", getpid());
    }
    else
    {
        printf("进程 %d - 获得锁定区域\n", getpid());
    }

    // 尝试对之前已经加锁的区域加写锁,并且等待直到获得锁
    region_to_lock.l_type = F_WRLCK;                                      // 写锁
    res                   = fcntl(file_desc, F_SETLKW, &region_to_lock);  // 设置锁并等待
    // 输出加锁结果
    printf("进程 %d, 尝试 F_WRLCK 并等待, 区域 %d 到 %d\n", getpid(), (int)region_to_lock.l_start, (int)(region_to_lock.l_start + region_to_lock.l_len));
    if (res == -1)
    {
        printf("进程 %d - 未能锁定区域\n", getpid());
    }
    else
    {
        printf("进程 %d - 获得锁定区域\n", getpid());
    }

    // 进程结束,关闭文件描述符
    printf("进程 %d 结束\n", getpid());
    close(file_desc);
    exit(EXIT_SUCCESS);
}
