// 引入标准库
#include <stdlib.h>
#include <stdio.h>

// 引入MySQL客户端库
#include "mysql.h"

// 定义全局变量,用于存储MySQL连接
MYSQL my_connection;
// 定义全局变量,用于存储查询结果
MYSQL_RES *res_ptr;
// 定义全局变量,用于存储行数据
MYSQL_ROW sqlrow;

// 函数声明,用于显示表头
void display_header();
// 函数声明,用于显示行数据
void display_row();

int main(int argc, char *argv[])
{
  int res;
  int first_row = 1; // 用于确保在成功检索数据时只显示一次行头

  // 初始化MySQL连接
  mysql_init(&my_connection);

  // 尝试连接到MySQL服务器
  if (mysql_real_connect(&my_connection, "localhost", "rick",
                         "secret", "foo", 0, NULL, 0))
  {
    printf("连接成功\n"); // 打印连接成功信息

    // 执行查询语句
    res = mysql_query(&my_connection, "SELECT childno, fname, age FROM children WHERE age > 5");

    // 检查查询是否成功
    if (res)
    {
      fprintf(stderr, "查询错误: %s\n", mysql_error(&my_connection)); // 打印查询错误信息
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
          // 如果是第一行,则显示表头
          if (first_row)
          {
            display_header();
            first_row = 0; // 更新标志,避免重复显示表头
          }
          // 显示当前行的数据
          display_row();
        }

        // 检查MySQL连接是否有错误
        if (mysql_errno(&my_connection))
        {
          fprintf(stderr, "检索错误: %s\n",
                  mysql_error(&my_connection)); // 打印检索错误信息
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

    // 检查MySQL连接是否有错误码
    if (mysql_errno(&my_connection))
    {
      // 打印具体的连接错误信息
      fprintf(stderr, "连接错误 %d: %s\n",
              mysql_errno(&my_connection),
              mysql_error(&my_connection));
    }
  }

  return EXIT_SUCCESS;
}

// 显示表头信息的函数
void display_header()
{
  // 定义一个指向MYSQL_FIELD结构体的指针
  MYSQL_FIELD *field_ptr;

  // 打印列详情的标题
  printf("列详细信息:\n");

  // 循环遍历结果集中的每个字段
  while ((field_ptr = mysql_fetch_field(res_ptr)) != NULL)
  {
    // 打印字段名称
    printf("\t 名称: %s\n", field_ptr->name);

    // 打印字段类型
    printf("\t 类型: ");
    // 如果字段类型是数字
    if (IS_NUM(field_ptr->type))
    {
      printf("数字字段\n");
    }
    else
    {
      // 根据字段类型打印相应的信息
      switch (field_ptr->type)
      {
      // 如果字段类型是可变字符串,则打印"VARCHAR"
      case FIELD_TYPE_VAR_STRING:
        printf("VARCHAR\n"); // \n用于换行
        break;
      // 如果字段类型是长整型,则打印"LONG"
      case FIELD_TYPE_LONG:
        printf("LONG\n"); // \n用于换行
        break;
      // 如果字段类型不是预定义的类型,打印类型编号和提示信息
      default:
        // 打印类型编号,并提示用户检查mysql_com.h文件
        printf("类型是 %d, 请检查mysql_com.h\n", field_ptr->type); // \n用于换行
      } /* switch */
    } /* else */

    // 打印字段的最大宽度
    printf("\t 最大宽度 %ld\n", field_ptr->length); /* 注意:在MySQL 4.0之前的版本,格式应该是%d而不是%ld */

    // 如果字段有自动递增标志,打印提示信息
    if (field_ptr->flags & AUTO_INCREMENT_FLAG)
      printf("\t 自动递增\n");

    // 打印换行符以分隔不同字段的输出
    printf("\n");
  } /* while */
}

// 定义一个名为display_row的函数,用于显示数据库查询结果的行
void display_row()
{
  // 声明一个无符号整数变量field_count,用于记录当前处理的字段数量
  unsigned int field_count;

  // 初始化field_count为0,准备开始处理第一个字段
  field_count = 0;

  // 使用while循环,当field_count小于查询结果的字段总数时,继续处理每个字段
  while (field_count < mysql_field_count(&my_connection))
  {
    // 检查当前字段是否有值(非NULL)
    if (sqlrow[field_count])
      // 如果有值,则打印该字段的值,并在后面加一个空格
      printf("%s ", sqlrow[field_count]);
    else
      // 如果当前字段为NULL,则打印"NULL"字符串
      printf("NULL");

    // 字段处理完毕后,field_count自增1,准备处理下一个字段
    field_count++;
  }

  // 在所有字段处理完毕后,打印一个换行符,以便于阅读输出结果
  printf("\n");
}
