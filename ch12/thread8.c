// 引入标准输入输出库,用于打印线程执行的信息
#include <stdio.h>
// 引入unistd库,用于提供对POSIX操作系统API的访问,这里主要使用sleep函数
#include <unistd.h>
// 引入stdlib库,用于程序的正常终止
#include <stdlib.h>
// 引入pthread库,用于创建和管理线程
#include <pthread.h>

// 定义线程的数量
#define NUM_THREADS 6

// 定义线程函数的原型,该函数将在每个线程中执行
void *thread_function(void *arg);

int main()
{
  int res;                         // 用于存储线程创建和连接的结果
  pthread_t a_thread[NUM_THREADS]; // 存储线程ID的数组
  void *thread_result;             // 用于接收线程函数的返回值
  int lots_of_threads;             // 循环变量,用于创建多个线程

  // 创建NUM_THREADS个线程
  for (lots_of_threads = 0; lots_of_threads < NUM_THREADS; lots_of_threads++)
  {
    // 创建线程,将新线程的ID存储在a_thread数组中
    // thread_function是线程执行的函数,(void *)&lots_of_threads是传递给线程函数的参数
    // 创建一个名为a_thread的线程数组,数组的大小为lots_of_threads
    // pthread_create函数的第一个参数是线程标识符的指针
    // 第二个参数是线程属性,这里设置为NULL表示使用默认属性
    // 第三个参数是线程执行的函数名,这里是thread_function
    // 第四个参数是传递给线程函数的参数,这里传递的是lots_of_threads的地址
    res = pthread_create(&(a_thread[lots_of_threads]), NULL, thread_function, (void *)&lots_of_threads);
    // 如果线程创建失败,打印错误信息并退出程序
    if (res != 0)
    {
      perror("Thread creation failed");
      exit(EXIT_FAILURE);
    }
    // 让主线程休眠1秒,以便新创建的线程有机会运行
    sleep(1);
  }
  // 打印等待线程完成的信息
  printf("等待线程完成...");
  // 等待所有线程完成
  for (lots_of_threads = NUM_THREADS - 1; lots_of_threads >= 0; lots_of_threads--)
  {
    // 连接线程,获取线程函数的返回值
    // 阻塞当前线程,等待由 'a_thread[lots_of_threads]' 表示的特定线程终止
    // 'thread_result' 是一个指向指针的指针,用于存储目标线程的返回值
    res = pthread_join(a_thread[lots_of_threads], &thread_result);
    // 如果线程连接成功,打印信息
    if (res == 0)
    {
      printf("接收到一个线程\n");
    }
    // 如果线程连接失败,打印错误信息
    else
    {
      perror("pthread_join failed");
    }
  }
  // 打印所有线程已完成的信息
  printf("全部完成\n");
  // 程序正常退出
  exit(EXIT_SUCCESS);
}

// 定义线程函数,参数为指向整数的指针
void *thread_function(void *arg)
{
  // 从参数中解引用得到线程编号
  int my_number = *(int *)arg;
  // 声明一个随机数变量
  int rand_num;

  // 打印线程启动信息,显示传入的线程编号
  printf("线程函数正在运行.参数是 %d\n", my_number);
  // 生成一个1到9之间的随机数
  rand_num = 1 + (int)(9.0 * rand() / (RAND_MAX + 1.0));
  // 使线程休眠随机时间
  sleep(rand_num);
  // 打印线程结束信息,显示线程编号
  printf("来自 %d 的再见\n", my_number);
  // 线程退出
  pthread_exit(NULL);
}
