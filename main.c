#include "head.h"
#pragma warning(disable:4996)
int main()
{
   COORD size = {SCR_COL, SCR_ROW};              /*���ڻ�������С*/

  gh_std_out = GetStdHandle(STD_OUTPUT_HANDLE); /* ��ȡ��׼����豸���*/
   gh_std_in = GetStdHandle(STD_INPUT_HANDLE);   /* ��ȡ��׼�����豸���*/

   SetConsoleTitle(gp_sys_name);                 /*���ô��ڱ���*/
  SetConsoleScreenBufferSize(gh_std_out, size); /*���ô��ڻ�������С80*25*/

  LoadData();                   /*���ݼ���*/
  InitInterface();          /*�����ʼ��*/
  RunSys(&gp_head);             /*ϵͳ����ģ���ѡ������*/
  CloseSys(gp_head);            /*�˳�ϵͳ*/

   return 0;
}

/**
 * ��������: CreatList
 * ��������: �������ļ���ȡ��������, ����ŵ���������ʮ��������.
 * �������: ��
 * �������: phead ����ͷָ��ĵ�ַ, ����������������ʮ����.
 * �� �� ֵ: int����ֵ, ��ʾ�����������.
 *           0  ����, ������
 *           4  �Ѽ�����Ȼ�ֺ�������Ϣ���ݣ����¼���Ϣ�;����Ϣ����
 *           12 �Ѽ�����Ȼ�ֺ�������Ϣ���¼���Ϣ���ݣ��޾����Ϣ����
 *           28 ����������ݶ��Ѽ���
 *
 * ����˵��:
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
        printf("��Ȼ�ֺ�������Ϣ�����ļ���ʧ��!\n");
        return re;
    }
    printf("��Ȼ�ֺ�������Ϣ�����ļ��򿪳ɹ�!\n");

    /*�������ļ��ж���Ȼ�ֺ�������Ϣ���ݣ������Ժ���ȳ���ʽ������������*/
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
        printf("��Ȼ�ֺ�������Ϣ�����ļ�����ʧ��!\n");
        return re;
    }
    printf("��Ȼ�ֺ�������Ϣ�����ļ����سɹ�!\n");
    *phead = hd;
    re += 4;

    if ((pFile = fopen(gp_event_info_filename, "rb")) == NULL)
    {
        printf("��Ȼ�ֺ��¼���Ϣ�����ļ���ʧ��!\n");
        return re;
    }
    printf("��Ȼ�ֺ��¼���Ϣ�����ļ��򿪳ɹ�!\n");
    re += 8;

    /*�������ļ��ж�ȡ��Ȼ�ֺ��¼���Ϣ���ݣ�����������Ӧ������Ȼ�ֺ��¼���Ϣ֧����*/
    while (fread(&tmp2, sizeof(EVENT), 1, pFile) == 1)
    {
        /*������㣬��Ŵ������ļ��ж�������Ȼ�ֺ��¼���Ϣ*/
        pEvent = (EVENT *)malloc(sizeof(EVENT));
        *pEvent = tmp2;
        pEvent->dnext = NULL;

        /*�������ϲ��Ҹ��¼�����Ӧ��Ȼ�ֺ�������������*/
        pAssort = hd;
        while (pAssort != NULL
               && (pAssort->type==pEvent->type) != 0)
        {
            pAssort = pAssort->next;
        }
        if (pAssort != NULL) /*����ҵ����򽫽���Ժ���ȳ���ʽ������Ȼ�ֺ��¼���Ϣ֧��*/
        {
            pEvent->next = pAssort->enext;
            pAssort->enext = pEvent;
        }
        else  /*���δ�ҵ������ͷ������������ڴ�ռ�*/
        {
            free(pEvent);
        }
    }
    fclose(pFile);

    if ((pFile = fopen(gp_donate_info_filename, "rb")) == NULL)
    {
        printf("��Ȼ�ֺ������Ϣ�����ļ���ʧ��!\n");
        return re;
    }
    printf("��Ȼ�ֺ������Ϣ�����ļ��򿪳ɹ�!\n");
    re += 16;

    /*�������ļ��ж�ȡ��Ȼ�ֺ������Ϣ���ݣ������ֺ��¼���Ϣ֧����Ӧ���ľ��֧����*/
    while (fread(&tmp3, sizeof(DONATE), 1, pFile) == 1)
    {
        /*������㣬��Ŵ������ļ��ж������ֺ������Ϣ*/
        pDonate = (DONATE *)malloc(sizeof(DONATE));
        *pDonate = tmp3;

        /*������Ȼ�ֺ��¼�֧���϶�Ӧ�¼����*/
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
        if (find)  /*����ҵ����򽫽���Ժ���ȳ���ʽ������Ȼ�ֺ������Ϣ֧����*/
        {
            pDonate->next = pEvent->dnext;
            pEvent->dnext = pDonate;
        }
        else /*���δ�ҵ������ͷ������������ڴ�ռ�*/
        {
            free(pDonate);
        }
    }
    fclose(pFile);

    return re;
}

/**
 * ��������: LoadCode
 * ��������: �������������ļ����뵽�ڴ滺����, �����������ȥ���ո�.
 * �������: FileName ��Ŵ����������ļ���.
 * �������: pBuffer ָ���ڴ滺������ָ������ĵ�ַ.
 * �� �� ֵ: ��Ŵ������ڴ滺������С(���ֽ�Ϊ��λ).
 *
 * ����˵��:
 */
int LoadCode(char *FileName, char **pBuffer)
{
    char *pTemp, *pStr1, *pStr2;
    int handle;
    int BufferLen, len, loc1, loc2, i;
    long filelen;

    if ((handle = open(FileName, O_RDONLY | O_TEXT)) == -1) /*�����ֻ����ʽ��ʧ�� */
    {
        handle = open(FileName, O_CREAT | O_TEXT, S_IREAD); /*�Դ�����ʽ��*/
    }
    filelen = filelength(handle);      /*�����ļ��ĳ���*/
    pTemp = (char *)calloc(filelen + 1, sizeof(char)); /*����ͬ����С�Ķ�̬�洢��*/
    BufferLen = read(handle, pTemp, filelen); /*�������ļ�������ȫ�����뵽�ڴ�*/
    close(handle);

    *(pTemp + BufferLen) = '\0'; /*�ڶ�̬�洢��β��һ�����ַ�����Ϊ�ַ���������־*/
    BufferLen++;

    for (i=0; i<BufferLen; i++) /*����̬�洢���е����л��з��滻�ɿ��ַ�*/
    {
        if (*(pTemp + i) == '\n')
        {
            *(pTemp + i) = '\0';
        }
    }

    /*������һ��ͬ����С�Ķ�̬�洢�������ڴ�������Ĵ��봮*/
    *pBuffer = (char *)calloc(BufferLen, sizeof(char));
    loc2 = 0;
    pStr1 = pTemp;
    len = strlen(pStr1);

    while (BufferLen > len + 1) /*ѡ������*/
    {
        loc1 = len + 1;
        while (BufferLen > loc1) /*ÿ���ҵ���������С���봮���׵�ַ����pStr1*/
        {
            pStr2 = pTemp + loc1;
            if (strcmp(pStr1, pStr2) > 0)
            {
                pStr1 = pStr2;
            }
            loc1 += strlen(pStr2) + 1;
        }
        len = strlen(pStr1);  /*��һ�����ҵ�����С���봮����*/

        /*������ǿմ�������и��ƣ�loc2����һ����С���봮��ŵ�ַ��ƫ����*/
        if (len > 0)
        {
            strcpy(*pBuffer + loc2, pStr1);
            loc2 += len + 1;  /*�Ѹ��ƵĴ��봮��ռ�洢�ռ��С*/
        }

        /*����С���봮��������ɾ����*/
        for(i=0; i<BufferLen-(pStr1-pTemp)-(len+1); i++)
        {
            *(pStr1 + i) = *(pStr1 + i + len + 1);
        }

        BufferLen -= len + 1; /*��һ���������������еĳ���*/
        pStr1 = pTemp;  /*�ٶ����еĵ�һ�����봮Ϊ��С���봮*/
        len = strlen(pStr1);
    } /*������ֻʣ��һ�����봮ʱ���������*/

    /*�������������봮*/
    len = strlen(pStr1);
    strcpy(*pBuffer + loc2, pStr1);

    /*�޸Ķ�̬�洢����С��ʹ�����÷����������봮*/
    loc2 += len + 1;
    *pBuffer = (char *)realloc(*pBuffer, loc2);
    free(pTemp);  /*�ͷ���������Ķ�̬�洢��*/

    return loc2;  /*���ش�Ŵ��봮���ڴ滺����ʵ�ʴ�С*/
}

/**
 * ��������: LoadData
 * ��������: ������������ݴ������ļ����뵽�ڴ滺������ʮ��������.
 * �������: ��.
 * �������: ��.
 * �� �� ֵ: bool����, ���ܺ����г��˺���ExitSys�ķ���ֵ����Ϊ��,
 *           ���������ķ���ֵ����Ϊtrue.
 *
 * ����˵��: Ϊ���ܹ���ͳһ�ķ�ʽ���ø����ܺ���, ����Щ���ܺ�����ԭ����Ϊ
 *           һ��, ���޲����ҷ���ֵΪBOOL. ����ֵΪʱ, ������������.
 */
 BOOL LoadData(void)
{
    int Re ;
    Re = CreatList(&gp_head);


    gc_sys_state |= Re;
    gc_sys_state &= ~(4 + 8 + 16 - Re);
    if (gc_sys_state < (1 | 2 | 4 | 8 | 16))
    {
        /*���ݼ�����ʾ��Ϣ*/

        printf("\n                         �����������...\n");
        getch();
    }

    return TRUE;
}

/**
 * ��������: set_text_success
 * ��������: ĳ������ɹ������������Ѵ���
 * �������: ��
 * �������: ��
 * �� �� ֵ: BOOL����, TRUE
 *
 * ����˵��:
 */
BOOL set_text_success(void)
{
    BOOL bRet = TRUE;
    LABEL_BUNDLE labels;
    HOT_AREA areas;
    SMALL_RECT rcPop;
    COORD pos;
    WORD att;
    char *pCh[] = {"�����ɹ�", "����"};
    int iHot = 1;
    pos.X = strlen(pCh[0]) +6;
    pos.Y = 7;
    rcPop.Left = (SCR_COL - pos.X) / 2;
    rcPop.Right = rcPop.Left + pos.X - 1;
    rcPop.Top = (25 - pos.Y) / 2;
    rcPop.Bottom = rcPop.Top + pos.Y - 1;
    att = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED|FOREGROUND_RED|FOREGROUND_INTENSITY;  /*�׵׺���*/
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
 * ��������: set_text_fail
 * ��������: ĳ�����ʧ�ܣ����������Ѵ���
 * �������: ��
 * �������: ��
 * �� �� ֵ: BOOL����, TRUE
 *
 * ����˵��:
 */
BOOL set_text_fail(void)
{
    BOOL bRet = TRUE;
    LABEL_BUNDLE labels;
    HOT_AREA areas;
    SMALL_RECT rcPop;
    COORD pos;
    WORD att;
    char *pCh[] = {"������Ч", "����"};
    int iHot = 1;
    pos.X = strlen(pCh[0]) +6;
    pos.Y = 7;
    rcPop.Left = (SCR_COL - pos.X) / 2;
    rcPop.Right = rcPop.Left + pos.X - 1;
    rcPop.Top = (25 - pos.Y) / 2;
    rcPop.Bottom = rcPop.Top + pos.Y - 1;
    att = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED|FOREGROUND_RED|FOREGROUND_INTENSITY;  /*�׵׺���*/
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
 * ��������: InitInterface
 * ��������: ��ʼ������.
 * �������: ��
 * �������: ��
 * �� �� ֵ: ��
 *
 * ����˵��:
 */
void InitInterface()
{
    WORD att = FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_INTENSITY
               | BACKGROUND_GREEN;

    SetConsoleTextAttribute(gh_std_out, att);  /*���ÿ���̨��Ļ�������ַ�����*/

    ClearScreen();  /* ����*/

    /*��������������Ϣ��ջ������ʼ�������Ļ���ڵ�����һ�㵯������*/
    gp_scr_att = (char *)calloc(SCR_COL * SCR_ROW, sizeof(char));/*��Ļ�ַ�����*/
    gp_top_layer = (LAYER_NODE *)malloc(sizeof(LAYER_NODE));
    gp_top_layer->LayerNo = 0;      /*�������ڵĲ��Ϊ0*/
    gp_top_layer->rcArea.Left = 0;  /*�������ڵ�����Ϊ������Ļ����*/
    gp_top_layer->rcArea.Top = 0;
    gp_top_layer->rcArea.Right = SCR_COL - 1;
    gp_top_layer->rcArea.Bottom = SCR_ROW - 1;
    gp_top_layer->pContent = NULL;
    gp_top_layer->pScrAtt = gp_scr_att;
    gp_top_layer->next = NULL;

    ShowMenu();     /*��ʾ�˵���*/
    ShowState();    /*��ʾ״̬��*/

    return;
}

/**
 * ��������: ClearScreen
 * ��������: �����Ļ��Ϣ.
 * �������: ��
 * �������: ��
 * �� �� ֵ: ��
 *
 * ����˵��:
 */
void ClearScreen(void)
{
    CONSOLE_SCREEN_BUFFER_INFO bInfo;
    COORD home = {0, 0};
    unsigned long size;

    GetConsoleScreenBufferInfo( gh_std_out, &bInfo );/*ȡ��Ļ��������Ϣ*/
    size =  bInfo.dwSize.X * bInfo.dwSize.Y; /*������Ļ�������ַ���Ԫ��*/

    /*����Ļ���������е�Ԫ���ַ���������Ϊ��ǰ��Ļ�������ַ�����*/
    FillConsoleOutputAttribute(gh_std_out, bInfo.wAttributes, size, home, &ul);

    /*����Ļ���������е�Ԫ���Ϊ�ո��ַ�*/
    FillConsoleOutputCharacter(gh_std_out, ' ', size, home, &ul);
    return;
}


/**
 * ��������: ShowMenu
 * ��������: ����Ļ����ʾ���˵�, ����������, �����˵���һ������ѡ�б��.
 * �������: ��
 * �������: ��
 * �� �� ֵ: ��
 *
 * ����˵��:
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
    for (i=0; i < 5; i++) /*�ڴ��ڵ�һ�е�һ�д�������˵���*/
    {
        printf("  %s  ", ga_main_menu[i]);
    }

    GetConsoleCursorInfo(gh_std_out, &lpCur);
    lpCur.bVisible = FALSE;
    SetConsoleCursorInfo(gh_std_out, &lpCur);  /*���ع��*/

    /*���붯̬�洢����Ϊ��Ų˵�����Ļ���ַ���Ϣ�Ļ�����*/
    gp_buff_menubar_info = (CHAR_INFO *)malloc(size.X * size.Y * sizeof(CHAR_INFO));
    SMALL_RECT rcMenu ={0, 0, size.X-1, 0} ;

    /*�����ڵ�һ�е����ݶ��뵽��Ų˵�����Ļ���ַ���Ϣ�Ļ�������*/
    ReadConsoleOutput(gh_std_out, gp_buff_menubar_info, size, pos, &rcMenu);

    /*����һ����Ӣ����ĸ��Ϊ��ɫ�������ַ���Ԫ��Ϊ�׵׺���*/
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

    /*�޸ĺ�Ĳ˵����ַ���Ϣ��д�����ڵĵ�һ��*/
    WriteConsoleOutput(gh_std_out, gp_buff_menubar_info, size, pos, &rcMenu);
    COORD endPos = {0, 1};
    SetConsoleCursorPosition(gh_std_out, endPos);  /*�����λ�������ڵ�2�е�1��*/

    /*���˵�����Ϊ�������������Ϊ�˵���ţ���������Ϊ0(��ť��)*/
    i = 0;
    do
    {
        PosB = PosA + strlen(ga_main_menu[i]);  /*��λ��i+1�Ų˵������ֹλ��*/
        for (j=PosA; j<PosB; j++)
        {
            gp_scr_att[j] |= (i+1) << 2; /*���ò˵��������ַ���Ԫ������ֵ*/
        }
        PosA = PosB + 4;
        i++;
    } while (i<5);

    TagMainMenu(gi_sel_menu);  /*��ѡ�����˵���������ǣ�gi_sel_menu��ֵΪ1*/

    return;
}


/**
 * ��������: ShowState
 * ��������: ��ʾ״̬��.
 * �������: ��
 * �������: ��
 * �� �� ֵ: ��
 *
 * ����˵��: ״̬���ַ�����Ϊ�׵׺���, ��ʼ״̬��״̬��Ϣ.
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
 * ��������: TagMainMenu
 * ��������: ��ָ�����˵�������ѡ�б�־.
 * �������: num ѡ�е����˵����
 * �������: ��
 * �� �� ֵ: ��
 *
 * ����˵��:
 */
void TagMainMenu(int num)
{
    CONSOLE_SCREEN_BUFFER_INFO bInfo;
    COORD size;
    COORD pos = {0, 0};
    int PosA = 2, PosB;
    char ch;
    int i;

    if (num == 0) /*numΪ0ʱ������ȥ�����˵���ѡ�б��*/
    {
        PosA = 0;
        PosB = 0;
    }
    else  /*���򣬶�λѡ�����˵������ֹλ��: PosAΪ��ʼλ��, PosBΪ��ֹλ��*/
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

    /*ȥ��ѡ�в˵���ǰ��Ĳ˵���ѡ�б��*/
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

    /*��ѡ�в˵���������ǣ��ڵװ���*/
    for (i=PosA; i<PosB; i++)
    {
        (gp_buff_menubar_info+i)->Attributes = FOREGROUND_BLUE | FOREGROUND_GREEN
                                               | FOREGROUND_RED;
    }

    /*ȥ��ѡ�в˵������Ĳ˵���ѡ�б��*/
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

    /*�����ñ�ǵĲ˵�����Ϣд�����ڵ�һ��*/
    SMALL_RECT rcMenu ={0, 0, size.X-1, 0};
    WriteConsoleOutput(gh_std_out, gp_buff_menubar_info, size, pos, &rcMenu);

    return;
}

/**
 * ��������: CloseSys
 * ��������: �ر�ϵͳ.
 * �������: hd ����ͷָ��
 * �������: ��
 * �� �� ֵ: ��
 *
 * ����˵��:
 */
void CloseSys(ASSORT *hd)
{
    ASSORT *pAssort1 = hd, *pAssort2;
    EVENT *pEvent1, *pEvent2;
    DONATE *pDonate1, *pDonate2;

    while (pAssort1 != NULL) /*�ͷ�ʮ�ֽ�������Ķ�̬�洢��*/
    {
        pAssort2 = pAssort1->next;
        pEvent1 = pAssort1->enext;
        while (pEvent1 != NULL) /*�ͷ���Ȼ�ֺ��¼�������Ϣ֧���Ķ�̬�洢��*/
        {
            pEvent2 = pEvent1->next;
            pDonate1 = pEvent1->dnext;
            while (pDonate1 != NULL) /*�ͷž����Ϣ֧���Ķ�̬�洢��*/
            {
                pDonate2 = pDonate1->next;
                free(pDonate1);
                pDonate1 = pDonate2;
            }
            free(pEvent1);
            pEvent1 = pEvent2;
        }
        free(pAssort1);  /*�ͷ��������Ķ�̬�洢��*/
        pAssort1 = pAssort2;
    }

    ClearScreen();        /*����*/

    /*�ͷŴ�Ų˵�����״̬������Ϣ��̬�洢��*/
    free(gp_buff_menubar_info);
    free(gp_buff_stateBar_info);

    /*�رձ�׼���������豸���*/
    CloseHandle(gh_std_out);
    CloseHandle(gh_std_in);

    /*�����ڱ�������Ϊ���н���*/
    SetConsoleTitle("���н���");

    return;
}

/**
 * ��������: RunSys
 * ��������: ����ϵͳ, ��ϵͳ�������������û���ѡ��Ĺ���ģ��.
 * �������: ��
 * �������: phead ����ͷָ��ĵ�ַ
 * �� �� ֵ: ��
 *
 * ����˵��:
 */
void RunSys(ASSORT **phead)
{
    INPUT_RECORD inRec;
    DWORD res;
    COORD pos = {0, 0};
    BOOL bRet = TRUE;
    int i, loc, num;
    int cNo, cAtt;      /*cNo:�ַ���Ԫ���, cAtt:�ַ���Ԫ����*/
    char vkc, asc;      /*vkc:���������, asc:�ַ���ASCII��ֵ*/

    while (bRet)
    {
        /*�ӿ���̨���뻺�����ж�һ����¼*/
        ReadConsoleInput(gh_std_in, &inRec, 1, &res);

        if (inRec.EventType == MOUSE_EVENT) /*�����¼������¼�����*/
        {
            pos = inRec.Event.MouseEvent.dwMousePosition;  /*��ȡ�������λ��*/
            cNo = gp_scr_att[pos.Y * SCR_COL + pos.X] & 3; /*ȡ��λ�õĲ��*/
            cAtt = gp_scr_att[pos.Y * SCR_COL + pos.X] >> 2;/*ȡ���ַ���Ԫ����*/
            if (cNo == 0) /*���Ϊ0��������λ��δ�������Ӳ˵�����*/
            {
                /* cAtt > 0 ������λ�ô�������(���˵����ַ���Ԫ)
                 * cAtt != gi_sel_menu ������λ�õ����˵���δ��ѡ��
                 * gp_top_layer->LayerNo > 0 ������ǰ���Ӳ˵�����
                 */
                if (cAtt > 0 && cAtt != gi_sel_menu && gp_top_layer->LayerNo > 0)
                {
                    PopOff();            /*�رյ������Ӳ˵�*/
                    gi_sel_sub_menu = 0; /*��ѡ���Ӳ˵���������Ϊ0*/
                    PopMenu(cAtt);       /*��������������˵����Ӧ���Ӳ˵�*/
                }
            }
            else if (cAtt > 0) /*�������λ��Ϊ�����Ӳ˵��Ĳ˵����ַ���Ԫ*/
            {
                TagSubMenu(cAtt); /*�ڸ��Ӳ˵�������ѡ�б��*/
            }

            if (inRec.Event.MouseEvent.dwButtonState
                == FROM_LEFT_1ST_BUTTON_PRESSED) /*������������ߵ�һ��*/
            {
                if (cNo == 0) /*���Ϊ0��������λ��δ�������Ӳ˵�����*/
                {
                    if (cAtt > 0) /*�����λ�ô�������(���˵����ַ���Ԫ)*/
                    {
                        PopMenu(cAtt);   /*��������������˵����Ӧ���Ӳ˵�*/
                    }
                    /*�����λ�ò��������˵����ַ���Ԫ�������Ӳ˵�����*/
                    else if (gp_top_layer->LayerNo > 0)
                    {
                        PopOff();            /*�رյ������Ӳ˵�*/
                        gi_sel_sub_menu = 0; /*��ѡ���Ӳ˵���������Ϊ0*/
                    }
                }
                else /*��Ų�Ϊ0��������λ�ñ������Ӳ˵�����*/
                {
                    if (cAtt > 0) /*�����λ�ô�������(�Ӳ˵����ַ���Ԫ)*/
                    {
                        PopOff(); /*�رյ������Ӳ˵�*/
                        gi_sel_sub_menu = 0; /*��ѡ���Ӳ˵���������Ϊ0*/

                        /*ִ�ж�Ӧ���ܺ���:gi_sel_menu���˵����,cAtt�Ӳ˵����*/
                        bRet = ExeFunction(gi_sel_menu, cAtt);
                    }
                }
            }
            else if (inRec.Event.MouseEvent.dwButtonState
                     == RIGHTMOST_BUTTON_PRESSED) /*�����������Ҽ�*/
            {
                if (cNo == 0) /*���Ϊ0��������λ��δ�������Ӳ˵�����*/
                {
                    PopOff();            /*�رյ������Ӳ˵�*/
                    gi_sel_sub_menu = 0; /*��ѡ���Ӳ˵���������Ϊ0*/
                }
            }
        }
        else if (inRec.EventType == KEY_EVENT  /*�����¼�ɰ�������*/
                 && inRec.Event.KeyEvent.bKeyDown) /*�Ҽ�������*/
        {
            vkc = inRec.Event.KeyEvent.wVirtualKeyCode; /*��ȡ�������������*/
            asc = inRec.Event.KeyEvent.uChar.AsciiChar; /*��ȡ������ASC��*/

            /*ϵͳ��ݼ��Ĵ���*/
            if (vkc == 112) /*�������F1��*/
            {
                if (gp_top_layer->LayerNo != 0) /*�����ǰ���Ӳ˵�����*/
                {
                    PopOff();            /*�رյ������Ӳ˵�*/
                    gi_sel_sub_menu = 0; /*��ѡ���Ӳ˵���������Ϊ0*/
                }
                bRet = ExeFunction(5, 1);  /*���а������⹦�ܺ���*/
            }
            else if (inRec.Event.KeyEvent.dwControlKeyState
                     & (LEFT_ALT_PRESSED | RIGHT_ALT_PRESSED))
            { /*������������Alt��*/
                switch (vkc)  /*�ж���ϼ�Alt+��ĸ*/
                {
                    case 88:  /*Alt+X �˳�*/
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
            else if (asc == 0) /*�������Ƽ��Ĵ���*/
            {
                if (gp_top_layer->LayerNo == 0) /*���δ�����Ӳ˵�*/
                {
                    switch (vkc) /*�������(���ҡ���)������Ӧ�������Ƽ�*/
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
                else  /*�ѵ����Ӳ˵�ʱ*/
                {
                    for (loc=0,i=1; i<gi_sel_menu; i++)
                    {
                        loc += ga_sub_menu_count[i-1];
                    }  /*������Ӳ˵��еĵ�һ�����Ӳ˵��ַ��������е�λ��(�±�)*/
                    switch (vkc) /*�����(���ҡ��ϡ���)�Ĵ���*/
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
            else if ((asc-vkc == 0) || (asc-vkc == 32)){  /*������ͨ��*/
                if (gp_top_layer->LayerNo == 0)  /*���δ�����Ӳ˵�*/
                {
                    switch (vkc)
                    {
                        case 70: /*f��F*/
                            PopMenu(1);
                            break;
                        case 77: /*m��M*/
                            PopMenu(2);
                            break;
                        case 81: /*q��Q*/
                            PopMenu(3);
                            break;
                        case 83: /*s��S*/
                            PopMenu(4);
                            break;
                        case 72: /*h��H*/
                            PopMenu(5);
                            break;
                        case 13: /*�س�*/
                            PopMenu(gi_sel_menu);
                            TagSubMenu(1);
                            break;
                    }
                }
                else /*�ѵ����Ӳ˵�ʱ�ļ������봦��*/
                {
                    if (vkc == 27) /*�������ESC��*/
                    {
                        PopOff();
                        gi_sel_sub_menu = 0;
                    }
                    else if(vkc == 13) /*������»س���*/
                    {
                        num = gi_sel_sub_menu;
                        PopOff();
                        gi_sel_sub_menu = 0;
                        bRet = ExeFunction(gi_sel_menu, num);
                    }
                    else /*������ͨ���Ĵ���*/
                    {
                        /*������Ӳ˵��еĵ�һ�����Ӳ˵��ַ��������е�λ��(�±�)*/
                        for (loc=0,i=1; i<gi_sel_menu; i++)
                        {
                            loc += ga_sub_menu_count[i-1];
                        }

                        /*�����뵱ǰ�Ӳ˵���ÿһ��Ĵ����ַ����бȽ�*/
                        for (i=loc; i<loc+ga_sub_menu_count[gi_sel_menu-1]; i++)
                        {
                            if (strlen(ga_sub_menu[i])>0 && vkc==ga_sub_menu[i][1])
                            { /*���ƥ��ɹ�*/
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
 * ��������: PopMenu
 * ��������: ����ָ�����˵����Ӧ���Ӳ˵�.
 * �������: num ָ�������˵����
 * �������: ��
 * �� �� ֵ: ��
 *
 * ����˵��:
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

    if (num != gi_sel_menu)       /*���ָ�����˵�������ѡ�в˵�*/
    {
        if (gp_top_layer->LayerNo != 0) /*�����ǰ�����Ӳ˵�����*/
        {
            PopOff();
            gi_sel_sub_menu = 0;
        }
    }
    else if (gp_top_layer->LayerNo != 0) /*���ѵ������Ӳ˵����򷵻�*/
    {
        return;
    }

    gi_sel_menu = num;    /*��ѡ�����˵�����Ϊָ�������˵���*/
    TagMainMenu(gi_sel_menu); /*��ѡ�е����˵����������*/
    LocSubMenu(gi_sel_menu, &rcPop); /*���㵯���Ӳ˵�������λ��, �����rcPop��*/

    /*������Ӳ˵��еĵ�һ�����Ӳ˵��ַ��������е�λ��(�±�)*/
    for (i=1; i<gi_sel_menu; i++)
    {
        loc += ga_sub_menu_count[i-1];
    }
    /*�������Ӳ˵������������ǩ���ṹ����*/
    labels.ppLabel = ga_sub_menu + loc;   /*��ǩ����һ����ǩ�ַ����ĵ�ַ*/
    labels.num = ga_sub_menu_count[gi_sel_menu-1]; /*��ǩ���б�ǩ�ַ����ĸ���*/
    COORD aLoc[labels.num];/*����һ���������飬���ÿ����ǩ�ַ������λ�õ�����*/
    for (i=0; i<labels.num; i++) /*ȷ����ǩ�ַ��������λ�ã����������������*/
    {
        aLoc[i].X = rcPop.Left + 2;
        aLoc[i].Y = rcPop.Top + i + 1;
    }
    labels.pLoc = aLoc; /*ʹ��ǩ���ṹ����labels�ĳ�ԱpLocָ�������������Ԫ��*/
    /*����������Ϣ*/
    areas.num = labels.num;       /*�����ĸ��������ڱ�ǩ�ĸ��������Ӳ˵�������*/
    SMALL_RECT aArea[areas.num];                    /*������������������λ��*/
    char aSort[areas.num];                      /*��������������������Ӧ���*/
    char aTag[areas.num];                         /*����������ÿ�������ı��*/
    for (i=0; i<areas.num; i++)
    {
        aArea[i].Left = rcPop.Left + 2;  /*������λ*/
        aArea[i].Top = rcPop.Top + i + 1;
        aArea[i].Right = rcPop.Right - 2;
        aArea[i].Bottom = aArea[i].Top;
        aSort[i] = 0;       /*�������Ϊ0(��ť��)*/
        aTag[i] = i + 1;           /*������˳����*/
    }
    areas.pArea = aArea;/*ʹ�����ṹ����areas�ĳ�ԱpAreaָ������λ��������Ԫ��*/
    areas.pSort = aSort;/*ʹ�����ṹ����areas�ĳ�ԱpSortָ���������������Ԫ��*/
    areas.pTag = aTag;   /*ʹ�����ṹ����areas�ĳ�ԱpTagָ���������������Ԫ��*/

    att = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED;  /*�׵׺���*/
    PopUp(&rcPop, att, &labels, &areas);
    DrawBox(&rcPop);  /*���������ڻ��߿�*/
    pos.X = rcPop.Left + 2;
    for (pos.Y=rcPop.Top+1; pos.Y<rcPop.Bottom; pos.Y++)
    { /*��ѭ�������ڿմ��Ӳ���λ�û����γɷָ�����ȡ���˲˵������������*/
        pCh = ga_sub_menu[loc+pos.Y-rcPop.Top-1];
        if (strlen(pCh)==0) /*����Ϊ0������Ϊ�մ�*/
        {   /*���Ȼ�����*/
            FillConsoleOutputCharacter(gh_std_out, '-', rcPop.Right-rcPop.Left-3, pos, &ul);
            for (j=rcPop.Left+2; j<rcPop.Right-1; j++)
            {   /*ȡ���������ַ���Ԫ����������*/
                gp_scr_att[pos.Y*SCR_COL+j] &= 3; /*��λ��Ľ�������˵���λ*/
            }
        }

    }
    /*���Ӳ˵���Ĺ��ܼ���Ϊ�׵׺���*/
    pos.X = rcPop.Left + 3;
    att =  FOREGROUND_RED | BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED;
    for (pos.Y=rcPop.Top+1; pos.Y<rcPop.Bottom; pos.Y++)
    {
        if (strlen(ga_sub_menu[loc+pos.Y-rcPop.Top-1])==0)
        {
            continue;  /*�����մ�*/
        }
        FillConsoleOutputAttribute(gh_std_out, att, 1, pos, &ul);
    }
    return;
}

void PopPrompt(int num)
{

}

/**
 * ��������: PopUp
 * ��������: ��ָ�������������������Ϣ, ͬʱ��������, ����������λ����Ϣ��ջ.
 * �������: pRc ��������λ�����ݴ�ŵĵ�ַ
 *           att �������������ַ�����
 *           pLabel ���������б�ǩ����Ϣ��ŵĵ�ַ
             pHotArea ����������������Ϣ��ŵĵ�ַ
 * �������: ��
 * �� �� ֵ: ��
 *
 * ����˵��:
 */
void PopUp(SMALL_RECT *pRc, WORD att, LABEL_BUNDLE *pLabel, HOT_AREA *pHotArea)
{
    LAYER_NODE *nextLayer;
    COORD size;
    COORD pos = {0, 0};
    char *pCh;
    int i, j, row;

    /*������������λ���ַ���Ԫ��Ϣ��ջ*/
    size.X = pRc->Right - pRc->Left + 1;    /*�������ڵĿ��*/
    size.Y = pRc->Bottom - pRc->Top + 1;    /*�������ڵĸ߶�*/
    /*�����ŵ������������Ϣ�Ķ�̬�洢��*/
    nextLayer = (LAYER_NODE *)malloc(sizeof(LAYER_NODE));
    nextLayer->next = gp_top_layer;
    nextLayer->LayerNo = gp_top_layer->LayerNo + 1;
    nextLayer->rcArea = *pRc;
    nextLayer->pContent = (CHAR_INFO *)malloc(size.X*size.Y*sizeof(CHAR_INFO));
    nextLayer->pScrAtt = (char *)malloc(size.X*size.Y*sizeof(char));
    pCh = nextLayer->pScrAtt;
    /*���������ڸ���������ַ���Ϣ���棬�����ڹرյ�������ʱ�ָ�ԭ��*/
    ReadConsoleOutput(gh_std_out, nextLayer->pContent, size, pos, pRc);
    for (i=pRc->Top; i<=pRc->Bottom; i++)
    {   /*�˶���ѭ�����������ַ���Ԫ��ԭ������ֵ���붯̬�洢���������Ժ�ָ�*/
        for (j=pRc->Left; j<=pRc->Right; j++)
        {
            *pCh = gp_scr_att[i*SCR_COL+j];
            pCh++;
        }
    }
    gp_top_layer = nextLayer;  /*��ɵ������������Ϣ��ջ����*/
    /*���õ������������ַ���������*/
    pos.X = pRc->Left;
    pos.Y = pRc->Top;
    for (i=pRc->Top; i<=pRc->Bottom; i++)
    {
        FillConsoleOutputAttribute(gh_std_out, att, size.X, pos, &ul);
        pos.Y++;
    }
    /*����ǩ���еı�ǩ�ַ������趨��λ�����*/
    for (i=0; i<pLabel->num; i++)
    {
        pCh = pLabel->ppLabel[i];
        if (strlen(pCh) != 0)
        {
            WriteConsoleOutputCharacter(gh_std_out, pCh, strlen(pCh),
                                        pLabel->pLoc[i], &ul);
        }
    }
    /*���õ������������ַ���Ԫ��������*/
    for (i=pRc->Top; i<=pRc->Bottom; i++)
    {   /*�˶���ѭ�������ַ���Ԫ�Ĳ��*/
        for (j=pRc->Left; j<=pRc->Right; j++)
        {
            gp_scr_att[i*SCR_COL+j] = gp_top_layer->LayerNo;
        }
    }

    for (i=0; i<pHotArea->num; i++)
    {   /*�˶���ѭ�����������������ַ���Ԫ���������ͺ��������*/
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
 * ��������: PopOff
 * ��������: �رն��㵯������, �ָ���������ԭ��ۺ��ַ���Ԫԭ����.
 * �������: ��
 * �������: ��
 * �� �� ֵ: ��
 *
 * ����˵��:
 */
void PopOff(void)
{
    LAYER_NODE *nextLayer;
    COORD size;
    COORD pos = {0, 0};
    char *pCh;
    int i, j;

    if ((gp_top_layer->next==NULL) || (gp_top_layer->pContent==NULL))
    {   /*ջ�״�ŵ���������Ļ��Ϣ�����ùر�*/
        return;
    }
    nextLayer = gp_top_layer->next;
    /*�ָ�������������ԭ���*/
    size.X = gp_top_layer->rcArea.Right - gp_top_layer->rcArea.Left + 1;
    size.Y = gp_top_layer->rcArea.Bottom - gp_top_layer->rcArea.Top + 1;
    WriteConsoleOutput(gh_std_out, gp_top_layer->pContent, size, pos, &(gp_top_layer->rcArea));
    /*�ָ��ַ���Ԫԭ����*/
    pCh = gp_top_layer->pScrAtt;
    for (i=gp_top_layer->rcArea.Top; i<=gp_top_layer->rcArea.Bottom; i++)
    {
        for (j=gp_top_layer->rcArea.Left; j<=gp_top_layer->rcArea.Right; j++)
        {
            gp_scr_att[i*SCR_COL+j] = *pCh;
            pCh++;
        }
    }
    free(gp_top_layer->pContent);    /*�ͷŶ�̬�洢��*/
    free(gp_top_layer->pScrAtt);
    free(gp_top_layer);
    gp_top_layer = nextLayer;
    gi_sel_sub_menu = 0;
    return;
}

/**
 * ��������: DrawBox
 * ��������: ��ָ�����򻭱߿�.
 * �������: pRc �������λ����Ϣ�ĵ�ַ
 * �������: ��
 * �� �� ֵ: ��
 *
 * ����˵��:
 */
void DrawBox(SMALL_RECT *pRc)
{
    char chBox[] = {'+','-','|'};  /*�����õ��ַ�*/
    COORD pos = {pRc->Left, pRc->Top};  /*��λ����������Ͻ�*/

    WriteConsoleOutputCharacter(gh_std_out, &chBox[0], 1, pos, &ul);/*���߿����Ͻ�*/
    for (pos.X = pRc->Left + 1; pos.X < pRc->Right; pos.X++)
    {   /*��ѭ�����ϱ߿����*/
        WriteConsoleOutputCharacter(gh_std_out, &chBox[1], 1, pos, &ul);
    }
    pos.X = pRc->Right;
    WriteConsoleOutputCharacter(gh_std_out, &chBox[0], 1, pos, &ul);/*���߿����Ͻ�*/
    for (pos.Y = pRc->Top+1; pos.Y < pRc->Bottom; pos.Y++)
    {   /*��ѭ�����߿�����ߺ��ұ���*/
        pos.X = pRc->Left;
        WriteConsoleOutputCharacter(gh_std_out, &chBox[2], 1, pos, &ul);
        pos.X = pRc->Right;
        WriteConsoleOutputCharacter(gh_std_out, &chBox[2], 1, pos, &ul);
    }
    pos.X = pRc->Left;
    pos.Y = pRc->Bottom;
    WriteConsoleOutputCharacter(gh_std_out, &chBox[0], 1, pos, &ul);/*���߿����½�*/
    for (pos.X = pRc->Left + 1; pos.X < pRc->Right; pos.X++)
    {   /*���±߿����*/
        WriteConsoleOutputCharacter(gh_std_out, &chBox[1], 1, pos, &ul);
    }
    pos.X = pRc->Right;
    WriteConsoleOutputCharacter(gh_std_out, &chBox[0], 1, pos, &ul);/*���߿����½�*/
    return;
}

/**
 * ��������: TagSubMenu
 * ��������: ��ָ���Ӳ˵�������ѡ�б��.
 * �������: num ѡ�е��Ӳ˵����
 * �������: ��
 * �� �� ֵ: ��
 *
 * ����˵��:
 */
void TagSubMenu(int num)
{
    SMALL_RECT rcPop;
    COORD pos;
    WORD att;
    int width;

    LocSubMenu(gi_sel_menu, &rcPop);  /*���㵯���Ӳ˵�������λ��, �����rcPop��*/
    if ((num<1) || (num == gi_sel_sub_menu) || (num>rcPop.Bottom-rcPop.Top-1))
    {   /*����Ӳ˵����Խ�磬������Ӳ˵��ѱ�ѡ�У��򷵻�*/
        return;
    }

    pos.X = rcPop.Left + 2;
    width = rcPop.Right - rcPop.Left - 3;
    if (gi_sel_sub_menu != 0) /*����ȡ��ԭѡ���Ӳ˵����ϵı��*/
    {
        pos.Y = rcPop.Top + gi_sel_sub_menu;
        att = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED;  /*�׵׺���*/
        FillConsoleOutputAttribute(gh_std_out, att, width, pos, &ul);
        pos.X += 1;
        att |=  FOREGROUND_RED;/*�׵׺���*/
        FillConsoleOutputAttribute(gh_std_out, att, 1, pos, &ul);
    }
    /*���ƶ��Ӳ˵�������ѡ�б��*/
    pos.X = rcPop.Left + 2;
    pos.Y = rcPop.Top + num;
    att = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED;  /*�ڵװ���*/
    FillConsoleOutputAttribute(gh_std_out, att, width, pos, &ul);
    gi_sel_sub_menu = num;  /*�޸�ѡ���Ӳ˵����*/
    return;
}

/**
 * ��������: LocSubMenu
 * ��������: ���㵯���Ӳ˵��������ϽǺ����½ǵ�λ��.
 * �������: num ѡ�е����˵����
 * �������: rc �������λ����Ϣ�ĵ�ַ
 * �� �� ֵ: ��
 *
 * ����˵��:
 */
void LocSubMenu(int num, SMALL_RECT *rc)
{
    int i, len, loc = 0;

    rc->Top = 1; /*������ϱ߶��ڵ�2�У��к�Ϊ1*/
    rc->Left = 1;
    for (i=1; i<num; i++)
    {   /*����������߽�λ��, ͬʱ�����һ���Ӳ˵������Ӳ˵��ַ��������е�λ��*/
        rc->Left += strlen(ga_main_menu[i-1]) + 4;
        loc += ga_sub_menu_count[i-1];
    }
    rc->Right = strlen(ga_sub_menu[loc]);/*��ʱ��ŵ�һ���Ӳ˵����ַ�������*/
    for (i=1; i<ga_sub_menu_count[num-1]; i++)
    {   /*������Ӳ˵��ַ��������䳤�ȴ����rc->Right*/
        len = strlen(ga_sub_menu[loc+i]);
        if (rc->Right < len)
        {
            rc->Right = len;
        }
    }
    rc->Right += rc->Left + 3;  /*����������ұ߽�*/
    rc->Bottom = rc->Top + ga_sub_menu_count[num-1] + 1;/*���������±ߵ��к�*/
    if (rc->Right >= SCR_COL)  /*�ұ߽�Խ��Ĵ���*/
    {
        len = rc->Right - SCR_COL + 1;
        rc->Left -= len;
        rc->Right = SCR_COL - 1;
    }
    return;
}

/**
 * ��������: DealInput
 * ��������: �ڵ�������������������, �ȴ�����Ӧ�û�����.
 * �������: pHotArea
 *           piHot ����������ŵĴ�ŵ�ַ, ��ָ�򽹵�������ŵ�ָ��
 * �������: piHot ����굥�������س���ո�ʱ���ص�ǰ�������
 * �� �� ֵ:
 *
 * ����˵��:
 */
int DealInput(HOT_AREA *pHotArea, int *piHot)
{
    INPUT_RECORD inRec;
    DWORD res;
    COORD pos = {0, 0};
    int num, arrow, iRet = 0;
    int cNo, cTag, cSort;/*cNo:���, cTag:�������, cSort: ��������*/
    char vkc, asc;       /*vkc:���������, asc:�ַ���ASCII��ֵ*/

    SetHotPoint(pHotArea, *piHot);
    while (TRUE)
    {    /*ѭ��*/
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
                {  /*�����(���ϡ��ҡ���)�Ĵ���*/
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
            {  /*ESC��*/
                iRet = 27;
                break;
            }
            else if (vkc == 13 || vkc == 32)
            {  /*�س�����ո��ʾ���µ�ǰ��ť*/
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

    att1 = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED;  /*�ڵװ���*/
    att2 = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED;  /*�׵׺���*/
    for (i=0; i<pHotArea->num; i++)
    {  /*����ť��������Ϊ�׵׺���*/
        pos.X = pHotArea->pArea[i].Left;
        pos.Y = pHotArea->pArea[i].Top;
        width = pHotArea->pArea[i].Right - pHotArea->pArea[i].Left + 1;
        if (pHotArea->pSort[i] == 0)
        {  /*�����ǰ�ť��*/
            FillConsoleOutputAttribute(gh_std_out, att2, width, pos, &ul);
        }
    }

    pos.X = pHotArea->pArea[iHot-1].Left;
    pos.Y = pHotArea->pArea[iHot-1].Top;
    width = pHotArea->pArea[iHot-1].Right - pHotArea->pArea[iHot-1].Left + 1;
    if (pHotArea->pSort[iHot-1] == 0)
    {  /*�����������ǰ�ť��*/
        FillConsoleOutputAttribute(gh_std_out, att1, width, pos, &ul);
    }
    else if (pHotArea->pSort[iHot-1] == 1)
    {  /*�������������ı�����*/
        SetConsoleCursorPosition(gh_std_out, pos);
        GetConsoleCursorInfo(gh_std_out, &lpCur);
        lpCur.bVisible = TRUE;
        SetConsoleCursorInfo(gh_std_out, &lpCur);
    }
}

/**
 * ��������: ExeFunction
 * ��������: ִ�������˵��ź��Ӳ˵���ȷ���Ĺ��ܺ���.
 * �������: m ���˵����
 *           s �Ӳ˵����
 * �������: ��
 * �� �� ֵ: BOOL����, TRUE �� FALSE
 *
 * ����˵��: ����ִ�к���ExitSysʱ, �ſ��ܷ���FALSE, ������������Ƿ���TRUE
 */
BOOL ExeFunction(int m, int s)
{
    BOOL bRet = TRUE;
    /*����ָ�����飬����������й��ܺ�������ڵ�ַ*/
    BOOL (*pFunction[ga_sub_menu_count[0]+ga_sub_menu_count[1]+ga_sub_menu_count[2]+ga_sub_menu_count[3]+ga_sub_menu_count[4]])(void);
    int i, loc;

    /*�����ܺ�����ڵ�ַ�����빦�ܺ����������˵��ź��Ӳ˵��Ŷ�Ӧ�±������Ԫ��*/
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


    for (i=1,loc=0; i<m; i++)  /*�������˵��ź��Ӳ˵��ż����Ӧ�±�*/
    {
        loc += ga_sub_menu_count[i-1];
    }
    loc += s - 1;

    if (pFunction[loc] != NULL)
    {
        bRet = (*pFunction[loc])();  /*�ú���ָ�������ָ��Ĺ��ܺ���*/
    }

    return bRet;
}

/**
*�������ƣ�ShowModule
*�������ܣ���ʾȷ�ϲ˵�
*���������char **pString, int n
*�����������
*����ֵ��BOOL���ͣ����ܺ����г���ExitSys�ķ���ֵ����ΪFALSE�⣬���������ķ���ֵ����ΪTRUE
*����˵����
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

    att = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED;  /*�׵׺���*/
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
 �������ƣ�SaveData
 �������ܣ�����ϵͳ�����������������
 �����������
 ����ֵ��BOOL���ͣ�����ΪTRUE
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
 �������ƣ�BackupData
 �������ܣ���ϵͳ����������������ݱ��ݵ�һ�������ļ�
 �����������
 ����ֵ��BOOL���ͣ�����ΪTRUE
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
*�������ƣ�EXitSys
*�������ܣ��˳�ϵͳ
*�����������
*�����������
*����ֵ��BOOL���ͣ����ܺ����г���ExitSys�ķ���ֵ����ΪFALSE�⣬���������ķ���ֵ����ΪTRUE
*����˵����
*/
BOOL ExitSys(void)
{
    LABEL_BUNDLE labels;
    HOT_AREA areas;
    BOOL bRet = TRUE;
    SMALL_RECT rcPop;
    COORD pos;
    WORD att;
    char *pCh[] = {"ȷ���˳�ϵͳ��", "ȷ��    ȡ��"};
    int iHot = 1;

    pos.X = strlen(pCh[0]) + 6;
    pos.Y = 7;
    rcPop.Left = (SCR_COL - pos.X) / 2;
    rcPop.Right = rcPop.Left + pos.X - 1;
    rcPop.Top = (SCR_ROW - pos.Y) / 2;
    rcPop.Bottom = rcPop.Top + pos.Y - 1;

    att = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED;  /*�׵׺���*/
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
 �������ƣ�InsertAssort
 �������ܣ���ʮ�������в���һ����Ȼ�ֺ�������Ϣ���
 ���������phd����ͷָ�룬pAssortָ����Ҫ�������ָ��
 ����ֵ��BOOL���ͣ�TRUE��ʾ����ɹ�,FALSE��ʾ����ʧ��
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
 �������ƣ�InsertEvent
 �������ܣ���ʮ�������в���һ����Ȼ�ֺ�������Ϣ���
 ���������phd����ͷָ�룬pEventָ����Ҫ�������ָ��
 ����ֵ��BOOL���ͣ�TRUE��ʾ����ɹ�,FALSE��ʾ����ʧ��
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
 �������ƣ�InsertDonate
 �������ܣ���ʮ�������в���һ��������Ϣ���
 ���������phd����ͷָ�룬pDonateָ����Ҫ�������ָ��
 ����ֵ��BOOL���ͣ�TRUE��ʾ����ɹ�,FALSE��ʾ����ʧ��
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
*�������ƣ�HelpTopic
*�������ܣ���������
*�����������
*�����������
*����ֵ��BOOL���ͣ����ܺ����г���ExitSys�ķ���ֵ����ΪFALSE�⣬���������ķ���ֵ����ΪTRUE
*����˵����
*/
BOOL HelpTopic(void)
{
    InitInterface();
    printf("\n\n\n\n\n\n\n                       ��������㲻������������Ű�һ�»س���\n");
    printf("                       ��Ȼ�ֺ���λΪ��Ԫ�����\n");
    printf("                       ֱ�Ӿ�����ʧΪ��Ԫ�����\n");
    printf("                       '_____'���Ϸ�Ϊ��Ȼ�ֺ�������Ϣ\n");
    printf("                       '====='���Ϸ�Ϊ��Ȼ�ֺ��¼���Ϣ\n");
    printf("                       '*****'���Ϸ�Ϊ��Ȼ�ֺ������Ϣ\n");
    getchar();
    InitInterface();
    return TRUE;
}

/**
*�������ƣ�About
*�������ܣ�����
*�����������
*�����������
*����ֵ��BOOL���ͣ����ܺ����г���ExitSys�ķ���ֵ����ΪFALSE�⣬���������ķ���ֵ����ΪTRUE
*����˵����
*/
BOOL About(void)
{
    ClearScreen();
    COORD pos = {30,6};
    SetConsoleCursorPosition(gh_std_out, pos);
    printf("������:CS1508������");
    COORD pos1 = {34,11};
    SetConsoleCursorPosition(gh_std_out, pos1);
    printf("�����������\n");
    getch();
    ClearScreen();
    InitInterface();
    return TRUE;
}

/**
*�������ƣ�ShowAll
*�������ܣ���ʾȫ����Ϣ
*�����������
*�����������
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
        printf("�ֺ����ƣ�%s\t",pAssort->name);
        printf("�ֺ����%c\n",pAssort->type);
        printf("____________________________________________________________\n");
        for(pEvent=pAssort->enext;pEvent!=NULL;pEvent=pEvent->next)
        {
            printf("��Ȼ�ֺ����%c\t",pEvent->type);
            printf("�¼���ţ�%s\t",pEvent->num);
            printf("����ʱ�䣺%s\t",pEvent->time);
            printf("�ֺ����ƣ�%s\n",pEvent->name);
            printf("�ֺ�����%c\t",pEvent->level);
            printf("�����ص㣺%s\t",pEvent->place);
            printf("����������%d��\t",pEvent->dead);
            printf("�񷿵�����%d\n��",pEvent->collapse);
            printf("ֱ�Ӿ�����ʧ��%f��Ԫ�����\n",pEvent->loss);
            printf("===============================================================\n");
            for(pDonate=pEvent->dnext;pDonate!=NULL;pDonate=pDonate->next)
            {
                printf("�¼���ţ�%s\t",pDonate->num);
                printf("������ڣ�%s\n",pDonate->time);
                printf("������%c\t",pDonate->type);
                printf("��������ƣ�%s\t",pDonate->dname);
                printf("�����Ŀ��%f����Ԫ\n",pDonate->money);
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
 �������ƣ�MatchString
 �������ܣ��Ը������ַ�������������ƥ��
 ���������string_item�����ַ�����pcond����ƥ����������ڵ������ַ���
 ����ֵ��ƥ��ɹ�ʱ������TRUE,���򷵻�FALSE
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
 �������ƣ�MatchChar
 �������ܣ��Ը����ַ�����������ƥ��
 ���������char_item�����ַ���pcond����ƥ����������ڵ������ַ���
 ����ֵ��ƥ��ɹ�ʱ������TRUE�����򷵻�FALSE
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
 �������ƣ�DelAssort
 �������ܣ���ʮ��������ɾ��ָ���ķ�����Ϣ���
 �������������ͷָ��phd,�ֺ�����name
 ����ֵ��BOOL���ͣ�TRUE��ʾ�ɹ���FALSE��ʾʧ��
 ����˵���������ֺ�����ȷ��Ψһ���ֺ�������Ϣ
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
 �������ƣ�DelEvent
 �������ܣ���ʮ��������ɾ��ָ�����¼���Ϣ���
 �������������ͷָ��phd,
 ����ֵ��BOOL���ͣ�TRUE��ʾ�ɹ���FALSE��ʾʧ��
 ����˵���������ֺ�����ȷ��Ψһ���¼���Ϣ
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
 �������ƣ�DelDonate
 �������ܣ���ʮ��������ɾ��ָ�����ֺ������Ϣ���
 �������������ͷָ��hd���ֺ�����name�����������dname
 ����ֵ��BOOL���ͣ�TRUE��ʾ�ɹ���FALSE��ʾʧ��
 ����˵���������ֺ����ƺ;��������ȷ��Ψһ�ľ����Ϣ
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
 �������ƣ�ModifAssort
 �������ܣ���ָ���ķ�����Ϣ���������Ϣ�����޸�
 �������������ͷָ��phd���ֺ�����type��pAssortָ�����޸����ݽ���ָ��
 ����ֵ��BOOL����,TRUE��ʾ�޸ĳɹ���FALSE��ʾ�޸�ʧ��
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
 �������ƣ�ModifEvent
 �������ܣ���ָ�����ֺ��¼���Ϣ���������Ϣ�����޸�
 �������������ͷָ��phd���¼���ţ�pEventָ�����޸����ݽ���ָ��
 ����ֵ��BOOL����,TRUE��ʾ�޸ĳɹ���FALSE��ʾ�޸�ʧ��
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
 �������ƣ�ModifDonate
 �������ܣ���ָ���ľ����Ϣ���������Ϣ�����޸�
 �������������ͷָ��phd���¼����num�����������dname��pDonateָ�����޸����ݽ���ָ��
 ����ֵ��BOOL����,TRUE��ʾ�޸ĳɹ���FALSE��ʾ�޸�ʧ��
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
 �������ƣ�SeekAssort1
 �������ܣ����ֺ����Ͳ����ֺ�������Ϣ���
 �������������ͷָ��phd���ֺ�����name
 ����ֵ���ҵ�ʱ���ؽ��ĵ�ַ�����򷵻�NULL
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
 �������ƣ�SeekAssort2
 �������ܣ����ֺ����Ͳ����ֺ�������Ϣ���
 �������������ͷָ��phd���ֺ����type
 ����ֵ���ҵ�ʱ���ؽ��ĵ�ַ�����򷵻�NULL
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
 �������ƣ�SeekEvent
 �������ܣ����ֺ����Ʋ����ֺ������¼���Ϣ���
 �������������ͷָ��phd���¼����num
 ����ֵ���ҵ�ʱ���ؽ��ĵ�ַ�����򷵻�NULL
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
 �������ƣ�SeekEvent2
 �������ܣ����ֺ����Ʋ����ֺ������¼���Ϣ���
 �������������ͷָ��phd���ֺ�����name���ֺ�ʱ������time1,time2
 ����ֵ���ҵ�ʱ���ؽ��ĵ�ַ�����򷵻�NULL
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
 �������ƣ�SeekDonate
 �������ܣ����¼���ź;�����������ַ������Ҿ����Ϣ���
 �������������ͷָ��phd���¼����num�����������dname
 ����ֵ���ҵ�ʱ���ؽ��ĵ�ַ�����򷵻�NULL
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
 �������ƣ�SeekDonate2
 �������ܣ���������;����Ŀ������Ҿ����Ϣ���
 �������������ͷָ��phd���ֺ�����name�����������dname
 ����ֵ���ҵ�ʱ���ؽ��ĵ�ַ�����򷵻�NULL
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
 �������ƣ�JudgeAssortItem
 �������ܣ��ж���Ȼ�ֺ�������Ϣ����Ƿ������������
 ���������pAssort������Ϣ���ָ�룬pcond������ʾ�������ַ���
 ����ֵ����������ʱ������TRUE�����򷵻�FALSE
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
 �������ƣ�JudgeEventItem1
 �������ܣ��ж���Ȼ�ֺ��¼���Ϣ����Ƿ������������
 ���������pEvent�¼���Ϣ���ָ�룬pcond������ʾ�������ַ���
 ����ֵ����������ʱ������TRUE�����򷵻�FALSE
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
 �������ƣ�JudgeEventItem2
 �������ܣ��ж���Ȼ�ֺ��¼���Ϣ����Ƿ������������
 ���������pEvent�¼���Ϣ���ָ�룬pcond������ʾ�������ַ���
 ����ֵ����������ʱ������TRUE�����򷵻�FALSE
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
 �������ƣ�JudgeDonateItem1
 �������ܣ��ж���Ȼ�ֺ��¼���Ϣ����Ƿ������������
 ���������pEvent�����Ϣ���ָ�룬pcond������ʾ�������ַ���
 ����ֵ����������ʱ������TRUE�����򷵻�FALSE
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
 �������ƣ�JudgeDonateItem2
 �������ܣ��ж���Ȼ�ֺ��¼���Ϣ����Ƿ������������
 ���������pEvent�����Ϣ���ָ�룬pcond������ʾ�������ַ���
 ����ֵ����������ʱ������TRUE�����򷵻�FALSE
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
*�������ƣ�MaintainAssort
*�������ܣ�ά����Ȼ�ֺ�������Ϣ
*�����������
*�����������
*����ֵ��TRUE
*����˵����
*/
BOOL MaintainAssort(void)
{
    BOOL bRet = TRUE;
    LABEL_BUNDLE labels;
    HOT_AREA areas;
    SMALL_RECT rcPop;
    COORD pos;
    WORD att;
    char *pCh[] = {"1��ɾ��������Ϣ", "2�����������Ϣ","3���޸ķ�����Ϣ","����"};
    char *pCh2[] = {"�����룺   �ֺ����","��ʼ����","ȡ��"};
    char *pCh3[] = {"�����룺�ֺ���������","��ʼ����","ȡ��"};
    char *pCh4[] = {"�����룺  ԭ�ֺ����     ","�����룺 ","��ʼ����","ȡ��"};
    int iHot = 4;/*�ȼ���������*/
    pos.X = strlen(pCh[0]) +12;
    pos.Y = 14;
    rcPop.Left = (SCR_COL - pos.X) / 2;
    rcPop.Right = rcPop.Left + pos.X - 1;
    rcPop.Top = (25 - pos.Y) / 2;
    rcPop.Bottom = rcPop.Top + pos.Y - 1;
    att = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED;  /*�׵׺���*/
    labels.num = 4;//����
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
    PopUp(&rcPop, att, &labels, &areas);//�γɵ���



    if (DealInput(&areas, &iHot) == 13 && iHot == 1)
    {
        /*ɾ��ĳ������Ϣ*/
        PopOff();
        iHot = 2;
        pos.X = 30;
        pos.Y = 14;
        rcPop.Left = 0;
        rcPop.Right = SCR_COL-1;
        rcPop.Top = (25 - pos.Y) / 2;
        rcPop.Bottom = rcPop.Top + pos.Y - 1;
        att = BACKGROUND_BLUE| BACKGROUND_GREEN | BACKGROUND_RED;  /*�׵׺���*/
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
            /*��ʼִ�в��Һ�ɾ������*/
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
            /*���������Ϣ*/
            PopOff();
            iHot = 2;
            pos.X = 30;
            pos.Y = 14;
            rcPop.Left = 0;
            rcPop.Right = SCR_COL-1;
            rcPop.Top = (25 - pos.Y) / 2;
            rcPop.Bottom = rcPop.Top + pos.Y - 1;
            att = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED;  /*�׵׺���*/
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
                /*��ʼִ�в������*/
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
                /*�޸�ĳ������Ϣ*/
                PopOff();
                iHot = 2;

                pos.X = 30;
                pos.Y = 14;
                rcPop.Left = 0;
                rcPop.Right = SCR_COL-1;
                rcPop.Top = (25 - pos.Y) / 2;
                rcPop.Bottom = rcPop.Top + pos.Y - 1;

                att = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED;  /*�׵׺���*/
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
                    /*��ʼִ�в��Һ��޸Ĳ���*/
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
*�������ƣ�MaintainEvent
*�������ܣ�ά����Ȼ�ֺ��¼�������Ϣ
*�����������
*�����������
*����ֵ��TRUE
*����˵����
*/
BOOL MaintainEvent(void)
{
    BOOL bRet = TRUE;
    LABEL_BUNDLE labels;
    HOT_AREA areas;
    SMALL_RECT rcPop;
    COORD pos;
    WORD att;
    char *pCh[] = {"1��ɾ���ֺ��¼���Ϣ", "2�������ֺ��¼���Ϣ","3���޸��ֺ��¼���Ϣ","����"};
    char *pCh2[] = {"�����룺  �¼����","��ʼ����","ȡ��"};
    char *pCh3[] = {"�������¼���Ϣ ����� ��� ʱ�� ���� ���� �ص� �������� �񷿵��� ������ʧ                  ","                                                ","��ʼ����","ȡ��"};
    char *pCh4[] = {"�����룺  ԭ�¼����","�����������Ϣ����� ��� ʱ�� ���� ���� �ص� �������� ���ݵ��� ������ʧ                                 ","                                                ","��ʼ����","ȡ��"};
    int iHot = 4;

    pos.X = strlen(pCh[0]) +11;
    pos.Y = 14;
    rcPop.Left = (SCR_COL - pos.X) / 2;
    rcPop.Right = rcPop.Left + pos.X - 1;
    rcPop.Top = (25 - pos.Y) / 2;
    rcPop.Bottom = rcPop.Top + pos.Y - 1;

    att = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED;  /*�׵׺���*/
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
        /*ɾ���ֺ��¼���Ϣ*/
        PopOff();
        iHot = 2;

        pos.X = 30;
        pos.Y = 14;
        rcPop.Left = 0;
        rcPop.Right = SCR_COL-1 ;
        rcPop.Top = (25 - pos.Y) / 2;
        rcPop.Bottom = rcPop.Top + pos.Y - 1;

        att = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED;  /*�׵׺���*/
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
            /*��ʼִ�в��Һ�ɾ������*/
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
            /*�����ֺ��¼���Ϣ���*/
            PopOff();
            iHot = 2;

            pos.X = 30;
            pos.Y = 14;
            rcPop.Left = 0;
            rcPop.Right =SCR_COL-1;
            rcPop.Top = (25 - pos.Y) / 2;
            rcPop.Bottom = rcPop.Top + pos.Y - 1;

            att = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED;  /*�׵׺���*/
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
                /*��ʼ���ҺͲ���*/
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
                /*�޸��ֺ��¼���Ϣ*/
                PopOff();
                iHot = 2;

                pos.X = 48;
                pos.Y = 15;
                rcPop.Left = 0;
                rcPop.Right = SCR_COL-1;
                rcPop.Top = (25 - pos.Y) / 2;
                rcPop.Bottom = rcPop.Top + pos.Y - 1;

                att = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED;  /*�׵׺���*/
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
                    /*��ʼ���Һ��޸�*/
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
*�������ƣ�MaintainDonate
*�������ܣ�ά���ֺ������Ϣ
*�����������
*�����������
*����ֵ��TRUE
*����˵����
*/
BOOL MaintainDonate(void)
{
    BOOL bRet = TRUE;
    LABEL_BUNDLE labels;
    HOT_AREA areas;
    SMALL_RECT rcPop;
    COORD pos;
    WORD att;
    char *pCh[] = {"1��ɾ�������Ϣ", "2����������Ϣ","3���޸ľ����Ϣ","����"};
    char *pCh2[] = {"�����룺  �¼���ź;��������","��ʼ����","ȡ��"};
    char *pCh3[] = {"����������Ϣ ���¼���� ������� ������ ��������� �����Ŀ                  ","                                                ","��ʼ����","ȡ��"};
    char *pCh4[] = {"�����룺  ԭ�¼����","�����������Ϣ�����ʱ�� ��� ��������� �����Ŀ                                 ","                                                ","��ʼ����","ȡ��"};

    int iHot = 4;
    pos.X = strlen(pCh[0]) +12;
    pos.Y = 14;
    rcPop.Left = (SCR_COL - pos.X) / 2;
    rcPop.Right = rcPop.Left + pos.X - 1;
    rcPop.Top = (25 - pos.Y) / 2;
    rcPop.Bottom = rcPop.Top + pos.Y - 1;
    att = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED;  /*�׵׺���*/
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
        /*ɾ�������Ϣ*/
        PopOff();
        iHot = 2;
        pos.X = 30;
        pos.Y = 14;
        rcPop.Left = 0;
        rcPop.Right = SCR_COL-1;
        rcPop.Top = (25 - pos.Y) / 2;
        rcPop.Bottom = rcPop.Top + pos.Y - 1;
        att = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED;  /*�׵׺���*/
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
            /*��ʼ���Һ�ɾ��*/
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
            /*��������Ϣ*/
            PopOff();
            iHot = 2;
            pos.X = 30;
            pos.Y = 14;
            rcPop.Left = 0;
            rcPop.Right = SCR_COL-1;
            rcPop.Top = (25 - pos.Y) / 2;
            rcPop.Bottom = rcPop.Top + pos.Y - 1;
            att = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED;  /*�׵׺���*/
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
                /*��ʼ���ҺͲ���*/
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
                att = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED;  /*�׵׺���*/
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
                    /*��ʼ���Һ��޸�*/
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
*�������ƣ�QueryAssort
*�������ܣ���ѯ��Ȼ�ֺ�������Ϣ
*�����������
*�����������
*����ֵ��TRUE
*����˵����
*/
BOOL QueryAssort(void)
{
    BOOL bRet = TRUE;
    LABEL_BUNDLE labels;
    HOT_AREA areas;
    SMALL_RECT rcPop;
    COORD pos;
    WORD att;
    char *pCh[] = {"�����룺 �ֺ������Ӵ�", "��ʼ����","����"};
    int iHot = 2;

    pos.X = 30;
    pos.Y = 14;
    rcPop.Left = 0;
    rcPop.Right = SCR_COL-1;
    rcPop.Top = (25 - pos.Y) / 2;
    rcPop.Bottom = rcPop.Top + pos.Y - 1;
    att = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED;  /*�׵׺���*/
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
            printf("�ֺ����ƣ�%s\n",pAssort->name);
            printf("�ֺ����%c\n",pAssort->type);
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
*�������ƣ�QueryEvent
*�������ܣ���ѯ��Ȼ�ֺ��¼���Ϣ
*�����������
*�����������
*����ֵ��TRUE
*����˵����
*/
BOOL QueryEvent(void)
{
    BOOL bRet = TRUE;
    LABEL_BUNDLE labels;
    HOT_AREA areas;
    SMALL_RECT rcPop;
    COORD pos;
    WORD att;
    char *pCh[] = {"1�����¼����","2����ʱ�䷶Χ������", "����"};
    char *pCh2[] = {"�����룺�¼����","��ʼ����","ȡ��"};
    char *pCh3[] = {"ʱ�䷶Χ����ʱ������˳��","�ֺ�����","��ʼ����","ȡ��"};
    int iHot = 3;

    pos.X = strlen(pCh[0]) +10;
    pos.Y = 12;
    rcPop.Left = (SCR_COL - pos.X) / 2;
    rcPop.Right = rcPop.Left + pos.X - 1;
    rcPop.Top = (25 - pos.Y) / 2;
    rcPop.Bottom = rcPop.Top + pos.Y - 1;

    att = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED;  /*�׵׺���*/
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
        att = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED;  /*�׵׺���*/
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
                printf("��Ȼ�ֺ����%c\n",pEvent->type);
                printf("�¼���ţ�%s\n",pEvent->num);
                printf("����ʱ�䣺%s\n",pEvent->time);
                printf("�ֺ����ƣ�%s\n",pEvent->name);
                printf("�ֺ�����%c\n",pEvent->level);
                printf("�����ص㣺%s\n",pEvent->place);
                printf("����������%d��\n",pEvent->dead);
                printf("�񷿵�����%d\n��",pEvent->collapse);
                printf("ֱ�Ӿ�����ʧ��%f��Ԫ�����",pEvent->loss);
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

            att = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED;  /*�׵׺���*/
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
                    printf("��Ȼ�ֺ����%c\n",pEvent2->type);
                    printf("�¼���ţ�%s\n",pEvent2->num);
                    printf("����ʱ�䣺%s\n",pEvent2->time);
                    printf("�ֺ����ƣ�%s\n",pEvent2->name);
                    printf("�ֺ�����%c\n",pEvent2->level);
                    printf("�����ص㣺%s\n",pEvent2->place);
                    printf("����������%d��\n",pEvent2->dead);
                    printf("�񷿵�����%d\n��",pEvent2->collapse);
                    printf("ֱ�Ӿ�����ʧ��%f��Ԫ�����",pEvent2->loss);
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
*�������ƣ�QueryDonate
*�������ܣ���ѯ��Ȼ�ֺ������Ϣ
*�����������
*�����������
*����ֵ��TRUE
*����˵����
*/
BOOL QueryDonate(void)
{
    BOOL bRet = TRUE;
    LABEL_BUNDLE labels;
    HOT_AREA areas;
    SMALL_RECT rcPop;
    COORD pos;
    WORD att;
    char *pCh[] = {"1�����¼���ż�����������Ӵ�","2����������;����Ŀ", "����"};
    char *pCh2[] = {"�����룺�¼���ź;���������Ӵ�","��ʼ����","ȡ��"};
    char *pCh3[] = {"���������","�����Ŀ����","��ʼ����","ȡ��"};
    int iHot = 3;

    pos.X = strlen(pCh[0]) +10;
    pos.Y = 12;
    rcPop.Left = (SCR_COL - pos.X) / 2;
    rcPop.Right = rcPop.Left + pos.X - 1;
    rcPop.Top = (25 - pos.Y) / 2;
    rcPop.Bottom = rcPop.Top + pos.Y - 1;

    att = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED;  /*�׵׺���*/
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
        att = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED;  /*�׵׺���*/
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
                printf("�¼���ţ�%s\n",pDonate->num);
                printf("������ڣ�%s\n",pDonate->time);
                printf("������%c\n",pDonate->type);
                printf("��������ƣ�%s\n",pDonate->dname);
                printf("�����Ŀ��%f����Ԫ\n",pDonate->money);
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

            att = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED;  /*�׵׺���*/
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
                    printf("�¼���ţ�%s\n",pDonate2->num);
                    printf("������ڣ�%s\n",pDonate2->time);
                    printf("������%c\n",pDonate2->type);
                    printf("��������ƣ�%s\n",pDonate2->dname);
                    printf("�����Ŀ��%f����Ԫ\n",pDonate2->money);
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
*�������ƣ�StatType
*�������ܣ����ֺ����ͳ��
*�����������
*�����������
*����ֵ��TRUE
*����˵����
*/
BOOL StatType(void)
{
    STAT *pStat=(STAT *)malloc(sizeof(STAT));		// �ֺ��ڲ��¼� 
    STAT *Mid=(STAT *)malloc(sizeof(STAT));
    STAT *phd=pStat;
    STAT *phead=phd;
    ASSORT *pAssort;     //�ֺ����� 
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
        printf("����������%d\t",phead->happen);
        printf("������������%d\n",phead->dead);
        printf("���ݵ���������%d\t",phead->collapse);
        printf("ֱ�Ӿ�����ʧ�ܶ%f��Ԫ\n",phead->loss);
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
*�������ƣ�StatName
*�������ܣ����ֺ�����ͳ��
*�����������
*�����������QueryEvent
*����ֵ��TRUE
*����˵����
*/
BOOL StatName(void)
{
    BOOL bRet = TRUE;
    LABEL_BUNDLE labels;
    HOT_AREA areas;
    SMALL_RECT rcPop;
    COORD pos;
    WORD att;
    char *pCh[] = {"�����룺 ���", "��ʼ����","����"};
    int iHot = 2;

    pos.X = 30;
    pos.Y = 14;
    rcPop.Left = 0;
    rcPop.Right = SCR_COL-1;
    rcPop.Top = (25 - pos.Y) / 2;
    rcPop.Bottom = rcPop.Top + pos.Y - 1;
    att = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED;  /*�׵׺���*/
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
            printf("����������%d\t",phead->happen);
            printf("������������%d\n",phead->dead);
            printf("���ݵ���������%d\t",phead->collapse);
            printf("ֱ�Ӿ�����ʧ�ܶ%f��Ԫ\n",phead->loss);
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
*�������ƣ�StatYear
*�������ܣ�����ݺ��ֺ�����ͳ��ĳ�ֺ�����
*�����������
*�����������
*����ֵ��TRUE
*����˵����
*/
BOOL StatYear(void)
{
    BOOL bRet = TRUE;
    LABEL_BUNDLE labels;
    HOT_AREA areas;
    SMALL_RECT rcPop;
    COORD pos;
    WORD att;
    char *pCh[] = {"�����룺 �ֺ��������", "��ʼ����","����"};

    int iHot = 2;
    pos.X = 30;
    pos.Y = 14;
    rcPop.Left = 0;
    rcPop.Right = SCR_COL-1;
    rcPop.Top = (25 - pos.Y) / 2;
    rcPop.Bottom = rcPop.Top + pos.Y - 1;
    att = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED;  /*�׵׺���*/
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
            printf("����������%d\t",phead->happen);
            printf("������������%d\n",phead->dead);
            printf("���ݵ���������%d\t",phead->collapse);
            printf("ֱ�Ӿ�����ʧ�ܶ%f��Ԫ\n",phead->loss);
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
*�������ƣ�StatDate
*�������ܣ����������ͳ��
*�����������
*�����������
*����ֵ��TRUE
*����˵����
*/
BOOL StatDate(void)
{
    BOOL bRet = TRUE;
    LABEL_BUNDLE labels;
    HOT_AREA areas;
    SMALL_RECT rcPop;
    COORD pos;
    WORD att;
    char *pCh[] = {"�����룺 ����ʱ���", "��ʼ����","����"};
    int iHot = 2;

    pos.X = 30;
    pos.Y = 14;
    rcPop.Left = 0;
    rcPop.Right = SCR_COL-1;
    rcPop.Top = (25 - pos.Y) / 2;
    rcPop.Bottom = rcPop.Top + pos.Y - 1;
    att = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED;  /*�׵׺���*/
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
        printf("���˾��������%f��Ԫ\n�������������%f��Ԫ\n",donate1,donate2);
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
*�������ƣ�StatRank
*�������ܣ�����Ȼ�ֺ��ȼ�ͳ��
*�����������
*�����������
*����ֵ��TRUE
*����˵����
*/
BOOL StatRank(void)
{
    BOOL bRet = TRUE;
    LABEL_BUNDLE labels;
    HOT_AREA areas;
    SMALL_RECT rcPop;
    COORD pos;
    WORD att;
    char *pCh[] = {"�����룺��Ȼ�ֺ��ȼ�", "��ʼ����","����"};
    int iHot = 2;

    pos.X = 30;
    pos.Y = 14;
    rcPop.Left = 0;
    rcPop.Right = SCR_COL-1;
    rcPop.Top = (25 - pos.Y) / 2;
    rcPop.Bottom = rcPop.Top + pos.Y - 1;
    att = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED;  /*�׵׺���*/
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
        printf("��Ȼ�ֺ��ȼ�Ϊ%c���ܾ����ĿΪ%f��Ԫ\n",Rank,donate);
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
















