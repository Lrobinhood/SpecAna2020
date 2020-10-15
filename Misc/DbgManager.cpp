/******************************************************************************************



	author:	Robin
	date:	2013-9-27

******************************************************************************************/
#include "stdafx.h"

#include "stdio.h"

#include "../AnaBase/AnaBaseType.h"

#include "DbgManager.h"

void dbg_show_mem(u8 *pMem, u32 u32MemLen, u32 u32DataOneLine)
{
	u8 data;

	printf("Mem Data (Addr:0x%x) HEX:", (u32)pMem);

	for (u32 i = 0; i < u32MemLen; i++)
	{
		if (0 == i%u32DataOneLine) printf("\n");

		data = 	*(pMem + i);

		if (data > 0x0f)
			printf("0x%x, ", data);
		else
			printf("0x0%x, ", data);
	}

	printf("\n");
}

