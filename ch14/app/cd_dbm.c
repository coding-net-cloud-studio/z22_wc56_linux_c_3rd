/*
   这个文件提供了访问CD数据库的函数.

   注意:_XOPEN_SOURCE 宏定义用于支持某些POSIX功能.
 */

// 引入标准库头文件
#include <unistd.h>  // 提供对POSIX操作系统API的访问
#include <stdlib.h>  // 提供通用工具函数
#include <stdio.h>   // 提供标准输入输出功能
#include <fcntl.h>   // 提供文件控制功能
#include <string.h>  // 提供字符串操作函数

// 引入ndbm数据库头文件,可能需要根据不同的Linux发行版更改为gdbm-ndbm.h
#include <ndbm.h>

// 引入自定义的CD数据结构头文件
#include "cd_data.h"

// 定义CDC文件的基础名称
#define CDC_FILE_BASE "cdc_data"
// 定义CDT文件的基础名称
#define CDT_FILE_BASE "cdt_data"
// 定义CDC文件的目录
#define CDC_FILE_DIR  "cdc_data.dir"
// 定义CDC文件的页面
#define CDC_FILE_PAG  "cdc_data.pag"
// 定义CDT文件的目录
#define CDT_FILE_DIR  "cdt_data.dir"
// 定义CDT文件的页面
#define CDT_FILE_PAG  "cdt_data.pag"

/*
 * 一些用于访问数据库的文件作用域变量
 * static关键字表示这些变量仅在当前文件中可见
 * DBM *类型表示这些变量是指向DBM数据库的指针
 */
static DBM *cdc_dbm_ptr = NULL;  // 指向CDC数据库的指针
static DBM *cdt_dbm_ptr = NULL;  // 指向CDT数据库的指针

/* 该函数用于初始化对数据库的访问.如果参数 new_database 为真,则开始一个新的数据库.
 *
 * @param new_database 如果为真,则创建新数据库并初始化;否则,尝试打开现有数据库.
 * @return 成功返回1,失败返回0.
 */
int database_initialize(const int new_database)
{
    int open_mode = O_RDWR;  // 默认以读写模式打开数据库

    /* 如果有任何现有的数据库已打开,则关闭它 */
    if (cdc_dbm_ptr)
        dbm_close(cdc_dbm_ptr);  // 关闭cdc_dbm_ptr指向的数据库
    if (cdt_dbm_ptr)
        dbm_close(cdt_dbm_ptr);  // 关闭cdt_dbm_ptr指向的数据库

    // 如果new_database为真,则执行以下代码块
    // 这个条件判断通常用于检查是否需要创建一个新的数据库
    if (new_database)
    {
        // 在这里添加创建新数据库的代码
        /* 删除旧文件 */
        (void)unlink(CDC_FILE_PAG);  // 删除CDC_FILE_PAG文件
        (void)unlink(CDC_FILE_DIR);  // 删除CDC_FILE_DIR文件
        (void)unlink(CDT_FILE_PAG);  // 删除CDT_FILE_PAG文件
        (void)unlink(CDT_FILE_DIR);  // 删除CDT_FILE_DIR文件

        open_mode = O_CREAT | O_RDWR;  // 设置打开模式为创建并读写
    }
    /* 打开一些新文件,如果需要则创建它们 */
    cdc_dbm_ptr = dbm_open(CDC_FILE_BASE, open_mode, 0644);  // 打开或创建CDC_FILE_BASE文件
    cdt_dbm_ptr = dbm_open(CDT_FILE_BASE, open_mode, 0644);  // 打开或创建CDT_FILE_BASE文件
    if (!cdc_dbm_ptr || !cdt_dbm_ptr)                        // 检查是否成功打开或创建数据库文件
    {
        fprintf(stderr, "Unable to create database\n");  // 输出错误信息
        cdc_dbm_ptr = cdt_dbm_ptr = NULL;                // 将数据库指针设置为NULL
        return (0);                                      // 返回失败
    }
    return (1);  // 返回成功
} /* database_initialize */

// 关闭数据库连接
// 该函数用于关闭已打开的数据库连接,释放相关资源.
void database_close(void)
{
    // 如果cdc_dbm_ptr不为空,则关闭对应的数据库连接
    if (cdc_dbm_ptr)
        dbm_close(cdc_dbm_ptr);  // 关闭数据库连接

    // 如果cdt_dbm_ptr不为空,则关闭对应的数据库连接
    if (cdt_dbm_ptr)
        dbm_close(cdt_dbm_ptr);  // 关闭数据库连接

    // 将数据库指针置为空,表示连接已关闭
    cdc_dbm_ptr = cdt_dbm_ptr = NULL;
} /* database_close */

/*
 * 函数功能:根据传入的目录文本字符串指针检索单个目录条目.
 * 如果未找到条目,则返回的数据中的目录字段为空.
 *
 * @param cd_catalog_ptr 目录文本字符串指针
 * @return cdc_entry 返回的目录条目结构体
 */
cdc_entry get_cdc_entry(const char *cd_catalog_ptr)
{
    cdc_entry entry_to_return;                 // 用于返回的目录条目结构体
    char      entry_to_find[CAT_CAT_LEN + 1];  // 用于存储要查找的条目的字符串
    datum     local_data_datum;                // 数据datum
    datum     local_key_datum;                 // 键datum

    memset(&entry_to_return, '\0', sizeof(entry_to_return));  // 初始化返回的目录条目结构体为空

    // 检查数据库是否已初始化以及参数是否有效
    if (!cdc_dbm_ptr || !cdt_dbm_ptr)
        return (entry_to_return);  // 如果数据库指针无效,则返回空条目
    if (!cd_catalog_ptr)
        return (entry_to_return);  // 如果传入的目录指针为空,则返回空条目
    if (strlen(cd_catalog_ptr) >= CAT_CAT_LEN)
        return (entry_to_return);  // 如果目录字符串过长,则返回空条目

    // 确保搜索键只包含有效字符串和空字符
    memset(&entry_to_find, '\0', sizeof(entry_to_find));  // 初始化查找字符串为空
    strcpy(entry_to_find, cd_catalog_ptr);                // 复制目录字符串到查找字符串

    local_key_datum.dptr  = (void *)entry_to_find;  // 设置键datum的指针
    local_key_datum.dsize = sizeof(entry_to_find);  // 设置键datum的大小

    memset(&local_data_datum, '\0', sizeof(local_data_datum));   // 初始化数据datum为空
    local_data_datum = dbm_fetch(cdc_dbm_ptr, local_key_datum);  // 从数据库中获取数据
    if (local_data_datum.dptr)
    {
        // 如果找到数据,则将数据复制到返回的目录条目结构体中
        // memcpy: 将指定数量的字节从源地址复制到目标地址
        // &entry_to_return: 目标地址,即将数据复制到的目录条目结构体的地址
        // (char *)local_data_datum.dptr: 源地址,即包含要复制数据的指针
        // local_data_datum.dsize: 要复制的字节数
        memcpy(&entry_to_return, (char *)local_data_datum.dptr, local_data_datum.dsize);
        // 如果找到数据,则复制到返回的目录条目结构体
    }
    return (entry_to_return);  // 返回目录条目结构体
} /* get_cdc_entry */

/*
 * 函数功能:根据提供的目录字符串和曲目编号检索单个曲目条目.
 * 如果未找到条目,则返回的数据具有空的目录字段.
 *
 * @param cd_catalog_ptr 指向目录字符串的指针
 * @param track_no 曲目编号
 * @return cdt_entry 返回的曲目条目结构体
 */
cdt_entry get_cdt_entry(const char *cd_catalog_ptr, const int track_no)
{
    cdt_entry entry_to_return;                  // 要返回的条目结构体
    char      entry_to_find[CAT_CAT_LEN + 10];  // 用于构建搜索键的缓冲区
    datum     local_data_datum;                 // 用于存储从数据库检索到的数据
    datum     local_key_datum;                  // 用于存储搜索键

    memset(&entry_to_return, '\0', sizeof(entry_to_return));  // 初始化返回的条目结构体

    // 检查数据库是否已初始化以及参数是否有效
    // 检查cdc_dbm_ptr和cdt_dbm_ptr是否为空指针,如果是,则返回entry_to_return
    if (!cdc_dbm_ptr || !cdt_dbm_ptr)
        return (entry_to_return);

    // 检查cd_catalog_ptr是否为空指针,如果是,则返回entry_to_return
    if (!cd_catalog_ptr)
        return (entry_to_return);

    // 检查cd_catalog_ptr的长度是否大于或等于CAT_CAT_LEN定义的长度限制,如果是,则返回entry_to_return
    if (strlen(cd_catalog_ptr) >= CAT_CAT_LEN)
        return (entry_to_return);

    // 构建搜索键,它是目录条目和曲目编号的组合键
    memset(&entry_to_find, '\0', sizeof(entry_to_find));
    sprintf(entry_to_find, "%s %d", cd_catalog_ptr, track_no);

    local_key_datum.dptr  = (void *)entry_to_find;  // 设置搜索键的数据指针
    local_key_datum.dsize = sizeof(entry_to_find);  // 设置搜索键的数据大小

    memset(&local_data_datum, '\0', sizeof(local_data_datum));   // 初始化数据结构体
    local_data_datum = dbm_fetch(cdt_dbm_ptr, local_key_datum);  // 从数据库中检索数据
    if (local_data_datum.dptr)
    {
        memcpy(&entry_to_return, (char *)local_data_datum.dptr, local_data_datum.dsize);  // 将检索到的数据复制到返回的条目结构体
    }
    return (entry_to_return);  // 返回条目结构体
} /* get_cdt_entry */

/* 这个函数用于添加一个新的目录条目. */
int add_cdc_entry(const cdc_entry entry_to_add)
{
    char  key_to_add[CAT_CAT_LEN + 1];  // 定义一个字符数组用于存储要添加的键
    datum local_data_datum;             // 定义一个datum结构体用于存储数据
    datum local_key_datum;              // 定义一个datum结构体用于存储键
    int   result;                       // 定义一个整型变量用于存储操作结果

    /* 检查数据库是否已初始化以及参数是否有效 */
    if (!cdc_dbm_ptr || !cdt_dbm_ptr)  // 如果数据库指针为空,则返回0
        return (0);
    if (strlen(entry_to_add.catalog) >= CAT_CAT_LEN)  // 如果目录名长度超过限制,则返回0
        return (0);

    /* 确保搜索键只包含有效的字符串和空字符 */
    memset(&key_to_add, '\0', sizeof(key_to_add));  // 将key_to_add数组清零
    strcpy(key_to_add, entry_to_add.catalog);       // 将entry_to_add的catalog字段复制到key_to_add

    local_key_datum.dptr   = (void *)key_to_add;     // 设置local_key_datum的dptr为key_to_add的地址
    local_key_datum.dsize  = sizeof(key_to_add);     // 设置local_key_datum的dsize为key_to_add的大小
    local_data_datum.dptr  = (void *)&entry_to_add;  // 设置local_data_datum的dptr为entry_to_add的地址
    local_data_datum.dsize = sizeof(entry_to_add);   // 设置local_data_datum的dsize为entry_to_add的大小

    result = dbm_store(cdc_dbm_ptr, local_key_datum, local_data_datum, DBM_REPLACE);  // 将条目存储到数据库中,如果键已存在则替换

    /* dbm_store() 使用0表示成功 */
    if (result == 0)  // 如果存储操作成功
        return (1);   // 返回1
    return (0);       // 否则返回0
} /* add_cdc_entry */

/* 这个函数用于添加一个新的目录条目.访问键是由目录字符串和曲目编号组成的复合键 */
int add_cdt_entry(const cdt_entry entry_to_add)
{
    char  key_to_add[CAT_CAT_LEN + 10];  // 定义用于存储复合键的字符数组
    datum local_data_datum;              // 定义用于存储数据的datum结构体
    datum local_key_datum;               // 定义用于存储键的datum结构体
    int   result;                        // 定义函数返回结果

    /* 检查数据库是否已初始化以及参数是否有效 */
    if (!cdc_dbm_ptr || !cdt_dbm_ptr)  // 如果数据库指针为空,则返回0
        return (0);
    if (strlen(entry_to_add.catalog) >= CAT_CAT_LEN)  // 如果目录字符串长度超过限制,则返回0
        return (0);

    /* 确保搜索键只包含有效的字符串和空字符 */
    memset(&key_to_add, '\0', sizeof(key_to_add));                              // 清空key_to_add数组
    sprintf(key_to_add, "%s %d", entry_to_add.catalog, entry_to_add.track_no);  // 格式化复合键

    local_key_datum.dptr   = (void *)key_to_add;     // 设置键的指针
    local_key_datum.dsize  = sizeof(key_to_add);     // 设置键的大小
    local_data_datum.dptr  = (void *)&entry_to_add;  // 设置数据的指针
    local_data_datum.dsize = sizeof(entry_to_add);   // 设置数据的大小

    // 将条目存储到数据库中
    // dbm_store函数用于将键值对存储到dbm数据库中
    // cdt_dbm_ptr: 指向dbm数据库的指针
    // local_key_datum: 要存储的键的数据
    // local_data_datum: 要存储的数据
    // DBM_REPLACE: 如果键已经存在,则替换旧值
    result = dbm_store(cdt_dbm_ptr, local_key_datum, local_data_datum, DBM_REPLACE);
    // 将条目存储到数据库中

    /* dbm_store() 使用0表示成功,负数表示错误 */
    if (result == 0)  // 如果存储成功
        return (1);
    return (0);  // 否则返回0
} /* add_cdt_entry */

int del_cdc_entry(const char *cd_catalog_ptr)
{
    // 定义要删除的键的数组
    char key_to_del[CAT_CAT_LEN + 1];
    // 定义一个datum结构体,用于存储键的数据
    datum local_key_datum;
    // 定义一个整型变量,用于存储操作结果
    int result;

    // 检查数据库是否已初始化以及参数是否有效
    if (!cdc_dbm_ptr || !cdt_dbm_ptr)
        return (0);  // 如果数据库指针为空,返回0
    if (strlen(cd_catalog_ptr) >= CAT_CAT_LEN)
        return (0);  // 如果目录字符串过长,返回0

    // 确保搜索键只包含有效的字符串和空字符
    memset(&key_to_del, '\0', sizeof(key_to_del));  // 清空键数组
    strcpy(key_to_del, cd_catalog_ptr);             // 复制目录字符串到键数组

    // 设置datum结构体的指针和大小
    local_key_datum.dptr  = (void *)key_to_del;
    local_key_datum.dsize = sizeof(key_to_del);

    // 调用dbm_delete函数删除键对应的条目
    result = dbm_delete(cdc_dbm_ptr, local_key_datum);

    // dbm_delete()函数使用0表示成功
    if (result == 0)
        return (1);  // 如果删除成功,返回1
    return (0);      // 如果删除失败,返回0
} /* del_cdc_entry */

int del_cdt_entry(const char *cd_catalog_ptr, const int track_no)
{
    // 定义用于删除的键的缓冲区
    char key_to_del[CAT_CAT_LEN + 10];
    // 定义数据库操作所需的数据结构
    datum local_key_datum;
    // 定义函数返回结果
    int result;

    /* 检查数据库是否已初始化以及参数是否有效 */
    if (!cdc_dbm_ptr || !cdt_dbm_ptr)
        return (0);  // 如果数据库指针为空,返回失败
    if (strlen(cd_catalog_ptr) >= CAT_CAT_LEN)
        return (0);  // 如果目录字符串过长,返回失败

    /* 确保搜索键只包含有效的字符串和空字符 */
    memset(&key_to_del, '\0', sizeof(key_to_del));           // 清空键缓冲区
    sprintf(key_to_del, "%s %d", cd_catalog_ptr, track_no);  // 格式化键字符串

    local_key_datum.dptr  = (void *)key_to_del;  // 设置数据指针
    local_key_datum.dsize = sizeof(key_to_del);  // 设置数据大小

    result = dbm_delete(cdt_dbm_ptr, local_key_datum);  // 执行删除操作

    /* dbm_delete() 使用 0 表示成功 */
    if (result == 0)
        return (1);  // 删除成功,返回成功标志
    return (0);      // 删除失败,返回失败标志
} /* del_cdt_entry */
/*
 * 函数功能:在目录中搜索包含提供的搜索文本的目录条目.
 * 如果搜索文本指向一个空字符,则认为所有条目都匹配.
 *
 * @param cd_catalog_ptr 指向要搜索的目录文本的指针.
 * @param first_call_ptr 指向一个整数的指针,用于标记是否是第一次调用.
 * @return 返回一个cdc_entry结构体,包含找到的条目或空条目.
 */
cdc_entry search_cdc_entry(const char *cd_catalog_ptr, int *first_call_ptr)
{
    static int   local_first_call = 1;  // 静态变量,标记是否是第一次调用
    cdc_entry    entry_to_return;       // 要返回的条目
    datum        local_data_datum;      // 从数据库中获取的数据
    static datum local_key_datum;       // 静态变量,存储数据库键值

    memset(&entry_to_return, '\0', sizeof(entry_to_return));  // 初始化返回条目为空

    // 检查数据库是否已初始化以及参数是否有效
    if (!cdc_dbm_ptr || !cdt_dbm_ptr)
        return (entry_to_return);
    if (!cd_catalog_ptr || !first_call_ptr)
        return (entry_to_return);
    if (strlen(cd_catalog_ptr) >= CAT_CAT_LEN)
        return (entry_to_return);

    // 防止永远不会将*first_call_ptr设置为true
    // 如果是第一次调用本地函数
    if (local_first_call)
    {
        local_first_call = 0;  // 将本地首次调用标志设置为0
        *first_call_ptr  = 1;  // 将首次调用指针设置为1
    }

    // 如果是首次调用
    if (*first_call_ptr)
    {
        *first_call_ptr = 0;                          // 将首次调用指针设置为0
        local_key_datum = dbm_firstkey(cdc_dbm_ptr);  // 获取数据库中的第一个键值
    }
    else
    {
        local_key_datum = dbm_nextkey(cdc_dbm_ptr);  // 获取数据库中的下一个键值
    }

    do
    {
        if (local_key_datum.dptr != NULL)
        {
            // 找到一个条目
            local_data_datum = dbm_fetch(cdc_dbm_ptr, local_key_datum);  // 获取条目数据
            if (local_data_datum.dptr)
            {
                memcpy(&entry_to_return, (char *)local_data_datum.dptr, local_data_datum.dsize);  // 复制数据到返回条目

                // 检查搜索字符串是否出现在条目中
                if (!strstr(entry_to_return.catalog, cd_catalog_ptr))
                {
                    memset(&entry_to_return, '\0', sizeof(entry_to_return));  // 如果不匹配,重置返回条目为空
                    local_key_datum = dbm_nextkey(cdc_dbm_ptr);               // 获取下一个键值
                }
            }
        }
    } while (local_key_datum.dptr &&
             local_data_datum.dptr &&
             (entry_to_return.catalog[0] == '\0'));  // 循环直到没有更多条目或找到匹配的条目

    return (entry_to_return);  // 返回找到的条目或空条目
} /* search_cdc_entry */
