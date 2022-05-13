/*******************************************************************************
 *  Copyright @ 2020
 *
 *  For cert module log
 *
 *
 *******************************************************************************/

/**
 * @brief   Functions for cert module log
 * @file    nordig_base.c
 * @date    2020-11-05
 */

/* compiler library header files */
#include <stdio.h>
#include <stdarg.h>

/* third party header files */
/* DVBCore header files*/
#include "techtype.h"
#include "dbgfuncs.h"
//#include "stbhwc.h"

#include "cert_log.h"


#ifdef __cplusplus
extern "C" {
#endif

/*---local (static) variable declarations for this file-----------------------*/
/*   (internal variables declared static to make them local) */
static char debug_msg_buff[512];

#ifdef _WIN32

U32BIT sg_u32LogLevel = ANDROID_LOG_DEBUG;

static char msg[1024];

void CERT_Common_Log(U32BIT loglevel, const char *module, const char *format, ... )
{
    va_list vparams;


    ASSERT(module != NULL);
    ASSERT(format != NULL);

    va_start(vparams, format);
    vsnprintf(debug_msg_buff, sizeof(debug_msg_buff), format, vparams);
    va_end(vparams);

    sprintf(msg, "<%s> %s", module, debug_msg_buff);

    if (loglevel >= sg_u32LogLevel)
    {
        printf("%s\n", msg);
    }

}

U32BIT CERT_Common_SetLogLevel(U32BIT loglevel)
{
    U32BIT oldlevel = sg_u32LogLevel;

    sg_u32LogLevel = loglevel;

    return oldlevel;
}



#else

void CERT_Common_Log(U32BIT loglevel, const char *module, const char *format, ...)
{
    va_list vparams;


    ASSERT(module != NULL);
    ASSERT(format != NULL);

    va_start(vparams, format);
    vsnprintf(debug_msg_buff, sizeof(debug_msg_buff), format, vparams);
    va_end(vparams);

    __android_log_print(loglevel, module, "%s", debug_msg_buff);
}

#endif



#ifdef __cplusplus
}
#endif

