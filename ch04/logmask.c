// 引入syslog库,用于系统日志记录
#include <syslog.h>

// 引入标准输入输出库,用于文件操作和调试输出
#include <stdio.h>

// 引入unistd库,提供进程控制相关函数
#include <unistd.h>

// 引入stdlib库,提供动态内存管理函数
#include <stdlib.h>

// 引入string库,提供字符串操作函数
#include <string.h>

int main()
{
    // 定义日志掩码变量
    int logmask;

    // 使用指定的选项和设施打开系统日志
    openlog("logmask", LOG_PID | LOG_CONS, LOG_USER);

    // 记录一条信息级别的日志,包含进程ID
    syslog(LOG_INFO, "informative message, pid = %d", getpid());

    // 记录一条调试级别的日志,这条日志应该显示
    syslog(LOG_DEBUG, "debug message, should appear");

    // 设置日志掩码,只允许记录通知级别及以上的日志
    logmask = setlogmask(LOG_UPTO(LOG_NOTICE));

    // 尝试记录一条调试级别的日志,这条日志不应该显示
    syslog(LOG_DEBUG, "debug message, should not appear");

    // 退出程序
    exit(0);
}
