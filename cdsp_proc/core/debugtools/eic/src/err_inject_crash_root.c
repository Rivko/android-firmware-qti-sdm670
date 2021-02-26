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
$Header: //components/rel/core.qdsp6/2.1.c4/debugtools/eic/src/err_inject_crash_root.c#1 $

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
extern void dog_force_bite(void);
extern void dog_force_nmi(void); 
extern void err_enter_stm_mode(void);

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

  // add ROOT PD unsupported commands here
  
  if (injected_crash >= SIZEOF_ERR_INJECT_CRASH_TYPE)
  {
    is_valid = FALSE;
  }
  
  return is_valid;
} 

void err_inject_crash_wdog(void)
{
  /* Halt other HW threads for Root process */
  TMS_MSG_HIGH("Entering STM and injecting DOG Bite.");
  err_enter_stm_mode();
  dog_force_bite();
  while(1); /* To prevent Q6 from entering power collapse */
}

void err_inject_crash_wdog_nmi(void)
{
  TMS_MSG_HIGH("Injecting DOG NMI");
  //dog_force_nmi(); //TODO: Uncomment after verifying NMI behavior
}

void err_inject_crash_wdog_multithreaded(void)
{
  TMS_MSG_HIGH("Injecting DOG multithreaded Bite");
  dog_force_bite();
}
