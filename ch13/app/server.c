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
    // 服务器信号处理设置
    if ((sigaction(SIGINT, &new_action, &old_action) != 0) ||  // 设置SIGINT信号的处理函数
        (sigaction(SIGHUP, &new_action, &old_action) != 0) ||  // 设置SIGHUP信号的处理函数
        (sigaction(SIGTERM, &new_action, &old_action) != 0))   // 设置SIGTERM信号的处理函数
    {
        // 如果任一信号捕捉设置失败
        // 输出错误信息到标准错误流
        fprintf(stderr, "服务器启动错误,信号捕捉失败\n");
        // 退出程序,返回失败状态码
        exit(EXIT_FAILURE);
    }

    // 解析命令行参数,判断是否需要初始化数据库
    // 检查命令行参数的数量是否大于1
    if (argc > 1)
    {
        // 移动到第二个参数
        argv++;
        // 检查当前参数是否以"-i"开头
        if (strncmp("-i", *argv, 2) == 0)
            // 如果是,设置数据库初始化类型为1
            database_init_type = 1;
    }

    // 初始化数据库
    // 初始化数据库,如果初始化失败,则输出错误信息并退出程序
    if (!database_initialize(database_init_type))
    {
        // 输出错误信息到标准错误流
        fprintf(stderr, "服务器错误:无法初始化数据库\n");
        // 退出程序,返回失败状态码
        exit(EXIT_FAILURE);
    }

    // 启动服务器
    // 如果服务器启动失败,则退出程序并返回失败状态码
    // 函数:server_starting()
    // 返回值:布尔值,表示服务器是否成功启动
    if (!server_starting())
        exit(EXIT_FAILURE);  // 退出程序,返回EXIT_FAILURE

    // 服务器运行循环
    while (server_running)
    {
        // 从客户端读取请求
        // 函数功能:尝试从客户端读取请求,如果成功则处理请求,失败则输出错误信息并停止服务器运行
        // 参数:&mess_command - 用于存储从客户端读取的请求信息的变量地址
        // 返回值:成功返回非零值,失败返回零
        if (read_request_from_client(&mess_command))
        {
            // 处理请求
            // 函数功能:处理从客户端读取到的请求
            // 参数:mess_command - 从客户端读取到的请求信息
            process_command(mess_command);
        }
        else
        {
            // 读取请求失败,输出错误信息并停止服务器运行
            // 如果服务器正在运行,输出错误信息到标准错误流
            if (server_running)
                fprintf(stderr, "服务器结束 - 无法读取管道\n");
            // 设置服务器运行状态为停止
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
    // 设置响应状态为成功
    resp.response = r_success;
    // 清空错误文本数组,确保没有残留的错误信息
    memset(resp.error_text, '\0', sizeof(resp.error_text));
    // 初始化保存错误编号的变量,用于记录可能发生的错误
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
            // 检查响应中的目录条目是否不为0,即是否存在条目
            if (resp.cdc_entry_data.catalog[0] != 0)
            {
                // 如果找到条目,设置响应状态为成功
                resp.response = r_success;
                // 尝试向客户端发送响应
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
