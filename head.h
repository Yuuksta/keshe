#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <wincon.h>
#include <conio.h>
#include <string.h>
#include <io.h>
#include <fcntl.h>
#include <sys\stat.h>
#include <ctype.h>
#include <time.h>


#ifndef HEAD_H_INCLUDED
#define HEAD_H_INCLUDED

#define SCR_ROW 25             /*屏幕行数*/
#define SCR_COL 80             /*屏幕列数*/

/*自然灾害分类信息链结点结构*/
typedef struct assort
{
        char name [15];  /*自然灾害名称*/
        char type ;      /*自然灾害类别*/
        struct assort *next; /*指向下一个结点的指针*/
        struct event *enext; /*指向灾害事件支链的指针*/
}ASSORT;

/*自然灾害事件基本信息链结点结构*/
typedef struct event
{
    char type;           /*自然灾害类别*/
    char num[12];        /*事件编号*/
    char time[15];       /*发生时间*/
    char name[15];       /*灾害名称*/
    char level;          /*灾害级别*/
    char place[20];      /*发生地点*/
    int dead;            /*死亡人数*/
    int collapse;        /*民房倒塌*/
    float loss;            /*直接经济损失*/
    struct event *next;  /*指向下一个结点的指针*/
    struct donate *dnext; /*指向捐款支链的指针*/
}EVENT;

/*自然灾害捐款信息链结点结构*/
typedef struct donate
{
    char num[12];        /*事件编号*/
    char time[10];       /*捐款日期*/
    char type;           /*捐款类别*/
    char dname[20];      /*捐款者名称*/
    float money;         /*捐款数目*/
    struct donate *next;  /*指向下一个结点的指针*/
}DONATE;

/*屏幕窗口信息链结点结点结构*/
typedef struct layer_node {
    char LayerNo;            /*弹出窗口层数*/
    SMALL_RECT rcArea;       /*弹出窗口区域坐标*/
    CHAR_INFO *pContent;     /*弹出窗口区域字符单元原信息存储缓冲区*/
    char *pScrAtt;           /*弹出窗口区域字符单元原属性值存储缓冲区*/
    struct layer_node *next; /*指向下一结点的指针*/
} LAYER_NODE;

/*标签束结构*/
typedef struct labe1_bundle {
    char **ppLabel;        /*签字符串数组首地址*/
    COORD *pLoc;           /*标签定位数组首地址*/
    int num;               /*标签个数*/
} LABEL_BUNDLE;

/*热区结构*/
typedef struct hot_area {
    SMALL_RECT *pArea;     /*热区定位数组首地址*/
    char *pSort;           /*区类别(按键、文本框、选项框)数组首地址*/
    char *pTag;            /*热区序号数组首地址*/
    int num;               /*热区个数*/
} HOT_AREA;

unsigned long ul;

LAYER_NODE *gp_top_layer = NULL;               /*弹出窗口信息链链头*/
ASSORT *gp_head = NULL;                     /*主链头指针*/

char *gp_sys_name = "自然灾害信息管理系统";    /*系统名称*/
char *gp_assort_info_filename = "assort.dat";  /*自然灾害分类信息数据文件*/
char *gp_event_info_filename = "event.dat";   /*自然灾害事件基本信息数据文件*/
char *gp_donate_info_filename = "donate.dat";    /*自然灾害捐款信息数据文件*/
char *gp_backup_filename = "backup.log";       /*备份数据文件*/
char *ga_main_menu[] = {"文件(F)",             /*系统主菜单名*/
    	                "数据维护(M)",
                        "数据查询(Q)",
                        "数据统计(S)",
                        "帮助(H)"
                       };

char *ga_sub_menu[] = {"[S] 数据保存",          /*系统子菜单名*/
                       "[B] 数据备份",
                       "",
                       "[X] 退出    Alt+X",

                       "[A] 自然灾害分类信息维护",
                       "[E] 自然灾害事件基本信息维护",
                       "[D] 自然灾害捐款信息维护",

                       "[S] 显示全部",
                       "[A] 自然灾害分类信息查询",
                       "[E] 自然灾害基本信息查询",
                       "[D] 自然灾害捐款信息查询",

                       "[N] 按灾害类别",
                       "[A] 按灾害名称",
                       "[B] 按年份和灾害类型",
                       "[D] 按捐款日期",
                       "[R] 按灾害等级",

                       "帮助",
                       "",
                       "关于",
                      };

int ga_sub_menu_count[] = {4, 3, 4, 5, 3};  /*各主菜单项下子菜单的个数*/
int gi_sel_menu = 1;                        /*被选中的主菜单项号,初始为1*/
int gi_sel_sub_menu = 0;                    /*被选中的子菜单项号,初始为0,表示未选中*/

CHAR_INFO *gp_buff_menubar_info = NULL;     /*存放菜单条屏幕区字符信息的缓冲区*/
CHAR_INFO *gp_buff_stateBar_info = NULL;    /*存放状态条屏幕区字符信息的缓冲区*/

char *gp_scr_att = NULL;    /*存放屏幕上字符单元属性值的缓冲区*/
char gc_sys_state = '\0';   /*用来保存系统状态的字符*/

HANDLE gh_std_out;          /*标准输出设备句柄*/
HANDLE gh_std_in;           /*标准输入设备句柄*/

int LoadCode(char *filename, char **ppbuffer);  /*代码表加载*/
int CreatList(ASSORT **phead);              /*数据链表初始化*/
void InitInterface(void);                 /*系统界面初始化*/
void ClearScreen(void);                         /*清屏*/
void ShowMenu(void);                            /*显示菜单栏*/
void PopMenu(int num);                          /*显示下拉菜单*/
void PopPrompt(int num);                        /*显示弹出窗口*/
void PopUp(SMALL_RECT *, WORD, LABEL_BUNDLE *, HOT_AREA *);  /*弹出窗口屏幕信息维护*/
void PopOff(void);                              /*关闭顶层弹出窗口*/
void DrawBox(SMALL_RECT *parea);                /*绘制边框*/
void LocSubMenu(int num, SMALL_RECT *parea);    /*主菜单下拉菜单定位*/
void ShowState(void);                           /*显示状态栏*/
void TagMainMenu(int num);                      /*标记被选中的主菜单项*/
void TagSubMenu(int num);                       /*标记被选中的子菜单项*/
int DealConInput(HOT_AREA *phot_area, int *pihot_num);  /*控制台输入处理*/
void SetHotPoint(HOT_AREA *phot_area, int hot_num);     /*设置热区*/
void RunSys(ASSORT **pphd);                  /*系统功能模块的选择和运行*/
BOOL ExeFunction(int main_menu_num, int sub_menu_num);  /*功能模块的调用*/
void CloseSys(ASSORT *phd);                  /*退出系统*/
BOOL ShowModule(char **pString, int n);
BOOL LoadData (void);        /*加载数据*/

BOOL SaveData(void);           /*保存数据*/
BOOL BackupData(void);         /*备份数据*/
BOOL ExitSys(void);            /*退出系统*/

BOOL ShowAll(void);             /*显示所有信息*/
BOOL MaintainAssort(void);    /*维护分类信息*/
BOOL MaintainEvent(void);   /*维护事件信息*/
BOOL MaintainDonate(void);   /*维护捐款信息*/

BOOL QueryAssort(void);       /*查询分类信息*/
BOOL QueryEvent(void);      /*查询事件信息*/
BOOL QueryDonate(void);      /*查询捐款信息*/

BOOL StatType(void);       /*按灾害类别统计*/
BOOL StatName(void);        /*按灾害名称统计*/
BOOL StatYear(void);         /*按年份和灾害类型统计*/
BOOL StatDate(void);       /*按捐款日期统计*/
BOOL StatRank(void);       /*按灾害级别统计*/

BOOL HelpTopic(void);          /*帮助主体*/
BOOL About(void);          /*关于系统*/

BOOL InsertAssort(ASSORT **phd, ASSORT *passort);/*插入自然灾害分类信息结点*/
BOOL InsertEvent(ASSORT *phd, EVENT *pevent);/*插入自然灾害事件信息结点*/
BOOL InsertDonate(ASSORT *phd, DONATE *pdonate);/*插入自然灾害捐款信息结点*/

BOOL DelAssort(ASSORT **phd, char type);/*删除自然灾害分类信息结点*/
BOOL DelEvent(ASSORT *phd, char *num);/*删除自然灾害事件信息结点*/
BOOL DelDonate(ASSORT *phd, char *name,char *dname);/*删除自然灾害捐款信息结点*/

BOOL ModifAssort(ASSORT *phd, char type, ASSORT *pAssort);/*修改自然灾害分类信息结点*/
BOOL ModifEvent(ASSORT *phd, char *name, EVENT *pEvent);/*修改自然灾害事件信息结点*/
BOOL ModifDonate(ASSORT *phd, char *name, char *dname,DONATE *pDonate);/*修改自然灾害捐款信息结点*/

ASSORT *SeekAssort1(ASSORT *phd, char *name);  /*查找自然灾害分类信息结点*/
ASSORT *SeekAssort2(ASSORT *phd, char type);  /*查找自然灾害分类信息结点*/
EVENT *SeekEvent(ASSORT *phd, char *num);/*查找自然灾害事件信息结点*/
EVENT *SeekEvent2(ASSORT *phd, char *time1,char *time2,char *name);/*查找自然灾害事件信息结点*/
DONATE *SeekDonate(ASSORT *phd, char *name,char *dname);/*查找自然灾害捐款信息结点*/
DONATE *SeekDonate2(ASSORT *phd,char type,float money1,float money2);/*查找自然灾害捐款信息结点*/


BOOL JudgeAssortItem(ASSORT *pAssort,char *pcond);/*判断自然灾害分类信息数据项是否满足查询条件*/
BOOL JudgeEventItem1(EVENT *pEvent,char *pcond);/*判断自然灾害事件信息数据项是否满足查询条件*/
BOOL JudgeEventItem2(EVENT *pEvent,char *pcond1,char *pcond2,char *pcond3);/*判断自然灾害事件信息数据项是否满足查询条件*/
BOOL JudgeDonateItem1(DONATE *pDonate,char *pcond1,char *pcond2);/*判断自然灾害捐款信息数据项是否满足查询条件*/
BOOL JudgeDonateItem2(DONATE *pDonate,char pcond,float money1,float money2);/*判断自然灾害捐款信息数据项是否满足查询条件*/
BOOL MatchString(char *string_item,char *pcond);/*字符串与查询条件是否满足指定的运算关系*/
BOOL MatchChar(char cahr_item,char *pcond);/*字符串与查询条件是否满足指定运算关系*/

BOOL SaveSysData(ASSORT *phd);                       /*保存系统数据*/
BOOL BackupSysData(ASSORT *phd, char *filename);     /*备份系统数据*/
BOOL RestoreSysData(ASSORT **phd, char *filename);  /*恢复系统数据*/
#endif // HEAD_H_INCLUDED
