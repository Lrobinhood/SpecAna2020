/******************************************************************************************



    Author: Robin
    Date:   2020-10-10

******************************************************************************************/
#include "../AnaBase/AnaBase.h"

#include "../Filter/TsPackageFilter.h"
#include "../Filter/PidFilter.h"
#include "../Filter/SectionFilter.h"

#include "../Parser/ParserFunc.h"

void assistant_PidFilter_2_SectionFilter_2_ParserEx(const char* TsFile, u16 pid, u8 tid, u16 xtid = 0xffff);

//---B: TsPackageFilter ---
void UT_TsPackageFIlter_GetTsPackage()
{
    int i = 0;

    u8 tsPackBuf[188];

    CFileStream fileStream("../../../../TestStream/Infocsat_scrambled_2018-12-03.ts");

    CRoundBuf roundBuf(1024);

    CTsPackageFilter tsFilter;

    roundBuf.ConnectFileSource(&fileStream);

    tsFilter.SetRoundBuf(&roundBuf);

    while (0!= tsFilter.GetTsPackage(tsPackBuf))
    {
       if (i < 10)
           printf("TS Package's starting 4 bytes: %2x %2x %2x %2x \n", tsPackBuf[0], tsPackBuf[1], tsPackBuf[2], tsPackBuf[3]);

    }

    printf("\n");

}

//---E: TsPackageFilter ---


//---B: PidFilter ---
void UT_PidFilter_ReceiveTSPack()
{
    u8 tsPackBuf[188];

    CFileStream fileStream("../../../../TestStream/Infocsat_scrambled_2018-12-03.ts");

    CRoundBuf roundBuf(1024);

    CTsPackageFilter tsFilter;

    CPidFilter pidFilter;

    size_t fileOffset = 0;

    roundBuf.ConnectFileSource(&fileStream);

    tsFilter.SetRoundBuf(&roundBuf);

    pidFilter.SetPid(0x11);
    pidFilter.SetDebug(true);
    pidFilter.SetRecord(true);

    while (0!= tsFilter.GetTsPackageEx(tsPackBuf, fileOffset))
    {
        pidFilter.ReceiveTSPack(tsPackBuf, fileOffset);
    }

    printf("\n");

}


void UT_TsPackageFilter_2_PidFilter()
{
    u8 tsPackBuf[188];

    CFileStream fileStream("../../../../TestStream/Infocsat_scrambled_2018-12-03.ts");

    CRoundBuf roundBuf(1024);

    CTsPackageFilter tsFilter;


    size_t fileOffset = 0;

    roundBuf.ConnectFileSource(&fileStream);

    tsFilter.SetRoundBuf(&roundBuf);

    CPidFilter pidFilter_0x11;
    CPidFilter pidFilter_0x42;

    pidFilter_0x11.SetPid(0x11);
    //pidFilter_0x11.SetDebug(true);
    pidFilter_0x11.SetRecord(true);
    pidFilter_0x11.EnableFilter();

    tsFilter.RegisterNotify(0x11, &pidFilter_0x11);

    pidFilter_0x42.SetPid(0);
    //pidFilter_0x42.SetDebug(true);
    pidFilter_0x42.SetRecord(true);
    pidFilter_0x42.EnableFilter();

    tsFilter.RegisterNotify(0, &pidFilter_0x42);

    while (0!= tsFilter.GetTsPackageEx(tsPackBuf, fileOffset))
    {
        ;
    }

    printf("\n");

}


//---E: PidFilter ---


//---B: SectionFilter ---
void UT_PidFilter_2_SectionFilter()
{
    u8 tsPackBuf[188];

    CFileStream fileStream("../../../../TestStream/Infocsat_scrambled_2018-12-03.ts");

    CRoundBuf roundBuf(1024);

    CTsPackageFilter tsFilter;


    size_t fileOffset = 0;

    roundBuf.ConnectFileSource(&fileStream);

    tsFilter.SetRoundBuf(&roundBuf);

    CPidFilter pidFilter_0x11;

    pidFilter_0x11.SetPid(0x11);
    //pidFilter_0x11.SetDebug(true);
    pidFilter_0x11.SetRecord(true);
    pidFilter_0x11.EnableFilter();

    tsFilter.RegisterNotify(0x11, &pidFilter_0x11);

    CSectionFilter secFilter_0x42;

    secFilter_0x42.SetFilterTableId(0x42);
    secFilter_0x42.SetDebug(true);
    secFilter_0x42.EnableFilter();

    pidFilter_0x11.RegisterNotify((void *)(&secFilter_0x42), secFilter_0x42);


    while (0!= tsFilter.GetTsPackageEx(tsPackBuf, fileOffset))
    {
        ;
    }

    printf("\n");

}



void UT_PidFilter_2_SectionFilter_2_Parser()
{
    u8 tsPackBuf[188];

    CFileStream fileStream("D:\\StreamSparkTS\\dvbc\\Netherlands_DVBC_250.8M_20150526_153053.ts");

    CRoundBuf roundBuf(1024);

    CTsPackageFilter tsFilter;


    size_t fileOffset = 0;

    roundBuf.ConnectFileSource(&fileStream);

    tsFilter.SetRoundBuf(&roundBuf);

    CPidFilter pidFilter_0x11;

    pidFilter_0x11.SetPid(0x11);
    //pidFilter_0x11.SetDebug(true);
    pidFilter_0x11.SetRecord(true);
    pidFilter_0x11.EnableFilter();

    tsFilter.RegisterNotify(0x11, &pidFilter_0x11);

    CSectionFilter secFilter_0x42;

    secFilter_0x42.SetFilterTableId(0x42);
    secFilter_0x42.SetDebug(true);
    secFilter_0x42.EnableFilter();
    secFilter_0x42.SetParserFunc(SDT_Parser);


    pidFilter_0x11.RegisterNotify((void *)(&secFilter_0x42), secFilter_0x42);


    while (0!= tsFilter.GetTsPackageEx(tsPackBuf, fileOffset))
    {
        ;
    }

    printf("\n");

}


//---E: SectionFilter ---


bool isPcrPack(u8* tsPack)
{
    if (((tsPack[3] & 0x30) == 0x20) || ((tsPack[3] & 0x30) == 0x30))
    {
        if (tsPack[4] > 0)
        {
            if ((tsPack[5] & 0x10) == 0x10)
            {
                return true;
            }
        }
    }

    return false;
}



void UT_FindPCR()
{
    int i = 0;
    u8 pcr_data[6];
    long long pcr, pcr_base, pcr_ext;

    u8 tsPackBuf[188];
    size_t tsPackOffset;

    CFileStream fileStream("D:\\StreamSparkTS\\dvbc\\Netherlands_DVBC_250.8M_20150526_153053.ts");

    CRoundBuf roundBuf(1024);

    CTsPackageFilter tsFilter;

    roundBuf.ConnectFileSource(&fileStream);

    tsFilter.SetRoundBuf(&roundBuf);

    u32 count = 0;
    u32 pid = 0;

    int pid_ref = -1;
    long long pcr1 = 0, pcr2 = 0;
    size_t ts_offset1 = 0;
    size_t ts_offset2 = 0;
    double ts_rate = 0;


    while (0!= tsFilter.GetTsPackageEx(tsPackBuf, tsPackOffset))
    {
        // DBG
        //printf("ts pack:%u \n", count++);

        pid = ((tsPackBuf[1] & 0x1F) << 8) + tsPackBuf[2];

        if (isPcrPack(tsPackBuf))
        {
            for (int i = 0; i < 6; i++)
            {
                pcr_data[i] = tsPackBuf[6 + i];
            }

            pcr_base = (((long long)(pcr_data[0])) << 25) + (((long long)(pcr_data[1])) << 17) + (((long long)(pcr_data[2])) << 9) + (((long long)(pcr_data[3])) << 1) + (pcr_data[4] >> 7);
            pcr_ext  = (((long long)(pcr_data[4]) & 0x01) << 8) + (long long)(pcr_data[5]);

            pcr = pcr_base * 300 + pcr_ext;

            if (-1 == pid_ref)
            {
                pid_ref = pid;
                pcr1 = pcr;
                ts_offset1 = tsPackOffset;
            }
            else if (pid == pid_ref)
            {
                pcr2 = pcr;
                ts_offset2 = tsPackOffset;

                break;
            }

            // DBG
            //printf("0x%x 0x%x 0x%x 0x%x 0x%x 0x%x \n", pcr_data[0], pcr_data[1], pcr_data[2], pcr_data[3], pcr_data[4], pcr_data[5]);
            //printf("pid:0x%x, pcr:%llu, pcr_base:%llu, pcr_ext:%llu, offset:%lu \n", pid, pcr, pcr_base, pcr_ext, tsPackOffset);
        }
    }

    if (0 != pcr1 && 0 != pcr2)
    {
        ts_rate = double((long long)(ts_offset2 - ts_offset1) * (long long)(27000000)) / (double)(pcr2 - pcr1);
    }

    printf("\nts_rate:%f \n", ts_rate);
}


double GetTsRate(const char* TsFileName)
{
    int i = 0;
    u8 pcr_data[6];
    long long pcr, pcr_base, pcr_ext;

    u8 tsPackBuf[188];
    size_t tsPackOffset;

    CFileStream fileStream(TsFileName);

    CRoundBuf roundBuf(1024);

    CTsPackageFilter tsFilter;

    roundBuf.ConnectFileSource(&fileStream);

    tsFilter.SetRoundBuf(&roundBuf);

    u32 count = 0;
    u32 pid = 0;

    int pid_ref = -1;
    long long pcr1 = 0, pcr2 = 0;
    size_t ts_offset1 = 0;
    size_t ts_offset2 = 0;
    double ts_rate = 0;


    while (0!= tsFilter.GetTsPackageEx(tsPackBuf, tsPackOffset))
    {
        // DBG
        //printf("ts pack:%u \n", count++);

        pid = ((tsPackBuf[1] & 0x1F) << 8) + tsPackBuf[2];

        if (isPcrPack(tsPackBuf))
        {
            for (int i = 0; i < 6; i++)
            {
                pcr_data[i] = tsPackBuf[6 + i];
            }

            pcr_base = (((long long)(pcr_data[0])) << 25) + (((long long)(pcr_data[1])) << 17) + (((long long)(pcr_data[2])) << 9) + (((long long)(pcr_data[3])) << 1) + (pcr_data[4] >> 7);
            pcr_ext  = (((long long)(pcr_data[4]) & 0x01) << 8) + (long long)(pcr_data[5]);

            pcr = pcr_base * 300 + pcr_ext;

            if (-1 == pid_ref)
            {
                pid_ref = pid;
                pcr1 = pcr;
                ts_offset1 = tsPackOffset;
            }
            else if (pid == pid_ref)
            {
                pcr2 = pcr;
                ts_offset2 = tsPackOffset;

                break;
            }

            // DBG
            //printf("0x%x 0x%x 0x%x 0x%x 0x%x 0x%x \n", pcr_data[0], pcr_data[1], pcr_data[2], pcr_data[3], pcr_data[4], pcr_data[5]);
            //printf("pid:0x%x, pcr:%llu, pcr_base:%llu, pcr_ext:%llu, offset:%lu \n", pid, pcr, pcr_base, pcr_ext, tsPackOffset);
        }
    }

    if (0 != pcr1 && 0 != pcr2)
    {
        ts_rate = double((long long)(ts_offset2 - ts_offset1) * (long long)(27000000)) / (double)(pcr2 - pcr1);
    }

    printf("\nts_rate:%f \n", ts_rate);

    return ts_rate;
}



void UT_GetNitTime()
{
    u8 tsPackBuf[188];
    size_t tsPackOffset;

    const char* TsFileName = "D:\\StreamSparkTS\\dvbc\\17-Germany-450.ts";
    //const char* TsFileName = "D:\\StreamSparkTS\\dvbc\\Netherlands_DVBC_250.8M_20150526_153053.ts";
    //const char* TsFileName = "D:\\StreamSparkTS\\BBC MUX UH.ts";
    double ts_rate = GetTsRate(TsFileName);

    CFileStream fileStream(TsFileName);

    CRoundBuf roundBuf(1024);

    CTsPackageFilter tsFilter;

    roundBuf.ConnectFileSource(&fileStream);

    tsFilter.SetRoundBuf(&roundBuf);


    u32 idx = 0;
    u32 pid = 0;

    double cur_time = 0;
    double pre_time = 0;
    double interval = 0;
    double max_interval  = 0;
    u32 max_interval_idx = 0;
    while (0 != tsFilter.GetTsPackageEx(tsPackBuf, tsPackOffset))
    {
        pid = ((tsPackBuf[1] & 0x1F) << 8) + tsPackBuf[2];

        if (pid == 0x10)
        {
            cur_time = (double)tsPackOffset * 1000 / ts_rate;

            if (pre_time < 0.001 && pre_time > -0.001)
            {
                interval = 0;
                pre_time = cur_time;
            }
            else
            {
                interval = cur_time - pre_time;
                pre_time = cur_time;
            }

            if (interval > max_interval)
            {
                max_interval = interval;
                max_interval_idx = idx;
            }

            printf("Count:%u, Time:%f, interval:%f, tsPackOffset:%zu \n", idx++, cur_time, interval, tsPackOffset);
        }
    }

    printf("idx:%u, max_interval:%f \n", max_interval_idx, max_interval);
}

// B: UT_GetTsRate()
typedef struct {
    long long pcr1;
    size_t offset1;
    long long pcr2;
    size_t offset2;
    u8 flag;
}pcr_rec;

#include <map>
#include <vector>
#include <algorithm>
#include <numeric>      // std::accumulate

double calc_mean(vector<double>& data_vec)
{
    double sum = std::accumulate(std::begin(data_vec), std::end(data_vec), 0.0);
	double mean =  sum / data_vec.size();

    return mean;
}

double calc_variance(vector<double>& data_vec)
{
    double accum  = 0.0;
    double mean = calc_mean(data_vec);

	std::for_each (std::begin(data_vec), std::end(data_vec), [&](const double d) {
		accum  += (d-mean)*(d-mean);
	});

	double variance = sqrt(accum/(data_vec.size()-1)); //·½²î

    return variance;
}

void UT_GetTsRate()
{
    map <u32, pcr_rec> pid_pcr_map;
    map <u32, pcr_rec>::reverse_iterator iter;


    pcr_rec tmp_pcr_rec;

    int i = 0;
    u8 pcr_data[6];
    long long pcr, pcr_base, pcr_ext;

    u8 tsPackBuf[188];
    size_t tsPackOffset;

    CFileStream fileStream("D:\\StreamSparkTS\\dvbc\\Netherlands_DVBC_250.8M_20150526_153053.ts");

    CRoundBuf roundBuf(1024);

    CTsPackageFilter tsFilter;

    roundBuf.ConnectFileSource(&fileStream);

    tsFilter.SetRoundBuf(&roundBuf);

    u32 count = 0;
    u32 pid = 0;

    int pid_ref = -1;
    long long pcr1 = 0, pcr2 = 0;
    size_t ts_offset1 = 0;
    size_t ts_offset2 = 0;
    double ts_rate = 0;


    while (0!= tsFilter.GetTsPackageEx(tsPackBuf, tsPackOffset))
    {
        // DBG
        //printf("ts pack:%u \n", count++);

        pid = ((tsPackBuf[1] & 0x1F) << 8) + tsPackBuf[2];

        if (isPcrPack(tsPackBuf))
        {
            for (int i = 0; i < 6; i++)
            {
                pcr_data[i] = tsPackBuf[6 + i];
            }

            pcr_base = (((long long)(pcr_data[0])) << 25) + (((long long)(pcr_data[1])) << 17) + (((long long)(pcr_data[2])) << 9) + (((long long)(pcr_data[3])) << 1) + (pcr_data[4] >> 7);
            pcr_ext  = (((long long)(pcr_data[4]) & 0x01) << 8) + (long long)(pcr_data[5]);

            pcr = pcr_base * 300 + pcr_ext;

            if (pid_pcr_map.count(pid) <= 0)
            {
                tmp_pcr_rec.flag = 0;

                tmp_pcr_rec.pcr1 = pcr;
                tmp_pcr_rec.offset1 = tsPackOffset;

                pid_pcr_map.insert(pair<u32, pcr_rec>(pid, tmp_pcr_rec));
            }
            else
            {
                if (0 == pid_pcr_map[pid].flag)
                {
                    pid_pcr_map[pid].pcr2 = pcr;
                    pid_pcr_map[pid].offset2 = tsPackOffset;
                    pid_pcr_map[pid].flag = 1;
                }
            }

            // DBG
            //printf("0x%x 0x%x 0x%x 0x%x 0x%x 0x%x \n", pcr_data[0], pcr_data[1], pcr_data[2], pcr_data[3], pcr_data[4], pcr_data[5]);
            //printf("pid:0x%x, pcr:%llu, pcr_base:%llu, pcr_ext:%llu, offset:%lu \n", pid, pcr, pcr_base, pcr_ext, tsPackOffset);
        }
    }

    vector <double> vec_ts_rate;

    for (iter = pid_pcr_map.rbegin(); iter != pid_pcr_map.rend(); iter++)
    {
        ts_rate = double((long long)(iter->second.offset2 - iter->second.offset1) * (long long)(27000000)) / (double)(iter->second.pcr2 - iter->second.pcr1);

        printf("\n pid:0x%x, ts_rate:%f \n", iter->first, ts_rate);

        vec_ts_rate.push_back(ts_rate);
    }

    double mean = calc_mean(vec_ts_rate);
    double variance = calc_variance(vec_ts_rate);

    printf("\n");
    printf("ts_rate mean:%f, variance:%f, %f%% \n", mean, variance, variance * 100 / mean);

}


// E: UT_GetTsRate()



void UT_PidFilter_2_SectionFilter_2_ParserEx(const char *TsFile)
{
    u8 tsPackBuf[188];

    //const char* TsFileName = "D:\\StreamSparkTS\\dvbc\\17-Germany-450.ts";
    double ts_rate = GetTsRate(TsFile);

    CFileStream fileStream(TsFile);

    CRoundBuf roundBuf(1024);

    CTsPackageFilter tsFilter;


    size_t fileOffset = 0;

    roundBuf.ConnectFileSource(&fileStream);

    tsFilter.SetRoundBuf(&roundBuf);

    CPidFilter pidFilter_0x10;

    pidFilter_0x10.SetPid(0x10);
    pidFilter_0x10.EnableFilter();

    tsFilter.RegisterNotify(0x10, &pidFilter_0x10);

    CSectionFilter secFilter_0x40;

    Common_Set_TsRate(ts_rate);
    secFilter_0x40.SetFilterTableId(0x40);
    //secFilter_0x42.SetDebug(true);
    secFilter_0x40.EnableFilter();
    secFilter_0x40.SetParserFuncEx(Common_Table_ParserEx);


    pidFilter_0x10.RegisterNotify((void *)(&secFilter_0x40), secFilter_0x40);


    while (0!= tsFilter.GetTsPackageEx(tsPackBuf, fileOffset))
    {
        ;
    }

    printf("\n");

}

void UT_SDT_Actual_Table_Statistics(const char *TsFile)
{
    Common_Set_Tbl_Parser(SDT_Parser);
    assistant_PidFilter_2_SectionFilter_2_ParserEx(TsFile, 0x11, 0x42);
}

void UT_NIT_Actual_Table_Statistics(const char *TsFile)
{
    Common_Set_Tbl_Parser(NIT_Parser);
    assistant_PidFilter_2_SectionFilter_2_ParserEx(TsFile, 0x10, 0x40);
}

void UT_EIT_Actual_Table_Statistics(const char *TsFile)
{
    Common_Set_Tbl_Parser(EIT_Parser);
    Common_Table_ParserEx_SetMatchVersion(FALSE);
    assistant_PidFilter_2_SectionFilter_2_ParserEx(TsFile, 0x12, 0x4E);
}

void UT_PAT_Actual_Table_Statistics(const char *TsFile)
{
    Common_Set_Tbl_Parser(PAT_Parser);
    assistant_PidFilter_2_SectionFilter_2_ParserEx(TsFile, 0x00, 0x00);
}

void UT_PMT_Actual_Table_Statistics(const char *TsFile)
{
    u16 pid =  0x3e8;

    Common_Set_Tbl_Parser(PMT_Parser);
    assistant_PidFilter_2_SectionFilter_2_ParserEx(TsFile, pid, 0x02);
}

void UT_BAT_Actual_Table_Statistics(const char *TsFile)
{
    Common_Set_Tbl_Parser(BAT_Parser);
    assistant_PidFilter_2_SectionFilter_2_ParserEx(TsFile, 0x11, 0x4A);
}



// B: Local Function Define
void assistant_PidFilter_2_SectionFilter_2_ParserEx(const char *TsFile, u16 pid, u8 tid, u16 xtid)
{
    u8 tsPackBuf[188];

    //const char* TsFileName = "D:\\StreamSparkTS\\dvbc\\17-Germany-450.ts";
    double ts_rate = GetTsRate(TsFile);

    CFileStream fileStream(TsFile);

    CRoundBuf roundBuf(1024);

    CTsPackageFilter tsFilter;


    size_t fileOffset = 0;

    roundBuf.ConnectFileSource(&fileStream);

    tsFilter.SetRoundBuf(&roundBuf);

    CPidFilter pidFilter;

    pidFilter.SetPid(pid);
    pidFilter.EnableFilter();

    tsFilter.RegisterNotify(pid, &pidFilter);

    CSectionFilter secFilter;

    Common_Set_TsRate(ts_rate);
    Common_Reinit_Max_Interval();

    if (0xffff == xtid)
    {
        secFilter.SetFilterTableId(tid);
    }
    else
    {
        secFilter.SetFilterTableId(tid, 0x00, xtid, 0x0000);
    }
    secFilter.SetDebug(true);
    secFilter.EnableFilter();
    secFilter.SetParserFuncEx(Common_Table_ParserEx);


    pidFilter.RegisterNotify((void *)(&secFilter), secFilter);


    while (0!= tsFilter.GetTsPackageEx(tsPackBuf, fileOffset))
    {
        ;
    }

    u32 max_interval_idx;
    double max_interval;

    Common_Get_Max_Interval(max_interval_idx, max_interval);

    printf("idx:%u, max_interval:%f \n", max_interval_idx, max_interval);

    printf("\n");

}

// E: Local Function Define


