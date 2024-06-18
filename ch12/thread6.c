// 引入标准输入输出库
#include <stdio.h>
// 引入unistd库,用于sleep函数
#include <unistd.h>
// 引入stdlib库,用于exit函数
#include <stdlib.h>
// 引入pthread库,用于创建和管理线程
#include <pthread.h>

// 定义线程函数原型
void *thread_function(void *arg);

// 定义全局变量message,用于在主线程和子线程间传递消息
char message[] = "Hello World";
// 定义全局变量thread_finished,用于标记子线程是否完成
int thread_finished = 0;

// 主函数
int main()
{
  int res;                    // 用于存储函数调用的结果
  pthread_t a_thread;         // 定义一个pthread_t类型的变量,用于标识线程
  void *thread_result;        // 用于接收线程函数的返回值
  pthread_attr_t thread_attr; // 定义线程属性对象

  int max_priority;                    // 用于存储最大优先级
  int min_priority;                    // 用于存储最小优先级
  struct sched_param scheduling_value; // 定义调度参数结构体

  // 初始化线程属性对象
  res = pthread_attr_init(&thread_attr);
  if (res != 0)
  {
    perror("Attribute creation failed");
    exit(EXIT_FAILURE);
  }
  // 设置线程调度策略为SCHED_OTHER
  // 设置线程属性,将调度策略设置为SCHED_OTHER(其他调度策略)
  // SCHED_OTHER是默认的非实时调度策略,适用于大多数普通线程
  res = pthread_attr_setschedpolicy(&thread_attr, SCHED_OTHER);
  // 检查系统调用返回值,确认调度策略设置是否成功
  if (res != 0)
  {
    // 如果设置调度策略失败,打印错误信息并退出程序
    perror("设置调度策略失败");
    exit(EXIT_FAILURE);
  }
  // 设置线程为分离状态
  // 设置线程属性为分离状态,这样创建的线程在结束时不会保留线程ID,资源会被立即回收
  res = pthread_attr_setdetachstate(&thread_attr, PTHREAD_CREATE_DETACHED);
  // 检查设置属性是否成功,如果失败则打印错误信息并退出程序
  if (res != 0)
  {
    perror("设置分离属性失败"); // 打印具体的错误原因
    exit(EXIT_FAILURE);         // 以失败状态退出程序
  }
  // 创建线程,并传入message作为参数
  // 使用pthread_create函数创建一个新的线程
  // 参数1:指向新线程标识符的指针
  // 参数2:用于设置线程属性的指针,此处使用默认属性,因此传入NULL
  // 参数3:指向线程函数的指针,该函数包含线程执行的代码
  // 参数4:传递给线程函数的参数,此处传递的是message变量的地址
  res = pthread_create(&a_thread, NULL, thread_function, (void *)message);

  // 检查线程创建是否成功
  if (res != 0)
  {
    // 如果线程创建失败,使用perror打印错误信息
    perror("Thread creation failed");
    // 退出程序并返回非零值表示失败
    exit(EXIT_FAILURE);
  }
  // 获取SCHED_OTHER策略的最大和最小优先级
  max_priority = sched_get_priority_max(SCHED_OTHER);
  min_priority = sched_get_priority_min(SCHED_OTHER);
  // 设置线程的最小优先级
  // 设置线程的调度优先级为最小优先级
  scheduling_value.sched_priority = min_priority;

  // 使用pthread_attr_setschedparam函数尝试设置线程属性中的调度参数
  res = pthread_attr_setschedparam(&thread_attr, &scheduling_value);

  // 检查设置调度参数的操作是否成功
  if (res != 0)
  {
    // 如果设置失败,使用perror打印错误信息
    perror("设置调度策略失败");
    // 并退出程序,返回失败状态码
    exit(EXIT_FAILURE);
  }
  // 销毁线程属性对象
  (void)pthread_attr_destroy(&thread_attr);
  // 等待子线程完成
  // 使用循环来检查线程是否完成
  while (!thread_finished)
  {
    // 打印提示信息,表明正在等待线程完成
    printf("等待线程完成...");
    // 使当前线程休眠1秒,以减少CPU的使用和避免忙等
    sleep(1);
  }
  // 子线程完成后,打印消息并退出
  printf("其他线程已完成,再见!\n");
  exit(EXIT_SUCCESS);
}

// 线程函数
void *thread_function(void *arg)
{
  // 打印线程运行信息和传入的参数
  printf("线程函数正在运行.参数是 %s\n", (char *)arg);
  // 线程休眠4秒
  sleep(4);
  // 线程设置完成标志,并退出
  // 打印提示信息,表明第二个线程已经完成了设置
  printf("第二个线程设置完成标志,现在退出\n");

  // 设置线程完成标志为1,表示该线程的工作已经完成
  thread_finished = 1;

  // 退出当前线程,并释放其资源
  pthread_exit(NULL);
}
