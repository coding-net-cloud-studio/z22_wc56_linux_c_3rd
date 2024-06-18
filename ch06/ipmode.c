/*  首先,我们设置程序和初始的curses调用.  */

#include <unistd.h>  // 包含unistd头文件以使用sleep函数
#include <stdlib.h>  // 包含stdlib头文件以使用exit函数和EXIT_SUCCESS常量
#include <curses.h>  // 包含curses头文件以使用curses库的功能
#include <string.h>  // 包含string头文件以使用strncmp和strlen函数

#define PW_LEN   256  // 定义密码长度常量
#define NAME_LEN 256  // 定义用户名长度常量

int main()
{
    char        name[NAME_LEN];           // 定义用户名字符数组
    char        password[PW_LEN];         // 定义密码字符数组
    const char *real_password = "xyzzy";  // 定义真实密码字符串
    int         i             = 0;        // 初始化索引变量

    initscr();  // 初始化curses模式

    move(5, 10);                    // 移动光标到屏幕位置(5, 10)
    printw("%s", "Please login:");  // 在当前位置打印提示信息

    move(7, 10);                  // 移动光标到屏幕位置(7, 10)
    printw("%s", "User name: ");  // 在当前位置打印提示信息
    getstr(name);                 // 读取用户输入的用户名

    move(8, 10);                 // 移动光标到屏幕位置(8, 10)
    printw("%s", "Password: ");  // 在当前位置打印提示信息
    refresh();                   // 刷新屏幕以显示最新的输出

    /*  当用户输入密码时,我们需要阻止密码被回显到屏幕上.
        然后我们将密码与xyzzy进行比较.  */

    cbreak();  // 关闭行缓冲
    noecho();  // 禁止字符回显

    memset(password, '\0', sizeof(password));  // 清空密码数组
    while (i < PW_LEN)
    {                           // 循环直到达到密码长度上限
        password[i] = getch();  // 获取用户输入的字符
        if (password[i] == '\n')
            break;        // 如果输入的是换行符,则退出循环
        move(8, 20 + i);  // 移动光标到密码输入位置
        addch('*');       // 在当前位置打印星号(隐藏密码)
        refresh();        // 刷新屏幕以显示最新的输出
        i++;              // 增加索引
    }

    /*  最后,我们重新启用键盘回显并打印出成功或失败的信息.  */

    echo();      // 启用字符回显
    nocbreak();  // 恢复行缓冲

    move(11, 10);                                                      // 移动光标到屏幕位置(11, 10)
    if (strncmp(real_password, password, strlen(real_password)) == 0)  // 比较用户输入的密码和真实密码
        printw("%s", "Correct");                                       // 如果匹配,打印"Correct"
    else                                                               // 否则
        printw("%s", "Wrong");                                         // 打印"Wrong"
    printw("%s", " password");                                         // 打印" password"
    refresh();                                                         // 刷新屏幕以显示最新的输出
    sleep(2);                                                          // 等待2秒

    endwin();            // 结束curses模式
    exit(EXIT_SUCCESS);  // 以成功状态退出程序
}
