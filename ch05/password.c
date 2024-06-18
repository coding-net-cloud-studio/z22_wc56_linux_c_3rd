// 引入termios库,用于控制终端的输入输出模式
#include <termios.h>

// 引入stdio库,提供标准输入输出功能
#include <stdio.h>

// 引入stdlib库,提供程序退出等辅助功能
#include <stdlib.h>

// 定义密码长度常量
#define PASSWORD_LEN 8

int main()
{
  // 获取并保存终端的初始设置
  struct termios initialrsettings, newrsettings;

  // 定义一个密码数组,长度为 PASSWORD_LEN + 1,用于存储用户输入的密码
  char password[PASSWORD_LEN + 1];

  // 使用 tcgetattr 函数获取标准输入(stdin)的当前终端设置
  // 这些设置将用于之后的密码输入,以确保密码不会在屏幕上显示
  tcgetattr(fileno(stdin), &initialrsettings);

  // 复制初始设置为新的设置,以便进行修改
  newrsettings = initialrsettings;

  // 修改新的终端设置,关闭回显功能
  newrsettings.c_lflag &= ~ECHO;

  // 提示用户输入密码
  printf("Enter password: ");

  // 设置新的终端属性,立即生效
  if (tcsetattr(fileno(stdin), TCSAFLUSH, &newrsettings) != 0)
  {
    // 如果设置失败,输出错误信息
    fprintf(stderr, "Could not set attributes\n");
  }
  else
  {
    // 读取用户输入的密码
    fgets(password, PASSWORD_LEN, stdin);

    // 恢复终端的初始设置
    tcsetattr(fileno(stdin), TCSANOW, &initialrsettings);

    // 输出用户输入的密码
    fprintf(stdout, "\nYou entered %s\n", password);
  }

  // 退出程序
  exit(0);
}
