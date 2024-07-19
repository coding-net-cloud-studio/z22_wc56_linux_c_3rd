/* Some global variables, a prototype for process_command and a signal catcher to ensure a clean exit. */

// 导入所需的头文件
#include <unistd.h>     // 提供对POSIX操作系统API的访问
#include <stdlib.h>     // 提供许多用于程序初始化和管理的函数
#include <stdio.h>      // 提供输入/输出函数
#include <fcntl.h>      // 提供文件控制函数
#include <limits.h>     // 定义了一些常量,这些常量限制了一些数值类型的大小
#include <signal.h>     // 提供信号处理功能
#include <string.h>     // 提供字符串操作函数
#include <errno.h>      // 定义了许多错误标志,当调用系统API出错时会设置相应的标志
#include <sys/types.h>  // 提供了各种基本系统数据类型
#include <sys/stat.h>   // 提供文件状态信息

// 导入自定义的头文件
#include "cd_data.h"  // 可能包含CD数据相关的结构和函数
#include "cliserv.h"  // 可能包含客户端/服务器通信相关的结构和函数

// 定义一个全局变量save_errno,用于保存错误码
int save_errno;

// 定义一个静态变量server_running,表示服务器是否正在运行,1为运行状态
static int server_running = 1;

// 声明一个静态函数process_command,用于处理接收到的命令消息
// 参数mess_command是一个message_db_t类型的常量指针,指向命令消息
static void process_command(const message_db_t mess_command);

// 定义一个catch_signals函数,用于捕获信号
void catch_signals()
{
    // 当信号被捕获时,将server_running设置为0,表示服务器停止运行
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

/*!SECTION
现在我们来谈谈主函数.

在检查了信号捕捉例程之后,程序会检查您是否在命令行上传递了-i.

如果您这样做了,它将创建一个新的数据库.
如果一切顺利并且服务器正在运行,来自客户端的任何请求都会被送到我们稍后会遇到的process_command函数.

*/

int main(int argc, char *argv[])
{
    // 定义信号处理结构体
    struct sigaction new_action, old_action;

    // 定义消息数据库类型
    message_db_t mess_command;

    // 定义数据库初始化类型,默认为0
    int database_init_type = 0;

    // 设置新的信号处理函数为catch_signals
    new_action.sa_handler = catch_signals;

    // 清空信号掩码集合,意味着目前不阻塞任何信号
    sigemptyset(&new_action.sa_mask);

    // 设置信号处理标志,0表示默认行为,没有特殊标志
    new_action.sa_flags = 0;

    if ((sigaction(SIGINT, &new_action, &old_action) != 0) ||
        (sigaction(SIGHUP, &new_action, &old_action) != 0) ||
        (sigaction(SIGTERM, &new_action, &old_action) != 0))
    {
        fprintf(stderr, "Server startup error, signal catching failed\n");
        exit(EXIT_FAILURE);
    }

    // 检查命令行参数数量是否超过1,即是否有额外的参数传入
    if (argc > 1)
    {
        // 如果有额外参数,将argv指针移动到第一个额外参数的位置
        argv++;
        // 使用strncmp函数比较当前参数是否以"-i"开头
        // 如果以"-i"开头,database_init_type被设置为1
        if (strncmp("-i", *argv, 2) == 0)
            database_init_type = 1;
    }

    // 尝试初始化数据库,如果失败则打印错误信息并退出程序
    if (!database_initialize(database_init_type))
    {
        fprintf(stderr, "Server error: 无法初始化数据库\n");
        exit(EXIT_FAILURE);
    }

    // 尝试启动服务器,如果失败则退出程序
    if (!server_starting())
        exit(EXIT_FAILURE);

    // 服务器主循环,控制服务器运行状态
    while (server_running)
    {
        // 尝试从客户端读取请求
        if (read_request_from_client(&mess_command))
        {
            // 如果读取成功,则处理命令
            process_command(mess_command);
        }
        else
        {
            // 如果读取失败,检查服务器是否应继续运行
            if (server_running)
                // 输出错误信息到标准错误流
                fprintf(stderr, "Server ended - can not read pipe\n");
            // 设置服务器运行状态为停止
            server_running = 0;
        }
    } /* while */

    // 调用服务器结束处理函数,确保所有资源被正确释放
    server_ending();
    // 退出程序并返回成功状态码
    exit(EXIT_SUCCESS);
}

/*
  Any client messages are fed to the process_command function,

  where they are fed into a case statement that makes the appropriate calls to cd_dbm.c. */

/*
任何客户端的消息都被送入process_command函数中,

在那里它们被送入一个case语句中,该语句调用cd_dbm.c中的适当函数. */

// 定义处理命令的函数,接收一个消息数据库类型的参数
static void process_command(const message_db_t comm)
{
    // 声明一个消息数据库类型的变量用于响应
    message_db_t resp;
    // 声明一个整型变量用于标记是否是第一次循环
    int first_time = 1;

    // 将传入的命令复制给响应变量,以便后续修改
    resp = comm;

    // 尝试开始向客户端发送响应,如果失败则打印警告并返回
    if (!start_resp_to_client(resp))
    {
        fprintf(stderr, "服务器警告:向客户端 %d 发送响应失败\n", resp.client_pid);
        return;
    }

    // 设置响应状态为成功
    resp.response = r_success;
    // 清空错误文本
    // 清空响应错误文本字段,确保没有遗留的字符
    memset(resp.error_text, '\0', sizeof(resp.error_text));

    // 保存当前的errno值,以便后续使用
    save_errno = errno;

    // 根据请求类型进行相应的处理
    switch (resp.request)
    {
    case s_create_new_database:       // 创建新数据库
        if (!database_initialize(1))  // 初始化数据库失败则响应失败
            resp.response = r_failure;
        break;
    case s_get_cdc_entry:  // 获取cdc条目
        resp.cdc_entry_data = get_cdc_entry(comm.cdc_entry_data.catalog);
        break;
    case s_get_cdt_entry:  // 获取cdt条目
                           // 获取cdt条目的函数,输入参数为目录和音轨号
        // 函数返回值赋给resp.cdt_entry_data
        resp.cdt_entry_data = get_cdt_entry(comm.cdt_entry_data.catalog, comm.cdt_entry_data.track_no);

        break;
    case s_add_cdc_entry:                         // 添加cdc条目
        if (!add_cdc_entry(comm.cdc_entry_data))  // 添加失败则响应失败
            resp.response = r_failure;
        break;
    case s_add_cdt_entry:                         // 添加cdt条目
        if (!add_cdt_entry(comm.cdt_entry_data))  // 添加失败则响应失败
            resp.response = r_failure;
        break;
    case s_del_cdc_entry:                                 // 删除cdc条目
        if (!del_cdc_entry(comm.cdc_entry_data.catalog))  // 删除失败则响应失败
            resp.response = r_failure;
        break;
    case s_del_cdt_entry:                                                               // 删除cdt条目
        if (!del_cdt_entry(comm.cdt_entry_data.catalog, comm.cdt_entry_data.track_no))  // 删除失败则响应失败
            resp.response = r_failure;
        break;
    case s_find_cdc_entry:  // 查找cdc条目
        do
        {
            // 查找cdc条目,如果是第一次则将first_time设为1
            // 定义一个函数search_cdc_entry,该函数接收两个参数:一个是cdc_entry的目录,另一个是指向布尔变量first_time的指针
            // 函数的返回值类型为resp.cdc_entry_data类型
            // 该函数的作用是在cdc_entry的目录中搜索相应的条目,并返回结果赋值给resp.cdc_entry_data
            // first_time变量用于标记是否是第一次搜索
            resp.cdc_entry_data = search_cdc_entry(comm.cdc_entry_data.catalog, &first_time);

            // 如果找到了条目
            if (resp.cdc_entry_data.catalog[0] != 0)
            {
                // 设置响应状态为成功
                resp.response = r_success;
                // 尝试向客户端发送响应,如果失败则打印警告并跳出循环
                if (!send_resp_to_client(resp))
                {
                    fprintf(stderr, "服务器警告:向客户端 %d 发送响应失败\n", resp.client_pid);
                    break;
                }
            }
            else
            {
                // 如果没有找到更多的条目,则设置响应状态为没有更多
                resp.response = r_find_no_more;
            }
        } while (resp.response == r_success);  // 当响应状态为成功时继续循环
        break;
    default:                        // 其他未处理的请求类型
        resp.response = r_failure;  // 设置响应状态为失败
        break;
    }  // switch语句结束

    // 根据保存的errno值生成错误文本
    sprintf(resp.error_text, "命令执行失败:\n\t%s\n", strerror(save_errno));

    // 尝试向客户端发送最终的响应,如果失败则打印警告
    if (!send_resp_to_client(resp))
    {
        fprintf(stderr, "服务器警告:向客户端 %d 发送响应失败\n", resp.client_pid);
    }

    // 结束向客户端发送响应的过程
    end_resp_to_client();
    // 函数返回
    return;
}
