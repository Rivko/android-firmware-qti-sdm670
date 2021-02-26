#ifndef __IPE_STRIPING_LIB_H__
#define __IPE_STRIPING_LIB_H__

//-------------------------------------------------------------------------
// Copyright (c) 2015-2017 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
//------------------------------------------------------------------------

//------------------------------------------------------------------------
// @file  ipeStripingLib.h
// @brief API for IPE striping library
//------------------------------------------------------------------------

//------------------------------------------------------------------------
// @Edit History
// when              who           what,where,why
// -----             ---           -------------
//
// 03/22/2017        etsairi       Initial version
//------------------------------------------------------------------------

/*------------------------------------------------------------------------
*       Include Files
* ----------------------------------------------------------------------- */
#include "stripingLib.h"
#include "ipe_data.h"

#ifdef __cplusplus
extern "C" {
#endif

/*------------------------------------------------------------------------
*       Type Declarations
* ----------------------------------------------------------------------- */

#pragma pack(push)
#pragma pack(4)

/**< Striping library input struct */
typedef struct _IPEStripingLibExecuteParams
{
    const IpeIQSettings*    iq;                     /**< Image quality settings */
    const IcaParameters*    ica1;                   /**< ICA 1 parameters */
    const IcaParameters*    ica2;                   /**< ICA 2 parameters */
    const IpeZoomWindow*    zoom;                   /**< Current zoom window parameters */
    const IpeZoomWindow*    prevZoom;               /**< Previous zoom window parameters */
    uint32_t                maxNumOfCoresToUse;     /**< Number of IPE cores used */
}IPEStripingLibExecuteParams;

/**< Striping library meta data output struct */
typedef struct _IPEStripingLibExecuteMetaData
{
    uint32_t                pixelCount;             /**< number of pixels processed on the worst case (busiest) core. */
} IPEStripingLibExecuteMetaData;

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
STRIPING_LIB_API int32_t IPEStripingLibraryContextCreate(const IpeConfigIoData* configIO,
                                                         StripingLibMemOp* mem,
                                                         uint32_t titanVersion,
                                                         uint32_t hwVersion,
                                                         SL_HANDLE* handle,
                                                         uint32_t* bufferSize,
                                                         uint32_t* cdmBufferSize);

/**
*  @brief   Calculates the stripes (main algorithm function)
*
*  @param [in,out] handle           Striping library Context handle
*  @param [in]     exe              Execution parameters for striping lib
*  @param [in,out] outputBuffer     buffer that will contains the striping library
*                                   output, as it will be passed to the FW.
*  @param [out]    metaDataBuffer   (Optional) buffer that will contain striping meta data
*
*  @return 0 in case of success, otherwise failed.
*
*  @note User is in charge of the allocation and free of outBuffer. IPEStripingLibraryExecute() only
*        assumes that buffer is at least of the requires size as defined in the
*        output of IPEStripingLibraryContextCreate().
*
*  @details Function calculates the striping for a given image, the output will be placed in outputBuffer.
*/
STRIPING_LIB_API int32_t IPEStripingLibraryExecute(SL_HANDLE handle,
                                                   const IPEStripingLibExecuteParams* exe,
                                                   void* outputBuffer,
                                                   IPEStripingLibExecuteMetaData* metaDataBuffer);

/**
*  @brief Destroy the striping library context.
*
*  @param [in,out] handle       Striping library Context handle
*
*  @return 0 in case of success, otherwise failed.
*
*  @details Free all data the was allocated by IPEStripingLibraryContextCreate()
*           and clears all other data in context.
*/
STRIPING_LIB_API int32_t IPEStripingLibraryContextDestroy(SL_HANDLE* handle);

#ifdef __cplusplus
}
#endif

#endif /* __IPE_STRIPING_LIB_H__ */
