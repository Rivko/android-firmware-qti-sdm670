/*! \file */

/*
===========================================================================

FILE:         hal_mdp_layermixer.c

DESCRIPTION: Manages the MDP (Display) Interface
  
===========================================================================
  Copyright (c) 2012-2017 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
===========================================================================
*/

/* -----------------------------------------------------------------------
 * Includes
 * ----------------------------------------------------------------------- */

#include "hal_mdp_i.h"

#ifdef __cplusplus
extern "C" {
#endif

/****************************************************************************
*
** FUNCTION: HAL_MDP_Get_LayerMixer_BaseOffset()
*/
/*!
* \brief
*     Returns offset of the layer mixer register
*
* \param [in] eLayerMixerId         - Layer Mixer ID (0, 1, 2, 3, and 4)
*
* \retval  uintPtr
*
****************************************************************************/
static uintPtr HAL_MDP_Get_LayerMixer_BaseOffset(HAL_MDP_LayerMixerId  eLayerMixerId)
{
   uintPtr      uRegOffset  = 0x0;

   if((HAL_MDP_LAYER_MIXER_NONE  == eLayerMixerId) ||
      (HAL_MDP_LAYER_MIXER_MAX   <= eLayerMixerId))
   {
      HAL_MDP_ASSERT();
   }
   else
   {
      uRegOffset = uMDPLayerMixerRegBaseOffset[eLayerMixerId];
   }
   return uRegOffset;
}

/****************************************************************************
*
** FUNCTION: HAL_MDP_Get_LayerMixer_BlendStage_BaseOffset()
*/
/*!
* \brief
*     Returns offset of the layer mixer register
*
* \param [in] eBlendStageId         - Blend Stage ID (0, 1, 2 )
*
* \retval  uintPtr
*
****************************************************************************/
static uintPtr HAL_MDP_Get_LayerMixer_BlendStage_BaseOffset(HAL_MDP_BlendStageId eBlendStageId)
{
   uintPtr   uRegOffset     = 0x0;

   if(HAL_MDP_GET_LM_BLEND_STAGES() <= (uint32)eBlendStageId)
   {
      HAL_MDP_ASSERT();
   }
   else
   {
      uRegOffset = uMDPLayerMixerBlendStateRegBaseOffset[eBlendStageId];
   }
   return uRegOffset;
}

/****************************************************************************
*
** FUNCTION: HAL_MDP_LayerMixer_OutputSize_Config()
*/
/*!
* \brief
*     Initializes the layer mixer Gamma Correction Block (Area Reduced Gamma Correction)
*
* \param [in] eLayerMixerId            - Layer Mixer ID (0, 1, 2, 3, and 4)
* \param [in] pMixerOutputSizeConfig   - Mixer output size configuration
* \param [in] uFlags                   - Reserved flags
*
* \retval NONE
*
****************************************************************************/
static void HAL_MDP_LayerMixer_OutputSize_Config(HAL_MDP_LayerMixerId                eLayerMixerId,
                                                 HAL_MDP_Mixer_OutputSizeConfigType *pMixerOutputSizeConfig,
                                                 uint32                              uFlags)
{
   uintPtr              uMixerOffset      = HAL_MDP_Get_LayerMixer_BaseOffset(eLayerMixerId);
   uint32               uOutputSize       = 0x00000000;
   uint32               uLeftRightSel     = 0x00000000;

   // Output Width
   uOutputSize = HWIO_OUT_FLD(uOutputSize, 
                              MDP_VP_0_LAYER_0_OUT_SIZE, 
                              DST_W,
                              pMixerOutputSizeConfig->uOutputWidthInPixels);
   // Output Height
   uOutputSize = HWIO_OUT_FLD(uOutputSize, 
                              MDP_VP_0_LAYER_0_OUT_SIZE, 
                              DST_H,
                              pMixerOutputSizeConfig->uOutputHeightInPixels);

   out_dword(HWIO_MDP_VP_0_LAYER_0_OUT_SIZE_ADDR + uMixerOffset, uOutputSize);

   
   uLeftRightSel = in_dword(HWIO_MDP_VP_0_LAYER_0_OP_MODE_ADDR + uMixerOffset);

   // Right Mixer Select for Split Display
   uLeftRightSel = HWIO_OUT_FLD(uLeftRightSel, 
                                MDP_VP_0_LAYER_0_OP_MODE, 
                                SPLIT_LEFT_RIGHT, 
                                (uFlags & HAL_MDP_FLAGS_SPLIT_DISPLAY_RIGHT_MIXER)? 1 : 0);
   
   out_dword(HWIO_MDP_VP_0_LAYER_0_OP_MODE_ADDR + uMixerOffset, uLeftRightSel);
   
}

/****************************************************************************
*
** FUNCTION: HAL_MDP_LayerMixer_BlendStage_Color3SelectConfig()
*/
/*!
* \brief
*     Selection of color3 (Alpha) component at the output of blend stage.
*
* \param [in] eLayerMixerId         - Layer Mixer ID (0, 1, 2, 3, and 4)
* \param [in] eBlendStageId         - Blend stage for configuration
* \param [in] bSelectC3FromFG       - TRUE : Select C3 from the blend stage from Foreground layer
* \param [in] uFlags                - Reserved flags
*
* \retval NONE
*
****************************************************************************/
static void HAL_MDP_LayerMixer_BlendStage_Color3SelectConfig(HAL_MDP_LayerMixerId                eLayerMixerId,
                                                             HAL_MDP_BlendStageId                eBlendStageId,
                                                             bool32                              bSelectC3FromFG,
                                                             uint32                              uFlags)
{
   uintPtr  uMixerOffset         = HAL_MDP_Get_LayerMixer_BaseOffset(eLayerMixerId);
   uint32   uBlendStageAlphaSel  = 0x00000000;

   uBlendStageAlphaSel = in_dword(HWIO_MDP_VP_0_LAYER_0_OP_MODE_ADDR + uMixerOffset);

   switch(eBlendStageId)
   {
   case HAL_MDP_BLEND_STAGE_0:
      {
         uBlendStageAlphaSel = HWIO_OUT_FLD(uBlendStageAlphaSel, MDP_VP_0_LAYER_0_OP_MODE, BLEND0_COLOR3_OUT, bSelectC3FromFG);
      }
      break;

   case HAL_MDP_BLEND_STAGE_1:
      {
         uBlendStageAlphaSel = HWIO_OUT_FLD(uBlendStageAlphaSel, MDP_VP_0_LAYER_0_OP_MODE, BLEND1_COLOR3_OUT, bSelectC3FromFG);
      }
      break;

   case HAL_MDP_BLEND_STAGE_2:
      {
         uBlendStageAlphaSel = HWIO_OUT_FLD(uBlendStageAlphaSel, MDP_VP_0_LAYER_0_OP_MODE, BLEND2_COLOR3_OUT, bSelectC3FromFG);
      }
      break;

   case HAL_MDP_BLEND_STAGE_3:
      {
         uBlendStageAlphaSel = HWIO_OUT_FLD(uBlendStageAlphaSel, MDP_VP_0_LAYER_0_OP_MODE, BLEND3_COLOR3_OUT, bSelectC3FromFG);
      }
      break;

   case HAL_MDP_BLEND_STAGE_4:
      {
         uBlendStageAlphaSel = HWIO_OUT_FLD(uBlendStageAlphaSel, MDP_VP_0_LAYER_0_OP_MODE, BLEND4_COLOR3_OUT, bSelectC3FromFG);
      }
      break;

   case HAL_MDP_BLEND_STAGE_5:
      {
         uBlendStageAlphaSel = HWIO_OUT_FLD(uBlendStageAlphaSel, MDP_VP_0_LAYER_0_OP_MODE, BLEND5_COLOR3_OUT, bSelectC3FromFG);
      }
      break;

   case HAL_MDP_BLEND_STAGE_6:
      {
         uBlendStageAlphaSel = HWIO_OUT_FLD(uBlendStageAlphaSel, MDP_VP_0_LAYER_0_OP_MODE, BLEND6_COLOR3_OUT, bSelectC3FromFG);
      }
      break;

   case HAL_MDP_BLEND_STAGE_CURSOR:
   case HAL_MDP_BLEND_STAGE_MAX:
   case HAL_MDP_BLEND_STAGE_FORCE_32BIT:
   default:
      break;
   }

   out_dword(HWIO_MDP_VP_0_LAYER_0_OP_MODE_ADDR + uMixerOffset, uBlendStageAlphaSel);
}

/****************************************************************************
*
** FUNCTION: HAL_MDP_LayerMixer_BlendStage_Config()
*/
/*!
* \brief
*     Initializes the layer mixer Gamma Correction Block (Area Reduced Gamma Correction)
*
* \param [in] eLayerMixerId         - Layer Mixer ID (0, 1, 2, 3, and 4)
* \param [in] eBlendStageId         - Blend stage for configuration
* \param [in] pBlendStageConfig     - Blend stage configuration information
* \param [in] uFlags                - Reserved flags
*
* \retval NONE
*
****************************************************************************/
static void HAL_MDP_LayerMixer_BlendStage_Config(HAL_MDP_LayerMixerId                eLayerMixerId,
                                                 HAL_MDP_BlendStageId                eBlendStageId,
                                                 HAL_MDP_Mixer_BlendStageConfigType *pBlendStageConfig,
                                                 uint32                              uFlags)
{
   uintPtr              uMixerOffset      = HAL_MDP_Get_LayerMixer_BaseOffset(eLayerMixerId);
   uint32               uBlendOpMode      = 0x00000000;
   uint32               uConstAlpha;

   // Color3 (Alpha) selection at blend-stage output configuration
   HAL_MDP_LayerMixer_BlendStage_Color3SelectConfig(eLayerMixerId, 
                                                    eBlendStageId, 
                                                    pBlendStageConfig->bSelectColor3FromFG, 
                                                    uFlags);

   uBlendOpMode   = in_dword(HWIO_MDP_VP_0_LAYER_0_BLENDn_OP_ADDR(eBlendStageId) + uMixerOffset);
   uConstAlpha    = in_dword(HWIO_MDP_VP_0_LAYER_0_BLENDn_CONSTANT_ALPHA_ADDR(eBlendStageId) + uMixerOffset);

   /* Transparency configurations - Deprecated */

   /* Foreground layer Transparency configuration - Deprecated */
   
   // Background Alpha configurations
   if(TRUE == pBlendStageConfig->sBackGroundAlphaBlend.bConfigure)
   {
      uBlendOpMode = HWIO_OUT_FLD(uBlendOpMode, 
                                    MDP_VP_0_LAYER_0_BLENDn_OP, 
                                    BG_ALPHA_SEL, 
                                    pBlendStageConfig->sBackGroundAlphaBlend.eAlphaSel);

      uBlendOpMode = HWIO_OUT_FLD(uBlendOpMode, 
                                    MDP_VP_0_LAYER_0_BLENDn_OP, 
                                    BG_MOD_ALPHA, 
                                    pBlendStageConfig->sBackGroundAlphaBlend.bModulateAlpha);

      uBlendOpMode = HWIO_OUT_FLD(uBlendOpMode, 
                                    MDP_VP_0_LAYER_0_BLENDn_OP, 
                                    BG_INV_ALPHA_SEL, 
                                    pBlendStageConfig->sBackGroundAlphaBlend.bInverseAlpha);

      uBlendOpMode = HWIO_OUT_FLD(uBlendOpMode, 
                                    MDP_VP_0_LAYER_0_BLENDn_OP, 
                                    BG_INV_MOD_ALPHA, 
                                    pBlendStageConfig->sBackGroundAlphaBlend.bInverseModulateAlpha);

      uConstAlpha = HWIO_OUT_FLD(uConstAlpha, 
                                 MDP_VP_0_LAYER_0_BLENDn_CONSTANT_ALPHA, 
                                 BG_CONSTANT_ALPHA, 
                                 pBlendStageConfig->sBackGroundAlphaBlend.uConstant);

   }

   // Foreground Alpha configuration
   if(TRUE == pBlendStageConfig->sForeGroundAlphaBlend.bConfigure)
   {
      uBlendOpMode = HWIO_OUT_FLD(uBlendOpMode, 
                                    MDP_VP_0_LAYER_0_BLENDn_OP, 
                                    FG_ALPHA_SEL, 
                                    pBlendStageConfig->sForeGroundAlphaBlend.eAlphaSel);

      uBlendOpMode = HWIO_OUT_FLD(uBlendOpMode, 
                                    MDP_VP_0_LAYER_0_BLENDn_OP, 
                                    FG_MOD_ALPHA, 
                                    pBlendStageConfig->sForeGroundAlphaBlend.bModulateAlpha);

      uBlendOpMode = HWIO_OUT_FLD(uBlendOpMode, 
                                    MDP_VP_0_LAYER_0_BLENDn_OP, 
                                    FG_INV_ALPHA_SEL, 
                                    pBlendStageConfig->sForeGroundAlphaBlend.bInverseAlpha);

      uBlendOpMode = HWIO_OUT_FLD(uBlendOpMode, 
                                    MDP_VP_0_LAYER_0_BLENDn_OP, 
                                    FG_INV_MOD_ALPHA, 
                                    pBlendStageConfig->sForeGroundAlphaBlend.bInverseModulateAlpha);

      
      uConstAlpha = HWIO_OUT_FLD(uConstAlpha, 
                                 MDP_VP_0_LAYER_0_BLENDn_CONSTANT_ALPHA, 
                                 FG_CONSTANT_ALPHA, 
                                 pBlendStageConfig->sForeGroundAlphaBlend.uConstant);
      
   }

   out_dword(HWIO_MDP_VP_0_LAYER_0_BLENDn_OP_ADDR(eBlendStageId) + uMixerOffset, uBlendOpMode);
   out_dword(HWIO_MDP_VP_0_LAYER_0_BLENDn_CONSTANT_ALPHA_ADDR(eBlendStageId) + uMixerOffset, uConstAlpha);
}

/****************************************************************************
*
** FUNCTION: HAL_MDP_LayerMixer_Cursor_Config()
*/
/*!
* \brief
*     Initializes the layer mixer Gamma Correction Block (Area Reduced Gamma Correction)
*
* \param [in] eLayerMixerId     - Layer Mixer ID (0, 1, 2, 3, and 4)
* \param [in] pCursorConfig     - Cursor configuration
* \param [in] uFlags            - Reserved flags
*
* \retval NONE
*
****************************************************************************/
static void HAL_MDP_LayerMixer_Cursor_Config(HAL_MDP_LayerMixerId            eLayerMixerId,
                                             HAL_MDP_Mixer_CursorConfigType *pCursorConfig,
                                             uint32                          uFlags)
{
  // Cursors are deprecated
}


/****************************************************************************
*
** FUNCTION: HAL_MDP_LayerMixer_AR_GammaCorrection_Config()
*/
/*!
* \brief
*     Initializes the layer mixer Gamma Correction Block (Area Reduced Gamma Correction)
*
* \param [in] eLayerMixerId - Layer Mixer ID (0, 1, 2, 3, and 4)
* \param [in] pGCConfig     - Gamma Correction configuration
* \param [in] uFlags        - Reserved flags
*
* \retval NONE
*
****************************************************************************/
static void HAL_MDP_LayerMixer_AR_GammaCorrection_Config(HAL_MDP_LayerMixerId                  eLayerMixerId,
                                                 HAL_MDP_Mixer_GammaCorrectConfigType *pGCConfig,
                                                 uint32                                uFlags)
{
  // AR GC LUT is deprecated
}

/****************************************************************************
*
** FUNCTION: HAL_MDP_LayerMixer_Linear_GammaCorrection_Config()
*/
/*!
* \brief
*     Initializes the layer mixer Gamma Correction Block 
*
* \param [in] eLayerMixerId - Layer Mixer ID (0, 1, 2, 3, and 4)
* \param [in] pGCConfig     - Gamma Correction configuration
* \param [in] uFlags        - Reserved flags
*
* \retval NONE
*
****************************************************************************/
static void HAL_MDP_LayerMixer_Linear_GammaCorrection_Config(HAL_MDP_LayerMixerId                        eLayerMixerId,
                                                             HAL_MDP_Mixer_Linear_GammaCorrectConfigType *pGCConfig,
                                                             uint32                                      uFlags)
{
  // todo: Linear LUT programming
}

/****************************************************************************
*
** FUNCTION: HAL_MDP_LayerMixer_BorderColor_Config()
*/
/*!
* \brief
*     Configures the layer mixer border color value
*
* \param [in] eLayerMixerId         - Layer Mixer ID (0, 1, 2, 3, and 4)
* \param [in] pBorderColorConfig    - Border Color configuration value
* \param [in] uFlags                - Reserved flags
*
* \retval NONE
*
****************************************************************************/
static void HAL_MDP_LayerMixer_BorderColor_Config(HAL_MDP_LayerMixerId                    eLayerMixerId,
                                                      HAL_MDP_Mixer_BorderColorConfigType *pBorderColorConfig,
                                                      uint32                              uFlags)
{
   uintPtr  uMixerOffset         = HAL_MDP_Get_LayerMixer_BaseOffset(eLayerMixerId);
   uint32   uBorderColor0Reg     = 0x00000000;        // Color 0 and Color 1 components   (C0 - Green C1 - Blue )
   uint32   uBorderColor1Reg     = 0x00000000;        // Color 2 and Color 3 components   (C2 - Red   C3 - Alpha)

   uint32   uGreenCompValue      = HAL_MDSS_CONVERT_8_TO_12BITS((pBorderColorConfig->uColorValue >> 8) & (0xFF));
   uint32   uBlueCompValue       = HAL_MDSS_CONVERT_8_TO_12BITS((pBorderColorConfig->uColorValue)      & (0xFF));
   //uint32 uBlueCompValue         = HAL_MDSS_CONVERT_8_TO_12BITS(0xFF);
   uint32   uRedCompValue        = HAL_MDSS_CONVERT_8_TO_12BITS((pBorderColorConfig->uColorValue >>16) & (0xFF));
   uint32   uAlphaCompValue      = ((pBorderColorConfig->uColorValue >> 23) & (0xFF));    // Alpha is only 8-bits

   uBorderColor0Reg = HWIO_OUT_FLD(uBorderColor0Reg,
                                   MDP_VP_0_LAYER_0_BORDER_COLOR_0,
                                   COLOR0,
                                   uGreenCompValue);     // C0 (Green)

   uBorderColor0Reg = HWIO_OUT_FLD(uBorderColor0Reg,
                                   MDP_VP_0_LAYER_0_BORDER_COLOR_0,
                                   COLOR1,
                                   uBlueCompValue);      // C1 (Blue)

   uBorderColor1Reg = HWIO_OUT_FLD(uBorderColor1Reg,
                                   MDP_VP_0_LAYER_0_BORDER_COLOR_1,
                                   COLOR2,
                                   uRedCompValue);       // C2 (Red)

   uBorderColor1Reg = HWIO_OUT_FLD(uBorderColor1Reg,
                                   MDP_VP_0_LAYER_0_BORDER_COLOR_1,
                                   COLOR3,
                                   uAlphaCompValue);     // C3 (Alpha)

   out_dword(HWIO_MDP_VP_0_LAYER_0_BORDER_COLOR_0_ADDR + uMixerOffset, uBorderColor0Reg);
   out_dword(HWIO_MDP_VP_0_LAYER_0_BORDER_COLOR_1_ADDR + uMixerOffset, uBorderColor1Reg);
}


/****************************************************************************
*
** FUNCTION: HAL_MDP_LayerMixer_Config()
*/
/*!
* \brief
*     Configures the layer mixer
*
* \param [in] eLayerMixerId         - Layer Mixer ID (0, 1, 2, 3, and 4)
* \param [in] pLayerMixerConfig     - Layer Mixer Configuration information
* \param [in] uFlags                - Reserved flags
*
* \retval HAL_MDSS_ErrorType
*
****************************************************************************/
HAL_MDSS_ErrorType HAL_MDP_LayerMixer_Config(HAL_MDP_LayerMixerId             eLayerMixerId,
                                             HAL_MDP_LayerMixerConfigType    *pLayerMixerConfig,
                                             uint32                           uFlags)
{
   HAL_MDSS_ErrorType   eStatus           = HAL_MDSS_STATUS_SUCCESS;
   uint32               uBlendStageIndex  = 0;

   // Output Size
   if(NULL != pLayerMixerConfig->psOutputSizeConfig)
   {
      HAL_MDP_LayerMixer_OutputSize_Config(eLayerMixerId,
                                           pLayerMixerConfig->psOutputSizeConfig,
                                           uFlags);
   }

   // Layer Blend Configuration
   for(uBlendStageIndex = 0; uBlendStageIndex < HAL_MDP_GET_LM_BLEND_STAGES(); uBlendStageIndex++)
   {
      //
      // Border color layer is always attached to support all kind of use-cases.
      // In general, driver never hits blend state#0 configuration
      if(NULL != pLayerMixerConfig->psBlendStageConfig[uBlendStageIndex])
      {
         HAL_MDP_LayerMixer_BlendStage_Config(eLayerMixerId,
                                              (HAL_MDP_BlendStageId)uBlendStageIndex,
                                              pLayerMixerConfig->psBlendStageConfig[uBlendStageIndex],
                                              0x00);
      }
   }

   // Cursor Blend Configuration
   if(NULL != pLayerMixerConfig->psCursorConfig)
   {
      HAL_MDP_LayerMixer_Cursor_Config(eLayerMixerId,
                                       pLayerMixerConfig->psCursorConfig,
                                       0x00);
   }

   // Area Reduced Gamma Correction Table Configuration
   if (NULL != pLayerMixerConfig->psGammaCorrectConfig)
   {
    // Call only if MDP Version supports Area reduced GC LUT
    if (HAL_MDP_GCLUT_AREA_REDUCED_TYPE == pgsMdpHwInfo->sHwInfo.eGCLUTType)
    {
      HAL_MDP_LayerMixer_AR_GammaCorrection_Config(eLayerMixerId, pLayerMixerConfig->psGammaCorrectConfig, 0x0);
    }
   }

   // Border Color Configuration
   if(NULL != pLayerMixerConfig->psBorderColorConfig)
   {
      HAL_MDP_LayerMixer_BorderColor_Config(eLayerMixerId,
                                            pLayerMixerConfig->psBorderColorConfig,
                                            0x00);
   }

   // Linear Gamma Correction Table Configuration
   if (NULL != pLayerMixerConfig->psLinearGammaCorrectionConfig)
   {
    // Call only if MDP Version supports Linear GC LUT
    if (HAL_MDP_GCLUT_LINEAR_TYPE == pgsMdpHwInfo->sHwInfo.eGCLUTType)
    {
      HAL_MDP_LayerMixer_Linear_GammaCorrection_Config(eLayerMixerId, pLayerMixerConfig->psLinearGammaCorrectionConfig, 0x0);
    }
   }

   return eStatus;
}

/****************************************************************************
*
** FUNCTION: HAL_MDP_LayerMixer_Init()
*/
/*!
* \brief
*     Initializes the layer mixer module
*
* \param [in] eLayerMixerId         - Layer Mixer ID (0, 1, 2, 3, and 4)
* \param [in] uInitFlags            - Internal blocks to be initialized
* \param [in] uFlags                - Reserved flags
*
* \retval HAL_MDSS_ErrorType
*
****************************************************************************/
HAL_MDSS_ErrorType HAL_MDP_LayerMixer_Init(HAL_MDP_LayerMixerId    eLayerMixerId, 
                                           uint32                  uInitFlags,
                                           uint32                  uFlags)
{
   HAL_MDSS_ErrorType      eStatus = HAL_MDSS_STATUS_SUCCESS;

   if(0x00 != (HAL_MDP_LM_DEFAULT_FEATURE_INIT & uInitFlags))
   {
      /* To initialize the layer mixer gamma tables with default gamma 
	     values call HAL_MDP_LayerMixer_GammaCorrection_Config() at this 
		 point with HAL_MDP_Mixer_GammaCorrectConfigType structure filled 
		 with the default table values. */


   }

   return eStatus;
}

#ifdef __cplusplus
}
#endif
