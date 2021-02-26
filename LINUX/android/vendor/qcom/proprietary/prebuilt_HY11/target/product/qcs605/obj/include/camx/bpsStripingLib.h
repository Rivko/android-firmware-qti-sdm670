#ifndef __BPS_STRIPING_LIB_H__
#define __BPS_STRIPING_LIB_H__

//-------------------------------------------------------------------------
// Copyright (c) 2015-2017 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
//------------------------------------------------------------------------

//------------------------------------------------------------------------
// @file  bpsStripingLib.h
// @brief API for BPS striping library
//------------------------------------------------------------------------

//------------------------------------------------------------------------
// @Edit History
// when              who           what,where,why
// -----             ---           -------------
//
// 04/06/2017        etsairi       Initial version
//------------------------------------------------------------------------

/*------------------------------------------------------------------------
*       Include Files
* ----------------------------------------------------------------------- */

#include "stripingLib.h"
#include "bps_data.h"

#ifdef __cplusplus
extern "C" {
#endif

/*------------------------------------------------------------------------
*       Type Declarations
* ----------------------------------------------------------------------- */

#pragma pack(push)
#pragma pack(4)

/**< Striping library input struct */
typedef struct _BPSStripingLibExecuteParams
{
    const BpsIQSettings*    iq;                     /**< Image quality settings */
    uint32_t                maxNumOfCoresToUse;     /**< Number of BPS cores used */
}BPSStripingLibExecuteParams;

/**< Striping library meta data output struct */
typedef struct _BPSStripingLibExecuteMetaData
{
    uint32_t                pixelCount;             /**< number of pixels processed on the worst case (busiest) core. */
} BPSStripingLibExecuteMetaData;

#pragma pack(pop)

/*------------------------------------------------------------------------
*       API Declarations
* ----------------------------------------------------------------------- */

/**
*  @brief   Creates a striping library context.
*
*  @param [in]  configIO        Holds image information
*  @param [in]  mem             Memory allocation functions for the context,
*                               if NULL, stdlib.h malloc() and free() will be used.
*  @param [in]  titanVersion    Titan Chip version, as read from the HW.
*  @param [in]  hwVersion       Titan HW version, as read from the HW.
*  @param [out] handle          context handle
*  @param [out] bufferSize      Buffer sizes required for StripingLibraryIpeRun()
*  @param [out] cdmBufferSize   Buffer size for CDM program buffer passed to process frame call.
*
*  @return 0 in case of success, otherwise failed.
*
*  @details Functions allocates striping library context and initializes at using the input parameters.
*           Then it calculates an upper bound on the total number of stripes required and returns
*           in bufferSize the total buffer size that needs to be allocated in the shared memory.
*/
STRIPING_LIB_API int32_t BPSStripingLibraryContextCreate(const BpsConfigIoData* configIO,
                                                         StripingLibMemOp* mem,
                                                         uint32_t titanVersion,
                                                         uint32_t hwVersion,
                                                         SL_HANDLE* handle,
                                                         uint32_t* bufferSize,
                                                         uint32_t* cdmBufferSize);

/**
*  @brief   Calculates the stripes (main algorithm function)
*
*  @param [in,out] handle       Striping library Context handle
*  @param [in]     exe          Execution parameters for striping lib
*  @param [in,out] outputBuffer buffer that will contains the striping library
*                               output, as it will be passed to the FW.
*  @param [out]    metaDataBuffer   (Optional) buffer that will contain striping meta data
*
*  @return 0 in case of success, otherwise failed.
*
*  @note User is in charge of the allocation and free of outBuffer. BPSStripingLibraryExecute() only
*        assumes that buffer is at least of the requires size as defined in the
*        output of BPStripingLibraryContextCreate().
*
*  @details Function calculates the striping for a given image, the output will be placed in outputBuffer.
*/
STRIPING_LIB_API int32_t BPSStripingLibraryExecute(SL_HANDLE handle,
                                                   const BPSStripingLibExecuteParams* exe,
                                                   void* outputBuffer,
                                                   BPSStripingLibExecuteMetaData* metaDataBuffer);

/**
*  @brief Destroy the striping library context.
*
*  @param [in,out] handle       Striping library Context handle
*
*  @return 0 in case of success, otherwise failed.
*
*  @details Free all data the was allocated by BPSStripingLibraryContextCreate()
*           and clears all other data in context.
*/
STRIPING_LIB_API int32_t BPSStripingLibraryContextDestroy(SL_HANDLE* handle);

#ifdef __cplusplus
}
#endif

#endif /* __BPS_STRIPING_LIB_H__ */
