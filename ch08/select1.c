// 引入标准库,用于内存管理和输入输出操作
#include <stdlib.h>
#include <stdio.h>

// 引入MySQL客户端库,用于连接和操作MySQL数据库
#include "mysql.h"

// 定义MySQL连接结构体
MYSQL my_connection;
// 定义结果集指针
MYSQL_RES *res_ptr;
// 定义行数据指针
MYSQL_ROW sqlrow;

int main(int argc, char *argv[])
{
  int res;

  // 初始化MySQL连接
  mysql_init(&my_connection);
  // 连接数据库,如果连接成功则打印"连接成功"
  if (mysql_real_connect(&my_connection, "localhost", "rick", "secret", "foo", 0, NULL, 0))
  {
    printf("连接成功\n");

    // 执行查询语句,查询年龄大于5岁的孩子的信息
    res = mysql_query(&my_connection, "SELECT childno, fname, age FROM children WHERE age > 5");

    // 检查查询是否成功,若失败则打印错误信息
    if (res)
    {
      printf("查询错误: %s\n", mysql_error(&my_connection));
    }
    else
    {
      // 获取查询结果
      res_ptr = mysql_store_result(&my_connection);
      // 如果结果集非空,则打印获取到的行数
      if (res_ptr)
      {
        printf("检索到 %lu 行\n", (unsigned long)mysql_num_rows(res_ptr));
        // 遍历结果集中的每一行
        while ((sqlrow = mysql_fetch_row(res_ptr)))
        {
          printf("获取数据中...\n");
          // 此处可以添加打印或处理每一行数据的代码
        }
        // 检查MySQL连接的错误码,如果有错误则打印错误信息
        if (mysql_errno(&my_connection))
        {
          fprintf(stderr, "检索错误: %s\n", mysql_error(&my_connection));
        }
        // 释放结果集占用的内存
        mysql_free_result(res_ptr);
      }
    }
    // 关闭MySQL连接
    mysql_close(&my_connection);
  }
  else
  {
    // 如果连接失败,打印"连接失败"
    fprintf(stderr, "连接失败\n");
    // 检查MySQL连接的错误码,如果有错误则打印错误信息
    if (mysql_errno(&my_connection))
    {
      fprintf(stderr, "连接错误 %d: %s\n", mysql_errno(&my_connection), mysql_error(&my_connection));
    }
  }

  return EXIT_SUCCESS;
}
