#include <stdlib.h>
#include <stdio.h>

// 引入MySQL库头文件
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

        // 执行插入操作
        res = mysql_query(&my_connection, "INSERT INTO children(fname, age) VALUES('Ann', 3)");
        // 检查插入操作是否成功
        if (!res)
        {
            // 打印插入成功的行数
            printf("插入了 %lu 行\n", (unsigned long)mysql_affected_rows(&my_connection));
        }
        else
        {
            // 打印插入错误信息
            fprintf(stderr, "插入错误 %d: %s\n", mysql_errno(&my_connection), mysql_error(&my_connection));
        }

        // 关闭MySQL连接
        mysql_close(&my_connection);
    }
    else
    {
        // 打印连接失败信息
        fprintf(stderr, "连接失败\n");
        // 如果有具体的错误码,则打印错误详情
        if (mysql_errno(&my_connection))
        {
            fprintf(stderr, "连接错误 %d: %s\n", mysql_errno(&my_connection), mysql_error(&my_connection));
        }
    }

    // 返回程序执行成功状态码
    return EXIT_SUCCESS;
}
