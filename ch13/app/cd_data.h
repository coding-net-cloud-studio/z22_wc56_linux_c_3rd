/* 数据结构定义部分 */

// 定义目录表中的各个字段长度常量
#define CAT_CAT_LEN    30  // 目录号长度
#define CAT_TITLE_LEN  70  // 标题长度
#define CAT_TYPE_LEN   30  // 类型长度
#define CAT_ARTIST_LEN 70  // 艺术家长度

// 定义目录条目结构体
typedef struct
{
    char catalog[CAT_CAT_LEN + 1];    // 目录号
    char title[CAT_TITLE_LEN + 1];    // 标题
    char type[CAT_TYPE_LEN + 1];      // 类型
    char artist[CAT_ARTIST_LEN + 1];  // 艺术家
} cdc_entry;

// 定义曲目表中的各个字段长度常量,使用已有目录表的长度常量
#define TRACK_CAT_LEN   CAT_CAT_LEN  // 目录号长度与目录表相同
#define TRACK_TTEXT_LEN 70           // 曲目文本长度

// 定义曲目条目结构体
typedef struct
{
    char catalog[TRACK_CAT_LEN + 1];      // 目录号
    int  track_no;                        // 曲目编号
    char track_txt[TRACK_TTEXT_LEN + 1];  // 曲目文本
} cdt_entry;

/* 函数声明部分 */

/* Now that we have some data structures, we can define some access routines that we'll need.
 Functions with cdc_ are for catalog entries; functions with cdt_ are for track entries.
 Notice that some of the functions return data structures.
 We can indicate the failure of these functions by forcing the contents of the structure to be empty.
*/

/* 现在我们已经有了一些数据结构,可以定义一些我们需要的访问例程.
带有cdc_前缀的函数用于目录条目;带有cdt_前缀的函数用于轨道条目.
请注意,一些函数返回数据结构.
我们可以通过使结构的内容为空来表示这些函数的失败.
*/

// 初始化和终止数据库的函数
int  database_initialize(const int new_database);  // 初始化数据库
void database_close(void);                         // 关闭数据库

// 用于简单数据检索的函数
// 获取目录条目
cdc_entry get_cdc_entry(const char *cd_catalog_ptr);
// 获取曲目条目
cdt_entry get_cdt_entry(const char *cd_catalog_ptr, const int track_no);

// 用于数据添加的函数
// 添加目录条目
int add_cdc_entry(const cdc_entry entry_to_add);
// 添加曲目条目
int add_cdt_entry(const cdt_entry entry_to_add);

// 用于数据删除的函数
// 删除目录条目
int del_cdc_entry(const char *cd_catalog_ptr);
// 删除曲目条目
int del_cdt_entry(const char *cd_catalog_ptr, const int track_no);

// 搜索功能函数
// 搜索目录条目
cdc_entry search_cdc_entry(const char *cd_catalog_ptr, int *first_call_ptr);
