// 引入标准输入输出库,用于打印信息
#include <stdio.h>
// 引入unistd库,提供对POSIX操作系统API的访问,包括sleep函数
#include <unistd.h>
// 引入stdlib库,提供程序所需的内存管理功能
#include <stdlib.h>
// 引入pthread库,用于创建和管理线程
#include <pthread.h>

// 检查系统是否支持线程优先级调度
#ifndef _POSIX_THREAD_PRIORITY_SCHEDULING
#error "对不起,您的系统不支持线程优先级调度"
#endif

// 定义线程函数
void *thread_function(void *arg);

// 全局变量,用于在主线程和子线程间通信
char message[]       = "Hello World";
int  thread_finished = 0;

int main()
{

    int                res;
    pthread_t          a_thread;                    // 线程标识符
    void              *thread_result;               // 用于接收线程结束时的返回值
    int                max_priority, min_priority;  // 用于获取线程的最大和最小优先级
    struct sched_param scheduling_value;            // 用于设置线程的优先级

    pthread_attr_t thread_attr;  // 线程属性对象

    // 初始化线程属性
    // 初始化线程属性对象
    res = pthread_attr_init(&thread_attr);
    // 检查初始化是否成功,若失败则输出错误信息并退出程序
    if (res != 0)
    {
        perror("属性创建失败");  // 打印具体的错误原因
        exit(EXIT_FAILURE);      // 以失败状态退出程序
    }

    // 设置线程为分离状态,这样线程结束后会自动释放资源
    // 设置线程属性为分离状态,这样创建的线程在结束时不会保留资源,也不需要其他线程回收
    // 如果设置失败,使用 perror 打印错误信息并退出程序
    res = pthread_attr_setdetachstate(&thread_attr, PTHREAD_CREATE_DETACHED);
    if (res != 0)
    {
        // 打印错误信息,提示用户设置分离属性失败的原因
        perror("设置分离属性失败");
        // 退出程序,返回非零值表示程序异常结束
        exit(EXIT_FAILURE);
    }

    // 设置线程调度策略为SCHED_OTHER
    // 使用pthread_attr_setschedpolicy函数来设置线程属性对象中的调度策略
    // 参数1:指向线程属性对象的指针
    // 参数2:指定调度策略,这里使用的是SCHED_OTHER,表示使用系统默认的调度策略
    res = pthread_attr_setschedpolicy(&thread_attr, SCHED_OTHER);
    if (res != 0)
    {
        perror("设置调度策略失败");
        exit(EXIT_FAILURE);
    }

    // 获取线程的最大和最小优先级
    max_priority = sched_get_priority_max(SCHED_OTHER);
    min_priority = sched_get_priority_min(SCHED_OTHER);
    // 设置线程的优先级为最小优先级
    scheduling_value.sched_priority = min_priority;
    // 设置线程属性为指定的调度参数
    // pthread_attr_t 对象,用于存储线程属性
    // struct sched_param 结构体,包含调度策略和优先级信息
    res = pthread_attr_setschedparam(&thread_attr, &scheduling_value);
    if (res != 0)
    {
        perror("设置调度参数失败");
        exit(EXIT_FAILURE);
    }
    // 打印设置的优先级信息
    printf("调度优先级设置为 %d, 最大允许的是 %d\n", min_priority, max_priority);

    // 创建线程
    // 使用pthread_create创建一个新的线程
    // 参数1: a_thread 是一个指向pthread_t类型变量的指针,用于标识新创建的线程
    // 参数2: thread_attr 是线程属性对象,用于设置线程的属性(如分离状态,优先级等),此处使用默认属性,因此传入NULL
    // 参数3: thread_function 是指向线程运行函数的指针,即新线程开始执行时将要调用的函数
    // 参数4: message 是一个void指针,可以传递给线程函数作为参数,此处传入的是指向某个消息的指针
    res = pthread_create(&a_thread, NULL, thread_function, (void *)message);
    if (res != 0)
    {
        perror("线程创建失败");
        exit(EXIT_FAILURE);
    }

    // 销毁线程属性对象
    // 销毁线程属性对象,释放与之关联的资源
    (void)pthread_attr_destroy(&thread_attr);

    // 等待线程结束
    // 使用循环来等待线程完成
    while (!thread_finished)
    {
        // 打印提示信息,表明正在等待线程
        printf("等待线程...");
        // 暂停当前线程1秒钟,以减少CPU的使用率并避免忙等
        sleep(1);
    }
    // 线程结束后打印消息
    printf("线程结束,再见!\n");

    // 退出程序,并返回成功的状态码(0)
    exit(EXIT_SUCCESS);
}

// 线程函数的实现
void *thread_function(void *arg)
{

    // 打印线程运行的信息
    printf("thread_function 正在运行.参数是 %s\n", (char *)arg);
    // 模拟线程执行需要一定时间
    sleep(4);

    // 通知主线程线程已完成
    thread_finished = 1;
    // 线程正常退出
    pthread_exit(NULL);
}
