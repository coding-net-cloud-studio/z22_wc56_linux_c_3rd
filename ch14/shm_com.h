/* 一个描述我们希望传递的共享内存的公共头文件. */

// 定义文本大小,此处为2048字节
#define TEXT_SZ 2048

// 定义一个结构体,用于共享内存区域的通信
struct shared_use_st
{
    // 用于标记当前共享内存区域的数据是否由你写入,1表示是你写入的,0表示不是
    int written_by_you;
    // 用于存储文本信息的字符数组,最大长度为TEXT_SZ
    char some_text[TEXT_SZ];
};
