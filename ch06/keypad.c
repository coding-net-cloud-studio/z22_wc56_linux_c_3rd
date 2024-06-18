/* 引入必要的头文件 */
#include <unistd.h>  // 提供对POSIX操作系统API的访问,如read和write函数
#include <stdlib.h>  // 提供程序退出的功能
#include <curses.h>  // 提供控制台输入输出的功能

/* 定义本地转义键的常量,用于后续比较 */
#define LOCAL_ESCAPE_KEY 27

int main()
{
    int key;  // 声明一个整型变量key,用于存储从键盘读取的按键值

    /* 初始化curses模式 */
    initscr();             // 初始化屏幕为curses模式
    crmode();              // 设置curses模式,这是旧版本的函数,新版本中通常不需要
    keypad(stdscr, TRUE);  // 启用特殊键的处理,使得可以识别方向键等功能键

    /* 关闭回显功能 */
    noecho();  // 防止按键时屏幕上的光标移动,通常用于密码输入等场景

    /* 清屏并显示提示信息 */
    clear();                                                     // 清除屏幕内容
    mvprintw(5, 5, "Key pad demonstration. Press 'q' to quit");  // 在屏幕的(5,5)位置打印提示信息
    move(7, 5);                                                  // 将光标移动到(7,5)位置,准备接收用户输入
    refresh();                                                   // 刷新屏幕,使之前的操作立即生效

    /* 循环读取用户按键 */
    key = getch();  // 读取用户按键
    while (key != ERR && key != 'q')
    {                // 当按键不是错误(ERR)且不是退出键('q')时,继续循环
        move(7, 5);  // 将光标移回(7,5)位置
        clrtoeol();  // 清除当前行的剩余部分

        /* 判断按键是否为字母,如果是则直接打印 */
        if ((key >= 'A' && key <= 'Z') ||
            (key >= 'a' && key <= 'z'))
        {
            printw("Key was %c", (char)key);
        }
        /* 如果不是字母,则判断是否为特殊键,并打印相应的信息 */
        else
        {
            switch (key)
            {
            case LOCAL_ESCAPE_KEY:
                printw("%s", "Escape key");
                break;  // 转义键
            case KEY_END:
                printw("%s", "END key");
                break;  // 结束键
            case KEY_BEG:
                printw("%s", "BEGINNING key");
                break;  // 开始键
            case KEY_RIGHT:
                printw("%s", "RIGHT key");
                break;  // 右箭头键
            case KEY_LEFT:
                printw("%s", "LEFT key");
                break;  // 左箭头键
            case KEY_UP:
                printw("%s", "UP key");
                break;  // 上箭头键
            case KEY_DOWN:
                printw("%s", "DOWN key");
                break;  // 下箭头键
            default:
                printw("Unmatched - %d", key);
                break;  // 未匹配到的键,打印其数值
            } /* switch */
        } /* else */

        refresh();      // 刷新屏幕,显示最新的信息
        key = getch();  // 继续读取下一个按键
    } /* while */

    /* 退出curses模式并退出程序 */
    endwin();            // 退出curses模式
    exit(EXIT_SUCCESS);  // 以成功状态退出程序
}
