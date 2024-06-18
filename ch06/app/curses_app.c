/*
   CD数据库应用程序

   <Linux编程入门>

   版本:终端

   版权 (c) 1996,2007 Wrox Press

   本程序是自由软件;您可以依据自由软件基金会发布的GNU通用公共许可证条款重新分发和/或修改本程序.

   本程序的发布希望对您有用,但不提供任何形式的保证;甚至不保证适销性或特定用途的适用性.有关更多细节,请参阅GNU通用公共许可证.

   如果未收到本程序的GNU通用公共许可证副本,请写信至Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

 */

/*
   注释

   此版本的CD数据库应用程序是根据"终端"章节中呈现的信息编写的.

   它是从"Shell编程"章节中呈现的shell脚本派生的.对于C语言实现,它没有重新设计,因此在这个版本中仍然可以看到许多shell原始的特性.

   此实现存在一些问题,将在后续修订中解决:

   它没有处理标题中的逗号.
   为了使它们显示在屏幕上,它对每张CD的曲目有实际限制.

   该程序有意使用标准输入和输出文件流.它并没有显式地处理重定向输入或输出.
*/

// 引入标准库和curses库,用于控制台应用程序的开发
#include <unistd.h>  // 提供对POSIX操作系统API的访问,如read/write等
#include <stdlib.h>  // 提供内存管理,随机数生成等功能
#include <stdio.h>   // 提供C语言标准输入输出功能
#include <string.h>  // 提供字符串处理函数
#include <curses.h>  // 提供控制台界面编程的功能

// 定义常量,用于限制字符串和数据库条目的最大长度
#define MAX_STRING   (80)    // 允许的最长响应长度
#define MAX_ENTRY    (1024)  // 允许的最长数据库条目长度

// 定义常量,用于指定消息,错误,问题和提示的行号
#define MESSAGE_LINE 6   // 杂项消息显示的行号
#define ERROR_LINE   22  // 错误信息显示的行号
#define Q_LINE       20  // 问题显示的行号
#define PROMPT_LINE  18  // 提示输入的行号

/*
   The variable current_cd is used to store the CD title we are
   working with. It is initialized so that the first character is NUL
   to indicate 'no CD selected'. The \0 is strictly unnecessary, but
   serves to emphasize the point.

   The variable current_cat will be used to record the catalog number
   of the current CD.
 */

/*
   变量 current_cd 用于存储我们正在处理的 CD 标题.
   它被初始化为第一个字符是 NUL,以表示'没有选择 CD'.\0 是严格不必要的,
   但有助于强调这一点.

   变量 current_cat 将用于记录当前 CD 的目录号.
 */

// 当前目录的字符串变量,用于存储用户当前所在的目录路径
static char current_cd[MAX_STRING] = "\0";

// 当前分类的字符串变量,用于存储用户当前选择的分类名称
static char current_cat[MAX_STRING];

/*
   File names.

   These files are fixed in this version to keep things simple.
   The temporary file name is also fixed, this would cause a
   problem if this program is run by two users in the same directory.

   A better way to obtain data base file names would be either by
   program arguments, or from environment variables.

   We need a better way to generate a unique a temporary file name.

   Many of these issues will be addresed in later versions.
 */

/*
   文件名.

   在这个版本中,这些文件名是固定的,以保持简单.
   临时文件名也是固定的,如果同一个目录下的两个用户运行此程序,这将导致问题.

   获取数据库文件名的更好方法是通过程序参数,或者从环境变量中获取.

   我们需要一种更好的方法来生成唯一的临时文件名.

   许多这些问题将在后续版本中得到解决.
 */

// 定义标题文件名,该文件用于存储数据库的标题信息
const char *title_file = "title.cdb";

// 定义音轨文件名,该文件用于存储数据库的音轨信息
const char *tracks_file = "tracks.cdb";

// 定义临时文件名,该文件在处理数据库操作时用作临时存储
const char *temp_file = "cdb.tmp";

/* Prototypes for local functions */
// 清除整个屏幕的函数原型声明
void clear_all_screen(void);
// 获取回车符的函数原型声明
void get_return(void);
// 获取用户确认的函数原型声明,返回值为整型
int get_confirm(void);
// 获取用户选择的函数原型声明,参数包括问候语和选项数组
int getchoice(char *greet, char *choices[]);
// 绘制菜单的函数原型声明,参数包括选项数组,高亮项索引,起始行号和起始列号
void draw_menu(char *options[], int highlight, int start_row, int start_col);
// 插入标题的函数原型声明,参数为标题字符串
void insert_title(char *cdtitle);
// 获取字符串输入的函数原型声明,参数为存储字符串的指针
void get_string(char *string);
// 添加记录的函数原型声明
void add_record(void);
// 计算CD数量的函数原型声明
void count_cds(void);
// 查找CD的函数原型声明
void find_cd(void);
// 列出音轨的函数原型声明
void list_tracks(void);
// 移除音轨的函数原型声明
void remove_tracks(void);
// 移除CD的函数原型声明
void remove_cd(void);
// 更新CD信息的函数原型声明
void update_cd(void);

/*
   Menu structures.
   The first character is the character to return when the
   chice is selected, the remaining text is to be displayed.
 */

/*
 * 菜单结构.
 * 第一个字符是当选择该选项时返回的字符,剩余的文本将显示.
 */

// 定义一个字符串数组,用于表示主菜单的选项
// 数组中的每个元素代表一个菜单项,最后一个元素为0表示菜单结束
char *main_menu[] =
    {
        // 添加新的CD
        "aadd new CD",
        // 查找CD
        "ffind CD",
        // 计算目录中CD和曲目的数量
        "ccount CDs and tracks in the catalog",
        // 退出程序
        "qquit",
        0,  // 菜单项结束标志
};

/*
   The extended menu is displayed when a CD is currently selected
 */

/*
 * 此注释说明当选择一个CD时,将显示扩展菜单.
 * 应用程序逻辑应该在选择CD后触发显示此菜单.
 * 可以在这里添加更多关于如何处理CD选择事件和显示扩展菜单的逻辑描述.
 */

// 定义一个字符串数组,用于表示扩展菜单的选项
// 定义一个扩展菜单的字符串数组,用于存储菜单项
char *extended_menu[] =
    {
        // 添加新CD
        "aadd new CD",
        // 查找CD
        "ffind CD",
        // 计算目录中的CD和曲目数量
        "ccount CDs and tracks in the catalog",
        // 列出当前CD上的曲目
        "llist tracks on current CD",
        // 移除当前CD
        "rremove current CD",
        // 更新曲目信息
        "uupdate track information",
        // 退出程序
        "qquit",
        // 数组结束标志
        0,
};

int main()
{
    int choice;

    initscr();

    do
    {

        choice = getchoice("Options:", current_cd[0] ? extended_menu : main_menu);

        switch (choice)
        {
        case 'q':
            break;

        case 'a':
            add_record();
            break;

        case 'c':
            count_cds();
            break;

        case 'f':
            find_cd();
            break;

        case 'l':
            list_tracks();
            break;

        case 'r':
            remove_cd();
            break;

        case 'u':
            update_cd();
            break;
        }
    } while (choice != 'q');

    endwin();
    exit(EXIT_SUCCESS);

} /* main */

/*
   getchoice - ask the user to choose
   passed: greet, an introduction
   choices, an array of strings, NULL at end
 */
int getchoice(char *greet, char *choices[])
{
    static int selected_row    = 0;
    int        max_row         = 0;
    int        start_screenrow = MESSAGE_LINE, start_screencol = 10;
    char     **option;
    int        selected;
    int        key = 0;

    option = choices;
    while (*option)
    {
        max_row++;
        option++;
    }

    /* protect against menu getting shorted when CD deleted */
    if (selected_row >= max_row)
        selected_row = 0;

    clear_all_screen();
    mvprintw(start_screenrow - 2, start_screencol, greet);

    keypad(stdscr, TRUE);
    cbreak();
    noecho();

    key = 0;
    while (key != 'q' && key != KEY_ENTER && key != '\n')
    {
        if (key == KEY_UP)
        {
            if (selected_row == 0)
                selected_row = max_row - 1;
            else
                selected_row--;
        }
        if (key == KEY_DOWN)
        {
            if (selected_row == (max_row - 1))
                selected_row = 0;
            else
                selected_row++;
        }
        selected = *choices[selected_row];
        draw_menu(choices, selected_row, start_screenrow, start_screencol);
        key = getch();
    }

    keypad(stdscr, FALSE);
    nocbreak();
    echo();

    if (key == 'q')
        selected = 'q';

    return (selected);
}

void draw_menu(char *options[], int current_highlight, int start_row, int start_col)
{
    int    current_row = 0;
    char **option_ptr;
    char  *txt_ptr;

    option_ptr = options;
    while (*option_ptr)
    {
        if (current_row == current_highlight)
            attron(A_STANDOUT);
        txt_ptr = options[current_row];
        txt_ptr++;
        mvprintw(start_row + current_row, start_col, "%s", txt_ptr);
        if (current_row == current_highlight)
            attroff(A_STANDOUT);
        current_row++;
        option_ptr++;
    }

    mvprintw(start_row + current_row + 3, start_col, "Move highlight then press Return ");

    refresh();
}

/*
   clear_all_screen

   Clear the screen and re-write the title.
   If a CD is selected then display the information.
 */
void clear_all_screen()
{
    clear();
    mvprintw(2, Q_LINE, "%s", "CD Database Application");
    if (current_cd[0])
    {
        mvprintw(ERROR_LINE, 0, "Current CD: %s: %s\n", current_cat, current_cd);
    }
    refresh();
}

/*
   get_return

   Prompt for and read a carriage return.
   Ignore other characters.
 */
void get_return()
{
    int ch;
    mvprintw(23, 0, "%s", " Press return ");
    refresh();
    while ((ch = getchar()) != '\n' && ch != EOF)
        ;
}

/*
   get_confirm

   Prompt for and read confirmation.
   Read a string and check first character for Y or y.
   On error or other character return no confirmation.
 */
int get_confirm()
{
    int  confirmed  = 0;
    char first_char = 'N';

    mvprintw(Q_LINE, 5, "Are you sure? ");
    clrtoeol();
    refresh();

    cbreak();
    first_char = getch();
    if (first_char == 'Y' || first_char == 'y')
    {
        confirmed = 1;
    }
    nocbreak();

    if (!confirmed)
    {
        mvprintw(Q_LINE, 1, "    Cancelled");
        clrtoeol();
        refresh();
        sleep(1);
    }
    return confirmed;
}

/*
   Database File Manipulation Functions
 */

/*
   insert_title

   Add a title to the CD database
   Simply add the title string to the end of the titles file

 */
void insert_title(char *cdtitle)
{
    FILE *fp = fopen(title_file, "a");
    if (!fp)
    {
        mvprintw(ERROR_LINE, 0, "cannot open CD titles database");
    }
    else
    {
        fprintf(fp, "%s\n", cdtitle);
        fclose(fp);
    }
}

/*
   get_string

   At the current screen position prompt for and read a string
   Delete any trailing newline.
 */
void get_string(char *string)
{
    int len;

    wgetnstr(stdscr, string, MAX_STRING);
    len = strlen(string);
    if (len > 0 && string[len - 1] == '\n')
        string[len - 1] = '\0';
}

/*
   add_record

   Add a new CD to the collection
 */

void add_record()
{
    char catalog_number[MAX_STRING];
    char cd_title[MAX_STRING];
    char cd_type[MAX_STRING];
    char cd_artist[MAX_STRING];
    char cd_entry[MAX_STRING];

    int screenrow = MESSAGE_LINE;
    int screencol = 10;

    clear_all_screen();
    mvprintw(screenrow, screencol, "Enter new CD details");
    screenrow += 2;

    mvprintw(screenrow, screencol, "Catalog Number: ");
    get_string(catalog_number);
    screenrow++;

    mvprintw(screenrow, screencol, "      CD Title: ");
    get_string(cd_title);
    screenrow++;

    mvprintw(screenrow, screencol, "       CD Type: ");
    get_string(cd_type);
    screenrow++;

    mvprintw(screenrow, screencol, "        Artist: ");
    get_string(cd_artist);
    screenrow++;

    mvprintw(15, 5, "About to add this new entry:");
    sprintf(cd_entry, "%s,%s,%s,%s", catalog_number, cd_title, cd_type, cd_artist);
    mvprintw(17, 5, "%s", cd_entry);
    refresh();

    move(PROMPT_LINE, 0);
    if (get_confirm())
    {
        insert_title(cd_entry);
        strcpy(current_cd, cd_title);
        strcpy(current_cat, catalog_number);
    }
}

/*
   count_cds - scan the database and count titles and tracks
 */
void count_cds()
{
    FILE *titles_fp, *tracks_fp;
    char  entry[MAX_ENTRY];
    int   titles = 0;
    int   tracks = 0;

    titles_fp = fopen(title_file, "r");
    if (titles_fp)
    {
        while (fgets(entry, MAX_ENTRY, titles_fp))
            titles++;
        fclose(titles_fp);
    }
    tracks_fp = fopen(tracks_file, "r");
    if (tracks_fp)
    {
        while (fgets(entry, MAX_ENTRY, tracks_fp))
            tracks++;
        fclose(tracks_fp);
    }
    mvprintw(ERROR_LINE, 0, "Database contains %d titles, with a total of %d tracks.", titles, tracks);
    get_return();
}

/*
   find_cd - locate a CD in the database

   prompt for a substring to match in the database
   set current_cd to the CD title

 */
void find_cd()
{
    char  match[MAX_STRING], entry[MAX_ENTRY];
    FILE *titles_fp;
    int   count = 0;
    char *found, *title, *catalog;

    mvprintw(Q_LINE, 0, "Enter a string to search for in CD titles: ");
    get_string(match);

    titles_fp = fopen(title_file, "r");
    if (titles_fp)
    {
        while (fgets(entry, MAX_ENTRY, titles_fp))
        {

            /* Skip past catalog number */
            catalog = entry;
            if (found = strstr(catalog, ","))
            {
                *found = 0;
                title  = found + 1;

                /* Zap the next comma in the entry to reduce it to title only */
                if (found = strstr(title, ","))
                {
                    *found = '\0';

                    /* Now see if the match substring is present */
                    if (found = strstr(title, match))
                    {
                        count++;
                        strcpy(current_cd, title);
                        strcpy(current_cat, catalog);
                    }
                }
            }
        }
        fclose(titles_fp);
    }
    if (count != 1)
    {
        if (count == 0)
            mvprintw(ERROR_LINE, 0, "Sorry, no matching CD found. ");
        if (count > 1)
            mvprintw(ERROR_LINE, 0, "Sorry, match is ambiguous: %d CDs found. ", count);
        current_cd[0] = '\0';
        get_return();
    }
}

/*
   remove_tracks - delete tracks from the current CD
 */
void remove_tracks()
{
    FILE *tracks_fp, *temp_fp;
    char  entry[MAX_ENTRY + 1];
    int   cat_length;

    if (current_cd[0] == '\0')
        return;

    cat_length = strlen(current_cat);

    tracks_fp = fopen(tracks_file, "r");
    if (tracks_fp == (FILE *)NULL)
        return;
    temp_fp = fopen(temp_file, "w");

    while (fgets(entry, MAX_ENTRY, tracks_fp))
    {
        /* Compare catalog number and copy entry if no match */
        if (strncmp(current_cat, entry, cat_length) != 0)
            fputs(entry, temp_fp);
    }
    fclose(tracks_fp);
    fclose(temp_fp);

    unlink(tracks_file);
    rename(temp_file, tracks_file);
}

/*
   remove_cd - delete the current CD from the database
 */
void remove_cd()
{
    FILE *titles_fp, *temp_fp;
    char  entry[MAX_ENTRY];
    int   cat_length;

    if (current_cd[0] == '\0')
        return;

    clear_all_screen();
    mvprintw(PROMPT_LINE, 0, "About to remove CD %s: %s. ", current_cat, current_cd);
    if (!get_confirm())
        return;

    cat_length = strlen(current_cat);

    /* Copy the titles file to a temporary, ignoring this CD */
    titles_fp = fopen(title_file, "r");
    temp_fp   = fopen(temp_file, "w");

    while (fgets(entry, MAX_ENTRY, titles_fp))
    {
        /* Compare catalog number and copy entry if no match */
        if (strncmp(current_cat, entry, cat_length) != 0)
            fputs(entry, temp_fp);
    }
    fclose(titles_fp);
    fclose(temp_fp);

    /* Delete the titles file, and rename the temporary file */
    unlink(title_file);
    rename(temp_file, title_file);

    /* Now do the same for the tracks file */
    remove_tracks();

    /* Reset current CD to 'None' */
    current_cd[0] = '\0';
}

/*
   Some defines we use only for showing or entering the track information
 */
#define BOXED_LINES  11
#define BOXED_ROWS   60
#define BOX_LINE_POS 8
#define BOX_ROW_POS  2
/*
   list_tracks - list the tracks for the current CD
 */
void list_tracks()
{
    FILE   *tracks_fp;
    char    entry[MAX_ENTRY];
    int     cat_length;
    int     lines_op = 0;
    WINDOW *track_pad_ptr;
    int     tracks = 0;
    int     key;
    int     first_line = 0;

    if (current_cd[0] == '\0')
    {
        mvprintw(ERROR_LINE, 0, "You must select a CD first. ", stdout);
        get_return();
        return;
    }
    clear_all_screen();
    cat_length = strlen(current_cat);

    /* First count the number of tracks for the current CD */
    tracks_fp = fopen(tracks_file, "r");
    if (!tracks_fp)
        return;
    while (fgets(entry, MAX_ENTRY, tracks_fp))
    {
        if (strncmp(current_cat, entry, cat_length) == 0)
            tracks++;
    }
    fclose(tracks_fp);

    /* Make a new pad, ensure that even if there is only a single
       track the PAD is large enough so the later prefresh() is always
       valid.
     */
    track_pad_ptr = newpad(tracks + 1 + BOXED_LINES, BOXED_ROWS + 1);
    if (!track_pad_ptr)
        return;

    tracks_fp = fopen(tracks_file, "r");
    if (!tracks_fp)
        return;

    mvprintw(4, 0, "CD Track Listing\n");

    /* write the track information into the pad */
    while (fgets(entry, MAX_ENTRY, tracks_fp))
    {
        /* Compare catalog number and output rest of entry */
        if (strncmp(current_cat, entry, cat_length) == 0)
        {
            mvwprintw(track_pad_ptr, lines_op++, 0, "%s", entry + cat_length + 1);
        }
    }
    fclose(tracks_fp);

    if (lines_op > BOXED_LINES)
    {
        mvprintw(MESSAGE_LINE, 0, "Cursor keys to scroll, RETURN or q to exit");
    }
    else
    {
        mvprintw(MESSAGE_LINE, 0, "RETURN or q to exit");
    }
    wrefresh(stdscr);
    keypad(stdscr, TRUE);
    cbreak();
    noecho();

    key = 0;
    while (key != 'q' && key != KEY_ENTER && key != '\n')
    {
        if (key == KEY_UP)
        {
            if (first_line > 0)
                first_line--;
        }
        if (key == KEY_DOWN)
        {
            if (first_line + BOXED_LINES + 1 < tracks)
                first_line++;
        }
        /* now draw the appropriate part of the pad on the screen */
        prefresh(track_pad_ptr, first_line, 0, BOX_LINE_POS, BOX_ROW_POS, BOX_LINE_POS + BOXED_LINES, BOX_ROW_POS + BOXED_ROWS);
        /*	wrefresh(stdscr); */
        key = getch();
    }

    delwin(track_pad_ptr);
    keypad(stdscr, FALSE);
    nocbreak();
    echo();
}

/*
   update_cd - re-enter tracks for current CD

   deletes all tracks for the current CD in the database
   and then prompts for new ones.
 */
void update_cd()
{
    FILE   *tracks_fp;
    char    track_name[MAX_STRING];
    int     len;
    int     track       = 1;
    int     screen_line = 1;
    WINDOW *box_window_ptr;
    WINDOW *sub_window_ptr;

    clear_all_screen();
    mvprintw(PROMPT_LINE, 0, "Re-entering tracks for CD. ");
    if (!get_confirm())
        return;
    move(PROMPT_LINE, 0);
    clrtoeol();

    remove_tracks();

    mvprintw(MESSAGE_LINE, 0, "Enter a blank line to finish");

    tracks_fp = fopen(tracks_file, "a");

    /* Just to show how, enter the information in a scrolling, boxed,
       window. The trick is to set-up a sub-window, draw a box around the
       edge, then add a new, scrolling, sub-window just inside the boxed
       sub-window. */
    box_window_ptr = subwin(stdscr, BOXED_LINES + 2, BOXED_ROWS + 2, BOX_LINE_POS - 1, BOX_ROW_POS - 1);
    if (!box_window_ptr)
        return;
    box(box_window_ptr, ACS_VLINE, ACS_HLINE);

    sub_window_ptr = subwin(stdscr, BOXED_LINES, BOXED_ROWS, BOX_LINE_POS, BOX_ROW_POS);
    if (!sub_window_ptr)
        return;
    scrollok(sub_window_ptr, TRUE);
    werase(sub_window_ptr);
    touchwin(stdscr);

    do
    {

        mvwprintw(sub_window_ptr, screen_line++, BOX_ROW_POS + 2, "Track %d: ", track);
        clrtoeol();
        refresh();
        wgetnstr(sub_window_ptr, track_name, MAX_STRING);
        len = strlen(track_name);
        if (len > 0 && track_name[len - 1] == '\n')
            track_name[len - 1] = '\0';

        if (*track_name)
            fprintf(tracks_fp, "%s,%d,%s\n", current_cat, track, track_name);
        track++;
        if (screen_line > BOXED_LINES - 1)
        {
            /* time to start scrolling */
            scroll(sub_window_ptr);
            screen_line--;
        }
    } while (*track_name);
    delwin(sub_window_ptr);

    fclose(tracks_fp);
}
