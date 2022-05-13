/******************************************************************************************



    Author: Robin
    Date:   2021-09-02

******************************************************************************************/
#include "../AnaBase/AnaBase.h"

#include "../Filter/TsPackageFilter.h"
#include "../Filter/PidFilter.h"
#include "../Filter/PesFilter.h"

#include "./audio/techtype.h"

extern BOOLEAN AC4ParsePesHeader(U8BIT *pes_ptr);

void ParseAC4(u8* pBuf, u32 u32DataLen)
{
    AC4ParsePesHeader(pBuf);
}

void UT_RAW_PES_Filter(const char *TsFile)
{
    {
    u8 tsPackBuf[188];

    //const char* TsFileName = "D:\\StreamSparkTS\\dvbc\\17-Germany-450.ts";

    CFileStream fileStream(TsFile);

    CRoundBuf roundBuf(1024);

    CTsPackageFilter tsFilter;


    size_t fileOffset = 0;

    roundBuf.ConnectFileSource(&fileStream);

    tsFilter.SetRoundBuf(&roundBuf);

    CPidFilter pidFilter_0x40;

    pidFilter_0x40.SetPid(0x40);
    pidFilter_0x40.EnableFilter();

    tsFilter.RegisterNotify(0x40, &pidFilter_0x40);

    CPesFilter pesFilter;

    pesFilter.SetDebug(true);
    pesFilter.EnableFilter();
    pesFilter.SetParserFunc(ParseAC4);

    pidFilter_0x40.RegisterNotify((void *)(&pesFilter), pesFilter);


    while (0!= tsFilter.GetTsPackageEx(tsPackBuf, fileOffset))
    {
        ;
    }

    printf("\n");

}

}