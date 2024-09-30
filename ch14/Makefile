# 默认目标,当执行`make`命令时,会依次构建msg1, msg2, shm1, shm2, sem1目标
all: msg1 msg2 shm1 shm2 sem1

# sem1.c文件依赖于semun.h头文件,构建sem1目标前需要先确保semun.h存在
sem1.c:	semun.h

# shm1.c文件依赖于shm_com.h头文件,构建shm1目标前需要先确保shm_com.h存在
shm1.c: shm_com.h

# shm2.c文件依赖于shm_com.h头文件,构建shm2目标前需要先确保shm_com.h存在
shm2.c:	shm_com.h

# 清理目标,执行`make clean`时会删除msg1, msg2, shm1, shm2, sem1这五个目标文件
clean:
	rm -f msg1 msg2 shm1 shm2 sem1

