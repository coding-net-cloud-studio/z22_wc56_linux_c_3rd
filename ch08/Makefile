# Makefile for Beginning Linux Programming 3rd Edition
# Chapter 8
# 注释:本Makefile用于编译和链接Linux编程入门第三版第8章的示例程序

# sudo apt update
# sudo apt install libmysqlclient-dev
# 注释:以上两行命令用于更新软件包列表并安装MySQL客户端开发库,这些命令需要在终端中手动执行

CONNECT=connect1 connect2
# 注释:CONNECT变量包含了需要编译的连接MySQL数据库的程序名

INSERT=insert1 insert2
# 注释:INSERT变量包含了需要编译的插入数据到MySQL数据库的程序名

SELECT=select1 select2 select3 select4
# 注释:SELECT变量包含了需要编译的从MySQL数据库查询数据的程序名

UPDATE=update1
# 注释:UPDATE变量包含了需要编译的更新MySQL数据库数据的程序名

CC=gcc
# 注释:CC变量定义了使用的C编译器为gcc

ALL=$(CONNECT) $(INSERT) $(SELECT) $(UPDATE)
# 注释:ALL变量包含了所有需要编译的程序的名称

all: $(ALL)
# 注释:all目标表示编译所有的程序

CFLAGS=-I/usr/include/mysql
# 注释:CFLAGS变量定义了编译时包含的头文件路径,指向MySQL客户端头文件目录

# CFLAGS=-I/usr/include/mysql -Wall -g
# 注释:这行被注释掉的代码显示了如何添加编译警告选项和调试信息选项

# LDFLAGS=-lmysqlclient -g
# 注释:这行被注释掉的代码显示了如何添加链接MySQL客户端库和调试信息的选项

# LDFLAGS=-lmysqlclient -g -L/usr/lib/mysql
# 注释:这行被注释掉的代码显示了如何指定链接MySQL客户端库的路径

LDFLAGS=-lmysqlclient
# 注释:LDFLAGS变量定义了链接MySQL客户端库的选项

# gcc connect1.c -o connect1 -I/usr/include/mysql -lmysqlclient
# gcc connect2.c -o connect2 -I/usr/include/mysql -lmysqlclient
# 注释:这两行被注释掉的代码显示了如何手动编译单个程序

%: %.c
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)
# 注释:这是一个模式规则,用于编译C源文件为目标可执行文件.其中$@代表目标,$<代表依赖的C源文件

clean:
	@rm -f $(ALL) *~
# 注释:clean目标用于清除编译生成的所有可执行文件和临时文件
