/*=============================================================================

  File: HALdsi.c
  

  Copyright (c) 2010-2016 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/

/*============================================================================
*                         INCLUDE FILES
============================================================================*/
#include "HALdsi.h"

/* --- DO NOT ADD HEADER FILES HERE! --- */

#ifdef __cplusplus
extern "C" {
#endif

/* ------------------------------------------------------------------------
Defines types
---------------------------------------------------------------------------*/

#define SwapEndian(in, out)   out = ( in>>24)    |  \
                                    ((in<<8) & 0x00FF0000) |  \
                                    ((in>>8) & 0x0000FF00) |  \
                                    ( in<<24);

/* DSI high speed timeout value(need to be large enough to accommodate more than a frame) */
#define TIMER_TIMEOUT_IN_MS    50

/* DSI High speed Timeout Max count value  */
#define DSI_TIMEOUT_MAX_COUNT  0xffff


/* -----------------------------------------------------------------------
** Local enumerated types
** -----------------------------------------------------------------------*/


/* -----------------------------------------------------------------------
** Global variables
** -----------------------------------------------------------------------*/

HAL_dsi_StatusType geDSIErrorCode = HAL_DSI_SUCCESS;

/* -----------------------------------------------------------------------
** Local macros
** -----------------------------------------------------------------------*/


/* -----------------------------------------------------------------------
** Local functions
** ----------------------------------------------------------------------- */

/* Calculates steps and count for timer timeout in terms of escape clock.
 */
static void HAL_DSI_CalculateTimeout(uint32    uTimeinMS, 
                                     uint32   *pTimeoutCount, 
                                     uint32   *pTimeoutSteps);

/* ------------------------------------------------------------------------
Functions
---------------------------------------------------------------------------*/


/****************************************************************************
*
** FUNCTION: HAL_DSI_GetRegBaseOffset()
*/
/*!
* \brief   
*     Get DSI register address offset for a give DSI device ID.
*
* \param [in]  eDeviceId - DSI core ID
*
* \retval - register base offset
*
****************************************************************************/
uintPtr HAL_DSI_GetRegBaseOffset(DSI_Device_IDType  eDeviceId)
{
  uintPtr   uOffset;

  switch (eDeviceId)
  {
    case DSI_DeviceID_0:
    {
      uOffset = 0;
      break;
    }

    case DSI_DeviceID_1:
    {
      uOffset = DSI_1_DSI_REG_BASE - DSI_0_DSI_REG_BASE;
      break;
    }

    default:
    {
      //No hardware support for other DSI device IDs
      uOffset        = 0;
      geDSIErrorCode = HAL_DSI_FAILED_NO_HW_SUPPORT;
      break;
    }
  }

  return uOffset;
}



/****************************************************************************
*
** FUNCTION: HAL_DSI_Init()
*/
/*!
* \brief
*   Initialize all DSI hardware blocks after a reset (hardware or software)
*   or at system power up. This function is called once prior to all other
*   MDP HAL functions with the exception of HAL_DSI_Reset.
*   Identical to calling HAL_DSI_InitDevice(HAL_DSI_ DEVICEID _ALL).
*
* \param [in]  eDeviceID  - DSI core device ID.
*
* \retval void
*
****************************************************************************/
void HAL_DSI_Init(DSI_Device_IDType  eDeviceID)
{
  uintPtr uOffset            = HAL_DSI_GetRegBaseOffset(eDeviceID);

  /* Disable DSI Controller, DSI lane states, DSI command-mode and DSI video-mode engines */
  out_dword(uOffset + HWIO_DSI_0_CTRL_ADDR, 0);

  /* Reset the DSI controller */
  HAL_DSI_Reset(eDeviceID);

  /* Disable and clear any pending interrupts */         
  HAL_DSI_SetInterruptsEnabled(eDeviceID, HAL_DSI_INTERRUPT_NONE);
  HAL_DSI_ClrInterruptStatus  (eDeviceID, HAL_DSI_INTERRUPT_ALL);

}

/****************************************************************************
*
** FUNCTION: HAL_DSI_Reset()
*/
/*!
* \brief
*   Use the DSI hardware reset mechanism to reset all DSI hardware blocks 
*   with any available hardware reset mechanism present.  If a global reset 
*   is not available then the individual available blocks will be reset.  
*   The power and clock sources for the DSI hardware are not interrupted.
*
* \param [in]  eDeviceID  - DSI core device ID.
*
* \retval void
*
****************************************************************************/
void HAL_DSI_Reset(DSI_Device_IDType        eDeviceID)
{
  uintPtr   uOffset          = HAL_DSI_GetRegBaseOffset(eDeviceID);
  uint32    uDsiTimeoutCount = 0;
  uint32    uDsiTimeoutSteps = 0;
  uint32    uDsiHSTimerReg;

  /*Reset DSI HW */
  out_dword(uOffset + HWIO_DSI_0_SOFT_RESET_ADDR, HWIO_DSI_0_SOFT_RESET_DSI_SOFT_RESET_BMSK);

  /* Clear Reset Bit */
  out_dword(uOffset + HWIO_DSI_0_SOFT_RESET_ADDR, 0);

  //* Set the High Speed Timer steps and count to account for an absolute timer value */
  HAL_DSI_CalculateTimeout(TIMER_TIMEOUT_IN_MS, &uDsiTimeoutCount, &uDsiTimeoutSteps);
  uDsiHSTimerReg = in_dword(uOffset + HWIO_DSI_0_HS_TIMER_CTRL_ADDR);
  uDsiHSTimerReg = HWIO_OUT_FLD( uDsiHSTimerReg, DSI_0, HS_TIMER_CTRL_TIMER_RESOLUTION, uDsiTimeoutSteps);
  uDsiHSTimerReg = HWIO_OUT_FLD( uDsiHSTimerReg, DSI_0, HS_TIMER_CTRL_HS_TX_TO,         uDsiTimeoutCount);
  out_dword(uOffset + HWIO_DSI_0_HS_TIMER_CTRL_ADDR, uDsiHSTimerReg);

  /* Reset TPG FIFO*/
  out_dword(uOffset + HWIO_DSI_0_TPG_DMA_FIFO_RESET_ADDR, HWIO_DSI_0_TPG_DMA_FIFO_RESET_FIFO_RESET_BMSK); 
  out_dword(uOffset + HWIO_DSI_0_TPG_DMA_FIFO_RESET_ADDR, 0); 

}

/****************************************************************************
*
** FUNCTION: HAL_DSI_Save()
*/
/*!
* \brief
*   Function has no effect and is reserved for future use.
*
* \retval void
*
****************************************************************************/
void HAL_DSI_Save(void)
{
  /* Do nothing here for now except provide the APIs which do nothing.
     Eventually, we would like to be able to save/restore state for the
     DSI HW when the power is turned off. */
  return;
}



/****************************************************************************
*
** FUNCTION: HAL_DSI_Restore()
*/
/*!
* \brief
*   Function has no effect and is reserved for future use.
*
* \retval void
*
****************************************************************************/
void HAL_DSI_Restore(void)
{
  /* Do nothing here for now except provide the APIs which do nothing.
     Eventually, we would like to be able to save/restore state for the
     DSI HW when the power is turned off. */
  return;
}



/****************************************************************************
*
** FUNCTION: HAL_DSI_GetLaneStatus()
*/
/*!
* \brief
*    Query the lane status information for a particular DSI device.
*
* \param [in]  eDeviceID    - DSI core device ID 
* \param [out] puDsiStatus  - Device lane status information is returned
*
* \retval void
*
****************************************************************************/
void HAL_DSI_GetLaneStatus(DSI_Device_IDType   eDeviceID,
                           uint32             *puDsiStatus)
{
  uintPtr    uOffset  = HAL_DSI_GetRegBaseOffset(eDeviceID);

  /* Valid parameters */
  if (NULL == puDsiStatus)
  {
    geDSIErrorCode = HAL_DSI_FAILED_INVALID_INPUT_PARAMETER;    
  }
  else
  {
    uint32 uDsiHWLaneStatusReg  = 0;

    *puDsiStatus   =  0;

    /* Get the HW generated DSI Status. */
    uDsiHWLaneStatusReg = in_dword(uOffset + HWIO_DSI_0_LANE_STATUS_ADDR);
    
    /* Data lane 0 direction */
    if (0 != HWIO_GETVAL(DSI_0_LANE_STATUS, uDsiHWLaneStatusReg, DLN0_DIRECTION))
    {
      *puDsiStatus |= DSI_LANE_STATUS_DLN0_DIRECTION;
    }
  }
}



/****************************************************************************
*
** FUNCTION: HAL_DSI_GetStatus()
*/
/*!
* \brief
*   Query if DSI hardware blocks are in use.  This function only determines if
*   the hardware block is active, periodic hardware will only appear in use when
*   doing work.  This function can be used to poll for completion of hardware.
*   Bit will be 1 if the corresponding HW block are busy.
*
* \param [in]  eDeviceID    - DSI core device ID 
* \param [out] puDsiStatus  - Bitwise fields defined in the HAL_DSI_StatusType.
*
* \retval void
*
****************************************************************************/
void HAL_DSI_GetStatus( DSI_Device_IDType   eDeviceID,
                        uint32             *puDsiStatus)
{
  uint32  uDsiHWStatusReg  = 0;
  uint32  uDsiStatus       = 0;
  uintPtr uOffset          = HAL_DSI_GetRegBaseOffset(eDeviceID);

  /* Validate input parameters */
  if (NULL == puDsiStatus)
  {
    geDSIErrorCode = HAL_DSI_FAILED_INVALID_INPUT_PARAMETER;
  }
  else
  {
    *puDsiStatus = 0;

    /* Get the HW generated DSI Status. */
    uDsiHWStatusReg = in_dword(uOffset + HWIO_DSI_0_STATUS_ADDR);

    /* Command-mode engine busy */
    if (0 != HWIO_GETVAL(DSI_0_STATUS, uDsiHWStatusReg, CMD_MODE_ENGINE_BUSY))  
    {
      uDsiStatus |= DSI_STATUS_DEVICE_BUSY_CMDENGINE;
    }

    /* Command-mode engine DMA busy */
    if (0 != HWIO_GETVAL(DSI_0_STATUS, uDsiHWStatusReg, CMD_MODE_DMA_BUSY))  
    {
      uDsiStatus |= DSI_STATUS_DEVICE_BUSY_CMDENGINEDMA;
    }

    /* Command-mode engine MDP busy */
    if (0 != HWIO_GETVAL(DSI_0_STATUS, uDsiHWStatusReg, CMD_MODE_MDP_BUSY))  
    {
      uDsiStatus |= DSI_STATUS_DEVICE_BUSY_CMDENGINEMDP;
    }

    /* Video-mode engine busy */
    if (0 != HWIO_GETVAL(DSI_0_STATUS, uDsiHWStatusReg, VIDEO_MODE_ENGINE_BUSY))  
    {
      uDsiStatus |= DSI_STATUS_DEVICE_BUSY_VIDEOENGINE;
    }

    /* DSI is waiting for response from panel */
    if (0 != HWIO_GETVAL(DSI_0_STATUS, uDsiHWStatusReg, BTA_BUSY))  
    {
      uDsiStatus |= DSI_STATUS_DEVICE_BUSY_BTA;
    }

    /* DSI is sending generic trigger */
    if (0 != HWIO_GETVAL(DSI_0_STATUS, uDsiHWStatusReg, GENERIC_TRIGGER_BUSY))  
    {
      uDsiStatus |= DSI_STATUS_DEVICE_BUSY_GENERICTRIG;
    }

    /* DSI is sending panel reset*/
    if (0 != HWIO_GETVAL(DSI_0_STATUS, uDsiHWStatusReg, PHY_RESET_BUSY))  
    {
      uDsiStatus |= DSI_STATUS_DEVICE_BUSY_PHYRESET;
    }

    *puDsiStatus = uDsiStatus;
  }
}



/****************************************************************************
*
** FUNCTION: HAL_DSI_ClockConfig()
*/
/*!
* \brief
*   Turn on/off the DSI internal clock according to the bitwise field.
*
* \param [in] eDeviceID     - DSI core device ID 
* \param [in] uClockConfig  - Bitwise fields defined in HAL_DSI_ClockIDType                               
*
* \retval void
*
****************************************************************************/
void HAL_DSI_ClockConfig(DSI_Device_IDType    eDeviceID,
                         uint32               uClockConfig)
{
  /* Default to all clock off */
  uint32  uClkCtrlReg  = 0;
  uintPtr uOffset      = HAL_DSI_GetRegBaseOffset(eDeviceID);

  /* Clock for AHI Bus Master, for DMA out from memory */
  if (0 != (uClockConfig & HAL_DSI_Clock_AHBM_SCLK ))
  {
    uClkCtrlReg = HWIO_OUT_FLD( uClkCtrlReg, DSI_0_CLK_CTRL, DSI_AHBM_SCLK_ON, 1);
    uClkCtrlReg = HWIO_OUT_FLD( uClkCtrlReg, DSI_0_CLK_CTRL, DSI_FORCE_ON_DYN_AHBM_HCLK, 1);
  }

  /* Clock for MDP/DSI, for DMA out from MDP          */
  if (0 != (uClockConfig & HAL_DSI_Clock_PCLK))
  {
    uClkCtrlReg = HWIO_OUT_FLD( uClkCtrlReg, DSI_0_CLK_CTRL, DSI_PCLK_ON, 1);
  }
  
  /* Clock for rest of DSI                             */
  if (0 != (uClockConfig & HAL_DSI_Clock_MAINCLK))
  {
    uClkCtrlReg = HWIO_OUT_FLD( uClkCtrlReg, DSI_0_CLK_CTRL, DSI_AHBS_HCLK_ON, 1);
    uClkCtrlReg = HWIO_OUT_FLD( uClkCtrlReg, DSI_0_CLK_CTRL, DSI_DSICLK_ON, 1);

    uClkCtrlReg = HWIO_OUT_FLD( uClkCtrlReg, DSI_0_CLK_CTRL, DSI_BYTECLK_ON, 1);
    uClkCtrlReg = HWIO_OUT_FLD( uClkCtrlReg, DSI_0_CLK_CTRL, DSI_ESCCLK_ON, 1);
  }

  /* TODO, Unused register bits, to find out use case for those 
  uClkCtrlReg  |= HWIO_DSI_0_CLK_CTRL_DSI_FORCE_ON_DYN_AHBS_HCLK_BMSK;
  uClkCtrlReg  |= HWIO_DSI_0_CLK_CTRL_DSI_FORCE_ON_DYN_DSICLK_BMSK;
  uClkCtrlReg  |= HWIO_DSI_0_CLK_CTRL_DSI_FORCE_ON_DYN_BYTECLK_BMSK;  
  uClkCtrlReg  |= HWIO_DSI_0_CLK_CTRL_DSI_AHBM_HCLK_HYSTERISIS1_CTRL_BMSK;
  uClkCtrlReg  |= HWIO_DSI_0_CLK_CTRL_DSI_AHBS_HCLK_HYSTERISIS1_CTRL_BMSK;
  uClkCtrlReg  |= HWIO_DSI_0_CLK_CTRL_DSI_DSICLK_HYSTERISIS1_CTRL_BMSK;
  */

  out_dword(uOffset + HWIO_DSI_0_CLK_CTRL_ADDR, uClkCtrlReg);

}

 

/****************************************************************************
*
** FUNCTION: HAL_DSI_Enable_CTRL()
*/
/*!
* \brief
*   Enables the HW blocks in each of DSI controller.
*
* \param [in] eDeviceID  - DSI core device ID.
* \param [in] eHWBlock   - DSI hardware block that needs to be enabled. 
*
* \retval void
*
****************************************************************************/
void HAL_DSI_Enable_CTRL(DSI_Device_IDType    eDeviceID,
                         HAL_DSI_HWBlockType  eHWBlock )
{
  uintPtr uOffset  = HAL_DSI_GetRegBaseOffset(eDeviceID);
  uint32  uRegVal  = in_dword(uOffset + HWIO_DSI_0_CTRL_ADDR);

  if (HAL_DSI_HWBLOCK_CONTROLLER  == eHWBlock)
  {
    uRegVal = HWIO_OUT_FLD( uRegVal, DSI_0_CTRL, DSI_EN, 1);
  }

  if (HAL_DSI_HWBLOCK_VIDEOENGINE  == eHWBlock)
  {
    uRegVal = HWIO_OUT_FLD( uRegVal, DSI_0_CTRL, VIDEO_MODE_EN, 1);
  }

  if (HAL_DSI_HWBLOCK_CMDMODEENGINE  == eHWBlock)
  {
    uRegVal = HWIO_OUT_FLD( uRegVal, DSI_0_CTRL, CMD_MODE_EN, 1);
  }

  out_dword(uOffset + HWIO_DSI_0_CTRL_ADDR, uRegVal);
}

 

/****************************************************************************
*
** FUNCTION: HAL_DSI_Disable_CTRL()
*/
/*!
* \brief
*   Disables the HW blocks in each of DSI controller.
*
* \param [in] eDeviceID  - DSI core device ID.
* \param [in] eHWBlock   - DSI hardware block that needs to be disabled.
*
* \retval void
*
****************************************************************************/
void HAL_DSI_Disable_CTRL(DSI_Device_IDType    eDeviceID,
                          HAL_DSI_HWBlockType  eHWBlock )
{
  uintPtr uOffset  = HAL_DSI_GetRegBaseOffset(eDeviceID);
  uint32  uRegVal  = in_dword(uOffset + HWIO_DSI_0_CTRL_ADDR);

  if (HAL_DSI_HWBLOCK_CONTROLLER  == eHWBlock)
  {
    uRegVal = HWIO_OUT_FLD( uRegVal, DSI_0_CTRL, DSI_EN, 0);
  }

  if (HAL_DSI_HWBLOCK_VIDEOENGINE  == eHWBlock)
  {
    uRegVal = HWIO_OUT_FLD( uRegVal, DSI_0_CTRL, VIDEO_MODE_EN, 0);
  }

  if (HAL_DSI_HWBLOCK_CMDMODEENGINE  == eHWBlock)
  {
    uRegVal = HWIO_OUT_FLD( uRegVal, DSI_0_CTRL, CMD_MODE_EN, 0);
  }

  out_dword(uOffset + HWIO_DSI_0_CTRL_ADDR, uRegVal);
}

 

/****************************************************************************
*
** FUNCTION: HAL_DSI_Init_LP11()
*/
/*!
* \brief
*   Initialize the DSI controller to LP11
*
* \param [in] eDeviceID  - DSI Core Device ID.
*
* \retval void
*
****************************************************************************/
void HAL_DSI_Init_LP11(DSI_Device_IDType    eDeviceID)
{
  uint32  iDsiCtrlReg;
  uintPtr uOffset     = HAL_DSI_GetRegBaseOffset(eDeviceID);

  /* Force all lanes to LP11 */
  iDsiCtrlReg = HWIO_DSI_0_CTRL_DSI_EN_BMSK  |
                HWIO_DSI_0_CTRL_DLN0_EN_BMSK |
                HWIO_DSI_0_CTRL_DLN1_EN_BMSK |
                HWIO_DSI_0_CTRL_DLN2_EN_BMSK |
                HWIO_DSI_0_CTRL_DLN3_EN_BMSK |
                HWIO_DSI_0_CTRL_CLKLN_EN_BMSK;
    
  out_dword(uOffset + HWIO_DSI_0_CTRL_ADDR, iDsiCtrlReg);    
}

 

/****************************************************************************
*
** FUNCTION: HAL_DSI_IsEngineBusy()
*/
/*!
* \brief
*    Check whether or not DSI engine is busy.
*
* \param [in] eDeviceID  - DSI Core Device ID.
*
* \retval bool32
*
****************************************************************************/
bool32 HAL_DSI_IsEngineBusy(DSI_Device_IDType eDeviceID)
{
  uint32      uDsiStatus=0;
  bool32      bDsiEngineBusy;

  /* Retrieve the DSI status */
  HAL_DSI_GetStatus(eDeviceID, &uDsiStatus);
  bDsiEngineBusy = (0 != (uDsiStatus & (DSI_STATUS_DEVICE_BUSY_CMDENGINE|DSI_STATUS_DEVICE_BUSY_VIDEOENGINE)) ? TRUE : FALSE);

  return bDsiEngineBusy;
}

 

/****************************************************************************
*
** FUNCTION: HAL_DSI_IsEngineBtaBusy()
*/
/*!
* \brief
*    check whether or not DSI engine is busy for BTA.
*
* \param [in] eDeviceID  - DSI Core Device ID.
*
* \retval bool32
*
****************************************************************************/
bool32 HAL_DSI_IsEngineBtaBusy(DSI_Device_IDType eDeviceID)
{
  uint32      uDsiStatus=0;
  bool32      bDsiEngineBusy;

  /* Retrieve the DSI status */
  HAL_DSI_GetStatus(eDeviceID, &uDsiStatus);
  bDsiEngineBusy = (0 != (uDsiStatus & DSI_STATUS_DEVICE_BUSY_BTA)? TRUE:FALSE);

  return bDsiEngineBusy;
}



/****************************************************************************
*
** FUNCTION:HAL_DSI_WriteData()
*/
/*!
* \brief
*   This API write DMA out data to registers in embedded mode, waiting for 
*   trigger.
*
* \param [in] eDeviceID      - DSI core device ID.
* \param [in] psWritePacket  - pointer to data to be write out.
*
* \retval void
*
****************************************************************************/
void HAL_DSI_WriteData( DSI_Device_IDType         eDeviceID,
                        HAL_DSI_WritePacketType  *psWritePacket)
{
  /* Check input parameters */
  if ( NULL == psWritePacket)
  {
    geDSIErrorCode = HAL_DSI_FAILED_EXPECTED_NON_NULL_PTR;
  }
  else
  {
    uint32  uRegVal        = 0;
    uint32  uCmdDmaCtrlReg = 0;
    uintPtr uOffset        = HAL_DSI_GetRegBaseOffset(eDeviceID);

    /* Only use embedded mode */
    uCmdDmaCtrlReg = HWIO_OUT_FLD( uCmdDmaCtrlReg, DSI_0_COMMAND_MODE_DMA_CTRL, EMBEDDED_MODE, HAL_DSI_CMDMODE_DMA_EMBEDED);
    uCmdDmaCtrlReg = HWIO_OUT_FLD( uCmdDmaCtrlReg, DSI_0_COMMAND_MODE_DMA_CTRL, POWER_MODE,    (TRUE == psWritePacket->bHighSpeedMode? 0:1));

    /* Register write */
    out_dword(uOffset + HWIO_DSI_0_COMMAND_MODE_DMA_CTRL_ADDR, uCmdDmaCtrlReg);
    out_dword(uOffset + HWIO_DSI_0_DMA_CMD_LENGTH_ADDR,        psWritePacket->uPayloadLen);
    
    /*Check the DSI command is sent via DMA VBIF or TPG FIFO */
    if (0 == (psWritePacket->uFlags & HAL_DSI_FLAG_DATA_FETCH_VIA_FIFO))
    {
      out_dword(uOffset + HWIO_DSI_0_DMA_CMD_OFFSET_ADDR,        psWritePacket->uPhyAddress);

      /* Disable TPG FIFO */
      uRegVal = in_dword(uOffset + HWIO_DSI_0_TEST_PATTERN_GEN_CTRL_ADDR);
      uRegVal = HWIO_OUT_FLD(uRegVal, DSI_0_TEST_PATTERN_GEN_CTRL, CMD_DMA_TPG_EN,      0);
      uRegVal = HWIO_OUT_FLD(uRegVal, DSI_0_TEST_PATTERN_GEN_CTRL, TPG_DMA_FIFO_MODE,   0);
      uRegVal = HWIO_OUT_FLD(uRegVal, DSI_0_TEST_PATTERN_GEN_CTRL, CMD_DMA_PATTERN_SEL, 0);
      out_dword(uOffset + HWIO_DSI_0_TEST_PATTERN_GEN_CTRL_ADDR, uRegVal);
    }
    else
    {
      uint32       *pPayLoad = (uint32*)psWritePacket->pPackedFifoBuffer;
      uint32        uPayloadLengthInWord = (psWritePacket->uPayloadLen + 3)>>2;
      uint32        uNumWrites           = 0;
      uint32        i;

      uRegVal = in_dword(uOffset + HWIO_DSI_0_TEST_PATTERN_GEN_CTRL_ADDR);
      uRegVal = HWIO_OUT_FLD(uRegVal, DSI_0_TEST_PATTERN_GEN_CTRL, CMD_DMA_TPG_EN,      1);
      uRegVal = HWIO_OUT_FLD(uRegVal, DSI_0_TEST_PATTERN_GEN_CTRL, TPG_DMA_FIFO_MODE,   1);
      uRegVal = HWIO_OUT_FLD(uRegVal, DSI_0_TEST_PATTERN_GEN_CTRL, CMD_DMA_PATTERN_SEL, 3);
      out_dword(uOffset + HWIO_DSI_0_TEST_PATTERN_GEN_CTRL_ADDR, uRegVal);

      for (i = 0; i< uPayloadLengthInWord; i++)
      {
        /* Write in to FIFO*/
        uRegVal = pPayLoad[i];
        out_dword(uOffset + HWIO_DSI_0_TEST_PATTERN_GEN_CMD_DMA_INIT_VAL_ADDR, uRegVal);
        uNumWrites++;
      }

      /* TPG FIFO is in unit of 64 bit, need to make sure SW write to FIFO even times*/
      if((uNumWrites %2 ) != 0)
      {
        out_dword(uOffset + HWIO_DSI_0_TEST_PATTERN_GEN_CMD_DMA_INIT_VAL_ADDR, 0);
      }
    }
  }
}


/****************************************************************************
*
** FUNCTION: HAL_DSI_CalculateECC()
*/
/*!
* \brief
*   Calculates ECC over the read response header received from the panel.
*
* \param [in] pData  - Response packet data from the panel.
*
* \retval uint8 
*
****************************************************************************/
uint8 HAL_DSI_CalculateECC(uint8 *pData)
{
  uint8       uECCParityRules[] = 
  {
    0x07,       /* data bit 0 */
    0x0B,       /* data bit 1 */
    0x0D,       /* data bit 2 */
    0x0E,       /* data bit 3 */
    0x13,       /* data bit 4 */
    0x15,       /* data bit 5 */
    0x16,       /* data bit 6 */
    0x19,       /* data bit 7 */
    0x1A,       /* data bit 8 */
    0x1C,       /* data bit 9 */
    0x23,       /* data bit 10 */
    0x25,       /* data bit 11 */
    0x26,       /* data bit 12 */
    0x29,       /* data bit 13 */
    0x2A,       /* data bit 14 */
    0x2C,       /* data bit 15 */
    0x31,       /* data bit 16 */
    0x32,       /* data bit 17 */
    0x34,       /* data bit 18 */
    0x38,       /* data bit 19 */
    0x1F,       /* data bit 20 */
    0x2F,       /* data bit 21 */
    0x37,       /* data bit 22 */
    0x3B        /* data bit 23 */
  };
  int         i, j;
  uint8       uData, uECC = 0;

  /* Only calculate ECC for 3 bytes */
  for(i=0; i<3; i++)
  {
    uData = *(pData+i);

    /* Loop 8 bits */
    for(j=0; j<8; j++)
    {
      /* Check data bit is 1 or 0 */
      if (uData & (1<<j))
      {
        uECC ^= uECCParityRules[i*8+j];
      }
    }
  }

  return uECC;
}



/****************************************************************************

HAL_DSI_IsShortPacket()

Description:
    This function is used to check whether a given 4-byte data is DSI short
    response packet or not.

****************************************************************************/
bool32 HAL_DSI_IsShortPacket(uint8 *pData, uint32 *pStartPos, uint32 *pEndPos)
{
  bool32  bRet = FALSE;
  uint8   uDataType, uECC;

  uDataType = *(pData+*pStartPos);

  if ((DSI_CMDMODE_DT_RETACKWITHERROR           == uDataType) ||
      (DSI_CMDMODE_DT_EOTP                      == uDataType) ||
      (DSI_CMDMODE_DT_RETGENERICSHORTREAD1PARA  == uDataType) ||
      (DSI_CMDMODE_DT_RETGENERICSHORTREAD2PARA  == uDataType) ||
      (DSI_CMDMODE_DT_RETDSCSHORTREAD1PARA      == uDataType) ||
      (DSI_CMDMODE_DT_RETDSCSHORTREAD2PARA      == uDataType))
  {
    /*
    * if data type matches, then we confirm this using ECC
    */
    uECC = HAL_DSI_CalculateECC(pData+*pStartPos);
    if (uECC == *(pData+*pStartPos+3))
    {
      bRet = TRUE;
    }
  }

  return bRet;
}


/****************************************************************************

HAL_DSI_SearchLongPacket()

Description:
    Given a data array, this function will search for a valid long DSI response
    packet

****************************************************************************/
bool32 HAL_DSI_SearchLongPacket(uint8 *pData, uint32 *pStartPos, uint32 *pEndPos, uint32 *pWC)
{
  bool32          bRet = FALSE;
  int             i;
  uint16          *pPtr16;
  uint8           uECC;

  for(i=(int)*pStartPos; i>=0; i--)
  {
    /* check data type */
    if (DSI_CMDMODE_DT_RETGENERICLONGEADPARA == *(pData+i) || 
       (DSI_CMDMODE_DT_RETDSCLONGREAD        == *(pData+i)) )
    {
      /* check WC */
      pPtr16 = (uint16 *)(pData+i+1);

      /*
      *
      *  pData                i                       *pEndPos
      *  xx xx xx xx xx xx xx xx xx xx xx xx xx xx xx xx
      *                       |  --WC-  | |      | |   |
      *                       +---------+ +------+ +---+
      *                          header    payload checksum
      *
      */

      if (*pPtr16 == (*pEndPos - i - HAL_DSI_EMBEDDED_HEADER_SIZE - 2 + 1))
      {
        /* confirm the packet header using ECC */
        uECC = HAL_DSI_CalculateECC(pData+i);
        if (uECC == *(pData+i+3))
        {
          bRet = TRUE;
          *pStartPos = i;     /* update the packet start position in the data array */
          *pWC = (uint32)*pPtr16;
          break;
        }
      }
    }
  }

  return bRet;
}


/****************************************************************************
*
** FUNCTION: HAL_DSI_ReadData()
*/
/*!
* \brief
*   This API allows users to send the DSI read command out to DSI Panels, 
*   and wait until it gets the data from panel.
*
* \param [in] eDeviceID     - DSI core device ID.
* \param [in] psReadPacket  - Contains the data to be read out from panels.
*
* \retval HAL_dsi_StatusType
*
****************************************************************************/
HAL_dsi_StatusType HAL_DSI_ReadData(DSI_Device_IDType       eDeviceID,
                                    HAL_DSI_ReadPacketType* psReadPacket)
{
  uint8   uReadDataReg[HAL_DSI_MAX_RETURNSIZE_BYTE];
  uint32 *puReadDataReg32;
  uint32  uErrorStatus;
  uint32  uStartPos                     = 1; 
  uint32  uEndPos                       = 0;
  uint32  uWC                           = 0;
  uintPtr uOffset                       = HAL_DSI_GetRegBaseOffset(eDeviceID);
  HAL_dsi_StatusType    eRetStatus      = HAL_DSI_SUCCESS;

  if (NULL == psReadPacket->puReadPacket)
  {
    eRetStatus = HAL_DSI_FAILED_EXPECTED_NON_NULL_PTR;
  }
  else
  {
    /*
    * we are going to return the whole packet, including header, payload and check sum,
    * so the buffer size should be at least 4
    */
    if (psReadPacket->uReadPacketBufferSize < HAL_DSI_EMBEDDED_HEADER_SIZE)
    {
       eRetStatus = HAL_DSI_FAILED_BUFFER_TOO_SMALL;
    }
    else
    {
      /* read DSI status for any ACK_ERROR case */
      HAL_DSI_GetPanelErrorStatus(eDeviceID, &uErrorStatus);

      if ((0 == uErrorStatus) ||
          (0 != (uErrorStatus & HAL_DSI_ACK_ERROR_SINGLE_ECC_ERR)))
      {
        /*
        * DSI RDBK Data[0~4] is working as FIFO 
        * Packets returned from the peripheral will be stored in DSI_RDBK_DATA[0-4] registers. 
        * They will shift from DSI_RDBK_DATA0 -> 1 -> 2 ->3 as packets get read in.
        */
        puReadDataReg32 = (uint32*) &uReadDataReg[0];
        *puReadDataReg32 = in_dword(uOffset + HWIO_DSI_0_RDBK_DATA3_ADDR);
        SwapEndian(*puReadDataReg32 , *puReadDataReg32 );
        puReadDataReg32++;
        *puReadDataReg32 = in_dword(uOffset + HWIO_DSI_0_RDBK_DATA2_ADDR);
        SwapEndian(*puReadDataReg32 , *puReadDataReg32 );
        puReadDataReg32++;
        *puReadDataReg32 = in_dword(uOffset + HWIO_DSI_0_RDBK_DATA1_ADDR);
        SwapEndian(*puReadDataReg32 , *puReadDataReg32 );
        puReadDataReg32++;
        *puReadDataReg32 = in_dword(uOffset + HWIO_DSI_0_RDBK_DATA0_ADDR);
        SwapEndian(*puReadDataReg32 , *puReadDataReg32 );

        uEndPos = HAL_DSI_MAX_RETURNSIZE_BYTE-1;

        /* initial packet start position */
        uStartPos = uEndPos - HAL_DSI_EMBEDDED_HEADER_SIZE + 1;

        /* confirm a valid short packet */
        if (FALSE == HAL_DSI_IsShortPacket(uReadDataReg, &uStartPos, &uEndPos))
        {
          /* search for a valid long packet */
          /* new start position offset: 2 bytes of check sum, 4 bytes of header */
          uStartPos = uEndPos - 2 - HAL_DSI_EMBEDDED_HEADER_SIZE + 1;
          if (FALSE == HAL_DSI_SearchLongPacket(uReadDataReg, &uStartPos, &uEndPos, &uWC))
          {
            /* we have an error here since we cannot find a valid long packet */
            eRetStatus = HAL_DSI_FAILED_PACKET_CORRUPTED;
            uEndPos = 0;
            uStartPos = 1;
          }
          else
          {
            if (0 == (psReadPacket->uFlags & HAL_DSI_CMD_FLAG_READ_RAW_PACKET))
            {
              uStartPos += HAL_DSI_EMBEDDED_HEADER_SIZE;
              uEndPos -= 2;
            }
          }
        }
        else
        {
          uWC = 2;

          if (0 == (psReadPacket->uFlags & HAL_DSI_CMD_FLAG_READ_RAW_PACKET))
          {
            uStartPos += 1;
            uEndPos -= 1;
          }
        }

        if ((HAL_DSI_MAX_RETURNSIZE_BYTE>uEndPos) && 
            (uEndPos >= uStartPos))
        {
          uint32 i;

          psReadPacket->uTotalPacketWC = uWC;
          psReadPacket->uRetPacketByte = uEndPos-uStartPos+1;

          /* set returned packet size based on the read buffer size */
          if (psReadPacket->uReadPacketBufferSize < psReadPacket->uRetPacketByte)
          {
            psReadPacket->uRetPacketByte = psReadPacket->uReadPacketBufferSize;

            /* Indicate the caller that the read response buffer passed was small */ 
            eRetStatus = HAL_DSI_FAILED_BUFFER_TOO_SMALL;
          }

          /* copy the packet */
          for (i=0; i < psReadPacket->uRetPacketByte; i++)
          {
            uint32 j = uStartPos + i;
            if (j < HAL_DSI_MAX_RETURNSIZE_BYTE)
            {
              psReadPacket->puReadPacket[i] = uReadDataReg[j];
            }
          }
        }
        else
        {
          psReadPacket->uTotalPacketWC = 0;
          psReadPacket->uRetPacketByte = 0;
        }
      }
    }
  }

  return eRetStatus;
}



/****************************************************************************
*
** FUNCTION: HAL_DSI_Trigger()
*/
/*!
* \brief
*   This API allows users to trigger the command DMA out by Software.
*
* \param [in] eDeviceID  - DSI core device ID.
* \param [in] eFlag      - Flag for commit function.
*
* \retval void
*
****************************************************************************/
void HAL_DSI_Trigger(DSI_Device_IDType         eDeviceID,
                     HAL_DSI_TriggerFlagType   eFlag)
{
  uintPtr  uOffset = HAL_DSI_GetRegBaseOffset(eDeviceID);

  switch(eFlag)
  {
    case DSI_TriggerFlag_DMA_SW:
    { 
      out_dword(uOffset + HWIO_DSI_0_CMD_MODE_DMA_SW_TRIGGER_ADDR, 0x01);
      break;
    }
    case DSI_TriggerFlag_MDP_SW:
    { 
      out_dword(uOffset + HWIO_DSI_0_CMD_MODE_MDP_SW_TRIGGER_ADDR, 0x01);
      break;
    }
    case DSI_TriggerFlag_BTA_SW:
    { 
      out_dword(uOffset + HWIO_DSI_0_CMD_MODE_BTA_SW_TRIGGER_ADDR, 0x01);
      break;
    }
    case DSI_TriggerFlag_RESET_SW:
    { 
      // SWI Deprecated 
      break;
    }
    default:
    {
      geDSIErrorCode = HAL_DSI_FAILED_NO_HW_SUPPORT;
      break;
    }
  }
  return;

}



/****************************************************************************
*
** FUNCTION: HAL_DSI_CalculateTimeout()
*/
/*!
* \brief
*    Calculates steps and count for timer timeout in terms of escape clock.
*    It uses the following algorithm:
*     1) Calculates the timer ticks needed for the requested timeout.
*         Timertick = Requested duration in ms * 19200 , for a 19.2 Mhz CLock
*     2) Computes the count and steps needed by finding the step and count 
*        that would provided a timer tick greater than the requested timer tick.
*
* \param [in]  uTimeinMS      - Time in milliseconds.
* \param [out] pTimeoutCount  - Timeout value in terms of count.
* \param [out] pTimeoutSteps  - Timeout value in terms of steps.
*
* \retval void
*
****************************************************************************/
void HAL_DSI_CalculateTimeout(uint32  uTimeinMS, 
                              uint32 *pTimeoutCount, 
                              uint32 *pTimeoutSteps)
{
  uint32       uIdx;
  uint32       uTimerTick = uTimeinMS * (uint32)(HAL_DSI_ESCCLK_SRC * 1000.0f);
  uint32       uCount     = DSI_TIMEOUT_MAX_COUNT; 

  /*
   * Steps required by the timer is in terms of  power of 2
   * The relationship between the steps ,count and timeout is the following 
   * Timeout = 2^Steps*count
   * The below loop calculates the Steps required
   */
  uIdx = 0;
  uCount = uTimerTick;

  while(uCount > DSI_TIMEOUT_MAX_COUNT)
  {
    uCount >>= 1;
    uIdx++;
  }

  if(pTimeoutCount != NULL)
  {
    *pTimeoutCount = uCount;
  }

  if(pTimeoutSteps != NULL)
  {
    *pTimeoutSteps = uIdx;
  }
}


/****************************************************************************
*
** FUNCTION: HAL_DSI_ReadCoreVersionInfo()
*/
/*!
* \brief
*    Returns the DSI controller version information.
*
* \param [out] psDSIVersionInfo  - DSI controller version information.
*
* \retval void
*
****************************************************************************/
void HAL_DSI_ReadCoreVersionInfo(HAL_DSI_VersionType *psDSIVersionInfo)
{
  uint32      uVersionInfo = in_dword(HWIO_DSI_0_HW_VERSION_ADDR);

  if (NULL != psDSIVersionInfo)
  {
    psDSIVersionInfo->uMajorVersion   = HWIO_GETVAL(DSI_0_HW_VERSION, uVersionInfo, MAJOR);
    psDSIVersionInfo->uMinorVersion   = HWIO_GETVAL(DSI_0_HW_VERSION, uVersionInfo, MINOR);
    psDSIVersionInfo->uReleaseVersion = HWIO_GETVAL(DSI_0_HW_VERSION, uVersionInfo, STEP);
  }
}

#ifdef __cplusplus
}
#endif
