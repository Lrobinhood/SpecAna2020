/*********************************************************************




        File:       typedef.h

        Author:     Robin
        Date:       2017-06-06


**********************************************************************/

#ifndef __TYPEDEF_H__
#define __TYPEDEF_H__

typedef unsigned int u32;
typedef unsigned short u16;	/*!< 2 bytes, range: 0 to 65536 */
typedef unsigned char u8;

typedef u8 BOOLEAN;


#ifndef TRUE
#define TRUE    1
#endif

#ifndef FALSE
#define FALSE   0
#endif


#define U32TO_LITTLE_END(a) (((a&0xff000000) >> 24) | ((a&0xff0000) >> 8) | ((a&0xff00) << 8) | ((a&0xff) << 24))

#endif  // #ifndef __TYPEDEF_H__