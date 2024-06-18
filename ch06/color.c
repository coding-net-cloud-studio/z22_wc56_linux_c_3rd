/*  首先,我们检查程序的显示终端是否支持颜色.
    如果支持,我们将启动彩色显示.  */

#include <unistd.h> // 包含unistd.h库,用于提供对POSIX操作系统API的访问
#include <stdlib.h> // 包含stdlib.h库,提供一些通用函数
#include <stdio.h>  // 包含stdio.h库,提供C语言的输入输出功能
#include <curses.h> // 包含curses.h库,用于编写基于文本的用户界面

int main()
{
  int i; // 声明一个整型变量i,用于循环计数

  initscr(); // 初始化curses模式
  if (!has_colors())
  {                                               // 检查终端是否支持颜色
    endwin();                                     // 结束curses模式
    fprintf(stderr, "错误 - 此终端不支持颜色\n"); // 输出错误信息到标准错误流
    exit(1);                                      // 以状态码1退出程序
  }

  if (start_color() != OK)
  {                                             // 初始化颜色,如果出错
    endwin();                                   // 结束curses模式
    fprintf(stderr, "错误 - 无法初始化颜色\n"); // 输出错误信息到标准错误流
    exit(2);                                    // 以状态码2退出程序
  }

  /*  现在我们可以打印出允许的颜色数量和颜色对数量.
      我们创建七种颜色对并逐一显示它们.  */

  clear();                                                                                  // 清除屏幕内容
  // mvprintw(5, 5, "有 %d 种颜色和 %d 对颜色组合可用", COLORS, COLOR_PAIRS); // 在指定位置打印颜色和颜色对的数量
  mvprintw(5, 5, "There are %d COLORS, and %d COLOR_PAIRS available", COLORS, COLOR_PAIRS); // 在指定位置打印颜色和颜色对的数量
  refresh();                                                                                // 刷新屏幕以显示更新后的内容

  // 初始化七种颜色对
  // 初始化颜色对,用于在终端中使用不同的颜色组合
  // 参数:颜色对编号,前景色编号,背景色编号

  // 颜色对1:红色前景,黑色背景
  init_pair(1, COLOR_RED, COLOR_BLACK);

  // 颜色对2:红色前景,绿色背景
  init_pair(2, COLOR_RED, COLOR_GREEN);

  // 颜色对3:绿色前景,红色背景
  init_pair(3, COLOR_GREEN, COLOR_RED);

  // 颜色对4:黄色前景,蓝色背景
  init_pair(4, COLOR_YELLOW, COLOR_BLUE);

  // 颜色对5:黑色前景,白色背景
  init_pair(5, COLOR_BLACK, COLOR_WHITE);

  // 颜色对6:品红色前景,蓝色背景
  init_pair(6, COLOR_MAGENTA, COLOR_BLUE);

  // 颜色对7:青色前景,白色背景
  init_pair(7, COLOR_CYAN, COLOR_WHITE);

  // 通过循环逐一显示颜色对
  for (i = 1; i <= 7; i++)
  {
    attroff(A_BOLD);                              // 关闭当前属性(这里是加粗)
    attrset(COLOR_PAIR(i));                       // 设置颜色对
    mvprintw(5 + i, 5, "Color pair %d", i);       // 在指定位置打印颜色对编号
    attrset(COLOR_PAIR(i) | A_BOLD);              // 设置颜色对并开启加粗属性
    mvprintw(5 + i, 25, "Bold color pair %d", i); // 在指定位置打印加粗的颜色对编号
    refresh();                                    // 刷新屏幕以显示更新后的内容
    sleep(1);                                     // 等待1秒
  }

  endwin();           // 结束curses模式
  exit(EXIT_SUCCESS); // 以成功状态退出程序
}
