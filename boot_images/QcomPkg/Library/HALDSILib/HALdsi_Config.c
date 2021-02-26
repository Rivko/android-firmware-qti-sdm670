/*=============================================================================

  File: HALdsi_Config.c
  

  Copyright (c) 2010-2014,2016-2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/

/*============================================================================
*                         INCLUDE FILES
============================================================================*/
#include "HALdsi.h"
#include "HALdsi_reg.h"
#include "dsiHostSystem.h"

/* --- DO NOT ADD HEADER FILES HERE! --- */

#ifdef __cplusplus
extern "C" {
#endif

/* Extern debug variable from HALdsi.c */
extern HAL_dsi_StatusType geDSIErrorCode;


/* -----------------------------------------------------------------------                                  
** Forward Declaration of Local Functions
** ----------------------------------------------------------------------- */
static void dsi_Get_VideoColorInfo (DSI_ColorFormatType     eColorFormat,
                                    HAL_DSI_ColorInfoType   *psColorInfo);

static void dsi_Get_CmdColorInfo (  DSI_ColorFormatType     eColorFormat,
                                    HAL_DSI_ColorInfoType   *psColorInfo);

static void HAL_DSI_VideoTimingConfig(DSI_Device_IDType     eDeviceID,
                                      uint32                iiVisWidthPx,
                                      uint32                iVisHeightPx,
                                      DSI_VideoTimingType*  psVideoTiming);

/****************************************************************************
HAL_DSI_DeviceConfig()

Description:
    This API configure the DSI Core which is shared by video and command mode
    engineer. DSI host will keep a record of the activity of DSI Display , and
    turn on/off  the different block accordingly.

****************************************************************************/
void HAL_DSI_DeviceConfig(DSI_Device_IDType              eDeviceID,
                          HAL_DSI_DevicePropertyType    *psDeviceProperty)
{
  uint32  uDsiControlReg = 0;
  uintPtr uOffset        = HAL_DSI_GetRegBaseOffset(eDeviceID);

  /* Check input Parameters */
  if( NULL == psDeviceProperty)
  {
    geDSIErrorCode = HAL_DSI_FAILED_EXPECTED_NON_NULL_PTR;
  }
  else
  {
  /* Need to swap bit ordering of each byte of a packet?  */
    uDsiControlReg = HWIO_OUT_FLD(uDsiControlReg, DSI_0_CTRL, PACKET_BYTE_MSB_LSB_FLIP, (FALSE == psDeviceProperty->bLittleEndian? 1:0 ));

  /* Need to supporting non-compliant panels?             */
    uDsiControlReg = HWIO_OUT_FLD(uDsiControlReg, DSI_0_CTRL, DEBUG_MODE_EN,            (TRUE == psDeviceProperty->bNonCompliantMode? 1:0 ));
  
  /* Need to check CRC of received packets?               */
    uDsiControlReg = HWIO_OUT_FLD(uDsiControlReg, DSI_0_CTRL, CRC_CHK_EN,               (TRUE == psDeviceProperty->bCRCCheckEn? 1:0 ));

  /* Need to check ECC of received packets?               */
    uDsiControlReg = HWIO_OUT_FLD(uDsiControlReg, DSI_0_CTRL, ECC_CHK_EN,               (TRUE == psDeviceProperty->bECCCheckEn? 1:0 ));

  /* Enable DSI PHY Clock Lane?                           */
    uDsiControlReg = HWIO_OUT_FLD(uDsiControlReg, DSI_0_CTRL, CLKLN_EN,                 (TRUE == psDeviceProperty->bClockLnEn? 1:0 ));

  /* Enable DSI PHY Data Lane3?                           */
    uDsiControlReg = HWIO_OUT_FLD(uDsiControlReg, DSI_0_CTRL, DLN3_EN,                  (TRUE == psDeviceProperty->bDataLn3En? 1:0 ));

  /* Enable DSI PHY Data Lane2?                           */
    uDsiControlReg = HWIO_OUT_FLD(uDsiControlReg, DSI_0_CTRL, DLN2_EN,                  (TRUE == psDeviceProperty->bDataLn2En? 1:0 ));

  /* Enable DSI PHY Data Lane1?                           */
    uDsiControlReg = HWIO_OUT_FLD(uDsiControlReg, DSI_0_CTRL, DLN1_EN,                  (TRUE == psDeviceProperty->bDataLn1En? 1:0 ));

  /* Enable DSI PHY Data Lane0?                           */
    uDsiControlReg = HWIO_OUT_FLD(uDsiControlReg, DSI_0_CTRL, DLN0_EN,                  (TRUE == psDeviceProperty->bDataLn0En? 1:0 ));

  /* Enable DSI Command Mode Engine?                      */    
    uDsiControlReg = HWIO_OUT_FLD(uDsiControlReg, DSI_0_CTRL, CMD_MODE_EN,              (TRUE == psDeviceProperty->bCMDModeEn? 1:0 ));

  /* Enable DSI Video Mode Engine?                        */    
    uDsiControlReg = HWIO_OUT_FLD(uDsiControlReg, DSI_0_CTRL, VIDEO_MODE_EN,            (TRUE == psDeviceProperty->bVideoModeEn? 1:0 ));

  /* Enable DSI Controller?                               */    
    uDsiControlReg = HWIO_OUT_FLD(uDsiControlReg, DSI_0_CTRL, DSI_EN,                   (TRUE == psDeviceProperty->bDSICtrlEN? 1:0 ));

    out_dword(uOffset + HWIO_DSI_0_CTRL_ADDR, uDsiControlReg);
  }

}

/****************************************************************************
HAL_DSI_CPhyModeEnable()

Description:
    This API enable/disable  DSI Cphy

****************************************************************************/
void HAL_DSI_CPhyModeEnable(DSI_Device_IDType           eDeviceID,
                            bool32                      bEnable)
{
  uint32                          uCPhyMode         = 0;
  uintPtr                         uOffset           = HAL_DSI_GetRegBaseOffset(eDeviceID);
  
  uCPhyMode = in_dword(uOffset + HWIO_DSI_0_CPHY_MODE_CTRL_ADDR);

  uCPhyMode = HWIO_OUT_FLD(uCPhyMode, DSI_0_CPHY_MODE_CTRL, EN, ((TRUE == bEnable)?1:0));

  out_dword(uOffset + HWIO_DSI_0_CPHY_MODE_CTRL_ADDR, uCPhyMode);
}

/****************************************************************************
HAL_DSI_SetScratchRegister1()

Description:
    This API set dsi scratch reigster 1

****************************************************************************/
void HAL_DSI_SetScratchRegister1(DSI_Device_IDType           eDeviceID,
                                 uint32                      uData)
{
  uintPtr                         uOffset           = HAL_DSI_GetRegBaseOffset(eDeviceID);

  out_dword(uOffset + HWIO_DSI_0_SCRATCH_REGISTER_1_ADDR, uData);
}

/****************************************************************************
HAL_DSI_LaneConfig()

Description:
    This API configure the DSI Lane configuration

****************************************************************************/
void HAL_DSI_LaneConfig(DSI_Device_IDType           eDeviceID,
                        HAL_DSI_LanePropertyType   *psLaneProperty)
{
  uint32                          uDsiLaneCntl      = 0;
  uintPtr                         uOffset           = HAL_DSI_GetRegBaseOffset(eDeviceID);
  
  /* Check input Parameters */
  if (NULL == psLaneProperty)
  {
    geDSIErrorCode = HAL_DSI_FAILED_EXPECTED_NON_NULL_PTR;
  }
  else
  {
    uDsiLaneCntl = in_dword(uOffset + HWIO_DSI_0_LANE_CTRL_ADDR);

    /* program clock lane */
    switch(psLaneProperty->eClockLaneOp)
    {
    case DSI_LANE_OP_CLOCK_HS_FORCE_REQUEST:
      {
        uDsiLaneCntl = HWIO_OUT_FLD(uDsiLaneCntl, DSI_0_LANE_CTRL, CLKLN_HS_FORCE_REQUEST,1);
      }
      break;
    case DSI_LANE_OP_CLOCK_HS_FORCE_REQUEST_CLEAR:
      {
        uDsiLaneCntl = HWIO_OUT_FLD(uDsiLaneCntl, DSI_0_LANE_CTRL, CLKLN_HS_FORCE_REQUEST,0);
      }
      break;
    default:
      break;
    }
    out_dword(uOffset + HWIO_DSI_0_LANE_CTRL_ADDR, uDsiLaneCntl);
  }

}


/****************************************************************************
HAL_DSI_VideoDscConfig()

Description:
    This API configures the DSI controller related registers for DSC for
    the video mode engine.
****************************************************************************/
void HAL_DSI_VideoDscConfig( DSI_Device_IDType                 eDeviceID,
                           DSI_DscVideoModeConfigType*      psVideoConfig)
{
  uint32  uCompressionModeCtrl      = 0;
  uint32  uCompressionModeCtrl2     = 0;
  uintPtr uOffset                   = HAL_DSI_GetRegBaseOffset(eDeviceID);

  /* Check input Parameters */
  if( NULL == psVideoConfig)
  {
    geDSIErrorCode = HAL_DSI_FAILED_EXPECTED_NON_NULL_PTR;
  }
  else
  {
    uCompressionModeCtrl  = HWIO_OUT_FLD(0,                    DSI_0_VIDEO_COMPRESSION_MODE_CTRL, WC, (psVideoConfig->uDscBytesPerPkt));
    uCompressionModeCtrl  = HWIO_OUT_FLD(uCompressionModeCtrl, DSI_0_VIDEO_COMPRESSION_MODE_CTRL, DATATYPE, 0x0B); //Compressed Image
    uCompressionModeCtrl  = HWIO_OUT_FLD(uCompressionModeCtrl, DSI_0_VIDEO_COMPRESSION_MODE_CTRL, PKT_PER_LINE, (psVideoConfig->uDscPktPerLine));
    uCompressionModeCtrl  = HWIO_OUT_FLD(uCompressionModeCtrl, DSI_0_VIDEO_COMPRESSION_MODE_CTRL, EOL_BYTE_NUM, (psVideoConfig->uDscEolByteNum));
    uCompressionModeCtrl  = HWIO_OUT_FLD(uCompressionModeCtrl, DSI_0_VIDEO_COMPRESSION_MODE_CTRL, EN, (psVideoConfig->uDscEnable));

    out_dword(uOffset + HWIO_DSI_0_VIDEO_COMPRESSION_MODE_CTRL2_ADDR, uCompressionModeCtrl2);
    out_dword(uOffset + HWIO_DSI_0_VIDEO_COMPRESSION_MODE_CTRL_ADDR,  uCompressionModeCtrl);
  }
}

/****************************************************************************
HAL_DSI_VideoModeConfig()

Description:
   This API configures the DSI video mode engine.
****************************************************************************/
void HAL_DSI_VideoModeConfig( DSI_Device_IDType               eDeviceID,
                              HAL_DSI_VideoModeConfigType*    psVideoConfig)
{
  HAL_DSI_ColorInfoType sColorInfo;
  uint32                iVideoModeReg     = 0;
  uint32                iVideoDataCtrlReg = 0;
  uint32                uEotPacketCtrlReg = 0;
  uintPtr               uOffset           = HAL_DSI_GetRegBaseOffset(eDeviceID);

  /* Check input Parameters */
  if( NULL == psVideoConfig)
  {
    geDSIErrorCode = HAL_DSI_FAILED_EXPECTED_NON_NULL_PTR;
  }
  else
  {
    HAL_DSI_VideoTimingConfig(eDeviceID, psVideoConfig->sDisplayInfo.iVisWidthPx,psVideoConfig->sDisplayInfo.iVisHeightPx,&psVideoConfig->sVideoTiming);

    /* If True,Send HSA and HE following VS/VE Packet */
    iVideoModeReg = HWIO_OUT_FLD(iVideoModeReg, DSI_0_VIDEO_MODE_CTRL, PULSE_MODE_OPT,  (TRUE == psVideoConfig->sPowerOption.bSendHsaHseAfterVsVe ? 1:0 ));

    /* If True, low power stop mode (LP-11) during HFP period */
    iVideoModeReg = HWIO_OUT_FLD(iVideoModeReg, DSI_0_VIDEO_MODE_CTRL, HFP_PWR_MODE,    (TRUE == psVideoConfig->sPowerOption.bEnableLowPowerModeInHFP? 1:0 ));

    /* If True, low power stop mode (LP-11) during HBP period */
    iVideoModeReg = HWIO_OUT_FLD(iVideoModeReg, DSI_0_VIDEO_MODE_CTRL, HBP_PWR_MODE,    (TRUE == psVideoConfig->sPowerOption.bEnableLowPowerModeInHBP? 1:0 ));

    /* If True, low power stop mode (LP-11) during HSA period */
    iVideoModeReg = HWIO_OUT_FLD(iVideoModeReg, DSI_0_VIDEO_MODE_CTRL, HSA_PWR_MODE,    (TRUE == psVideoConfig->sPowerOption.bEnableLowPowerModeInHSA? 1:0 ));

    /* If True, low power stop mode (LP-11) for the BLLP of the last line of a frame or let Cmd engineer send packets in HS/LP mode */
    iVideoModeReg = HWIO_OUT_FLD(iVideoModeReg, DSI_0_VIDEO_MODE_CTRL, EOF_BLLP_PWR_MODE,(TRUE == psVideoConfig->sPowerOption.bEnableLowPowerModeInBLLPEOF? 1:0 ));

    /* If True, low power stop mode (LP-11) for the BLLP of the last line of a frame or let Cmd engineer send packets in HS/LP mode */
    iVideoModeReg = HWIO_OUT_FLD(iVideoModeReg, DSI_0_VIDEO_MODE_CTRL, BLLP_PWR_MODE,   (TRUE == psVideoConfig->sPowerOption.bEnableLowPowerModeInBLLP? 1:0 ));

    /* Traffic Mode */
    switch (psVideoConfig->eTrafficeMode)
    {
    case DSI_Video_TrafficMode_NonBurst_VSPulse:
      iVideoModeReg = HWIO_OUT_FLD(iVideoModeReg, DSI_0_VIDEO_MODE_CTRL, TRAFFIC_MODE, HAL_DSI_VIDEO_TRAFFIC_MODE_NONBURST_SYNCPULSE);
      break;
    case DSI_Video_TrafficMode_NonBurst_VSEvent:
      iVideoModeReg = HWIO_OUT_FLD(iVideoModeReg, DSI_0_VIDEO_MODE_CTRL, TRAFFIC_MODE, HAL_DSI_VIDEO_TRAFFIC_MODE_NONBURST_SYNCEVENT);
      break;
    case DSI_Video_TrafficMode_Burst:
      iVideoModeReg = HWIO_OUT_FLD(iVideoModeReg, DSI_0_VIDEO_MODE_CTRL, TRAFFIC_MODE, HAL_DSI_VIDEO_TRAFFIC_MODE_BURST);
      break;
    default:
      geDSIErrorCode = HAL_DSI_FAILED_NO_SUPPORT_OR_NO_HW;
      return;
    }

    /* Clear the content of sColorInfo*/
    DSI_OSAL_MemZero(&sColorInfo, sizeof(HAL_DSI_ColorInfoType));

    dsi_Get_VideoColorInfo(psVideoConfig->sDisplayInfo.eColorFormat, &sColorInfo);

    /* Register value for DSI_VIDEO_MODE_CTRL */
    iVideoModeReg = HWIO_OUT_FLD(iVideoModeReg, DSI_0_VIDEO_MODE_CTRL, DST_FORMAT, sColorInfo.iColorDepth);
    iVideoModeReg = HWIO_OUT_FLD(iVideoModeReg, DSI_0_VIDEO_MODE_CTRL, VC,         psVideoConfig->sDisplayInfo.eDisplayVC);


    /* Register value for DSI_VIDEO_MODE_DATA_CTRL */
    iVideoDataCtrlReg = HWIO_OUT_FLD(iVideoDataCtrlReg, DSI_0_VIDEO_MODE_DATA_CTRL, RGB_SWAP, sColorInfo.iColorSwap);
    iVideoDataCtrlReg = HWIO_OUT_FLD(iVideoDataCtrlReg, DSI_0_VIDEO_MODE_DATA_CTRL, R_SEL,    sColorInfo.iColorRSel);
    iVideoDataCtrlReg = HWIO_OUT_FLD(iVideoDataCtrlReg, DSI_0_VIDEO_MODE_DATA_CTRL, G_SEL,    sColorInfo.iColorGSel);
    iVideoDataCtrlReg = HWIO_OUT_FLD(iVideoDataCtrlReg, DSI_0_VIDEO_MODE_DATA_CTRL, B_SEL,    sColorInfo.iColorBSel);

    out_dword(uOffset + HWIO_DSI_0_VIDEO_MODE_CTRL_ADDR,      iVideoModeReg);
    out_dword(uOffset + HWIO_DSI_0_VIDEO_MODE_DATA_CTRL_ADDR, iVideoDataCtrlReg);

    /* enable EoTp appending at the end of forward HS data burst */
    uEotPacketCtrlReg = in_dword(uOffset + HWIO_DSI_0_EOT_PACKET_CTRL_ADDR);
    uEotPacketCtrlReg = HWIO_OUT_FLD(uEotPacketCtrlReg, DSI_0_EOT_PACKET_CTRL, TX_EOT_APPEND, (TRUE == psVideoConfig->sDisplayInfo.bDisableEotp? 0 : 1));
    out_dword(uOffset + HWIO_DSI_0_EOT_PACKET_CTRL_ADDR, uEotPacketCtrlReg);
  }

}

/****************************************************************************
HAL_DSI_CMDDscConfig()

Description:
    This API configures the DSI controller related registers for DSC for
    the command mode engine.
****************************************************************************/
void HAL_DSI_CMDDscConfig( DSI_Device_IDType                 eDeviceID,
                           DSI_DscCmdModeConfigType*    psCMDDscConfig)
{
  uint32  uCompressionModeCtrl      = 0;
  uint32  uCompressionModeCtrl2     = 0;
  uint32  uCompressionModeCtrl3     = 0;
  uintPtr uOffset                   = HAL_DSI_GetRegBaseOffset(eDeviceID);


  /* Check input Parameters */
  if( NULL == psCMDDscConfig)
  {
    geDSIErrorCode = HAL_DSI_FAILED_EXPECTED_NON_NULL_PTR;
  }
  else
  {
    uCompressionModeCtrl  = HWIO_OUT_FLD(0,                    DSI_0_COMMAND_COMPRESSION_MODE_CTRL, STREAM0_DATATYPE, 0x39); //Long Packet
    uCompressionModeCtrl  = HWIO_OUT_FLD(uCompressionModeCtrl, DSI_0_COMMAND_COMPRESSION_MODE_CTRL, STREAM0_PKT_PER_LINE, (psCMDDscConfig->uDscPktPerLine));
    uCompressionModeCtrl  = HWIO_OUT_FLD(uCompressionModeCtrl, DSI_0_COMMAND_COMPRESSION_MODE_CTRL, STREAM0_EOL_BYTE_NUM, (psCMDDscConfig->uDscEolByteNum));
    uCompressionModeCtrl  = HWIO_OUT_FLD(uCompressionModeCtrl, DSI_0_COMMAND_COMPRESSION_MODE_CTRL, STREAM0_EN, (psCMDDscConfig->uDscEnable));

    out_dword(uOffset + HWIO_DSI_0_COMMAND_COMPRESSION_MODE_CTRL3_ADDR, uCompressionModeCtrl3);
    out_dword(uOffset + HWIO_DSI_0_COMMAND_COMPRESSION_MODE_CTRL2_ADDR, uCompressionModeCtrl2);
    out_dword(uOffset + HWIO_DSI_0_COMMAND_COMPRESSION_MODE_CTRL_ADDR,  uCompressionModeCtrl);
  }
}

/****************************************************************************
HAL_DSI_CMDModeConfig()

Description:
    This API configures the DSI command mode engine.
****************************************************************************/
void HAL_DSI_CMDModeConfig( DSI_Device_IDType                 eDeviceID,
                            HAL_DSI_CmdModeConfigType*        psCMDConfig)
{
  uint32  uCommandModeMDPCtrlReg    = 0; 
  uint32  uCommandMdpTotalReg       = 0;
  uint32  uCommandMdpStreamReg      = 0;
  uint32  uEotPacketCtrlReg         = 0;
  uint32  uIdleCtrlReg              = 0;
  uintPtr uOffset                   = HAL_DSI_GetRegBaseOffset(eDeviceID);
  
  /* Check input Parameters */
  if( NULL == psCMDConfig)
  {
    geDSIErrorCode = HAL_DSI_FAILED_EXPECTED_NON_NULL_PTR;
  }
  else
  {
    HAL_DSI_ColorInfoType sColorInfo;

    /* Clear the content of sColorInfo*/
    DSI_OSAL_MemZero(&sColorInfo, sizeof(HAL_DSI_ColorInfoType));

    /* Maximum number of command mode RGB packets to send within one H blanking period of video mode frame, */
    uCommandModeMDPCtrlReg = HWIO_OUT_FLD(0, DSI_0_COMMAND_MODE_MDP_CTRL, INTERLEAVE_MAX, psCMDConfig->uMaxInterleave);

    dsi_Get_CmdColorInfo(psCMDConfig->sDisplayInfo.eColorFormat, &sColorInfo);
    /* Register value for DSI_COMMAND_MODE_MDP_CTRL */
    uCommandModeMDPCtrlReg = HWIO_OUT_FLD(0,                      DSI_0_COMMAND_MODE_MDP_CTRL, INTERLEAVE_MAX, psCMDConfig->uMaxInterleave);
    uCommandModeMDPCtrlReg = HWIO_OUT_FLD(uCommandModeMDPCtrlReg, DSI_0_COMMAND_MODE_MDP_CTRL, DST_FORMAT,     sColorInfo.iColorDepth);
    uCommandModeMDPCtrlReg = HWIO_OUT_FLD(uCommandModeMDPCtrlReg, DSI_0_COMMAND_MODE_MDP_CTRL, RGB_SWAP,       sColorInfo.iColorSwap);
    uCommandModeMDPCtrlReg = HWIO_OUT_FLD(uCommandModeMDPCtrlReg, DSI_0_COMMAND_MODE_MDP_CTRL, R_SEL,          sColorInfo.iColorRSel);
    uCommandModeMDPCtrlReg = HWIO_OUT_FLD(uCommandModeMDPCtrlReg, DSI_0_COMMAND_MODE_MDP_CTRL, G_SEL,          sColorInfo.iColorGSel);
    uCommandModeMDPCtrlReg = HWIO_OUT_FLD(uCommandModeMDPCtrlReg, DSI_0_COMMAND_MODE_MDP_CTRL, B_SEL,          sColorInfo.iColorBSel);

    if (DSI_ChannelID_CMD0 == psCMDConfig->sDisplayInfo.eDisplayId)
    {
      /* Register value for DSI_COMMAND_MODE_MDP_STREAM0_TOTAL */
      uCommandMdpTotalReg = HWIO_OUT_FLD(uCommandMdpTotalReg, DSI_0_COMMAND_MODE_MDP_STREAM0_TOTAL, MDP0_H_TOTAL, psCMDConfig->sDisplayInfo.iVisWidthPx);
      uCommandMdpTotalReg = HWIO_OUT_FLD(uCommandMdpTotalReg, DSI_0_COMMAND_MODE_MDP_STREAM0_TOTAL, MDP0_V_TOTAL, psCMDConfig->sDisplayInfo.iVisHeightPx);

      /* Register value for DSI_COMMAND_MODE_MDP_STREAM0_CTRL */
      uCommandMdpStreamReg = HWIO_OUT_FLD(uCommandMdpStreamReg, DSI_0_COMMAND_MODE_MDP_STREAM0_CTRL, MDP0_WC, (((psCMDConfig->sDisplayInfo.iVisWidthPx * sColorInfo.iBpp)>>3 )+1));
      uCommandMdpStreamReg = HWIO_OUT_FLD(uCommandMdpStreamReg, DSI_0_COMMAND_MODE_MDP_STREAM0_CTRL, MDP0_VC, psCMDConfig->sDisplayInfo.eDisplayVC);
      uCommandMdpStreamReg = HWIO_OUT_FLD(uCommandMdpStreamReg, DSI_0_COMMAND_MODE_MDP_STREAM0_CTRL, MDP0_DT, psCMDConfig->uMDPDataType);

      out_dword(uOffset + HWIO_DSI_0_COMMAND_MODE_MDP_STREAM0_TOTAL_ADDR, uCommandMdpTotalReg);
      out_dword(uOffset + HWIO_DSI_0_COMMAND_MODE_MDP_STREAM0_CTRL_ADDR,  uCommandMdpStreamReg);
    }
    else
    {
      /* Register value for DSI_COMMAND_MODE_MDP_STREAM1_TOTAL */
      uCommandMdpTotalReg = HWIO_OUT_FLD(uCommandMdpTotalReg, DSI_0_COMMAND_MODE_MDP_STREAM1_TOTAL, MDP1_H_TOTAL, psCMDConfig->sDisplayInfo.iVisWidthPx);
      uCommandMdpTotalReg = HWIO_OUT_FLD(uCommandMdpTotalReg, DSI_0_COMMAND_MODE_MDP_STREAM1_TOTAL, MDP1_V_TOTAL, psCMDConfig->sDisplayInfo.iVisHeightPx);

      /* Register value for DSI_COMMAND_MODE_MDP_STREAM1_CTRL */
      uCommandMdpStreamReg = HWIO_OUT_FLD(uCommandMdpStreamReg, DSI_0_COMMAND_MODE_MDP_STREAM1_CTRL, MDP1_WC, (((psCMDConfig->sDisplayInfo.iVisWidthPx * sColorInfo.iBpp)>>3 )+1));
      uCommandMdpStreamReg = HWIO_OUT_FLD(uCommandMdpStreamReg, DSI_0_COMMAND_MODE_MDP_STREAM1_CTRL, MDP1_VC, psCMDConfig->sDisplayInfo.eDisplayVC);
      uCommandMdpStreamReg = HWIO_OUT_FLD(uCommandMdpStreamReg, DSI_0_COMMAND_MODE_MDP_STREAM1_CTRL, MDP1_DT, psCMDConfig->uMDPDataType);

      out_dword(uOffset + HWIO_DSI_0_COMMAND_MODE_MDP_STREAM1_TOTAL_ADDR, uCommandMdpTotalReg);
      out_dword(uOffset + HWIO_DSI_0_COMMAND_MODE_MDP_STREAM1_CTRL_ADDR,  uCommandMdpStreamReg);
    }

    out_dword(uOffset + HWIO_DSI_0_COMMAND_MODE_MDP_DCS_CMD_CTRL_ADDR, 0x13C2C);
    out_dword(uOffset + HWIO_DSI_0_COMMAND_MODE_MDP_CTRL_ADDR,         uCommandModeMDPCtrlReg);

    /* enable DSI HS burst mode (this feature is available for DSI core 1.3.0 and above) */
    uCommandModeMDPCtrlReg = in_dword(uOffset + HWIO_DSI_0_COMMAND_MODE_MDP_CTRL2_ADDR);
    uCommandModeMDPCtrlReg = HWIO_OUT_FLD(uCommandModeMDPCtrlReg, DSI_0_COMMAND_MODE_MDP_CTRL2, BURST_MODE, 1);
    out_dword(uOffset + HWIO_DSI_0_COMMAND_MODE_MDP_CTRL2_ADDR, uCommandModeMDPCtrlReg);

    /* enable EoTp appending at the end of forward HS data burst */
    uEotPacketCtrlReg = in_dword(uOffset + HWIO_DSI_0_EOT_PACKET_CTRL_ADDR);
    uEotPacketCtrlReg = HWIO_OUT_FLD(uEotPacketCtrlReg, DSI_0_EOT_PACKET_CTRL, TX_EOT_APPEND, (TRUE == psCMDConfig->sDisplayInfo.bDisableEotp? 0:1));
    out_dword(uOffset + HWIO_DSI_0_EOT_PACKET_CTRL_ADDR , uEotPacketCtrlReg);

    /*
    * program idle time to be inserted between command mode MDP packets
    *
    * This delay is added between cmd mdp packets in the application layer. If the delay setting is not long enough, 
    * then the packets may eventually get merged in the lane layer. The delay has to be large enough so that the lane 
    * layer is starved by the time the next cmd mdp packet arrives. This way we guarantee that each cmd mdp packets 
    * are transmitted in separate transfers.
    */
    if(psCMDConfig->uIdleTime > 0)
    {
      /* enable for stream 0 */
      uIdleCtrlReg = HWIO_OUT_FLD(uIdleCtrlReg, DSI_0_COMMAND_MODE_MDP_IDLE_CTRL, EN,     1);
      uIdleCtrlReg = HWIO_OUT_FLD(uIdleCtrlReg, DSI_0_COMMAND_MODE_MDP_IDLE_CTRL, LENGTH, psCMDConfig->uIdleTime);
    }
    out_dword(uOffset + HWIO_DSI_0_COMMAND_MODE_MDP_IDLE_CTRL_ADDR , uIdleCtrlReg);
  }
}


/****************************************************************************

HAL_DSI_SetExtTEPin()

Description:
    This API configures the DSI Controller according to External TE Pin .

****************************************************************************/
void HAL_DSI_ExtTEPinConfig(DSI_Device_IDType              eDeviceID,
                            HAL_DSI_ExtTEConfigType       *psExtTEfig)
{
  uint32  uExtMuxReg            = 0; 
  uint32  uExtTEPulseCtrlReg    = 0;
  uintPtr uOffset               = HAL_DSI_GetRegBaseOffset(eDeviceID);

  /* Check input Parameters */
  if( NULL == psExtTEfig)
  {
    geDSIErrorCode = HAL_DSI_FAILED_EXPECTED_NON_NULL_PTR;
  }
  else if (DSI_ExtTEMode_NONE == psExtTEfig->eExtTEMode)
  {
    /* No need to config Ext TE */
  }
  else
  {
    /* Polarity of External Tearing Signal*/
    uExtMuxReg = HWIO_OUT_FLD(uExtMuxReg, DSI_0_EXT_MUX, EXT_TE_POL,            psExtTEfig->bExtTEFallingEdge);
    uExtMuxReg = HWIO_OUT_FLD(uExtMuxReg, DSI_0_EXT_MUX, EXT_TE_HSYNC_TOTAL,    psExtTEfig->iTEHsyncTotal);
    uExtMuxReg = HWIO_OUT_FLD(uExtMuxReg, DSI_0_EXT_MUX, EXT_TE_HSYNC_TRIG_CNT, psExtTEfig->iTEHSyncCount);
    uExtMuxReg = HWIO_OUT_FLD(uExtMuxReg, DSI_0_EXT_MUX, EXT_TE_MUX,            psExtTEfig->uExtTEMux);

    uExtTEPulseCtrlReg = HWIO_OUT_FLD(uExtTEPulseCtrlReg, DSI_0_EXT_TE_PULSE, DETECT_CTRL_TE_VSYNC_MIN_WIDTH, psExtTEfig->iTEVsyncPulseWidth_Min);
    uExtTEPulseCtrlReg = HWIO_OUT_FLD(uExtTEPulseCtrlReg, DSI_0_EXT_TE_PULSE, DETECT_CTRL_TE_HSYNC_MAX_WIDTH, psExtTEfig->iTEHsyncPulseWidth_Max);

    switch(psExtTEfig->eExtTEMode)
    {
    case DSI_ExtTEMode_VS_EDGE:     /**< edge active, Vsync only      */
      {
        uExtMuxReg = HWIO_OUT_FLD(uExtMuxReg, DSI_0_EXT_MUX, EXT_TE_MODE,   HAL_DSI_CMDMODE_TEMODE_VSYNCEDGE);
        break;
      }
    case DSI_ExtTEMode_VS_WIDTH:    /**< level active, Vsync only     */
      {
        uExtMuxReg = HWIO_OUT_FLD(uExtMuxReg, DSI_0_EXT_MUX, EXT_TE_MODE,   HAL_DSI_CMDMODE_TEMODE_VSYNCWIDTH);
        break;
      }
    case DSI_ExtTEMode_VSHS_EDGE:   /**< edge active, Vsync and HSync */
      {
        uExtMuxReg = HWIO_OUT_FLD(uExtMuxReg, DSI_0_EXT_MUX, EXT_TE_MODE,   HAL_DSI_CMDMODE_TEMODE_VSYNCHSYNCEDGE);
        break;
      }
    case DSI_ExtTEMode_VSHS_WIDTH:  /**< level active,Vsync and HSync */
      {
        uExtMuxReg = HWIO_OUT_FLD(uExtMuxReg, DSI_0_EXT_MUX, EXT_TE_MODE,   HAL_DSI_CMDMODE_TEMODE_VSYNCHSYNCWIDTH);
        break;
      }
    default:
      geDSIErrorCode = HAL_DSI_FAILED_INVALID_INPUT_PARAMETER;
      return;
    }

    out_dword(uOffset + HWIO_DSI_0_EXT_MUX_ADDR,                   uExtMuxReg);
    out_dword(uOffset + HWIO_DSI_0_EXT_TE_PULSE_DETECT_CTRL_ADDR,  uExtTEPulseCtrlReg);
  }
}

/****************************************************************************

HAL_DSI_SetDMATriggerConfig()

Description:
    This API sets the command mode DMA trigger mode.

****************************************************************************/
void HAL_DSI_SetDMATriggerConfig(DSI_Device_IDType    eDeviceID, 
                                 DSI_CMDTriggerType   eDMATriggerType )
{
  uint32        uDMATrigType     = 0;
  uintPtr       uOffset          = HAL_DSI_GetRegBaseOffset(eDeviceID);
  uint32        uTriggerCtrlReg  = in_dword(uOffset + HWIO_DSI_0_TRIG_CTRL_ADDR );
  const uint32  uTriggerTypeLUT[]= 
  {
    HAL_DSI_CMDMODE_DMA_TRIGGER_NONE,     /*DSI_Display_CMDTrigger_NONE       */
    HAL_DSI_CMDMODE_DMA_TRIGGER_SOFEOF,   /*DSI_Display_CMDTrigger_SOFEOF     */
    HAL_DSI_CMDMODE_DMA_TRIGGER_TE,       /*DSI_Display_CMDTrigger_TE         */
    HAL_DSI_CMDMODE_DMA_TRIGGER_SW,       /*DSI_Display_CMDTrigger_SW         */
    HAL_DSI_CMDMODE_DMA_TRIGGER_SWSOFEOF, /*DSI_Display_CMDTrigger_SW_SOFEOF  */
    HAL_DSI_CMDMODE_DMA_TRIGGER_SWTE,     /*DSI_Display_CMDTrigger_SW_TE      */
  };

  if( DSI_Display_CMDTrigger_MAX <= eDMATriggerType )
  {
    geDSIErrorCode = HAL_DSI_FAILED_INVALID_INPUT_PARAMETER;
  }
  else
  {
    uDMATrigType = uTriggerTypeLUT[eDMATriggerType];

    //clear the  COMMAND_MODE_DMA_TRIGGER_SEL field.
    uTriggerCtrlReg = HWIO_OUT_FLD(uTriggerCtrlReg, DSI_0_TRIG_CTRL_COMMAND_MODE, DMA_TRIGGER_SEL,   uDMATrigType);

    out_dword(uOffset + HWIO_DSI_0_TRIG_CTRL_ADDR ,uTriggerCtrlReg);
  }
}


/****************************************************************************

HAL_DSI_GetDMATriggerConfig()

Description:
    This API gets the command mode DMA trigger mode.

****************************************************************************/
void HAL_DSI_GetDMATriggerConfig(DSI_Device_IDType    eDeviceID, 
                                 DSI_CMDTriggerType  *pDMATriggerType )
{
  uintPtr                   uOffset            = HAL_DSI_GetRegBaseOffset(eDeviceID);
  uint32                    uTriggerCtrlReg    = in_dword(uOffset + HWIO_DSI_0_TRIG_CTRL_ADDR );
  uint32                    uDMATrigCtrlRegVal = HWIO_GETVAL(DSI_0_TRIG_CTRL_COMMAND_MODE, uTriggerCtrlReg, DMA_TRIGGER_SEL);
  const DSI_CMDTriggerType  uTriggerTypeLUT[]  = 
  {
    DSI_Display_CMDTrigger_NONE,      /*HAL_DSI_CMDMODE_DMA_TRIGGER_NONE     = 0  */
    DSI_Display_CMDTrigger_SOFEOF,    /*HAL_DSI_CMDMODE_DMA_TRIGGER_SOFEOF   = 1  */
    DSI_Display_CMDTrigger_TE,        /*HAL_DSI_CMDMODE_DMA_TRIGGER_TE       = 2  */
    DSI_Display_CMDTrigger_NONE,      /*Invalid trigger value 0x3            = 3  */
    DSI_Display_CMDTrigger_SW,        /*HAL_DSI_CMDMODE_DMA_TRIGGER_SW       = 4  */
    DSI_Display_CMDTrigger_SW_SOFEOF, /*HAL_DSI_CMDMODE_DMA_TRIGGER_SWSOFEOF = 5  */
    DSI_Display_CMDTrigger_SW_TE,     /*HAL_DSI_CMDMODE_DMA_TRIGGER_SWTE     = 6  */
  };

  if( HAL_DSI_CMDMODE_DMA_TRIGGER_SWTE < uDMATrigCtrlRegVal )
  {
    *pDMATriggerType  = DSI_Display_CMDTrigger_NONE;
    geDSIErrorCode    = HAL_DSI_FAILED_INVALID_INPUT_PARAMETER;
  }
  else
  {
    *pDMATriggerType  = uTriggerTypeLUT[uDMATrigCtrlRegVal];
  }

}


/****************************************************************************

HAL_DSI_TriggerConfig()

Description:
    This API configures the command mode DMA and MDP trigger mode.

****************************************************************************/
void HAL_DSI_TriggerConfig( DSI_Device_IDType                eDeviceID,
                            HAL_DSI_TriggerModeType         *psTriggerMode)
{
  uintPtr       uOffset             = HAL_DSI_GetRegBaseOffset(eDeviceID);
  uint32        uTriggerCtrlReg     = 0;
  uint32        uTriggerType        = 0;
  const uint32  uMDPTriggerTypeLUT[]= 
  {
    HAL_DSI_CMDMODE_MDP_TRIGGER_NONE,       /*DSI_Display_ImageTrigger_NONE   */
    HAL_DSI_CMDMODE_MDP_TRIGGER_TE,         /*DSI_Display_ImageTrigger_TE     */
    HAL_DSI_CMDMODE_MDP_TRIGGER_SW,         /*DSI_Display_ImageTrigger_SW     */
    HAL_DSI_CMDMODE_MDP_TRIGGER_SWTE,       /*DSI_Display_ImageTrigger_SW_TE  */
  };

  /* Check input Parameters */
  if( NULL == psTriggerMode)
  {
    geDSIErrorCode = HAL_DSI_FAILED_EXPECTED_NON_NULL_PTR;
  }
  else
  {
    /* if True, Te from External Pin */
    uTriggerCtrlReg = HWIO_OUT_FLD(uTriggerCtrlReg, DSI_0_TRIG_CTRL, TE_SEL,                      (TRUE == psTriggerMode->bExtTEPin?        1:0));
    /* if True, MDP has higher priority than DMA */
    uTriggerCtrlReg = HWIO_OUT_FLD(uTriggerCtrlReg, DSI_0_TRIG_CTRL, COMMAND_MODE_DMA_MDP_ORDER,  (TRUE == psTriggerMode->bOrderMdpDma?     1:0));
    /* if True, PHY has higher priority than MDP */
    uTriggerCtrlReg = HWIO_OUT_FLD(uTriggerCtrlReg, DSI_0_TRIG_CTRL, COMMAND_MODE_MDP_REQ_ORDER,  (TRUE == psTriggerMode->bOrderPHYMdp?     1:0));
    /* if True, Trigger applied to MDP Stream 1 */
    uTriggerCtrlReg = HWIO_OUT_FLD(uTriggerCtrlReg, DSI_0_TRIG_CTRL, COMMAND_MODE_DMA_STREAM_SEL, (TRUE == psTriggerMode->bTriggerStream1?  1:0));

    /* Trigger Setting for MDP */ 
    if( DSI_Display_ImageTrigger_MAX <= psTriggerMode->eImageTrigger )
    {
      uTriggerType    = DSI_Display_CMDTrigger_NONE;
      geDSIErrorCode  = HAL_DSI_FAILED_INVALID_INPUT_PARAMETER;
    }
    else
    {
      uTriggerType    = uMDPTriggerTypeLUT[psTriggerMode->eImageTrigger];
      uTriggerCtrlReg = HWIO_OUT_FLD(uTriggerCtrlReg, DSI_0_TRIG_CTRL, COMMAND_MODE_MDP_TRIGGER_SEL, uTriggerType);
      out_dword(uOffset + HWIO_DSI_0_TRIG_CTRL_ADDR , uTriggerCtrlReg);

      /* Trigger Setting for DMA */ 
      HAL_DSI_SetDMATriggerConfig(eDeviceID, psTriggerMode->eCMDTrigger);
    }
  }

}

/****************************************************************************

HAL_DSI_CMDDatatypeConfig()

Description:
    This API configures the DSI command mode data type.
****************************************************************************/
void HAL_DSI_CMDDatatypeConfig(DSI_Device_IDType              eDeviceID,
                               DSI_CMDDDTType                *psCmdDataType)
{
  uintPtr uOffset                 = HAL_DSI_GetRegBaseOffset(eDeviceID);
  uint32  uCmdMdpDcsCmdCtrlReg    = 0;
  uint32  uCmdDmaNullPacketReg    = 0; 
  uint32  uCmdReadbackDT0Reg      = 0;
  uint32  uCmdReadbackDT1Reg      = 0;
  uint32  uEotpReg                = 0;

  /* Check input Parameters */
  if( NULL == psCmdDataType)
  {
    geDSIErrorCode = HAL_DSI_FAILED_EXPECTED_NON_NULL_PTR;
  }
  else
  {
    /*  */
    uCmdMdpDcsCmdCtrlReg = HWIO_OUT_FLD(uCmdMdpDcsCmdCtrlReg, DSI_0_COMMAND_MODE_MDP_DCS_CMD_CTRL, INSERT_DCS_COMMAND,  0x01);
    uCmdMdpDcsCmdCtrlReg = HWIO_OUT_FLD(uCmdMdpDcsCmdCtrlReg, DSI_0_COMMAND_MODE_MDP_DCS_CMD_CTRL, WR_MEM_CONTINUE,     psCmdDataType->uDCSWriteMemContinueDT);
    uCmdMdpDcsCmdCtrlReg = HWIO_OUT_FLD(uCmdMdpDcsCmdCtrlReg, DSI_0_COMMAND_MODE_MDP_DCS_CMD_CTRL, WR_MEM_START,        psCmdDataType->uDCSWriteMemStartDT);

    /*  */
    uCmdDmaNullPacketReg = HWIO_OUT_FLD(uCmdDmaNullPacketReg, DSI_0_DMA_NULL_PACKET_DATA, NULL_DATATYPE, psCmdDataType->uNullPacketDT);
    uCmdDmaNullPacketReg = HWIO_OUT_FLD(uCmdDmaNullPacketReg, DSI_0_DMA_NULL_PACKET_DATA, NULL_DATATYPE, 0x00);

    /**/
    uCmdReadbackDT0Reg = HWIO_OUT_FLD(uCmdReadbackDT0Reg, DSI_0_RDBK_DATATYPE0, DCS_SHORT_RD_2_BYTE,     psCmdDataType->uReDSCShortRead2ParaDT);
    uCmdReadbackDT0Reg = HWIO_OUT_FLD(uCmdReadbackDT0Reg, DSI_0_RDBK_DATATYPE0, DCS_SHORT_RD_1_BYTE,     psCmdDataType->uRetDSCShortRead1ParaDT );
    uCmdReadbackDT0Reg = HWIO_OUT_FLD(uCmdReadbackDT0Reg, DSI_0_RDBK_DATATYPE0, GENERIC_SHORT_RD_2_BYTE, psCmdDataType->uRetGenericShortRead2ParaDT);
    uCmdReadbackDT0Reg = HWIO_OUT_FLD(uCmdReadbackDT0Reg, DSI_0_RDBK_DATATYPE0, GENERIC_SHORT_RD_1_BYTE, psCmdDataType->uRetGenericShortRead1ParaDT);

    /**/
    uCmdReadbackDT1Reg = HWIO_OUT_FLD(uCmdReadbackDT1Reg, DSI_0_RDBK_DATATYPE1, DCS_LONG_RD,     psCmdDataType->uRetDSCLongReadDT);
    uCmdReadbackDT1Reg = HWIO_OUT_FLD(uCmdReadbackDT1Reg, DSI_0_RDBK_DATATYPE1, GENERIC_LONG_RD, psCmdDataType->uRetGenericLongReadDT );
    uCmdReadbackDT1Reg = HWIO_OUT_FLD(uCmdReadbackDT1Reg, DSI_0_RDBK_DATATYPE1, ERROR_REPORT,    psCmdDataType->uRetAckWithErrorDT);

    /**/
    uEotpReg = HWIO_OUT_FLD(uEotpReg, DSI_0_EOT_PACKET, DI,     psCmdDataType->uEoTpDI);
    uEotpReg = HWIO_OUT_FLD(uEotpReg, DSI_0_EOT_PACKET, WC,     psCmdDataType->uEoTpWC );
    uEotpReg = HWIO_OUT_FLD(uEotpReg, DSI_0_EOT_PACKET, ECC,    psCmdDataType->uEoTpECC);

    out_dword(uOffset + HWIO_DSI_0_COMMAND_MODE_MDP_DCS_CMD_CTRL_ADDR ,  uCmdMdpDcsCmdCtrlReg);
    out_dword(uOffset + HWIO_DSI_0_DMA_NULL_PACKET_DATA_ADDR ,           uCmdDmaNullPacketReg);
    out_dword(uOffset + HWIO_DSI_0_RDBK_DATATYPE0_ADDR ,                 uCmdReadbackDT0Reg);
    out_dword(uOffset + HWIO_DSI_0_RDBK_DATATYPE1_ADDR ,                 uCmdReadbackDT1Reg);
    out_dword(uOffset + HWIO_DSI_0_EOT_PACKET_ADDR ,                     uEotpReg);
  }

}

/****************************************************************************

HAL_DSI_VideoDatatypeConfig()

Description:
    This API configures the DSI Video Mode engine.

****************************************************************************/
void HAL_DSI_VideoDatatypeConfig(DSI_Device_IDType           eDeviceID,
                                 DSI_VideoDTType            *psVideoDataType)
{
  uintPtr uOffset             = HAL_DSI_GetRegBaseOffset(eDeviceID);
  uint32  iVideoSyncDTReg     = 0;
  uint32  iVideoPixelDTReg    = 0;
  uint32  iVideoBlankDTReg    = 0;

  /* Check input Parameters */
  if( NULL == psVideoDataType)
  {
    geDSIErrorCode = HAL_DSI_FAILED_EXPECTED_NON_NULL_PTR;
  }
  else
  {
    /**/
    iVideoSyncDTReg = HWIO_OUT_FLD(iVideoSyncDTReg, DSI_0_VIDEO_MODE_SYNC_DATATYPE, HE, psVideoDataType->uHsyncEndDT );
    iVideoSyncDTReg = HWIO_OUT_FLD(iVideoSyncDTReg, DSI_0_VIDEO_MODE_SYNC_DATATYPE, HS, psVideoDataType->uHsyncStartDT );
    iVideoSyncDTReg = HWIO_OUT_FLD(iVideoSyncDTReg, DSI_0_VIDEO_MODE_SYNC_DATATYPE, VE, psVideoDataType->uVsyncEndDT);
    iVideoSyncDTReg = HWIO_OUT_FLD(iVideoSyncDTReg, DSI_0_VIDEO_MODE_SYNC_DATATYPE, VS, psVideoDataType->uVsyncStartDT);

    /**/
    iVideoPixelDTReg = HWIO_OUT_FLD(iVideoPixelDTReg, DSI_0_VIDEO_MODE_PIXEL_DATATYPE, RGB888,        psVideoDataType->uPackedPacketRGB888DT );
    iVideoPixelDTReg = HWIO_OUT_FLD(iVideoPixelDTReg, DSI_0_VIDEO_MODE_PIXEL_DATATYPE, RGB666,        psVideoDataType->uLooselyPacketRGB666DT );
    iVideoPixelDTReg = HWIO_OUT_FLD(iVideoPixelDTReg, DSI_0_VIDEO_MODE_PIXEL_DATATYPE, RGB666_PACKED, psVideoDataType->uPackedPacketRGB888DT);
    iVideoPixelDTReg = HWIO_OUT_FLD(iVideoPixelDTReg, DSI_0_VIDEO_MODE_PIXEL_DATATYPE, RGB565,        psVideoDataType->uPackedPacketRGB565DT);

    /**/
    iVideoBlankDTReg = HWIO_OUT_FLD(iVideoBlankDTReg, DSI_0_VIDEO_MODE_BLANKING_DATATYPE, BLANK_PKT_DATATYPE, psVideoDataType->uBlankPacketDT );
    iVideoBlankDTReg = HWIO_OUT_FLD(iVideoBlankDTReg, DSI_0_VIDEO_MODE_BLANKING_DATATYPE, BLANK_PKT_DATA,     psVideoDataType->uBlankPacketData );

    out_dword(uOffset + HWIO_DSI_0_VIDEO_MODE_SYNC_DATATYPE_ADDR,     iVideoSyncDTReg   );
    out_dword(uOffset + HWIO_DSI_0_VIDEO_MODE_PIXEL_DATATYPE_ADDR,    iVideoPixelDTReg  );
    out_dword(uOffset + HWIO_DSI_0_VIDEO_MODE_BLANKING_DATATYPE_ADDR, iVideoBlankDTReg  );
  }

}

/****************************************************************************
HAL_DSI_TimeoutConfig()

Description:
   Config DSI Timeout setting in case of loss of packet in transmission.

****************************************************************************/
void HAL_DSI_TimeoutConfig( DSI_Device_IDType           eDeviceID,
                            HAL_DSI_TimeoutType        *psTimeoutConfig)
{
  uintPtr uOffset           = HAL_DSI_GetRegBaseOffset(eDeviceID);
  uint32  uLPTimerCtrlReg   = 0;
  uint32  uHSTimerCtrlReg   = 0;

  /* Check input Parameters */
  if( NULL == psTimeoutConfig)
  {
    geDSIErrorCode = HAL_DSI_FAILED_EXPECTED_NON_NULL_PTR;
  }
  else
  {
    /*DSI Timeout Control for Lower Power Mode Transmission*/
    uLPTimerCtrlReg = HWIO_OUT_FLD(uLPTimerCtrlReg, DSI_0_LP_TIMER_CTRL, BTA_TO,    psTimeoutConfig->uBTATimeout );
    uLPTimerCtrlReg = HWIO_OUT_FLD(uLPTimerCtrlReg, DSI_0_LP_TIMER_CTRL, LP_RX_TO,  psTimeoutConfig->uLPRxTimeout );

    /*DSI Timeout Control for High Speed Mode Transmission*/
    uHSTimerCtrlReg = HWIO_OUT_FLD(uHSTimerCtrlReg, DSI_0_HS_TIMER_CTRL, HS_TX_TO,  psTimeoutConfig->uHSTxTimeout );

    out_dword(uOffset + HWIO_DSI_0_LP_TIMER_CTRL_ADDR, uLPTimerCtrlReg  );
    out_dword(uOffset + HWIO_DSI_0_HS_TIMER_CTRL_ADDR, uHSTimerCtrlReg  );
  }

}

/* -----------------------------------------------------------------------
** Private Functions
** ----------------------------------------------------------------------- */

/****************************************************************************
HAL_DSI_VideoModeConfig()

Description:
   This API configures the DSI video mode engine.
****************************************************************************/
void HAL_DSI_VideoTimingConfig( DSI_Device_IDType           eDeviceID,
                                uint32                      iiVisWidthPx,
                                uint32                      iVisHeightPx,
                                DSI_VideoTimingType        *psVideoTiming)
{
  uintPtr uOffset            = HAL_DSI_GetRegBaseOffset(eDeviceID);
  uint32  iVideoActiveHReg   = 0;
  uint32  iVideoActiveVReg   = 0; 
  uint32  iVideoTotalReg     = 0;
  uint32  iVideoHSyncReg     = 0; 
  uint32  iVideoVSyncReg     = 0;
  uint32  iVideoVSyncVPosReg = 0;
  uint32  iHsyncPeriodDclk   = 0;
  uint32  iVsyncPeriodLine   = 0;

  /* Check input Parameters */
  if( NULL == psVideoTiming)
  {
    geDSIErrorCode = HAL_DSI_FAILED_EXPECTED_NON_NULL_PTR;
  }
  else
  {
    /* Find the horizontal sync period in dot_clks. */
    iHsyncPeriodDclk =  psVideoTiming->iHsyncPulseWidthDclk +
                        psVideoTiming->iHsyncBackPorchDclk  +
                        iiVisWidthPx +
                        psVideoTiming->iHsyncFrontPorchDclk - 1; 

    iVsyncPeriodLine =  psVideoTiming->iVsyncPulseWidthLines+
                        psVideoTiming->iVsyncBackPorchLine  +
                        iVisHeightPx +
                        psVideoTiming->iVsyncFrontPorchLine - 1; 

    /* Video Mode Active H */
    iVideoActiveHReg = HWIO_OUT_FLD(iVideoActiveHReg, DSI_0_VIDEO_MODE_ACTIVE_H, ACTIVE_H_END,    (psVideoTiming->iHsyncBackPorchDclk + iiVisWidthPx + psVideoTiming->iHsyncPulseWidthDclk));
    iVideoActiveHReg = HWIO_OUT_FLD(iVideoActiveHReg, DSI_0_VIDEO_MODE_ACTIVE_H, ACTIVE_H_START,  (psVideoTiming->iHsyncBackPorchDclk + psVideoTiming->iHsyncPulseWidthDclk));

    /* Video Mode Active V */
    iVideoActiveVReg = HWIO_OUT_FLD(iVideoActiveVReg, DSI_0_VIDEO_MODE_ACTIVE_V, ACTIVE_V_END,    (psVideoTiming->iVsyncBackPorchLine + iVisHeightPx + psVideoTiming->iVsyncPulseWidthLines));
    iVideoActiveVReg = HWIO_OUT_FLD(iVideoActiveVReg, DSI_0_VIDEO_MODE_ACTIVE_V, ACTIVE_V_START,  (psVideoTiming->iVsyncBackPorchLine + psVideoTiming->iVsyncPulseWidthLines));

    /* Video Mode Total*/
    iVideoTotalReg = HWIO_OUT_FLD(iVideoTotalReg, DSI_0_VIDEO_MODE_TOTAL, VIDEO_V_TOTAL,    iVsyncPeriodLine );
    iVideoTotalReg = HWIO_OUT_FLD(iVideoTotalReg, DSI_0_VIDEO_MODE_TOTAL, VIDEO_H_TOTAL,    iHsyncPeriodDclk );

    /* Video Mode VSYNC VPOS*/
    iVideoVSyncVPosReg = HWIO_OUT_FLD(iVideoVSyncVPosReg, DSI_0_VIDEO_MODE_VSYNC, VS_END,   (psVideoTiming->iVsyncSkewDclk + psVideoTiming->iVsyncPulseWidthLines));
    iVideoVSyncVPosReg = HWIO_OUT_FLD(iVideoVSyncVPosReg, DSI_0_VIDEO_MODE_VSYNC, VS_START, psVideoTiming->iVsyncSkewDclk );

    /* Video Mode VSYNC*/
    iVideoVSyncReg = HWIO_OUT_FLD(iVideoVSyncReg, DSI_0_VIDEO_MODE_HSYNC, HS_END,   psVideoTiming->iVsyncEnd   );
    iVideoVSyncReg = HWIO_OUT_FLD(iVideoVSyncReg, DSI_0_VIDEO_MODE_HSYNC, HS_START, psVideoTiming->iVsyncStart );

    /* Video Mode HSYNC*/
    iVideoHSyncReg = HWIO_OUT_FLD(iVideoHSyncReg, DSI_0_VIDEO_MODE_VSYNC, VPOS_VS_VPOS_END,   (psVideoTiming->iHsyncSkewDclk+ psVideoTiming->iHsyncPulseWidthDclk));
    iVideoHSyncReg = HWIO_OUT_FLD(iVideoHSyncReg, DSI_0_VIDEO_MODE_VSYNC, VPOS_VS_VPOS_START, psVideoTiming->iHsyncSkewDclk );

    out_dword(uOffset + HWIO_DSI_0_VIDEO_MODE_ACTIVE_H_ADDR,  iVideoActiveHReg    );
    out_dword(uOffset + HWIO_DSI_0_VIDEO_MODE_ACTIVE_V_ADDR,  iVideoActiveVReg    ); 
    out_dword(uOffset + HWIO_DSI_0_VIDEO_MODE_TOTAL_ADDR,     iVideoTotalReg      );
    out_dword(uOffset + HWIO_DSI_0_VIDEO_MODE_HSYNC_ADDR,     iVideoHSyncReg      ); 
    out_dword(uOffset + HWIO_DSI_0_VIDEO_MODE_VSYNC_ADDR,     iVideoVSyncReg      );
    out_dword(uOffset + HWIO_DSI_0_VIDEO_MODE_VSYNC_VPOS_ADDR,iVideoVSyncVPosReg  ); 
  }

}


void dsi_Get_VideoColorInfo ( DSI_ColorFormatType     eColorFormat,
                              HAL_DSI_ColorInfoType  *psColorInfo)
{
  /* Color Format */
  switch (eColorFormat)
  {
  case DSI_COLOR_RGB_565_16BPP:
    psColorInfo->iColorDepth      = HAL_DSI_VIDEO_DST_FOMRAT_RGB565_16BIT;
    psColorInfo->iColorSwap       = HAL_DSI_DST_FOMRAT_SWAP_RGB;
    psColorInfo->iColorRSel       = HAL_DSI_R_SEL_NONE;
    psColorInfo->iColorGSel       = HAL_DSI_G_SEL_NONE;
    psColorInfo->iColorBSel       = HAL_DSI_B_SEL_NONE;
    psColorInfo->iBpp             = 16;
    break;
  case DSI_COLOR_RBG_565_16BPP:
    psColorInfo->iColorDepth      = HAL_DSI_VIDEO_DST_FOMRAT_RGB565_16BIT;
    psColorInfo->iColorSwap       = HAL_DSI_DST_FOMRAT_SWAP_RBG;
    psColorInfo->iColorRSel       = HAL_DSI_R_SEL_NONE;
    psColorInfo->iColorGSel       = HAL_DSI_G_SEL_NONE;
    psColorInfo->iColorBSel       = HAL_DSI_B_SEL_NONE;
    psColorInfo->iBpp             = 16;
    break;
  case DSI_COLOR_BGR_565_16BPP:
    psColorInfo->iColorDepth      = HAL_DSI_VIDEO_DST_FOMRAT_RGB565_16BIT;
    psColorInfo->iColorSwap       = HAL_DSI_DST_FOMRAT_SWAP_BGR;
    psColorInfo->iColorRSel       = HAL_DSI_R_SEL_NONE;
    psColorInfo->iColorGSel       = HAL_DSI_G_SEL_NONE;
    psColorInfo->iColorBSel       = HAL_DSI_B_SEL_NONE;
    psColorInfo->iBpp             = 16;
    break;
  case DSI_COLOR_BRG_565_16BPP:
    psColorInfo->iColorDepth      = HAL_DSI_VIDEO_DST_FOMRAT_RGB565_16BIT;
    psColorInfo->iColorSwap       = HAL_DSI_DST_FOMRAT_SWAP_BRG;
    psColorInfo->iColorRSel       = HAL_DSI_R_SEL_NONE;
    psColorInfo->iColorGSel       = HAL_DSI_G_SEL_NONE;
    psColorInfo->iColorBSel       = HAL_DSI_B_SEL_NONE;
    psColorInfo->iBpp             = 16;
    break;
  case DSI_COLOR_GRB_565_16BPP:
    psColorInfo->iColorDepth      = HAL_DSI_VIDEO_DST_FOMRAT_RGB565_16BIT;
    psColorInfo->iColorSwap       = HAL_DSI_DST_FOMRAT_SWAP_GRB;
    psColorInfo->iColorRSel       = HAL_DSI_R_SEL_NONE;
    psColorInfo->iColorGSel       = HAL_DSI_G_SEL_NONE;
    psColorInfo->iColorBSel       = HAL_DSI_B_SEL_NONE;
    psColorInfo->iBpp             = 16;
    break;
  case DSI_COLOR_GBR_565_16BPP:
    psColorInfo->iColorDepth      = HAL_DSI_VIDEO_DST_FOMRAT_RGB565_16BIT;
    psColorInfo->iColorSwap       = HAL_DSI_DST_FOMRAT_SWAP_GBR;
    psColorInfo->iColorRSel       = HAL_DSI_R_SEL_NONE;
    psColorInfo->iColorGSel       = HAL_DSI_G_SEL_NONE;
    psColorInfo->iColorBSel       = HAL_DSI_B_SEL_NONE;
    psColorInfo->iBpp             = 16;
    break;
    /* */
  case DSI_COLOR_RGB_666_18BPP:
    psColorInfo->iColorDepth      = HAL_DSI_VIDEO_DST_FOMRAT_RGB666_18BIT;
    psColorInfo->iColorSwap       = HAL_DSI_DST_FOMRAT_SWAP_RGB;
    psColorInfo->iColorRSel       = HAL_DSI_R_SEL_NONE;
    psColorInfo->iColorGSel       = HAL_DSI_G_SEL_NONE;
    psColorInfo->iColorBSel       = HAL_DSI_B_SEL_NONE;
    psColorInfo->iBpp             = 18;
    break;
  case DSI_COLOR_RBG_666_18BPP:
    psColorInfo->iColorDepth      = HAL_DSI_VIDEO_DST_FOMRAT_RGB666_18BIT;
    psColorInfo->iColorSwap       = HAL_DSI_DST_FOMRAT_SWAP_RBG;
    psColorInfo->iColorRSel       = HAL_DSI_R_SEL_NONE;
    psColorInfo->iColorGSel       = HAL_DSI_G_SEL_NONE;
    psColorInfo->iColorBSel       = HAL_DSI_B_SEL_NONE;
    psColorInfo->iBpp             = 18;
    break;
  case DSI_COLOR_BGR_666_18BPP:
    psColorInfo->iColorDepth      = HAL_DSI_VIDEO_DST_FOMRAT_RGB666_18BIT;
    psColorInfo->iColorSwap       = HAL_DSI_DST_FOMRAT_SWAP_BGR;
    psColorInfo->iColorRSel       = HAL_DSI_R_SEL_NONE;
    psColorInfo->iColorGSel       = HAL_DSI_G_SEL_NONE;
    psColorInfo->iColorBSel       = HAL_DSI_B_SEL_NONE;
    psColorInfo->iBpp             = 18;
    break;
  case DSI_COLOR_BRG_666_18BPP:
    psColorInfo->iColorDepth      = HAL_DSI_VIDEO_DST_FOMRAT_RGB666_18BIT;
    psColorInfo->iColorSwap       = HAL_DSI_DST_FOMRAT_SWAP_BRG;
    psColorInfo->iColorRSel       = HAL_DSI_R_SEL_NONE;
    psColorInfo->iColorGSel       = HAL_DSI_G_SEL_NONE;
    psColorInfo->iColorBSel       = HAL_DSI_B_SEL_NONE;
    psColorInfo->iBpp             = 18;
    break;
  case DSI_COLOR_GRB_666_18BPP:
    psColorInfo->iColorDepth      = HAL_DSI_VIDEO_DST_FOMRAT_RGB666_18BIT;
    psColorInfo->iColorSwap       = HAL_DSI_DST_FOMRAT_SWAP_GRB;
    psColorInfo->iColorRSel       = HAL_DSI_R_SEL_NONE;
    psColorInfo->iColorGSel       = HAL_DSI_G_SEL_NONE;
    psColorInfo->iColorBSel       = HAL_DSI_B_SEL_NONE;
    psColorInfo->iBpp             = 18;
    break;
  case DSI_COLOR_GBR_666_18BPP:
    psColorInfo->iColorDepth      = HAL_DSI_VIDEO_DST_FOMRAT_RGB666_18BIT;
    psColorInfo->iColorSwap       = HAL_DSI_DST_FOMRAT_SWAP_GBR;
    psColorInfo->iColorRSel       = HAL_DSI_R_SEL_NONE;
    psColorInfo->iColorGSel       = HAL_DSI_G_SEL_NONE;
    psColorInfo->iColorBSel       = HAL_DSI_B_SEL_NONE;
    psColorInfo->iBpp             = 18;
    break;

    /* */
  case DSI_COLOR_RGB_888_24BPP:
    psColorInfo->iColorDepth      = HAL_DSI_VIDEO_DST_FOMRAT_RGB888_24BIT;
    psColorInfo->iColorSwap       = HAL_DSI_DST_FOMRAT_SWAP_RGB;
    psColorInfo->iColorRSel       = HAL_DSI_R_SEL_NONE;
    psColorInfo->iColorGSel       = HAL_DSI_G_SEL_NONE;
    psColorInfo->iColorBSel       = HAL_DSI_B_SEL_NONE;
    psColorInfo->iBpp             = 24;
    break;
  case DSI_COLOR_RBG_888_24BPP:
    psColorInfo->iColorDepth      = HAL_DSI_VIDEO_DST_FOMRAT_RGB888_24BIT;
    psColorInfo->iColorSwap       = HAL_DSI_DST_FOMRAT_SWAP_RBG;
    psColorInfo->iColorRSel       = HAL_DSI_R_SEL_NONE;
    psColorInfo->iColorGSel       = HAL_DSI_G_SEL_NONE;
    psColorInfo->iColorBSel       = HAL_DSI_B_SEL_NONE;
    psColorInfo->iBpp             = 24;
    break;
  case DSI_COLOR_BGR_888_24BPP:
    psColorInfo->iColorDepth      = HAL_DSI_VIDEO_DST_FOMRAT_RGB888_24BIT;
    psColorInfo->iColorSwap       = HAL_DSI_DST_FOMRAT_SWAP_BGR;
    psColorInfo->iColorRSel       = HAL_DSI_R_SEL_NONE;
    psColorInfo->iColorGSel       = HAL_DSI_G_SEL_NONE;
    psColorInfo->iColorBSel       = HAL_DSI_B_SEL_NONE;
    psColorInfo->iBpp             = 24;
    break;
  case DSI_COLOR_BRG_888_24BPP:
    psColorInfo->iColorDepth      = HAL_DSI_VIDEO_DST_FOMRAT_RGB888_24BIT;
    psColorInfo->iColorSwap       = HAL_DSI_DST_FOMRAT_SWAP_BRG;
    psColorInfo->iColorRSel       = HAL_DSI_R_SEL_NONE;
    psColorInfo->iColorGSel       = HAL_DSI_G_SEL_NONE;
    psColorInfo->iColorBSel       = HAL_DSI_B_SEL_NONE;
    psColorInfo->iBpp             = 24;
    break;
  case DSI_COLOR_GRB_888_24BPP:
    psColorInfo->iColorDepth      = HAL_DSI_VIDEO_DST_FOMRAT_RGB888_24BIT;
    psColorInfo->iColorSwap       = HAL_DSI_DST_FOMRAT_SWAP_GRB;
    psColorInfo->iColorRSel       = HAL_DSI_R_SEL_NONE;
    psColorInfo->iColorGSel       = HAL_DSI_G_SEL_NONE;
    psColorInfo->iColorBSel       = HAL_DSI_B_SEL_NONE;
    psColorInfo->iBpp             = 24;
    break;
  case DSI_COLOR_GBR_888_24BPP:
    psColorInfo->iColorDepth      = HAL_DSI_VIDEO_DST_FOMRAT_RGB888_24BIT;
    psColorInfo->iColorSwap       = HAL_DSI_DST_FOMRAT_SWAP_GBR;
    psColorInfo->iColorRSel       = HAL_DSI_R_SEL_NONE;
    psColorInfo->iColorGSel       = HAL_DSI_G_SEL_NONE;
    psColorInfo->iColorBSel       = HAL_DSI_B_SEL_NONE;
    psColorInfo->iBpp             = 24;
    break;
  default:
    geDSIErrorCode = HAL_DSI_FAILED_NO_SUPPORT_OR_NO_HW;
    return;
  }
}

void dsi_Get_CmdColorInfo ( DSI_ColorFormatType     eColorFormat,
                            HAL_DSI_ColorInfoType  *psColorInfo)
{
  /* Color Format */
  switch (eColorFormat)
  {
  case DSI_COLOR_RGB_565_16BPP:
    psColorInfo->iColorDepth      = HAL_DSI_CMDMODE_DST_FOMRAT_RGB565_16BIT;
    psColorInfo->iColorSwap       = HAL_DSI_DST_FOMRAT_SWAP_RGB;
    psColorInfo->iColorRSel       = HAL_DSI_R_SEL_NONE;
    psColorInfo->iColorGSel       = HAL_DSI_G_SEL_NONE;
    psColorInfo->iColorBSel       = HAL_DSI_B_SEL_NONE;
    psColorInfo->iBpp             = 16;
    break;
  case DSI_COLOR_RBG_565_16BPP:
    psColorInfo->iColorDepth      = HAL_DSI_CMDMODE_DST_FOMRAT_RGB565_16BIT;
    psColorInfo->iColorSwap       = HAL_DSI_DST_FOMRAT_SWAP_RBG;
    psColorInfo->iColorRSel       = HAL_DSI_R_SEL_NONE;
    psColorInfo->iColorGSel       = HAL_DSI_G_SEL_NONE;
    psColorInfo->iColorBSel       = HAL_DSI_B_SEL_NONE;
    psColorInfo->iBpp             = 16;
    break;
  case DSI_COLOR_BGR_565_16BPP:
    psColorInfo->iColorDepth      = HAL_DSI_CMDMODE_DST_FOMRAT_RGB565_16BIT;
    psColorInfo->iColorSwap       = HAL_DSI_DST_FOMRAT_SWAP_BGR;
    psColorInfo->iColorRSel       = HAL_DSI_R_SEL_NONE;
    psColorInfo->iColorGSel       = HAL_DSI_G_SEL_NONE;
    psColorInfo->iColorBSel       = HAL_DSI_B_SEL_NONE;
    psColorInfo->iBpp             = 16;
    break;
  case DSI_COLOR_BRG_565_16BPP:
    psColorInfo->iColorDepth      = HAL_DSI_CMDMODE_DST_FOMRAT_RGB565_16BIT;
    psColorInfo->iColorSwap       = HAL_DSI_DST_FOMRAT_SWAP_BRG;
    psColorInfo->iColorRSel       = HAL_DSI_R_SEL_NONE;
    psColorInfo->iColorGSel       = HAL_DSI_G_SEL_NONE;
    psColorInfo->iColorBSel       = HAL_DSI_B_SEL_NONE;
    psColorInfo->iBpp             = 16;
    break;
  case DSI_COLOR_GRB_565_16BPP:
    psColorInfo->iColorDepth      = HAL_DSI_CMDMODE_DST_FOMRAT_RGB565_16BIT;
    psColorInfo->iColorSwap       = HAL_DSI_DST_FOMRAT_SWAP_GRB;
    psColorInfo->iColorRSel       = HAL_DSI_R_SEL_NONE;
    psColorInfo->iColorGSel       = HAL_DSI_G_SEL_NONE;
    psColorInfo->iColorBSel       = HAL_DSI_B_SEL_NONE;
    psColorInfo->iBpp             = 16;
    break;
  case DSI_COLOR_GBR_565_16BPP:
    psColorInfo->iColorDepth      = HAL_DSI_CMDMODE_DST_FOMRAT_RGB565_16BIT;
    psColorInfo->iColorSwap       = HAL_DSI_DST_FOMRAT_SWAP_GBR;
    psColorInfo->iColorRSel       = HAL_DSI_R_SEL_NONE;
    psColorInfo->iColorGSel       = HAL_DSI_G_SEL_NONE;
    psColorInfo->iColorBSel       = HAL_DSI_B_SEL_NONE;
    psColorInfo->iBpp             = 16;
    break;
    /* */
  case DSI_COLOR_RGB_666_18BPP:
    psColorInfo->iColorDepth      = HAL_DSI_CMDMODE_DST_FOMRAT_RGB666_18BIT;
    psColorInfo->iColorSwap       = HAL_DSI_DST_FOMRAT_SWAP_RGB;
    psColorInfo->iColorRSel       = HAL_DSI_R_SEL_NONE;
    psColorInfo->iColorGSel       = HAL_DSI_G_SEL_NONE;
    psColorInfo->iColorBSel       = HAL_DSI_B_SEL_NONE;
    psColorInfo->iBpp             = 18;
    break;
  case DSI_COLOR_RBG_666_18BPP:
    psColorInfo->iColorDepth      = HAL_DSI_CMDMODE_DST_FOMRAT_RGB666_18BIT;
    psColorInfo->iColorSwap       = HAL_DSI_DST_FOMRAT_SWAP_RBG;
    psColorInfo->iColorRSel       = HAL_DSI_R_SEL_NONE;
    psColorInfo->iColorGSel       = HAL_DSI_G_SEL_NONE;
    psColorInfo->iColorBSel       = HAL_DSI_B_SEL_NONE;
    psColorInfo->iBpp             = 18;
    break;
  case DSI_COLOR_BGR_666_18BPP:
    psColorInfo->iColorDepth      = HAL_DSI_CMDMODE_DST_FOMRAT_RGB666_18BIT;
    psColorInfo->iColorSwap       = HAL_DSI_DST_FOMRAT_SWAP_BGR;
    psColorInfo->iColorRSel       = HAL_DSI_R_SEL_NONE;
    psColorInfo->iColorGSel       = HAL_DSI_G_SEL_NONE;
    psColorInfo->iColorBSel       = HAL_DSI_B_SEL_NONE;
    psColorInfo->iBpp             = 18;
    break;
  case DSI_COLOR_BRG_666_18BPP:
    psColorInfo->iColorDepth      = HAL_DSI_CMDMODE_DST_FOMRAT_RGB666_18BIT;
    psColorInfo->iColorSwap       = HAL_DSI_DST_FOMRAT_SWAP_BRG;
    psColorInfo->iColorRSel       = HAL_DSI_R_SEL_NONE;
    psColorInfo->iColorGSel       = HAL_DSI_G_SEL_NONE;
    psColorInfo->iColorBSel       = HAL_DSI_B_SEL_NONE;
    psColorInfo->iBpp             = 18;
    break;
  case DSI_COLOR_GRB_666_18BPP:
    psColorInfo->iColorDepth      = HAL_DSI_CMDMODE_DST_FOMRAT_RGB666_18BIT;
    psColorInfo->iColorSwap       = HAL_DSI_DST_FOMRAT_SWAP_GRB;
    psColorInfo->iColorRSel       = HAL_DSI_R_SEL_NONE;
    psColorInfo->iColorGSel       = HAL_DSI_G_SEL_NONE;
    psColorInfo->iColorBSel       = HAL_DSI_B_SEL_NONE;
    psColorInfo->iBpp             = 18;
    break;
  case DSI_COLOR_GBR_666_18BPP:
    psColorInfo->iColorDepth      = HAL_DSI_CMDMODE_DST_FOMRAT_RGB666_18BIT;
    psColorInfo->iColorSwap       = HAL_DSI_DST_FOMRAT_SWAP_GBR;
    psColorInfo->iColorRSel       = HAL_DSI_R_SEL_NONE;
    psColorInfo->iColorGSel       = HAL_DSI_G_SEL_NONE;
    psColorInfo->iColorBSel       = HAL_DSI_B_SEL_NONE;
    psColorInfo->iBpp             = 18;
    break;

    /* */
  case DSI_COLOR_RGB_888_24BPP:
    psColorInfo->iColorDepth      = HAL_DSI_CMDMODE_DST_FOMRAT_RGB888_24BIT;
    psColorInfo->iColorSwap       = HAL_DSI_DST_FOMRAT_SWAP_RGB;
    psColorInfo->iColorRSel       = HAL_DSI_R_SEL_NONE;
    psColorInfo->iColorGSel       = HAL_DSI_G_SEL_NONE;
    psColorInfo->iColorBSel       = HAL_DSI_B_SEL_NONE;
    psColorInfo->iBpp             = 24;
    break;
  case DSI_COLOR_RBG_888_24BPP:
    psColorInfo->iColorDepth      = HAL_DSI_CMDMODE_DST_FOMRAT_RGB888_24BIT;
    psColorInfo->iColorSwap       = HAL_DSI_DST_FOMRAT_SWAP_RBG;
    psColorInfo->iColorRSel       = HAL_DSI_R_SEL_NONE;
    psColorInfo->iColorGSel       = HAL_DSI_G_SEL_NONE;
    psColorInfo->iColorBSel       = HAL_DSI_B_SEL_NONE;
    psColorInfo->iBpp             = 24;
    break;
  case DSI_COLOR_BGR_888_24BPP:
    psColorInfo->iColorDepth      = HAL_DSI_CMDMODE_DST_FOMRAT_RGB888_24BIT;
    psColorInfo->iColorSwap       = HAL_DSI_DST_FOMRAT_SWAP_BGR;
    psColorInfo->iColorRSel       = HAL_DSI_R_SEL_NONE;
    psColorInfo->iColorGSel       = HAL_DSI_G_SEL_NONE;
    psColorInfo->iColorBSel       = HAL_DSI_B_SEL_NONE;
    psColorInfo->iBpp             = 24;
    break;
  case DSI_COLOR_BRG_888_24BPP:
    psColorInfo->iColorDepth      = HAL_DSI_CMDMODE_DST_FOMRAT_RGB888_24BIT;
    psColorInfo->iColorSwap       = HAL_DSI_DST_FOMRAT_SWAP_BRG;
    psColorInfo->iColorRSel       = HAL_DSI_R_SEL_NONE;
    psColorInfo->iColorGSel       = HAL_DSI_G_SEL_NONE;
    psColorInfo->iColorBSel       = HAL_DSI_B_SEL_NONE;
    psColorInfo->iBpp             = 24;
    break;
  case DSI_COLOR_GRB_888_24BPP:
    psColorInfo->iColorDepth      = HAL_DSI_CMDMODE_DST_FOMRAT_RGB888_24BIT;
    psColorInfo->iColorSwap       = HAL_DSI_DST_FOMRAT_SWAP_GRB;
    psColorInfo->iColorRSel       = HAL_DSI_R_SEL_NONE;
    psColorInfo->iColorGSel       = HAL_DSI_G_SEL_NONE;
    psColorInfo->iColorBSel       = HAL_DSI_B_SEL_NONE;
    psColorInfo->iBpp             = 24;
    break;
  case DSI_COLOR_GBR_888_24BPP:
    psColorInfo->iColorDepth      = HAL_DSI_CMDMODE_DST_FOMRAT_RGB888_24BIT;
    psColorInfo->iColorSwap       = HAL_DSI_DST_FOMRAT_SWAP_GBR;
    psColorInfo->iColorRSel       = HAL_DSI_R_SEL_NONE;
    psColorInfo->iColorGSel       = HAL_DSI_G_SEL_NONE;
    psColorInfo->iColorBSel       = HAL_DSI_B_SEL_NONE;
    psColorInfo->iBpp             = 24;
    break;
  default:
    geDSIErrorCode = HAL_DSI_FAILED_NO_SUPPORT_OR_NO_HW;
    return;
  }
}

#ifdef __cplusplus
}
#endif
