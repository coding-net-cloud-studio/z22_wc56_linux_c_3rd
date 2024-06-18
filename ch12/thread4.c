// 引入标准输入输出库,用于打印信息
#include <stdio.h>
// 引入unistd库,用于提供对POSIX操作系统API的访问,包括sleep函数
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

// 定义互斥锁,用于保护共享资源
pthread_mutex_t work_mutex;

// 定义工作区域和退出标志
// 定义一个常量,表示工作区域的大小
#define WORK_SIZE 1024

// 创建一个字符数组,用于存储工作区域的数据
char work_area[WORK_SIZE];

// 定义一个整型变量,用于控制程序的退出时间
int time_to_exit = 0;

int main()
{
  // 声明一个整型变量res,用于存储线程执行的结果
  int res;

  // 声明一个pthread_t类型的变量a_thread,用于标识创建的线程
  pthread_t a_thread;

  // 声明一个void指针类型的变量thread_result,用于线程函数返回结果
  void *thread_result;

  // 初始化互斥锁
  // 初始化互斥锁,用于控制对共享资源的访问
  res = pthread_mutex_init(&work_mutex, NULL);
  // 检查互斥锁初始化是否成功
  if (res != 0)
  {
    // 如果初始化失败,打印错误信息
    perror("Mutex initialization failed");
    // 并退出程序,返回失败状态
    exit(EXIT_FAILURE);
  }

  // 创建线程
  // 使用pthread_create函数创建一个新的线程,并将该线程的标识符存储在a_thread变量中
  // 传递NULL作为第一个参数表示使用默认的线程属性
  // 传递thread_function作为第三个参数,这是将要在新线程中执行的函数
  // 传递NULL作为第四个参数,表示没有传递给线程函数的数据
  res = pthread_create(&a_thread, NULL, thread_function, NULL);

  // 检查线程创建操作是否成功
  if (res != 0)
  {
    // 如果线程创建失败,使用perror打印错误信息
    perror("Thread creation failed");
    // 退出程序并返回一个非零值,表示程序异常退出
    exit(EXIT_FAILURE);
  }

  // 加锁并提示用户输入
  pthread_mutex_lock(&work_mutex);
  printf("请输入一些文本.输入'end'结束\n");

  // 循环读取用户输入,直到用户输入'end'
  // 循环直到接收到退出信号
  while (!time_to_exit)
  {
    // 从标准输入读取数据到工作区
    fgets(work_area, WORK_SIZE, stdin);

    // 解锁互斥锁,允许线程处理输入
    pthread_mutex_unlock(&work_mutex);

    // 等待线程处理完毕
    while (1)
    {
      // 上锁互斥锁,检查工作区是否已被处理
      pthread_mutex_lock(&work_mutex);

      // 如果工作区不为空,说明线程还在处理,解锁并等待
      if (work_area[0] != '\0')
      {
        pthread_mutex_unlock(&work_mutex);
        sleep(1); // 让出CPU时间片,避免忙等
      }
      // 如果工作区为空,说明线程已处理完毕,跳出循环
      else
      {
        break;
      }
    }
  }

  // 解锁
  pthread_mutex_unlock(&work_mutex);

  // 等待线程结束
  // 打印提示信息,表明正在等待线程结束
  printf("等待线程结束...\n");

  // 使用pthread_join函数等待名为a_thread的线程结束,并获取线程的返回值
  // 如果线程结束成功,res将等于0;否则,res将不等于0
  res = pthread_join(a_thread, &thread_result);

  // 检查线程是否成功结束
  if (res != 0)
  {
    // 如果线程没有成功结束,使用perror函数打印错误信息
    perror("线程连接失败");
    // 退出程序并返回一个非零的退出状态码,表示程序出错
    exit(EXIT_FAILURE);
  }

  // 打印提示信息,表明线程已经结束
  printf("线程已结束\n");

  // 销毁互斥锁
  pthread_mutex_destroy(&work_mutex);

  // 程序正常退出
  exit(EXIT_SUCCESS);
}

// 线程函数,用于处理用户输入
void *thread_function(void *arg)
{
  sleep(1); // 初始化时休眠1秒

  // 循环处理用户输入,直到输入为'end'
  pthread_mutex_lock(&work_mutex);
  // 循环直到用户输入"end"为止
  while (strncmp("end", work_area, 3) != 0)
  {
    // 打印用户输入的字符数,减去1是因为字符串结尾有'\0'
    printf("您输入了%d个字符\n", strlen(work_area) - 1);

    // 清空工作区,为下一次输入做准备
    work_area[0] = '\0';

    // 解锁互斥锁,允许用户线程继续输入
    pthread_mutex_unlock(&work_mutex);

    // 等待新的输入
    sleep(1);

    // 再次加锁,以便检查是否有新的输入
    pthread_mutex_lock(&work_mutex);

    // 如果工作区仍为空,说明没有新的输入,解锁并等待
    // 当前代码段是一个循环,用于检查工作区域(work_area)是否有数据
    while (work_area[0] == '\0')
    {
      // 如果工作区域为空('\0'表示字符串结束或空字符串),解锁互斥锁,允许其他线程访问
      pthread_mutex_unlock(&work_mutex);

      // 使当前线程休眠1秒,减少CPU占用并等待工作区域被填充
      sleep(1);

      // 醒来后重新锁定互斥锁,以便安全地检查工作区域的状态
      pthread_mutex_lock(&work_mutex);
    }
  }

  // 设置退出标志
  // 设置退出标志为1,表示线程即将退出
  time_to_exit = 1;

  // 将工作区域清空,确保不会遗留未处理的数据
  work_area[0] = '\0';

  // 解锁互斥锁,允许其他线程访问共享资源
  pthread_mutex_unlock(&work_mutex); // 解锁

  // 线程正常退出
  pthread_exit(0);
}
