// 引入所需的头文件
#include <sys/types.h>  // 用于定义各种数据类型和宏
#include <pwd.h>        // 用于获取用户信息
#include <stdio.h>      // 用于输入输出功能
#include <unistd.h>     // 用于进程控制
#include <stdlib.h>     // 用于通用函数,如内存分配和字符串转换

#include <stdio.h>      // 包含标准输入输出库,用于文件操作和打印输出
#include <unistd.h>     // 包含系统调用和实用工具函数,如getpid()
#include <sys/types.h>  // 包含系统数据类型定义,如pid_t
#include <pwd.h>        // 包含用户账户信息的结构体和函数,如getpwuid()

int main()
{
    // 获取当前用户的用户ID和组ID

    // 定义用户ID变量
    uid_t uid;

    // 定义组ID变量
    gid_t gid;

    // 定义指向passwd结构体的指针,用于存储用户信息
    struct passwd *pw;

    // 获取当前用户的用户ID和组ID
    // 获取当前进程的用户ID
    uid = getuid();

    // 获取当前进程的组ID
    gid = getgid();

    // 打印当前登录用户的名字
    printf("用户是 %s\n", getlogin());

    // 打印用户ID和组ID
    printf("用户ID: uid=%d, gid=%d\n", uid, gid);

    // 通过用户ID获取密码条目
    pw = getpwuid(uid);
    // 打印用户ID对应的密码条目的详细信息
    // 打印用户信息
    // 该函数用于输出用户的详细信息,包括用户名,用户ID,组ID,主目录和壳层
    printf("UID密码条目:\n名字=%s, 用户ID=%d, 组ID=%d, 主目录=%s, 壳层=%s\n",
           pw->pw_name,    // 用户名
           pw->pw_uid,     // 用户ID
           pw->pw_gid,     // 组ID
           pw->pw_dir,     // 主目录
           pw->pw_shell);  // 壳层

    // 通过用户名"root"获取密码条目
    pw = getpwnam("root");
    // 打印用户名为"root"的密码条目的详细信息
    printf("root密码条目:\n");
    // 打印用户信息
    // 该代码段使用printf函数打印出用户的名字,用户ID,组ID,主目录和壳层信息
    // pw是一个指向passwd结构体的指针,该结构体包含了用户的相关信息
    printf("名字=%s, 用户ID=%d, 组ID=%d, 主目录=%s, 壳层=%s\n",
           pw->pw_name,  // 用户名
           pw->pw_uid,   // 用户ID
           pw->pw_gid,   // 组ID
           pw->pw_dir,   // 主目录
           pw->pw_shell  // 壳层
    );

    // 程序正常退出
    exit(0);
}
