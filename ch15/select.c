/* 包含必要的头文件和声明 */
#include <sys/types.h>
#include <sys/time.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <stdlib.h>

int main()
{
    // 定义接收键盘输入的缓冲区
    char buffer[128];
    // 定义结果变量和读取的字节数
    int result, nread;

    // 定义文件描述符集合,用于监视文件描述符的变化
    fd_set inputs, testfds;
    // 定义超时时间结构体
    struct timeval timeout;

    // 清空文件描述符集合
    FD_ZERO(&inputs);
    // 将标准输入的文件描述符加入集合
    FD_SET(0, &inputs);

    // 无限循环等待标准输入,最大等待时间为2.5秒
    while (1)
    {
        // 复制文件描述符集合到测试集合
        testfds = inputs;
        // 设置超时时间为2秒500毫秒
        timeout.tv_sec  = 2;
        timeout.tv_usec = 500000;

        // 使用select函数等待文件描述符集合中的文件描述符发生变化或超时
        result = select(FD_SETSIZE, &testfds, (fd_set *)0, (fd_set *)0, &timeout);

        // 根据select函数的返回值进行不同的处理
        switch (result)
        {
        case 0:
            // 超时打印提示信息
            printf("timeout\n");
            break;
        case -1:
            // 出错打印错误信息并退出程序
            perror("select");
            exit(1);

        default:
            // 如果有输入发生,读取标准输入并回显,直到遇到Ctrl-D结束
            if (FD_ISSET(0, &testfds))
            {
                // 获取可读的字节数
                ioctl(0, FIONREAD, &nread);
                // 如果没有可读字节,表示键盘输入结束,打印提示并退出程序
                if (nread == 0)
                {
                    printf("keyboard done\n");
                    exit(0);
                }
                // 读取指定字节数的数据
                nread = read(0, buffer, nread);
                // 在读取的数据末尾添加字符串结束符
                buffer[nread] = 0;
                // 打印读取的数据
                printf("read %d from keyboard: %s", nread, buffer);
            }
            break;
        }
    }
}
