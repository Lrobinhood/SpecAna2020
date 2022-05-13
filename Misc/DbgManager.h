/******************************************************************************************



	author:	Robin
	date:	2013-9-27

******************************************************************************************/

#ifndef __DBG_MANAGER_H__
#define __DBG_MANAGER_H__

void dbg_show_mem(u8 *pMem, u32 u32MemLen, u32 u32DataOneLine = 16);

unsigned int
xcrc32 (const unsigned char *buf, int len, unsigned int init);

#endif