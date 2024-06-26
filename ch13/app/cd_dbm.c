/*
   This file provides the functions for accessing the CD database.
 */

#define _XOPEN_SOURCE

// 引入unistd头文件,提供对POSIX操作系统API的访问,如read(), write()等系统调用
#include <unistd.h>
// 引入stdlib头文件,提供通用的函数,如malloc(), free(), exit()等
#include <stdlib.h>
// 引入stdio头文件,提供C语言的输入输出功能,如printf(), scanf()等
#include <stdio.h>
// 引入fcntl头文件,提供对文件控制的接口,如open(), close(), fcntl()等
#include <fcntl.h>
// 引入string头文件,提供字符串处理函数,如strcpy(), strcmp()等
#include <string.h>
// 引入ndbm头文件,提供对数据库管理库(DBM)的访问接口
#include <ndbm.h>
/* The above may need to be changed to gdbm-ndbm.h on some distributions */

// 引入cd_data头文件,该文件可能包含了数据结构和函数声明
#include "cd_data.h"

// 定义CDC(可能是某种配置或常量)文件的基名
#define CDC_FILE_BASE "cdc_data"
// 定义CDT(可能是另一种配置或常量)文件的基名
#define CDT_FILE_BASE "cdt_data"

// 定义CDC文件的目录文件名
#define CDC_FILE_DIR  "cdc_data.dir"
// 定义CDC文件的页文件名
#define CDC_FILE_PAG  "cdc_data.pag"

// 定义CDT文件的目录文件名
#define CDT_FILE_DIR  "cdt_data.dir"
// 定义CDT文件的页文件名
#define CDT_FILE_PAG  "cdt_data.pag"

/* Some file scope variables for accessing the database */
/* 一些用于访问数据库的文件作用域变量 */

// 定义两个静态DBM指针,用于在程序中管理数据库连接
// cdc_dbm_ptr 用于处理某种特定类型的数据库连接
static DBM *cdc_dbm_ptr = NULL;
// cdt_dbm_ptr 用于处理另一种特定类型的数据库连接
static DBM *cdt_dbm_ptr = NULL;

/* This function initializes access to the database. If the parameter
   new_database is true, then a new database is started.
 */

/* 这个函数初始化对数据库的访问.如果参数
  new_database 为 true,则启动一个新的数据库.
*/

// 函数:database_initialize
// 功能:初始化数据库,可以创建新数据库或打开现有数据库
// 参数:new_database - 非零值表示创建新数据库,零表示打开现有数据库
// 返回值:成功返回1,失败返回0
int database_initialize(const int new_database)
{
    // 定义打开模式,默认为读写模式
    int open_mode = O_RDWR;

    // 如果任何现有的数据库已打开,则关闭它
    // 关闭cdc_dbm_ptr指向的数据库,如果它不是空指针
    if (cdc_dbm_ptr)
        // 使用dbm_close函数关闭数据库
        dbm_close(cdc_dbm_ptr);

    // 关闭cdt_dbm_ptr指向的数据库,如果它不是空指针
    if (cdt_dbm_ptr)
        // 使用dbm_close函数关闭数据库
        dbm_close(cdt_dbm_ptr);

    // 如果是创建新数据库
    if (new_database)
    {
        // 删除旧文件
        // 删除与CD数据库相关的文件,这些文件可能包括页数据文件和目录文件
        // CDC_FILE_PAG 代表 CD 数据库的页数据文件的路径名
        (void)unlink(CDC_FILE_PAG);  // 删除CD数据库的页数据文件
        // CDC_FILE_DIR 代表 CD 数据库的目录文件的路径名
        (void)unlink(CDC_FILE_DIR);  // 删除CD数据库的目录文件

        // 删除与CDT数据库相关的文件,这些文件也可能包括页数据文件和目录文件
        // CDT_FILE_PAG 代表 CDT 数据库的页数据文件的路径名
        (void)unlink(CDT_FILE_PAG);  // 删除CDT数据库的页数据文件
        // CDT_FILE_DIR 代表 CDT 数据库的目录文件的路径名
        (void)unlink(CDT_FILE_DIR);  // 删除CDT数据库的目录文件

        // 更新打开模式以包含创建选项
        open_mode |= O_CREAT;
    }
    // 打开新文件,如果需要则创建它们
    // 打开CDC数据库文件,使用指定的打开模式和权限(0644)
    cdc_dbm_ptr = dbm_open(CDC_FILE_BASE, open_mode, 0644);

    // 打开CDT数据库文件,使用相同的打开模式和权限
    cdt_dbm_ptr = dbm_open(CDT_FILE_BASE, open_mode, 0644);

    // 检查是否成功打开文件
    // 检查数据库指针是否有效
    if (!cdc_dbm_ptr || !cdt_dbm_ptr)
    {
        // 如果指针无效,向标准错误输出错误信息
        fprintf(stderr, "无法创建数据库\n");
        // 将指针设置为NULL,表示数据库未成功打开
        cdc_dbm_ptr = cdt_dbm_ptr = NULL;
        // 返回失败状态码
        return (0);
    }
    // 返回成功状态
    return (1);
} /* database_initialize */

// 关闭数据库连接的函数
void database_close(void)
{
    // 如果cdc_dbm_ptr非空,则关闭对应的数据库连接
    if (cdc_dbm_ptr)
        dbm_close(cdc_dbm_ptr);

    // 如果cdt_dbm_ptr非空,则关闭对应的数据库连接
    if (cdt_dbm_ptr)
        dbm_close(cdt_dbm_ptr);

    // 将两个数据库指针设置为NULL,表示关闭了连接
    cdc_dbm_ptr = cdt_dbm_ptr = NULL;

} /* database_close */

/*
   This function retrieves a single catalog entry, when passed a pointer
   pointing to catalog text string. If the entry is not found then the
   returned data has an empty catalog field. */

/*
  此函数检索单个目录条目,当传递一个指向目录文本字符串的指针时.
  如果找不到条目,则返回的数据具有空的目录字段. */

// 定义一个函数get_cdc_entry,用于从数据库中获取特定的cdc_entry
cdc_entry get_cdc_entry(const char *cd_catalog_ptr)
{
    // 初始化一个cdc_entry结构体变量,用于存储要返回的条目
    cdc_entry entry_to_return;
    // 初始化一个字符数组,用于构建搜索键
    char entry_to_find[CAT_CAT_LEN + 1];
    // 定义两个datum类型变量,用于与数据库交互
    datum local_data_datum;
    datum local_key_datum;

    // 将entry_to_return结构体的内存清零
    memset(&entry_to_return, '\0', sizeof(entry_to_return));

    // 检查数据库是否已初始化以及参数是否有效
    if (!cdc_dbm_ptr || !cdt_dbm_ptr)
        return (entry_to_return);  // 如果数据库未初始化或参数无效,则返回空的entry_to_return
    if (!cd_catalog_ptr)
        return (entry_to_return);  // 如果传入的指针为空,则返回空的entry_to_return
    if (strlen(cd_catalog_ptr) >= CAT_CAT_LEN)
        return (entry_to_return);  // 如果传入的字符串长度超过限制,则返回空的entry_to_return

    // 确保搜索键只包含有效字符串和空字符
    memset(&entry_to_find, '\0', sizeof(entry_to_find));  // 清空entry_to_find数组
    strcpy(entry_to_find, cd_catalog_ptr);                // 将传入的字符串复制到entry_to_find数组中

    // 设置local_key_datum的值,用于在数据库中查找
    local_key_datum.dptr  = (void *)entry_to_find;
    local_key_datum.dsize = sizeof(entry_to_find);

    // 清空local_data_datum的内存
    memset(&local_data_datum, '\0', sizeof(local_data_datum));
    // 使用dbm_fetch函数从数据库中获取数据
    local_data_datum = dbm_fetch(cdc_dbm_ptr, local_key_datum);
    // 如果获取到了数据
    if (local_data_datum.dptr)
    {
        // 将获取到的数据复制到entry_to_return结构体中
        // 将local_data_datum中的数据复制到entry_to_return结构体中
        // 注意:这里假设entry_to_return是预先定义好的结构体类型,且其大小与local_data_datum.dsize相匹配
        memcpy(&entry_to_return,               // 目标内存地址,即要填充数据的结构体变量地址
               (char *)local_data_datum.dptr,  // 源内存地址,即包含数据的指针地址
               local_data_datum.dsize);        // 要复制的字节数
    }
    // 返回获取到的条目
    return (entry_to_return);
} /* get_cdc_entry */

/*
   该函数用于检索单个曲目条目,当传入一个指向目录字符串的指针和曲目标号时.
   如果没有找到条目,则返回的数据中目录字段为空. */
cdt_entry get_cdt_entry(const char *cd_catalog_ptr, const int track_no)
{
    // 初始化要返回的条目
    cdt_entry entry_to_return;
    // 用于构造搜索键的缓冲区
    char entry_to_find[CAT_CAT_LEN + 10];
    // 用于存储从数据库检索到的数据的datum结构
    datum local_data_datum;
    // 用于存储搜索键的datum结构
    datum local_key_datum;

    // 清空返回条目,准备填充数据
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

    // 构造搜索键,它是目录条目和曲目标号的组合键
    // 清空entry_to_find结构体,准备填充新数据
    memset(&entry_to_find, '\0', sizeof(entry_to_find));

    // 将cd_catalog_ptr和track_no格式化为字符串,并存储到entry_to_find中
    // cd_catalog_ptr是CD目录的指针,track_no是要查找的曲目号
    sprintf(entry_to_find, "%s %d", cd_catalog_ptr, track_no);

    // 准备搜索键的数据结构
    // 定义一个本地密钥数据结构体变量
    // dptr指针指向要查找的条目
    // dsize设置为条目的字节大小
    local_key_datum.dptr  = (void *)entry_to_find;
    local_key_datum.dsize = sizeof(entry_to_find);

    // 清空数据datum结构,准备接收数据
    memset(&local_data_datum, '\0', sizeof(local_data_datum));
    // 从数据库中检索数据
    local_data_datum = dbm_fetch(cdt_dbm_ptr, local_key_datum);
    // 如果检索到数据,则复制到返回条目中
    // 如果local_data_datum.dptr不为空,即存在指向数据的指针
    if (local_data_datum.dptr)
    {
        // 使用memcpy函数将local_data_datum.dptr指向的数据复制到entry_to_return中
        // local_data_datum.dsize指定了要复制的字节数
        memcpy(&entry_to_return, (char *)local_data_datum.dptr, local_data_datum.dsize);
    }
    // 返回检索到的条目
    return (entry_to_return);
} /* get_cdt_entry */

/* 这个函数添加一个新的目录条目. */
int add_cdc_entry(const cdc_entry entry_to_add)  // 定义一个函数,用于添加新的目录条目
{
    char key_to_add[CAT_CAT_LEN + 1];  // 定义一个字符数组,用于存储要添加的键值

    // 定义datum结构体变量,用于存储数据和键值
    datum local_data_datum;
    datum local_key_datum;

    int result;  // 定义一个整型变量,用于存储操作结果

    /* 检查数据库是否已初始化并且参数有效 */
    if (!cdc_dbm_ptr || !cdt_dbm_ptr)  // 如果数据库指针为空,则返回0表示失败
        return (0);
    if (strlen(entry_to_add.catalog) >= CAT_CAT_LEN)  // 如果输入的目录长度超过定义的最大长度,则返回0表示失败
        return (0);

    /* 确保搜索键只包含有效字符串和空值 */
    memset(&key_to_add, '\0', sizeof(key_to_add));  // 将键值数组初始化为全0
    strcpy(key_to_add, entry_to_add.catalog);       // 将输入的目录复制到键值数组中

    // 初始化datum结构体变量,准备存储数据和键值
    // 定义一个本地密钥数据结构体变量,将指针指向要添加的密钥,并设置数据大小为密钥的大小
    local_key_datum.dptr  = (void *)key_to_add;
    local_key_datum.dsize = sizeof(key_to_add);

    // 定义一个本地数据数据结构体变量,将指针指向要添加的条目,并设置数据大小为条目的大小
    local_data_datum.dptr  = (void *)&entry_to_add;
    local_data_datum.dsize = sizeof(entry_to_add);

    // 使用dbm_store函数存储数据,如果键值已存在则替换
    // 调用dbm_store函数来存储数据
    // cdc_dbm_ptr是指向数据库管理结构的指针
    // local_key_datum是包含要存储的关键字的datum结构
    // local_data_datum是包含要存储的数据的datum结构
    // DBM_REPLACE指示如果存在相同的键,则替换该键的值
    result = dbm_store(cdc_dbm_ptr, local_key_datum, local_data_datum, DBM_REPLACE);

    /* dbm_store()函数使用0表示成功 */
    if (result == 0)  // 如果操作成功,则返回1
        return (1);
    return (0);  // 否则返回0表示失败

} /* add_cdc_entry */

/* 这个函数添加一个新的目录条目.访问键是目录字符串和曲目号组成的复合键 */
int add_cdt_entry(const cdt_entry entry_to_add)  // 定义函数,参数为要添加的目录条目结构体
{
    char  key_to_add[CAT_CAT_LEN + 10];  // 定义一个字符串数组,用于存储生成的键值
    datum local_data_datum;              // 定义一个datum结构体,用于存储要添加的数据
    datum local_key_datum;               // 定义一个datum结构体,用于存储要添加的键值
    int   result;                        // 定义一个整型变量,用于存储操作结果

    /* 检查数据库是否已初始化且参数有效 */
    if (!cdc_dbm_ptr || !cdt_dbm_ptr)  // 如果数据库指针为空,返回0表示失败
        return (0);
    if (strlen(entry_to_add.catalog) >= CAT_CAT_LEN)  // 如果目录字符串长度超过限制,返回0表示失败
        return (0);

    /* 确保搜索键只包含有效字符串和空字符 */
    memset(&key_to_add, '\0', sizeof(key_to_add));                              // 将键值数组初始化为全0
    sprintf(key_to_add, "%s %d", entry_to_add.catalog, entry_to_add.track_no);  // 生成键值,格式为"目录字符串 空格 曲目号"

    local_key_datum.dptr   = (void *)key_to_add;     // 设置键值的指针
    local_key_datum.dsize  = sizeof(key_to_add);     // 设置键值的大小
    local_data_datum.dptr  = (void *)&entry_to_add;  // 设置数据的指针
    local_data_datum.dsize = sizeof(entry_to_add);   // 设置数据的大小

    result = dbm_store(cdt_dbm_ptr, local_key_datum, local_data_datum, DBM_REPLACE);  // 调用dbm_store函数存储数据,如果键值已存在则替换

    /* dbm_store()函数使用0表示成功,负数表示错误 */
    if (result == 0)  // 如果操作成功,返回1
        return (1);
    return (0);  // 如果操作失败,返回0
} /* add_cdt_entry */

// 删除CD目录条目的函数
int del_cdc_entry(const char *cd_catalog_ptr)
{
    // 定义要删除的键,长度为CAT_CAT_LEN + 1
    char key_to_del[CAT_CAT_LEN + 1];
    // 定义一个datum结构体用于存储键值
    datum local_key_datum;
    // 定义删除操作的结果变量
    int result;

    // 检查数据库是否已初始化并且参数有效
    if (!cdc_dbm_ptr || !cdt_dbm_ptr)
        return (0);  // 如果数据库指针无效,则返回0表示失败
    if (strlen(cd_catalog_ptr) >= CAT_CAT_LEN)
        return (0);  // 如果输入的字符串长度超过限制,则返回0表示失败

    // 确保搜索键只包含有效字符串和空字符
    memset(&key_to_del, '\0', sizeof(key_to_del));  // 将键缓冲区清零
    strcpy(key_to_del, cd_catalog_ptr);             // 将输入的字符串复制到键中

    // 准备datum结构体,用于传递给dbm_delete函数
    local_key_datum.dptr  = (void *)key_to_del;  // 设置键的指针
    local_key_datum.dsize = sizeof(key_to_del);  // 设置键的大小

    // 执行删除操作
    result = dbm_delete(cdc_dbm_ptr, local_key_datum);

    // dbm_delete()函数使用0表示成功
    if (result == 0)
        return (1);  // 如果删除成功,则返回1
    return (0);      // 如果删除失败,则返回0

} /* del_cdc_entry */

// 删除CD目录项的函数
// 参数:cd_catalog_ptr - CD目录的指针
//       track_no - 要删除的曲目的轨道号
int del_cdt_entry(const char *cd_catalog_ptr, const int track_no)
{
    // 定义要删除的键,包括CD目录和轨道号
    char key_to_del[CAT_CAT_LEN + 10];
    // 定义一个datum结构体用于存储键值
    datum local_key_datum;
    // 定义删除操作的结果变量
    int result;

    // 检查数据库是否已初始化并且参数有效
    if (!cdc_dbm_ptr || !cdt_dbm_ptr)
        return (0);  // 如果数据库指针无效,则返回0表示失败
    if (strlen(cd_catalog_ptr) >= CAT_CAT_LEN)
        return (0);  // 如果CD目录长度超过限制,则返回0表示失败

    // 确保搜索键只包含有效字符串和空字符
    memset(&key_to_del, '\0', sizeof(key_to_del));           // 清空键缓冲区
    sprintf(key_to_del, "%s %d", cd_catalog_ptr, track_no);  // 构造键字符串

    // 准备datum结构体的dptr和dsize字段
    local_key_datum.dptr  = (void *)key_to_del;
    local_key_datum.dsize = sizeof(key_to_del);

    // 执行删除操作
    // 删除数据库中的指定键值对
    // 参数:cdt_dbm_ptr - 指向dbm数据库的指针
    //       local_key_datum - 要删除的键的数据结构
    // 返回值:result - 删除操作的结果
    result = dbm_delete(cdt_dbm_ptr, local_key_datum);

    // dbm_delete()函数使用0表示成功
    if (result == 0)
        return (1);  // 如果删除成功,则返回1
    return (0);      // 如果删除失败,则返回0

} /* del_cdt_entry */
/* 这个函数用于搜索目录条目,其中目录文本包含提供的搜索文本.如果搜索文本指向空字符,则认为所有条目都匹配 */
cdc_entry search_cdc_entry(const char *cd_catalog_ptr, int *first_call_ptr)
{
    // 静态变量,用于记录是否是第一次调用
    static int local_first_call = 1;
    // 用于返回找到的条目
    cdc_entry entry_to_return;
    // 用于存储当前数据的datum结构
    datum local_data_datum;
    // 静态变量,用于存储当前键的datum结构,注意必须是静态的
    static datum local_key_datum;

    // 初始化返回条目为零值
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

    // 防止*first_call_ptr永远不传递true
    // 如果这是第一次调用,设置标志位并告知调用者这是第一次调用
    if (local_first_call)
    {
        local_first_call = 0;  // 清除本地首次调用标志
        *first_call_ptr  = 1;  // 设置调用者首次调用标志
    }
    // 如果调用者首次调用标志被设置,则清除它并开始获取第一个键
    if (*first_call_ptr)
    {
        *first_call_ptr = 0;  // 清除调用者首次调用标志
        // 获取数据库中的第一个键值对
        local_key_datum = dbm_firstkey(cdc_dbm_ptr);
    }
    else
    {
        // 如果不是首次调用,则获取下一个键值对
        local_key_datum = dbm_nextkey(cdc_dbm_ptr);
    }

    // 循环直到找到匹配的条目或没有更多条目
    // 定义一个循环,用于遍历数据库中的所有条目
    do
    {
        // 如果找到了一个条目(local_key_datum.dptr 不为空)
        if (local_key_datum.dptr != NULL)
        {
            // 获取当前键对应的数据
            local_data_datum = dbm_fetch(cdc_dbm_ptr, local_key_datum);

            // 如果获取到的数据非空(local_data_datum.dptr 不为空)
            if (local_data_datum.dptr)
            {
                // 将数据复制到返回条目中
                memcpy(&entry_to_return, (char *)local_data_datum.dptr, local_data_datum.dsize);

                // 检查搜索字符串是否出现在条目中
                // 如果不出现(strstr 返回 NULL),则执行以下操作
                if (!strstr(entry_to_return.catalog, cd_catalog_ptr))
                {
                    // 如果没有找到,清空返回条目
                    memset(&entry_to_return, '\0', sizeof(entry_to_return));

                    // 继续查找下一个键
                    local_key_datum = dbm_nextkey(cdc_dbm_ptr);
                }
            }
        }
        // 当以下条件之一不满足时,退出循环:
        // 1. local_key_datum.dptr 为空(没有找到更多的条目)
        // 2. local_data_datum.dptr 为空(无法获取当前条目的数据)
        // 3. entry_to_return.catalog[0] 不为 '\0'(已找到符合条件的条目)
    } while (local_key_datum.dptr &&
             local_data_datum.dptr &&
             (entry_to_return.catalog[0] == '\0'));
    // 完成查找条目,要么没有更多,要么找到一个匹配的

    // 返回找到的条目
    return (entry_to_return);
} /* search_cdc_entry */
