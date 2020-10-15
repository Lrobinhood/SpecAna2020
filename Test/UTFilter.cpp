/******************************************************************************************



	Author:	Robin
	Date:	2020-10-10

******************************************************************************************/
#include "stdafx.h"

#include "../AnaBase/AnaBase.h"

#include "../Filter/TsPackageFilter.h"
#include "../Filter/PidFilter.h"
#include "../Filter./SectionFilter.h"



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

    tsFilter.RegisterNotify(0x11, pidFilter_0x11);

    pidFilter_0x42.SetPid(0);
    //pidFilter_0x42.SetDebug(true);
    pidFilter_0x42.SetRecord(true);
    pidFilter_0x42.EnableFilter();
    
    tsFilter.RegisterNotify(0, pidFilter_0x42);

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

    tsFilter.RegisterNotify(0x11, pidFilter_0x11);

    CSectionFilter secFilter_0x42;
    
    secFilter_0x42.SetFilterTableId(0x42);
    secFilter_0x42.SetDebug(true);
    secFilter_0x42.EnableFilter();
    
    pidFilter_0x11.RegisterNotify((u32)(&secFilter_0x42), secFilter_0x42);

    
	while (0!= tsFilter.GetTsPackageEx(tsPackBuf, fileOffset))
	{
        ;
	}
	
	printf("\n");

}


//---E: SectionFilter ---