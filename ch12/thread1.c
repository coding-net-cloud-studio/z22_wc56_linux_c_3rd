// 导入标准输入输出库
#include <stdio.h>
// 导入unistd库,用于sleep函数
#include <unistd.h>
// 导入stdlib库,用于exit函数
#include <stdlib.h>
// 导入string库,用于strcpy函数
#include <string.h>
// 导入pthread库,用于创建和管理线程
#include <pthread.h>

// 定义线程函数原型
void *thread_function(void *arg);

// 定义全局字符数组message
char message[] = "Hello World";

// 主函数
int main()
{
    // 定义整型变量res,用于存储线程创建和加入的结果
    int res;
    // 定义pthread_t类型的变量a_thread,用于存储线程标识符
    pthread_t a_thread;
    // 定义void指针变量thread_result,用于存储线程的返回值
    void *thread_result;

    // 创建一个新线程,将thread_function作为线程函数,message作为参数传递给线程函数
    res = pthread_create(&a_thread, NULL, thread_function, (void *)message);
    // 检查线程是否创建成功
    if (res != 0)
    {
        perror("Thread creation failed");
        exit(EXIT_FAILURE);
    }
    // 打印等待线程完成的信息
    printf("等待线程完成...");
    // 等待线程结束,并获取线程的返回值
    res = pthread_join(a_thread, &thread_result);
    // 检查线程是否成功加入
    if (res != 0)
    {
        perror("Thread join failed");
        exit(EXIT_FAILURE);
    }
    // 打印线程返回的信息和message数组的新内容
    printf("线程已加入,它返回了%s\n", (char *)thread_result);
    printf("消息现在是%s\n", message);
    // 程序正常退出
    exit(EXIT_SUCCESS);
}

// 线程函数定义
void *thread_function(void *arg)
{
    // 打印线程函数运行中的信息,以及传入的参数
    printf("thread_function正在运行.参数是%s\n", (char *)arg);
    // 使线程休眠3秒
    sleep(3);
    // 将message数组的内容修改为"Bye!"
    strcpy(message, "Bye!");
    // 线程退出,并返回字符串"Thank you for the CPU time"
    pthread_exit("Thank you for the CPU time");
}
