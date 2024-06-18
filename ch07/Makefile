# 定义所有目标的列表
all: dbm1 dbm2 lock1 lock2 lock3 lock4 lock5 memory1 memory2 memory3 memory4 memory5a memory5b memory6

# 注释说明在Ubuntu中默认安装的是gdbm数据库,并提供了使用gdbm实现访问数据库的方法
# 以及如果需要以dbm或ndbm方式访问数据库,应如何操作
# https://www.cnblogs.com/yangbajing/archive/2009/11/17/1604765.html
# Ubuntu中默认安装的是gdbm数据库,应该使用gdbm实现来访问数据库.若还是需要以dbm若ndbm方式来访问数据库的话就需要使用gdbm-ndbm.h头文件,编译链接时应使用-lgdbm_compat链接选项
# gcc -lgdbm_compat -o dbm1 dbm1.c

# 定义包含gdbm头文件的路径
INCLUDE=/usr/include/gdbm

# 定义链接库,这里首先尝试直接链接gdbm库,如果系统需要兼容库,则注释掉第一行,使用下面的行
# LIBS=-lgdbm

# On some systems, you may have to change the line above to include the
# compatability library, as shown below. Note that the compatability library
# may have to come first.
# 如果系统需要兼容库,则使用下面的两行,注意兼容库可能需要在前面

LIBS=-lgdbm_compat -lgdbm


# You make also need to edit the files dbm1.c and dbm2.c to change the
# include of  <ndbm.h> to be <gdbm-ndbm.h>

# 提示可能需要编辑dbm1.c和dbm2.c文件,将<ndbm.h>改为<gdbm-ndbm.h>

# 定义dbm1的构建规则,指定了包含路径和链接库
dbm1: dbm1.c
	gcc -I$(INCLUDE) $< -o $@ $(LIBS)

# 定义dbm2的构建规则,与dbm1类似
dbm2: dbm2.c
	gcc -I$(INCLUDE) $< -o $@ $(LIBS)

# 定义清理目标,用于删除所有生成的可执行文件
clean:
	rm -f dbm1 dbm2 lock1 lock2 lock3 lock4 lock5 memory1 memory2 memory3 memory4 memory5a memory5b memory6
