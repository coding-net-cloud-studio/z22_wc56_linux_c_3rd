// 引入标准库
#include <stdlib.h>  // 提供内存分配和释放函数
#include <stdio.h>   // 提供输入输出函数

// 引入MySQL客户端库
#include "mysql.h"

// 定义一个MYSQL类型的变量my_connection,用于存储与MySQL服务器的连接信息
MYSQL my_connection;

// 定义一个MYSQL_RES类型的指针res_ptr,用于存储查询结果
MYSQL_RES *res_ptr;

// 定义一个MYSQL_ROW类型的变量sqlrow,用于存储从结果集中检索到的一行数据
MYSQL_ROW sqlrow;

int main(int argc, char *argv[])
{
    int        res;
    MYSQL      my_connection;  // 创建MySQL连接结构体
    MYSQL_RES *res_ptr;        // 定义结果集指针
    MYSQL_ROW  sqlrow;         // 定义行指针

    // 初始化MySQL连接
    mysql_init(&my_connection);

    // 尝试连接到MySQL服务器
    if (mysql_real_connect(&my_connection, "localhost", "rick", "secret", "foo", 0, NULL, 0))
    {
        printf("连接成功\n");  // 打印连接成功信息

        // 执行查询语句
        res = mysql_query(&my_connection, "SELECT childno, fname, age FROM children WHERE age > 5");

        // 检查查询是否成功
        if (res)
        {
            printf("查询错误: %s\n", mysql_error(&my_connection));  // 打印查询错误信息
        }
        else
        {
            // 使用查询结果
            res_ptr = mysql_use_result(&my_connection);

            // 检查结果集是否有效
            if (res_ptr)
            {
                // 遍历结果集中的每一行
                while ((sqlrow = mysql_fetch_row(res_ptr)))
                {
                    printf("获取数据...\n");  // 打印获取数据信息
                                              // 这里可以添加处理每一行数据的代码
                }

                // 检查MySQL连接是否有错误
                if (mysql_errno(&my_connection))
                {
                    printf("检索错误: %s\n", mysql_error(&my_connection));  // 打印检索错误信息
                }

                // 释放结果集内存
                mysql_free_result(res_ptr);
            }
        }

        // 关闭MySQL连接
        mysql_close(&my_connection);
    }
    else
    {
        fprintf(stderr, "连接失败\n");  // 打印连接失败信息

        // 检查MySQL连接是否有错误
        if (mysql_errno(&my_connection))
        {
            fprintf(stderr, "连接错误 %d: %s\n", mysql_errno(&my_connection), mysql_error(&my_connection));  // 打印连接错误信息
        }
    }

    return EXIT_SUCCESS;
}
