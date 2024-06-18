/* 当前CD的简化结构,不包括曲目信息 */
struct current_cd_st
{
  int artist_id;         // 艺术家ID
  int cd_id;             // CD ID
  char artist_name[100]; // 艺术家姓名
  char title[100];       // CD标题
  char catalogue[100];   // CD目录号
};

/* 简化的曲目详情结构 */
struct current_tracks_st
{
  int cd_id;           // CD ID
  char track[20][100]; // 曲目名称数组
};

#define MAX_CD_RESULT 10 // 最大搜索结果数量
struct cd_search_st
{
  int cd_id[MAX_CD_RESULT]; // 存储搜索结果的CD ID数组
};

/* 数据库后端函数 */
// 启动数据库连接,参数为数据库名称和密码
int database_start(char *name, char *password);
// 结束数据库连接
void database_end();

/* 添加CD的函数 */
// 添加CD,参数包括艺术家,标题,目录号,返回CD ID
int add_cd(char *artist, char *title, char *catalogue, int *cd_id);
// 添加曲目,参数为当前曲目结构体指针
int add_tracks(struct current_tracks_st *tracks);

/* 查找和检索CD的函数 */
// 根据搜索字符串查找CD,结果存储在cd_search_st结构体中
int find_cds(char *search_str, struct cd_search_st *results);
// 根据CD ID获取CD信息,存储在当前CD结构体中
int get_cd(int cd_id, struct current_cd_st *dest);
// 根据CD ID获取曲目信息,存储在当前曲目结构体中
int get_cd_tracks(int cd_id, struct current_tracks_st *dest);

/* 删除项目的函数 */
// 根据CD ID删除CD
int delete_cd(int cd_id);
