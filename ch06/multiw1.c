/* 引入必要的头文件 */
#include <unistd.h>  // 提供sleep函数
#include <stdlib.h>  // 提供exit函数和EXIT_SUCCESS常量
#include <curses.h>  // 提供ncurses库的功能

int main()
{
    // 定义窗口指针和循环变量
    // 声明一个指向新窗口的指针
    WINDOW *new_window_ptr;

    // 声明一个指向弹出窗口的指针
    WINDOW *popup_window_ptr;

    // 声明两个整型变量,用于循环控制
    int x_loop;
    int y_loop;

    // 声明一个字符变量,初始化为'a'
    char a_letter = 'a';

    // 初始化curses模式
    initscr();

    /* 填充基础窗口的字符,并在逻辑屏幕填满后刷新实际屏幕 */
    // move(5, 5); // 这行代码被注释掉了,所以不会执行
    // printw("%s", "Testing multiple windows"); // 这行代码被注释掉了,所以不会执行
    // refresh(); // 这行代码被注释掉了,所以不会执行

    // 使用两个嵌套循环遍历屏幕上的每个位置,并填充字符
    // 在多窗口环境中,遍历屏幕的每一列和每一行
    for (x_loop = 0; x_loop < COLS - 1; x_loop++)  // x_loop 控制列的遍历
    {
        for (y_loop = 0; y_loop < LINES - 1; y_loop++)  // y_loop 控制行的遍历
        {
            mvwaddch(stdscr, y_loop, x_loop, a_letter);  // 在当前位置添加字符 a_letter
            a_letter++;                                  // 字符 a_letter 自增
            if (a_letter > 'z')                          // 如果 a_letter 超出了 'z'
                a_letter = 'a';                          // 将 a_letter 重置为 'a'
        }
    }

    // 刷新屏幕并暂停2秒
    refresh();
    sleep(2);

    /* 创建一个新的10x20窗口,并在其中添加文本 */
    new_window_ptr = newwin(10, 20, 5, 5);
    // 在新的窗口中打印 "Hello World" 字符串,位置在第二行第三列
    mvwprintw(new_window_ptr, 2, 2, "%s", "Hello World");

    // 在新的窗口中打印一个较长的字符串,演示如何在窗口内换行,位置在第五行第三列
    mvwprintw(new_window_ptr, 5, 2, "%s", "Notice how very long lines wrap inside the window");

    // 刷新新窗口并暂停2秒
    wrefresh(new_window_ptr);
    sleep(2);

    /* 改变背景窗口的内容,新窗口会被遮挡 */
    a_letter = '0';
    // 在多窗口环境中,遍历屏幕的每一行和每一列
    for (x_loop = 0; x_loop < COLS - 1; x_loop++)  // 遍历每一列,但不包括最后一列
    {
        for (y_loop = 0; y_loop < LINES - 1; y_loop++)  // 遍历每一行,但不包括最后一行
        {
            // 使用mvwaddch函数在当前位置(y_loop, x_loop)添加字符a_letter到标准窗口stdscr
            mvwaddch(stdscr, y_loop, x_loop, a_letter);

            // 字符a_letter递增,用于在屏幕上显示连续的数字字符
            a_letter++;

            // 如果字符a_letter超过了'9',则将其重置为'0',以便循环显示数字0到9
            if (a_letter > '9')
                a_letter = '0';
        }
    }

    // 刷新屏幕并暂停2秒,以便用户可以看到屏幕更新后的效果
    refresh();
    sleep(2);

    /* 尝试刷新新窗口,但由于没有改变新窗口的内容,所以不会有任何视觉上的变化 */
    wrefresh(new_window_ptr);  // 刷新新窗口
    sleep(2);                  // 等待2秒

    /* 通过touchwin函数告诉curses库,新窗口已经改变,这样下次调用wrefresh时新窗口会重新显示 */
    touchwin(new_window_ptr);  // 标记新窗口为已改变
    wrefresh(new_window_ptr);  // 再次刷新新窗口,这次会因为touchwin的调用而显示更新
    sleep(2);                  // 等待2秒,让用户看到新窗口的更新效果

    /* 添加另一个重叠的带框窗口 */
    // 创建一个新的弹出窗口,指定高度为10行,宽度为20列,起始位置为第8行第8列
    popup_window_ptr = newwin(10, 20, 8, 8);

    // 在弹出窗口周围绘制边框,使用竖线'|'作为两边的边框,横线'-'作为顶部和底部的边框
    box(popup_window_ptr, '|', '-');

    // 在弹出窗口的第5行第2列打印字符串"Pop Up Window!"
    mvwprintw(popup_window_ptr, 5, 2, "%s", "Pop Up Window!");

    // 刷新弹出窗口,使所有更改立即显示在屏幕上
    wrefresh(popup_window_ptr);

    // 等待2秒(2000毫秒),以便用户可以看到弹出窗口及其内容
    sleep(2);

    /* 对新窗口和弹出窗口进行操作,然后清除和删除它们 */
    // 触摸新窗口并刷新,显示新窗口内容,等待2秒
    touchwin(new_window_ptr);  // 标记新窗口为需要更新的状态
    wrefresh(new_window_ptr);  // 将新窗口的内容更新到屏幕上
    sleep(2);                  // 等待2秒

    // 清除新窗口内容并刷新,等待2秒后删除窗口
    wclear(new_window_ptr);    // 清除新窗口的内容
    wrefresh(new_window_ptr);  // 更新屏幕上的新窗口内容为清除后的状态
    sleep(2);                  // 等待2秒
    delwin(new_window_ptr);    // 删除新窗口

    // 触摸弹出窗口并刷新,显示弹出窗口内容,等待2秒后删除窗口
    touchwin(popup_window_ptr);  // 标记弹出窗口为需要更新的状态
    wrefresh(popup_window_ptr);  // 将弹出窗口的内容更新到屏幕上
    sleep(2);                    // 等待2秒
    delwin(popup_window_ptr);    // 删除弹出窗口

    // 触摸标准窗口并刷新,最后退出curses模式并正常退出程序
    // 触摸标准窗口,更新窗口内容
    touchwin(stdscr);
    // 刷新屏幕以显示更新后的内容
    refresh();
    // 等待2秒
    sleep(2);
    // 结束窗口模式
    endwin();
    // 以成功状态退出程序
    exit(EXIT_SUCCESS);
}
