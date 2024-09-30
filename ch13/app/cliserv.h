/* 引入必要的头文件. */

#include <unistd.h>     // 标准UNIX库,提供读写,进程控制等函数
#include <stdlib.h>     // 提供内存分配和释放等功能
#include <stdio.h>      // 提供输入输出功能
#include <fcntl.h>      // 提供文件控制选项
#include <limits.h>     // 提供数值限制
#include <sys/types.h>  // 提供数据类型定义
#include <sys/stat.h>   // 提供文件状态信息

/*
We then define the named pipes. We use one pipe for the server and one pipe for each client.
Since there may be multiple clients, the client incorporates a process ID into the name to ensure that its pipe is unique.
*/

/* 定义命名管道.我们使用一个管道用于服务器,每个客户端使用一个管道.
由于可能有多个客户端,客户端在管道名称中包含进程ID以确保其管道的唯一性.*/

#define SERVER_PIPE  "/tmp/server_pipe"     // 服务器管道的路径
#define CLIENT_PIPE  "/tmp/client_%d_pipe"  // 客户端管道的路径模板,%d将被替换为进程ID

#define ERR_TEXT_LEN 80  // 错误文本的最大长度

/*
 We implement the commands as enumerated types, rather than #defines.
 This is a good way of allowing the compiler to do more type checking and also helps
 in debugging the application, as many debuggers are able to show the name of enumerated
 constants, but not the name defined by a #define directive.
 The first typedef gives the type of request being sent to the server, the second
 the server response to the client.
*/

/* 我们将命令实现为枚举类型,而不是#define.
这是允许编译器进行更多类型检查的好方法,也有助于调试应用程序,因为许多调试器能够显示枚举常量的名称,但不能显示由#define指令定义的名称.
第一个typedef给出了发送到服务器的请求类型,第二个给出了服务器对客户端的响应.*/

typedef enum
{
    s_create_new_database = 0,  // 创建新数据库
    s_get_cdc_entry,            // 获取cdc条目
    s_get_cdt_entry,            // 获取cdt条目
    s_add_cdc_entry,            // 添加cdc条目
    s_add_cdt_entry,            // 添加cdt条目
    s_del_cdc_entry,            // 删除cdc条目
    s_del_cdt_entry,            // 删除cdt条目
    s_find_cdc_entry            // 查找cdc条目
} client_request_e;

typedef enum
{
    r_success = 0,  // 成功
    r_failure,      // 失败
    r_find_no_more  // 查找完毕,没有更多结果
} server_response_e;

/*  Next, we declare a structure that will form the message passed in both directions between the two processes.

Since we don't actually need to return both a cdc_entry and cdt_entry in the same response, we could have combined them in a union. For simplicity, we keep them separate.  This also makes the code easier to maintain.
*/

/* 接下来,我们声明一个结构体,它将构成在两个进程之间双向传递的消息.

由于我们实际上不需要在同一响应中返回cdc_entry和cdt_entry,我们可以将它们组合在一个联合体中.为了简单起见,我们保持它们分开.这也使代码更易于维护.*/

typedef struct
{
    pid_t             client_pid;                    // 发送请求的客户端进程ID
    client_request_e  request;                       // 客户端请求的类型
    server_response_e response;                      // 服务器的响应类型
    cdc_entry         cdc_entry_data;                // cdc条目的数据
    cdt_entry         cdt_entry_data;                // cdt条目的数据
    char              error_text[ERR_TEXT_LEN + 1];  // 错误信息的文本
} message_db_t;

/* 最后,我们到达了实现数据传输的管道接口函数,这些函数在pipe_imp.c中实现.
这些分为服务器端和客户端函数,分别在第一块和第二块中.*/

int  server_starting(void);                                  // 服务器启动时调用的函数
void server_ending(void);                                    // 服务器结束时调用的函数
int  read_request_from_client(message_db_t *rec_ptr);        // 从客户端读取请求的函数
int  start_resp_to_client(const message_db_t mess_to_send);  // 开始向客户端发送响应的函数
int  send_resp_to_client(const message_db_t mess_to_send);   // 向客户端发送响应的函数
void end_resp_to_client(void);                               // 结束向客户端发送响应的函数

int  client_starting(void);                           // 客户端启动时调用的函数
void client_ending(void);                             // 客户端结束时调用的函数
int  send_mess_to_server(message_db_t mess_to_send);  // 向服务器发送消息的函数
int  start_resp_from_server(void);                    // 开始从服务器接收响应的函数
int  read_resp_from_server(message_db_t *rec_ptr);    // 从服务器读取响应的函数
void end_resp_from_server(void);                      // 结束从服务器接收响应的函数
