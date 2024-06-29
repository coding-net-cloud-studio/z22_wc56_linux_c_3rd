/* 在 #include 之后,函数原型和全局变量之后,我们来到主函数.在这里,信号量通过调用 semget 创建,并返回信号量 ID.
如果程序是第一次被调用(即带有参数且 argc > 1),则会调用 set_semvalue 初始化信号量,并将 op_char 设置为 X. */

// 导入unistd.h头文件,提供对POSIX操作系统API的访问
#include <unistd.h>
// 导入stdlib.h头文件,提供一些通用的函数,如malloc,free等
#include <stdlib.h>
// 导入stdio.h头文件,提供标准输入输出功能
#include <stdio.h>

// 导入sys/sem.h头文件,提供对System V信号量的操作
#include <sys/sem.h>

// 导入自定义的semun.h头文件,可能包含对semun联合体的定义
#include "semun.h"

// 定义四个信号量操作函数
// set_semvalue 用于设置信号量的值
static int set_semvalue(void);
// del_semvalue 用于删除信号量的值
static void del_semvalue(void);
// semaphore_p 用于对信号量进行P操作,即请求资源
static int semaphore_p(void);
// semaphore_v 用于对信号量进行V操作,即释放资源
static int semaphore_v(void);

// 声明一个静态整型变量sem_id,用于后续信号量的标识符
static int sem_id;

int main(int argc, char *argv[])
{
    int  i;              // 用于循环的计数器
    int  pause_time;     // 随机暂停时间
    char op_char = 'O';  // 操作字符,表示进程的状态

    // 初始化随机数种子
    srand((unsigned int)getpid());

    // 获取信号量ID
    sem_id = semget((key_t)1234, 1, 0666 | IPC_CREAT);

    // 如果命令行参数大于1,表示是第二个进程启动
    if (argc > 1)
    {
        // 初始化信号量
        // 检查信号量初始化是否成功
        if (!set_semvalue())
        {
            // 如果初始化失败,输出错误信息到标准错误流
            fprintf(stderr, "Failed to initialize semaphore\n");
            // 退出程序并返回非零值表示错误
            exit(EXIT_FAILURE);
        }
        op_char = 'X';  // 更改操作字符为'X'
        sleep(2);       // 等待2秒
    }

    // 进入循环,模拟10次进入临界区
    for (i = 0; i < 10; i++)
    {
        // 请求信号量,进入临界区前等待
        if (!semaphore_p())
            exit(EXIT_FAILURE);
        printf("%c", op_char);    // 打印操作字符
        fflush(stdout);           // 清空输出缓冲区
        pause_time = rand() % 3;  // 随机生成0到2的暂停时间
        sleep(pause_time);        // 暂停执行
        printf("%c", op_char);    // 再次打印操作字符
        fflush(stdout);           // 清空输出缓冲区

        // 释放信号量,退出临界区
        if (!semaphore_v())
            exit(EXIT_FAILURE);

        pause_time = rand() % 2;  // 随机生成0到1的暂停时间
        sleep(pause_time);        // 暂停执行
    }

    // 打印进程完成信息
    printf("\n%d - finished\n", getpid());

    // 如果是第二个进程启动,清理信号量
    if (argc > 1)
    {
        sleep(10);       // 等待10秒
        del_semvalue();  // 删除信号量
    }

    // 正常退出程序
    exit(EXIT_SUCCESS);
}

/* set_semvalue 函数使用 semctl 调用中的 SETVAL 命令初始化信号量.我们需要在可以使用信号量之前执行此操作. */

// 定义一个函数set_semvalue,用于设置信号量的值
static int set_semvalue(void)
{
    // 定义一个联合体semun,用于存储信号量的值
    union semun sem_union;

    // 将信号量的值设置为1
    sem_union.val = 1;
    // 使用semctl系统调用设置信号量的值,如果失败返回0
    if (semctl(sem_id, 0, SETVAL, sem_union) == -1)
        return (0);
    // 如果成功设置信号量的值,返回1
    return (1);
}

/* del_semvalue 函数的形式几乎相同,除了 semctl 调用使用 IPC_RMID 命令来删除信号量的 ID. */

// 定义一个删除信号量值的函数
static void del_semvalue(void)
{
    // 定义一个联合体,用于与信号量控制接口交互
    union semun sem_union;

    // 使用semctl系统调用尝试删除信号量集
    // 参数分别是信号量集的ID,要操作的信号量的编号(这里为0表示第一个),操作类型(IPC_RMID表示删除信号量集),以及联合体指针
    // 如果操作失败,返回-1
    if (semctl(sem_id, 0, IPC_RMID, sem_union) == -1)
        // 如果删除失败,向标准错误输出流打印错误信息
        fprintf(stderr, "Failed to delete semaphore\n");
}

/* semaphore_p 将信号量减 1(等待). */

// 定义一个名为semaphore_p的函数,用于实现信号量的P操作(等待信号量)
static int semaphore_p(void)
{
    // 定义一个sembuf结构体变量sem_b,用于设置信号量操作
    struct sembuf sem_b;

    // 设置sem_b的sem_num成员为0,表示操作的是第一个信号量
    sem_b.sem_num = 0;
    // 设置sem_b的sem_op成员为-1,表示执行P操作(等待信号量)
    sem_b.sem_op = -1; /* P() */
    // 设置sem_b的sem_flg成员为SEM_UNDO,表示在进程退出时撤销该信号量操作
    sem_b.sem_flg = SEM_UNDO;
    // 调用semop函数执行信号量操作,如果返回-1表示操作失败
    if (semop(sem_id, &sem_b, 1) == -1)
    {
        // 如果操作失败,输出错误信息到标准错误流
        fprintf(stderr, "semaphore_p failed\n");
        // 返回0表示操作失败
        return (0);
    }
    // 如果操作成功,返回1
    return (1);
}

/* semaphore_v 类似,只是将 sembuf 结构的 sem_op 部分设置为 1,以便信号量变为可用. */

// 定义一个信号量释放函数semaphore_v
static int semaphore_v(void)
{
    // 定义一个信号量操作缓冲区sem_b
    struct sembuf sem_b;

    // 设置信号量的编号为0
    sem_b.sem_num = 0;
    // 设置信号量的操作为释放(V操作)
    sem_b.sem_op = 1; /* V() */
    // 设置信号量操作的标志为SEM_UNDO,表示如果进程终止,操作系统会撤销该操作
    sem_b.sem_flg = SEM_UNDO;
    // 调用semop函数执行信号量操作,如果失败则输出错误信息并返回0
    // 尝试对信号量sem_b执行V操作(即释放一个资源)
    // semop函数用于操作信号量集,其中sem_id是信号量集的标识符
    // 第二个参数&sem_b是一个指向sembuf结构的指针,用于指定要执行的操作
    // 最后的1表示操作的信号量数量
    if (semop(sem_id, &sem_b, 1) == -1)
    {
        // 如果semop函数返回-1,表示操作失败
        // 打印错误信息到标准错误输出
        fprintf(stderr, "semaphore_v failed\n");
        // 返回0表示失败,可能需要进行错误处理或退出程序
        return (0);
    }
    // 如果成功则返回1
    return (1);
}
