/* Some global variables, a prototype for process_command and a signal catcher to ensure a clean exit. */

/* 一些全局变量,process_command 的原型,以及一个信号捕获器以确保干净的退出.*/

// 引入unistd头文件,提供对POSIX操作系统API的访问,如read(), write()等
#include <unistd.h>
// 引入stdlib头文件,提供通用的函数,如exit(), malloc()等
#include <stdlib.h>
// 引入stdio头文件,提供输入输出函数,如printf(), scanf()等
#include <stdio.h>
// 引入fcntl头文件,提供文件控制选项,如O_RDONLY, O_WRONLY等
#include <fcntl.h>
// 引入limits头文件,提供数值限制,如INT_MAX, PATH_MAX等
#include <limits.h>
// 引入signal头文件,提供信号处理功能
#include <signal.h>
// 引入string头文件,提供字符串处理函数,如strcpy(), strcmp()等
#include <string.h>
// 引入errno头文件,提供错误码定义
#include <errno.h>
// 引入sys/types头文件,提供基本系统数据类型
#include <sys/types.h>
// 引入sys/stat头文件,提供文件状态信息
#include <sys/stat.h>

// 引入自定义的cd_data.h头文件
#include "cd_data.h"
// 引入自定义的cliserv.h头文件
#include "cliserv.h"

// 定义一个全局变量save_errno,用于存储错误编号
int save_errno;

// 定义一个静态变量server_running,用于指示服务器是否正在运行
static int server_running = 1;

// 声明一个处理命令的函数原型
static void process_command(const message_db_t mess_command);

// 定义一个函数catch_signals,用于捕获信号并设置服务器停止运行的标志
void catch_signals()
{
    // 当接收到信号时,将server_running设置为0,表示服务器停止运行
    server_running = 0;
}

/*
Now we come to the main function.

After checking that the signal catching routines,
the program checks to see whether you passed -i on the command line.

If you did, it will create a new database.
If all is well and the server is running,
any requests from the client are fed to the process_command function
that we'll meet in a moment.

*/

/*
现在我们来到主函数.

在检查信号捕获程序之后,
程序会检查你是否在命令行中传递了-i参数.

如果你传递了-i,程序将会创建一个新的数据库.
如果一切正常且服务器正在运行,
来自客户端的任何请求都会被传递给process_command函数,
我们稍后会遇到这个函数.

*/

int main(int argc, char *argv[])
{
    // 定义信号动作结构体
    struct sigaction new_action, old_action;
    // 定义消息数据库结构体
    message_db_t mess_command;
    // 定义数据库初始化类型变量
    int database_init_type = 0;

    // 设置信号处理函数
    new_action.sa_handler = catch_signals;
    sigemptyset(&new_action.sa_mask);
    new_action.sa_flags = 0;
    // 捕获SIGINT,SIGHUP,SIGTERM信号
    if ((sigaction(SIGINT, &new_action, &old_action) != 0) ||
        (sigaction(SIGHUP, &new_action, &old_action) != 0) ||
        (sigaction(SIGTERM, &new_action, &old_action) != 0))
    {
        // 信号捕捉失败,输出错误信息并退出程序
        fprintf(stderr, "服务器启动错误,信号捕捉失败\n");
        exit(EXIT_FAILURE);
    }

    // 解析命令行参数,判断是否需要初始化数据库
    if (argc > 1)
    {
        argv++;
        if (strncmp("-i", *argv, 2) == 0)
            database_init_type = 1;
    }
    // 初始化数据库
    if (!database_initialize(database_init_type))
    {
        // 数据库初始化失败,输出错误信息并退出程序
        fprintf(stderr, "服务器错误:无法初始化数据库\n");
        exit(EXIT_FAILURE);
    }

    // 启动服务器
    if (!server_starting())
        exit(EXIT_FAILURE);

    // 服务器运行循环
    while (server_running)
    {
        // 从客户端读取请求
        if (read_request_from_client(&mess_command))
        {
            // 处理请求
            process_command(mess_command);
        }
        else
        {
            // 读取请求失败,输出错误信息并停止服务器运行
            if (server_running)
                fprintf(stderr, "服务器结束 - 无法读取管道\n");
            server_running = 0;
        }
    } /* while */
    // 服务器结束处理
    server_ending();
    // 程序正常退出
    exit(EXIT_SUCCESS);
}

/*
  Any client messages are fed to the process_command function,
  where they are fed into a case statement that makes the appropriate calls to cd_dbm.c. */

/*
任何客户端消息都会被传送到process_command函数,
在这个函数中,它们会被送入一个case语句,该语句会调用cd_dbm.c中相应的函数.*/
// 定义处理命令的函数,输入参数为一个消息数据库结构体
static void process_command(const message_db_t comm)
{
    // 定义响应消息结构体
    message_db_t resp;
    // 定义一个标志位,用于在查找CDC条目时判断是否为第一次
    int first_time = 1;

    // 将输入的命令复制到响应结构体中,以便后续修改
    resp = comm;

    // 开始向客户端发送响应
    if (!start_resp_to_client(resp))
    {
        // 如果开始发送响应失败,打印警告信息并返回
        fprintf(stderr, "服务器警告:start_resp_to_client %d 失败\n", resp.client_pid);
        return;
    }

    // 设置响应状态为成功,并清空错误文本
    resp.response = r_success;
    memset(resp.error_text, '\0', sizeof(resp.error_text));
    // 初始化保存错误编号的变量
    save_errno = 0;

    // 根据请求类型进行相应处理
    switch (resp.request)
    {
    case s_create_new_database:
        // 创建新数据库
        if (!database_initialize(1))
            resp.response = r_failure;
        break;
    case s_get_cdc_entry:
        // 获取CDC条目
        resp.cdc_entry_data = get_cdc_entry(comm.cdc_entry_data.catalog);
        break;
    case s_get_cdt_entry:
        // 获取CDT条目
        resp.cdt_entry_data = get_cdt_entry(comm.cdt_entry_data.catalog, comm.cdt_entry_data.track_no);
        break;
    case s_add_cdc_entry:
        // 添加CDC条目
        if (!add_cdc_entry(comm.cdc_entry_data))
            resp.response = r_failure;
        break;
    case s_add_cdt_entry:
        // 添加CDT条目
        if (!add_cdt_entry(comm.cdt_entry_data))
            resp.response = r_failure;
        break;
    case s_del_cdc_entry:
        // 删除CDC条目
        if (!del_cdc_entry(comm.cdc_entry_data.catalog))
            resp.response = r_failure;
        break;
    case s_del_cdt_entry:
        // 删除CDT条目
        if (!del_cdt_entry(comm.cdt_entry_data.catalog, comm.cdt_entry_data.track_no))
            resp.response = r_failure;
        break;
    case s_find_cdc_entry:
        // 查找CDC条目
        do
        {
            // 从通信结构体中获取cdc_entry_data的目录信息
            resp.cdc_entry_data = search_cdc_entry(comm.cdc_entry_data.catalog, &first_time);
            if (resp.cdc_entry_data.catalog[0] != 0)
            {
                // 如果找到条目,设置响应状态为成功,并向客户端发送响应
                resp.response = r_success;
                if (!send_resp_to_client(resp))
                {
                    // 如果发送响应失败,打印警告信息并跳出循环
                    fprintf(stderr, "服务器警告:向 %d 发送响应失败\n", resp.client_pid);
                    break;
                }
            }
            else
            {
                // 如果没有找到更多条目,设置响应状态为没有更多
                resp.response = r_find_no_more;
            }
        } while (resp.response == r_success);
        break;
    default:
        // 如果请求类型未知,设置响应状态为失败
        resp.response = r_failure;
        break;
    }  // switch

    // 根据保存的错误编号生成错误文本
    sprintf(resp.error_text, "命令失败:\t%s\n", strerror(save_errno));

    // 向客户端发送最终的响应
    if (!send_resp_to_client(resp))
    {
        // 如果发送响应失败,打印警告信息
        fprintf(stderr, "服务器警告:向 %d 发送响应失败\n", resp.client_pid);
    }

    // 结束向客户端发送响应
    end_resp_to_client();
    // 函数返回
    return;
}
