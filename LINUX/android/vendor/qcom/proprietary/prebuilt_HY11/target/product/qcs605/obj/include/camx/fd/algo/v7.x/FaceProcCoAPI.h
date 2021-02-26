/*-------------------------------------------------------------------*/
/*  Copyright(C) 2003-2017                                           */
/*  All Rights Reserved.                                             */
/*                                                                   */
/*   This source code is the Confidential and Proprietary Property   */
/*   of the Licensor.  Any unauthorized use, reproduction or         */
/*   transfer of this software is strictly prohibited.               */
/*                                                                   */
/*-------------------------------------------------------------------*/
/* 
    FACEPROC_SDK Library API
*/
#ifndef FACEPROCCOAPI_H__
#define FACEPROCCOAPI_H__

#define FACEPROC_API

#include "FaceProcCoDef.h"

#ifndef FACEPROC_DEF_HCOMMON
#define FACEPROC_DEF_HCOMMON
    typedef VOID*   HCOMMON;
#endif /* FACEPROC_DEF_HCOMMON */

#ifdef  __cplusplus
extern "C" {
#endif

/************************************************************/
/* Get Version                                              */
/************************************************************/
/* Get Version */
FACEPROC_API INT32      FACEPROC_CO_GetVersion(UINT8 *pucMajor, UINT8 *pucMinor);

/************************************************************/
/* Common Function Handle Creation/Deletion                 */
/************************************************************/
/* Creation */
FACEPROC_API HCOMMON    FACEPROC_CO_CreateHandle(void);

FACEPROC_API HCOMMON    FACEPROC_CO_CreateHandleMalloc(void *(*malloc)(size_t size),
                                               void (*free)(void *));

FACEPROC_API HCOMMON    FACEPROC_CO_CreateHandleMemory(VOID *pBMemoryAddr, UINT32 unBMemorySize,
                                               VOID *pWMemoryAddr, UINT32 unWMemorySize);
/* Deletion */
FACEPROC_API INT32      FACEPROC_CO_DeleteHandle(HCOMMON hCO);

/************************************************************/
/* Square Points to Center-Form                             */
/************************************************************/
/* Conversion from Square Points to Center-Form */
FACEPROC_API INT32      FACEPROC_CO_ConvertSquareToCenter(POINT ptLeftTop,
                                                  POINT ptRightTop,
                                                  POINT ptLeftBottom,
                                                  POINT ptRightBottom,
                                                  POINT *pptCenter,
                                                  INT32 *pnSize,
                                                  INT32 *pnAngle);

/************************************************************/
/* Center-Form to Square Points                             */
/************************************************************/
/* Convertsion from Center-Form to Square Points */
FACEPROC_API INT32      FACEPROC_CO_ConvertCenterToSquare(POINT ptCenter,
                                                  INT32 nSize,
                                                  INT32 nAngle,
                                                  POINT *pptLeftTop,
                                                  POINT *pptRightTop,
                                                  POINT *pptLeftBottom,
                                                  POINT *pptRightBottom);

#ifdef  __cplusplus
}
#endif

#endif  /* FACEPROCCOAPI_H__ */
