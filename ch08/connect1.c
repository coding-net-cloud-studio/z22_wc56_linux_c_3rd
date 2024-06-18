// 引入标准库,用于内存管理和输入输出操作
#include <stdlib.h>
#include <stdio.h>

// 引入MySQL客户端库,用于连接MySQL数据库并执行SQL查询
#include "mysql.h"

int main(int argc, char *argv[])
{
  // 声明MySQL连接指针
  MYSQL *conn_ptr;

  // 初始化MySQL连接
  conn_ptr = mysql_init(NULL);
  // 检查初始化是否成功,如果失败则打印错误信息并退出程序
  // 检查conn_ptr是否为NULL,确保数据库连接初始化成功
  if (!conn_ptr)
  {
    // 如果初始化失败,打印错误信息到标准错误流
    fprintf(stderr, "mysql_init失败\n"); // 注意这里应该有一个换行符\n
    // 返回EXIT_FAILURE表示程序执行失败
    return EXIT_FAILURE;
  }

  // 尝试连接到MySQL服务器,参数包括连接指针,服务器地址,用户名,密码,数据库名等
  conn_ptr = mysql_real_connect(conn_ptr, "localhost", "rick", "secret",
                                "foo", 0, NULL, 0);
  // 检查连接是否成功,如果成功则打印成功信息,否则打印失败信息
  // 检查conn_ptr是否为非空指针
  if (conn_ptr)
  {
    // 如果conn_ptr非空,说明连接成功,打印成功信息
    printf("连接成功\n"); // 注意这里需要换行符\n
  }
  else
  {
    // 如果conn_ptr为空,说明连接失败,打印失败信息
    printf("连接失败\n"); // 注意这里需要换行符\n
  }

  // 关闭MySQL连接
  mysql_close(conn_ptr);

  // 返回程序执行成功状态码
  return EXIT_SUCCESS;
}
