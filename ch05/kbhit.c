// 引入必要的头文件
#include <stdio.h>   // 用于标准输入输出
#include <stdlib.h>  // 用于系统调用和错误处理
#include <termios.h> // 用于控制终端的属性
#include <term.h>    // 用于与终端相关的功能
#include <curses.h>  // 用于创建图形界面
#include <unistd.h>  // 用于进程控制和系统调用

static struct termios initial_settings, new_settings;
static int peek_character = -1;

// 初始化键盘输入模式
// 初始化键盘设置,以便进行非阻塞读取
void init_keyboard();

// 关闭键盘,恢复原始设置
void close_keyboard();
// kbhit() 函数用于检查键盘是否有输入
int kbhit();
// 读取一个字符,如果peek_character有值,则直接返回该值,否则从标准输入读取
int readch();

int main()
{
  // 初始化键盘
  init_keyboard();

  // 使用循环来持续检查按键输入,直到用户按下'q'键退出
  char ch = 0;
  while (ch != 'q')
  {
    // 打印提示信息,表明程序正在运行
    printf("looping\n");
    // 等待1秒
    sleep(1);
    // 检查是否有按键被按下
    if (kbhit())
    {
      // 如果有按键被按下,读取该按键的值
      ch = readch();
      // 打印出按下的键
      printf("you hit %c\n", ch);
    }
  }

  // 关闭键盘
  close_keyboard();

  // 程序正常退出
  exit(0);
}

// 初始化键盘输入模式
void init_keyboard()
{
  // 获取当前终端设置
  tcgetattr(0, &initial_settings);

  // 创建一个新的终端设置,以便进行修改
  new_settings = initial_settings;

  // 关闭规范模式(ICANON),这样就可以读取单个字符而不是整行
  new_settings.c_lflag &= ~ICANON;

  // 关闭回显(ECHO),以防止键入的字符被显示在屏幕上
  new_settings.c_lflag &= ~ECHO;

  // 关闭信号中断(ISIG),以防止Ctrl+C等信号中断程序
  new_settings.c_lflag &= ~ISIG;

  // 设置非阻塞读取,至少读取一个字符
  new_settings.c_cc[VMIN] = 1;

  // 设置读取超时时间为0秒
  new_settings.c_cc[VTIME] = 0;

  // 立即应用新的终端设置
  tcsetattr(0, TCSANOW, &new_settings);
}

// 关闭键盘输入函数
void close_keyboard()
{
  // 使用tcsetattr函数,立即改变终端的属性
  // 第一个参数0表示标准输入(通常是键盘)
  // TCSANOW表示立即应用更改
  // &initial_settings是一个指向初始设置结构的指针,用于恢复原始设置
  tcsetattr(0, TCSANOW, &initial_settings);
}

// kbhit() 函数用于检查键盘是否有输入
int kbhit()
{
  char ch;   // 用于存储读取的字符
  int nread; // 用于存储读取的字节数

  // 如果 peek_character 不为 -1,说明已经有字符被读取,直接返回 1
  if (peek_character != -1)
    return 1;

  // 设置终端的 VMIN 参数为 0,表示非阻塞读取
  new_settings.c_cc[VMIN] = 0;
  // 立即更新终端设置
  tcsetattr(0, TCSANOW, &new_settings);

  // 尝试读取一个字符
  nread = read(0, &ch, 1);

  // 将 VMIN 参数恢复为 1,表示阻塞读取
  new_settings.c_cc[VMIN] = 1;
  // 再次更新终端设置
  tcsetattr(0, TCSANOW, &new_settings);

  // 如果成功读取到一个字符
  if (nread == 1)
  {
    // 将读取到的字符存储在 peek_character 中
    peek_character = ch;
    // 返回 1 表示有字符输入
    return 1;
  }

  // 如果没有字符输入,返回 0
  return 0;
}

// 读取一个字符,如果peek_character有值,则直接返回该值,否则从标准输入读取
int readch()
{
  char ch; // 用于存储读取的字符

  // 如果peek_character不为-1,说明之前已经读取了一个字符但还没有被取走
  if (peek_character != -1)
  {
    ch = peek_character; // 直接使用peek_character的值
    peek_character = -1; // 重置peek_character为-1,表示没有缓存的字符了
    return ch;           // 返回读取到的字符
  }

  // 从标准输入(文件描述符为0)读取一个字符到ch中
  read(0, &ch, 1);
  return ch; // 返回读取到的字符
}
