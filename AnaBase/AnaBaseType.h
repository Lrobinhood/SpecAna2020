/******************************************************************************************



	author:	Robin
	date:	2013-9-27

******************************************************************************************/

#ifndef __ANABASE_TYPE_H__
#define __ANABASE_TYPE_H__



#ifndef TRUE
#define TRUE	1
#endif

#ifndef FALSE
#define FALSE	0
#endif

#define BIT7 (0x1 << 7)
#define BIT6 (0x1 << 6)
#define BIT5 (0x1 << 5)
#define BIT4 (0x1 << 4)
#define BIT3 (0x1 << 3)
#define BIT2 (0x1 << 2)
#define BIT1 (0x1 << 1)
#define BIT0 (0x1 << 0)

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;



#define U16_LI2B_END(a) (((a&0xff00) >> 8) | ((a&0xff) << 8))
#define U32_LI2B_END(a) (((a&0xff000000) >> 24) | ((a&0xff0000) << 8) | ((a&0xff00) << 8) | ((a&0xff) << 24))


#define M_TS_PACKAGE_SYNC	        (0x47)
#define M_TS_PACKAGE_LEN	        (188)
#define M_TS_PACKAGE_PAYLOAD_LEN	(184)


#if 0

#define M_RET_SUCCESS       0
#define M_RET_FAILED        1

#define M_MODULE_COMMON                   0
#define M_MODULE_COMMON_SUBMODULE         0

#define M_MODULE_TSPU       1

#define M_MODULE_TSPU_COMMON        0
#define M_MODULE_TSPU_PIDFILTER     1
#define M_MODULE_TSPU_SECTIONFILTER 2

#define M_BUILD_MODULE_RETURN_CODE(module, submodule, errcode)  (((module&0xff) << 24) | ((submodule&0xff) << 16) | (errcode&0xffff))
#define M_GET_RETURN_CODE_MODULE(errcode)                       (errcode>>24)
#define M_GET_RETURN_CODE_SUBMODULE(errcode)                    ((errcode>>16)&0xff)
#define M_GET_RETURN_CODE_ERRCODE(errcode)                      (errcode&0xffff)

// #define M_MODULE_COMMON      0
#define M_RET_FAILED_COMMON_NOT_MEMORY          M_BUILD_MODULE_RETURN_CODE(M_MODULE_COMMON, M_MODULE_COMMON_SUBMODULE, 1);

// #define M_MODULE_TSPU        1

#define M_RET_FAILED_NOT_FOUND_PIDFILTER        M_BUILD_MODULE_RETURN_CODE(M_MODULE_TSPU, M_MODULE_TSPU_COMMON, 1);
#define M_RET_FAILED_NOT_FOUND_SECFILTER        M_BUILD_MODULE_RETURN_CODE(M_MODULE_TSPU, M_MODULE_TSPU_COMMON, 2);

#define M_RET_FAILED_NOT_HAVE_ENOUGH_PIDFILTER  M_BUILD_MODULE_RETURN_CODE(M_MODULE_TSPU, M_MODULE_TSPU_PIDFILTER, 1);
#define M_RET_FAILED_NOT_CREATE_NEW_PIDFILTER   M_BUILD_MODULE_RETURN_CODE(M_MODULE_TSPU, M_MODULE_TSPU_PIDFILTER, 2);
#define M_RET_FAILED_NOT_FIND_HANDLE_PIDFILTER  M_BUILD_MODULE_RETURN_CODE(M_MODULE_TSPU, M_MODULE_TSPU_PIDFILTER, 3);
#define M_RET_FAILED_NOT_FILTER_NOT_CONNECTTED  M_BUILD_MODULE_RETURN_CODE(M_MODULE_TSPU, M_MODULE_TSPU_PIDFILTER, 4);

#define M_RET_FAILED_NOT_HAVE_ENOUGH_SECTIONFILTER  M_BUILD_MODULE_RETURN_CODE(M_MODULE_TSPU, M_MODULE_TSPU_SECTIONFILTER, 1);
#define M_RET_FAILED_NOT_CREATE_NEW_SECTIONFILTER   M_BUILD_MODULE_RETURN_CODE(M_MODULE_TSPU, M_MODULE_TSPU_SECTIONFILTER, 2);
#define M_RET_FAILED_NOT_FIND_HANDLE_SECTIONFILTER  M_BUILD_MODULE_RETURN_CODE(M_MODULE_TSPU, M_MODULE_TSPU_SECTIONFILTER, 3);

#define SIM_ALLOC(_size_) ((_size_) == 0 ? NULL: malloc((_size_)))
#define SIM_FREE(_addr_)  do { free((_addr_)); (_addr_) = NULL; } while(0)


typedef struct tagTspuDataMsg{
    u32 u32PIDFilterHandle;
    u32 u32CallBackHandle;
    u32 u32DataLen;
    u8  *pu8Data;
}TSPU_DATA_MSG_STRU;

#endif 


#endif // end "#ifndef __ANABASE_TYPE_H__¡°

