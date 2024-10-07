#!/usr/bin/env bash


# 定义一个函数,用于随机选择并显示一个_祝贺古诗_文本文件的内容
gs30_bg58_30_show_random_gs_text_file() {
    # 获取所有匹配sg*.txt模式的文件列表
    files=($(ls sg*.txt))

    # 检查是否有匹配的文件
    if [ ${#files[@]} -eq 0 ]; then
        echo "没有发现古诗文本文件."
        return 1
    fi

    # 随机选择一个文件
    random_index=$((RANDOM %${#files[@]}))
    selected_file="${files[$random_index]}"

    # 显示选中的文件内容
    cat "$selected_file"
}

# 调用函数
gs30_bg58_30_show_random_gs_text_file
