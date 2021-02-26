#ifndef __INTLOG_H__
#define __INTLOG_H__
/*===========================================================================

  D A L    I N T E R R U P T   C O N T R O L L E R   L O G 

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

$Header: //components/rel/core.qdsp6/2.1.c4/systemdrivers/InterruptController/inc/InterruptControllerLog.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/23/15   cpaulo  First draft created. 
===========================================================================*/
#include "ULog.h"
#include "ULogFront.h"
#include "comdef.h"

#define INTLOG_REG          0x1
#define INTLOG_PDC          0x2
#define INTLOG_SETTRIGGER   0x4
#define INTLOG_ENABLE       0x8
#define INTLOG_CLEAR       0x10
#define INTLOG_TRIGGER     0x20


/*===========================================================================
FUNCTION IntLog_Init
 
DESCRIPTION 
  This function will be called from InterruptController_DeviceInit.
  This function will initialize the uLog required for InterruptController.
===========================================================================*/
void IntLog_Init( ULogHandle* hL );


/*===========================================================================
FUNCTION IntLog_Out
 
DESCRIPTION 
  This function will be called to print a log to the buffer.  
===========================================================================*/
void IntLog_Out( uint32 nMask, ULogHandle hL, uint32 nArgs, const char* sztStr, ... );

#endif /*__INTLOG_H__*/
