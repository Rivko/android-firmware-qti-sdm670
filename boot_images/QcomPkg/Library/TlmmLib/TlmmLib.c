/** @file TlmmLib.c
  
  Functions for TlmmLib

  Copyright (c) 2014, Qualcomm Technologies, Inc. All rights reserved.
**/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 09/26/14   ck      Initial revision

=============================================================================*/


/*==========================================================================

                               INCLUDE FILES

===========================================================================*/
#include "DalDevice.h"
#include "DALSys.h"
#include "DALDeviceId.h"
#include "DALStdErr.h"
#include "comdef.h"
#include "DDITlmm.h"
#include "TlmmLib.h"
//#include "gpio_boot.h"

/*===========================================================================
                      DATA DECLARATIONS
===========================================================================*/


/*===========================================================================
                      FUNCTION DECLARATIONS
===========================================================================*/ 
/*===========================================================================

  FUNCTION TLMM_GPIO_INIT

  DESCRIPTION
        This function initializes the GPIOs for the TLMM configuration
        specified at boot time.

  PARAMETERS
    None.

  DEPENDENCIES
    None.

  RETURN VALUE
    None.

  SIDE EFFECTS
    It uses the stack
  DalDeviceHandle* hTlmmDriver = NULL;
  DALResult eResult = DAL_ERROR;

===========================================================================*/
boolean tlmm_gpio_init(void)
{
  DalDeviceHandle* hTlmmDriver = NULL;
  DALResult eResult = DAL_ERROR;

  /* Initialize the TLMM handle. */
  eResult = DAL_DeviceAttach(DALDEVICEID_TLMM, &hTlmmDriver);
  if(DAL_SUCCESS == eResult)
  {
    /* 
     * Try to allocate or attach to shared memory.
     */
    eResult = DalTlmm_PostInit(hTlmmDriver);

    if(DAL_SUCCESS == eResult)
    {
      /* 
       * Only need to initialize TLMM internal structures.  No need to
       * maintain a handle.
       */
      eResult = DAL_DeviceDetach(hTlmmDriver);
    }
  }

  if(DAL_SUCCESS == eResult)
  {
    return(TRUE);
  }
  return(FALSE);

}
