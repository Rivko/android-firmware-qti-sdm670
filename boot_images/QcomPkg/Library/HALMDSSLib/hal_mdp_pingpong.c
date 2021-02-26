/*! \file */

/*
===========================================================================

FILE:         hal_mdp_pingpong.c

DESCRIPTION: Manages the MDP (Display) Pingpong blocks

===========================================================================
  Copyright (c) 2012-2017 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
===========================================================================
*/

#include "hal_mdp_i.h"

#ifdef __cplusplus
extern "C" {
#endif


/****************************************************************************
*
** FUNCTION: HAL_MDP_Get_PingPong_BaseOffset()
*/
/*!
* \brief
*     Returns offset of the pingpong block
*
* \param [in] ePingPongId    - pingpong block ID (none, 0, 1, 2)
*
* \retval uintPtr
*
****************************************************************************/
static uintPtr HAL_MDP_Get_PingPong_BaseOffset(HAL_MDP_PingPongId  ePingPongId)
{
   uintPtr   uRegOffset  = 0x0;

   if((HAL_MDP_PINGPONG_NONE        == ePingPongId) ||
      (HAL_MDP_PINGPONG_MAX         <= ePingPongId))
   {
      HAL_MDP_ASSERT();
   }
   else
   {
      uRegOffset = uMDPPingPongRegBaseOffset[ePingPongId];
   }
   return uRegOffset;
}


/****************************************************************************
*
** FUNCTION: HAL_MDP_PingPong_Vsync_Config()
*/
/*!
* \brief
*     Set up vsync count for given pingpong block
*
* \param [in]   ePingPongId       - Pingpong block id
* \param [in]   pPingPongConfig   - Configuration information
* \param [in]   uSetupFlags       - Reserved
*
* \retval NONE
*
****************************************************************************/
static void HAL_MDP_PingPong_Vsync_Config(HAL_MDP_PingPongId                    ePingPongId,
                                          HAL_MDP_PingPong_VsyncConfigType      *pPingPongConfig,
                                          uint32                                uSetupFlags)
{
   uintPtr      uRegOffset              = 0x0;
   uint32       uRegVal                 = 0x00000000;

   // get register base offset
   uRegOffset = HAL_MDP_Get_PingPong_BaseOffset(ePingPongId);

   /*========================================================================
   * (1) enable auto-refresh
   *========================================================================*/
   if(TRUE == pPingPongConfig->bEnableAutoRefresh)
   {
      //Actual Refresh Rate = uMdpRefreshrate/uAutoRefreshFrameNumber
      uRegVal = HWIO_OUT_FLD(0,       MDP_PP_0_AUTOREFRESH_CONFIG, ENABLE,      1);
      uRegVal = HWIO_OUT_FLD(uRegVal, MDP_PP_0_AUTOREFRESH_CONFIG, FRAME_COUNT, pPingPongConfig->uAutoRefreshFrameNumDiv);
      out_dword(HWIO_MDP_PP_0_AUTOREFRESH_CONFIG_ADDR + uRegOffset, uRegVal);
   }
   else
   {
      out_dword(HWIO_MDP_PP_0_AUTOREFRESH_CONFIG_ADDR + uRegOffset, 0);
   }

   /*========================================================================
   * (2) program vsync counter
   *========================================================================*/
   uRegVal = in_dword(HWIO_MDP_PP_0_SYNC_CONFIG_VSYNC_ADDR + uRegOffset);
   // VSYNC_COUNT
   uRegVal = HWIO_OUT_FLD(uRegVal, MDP_PP_0_SYNC_CONFIG_VSYNC, VSYNC_COUNT, pPingPongConfig->uVsyncCount);
   out_dword(HWIO_MDP_PP_0_SYNC_CONFIG_VSYNC_ADDR + uRegOffset, uRegVal);

   // VSYNC COUNT HEIGHT
   uRegVal = HWIO_OUT_FLD(0, MDP_PP_0_SYNC_CONFIG_HEIGHT, HEIGHT, pPingPongConfig->uVsyncHeight);
   out_dword(HWIO_MDP_PP_0_SYNC_CONFIG_HEIGHT_ADDR + uRegOffset, uRegVal);

   /*========================================================================
   * (3) program read-pointer IRQ line
   *========================================================================*/
   uRegVal = HWIO_OUT_FLD(0, MDP_PP_0_RD_PTR_IRQ, IRQ_LINE, pPingPongConfig->uVsyncRdPtrIrqLine);
   out_dword(HWIO_MDP_PP_0_RD_PTR_IRQ_ADDR + uRegOffset, uRegVal);

   /*========================================================================
   * (4) program write-pointer IRQ line
   *========================================================================*/
   uRegVal = HWIO_OUT_FLD(0, MDP_PP_0_WR_PTR_IRQ, IRQ_LINE, pPingPongConfig->uVsyncWrPtrIrqLine);
   out_dword(HWIO_MDP_PP_0_WR_PTR_IRQ_ADDR + uRegOffset, uRegVal);

   /*========================================================================
   * (5) program vsync init value
   *========================================================================*/
   out_dword(HWIO_MDP_PP_0_VSYNC_INIT_VAL_ADDR + uRegOffset, pPingPongConfig->uVsyncInitValue);
}

/****************************************************************************
*
** FUNCTION: HAL_MDP_PingPong_TE_Config()
*/
/*!
* \brief
*     Set up tear check for given pingpong block
*
* \param [in]   ePingPongId      - Pingpong block id
* \param [in]   pPingPongConfig  - Configuration information
* \param [in]   uSetupFlags      - Reserved
*
* \retval NONE
*
****************************************************************************/
static void HAL_MDP_PingPong_TE_Config(HAL_MDP_PingPongId                   ePingPongId,
                                       HAL_MDP_PingPong_TEConfigType        *pPingPongConfig,
                                       uint32                               uSetupFlags)
{
   uintPtr          uRegOffset             = 0;
   uint32           uRegVal                = 0;

   // get register base offset
   uRegOffset = HAL_MDP_Get_PingPong_BaseOffset(ePingPongId);

   // program sync threshold
   uRegVal = HWIO_OUT_FLD(0,       MDP_PP_0_SYNC_THRESH, CONTINUE_THRESHOLD, pPingPongConfig->uContinueThreshold);
   uRegVal = HWIO_OUT_FLD(uRegVal, MDP_PP_0_SYNC_THRESH, START_THRESHOLD,    pPingPongConfig->uStartThreshold);
   out_dword(HWIO_MDP_PP_0_SYNC_THRESH_ADDR + uRegOffset, uRegVal);

   // program start position
   uRegVal = HWIO_OUT_FLD(0, MDP_PP_0_START_POS, POS, pPingPongConfig->uStartPosition);
   out_dword(HWIO_MDP_PP_0_START_POS_ADDR + uRegOffset, uRegVal);

   // program vsync input 
   uRegVal = in_dword(HWIO_MDP_PP_0_SYNC_CONFIG_VSYNC_ADDR + uRegOffset);
   if(TRUE == pPingPongConfig->bDedicatedTEPin)
   {
      uRegVal = HWIO_OUT_FLD(uRegVal, MDP_PP_0_SYNC_CONFIG_VSYNC, VSYNC_IN_EN, 1);
   }
   else
   {
      uRegVal = HWIO_OUT_FLD(uRegVal, MDP_PP_0_SYNC_CONFIG_VSYNC, VSYNC_IN_EN, 0);
   }
   out_dword(HWIO_MDP_PP_0_SYNC_CONFIG_VSYNC_ADDR + uRegOffset, uRegVal);

   // external vsync source select
   uRegVal = HWIO_MDP_MDP_VSYNC_SEL_IN;
   switch(ePingPongId)
   {
      case HAL_MDP_PINGPONG_0:
         uRegVal = HWIO_OUT_FLD(uRegVal, MDP_MDP_VSYNC_SEL, PING_PONG_0, pPingPongConfig->eVsyncSelect);
         break;
      case HAL_MDP_PINGPONG_1:
         uRegVal = HWIO_OUT_FLD(uRegVal, MDP_MDP_VSYNC_SEL, PING_PONG_1, pPingPongConfig->eVsyncSelect);
         break;
      case HAL_MDP_PINGPONG_2:
         uRegVal = HWIO_OUT_FLD(uRegVal, MDP_MDP_VSYNC_SEL, PING_PONG_2, pPingPongConfig->eVsyncSelect);
         break;
      default:
         break;
   }
   out_dword(HWIO_MDP_MDP_VSYNC_SEL_ADDR, uRegVal);
   
}

/****************************************************************************
*
** FUNCTION: HAL_MDP_PingPong_TE_Enable()
*/
/*!
* \brief
*     Enable/Disable tear check for given pingpong block
*
* \param [in]   ePingPongId      - Pingpong block id
* \param [in]   pPingPongConfig  - Configuration information
* \param [in]   uSetupFlags      - Reserved
*
* \retval NONE
*
****************************************************************************/
static void HAL_MDP_PingPong_TE_Enable(HAL_MDP_PingPongId                   ePingPongId,
                                       HAL_MDP_PingPong_TEEnableType        *pPingPongConfig,
                                       uint32                               uSetupFlags)
{
    uintPtr           uRegOffset = 0;

   // get register base offset
   uRegOffset = HAL_MDP_Get_PingPong_BaseOffset(ePingPongId);

   out_dword(HWIO_MDP_PP_0_TEAR_CHECK_EN_ADDR + uRegOffset, ((TRUE==pPingPongConfig->bEnable)? 1:0));
}

/****************************************************************************
*
** FUNCTION: HAL_MDP_PingPong_Vsync_Enable()
*/
/*!
* \brief
*     Enable/Disable vsync counter for the pingpong block
*
* \param [in]   ePingPongId      - Pingpong block id
* \param [in]   pPingPongConfig  - Configuration information
* \param [in]   uSetupFlags      - Reserved
*
* \retval NONE
*
****************************************************************************/
static void HAL_MDP_PingPong_Vsync_Enable(HAL_MDP_PingPongId                   ePingPongId,
                                          HAL_MDP_PingPong_VsyncEnableType     *pPingPongConfig,
                                          uint32                               uSetupFlags)
{
    uintPtr          uRegOffset = 0;
   uint32           uRegVal    = 0;

   // get register base offset
   uRegOffset = HAL_MDP_Get_PingPong_BaseOffset(ePingPongId);

   // enable the vsync counter
   uRegVal = in_dword(HWIO_MDP_PP_0_SYNC_CONFIG_VSYNC_ADDR + uRegOffset);
   uRegVal = HWIO_OUT_FLD(uRegVal, MDP_PP_0_SYNC_CONFIG_VSYNC, VSYNC_COUNTER_EN, ((TRUE==pPingPongConfig->bEnable)? 1:0));
   out_dword(HWIO_MDP_PP_0_SYNC_CONFIG_VSYNC_ADDR + uRegOffset, uRegVal);
}

/****************************************************************************
*
** FUNCTION: HAL_MDP_PingPong_AutoRefresh_Enable()
*/
/*!
* \brief
*     Enable/Disable auto-refresh for the pingpong block
*
* \param [in]   ePingPongId      - Pingpong block id
* \param [in]   pPingPongConfig  - Configuration information
* \param [in]   uSetupFlags      - Reserved
*
* \retval NONE
*
****************************************************************************/
static void HAL_MDP_PingPong_AutoRefresh_Enable(HAL_MDP_PingPongId                          ePingPongId,
                                                HAL_MDP_PingPong_AutoRefreshEnableType     *pPingPongConfig,
                                                uint32                                      uSetupFlags)
{
   uintPtr          uRegOffset = 0;
   uint32           uRegVal    = 0;

   // get register base offset
   uRegOffset = HAL_MDP_Get_PingPong_BaseOffset(ePingPongId);

   // enable the vsync counter
   uRegVal = in_dword(HWIO_MDP_PP_0_AUTOREFRESH_CONFIG_ADDR + uRegOffset);
   uRegVal = HWIO_OUT_FLD(uRegVal, MDP_PP_0_AUTOREFRESH_CONFIG, ENABLE, ((TRUE==pPingPongConfig->bEnable)? 1:0));
   out_dword(HWIO_MDP_PP_0_AUTOREFRESH_CONFIG_ADDR + uRegOffset, uRegVal);
}

/****************************************************************************
*
** FUNCTION: HAL_MDP_PingPong_RefreshInfo()
*/
/*!
* \brief
*     Returns the frame refresh information of the pingpong block
*
* \param [in]   ePingPongId        - Pingpong block id
* \param [out]  pFrameRefreshInfo  - Frame refresh information
* \param [in]   uFlags             - Reserved
*
* \retval NONE
*
****************************************************************************/
static void HAL_MDP_PingPong_RefreshInfo(HAL_MDP_PingPongId                      ePingPongId,
                                         HAL_MDP_PingPong_FrameRefreshInfoType   *pFrameRefreshInfo,
                                         uint32                                  uFlags)
{
   uintPtr          uRegOffset = 0;
   uint32           uRegVal    = 0;

   // get register base offset
   uRegOffset = HAL_MDP_Get_PingPong_BaseOffset(ePingPongId);

   uRegVal = in_dword(HWIO_MDP_PP_0_INT_COUNT_VAL_ADDR + uRegOffset);
   pFrameRefreshInfo->uCurrentFrameCnt = HWIO_GETVAL(MDP_PP_0_INT_COUNT_VAL, uRegVal, FRAME_COUNT);
   pFrameRefreshInfo->uCurrentLineCnt  = HWIO_GETVAL(MDP_PP_0_INT_COUNT_VAL, uRegVal, LINE_COUNT);
}

/****************************************************************************
*
** FUNCTION: HAL_MDP_PingPong_EngineStatus()
*/
/*!
* \brief
*     Returns the status of the pingpong block
*
* \param [in]   ePingPongId        - Pingpong block id
* \param [out]  pEngineStatus      - engine status information
* \param [in]   uFlags             - Reserved
*
* \retval NONE
*
****************************************************************************/
static void HAL_MDP_PingPong_EngineStatus(HAL_MDP_PingPongId                      ePingPongId,
                                          HAL_MDP_PingPong_EngineStatusType       *pEngineStatus,
                                          uint32                                  uFlags)
{
   uintPtr          uRegOffset = 0;
   uint32           uRegVal    = 0;

   // get register base offset
   uRegOffset = HAL_MDP_Get_PingPong_BaseOffset(ePingPongId);

   uRegVal = in_dword(HWIO_MDP_PP_0_SYNC_CONFIG_VSYNC_ADDR + uRegOffset);
   uRegVal = HWIO_GETVAL(MDP_PP_0_SYNC_CONFIG_VSYNC, uRegVal, VSYNC_COUNTER_EN);
   pEngineStatus->bIsEngineOn = (1 == uRegVal)? TRUE : FALSE;
}

/****************************************************************************
*
** FUNCTION: HAL_MDP_PingPong_GetStateInfo()
*/
/*!
* \brief
*     Returns the current state (active/idle) of the given ping-pong id
*
* \param [in]   ePingPongId      - Pingpong block id
* \param [out]  pbActive         - TRUE : Module is active
* \param [in]   uFlags           - Reserved
*
* \retval NONE
*
****************************************************************************/
static void HAL_MDP_PingPong_GetStateInfo(HAL_MDP_PingPongId     ePingPongId,
                                          bool32                *pbActive,
                                          uint32                 uFlags)
{
   uint32                        uRegValue         = 0x00;
   HAL_MDP_ModuleStatusRegInfo  *psModuleRegInfo   = NULL;

   *(pbActive)  = FALSE;  // Initial state

   psModuleRegInfo = (HAL_MDP_ModuleStatusRegInfo  *)pgsMdpHwInfo->gpsModuleStatusRegInfo[HAL_MDP_MODULE_PINGPONG];

   if(NULL != psModuleRegInfo)
   {
      uRegValue   = in_dword(MDSS_REG_BASE + psModuleRegInfo[ePingPongId].uRegAddress);

      if(0x00 != (uRegValue & psModuleRegInfo[ePingPongId].uBitFieldMask) )
      {
         *(pbActive)  = TRUE;        // Module is in active state
      }
   }
}

/****************************************************************************
*
** FUNCTION: HAL_MDP_PingPong_DitherConfig()
*/
/*!
* \brief
*     Set up configurations for ping-pong dither module
*
* \param [in]   ePingPongId       - Ping-Pong ID
* \param [in]   psDitherConfig    - Dither configuration information
*
* \retval NONE
*
****************************************************************************/
static void HAL_MDP_PingPong_DitherConfig(HAL_MDP_PingPongId                 ePingPongId,
                                          HAL_MDP_PingPong_DitherConfigType *psDitherConfig)
{
  uintPtr  uRegisterOffset   = 0;
  uint32   uDsppOpModeInfo   = 0;
  uint32   uDitherBitDepth   = 0;

  // Extract the base address to program the HW registers
  uRegisterOffset  = HAL_MDP_Get_PingPong_BaseOffset(ePingPongId);

  uDsppOpModeInfo  = in_dword(HWIO_MDP_PPB_0_DITHER_OP_MODE_ADDR + uRegisterOffset);

  // Dither is only for RGB destination with lower color depth than RGB888
  if (TRUE == psDitherConfig->bEnable) 
  {

    uDitherBitDepth = HWIO_OUT_FLD(uDitherBitDepth,
      MDP_PPB_0_DITHER_BITDEPTH,
      C0_BITS,
      psDitherConfig->sDitherBitDepth.uColor0);

    uDitherBitDepth = HWIO_OUT_FLD(uDitherBitDepth,
      MDP_PPB_0_DITHER_BITDEPTH,
      C1_BITS,
      psDitherConfig->sDitherBitDepth.uColor1);

    uDitherBitDepth = HWIO_OUT_FLD(uDitherBitDepth,
      MDP_PPB_0_DITHER_BITDEPTH,
      C2_BITS,
      psDitherConfig->sDitherBitDepth.uColor2);

    if (psDitherConfig->eDitherType == HAL_MDP_DITHER_TYPE_TEMPORAL)
    {
      uDitherBitDepth = HWIO_OUT_FLD(uDitherBitDepth,
        MDP_PPB_0_DITHER_BITDEPTH,
        TEMPORAL_DITHER_EN,
        1);
    }
    else
    {
      uDitherBitDepth = HWIO_OUT_FLD(uDitherBitDepth,
        MDP_PPB_0_DITHER_BITDEPTH,
        TEMPORAL_DITHER_EN,
        0);
    }

    out_dword(HWIO_MDP_PPB_0_DITHER_BITDEPTH_ADDR + uRegisterOffset, uDitherBitDepth);

    // Dither matrix
    if (NULL != psDitherConfig->pDitherMatrix)
    {
      out_dword(HWIO_MDP_PPB_0_DITHER_MATRIX_ROW0_ADDR + uRegisterOffset, psDitherConfig->pDitherMatrix->uRow0Value);    // ROW#0
      out_dword(HWIO_MDP_PPB_0_DITHER_MATRIX_ROW1_ADDR + uRegisterOffset, psDitherConfig->pDitherMatrix->uRow1Value);    // ROW#1
      out_dword(HWIO_MDP_PPB_0_DITHER_MATRIX_ROW2_ADDR + uRegisterOffset, psDitherConfig->pDitherMatrix->uRow2Value);    // ROW#2
      out_dword(HWIO_MDP_PPB_0_DITHER_MATRIX_ROW3_ADDR + uRegisterOffset, psDitherConfig->pDitherMatrix->uRow3Value);    // ROW#3
    }

    uDsppOpModeInfo  = HWIO_OUT_FLD(uDsppOpModeInfo,
      MDP_PPB_0_DITHER_OP_MODE,
      DITHER_EN,
      1);
  }
  else
  {
    uDsppOpModeInfo  = HWIO_OUT_FLD(uDsppOpModeInfo,
      MDP_PPB_0_DITHER_OP_MODE,
      DITHER_EN,
      0);
  }

  out_dword(HWIO_MDP_PPB_0_DITHER_OP_MODE_ADDR + uRegisterOffset, uDsppOpModeInfo);
}



/****************************************************************************
*
** FUNCTION: HAL_MDP_PingPong_DSCConfig()
*/
/*!
* \brief
*     Enables/Disables the DSC encoder connected to the given ping-pong id
*
* \param [in]   ePingPongId      - pingpong block id
* \param [in]   psDSConfig       - Configuration parameters for DSC encoder connected to ePingPongId
* \param [in]   uFlags           - Reserved.
*
* \retval HAL_MDSS_ErrorType
*
****************************************************************************/
static void HAL_MDP_PingPong_DSCConfig(HAL_MDP_PingPongId               ePingPongId,
                                       HAL_MDP_PingPong_DSCConfigType  *psDSConfig,
                                       uint32                           uFlags)
{
   uintPtr  uRegOffset  = 0;
   uintPtr  uReg        = 0;

   uReg = HWIO_OUT_FLD(0, MDP_PP_0_DCE_DATA_OUT_SWAP, ENDIAN, (psDSConfig->bEndianess? 1 : 0));

   /* Following are byte mapping will be true always for all targets */
   uReg = HWIO_OUT_FLD(uReg, MDP_PP_0_DCE_DATA_OUT_SWAP, BYTE5, 0x05);
   uReg = HWIO_OUT_FLD(uReg, MDP_PP_0_DCE_DATA_OUT_SWAP, BYTE4, 0x04);
   uReg = HWIO_OUT_FLD(uReg, MDP_PP_0_DCE_DATA_OUT_SWAP, BYTE3, 0x03);
   uReg = HWIO_OUT_FLD(uReg, MDP_PP_0_DCE_DATA_OUT_SWAP, BYTE2, 0x02);
   uReg = HWIO_OUT_FLD(uReg, MDP_PP_0_DCE_DATA_OUT_SWAP, BYTE1, 0x01);
   uReg = HWIO_OUT_FLD(uReg, MDP_PP_0_DCE_DATA_OUT_SWAP, BYTE0, 0x00);

   // Get register base offset
   uRegOffset = HAL_MDP_Get_PingPong_BaseOffset(ePingPongId);

   out_dword(HWIO_MDP_PP_0_DCE_DATA_OUT_SWAP_ADDR + uRegOffset, uReg);
   out_dword(HWIO_MDP_PP_0_DSC_MODE_ADDR + uRegOffset, psDSConfig->bEnableDSCEnc);
}

/****************************************************************************
*
** FUNCTION: HAL_MDP_PingPong_Setup()
*/
/*!
* \brief
*     Set up configurations for a particular pingpong block
*
* \param [in]   ePingPongId      - pingpong block id
* \param [in]   pPingPongConfig  - Configuration for the selected pingpong block
* \param [in]   uSetupFlags      - Reserved.
*
* \retval HAL_MDSS_ErrorType
*
****************************************************************************/
HAL_MDSS_ErrorType HAL_MDP_PingPong_Setup(HAL_MDP_PingPongId             ePingPongId,
                                          HAL_MDP_PingPongConfigType     *pPingPongConfig,
                                          uint32                         uSetupFlags)
{
   HAL_MDSS_ErrorType      eStatus  = HAL_MDSS_STATUS_SUCCESS;

   if((ePingPongId >= HAL_MDP_PINGPONG_0) && ((uint32)ePingPongId <= HAL_MDP_GET_PINGPONG_COUNT()))
   {
      // Vsync counter configuration for smart panel
      if(NULL != pPingPongConfig->pIntrVsyncConfig)
      {
         HAL_MDP_PingPong_Vsync_Config(ePingPongId, pPingPongConfig->pIntrVsyncConfig, 0x00);
      }

      // Tear check configuration for smart panel
      if(NULL != pPingPongConfig->pIntrTEConfig)
      {
         HAL_MDP_PingPong_TE_Config(ePingPongId, pPingPongConfig->pIntrTEConfig, 0x00);
      }

      // Tear check enable for smart panel
      if(NULL != pPingPongConfig->pIntrTEEnable)
      {
         HAL_MDP_PingPong_TE_Enable(ePingPongId, pPingPongConfig->pIntrTEEnable, 0x00);
      }

      // Vsync counter enable for smart panel
      if(NULL != pPingPongConfig->pIntrVsyncEnable)
      {
         HAL_MDP_PingPong_Vsync_Enable(ePingPongId, pPingPongConfig->pIntrVsyncEnable, 0x00);
      }

      // Auto-refresh enable for smart panel
      if(NULL != pPingPongConfig->pIntrAutoRefreshEnable)
      {
         HAL_MDP_PingPong_AutoRefresh_Enable(ePingPongId, pPingPongConfig->pIntrAutoRefreshEnable, 0x00);
      }

      // Display stream compression (DSC)
      if(NULL != pPingPongConfig->pDSCConfig)
      {
         HAL_MDP_PingPong_DSCConfig(ePingPongId, pPingPongConfig->pDSCConfig, 0x00);
      }

      // Dithering
      if(NULL != pPingPongConfig->pDitherConfig)
      {
         HAL_MDP_PingPong_DitherConfig(ePingPongId, pPingPongConfig->pDitherConfig);
      }
      
   }
   else
   {
      eStatus = HAL_MDSS_STATUS_FAILED_INVALID_INPUT_PARAMETER;
   }

   return eStatus;
}

/****************************************************************************
*
** FUNCTION: HAL_MDP_PingPong_GetProperty()
*/
/*!
* \brief
*     Retrieve information for a particular pingpong block
*
* \param [in]   ePingPongId           - pingpong block id
* \param [out]  pPingPongInfoData     - Info data
* \param [in]   uGetPropertyFlags     - Reserved
*
* \retval HAL_MDSS_ErrorType
*
****************************************************************************/
HAL_MDSS_ErrorType HAL_MDP_PingPong_GetProperty(HAL_MDP_PingPongId                   ePingPongId,
                                                HAL_MDP_PingPongInfoParamsType       *pPingPongInfoData,
                                                uint32                               uGetPropertyFlags)
{
   HAL_MDSS_ErrorType   eStatus  = HAL_MDSS_STATUS_SUCCESS;

   // Refresh rate info
   if(NULL != pPingPongInfoData->pFrameRefreshInfo)
   {
      HAL_MDP_PingPong_RefreshInfo(ePingPongId, pPingPongInfoData->pFrameRefreshInfo, 0x00);
   }

   // Engine status info
   if(NULL != pPingPongInfoData->pEngineStatus)
   {
       HAL_MDP_PingPong_EngineStatus(ePingPongId, pPingPongInfoData->pEngineStatus, 0x00);
   }

   // Module active/idle state
   if(NULL != pPingPongInfoData->pbActive)
   {
      HAL_MDP_PingPong_GetStateInfo(ePingPongId, pPingPongInfoData->pbActive, 0x00);
   }

   return eStatus;
}

#ifdef __cplusplus
}
#endif
