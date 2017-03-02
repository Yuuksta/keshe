#include "head.h"
#pragma warning(disable:4996)
int main()
{
   COORD size = {SCR_COL, SCR_ROW};              /*窗口缓冲区大小*/

  gh_std_out = GetStdHandle(STD_OUTPUT_HANDLE); /* 获取标准输出设备句柄*/
   gh_std_in = GetStdHandle(STD_INPUT_HANDLE);   /* 获取标准输入设备句柄*/

   SetConsoleTitle(gp_sys_name);                 /*设置窗口标题*/
  SetConsoleScreenBufferSize(gh_std_out, size); /*设置窗口缓冲区大小80*25*/

  LoadData();                   /*数据加载*/
  InitInterface();          /*界面初始化*/
  RunSys(&gp_head);             /*系统功能模块的选择及运行*/
  CloseSys(gp_head);            /*退出系统*/

   return 0;
}

/**
 * 函数名称: CreatList
 * 函数功能: 从数据文件读取基础数据, 并存放到所创建的十字链表中.
 * 输入参数: 无
 * 输出参数: phead 主链头指针的地址, 用来返回所创建的十字链.
 * 返 回 值: int型数值, 表示链表创建的情况.
 *           0  空链, 无数据
 *           4  已加载自然灾害分类信息数据，无事件信息和捐款信息数据
 *           12 已加载自然灾害分类信息和事件信息数据，无捐款信息数据
 *           28 三类基础数据都已加载
 *
 * 调用说明:
 */
int CreatList(ASSORT **phead)
{
    ASSORT *hd = NULL, *pAssort, tmp1;
    EVENT *pEvent, tmp2;
    DONATE *pDonate, tmp3;
    FILE *pFile;
    int find;
    int re = 0;

    if ((pFile = fopen(gp_assort_info_filename, "rb")) == NULL)
    {
        printf("自然灾害分类信息数据文件打开失败!\n");
        return re;
    }
    printf("自然灾害分类信息数据文件打开成功!\n");

    /*从数据文件中读自然灾害分类信息数据，存入以后进先出方式建立的主链中*/
    while (fread(&tmp1, sizeof(ASSORT), 1, pFile) == 1)
    {
        pAssort = (ASSORT*)malloc(sizeof(ASSORT));
        *pAssort = tmp1;
        pAssort->enext = NULL;
        pAssort->next = hd;
        hd = pAssort;
    }
    fclose(pFile);
    if (hd == NULL)
    {
        printf("自然灾害分类信息数据文件加载失败!\n");
        return re;
    }
    printf("自然灾害分类信息数据文件加载成功!\n");
    *phead = hd;
    re += 4;

    if ((pFile = fopen(gp_event_info_filename, "rb")) == NULL)
    {
        printf("自然灾害事件信息数据文件打开失败!\n");
        return re;
    }
    printf("自然灾害事件信息数据文件打开成功!\n");
    re += 8;

    /*从数据文件中读取自然灾害事件信息数据，存入主链对应结点的自然灾害事件信息支链中*/
    while (fread(&tmp2, sizeof(EVENT), 1, pFile) == 1)
    {
        /*创建结点，存放从数据文件中读出的自然灾害事件信息*/
        pEvent = (EVENT *)malloc(sizeof(EVENT));
        *pEvent = tmp2;
        pEvent->dnext = NULL;

        /*在主链上查找该事件所对应自然灾害分类的主链结点*/
        pAssort = hd;
        while (pAssort != NULL
               && (pAssort->type==pEvent->type) != 0)
        {
            pAssort = pAssort->next;
        }
        if (pAssort != NULL) /*如果找到，则将结点以后进先出方式插入自然灾害事件信息支链*/
        {
            pEvent->next = pAssort->enext;
            pAssort->enext = pEvent;
        }
        else  /*如果未找到，则释放所创建结点的内存空间*/
        {
            free(pEvent);
        }
    }
    fclose(pFile);

    if ((pFile = fopen(gp_donate_info_filename, "rb")) == NULL)
    {
        printf("自然灾害捐款信息数据文件打开失败!\n");
        return re;
    }
    printf("自然灾害捐款信息数据文件打开成功!\n");
    re += 16;

    /*从数据文件中读取自然灾害捐款信息数据，存入灾害事件信息支链对应结点的捐款支链中*/
    while (fread(&tmp3, sizeof(DONATE), 1, pFile) == 1)
    {
        /*创建结点，存放从数据文件中读出的灾害捐款信息*/
        pDonate = (DONATE *)malloc(sizeof(DONATE));
        *pDonate = tmp3;

        /*查找自然灾害事件支链上对应事件结点*/
        pAssort = hd;
        find = 0;
        while (pAssort != NULL && find == 0)
        {
            pEvent = pAssort->enext;
            while (pEvent != NULL && find == 0)
            {
                if (strcmp(pEvent->num, pDonate->num) == 0)
                {
                    find = 1;
                    break;
                }
                pEvent = pEvent->next;
            }
            pAssort = pAssort->next;
        }
        if (find)  /*如果找到，则将结点以后进先出方式插入自然灾害捐款信息支链中*/
        {
            pDonate->next = pEvent->dnext;
            pEvent->dnext = pDonate;
        }
        else /*如果未找到，则释放所创建结点的内存空间*/
        {
            free(pDonate);
        }
    }
    fclose(pFile);

    return re;
}

/**
 * 函数名称: LoadCode
 * 函数功能: 将代码表从数据文件载入到内存缓冲区, 并进行排序和去除空格.
 * 输入参数: FileName 存放代码表的数据文件名.
 * 输出参数: pBuffer 指向内存缓冲区的指针变量的地址.
 * 返 回 值: 存放代码表的内存缓冲区大小(以字节为单位).
 *
 * 调用说明:
 */
int LoadCode(char *FileName, char **pBuffer)
{
    char *pTemp, *pStr1, *pStr2;
    int handle;
    int BufferLen, len, loc1, loc2, i;
    long filelen;

    if ((handle = open(FileName, O_RDONLY | O_TEXT)) == -1) /*如果以只读方式打开失败 */
    {
        handle = open(FileName, O_CREAT | O_TEXT, S_IREAD); /*以创建方式打开*/
    }
    filelen = filelength(handle);      /*数据文件的长度*/
    pTemp = (char *)calloc(filelen + 1, sizeof(char)); /*申请同样大小的动态存储区*/
    BufferLen = read(handle, pTemp, filelen); /*将数据文件的内容全部读入到内存*/
    close(handle);

    *(pTemp + BufferLen) = '\0'; /*在动态存储区尾存一个空字符，作为字符串结束标志*/
    BufferLen++;

    for (i=0; i<BufferLen; i++) /*将动态存储区中的所有换行符替换成空字符*/
    {
        if (*(pTemp + i) == '\n')
        {
            *(pTemp + i) = '\0';
        }
    }

    /*再申请一块同样大小的动态存储区，用于存放排序后的代码串*/
    *pBuffer = (char *)calloc(BufferLen, sizeof(char));
    loc2 = 0;
    pStr1 = pTemp;
    len = strlen(pStr1);

    while (BufferLen > len + 1) /*选择法排序*/
    {
        loc1 = len + 1;
        while (BufferLen > loc1) /*每趟找到序列中最小代码串，首地址存入pStr1*/
        {
            pStr2 = pTemp + loc1;
            if (strcmp(pStr1, pStr2) > 0)
            {
                pStr1 = pStr2;
            }
            loc1 += strlen(pStr2) + 1;
        }
        len = strlen(pStr1);  /*这一趟所找到的最小代码串长度*/

        /*如果不是空串，则进行复制，loc2是下一个最小代码串存放地址的偏移量*/
        if (len > 0)
        {
            strcpy(*pBuffer + loc2, pStr1);
            loc2 += len + 1;  /*已复制的代码串所占存储空间大小*/
        }

        /*将最小代码串从序列中删除掉*/
        for(i=0; i<BufferLen-(pStr1-pTemp)-(len+1); i++)
        {
            *(pStr1 + i) = *(pStr1 + i + len + 1);
        }

        BufferLen -= len + 1; /*下一趟排序所处理序列的长度*/
        pStr1 = pTemp;  /*假定序列的第一个代码串为最小代码串*/
        len = strlen(pStr1);
    } /*序列中只剩下一个代码串时，排序结束*/

    /*复制最后这个代码串*/
    len = strlen(pStr1);
    strcpy(*pBuffer + loc2, pStr1);

    /*修改动态存储区大小，使其正好放下排序后代码串*/
    loc2 += len + 1;
    *pBuffer = (char *)realloc(*pBuffer, loc2);
    free(pTemp);  /*释放最先申请的动态存储区*/

    return loc2;  /*返回存放代码串的内存缓冲区实际大小*/
}

/**
 * 函数名称: LoadData
 * 函数功能: 将三类基础数据从数据文件载入到内存缓冲区和十字链表中.
 * 输入参数: 无.
 * 输出参数: 无.
 * 返 回 值: bool类型, 功能函数中除了函数ExitSys的返回值可以为外,
 *           其他函数的返回值必须为true.
 *
 * 调用说明: 为了能够以统一的方式调用各功能函数, 将这些功能函数的原型设为
 *           一致, 即无参数且返回值为BOOL. 返回值为时, 结束程序运行.
 */
 BOOL LoadData(void)
{
    int Re ;
    Re = CreatList(&gp_head);


    gc_sys_state |= Re;
    gc_sys_state &= ~(4 + 8 + 16 - Re);
    if (gc_sys_state < (1 | 2 | 4 | 8 | 16))
    {
        /*数据加载提示信息*/

        printf("\n                         按任意键继续...\n");
        getch();
    }

    return TRUE;
}

/**
 * 函数名称: set_text_success
 * 函数功能: 某项操作成功，并弹出提醒窗口
 * 输入参数: 无
 * 输出参数: 无
 * 返 回 值: BOOL类型, TRUE
 *
 * 调用说明:
 */
BOOL set_text_success(void)
{
    BOOL bRet = TRUE;
    LABEL_BUNDLE labels;
    HOT_AREA areas;
    SMALL_RECT rcPop;
    COORD pos;
    WORD att;
    char *pCh[] = {"操作成功", "返回"};
    int iHot = 1;
    pos.X = strlen(pCh[0]) +6;
    pos.Y = 7;
    rcPop.Left = (SCR_COL - pos.X) / 2;
    rcPop.Right = rcPop.Left + pos.X - 1;
    rcPop.Top = (25 - pos.Y) / 2;
    rcPop.Bottom = rcPop.Top + pos.Y - 1;
    att = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED|FOREGROUND_RED|FOREGROUND_INTENSITY;  /*白底黑字*/
    labels.num = 2;
    labels.ppLabel = pCh;
    COORD aLoc[] = {{rcPop.Left+3, rcPop.Top+2},
        {rcPop.Left+5, rcPop.Top+5}
    };
    labels.pLoc = aLoc;

    areas.num = 2;
    SMALL_RECT aArea[] =
    {
        {
            rcPop.Left + 5, rcPop.Top + 5,
            rcPop.Left + 8, rcPop.Top +5
        }
    };
    char aSort[] = {0};
    char aTag[] = {1};
    areas.pArea = aArea;
    areas.pSort = aSort;
    areas.pTag = aTag;
    PopUp(&rcPop, att, &labels, &areas);
    if(DealInput(&areas, &iHot) == 13 && iHot == 1)
    {
        PopOff();
    }
    return bRet;
}


/**
 * 函数名称: set_text_fail
 * 函数功能: 某项操作失败，并弹出提醒窗口
 * 输入参数: 无
 * 输出参数: 无
 * 返 回 值: BOOL类型, TRUE
 *
 * 调用说明:
 */
BOOL set_text_fail(void)
{
    BOOL bRet = TRUE;
    LABEL_BUNDLE labels;
    HOT_AREA areas;
    SMALL_RECT rcPop;
    COORD pos;
    WORD att;
    char *pCh[] = {"操作无效", "返回"};
    int iHot = 1;
    pos.X = strlen(pCh[0]) +6;
    pos.Y = 7;
    rcPop.Left = (SCR_COL - pos.X) / 2;
    rcPop.Right = rcPop.Left + pos.X - 1;
    rcPop.Top = (25 - pos.Y) / 2;
    rcPop.Bottom = rcPop.Top + pos.Y - 1;
    att = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED|FOREGROUND_RED|FOREGROUND_INTENSITY;  /*白底黑字*/
    labels.num = 2;
    labels.ppLabel = pCh;
    COORD aLoc[] = {{rcPop.Left+3, rcPop.Top+2},
        {rcPop.Left+5, rcPop.Top+5}
    };
    labels.pLoc = aLoc;
    areas.num = 2;
    SMALL_RECT aArea[] =
    {
        {
            rcPop.Left + 5, rcPop.Top + 5,
            rcPop.Left + 8, rcPop.Top +5
        }
    };
    char aSort[] = {0};
    char aTag[] = {1};
    areas.pArea = aArea;
    areas.pSort = aSort;
    areas.pTag = aTag;
    PopUp(&rcPop, att, &labels, &areas);
    if(DealInput(&areas, &iHot) == 13 && iHot == 1)
    {
        PopOff();
    }
    return bRet;
}
/**
 * 函数名称: InitInterface
 * 函数功能: 初始化界面.
 * 输入参数: 无
 * 输出参数: 无
 * 返 回 值: 无
 *
 * 调用说明:
 */
void InitInterface()
{
    WORD att = FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_INTENSITY
               | BACKGROUND_GREEN;

    SetConsoleTextAttribute(gh_std_out, att);  /*设置控制台屏幕缓冲区字符属性*/

    ClearScreen();  /* 清屏*/

    /*创建弹出窗口信息堆栈，将初始化后的屏幕窗口当作第一层弹出窗口*/
    gp_scr_att = (char *)calloc(SCR_COL * SCR_ROW, sizeof(char));/*屏幕字符属性*/
    gp_top_layer = (LAYER_NODE *)malloc(sizeof(LAYER_NODE));
    gp_top_layer->LayerNo = 0;      /*弹出窗口的层号为0*/
    gp_top_layer->rcArea.Left = 0;  /*弹出窗口的区域为整个屏幕窗口*/
    gp_top_layer->rcArea.Top = 0;
    gp_top_layer->rcArea.Right = SCR_COL - 1;
    gp_top_layer->rcArea.Bottom = SCR_ROW - 1;
    gp_top_layer->pContent = NULL;
    gp_top_layer->pScrAtt = gp_scr_att;
    gp_top_layer->next = NULL;

    ShowMenu();     /*显示菜单栏*/
    ShowState();    /*显示状态栏*/

    return;
}

/**
 * 函数名称: ClearScreen
 * 函数功能: 清除屏幕信息.
 * 输入参数: 无
 * 输出参数: 无
 * 返 回 值: 无
 *
 * 调用说明:
 */
void ClearScreen(void)
{
    CONSOLE_SCREEN_BUFFER_INFO bInfo;
    COORD home = {0, 0};
    unsigned long size;

    GetConsoleScreenBufferInfo( gh_std_out, &bInfo );/*取屏幕缓冲区信息*/
    size =  bInfo.dwSize.X * bInfo.dwSize.Y; /*计算屏幕缓冲区字符单元数*/

    /*将屏幕缓冲区所有单元的字符属性设置为当前屏幕缓冲区字符属性*/
    FillConsoleOutputAttribute(gh_std_out, bInfo.wAttributes, size, home, &ul);

    /*将屏幕缓冲区所有单元填充为空格字符*/
    FillConsoleOutputCharacter(gh_std_out, ' ', size, home, &ul);
    return;
}


/**
 * 函数名称: ShowMenu
 * 函数功能: 在屏幕上显示主菜单, 并设置热区, 在主菜单第一项上置选中标记.
 * 输入参数: 无
 * 输出参数: 无
 * 返 回 值: 无
 *
 * 调用说明:
 */
void ShowMenu()
{
    CONSOLE_SCREEN_BUFFER_INFO bInfo;
    CONSOLE_CURSOR_INFO lpCur;
    COORD size;
    COORD pos = {0, 0};
    int i, j;
    int PosA = 2, PosB;
    char ch;

    GetConsoleScreenBufferInfo( gh_std_out, &bInfo );
    size.X = bInfo.dwSize.X;
    size.Y = 1;
    SetConsoleCursorPosition(gh_std_out, pos);
    for (i=0; i < 5; i++) /*在窗口第一行第一列处输出主菜单项*/
    {
        printf("  %s  ", ga_main_menu[i]);
    }

    GetConsoleCursorInfo(gh_std_out, &lpCur);
    lpCur.bVisible = FALSE;
    SetConsoleCursorInfo(gh_std_out, &lpCur);  /*隐藏光标*/

    /*申请动态存储区作为存放菜单条屏幕区字符信息的缓冲区*/
    gp_buff_menubar_info = (CHAR_INFO *)malloc(size.X * size.Y * sizeof(CHAR_INFO));
    SMALL_RECT rcMenu ={0, 0, size.X-1, 0} ;

    /*将窗口第一行的内容读入到存放菜单条屏幕区字符信息的缓冲区中*/
    ReadConsoleOutput(gh_std_out, gp_buff_menubar_info, size, pos, &rcMenu);

    /*将这一行中英文字母置为红色，其他字符单元置为白底黑字*/
    for (i=0; i<size.X; i++)
    {
        (gp_buff_menubar_info+i)->Attributes = BACKGROUND_BLUE | BACKGROUND_GREEN
                                               | BACKGROUND_RED;
        ch = (char)((gp_buff_menubar_info+i)->Char.AsciiChar);
        if ((ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z'))
        {
            (gp_buff_menubar_info+i)->Attributes |= FOREGROUND_RED;
        }
    }

    /*修改后的菜单条字符信息回写到窗口的第一行*/
    WriteConsoleOutput(gh_std_out, gp_buff_menubar_info, size, pos, &rcMenu);
    COORD endPos = {0, 1};
    SetConsoleCursorPosition(gh_std_out, endPos);  /*将光标位置设置在第2行第1列*/

    /*将菜单项置为热区，热区编号为菜单项号，热区类型为0(按钮型)*/
    i = 0;
    do
    {
        PosB = PosA + strlen(ga_main_menu[i]);  /*定位第i+1号菜单项的起止位置*/
        for (j=PosA; j<PosB; j++)
        {
            gp_scr_att[j] |= (i+1) << 2; /*设置菜单项所在字符单元的属性值*/
        }
        PosA = PosB + 4;
        i++;
    } while (i<5);

    TagMainMenu(gi_sel_menu);  /*在选中主菜单项上做标记，gi_sel_menu初值为1*/

    return;
}


/**
 * 函数名称: ShowState
 * 函数功能: 显示状态条.
 * 输入参数: 无
 * 输出参数: 无
 * 返 回 值: 无
 *
 * 调用说明: 状态条字符属性为白底黑字, 初始状态无状态信息.
 */
void ShowState()
{
    CONSOLE_SCREEN_BUFFER_INFO bInfo;
    COORD size;
    COORD pos = {0, 0};
    int i;

    GetConsoleScreenBufferInfo( gh_std_out, &bInfo );
    size.X = bInfo.dwSize.X;
    size.Y = 1;
    SMALL_RECT rcMenu ={0, bInfo.dwSize.Y-1, size.X-1, bInfo.dwSize.Y-1};

    if (gp_buff_stateBar_info == NULL)
    {
        gp_buff_stateBar_info = (CHAR_INFO *)malloc(size.X * size.Y * sizeof(CHAR_INFO));
        ReadConsoleOutput(gh_std_out, gp_buff_stateBar_info, size, pos, &rcMenu);
    }

    for (i=0; i<size.X; i++)
    {
        (gp_buff_stateBar_info+i)->Attributes = BACKGROUND_BLUE | BACKGROUND_GREEN
                                                | BACKGROUND_RED;
/*
        ch = (char)((gp_buff_stateBar_info+i)->Char.AsciiChar);
        if ((ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z'))
        {
            (gp_buff_stateBar_info+i)->Attributes |= FOREGROUND_RED;
        }
*/
    }

    WriteConsoleOutput(gh_std_out, gp_buff_stateBar_info, size, pos, &rcMenu);

    return;
}

/**
 * 函数名称: TagMainMenu
 * 函数功能: 在指定主菜单项上置选中标志.
 * 输入参数: num 选中的主菜单项号
 * 输出参数: 无
 * 返 回 值: 无
 *
 * 调用说明:
 */
void TagMainMenu(int num)
{
    CONSOLE_SCREEN_BUFFER_INFO bInfo;
    COORD size;
    COORD pos = {0, 0};
    int PosA = 2, PosB;
    char ch;
    int i;

    if (num == 0) /*num为0时，将会去除主菜单项选中标记*/
    {
        PosA = 0;
        PosB = 0;
    }
    else  /*否则，定位选中主菜单项的起止位置: PosA为起始位置, PosB为截止位置*/
    {
        for (i=1; i<num; i++)
        {
            PosA += strlen(ga_main_menu[i-1]) + 4;
        }
        PosB = PosA + strlen(ga_main_menu[num-1]);
    }

    GetConsoleScreenBufferInfo( gh_std_out, &bInfo );
    size.X = bInfo.dwSize.X;
    size.Y = 1;

    /*去除选中菜单项前面的菜单项选中标记*/
    for (i=0; i<PosA; i++)
    {
        (gp_buff_menubar_info+i)->Attributes = BACKGROUND_BLUE | BACKGROUND_GREEN
                                               | BACKGROUND_RED;
        ch = (gp_buff_menubar_info+i)->Char.AsciiChar;
        if ((ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z'))
        {
            (gp_buff_menubar_info+i)->Attributes |= FOREGROUND_RED;
        }
    }

    /*在选中菜单项上做标记，黑底白字*/
    for (i=PosA; i<PosB; i++)
    {
        (gp_buff_menubar_info+i)->Attributes = FOREGROUND_BLUE | FOREGROUND_GREEN
                                               | FOREGROUND_RED;
    }

    /*去除选中菜单项后面的菜单项选中标记*/
    for (i=PosB; i<bInfo.dwSize.X; i++)
    {
        (gp_buff_menubar_info+i)->Attributes = BACKGROUND_BLUE | BACKGROUND_GREEN
                                               | BACKGROUND_RED;
        ch = (char)((gp_buff_menubar_info+i)->Char.AsciiChar);
        if ((ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z'))
        {
            (gp_buff_menubar_info+i)->Attributes |= FOREGROUND_RED;
        }
    }

    /*将做好标记的菜单条信息写到窗口第一行*/
    SMALL_RECT rcMenu ={0, 0, size.X-1, 0};
    WriteConsoleOutput(gh_std_out, gp_buff_menubar_info, size, pos, &rcMenu);

    return;
}

/**
 * 函数名称: CloseSys
 * 函数功能: 关闭系统.
 * 输入参数: hd 主链头指针
 * 输出参数: 无
 * 返 回 值: 无
 *
 * 调用说明:
 */
void CloseSys(ASSORT *hd)
{
    ASSORT *pAssort1 = hd, *pAssort2;
    EVENT *pEvent1, *pEvent2;
    DONATE *pDonate1, *pDonate2;

    while (pAssort1 != NULL) /*释放十字交叉链表的动态存储区*/
    {
        pAssort2 = pAssort1->next;
        pEvent1 = pAssort1->enext;
        while (pEvent1 != NULL) /*释放自然灾害事件基本信息支链的动态存储区*/
        {
            pEvent2 = pEvent1->next;
            pDonate1 = pEvent1->dnext;
            while (pDonate1 != NULL) /*释放捐款信息支链的动态存储区*/
            {
                pDonate2 = pDonate1->next;
                free(pDonate1);
                pDonate1 = pDonate2;
            }
            free(pEvent1);
            pEvent1 = pEvent2;
        }
        free(pAssort1);  /*释放主链结点的动态存储区*/
        pAssort1 = pAssort2;
    }

    ClearScreen();        /*清屏*/

    /*释放存放菜单条、状态条等信息动态存储区*/
    free(gp_buff_menubar_info);
    free(gp_buff_stateBar_info);

    /*关闭标准输入和输出设备句柄*/
    CloseHandle(gh_std_out);
    CloseHandle(gh_std_in);

    /*将窗口标题栏置为运行结束*/
    SetConsoleTitle("运行结束");

    return;
}

/**
 * 函数名称: RunSys
 * 函数功能: 运行系统, 在系统主界面下运行用户所选择的功能模块.
 * 输入参数: 无
 * 输出参数: phead 主链头指针的地址
 * 返 回 值: 无
 *
 * 调用说明:
 */
void RunSys(ASSORT **phead)
{
    INPUT_RECORD inRec;
    DWORD res;
    COORD pos = {0, 0};
    BOOL bRet = TRUE;
    int i, loc, num;
    int cNo, cAtt;      /*cNo:字符单元层号, cAtt:字符单元属性*/
    char vkc, asc;      /*vkc:虚拟键代码, asc:字符的ASCII码值*/

    while (bRet)
    {
        /*从控制台输入缓冲区中读一条记录*/
        ReadConsoleInput(gh_std_in, &inRec, 1, &res);

        if (inRec.EventType == MOUSE_EVENT) /*如果记录由鼠标事件产生*/
        {
            pos = inRec.Event.MouseEvent.dwMousePosition;  /*获取鼠标坐标位置*/
            cNo = gp_scr_att[pos.Y * SCR_COL + pos.X] & 3; /*取该位置的层号*/
            cAtt = gp_scr_att[pos.Y * SCR_COL + pos.X] >> 2;/*取该字符单元属性*/
            if (cNo == 0) /*层号为0，表明该位置未被弹出子菜单覆盖*/
            {
                /* cAtt > 0 表明该位置处于热区(主菜单项字符单元)
                 * cAtt != gi_sel_menu 表明该位置的主菜单项未被选中
                 * gp_top_layer->LayerNo > 0 表明当前有子菜单弹出
                 */
                if (cAtt > 0 && cAtt != gi_sel_menu && gp_top_layer->LayerNo > 0)
                {
                    PopOff();            /*关闭弹出的子菜单*/
                    gi_sel_sub_menu = 0; /*将选中子菜单项的项号置为0*/
                    PopMenu(cAtt);       /*弹出鼠标所在主菜单项对应的子菜单*/
                }
            }
            else if (cAtt > 0) /*鼠标所在位置为弹出子菜单的菜单项字符单元*/
            {
                TagSubMenu(cAtt); /*在该子菜单项上做选中标记*/
            }

            if (inRec.Event.MouseEvent.dwButtonState
                == FROM_LEFT_1ST_BUTTON_PRESSED) /*如果按下鼠标左边第一键*/
            {
                if (cNo == 0) /*层号为0，表明该位置未被弹出子菜单覆盖*/
                {
                    if (cAtt > 0) /*如果该位置处于热区(主菜单项字符单元)*/
                    {
                        PopMenu(cAtt);   /*弹出鼠标所在主菜单项对应的子菜单*/
                    }
                    /*如果该位置不属于主菜单项字符单元，且有子菜单弹出*/
                    else if (gp_top_layer->LayerNo > 0)
                    {
                        PopOff();            /*关闭弹出的子菜单*/
                        gi_sel_sub_menu = 0; /*将选中子菜单项的项号置为0*/
                    }
                }
                else /*层号不为0，表明该位置被弹出子菜单覆盖*/
                {
                    if (cAtt > 0) /*如果该位置处于热区(子菜单项字符单元)*/
                    {
                        PopOff(); /*关闭弹出的子菜单*/
                        gi_sel_sub_menu = 0; /*将选中子菜单项的项号置为0*/

                        /*执行对应功能函数:gi_sel_menu主菜单项号,cAtt子菜单项号*/
                        bRet = ExeFunction(gi_sel_menu, cAtt);
                    }
                }
            }
            else if (inRec.Event.MouseEvent.dwButtonState
                     == RIGHTMOST_BUTTON_PRESSED) /*如果按下鼠标右键*/
            {
                if (cNo == 0) /*层号为0，表明该位置未被弹出子菜单覆盖*/
                {
                    PopOff();            /*关闭弹出的子菜单*/
                    gi_sel_sub_menu = 0; /*将选中子菜单项的项号置为0*/
                }
            }
        }
        else if (inRec.EventType == KEY_EVENT  /*如果记录由按键产生*/
                 && inRec.Event.KeyEvent.bKeyDown) /*且键被按下*/
        {
            vkc = inRec.Event.KeyEvent.wVirtualKeyCode; /*获取按键的虚拟键码*/
            asc = inRec.Event.KeyEvent.uChar.AsciiChar; /*获取按键的ASC码*/

            /*系统快捷键的处理*/
            if (vkc == 112) /*如果按下F1键*/
            {
                if (gp_top_layer->LayerNo != 0) /*如果当前有子菜单弹出*/
                {
                    PopOff();            /*关闭弹出的子菜单*/
                    gi_sel_sub_menu = 0; /*将选中子菜单项的项号置为0*/
                }
                bRet = ExeFunction(5, 1);  /*运行帮助主题功能函数*/
            }
            else if (inRec.Event.KeyEvent.dwControlKeyState
                     & (LEFT_ALT_PRESSED | RIGHT_ALT_PRESSED))
            { /*如果按下左或右Alt键*/
                switch (vkc)  /*判断组合键Alt+字母*/
                {
                    case 88:  /*Alt+X 退出*/
                        if (gp_top_layer->LayerNo != 0)
                        {
                            PopOff();
                            gi_sel_sub_menu = 0;
                        }
                        bRet = ExeFunction(1,4);
                        break;
                    case 70:  /*Alt+F*/
                        PopMenu(1);
                        break;
                    case 77: /*Alt+M*/
                        PopMenu(2);
                        break;
                    case 81: /*Alt+Q*/
                        PopMenu(3);
                        break;
                    case 83: /*Alt+S*/
                        PopMenu(4);
                        break;
                    case 72: /*Alt+H*/
                        PopMenu(5);
                        break;
                }
            }
            else if (asc == 0) /*其他控制键的处理*/
            {
                if (gp_top_layer->LayerNo == 0) /*如果未弹出子菜单*/
                {
                    switch (vkc) /*处理方向键(左、右、下)，不响应其他控制键*/
                    {
                        case 37:
                            gi_sel_menu--;
                            if (gi_sel_menu == 0)
                            {
                                gi_sel_menu = 5;
                            }
                            TagMainMenu(gi_sel_menu);
                            break;
                        case 39:
                            gi_sel_menu++;
                            if (gi_sel_menu == 6)
                            {
                                gi_sel_menu = 1;
                            }
                            TagMainMenu(gi_sel_menu);
                            break;
                        case 40:
                            PopMenu(gi_sel_menu);
                            TagSubMenu(1);
                            break;
                    }
                }
                else  /*已弹出子菜单时*/
                {
                    for (loc=0,i=1; i<gi_sel_menu; i++)
                    {
                        loc += ga_sub_menu_count[i-1];
                    }  /*计算该子菜单中的第一项在子菜单字符串数组中的位置(下标)*/
                    switch (vkc) /*方向键(左、右、上、下)的处理*/
                    {
                        case 37:
                            gi_sel_menu--;
                            if (gi_sel_menu < 1)
                            {
                                gi_sel_menu = 5;
                            }
                            TagMainMenu(gi_sel_menu);
                            PopOff();
                            PopMenu(gi_sel_menu);
                            TagSubMenu(1);
                            break;
                        case 38:
                            num = gi_sel_sub_menu - 1;
                            if (num < 1)
                            {
                                num = ga_sub_menu_count[gi_sel_menu-1];
                            }
                            if (strlen(ga_sub_menu[loc+num-1]) == 0)
                            {
                                num--;
                            }
                            TagSubMenu(num);
                            break;
                        case 39:
                            gi_sel_menu++;
                            if (gi_sel_menu > 5)
                            {
                                gi_sel_menu = 1;
                            }
                            TagMainMenu(gi_sel_menu);
                            PopOff();
                            PopMenu(gi_sel_menu);
                            TagSubMenu(1);
                            break;
                        case 40:
                            num = gi_sel_sub_menu + 1;
                            if (num > ga_sub_menu_count[gi_sel_menu-1])
                            {
                                num = 1;
                            }
                            if (strlen(ga_sub_menu[loc+num-1]) == 0)
                            {
                                num++;
                            }
                            TagSubMenu(num);
                            break;
                    }
                }
            }
            else if ((asc-vkc == 0) || (asc-vkc == 32)){  /*按下普通键*/
                if (gp_top_layer->LayerNo == 0)  /*如果未弹出子菜单*/
                {
                    switch (vkc)
                    {
                        case 70: /*f或F*/
                            PopMenu(1);
                            break;
                        case 77: /*m或M*/
                            PopMenu(2);
                            break;
                        case 81: /*q或Q*/
                            PopMenu(3);
                            break;
                        case 83: /*s或S*/
                            PopMenu(4);
                            break;
                        case 72: /*h或H*/
                            PopMenu(5);
                            break;
                        case 13: /*回车*/
                            PopMenu(gi_sel_menu);
                            TagSubMenu(1);
                            break;
                    }
                }
                else /*已弹出子菜单时的键盘输入处理*/
                {
                    if (vkc == 27) /*如果按下ESC键*/
                    {
                        PopOff();
                        gi_sel_sub_menu = 0;
                    }
                    else if(vkc == 13) /*如果按下回车键*/
                    {
                        num = gi_sel_sub_menu;
                        PopOff();
                        gi_sel_sub_menu = 0;
                        bRet = ExeFunction(gi_sel_menu, num);
                    }
                    else /*其他普通键的处理*/
                    {
                        /*计算该子菜单中的第一项在子菜单字符串数组中的位置(下标)*/
                        for (loc=0,i=1; i<gi_sel_menu; i++)
                        {
                            loc += ga_sub_menu_count[i-1];
                        }

                        /*依次与当前子菜单中每一项的代表字符进行比较*/
                        for (i=loc; i<loc+ga_sub_menu_count[gi_sel_menu-1]; i++)
                        {
                            if (strlen(ga_sub_menu[i])>0 && vkc==ga_sub_menu[i][1])
                            { /*如果匹配成功*/
                                PopOff();
                                gi_sel_sub_menu = 0;
                                bRet = ExeFunction(gi_sel_menu, i-loc+1);
                            }
                        }
                    }
                }
            }
        }
    }
}


/**
 * 函数名称: PopMenu
 * 函数功能: 弹出指定主菜单项对应的子菜单.
 * 输入参数: num 指定的主菜单项号
 * 输出参数: 无
 * 返 回 值: 无
 *
 * 调用说明:
 */
void PopMenu(int num)
{
    LABEL_BUNDLE labels;
    HOT_AREA areas;
    SMALL_RECT rcPop;
    COORD pos;
    WORD att;
    char *pCh;
    int i, j, loc = 0;

    if (num != gi_sel_menu)       /*如果指定主菜单不是已选中菜单*/
    {
        if (gp_top_layer->LayerNo != 0) /*如果此前已有子菜单弹出*/
        {
            PopOff();
            gi_sel_sub_menu = 0;
        }
    }
    else if (gp_top_layer->LayerNo != 0) /*若已弹出该子菜单，则返回*/
    {
        return;
    }

    gi_sel_menu = num;    /*将选中主菜单项置为指定的主菜单项*/
    TagMainMenu(gi_sel_menu); /*在选中的主菜单项上做标记*/
    LocSubMenu(gi_sel_menu, &rcPop); /*计算弹出子菜单的区域位置, 存放在rcPop中*/

    /*计算该子菜单中的第一项在子菜单字符串数组中的位置(下标)*/
    for (i=1; i<gi_sel_menu; i++)
    {
        loc += ga_sub_menu_count[i-1];
    }
    /*将该组子菜单项项名存入标签束结构变量*/
    labels.ppLabel = ga_sub_menu + loc;   /*标签束第一个标签字符串的地址*/
    labels.num = ga_sub_menu_count[gi_sel_menu-1]; /*标签束中标签字符串的个数*/
    COORD aLoc[labels.num];/*定义一个坐标数组，存放每个标签字符串输出位置的坐标*/
    for (i=0; i<labels.num; i++) /*确定标签字符串的输出位置，存放在坐标数组中*/
    {
        aLoc[i].X = rcPop.Left + 2;
        aLoc[i].Y = rcPop.Top + i + 1;
    }
    labels.pLoc = aLoc; /*使标签束结构变量labels的成员pLoc指向坐标数组的首元素*/
    /*设置热区信息*/
    areas.num = labels.num;       /*热区的个数，等于标签的个数，即子菜单的项数*/
    SMALL_RECT aArea[areas.num];                    /*定义数组存放所有热区位置*/
    char aSort[areas.num];                      /*定义数组存放所有热区对应类别*/
    char aTag[areas.num];                         /*定义数组存放每个热区的编号*/
    for (i=0; i<areas.num; i++)
    {
        aArea[i].Left = rcPop.Left + 2;  /*热区定位*/
        aArea[i].Top = rcPop.Top + i + 1;
        aArea[i].Right = rcPop.Right - 2;
        aArea[i].Bottom = aArea[i].Top;
        aSort[i] = 0;       /*热区类别都为0(按钮型)*/
        aTag[i] = i + 1;           /*热区按顺序编号*/
    }
    areas.pArea = aArea;/*使热区结构变量areas的成员pArea指向热区位置数组首元素*/
    areas.pSort = aSort;/*使热区结构变量areas的成员pSort指向热区类别数组首元素*/
    areas.pTag = aTag;   /*使热区结构变量areas的成员pTag指向热区编号数组首元素*/

    att = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED;  /*白底黑字*/
    PopUp(&rcPop, att, &labels, &areas);
    DrawBox(&rcPop);  /*给弹出窗口画边框*/
    pos.X = rcPop.Left + 2;
    for (pos.Y=rcPop.Top+1; pos.Y<rcPop.Bottom; pos.Y++)
    { /*此循环用来在空串子菜项位置画线形成分隔，并取消此菜单项的热区属性*/
        pCh = ga_sub_menu[loc+pos.Y-rcPop.Top-1];
        if (strlen(pCh)==0) /*串长为0，表明为空串*/
        {   /*首先画横线*/
            FillConsoleOutputCharacter(gh_std_out, '-', rcPop.Right-rcPop.Left-3, pos, &ul);
            for (j=rcPop.Left+2; j<rcPop.Right-1; j++)
            {   /*取消该区域字符单元的热区属性*/
                gp_scr_att[pos.Y*SCR_COL+j] &= 3; /*按位与的结果保留了低两位*/
            }
        }

    }
    /*将子菜单项的功能键设为白底红字*/
    pos.X = rcPop.Left + 3;
    att =  FOREGROUND_RED | BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED;
    for (pos.Y=rcPop.Top+1; pos.Y<rcPop.Bottom; pos.Y++)
    {
        if (strlen(ga_sub_menu[loc+pos.Y-rcPop.Top-1])==0)
        {
            continue;  /*跳过空串*/
        }
        FillConsoleOutputAttribute(gh_std_out, att, 1, pos, &ul);
    }
    return;
}

void PopPrompt(int num)
{

}

/**
 * 函数名称: PopUp
 * 函数功能: 在指定区域输出弹出窗口信息, 同时设置热区, 将弹出窗口位置信息入栈.
 * 输入参数: pRc 弹出窗口位置数据存放的地址
 *           att 弹出窗口区域字符属性
 *           pLabel 弹出窗口中标签束信息存放的地址
             pHotArea 弹出窗口中热区信息存放的地址
 * 输出参数: 无
 * 返 回 值: 无
 *
 * 调用说明:
 */
void PopUp(SMALL_RECT *pRc, WORD att, LABEL_BUNDLE *pLabel, HOT_AREA *pHotArea)
{
    LAYER_NODE *nextLayer;
    COORD size;
    COORD pos = {0, 0};
    char *pCh;
    int i, j, row;

    /*弹出窗口所在位置字符单元信息入栈*/
    size.X = pRc->Right - pRc->Left + 1;    /*弹出窗口的宽度*/
    size.Y = pRc->Bottom - pRc->Top + 1;    /*弹出窗口的高度*/
    /*申请存放弹出窗口相关信息的动态存储区*/
    nextLayer = (LAYER_NODE *)malloc(sizeof(LAYER_NODE));
    nextLayer->next = gp_top_layer;
    nextLayer->LayerNo = gp_top_layer->LayerNo + 1;
    nextLayer->rcArea = *pRc;
    nextLayer->pContent = (CHAR_INFO *)malloc(size.X*size.Y*sizeof(CHAR_INFO));
    nextLayer->pScrAtt = (char *)malloc(size.X*size.Y*sizeof(char));
    pCh = nextLayer->pScrAtt;
    /*将弹出窗口覆盖区域的字符信息保存，用于在关闭弹出窗口时恢复原样*/
    ReadConsoleOutput(gh_std_out, nextLayer->pContent, size, pos, pRc);
    for (i=pRc->Top; i<=pRc->Bottom; i++)
    {   /*此二重循环将所覆盖字符单元的原先属性值存入动态存储区，便于以后恢复*/
        for (j=pRc->Left; j<=pRc->Right; j++)
        {
            *pCh = gp_scr_att[i*SCR_COL+j];
            pCh++;
        }
    }
    gp_top_layer = nextLayer;  /*完成弹出窗口相关信息入栈操作*/
    /*设置弹出窗口区域字符的新属性*/
    pos.X = pRc->Left;
    pos.Y = pRc->Top;
    for (i=pRc->Top; i<=pRc->Bottom; i++)
    {
        FillConsoleOutputAttribute(gh_std_out, att, size.X, pos, &ul);
        pos.Y++;
    }
    /*将标签束中的标签字符串在设定的位置输出*/
    for (i=0; i<pLabel->num; i++)
    {
        pCh = pLabel->ppLabel[i];
        if (strlen(pCh) != 0)
        {
            WriteConsoleOutputCharacter(gh_std_out, pCh, strlen(pCh),
                                        pLabel->pLoc[i], &ul);
        }
    }
    /*设置弹出窗口区域字符单元的新属性*/
    for (i=pRc->Top; i<=pRc->Bottom; i++)
    {   /*此二重循环设置字符单元的层号*/
        for (j=pRc->Left; j<=pRc->Right; j++)
        {
            gp_scr_att[i*SCR_COL+j] = gp_top_layer->LayerNo;
        }
    }

    for (i=0; i<pHotArea->num; i++)
    {   /*此二重循环设置所有热区中字符单元的热区类型和热区编号*/
        row = pHotArea->pArea[i].Top;
        for (j=pHotArea->pArea[i].Left; j<=pHotArea->pArea[i].Right; j++)
        {
            gp_scr_att[row*SCR_COL+j] |= (pHotArea->pSort[i] << 6)
                                    | (pHotArea->pTag[i] << 2);
        }
    }
    return;
}

/**
 * 函数名称: PopOff
 * 函数功能: 关闭顶层弹出窗口, 恢复覆盖区域原外观和字符单元原属性.
 * 输入参数: 无
 * 输出参数: 无
 * 返 回 值: 无
 *
 * 调用说明:
 */
void PopOff(void)
{
    LAYER_NODE *nextLayer;
    COORD size;
    COORD pos = {0, 0};
    char *pCh;
    int i, j;

    if ((gp_top_layer->next==NULL) || (gp_top_layer->pContent==NULL))
    {   /*栈底存放的主界面屏幕信息，不用关闭*/
        return;
    }
    nextLayer = gp_top_layer->next;
    /*恢复弹出窗口区域原外观*/
    size.X = gp_top_layer->rcArea.Right - gp_top_layer->rcArea.Left + 1;
    size.Y = gp_top_layer->rcArea.Bottom - gp_top_layer->rcArea.Top + 1;
    WriteConsoleOutput(gh_std_out, gp_top_layer->pContent, size, pos, &(gp_top_layer->rcArea));
    /*恢复字符单元原属性*/
    pCh = gp_top_layer->pScrAtt;
    for (i=gp_top_layer->rcArea.Top; i<=gp_top_layer->rcArea.Bottom; i++)
    {
        for (j=gp_top_layer->rcArea.Left; j<=gp_top_layer->rcArea.Right; j++)
        {
            gp_scr_att[i*SCR_COL+j] = *pCh;
            pCh++;
        }
    }
    free(gp_top_layer->pContent);    /*释放动态存储区*/
    free(gp_top_layer->pScrAtt);
    free(gp_top_layer);
    gp_top_layer = nextLayer;
    gi_sel_sub_menu = 0;
    return;
}

/**
 * 函数名称: DrawBox
 * 函数功能: 在指定区域画边框.
 * 输入参数: pRc 存放区域位置信息的地址
 * 输出参数: 无
 * 返 回 值: 无
 *
 * 调用说明:
 */
void DrawBox(SMALL_RECT *pRc)
{
    char chBox[] = {'+','-','|'};  /*画框用的字符*/
    COORD pos = {pRc->Left, pRc->Top};  /*定位在区域的左上角*/

    WriteConsoleOutputCharacter(gh_std_out, &chBox[0], 1, pos, &ul);/*画边框左上角*/
    for (pos.X = pRc->Left + 1; pos.X < pRc->Right; pos.X++)
    {   /*此循环画上边框横线*/
        WriteConsoleOutputCharacter(gh_std_out, &chBox[1], 1, pos, &ul);
    }
    pos.X = pRc->Right;
    WriteConsoleOutputCharacter(gh_std_out, &chBox[0], 1, pos, &ul);/*画边框右上角*/
    for (pos.Y = pRc->Top+1; pos.Y < pRc->Bottom; pos.Y++)
    {   /*此循环画边框左边线和右边线*/
        pos.X = pRc->Left;
        WriteConsoleOutputCharacter(gh_std_out, &chBox[2], 1, pos, &ul);
        pos.X = pRc->Right;
        WriteConsoleOutputCharacter(gh_std_out, &chBox[2], 1, pos, &ul);
    }
    pos.X = pRc->Left;
    pos.Y = pRc->Bottom;
    WriteConsoleOutputCharacter(gh_std_out, &chBox[0], 1, pos, &ul);/*画边框左下角*/
    for (pos.X = pRc->Left + 1; pos.X < pRc->Right; pos.X++)
    {   /*画下边框横线*/
        WriteConsoleOutputCharacter(gh_std_out, &chBox[1], 1, pos, &ul);
    }
    pos.X = pRc->Right;
    WriteConsoleOutputCharacter(gh_std_out, &chBox[0], 1, pos, &ul);/*画边框右下角*/
    return;
}

/**
 * 函数名称: TagSubMenu
 * 函数功能: 在指定子菜单项上做选中标记.
 * 输入参数: num 选中的子菜单项号
 * 输出参数: 无
 * 返 回 值: 无
 *
 * 调用说明:
 */
void TagSubMenu(int num)
{
    SMALL_RECT rcPop;
    COORD pos;
    WORD att;
    int width;

    LocSubMenu(gi_sel_menu, &rcPop);  /*计算弹出子菜单的区域位置, 存放在rcPop中*/
    if ((num<1) || (num == gi_sel_sub_menu) || (num>rcPop.Bottom-rcPop.Top-1))
    {   /*如果子菜单项号越界，或该项子菜单已被选中，则返回*/
        return;
    }

    pos.X = rcPop.Left + 2;
    width = rcPop.Right - rcPop.Left - 3;
    if (gi_sel_sub_menu != 0) /*首先取消原选中子菜单项上的标记*/
    {
        pos.Y = rcPop.Top + gi_sel_sub_menu;
        att = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED;  /*白底黑字*/
        FillConsoleOutputAttribute(gh_std_out, att, width, pos, &ul);
        pos.X += 1;
        att |=  FOREGROUND_RED;/*白底红字*/
        FillConsoleOutputAttribute(gh_std_out, att, 1, pos, &ul);
    }
    /*在制定子菜单项上做选中标记*/
    pos.X = rcPop.Left + 2;
    pos.Y = rcPop.Top + num;
    att = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED;  /*黑底白字*/
    FillConsoleOutputAttribute(gh_std_out, att, width, pos, &ul);
    gi_sel_sub_menu = num;  /*修改选中子菜单项号*/
    return;
}

/**
 * 函数名称: LocSubMenu
 * 函数功能: 计算弹出子菜单区域左上角和右下角的位置.
 * 输入参数: num 选中的主菜单项号
 * 输出参数: rc 存放区域位置信息的地址
 * 返 回 值: 无
 *
 * 调用说明:
 */
void LocSubMenu(int num, SMALL_RECT *rc)
{
    int i, len, loc = 0;

    rc->Top = 1; /*区域的上边定在第2行，行号为1*/
    rc->Left = 1;
    for (i=1; i<num; i++)
    {   /*计算区域左边界位置, 同时计算第一个子菜单项在子菜单字符串数组中的位置*/
        rc->Left += strlen(ga_main_menu[i-1]) + 4;
        loc += ga_sub_menu_count[i-1];
    }
    rc->Right = strlen(ga_sub_menu[loc]);/*暂时存放第一个子菜单项字符串长度*/
    for (i=1; i<ga_sub_menu_count[num-1]; i++)
    {   /*查找最长子菜单字符串，将其长度存放在rc->Right*/
        len = strlen(ga_sub_menu[loc+i]);
        if (rc->Right < len)
        {
            rc->Right = len;
        }
    }
    rc->Right += rc->Left + 3;  /*计算区域的右边界*/
    rc->Bottom = rc->Top + ga_sub_menu_count[num-1] + 1;/*计算区域下边的行号*/
    if (rc->Right >= SCR_COL)  /*右边界越界的处理*/
    {
        len = rc->Right - SCR_COL + 1;
        rc->Left -= len;
        rc->Right = SCR_COL - 1;
    }
    return;
}

/**
 * 函数名称: DealInput
 * 函数功能: 在弹出窗口区域设置热区, 等待并相应用户输入.
 * 输入参数: pHotArea
 *           piHot 焦点热区编号的存放地址, 即指向焦点热区编号的指针
 * 输出参数: piHot 用鼠标单击、按回车或空格时返回当前热区编号
 * 返 回 值:
 *
 * 调用说明:
 */
int DealInput(HOT_AREA *pHotArea, int *piHot)
{
    INPUT_RECORD inRec;
    DWORD res;
    COORD pos = {0, 0};
    int num, arrow, iRet = 0;
    int cNo, cTag, cSort;/*cNo:层号, cTag:热区编号, cSort: 热区类型*/
    char vkc, asc;       /*vkc:虚拟键代码, asc:字符的ASCII码值*/

    SetHotPoint(pHotArea, *piHot);
    while (TRUE)
    {    /*循环*/
        ReadConsoleInput(gh_std_in, &inRec, 1, &res);
        if ((inRec.EventType == MOUSE_EVENT) &&
            (inRec.Event.MouseEvent.dwButtonState
             == FROM_LEFT_1ST_BUTTON_PRESSED))
        {
            pos = inRec.Event.MouseEvent.dwMousePosition;
            cNo = gp_scr_att[pos.Y * SCR_COL + pos.X] & 3;
            cTag = (gp_scr_att[pos.Y * SCR_COL + pos.X] >> 2) & 15;
            cSort = (gp_scr_att[pos.Y * SCR_COL + pos.X] >> 6) & 3;

            if ((cNo == gp_top_layer->LayerNo) && cTag > 0)
            {
                *piHot = cTag;
                SetHotPoint(pHotArea, *piHot);
                if (cSort == 0)
                {
                    iRet = 13;
                    break;
                }
            }
        }
        else if (inRec.EventType == KEY_EVENT && inRec.Event.KeyEvent.bKeyDown)
        {
            vkc = inRec.Event.KeyEvent.wVirtualKeyCode;
            asc = inRec.Event.KeyEvent.uChar.AsciiChar;;
            if (asc == 0)
            {
                arrow = 0;
                switch (vkc)
                {  /*方向键(左、上、右、下)的处理*/
                    case 37: arrow = 1; break;
                    case 38: arrow = 2; break;
                    case 39: arrow = 3; break;
                    case 40: arrow = 4; break;
                }
                if (arrow > 0)
                {
                    num = *piHot;
                    while (TRUE)
                    {
                        if (arrow < 3)
                        {
                            num--;
                        }
                        else
                        {
                            num++;
                        }
                        if ((num < 1) || (num > pHotArea->num) ||
                            ((arrow % 2) && (pHotArea->pArea[num-1].Top
                            == pHotArea->pArea[*piHot-1].Top)) || ((!(arrow % 2))
                            && (pHotArea->pArea[num-1].Top
                            != pHotArea->pArea[*piHot-1].Top)))
                        {
                            break;
                        }
                    }
                    if (num > 0 && num <= pHotArea->num)
                    {
                        *piHot = num;
                        SetHotPoint(pHotArea, *piHot);
                    }
                }
            }
            else if (vkc == 27)
            {  /*ESC键*/
                iRet = 27;
                break;
            }
            else if (vkc == 13 || vkc == 32)
            {  /*回车键或空格表示按下当前按钮*/
                iRet = 13;
                break;
            }
        }
    }
    return iRet;
}

void SetHotPoint(HOT_AREA *pHotArea, int iHot)
{
    CONSOLE_CURSOR_INFO lpCur;
    COORD pos = {0, 0};
    WORD att1, att2;
    int i, width;

    att1 = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED;  /*黑底白字*/
    att2 = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED;  /*白底黑字*/
    for (i=0; i<pHotArea->num; i++)
    {  /*将按钮类热区置为白底黑字*/
        pos.X = pHotArea->pArea[i].Left;
        pos.Y = pHotArea->pArea[i].Top;
        width = pHotArea->pArea[i].Right - pHotArea->pArea[i].Left + 1;
        if (pHotArea->pSort[i] == 0)
        {  /*热区是按钮类*/
            FillConsoleOutputAttribute(gh_std_out, att2, width, pos, &ul);
        }
    }

    pos.X = pHotArea->pArea[iHot-1].Left;
    pos.Y = pHotArea->pArea[iHot-1].Top;
    width = pHotArea->pArea[iHot-1].Right - pHotArea->pArea[iHot-1].Left + 1;
    if (pHotArea->pSort[iHot-1] == 0)
    {  /*被激活热区是按钮类*/
        FillConsoleOutputAttribute(gh_std_out, att1, width, pos, &ul);
    }
    else if (pHotArea->pSort[iHot-1] == 1)
    {  /*被激活热区是文本框类*/
        SetConsoleCursorPosition(gh_std_out, pos);
        GetConsoleCursorInfo(gh_std_out, &lpCur);
        lpCur.bVisible = TRUE;
        SetConsoleCursorInfo(gh_std_out, &lpCur);
    }
}

/**
 * 函数名称: ExeFunction
 * 函数功能: 执行由主菜单号和子菜单号确定的功能函数.
 * 输入参数: m 主菜单项号
 *           s 子菜单项号
 * 输出参数: 无
 * 返 回 值: BOOL类型, TRUE 或 FALSE
 *
 * 调用说明: 仅在执行函数ExitSys时, 才可能返回FALSE, 其他情况下总是返回TRUE
 */
BOOL ExeFunction(int m, int s)
{
    BOOL bRet = TRUE;
    /*函数指针数组，用来存放所有功能函数的入口地址*/
    BOOL (*pFunction[ga_sub_menu_count[0]+ga_sub_menu_count[1]+ga_sub_menu_count[2]+ga_sub_menu_count[3]+ga_sub_menu_count[4]])(void);
    int i, loc;

    /*将功能函数入口地址存入与功能函数所在主菜单号和子菜单号对应下标的数组元素*/
    pFunction[0] = SaveData;
    pFunction[1] = BackupData;
    pFunction[2] = NULL;
    pFunction[3] = ExitSys;

    pFunction[4] = MaintainAssort;
    pFunction[5] = MaintainEvent;
    pFunction[6] = MaintainDonate;

    pFunction[7] = ShowAll;
    pFunction[8] = QueryAssort;
    pFunction[9] = QueryEvent;
    pFunction[10] = QueryDonate;

    pFunction[11] = StatType;
    pFunction[12] = StatName;
    pFunction[13] = StatYear;
    pFunction[14] = StatDate;
    pFunction[15] = StatRank;

    pFunction[16] = HelpTopic;
    pFunction[17] = NULL;
    pFunction[18] = About;


    for (i=1,loc=0; i<m; i++)  /*根据主菜单号和子菜单号计算对应下标*/
    {
        loc += ga_sub_menu_count[i-1];
    }
    loc += s - 1;

    if (pFunction[loc] != NULL)
    {
        bRet = (*pFunction[loc])();  /*用函数指针调用所指向的功能函数*/
    }

    return bRet;
}

/**
*函数名称：ShowModule
*函数功能：显示确认菜单
*输入参数：char **pString, int n
*输出参数：无
*返回值：BOOL类型，功能函数中除了ExitSys的返回值可以为FALSE外，其他函数的返回值必须为TRUE
*调用说明：
*/
BOOL ShowModule(char **pString, int n)
{
    LABEL_BUNDLE labels;
    HOT_AREA areas;
    BOOL bRet = TRUE;
    SMALL_RECT rcPop;
    COORD pos;
    WORD att;
    int iHot = 1;
    int i, maxlen, str_len;

    for (i=0,maxlen=0; i<n; i++) {
        str_len = strlen(pString[i]);
        if (maxlen < str_len) {
            maxlen = str_len;
        }
    }

    pos.X = maxlen + 6;
    pos.Y = n + 5;
    rcPop.Left = (SCR_COL - pos.X) / 2;
    rcPop.Right = rcPop.Left + pos.X - 1;
    rcPop.Top = (SCR_ROW - pos.Y) / 2;
    rcPop.Bottom = rcPop.Top + pos.Y - 1;

    att = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED;  /*白底黑字*/
    labels.num = n;
    labels.ppLabel = pString;
    COORD aLoc[n];

    for (i=0; i<n; i++) {
        aLoc[i].X = rcPop.Left + 3;
        aLoc[i].Y = rcPop.Top + 2 + i;

    }
    str_len = strlen(pString[n-1]);
    aLoc[n-1].X = rcPop.Left + 3 + (maxlen-str_len)/2;
    aLoc[n-1].Y = aLoc[n-1].Y + 2;

    labels.pLoc = aLoc;

    areas.num = 1;
    SMALL_RECT aArea[] = {{aLoc[n-1].X, aLoc[n-1].Y,
                           aLoc[n-1].X + 3, aLoc[n-1].Y}};

    char aSort[] = {0};
    char aTag[] = {1};

    areas.pArea = aArea;
    areas.pSort = aSort;
    areas.pTag = aTag;
    PopUp(&rcPop, att, &labels, &areas);

    pos.X = rcPop.Left + 1;
    pos.Y = rcPop.Top + 2 + n;
    FillConsoleOutputCharacter(gh_std_out, '-', rcPop.Right-rcPop.Left-1, pos, &ul);

    DealInput(&areas, &iHot);
    PopOff();

    return bRet;

}

/**
 函数名称：SaveData
 函数功能：保存系统代码表和三类基础数据
 输入参数：无
 返回值：BOOL类型，总是为TRUE
*/
BOOL SaveData(void)
{
    ASSORT *pAssort;
    EVENT *pEvent;
    DONATE *pDonate;
    FILE *pfout;
    pfout=fopen(gp_assort_info_filename,"wb");
    for(pAssort=gp_head;pAssort!=NULL;pAssort=pAssort->next)
    {
        fwrite(pAssort,sizeof(ASSORT),1,pfout);
    }
    fclose(pfout);
    pfout=fopen(gp_event_info_filename,"wb");
    for(pAssort=gp_head;pAssort!=NULL;pAssort=pAssort->next)
    {
        pEvent=pAssort->enext;
        while(pEvent!=NULL)
        {
            fwrite(pEvent,sizeof(EVENT),1,pfout);
            pEvent=pEvent->next;
        }
    }
    fclose(pfout);
    pfout=fopen(gp_donate_info_filename,"wb");
    for(pAssort=gp_head;pAssort!=NULL;pAssort=pAssort->next)
    {
        pEvent=pAssort->enext;
        while(pEvent!=NULL)
        {
            pDonate=pEvent->dnext;
            while(pDonate!=NULL)
            {
                fwrite(pDonate,sizeof(DONATE),1,pfout);
                pDonate=pDonate->next;
            }
            pEvent=pEvent->next;
        }
    }
    fclose(pfout);
    set_text_success();
    return TRUE;
}

/**
 函数名称：BackupData
 函数功能：将系统代码表和三类基础数据备份到一个数据文件
 输入参数：无
 返回值：BOOL类型，总是为TRUE
*/
BOOL BackupData(void)
{
    ASSORT *pAssort;
    EVENT *pEvent;
    DONATE *pDonate;
    unsigned long assort_num=0;
    unsigned long event_num=0;
    unsigned long donate_num=0;
    int handle;
    for(pAssort=gp_head;pAssort!=NULL;pAssort=pAssort->next)
    {
        assort_num++;
        pEvent=pAssort->enext;
        while(pEvent!=NULL)
        {
            event_num++;
            pDonate=pEvent->dnext;
            while(pDonate!=NULL)
            {
                donate_num++;
                pDonate=pDonate->next;
            }
            pEvent=pEvent->next;
        }
    }
    if((handle=open(gp_backup_filename,O_WRONLY|O_BINARY))==-1)
    {
        handle=open(gp_backup_filename,O_CREAT|O_BINARY,S_IWRITE);
    }
    write(handle,(char *)&assort_num,sizeof(assort_num));
    write(handle,(char *)&event_num,sizeof(event_num));
    write(handle,(char *)&donate_num,sizeof(donate_num));
    for(pAssort=gp_head;pAssort!=NULL;pAssort=pAssort->next)
    {
        write(handle,(char *)pAssort,sizeof(ASSORT));
    }
    for(pAssort=gp_head;pAssort!=NULL;pAssort=pAssort->next)
    {
        pEvent=pAssort->enext;
        while(pEvent!=NULL)
        {
            write(handle,(char *)pEvent,sizeof(EVENT));
            pEvent=pEvent->next;
        }
    }
    for(pAssort=gp_head;pAssort!=NULL;pAssort=pAssort->next)
    {
        pEvent=pAssort->enext;
        while(pEvent!=NULL)
        {
            pDonate=pEvent->dnext;
            while(pDonate!=NULL)
            {
                write(handle,(char *)pDonate,sizeof(DONATE));
                pDonate=pDonate->next;
            }
            pEvent=pEvent->next;
        }
    }
    close(handle);
    set_text_success();
    return TRUE;
}


/**
*函数名称：EXitSys
*函数功能：退出系统
*输入参数：无
*输出参数：无
*返回值：BOOL类型，功能函数中除了ExitSys的返回值可以为FALSE外，其他函数的返回值必须为TRUE
*调用说明：
*/
BOOL ExitSys(void)
{
    LABEL_BUNDLE labels;
    HOT_AREA areas;
    BOOL bRet = TRUE;
    SMALL_RECT rcPop;
    COORD pos;
    WORD att;
    char *pCh[] = {"确认退出系统吗？", "确定    取消"};
    int iHot = 1;

    pos.X = strlen(pCh[0]) + 6;
    pos.Y = 7;
    rcPop.Left = (SCR_COL - pos.X) / 2;
    rcPop.Right = rcPop.Left + pos.X - 1;
    rcPop.Top = (SCR_ROW - pos.Y) / 2;
    rcPop.Bottom = rcPop.Top + pos.Y - 1;

    att = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED;  /*白底黑字*/
    labels.num = 2;
    labels.ppLabel = pCh;
    COORD aLoc[] = {{rcPop.Left+3, rcPop.Top+2},
                    {rcPop.Left+5, rcPop.Top+5}};
    labels.pLoc = aLoc;

    areas.num = 2;
    SMALL_RECT aArea[] = {{rcPop.Left + 5, rcPop.Top + 5,
                           rcPop.Left + 8, rcPop.Top + 5},
                          {rcPop.Left + 13, rcPop.Top + 5,
                           rcPop.Left + 16, rcPop.Top + 5}};
    char aSort[] = {0, 0};
    char aTag[] = {1, 2};
    areas.pArea = aArea;
    areas.pSort = aSort;
    areas.pTag = aTag;
    PopUp(&rcPop, att, &labels, &areas);

    pos.X = rcPop.Left + 1;
    pos.Y = rcPop.Top + 4;
    FillConsoleOutputCharacter(gh_std_out, '-', rcPop.Right-rcPop.Left-1, pos, &ul);

    if (DealInput(&areas, &iHot) == 13 && iHot == 1)
    {
        bRet = FALSE;
    }
    else
    {
        bRet = TRUE;
    }
    PopOff();

    return bRet;
}

/**
 函数名称：InsertAssort
 函数功能：在十字链表中插入一个自然灾害分类信息结点
 输入参数：phd主链头指针，pAssort指向所要插入结点的指针
 返回值：BOOL类型，TRUE表示插入成功,FALSE表示插入失败
*/
BOOL InsertAssort(ASSORT **phd, ASSORT *pAssort)
{
    ASSORT *passort=*phd;
    while(passort!=NULL&&strcmp(passort->name,pAssort->name))
    {
        passort=passort->next;
    }
    if(passort==NULL)
    {
        pAssort->enext=NULL;
        pAssort->next=*phd;
        *phd=pAssort;
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

/**
 函数名称：InsertEvent
 函数功能：在十字链表中插入一个自然灾害分类信息结点
 输入参数：phd主链头指针，pEvent指向所要插入结点的指针
 返回值：BOOL类型，TRUE表示插入成功,FALSE表示插入失败
*/
BOOL InsertEvent(ASSORT *phd,EVENT *pEvent)
{
    ASSORT *pAssort;
    EVENT *pevent;
    pAssort=SeekAssort1(phd,pEvent->name);
    if(pAssort!=NULL)
    {
        pevent=pAssort->enext;
        while(pevent!=NULL&&strcmp(pevent->name,pEvent->name))
        {
            pevent=pevent->next;
        }
        if(pevent==NULL)
        {
            pEvent->next=pAssort->enext;
            pAssort->enext=pEvent;
            return TRUE;
        }
    }
    return FALSE;
}

/**
 函数名称：InsertDonate
 函数功能：在十字链表中插入一个房屋信息结点
 输入参数：phd主链头指针，pDonate指向所要插入结点的指针
 返回值：BOOL类型，TRUE表示插入成功,FALSE表示插入失败
*/
BOOL InsertDonate(ASSORT *phd,DONATE *pDonate)
{
    EVENT *pevent;
    pevent=SeekEvent(phd,pDonate->num);
    if(pevent!=NULL)
    {
        pDonate->next=pevent->dnext;
        pevent->dnext=pDonate;
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}


/**
*函数名称：HelpTopic
*函数功能：帮助主体
*输入参数：无
*输出参数：无
*返回值：BOOL类型，功能函数中除了ExitSys的返回值可以为FALSE外，其他函数的返回值必须为TRUE
*调用说明：
*/
BOOL HelpTopic(void)
{
    InitInterface();
    printf("\n\n\n\n\n\n\n                       如果遇到点不动的情况，试着按一下回车键\n");
    printf("                       自然灾害捐款单位为万元人民币\n");
    printf("                       直接经济损失为亿元人民币\n");
    printf("                       '_____'的上方为自然灾害分类信息\n");
    printf("                       '====='的上方为自然灾害事件信息\n");
    printf("                       '*****'的上方为自然灾害捐款信息\n");
    getchar();
    InitInterface();
    return TRUE;
}

/**
*函数名称：About
*函数功能：关于
*输入参数：无
*输出参数：无
*返回值：BOOL类型，功能函数中除了ExitSys的返回值可以为FALSE外，其他函数的返回值必须为TRUE
*调用说明：
*/
BOOL About(void)
{
    ClearScreen();
    COORD pos = {30,6};
    SetConsoleCursorPosition(gh_std_out, pos);
    printf("制作者:CS1508李泽民");
    COORD pos1 = {34,11};
    SetConsoleCursorPosition(gh_std_out, pos1);
    printf("按任意键返回\n");
    getch();
    ClearScreen();
    InitInterface();
    return TRUE;
}

/**
*函数名称：ShowAll
*函数功能：显示全部信息
*输入参数：无
*输出参数：无
*/
BOOL ShowAll(void)
{
    ASSORT *pAssort;
    EVENT *pEvent;
    DONATE *pDonate;
    CONSOLE_CURSOR_INFO lp,*plp = &lp;
    lp.bVisible = TRUE;
    lp.dwSize = 1;
    SetConsoleCursorPosition(gh_std_out,(COORD)
    {
        1, 1
    });
    SetConsoleCursorInfo(gh_std_out,plp);
    for(pAssort=gp_head;pAssort!=NULL;pAssort=pAssort->next)
    {
        printf("灾害名称：%s\t",pAssort->name);
        printf("灾害类别：%c\n",pAssort->type);
        printf("____________________________________________________________\n");
        for(pEvent=pAssort->enext;pEvent!=NULL;pEvent=pEvent->next)
        {
            printf("自然灾害类别：%c\t",pEvent->type);
            printf("事件编号：%s\t",pEvent->num);
            printf("发生时间：%s\t",pEvent->time);
            printf("灾害名称：%s\n",pEvent->name);
            printf("灾害级别：%c\t",pEvent->level);
            printf("发生地点：%s\t",pEvent->place);
            printf("死亡人数：%d人\t",pEvent->dead);
            printf("民房倒塌：%d\n间",pEvent->collapse);
            printf("直接经济损失：%f亿元人民币\n",pEvent->loss);
            printf("===============================================================\n");
            for(pDonate=pEvent->dnext;pDonate!=NULL;pDonate=pDonate->next)
            {
                printf("事件编号：%s\t",pDonate->num);
                printf("捐款日期：%s\n",pDonate->time);
                printf("捐款类别：%c\t",pDonate->type);
                printf("捐款者名称：%s\t",pDonate->dname);
                printf("捐款数目：%f亿万元\n",pDonate->money);
                printf("***********************************************************************\n");
            }
        }
    }
    printf("\n");
    getchar();
    getchar();
    InitInterface();
}
/**
 函数名称：MatchString
 函数功能：对给定的字符串按条件进行匹配
 输入参数：string_item给定字符串，pcond包含匹配运算符在内的条件字符串
 返回值：匹配成功时，返回TRUE,否则返回FALSE
*/
BOOL MatchString(char *string_item,char *pcond)
{
    char op;
    int compare_result;
    char *sub_string_pos;
    BOOL bRet=FALSE;
    compare_result=strcmp(string_item,pcond+1);
    sub_string_pos=strstr(string_item,pcond+1);
    op=*pcond;
    switch(op)
    {
    case '=':
        if(compare_result==0)
        {
            bRet=TRUE;
        }
        else{
            bRet=FALSE;
        }
        break;
    case '<':
        if(compare_result<0)
        {
            bRet=TRUE;
        }
        else{
            bRet=FALSE;
        }
        break;
    case '>':
        if(compare_result>0)
        {
            bRet=TRUE;
        }
        else{
            bRet=FALSE;
        }
        break;
    case '!':
        if(compare_result!=0)
        {
            bRet=TRUE;
        }
        else{
            bRet=FALSE;
        }
        break;
    case '@':
        if(sub_string_pos!=NULL)
        {
            bRet=TRUE;
        }
        else{
            bRet=FALSE;
        }
        break;
    case '?':
        if(sub_string_pos==NULL)
        {
            bRet=TRUE;
        }
        else{
            bRet=FALSE;
        }
        break;
    }
    return bRet;
}


/**
 函数名称：MatchChar
 函数功能：对给定字符按条件进行匹配
 输入参数：char_item给定字符，pcond包含匹配运算符在内的条件字符串
 返回值：匹配成功时，返回TRUE，否则返回FALSE
*/
BOOL MatchChar(char char_item,char *pcond)
{
    char op;
    int compare_result;
    BOOL bRet=FALSE;
    compare_result=char_item-*(pcond+1);
    op=*pcond;
    switch(op)
    {
    case '=':
        if(compare_result==0)
        {
            bRet=TRUE;
        }
        else{
            bRet=FALSE;
        }
        break;
    case '<':
        if(compare_result<0)
        {
            bRet=TRUE;
        }
        else{
            bRet=FALSE;
        }
        break;
    case '>':
        if(compare_result>0)
        {
            bRet=TRUE;
        }
        else{
            bRet=FALSE;
        }
        break;
    case '!':
        if(compare_result!=0)
        {
            bRet=TRUE;
        }
        else{
            bRet=FALSE;
        }
        break;
    }
    return bRet;
}


/**
 函数名称：DelAssort
 函数功能：从十字链表中删除指定的分类信息结点
 输入参数：主链头指针phd,灾害名称name
 返回值：BOOL类型，TRUE表示成功，FALSE表示失败
 调用说明：根据灾害名称确定唯一的灾害分类信息
*/
BOOL DelAssort(ASSORT **phd,char type)
{
    BOOL bRet=FALSE;
    ASSORT *pAssort1=NULL;
    ASSORT *pAssort2=*phd;
    while(pAssort2!=NULL&&(pAssort2->type!=type))
    {
        pAssort1=pAssort2;
        pAssort2=pAssort2->next;
    }
    if(pAssort2!=NULL)
    {
        bRet=TRUE;
        if(pAssort1==NULL)
        {
            *phd=pAssort2->next;
        }
        else
        {
            pAssort1->next=pAssort2->next;
        }
        free(pAssort2);
    }
    return bRet;
}

/**
 函数名称：DelEvent
 函数功能：从十字链表中删除指定的事件信息结点
 输入参数：主链头指针phd,
 返回值：BOOL类型，TRUE表示成功，FALSE表示失败
 调用说明：根据灾害名称确定唯一的事件信息
*/
BOOL DelEvent(ASSORT *phd,char *num)
{
    ASSORT *pAssort=phd;
    EVENT *pEvent1=NULL;
    EVENT *pEvent2;
    DONATE *pDonate1,*pDonate2;
    BOOL bRet=FALSE;
    while(pAssort!=NULL)
    {
        pEvent2=pAssort->enext;
        pEvent1=NULL;
        while(pEvent2!=NULL&&strcmp(pEvent2->num,num))
        {
            pEvent1=pEvent2;
            pEvent2=pEvent2->next;
        }
        if(pEvent2!=NULL)
        {
            bRet=TRUE;
            break;
        }
        pAssort=pAssort->next;
    }
    if(pAssort!=NULL)
    {
        if(pEvent1==NULL)
        {
            pAssort->enext=pEvent2->next;
        }
        else
        {
            pEvent1->next=pEvent2->next;
        }
        pDonate2=pEvent2->dnext;
        free(pEvent2);
        while(pDonate2!=NULL)
        {
            pDonate1=pDonate2;
            pDonate2=pDonate2->next;
            free(pDonate1);
        }
    }
    return bRet;
}


/**
 函数名称：DelDonate
 函数功能：从十字链表中删除指定的灾害捐款信息结点
 输入参数：主链头指针hd，灾害名称name，捐款者名称dname
 返回值：BOOL类型，TRUE表示成功，FALSE表示失败
 调用说明：根据灾害名称和捐款者名称确定唯一的捐款信息
*/
BOOL DelDonate(ASSORT *phd,char *num,char *dname)
{
    EVENT *pEvent;
    DONATE *pDonate1;
    DONATE *pDonate2;
    BOOL bRet=FALSE;
    pEvent=SeekEvent(phd,num);
    if(pEvent!=NULL)
    {
        pDonate1=NULL;
        pDonate2=pEvent->dnext;
        while(pDonate2!=NULL&&strcmp(pDonate2->dname,dname)!=0)
        {
            pDonate1=pDonate2;
            pDonate2=pDonate2->next;
        }
        if(pDonate2!=NULL)
        {
            bRet=TRUE;
            if(pDonate1==NULL)
            {
                pEvent->dnext=pDonate2->next;
            }
            else
            {
                pDonate1->next=pDonate2->next;
            }
            free(pDonate2);
        }
    }
    return bRet;
}


/**
 函数名称：ModifAssort
 函数功能：对指定的分类信息结点内容信息进行修改
 输入参数：主链头指针phd，灾害类型type，pAssort指向存放修改内容结点的指针
 返回值：BOOL类型,TRUE表示修改成功，FALSE表示修改失败
*/
BOOL ModifAssort(ASSORT *phd,char type,ASSORT *pAssort)
{
    pAssort->enext=NULL;
    ASSORT *pAssort1;
    ASSORT *pAssort2;
    pAssort1=SeekAssort2(phd,type);
    if(pAssort1!=NULL)
    {
        pAssort2=pAssort1->next;
        *pAssort1=*pAssort;
        pAssort1->next=pAssort2;
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

/**
 函数名称：ModifEvent
 函数功能：对指定的灾害事件信息结点内容信息进行修改
 输入参数：主链头指针phd，事件编号，pEvent指向存放修改内容结点的指针
 返回值：BOOL类型,TRUE表示修改成功，FALSE表示修改失败
*/
BOOL ModifEvent(ASSORT *phd,char *num,EVENT *pEvent)
{
    EVENT *pEvent1;
    EVENT *pEvent2;
    pEvent1=SeekEvent(phd,num);
    if(pEvent1!=NULL)
    {
        pEvent2=pEvent1->next;
        *pEvent1=*pEvent;
        pEvent1->next=pEvent2;
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}


/**
 函数名称：ModifDonate
 函数功能：对指定的捐款信息结点内容信息进行修改
 输入参数：主链头指针phd，事件编号num，捐款人名称dname，pDonate指向存放修改内容结点的指针
 返回值：BOOL类型,TRUE表示修改成功，FALSE表示修改失败
*/
BOOL ModifDonate(ASSORT *phd,char *num,char *dname,DONATE *pDonate)
{
    DONATE *pDonate1;
    DONATE *pDonate2;
    pDonate1=SeekDonate(phd,num,dname);
    if(pDonate1!=NULL)
    {
        pDonate2=pDonate1->next;
        *pDonate1=*pDonate;
        pDonate1->next=pDonate2;
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

/**
 函数名称：SeekAssort1
 函数功能：按灾害类型查找灾害类型信息结点
 输入参数：主链头指针phd，灾害名称name
 返回值：找到时返回结点的地址，否则返回NULL
*/
ASSORT *SeekAssort1(ASSORT *phd,char *name)
{
    ASSORT *pAssort=phd;
    while(pAssort!=NULL&&(strstr(pAssort->name,name)==NULL))
    {
        pAssort=pAssort->next;
    }
    if(pAssort!=NULL)
    {
        return pAssort;
    }
    else
    {
        return NULL;
    }
}

/**
 函数名称：SeekAssort2
 函数功能：按灾害类型查找灾害类型信息结点
 输入参数：主链头指针phd，灾害类别type
 返回值：找到时返回结点的地址，否则返回NULL
*/
ASSORT *SeekAssort2(ASSORT *phd,char type)
{
    ASSORT *pAssort=phd;
    while(pAssort!=NULL&&(pAssort->type==type))
    {
        pAssort=pAssort->next;
    }
    if(pAssort!=NULL)
    {
        return pAssort;
    }
    else
    {
        return NULL;
    }
}

/**
 函数名称：SeekEvent
 函数功能：按灾害名称查找灾害基本事件信息结点
 输入参数：主链头指针phd，事件编号num
 返回值：找到时返回结点的地址，否则返回NULL
*/
EVENT *SeekEvent(ASSORT *phd,char *num)
{
    ASSORT *pAssort;
    EVENT *pEvent;
    int find=0;
    for(pAssort=phd;pAssort!=NULL;pAssort=pAssort->next)
    {
        pEvent=pAssort->enext;
        while(pEvent!=NULL)
        {
            if(strcmp(pEvent->num,num)==0)
            {
                find=1;
                break;
            }
            pEvent=pEvent->next;
        }
        if(find)
        {
            break;
        }
    }
    if(find)
    {
        return pEvent;
    }
    else
    {
        return NULL;
    }
}


/**
 函数名称：SeekEvent2
 函数功能：按灾害名称查找灾害基本事件信息结点
 输入参数：主链头指针phd，灾害名称name，灾害时间区间time1,time2
 返回值：找到时返回结点的地址，否则返回NULL
*/
EVENT *SeekEvent2(ASSORT *phd,char *time1,char *time2,char *name)
{
    ASSORT *pAssort;
    EVENT *pEvent;
    int find=0;
    for(pAssort=phd;pAssort!=NULL;pAssort=pAssort->next)
    {
        pEvent=pAssort->enext;
        while(pEvent!=NULL)
        {
            if(JudgeEventItem2(pEvent,time1,time2,name)==TRUE)
            {
                find=1;
                break;
            }
            pEvent=pEvent->next;
        }
        if(find)
        {
            break;
        }
    }
    if(find)
    {
        return pEvent;
    }
    else
    {
        return NULL;
    }
}

/**
 函数名称：SeekDonate
 函数功能：按事件编号和捐款人姓名子字符串查找捐款信息结点
 输入参数：主链头指针phd，事件编号num，捐款人姓名dname
 返回值：找到时返回结点的地址，否则返回NULL
*/
DONATE *SeekDonate(ASSORT *phd,char *num,char *dname)
{
    EVENT *pEvent;
    DONATE *pDonate;
    int find=0;
    pEvent=SeekEvent(phd,num);
    if(pEvent!=NULL)
    {
        pDonate=pEvent->dnext;
        while(pDonate!=NULL)
        {
            if(MatchString(pDonate->dname,dname)==TRUE)
            {
                find=1;
                break;
            }
            pDonate=pDonate->next;
        }
    }
    if(find)
    {
        return pDonate;
    }
    else
    {
        return NULL;
    }
}

/**
 函数名称：SeekDonate2
 函数功能：按捐款类别和捐款数目区间查找捐款信息结点
 输入参数：主链头指针phd，灾害名称name，捐款人姓名dname
 返回值：找到时返回结点的地址，否则返回NULL
*/
DONATE *SeekDonate2(ASSORT *phd,char type,float money1,float money2)
{
    ASSORT *pAssort;
    EVENT *pEvent;
    DONATE *pDonate;
    int find=0;
    for(pAssort=phd;pAssort!=NULL;pAssort=pAssort->next)
    {
        pEvent=pAssort->enext;
        while(pEvent!=NULL)
        {
            pDonate=pEvent->dnext;
            while(pDonate!=NULL)
            {
                if(pDonate->type==type)
                {
                    if((pDonate->money)>=money1&&(pDonate->money)<=money2)
                    {
                        find=1;
                        return pDonate;
                    }
                }
                pDonate=pDonate->next;
            }
            pEvent=pEvent->next;
        }
    }
    return NULL;
}


/**
 函数名称：JudgeAssortItem
 函数功能：判断自然灾害分类信息结点是否满足给定条件
 输入参数：pAssort分类信息结点指针，pcond用来表示条件的字符串
 返回值：满足条件时，返回TRUE，否则返回FALSE
*/
BOOL JudgeAssortItem(ASSORT *pAssort, char *pcond)
{
    int i;
    BOOL bRet=FALSE;
    i=(*pcond)-'0';
    switch(i)
    {
        case 1:bRet=MatchString(pAssort->name,pcond+1);break;
    }
    return bRet;
}


/**
 函数名称：JudgeEventItem1
 函数功能：判断自然灾害事件信息结点是否满足给定条件
 输入参数：pEvent事件信息结点指针，pcond用来表示条件的字符串
 返回值：满足条件时，返回TRUE，否则返回FALSE
*/
BOOL JudgeEventItem1(EVENT *pEvent,char *pcond)
{
    if(strcmp(pEvent->num,pcond)==0)
    {
        return TRUE;
    }
    else return FALSE;
}

/**
 函数名称：JudgeEventItem2
 函数功能：判断自然灾害事件信息结点是否满足给定条件
 输入参数：pEvent事件信息结点指针，pcond用来表示条件的字符串
 返回值：满足条件时，返回TRUE，否则返回FALSE
*/
BOOL JudgeEventItem2(EVENT *pEvent,char *pcond1,char *pcond2,char *pcond3)
{
    BOOL bRet=FALSE;
    int time1,time2,Time,it;
    char str1[8],str2[8],Str[8];
    for(it=0;it<8;it++)
    {
        str1[it]=pcond1[it];
        str2[it]=pcond2[it];
        Str[it]=(pEvent->num)[it];
    }
    time1=atoi(str1);
    time2=atoi(str2);
    Time=atoi(Str);
    if((Str>=time1&&Str<=time2)&&(strcmp(pEvent->name,pcond3)==0))
    {
        bRet=TRUE;
    }
    else
    {
        bRet=FALSE;
    }
    return bRet;
}

/**
 函数名称：JudgeDonateItem1
 函数功能：判断自然灾害事件信息结点是否满足给定条件
 输入参数：pEvent捐款信息结点指针，pcond用来表示条件的字符串
 返回值：满足条件时，返回TRUE，否则返回FALSE
*/
BOOL JudgeDonateItem1(DONATE *pDonate,char *pcond1,char *pcond2)
{
    BOOL bRet=FALSE;
    int it;
    if(strcmp(pDonate->num,pcond1)==0)
    {
        switch(it)
        {
            case 1:bRet=MatchString(pDonate->dname,pcond2+1);break;
        }
    }
    return bRet;
}

/**
 函数名称：JudgeDonateItem2
 函数功能：判断自然灾害事件信息结点是否满足给定条件
 输入参数：pEvent捐款信息结点指针，pcond用来表示条件的字符串
 返回值：满足条件时，返回TRUE，否则返回FALSE
*/
BOOL JudgeDonateItem2(DONATE *pDonate,char pcond,float money1,float money2)
{
    BOOL bRet=FALSE;
    if(pDonate->type==pcond)
    {
        if(((pDonate->money)>=money1)&&((pDonate->money)<=money2))
        {
            bRet=TRUE;
        }
    }
    return bRet;
}
/**
*函数名称：MaintainAssort
*函数功能：维护自然灾害分类信息
*输入参数：无
*输出参数：无
*返回值：TRUE
*调用说明：
*/
BOOL MaintainAssort(void)
{
    BOOL bRet = TRUE;
    LABEL_BUNDLE labels;
    HOT_AREA areas;
    SMALL_RECT rcPop;
    COORD pos;
    WORD att;
    char *pCh[] = {"1、删除分类信息", "2、插入分类信息","3、修改分类信息","返回"};
    char *pCh2[] = {"请输入：   灾害类别","开始输入","取消"};
    char *pCh3[] = {"请输入：灾害类别和名称","开始输入","取消"};
    char *pCh4[] = {"请输入：  原灾害类别     ","请输入： ","开始输入","取消"};
    int iHot = 4;/*热键所在行数*/
    pos.X = strlen(pCh[0]) +12;
    pos.Y = 14;
    rcPop.Left = (SCR_COL - pos.X) / 2;
    rcPop.Right = rcPop.Left + pos.X - 1;
    rcPop.Top = (25 - pos.Y) / 2;
    rcPop.Bottom = rcPop.Top + pos.Y - 1;
    att = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED;  /*白底黑字*/
    labels.num = 4;//行数
    labels.ppLabel = pCh;
    COORD aLoc[] = {{rcPop.Left+6, rcPop.Top+2},
        {rcPop.Left+6, rcPop.Top+5},
        {rcPop.Left+6, rcPop.Top+8},
        {rcPop.Left+10, rcPop.Top+12}
    };
    labels.pLoc = aLoc;

    areas.num = 4;
    SMALL_RECT aArea[] =
    {
        {
            rcPop.Left+6, rcPop.Top+2,
            rcPop.Left+22, rcPop.Top+2
        },
        {
            rcPop.Left + 6, rcPop.Top + 5,
            rcPop.Left +22, rcPop.Top +5
        },
        {
            rcPop.Left + 6, rcPop.Top + 8,
            rcPop.Left +22, rcPop.Top + 8
        },
        {
            rcPop.Left + 10, rcPop.Top + 12,
            rcPop.Left + 13, rcPop.Top + 12
        }
    };
    char aSort[] = {0, 0,0,0};
    char aTag[] = {1, 2,3,4};
    areas.pArea = aArea;
    areas.pSort = aSort;
    areas.pTag = aTag;
    PopUp(&rcPop, att, &labels, &areas);//形成弹窗



    if (DealInput(&areas, &iHot) == 13 && iHot == 1)
    {
        /*删除某分类信息*/
        PopOff();
        iHot = 2;
        pos.X = 30;
        pos.Y = 14;
        rcPop.Left = 0;
        rcPop.Right = SCR_COL-1;
        rcPop.Top = (25 - pos.Y) / 2;
        rcPop.Bottom = rcPop.Top + pos.Y - 1;
        att = BACKGROUND_BLUE| BACKGROUND_GREEN | BACKGROUND_RED;  /*白底黑字*/
        labels.num = 3;
        labels.ppLabel = pCh2;
        COORD aLoc2[] =
        {
            {rcPop.Left+0, rcPop.Top+1},
            {rcPop.Left+28, rcPop.Top+13},
            {rcPop.Left+46, rcPop.Top+13}
        };
        labels.pLoc = aLoc2;

        areas.num = 2;
        SMALL_RECT aArea2[] =
        {
            {
                rcPop.Left+26, rcPop.Top+13,
                rcPop.Left+36, rcPop.Top+13
            },
            {
                rcPop.Left + 46, rcPop.Top + 13,
                rcPop.Left +49, rcPop.Top +13
            }
        };
        char aSort2[] = {0, 0};
        char aTag2[] = {1, 2};
        areas.pArea = aArea2;
        areas.pSort = aSort2;
        areas.pTag = aTag2;
        PopUp(&rcPop, att, &labels, &areas);

    if(DealInput(&areas, &iHot) == 13 && iHot == 1)
        {
            /*开始执行查找和删除操作*/
            CONSOLE_CURSOR_INFO lp,*plp = &lp;
            lp.dwSize = 1;
            lp.bVisible = TRUE;
            SetConsoleCursorInfo(gh_std_out,plp);
            SetConsoleCursorPosition(gh_std_out,(COORD)
            {
                rcPop.Left+0, rcPop.Top+3
            });
            char type;
            scanf("%c",&type);

            lp.bVisible = FALSE;
            SetConsoleCursorInfo(gh_std_out,plp);
            PopOff();
            if(DelAssort(&gp_head,type) != 0)
            {
                set_text_success();
            }
            else
            {
                set_text_fail();
            }

        }
        else
        {
            PopOff();
        }
    }
  else
    {
        if(iHot == 2)
        {
            /*插入分类信息*/
            PopOff();
            iHot = 2;
            pos.X = 30;
            pos.Y = 14;
            rcPop.Left = 0;
            rcPop.Right = SCR_COL-1;
            rcPop.Top = (25 - pos.Y) / 2;
            rcPop.Bottom = rcPop.Top + pos.Y - 1;
            att = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED;  /*白底黑字*/
            labels.num = 3;
            labels.ppLabel = pCh3;
            COORD aLoc3[] =
            {
                {rcPop.Left+0, rcPop.Top+1},
                {rcPop.Left+28, rcPop.Top+13},
                {rcPop.Left+46, rcPop.Top+13}
            };
            labels.pLoc = aLoc3;

            areas.num = 2;
            SMALL_RECT aArea3[] =
            {
                {
                    rcPop.Left+26, rcPop.Top+13,
                    rcPop.Left+36, rcPop.Top+13
                },
                {
                    rcPop.Left + 46, rcPop.Top + 13,
                    rcPop.Left +49, rcPop.Top +13
                }
            };
            char aSort2[] = {0, 0};
            char aTag2[] = {1, 2};
            areas.pArea = aArea3;
            areas.pSort = aSort2;
            areas.pTag = aTag2;
            PopUp(&rcPop, att, &labels, &areas);
            if(DealInput(&areas, &iHot) == 13 && iHot == 1)
            {
                /*开始执行插入操作*/
                CONSOLE_CURSOR_INFO lp,*plp = &lp;
                lp.dwSize = 1;
                lp.bVisible = TRUE;
                SetConsoleCursorInfo(gh_std_out,plp);
                SetConsoleCursorPosition(gh_std_out,(COORD)
                {
                    rcPop.Left+0, rcPop.Top+3
                });
                ASSORT *pAssort1=(ASSORT *)malloc(sizeof(ASSORT));
                scanf("%c %s",&(pAssort1->type),pAssort1->name);
                lp.bVisible = FALSE;
                SetConsoleCursorInfo(gh_std_out,plp);
                PopOff();
                if(InsertAssort(&gp_head,pAssort1))
                {
                          set_text_success();
                }
                else
                {
                    free(pAssort1);
                    set_text_fail();
                }
            }
            else
            {
                PopOff();
            }
        }
        else
        {
            if(iHot == 3)
            {
                /*修改某分类信息*/
                PopOff();
                iHot = 2;

                pos.X = 30;
                pos.Y = 14;
                rcPop.Left = 0;
                rcPop.Right = SCR_COL-1;
                rcPop.Top = (25 - pos.Y) / 2;
                rcPop.Bottom = rcPop.Top + pos.Y - 1;

                att = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED;  /*白底黑字*/
                labels.num = 5;
                labels.ppLabel = pCh4;
                COORD aLoc3[] =
                {
                    {rcPop.Left+0, rcPop.Top+1},
                    {rcPop.Left+0, rcPop.Top+3},
                    {rcPop.Left+28, rcPop.Top+13},
                    {rcPop.Left+46, rcPop.Top+13}
                };
                labels.pLoc = aLoc3;

                areas.num = 2;
                SMALL_RECT aArea3[] =
                {
                    {
                        rcPop.Left+26, rcPop.Top+13,
                        rcPop.Left+36, rcPop.Top+13
                    },
                    {
                        rcPop.Left + 46, rcPop.Top + 13,
                        rcPop.Left +49, rcPop.Top +13
                    }
                };
                char aSort2[] = {0, 0};
                char aTag2[] = {1, 2};
                areas.pArea = aArea3;
                areas.pSort = aSort2;
                areas.pTag = aTag2;
                PopUp(&rcPop, att, &labels, &areas);

                if(DealInput(&areas, &iHot) == 13 && iHot == 1)
                {
                    /*开始执行查找和修改操作*/
                    CONSOLE_CURSOR_INFO lp,*plp = &lp;
                    lp.dwSize = 1;
                    lp.bVisible = TRUE;
                    SetConsoleCursorInfo(gh_std_out,plp);
                    SetConsoleCursorPosition(gh_std_out,(COORD)
                    {
                        rcPop.Left+0, rcPop.Top+4
                    });
                    ASSORT *pAssort2=(ASSORT *)malloc(sizeof(ASSORT));
                    char type;
                    scanf("%c %c %s",&type,&(pAssort2->type),pAssort2->name);

                    lp.bVisible = FALSE;
                    SetConsoleCursorInfo(gh_std_out,plp);
                    PopOff();
                    if(ModifAssort(gp_head,type,pAssort2) != 0)
                    {
                        set_text_success();
                    }
                    else
                    {
                        free(pAssort2);
                        set_text_fail();
                    }
                }
                else
                {
                    PopOff();
                }
            }
            else
            {
                PopOff();
            }
        }
    }
    PopOff();
    getchar();
    InitInterface();
    return bRet;
}



/**
*函数名称：MaintainEvent
*函数功能：维护自然灾害事件基本信息
*输入参数：无
*输出参数：无
*返回值：TRUE
*调用说明：
*/
BOOL MaintainEvent(void)
{
    BOOL bRet = TRUE;
    LABEL_BUNDLE labels;
    HOT_AREA areas;
    SMALL_RECT rcPop;
    COORD pos;
    WORD att;
    char *pCh[] = {"1、删除灾害事件信息", "2、插入灾害事件信息","3、修改灾害事件信息","返回"};
    char *pCh2[] = {"请输入：  事件编号","开始输入","取消"};
    char *pCh3[] = {"请输入事件信息 ：类别 编号 时间 名称 级别 地点 死亡人数 民房倒塌 经济损失                  ","                                                ","开始输入","取消"};
    char *pCh4[] = {"请输入：  原事件编号","请输入更改信息：类别 编号 时间 名称 级别 地点 死亡人数 民屋倒塌 经济损失                                 ","                                                ","开始输入","取消"};
    int iHot = 4;

    pos.X = strlen(pCh[0]) +11;
    pos.Y = 14;
    rcPop.Left = (SCR_COL - pos.X) / 2;
    rcPop.Right = rcPop.Left + pos.X - 1;
    rcPop.Top = (25 - pos.Y) / 2;
    rcPop.Bottom = rcPop.Top + pos.Y - 1;

    att = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED;  /*白底黑字*/
    labels.num = 4;
    labels.ppLabel = pCh;
    COORD aLoc[] = {{rcPop.Left+6, rcPop.Top+2},
        {rcPop.Left+6, rcPop.Top+5},
        {rcPop.Left+6, rcPop.Top+8},
        {rcPop.Left+10, rcPop.Top+12}
    };
    labels.pLoc = aLoc;

    areas.num = 4;
    SMALL_RECT aArea[] =
    {
        {
            rcPop.Left+6, rcPop.Top+2,
            rcPop.Left+22, rcPop.Top+2
        },
        {
            rcPop.Left + 6, rcPop.Top + 5,
            rcPop.Left +22, rcPop.Top +5
        },
        {
            rcPop.Left + 6, rcPop.Top + 8,
            rcPop.Left +22, rcPop.Top + 8
        },
        {
            rcPop.Left + 10, rcPop.Top + 12,
            rcPop.Left +13, rcPop.Top + 12
        }
    };
    char aSort[] = {0, 0,0,0};
    char aTag[] = {1, 2,3,4};
    areas.pArea = aArea;
    areas.pSort = aSort;
    areas.pTag = aTag;
    PopUp(&rcPop, att, &labels, &areas);
    if (DealInput(&areas, &iHot) == 13 && iHot == 1)
    {
        /*删除灾害事件信息*/
        PopOff();
        iHot = 2;

        pos.X = 30;
        pos.Y = 14;
        rcPop.Left = 0;
        rcPop.Right = SCR_COL-1 ;
        rcPop.Top = (25 - pos.Y) / 2;
        rcPop.Bottom = rcPop.Top + pos.Y - 1;

        att = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED;  /*白底黑字*/
        labels.num = 3;
        labels.ppLabel = pCh2;
        COORD aLoc2[] =
        {
            {rcPop.Left+0, rcPop.Top+1},
            {rcPop.Left+26, rcPop.Top+13},
            {rcPop.Left+46, rcPop.Top+13}
        };
        labels.pLoc = aLoc2;

        areas.num = 2;
        SMALL_RECT aArea2[] =
        {
            {
                rcPop.Left+26, rcPop.Top+13,
                rcPop.Left+36, rcPop.Top+13
            },
            {
                rcPop.Left + 46, rcPop.Top + 13,
                rcPop.Left +49, rcPop.Top +13
            }
        };
        char aSort2[] = {0, 0};
        char aTag2[] = {1, 2};
        areas.pArea = aArea2;
        areas.pSort = aSort2;
        areas.pTag = aTag2;
        PopUp(&rcPop, att, &labels, &areas);

        if(DealInput(&areas, &iHot) == 13 && iHot == 1)
        {
            /*开始执行查找和删除操作*/
            CONSOLE_CURSOR_INFO lp,*plp = &lp;
            lp.dwSize = 1;
            lp.bVisible = TRUE;
            SetConsoleCursorInfo(gh_std_out,plp);
            SetConsoleCursorPosition(gh_std_out,(COORD)
            {
                rcPop.Left+0, rcPop.Top+3
            });
            char name[15];
            scanf("%s",name);
            lp.bVisible = FALSE;
            SetConsoleCursorInfo(gh_std_out,plp);
            PopOff();
            if(DelEvent(gp_head,name) != 0)
            {
                set_text_success();
            }
            else
            {
                set_text_fail();
            }

        }
        else
        {
            PopOff();
        }
    }
    else
    {
        if(iHot == 2)
        {
            /*插入灾害事件信息结点*/
            PopOff();
            iHot = 2;

            pos.X = 30;
            pos.Y = 14;
            rcPop.Left = 0;
            rcPop.Right =SCR_COL-1;
            rcPop.Top = (25 - pos.Y) / 2;
            rcPop.Bottom = rcPop.Top + pos.Y - 1;

            att = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED;  /*白底黑字*/
            labels.num = 4;
            labels.ppLabel = pCh3;
            COORD aLoc3[] =
            {
                {rcPop.Left+0, rcPop.Top+1},
                {rcPop.Left+1, rcPop.Top+3},
                {rcPop.Left+28, rcPop.Top+13},
                {rcPop.Left+46, rcPop.Top+13}
            };
            labels.pLoc = aLoc3;

            areas.num = 2;
            SMALL_RECT aArea3[] =
            {
                {
                    rcPop.Left+26, rcPop.Top+13,
                    rcPop.Left+36, rcPop.Top+13
                },
                {
                    rcPop.Left + 46, rcPop.Top + 13,
                    rcPop.Left +49, rcPop.Top +13
                }
            };
            char aSort2[] = {0, 0};
            char aTag2[] = {1, 2};
            areas.pArea = aArea3;
            areas.pSort = aSort2;
            areas.pTag = aTag2;
            PopUp(&rcPop, att, &labels, &areas);

            if(DealInput(&areas, &iHot) == 13 && iHot == 1)
            {
                /*开始查找和插入*/
                CONSOLE_CURSOR_INFO lp,*plp = &lp;
                lp.dwSize = 1;
                lp.bVisible = TRUE;
                SetConsoleCursorInfo(gh_std_out,plp);
                SetConsoleCursorPosition(gh_std_out,(COORD)
                {
                    rcPop.Left+0, rcPop.Top+4
                });
                EVENT *pEvent=(EVENT *)malloc(sizeof(EVENT));
                scanf("%c %s %s %s %c %s %d %d %f",&(pEvent->type),pEvent->num,pEvent->time,pEvent->name,&(pEvent->level),pEvent->place,&(pEvent->dead),&(pEvent->collapse),&(pEvent->loss));

                lp.bVisible = FALSE;
                SetConsoleCursorInfo(gh_std_out,plp);
                PopOff();
                if(InsertEvent(gp_head,pEvent) != 0)
                {
                    set_text_success();
                }
                else
                {
                    free(pEvent);
                    set_text_fail();
                }
            }
            else
            {
                PopOff();
            }
        }
        else
        {
            if(iHot == 3)
            {
                /*修改灾害事件信息*/
                PopOff();
                iHot = 2;

                pos.X = 48;
                pos.Y = 15;
                rcPop.Left = 0;
                rcPop.Right = SCR_COL-1;
                rcPop.Top = (25 - pos.Y) / 2;
                rcPop.Bottom = rcPop.Top + pos.Y - 1;

                att = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED;  /*白底黑字*/
                labels.num = 5;
                labels.ppLabel = pCh4;
                COORD aLoc3[] =
                {
                    {rcPop.Left+0, rcPop.Top+1},
                    {rcPop.Left+0, rcPop.Top+2},
                    {rcPop.Left+1, rcPop.Top+4},
                    {rcPop.Left+26, rcPop.Top+14},
                    {rcPop.Left+46, rcPop.Top+14}
                };
                labels.pLoc = aLoc3;

                areas.num = 2;
                SMALL_RECT aArea3[] =
                {
                    {
                        rcPop.Left+26, rcPop.Top+14,
                        rcPop.Left+33, rcPop.Top+13
                    },
                    {
                        rcPop.Left + 46, rcPop.Top + 14,
                        rcPop.Left +49, rcPop.Top +13
                    }
                };
                char aSort2[] = {0, 0};
                char aTag2[] = {1, 2};
                areas.pArea = aArea3;
                areas.pSort = aSort2;
                areas.pTag = aTag2;
                PopUp(&rcPop, att, &labels, &areas);

                if(DealInput(&areas, &iHot) == 13 && iHot == 1)
                {
                    /*开始查找和修改*/
                    CONSOLE_CURSOR_INFO lp,*plp = &lp;
                    lp.dwSize = 1;
                    lp.bVisible = TRUE;
                    SetConsoleCursorInfo(gh_std_out,plp);
                    SetConsoleCursorPosition(gh_std_out,(COORD)
                    {
                        rcPop.Left+0, rcPop.Top+4
                    });
                    EVENT *pEvent2=(EVENT *)malloc(sizeof(EVENT));
                    char num[20];
                    scanf("%s %c %s %s %s %c %s %d %d %f",num,&(pEvent2->type),pEvent2->num,pEvent2->time,pEvent2->name,&(pEvent2->level),pEvent2->place,&(pEvent2->dead),&(pEvent2->collapse),&(pEvent2->loss));
                    lp.bVisible = FALSE;
                    SetConsoleCursorInfo(gh_std_out,plp);
                    PopOff();
                    if(ModifEvent(gp_head,num,pEvent2) != 0)
                    {
                        set_text_success();
                    }
                    else
                    {
                        free(pEvent2);
                        set_text_fail();
                    }
                }
                else
                {
                    PopOff();
                }
            }
            else
            {
                PopOff();
            }
        }
    }
    PopOff();
    getchar();
    InitInterface();
    return bRet;
}

/**
*函数名称：MaintainDonate
*函数功能：维护灾害捐款信息
*输入参数：无
*输出参数：无
*返回值：TRUE
*调用说明：
*/
BOOL MaintainDonate(void)
{
    BOOL bRet = TRUE;
    LABEL_BUNDLE labels;
    HOT_AREA areas;
    SMALL_RECT rcPop;
    COORD pos;
    WORD att;
    char *pCh[] = {"1、删除捐款信息", "2、插入捐款信息","3、修改捐款信息","返回"};
    char *pCh2[] = {"请输入：  事件编号和捐款人姓名","开始输入","取消"};
    char *pCh3[] = {"请输入捐款信息 ：事件编号 捐款日期 捐款类别 捐款者名称 捐款数目                  ","                                                ","开始输入","取消"};
    char *pCh4[] = {"请输入：  原事件编号","请输入更改信息：捐款时间 类别 捐款者姓名 捐款数目                                 ","                                                ","开始输入","取消"};

    int iHot = 4;
    pos.X = strlen(pCh[0]) +12;
    pos.Y = 14;
    rcPop.Left = (SCR_COL - pos.X) / 2;
    rcPop.Right = rcPop.Left + pos.X - 1;
    rcPop.Top = (25 - pos.Y) / 2;
    rcPop.Bottom = rcPop.Top + pos.Y - 1;
    att = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED;  /*白底黑字*/
    labels.num = 4;
    labels.ppLabel = pCh;
    COORD aLoc[] = {{rcPop.Left+6, rcPop.Top+2},
        {rcPop.Left+6, rcPop.Top+5},
        {rcPop.Left+6, rcPop.Top+8},
        {rcPop.Left+10, rcPop.Top+12}
    };
    labels.pLoc = aLoc;
    areas.num = 4;
    SMALL_RECT aArea[] =
    {
        {
            rcPop.Left+6, rcPop.Top+2,
            rcPop.Left+22, rcPop.Top+2
        },
        {
            rcPop.Left + 6, rcPop.Top + 5,
            rcPop.Left +22, rcPop.Top +5
        },
        {
            rcPop.Left + 6, rcPop.Top + 8,
            rcPop.Left +22, rcPop.Top + 8
        },
        {
            rcPop.Left + 10, rcPop.Top + 12,
            rcPop.Left +13, rcPop.Top + 12
        }
    };
    char aSort[] = {0, 0,0,0};
    char aTag[] = {1, 2,3,4};
    areas.pArea = aArea;
    areas.pSort = aSort;
    areas.pTag = aTag;
    PopUp(&rcPop, att, &labels, &areas);
    if (DealInput(&areas, &iHot) == 13 && iHot == 1)
    {
        /*删除捐款信息*/
        PopOff();
        iHot = 2;
        pos.X = 30;
        pos.Y = 14;
        rcPop.Left = 0;
        rcPop.Right = SCR_COL-1;
        rcPop.Top = (25 - pos.Y) / 2;
        rcPop.Bottom = rcPop.Top + pos.Y - 1;
        att = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED;  /*白底黑字*/
        labels.num = 3;
        labels.ppLabel = pCh2;
        COORD aLoc2[] =
        {
            {rcPop.Left+0, rcPop.Top+1},
            {rcPop.Left+28, rcPop.Top+13},
            {rcPop.Left+46, rcPop.Top+13}
        };
        labels.pLoc = aLoc2;
        areas.num = 2;
        SMALL_RECT aArea2[] =
        {
            {
                rcPop.Left+26, rcPop.Top+13,
                rcPop.Left+36, rcPop.Top+13
            },
            {
                rcPop.Left + 46, rcPop.Top + 13,
                rcPop.Left +49, rcPop.Top +13
            }
        };
        char aSort2[] = {0, 0};
        char aTag2[] = {1, 2};
        areas.pArea = aArea2;
        areas.pSort = aSort2;
        areas.pTag = aTag2;
        PopUp(&rcPop, att, &labels, &areas);
        if(DealInput(&areas, &iHot) == 13 && iHot == 1)
        {
            /*开始查找和删除*/
            CONSOLE_CURSOR_INFO lp,*plp = &lp;
            lp.dwSize = 1;
            lp.bVisible = TRUE;
            SetConsoleCursorInfo(gh_std_out,plp);
            SetConsoleCursorPosition(gh_std_out,(COORD)
            {
                rcPop.Left+0, rcPop.Top+3
            });
            char name[15];
            char dname[20];
            scanf("%s %s",name,dname);
            lp.bVisible = FALSE;
            SetConsoleCursorInfo(gh_std_out,plp);
            PopOff();
            if(DelDonate(gp_head,name,dname))
            {
                set_text_success();
            }
            else
            {
                set_text_fail();
            }

        }
        else
        {
            PopOff();
        }
    }
    else
    {
        if(iHot == 2)
        {
            /*插入捐款信息*/
            PopOff();
            iHot = 2;
            pos.X = 30;
            pos.Y = 14;
            rcPop.Left = 0;
            rcPop.Right = SCR_COL-1;
            rcPop.Top = (25 - pos.Y) / 2;
            rcPop.Bottom = rcPop.Top + pos.Y - 1;
            att = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED;  /*白底黑字*/
            labels.num = 4;
            labels.ppLabel = pCh3;
            COORD aLoc3[] =
            {
                {rcPop.Left+0, rcPop.Top+1},
                {rcPop.Left+1, rcPop.Top+3},
                {rcPop.Left+28, rcPop.Top+13},
                {rcPop.Left+46, rcPop.Top+13}
            };
            labels.pLoc = aLoc3;
            areas.num = 2;
            SMALL_RECT aArea3[] =
            {
                {
                    rcPop.Left+26, rcPop.Top+13,
                    rcPop.Left+36, rcPop.Top+13
                },
                {
                    rcPop.Left + 46, rcPop.Top + 13,
                    rcPop.Left +49, rcPop.Top +13
                }
            };
            char aSort2[] = {0, 0};
            char aTag2[] = {1, 2};
            areas.pArea = aArea3;
            areas.pSort = aSort2;
            areas.pTag = aTag2;
            PopUp(&rcPop, att, &labels, &areas);
            if(DealInput(&areas, &iHot) == 13 && iHot == 1)
            {
                /*开始查找和插入*/
                CONSOLE_CURSOR_INFO lp,*plp = &lp;
                lp.dwSize = 1;
                lp.bVisible = TRUE;
                SetConsoleCursorInfo(gh_std_out,plp);
                SetConsoleCursorPosition(gh_std_out,(COORD)
                {
                    rcPop.Left+0, rcPop.Top+4
                });
                DONATE *pDonate=(DONATE *)malloc(sizeof(DONATE));
                scanf("%s %s %c %s %f",pDonate->num,pDonate->time,&(pDonate->type),pDonate->dname,&(pDonate->money));
                lp.bVisible = FALSE;
                SetConsoleCursorInfo(gh_std_out,plp);
                PopOff();

                if(InsertDonate(gp_head,pDonate))
                {
                    set_text_success();
                }
                else
                {
                    free(pDonate);
                    set_text_fail();
                }
            }
            else
            {
                PopOff();
            }
        }
        else
        {
            if(iHot == 3)
            {
                PopOff();
                iHot = 2;
                pos.X = 48;
                pos.Y = 14;
                rcPop.Left = 0;
                rcPop.Right = SCR_COL-1;
                rcPop.Top = (25 - pos.Y) / 2;
                rcPop.Bottom = rcPop.Top + pos.Y - 1;
                att = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED;  /*白底黑字*/
                labels.num = 5;
                labels.ppLabel = pCh4;
                COORD aLoc3[] =
                {
                    {rcPop.Left+0, rcPop.Top+1},
                    {rcPop.Left+0, rcPop.Top+2},
                    {rcPop.Left+1, rcPop.Top+4},
                    {rcPop.Left+26, rcPop.Top+13},
                    {rcPop.Left+46, rcPop.Top+13}
                };
                labels.pLoc = aLoc3;

                areas.num = 2;
                SMALL_RECT aArea3[] =
                {
                    {
                        rcPop.Left+26, rcPop.Top+13,
                        rcPop.Left+36, rcPop.Top+13
                    },
                    {
                        rcPop.Left + 46, rcPop.Top + 13,
                        rcPop.Left +49, rcPop.Top +13
                    }
                };
                char aSort2[] = {0, 0};
                char aTag2[] = {1, 2};
                areas.pArea = aArea3;
                areas.pSort = aSort2;
                areas.pTag = aTag2;
                PopUp(&rcPop, att, &labels, &areas);

                if(DealInput(&areas, &iHot) == 13 && iHot == 1)
                {
                    /*开始查找和修改*/
                    CONSOLE_CURSOR_INFO lp,*plp = &lp;
                    lp.dwSize = 1;
                    lp.bVisible = TRUE;
                    SetConsoleCursorInfo(gh_std_out,plp);
                    SetConsoleCursorPosition(gh_std_out,(COORD)
                    {
                        rcPop.Left+0, rcPop.Top+4
                    });
                    DONATE *pDonate2=(DONATE *)malloc(sizeof(DONATE));
                    scanf("%s %s %c %s %f",pDonate2->num,pDonate2->time,&(pDonate2->type),pDonate2->dname,&(pDonate2->money));
                    lp.bVisible = FALSE;
                    SetConsoleCursorInfo(gh_std_out,plp);
                    PopOff();
                    if(ModifDonate(gp_head,pDonate2->num,pDonate2->dname,pDonate2))
                    {
                        set_text_success();
                    }
                    else
                    {
                        free(pDonate2);
                        set_text_fail();
                    }
                }
                else
                {
                    PopOff();
                }
            }
            else
            {
                PopOff();
            }
        }
    }
    getchar();
    InitInterface();
    return bRet;
}

/**
*函数名称：QueryAssort
*函数功能：查询自然灾害分类信息
*输入参数：无
*输出参数：无
*返回值：TRUE
*调用说明：
*/
BOOL QueryAssort(void)
{
    BOOL bRet = TRUE;
    LABEL_BUNDLE labels;
    HOT_AREA areas;
    SMALL_RECT rcPop;
    COORD pos;
    WORD att;
    char *pCh[] = {"请输入： 灾害名称子串", "开始输入","返回"};
    int iHot = 2;

    pos.X = 30;
    pos.Y = 14;
    rcPop.Left = 0;
    rcPop.Right = SCR_COL-1;
    rcPop.Top = (25 - pos.Y) / 2;
    rcPop.Bottom = rcPop.Top + pos.Y - 1;
    att = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED;  /*白底黑字*/
    labels.num = 3;
    labels.ppLabel = pCh;
    COORD aLoc[] =
    {
        {rcPop.Left+0, rcPop.Top+1},
        {rcPop.Left+28, rcPop.Top+13},
        {rcPop.Left+46, rcPop.Top+13}
    };
    labels.pLoc = aLoc;
    areas.num = 2;
    SMALL_RECT aArea[] =
    {
         {
            rcPop.Left+26, rcPop.Top+13,
            rcPop.Left+36, rcPop.Top+13
        },
        {
            rcPop.Left + 46, rcPop.Top + 13,
            rcPop.Left +49, rcPop.Top +13
        }
    };
    char aSort[] = {0,0};
    char aTag[] = {1,2};
    areas.pArea = aArea;
    areas.pSort = aSort;
    areas.pTag = aTag;
    PopUp(&rcPop, att, &labels, &areas);

    if(DealInput(&areas,&iHot)==13 && iHot==1)
    {
        CONSOLE_CURSOR_INFO lp,*plp = &lp;
        lp.dwSize = 1;
        lp.bVisible = TRUE;
        SetConsoleCursorInfo(gh_std_out,plp);
        SetConsoleCursorPosition(gh_std_out,(COORD)
        {
            rcPop.Left+0, rcPop.Top+3
        });
        char name[15];
        ASSORT *pAssort;
        scanf("%s",name);
        lp.bVisible = FALSE;
        SetConsoleCursorInfo(gh_std_out,plp);
        PopOff();
        if((pAssort=SeekAssort1(gp_head,name))!=NULL)
        {
            SetConsoleCursorPosition(gh_std_out,(COORD)
            {
                1, 1
            });
            lp.bVisible = TRUE;
            SetConsoleCursorInfo(gh_std_out,plp);
            printf("\n");
            printf("灾害名称：%s\n",pAssort->name);
            printf("灾害类别：%c\n",pAssort->type);
            printf("\n");
            getchar();
            getchar();
            InitInterface();
        }
        else
        {
            set_text_fail();
        }
    }
    else
    {
        PopOff();
    }
    return bRet;
}

/**
*函数名称：QueryEvent
*函数功能：查询自然灾害事件信息
*输入参数：无
*输出参数：无
*返回值：TRUE
*调用说明：
*/
BOOL QueryEvent(void)
{
    BOOL bRet = TRUE;
    LABEL_BUNDLE labels;
    HOT_AREA areas;
    SMALL_RECT rcPop;
    COORD pos;
    WORD att;
    char *pCh[] = {"1、按事件编号","2、按时间范围和名称", "返回"};
    char *pCh2[] = {"请输入：事件编号","开始输入","取消"};
    char *pCh3[] = {"时间范围（按时间流逝顺序）","灾害名称","开始输入","取消"};
    int iHot = 3;

    pos.X = strlen(pCh[0]) +10;
    pos.Y = 12;
    rcPop.Left = (SCR_COL - pos.X) / 2;
    rcPop.Right = rcPop.Left + pos.X - 1;
    rcPop.Top = (25 - pos.Y) / 2;
    rcPop.Bottom = rcPop.Top + pos.Y - 1;

    att = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED;  /*白底黑字*/
    labels.num = 3;
    labels.ppLabel = pCh;
    COORD aLoc[] = {{rcPop.Left+3, rcPop.Top+2},
        {rcPop.Left+3, rcPop.Top+5},
        {rcPop.Left+8, rcPop.Top+10}
    };
    labels.pLoc = aLoc;

    areas.num = 3;
    SMALL_RECT aArea[] =
    {
        {
            rcPop.Left + 3, rcPop.Top + 2,
            rcPop.Left + 14, rcPop.Top +2
        },
        {
            rcPop.Left + 3, rcPop.Top + 5,
            rcPop.Left + 19, rcPop.Top +5
        },
        {
            rcPop.Left + 8, rcPop.Top + 10,
            rcPop.Left + 11, rcPop.Top + 10
        }
    };
    char aSort[] = {0,0,0};
    char aTag[] = {1,2,3};
    areas.pArea = aArea;
    areas.pSort = aSort;
    areas.pTag = aTag;
    PopUp(&rcPop, att, &labels, &areas);
    if(DealInput(&areas, &iHot) == 13 && iHot == 1)
    {
        PopOff();
        iHot = 2;

        pos.X = 30;
        pos.Y = 14;
        rcPop.Left = (SCR_COL - pos.X) / 2;
        rcPop.Right = rcPop.Left + pos.X - 1;
        rcPop.Top = (25 - pos.Y) / 2;
        rcPop.Bottom = rcPop.Top + pos.Y - 1;
        att = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED;  /*白底黑字*/
        labels.num = 3;
        labels.ppLabel = pCh2;
        COORD aLoc2[] =
        {
            {rcPop.Left+0, rcPop.Top+1},
            {rcPop.Left+7, rcPop.Top+13},
            {rcPop.Left+18, rcPop.Top+13}
        };
        labels.pLoc = aLoc2;

        areas.num = 2;
        SMALL_RECT aArea2[] =
        {
            {
                rcPop.Left+7, rcPop.Top+13,
                rcPop.Left+15, rcPop.Top+13
            },
            {
                rcPop.Left + 18, rcPop.Top + 13,
                rcPop.Left +21, rcPop.Top +13
            }
        };
        char aSort2[] = {0, 0};
        char aTag2[] = {1, 2};
        areas.pArea = aArea2;
        areas.pSort = aSort2;
        areas.pTag = aTag2;
        PopUp(&rcPop, att, &labels, &areas);

        if(DealInput(&areas, &iHot) == 13 && iHot == 1)
        {
            CONSOLE_CURSOR_INFO lp,*plp = &lp;
            lp.dwSize = 1;
            lp.bVisible = TRUE;
            SetConsoleCursorInfo(gh_std_out,plp);
            SetConsoleCursorPosition(gh_std_out,(COORD)
            {
                rcPop.Left+0, rcPop.Top+3
            });
            char num[12];
            EVENT *pEvent=NULL;
            scanf("%s",num);
            pEvent=SeekEvent(gp_head,num);
            lp.bVisible = FALSE;
            SetConsoleCursorInfo(gh_std_out,plp);
            PopOff();
            if(pEvent!=NULL)
            {
                SetConsoleCursorPosition(gh_std_out,(COORD)
                {
                    1, 1
                });
                lp.bVisible = TRUE;
                SetConsoleCursorInfo(gh_std_out,plp);
                printf("\n");
                printf("自然灾害类别：%c\n",pEvent->type);
                printf("事件编号：%s\n",pEvent->num);
                printf("发生时间：%s\n",pEvent->time);
                printf("灾害名称：%s\n",pEvent->name);
                printf("灾害级别：%c\n",pEvent->level);
                printf("发生地点：%s\n",pEvent->place);
                printf("死亡人数：%d人\n",pEvent->dead);
                printf("民房倒塌：%d\n间",pEvent->collapse);
                printf("直接经济损失：%f亿元人民币",pEvent->loss);
                getchar();
                getchar();
                InitInterface();
            }
            else
            {
                set_text_fail();
            }
        }
        else
        {
            PopOff();
        }
    }
    else
    {
        if(iHot==2)
        {
            PopOff();
            iHot = 2;

            pos.X = 30;
            pos.Y = 14;
            rcPop.Left = 0;
            rcPop.Right = SCR_COL - 1;
            rcPop.Top = (25 - pos.Y) / 2;
            rcPop.Bottom = rcPop.Top + pos.Y - 1;

            att = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED;  /*白底黑字*/
            labels.num = 4;
            labels.ppLabel = pCh3;
            COORD aLoc3[] =
            {
                {rcPop.Left+0, rcPop.Top+1},
                {rcPop.Left+0, rcPop.Top+2},
                {rcPop.Left+28, rcPop.Top+13},
                {rcPop.Left+46, rcPop.Top+13}
            };
            labels.pLoc = aLoc3;

            areas.num = 2;
            SMALL_RECT aArea2[] =
            {
                {
                    rcPop.Left+28, rcPop.Top+13,
                    rcPop.Left+36, rcPop.Top+13
                },
                {
                    rcPop.Left + 46, rcPop.Top + 13,
                    rcPop.Left +49, rcPop.Top +13
                }
            };
            char aSort2[] = {0, 0};
            char aTag2[] = {1, 2};
            areas.pArea = aArea2;
            areas.pSort = aSort2;
            areas.pTag = aTag2;
            PopUp(&rcPop, att, &labels, &areas);

            if(DealInput(&areas, &iHot) == 13 && iHot == 1)
            {
                CONSOLE_CURSOR_INFO lp,*plp = &lp;
                lp.dwSize = 1;
                lp.bVisible = TRUE;
                SetConsoleCursorInfo(gh_std_out,plp);
                SetConsoleCursorPosition(gh_std_out,(COORD)
                {
                    rcPop.Left+0, rcPop.Top+4
                });
                char time1[15];
                char time2[15];
                char name[15];
                scanf("%s %s %s",time1,time2,name);
                EVENT *pEvent2=NULL;
                pEvent2=SeekEvent2(gp_head,time1,time2,name);
                lp.bVisible = FALSE;
                SetConsoleCursorInfo(gh_std_out,plp);
                PopOff();
                InitInterface();
                if(pEvent2!=NULL)
                {
                    SetConsoleCursorPosition(gh_std_out,(COORD)
                    {
                        1, 1
                    });
                    lp.bVisible = TRUE;
                    SetConsoleCursorInfo(gh_std_out,plp);
                    printf("\n");
                    printf("自然灾害类别：%c\n",pEvent2->type);
                    printf("事件编号：%s\n",pEvent2->num);
                    printf("发生时间：%s\n",pEvent2->time);
                    printf("灾害名称：%s\n",pEvent2->name);
                    printf("灾害级别：%c\n",pEvent2->level);
                    printf("发生地点：%s\n",pEvent2->place);
                    printf("死亡人数：%d人\n",pEvent2->dead);
                    printf("民房倒塌：%d\n间",pEvent2->collapse);
                    printf("直接经济损失：%f亿元人民币",pEvent2->loss);
                    getchar();
                    getchar();
                    InitInterface();
                }
                else
                {
                    set_text_fail();
                }
            }
            else
            {
                PopOff();
            }
        }
        else
        {
            PopOff();
        }
    }
}


/**
*函数名称：QueryDonate
*函数功能：查询自然灾害捐款信息
*输入参数：无
*输出参数：无
*返回值：TRUE
*调用说明：
*/
BOOL QueryDonate(void)
{
    BOOL bRet = TRUE;
    LABEL_BUNDLE labels;
    HOT_AREA areas;
    SMALL_RECT rcPop;
    COORD pos;
    WORD att;
    char *pCh[] = {"1、按事件编号及捐款者名称子串","2、按捐款类别和捐款数目", "返回"};
    char *pCh2[] = {"请输入：事件编号和捐款者名称子串","开始输入","取消"};
    char *pCh3[] = {"输入捐款类别","捐款数目区间","开始输入","取消"};
    int iHot = 3;

    pos.X = strlen(pCh[0]) +10;
    pos.Y = 12;
    rcPop.Left = (SCR_COL - pos.X) / 2;
    rcPop.Right = rcPop.Left + pos.X - 1;
    rcPop.Top = (25 - pos.Y) / 2;
    rcPop.Bottom = rcPop.Top + pos.Y - 1;

    att = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED;  /*白底黑字*/
    labels.num = 3;
    labels.ppLabel = pCh;
    COORD aLoc[] = {{rcPop.Left+3, rcPop.Top+2},
        {rcPop.Left+3, rcPop.Top+5},
        {rcPop.Left+8, rcPop.Top+10}
    };
    labels.pLoc = aLoc;

    areas.num = 3;
    SMALL_RECT aArea[] =
    {
        {
            rcPop.Left + 3, rcPop.Top + 2,
            rcPop.Left + 14, rcPop.Top +2
        },
        {
            rcPop.Left + 3, rcPop.Top + 5,
            rcPop.Left + 19, rcPop.Top +5
        },
        {
            rcPop.Left + 8, rcPop.Top + 10,
            rcPop.Left + 11, rcPop.Top + 10
        }
    };
    char aSort[] = {0,0,0};
    char aTag[] = {1,2,3};
    areas.pArea = aArea;
    areas.pSort = aSort;
    areas.pTag = aTag;
    PopUp(&rcPop, att, &labels, &areas);
    if(DealInput(&areas, &iHot) == 13 && iHot == 1)
    {
        PopOff();
        iHot = 2;

        pos.X = 30;
        pos.Y = 14;
        rcPop.Left = (SCR_COL - pos.X) / 2;
        rcPop.Right = rcPop.Left + pos.X - 1;
        rcPop.Top = (25 - pos.Y) / 2;
        rcPop.Bottom = rcPop.Top + pos.Y - 1;
        att = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED;  /*白底黑字*/
        labels.num = 3;
        labels.ppLabel = pCh2;
        COORD aLoc2[] =
        {
            {rcPop.Left+0, rcPop.Top+1},
            {rcPop.Left+7, rcPop.Top+13},
            {rcPop.Left+18, rcPop.Top+13}
        };
        labels.pLoc = aLoc2;

        areas.num = 2;
        SMALL_RECT aArea2[] =
        {
            {
                rcPop.Left+7, rcPop.Top+13,
                rcPop.Left+15, rcPop.Top+13
            },
            {
                rcPop.Left + 18, rcPop.Top + 13,
                rcPop.Left +21, rcPop.Top +13
            }
        };
        char aSort2[] = {0, 0};
        char aTag2[] = {1, 2};
        areas.pArea = aArea2;
        areas.pSort = aSort2;
        areas.pTag = aTag2;
        PopUp(&rcPop, att, &labels, &areas);

        if(DealInput(&areas, &iHot) == 13 && iHot == 1)
        {
            CONSOLE_CURSOR_INFO lp,*plp = &lp;
            lp.dwSize = 1;
            lp.bVisible = TRUE;
            SetConsoleCursorInfo(gh_std_out,plp);
            SetConsoleCursorPosition(gh_std_out,(COORD)
            {
                rcPop.Left+0, rcPop.Top+3
            });
            char num[12];
            char dname[20];
            DONATE *pDonate=NULL;
            scanf("%s %s",num,dname);
            pDonate=SeekDonate(gp_head,num,dname);
            lp.bVisible = FALSE;
            SetConsoleCursorInfo(gh_std_out,plp);
            PopOff();
            if(pDonate!=NULL)
            {
                SetConsoleCursorPosition(gh_std_out,(COORD)
                {
                    1, 1
                });
                lp.bVisible = TRUE;
                SetConsoleCursorInfo(gh_std_out,plp);
                printf("\n");
                printf("事件编号：%s\n",pDonate->num);
                printf("捐款日期：%s\n",pDonate->time);
                printf("捐款类别：%c\n",pDonate->type);
                printf("捐款者名称：%s\n",pDonate->dname);
                printf("捐款数目：%f亿万元\n",pDonate->money);
                getchar();
                getchar();
                InitInterface();
            }
            else
            {
                set_text_fail();
            }
        }
        else
        {
            PopOff();
        }
    }
    else
    {
        if(iHot==2)
        {
            PopOff();
            iHot = 2;

            pos.X = 30;
            pos.Y = 14;
            rcPop.Left = 0;
            rcPop.Right = SCR_COL - 1;
            rcPop.Top = (25 - pos.Y) / 2;
            rcPop.Bottom = rcPop.Top + pos.Y - 1;

            att = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED;  /*白底黑字*/
            labels.num = 4;
            labels.ppLabel = pCh3;
            COORD aLoc3[] =
            {
                {rcPop.Left+0, rcPop.Top+1},
                {rcPop.Left+0, rcPop.Top+2},
                {rcPop.Left+28, rcPop.Top+13},
                {rcPop.Left+46, rcPop.Top+13}
            };
            labels.pLoc = aLoc3;

            areas.num = 2;
            SMALL_RECT aArea2[] =
            {
                {
                    rcPop.Left+28, rcPop.Top+13,
                    rcPop.Left+36, rcPop.Top+13
                },
                {
                    rcPop.Left + 46, rcPop.Top + 13,
                    rcPop.Left +49, rcPop.Top +13
                }
            };
            char aSort2[] = {0, 0};
            char aTag2[] = {1, 2};
            areas.pArea = aArea2;
            areas.pSort = aSort2;
            areas.pTag = aTag2;
            PopUp(&rcPop, att, &labels, &areas);

            if(DealInput(&areas, &iHot) == 13 && iHot == 1)
            {
                CONSOLE_CURSOR_INFO lp,*plp = &lp;
                lp.dwSize = 1;
                lp.bVisible = TRUE;
                SetConsoleCursorInfo(gh_std_out,plp);
                SetConsoleCursorPosition(gh_std_out,(COORD)
                {
                    rcPop.Left+0, rcPop.Top+4
                });
                char type;
                float money1,money2;
                scanf("%c %f %f",&type,&money1,&money2);
                DONATE *pDonate2=NULL;
                pDonate2=SeekDonate2(gp_head,type,money1,money2);
                lp.bVisible = FALSE;
                SetConsoleCursorInfo(gh_std_out,plp);
                PopOff();
                InitInterface();
                if(pDonate2!=NULL)
                {
                    SetConsoleCursorPosition(gh_std_out,(COORD)
                    {
                        1, 1
                    });
                    lp.bVisible = TRUE;
                    SetConsoleCursorInfo(gh_std_out,plp);
                    printf("\n");
                    printf("事件编号：%s\n",pDonate2->num);
                    printf("捐款日期：%s\n",pDonate2->time);
                    printf("捐款类别：%c\n",pDonate2->type);
                    printf("捐款者名称：%s\n",pDonate2->dname);
                    printf("捐款数目：%f亿万元\n",pDonate2->money);
                    getchar();
                    getchar();
                    InitInterface();
                }
                else
                {
                    set_text_fail();
                }
            }
            else
            {
                PopOff();
            }
        }
        else
        {
            PopOff();
        }
    }
}



/**
*函数名称：StatType
*函数功能：按灾害类别统计
*输入参数：无
*输出参数：无
*返回值：TRUE
*调用说明：
*/
BOOL StatType(void)
{
    STAT *pStat=(STAT *)malloc(sizeof(STAT));		// 灾害内部事件 
    STAT *Mid=(STAT *)malloc(sizeof(STAT));
    STAT *phd=pStat;
    STAT *phead=phd;
    ASSORT *pAssort;     //灾害种类 
    EVENT *pEvent;
    DONATE *pDonate;
    CONSOLE_CURSOR_INFO lp,*plp = &lp;
    lp.bVisible = TRUE;
    lp.dwSize = 1;
    SetConsoleCursorPosition(gh_std_out,(COORD)
    {
        1, 1
    });
    SetConsoleCursorInfo(gh_std_out,plp);
    for(pAssort=gp_head;pAssort!=NULL;pAssort=pAssort->next)
    {
    	pStat->next=(STAT *)malloc(sizeof(STAT));
		pStat=pStat->next;
        for(pEvent=pAssort->enext;pEvent!=NULL;pEvent=pEvent->next)
        {
            pStat->happen++;
            pStat->dead+=pEvent->dead;
            pStat->collapse+=pEvent->collapse;
            pStat->loss+=pEvent->loss;
        }
    }
    while(phead!=NULL)
    {
        if(phead->happen<(phead->next)->happen)
        {
            phead->next=Mid;
            (phead->next)->next=Mid->next;
            Mid->next=phead->next;
        }
        phead=phead->next;
    }
    for(phead=phd;phead!=NULL;phead=phead->next)
    {
        printf("发生次数：%d\t",phead->happen);
        printf("死亡总人数：%d\n",phead->dead);
        printf("房屋倒塌总数：%d\t",phead->collapse);
        printf("直接经济损失总额：%f亿元\n",phead->loss);
        printf("*********************************************************");
        printf("\n");
    }
    printf("\n");
    getchar();
    getchar();
    InitInterface();
    return TRUE;
}

/**
*函数名称：StatName
*函数功能：按灾害名称统计
*输入参数：无
*输出参数：无QueryEvent
*返回值：TRUE
*调用说明：
*/
BOOL StatName(void)
{
    BOOL bRet = TRUE;
    LABEL_BUNDLE labels;
    HOT_AREA areas;
    SMALL_RECT rcPop;
    COORD pos;
    WORD att;
    char *pCh[] = {"请输入： 年份", "开始输入","返回"};
    int iHot = 2;

    pos.X = 30;
    pos.Y = 14;
    rcPop.Left = 0;
    rcPop.Right = SCR_COL-1;
    rcPop.Top = (25 - pos.Y) / 2;
    rcPop.Bottom = rcPop.Top + pos.Y - 1;
    att = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED;  /*白底黑字*/
    labels.num = 3;
    labels.ppLabel = pCh;
    COORD aLoc[] =
    {
        {rcPop.Left+0, rcPop.Top+1},
        {rcPop.Left+28, rcPop.Top+13},
        {rcPop.Left+46, rcPop.Top+13}
    };
    labels.pLoc = aLoc;
    areas.num = 2;
    SMALL_RECT aArea[] =
    {
         {
            rcPop.Left+26, rcPop.Top+13,
            rcPop.Left+36, rcPop.Top+13
        },
        {
            rcPop.Left + 46, rcPop.Top + 13,
            rcPop.Left +49, rcPop.Top +13
        }
    };
    char aSort[] = {0,0};
    char aTag[] = {1,2};
    areas.pArea = aArea;
    areas.pSort = aSort;
    areas.pTag = aTag;
    PopUp(&rcPop, att, &labels, &areas);

    if(DealInput(&areas,&iHot)==13 && iHot==1)
    {
        CONSOLE_CURSOR_INFO lp,*plp = &lp;
        lp.dwSize = 1;
        lp.bVisible = TRUE;
        SetConsoleCursorInfo(gh_std_out,plp);
        SetConsoleCursorPosition(gh_std_out,(COORD)
        {
            rcPop.Left+0, rcPop.Top+3
        });
        STAT *pStat,*Mid;
        STAT *phd=pStat;
        STAT *phead=phd;
        ASSORT *pAssort;
        EVENT *pEvent;
        DONATE *pDonate;
        int year,i,_year;
        scanf("%d",&year);
        char Year[4];
        for(pAssort=gp_head;pAssort!=NULL;pAssort=pAssort->next)
        {
        	pStat->next=(STAT *)malloc(sizeof(STAT));
			pStat=pStat->next;
            for(pEvent=pAssort->enext;pEvent!=NULL;pEvent=pEvent->next)
            {
                for(i=0;i<4;i++)
                {
                    Year[i]=(pEvent->num)[i];
                }
                _year=atoi(Year);
                if((_year==year)&&(pEvent->level=='1'||pEvent->level=='2'))
                {
                    pStat->happen++;
                    pStat->dead+=pEvent->dead;
                    pStat->collapse+=pEvent->collapse;
                    pStat->loss+=pEvent->loss;
                }
            }
        }
        while(phead!=NULL)
        {
            if(phead->happen<(phead->next)->happen)
            {
                phead->next=Mid;
                (phead->next)->next=Mid->next;
                Mid->next=phead->next;
            }
            phead=phead->next;
        }
        for(phead=phd;phead!=NULL;phead=phead->next)
        {
            printf("发生次数：%d\t",phead->happen);
            printf("死亡总人数：%d\n",phead->dead);
            printf("房屋倒塌总数：%d\t",phead->collapse);
            printf("直接经济损失总额：%f亿元\n",phead->loss);
            printf("*********************************************************");
            printf("\n");
        }
        getch();
        getch();
        PopOff();
    }
    else
    {
        PopOff();
    }
    return bRet;
}



/**
*函数名称：StatYear
*函数功能：按年份和灾害类型统计某灾害数据
*输入参数：无
*输出参数：无
*返回值：TRUE
*调用说明：
*/
BOOL StatYear(void)
{
    BOOL bRet = TRUE;
    LABEL_BUNDLE labels;
    HOT_AREA areas;
    SMALL_RECT rcPop;
    COORD pos;
    WORD att;
    char *pCh[] = {"请输入： 灾害类别和年份", "开始输入","返回"};

    int iHot = 2;
    pos.X = 30;
    pos.Y = 14;
    rcPop.Left = 0;
    rcPop.Right = SCR_COL-1;
    rcPop.Top = (25 - pos.Y) / 2;
    rcPop.Bottom = rcPop.Top + pos.Y - 1;
    att = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED;  /*白底黑字*/
    labels.num = 3;
    labels.ppLabel = pCh;
    COORD aLoc[] =
    {
        {rcPop.Left+0, rcPop.Top+1},
        {rcPop.Left+28, rcPop.Top+13},
        {rcPop.Left+46, rcPop.Top+13}
    };
    labels.pLoc = aLoc;
    areas.num = 2;
    SMALL_RECT aArea[] =
    {
         {
            rcPop.Left+26, rcPop.Top+13,
            rcPop.Left+36, rcPop.Top+13
        },
        {
            rcPop.Left + 46, rcPop.Top + 13,
            rcPop.Left +49, rcPop.Top +13
        }
    };
    char aSort[] = {0,0};
    char aTag[] = {1,2};
    areas.pArea = aArea;
    areas.pSort = aSort;
    areas.pTag = aTag;
    PopUp(&rcPop, att, &labels, &areas);

    if(DealInput(&areas,&iHot)==13 && iHot==1)
    {
        CONSOLE_CURSOR_INFO lp,*plp = &lp;
        lp.dwSize = 1;
        lp.bVisible = TRUE;
        SetConsoleCursorInfo(gh_std_out,plp);
        SetConsoleCursorPosition(gh_std_out,(COORD)
        {
            rcPop.Left+0, rcPop.Top+3
        });
        STAT *pStat,*Mid;
        STAT *phd=pStat;
        STAT *phead=phd;
        ASSORT *pAssort;
        EVENT *pEvent;
        DONATE *pDonate;
        char type,Year[4];
        int year,i,_year;
        scanf("%c %d",&type,&year);
        for(pAssort=gp_head;pAssort!=NULL;pAssort=pAssort->next,pStat=pStat->next)
        {
            if(pAssort->type==type)
            {
                for(pEvent=pAssort->enext;pEvent!=NULL;pEvent=pEvent->next)
                {
                    for(i=0;i<4;i++)
                    {
                        Year[i]=(pEvent->num)[i];
                    }
                    _year=atoi(Year);
                    if((_year==year)&&(pEvent->level=='1'||pEvent->level=='2'))
                    {
                        pStat->happen++;
                        pStat->dead+=pEvent->dead;
                        pStat->collapse+=pEvent->collapse;
                        pStat->loss+=pEvent->loss;
                    }
                }
            }
        }
        for(phead=phd;phead!=NULL;phead=phead->next)
        {
            printf("发生次数：%d\t",phead->happen);
            printf("死亡总人数：%d\n",phead->dead);
            printf("房屋倒塌总数：%d\t",phead->collapse);
            printf("直接经济损失总额：%f亿元\n",phead->loss);
            printf("*********************************************************");
            printf("\n");
        }
        getch();
        getch();
        PopOff();
    }
    else
    {
        PopOff();
    }
    return bRet;
}

/**
*函数名称：StatDate
*函数功能：按捐款日期统计
*输入参数：无
*输出参数：无
*返回值：TRUE
*调用说明：
*/
BOOL StatDate(void)
{
    BOOL bRet = TRUE;
    LABEL_BUNDLE labels;
    HOT_AREA areas;
    SMALL_RECT rcPop;
    COORD pos;
    WORD att;
    char *pCh[] = {"请输入： 两个时间点", "开始输入","返回"};
    int iHot = 2;

    pos.X = 30;
    pos.Y = 14;
    rcPop.Left = 0;
    rcPop.Right = SCR_COL-1;
    rcPop.Top = (25 - pos.Y) / 2;
    rcPop.Bottom = rcPop.Top + pos.Y - 1;
    att = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED;  /*白底黑字*/
    labels.num = 3;
    labels.ppLabel = pCh;
    COORD aLoc[] =
    {
        {rcPop.Left+0, rcPop.Top+1},
        {rcPop.Left+28, rcPop.Top+13},
        {rcPop.Left+46, rcPop.Top+13}
    };
    labels.pLoc = aLoc;
    areas.num = 2;
    SMALL_RECT aArea[] =
    {
         {
            rcPop.Left+26, rcPop.Top+13,
            rcPop.Left+36, rcPop.Top+13
        },
        {
            rcPop.Left + 46, rcPop.Top + 13,
            rcPop.Left +49, rcPop.Top +13
        }
    };
    char aSort[] = {0,0};
    char aTag[] = {1,2};
    areas.pArea = aArea;
    areas.pSort = aSort;
    areas.pTag = aTag;
    PopUp(&rcPop, att, &labels, &areas);

    if(DealInput(&areas,&iHot)==13 && iHot==1)
    {
        CONSOLE_CURSOR_INFO lp,*plp = &lp;
        lp.dwSize = 1;
        lp.bVisible = TRUE;
        SetConsoleCursorInfo(gh_std_out,plp);
        SetConsoleCursorPosition(gh_std_out,(COORD)
        {
            rcPop.Left+0, rcPop.Top+3
        });
        ASSORT *pAssort;
        EVENT *pEvent;
        DONATE *pDonate;
        int time,time1,time2,i;
        float donate1,donate2;
        char _time[8];
        scanf("%d %d",&time1,&time2);
        for(pAssort=gp_head;pAssort!=NULL;pAssort=pAssort->next)
        {
            for(pEvent=pAssort->enext;pEvent!=NULL;pEvent=pEvent->next)
            {
                for(i=0;i<8;i++)
                {
                    _time[i]=pEvent->num[i];
                }
                time=atoi(_time);
                if(time<=time2&&time>=time1)
                {
                    for(pDonate=pEvent->dnext;pDonate!=NULL;pDonate=pDonate->next)
                    {
                        if(pDonate->type='1')
                        {
                            donate1+=pDonate->money;
                        }
                        else
                        {
                            donate2+=pDonate->money;
                        }
                    }
                }
            }
        }
        printf("个人捐款总数：%f万元\n机构捐款总数：%f万元\n",donate1,donate2);
        getch();
        getch();
        PopOff();
    }
    else
    {
        PopOff();
    }
    return bRet;
}

/**
*函数名称：StatRank
*函数功能：按自然灾害等级统计
*输入参数：无
*输出参数：无
*返回值：TRUE
*调用说明：
*/
BOOL StatRank(void)
{
    BOOL bRet = TRUE;
    LABEL_BUNDLE labels;
    HOT_AREA areas;
    SMALL_RECT rcPop;
    COORD pos;
    WORD att;
    char *pCh[] = {"请输入：自然灾害等级", "开始输入","返回"};
    int iHot = 2;

    pos.X = 30;
    pos.Y = 14;
    rcPop.Left = 0;
    rcPop.Right = SCR_COL-1;
    rcPop.Top = (25 - pos.Y) / 2;
    rcPop.Bottom = rcPop.Top + pos.Y - 1;
    att = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED;  /*白底黑字*/
    labels.num = 3;
    labels.ppLabel = pCh;
    COORD aLoc[] =
    {
        {rcPop.Left+0, rcPop.Top+1},
        {rcPop.Left+28, rcPop.Top+13},
        {rcPop.Left+46, rcPop.Top+13}
    };
    labels.pLoc = aLoc;
    areas.num = 2;
    SMALL_RECT aArea[] =
    {
         {
            rcPop.Left+26, rcPop.Top+13,
            rcPop.Left+36, rcPop.Top+13
        },
        {
            rcPop.Left + 46, rcPop.Top + 13,
            rcPop.Left +49, rcPop.Top +13
        }
    };
    char aSort[] = {0,0};
    char aTag[] = {1,2};
    areas.pArea = aArea;
    areas.pSort = aSort;
    areas.pTag = aTag;
    PopUp(&rcPop, att, &labels, &areas);

    if(DealInput(&areas,&iHot)==13 && iHot==1)
    {
        CONSOLE_CURSOR_INFO lp,*plp = &lp;
        lp.dwSize = 1;
        lp.bVisible = TRUE;
        SetConsoleCursorInfo(gh_std_out,plp);
        SetConsoleCursorPosition(gh_std_out,(COORD)
        {
            rcPop.Left+0, rcPop.Top+3
        });
        STAT *pStat,*Mid;
        STAT *phd=pStat;
        STAT *phead=phd;
        ASSORT *pAssort;
        EVENT *pEvent;
        DONATE *pDonate;
        char Rank;
        float donate;
        scanf("%c",&Rank);
        for(pAssort=gp_head;pAssort!=NULL;pAssort=pAssort->next)
        {
            for(pEvent=pAssort->enext;pEvent!=NULL;pEvent=pEvent->next)
            {
                if(Rank==pEvent->level)
                {
                    for(pDonate=pEvent->dnext;pDonate!=NULL;pDonate=pDonate->next)
                    {
                        donate+=pDonate->money;
                    }
                }
            }
        }
        printf("自然灾害等级为%c的总捐款数目为%f万元\n",Rank,donate);
        getch();
        getch();
        PopOff();
    }
    else
    {
        PopOff();
    }
    return bRet;
}
















