/******************************************************************************************



	Author:	Robin
	Date:	2020-10-10

******************************************************************************************/

#include <stdio.h>
#include <string>
#include <map>
#include "UT_main.h"

using namespace std;

typedef void (* UT_FUNC)(const char*);
typedef struct {
    string func_name;
    UT_FUNC func_ptr;
}UT_FUNC_REC;

void UT_main(const char *TsFile)
{
    int count;
    int select;
    map <int, UT_FUNC_REC> func_map;

    FILE* fp = NULL;

#define REG_FUNC(func) do { func_map.insert(pair<int, UT_FUNC_REC> (++count, {#func, func})); } while(0)

    count = 0;
    REG_FUNC(UT_SDT_Actual_Table_Statistics);
    REG_FUNC(UT_NIT_Actual_Table_Statistics);
    REG_FUNC(UT_EIT_Actual_Table_Statistics);
    REG_FUNC(UT_PAT_Actual_Table_Statistics);
    REG_FUNC(UT_PMT_Actual_Table_Statistics);
    REG_FUNC(UT_BAT_Actual_Table_Statistics);

    REG_FUNC(UT_PidFilter_2_SectionFilter_2_ParserEx);

    REG_FUNC(UT_RAW_PES_Filter);

    // map <int, UT_FUNC_REC>::iterator iter;

    printf("UT Function List:\n");

    for (count = 1; count <= func_map.size(); count++)
    {
        printf("\t%d) %s\n", count, func_map[count].func_name.c_str());
    }

    printf("\nPlease Input Test No (e.x. input \"1\" will run func -> %s; input \"0\" to End test):\n", func_map[1].func_name.c_str());

    select = 0;
    do
    {
        scanf("%d", &select);

        if (0 < select && select <= count)
        {
            printf("Run Test %d : %s\n", select, func_map[select].func_name.c_str());

            printf("Please Waiting ... ...\n");

            FILE *fp = freopen("result.txt", "w", stdout);
            if (NULL == fp)
            {
                printf("freopen failed\n");
            }

            func_map[select].func_ptr(TsFile);

            printf("Test %d : %s Completed.\n", select, func_map[select].func_name.c_str());

            break;
        }

    } while (0 != select);

    //UT_RecordFile_ReordOneLog();
    //UT_TsPackageFilter_GetTsPackage();
    //UT_PidFilter_ReceiveTSPack();
    //UT_TsPackageFilter_2_PidFilter();
    //UT_PidFilter_2_SectionFilter();
    //UT_PidFilter_2_SectionFilter_2_Parser();
    //UT_FindPCR();
    //UT_GetTsRate();
    //UT_GetNitTime();
    //UT_FieldFactory();

    //UT_PidFilter_2_SectionFilter_2_ParserEx(TsFile);
    //UT_SDT_Actual_Table_Statistics(TsFile);
    //UT_NIT_Actual_Table_Statistics(TsFile);
    //UT_RAW_PES_Filter(TsFile);


    if (NULL != fp)
    {
        fclose(fp);
        fp = NULL;
    }

    freopen("CON","w",stdout);

    printf("Test Completed!\n");
}