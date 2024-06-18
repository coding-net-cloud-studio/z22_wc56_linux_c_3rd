#include <stdio.h>   // 引入标准输入输出头文件
#include <stdlib.h>  // 引入标准库头文件

char *menu[] = {
    "a - add new record",  // 添加新记录
    "d - delete record",   // 删除记录
    "q - quit",            // 退出
    NULL,
};

// 函数声明,用于获取用户的选择,参数greet为欢迎语,参数choices为选项数组
int getchoice(char *greet, char *choices[]);

// 主函数
int main()
{
    int choice = 0;

    // 使用do-while循环,直到用户选择退出
    do
    {
        // 调用getchoice函数获取用户选择
        choice = getchoice("请选择一个操作", menu);
        // 打印用户的选择
        printf("您选择了: %c\n", choice);
    } while (choice != 'q');
    // 退出程序
    exit(0);
}

// 函数getchoice接收两个参数:一个欢迎语和一个字符串数组,用于显示菜单并获取用户的选择.
int getchoice(char *greet, char *choices[])
{
    int    chosen = 0;  // 初始化chosen为0,表示用户尚未做出选择
    int    selected;    // 用于存储用户输入的字符
    char **option;      // 指向字符串数组的指针

    // 当用户未做出选择时,循环继续
    do
    {
        printf("Choice: %s\n", greet);  // 打印欢迎语
        option = choices;               // 将option指向choices数组的首地址
                                        // 遍历choices数组,打印每个选项
        // 遍历字符串数组,并逐个打印出每个选项
        while (*option)
        {
            // 使用printf函数打印当前选项
            printf("%s\n", *option);
            // 移动到下一个选项
            option++;
        }
        selected = getchar();  // 获取用户输入的字符
        option   = choices;    // 重置option指向choices数组的首地址
                               // 遍历choices数组,检查用户输入的字符是否与某个选项的首字符匹配
        // 遍历菜单选项,查找用户选择的项
        while (*option)
        {
            // 检查当前选项的第一个字符是否与用户选择的字符匹配
            if (selected == *option[0])
            {
                // 如果匹配,设置chosen为1,表示用户已做出选择
                chosen = 1;
                break;  // 跳出循环,不再继续检查其他选项
            }
            // 指向下一个选项
            option++;
        }
        // 如果用户未做出正确选择,提示重新选择
        if (!chosen)
        {
            printf("Incorrect choice, select again\n");
        }
    } while (!chosen);  // 当用户未做出选择时,循环继续
    return selected;  // 返回用户选择的字符
}
