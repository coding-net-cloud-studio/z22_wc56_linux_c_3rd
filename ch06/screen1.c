/* 引入所需的头文件 */
#include <unistd.h> // 提供sleep函数
#include <stdlib.h> // 提供exit函数和EXIT_SUCCESS常量
#include <curses.h> // 提供curses库的功能

int main()
{
  /* 初始化curses模式 */
  initscr();

  /*
  将光标移动到逻辑屏幕的(5,15)位置,
  打印"Hello World"并刷新实际屏幕.
  最后,使用sleep(2)函数暂停程序两秒,
  以便我们可以在程序结束前看到输出.
  */
  move(5, 15);                 // 移动光标到第5行第15列
  printw("%s", "Hello World"); // 在当前光标位置打印"Hello World"
  refresh();                   // 刷新屏幕以显示更新内容

  sleep(2); // 程序暂停两秒

  /* 退出curses模式并退出程序 */
  endwin();           // 结束curses模式
  exit(EXIT_SUCCESS); // 以成功状态退出程序
}
