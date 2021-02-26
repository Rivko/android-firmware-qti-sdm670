/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                      ERR service crash root PD simulator
                
GENERAL DESCRIPTION
  Command handler for injecting various types of crashes for testing

INITIALIZATION AND SEQUENCING REQUIREMENTS
  Diag must be initialized

Copyright (c) 2017 by Qualcomm Technologies, Inc.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*==========================================================================

                        EDIT HISTORY FOR MODULE
$Header: //components/rel/core.qdsp6/2.1/debugtools/eic/src/err_inject_crash_user.c#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
06/15/17   bvb     New file for EIC Root PD

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "err.h"
#include "err_inject_crash.h"
#include "err_inject_crash_settings.h"

/*===========================================================================

                     EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/

/*===========================================================================

                     EXTERNAL DATA DECLARATIONS

===========================================================================*/

/*===========================================================================

                        Private function prototypes

===========================================================================*/
boolean err_inject_crash_validate(err_inject_crash_type injected_crash);

/*===========================================================================

                              Function definitions

===========================================================================*/

boolean err_inject_crash_validate(err_inject_crash_type injected_crash)
{
  boolean is_valid = TRUE;

  // add USER PD unsupported commands here
  switch(injected_crash)
  {
    case ERR_INJECT_WDOG_TIMEOUT:
      is_valid = FALSE;
      break;
    case ERR_INJECT_WDOG_NMI:
      is_valid = FALSE;
      break;
    case ERR_INJECT_WDOG_MULTITHREADED:
      is_valid = FALSE;
      break;
    default:
      break;
  }
  
  if (injected_crash >= SIZEOF_ERR_INJECT_CRASH_TYPE)
  {
    is_valid = FALSE;
  }
  
  return is_valid;
} 

void err_inject_crash_wdog(void)
{
  /* Halt other HW threads for Root process */
  TMS_MSG_ERROR("Entering STM and injecting DOG Bite, Not Supported in User PD");
}

void err_inject_crash_wdog_nmi(void)
{
  TMS_MSG_ERROR("Injecting DOG NMI, Not Supported in User PD");
}

void err_inject_crash_wdog_multithreaded(void)
{
  TMS_MSG_ERROR("Injecting DOG multithreaded Bite, Not Supported in User PD");
}