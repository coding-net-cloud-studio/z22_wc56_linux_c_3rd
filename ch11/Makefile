# Makefile 用于《Beginning Linux Programming》第三版
# 第11章

# 定义系统变量，包含所有示例程序的名称
SYSTEM=system1 system2 pexec

# 定义fork相关的示例程序名称
FORK=fork1 wait fork2

# 定义过滤器相关的示例程序名称
FILTER=upper useupper

# 定义信号处理相关的示例程序名称
SIGNALS=ctrlc1 alarm ctrlc2

# 编译标志，用于启用调试信息
CFLAGS=-g

# 链接标志，同样用于启用调试信息
LDFLAGS=-g

# 定义所有示例程序的集合
ALL= $(SYSTEM) $(FORK) $(FILTER) $(SIGNALS)

# 默认目标，构建所有示例程序
all: $(ALL)

# 清理目标，移除所有编译生成的文件
clean:
	@rm -f $(ALL) *~ *.o
