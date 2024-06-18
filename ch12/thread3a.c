// 引入标准输入输出库,用于打印信息
#include <stdio.h>
// 引入unistd库,用于提供对POSIX操作系统API的访问,包括进程控制
#include <unistd.h>
// 引入stdlib库,用于程序的常见任务的函数,如内存分配
#include <stdlib.h>
// 引入string库,用于处理字符串
#include <string.h>
// 引入pthread库,用于创建和管理线程
#include <pthread.h>
// 引入semaphore库,用于同步线程间的操作
#include <semaphore.h>

// 定义线程函数
void *thread_function(void *arg);
// 定义一个信号量
sem_t bin_sem;

#define WORK_SIZE 1024
// 定义一个工作区,用于存储用户输入的文本
char work_area[WORK_SIZE];

int main()
{
  // 声明一个整型变量 res,用于存储函数的返回值或者中间计算结果
  int res;
  pthread_t a_thread;  // 创建一个线程标识符
  void *thread_result; // 用于接收线程结束时的返回值

  // 初始化信号量,第二个参数0表示信号量在当前进程的线程间共享
  res = sem_init(&bin_sem, 0, 0);
  if (res != 0)
  {
    perror("Semaphore initialization failed");
    exit(EXIT_FAILURE);
  }
  // 创建一个新线程,并运行thread_function函数
  res = pthread_create(&a_thread, NULL, thread_function, NULL);
  if (res != 0)
  {
    perror("Thread creation failed");
    exit(EXIT_FAILURE);
  }

  // 提示用户输入文本
  printf("请输入一些文本.输入'end'结束\n");
  // 循环读取用户输入,直到用户输入'end'
  while (strncmp("end", work_area, 3) != 0)
  {
    // 如果用户输入"FAST",则立即发出信号量
    if (strncmp(work_area, "FAST", 4) == 0)
    {
      // 使用信号量bin_sem来同步线程间的操作,确保某一条件满足后执行后续代码
      sem_post(&bin_sem);

      // 将字符串"Wheeee..."复制到工作区work_area中,可能用于线程间的通信或状态更新
      strcpy(work_area, "Wheeee...");
    }
    else
    {
      // 否则从标准输入读取一行文本
      fgets(work_area, WORK_SIZE, stdin);
    }
    // 读取完毕后,发出信号量,允许线程处理数据
    sem_post(&bin_sem);
  }

  // 等待线程结束
  printf("\n等待线程结束...\n");
  // 使用pthread_join函数等待线程a_thread结束,并收回其资源(如栈空间等)
  // 如果线程没有成功结束,该函数会返回一个非零值
  res = pthread_join(a_thread, &thread_result);

  // 检查pthread_join函数的返回值,以确定线程是否成功结束
  if (res != 0)
  {
    // 如果返回值不为0,表示线程加入失败,使用perror打印错误信息
    perror("Thread join failed");

    // 退出程序并返回失败状态码
    exit(EXIT_FAILURE);
  }
  printf("线程已结束\n");
  // 销毁信号量
  sem_destroy(&bin_sem);
  // 程序正常退出
  exit(EXIT_SUCCESS);
}

// 线程函数,用于处理用户输入的文本
void *thread_function(void *arg)
{
  // 等待信号量
  sem_wait(&bin_sem);
  // 循环处理用户输入,直到用户输入'end'
  while (strncmp("end", work_area, 3) != 0)
  {
    // 打印用户输入的字符数(不包括换行符)
    printf("您输入了%d个字符\n", strlen(work_area) - 1);
    // 处理完毕后,等待下一个信号量
    sem_wait(&bin_sem);
  }
  // 线程正常退出
  pthread_exit(NULL);
}
