all:	application


INCLUDE=/usr/include/gdbm
# LIBS=-lgdbm
# On some distributoions you may need to change the above line to include
# the compatability library, as shown below.
LIBS=-lgdbm_compat -lgdbm

# 定义编译器的标志,可以在后续使用中通过make CFLAGS=-O2来指定优化等级等
CFLAGS=-g

# 编译app_ui.c为app_ui.o,依赖app_ui.c和cd_data.h文件
app_ui.o: app_ui.c cd_data.h
	@# 使用gcc编译器,加上CFLAGS变量中的标志,-c表示只编译不链接
	gcc $(CFLAGS) -c app_ui.c

# 编译access.c为access.o,依赖access.c和cd_data.h文件,同时指定了额外的头文件搜索路径INCLUDE
access.o: access.c cd_data.h
	@# 使用gcc编译器,加上CFLAGS变量中的标志,-I指定头文件搜索路径,-c表示只编译不链接
	gcc $(CFLAGS) -I$(INCLUDE) -c access.c

# 链接app_ui.o和access.o生成可执行文件application,同时链接LIBS变量中指定的库
application:	app_ui.o access.o
	@# 使用gcc编译器,加上CFLAGS变量中的标志,-o指定输出文件名
	gcc $(CFLAGS) -o application app_ui.o access.o $(LIBS)

# 清理目标,删除生成的可执行文件和.o对象文件
clean:
	@# rm命令用于删除文件,-f表示强制删除不提示
	rm -f application *.o

# 删除数据库文件,通常是在不使用数据库时使用
nodbmfiles:
	@# 删除以.dir和.pag为后缀的文件
	rm -f *.dir *.pag
