#ifndef RPMH_IMAGE_OS_H
#define RPMH_IMAGE_OS_H

/*===========================================================================
                             rpmh_image_os.h

DESCRIPTION:
  Any necessary os specific header information 

Copyright (c) 2016 QUALCOMM Technologies, Inc. (QTI). All Rights Reserved.  
QUALCOMM Proprietary.  Export of this technology or software is regulated 
by the U.S. Government. Diversion contrary to U.S. law prohibited.
===========================================================================*/


//===========================================================================
//                   Includes and Public Data Declarations
//===========================================================================

//---------------------------------------------------------------------------
// Include Files
//---------------------------------------------------------------------------
#include "ULogFront.h"
#include "DALSys.h"
#include "DALDeviceId.h"
#include "DDIInterruptController.h"
#include "DDITimer.h"
#include "DALSysTypes.h"
#include "DDIClock.h"

//---------------------------------------------------------------------------
// Constant / Define Declarations
//---------------------------------------------------------------------------

typedef struct
{
    DALSYSSyncHandle      lock;
    DalDeviceHandle       *isr_handle;    
    DalDeviceHandle       *timer_handle;    
    DalDeviceHandle       *clock_handle;
    ULogHandle            log;
} rpmh_os_t;

extern rpmh_os_t rpmh_os;

#define RPMH_LOG_0(a)             ULOG_RT_PRINTF_0(rpmh_os.log, a) 
#define RPMH_LOG_1(a, b)          ULOG_RT_PRINTF_1(rpmh_os.log, a, b) 
#define RPMH_LOG_2(a, b, c)       ULOG_RT_PRINTF_2(rpmh_os.log, a, b, c)
#define RPMH_LOG_3(a, b, c, d)    ULOG_RT_PRINTF_3(rpmh_os.log, a, b, c, d)
#define RPMH_LOG_4(a, b, c, d, e) ULOG_RT_PRINTF_4(rpmh_os.log, a, b, c, d, e)

//---------------------------------------------------------------------------
// Type Declarations
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Forward Declarations
//---------------------------------------------------------------------------

//===========================================================================
//                             Function Definitions
//===========================================================================

#endif //RPMH_IMAGE_OS
