#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

// 定义线程数量常量
#define NUM_THREADS 6

// 线程函数声明
void *thread_function(void *arg);

int main()
{

  int res;                         // 用于存储线程创建和连接的结果
  pthread_t a_thread[NUM_THREADS]; // 存储线程ID的数组
  void *thread_result;             // 用于接收线程结束时的返回值
  int lots_of_threads;             // 循环变量,用于创建和连接线程

  // 创建NUM_THREADS个线程
  for (lots_of_threads = 0; lots_of_threads < NUM_THREADS; lots_of_threads++)
  {
    // 创建线程,将线程ID存储在a_thread数组中
    // 创建一个名为a_thread的线程数组,lots_of_threads表示要创建的线程数量
    // pthread_create函数的第一个参数是指向线程标识符的指针
    // 第二个参数是线程属性,设为NULL表示使用默认属性
    // 第三个参数是线程运行函数的起始地址,即线程开始执行时的函数
    // 第四个参数是传递给线程函数的参数,这里传递的是lots_of_threads的指针
    res = pthread_create(&(a_thread[lots_of_threads]), NULL, thread_function, (void *)lots_of_threads);
    // 检查线程是否创建成功
    if (res != 0)
    {
      perror("Thread creation failed");
      exit(EXIT_FAILURE);
    }
    // 注释掉的sleep(1)可以防止线程立即全部启动,造成输出混乱
    /*    sleep(1); */
  }

  // 等待所有线程完成
  printf("等待线程完成...");
  for (lots_of_threads = NUM_THREADS - 1; lots_of_threads >= 0; lots_of_threads--)
  {
    // 连接线程,获取线程结束时的返回值
    res = pthread_join(a_thread[lots_of_threads], &thread_result);
    // 检查线程是否连接成功
    if (res == 0)
    {
      printf("接收到一个线程结束的信号\n");
    }
    else
    {
      perror("pthread_join失败");
    }
  }

  printf("全部完成\n");

  // 程序正常退出
  exit(EXIT_SUCCESS);
}

// 线程函数的实现
void *thread_function(void *arg)
{
  int my_number = (int)arg; // 从参数中获取线程编号
  int rand_num;             // 用于生成随机数

  // 打印线程启动信息
  printf("线程函数正在运行.参数是 %d\n", my_number);
  // 生成一个1到9之间的随机数
  rand_num = 1 + (int)(9.0 * rand() / (RAND_MAX + 1.0));
  // 使线程休眠随机时间
  sleep(rand_num);
  // 线程结束,打印结束信息
  printf("线程%d结束\n", my_number);

  // 线程正常退出
  pthread_exit(NULL);
}
