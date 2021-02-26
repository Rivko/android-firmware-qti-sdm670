/** @file ClockLibNull.c
  
  Stub functions for ClockLib

  Copyright (c) 2014, Qualcomm Technologies, Inc. All rights reserved.
**/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 06/15/16   kpa     Initial revision

=============================================================================*/


/*==========================================================================

                               INCLUDE FILES

===========================================================================*/
#include "DisplayLib.h"


/*==========================================================================

                                  DEFINES

===========================================================================*/

/*===========================================================================
                      FUNCTION DECLARATIONS
===========================================================================*/ 
uint8 data_buffer[8];

/****************************************************************************
*
** FUNCTION: DisplayInitialize()
*/
/*!
* \brief
*   This function will initialize the display and return the frame buffer that is currently being scanned out from.
*
* \param [out] pFrameBufferInfo   - Information regarding the display frame buffer
*        [in]  uFlags             - Reserved
*
* \retval EFI_STATUS
*
****************************************************************************/
MDP_Status DisplayInitialize(
  DisplayLibBufferInfoType     *pFrameBufferInfo,
  uint32                        uFlags)
{
  pFrameBufferInfo->pBufferAddr=&data_buffer[0];
  pFrameBufferInfo->uWidth = 1080;
  pFrameBufferInfo->uHeight = 1920;
  return MDP_STATUS_OK;
}

  /****************************************************************************
*
** FUNCTION: DisplayBlt()
*/
/*!
* \brief
*   This function will perform BLT operation between Blt buffer and frame buffer.
*
*  \param [in] pFrameBufferInfo   - Frame buffer info
*         [in] uFrameBufferRoiX   - X coordinate of destination for the BltBuffer.
*         [in] uFrameBufferRoiY   - Y coordinate of destination for the BltBuffer.
*         [in] eBltOperation      - Operation to perform on BlitBuffer and frame buffer
*         [in] pBltBufferInfo     - Blt buffer info, it should be always in ARGB8888.
*         [in] uBLTRoiX           - X coordinate of source for the BltBuffer.
*         [in] uBLTRoiY           - Y coordinate of source for the BltBuffer.
*         [in] uRoiWidth          - Width of rectangle in BltBuffer in pixels.
*         [in] uRoiHeight         - Hight of rectangle in BltBuffer in pixels.
*         [in] uFlags             - Reserved
*
* \retval EFI_STATUS
*
****************************************************************************/
MDP_Status DisplayBlt(
  DisplayLibBufferInfoType     *pFrameBufferInfo,
  uint32                        uFrameBufferRoiX,
  uint32                        uFrameBufferRoiY,
  DisplayLibBltOperationType    eBltOperation,
  DisplayLibBufferInfoType     *pBltBufferInfo,
  uint32                        uBLTRoiX,
  uint32                        uBLTRoiY,
  uint32                        uRoiWidth,
  uint32                        uRoiHeight,
  uint32                        uFlags)
{
  return MDP_STATUS_OK;
}  
