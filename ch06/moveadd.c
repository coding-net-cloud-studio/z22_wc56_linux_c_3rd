/* 引入所需的头文件,定义一些字符数组和一个指向这些数组的指针,然后初始化curses结构. */

// 引入标准输入输出头文件,用于基础的输入输出功能
#include <stdio.h>
// 引入unistd.h,提供对POSIX操作系统API的访问,如sleep函数
#include <unistd.h>
// 引入标准库,提供exit函数用于程序退出
#include <stdlib.h>
// 引入字符串操作函数头文件,如strlen函数
#include <string.h>
// 引入curses.h库,用于创建基于文本的用户界面
#include <curses.h>

int main()
{
  const char witch_one[] = " First Witch  "; // 第一个女巫的名字
  const char witch_two[] = " Second Witch "; // 第二个女巫的名字
  const char *scan_ptr;                      // 指向字符数组的指针

  initscr(); // 初始化curses模式

  /* 现在是屏幕上间隔出现的三个初始文本集.
     注意文本属性的开启和关闭标志. */

  move(5, 15);             // 将光标移动到第5行第15列
  attron(A_BOLD);          // 开启加粗属性
  printw("%s", "Macbeth"); // 打印"Macbeth"
  attroff(A_BOLD);         // 关闭加粗属性
  refresh();               // 刷新屏幕以显示更新
  sleep(1);                // 等待1秒

  move(8, 15);                           // 移动光标
  attron(A_STANDOUT);                    // 开启突出显示属性
  printw("%s", "Thunder and Lightning"); // 打印"Thunder and Lightning"
  attroff(A_STANDOUT);                   // 关闭突出显示属性
  refresh();                             // 刷新屏幕
  sleep(1);                              // 等待1秒

  move(10, 10); // 移动光标并打印文本
  printw("%s", "When shall we three meet again");
  move(11, 23);
  printw("%s", "In thunder, lightning, or in rain ?");
  move(13, 10);
  printw("%s", "When the hurlyburly's done,");
  move(14, 23);
  printw("%s", "When the battle's lost and won.");
  refresh(); // 刷新屏幕
  sleep(1);  // 等待1秒

  /* 最后,识别演员并逐个字符地插入他们的名字.
     我们还在main函数的末尾添加了重置函数. */

  attron(A_DIM);                                // 开启昏暗属性
  scan_ptr = witch_one + strlen(witch_one) - 1; // 指针指向witch_one数组的最后一个字符
  while (scan_ptr != witch_one)
  {                     // 当指针不等于数组起始位置时循环
    move(10, 10);       // 移动光标
    insch(*scan_ptr--); // 插入当前指针指向的字符,并将指针向前移动一位
  }

  scan_ptr = witch_two + strlen(witch_two) - 1; // 对第二个女巫的名字做同样的操作
  while (scan_ptr != witch_two)
  {
    move(13, 10);
    insch(*scan_ptr--);
  }
  attroff(A_DIM); // 关闭昏暗属性

  refresh(); // 刷新屏幕
  sleep(1);  // 等待1秒

  move(LINES - 1, COLS - 1); // 将光标移动到屏幕右下角
  refresh();                 // 刷新屏幕
  sleep(1);                  // 等待1秒

  endwin();           // 退出curses模式
  exit(EXIT_SUCCESS); // 以成功状态退出程序
}
