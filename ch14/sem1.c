/* 在 #include 之后，函数原型和全局变量之后，我们来到主函数。在这里，信号量通过调用 semget 创建，并返回信号量 ID。
如果程序是第一次被调用（即带有参数且 argc > 1），则会调用 set_semvalue 初始化信号量，并将 op_char 设置为 X。 */

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#include <sys/sem.h>

#include "semun.h"

static int  set_semvalue(void);
static void del_semvalue(void);
static int  semaphore_p(void);
static int  semaphore_v(void);

static int sem_id;

int main(int argc, char *argv[])
{
    int  i;
    int  pause_time;
    char op_char = 'O';

    srand((unsigned int)getpid());

    sem_id = semget((key_t)1234, 1, 0666 | IPC_CREAT);

    if (argc > 1)
    {
        if (!set_semvalue())
        {
            fprintf(stderr, "Failed to initialize semaphore\n");
            exit(EXIT_FAILURE);
        }
        op_char = 'X';
        sleep(2);
    }

    /* Then we have a loop which enters and leaves the critical section ten times.
     There, we first make a call to semaphore_p which sets the semaphore to wait, as
     this program is about to enter the critical section. */

    for (i = 0; i < 10; i++)
    {

        if (!semaphore_p())
            exit(EXIT_FAILURE);
        printf("%c", op_char);
        fflush(stdout);
        pause_time = rand() % 3;
        sleep(pause_time);
        printf("%c", op_char);
        fflush(stdout);

        /* After the critical section, we call semaphore_v, setting the semaphore available,
         before going through the for loop again after a random wait. After the loop, the call
         to del_semvalue is made to clean up the code. */

        if (!semaphore_v())
            exit(EXIT_FAILURE);

        pause_time = rand() % 2;
        sleep(pause_time);
    }

    printf("\n%d - finished\n", getpid());

    if (argc > 1)
    {
        sleep(10);
        del_semvalue();
    }

    exit(EXIT_SUCCESS);
}

/* set_semvalue 函数使用 semctl 调用中的 SETVAL 命令初始化信号量。我们需要在可以使用信号量之前执行此操作。 */

static int set_semvalue(void)
{
    union semun sem_union;

    sem_union.val = 1;
    if (semctl(sem_id, 0, SETVAL, sem_union) == -1)
        return (0);
    return (1);
}

/* del_semvalue 函数的形式几乎相同，除了 semctl 调用使用 IPC_RMID 命令来删除信号量的 ID。 */

static void del_semvalue(void)
{
    union semun sem_union;

    if (semctl(sem_id, 0, IPC_RMID, sem_union) == -1)
        fprintf(stderr, "Failed to delete semaphore\n");
}

/* semaphore_p 将信号量减 1（等待）。 */

static int semaphore_p(void)
{
    struct sembuf sem_b;

    sem_b.sem_num = 0;
    sem_b.sem_op  = -1; /* P() */
    sem_b.sem_flg = SEM_UNDO;
    if (semop(sem_id, &sem_b, 1) == -1)
    {
        fprintf(stderr, "semaphore_p failed\n");
        return (0);
    }
    return (1);
}

/* semaphore_v 类似，只是将 sembuf 结构的 sem_op 部分设置为 1，以便信号量变为可用。 */

static int semaphore_v(void)
{
    struct sembuf sem_b;

    sem_b.sem_num = 0;
    sem_b.sem_op  = 1; /* V() */
    sem_b.sem_flg = SEM_UNDO;
    if (semop(sem_id, &sem_b, 1) == -1)
    {
        fprintf(stderr, "semaphore_v failed\n");
        return (0);
    }
    return (1);
}
