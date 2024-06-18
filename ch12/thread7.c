// 引入标准输入输出库,用于打印信息
#include <stdio.h>
// 引入unistd库,提供对POSIX操作系统API的访问,包括sleep函数
#include <unistd.h>
// 引入stdlib库,提供程序所需的内存管理功能
#include <stdlib.h>
// 引入pthread库,用于创建和管理线程
#include <pthread.h>

// 定义线程函数原型
void *thread_function(void *arg);

int main()
{
  int res;             // 用于存储线程操作的返回值
  pthread_t a_thread;  // 定义一个线程标识符
  void *thread_result; // 用于接收线程结束时的返回值

  // 创建一个新线程,执行 thread_function 函数
  // 使用pthread_create创建一个新的线程
  // 参数1:指向新线程标识符的指针
  // 参数2:用于设置线程属性的指针,这里使用NULL表示使用默认属性
  // 参数3:指向线程函数的指针,这里是thread_function
  // 参数4:传递给线程函数的参数,这里使用NULL表示没有参数传递
  res = pthread_create(&a_thread, NULL, thread_function, NULL);
  if (res != 0)
  {
    perror("线程创建失败"); // 如果线程创建失败,打印错误信息
    exit(EXIT_FAILURE);     // 并退出程序
  }
  sleep(3);                       // 主线程等待3秒
  printf("取消线程...");          // 打印取消线程的信息
  res = pthread_cancel(a_thread); // 发送取消线程的请求
  if (res != 0)
  {
    perror("线程取消失败"); // 如果取消线程失败,打印错误信息
    exit(EXIT_FAILURE);     // 并退出程序
  }
  printf("等待线程结束..."); // 打印等待线程结束的信息
  // 使用pthread_join函数等待一个特定线程结束,并收回其资源(如栈空间等)
  // 参数a_thread是要等待的线程的标识符
  // 参数&thread_result用于存储目标线程的返回值
  // 返回0表示成功,返回非0表示出错
  res = pthread_join(a_thread, &thread_result); // 等待线程结束
  if (res != 0)
  {
    perror("线程加入失败"); // 如果等待线程结束失败,打印错误信息
    exit(EXIT_FAILURE);     // 并退出程序
  }
  exit(EXIT_SUCCESS); // 程序正常退出
}

// 线程函数的实现
void *thread_function(void *arg)
{
  int i, res;
  // 启用线程取消功能,允许线程在接收到取消请求时终止
  res = pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
  if (res != 0)
  {
    perror("线程 pthread_setcancelstate 失败"); // 如果设置失败,打印错误信息
    exit(EXIT_FAILURE);                         // 并退出程序
  }
  // 使用pthread_setcanceltype函数设置线程的取消类型
  // 参数1:PTHREAD_CANCEL_DEFERRED 表示线程取消类型为延迟取消
  // 参数2:NULL 表示不需要获取原有线程取消类型
  // 返回值:res 用于接收函数执行结果,成功返回0,失败返回错误码
  res = pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, NULL); // 设置线程取消类型为延迟取消
  if (res != 0)
  {
    perror("线程 pthread_setcanceltype 失败"); // 如果设置失败,打印错误信息
    exit(EXIT_FAILURE);                        // 并退出程序
  }
  printf("线程函数正在运行\n"); // 打印线程函数正在运行的信息
  // 循环变量初始化
  for (i = 0; i < 10; i++)
  {
    // 打印线程当前运行状态和计数器值
    printf("线程仍在运行(%d)...\n", i);
    // 使线程休眠1秒
    sleep(1);
  }
  pthread_exit(0); // 线程正常结束
}
