// 导入所需的头文件
#include <stdio.h>   // 标准输入输出
#include <unistd.h>  // Unix标准库,提供sleep函数
#include <stdlib.h>  // 提供exit函数
#include <pthread.h> // 线程库

// 定义线程函数原型
void *thread_function(void *arg);

// 全局变量,用于控制线程间的交替运行
int run_now = 1;
// 全局字符数组,作为线程函数的参数传递
char message[] = "Hello World";

// 主函数
int main()
{
    int res;              // 用于存储线程创建和加入的结果
    pthread_t a_thread;   // 线程标识符
    void *thread_result;  // 用于接收线程结束时的返回值
    int print_count1 = 0; // 主线程打印计数器

    // 创建一个新线程,并指定线程函数和传递给线程函数的参数
    res = pthread_create(&a_thread, NULL, thread_function, (void *)message);
    // 检查线程是否成功创建
    if (res != 0)
    {
        perror("Thread creation failed"); // 打印错误信息
        exit(EXIT_FAILURE);               // 退出程序
    }

    // 主线程循环打印
    while (print_count1++ < 20)
    {
        // 如果run_now为1,则主线程打印
        if (run_now == 1)
        {
            printf("1");
            run_now = 2; // 改变状态,让线程函数运行
        }
        else
        {
            sleep(1); // 等待
        }
    }

    // 打印等待线程完成的信息
    printf("\nWaiting for thread to finish...\n");
    // 等待线程结束
    res = pthread_join(a_thread, &thread_result);
    // 检查线程是否成功加入
    if (res != 0)
    {
        perror("Thread join failed"); // 打印错误信息
        exit(EXIT_FAILURE);           // 退出程序
    }
    // 打印线程成功加入的信息
    printf("Thread joined\n");
    // 程序正常退出
    exit(EXIT_SUCCESS);
}

// 线程函数
void *thread_function(void *arg)
{
    int print_count2 = 0; // 线程函数打印计数器

    // 线程函数循环打印
    while (print_count2++ < 20)
    {
        // 如果run_now为2,则线程函数打印
        if (run_now == 2)
        {
            printf("2");
            run_now = 1; // 改变状态,让主线程运行
        }
        else
        {
            sleep(1); // 等待
        }
    }

    sleep(3); // 线程结束后额外等待3秒(这部分逻辑可能需要根据实际需求调整)
}
