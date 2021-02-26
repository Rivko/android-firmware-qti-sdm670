/** 
  @file  QcomChargerApp.h
  @brief QCOM Charger App header API definitions.
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
 07/07/16   va      DEBUG print to print time stamp in seconds
 06/14/16   va      Adding Thermal and File log support
 05/20/16   va      Initial File

=============================================================================*/
#ifndef __QCOMCHARGERAPP_H__
#define __QCOMCHARGERAPP_H__

/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/

/**
UEFI DEBUG Dependencies 
*/
#include <Library/DebugLib.h>
#include <Library/QcomBaseLib.h>
#include <Library/TimerLib.h>

/*===========================================================================
                     MACRO DEFINATIONS
===========================================================================*/

#if !defined(DEBUG_TIME)      
  #define DEBUG_TIME(Expression)          \
      do {                           \
           DEBUG ((EFI_D_WARN, "%lld, ", QcomChargerApp_CalculateTime())); \
          _DEBUG (Expression);       \
        }while (FALSE)
#endif
  
#define _ULOG(Expression)   ULogPrint Expression
  
#if !defined(CHARGERAPP_DEBUG)      
  #define CHARGERAPP_DEBUG(Expression)        \
      do {                           \
        if (QcomChargerApp_PrintDebugMsg ()) {  \
           DEBUG ((EFI_D_WARN, "%lld, ", QcomChargerApp_CalculateTime())); \
          _DEBUG (Expression);       \
        }                            \
        if(QcomChargerApp_PrintDebugMsgToFile()) {  \
          _ULOG (Expression);         \
        }                            \
      } while (FALSE)
  #define CHARGERAPP_FILE_DEBUG(Expression)        \
      do {                           \
        if(QcomChargerApp_PrintDebugMsgToFile()) {  \
          _ULOG (Expression);         \
        }                            \
      } while (FALSE)
    /*Use below for File debug a log with Always on UART messeages */
  #define CHARGERAPP_FILE_UART_DEBUG(Expression)      \
      do {               \
        DEBUG_TIME(Expression);    \
        if(QcomChargerApp_PrintDebugMsgToFile()) {  \
        _ULOG (Expression);     \
        }                \
      } while (FALSE)
  #define CHARGERAPP_UART_DEBUG(Expression)        \
      do {                           \
        if (QcomChargerApp_PrintDebugMsg ()) {  \
           DEBUG ((EFI_D_WARN, "%lld, ", QcomChargerApp_CalculateTime())); \
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
BOOLEAN QcomChargerApp_PrintDebugMsg(void);

BOOLEAN QcomChargerApp_PrintDebugMsgToFile(void);

VOID ULogPrint (IN  UINTN ErrorLevel, IN  CONST CHAR8  *Format,  ...);

UINT64 QcomChargerApp_TimeGet64(void);

UINT64 QcomChargerApp_CalculateTime();

#endif  /* __QCOMCHARGERAPP_H__ */

