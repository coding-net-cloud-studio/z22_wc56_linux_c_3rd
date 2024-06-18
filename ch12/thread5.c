// 导入必要的头文件
// 引入标准输入输出库,用于printf等函数
#include <stdio.h>
// 引入unistd库,提供sleep函数用于线程休眠
#include <unistd.h>
// 引入stdlib库,提供exit函数用于退出程序
#include <stdlib.h>
// 引入pthread库,提供线程创建和管理的相关函数
#include <pthread.h>

// 定义线程函数原型
void *thread_function(void *arg);

// 全局变量,用于在主线程和子线程间通信
char message[]       = "Hello World";
int  thread_finished = 0;

int main()
{
    int            res;            // 用于存储函数调用的结果
    pthread_t      a_thread;       // 线程标识符
    void          *thread_result;  // 用于接收线程函数的返回值
    pthread_attr_t thread_attr;    // 线程属性对象

    // 初始化线程属性为默认值
    res = pthread_attr_init(&thread_attr);
    if (res != 0)
    {
        perror("Attribute creation failed");  // 打印错误信息
        exit(EXIT_FAILURE);                   // 退出程序
    }
    // 设置线程属性为分离状态,这样线程结束后会自动释放资源
    // 设置线程属性为分离状态,这样创建的线程在结束时不会保留线程ID,不需要手动释放资源
    res = pthread_attr_setdetachstate(&thread_attr, PTHREAD_CREATE_DETACHED);
    if (res != 0)
    {
        perror("Setting detached attribute failed");  // 打印错误信息
        exit(EXIT_FAILURE);                           // 退出程序
    }
    // 创建一个新线程,并传入消息字符串作为参数
    // 使用pthread_create创建一个新的线程
    // 参数1: a_thread 是一个指向pthread_t类型变量的指针,用于标识新创建的线程
    // 参数2: thread_attr 是线程属性对象,用于设置线程的属性(如分离状态,优先级等),这里使用默认属性,因此传入NULL
    // 参数3: thread_function 是指向线程函数的指针,线程启动后将执行该函数
    // 参数4: message 是一个void指针,可以传递给线程函数作为参数
    // 返回值: res 用于存储pthread_create函数的返回值,如果成功创建线程,res应为0;否则,它包含一个错误码
    res = pthread_create(&a_thread, NULL, thread_function, (void *)message);
    if (res != 0)
    {
        perror("Thread creation failed");  // 打印错误信息
        exit(EXIT_FAILURE);                // 退出程序
    }
    // 销毁线程属性对象
    (void)pthread_attr_destroy(&thread_attr);
    // 主线程循环等待,直到子线程设置标志位为1
    while (!thread_finished)
    {
        printf("等待线程完成...\n");
        sleep(1);  // 每秒检查一次
    }
    printf("其他线程已完成,再见!\n");
    exit(EXIT_SUCCESS);  // 程序正常退出
}

// 线程函数定义
void *thread_function(void *arg)
{
    printf("线程函数正在运行.参数是 %s\n", (char *)arg);
    sleep(4);  // 模拟耗时操作
    printf("次线程设置完成标志,现在退出\n");
    thread_finished = 1;  // 设置全局标志位为1
    pthread_exit(NULL);   // 线程正常退出
}
