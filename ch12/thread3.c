// 导入必要的头文件
// 引入标准输入输出库,用于打印信息
#include <stdio.h>
// 引入unistd库,用于提供对POSIX操作系统API的访问,包括进程控制
#include <unistd.h>
// 引入stdlib库,用于提供动态内存分配等功能
#include <stdlib.h>
// 引入string库,用于处理字符串操作
#include <string.h>
// 引入pthread库,用于创建和管理线程
#include <pthread.h>
// 引入semaphore库,用于实现信号量,用于线程间的同步和互斥
#include <semaphore.h>

// 定义线程函数
void *thread_function(void *arg);

// 定义一个信号量
sem_t bin_sem;

// 定义工作区大小
#define WORK_SIZE 1024

// 定义工作区数组
char work_area[WORK_SIZE];

// 主函数
int main()
{
  int res;             // 用于存储函数调用的结果
  pthread_t a_thread;  // 定义一个线程标识符
  void *thread_result; // 用于接收线程的返回值

  // 初始化信号量,第二个参数0表示信号量在当前进程的线程间共享,第三个参数0表示初始值为0
  res = sem_init(&bin_sem, 0, 0);
  if (res != 0)
  {
    perror("Semaphore initialization failed"); // 如果初始化失败,打印错误信息并退出程序
    exit(EXIT_FAILURE);
  }
  // 创建一个新线程,并获取其标识符
  res = pthread_create(&a_thread, NULL, thread_function, NULL);
  if (res != 0)
  {
    perror("Thread creation failed"); // 如果创建失败,打印错误信息并退出程序
    exit(EXIT_FAILURE);
  }
  // 提示用户输入文本
  printf("请输入一些文本.输入'end'结束\n");
  // 循环读取用户输入,直到用户输入'end'
  while (strncmp("end", work_area, 3) != 0)
  {
    fgets(work_area, WORK_SIZE, stdin); // 从标准输入读取一行文本到工作区
    sem_post(&bin_sem);                 // 发送信号量,允许线程处理数据
  }
  printf("\n等待线程完成...\n");
  // 等待线程结束
  res = pthread_join(a_thread, &thread_result);
  if (res != 0)
  {
    perror("Thread join failed"); // 如果等待失败,打印错误信息并退出程序
    exit(EXIT_FAILURE);
  }
  printf("线程已加入\n");
  // 销毁信号量
  sem_destroy(&bin_sem);
  // 程序正常退出
  exit(EXIT_SUCCESS);
}

// 线程函数
void *thread_function(void *arg)
{
  // 等待信号量
  sem_wait(&bin_sem);
  // 循环处理工作区中的数据,直到用户输入'end'
  while (strncmp("end", work_area, 3) != 0)
  {
    printf("您输入了%d个字符\n", strlen(work_area) - 1); // 打印用户输入的字符数(不包括换行符)
    sem_wait(&bin_sem);                                  // 再次等待信号量,以便继续处理下一组数据
  }
  // 线程正常退出
  pthread_exit(NULL);
}
