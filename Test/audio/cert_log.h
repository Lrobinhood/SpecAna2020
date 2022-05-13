/*******************************************************************************
 *  Copyright @ 2020
 *
 *  For cert module log
 *
 *
 *******************************************************************************/

/**
 * @brief   Functions for cert module log
 * @file    nordig_base.h
 * @date    2020-11-05
 */

//#include <android/log.h>
#ifdef _WIN32
#include "type_for_win32.h"
#else
#include <utils/Log.h>
#endif

#ifndef __CERT_LOG_H__
#define __CERT_LOG_H__


#ifdef __cplusplus
extern "C" {
#endif


#define CERT_LOG_DEBG(module, ...) CERT_Common_Log(ANDROID_LOG_DEBUG, module, __VA_ARGS__)
#define CERT_LOG_INFO(module, ...) CERT_Common_Log(ANDROID_LOG_INFO, module, __VA_ARGS__)
#define CERT_LOG_WARN(module, ...) CERT_Common_Log(ANDROID_LOG_WARN, module, __VA_ARGS__)

#define CERT_LOG_ERROR(module, ...) CERT_Common_Log(ANDROID_LOG_ERROR, module, __VA_ARGS__)
#define CERT_LOG_FATAL(module, ...) CERT_Common_Log(ANDROID_LOG_FATAL, module, __VA_ARGS__)



void CERT_Common_Log(U32BIT loglevel, const char *module, const char *format, ... );

#ifdef __cplusplus
}
#endif

#endif // "#ifndef __CERT_LOG_H__"
