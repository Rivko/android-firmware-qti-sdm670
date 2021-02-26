/*! \file */

/*
===========================================================================

FILE:         hal_mdp_chroma_down.c

DESCRIPTION:  Manages the Chroma down module for HDMI interface and WB2 interface

===========================================================================
Copyright (c) 2014-2017 Qualcomm Technologies, Inc.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================
*/

/*------------------------------------------------------------------------------
 * Include Files
 *----------------------------------------------------------------------------*/
#include "hal_mdp_i.h"

#ifdef __cplusplus
extern "C" {
#endif

/*------------------------------------------------------------------------------
 * Local Defines
 *----------------------------------------------------------------------------*/
/*
 * Chroma down-sample output bits 
 */
#define  HAL_MDP_CDM_10_BIT_OUTPUT                       0x0
#define  HAL_MDP_CDM_8_BIT_OUTPUT                        0x1

/* 
 * Clamp sizes in for Chroma down sample in 10 bit
 */
#define HAL_MDP_CDM_CDWN2_CLAMP_MIN                      0x0
#define HAL_MDP_CDM_CDWN2_CLAMP_MAX                      0x3FF
/*
 * Hdmi packer supported output formats
 */
#define  HAL_MDP_CDM_HDMI_OUTPUT_FORMAT_YUV444           0x0
#define  HAL_MDP_CDM_HDMI_OUTPUT_FORMAT_YUV422           0x1
#define  HAL_MDP_CDM_HDMI_OUTPUT_FORMAT_YUV420           0x3
#define  HAL_MDP_CDM_HDMI_OUTPUT_FORMAT_UNSUPPORTED      0xFF

/*------------------------------------------------------------------------------
 * Local Definitions and constants
 *----------------------------------------------------------------------------*/

/* 
 * Horizontal coefficients for cosite chroma downscale 
 * [22,    0]   signed 9 bits coeff_00 and coeff_01
 * [-52,   0]   signed 9 bits coeff_02 and coeff_03
 * [158, 256]   signed 9 bits coeff_04 and coeff_05
 */
const uint32 MDP_CDWN2_Cosite_Horizontal_Coeff[] = {0x00000016, 0x000001cc, 0x0100009e};

/* 
 * Vertical coefficients for cosite chroma downscale 
 * [4,  8]   unsigned 4 bits coeff_00 and coeff_01
 */
const uint32 MDP_CDWN2_Cosite_Vertical_Coeff[] = {0x00080004};

/* 
 * Horizontal coefficients for offsite chroma downscale 
 * [5,   11]    signed 9 bits coeff_00 and coeff_01
 * [-21, -37]   signed 9 bits coeff_02 and coeff_03
 * [70,  228]   signed 9 bits coeff_04 and coeff_05
 *
 *
 *
 */
const uint32 MDP_CDWN2_Offsite_Horizontal_Coeff[] = {0x000b0005, 0x01db01eb, 0x00e40046};

/* Vertical coefficients for offsite chroma downscale 
 * [2,  6]   unsigned 4 bits coeff_00 and coeff_01
 *
 */
const uint32 MDP_CDWN2_Offsite_Vertical_Coeff[] = {0x00060002};


/****************************************************************************
*
** FUNCTION: HAL_MDP_Get_CDM_BaseOffset()
*/
/*!
* \brief
*     Returns the offset of Chroma down block register offset 
*
* \param [in]   
*
* \retval uint32
*
****************************************************************************/
uintPtr HAL_MDP_Get_CDM_BaseOffset(HAL_MDP_ChromaDownModuleId          eChromaDownModuleId)
{
  // Only one chroma down block exists, this is for future 
  return  0;
}


/****************************************************************************
*
** FUNCTION: HAL_MDP_CDM_ColorEncodeConfig()
*/
/*!
* \brief
*     Set up configurations for Chroma down module
*
* \param [in]   eChromaDownModuleId   - Chroma down module
* \param [in]   psColorEncInfo        - Color encoding configuration information
* \param [in]   uFlags                - Reserved.
*
* \retval HAL_MDSS_ErrorType 
*
****************************************************************************/
static HAL_MDSS_ErrorType HAL_MDP_CDM_ColorEncode_Config(HAL_MDP_ChromaDownModuleId           eChromaDownModuleId,
                                          HAL_MDP_ColorEncodeConfigType                      *psColorEncInfo,
                                          uint32                                              uFlags)
{
  uintPtr                 uRegOffset        = 0x0;
  uint32                 *pCSCMatrixCoeff   = NULL;
  uint32                 *pCSCPreClamp      = NULL;
  uint32                 *pCSCPostClamp     = NULL;
  uint16                 *pCSCPreBias       = NULL;
  uint16                 *pCSCPostBias      = NULL;
  HAL_MDSS_ErrorType      eStatus           = HAL_MDSS_STATUS_SUCCESS;

  switch(psColorEncInfo->eColorEncodeType)
  {
    case HAL_MDP_COLOR_ENCODING_DEFAULT:
    case HAL_MDP_COLOR_ENCODING_REC601:            /** ITU-R BT.601 (CCIR601)  */
      pCSCMatrixCoeff   = pgsMdpHwInfo->pCSC_ColorRGBToYUVEncodingInfo[HAL_MDP_COLOR_ENCODING_REC601]->pCSC_Coeff_Matrix;
      pCSCPreClamp      = pgsMdpHwInfo->pCSC_ColorRGBToYUVEncodingInfo[HAL_MDP_COLOR_ENCODING_REC601]->pCSC10Bit_PreClamp;
      pCSCPostClamp     = pgsMdpHwInfo->pCSC_ColorRGBToYUVEncodingInfo[HAL_MDP_COLOR_ENCODING_REC601]->pCSC10Bit_PostClamp;
      pCSCPreBias       = pgsMdpHwInfo->pCSC_ColorRGBToYUVEncodingInfo[HAL_MDP_COLOR_ENCODING_REC601]->pCSC10Bit_PreBias;
      pCSCPostBias      = pgsMdpHwInfo->pCSC_ColorRGBToYUVEncodingInfo[HAL_MDP_COLOR_ENCODING_REC601]->pCSC10Bit_PostBias;
      break;

    case HAL_MDP_COLOR_ENCODING_REC709:            /** ITU-R BT.709 (CCIR709)  */
      pCSCMatrixCoeff   = pgsMdpHwInfo->pCSC_ColorRGBToYUVEncodingInfo[HAL_MDP_COLOR_ENCODING_REC709]->pCSC_Coeff_Matrix;
      pCSCPreClamp      = pgsMdpHwInfo->pCSC_ColorRGBToYUVEncodingInfo[HAL_MDP_COLOR_ENCODING_REC709]->pCSC10Bit_PreClamp;
      pCSCPostClamp     = pgsMdpHwInfo->pCSC_ColorRGBToYUVEncodingInfo[HAL_MDP_COLOR_ENCODING_REC709]->pCSC10Bit_PostClamp;
      pCSCPreBias       = pgsMdpHwInfo->pCSC_ColorRGBToYUVEncodingInfo[HAL_MDP_COLOR_ENCODING_REC709]->pCSC10Bit_PreBias;
      pCSCPostBias      = pgsMdpHwInfo->pCSC_ColorRGBToYUVEncodingInfo[HAL_MDP_COLOR_ENCODING_REC709]->pCSC10Bit_PostBias;
      break;

    case HAL_MDP_COLOR_ENCODING_FULL_REC601:      /**< ITU.BT-601 SDTV         */   
      pCSCMatrixCoeff   = pgsMdpHwInfo->pCSC_ColorRGBToYUVEncodingInfo[HAL_MDP_COLOR_ENCODING_FULL_REC601]->pCSC_Coeff_Matrix;
      pCSCPreClamp      = pgsMdpHwInfo->pCSC_ColorRGBToYUVEncodingInfo[HAL_MDP_COLOR_ENCODING_FULL_REC601]->pCSC10Bit_PreClamp;
      pCSCPostClamp     = pgsMdpHwInfo->pCSC_ColorRGBToYUVEncodingInfo[HAL_MDP_COLOR_ENCODING_FULL_REC601]->pCSC10Bit_PostClamp;
      pCSCPreBias       = pgsMdpHwInfo->pCSC_ColorRGBToYUVEncodingInfo[HAL_MDP_COLOR_ENCODING_FULL_REC601]->pCSC10Bit_PreBias;
      pCSCPostBias      = pgsMdpHwInfo->pCSC_ColorRGBToYUVEncodingInfo[HAL_MDP_COLOR_ENCODING_FULL_REC601]->pCSC10Bit_PostBias; 
      break;

    case HAL_MDP_COLOR_ENCODING_FULL_REC709:      /**< ITU.BT-709 HDTV         */ 
      pCSCMatrixCoeff   = pgsMdpHwInfo->pCSC_ColorRGBToYUVEncodingInfo[HAL_MDP_COLOR_ENCODING_FULL_REC709]->pCSC_Coeff_Matrix;
      pCSCPreClamp      = pgsMdpHwInfo->pCSC_ColorRGBToYUVEncodingInfo[HAL_MDP_COLOR_ENCODING_FULL_REC709]->pCSC10Bit_PreClamp;
      pCSCPostClamp     = pgsMdpHwInfo->pCSC_ColorRGBToYUVEncodingInfo[HAL_MDP_COLOR_ENCODING_FULL_REC709]->pCSC10Bit_PostClamp;
      pCSCPreBias       = pgsMdpHwInfo->pCSC_ColorRGBToYUVEncodingInfo[HAL_MDP_COLOR_ENCODING_FULL_REC709]->pCSC10Bit_PreBias;
      pCSCPostBias      = pgsMdpHwInfo->pCSC_ColorRGBToYUVEncodingInfo[HAL_MDP_COLOR_ENCODING_FULL_REC709]->pCSC10Bit_PostBias; 
      break; 

    case HAL_MDP_COLOR_ENCODING_sRGB:              /** Standard RGB */     
    case HAL_MDP_COLOR_ENCODING_MAX:
    case HAL_MDP_COLOR_ENCODING_FORCE_32BIT:
    default:
      eStatus     = HAL_MDSS_STATUS_FAILED_NOT_SUPPORTED;
      break;
   }
   
   if(HAL_MDSS_STATUS_SUCCESS == eStatus)
   {
     if ((NULL != pCSCMatrixCoeff) && (NULL != pCSCPreClamp) &&
         (NULL != pCSCPostClamp)   && (NULL != pCSCPreBias ) && 
         (NULL != pCSCPostBias))
     {
       uRegOffset = HAL_MDP_Get_CDM_BaseOffset(eChromaDownModuleId);
       // Matrix Coefficients
       out_dword(HWIO_MDP_CDM_0_CSC_10_MATRIX_COEFF_0_ADDR + uRegOffset, *(pCSCMatrixCoeff + 0x00000000));
       out_dword(HWIO_MDP_CDM_0_CSC_10_MATRIX_COEFF_1_ADDR + uRegOffset, *(pCSCMatrixCoeff + 0x00000001));
       out_dword(HWIO_MDP_CDM_0_CSC_10_MATRIX_COEFF_2_ADDR + uRegOffset, *(pCSCMatrixCoeff + 0x00000002));
       out_dword(HWIO_MDP_CDM_0_CSC_10_MATRIX_COEFF_3_ADDR + uRegOffset, *(pCSCMatrixCoeff + 0x00000003));
       out_dword(HWIO_MDP_CDM_0_CSC_10_MATRIX_COEFF_4_ADDR + uRegOffset, *(pCSCMatrixCoeff + 0x00000004));

       // Preclamp
       out_dword(HWIO_MDP_CDM_0_CSC_10_COMP_0_PRE_CLAMP_ADDR + uRegOffset, *(pCSCPreClamp + 0x00000000));
       out_dword(HWIO_MDP_CDM_0_CSC_10_COMP_1_PRE_CLAMP_ADDR + uRegOffset, *(pCSCPreClamp + 0x00000001));
       out_dword(HWIO_MDP_CDM_0_CSC_10_COMP_2_PRE_CLAMP_ADDR + uRegOffset, *(pCSCPreClamp + 0x00000002));

       // Postclamp
       out_dword(HWIO_MDP_CDM_0_CSC_10_COMP_0_POST_CLAMP_ADDR + uRegOffset, *(pCSCPostClamp + 0x00000000));
       out_dword(HWIO_MDP_CDM_0_CSC_10_COMP_1_POST_CLAMP_ADDR + uRegOffset, *(pCSCPostClamp + 0x00000001));
       out_dword(HWIO_MDP_CDM_0_CSC_10_COMP_2_POST_CLAMP_ADDR + uRegOffset, *(pCSCPostClamp + 0x00000002));

       // Prebias
       out_dword(HWIO_MDP_CDM_0_CSC_10_COMP_0_PRE_BIAS_ADDR + uRegOffset, *(pCSCPreBias + 0x00000000));
       out_dword(HWIO_MDP_CDM_0_CSC_10_COMP_1_PRE_BIAS_ADDR + uRegOffset, *(pCSCPreBias + 0x00000001));
       out_dword(HWIO_MDP_CDM_0_CSC_10_COMP_2_PRE_BIAS_ADDR + uRegOffset, *(pCSCPreBias + 0x00000002));

       // Postbias
       out_dword(HWIO_MDP_CDM_0_CSC_10_COMP_0_POST_BIAS_ADDR + uRegOffset, *(pCSCPostBias + 0x00000000));
       out_dword(HWIO_MDP_CDM_0_CSC_10_COMP_1_POST_BIAS_ADDR + uRegOffset, *(pCSCPostBias + 0x00000001));
       out_dword(HWIO_MDP_CDM_0_CSC_10_COMP_2_POST_BIAS_ADDR + uRegOffset, *(pCSCPostBias + 0x00000002));
    }
  }
   return eStatus;
}
/****************************************************************************
*
** FUNCTION: HAL_MDP_CDM_DownSampling_Config()
*/
/*!
* \brief
*     Set up down-sampler coefficients for Chroma down module 
*
* \param [in]   eChromaDownModuleId   - Chroma down module
* \param [in]   psChromaDownConfig    - Chroma down sampler configuration 
* \param [in]   uFlags                - Reserved.
*
* \retval HAL_MDSS_ErrorType 
*
****************************************************************************/
static HAL_MDSS_ErrorType HAL_MDP_CDM_Coefficients_Config(HAL_MDP_ChromaDownModuleId              eChromaDownModuleId,
                                                          HAL_MDP_ChromaDownSampleConfigType      *psChromaDownConfig,
                                                          uint32                                   uFlags)
{
  uintPtr                  uRegOffset;
  uint32                   uRegOpModeVal     = 0X00000000;
  HAL_MDSS_ErrorType       eRetStatus        = HAL_MDSS_STATUS_SUCCESS;

  uRegOffset = HAL_MDP_Get_CDM_BaseOffset(eChromaDownModuleId);

  uRegOpModeVal  = in_dword(HWIO_MDP_CDM_0_CDWN2_OP_MODE_ADDR + uRegOffset);

  // program the down sample methods
  uRegOpModeVal  = HWIO_OUT_FLD(uRegOpModeVal, MDP_CDM_0_CDWN2_OP_MODE, METHOD_H, psChromaDownConfig->eHorDownSampleMethod);
  uRegOpModeVal  = HWIO_OUT_FLD(uRegOpModeVal, MDP_CDM_0_CDWN2_OP_MODE, METHOD_V, psChromaDownConfig->eVerDownSampleMethod);

  // Program the Horizontal Coefficients based on the Method selected
  if (HAL_MDP_CHROMA_DOWNSAMPLE_METHOD_TYPE_COSITE == psChromaDownConfig->eHorDownSampleMethod)
  {
    out_dword(HWIO_MDP_CDM_0_CDWN2_COEFF_COSITE_H_0_ADDR + uRegOffset, MDP_CDWN2_Cosite_Horizontal_Coeff[0]);
    out_dword(HWIO_MDP_CDM_0_CDWN2_COEFF_COSITE_H_1_ADDR + uRegOffset, MDP_CDWN2_Cosite_Horizontal_Coeff[1]);
    out_dword(HWIO_MDP_CDM_0_CDWN2_COEFF_COSITE_H_2_ADDR + uRegOffset, MDP_CDWN2_Cosite_Horizontal_Coeff[2]);
  }
  else if (HAL_MDP_CHROMA_DOWNSAMPLE_METHOD_TYPE_OFFSITE == psChromaDownConfig->eHorDownSampleMethod)
  {
    out_dword(HWIO_MDP_CDM_0_CDWN2_COEFF_OFFSITE_H_0_ADDR + uRegOffset, MDP_CDWN2_Offsite_Horizontal_Coeff[0]);
    out_dword(HWIO_MDP_CDM_0_CDWN2_COEFF_OFFSITE_H_1_ADDR + uRegOffset, MDP_CDWN2_Offsite_Horizontal_Coeff[1]);
    out_dword(HWIO_MDP_CDM_0_CDWN2_COEFF_OFFSITE_H_2_ADDR + uRegOffset, MDP_CDWN2_Offsite_Horizontal_Coeff[2]);
  }
  // Program the Vertical Coefficients based on the Method selected
  if (HAL_MDP_CHROMA_DOWNSAMPLE_METHOD_TYPE_COSITE == psChromaDownConfig->eVerDownSampleMethod)
  {
    out_dword(HWIO_MDP_CDM_0_CDWN2_COEFF_COSITE_V_ADDR + uRegOffset, MDP_CDWN2_Cosite_Vertical_Coeff[0]);
  } 
  else if (HAL_MDP_CHROMA_DOWNSAMPLE_METHOD_TYPE_OFFSITE == psChromaDownConfig->eVerDownSampleMethod)
  {
    out_dword(HWIO_MDP_CDM_0_CDWN2_COEFF_OFFSITE_V_ADDR + uRegOffset, MDP_CDWN2_Offsite_Vertical_Coeff[0]);
  }

  out_dword(HWIO_MDP_CDM_0_CDWN2_OP_MODE_ADDR, uRegOpModeVal);

  return eRetStatus;
}
 
/****************************************************************************
*
** FUNCTION: HAL_MDP_CDM_3DFrame_Config()
*/
/*!
* \brief
*     Sets up 3D frame configuration for Chroma down module 
*
* \param [in]   eChromaDownModuleId   - Chroma down module
* \param [in]   ps3DConfigInfo        - Chroma down sampler 3D frame configuration 
* \param [in]   uFlags                - Reserved.
*
* \retval HAL_MDSS_ErrorType 
*
****************************************************************************/
static HAL_MDSS_ErrorType HAL_MDP_CDM_3DFrame_Config(HAL_MDP_ChromaDownModuleId              eChromaDownModuleId,
                                                     HAL_MDP_ChromaDown3DConfigType           *ps3DConfigInfo,
                                                     uint32                                   uFlags)
{
  
  uintPtr                    uCDMRegOffset           = 0;              // Chroma Down module offset
  uint32                     uRegOpModeVal           = 0;              // Chroma down sampler OP MODE register 
  uint32                     u3dframeWidthHeight     = 0;              // Pixel Width/Height for SBS/TB frame type 
  uint32                     u3dParamActiveGap       = 0;              // Vertical active gap for Frame packed format
  HAL_MDSS_ErrorType         eRetStatus              = HAL_MDSS_STATUS_SUCCESS;

  // Extract the base address to program the HW registers
  uCDMRegOffset =  HAL_MDP_Get_CDM_BaseOffset(eChromaDownModuleId);
  
  uRegOpModeVal     = in_dword(HWIO_MDP_CDM_0_CDWN2_OP_MODE_ADDR + uCDMRegOffset);

  if (HAL_MDP_3D_FORMAT_TYPE_MAX < ps3DConfigInfo->e3DFrameFormat)
  {
    // set 3D frame type
    uRegOpModeVal  = HWIO_OUT_FLD(uRegOpModeVal, MDP_CDM_0_CDWN2_OP_MODE, MODE_3D, (uint32)ps3DConfigInfo->e3DFrameFormat);

    // program the frame offsets
    switch (ps3DConfigInfo->e3DFrameFormat)
    {
      case HAL_MDP_3D_FORMAT_TYPE_SBS:
        u3dframeWidthHeight  = HWIO_OUT_FLD(u3dframeWidthHeight, MDP_CDM_0_CDWN2_PARAMS_3D_0, OFFSET_3D_H, ps3DConfigInfo->u3DFrameOffset);
        u3dframeWidthHeight  = HWIO_OUT_FLD(u3dframeWidthHeight, MDP_CDM_0_CDWN2_PARAMS_3D_0, OFFSET_3D_V, 0x0);
        break;
      case HAL_MDP_3D_FORMAT_TYPE_TB:
        u3dframeWidthHeight  = HWIO_OUT_FLD(u3dframeWidthHeight, MDP_CDM_0_CDWN2_PARAMS_3D_0, OFFSET_3D_H, 0x0);
        u3dframeWidthHeight  = HWIO_OUT_FLD(u3dframeWidthHeight, MDP_CDM_0_CDWN2_PARAMS_3D_0, OFFSET_3D_V, ps3DConfigInfo->u3DFrameOffset);
        break;
      case HAL_MDP_3D_FORMAT_TYPE_FRAMEPACKED:
        u3dframeWidthHeight  = HWIO_OUT_FLD(u3dframeWidthHeight, MDP_CDM_0_CDWN2_PARAMS_3D_0, OFFSET_3D_H, 0x0);
        u3dframeWidthHeight  = HWIO_OUT_FLD(u3dframeWidthHeight, MDP_CDM_0_CDWN2_PARAMS_3D_0, OFFSET_3D_V, ps3DConfigInfo->u3DFrameOffset);
        u3dParamActiveGap    = HWIO_OUT_FLD(u3dParamActiveGap, MDP_CDM_0_CDWN2_PARAMS_3D_1, OFFSET_3D_VGAP, ps3DConfigInfo->u3DFrameOffset);
        break;
      default:
        break;
    }

    out_dword(HWIO_MDP_CDM_0_CDWN2_OP_MODE_ADDR + uCDMRegOffset, uRegOpModeVal);
    out_dword(HWIO_MDP_CDM_0_CDWN2_PARAMS_3D_0_ADDR + uCDMRegOffset, u3dframeWidthHeight);
    out_dword(HWIO_MDP_CDM_0_CDWN2_PARAMS_3D_1_ADDR + uCDMRegOffset, u3dParamActiveGap);

  } 
  else
  {
    eRetStatus = HAL_MDSS_STATUS_FAILED_INVALID_INPUT_PARAMETER;
  }

 return eRetStatus;
}
/****************************************************************************
*
** FUNCTION: HAL_MDP_DataPath_OutRect_Config()
*/
/*!
* \brief
*     Configures output rectangle for given data path
*
* \param [in]   eChromaDownModuleId  - Chroma down module 
* \param [in]   psRectConfig         - Source rectangle information
* \param [in]   uFlags               - Reserved.
*
* \retval NONE
*
****************************************************************************/
 void HAL_MDP_CDM_OutRect_Config( HAL_MDP_ChromaDownModuleId  eChromaDownModuleId,
                                  HAL_MDP_RectType            *psRectConfig,
                                  uint32                      uFlags)
{
   uintPtr        uRegOffset         = 0;     // Register offset
   uint32         uRegValue          = 0;

   // Extract the base address to program the HW registers
    uRegOffset =  HAL_MDP_Get_CDM_BaseOffset(eChromaDownModuleId);
      
   uRegValue  = HWIO_OUT_FLD(uRegValue, MDP_CDM_0_CDWN2_OUT_SIZE, HEIGHT, psRectConfig->uHeightInPixels);
   uRegValue  = HWIO_OUT_FLD(uRegValue, MDP_CDM_0_CDWN2_OUT_SIZE, WIDTH, psRectConfig->uWidthInPixels);

   out_dword(HWIO_MDP_CDM_0_CDWN2_OUT_SIZE_ADDR + uRegOffset,uRegValue);
}

/****************************************************************************
*
** FUNCTION: HAL_MDP_CDM_Output_Format_Config()
*/
/*!
* \brief
*     programs the Chroma down module CSC configuration and chroma down-sampler
*
*
* \param [in]   eChromaDownModuleId    - Chroma down module Id
* \param [in]   eInterfaceId           - Interface Id for routing the output 
* \param [in]   ePixelFormat           - HAL Pixel format.
* \param [in]   uFlags                 - Reserved.
*
* \retval HAL_MDSS_ErrorType
*
****************************************************************************/
HAL_MDSS_ErrorType HAL_MDP_CDM_Output_Format_Config(HAL_MDP_ChromaDownModuleId     eChromaDownModuleId,
                                                    HAL_MDP_InterfaceId            eInterfaceId,
                                                    HAL_MDP_PixelFormatType        ePixelFormat,
                                                    uint32                         uFlags)
{
  uintPtr                    uCDMRegOffset           = 0;              // Chroma Down module offset
  uint32                     uCSC10BitMode           = 0;              // CSC-10 bit inside Chroma down
  uint32                     uCdwn2OpMode            = 0;              // Chroma down sampler OP MODE register 
  uint32                     uClampSize              = 0;              // Chroma down module output clamp Min/Max values
  uint32                     uChromaBlockEn          = 0;
  uint32                     uHDMIPackFormat         = 0;
  uint32                     uHdmiPack               = 0;
  HAL_MDSS_ErrorType         eRetStatus              = HAL_MDSS_STATUS_SUCCESS;
 
  // Extract the base address to program the HW registers
  uCDMRegOffset =  HAL_MDP_Get_CDM_BaseOffset(eChromaDownModuleId);

  uCdwn2OpMode  = in_dword(HWIO_MDP_CDM_0_CDWN2_OP_MODE_ADDR + uCDMRegOffset); 

  // YUV
  if (HAL_MDP_IS_MDP_PIXEL_YUV_FORMAT(ePixelFormat))
  {
    uint32   uChromaSampleInfo  = HWIO_GETVAL(MDP_VP_0_VIG_0_SSPP_SRC_FORMAT, guSrcFormatInfo[ePixelFormat], SRC_CHROMA_SAMP);

    // 1.) Enable color conversion
    uCSC10BitMode = HWIO_OUT_FLD(uCSC10BitMode,MDP_CDM_0_CSC_10_OP_MODE, EN, TRUE);
    uCSC10BitMode = HWIO_OUT_FLD(uCSC10BitMode,MDP_CDM_0_CSC_10_OP_MODE, SRC_DATA_FORMAT, 0x0);                  // RGB888
    uCSC10BitMode = HWIO_OUT_FLD(uCSC10BitMode,MDP_CDM_0_CSC_10_OP_MODE, DST_DATA_FORMAT, 0x1);                  // YUV
    uCdwn2OpMode  = HWIO_OUT_FLD(uCdwn2OpMode, MDP_CDM_0_CDWN2_OP_MODE, BITS_OUT, HAL_MDP_CDM_8_BIT_OUTPUT);    // 8-bit output

    // YUV clamp size
    uClampSize = HWIO_OUT_FLD(uClampSize, MDP_CDM_0_CDWN2_CLAMP_OUT, MIN, HAL_MDP_CDM_CDWN2_CLAMP_MIN);
    uClampSize = HWIO_OUT_FLD(uClampSize, MDP_CDM_0_CDWN2_CLAMP_OUT, MAX, HAL_MDP_CDM_CDWN2_CLAMP_MAX);

    // 2.) Configure the Down-scalar
    switch (uChromaSampleInfo)
    {
      case HAL_MDP_SRC_FORMAT_CHROMA_SAMPLE_YUV444:
        uCdwn2OpMode    = HWIO_OUT_FLD(uCdwn2OpMode, MDP_CDM_0_CDWN2_OP_MODE, EN, FALSE);                   // No down sampling required
        uHDMIPackFormat = HAL_MDP_CDM_HDMI_OUTPUT_FORMAT_YUV444;
        break;
      case HAL_MDP_SRC_FORMAT_CHROMA_SAMPLE_YUV422_H1V2:
        uCdwn2OpMode    = HWIO_OUT_FLD(uCdwn2OpMode, MDP_CDM_0_CDWN2_OP_MODE, EN, TRUE);                    // Chroma Down Sample
        uCdwn2OpMode    = HWIO_OUT_FLD(uCdwn2OpMode, MDP_CDM_0_CDWN2_OP_MODE, DWNS_V, TRUE);                // Vertical down sample Enabled
        uCdwn2OpMode    = HWIO_OUT_FLD(uCdwn2OpMode, MDP_CDM_0_CDWN2_OP_MODE, DWNS_H, FALSE);
        uHDMIPackFormat = HAL_MDP_CDM_HDMI_OUTPUT_FORMAT_UNSUPPORTED;
        break;
      case HAL_MDP_SRC_FORMAT_CHROMA_SAMPLE_YUV422_H2V1:
        uCdwn2OpMode    = HWIO_OUT_FLD(uCdwn2OpMode, MDP_CDM_0_CDWN2_OP_MODE, EN, TRUE);                    // Chroma Down Sample
        uCdwn2OpMode    = HWIO_OUT_FLD(uCdwn2OpMode, MDP_CDM_0_CDWN2_OP_MODE, DWNS_V, FALSE);
        uCdwn2OpMode    = HWIO_OUT_FLD(uCdwn2OpMode, MDP_CDM_0_CDWN2_OP_MODE, DWNS_H, TRUE);               // Horizontal down sample Enabled
        uHDMIPackFormat = HAL_MDP_CDM_HDMI_OUTPUT_FORMAT_YUV422;
        break;
      case HAL_MDP_SRC_FORMAT_CHROMA_SAMPLE_YUV420_N2V1:
      case HAL_MDP_SRC_FORMAT_CHROMA_SAMPLE_YUV420_N1V2:
        uCdwn2OpMode    = HWIO_OUT_FLD(uCdwn2OpMode, MDP_CDM_0_CDWN2_OP_MODE, EN, TRUE);                    // Chroma Down Sample
        uCdwn2OpMode    = HWIO_OUT_FLD(uCdwn2OpMode, MDP_CDM_0_CDWN2_OP_MODE, DWNS_V, TRUE);               // Horizontal down sample Enabled
        uCdwn2OpMode    = HWIO_OUT_FLD(uCdwn2OpMode, MDP_CDM_0_CDWN2_OP_MODE, DWNS_H, TRUE);               // Vertical down sample Enabled
        uHDMIPackFormat = HAL_MDP_CDM_HDMI_OUTPUT_FORMAT_YUV420;
        break;
      default:
        eRetStatus      = HAL_MDSS_STATUS_FAILED_NO_HW_SUPPORT;
    }

    if (HAL_MDSS_STATUS_SUCCESS == eRetStatus)
    {
      // 3) Enable Output to HDMI Interface
      if (HAL_MDP_INTERFACE_3 == eInterfaceId)
      {
        // Configure the HDMI packer 
        uChromaBlockEn = HWIO_OUT_FLD(uChromaBlockEn, MDP_MDP_OUT_CTL_0,INTF_3_CDWN_EN,TRUE);
        uHdmiPack      = HWIO_OUT_FLD(uHdmiPack, MDP_CDM_0_HDMI_PACK_OP_MODE, EN, TRUE);

        if (HAL_MDP_CDM_HDMI_OUTPUT_FORMAT_UNSUPPORTED != uHDMIPackFormat)
        {
          uHdmiPack = HWIO_OUT_FLD(uHdmiPack, MDP_CDM_0_HDMI_PACK_OP_MODE, FORMAT,uHDMIPackFormat);
        }
        else
        {
          eRetStatus = HAL_MDSS_STATUS_FAILED_NO_HW_SUPPORT;
        }
      }
      // 3) Enable out to Writeback 2 interface
      else
      {
        uHdmiPack      = HWIO_OUT_FLD(uHdmiPack, MDP_CDM_0_HDMI_PACK_OP_MODE, EN, FALSE);
        uChromaBlockEn = HWIO_OUT_FLD(uChromaBlockEn, MDP_MDP_OUT_CTL_0,WB_2_CDWN_EN,TRUE);
      }
    }
  }
  // RGB
  else
  {
    uChromaBlockEn = HWIO_OUT_FLD(uChromaBlockEn, MDP_MDP_OUT_CTL_0,WB_2_CDWN_EN, FALSE);
    uCSC10BitMode = HWIO_OUT_FLD(uCSC10BitMode,MDP_CDM_0_CSC_10_OP_MODE, EN, FALSE);
    uCdwn2OpMode  = HWIO_OUT_FLD(uCdwn2OpMode, MDP_CDM_0_CDWN2_OP_MODE, EN, FALSE);
  }

  if (HAL_MDSS_STATUS_SUCCESS == eRetStatus)
  {
    out_dword(HWIO_MDP_CDM_0_CSC_10_OP_MODE_ADDR    + uCDMRegOffset, uCSC10BitMode);
    out_dword(HWIO_MDP_CDM_0_CDWN2_OP_MODE_ADDR     + uCDMRegOffset, uCdwn2OpMode);
    out_dword(HWIO_MDP_CDM_0_CDWN2_CLAMP_OUT_ADDR   + uCDMRegOffset, uClampSize);
    out_dword(HWIO_MDP_CDM_0_HDMI_PACK_OP_MODE_ADDR + uCDMRegOffset, uHdmiPack);
    out_dword(HWIO_MDP_MDP_OUT_CTL_0_ADDR           + uCDMRegOffset, uChromaBlockEn);
  }

  return eRetStatus;
}

/****************************************************************************
*
** FUNCTION: HAL_MDP_CDM_Setup()
*/
/*!
* \brief
*     Configures the Chroma down module
*
*
* \param [in]   psDataPathConfig    - Data path configuration data
* \param [in]   uFlags              - Reserved.
*
* \retval HAL_MDSS_ErrorType
*
****************************************************************************/
HAL_MDSS_ErrorType HAL_MDP_CDM_Setup(HAL_MDP_ChromaDownModuleId           eChromaDownModuleId,
                                     HAL_MDP_ChromaDownModuleConfigType  *psChromadownConfig,
                                     uint32                               uFlags)
{

   HAL_MDSS_ErrorType            eStatus     = HAL_MDSS_STATUS_SUCCESS;

   /* CSC 10-bit configuration */
   if (NULL != psChromadownConfig->psColorEncInfo)
   {
       eStatus = HAL_MDP_CDM_ColorEncode_Config(eChromaDownModuleId, psChromadownConfig->psColorEncInfo, 0x00);
   }

   /* Chroma down sampling Coefficients configuration */
   if((HAL_MDSS_STATUS_SUCCESS == eStatus) && (NULL != psChromadownConfig->psChromaDown2Info))
   {
      eStatus = HAL_MDP_CDM_Coefficients_Config(eChromaDownModuleId, psChromadownConfig->psChromaDown2Info, 0x00);
   }

   /* 3D configuration*/
   if((HAL_MDSS_STATUS_SUCCESS == eStatus) && (NULL != psChromadownConfig->ps3DConfigInfo))
   {
      eStatus = HAL_MDP_CDM_3DFrame_Config(eChromaDownModuleId, psChromadownConfig->ps3DConfigInfo, 0x00);
   }
   return eStatus;
}
/****************************************************************************
*
** FUNCTION: HAL_MDP_CDM_Init()
*/
/*!
* \brief
*     Initializes the Chroma down block
*
* \param [in] uInitFlags            - Internal modules to be initialized
* \param [in] uFlags                - Reserved
*
* \retval NONE
*
****************************************************************************/
void HAL_MDP_CDM_Init(HAL_MDP_ChromaDownModuleId   eChromaDownModuleId,
                      uint32                       uInitFlags,
                      uint32                       uFlags)
{
   if(0x00 != (HAL_MDP_CDM_FEATURE_CSC & uInitFlags))
   {       
      HAL_MDP_ColorEncodeConfigType       sColorEncInfo; 

      /* Default to limited REC601 - RGB to YUV color space conversion */
      sColorEncInfo.eColorEncodeType   =  HAL_MDP_COLOR_ENCODING_REC601;

      /* Configure the hardware with the default color encoding */
      HAL_MDP_CDM_ColorEncode_Config(eChromaDownModuleId, &sColorEncInfo, 0x00);
   }

   if(0x00 != (HAL_MDP_CDM_FEATURE_CDWN2 & uInitFlags))
   {       
      HAL_MDP_ChromaDownSampleConfigType  sChromaDownConfig;

      /* Default to Pixel drop method in both directions */
      sChromaDownConfig.eHorDownSampleMethod   =  HAL_MDP_CHROMA_DOWNSAMPLE_METHOD_TYPE_PIXELDROP;
      sChromaDownConfig.eVerDownSampleMethod   =  HAL_MDP_CHROMA_DOWNSAMPLE_METHOD_TYPE_PIXELDROP;

      /* Configure the hardware with the default color encoding */
      HAL_MDP_CDM_Coefficients_Config(eChromaDownModuleId, &sChromaDownConfig, 0x00);
   }
}


