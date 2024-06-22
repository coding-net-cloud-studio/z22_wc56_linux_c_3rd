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
cdc_entry get_cdc_entry(const char *cd_catalog_ptr)
{
    cdc_entry entry_to_return;
    char      entry_to_find[CAT_CAT_LEN + 1];
    datum     local_data_datum;
    datum     local_key_datum;

    memset(&entry_to_return, '\0', sizeof(entry_to_return));

    /* check database initialized and parameters valid */
    if (!cdc_dbm_ptr || !cdt_dbm_ptr)
        return (entry_to_return);
    if (!cd_catalog_ptr)
        return (entry_to_return);
    if (strlen(cd_catalog_ptr) >= CAT_CAT_LEN)
        return (entry_to_return);

    /* ensure the search key contains only the valid string and nulls */
    memset(&entry_to_find, '\0', sizeof(entry_to_find));
    strcpy(entry_to_find, cd_catalog_ptr);

    local_key_datum.dptr  = (void *)entry_to_find;
    local_key_datum.dsize = sizeof(entry_to_find);

    memset(&local_data_datum, '\0', sizeof(local_data_datum));
    local_data_datum = dbm_fetch(cdc_dbm_ptr, local_key_datum);
    if (local_data_datum.dptr)
    {
        memcpy(&entry_to_return, (char *)local_data_datum.dptr, local_data_datum.dsize);
    }
    return (entry_to_return);
} /* get_cdc_entry */

/*
   This function retrieves a single track entry, when passed a pointer
   pointing to a catalog string and a track number. If the entry is not
   found then the returned data has an empty catalog field. */
cdt_entry get_cdt_entry(const char *cd_catalog_ptr, const int track_no)
{
    cdt_entry entry_to_return;
    char      entry_to_find[CAT_CAT_LEN + 10];
    datum     local_data_datum;
    datum     local_key_datum;

    memset(&entry_to_return, '\0', sizeof(entry_to_return));

    /* check database initialized and parameters valid */
    if (!cdc_dbm_ptr || !cdt_dbm_ptr)
        return (entry_to_return);
    if (!cd_catalog_ptr)
        return (entry_to_return);
    if (strlen(cd_catalog_ptr) >= CAT_CAT_LEN)
        return (entry_to_return);

    /* setup the search key, which is a composite key of catalog entry
       and track number */
    memset(&entry_to_find, '\0', sizeof(entry_to_find));
    sprintf(entry_to_find, "%s %d", cd_catalog_ptr, track_no);

    local_key_datum.dptr  = (void *)entry_to_find;
    local_key_datum.dsize = sizeof(entry_to_find);

    memset(&local_data_datum, '\0', sizeof(local_data_datum));
    local_data_datum = dbm_fetch(cdt_dbm_ptr, local_key_datum);
    if (local_data_datum.dptr)
    {
        memcpy(&entry_to_return, (char *)local_data_datum.dptr, local_data_datum.dsize);
    }
    return (entry_to_return);
} /* get_cdt_entry */

/* This function adds a new catalog entry. */
int add_cdc_entry(const cdc_entry entry_to_add)
{
    char  key_to_add[CAT_CAT_LEN + 1];
    datum local_data_datum;
    datum local_key_datum;
    int   result;

    /* check database initialized and parameters valid */
    if (!cdc_dbm_ptr || !cdt_dbm_ptr)
        return (0);
    if (strlen(entry_to_add.catalog) >= CAT_CAT_LEN)
        return (0);

    /* ensure the search key contains only the valid string and nulls */
    memset(&key_to_add, '\0', sizeof(key_to_add));
    strcpy(key_to_add, entry_to_add.catalog);

    local_key_datum.dptr   = (void *)key_to_add;
    local_key_datum.dsize  = sizeof(key_to_add);
    local_data_datum.dptr  = (void *)&entry_to_add;
    local_data_datum.dsize = sizeof(entry_to_add);

    result = dbm_store(cdc_dbm_ptr, local_key_datum, local_data_datum, DBM_REPLACE);

    /* dbm_store() uses 0 for success */
    if (result == 0)
        return (1);
    return (0);

} /* add_cdc_entry */

/* This function adds a new catalog entry. The access key is the
   catalog string and track number acting as a composite key */
int add_cdt_entry(const cdt_entry entry_to_add)
{
    char  key_to_add[CAT_CAT_LEN + 10];
    datum local_data_datum;
    datum local_key_datum;
    int   result;

    /* check database initialized and parameters valid */
    if (!cdc_dbm_ptr || !cdt_dbm_ptr)
        return (0);
    if (strlen(entry_to_add.catalog) >= CAT_CAT_LEN)
        return (0);

    /* ensure the search key contains only the valid string and nulls */
    memset(&key_to_add, '\0', sizeof(key_to_add));
    sprintf(key_to_add, "%s %d", entry_to_add.catalog, entry_to_add.track_no);

    local_key_datum.dptr   = (void *)key_to_add;
    local_key_datum.dsize  = sizeof(key_to_add);
    local_data_datum.dptr  = (void *)&entry_to_add;
    local_data_datum.dsize = sizeof(entry_to_add);

    result = dbm_store(cdt_dbm_ptr, local_key_datum, local_data_datum, DBM_REPLACE);

    /* dbm_store() uses 0 for success and -ve numbers for errors */
    if (result == 0)
        return (1);
    return (0);
} /* add_cdt_entry */

int del_cdc_entry(const char *cd_catalog_ptr)
{
    char  key_to_del[CAT_CAT_LEN + 1];
    datum local_key_datum;
    int   result;

    /* check database initialized and parameters valid */
    if (!cdc_dbm_ptr || !cdt_dbm_ptr)
        return (0);
    if (strlen(cd_catalog_ptr) >= CAT_CAT_LEN)
        return (0);

    /* ensure the search key contains only the valid string and nulls */
    memset(&key_to_del, '\0', sizeof(key_to_del));
    strcpy(key_to_del, cd_catalog_ptr);

    local_key_datum.dptr  = (void *)key_to_del;
    local_key_datum.dsize = sizeof(key_to_del);

    result = dbm_delete(cdc_dbm_ptr, local_key_datum);

    /* dbm_delete() uses 0 for success */
    if (result == 0)
        return (1);
    return (0);

} /* del_cdc_entry */

int del_cdt_entry(const char *cd_catalog_ptr, const int track_no)
{
    char  key_to_del[CAT_CAT_LEN + 10];
    datum local_key_datum;
    int   result;

    /* check database initialized and parameters valid */
    if (!cdc_dbm_ptr || !cdt_dbm_ptr)
        return (0);
    if (strlen(cd_catalog_ptr) >= CAT_CAT_LEN)
        return (0);

    /* ensure the search key contains only the valid string and nulls */
    memset(&key_to_del, '\0', sizeof(key_to_del));
    sprintf(key_to_del, "%s %d", cd_catalog_ptr, track_no);

    local_key_datum.dptr  = (void *)key_to_del;
    local_key_datum.dsize = sizeof(key_to_del);

    result = dbm_delete(cdt_dbm_ptr, local_key_datum);

    /* dbm_delete() uses 0 for success */
    if (result == 0)
        return (1);
    return (0);

} /* del_cdt_entry */

/* This function searches for a catalog entry, where the catalog
   text contains the provided search text. If the search text points
   to a null character then all entries are considered to match */
cdc_entry search_cdc_entry(const char *cd_catalog_ptr, int *first_call_ptr)
{
    static int   local_first_call = 1;
    cdc_entry    entry_to_return;
    datum        local_data_datum;
    static datum local_key_datum; /* notice this must be static */

    memset(&entry_to_return, '\0', sizeof(entry_to_return));

    /* check database initialized and parameters valid */
    if (!cdc_dbm_ptr || !cdt_dbm_ptr)
        return (entry_to_return);
    if (!cd_catalog_ptr || !first_call_ptr)
        return (entry_to_return);
    if (strlen(cd_catalog_ptr) >= CAT_CAT_LEN)
        return (entry_to_return);

    /* protect against never passing *first_call_ptr true */
    if (local_first_call)
    {
        local_first_call = 0;
        *first_call_ptr  = 1;
    }
    if (*first_call_ptr)
    {
        *first_call_ptr = 0;
        local_key_datum = dbm_firstkey(cdc_dbm_ptr);
    }
    else
    {
        local_key_datum = dbm_nextkey(cdc_dbm_ptr);
    }

    do
    {
        if (local_key_datum.dptr != NULL)
        {
            /* an entry was found */
            local_data_datum = dbm_fetch(cdc_dbm_ptr, local_key_datum);
            if (local_data_datum.dptr)
            {
                memcpy(&entry_to_return, (char *)local_data_datum.dptr, local_data_datum.dsize);

                /* check if search string occures in the entry */
                if (!strstr(entry_to_return.catalog, cd_catalog_ptr))
                {
                    memset(&entry_to_return, '\0', sizeof(entry_to_return));
                    local_key_datum = dbm_nextkey(cdc_dbm_ptr);
                }
            }
        }
    } while (local_key_datum.dptr &&
             local_data_datum.dptr &&
             (entry_to_return.catalog[0] == '\0'));
    /* Finished finding entries, either there are no more or one matched */

    return (entry_to_return);
} /* search_cdc_entry */
