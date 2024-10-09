#!/usr/bin/make -f

# makefile中使用echo -e引发的思考(涉及dash和bash)
# https://blog.csdn.net/benkaoya/article/details/12410295
SHELL=/bin/bash

# 设置编译器
CC = gcc

# 设置编译选项和头文件路径
CFLAGS = -Wall -I./include

# 子目录列表
SUBDIRS = ch03 ch04 ch05 ch06 ch07 ch08 ch09 ch10 ch11 ch12 ch13 ch14 ch15 ch06/app ch07/app ch08/app ch13/app ch14/app

# 默认目标
all: $(SUBDIRS)

# 递归规则,用于遍历每个子目录并调用其 Makefile
$(SUBDIRS):
	$(MAKE) -C $@

# 清理目标
clean:
	for dir in $(SUBDIRS); do \
			$(MAKE) -C $$dir clean; \
	done

# 通过git clean -xdf -n 查看构建了哪些_可执行文件
show:
	-@git clean -xdf -n

# 使用bash执行脚本_安装一下需要用到的软件
setup:
	-@bash ./02_setup_env.sh

# 定义伪目标,防止 make 时没有指定目标而报错
.PHONY: all clean $(SUBDIRS) show setup

.PHONY: help

help:
	@echo "Makefile 帮助信息:"
	@echo ""
	@echo "可用的构建目标:"
	@echo "  all       : 构建项目(默认) "
	@echo "  setup     : 安装一下需要用到的软件与库 "
	@echo "  show      : 通过git clean -xdf -n 查看构建了哪些_可执行文件 "
	@echo "  clean     : 清理生成的文件"
	@echo "  help      : 显示此帮助信息"
	@echo ""
	@echo "示例命令:"
	@echo "  make: 构建项目"
	@echo "  make clean : 清理生成的文件"
	@echo "  make help  : 显示此帮助信息"

