/*
===========================================================================

FILE:         SlimBusMasterStubs.c

DESCRIPTION:  This file implements stub master functionality for the 
              SLIMbus core driver.

===========================================================================

                             Edit History

$Header: //components/rel/core.qdsp6/2.1.c4/buses/slimbus/src/common/SlimBusMasterStubs.c#1 $

when       who     what, where, why
--------   ---     --------------------------------------------------------
07/17/15   MJS     Add stubs for SSReset and SSRestart.
10/26/12   MJS     Add interrupt disable function.
09/21/12   MJS     Add interrupt enable function.
06/13/12   MJS     Support for standlone satellite low-power management.
12/11/11   MJS     Remove references to DAL.
05/24/11   MJS     Add pause clock support, separate hardware init function.
05/24/11   MJS     Add master-specific interrupt handling function.
11/01/10   MJS     Initial revision.

===========================================================================
             Copyright (c) 2010, 2011, 2012, 2015 QUALCOMM Technologies Incorporated.
                    All Rights Reserved.
                  QUALCOMM Proprietary/GTDR
===========================================================================
*/

#include "SlimBusTarget.h"
#include SLIMBUS_OS_H
#include "SlimBus.h"
#include "SlimBusMaster.h"


/*------------------------------------------------------------------------------
Driver functions
------------------------------------------------------------------------------*/

/**
 * @brief Checks if the driver binary supports master-specific logic 
 *
 * This function checks whether the compiled driver binary supports 
 * master-specific logic
 * 
 * @return TRUE if master-specfic logic is supported, FALSE otherwise 
 */
SBBOOL SlimBus_IsMasterSupported( void )
{
  return FALSE;
}

/**
 * @brief Mark internal state as having recovered from pause 
 *        clock
 *
 * This function marks the internal state as having recovered 
 * from pause clock. 
 * 
 * @param[in] pDevCtxt  Pointer to the device context
 * 
 * @return SB_SUCCESS on success, error code on error
 */
SBResult SlimBus_RecoverFromPauseClock(SlimBusDevCtxt *pDevCtxt)
{
  UNREFERENCED_PARAMETER(pDevCtxt);

  return SB_SUCCESS;
}

/**
 * @brief Process a master-specific received message
 *
 * Process a master-specific received message 
 * 
 * @param[in] pDevCtxt  Pointer to the device context
 * @param[in] length  Length of the message
 * @param[in] data  First word of the received message
 * @param[in] pDataBuf  Pointer to the second word in the 
 *       received message buffer
 * 
 * @return SB_SUCCESS on success, error code on error 
 */
SBResult SlimBus_ProcessMasterRxMsg
(
  SlimBusDevCtxt *pDevCtxt,
  uint32 length,
  uint32 data,
  uint32 *pDataBuf
)
{
  UNREFERENCED_PARAMETER(pDevCtxt);
  UNREFERENCED_PARAMETER(length);
  UNREFERENCED_PARAMETER(data);
  UNREFERENCED_PARAMETER(pDataBuf);

  return SB_ERROR;
}

/**
 * @brief Handle master-specific interrupts
 *
 * This function processes master-specific interrupts
 * 
 * @param[in] pDevCtxt  Pointer to the device context
 * @param[in] uIntStatus  The messaging device interrupt status
 * 
 * @return SB_SUCCESS on success, error code on error 
 */
SBResult SlimBus_HandleMasterInterrupts
(
  SlimBusDevCtxt *pDevCtxt,
  struct SlimBusIsrCtxt *pIsrCtxt
)
{
  UNREFERENCED_PARAMETER(pDevCtxt);
  UNREFERENCED_PARAMETER(pIsrCtxt);

  return SB_ERROR;
}

/**
 * @brief Callback for SlimBus master workloop for handling 
 *        remote reconfiguration sequences
 *
 * Callback function for SlimBus master workloop for handling 
 * remote reconfiguration sequences 
 * 
 * @param[in] hEvent  Workloop event handle
 * @param[in] pVoid  Workloop callback context
 * 
 * @return SB_SUCCESS 
 */
SBResult SlimBusMasterWorkLoopHandler(SlimBusEventHandle hEvent, void *pVoid)
{
  UNREFERENCED_PARAMETER(hEvent);
  UNREFERENCED_PARAMETER(pVoid);
  
  return SB_ERROR;
}

/**
 * @brief Perform master-specific device driver initialization 
 *
 * This function performs master-specific device driver 
 * initialization 
 * 
 * @param[in] pDevCtxt  Pointer to the device context
 * 
 * @return SB_SUCCESS on sucess, error code on error 
 */
SBResult SlimBus_DeviceInitMaster(SlimBusDevCtxt *pDevCtxt)
{
  UNREFERENCED_PARAMETER(pDevCtxt);

  return SB_ERROR;
}


/**
 * @brief Enable master-specific interrupts
 *
 * This function enables master-specific interrupts
 * 
 * @param[in] pDevCtxt  Pointer to the device context
 * 
 * @return SB_SUCCESS on sucess, error code on error 
 */
SBResult SlimBus_DeviceEnableMasterInterrupts(SlimBusDevCtxt *pDevCtxt)
{
  UNREFERENCED_PARAMETER(pDevCtxt);

  return SB_ERROR;
}


/**
 * @brief Disable master-specific interrupts
 *
 * This function disables master-specific interrupts
 * 
 * @param[in] pDevCtxt  Pointer to the device context
 * 
 * @return SB_SUCCESS on sucess, error code on error 
 */
SBResult SlimBus_DeviceDisableMasterInterrupts(SlimBusDevCtxt *pDevCtxt)
{
  UNREFERENCED_PARAMETER(pDevCtxt);

  return SB_ERROR;
}


/**
 * @brief Perform master-specific device driver hardware 
 *        initialization
 *
 * This function performs master-specific device driver hardware 
 * initialization 
 * 
 * @param[in] pDevCtxt  Pointer to the device context
 * 
 * @return SB_SUCCESS on sucess, error code on error 
 */
SBResult SlimBus_DeviceInitHardwareMaster(SlimBusDevCtxt *pDevCtxt)
{
  UNREFERENCED_PARAMETER(pDevCtxt);

  return SB_ERROR;
}

/**
 * @brief Reset routine before SSR restart is performed.
 *  
 * This function does the cleanup on Subsystem Reset 
 * 
 * @param[in] pDevCtxt      Device Ctxt 
 * 
 * @return None 
 */
void SlimBus_SSReset(SlimBusDevCtxt *pDevCtxt)
{
  UNREFERENCED_PARAMETER(pDevCtxt);
}

/**
 * @brief Restart routine after SSR restart is done.
 *  
 * This function does the re-initialization on Subsystem Restart
 * 
 * @param[in] pDevCtxt      Device Ctxt 
 * 
 * @return None 
 */
void SlimBus_SSRestart(SlimBusDevCtxt *pDevCtxt)
{
  UNREFERENCED_PARAMETER(pDevCtxt);
}

