#ifndef __GPIOINTLOG_H__
#define __GPIOINTLOG_H__
/*===========================================================================

  G P I O   I N T E R R U P T   C O N T R O L L E R   L O G 

DESCRIPTION
  This modules contains the logging mechanism for GPIO Interrupt logs.

REFERENCES

       Copyright (c) 2015 QUALCOMM Technologies Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.qdsp6/2.1/systemdrivers/GPIOInt/inc/GPIOIntLog.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/23/15   cpaulo  First draft created. 
===========================================================================*/
#include "ULog.h"
#include "ULogFront.h"
#include "comdef.h"

#define GPIOINTLOG_REG          0x1
#define GPIOINTLOG_MPM          0x2
#define GPIOINTLOG_NPAREQ       0x4
#define GPIOINTLOG_SETTRIGGER   0x8
#define GPIOINTLOG_ENABLE      0x10
#define GPIOINTLOG_CLEAR       0x20
#define GPIOINTLOG_TRIGGER     0x40
#define GPIOINTLOG_SUMMARYISR  0x80         


/*===========================================================================
FUNCTION GPIOIntLog_Init
 
DESCRIPTION 
  This function will be called from GPIOInt_DeviceInit.  This function will
  initialize the uLog required for GPIOInt.
===========================================================================*/
void GPIOIntLog_Init( ULogHandle* hL );


/*===========================================================================
FUNCTION GPIOIntLog_Out
 
DESCRIPTION 
  This function will be called to print a log to the buffer.  
===========================================================================*/
void GPIOIntLog_Out( uint32 nMask, ULogHandle hL, uint32 nArgs, const char* sztStr, ... );

#endif /*__GPIOINTLOG_H__*/
