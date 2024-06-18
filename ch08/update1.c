#include <stdlib.h>
#include <stdio.h>

// 引入MySQL头文件
#include "mysql.h"

int main(int argc, char *argv[])
{
    // 声明MySQL连接结构体
    MYSQL my_connection;
    // 声明查询结果状态变量
    int res;

    // 初始化MySQL连接
    mysql_init(&my_connection);
    // 尝试连接到MySQL服务器
    if (mysql_real_connect(&my_connection, "localhost", "rick", "secret", "foo", 0, NULL, 0))
    {
        // 打印连接成功信息
        printf("连接成功\n");

        // 执行更新操作
        res = mysql_query(&my_connection, "UPDATE children SET AGE = 4 WHERE fname = 'Ann'");
        // 检查更新操作是否成功
        if (!res)
        {
            // 打印更新的行数
            printf("更新了%lu行\n", (unsigned long)mysql_affected_rows(&my_connection));
        }
        else
        {
            // 打印更新错误信息
            fprintf(stderr, "更新错误 %d: %s\n", mysql_errno(&my_connection), mysql_error(&my_connection));
        }

        // 关闭MySQL连接
        mysql_close(&my_connection);
    }
    else
    {
        // 打印连接失败信息
        fprintf(stderr, "连接失败\n");
        // 如果有连接错误,打印错误详情
        if (mysql_errno(&my_connection))
        {
            fprintf(stderr, "连接错误 %d: %s\n", mysql_errno(&my_connection), mysql_error(&my_connection));
        }
    }

    // 返回程序执行成功状态码
    return EXIT_SUCCESS;
}
