# 编译所有线程示例程序
all: thread1 thread2 thread3 thread3a thread4 thread5 thread6 thread6a thread7 thread8 thread8a

# 定义编译器标志,启用可重入函数支持(用于线程安全)
CFLAGS=-D_REENTRANT
# 定义链接器标志,链接 pthread 库以支持线程
LDFLAGS=-lpthread
# 定义使用的编译器为 gcc
CC=gcc

%: %.c
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)
# 注释:这是一个模式规则,用于编译C源文件为目标可执行文件.其中$@代表目标,$<代表依赖的C源文件

clean:
	rm -f thread1 thread2 thread3 thread3a thread4 thread5 thread6 thread6a thread7 thread7a thread8 thread8a


