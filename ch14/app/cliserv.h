/* 首先,我们包含了所需的头文件.这些头文件为程序提供了基本的系统调用和数据类型定义. */
#include <unistd.h>     // 提供对POSIX操作系统API的访问
#include <stdlib.h>     // 提供内存分配,随机数生成等功能
#include <stdio.h>      // 提供输入输出函数
#include <fcntl.h>      // 提供文件控制函数
#include <limits.h>     // 定义了一些常量,如INT_MAX
#include <sys/types.h>  // 定义了系统数据类型
#include <sys/stat.h>   // 提供了文件状态信息

/* 然后我们定义了命名管道.服务器使用一个管道,每个客户端使用一个管道.
   由于可能有多个客户端,客户端在管道名称中加入了进程ID以确保其管道的唯一性. */
#define SERVER_PIPE  "/tmp/server_pipe"     // 服务器管道名称
#define CLIENT_PIPE  "/tmp/client_%d_pipe"  // 客户端管道名称模板,%d将被进程ID替换

#define ERR_TEXT_LEN 80  // 错误文本的最大长度

/* 我们使用枚举类型来实现命令,而不是使用#define.
   这样做可以让编译器进行更多的类型检查,并且在调试应用程序时也有帮助,
   因为许多调试器能够显示枚举常量的名称,但无法显示由#define指令定义的名称.
   第一个typedef定义了发送到服务器的请求类型,第二个定义了服务器对客户端的响应类型. */
// 定义客户端请求类型枚举
typedef enum
{
    s_create_new_database,  // 创建新数据库的请求
    s_get_cdc_entry,        // 获取cdc条目的请求
    s_get_cdt_entry,        // 获取cdt条目的请求
    s_add_cdc_entry,        // 添加cdc条目的请求
    s_add_cdt_entry,        // 添加cdt条目的请求
    s_del_cdc_entry,        // 删除cdc条目的请求
    s_del_cdt_entry,        // 删除cdt条目的请求
    s_find_cdc_entry        // 查找cdc条目的请求
} client_request_e;

// 定义服务器响应枚举类型
typedef enum
{
    r_success = 0,  // 表示操作成功
    r_failure,      // 表示操作失败
    r_find_no_more  // 表示查找操作没有更多结果
} server_response_e;

/* 接下来,我们声明了一个结构体,它将构成在两个进程之间传递的消息.
   由于我们实际上不需要在同一个响应中同时返回cdc_entry和cdt_entry,我们可以将它们合并为一个联合体.
   为了简单起见,我们保持它们分开,这样代码也更容易维护. */
// 定义消息数据库结构体
typedef struct
{
    pid_t             client_pid;                    // 客户端进程ID
    client_request_e  request;                       // 客户端请求类型
    server_response_e response;                      // 服务器响应类型
    cdc_entry         cdc_entry_data;                // cdc(Change Data Capture)条目数据
    cdt_entry         cdt_entry_data;                // cdt(Change Data Table)条目数据
    char              error_text[ERR_TEXT_LEN + 1];  // 错误文本,用于存储错误信息
} message_db_t;

/* 最后,我们得到了在pipe_imp.c中实现的管道接口函数,这些函数执行数据传输.
   它们分为服务器端和客户端两部分函数,分别在第一和第二块中. */
int  server_starting(void);                                  // 服务器启动函数
void server_ending(void);                                    // 服务器结束函数
int  read_request_from_client(message_db_t *rec_ptr);        // 从客户端读取请求函数
int  start_resp_to_client(const message_db_t mess_to_send);  // 开始向客户端发送响应函数
int  send_resp_to_client(const message_db_t mess_to_send);   // 向客户端发送响应函数
void end_resp_to_client(void);                               // 结束向客户端发送响应函数

int  client_starting(void);                           // 客户端启动函数
void client_ending(void);                             // 客户端结束函数
int  send_mess_to_server(message_db_t mess_to_send);  // 向服务器发送消息函数
int  start_resp_from_server(void);                    // 开始从服务器接收响应函数
int  read_resp_from_server(message_db_t *rec_ptr);    // 从服务器读取响应函数
void end_resp_from_server(void);                      // 结束从服务器接收响应函数
