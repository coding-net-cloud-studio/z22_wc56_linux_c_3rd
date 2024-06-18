#include <stdlib.h>  // 引入标准库,提供内存分配和释放等功能
#include <stdio.h>   // 引入标准输入输出库,用于printf等函数

// 引入MySQL头文件,用于连接MySQL数据库并执行SQL查询
#include "mysql.h"

int main(int argc, char *argv[])
{
    // 初始化MySQL连接结构体
    MYSQL my_connection;

    // 对连接结构体进行初始化
    mysql_init(&my_connection);

    // 尝试连接到MySQL服务器
    // 参数分别是:连接结构体指针,服务器地址,用户名,密码,数据库名,端口号(0表示默认),Unix套接字(NULL表示默认),客户端标志(0表示默认)
    if (mysql_real_connect(&my_connection, "localhost", "rick", "I do not know", "foo", 0, NULL, 0))
    {
        // 如果连接成功,打印成功信息
        printf("连接成功\n");

        // 关闭数据库连接
        mysql_close(&my_connection);
    }
    else
    {
        // 如果连接失败,打印失败信息到标准错误输出
        fprintf(stderr, "连接失败\n");

        // 检查MySQL的错误码,如果不是0,则打印具体的错误信息
        if (mysql_errno(&my_connection))
        {
            fprintf(stderr, "连接错误 %d: %s\n", mysql_errno(&my_connection), mysql_error(&my_connection));
        }
    }

    // 返回程序执行成功的标识
    return EXIT_SUCCESS;
}
