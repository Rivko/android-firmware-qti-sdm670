/*
===========================================================================
*/
/**
  @file uClock.c
  @brief Public definitions include file for accessing the uClock device driver.
*/
/*===========================================================================
NOTE: The @brief description and any detailed descriptions above do not appear 
      in the PDF. 

      This file is the public header file for the sensors micro image (uImage)
      clock driver implementation.  It is meant to be used by a small subset
      of drivers supporting the sensors uImage feature, which cannot access
      DDR.

===========================================================================*/
/*
  ===========================================================================

  Copyright (c) 2016 QUALCOMM Technologies Incorporated.  
  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ===========================================================================

  $Header: //components/rel/core.qdsp6/2.1/systemdrivers/clock/image/slpi/hw/sdm855/uClock.c#1 $
  $DateTime: 2017/07/21 22:10:47 $
  $Author: pwbldsvc $

  when       who     what, where, why
  --------   ---     ---------------------------------------------------------- 
  12/17/13   dcf     Added for MSM8994 micro sensors functionality.

  ===========================================================================
*/


/*=========================================================================
      Include Files
==========================================================================*/

#include "com_dtypes.h"
#include "uClock.h"
#include "uClockInternal.h"
#include "qurt.h"
#include "qurt_qdi.h"
#include "qurt_qdi_driver.h"
#include "uClockHWIO.h"
#include "HALhwio.h"

/*=========================================================================
      Type Definitions
==========================================================================*/

/*
 * uClock user context type.
 * Parameters:
 *   bIsInitialized  - Specifies if the driver has been initialized.
 *   nClockQDIHandle - The QDI handle to the root PD.
 */
typedef struct
{
  boolean bIsInitialized;
  int32   nClockQDIHandle;

}uClockUserCtxtType;

/*=========================================================================
      Local Variables
==========================================================================*/
/*
 * Bit-field values for the clock configuration registers.
 */
#define CLOCK_CBCR_CLK_OFF_BMSK  0x80000000
#define CLOCK_CBCR_CLK_ENA_BMSK  0x00000001

/*
 * Register map table for clocks residing in the uImage.  Some clocks have divider
 * registers separate from their CBCRs so that must be reflected here.
 */
static uClockRegType gnClockRegs[CLOCK_TOTAL_CLOCK_ENUMS] =
{
  {HWIO_ADDR(SSC_SCC_QUPV3_SE0_CBCR),
   HWIO_ADDR(SSC_SCC_Q6_CLOCK_BRANCH_EN_VOTE), 
   HWIO_SSC_SCC_Q6_CLOCK_BRANCH_EN_VOTE_QUPV3_SE0_CLK_ENA_BMSK},

   {HWIO_ADDR(SSC_SCC_QUPV3_SE1_CBCR),
    HWIO_ADDR(SSC_SCC_Q6_CLOCK_BRANCH_EN_VOTE), 
    HWIO_SSC_SCC_Q6_CLOCK_BRANCH_EN_VOTE_QUPV3_SE1_CLK_ENA_BMSK},

   {HWIO_ADDR(SSC_SCC_QUPV3_SE2_CBCR),
    HWIO_ADDR(SSC_SCC_Q6_CLOCK_BRANCH_EN_VOTE), 
    HWIO_SSC_SCC_Q6_CLOCK_BRANCH_EN_VOTE_QUPV3_SE2_CLK_ENA_BMSK},

   {HWIO_ADDR(SSC_SCC_QUPV3_SE3_CBCR),
    HWIO_ADDR(SSC_SCC_Q6_CLOCK_BRANCH_EN_VOTE), 
    HWIO_SSC_SCC_Q6_CLOCK_BRANCH_EN_VOTE_QUPV3_SE3_CLK_ENA_BMSK},

   {HWIO_ADDR(SSC_SCC_QUPV3_SE4_CBCR),
    HWIO_ADDR(SSC_SCC_Q6_CLOCK_BRANCH_EN_VOTE), 
    HWIO_SSC_SCC_Q6_CLOCK_BRANCH_EN_VOTE_QUPV3_SE4_CLK_ENA_BMSK},

   {HWIO_ADDR(SSC_SCC_QUPV3_SE5_CBCR),
    HWIO_ADDR(SSC_SCC_Q6_CLOCK_BRANCH_EN_VOTE), 
    HWIO_SSC_SCC_Q6_CLOCK_BRANCH_EN_VOTE_QUPV3_SE5_CLK_ENA_BMSK},
	
   {HWIO_ADDR(SSC_SCC_QUPV3_S_HCLK_CBCR),
    HWIO_ADDR(SSC_SCC_Q6_CLOCK_BRANCH_EN_VOTE), 
    HWIO_SSC_SCC_Q6_CLOCK_BRANCH_EN_VOTE_QUPV3_S_HCLK_ENA_BMSK},

   {HWIO_ADDR(SSC_SCC_QUPV3_M_HCLK_CBCR),
    HWIO_ADDR(SSC_SCC_Q6_CLOCK_BRANCH_EN_VOTE), 
    HWIO_SSC_SCC_Q6_CLOCK_BRANCH_EN_VOTE_QUPV3_M_HCLK_ENA_BMSK},
	
   {HWIO_ADDR(SSC_SCC_QUPV3_CORE_CBCR),
    HWIO_ADDR(SSC_SCC_Q6_CLOCK_BRANCH_EN_VOTE), 
    HWIO_SSC_SCC_Q6_CLOCK_BRANCH_EN_VOTE_QUPV3_CORE_CLK_ENA_BMSK},
	
   {HWIO_ADDR(SSC_SCC_QUPV3_2XCORE_CBCR),
    HWIO_ADDR(SSC_SCC_Q6_CLOCK_BRANCH_EN_VOTE), 
    HWIO_SSC_SCC_Q6_CLOCK_BRANCH_EN_VOTE_QUPV3_2X_CORE_CLK_ENA_BMSK},
};

/*
 * Local uClock user context.
 */
static uClockUserCtxtType uClockUserCtxt;


/*=========================================================================
      Function Definitions.
==========================================================================*/

/* ============================================================================
**  Function : uClock_SendRequest
** ============================================================================
*/
/**
  Sends a request to the root PD for the specified command.
    
  @param[in]  eClockId  - The ID of the clock to perform the operation on.
  @param[in]  eCmd      - The requested command to perform.
  @param[in]  nParams   - The parameters to pass in.

  @return
  TRUE  - The operation completed successfully.
  FALSE - Initialization failed or one of the parameters is invalid.
  
  @dependencies
  None.

*/

boolean uClock_SendRequest(uClockIdType eClockId, uClockCmdType eCmd, uint32 nParams)
{
  int nQDIResult;

  /*
   * Make sure the QDI path is open and all initialization has taken
   * place.
   */
  if( uClockUserCtxt.bIsInitialized == FALSE )
  {
    if(0 != (uClockUserCtxt.nClockQDIHandle = qurt_qdi_open(uClockQDIName)))
    {
      uClockUserCtxt.bIsInitialized = TRUE;
    }
    else
    {
      return(FALSE);
    }
  }

  /*
   * Send the request to the root PD.
   */
  nQDIResult = qurt_qdi_handle_invoke(uClockUserCtxt.nClockQDIHandle, 
                                      UCLOCK_QDI_CLOCK_CMD_ROUTER,
                                      eCmd,
                                      (uint32)eClockId,
                                      nParams);

  if(nQDIResult != 0)
  {
    return(FALSE);
  }
  return(TRUE);

}


/* ============================================================================
**  Function : uClock_EnableClock
** ============================================================================
*/
/**
  Enables a clock.
    
  @param[in]  eClockId  - The ID of the clock to enable.

  @return
  TRUE  - The clock was enabled.
  FALSE - Otherwise.
  
  @dependencies
  None.

*/

boolean uClock_EnableClock(uClockIdType eClockId)
{
  return uClock_SendRequest(eClockId, UCLOCK_CMD_ENABLE_CLOCK, 0);

} /* uClock_EnableClock */


/* ============================================================================
**  Function : uClock_DisableClock
** ============================================================================
*/
/**
  Disables a clock.
    
  @param[in]  eClockId  - The ID of the clock to disable.

  @return
  TRUE  - The clock was disabled.
  FALSE - Otherwise.
  
  @dependencies
  None.

*/

boolean uClock_DisableClock(uClockIdType eClockId)
{
  return uClock_SendRequest(eClockId, UCLOCK_CMD_DISABLE_CLOCK, 0);

} /* uClock_DisableClock */


/* ============================================================================
**  Function : uClock_IsClockOn
** ============================================================================
*/
/**
  Returns whether a clock is enabled or not.
    
  @param[in]  eClockId  - The ID of the clock to disable.

  @return
  TRUE  - The clock is on.
  FALSE - The clock is off.
  
  @dependencies
  None.

*/

boolean uClock_IsClockOn(uClockIdType eClockId)
{
  return TRUE;

  if(eClockId < CLOCK_TOTAL_CLOCK_ENUMS)
  {
    if(in_dword(gnClockRegs[eClockId].nCbcAddr) & CLOCK_CBCR_CLK_OFF_BMSK)
    {
      return(FALSE);
    }
    else
    {
      return(TRUE);
    }
  }

  return(FALSE);

} /* uClock_IsClockEnabled */


