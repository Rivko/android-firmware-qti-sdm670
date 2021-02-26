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
//#include "ULogFront.h"
//#include "DALSys.h"
//#include "DALDeviceId.h"
//#include "DDIInterruptController.h"
//#include "DDITimer.h"
#include "rex.h"
#include "aop_ulog.h"

//---------------------------------------------------------------------------
// Constant / Define Declarations
//---------------------------------------------------------------------------

typedef struct
{
  rex_crit_sect_type      lock;
  boolean                 in_isr;
  //DalDeviceHandle       *isr_handle;    
  //DalDeviceHandle       *timer_handle;    
  //ULogHandle            log;
} rpmh_os_t;

extern rpmh_os_t rpmh_os;

#define RPMH_LOG_0(a)             if(!rpmh_os.in_isr) {SWEVENT(a);} 
#define RPMH_LOG_1(a, b)          if(!rpmh_os.in_isr) {SWEVENT(a, b);} 
#define RPMH_LOG_2(a, b, c)       if(!rpmh_os.in_isr) {SWEVENT(a, b, c);}
#define RPMH_LOG_3(a, b, c, d)    if(!rpmh_os.in_isr) {SWEVENT(a, b, c, d);}
#define RPMH_LOG_4(a, b, c, d, e) if(!rpmh_os.in_isr) {SWEVENT(a, b, c, d, e);}

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
