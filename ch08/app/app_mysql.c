// 引入标准库
/* 引入标准库函数 */
#include <stdlib.h> // 提供内存管理相关函数,如malloc, free等
#include <stdio.h>  // 提供输入输出相关函数,如printf, scanf等
#include <string.h> // 提供字符串处理相关函数,如strcpy, strcmp等

// 引入MySQL客户端库
#include "mysql.h"
// 引入应用程序MySQL模块头文件
#include "app_mysql.h"

// 定义一个静态的MYSQL结构体变量,用于维护数据库连接
static MYSQL my_connection;
// 定义一个静态整型变量,用于标记数据库是否已连接
static int dbconnected = 0;

// 声明一个静态函数,用于根据艺术家名字获取艺术家ID
static int get_artist_id(char *artist);

// 函数:启动数据库连接
// 参数:name - 用户名;pwd - 密码
// 返回值:1 - 连接成功;0 - 连接失败
int database_start(char *name, char *pwd)
{
  // 如果已经连接,直接返回成功
  if (dbconnected)
    return 1;

  // 初始化MySQL连接
  mysql_init(&my_connection);

  // 尝试连接数据库
  // 参数:"localhost" - 服务器地址;name - 用户名;pwd - 密码;"blpcd" - 数据库名;0 - 端口号;NULL - Unix套接字;0 - 客户端标志
  if (!mysql_real_connect(&my_connection, "localhost", name, pwd, "blpcd", 0, NULL, 0))
  {
    // 打印错误信息并返回失败
    fprintf(stderr, "数据库连接失败:%d, %s\n", mysql_errno(&my_connection), mysql_error(&my_connection));
    return 0;
  }

  // 标记为已连接
  dbconnected = 1;

  // 返回成功
  return 1;
} /* 数据库启动 */

// 函数:结束数据库连接
void database_end()
{
  // 如果已经连接,关闭连接
  if (dbconnected)
    mysql_close(&my_connection);

  // 标记为未连接
  dbconnected = 0;
} /* 数据库结束 */

// 函数add_cd用于向数据库中添加一个新的CD记录
int add_cd(char *artist, char *title, char *catalogue, int *cd_id)
{
  // 定义MySQL结果和行指针
  MYSQL_RES *res_ptr;
  MYSQL_ROW mysqlrow;

  // 定义返回值和中间变量
  int res;
  char is[250];       // 用于构建SQL插入语句
  char es[250];       // 用于存储转义后的标题
  int artist_id = -1; // 存储艺术家ID
  int new_cd_id = -1; // 存储新插入的CD ID

  // 检查数据库是否已连接
  if (!dbconnected)
    return 0;

  // 获取艺术家ID
  artist_id = get_artist_id(artist);

  // 转义标题字符串以防止SQL注入
  mysql_escape_string(es, title, strlen(title));

  // 构建插入CD记录的SQL语句
  sprintf(is, "INSERT INTO cd(title, artist_id, catalogue) VALUES('%s', %d, '%s')", es, artist_id, catalogue);

  // 执行SQL插入操作
  res = mysql_query(&my_connection, is);
  if (res)
  {
    // 如果插入失败,打印错误信息并返回0
    fprintf(stderr, "Insert error %d: %s\n", mysql_errno(&my_connection), mysql_error(&my_connection));
    return 0;
  }

  // 查询最后插入行的ID
  res = mysql_query(&my_connection, "SELECT LAST_INSERT_ID()");
  if (res)
  {
    // 如果查询失败,打印错误信息并返回0
    printf("SELECT error: %s\n", mysql_error(&my_connection));
    return 0;
  }
  else
  {
    // 如果查询成功,获取结果集
    res_ptr = mysql_use_result(&my_connection);
    if (res_ptr)
    {
      // 从结果集中获取一行数据
      if ((mysqlrow = mysql_fetch_row(res_ptr)))
      {
        // 解析出新插入的CD ID
        sscanf(mysqlrow[0], "%d", &new_cd_id);
      }
      // 释放结果集资源
      mysql_free_result(res_ptr);
    }
    // 将新插入的CD ID赋值给输出参数
    *cd_id = new_cd_id;

    // 如果新插入的CD ID有效,则返回1,否则返回0
    if (new_cd_id != -1)
      return 1;
    return 0;
  }
} /* add_cd */

/* 定义一个函数,用于根据给定的字符串查找或创建一个artist_id */
static int get_artist_id(char *artist)
{
  // 定义MySQL结果集和行指针
  MYSQL_RES *res_ptr;
  MYSQL_ROW mysqlrow;

  // 定义查询和插入语句的字符串
  int res;
  // 定义三个字符数组,用于存储不同类型的查询字符串
  // qs 用于存储查询字符串
  char qs[250];
  // is 用于存储插入语句的字符串
  char is[250];

  /* 定义一个字符数组es,用于存储最多250个字符 */
  char es[250];
  // 初始化artist_id为-1,表示尚未找到或创建
  int artist_id = -1;

  // 检查artist是否已经存在于数据库中
  mysql_escape_string(es, artist, strlen(artist));            // 转义字符串以防止SQL注入
  sprintf(qs, "SELECT id FROM artist WHERE name = '%s'", es); // 构造查询语句

  // 执行查询
  res = mysql_query(&my_connection, qs);
  if (res)
  {
    // 查询出错时打印错误信息
    fprintf(stderr, "SELECT error: %s\n", mysql_error(&my_connection));
  }
  else
  {
    // 获取查询结果
    res_ptr = mysql_store_result(&my_connection);
    if (res_ptr)
    {
      // 如果有匹配的行
      if (mysql_num_rows(res_ptr) > 0)
      {
        // 读取第一行的数据
        if (mysqlrow = mysql_fetch_row(res_ptr))
        {
          // 从结果中解析出artist_id
          sscanf(mysqlrow[0], "%d", &artist_id);
        }
      }
      // 释放结果集
      mysql_free_result(res_ptr);
    }
  }
  // 如果找到了artist_id,直接返回
  if (artist_id != -1)
    return artist_id;

  // 如果没有找到,则插入新记录
  sprintf(is, "INSERT INTO artist(name) VALUES('%s')", es); // 构造插入语句
  res = mysql_query(&my_connection, is);
  if (res)
  {
    // 插入出错时打印错误信息
    fprintf(stderr, "Insert error %d: %s\n", mysql_errno(&my_connection), mysql_error(&my_connection));
    return 0;
  }
  // 查询最后插入行的ID
  res = mysql_query(&my_connection, "SELECT LAST_INSERT_ID()");
  if (res)
  {
    // 查询出错时打印错误信息
    printf("SELECT error: %s\n", mysql_error(&my_connection));
    return 0;
  }
  else
  {
    // 获取查询结果
    res_ptr = mysql_use_result(&my_connection);
    if (res_ptr)
    {
      // 读取第一行的数据
      if ((mysqlrow = mysql_fetch_row(res_ptr)))
      {
        // 从结果中解析出artist_id
        sscanf(mysqlrow[0], "%d", &artist_id);
      }
      // 释放结果集
      mysql_free_result(res_ptr);
    }
  }
  // 返回找到或创建的artist_id
  return artist_id;
} /* get_artist_id */

// 函数add_tracks用于向数据库中添加曲目信息
int add_tracks(struct current_tracks_st *tracks)
{
  // 定义结果变量res,用于存储SQL查询的结果
  int res;
  // 定义两个字符数组is和es,用于构建SQL语句和转义字符串
  char is[250];
  char es[250];
  // 定义索引变量i,用于遍历tracks结构体中的曲目数组
  int i;

  // 检查数据库是否已连接,如果没有连接则返回0
  if (!dbconnected)
    return 0;

  // 初始化索引i为0
  i = 0;
  // 使用while循环遍历tracks结构体中的曲目数组
  while (tracks->track[i][0])
  {
    // 使用mysql_escape_string函数对曲目名称进行转义,以防止SQL注入攻击
    mysql_escape_string(es, tracks->track[i], strlen(tracks->track[i]));
    // 构建SQL插入语句,将曲目信息插入到数据库的track表中
    sprintf(is, "INSERT INTO track(cd_id, track_id, title) VALUES(%d, %d, '%s')", tracks->cd_id, i + 1, es);
    // 执行SQL查询,并将结果存储在res变量中
    res = mysql_query(&my_connection, is);
    // 如果查询执行失败,打印错误信息并返回0
    if (res)
    {
      fprintf(stderr, "Insert error %d: %s\n", mysql_errno(&my_connection), mysql_error(&my_connection));
      return 0;
    }
    // 索引i自增,继续处理下一首曲目
    i++;
  }
  // 如果所有曲目都成功插入,返回1
  return 1;
} /* add_tracks */

// get_cd函数实现
int get_cd(int cd_id, struct current_cd_st *dest)
{
  // 定义MySQL查询结果指针和行数据指针
  // 定义一个MYSQL_RES类型的指针,用于存储从MySQL数据库查询返回的结果集
  MYSQL_RES *res_ptr;

  // 定义一个MYSQL_ROW类型的变量,用于存储结果集中的单行数据
  MYSQL_ROW mysqlrow;

  // 定义查询结果的整数变量和查询语句字符串
  int res;
  char qs[250];

  // 检查数据库是否已连接,未连接则返回0
  if (!dbconnected)
    return 0;

  // 清空目标结构体的内容,并将artist_id设置为-1作为未找到的标记
  memset(dest, 0, sizeof(*dest));
  dest->artist_id = -1;

  // 构造SQL查询语句,使用参数化查询以避免SQL注入风险
  // 使用sprintf函数构造一个SQL查询语句
  // 查询的表格是artist和cd,需要获取的信息包括艺术家的id,cd的id,艺术家的名字,cd的标题和cd的目录编号
  // 条件是artist表中的id与cd表中的artist_id相等,且cd的id等于给定的cd_id
  sprintf(qs, "SELECT artist.id, cd.id, artist.name, cd.title, cd.catalogue FROM artist, cd WHERE artist.id = cd.artist_id and cd.id = %d", cd_id);

  // 执行SQL查询
  res = mysql_query(&my_connection, qs);

  // 如果查询出错,打印错误信息
  if (res)
  {
    fprintf(stderr, "SELECT error: %s\n", mysql_error(&my_connection));
  }
  else
  {
    // 获取查询结果集
    res_ptr = mysql_store_result(&my_connection);

    // 如果有结果集
    if (res_ptr)
    {
      // 如果结果集中有行数据
      if (mysql_num_rows(res_ptr) > 0)
      {
        // 获取第一行数据
        if (mysqlrow = mysql_fetch_row(res_ptr))
        {
          // 从行数据中解析出所需信息并填充到结构体中
          // 从MySQL查询结果中提取数据并填充到dest结构体中
          // mysqlrow数组包含了从数据库查询返回的每一行数据的字段值

          // 将mysqlrow[0]中的字符串转换为整数,并存储到dest的artist_id成员变量中
          sscanf(mysqlrow[0], "%d", &dest->artist_id);

          // 将mysqlrow[1]中的字符串转换为整数,并存储到dest的cd_id成员变量中
          sscanf(mysqlrow[1], "%d", &dest->cd_id);

          // 将mysqlrow[2]中的字符串复制到dest的artist_name成员变量中
          strcpy(dest->artist_name, mysqlrow[2]);

          // 将mysqlrow[3]中的字符串复制到dest的title成员变量中
          strcpy(dest->title, mysqlrow[3]);

          // 将mysqlrow[4]中的字符串复制到dest的catalogue成员变量中
          strcpy(dest->catalogue, mysqlrow[4]);
        }
      }
      // 释放结果集内存
      mysql_free_result(res_ptr);
    }
  }
  // 如果artist_id不为-1,表示找到了对应的CD信息,返回1;否则返回0
  if (dest->artist_id != -1)
    return 1;
  return 0;
} /* get_cd */

// 函数实现:get_cd_tracks
int get_cd_tracks(int cd_id, struct current_tracks_st *dest)
{
  MYSQL_RES *res_ptr; // 用于存储查询结果
  MYSQL_ROW mysqlrow; // 用于遍历查询结果的每一行

  int res;                   // 存储查询操作的返回值
  char qs[250];              // 用于构造SQL查询语句的缓冲区
  int i = 0, num_tracks = 0; // i用于计数音轨,num_tracks存储查询到的音轨数量

  // 检查数据库是否已连接,未连接则返回0
  if (!dbconnected)
    return 0;

  // 初始化目标结构体
  memset(dest, 0, sizeof(*dest));
  dest->cd_id = -1;

  // 构造SQL查询语句,查询指定CD ID的所有音轨
  sprintf(qs, "SELECT track_id, title FROM track WHERE track.cd_id = %d ORDER BY track_id", cd_id);

  // 执行查询操作
  res = mysql_query(&my_connection, qs);
  if (res)
  {
    // 查询出错时打印错误信息
    fprintf(stderr, "SELECT error: %s\n", mysql_error(&my_connection));
  }
  else
  {
    // 获取查询结果集
    res_ptr = mysql_store_result(&my_connection);
    if (res_ptr)
    {
      // 获取结果集中的行数
      if ((num_tracks = mysql_num_rows(res_ptr)) > 0)
      {
        // 遍历结果集的每一行
        while (mysqlrow = mysql_fetch_row(res_ptr))
        {
          // 将音轨标题复制到目标结构体中
          strcpy(dest->track[i], mysqlrow[1]);
          i++;
        }
        // 设置目标结构体的CD ID
        dest->cd_id = cd_id;
      }
      // 释放结果集占用的内存
      mysql_free_result(res_ptr);
    }
  }
  // 返回查询到的音轨数量
  return num_tracks;
} /* get_cd_tracks */

// 函数声明find_cds,接收一个搜索字符串和一个指向cd_search_st结构体的指针
int find_cds(char *search_str, struct cd_search_st *dest)
{
  MYSQL_RES *res_ptr; // 定义MYSQL_RES类型的指针,用于存储查询结果
  MYSQL_ROW mysqlrow; // 定义MYSQL_ROW类型的变量,用于遍历查询结果的每一行

  int res;          // 定义整型变量res,用于存储mysql_query的返回值
  char qs[500];     // 定义字符数组qs,用于存储构造的SQL查询语句
  int i = 0;        // 定义整型变量i,用于记录查询结果的数量
  char ss[250];     // 定义字符数组ss,用于存储转义后的搜索字符串
  int num_rows = 0; // 定义整型变量num_rows,用于存储查询结果的行数

  // 检查数据库是否已连接,如果未连接则返回0
  if (!dbconnected)
    return 0;
  // 将dest结构体的内存设置为-1,用于初始化结果
  memset(dest, -1, sizeof(*dest));
  // 使用mysql_escape_string函数对搜索字符串进行转义处理,防止SQL注入攻击
  mysql_escape_string(ss, search_str, strlen(search_str));

  // 构造SQL查询语句,使用LIKE操作符进行模糊匹配
  sprintf(qs, "SELECT DISTINCT artist.id, cd.id FROM artist, cd WHERE artist.id = cd.artist_id and (artist.name LIKE '%%%s%%' OR cd.title LIKE '%%%s%%' OR cd.catalogue LIKE '%%%s%%')", ss, ss, ss);

  // 执行SQL查询语句,将返回值存储在res中
  res = mysql_query(&my_connection, qs);
  // 如果查询出错,打印错误信息
  if (res)
  {
    fprintf(stderr, "SELECT error: %s\n", mysql_error(&my_connection));
  }
  else
  {
    // 获取查询结果集
    res_ptr = mysql_store_result(&my_connection);
    // 如果结果集不为空
    if (res_ptr)
    {
      // 获取结果集的行数
      num_rows = mysql_num_rows(res_ptr);
      // 如果结果集的行数大于0
      if (num_rows > 0)
      {
        // 遍历结果集的每一行
        while ((mysqlrow = mysql_fetch_row(res_ptr)) && i < MAX_CD_RESULT)
        {
          // 从结果集中提取cd的ID,并存储到dest结构体中
          sscanf(mysqlrow[1], "%d", &dest->cd_id[i]);
          i++; // 更新结果数量
        }
      }
      // 释放结果集占用的内存
      mysql_free_result(res_ptr);
    }
  }
  // 返回查询结果的行数
  return num_rows;
} /* find_cds */

// 删除CD的函数
int delete_cd(int cd_id)
{
  int res;                 // 用于存储SQL查询的结果状态
  char qs[250];            // 用于构造SQL查询语句的缓冲区
  int artist_id, num_rows; // artist_id用于存储艺术家ID,num_rows用于存储查询结果的行数
  MYSQL_RES *res_ptr;      // 指向查询结果集的指针
  MYSQL_ROW mysqlrow;      // 指向查询结果行的指针

  // 检查数据库是否已连接
  if (!dbconnected)
    return 0;

  artist_id = -1; // 初始化artist_id为-1,表示尚未找到对应的艺术家ID

  // 构造查询语句,检查是否有其他CD使用相同的艺术家ID
  sprintf(qs, "SELECT artist_id FROM cd WHERE artist_id = (SELECT artist_id FROM cd WHERE id = '%d')", cd_id);
  res = mysql_query(&my_connection, qs); // 执行查询
  if (res)                               // 如果查询出错
  {
    fprintf(stderr, "SELECT error: %s\n", mysql_error(&my_connection)); // 输出错误信息
  }
  else // 查询成功
  {
    res_ptr = mysql_store_result(&my_connection); // 存储查询结果
    if (res_ptr)                                  // 如果有结果集
    {
      num_rows = mysql_num_rows(res_ptr); // 获取结果集的行数
      if (num_rows == 1)                  // 如果只有一行结果,说明没有其他CD使用相同的艺术家ID
      {
        mysqlrow = mysql_fetch_row(res_ptr);   // 获取结果行的数据
        sscanf(mysqlrow[0], "%d", &artist_id); // 从结果中解析出artist_id
      }
      mysql_free_result(res_ptr); // 释放结果集占用的内存
    }
  }

  // 构造并执行删除音轨的SQL语句
  sprintf(qs, "DELETE FROM track WHERE cd_id = '%d'", cd_id);
  res = mysql_query(&my_connection, qs);
  if (res) // 如果删除出错
  {
    fprintf(stderr, "Delete error (track) %d: %s\n", mysql_errno(&my_connection), mysql_error(&my_connection)); // 输出错误信息
    return 0;                                                                                                   // 返回0表示删除失败
  }

  // 构造并执行删除CD的SQL语句
  sprintf(qs, "DELETE FROM cd WHERE id = '%d'", cd_id);
  res = mysql_query(&my_connection, qs);
  if (res) // 如果删除出错
  {
    fprintf(stderr, "Delete error (cd) %d: %s\n", mysql_errno(&my_connection), mysql_error(&my_connection)); // 输出错误信息
    return 0;                                                                                                // 返回0表示删除失败
  }

  // 如果artist_id不为-1,说明没有其他CD使用该艺术家ID,可以删除艺术家记录
  if (artist_id != -1)
  {
    sprintf(qs, "DELETE FROM artist WHERE id = '%d'", artist_id);
    res = mysql_query(&my_connection, qs);
    if (res) // 如果删除出错
    {
      fprintf(stderr, "Delete error (artist) %d: %s\n", mysql_errno(&my_connection), mysql_error(&my_connection)); // 输出错误信息
    }
  }

  return 1; // 删除成功,返回1
} /* delete_cd */
