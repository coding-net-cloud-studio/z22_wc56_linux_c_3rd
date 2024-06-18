#include <stdlib.h>  // 引入标准库,用于内存分配和释放
#include <stdio.h>   // 引入标准输入输出库,用于文件操作和调试信息输出

// 引入MySQL头文件,用于连接MySQL数据库和执行SQL查询
#include "mysql.h"

// 定义全局变量,用于存储MySQL连接,结果集和行数据
// MYSQL结构体用于管理到MySQL服务器的连接
MYSQL my_connection;
// MYSQL_RES结构体指针用于存储查询结果集
MYSQL_RES *res_ptr;
// MYSQL_ROW类型用于存储从结果集中检索出的行数据
MYSQL_ROW sqlrow;

// 声明函数,用于显示查询结果的行数据
void display_row();

int main(int argc, char *argv[])
{
    int res;

    // 初始化MySQL连接
    mysql_init(&my_connection);
    // 尝试连接到MySQL服务器
    if (mysql_real_connect(&my_connection, "localhost", "rick", "secret", "foo", 0, NULL, 0))
    {
        // 连接成功,输出提示信息
        printf("连接成功\n");

        // 执行查询语句
        res = mysql_query(&my_connection, "SELECT childno, fname, age FROM children WHERE age > 5");

        // 检查查询是否成功
        if (res)
        {
            // 查询失败,输出错误信息
            printf("查询错误: %s\n", mysql_error(&my_connection));
        }
        else
        {
            // 查询成功,获取结果集
            res_ptr = mysql_use_result(&my_connection);
            // 检查结果集是否有效
            if (res_ptr)
            {
                // 遍历结果集中的每一行数据
                while ((sqlrow = mysql_fetch_row(res_ptr)))
                {
                    // 输出提示信息
                    printf("获取数据中...\n");
                    // 调用函数显示当前行的数据
                    display_row();
                }
                // 检查MySQL连接的错误码
                if (mysql_errno(&my_connection))
                {
                    printf("检索错误: %s\n", mysql_error(&my_connection));
                }
                // 释放结果集资源
                mysql_free_result(res_ptr);
            }
        }
        // 关闭MySQL连接
        mysql_close(&my_connection);
    }
    else
    {
        // 连接失败,输出错误信息
        fprintf(stderr, "连接失败\n");
        // 检查MySQL连接的错误码并输出错误信息
        if (mysql_errno(&my_connection))
        {
            fprintf(stderr, "连接错误 %d: %s\n", mysql_errno(&my_connection), mysql_error(&my_connection));
        }
    }

    // 返回程序执行状态
    return EXIT_SUCCESS;
}

// 定义一个名为display_row的函数,用于显示数据库查询结果的行
void display_row()
{
    // 声明一个无符号整数变量field_count,用于记录当前处理的字段数量
    unsigned int field_count;

    // 初始化field_count为0,准备开始处理查询结果的第一列
    field_count = 0;

    // 使用while循环遍历所有字段,直到处理完所有字段
    while (field_count < mysql_field_count(&my_connection))
    {
        // 打印当前字段的值,假设sqlrow数组已经包含了查询结果的每一行的数据
        printf("%s ", sqlrow[field_count]);

        // 字段计数器递增,准备处理下一个字段
        field_count++;
    }

    // 打印换行符,以便在输出中分隔不同的行
    printf("\n");
}
