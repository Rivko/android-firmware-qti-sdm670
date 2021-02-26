/*=============================================================================
 
  File: DisplayLib.h
 
  Display driver for XBL RAM loader
  
 
    Copyright (c) 2016 Qualcomm Technologies, Inc.
    All Rights Reserved
    Qualcomm Confidential and Proprietary
=============================================================================*/
#ifndef __DISPLAYLIB_H__
#define __DISPLAYLIB_H__

#include "MDPTypes.h"

/**********************************************************************************************************************
 * Defines
 **/

/** Defines display BLT operation type.
*/
typedef enum {
  DISPLAY_BLT_FRAMEFILL,                        /** Color fill whole frame buffer to a solid color                 */
  DISPLAY_BLT_FRAMETOBLTBUFFER,                 /** Read data from frame buffer to BLT buffer.                     */
  DISPLAY_BLT_BUFFERTOFRAME,                    /** Write data from BLT buffer to frame buffer.                    */
  DISPLAY_BLT_FRAMETOFRAME,                     /** Copy frame buffer rectangle to another position of frame buffer*/
} DisplayLibBltOperationType;

/** Defines display buffer info.
*/
typedef struct
{
  uint32               uWidth;                  /** The size of buffer in pixels in the X dimension.               */
  uint32               uHeight;                 /** The size of buffer in pixels in the Y dimension.               */
  uint32               uStride;                 /** The buffer stride.                                             */
  void                *pBufferAddr;             /** The buffer address.                                            */
} DisplayLibBufferInfoType;
 

/**********************************************************************************************************************
 *   Function Prototypes.
 **/
 
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
  uint32                        uFlags);


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
  uint32                        uFlags);


#endif // __DISPLAYLIB_H__
