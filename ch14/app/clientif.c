// 以下代码包含了必要的头文件和常量定义.

// 使用 POSIX 标准
#define _POSIX_SOURCE

// 包含标准库头文件
#include <unistd.h>     // 提供对 POSIX 操作系统的访问,如 fork(), exec(), pipe() 等
#include <stdlib.h>     // 提供通用工具函数,如 malloc(), free(), exit() 等
#include <stdio.h>      // 提供标准输入输出功能,如 printf(), scanf(), fopen() 等
#include <string.h>     // 提供字符串处理函数,如 strcpy(), strcmp(), strlen() 等
#include <fcntl.h>      // 提供文件控制操作,如 open(), close(), fcntl() 等
#include <limits.h>     // 定义了各种数据类型的限制,如 INT_MAX, PATH_MAX 等
#include <sys/types.h>  // 定义了系统数据类型,如 pid_t, size_t 等
#include <sys/stat.h>   // 提供文件状态信息,如 stat(), fstat() 等

// 包含项目特定的头文件
#include "cd_data.h"  // 假设包含 CD 数据的定义
#include "cliserv.h"  // 假设包含客户端/服务器交互的定义

/*
 * 静态变量mypid减少了原本所需的getpid调用次数.
 * 我们使用一个局部函数read_one_response来消除重复代码.
 */
static pid_t mypid;

/*
 * read_one_response函数的声明,用于读取单个响应.
 * 参数rec_ptr是指向message_db_t结构体的指针.
 * 返回值类型为int.
 */
static int read_one_response(message_db_t *rec_ptr);

/*
 * database_initialize和close例程仍然被调用,但现在分别用于初始化管道接口的客户端部分
 * 和在客户端退出时移除多余的命名管道.
 */

/**
 * @brief 初始化数据库客户端
 *
 * 该函数用于初始化数据库客户端的管道接口.
 *
 * @param new_database 新的数据库标识符
 * @return 如果客户端正在启动则返回1,否则返回0
 */
// 初始化数据库
// @param new_database 表示是否是新数据库的标志
// @return 如果客户端启动成功并且获取到进程ID,则返回1,否则返回0
int database_initialize(const int new_database)
{
    // 检查客户端是否正在启动
    if (!client_starting())
        return (0);    // 如果客户端没有启动,返回0
    mypid = getpid();  // 获取当前进程ID
    return (1);        // 成功获取进程ID,返回1
} /* database_initialize */

// 关闭数据库连接
// 该函数调用client_ending()来结束客户端连接
void database_close(void)
{
    client_ending();  // 结束客户端连接
}

/*
The get_cdc_entry routine is called to get a catalog entry from the database,
given a CD catalog title.
Here we encode the request in a message_db_t structure and pass it to the server.
We then read the response back into a different message_db_t structure.
If an entry is found, it's included inside the message_db_t structure as a cdc_entry
structure, so we pass back the appropriate part of the structure.
*/

/*
get_cdc_entry 函数用于从数据库中获取目录条目,
给定一个 CD 目录标题.
在这里,我们将请求编码在一个 message_db_t 结构中,并将其传递给服务器.
然后,我们将响应重新读取到一个不同的 message_db_t 结构中.
如果找到一个条目,它将被包含在 message_db_t 结构中作为一个 cdc_entry 结构,
所以我们返回该结构中的适当部分.
*/

// 获取CDC条目的函数
// 参数: cd_catalog_ptr - 指向CDC目录字符串的指针
// 返回: cdc_entry结构体,包含请求的CDC条目信息或错误信息
cdc_entry get_cdc_entry(const char *cd_catalog_ptr)
{
    cdc_entry    ret_val;    // 用于存储返回的CDC条目
    message_db_t mess_send;  // 发送给服务器的消息结构
    message_db_t mess_ret;   // 从服务器接收的消息结构

    ret_val.catalog[0] = '\0';  // 初始化返回的CDC条目的目录字符串为空

    // 填充发送消息结构
    mess_send.client_pid = mypid;                              // 客户端进程ID
    mess_send.request    = s_get_cdc_entry;                    // 请求类型
    strcpy(mess_send.cdc_entry_data.catalog, cd_catalog_ptr);  // 复制CDC目录字符串到消息结构中

    // 发送消息到服务器
    if (send_mess_to_server(mess_send))
    {
        // 读取服务器的响应
        if (read_one_response(&mess_ret))
        {
            // 检查响应是否成功
            if (mess_ret.response == r_success)
            {
                ret_val = mess_ret.cdc_entry_data;  // 成功则复制响应中的CDC条目数据
            }
            else
            {
                fprintf(stderr, "%s", mess_ret.error_text);  // 失败则打印错误信息
            }
        }
        else
        {
            fprintf(stderr, "Server failed to respond\n");  // 服务器无响应
        }
    }
    else
    {
        fprintf(stderr, "Server not accepting requests\n");  // 服务器不接受请求
    }
    return (ret_val);  // 返回CDC条目或错误信息
}

/* 该函数用于从服务器读取一个响应,以避免代码重复. */

/**
 * @brief 从服务器读取一个响应.
 *
 * @param rec_ptr 指向消息数据库的指针.
 * @return int 返回代码,如果成功读取响应则返回1,否则返回0.
 */
static int read_one_response(message_db_t *rec_ptr)
{
    /* 此变量存储函数的返回代码. */
    int return_code = 0;
    /* 如果指向消息数据库的指针为空,返回0. */
    if (!rec_ptr)
        return (0);

    /* 如果函数start_resp_from_server()返回1, */
    if (start_resp_from_server())
    {
        /* 并且函数read_resp_from_server()也返回1, */
        if (read_resp_from_server(rec_ptr))
        {
            /* 设置返回代码为1. */
            return_code = 1;
        }
        /* 调用函数end_resp_from_server()来结束响应. */
        end_resp_from_server();
    }
    /* 返回返回代码. */
    return (return_code);
}

/* 从文件: clientif.c */

/* 首先,检索CD曲目的函数. */

/**
 * @brief 获取CD曲目信息
 *
 * @param cd_catalog_ptr CD目录指针
 * @param track_no 曲目编号
 * @return cdt_entry 返回CD曲目信息结构体
 */
cdt_entry get_cdt_entry(const char *cd_catalog_ptr, const int track_no)
{
    cdt_entry    ret_val;    // 返回值
    message_db_t mess_send;  // 发送的消息结构体
    message_db_t mess_ret;   // 接收的消息结构体

    ret_val.catalog[0]   = '\0';                               // 初始化返回值的目录为空字符串
    mess_send.client_pid = mypid;                              // 设置发送消息的客户端进程ID
    mess_send.request    = s_get_cdt_entry;                    // 设置请求类型为获取CD曲目信息
    strcpy(mess_send.cdt_entry_data.catalog, cd_catalog_ptr);  // 复制CD目录到发送消息结构体
    mess_send.cdt_entry_data.track_no = track_no;              // 设置曲目编号

    if (send_mess_to_server(mess_send))  // 发送消息到服务器
    {
        if (read_one_response(&mess_ret))  // 读取服务器响应
        {
            if (mess_ret.response == r_success)  // 如果响应成功
            {
                ret_val = mess_ret.cdt_entry_data;  // 将响应中的曲目信息复制到返回值
            }
            else  // 如果响应失败
            {
                fprintf(stderr, "%s", mess_ret.error_text);  // 输出错误信息
            }
        }
        else  // 如果服务器没有响应
        {
            fprintf(stderr, "Server failed to respond\n");  // 输出错误信息
        }
    }
    else  // 如果服务器不接受请求
    {
        fprintf(stderr, "Server not accepting requests\n");  // 输出错误信息
    }
    return (ret_val);  // 返回CD曲目信息
}

/* 添加CDC条目到目录的函数.
 *
 * @param entry_to_add 要添加的CDC条目结构体.
 * @return 成功返回1,失败返回0.
 */
int add_cdc_entry(const cdc_entry entry_to_add)
{
    message_db_t mess_send;  // 发送给服务器的消息结构体
    message_db_t mess_ret;   // 从服务器接收的消息结构体

    // 填充发送消息的字段
    mess_send.client_pid     = mypid;            // 客户端进程ID
    mess_send.request        = s_add_cdc_entry;  // 请求类型
    mess_send.cdc_entry_data = entry_to_add;     // 要添加的CDC条目数据

    // 尝试发送消息到服务器
    if (send_mess_to_server(mess_send))
    {
        // 尝试读取服务器的响应
        if (read_one_response(&mess_ret))
        {
            // 检查响应是否表示成功
            if (mess_ret.response == r_success)
            {
                return (1);  // 成功添加条目
            }
            else
            {
                // 打印错误信息
                fprintf(stderr, "%s", mess_ret.error_text);
            }
        }
        else
        {
            // 服务器没有响应
            fprintf(stderr, "Server failed to respond\n");
        }
    }
    else
    {
        // 服务器不接受请求
        fprintf(stderr, "Server not accepting requests\n");
    }
    return (0);  // 添加条目失败
}

// add_cdt_entry 函数用于向服务器添加一个CDT条目.
// 参数:
//   entry_to_add: 要添加的CDT条目结构体.
// 返回值:
//   成功返回1,失败返回0,并在stderr输出错误信息.
int add_cdt_entry(const cdt_entry entry_to_add)
{
    // 定义发送给服务器的消息结构体
    message_db_t mess_send;
    // 定义从服务器接收到的消息结构体
    message_db_t mess_ret;

    // 填充发送消息的字段
    mess_send.client_pid     = mypid;            // 当前客户端进程ID
    mess_send.request        = s_add_cdt_entry;  // 请求类型为添加CDT条目
    mess_send.cdt_entry_data = entry_to_add;     // 要添加的CDT条目数据

    // 尝试发送消息到服务器
    if (send_mess_to_server(mess_send))
    {
        // 尝试读取服务器的一个响应
        if (read_one_response(&mess_ret))
        {
            // 检查响应是否成功
            if (mess_ret.response == r_success)
            {
                return (1);  // 成功添加条目
            }
            else
            {
                // 响应失败,输出错误信息
                fprintf(stderr, "%s", mess_ret.error_text);
            }
        }
        else
        {
            // 服务器未能响应
            fprintf(stderr, "Server failed to respond\n");
        }
    }
    else
    {
        // 服务器不接受请求
        fprintf(stderr, "Server not accepting requests\n");
    }
    return (0);  // 添加条目失败
}

/* 删除CDC条目的函数. */

/**
 * @brief 删除CDC条目
 *
 * 该函数负责构建删除请求消息,并将其发送到服务器.如果服务器成功处理请求,
 * 则返回1表示删除成功;否则,打印错误信息并返回0.
 *
 * @param cd_catalog_ptr 指向CDC目录条目的指针
 * @return int 返回1表示删除成功,返回0表示删除失败
 */
int del_cdc_entry(const char *cd_catalog_ptr)
{
    message_db_t mess_send;  // 发送的消息结构体
    message_db_t mess_ret;   // 接收的消息结构体

    mess_send.client_pid = mypid;                              // 设置客户端进程ID
    mess_send.request    = s_del_cdc_entry;                    // 设置请求类型为删除CDC条目
    strcpy(mess_send.cdc_entry_data.catalog, cd_catalog_ptr);  // 复制CDC目录条目数据

    if (send_mess_to_server(mess_send))  // 发送消息到服务器
    {
        if (read_one_response(&mess_ret))  // 读取服务器响应
        {
            if (mess_ret.response == r_success)  // 如果响应成功
            {
                return (1);  // 返回删除成功
            }
            else  // 如果响应失败
            {
                fprintf(stderr, "%s", mess_ret.error_text);  // 打印错误信息
            }
        }
        else  // 如果服务器没有响应
        {
            fprintf(stderr, "Server failed to respond\n");  // 打印服务器未响应的错误信息
        }
    }
    else  // 如果服务器不接受请求
    {
        fprintf(stderr, "Server not accepting requests\n");  // 打印服务器不接受请求的错误信息
    }
    return (0);  // 返回删除失败
}

// del_cdt_entry 函数用于从CD目录中删除指定轨道号的条目
// 参数:
//   cd_catalog_ptr: 指向CD目录的指针
//   track_no: 要删除的轨道号
// 返回值:
//   成功删除返回1,否则返回0
int del_cdt_entry(const char *cd_catalog_ptr, const int track_no)
{
    message_db_t mess_send;  // 发送给服务器的消息结构体
    message_db_t mess_ret;   // 从服务器接收的响应消息结构体

    // 填充发送消息结构体
    mess_send.client_pid = mypid;                              // 客户端进程ID
    mess_send.request    = s_del_cdt_entry;                    // 请求类型
    strcpy(mess_send.cdt_entry_data.catalog, cd_catalog_ptr);  // 复制CD目录
    mess_send.cdt_entry_data.track_no = track_no;              // 设置轨道号

    // 发送消息到服务器
    if (send_mess_to_server(mess_send))
    {
        // 读取服务器的响应
        if (read_one_response(&mess_ret))
        {
            // 检查响应是否成功
            if (mess_ret.response == r_success)
            {
                return (1);  // 成功删除
            }
            else
            {
                fprintf(stderr, "%s", mess_ret.error_text);  // 打印错误信息
            }
        }
        else
        {
            fprintf(stderr, "Server failed to respond\n");  // 服务器无响应
        }
    }
    else
    {
        fprintf(stderr, "Server not accepting requests\n");  // 服务器不接受请求
    }
    return (0);  // 删除失败
}
/*
 * 函数功能:根据CD目录指针搜索CDC条目
 * 参数:
 *   cd_catalog_ptr: 指向CD目录的指针
 *   first_call_ptr: 指示是否为第一次调用的指针
 * 返回值:返回一个cdc_entry结构体,包含搜索结果
 *
 * 该函数看起来比较复杂,因为它调用了三个管道函数:
 * send_mess_to_server, start_resp_from_server 和 read_resp_from_server.
 */

cdc_entry search_cdc_entry(const char *cd_catalog_ptr, int *first_call_ptr)
{
    message_db_t mess_send;  // 发送给服务器的消息结构体
    message_db_t mess_ret;   // 从服务器接收的消息结构体

    static FILE *work_file        = (FILE *)0;  // 工作文件指针,用于存储匹配的CDC条目
    static int   entries_matching = 0;          // 匹配的条目数量
    cdc_entry    ret_val;                       // 返回的CDC条目结构体

    ret_val.catalog[0] = '\0';  // 初始化返回的CDC条目目录为空字符串

    // 如果工作文件未打开且为第一次调用,则直接返回空结果
    if (!work_file && (*first_call_ptr == 0))
        return (ret_val);

    // 第一次调用的处理逻辑
    if (*first_call_ptr)
    {
        *first_call_ptr = 0;  // 立即将first_call_ptr设置为false
        if (work_file)
            fclose(work_file);  // 关闭之前的工作文件
        work_file = tmpfile();  // 创建一个新的临时文件作为工作文件
        if (!work_file)
            return (ret_val);  // 如果文件创建失败,返回空结果

        // 初始化客户端消息结构体
        // 设置消息发送结构体中的客户端进程ID为当前进程ID
        mess_send.client_pid = mypid;

        // 设置消息发送结构体中的请求类型为查找CDC条目
        mess_send.request = s_find_cdc_entry;

        // 将CD目录指针指向的数据复制到消息发送结构体的CDC条目数据目录字段中
        strcpy(mess_send.cdc_entry_data.catalog, cd_catalog_ptr);

        // 发送消息到服务器并等待响应
        if (send_mess_to_server(mess_send))
        {
            if (start_resp_from_server())
            {
                // 循环读取服务器响应
                while (read_resp_from_server(&mess_ret))
                {
                    if (mess_ret.response == r_success)
                    {
                        // 如果响应成功,将匹配的CDC条目写入工作文件
                        fwrite(&mess_ret.cdc_entry_data, sizeof(cdc_entry), 1, work_file);
                        entries_matching++;  // 增加匹配条目计数
                    }
                    else
                    {
                        break;  // 如果响应不成功,跳出循环
                    }
                } /* while */
            }
            else
            {
                fprintf(stderr, "Server not responding\n");  // 服务器无响应
            }
        }
        else
        {
            fprintf(stderr, "Server not accepting requests\n");  // 服务器不接受请求
        }

        // 检查是否有匹配的条目
        if (entries_matching == 0)
        {
            fclose(work_file);      // 关闭工作文件
            work_file = (FILE *)0;  // 重置工作文件指针
            return (ret_val);       // 返回空结果
        }
        (void)fseek(work_file, 0L, SEEK_SET);  // 将工作文件指针移到文件开头
    }
    else
    {
        // 非第一次调用的处理逻辑
        if (entries_matching == 0)
        {
            fclose(work_file);      // 如果没有匹配的条目,关闭工作文件
            work_file = (FILE *)0;  // 重置工作文件指针
            return (ret_val);       // 返回空结果
        }
    }

    // 从工作文件中读取下一个匹配的CDC条目
    fread(&ret_val, sizeof(cdc_entry), 1, work_file);
    entries_matching--;  // 减少匹配条目计数

    return (ret_val);  // 返回读取到的CDC条目
}
