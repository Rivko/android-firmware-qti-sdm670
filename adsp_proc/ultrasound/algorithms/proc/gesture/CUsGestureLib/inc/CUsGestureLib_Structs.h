#ifndef C_US_GESTURE_LIB_STRUCTS_H
#define C_US_GESTURE_LIB_STRUCTS_H

/* ========================================================================
   US gesture library CAPI wrapper header file

   *//** @file CUsGestureLib_Structs.h
     This is a CAPI wrapper code for gesture detection library. The functions
     in this file are called by the proc service.

     Copyright (c) 2013 Qualcomm Technologies, Inc.
     All Rights Reserved. Qualcomm Technologies, Inc. Proprietary and Confidential.
   *//*==================================================================== */

/* ========================================================================
   Edit History

   when       who     what, where, why
   --------   ---     ----------------------------------------------------
   10/20/2013 LD      Created file.
   ======================================================================= */


/* =======================================================================
                        DEFINITIONS AND DECLARATIONS
   ======================================================================= */



#ifdef __cplusplus
extern "C" {
#include "typedef_ultrasound.h"
#endif

/**
 * this enum must match the QcUsGestureLibResultType enum from 
 * gesture detection library (QcUsGestureLibApi.h) 
 * TODO move this structure to adsp_ultrasound_media_fmt.h as it 
 * is part of our API 
 */
typedef enum
{
    GESTURE_RESULT_NOT_READY   = -1,   /* Not Ready */
    GESTURE_RESULT_IDLE        =  0,   /* No detected */
    GESTURE_RESULT_SELECT      =  1,   /* Select detected */
    GESTURE_RESULT_LEFT        =  2,   /* Left detected */
    GESTURE_RESULT_RIGHT       =  3,   /* Right detected */
    GESTURE_RESULT_UP          =  4,   /* Up detected */
    GESTURE_RESULT_DOWN        =  5,   /* Down detected */
    GESTURE_RESULT_REPEAT      =  6,   /* Please repeat */
} UsGestureLibOutputType;

// Structure for generating external event
typedef struct UsGestureExternalResult
{
	UWord32 uTimeStamp;     			// Time Stamp
	UWord32 uSeqNum;					// Sequence num
	Word32  uResult;	                // gesture library result
    Word32  uVelocity;	                // gesture library velocity result
} UsGestureExternalResult_t;

// gesture Detection result structure
typedef struct UsGestureResultType
{
   UsGestureLibOutputType uGestureResult;
   int velocity;
} UsGestureResultType_t;

//Structure to be used in INIT Params 
typedef struct
{
	UWord32 uConfigBlkSize; 			// size of structure for error check
	void*  uConfigBlk;     			 	// Encoder COnfiguratoin block or Decoder format block
} UsGestureInitParamsType_t;

#ifdef __cplusplus
}
#endif

#endif /* C_US_PROXIMITY_LIB_STRUCTS_H */
