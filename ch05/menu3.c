// 引入所需的头文件
#include <stdio.h>   // 用于输入输出函数
#include <unistd.h>  // 用于进程控制
#include <stdlib.h>  // 用于动态内存管理

// 定义一个字符指针数组,用于存储菜单项
char *menu[] = {
    "a - add new record",  // 添加新记录的选项
    "d - delete record",   // 删除记录的选项
    "q - quit",            // 退出程序的选项
    NULL,                  // 数组结束标志
};

int getchoice(char *greet, char *choices[], FILE *in, FILE *out);

// 主函数
int main()
{
    // 初始化选择变量
    int choice = 0;

    // 打开输入输出设备文件,用于与终端交互
    FILE *input;
    FILE *output;

    // 检查当前输出是否为标准终端
    if (!isatty(fileno(stdout)))
    {
        // 如果不是终端,打印提示信息
        fprintf(stderr, "您不是在一个终端上,确定.\n");
    }

    // 尝试打开/dev/tty作为输入和输出
    input  = fopen("/dev/tty", "r");
    output = fopen("/dev/tty", "w");

    // 如果无法打开,打印错误信息并退出程序
    if (!input || !output)
    {
        fprintf(stderr, "无法打开/dev/tty\n");
        exit(1);
    }

    // 循环获取用户选择
    do
    {
        // 从终端获取用户的选择
        choice = getchoice("请选择一个操作", menu, input, output);

        // 打印用户的选择
        printf("您选择了:%c\n", choice);
    } while (choice != 'q');  // 当用户选择'q'时退出循环

    // 退出程序
    exit(0);
}

// 获取用户选择的函数,用于菜单选择等场景
int getchoice(char *greet, char *choices[], FILE *in, FILE *out)
{
    int    chosen = 0;  // 初始化未选择状态
    int    selected;    // 存储用户输入的字符
    char **option;      // 指向选项数组的指针

    // 循环直到用户做出选择
    do
    {
        // 向输出文件打印问候语和选项列表
        // 打印欢迎信息和选项列表
        fprintf(out, "选择: %s\n", greet);
        option = choices;
        while (*option)
        {
            // 遍历选项并打印
            fprintf(out, "%s\n", *option);
            option++;
        }

        // 读取用户输入,跳过空白字符
        do
        {
            // 获取用户输入的字符
            selected = fgetc(in);
        } while (selected == '\n');  // 如果输入是换行符,则继续读取下一个字符

        // 检查用户输入是否与某个选项匹配
        option = choices;
        // 遍历菜单选项,查找与当前选中字符匹配的选项
        while (*option)
        {
            // 判断当前选项的第一个字符是否与选中字符匹配
            if (selected == *option[0])
            {
                chosen = 1;  // 如果匹配,设置已选择状态
                break;       // 跳出循环,不再继续查找
            }
            option++;  // 指向下一个选项
        }

        // 如果用户输入不匹配任何选项,提示重新选择
        if (!chosen)
        {
            fprintf(out, "Incorrect choice, select again\n");
        }
    } while (!chosen);  // 未选择时继续循环

    return selected;  // 返回用户选择的字符
}
