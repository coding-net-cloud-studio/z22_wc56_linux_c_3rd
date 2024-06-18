# Makefile for Beginning Linux Programming 3rd Edition
# Chapter 4

# ARGS 变量包含命令行参数示例
ARGS=args argopt longopt

# ENV 变量用于展示环境变量操作
ENV=environ showenv

# TIME 变量包含与时间相关的函数
TIME=envtime gmtime ctime strftime

# TMP 变量用于演示临时文件名生成
TMP=tmpnam

# USER 变量用于获取当前用户信息
USER=user

# HOST 变量用于获取主机信息
HOST=hostget

# LOG 变量包含与日志记录相关的函数
LOG=syslog logmask

# RES 变量用于获取系统资源限制
RES=limits

# LDFLAGS 变量包含链接器标志,这里仅为示例,通常不包含在Makefile的规则中
LDFLAGS=-lm

# NOTE 原始内容
# ALL= $(ARGS) $(ENV) $(TIME) $(TMP) $(USER) $(HOST) $(LOG) $(RES)

# NOTE 修改内容
# 删除了$(RES)变量,可能是因为不再需要它
ALL= $(ARGS) $(ENV) $(TIME) $(TMP) $(USER) $(HOST) $(LOG)

# all目标是构建ALL变量中定义的所有内容,并执行limits目标
all: $(ALL) limits

# limits目标编译limits.c文件,生成名为limits的可执行文件
limits: limits.c
	gcc -o limits limits.c $(LDFLAGS)

# clean目标删除ALL变量中定义的所有内容,RES变量以及所有临时文件
clean:
	@rm -f $(ALL) $(RES) *~
