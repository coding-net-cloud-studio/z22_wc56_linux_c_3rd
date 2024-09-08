/* We now move on to the user interface. This gives us a (relatively) simple program with
    which to access our database functions, which we'll implement in a separate file.
    We start, as usual, with some header files.    */

/* 现在我们开始实现用户界面.这将使我们有一个(相对)简单的程序来访问我们的数据库函数,这些函数我们将会在一个单独的文件中实现.
我们首先,像往常一样,包含一些头文件.    */

#define _XOPEN_SOURCE  // 定义_XOPEN_SOURCE以使用POSIX标准功能

#include <stdlib.h>  // 包含标准库头文件
#include <unistd.h>  // 包含POSIX标准头文件
#include <stdio.h>   // 包含标准输入输出头文件
#include <string.h>  // 包含字符串处理头文件

#include "cd_data.h"  // 包含自定义的头文件,可能包含CD数据的定义和函数声明

#define TMP_STRING_LEN 125
/* this number must be larger than the biggest single string in any database structure */

/* We make our menu options typedefs. This is in preference to using #defined constants,
 as it allows the compiler to check the types of the menu option variables. */

/* 这个数字必须大于数据库结构中任何单个字符串的最大长度 */

/* 我们为菜单选项创建typedefs.这优先于使用#defined常量,
因为它允许编译器检查菜单选项变量的类型. */

// 定义一个枚举类型,表示菜单选项
typedef enum
{
    mo_invalid,          // 无效的菜单选项
    mo_add_cat,          // 添加分类
    mo_add_tracks,       // 添加曲目
    mo_del_cat,          // 删除分类
    mo_find_cat,         // 查找分类
    mo_list_cat_tracks,  // 列出分类下的曲目
    mo_del_tracks,       // 删除曲目
    mo_count_entries,    // 计算条目数量
    mo_exit              // 退出程序
} menu_options;

/* Now the prototypes for the local functions.
 The prototypes for accessing the database were included in cd_data.h. */

// 声明了一系列静态函数,这些函数用于实现应用程序的用户界面和功能.
// 这些函数包括命令模式处理,公告,菜单显示,确认获取,新类别条目输入,新曲目条目输入,删除类别条目,删除曲目条目,查找类别,列出曲目,计数所有条目,显示CDC,显示CDT以及去除字符串中的换行符.

static int          command_mode(int argc, char *argv[]);                       // 处理命令模式的函数
static void         announce(void);                                             // 发布公告的函数
static menu_options show_menu(const cdc_entry *current_cdc);                    // 显示菜单的函数
static int          get_confirm(const char *question);                          // 获取用户确认的函数
static int          enter_new_cat_entry(cdc_entry *entry_to_update);            // 输入新类别条目的函数
static void         enter_new_track_entries(const cdc_entry *entry_to_add_to);  // 输入新曲目条目的函数
static void         del_cat_entry(const cdc_entry *entry_to_delete);            // 删除类别条目的函数
static void         del_track_entries(const cdc_entry *entry_to_delete);        // 删除曲目条目的函数
static cdc_entry    find_cat(void);                                             // 查找类别的函数
static void         list_tracks(const cdc_entry *entry_to_use);                 // 列出曲目的函数
static void         count_all_entries(void);                                    // 计数所有条目的函数
static void         display_cdc(const cdc_entry *cdc_to_show);                  // 显示CDC的函数
static void         display_cdt(const cdt_entry *cdt_to_show);                  // 显示CDT的函数
static void         strip_return(char *string_to_strip);                        // 去除字符串中换行符的函数

/* Finally, we get to main. This starts by ensuring the current_cdc_entry that we use
 to keep track of the currently selected CD catalog entry is initialized.
 We also parse the command line, announce what program is being run and initialize the database. */

int main(int argc, char *argv[])
{
    // 定义当前菜单选项和当前cdc条目
    // 定义当前菜单选项的变量
    // 该变量用于存储用户在当前菜单中的选择
    menu_options current_option;

    // 定义当前CDC条目的变量
    // CDC条目可能代表某种数据通信设备或控制条目
    // 该变量用于存储当前操作或显示的CDC条目信息
    cdc_entry current_cdc_entry;

    // 定义命令执行结果
    int command_result;

    // 将current_cdc_entry结构体清零
    memset(&current_cdc_entry, '\0', sizeof(current_cdc_entry));

    // 如果命令行参数大于1,表示进入了命令模式
    if (argc > 1)
    {
        // 执行命令模式并获取结果
        command_result = command_mode(argc, argv);
        // 根据命令执行结果退出程序
        exit(command_result);
    }

    // 输出欢迎信息
    announce();

    // 初始化数据库,如果初始化失败
    if (!database_initialize(0))
    {
        // 输出错误信息
        fprintf(stderr, "Sorry, unable to initialize database\n");
        // 提示用户如何创建新数据库
        fprintf(stderr, "To create a new database use %s -i\n", argv[0]);
        // 退出程序,返回失败状态码
        exit(EXIT_FAILURE);
    }

    /*  We're now ready to process user input. We sit in a loop, asking for a menu choice
     and processing it, until the user selects the exit option.
     We pass the current_cdc_entry structure to the show_menu function.
     We do this to allow the menu choices to change if a catalog entry is currently selected.  */

    /* 我们现在可以处理用户输入了.我们坐在一个循环中,要求用户选择一个菜单选项并处理它,直到用户选择退出选项.
        我们将当前的cdc_entry结构传递给show_menu函数.
        我们这样做是为了允许菜单选项根据当前选定的目录条目进行更改. */

    // 主循环,持续显示菜单直到用户选择退出
    while (current_option != mo_exit)
    {
        // 显示菜单并获取用户选择
        current_option = show_menu(&current_cdc_entry);

        // 根据用户选择执行相应操作
        switch (current_option)
        {
        case mo_add_cat:
            // 输入新的分类条目
            if (enter_new_cat_entry(&current_cdc_entry))
            {
                // 尝试添加新条目
                if (!add_cdc_entry(current_cdc_entry))
                {
                    // 添加失败,输出错误信息并清空当前条目
                    fprintf(stderr, "Failed to add new entry\n");
                    memset(&current_cdc_entry, '\0', sizeof(current_cdc_entry));
                }
            }
            break;
        case mo_add_tracks:
            // 输入新的曲目条目
            enter_new_track_entries(&current_cdc_entry);
            break;
        case mo_del_cat:
            // 删除分类条目
            del_cat_entry(&current_cdc_entry);
            break;
        case mo_find_cat:
            // 查找分类条目
            current_cdc_entry = find_cat();
            break;
        case mo_list_cat_tracks:
            // 列出分类下的所有曲目
            list_tracks(&current_cdc_entry);
            break;
        case mo_del_tracks:
            // 删除曲目条目
            del_track_entries(&current_cdc_entry);
            break;
        case mo_count_entries:
            // 计算所有条目数量
            count_all_entries();
            break;
        case mo_exit:
            // 用户选择退出
            break;
        case mo_invalid:
            // 无效的选择
            break;
        default:
            // 默认情况,未处理的选项
            break;
        } /* switch */
    } /* while */

    /* When the main loop exits, we close the database and exit back to the environment. The welcoming sentence is printed by the announce function. */

    /* 接下来,我们定义一些全局变量,包括一个指向当前目录条目结构体的指针,
    一个用于存储目录条目名称的字符串,以及一个用于存储目录条目路径的字符串. */

    // 关闭数据库连接
    // 该函数用于关闭与数据库的连接,释放相关资源
    database_close();

    // 退出程序,返回成功状态码
    // exit() 函数用于终止当前进程,EXIT_SUCCESS 是一个宏定义,通常值为0,表示程序成功执行
    exit(EXIT_SUCCESS);

} /* main */

static void announce(void)
{
    /**
     * 打印欢迎信息,介绍该程序为演示CD目录数据库程序.
     */
    printf("\n\nWelcome to the demonstration CD catalog database \
             program\n");
}

/* Here we implement the show_menu function. This checks whether a current catalog entry
 is selected, using the first character of the catalog name. More options are available
 if a catalog entry is selected.
 Numbers are now used to select menu items rather than the initial letters that we used
 in the previous two examples. */

/* 在这里我们实现了show_menu函数.这个函数检查是否选择了当前的目录项,
通过目录名称的第一个字符来判断.如果选择了目录项,则有更多选项可用.
现在使用数字来选择菜单项,而不是像前两个例子中那样使用首字母. */

static menu_options show_menu(const cdc_entry *cdc_selected)
{
    char         tmp_str[TMP_STRING_LEN + 1];  // 用于存储用户输入的临时字符串
    menu_options option_chosen = mo_invalid;   // 初始化选择的菜单选项为无效

    // 循环直到用户选择一个有效的菜单选项
    while (option_chosen == mo_invalid)
    {
        // 如果当前选中的条目存在
        if (cdc_selected->catalog[0])
        {
            // 打印当前选中的CD信息
            // 打印当前选中的条目信息
            // 该函数用于输出当前选中条目的详细信息,包括目录,标题,类型和艺术家
            printf("\n\n当前条目: ");
            printf("%s, %s, %s, %s\n", cdc_selected->catalog, cdc_selected->title, cdc_selected->type, cdc_selected->artist);

            // 显示完整的菜单选项
            // 显示CD管理系统的菜单选项
            // 用户可以通过这些选项来执行添加,搜索,计数,重新输入曲目,删除和列出曲目等操作
            printf("\n");
            printf("1 - 添加新CD\n");                    // 添加新的CD到数据库
            printf("2 - 搜索CD\n");                      // 在数据库中搜索特定的CD
            printf("3 - 计算数据库中的CD和曲目数量\n");  // 统计数据库中CD和曲目的总数
            printf("4 - 重新输入当前CD的曲目\n");        // 允许用户为当前选中的CD重新输入曲目信息
            printf("5 - 删除当前CD及其所有曲目\n");      // 从数据库中删除当前选中的CD及其所有曲目
            printf("6 - 列出当前CD的曲目\n");            // 显示当前选中CD的所有曲目
            printf("q - 退出\n");                        // 退出程序
            printf("\n选项: ");
            // 读取用户输入的选项
            fgets(tmp_str, TMP_STRING_LEN, stdin);

            // 根据用户输入选择相应的菜单选项
            // 根据用户输入的第一个字符选择相应的操作函数
            switch (tmp_str[0])
            {
            case '1':  // 如果用户输入'1',选择添加类别的操作
                option_chosen = mo_add_cat;
                break;
            case '2':  // 如果用户输入'2',选择查找类别的操作
                option_chosen = mo_find_cat;
                break;
            case '3':  // 如果用户输入'3',选择统计条目的操作
                option_chosen = mo_count_entries;
                break;
            case '4':  // 如果用户输入'4',选择添加曲目的操作
                option_chosen = mo_add_tracks;
                break;
            case '5':  // 如果用户输入'5',选择删除类别的操作
                option_chosen = mo_del_cat;
                break;
            case '6':  // 如果用户输入'6',选择列出类别曲目的操作
                option_chosen = mo_list_cat_tracks;
                break;
            case 'q':  // 如果用户输入'q',选择退出程序的操作
                option_chosen = mo_exit;
                break;
            }
        }
        else
        {
            // 如果当前没有选中的条目,显示简化的菜单选项
            // 显示菜单选项,允许用户选择不同的操作
            // 1 - 添加新的CD
            // 2 - 搜索CD
            // 3 - 计算数据库中的CD和曲目数量
            // q - 退出程序
            printf("\n\n");
            printf("1 - add new CD\n");                                // 显示添加新CD的选项
            printf("2 - search for a CD\n");                           // 显示搜索CD的选项
            printf("3 - count the CDs and tracks in the database\n");  // 显示计算CD和曲目数量的选项
            printf("q - quit\n");                                      // 显示退出程序的选项
            printf("\nOption: ");                                      // 提示用户输入选项
            fgets(tmp_str, TMP_STRING_LEN, stdin);                     // 从标准输入读取用户的选择

            // 根据用户输入选择相应的菜单选项
            switch (tmp_str[0])
            {
            case '1':
                // 用户选择添加分类
                option_chosen = mo_add_cat;
                break;
            case '2':
                // 用户选择查找分类
                option_chosen = mo_find_cat;
                break;
            case '3':
                // 用户选择统计条目
                option_chosen = mo_count_entries;
                break;
            case 'q':
                // 用户选择退出
                option_chosen = mo_exit;
                break;
            }
        }
    } /* while */
    return (option_chosen);  // 返回用户选择的菜单选项
}

/* There are several places where we wish to ask the user if they are sure about what
 they requested.
 Rather than have several places in the code asking the question, we extract the code
 as a separate function, get_confirm. */

/* 在几个地方,我们希望询问用户是否确定他们所请求的内容.
与其在代码中多个地方提出这个问题,我们将其提取为一个单独的函数,get_confirm.*/

static int get_confirm(const char *question)
{
    // 定义一个字符串变量,用于存储用户输入
    char tmp_str[TMP_STRING_LEN + 1];

    /**
     * 输出问题,提示用户进行确认
     * @param question 要输出的问题字符串
     */
    printf("%s", question);

    // 从标准输入读取一行字符串,最多读取TMP_STRING_LEN个字符
    fgets(tmp_str, TMP_STRING_LEN, stdin);

    // 如果用户输入的字符串第一个字符是Y或者y,则表示确认,返回1
    if (tmp_str[0] == 'Y' || tmp_str[0] == 'y')
    {
        return (1);
    }

    // 否则表示不确认,返回0
    return (0);
}

/* The function, enter_new_cat_entry, allows the user to enter a new catalog entry.
We don't want to store the linefeed that fgets returns, so we strip it off.
We don't use the gets function since it has no way of checking for an overflow of the buffer. */

/* 该函数允许用户输入一个新的目录条目.
我们不想存储fgets返回的换行符,所以我们需要去掉它.
我们不使用gets函数,因为它没有检查缓冲区溢出的方法.*/

static int enter_new_cat_entry(cdc_entry *entry_to_update)
{
    // 定义一个新的cdc_entry结构体实例
    cdc_entry new_entry;
    // 定义一个临时字符串用于存储用户输入
    char tmp_str[TMP_STRING_LEN + 1];

    // 将新条目的内存清零
    memset(&new_entry, '\0', sizeof(new_entry));

    // 提示用户输入目录条目
    printf("Enter catalog entry: ");
    // 从标准输入读取一行文本
    (void)fgets(tmp_str, TMP_STRING_LEN, stdin);
    // 去除输入字符串末尾的换行符
    strip_return(tmp_str);
    // 将输入的字符串复制到新条目的catalog字段,最多复制CAT_CAT_LEN - 1个字符
    strncpy(new_entry.catalog, tmp_str, CAT_CAT_LEN - 1);

    // 提示用户输入标题
    printf("Enter title: ");
    // 读取标题
    // 从标准输入读取一行文本到tmp_str,最多读取TMP_STRING_LEN-1个字符
    (void)fgets(tmp_str, TMP_STRING_LEN, stdin);

    // 去除tmp_str末尾的换行符
    strip_return(tmp_str);

    // 将处理过的tmp_str复制到new_entry.title中,最多复制CAT_TITLE_LEN-1个字符,并确保字符串以空字符结尾
    strncpy(new_entry.title, tmp_str, CAT_TITLE_LEN - 1);

    // 提示用户输入类型
    printf("Enter type: ");
    // 读取类型
    // 从标准输入读取一行文本到tmp_str,最多读取TMP_STRING_LEN-1个字符
    (void)fgets(tmp_str, TMP_STRING_LEN, stdin);

    // 去除tmp_str末尾的换行符
    strip_return(tmp_str);

    // 将处理过的tmp_str复制到new_entry.type中,最多复制CAT_TYPE_LEN-1个字符,以防止溢出
    strncpy(new_entry.type, tmp_str, CAT_TYPE_LEN - 1);

    // 提示用户输入艺术家
    printf("Enter artist: ");
    // 读取艺术家
    // 从标准输入读取一行文本到tmp_str,最多读取TMP_STRING_LEN-1个字符
    (void)fgets(tmp_str, TMP_STRING_LEN, stdin);
    // 去除tmp_str末尾的换行符
    strip_return(tmp_str);
    // 将处理后的tmp_str复制到new_entry.artist中,最多复制CAT_ARTIST_LEN-1个字符,并确保字符串以空字符结尾
    strncpy(new_entry.artist, tmp_str, CAT_ARTIST_LEN - 1);

    // 显示新的目录条目
    // 打印新目录条目的信息
    // 使用display_cdc函数显示新条目的详细信息
    printf("\n新目录条目信息如下:\n");
    display_cdc(&new_entry);  // 需要提供display_cdc函数的文档注释

    // 确认用户是否要添加这个条目
    if (get_confirm("Add this entry ?"))
    {
        // 如果用户确认,将新条目复制到传入的entry_to_update指针指向的位置
        memcpy(entry_to_update, &new_entry, sizeof(new_entry));
        // 返回1表示添加成功
        return (1);
    }
    // 返回0表示用户取消添加
    return (0);
}

/* We now come to the function for entering the track information, enter_new_track_entries.
 This is slightly more complex than the catalog entry function, because we allow an existing
 track entry to be left alone. */

/*
现在我们来看进入曲目信息的函数,enter_new_track_entries.
这个函数比目录条目函数稍微复杂一些,因为我们允许现有的曲目条目保持不变.
*/
static void enter_new_track_entries(const cdc_entry *entry_to_add_to)
{
    // 定义新的和现有的音轨条目结构体
    cdt_entry new_track, existing_track;
    // 定义临时字符串用于存储用户输入
    char tmp_str[TMP_STRING_LEN + 1];
    // 初始化音轨编号
    int track_no = 1;

    // 如果目录为空,则直接返回
    if (entry_to_add_to->catalog[0] == '\0')
        return;
    // 打印更新音轨的信息
    printf("\nUpdating tracks for %s\n", entry_to_add_to->catalog);
    // 提示用户如何操作
    // 打印提示信息,指导用户如何操作以修改或删除音轨描述
    // 用户可以选择按下回车键保持现有描述不变,
    // 输入单个 'd' 删除当前及后续所有音轨,
    // 或者输入新的音轨描述
    printf("Press return to leave existing description unchanged,\n");
    printf(" a single d to delete this and remaining tracks,\n");
    printf(" or new track description\n");

    // 循环直到用户完成操作
    while (1)
    {
        // 检查当前音轨编号是否已存在音轨条目
        memset(&new_track, '\0', sizeof(new_track));
        existing_track = get_cdt_entry(entry_to_add_to->catalog, track_no);
        if (existing_track.catalog[0])
        {
            // 如果存在,则打印现有音轨信息
            printf("\tTrack %d: %s\n", track_no, existing_track.track_txt);
            printf("\tNew text: ");
        }
        else
        {
            // 如果不存在,则提示用户输入新的音轨描述
            printf("\tTrack %d description: ", track_no);
        }
        // 读取用户输入
        // 从标准输入读取一行文本到tmp_str中,TMP_STRING_LEN是预定义的常量,表示最大可读取的字符数
        // 如果读取成功,fgets会在字符串末尾添加一个'\0'作为字符串结束符
        // 如果读取失败或遇到文件结束标志,tmp_str将是一个空指针
        // 注意:fgets会保留输入中的换行符
        fgets(tmp_str, TMP_STRING_LEN, stdin);

        // 调用strip_return函数移除tmp_str末尾的换行符(如果存在)
        // 这样做是为了确保处理字符串时不会因为意外的换行符而出错
        // strip_return函数的实现未在此处展示,但可以假设它会检查并移除字符串末尾的'\n'
        strip_return(tmp_str);

        // 如果用户没有输入任何内容
        if (strlen(tmp_str) == 0)
        {
            if (existing_track.catalog[0] == '\0')
            {
                // 如果没有现有条目,则认为用户完成了添加
                break;
            }
            else
            {
                // 如果有现有条目,则跳到下一个音轨
                track_no++;
                continue;
            }
        }

        // 如果用户输入了单个'd'字符,则删除当前及更高编号的音轨
        // 检查tmp_str的长度是否为1且第一个字符是否为'd'
        if ((strlen(tmp_str) == 1) && tmp_str[0] == 'd')
        {
            // 如果条件满足,进入循环
            // 循环中调用del_cdt_entry函数删除entry_to_add_to->catalog中的条目
            // 每次删除后track_no增加1
            while (del_cdt_entry(entry_to_add_to->catalog, track_no))
            {
                track_no++;
            }
            // 循环结束后跳出当前代码块
            break;
        }

        // 添加新音轨或更新现有音轨
        strncpy(new_track.track_txt, tmp_str, TRACK_TTEXT_LEN - 1);
        strcpy(new_track.catalog, entry_to_add_to->catalog);
        new_track.track_no = track_no;
        if (!add_cdt_entry(new_track))
        {
            // 如果添加失败,则打印错误信息并退出循环
            fprintf(stderr, "Failed to add new track\n");
            break;
        }
        // 移动到下一个音轨编号
        track_no++;
    } /* while */
}

/* The function del_cat_entry deletes a catalog entry. We never allow tracks for a
 non-existent catalog entry to exist. */

/* 函数 del_cat_entry 用于删除目录条目.我们绝不允许为不存在的目录条目存在曲目.
 *
 * 参数:
 *   entry_id - 要删除的目录条目的ID.
 *
 * 返回值:
 *   成功时返回0,失败时返回错误代码.
 */

static void del_cat_entry(const cdc_entry *entry_to_delete)
{
    // 初始化音轨编号
    int track_no = 1;
    // 删除操作是否成功的标志
    int delete_ok;

    // 显示要删除的条目信息
    display_cdc(entry_to_delete);
    // 确认用户是否真的想要删除这个条目及其所有音轨
    if (get_confirm("Delete this entry and all it's tracks? "))
    {
        // 循环删除每个音轨,直到没有更多的音轨可以删除
        do
        {
            // 尝试删除指定音轨编号的音轨
            delete_ok = del_cdt_entry(entry_to_delete->catalog, track_no);
            // 音轨编号递增,准备删除下一个音轨
            track_no++;
        } while (delete_ok);  // 如果删除成功,继续循环

        // 尝试删除整个条目
        if (!del_cdc_entry(entry_to_delete->catalog))
        {
            // 如果删除条目失败,输出错误信息
            fprintf(stderr, "Failed to delete entry\n");
        }
    }
}

/* The next function is a utility for deleting all the tracks for a catalog. */
/* 下一个函数是用于删除目录中所有曲目的实用工具. */

static void del_track_entries(const cdc_entry *entry_to_delete)
{
    // 初始化音轨编号为1
    int track_no = 1;

    // 初始化删除确认标志
    int delete_ok;

    // 显示要删除的条目信息
    display_cdc(entry_to_delete);
    // 如果用户确认删除该条目的所有音轨
    if (get_confirm("Delete tracks for this entry? "))
    {
        // 循环删除音轨,直到没有更多的音轨可以删除
        do
        {
            // 尝试删除指定音轨编号的音轨
            delete_ok = del_cdt_entry(entry_to_delete->catalog, track_no);
            // 如果删除成功,音轨编号递增
            track_no++;
        } while (delete_ok);  // 当delete_ok为0时,表示没有更多音轨可以删除,退出循环
    }
}

/* Next, we create a very simple catalog search facility. We allow the user to enter a string,
 then check for catalog entries that contain the string. Since there could be multiple
 entries that match, we simply offer the user each match in turn. */

/* 接下来,我们创建一个非常简单的目录搜索功能.我们允许用户输入一个字符串,然后检查包含该字符串的目录条目.由于可能有多个匹配的条目,我们只需提供给用户轮流查看每个匹配项即可. */

static cdc_entry find_cat(void)
{
    // 定义用于存储查找结果的变量
    cdc_entry item_found;
    // 定义临时字符串用于存储用户输入
    char tmp_str[TMP_STRING_LEN + 1];
    // 标记是否是第一次调用搜索函数
    int first_call = 1;
    // 标记是否找到任何条目
    int any_entry_found = 0;
    // 标记输入的字符串是否有效
    int string_ok;
    // 标记是否选择了条目
    int entry_selected = 0;

    // 循环直到用户输入有效的字符串
    do
    {
        string_ok = 1;
        printf("请输入要在目录条目中搜索的字符串: ");
        fgets(tmp_str, TMP_STRING_LEN, stdin);
        strip_return(tmp_str);  // 去除输入字符串中的换行符
        // 检查字符串长度是否超过最大限制
        if (strlen(tmp_str) > CAT_CAT_LEN)
        {
            fprintf(stderr, "抱歉,字符串太长,最多 %d 个字符\n", CAT_CAT_LEN);
            string_ok = 0;  // 设置标记为无效
        }
    } while (!string_ok);  // 如果字符串无效,继续循环

    // 循环直到用户选择了一个条目
    while (!entry_selected)
    {
        // 调用搜索函数查找条目
        item_found = search_cdc_entry(tmp_str, &first_call);
        // 如果找到了条目
        if (item_found.catalog[0] != '\0')
        {
            any_entry_found = 1;  // 设置已找到条目标记
            printf("\n");
            display_cdc(&item_found);  // 显示找到的条目
            // 询问用户是否选择这个条目
            if (get_confirm("选择这个条目吗? "))
            {
                entry_selected = 1;  // 设置已选择条目标记
            }
        }
        else
        {
            // 如果没有更多匹配项且之前找到过条目
            if (any_entry_found)
                printf("抱歉,没有更多匹配项\n");
            else
                printf("抱歉,什么也没找到\n");
            break;  // 结束循环
        }
    }
    return item_found;  // 返回查找结果
}

/* list_tracks is a utility function that prints out all the tracks for a given catalog entry. */

static void list_tracks(const cdc_entry *entry_to_use)
{
    // 初始化曲目编号
    int track_no = 1;
    // 定义用于存储找到的曲目条目的变量
    cdt_entry entry_found;

    // 显示传入的cdc_entry信息
    display_cdc(entry_to_use);
    // 打印曲目标题
    printf("\nTracks\n");
    // 循环直到没有更多的曲目条目
    do
    {
        // 获取当前曲目编号对应的曲目条目
        entry_found = get_cdt_entry(entry_to_use->catalog, track_no);
        // 如果找到了曲目条目
        if (entry_found.catalog[0])
        {
            // 显示找到的曲目条目信息
            display_cdt(&entry_found);
            // 曲目编号递增,准备查找下一个曲目
            track_no++;
        }
    } while (entry_found.catalog[0]);  // 当找到曲目时继续循环
    // 等待用户按下回车键确认
    (void)get_confirm("Press return");
} /* list_tracks */

/* The count_all_entries function counts all the tracks.  */

/* count_all_entries 函数计算所有的曲目.*/

static void count_all_entries(void)
{
    // 初始化CD条目和曲目条目的计数器
    int cd_entries_found    = 0;
    int track_entries_found = 0;

    // 定义CDC和CDT条目结构体变量
    cdc_entry cdc_found;
    cdt_entry cdt_found;

    // 初始化曲目编号和首次搜索标志
    int track_no   = 1;
    int first_time = 1;

    // 初始化搜索字符串
    char *search_string = "";

    // 循环直到没有更多的CDC条目被发现
    do
    {
        // 搜索CDC条目
        cdc_found = search_cdc_entry(search_string, &first_time);
        // 如果找到了CDC条目
        if (cdc_found.catalog[0])
        {
            // 增加CD条目计数器
            cd_entries_found++;
            // 重置曲目编号
            track_no = 1;
            // 循环直到没有更多的CDT条目被发现
            do
            {
                // 获取CDT条目
                cdt_found = get_cdt_entry(cdc_found.catalog, track_no);
                // 如果找到了CDT条目
                if (cdt_found.catalog[0])
                {
                    // 增加曲目条目计数器
                    track_entries_found++;
                    // 增加曲目编号
                    track_no++;
                }
            } while (cdt_found.catalog[0]);  // 当找到CDT条目时继续循环
        }
    } while (cdc_found.catalog[0]);  // 当找到CDC条目时继续循环

    // 打印找到的CD和曲目总数
    printf("Found %d CDs, with a total of %d tracks\n", cd_entries_found, track_entries_found);

    // 等待用户确认
    (void)get_confirm("Press return");
}

/* Now we have display_cdc, a utility for displaying a catalog entry. */

/* 现在我们有了`display_cdc`,一个用于显示目录条目的实用程序. */

static void display_cdc(const cdc_entry *cdc_to_show)
{
    // 打印目录信息
    printf("Catalog: %s\n", cdc_to_show->catalog);
    // 打印标题信息
    printf("\ttitle: %s\n", cdc_to_show->title);
    // 打印类型信息
    printf("\ttype: %s\n", cdc_to_show->type);
    // 打印艺术家信息
    printf("\tartist: %s\n", cdc_to_show->artist);
}

/* and display_cdt, for displaying a single track entry. */

// display_cdt函数用于显示CDT条目的信息.
// 参数:
//   cdt_to_show: 指向要显示的CDT条目的指针.
static void display_cdt(const cdt_entry *cdt_to_show)
{
    // 打印CDT条目的轨道编号和轨道文本.
    printf("%d: %s\n", cdt_to_show->track_no, cdt_to_show->track_txt);
}

/* The utility function strip_return removes a trailing linefeed character from a string.
 Remember that UNIX uses a single linefeed to indicate end of line. */

/* utility函数strip_return用于去除字符串末尾的换行符.
记得UNIX使用单个换行符来表示行尾. */

static void strip_return(char *string_to_strip)
{
    // 获取字符串的长度
    int len = strlen(string_to_strip);

    // 如果字符串的最后一个字符是换行符,则将其替换为字符串结束符
    if (string_to_strip[len - 1] == '\n')
        string_to_strip[len - 1] = '\0';
}

/* command_mode is a function for parsing the command line arguments.
 The getopt function is a good way of ensuring your program accepts arguments conforming
 to the standard UNIX conventions. */

/* 现在我们有了`command_mode`,一个用于解析命令行参数的函数.`getopt`函数是确保您的程序接受符合标准UNIX约定的参数的好方法.*/

static int command_mode(int argc, char *argv[])
{
    int   c;                         // 命令行选项的字符
    int   result    = EXIT_SUCCESS;  // 程序执行结果,默认成功
    char *prog_name = argv[0];       // 程序名称

    /* 这些外部变量被getopt使用 */
    extern char *optarg;                  // 指向下一个选项参数的指针
    extern int   optind, opterr, optopt;  // 分别表示下一个要处理的参数索引,错误消息的开关,无效选项字符

    while ((c = getopt(argc, argv, ":i")) != -1)  // 解析命令行参数
    {
        switch (c)
        {
        case 'i':                         // 如果命令行参数包含'-i'
            if (!database_initialize(1))  // 尝试初始化数据库
            {
                result = EXIT_FAILURE;                               // 如果初始化失败,设置结果为失败
                fprintf(stderr, "Failed to initialize database\n");  // 输出错误信息
            }
            break;
        case ':':                                            // 如果缺少选项参数
        case '?':                                            // 如果遇到未知选项
        default:                                             // 默认情况
            fprintf(stderr, "Usage: %s [-i]\n", prog_name);  // 输出使用方法
            result = EXIT_FAILURE;                           // 设置结果为失败
            break;
        } /* switch */
    } /* while */
    return (result);  // 返回程序执行结果
}
