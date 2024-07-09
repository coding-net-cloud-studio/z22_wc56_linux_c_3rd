// 定义_XOPEN_SOURCE宏,以便使用X/Open规范中的某些特性
#define _XOPEN_SOURCE

// 包含标准库头文件
#include <stdlib.h>  // 提供内存管理函数,如malloc, free等
#include <unistd.h>  // 提供系统调用函数,如exit, read, write等
#include <stdio.h>   // 提供输入输出函数,如printf, scanf等
#include <string.h>  // 提供字符串处理函数,如strcpy, strcmp等

// 包含自定义头文件cd_data.h,该文件可能包含一些与应用程序界面相关的数据结构和函数声明
#include "cd_data.h"

// 定义一个常量TMP_STRING_LEN,其值为125
// 这个数值必须大于数据库结构中任何一个字符串的最大长度
#define TMP_STRING_LEN 125 /* this number must be larger than the biggest \
                              single string in any database structure */

/* 菜单选项枚举类型,定义了应用中可能出现的所有菜单选项 */
typedef enum
{
    mo_invalid,          // 无效选项,通常作为默认选项或错误处理时使用
    mo_add_cat,          // 添加分类,用于向应用中添加新的音乐分类
    mo_add_tracks,       // 添加音轨,用于向特定分类中添加音乐曲目
    mo_del_cat,          // 删除分类,用于移除应用中的某个音乐分类
    mo_find_cat,         // 查找分类,用于搜索并定位到特定的音乐分类
    mo_list_cat_tracks,  // 列出分类曲目,用于展示某个分类下的所有音乐曲目
    mo_del_tracks,       // 删除曲目,用于从特定分类中移除音乐曲目
    mo_count_entries,    // 计算条目数,用于统计应用中的音乐分类或曲目总数
    mo_exit              // 退出应用,结束程序运行
} menu_options;

/* local prototypes */
// 命令模式函数,用于处理用户输入的命令
static int command_mode(int argc, char *argv[]);
// 通知函数,用于向用户显示通知信息
static void announce(void);
// 显示菜单函数,用于展示当前可选的菜单项
static menu_options show_menu(const cdc_entry *current_cdc);
// 获取确认函数,用于在删除或修改前获取用户确认
static int get_confirm(const char *question);
// 进入新分类条目函数,用于添加新的分类信息
static int enter_new_cat_entry(cdc_entry *entry_to_update);
// 进入新曲目条目函数,用于添加新的曲目信息到指定分类
static void enter_new_track_entries(const cdc_entry *entry_to_add_to);
// 删除分类条目函数,用于移除指定的分类信息
static void del_cat_entry(const cdc_entry *entry_to_delete);
// 删除曲目条目函数,用于移除指定分类下的曲目信息
static void del_track_entries(const cdc_entry *entry_to_delete);
// 查找分类函数,用于检索分类信息
static cdc_entry find_cat(void);
// 列出曲目函数,用于展示指定分类下的所有曲目信息
static void list_tracks(const cdc_entry *entry_to_use);
// 统计所有条目函数,用于计算数据库中的总条目数
static void count_all_entries(void);
// 显示cdc函数,用于展示cdc类型的数据
static void display_cdc(const cdc_entry *cdc_to_show);
// 显示cdt函数,用于展示cdt类型的数据
static void display_cdt(const cdt_entry *cdt_to_show);
// 去除回车函数,用于移除字符串末尾的回车字符
static void strip_return(char *string_to_strip);

// 主函数,程序的入口点
// argc: 命令行参数的个数
// argv: 命令行参数的字符串数组
int main(int argc, char *argv[])
{
    // 定义菜单选项结构体变量,用于存储当前选择的菜单项
    menu_options current_option;

    // 定义cdc条目结构体变量,用于存储当前选中的cdc条目信息
    cdc_entry current_cdc_entry;

    // 定义整型变量,用于存储命令执行的结果
    int command_result;

    // 清空当前cdc条目,确保所有成员都被初始化为0
    memset(&current_cdc_entry, '\0', sizeof(current_cdc_entry));

    // 如果命令行参数多于1个,进入命令行模式处理
    if (argc > 1)
    {
        // 调用command_mode函数处理命令行参数,并获取处理结果
        command_result = command_mode(argc, argv);
        // 根据处理结果退出程序
        exit(command_result);
    }

    // 如果没有命令行参数,调用announce函数进行公告
    announce();

    // 尝试初始化数据库,参数0表示使用默认配置
    if (!database_initialize(0))
    {
        // 如果初始化失败,向标准错误输出错误信息
        fprintf(stderr, "抱歉,无法初始化数据库\n");
        // 提供创建新数据库的命令行指令
        fprintf(stderr, "要创建新数据库,请使用 %s -i\n", argv[0]);
        // 退出程序并返回失败状态码
        exit(EXIT_FAILURE);
    }

    // 当前选项不为退出时,继续循环
    while (current_option != mo_exit)
    {
        // 显示菜单并获取用户选择的当前选项
        current_option = show_menu(&current_cdc_entry);

        // 定义一个switch语句来处理不同的选项
        switch (current_option)
        {
        // 当选项为添加分类时
        case mo_add_cat:
            // 如果成功进入新的分类条目
            if (enter_new_cat_entry(&current_cdc_entry))
            {
                // 如果添加分类条目失败
                if (!add_cdc_entry(current_cdc_entry))
                {
                    // 输出错误信息到标准错误流
                    fprintf(stderr, "Failed to add new entry\n");
                    // 清空当前分类条目
                    memset(&current_cdc_entry, '\0', sizeof(current_cdc_entry));
                }
            }
            break;
        // 当选项为添加曲目时
        case mo_add_tracks:
            // 进入新的曲目条目
            enter_new_track_entries(&current_cdc_entry);
            break;
        // 当选项为删除分类时
        case mo_del_cat:
            // 删除分类条目
            del_cat_entry(&current_cdc_entry);
            break;
        // 这里有一个多余的break,应该删除
        // case mo_find_cat:
        //     current_cdc_entry = find_cat();
        //     break;
        // 当选项为查找分类时
        case mo_find_cat:
            // 查找分类并更新当前分类条目
            current_cdc_entry = find_cat();
            break;
        // 当选项为列出分类曲目时
        case mo_list_cat_tracks:
            // 列出曲目的详细信息
            list_tracks(&current_cdc_entry);
            break;
        // 当选项为删除曲目时
        case mo_del_tracks:
            // 删除曲目条目
            del_track_entries(&current_cdc_entry);
            break;
        // 当选项为计算条目总数时
        case mo_count_entries:
            // 计算所有条目的数量
            count_all_entries();
            break;
        // 当选项为退出时
        case mo_exit:
            break;
        // 当选项无效时
        case mo_invalid:
            break;
        // 处理其他未定义的选项
        default:
            break;
        } /* switch */
    } /* while */

    // 关闭数据库连接,确保所有数据操作完成且资源已释放
    database_close();

    // 程序执行成功,以成功状态退出程序
    exit(EXIT_SUCCESS);
} /* main */

// 定义一个名为announce的静态函数,用于在程序启动时输出欢迎信息
static void announce(void)
{
    // 使用printf函数打印欢迎信息到控制台
    printf("\n\n欢迎使用演示CD目录数据库程序\n");
} /* announce */

static menu_options show_menu(const cdc_entry *cdc_selected)
{
    // 定义一个临时字符串,长度为TMP_STRING_LEN + 1,用于存储临时数据
    char tmp_str[TMP_STRING_LEN + 1];

    // 定义一个枚举变量,用于记录用户选择的菜单选项
    // mo_invalid表示无效选项
    menu_options option_chosen = mo_invalid;

    while (option_chosen == mo_invalid)
    {
        if (cdc_selected->catalog[0])
        {
            // 打印当前选中的CD信息
            printf("\n\n当前条目: ");
            printf("%s, %s, %s, %s\n", cdc_selected->catalog, cdc_selected->title, cdc_selected->type, cdc_selected->artist);

            // 打印操作选项
            printf("\n");
            printf("1 - 添加新CD\n");
            printf("2 - 搜索CD\n");
            printf("3 - 统计数据库中的CD和曲目数量\n");
            printf("4 - 重新输入当前CD的曲目\n");
            printf("5 - 删除此CD及其所有曲目\n");
            printf("6 - 列出此CD的曲目\n");
            printf("q - 退出\n");
            printf("\n选项: ");
            // 从标准输入读取用户选择的操作
            fgets(tmp_str, TMP_STRING_LEN, stdin);
            // 根据输入的第一个字符选择相应的操作选项
            switch (tmp_str[0])
            {
            // 输入'1'选择添加分类操作
            case '1':
                option_chosen = mo_add_cat;
                break;
            // 输入'2'选择查找分类操作
            case '2':
                option_chosen = mo_find_cat;
                break;
            // 输入'3'选择统计条目数量操作
            case '3':
                option_chosen = mo_count_entries;
                break;
            // 输入'4'选择添加曲目操作
            case '4':
                option_chosen = mo_add_tracks;
                break;
            // 输入'5'选择删除分类操作
            case '5':
                option_chosen = mo_del_cat;
                break;
            // 输入'6'选择列出分类曲目操作
            case '6':
                option_chosen = mo_list_cat_tracks;
                break;
            // 输入'q'选择退出操作
            case 'q':
                option_chosen = mo_exit;
                break;
            }
        }
        else
        {
            // 打印空行以分隔输出内容
            printf("\n\n");
            // 打印选项菜单,提示用户进行选择
            printf("1 - 添加新CD\n");
            printf("2 - 搜索CD\n");
            printf("3 - 计算数据库中CD和曲目的数量\n");
            printf("q - 退出\n");
            printf("\n请选择操作: ");
            // 使用fgets从标准输入读取用户的选择,存储在tmp_str中
            fgets(tmp_str, TMP_STRING_LEN, stdin);
            // 根据输入的第一个字符来选择要执行的操作
            switch (tmp_str[0])
            {
            // 如果输入的是'1',则选择添加分类的选项
            case '1':
                option_chosen = mo_add_cat;
                break;
            // 如果输入的是'2',则选择查找分类的选项
            case '2':
                option_chosen = mo_find_cat;
                break;
            // 如果输入的是'3',则选择统计条目数量的选项
            case '3':
                option_chosen = mo_count_entries;
                break;
            // 如果输入的是'q',则选择退出程序的选项
            case 'q':
                option_chosen = mo_exit;
                break;
            }
        }
    } /* while */
    // 返回用户选择的选项
    return (option_chosen);
} /* show_menu */

// 定义一个函数get_confirm,用于获取用户对于给定问题的确认回答
static int get_confirm(const char *question)
{
    // 定义一个字符数组tmp_str,用于存储用户输入的回答,长度为TMP_STRING_LEN + 1
    char tmp_str[TMP_STRING_LEN + 1];

    // 打印问题到控制台
    printf("%s", question);
    // 从标准输入读取用户回答,最多读取TMP_STRING_LEN个字符,并存储到tmp_str数组中
    fgets(tmp_str, TMP_STRING_LEN, stdin);
    // 检查用户输入的第一个字符是否为'Y'或'y',如果是则返回1,表示用户回答确认
    if (tmp_str[0] == 'Y' || tmp_str[0] == 'y')
    {
        return (1);
    }
    // 如果用户输入的不是'Y'或'y',则返回0,表示用户回答否定
    return (0);
} /* get_confirm */

// 定义一个函数用于输入新的目录条目
static int enter_new_cat_entry(cdc_entry *entry_to_update)
{
    // 创建一个新的cdc_entry结构体变量
    cdc_entry new_entry;
    // 定义一个临时字符串用于存储用户输入
    char tmp_str[TMP_STRING_LEN + 1];

    // 将新条目初始化为空
    memset(&new_entry, '\0', sizeof(new_entry));

    // 提示用户输入目录
    printf("Enter catalog entry: ");
    // 从标准输入读取用户输入的目录,并存储到临时字符串中
    (void)fgets(tmp_str, TMP_STRING_LEN, stdin);
    // 去除字符串末尾的换行符
    strip_return(tmp_str);
    // 将用户输入的目录复制到新条目的catalog字段中
    strncpy(new_entry.catalog, tmp_str, CAT_CAT_LEN - 1);

    // 提示用户输入标题
    printf("Enter title: ");
    // 从标准输入读取用户输入的标题,并存储到临时字符串中
    (void)fgets(tmp_str, TMP_STRING_LEN, stdin);
    // 去除字符串末尾的换行符
    strip_return(tmp_str);
    // 将用户输入的标题复制到新条目的title字段中
    strncpy(new_entry.title, tmp_str, CAT_TITLE_LEN - 1);

    // 提示用户输入类型
    printf("Enter type: ");
    // 从标准输入读取用户输入的类型,并存储到临时字符串中
    (void)fgets(tmp_str, TMP_STRING_LEN, stdin);
    // 去除字符串末尾的换行符
    strip_return(tmp_str);
    // 将用户输入的类型复制到新条目的type字段中
    strncpy(new_entry.type, tmp_str, CAT_TYPE_LEN - 1);

    // 提示用户输入艺术家
    printf("Enter artist: ");
    // 从标准输入读取用户输入的艺术家,并存储到临时字符串中
    (void)fgets(tmp_str, TMP_STRING_LEN, stdin);
    // 去除字符串末尾的换行符
    strip_return(tmp_str);
    // 将用户输入的艺术家复制到新条目的artist字段中
    strncpy(new_entry.artist, tmp_str, CAT_ARTIST_LEN - 1);

    // 显示新输入的目录条目信息
    printf("\nNew catalog entry entry is :-\n");
    display_cdc(&new_entry);
    // 如果用户确认添加此条目
    if (get_confirm("Add this entry ?"))
    {
        // 将新条目复制到传入的entry_to_update指针指向的结构体中
        memcpy(entry_to_update, &new_entry, sizeof(new_entry));
        // 返回1表示成功添加
        return (1);
    }
    // 如果用户不确认,返回0表示未添加
    return (0);
} /* add_new_cdc_entry */

static void enter_new_track_entries(const cdc_entry *entry_to_add_to)
{
    // 定义一个新的cdt_entry类型的变量new_track,用于表示新的音轨
    cdt_entry new_track;
    // 定义一个现有的cdt_entry类型的变量existing_track,用于表示已存在的音轨
    cdt_entry existing_track;
    // 定义一个字符数组tmp_str,长度为TMP_STRING_LEN + 1,用于存储临时字符串
    char tmp_str[TMP_STRING_LEN + 1];
    // 定义一个整型变量track_no,初始化为1,用于表示音轨编号
    int track_no = 1;

    // 如果要添加的条目的目录字段为空,则直接返回不进行任何操作
    if (entry_to_add_to->catalog[0] == '\0')
        return;

    // 打印更新信息的提示,告知用户正在为哪个目录更新音轨
    printf("\nUpdating tracks for %s\n", entry_to_add_to->catalog);

    // 提示用户按下回车键保持现有描述不变
    printf("Press return to leave existing description unchanged,\n");

    // 提示用户输入单个'd'来删除当前及剩余的音轨
    printf(" a single d to delete this and remaining tracks,\n");

    // 提示用户可以输入新的音轨描述
    printf(" or new track description\n");

    while (1)
    {
        // 清空新曲目结构体,准备填充新数据
        memset(&new_track, '\0', sizeof(new_track));

        // 根据给定的目录和曲目号获取现有曲目信息
        existing_track = get_cdt_entry(entry_to_add_to->catalog, track_no);

        // 如果现有曲目的目录不为空,说明曲目已存在
        if (existing_track.catalog[0])
        {
            // 打印现有曲目的信息
            printf("\t曲目 %d: %s\n", track_no, existing_track.track_txt);
            // 提示用户输入新的曲目描述文本
            printf("\t新文本: ");
        }
        else
        {
            // 如果目录为空,提示用户输入曲目描述
            printf("\t曲目 %d 描述: ", track_no);
        }

        // 从标准输入读取用户输入的字符串
        fgets(tmp_str, TMP_STRING_LEN, stdin);
        // 去除字符串末尾的换行符
        strip_return(tmp_str);
        // 检查tmp_str的长度,如果为0,则表示没有新的输入或输入结束
        if (strlen(tmp_str) == 0)
        {
            // 如果现有曲目的目录为空('\0'),则表示没有现有的条目,添加操作完成
            if (existing_track.catalog[0] == '\0')
            {
                /* 没有现有条目,添加操作结束 */
                break;
            }
            else
            {
                /* 保留现有条目,跳转到下一曲目 */
                track_no++;
                continue;
            }
        }
        // 如果tmp_str的长度为1且字符为'd',则执行删除操作
        if ((strlen(tmp_str) == 1) && tmp_str[0] == 'd')
        {
            // 删除当前条目及其后续所有曲目
            while (del_cdt_entry(entry_to_add_to->catalog, track_no))
            {
                track_no++;  // 逐个增加曲目号以尝试删除下一个曲目
            }
            break;  // 删除完成后跳出循环
        }

        // 如果程序执行到这里,说明正在添加或更新曲目信息
        strncpy(new_track.track_txt, tmp_str, TRACK_TTEXT_LEN - 1);  // 将tmp_str的内容复制到new_track.track_txt,长度为TRACK_TTEXT_LEN - 1,防止溢出
        strcpy(new_track.catalog, entry_to_add_to->catalog);         // 将entry_to_add_to的catalog复制给new_track.catalog
        new_track.track_no = track_no;                               // 设置new_track的曲目编号为track_no
        if (!add_cdt_entry(new_track))                               // 尝试添加新的曲目条目到cdt
        {
            fprintf(stderr, "Failed to add new track\n");  // 如果添加失败,向标准错误输出错误信息
            break;                                         // 跳出当前循环或语句
        }
        track_no++;  // 如果添加成功,曲目编号加1
    } /* while */
} /* enter_new_track_entries */

// 定义删除目录项的函数,参数为要删除的目录项指针
static void del_cat_entry(const cdc_entry *entry_to_delete)
{
    // 初始化音轨号为1
    int track_no = 1;
    // 定义删除操作是否成功的标志变量
    int delete_ok;

    // 显示要删除的目录项
    display_cdc(entry_to_delete);
    // 询问用户是否确认删除该目录项及其所有音轨
    if (get_confirm("Delete this entry and all it's tracks? "))
    {
        // 循环删除所有音轨
        do
        {
            // 调用删除音轨函数,参数为目录号和音轨号
            delete_ok = del_cdt_entry(entry_to_delete->catalog, track_no);
            // 音轨号自增
            track_no++;
        } while (delete_ok);  // 当删除操作成功时继续循环

        // 删除目录项本身
        if (!del_cdc_entry(entry_to_delete->catalog))
        {
            // 如果删除失败,输出错误信息到标准错误流
            fprintf(stderr, "Failed to delete entry\n");
        }
    }
} /* del_cat_entry */

// 定义删除曲目条目的函数,参数为要删除的cdc条目
static void del_track_entries(const cdc_entry *entry_to_delete)
{
    // 初始化曲目号为1
    int track_no = 1;
    // 定义删除操作是否成功的标志变量
    int delete_ok;

    // 显示要删除的cdc条目信息
    display_cdc(entry_to_delete);
    // 询问用户是否确认删除此条目的所有曲目
    if (get_confirm("Delete tracks for this entry? "))
    {
        // 当删除操作成功时,继续执行循环
        do
        {
            // 尝试删除指定曲目的条目
            delete_ok = del_cdt_entry(entry_to_delete->catalog, track_no);
            // 曲目号自增,准备尝试删除下一首曲目
            track_no++;
        } while (delete_ok);
    }
} /* del_track_entries */

static cdc_entry find_cat(void)
{
    // 定义cdc_entry类型的变量item_found,用于存储找到的条目信息
    cdc_entry item_found;

    // 定义一个字符数组tmp_str,长度为TMP_STRING_LEN + 1,用于临时存储字符串
    char tmp_str[TMP_STRING_LEN + 1];

    // 定义整型变量first_call,初始值为1,表示是否是第一次调用
    int first_call = 1;

    // 定义整型变量any_entry_found,初始值为0,表示是否找到了任何条目
    int any_entry_found = 0;

    // 定义整型变量string_ok,用于标记字符串是否合法
    int string_ok;

    // 定义整型变量entry_selected,初始值为0,表示是否有条目被选中
    int entry_selected = 0;

    // 定义一个循环,用于获取用户输入的字符串,并检查其长度是否合适
    do
    {
        // 初始化字符串状态为有效
        string_ok = 1;

        // 提示用户输入要搜索的字符串
        printf("请输入要在目录条目中搜索的字符串: ");

        // 从标准输入读取一行字符串到临时缓冲区
        fgets(tmp_str, TMP_STRING_LEN, stdin);

        // 去除字符串末尾的换行符
        strip_return(tmp_str);

        // 检查字符串长度是否超过最大限制
        if (strlen(tmp_str) > CAT_CAT_LEN)
        {
            // 如果超过,输出错误信息
            fprintf(stderr, "抱歉,字符串太长了,最大允许 %d 个字符\n", CAT_CAT_LEN);

            // 设置字符串状态为无效
            string_ok = 0;
        }
    }  // 当字符串状态为无效时,继续循环
    while (!string_ok);

    // 当前代码段是一个循环,用于在用户界面中选择一个条目
    while (!entry_selected)  // 当没有选中任何条目时,继续循环
    {
        item_found = search_cdc_entry(tmp_str, &first_call);  // 调用函数搜索CDC条目
        if (item_found.catalog[0] != '\0')                    // 如果找到了条目(即catalog不为空)
        {
            any_entry_found = 1;              // 标记至少找到了一个条目
            printf("\n");                     // 打印换行符,可能是为了格式化输出
            display_cdc(&item_found);         // 显示找到的CDC条目信息
            if (get_confirm("This entry? "))  // 如果用户确认选择这个条目
            {
                entry_selected = 1;  // 设置标志位,表示已选中条目
            }
        }
        else  // 如果没有找到条目
        {
            if (any_entry_found)                           // 如果之前至少找到一个条目
                printf("Sorry, no more matches found\n");  // 提示用户没有更多的匹配项
            else
                printf("Sorry, nothing found\n");  // 如果之前一个条目都没找到,提示用户没有任何匹配项
            break;                                 // 退出循环
        }
    }
    return (item_found);  // 返回找到的条目信息
} /* find_cat */

// 定义一个函数list_tracks,用于列出CD上的所有曲目信息
// 参数为指向cdc_entry结构体的指针,该结构体包含CD的相关信息
static void list_tracks(const cdc_entry *entry_to_use)
{
    // 初始化曲目编号为1
    int track_no = 1;
    // 定义一个cdt_entry结构体变量,用于存储从目录中获取的曲目信息
    cdt_entry entry_found;

    // 显示传入的cdc_entry结构体对应的CD信息
    display_cdc(entry_to_use);
    // 打印"Tracks"标题
    printf("\nTracks\n");
    // 使用do-while循环来遍历所有曲目
    do
    {
        // 调用get_cdt_entry函数,根据曲目编号获取曲目信息
        entry_found = get_cdt_entry(entry_to_use->catalog, track_no);
        // 如果获取到的曲目信息中目录不为空,则表示还有曲目未列出
        if (entry_found.catalog[0])
        {
            // 显示当前曲目的信息
            display_cdt(&entry_found);
            // 曲目编号加1,继续获取下一曲目的信息
            track_no++;
        }
    } while (entry_found.catalog[0]);  // 当目录不为空时,继续循环
    // 等待用户按下回车键,然后继续
    (void)get_confirm("Press return");
} /* list_tracks */

// 定义一个函数count_all_entries,用于统计所有条目的数量
static void count_all_entries(void)
{
    // 定义变量cd_entries_found,用于记录找到的CD条目数量
    int cd_entries_found = 0;
    // 定义变量track_entries_found,用于记录找到的音轨条目数量
    int track_entries_found = 0;
    // 定义变量cdc_found,用于存储找到的CDC条目
    cdc_entry cdc_found;
    // 定义变量cdt_found,用于存储找到的CDT条目
    cdt_entry cdt_found;
    // 定义变量track_no,用于记录当前音轨编号
    int track_no = 1;
    // 定义变量first_time,用于标记是否是第一次执行
    int first_time = 1;
    // 定义变量search_string,用于存储搜索字符串
    char *search_string = "";

    // 定义一个循环,用于搜索CDC条目
    do
    {
        // 调用search_cdc_entry函数搜索CDC条目
        // 定义一个函数search_cdc_entry,用于搜索CDC条目
        // 参数search_string为要搜索的字符串
        // 参数first_time为一个指向布尔值的指针,用于指示是否是首次搜索
        // 函数返回值cdc_found表示是否找到了CDC条目
        cdc_found = search_cdc_entry(search_string, &first_time);
        // 如果找到了CDC条目
        if (cdc_found.catalog[0])
        {
            // 增加找到的CD条目计数
            cd_entries_found++;
            // 初始化音轨号为1
            track_no = 1;
            // 定义一个内部循环,用于获取CDT条目
            do
            {
                // 调用get_cdt_entry函数获取当前音轨的CDT条目
                // 获取与给定track_no对应的cdt条目
                // cdc_found.catalog: 存储目录信息的结构体变量
                // track_no: 要查找的曲目编号
                // cdt_found: 存储找到的cdt条目的变量
                cdt_found = get_cdt_entry(cdc_found.catalog, track_no);
                // 如果找到了CDT条目
                if (cdt_found.catalog[0])
                {
                    // 增加找到的音轨条目计数
                    track_entries_found++;
                    // 音轨号加1,继续搜索下一个音轨
                    track_no++;
                }
            } while (cdt_found.catalog[0]);  // 当CDT条目存在时,继续内部循环
        }
    } while (cdc_found.catalog[0]);  // 当CDC条目存在时,继续外部循环

    // 打印找到的CD数量和总曲目数
    printf("找到 %d 张CD,共 %d 首曲目\n", cd_entries_found, track_entries_found);

    // 等待用户按下回车键确认,函数get_confirm会阻塞直到用户输入
    (void)get_confirm("按回车键继续");

} /* count_all_entries */

// 定义一个函数,用于显示cdc条目的详细信息
static void display_cdc(const cdc_entry *cdc_to_show)
{
    // 打印cdc条目的目录信息
    printf("Catalogue: %s\n", cdc_to_show->catalog);
    // 打印cdc条目的标题信息
    printf("\ttitle: %s\n", cdc_to_show->title);
    // 打印cdc条目的类型信息
    printf("\ttype: %s\n", cdc_to_show->type);
    // 打印cdc条目的艺术家信息
    printf("\tartist: %s\n", cdc_to_show->artist);
} /* display_cdc */

// 定义一个函数,用于显示CD轨迹信息
static void display_cdt(const cdt_entry *cdt_to_show)
{
    // 打印CD轨迹的编号和文本信息
    printf("%d: %s\n", cdt_to_show->track_no, cdt_to_show->track_txt);
} /* display_cdt */

// 定义一个函数strip_return,用于去除字符串末尾的换行符
static void strip_return(char *string_to_strip)
{
    // 获取字符串的长度
    int len;
    len = strlen(string_to_strip);

    // 检查字符串最后一个字符是否为换行符
    if (string_to_strip[len - 1] == '\n')
    {
        // 如果是换行符,则将其替换为字符串结束符'\0'
        string_to_strip[len - 1] = '\0';
    }
} /* strip_return */

// 定义一个函数command_mode,接收命令行参数argc和argv
static int command_mode(int argc, char *argv[])
{
    int   c;                         // 用于存储getopt函数的返回值
    int   result    = EXIT_SUCCESS;  // 初始化结果为成功
    char *prog_name = argv[0];       // 获取程序名称

    // 注释说明extern的作用,这些变量用于getopt函数
    /* these externals used by getopt************************
extern char *optarg; // 用于存储当前选项的参数
extern optind, opterr, optopt; // 分别表示当前选项的位置,是否打印错误信息,未知选项的字符
***************************/

    // 注释中提到的Neil - getopt() in unistd only if __USE_POSIX2,是关于getopt函数在不同标准库中的实现问题
    // 如果定义了__USE_POSIX2,则unistd.h中会包含getopt函数的定义
    /*************  Neil - getopt() in unistd only if __USE_POSIX2
      any idea what this means or why? **************************/

    // 使用getopt函数解析命令行参数,参数格式为":i",其中":"表示选项可以带参数,但不打印错误信息
    while ((c = getopt(argc, argv, ":i")) != -1)
    {
        // 使用switch语句处理不同的选项
        switch (c)
        {
        case 'i':  // 如果选项为'i'
                   // 尝试初始化数据库,如果失败则设置结果为失败,并打印错误信息
            // 尝试初始化数据库,参数1可能表示使用某种配置或模式
            if (!database_initialize(1))
            {
                // 如果初始化失败,设置结果为EXIT_FAILURE,表示程序异常退出
                result = EXIT_FAILURE;
                // 向标准错误输出流打印错误信息,注意这里的字符串没有闭合的双引号,需要补充
                fprintf(stderr, "Failed to initialize database\n");
            }
            break;
        case ':':  // 如果选项缺少参数,打印使用说明并设置结果为失败
        case '?':  // 如果遇到未知选项,打印使用说明并设置结果为失败
        default:
            fprintf(stderr, "Usage: %s [-i]\n", prog_name);
            result = EXIT_FAILURE;
            break;
        } /* switch */
    } /* while */
    // 返回最终结果
    return (result);
} /* command_mode */
