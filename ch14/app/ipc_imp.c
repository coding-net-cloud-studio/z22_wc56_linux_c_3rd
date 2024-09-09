/* 首先,我们包含适当的头文件,声明一些消息队列键,并定义一个结构来保存我们的消息数据. */

#include "cd_data.h"  // 包含CD数据相关的头文件
#include "cliserv.h"  // 包含客户端服务器相关的头文件

#include <sys/msg.h>  // 包含系统消息队列相关的头文件

#define SERVER_MQUEUE 1234  // 定义服务器消息队列的键值
#define CLIENT_MQUEUE 4321  // 定义客户端消息队列的键值

/* 定义一个结构体来存储传递的消息数据 */
struct msg_passed
{
    long int     msg_key;       // 用于存储客户端进程ID
    message_db_t real_message;  // 存储实际的消息数据
};

/* 两个具有文件作用域的变量保存了从msgget函数返回的两个队列标识符. */

static int serv_qid = -1;  // 服务器消息队列的标识符
static int cli_qid  = -1;  // 客户端消息队列的标识符

/* 我们让服务器负责创建两个消息队列. */

/**
 * @brief 服务器启动函数,负责创建服务器和客户端的消息队列
 *
 * @return int 如果成功创建至少一个队列返回1,否则返回0
 */
int server_starting()
{
#if DEBUG_TRACE
    printf("%d :- server_starting()\n", getpid());  // 打印调试信息,显示服务器启动的进程ID
#endif

    serv_qid = msgget((key_t)SERVER_MQUEUE, 0666 | IPC_CREAT);  // 创建服务器消息队列
    if (serv_qid == -1)                                         // 如果创建失败
        return (0);

    cli_qid = msgget((key_t)CLIENT_MQUEUE, 0666 | IPC_CREAT);  // 创建客户端消息队列
    if (cli_qid == -1)                                         // 如果创建失败
        return (0);

    return (1);  // 如果两个队列都创建成功,返回1
}

/* 服务器在退出时也负责清理工作.当服务器结束时,
我们将文件作用域变量设置为非法值.如果服务器在调用server_ending后尝试发送消息,
这将捕获任何错误. */

/// @brief 结束服务器并清理IPC资源
///
/// 该函数在服务器结束时调用,负责移除消息队列并设置相关变量为非法值,
/// 以防止服务器在结束后的错误操作.
void server_ending()
{
#if DEBUG_TRACE
    printf("%d :- server_ending()\n", getpid());  // 打印当前进程ID和函数名,用于调试跟踪
#endif

    (void)msgctl(serv_qid, IPC_RMID, 0);  // 移除服务器消息队列
    (void)msgctl(cli_qid, IPC_RMID, 0);   // 移除客户端消息队列

    serv_qid = -1;  // 设置服务器消息队列ID为非法值
    cli_qid  = -1;  // 设置客户端消息队列ID为非法值
}

/* 服务器读取函数从队列中读取消息,该消息可以是任何类型(即来自任何客户端),
并返回消息的数据部分(忽略类型). */

/**
 * @brief 从客户端读取消息请求
 *
 * @param rec_ptr 指向接收消息的指针
 * @return int 成功读取返回1,失败返回0
 */
int read_request_from_client(message_db_t *rec_ptr)
{
    struct msg_passed my_msg;
#if DEBUG_TRACE
    printf("%d :- read_request_from_client()\n", getpid());
#endif

    // 使用msgrcv从消息队列中接收消息
    if (msgrcv(serv_qid, (void *)&my_msg, sizeof(*rec_ptr), 0, 0) == -1)
    {
        // 如果接收失败,返回0
        return (0);
    }
    // 将接收到的消息数据部分赋值给rec_ptr指向的结构体
    *rec_ptr = my_msg.real_message;
    // 成功读取,返回1
    return (1);
}

/* 发送响应使用在请求中存储的客户端进程ID来寻址消息. */

/**
 * @brief 向客户端发送响应消息
 *
 * @param mess_to_send 要发送的消息结构体
 * @return int 成功返回1,失败返回0
 */
int send_resp_to_client(const message_db_t mess_to_send)
{
    struct msg_passed my_msg;
#if DEBUG_TRACE
    printf("%d :- send_resp_to_client()\n", getpid());  // 调试信息:打印当前进程ID和函数名
#endif

    my_msg.real_message = mess_to_send;             // 将要发送的消息赋值给my_msg结构体的real_message字段
    my_msg.msg_key      = mess_to_send.client_pid;  // 将客户端进程ID赋值给my_msg结构体的msg_key字段

    if (msgsnd(cli_qid, (void *)&my_msg, sizeof(mess_to_send), 0) == -1)  // 发送消息到客户端队列
    {
        return (0);  // 发送失败返回0
    }
    return (1);  // 发送成功返回1
}

/* 当客户端启动时,它需要找到服务器和客户端的队列标识符.
客户端不创建队列.如果服务器没有运行,此函数将失败,
因为消息队列将不存在. */

// 客户端启动函数
int client_starting()
{
#if DEBUG_TRACE
    // 如果定义了DEBUG_TRACE,则打印当前进程ID和函数名
    printf("%d :- client_starting\n", getpid());
#endif

    // 获取服务器消息队列的标识符,权限设置为0666
    serv_qid = msgget((key_t)SERVER_MQUEUE, 0666);
    // 如果获取失败,返回0
    if (serv_qid == -1)
        return (0);

    // 获取客户端消息队列的标识符,权限设置为0666
    cli_qid = msgget((key_t)CLIENT_MQUEUE, 0666);
    // 如果获取失败,返回0
    if (cli_qid == -1)
        return (0);
    // 如果都成功获取,返回1
    return (1);
}

/* 当客户端结束时,我们设置文件作用域变量为非法值.
这将捕获客户端在调用client_ending后尝试发送消息的任何错误. */

/// <summary>
/// 客户端结束函数,用于设置文件作用域变量为非法值,以防止客户端在结束调用后发送消息.
/// </summary>
void client_ending()
{
#if DEBUG_TRACE
    printf("%d :- client_ending()\n", getpid());  // 如果定义了DEBUG_TRACE,则打印当前进程ID和函数名
#endif

    serv_qid = -1;  // 设置服务器队列ID为非法值
    cli_qid  = -1;  // 设置客户端队列ID为非法值
}

/* 发送消息到服务器,我们将数据存储在我们的结构体中.
 * 注意我们必须设置消息键.由于0是键的非法值,
 * 如果键未定义,它将取一个(表面上)随机的值,
 * 因此如果该值恰好为0,这个函数偶尔会失败. */

/**
 * @brief 发送消息到服务器
 *
 * @param mess_to_send 要发送的消息结构体
 * @return int 成功返回1,失败返回0
 */
int send_mess_to_server(message_db_t mess_to_send)
{
    struct msg_passed my_msg;
#if DEBUG_TRACE
    printf("%d :- send_mess_to_server()\n", getpid());  // 调试信息,打印当前进程ID和函数名
#endif

    my_msg.real_message = mess_to_send;             // 将要发送的消息赋值给结构体成员
    my_msg.msg_key      = mess_to_send.client_pid;  // 设置消息键为客户端进程ID

    // 尝试发送消息到服务器队列,如果失败则打印错误信息并返回0
    if (msgsnd(serv_qid, (void *)&my_msg, sizeof(mess_to_send), 0) == -1)
    {
        perror("Message send failed");
        return (0);
    }
    return (1);  // 发送成功返回1
}

/* 当客户端从服务器检索消息时,它使用自己的进程ID来接收仅针对自己的消息,忽略其他客户端的任何消息. */

/**
 * @brief 从服务器读取消息响应
 *
 * @param rec_ptr 指向接收消息的结构体的指针
 * @return int 成功返回1,失败返回0
 */
int read_resp_from_server(message_db_t *rec_ptr)
{
    struct msg_passed my_msg;
#if DEBUG_TRACE
    printf("%d :- read_resp_from_server()\n", getpid());  // 调试信息,打印当前进程ID和函数名
#endif

    // 使用msgrcv函数从消息队列接收消息,消息类型为当前进程ID
    if (msgrcv(cli_qid, (void *)&my_msg, sizeof(*rec_ptr), getpid(), 0) == -1)
    {
        return (0);  // 接收失败,返回0
    }
    *rec_ptr = my_msg.real_message;  // 将接收到的消息赋值给传入的指针指向的结构体
    return (1);                      // 接收成功,返回1
}

/* 为了与pipe_imp.c保持完全兼容,我们需要定义额外的四个函数.
   然而,在我们的新程序中,这些函数是空的.当使用管道时它们实现的操作现在不再需要了. */

/**
 * @brief 向客户端发送响应的开始函数
 *
 * @param mess_to_send 要发送的消息数据库结构体
 * @return int 返回值表示操作状态,1表示成功
 */
int start_resp_to_client(const message_db_t mess_to_send)
{
    return (1);  // 返回成功状态
}

/**
 * @brief 结束向客户端发送响应的函数
 */
void end_resp_to_client(void)
{
    // 空函数体,不需要实现任何操作
}

/**
 * @brief 从服务器接收响应的开始函数
 *
 * @return int 返回值表示操作状态,1表示成功
 */
int start_resp_from_server(void)
{
    return (1);  // 返回成功状态
}

/**
 * @brief 结束从服务器接收响应的函数
 */
void end_resp_from_server(void)
{
    // 空函数体,不需要实现任何操作
}
