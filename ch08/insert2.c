// 引入标准库<stdlib.h>,提供动态内存分配,随机数生成等功能
#include <stdlib.h>
// 引入标准输入输出库<stdio.h>,提供格式化输入输出功能
#include <stdio.h>

// 引入MySQL库头文件
#include "mysql.h"

int main(int argc, char *argv[])
{
  // 初始化MySQL连接结构体
  MYSQL my_connection;
  // 定义结果集指针
  MYSQL_RES *res_ptr;
  // 定义行数据指针
  MYSQL_ROW sqlrow;
  // 定义查询结果状态变量
  int res;

  // 对MySQL连接进行初始化
  mysql_init(&my_connection);
  // 尝试连接到MySQL服务器
  if (mysql_real_connect(&my_connection, "localhost",
                         "rick", "secret", "foo", 0, NULL, 0))
  {
    // 打印连接成功信息
    printf("连接成功\n");

    // 执行插入操作
    res = mysql_query(&my_connection, "INSERT INTO children(fname, age) VALUES('Robert', 7)");
    // 检查插入操作是否成功
    if (!res)
    {
      // 打印插入成功的行数
      printf("插入了 %lu 行\n", (unsigned long)mysql_affected_rows(&my_connection));
    }
    else
    {
      // 打印插入错误信息
      fprintf(stderr, "插入错误 %d: %s\n", mysql_errno(&my_connection),
              mysql_error(&my_connection));
    }

    // 查询最后插入行的ID
    res = mysql_query(&my_connection, "SELECT LAST_INSERT_ID()");

    // 检查查询操作是否成功
    if (res)
    {
      // 打印查询错误信息
      printf("查询错误: %s\n", mysql_error(&my_connection));
    }
    else
    {
      // 使用查询结果
      res_ptr = mysql_use_result(&my_connection);
      // 检查结果集是否有效
      if (res_ptr)
      {
        // 遍历结果集并打印每一行的数据
        while ((sqlrow = mysql_fetch_row(res_ptr)))
        {
          printf("我们插入了 childno %s\n", sqlrow[0]);
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
    // 打印连接失败信息
    fprintf(stderr, "连接失败\n");
    // 如果有具体的错误码,则打印错误详情
    if (mysql_errno(&my_connection))
    {
      fprintf(stderr, "连接错误 %d: %s\n",
              mysql_errno(&my_connection), mysql_error(&my_connection));
    }
  }

  // 返回程序执行成功状态码
  return EXIT_SUCCESS;
}
