/*! \file */

/*
===========================================================================

FILE:         hal_mdp_format_info.c

DESCRIPTION: 
  

===========================================================================
  Copyright (c) 2010-2017 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
===========================================================================
*/
/*------------------------------------------------------------------------------
 * Include Files
 *----------------------------------------------------------------------------*/

#include "hal_mdp_i.h"
/* !! ALERT !!
 * DO NOT INCLUDE ANY HEADER FILES OTHER THAN hal_mdp_i.h
 */

#ifdef __cplusplus
extern "C" {
#endif

#define HAL_MDP_PREPARE_SRC_FORMAT_INFO(eFrameFormat,      \
                                        eChromaSample,     \
                                        uNumOfPlanes,      \
                                        bPackAlign,        \
                                        bPackTight,        \
                                        uUnPackCount,      \
                                        uBytesPerPixel,    \
                                        bAlphaPresent,     \
                                        uBitsPerColor3,    \
                                        uBitsPerColor2,    \
                                        uBitsPerColor1,    \
                                        uBitsPerColor0,    \
                                        uDxFormat    \
                                        )    \
                                          eFrameFormat   << HWIO_MDP_VP_0_VIG_0_SSPP_SRC_FORMAT_FRAME_FORMAT_SHFT    |     \
                                          eChromaSample  << HWIO_MDP_VP_0_VIG_0_SSPP_SRC_FORMAT_SRC_CHROMA_SAMP_SHFT |     \
                                          uNumOfPlanes   << HWIO_MDP_VP_0_VIG_0_SSPP_SRC_FORMAT_FETCH_PLANES_SHFT    |     \
                                          bPackAlign     << HWIO_MDP_VP_0_VIG_0_SSPP_SRC_FORMAT_UNPACK_ALIGN_SHFT    |     \
                                          bPackTight     << HWIO_MDP_VP_0_VIG_0_SSPP_SRC_FORMAT_UNPACK_TIGHT_SHFT    |     \
                                          uUnPackCount   << HWIO_MDP_VP_0_VIG_0_SSPP_SRC_FORMAT_UNPACK_COUNT_SHFT    |     \
                                          uBytesPerPixel << HWIO_MDP_VP_0_VIG_0_SSPP_SRC_FORMAT_SRC_BPP_SHFT         |     \
                                          bAlphaPresent  << HWIO_MDP_VP_0_VIG_0_SSPP_SRC_FORMAT_SRCC3_EN_SHFT        |     \
                                          uBitsPerColor3 << HWIO_MDP_VP_0_VIG_0_SSPP_SRC_FORMAT_SRCC3_SHFT           |     \
                                          uBitsPerColor2 << HWIO_MDP_VP_0_VIG_0_SSPP_SRC_FORMAT_SRCC2_SHFT           |     \
                                          uBitsPerColor1 << HWIO_MDP_VP_0_VIG_0_SSPP_SRC_FORMAT_SRCC1_SHFT           |     \
                                          uBitsPerColor0 << HWIO_MDP_VP_0_VIG_0_SSPP_SRC_FORMAT_SRCC0_SHFT           |     \
                                          uDxFormat      << HWIO_MDP_VP_0_VIG_0_SSPP_SRC_FORMAT_UNPACK_DX_FORMAT_SHFT

uint32 guSrcFormatInfo[HAL_MDP_PIXEL_FORMAT_MAX] = 
{
                                                                     // HAL_MDP_PIXEL_FORMAT_NONE
   0x00000000,

   /* Interleaved Pixel Formats                                      HAL_MDP_PIXEL_FORMAT_INTERLEAVED_BASE */

                                                                     // HAL_MDP_PIXEL_FORMAT_RGB_565_16BPP
   HAL_MDP_PREPARE_SRC_FORMAT_INFO(HAL_MDP_SRC_FORMAT_FRAME_FORMAT_LINEAR,
                                   HAL_MDP_SRC_FORMAT_CHROMA_SAMPLE_RGB,
                                   HAL_MDP_SRC_FORMAT_FETCH_PLANES_INTERLEAVED,
                                   HAL_MDP_SRC_FORMAT_UNPACK_ALIGN_TO_LSB,
                                   HAL_MDP_SRC_FORMAT_UNPACK_TIGHT,
                                   HAL_MDP_SRC_FORAMT_UNPACK_COMPONENT_COUNT3,
                                   HAL_MDP_SRC_FORMAT_SRC_TWO_BPP,
                                   HAL_MDP_SRC_FORMAT_ALPHA_NOT_PRESENT,
                                   HAL_MDP_SRC_FORMAT_ALPHA_COMP_BITS_0,
                                   HAL_MDP_SRC_FORMAT_RED_COMP_BITS_5,
                                   HAL_MDP_SRC_FORMAT_BLUE_COMP_BITS_5,
                                   HAL_MDP_SRC_FORMAT_GREEN_COMP_BITS_6,
                                   HAL_MDP_SRC_FORMAT_DX_FORMAT_OFF),

                                                                     // HAL_MDP_PIXEL_FORMAT_RGB_666_18BPP
   HAL_MDP_PREPARE_SRC_FORMAT_INFO(HAL_MDP_SRC_FORMAT_FRAME_FORMAT_LINEAR,
                                   HAL_MDP_SRC_FORMAT_CHROMA_SAMPLE_RGB,
                                   HAL_MDP_SRC_FORMAT_FETCH_PLANES_INTERLEAVED,
                                   HAL_MDP_SRC_FORMAT_UNPACK_ALIGN_TO_LSB,
                                   HAL_MDP_SRC_FORMAT_UNPACK_TIGHT,
                                   HAL_MDP_SRC_FORAMT_UNPACK_COMPONENT_COUNT3,
                                   HAL_MDP_SRC_FORMAT_SRC_THREE_BPP,
                                   HAL_MDP_SRC_FORMAT_ALPHA_NOT_PRESENT,
                                   HAL_MDP_SRC_FORMAT_ALPHA_COMP_BITS_0,
                                   HAL_MDP_SRC_FORMAT_RED_COMP_BITS_6,
                                   HAL_MDP_SRC_FORMAT_BLUE_COMP_BITS_6,
                                   HAL_MDP_SRC_FORMAT_GREEN_COMP_BITS_6,
                                   HAL_MDP_SRC_FORMAT_DX_FORMAT_OFF),

                                                                     // HAL_MDP_PIXEL_FORMAT_RGB_888_24BPP
   HAL_MDP_PREPARE_SRC_FORMAT_INFO(HAL_MDP_SRC_FORMAT_FRAME_FORMAT_LINEAR,
                                   HAL_MDP_SRC_FORMAT_CHROMA_SAMPLE_RGB,
                                   HAL_MDP_SRC_FORMAT_FETCH_PLANES_INTERLEAVED,
                                   HAL_MDP_SRC_FORMAT_UNPACK_ALIGN_TO_LSB,
                                   HAL_MDP_SRC_FORMAT_UNPACK_TIGHT,
                                   HAL_MDP_SRC_FORAMT_UNPACK_COMPONENT_COUNT3,
                                   HAL_MDP_SRC_FORMAT_SRC_THREE_BPP,
                                   HAL_MDP_SRC_FORMAT_ALPHA_NOT_PRESENT,
                                   HAL_MDP_SRC_FORMAT_ALPHA_COMP_BITS_0,
                                   HAL_MDP_SRC_FORMAT_RED_COMP_BITS_8,
                                   HAL_MDP_SRC_FORMAT_BLUE_COMP_BITS_8,
                                   HAL_MDP_SRC_FORMAT_GREEN_COMP_BITS_8,
                                   HAL_MDP_SRC_FORMAT_DX_FORMAT_OFF),

                                                                     // HAL_MDP_PIXEL_FORMAT_ARGB_1555_16BPP
   HAL_MDP_PREPARE_SRC_FORMAT_INFO(HAL_MDP_SRC_FORMAT_FRAME_FORMAT_LINEAR,
                                   HAL_MDP_SRC_FORMAT_CHROMA_SAMPLE_RGB,
                                   HAL_MDP_SRC_FORMAT_FETCH_PLANES_INTERLEAVED,
                                   HAL_MDP_SRC_FORMAT_UNPACK_ALIGN_TO_LSB,
                                   HAL_MDP_SRC_FORMAT_UNPACK_TIGHT,
                                   HAL_MDP_SRC_FORAMT_UNPACK_COMPONENT_COUNT4,
                                   HAL_MDP_SRC_FORMAT_SRC_TWO_BPP,
                                   HAL_MDP_SRC_FORMAT_ALPHA_PRESENT,
                                   HAL_MDP_SRC_FORMAT_ALPHA_COMP_BITS_1,
                                   HAL_MDP_SRC_FORMAT_RED_COMP_BITS_5,
                                   HAL_MDP_SRC_FORMAT_BLUE_COMP_BITS_5,
                                   HAL_MDP_SRC_FORMAT_GREEN_COMP_BITS_5,
                                   HAL_MDP_SRC_FORMAT_DX_FORMAT_OFF),

                                                                     // HAL_MDP_PIXEL_FORMAT_XRGB_8888_32BPP
   HAL_MDP_PREPARE_SRC_FORMAT_INFO(HAL_MDP_SRC_FORMAT_FRAME_FORMAT_LINEAR,
                                   HAL_MDP_SRC_FORMAT_CHROMA_SAMPLE_RGB,
                                   HAL_MDP_SRC_FORMAT_FETCH_PLANES_INTERLEAVED,
                                   HAL_MDP_SRC_FORMAT_UNPACK_ALIGN_TO_LSB,
                                   HAL_MDP_SRC_FORMAT_UNPACK_TIGHT,
                                   HAL_MDP_SRC_FORAMT_UNPACK_COMPONENT_COUNT4,
                                   HAL_MDP_SRC_FORMAT_SRC_FOUR_BPP,
                                   HAL_MDP_SRC_FORMAT_ALPHA_PRESENT,
                                   HAL_MDP_SRC_FORMAT_ALPHA_COMP_BITS_8,
                                   HAL_MDP_SRC_FORMAT_RED_COMP_BITS_8,
                                   HAL_MDP_SRC_FORMAT_BLUE_COMP_BITS_8,
                                   HAL_MDP_SRC_FORMAT_GREEN_COMP_BITS_8,
                                   HAL_MDP_SRC_FORMAT_DX_FORMAT_OFF),

                                                                     // HAL_MDP_PIXEL_FORMAT_ARGB_8888_32BPP
   HAL_MDP_PREPARE_SRC_FORMAT_INFO(HAL_MDP_SRC_FORMAT_FRAME_FORMAT_LINEAR,
                                   HAL_MDP_SRC_FORMAT_CHROMA_SAMPLE_RGB,
                                   HAL_MDP_SRC_FORMAT_FETCH_PLANES_INTERLEAVED,
                                   HAL_MDP_SRC_FORMAT_UNPACK_ALIGN_TO_LSB,
                                   HAL_MDP_SRC_FORMAT_UNPACK_TIGHT,
                                   HAL_MDP_SRC_FORAMT_UNPACK_COMPONENT_COUNT4,
                                   HAL_MDP_SRC_FORMAT_SRC_FOUR_BPP,
                                   HAL_MDP_SRC_FORMAT_ALPHA_PRESENT,
                                   HAL_MDP_SRC_FORMAT_ALPHA_COMP_BITS_8,
                                   HAL_MDP_SRC_FORMAT_RED_COMP_BITS_8,
                                   HAL_MDP_SRC_FORMAT_BLUE_COMP_BITS_8,
                                   HAL_MDP_SRC_FORMAT_GREEN_COMP_BITS_8,
                                   HAL_MDP_SRC_FORMAT_DX_FORMAT_OFF),

                                                                     // HAL_MDP_PIXEL_FORMAT_BGR_565_16BPP
   HAL_MDP_PREPARE_SRC_FORMAT_INFO(HAL_MDP_SRC_FORMAT_FRAME_FORMAT_LINEAR,
                                   HAL_MDP_SRC_FORMAT_CHROMA_SAMPLE_RGB,
                                   HAL_MDP_SRC_FORMAT_FETCH_PLANES_INTERLEAVED,
                                   HAL_MDP_SRC_FORMAT_UNPACK_ALIGN_TO_LSB,
                                   HAL_MDP_SRC_FORMAT_UNPACK_TIGHT,
                                   HAL_MDP_SRC_FORAMT_UNPACK_COMPONENT_COUNT3,
                                   HAL_MDP_SRC_FORMAT_SRC_TWO_BPP,
                                   HAL_MDP_SRC_FORMAT_ALPHA_NOT_PRESENT,
                                   HAL_MDP_SRC_FORMAT_ALPHA_COMP_BITS_0,
                                   HAL_MDP_SRC_FORMAT_RED_COMP_BITS_5,
                                   HAL_MDP_SRC_FORMAT_BLUE_COMP_BITS_5,
                                   HAL_MDP_SRC_FORMAT_GREEN_COMP_BITS_6,
                                   HAL_MDP_SRC_FORMAT_DX_FORMAT_OFF),

                                                                     // HAL_MDP_PIXEL_FORMAT_BGR_888_24BPP
   HAL_MDP_PREPARE_SRC_FORMAT_INFO(HAL_MDP_SRC_FORMAT_FRAME_FORMAT_LINEAR,
                                   HAL_MDP_SRC_FORMAT_CHROMA_SAMPLE_RGB,
                                   HAL_MDP_SRC_FORMAT_FETCH_PLANES_INTERLEAVED,
                                   HAL_MDP_SRC_FORMAT_UNPACK_ALIGN_TO_LSB,
                                   HAL_MDP_SRC_FORMAT_UNPACK_TIGHT,
                                   HAL_MDP_SRC_FORAMT_UNPACK_COMPONENT_COUNT3,
                                   HAL_MDP_SRC_FORMAT_SRC_THREE_BPP,
                                   HAL_MDP_SRC_FORMAT_ALPHA_NOT_PRESENT,
                                   HAL_MDP_SRC_FORMAT_ALPHA_COMP_BITS_0,
                                   HAL_MDP_SRC_FORMAT_RED_COMP_BITS_8,
                                   HAL_MDP_SRC_FORMAT_BLUE_COMP_BITS_8,
                                   HAL_MDP_SRC_FORMAT_GREEN_COMP_BITS_8,
                                   HAL_MDP_SRC_FORMAT_DX_FORMAT_OFF),

                                                                     // HAL_MDP_PIXEL_FORMAT_ABGR_1555_16BPP
   HAL_MDP_PREPARE_SRC_FORMAT_INFO(HAL_MDP_SRC_FORMAT_FRAME_FORMAT_LINEAR,
                                   HAL_MDP_SRC_FORMAT_CHROMA_SAMPLE_RGB,
                                   HAL_MDP_SRC_FORMAT_FETCH_PLANES_INTERLEAVED,
                                   HAL_MDP_SRC_FORMAT_UNPACK_ALIGN_TO_LSB,
                                   HAL_MDP_SRC_FORMAT_UNPACK_TIGHT,
                                   HAL_MDP_SRC_FORAMT_UNPACK_COMPONENT_COUNT4,
                                   HAL_MDP_SRC_FORMAT_SRC_TWO_BPP,
                                   HAL_MDP_SRC_FORMAT_ALPHA_PRESENT,
                                   HAL_MDP_SRC_FORMAT_ALPHA_COMP_BITS_1,
                                   HAL_MDP_SRC_FORMAT_RED_COMP_BITS_5,
                                   HAL_MDP_SRC_FORMAT_BLUE_COMP_BITS_5,
                                   HAL_MDP_SRC_FORMAT_GREEN_COMP_BITS_5,
                                   HAL_MDP_SRC_FORMAT_DX_FORMAT_OFF),

                                                                     // HAL_MDP_PIXEL_FORMAT_XBGR_8888_32BPP
   HAL_MDP_PREPARE_SRC_FORMAT_INFO(HAL_MDP_SRC_FORMAT_FRAME_FORMAT_LINEAR,
                                   HAL_MDP_SRC_FORMAT_CHROMA_SAMPLE_RGB,
                                   HAL_MDP_SRC_FORMAT_FETCH_PLANES_INTERLEAVED,
                                   HAL_MDP_SRC_FORMAT_UNPACK_ALIGN_TO_LSB,
                                   HAL_MDP_SRC_FORMAT_UNPACK_TIGHT,
                                   HAL_MDP_SRC_FORAMT_UNPACK_COMPONENT_COUNT4,
                                   HAL_MDP_SRC_FORMAT_SRC_FOUR_BPP,
                                   HAL_MDP_SRC_FORMAT_ALPHA_PRESENT,
                                   HAL_MDP_SRC_FORMAT_ALPHA_COMP_BITS_8,
                                   HAL_MDP_SRC_FORMAT_RED_COMP_BITS_8,
                                   HAL_MDP_SRC_FORMAT_BLUE_COMP_BITS_8,
                                   HAL_MDP_SRC_FORMAT_GREEN_COMP_BITS_8,
                                   HAL_MDP_SRC_FORMAT_DX_FORMAT_OFF),

                                                                     // HAL_MDP_PIXEL_FORMAT_ABGR_8888_32BPP
   HAL_MDP_PREPARE_SRC_FORMAT_INFO(HAL_MDP_SRC_FORMAT_FRAME_FORMAT_LINEAR,
                                   HAL_MDP_SRC_FORMAT_CHROMA_SAMPLE_RGB,
                                   HAL_MDP_SRC_FORMAT_FETCH_PLANES_INTERLEAVED,
                                   HAL_MDP_SRC_FORMAT_UNPACK_ALIGN_TO_LSB,
                                   HAL_MDP_SRC_FORMAT_UNPACK_TIGHT,
                                   HAL_MDP_SRC_FORAMT_UNPACK_COMPONENT_COUNT4,
                                   HAL_MDP_SRC_FORMAT_SRC_FOUR_BPP,
                                   HAL_MDP_SRC_FORMAT_ALPHA_PRESENT,
                                   HAL_MDP_SRC_FORMAT_ALPHA_COMP_BITS_8,
                                   HAL_MDP_SRC_FORMAT_RED_COMP_BITS_8,
                                   HAL_MDP_SRC_FORMAT_BLUE_COMP_BITS_8,
                                   HAL_MDP_SRC_FORMAT_GREEN_COMP_BITS_8,
                                   HAL_MDP_SRC_FORMAT_DX_FORMAT_OFF),

                                                                     // HAL_MDP_PIXEL_FORMAT_RGBA_5551_16BPP
   HAL_MDP_PREPARE_SRC_FORMAT_INFO(HAL_MDP_SRC_FORMAT_FRAME_FORMAT_LINEAR,
                                   HAL_MDP_SRC_FORMAT_CHROMA_SAMPLE_RGB,
                                   HAL_MDP_SRC_FORMAT_FETCH_PLANES_INTERLEAVED,
                                   HAL_MDP_SRC_FORMAT_UNPACK_ALIGN_TO_LSB,
                                   HAL_MDP_SRC_FORMAT_UNPACK_TIGHT,
                                   HAL_MDP_SRC_FORAMT_UNPACK_COMPONENT_COUNT4,
                                   HAL_MDP_SRC_FORMAT_SRC_TWO_BPP,
                                   HAL_MDP_SRC_FORMAT_ALPHA_PRESENT,
                                   HAL_MDP_SRC_FORMAT_ALPHA_COMP_BITS_1,
                                   HAL_MDP_SRC_FORMAT_RED_COMP_BITS_5,
                                   HAL_MDP_SRC_FORMAT_BLUE_COMP_BITS_5,
                                   HAL_MDP_SRC_FORMAT_GREEN_COMP_BITS_5,
                                   HAL_MDP_SRC_FORMAT_DX_FORMAT_OFF),

                                                                     // HAL_MDP_PIXEL_FORMAT_RGBA_8888_32BPP
   HAL_MDP_PREPARE_SRC_FORMAT_INFO(HAL_MDP_SRC_FORMAT_FRAME_FORMAT_LINEAR,
                                   HAL_MDP_SRC_FORMAT_CHROMA_SAMPLE_RGB,
                                   HAL_MDP_SRC_FORMAT_FETCH_PLANES_INTERLEAVED,
                                   HAL_MDP_SRC_FORMAT_UNPACK_ALIGN_TO_LSB,
                                   HAL_MDP_SRC_FORMAT_UNPACK_TIGHT,
                                   HAL_MDP_SRC_FORAMT_UNPACK_COMPONENT_COUNT4,
                                   HAL_MDP_SRC_FORMAT_SRC_FOUR_BPP,
                                   HAL_MDP_SRC_FORMAT_ALPHA_PRESENT,
                                   HAL_MDP_SRC_FORMAT_ALPHA_COMP_BITS_8,
                                   HAL_MDP_SRC_FORMAT_RED_COMP_BITS_8,
                                   HAL_MDP_SRC_FORMAT_BLUE_COMP_BITS_8,
                                   HAL_MDP_SRC_FORMAT_GREEN_COMP_BITS_8,
                                   HAL_MDP_SRC_FORMAT_DX_FORMAT_OFF),

                                                                     // HAL_MDP_PIXEL_FORMAT_RGB_OXILI_TILE_4x4_24BPP
   0x00000000,

                                                                     // HAL_MDP_PIXEL_FORMAT_ARGB_4444_16BPP
   HAL_MDP_PREPARE_SRC_FORMAT_INFO(HAL_MDP_SRC_FORMAT_FRAME_FORMAT_LINEAR,
                                   HAL_MDP_SRC_FORMAT_CHROMA_SAMPLE_RGB,
                                   HAL_MDP_SRC_FORMAT_FETCH_PLANES_INTERLEAVED,
                                   HAL_MDP_SRC_FORMAT_UNPACK_ALIGN_TO_LSB,
                                   HAL_MDP_SRC_FORMAT_UNPACK_TIGHT,
                                   HAL_MDP_SRC_FORAMT_UNPACK_COMPONENT_COUNT4,
                                   HAL_MDP_SRC_FORMAT_SRC_TWO_BPP,
                                   HAL_MDP_SRC_FORMAT_ALPHA_PRESENT,
                                   HAL_MDP_SRC_FORMAT_ALPHA_COMP_BITS_4,
                                   HAL_MDP_SRC_FORMAT_RED_COMP_BITS_4,
                                   HAL_MDP_SRC_FORMAT_BLUE_COMP_BITS_4,
                                   HAL_MDP_SRC_FORMAT_GREEN_COMP_BITS_4,
                                   HAL_MDP_SRC_FORMAT_DX_FORMAT_OFF),

                                                                     // HAL_MDP_PIXEL_FORMAT_XRGB_2101010_32BPP
   HAL_MDP_PREPARE_SRC_FORMAT_INFO(HAL_MDP_SRC_FORMAT_FRAME_FORMAT_LINEAR,
                                   HAL_MDP_SRC_FORMAT_CHROMA_SAMPLE_RGB,
                                   HAL_MDP_SRC_FORMAT_FETCH_PLANES_INTERLEAVED,
                                   HAL_MDP_SRC_FORMAT_UNPACK_ALIGN_TO_LSB,
                                   HAL_MDP_SRC_FORMAT_UNPACK_TIGHT,
                                   HAL_MDP_SRC_FORAMT_UNPACK_COMPONENT_COUNT4,
                                   HAL_MDP_SRC_FORMAT_SRC_FOUR_BPP,
                                   HAL_MDP_SRC_FORMAT_ALPHA_PRESENT,
                                   HAL_MDP_SRC_FORMAT_ALPHA_COMP_BITS_8,
                                   HAL_MDP_SRC_FORMAT_RED_COMP_BITS_8,
                                   HAL_MDP_SRC_FORMAT_BLUE_COMP_BITS_8,
                                   HAL_MDP_SRC_FORMAT_GREEN_COMP_BITS_8,
                                   HAL_MDP_SRC_FORMAT_DX_FORMAT_ON),

                                                                     // HAL_MDP_PIXEL_FORMAT_ARGB_2101010_32BPP
   HAL_MDP_PREPARE_SRC_FORMAT_INFO(HAL_MDP_SRC_FORMAT_FRAME_FORMAT_LINEAR,
                                   HAL_MDP_SRC_FORMAT_CHROMA_SAMPLE_RGB,
                                   HAL_MDP_SRC_FORMAT_FETCH_PLANES_INTERLEAVED,
                                   HAL_MDP_SRC_FORMAT_UNPACK_ALIGN_TO_LSB,
                                   HAL_MDP_SRC_FORMAT_UNPACK_TIGHT,
                                   HAL_MDP_SRC_FORAMT_UNPACK_COMPONENT_COUNT4,
                                   HAL_MDP_SRC_FORMAT_SRC_FOUR_BPP,
                                   HAL_MDP_SRC_FORMAT_ALPHA_PRESENT,
                                   HAL_MDP_SRC_FORMAT_ALPHA_COMP_BITS_8,
                                   HAL_MDP_SRC_FORMAT_RED_COMP_BITS_8,
                                   HAL_MDP_SRC_FORMAT_BLUE_COMP_BITS_8,
                                   HAL_MDP_SRC_FORMAT_GREEN_COMP_BITS_8,
                                   HAL_MDP_SRC_FORMAT_DX_FORMAT_ON),

                                                                     // HAL_MDP_PIXEL_FORMAT_XBGR_2101010_32BPP
   HAL_MDP_PREPARE_SRC_FORMAT_INFO(HAL_MDP_SRC_FORMAT_FRAME_FORMAT_LINEAR,
                                   HAL_MDP_SRC_FORMAT_CHROMA_SAMPLE_RGB,
                                   HAL_MDP_SRC_FORMAT_FETCH_PLANES_INTERLEAVED,
                                   HAL_MDP_SRC_FORMAT_UNPACK_ALIGN_TO_LSB,
                                   HAL_MDP_SRC_FORMAT_UNPACK_TIGHT,
                                   HAL_MDP_SRC_FORAMT_UNPACK_COMPONENT_COUNT4,
                                   HAL_MDP_SRC_FORMAT_SRC_FOUR_BPP,
                                   HAL_MDP_SRC_FORMAT_ALPHA_PRESENT,
                                   HAL_MDP_SRC_FORMAT_ALPHA_COMP_BITS_8,
                                   HAL_MDP_SRC_FORMAT_RED_COMP_BITS_8,
                                   HAL_MDP_SRC_FORMAT_BLUE_COMP_BITS_8,
                                   HAL_MDP_SRC_FORMAT_GREEN_COMP_BITS_8,
                                   HAL_MDP_SRC_FORMAT_DX_FORMAT_ON),

                                                                     // HAL_MDP_PIXEL_FORMAT_ABGR_2101010_32BPP
   HAL_MDP_PREPARE_SRC_FORMAT_INFO(HAL_MDP_SRC_FORMAT_FRAME_FORMAT_LINEAR,
                                   HAL_MDP_SRC_FORMAT_CHROMA_SAMPLE_RGB,
                                   HAL_MDP_SRC_FORMAT_FETCH_PLANES_INTERLEAVED,
                                   HAL_MDP_SRC_FORMAT_UNPACK_ALIGN_TO_LSB,
                                   HAL_MDP_SRC_FORMAT_UNPACK_TIGHT,
                                   HAL_MDP_SRC_FORAMT_UNPACK_COMPONENT_COUNT4,
                                   HAL_MDP_SRC_FORMAT_SRC_FOUR_BPP,
                                   HAL_MDP_SRC_FORMAT_ALPHA_PRESENT,
                                   HAL_MDP_SRC_FORMAT_ALPHA_COMP_BITS_8,
                                   HAL_MDP_SRC_FORMAT_RED_COMP_BITS_8,
                                   HAL_MDP_SRC_FORMAT_BLUE_COMP_BITS_8,
                                   HAL_MDP_SRC_FORMAT_GREEN_COMP_BITS_8,
                                   HAL_MDP_SRC_FORMAT_DX_FORMAT_ON),

                                                                     // HAL_MDP_PIXEL_FORMAT_RGBX_1010102_32BPP
   HAL_MDP_PREPARE_SRC_FORMAT_INFO(HAL_MDP_SRC_FORMAT_FRAME_FORMAT_LINEAR,
                                   HAL_MDP_SRC_FORMAT_CHROMA_SAMPLE_RGB,
                                   HAL_MDP_SRC_FORMAT_FETCH_PLANES_INTERLEAVED,
                                   HAL_MDP_SRC_FORMAT_UNPACK_ALIGN_TO_LSB,
                                   HAL_MDP_SRC_FORMAT_UNPACK_TIGHT,
                                   HAL_MDP_SRC_FORAMT_UNPACK_COMPONENT_COUNT4,
                                   HAL_MDP_SRC_FORMAT_SRC_FOUR_BPP,
                                   HAL_MDP_SRC_FORMAT_ALPHA_PRESENT,
                                   HAL_MDP_SRC_FORMAT_ALPHA_COMP_BITS_8,
                                   HAL_MDP_SRC_FORMAT_RED_COMP_BITS_8,
                                   HAL_MDP_SRC_FORMAT_BLUE_COMP_BITS_8,
                                   HAL_MDP_SRC_FORMAT_GREEN_COMP_BITS_8,
                                   HAL_MDP_SRC_FORMAT_DX_FORMAT_ON),

                                                                     // HAL_MDP_PIXEL_FORMAT_RGBA_1010102_32BPP
   HAL_MDP_PREPARE_SRC_FORMAT_INFO(HAL_MDP_SRC_FORMAT_FRAME_FORMAT_LINEAR,
                                   HAL_MDP_SRC_FORMAT_CHROMA_SAMPLE_RGB,
                                   HAL_MDP_SRC_FORMAT_FETCH_PLANES_INTERLEAVED,
                                   HAL_MDP_SRC_FORMAT_UNPACK_ALIGN_TO_LSB,
                                   HAL_MDP_SRC_FORMAT_UNPACK_TIGHT,
                                   HAL_MDP_SRC_FORAMT_UNPACK_COMPONENT_COUNT4,
                                   HAL_MDP_SRC_FORMAT_SRC_FOUR_BPP,
                                   HAL_MDP_SRC_FORMAT_ALPHA_PRESENT,
                                   HAL_MDP_SRC_FORMAT_ALPHA_COMP_BITS_8,
                                   HAL_MDP_SRC_FORMAT_RED_COMP_BITS_8,
                                   HAL_MDP_SRC_FORMAT_BLUE_COMP_BITS_8,
                                   HAL_MDP_SRC_FORMAT_GREEN_COMP_BITS_8,
                                   HAL_MDP_SRC_FORMAT_DX_FORMAT_ON),

                                                                     // HAL_MDP_PIXEL_FORMAT_BGRX_1010102_32BPP
   HAL_MDP_PREPARE_SRC_FORMAT_INFO(HAL_MDP_SRC_FORMAT_FRAME_FORMAT_LINEAR,
                                   HAL_MDP_SRC_FORMAT_CHROMA_SAMPLE_RGB,
                                   HAL_MDP_SRC_FORMAT_FETCH_PLANES_INTERLEAVED,
                                   HAL_MDP_SRC_FORMAT_UNPACK_ALIGN_TO_LSB,
                                   HAL_MDP_SRC_FORMAT_UNPACK_TIGHT,
                                   HAL_MDP_SRC_FORAMT_UNPACK_COMPONENT_COUNT4,
                                   HAL_MDP_SRC_FORMAT_SRC_FOUR_BPP,
                                   HAL_MDP_SRC_FORMAT_ALPHA_PRESENT,
                                   HAL_MDP_SRC_FORMAT_ALPHA_COMP_BITS_8,
                                   HAL_MDP_SRC_FORMAT_RED_COMP_BITS_8,
                                   HAL_MDP_SRC_FORMAT_BLUE_COMP_BITS_8,
                                   HAL_MDP_SRC_FORMAT_GREEN_COMP_BITS_8,
                                   HAL_MDP_SRC_FORMAT_DX_FORMAT_ON),

                                                                     // HAL_MDP_PIXEL_FORMAT_BGRA_1010102_32BPP
   HAL_MDP_PREPARE_SRC_FORMAT_INFO(HAL_MDP_SRC_FORMAT_FRAME_FORMAT_LINEAR,
                                   HAL_MDP_SRC_FORMAT_CHROMA_SAMPLE_RGB,
                                   HAL_MDP_SRC_FORMAT_FETCH_PLANES_INTERLEAVED,
                                   HAL_MDP_SRC_FORMAT_UNPACK_ALIGN_TO_LSB,
                                   HAL_MDP_SRC_FORMAT_UNPACK_TIGHT,
                                   HAL_MDP_SRC_FORAMT_UNPACK_COMPONENT_COUNT4,
                                   HAL_MDP_SRC_FORMAT_SRC_FOUR_BPP,
                                   HAL_MDP_SRC_FORMAT_ALPHA_PRESENT,
                                   HAL_MDP_SRC_FORMAT_ALPHA_COMP_BITS_8,
                                   HAL_MDP_SRC_FORMAT_RED_COMP_BITS_8,
                                   HAL_MDP_SRC_FORMAT_BLUE_COMP_BITS_8,
                                   HAL_MDP_SRC_FORMAT_GREEN_COMP_BITS_8,
                                   HAL_MDP_SRC_FORMAT_DX_FORMAT_ON),

                                                                     // HAL_MDP_PIXEL_FORMAT_YCRYCB_H2V1_16BPP
   HAL_MDP_PREPARE_SRC_FORMAT_INFO(HAL_MDP_SRC_FORMAT_FRAME_FORMAT_LINEAR,
                                   HAL_MDP_SRC_FORMAT_CHROMA_SAMPLE_YUV422_H2V1,
                                   HAL_MDP_SRC_FORMAT_FETCH_PLANES_INTERLEAVED,
                                   HAL_MDP_SRC_FORMAT_UNPACK_ALIGN_TO_LSB,
                                   HAL_MDP_SRC_FORMAT_UNPACK_TIGHT,
                                   HAL_MDP_SRC_FORAMT_UNPACK_COMPONENT_COUNT4,
                                   HAL_MDP_SRC_FORMAT_SRC_TWO_BPP,
                                   HAL_MDP_SRC_FORMAT_ALPHA_NOT_PRESENT,
                                   HAL_MDP_SRC_FORMAT_ALPHA_COMP_BITS_0,
                                   HAL_MDP_SRC_FORMAT_RED_COMP_BITS_8,
                                   HAL_MDP_SRC_FORMAT_BLUE_COMP_BITS_8,
                                   HAL_MDP_SRC_FORMAT_GREEN_COMP_BITS_8,
                                   HAL_MDP_SRC_FORMAT_DX_FORMAT_OFF),

                                                                     // HAL_MDP_PIXEL_FORMAT_YCBYCR_H2V1_16BPP
   HAL_MDP_PREPARE_SRC_FORMAT_INFO(HAL_MDP_SRC_FORMAT_FRAME_FORMAT_LINEAR,
                                   HAL_MDP_SRC_FORMAT_CHROMA_SAMPLE_YUV422_H2V1,
                                   HAL_MDP_SRC_FORMAT_FETCH_PLANES_INTERLEAVED,
                                   HAL_MDP_SRC_FORMAT_UNPACK_ALIGN_TO_LSB,
                                   HAL_MDP_SRC_FORMAT_UNPACK_TIGHT,
                                   HAL_MDP_SRC_FORAMT_UNPACK_COMPONENT_COUNT4,
                                   HAL_MDP_SRC_FORMAT_SRC_TWO_BPP,
                                   HAL_MDP_SRC_FORMAT_ALPHA_NOT_PRESENT,
                                   HAL_MDP_SRC_FORMAT_ALPHA_COMP_BITS_0,
                                   HAL_MDP_SRC_FORMAT_RED_COMP_BITS_8,
                                   HAL_MDP_SRC_FORMAT_BLUE_COMP_BITS_8,
                                   HAL_MDP_SRC_FORMAT_GREEN_COMP_BITS_8,
                                   HAL_MDP_SRC_FORMAT_DX_FORMAT_OFF),

                                                                     // HAL_MDP_PIXEL_FORMAT_CRYCBY_H2V1_16BPP
   HAL_MDP_PREPARE_SRC_FORMAT_INFO(HAL_MDP_SRC_FORMAT_FRAME_FORMAT_LINEAR,
                                   HAL_MDP_SRC_FORMAT_CHROMA_SAMPLE_YUV422_H2V1,
                                   HAL_MDP_SRC_FORMAT_FETCH_PLANES_INTERLEAVED,
                                   HAL_MDP_SRC_FORMAT_UNPACK_ALIGN_TO_LSB,
                                   HAL_MDP_SRC_FORMAT_UNPACK_TIGHT,
                                   HAL_MDP_SRC_FORAMT_UNPACK_COMPONENT_COUNT4,
                                   HAL_MDP_SRC_FORMAT_SRC_TWO_BPP,
                                   HAL_MDP_SRC_FORMAT_ALPHA_NOT_PRESENT,
                                   HAL_MDP_SRC_FORMAT_ALPHA_COMP_BITS_0,
                                   HAL_MDP_SRC_FORMAT_RED_COMP_BITS_8,
                                   HAL_MDP_SRC_FORMAT_BLUE_COMP_BITS_8,
                                   HAL_MDP_SRC_FORMAT_GREEN_COMP_BITS_8,
                                   HAL_MDP_SRC_FORMAT_DX_FORMAT_OFF),

                                                                     // HAL_MDP_PIXEL_FORMAT_CBYCRY_H2V1_16BPP
   HAL_MDP_PREPARE_SRC_FORMAT_INFO(HAL_MDP_SRC_FORMAT_FRAME_FORMAT_LINEAR,
                                   HAL_MDP_SRC_FORMAT_CHROMA_SAMPLE_YUV422_H2V1,
                                   HAL_MDP_SRC_FORMAT_FETCH_PLANES_INTERLEAVED,
                                   HAL_MDP_SRC_FORMAT_UNPACK_ALIGN_TO_LSB,
                                   HAL_MDP_SRC_FORMAT_UNPACK_TIGHT,
                                   HAL_MDP_SRC_FORAMT_UNPACK_COMPONENT_COUNT4,
                                   HAL_MDP_SRC_FORMAT_SRC_TWO_BPP,
                                   HAL_MDP_SRC_FORMAT_ALPHA_NOT_PRESENT,
                                   HAL_MDP_SRC_FORMAT_ALPHA_COMP_BITS_0,
                                   HAL_MDP_SRC_FORMAT_RED_COMP_BITS_8,
                                   HAL_MDP_SRC_FORMAT_BLUE_COMP_BITS_8,
                                   HAL_MDP_SRC_FORMAT_GREEN_COMP_BITS_8,
                                   HAL_MDP_SRC_FORMAT_DX_FORMAT_OFF),

                                                                     //HAL_MDP_PIXEL_FORMAT_CRYCB_H1V1_24BPP 
   HAL_MDP_PREPARE_SRC_FORMAT_INFO(HAL_MDP_SRC_FORMAT_FRAME_FORMAT_LINEAR,
                                   HAL_MDP_SRC_FORMAT_CHROMA_SAMPLE_YUV444,
                                   HAL_MDP_SRC_FORMAT_FETCH_PLANES_INTERLEAVED,
                                   HAL_MDP_SRC_FORMAT_UNPACK_ALIGN_TO_LSB,
                                   HAL_MDP_SRC_FORMAT_UNPACK_TIGHT,
                                   HAL_MDP_SRC_FORAMT_UNPACK_COMPONENT_COUNT3,
                                   HAL_MDP_SRC_FORMAT_SRC_THREE_BPP,
                                   HAL_MDP_SRC_FORMAT_ALPHA_NOT_PRESENT,
                                   HAL_MDP_SRC_FORMAT_ALPHA_COMP_BITS_0,
                                   HAL_MDP_SRC_FORMAT_RED_COMP_BITS_8,
                                   HAL_MDP_SRC_FORMAT_BLUE_COMP_BITS_8,
                                   HAL_MDP_SRC_FORMAT_GREEN_COMP_BITS_8,
                                   HAL_MDP_SRC_FORMAT_DX_FORMAT_OFF),

                                                                     // HAL_MDP_PIXEL_FORMAT_AYCBCR_H1V1_32BPP
   HAL_MDP_PREPARE_SRC_FORMAT_INFO(HAL_MDP_SRC_FORMAT_FRAME_FORMAT_LINEAR,
                                   HAL_MDP_SRC_FORMAT_CHROMA_SAMPLE_YUV444,
                                   HAL_MDP_SRC_FORMAT_FETCH_PLANES_INTERLEAVED,
                                   HAL_MDP_SRC_FORMAT_UNPACK_ALIGN_TO_LSB,
                                   HAL_MDP_SRC_FORMAT_UNPACK_TIGHT,
                                   HAL_MDP_SRC_FORAMT_UNPACK_COMPONENT_COUNT4,
                                   HAL_MDP_SRC_FORMAT_SRC_FOUR_BPP,
                                   HAL_MDP_SRC_FORMAT_ALPHA_PRESENT,
                                   HAL_MDP_SRC_FORMAT_ALPHA_COMP_BITS_8,
                                   HAL_MDP_SRC_FORMAT_RED_COMP_BITS_8,
                                   HAL_MDP_SRC_FORMAT_BLUE_COMP_BITS_8,
                                   HAL_MDP_SRC_FORMAT_GREEN_COMP_BITS_8,
                                   HAL_MDP_SRC_FORMAT_DX_FORMAT_OFF),
                                   
                                                                     // HAL_MDP_PIXEL_FORMAT_ACRCBY_H1V1_32BPP
   HAL_MDP_PREPARE_SRC_FORMAT_INFO(HAL_MDP_SRC_FORMAT_FRAME_FORMAT_LINEAR,
                                   HAL_MDP_SRC_FORMAT_CHROMA_SAMPLE_YUV444,
                                   HAL_MDP_SRC_FORMAT_FETCH_PLANES_INTERLEAVED,
                                   HAL_MDP_SRC_FORMAT_UNPACK_ALIGN_TO_LSB,
                                   HAL_MDP_SRC_FORMAT_UNPACK_TIGHT,
                                   HAL_MDP_SRC_FORAMT_UNPACK_COMPONENT_COUNT4,
                                   HAL_MDP_SRC_FORMAT_SRC_FOUR_BPP,
                                   HAL_MDP_SRC_FORMAT_ALPHA_PRESENT,
                                   HAL_MDP_SRC_FORMAT_ALPHA_COMP_BITS_8,
                                   HAL_MDP_SRC_FORMAT_RED_COMP_BITS_8,
                                   HAL_MDP_SRC_FORMAT_BLUE_COMP_BITS_8,
                                   HAL_MDP_SRC_FORMAT_GREEN_COMP_BITS_8,
                                   HAL_MDP_SRC_FORMAT_DX_FORMAT_OFF),

   /* Pseudo plannar pixel formats                                      HAL_MDP_PIXEL_FORMAT_PSEUDO_PLANNAR_BASE */

                                                                     // HAL_MDP_PIXEL_FORMAT_Y_CBCR_H2V2_12BPP
   HAL_MDP_PREPARE_SRC_FORMAT_INFO(HAL_MDP_SRC_FORMAT_FRAME_FORMAT_LINEAR,
                                   HAL_MDP_SRC_FORMAT_CHROMA_SAMPLE_YUV420_N2V1,
                                   HAL_MDP_SRC_FORMAT_FETCH_PLANES_PSUEDO_PLANAR,
                                   HAL_MDP_SRC_FORMAT_UNPACK_ALIGN_TO_LSB,
                                   HAL_MDP_SRC_FORMAT_UNPACK_TIGHT,
                                   HAL_MDP_SRC_FORAMT_UNPACK_COMPONENT_COUNT2,
                                   HAL_MDP_SRC_FORMAT_SRC_TWO_BPP,
                                   HAL_MDP_SRC_FORMAT_ALPHA_NOT_PRESENT,
                                   HAL_MDP_SRC_FORMAT_ALPHA_COMP_BITS_0,
                                   HAL_MDP_SRC_FORMAT_RED_COMP_BITS_8,
                                   HAL_MDP_SRC_FORMAT_BLUE_COMP_BITS_8,
                                   HAL_MDP_SRC_FORMAT_GREEN_COMP_BITS_8,
                                   HAL_MDP_SRC_FORMAT_DX_FORMAT_OFF),

                                                                     // HAL_MDP_PIXEL_FORMAT_Y_CRCB_H2V2_12BPP
   HAL_MDP_PREPARE_SRC_FORMAT_INFO(HAL_MDP_SRC_FORMAT_FRAME_FORMAT_LINEAR,
                                   HAL_MDP_SRC_FORMAT_CHROMA_SAMPLE_YUV420_N2V1,
                                   HAL_MDP_SRC_FORMAT_FETCH_PLANES_PSUEDO_PLANAR,
                                   HAL_MDP_SRC_FORMAT_UNPACK_ALIGN_TO_LSB,
                                   HAL_MDP_SRC_FORMAT_UNPACK_TIGHT,
                                   HAL_MDP_SRC_FORAMT_UNPACK_COMPONENT_COUNT2,
                                   HAL_MDP_SRC_FORMAT_SRC_TWO_BPP,
                                   HAL_MDP_SRC_FORMAT_ALPHA_NOT_PRESENT,
                                   HAL_MDP_SRC_FORMAT_ALPHA_COMP_BITS_0,
                                   HAL_MDP_SRC_FORMAT_RED_COMP_BITS_8,
                                   HAL_MDP_SRC_FORMAT_BLUE_COMP_BITS_8,
                                   HAL_MDP_SRC_FORMAT_GREEN_COMP_BITS_8,
                                   HAL_MDP_SRC_FORMAT_DX_FORMAT_OFF),

                                                                     // HAL_MDP_PIXEL_FORMAT_Y_CRCB_H2V1_16BPP
   HAL_MDP_PREPARE_SRC_FORMAT_INFO(HAL_MDP_SRC_FORMAT_FRAME_FORMAT_LINEAR,
                                   HAL_MDP_SRC_FORMAT_CHROMA_SAMPLE_YUV422_H2V1,
                                   HAL_MDP_SRC_FORMAT_FETCH_PLANES_PSUEDO_PLANAR,
                                   HAL_MDP_SRC_FORMAT_UNPACK_ALIGN_TO_LSB,
                                   HAL_MDP_SRC_FORMAT_UNPACK_TIGHT,
                                   HAL_MDP_SRC_FORAMT_UNPACK_COMPONENT_COUNT2,
                                   HAL_MDP_SRC_FORMAT_SRC_TWO_BPP,
                                   HAL_MDP_SRC_FORMAT_ALPHA_NOT_PRESENT,
                                   HAL_MDP_SRC_FORMAT_ALPHA_COMP_BITS_0,
                                   HAL_MDP_SRC_FORMAT_RED_COMP_BITS_8,
                                   HAL_MDP_SRC_FORMAT_BLUE_COMP_BITS_8,
                                   HAL_MDP_SRC_FORMAT_GREEN_COMP_BITS_8,
                                   HAL_MDP_SRC_FORMAT_DX_FORMAT_OFF),

                                                                     // HAL_MDP_PIXEL_FORMAT_Y_CBCR_H2V1_16BPP
   HAL_MDP_PREPARE_SRC_FORMAT_INFO(HAL_MDP_SRC_FORMAT_FRAME_FORMAT_LINEAR,
                                   HAL_MDP_SRC_FORMAT_CHROMA_SAMPLE_YUV422_H2V1,
                                   HAL_MDP_SRC_FORMAT_FETCH_PLANES_PSUEDO_PLANAR,
                                   HAL_MDP_SRC_FORMAT_UNPACK_ALIGN_TO_LSB,
                                   HAL_MDP_SRC_FORMAT_UNPACK_TIGHT,
                                   HAL_MDP_SRC_FORAMT_UNPACK_COMPONENT_COUNT2,
                                   HAL_MDP_SRC_FORMAT_SRC_TWO_BPP,
                                   HAL_MDP_SRC_FORMAT_ALPHA_NOT_PRESENT,
                                   HAL_MDP_SRC_FORMAT_ALPHA_COMP_BITS_0,
                                   HAL_MDP_SRC_FORMAT_RED_COMP_BITS_8,
                                   HAL_MDP_SRC_FORMAT_BLUE_COMP_BITS_8,
                                   HAL_MDP_SRC_FORMAT_GREEN_COMP_BITS_8,
                                   HAL_MDP_SRC_FORMAT_DX_FORMAT_OFF),

                                                                     // HAL_MDP_PIXEL_FORMAT_Y_CBCR_H2V2_VC1_12BPP
   HAL_MDP_PREPARE_SRC_FORMAT_INFO(HAL_MDP_SRC_FORMAT_FRAME_FORMAT_LINEAR,
                                   HAL_MDP_SRC_FORMAT_CHROMA_SAMPLE_YUV420_N2V1,
                                   HAL_MDP_SRC_FORMAT_FETCH_PLANES_PSUEDO_PLANAR,
                                   HAL_MDP_SRC_FORMAT_UNPACK_ALIGN_TO_LSB,
                                   HAL_MDP_SRC_FORMAT_UNPACK_TIGHT,
                                   HAL_MDP_SRC_FORAMT_UNPACK_COMPONENT_COUNT2,
                                   HAL_MDP_SRC_FORMAT_SRC_TWO_BPP,
                                   HAL_MDP_SRC_FORMAT_ALPHA_NOT_PRESENT,
                                   HAL_MDP_SRC_FORMAT_ALPHA_COMP_BITS_0,
                                   HAL_MDP_SRC_FORMAT_RED_COMP_BITS_8,
                                   HAL_MDP_SRC_FORMAT_BLUE_COMP_BITS_8,
                                   HAL_MDP_SRC_FORMAT_GREEN_COMP_BITS_8,
                                   HAL_MDP_SRC_FORMAT_DX_FORMAT_OFF),

                                                                     // HAL_MDP_PIXEL_FORMAT_Y_CRCB_H2V2_VC1_12BPP
   HAL_MDP_PREPARE_SRC_FORMAT_INFO(HAL_MDP_SRC_FORMAT_FRAME_FORMAT_LINEAR,
                                   HAL_MDP_SRC_FORMAT_CHROMA_SAMPLE_YUV420_N2V1,
                                   HAL_MDP_SRC_FORMAT_FETCH_PLANES_PSUEDO_PLANAR,
                                   HAL_MDP_SRC_FORMAT_UNPACK_ALIGN_TO_LSB,
                                   HAL_MDP_SRC_FORMAT_UNPACK_TIGHT,
                                   HAL_MDP_SRC_FORAMT_UNPACK_COMPONENT_COUNT2,
                                   HAL_MDP_SRC_FORMAT_SRC_TWO_BPP,
                                   HAL_MDP_SRC_FORMAT_ALPHA_NOT_PRESENT,
                                   HAL_MDP_SRC_FORMAT_ALPHA_COMP_BITS_0,
                                   HAL_MDP_SRC_FORMAT_RED_COMP_BITS_8,
                                   HAL_MDP_SRC_FORMAT_BLUE_COMP_BITS_8,
                                   HAL_MDP_SRC_FORMAT_GREEN_COMP_BITS_8,
                                   HAL_MDP_SRC_FORMAT_DX_FORMAT_OFF),

                                                                     // HAL_MDP_PIXEL_FORMAT_Y_CBCR_SUPERTILE_4x2_12BPP
   0x00000000,

                                                                     // HAL_MDP_PIXEL_FORMAT_Y_CBCR_VENUS_4x4_12BPP
   0x00000000,

                                                                     // HAL_MDP_PIXEL_FORMAT_Y_CRCB_H2V2_P010_24BPP
   HAL_MDP_PREPARE_SRC_FORMAT_INFO(HAL_MDP_SRC_FORMAT_FRAME_FORMAT_LINEAR,
                                   HAL_MDP_SRC_FORMAT_CHROMA_SAMPLE_YUV420_N2V1,
                                   HAL_MDP_SRC_FORMAT_FETCH_PLANES_PSUEDO_PLANAR,
                                   HAL_MDP_SRC_FORMAT_UNPACK_ALIGN_TO_MSB,
                                   HAL_MDP_SRC_FORMAT_UNPACK_LOOSE,
                                   HAL_MDP_SRC_FORAMT_UNPACK_COMPONENT_COUNT2,
                                   HAL_MDP_SRC_FORMAT_SRC_TWO_BPP,
                                   HAL_MDP_SRC_FORMAT_ALPHA_NOT_PRESENT,
                                   HAL_MDP_SRC_FORMAT_ALPHA_COMP_BITS_0,
                                   HAL_MDP_SRC_FORMAT_RED_COMP_BITS_8,
                                   HAL_MDP_SRC_FORMAT_BLUE_COMP_BITS_8,
                                   HAL_MDP_SRC_FORMAT_GREEN_COMP_BITS_8,
                                   HAL_MDP_SRC_FORMAT_DX_FORMAT_ON),
                                                                     // HAL_MDP_PIXEL_FORMAT_Y_CRCB_H2V2_TP10_16BPP
   HAL_MDP_PREPARE_SRC_FORMAT_INFO(HAL_MDP_SRC_FORMAT_FRAME_FORMAT_TILE,
                                   HAL_MDP_SRC_FORMAT_CHROMA_SAMPLE_YUV420_N2V1,
                                   HAL_MDP_SRC_FORMAT_FETCH_PLANES_PSUEDO_PLANAR,
                                   HAL_MDP_SRC_FORMAT_UNPACK_ALIGN_TO_LSB,
                                   HAL_MDP_SRC_FORMAT_UNPACK_TIGHT,
                                   HAL_MDP_SRC_FORAMT_UNPACK_COMPONENT_COUNT2,
                                   HAL_MDP_SRC_FORMAT_SRC_TWO_BPP,
                                   HAL_MDP_SRC_FORMAT_ALPHA_NOT_PRESENT,
                                   HAL_MDP_SRC_FORMAT_ALPHA_COMP_BITS_0,
                                   HAL_MDP_SRC_FORMAT_RED_COMP_BITS_8,
                                   HAL_MDP_SRC_FORMAT_BLUE_COMP_BITS_8,
                                   HAL_MDP_SRC_FORMAT_GREEN_COMP_BITS_8,
                                   HAL_MDP_SRC_FORMAT_DX_FORMAT_ON),

   /* Planar pixel formats                                          HAL_MDP_PIXEL_FORMAT_PLANAR_BASE */

                                                                     // HAL_MDP_PIXEL_FORMAT_Y_CR_CB_H2V2_12BPP
   HAL_MDP_PREPARE_SRC_FORMAT_INFO(HAL_MDP_SRC_FORMAT_FRAME_FORMAT_LINEAR,
                                   HAL_MDP_SRC_FORMAT_CHROMA_SAMPLE_YUV420_N2V1,
                                   HAL_MDP_SRC_FORMAT_FETCH_PLANES_PLANAR,
                                   HAL_MDP_SRC_FORMAT_UNPACK_ALIGN_TO_LSB,
                                   HAL_MDP_SRC_FORMAT_UNPACK_TIGHT,
                                   HAL_MDP_SRC_FORAMT_UNPACK_COMPONENT_COUNT_NONE,
                                   HAL_MDP_SRC_FORMAT_SRC_NO_BPP,
                                   HAL_MDP_SRC_FORMAT_ALPHA_NOT_PRESENT,
                                   HAL_MDP_SRC_FORMAT_ALPHA_COMP_BITS_0,
                                   HAL_MDP_SRC_FORMAT_RED_COMP_BITS_8,
                                   HAL_MDP_SRC_FORMAT_BLUE_COMP_BITS_8,
                                   HAL_MDP_SRC_FORMAT_GREEN_COMP_BITS_8,
                                   HAL_MDP_SRC_FORMAT_DX_FORMAT_OFF),

                                                                     // HAL_MDP_PIXEL_FORMAT_Y_CR_CB_H2V1_16BPP
   HAL_MDP_PREPARE_SRC_FORMAT_INFO(HAL_MDP_SRC_FORMAT_FRAME_FORMAT_LINEAR,
                                   HAL_MDP_SRC_FORMAT_CHROMA_SAMPLE_YUV422_H2V1,
                                   HAL_MDP_SRC_FORMAT_FETCH_PLANES_PLANAR,
                                   HAL_MDP_SRC_FORMAT_UNPACK_ALIGN_TO_LSB,
                                   HAL_MDP_SRC_FORMAT_UNPACK_TIGHT,
                                   HAL_MDP_SRC_FORAMT_UNPACK_COMPONENT_COUNT_NONE,
                                   HAL_MDP_SRC_FORMAT_SRC_NO_BPP,
                                   HAL_MDP_SRC_FORMAT_ALPHA_NOT_PRESENT,
                                   HAL_MDP_SRC_FORMAT_ALPHA_COMP_BITS_0,
                                   HAL_MDP_SRC_FORMAT_RED_COMP_BITS_8,
                                   HAL_MDP_SRC_FORMAT_BLUE_COMP_BITS_8,
                                   HAL_MDP_SRC_FORMAT_GREEN_COMP_BITS_8,
                                   HAL_MDP_SRC_FORMAT_DX_FORMAT_OFF),

                                                                     // HAL_MDP_PIXEL_FORMAT_Y_CB_CR_H2V2_12BPP
   HAL_MDP_PREPARE_SRC_FORMAT_INFO(HAL_MDP_SRC_FORMAT_FRAME_FORMAT_LINEAR,
                                   HAL_MDP_SRC_FORMAT_CHROMA_SAMPLE_YUV420_N2V1,
                                   HAL_MDP_SRC_FORMAT_FETCH_PLANES_PLANAR,
                                   HAL_MDP_SRC_FORMAT_UNPACK_ALIGN_TO_LSB,
                                   HAL_MDP_SRC_FORMAT_UNPACK_TIGHT,
                                   HAL_MDP_SRC_FORAMT_UNPACK_COMPONENT_COUNT_NONE,
                                   HAL_MDP_SRC_FORMAT_SRC_NO_BPP,
                                   HAL_MDP_SRC_FORMAT_ALPHA_NOT_PRESENT,
                                   HAL_MDP_SRC_FORMAT_ALPHA_COMP_BITS_0,
                                   HAL_MDP_SRC_FORMAT_RED_COMP_BITS_8,
                                   HAL_MDP_SRC_FORMAT_BLUE_COMP_BITS_8,
                                   HAL_MDP_SRC_FORMAT_GREEN_COMP_BITS_8,
                                   HAL_MDP_SRC_FORMAT_DX_FORMAT_OFF),

                                                                     // HAL_MDP_PIXEL_FORMAT_Y_CB_CR_H2V1_16BPP
   HAL_MDP_PREPARE_SRC_FORMAT_INFO(HAL_MDP_SRC_FORMAT_FRAME_FORMAT_LINEAR,
                                   HAL_MDP_SRC_FORMAT_CHROMA_SAMPLE_YUV422_H2V1,
                                   HAL_MDP_SRC_FORMAT_FETCH_PLANES_PLANAR,
                                   HAL_MDP_SRC_FORMAT_UNPACK_ALIGN_TO_LSB,
                                   HAL_MDP_SRC_FORMAT_UNPACK_TIGHT,
                                   HAL_MDP_SRC_FORAMT_UNPACK_COMPONENT_COUNT_NONE,
                                   HAL_MDP_SRC_FORMAT_SRC_NO_BPP,
                                   HAL_MDP_SRC_FORMAT_ALPHA_NOT_PRESENT,
                                   HAL_MDP_SRC_FORMAT_ALPHA_COMP_BITS_0,
                                   HAL_MDP_SRC_FORMAT_RED_COMP_BITS_8,
                                   HAL_MDP_SRC_FORMAT_BLUE_COMP_BITS_8,
                                   HAL_MDP_SRC_FORMAT_GREEN_COMP_BITS_8,
                                   HAL_MDP_SRC_FORMAT_DX_FORMAT_OFF)
};

/* 
 * UNPACK WORD GENERATION:
 * ========================================================================================
 * MDP HW always works in 4 color components: C0-C1-C2-C3
 * COLOR MAPPING IS:
 *                   C0 - GREEN/LUMA   = 0x00
 *                   C1 - BLUE/Cb      = 0x01
 *                   C2 - REG/Cr       = 0x02
 *                   C3 - ALPHA        = 0x03
 *
 * ELEMENTS: Each pixel data in memory from LSB (Least Significant Byte) to MSB components
 *
 * For example: RGB888. In memory/frame buffer: 
 *           ELEMENT[0] =       ADDR[0]  = B   = C1  = 0x01
 *           ELEMENT[1] =       ADDR[1]  = G   = C0  = 0x00
 *           ELEMENT[2] =       ADDR[2]  = R   = C2  = 0x02
 *
 * UNPACK PATTERN IS 0xELEMENT[3]-ELEMENT[2]-ELEMENT[1]-ELEMENT[0]
 *
 * RGB888 UNPACK PATTERN = 0x00-0x02-0x00-0x01  = 0x00020001
 * =========================================================================================
 */

uint32 guSrcUnpackInfo[HAL_MDP_PIXEL_FORMAT_MAX] =
{
   0x00000000,                                     // HAL_MDP_PIXEL_FORMAT_NONE,
   0x00020001,                                     // HAL_MDP_PIXEL_FORMAT_RGB_565_16BPP
   0x00020001,                                     // HAL_MDP_PIXEL_FORMAT_RGB_666_18BPP
   0x00020001,                                     // HAL_MDP_PIXEL_FORMAT_RGB_888_24BPP
   0x03020001,                                     // HAL_MDP_PIXEL_FORMAT_ARGB_1555_16BPP
   0x03020001,                                     // HAL_MDP_PIXEL_FORMAT_XRGB_8888_32BPP
   0x03020001,                                     // HAL_MDP_PIXEL_FORMAT_ARGB_8888_32BPP
   0x00010002,                                     // HAL_MDP_PIXEL_FORMAT_BGR_565_16BPP
   0x00010002,                                     // HAL_MDP_PIXEL_FORMAT_BGR_888_24BPP
   0x03010002,                                     // HAL_MDP_PIXEL_FORMAT_ABGR_1555_16BPP
   0x03010002,                                     // HAL_MDP_PIXEL_FORMAT_XBGR_8888_32BPP
   0x03010002,                                     // HAL_MDP_PIXEL_FORMAT_ABGR_8888_32BPP
   0x02000103,                                     // HAL_MDP_PIXEL_FORMAT_RGBA_5551_16BPP
   0x02000103,                                     // HAL_MDP_PIXEL_FORMAT_RGBA_8888_32BPP
   0x00000000,                                     // HAL_MDP_PIXEL_FORMAT_RGB_OXILI_TILE_4x4_24BPP
   0x03020001,                                     // HAL_MDP_PIXEL_FORMAT_ARGB_4444_16BPP
   0x03020001,                                     // HAL_MDP_PIXEL_FORMAT_XRGB_2101010_32BPP
   0x03020001,                                     // HAL_MDP_PIXEL_FORMAT_ARGB_2101010_32BPP
   0x03010002,                                     // HAL_MDP_PIXEL_FORMAT_XBGR_2101010_32BPP
   0x03010002,                                     // HAL_MDP_PIXEL_FORMAT_ABGR_2101010_32BPP
   0x02000103,                                     // HAL_MDP_PIXEL_FORMAT_RGBX_1010102_32BPP
   0x02000103,                                     // HAL_MDP_PIXEL_FORMAT_RGBA_1010102_32BPP
   0x01000203,                                     // HAL_MDP_PIXEL_FORMAT_BGRX_1010102_32BPP
   0x01000203,                                     // HAL_MDP_PIXEL_FORMAT_BGRA_1010102_32BPP

   0x00020001,                                     // HAL_MDP_PIXEL_FORMAT_YCRYCB_H2V1_16BPP
   0x00010002,                                     // HAL_MDP_PIXEL_FORMAT_YCBYCR_H2V1_16BPP
   0x02000100,                                     // HAL_MDP_PIXEL_FORMAT_CRYCBY_H2V1_16BPP
   0x01000200,                                     // HAL_MDP_PIXEL_FORMAT_CBYCRY_H2V1_16BPP
   0x00020001,                                     // HAL_MDP_PIXEL_FORMAT_CRYCB_H1V1_24BPP 
   0x03000102,                                     // HAL_MDP_PIXEL_FORMAT_AYCBCR_H1V1_32BPP
   0x03020100,                                     // HAL_MDP_PIXEL_FORMAT_ACRCBY_H1V1_32BPP

   0x00000102,                                     // HAL_MDP_PIXEL_FORMAT_Y_CBCR_H2V2_12BPP
   0x00000201,                                     // HAL_MDP_PIXEL_FORMAT_Y_CRCB_H2V2_12BPP
   0x00000201,                                     // HAL_MDP_PIXEL_FORMAT_Y_CRCB_H2V1_16BPP
   0x00000102,                                     // HAL_MDP_PIXEL_FORMAT_Y_CBCR_H2V1_16BPP
   0x00000102,                                     // HAL_MDP_PIXEL_FORMAT_Y_CBCR_H2V2_VC1_12BPP
   0x00000201,                                     // HAL_MDP_PIXEL_FORMAT_Y_CRCB_H2V2_VC1_12BPP
   0x00000102,                                     // HAL_MDP_PIXEL_FORMAT_Y_CBCR_SUPERTILE_4x2_12BPP
   0x00000102,                                     // HAL_MDP_PIXEL_FORMAT_Y_CBCR_VENUS_4x4_12BPP
   0x00000201,                                     // HAL_MDP_PIXEL_FORMAT_Y_CRCB_H2V2_P010_24BPP
   0x00000201,                                     // HAL_MDP_PIXEL_FORMAT_Y_CRCB_H2V2_TP10_16BPP

   0x00000102,                                     // HAL_MDP_PIXEL_FORMAT_Y_CR_CB_H2V2_12BPP
   0x00000102,                                     // HAL_MDP_PIXEL_FORMAT_Y_CR_CB_H2V1_16BPP
   0x00000102,                                     // HAL_MDP_PIXEL_FORMAT_Y_CB_CR_H2V2_12BPP
   0x00000102,                                     // HAL_MDP_PIXEL_FORMAT_Y_CB_CR_H2V1_16BPP
};

/* 
 * UNPACK WORD GENERATION IN DECOMPRESSION:
 * ========================================================================================
 * The generation is the same as in guSrcUnpackInfo.
 * 
 * For reverse colour order, e.g. BGR888, it is assigned the same unpack pattern as RGB888
 * Similarly for Y_CRCB, it is assigned the unpack pattern of Y_CBCR
 * 
 * =========================================================================================
 */
uint32 guSrcDecompressUnpackInfo[HAL_MDP_PIXEL_FORMAT_MAX] =
{
   0x00000000,                                     // HAL_MDP_PIXEL_FORMAT_NONE,
   0x00020001,                                     // HAL_MDP_PIXEL_FORMAT_RGB_565_16BPP
   0x00020001,                                     // HAL_MDP_PIXEL_FORMAT_RGB_666_18BPP
   0x00020001,                                     // HAL_MDP_PIXEL_FORMAT_RGB_888_24BPP
   0x03020001,                                     // HAL_MDP_PIXEL_FORMAT_ARGB_1555_16BPP
   0x03020001,                                     // HAL_MDP_PIXEL_FORMAT_XRGB_8888_32BPP
   0x03020001,                                     // HAL_MDP_PIXEL_FORMAT_ARGB_8888_32BPP
   0x00010002,                                     // HAL_MDP_PIXEL_FORMAT_BGR_565_16BPP
   0x00020001,                                     // HAL_MDP_PIXEL_FORMAT_BGR_888_24BPP, using the same one as RGB_888_24BPP
   0x03010002,                                     // HAL_MDP_PIXEL_FORMAT_ABGR_1555_16BPP
   0x03020001,                                     // HAL_MDP_PIXEL_FORMAT_XBGR_8888_32BPP, using the same one as XRGB_8888_32BPP
   0x03020001,                                     // HAL_MDP_PIXEL_FORMAT_ABGR_8888_32BPP, using the same one as ARGB_8888_32BPP
   0x02000103,                                     // HAL_MDP_PIXEL_FORMAT_RGBA_5551_16BPP
   0x03020001,                                     // HAL_MDP_PIXEL_FORMAT_RGBA_8888_32BPP, using the same one as ARGB_8888_32BPP
   0x00000000,                                     // HAL_MDP_PIXEL_FORMAT_RGB_OXILI_TILE_4x4_24BPP
   0x03020001,                                     // HAL_MDP_PIXEL_FORMAT_ARGB_4444_16BPP
   0x03020001,                                     // HAL_MDP_PIXEL_FORMAT_XRGB_2101010_32BPP
   0x03020001,                                     // HAL_MDP_PIXEL_FORMAT_ARGB_2101010_32BPP
   0x03020001,                                     // HAL_MDP_PIXEL_FORMAT_XBGR_2101010_32BPP
   0x03020001,                                     // HAL_MDP_PIXEL_FORMAT_ABGR_2101010_32BPP
   0x02000103,                                     // HAL_MDP_PIXEL_FORMAT_RGBX_1010102_32BPP
   0x02000103,                                     // HAL_MDP_PIXEL_FORMAT_RGBA_1010102_32BPP
   0x02000103,                                     // HAL_MDP_PIXEL_FORMAT_BGRX_1010102_32BPP
   0x02000103,                                     // HAL_MDP_PIXEL_FORMAT_BGRA_1010102_32BPP

   0x00020001,                                     // HAL_MDP_PIXEL_FORMAT_YCRYCB_H2V1_16BPP
   0x00010002,                                     // HAL_MDP_PIXEL_FORMAT_YCBYCR_H2V1_16BPP
   0x02000100,                                     // HAL_MDP_PIXEL_FORMAT_CRYCBY_H2V1_16BPP
   0x01000200,                                     // HAL_MDP_PIXEL_FORMAT_CBYCRY_H2V1_16BPP
   0x00020001,                                     // HAL_MDP_PIXEL_FORMAT_CRYCB_H1V1_24BPP 
   0x03000102,                                     // HAL_MDP_PIXEL_FORMAT_AYCBCR_H1V1_32BPP
   0x03020100,                                     // HAL_MDP_PIXEL_FORMAT_ACRCBY_H1V1_32BPP

   0x00000102,                                     // HAL_MDP_PIXEL_FORMAT_Y_CBCR_H2V2_12BPP
   0x00000102,                                     // HAL_MDP_PIXEL_FORMAT_Y_CRCB_H2V2_12BPP, using the same one as Y_CBCR_H2V2_12BPP
   0x00000102,                                     // HAL_MDP_PIXEL_FORMAT_Y_CRCB_H2V1_16BPP, using the same one as Y_CBCR_H2V1_16BPP
   0x00000102,                                     // HAL_MDP_PIXEL_FORMAT_Y_CBCR_H2V1_16BPP
   0x00000102,                                     // HAL_MDP_PIXEL_FORMAT_Y_CBCR_H2V2_VC1_12BPP
   0x00000102,                                     // HAL_MDP_PIXEL_FORMAT_Y_CRCB_H2V2_VC1_12BPP, using the same one as Y_CBCR_H2V2_12BPP
   0x00000102,                                     // HAL_MDP_PIXEL_FORMAT_Y_CBCR_SUPERTILE_4x2_12BPP
   0x00000102,                                     // HAL_MDP_PIXEL_FORMAT_Y_CBCR_VENUS_4x4_12BPP
   0x00000102,                                     // HAL_MDP_PIXEL_FORMAT_Y_CRCB_H2V2_P010_24BPP
   0x00000102,                                     // HAL_MDP_PIXEL_FORMAT_Y_CRCB_H2V2_TP10_16BPP

   0x00000102,                                     // HAL_MDP_PIXEL_FORMAT_Y_CR_CB_H2V2_12BPP
   0x00000102,                                     // HAL_MDP_PIXEL_FORMAT_Y_CR_CB_H2V1_16BPP
   0x00000102,                                     // HAL_MDP_PIXEL_FORMAT_Y_CB_CR_H2V2_12BPP
   0x00000102,                                     // HAL_MDP_PIXEL_FORMAT_Y_CB_CR_H2V1_16BPP
};

/* 
 * PANEL FORMAT WORD GENERATION:
 * ========================================================================================
 * Panel format only support 3 color components R-G-B and Bit-depths of 4, 5, 6 and 8
 *
 * The register definition is:
 *             COMPONENT-2  [Bits - 5:4]  = 0x00   - 4 Bits
 *                                        = 0x01   - 5 Bits
 *                                        = 0x02   - 6 Bits
 *                                        = 0x03   - 8 Bits
 *             COMPONENT-1  [Bits - 3:2]  = 0x00   - 4 Bits
 *                                        = 0x01   - 5 Bits
 *                                        = 0x02   - 6 Bits
 *                                        = 0x03   - 8 Bits
 *             COMPONENT-0  [Bits - 1:0]  = 0x00   - 4 Bits
 *                                        = 0x01   - 5 Bits
 *                                        = 0x02   - 6 Bits
 *                                        = 0x03   - 8 Bits
 *
 * Remaining bits fileds should be same as default register value
 * =========================================================================================
 */
uint32 guPanelFormatInfo[HAL_MDP_PIXEL_FORMAT_MAX] =
{
   0x00000000,                                     // HAL_MDP_PIXEL_FORMAT_NONE,
   0x00000016,                                     // HAL_MDP_PIXEL_FORMAT_RGB_565_16BPP
   0x0000002A,                                     // HAL_MDP_PIXEL_FORMAT_RGB_666_18BPP
   0x0000003F,                                     // HAL_MDP_PIXEL_FORMAT_RGB_888_24BPP
   0x00000000,                                     // HAL_MDP_PIXEL_FORMAT_ARGB_1555_16BPP
   0x00000000,                                     // HAL_MDP_PIXEL_FORMAT_XRGB_8888_32BPP
   0x00000000,                                     // HAL_MDP_PIXEL_FORMAT_ARGB_8888_32BPP
   0x00000000,                                     // HAL_MDP_PIXEL_FORMAT_BGR_565_16BPP
   0x00000000,                                     // HAL_MDP_PIXEL_FORMAT_BGR_888_24BPP
   0x00000000,                                     // HAL_MDP_PIXEL_FORMAT_ABGR_1555_16BPP
   0x00000000,                                     // HAL_MDP_PIXEL_FORMAT_XBGR_8888_32BPP
   0x00000000,                                     // HAL_MDP_PIXEL_FORMAT_ABGR_8888_32BPP
   0x00000000,                                     // HAL_MDP_PIXEL_FORMAT_RGBA_5551_16BPP
   0x00000000,                                     // HAL_MDP_PIXEL_FORMAT_RGBA_8888_32BPP
   0x00000000,                                     // HAL_MDP_PIXEL_FORMAT_RGB_OXILI_TILE_4x4_24BPP
   0x00000000,                                     // HAL_MDP_PIXEL_FORMAT_ARGB_4444_16BPP
   0x00000000,                                     // HAL_MDP_PIXEL_FORMAT_XRGB_2101010_32BPP
   0x00000000,                                     // HAL_MDP_PIXEL_FORMAT_ARGB_2101010_32BPP
   0x00000000,                                     // HAL_MDP_PIXEL_FORMAT_XBGR_2101010_32BPP
   0x00000000,                                     // HAL_MDP_PIXEL_FORMAT_ABGR_2101010_32BPP
   0x00000000,                                     // HAL_MDP_PIXEL_FORMAT_RGBX_1010102_32BPP
   0x00000000,                                     // HAL_MDP_PIXEL_FORMAT_RGBA_1010102_32BPP
   0x00000000,                                     // HAL_MDP_PIXEL_FORMAT_BGRX_1010102_32BPP
   0x00000000,                                     // HAL_MDP_PIXEL_FORMAT_BGRA_1010102_32BPP

   0x00000000,                                     // HAL_MDP_PIXEL_FORMAT_YCRYCB_H2V1_16BPP
   0x00000000,                                     // HAL_MDP_PIXEL_FORMAT_YCBYCR_H2V1_16BPP
   0x00000000,                                     // HAL_MDP_PIXEL_FORMAT_CRYCBY_H2V1_16BPP
   0x00000000,                                     // HAL_MDP_PIXEL_FORMAT_CBYCRY_H2V1_16BPP
   0x00000000,                                     // HAL_MDP_PIXEL_FORMAT_CRYCB_H1V1_24BPP 
   0x00000000,                                     // HAL_MDP_PIXEL_FORMAT_AYCBCR_H1V1_32BPP
   0x00000000,                                     // HAL_MDP_PIXEL_FORMAT_ACRCBY_H1V1_32BPP

   0x00000000,                                     // HAL_MDP_PIXEL_FORMAT_Y_CBCR_H2V2_12BPP
   0x00000000,                                     // HAL_MDP_PIXEL_FORMAT_Y_CRCB_H2V2_12BPP
   0x00000000,                                     // HAL_MDP_PIXEL_FORMAT_Y_CRCB_H2V1_16BPP
   0x00000000,                                     // HAL_MDP_PIXEL_FORMAT_Y_CBCR_H2V1_16BPP
   0x00000000,                                     // HAL_MDP_PIXEL_FORMAT_Y_CBCR_H2V2_VC1_12BPP
   0x00000000,                                     // HAL_MDP_PIXEL_FORMAT_Y_CRCB_H2V2_VC1_12BPP
   0x00000000,                                     // HAL_MDP_PIXEL_FORMAT_Y_CBCR_SUPERTILE_4x2_12BPP
   0x00000000,                                     // HAL_MDP_PIXEL_FORMAT_Y_CBCR_VENUS_4x4_12BPP
   0x00000000,                                     // HAL_MDP_PIXEL_FORMAT_Y_CRCB_H2V2_P010_24BPP
   0x00000000,                                     // HAL_MDP_PIXEL_FORMAT_Y_CRCB_H2V2_TP10_16BPP

   0x00000000,                                     // HAL_MDP_PIXEL_FORMAT_Y_CR_CB_H2V2_12BPP
   0x00000000,                                     // HAL_MDP_PIXEL_FORMAT_Y_CR_CB_H2V1_16BPP
   0x00000000,                                     // HAL_MDP_PIXEL_FORMAT_Y_CB_CR_H2V2_12BPP
   0x00000000,                                     // HAL_MDP_PIXEL_FORMAT_Y_CB_CR_H2V1_16BPP
};

/****************************************************************************
*
** FUNCTION: HAL_MDP_GetSourceFormatInfo()
*/
/*!
* \brief
*     Returns the source format register programming information
*
* \param [in] psSurface           - Surface Information
*
* \retval uint32
*
****************************************************************************/
uint32 HAL_MDP_GetSourceFormatInfo(HAL_MDP_SurfaceAttrType   *psSurface)
{
   uint32 uSourceFormatInfo = 0;

  if (psSurface->ePixelFormat < HAL_MDP_PIXEL_FORMAT_MAX)
  {
    uSourceFormatInfo = guSrcFormatInfo[psSurface->ePixelFormat];

    if ((HAL_MDP_FLAGS_SURFACE_ATTR_MACRO_TILE & psSurface->uFlags) && (NULL != pgsMdpHwInfo))
    {
      uSourceFormatInfo &=~(HWIO_MDP_WB_2_DST_FORMAT_FRAME_FORMAT_BMSK);
      uSourceFormatInfo |= ((pgsMdpHwInfo->eMacroTileFormat)<< HWIO_MDP_WB_2_DST_FORMAT_FRAME_FORMAT_SHFT);
    }

    // Use pseudo planar parameters for planar format if the surface is compressed
    if (0x00 != (HAL_MDP_FLAGS_SURFACE_COMPRESSED & psSurface->uFlags))
    {
      if(HAL_MDP_CHECK_PSEUDO_PLANAR_PIXEL_FORMAT(psSurface->ePixelFormat))
      {
        if (TRUE == psSurface->uPlanes.sCompressedPlane.bChromaVerticalPacked)
        {
          uSourceFormatInfo = HWIO_OUT_FLD( uSourceFormatInfo,
                                            MDP_VP_0_VIG_0_SSPP_SRC_FORMAT,
                                            SRC_CHROMA_SAMP,
                                            HAL_MDP_SRC_FORMAT_CHROMA_SAMPLE_YUV422_H1V2);
        }
      }
    }
    else
    {
      // Check for H1V2 pixel format
      if(HAL_MDP_CHECK_PSEUDO_PLANAR_PIXEL_FORMAT(psSurface->ePixelFormat))
      {
        if(TRUE == psSurface->uPlanes.sYuvPsuedoPlanar.bChromaVerticalPacked)
        {
          if((HAL_MDP_PIXEL_FORMAT_Y_CRCB_H2V1_16BPP == psSurface->ePixelFormat) ||
             (HAL_MDP_PIXEL_FORMAT_Y_CBCR_H2V1_16BPP == psSurface->ePixelFormat))
          {   
            uSourceFormatInfo = HWIO_OUT_FLD( uSourceFormatInfo,
                                              MDP_VP_0_VIG_0_SSPP_SRC_FORMAT,
                                              SRC_CHROMA_SAMP,
                                              HAL_MDP_SRC_FORMAT_CHROMA_SAMPLE_YUV422_H1V2);
          }
        }
      }
      else if(HAL_MDP_CHECK_PLANAR_PIXEL_FORMAT(psSurface->ePixelFormat))
      {
        if(TRUE == psSurface->uPlanes.sYuvPlanar.bChromaVerticalPacked)
        {
          if((HAL_MDP_PIXEL_FORMAT_Y_CR_CB_H2V1_16BPP == psSurface->ePixelFormat) ||
             (HAL_MDP_PIXEL_FORMAT_Y_CB_CR_H2V1_16BPP == psSurface->ePixelFormat))
          {   
            uSourceFormatInfo = HWIO_OUT_FLD( uSourceFormatInfo,
                                              MDP_VP_0_VIG_0_SSPP_SRC_FORMAT,
                                              SRC_CHROMA_SAMP,
                                              HAL_MDP_SRC_FORMAT_CHROMA_SAMPLE_YUV422_H1V2);
          }
        }  
      }
	  
      // Use pseudo planar parameters for planar format if the surface is compressed
      if (0x00 != (HAL_MDP_FLAGS_SURFACE_COMPRESSED & psSurface->uFlags))
      {
        if(HAL_MDP_CHECK_PLANAR_PIXEL_FORMAT(psSurface->ePixelFormat))
        {
           uSourceFormatInfo = HWIO_OUT_FLD(uSourceFormatInfo,
                                            MDP_VP_0_VIG_0_SSPP_SRC_FORMAT,
                                            FETCH_PLANES,
                                            HAL_MDP_SRC_FORMAT_FETCH_PLANES_PSUEDO_PLANAR);
           uSourceFormatInfo = HWIO_OUT_FLD(uSourceFormatInfo,
                                            MDP_VP_0_VIG_0_SSPP_SRC_FORMAT,
                                            UNPACK_COUNT,
                                            HAL_MDP_SRC_FORAMT_UNPACK_COMPONENT_COUNT2);
           uSourceFormatInfo = HWIO_OUT_FLD(uSourceFormatInfo,
                                            MDP_VP_0_VIG_0_SSPP_SRC_FORMAT,
                                            SRC_BPP,
                                            HAL_MDP_SRC_FORMAT_SRC_TWO_BPP);
        }
     } 

    }

  }

  

  return uSourceFormatInfo;
}

/****************************************************************************
*
** FUNCTION: HAL_MDP_GetDestinationFormatInfo()
*/
/*!
* \brief
*     Returns the destination format register programming information
*
* \param [in] psSurface           - Surface Information
*
* \retval uint32
*
****************************************************************************/
uint32 HAL_MDP_GetDestinationFormatInfo(HAL_MDP_SurfaceAttrType   *psSurface)
{
  uint32 uDestinationFormatInfo = HAL_MDP_GetSourceFormatInfo(psSurface);
  if((1 << HWIO_MDP_VP_0_VIG_0_SSPP_SRC_FORMAT_UNPACK_DX_FORMAT_SHFT) & uDestinationFormatInfo )
  {
    uDestinationFormatInfo &=~(HWIO_MDP_VP_0_VIG_0_SSPP_SRC_FORMAT_UNPACK_DX_FORMAT_BMSK);
    uDestinationFormatInfo |= (HWIO_MDP_WB_2_DST_FORMAT_PACK_DX_FORMAT_BMSK);
  }

  return uDestinationFormatInfo;
}


#ifdef __cplusplus
}
#endif
