/*  We start by defining a RECORD structure
    and then create NRECORDS versions each recording their number.
    These are appended to the file records.dat.  */

#include <unistd.h>
#include <stdio.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <stdlib.h>

// 定义一个RECORD结构体,包含一个整数和一个字符串数组
typedef struct
{
    int  integer;
    char string[24];
} RECORD;

// 定义记录的数量
#define NRECORDS (100)

int main()
{
    // 声明一个RECORD类型的变量和一个指向RECORD的指针
    RECORD record, *mapped;
    // 声明循环计数器和文件指针
    int   i, f;
    FILE *fp;

    // 打开一个名为"records.dat"的文件,用于读写
    fp = fopen("records.dat", "w+");
    // 循环创建NRECORDS个记录,并将它们写入文件
    for (i = 0; i < NRECORDS; i++)
    {
        record.integer = i;
        sprintf(record.string, "RECORD-%d", i);
        fwrite(&record, sizeof(record), 1, fp);
    }
    // 关闭文件
    fclose(fp);

    /*  We now change the integer value of record 43 to 143
        and write this to the 43rd record's string.  */

    // 重新打开文件,用于读写
    fp = fopen("records.dat", "r+");
    // 将文件指针定位到第43个记录的位置
    fseek(fp, 43 * sizeof(record), SEEK_SET);
    // 读取第43个记录
    fread(&record, sizeof(record), 1, fp);

    // 修改第43个记录的整数值
    record.integer = 143;
    // 更新第43个记录的字符串
    sprintf(record.string, "RECORD-%d", record.integer);

    // 将修改后的第43个记录写回文件
    fseek(fp, 43 * sizeof(record), SEEK_SET);
    fwrite(&record, sizeof(record), 1, fp);
    // 关闭文件
    fclose(fp);

    /*  We now map the records into memory
    and access the 43rd record in order to change the integer to 243
    (and update the record string), again using memory mapping.  */

    // 使用内存映射来访问文件中的记录
    f = open("records.dat", O_RDWR);
    // 将文件映射到内存中
    mapped = (RECORD *)mmap(0, NRECORDS * sizeof(record), PROT_READ | PROT_WRITE, MAP_SHARED, f, 0);

    // 通过内存映射修改第43个记录的整数值
    mapped[43].integer = 243;
    // 更新第43个记录的字符串
    sprintf(mapped[43].string, "RECORD-%d", mapped[43].integer);

    // 将内存中的更改同步到文件
    msync((void *)mapped, NRECORDS * sizeof(record), MS_ASYNC);
    // 解除内存映射
    munmap((void *)mapped, NRECORDS * sizeof(record));
    // 关闭文件描述符
    close(f);

    // 程序退出
    exit(0);
}
