# Makefile for Beginning Linux Programming 3rd Edition
# Chapter 3

# SIMPLE 变量包含两个简单的读写程序
# simple_write 用于写入文件
# simple_read 用于读取文件
SIMPLE=simple_write simple_read

# COPY 变量包含三个复制文件的程序
# copy_system 使用系统调用进行复制
# copy_block 使用块设备进行复制
# copy_stdio 使用标准I/O进行复制
COPY=copy_system copy_block copy_stdio

# DIR 变量包含两个打印目录内容的程序
# printdir 打印当前目录内容
# printdir2 打印指定目录内容
DIR=printdir printdir2

# MMAP 变量包含一个使用内存映射的文件操作程序
MMAP=mmap

# 定义所有目标变量
ALL = $(SIMPLE) $(COPY) $(DIR) $(MMAP)

# 默认构建所有目标
all: $(ALL)
    # 这里可以添加构建后执行的命令,例如清理临时文件等
    # @echo "所有目标已构建完成"

# 清理目标:删除所有生成的文件,输出文件和临时文件
clean:
	@rm -f $(ALL) file.out records.dat *~

