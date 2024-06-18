#!/bin/bash

# 非常简单的示例 shell 脚本,用于管理 CD 收藏.
# 版权 (C) 1996-2007 Wiley Publishing Inc.

# 本程序是自由软件;您可以依据自由软件基金会发布的 GNU 通用公共许可证条款重新分发和/或修改本程序;
# 您可以选择许可证的任一后续版本.

# 本程序的发布希望对您有用,但不提供任何形式的保证;甚至不保证适销性或特定用途的适用性.
# 有关更多详细信息,请参阅 GNU 通用公共许可证.

# 如果您没有与本程序一起收到 GNU 通用公共许可证的副本,请写信给:
# Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

# 首先要做的是确保我们在整个脚本中使用的一些全局变量已经设置好.
# 我们设置了标题文件和曲目文件以及一个临时文件.
# 我们还捕获了 Ctrl-C,这样如果用户中断脚本,我们的临时文件将被删除.

menu_choice=""  # 用户选择的菜单项
current_cd=""   # 当前选中的CD
title_file="title.cdb"  # CD标题文件
tracks_file="tracks.cdb"  # CD曲目文件
temp_file=/tmp/cdb.$  # 临时文件,用于存储中间结果
trap 'rm -f $temp_file' EXIT  # 当脚本退出时,删除临时文件

# 现在我们定义我们的函数,以便脚本从顶部行执行时,可以在首次尝试调用任何函数之前找到所有函数定义.
# 为了避免在多个地方重写相同的代码,前两个函数是简单的实用工具.

get_return() {
  echo -e "按回车键 \c"  # 提示用户按回车键
  read x  # 读取用户的输入
  return 0  # 函数返回0,表示成功执行
}

get_confirm() {
  echo -e "你确定吗? \c"  # 提示用户确认操作
  while true  # 循环直到用户输入yes或no
  do
    read x  # 读取用户的输入
    case "$x" in
      y | yes | Y | Yes | YES )  # 如果用户输入yes或其等效形式
        return 0;;  # 函数返回0,表示用户确认
      n | no  | N | No  | NO )  # 如果用户输入no或其等效形式
        echo  # 输出空行
        echo "已取消"  # 提示用户操作已取消
        return 1;;  # 函数返回1,表示用户取消操作
      *) echo "请输入是或否"  # 如果用户输入既不是yes也不是no
    esac
  done
}

# 接下来是主菜单函数 set_menu_choice.
# 菜单的内容动态变化,如果选择了 CD 条目,会添加额外的选项.
# 注意,echo -e 可能不适用于某些 shell.

set_menu_choice() {
  clear  # 清屏
  echo "选项 :-"  # 输出菜单选项
  echo
  echo "   a) 添加新 CD"
  echo "   f) 查找 CD"
  echo "   c) 计算目录中 CD 和曲目的数量"
  if [ "$cdcatnum" != "" ]; then  # 如果当前有选中的CD
    echo "   l) 列出 $cdtitle 的曲目"
    echo "   r) 移除 $cdtitle"
    echo "   u) 更新 $cdtitle 的曲目信息"
  fi
  echo "   q) 退出"
  echo
  echo -e "请输入选择后按回车键 \c"  # 提示用户输入选择
  read menu_choice  # 读取用户的选择
  return  # 函数返回
}

# 两个更短的函数,insert_title 和 insert_track 用于添加到数据库文件.
# 尽管有些人讨厌这样的单行函数,但它们有助于使其他函数更清晰.
# 它们后面跟着较大的 add_record_track 函数,该函数使用它们.
# 此函数使用模式匹配来确保不输入逗号(因为我们使用逗号作为字段分隔符),并且还使用算术运算来递增当前曲目编号
# 随着曲目的输入.

insert_title() {
  echo $* >> $title_file  # 将参数追加到标题文件
  return  # 函数返回
}

insert_track() {
  echo $* >> $tracks_file  # 将参数追加到曲目文件
  return  # 函数返回
}

add_record_tracks() {
  echo "输入此 CD 的曲目信息"  # 提示用户输入曲目信息
  echo "没有更多曲目时输入 q"  # 提示用户在输入完曲目后输入q
  cdtrack=1  # 初始化曲目编号为1
  cdttitle=""  # 初始化曲目标题为空
  while [ "$cdttitle" != "q" ]  # 循环直到用户输入q
  do
      echo -e "曲目 $cdtrack, 曲目标题? \c"  # 提示用户输入曲目标题
      read tmp  # 读取用户输入的曲目标题
      cdttitle=${tmp%%,*}  # 从用户输入中去除尾部的逗号
      if [ "$tmp" != "$cdttitle" ]; then  # 如果用户输入包含逗号
        echo "抱歉,不允许使用逗号"  # 提示用户输入错误
        continue  # 跳过当前循环,继续下一次循环
      fi
      if [ -n "$cdttitle" ] ; then  # 如果用户输入了有效的曲目标题
        if [ "$cdttitle" != "q" ]; then  # 如果用户没有输入q
          insert_track $cdcatnum,$cdtrack,$cdttitle  # 将曲目信息追加到曲目文件
        fi
      else
        cdtrack=$((cdtrack-1))  # 如果没有输入有效标题,曲目编号减1
      fi
    cdtrack=$((cdtrack+1))  # 曲目编号加1
  done
}

# add_records 函数允许输入新 CD 的主要信息.

add_records() {
  # 提示输入初始信息

  echo -e "输入目录名称 \c"  # 提示用户输入目录名称
  read tmp  # 读取用户输入的目录名称
  cdcatnum=${tmp%%,*}  # 从用户输入中去除尾部的逗号

  echo -e "输入标题 \c"  # 提示用户输入标题
  read tmp  # 读取用户输入的标题
  cdtitle=${tmp%%,*}  # 从用户输入中去除尾部的逗号

  echo -e "输入类型 \c"  # 提示用户输入类型
  read tmp  # 读取用户输入的类型
  cdtype=${tmp%%,*}  # 从用户输入中去除尾部的逗号

  echo -e "输入艺术家/作曲家 \c"  # 提示用户输入艺术家/作曲家
  read tmp  # 读取用户输入的艺术家/作曲家
  cdac=${tmp%%,*}  # 从用户输入中去除尾部的逗号

  # 检查他们是否想要输入信息

  echo 即将添加新条目
  echo "$cdcatnum $cdtitle $cdtype $cdac"  # 显示即将添加的CD信息

  # 如果得到确认,则将其追加到标题文件中

  if get_confirm ; then  # 如果用户确认添加
    insert_title $cdcatnum,$cdtitle,$cdtype,$cdac  # 将CD信息追加到标题文件
    add_record_tracks  # 调用函数添加曲目信息
  else
    remove_records  # 否则,调用函数移除记录
  fi

  return  # 函数返回
}

# find_cd 函数在 CD 标题文件中搜索目录名称文本,使用 grep 命令.
# 我们需要知道字符串被找到的次数,但 grep 只返回一个值告诉我们它是匹配零次还是多次.
# 为了解决这个问题,我们将输出存储在一个文件中,该文件将每匹配一次就有一行,然后计算文件中的行数.
# word count 命令 wc 在其输出中有空格,分隔文件中的行数,单词数和字符数.
# 我们使用 $(wc -l $temp_file) 表示法提取输出中的第一个参数,以设置 linesfound 变量.
# 如果我们想要另一个参数,稍后我们会使用 set 命令将 shell 的参数变量设置为命令输出.
# 我们将 IFS(内部字段分隔符)更改为 ,(逗号),以便我们可以分隔逗号分隔的字段.
# 另一个命令是 cut.

find_cd() {
  if [ "$1" = "n" ]; then  # 如果参数为n,表示不需要再次询问列表曲目
    asklist=n
  else
    asklist=y
  fi
  cdcatnum=""  # 初始化目录编号为空
  echo -e "输入要在 CD 标题中搜索的字符串 \c"  # 提示用户输入搜索字符串
  read searchstr  # 读取用户输入的搜索字符串
  if [ "$searchstr" = "" ]; then  # 如果用户没有输入任何内容
    return 0  # 函数返回0,表示没有进行搜索
  fi

  grep "$searchstr" $title_file > $temp_file  # 在标题文件中搜索字符串,并将结果保存到临时文件

  set $(wc -l $temp_file)  # 计算临时文件中的行数
  linesfound=$1  # 保存行数

  case "$linesfound" in
  0)    echo "抱歉,未找到任何内容"  # 如果没有找到匹配项
        get_return  # 提示用户按回车键
        return 0  # 函数返回0,表示没有找到匹配项
        ;;
  1)    ;;  # 如果找到一个匹配项,不做任何操作
  2)    echo "抱歉,不唯一."  # 如果找到两个匹配项
        echo "找到以下内容"  # 显示找到的匹配项
        cat $temp_file  # 显示临时文件内容
        get_return  # 提示用户按回车键
        return 0  # 函数返回0,表示匹配项不唯一
  esac

  IFS=","  # 设置内部字段分隔符为逗号
  read cdcatnum cdtitle cdtype cdac < $temp_file  # 从临时文件中读取目录编号,标题,类型和艺术家/作曲家
  IFS=" "  # 将内部字段分隔符重置为空格

  if [ -z "$cdcatnum" ]; then  # 如果没有提取到目录编号
    echo "抱歉,无法从 $temp_file 中提取目录字段"  # 提示错误信息
    get_return  # 提示用户按回车键
    return 0  # 函数返回0,表示提取失败
  fi

  echo  # 输出空行
  echo 目录编号: $cdcatnum  # 显示目录编号
  echo 标题: $cdtitle  # 显示标题
  echo 类型: $cdtype  # 显示类型
  echo 艺术家/作曲家: $cdac  # 显示艺术家/作曲家
  echo  # 输出空行
  get_return  # 提示用户按回车键

  if [ "$asklist" = "y" ]; then  # 如果需要再次询问列表曲目
    echo -e "查看此 CD 的曲目? \c"  # 提示用户是否查看曲目
      read x  # 读取用户输入
    if [ "$x" = "y" ]; then  # 如果用户选择查看曲目
      echo  # 输出空行
      list_tracks  # 调用函数列出曲目
      echo  # 输出空行
    fi
  fi
  return 1  # 函数返回1,表示找到了匹配项
}

# update_cd 允许我们重新输入 CD 的信息.注意我们搜索(grep)
# 以 ^${cdcatnum}, 开头的行,并且我们需要将 ${cdcatnum} 的展开包装在 {} 中,
# 以便我们可以搜索与目录编号之间没有空格的 ,.这个函数还使用 {} 来封装多个语句
# 如果 get_confirm 返回 true 则执行.

update_cd() {
  if [ -z "$cdcatnum" ]; then  # 如果没有选中CD
    echo "您必须先选择一个 CD"  # 提示用户先选择CD
    find_cd n  # 调用函数查找CD
  fi
  if [ -n "$cdcatnum" ]; then  # 如果选中了CD
    echo "当前曲目如下 :-"  # 显示当前曲目信息
    list_tracks  # 调用函数列出曲目
    echo  # 输出空行
    echo "这将重新输入 $cdtitle 的曲目"  # 提示用户将重新输入曲目信息
    get_confirm && {  # 如果用户确认更新
      grep -v "^${cdcatnum}," $tracks_file > $temp_file  # 从曲目文件中移除旧曲目信息
      mv $temp_file $tracks_file  # 用临时文件替换曲目文件
      echo  # 输出空行
      add_record_tracks  # 调用函数添加新曲目信息
    }
  fi
  return  # 函数返回
}

# count_cds 为我们快速统计数据库内容的数量.

count_cds() {
  set $(wc -l $title_file)  # 计算标题文件中的行数
  num_titles=$1  # 保存标题数量
  set $(wc -l $tracks_file)  # 计算曲目文件中的行数
  num_tracks=$1  # 保存曲目数量
  echo 找到 $num_titles 个 CD,共有 $num_tracks 个曲目  # 显示CD和曲目数量
  get_return  # 提示用户按回车键
  return  # 函数返回
}

# remove_records 从数据库文件中剥离条目,使用 grep -v 移除所有
# 匹配的字符串.注意我们必须使用临时文件.
# 如果我们尝试这样做,
# grep -v "^$cdcatnum" > $title_file
# $title_file 会在 grep 执行之前被 > 输出重定向设置为空,因此 grep 会从空文件读取.

remove_records() {
  if [ -z "$cdcatnum" ]; then  # 如果没有选中CD
    echo 您必须先选择一个 CD  # 提示用户先选择CD
    find_cd n  # 调用函数查找CD
  fi
  if [ -n "$cdcatnum" ]; then  # 如果选中了CD
    echo "您即将删除 $cdtitle"  # 提示用户将删除选中的CD
    get_confirm && {  # 如果用户确认删除
      grep -v "^${cdcatnum}," $title_file > $temp_file  # 从标题文件中移除选中CD的信息
      mv $temp_file $title_file  # 用临时文件替换标题文件
      grep -v "^${cdcatnum}," $tracks_file > $temp_file  # 从曲目文件中移除选中CD的信息
      mv $temp_file $tracks_file  # 用临时文件替换曲目文件
      cdcatnum=""  # 清除选中的CD信息
      echo 条目已移除  # 提示用户条目已移除
    }
    get_return  # 提示用户按回车键
  fi
  return  # 函数返回
}

# List_tracks 再次使用 grep 提取我们想要的行,cut 访问我们想要的字段
# 然后 more 提供分页输出.如果您考虑用多少行 C 代码重新实现这 20 多行代码,
# 您会体会到 shell 作为工具的威力.

list_tracks() {
  if [ "$cdcatnum" = "" ]; then  # 如果没有选中CD
    echo 没有选择 CD  # 提示用户没有选中CD
    return  # 函数返回
  else
    grep "^${cdcatnum}," $tracks_file > $temp_file  # 在曲目文件中搜索选中CD的曲目信息
    num_tracks=$(wc -l $temp_file)  # 计算找到的曲目数量
    if [ "$num_tracks" = "0" ]; then  # 如果没有找到曲目
      echo 没有找到 $cdtitle 的曲目  # 提示用户没有找到曲目
    else {
      echo
      echo "$cdtitle :-"  # 显示选中CD的标题
      echo
      cut -f 2- -d , $temp_file  # 提取并显示曲目信息
      echo
    } | ${PAGER:-more}  # 使用分页器显示曲目信息
    fi
  fi
  get_return  # 提示用户按回车键
  return  # 函数返回
}

# 现在所有的函数都已定义,我们可以进入主程序.
# 前几行只是让文件进入已知状态,然后我们调用菜单
# 函数 set_menu_choice,并根据输出执行操作.
# 当选择退出时,我们删除临时文件,写入消息并以成功完成条件退出

rm -f $temp_file  # 删除临时文件
if [ ! -f $title_file ]; then  # 如果标题文件不存在
  touch $title_file  # 创建标题文件
fi
if [ ! -f $tracks_file ]; then  # 如果曲目文件不存在
  touch $tracks_file  # 创建曲目文件
fi

# 现在应用程序本身

clear  # 清屏
echo
echo
echo "迷你 CD 管理器"  # 显示程序标题
sleep 1  # 等待1秒

quit=n  # 初始化退出标志为n
# 循环直到用户选择退出
while [ "$quit" != "y" ];
do
  # 设置菜单选项
  set_menu_choice
  # 根据用户选择的菜单项执行相应的功能
  case "$menu_choice" in
    # 添加记录
    a) add_records;;
    # 删除记录
    r) remove_records;;
    # 查找 CD
    f) find_cd y;;
    # 更新 CD 信息
    u) update_cd;;
    # 计算 CD 数量
    c) count_cds;;
    # 列出音轨
    l) list_tracks;;
    # 显示 CD 标题文件内容
    b)
      echo
      more $title_file
      echo
      get_return;;
    # 用户选择退出
    q | Q ) quit=y;;
    # 如果用户输入了无法识别的选项,则显示错误信息
    *) echo "对不起,选择无法识别";;
  esac
done

# 删除临时文件,清理工作目录
rm -f $temp_file
# 输出完成信息到终端
echo "Finished"
# 退出脚本并返回状态码0,表示成功执行
exit 0
