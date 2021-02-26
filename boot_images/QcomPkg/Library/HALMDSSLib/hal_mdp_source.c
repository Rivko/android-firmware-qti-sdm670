/*
===========================================================================

FILE:         hal_mdp_source.h

===========================================================================
  Copyright (c) 2010-2017 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
===========================================================================
*/

/* -----------------------------------------------------------------------
 * Includes
 * ----------------------------------------------------------------------- */

#include "hal_mdp_i.h"
#include "hal_mdp.h"
#include "hal_mdp_osal.h"

#ifdef __cplusplus
extern "C" {
#endif

/* -----------------------------------------------------------------------
 * Static Function Declarations and Defintions
 * ----------------------------------------------------------------------- */
#define  HAL_MDP_SOURCE_FLAGS_EXTRACT_ONLY_PIXEL_FORMAT              0x00000001
/**The HAL_MDP_SOURCE_FLAGS_PRE_SURFACE_CONFIG is used to indicate the Pre-condition call for 'HAL_MDP_SSPP_SurfaceConfig'*/
#define  HAL_MDP_SOURCE_FLAGS_PRE_SURFACE_CONFIG                     0x00000002


// Get the pixel source format - Mask solid fill, VC1 reduce, rotate90 and source color space 
#define  HAL_MDP_GET_SOURCE_PIXEL_FORMAT_MASK         (~((uint32)((HWIO_MDP_VP_0_VIG_0_SSPP_SRC_FORMAT_FRAME_FORMAT_BMSK)         | \
                                                                  (HWIO_MDP_VP_0_VIG_0_SSPP_SRC_FORMAT_SOLID_FILL_BMSK)            | \
                                                                  (HWIO_MDP_VP_0_VIG_0_SSPP_SRC_FORMAT_VC1_REDUCE_BMSK)            | \
                                                                  (HWIO_MDP_VP_0_VIG_0_SSPP_SRC_FORMAT_SRC_COLOR_SPACE_BMSK)       | \
                                                                  (HWIO_MDP_VP_0_VIG_0_SSPP_SRC_FORMAT_ROT90_BMSK))))

// Clear the pixel source format - Mask all the fields except solid fill and VC1 reduce 
#define  HAL_MDP_CLEAR_SOURCE_PIXEL_FORMAT_MASK       (~((uint32)((HWIO_MDP_VP_0_VIG_0_SSPP_SRC_FORMAT_FRAME_FORMAT_BMSK)          | \
                                                                  (HWIO_MDP_VP_0_VIG_0_SSPP_SRC_FORMAT_EXTEND_PIXEL_FORMAT_BMSK)   | \
                                                                  (HWIO_MDP_VP_0_VIG_0_SSPP_SRC_FORMAT_SRC_CHROMA_SAMP_BMSK)       | \
                                                                  (HWIO_MDP_VP_0_VIG_0_SSPP_SRC_FORMAT_FETCH_PLANES_BMSK)          | \
                                                                  (HWIO_MDP_VP_0_VIG_0_SSPP_SRC_FORMAT_UNPACK_ALIGN_BMSK)          | \
                                                                  (HWIO_MDP_VP_0_VIG_0_SSPP_SRC_FORMAT_UNPACK_TIGHT_BMSK)          | \
                                                                  (HWIO_MDP_VP_0_VIG_0_SSPP_SRC_FORMAT_UNPACK_DX_FORMAT_BMSK)      | \
                                                                  (HWIO_MDP_VP_0_VIG_0_SSPP_SRC_FORMAT_UNPACK_COUNT_BMSK)          | \
                                                                  (HWIO_MDP_VP_0_VIG_0_SSPP_SRC_FORMAT_ROT90_BMSK)                 | \
                                                                  (HWIO_MDP_VP_0_VIG_0_SSPP_SRC_FORMAT_SRC_BPP_BMSK)               | \
                                                                  (HWIO_MDP_VP_0_VIG_0_SSPP_SRC_FORMAT_SRCC3_EN_BMSK)              | \
                                                                  (HWIO_MDP_VP_0_VIG_0_SSPP_SRC_FORMAT_SRCC3_BMSK)                 | \
                                                                  (HWIO_MDP_VP_0_VIG_0_SSPP_SRC_FORMAT_SRCC2_BMSK)                 | \
                                                                  (HWIO_MDP_VP_0_VIG_0_SSPP_SRC_FORMAT_SRCC1_BMSK)                 | \
                                                                  (HWIO_MDP_VP_0_VIG_0_SSPP_SRC_FORMAT_SRCC0_BMSK))))

/* -----------------------------------------------------------------------
 * Structures
 * ----------------------------------------------------------------------- */

/*!
 * \enum HAL_MDP_SmartDMARecIDType
 *
 * The \b HAL_MDP_SmartDMARecIDType IDs associated with Smart DMA RECs
 */
typedef enum _HAL_MDP_SmartDMARecIDType
{
  HAL_MDP_SMART_DMA_REC0_ID          = 0x00000000,  /**< Smart DMA REC0 */
  HAL_MDP_SMART_DMA_REC1_ID,                        /**< Smart DMA REC1 */
  HAL_MDP_SMART_DMA_REC_MAX,
  HAL_MDP_SMART_DMA_REC_FORCE_32BIT  = 0x7FFFFFFF
} HAL_MDP_SmartDMARecIDType;

/* -----------------------------------------------------------------------
 * Static functions declarations 
 * ----------------------------------------------------------------------- */

/****************************************************************************
*
** FUNCTION: HAL_MDP_Get_SmartDMARecID()
*/
/*!
* \brief
*     Returns REC ID (0 or 1) based on the eSourcePipe
*
* \retval uint32
*
****************************************************************************/
static uint32 HAL_MDP_Get_SmartDMARecID(HAL_MDP_SourcePipeId eSourcePipe)
{
  uint32 uRecID = HAL_MDP_SMART_DMA_REC0_ID;

  if ((HAL_MDP_SOURCE_PIPE_SMART_DMA_BASE <= eSourcePipe) &&
      (HAL_MDP_SOURCE_PIPE_SMART_DMA_END >= eSourcePipe)) // SMART DMA REC1
  {
    uRecID = HAL_MDP_SMART_DMA_REC1_ID;
  }

  return uRecID;
}

/****************************************************************************
*
** FUNCTION: HAL_MDP_ValidateSourcePipe()
*/
/*!
* \brief
*     Validate the src pipe
*
* \param [in]   eSourcePipe         - Source pipe id
* \param [in]   eLayerType          - Layer Type (RGB, VIG, DMA or Cursor)
*
* \retval HAL_MDSS_ErrorType
*
****************************************************************************/
static HAL_MDSS_ErrorType HAL_MDP_ValidateSourcePipe(HAL_MDP_SourcePipeId  eSourcePipe,
                                                     HAL_MDP_LayerType     eLayerType)
{
   HAL_MDSS_ErrorType   eStatus           = HAL_MDSS_STATUS_SUCCESS;

   if((HAL_MDP_SOURCE_PIPE_NONE  == eSourcePipe) ||
      (HAL_MDP_SOURCE_PIPE_MAX   <= eSourcePipe) ||
      (HAL_MDP_LAYER_TYPE_NONE   == eLayerType)  ||
      (HAL_MDP_LAYER_TYPE_MAX    <= eLayerType))
   {
      eStatus = HAL_MDSS_STATUS_FAILED_INVALID_INPUT_PARAMETER;
   }
   else
   {
      uint32               uTotalSourcePipes = 0;
      uint32               uSourcePipeBaseId = 0;
     
      if(HAL_MDP_LAYER_TYPE_RGB == eLayerType)
      {
         uTotalSourcePipes = HAL_MDP_GET_RGB_LAYER_COUNT();
         uSourcePipeBaseId = HAL_MDP_SOURCE_PIPE_RGB_BASE;
      }
      else if(HAL_MDP_LAYER_TYPE_VIG == eLayerType)
      {
         uTotalSourcePipes = HAL_MDP_GET_VIG_LAYER_COUNT();
         uSourcePipeBaseId = HAL_MDP_SOURCE_PIPE_VIG_BASE;
      }
      else if(HAL_MDP_LAYER_TYPE_DMA == eLayerType)
      {
         uTotalSourcePipes = HAL_MDP_GET_DMA_LAYER_COUNT();

         // The offsets for REC0 and REC1 registers are different
         // Therefore the Smart DMA base is treated differently from physcial DMA
         if (HAL_MDP_SMART_DMA_REC1_ID == HAL_MDP_Get_SmartDMARecID(eSourcePipe))
         {
            uSourcePipeBaseId = HAL_MDP_SOURCE_PIPE_SMART_DMA_BASE;
         }
         else
         {
            uSourcePipeBaseId = HAL_MDP_SOURCE_PIPE_DMA_BASE;
         }
      }
      else if((HAL_MDP_LAYER_TYPE_CURSOR == eLayerType) || (HAL_MDP_LAYER_TYPE_SSPP_CURSOR == eLayerType))
      {
         uTotalSourcePipes = HAL_MDP_GET_CURSOR_LAYER_COUNT();
         uSourcePipeBaseId = HAL_MDP_SOURCE_PIPE_CURSOR_BASE;
      }

      if((uSourcePipeBaseId + uTotalSourcePipes) <= (uint32) eSourcePipe)
      {
         eStatus = HAL_MDSS_STATUS_FAILED_INVALID_INPUT_PARAMETER;
      }
   }
   
   return eStatus;
}

/****************************************************************************
*
** FUNCTION: HAL_MDP_Get_SSPP_BaseOffset()
*/
/*!
* \brief
*     Returns the offset of SSPP register block from first layer's SSPP address
*
* \param [in]   eSourcePipe         - Source pipe id
*
* \retval uintPtr
*
****************************************************************************/
static uintPtr HAL_MDP_Get_SSPP_BaseOffset(HAL_MDP_SourcePipeId  eSourcePipe)
{
   uintPtr     uRegOffset        = 0;

   uRegOffset = uMDPSSPPRegBaseOffset[eSourcePipe];
   
   return uRegOffset;
}

/****************************************************************************
*
** FUNCTION: HAL_MDP_Get_Layer_BaseOffset()
*/
/*!
* \brief
*     Returns the offset of SSPP register block from first layer's SSPP address
*
* \param [in]   eSourcePipe         - Source pipe id
*
* \retval uint32
*
****************************************************************************/
uintPtr HAL_MDP_Get_Layer_BaseOffset(HAL_MDP_SourcePipeId  eSourcePipe)
{
   uintPtr      uRegOffset        = 0;

   uRegOffset = uMDPLayerRegBaseOffset[eSourcePipe];

   return uRegOffset;
}

/****************************************************************************
*
** FUNCTION: HAL_MDP_SSPP_GetCropRectConfig()
*/
/*!
* \brief
*     Returns the cropping rectangle information for the given piple
*
* \param [in]   eSourcePipe         - Source pipe id
* \param [in]   eLayerType          - Layer Type (RGB, VIG, DMA, CURSOR etc)
* \param [out]  psRectInfo          - Rectangle configuration information
* \param [in]   uFlags              - Reserved.
*
* \retval NONE
*
****************************************************************************/
static void HAL_MDP_SSPP_GetCropRectConfig(HAL_MDP_SourcePipeId         eSourcePipe,
                                           HAL_MDP_LayerType            eLayerType,
                                           HAL_MDP_CropRectangleType   *psRectInfo,
                                           uint32                       uFlags)
{
   uintPtr  uRegisterOffset   = 0;
   uint32   uSrcRectXYPos     = 0;
   uint32   uSrcRectSize      = 0;
   uint32   uDstRectXYPos     = 0;
   uint32   uDstRectSize      = 0;

   // Extract the SSPP base address to program the HW registers
   uRegisterOffset   = HAL_MDP_Get_SSPP_BaseOffset(eSourcePipe);

   if (HAL_MDP_LAYER_TYPE_LAYERMIXER_CURSOR != eLayerType)  // Only for RGB/VIG/DMA layers
   {
     // The offsets for REC0 and REC1 registers are different
     // Therefore the Smart DMA base is treated differently from physcial DMA
     if (HAL_MDP_SMART_DMA_REC1_ID == HAL_MDP_Get_SmartDMARecID(eSourcePipe)) // SMART DMA REC1
     {
        uSrcRectXYPos = in_dword(HWIO_MDP_VP_0_VIG_0_SSPP_SRC_XY_REC1_ADDR + uRegisterOffset);
        uSrcRectSize  = in_dword(HWIO_MDP_VP_0_VIG_0_SSPP_SRC_SIZE_REC1_ADDR + uRegisterOffset);
        uDstRectXYPos = in_dword(HWIO_MDP_VP_0_VIG_0_SSPP_OUT_XY_REC1_ADDR + uRegisterOffset);
        uDstRectSize  = in_dword(HWIO_MDP_VP_0_VIG_0_SSPP_OUT_SIZE_REC1_ADDR + uRegisterOffset);

        // SRC ROI (x,y)
        psRectInfo->sSrcRectConfig.uPosX             = HWIO_GETVAL(MDP_VP_0_VIG_0_SSPP_SRC_XY_REC1, uSrcRectXYPos, REC1_SRC_X);
        psRectInfo->sSrcRectConfig.uPosY             = HWIO_GETVAL(MDP_VP_0_VIG_0_SSPP_SRC_XY_REC1, uSrcRectXYPos, REC1_SRC_Y);

        // SRC ROI (w, h)
        psRectInfo->sSrcRectConfig.uWidthInPixels    = HWIO_GETVAL(MDP_VP_0_VIG_0_SSPP_SRC_SIZE_REC1, uSrcRectSize, REC1_ROI_W);
        psRectInfo->sSrcRectConfig.uHeightInPixels   = HWIO_GETVAL(MDP_VP_0_VIG_0_SSPP_SRC_SIZE_REC1, uSrcRectSize, REC1_ROI_H);

        // DST ROI (x,y)
        psRectInfo->sDstRectConfig.uPosX             = HWIO_GETVAL(MDP_VP_0_VIG_0_SSPP_OUT_XY_REC1, uDstRectXYPos, REC1_DST_X);
        psRectInfo->sDstRectConfig.uPosY             = HWIO_GETVAL(MDP_VP_0_VIG_0_SSPP_OUT_XY_REC1, uDstRectXYPos, REC1_DST_Y);

        // DST ROI (w, h)
        psRectInfo->sDstRectConfig.uWidthInPixels    = HWIO_GETVAL(MDP_VP_0_VIG_0_SSPP_OUT_SIZE_REC1, uDstRectSize, REC1_DST_W);
        psRectInfo->sDstRectConfig.uHeightInPixels   = HWIO_GETVAL(MDP_VP_0_VIG_0_SSPP_OUT_SIZE_REC1, uDstRectSize, REC1_DST_H);
      }
      else
      {
        // Layer SSPP configuration (Common configuration for RGB, VIG, DMA layers)
        uSrcRectXYPos = in_dword(HWIO_MDP_VP_0_VIG_0_SSPP_SRC_XY_ADDR + uRegisterOffset);      // Source ROI (x,y)
        uSrcRectSize  = in_dword(HWIO_MDP_VP_0_VIG_0_SSPP_SRC_SIZE_ADDR + uRegisterOffset);    // Source ROI (w,h)
        uDstRectXYPos = in_dword(HWIO_MDP_VP_0_VIG_0_SSPP_OUT_XY_ADDR + uRegisterOffset);      // Destination ROI (x,y)
        uDstRectSize  = in_dword(HWIO_MDP_VP_0_VIG_0_SSPP_OUT_SIZE_ADDR + uRegisterOffset);    // Destination ROI (w,h)

        // SRC ROI (x,y)
        psRectInfo->sSrcRectConfig.uPosX             = HWIO_GETVAL(MDP_VP_0_VIG_0_SSPP_SRC_XY, uSrcRectXYPos, SRC_X);
        psRectInfo->sSrcRectConfig.uPosY             = HWIO_GETVAL(MDP_VP_0_VIG_0_SSPP_SRC_XY, uSrcRectXYPos, SRC_Y);

        // SRC ROI (w, h)
        psRectInfo->sSrcRectConfig.uWidthInPixels    = HWIO_GETVAL(MDP_VP_0_VIG_0_SSPP_SRC_SIZE, uSrcRectSize, ROI_W);
        psRectInfo->sSrcRectConfig.uHeightInPixels   = HWIO_GETVAL(MDP_VP_0_VIG_0_SSPP_SRC_SIZE, uSrcRectSize, ROI_H);

        // DST ROI (x,y)
        psRectInfo->sDstRectConfig.uPosX             = HWIO_GETVAL(MDP_VP_0_VIG_0_SSPP_OUT_XY, uDstRectXYPos, DST_X);
        psRectInfo->sDstRectConfig.uPosY             = HWIO_GETVAL(MDP_VP_0_VIG_0_SSPP_OUT_XY, uDstRectXYPos, DST_Y);

        // DST ROI (w, h)
        psRectInfo->sDstRectConfig.uWidthInPixels    = HWIO_GETVAL(MDP_VP_0_VIG_0_SSPP_OUT_SIZE, uDstRectSize, DST_W);
        psRectInfo->sDstRectConfig.uHeightInPixels   = HWIO_GETVAL(MDP_VP_0_VIG_0_SSPP_OUT_SIZE, uDstRectSize, DST_H);
      }
   }
}

/****************************************************************************
*
** FUNCTION: HAL_MDP_SSPP_SurfaceInfo()
*/
/*!
* \brief
*     HSYNC Flip mode configuration
*
* \param [in]   eSourcePipe         - Source pipe id
* \param [in]   eLayerType          - Layer Type (RGB, VIG, DMA, CURSOR etc)
* \param [out]  psSurfaceInfo       - Surface information
* \param [in]   uFlags              - Reserved
*
* \retval NONE
*
****************************************************************************/
static void HAL_MDP_SSPP_SurfaceInfo(HAL_MDP_SourcePipeId           eSourcePipe,
                                     HAL_MDP_LayerType              eLayerType,
                                     HAL_MDP_SurfaceAttrType        *psSurfaceInfo,
                                     HAL_MDP_CropRectangleType      *psRectInfo,
                                     uint32                         uFlags)
{
   uintPtr               uRegisterOffset     = 0;
   uint32                uSrcFormatRegValue  = 0;
   uint32                uRegValue           = 0;
   bool32                bUBWCEnabled        = FALSE;
   HAL_MDP_FetchPlanes   eFetchPlane         = 0;

   if (NULL != psSurfaceInfo)
   {
      if (HAL_MDP_LAYER_TYPE_LAYERMIXER_CURSOR != eLayerType)
      {
         // Extract the SSPP base address to program the HW registers
         uRegisterOffset   = HAL_MDP_Get_SSPP_BaseOffset(eSourcePipe);

         // Read programmed pixel format from register
         uSrcFormatRegValue = in_dword(HWIO_MDP_VP_0_VIG_0_SSPP_SRC_FORMAT_ADDR + uRegisterOffset);
         eFetchPlane        = (uSrcFormatRegValue & HWIO_MDP_VP_0_VIG_0_SSPP_SRC_FORMAT_FETCH_PLANES_BMSK) >> HWIO_MDP_VP_0_VIG_0_SSPP_SRC_FORMAT_FETCH_PLANES_SHFT;

         // Original defined pixel formats in uSrcFormatInfo don't have tile setting info. Ignore this
         // field value so that we can check corresponding programmed pixel format.
         uSrcFormatRegValue &= ~HWIO_MDP_VP_0_VIG_0_SSPP_SRC_FORMAT_FRAME_FORMAT_BMSK;

         // if chroma sample has been changed to H1V2, change it back to H2V1 just for comparison with uSrcFormatInfo
         if (HAL_MDP_SRC_FORMAT_CHROMA_SAMPLE_YUV422_H1V2 == HWIO_GETVAL(MDP_VP_0_VIG_0_SSPP_SRC_FORMAT, uSrcFormatRegValue, SRC_CHROMA_SAMP))
         {
            uSrcFormatRegValue = HWIO_OUT_FLD(uSrcFormatRegValue,
                                              MDP_VP_0_VIG_0_SSPP_SRC_FORMAT,
                                              SRC_CHROMA_SAMP,
                                              HAL_MDP_SRC_FORMAT_CHROMA_SAMPLE_YUV422_H2V1);    
         }

         if (0x00 == (HAL_MDP_SOURCE_FLAGS_EXTRACT_ONLY_PIXEL_FORMAT & uFlags))
         {
            // Update width and height information
            uRegValue = in_dword(HWIO_MDP_VP_0_VIG_0_SSPP_SRC_IMG_SIZE_ADDR + uRegisterOffset);    // Image size

            psSurfaceInfo->uWidthInPixel  = HWIO_GETVAL(MDP_VP_0_VIG_0_SSPP_SRC_IMG_SIZE, uRegValue, SRC_W);
            psSurfaceInfo->uHeightInPixel = HWIO_GETVAL(MDP_VP_0_VIG_0_SSPP_SRC_IMG_SIZE, uRegValue, SRC_H);

            // Determine if UBWC is enabled
            uRegValue = in_dword(HWIO_MDP_VP_0_VIG_0_SSPP_SRC_OP_MODE_ADDR + uRegisterOffset);

            if ((uRegValue & HWIO_MDP_VP_0_VIG_0_SSPP_SRC_OP_MODE_BWC_DEC_EN_BMSK) &&
                (HAL_MDP_COMPRESSION_UBWC == pgsMdpHwInfo->eBWC))
            {
               bUBWCEnabled = TRUE;
            }

            if (TRUE == bUBWCEnabled)
            {
               /*
                * For UBWC, the memory layout is metadata first, then the pixel data.
                * Graphics expects display to report the address which points to the metadata.
                * So, we read the SRC2_ADDR which contains the metadata address
                */
               uRegValue =  in_dword(HWIO_MDP_VP_0_VIG_0_SSPP_CURRENT_SRC2_ADDR_ADDR + uRegisterOffset);
               psSurfaceInfo->uPlanes.sRgb.sDeviceAddress.u.uLowPart    = uRegValue;
               psSurfaceInfo->uPlanes.sRgb.sDeviceAddress.u.iHighPart   = 0x00;
 
               uRegValue = in_dword(HWIO_MDP_VP_0_VIG_0_SSPP_SRC_YSTRIDE0_ADDR + uRegisterOffset); // Stride
               psSurfaceInfo->uPlanes.sRgb.uStrideInBytes = HWIO_GETVAL(MDP_VP_0_VIG_0_SSPP_SRC_YSTRIDE0, uRegValue, SRCP0_YSTRIDE);
            }
            else
            {
               if (HAL_MDP_FETCH_PLANES_INTERLEAVED == eFetchPlane)  // One plane surface
               {
                  uRegValue =  in_dword(HWIO_MDP_VP_0_VIG_0_SSPP_CURRENT_SRC0_ADDR_ADDR + uRegisterOffset);   // Address                   
          
                  psSurfaceInfo->uPlanes.sRgb.sDeviceAddress.u.uLowPart    = uRegValue;
                  psSurfaceInfo->uPlanes.sRgb.sDeviceAddress.u.iHighPart   = 0x00;
 
                  uRegValue = in_dword(HWIO_MDP_VP_0_VIG_0_SSPP_SRC_YSTRIDE0_ADDR + uRegisterOffset); // Stride
                  psSurfaceInfo->uPlanes.sRgb.uStrideInBytes = HWIO_GETVAL(MDP_VP_0_VIG_0_SSPP_SRC_YSTRIDE0, uRegValue, SRCP0_YSTRIDE);
               }
               else if (HAL_MDP_FETCH_PLANES_PSEUDO_PLANAR == eFetchPlane) // 2 plane address
               {
                  uRegValue =  in_dword(HWIO_MDP_VP_0_VIG_0_SSPP_CURRENT_SRC0_ADDR_ADDR + uRegisterOffset);   // Address 1
                  psSurfaceInfo->uPlanes.sYuvPsuedoPlanar.sLumaDeviceAddress.u.uLowPart    = uRegValue;
                  psSurfaceInfo->uPlanes.sYuvPsuedoPlanar.sLumaDeviceAddress.u.iHighPart   = 0x00;
 
                  uRegValue =  in_dword(HWIO_MDP_VP_0_VIG_0_SSPP_CURRENT_SRC1_ADDR_ADDR + uRegisterOffset);   // Address 2
                  psSurfaceInfo->uPlanes.sYuvPsuedoPlanar.sChromaDeviceAddress.u.uLowPart    = uRegValue;
                  psSurfaceInfo->uPlanes.sYuvPsuedoPlanar.sChromaDeviceAddress.u.iHighPart   = 0x00;
 
                  uRegValue = in_dword(HWIO_MDP_VP_0_VIG_0_SSPP_SRC_YSTRIDE0_ADDR + uRegisterOffset); // Stride 1
                  psSurfaceInfo->uPlanes.sYuvPsuedoPlanar.uLumaStrideInBytes = HWIO_GETVAL(MDP_VP_0_VIG_0_SSPP_SRC_YSTRIDE0, 
                                                                                           uRegValue, 
                                                                                           SRCP0_YSTRIDE);
 
                  psSurfaceInfo->uPlanes.sYuvPsuedoPlanar.uChromaStrideInBytes = HWIO_GETVAL(MDP_VP_0_VIG_0_SSPP_SRC_YSTRIDE0, 
                                                                                             uRegValue, 
                                                                                             SRCP1_YSTRIDE);   // Stride 2
               }
               else // 3 Plane surface
               {
                  uRegValue =  in_dword(HWIO_MDP_VP_0_VIG_0_SSPP_CURRENT_SRC0_ADDR_ADDR + uRegisterOffset);   // Address 1
                  psSurfaceInfo->uPlanes.sYuvPlanar.sLumaDeviceAddress.u.uLowPart    = uRegValue;
                  psSurfaceInfo->uPlanes.sYuvPlanar.sLumaDeviceAddress.u.iHighPart   = 0x00;
 
                  uRegValue =  in_dword(HWIO_MDP_VP_0_VIG_0_SSPP_CURRENT_SRC1_ADDR_ADDR + uRegisterOffset);   // Address 2
                  psSurfaceInfo->uPlanes.sYuvPlanar.sChromaBlueDeviceAddress.u.uLowPart    = uRegValue;
                  psSurfaceInfo->uPlanes.sYuvPlanar.sChromaBlueDeviceAddress.u.iHighPart   = 0x00;
 
                  uRegValue =  in_dword(HWIO_MDP_VP_0_VIG_0_SSPP_CURRENT_SRC2_ADDR_ADDR + uRegisterOffset);   // Address 3
                  psSurfaceInfo->uPlanes.sYuvPlanar.sChromaRedDeviceAddress.u.uLowPart    = uRegValue;
                  psSurfaceInfo->uPlanes.sYuvPlanar.sChromaRedDeviceAddress.u.iHighPart   = 0x00;
                
                  uRegValue = in_dword(HWIO_MDP_VP_0_VIG_0_SSPP_SRC_YSTRIDE0_ADDR + uRegisterOffset);
                
                  psSurfaceInfo->uPlanes.sYuvPlanar.uLumaStrideInBytes = HWIO_GETVAL(MDP_VP_0_VIG_0_SSPP_SRC_YSTRIDE0, 
                                                                                     uRegValue, 
                                                                                     SRCP0_YSTRIDE); // Stride 1
 
                  psSurfaceInfo->uPlanes.sYuvPlanar.uChromaBlueStrideInBytes = HWIO_GETVAL(MDP_VP_0_VIG_0_SSPP_SRC_YSTRIDE0, 
                                                                                           uRegValue, 
                                                                                           SRCP1_YSTRIDE);   // Stride 2
 
                  uRegValue = in_dword(HWIO_MDP_VP_0_VIG_0_SSPP_SRC_YSTRIDE1_ADDR + uRegisterOffset);
                  psSurfaceInfo->uPlanes.sYuvPlanar.uChromaRedStrideInBytes = HWIO_GETVAL(MDP_VP_0_VIG_0_SSPP_SRC_YSTRIDE1,
                                                                                          uRegValue, 
                                                                                          SRCP2_YSTRIDE);   // Stride 3
               }
            }
         }
      }
   }

   // Rectangle Information
   if (NULL != psRectInfo)
   {
      // Clear Rectangle Information
      psRectInfo->sSrcRectConfig.uPosX             = 0;
      psRectInfo->sSrcRectConfig.uPosY             = 0;
      psRectInfo->sSrcRectConfig.uWidthInPixels    = 0;
      psRectInfo->sSrcRectConfig.uHeightInPixels   = 0;

      psRectInfo->sDstRectConfig.uPosX             = 0;
      psRectInfo->sDstRectConfig.uPosY             = 0;
      psRectInfo->sDstRectConfig.uWidthInPixels    = 0;
      psRectInfo->sDstRectConfig.uHeightInPixels   = 0;

      HAL_MDP_SSPP_GetCropRectConfig(eSourcePipe, eLayerType, psRectInfo, uFlags);
   }
}

/****************************************************************************
*
** FUNCTION: HAL_MDP_SSPP_GetStateInfo()
*/
/*!
* \brief
*     Returns the currrent state of the given SSPP ID
*
* \param [in]   eSourcePipe         - Source pipe id
* \param [in]   eLayerType          - Layer Type (RGB, VIG, DMA, CURSOR etc)
* \param [out]  pbActive            - TRUE - If the SSPP is active
* \param [in]   uFlags              - Reserved
*
* \retval NONE
*
****************************************************************************/
static void HAL_MDP_SSPP_GetStateInfo(HAL_MDP_SourcePipeId     eSourcePipe,
                                      HAL_MDP_LayerType        eLayerType,
                                      bool32                  *pbActive,
                                      uint32                   uFlags)
{
   uint32                        uRegValue         = 0x00;
   HAL_MDP_ModuleStatusRegInfo  *psModuleRegInfo   = NULL;

   *(pbActive)  = FALSE;  // Initial state

   psModuleRegInfo = (HAL_MDP_ModuleStatusRegInfo  *)pgsMdpHwInfo->gpsModuleStatusRegInfo[HAL_MDP_MODULE_SOURCE_PIPE];

   if(NULL != psModuleRegInfo)
   {
      uRegValue   = in_dword(MDSS_REG_BASE + psModuleRegInfo[eSourcePipe].uRegAddress);

      if(0x00 != (uRegValue & psModuleRegInfo[eSourcePipe].uBitFieldMask) )
      {
         *(pbActive)  = TRUE;        // Module is in active state
      }
   }
}

/****************************************************************************
*
** FUNCTION: HAL_MDP_SSPP_SurfaceConfig()
*/
/*!
* \brief
*     Set up configurations for a particular source pipe
*
* \param [in]   eSourcePipe         - Source pipe id
* \param [in]   eLayerType          - Layer Type (RGB, VIG, DMA, CURSOR etc)
* \param [in]   psSurface           - Surface information
* \param [in]   uFlags              - Reserved.
*
* \retval NONE
*
****************************************************************************/
static void HAL_MDP_SSPP_SurfaceConfig(HAL_MDP_SourcePipeId       eSourcePipe,
                                       HAL_MDP_LayerType          eLayerType,
                                       HAL_MDP_SurfaceAttrType   *psSurface,
                                       uint32                     uFlags)
{
   uintPtr                    uRegisterOffset         = 0;              // Register offset
   uint32                     uImageSizeInPixels      = 0;              // Image size
   uint32                     uPlane0Address          = 0;              // Plane 0 address (Color#0 - G/Luma)
   uint32                     uPlane1Address          = 0;              // Plane 1 address (Color#1 - R/Cr)
   uint32                     uPlane2Address          = 0;              // Plane 2 address (Color#2 - B/Cb)
   uint32                     uPlane3Address          = 0;              // Plane 3 address (Color#3 - Alpha)
   uint32                     uPlane01StrideInBytes   = 0;              // Plane 0 and 1 stride
   uint32                     uPlane23StrideInBytes   = 0;              // Plane 2 and 3 stride
   uint32                     uSrcFormat              = 0;              // Source format information (#of planes, #colors, #bits/each color)
   uint32                     uSrcCurrentFormatInfo   = 0;
   uint32                     uSrcUnpack              = 0;              // Unpacking information of the source buffer 
   uint32                     uOpMode                 = 0;              // OP_MODE register
   uint32                     uSrcOpModeInfo          = 0;              // Source OP_MODE register
   uintPtr                    uLayerRegBaseOffset     = 0;
   HAL_MDSS_ErrorType         eStatus                 = HAL_MDSS_STATUS_SUCCESS;
   uint32                     uSourceRecID            = HAL_MDP_Get_SmartDMARecID(eSourcePipe);

   uLayerRegBaseOffset = HAL_MDP_Get_Layer_BaseOffset(eSourcePipe);
   uRegisterOffset     = HAL_MDP_Get_SSPP_BaseOffset(eSourcePipe);

   // Height (16bits) : Width (16bits) SSPP_SRC_IMG_SIZE
   uImageSizeInPixels = HWIO_OUT_FLD(uImageSizeInPixels,
                                     MDP_VP_0_VIG_0_SSPP_SRC_IMG_SIZE,
                                     SRC_H,
                                     psSurface->uHeightInPixel);

   uImageSizeInPixels = HWIO_OUT_FLD(uImageSizeInPixels,
                                     MDP_VP_0_VIG_0_SSPP_SRC_IMG_SIZE,
                                     SRC_W,
                                     psSurface->uWidthInPixel);
   
   uPlane01StrideInBytes = in_dword(HWIO_MDP_VP_0_VIG_0_SSPP_SRC_YSTRIDE0_ADDR + uRegisterOffset); // Plane-0 and 1 stride
   uPlane23StrideInBytes = in_dword(HWIO_MDP_VP_0_VIG_0_SSPP_SRC_YSTRIDE1_ADDR + uRegisterOffset); // Plane-2 and 3 stride

   if (0x00 != (HAL_MDP_FLAGS_SURFACE_COMPRESSED & psSurface->uFlags))
   {
     if(HAL_MDP_COMPRESSION_UBWC == pgsMdpHwInfo->eBWC)
     {
       uPlane0Address = psSurface->uPlanes.sCompressedPlane.sCompressedDataPlane0DeviceAddress.u.uLowPart;
       uPlane1Address = psSurface->uPlanes.sCompressedPlane.sCompressedDataPlane1DeviceAddress.u.uLowPart;
       uPlane2Address = psSurface->uPlanes.sCompressedPlane.sMetadataPlane0DeviceAddress.u.uLowPart;
       uPlane3Address = psSurface->uPlanes.sCompressedPlane.sMetadataPlane1DeviceAddress.u.uLowPart;

       // The offsets for REC0 and REC1 registers are different
       // Therefore the Smart DMA base is treated differently from physcial DMA
       if (HAL_MDP_SMART_DMA_REC1_ID == uSourceRecID) // SMART DMA REC1
       {
         uPlane01StrideInBytes   = HWIO_OUT_FLD(uPlane01StrideInBytes,
                                                MDP_VP_0_VIG_0_SSPP_SRC_YSTRIDE0,
                                                SRCP1_YSTRIDE,
                                                psSurface->uPlanes.sCompressedPlane.uCompressedDataPlane0StrideInBytes);

         uPlane23StrideInBytes   = HWIO_OUT_FLD(uPlane23StrideInBytes,
                                                MDP_VP_0_VIG_0_SSPP_SRC_YSTRIDE1,
                                                SRCP3_YSTRIDE,
                                                psSurface->uPlanes.sCompressedPlane.uMetadataPlane0StrideInBytes);
       }
       else
       {
          uPlane01StrideInBytes   = HWIO_OUT_FLD(uPlane01StrideInBytes,
                                              MDP_VP_0_VIG_0_SSPP_SRC_YSTRIDE0,
                                              SRCP0_YSTRIDE,
                                              psSurface->uPlanes.sCompressedPlane.uCompressedDataPlane0StrideInBytes);
          uPlane01StrideInBytes   = HWIO_OUT_FLD(uPlane01StrideInBytes,
                                              MDP_VP_0_VIG_0_SSPP_SRC_YSTRIDE0,
                                              SRCP1_YSTRIDE,
                                              psSurface->uPlanes.sCompressedPlane.uCompressedDataPlane1StrideInBytes);
          uPlane23StrideInBytes   = HWIO_OUT_FLD(uPlane23StrideInBytes,
                                              MDP_VP_0_VIG_0_SSPP_SRC_YSTRIDE1,
                                              SRCP2_YSTRIDE,
                                              psSurface->uPlanes.sCompressedPlane.uMetadataPlane0StrideInBytes);
          uPlane23StrideInBytes   = HWIO_OUT_FLD(uPlane23StrideInBytes,
                                              MDP_VP_0_VIG_0_SSPP_SRC_YSTRIDE1,
                                              SRCP3_YSTRIDE,
                                              psSurface->uPlanes.sCompressedPlane.uMetadataPlane1StrideInBytes);
       }
     }
     else
     {
       // SSPP_SRCx_ADDR
       uPlane0Address          = psSurface->uPlanes.sCompressedPlane.sCompressedDataPlane0DeviceAddress.u.uLowPart;
       uPlane1Address          = psSurface->uPlanes.sCompressedPlane.sMetadataPlane0DeviceAddress.u.uLowPart; 

       // SSPP_SRC_YSTRIDEx
       uPlane01StrideInBytes   = HWIO_OUT_FLD(uPlane01StrideInBytes,
                                              MDP_VP_0_VIG_0_SSPP_SRC_YSTRIDE0,
                                              SRCP0_YSTRIDE,
                                              psSurface->uPlanes.sCompressedPlane.uCompressedDataPlane0StrideInBytes);
       uPlane01StrideInBytes   = HWIO_OUT_FLD(uPlane01StrideInBytes,
                                              MDP_VP_0_VIG_0_SSPP_SRC_YSTRIDE0,
                                              SRCP1_YSTRIDE,
                                              psSurface->uPlanes.sCompressedPlane.uMetadataPlane0StrideInBytes);
     }
   }
   else if (TRUE == HAL_MDP_CHECK_INTERLEAVED_PIXEL_FORMAT(psSurface->ePixelFormat))
   {
      // SSPP_SRCx_ADDR
      uPlane0Address          = psSurface->uPlanes.sRgb.sDeviceAddress.u.uLowPart;

      // The offsets for REC0 and REC1 registers are different
      // Therefore the Smart DMA base is treated differently from physcial DMA
      if (HAL_MDP_SMART_DMA_REC1_ID == uSourceRecID) // SMART DMA REC1
      {
         uPlane01StrideInBytes   = HWIO_OUT_FLD(uPlane01StrideInBytes,
                                                MDP_VP_0_VIG_0_SSPP_SRC_YSTRIDE0,
                                                SRCP1_YSTRIDE,
                                                psSurface->uPlanes.sRgb.uStrideInBytes);
      }
      else
      {

         // SSPP_SRC_YSTRIDEx
         uPlane01StrideInBytes   = HWIO_OUT_FLD(uPlane01StrideInBytes,
                                             MDP_VP_0_VIG_0_SSPP_SRC_YSTRIDE0,
                                             SRCP0_YSTRIDE,
                                             psSurface->uPlanes.sRgb.uStrideInBytes);
      }
   }
   else if(TRUE == HAL_MDP_CHECK_PSEUDO_PLANAR_PIXEL_FORMAT(psSurface->ePixelFormat))
   {
      // SSPP_SRCx_ADDR
      uPlane0Address          = psSurface->uPlanes.sYuvPsuedoPlanar.sLumaDeviceAddress.u.uLowPart;
      uPlane1Address          = psSurface->uPlanes.sYuvPsuedoPlanar.sChromaDeviceAddress.u.uLowPart;

      // SSPP_SRC_YSTRIDEx
      uPlane01StrideInBytes   = HWIO_OUT_FLD(uPlane01StrideInBytes,
                                             MDP_VP_0_VIG_0_SSPP_SRC_YSTRIDE0,
                                             SRCP0_YSTRIDE,
                                             psSurface->uPlanes.sYuvPsuedoPlanar.uLumaStrideInBytes);
      uPlane01StrideInBytes   = HWIO_OUT_FLD(uPlane01StrideInBytes,
                                             MDP_VP_0_VIG_0_SSPP_SRC_YSTRIDE0,
                                             SRCP1_YSTRIDE,
                                             psSurface->uPlanes.sYuvPsuedoPlanar.uChromaStrideInBytes);
   }
   else if(TRUE == HAL_MDP_CHECK_PLANAR_PIXEL_FORMAT(psSurface->ePixelFormat))
   {
      // SSPP_SRCx_ADDR
      uPlane0Address          = psSurface->uPlanes.sYuvPlanar.sLumaDeviceAddress.u.uLowPart;
      uPlane1Address          = psSurface->uPlanes.sYuvPlanar.sChromaBlueDeviceAddress.u.uLowPart;
      uPlane2Address          = psSurface->uPlanes.sYuvPlanar.sChromaRedDeviceAddress.u.uLowPart;

      // SSPP_SRC_YSTRIDEx
      uPlane01StrideInBytes   = HWIO_OUT_FLD(uPlane01StrideInBytes,
                                             MDP_VP_0_VIG_0_SSPP_SRC_YSTRIDE0,
                                             SRCP0_YSTRIDE,
                                             psSurface->uPlanes.sYuvPlanar.uLumaStrideInBytes);
      uPlane01StrideInBytes   = HWIO_OUT_FLD(uPlane01StrideInBytes,
                                             MDP_VP_0_VIG_0_SSPP_SRC_YSTRIDE0,
                                             SRCP1_YSTRIDE,
                                             psSurface->uPlanes.sYuvPlanar.uChromaBlueStrideInBytes);
      uPlane23StrideInBytes   = HWIO_OUT_FLD(uPlane23StrideInBytes,
                                             MDP_VP_0_VIG_0_SSPP_SRC_YSTRIDE1,
                                             SRCP2_YSTRIDE,
                                             psSurface->uPlanes.sYuvPlanar.uChromaRedStrideInBytes);
   }
   else
   {
      eStatus = HAL_MDSS_STATUS_FAILED_NO_HW_SUPPORT;
   }
   
   if(HAL_MDSS_STATUS_SUCCESS == eStatus)
   {
      // OP MODE Register configuration
      if(HAL_MDP_LAYER_TYPE_VIG == eLayerType)     // Only for VIG layers
      {
         uOpMode  = in_dword(HWIO_MDP_VP_0_VIG_0_CSC_1_OP_MODE_ADDR + uLayerRegBaseOffset);

         if(TRUE == HAL_MDP_IS_MDP_PIXEL_YUV_FORMAT(psSurface->ePixelFormat)) // If YUV format
         {
            uOpMode   = HWIO_OUT_FLD(uOpMode, MDP_VP_0_VIG_0_CSC_1_OP_MODE, EN, TRUE);
            uOpMode   = HWIO_OUT_FLD(uOpMode, MDP_VP_0_VIG_0_CSC_1_OP_MODE, SRC_DATA_FORMAT, TRUE);     // SRC YUV Format and DST is always RGB
         }
         else  // RGB format
         {
            uOpMode   = HWIO_OUT_FLD(uOpMode, MDP_VP_0_VIG_0_CSC_1_OP_MODE, EN, FALSE);               // Bypass CSC
         }
         out_dword(HWIO_MDP_VP_0_VIG_0_CSC_1_OP_MODE_ADDR + uLayerRegBaseOffset, uOpMode);
      }

      // SSPP

      // Extract the base address to program the HW registers
      uRegisterOffset   = HAL_MDP_Get_SSPP_BaseOffset(eSourcePipe);
      
      // SSPP_SRC_FORMAT
      uSrcFormat               = HAL_MDP_GetSourceFormatInfo(psSurface);

      // The offsets for REC0 and REC1 registers are different
      // Therefore the Smart DMA base is treated differently from physcial DMA
      if (HAL_MDP_SMART_DMA_REC1_ID == uSourceRecID) // SMART DMA REC1
      {
         uSrcCurrentFormatInfo = in_dword(HWIO_MDP_VP_0_VIG_0_SSPP_SRC_FORMAT_REC1_ADDR + uRegisterOffset);
         uSrcOpModeInfo        = in_dword(HWIO_MDP_VP_0_VIG_0_SSPP_SRC_OP_MODE_REC1_ADDR + uRegisterOffset);
      }
      else
      {
         uSrcCurrentFormatInfo = in_dword(HWIO_MDP_VP_0_VIG_0_SSPP_SRC_FORMAT_ADDR + uRegisterOffset);
         uSrcOpModeInfo        = in_dword(HWIO_MDP_VP_0_VIG_0_SSPP_SRC_OP_MODE_ADDR + uRegisterOffset);
      }
      uSrcCurrentFormatInfo   &= HAL_MDP_CLEAR_SOURCE_PIXEL_FORMAT_MASK;
      uSrcFormat              |= uSrcCurrentFormatInfo;                  // Do not overwrite other register information

      if(HAL_MDP_LAYER_TYPE_DMA == eLayerType)        // Configuring rotate 90 path
      {
         if(0x00 != (HAL_MDP_FLAGS_SSPP_SURFACE_ROTATE_90 & psSurface->uFlags))
         {
            uSrcFormat = HWIO_OUT_FLD(uSrcFormat, MDP_VP_0_VIG_0_SSPP_SRC_FORMAT, ROT90, TRUE);               // DMA Layer is going through rotator with 90-degree rotation active
         }
      }
      uSrcFormat = HWIO_OUT_FLD(uSrcFormat, MDP_VP_0_VIG_0_SSPP_SRC_FORMAT, SRC_COLOR_SPACE, HAL_MDP_IS_MDP_PIXEL_YUV_FORMAT(psSurface->ePixelFormat) ? TRUE : FALSE);
      // Source operation mode and unpack pattern
      uSrcOpModeInfo  = in_dword(HWIO_MDP_VP_0_VIG_0_SSPP_SRC_OP_MODE_ADDR + uRegisterOffset);

      if (0x00 == (HAL_MDP_FLAGS_SURFACE_COMPRESSED & psSurface->uFlags))
      {
         // clear BWC_DEC_EN bit in case it has been set to 1 previously
         uSrcOpModeInfo  = HWIO_OUT_FLD(uSrcOpModeInfo, MDP_VP_0_VIG_0_SSPP_SRC_OP_MODE, BWC_DEC_EN, 0);
         
         uSrcUnpack      = HAL_PIXELFORMAT_TO_SOURCE_PACK_MAP(psSurface->ePixelFormat);
         // Because UBWC only takes one RGB order which is RGBA, so the color components (red and blue)
         // are swapped in GPU in tile before it goes to UBWC encoder. So, MDP needs to swap these two color
         // components again to get correct color. This is required for A5x macro-tile.
         if((HAL_MDP_FLAGS_SURFACE_ATTR_MACRO_TILE & psSurface->uFlags) &&
            (NULL != pgsMdpHwInfo->sIpFxnTbl.SwapMacroTileColorComponent))
         {
            /* For A5x macro-tile only case */
            uSrcUnpack = pgsMdpHwInfo->sIpFxnTbl.SwapMacroTileColorComponent(psSurface->ePixelFormat, uSrcUnpack);
         }
      } 
      else 
      {
         // set bandwidth decompression
         uSrcOpModeInfo  = HWIO_OUT_FLD(uSrcOpModeInfo, MDP_VP_0_VIG_0_SSPP_SRC_OP_MODE, BWC_DEC_EN, 1);
         // only support compression quality of LOSSLESS
         uSrcOpModeInfo  = HWIO_OUT_FLD(uSrcOpModeInfo, MDP_VP_0_VIG_0_SSPP_SRC_OP_MODE, BWC_DEC_QP, 0);
         if(HAL_MDP_COMPRESSION_UBWC == pgsMdpHwInfo->eBWC)
         {
           uSrcUnpack      = HAL_PIXELFORMAT_TO_SOURCE_PACK_MAP(psSurface->ePixelFormat);
           // Because UBWC only takes one RGB order which is RGBA, so the color components (red and blue)
           // are swapped in GPU in tile before it goes to UBWC encoder. So, MDP needs to swap these two color
           // components again to get correct color. This is required for A5x macro-tile.
           if(NULL != pgsMdpHwInfo->sIpFxnTbl.SwapMacroTileColorComponent)
           {
             uSrcUnpack = pgsMdpHwInfo->sIpFxnTbl.SwapMacroTileColorComponent(psSurface->ePixelFormat, uSrcUnpack);
           }
         }
         else
         {
           uSrcUnpack      = guSrcDecompressUnpackInfo[psSurface->ePixelFormat];
         }
      }

      if (HAL_MDP_LAYER_TYPE_DMA == eLayerType) // DMA layers
      {
        // The offsets for REC0 and REC1 registers are different
        // Therefore the Smart DMA base is treated differently from physcial DMA
        if (HAL_MDP_SMART_DMA_REC1_ID == uSourceRecID) // SMART DMA REC1
        {
          out_dword(HWIO_MDP_VP_0_VIG_0_SSPP_SRC1_ADDR_ADDR + uRegisterOffset, uPlane0Address);                // Plane-0 address
          out_dword(HWIO_MDP_VP_0_VIG_0_SSPP_SRC3_ADDR_ADDR + uRegisterOffset, uPlane2Address);                // Plane-1 address
          out_dword(HWIO_MDP_VP_0_VIG_0_SSPP_SRC_FORMAT_REC1_ADDR + uRegisterOffset, uSrcFormat);              // Src format information
          out_dword(HWIO_MDP_VP_0_VIG_0_SSPP_SRC_UNPACK_PATTERN_REC1_ADDR + uRegisterOffset, uSrcUnpack);      // Src unpack information
          out_dword(HWIO_MDP_VP_0_VIG_0_SSPP_SRC_OP_MODE_REC1_ADDR + uRegisterOffset, uSrcOpModeInfo);         // Src operation mode
        }
        else // REC0
        {
          out_dword(HWIO_MDP_VP_0_VIG_0_SSPP_SRC_IMG_SIZE_ADDR + uRegisterOffset, uImageSizeInPixels);    // Image size
          out_dword(HWIO_MDP_VP_0_VIG_0_SSPP_SRC0_ADDR_ADDR + uRegisterOffset, uPlane0Address);           // Plane-0 address
          out_dword(HWIO_MDP_VP_0_VIG_0_SSPP_SRC2_ADDR_ADDR + uRegisterOffset, uPlane2Address);           // Plane-2 address
          out_dword(HWIO_MDP_VP_0_VIG_0_SSPP_SRC_FORMAT_ADDR + uRegisterOffset, uSrcFormat);              // Src format information
          out_dword(HWIO_MDP_VP_0_VIG_0_SSPP_SRC_UNPACK_PATTERN_ADDR + uRegisterOffset, uSrcUnpack);      // Src unpack information
          out_dword(HWIO_MDP_VP_0_VIG_0_SSPP_SRC_OP_MODE_ADDR + uRegisterOffset, uSrcOpModeInfo);         // Src operation mode
        }
        out_dword(HWIO_MDP_VP_0_VIG_0_SSPP_SRC_YSTRIDE0_ADDR + uRegisterOffset, uPlane01StrideInBytes); // Plane-0 and 1 stride
        out_dword(HWIO_MDP_VP_0_VIG_0_SSPP_SRC_YSTRIDE1_ADDR + uRegisterOffset, uPlane23StrideInBytes); // Plane-2 and 3 stride
      } 
      else if (HAL_MDP_LAYER_TYPE_LAYERMIXER_CURSOR != eLayerType)     // Only for VIG/RGB layers
      {
         out_dword(HWIO_MDP_VP_0_VIG_0_SSPP_SRC_IMG_SIZE_ADDR + uRegisterOffset, uImageSizeInPixels);        // Image size
         out_dword(HWIO_MDP_VP_0_VIG_0_SSPP_SRC0_ADDR_ADDR + uRegisterOffset, uPlane0Address); // Plane-0 address
         out_dword(HWIO_MDP_VP_0_VIG_0_SSPP_SRC1_ADDR_ADDR + uRegisterOffset, uPlane1Address); // Plane-1 address
         out_dword(HWIO_MDP_VP_0_VIG_0_SSPP_SRC2_ADDR_ADDR + uRegisterOffset, uPlane2Address); // Plane-2 address
         out_dword(HWIO_MDP_VP_0_VIG_0_SSPP_SRC3_ADDR_ADDR + uRegisterOffset, uPlane3Address); // Plane-3 address
         out_dword(HWIO_MDP_VP_0_VIG_0_SSPP_SRC_YSTRIDE0_ADDR + uRegisterOffset, uPlane01StrideInBytes);     // Plane-0 and 1 stride
         out_dword(HWIO_MDP_VP_0_VIG_0_SSPP_SRC_YSTRIDE1_ADDR + uRegisterOffset, uPlane23StrideInBytes);     // Plane-2 and 3 stride
         out_dword(HWIO_MDP_VP_0_VIG_0_SSPP_SRC_FORMAT_ADDR + uRegisterOffset, uSrcFormat);                  // Src format information
         out_dword(HWIO_MDP_VP_0_VIG_0_SSPP_SRC_UNPACK_PATTERN_ADDR + uRegisterOffset, uSrcUnpack);          // Src unpack information
         out_dword(HWIO_MDP_VP_0_VIG_0_SSPP_SRC_OP_MODE_ADDR + uRegisterOffset, uSrcOpModeInfo);             // Src operation mode
      }

       
   }
}

/****************************************************************************
*
** FUNCTION: HAL_MDP_SSPP_GetMultiRectConfig()
*/
/*!
* \brief
*     Returns multi-rect configuration for given pipe
*
* \param [in]   eSourcePipe         - Source pipe id
* \param [out]  psMultiRectInfo     - Multirect config
* \param [in]   uFlags              - Reserved.
*
* \retval NONE
*
****************************************************************************/
static void HAL_MDP_SSPP_GetMultiRectConfig(HAL_MDP_SourcePipeId         eSourcePipe,
                                            HAL_MDP_MultiRectConfigType *psMultiRectInfo,
                                            uint32                       uFlags)
{
   HAL_MDP_SourcePipeId eVirtualSourcePipe = pgsMdpHwInfo->pMultiRectMappingTable[eSourcePipe];

   // Check if multirect is supported by pipe
   if (HAL_MDP_SOURCE_PIPE_NONE != eVirtualSourcePipe)
   {
      // Extract the SSPP base address to program the HW registers
      uintPtr  uRegisterOffset         = HAL_MDP_Get_SSPP_BaseOffset(eSourcePipe);
      uint32   uSourceRecID            = HAL_MDP_Get_SmartDMARecID(eSourcePipe);
      uint32   uMultiRectOpModeInfo    = 0;
      uint32   uMultiRecEnableMask     = 0;
      
      // Read current multi-rect config
      uMultiRectOpModeInfo = in_dword(HWIO_MDP_VP_0_VIG_0_SSPP_MULTI_REC_OP_MODE_ADDR + uRegisterOffset);
      uMultiRecEnableMask  = HWIO_GETVAL(MDP_VP_0_VIG_0_SSPP_MULTI_REC_OP_MODE,
                                         uMultiRectOpModeInfo,
                                         MULTI_REC_ENABLE);

      // If uMultiRecEnableMask = b01, means REC0 is enabled
      // If uMultiRecEnableMask = b10, means REC1 is enabled
      // If uMultiRecEnableMask = b11, means both REC0 and REC1 are enabled
      if (uMultiRecEnableMask & (1 << uSourceRecID))
      {
         // Multirect is enabled
         psMultiRectInfo->bEnable        = TRUE;
 
         // Check for multirect mode type
         psMultiRectInfo->eMultiRectType = HWIO_GETVAL(MDP_VP_0_VIG_0_SSPP_MULTI_REC_OP_MODE,
                                           uMultiRectOpModeInfo,
                                           MULTI_REC_MODE);
      }
   }   
}


/****************************************************************************
*
** FUNCTION: HAL_MDP_DecimationConfig()
*/
/*!
* \brief
*     Set up the decimator configuration
*
* \param [in]   eSourcePipe               - Source pipe id
* \param [in]   sMDPScaleInfo             - Surface information, it will recalculated for the Scaler
* \param [in]   sMDPDecimationInfo        - Decimation Factors.
*
* \retval NONE
*
****************************************************************************/
static void HAL_MDP_DecimationConfig(HAL_MDP_SourcePipeId          eSourcePipe,
                                             HAL_MDP_ScalarInfo*           sMDPScaleInfo,
                                             HAL_MDP_DecimationConfigType* sMDPDecimationInfo)
{
   // Extract the SSPP base address to program the HW registers
   uintPtr uRegisterOffset   = HAL_MDP_Get_SSPP_BaseOffset(eSourcePipe);
   uint32 uDecimationValues = 0;

   /* Set the decimation factors if those are higher than the unit, otherwise we need to disable decimation */

   if (1 < sMDPDecimationInfo->uVertDecimationFactor) // Configure Vertical decimation
   {
      /* Recalculate the Height after decimation, this is the height needed to configure the Scaler */
      sMDPScaleInfo->uSrcHeightInPixels = (int) sMDPScaleInfo->uSrcHeightInPixels / sMDPDecimationInfo->uVertDecimationFactor;

      uDecimationValues = HWIO_OUT_FLD(uDecimationValues,
                                       MDP_VP_0_VIG_0_SSPP_DECIMATION_CONFIG,
                                       VERT_S,
                                       (sMDPDecimationInfo->uVertDecimationFactor - 1));
      
   }

   if (1 < sMDPDecimationInfo->uHorzDecimationFactor) // Configure Horizontal decimation
   {
      /* Recalculate the Width after decimation, this is the width needed to configure the Scaler */
      sMDPScaleInfo->uSrcWidthInPixels = (int) sMDPScaleInfo->uSrcWidthInPixels / sMDPDecimationInfo->uHorzDecimationFactor;

      uDecimationValues = HWIO_OUT_FLD(uDecimationValues,
                                       MDP_VP_0_VIG_0_SSPP_DECIMATION_CONFIG,
                                       HORZ_S,
                                       (sMDPDecimationInfo->uHorzDecimationFactor - 1));
   }

   /* Set Decimation Values: It will enable decimation if factor was higher than the unit, otherwise it will disable decimation */
   out_dword(HWIO_MDP_VP_0_VIG_0_SSPP_DECIMATION_CONFIG_ADDR + uRegisterOffset, uDecimationValues);

   return;

}

/****************************************************************************
*
** FUNCTION: HAL_MDP_PixelExtensionConfig()
*/
/*!
* \brief
*     Program decimation, scaler, initial phase, phase step and PE registers
*
* \param [in]   eSourcePipe             - Source pipe id
* \param [in]   eLayerType              - Layer Type, Only RGB, VG and DMA are supported
* \param [in]   psPixelExtLayerConfig   - Pixel extension configuration
*
* \retval void
*
****************************************************************************/
static void HAL_MDP_PixelExtensionConfig(HAL_MDP_SourcePipeId             eSourcePipe,
                                         HAL_MDP_LayerType                eLayerType,
                                         HAL_MDP_PixelExtLayerConfigType *psPixelExtLayerConfig)
{
   uintPtr                             uLayerRegOffset                                   = 0;
   uintPtr                             uRegisterOffset                                   = 0;
   uint32                              uSrcOpModeRegValue                                = 0;
   uint32                              uDecimationRegValue                               = 0;
   uint32                              uVertDecimationFactor                             = psPixelExtLayerConfig->sDecimationConfig.uVertDecimationFactor;
   uint32                              uHorzDecimationFactor                             = psPixelExtLayerConfig->sDecimationConfig.uHorzDecimationFactor;
   uint32                              uPixExtLR[HAL_MDP_SOURCE_PIPE_MAX_COMPONENTS]     = {0};
   uint32                              uPixExtTB[HAL_MDP_SOURCE_PIPE_MAX_COMPONENTS]     = {0};
   uint32                              uPixExtRegPix[HAL_MDP_SOURCE_PIPE_MAX_COMPONENTS] = {0};
   uint32                              uComponent                                        = 0;
   HAL_MDP_PixelExtOverrideConfigType *psPixelExtOverrideConfig                          = NULL;
   bool32                              bDecimationRequired                               = FALSE;

   // Extract the layer SSPP base address
   uRegisterOffset   = HAL_MDP_Get_SSPP_BaseOffset(eSourcePipe);
   // Only for RGB and VIG layer
   if((HAL_MDP_LAYER_TYPE_RGB == eLayerType) || (HAL_MDP_LAYER_TYPE_VIG == eLayerType))
   {
     // Extract the layer base address
     uLayerRegOffset   = HAL_MDP_Get_Layer_BaseOffset(eSourcePipe);

     /* Program decimation register when decimation is enabled */
     uDecimationRegValue = in_dword(HWIO_MDP_VP_0_VIG_0_SSPP_DECIMATION_CONFIG_ADDR + uLayerRegOffset);
   
     if (1 < uVertDecimationFactor) 
     {
        bDecimationRequired = TRUE;
        /* Vertical decimation */
        uDecimationRegValue = HWIO_OUT_FLD(uDecimationRegValue,
                                           MDP_VP_0_VIG_0_SSPP_DECIMATION_CONFIG,
                                           VERT_S,
                                           (uVertDecimationFactor - 1));
     }

     if (1 < uHorzDecimationFactor)
     {
        bDecimationRequired = TRUE;
        /* Horizontal decimation */
        uDecimationRegValue = HWIO_OUT_FLD(uDecimationRegValue,
                                           MDP_VP_0_VIG_0_SSPP_DECIMATION_CONFIG,
                                           HORZ_S,
                                           (uHorzDecimationFactor - 1));
     }

     if (TRUE == bDecimationRequired)
     {
       /* Program decimation config register */
       out_dword(HWIO_MDP_VP_0_VIG_0_SSPP_DECIMATION_CONFIG_ADDR + uLayerRegOffset, uDecimationRegValue);    
     }
     HAL_MDP_SSPP_ScalerConfig(eSourcePipe,eLayerType,&psPixelExtLayerConfig->sScalarConfig);
   }

   /* SW pixel extension override */

   /* Program pixel extension override registers for four components*/
   for (uComponent = 0; uComponent < HAL_MDP_SOURCE_PIPE_MAX_COMPONENTS; uComponent++)
   {
      psPixelExtOverrideConfig = &psPixelExtLayerConfig->aPixelExtComponentConfig[uComponent].sPixelExtOverrideConfig;

      /* Left/Right Pixel Extension */
      uPixExtLR[uComponent] = HWIO_OUT_FLD(uPixExtLR[uComponent],
                                       MDP_VP_0_VIG_0_SSPP_SW_PIX_EXT_C0_LR,
                                       LEFT_OVF,
                                       psPixelExtOverrideConfig->iLeftOverFetch);
      uPixExtLR[uComponent] = HWIO_OUT_FLD(uPixExtLR[uComponent],
                                       MDP_VP_0_VIG_0_SSPP_SW_PIX_EXT_C0_LR,
                                       LEFT_RPT,
                                       psPixelExtOverrideConfig->uLeftRepeat);
      uPixExtLR[uComponent] = HWIO_OUT_FLD(uPixExtLR[uComponent],
                                       MDP_VP_0_VIG_0_SSPP_SW_PIX_EXT_C0_LR,
                                       RIGHT_OVF,
                                       psPixelExtOverrideConfig->iRightOverFetch);
      uPixExtLR[uComponent] = HWIO_OUT_FLD(uPixExtLR[uComponent],
                                       MDP_VP_0_VIG_0_SSPP_SW_PIX_EXT_C0_LR,
                                       RIGHT_RPT,
                                       psPixelExtOverrideConfig->uRightRepeat);

      /* Top/Bottom Pixel Extension */
      uPixExtTB[uComponent] = HWIO_OUT_FLD(uPixExtTB[uComponent],
                                       MDP_VP_0_VIG_0_SSPP_SW_PIX_EXT_C0_TB,
                                       TOP_OVF,
                                       psPixelExtOverrideConfig->iTopOverFetch);
      uPixExtTB[uComponent] = HWIO_OUT_FLD(uPixExtTB[uComponent],
                                       MDP_VP_0_VIG_0_SSPP_SW_PIX_EXT_C0_TB,
                                       TOP_RPT,
                                       psPixelExtOverrideConfig->uTopRepeat);
      uPixExtTB[uComponent] = HWIO_OUT_FLD(uPixExtTB[uComponent],
                                       MDP_VP_0_VIG_0_SSPP_SW_PIX_EXT_C0_TB,
                                       BOTTOM_OVF,
                                       psPixelExtOverrideConfig->iBottomOverFetch);
      uPixExtTB[uComponent] = HWIO_OUT_FLD(uPixExtTB[uComponent],
                                       MDP_VP_0_VIG_0_SSPP_SW_PIX_EXT_C0_TB,
                                       BOTTOM_RPT,
                                       psPixelExtOverrideConfig->uBottomRepeat);

      /* Required Pixels */
      uPixExtRegPix[uComponent] = HWIO_OUT_FLD(uPixExtRegPix[uComponent],
                                           MDP_VP_0_VIG_0_SSPP_SW_PIX_EXT_C0_REQ_PIXELS,
                                           TOP_BOTTOM,
                                           psPixelExtOverrideConfig->uTopBottomReqPxls);
      uPixExtRegPix[uComponent] = HWIO_OUT_FLD(uPixExtRegPix[uComponent],
                                           MDP_VP_0_VIG_0_SSPP_SW_PIX_EXT_C0_REQ_PIXELS,
                                           LEFT_RIGHT,
                                           psPixelExtOverrideConfig->uLeftRightReqPxls);
   }

   out_dword(HWIO_MDP_VP_0_VIG_0_SSPP_SW_PIX_EXT_C0_LR_ADDR + uRegisterOffset, uPixExtLR[0]); 
   out_dword(HWIO_MDP_VP_0_VIG_0_SSPP_SW_PIX_EXT_C0_TB_ADDR + uRegisterOffset, uPixExtTB[0]); 
   out_dword(HWIO_MDP_VP_0_VIG_0_SSPP_SW_PIX_EXT_C0_REQ_PIXELS_ADDR + uRegisterOffset, uPixExtRegPix[0]); 

   /* Component1 and Component2 share the same values, either Component1 or Component2 can be used here */
   out_dword(HWIO_MDP_VP_0_VIG_0_SSPP_SW_PIX_EXT_C1C2_LR_ADDR + uRegisterOffset, uPixExtLR[1]); 
   out_dword(HWIO_MDP_VP_0_VIG_0_SSPP_SW_PIX_EXT_C1C2_TB_ADDR + uRegisterOffset, uPixExtTB[1]); 
   out_dword(HWIO_MDP_VP_0_VIG_0_SSPP_SW_PIX_EXT_C1C2_REQ_PIXELS_ADDR + uRegisterOffset, uPixExtRegPix[1]);

   out_dword(HWIO_MDP_VP_0_VIG_0_SSPP_SW_PIX_EXT_C3_LR_ADDR + uRegisterOffset, uPixExtLR[3]); 
   out_dword(HWIO_MDP_VP_0_VIG_0_SSPP_SW_PIX_EXT_C3_TB_ADDR + uRegisterOffset, uPixExtTB[3]); 
   out_dword(HWIO_MDP_VP_0_VIG_0_SSPP_SW_PIX_EXT_C3_REQ_PIXELS_ADDR + uRegisterOffset, uPixExtRegPix[3]);

   /* Enable SW pixel extension override */
   uSrcOpModeRegValue = in_dword(HWIO_MDP_VP_0_VIG_0_SSPP_SRC_OP_MODE_ADDR + uRegisterOffset);
   uSrcOpModeRegValue = HWIO_OUT_FLD(uSrcOpModeRegValue,
                                  MDP_VP_0_VIG_0_SSPP_SRC_OP_MODE,
                                  SW_PIX_EXT_OVERRIDE,
                                  TRUE);
   out_dword(HWIO_MDP_VP_0_VIG_0_SSPP_SRC_OP_MODE_ADDR + uRegisterOffset, uSrcOpModeRegValue); 

}

/****************************************************************************
*
** FUNCTION: HAL_MDP_SSPP_CropRectConfig()
*/
/*!
* \brief
*     Set up configurations for a particular source pipe
*
* \param [in]   eSourcePipe         - Source pipe id
* \param [in]   eLayerType          - Layer Type (RGB, VIG, DMA, CURSOR etc)
* \param [in]   psRectInfo          - Rectangle configuration information
* \param [in]   uFlags              - Reserved.
*
* \retval NONE
*
****************************************************************************/
static void HAL_MDP_SSPP_CropRectConfig(HAL_MDP_SourcePipeId         eSourcePipe,
                                        HAL_MDP_LayerType            eLayerType,
                                        HAL_MDP_CropRectangleType   *psRectInfo,
                                        uint32                       uFlags)
{
   uintPtr  uRegisterOffset   = 0;
   uint32   uSrcRectXYPos     = 0;
   uint32   uSrcRectSize      = 0;
   uint32   uDstRectXYPos     = 0;
   uint32   uDstRectSize      = 0;

   // Extract the SSPP base address to program the HW registers
   uRegisterOffset   = HAL_MDP_Get_SSPP_BaseOffset(eSourcePipe);

   // SRC ROI (x,y)
   uSrcRectXYPos   = HWIO_OUT_FLD(uSrcRectXYPos,
                                  MDP_VP_0_VIG_0_SSPP_SRC_XY,
                                  SRC_X,
                                  psRectInfo->sSrcRectConfig.uPosX);

   uSrcRectXYPos   = HWIO_OUT_FLD(uSrcRectXYPos,
                                  MDP_VP_0_VIG_0_SSPP_SRC_XY,
                                  SRC_Y,
                                  psRectInfo->sSrcRectConfig.uPosY);

   // SRC ROI (w, h)
   uSrcRectSize   = HWIO_OUT_FLD(uSrcRectSize,
                                 MDP_VP_0_VIG_0_SSPP_SRC_SIZE,
                                 ROI_W,
                                 psRectInfo->sSrcRectConfig.uWidthInPixels);

   uSrcRectSize   = HWIO_OUT_FLD(uSrcRectSize,
                                 MDP_VP_0_VIG_0_SSPP_SRC_SIZE,
                                 ROI_H,
                                 psRectInfo->sSrcRectConfig.uHeightInPixels);

   // DST ROI (x,y)
   uDstRectXYPos   = HWIO_OUT_FLD(uDstRectXYPos,
                                  MDP_VP_0_VIG_0_SSPP_OUT_XY,
                                  DST_X,
                                  psRectInfo->sDstRectConfig.uPosX);

   uDstRectXYPos   = HWIO_OUT_FLD(uDstRectXYPos,
                                  MDP_VP_0_VIG_0_SSPP_OUT_XY,
                                  DST_Y,
                                  psRectInfo->sDstRectConfig.uPosY);

   // DST ROI (w, h)
   uDstRectSize   = HWIO_OUT_FLD(uDstRectSize,
                                 MDP_VP_0_VIG_0_SSPP_OUT_SIZE,
                                 DST_W,
                                 psRectInfo->sDstRectConfig.uWidthInPixels);

   uDstRectSize   = HWIO_OUT_FLD(uDstRectSize,
                                 MDP_VP_0_VIG_0_SSPP_OUT_SIZE,
                                 DST_H,
                                 psRectInfo->sDstRectConfig.uHeightInPixels);

   // Configure scale step information
   // The offsets for REC0 and REC1 registers are different
   // Therefore the Smart DMA base is treated differently from physcial DMA
   if (HAL_MDP_SMART_DMA_REC1_ID == HAL_MDP_Get_SmartDMARecID(eSourcePipe)) // SMART DMA REC1
   {
      out_dword(HWIO_MDP_VP_0_VIG_0_SSPP_SRC_XY_REC1_ADDR + uRegisterOffset, uSrcRectXYPos);    // Source ROI (x,y)
      out_dword(HWIO_MDP_VP_0_VIG_0_SSPP_SRC_SIZE_REC1_ADDR + uRegisterOffset, uSrcRectSize);   // Source ROI (w,h)
      out_dword(HWIO_MDP_VP_0_VIG_0_SSPP_OUT_XY_REC1_ADDR + uRegisterOffset, uDstRectXYPos);    // Destination ROI (x,y)
      out_dword(HWIO_MDP_VP_0_VIG_0_SSPP_OUT_SIZE_REC1_ADDR + uRegisterOffset, uDstRectSize);   // Destination ROI (w,h)
   }
   else if (HAL_MDP_LAYER_TYPE_LAYERMIXER_CURSOR != eLayerType)  // Only for RGB/VIG/DMA layers
   {
      // Layer SSPP configuration (Common configuration for RGB, VIG, DMA layers)
      out_dword(HWIO_MDP_VP_0_VIG_0_SSPP_SRC_XY_ADDR + uRegisterOffset, uSrcRectXYPos);    // Source ROI (x,y)
      out_dword(HWIO_MDP_VP_0_VIG_0_SSPP_SRC_SIZE_ADDR + uRegisterOffset, uSrcRectSize);   // Source ROI (w,h)
      out_dword(HWIO_MDP_VP_0_VIG_0_SSPP_OUT_XY_ADDR + uRegisterOffset, uDstRectXYPos);    // Destination ROI (x,y)
      out_dword(HWIO_MDP_VP_0_VIG_0_SSPP_OUT_SIZE_ADDR + uRegisterOffset, uDstRectSize);   // Destination ROI (w,h)
   }
}

/****************************************************************************
*
** FUNCTION: HAL_MDP_SSPP_ColorfillConfig()
*/
/*!
* \brief
*     Initializes the constant color fill information for the given pipe
*
* \param [in]   eSourcePipe         - Source pipe id
* \param [in]   eLayerType          - Layer Type (RGB, VIG, DMA, CURSOR etc)
* \param [in]   psColorFillInfo     - Color fill information
* \param [in]   uFlags              - Reserved
*
* \retval NONE
*
****************************************************************************/
static void HAL_MDP_SSPP_ColorfillConfig(HAL_MDP_SourcePipeId        eSourcePipe,
                                         HAL_MDP_LayerType           eLayerType,
                                         HAL_MDP_ColorFillConfigType *psColorFillInfo,
                                         uint32                      uFlags)
{
   uintPtr  uRegisterOffset         = 0;
   uint32   uSrcCurrentFormatInfo   = 0;

   // Extract the base address to program the HW registers
   uRegisterOffset   = HAL_MDP_Get_SSPP_BaseOffset(eSourcePipe);

   // The offsets for REC0 and REC1 registers are different
   // Therefore the Smart DMA base is treated differently from physcial DMA
   if (HAL_MDP_SMART_DMA_REC1_ID == HAL_MDP_Get_SmartDMARecID(eSourcePipe)) // SMART DMA REC1
   {
      uSrcCurrentFormatInfo  = in_dword(HWIO_MDP_VP_0_VIG_0_SSPP_SRC_FORMAT_REC1_ADDR + uRegisterOffset);
      uSrcCurrentFormatInfo  = HWIO_OUT_FLD(uSrcCurrentFormatInfo,
                                            MDP_VP_0_VIG_0_SSPP_SRC_FORMAT,
                                            SOLID_FILL,
                                            (uint32)psColorFillInfo->bEnable);

      // Expected source, rectangle information configuration
      out_dword(HWIO_MDP_VP_0_VIG_0_SSPP_SRC_FORMAT_REC1_ADDR + uRegisterOffset, uSrcCurrentFormatInfo);                     // Src format information
      out_dword(HWIO_MDP_VP_0_VIG_0_SSPP_SRC_CONSTANT_COLOR_REC1_ADDR + uRegisterOffset, psColorFillInfo->uConstantColor);   // Color
   }
   else
   {
      // Color Fill 
      uSrcCurrentFormatInfo  = in_dword(HWIO_MDP_VP_0_VIG_0_SSPP_SRC_FORMAT_ADDR + uRegisterOffset);
      uSrcCurrentFormatInfo  = HWIO_OUT_FLD(uSrcCurrentFormatInfo,
                                         MDP_VP_0_VIG_0_SSPP_SRC_FORMAT,
                                         SOLID_FILL,
                                         (uint32)psColorFillInfo->bEnable);

      // Expected source, rectangle information configuration is already format
      out_dword(HWIO_MDP_VP_0_VIG_0_SSPP_SRC_FORMAT_ADDR + uRegisterOffset, uSrcCurrentFormatInfo);                     // Src format information
      out_dword(HWIO_MDP_VP_0_VIG_0_SSPP_SRC_CONSTANT_COLOR_ADDR + uRegisterOffset, psColorFillInfo->uConstantColor);   // Color
   }
}

/****************************************************************************
*
** FUNCTION: HAL_MDP_SSPP_ImmediateFlipConfig()
*/
/*!
* \brief
*     HSYNC Flip mode configuration
*
* \param [in]   eSourcePipe         - Source pipe id
* \param [in]   eLayerType          - Layer Type (RGB, VIG, DMA, CURSOR etc)
* \param [in]   psImmediateFlip     - Immediate flip configuration information
* \param [in]   uFlags              - Reserved.
*
* \retval NONE
*
****************************************************************************/
static void HAL_MDP_SSPP_ImmediateFlipConfig(HAL_MDP_SourcePipeId             eSourcePipe,
                                             HAL_MDP_LayerType                eLayerType,
                                             HAL_MDP_ImmediateFlipConfigType  *psImmediateFlip,
                                             uint32                           uFlags)
{
   uintPtr  uRegisterOffset      = 0;
   uint32   uSrcAddrFetchConfig  = 0;

   // Only VIG, RGB and DMA layers support HSYNC flip
   if(HAL_MDP_LAYER_TYPE_LAYERMIXER_CURSOR != eLayerType)
   {
      if(((HAL_MDP_SOURCE_FLAGS_PRE_SURFACE_CONFIG & uFlags) && !psImmediateFlip->bHsyncFlipEnable) || (0x00 == uFlags))
      {
        // Extract the base address to program the HW registers
        uRegisterOffset   = HAL_MDP_Get_SSPP_BaseOffset(eSourcePipe);

        // ADDR FETCH Configuration
        uSrcAddrFetchConfig  = in_dword(HWIO_MDP_VP_0_VIG_0_SSPP_FETCH_CONFIG_ADDR + uRegisterOffset);
        uSrcAddrFetchConfig  = HWIO_OUT_FLD(uSrcAddrFetchConfig,
                                          MDP_VP_0_VIG_0_SSPP_FETCH_CONFIG,
                                          ADDR_SYNC_MODE,
                                          (uint32)psImmediateFlip->bHsyncFlipEnable);

        out_dword(HWIO_MDP_VP_0_VIG_0_SSPP_FETCH_CONFIG_ADDR + uRegisterOffset, uSrcAddrFetchConfig); // Src Fetch Configuration
      }
   }
}

/****************************************************************************
*
** FUNCTION: HAL_MDP_SSPP_GpuMemoryBankBitConfig()
*/
/*!
* \brief
*     Gpu memory bank bit configuration
*
* \param [in]   eSourcePipe               - Source pipe id
* \param [in]   eLayerType                - Layer Type (RGB, VIG, DMA, CURSOR etc)
* \param [in]   psGpuMemoryBankConfig     - Gpu highest memory bank bit config
* \param [in]   uFlags                    - Reserved.
*
* \retval NONE
*
****************************************************************************/
static void HAL_MDP_SSPP_GpuMemoryBankBitConfig(HAL_MDP_SourcePipeId             eSourcePipe,
                                                HAL_MDP_LayerType                eLayerType,
                                                HAL_MDP_GpuMemoryBankConfigType  *psGpuMemoryBankConfig,
                                                uint32                           uFlags)
{
   uintPtr  uRegisterOffset                 = 0;
   uint32   uSrcAddrFetchConfig             = 0;
   uint32   uGpuHighestMemoryBankBit        = 0;
   uint32   uCurrentGpuHighestMemoryBankBit = 0;
   uint32   uGpuMacroTilingMode             = 0;

   // Extract the base address to program the HW registers
   uRegisterOffset          = HAL_MDP_Get_SSPP_BaseOffset(eSourcePipe);

   // ADDR FETCH Configuration
   uSrcAddrFetchConfig  = in_dword(HWIO_MDP_VP_0_VIG_0_SSPP_FETCH_CONFIG_ADDR + uRegisterOffset);
   uCurrentGpuHighestMemoryBankBit  = HWIO_GETVAL(MDP_VP_0_VIG_0_SSPP_FETCH_CONFIG, uSrcAddrFetchConfig, HIGHEST_BANK_BIT);
   uGpuHighestMemoryBankBit         = HAL_MDP_Get_GpuMemoryBankBitConfig(psGpuMemoryBankConfig, uFlags);
   uGpuMacroTilingMode              = psGpuMemoryBankConfig->uGpuMacroTileMode;
   
   // The highest bank bit configuration should be programmed only once, its the caller 
   // responsibility to do it once, but additional precaution has been taken here 
   // not to do it multiple times
   if(uCurrentGpuHighestMemoryBankBit != uGpuHighestMemoryBankBit)
   {
     uSrcAddrFetchConfig  = HWIO_OUT_FLD(uSrcAddrFetchConfig,
                                           MDP_VP_0_VIG_0_SSPP_FETCH_CONFIG,
                                           HIGHEST_BANK_BIT,
                                           uGpuHighestMemoryBankBit);

     // Write the Highest bank bit value into the register
     out_dword(HWIO_MDP_VP_0_VIG_0_SSPP_FETCH_CONFIG_ADDR + uRegisterOffset, uSrcAddrFetchConfig);
   }
   //Configure the Macro Tiling mode / Address gen mode
   uSrcAddrFetchConfig = HWIO_OUT_FLD(uSrcAddrFetchConfig,
                                      MDP_VP_0_VIG_0_SSPP_FETCH_CONFIG,
                                      ADDR_GEN_MODE,
                                      uGpuMacroTilingMode);

   // Write the Macro Tiling mode / Address gen mode bit value into the register
   out_dword(HWIO_MDP_VP_0_VIG_0_SSPP_FETCH_CONFIG_ADDR + uRegisterOffset, uSrcAddrFetchConfig); // Src Fetch Configuration

}



/****************************************************************************
*
** FUNCTION: HAL_MDP_SSPP_VC1RangeLevelConfig()
*/
/*!
* \brief
*     VC1 range level mapping configuration
*
* \param [in]   eSourcePipe         - Source pipe id
* \param [in]   eLayerType          - Layer Type (RGB, VIG, DMA, CURSOR etc)
* \param [in]   psVC1RangeLevel     - VC1 range level configuration information
* \param [in]   uFlags              - Reserved.
*
* \retval NONE
*
****************************************************************************/
static void HAL_MDP_SSPP_VC1RangeLevelConfig(HAL_MDP_SourcePipeId             eSourcePipe,
                                             HAL_MDP_LayerType                eLayerType,
                                             HAL_MDP_VC1RangeLevelConfigType  *psVC1RangeLevel,
                                             uint32                           uFlags)
{
   uintPtr  uRegisterOffset         = 0;
   uint32   uSrcCurrentFormatInfo   = 0;
   bool32   bVC1RangeMapEnable      = TRUE;
   uint32   uVC1RangeMapReg         = 0x00;

   if(HAL_MDP_LAYER_TYPE_VIG == eLayerType)     // Only ViG Layer supports VC1 range level mapping
   {
      // Extract the base address to program the HW registers
      uRegisterOffset   = HAL_MDP_Get_SSPP_BaseOffset(eSourcePipe);

      if((HAL_MDP_VC1_RANGE_LEVEL_NONE == psVC1RangeLevel->eRangeMapY)   && 
         (HAL_MDP_VC1_RANGE_LEVEL_NONE == psVC1RangeLevel->eRangeMapUV)  )
      {
         bVC1RangeMapEnable = FALSE;        // Disable if both Luma and Chroma are NONE
      }
      else
      {
         uVC1RangeMapReg   = HWIO_OUT_FLD(uVC1RangeMapReg,
                                          MDP_VP_0_VIG_0_SSPP_VC1_RANGE,
                                          RANGE_MAPY,
                                          ((uint32)psVC1RangeLevel->eRangeMapY - 1));     // Range is (0 to 7)

         uVC1RangeMapReg   = HWIO_OUT_FLD(uVC1RangeMapReg,
                                          MDP_VP_0_VIG_0_SSPP_VC1_RANGE,
                                          RANGE_MAPUV,
                                          ((uint32)psVC1RangeLevel->eRangeMapUV - 1));    // Range is (0 to 7)

         out_dword(HWIO_MDP_VP_0_VIG_0_SSPP_VC1_RANGE_ADDR + uRegisterOffset, uVC1RangeMapReg);
      }

      // VC1 range map enable/disable configuration
      uSrcCurrentFormatInfo  = in_dword(HWIO_MDP_VP_0_VIG_0_SSPP_SRC_FORMAT_ADDR + uRegisterOffset);
      uSrcCurrentFormatInfo  = HWIO_OUT_FLD(uSrcCurrentFormatInfo,
                                            MDP_VP_0_VIG_0_SSPP_SRC_FORMAT,
                                            VC1_REDUCE,
                                            (uint32)bVC1RangeMapEnable);

      out_dword(HWIO_MDP_VP_0_VIG_0_SSPP_SRC_FORMAT_ADDR + uRegisterOffset, uSrcCurrentFormatInfo);    // Src format information
   }
}

/****************************************************************************
*
** FUNCTION: HAL_MDP_SSPP_FetchModeConfig()
*/
/*!
* \brief
*     Flip/Mirror mode configuration (Vertical or Horizontal mirror mode)
*
* \param [in]   eSourcePipe         - Source pipe id
* \param [in]   eLayerType          - Layer Type (RGB, VIG, DMA, CURSOR etc)
* \param [in]   psFetchInfo         - Fetch configuration information
* \param [in]   uFlags              - Reserved.
*
* \retval NONE
*
****************************************************************************/
static void HAL_MDP_SSPP_FetchModeConfig(HAL_MDP_SourcePipeId        eSourcePipe,
                                         HAL_MDP_LayerType           eLayerType,
                                         HAL_MDP_FetchModeConfigType *psFetchInfo,
                                         uint32                      uFlags)
{
   uintPtr  uRegisterOffset      = 0;
   uint32   uSrcOpModeInfo       = 0;

   // Extract the base address to program the HW registers
   uRegisterOffset   = HAL_MDP_Get_SSPP_BaseOffset(eSourcePipe);

   // The offsets for REC0 and REC1 registers are different
   // Therefore the Smart DMA base is treated differently from physcial DMA
   if (HAL_MDP_SMART_DMA_REC1_ID == HAL_MDP_Get_SmartDMARecID(eSourcePipe)) // SMART DMA REC1
   {
      uSrcOpModeInfo  = in_dword(HWIO_MDP_VP_0_VIG_0_SSPP_SRC_OP_MODE_REC1_ADDR + uRegisterOffset);
      uSrcOpModeInfo &= (~HWIO_MDP_VP_0_VIG_0_SSPP_SRC_OP_MODE_FLIP_MODE_BMSK);
      uSrcOpModeInfo  = HWIO_OUT_FLD(uSrcOpModeInfo,
                                     MDP_VP_0_VIG_0_SSPP_SRC_OP_MODE,
                                     FLIP_MODE,
                                     psFetchInfo->eFetchConfigType);

      out_dword(HWIO_MDP_VP_0_VIG_0_SSPP_SRC_OP_MODE_REC1_ADDR + uRegisterOffset, uSrcOpModeInfo);  // Update fetch information
   }
   else
   {
      uSrcOpModeInfo  = in_dword(HWIO_MDP_VP_0_VIG_0_SSPP_SRC_OP_MODE_ADDR + uRegisterOffset);
      uSrcOpModeInfo &= (~HWIO_MDP_VP_0_VIG_0_SSPP_SRC_OP_MODE_FLIP_MODE_BMSK);
      uSrcOpModeInfo  = HWIO_OUT_FLD(uSrcOpModeInfo,
                                  MDP_VP_0_VIG_0_SSPP_SRC_OP_MODE,
                                  FLIP_MODE,
                                  psFetchInfo->eFetchConfigType);

      out_dword(HWIO_MDP_VP_0_VIG_0_SSPP_SRC_OP_MODE_ADDR + uRegisterOffset, uSrcOpModeInfo);  // Update fetch information
   }
}

/****************************************************************************
*
** FUNCTION: HAL_MDP_SSPP_IGCConfig()
*/
/*!
* \brief
*     Enables or disables the IGC feature for the given pipe
*
* \param [in]   eSourcePipe         - Source pipe id
* \param [in]   eLayerType          - Layer Type (RGB, VIG, DMA, CURSOR etc)
* \param [in]   psIGCInfo           - IGC enable/disable information
* \param [in]   uFlags              - Reserved
*
* \retval NONE
*
****************************************************************************/
static void HAL_MDP_SSPP_IGCConfig(HAL_MDP_SourcePipeId      eSourcePipe,
                                   HAL_MDP_LayerType         eLayerType,
                                   HAL_MDP_IGCConfigType    *psIGCInfo,
                                   uint32                    uFlags)
{
   // SSPP IGC Deprecated
}

/****************************************************************************
*
** FUNCTION: HAL_MDP_SSPP_SharpenConfig()
*/
/*!
* \brief
*     Set up configurations for a particular source pipe
*
* \param [in]   eSourcePipe         - Source pipe id
* \param [in]   eLayerType          - Layer Type (RGB, VIG, DMA, CURSOR etc)
* \param [in]   psSharpenInfo       - QSEED Sharpening Information
* \param [in]   uFlags              - Reserved
*
* \retval NONE
*
****************************************************************************/
static void HAL_MDP_SSPP_SharpenConfig(HAL_MDP_SourcePipeId         eSourcePipe,
                                       HAL_MDP_LayerType            eLayerType,
                                       HAL_MDP_SharpenConfigType   *psSharpenInfo,
                                       uint32                       uFlags)
{
   int32                  iSharpeningLevel        = 0;
   uintPtr                uRegisterOffset         = 0;  
   uint32                 uSharpEdgeThresh        = 0;
   uint32                 uSmoothThresh           = 0;
   uint32                 uNoiseThresh            = 0; 
   HAL_MDSS_ErrorType     eStatus                 = HAL_MDSS_STATUS_SUCCESS;

   // Extract the base address to program the HW registers
  uRegisterOffset = HAL_MDP_Get_SSPP_BaseOffset(eSourcePipe);

   switch (psSharpenInfo->eSharpeningMode)
   {
      case HAL_MDP_SHARPENING_TYPE_NONE:
      {
         // Default no sharpening/smoothening 
         iSharpeningLevel        =  HAL_MDP_DEFAULT_SHARPENING_LEVEL;
         uSharpEdgeThresh        =  HAL_MDP_DEFAULT_SHARPENING_SHARP_EDGE_THRESH;
         uSmoothThresh           =  HAL_MDP_DEFAULT_SHARPENING_SMOOTH_THRESH;
         uNoiseThresh            =  HAL_MDP_DEFAULT_SHARPENING_NOISE_THRESH;        

         break; 
      }

      case HAL_MDP_SHARPENING_TYPE_SHARPEN:
      { 
         int32 iNormalizeLevel   =  0;

         iNormalizeLevel         =  (255 * psSharpenInfo->uLevel) / 100; 

         // Content-adaptive sharpening 
         iSharpeningLevel        =  iNormalizeLevel;        
         uSharpEdgeThresh        =  HAL_MDP_DEFAULT_CONTENT_ADAPTIVE_SHARPENING_SHARP_EDGE_THRESH;
         uSmoothThresh           =  HAL_MDP_DEFAULT_CONTENT_ADAPTIVE_SHARPENING_SMOOTH_THRESH;
         uNoiseThresh            =  HAL_MDP_DEFAULT_CONTENT_ADAPTIVE_SHARPENING_NOISE_THRESH;    

         break; 
      }

      case HAL_MDP_SHARPENING_TYPE_SMOOTHEN:
      {
         int32 iNormalizeLevel   =  0;

         iNormalizeLevel         =  (256 * psSharpenInfo->uLevel) / 100; 

         // Smoothening 
         iSharpeningLevel        =  (-iNormalizeLevel);
         uSharpEdgeThresh        =  HAL_MDP_DEFAULT_SMOOTHENING_SHARP_EDGE_THRESH;
         uSmoothThresh           =  HAL_MDP_DEFAULT_SMOOTHENING_SMOOTH_THRESH;
         uNoiseThresh            =  HAL_MDP_DEFAULT_SMOOTHENING_NOISE_THRESH;    

         break; 
      }

      default:
         eStatus     = HAL_MDSS_STATUS_FAILED_NOT_SUPPORTED;
         break;
   }

   if (HAL_MDSS_STATUS_SUCCESS == eStatus) 
   {
      // Sharpen edge threshold 
      out_dword(HWIO_MDP_VP_0_VIG_0_QSEED2_SHARP_THRESHOLD_EDGE_ADDR + uRegisterOffset, uSharpEdgeThresh);   
      // Smooth threshold 
      out_dword(HWIO_MDP_VP_0_VIG_0_QSEED2_SHARP_THRESHOLD_SMOOTH_ADDR + uRegisterOffset, uSmoothThresh);   
      // Noise threshold 
      out_dword(HWIO_MDP_VP_0_VIG_0_QSEED2_SHARP_THRESHOLD_NOISE_ADDR + uRegisterOffset, uNoiseThresh);   
      // Sharpening levels (Smoothen : -256 to -1 ; Sharpen : 1 to 255; None : 0)
      out_dword(HWIO_MDP_VP_0_VIG_0_QSEED2_SHARP_SMOOTH_STRENGTH_ADDR + uRegisterOffset, iSharpeningLevel);               
   }

}

/****************************************************************************
*
** FUNCTION: HAL_MDP_SSPP_HistogramConfig()
*/
/*!
* \brief
*     Set up configurations for a particular source pipe
*
* \param [in]   eSourcePipe         - Source pipe id
* \param [in]   eLayerType          - Layer Type (RGB, VIG, DMA, CURSOR etc)
* \param [in]   psHistogramInfo     - Histogram configuration information
* \param [in]   uFlags              - Reserved
*
* \retval NONE
*
****************************************************************************/
static void HAL_MDP_SSPP_HistogramConfig(HAL_MDP_SourcePipeId         eSourcePipe,
                                         HAL_MDP_LayerType            eLayerType,
                                         HAL_MDP_HistogramConfigType *psHistogramInfo,
                                         uint32                       uFlags)
{

}


/****************************************************************************
*
** FUNCTION: HAL_MDP_SSPP_Picture_Adjustment_Calculation
*/
/*!
* \brief
*     Calculates the actual picture adjustment value for the HSIC settings
*
* \param [in]   iPictAdjustValue        - Picture adjustment value between -100% to 100%
* \param [in]   iMinPictAdjustValue     - Minimum actual picture adjustment value 
* \param [in]   iMaxPictAdjustValue     - Maximum actual picture adjustment value 
* \param [in]   iMinUnchPictAdjustValue - Unchange value going to the direction of minimum picture adjustment value
* \param [in]   iMaxUnchPictAdjustValue - Unchange value going to the direction of maximum picture adjustment value
*
* \retval Actual picture adjustment value
*
****************************************************************************/
static int32 HAL_MDP_SSPP_Picture_Adjustment_Calculation(int32         iPictAdjustValue,
                                                         int32         iMinPictAdjustValue,
                                                         int32         iMaxPictAdjustValue,
                                                         int32         iMinUnchPictAdjustValue,
                                                         int32         iMaxUnchPictAdjustValue,
                                                         uint32        uMaxInputRange)
{
   int32 iValue = 0;
   int32  iInputRange = (uMaxInputRange>0)?(int32)uMaxInputRange:100;

   if (iPictAdjustValue > 0)
   {
       iValue = ((iPictAdjustValue * (iMaxPictAdjustValue - iMaxUnchPictAdjustValue))/(iInputRange)) + iMaxUnchPictAdjustValue;
   }
   else
   {
       iValue = ((iPictAdjustValue * (iMinPictAdjustValue - iMinUnchPictAdjustValue))/(-iInputRange)) + iMinUnchPictAdjustValue;
   }

   return iValue; 
}


/****************************************************************************
*
** FUNCTION: HAL_MDP_SSPP_PictureAdjustmentConfig()
*/
/*!
* \brief
*     Set up configurations for a particular source pipe
*
* \param [in]   eSourcePipe         - Source pipe id
* \param [in]   eLayerType          - Layer Type (RGB, VIG, DMA, CURSOR etc)
* \param [in]   psPictureAdjustInfo - SPA (Source Picture Adjust)/CE (Color Enhancement) information
* \param [in]   uFlags              - Reserved
*
* \retval NONE
*
****************************************************************************/
static void HAL_MDP_SSPP_PictureAdjustmentConfig(HAL_MDP_SourcePipeId             eSourcePipe,
                                                 HAL_MDP_LayerType                eLayerType,
                                                 HAL_MDP_PictureAdjustConfigType  *psPictureAdjustInfo,
                                                 uint32                           uFlags)
{
   int32  iPictAdjValue    = 0;
   int32  iSatAdjVal       = 0;
   uint32 uPictAdjEnable   = 0;
   uintPtr uLayerRegOffset  = 0;
   
   /* Picture adjustment is done only for VG layers */
   if (HAL_MDP_LAYER_TYPE_VIG == eLayerType)
   {
      /* Extract the layer base address */
      uLayerRegOffset = HAL_MDP_Get_Layer_BaseOffset(eSourcePipe);

      /* Enable/Disable the picture adjustment */
      uPictAdjEnable = in_dword(HWIO_MDP_VP_0_VIG_0_PA_MODE_ADDR + uLayerRegOffset);
      uPictAdjEnable = HWIO_OUT_FLD(uPictAdjEnable, MDP_VP_0_VIG_0_PA_MODE, PA_EN, psPictureAdjustInfo->bEnable);

      // Along with the enabling/disabling enable bit, mask bits also needs to be enabled/disabled
      if(TRUE == psPictureAdjustInfo->bEnable)
      {
        uPictAdjEnable |= HWIO_MDP_VP_0_VIG_0_PA_MODE_PA_CONT_MASK_BMSK      |
                          HWIO_MDP_VP_0_VIG_0_PA_MODE_PA_VAL_MASK_BMSK       |
                          HWIO_MDP_VP_0_VIG_0_PA_MODE_PA_SAT_MASK_BMSK       |
                          HWIO_MDP_VP_0_VIG_0_PA_MODE_PA_HUE_MASK_BMSK;
      }
      else
      {
        // Disable the HSIC adjustment masks
        uPictAdjEnable &= ~(HWIO_MDP_VP_0_VIG_0_PA_MODE_PA_CONT_MASK_BMSK      |
                            HWIO_MDP_VP_0_VIG_0_PA_MODE_PA_VAL_MASK_BMSK       |
                            HWIO_MDP_VP_0_VIG_0_PA_MODE_PA_SAT_MASK_BMSK       |
                            HWIO_MDP_VP_0_VIG_0_PA_MODE_PA_HUE_MASK_BMSK);
      }

      out_dword(HWIO_MDP_VP_0_VIG_0_PA_MODE_ADDR + uLayerRegOffset, uPictAdjEnable);

      /* Check if the picture adjustment info is enabled */
      if (TRUE == psPictureAdjustInfo->bEnable)
      {
         /* Hue */
         iPictAdjValue = HAL_MDP_SSPP_Picture_Adjustment_Calculation(psPictureAdjustInfo->iHueValue, 
                                                                     pgsMdpHwInfo->sPictureAdjustInfo.iMinHue,
                                                                     pgsMdpHwInfo->sPictureAdjustInfo.iMaxHue,
                                                                     pgsMdpHwInfo->sPictureAdjustInfo.iMinUnchangeHue,
                                                                     pgsMdpHwInfo->sPictureAdjustInfo.iMaxUnchangeHue,
                                                                     HAL_MDP_PA_HUE_INPUT_RANGE);
         out_dword(HWIO_MDP_VP_0_VIG_0_PA_HUE_ADJUST_ADDR + uLayerRegOffset, iPictAdjValue);              

         /* Contrast */
         iPictAdjValue = HAL_MDP_SSPP_Picture_Adjustment_Calculation(psPictureAdjustInfo->iContrastValue, 
                                                                     pgsMdpHwInfo->sPictureAdjustInfo.iMinContrast,
                                                                     pgsMdpHwInfo->sPictureAdjustInfo.iMaxContrast,
                                                                     pgsMdpHwInfo->sPictureAdjustInfo.iUnchangeContrast,
                                                                     pgsMdpHwInfo->sPictureAdjustInfo.iUnchangeContrast,
                                                                     HAL_MDP_PA_HUE_INPUT_RANGE);
         out_dword(HWIO_MDP_VP_0_VIG_0_PA_CONT_ADJUST_ADDR + uLayerRegOffset, iPictAdjValue);

         /* Saturation + Saturation threshold (default 26) */
         iSatAdjVal = in_dword(HWIO_MDP_VP_0_VIG_0_PA_SAT_ADJUST_ADDR + uLayerRegOffset);
         iPictAdjValue = HAL_MDP_SSPP_Picture_Adjustment_Calculation(psPictureAdjustInfo->iSaturationValue, 
                                                                     pgsMdpHwInfo->sPictureAdjustInfo.iMinSaturation,
                                                                     pgsMdpHwInfo->sPictureAdjustInfo.iMaxSaturation,
                                                                     pgsMdpHwInfo->sPictureAdjustInfo.iUnchangeSaturation,
                                                                     pgsMdpHwInfo->sPictureAdjustInfo.iUnchangeSaturation,
                                                                     HAL_MDP_PA_HUE_INPUT_RANGE);
         iSatAdjVal = HWIO_OUT_FLD(iSatAdjVal, MDP_VP_0_VIG_0_PA_SAT_ADJUST, VALUE, iPictAdjValue);
         out_dword(HWIO_MDP_VP_0_VIG_0_PA_SAT_ADJUST_ADDR + uLayerRegOffset, iSatAdjVal);

         /* Intensity */
         iPictAdjValue = HAL_MDP_SSPP_Picture_Adjustment_Calculation(psPictureAdjustInfo->iIntensityValue, 
                                                                     pgsMdpHwInfo->sPictureAdjustInfo.iMinIntensity,
                                                                     pgsMdpHwInfo->sPictureAdjustInfo.iMaxIntensity,
                                                                     pgsMdpHwInfo->sPictureAdjustInfo.iUnchangeIntensity,
                                                                     pgsMdpHwInfo->sPictureAdjustInfo.iUnchangeIntensity,
                                                                     HAL_MDP_PA_HUE_INPUT_RANGE);
         out_dword(HWIO_MDP_VP_0_VIG_0_PA_VAL_ADJUST_ADDR + uLayerRegOffset, iPictAdjValue);
      }
   }
}


/****************************************************************************
*
** FUNCTION: HAL_MDP_SSPP_CSC_1_Init()
*/
/*!
* \brief
*     Initializes the CSC_1 (Color Space Connversion) module of given pipe
*
* \param [in] eSourcePipe           - Source Pipe ID
* \param [in] eLayerType            - Layer Type (RGB, VIG, DMA, or CURSOR etc)
* \param [in] eColorEncodeType      - Color Encoding Type
* \param [in] uFlags                - Reserved
*
* \retval NONE
*
****************************************************************************/
static void HAL_MDP_SSPP_CSC_1_Init(HAL_MDP_SourcePipeId    eSourcePipe,
                                    HAL_MDP_LayerType       eLayerType,
                                    HAL_MDP_ColorEncodeType eColorEncodeType,
                                    uint32                  uFlags)
{
   uintPtr        uLayerRegOffset   = 0x0;
   uint32        *pCSCMatrixCoeff   = NULL;
   uint32        *pCSCPreClamp      = NULL;
   uint32        *pCSCPostClamp     = NULL;
   uint16        *pCSCPreBias       = NULL;
   uint16        *pCSCPostBias      = NULL;

   HAL_MDSS_ErrorType      eStatus     = HAL_MDSS_STATUS_SUCCESS;

   switch(eLayerType)
   {
   case HAL_MDP_LAYER_TYPE_VIG:

      // Extract the layer base address
      uLayerRegOffset   = HAL_MDP_Get_Layer_BaseOffset(eSourcePipe);

      switch(eColorEncodeType)
      {
      case HAL_MDP_COLOR_ENCODING_DEFAULT:
      case HAL_MDP_COLOR_ENCODING_REC601:            /** ITU-R BT.601 (CCIR601)  */
         pCSCMatrixCoeff   = pgsMdpHwInfo->pCSC_ColorYUVToRGBEncodingInfo[HAL_MDP_COLOR_ENCODING_REC601]->pCSC_Coeff_Matrix;
         pCSCPreClamp      = pgsMdpHwInfo->pCSC_ColorYUVToRGBEncodingInfo[HAL_MDP_COLOR_ENCODING_REC601]->pCSC10Bit_PreClamp;
         pCSCPostClamp     = pgsMdpHwInfo->pCSC_ColorYUVToRGBEncodingInfo[HAL_MDP_COLOR_ENCODING_REC601]->pCSC10Bit_PostClamp;
         pCSCPreBias       = pgsMdpHwInfo->pCSC_ColorYUVToRGBEncodingInfo[HAL_MDP_COLOR_ENCODING_REC601]->pCSC10Bit_PreBias;
         pCSCPostBias      = pgsMdpHwInfo->pCSC_ColorYUVToRGBEncodingInfo[HAL_MDP_COLOR_ENCODING_REC601]->pCSC10Bit_PostBias;
         break;

      case HAL_MDP_COLOR_ENCODING_REC709:            /** ITU-R BT.709 (CCIR709)  */
         pCSCMatrixCoeff   = pgsMdpHwInfo->pCSC_ColorYUVToRGBEncodingInfo[HAL_MDP_COLOR_ENCODING_REC709]->pCSC_Coeff_Matrix;
         pCSCPreClamp      = pgsMdpHwInfo->pCSC_ColorYUVToRGBEncodingInfo[HAL_MDP_COLOR_ENCODING_REC709]->pCSC10Bit_PreClamp;
         pCSCPostClamp     = pgsMdpHwInfo->pCSC_ColorYUVToRGBEncodingInfo[HAL_MDP_COLOR_ENCODING_REC709]->pCSC10Bit_PostClamp;
         pCSCPreBias       = pgsMdpHwInfo->pCSC_ColorYUVToRGBEncodingInfo[HAL_MDP_COLOR_ENCODING_REC709]->pCSC10Bit_PreBias;
         pCSCPostBias      = pgsMdpHwInfo->pCSC_ColorYUVToRGBEncodingInfo[HAL_MDP_COLOR_ENCODING_REC709]->pCSC10Bit_PostBias;
         break;

      case HAL_MDP_COLOR_ENCODING_FULL_REC601:      /**< ITU.BT-601 SDTV         */   
         pCSCMatrixCoeff   = pgsMdpHwInfo->pCSC_ColorYUVToRGBEncodingInfo[HAL_MDP_COLOR_ENCODING_FULL_REC601]->pCSC_Coeff_Matrix;
         pCSCPreClamp      = pgsMdpHwInfo->pCSC_ColorYUVToRGBEncodingInfo[HAL_MDP_COLOR_ENCODING_FULL_REC601]->pCSC10Bit_PreClamp;
         pCSCPostClamp     = pgsMdpHwInfo->pCSC_ColorYUVToRGBEncodingInfo[HAL_MDP_COLOR_ENCODING_FULL_REC601]->pCSC10Bit_PostClamp;
         pCSCPreBias       = pgsMdpHwInfo->pCSC_ColorYUVToRGBEncodingInfo[HAL_MDP_COLOR_ENCODING_FULL_REC601]->pCSC10Bit_PreBias;
         pCSCPostBias      = pgsMdpHwInfo->pCSC_ColorYUVToRGBEncodingInfo[HAL_MDP_COLOR_ENCODING_FULL_REC601]->pCSC10Bit_PostBias;
         break;

      case HAL_MDP_COLOR_ENCODING_FULL_REC709:      /**< ITU.BT-709 HDTV         */ 
         pCSCMatrixCoeff   = pgsMdpHwInfo->pCSC_ColorYUVToRGBEncodingInfo[HAL_MDP_COLOR_ENCODING_FULL_REC709]->pCSC_Coeff_Matrix;
         pCSCPreClamp      = pgsMdpHwInfo->pCSC_ColorYUVToRGBEncodingInfo[HAL_MDP_COLOR_ENCODING_FULL_REC709]->pCSC10Bit_PreClamp;
         pCSCPostClamp     = pgsMdpHwInfo->pCSC_ColorYUVToRGBEncodingInfo[HAL_MDP_COLOR_ENCODING_FULL_REC709]->pCSC10Bit_PostClamp;
         pCSCPreBias       = pgsMdpHwInfo->pCSC_ColorYUVToRGBEncodingInfo[HAL_MDP_COLOR_ENCODING_FULL_REC709]->pCSC10Bit_PreBias;
         pCSCPostBias      = pgsMdpHwInfo->pCSC_ColorYUVToRGBEncodingInfo[HAL_MDP_COLOR_ENCODING_FULL_REC709]->pCSC10Bit_PostBias;
         break; 

      case HAL_MDP_COLOR_ENCODING_sRGB:              /** Standard RGB */     
      case HAL_MDP_COLOR_ENCODING_MAX:
      case HAL_MDP_COLOR_ENCODING_FORCE_32BIT:
      default:
         eStatus     = HAL_MDSS_STATUS_FAILED_NOT_SUPPORTED;
         break;
      }
      break;

   case HAL_MDP_LAYER_TYPE_RGB:
   case HAL_MDP_LAYER_TYPE_DMA:
   case HAL_MDP_LAYER_TYPE_CURSOR:
   case HAL_MDP_LAYER_TYPE_SSPP_CURSOR:
   case HAL_MDP_LAYER_TYPE_NONE:
   case HAL_MDP_LAYER_TYPE_MAX:
   case HAL_MDP_LAYER_TYPE_FORCE_32BIT:
   default:
      eStatus = HAL_MDSS_STATUS_FAILED_NOT_SUPPORTED;
      break;
   }

   if(HAL_MDSS_STATUS_SUCCESS == eStatus)
   {
      if ((NULL != pCSCMatrixCoeff) && (NULL != pCSCPreClamp) &&
          (NULL != pCSCPostClamp)   && (NULL != pCSCPreBias ) && 
          (NULL != pCSCPostBias))
      {
         // Matrix Coefficients
         out_dword(HWIO_MDP_VP_0_VIG_0_CSC_1_MATRIX_COEFF_0_ADDR + uLayerRegOffset, *(pCSCMatrixCoeff + 0x00000000) );
         out_dword(HWIO_MDP_VP_0_VIG_0_CSC_1_MATRIX_COEFF_1_ADDR + uLayerRegOffset, *(pCSCMatrixCoeff + 0x00000001) );
         out_dword(HWIO_MDP_VP_0_VIG_0_CSC_1_MATRIX_COEFF_2_ADDR + uLayerRegOffset, *(pCSCMatrixCoeff + 0x00000002) );
         out_dword(HWIO_MDP_VP_0_VIG_0_CSC_1_MATRIX_COEFF_3_ADDR + uLayerRegOffset, *(pCSCMatrixCoeff + 0x00000003) );
         out_dword(HWIO_MDP_VP_0_VIG_0_CSC_1_MATRIX_COEFF_4_ADDR + uLayerRegOffset, *(pCSCMatrixCoeff + 0x00000004) );

         // Preclamp
         out_dword(HWIO_MDP_VP_0_VIG_0_CSC_1_COMP_0_PRE_CLAMP_ADDR + uLayerRegOffset, *(pCSCPreClamp + 0x00000000) );
         out_dword(HWIO_MDP_VP_0_VIG_0_CSC_1_COMP_1_PRE_CLAMP_ADDR + uLayerRegOffset, *(pCSCPreClamp + 0x00000001) );
         out_dword(HWIO_MDP_VP_0_VIG_0_CSC_1_COMP_2_PRE_CLAMP_ADDR + uLayerRegOffset, *(pCSCPreClamp + 0x00000002) );

         // Postclamp
         out_dword(HWIO_MDP_VP_0_VIG_0_CSC_1_COMP_0_POST_CLAMP_ADDR + uLayerRegOffset, *(pCSCPostClamp + 0x00000000) );
         out_dword(HWIO_MDP_VP_0_VIG_0_CSC_1_COMP_1_POST_CLAMP_ADDR + uLayerRegOffset, *(pCSCPostClamp + 0x00000001) );
         out_dword(HWIO_MDP_VP_0_VIG_0_CSC_1_COMP_2_POST_CLAMP_ADDR + uLayerRegOffset, *(pCSCPostClamp + 0x00000002) );

         // Prebias
         out_dword(HWIO_MDP_VP_0_VIG_0_CSC_1_COMP_0_PRE_BIAS_ADDR + uLayerRegOffset, *(pCSCPreBias + 0x00000000) );
         out_dword(HWIO_MDP_VP_0_VIG_0_CSC_1_COMP_1_PRE_BIAS_ADDR + uLayerRegOffset, *(pCSCPreBias + 0x00000001) );
         out_dword(HWIO_MDP_VP_0_VIG_0_CSC_1_COMP_2_PRE_BIAS_ADDR + uLayerRegOffset, *(pCSCPreBias + 0x00000002) );

         // Postbias
         out_dword(HWIO_MDP_VP_0_VIG_0_CSC_1_COMP_0_POST_BIAS_ADDR + uLayerRegOffset, *(pCSCPostBias + 0x00000000) );
         out_dword(HWIO_MDP_VP_0_VIG_0_CSC_1_COMP_1_POST_BIAS_ADDR + uLayerRegOffset, *(pCSCPostBias + 0x00000001) );
         out_dword(HWIO_MDP_VP_0_VIG_0_CSC_1_COMP_2_POST_BIAS_ADDR + uLayerRegOffset, *(pCSCPostBias + 0x00000002) );
      }
   }
}

/****************************************************************************
*
** FUNCTION: HAL_MDP_SSPP_ColorEncodeConfig()
*/
/*!
* \brief
*     Set up configurations for a particular source pipe
*
* \param [in]   eSourcePipe         - Source pipe id
* \param [in]   eLayerType          - Layer Type (RGB, VIG, DMA, or CURSOR etc)
* \param [in]   psColorEncInfo      - Color encoding information
* \param [in]   uFlags              - Reserved
*
* \retval NONE
*
****************************************************************************/
static void HAL_MDP_SSPP_ColorEncodeConfig(HAL_MDP_SourcePipeId            eSourcePipe,
                                           HAL_MDP_LayerType               eLayerType,
                                           HAL_MDP_ColorEncodeConfigType  *psColorEncInfo,
                                           uint32                          uFlags)
{
   HAL_MDP_SSPP_CSC_1_Init(eSourcePipe,
                           eLayerType,
                           psColorEncInfo->eColorEncodeType,
                           0x00);
}

/****************************************************************************
*
** FUNCTION: HAL_MDP_SSPP_QSEED_Init()
*/
/*!
* \brief
*     Initializes the QSEED Normal and Smooth Filter Coefficients of given pipe
*
* \param [in] eSourcePipe           - Source Pipe ID
* \param [in] eLayerType            - Layer Type (RGB, VIG, DMA, or CURSOR etc)
* \param [in] uFlags                - Reserved
*
* \retval NONE
*
****************************************************************************/
static void HAL_MDP_SSPP_QSEED_Init(HAL_MDP_SourcePipeId    eSourcePipe, 
                                    HAL_MDP_LayerType       eLayerType,
                                    uint32                  uFlags)
{
   uintPtr            uLayerRegOffset    = 0x0;
   HAL_MDSS_ErrorType eStatus            = HAL_MDSS_STATUS_SUCCESS;

   switch(eLayerType)
   {
   case HAL_MDP_LAYER_TYPE_VIG:

      // Extract the layer base address
      uLayerRegOffset   = HAL_MDP_Get_Layer_BaseOffset(eSourcePipe);
      break;
   case HAL_MDP_LAYER_TYPE_RGB:
   case HAL_MDP_LAYER_TYPE_DMA:
   case HAL_MDP_LAYER_TYPE_CURSOR:
   case HAL_MDP_LAYER_TYPE_SSPP_CURSOR:
   case HAL_MDP_LAYER_TYPE_NONE:
   case HAL_MDP_LAYER_TYPE_MAX:
   case HAL_MDP_LAYER_TYPE_FORCE_32BIT:
   default:
      eStatus = HAL_MDSS_STATUS_FAILED_NOT_SUPPORTED;
      break;
   }

   if(HAL_MDSS_STATUS_SUCCESS == eStatus)
   {
     HAL_MDP_ScalarConfigType sScalarConfig;
     MDSS_OSAL_MEMZERO((char*)&sScalarConfig, sizeof(sScalarConfig));
     sScalarConfig.b2dWriteEnable     = TRUE;
     sScalarConfig.e2dLutIndex        = HAL_MDP_QSEED_LUT_INDEX_0;
     
     HAL_MDP_SSPP_ScalerLUTConfig(eSourcePipe, &sScalarConfig);
   }
}

/****************************************************************************
*
** FUNCTION: HAL_MDP_SSPP_MultiRectConfig()
*/
/*!
* \brief
*     Configures multi rectangle settings
*
* \param [in] eSourcePipe           - Source Pipe ID
* \param [in] eLayerType            - Layer Type (RGB, VIG, DMA, or CURSOR etc)
* \param [in] psMultiRectInfo       - Multi rectangle information
* \param [in] uFlags                - Reserved
*
* \retval NONE
*
****************************************************************************/
static void HAL_MDP_SSPP_MultiRectConfig(HAL_MDP_SourcePipeId         eSourcePipe,
                                         HAL_MDP_LayerType            eLayerType,
                                         HAL_MDP_MultiRectConfigType *psMultiRectInfo,
                                         uint32                       uFlags)
{
   uintPtr  uRegisterOffset         = 0;
   uint32   uMultiRectOpModeInfo    = 0;
   uint32   uMultiRecEnableMask     = 0;
   uint32   uSourceRecID            = HAL_MDP_Get_SmartDMARecID(eSourcePipe);

   // Configure the multi-rect parameters based on the currently programmed ROIs
   if (TRUE == psMultiRectInfo->bEnable)
   {
     // If eSourcePipe is REC0, then eVirtualSourcePipe is REC1 and vice versa
     // then uSourceRecID = 0, and uVirtualRecID = 1
     
     HAL_MDP_SourcePipeId eVirtualSourcePipe    = pgsMdpHwInfo->pMultiRectMappingTable[eSourcePipe];
     uint32               uVirtualRecID         = HAL_MDP_Get_SmartDMARecID(eVirtualSourcePipe);

     uint32               uPosX                 = 0;
     uint32               uPosY                 = 0;
     uint32               uWidth                = 0;
     uint32               uHeight               = 0;

     uint32               uVirtualPosX          = 0;
     uint32               uVirtualPosY          = 0;
     uint32               uVirtualWidth         = 0;
     uint32               uVirtualHeight        = 0;
     uint32               uRegVal               = 0;

     uRegisterOffset      = HAL_MDP_Get_SSPP_BaseOffset(eSourcePipe);

     // Read current multi-rect config
     uMultiRectOpModeInfo = in_dword(HWIO_MDP_VP_0_VIG_0_SSPP_MULTI_REC_OP_MODE_ADDR + uRegisterOffset);
     uMultiRecEnableMask  = HWIO_GETVAL(MDP_VP_0_VIG_0_SSPP_MULTI_REC_OP_MODE,
                                        uMultiRectOpModeInfo,
                                        MULTI_REC_ENABLE);

     // Check whether virtual pipe is enabled
     // For example, if eSourcePipe is REC0, this checks whether REC1 is enabled
     // If uMultiRecEnableMask = b01, means REC0 is enabled
     // If uMultiRecEnableMask = b10, means REC1 is enabled
     // If uMultiRecEnableMask = b11, means both REC0 and REC1 are enabled
     if (uMultiRecEnableMask & (1 << uVirtualRecID))
     {
       // If virtual pipe is enabled, then read the ROIs programmed for both RECs 
       // and calculate the multirect mode accordingly
       if (HAL_MDP_SMART_DMA_REC1_ID == uSourceRecID) // REC1
       {
         // eSourcePipe is REC1
         uRegisterOffset   = HAL_MDP_Get_SSPP_BaseOffset(eSourcePipe);
         
         uRegVal           = in_dword(HWIO_MDP_VP_0_VIG_0_SSPP_OUT_XY_REC1_ADDR + uRegisterOffset);
         uPosX             = HWIO_GETVAL(MDP_VP_0_VIG_0_SSPP_OUT_XY,
                                         uRegVal,
                                         DST_X);
         uPosY             = HWIO_GETVAL(MDP_VP_0_VIG_0_SSPP_OUT_XY,
                                         uRegVal,
                                         DST_Y);

         uRegVal           = in_dword(HWIO_MDP_VP_0_VIG_0_SSPP_OUT_SIZE_REC1_ADDR + uRegisterOffset);
         uWidth            = HWIO_GETVAL(MDP_VP_0_VIG_0_SSPP_OUT_SIZE,
                                         uRegVal,
                                         DST_W);
         uHeight           = HWIO_GETVAL(MDP_VP_0_VIG_0_SSPP_OUT_SIZE,
                                         uRegVal,
                                         DST_H);
       }
       else
       {
         // eSourcePipe is REC0
         uRegisterOffset = HAL_MDP_Get_SSPP_BaseOffset(eSourcePipe);

         uRegVal         = in_dword(HWIO_MDP_VP_0_VIG_0_SSPP_OUT_XY_ADDR + uRegisterOffset);
         uPosX           = HWIO_GETVAL(MDP_VP_0_VIG_0_SSPP_OUT_XY,
                                       uRegVal,
                                       DST_X);
         uPosY           = HWIO_GETVAL(MDP_VP_0_VIG_0_SSPP_OUT_XY,
                                       uRegVal,
                                       DST_Y);

         uRegVal         = in_dword(HWIO_MDP_VP_0_VIG_0_SSPP_OUT_SIZE_ADDR + uRegisterOffset);
         uWidth          = HWIO_GETVAL(MDP_VP_0_VIG_0_SSPP_OUT_SIZE,
                                       uRegVal,
                                       DST_W);
         uHeight         = HWIO_GETVAL(MDP_VP_0_VIG_0_SSPP_OUT_SIZE,
                                       uRegVal,
                                       DST_H);
       }

       // Read the ROIs of virtual pipe
       if (HAL_MDP_SMART_DMA_REC1_ID == uVirtualRecID) //REC1
       {
         // eVirtualSourcePipe is REC1
         uRegisterOffset = HAL_MDP_Get_SSPP_BaseOffset(eVirtualSourcePipe);

         uRegVal         = in_dword(HWIO_MDP_VP_0_VIG_0_SSPP_OUT_XY_REC1_ADDR + uRegisterOffset);
         uVirtualPosX    = HWIO_GETVAL(MDP_VP_0_VIG_0_SSPP_OUT_XY,
                                       uRegVal,
                                       DST_X);
         uVirtualPosY    = HWIO_GETVAL(MDP_VP_0_VIG_0_SSPP_OUT_XY,
                                       uRegVal,
                                       DST_Y);
         
         uRegVal         = in_dword(HWIO_MDP_VP_0_VIG_0_SSPP_OUT_SIZE_REC1_ADDR + uRegisterOffset);
         uVirtualWidth   = HWIO_GETVAL(MDP_VP_0_VIG_0_SSPP_OUT_SIZE,
                                       uRegVal,
                                       DST_W);
         uVirtualHeight  = HWIO_GETVAL(MDP_VP_0_VIG_0_SSPP_OUT_SIZE,
                                       uRegVal,
                                       DST_H);
         
       }
       else 
       {
         // eVirtualSourcePipe is REC0
         uRegisterOffset = HAL_MDP_Get_SSPP_BaseOffset(eVirtualSourcePipe);

         uRegVal         = in_dword(HWIO_MDP_VP_0_VIG_0_SSPP_OUT_XY_ADDR + uRegisterOffset);
         uVirtualPosX    = HWIO_GETVAL(MDP_VP_0_VIG_0_SSPP_OUT_XY,
                                       uRegVal,
                                       DST_X);
         uVirtualPosY    = HWIO_GETVAL(MDP_VP_0_VIG_0_SSPP_OUT_XY,
                                       uRegVal,
                                       DST_Y);

         uRegVal         = in_dword(HWIO_MDP_VP_0_VIG_0_SSPP_OUT_SIZE_ADDR + uRegisterOffset);
         uVirtualWidth   = HWIO_GETVAL(MDP_VP_0_VIG_0_SSPP_OUT_SIZE,
                                       uRegVal,
                                       DST_W);
         uVirtualHeight  = HWIO_GETVAL(MDP_VP_0_VIG_0_SSPP_OUT_SIZE,
                                       uRegVal,
                                       DST_H);
       }

       // Calculate the multi-rect mode type
       // Note: assumption is that upper layer has done the validation of the ROIs
       if (uPosY < uVirtualPosY)
       {
         // If the vertical distance between the ROIs > uMinTimeMultiplexModeLines, then assume time-multiplex
         if ((uPosY + uHeight + pgsMdpHwInfo->uMinTimeMultiplexModeLines) < uVirtualPosY)
         {
           psMultiRectInfo->eMultiRectType = HAL_MDP_MULTIRECT_TIME_MULTIPLEX_FETCH;
         }
         // Else assume parallel fetch
         else
         {
           psMultiRectInfo->eMultiRectType = HAL_MDP_MULTIRECT_PARALLEL_FETCH;
         }
       }
       else
       {
         // If the vertical distance between the ROIs > uMinTimeMultiplexModeLines, then assume time-multiplex
         if ((uVirtualPosY + uVirtualHeight + pgsMdpHwInfo->uMinTimeMultiplexModeLines) < uPosY)
         {
           psMultiRectInfo->eMultiRectType = HAL_MDP_MULTIRECT_TIME_MULTIPLEX_FETCH;
         }
         // Else assume parallel fetch
         else
         {
           psMultiRectInfo->eMultiRectType = HAL_MDP_MULTIRECT_PARALLEL_FETCH;
         }
       }
     }
     else
     {
       // If virtual pipe is not enabled, default to time-multiplex
       // This ensures that the current rect uses full PD_MEM
       psMultiRectInfo->eMultiRectType = HAL_MDP_MULTIRECT_TIME_MULTIPLEX_FETCH;
     }     
   }
   
   // Extract the base address to program the HW registers
   uRegisterOffset      = HAL_MDP_Get_SSPP_BaseOffset(eSourcePipe);

   uMultiRectOpModeInfo = in_dword(HWIO_MDP_VP_0_VIG_0_SSPP_MULTI_REC_OP_MODE_ADDR + uRegisterOffset);
   uMultiRecEnableMask  = HWIO_GETVAL(MDP_VP_0_VIG_0_SSPP_MULTI_REC_OP_MODE,
                                      uMultiRectOpModeInfo,
                                      MULTI_REC_ENABLE);

   uMultiRectOpModeInfo = HWIO_OUT_FLD(uMultiRectOpModeInfo,
                                     MDP_VP_0_VIG_0_SSPP_MULTI_REC_OP_MODE,
                                     MULTI_REC_MODE,
                                     psMultiRectInfo->eMultiRectType);

   // If uMultiRecEnableMask = b01, means REC0 is enabled
   // If uMultiRecEnableMask = b10, means REC1 is enabled
   // If uMultiRecEnableMask = b11, means both REC0 and REC1 are enabled
   if (psMultiRectInfo->bEnable)
   {
     uMultiRecEnableMask |= (1 << uSourceRecID);
   }
   else
   {
     uMultiRecEnableMask &= ~(1 << uSourceRecID);
   }
    
   uMultiRectOpModeInfo = HWIO_OUT_FLD(uMultiRectOpModeInfo,
                                  MDP_VP_0_VIG_0_SSPP_MULTI_REC_OP_MODE,
                                  MULTI_REC_ENABLE,
                                  uMultiRecEnableMask);

   out_dword(HWIO_MDP_VP_0_VIG_0_SSPP_MULTI_REC_OP_MODE_ADDR + uRegisterOffset, uMultiRectOpModeInfo);  // Update fetch information
}

/****************************************************************************
*
** FUNCTION: HAL_MDP_SSPP_DangerSafe_Init()
*/
/*!
* \brief
*     Initializes the danger-safe LUT for a given pipe
*
* \param [in] eSourcePipe           - Source Pipe ID
* \param [in] uFlags                - Reserved
*
* \retval NONE
*
****************************************************************************/
static void HAL_MDP_SSPP_DangerSafe_Init(HAL_MDP_SourcePipeId    eSourcePipe,
                                         uint32                  uFlags)
{
   if (NULL!= pgsMdpHwInfo->psSSPPQoSInfo)
   {
      uintPtr        uRegisterOffset = 0x0;

      // Extract the SSPP base address
      uRegisterOffset = HAL_MDP_Get_SSPP_BaseOffset(eSourcePipe);

      out_dword(HWIO_MDP_VP_0_VIG_0_SSPP_DANGER_LUT_ADDR + uRegisterOffset, pgsMdpHwInfo->psSSPPQoSInfo->sLinearQoSInfo.uDangerLutValue);
      out_dword(HWIO_MDP_VP_0_VIG_0_SSPP_SAFE_LUT_ADDR   + uRegisterOffset, pgsMdpHwInfo->psSSPPQoSInfo->sLinearQoSInfo.uSafeLutValue);
   }
}

/* -----------------------------------------------------------------------
 * Externalized Function Definitions
 * ----------------------------------------------------------------------- */

/****************************************************************************
*
** FUNCTION: HAL_MDP_SSPP_Init()
*/
/*!
* \brief
*     Initializes the MDP SSPP block
*
* \param [in] eSourcePipe           - Source Pipe ID
* \param [in] eLayerType            - Layer Type (RGB, VIG, DMA, or CURSOR etc)
* \param [in] uInitFlags            - Internal modules to be initialized
* \param [in] uFlags                - Reserved
*
* \retval NONE
*
****************************************************************************/
void HAL_MDP_SSPP_Init(HAL_MDP_SourcePipeId    eSourcePipe, 
                       HAL_MDP_LayerType       eLayerType,
                       uint32                  uInitFlags,
                       uint32                  uFlags)
{
   if(HAL_MDSS_STATUS_SUCCESS == HAL_MDP_ValidateSourcePipe(eSourcePipe, eLayerType))
   {
      if(0x00 != (HAL_MDP_SSPP_FEATURE_IGC_LUT & uInitFlags))
      {
         /* To initialize the layer's inverse gamma tables with default inverse gamma values call 
            HAL_MDP_SSPP_IGCConfig() at this point with HAL_MDP_IGCConfigType structure populated  
            with the default inverse gamma table values. */
 
      }
 
      if(0x00 != (HAL_MDP_SSPP_FEATURE_CSC_1 & uInitFlags))
      {
         HAL_MDP_SSPP_CSC_1_Init(eSourcePipe, eLayerType, HAL_MDP_COLOR_ENCODING_REC601, uFlags);
      }
 
      if(0x00 != (HAL_MDP_SSPP_FEATURE_QSEED2 & uInitFlags))
      {
         HAL_MDP_SSPP_QSEED_Init(eSourcePipe, eLayerType, uFlags);
      }

      /* Initialize danger safe settings here */
      if (TRUE == pgsMdpHwInfo->bSSPPDangerSafe)
      {
         HAL_MDP_SSPP_DangerSafe_Init(eSourcePipe, 0);
      }
   }
}

/****************************************************************************
*
** FUNCTION: HAL_MDP_SourcePipe_Setup()
*/
/*!
* \brief
*     Set up configurations for a particular source pipe
*
* \param [in]   eSourcePipe         - Source Pipe ID
* \param [in]   eLayerType          - Layer Type (RGB, VIG, DMA, or CURSOR etc)
* \param [in]   psSourcePipeConfig  - Configuration for the selected pipe
* \param [in]   uSetupFlags         - Reserved
*
* \retval HAL_MDSS_ErrorType
*
****************************************************************************/
    HAL_MDSS_ErrorType HAL_MDP_SourcePipe_Setup(HAL_MDP_SourcePipeId            eSourcePipe,
                                            HAL_MDP_LayerType               eLayerType,
                                            HAL_MDP_SourcePipeConfigType   *psSourcePipeConfig,
                                            uint32                          uSetupFlags)
{
   HAL_MDSS_ErrorType         eStatus        = HAL_MDSS_STATUS_SUCCESS;

   // Validate source pipe and do the configuration, if it is a valid layer
   if(HAL_MDSS_STATUS_SUCCESS == (eStatus = HAL_MDP_ValidateSourcePipe(eSourcePipe, eLayerType)))
   {
      HAL_MDP_CropRectangleType  sCropRectInfo;
      HAL_MDP_SurfaceAttrType    sSurfInfo;

      MDSS_OSAL_MEMZERO(&sCropRectInfo, sizeof(sCropRectInfo));
      MDSS_OSAL_MEMZERO(&sSurfInfo, sizeof(sSurfInfo));  
      
      if(HAL_MDP_LAYER_TYPE_CURSOR == eLayerType)     // Override the the type with HAL driver internal cursor type
      {
         eLayerType  = pgsMdpHwInfo->eCursorIntLayerType;
      }
 
      /**If disable HsyncFlip is requested:then it has be done before SSPP_Source (HAL_MDP_SSPP_SurfaceConfig),otherwise it may lead to SMMU fault*/
      if(NULL != psSourcePipeConfig->psImmediateFlip)
      {
         HAL_MDP_SSPP_ImmediateFlipConfig(eSourcePipe, eLayerType, psSourcePipeConfig->psImmediateFlip, HAL_MDP_SOURCE_FLAGS_PRE_SURFACE_CONFIG);
      }
 
      /** Surface Configuration */
      if(NULL != psSourcePipeConfig->psSurfaceInfo)
      {
         HAL_MDP_SSPP_SurfaceConfig(eSourcePipe, eLayerType, psSourcePipeConfig->psSurfaceInfo, 0x00);
      }
 
      /** Cropping rectangle configuration */
      if(NULL != psSourcePipeConfig->psCropRectInfo)
      {
         HAL_MDP_SSPP_CropRectConfig(eSourcePipe, eLayerType, psSourcePipeConfig->psCropRectInfo, 0x00);
      }
 
      // Scalar configuration
      if((NULL != psSourcePipeConfig->psSurfaceInfo) || (NULL != psSourcePipeConfig->psCropRectInfo))
      {
         // Extract both surface and cropping rectangle information
         HAL_MDP_SSPP_SurfaceInfo(eSourcePipe,
                                  eLayerType,
                                  &sSurfInfo,
                                  &sCropRectInfo,
                                  HAL_MDP_SOURCE_FLAGS_EXTRACT_ONLY_PIXEL_FORMAT);


         // Enable Pixel Extension Software Override, Setup PE, Scaler and Decimator 
         HAL_MDP_PixelExtensionConfig(eSourcePipe,
                                      eLayerType,
                                      psSourcePipeConfig->psPixelExtLayerConfig);
     }

      /** Color fill configuration */
      if(NULL != psSourcePipeConfig->psColorFillInfo)
      {
         HAL_MDP_SSPP_ColorfillConfig(eSourcePipe, eLayerType, psSourcePipeConfig->psColorFillInfo, 0x00);
      }
 
      /** CSC configuration */
      if(NULL != psSourcePipeConfig->psColorEncInfo)
      {
         HAL_MDP_SSPP_ColorEncodeConfig(eSourcePipe, eLayerType, psSourcePipeConfig->psColorEncInfo, 0x00);
      }
 
      /** Fetch configuration */
      if(NULL != psSourcePipeConfig->psFetchInfo)
      {
         HAL_MDP_SSPP_FetchModeConfig(eSourcePipe, eLayerType, psSourcePipeConfig->psFetchInfo, 0x00);
      }
 
      /** IGC configuration */
      if(NULL != psSourcePipeConfig->psIGCInfo)
      {
         HAL_MDP_SSPP_IGCConfig(eSourcePipe, eLayerType, psSourcePipeConfig->psIGCInfo, 0x00);
      }
 
 
      /** Sharpening Mode Configuration */
      if(NULL != psSourcePipeConfig->psHistogramConfig)
      {
         HAL_MDP_SSPP_HistogramConfig(eSourcePipe, eLayerType, psSourcePipeConfig->psHistogramConfig, 0x00);
      }
 
      /** Picture Adjust Configuration */
      if(NULL != psSourcePipeConfig->psPictureAdjust)
      {
         HAL_MDP_SSPP_PictureAdjustmentConfig(eSourcePipe, eLayerType, psSourcePipeConfig->psPictureAdjust, 0x00);
      }
 
      /** Immediate Flip Configuration: If you need to enable the HsyncFlip: Do it after you config the SSPP_Source (HAL_MDP_SSPP_SurfaceConfig) */
      if(NULL != psSourcePipeConfig->psImmediateFlip)
      {
         HAL_MDP_SSPP_ImmediateFlipConfig(eSourcePipe, eLayerType, psSourcePipeConfig->psImmediateFlip, 0x00);
      }
 
      /** VC1 Range Level Mapping Configuration */
      if(NULL != psSourcePipeConfig->psVC1RangeInfo)
      {
         HAL_MDP_SSPP_VC1RangeLevelConfig(eSourcePipe, eLayerType, psSourcePipeConfig->psVC1RangeInfo, 0x00);
      }
 
      /** Gpu highest memory bank bit configuration */
      if(NULL != psSourcePipeConfig->psGpuMemoryBankConfig)
      {
        HAL_MDP_SSPP_GpuMemoryBankBitConfig(eSourcePipe, eLayerType, psSourcePipeConfig->psGpuMemoryBankConfig, 0x00);
      }

      /** Multi-Rect configuration */
      if(NULL != psSourcePipeConfig->psMultiRectConfig)
      {
         HAL_MDP_SSPP_MultiRectConfig(eSourcePipe, eLayerType, psSourcePipeConfig->psMultiRectConfig, 0x00);
      }
   }

   return eStatus;
}

/****************************************************************************
*
** FUNCTION: HAL_MDP_SourcePipe_GetProperty()
*/
/*!
* \brief
*     Returns MDP source pipe property
*
* \param [in]   eSourcePipe         - Source Pipe ID
* \param [in]   eLayerType          - Layer Type (RGB, VIG, DMA, or CURSOR etc)
* \param [out]  psSourcePipeInfo    - Source pipe information
* \param [in]   uFlags              - Reserved
*
* \retval HAL_MDSS_ErrorType
*
****************************************************************************/
HAL_MDSS_ErrorType HAL_MDP_SourcePipe_GetProperty(HAL_MDP_SourcePipeId          eSourcePipe,
                                                  HAL_MDP_LayerType             eLayerType,
                                                  HAL_MDP_SourcePipeInfoType    *psSourcePipeInfo,
                                                  uint32                        uFlags)
{
   HAL_MDSS_ErrorType      eStatus = HAL_MDSS_STATUS_SUCCESS;

   // Validate source pipe and do the configuration, if it is a valid layer
   if(HAL_MDSS_STATUS_SUCCESS == (eStatus = HAL_MDP_ValidateSourcePipe(eSourcePipe, eLayerType)))
   {
      if(HAL_MDP_LAYER_TYPE_CURSOR == eLayerType)     // Override the the type with HAL driver internal cursor type
      {
         eLayerType  = pgsMdpHwInfo->eCursorIntLayerType;
      }
 
      if(NULL != psSourcePipeInfo->psSurfaceInfo)
      {
         HAL_MDP_SSPP_SurfaceInfo(eSourcePipe, eLayerType, psSourcePipeInfo->psSurfaceInfo, NULL, 0x00);
      }
 
      // Module state
      if(NULL != psSourcePipeInfo->pbActive)
      {
         HAL_MDP_SSPP_GetStateInfo(eSourcePipe, eLayerType, psSourcePipeInfo->pbActive, 0x00);
      }
   }

   return eStatus;
}

#ifdef __cplusplus
}
#endif
