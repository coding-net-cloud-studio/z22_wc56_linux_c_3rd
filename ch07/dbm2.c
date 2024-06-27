#include <unistd.h>  // 用于unistd相关的功能,如sleep等
#include <stdlib.h>  // 用于标准库函数,如exit等
#include <stdio.h>   // 用于标准输入输出函数,如printf等
#include <fcntl.h>   // 用于文件控制相关的函数

#include <ndbm.h>  // 包含ndbm数据库的头文件
/* 在某些系统上,需要将上面的行替换为
#include <gdbm-ndbm.h>
*/

#include <string.h>  // 用于字符串处理函数,如strcpy等

// 定义测试数据库文件的路径
#define TEST_DB_FILE "/tmp/dbm2_test"
// 定义用于测试的项目数量
#define ITEMS_USED   3

// 定义一个结构体用于测试dbm
struct test_data
{
    char misc_chars[15];  // 一些杂项字符
    int  any_integer;     // 一个任意整数
    char more_chars[21];  // 更多字符
};

int main()
{
    // 定义一个结构体数组用于存储要存储的数据
    struct test_data items_to_store[ITEMS_USED];
    // 定义一个结构体用于检索数据
    struct test_data item_retrieved;

    // 定义一个用于构建键的字符数组
    char key_to_use[20];
    // 定义循环变量和结果变量
    int i, result;

    // 定义datum类型变量用于存储键和数据
    datum key_datum;
    datum data_datum;

    // 打开或创建数据库文件
    DBM *dbm_ptr = dbm_open(TEST_DB_FILE, O_RDWR | O_CREAT, 0666);
    if (!dbm_ptr)
    {
        // 如果打开数据库失败,打印错误信息并退出程序
        fprintf(stderr, "Failed to open database\n");
        exit(EXIT_FAILURE);
    }

    // 初始化结构体数组
    memset(items_to_store, '\0', sizeof(items_to_store));
    // 填充结构体数组的第一个元素
    strcpy(items_to_store[0].misc_chars, "First!");
    items_to_store[0].any_integer = 47;
    strcpy(items_to_store[0].more_chars, "foo");
    // 填充结构体数组的第二个元素
    strcpy(items_to_store[1].misc_chars, "bar");
    items_to_store[1].any_integer = 13;
    strcpy(items_to_store[1].more_chars, "unlucky?");
    // 填充结构体数组的第三个元素
    strcpy(items_to_store[2].misc_chars, "Third");
    items_to_store[2].any_integer = 3;
    strcpy(items_to_store[2].more_chars, "baz");

    // 遍历结构体数组,存储每个元素到数据库
    for (i = 0; i < ITEMS_USED; i++)
    {
        // 构建键
        sprintf(key_to_use, "%c%c%d", items_to_store[i].misc_chars[0], items_to_store[i].more_chars[0], items_to_store[i].any_integer);

        // 构建键的datum结构
        key_datum.dptr  = key_to_use;
        key_datum.dsize = strlen(key_to_use);
        // 构建数据的datum结构
        data_datum.dptr  = (void *)&items_to_store[i];
        data_datum.dsize = sizeof(struct test_data);

        // 存储数据到数据库,如果键已存在则替换
        result = dbm_store(dbm_ptr, key_datum, data_datum, DBM_REPLACE);
        if (result != 0)
        {
            // 如果存储失败,打印错误信息并退出程序
            fprintf(stderr, "dbm_store failed on key %s\n", key_to_use);
            exit(2);
        }
    }  // for循环结束

    // 尝试删除一些数据
    sprintf(key_to_use, "bu%d", 13);  // 这是第二个元素的键
    // 将要使用的键的指针赋值给key_datum的dptr成员
    key_datum.dptr = key_to_use;

    // 计算键的长度,并将结果赋值给key_datum的dsize成员
    key_datum.dsize = strlen(key_to_use);

    // 删除键对应的数据,如果成功则打印确认信息
    // 删除指定键值的项
    // 参数:dbm_ptr - 指向数据库管理结构的指针
    //       key_datum - 要删除的键的数据结构
    if (dbm_delete(dbm_ptr, key_datum) == 0)
    {
        printf("Data with key %s deleted\n", key_to_use);
    }
    else
    {
        // 如果删除失败,打印未删除的信息
        printf("Nothing deleted for key %s\n", key_to_use);
    }

    // 遍历数据库中的所有键
    // 遍历数据库中的所有键值对
    for (key_datum = dbm_firstkey(dbm_ptr);  // 从数据库中获取第一个键值对
         key_datum.dptr;                     // 当指针不为空时,继续循环
         key_datum = dbm_nextkey(dbm_ptr))   // 获取下一个键值对
    {
        // 根据键检索数据
        data_datum = dbm_fetch(dbm_ptr, key_datum);
        if (data_datum.dptr)
        {
            // 如果检索到数据,打印检索到的信息
            printf("Data retrieved\n");
            // 将检索到的数据复制到item_retrieved结构体中
            memcpy(&item_retrieved, data_datum.dptr, data_datum.dsize);
            // 打印检索到的项目的详细信息
            // 打印检索到的项目信息
            // 参数分别为:
            // item_retrieved.misc_chars -杂项字符
            // item_retrieved.any_integer - 任意整数
            // item_retrieved.more_chars - 更多字符
            printf("Retrieved item - %s %d %s\n",
                   item_retrieved.misc_chars,
                   item_retrieved.any_integer,
                   item_retrieved.more_chars);
        }
        else
        {
            // 如果没有检索到数据,打印错误信息
            printf("Woops - no data found for key %s\n", key_to_use);
        }
    }  // for循环结束

    // 关闭数据库连接
    dbm_close(dbm_ptr);

    // 程序正常退出
    exit(EXIT_SUCCESS);
}
