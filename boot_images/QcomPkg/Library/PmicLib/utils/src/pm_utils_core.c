/*! \file
*  
*  \brief  pm_utils_core.c 
*  
*  &copy; Copyright 2016-2017 QUALCOMM Technologies Incorporated, All Rights Reserved
*/

/*===========================================================================

EDIT HISTORY FOR MODULE

This document is created by a code generator, therefore this section will
not contain comments describing changes made to the module.


when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
08/03/16   akm      Created

===========================================================================*/

/*===========================================================================

INCLUDE FILES 

===========================================================================*/
#include <Uefi.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/SerialPortShLib.h>
#include <Library/TimerLib.h>

#include "pm_utils.h"


pm_err_flag_type pm_busy_wait(uint32 req_wait_usec)
{
  uint32 actual_wait_usec = 0;

  /*the MicroSecondDelay API returns wait_usec as return value*/
  actual_wait_usec = MicroSecondDelay(req_wait_usec);

  if(actual_wait_usec < req_wait_usec)
  {
   return PM_ERR_FLAG_INVALID_PARAMETER; 
  }
  else
  {
    return PM_ERR_FLAG_SUCCESS;
  }

}

pm_err_flag_type 
pm_device_shutdown(void)
{
  /*flush UART before shutdown*/
  SerialPortFlush();
  gRT->ResetSystem(EfiResetShutdown, EFI_SUCCESS, 0, NULL);
  /*control should not return from here*/
  return PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
}
