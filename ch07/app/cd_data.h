/*
  This is the data stucture definition for the CD database.

  It defines structures and sizes for the two tables that comprise
  the database.

  */

/*
这是CD数据库的数据结构定义.

它定义了两个表格的结构和大小,这些表格构成了数据库.
*/

/* The catalog table */
// 定义CD数据的各个字段长度
#define CAT_CAT_LEN    30  // CD的类别长度
#define CAT_TITLE_LEN  70  // CD的标题长度
#define CAT_TYPE_LEN   30  // CD的类型长度
#define CAT_ARTIST_LEN 70  // CD的艺术家长度

// 定义一个结构体类型cdc_entry,用于表示CD目录条目
typedef struct
{
    // catalog字段用于存储CD的目录名称,长度为CAT_CAT_LEN + 1以留出空格
    char catalog[CAT_CAT_LEN + 1];
    // title字段用于存储CD的标题,长度为CAT_TITLE_LEN + 1以留出空格
    char title[CAT_TITLE_LEN + 1];
    // type字段用于存储CD的类型,长度为CAT_TYPE_LEN + 1以留出空格
    char type[CAT_TYPE_LEN + 1];
    // artist字段用于存储CD的艺术家名称,长度为CAT_ARTIST_LEN + 1以留出空格
    char artist[CAT_ARTIST_LEN + 1];
} cdc_entry;
// 定义曲目表中的目录长度,与目录表中的长度一致
#define TRACK_CAT_LEN   CAT_CAT_LEN
// 定义曲目文本的最大长度
#define TRACK_TTEXT_LEN 70

// 定义曲目表条目结构体
typedef struct
{
    char catalog[TRACK_CAT_LEN + 1];      // 曲目目录
    int  track_no;                        // 曲目编号
    char track_txt[TRACK_TTEXT_LEN + 1];  // 曲目文本描述
} cdt_entry;

// 为了方便,这里也放上了访问数据的函数原型

// 初始化和终止函数
int  database_initialize(const int new_database);  // 初始化数据库
void database_close(void);                         // 关闭数据库连接

// 两个简单的数据检索函数
cdc_entry get_cdc_entry(const char *cd_catalog_ptr);                      // 获取目录表条目
cdt_entry get_cdt_entry(const char *cd_catalog_ptr, const int track_no);  // 根据曲目编号获取曲目表条目

// 两个数据添加函数
int add_cdc_entry(const cdc_entry entry_to_add);  // 添加目录表条目
int add_cdt_entry(const cdt_entry entry_to_add);  // 添加曲目表条目

// 两个数据删除函数
int del_cdc_entry(const char *cd_catalog_ptr);                      // 删除目录表条目
int del_cdt_entry(const char *cd_catalog_ptr, const int track_no);  // 根据曲目编号删除曲目表条目

// 一个搜索函数
cdc_entry search_cdc_entry(const char *cd_catalog_ptr, int *first_call_ptr);  // 搜索目录表条目
