/*================================================================================================
 *
 * FILE:        kickstart_log.h
 *
 * DESCRIPTION:
 *   Declares debug log macro and log function for supporting EVENT, INFO, ERROR and WARN
 *   messages
 *
 *        Copyright � 2009-2012,2018 Qualcomm Technologies, Inc.
 *               All Rights Reserved.
 *===============================================================================================
 *
 *
 *  kickstart_log.h : Declares debug log macro and log function for supporting EVENT, INFO, ERROR
 *  and WARN messages
 * ==========================================================================================
 *   $Header: //components/rel/boot.xf/1.4/QcomPkg/Tools/storage/fh_loader/QSaharaServer/src/kickstart_log.h#3 $
 *   $DateTime: 2018/09/12 03:44:02 $
 *   $Author: pwbldsvc $
 *
 *  Edit History:
 *  YYYY-MM-DD		who		why
 *  -----------------------------------------------------------------------------
 *  2010-09-28       ng      Added command mode support
 *
 *
 *==========================================================================================
 */

#ifndef KICKSTART_LOG_H
#define KICKSTART_LOG_H
//#if defined(WINDOWSPC)
#include <time.h>
//#endif

// definitions for Android logging
#if defined(LINUXPC) || defined(WINDOWSPC) || defined(linux)
   #define LOGE printf
   #define LOGI printf
#else
   #define LOG_TAG "kickstart"
   #include "cutils/log.h"
   #include "common_log.h"
#endif

#define LOG_BUFFER_SIZE 2048

// Logging level
enum {
    LOG_ERROR,
    LOG_INFO,
    LOG_WARN,
    LOG_EVENT,
    LOG_STATUS
} LOG_LEVEL;

/*macro for logging */
#if defined(WINDOWSPC)
#define dbg(log_level, fmt, ...) kickstart_log (log_level, __FUNCTION__, __LINE__, fmt, __VA_ARGS__)
#else
#define dbg(log_level, fmt ...) kickstart_log (log_level, __FUNCTION__, __LINE__, fmt)
#endif

/******************************************************************************
* Name: kickstart_log
*
* Description:
*    This function does the app level logging
*
* Arguments:
*    log              -   type of log INFO/ERROR/WARN
*    function         -   function name
*    line_number      -   line number of the log
*    format           -   format
*    ...              -   variable list argumet
*
* Returns:
*    None
*
* Note:
*
******************************************************************************/
void kickstart_log (int log, const char *function, int line_number, const char *format, ...);

#endif
