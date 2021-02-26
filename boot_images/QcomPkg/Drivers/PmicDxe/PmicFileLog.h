#ifndef __PMICFILELOG_H__
#define __PMICFILELOG_H__

/*! @file PmicFileLog.h
 *
 *  PMIC FILE Logging
 *
 *  Copyright (c) 2014 - 2017 Qualcomm Technologies, Inc.  All Rights Reserved. 
 *  Qualcomm Technologies Proprietary and Confidential.
 */

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/30/17   ll      Implemented level file-logging 
07/07/16   va      DEBUG print to print time stamp in seconds
10/23/15   va      Time stamp for UART logging
06/09/15   mr      UEFI Charger logging in file in LogFS partition (CR-845523)
12/09/14   al      Sync with latest
08/21/14   va      Enable File Logging 

===========================================================================*/

/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/
  /**
  File Logging Dependencies 
  */
#include "ULogFront.h"
#include "ULogUEFI.h"


/**
UEFI DEBUG Dependencies 
*/
#include <Library/DebugLib.h>
#include <Library/QcomBaseLib.h>
#include <Library/TimerLib.h>

/*===========================================================================
                     MACRO DEFINATIONS
===========================================================================*/
/* PMIC Log File Name */
#define LOGFILE_IN_EFS "PmicLog"
#define LOGFILE_SIZE   8388608 // 8MB


#if !defined(DEBUG_TIME)      
  #define DEBUG_TIME(Expression)          \
    do {                           \
         DEBUG ((EFI_D_ERROR, "%lld, ", PmicFileLog_CalculateTime())); \
        _DEBUG (Expression);       \
      }while (FALSE)
#endif
         /*DEBUG ((EFI_D_ERROR, "0x%x, ", GetTimerCountms())); \*/
#define _ULOG(Expression)   ULogPrint Expression

#if !defined(CHARGER_DEBUG)
  #define PMIC_DEBUG_SIMPLE(Expression)        \
    do {                           \
      if (PmicFileLog_PrintDebugMsg ()) {  \
        _DEBUG (Expression);       \
      }                            \
      if(PmicFileLog_PrintDebugMsgToFile()) {  \
        _ULOG (Expression);         \
      }                            \
    } while (FALSE)
  #define PMIC_DEBUG(Expression)        \
    do {                           \
      if (PmicFileLog_PrintDebugMsg ()) {  \
         DEBUG ((EFI_D_ERROR, "%lld, ", PmicFileLog_CalculateTime())); \
        _DEBUG (Expression);       \
      }                            \
      if(PmicFileLog_PrintDebugMsgToFile()) {  \
        _ULOG (Expression);         \
      }                            \
    } while (FALSE)
  #define PMIC_FILE_DEBUG(Expression)        \
    do {                           \
      if(PmicFileLog_PrintDebugMsgToFile()) {  \
        _ULOG (Expression);         \
      }                            \
    } while (FALSE)
  /*Use below for File debug a log with Always on UART messeages */
  #define PMIC_FILE_UART_DEBUG(Expression)      \
    do {               \
      DEBUG_TIME(Expression);    \
      if(PmicFileLog_PrintDebugMsgToFile()) {  \
      _ULOG (Expression);     \
      }                \
    } while (FALSE)

  #define PMIC_UART_DEBUG(Expression)        \
    do {                           \
      if (PmicFileLog_PrintDebugMsg ()) {  \
         DEBUG ((EFI_D_ERROR, "%lld, ", PmicFileLog_CalculateTime())); \
         _DEBUG (Expression);       \
      }                            \
    } while (FALSE)
#else
  //#define CHARGER_DEBUG(Expression)
#endif


/*===========================================================================
                     TYPE DECLARATIONS
===========================================================================*/


/*===========================================================================
                       FUNCTION PROTOTYPES
===========================================================================*/

BOOLEAN EFIAPI PmicFileLog_PrintDebugMsg(void);

BOOLEAN EFIAPI PmicFileLog_PrintDebugMsgToFile(void);

EFI_STATUS PmicFileLog_InitFileLog(BOOLEAN PrintChgAppDbgMsg, BOOLEAN PrintChgAppDbgMsgToFile, UINT32 logLevelMask);

VOID  ULogPrint (IN  UINTN ErrorLevel, IN  CONST CHAR8  *Format,  ...);

UINT64 PmicFileLog_TimeGet64(void);

UINT64 PmicFileLog_CalculateTime();


#endif // __PMICFILELOG_H__


