#include <unistd.h>  // 引入标准Unix库,用于系统调用
#include <stdlib.h>  // 引入标准库,用于程序退出和内存管理
#include <stdio.h>   // 引入标准输入输出库,用于打印信息
#include <fcntl.h>   // 引入文件控制库,用于文件锁定
#include <errno.h>   // 引入错误号定义库,用于处理错误情况

const char *lock_file = "/tmp/LCK.test2";  // 定义锁文件的路径

int main()
{
    int file_desc;   // 定义文件描述符变量
    int tries = 10;  // 定义尝试获取锁的次数

    // 使用while循环尝试获取锁
    while (tries--)
    {
        // 以读写模式,创建模式和排他模式打开锁文件
        // 如果文件已存在,open函数会失败并返回-1
        file_desc = open(lock_file, O_RDWR | O_CREAT | O_EXCL, 0444);

        // 检查是否成功打开文件(即获取到锁)
        if (file_desc == -1)
        {
            // 如果获取锁失败,打印当前进程ID和锁已被占用的信息
            printf("%d - 锁已被占用\n", getpid());
            sleep(3);  // 等待3秒后再次尝试获取锁
        }
        else
        {
            // 成功获取到锁,进入临界区
            printf("%d - 我获得了独占访问权限\n", getpid());
            sleep(1);           // 模拟临界区内操作耗时
            close(file_desc);   // 关闭文件描述符,释放资源
            unlink(lock_file);  // 删除锁文件
            // 离开临界区
            sleep(2);  // 模拟非临界区内操作耗时
        }
    }  // 结束while循环
    exit(EXIT_SUCCESS);  // 程序正常退出
}
