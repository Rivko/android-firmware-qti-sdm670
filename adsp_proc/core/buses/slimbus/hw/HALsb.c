/*============================================================================
  @file         HALsb.c

  @brief        This file contain the HAL (Hardware Abstraction Layer) 
                Slimbus API (Application Programin Interface).

  @details      The HAL purpose is more "Register Abstraction" rather than
                "Device Abstraction".
 
                Copyright (c) 2010-2017 Qualcomm Technologies Incorporated.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
============================================================================*/

/*============================================================================
  EDIT HISTORY FOR MODULE

  $Header: //components/rel/core.qdsp6/2.1/buses/slimbus/hw/HALsb.c#1 $

  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  2017-03-03  MJS  Update FIFO level enumerations for core version 2.1.0.
  2016-10-03  MJS  Support for full secondary line bandwidth scheduling.
  2016-04-14  MJS  Enable FRM_CGC_IDLE_LOW for retention support.
  2016-03-24  MJS  Retention support.
  2016-02-05  MJS  Add support for multiple endpoints per port.
  2015-10-22  MJS  Update port selector logic for core version 1.5.0.
  2015-04-22  NSD  Add APIs to check for hardware superframe sync shift support
                   and also to enable/disable the same.
  2014-04-23  MJS  Add IsEnumerated support for PGD device.
  2014-04-30  MJS  Check reset ACK bit for core version 1.3.0.
  2014-04-09  MJS  Change software reset delay.
  2013-02-28  MJS  Add PGD port get events API.
  2013-08-19  MJS  Fix API name.
  2013-07-31  MJS  Support for shadow counter, upper watermark, port DMA
                   interrupt and hardware optional capabilities.
  2013-07-24  MJS  Update progress counter index for pre-Gandalf.
  2013-06-17  MJS  Deprecate HALsbTarget.h.
  2013-04-30  MJS  Add get assigned LA APIs.
  2013-04-29  MJS  Add IsEnumerated support for framer and interface devices.
  2013-03-04  MJS  Add platform environment configuration.
  2012-07-29  MJS  Fixes for level 4 warnings.
  2011-06-09  MJS  Add shadow register support.
  2010-11-15  MJS  Don't read-modify-write the read-only TxMsg device CSRs.
  2010-10-22  ps   Updated few fields to reflect the changes in new HWIO
  2010-07-13  ps   Initial version.

============================================================================*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "halsb.h"            /* HAL Slimbus exported API */
#include "HALsbInternal.h"    /* HAL SlimBus private API */

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

/* Address inverval for PC registers is double for pre-Gandalf HWIO */
#define SB_PC_GET_ADJUSTED_INDEX(hHandle, uPcNum) \
  ((SB_HWIO_IN(hHandle,COMP_HW_VERSION) < 0x01030000)?((uPcNum)*2):(uPcNum))
#define SB_PC_GET_ADJUSTED_INDEX2(hHandle, uPcNum, uHwVersion) \
  ((uHwVersion < 0x01030000)?((uPcNum)*2):(uPcNum))


/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Global Data Definitions
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Static Variable Definitions
 * -------------------------------------------------------------------------*/

/* Environment data structure */
static HAL_sb_EnvType sb_env =
{
  NULL,
};

/*----------------------------------------------------------------------------
 * Static Function Declarations and Definitions
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Externalized Function Definitions
 * -------------------------------------------------------------------------*/

/*****************************************************************************/
/*                      HAL Mandatory APIs                                   */
/*****************************************************************************/
/** @name  Mandatory APIs */
/** @{ */

/**
 * Configure the SlimBus HAL environment.
 *  
 * @param[in] pEnv - Pointer to the environment data structure. 
 *
 * @return None
 *
 * @par Dependencies
 *    None
*/ 
void HAL_sb_ConfigEnv(const HAL_sb_EnvType *pEnv)
{
  if ( NULL != pEnv )
  {
    sb_env = *pEnv;
  }
}

/**
 * Initalize the SB HAL. 
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 *
 * @param[out] ppszSbVersion - pointer to SB Version String.
 *
 * @return None
 *
 * @par Dependencies
 *    None
*/  
void HAL_sb_Init(HAL_sb_HandleType hHandle, char **ppszSbVersion)
{
  UNREFERENCED_PARAMETER(hHandle);
  
  if(NULL != ppszSbVersion)
  {
    *ppszSbVersion = HAL_SB_VERSION_STRING;
  }
}


/**
 * Reset the SB 
 *  
 * @param[in] hHandle - The SB hHandle.
 *
 * @return None
 * 
 * @par Dependencies
 *   None.
 */
void HAL_sb_Reset(HAL_sb_HandleType hHandle)
{
  HAL_sb_CompReset(hHandle);
}


/** @} */
/** @name Component Apis */
/** @{ */


/**
 * Get the hardware version 
 * 
 * @param[in] hHandle - This SlimBus hHandle.
 *  
 * @return Hardware Version.
 * 
 * @attention
 *   None.
 */
uint32 HAL_sb_CompGetHwVersion(HAL_sb_HandleType hHandle)
{
  return SB_HWIO_IN(hHandle,COMP_HW_VERSION);
}


/**
 * Check if the combined port DMA interrupt feature is supported
 * 
 * @param[in] hHandle - This SlimBus hHandle.
 *  
 * @return TRUE if the feature is supported, FALSE otherwise. 
 * 
 * @attention
 *   None.
 */
bool32 HAL_sb_IsPortDmaIntSupported(HAL_sb_HandleType hHandle)
{
  return (SB_HWIO_IN(hHandle,COMP_HW_VERSION) >= 0x01020000);
}

/**
 * Check if the shadow progress counter and timestamp features 
 * are supported 
 * 
 * @param[in] hHandle - This SlimBus hHandle.
 *  
 * @return TRUE if the features are supported, FALSE otherwise. 
 * 
 * @attention
 *   None.
 */
bool32 HAL_sb_IsShadowProgressCounterSupported(HAL_sb_HandleType hHandle)
{
  return (SB_HWIO_IN(hHandle,COMP_HW_VERSION) >= 0x01030000);
}

/**
 * Check if the port upper watermark feature is supported 
 * 
 * @param[in] hHandle - This SlimBus hHandle.
 *  
 * @return TRUE if the feature is supported, FALSE otherwise. 
 * 
 * @attention
 *   None.
 */
bool32 HAL_sb_IsPortUpperWatermarkSupported(HAL_sb_HandleType hHandle)
{
  return (SB_HWIO_IN(hHandle,COMP_HW_VERSION) >= 0x01020000);
}

/**
 * Check if the port multi-endpoint feature is supported
 * 
 * @param[in] hHandle - This SlimBus hHandle.
 *  
 * @return TRUE if the feature is supported, FALSE otherwise. 
 * 
 * @attention
 *   None.
 */
bool32 HAL_sb_IsPortMultiEndpointSupported(HAL_sb_HandleType hHandle)
{
  return (SB_HWIO_IN(hHandle,COMP_HW_VERSION) >= 0x02000000);
}

/**
 * Enables the entire component. 
 * 
 * @param[in] hHandle - This SlimBus hHandle.
 * 
 * @return None
 * 
 * @sa HAL_sb_CompDisable, HAL_sb_CompReset
 * 
 * @attention
 *   None.
 */ 
void HAL_sb_CompEnable(HAL_sb_HandleType hHandle)
{
  SB_HWIO_OUTF(hHandle,COMP_CFG,ENABLE,1);
}


/**
 * Disables the entire component. 
 * 
 * @param[in] hHandle - This SlimBus hHandle.
 * 
 * @return None
 * 
 * @sa HAL_sb_CompEnable, HAL_sb_CompReset
 * 
 * @attention
 *   None.
 */
void HAL_sb_CompDisable(HAL_sb_HandleType hHandle)
{
  SB_HWIO_OUTF(hHandle,COMP_CFG,ENABLE,0);
}


/**
 * Check if the component is enabled. 
 * 
 * @param[in] hHandle - This SlimBus hHandle.
 * 
 * @return TRUE, if enabled, FALSE otherwise
 * 
 * @sa HAL_sb_CompEnable, HAL_sb_CompDisable
 * 
 * @attention
 *   None.
 */
bool32 HAL_sb_CompIsEnabled(HAL_sb_HandleType hHandle)
{
  if(0 != SB_HWIO_INF(hHandle,COMP_CFG,ENABLE))
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}


/**
 * Resets the component. 
 * 
 * @param[in] hHandle - This SlimBus hHandle.
 * 
 * @return TRUE on successful reset, FALSE otherwise
 * 
 * @sa HAL_sb_CompEnable, HAL_sb_CompDisable
 * 
 * @attention
 *   None.
 */
bool32 HAL_sb_CompReset(HAL_sb_HandleType hHandle)
{
  bool32 bSuccess = FALSE;

  if (SB_HWIO_IN(hHandle,COMP_HW_VERSION) >= 0x01030000)
  {
    uint32 uCnt = 0;

    SB_HWIO_OUT(hHandle,SW_RESET,1);

    while (uCnt++ < 100)
    {
      if (SB_HWIO_INF(hHandle,SW_RESET,SW_RESET_ACK) != 0)
      {
         bSuccess = TRUE;
         break;
      }
      if(NULL != sb_env.pfnClkPauseUs)
      {
        /* Delay 1us */
        sb_env.pfnClkPauseUs(1);
      }
    }
  }
  else
  {
    SB_HWIO_OUT(hHandle,SW_RESET,1);

    if(NULL != sb_env.pfnClkPauseUs)
    {
      /* Delay 1us */
      sb_env.pfnClkPauseUs(1);
    }

    SB_HWIO_OUT(hHandle,SW_RESET,1);

    bSuccess = TRUE;
  }

  /* Clear reset for HW core v1.3.0 */
  SB_HWIO_OUT(hHandle,SW_RESET,0);

  return bSuccess;
}


/**
 * Assert an Interrupt for a particular Execution environment. 
 * 
 * @param[in] hHandle - This SlimBus hHandle.
 *  
 * @param[in] uEE - EE index.
 * 
 * @return None
 * 
 * @sa HAL_sb_CompdeAssertInterruptEe
 * 
 * @attention
 *   None.
 */
void HAL_sb_CompAssertInterruptEe(HAL_sb_HandleType hHandle, uint32 uEE) 
{
  switch(uEE)
  {
    case 0:
    {
      SB_HWIO_OUTF(hHandle,COMP_TEST,INT_EE0,1);
      break;
    }
    case 1:
    {
      SB_HWIO_OUTF(hHandle,COMP_TEST,INT_EE1,1);
      break;
    }
    case 2:
    {
      SB_HWIO_OUTF(hHandle,COMP_TEST,INT_EE2,1);
      break;
    }
    default:
      break;
  }
}


/**
 * Deassert an Interrupt for a particular Execution environment.
 * 
 * @param[in] hHandle - This SlimBus hHandle.
 *  
 * @param[in] uEE - EE index.
 * 
 * @return None
 * 
 * @sa HAL_sb_CompAssertInterruptEe
 * 
 * @attention
 *   None.
 */
void HAL_sb_CompDeassertInterruptEe(HAL_sb_HandleType hHandle, uint32 uEE)
{
  switch(uEE)
  {
    case 0:
    {
      SB_HWIO_OUTF(hHandle,COMP_TEST,INT_EE0,0);
      break;
    }
    case 1:
    {
      SB_HWIO_OUTF(hHandle,COMP_TEST,INT_EE1,0);
      break;
    }
    case 2:
    {
      SB_HWIO_OUTF(hHandle,COMP_TEST,INT_EE2,0);
      break;
    }
    default:
      break;
  }
}


/**
 * Enable Debug Bus to be actively driven by the Slimbus 
 * Component. 
 * 
 * @param[in] hHandle - This SlimBus hHandle.
 *  
 * @return None
 * 
 * @sa HAL_sb_CompTestBusDisable
 * 
 * @attention
 *   None.
 */
void HAL_sb_CompTestBusEnable(HAL_sb_HandleType hHandle)
{
  SB_HWIO_OUTF(hHandle,COMP_TEST,SB_TESTBUS_EN,1);
}


/**
 * Disable Debug Bus to be actively driven by the Slimbus 
 * Component. 
 * 
 * @param[in] hHandle - This SlimBus hHandle.
 *  
 * @return None
 * 
 * @sa HAL_sb_CompTestBusEnable
 * 
 * @attention
 *   None.
 */
void HAL_sb_CompTestBusDisable(HAL_sb_HandleType hHandle)
{
  SB_HWIO_OUTF(hHandle,COMP_TEST,SB_TESTBUS_EN,0);
}


/**
 * Check if Debug Bus is enabled to be actively driven by the 
 * Slimbus Component. 
 * 
 * @param[in] hHandle - This SlimBus hHandle.
 *  
 * @return True, if enabled, FALSE otherwise
 * 
 * @sa HAL_sb_CompTestBusDisable, HAL_sb_CompTestBusEnable
 * 
 * @attention
 *   None.
 */
bool32 HAL_sb_CompTestBusIsEnabled(HAL_sb_HandleType hHandle)
{
  if(0 != SB_HWIO_INF(hHandle,COMP_TEST,SB_TESTBUS_EN))
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}


/**
 * Set the Test Bus Select. 
 * 
 * @param[in] hHandle - This SlimBus hHandle.
 *  
 * @param[in] eTestBus - Test Bus type.
 *  
 * @return None
 * 
 * @sa HAL_sb_CompGetTestBusSelect
 * 
 * @attention
 *   None.
 */
void HAL_sb_CompSetTestBusSelect(HAL_sb_HandleType hHandle, 
                              HAL_sb_TestBusType eTestBus)
{
  SB_HWIO_OUTF(hHandle,COMP_TEST,SB_TESTBUS_SEL,eTestBus);
}


/**
 * Get the Test Bus Select. 
 * 
 * @param[in] hHandle - This SlimBus hHandle.
 *  
 * @return Test Bus type.
 * 
 * @sa HAL_sb_CompSetTestBusSelect
 * 
 * @attention
 *   None.
 */
HAL_sb_TestBusType HAL_sb_CompGetTestBusSelect(HAL_sb_HandleType hHandle)
{
  return ((HAL_sb_TestBusType)SB_HWIO_INF(hHandle,COMP_TEST,SB_TESTBUS_SEL));
}


/**
 * Blocks Reconfig messages from being sent or executed if 
 * received. 
 * 
 * @param[in] hHandle - This SlimBus hHandle.
 *  
 * @return None
 * 
 * @sa HAL_sb_CompUnhaltReconfig, HAL_sb_CompIsHaltReconfig.
 * 
 * @attention
 *   None.
 */
void HAL_sb_CompHaltReconfig(HAL_sb_HandleType hHandle)
{
  SB_HWIO_OUTF(hHandle,COMP_TRUST_CFG,HALT_RECONFIG,1);
}


/**
 * Unblocks Reconfig messages from being sent or executed if 
 * received. 
 * 
 * @param[in] hHandle - This SlimBus hHandle.
 *  
 * @return None
 * 
 * @sa HAL_sb_CompHaltReconfig, HAL_sb_CompIsHaltReconfig.
 * 
 * @attention
 *   None.
 */
void HAL_sb_CompUnhaltReconfig(HAL_sb_HandleType hHandle)
{
  SB_HWIO_OUTF(hHandle,COMP_TRUST_CFG,HALT_RECONFIG,0);
}


/**
 * Check if Reconfig messages are blocked from being sent or
 * executed if received. 
 * 
 * @param[in] hHandle - This SlimBus hHandle.
 *  
 * @return TRUE, if reconfig is blocked, FALSE otherwise
 * 
 * @sa HAL_sb_CompHaltReconfig, HAL_sb_CompUnhaltReconfig
 * 
 * @attention
 *   None.
 */
bool32 HAL_sb_CompIsHaltReconfig(HAL_sb_HandleType hHandle)
{
  if(0 != SB_HWIO_INF(hHandle,COMP_TRUST_CFG,HALT_RECONFIG))
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}


/**
 * Blocks SW Reset. 
 * 
 * @param[in] hHandle - This SlimBus hHandle.
 *  
 * @return None
 * 
 * @sa HAL_sb_CompUnhaltSwReset, HAL_sb_CompIsHaltSwReset
 * 
 * @attention
 *   None.
 */
void HAL_sb_CompHaltSwReset(HAL_sb_HandleType hHandle)
{
  SB_HWIO_OUTF(hHandle,COMP_TRUST_CFG,HALT_SW_RESET,1);
}


/**
 * Unblocks SW Reset. 
 * 
 * @param[in] hHandle - This SlimBus hHandle.
 *  
 * @return None
 * 
 * @sa HAL_sb_CompHaltSwReset, HAL_sb_CompIsHaltSwReset
 * 
 * @attention
 *   None.
 */
void HAL_sb_CompUnhaltSwReset(HAL_sb_HandleType hHandle)
{
  SB_HWIO_OUTF(hHandle,COMP_TRUST_CFG,HALT_SW_RESET,0);
}


/**
 * Check to see if SW Reset is blocked. 
 * 
 * @param[in] hHandle - This SlimBus hHandle.
 *  
 * @return TRUE, if SW Reset is blocked, FALSE otherwise
 * 
 * @sa HAL_sb_CompHaltSwReset, HAL_sb_CompUnhaltSwReset.
 * 
 * @attention
 *   None.
 */
bool32 HAL_sb_CompIsHaltSwReset(HAL_sb_HandleType hHandle)
{
  if(0 != SB_HWIO_INF(hHandle,COMP_TRUST_CFG,HALT_SW_RESET))
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}


/**
 * Blocks changes to the EN bit of all devices in this 
 * component. 
 * 
 * @param[in] hHandle - This SlimBus hHandle.
 *  
 * @return None
 * 
 * @sa HAL_sb_CompUnhaltEnChange, HAL_sb_CompIsHaltEnChange
 * 
 * @attention
 *   None.
 */
void HAL_sb_CompHaltEnChange(HAL_sb_HandleType hHandle)
{
  SB_HWIO_OUTF(hHandle,COMP_TRUST_CFG,HALT_EN_CHANGE,1);
}


/**
 * Unlocks changes to EN bit of all devices in this component. 
 * 
 * @param[in] hHandle - This SlimBus hHandle.
 *  
 * @return None
 * 
 * @sa HAL_sb_CompUnHaltEnChange, HAL_sb_CompIsHaltEnChange
 * 
 * @attention
 *   None.
 */
void HAL_sb_CompUnhaltEnChange(HAL_sb_HandleType hHandle)
{
  SB_HWIO_OUTF(hHandle,COMP_TRUST_CFG,HALT_EN_CHANGE,0);
}

/**
 * Check if changes to EN bit of all devices in this component 
 * is blocked. 
 * 
 * @param[in] hHandle - This SlimBus hHandle.
 *  
 * @return TRUE, if blocked, FALSE otherwise
 * 
 * @sa HAL_sb_CompUnhaltEnChange, HAL_sb_CompUnhaltEnChange
 * 
 * @attention
 *   None.
 */
bool32 HAL_sb_CompIsHaltEnChange(HAL_sb_HandleType hHandle)
{
  if(0 != SB_HWIO_INF(hHandle,COMP_TRUST_CFG,HALT_EN_CHANGE))
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}


/**
 * Blocks Slimbus Devices to send any Slimbus message to the 
 * trusted device. 
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 *  
 * @param[in] eSbDevice - Device type. 
 *  
 * @return None
 * 
 * @sa HAL_sb_CompUnhaltDeviceTxMsg, HAL_sb_CompIsHaltDeviceTxMsg
 * 
 * @attention
 *   None.
 */
void HAL_sb_CompHaltDeviceTxMsg(HAL_sb_HandleType hHandle, 
                                HAL_sb_DeviceType eSbDevice)
{
  switch(eSbDevice)
  {
    case HAL_SB_DEVICE_NON_PORTED_GENERIC_1:
    {
      SB_HWIO_OUTF(hHandle,COMP_TRUST_CFG,HALT_NGD1_MSG_TX,1);
      break;
    }
    case HAL_SB_DEVICE_NON_PORTED_GENERIC_2:
    {
      SB_HWIO_OUTF(hHandle,COMP_TRUST_CFG,HALT_NGD2_MSG_TX,1);
      break;
    }
    case HAL_SB_DEVICE_MANAGER:
    {
      SB_HWIO_OUTF(hHandle,COMP_TRUST_CFG,HALD_MGR_MSG_TX,1);
      break;
    }
    default:
      break;
  }
}


/**
 * Unblocks Slimbus Device to send any Slimbus message to the 
 * trusted device. 
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 *  
 * @param[in] eSbDevice - Device type. 
 *  
 * @return None
 * 
 * @sa HAL_sb_CompHaltDeviceTxMsg, HAL_sb_CompIsHaltDeviceTxMsg
 * 
 * @attention
 *   None.
 */
void HAL_sb_CompUnhaltDeviceTxMsg(HAL_sb_HandleType hHandle, 
                                  HAL_sb_DeviceType eSbDevice)
{
  switch(eSbDevice)
  {
    case HAL_SB_DEVICE_NON_PORTED_GENERIC_1:
    {
      SB_HWIO_OUTF(hHandle,COMP_TRUST_CFG,HALT_NGD1_MSG_TX,0);
      break;
    }
    case HAL_SB_DEVICE_NON_PORTED_GENERIC_2:
    {
      SB_HWIO_OUTF(hHandle,COMP_TRUST_CFG,HALT_NGD2_MSG_TX,0);
      break;
    }
    case HAL_SB_DEVICE_MANAGER:
    {
      SB_HWIO_OUTF(hHandle,COMP_TRUST_CFG,HALD_MGR_MSG_TX,0);
      break;
    }
    default:
      break;
  }
}


/**
 * Check if Slimbus Device are blocked to send any Slimbus 
 * message to the trusted device. 
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 *  
 * @param[in] eSbDevice - Device type. 
 *  
 * @return TRUE, if device is blocked, FALSE otherwise
 * 
 * @sa HAL_sb_CompHaltDeviceTxMsg, HAL_sb_CompUnhaltDeviceTxMsg
 * 
 * @attention
 *   None.
 */
bool32 HAL_sb_CompIsHaltDeviceTxMsg(HAL_sb_HandleType hHandle, 
                                    HAL_sb_DeviceType eSbDevice)
{
  bool32 bResult = FALSE;
  switch(eSbDevice)
  {
    case HAL_SB_DEVICE_NON_PORTED_GENERIC_1:
    {
      if(0 != SB_HWIO_INF(hHandle,COMP_TRUST_CFG,HALT_NGD1_MSG_TX))
      {
        bResult = TRUE;
      }
      break;
    }
    case HAL_SB_DEVICE_NON_PORTED_GENERIC_2:
    {
      if(0 != SB_HWIO_INF(hHandle,COMP_TRUST_CFG,HALT_NGD2_MSG_TX))
      {
        bResult = TRUE;
      }
      break;
    }
    case HAL_SB_DEVICE_MANAGER:
    {
      if(0 != SB_HWIO_INF(hHandle,COMP_TRUST_CFG,HALD_MGR_MSG_TX))
      {
        bResult = TRUE;
      }
      break;
    }
    default:
      break;
  }
  return bResult;
}


/**
 * Set the Execution Environment value for NGDs. NGDs interrupts
 * will be routed to this programmed EE. 
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 *  
 * @param[in] eNGD - NGD index.  
 *  
 * @param[in] uEE - EE index.
 *   
 * @return None
 * 
 * @sa HAL_sb_CompGetNgdEe
 * 
 * @attention
 *   None.
 */
void HAL_sb_CompSetNgdEe(HAL_sb_HandleType hHandle, 
                         HAL_sb_NgdDeviceType eNGD, 
                         uint32 uEE)
{
  switch(eNGD)
  {
    case  HAL_SB_NON_PORTED_GENERIC_1:
    {
      SB_HWIO_OUTF(hHandle,COMP_TRUST_CFG,EE_FOR_NGD1,uEE);
      break;
    }    
    case  HAL_SB_NON_PORTED_GENERIC_2:
    {
      SB_HWIO_OUTF(hHandle,COMP_TRUST_CFG,EE_FOR_NGD2,uEE);
      break;
    }
    default:
      break;
  }
}


/**
 * Get the Execution Environment value for NGDs. NGDs interrupts
 * are routed to this programmed EE. 
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 *  
 * @param[in] eNGD - NGD index.  
 *   
 * @return EE index.
 * 
 * @sa HAL_sb_CompSetNgdEe
 * 
 * @attention
 *   None.
 */
uint32 HAL_sb_CompGetNgdEe(HAL_sb_HandleType hHandle, 
                                  HAL_sb_NgdDeviceType eNGD)
{
  
  uint32 uEE = 0;
  switch(eNGD)
  {
    case  HAL_SB_NON_PORTED_GENERIC_1:
    {
      uEE = (uint32)SB_HWIO_INF(hHandle,COMP_TRUST_CFG,EE_FOR_NGD1);
      break;
    }    
    case  HAL_SB_NON_PORTED_GENERIC_2:
    {
      uEE = (uint32)SB_HWIO_INF(hHandle,COMP_TRUST_CFG,EE_FOR_NGD2);
      break;
    }
    default:
      break;
  }
  return uEE;
}


/**
 * Set the Execution Environment value for Manager Resource 
 * Group. Interrupts will be routed to this programmed EE. 
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 *  
 * @param[in] uEE - EE index.
 *   
 * @return None
 * 
 * @sa HAL_sb_CompGetMgrRscGrpEe
 * 
 * @attention
 *   None.
 */
void HAL_sb_CompSetMgrRscGrpEe(HAL_sb_HandleType hHandle, uint32 uEE)
{
  SB_HWIO_OUTF(hHandle,COMP_TRUST_CFG,EE_FOR_MGR_RSRC_GRP,uEE);
}

/**
 * Get the Execution Environment value for Manager Resource 
 * Group. Interrupts will be routed to this programmed EE. 
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 *   
 * @return EE index.
 * 
 * @sa HAL_sb_CompSetMgrRscGrpEe
 * 
 * @attention
 *   None.
 */
uint32 HAL_sb_CompGetMgrRscGrpEe(HAL_sb_HandleType hHandle)
{
  return (uint32)SB_HWIO_INF(hHandle,COMP_TRUST_CFG,EE_FOR_MGR_RSRC_GRP);
}


/**
 * Enables the Manager to monitor for said Trusted device 
 * access. 
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 *   
 * @return None.
 * 
 * @sa HAL_sb_CompMgrSetTrustDevAccessDisable, 
 *     HAL_sb_CompMgrIsTrustDevAccessEnabled
 * 
 * @attention
 *   None.
 */
void HAL_sb_CompMgrSetTrustDevAccessEnable(HAL_sb_HandleType hHandle)
{
  SB_HWIO_OUTF(hHandle,COMP_TRUST_DEV1,TRUST_DEV_EN,1);
}

/**
 * Disables the Manager to monitor for said Trusted device 
 * access. 
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 *   
 * @return None.
 * 
 * @sa HAL_sb_CompMgrSetTrustDevAccessEnable, 
 *     HAL_sb_CompMgrIsTrustDevAccessEnabled
 * 
 * @attention
 *   None.
 */
void HAL_sb_CompMgrSetTrustDevAccessDisable(HAL_sb_HandleType hHandle)
{
  SB_HWIO_OUTF(hHandle,COMP_TRUST_DEV1,TRUST_DEV_EN,0);
}


/**
 * Ckecks if the Manager is enabled to monitor for said Trusted 
 * device access. 
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 *   
 * @return  True, If Trusted Device Access is enabled, FALSE 
 *          otherwise
 * 
 * @sa HAL_sb_CompMgrSetTrustDevAccessDisable, 
 *     HAL_sb_CompMgrGetTrustDevAccessEnable
 * 
 * @attention
 *   None.
 */
bool32 HAL_sb_CompMgrIsTrustDevAccessEnabled(HAL_sb_HandleType hHandle)
{
  if(0 != SB_HWIO_INF(hHandle,COMP_TRUST_DEV1,TRUST_DEV_EN))
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}


/**
 * Set the Logical address of the Trusted Device. 
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 *  
 * @param[in] uLogicalAddress - Logical Address. 
 *  
 * @return None.
 * 
 * @sa HAL_sb_CompGetTrustDevLa. 
 * 
 * @attention
 *   None.
 */
void HAL_sb_CompSetTrustDevLa(HAL_sb_HandleType hHandle, uint32 uLogicalAddress)
{
  SB_HWIO_OUTF(hHandle,COMP_TRUST_DEV1,TRUSTED_LA,uLogicalAddress);
}


/**
 * Get the Logical address of the Trusted Device. 
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 *  
 * @return Logical Address.
 * 
 * @sa HAL_sb_CompSetTrustDevLa. 
 * 
 * @attention
 *   None.
 */
uint32 HAL_sb_CompGetTrustDevLa(HAL_sb_HandleType hHandle)
{
  return (uint32)SB_HWIO_INF(hHandle,COMP_TRUST_DEV1,TRUSTED_LA);
}


/**
 * Set the Enumeration address of the Trusted Device. 
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 *  
 * @param[in] pucEnumAddress - Pointer to the Enumeration address.
 *                             pucEnumAddress[0] = LSB byte of EA
 *                             pucEnumAddress[5] = MSB byte of EA
 *  
 * @return None.
 * 
 * @sa HAL_sb_CompGetTrustDevEa. 
 * 
 * @attention
 *   None.
 */
void HAL_sb_CompSetTrustDevEa(HAL_sb_HandleType hHandle, uint8 *pucEnumAddress)
{
  uint32 *puLsbAddress;
  uint16 *pusMsbAddress;
  if(pucEnumAddress != NULL)
  {
    puLsbAddress = (uint32 *)(pucEnumAddress);
    pusMsbAddress = (uint16 *)(pucEnumAddress + 4);
    SB_HWIO_OUTF(hHandle,COMP_TRUST_DEV1,TRUSTED_EA_MSB,*pusMsbAddress);
    SB_HWIO_OUTF(hHandle,COMP_TRUST_DEV2,TRUSTED_EA_LSB,*puLsbAddress);
  }
}


/**
 * Get the Enumeration address of the Trusted Device. 
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 *
 * @param[out] pucEnumAddress - Pointer to the Enumeration address.
 *                             pucEnumAddress[0] = LSB byte of EA
 *                             pucEnumAddress[5] = MSB byte of EA
 *
 * @return None.
 * 
 * @sa HAL_sb_CompSetTrustDevEa. 
 * 
 * @attention
 *   None.
 */
void HAL_sb_CompGetTrustDevEa(HAL_sb_HandleType hHandle, uint8 *pucEnumAddress)
{
  uint32 *puLsbAddress;
  uint16 *pusMsbAddress;
  if(pucEnumAddress != NULL)
  {
    puLsbAddress = (uint32 *)(pucEnumAddress);
    pusMsbAddress = (uint16 *)(pucEnumAddress + 4);
    *pusMsbAddress = (uint16)SB_HWIO_INF(hHandle,COMP_TRUST_DEV1,TRUSTED_EA_MSB);
    *puLsbAddress = SB_HWIO_INF(hHandle,COMP_TRUST_DEV2,TRUSTED_EA_LSB);
  }
}


/** @} */
/** @name Common Device APIs */
/** @{ */


/**
 * Enable the Device.
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 *  
 * @param[in] eSbDevice - Slimbus device to be enabled. 
 *  
 * @return None.
 * 
 * @sa HAL_sb_DeviceIsEnabled, HAL_sb_DeviceDisable. 
 * 
 * @attention
 *   None.
 */
void HAL_sb_DeviceEnable(HAL_sb_HandleType hHandle, HAL_sb_DeviceType eSbDevice)
{
  switch(eSbDevice)
  {
     case HAL_SB_DEVICE_MANAGER:
     {
       SB_HWIO_OUTF(hHandle,MGR_CFG,ENABLE,1);
       break;
     }
     case HAL_SB_DEVICE_INTERFACE:
     {
       SB_HWIO_OUTF(hHandle,INTF_CFG,ENABLE,1);
       break;
     }
     case HAL_SB_DEVICE_FRAMER:
     {
       SB_HWIO_OUTF(hHandle,FRM_CFG,ENABLE,1);
       break;
     }
     case HAL_SB_DEVICE_PORTED_GENERIC:
     {
       SB_HWIO_OUTF(hHandle,PGD_CFG,ENABLE,1);
       break;
     }
     case HAL_SB_DEVICE_NON_PORTED_GENERIC_1:
     {
       SB_HWIO_OUTF(hHandle,NGD1_CFG,ENABLE,1);
       break;
     }
     case HAL_SB_DEVICE_NON_PORTED_GENERIC_2:
     {
       SB_HWIO_OUTF(hHandle,NGD2_CFG,ENABLE,1);
       break;
     }
     default:
       break;
  }
}


/**
 * Disable the Device.
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 *  
 * @param[in] eSbDevice - Slimbus device to be disabled. 
 *  
 * @return None.
 * 
 * @sa HAL_sb_DeviceIsEnabled, HAL_sb_DeviceEnable. 
 * 
 * @attention
 *   None.
 */
void HAL_sb_DeviceDisable(HAL_sb_HandleType hHandle, 
                          HAL_sb_DeviceType eSbDevice)
{
  switch(eSbDevice)
  {
     case HAL_SB_DEVICE_MANAGER:
     {
       SB_HWIO_OUTF(hHandle,MGR_CFG,ENABLE,0);
       break;
     }
     case HAL_SB_DEVICE_INTERFACE:
     {
       SB_HWIO_OUTF(hHandle,INTF_CFG,ENABLE,0);
       break;
     }
     case HAL_SB_DEVICE_FRAMER:
     {
       SB_HWIO_OUTF(hHandle,FRM_CFG,ENABLE,0);
       break;
     }
     case HAL_SB_DEVICE_PORTED_GENERIC:
     {
       SB_HWIO_OUTF(hHandle,PGD_CFG,ENABLE,0);
       break;
     }
     case HAL_SB_DEVICE_NON_PORTED_GENERIC_1:
     {
       SB_HWIO_OUTF(hHandle,NGD1_CFG,ENABLE,0);
       break;
     }
     case HAL_SB_DEVICE_NON_PORTED_GENERIC_2:
     {
       SB_HWIO_OUTF(hHandle,NGD2_CFG,ENABLE,0);
       break;
     }
     default:
       break;
  }
}

/**
 * Check if the device is enabled.
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 *  
 * @param[in] eSbDevice - Slimbus device to be disabled. 
 *  
 * @return TRUE, if device is enabled, FALSE otherwise.
 * 
 * @sa HAL_sb_DeviceEnable, HAL_sb_DeviceDisable. 
 * 
 * @attention
 *   None.
 */
bool32 HAL_sb_DeviceIsEnabled(HAL_sb_HandleType hHandle, 
                              HAL_sb_DeviceType eSbDevice)
{
bool32 bResult = FALSE;
  switch(eSbDevice)
  {
     case HAL_SB_DEVICE_MANAGER:
     {
       if(0 != SB_HWIO_INF(hHandle,MGR_CFG,ENABLE))
       {
         bResult = TRUE;
       }
       break;
     }
     case HAL_SB_DEVICE_INTERFACE:
     {
       if(0 != SB_HWIO_INF(hHandle,INTF_CFG,ENABLE))
       {
         bResult = TRUE;
       }
       break;
     }
     case HAL_SB_DEVICE_FRAMER:
     {
       if(0 != SB_HWIO_INF(hHandle,FRM_CFG,ENABLE))
       {
         bResult = TRUE;
       }
       break;
     }
     case HAL_SB_DEVICE_PORTED_GENERIC:
      {
       if(0 != SB_HWIO_INF(hHandle,PGD_CFG,ENABLE))
       {
         bResult = TRUE;
       }
       break;
     }
     case HAL_SB_DEVICE_NON_PORTED_GENERIC_1:
     {
       if(0 != SB_HWIO_INF(hHandle,NGD1_CFG,ENABLE))
       {
         bResult = TRUE;
       }
       break;
     }
     case HAL_SB_DEVICE_NON_PORTED_GENERIC_2:
     {
       if(0 != SB_HWIO_INF(hHandle,NGD2_CFG,ENABLE))
       {
         bResult = TRUE;
       }
       break;
     }
     default:
       break;
  }
  return bResult;
}


/**
 * Get the Error status that the device may want to report. 
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 *  
 * @param[in] eSbDevice - Slimbus device type. 
 *  
 * @return ERROR_STATUS
 * 
 * @sa HAL_sb_DeviceIsErrorSet. 
 * 
 * @attention Error status is only valid if DEV_ERR Int is set 
 *            for the respective device.
 */
uint32 HAL_sb_DeviceGetErrorStatus(HAL_sb_HandleType hHandle, 
                                   HAL_sb_DeviceType eSbDevice)
{
  uint32 uErrorStatus = 0;
  switch(eSbDevice)
  {
     case HAL_SB_DEVICE_MANAGER:
     {
       uErrorStatus = SB_HWIO_INF(hHandle,MGR_STATUS,ERROR);
       break;
     }
     case HAL_SB_DEVICE_INTERFACE:
     {
       uErrorStatus = SB_HWIO_INF(hHandle,INTF_STATUS,ERROR);
       break;
     }
     case HAL_SB_DEVICE_FRAMER:
     {
       uErrorStatus = SB_HWIO_INF(hHandle,FRM_STATUS,ERROR);
       break;
     }
     case HAL_SB_DEVICE_PORTED_GENERIC:
     {
       uErrorStatus = SB_HWIO_INF(hHandle,PGD_STATUS,ERROR);
       break;
     }
     case HAL_SB_DEVICE_NON_PORTED_GENERIC_1:
     {
       uErrorStatus = SB_HWIO_INF(hHandle,NGD1_STATUS,ERROR);
       break;
     }
     case HAL_SB_DEVICE_NON_PORTED_GENERIC_2:
     {
       uErrorStatus = SB_HWIO_INF(hHandle,NGD2_STATUS,ERROR);
       break;
     }
     default:
       break;
  }
  return uErrorStatus;
}


/**
 * Set Interrupt Mask for a device
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 *  
 * @param[in] eSbDevice - Slimbus device type. 
 *  
 * @param[in] uMask - bitmask of interrupt to enable/disable - 
 *            1=enable/0=disable see 
 *       HAL_sb_MgrIntType for Manager device.
 *       HAL_sb_FrmIntType for Framer device.
 *       HAL_sb_IntfIntType for Interface device.
 *       HAL_sb_NgdIntType for Non Ported generic devices.
 *       HAL_sb_PgdIntType for Ported Generic devices.
 *  
 * @return None
 * 
 * @sa HAL_sb_DeviceIntGetMask. 
 * 
 * @attention None.
 */
void HAL_sb_DeviceIntSetMask(HAL_sb_HandleType hHandle, 
                             HAL_sb_DeviceType eSbDevice, 
                             uint32 uMask)
{
  switch(eSbDevice)
  {
     case HAL_SB_DEVICE_MANAGER:
     {
       SB_HWIO_OUT(hHandle,MGR_INT_EN,uMask);
       break;
     }
     case HAL_SB_DEVICE_INTERFACE:
     {
       SB_HWIO_OUT(hHandle,INTF_INT_EN,uMask);
       break;
     }
     case HAL_SB_DEVICE_FRAMER:
     {
       SB_HWIO_OUT(hHandle,FRM_INT_EN,uMask);
       break;
     }
     case HAL_SB_DEVICE_PORTED_GENERIC:
     {
       SB_HWIO_OUT(hHandle,PGD_DEV_INT_EN,uMask);
       break;
     }
     case HAL_SB_DEVICE_NON_PORTED_GENERIC_1:
     {
       SB_HWIO_OUT(hHandle,NGD1_INT_EN,uMask);
       break;
     }
     case HAL_SB_DEVICE_NON_PORTED_GENERIC_2:
     {
       SB_HWIO_OUT(hHandle,NGD2_INT_EN,uMask);
       break;
     }
     default:
       break;
  }
}


/**
 * Get Interrupt Mask for a device
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 *  
 * @param[in] eSbDevice - Slimbus device type. 
 *  
 * @return bitmask of interrupts enable/disable - 
 *            1=enable/0=disable see 
 *       HAL_sb_MgrIntType for Manager device.
 *       HAL_sb_FrmIntType for Framer device.
 *       HAL_sb_IntfIntType for Interface device.
 *       HAL_sb_NgdIntType for Non Ported generic devices.
 *       HAL_sb_PgdIntType for Ported Generic devices.
 * 
 * @sa HAL_sb_DeviceIntSetMask. 
 * 
 * @attention None.
 */
uint32 HAL_sb_DeviceIntGetMask(HAL_sb_HandleType hHandle, 
                               HAL_sb_DeviceType eSbDevice)
{
  uint32 uMask = 0;
  switch(eSbDevice)
  {
     case HAL_SB_DEVICE_MANAGER:
     {
       uMask = SB_HWIO_IN(hHandle,MGR_INT_EN);
       break;
     }
     case HAL_SB_DEVICE_INTERFACE:
     {
       uMask = SB_HWIO_IN(hHandle,INTF_INT_EN);
       break;
     }
     case HAL_SB_DEVICE_FRAMER:
     {
       uMask = SB_HWIO_IN(hHandle,FRM_INT_EN);
       break;
     }
     case HAL_SB_DEVICE_PORTED_GENERIC:
     {
       uMask = SB_HWIO_IN(hHandle,PGD_DEV_INT_EN);
       break;
     }
     case HAL_SB_DEVICE_NON_PORTED_GENERIC_1:
     {
       uMask = SB_HWIO_IN(hHandle,NGD1_INT_EN);
       break;
     }
     case HAL_SB_DEVICE_NON_PORTED_GENERIC_2:
     {
       uMask = SB_HWIO_IN(hHandle,NGD2_INT_EN);
       break;
     }
     default:
       break;
  }
  return uMask;
}


/**
 * Get the status of Interrupts for a device.
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 *  
 * @param[in] eSbDevice - Slimbus device type. 
 *  
 * @return bitmask of interrupt status - see 
 *       HAL_sb_MgrIntType for Manager device.
 *       HAL_sb_FrmIntType for Framer device.
 *       HAL_sb_IntfIntType for Interface device.
 *       HAL_sb_NgdIntType for Non Ported generic devices.
 *       HAL_sb_PgdIntType for Ported Generic devices.
 * 
 * @sa HAL_sb_DeviceIntClear
 * 
 * @attention None.
 */
uint32 HAL_sb_DeviceIntGetStatus(HAL_sb_HandleType hHandle, 
                                 HAL_sb_DeviceType eSbDevice)
{
  uint32 uIntStatus = 0;
  switch(eSbDevice)
  {
     case HAL_SB_DEVICE_MANAGER:
     {
       uIntStatus = SB_HWIO_IN(hHandle,MGR_INT_STATUS);
       break;
     }
     case HAL_SB_DEVICE_INTERFACE:
     {
       uIntStatus = SB_HWIO_IN(hHandle,INTF_INT_STATUS);
       break;
     }
     case HAL_SB_DEVICE_FRAMER:
     {
       uIntStatus = SB_HWIO_IN(hHandle,FRM_INT_STATUS);
       break;
     }
     case HAL_SB_DEVICE_PORTED_GENERIC:
     {
       uIntStatus = SB_HWIO_IN(hHandle,PGD_DEV_INT_STATUS);
       break;
     }
     case HAL_SB_DEVICE_NON_PORTED_GENERIC_1:
     {
       uIntStatus = SB_HWIO_IN(hHandle,NGD1_INT_STATUS);
       break;
     }
     case HAL_SB_DEVICE_NON_PORTED_GENERIC_2:
     {
       uIntStatus = SB_HWIO_IN(hHandle,NGD2_INT_STATUS);
       break;
     }
     default:
       break;
  }
  return uIntStatus;
}


/**
 * Clear Interrupts for a device.
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 *  
 * @param[in] eSbDevice - Slimbus device type. 
 *  
 * @param[in] uMask- bitmask of interrupt to clear - see 
 *       HAL_sb_MgrIntType for Manager device.
 *       HAL_sb_FrmIntType for Framer device.
 *       HAL_sb_IntfIntType for Interface device.
 *       HAL_sb_NgdIntType for Non Ported generic devices.
 *       HAL_sb_PgdIntType for Ported Generic devices.
 *  
 * @return None.
 * 
 * @sa HAL_sb_DeviceIntGetStatus
 * 
 * @attention None.
 */
void HAL_sb_DeviceIntClear(HAL_sb_HandleType hHandle, 
                           HAL_sb_DeviceType eSbDevice, 
                           uint32 uMask)
{
  switch(eSbDevice)
  {
     case HAL_SB_DEVICE_MANAGER:
     {
       SB_HWIO_OUT(hHandle,MGR_INT_CLR,uMask);
       break;
     }
     case HAL_SB_DEVICE_INTERFACE:
     {
       SB_HWIO_OUT(hHandle,INTF_INT_CLR,uMask);
       break;
     }
     case HAL_SB_DEVICE_FRAMER:
     {
       SB_HWIO_OUT(hHandle,FRM_INT_CLR,uMask);
       break;
     }
     case HAL_SB_DEVICE_PORTED_GENERIC:
     {
       SB_HWIO_OUT(hHandle,PGD_DEV_INT_CLR,uMask);
       break;
     }
     case HAL_SB_DEVICE_NON_PORTED_GENERIC_1:
     {
       SB_HWIO_OUT(hHandle,NGD1_INT_CLR,uMask);
       break;
     }
     case HAL_SB_DEVICE_NON_PORTED_GENERIC_2:
     {
       SB_HWIO_OUT(hHandle,NGD2_INT_CLR,uMask);
       break;
     }
     default:
       break;
  }
}


/**
 * Get the Value Element Status of a device.
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 *  
 * @param[in] eSbDevice - Slimbus device type. 
 *  
 * @return VE Status.
 * 
 * @sa None.
 * 
 * @attention None.
 */
uint32 HAL_sb_DeviceGetVeStatus(HAL_sb_HandleType hHandle, 
                                HAL_sb_DeviceType eSbDevice)
{
  uint32 uValueElement = 0;
  switch(eSbDevice)
  {
     case HAL_SB_DEVICE_MANAGER:
     {
       uValueElement = SB_HWIO_INF(hHandle,MGR_VE_STATUS,VE_VAL);
       break;
     }
     case HAL_SB_DEVICE_INTERFACE:
     {
       uValueElement = SB_HWIO_INF(hHandle,INTF_VE_STATUS,VE_VAL);
       break;
     }
     case HAL_SB_DEVICE_FRAMER:
     {
       uValueElement = SB_HWIO_INF(hHandle,FRM_VE_STATUS,VE_VAL);
       break;
     }
     case HAL_SB_DEVICE_PORTED_GENERIC:
     {
       uValueElement = SB_HWIO_INF(hHandle,PGD_VE_STATUS,VE_VAL);
       break;
     }
     case HAL_SB_DEVICE_NON_PORTED_GENERIC_1:
     {
       uValueElement = SB_HWIO_INF(hHandle,NGD1_VE_STATUS,VE_VAL);
       break;
     }
     case HAL_SB_DEVICE_NON_PORTED_GENERIC_2:
     {
       uValueElement = SB_HWIO_INF(hHandle,NGD2_VE_STATUS,VE_VAL);
       break;
     }
     default:
       break;
  }
  return uValueElement;
}


/**
 * Get the Information Element Status of the device.
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 *  
 * @param[in] eSbDevice - Slimbus device type. 
 *
 * @return IE Status  See
 *                 HAL_sb_MgrIeType  for Manager device
 *                 HAL_sb_IntfIeType  for Interface device
 *                 HAL_sb_FrmType  for Framer device
 *                 HAL_sb_GdIeType  for Generic device (Ported and Non Ported)
 * 
 * @sa None.
 * 
 * @attention Caller needs to typecast the return value to one of struct 
 *            listed above
 */
uint32 HAL_sb_DeviceGetIeStatus(HAL_sb_HandleType hHandle, 
                                HAL_sb_DeviceType eSbDevice)
{
  uint32 uInfoElement = 0;
  switch(eSbDevice)
  {
     case HAL_SB_DEVICE_MANAGER:
     {
       uInfoElement = SB_HWIO_IN(hHandle,MGR_IE_STATUS);  
       break;
     }
     case HAL_SB_DEVICE_INTERFACE:
     {
       uInfoElement = SB_HWIO_IN(hHandle,INTF_IE_STATUS);
       break;
     }
     case HAL_SB_DEVICE_FRAMER:
     {
       uInfoElement = SB_HWIO_IN(hHandle,FRM_IE_STATUS);
       break;
     }
     case HAL_SB_DEVICE_PORTED_GENERIC:
     {
       uInfoElement = SB_HWIO_IN(hHandle,PGD_IE_STATUS);
       break;
     }
     case HAL_SB_DEVICE_NON_PORTED_GENERIC_1:
     {
       uInfoElement = SB_HWIO_IN(hHandle,NGD1_IE_STATUS);
       break;
     }
     case HAL_SB_DEVICE_NON_PORTED_GENERIC_2:
     {
       uInfoElement = SB_HWIO_IN(hHandle,NGD2_IE_STATUS);
       break;
     }
     default:
       break;
  }
  return uInfoElement;
}


/**
 * Check if the Device's Tx Msg Buffer is Busy.
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 *  
 * @param[in] eSbDevice - Slimbus device type. 
 *  
 * @return TRUE, if Buffer is busy, FALSE otherwise
 * 
 * @sa None.
 * 
 * @attention Only valid for devices having direct access 
 *            buffers. Should be used only if the device is
 *            enumerated. Should not be used as a check for tx
 *            msg sent.
 */
bool32 HAL_sb_DeviceIsTxMsgBufferBusy(HAL_sb_HandleType hHandle, 
                                      HAL_sb_DeviceType eSbDevice)
{
  bool32 bIsBusy = FALSE;
  switch(eSbDevice)
  {
     case HAL_SB_DEVICE_MANAGER:
     {
       if(0 != SB_HWIO_INF(hHandle,MGR_STATUS,TX_MSG_BUFFER_BUSY))
       {
         bIsBusy = TRUE;
       }
       break;
     }
     case HAL_SB_DEVICE_NON_PORTED_GENERIC_1:
     {
       if(0 != SB_HWIO_INF(hHandle,NGD1_STATUS,TX_MSG_BUFFER_BUSY))
       {
         bIsBusy = TRUE;
       }
       break;
     }
     case HAL_SB_DEVICE_NON_PORTED_GENERIC_2:
     {
       if(0 != SB_HWIO_INF(hHandle,NGD2_STATUS,TX_MSG_BUFFER_BUSY))
       {
         bIsBusy = TRUE;
       }
       break;
     }
     default:
       break;
  }
  return bIsBusy;
}


/**
 * Check if the Device has been Enumerated.
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 *  
 * @param[in] eSbDevice - Slimbus device type. 
 *  
 * @return TRUE, if the device has been enumerated, FALSE 
 *         otherwise
 * 
 * @sa None.
 * 
 * @attention None.
 */
bool32 HAL_sb_DeviceIsEnumerated(HAL_sb_HandleType hHandle, 
                                 HAL_sb_DeviceType eSbDevice)
{
  bool32 bIsEnumerated = FALSE;
  switch(eSbDevice)
  {
     case HAL_SB_DEVICE_MANAGER:
     {
       if(0 != SB_HWIO_INF(hHandle,MGR_STATUS,ENUMERATED))
       {
         bIsEnumerated = TRUE;
       }
       break;
     }
     case HAL_SB_DEVICE_FRAMER:
     {
       if(0 != SB_HWIO_INF(hHandle,FRM_STATUS,ENUMERATED))
       {
         bIsEnumerated = TRUE;
       }
       break;
     }
     case HAL_SB_DEVICE_INTERFACE:
     {
       if(0 != SB_HWIO_INF(hHandle,INTF_STATUS,ENUMERATED))
       {
         bIsEnumerated = TRUE;
       }
       break;
     }
     case HAL_SB_DEVICE_NON_PORTED_GENERIC_1:
     {
       if(0 != SB_HWIO_INF(hHandle,NGD1_STATUS,ENUMERATED))
       {
         bIsEnumerated = TRUE;
       }
       break;
     }
     case HAL_SB_DEVICE_NON_PORTED_GENERIC_2:
     {
       if(0 != SB_HWIO_INF(hHandle,NGD2_STATUS,ENUMERATED))
       {
         bIsEnumerated = TRUE;
       }
       break;
     }
    case HAL_SB_DEVICE_PORTED_GENERIC:
    {
      if ( (SB_HWIO_IN(hHandle,COMP_HW_VERSION) < 0x01030000) ||
           (0 != SB_HWIO_INF(hHandle,PGD_STATUS,ENUMERATED)) )
      {
        bIsEnumerated = TRUE;
      }
      break;
    }
     default:
       break;
  }
  return bIsEnumerated;
}


/**
 * Get the Pipe num offset of a Message Queue
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 *  
 * @param[in] eSbDevice - Slimbus device type. 
 *
 * @param[in] eMsgQ - Device Message Queue Type.
 *  
 * @return PIPE_NUM_OFFSET
 * 
 * @sa None.
 * 
 * @attention Only valid for devices having MsgQs. 
 */
uint32 HAL_sb_DeviceMsgQGetPipeOffset(HAL_sb_HandleType hHandle, 
                                      HAL_sb_DeviceType eSbDevice,
                                      HAL_sb_DeviceMsgQType eMsgQ)
{
  uint32 uPipeOffset = 0;
  if(HAL_sb_DeviceIsMsgQsSupported(hHandle, eSbDevice))
  {
    switch(eSbDevice)
    {
      case HAL_SB_DEVICE_MANAGER:
      {
        if(HAL_SB_DEVICE_MSG_QUEUE_RX == eMsgQ)
        {
          uPipeOffset = SB_HWIO_INF(hHandle,MGR_STATUS,PIPE_NUM_OFFSET);
        }
        else if(HAL_SB_DEVICE_MSG_QUEUE_TX_PRIORITY_LOW == eMsgQ)
        {
          uPipeOffset = SB_HWIO_INF(hHandle,MGR_STATUS,PIPE_NUM_OFFSET)+ 1;
        }
        else if(HAL_SB_DEVICE_MSG_QUEUE_TX_PRIORITY_HIGH== eMsgQ)
        {
          uPipeOffset = SB_HWIO_INF(hHandle,MGR_STATUS,PIPE_NUM_OFFSET) + 2;
        }
        break;
      }

      case HAL_SB_DEVICE_NON_PORTED_GENERIC_1:
      {
        if(HAL_SB_DEVICE_MSG_QUEUE_RX == eMsgQ)
        {
          uPipeOffset = SB_HWIO_INF(hHandle,NGD1_STATUS,PIPE_NUM_OFFSET);
        }
        else if(HAL_SB_DEVICE_MSG_QUEUE_TX == eMsgQ)
        {
          uPipeOffset = SB_HWIO_INF(hHandle,NGD1_STATUS,PIPE_NUM_OFFSET)+ 1;
        }
        break;
      }

      case HAL_SB_DEVICE_NON_PORTED_GENERIC_2:
      {
        if(HAL_SB_DEVICE_MSG_QUEUE_RX == eMsgQ)
        {
          uPipeOffset = SB_HWIO_INF(hHandle,NGD2_STATUS,PIPE_NUM_OFFSET);
        }
        else if(HAL_SB_DEVICE_MSG_QUEUE_TX == eMsgQ)
        {
          uPipeOffset = SB_HWIO_INF(hHandle,NGD2_STATUS,PIPE_NUM_OFFSET)+ 1;
        }
        break;
      }
      
      default: 
        break;
    }
  }
  return uPipeOffset;
}


/**
 * Get the ACKED TX Message MC
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 *  
 * @param[in] eSbDevice - Slimbus device type. 
 *  
 * @return ACKED_TX_MSG_MC
 * 
 * @sa HAL_sb_DeviceTxMsgGetNackedMc.
 * 
 * @attention None.
 */
uint32 HAL_sb_DeviceTxMsgGetAckedMc(HAL_sb_HandleType hHandle, 
                                    HAL_sb_DeviceType eSbDevice)
{
  uint32 uTxMsgAckedMc = 0;

  switch(eSbDevice)
  {
    case HAL_SB_DEVICE_MANAGER:
    {
      uTxMsgAckedMc = SB_HWIO_INF(hHandle,MGR_STATUS,ACKED_TX_MSG_MC);
      break;
    }
    case HAL_SB_DEVICE_NON_PORTED_GENERIC_1:
    {
      uTxMsgAckedMc = SB_HWIO_INF(hHandle,NGD1_STATUS,ACKED_TX_MSG_MC);
      break;
    }
    case HAL_SB_DEVICE_NON_PORTED_GENERIC_2:
    {
      uTxMsgAckedMc = SB_HWIO_INF(hHandle,NGD2_STATUS,ACKED_TX_MSG_MC);
      break;
    }
    default:
      break;
  }
  return uTxMsgAckedMc;
}


/**
 * Get the NACKED TX Message MC
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 *  
 * @param[in] eSbDevice - Slimbus device type. 
 *  
 * @return NACKED_TX_MSG_MC
 * 
 * @sa HAL_sb_DeviceTxMsgGetAckedMc.
 * 
 * @attention None.
 */
uint32 HAL_sb_DeviceTxMsgGetNackedMc(HAL_sb_HandleType hHandle, 
                                     HAL_sb_DeviceType eSbDevice)
{
  uint32 uTxMsgNackedMc = 0;

  switch(eSbDevice)
  {
    case HAL_SB_DEVICE_MANAGER:
    {
      uTxMsgNackedMc = SB_HWIO_INF(hHandle,MGR_STATUS,NACKED_TX_MSG_MC);
      break;
    }
    case HAL_SB_DEVICE_NON_PORTED_GENERIC_1:
    {
      uTxMsgNackedMc = SB_HWIO_INF(hHandle,NGD1_STATUS,NACKED_TX_MSG_MC);
      break;
    }
    case HAL_SB_DEVICE_NON_PORTED_GENERIC_2:
    {
      uTxMsgNackedMc = SB_HWIO_INF(hHandle,NGD2_STATUS,NACKED_TX_MSG_MC);
      break;
    }
    default:
      break;
  }
  return uTxMsgNackedMc;
}


/**
 * Set the Rx Message Queue Block Size. After receiving that 
 * many messages, a block_end is asserted 
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 *  
 * @param[in] eSbDevice - Slimbus device type. 
 *  
 * @param[in] uBlockSize - Block size specified in units of 
 *       messages.
 *  
 * @return None. 
 *  
 * @sa HAL_sb_DeviceRxMsgQGetBlockSize.
 * 
 * @attention Only valid for devices having MsgQs.
 */
void HAL_sb_DeviceRxMsgQSetBlockSize(HAL_sb_HandleType hHandle, 
                                     HAL_sb_DeviceType eSbDevice, 
                                     uint32 uBlockSize)
{
  if(HAL_sb_DeviceIsMsgQsSupported(hHandle,eSbDevice))
  {
    switch(eSbDevice)
    {
       case HAL_SB_DEVICE_MANAGER:
       {
         SB_HWIO_OUTF(hHandle,MGR_RX_MSGQ_CFG,BLOCK_SIZE,uBlockSize);
         break;
       }
       case HAL_SB_DEVICE_NON_PORTED_GENERIC_1:
       {
         SB_HWIO_OUTF(hHandle,NGD1_RX_MSGQ_CFG,BLOCK_SIZE, uBlockSize);
         break;
       }
       case HAL_SB_DEVICE_NON_PORTED_GENERIC_2:
       {
         SB_HWIO_OUTF(hHandle,NGD2_RX_MSGQ_CFG,BLOCK_SIZE, uBlockSize);
         break;
       }
       default:
         break;
    }
  }
}


/**
 * Get the Rx Message Queue Block Size. After receiving that 
 * many messages, a block_end is asserted 
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 *  
 * @param[in] eSbDevice - Slimbus device type. 
 *  
 * @return Block size specified in units of messages
 * 
 * @sa HAL_sb_DeviceRxMsgQSetBlockSize.
 * 
 * @attention Only valid for devices having MsgQs.
 */
uint32 HAL_sb_DeviceRxMsgQGetBlockSize(HAL_sb_HandleType hHandle, 
                                       HAL_sb_DeviceType eSbDevice)
{
  uint32 uBlockSize = 0;
  if(HAL_sb_DeviceIsMsgQsSupported(hHandle,eSbDevice))
  {
    switch(eSbDevice)
    {
       case HAL_SB_DEVICE_MANAGER:
       {
         uBlockSize = SB_HWIO_INF(hHandle,MGR_RX_MSGQ_CFG,BLOCK_SIZE);
         break;
       }
       case HAL_SB_DEVICE_NON_PORTED_GENERIC_1:
       {
         uBlockSize = SB_HWIO_INF(hHandle,NGD1_RX_MSGQ_CFG,BLOCK_SIZE);
         break;
       }
       case HAL_SB_DEVICE_NON_PORTED_GENERIC_2:
       {
         uBlockSize = SB_HWIO_INF(hHandle,NGD2_RX_MSGQ_CFG,BLOCK_SIZE);
         break;
       }
       default:
         break;
    }
  }
  return uBlockSize;
}


/**
 * Set the Rx Message Queue Transaction Size. After receiving 
 * that many messages, a trans_end is asserted 
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 *  
 * @param[in] eSbDevice - Slimbus device type. 
 *  
 * @param[in] uTransSize - Transaction size specified in units 
 *       of messages.
 * 
 * @return None.
 *  
 * @sa HAL_sb_DeviceRxMsgQGetTransSize.
 * 
 * @attention Only valid for devices having MsgQs.
 */
void  HAL_sb_DeviceRxMsgQSetTransSize(HAL_sb_HandleType hHandle, 
                                      HAL_sb_DeviceType eSbDevice, 
                                      uint32 uTransSize)
{
  if(HAL_sb_DeviceIsMsgQsSupported(hHandle,eSbDevice))
  {
    switch(eSbDevice)
    {
       case HAL_SB_DEVICE_MANAGER:
       {
         SB_HWIO_OUTF(hHandle,MGR_RX_MSGQ_CFG,TRANS_SIZE,uTransSize);
         break;
       }
       case HAL_SB_DEVICE_NON_PORTED_GENERIC_1:
       {
         SB_HWIO_OUTF(hHandle,NGD1_RX_MSGQ_CFG,TRANS_SIZE, uTransSize);
         break;
       }
       case HAL_SB_DEVICE_NON_PORTED_GENERIC_2:
       {
         SB_HWIO_OUTF(hHandle,NGD2_RX_MSGQ_CFG,TRANS_SIZE, uTransSize);
         break;
       }
       default:
         break;
    }
  }
}


/**
 * Get the Rx Message Queue Transaction Size. After receiving 
 * that many messages, a trans_end is asserted 
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 *  
 * @param[in] eSbDevice - Slimbus device type. 
 *  
 * @return Transaction size specified in units of messages 
 *  
 * @sa HAL_sb_DeviceRxMsgQSetTransSize.
 * 
 * @attention Only valid for devices having MsgQs.
 */
uint32 HAL_sb_DeviceRxMsgQGetTransSize(HAL_sb_HandleType hHandle, 
                                       HAL_sb_DeviceType eSbDevice)
{
  uint32 uTransSize = 0;
  if(HAL_sb_DeviceIsMsgQsSupported(hHandle,eSbDevice))
  {
    switch(eSbDevice)
    {
       case HAL_SB_DEVICE_MANAGER:
       {
         uTransSize = SB_HWIO_INF(hHandle,MGR_RX_MSGQ_CFG,TRANS_SIZE);
         break;
       }
       case HAL_SB_DEVICE_NON_PORTED_GENERIC_1:
       {
         uTransSize = SB_HWIO_INF(hHandle,NGD1_RX_MSGQ_CFG,TRANS_SIZE);
         break;
       }
       case HAL_SB_DEVICE_NON_PORTED_GENERIC_2:
       {
         uTransSize = SB_HWIO_INF(hHandle,NGD2_RX_MSGQ_CFG,TRANS_SIZE);
         break;
       }
       default:
         break;
    }
  }
  return uTransSize;
}


/**
 * Set the Time out value for Rx Message Queue 
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 *  
 * @param[in] eSbDevice - Slimbus device type. 
 *  
 * @param[in] uTimeoutValInAhbClkCycles - Timeout value in units
 *       of AHB cycles
 *  
 * @return None. 
 *  
 * @sa HAL_sb_DeviceRxMsgQGetTimeOutVal.
 * 
 * @attention Time out counter is enabled if this is set to a 
 *            non zero value and when the first message is
 *            received. Time out cntr reset after every Rx msg
 *            is received. If next Rx message not received
 *            before it expires, a trans_end is automatically
 *            generated even if BLOCK_SIZE/TRANS_SIZE is not
 *            got. Specified in terms of AHB Bus clock cycles
 *            given to the core by the ClockCtrl of SoC. Only
 *            valid for devices having MsgQs.
 */
void HAL_sb_DeviceRxMsgQSetTimeOutVal(HAL_sb_HandleType hHandle, 
                                      HAL_sb_DeviceType eSbDevice, 
                                      uint32 uTimeoutValInAhbClkCycles)
{
  if(HAL_sb_DeviceIsMsgQsSupported(hHandle,eSbDevice))
  {
    switch(eSbDevice)
    {
       case HAL_SB_DEVICE_MANAGER:
       {
         SB_HWIO_OUTF(hHandle,
                      MGR_RX_MSGQ_CFG,
                      TIME_OUT_VAL,
                      uTimeoutValInAhbClkCycles);
         break;
       }
       case HAL_SB_DEVICE_NON_PORTED_GENERIC_1:
       {
         SB_HWIO_OUTF(hHandle,
                      NGD1_RX_MSGQ_CFG,
                      TIME_OUT_VAL,
                      uTimeoutValInAhbClkCycles);
         break;
       }
       case HAL_SB_DEVICE_NON_PORTED_GENERIC_2:
       {
         SB_HWIO_OUTF(hHandle,
                      NGD2_RX_MSGQ_CFG,
                      TIME_OUT_VAL, 
                      uTimeoutValInAhbClkCycles);
         break;
       }
       default:
         break;
    }
  }
}


/**
 * Get the Time out value for Rx Message Queue 
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 *  
 * @param[in] eSbDevice - Slimbus device type. 
 *  
 * @return Timeout value in units of AHB cycles 
 *  
 * @sa HAL_sb_DeviceRxMsgQSetTimeOutVal.
 * 
 * @attention Time out counter is enabled if this is set to a 
 *            non zero value and when the first message is
 *            received. Time out cntr reset after every Rx msg
 *            is received. If next Rx message not received
 *            before it expires, a trans_end is automatically
 *            generated even if BLOCK_SIZE/TRANS_SIZE is not
 *            got. Specified in terms of AHB Bus clock cycles
 *            given to the core by the ClockCtrl of SoC. Only
 *            valid for devices having MsgQs.
 */
uint32 HAL_sb_DeviceRxMsgQGetTimeOutVal(HAL_sb_HandleType hHandle, 
                                        HAL_sb_DeviceType eSbDevice)
{
  uint32 uTimeoutVal = 0;
  if(HAL_sb_DeviceIsMsgQsSupported(hHandle,eSbDevice))
  {
    switch(eSbDevice)
    {
       case HAL_SB_DEVICE_MANAGER:
       {
         uTimeoutVal = SB_HWIO_INF(hHandle,MGR_RX_MSGQ_CFG,TIME_OUT_VAL);
         break;
       }
       case HAL_SB_DEVICE_NON_PORTED_GENERIC_1:
       {
         uTimeoutVal = SB_HWIO_INF(hHandle,NGD1_RX_MSGQ_CFG,TIME_OUT_VAL);
         break;
       }
       case HAL_SB_DEVICE_NON_PORTED_GENERIC_2:
       {
         uTimeoutVal = SB_HWIO_INF(hHandle,NGD2_RX_MSGQ_CFG,TIME_OUT_VAL);
         break;
       }
       default:
         break;
    }
  }
  return uTimeoutVal;
}


/**
 * Write Tx message to be sent over Slimbus. This is direct 
 * access messaging API, bypassing the MsgQs. 
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 *  
 * @param[in] eSbDevice - Slimbus device type. 
 *  
 * @param[in] pBuffer - Pointer to the Write Buffer. 
 *  
 * @param[in] uBufferSize - Buffer size in words (uint32).
 *  
 * @return None. 
 *  
 * @sa HAL_sb_DeviceRxMsgRead.
 * 
 * @attention This Direct access Messaging API SHOULD NOT be 
 * used if any of the Tx queues are enabled. The resulting 
 * behavior is not defined. Only valid for devices having 
 * direct access buffers. 
 */
void HAL_sb_DeviceTxMsgWrite(HAL_sb_HandleType hHandle, 
                             HAL_sb_DeviceType eSbDevice, 
                             uint32* pBuffer, 
                             uint32 uBufferSize)
{

  uint32 uTemp;

  if(HAL_sb_DeviceIsMsgQsSupported(hHandle,eSbDevice))
  {
    switch(eSbDevice)
    {
      case HAL_SB_DEVICE_MANAGER:
      {
        if(SB_MAXn(MGR_TX_MSGn) + 1 >= uBufferSize)
        {
          for(uTemp=0; uTemp < uBufferSize; uTemp++)
          {
            SB_HWIO_OUTI(hHandle,MGR_TX_MSGn,uTemp,pBuffer[uTemp]);
          }
        }
        break;
      }
      case HAL_SB_DEVICE_NON_PORTED_GENERIC_1:
      {
        if(SB_MAXn(NGD1_TX_MSGn) + 1 >= uBufferSize)
        {
          for(uTemp=0; uTemp < uBufferSize; uTemp++)
          {
            SB_HWIO_OUTI(hHandle,NGD1_TX_MSGn,uTemp,pBuffer[uTemp]);
          }
        } 
        break;
      }
      case HAL_SB_DEVICE_NON_PORTED_GENERIC_2:
      {
        if(SB_MAXn(NGD2_TX_MSGn) + 1 >= uBufferSize)
        {
          for(uTemp=0; uTemp < uBufferSize; uTemp++)
          {
            SB_HWIO_OUTI(hHandle,NGD2_TX_MSGn,uTemp,pBuffer[uTemp]);
          }
        }
        break;
      }
      default:
        break;
    }
  }
}


/**
 * Read Rx message from over Slimbus.This is direct 
 * access messaging API, bypassing the MsgQs. 
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 *  
 * @param[in] eSbDevice - Slimbus device type. 
 *  
 * @param[in] pBuffer - Pointer to the Read Buffer. 
 *  
 * @param[in] uBufferSize - Buffer size in words (uint32).
 *  
 * @return None. 
 *  
 * @sa HAL_sb_DeviceTxMsgWrite.
 * 
 * @attention This Direct access Messaging API SHOULD NOT be 
 * used if any of the Rx queues are enabled. The resulting 
 * behavior is not defined. Only valid for devices having direct
 * access buffers. 
 */
void HAL_sb_DeviceRxMsgRead(HAL_sb_HandleType hHandle, 
                            HAL_sb_DeviceType eSbDevice, 
                            uint32* pBuffer, 
                            uint32 uBufferSize)
{

  uint32 uTemp;

  if(HAL_sb_DeviceIsMsgQsSupported(hHandle,eSbDevice))
  {
    switch(eSbDevice)
    {
      case HAL_SB_DEVICE_MANAGER:
      {
        if(SB_MAXn(MGR_RX_MSGn) + 1 >= uBufferSize)
        {
          for(uTemp=0; uTemp < uBufferSize; uTemp++)
          {
            pBuffer[uTemp] = SB_HWIO_INFI(hHandle,MGR_RX_MSGn,uTemp,MSGVAL);
          }
        }
        break;
      }
      case HAL_SB_DEVICE_NON_PORTED_GENERIC_1:
      {
        if(SB_MAXn(NGD1_RX_MSGn) + 1 >= uBufferSize)
        {
          for(uTemp=0; uTemp < uBufferSize; uTemp++)
          {
            pBuffer[uTemp] = SB_HWIO_INFI(hHandle,NGD1_RX_MSGn,uTemp,MSGVAL);
          }
        }
        break;
      }
      case HAL_SB_DEVICE_NON_PORTED_GENERIC_2:
      {
        if(SB_MAXn(NGD2_RX_MSGn) + 1 >= uBufferSize)
        {
          for(uTemp=0; uTemp < uBufferSize; uTemp++)
          {
            pBuffer[uTemp] = SB_HWIO_INFI(hHandle,NGD2_RX_MSGn,uTemp,MSGVAL);
          }
        }
        break;
      }
      default:
        break;
    }
  }
}


/**
 * Check if Message Queue is enabled on this device
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 *  
 * @param[in] eSbDevice - Slimbus device type. 
 *
 * @param[in] eMsgQ - Message Queue type. 
 *  
 * @return TRUE, if message queue is enabled, FALSE, otherwise. 
 *  
 * @sa HAL_sb_DeviceMsgQEnable, HAL_sb_DeviceMsgQDisable.
 * 
 * @attention If the respective message queue is not supported 
 *            on the device, this function will always return
 *            FALSE.
 */
bool32 HAL_sb_DeviceMsgQIsEnabled(HAL_sb_HandleType hHandle, 
                                  HAL_sb_DeviceType eSbDevice,
                                  HAL_sb_DeviceMsgQType eMsgQ)
{
  bool32 bIsMsgQEn = FALSE;
  if(HAL_sb_DeviceIsMsgQsSupported(hHandle, eSbDevice))
  {
    switch(eSbDevice)
    {
      case HAL_SB_DEVICE_MANAGER:
      {
        if(HAL_SB_DEVICE_MSG_QUEUE_RX == eMsgQ)
        {
          if(SB_HWIO_INF(hHandle,MGR_CFG,RX_MSGQ_EN))
          { 
            bIsMsgQEn = TRUE;
          }
        }
        else if(HAL_SB_DEVICE_MSG_QUEUE_TX_PRIORITY_LOW == eMsgQ)
        {
          if(SB_HWIO_INF(hHandle,MGR_CFG,TX_MSGQ_EN_LOW))
          { 
            bIsMsgQEn = TRUE;
          }
        }
        else if(HAL_SB_DEVICE_MSG_QUEUE_TX_PRIORITY_HIGH== eMsgQ)
        {
          if(SB_HWIO_INF(hHandle,MGR_CFG,TX_MSGQ_EN_HIGH))
          { 
            bIsMsgQEn = TRUE;
          }
        }
        break;
      }

      case HAL_SB_DEVICE_NON_PORTED_GENERIC_1:
      {
        if(HAL_SB_DEVICE_MSG_QUEUE_RX == eMsgQ)
        {
          if(SB_HWIO_INF(hHandle,NGD1_CFG,RX_MSGQ_EN))
          { 
            bIsMsgQEn = TRUE;
          }
        }
        else if(HAL_SB_DEVICE_MSG_QUEUE_TX == eMsgQ)
        {
          if(SB_HWIO_INF(hHandle,NGD1_CFG,TX_MSGQ_EN))
          { 
            bIsMsgQEn = TRUE;
          }
        }
        break;
      }

      case HAL_SB_DEVICE_NON_PORTED_GENERIC_2:
      {
        if(HAL_SB_DEVICE_MSG_QUEUE_RX == eMsgQ)
        {
          if(SB_HWIO_INF(hHandle,NGD2_CFG,RX_MSGQ_EN))
          { 
            bIsMsgQEn = TRUE;
          }
        }
        else if(HAL_SB_DEVICE_MSG_QUEUE_TX == eMsgQ)
        {
          if(SB_HWIO_INF(hHandle,NGD2_CFG,TX_MSGQ_EN))
          { 
            bIsMsgQEn = TRUE;
          }
        }
        break;
      }
      
      default: 
        break;
    }
  }
  return bIsMsgQEn;
}


/**
 * Enable the message Queue on this device, if supported
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 *  
 * @param[in] eSbDevice - Slimbus device type. 
 *
 * @param[in] eMsgQ - Message Queue type. 
 *  
 * @return TRUE, if supported and enabled, FALSE, if not 
 *         supported on this device.
 *  
 * @sa HAL_sb_DeviceMsgQIsEnabled, HAL_sb_DeviceMsgQDisable.
 * 
 * @attention If the respective message queue is not supported 
 *            on the device, this function will always return
 *            FALSE.
 */
bool32 HAL_sb_DeviceMsgQEnable(HAL_sb_HandleType hHandle, 
                               HAL_sb_DeviceType eSbDevice,
                               HAL_sb_DeviceMsgQType eMsgQ)
{
  bool32 uMsgQIsSupported = FALSE;
  if(HAL_sb_DeviceIsMsgQsSupported(hHandle, eSbDevice))
  {
    uMsgQIsSupported = TRUE;
    switch(eSbDevice)
    {
      case HAL_SB_DEVICE_MANAGER:
      {
        if(HAL_SB_DEVICE_MSG_QUEUE_RX == eMsgQ)
        {
          SB_HWIO_OUTF(hHandle,MGR_CFG,RX_MSGQ_EN,1);
        }
        else if(HAL_SB_DEVICE_MSG_QUEUE_TX_PRIORITY_LOW == eMsgQ)
        {
          SB_HWIO_OUTF(hHandle,MGR_CFG,TX_MSGQ_EN_LOW,1);
        }
        else if(HAL_SB_DEVICE_MSG_QUEUE_TX_PRIORITY_HIGH== eMsgQ)
        {
          SB_HWIO_OUTF(hHandle,MGR_CFG,TX_MSGQ_EN_HIGH,1);
        }
        break;
      }

      case HAL_SB_DEVICE_NON_PORTED_GENERIC_1:
      {
        if(HAL_SB_DEVICE_MSG_QUEUE_RX == eMsgQ)
        {
          SB_HWIO_OUTF(hHandle,NGD1_CFG,RX_MSGQ_EN,1);
        }
        else if(HAL_SB_DEVICE_MSG_QUEUE_TX == eMsgQ)
        {
          SB_HWIO_OUTF(hHandle,NGD1_CFG,TX_MSGQ_EN,1);
        }
        break;
      }

      case HAL_SB_DEVICE_NON_PORTED_GENERIC_2:
      {
        if(HAL_SB_DEVICE_MSG_QUEUE_RX == eMsgQ)
        {
          SB_HWIO_OUTF(hHandle,NGD2_CFG,RX_MSGQ_EN,1);
        }
        else if(HAL_SB_DEVICE_MSG_QUEUE_TX == eMsgQ)
        {
          SB_HWIO_OUTF(hHandle,NGD2_CFG,TX_MSGQ_EN,1);
        }
        break;
      }
      
      default: 
        break;
    }
  }
  return uMsgQIsSupported;
}

/**
 * Disable the message Queue on this device, if supported
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 *  
 * @param[in] eSbDevice - Slimbus device type. 
 *
 * @param[in] eMsgQ - Message Queue type. 
 *  
 * @return None.
 *  
 * @sa HAL_sb_DeviceMsgQIsEnabled, HAL_sb_DeviceMsgQEnable.
 * 
 * @attention None.
 */
void HAL_sb_DeviceMsgQDisable(HAL_sb_HandleType hHandle, 
                              HAL_sb_DeviceType eSbDevice, 
                              HAL_sb_DeviceMsgQType eMsgQ)
{
  if(HAL_sb_DeviceIsMsgQsSupported(hHandle, eSbDevice))
  {
    switch(eSbDevice)
    {
      case HAL_SB_DEVICE_MANAGER:
      {
        if(HAL_SB_DEVICE_MSG_QUEUE_RX == eMsgQ)
        {
          SB_HWIO_OUTF(hHandle,MGR_CFG,RX_MSGQ_EN,0);
        }
        else if(HAL_SB_DEVICE_MSG_QUEUE_TX_PRIORITY_LOW == eMsgQ)
        {
          SB_HWIO_OUTF(hHandle,MGR_CFG,TX_MSGQ_EN_LOW,0);
        }
        else if(HAL_SB_DEVICE_MSG_QUEUE_TX_PRIORITY_HIGH== eMsgQ)
        {
          SB_HWIO_OUTF(hHandle,MGR_CFG,TX_MSGQ_EN_HIGH,0);
        }
        break;
      }

      case HAL_SB_DEVICE_NON_PORTED_GENERIC_1:
      {
        if(HAL_SB_DEVICE_MSG_QUEUE_RX == eMsgQ)
        {
          SB_HWIO_OUTF(hHandle,NGD1_CFG,RX_MSGQ_EN,0);
        }
        else if(HAL_SB_DEVICE_MSG_QUEUE_TX == eMsgQ)
        {
          SB_HWIO_OUTF(hHandle,NGD1_CFG,TX_MSGQ_EN,0);
        }
        break;
      }

      case HAL_SB_DEVICE_NON_PORTED_GENERIC_2:
      {
        if(HAL_SB_DEVICE_MSG_QUEUE_RX == eMsgQ)
        {
          SB_HWIO_OUTF(hHandle,NGD2_CFG,RX_MSGQ_EN,0);
        }
        else if(HAL_SB_DEVICE_MSG_QUEUE_TX == eMsgQ)
        {
          SB_HWIO_OUTF(hHandle,NGD2_CFG,TX_MSGQ_EN,0);
        }
        break;
      }
      default: 
        break;
    }
  }
}


/**
 * Check if the Tx and Rx Msg Queues are supported on this 
 * device. 
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 * 
 * @param[in] eSbDevice - Slimbus device type. 
 *  
 * @return TRUE, if supported, FALSE, otherwise.
 * 
 * @sa None.
 * 
 * @attention None.
 */
bool32 HAL_sb_DeviceIsMsgQsSupported(HAL_sb_HandleType hHandle, 
                                     HAL_sb_DeviceType eSbDevice)
{
  bool32 bIsMsgQSupported = FALSE;

  switch(eSbDevice)
  {
     case HAL_SB_DEVICE_MANAGER:
     {
       bIsMsgQSupported = TRUE;
       break;
     }
     case HAL_SB_DEVICE_NON_PORTED_GENERIC_1:
     {
       if(0 != SB_HWIO_INF(hHandle,NGD1_STATUS,MSGQ_SUPPORT))
       {
         bIsMsgQSupported = TRUE;
       }
       break;
     }
     case HAL_SB_DEVICE_NON_PORTED_GENERIC_2:
     {
       if(0 != SB_HWIO_INF(hHandle,NGD2_STATUS,MSGQ_SUPPORT))
       {
         bIsMsgQSupported = TRUE;
       }
       break;
     }
     default:
       break;
  }
  return bIsMsgQSupported;
}

/** @} */
/** @name Interface Device APIs */
/** @{ */


/**
 * Get the Interface Assigned Logical Address. 
 * 
 * @param[in] hHandle - This SlimBus hHandle. 
 *  
 * @return Interface Assigned Logical Address.
 *  
 * @attention None.
 */
uint32 HAL_sb_IntfGetAssignedLA(HAL_sb_HandleType hHandle)
{
  return SB_HWIO_INF(hHandle,INTF_STATUS,DEV_LA);
}

/**
 * Get the Interface Frame Sync Status. 
 * 
 * @param[in] hHandle - This SlimBus hHandle. 
 *  
 * @return TRUE if frame sync is acquired, FALSE otherwise 
 *  
 * @attention None.
 */
bool32 HAL_sb_IntfIsFrameSyncAcquired(HAL_sb_HandleType hHandle)
{
  return SB_HWIO_INF(hHandle,INTF_STATUS,FRAME_SYNC_ACQUIRED);
}

/**
 * Get the Interface Superframe Sync Status. 
 * 
 * @param[in] hHandle - This SlimBus hHandle. 
 *  
 * @return TRUE if superframe sync is acquired, FALSE otherwise 
 *  
 * @attention None.
 */
bool32 HAL_sb_IntfIsSuperframeSyncAcquired(HAL_sb_HandleType hHandle)
{
  return SB_HWIO_INF(hHandle,INTF_STATUS,SUPERFRAME_SYNC_ACQUIRED);
}

/**
 * Get the Interface Message Sync Status. 
 * 
 * @param[in] hHandle - This SlimBus hHandle. 
 *  
 * @return TRUE if message sync is acquired, FALSE otherwise 
 *  
 * @attention None.
 */
bool32 HAL_sb_IntfIsMessageSyncAcquired(HAL_sb_HandleType hHandle)
{
  return SB_HWIO_INF(hHandle,INTF_STATUS,MESSAGE_SYNC_ACQUIRED);
}

/** @} */
/** @name Framer Device APIs */
/** @{ */


/**
 * Enables devices within the component to wakeup Framer when in
 * idle mode. 
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 *  
 * @return None.
 *  
 * @sa HAL_sb_FrmInternalWakeupDisable, HAL_sb_FrmIsInternalWakeupEnabled.
 * 
 * @attention None.
 */
void HAL_sb_FrmInternalWakeupEnable(HAL_sb_HandleType hHandle)
{
  SB_HWIO_OUTF(hHandle,FRM_CFG,INTERNAL_WAKEUP_EN,1);
}


/**
 * Disables devices within the component to wakeup Framer when 
 * in idle mode. 
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 *  
 * @return None.
 *  
 * @sa HAL_sb_FrmInternalWakeupEnable, HAL_sb_FrmIsInternalWakeupEnabled.
 * 
 * @attention None.
 */
void HAL_sb_FrmInternalWakeupDisable(HAL_sb_HandleType hHandle)
{
  SB_HWIO_OUTF(hHandle,FRM_CFG,INTERNAL_WAKEUP_EN,0);
}


/**
 * Checks if devices within the component are enabled to wakeup 
 * Framer when in idle mode. 
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 *  
 * @return TRUE, if enabled, FALSE if disabled.
 *  
 * @sa HAL_sb_FrmInternalWakeupDisable, 
 *     HAL_sb_FrmInternalWakeupEnable
 * 
 * @attention None.
 */
bool32 HAL_sb_FrmIsInternalWakeupEnabled(HAL_sb_HandleType hHandle)
{
  if(0 != SB_HWIO_INF(hHandle,FRM_CFG,INTERNAL_WAKEUP_EN))
  {
    return TRUE; 
  }
  else
  {
    return FALSE;
  }
}

/**
 * Get the Framer Reference Clk CG. 
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 *  
 * @return Equivalent CG of the Slimbus reference Clk.
 *  
 * @sa HAL_sb_FrmSetRefClkClkGear.
 * 
 * @attention None.
 */
uint32 HAL_sb_FrmGetRefClkClkGear(HAL_sb_HandleType hHandle)
{
  return SB_HWIO_INF(hHandle,FRM_CFG,REFERENCE_CLK_CG);
}


/**
 * Set the Framer Reference Clk CG. 
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 *  
 * @param[in] uRefClkClkGear - Equivalent CG of the Slimbus reference
 *       Clk.
 *  
 * @return None.
 *  
 * @sa HAL_sb_FrmGetRefClkClkGear.
 * 
 * @attention None.
 */
void HAL_sb_FrmSetRefClkClkGear(HAL_sb_HandleType hHandle, 
                                uint32 uRefClkClkGear)
{
  SB_HWIO_OUTF(hHandle,FRM_CFG,REFERENCE_CLK_CG,uRefClkClkGear);
}


/**
 * Get the Framer Root Frequency at Boot. 
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 *  
 * @return Framer Root Frequency at Boot.
 *  
 * @sa HAL_sb_FrmSetBootRootFreq.
 * 
 * @attention None.
 */
uint32 HAL_sb_FrmGetBootRootFreq(HAL_sb_HandleType hHandle)
{
  return SB_HWIO_INF(hHandle,FRM_CFG,BOOT_ROOT_FREQ);
}


/**
 * Set the Framer Root Frequency at Boot. 
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 *  
 * @param[in] uBootRootFreq - Framer Root Frequency at Boot.
 *  
 * @return None.
 *  
 * @sa HAL_sb_FrmGetBootRootFreq.
 * 
 * @attention None.
 */
void HAL_sb_FrmSetBootRootFreq(HAL_sb_HandleType hHandle, uint32 uBootRootFreq)
{
  SB_HWIO_OUTF(hHandle,FRM_CFG,BOOT_ROOT_FREQ,uBootRootFreq);
}


/**
 * Get the Framer Clk Gear at Boot. 
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 *  
 * @return Framer Clk Gear at Boot.
 *  
 * @sa HAL_sb_FrmSetBootClkGear.
 * 
 * @attention None.
 */
uint32 HAL_sb_FrmGetBootClkGear(HAL_sb_HandleType hHandle)
{
  return SB_HWIO_INF(hHandle,FRM_CFG,BOOT_CLOCK_GEAR);
}


/**
 * Set the Framer Clk Gear at Boot. 
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 *  
 * @param[in] uBootClkGear - Framer Clock Gear at Boot.
 *  
 * @return None.
 *  
 * @sa HAL_sb_FrmGetBootRootFreq.
 * 
 * @attention None.
 */
void HAL_sb_FrmSetBootClkGear(HAL_sb_HandleType hHandle, uint32 uBootClkGear)
{
  SB_HWIO_OUTF(hHandle,FRM_CFG,BOOT_CLOCK_GEAR,uBootClkGear);
}


/**
 * Get the Framer SubFrame mode at Boot. 
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 *  
 * @return Framer SubFrame mode at at Boot.
 *  
 * @sa HAL_sb_FrmSetBootSubFrmMode.
 * 
 * @attention None.
 */
uint32 HAL_sb_FrmGetBootSubFrmMode(HAL_sb_HandleType hHandle)
{
  return SB_HWIO_INF(hHandle,FRM_CFG,BOOT_SUBFRAME_MODE);
}


/**
 * Set the Framer SubFrame mode at at Boot. 
 * 
 * @param[in] hHandle - This SlimBus hHandle. 
 *  
 * @param[in] uBootSubFrmMode - Framer SubFrame mode at at Boot.
 *  
 * @return None.
 *  
 * @sa HAL_sb_FrmGetBootSubFrmMode.
 * 
 * @attention None.
 */
void HAL_sb_FrmSetBootSubFrmMode(HAL_sb_HandleType hHandle, 
                                 uint32 uBootSubFrmMode)
{
  SB_HWIO_OUTF(hHandle,FRM_CFG,BOOT_SUBFRAME_MODE,uBootSubFrmMode);
}


/**
 * Set this Framer as an active Framer in the Bus at Boot. 
 * 
 * @param[in] hHandle - This SlimBus hHandle. 
 *  
 * @return None.
 *  
 * @sa HAL_sb_FrmBootSetUnActiveFrm, HAL_sb_FrmBootIsActiveFrm.
 * 
 * @attention If set, this Framer Device is made the active one
 *            in the bus. This setting takes effect only when
 *            bus is booting up. Any subsequent call to this API
 *            has no effect until bus re-boots again.
 */
void HAL_sb_FrmBootSetActiveFrm(HAL_sb_HandleType hHandle)
{
  SB_HWIO_OUTF(hHandle,FRM_CFG,BOOT_AS_ACTIVE_FRAMER,1);
}


/**
 * Set this Framer as an unactive Framer in the Bus at Boot. 
 * 
 * @param[in] hHandle - This SlimBus hHandle. 
 *  
 * @return None.
 *  
 * @sa HAL_sb_FrmBootSetActiveFrm,HAL_sb_FrmBootIsActiveFrm.
 * 
 * @attention If set, this Framer Device is made as unactive one
 *            in the bus. This setting takes effect only when
 *            bus is booting up. Any subsequent call to this API
 *            has no effect until bus re-boots again.
 */
void HAL_sb_FrmBootSetUnActiveFrm(HAL_sb_HandleType hHandle)
{
  SB_HWIO_OUTF(hHandle,FRM_CFG,BOOT_AS_ACTIVE_FRAMER,0);
}

/**
 * Check if this Framer is set to be an active Framer in the Bus
 * at Boot. 
 * 
 * @param[in] hHandle - This SlimBus hHandle. 
 *  
 * @return TRUE if set as active, FALSE if set as Unactive.
 *  
 * @sa HAL_sb_FrmBootSetUnActiveFrm, HAL_sb_FrmBootSetActiveFrm.
 * 
 * @attention None.
 */
bool32 HAL_sb_FrmBootIsActiveFrm(HAL_sb_HandleType hHandle)
{
  if(0 != SB_HWIO_INF(hHandle,FRM_CFG,BOOT_AS_ACTIVE_FRAMER))
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}


/**
 * Check if the Slimbus is Booted. 
 * 
 * @param[in] hHandle - This SlimBus hHandle. 
 *  
 * @return TRUE if booted, FALSE otherwise.
 *  
 * @sa None.
 * 
 * @attention None.
 */
bool32 HAL_sb_FrmIsSbBooted(HAL_sb_HandleType hHandle)
{
  if(0 != SB_HWIO_INF(hHandle,FRM_STATUS,FRAMER_BOOTED))
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}

/**
 * Get the Framer Assigned Logical Address. 
 * 
 * @param[in] hHandle - This SlimBus hHandle. 
 *  
 * @return Framer Assigned Logical Address.
 *  
 * @sa HAL_sb_FrmGetBootSubFrmMode, HAL_sb_FrmSetBootSubFrmMode
 * 
 * @attention None.
 */
uint32 HAL_sb_FrmGetAssignedLA(HAL_sb_HandleType hHandle)
{
  return SB_HWIO_INF(hHandle,FRM_STATUS,DEV_LA);
}

/**
 * Get the Framer Current SubFrame mode. 
 * 
 * @param[in] hHandle - This SlimBus hHandle. 
 *  
 * @return Framer Current SubFrame mode.
 *  
 * @sa HAL_sb_FrmGetBootSubFrmMode, HAL_sb_FrmSetBootSubFrmMode
 * 
 * @attention None.
 */
uint32 HAL_sb_FrmGetCurSubFrmMode(HAL_sb_HandleType hHandle)
{
  return SB_HWIO_INF(hHandle,FRM_STATUS,CUR_SUBFRAME_MODE);
}


/**
 * Get the Framer Current Clk Gear. 
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 *  
 * @return Framer Current Clk Gear.
 *  
 * @sa HAL_sb_FrmSetBootClkGear, HAL_sb_FrmGetBootClkGear
 * 
 * @attention None.
 */
uint32 HAL_sb_FrmGetCurClkGear(HAL_sb_HandleType hHandle)
{
  return SB_HWIO_INF(hHandle,FRM_STATUS,CUR_CLOCK_GEAR);
}


/**
 * Get the Framer Current Root Frequency. 
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 *  
 * @return Framer Current Root Frequency.
 *  
 * @sa HAL_sb_FrmSetBootRootFreq, HAL_sb_FrmGetBootRootFreq.
 * 
 * @attention None.
 */
uint32 HAL_sb_FrmGetCurRootFreq(HAL_sb_HandleType hHandle)
{
  return SB_HWIO_INF(hHandle,FRM_STATUS,CUR_ROOT_FREQUENCY);
}


/**
 * Check if this Framer is the active Framer on the bus. 
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 *  
 * @return TRUE if the Framer is the active Framer on the bus,
 *         FALSE otherwise.
 *  
 * @sa None.
 * 
 * @attention None.
 */
bool32 HAL_sb_FrmIsActiveFrm(HAL_sb_HandleType hHandle)
{
  if(0 != SB_HWIO_INF(hHandle,FRM_STATUS,ACTIVE_FRAMER))
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}


/**
 * Check if this Framer is in Idle Mode. 
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 *  
 * @return TRUE if the Framer is in the idle mode, FALSE otherwise.
 *  
 * @sa HAL_sb_FrmWakeUp.
 * 
 * @attention None.
 */
bool32 HAL_sb_FrmIsInIdleMode(HAL_sb_HandleType hHandle)
{
  if(0 != SB_HWIO_INF(hHandle,FRM_STATUS,IDLE_MODE))
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}


/**
 * Wake up Framer, if in Idle mode. 
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 *  
 * @return None.
 *  
 * @sa HAL_sb_FrmIsInIdleMode.
 * 
 * @attention None.
 */
void HAL_sb_FrmWakeUp(HAL_sb_HandleType hHandle)
{
  SB_HWIO_OUTF(hHandle,FRM_WAKEUP,WAKEUP_NOW,1);
}


/** @} */
/** @name PGD Device APIs */
/** @{ */


/**
 * Get the number of ports in this Generic Device. 
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 * 
 * @param[in] uPgdIndex - Ported Generic Device Index.
 *  
 * @return NUM_PORTS.
 * 
 * @sa None.
 * 
 * @attention None.
 */
uint32 HAL_sb_PgdGetNumPorts(HAL_sb_HandleType hHandle, uint32 uPgdIndex)
{
  UNREFERENCED_PARAMETER(uPgdIndex);

  return SB_HWIO_INF(hHandle,PGD_STATUS,NUM_PORTS);
}


/**
 * Get the pipe number offset for this Generic Device. 
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 * 
 * @param[in] uPgdIndex - Ported Generic Device Index.
 *  
 * @return PIPE_NUM_OFFSET.
 * 
 * @sa None.
 * 
 * @attention None.
 */
uint32 HAL_sb_PgdGetPipeOffset(HAL_sb_HandleType hHandle, uint32 uPgdIndex)
{
  UNREFERENCED_PARAMETER(uPgdIndex);

  return SB_HWIO_INF(hHandle,PGD_STATUS,PIPE_NUM_OFFSET);
}

/**
 * Get the number of Progressive Counter and VFR Timestamp logic
 * blocks. 
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 * 
 * @param[in] uPgdIndex - Ported Generic Device Index.
 *  
 * @return NUM_PC_VFR_BLKS.
 * 
 * @sa None.
 * 
 * @attention None.
 */
uint32 HAL_sb_PgdGetNumPcVfrBlks(HAL_sb_HandleType hHandle, uint32 uPgdIndex)
{
  UNREFERENCED_PARAMETER(uPgdIndex);

  return SB_HWIO_INF(hHandle,PGD_STATUS,NUM_PC_VFR_BLKS);
}


/**
 * Set the port interrupt ownership of an EE. 
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 * 
 * @param[in] uPgdIndex - Ported Generic Device Index. 
 *  
 * @param[in] uEE - Execution Environment.
 *  
 * @param[in] uPortBitField - Bit field. If bit n of this 
 *       parameter is set, then uEE owns that port number.
 *  
 * @return HAL_sb_PgdGetNumPorts.
 * 
 * @sa None.
 * 
 * @attention None.
 */
void HAL_sb_PgdPortSetOwnerEe(HAL_sb_HandleType hHandle, 
                              uint32 uPgdIndex, 
                              uint32 uEE, 
                              uint32 uPortBitField)
{
  UNREFERENCED_PARAMETER(uPgdIndex);

  switch(uEE)
  {
    case 0:
    {
      SB_HWIO_OUTF(hHandle,PGD_TRUST_OWN_EE0,PORT_OWN,uPortBitField);
      break;
    }
    case 1:
    {
      SB_HWIO_OUTF(hHandle,PGD_TRUST_OWN_EE1,PORT_OWN,uPortBitField);
      break;
    }
    case 2:
    {
      SB_HWIO_OUTF(hHandle,PGD_TRUST_OWN_EE2,PORT_OWN,uPortBitField);
      break;
    }
    default:
      break;
  }
}


/**
 * Get the port interrupt ownership of an EE. 
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 * 
 * @param[in] uPgdIndex - Ported Generic Device Index. 
 *  
 * @param[in] uEE - Execution Environment.
 *  
 * @return Bit field. If bit n of this parameter is set, then 
 *         uEE owns that port number.
 * 
 * @sa HAL_sb_PgdGetNumPorts.
 * 
 * @attention None.
 */
uint32 HAL_sb_PgdPortGetOwnerEe(HAL_sb_HandleType hHandle, 
                                uint32 uPgdIndex, 
                                uint32 uEE)
{
  uint32 uPortBitField = 0;

  UNREFERENCED_PARAMETER(uPgdIndex);

  switch(uEE)
  {
    case 0:
    {
      uPortBitField = SB_HWIO_INF(hHandle,PGD_TRUST_OWN_EE0,PORT_OWN);
      break;
    }
    case 1:
    {
      uPortBitField = SB_HWIO_INF(hHandle,PGD_TRUST_OWN_EE1,PORT_OWN);
      break;
    }
    case 2:
    {
      uPortBitField = SB_HWIO_INF(hHandle,PGD_TRUST_OWN_EE2,PORT_OWN);
      break;
    }
    default:
      break;
  }
  return uPortBitField;
}


/**
 * Set the ports interrupt enable bit for an EE. 
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 * 
 * @param[in] uPgdIndex - Ported Generic Device Index. 
 *  
 * @param[in] uEE - Execution Environment.
 *  
 * @param[in] uPortBitField - Bit field. If bit n of this 
 *       parameter is set, then nth port interrupt is enabled
 *       for uEE. Only the bit corresponding to owned port
 *       should be changed for an EE.
 *  
 * @return None.
 * 
 * @sa HAL_sb_PgdGetNumPorts.
 * 
 * @attention Only bits upto the number dictacted by 
 *            HAL_sb_PgdGetNumPorts() api are valid.
 */
void HAL_sb_PgdPortSetIntEnEe(HAL_sb_HandleType hHandle, 
                              uint32 uPgdIndex, 
                              uint32 uEE, 
                              uint32 uPortBitField)
{
  UNREFERENCED_PARAMETER(uPgdIndex);

  if(SB_MAXn(PGD_PORT_INT_EN_EEn)  >= uEE)
  {
    SB_HWIO_OUTFI(hHandle,PGD_PORT_INT_EN_EEn,uEE,PORT_INT,uPortBitField);
  }
}

/**
 * Get the ports interrupt enable bit for an EE. 
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 * 
 * @param[in] uPgdIndex - Ported Generic Device Index. 
 *  
 * @param[in] uEE - Execution Environment.
 *  
 * @return Bit field. If bit n of this parameter is set, then 
 *       nth port interrupt is enabled for uEE.
 * 
 * @sa HAL_sb_PgdGetNumPorts.
 * 
 * @attention Only bits upto the number dictacted by 
 *            HAL_sb_PgdGetNumPorts() api are valid.
 */
uint32 HAL_sb_PgdPortGetIntEnEe(HAL_sb_HandleType hHandle, 
                                uint32 uPgdIndex, 
                                uint32 uEE)
{
  uint32 uPortBitField = 0;

  UNREFERENCED_PARAMETER(uPgdIndex);

  if(SB_MAXn(PGD_PORT_INT_EN_EEn) >= uEE)
  {
    uPortBitField = SB_HWIO_INFI(hHandle,PGD_PORT_INT_EN_EEn,uEE,PORT_INT);
  }
  return uPortBitField;
}


/**
 * Get the port interrupt status bit for an EE. 
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 * 
 * @param[in] uPgdIndex - Ported Generic Device Index. 
 *  
 * @param[in] uEE - Execution Environment.
 *  
 * @return Bit field. If bit n of this return value is set, 
 *       then nth port interrupt status is set.
 * 
 * @sa HAL_sb_PgdGetNumPorts.
 * 
 * @attention Only bits upto the number dictacted by 
 *            HAL_sb_PgdGetNumPorts() api are valid.
 */
uint32 HAL_sb_PgdPortGetIntStatusEe(HAL_sb_HandleType hHandle, 
                                    uint32 uPgdIndex, 
                                    uint32 uEE)
{
  uint32 uPortBitField = 0;

  UNREFERENCED_PARAMETER(uPgdIndex);

  if(SB_MAXn(PGD_PORT_INT_STATUS_EEn) >= uEE)
  {
    uPortBitField = SB_HWIO_INFI(hHandle,
                                 PGD_PORT_INT_STATUS_EEn,
                                 uEE,
                                 PORT_INT);
  }
  return uPortBitField;
}


/**
 * Clear the port interrupt status bit for an EE. 
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 * 
 * @param[in] uPgdIndex - Ported Generic Device Index. 
 *  
 * @param[in] uEE - Execution Environment.
 *  
 * @param[in] uPortBitField -  If bit n of this parameter is set, then 
 *       nth port interrupt status bit will be cleared. Only the
 *       bit corresponding to owned port should be changed for
 *       an EE.
 *  
 * @return None.
 * 
 * @sa HAL_sb_PgdGetNumPorts.
 * 
 * @attention Only bits upto the number dictacted by 
 *            HAL_sb_PgdGetNumPorts() api are valid.
 */
void HAL_sb_PgdPortClearIntEe(HAL_sb_HandleType hHandle, 
                              uint32 uPgdIndex, 
                              uint32 uEE, 
                              uint32 uPortBitField)
{
  UNREFERENCED_PARAMETER(uPgdIndex);

  if(SB_MAXn(PGD_PORT_INT_CLR_EEn)  >= uEE)
  {
    SB_HWIO_OUTFI(hHandle,
                  PGD_PORT_INT_CLR_EEn,
                  uEE,
                  PORT_INT,
                  uPortBitField);
  }
}


/**
 * Enable a Port. 
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 * 
 * @param[in] uPgdIndex - Ported Generic Device Index. 
 *  
 * @param[in] uPortNum -  Port number
 *  
 * @return None.
 * 
 * @sa HAL_sb_PgdPortDisable, HAL_sb_PgdPortIsEnabled.
 * 
 * @attention None.
 */
void HAL_sb_PgdPortEnable(HAL_sb_HandleType hHandle, 
                          uint32 uPgdIndex, 
                          uint32 uPortNum)
{
  UNREFERENCED_PARAMETER(uPgdIndex);

  if(uPortNum <= SB_MAXn(PGD_PORT_CFGn))
  {
    SB_HWIO_OUTFI(hHandle,PGD_PORT_CFGn,uPortNum,ENABLE,1);
  }
}


/**
 * Disable a Port. 
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 * 
 * @param[in] uPgdIndex - Ported Generic Device Index. 
 *  
 * @param[in] uPortNum -  Port number
 *  
 * @return None.
 * 
 * @sa HAL_sb_PgdPortEnable, HAL_sb_PgdPortIsEnabled.
 * 
 * @attention None.
 */
void HAL_sb_PgdPortDisable(HAL_sb_HandleType hHandle, 
                           uint32 uPgdIndex, 
                           uint32 uPortNum)
{
  UNREFERENCED_PARAMETER(uPgdIndex);

  if(uPortNum <= SB_MAXn(PGD_PORT_CFGn))
  {
    SB_HWIO_OUTFI(hHandle,PGD_PORT_CFGn,uPortNum,ENABLE,0);
  }
}


/**
 * Check if a Port is enabled. 
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 * 
 * @param[in] uPgdIndex - Ported Generic Device Index. 
 *  
 * @param[in] uPortNum -  Port number
 *  
 * @return TRUE, if port is enabled, FALSE otherwise.
 * 
 * @sa HAL_sb_PgdPortDisable, HAL_sb_PgdPortEnable.
 * 
 * @attention None.
 */
bool32 HAL_sb_PgdPortIsEnabled(HAL_sb_HandleType hHandle, 
                               uint32 uPgdIndex, 
                               uint32 uPortNum)
{
  UNREFERENCED_PARAMETER(uPgdIndex);

  if(uPortNum <= SB_MAXn(PGD_PORT_CFGn))
  {
    if(0 != SB_HWIO_INFI(hHandle,PGD_PORT_CFGn,uPortNum,ENABLE))
    {
      return TRUE;
    }
  }
  return FALSE;
}


/**
 * Set the Watermark level for a port. 
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 * 
 * @param[in] uPgdIndex - Ported Generic Device Index. 
 *  
 * @param[in] uPortNum -  Port number.
 *  
 * @param[in] uWaterMark -  Watermark level.
 *  
 * @return None.
 * 
 * @sa HAL_sb_PgdPortGetWatermark.
 * 
 * @attention None.
 */
void HAL_sb_PgdPortSetWatermark(HAL_sb_HandleType hHandle, 
                                uint32 uPgdIndex, 
                                uint32 uPortNum, 
                                uint32 uWaterMark)
{
  UNREFERENCED_PARAMETER(uPgdIndex);

  if(uPortNum <= SB_MAXn(PGD_PORT_CFGn))
  {
    SB_HWIO_OUTFI(hHandle,PGD_PORT_CFGn,uPortNum,WATERMARK,uWaterMark);
  }
}


/**
 * Get the Watermark level for a port. 
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 * 
 * @param[in] uPgdIndex - Ported Generic Device Index. 
 *  
 * @param[in] uPortNum -  Port number.
 *  
 * @return Watermark level.
 * 
 * @sa HAL_sb_PgdPortSetWatermark.
 * 
 * @attention None.
 */
uint32 HAL_sb_PgdPortGetWatermark(HAL_sb_HandleType hHandle, 
                                  uint32 uPgdIndex, 
                                  uint32 uPortNum)
{
  uint32 uWaterMark = 0;

  UNREFERENCED_PARAMETER(uPgdIndex);

  if(uPortNum <= SB_MAXn(PGD_PORT_CFGn))
  {
    uWaterMark = SB_HWIO_INFI(hHandle,PGD_PORT_CFGn,uPortNum,WATERMARK);
  }
  return uWaterMark;
}


/**
 * Set the upper watermark level for a port. 
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 * 
 * @param[in] uPgdIndex - Ported Generic Device Index. 
 *  
 * @param[in] uPortNum -  Port number.
 *  
 * @param[in] uWaterMark -  Upper watermark level.
 *  
 * @return None.
 * 
 * @sa HAL_sb_PgdPortGetUpperWatermark, 
 *     HAL_sb_PgdPortSetWatermark, HAL_sb_PgdPortGetWatermark,
 *     HAL_sb_PgdPortUpperWatermarkEnable,
 *     HAL_sb_PgdPortUpperWatermarkDisable
 * 
 * @attention None.
 */
void HAL_sb_PgdPortSetUpperWatermark(HAL_sb_HandleType hHandle, 
                                     uint32 uPgdIndex, 
                                     uint32 uPortNum, 
                                     uint32 uWaterMark)
{
  UNREFERENCED_PARAMETER(uPgdIndex);

  if(uPortNum <= SB_MAXn(PGD_PORT_CFGn))
  {
    SB_HWIO_OUTFI(hHandle,PGD_PORT_CFGn,uPortNum,UPPER_WATERMARK,uWaterMark);
  }
}


/**
 * Get the Upper watermark level for a port. 
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 * 
 * @param[in] uPgdIndex - Ported Generic Device Index. 
 *  
 * @param[in] uPortNum -  Port number.
 *  
 * @return Upper watermark level.
 * 
 * @sa HAL_sb_PgdPortSetUpperWatermark, 
 *     HAL_sb_PgdPortSetWatermark, HAL_sb_PgdPortGetWatermark,
 *     HAL_sb_PgdPortUpperWatermarkEnable,
 *     HAL_sb_PgdPortUpperWatermarkDisable
 * 
 * @attention None.
 */
uint32 HAL_sb_PgdPortGetUpperWatermark(HAL_sb_HandleType hHandle, 
                                       uint32 uPgdIndex, 
                                       uint32 uPortNum)
{
  uint32 uUpperWaterMark = 0;

  UNREFERENCED_PARAMETER(uPgdIndex);

  if(uPortNum <= SB_MAXn(PGD_PORT_CFGn))
  {
    uUpperWaterMark = SB_HWIO_INFI(hHandle,PGD_PORT_CFGn,uPortNum,UPPER_WATERMARK);
  }
  return uUpperWaterMark;
}


/**
 * Enable use of the upper watermark for a port. 
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 * 
 * @param[in] uPgdIndex - Ported Generic Device Index. 
 *  
 * @param[in] uPortNum -  Port number.
 *  
 * @return None.
 * 
 * @sa HAL_sb_PgdPortSetUpperWatermark, 
 *     HAL_sb_PgdPortGetUpperWatermark,
 *     HAL_sb_PgdPortUpperWatermarkDisable 
 * 
 * @attention None.
 */
void HAL_sb_PgdPortUpperWatermarkEnable(HAL_sb_HandleType hHandle, 
                                        uint32 uPgdIndex, 
                                        uint32 uPortNum)
{
  UNREFERENCED_PARAMETER(uPgdIndex);

  if(uPortNum <= SB_MAXn(PGD_PORT_CFGn))
  {
    SB_HWIO_OUTFI(hHandle,PGD_PORT_CFGn,uPortNum,USE_UPPER_WATERMARK,1);
  }
}


/**
 * Disable use of the upper watermark for a port. 
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 * 
 * @param[in] uPgdIndex - Ported Generic Device Index. 
 *  
 * @param[in] uPortNum -  Port number.
 *  
 * @return None.
 * 
 * @sa HAL_sb_PgdPortSetUpperWatermark, 
 *     HAL_sb_PgdPortGetUpperWatermark,
 *     HAL_sb_PgdPortUpperWatermarkEnable
 * 
 * @attention None.
 */
void HAL_sb_PgdPortUpperWatermarkDisable(HAL_sb_HandleType hHandle, 
                                         uint32 uPgdIndex, 
                                         uint32 uPortNum)
{
  UNREFERENCED_PARAMETER(uPgdIndex);

  if(uPortNum <= SB_MAXn(PGD_PORT_CFGn))
  {
    SB_HWIO_OUTFI(hHandle,PGD_PORT_CFGn,uPortNum,USE_UPPER_WATERMARK,0);
  }
}


/**
 * Enable the data packing for a port. 
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 * 
 * @param[in] uPgdIndex - Ported Generic Device Index. 
 *  
 * @param[in] uPortNum -  Port number.
 *  
 * @return None.
 * 
 * @sa HAL_sb_PgdPortPackDisable, HAL_sb_PgdPortIsPackEnabled.
 * 
 * @attention None.
 */
void HAL_sb_PgdPortPackEnable(HAL_sb_HandleType hHandle, 
                              uint32 uPgdIndex, 
                              uint32 uPortNum)
{
  UNREFERENCED_PARAMETER(uPgdIndex);

  if(uPortNum <= SB_MAXn(PGD_PORT_CFGn))
  {
    SB_HWIO_OUTFI(hHandle,PGD_PORT_CFGn,uPortNum,PACK,1);
  }
}


/**
 * Disable the data packing for a port. 
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 * 
 * @param[in] uPgdIndex - Ported Generic Device Index. 
 *  
 * @param[in] uPortNum -  Port number.
 *  
 * @return None.
 * 
 * @sa HAL_sb_PgdPortPackEnable, HAL_sb_PgdPortIsPackEnabled.
 * 
 * @attention None.
 */
void HAL_sb_PgdPortPackDisable(HAL_sb_HandleType hHandle, 
                               uint32 uPgdIndex, 
                               uint32 uPortNum)
{
  UNREFERENCED_PARAMETER(uPgdIndex);

  if(uPortNum <= SB_MAXn(PGD_PORT_CFGn))
  {
    SB_HWIO_OUTFI(hHandle,PGD_PORT_CFGn,uPortNum,PACK,0);
  }
}


/**
 * Check if the data packing is enabled for a port. 
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 * 
 * @param[in] uPgdIndex - Ported Generic Device Index. 
 *  
 * @param[in] uPortNum -  Port number.
 *  
 * @return TRUE, if packing enabled, FALSE otherwise.
 * 
 * @sa HAL_sb_PgdPortPackEnable, HAL_sb_PgdPortPackDisable. 
 * 
 * @attention None.
 */
bool32 HAL_sb_PgdPortIsPackEnabled(HAL_sb_HandleType hHandle, 
                                   uint32 uPgdIndex, 
                                   uint32 uPortNum)
{
  UNREFERENCED_PARAMETER(uPgdIndex);

  if(uPortNum <= SB_MAXn(PGD_PORT_CFGn))
  {
    if(0 != SB_HWIO_INFI(hHandle,PGD_PORT_CFGn,uPortNum,PACK))
    {
      return TRUE;
    }
  }
  return FALSE;
}


/**
 * Set the data alignment settings for a port.
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 * 
 * @param[in] uPgdIndex - Ported Generic Device Index. 
 *  
 * @param[in] uPortNum -  Port number. 
 *  
 * @param[in] eAlignment - Alignment type
 *  
 * @return None.
 * 
 * @sa HAL_sb_PgdPortGetAlignment. 
 * 
 * @attention None.
 */
void HAL_sb_PgdPortSetAlignment(HAL_sb_HandleType hHandle, 
                                uint32 uPgdIndex, 
                                uint32 uPortNum, 
                                HAL_sb_PgdPortDataAlignType eAlignment)
{
  UNREFERENCED_PARAMETER(uPgdIndex);

  if(uPortNum <= SB_MAXn(PGD_PORT_CFGn))
  {
    SB_HWIO_OUTFI(hHandle,PGD_PORT_CFGn,uPortNum,ALIGN_MSB,eAlignment);
  }
}


/**
 * Get the data alignment settings for a port.
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 * 
 * @param[in] uPgdIndex - Ported Generic Device Index. 
 *  
 * @param[in] uPortNum -  Port number. 
 *  
 * @return Alignment type 
 *  
 * @sa HAL_sb_PgdPortSetAlignment. 
 * 
 * @attention None.
 */
HAL_sb_PgdPortDataAlignType HAL_sb_PgdPortGetAlignment
                                       (HAL_sb_HandleType hHandle, 
                                        uint32 uPgdIndex, 
                                        uint32 uPortNum)
{
  HAL_sb_PgdPortDataAlignType eAlignment = HAL_SB_PGD_PORT_DATA_ALIGN_LSB;

  UNREFERENCED_PARAMETER(uPgdIndex);

  if(uPortNum <= SB_MAXn(PGD_PORT_CFGn))
  {
    eAlignment =  (HAL_sb_PgdPortDataAlignType) SB_HWIO_INFI(hHandle,
                                                             PGD_PORT_CFGn,
                                                             uPortNum,
                                                             ALIGN_MSB);
  }
  return eAlignment;
}


/**
 * Enable the DMA interrupt for a port. 
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 * 
 * @param[in] uPgdIndex - Ported Generic Device Index. 
 *  
 * @param[in] uPortNum -  Port number.
 *  
 * @return None.
 * 
 * @sa HAL_sb_PgdPortDmaIrqDisable, 
 *     HAL_sb_PgdPortIsDmaIrqEnabled
 * 
 * @attention None.
 */
void HAL_sb_PgdPortDmaIrqEnable(HAL_sb_HandleType hHandle, 
                                uint32 uPgdIndex, 
                                uint32 uPortNum)
{
  UNREFERENCED_PARAMETER(uPgdIndex);

  if(uPortNum <= SB_MAXn(PGD_PORT_CFGn))
  {
    SB_HWIO_OUTFI(hHandle,PGD_PORT_CFGn,uPortNum,DMA_IRQ_EN,1);
  }
}


/**
 * Disable the DMA interrupt for a port. 
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 * 
 * @param[in] uPgdIndex - Ported Generic Device Index. 
 *  
 * @param[in] uPortNum -  Port number.
 *  
 * @return None.
 * 
 * @sa HAL_sb_PgdPortDmaIrqEnable, 
 *     HAL_sb_PgdPortIsDmaIrqEnabled
 * 
 * @attention None.
 */
void HAL_sb_PgdPortDmaIrqDisable(HAL_sb_HandleType hHandle, 
                                 uint32 uPgdIndex, 
                                 uint32 uPortNum)
{
  UNREFERENCED_PARAMETER(uPgdIndex);

  if(uPortNum <= SB_MAXn(PGD_PORT_CFGn))
  {
    SB_HWIO_OUTFI(hHandle,PGD_PORT_CFGn,uPortNum,DMA_IRQ_EN,0);
  }
}


/**
 * Check if the DMA interrupt is enabled for a port. 
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 * 
 * @param[in] uPgdIndex - Ported Generic Device Index. 
 *  
 * @param[in] uPortNum -  Port number.
 *  
 * @return TRUE, if DMA interrupt is enabled, FALSE otherwise.
 * 
 * @sa HAL_sb_PgdPortDmaIrqEnable, HAL_sb_PgdPortDmaIrqDisable
 * 
 * @attention None.
 */
bool32 HAL_sb_PgdPortIsDmaIrqEnabled(HAL_sb_HandleType hHandle, 
                                     uint32 uPgdIndex, 
                                     uint32 uPortNum)
{
  UNREFERENCED_PARAMETER(uPgdIndex);

  if(uPortNum <= SB_MAXn(PGD_PORT_CFGn))
  {
    if(0 != SB_HWIO_INFI(hHandle,PGD_PORT_CFGn,uPortNum,DMA_IRQ_EN))
    {
      return TRUE;
    }
  }
  return FALSE;
}


/**
 * Get the FIFO size for a port.
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 * 
 * @param[in] uPgdIndex - Ported Generic Device Index. 
 *  
 * @param[in] uPortNum -  Port number. 
 *  
 * @return Fifo size in bytes.
 *  
 * @sa None. 
 * 
 * @attention None.
 */
uint32 HAL_sb_PgdPortGetFifoSize(HAL_sb_HandleType hHandle, 
                                 uint32 uPgdIndex, 
                                 uint32 uPortNum)
{
  uint32 uFifoSize = 0;

  UNREFERENCED_PARAMETER(uPgdIndex);

  if(uPortNum <= SB_MAXn(PGD_PORT_CFGn))
  {
    switch (SB_HWIO_INFI(hHandle,PGD_PORT_STATUSn,uPortNum,FIFO_SIZE))
    {
      case 0 : 
        uFifoSize = 8;
        break;
      case 1 : 
        uFifoSize = 16;
        break;
      case 2 : 
        uFifoSize = 32;
        break;
      case 3 : 
        uFifoSize = 64;
        break;
     default:
       break;   
    }
  }
  return uFifoSize;
}


/**
 * Check if the FIFO has overflowed for a port.
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 * 
 * @param[in] uPgdIndex - Ported Generic Device Index. 
 *  
 * @param[in] uPortNum -  Port number. 
 *  
 * @return TRUE, if overflowed. FALSE otherwise.
 *  
 * @sa HAL_sb_PgdPortIsUnderflow. 
 * 
 * @attention None.
 */
bool32 HAL_sb_PgdPortIsOverflow(HAL_sb_HandleType hHandle, 
                                uint32 uPgdIndex, 
                                uint32 uPortNum)
{
  UNREFERENCED_PARAMETER(uPgdIndex);

  if(uPortNum <= SB_MAXn(PGD_PORT_STATUSn))
  {
    if(0 != SB_HWIO_INFI(hHandle,PGD_PORT_STATUSn,uPortNum,OVERFLOW))
    {
      return TRUE;
    }
  }
  return FALSE;
}


/**
 * Check if the FIFO has underflowed for a port.
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 * 
 * @param[in] uPgdIndex - Ported Generic Device Index. 
 *  
 * @param[in] uPortNum -  Port number. 
 *  
 * @return TRUE, if underflowed. FALSE otherwise.
 *  
 * @sa HAL_sb_PgdPortIsOverflow. 
 * 
 * @attention None.
 */
bool32 HAL_sb_PgdPortIsUnderflow(HAL_sb_HandleType hHandle, 
                                 uint32 uPgdIndex, 
                                 uint32 uPortNum)
{
  UNREFERENCED_PARAMETER(uPgdIndex);

  if(uPortNum <= SB_MAXn(PGD_PORT_STATUSn))
  {
    if(0 != SB_HWIO_INFI(hHandle,PGD_PORT_STATUSn,uPortNum,UNDERFLOW))
    {
      return TRUE;
    }
  }
  return FALSE;
}


/**
 * Get the pipe number for a port.
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 * 
 * @param[in] uPgdIndex - Ported Generic Device Index. 
 *  
 * @param[in] uPortNum -  Port number. 
 *  
 * @return PIPE_NUM.
 *  
 * @sa None. 
 * 
 * @attention None.
 */
uint32 HAL_sb_PgdPortGetPipeNum(HAL_sb_HandleType hHandle, 
                                uint32 uPgdIndex, 
                                uint32 uPortNum)
{
  uint32 uPipeNum = 0;

  UNREFERENCED_PARAMETER(uPgdIndex);

  if(uPortNum <= SB_MAXn(PGD_PORT_STATUSn))
  {
    uPipeNum = SB_HWIO_INFI(hHandle,PGD_PORT_STATUSn,uPortNum,PIPE_NUM);
  }
  return uPipeNum;
}


/**
 * Get the number of words currently present in the FIFO. 
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 * 
 * @param[in] uPgdIndex - Ported Generic Device Index. 
 *  
 * @param[in] uPortNum -  Port number. 
 *  
 * @return FIFO_WORD_CNT.
 *  
 * @sa None. 
 * 
 * @attention This is the dynamic value of the count which may 
 *            change.
 */
uint32 HAL_sb_PgdPortGetCurrentFifoWordCount(HAL_sb_HandleType hHandle, 
                                             uint32 uPgdIndex, 
                                             uint32 uPortNum)
{
  uint32 uWordCount = 0;

  UNREFERENCED_PARAMETER(uPgdIndex);

  if(uPortNum <= SB_MAXn(PGD_PORT_STATUSn))
  {
    uWordCount = SB_HWIO_INFI(hHandle,PGD_PORT_STATUSn,uPortNum,FIFO_WORD_CNT);
  }
  return uWordCount;
}


/**
 * Check if the FIFO contains "watermark" worth of data for a 
 * port when acting a source to a data channel. 
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 * 
 * @param[in] uPgdIndex - Ported Generic Device Index. 
 *  
 * @param[in] uPortNum -  Port number. 
 *  
 * @return TRUE, if watermark is hit, FALSE otherwise.
 *  
 * @sa None. 
 * 
 * @attention None.
 */
bool32 HAL_sb_PgdPortIsWatermarkHit(HAL_sb_HandleType hHandle, 
                                    uint32 uPgdIndex, 
                                    uint32 uPortNum)
{
  UNREFERENCED_PARAMETER(uPgdIndex);

  if(uPortNum <= SB_MAXn(PGD_PORT_STATUSn))
  {
    if(0 != SB_HWIO_INFI(hHandle,PGD_PORT_STATUSn,uPortNum,FIFO_FILL_LEVEL))
    {
      return TRUE;
    }
  }
  return FALSE;
}


/**
 * Check if the Port disconnect event has happened for a port.
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 * 
 * @param[in] uPgdIndex - Ported Generic Device Index. 
 *  
 * @param[in] uPortNum -  Port number. 
 *  
 * @return TRUE, if Port disconnect event has happened, FALSE
 *         otherwise.
 *  
 * @sa None. 
 * 
 * @attention None.
 */
bool32 HAL_sb_PgdPortIsDisconnectEventSet(HAL_sb_HandleType hHandle, 
                                          uint32 uPgdIndex, 
                                          uint32 uPortNum)
{
  UNREFERENCED_PARAMETER(uPgdIndex);

  if(uPortNum <= SB_MAXn(PGD_PORT_STATUSn))
  {
    if(0 != SB_HWIO_INFI(hHandle,PGD_PORT_STATUSn,uPortNum,PRT_DISCONN))
    {
      return TRUE;
    }
  }
  return FALSE;
}


/**
 * Check if the Port DMA event has happened for a port.
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 * 
 * @param[in] uPgdIndex - Ported Generic Device Index. 
 *  
 * @param[in] uPortNum -  Port number. 
 *  
 * @return TRUE, if Port DMA event has happened, FALSE
 *         otherwise.
 *  
 * @sa None. 
 * 
 * @attention None.
 */
bool32 HAL_sb_PgdPortIsDmaEventSet(HAL_sb_HandleType hHandle, 
                                   uint32 uPgdIndex, 
                                   uint32 uPortNum)
{
  UNREFERENCED_PARAMETER(uPgdIndex);

  if(uPortNum <= SB_MAXn(PGD_PORT_STATUSn))
  {
    if(0 != SB_HWIO_INFI(hHandle,PGD_PORT_STATUSn,uPortNum,DMA_IRQ))
    {
      return TRUE;
    }
  }
  return FALSE;
}


/**
 * Check the events that are asserted for a port. 
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 * 
 * @param[in] uPgdIndex - Ported Generic Device Index. 
 *  
 * @param[in] uPortNum -  Port number. 
 *  
 * @return Enumeration representing the asserted events for the 
 *         port.
 *  
 * @sa None. 
 * 
 * @attention None.
 */
HAL_sb_PgdPortEventType HAL_sb_PgdPortGetEvents(HAL_sb_HandleType hHandle,
                                                uint32 uPgdIndex,
                                                uint32 uPortNum)
{
  HAL_sb_PgdPortEventType eEvents = HAL_SB_PGD_PORT_NO_EVENT;


  UNREFERENCED_PARAMETER(uPgdIndex);
  
  if(uPortNum <= SB_MAXn(PGD_PORT_STATUSn))
  {
    uint32 uStatus = SB_HWIO_INI(hHandle,PGD_PORT_STATUSn,uPortNum);
    
    if( uStatus & SB_BSMSK(PGD_PORT_STATUSn,OVERFLOW) )
    {
      eEvents |= HAL_SB_PGD_PORT_OVERFLOW_EVENT;
    }
    if( uStatus & SB_BSMSK(PGD_PORT_STATUSn,UNDERFLOW) )
    {
      eEvents |= HAL_SB_PGD_PORT_UNDERFLOW_EVENT;
    }
    if( uStatus & SB_BSMSK(PGD_PORT_STATUSn,PRT_DISCONN) )
    {
      eEvents |= HAL_SB_PGD_PORT_DISCONNECT_EVENT;
    }
    if( uStatus & SB_BSMSK(PGD_PORT_STATUSn,REMOVE_CHANNEL) )
    {
      eEvents |= HAL_SB_PGD_PORT_REMOVE_CHANNEL_EVENT;
    }
    if( uStatus & SB_BSMSK(PGD_PORT_STATUSn,DMA_IRQ) )
    {
      eEvents |= HAL_SB_PGD_PORT_DMA_EVENT;
    }
  }
  
  return eEvents;
}


/**
 * Check if the channel is active for a port. 
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 * 
 * @param[in] uPgdIndex - Ported Generic Device Index. 
 *  
 * @param[in] uPortNum -  Port number. 
 *  
 * @return TRUE, if channel is active, FALSE otherwise.
 *  
 * @sa None. 
 * 
 * @attention None.
 */
bool32 HAL_sb_PgdPortIsChannelActive(HAL_sb_HandleType hHandle, 
                                     uint32 uPgdIndex, 
                                     uint32 uPortNum)
{
  UNREFERENCED_PARAMETER(uPgdIndex);

  if(uPortNum <= SB_MAXn(PGD_PORT_PARAMn))
  {
    if(0 != SB_HWIO_INFI(hHandle,PGD_PORT_PARAMn,uPortNum,CHANNEL_ACTIVE))
    {
      return TRUE;
    }
  }
  return FALSE;
}


/**
 * Get the channel number for a Port. 
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 * 
 * @param[in] uPgdIndex - Ported Generic Device Index. 
 *  
 * @param[in] uPortNum -  Port number. 
 *  
 * @return Channel Number.
 *  
 * @sa None. 
 * 
 * @attention None.
 */
uint32 HAL_sb_PgdPortGetChannelNumber(HAL_sb_HandleType hHandle, 
                                      uint32 uPgdIndex, 
                                      uint32 uPortNum)
{
  uint32 uChannelNumber = 0;

  UNREFERENCED_PARAMETER(uPgdIndex);

  if(uPortNum <= SB_MAXn(PGD_PORT_PARAMn))
  {
    uChannelNumber = SB_HWIO_INFI(hHandle,
                                  PGD_PORT_PARAMn,
                                  uPortNum,
                                  CHANNEL_NUMBER);
  }
  return uChannelNumber;
}


/**
 * Get the channel Segment distribution value for a Port . 
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 * 
 * @param[in] uPgdIndex - Ported Generic Device Index. 
 *  
 * @param[in] uPortNum -  Port number. 
 *  
 * @return Segment distribution value.
 *  
 * @sa None. 
 * 
 * @attention None.
 */
uint32 HAL_sb_PgdPortGetSegDistribution(HAL_sb_HandleType hHandle, 
                                        uint32 uPgdIndex, 
                                        uint32 uPortNum)
{  
  uint32 uDistribution = 0;

  UNREFERENCED_PARAMETER(uPgdIndex);

  if(uPortNum <= SB_MAXn(PGD_PORT_PARAMn))
  {
    uDistribution = SB_HWIO_INFI(hHandle,PGD_PORT_PARAMn,uPortNum,SEG_DISTN);
  }
  return uDistribution;
}


/**
 * Get the Channel Transport Protocol for a Port. 
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 * 
 * @param[in] uPgdIndex - Ported Generic Device Index. 
 *  
 * @param[in] uPortNum -  Port number. 
 *  
 * @return Channel Transport Protocol.
 *  
 * @sa None. 
 * 
 * @attention None.
 */
HAL_sb_TransportProtocolType HAL_sb_PgdPortGetTransportProtocol(
                                   HAL_sb_HandleType hHandle, 
                                   uint32 uPgdIndex, 
                                   uint32 uPortNum)
{  
  HAL_sb_TransportProtocolType eProtocol = HAL_SB_TP_ISOCHRONOUS;

  UNREFERENCED_PARAMETER(uPgdIndex);

  if(uPortNum <= SB_MAXn(PGD_PORT_PARAMn))
  {
    eProtocol = (HAL_sb_TransportProtocolType) SB_HWIO_INFI(hHandle,
                                                            PGD_PORT_PARAMn,
                                                            uPortNum,
                                                            TRANSPORT_PROTOCOL);
  }
  return eProtocol;
}


/**
 * Get the Channel Segment Length for a Port.
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 * 
 * @param[in] uPgdIndex - Ported Generic Device Index. 
 *  
 * @param[in] uPortNum -  Port number. 
 *  
 * @return Segment Length.
 *  
 * @sa None. 
 * 
 * @attention None.
 */
uint32 HAL_sb_PgdPortGetSegLen(HAL_sb_HandleType hHandle, 
                               uint32 uPgdIndex, 
                               uint32 uPortNum)
{  
  uint32 uLength = 0;

  UNREFERENCED_PARAMETER(uPgdIndex);

  if(uPortNum <= SB_MAXn(PGD_PORT_PARAMn))
  {
    uLength = SB_HWIO_INFI(hHandle,PGD_PORT_PARAMn,uPortNum,SEG_LENGTH);
  }
  return uLength;
}


/**
 * Set the Block Size for a Port .
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 * 
 * @param[in] uPgdIndex - Ported Generic Device Index. 
 *  
 * @param[in] uPortNum -  Port number. 
 *  
 * @param[in] uBlkSize -  Block size in units of bytes. 
 *   
 * @return None.
 *  
 * @sa HAL_sb_PgdPortGetBlkSize. 
 * 
 * @attention None.
 */
void HAL_sb_PgdPortSetBlkSize(HAL_sb_HandleType hHandle, 
                              uint32 uPgdIndex, 
                              uint32 uPortNum, 
                              uint32 uBlkSize)
{  
  UNREFERENCED_PARAMETER(uPgdIndex);

  if(uPortNum <= SB_MAXn(PGD_PORT_BLK_SIZEn))
  {
    SB_HWIO_OUTFI(hHandle,PGD_PORT_BLK_SIZEn,uPortNum,BLK_SIZE,uBlkSize);
  }
}


/**
 * Get the Block Size for a Port .
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 * 
 * @param[in] uPgdIndex - Ported Generic Device Index. 
 *  
 * @param[in] uPortNum -  Port number. 
 *  
 * @return Block size in units of bytes..
 *  
 * @sa HAL_sb_PgdPortSetBlkSize. 
 * 
 * @attention None.
 */
uint32 HAL_sb_PgdPortGetBlkSize(HAL_sb_HandleType hHandle, 
                                uint32 uPgdIndex, 
                                uint32 uPortNum)
{  
  uint32 uBlkSize = 0;

  UNREFERENCED_PARAMETER(uPgdIndex);

  if(uPortNum <= SB_MAXn(PGD_PORT_BLK_SIZEn))
  {
    uBlkSize = SB_HWIO_INFI(hHandle,PGD_PORT_BLK_SIZEn,uPortNum,BLK_SIZE);
  }
  return uBlkSize;
}


/**
 * Set the Transaction Size for a Port .
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 * 
 * @param[in] uPgdIndex - Ported Generic Device Index. 
 *  
 * @param[in] uPortNum -  Port number. 
 *  
 * @param[in] uTransSize -  Transaction size in units of bytes.
 *   
 * @return None.
 *  
 * @sa HAL_sb_PgdPortGetTransSize. 
 * 
 * @attention None.
 */
void HAL_sb_PgdPortSetTransSize(HAL_sb_HandleType hHandle, 
                                uint32 uPgdIndex, 
                                uint32 uPortNum, 
                                uint32 uTransSize)
{  
  UNREFERENCED_PARAMETER(uPgdIndex);

  if(uPortNum <= SB_MAXn(PGD_PORT_TRANS_SIZEn))
  {
    SB_HWIO_OUTFI(hHandle,PGD_PORT_TRANS_SIZEn,uPortNum,TRANS_SIZE,uTransSize);
  }
}


/**
 * Get the Transaction Size for a Port .
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 * 
 * @param[in] uPgdIndex - Ported Generic Device Index. 
 *  
 * @param[in] uPortNum -  Port number. 
 *  
 * @return Transaction size in units of bytes.
 *  
 * @sa HAL_sb_PgdPortSetTransSize. 
 * 
 * @attention None.
 */
uint32 HAL_sb_PgdPortGetTransSize(HAL_sb_HandleType hHandle, 
                                  uint32 uPgdIndex, 
                                  uint32 uPortNum)
{  
  uint32 uTransSize = 0;

  UNREFERENCED_PARAMETER(uPgdIndex);

  if(uPortNum < SB_MAXn(PGD_PORT_TRANS_SIZEn))
  {
    uTransSize = SB_HWIO_INFI(hHandle,PGD_PORT_TRANS_SIZEn,uPortNum,TRANS_SIZE);
  }
  return uTransSize;
}


/**
 * Set Multichannel Ports.
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 * 
 * @param[in] uPgdIndex - Ported Generic Device Index. 
 *  
 * @param[in] uPortBitField -  If bit n of this parameter is set, then 
 *       nth port channel is part of a multichannel group. For
 *       eg, if ports 0 to 5 participate in a 5:1 audio stream,
 *       then this parameter should be set to 0x3F
 *  
 * @return None.
 *  
 * @sa HAL_sb_PgdPortGettMultiChanl. 
 * 
 * @attention None.
 */
void HAL_sb_PgdPortSetMultiChanl(HAL_sb_HandleType hHandle, 
                                 uint32 uPgdIndex, 
                                 uint32 uPortBitField)
{
  uint32 uPort;
  uint32 uTemp = uPortBitField;
  uint32 uNumPorts = HAL_sb_PgdGetNumPorts(hHandle,uPgdIndex);
  for(uPort=0; uPort<uNumPorts; uPort++)
  {
    if(uTemp & 1)
    {
      SB_HWIO_OUTFI(hHandle,PGD_PORT_MULTI_CHNLn,uPort,PORTS,uPortBitField);
    }
    uTemp = uTemp>>1;
  }
}


/**
 * Get Multichannel Ports.
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 * 
 * @param[in] uPgdIndex - Ported Generic Device Index. 
 *  
 * @param[in] uPortNum -  One of the Ports which is part of this
 *       queried multichannel group
 *  
 * @return Port Bit Field, If bit n of this return value is 
 *       set, then nth port channel is part of a multichannel
 *       group. For eg, if ports 0 to 5 participate in a 5:1
 *       audio stream, then this value will be set to 0x3F
 *  
 * @sa HAL_sb_PgdPortSetMultiChanl. 
 * 
 * @attention None.
 */
uint32 HAL_sb_PgdPortGetMultiChanl(HAL_sb_HandleType hHandle, 
                                   uint32 uPgdIndex, 
                                   uint32 uPortNum)
{  
  uint32 uPortBitField = 0;
  if(uPortNum < HAL_sb_PgdGetNumPorts(hHandle,uPgdIndex))
  {
    uPortBitField = SB_HWIO_INFI(hHandle,PGD_PORT_MULTI_CHNLn,uPortNum,PORTS);
  }
  return uPortBitField;
}


/**
 * Set repeat_period used by the push/pull Transport Protocol 
 * hardware. 
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 * 
 * @param[in] uPgdIndex - Ported Generic Device Index. 
 *  
 * @param[in] uPortNum -  Port number.
 *  
 * @param[in] uRepeatPeriod -  Repeat period in units of TBD
 * 
 * @return None
 *  
 * @sa HAL_sb_PgdPortGetRptPeriodForPushPull. 
 * 
 * @attention None.
 */
void HAL_sb_PgdPortSetRptPeriodForPushPull(HAL_sb_HandleType hHandle, 
                                           uint32 uPgdIndex, 
                                           uint32 uPortNum, 
                                           uint32 uRepeatPeriod)
{  
  UNREFERENCED_PARAMETER(uPgdIndex);

  if(uPortNum <= SB_MAXn(PGD_PORT_PUSHPULLn))
  {
    SB_HWIO_OUTFI(hHandle,PGD_PORT_PUSHPULLn,uPortNum,RPT_PERIOD,uRepeatPeriod);
  }
}


/**
 * Get repeat_period used by the push/pull Transport Protocol 
 * hardware. 
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 * 
 * @param[in] uPgdIndex - Ported Generic Device Index. 
 *  
 * @param[in] uPortNum - Port number.
 *  
 * @return Repeat period in units of TBD.
 *  
 * @sa HAL_sb_PgdPortSetRptPeriodForPushPull. 
 * 
 * @attention None.
 */
uint32 HAL_sb_PgdPortGetRptPeriodForPushPull(HAL_sb_HandleType hHandle, 
                                             uint32 uPgdIndex, 
                                             uint32 uPortNum)
{
  uint32 uRepeatPeriod = 0;

  UNREFERENCED_PARAMETER(uPgdIndex);

  if(uPortNum <= SB_MAXn(PGD_PORT_PUSHPULLn))
  {
    uRepeatPeriod = SB_HWIO_INFI(hHandle,
                                 PGD_PORT_PUSHPULLn,
                                 uPortNum,
                                 RPT_PERIOD);
  }
  return uRepeatPeriod;
}


/**
 * Set num_samples used by the push/pull Transport Protocol 
 * hardware. 
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 * 
 * @param[in] uPgdIndex - Ported Generic Device Index. 
 *  
 * @param[in] uPortNum -  Port number.
 *  
 * @param[in] uNumSamples -  Number of samples
 * 
 * @return None
 *  
 * @sa HAL_sb_PgdPortGetNumSamplesForPushPull. 
 * 
 * @attention None.
 */
void HAL_sb_PgdPortSetNumSamplesForPushPull(HAL_sb_HandleType hHandle, 
                                            uint32 uPgdIndex, 
                                            uint32 uPortNum, 
                                            uint32 uNumSamples)
{  
  UNREFERENCED_PARAMETER(uPgdIndex);

  if(uPortNum <= SB_MAXn(PGD_PORT_PUSHPULLn))
  {
    SB_HWIO_OUTFI(hHandle,PGD_PORT_PUSHPULLn,uPortNum,NUM_SAMPLES,uNumSamples);
  }
}


/**
 * Get num_samples used by the push/pull Transport Protocol 
 * hardware. 
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 * 
 * @param[in] uPgdIndex - Ported Generic Device Index. 
 *  
 * @param[in] uPortNum -  Port number.
 *  
 * @return Number of samples.
 *  
 * @sa HAL_sb_PgdPortSetNumSamplesForPushPull. 
 * 
 * @attention None.
 */
uint32 HAL_sb_PgdPortGetNumSamplesForPushPull(HAL_sb_HandleType hHandle, 
                                              uint32 uPgdIndex, 
                                              uint32 uPortNum)
{
  uint32 uNumSamples = 0;

  UNREFERENCED_PARAMETER(uPgdIndex);

  if(uPortNum <= SB_MAXn(PGD_PORT_PUSHPULLn))
  {
    uNumSamples = SB_HWIO_INFI(hHandle,PGD_PORT_PUSHPULLn,uPortNum,NUM_SAMPLES);
  }
  return uNumSamples;
}


/**
 * Set Progress counter initial Value to be loaded. 
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 * 
 * @param[in] uPgdIndex - Ported Generic Device Index. 
 *  
 * @param[in] uPcNum -  Progress counter index.
 *  
 * @param[in] uInitVal -  PC init value.
 *  
 * @return None.
 *  
 * @sa HAL_sb_PgdPcGetInitVal, HAL_sb_PgdPcLoadInitVal. 
 * 
 * @attention This initial value will be loaded in PC only after
 *            calling HAL_sb_PgdPcLoadInitVal() api.
 */
void HAL_sb_PgdPcSetInitVal(HAL_sb_HandleType hHandle, 
                            uint32 uPgdIndex, 
                            uint32 uPcNum, 
                            uint32 uInitVal)
{  
  UNREFERENCED_PARAMETER(uPgdIndex);

  uPcNum = SB_PC_GET_ADJUSTED_INDEX(hHandle, uPcNum);
  if(SB_MAXn(PGD_PC_CFGn) >= uPcNum)
  {
    SB_HWIO_OUTFI(hHandle,PGD_PC_CFGn,uPcNum,INIT_VAL,uInitVal);
  }
}


/**
 * Get Progress counter initial Value to be loaded. 
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 * 
 * @param[in] uPgdIndex - Ported Generic Device Index. 
 *  
 * @param[in] uPcNum -  Progress counter index.
 *  
 * @return PC init value.
 *  
 * @sa HAL_sb_PgdPcSetInitVal, HAL_sb_PgdPcLoadInitVal. 
 * 
 * @attention This initial value will be loaded in PC only after
 *            calling HAL_sb_PgdPcLoadInitVal() api.
 */
uint32 HAL_sb_PgdPcGetInitVal(HAL_sb_HandleType hHandle,
                              uint32 uPgdIndex,
                              uint32 uPcNum)
{
  uint32 uInitVal = 0;

  UNREFERENCED_PARAMETER(uPgdIndex);

  uPcNum = SB_PC_GET_ADJUSTED_INDEX(hHandle, uPcNum);
  if(SB_MAXn(PGD_PC_CFGn) >= uPcNum)
  {
    uInitVal =  SB_HWIO_INFI(hHandle,PGD_PC_CFGn,uPcNum,INIT_VAL);
  }
  return uInitVal;
}


/**
 * Set the Progress counter association with a port. 
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 * 
 * @param[in] uPgdIndex - Ported Generic Device Index. 
 *  
 * @param[in] uPcNum -  Progress counter index. 
 *  
 * @param[in] uPortNum -  Port number to associate PC with. 
 *  
 * @return None.
 *  
 * @sa HAL_sb_PgdPcGetPortNum. 
 * 
 * @attention None.
 */
void HAL_sb_PgdPcSetPortNum(HAL_sb_HandleType hHandle, 
                            uint32 uPgdIndex, 
                            uint32 uPcNum, 
                            uint32 uPortNum)
{
  uint32 uHwVersion = SB_HWIO_IN(hHandle,COMP_HW_VERSION);
  
  UNREFERENCED_PARAMETER(uPgdIndex);

  if(uHwVersion < 0x01050000)  /* PORT_SEL exists only before v1.5 */
  {
    uPcNum = SB_PC_GET_ADJUSTED_INDEX2(hHandle, uPcNum, uHwVersion);
    if(SB_MAXn(PGD_PC_CFGn) >= uPcNum)
    {
      SB_HWIO_OUTFI(hHandle,PGD_PC_CFGn,uPcNum,_V1_PORT_SEL,uPortNum);
    }
  }
}


/**
 * Get the Progress counter association with a port. 
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 * 
 * @param[in] uPgdIndex - Ported Generic Device Index. 
 *  
 * @param[in] uPcNum -  Progress counter index.
 *  
 * @return Port number associated to a PC instance.
 *  
 * @sa HAL_sb_PgdPcSetPortNum. 
 * 
 * @attention None.
 */
uint32 HAL_sb_PgdPcGetPortNum(HAL_sb_HandleType hHandle, 
                              uint32 uPgdIndex, 
                              uint32 uPcNum)
{
  uint32 uHwVersion = SB_HWIO_IN(hHandle,COMP_HW_VERSION);
  uint32 uPortNum = 0;
  
  UNREFERENCED_PARAMETER(uPgdIndex);

  if(uHwVersion < 0x01050000)  /* PORT_SEL exists only before v1.5 */
  {
    uPcNum = SB_PC_GET_ADJUSTED_INDEX2(hHandle, uPcNum, uHwVersion);
    if(SB_MAXn(PGD_PC_CFGn) >= uPcNum)
    {
      uPortNum =  SB_HWIO_INFI(hHandle,PGD_PC_CFGn,uPcNum,_V1_PORT_SEL);
    }
  }
  else
  {
    uPortNum = uPcNum;
  }
  return uPortNum;
}


/**
 * Load the Init value to the Progress Counter now. 
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 * 
 * @param[in] uPgdIndex - Ported Generic Device Index. 
 *  
 * @param[in] uPcNum -  Progress counter index.
 *  
 * @return None.
 *  
 * @sa HAL_sb_PgdPcSetInitVal,HAL_sb_PgdPcGetInitVal. 
 * 
 * @attention None.
 */
void HAL_sb_PgdPcLoadInitVal(HAL_sb_HandleType hHandle, 
                             uint32 uPgdIndex, 
                             uint32 uPcNum)
{  
  UNREFERENCED_PARAMETER(uPgdIndex);

  uPcNum = SB_PC_GET_ADJUSTED_INDEX(hHandle, uPcNum);
  if(SB_MAXn(PGD_PC_CFGn) >= uPcNum)
  {
    SB_HWIO_OUTFI(hHandle,PGD_PC_CFGn,uPcNum,INIT,(uint32)0x1);
  }
}


/**
 * Set the Init value of the Progress Counter and load the 
 * Progress counter now. This is combination of 
 * HAL_sb_PgdPcSetInitVal and HAL_sb_PgdPcLoadInitVal apis.
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 * 
 * @param[in] uPgdIndex - Ported Generic Device Index. 
 *  
 * @param[in] uPcNum -  Progress counter index. 
 *  
 * @param[in] uInitVal -  PC init value. 
 *  
 * @return None.
 *  
 * @sa HAL_sb_PgdPcSetInitVal,HAL_sb_PgdPcGetInitVal, HAL_sb_PgdPcLoadInitVal.
 * 
 * @attention None.
 */
void HAL_sb_PgdPcSetAndLoadInitVal(HAL_sb_HandleType hHandle, 
                                   uint32 uPgdIndex, 
                                   uint32 uPcNum, 
                                   uint32 uInitVal)
{
  HAL_sb_PgdPcSetInitVal(hHandle, uPgdIndex, uPcNum, uInitVal);
  HAL_sb_PgdPcLoadInitVal(hHandle, uPgdIndex, uPcNum);
}


/**
 * Get the Progress counter value 
 * Counting number of words DMA-ed to (if src) or from (if sink) from the 
 * associated port. 
 * And number of samples in the FIFO.  
 *
 * @param[in] hHandle - This SlimBus hHandle. 
 * 
 * @param[in] uPgdIndex - Ported Generic Device Index. 
 *  
 * @param[in] uPcNum -  Progress counter index.
 *  
 * @param[out] puWordsDMAed -  Number of words DMA-ed to (if src) or
 * from (if sink) from the associated port
 *  
 * @param[out] puNumSamplesinFifo -  Number of samples in the FIFO.  
 *  
 * @return None.
 *  
 * @sa None. 
 * 
 * @attention None.
 */
void HAL_sb_PgdPcGetVal(HAL_sb_HandleType hHandle, 
                        uint32 uPgdIndex, 
                        uint32 uPcNum,
                        uint32 *puWordsDMAed,
                        uint32 *puNumSamplesinFifo)
{
  uint32 uPcVal = 0;

  UNREFERENCED_PARAMETER(uPgdIndex);

  if((NULL != puWordsDMAed) && (NULL != puNumSamplesinFifo))
  {
    *puWordsDMAed = 0;
    *puNumSamplesinFifo = 0;
    uPcNum = SB_PC_GET_ADJUSTED_INDEX(hHandle, uPcNum);
    if(SB_MAXn(PGD_PC_VALn) >= uPcNum)
    {
      uPcVal = SB_HWIO_INI(hHandle,PGD_PC_VALn,uPcNum);
      *puWordsDMAed = (uPcVal & SB_BSMSK(PGD_PC_VALn,PC_DMA)) >> 
                       SB_SHFT(PGD_PC_VALn,PC_DMA);
      *puNumSamplesinFifo = (uPcVal & SB_BSMSK(PGD_PC_VALn,PC_FIFO_SAMPLES)) >>
                             SB_SHFT(PGD_PC_VALn,PC_FIFO_SAMPLES);
    }
  }
}


/**
 * Get the Progress counter value when the VFR TS interrupt 
 * occured. 
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 * 
 * @param[in] uPgdIndex - Ported Generic Device Index. 
 *  
 * @param[in] uPcNum -  Progress counter index.
 *  
 * @param[out] puWordsDMAed -  Number of words DMA-ed to (if src) or 
 * from (if sink) from the associated port
 *  
 * @param[out] puNumSamplesinFifo -  Number of samples in the FIFO.  
 *  
 * @return None.
 *  
 * @sa None. 
 * 
 * @attention None.
 */
void HAL_sb_PgdPcGetValOnVfrTs(HAL_sb_HandleType hHandle, 
                                 uint32 uPgdIndex, 
                                 uint32 uPcNum,
                                 uint32 *puWordsDMAed,
                                 uint32 *puNumSamplesinFifo)
{
  uint32 uPcVal = 0;

  UNREFERENCED_PARAMETER(uPgdIndex);

  if((NULL != puWordsDMAed) && (NULL != puNumSamplesinFifo))
  {
    *puWordsDMAed = 0;
    *puNumSamplesinFifo = 0;
    uPcNum = SB_PC_GET_ADJUSTED_INDEX(hHandle, uPcNum);
    if(SB_MAXn(PGD_PC_VFR_TSn) >= uPcNum)
    {
      uPcVal = SB_HWIO_INI(hHandle,PGD_PC_VFR_TSn,uPcNum);
      *puWordsDMAed = (uPcVal & SB_BSMSK(PGD_PC_VALn,PC_DMA)) >> 
                       SB_SHFT(PGD_PC_VALn,PC_DMA);
      *puNumSamplesinFifo = (uPcVal & SB_BSMSK(PGD_PC_VALn,PC_FIFO_SAMPLES)) >> 
                             SB_SHFT(PGD_PC_VALn,PC_FIFO_SAMPLES);
    }
  }
}


/**
 * Get the shadow progress counter value, counting number of 
 * words DMA-ed to (if src) or from (if sink) from the 
 * associated port, and number of samples in the FIFO. 
 *
 * @param[in] hHandle - This SlimBus hHandle. 
 * 
 * @param[in] uPgdIndex - Ported Generic Device Index. 
 *  
 * @param[in] uPcNum -  Progress counter index.
 *  
 * @param[out] puWordsDMAed -  Number of words DMA-ed to (if src) or
 * from (if sink) from the associated port
 *  
 * @param[out] puNumSamplesinFifo -  Number of samples in the FIFO.  
 *  
 * @return None.
 *  
 * @sa None. 
 * 
 * @attention None.
 */
void HAL_sb_PgdPcGetShadowVal(HAL_sb_HandleType hHandle, 
                              uint32 uPgdIndex, 
                              uint32 uPcNum,
                              uint32 *puWordsDMAed,
                              uint32 *puNumSamplesinFifo)
{
  uint32 uPcVal = 0;

  UNREFERENCED_PARAMETER(uPgdIndex);

  if((NULL != puWordsDMAed) && (NULL != puNumSamplesinFifo))
  {
    *puWordsDMAed = 0;
    *puNumSamplesinFifo = 0;
    uPcNum = SB_PC_GET_ADJUSTED_INDEX(hHandle, uPcNum);
    if(SB_MAXn(PGD_PC_VAL_SHADOWn) >= uPcNum)
    {
      uPcVal = SB_HWIO_INI(hHandle,PGD_PC_VAL_SHADOWn,uPcNum);
      *puWordsDMAed = (uPcVal & SB_BSMSK(PGD_PC_VALn,PC_DMA)) >> 
                       SB_SHFT(PGD_PC_VALn,PC_DMA);
      *puNumSamplesinFifo = (uPcVal & SB_BSMSK(PGD_PC_VALn,PC_FIFO_SAMPLES)) >> 
                             SB_SHFT(PGD_PC_VALn,PC_FIFO_SAMPLES);
    }
  }
}


/**
 * Check if VFR TS Interrupt is set for a PC. 
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 * 
 * @param[in] uPgdIndex - Ported Generic Device Index. 
 *  
 * @param[in] uPcNum -  Progress counter index.
 *  
 * @return TRUE, if the VFR TS interrupt is set, FALSE 
 *         otherwise.
 *  
 * @sa HAL_sb_PgdPcClearVfrInt. 
 * 
 * @attention None.
 */
bool32 HAL_sb_PgdPcIsVfrIntSet(HAL_sb_HandleType hHandle, 
                               uint32 uPgdIndex, 
                               uint32 uPcNum)
{
  UNREFERENCED_PARAMETER(uPgdIndex);

  uPcNum = SB_PC_GET_ADJUSTED_INDEX(hHandle, uPcNum);
  if(SB_MAXn(PGD_PC_VFR_TS_STATUSn) >= uPcNum)
  {
    if(0 != SB_HWIO_INFI(hHandle,PGD_PC_VFR_TS_STATUSn,uPcNum,VFR_INT))
    {
      return TRUE;
    }
  }
    return FALSE;
}


/**
 * Clear VFR TS Interrupt if set for a PC. 
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 * 
 * @param[in] uPgdIndex - Ported Generic Device Index. 
 *  
 * @param[in] uPcNum -  Progress counter index.
 *  
 * @return None.
 *  
 * @sa HAL_sb_PgdPcIsVfrIntSet. 
 * 
 * @attention None.
 */
void HAL_sb_PgdPcClearVfrInt(HAL_sb_HandleType hHandle, 
                             uint32 uPgdIndex, 
                             uint32 uPcNum)
{
  UNREFERENCED_PARAMETER(uPgdIndex);

  uPcNum = SB_PC_GET_ADJUSTED_INDEX(hHandle, uPcNum);
  if(SB_MAXn(PGD_PC_VFR_TS_STATUSn) >= uPcNum)
  {
    SB_HWIO_OUTFI(hHandle,PGD_PC_VFR_TS_CLRn,uPcNum,VFR_INT,1);    
  }
}


/**
 * Get the progress counter timestamp value corresponding to the
 * last DMA interrupt, as well as indication if the timestamp is
 * valid or if samples were missed.
 *
 * @param[in] hHandle - This SlimBus hHandle. 
 * 
 * @param[in] uPgdIndex - Ported Generic Device Index. 
 *  
 * @param[in] uPcNum -  Progress counter index.
 *  
 * @param[out] puTimestamp -  Timetstamp value at the last 
 * DMA interrupt
 *  
 * @param[out] pbTimestampValid -  If the timestamp is valid or 
 *       not.
 *  
 * @param[out] pbSampleMissed -  If there was a timestamp sample
 *       that was missed.
 *  
 * @return None.
 *  
 * @sa None. 
 * 
 * @attention None.
 */
void HAL_sb_PgdPcGetTimestamp(HAL_sb_HandleType hHandle, 
                              uint32 uPgdIndex, 
                              uint32 uPcNum,
                              uint64 *puTimestamp,
                              bool32 *pbTimestampValid,
                              bool32 *pbSampleMissed)
{
  uint32 uPcTsLsb = 0, uPcTsMsb = 0;

  UNREFERENCED_PARAMETER(uPgdIndex);

  if( NULL != puTimestamp &&
      NULL != pbTimestampValid &&
      NULL != pbSampleMissed )
  {
    *pbTimestampValid = FALSE;
    uPcNum = SB_PC_GET_ADJUSTED_INDEX(hHandle, uPcNum);
    if(SB_MAXn(PGD_PC_TIMESTAMP_LSBn) >= uPcNum)
    {
      uPcTsLsb = SB_HWIO_INI(hHandle,PGD_PC_TIMESTAMP_LSBn,uPcNum);
      uPcTsMsb = SB_HWIO_INI(hHandle,PGD_PC_TIMESTAMP_MSBn,uPcNum);
      *puTimestamp = (uint64)(uPcTsLsb) | ((uint64)((uPcTsMsb) & SB_BSMSK(PGD_PC_TIMESTAMP_MSBn,PC_TIMESTAMP_MSB) >> SB_SHFT(PGD_PC_TIMESTAMP_MSBn,PC_TIMESTAMP_MSB)) << 32);
      *pbTimestampValid = (uPcTsMsb & SB_BSMSK(PGD_PC_TIMESTAMP_MSBn,PC_TIMESTAMP_SAMPLE_VALID)) != 0;
      *pbSampleMissed = (uPcTsMsb & SB_BSMSK(PGD_PC_TIMESTAMP_MSBn,PC_TIMESTAMP_SAMPLE_MISSED)) != 0;
    }
  }
}

/**
 * Check if the Frame Sync Shift feature is available for this 
 * chip or not.
 * 
 * @param[in] hHandle - This SlimBus hHandle.
 * 
 * @return True/False 
 */
bool32 HAL_sb_IsFrameSyncShiftSupported(HAL_sb_HandleType hHandle)
{
  uint32 HwVersion = HAL_sb_CompGetHwVersion(hHandle);
  
  if (HwVersion >= ISTARI_HW_VERSION)
  {
    return TRUE;
  } 
  
  return FALSE;
}

/**
 * Checks if scheduling a data channel starting on slot 0 on a 
 * secondary data line is supported for this chip or not. 
 * 
 * @param[in] hHandle - This SlimBus hHandle.
 * 
 * @return True/False 
 */
bool32 HAL_sb_IsSecondaryDataLineSlot0Supported(HAL_sb_HandleType hHandle)
{
  uint32 HwVersion = HAL_sb_CompGetHwVersion(hHandle);

  if (HwVersion >= NAZGUL_HW_VERSION)
  {
    return TRUE;
  } 

  return FALSE;
}

/**
 * Enable the Hardware Frame Sync Shift Support
 * 
 * @param[in] hHandle - This SlimBus hHandle.
 * 
 */
void HAL_sb_CompEnablePhaseShift(HAL_sb_HandleType hHandle)
{
  SB_HWIO_OUTF(hHandle, COMP_FEATURE_CFG, ENABLE_PHASE_SHIFT, 1);
}

/**
 * Disable the Hardware Frame Sync Shift Support
 * 
 * @param[in] hHandle - This SlimBus hHandle.
 * 
 */

void HAL_sb_CompDisablePhaseShift(HAL_sb_HandleType hHandle)
{
  SB_HWIO_OUTF(hHandle, COMP_FEATURE_CFG, ENABLE_PHASE_SHIFT, 0);
}

/**
 * Initialize retention configuration for the component.
 * 
 * @param[in] hHandle - This SlimBus hHandle.
 * 
 * @return None 
 * 
 * @attention 
 *   None.
 */
void HAL_sb_CompInitRetention(HAL_sb_HandleType hHandle)
{
  SB_HWIO_OUTF(hHandle, COMP_CFG, INT_FRM_CGC_IDLE_LOW, 1);
}

/**
 * Enable retention for the component.
 * 
 * @param[in] hHandle - This SlimBus hHandle.
 * 
 * @return None 
 * 
 * @attention 
 *   None.
 */
void HAL_sb_CompEnableRetention(HAL_sb_HandleType hHandle)
{
  SB_HWIO_OUTF(hHandle, COMP_CFG, RET_EN, 1);
  SB_HWIO_OUTF(hHandle, COMP_CFG, SELECT_RET_TO_NEGEDGE, 1);
}

/**
 * Disable retention for the component.
 * 
 * @param[in] hHandle - This SlimBus hHandle.
 * 
 * @return None 
 * 
 * @attention 
 *   None.
 */
void HAL_sb_CompDisableRetention(HAL_sb_HandleType hHandle)
{
  SB_HWIO_OUTF(hHandle, COMP_CFG, RET_EN, 0);
  SB_HWIO_OUTF(hHandle, COMP_CFG, SELECT_RET_TO_NEGEDGE, 0);
}

/**
 * Checks if retention is enabled for the component or not. 
 * 
 * @param[in] hHandle - This SlimBus hHandle.
 * 
 * @return True/False 
 * 
 * @attention 
 *   None.
 */
bool32 HAL_sb_CompIsRetentionEnabled(HAL_sb_HandleType hHandle)
{
  if(SB_HWIO_INF(hHandle, COMP_CFG, RET_EN) != 0)
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}


/** @} */
