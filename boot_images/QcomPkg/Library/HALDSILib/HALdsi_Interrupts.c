/*=============================================================================

  File: HALdsi_Interrupts.c
  

     Copyright (c) 2010-2014 Qualcomm Technologies, Inc.  All Rights Reserved.
     Qualcomm Technologies Proprietary and Confidential.
=============================================================================*/

/*============================================================================
*                         INCLUDE FILES
============================================================================*/
#include "HALdsi.h"
#include "HALdsi_reg.h"

/* --- DO NOT ADD HEADER FILES HERE! --- */

#ifdef __cplusplus
extern "C" {
#endif

/* Extern debug variable from HALdsi.c */
extern HAL_dsi_StatusType geDSIErrorCode;

/****************************************************************************
HAL_DSI_SetInterruptsEnabled()

Description:
    Enable the hardware interrupts specified in the iInterruptSource field.
    Each bit field is parsed and mapped to the equivalent hardware field.  
    The value in iInterruptSource is NOT directly used to program the hardware 
    register

****************************************************************************/
void HAL_DSI_SetInterruptsEnabled( DSI_Device_IDType    eDeviceID,
                                   uint32               uInterruptSource)
{
  uint32  uIntrEnableReg    = 0;
  uintPtr uOffset           = HAL_DSI_GetRegBaseOffset(eDeviceID);

  /* Does the HAL support all of the DSI devices requested? */
  if ( (uInterruptSource & HAL_DSI_INTERRUPT_ALL) != uInterruptSource )
  {
    /* Does the HAL support all of the interrupts requested? */
    geDSIErrorCode = HAL_DSI_FAILED_NO_SUPPORT_OR_NO_HW;
  }
  else
  {
    /* Map CMD mode DMA Done to HW interrupts. */
    if ( (uInterruptSource & HAL_DSI_INTERRUPT_CMDMODE_DMA_DONE) != 0 )
    {  uIntrEnableReg = HWIO_OUT_FLD(uIntrEnableReg, DSI_0_INT_CTRL, DSI_CMD_MODE_DMA_DONE_MASK,  1); }

    /* Map CMD mode MDP Done to HW interrupts. */
    if ( (uInterruptSource & HAL_DSI_INTERRUPT_CMDMODE_MDP_DONE) != 0 )
    {  uIntrEnableReg = HWIO_OUT_FLD(uIntrEnableReg, DSI_0_INT_CTRL, DSI_CMD_MODE_MDP_DONE_MASK,  1); }

    /* Map Video mode Done to HW interrupts. */
    if ( (uInterruptSource & HAL_DSI_INTERRUPT_VIDEOMODE_DONE) != 0 )
    {  uIntrEnableReg = HWIO_OUT_FLD(uIntrEnableReg, DSI_0_INT_CTRL, DSI_VIDEO_MODE_DONE_MASK,    1); }

    /* If any Error interrupt has been enabled? */
    if ( (uInterruptSource & HAL_DSI_INTERRUPT_DSI_ERROR) != 0 )
    {  uIntrEnableReg = HWIO_OUT_FLD(uIntrEnableReg, DSI_0_INT_CTRL, DSI_ERROR_MASK,              1); }

    /* If need to enable DSI_BTA_DONE */
    if ( (uInterruptSource & HAL_DSI_INTERRUPT_BTA_DONE) != 0 )
    {  uIntrEnableReg = HWIO_OUT_FLD(uIntrEnableReg, DSI_0_INT_CTRL, DSI_BTA_DONE_MASK,           1); }

    /* Enable the HW interrupts. */
    out_dword(uOffset + HWIO_DSI_0_INT_CTRL_ADDR, uIntrEnableReg );
  }

}
  
/****************************************************************************
HAL_DSI_GetInterruptsEnabled()

Description:
    Get the hardware interrupts enabled.  Each bit field is parsed and mapped
    to the equivalent hardware field.  The value in iInterruptSource is NOT
    directly used to program the hardware register

****************************************************************************/
void HAL_DSI_GetInterruptsEnabled(DSI_Device_IDType   eDeviceID,
                                  uint32             *puInterruptSource)
{
  uint32  uIntrEnableReg    = 0;
  uint32  uHalIntrSource    = HAL_DSI_INTERRUPT_NONE;
  uintPtr uOffset           = HAL_DSI_GetRegBaseOffset(eDeviceID);

  if (NULL == puInterruptSource)
  {
    geDSIErrorCode      = HAL_DSI_FAILED_EXPECTED_NON_NULL_PTR;
  }
  else
  {
    /* Get the DSI HW interrupts enabled. */
    uIntrEnableReg  = in_dword(uOffset + HWIO_DSI_0_INT_CTRL_ADDR);

    /* Map CMD mode DMA Done interrupts to generic HAL interface. */
    if (0 != HWIO_GETVAL(DSI_0_INT_CTRL, uIntrEnableReg, DSI_CMD_MODE_DMA_DONE_MASK))
    { uHalIntrSource |= HAL_DSI_INTERRUPT_CMDMODE_DMA_DONE; }

    /* Map CMD mode MDP Done interrupts to generic HAL interface. */
    if (0 != HWIO_GETVAL(DSI_0_INT_CTRL, uIntrEnableReg, DSI_CMD_MODE_MDP_DONE_MASK))  
    { uHalIntrSource |= HAL_DSI_INTERRUPT_CMDMODE_MDP_DONE; }

    /* Map Video mode Done interrupts to generic HAL interface. */
    if (0 != HWIO_GETVAL(DSI_0_INT_CTRL, uIntrEnableReg, DSI_VIDEO_MODE_DONE_MASK))  
    { uHalIntrSource |= HAL_DSI_INTERRUPT_VIDEOMODE_DONE; }

    /* If any Error interrupt has been enabled? */
    if (0 != HWIO_GETVAL(DSI_0_INT_CTRL, uIntrEnableReg, DSI_ERROR_MASK))  
    { uHalIntrSource |= HAL_DSI_INTERRUPT_DSI_ERROR; }

    /* If DSI BTA_DONE has been enabled */
    if (0 != HWIO_GETVAL(DSI_0_INT_CTRL, uIntrEnableReg, DSI_BTA_DONE_MASK))  
    { uHalIntrSource |= HAL_DSI_INTERRUPT_BTA_DONE; }

    *puInterruptSource = uHalIntrSource;
  }
}
/****************************************************************************

HAL_DSI_GetInterruptStatus()

Description:
    Get the hardware interrupt status.  Each bit field is parsed and mapped to
    the equivalent hardware field.  The value in iInterruptSource is NOT 
    directly used to program the hardware status register.

****************************************************************************/
void HAL_DSI_GetInterruptStatus(DSI_Device_IDType   eDeviceID,
                                uint32             *puInterruptSource,
                                uint32             *puErrorCode)
{
  uint32  uHalIntrSource    = HAL_DSI_INTERRUPT_NONE;
  uint32  uIntrStatusReg    = HAL_DSI_INTERRUPT_NONE;
  uintPtr uOffset           = HAL_DSI_GetRegBaseOffset(eDeviceID);

  if ((NULL == puErrorCode) ||
      (NULL == puInterruptSource))
  {
    geDSIErrorCode = HAL_DSI_FAILED_EXPECTED_NON_NULL_PTR;
  }
  else
  {
    *puErrorCode                      = 0;

    uIntrStatusReg  = in_dword(uOffset + HWIO_DSI_0_INT_CTRL_ADDR);

    /* Get CMD mode DMA Done and map to generic HAL interface. */
    if (0 != HWIO_GETVAL(DSI_0_INT_CTRL, uIntrStatusReg, DSI_CMD_MODE_DMA_DONE_STAT))  
    {
      uHalIntrSource |= HAL_DSI_INTERRUPT_CMDMODE_DMA_DONE;
    }

    /* Get CMD mode MDP Done and map to generic HAL interface. */
    if (0 != HWIO_GETVAL(DSI_0_INT_CTRL, uIntrStatusReg, DSI_CMD_MODE_MDP_DONE_STAT))  
    {
      uHalIntrSource |= HAL_DSI_INTERRUPT_CMDMODE_MDP_DONE;
    }

    /* Get Video mode Done and map to generic HAL interface. */
    if (0 != HWIO_GETVAL(DSI_0_INT_CTRL, uIntrStatusReg, DSI_VIDEO_MODE_DONE_STAT))  
    {
      uHalIntrSource |= HAL_DSI_INTERRUPT_VIDEOMODE_DONE;
    }

    /* Get the BTA done and map to generic HAL interface */
    if (0 != HWIO_GETVAL(DSI_0_INT_CTRL, uIntrStatusReg, DSI_BTA_DONE_STAT))  
    {
      uHalIntrSource |= HAL_DSI_INTERRUPT_BTA_DONE;
    } 

    /* If any Error interrupt Triggered */
    if (0 != HWIO_GETVAL(DSI_0_INT_CTRL, uIntrStatusReg, DSI_ERROR_STAT))  
    {
      uHalIntrSource |= HAL_DSI_INTERRUPT_DSI_ERROR;
    } 

    *puInterruptSource = uHalIntrSource;
  }

}/* Function: HAL_DSI_GetInterruptStatus()*/

/****************************************************************************

HAL_DSI_ClrInterruptStatus()

Description:
    Clear the hardware interrupt status register. Each bit field is parsed 
    and mapped to the equivalent hardware field. The value in iInterruptSource
    is NOT directly used to program the hardware status register.

   ****************************************************************************/
void HAL_DSI_ClrInterruptStatus(DSI_Device_IDType   eDeviceID,
                                uint32              uInterruptSource)
{
  uintPtr uOffset            = HAL_DSI_GetRegBaseOffset(eDeviceID);
  uint32  uIntrStatusReg     = in_dword(uOffset + HWIO_DSI_0_INT_CTRL_ADDR);

  /* Trying to use the unmapped interrupt? */
  if ( (uInterruptSource & HAL_DSI_INTERRUPT_ALL) != uInterruptSource )
  {
    geDSIErrorCode = HAL_DSI_FAILED_INTERRUPTS_UNMAPPED;
  }

  /* Get CMD mode DMA Done and map to generic HAL interface. */
  if ( (uInterruptSource & HAL_DSI_INTERRUPT_CMDMODE_DMA_DONE) != 0)
  {
    uIntrStatusReg = HWIO_OUT_FLD(uIntrStatusReg, DSI_0_INT_CTRL, DSI_CMD_MODE_DMA_DONE_AK, 1); 
  }

  /* Get CMD mode MDP Done and map to generic HAL interface. */
  if ( (uInterruptSource & HAL_DSI_INTERRUPT_CMDMODE_MDP_DONE) != 0)
  {
    uIntrStatusReg = HWIO_OUT_FLD(uIntrStatusReg, DSI_0_INT_CTRL, DSI_CMD_MODE_MDP_DONE_AK, 1); 
  }

  /* Get Video mode Done and map to generic HAL interface. */
  if ( (uInterruptSource & HAL_DSI_INTERRUPT_VIDEOMODE_DONE) != 0)
  {
    uIntrStatusReg = HWIO_OUT_FLD(uIntrStatusReg, DSI_0_INT_CTRL, DSI_VIDEO_MODE_DONE_AK,   1); 
  }

  /* Get the BTA done and map to generic HAL interface. clear the interrupt status bit */
  if ( (uInterruptSource & HAL_DSI_INTERRUPT_BTA_DONE) != 0)
  {
    uIntrStatusReg = HWIO_OUT_FLD(uIntrStatusReg, DSI_0_INT_CTRL, DSI_BTA_DONE_AK,          1); 
  }

  if ( (uInterruptSource & HAL_DSI_INTERRUPT_DSI_ERROR) != 0)
  {
    uIntrStatusReg = HWIO_OUT_FLD(uIntrStatusReg, DSI_0_INT_CTRL, DSI_ERROR_AK,             1); 
  }

  /* Enable the HW interrupts. */
  out_dword(uOffset + HWIO_DSI_0_INT_CTRL_ADDR, uIntrStatusReg);

}/* Function: HAL_DSI_ClrInterruptStatus()  */

/****************************************************************************

HAL_DSI_SetErrorMaskEnabled()

Description:
   Enable the DSI Error Mask specified in the uErrorSource field.
   Each bit field is parsed and mapped to the equivalent hardware field.  
   The value in uErrorSource is NOT directly used to program the hardware 
   register

****************************************************************************/
void HAL_DSI_SetErrorMaskEnabled( DSI_Device_IDType       eDeviceID,
                                  uint32                  uErrorSource)
{
  uint32  uErrorIntrEnableReg  = 0;
  uintPtr uOffset              = HAL_DSI_GetRegBaseOffset(eDeviceID);

  /* Does the HAL support all of the interrupts requested? */
  if ( (uErrorSource & HAL_DSI_ERROR_INT_ALL) != uErrorSource )
  {
    geDSIErrorCode = HAL_DSI_FAILED_NO_SUPPORT_OR_NO_HW;
  }
  else
  {
    /* Below are the Error interrupts           */
    /* Map PHY PLL unlock error to HW interrupts.    */
    if ( (uErrorSource & HAL_DSI_ERROR_INT_DPHY_DSIPLL_UNLOCK) != 0 )
      uErrorIntrEnableReg = HWIO_OUT_FLD(uErrorIntrEnableReg, DSI_0_ERR_INT_MASK0, DPHY_DSIPLL_UNLOCK_MASK,         1); 

    /* Map Contention error while LN0 is driven high to HW interrupts.    */
    if ( (uErrorSource & HAL_DSI_ERROR_INT_DLN0_CONTENTION_LP1) != 0 )
      uErrorIntrEnableReg = HWIO_OUT_FLD(uErrorIntrEnableReg, DSI_0_ERR_INT_MASK0, DLN0_ERR_CONTENTION_LP1_MASK,    1);

    /* Map Contention error while LN0 is driven low to HW interrupts.    */
    if ( (uErrorSource & HAL_DSI_ERROR_INT_DLN0_CONTENTION_LP0) != 0 )
      uErrorIntrEnableReg = HWIO_OUT_FLD(uErrorIntrEnableReg, DSI_0_ERR_INT_MASK0, DLN0_ERR_CONTENTION_LP0_MASK,    1); 

    /* Map LP RX state sequence error to HW interrupts.    */
    if ( (uErrorSource & HAL_DSI_ERROR_INT_DLN0_CONTROL) != 0 )
      uErrorIntrEnableReg = HWIO_OUT_FLD(uErrorIntrEnableReg, DSI_0_ERR_INT_MASK0, DLN0_ERR_CONTROL_MASK,           1); 

    /* Map LP RX data not byte aligned error to HW interrupts.    */
    if ( (uErrorSource & HAL_DSI_ERROR_INT_DLN0_SYNC_ESC) != 0 )
      uErrorIntrEnableReg = HWIO_OUT_FLD(uErrorIntrEnableReg, DSI_0_ERR_INT_MASK0, DLN0_ERR_SYNC_ESC_MASK,          1); 

    /* Map LP RX Escape Entry error to HW interrupts.    */
    if ( (uErrorSource & HAL_DSI_ERROR_INT_DLN0_ESC) != 0 )
      uErrorIntrEnableReg = HWIO_OUT_FLD(uErrorIntrEnableReg, DSI_0_ERR_INT_MASK0, DLN0_ERR_ESC_MASK,               1); 

    /* Map DL0 Lower Power FIFO error to HW interrupts.    */
    if ( (uErrorSource & HAL_DSI_ERROR_INT_DLN0_LP_FIFO_OVERFLOW) != 0 )
      uErrorIntrEnableReg = HWIO_OUT_FLD(uErrorIntrEnableReg, DSI_0_ERR_INT_MASK0, DLN0_LP_FIFO_OVERFLOW_MASK,      1); 

    /* Map DL0 High Speed FIFO error to HW interrupts.    */
    if ( (uErrorSource & HAL_DSI_ERROR_INT_DLN0_HS_FIFO_OVERFLOW) != 0 )
      uErrorIntrEnableReg = HWIO_OUT_FLD(uErrorIntrEnableReg, DSI_0_ERR_INT_MASK0, DLN0_HS_FIFO_OVERFLOW_MASK,      1); 

    /* Map DL1 High Speed FIFO error to HW interrupts.    */
    if ( (uErrorSource & HAL_DSI_ERROR_INT_DLN1_HS_FIFO_OVERFLOW) != 0 )
      uErrorIntrEnableReg = HWIO_OUT_FLD(uErrorIntrEnableReg, DSI_0_ERR_INT_MASK0, DLN1_HS_FIFO_OVERFLOW_MASK,      1); 

    /* Map DL2 High Speed FIFO error to HW interrupts.    */
    if ( (uErrorSource & HAL_DSI_ERROR_INT_DLN2_HS_FIFO_OVERFLOW) != 0 )
      uErrorIntrEnableReg = HWIO_OUT_FLD(uErrorIntrEnableReg, DSI_0_ERR_INT_MASK0, DLN2_HS_FIFO_OVERFLOW_MASK,      1); 

    /* Map DL3 High Speed FIFO error to HW interrupts.    */
    if ( (uErrorSource & HAL_DSI_ERROR_INT_DLN3_HS_FIFO_OVERFLOW) != 0 )
      uErrorIntrEnableReg = HWIO_OUT_FLD(uErrorIntrEnableReg, DSI_0_ERR_INT_MASK0, DLN3_HS_FIFO_OVERFLOW_MASK,      1); 

    /* Map Video mode FIFO underflow Error to HW interrupts.    */
    if ( (uErrorSource & HAL_DSI_ERROR_INT_VID_MDP_FIFO_UNDERFLOW) != 0 )
      uErrorIntrEnableReg = HWIO_OUT_FLD(uErrorIntrEnableReg, DSI_0_ERR_INT_MASK0, VID_MDP_FIFO_UNDERFLOW_MASK,     1); 

    /* Map Video mode FIFO overflow Error to HW interrupts.    */
    if ( (uErrorSource & HAL_DSI_ERROR_INT_VID_MDP_FIFO_OVERFLOW) != 0 )
      uErrorIntrEnableReg = HWIO_OUT_FLD(uErrorIntrEnableReg, DSI_0_ERR_INT_MASK0, VID_MDP_FIFO_OVERFLOW_MASK,      1); 

    /* Map CMD mode MDP FIFO underflow Error to HW interrupts.    */
    if ( (uErrorSource & HAL_DSI_ERROR_INT_CMD_MDP_FIFO_UNDERFLOW) != 0 )
      uErrorIntrEnableReg = HWIO_OUT_FLD(uErrorIntrEnableReg, DSI_0_ERR_INT_MASK0, CMD_MDP_FIFO_UNDERFLOW_MASK,     1); 

    /* MapCMD mode DMA FIFO underflow Error to HW interrupts.    */
    if ( (uErrorSource & HAL_DSI_ERROR_INT_CMD_DMA_FIFO_UNDERFLOW) != 0 )
      uErrorIntrEnableReg = HWIO_OUT_FLD(uErrorIntrEnableReg, DSI_0_ERR_INT_MASK0, CMD_DMA_FIFO_UNDERFLOW_MASK,     1); 

    /* Map Interleave operation contention Error to HW interrupts.    */
    if ( (uErrorSource & HAL_DSI_ERROR_INT_INTERLEAVE_OP_CONTENTION) != 0 )
      uErrorIntrEnableReg = HWIO_OUT_FLD(uErrorIntrEnableReg, DSI_0_ERR_INT_MASK0, INTERLEAVE_OP_CONTENTION_MASK,   1); 

    /* Map BTA timeout error to HW interrupts.    */
    if ( (uErrorSource & HAL_DSI_ERROR_INT_BTA_TO) != 0 )
      uErrorIntrEnableReg = HWIO_OUT_FLD(uErrorIntrEnableReg, DSI_0_ERR_INT_MASK0, BTA_TO_MASK,                     1); 

    /* Map HS forward transmission timeout error to HW interrupts.    */
    if ( (uErrorSource & HAL_DSI_ERROR_INT_HS_TX_TO) != 0 )
      uErrorIntrEnableReg = HWIO_OUT_FLD(uErrorIntrEnableReg, DSI_0_ERR_INT_MASK0, HS_TX_TO_MASK,                   1); 

    /* Map LP reverse transmission timeout received to HW interrupts.    */
    if ( (uErrorSource & HAL_DSI_ERROR_INT_LP_RX_TO) != 0 )
      uErrorIntrEnableReg = HWIO_OUT_FLD(uErrorIntrEnableReg, DSI_0_ERR_INT_MASK0, LP_RX_TO_MASK,                   1); 

    /* Map Error Packet returned from Panel to HW interrupts.    */
    if ( (uErrorSource & HAL_DSI_ERROR_INT_PACKET) != 0 )
      uErrorIntrEnableReg = HWIO_OUT_FLD(uErrorIntrEnableReg, DSI_0_ERR_INT_MASK0, ERROR_PACKET_MASK,               1); 

    /* Map Incomplete read packet error to HW interrupts.    */
    if ( (uErrorSource & HAL_DSI_ERROR_INT_RDBK_INCOMPLETE_PACKET) != 0 )
      uErrorIntrEnableReg = HWIO_OUT_FLD(uErrorIntrEnableReg, DSI_0_ERR_INT_MASK0, RDBK_INCOMPLETE_PACKET_ERR_MASK, 1); 

    /* Map CRC Error in read packet to HW interrupts.    */
    if ( (uErrorSource & HAL_DSI_ERROR_INT_RDBK_DATA_CRC) != 0 )
      uErrorIntrEnableReg = HWIO_OUT_FLD(uErrorIntrEnableReg, DSI_0_ERR_INT_MASK0, RDBK_DATA_CRC_ERR_MASK,          1); 

    /* Map multi-bit ECC Error in read packet to HW interrupts.    */
    if ( (uErrorSource & HAL_DSI_ERROR_INT_RDBK_DATA_MULTI_ECC) != 0 )
      uErrorIntrEnableReg = HWIO_OUT_FLD(uErrorIntrEnableReg, DSI_0_ERR_INT_MASK0, RDBK_DATA_MULTI_ECC_ERR_MASK,    1); 

    /* Map single bit ECC Error in read packet to HW interrupts.    */
    if ( (uErrorSource & HAL_DSI_ERROR_INT_RDBK_DATA_ECC) != 0 )
      uErrorIntrEnableReg = HWIO_OUT_FLD(uErrorIntrEnableReg, DSI_0_ERR_INT_MASK0, RDBK_DATA_ECC_ERR_MASK,          1); 

    /* Enable the Error interrupts. */
    out_dword(uOffset + HWIO_DSI_0_ERR_INT_MASK0_ADDR, uErrorIntrEnableReg);
  }
}

/****************************************************************************

FUNCTION: HAL_DSI_GetErrorMaskEnabled()

Description:
   Get the DSI Error Mask enabled.  Each bit field is parsed and mapped
   to the equivalent hardware field.  The value in puErrorSource is NOT
   directly read from the hardware register

****************************************************************************/
void HAL_DSI_GetErrorMaskEnabled( DSI_Device_IDType       eDeviceID,
                                  uint32                 *puErrorSource)
{
  uint32  uErrorIntrEnableReg  = 0;
  uint32  uHalErrorSource      = HAL_DSI_INTERRUPT_NONE;
  uintPtr uOffset              = HAL_DSI_GetRegBaseOffset(eDeviceID);

  if (NULL == puErrorSource)
  {
    geDSIErrorCode      = HAL_DSI_FAILED_EXPECTED_NON_NULL_PTR;
  }
  else
  {
    /* Get the DSI Error HW interrupts enabled. */
    uErrorIntrEnableReg = in_dword(uOffset + HWIO_DSI_0_ERR_INT_MASK0_ADDR);

    /* Map PHY PLL unlock error interrupts to generic HAL interface. */
    if (0 != HWIO_GETVAL(DSI_0_ERR_INT_MASK0, uErrorIntrEnableReg, DPHY_DSIPLL_UNLOCK_MASK))  
      uHalErrorSource |= HAL_DSI_ERROR_INT_DPHY_DSIPLL_UNLOCK;

    /* Map Contention error while LN0 is driven high interrupts to generic HAL interface.*/
    if (0 != HWIO_GETVAL(DSI_0_ERR_INT_MASK0, uErrorIntrEnableReg, DLN0_ERR_CONTENTION_LP1_MASK))  
      uHalErrorSource |= HAL_DSI_ERROR_INT_DLN0_CONTENTION_LP1;

    /* Map Contention error while LN0 is driven low interrupts to generic HAL interface. */
    if (0 != HWIO_GETVAL(DSI_0_ERR_INT_MASK0, uErrorIntrEnableReg, DLN0_ERR_CONTENTION_LP0_MASK))  
      uHalErrorSource |= HAL_DSI_ERROR_INT_DLN0_CONTENTION_LP0;

    /* Map LP RX state sequence error interrupts to generic HAL interface. */
    if (0 != HWIO_GETVAL(DSI_0_ERR_INT_MASK0, uErrorIntrEnableReg, DLN0_ERR_CONTROL_MASK))  
      uHalErrorSource |= HAL_DSI_ERROR_INT_DLN0_CONTROL;

    /* Map LP RX data not byte aligned error interrupts to generic HAL interface. */
    if (0 != HWIO_GETVAL(DSI_0_ERR_INT_MASK0, uErrorIntrEnableReg, DLN0_ERR_SYNC_ESC_MASK))  
      uHalErrorSource |= HAL_DSI_ERROR_INT_DLN0_SYNC_ESC;

    /* Map LP RX Escape Entry error interrupts to generic HAL interface. */
    if (0 != HWIO_GETVAL(DSI_0_ERR_INT_MASK0, uErrorIntrEnableReg, DLN0_ERR_ESC_MASK))  
      uHalErrorSource |= HAL_DSI_ERROR_INT_DLN0_ESC;

    /* Map DL0 Lower Power FIFO error interrupts to generic HAL interface. */
    if (0 != HWIO_GETVAL(DSI_0_ERR_INT_MASK0, uErrorIntrEnableReg, DLN0_LP_FIFO_OVERFLOW_MASK))  
      uHalErrorSource |= HAL_DSI_ERROR_INT_DLN0_LP_FIFO_OVERFLOW;

    /* Map DL0 High Speed FIFO error interrupts to generic HAL interface. */
    if (0 != HWIO_GETVAL(DSI_0_ERR_INT_MASK0, uErrorIntrEnableReg, DLN0_HS_FIFO_OVERFLOW_MASK))  
      uHalErrorSource |= HAL_DSI_ERROR_INT_DLN0_HS_FIFO_OVERFLOW;

    /* Map DL1 High Speed FIFO error interrupts to generic HAL interface. */
    if (0 != HWIO_GETVAL(DSI_0_ERR_INT_MASK0, uErrorIntrEnableReg, DLN1_HS_FIFO_OVERFLOW_MASK))  
      uHalErrorSource |= HAL_DSI_ERROR_INT_DLN1_HS_FIFO_OVERFLOW;

    /* Map DL2 High Speed FIFO error interrupts to generic HAL interface. */
    if (0 != HWIO_GETVAL(DSI_0_ERR_INT_MASK0, uErrorIntrEnableReg, DLN2_HS_FIFO_OVERFLOW_MASK))  
      uHalErrorSource |= HAL_DSI_ERROR_INT_DLN2_HS_FIFO_OVERFLOW;

    /* Map DL3 High Speed FIFO error interrupts to generic HAL interface. */
    if (0 != HWIO_GETVAL(DSI_0_ERR_INT_MASK0, uErrorIntrEnableReg, DLN3_HS_FIFO_OVERFLOW_MASK))  
      uHalErrorSource |= HAL_DSI_ERROR_INT_DLN3_HS_FIFO_OVERFLOW;

    /* Map Video mode FIFO underflow Error interrupts to generic HAL interface. */
    if (0 != HWIO_GETVAL(DSI_0_ERR_INT_MASK0, uErrorIntrEnableReg, VID_MDP_FIFO_UNDERFLOW_MASK))  
      uHalErrorSource |= HAL_DSI_ERROR_INT_VID_MDP_FIFO_UNDERFLOW;

    /* Map Video mode FIFO overflow Error interrupts to generic HAL interface. */
    if (0 != HWIO_GETVAL(DSI_0_ERR_INT_MASK0, uErrorIntrEnableReg, VID_MDP_FIFO_OVERFLOW_MASK))  
      uHalErrorSource |= HAL_DSI_ERROR_INT_VID_MDP_FIFO_OVERFLOW;

    /* Map CMD mode MDP FIFO underflow Error interrupts to generic HAL interface. */
    if (0 != HWIO_GETVAL(DSI_0_ERR_INT_MASK0, uErrorIntrEnableReg, CMD_MDP_FIFO_UNDERFLOW_MASK))  
      uHalErrorSource |= HAL_DSI_ERROR_INT_CMD_MDP_FIFO_UNDERFLOW;

    /* Map CMD mode DMA FIFO underflow Error interrupts to generic HAL interface. */
    if (0 != HWIO_GETVAL(DSI_0_ERR_INT_MASK0, uErrorIntrEnableReg, CMD_DMA_FIFO_UNDERFLOW_MASK))  
      uHalErrorSource |= HAL_DSI_ERROR_INT_CMD_DMA_FIFO_UNDERFLOW;

    /* Map Interleave operation contention Error interrupts to generic HAL interface. */
    if (0 != HWIO_GETVAL(DSI_0_ERR_INT_MASK0, uErrorIntrEnableReg, INTERLEAVE_OP_CONTENTION_MASK))  
      uHalErrorSource |= HAL_DSI_ERROR_INT_INTERLEAVE_OP_CONTENTION;

    /* Map BTA timeout error interrupts to generic HAL interface. */
    if (0 != HWIO_GETVAL(DSI_0_ERR_INT_MASK0, uErrorIntrEnableReg, BTA_TO_MASK))  
      uHalErrorSource |= HAL_DSI_ERROR_INT_BTA_TO;

    /* Map HS forward transmission timeout error interrupts to generic HAL interface. */
    if (0 != HWIO_GETVAL(DSI_0_ERR_INT_MASK0, uErrorIntrEnableReg, HS_TX_TO_MASK))  
      uHalErrorSource |= HAL_DSI_ERROR_INT_HS_TX_TO;

    /* Map LP reverse transmission timeout received interrupts to generic HAL interface. */
    if (0 != HWIO_GETVAL(DSI_0_ERR_INT_MASK0, uErrorIntrEnableReg, LP_RX_TO_MASK))  
      uHalErrorSource |= HAL_DSI_ERROR_INT_LP_RX_TO;

    /* Map Error Packet returned from Panel interrupts to generic HAL interface. */
    if (0 != HWIO_GETVAL(DSI_0_ERR_INT_MASK0, uErrorIntrEnableReg, ERROR_PACKET_MASK))  
      uHalErrorSource |= HAL_DSI_ERROR_INT_PACKET;

    /* Map Incomplete read packet error interrupts to generic HAL interface. */
    if (0 != HWIO_GETVAL(DSI_0_ERR_INT_MASK0, uErrorIntrEnableReg, RDBK_INCOMPLETE_PACKET_ERR_MASK))  
      uHalErrorSource |= HAL_DSI_ERROR_INT_RDBK_INCOMPLETE_PACKET;

    /* Map CRC Error in read packet interrupts to generic HAL interface. */
    if (0 != HWIO_GETVAL(DSI_0_ERR_INT_MASK0, uErrorIntrEnableReg, RDBK_DATA_CRC_ERR_MASK))  
      uHalErrorSource |= HAL_DSI_ERROR_INT_RDBK_DATA_CRC;

    /* Map multi-bit ECC Error in read packet interrupts to generic HAL interface. */
    if (0 != HWIO_GETVAL(DSI_0_ERR_INT_MASK0, uErrorIntrEnableReg, RDBK_DATA_MULTI_ECC_ERR_MASK))  
      uHalErrorSource |= HAL_DSI_ERROR_INT_RDBK_DATA_MULTI_ECC;

    /* Map single bit ECC Error in read packet interrupts to generic HAL interface. */
    if (0 != HWIO_GETVAL(DSI_0_ERR_INT_MASK0, uErrorIntrEnableReg, RDBK_DATA_ECC_ERR_MASK))  
      uHalErrorSource |= HAL_DSI_ERROR_INT_RDBK_DATA_ECC;

    *puErrorSource  = uHalErrorSource;
  }

}

/****************************************************************************

HAL_DSI_GetHostErrorStatus()

Description:
   Get the DSI Host Error status.  Each bit field is parsed and mapped to the
   equivalent hardware field.  The value in puErrorCode is NOT directly read 
   from the hardware status register.

****************************************************************************/
void HAL_DSI_GetHostErrorStatus(DSI_Device_IDType   eDeviceID,
                                uint32             *puErrorStatus,
                                uint32             *puErrorCode)
{
  uint32  uHalErrorStatus            = HAL_DSI_INTERRUPT_NONE;
  uint32  uDL0PhyErrorIntrStatusReg  = HAL_DSI_INTERRUPT_NONE;
  uint32  uFifoErrorIntrStatusReg    = HAL_DSI_INTERRUPT_NONE;
  uint32  uTimeoutErrIntrStatusReg   = HAL_DSI_INTERRUPT_NONE;
  uint32  uAckErrIntrStatusReg       = HAL_DSI_INTERRUPT_NONE;
  uintPtr uOffset                    = HAL_DSI_GetRegBaseOffset(eDeviceID);

  /* Does the HAL support all of the DSI devices requested? */
  if ( (NULL == puErrorStatus)  ||
       (NULL == puErrorCode) )
  {
    geDSIErrorCode = HAL_DSI_FAILED_NO_HW_SUPPORT;
  }
  else
  {
    *puErrorCode                      = 0;

    /* TODO HAL_DSI_INTERRUPT_DPHY_DSIPLL_UNLOCK*/

    /**************************************************/
    /* Get the HW generated DL0 Phy Error interrupts. */
    /**************************************************/
    uDL0PhyErrorIntrStatusReg =  in_dword(uOffset + HWIO_DSI_0_DLN0_PHY_ERR_ADDR);

    /* Get Contention error while LN0 is driven high and map to generic HAL interface. */
    if (0 != HWIO_GETVAL(DSI_0_DLN0_PHY_ERR, uDL0PhyErrorIntrStatusReg, DLN0_ERR_CONTENTION_LP1))  
    {
      uHalErrorStatus |= HAL_DSI_ERROR_INT_DLN0_CONTENTION_LP1;
    }

    /* Get Contention error while LN0 is driven low and map to generic HAL interface. */
    if (0 != HWIO_GETVAL(DSI_0_DLN0_PHY_ERR, uDL0PhyErrorIntrStatusReg, DLN0_ERR_CONTENTION_LP0))  
    {
      uHalErrorStatus |= HAL_DSI_ERROR_INT_DLN0_CONTENTION_LP0;
    }

    /* Get LP RX state sequence error and map to generic HAL interface. */
    if (0 != HWIO_GETVAL(DSI_0_DLN0_PHY_ERR, uDL0PhyErrorIntrStatusReg, DLN0_ERR_CONTROL))  
    {
      uHalErrorStatus |= HAL_DSI_ERROR_INT_DLN0_CONTROL;
    }

    /* Get LP RX data not byte aligned error and map to generic HAL interface. */
    if (0 != HWIO_GETVAL(DSI_0_DLN0_PHY_ERR, uDL0PhyErrorIntrStatusReg, DLN0_ERR_SYNC_ESC))  
    {
      uHalErrorStatus |= HAL_DSI_ERROR_INT_DLN0_SYNC_ESC;
    }

    /* Get LP RX Escape Entry error and map to generic HAL interface. */
    if (0 != HWIO_GETVAL(DSI_0_DLN0_PHY_ERR, uDL0PhyErrorIntrStatusReg, DLN0_ERR_ESC))  
    {
      uHalErrorStatus |= HAL_DSI_ERROR_INT_DLN0_ESC;
    }

    /**************************************************/
    /* Get the HW generated DSI FIFO Error interrupts.*/
    /**************************************************/
    uFifoErrorIntrStatusReg =  in_dword(uOffset + HWIO_DSI_0_FIFO_STATUS_ADDR);

    /* Get DL0 Lower Power FIFO overflow error and map to generic HAL interface. */
    if (0 != HWIO_GETVAL(DSI_0_FIFO_STATUS, uFifoErrorIntrStatusReg, DLN0_LP_FIFO_OVERFLOW))  
    {
      uHalErrorStatus |= HAL_DSI_ERROR_INT_DLN0_LP_FIFO_OVERFLOW;
    }

    /* Get DL0 High Speed FIFO overflow error and map to generic HAL interface. */
    if (0 != HWIO_GETVAL(DSI_0_FIFO_STATUS, uFifoErrorIntrStatusReg, DLN0_HS_FIFO_OVERFLOW))  
    {
      uHalErrorStatus |= HAL_DSI_ERROR_INT_DLN0_HS_FIFO_OVERFLOW;
    }

    /* Get DL1 High Speed FIFO overflow error  and map to generic HAL interface. */
    if (0 != HWIO_GETVAL(DSI_0_FIFO_STATUS, uFifoErrorIntrStatusReg, DLN1_HS_FIFO_OVERFLOW))  
    {
      uHalErrorStatus |= HAL_DSI_ERROR_INT_DLN1_HS_FIFO_OVERFLOW;
    }

    /* Get DL2 Lower Power FIFO overflow error and map to generic HAL interface. */
    if (0 != HWIO_GETVAL(DSI_0_FIFO_STATUS, uFifoErrorIntrStatusReg, DLN2_HS_FIFO_OVERFLOW))  
    {
      uHalErrorStatus |= HAL_DSI_ERROR_INT_DLN2_HS_FIFO_OVERFLOW;
    }

    /* Get DL3 Lower Power FIFO overflow error and map to generic HAL interface. */
    if (0 != HWIO_GETVAL(DSI_0_FIFO_STATUS, uFifoErrorIntrStatusReg, DLN3_HS_FIFO_OVERFLOW))  
    {
      uHalErrorStatus |= HAL_DSI_ERROR_INT_DLN3_HS_FIFO_OVERFLOW;
    }

    /* Get Video mode FIFO underflow Error and map to generic HAL interface. */
    if (0 != HWIO_GETVAL(DSI_0_FIFO_STATUS, uFifoErrorIntrStatusReg, VIDEO_MDP_FIFO_UNDERFLOW))  
    {
      uHalErrorStatus |= HAL_DSI_ERROR_INT_VID_MDP_FIFO_UNDERFLOW;
    }

    /* Get Video mode FIFO overflow Error and map to generic HAL interface. */
    if (0 != HWIO_GETVAL(DSI_0_FIFO_STATUS, uFifoErrorIntrStatusReg, VIDEO_MDP_FIFO_OVERFLOW))  
    {
      uHalErrorStatus |= HAL_DSI_ERROR_INT_VID_MDP_FIFO_OVERFLOW;
    }

    /* Get CMD mode MDP FIFO underflow Error and map to generic HAL interface. */
    if (0 != HWIO_GETVAL(DSI_0_FIFO_STATUS, uFifoErrorIntrStatusReg, CMD_MDP_FIFO_UNDERFLOW))  
    {
      uHalErrorStatus |= HAL_DSI_ERROR_INT_CMD_MDP_FIFO_UNDERFLOW;
    }

    /* Get CMD mode DMA FIFO underflow Error and map to generic HAL interface. */
    if (0 != HWIO_GETVAL(DSI_0_FIFO_STATUS, uFifoErrorIntrStatusReg, CMD_DMA_FIFO_UNDERFLOW))  
    {
      uHalErrorStatus |= HAL_DSI_ERROR_INT_CMD_DMA_FIFO_UNDERFLOW;
    }

    /* TODO HAL_DSI_ERROR_INT_INTERLEAVE_OP_CONTENTION*/

    /**************************************************/
    /* Get the HW generated Timeout Error interrupts. */
    /**************************************************/
    uTimeoutErrIntrStatusReg =  in_dword(uOffset + HWIO_DSI_0_TIMEOUT_STATUS_ADDR);

    /* Get BTA timeout error and map to generic HAL interface. */
    if (0 != HWIO_GETVAL(DSI_0_TIMEOUT_STATUS, uTimeoutErrIntrStatusReg, BTA_TIMEOUT))  
    {
      uHalErrorStatus |= HAL_DSI_ERROR_INT_BTA_TO;
    }

    /* Get BHS forward transmission timeout error and map to generic HAL interface. */
    if (0 != HWIO_GETVAL(DSI_0_TIMEOUT_STATUS, uTimeoutErrIntrStatusReg, HS_TX_TIMEOUT))  
    {
      uHalErrorStatus |= HAL_DSI_ERROR_INT_HS_TX_TO;
    }

    /* Get LP reverse transmission timeout received and map to generic HAL interface. */
    if (0 != HWIO_GETVAL(DSI_0_TIMEOUT_STATUS, uTimeoutErrIntrStatusReg, LP_RX_TIMEOUT))  
    {
      uHalErrorStatus |= HAL_DSI_ERROR_INT_LP_RX_TO;
    }
    /**************************************************/
    /* Get the HW generated Ack Error interrupts. */
    /**************************************************/
    uAckErrIntrStatusReg =  in_dword(uOffset + HWIO_DSI_0_ACK_ERR_STATUS_ADDR);

    /* Get  Error Packet returned from Panel and map to generic HAL interface. */
    if (0 != HWIO_GETVAL(DSI_0_ACK_ERR_STATUS, uAckErrIntrStatusReg, ERROR_REPORT_DETECTED))  
    {
      uHalErrorStatus |= HAL_DSI_ERROR_INT_PACKET;
    }

    /* Get Incomplete read packet error and map to generic HAL interface. */
    if (0 != HWIO_GETVAL(DSI_0_ACK_ERR_STATUS, uAckErrIntrStatusReg, RDBK_INCOMPLETE_PACKET_ERR))  
    {
      uHalErrorStatus |= HAL_DSI_ERROR_INT_RDBK_INCOMPLETE_PACKET;
    }

    /* Get CRC Error in read packet and map to generic HAL interface. */
    if (0 != HWIO_GETVAL(DSI_0_ACK_ERR_STATUS, uAckErrIntrStatusReg, RDBK_DATA_CRC_ERR))  
    {
      uHalErrorStatus |= HAL_DSI_ERROR_INT_RDBK_DATA_CRC;
    }

    /* Get multi-bit ECC Error and map to generic HAL interface. */
    if (0 != HWIO_GETVAL(DSI_0_ACK_ERR_STATUS, uAckErrIntrStatusReg, RDBK_DATA_MULTI_ECC_ERR))  
    {
      uHalErrorStatus |= HAL_DSI_ERROR_INT_RDBK_DATA_MULTI_ECC;
    }

    /* Get single bit ECC Error in read packet and map to generic HAL interface. */
    if (0 != HWIO_GETVAL(DSI_0_ACK_ERR_STATUS, uAckErrIntrStatusReg, RDBK_DATA_ECC_ERR))  
    {
      uHalErrorStatus |= HAL_DSI_ERROR_INT_RDBK_DATA_ECC;
    }

    *puErrorStatus  = uHalErrorStatus;
  }

}

/****************************************************************************

HAL_DSI_ClrHostErrorStatus()

Description:
   Clear the DSI Host Error status register. Each bit field is parsed 
   and mapped to the equivalent hardware field. The value in uErrorStatus
    is NOT directly used to program the hardware status register.

****************************************************************************/
void HAL_DSI_ClrHostErrorStatus(DSI_Device_IDType     eDeviceID,
                                uint32                uErrorStatus)
{
  uint32  uDL0PhyErrorIntrStatusReg  = 0;
  uint32  uFifoErrorIntrStatusReg    = 0;
  uint32  uTimeoutErrIntrStatusReg   = 0;
  uint32  uAckErrIntrStatusReg       = 0;
  uintPtr uOffset                    = HAL_DSI_GetRegBaseOffset(eDeviceID);

  /* TODO HAL_DSI_INTERRUPT_DPHY_DSIPLL_UNLOCK*/

  /************************************************************************/
  /* Clear the HW generated DL0 Phy Error interrupts.                         */
  /************************************************************************/
  /* Get Contention error while LN0 is driven high and map to generic HAL interface. */
  if ( (uErrorStatus & HAL_DSI_ERROR_INT_DLN0_CONTENTION_LP1) != 0)
  {
    uDL0PhyErrorIntrStatusReg = HWIO_OUT_FLD(uDL0PhyErrorIntrStatusReg, DSI_0_DLN0_PHY_ERR, DLN0_ERR_CONTENTION_LP1_CLR,    1); 
  }

  /* Get Contention error while LN0 is driven low and map to generic HAL interface. */
  if ( (uErrorStatus & HAL_DSI_ERROR_INT_DLN0_CONTENTION_LP0) != 0)
  {
    uDL0PhyErrorIntrStatusReg = HWIO_OUT_FLD(uDL0PhyErrorIntrStatusReg, DSI_0_DLN0_PHY_ERR, DLN0_ERR_CONTENTION_LP0_CLR,    1); 
  }

  /* Get LP RX state sequence error and map to generic HAL interface. */
  if ( (uErrorStatus & HAL_DSI_ERROR_INT_DLN0_CONTROL) != 0)
  {
    uDL0PhyErrorIntrStatusReg = HWIO_OUT_FLD(uDL0PhyErrorIntrStatusReg, DSI_0_DLN0_PHY_ERR, DLN0_ERR_CONTROL_CLR,     1); 
  }

  /* Get LP RX data not byte aligned error and map to generic HAL interface. */
  if ( (uErrorStatus & HAL_DSI_ERROR_INT_DLN0_SYNC_ESC) != 0)
  {
    uDL0PhyErrorIntrStatusReg = HWIO_OUT_FLD(uDL0PhyErrorIntrStatusReg, DSI_0_DLN0_PHY_ERR, DLN0_ERR_SYNC_ESC_CLR,   1); 
  }

  /* Get LP RX Escape Entry error and map to generic HAL interface. */
  if ( (uErrorStatus & HAL_DSI_ERROR_INT_DLN0_ESC) != 0)
  {
    uDL0PhyErrorIntrStatusReg = HWIO_OUT_FLD(uDL0PhyErrorIntrStatusReg, DSI_0_DLN0_PHY_ERR, DLN0_ERR_ESC_CLR,        1); 
  }

  /************************************************************************/
  /* Clear the HW generated DSI FIFO Error interrupts.                    */
  /************************************************************************/

  /* Get DL0 Lower Power FIFO overflow error and map to generic HAL interface. */
  if ( (uErrorStatus & HAL_DSI_ERROR_INT_DLN0_LP_FIFO_OVERFLOW) != 0)
  {
    uFifoErrorIntrStatusReg = HWIO_OUT_FLD(uFifoErrorIntrStatusReg, DSI_0_FIFO_STATUS, DLN0_LP_FIFO_OVERFLOW_CLR,    1); 
  }

  /* Get DL0 High Speed FIFO overflow error and map to generic HAL interface. */
  if ( (uErrorStatus & HAL_DSI_ERROR_INT_DLN0_HS_FIFO_OVERFLOW) != 0)
  {
    uFifoErrorIntrStatusReg = HWIO_OUT_FLD(uFifoErrorIntrStatusReg, DSI_0_FIFO_STATUS, DLN0_HS_FIFO_OVERFLOW_CLR,    1); 
  }

  /* Get DL1 High Speed FIFO overflow error  and map to generic HAL interface. */
  if ( (uErrorStatus & HAL_DSI_ERROR_INT_DLN1_HS_FIFO_OVERFLOW) != 0)
  {
    uFifoErrorIntrStatusReg = HWIO_OUT_FLD(uFifoErrorIntrStatusReg, DSI_0_FIFO_STATUS, DLN1_HS_FIFO_OVERFLOW_CLR,    1); 
  }

  /* Get DL2 Lower Power FIFO overflow error and map to generic HAL interface. */
  if ( (uErrorStatus & HAL_DSI_ERROR_INT_DLN2_HS_FIFO_OVERFLOW) != 0)
  {
    uFifoErrorIntrStatusReg = HWIO_OUT_FLD(uFifoErrorIntrStatusReg, DSI_0_FIFO_STATUS, DLN2_HS_FIFO_OVERFLOW_CLR,    1); 
  }

  /* Get DL3 Lower Power FIFO overflow error and map to generic HAL interface. */
  if ( (uErrorStatus & HAL_DSI_ERROR_INT_DLN3_HS_FIFO_OVERFLOW) != 0)
  {
    uFifoErrorIntrStatusReg = HWIO_OUT_FLD(uFifoErrorIntrStatusReg, DSI_0_FIFO_STATUS, DLN3_HS_FIFO_OVERFLOW_CLR,    1); 
  }

  /* Get Video mode FIFO underflow Error and map to generic HAL interface. */
  if ( (uErrorStatus & HAL_DSI_ERROR_INT_VID_MDP_FIFO_UNDERFLOW) != 0)
  {
    uFifoErrorIntrStatusReg = HWIO_OUT_FLD(uFifoErrorIntrStatusReg, DSI_0_FIFO_STATUS, VIDEO_MDP_FIFO_UNDERFLOW_CLR, 1); 
  }

  /* Get Video mode FIFO overflow Error and map to generic HAL interface. */
  if ( (uErrorStatus & HAL_DSI_ERROR_INT_VID_MDP_FIFO_OVERFLOW) != 0)
  {
    uFifoErrorIntrStatusReg = HWIO_OUT_FLD(uFifoErrorIntrStatusReg, DSI_0_FIFO_STATUS, VIDEO_MDP_FIFO_OVERFLOW_CLR,  1); 
  }

  /* Get CMD mode MDP FIFO underflow Error and map to generic HAL interface. */
  if ( (uErrorStatus & HAL_DSI_ERROR_INT_CMD_MDP_FIFO_UNDERFLOW) != 0)
  {
    uFifoErrorIntrStatusReg = HWIO_OUT_FLD(uFifoErrorIntrStatusReg, DSI_0_FIFO_STATUS, CMD_MDP_FIFO_UNDERFLOW_CLR,   1); 
  }

  /* Get CMD mode DMA FIFO underflow Error and map to generic HAL interface. */
  if ( (uErrorStatus & HAL_DSI_ERROR_INT_CMD_DMA_FIFO_UNDERFLOW) != 0)
  {
    uFifoErrorIntrStatusReg = HWIO_OUT_FLD(uFifoErrorIntrStatusReg, DSI_0_FIFO_STATUS, CMD_DMA_FIFO_UNDERFLOW_CLR,   1); 
  }
  /************************************************************************/
  /* Clear the HW generated Timeout Error interrupts.                         */
  /************************************************************************/

  /* Get BTA timeout error and map to generic HAL interface. */
  if ( (uErrorStatus & HAL_DSI_ERROR_INT_BTA_TO) != 0)
  {
    uTimeoutErrIntrStatusReg = HWIO_OUT_FLD(uTimeoutErrIntrStatusReg, DSI_0_TIMEOUT_STATUS, BTA_TIMEOUT_CLR,      1); 
  }

  /* Get BHS forward transmission timeout error and map to generic HAL interface. */
  if ( (uErrorStatus & HAL_DSI_ERROR_INT_HS_TX_TO) != 0)
  {
    uTimeoutErrIntrStatusReg = HWIO_OUT_FLD(uTimeoutErrIntrStatusReg, DSI_0_TIMEOUT_STATUS, HS_TX_TIMEOUT_CLR,    1); 
  }

  /* Get LP reverse transmission timeout received and map to generic HAL interface. */
  if ( (uErrorStatus & HAL_DSI_ERROR_INT_LP_RX_TO) != 0)
  {
    uTimeoutErrIntrStatusReg = HWIO_OUT_FLD(uTimeoutErrIntrStatusReg, DSI_0_TIMEOUT_STATUS, LP_RX_TIMEOUT_CLR,    1); 
  }

  /************************************************************************/
  /* Clear the HW generated Ack Error interrupts.                         */
  /************************************************************************/
  /* Get  Error Packet returned from Panel and map to generic HAL interface. */
  if ( (uErrorStatus & HAL_DSI_ERROR_INT_PACKET) != 0)
  {
    uAckErrIntrStatusReg = HWIO_OUT_FLD(uAckErrIntrStatusReg, DSI_0_ACK_ERR_STATUS, ERROR_REPORT_DETECTED_CLR,    1); 
  }

  /* Get Incomplete read packet error and map to generic HAL interface. */
  if ( (uErrorStatus & HAL_DSI_ERROR_INT_RDBK_INCOMPLETE_PACKET) != 0)
  {
    uAckErrIntrStatusReg = HWIO_OUT_FLD(uAckErrIntrStatusReg, DSI_0_ACK_ERR_STATUS, RDBK_INCOMPLETE_PACKET_ERR_CLR,    1); 
  }

  /* Get CRC Error in read packet and map to generic HAL interface. */
  if ( (uErrorStatus & HAL_DSI_ERROR_INT_RDBK_DATA_CRC) != 0)
  {
    uAckErrIntrStatusReg = HWIO_OUT_FLD(uAckErrIntrStatusReg, DSI_0_ACK_ERR_STATUS, RDBK_DATA_CRC_ERR_CLR,    1); 
  }

  /* Get multi-bit ECC Error and map to generic HAL interface. */
  if ( (uErrorStatus & HAL_DSI_ERROR_INT_RDBK_DATA_MULTI_ECC) != 0)
  {
    uAckErrIntrStatusReg = HWIO_OUT_FLD(uAckErrIntrStatusReg, DSI_0_ACK_ERR_STATUS, RDBK_DATA_MULTI_ECC_ERR_CLR,    1); 
  }

  /* Get single bit ECC Error in read packet and map to generic HAL interface. */
  if ( (uErrorStatus & HAL_DSI_ERROR_INT_RDBK_DATA_ECC) != 0)
  {
    uAckErrIntrStatusReg = HWIO_OUT_FLD(uAckErrIntrStatusReg, DSI_0_ACK_ERR_STATUS, RDBK_DATA_ECC_ERR_CLR,    1); 
  }

  if( uDL0PhyErrorIntrStatusReg != 0)
  {
    out_dword(uOffset + HWIO_DSI_0_DLN0_PHY_ERR_ADDR, uDL0PhyErrorIntrStatusReg );
  }

  if( uFifoErrorIntrStatusReg != 0)
  {
    out_dword(uOffset + HWIO_DSI_0_FIFO_STATUS_ADDR, uFifoErrorIntrStatusReg );
  }

  if( uTimeoutErrIntrStatusReg != 0)
  {
    out_dword(uOffset + HWIO_DSI_0_TIMEOUT_STATUS_ADDR, uTimeoutErrIntrStatusReg );
  }

  if( uAckErrIntrStatusReg != 0)
  {
    /* need to write '1' and then '0' to clear the error */
    uAckErrIntrStatusReg &= in_dword(uOffset + HWIO_DSI_0_ACK_ERR_STATUS_ADDR);
    out_dword(uOffset + HWIO_DSI_0_ACK_ERR_STATUS_ADDR, uAckErrIntrStatusReg);
    out_dword(uOffset + HWIO_DSI_0_ACK_ERR_STATUS_ADDR, 0);
  }

}

/****************************************************************************

HAL_DSI_GetPanelErrorStatus()

Description:
   This API query the ACK Error Status of DSI Controller from Panels.

****************************************************************************/
void HAL_DSI_GetPanelErrorStatus( DSI_Device_IDType     eDeviceID,
                                  uint32               *puErrorStatus)
{
  uint32  uDsiAckErrorStatusReg  = 0;
  uint32  uDsiError              = 0;
  uintPtr uOffset                = HAL_DSI_GetRegBaseOffset(eDeviceID);

  /* Check parameters */
  if(NULL == puErrorStatus)
  {
    geDSIErrorCode = HAL_DSI_FAILED_INVALID_INPUT_PARAMETER;
  }
  else
  {
    *puErrorStatus              = 0;

    /* Get the HW generated DSI Error Status. */
    uDsiAckErrorStatusReg = in_dword(uOffset + HWIO_DSI_0_ACK_ERR_STATUS_ADDR);

    /* Panel Specific Error  */
    if (0 != HWIO_GETVAL(DSI_0_ACK_ERR_STATUS, uDsiAckErrorStatusReg, PANEL_SPECIFIC_ERR))  
      uDsiError |= HAL_DSI_ACK_ERROR_PANEL_SPECIFIC_ERR;
    /* Protocol Violation Error */
    if (0 != HWIO_GETVAL(DSI_0_ACK_ERR_STATUS, uDsiAckErrorStatusReg, PROTOCOL_VIOLATION))  
      uDsiError |= HAL_DSI_ACK_ERROR_PROTOCOL_VIOLATION;
    /* VC Error */
    if (0 != HWIO_GETVAL(DSI_0_ACK_ERR_STATUS, uDsiAckErrorStatusReg, VC_ERR))  
      uDsiError |= HAL_DSI_ACK_ERROR_VC_ERR;
    /* DT Error */
    if (0 != HWIO_GETVAL(DSI_0_ACK_ERR_STATUS, uDsiAckErrorStatusReg, DT_ERR))  
      uDsiError |= HAL_DSI_ACK_ERROR_DT_ERR;
    /* CRC Error */
    if (0 != HWIO_GETVAL(DSI_0_ACK_ERR_STATUS, uDsiAckErrorStatusReg, CRC_ERR))  
      uDsiError |= HAL_DSI_ACK_ERROR_CRC_ERR;
    /* Multi-bit ECC Error */
    if (0 != HWIO_GETVAL(DSI_0_ACK_ERR_STATUS, uDsiAckErrorStatusReg, MULTI_ECC_ERR))  
      uDsiError |= HAL_DSI_ACK_ERROR_MULTI_ECC_ERR;
    /* Signal Bit ECC Error */
    if (0 != HWIO_GETVAL(DSI_0_ACK_ERR_STATUS, uDsiAckErrorStatusReg, ECC_ERR))  
      uDsiError |= HAL_DSI_ACK_ERROR_SINGLE_ECC_ERR;
    /* Contention Error */
    if (0 != HWIO_GETVAL(DSI_0_ACK_ERR_STATUS, uDsiAckErrorStatusReg, CONTENTION))  
      uDsiError |= HAL_DSI_ACK_ERROR_CONTENTION;
    /* False Control Error */
    if (0 != HWIO_GETVAL(DSI_0_ACK_ERR_STATUS, uDsiAckErrorStatusReg, FALSE_CTRL_ERR))  
      uDsiError |= HAL_DSI_ACK_ERROR_FALSE_CTRL_ERR;
    /* Time Out Error */
    if (0 != HWIO_GETVAL(DSI_0_ACK_ERR_STATUS, uDsiAckErrorStatusReg, TIMEOUT))  
      uDsiError |= HAL_DSI_ACK_ERROR_TIMEOUT;
    /* LP Error */
    if (0 != HWIO_GETVAL(DSI_0_ACK_ERR_STATUS, uDsiAckErrorStatusReg, LP_ERR))  
      uDsiError |= HAL_DSI_ACK_ERROR_LP_ERR;
    /* ESC Error */
    if (0 != HWIO_GETVAL(DSI_0_ACK_ERR_STATUS, uDsiAckErrorStatusReg, ESC_ERR))  
      uDsiError |= HAL_DSI_ACK_ERROR_ESC_ERR;
    /* EOT Error */
    if (0 != HWIO_GETVAL(DSI_0_ACK_ERR_STATUS, uDsiAckErrorStatusReg, EOT_ERR))  
      uDsiError |= HAL_DSI_ACK_ERROR_EOT_ERR;
    /* SOT Sync Error */
    if (0 != HWIO_GETVAL(DSI_0_ACK_ERR_STATUS, uDsiAckErrorStatusReg, SOT_SYNC_ERR))  
      uDsiError |= HAL_DSI_ACK_ERROR_SOT_SYNC_ERR;
    /* SOT Error */
    if (0 != HWIO_GETVAL(DSI_0_ACK_ERR_STATUS, uDsiAckErrorStatusReg, SOT_ERR))  
      uDsiError |= HAL_DSI_ACK_ERROR_SOT_ERR;

    /* DSI Error Status */
    *puErrorStatus = uDsiError;
  }
}

/****************************************************************************

HAL_DSI_ClrPanelErrorStatus()

Description:
   Clear the ACK Error Status register. Each bit field is parsed and mapped 
   to the equivalent hardware field. The value in uErrorStatus is NOT directly 
   used to program the hardware status register.
  
****************************************************************************/
void HAL_DSI_ClrPanelErrorStatus(DSI_Device_IDType    eDeviceID,
                                 uint32               uErrorStatus)
{
  uint32  uDsiAckErrorStatusReg  = 0;
  uintPtr uOffset                = HAL_DSI_GetRegBaseOffset(eDeviceID);

  /* Panel Specific Error  */
  if((uErrorStatus & HAL_DSI_ACK_ERROR_PANEL_SPECIFIC_ERR) != 0)
    uDsiAckErrorStatusReg = HWIO_OUT_FLD(uDsiAckErrorStatusReg, DSI_0_ACK_ERR_STATUS, PANEL_SPECIFIC_ERR, 1); 
  /* Protocol Violation Error */
  if((uErrorStatus & HAL_DSI_ACK_ERROR_PROTOCOL_VIOLATION) != 0)
    uDsiAckErrorStatusReg = HWIO_OUT_FLD(uDsiAckErrorStatusReg, DSI_0_ACK_ERR_STATUS, PROTOCOL_VIOLATION, 1); 
  /* VC Error */
  if((uErrorStatus & HAL_DSI_ACK_ERROR_VC_ERR) != 0)
    uDsiAckErrorStatusReg = HWIO_OUT_FLD(uDsiAckErrorStatusReg, DSI_0_ACK_ERR_STATUS, VC_ERR, 1); 
  /* DT Error */
  if((uErrorStatus & HAL_DSI_ACK_ERROR_DT_ERR) != 0)
    uDsiAckErrorStatusReg = HWIO_OUT_FLD(uDsiAckErrorStatusReg, DSI_0_ACK_ERR_STATUS, DT_ERR, 1); 
  /* CRC Error */
  if((uErrorStatus & HAL_DSI_ACK_ERROR_CRC_ERR) != 0)
    uDsiAckErrorStatusReg = HWIO_OUT_FLD(uDsiAckErrorStatusReg, DSI_0_ACK_ERR_STATUS, CRC_ERR, 1); 
  /* Multi-bit ECC Error */
  if((uErrorStatus & HAL_DSI_ACK_ERROR_MULTI_ECC_ERR) != 0)
    uDsiAckErrorStatusReg = HWIO_OUT_FLD(uDsiAckErrorStatusReg, DSI_0_ACK_ERR_STATUS, MULTI_ECC_ERR, 1); 
  /* Signal Bit ECC Error */
  if((uErrorStatus & HAL_DSI_ACK_ERROR_SINGLE_ECC_ERR) != 0)
    uDsiAckErrorStatusReg = HWIO_OUT_FLD(uDsiAckErrorStatusReg, DSI_0_ACK_ERR_STATUS, ECC_ERR, 1); 
  /* Contention Error */
  if((uErrorStatus & HAL_DSI_ACK_ERROR_CONTENTION) != 0)
    uDsiAckErrorStatusReg = HWIO_OUT_FLD(uDsiAckErrorStatusReg, DSI_0_ACK_ERR_STATUS, CONTENTION, 1); 
  /* False Control Error */
  if((uErrorStatus & HAL_DSI_ACK_ERROR_FALSE_CTRL_ERR) != 0)
    uDsiAckErrorStatusReg = HWIO_OUT_FLD(uDsiAckErrorStatusReg, DSI_0_ACK_ERR_STATUS, FALSE_CTRL_ERR, 1); 
  /* Time Out Error */
  if((uErrorStatus & HAL_DSI_ACK_ERROR_TIMEOUT) != 0)
    uDsiAckErrorStatusReg = HWIO_OUT_FLD(uDsiAckErrorStatusReg, DSI_0_ACK_ERR_STATUS, TIMEOUT, 1); 
  /* LP Error */
  if((uErrorStatus & HAL_DSI_ACK_ERROR_LP_ERR) != 0)
    uDsiAckErrorStatusReg = HWIO_OUT_FLD(uDsiAckErrorStatusReg, DSI_0_ACK_ERR_STATUS, LP_ERR, 1); 
  /* ESC Error */
  if((uErrorStatus & HAL_DSI_ACK_ERROR_ESC_ERR) != 0)
    uDsiAckErrorStatusReg = HWIO_OUT_FLD(uDsiAckErrorStatusReg, DSI_0_ACK_ERR_STATUS, ESC_ERR, 1); 
  /* EOT Error */
  if((uErrorStatus & HAL_DSI_ACK_ERROR_EOT_ERR) != 0)
    uDsiAckErrorStatusReg = HWIO_OUT_FLD(uDsiAckErrorStatusReg, DSI_0_ACK_ERR_STATUS, EOT_ERR, 1); 
  /* SOT Sync Error */
  if((uErrorStatus & HAL_DSI_ACK_ERROR_SOT_SYNC_ERR) != 0)
    uDsiAckErrorStatusReg = HWIO_OUT_FLD(uDsiAckErrorStatusReg, DSI_0_ACK_ERR_STATUS, SOT_SYNC_ERR, 1); 
  /* SOT Error */
  if((uErrorStatus & HAL_DSI_ACK_ERROR_SOT_ERR) != 0)
    uDsiAckErrorStatusReg = HWIO_OUT_FLD(uDsiAckErrorStatusReg, DSI_0_ACK_ERR_STATUS, SOT_ERR, 1); 

  if(0 != uDsiAckErrorStatusReg )
  {
    /* need to write '1' and then '0' to clear the error */
    uDsiAckErrorStatusReg &= in_dword(uOffset + HWIO_DSI_0_ACK_ERR_STATUS_ADDR);
    out_dword(uOffset + HWIO_DSI_0_ACK_ERR_STATUS_ADDR, uDsiAckErrorStatusReg );
    out_dword(uOffset + HWIO_DSI_0_ACK_ERR_STATUS_ADDR, 0);
  }
}

#ifdef __cplusplus
}
#endif
