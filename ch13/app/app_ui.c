/* We now move on to the user interface. This gives us a (relatively) simple program with
    which to access our database functions, which we'll implement in a separate file.
    We start, as usual, with some header files.    */

/* 我们现在转向用户界面.
这为我们提供了一个(相对)简单的程序,
通过它我们可以访问我们的数据库功能,
我们将在另一个文件中实现这些功能.
像往常一样,我们首先从一些头文件开始.*/

// 引入_XOPEN_SOURCE定义,以便使用兼容POSIX标准的一些特性
#define _XOPEN_SOURCE

// 引入标准库,包括内存管理,系统调用,输入输出和字符串操作相关函数
// 引入标准库<stdlib.h>,提供内存分配,随机数生成等功能
#include <stdlib.h>
// 引入标准库<unistd.h>,提供对POSIX操作系统API的访问,如文件I/O,进程控制等
#include <unistd.h>
// 引入标准输入输出库<stdio.h>,提供格式化输入输出功能
#include <stdio.h>
// 引入字符串处理库<string.h>,提供字符串操作函数
#include <string.h>

// 引入自定义的cd_data模块,该模块可能包含与CD数据相关的结构和函数
#include "cd_data.h"

// 定义一个常量TMP_STRING_LEN,用于限制某些字符串操作的长度,以避免缓冲区溢出
#define TMP_STRING_LEN 125

/* this number must be larger than the biggest single string in any database structure */

/* 这个数字必须大于任何数据库结构中最大的单个字符串的长度 */

/* We make our menu options typedefs. This is in preference to using #defined constants,
 as it allows the compiler to check the types of the menu option variables. */

/* 我们为我们的菜单选项定义了类型别名.这是优先于使用 #defined 常量的,因为它允许编译器检查菜单选项变量的类型. */

// 定义一个枚举类型,用于表示应用程序的菜单选项
typedef enum
{
    // 无效选项,用于错误或未初始化的情况
    mo_invalid,
    // 添加新分类
    mo_add_cat,
    // 添加音轨到某个分类
    mo_add_tracks,
    // 删除分类
    mo_del_cat,
    // 查找特定分类
    mo_find_cat,
    // 列出指定分类下的所有音轨
    mo_list_cat_tracks,
    // 删除音轨
    mo_del_tracks,
    // 计算所有条目的数量
    mo_count_entries,
    // 退出程序
    mo_exit
} menu_options;

/* Now the prototypes for the local functions.
 The prototypes for accessing the database were included in cd_data.h. */

/* 接下来是本地函数的原型.
访问数据库的原型已经包含在cd_data.h中了. */

// 定义命令模式函数,处理命令行参数
static int command_mode(int argc, char *argv[]);

// 定义公告函数,用于显示程序信息
static void announce(void);

// 定义显示菜单函数,根据当前CDC条目展示菜单选项
static menu_options show_menu(const cdc_entry *current_cdc);

// 定义获取确认函数,用于向用户提问并得到确认
static int get_confirm(const char *question);

// 定义输入新类别条目函数,更新传入的条目
static int enter_new_cat_entry(cdc_entry *entry_to_update);

// 定义输入新曲目条目函数,添加到指定的CDC条目中
static void enter_new_track_entries(const cdc_entry *entry_to_add_to);

// 定义删除类别条目函数,删除指定的CDC条目
static void del_cat_entry(const cdc_entry *entry_to_delete);

// 定义删除曲目条目函数,删除指定CDC条目中的曲目
static void del_track_entries(const cdc_entry *entry_to_delete);

// 定义查找类别函数,返回一个CDC条目
static cdc_entry find_cat(void);

// 定义列出曲目函数,使用指定的CDC条目来显示曲目列表
static void list_tracks(const cdc_entry *entry_to_use);

// 定义统计所有条目函数,计算数据库中的总条目数
static void count_all_entries(void);

// 定义显示CDC函数,展示指定的CDC条目内容
static void display_cdc(const cdc_entry *cdc_to_show);

// 定义显示CDT函数,展示指定的CDT条目内容
static void display_cdt(const cdt_entry *cdt_to_show);

// 定义去除回车函数,移除字符串末尾的回车符
static void strip_return(char *string_to_strip);

/* Finally, we get to main. This starts by ensuring the current_cdc_entry that we use
 to keep track of the currently selected CD catalog entry is initialized.
 We also parse the command line, announce what program is being run and initialize the database. */

/* 最后,我们进入主函数.
首先,确保我们用来跟踪当前选定的 CD 目录条目的 current_cdc_entry 被初始化.
我们还会解析命令行,宣布正在运行哪个程序,并初始化数据库. */

int main(int argc, char *argv[])
{
    menu_options current_option;     // 当前菜单选项
    cdc_entry    current_cdc_entry;  // 当前目录项
    int          command_result;     // 命令模式的结果

    // 初始化当前目录项结构体
    memset(&current_cdc_entry, '\0', sizeof(current_cdc_entry));

    // 如果提供了命令行参数,进入命令模式
    if (argc > 1)
    {
        command_result = command_mode(argc, argv);
        exit(command_result);
    }

    // 打印欢迎信息
    announce();

    // 初始化数据库,如果初始化失败则打印错误信息并退出
    if (!database_initialize(0))
    {
        fprintf(stderr, "抱歉,无法初始化数据库\n");
        fprintf(stderr, "要创建新数据库请使用 %s -i\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    /*  We're now ready to process user input. We sit in a loop, asking for a menu choice
     and processing it, until the user selects the exit option.
     We pass the current_cdc_entry structure to the show_menu function.
     We do this to allow the menu choices to change if a catalog entry is currently selected.  */

    /* 我们现在准备处理用户输入.我们处于一个循环中,要求用户选择一个菜单项并处理它,直到用户选择了退出选项.
        我们将当前的current_cdc_entry结构体传递给show_menu函数.
        我们这样做是为了允许菜单选项根据当前是否选择了某个目录条目而发生变化. */

    // 进入主循环,处理用户输入直到选择退出
    while (current_option != mo_exit)
    {
        // 显示菜单并根据当前选择的目录项更新选项
        current_option = show_menu(&current_cdc_entry);

        // 根据用户选择的菜单项执行相应操作
        switch (current_option)
        {
        case mo_add_cat:
            // 添加新的目录项
            if (enter_new_cat_entry(&current_cdc_entry))
            {
                // 如果添加成功,则将新条目写入数据库
                if (!add_cdc_entry(current_cdc_entry))
                {
                    fprintf(stderr, "添加新条目失败\n");
                    // 清空当前目录项结构体
                    memset(&current_cdc_entry, '\0', sizeof(current_cdc_entry));
                }
            }
            break;
        case mo_add_tracks:
            // 添加新的曲目到当前目录项
            enter_new_track_entries(&current_cdc_entry);
            break;
        case mo_del_cat:
            // 删除当前目录项
            del_cat_entry(&current_cdc_entry);
            break;
        case mo_find_cat:
            // 查找目录项并更新当前目录项
            current_cdc_entry = find_cat();
            break;
        case mo_list_cat_tracks:
            // 列出当前目录项的所有曲目
            list_tracks(&current_cdc_entry);
            break;
        case mo_del_tracks:
            // 删除当前目录项的所有曲目
            del_track_entries(&current_cdc_entry);
            break;
        case mo_count_entries:
            // 计算并显示所有条目的数量
            count_all_entries();
            break;
        case mo_exit:
            // 用户选择退出,跳出循环
            break;
        case mo_invalid:
            // 用户选择了无效的菜单项
            break;
        default:
            // 其他情况
            break;
        } /* switch */
    } /* while */

    /* When the main loop exits, we close the database and exit back to the environment. The welcoming sentence is printed by the announce function. */

    /* 当主循环退出时,我们关闭数据库并返回到环境中.欢迎语句是由announce函数打印的. */

    // 关闭数据库并退出程序
    database_close();
    exit(EXIT_SUCCESS);
} /* main */

// 静态函数announce用于显示欢迎信息
static void announce(void)
{
    // 打印欢迎信息到控制台
    printf("\n\n欢迎使用演示CD目录数据库程序\n");
}

/* Here we implement the show_menu function.

 This checks whether a current catalog entry is selected,

 using the first character of the catalog name.

 More options are available

 if a catalog entry is selected.

 Numbers are now used to select menu items rather than the initial letters that
 we used in the previous two examples. */

/* 在这里,我们实现 show_menu 函数.

该函数检查是否选择了当前目录条目,

通过目录名的第一个字符进行判断.

如果选择了目录条目,则提供更多选项.

现在我们使用数字来选择菜单项,而不是前两个示例中使用的首字母. */
// 函数show_menu用于显示菜单并根据用户输入选择相应的操作
static menu_options show_menu(const cdc_entry *cdc_selected)
{
    // 定义临时字符串变量用于存储用户输入
    char tmp_str[TMP_STRING_LEN + 1];
    // 初始化选项为无效,用于循环直到用户选择有效选项
    menu_options option_chosen = mo_invalid;

    // 当用户未选择有效选项时,继续显示菜单
    while (option_chosen == mo_invalid)
    {
        // 如果当前有选中的CD目录项
        if (cdc_selected->catalog[0])
        {
            // 打印当前选中项的信息
            printf("\n\n当前条目: ");
            printf("%s, %s, %s, %s\n", cdc_selected->catalog, cdc_selected->title, cdc_selected->type, cdc_selected->artist);

            // 打印菜单选项
            printf("\n");
            printf("1 - 添加新CD\n");
            printf("2 - 搜索CD\n");
            printf("3 - 计算数据库中CD和曲目的数量\n");
            printf("4 - 重新输入当前CD的曲目\n");
            printf("5 - 删除此CD及其所有曲目\n");
            printf("6 - 列出此CD的曲目\n");
            printf("q - 退出\n");
            printf("\n选项: ");
            // 读取用户输入
            fgets(tmp_str, TMP_STRING_LEN, stdin);
            // 根据用户输入的字符选择对应的菜单选项
            // 根据用户输入的字符选择相应的操作
            switch (tmp_str[0])
            {
            // 用户输入'1'时,选择添加分类的操作
            case '1':
                option_chosen = mo_add_cat;
                break;
            // 用户输入'2'时,选择查找分类的操作
            case '2':
                option_chosen = mo_find_cat;
                break;
            // 用户输入'3'时,选择统计条目的操作
            case '3':
                option_chosen = mo_count_entries;
                break;
            // 用户输入'4'时,选择添加曲目的操作
            case '4':
                option_chosen = mo_add_tracks;
                break;
            // 用户输入'5'时,选择删除分类的操作
            case '5':
                option_chosen = mo_del_cat;
                break;
            // 用户输入'6'时,选择列出分类下的曲目的操作
            case '6':
                option_chosen = mo_list_cat_tracks;
                break;
            // 用户输入'q'时,选择退出程序的操作
            case 'q':
                option_chosen = mo_exit;
                break;
            // 如果用户输入其他字符,没有对应的操作,这里可以添加默认处理或者错误处理
            default:
                // 例如:提示用户输入无效,重新输入
                printf("无效输入,请重新输入!\n");
                break;
            }
        }
        else
        {
            // 如果没有选中的CD目录项,则显示简化菜单
            printf("\n\n");
            printf("1 - 添加新CD\n");
            printf("2 - 搜索CD\n");
            printf("3 - 计算数据库中CD和曲目的数量\n");
            printf("q - 退出\n");
            printf("\n选项: ");
            // 读取用户输入
            fgets(tmp_str, TMP_STRING_LEN, stdin);
            // 根据用户输入的字符选择对应的菜单选项
            // 根据用户输入的字符选择相应的操作
            switch (tmp_str[0])
            {
            // 如果用户输入'1',则选择添加分类的操作
            case '1':
                option_chosen = mo_add_cat;
                break;
            // 如果用户输入'2',则选择查找分类的操作
            case '2':
                option_chosen = mo_find_cat;
                break;
            // 如果用户输入'3',则选择统计条目的操作
            case '3':
                option_chosen = mo_count_entries;
                break;
            // 如果用户输入'q',则选择退出程序的操作
            case 'q':
                option_chosen = mo_exit;
                break;
            // 如果没有匹配的选项,可以在这里添加默认处理
            default:
                // 例如:提示用户输入无效,或者直接退出
                break;
            }
        }
    } /* while */
    // 返回用户选择的菜单选项
    return (option_chosen);
}

/* There are several places where we wish to ask the user if they are sure about what  they requested.

 Rather than have several places in the code asking the question, we extract the code as a separate function, get_confirm. */

/* 有几个地方我们希望询问用户是否确定他们的请求.

与其在代码的多个地方提出这个问题,我们将这段代码提取为一个单独的函数,即 get_confirm. */

// 定义一个函数get_confirm,用于获取用户输入的确认信息
static int get_confirm(const char *question)
{
    // 定义一个临时字符串变量tmp_str,用于存储用户输入的字符
    char tmp_str[TMP_STRING_LEN + 1];

    // 打印问题,等待用户输入
    printf("%s", question);
    // 从标准输入读取用户输入的字符,最多读取TMP_STRING_LEN个字符,并存储在tmp_str中
    fgets(tmp_str, TMP_STRING_LEN, stdin);
    // 检查用户输入的第一个字符是否为'Y'或'y',如果是,则返回1表示确认,否则返回0表示否认
    if (tmp_str[0] == 'Y' || tmp_str[0] == 'y')
    {
        return (1);
    }
    return (0);
}

/* The function, enter_new_cat_entry, allows the user to enter a new catalog entry.
 We don't want to store the linefeed that fgets returns, so we strip it off.
 We don't use the gets function since it has no way of checking for an overflow of the buffer. */

/* enter_new_cat_entry 函数允许用户输入一个新的目录条目.
我们不想存储 fgets 返回的换行符,所以我们将其删除.
我们不使用 gets 函数,因为它无法检查缓冲区是否溢出. */
// 函数enter_new_cat_entry用于输入一个新的目录条目并更新给定的cdc_entry结构体
static int enter_new_cat_entry(cdc_entry *entry_to_update)
{
    cdc_entry new_entry;                    // 创建一个新的cdc_entry结构体用于存储输入的信息
    char      tmp_str[TMP_STRING_LEN + 1];  // 创建一个临时字符串数组用于读取用户输入

    memset(&new_entry, '\0', sizeof(new_entry));  // 将新条目初始化为空字符

    // 读取并存储目录信息
    printf("输入目录条目: ");
    (void)fgets(tmp_str, TMP_STRING_LEN, stdin);
    strip_return(tmp_str);                                 // 去除输入字符串末尾的换行符
    strncpy(new_entry.catalog, tmp_str, CAT_CAT_LEN - 1);  // 将输入的目录复制到新条目中,确保不超过最大长度

    // 读取并存储标题信息
    // 提示用户输入标题
    printf("输入标题: ");

    // 从标准输入读取一行文本到临时字符串变量tmp_str中
    (void)fgets(tmp_str, TMP_STRING_LEN, stdin);

    // 移除tmp_str末尾的换行符,以便后续处理
    strip_return(tmp_str);

    // 将处理后的字符串复制到新条目结构体的标题字段,确保不超过预定义长度
    strncpy(new_entry.title, tmp_str, CAT_TITLE_LEN - 1);

    // 读取并存储类型信息
    // 提示用户输入类型
    printf("输入类型: ");
    // 从标准输入读取一行文本到临时字符串变量tmp_str中
    (void)fgets(tmp_str, TMP_STRING_LEN, stdin);
    // 去除tmp_str末尾的换行符
    strip_return(tmp_str);
    // 将tmp_str的内容复制到新条目结构体的type字段,确保不会超出CAT_TYPE_LEN的长度
    strncpy(new_entry.type, tmp_str, CAT_TYPE_LEN - 1);

    // 读取并存储艺术家信息
    // 提示用户输入艺术家名称
    printf("输入艺术家: ");

    // 从标准输入读取一行文本,最多读取 TMP_STRING_LEN-1 个字符,并存储到 tmp_str 中
    (void)fgets(tmp_str, TMP_STRING_LEN, stdin);

    // 去除从 fgets 读取的字符串末尾的换行符
    strip_return(tmp_str);

    // 将处理后的字符串复制到新条目结构体的 artist 字段中,确保不会超出预定义的长度 CAT_ARTIST_LEN
    strncpy(new_entry.artist, tmp_str, CAT_ARTIST_LEN - 1);

    // 显示新输入的目录条目
    /* 打印新目录条目的信息 */
    printf("\n新目录条目是 :-");  // 输出提示信息,表明接下来将显示新目录条目的内容
    display_cdc(&new_entry);      /* 显示新目录条目的详细信息 */

    // 如果用户确认添加此条目,则将新条目的内容复制到传入的entry_to_update结构体中,并返回1表示成功
    // 询问用户是否确认添加新条目
    if (get_confirm("添加此条目吗?"))
    {
        // 如果用户确认,将新条目的数据复制到待更新的条目中
        memcpy(entry_to_update, &new_entry, sizeof(new_entry));
        // 返回1表示成功添加了新条目
        return (1);
    }
    // 如果用户不确认,则返回0表示未添加条目
    return (0);
}

/* We now come to the function for entering the track information, enter_new_track_entries.

 This is slightly more complex than the catalog entry function, because we allow an existing track entry to be left alone. */

/* 我们现在来到了输入曲目信息的函数,即 enter_new_track_entries.

这个函数比目录条目函数稍微复杂一些,因为我们允许现有的曲目条目保持不变. */

// 定义一个函数,用于输入新的曲目条目
static void enter_new_track_entries(const cdc_entry *entry_to_add_to)
{
    // 定义新的曲目条目和已存在的曲目条目结构体变量
    cdt_entry new_track, existing_track;
    // 定义一个临时字符串变量,用于存储用户输入
    char tmp_str[TMP_STRING_LEN + 1];
    // 定义一个整数变量,用于追踪当前曲目的编号
    int track_no = 1;

    // 如果目录字段为空,则直接返回,不进行任何操作
    if (entry_to_add_to->catalog[0] == '\0')
        return;
    // 打印更新曲目的提示信息
    printf("\n更新 %s 的曲目\n", entry_to_add_to->catalog);
    // 打印用户操作的提示信息
    printf("按回车键保留现有描述不变,\n");
    printf("输入单个 d 删除此曲目及剩余曲目,\n");
    printf("或输入新曲目描述\n");

    // 使用无限循环来处理曲目的输入和更新
    while (1)
    {
        /* First, we must check whether a track already exists with the current track number.
            Depending on what we find, we change the prompt. */

        /* 首先,我们必须检查是否已存在具有当前轨道编号的轨道.
            根据我们找到的结果,我们改变提示. */

        // 清空新曲目条目的内存
        memset(&new_track, '\0', sizeof(new_track));
        // 获取当前曲目编号的已存在曲目条目
        existing_track = get_cdt_entry(entry_to_add_to->catalog, track_no);
        // 如果已存在曲目条目,则打印现有曲目信息并提示用户输入新文本
        // 如果曲目的目录不为空,则打印曲目的信息以及提示输入新文本
        if (existing_track.catalog[0])
        {
            // 打印曲目编号和曲目文本
            printf("\t曲目 %d: %s\n", track_no, existing_track.track_txt);
            // 提示用户输入新的文本
            printf("\t新文本: ");
        }
        else
        {
            // 如果没有已存在的曲目条目,则提示用户输入曲目描述
            printf("\t曲目 %d 描述: ", track_no);
        }
        // 从标准输入读取用户输入的字符串
        fgets(tmp_str, TMP_STRING_LEN, stdin);
        // 去除字符串末尾的换行符
        strip_return(tmp_str);

        /* If there was no existing entry for this track and the user hasn't added one,
        we assume that there are no more tracks to be added. */

        /* 如果该轨道没有现有的条目且用户也没有添加,我们假设没有更多轨道需要添加. */

        // 如果用户没有输入任何内容,且没有已存在的曲目条目,则认为曲目添加完成
        // 如果临时字符串长度为0
        if (strlen(tmp_str) == 0)
        {
            // 检查现有曲目的目录是否为空
            if (existing_track.catalog[0] == '\0')
            {
                // 如果没有已存在的条目,则结束添加操作
                break;
            }
            else
            {
                // 如果存在现有条目,则保留现有条目并跳转到下一个曲目
                track_no++;
                continue;
            }
        }

        /* If the user enters a single d character, this deletes the current and any higher
         numbered tracks.
         The del_cdt_entry function will return false if it couldn't find a track to delete. */

        /* 如果用户输入单个 'd' 字符,这将删除当前及更高编号的所有轨道.
            如果 del_cdt_entry 函数找不到要删除的轨道,它将返回 false.*/

        // 如果用户输入的是单个字符'd',则删除当前曲目及更高编号的曲目
        // 如果tmp_str的长度为1且字符为'd',则执行以下操作
        if ((strlen(tmp_str) == 1) && tmp_str[0] == 'd')
        {
            // 循环删除当前曲目编号开始的后续所有曲目
            // del_cdt_entry函数用于从目录中删除指定曲目编号的条目
            // 当del_cdt_entry返回非零值时,表示还有曲目可删,继续循环
            while (del_cdt_entry(entry_to_add_to->catalog, track_no))
            {
                // 曲目编号递增,以便删除下一个曲目
                track_no++;
            }
            // 退出循环后,break语句确保跳出包含此循环的最外层循环
            break;
        }

        /* Here we get to the code for adding a new track, or updating an existing one.
         We construct the cdt_entry structure new_track, then call the database function
         add_cdt_entry to add it to the database. */

        /* 这里我们到达添加新轨道或更新现有轨道的代码部分.
            我们构造 cdt_entry 结构体 new_track,然后调用数据库函数 add_cdt_entry 将其添加到数据库中.*/

        // 构建新的曲目条目结构体,并调用数据库函数将其添加到数据库中
        // 将tmp_str中的文本复制到new_track.track_txt,确保不会超出TRACK_TTEXT_LEN的长度
        strncpy(new_track.track_txt, tmp_str, TRACK_TTEXT_LEN - 1);
        // 将entry_to_add_to的目录信息复制到new_track.catalog
        strcpy(new_track.catalog, entry_to_add_to->catalog);
        // 为new_track设置曲目标号
        new_track.track_no = track_no;
        // 尝试将new_track添加到CDT条目中,如果添加失败则执行以下操作
        if (!add_cdt_entry(new_track))
        {
            // 打印错误信息,指出添加新曲目失败
            fprintf(stderr, "添加新曲目失败\n");
            // 退出循环
            break;
        }
        // 曲目编号递增,准备处理下一个曲目
        track_no++;
    } /* while */
}

/* The function del_cat_entry deletes a catalog entry. We never allow tracks for a non-existent catalog entry to exist. */

/* 函数 del_cat_entry 用于删除目录条目.我们从不允许不存在的目录条目中存在曲目. */

// 定义删除目录条目的函数,参数为指向要删除的cdc_entry结构体的指针
static void del_cat_entry(const cdc_entry *entry_to_delete)
{
    // 初始化音轨编号为1
    int track_no = 1;
    // 初始化删除状态变量,用于判断是否成功删除音轨条目
    int delete_ok;

    // 显示要删除的目录条目信息
    display_cdc(entry_to_delete);
    // 获取用户确认,询问是否删除该条目及其所有音轨
    if (get_confirm("删除此条目及其所有音轨?"))
    {
        // 循环删除所有音轨条目,直到delete_ok为0(即删除失败或无更多音轨)
        do
        {
            // 尝试删除指定音轨编号的条目
            delete_ok = del_cdt_entry(entry_to_delete->catalog, track_no);
            // 如果删除成功,音轨编号加1,继续删除下一个音轨
            track_no++;
        } while (delete_ok);

        // 删除目录条目本身
        // 如果删除失败,向标准错误输出错误信息
        if (!del_cdc_entry(entry_to_delete->catalog))
        {
            fprintf(stderr, "删除条目失败\n");
        }
    }
}

/* The next function is a utility for deleting all the tracks for a catalog. */

/* 下一个函数是一个用于删除目录中所有曲目的实用程序. */

// 定义一个函数用于删除特定条目的音轨记录
static void del_track_entries(const cdc_entry *entry_to_delete)
{
    // 初始化音轨编号为1
    int track_no = 1;
    // 初始化删除状态,假设删除不成功
    int delete_ok;

    // 显示要删除的cdc条目信息
    display_cdc(entry_to_delete);
    // 如果用户确认删除该条目的音轨
    if (get_confirm("Delete tracks for this entry? "))
    {
        // 循环删除所有音轨,直到删除失败(可能是没有更多音轨或删除出错)
        do
        {
            // 尝试删除指定音轨编号的记录
            delete_ok = del_cdt_entry(entry_to_delete->catalog, track_no);
            // 如果删除成功,音轨编号加1,继续删除下一个音轨
            track_no++;
        } while (delete_ok);
    }
}

/* Next, we create a very simple catalog search facility.

  We allow the user to enter a string,
  then check for catalog entries that contain the string.

 Since there could be multiple
 entries that match, we simply offer the user each match in turn. */

/* 接下来,我们创建一个非常简单的目录搜索功能.

我们允许用户输入一个字符串,
然后检查包含该字符串的目录条目.

由于可能有多个匹配的条目,
我们只需按顺序向用户展示每个匹配的条目. */
// 定义一个函数 find_cat,用于在目录中查找特定的条目
static cdc_entry find_cat(void)
{
    // 定义一个 cdc_entry 结构体变量,用于存储找到的条目
    cdc_entry item_found;
    // 定义一个临时字符串变量,用于存储用户输入的搜索字符串
    char tmp_str[TMP_STRING_LEN + 1];
    // 定义一个标志位,表示是否为第一次调用 search_cdc_entry 函数
    int first_call = 1;
    // 定义一个标志位,表示是否找到了任何条目
    int any_entry_found = 0;
    // 定义一个标志位,表示用户输入的字符串是否合法
    int string_ok;
    // 定义一个标志位,表示用户是否已选择条目
    int entry_selected = 0;

    // 循环直到用户输入的字符串合法
    do
    {
        string_ok = 1;
        // 提示用户输入要搜索的字符串
        printf("请输入要在目录条目中搜索的字符串:");
        // 从标准输入读取一行字符串
        fgets(tmp_str, TMP_STRING_LEN, stdin);
        // 去除字符串末尾的换行符
        strip_return(tmp_str);
        // 检查字符串长度是否超过最大限制
        if (strlen(tmp_str) > CAT_CAT_LEN)
        {
            // 如果超过最大限制,输出错误信息并设置 string_ok 为 0
            fprintf(stderr, "抱歉,字符串太长,最大 %d 个字符\n", CAT_CAT_LEN);
            string_ok = 0;
        }
    } while (!string_ok);

    // 循环直到用户选择一个条目或没有更多匹配的条目
    while (!entry_selected)
    {
        // 调用 search_cdc_entry 函数搜索目录条目
        item_found = search_cdc_entry(tmp_str, &first_call);
        // 如果找到了条目
        if (item_found.catalog[0] != '\0')
        {
            // 设置找到条目的标志位为 1
            any_entry_found = 1;
            // 输出找到的条目信息
            printf("\n");
            display_cdc(&item_found);
            // 如果用户确认这个条目,设置 entry_selected 为 1
            if (get_confirm("这个条目?"))
            {
                entry_selected = 1;
            }
        }
        else
        {
            // 如果没有找到更多匹配的条目且之前已找到过条目,输出提示信息
            if (any_entry_found)
                printf("抱歉,没有找到更多匹配的条目\n");
            else
                printf("抱歉,没有找到任何内容\n");
            // 跳出循环
            break;
        }
    }
    // 返回找到的条目
    return item_found;
}

/* list_tracks is a utility function that prints out all the tracks for a given catalog entry. */

/* list_tracks 是一个实用函数,用于打印给定目录条目的所有曲目. */

// 定义一个函数list_tracks,用于列出给定cdc_entry的所有音轨信息
static void list_tracks(const cdc_entry *entry_to_use)
{
    // 初始化音轨编号为1
    int track_no = 1;
    // 定义一个变量entry_found,用于存储从cdt中检索到的音轨信息
    cdt_entry entry_found;

    // 显示传入的cdc_entry信息
    display_cdc(entry_to_use);
    // 打印音轨标题
    printf("\n曲目\n");
    // 使用do-while循环来遍历所有音轨
    do
    {
        // 从cdt中获取指定编号的音轨信息
        entry_found = get_cdt_entry(entry_to_use->catalog,
                                    track_no);
        // 如果检索到的音轨信息非空,则显示该音轨信息
        if (entry_found.catalog[0])
        {
            display_cdt(&entry_found);
            // 音轨编号递增
            track_no++;
        }
    } while (entry_found.catalog[0]);  // 当检索到的音轨信息非空时,继续循环
    // 等待用户按下回车键
    (void)get_confirm("按回车键");
} /* list_tracks */

/* The count_all_entries function counts all the tracks.  */

/* count_all_entries 函数计算所有曲目的数量. */

// 定义一个函数count_all_entries,用于统计CD和音轨的总数
static void count_all_entries(void)
{
    // 初始化CD条目计数器和音轨条目计数器
    int cd_entries_found    = 0;
    int track_entries_found = 0;

    // 定义两个结构体变量,用于存储从数据库中检索到的CD和音轨信息
    cdc_entry cdc_found;
    cdt_entry cdt_found;

    // 初始化音轨编号为1
    int track_no = 1;

    // 初始化一个标志位,用于指示是否是第一次搜索
    int first_time = 1;

    // 初始化搜索字符串为空
    char *search_string = "";

    // 使用do-while循环来搜索所有的CD条目
    do
    {
        // 调用search_cdc_entry函数搜索CD条目,并将结果存储在cdc_found中
        cdc_found = search_cdc_entry(search_string, &first_time);

        // 如果找到了CD条目
        if (cdc_found.catalog[0])
        {
            // CD条目计数器加1
            cd_entries_found++;

            // 重置音轨编号为1
            track_no = 1;

            // 使用嵌套的do-while循环来搜索当前CD上的所有音轨条目
            do
            {
                // 调用get_cdt_entry函数获取音轨条目,并将结果存储在cdt_found中
                cdt_found = get_cdt_entry(cdc_found.catalog, track_no);

                // 如果找到了音轨条目
                if (cdt_found.catalog[0])
                {
                    // 音轨条目计数器加1
                    track_entries_found++;

                    // 音轨编号加1,继续搜索下一个音轨
                    track_no++;
                }
            } while (cdt_found.catalog[0]);  // 当音轨条目存在时,继续循环
        }
    } while (cdc_found.catalog[0]);  // 当CD条目存在时,继续循环

    // 打印找到的CD总数和音轨总数
    printf("找到%d张CD,共有%d首曲目\n", cd_entries_found, track_entries_found);

    // 等待用户按下回车键确认
    (void)get_confirm("按回车键");
}

/* Now we have display_cdc, a utility for displaying a catalog entry. */

/* 现在我们有了一个名为 display_cdc 的实用程序,用于显示目录条目. */

// 定义一个函数,用于显示cdc条目的详细信息
static void display_cdc(const cdc_entry *cdc_to_show)
{
    // 打印cdc条目的目录信息
    printf("目录: %s\n", cdc_to_show->catalog);
    // 打印cdc条目的标题信息
    printf("\t标题: %s\n", cdc_to_show->title);
    // 打印cdc条目的类型信息
    printf("\t类型: %s\n", cdc_to_show->type);
    // 打印cdc条目的艺术家信息
    printf("\t艺术家: %s\n", cdc_to_show->artist);
}
/* and display_cdt, for displaying a single track entry. */

/* 还有 display_cdt,用于显示单一的曲目条目. */

// 这个函数用于显示CDT条目的信息
// 参数cdt_to_show是指向要显示的CDT条目的指针

static void display_cdt(const cdt_entry *cdt_to_show)
{
    // 打印CDT条目的轨道编号和文本信息
    printf("%d: %s\n", cdt_to_show->track_no, cdt_to_show->track_txt);
}

/* The utility function strip_return removes a trailing linefeed character from a string.
 Remember that UNIX uses a single linefeed to indicate end of line. */

/* 实用函数 strip_return 用于从字符串中删除尾部的换行符.
请记住,UNIX 使用单个换行符来表示行的结束. */

// 定义一个名为strip_return的静态函数,用于去除字符串末尾的换行符
static void strip_return(char *string_to_strip)
{
    // 获取字符串的长度
    int len;

    // 使用strlen函数计算字符串长度,并将结果赋值给变量len
    len = strlen(string_to_strip);

    // 检查字符串的最后一个字符是否为换行符
    if (string_to_strip[len - 1] == '\n')
    {
        // 如果是换行符,则将其替换为字符串结束符'\0'
        string_to_strip[len - 1] = '\0';
    }
}

/* command_mode is a function for parsing the command line arguments.
 The getopt function is a good way of ensuring your program accepts arguments conforming
 to the standard UNIX conventions. */

/* command_mode 是一个用于解析命令行参数的函数.
getopt 函数是一个很好的方式来确保你的程序接受符合标准 UNIX 约定的参数. */

// 定义一个函数command_mode,接收命令行参数argc和argv[]
static int command_mode(int argc, char *argv[])
{
    // 定义一个整型变量c用于存储getopt的返回值
    int c;
    // 定义一个整型变量result用于存储函数的返回结果,默认为EXIT_SUCCESS
    int result = EXIT_SUCCESS;
    // 定义一个字符指针prog_name指向当前程序的路径名
    char *prog_name = argv[0];

    // 声明外部变量,这些变量被getopt函数使用
    extern char *optarg;  // 用于存储当前选项的参数值
    extern int   optind;  // 用于存储下一个要处理的argv元素的下标
    extern int   opterr;  // 如果opterr非零,getopt会在遇到错误时打印错误信息
    extern int   optopt;  // 如果opterr非零,getopt会在遇到错误时打印错误信息

    // 使用while循环处理命令行参数
    while ((c = getopt(argc, argv, ":i")) != -1)
    {
        // 使用switch语句根据getopt的返回值c进行不同操作
        switch (c)
        {
        case 'i':  // 如果选项是'i'
            // 尝试初始化数据库,如果初始化失败则设置result为EXIT_FAILURE并打印错误信息
            if (!database_initialize(1))
            {
                result = EXIT_FAILURE;
                fprintf(stderr, "Failed to initialize database\n");
            }
            break;
        case ':':  // 如果选项缺少必要的参数
        case '?':  // 如果遇到了未知选项
        default:   // 其他错误情况
            // 打印程序的使用说明,并设置result为EXIT_FAILURE
            fprintf(stderr, "Usage: %s [-i]\n", prog_name);
            result = EXIT_FAILURE;
            break;
        } /* switch */
    } /* while */
    // 返回函数的执行结果
    return (result);
}
