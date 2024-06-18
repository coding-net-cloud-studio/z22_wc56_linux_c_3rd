/*  首先,初始代码部分.基础窗口显示用一些文本初始化.  */

#include <unistd.h>  // 引入unistd库以使用sleep函数
#include <stdlib.h>  // 引入stdlib库以使用exit函数
#include <curses.h>  // 引入curses库以使用ncurses功能

int main()
{
    WINDOW *sub_window_ptr;  // 声明一个指向子窗口的指针
    int     x_loop;          // 用于遍历窗口的x坐标循环变量
    int     y_loop;          // 用于遍历窗口的y坐标循环变量
    int     counter;         // 用于控制滚动文本循环次数的计数器
    char    a_letter = '1';  // 初始化一个字符变量,用于在窗口填充数字

    initscr();  // 初始化curses模式

    // 使用两个嵌套循环填充基础窗口的文本
    for (y_loop = 0; y_loop < LINES - 1; y_loop++)
    {
        for (x_loop = 0; x_loop < COLS - 1; x_loop++)
        {
            mvwaddch(stdscr, y_loop, x_loop, a_letter);  // 在当前位置添加字符
            a_letter++;                                  // 字符递增
            if (a_letter > '9')
                a_letter = '1';  // 如果字符超过'9',则重置为'1'
        }
    }

    /* 现在我们创建新的滚动子窗口
       并且,如建议的那样,我们必须在刷新屏幕之前'触摸'父窗口. */

    sub_window_ptr = subwin(stdscr, 10, 20, 10, 10);  // 创建一个新的子窗口
    scrollok(sub_window_ptr, 1);                      // 允许子窗口滚动

    touchwin(stdscr);  // 标记父窗口为已更改,以确保刷新时更新
    refresh();         // 刷新屏幕
    sleep(1);          // 等待1秒

    /* 然后我们擦除子窗口的内容,向其中打印文本并刷新它.
       通过循环实现滚动文本. */

    werase(sub_window_ptr);                                                // 擦除子窗口内容
    mvwprintw(sub_window_ptr, 2, 0, "%s", "This window will now scroll");  // 在子窗口中打印文本
    wrefresh(sub_window_ptr);                                              // 刷新子窗口
    sleep(1);                                                              // 等待1秒

    // 循环打印文本以实现滚动效果
    for (counter = 1; counter < 10; counter++)
    {
        wprintw(sub_window_ptr, "%s", "This text is both wrapping and \
                    scrolling.");  // 打印文本,它会自动换行并滚动
        wrefresh(sub_window_ptr);  // 刷新子窗口以显示新文本
        sleep(1);                  // 等待1秒
    }

    /* 完成循环后,我们删除子窗口.然后我们刷新基础屏幕. */

    delwin(sub_window_ptr);  // 删除子窗口

    touchwin(stdscr);  // 再次标记父窗口为已更改
    refresh();         // 刷新屏幕
    sleep(1);          // 等待1秒

    endwin();            // 退出curses模式
    exit(EXIT_SUCCESS);  // 以成功状态退出程序
}
