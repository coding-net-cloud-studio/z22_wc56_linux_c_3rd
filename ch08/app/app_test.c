// 引入标准库,提供内存分配,输入输出和字符串操作功能
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// 引入自定义的MySQL应用接口头文件,用于数据库操作
#include "app_mysql.h"

int main()
{
  // 定义结构体变量用于存储CD和音轨信息
  struct current_cd_st cd;
  struct cd_search_st cd_res;
  struct current_tracks_st ct;
  int cd_id;  // 用于存储新添加的CD的ID
  int res, i; // res用于存储函数调用的结果,i用于循环计数

  // 启动数据库连接,使用用户名"rick"和密码"secret"
  database_start("rick", "secret");

  // 添加一张CD到数据库,并获取其ID
  res = add_cd("Mahler", "Symphony No 1", "4596102", &cd_id);
  // 打印添加CD的结果和CD的ID
  printf("添加CD的结果是%d,CD的ID是%d\n", res, cd_id);

  // 初始化音轨结构体
  memset(&ct, 0, sizeof(ct));
  ct.cd_id = cd_id;
  // 将音轨名称复制到结构体中
  strcpy(ct.track[0], "Lansam Schleppend");
  strcpy(ct.track[1], "Kraftig bewegt");
  strcpy(ct.track[2], "Feierlich und gemesen");
  strcpy(ct.track[3], "Sturmisch bewegt");
  // 将音轨添加到数据库中的相应CD
  add_tracks(&ct);

  // 根据关键字搜索CD
  res = find_cds("Symphony", &cd_res);
  // 打印找到的CD数量和第一个CD的ID
  printf("找到%d张CD,第一张的ID是%d\n", res, cd_res.cd_id[0]);

  // 根据ID获取CD信息
  res = get_cd(cd_res.cd_id[0], &cd);
  // 打印获取CD信息的返回值
  printf("get_cd返回%d\n", res);

  // 重新初始化音轨结构体以获取CD的音轨信息
  memset(&ct, 0, sizeof(ct));
  res = get_cd_tracks(cd_res.cd_id[0], &ct);
  // 打印获取音轨信息的返回值
  printf("get_cd_tracks返回%d\n", res);
  // 打印CD标题
  printf("标题: %s\n", cd.title);
  // 循环打印所有音轨名称
  i = 0;
  while (i < res)
  {
    printf("\t音轨 %d 是 %s\n", i, ct.track[i]);
    i++;
  }

  // 根据ID删除CD
  res = delete_cd(cd_res.cd_id[0]);
  // 打印删除CD的返回值
  printf("Delete_cd返回%d\n", res);

  // 结束数据库连接
  database_end();

  return EXIT_SUCCESS;
}
