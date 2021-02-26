/** 
  @file  QcomChargerAppFileLogging.h
  @brief QCOM Charger App File Logging header API definitions.
*/
/*=============================================================================
  Copyright (c) 2016 QUALCOMM Technologies Incorporated.
  All rights reserved.
  Qualcomm Confidential and Proprietary.
=============================================================================*/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 12/28/16   sm      Initial draft

=============================================================================*/
#ifndef __QCOMCHARGERAPPFILELOGGING_H__
#define __QCOMCHARGERAPPFILELOGGING_H__

/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/

/**
UEFI DEBUG Dependencies 
*/
#include <Library/DebugLib.h>
#include <Library/QcomBaseLib.h>
#include <Library/TimerLib.h>
#include <Protocol/EFIQcomCharger.h>

/*===========================================================================
                     MACRO DEFINATIONS
===========================================================================*/

#if !defined(DEBUG_TIME)      
  #define DEBUG_TIME(Expression)          \
      do {                           \
           DEBUG ((EFI_D_WARN, "%lld, ", QcomChargerAppFileLog_CalculateTime())); \
          _DEBUG (Expression);       \
        }while (FALSE)
#endif
  
#define _ULOG(Expression)   ULogPrint Expression
  
#if !defined(CHARGERAPP_DEBUG)      
  #define CHARGERAPP_DEBUG(Expression)        \
      do {                           \
        if (QcomChargerAppFileLog_PrintDebugMsg ()) {  \
           DEBUG ((EFI_D_WARN, "%lld, ", QcomChargerAppFileLog_CalculateTime())); \
          _DEBUG (Expression);       \
        }                            \
        if(QcomChargerAppFileLog_PrintDebugMsgToFile()) {  \
          _ULOG (Expression);         \
        }                            \
      } while (FALSE)
  #define CHARGERAPP_FILE_DEBUG(Expression)        \
      do {                           \
        if(QcomChargerAppFileLog_PrintDebugMsgToFile()) {  \
          _ULOG (Expression);         \
        }                            \
      } while (FALSE)
    /*Use below for File debug a log with Always on UART messeages */
  #define CHARGERAPP_FILE_UART_DEBUG(Expression)      \
      do {               \
        DEBUG_TIME(Expression);    \
        if(QcomChargerAppFileLog_PrintDebugMsgToFile()) {  \
        _ULOG (Expression);     \
        }                \
      } while (FALSE)
  #define CHARGERAPP_UART_DEBUG(Expression)        \
      do {                           \
        if (QcomChargerAppFileLog_PrintDebugMsg ()) {  \
           DEBUG ((EFI_D_WARN, "%lld, ", QcomChargerAppFileLog_CalculateTime())); \
          _DEBUG (Expression);       \
        }                            \
      } while (FALSE)
#else
    //#define CHARGERAPP_DEBUG(Expression)
#endif

/*===========================================================================
                     TYPE DECLARATIONS
===========================================================================*/

/*===========================================================================
  FUNCTION DEFINITIONS
===========================================================================*/
EFI_STATUS QcomChargerAppFileLog_Init(void);

BOOLEAN QcomChargerAppFileLog_PrintDebugMsg(void);

BOOLEAN QcomChargerAppFileLog_PrintDebugMsgToFile(void);

VOID ULogPrint (IN  UINTN ErrorLevel, IN  CONST CHAR8  *Format,  ...);

UINT64 QcomChargerAppFileLog_TimeGet64(void);

UINT64 QcomChargerAppFileLog_CalculateTime(void);

EFI_STATUS QcomChargerAppFileLog_LogParam(EFI_QCOM_CHARGER_ACTION_INFO *ChargerActionInfo);

#endif  /* __QCOMCHARGERAPP_H__ */

