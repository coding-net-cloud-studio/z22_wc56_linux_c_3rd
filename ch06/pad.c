/*  在程序开始时,我们初始化 pad 结构体,然后创建一个 pad,这将返回一个指向该 pad 的指针.
    我们添加字符以填充 pad 结构体
    (比终端显示宽和长 50 个字符).*/

#include <unistd.h> // 引入标准库,用于 sleep 函数
#include <stdlib.h> // 引入标准库,用于 exit 函数
#include <curses.h> // 引入 curses 库,用于终端操作的函数

int main()
{
  WINDOW *pad_ptr; // 声明一个指向 WINDOW 类型的指针,用于后续操作 pad
  int x, y;        // 声明两个整型变量,用于循环遍历 pad 的行和列
  int pad_lines;   // 声明整型变量,用于存储 pad 的行数
  int pad_cols;    // 声明整型变量,用于存储 pad 的列数
  char disp_char;  // 声明字符型变量,用于在 pad 中填充的字符

  initscr(); // 初始化 curses 模式

  pad_lines = LINES + 50; // 设置 pad 的行数为当前终端行数加 50
  pad_cols = COLS + 50;   // 设置 pad 的列数为当前终端列数加 50

  pad_ptr = newpad(pad_lines, pad_cols); // 创建一个新的 pad,并返回指向它的指针

  disp_char = 'a'; // 初始化填充字符为 'a'
  // 使用两层循环遍历 pad 的每个位置,并填充字符
  for (x = 0; x < pad_lines; x++)
  {
    for (y = 0; y < pad_cols; y++)
    {
      mvwaddch(pad_ptr, x, y, disp_char); // 在 pad 的 (x, y) 位置添加字符 disp_char
      if (disp_char == 'z')
        disp_char = 'a'; // 如果字符为 'z',则将其重置为 'a'
      else
        disp_char++; // 否则,字符递增
    }
  }

  /*  现在我们可以在屏幕上不同位置绘制 pad 的不同区域,然后退出.*/

  prefresh(pad_ptr, 5, 7, 2, 2, 9, 9); // 将 pad 的部分区域(从第 5 行第 7 列开始)刷新显示到屏幕上的指定区域(从第 2 行第 2 列到第 9 行第 9 列)
  sleep(1);                            // 等待 1 秒

  prefresh(pad_ptr, LINES + 5, COLS + 7, 5, 5, 21, 19); // 将 pad 的另一部分区域刷新显示到屏幕上的另一指定区域
  sleep(1);                                             // 等待 1 秒

  delwin(pad_ptr); // 删除 pad 窗口

  endwin();           // 结束 curses 模式
  exit(EXIT_SUCCESS); // 程序正常退出
}
