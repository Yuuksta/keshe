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

#define SCR_ROW 25             /*��Ļ����*/
#define SCR_COL 80             /*��Ļ����*/

/*��Ȼ�ֺ�������Ϣ�����ṹ*/
typedef struct assort
{
        char name [15];  /*��Ȼ�ֺ�����*/
        char type ;      /*��Ȼ�ֺ����*/
        struct assort *next; /*ָ����һ������ָ��*/
        struct event *enext; /*ָ���ֺ��¼�֧����ָ��*/
}ASSORT;

/*��Ȼ�ֺ��¼�������Ϣ�����ṹ*/
typedef struct event
{
    char type;           /*��Ȼ�ֺ����*/
    char num[12];        /*�¼����*/
    char time[15];       /*����ʱ��*/
    char name[15];       /*�ֺ�����*/
    char level;          /*�ֺ�����*/
    char place[20];      /*�����ص�*/
    int dead;            /*��������*/
    int collapse;        /*�񷿵���*/
    float loss;            /*ֱ�Ӿ�����ʧ*/
    struct event *next;  /*ָ����һ������ָ��*/
    struct donate *dnext; /*ָ����֧����ָ��*/
}EVENT;

/*��Ȼ�ֺ������Ϣ�����ṹ*/
typedef struct donate
{
    char num[12];        /*�¼����*/
    char time[10];       /*�������*/
    char type;           /*������*/
    char dname[20];      /*���������*/
    float money;         /*�����Ŀ*/
    struct donate *next;  /*ָ����һ������ָ��*/
}DONATE;

/*��Ļ������Ϣ�������ṹ*/
typedef struct layer_node {
    char LayerNo;            /*�������ڲ���*/
    SMALL_RECT rcArea;       /*����������������*/
    CHAR_INFO *pContent;     /*�������������ַ���Ԫԭ��Ϣ�洢������*/
    char *pScrAtt;           /*�������������ַ���Ԫԭ����ֵ�洢������*/
    struct layer_node *next; /*ָ����һ����ָ��*/
} LAYER_NODE;

/*��ǩ���ṹ*/
typedef struct labe1_bundle {
    char **ppLabel;        /*ǩ�ַ��������׵�ַ*/
    COORD *pLoc;           /*��ǩ��λ�����׵�ַ*/
    int num;               /*��ǩ����*/
} LABEL_BUNDLE;

/*�����ṹ*/
typedef struct hot_area {
    SMALL_RECT *pArea;     /*������λ�����׵�ַ*/
    char *pSort;           /*�����(�������ı���ѡ���)�����׵�ַ*/
    char *pTag;            /*������������׵�ַ*/
    int num;               /*��������*/
} HOT_AREA;

unsigned long ul;

LAYER_NODE *gp_top_layer = NULL;               /*����������Ϣ����ͷ*/
ASSORT *gp_head = NULL;                     /*����ͷָ��*/

char *gp_sys_name = "��Ȼ�ֺ���Ϣ����ϵͳ";    /*ϵͳ����*/
char *gp_assort_info_filename = "assort.dat";  /*��Ȼ�ֺ�������Ϣ�����ļ�*/
char *gp_event_info_filename = "event.dat";   /*��Ȼ�ֺ��¼�������Ϣ�����ļ�*/
char *gp_donate_info_filename = "donate.dat";    /*��Ȼ�ֺ������Ϣ�����ļ�*/
char *gp_backup_filename = "backup.log";       /*���������ļ�*/
char *ga_main_menu[] = {"�ļ�(F)",             /*ϵͳ���˵���*/
    	                "����ά��(M)",
                        "���ݲ�ѯ(Q)",
                        "����ͳ��(S)",
                        "����(H)"
                       };

char *ga_sub_menu[] = {"[S] ���ݱ���",          /*ϵͳ�Ӳ˵���*/
                       "[B] ���ݱ���",
                       "",
                       "[X] �˳�    Alt+X",

                       "[A] ��Ȼ�ֺ�������Ϣά��",
                       "[E] ��Ȼ�ֺ��¼�������Ϣά��",
                       "[D] ��Ȼ�ֺ������Ϣά��",

                       "[S] ��ʾȫ��",
                       "[A] ��Ȼ�ֺ�������Ϣ��ѯ",
                       "[E] ��Ȼ�ֺ�������Ϣ��ѯ",
                       "[D] ��Ȼ�ֺ������Ϣ��ѯ",

                       "[N] ���ֺ����",
                       "[A] ���ֺ�����",
                       "[B] ����ݺ��ֺ�����",
                       "[D] ���������",
                       "[R] ���ֺ��ȼ�",

                       "����",
                       "",
                       "����",
                      };

int ga_sub_menu_count[] = {4, 3, 4, 5, 3};  /*�����˵������Ӳ˵��ĸ���*/
int gi_sel_menu = 1;                        /*��ѡ�е����˵����,��ʼΪ1*/
int gi_sel_sub_menu = 0;                    /*��ѡ�е��Ӳ˵����,��ʼΪ0,��ʾδѡ��*/

CHAR_INFO *gp_buff_menubar_info = NULL;     /*��Ų˵�����Ļ���ַ���Ϣ�Ļ�����*/
CHAR_INFO *gp_buff_stateBar_info = NULL;    /*���״̬����Ļ���ַ���Ϣ�Ļ�����*/

char *gp_scr_att = NULL;    /*�����Ļ���ַ���Ԫ����ֵ�Ļ�����*/
char gc_sys_state = '\0';   /*��������ϵͳ״̬���ַ�*/

HANDLE gh_std_out;          /*��׼����豸���*/
HANDLE gh_std_in;           /*��׼�����豸���*/

int LoadCode(char *filename, char **ppbuffer);  /*��������*/
int CreatList(ASSORT **phead);              /*���������ʼ��*/
void InitInterface(void);                 /*ϵͳ�����ʼ��*/
void ClearScreen(void);                         /*����*/
void ShowMenu(void);                            /*��ʾ�˵���*/
void PopMenu(int num);                          /*��ʾ�����˵�*/
void PopPrompt(int num);                        /*��ʾ��������*/
void PopUp(SMALL_RECT *, WORD, LABEL_BUNDLE *, HOT_AREA *);  /*����������Ļ��Ϣά��*/
void PopOff(void);                              /*�رն��㵯������*/
void DrawBox(SMALL_RECT *parea);                /*���Ʊ߿�*/
void LocSubMenu(int num, SMALL_RECT *parea);    /*���˵������˵���λ*/
void ShowState(void);                           /*��ʾ״̬��*/
void TagMainMenu(int num);                      /*��Ǳ�ѡ�е����˵���*/
void TagSubMenu(int num);                       /*��Ǳ�ѡ�е��Ӳ˵���*/
int DealConInput(HOT_AREA *phot_area, int *pihot_num);  /*����̨���봦��*/
void SetHotPoint(HOT_AREA *phot_area, int hot_num);     /*��������*/
void RunSys(ASSORT **pphd);                  /*ϵͳ����ģ���ѡ�������*/
BOOL ExeFunction(int main_menu_num, int sub_menu_num);  /*����ģ��ĵ���*/
void CloseSys(ASSORT *phd);                  /*�˳�ϵͳ*/
BOOL ShowModule(char **pString, int n);
BOOL LoadData (void);        /*��������*/

BOOL SaveData(void);           /*��������*/
BOOL BackupData(void);         /*��������*/
BOOL ExitSys(void);            /*�˳�ϵͳ*/

BOOL ShowAll(void);             /*��ʾ������Ϣ*/
BOOL MaintainAssort(void);    /*ά��������Ϣ*/
BOOL MaintainEvent(void);   /*ά���¼���Ϣ*/
BOOL MaintainDonate(void);   /*ά�������Ϣ*/

BOOL QueryAssort(void);       /*��ѯ������Ϣ*/
BOOL QueryEvent(void);      /*��ѯ�¼���Ϣ*/
BOOL QueryDonate(void);      /*��ѯ�����Ϣ*/

BOOL StatType(void);       /*���ֺ����ͳ��*/
BOOL StatName(void);        /*���ֺ�����ͳ��*/
BOOL StatYear(void);         /*����ݺ��ֺ�����ͳ��*/
BOOL StatDate(void);       /*���������ͳ��*/
BOOL StatRank(void);       /*���ֺ�����ͳ��*/

BOOL HelpTopic(void);          /*��������*/
BOOL About(void);          /*����ϵͳ*/

BOOL InsertAssort(ASSORT **phd, ASSORT *passort);/*������Ȼ�ֺ�������Ϣ���*/
BOOL InsertEvent(ASSORT *phd, EVENT *pevent);/*������Ȼ�ֺ��¼���Ϣ���*/
BOOL InsertDonate(ASSORT *phd, DONATE *pdonate);/*������Ȼ�ֺ������Ϣ���*/

BOOL DelAssort(ASSORT **phd, char type);/*ɾ����Ȼ�ֺ�������Ϣ���*/
BOOL DelEvent(ASSORT *phd, char *num);/*ɾ����Ȼ�ֺ��¼���Ϣ���*/
BOOL DelDonate(ASSORT *phd, char *name,char *dname);/*ɾ����Ȼ�ֺ������Ϣ���*/

BOOL ModifAssort(ASSORT *phd, char type, ASSORT *pAssort);/*�޸���Ȼ�ֺ�������Ϣ���*/
BOOL ModifEvent(ASSORT *phd, char *name, EVENT *pEvent);/*�޸���Ȼ�ֺ��¼���Ϣ���*/
BOOL ModifDonate(ASSORT *phd, char *name, char *dname,DONATE *pDonate);/*�޸���Ȼ�ֺ������Ϣ���*/

ASSORT *SeekAssort1(ASSORT *phd, char *name);  /*������Ȼ�ֺ�������Ϣ���*/
ASSORT *SeekAssort2(ASSORT *phd, char type);  /*������Ȼ�ֺ�������Ϣ���*/
EVENT *SeekEvent(ASSORT *phd, char *num);/*������Ȼ�ֺ��¼���Ϣ���*/
EVENT *SeekEvent2(ASSORT *phd, char *time1,char *time2,char *name);/*������Ȼ�ֺ��¼���Ϣ���*/
DONATE *SeekDonate(ASSORT *phd, char *name,char *dname);/*������Ȼ�ֺ������Ϣ���*/
DONATE *SeekDonate2(ASSORT *phd,char type,float money1,float money2);/*������Ȼ�ֺ������Ϣ���*/


BOOL JudgeAssortItem(ASSORT *pAssort,char *pcond);/*�ж���Ȼ�ֺ�������Ϣ�������Ƿ������ѯ����*/
BOOL JudgeEventItem1(EVENT *pEvent,char *pcond);/*�ж���Ȼ�ֺ��¼���Ϣ�������Ƿ������ѯ����*/
BOOL JudgeEventItem2(EVENT *pEvent,char *pcond1,char *pcond2,char *pcond3);/*�ж���Ȼ�ֺ��¼���Ϣ�������Ƿ������ѯ����*/
BOOL JudgeDonateItem1(DONATE *pDonate,char *pcond1,char *pcond2);/*�ж���Ȼ�ֺ������Ϣ�������Ƿ������ѯ����*/
BOOL JudgeDonateItem2(DONATE *pDonate,char pcond,float money1,float money2);/*�ж���Ȼ�ֺ������Ϣ�������Ƿ������ѯ����*/
BOOL MatchString(char *string_item,char *pcond);/*�ַ������ѯ�����Ƿ�����ָ���������ϵ*/
BOOL MatchChar(char cahr_item,char *pcond);/*�ַ������ѯ�����Ƿ�����ָ�������ϵ*/

BOOL SaveSysData(ASSORT *phd);                       /*����ϵͳ����*/
BOOL BackupSysData(ASSORT *phd, char *filename);     /*����ϵͳ����*/
BOOL RestoreSysData(ASSORT **phd, char *filename);  /*�ָ�ϵͳ����*/
#endif // HEAD_H_INCLUDED
