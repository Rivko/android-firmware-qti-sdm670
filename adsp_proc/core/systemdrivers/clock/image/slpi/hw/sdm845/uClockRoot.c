/*
===========================================================================
*/
/**
  @file uClockRoot.c
  @brief Implementation of the micro clock driver in the root protection
  domain.
*/
/*===========================================================================
NOTE: The @brief description and any detailed descriptions above do not appear
      in the PDF.

      This file is source file for the sensors micro image (uImage)
      clock driver implementation.  It is meant to be used by a small subset
      of drivers supporting the sensors uImage feature, which cannot access
      DDR.  This file implements the portion of the driver residing in the
      root protection domain.

===========================================================================*/
/*
  ===========================================================================

  Copyright (c) 2016 QUALCOMM Technologies Incorporated.
  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ===========================================================================

  $Header: //components/rel/core.qdsp6/2.1/systemdrivers/clock/image/slpi/hw/sdm845/uClockRoot.c#2 $
  $DateTime: 2017/11/11 10:34:24 $
  $Author: pwbldsvc $

  when       who     what, where, why
  --------   ---     ----------------------------------------------------------
  12/01/16   dcf     Initial driver for uImage.

  ===========================================================================
*/


/*=========================================================================
      Include Files
==========================================================================*/

#include "com_dtypes.h"
#include "uClock.h"
#include "uClockHWIO.h"
#include "uClockInternal.h"
#include "HALhwio.h"
#include "qurt.h"
#include "busywait.h"
#include "qurt_qdi.h"
#include "qurt_qdi_driver.h"
#include "qurt_futex.h"
#include "qurt_atomic_ops.h"
#include "stdlib.h"

/*=========================================================================
      Type Definitions
==========================================================================*/

/*
 * QDI Opener object type.
 */
typedef struct
{
  qurt_qdi_obj_t    qdiobj;

}uClockQDIOpenerType;

/*
 * QDI Driver object type.
 */
typedef struct
{
  qurt_qdi_obj_t qdiobj;

}uClockQDIDrvType;


/*
 * Micro-Clock state context structure.
 * Parameters:
 *   Mutex             - Synchronization mutex locking.
 *   bInitialized      - Specifies if initialization has taken place.
 *   uClockQDIDrv      - uClock QDI driver handle.
 *   anClockReferences - Array of clock references.
 */
typedef struct
{
   qurt_mutex_t     Mutex;
   boolean          bInitialized;
   uClockQDIDrvType uClockQDIDrv;
   uint8            anClockReferences[CLOCK_TOTAL_CLOCK_ENUMS];

}uClockCtxtType;


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
 * Global Micro-Clock Driver Context.
 */
static uClockCtxtType ClockDriverCtxt;


/*=========================================================================
      Function Prototypes.
==========================================================================*/

/*
 * Invocation function for routing QDI requests to the associated clock APIs.
 */
static int uClockQDI_Invoke(int hClientHandle,
                          qurt_qdi_obj_t *pobj,
                          int method,
                          qurt_qdi_arg_t a1,
                          qurt_qdi_arg_t a2,
                          qurt_qdi_arg_t a3,
                          qurt_qdi_arg_t a4,
                          qurt_qdi_arg_t a5,
                          qurt_qdi_arg_t a6,
                          qurt_qdi_arg_t a7,
                          qurt_qdi_arg_t a8,
                          qurt_qdi_arg_t a9);

/*
 * Release function for QDI objects.
 */
void uClockQDI_Release(qurt_qdi_obj_t *qdiobj);


/*
 * The QDI functionality is implemented in a opener object.
 */
const uClockQDIOpenerType uClockQDIOpener = {
  {
    uClockQDI_Invoke,
    QDI_REFCNT_PERM,
    uClockQDI_Release
  }
};


/*
 * Internal Clock_IsOn API used by QDI router function.
 */
boolean uClock_IsClockOn(uClockIdType eClockId);


/*=============================================================================
 ==============================================================================

       Q D I   H A N D L E R   P R O G R A M M E R   I N T E R F A C E

 ==============================================================================
 ============================================================================*/

/* ============================================================================
**  Function : uClockQDI_Release
** ============================================================================
*/
/**
  Releases a QDI object.

  @param[in]  qdiobj  - The QDI object to release.

  @return
  None.

  @dependencies
  None.

*/
void uClockQDI_Release
(
  qurt_qdi_obj_t *qdiobj
)
{
  free(qdiobj);
}

/* ============================================================================
**  Function : uClock_QDIOpen
** ============================================================================
*/
/**
  Opens a QDI object.

  @param[in]  hClientHandle  - Handle to the requesting client.
  @param[in]  objptr         - Pointer to the object.
  @param[in]  devname        - The string name of the device.
  @param[in]  mode           - Unused.

  @return
  0  - success
  -1 - failure

  @dependencies
  None.

*/
int uClock_QDIOpen
(
  int hClientHandle,
  qurt_qdi_obj_t *objptr,
  const char *devname,
  int mode
)
{
  /*
   * Initialize the QDI object function pointers and create the handle.
   */
  ClockDriverCtxt.uClockQDIDrv.qdiobj.invoke = uClockQDI_Invoke;
  ClockDriverCtxt.uClockQDIDrv.qdiobj.refcnt = QDI_REFCNT_INIT;
  ClockDriverCtxt.uClockQDIDrv.qdiobj.release = uClockQDI_Release;

  return qurt_qdi_handle_create_from_obj_t(hClientHandle,&ClockDriverCtxt.uClockQDIDrv.qdiobj);

} /* uClock_QDIOpen */


/* ============================================================================
**  Function : uClock_QDIRouteCommand
** ============================================================================
*/
/**
  Router API to invoke the proper clock API based on QDI request.

  @param[in]  hClientHandle  - Handle to the requesting client.
  @param[in]  nCmd           - The requested command.
  @param[in]  nClockId       - The clock identifier.
  @param[in]  nDivider       - The divider value for the SetClockDivider API,
                               if used.

  @return
  0  - success
  -1 - failure

  @dependencies
  None.

*/
int uClock_QDIRouteCommand
(
  int hClientHandle,
  int nCmd,
  int nClockId,
  int nClockVal
)
{
  boolean bRetVal = FALSE;

  switch((uClockCmdType)nCmd)
  {
    case UCLOCK_CMD_ENABLE_CLOCK:
      bRetVal = uClock_EnableClock((uClockIdType)nClockId);
      break;

    case UCLOCK_CMD_DISABLE_CLOCK:
      bRetVal = uClock_DisableClock((uClockIdType)nClockId);
      break;

    case UCLOCK_CMD_IS_CLOCK_ON:
      /*
       * Return a success only if the clock is on.
       */
      if(uClock_IsClockOn((uClockIdType)nClockId)) { bRetVal = TRUE; }
      else { bRetVal = FALSE; }

    default:
      bRetVal = FALSE;
      break;
  }

  if(bRetVal == TRUE)
  {
    return 0;
  }
  return -1;

}

/* ============================================================================
**  Function : uClockQDI_Invoke
** ============================================================================
*/
/**
  Main receiving API for QDI requests from user PD.

  @param[in]  hClientHandle         - Handle to the requesting client.
  @param[in]  method                - The requested command.
  @param[in]  a1 (a1.num: nClockId) - The clock identifier.
  @param[in]  a2 (a2.num: nDivider) - The divider value for the SetClockDivider API,
                                      if used.
  @param[in] ...

  @return
  0  - success
  -1 - failure

  @dependencies
  None.

*/
static int uClockQDI_Invoke(int hClientHandle,
                          qurt_qdi_obj_t *pobj,
                          int method,
                          qurt_qdi_arg_t a1,
                          qurt_qdi_arg_t a2,
                          qurt_qdi_arg_t a3,
                          qurt_qdi_arg_t a4,
                          qurt_qdi_arg_t a5,
                          qurt_qdi_arg_t a6,
                          qurt_qdi_arg_t a7,
                          qurt_qdi_arg_t a8,
                          qurt_qdi_arg_t a9)
{
  switch (method)
  {
    case QDI_OPEN:
      return uClock_QDIOpen(hClientHandle, (void *)pobj, a1.ptr, a2.num);

    case UCLOCK_QDI_CLOCK_CMD_ROUTER:
      return uClock_QDIRouteCommand(hClientHandle, a1.num, a2.num, a3.num);

    case QDI_ISLAND_CHECK_ALLOC:
      return 1;

    default:
      return qurt_qdi_method_default(hClientHandle, pobj, method,
                                     a1, a2, a3, a4, a5, a6, a7, a8, a9);
  }

  return -1;

} /* End uClockQDI_Invoke */




/*=============================================================================
 ==============================================================================

  A P P L I C A T I O N    P R O G R A M M E R   I N T E R F A C E

 ==============================================================================
 ============================================================================*/


/* ============================================================================
**  Function : uClock_Init
** ============================================================================
*/
/**
  Initializes the Clock driver.

  @return
  TRUE  - Driver was initialized.
  FALSE - Otherwise.

  @dependencies
  None.

*/
boolean uClock_Init(void)
{
  qurt_mutex_init(&ClockDriverCtxt.Mutex);

  if (qurt_qdi_register_devname(uClockQDIName, &uClockQDIOpener) < 0)
  {
    return(FALSE);
  }

  ClockDriverCtxt.bInitialized = TRUE;
  
  return (TRUE);

} /* Clock_Init */


/* ============================================================================
**  Function : uClock_IsClockOn
** ============================================================================
*/
/**
  Internal API which returns whether a clock is on or not.

  @param[in]  eClockId  - The ID of the clock to enable.

  @return
  TRUE  - The clock is on.
  FALSE - Otherwise.

  @dependencies
  None.

*/
boolean uClock_IsClockOn(uClockIdType eClockId)
{
  if(eClockId < CLOCK_TOTAL_CLOCK_ENUMS)
  { 
    if (in_dword(gnClockRegs[eClockId].nCbcAddr) & CLOCK_CBCR_CLK_OFF_BMSK)
    {
      return(FALSE);
    }
    else
    {
      return(TRUE);
    }
  }

  return(FALSE);

} /* uClock_IsClockOn */


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
  boolean bRetVal = FALSE;
  uint32 nTimeout = 150;

  if(eClockId < CLOCK_TOTAL_CLOCK_ENUMS)
  {
    /*
     * Synchronize.
     */
    qurt_mutex_lock(&ClockDriverCtxt.Mutex);

    if(ClockDriverCtxt.anClockReferences[eClockId] == 0)
    {
      /*
       * Clock is not already on, so enable it here.
       */
      out_dword(gnClockRegs[eClockId].nVoteAddr,
          (in_dword(gnClockRegs[eClockId].nVoteAddr) | gnClockRegs[eClockId].nVoteBmsk));

      /*
       * Make sure the clock is on.
       */
      while(uClock_IsClockOn(eClockId) != TRUE)
      {
        if(nTimeout == 0)
        {
          break;
        }
        busywait(2);
        nTimeout--;
      }

      /*
       * Increase the reference count on this clock.
       */
      if(nTimeout != 0)
      {
        ClockDriverCtxt.anClockReferences[eClockId]++;
        bRetVal = TRUE;
      }
    }
    else
    {
      /*
       * Clock is already on so just increase reference count.
       */
      ClockDriverCtxt.anClockReferences[eClockId]++;
      bRetVal = TRUE;
    }

    /*
     * Release lock.
     */
    qurt_mutex_unlock(&ClockDriverCtxt.Mutex);
	
  }

  return(bRetVal);

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
  if(eClockId < CLOCK_TOTAL_CLOCK_ENUMS)
  {
    /*
     * Synchronize.
     */
    qurt_mutex_lock(&ClockDriverCtxt.Mutex);

    /*
     * Only enable the clock if it's not already enabled.
     */
    if(ClockDriverCtxt.anClockReferences[eClockId] > 0)
    {
      ClockDriverCtxt.anClockReferences[eClockId]--;
    }

    if(ClockDriverCtxt.anClockReferences[eClockId] == 0)
    {
      out_dword(gnClockRegs[eClockId].nVoteAddr,
          (in_dword(gnClockRegs[eClockId].nVoteAddr) & ~(gnClockRegs[eClockId].nVoteBmsk)));
    }

    /*
     * Release lock.
     */
    qurt_mutex_unlock(&ClockDriverCtxt.Mutex);

    return(TRUE);
  }

  return(FALSE);

} /* uClock_DisableClock */

