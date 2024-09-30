// 检查是否在GNU库中定义了semun联合,如果没有则自定义定义
#if defined(__GNU_LIBRARY__) && !defined(_SEM_SEMUN_UNDEFINED)
/* 如果通过包含<sys/sem.h>定义了union semun,则使用该定义 */
#else
/* 根据X/OPEN标准,我们需要自己定义它 */
union semun
{
    int                 val;    // 用于SETVAL的值
    struct semid_ds    *buf;    // 用于IPC_STAT, IPC_SET的缓冲区
    unsigned short int *array;  // 用于GETALL, SETALL的数组
    struct seminfo     *__buf;  // 用于IPC_INFO的缓冲区
};
#endif
