#ifndef C_US_PROXIMITY_LIB_STRUCTS_H
#define C_US_PROXIMITY_LIB_STRUCTS_H

/* ========================================================================
   US proximity library CAPI wrapper header file

   *//** @file CUsProximityLib_Structs.h
     This is a CAPI wrapper code for proximity library. The functions
     in this file are called by the proc service.

     Copyright (c) 2014 Qualcomm Technologies, Inc. All Rights Reserved.
     Qualcomm Technologies, Inc. Proprietary and Confidential.
   *//*==================================================================== */

/* ========================================================================
   Edit History

   when       who     what, where, why
   --------   ---     ----------------------------------------------------
   11/12/2014 RD      Created file
   ======================================================================= */


/* =======================================================================
                        DEFINITIONS AND DECLARATIONS
   ======================================================================= */



#ifdef __cplusplus
extern "C" {
#include "typedef_ultrasound.h"
#endif

/**
 * this enum must match the QcUsProximityLibResultType enum from 
 * proximity detection library (QcUsProximityLibApi.h) 
 * TODO move this structure to adsp_ultrasound_media_fmt.h as it 
 * is part of our API 
 */
typedef enum
{
  PROXIMITY_RESULT_NOT_READY =  -1,  /* Not ready */
  PROXIMITY_RESULT_IDLE      =  0,   /* No Proximity */
  PROXIMITY_RESULT_DETECTED  =  1,   /* Proximity */
} UsProximityLibOutputType;

// Structure for generating external event
typedef struct UsProximityExternalResult
{
	UWord32 uTimeStamp;     			// Time Stamp
	UWord32 uSeqNum;					// Sequence num
	Word32  uResult;	                // proximity library result
    Word32  uDistance;	                // proximity library distance result
} UsProximityExternalResult_t;

// proximity Detection result structure
typedef struct UsProximityResultType
{
   UsProximityLibOutputType uProximityResult;
   int distance;
} UsProximityResultType_t;

//Structure to be used in INIT Params 
typedef struct
{
	UWord32 uConfigBlkSize; 			// size of structure for error check
	void*  uConfigBlk;     			 	// Encoder COnfiguratoin block or Decoder format block
} UsProximityInitParamsType_t;

#ifdef __cplusplus
}
#endif

#endif /* C_US_PROXIMITY_LIB_STRUCTS_H */
