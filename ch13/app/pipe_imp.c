/* Include files */

#include "cd_data.h"  // 包含CD数据相关的头文件
#include "cliserv.h"  // 包含客户端服务相关的头文件

/* define some values that we need in different functions within the file. */

/* 定义一些我们在文件的不同函数中需要使用的值. */

// NOTE wmtag_memo_下面是我自己加上去的_开始
// #define PATH_MAX 1023
// /usr/include/linux/limits.h 中定义 PAHT_MAX为4096
// NOTE wmtag_memo_下面是我自己加上去的_结束

// 服务器文件描述符,用于标识服务器端的套接字
static int server_fd = -1;

// 进程ID,用于标识当前进程
static pid_t mypid = 0;

// 客户端管道名称,用于存储客户端管道的名称
// 位于/usr/include/linux/limits.h:13 中定义了
static char client_pipe_name[PATH_MAX + 1] = {'\0'};

// 客户端文件描述符,用于标识客户端的套接字
static int client_fd = -1;

// 客户端写入文件描述符,用于标识客户端写入套接字的文件描述符
static int client_write_fd = -1;

/*
The server_starting routine creates the named pipe from which the server will read commands.
It then opens it for reading. This open will block until a client opens the pipe for writing.
We use a blocking mode, so that the server can perform blocking reads on the pipe while waiting for commands to be sent to it.
*/

/* 服务器启动函数创建服务器将从中读取命令的命名管道.
然后以读取模式打开它.
这个打开操作将阻塞,直到客户端以写入模式打开管道.
我们使用阻塞模式,
以便服务器可以在等待命令发送给它时对管道进行阻塞读取. */

int server_starting(void)
{
#if DEBUG_TRACE
    // 如果定义了DEBUG_TRACE宏,则打印当前进程ID和函数名,用于调试跟踪
    printf("%d :- server_starting()\n", getpid());
#endif

    // 删除已存在的服务器管道文件
    unlink(SERVER_PIPE);
    // 创建一个新的命名管道,权限为0777
    if (mkfifo(SERVER_PIPE, 0777) == -1)
    {
        // 如果创建失败,打印错误信息并返回0
        fprintf(stderr, "Server startup error, no FIFO created\n");
        return (0);
    }

    // 打开命名管道以读取数据
    if ((server_fd = open(SERVER_PIPE, O_RDONLY)) == -1)
    {
        // 如果打开失败,打印错误信息并返回0
        fprintf(stderr, "Server startup error, no FIFO opened\n");
        return (0);
    }
    // 成功启动服务器,返回1
    return (1);
}

/* When the server ends, it removes the named pipe, so clients can detect that no server is running. */

/* 当服务器结束时,它会移除命名管道,以便客户端可以检测到没有服务器在运行. */

void server_ending(void)
{
#if DEBUG_TRACE
    // 在调试跟踪模式下,打印当前进程ID和函数名
    printf("%d :- server_ending()\n", getpid());
#endif

    // 关闭服务器文件描述符
    (void)close(server_fd);
    // 删除服务器管道
    (void)unlink(SERVER_PIPE);
}

/* The read_request_from_client function, shown below, will block reading the server pipe until a client writes a message into it. */

/* 从客户端读取请求的函数,如下所示,将阻塞读取服务器管道,直到客户端写入消息为止. */

/**
 * @brief 从客户端读取请求
 *
 * 该函数尝试从服务器管道中读取一个请求消息,并将其存储在提供的消息数据库指针中.
 * 如果读取的字节数为0,表示连接已关闭,函数会尝试重新打开管道并再次读取.
 * 如果成功读取到完整的消息,则返回1,否则返回0.
 *
 * @param rec_ptr 指向消息数据库结构的指针,用于存储读取的消息
 * @return int 返回1表示成功读取到完整的消息,返回0表示读取失败或连接关闭
 */
int read_request_from_client(message_db_t *rec_ptr)
{
    int return_code = 0;  // 初始化返回代码
    int read_bytes;       // 用于存储实际读取的字节数

#if DEBUG_TRACE
    printf("%d :- read_request_from_client()\n", getpid());  // 调试信息,打印当前进程ID和函数名
#endif

    if (server_fd != -1)  // 检查服务器文件描述符是否有效
    {
        read_bytes = read(server_fd, rec_ptr, sizeof(*rec_ptr));  // 尝试从服务器管道读取数据
        /*
          In the special case when no clients have the pipe open for writing,
          the read will return 0, i.e.
          it detects an EOF.
          Then the server closes the pipe and opens it again,
          so that it blocks until a client also opens the pipe.
          This is just the same as when the server first starts;
          we have re-initialized the server.
          */

        /*
          在没有客户端打开管道进行写入的特殊情况下,
          读取操作将返回 0,即
          它检测到一个文件结束符.
          然后服务器关闭管道并再次打开它,
          这样它就会阻塞,直到客户端也打开管道.
          这与服务器首次启动时的情况相同;
          我们已经重新初始化了服务器.
          */

        if (read_bytes == 0)  // 如果读取的字节数为0,表示连接关闭
        {
            (void)close(server_fd);                               // 关闭当前的服务器文件描述符
            if ((server_fd = open(SERVER_PIPE, O_RDONLY)) == -1)  // 尝试重新打开服务器管道
            {
                fprintf(stderr, "Server error, FIFO open failed\n");  // 打开失败,打印错误信息
                return (0);                                           // 返回0表示失败
            }
            read_bytes = read(server_fd, rec_ptr, sizeof(*rec_ptr));  // 再次尝试读取数据
        }
        if (read_bytes == sizeof(*rec_ptr))  // 如果成功读取到完整的消息
            return_code = 1;                 // 设置返回代码为1,表示成功
    }
    return (return_code);  // 返回读取结果
}

/* open the client pipe. */

/* 打开客户端的管道 */

/**
 * @brief 向客户端发送响应的起始函数
 *
 * 该函数负责构建客户端管道名称,并尝试打开该管道以进行写操作.
 * 如果成功打开管道,则返回1,表示可以进行后续的写操作;
 * 如果打开失败,则返回0.
 *
 * @param mess_to_send 包含要发送消息的结构体
 * @return int 打开管道的结果,成功返回1,失败返回0
 */
int start_resp_to_client(const message_db_t mess_to_send)
{
#if DEBUG_TRACE
    // 在调试模式下打印当前进程ID和函数名
    printf("%d :- start_resp_to_client()\n", getpid());
#endif

    // 使用sprintf函数构建客户端管道名称
    // 使用sprintf函数将客户端进程ID格式化并拼接到管道名称中
    // client_pipe_name: 存储拼接后的管道名称的字符数组
    // CLIENT_PIPE: 管道名称的前缀,可能是一个宏定义
    // mess_to_send.client_pid: 要发送的消息中包含的客户端进程ID
    (void)sprintf(client_pipe_name, CLIENT_PIPE, mess_to_send.client_pid);

    // 尝试以只写模式打开管道
    if ((client_fd = open(client_pipe_name, O_WRONLY)) == -1)
        // 打开失败,返回0
        return (0);

    // 打开成功,返回1
    return (1);
}

/*
The messages are all sent using calls to this function.
We'll look at the corresponding client-side functions that field the message soon.
*/

/*
所有消息都是通过调用此函数发送的.
我们很快就会看到处理消息的相应客户端函数.
*/

int send_resp_to_client(const message_db_t mess_to_send)
{
    // 定义一个整型变量write_bytes,用于表示写入的字节数.
    // 具体的用途和赋值逻辑需要在函数或上下文中定义.
    int write_bytes;

// 调试信息输出
#if DEBUG_TRACE
    printf("%d :- send_resp_to_client()\n", getpid());
#endif

    // 检查客户端文件描述符是否有效
    if (client_fd == -1)
        return (0);  // 如果无效,返回0表示发送失败

    // 尝试向客户端发送消息
    write_bytes = write(client_fd, &mess_to_send, sizeof(mess_to_send));

    // 检查是否成功写入了预期数量的字节
    if (write_bytes != sizeof(mess_to_send))
        return (0);  // 如果没有,返回0表示发送失败

    return (1);  // 成功发送消息,返回1
}

/* close the client pipe. */

/* 关闭客户端管道. */

void end_resp_to_client(void)
{
#if DEBUG_TRACE
    // 在调试跟踪模式下,打印当前进程ID和函数名
    printf("%d :- end_resp_to_client()\n", getpid());
#endif

    // 如果客户端文件描述符有效
    if (client_fd != -1)
    {
        // 关闭客户端文件描述符
        (void)close(client_fd);
        // 将客户端文件描述符重置为无效状态
        client_fd = -1;
    }
}

/* After checking that a server is accessible, the client_starting function initializes the client-side pipe. */

/* 在确认服务器可用后,客户端启动函数初始化客户端端管道. */

int client_starting(void)
{
    // 如果定义了DEBUG_TRACE宏,则打印当前进程ID和函数名
#if DEBUG_TRACE
    printf("%d :- client_starting\n", getpid());
#endif

    // 获取当前进程ID并赋值给mypid
    mypid = getpid();
    // 尝试以只写模式打开服务器管道,如果失败则打印错误信息并返回0
    // 打开服务器管道以进行写操作
    // 如果打开失败,说明服务器没有运行
    if ((server_fd = open(SERVER_PIPE, O_WRONLY)) == -1)
    {
        // 输出错误信息到标准错误流
        fprintf(stderr, "Server not running\n");
        // 返回0表示失败
        return (0);
    }

    // 格式化客户端管道名称,并尝试删除已存在的同名管道
    // 使用sprintf函数格式化字符串,将mypid的值代入到CLIENT_PIPE宏定义的字符串中,生成客户端管道名称
    (void)sprintf(client_pipe_name, CLIENT_PIPE, mypid);

    // 使用unlink函数尝试删除指定的文件,这里是尝试删除之前创建的客户端管道
    (void)unlink(client_pipe_name);

    // 尝试创建一个新的命名管道,权限为0777,如果失败则打印错误信息并返回0
    // 创建一个名为 client_pipe_name 的命名管道,权限设置为 0777
    // 如果创建失败(返回 -1),则向标准错误输出打印错误信息,并返回 0
    if (mkfifo(client_pipe_name, 0777) == -1)
    {
        fprintf(stderr, "无法创建客户端管道 %s\n", client_pipe_name);
        return (0);
    }

    // 如果一切顺利,返回1表示客户端启动成功
    return (1);
}

/* The client_ending function closes file descriptors and deletes the now redundant named pipe. */

/*
 * 客户端结束函数关闭文件描述符并删除现在多余的命名管道.
 */

void client_ending(void)
{
    // 如果定义了DEBUG_TRACE宏,则打印当前进程ID和函数名,用于调试跟踪
#if DEBUG_TRACE
    printf("%d :- client_ending()\n", getpid());
#endif

    // 关闭客户端写文件描述符,如果它不是默认的无效文件描述符-1
    if (client_write_fd != -1)
        (void)close(client_write_fd);

    // 关闭客户端文件描述符,如果它不是默认的无效文件描述符-1
    if (client_fd != -1)
        (void)close(client_fd);

    // 关闭服务器文件描述符,如果它不是默认的无效文件描述符-1
    if (server_fd != -1)
        (void)close(server_fd);

    // 删除客户端管道名称对应的文件
    (void)unlink(client_pipe_name);
}

/* The send_mess_to_server function passes the request through the server pipe. */

/* send_mess_to_server函数通过服务器管道传递请求. */

/**
 * @brief 向服务器发送消息的函数
 *
 * 该函数负责将消息结构体发送到服务器.如果服务器文件描述符未初始化或写入的字节数与消息大小不匹配,
 * 则返回0,表示发送失败;否则返回1,表示发送成功.
 *
 * @param mess_to_send 要发送的消息结构体
 * @return int 发送成功返回1,发送失败返回0
 */
int send_mess_to_server(message_db_t mess_to_send)
{
    int write_bytes;

#if DEBUG_TRACE
    // 在调试模式下打印当前进程ID和函数名
    printf("%d :- send_mess_to_server()\n", getpid());
#endif

    // 检查服务器文件描述符是否已初始化
    if (server_fd == -1)
        return (0);  // 未初始化则返回失败

    // 设置消息结构体中的客户端进程ID
    mess_to_send.client_pid = mypid;

    // 尝试将消息写入服务器文件描述符

    // 向服务器文件描述符写入消息
    // server_fd: 服务器的文件描述符
    // mess_to_send: 要发送的消息的指针
    // sizeof(mess_to_send): 消息的大小
    // 返回值: 成功写入的字节数,如果发生错误则返回-1
    write_bytes = write(server_fd, &mess_to_send, sizeof(mess_to_send));

    // 检查是否写入了正确数量的字节
    if (write_bytes != sizeof(mess_to_send))
        return (0);  // 字节数不匹配则返回失败

    return (1);  // 发送成功
}

/* This client function starts to listen for the server response.
It opens a client pipe as read-only and then reopens this pipe's file as write-only.
We'll see why in a moment.
*/

/* 这个客户端函数开始监听服务器响应.
它以只读方式打开一个客户端管道,然后以只写方式重新打开这个管道的文件.
我们稍后会看到为什么这样做.
*/

int start_resp_from_server(void)
{
#if DEBUG_TRACE
    // 在调试跟踪模式下,打印当前进程ID和函数名
    printf("%d :- start_resp_from_server()\n", getpid());
#endif

    // 如果客户端管道名称为空字符串,返回0
    if (client_pipe_name[0] == '\0')
        return (0);
    // 如果客户端文件描述符已经打开,返回1
    if (client_fd != -1)
        return (1);

    // 尝试以只读模式打开客户端管道
    client_fd = open(client_pipe_name, O_RDONLY);
    // 如果成功打开只读管道
    if (client_fd != -1)
    {
        // 尝试以只写模式打开同一个管道
        client_write_fd = open(client_pipe_name, O_WRONLY);
        // 如果成功打开只写管道
        if (client_write_fd != -1)
            return (1);  // 返回1表示成功打开读写管道
        // 如果只写管道打开失败,关闭已打开的只读管道
        (void)close(client_fd);
        client_fd = -1;  // 重置客户端文件描述符
    }
    // 返回0表示打开管道失败
    return (0);
}

/* Here's the main read from the server which gets the matching database entries. */

/* 这是从服务器读取匹配数据库条目的主要代码. */

/**
 * @brief 从服务器读取响应
 *
 * 该函数尝试从服务器读取响应数据,并将其存储在提供的消息数据库指针中.
 * 如果读取成功,返回1;如果读取失败或参数无效,返回0.
 *
 * @param rec_ptr 指向消息数据库结构的指针,用于存储从服务器读取的数据
 * @return int 读取成功返回1,失败或参数无效返回0
 */
int read_resp_from_server(message_db_t *rec_ptr)
{
    int read_bytes;       // 用于存储实际读取的字节数
    int return_code = 0;  // 初始化返回代码为0

#if DEBUG_TRACE
    printf("%d :- read_resp_from_server()\n", getpid());  // 调试信息,打印当前进程ID和函数名
#endif

    if (!rec_ptr)         // 检查rec_ptr是否为空
        return (0);       // 如果为空,直接返回0
    if (client_fd == -1)  // 检查客户端文件描述符是否有效
        return (0);       // 如果无效,直接返回0

    read_bytes = read(client_fd, rec_ptr, sizeof(*rec_ptr));  // 尝试从客户端文件描述符读取数据
    if (read_bytes == sizeof(*rec_ptr))                       // 检查是否读取到预期大小的数据
        return_code = 1;                                      // 如果是,设置返回代码为1
    return (return_code);                                     // 返回结果
}

/* And finally, the client function that marks the end of the server response. */

/* 最后,客户端函数标志着服务器响应的结束. */

/**
 * @brief 标记服务器响应结束的客户端函数
 *
 * 该函数用于通知服务器客户端已经接收到了所有的数据,并且准备关闭连接.
 * 它通常在数据传输完成后被调用,是一个协议的一部分,确保双方都知道通信已经结束.
 *
 * @return 返回一个表示操作成功与否的状态码
 */
void end_resp_from_server(void)
{
#if DEBUG_TRACE
    // 在调试跟踪模式下,打印当前进程ID和函数名
    printf("%d :- end_resp_from_server()\n", getpid());
#endif

    /* This function is empty in the pipe implementation */
    /*
     * 在管道实现中,此函数为空,没有任何操作.
     * 这意味着服务器响应结束的处理逻辑可能在其他地方实现,
     * 或者这个函数被设计为一个占位符,以备将来扩展使用.
     */
}
