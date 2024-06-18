// 引入所需的头文件
#include <sys/types.h> // 用于定义各种数据类型和宏
#include <pwd.h>       // 用于获取用户信息
#include <stdio.h>     // 用于输入输出功能
#include <unistd.h>    // 用于进程控制
#include <stdlib.h>    // 用于通用函数,如内存分配和字符串转换

// 引入必要的头文件
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>

int main()
{
  // 获取当前用户的用户ID和组ID
  uid_t uid;
  gid_t gid;
  struct passwd *pw;

  // 获取当前用户的用户ID和组ID
  uid = getuid();
  gid = getgid();

  // 打印当前登录用户的名字
  printf("用户是 %s\n", getlogin());

  // 打印用户ID和组ID
  printf("用户ID: uid=%d, gid=%d\n", uid, gid);

  // 通过用户ID获取密码条目
  pw = getpwuid(uid);
  // 打印用户ID对应的密码条目的详细信息
  printf("UID密码条目:\n名字=%s, 用户ID=%d, 组ID=%d, 主目录=%s, 壳层=%s\n",
         pw->pw_name, pw->pw_uid, pw->pw_gid, pw->pw_dir, pw->pw_shell);

  // 通过用户名"root"获取密码条目
  pw = getpwnam("root");
  // 打印用户名为"root"的密码条目的详细信息
  printf("root密码条目:\n");
  printf("名字=%s, 用户ID=%d, 组ID=%d, 主目录=%s, 壳层=%s\n",
         pw->pw_name, pw->pw_uid, pw->pw_gid, pw->pw_dir, pw->pw_shell);

  // 程序正常退出
  exit(0);
}
