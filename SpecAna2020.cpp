// SpecAna2020.cpp : Defines the entry point for the console application.
//


#include "./Test/UT_main.h"

int main(int argc, char* argv[])
{
#if 1
	//const char* TsFileName = "D:\\StreamSparkTS\\Presentation_Selection_APD_Subset_AC4_H265_DVB_50fps.trp";
	//const char* TsFileName = "D:\\StreamSparkTS\\BBC MUX UH.ts";
	//const char* TsFileName = "D:\\StreamSparkTS\\dvbc\\17-Germany-450.ts";

	const char* TsFileName = "D:\\StreamSparkTS\\H264_1080p_25.trp";

	if (2 == argc)
		UT_main(argv[1]);
	else
		UT_main(TsFileName);
#else

#endif

	return 0;
}

