#ifndef __HALMDPLIB_H__
#define __HALMDPLIB_H__
/*=============================================================================
 
  File: HALMDPLib.h
 
  Header file for common MDP driver HAL functions
  
 
  Copyright (c) 2011-2014 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
=============================================================================*/
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include <Uefi.h>
#include "MDPTypes.h"


/*===========================================================================

                        Public Functions

===========================================================================*/

/* Configure the interface connection between channel (DMA)  and physical interface (LCDC/DSI/HDMI) */
EFI_STATUS MDPHalConfigConnection(MDP_Panel_AttrType *pPanelInfo, MDP_MixerType eMixer);

/* Configure the source layer (RGB,VG) */
EFI_STATUS MDPHalConfigLayer(MDPLayerInfoType *pLayerInfo);

/* Apply changes to hardware and trigger DB flush */
EFI_STATUS MDPHALDeviceCommit(MDP_LayerType   eLayerSource, MDP_DMAType     eDMATarget);

/* Configure layer color fill */
EFI_STATUS MDPHalConfigColorFill(bool32 bEnable, MDP_LayerType eLayer, uint32 uColor);

/* Intiate a mixer blt operation */
void MDPHALMixerStartBlt(MDP_MixerType eMixerType);

/* Check for mixer done */
bool32 MDPHALMixerDone(MDP_MixerType eMixerType);


EFI_STATUS MDPHalSetupBlt(MDP_MixerType      eMixer,
                              MDPSurfaceInfo     *pSrcSurf,
                              MDPPointType       *pDestOffset,
                              MDPSurfaceInfo     *pDestSurf,
                              uint32              uBackgroundColor,
                              uint32              uFlags);


/* Check the status of the MDP DMA hardware */
bool32 MDPHALDeviceBusy(MDP_DMAType     eDMATarget);

/* Get interrupt status */
void MDPHALGetInterruptStatus (uint32   *pInterruptSource);

/* Clear interrupt status */
void MDPHALClrInterruptStatus (uint32 uInterruptSource);

/* Detect the MDP version */
void MDPHal_Detect (MDPPlatformIdType *pPlatformId);

/* get pixel color info */
void MDPHal_get_colorInfo (MDP_PixelFormatType  eColorFormat,
                               MDP_ColorComponentsPackingType  eColorComponentsPacking,
                               MDP_ColorComponentAlignmentType eColorComponentAlignment,
                               MDP_ColorInfoType           *peColorInfo);

/* Write to the MDP scratch register */
void MDPHALWriteScratch(uint32 uValue);



#endif // __HALMDPLIB_H__

