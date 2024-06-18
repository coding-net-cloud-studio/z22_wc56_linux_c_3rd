// 引入ncurses库,用于创建基于文本的用户界面
#include <ncursesw/ncurses.h>
// 引入locale库,用于设置本地化环境
#include <locale.h>

int main()
{
  // 设置本地化环境为UTF-8,以支持中文字符
  setlocale(LC_ALL, "");

  // 初始化ncurses库,准备创建用户界面
  initscr();

  // 检查终端是否支持UTF-8和颜色,这是显示中文和颜色的前提条件
  if (!has_colors() || !can_change_color())
  {
    // 如果终端不支持,退出程序并给出提示
    endwin();
    printf("您的终端不支持UTF-8或颜色.\n");
    return 1;
  }

  // 启用颜色功能
  start_color();
  // 初始化颜色对,第一个参数是颜色对的编号,后两个参数分别是前景色和背景色
  init_pair(1, COLOR_WHITE, COLOR_BLUE);

  // 使用颜色对1来改变输出的颜色属性
  wattron(stdscr, COLOR_PAIR(1));
  // 打印UTF-8编码的中文字符串
  waddnwstr(stdscr, L"你好,世界 2025!", 20); // L前缀表示宽字符串,确保正确处理中文字符
  // 关闭颜色对1的颜色属性
  wattroff(stdscr, COLOR_PAIR(1));

  // 刷新屏幕,使之前的内容显示出来
  refresh();

  // 等待用户输入,防止程序立即退出
  getch();

  // 结束ncurses模式,清理资源
  endwin();

  return 0;
}
