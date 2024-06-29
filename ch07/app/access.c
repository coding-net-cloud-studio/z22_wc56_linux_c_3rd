/*
   This file provides the functions for accessing the CD database.
 */
/*
   此文件提供了访问CD数据库的函数.
 */
#define _XOPEN_SOURCE

// 导入所需的头文件
#include <unistd.h>  // 提供对POSIX操作系统API的访问,如read/write等系统调用
#include <stdlib.h>  // 提供程序退出,动态内存分配等功能
#include <stdio.h>   // 提供标准输入输出功能
#include <fcntl.h>   // 提供文件控制和文件描述符操作功能
#include <string.h>  // 提供字符串处理功能

// 引入ndbm库,这是为了使用gdbm数据库,某些Linux发行版可能需要使用gdbm-ndbm.h
#include <ndbm.h>

// 引入自定义的cd_data头文件,这个文件可能包含了与CD数据相关的结构体和函数声明
#include "cd_data.h"

// 定义CDC(CD数据)和CDT(CD时间)文件的基名
#define CDC_FILE_BASE "cdc_data"
#define CDT_FILE_BASE "cdt_data"

// 定义CDC和CDT文件的目录和页码文件名
// 定义CDC数据的目录文件名
#define CDC_FILE_DIR  "cdc_data.dir"
// 定义CDC数据的页文件名
#define CDC_FILE_PAG  "cdc_data.pag"
// 定义CDT数据的目录文件名
#define CDT_FILE_DIR  "cdt_data.dir"
// 定义CDT数据的页文件名
#define CDT_FILE_PAG  "cdt_data.pag"

/* 一些文件作用域变量,用于访问数据库 */
// cdc_dbm_ptr 用于指向控制数据管理块(Control Data Management Block)的指针
static DBM *cdc_dbm_ptr = NULL;
// cdt_dbm_ptr 用于指向控制数据表(Control Data Table)的指针
static DBM *cdt_dbm_ptr = NULL;

/* This function initializes access to the database. If the parameter
   new_database is true, then a new database is started.
 */
/* 此函数用于初始化对数据库的访问.如果参数 new_database 为真,则启动一个新数据库.
 */
// 定义数据库初始化函数,参数为是否创建新数据库的标识
int database_initialize(const int new_database)
{
    // 定义打开模式,包括创建和读写权限
    int open_mode = O_CREAT | O_RDWR;

    // 如果当前有打开的数据库,则先关闭它们
    // 如果cdc_dbm_ptr不为空,则关闭对应的数据库连接
    if (cdc_dbm_ptr)
        dbm_close(cdc_dbm_ptr);
    // 如果cdt_dbm_ptr不为空,则关闭对应的数据库连接
    if (cdt_dbm_ptr)
        dbm_close(cdt_dbm_ptr);

    // 如果需要创建新数据库
    if (new_database)
    {
        // 删除旧的文件
        // 删除与CDC相关的文件页和目录文件
        (void)unlink(CDC_FILE_PAG);
        (void)unlink(CDC_FILE_DIR);

        // 删除与CDT相关的文件页和目录文件
        (void)unlink(CDT_FILE_PAG);
        (void)unlink(CDT_FILE_DIR);
    }
    // 打开或创建新的数据库文件
    cdc_dbm_ptr = dbm_open(CDC_FILE_BASE, open_mode, 0644);
    cdt_dbm_ptr = dbm_open(CDT_FILE_BASE, open_mode, 0644);

    // 如果无法打开或创建,则输出错误信息并返回0
    // 检查cdc_dbm_ptr和cdt_dbm_ptr是否都未为空,这是确保数据库连接成功的关键步骤
    if (!cdc_dbm_ptr || !cdt_dbm_ptr)
    {
        // 如果任一指针为空,则输出错误信息到标准错误流
        fprintf(stderr, "无法创建数据库\n");
        // 并将两个指针都设置为NULL,以便后续代码知道连接失败
        cdc_dbm_ptr = cdt_dbm_ptr = NULL;
        // 返回0表示函数执行失败
        return (0);
    }
    // 成功则返回1
    return (1);

} /* database_initialize */

// 定义一个函数database_close,用于关闭数据库连接
void database_close(void)
{
    // 检查cdc_dbm_ptr是否不为空,如果不为空,则调用dbm_close函数关闭对应的数据库连接
    if (cdc_dbm_ptr)
        dbm_close(cdc_dbm_ptr);
    // 检查cdt_dbm_ptr是否不为空,如果不为空,则调用dbm_close函数关闭对应的数据库连接
    if (cdt_dbm_ptr)
        dbm_close(cdt_dbm_ptr);

    // 将cdc_dbm_ptr和cdt_dbm_ptr都设置为NULL,表示当前没有打开的数据库连接
    cdc_dbm_ptr = cdt_dbm_ptr = NULL;

} /* database_close */

/*
   This function retrieves a single catalog entry, when passed a pointer
   pointing to catalog text string. If the entry is not found then the
   returned data has an empty catalog field. */
/*
这个函数当接收到指向目录文本字符串的指针时,检索单个目录条目.
如果未找到该条目,则返回的数据具有空的目录字段. */
// 定义函数get_cdc_entry,用于从数据库中获取cdc_entry结构体
cdc_entry get_cdc_entry(const char *cd_catalog_ptr)
{
    // 初始化返回的cdc_entry结构体为全0
    cdc_entry entry_to_return;
    // 定义一个字符数组用于存储要查找的条目名称
    char entry_to_find[CAT_CAT_LEN + 1];
    // 定义datum类型变量用于存储数据和键值
    datum local_data_datum;
    datum local_key_datum;

    // 清空entry_to_return结构体的内存
    memset(&entry_to_return, '\0', sizeof(entry_to_return));

    // 检查数据库是否已初始化且参数有效
    // 检查cdc_dbm_ptr和cdt_dbm_ptr是否为空,如果为空则返回entry_to_return
    if (!cdc_dbm_ptr || !cdt_dbm_ptr)
        return (entry_to_return);
    // 检查cd_catalog_ptr是否为空,如果为空则返回entry_to_return
    if (!cd_catalog_ptr)
        return (entry_to_return);
    // 检查cd_catalog_ptr的长度是否超过了CAT_CAT_LEN,如果超过则返回entry_to_return
    if (strlen(cd_catalog_ptr) >= CAT_CAT_LEN)
        return (entry_to_return);

    // 确保搜索键只包含有效字符串和空字符
    memset(&entry_to_find, '\0', sizeof(entry_to_find));
    strcpy(entry_to_find, cd_catalog_ptr);

    // 初始化键值datum,指针指向entry_to_find,大小为entry_to_find的大小
    // 将指针entry_to_find转换为void指针类型,并赋值给local_key_datum的dptr成员
    // 这样做是为了保持数据类型的通用性,因为void指针可以指向任何类型的数据
    local_key_datum.dptr = (void *)entry_to_find;

    // 设置local_key_datum的dsize成员,表示entry_to_find的大小
    // 这通常在处理内存块或结构体时很有用,因为它允许程序知道要处理多少数据
    local_key_datum.dsize = sizeof(entry_to_find);

    // 清空数据datum的内存
    memset(&local_data_datum, '\0', sizeof(local_data_datum));
    // 使用dbm_fetch函数从数据库中获取对应键值的数据
    local_data_datum = dbm_fetch(cdc_dbm_ptr, local_key_datum);
    // 如果获取到的数据非空,则将数据复制到返回的结构体中
    if (local_data_datum.dptr)
    {
        // 将 local_data_datum.dptr 指向的内存内容复制到 entry_to_return 结构体中
        // local_data_datum.dsize 指定了要复制的字节数
        memcpy(&entry_to_return, (char *)local_data_datum.dptr, local_data_datum.dsize);
    }
    // 返回处理后的cdc_entry结构体
    return (entry_to_return);
} /* get_cdc_entry */

/*
   This function retrieves a single track entry, when passed a pointer
   pointing to a catalog string and a track number. If the entry is not
   found then the returned data has an empty catalog field. */

/*
这个函数接收一个指向目录字符串的指针和一个音轨号作为参数,检索单个音轨条目.
如果没有找到条目,则返回的数据的目录字段为空. */

// 定义函数get_cdt_entry,用于从数据库中获取特定CD目录条目的信息
cdt_entry get_cdt_entry(const char *cd_catalog_ptr, const int track_no)
{
    // 定义返回的cdt_entry结构体变量
    cdt_entry entry_to_return;
    // 定义用于搜索的字符串,长度为CAT_CAT_LEN + 10
    char entry_to_find[CAT_CAT_LEN + 10];
    // 定义用于存储数据的datum结构体变量
    datum local_data_datum;
    // 定义用于存储键的datum结构体变量
    datum local_key_datum;

    // 初始化返回的cdt_entry结构体变量为空
    memset(&entry_to_return, '\0', sizeof(entry_to_return));

    // 检查数据库是否已初始化且参数有效
    // 检查cdc_dbm_ptr和cdt_dbm_ptr是否为空,如果为空则返回entry_to_return
    if (!cdc_dbm_ptr || !cdt_dbm_ptr)
        return (entry_to_return);
    // 检查cd_catalog_ptr是否为空,如果为空则返回entry_to_return
    if (!cd_catalog_ptr)
        return (entry_to_return);
    // 检查cd_catalog_ptr的长度是否超过了CAT_CAT_LEN,如果超过则返回entry_to_return
    if (strlen(cd_catalog_ptr) >= CAT_CAT_LEN)
        return (entry_to_return);

    // 设置搜索键,它是目录条目和音轨编号的组合键
    // 清空entry_to_find结构体,准备填充新的数据
    memset(&entry_to_find, '\0', sizeof(entry_to_find));

    // 将cd_catalog_ptr和track_no格式化为字符串,并存储到entry_to_find中
    // cd_catalog_ptr是CD目录指针,track_no是音轨编号
    sprintf(entry_to_find, "%s %d", cd_catalog_ptr, track_no);

    // 初始化键的datum结构体变量
    // 将指针dptr指向要查找的条目entry_to_find的地址
    local_key_datum.dptr = (void *)entry_to_find;

    // 设置结构体中的dsize成员,表示条目的字节大小
    local_key_datum.dsize = sizeof(entry_to_find);

    // 初始化数据的datum结构体变量为空
    memset(&local_data_datum, '\0', sizeof(local_data_datum));
    // 从数据库中获取数据
    local_data_datum = dbm_fetch(cdt_dbm_ptr, local_key_datum);
    // 如果获取到了数据
    if (local_data_datum.dptr)
    {
        // 将数据复制到返回的结构体变量中
        memcpy(&entry_to_return, (char *)local_data_datum.dptr, local_data_datum.dsize);
    }
    // 返回结果
    return (entry_to_return);
} /* get_cdt_entry */

/* 这个函数添加一个新的目录条目. */
int add_cdc_entry(const cdc_entry entry_to_add)  // 定义一个函数,用于添加新的cdc_entry到数据库
{
    char key_to_add[CAT_CAT_LEN + 1];  // 定义一个字符数组,用于存储要添加的键值

    datum local_data_datum;  // 定义一个datum结构体变量,用于存储要添加的数据
    datum local_key_datum;   // 定义一个datum结构体变量,用于存储要添加的键
    int   result;            // 定义一个整型变量,用于存储dbm_store函数的返回值

    /* 检查数据库是否已初始化并且参数有效 */
    if (!cdc_dbm_ptr || !cdt_dbm_ptr)  // 如果数据库指针为空,则返回0表示失败
        return (0);
    if (strlen(entry_to_add.catalog) >= CAT_CAT_LEN)  // 如果输入的目录长度超过定义的最大长度,则返回0表示失败
        return (0);

    /* 确保搜索键只包含有效字符串和空值 */
    memset(&key_to_add, '\0', sizeof(key_to_add));  // 将键值数组初始化为全0
    strcpy(key_to_add, entry_to_add.catalog);       // 将输入的目录复制到键值数组中

    local_key_datum.dptr   = (void *)key_to_add;     // 设置键的指针指向键值数组
    local_key_datum.dsize  = sizeof(key_to_add);     // 设置键的大小为键值数组的大小
    local_data_datum.dptr  = (void *)&entry_to_add;  // 设置数据的指针指向输入的cdc_entry
    local_data_datum.dsize = sizeof(entry_to_add);   // 设置数据的大小为cdc_entry的大小

    result = dbm_store(cdc_dbm_ptr, local_key_datum, local_data_datum, DBM_REPLACE);  // 调用dbm_store函数存储数据,如果键已存在则替换

    /* dbm_store()使用0表示成功 */
    if (result == 0)  // 如果dbm_store函数返回0,表示操作成功
        return (1);
    return (0);  // 否则返回0表示失败

} /* add_cdc_entry */

/* This function adds a new catalog entry. The access key is the
   catalog string and track number acting as a composite key */
/*这个函数添加一个新的目录条目.访问密钥是作为复合键的目录字符串和轨道号.*/
// 定义添加cdt条目的函数,参数为要添加的cdt_entry结构体
int add_cdt_entry(const cdt_entry entry_to_add)
{
    // 定义一个字符串用于存储搜索键,长度为CAT_CAT_LEN + 10
    char key_to_add[CAT_CAT_LEN + 10];

    // 定义datum类型的局部变量,用于存储数据和键
    datum local_data_datum;
    datum local_key_datum;

    // 定义整型变量result,用于存储操作结果
    int result;

    // 检查数据库是否已初始化且参数有效
    if (!cdc_dbm_ptr || !cdt_dbm_ptr)
        return (0);  // 如果数据库指针为空,返回0表示失败
    if (strlen(entry_to_add.catalog) >= CAT_CAT_LEN)
        return (0);  // 如果目录名长度超过限制,返回0表示失败

    // 确保搜索键只包含有效字符串和空字符
    memset(&key_to_add, '\0', sizeof(key_to_add));                              // 清空key_to_add数组
    sprintf(key_to_add, "%s %d", entry_to_add.catalog, entry_to_add.track_no);  // 构造搜索键

    // 初始化局部键和数据datum
    local_key_datum.dptr   = (void *)key_to_add;     // 设置键的指针
    local_key_datum.dsize  = sizeof(key_to_add);     // 设置键的大小
    local_data_datum.dptr  = (void *)&entry_to_add;  // 设置数据的指针
    local_data_datum.dsize = sizeof(entry_to_add);   // 设置数据的大小

    // 使用dbm_store函数存储数据到数据库
    result = dbm_store(cdt_dbm_ptr, local_key_datum, local_data_datum, DBM_REPLACE);  // 存储数据并替换旧条目

    // 根据dbm_store的返回值判断操作是否成功
    if (result == 0)
        return (1);  // 成功则返回1
    return (0);      // 失败则返回0
} /* add_cdt_entry */

// 定义删除CDC条目的函数,参数为指向CD目录的指针
int del_cdc_entry(const char *cd_catalog_ptr)
{
    // 定义要删除的键,长度为CAT_CAT_LEN + 1
    char key_to_del[CAT_CAT_LEN + 1];
    // 定义本地键数据结构
    datum local_key_datum;
    // 定义结果变量
    int result;

    // 检查数据库是否已初始化且参数有效
    if (!cdc_dbm_ptr || !cdt_dbm_ptr)
        return (0);  // 如果数据库指针无效,则返回0表示失败
    if (strlen(cd_catalog_ptr) >= CAT_CAT_LEN)
        return (0);  // 如果目录长度超过限制,则返回0表示失败

    // 确保搜索键只包含有效字符串和空值
    memset(&key_to_del, '\0', sizeof(key_to_del));  // 将键内存区域清零
    strcpy(key_to_del, cd_catalog_ptr);             // 将目录指针指向的内容复制到键中

    // 设置本地键数据结构的dptr和dsize
    local_key_datum.dptr  = (void *)key_to_del;
    local_key_datum.dsize = sizeof(key_to_del);

    // 调用dbm_delete函数尝试删除键值对
    result = dbm_delete(cdc_dbm_ptr, local_key_datum);

    // dbm_delete()函数使用0表示成功
    if (result == 0)
        return (1);  // 如果删除成功,则返回1
    return (0);      // 否则返回0表示失败

} /* del_cdc_entry */

// 定义删除CD目录条目的函数,接收CD目录指针和曲目号作为参数
int del_cdt_entry(const char *cd_catalog_ptr, const int track_no)
{
    // 定义要删除的键的缓冲区,长度为CAT_CAT_LEN + 10
    char key_to_del[CAT_CAT_LEN + 10];
    // 定义用于操作数据库的datum结构体变量
    datum local_key_datum;
    // 定义操作结果变量
    int result;

    // 检查数据库是否已初始化并且参数是否有效
    if (!cdc_dbm_ptr || !cdt_dbm_ptr)
        return (0);  // 如果数据库指针无效,则返回0表示失败
    if (strlen(cd_catalog_ptr) >= CAT_CAT_LEN)
        return (0);  // 如果CD目录指针长度超过限制,则返回0表示失败

    // 确保搜索键只包含有效字符串和空字符
    memset(&key_to_del, '\0', sizeof(key_to_del));           // 清空缓冲区
    sprintf(key_to_del, "%s %d", cd_catalog_ptr, track_no);  // 构造搜索键

    // 准备datum结构体以用于数据库操作
    local_key_datum.dptr  = (void *)key_to_del;  // 设置键的指针
    local_key_datum.dsize = sizeof(key_to_del);  // 设置键的大小

    // 执行删除操作
    result = dbm_delete(cdt_dbm_ptr, local_key_datum);  // 调用dbm_delete函数

    // 根据dbm_delete()的返回值判断操作是否成功(0表示成功)
    if (result == 0)
        return (1);  // 成功删除,返回1
    return (0);      // 删除失败,返回0

} /* del_cdt_entry */

/* This function searches for a catalog entry, where the catalog
   text contains the provided search text. If the search text points
   to a null character then all entries are considered to match */

/* 这个函数用于搜索目录条目,其中目录文本包含提供的搜索文本.
如果搜索文本指向一个空字符,则认为所有条目都匹配. */
// 定义一个函数search_cdc_entry,用于在数据库中搜索匹配的cdc_entry
cdc_entry search_cdc_entry(const char *cd_catalog_ptr, int *first_call_ptr)
{
    // 定义一个静态变量local_first_call,用于标记是否是第一次调用
    static int local_first_call = 1;
    // 定义一个cdc_entry类型的变量entry_to_return,用于返回找到的条目
    cdc_entry entry_to_return;
    // 定义一个datum类型的变量local_data_datum,用于存储从数据库中获取的数据
    datum local_data_datum;
    // 定义一个静态datum类型的变量local_key_datum,用于存储数据库中的键值,必须是静态的
    static datum local_key_datum;

    // 将entry_to_return变量清零
    // 将entry_to_return结构体的内存清零
    memset(&entry_to_return, '\0', sizeof(entry_to_return));

    // 检查数据库是否已初始化且参数有效
    // 检查指针是否为空,如果cdc_dbm_ptr或cdt_dbm_ptr为空,则返回entry_to_return
    if (!cdc_dbm_ptr || !cdt_dbm_ptr)
        return (entry_to_return);
    // 检查指针是否为空,如果cd_catalog_ptr或first_call_ptr为空,则返回entry_to_return
    if (!cd_catalog_ptr || !first_call_ptr)
        return (entry_to_return);
    // 检查字符串长度是否超出定义的长度,如果cd_catalog_ptr的长度大于等于CAT_CAT_LEN,则返回entry_to_return
    if (strlen(cd_catalog_ptr) >= CAT_CAT_LEN)
        return (entry_to_return);

    // 防止*first_call_ptr永远不被设置为true
    // 如果这是本地函数第一次被调用
    if (local_first_call)
    {
        // 将本地首次调用标志设置为假(false)
        local_first_call = 0;
        // 将首次调用标志指针指向的值设置为真(true)
        *first_call_ptr = 1;
    }
    // 定义一个指针变量 first_call_ptr,用于标记是否是第一次调用
    if (*first_call_ptr)
    {
        // 如果是第一次调用,将 first_call_ptr 指向的值设为 0,表示已经调用过
        *first_call_ptr = 0;
        // 调用 dbm_firstkey 函数获取数据库中的第一个键值对
        local_key_datum = dbm_firstkey(cdc_dbm_ptr);
    }
    else
    {
        // 如果不是第一次调用,调用 dbm_nextkey 函数获取数据库中的下一个键值对
        local_key_datum = dbm_nextkey(cdc_dbm_ptr);
    }

    // 循环查找
    do
    {
        // 如果找到了键值
        if (local_key_datum.dptr != NULL)
        {
            // 从数据库中获取对应的数据
            local_data_datum = dbm_fetch(cdc_dbm_ptr, local_key_datum);
            // 如果获取到了数据
            if (local_data_datum.dptr)
            {
                // 将数据复制到entry_to_return
                // 将 local_data_datum.dptr 指向的内存内容复制到 entry_to_return 结构体中
                // local_data_datum.dsize 指定了要复制的字节数
                memcpy(&entry_to_return, (char *)local_data_datum.dptr, local_data_datum.dsize);

                // 检查搜索字符串是否出现在条目中
                if (!strstr(entry_to_return.catalog, cd_catalog_ptr))
                {
                    // 如果没有找到,清空entry_to_return并继续查找下一个键值
                    // 将entry_to_return结构体清零,为接下来的操作做准备
                    memset(&entry_to_return, '\0', sizeof(entry_to_return));

                    // 获取数据库中的下一个键值对
                    local_key_datum = dbm_nextkey(cdc_dbm_ptr);
                }
            }
        }
    } while (local_key_datum.dptr &&
             local_data_datum.dptr &&
             (entry_to_return.catalog[0] == '\0'));
    // 循环检查以下条件是否同时满足:
    // 1. local_key_datum.dptr 不为空,表示关键数据指针有效.
    // 2. local_data_datum.dptr 不为空,表示数据指针有效.
    // 3. entry_to_return.catalog[0] 为 '\0',表示目录条目为空,需要继续查找.
    // 结束查找,要么没有更多的条目,要么找到了一个匹配的条目

    // 返回找到的条目
    return (entry_to_return);
} /* search_cdc_entry */
