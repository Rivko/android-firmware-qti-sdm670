/*
===========================================================================

FILE:         SlimBusSatStub.c

DESCRIPTION:  This file implements stub satellite functionality for the 
              SLIMbus core driver.

===========================================================================

                             Edit History

$Header: //components/rel/core.qdsp6/2.1.c4/buses/slimbus/src/common/SlimBusSatStubs.c#1 $

when       who     what, where, why
--------   ---     --------------------------------------------------------
11/15/16   MJS     Remove more references to DAL.
10/26/12   MJS     Add interrupt disable function.
09/21/12   MJS     Add interrupt enable function.
06/13/12   MJS     Support for standlone satellite low-power management.
04/04/12   MJS     Fixes for level 4 warnings.
12/11/11   MJS     Remove references to DAL.
09/26/11   MJS     Fix to allow master driver to call WaitForMasterAck().
05/25/11   MJS     Add separate satellite hardware init function.
11/01/10   MJS     Initial revision.

===========================================================================
             Copyright (c) 2010-2016 Qualcomm Technologies Incorporated.
                    All Rights Reserved.
                  QUALCOMM Proprietary/GTDR
===========================================================================
*/

#include "SlimBusTarget.h"
#include SLIMBUS_OS_H
#include "SlimBus.h"
#include "SlimBusSat.h"


/*------------------------------------------------------------------------------
Driver functions
------------------------------------------------------------------------------*/

/**
 * @brief Checks if the driver binary supports satellite-specific logic 
 *
 * This function checks whether the compiled driver binary supports 
 * satellite-specific logic
 * 
 * @return TRUE if satellite-specfic logic is supported, FALSE otherwise 
 */
SBBOOL SlimBus_IsSatSupported( void )
{
  return FALSE;
}

/**
 * @brief Process a satellite-specific received message
 *
 * Process a satellite-specific received message 
 * 
 * @param[in] pDevCtxt  Pointer to the device context
 * @param[in] length  Length of the message
 * @param[in] data  First word of the received message
 * @param[in] pDataBuf  Pointer to the second word in the 
 *       received message buffer
 * 
 * @return SB_SUCCESS on success, error code on error 
 */
SBResult SlimBus_ProcessSatRxMsg(SlimBusDevCtxt *pDevCtxt, uint32 length, uint32 data, uint32 *pDataBuf)
{
  UNREFERENCED_PARAMETER(pDataBuf);
  UNREFERENCED_PARAMETER(data);
  UNREFERENCED_PARAMETER(length);
  UNREFERENCED_PARAMETER(pDevCtxt);

  return SB_ERROR;
}

/**
 * @brief Wait for acknowledgement response from the master 
 *        driver
 *
 * This function waits for an acknowledgement response from the 
 * master driver 
 * 
 * @param[in] pClientCtxt  Pointer to the client context
 * @param[in] pEventNode  Pointer to event node structure used 
 *       for completion notification
 * @param[in] TID  Transaction ID
 * @param[in] puMarker  Pointer to location to store ack marker 
 *       data
 * 
 * @return SB_SUCCESS on success, error code on error 
 */
SBResult SlimBus_WaitForMasterAck
(
  SlimBusClientCtxt *pClientCtxt,
  SlimBusEventNodeType *pEventNode,
  uint8 TID,
  uint32 *puMarker
)
{
  SlimBusDevCtxt *pDevCtxt = SB_GET_PDEVCTXT(pClientCtxt->hDevCtxt);

  UNREFERENCED_PARAMETER(pEventNode);
  UNREFERENCED_PARAMETER(TID);
  UNREFERENCED_PARAMETER(puMarker);

  if (pDevCtxt->eDrvType == SB_SATELLITE_DRIVER)
  {
    return SB_ERROR;
  }
  else
  {
    return SB_SUCCESS;
  }
}

/**
 * @brief Query the master driver for a logical address
 *
 * This function queries the remote master driver for a Slimbus 
 * native device logical address 
 * 
 * @param[in] pClientCtxt  Pointer to the client context
 * @param[in] pEA  Pointer to the enumeration address to query
 * @param[in] uEASize  Size of the enumeration address
 * @param[out] pLA  Pointer to location to store the logical 
 *       address that was looked up
 * 
 * @return SB_SUCCESS on success, error code on error 
 */
SBResult SlimBus_QueryRemoteLogicalAddr
(
  SlimBusClientCtxt *pClientCtxt,
  const uint8 *pEA,
  uint32 uEASize,
  SlimBusLogicalAddrType * pLA
)
{
  UNREFERENCED_PARAMETER(pClientCtxt);
  UNREFERENCED_PARAMETER(pEA);
  UNREFERENCED_PARAMETER(uEASize);
  UNREFERENCED_PARAMETER(pLA);

  return SB_ERROR;
}

/**
 * @brief Check for standalone satellite mode and perform
 *        standalone satellite initialization if the mode is
 *        detected
 *
 * This function checks for standalone satellite mode and 
 * performs the standalone satellite initialization if the mode 
 * is detected 
 * 
 * @param[in] pDevCtxt  Pointer to the device context
 */
void SlimBus_CheckInitStandaloneSat(SlimBusDevCtxt *pDevCtxt)
{
  UNREFERENCED_PARAMETER(pDevCtxt);
}

/**
 * @brief Perform satellite-specific device driver 
 *        initialization
 *
 * This function performs satellite-specific device driver 
 * initialization 
 * 
 * @param[in] pDevCtxt  Pointer to the device context
 * 
 * @return SB_SUCCESS on sucess, error code on error 
 */
SBResult SlimBus_DeviceInitSat(SlimBusDevCtxt *pDevCtxt)
{
  UNREFERENCED_PARAMETER(pDevCtxt);

  return SB_ERROR;
}

/**
 * @brief Enable satellite-specific interrupts
 *
 * This function enables satellite-specific interrupts
 * 
 * @param[in] pDevCtxt  Pointer to the device context
 * 
 * @return SB_SUCCESS on sucess, error code on error 
 */
SBResult SlimBus_DeviceEnableSatInterrupts(SlimBusDevCtxt *pDevCtxt)
{
  UNREFERENCED_PARAMETER(pDevCtxt);

  return SB_ERROR;
}

/**
 * @brief Disable satellite-specific interrupts
 *
 * This function disables satellite-specific interrupts
 * 
 * @param[in] pDevCtxt  Pointer to the device context
 * 
 * @return SB_SUCCESS on sucess, error code on error 
 */
SBResult SlimBus_DeviceDisableSatInterrupts(SlimBusDevCtxt *pDevCtxt)
{
  UNREFERENCED_PARAMETER(pDevCtxt);

  return SB_ERROR;
}

/**
 * @brief Perform satellite-specific device driver hardware
 *        initialization
 *
 * This function performs satellite-specific device driver 
 * hardware initialization 
 * 
 * @param[in] pDevCtxt  Pointer to the device context
 * 
 * @return SB_SUCCESS on sucess, error code on error 
 */
SBResult SlimBus_DeviceInitHardwareSat(SlimBusDevCtxt *pDevCtxt)
{
  UNREFERENCED_PARAMETER(pDevCtxt);

  return SB_ERROR;
}

