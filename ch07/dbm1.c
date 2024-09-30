#include <unistd.h>  // 包含unistd.h头文件,提供对POSIX操作系统API的访问
#include <stdlib.h>  // 包含stdlib.h头文件,提供内存分配和程序控制功能
#include <stdio.h>   // 包含stdio.h头文件,提供标准输入输出功能
#include <fcntl.h>   // 包含fcntl.h头文件,提供文件控制功能

#include <ndbm.h>  // 包含ndbm.h头文件,提供NDBM数据库操作的函数原型

/* On some systems you need to replace the above with
#include <gdbm-ndbm.h>
*/

/* 在某些系统上,您需要将上面的代码替换为
#include <gdbm-ndbm.h>
*/

#include <string.h>

// 定义测试数据库文件的路径
#define TEST_DB_FILE "/tmp/dbm1_test"
// 定义测试中使用的数据项数量
#define ITEMS_USED   3

/* 用于测试dbm的结构体
 * struct test_data包含了用于测试的随机字符数组,任意整数和更多字符数组.
 */
struct test_data
{
    char misc_chars[15];  // 随机字符数组,长度为15
    int  any_integer;     // 任意整数
    char more_chars[21];  // 更多的字符数组,长度为21
};

int main()
{

    // 定义用于存储的数据结构数组
    struct test_data items_to_store[ITEMS_USED];
    // 定义用于检索的数据结构变量
    struct test_data item_retrieved;

    // 定义用于存储键的字符数组
    char key_to_use[20];
    // 定义循环变量和结果变量
    int i, result;

    // 定义用于存储数据库键和数据的datum类型变量
    datum key_datum;
    datum data_datum;

    // 定义指向DBM数据库的指针
    DBM *dbm_ptr;

    // 打开数据库,如果失败则输出错误信息并退出程序
    // TEST_DB_FILE是数据库文件的路径,O_RDWR | O_CREAT表示以读写模式打开,如果不存在则创建,0666是文件权限
    dbm_ptr = dbm_open(TEST_DB_FILE, O_RDWR | O_CREAT, 0666);
    if (!dbm_ptr)
    {
        fprintf(stderr, "Failed to open database\n");
        exit(EXIT_FAILURE);
    }

    /* put some data in the structures */

    /* 将一些数据放入结构体中 */

    // 初始化存储项数组,将其所有内容设置为'\0'
    memset(items_to_store, '\0', sizeof(items_to_store));

    // 将字符串"First!"复制到items_to_store数组的第一个元素的misc_chars字段
    strcpy(items_to_store[0].misc_chars, "First!");
    // 设置items_to_store数组的第一个元素的any_integer字段为47
    items_to_store[0].any_integer = 47;
    // 将字符串"foo"复制到items_to_store数组的第一个元素的more_chars字段
    strcpy(items_to_store[0].more_chars, "foo");

    // 将字符串"bar"复制到items_to_store数组的第二个元素的misc_chars字段
    strcpy(items_to_store[1].misc_chars, "bar");
    // 设置items_to_store数组的第二个元素的any_integer字段为13
    items_to_store[1].any_integer = 13;
    // 将字符串"unlucky?"复制到items_to_store数组的第二个元素的more_chars字段
    strcpy(items_to_store[1].more_chars, "unlucky?");

    // 将字符串"Third"复制到items_to_store数组的第三个元素的misc_chars字段
    strcpy(items_to_store[2].misc_chars, "Third");
    // 设置items_to_store数组的第三个元素的any_integer字段为3
    items_to_store[2].any_integer = 3;
    // 将字符串"baz"复制到items_to_store数组的第三个元素的more_chars字段
    strcpy(items_to_store[2].more_chars, "baz");

    // 为选定的代码添加注释
    // 如果遇到函数方法,请添加标准的文档注释
    // 不要修改活动选择代码中的任何字符

    for (i = 0; i < ITEMS_USED; i++)  // 遍历所有要存储的项目
    {
        /* 构建要使用的键 */
        // 使用sprintf函数格式化键字符串,包括项目的misc_chars的第一个字符,more_chars的第一个字符和any_integer
        sprintf(key_to_use, "%c%c%d", items_to_store[i].misc_chars[0], items_to_store[i].more_chars[0], items_to_store[i].any_integer);

        /* 构建键数据结构 */
        // 设置键数据结构的指针指向构建的键字符串,大小为键字符串的长度
        key_datum.dptr  = (void *)key_to_use;
        key_datum.dsize = strlen(key_to_use);

        // 设置数据数据结构的指针指向当前项目,大小为项目的结构体大小
        // 为选定的代码添加注释
        // 以下代码段将数组items_to_store中的第i个元素的地址赋给data_datum的dptr成员
        // 并将test_data结构体的大小赋给data_datum的dsize成员
        // 这样做是为了存储或操作items_to_store中的数据
        data_datum.dptr  = (void *)&items_to_store[i];  // 将items_to_store中第i个元素的地址转换为void指针并赋给dptr
        data_datum.dsize = sizeof(struct test_data);    // 获取test_data结构体的大小并赋给dsize

        // 使用dbm_store函数将键值对存储到数据库中,如果键已存在则替换
        result = dbm_store(dbm_ptr, key_datum, data_datum, DBM_REPLACE);
        // 如果存储失败,打印错误信息并退出程序
        if (result != 0)
        {
            fprintf(stderr, "dbm_store failed on key %s\n", key_to_use);
            exit(2);
        }
    } /* for */

    /* 现在尝试检索一些数据 */
    // 使用sprintf函数格式化字符串,生成用于查找的键值,这里的"bu%d"表示一个以"bu"开头后跟数字的字符串,数字为13
    sprintf(key_to_use, "bu%d", 13); /* 这是第二个条目的键 */
    // 将生成的键值字符串的指针赋给key_datum结构体的dptr成员
    key_datum.dptr = key_to_use;
    // 计算键值字符串的长度,并赋给key_datum结构体的dsize成员
    key_datum.dsize = strlen(key_to_use);

    // 从数据库中获取与给定键关联的数据
    data_datum = dbm_fetch(dbm_ptr, key_datum);

    // 检查是否成功获取数据
    if (data_datum.dptr)
    {
        // 如果数据指针不为空,表示成功获取数据
        printf("Data retrieved\n");

        // 将获取的数据复制到结构体变量中
        // 从数据项中复制数据到item_retrieved变量
        // data_datum.dptr指向要复制的数据的指针
        // data_datum.dsize是要复制的字节数
        memcpy(&item_retrieved, data_datum.dptr, data_datum.dsize);

        // 打印获取的数据项的详细信息
        // 打印检索到的项目信息
        // item_retrieved.misc_chars: 检索到的项目的杂项字符
        // item_retrieved.any_integer: 检索到的项目的任意整数
        // item_retrieved.more_chars: 检索到的项目的更多字符
        printf("Retrieved item - %s %d %s\n",
               item_retrieved.misc_chars,
               item_retrieved.any_integer,
               item_retrieved.more_chars);
    }
    else
    {
        // 如果数据指针为空,表示未找到与键关联的数据
        printf("No data found for key %s\n", key_to_use);
    }

    dbm_close(dbm_ptr);  // 关闭数据库指针指向的数据库,释放相关资源
    exit(EXIT_SUCCESS);  // 程序正常退出,返回成功状态码
}
