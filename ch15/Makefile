# Makefile for Beginning Linux Programming 3rd Edition
# Chapter 15

# 定义客户端程序列表
CLIENTS=client1 client2 client3

# 定义服务器程序列表
SERVERS=server1 server2 server3 server4 server5

# 定义信息收集程序列表
INFO=getname getdate getdate-udp

# 定义选择程序的名称
SELECT=select

# 定义编译器标志
CFLAGS=-g

# 定义链接器标志
LDFLAGS=-g

# 将所有客户端,服务器端,信息收集和选择程序的名称合并为一个列表
ALL= $(CLIENTS) $(SERVERS) $(INFO) $(SELECT)

# 'all' 目标依赖于 'ALL' 变量中的所有程序
all: $(ALL)

# 'clean' 目标用于清理,删除所有生成的文件
clean:
	@rm -f $(ALL) *~ *.o
