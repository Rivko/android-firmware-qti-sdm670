/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                      ERR service crash simulator
                
GENERAL DESCRIPTION
  Command handler for injecting various types of crashes for testing

INITIALIZATION AND SEQUENCING REQUIREMENTS
  Diag must be initialized

Copyright (c) 2014 - 2017 by QUALCOMM Technologies, Inc.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*==========================================================================

                        EDIT HISTORY FOR MODULE
$Header: //components/rel/core.qdsp6/2.1/debugtools/eic/src/uerr_inject_crash.c#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
01/25/17   jlg     Established settings interface header file
08/04/15   dav     Added changes for MPSS island mode
04/02/14   psu     Update diag macros using optimized versions.
09/26/13   rks     Fix for CR502558:use non-deferrable timer
10/28/11   mcg     File created.

===========================================================================*/

#ifndef ERR_INJECT_CRASH
  #error Required definition is missing, check configuration
#endif //ERR_INJECT_CRASH

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "err.h"
#include "err_inject_crash.h"
#include "err_inject_crash_settings.h"
#include "utimer.h"
#include "uTimetick.h"
#include "qurt.h"

/*===========================================================================

                     EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/


/*===========================================================================

                     INTERNAL DATA DECLARATIONS

===========================================================================*/

static utimer_type      uImage_timer;
static volatile int err_inject_crash_zero = 0;

#define ERR_UIMAGE_TIMER_PERIOD_REPEAT 10   

/*===========================================================================

                        Private function prototypes

===========================================================================*/

static void uerr_inject_crash_err_fatal(void);
static void uerr_inject_crash_wdog(void);
static void uerr_inject_null_access(void);
static void uerr_inject_crash_div0(void);

static void uerr_inject_crash_execute(err_inject_crash_type crash_type);


/*===========================================================================

                              Function definitions

===========================================================================*/

static void uerr_inject_crash_err_fatal(void)
{
  ERR_FATAL("Crash injected via Diag",0,0,0);
}

static void uerr_inject_crash_wdog(void)
{
  //dog_intentional_timeout();
}

static void uerr_inject_null_access(void)
{
  static int result;

  result = *(int*)err_inject_crash_zero;

  //TMS_MSG_ERROR_2("Should have crashed due to null access: %#x->%#x", err_inject_crash_zero, result);

}

static void uerr_inject_crash_div0(void)
{
  static float result;

  result = 7/err_inject_crash_zero;

  //TMS_MSG_ERROR_2("Should have crashed due to div0: 7/%u=%d", err_inject_crash_zero, result);

}


void uerr_inject_crash_execute(err_inject_crash_type crash_type) 
{
  if ( qurt_island_get_status() == TRUE )
  {
    switch(crash_type)
    {
    case ERR_INJECT_DIV0: 
      uerr_inject_crash_div0();
      break;
    case ERR_INJECT_WDOG_TIMEOUT:
      uerr_inject_crash_wdog();
      break;
    case ERR_INJECT_NULLPTR:
      uerr_inject_null_access();
      break;
    case ERR_INJECT_ERR_FATAL :
      uerr_inject_crash_err_fatal();
      break;
    default:
      break;
      //TMS_MSG_ERROR_1("uerr_inject_crash: unhandled crash ID %#x", crash_type);
    }
  }
}

void uerr_inject_crash_initiate(err_inject_crash_type crash_type)
{
  utimer_def_osal(&uImage_timer, UTIMER_FUNC1_CB_TYPE, uerr_inject_crash_execute, (utimer_cb_data_type)crash_type);
  utimer_set_64(&uImage_timer, (utimer_timetick_type)ERR_UIMAGE_TIMER_PERIOD_REPEAT, (utimer_timetick_type)ERR_UIMAGE_TIMER_PERIOD_REPEAT, UT_MSEC);
}

