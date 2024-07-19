/* 定义目录表常量及结构体 */
#define CAT_CAT_LEN    30 // 目录号长度
#define CAT_TITLE_LEN  70 // 标题长度
#define CAT_TYPE_LEN   30 // 类型长度
#define CAT_ARTIST_LEN 70 // 艺术家长度
typedef struct {
    char catalog[CAT_CAT_LEN + 1]; // 目录号
    char title[CAT_TITLE_LEN + 1];   // 标题
    char type[CAT_TYPE_LEN + 1];     // 类型
    char artist[CAT_ARTIST_LEN + 1]; // 艺术家
} cdc_entry;

/* 定义音轨表常量及结构体 */
#define TRACK_CAT_LEN   CAT_CAT_LEN // 音轨目录号长度
#define TRACK_TTEXT_LEN 70          // 音轨文本长度
typedef struct {
    char catalog[TRACK_CAT_LEN + 1]; // 目录号
    int  track_no;                  // 曲目号
    char track_txt[TRACK_TTEXT_LEN + 1]; // 音轨文本
} cdt_entry;

/* 定义访问函数,cdc_前缀用于目录条目,cdt_前缀用于音轨条目 */
/* 初始化和终止函数 */
int  database_initialize(const int new_database); // 初始化数据库
void database_close(void);                     // 关闭数据库

/* 两个简单的数据检索函数 */
cdc_entry get_cdc_entry(const char *cd_catalog_ptr); // 获取目录条目
cdt_entry get_cdt_entry(const char *cd_catalog_ptr, const int track_no); // 获取音轨条目

/* 两个数据添加函数 */
int add_cdc_entry(const cdc_entry entry_to_add); // 添加目录条目
int add_cdt_entry(const cdt_entry entry_to_add); // 添加音轨条目

/* 两个数据删除函数 */
int del_cdc_entry(const char *cd_catalog_ptr); // 删除目录条目
int del_cdt_entry(const char *cd_catalog_ptr, const int track_no); // 删除音轨条目

/* 一个搜索函数 */
cdc_entry search_cdc_entry(const char *cd_catalog_ptr, int *first_call_ptr); // 搜索目录条目
