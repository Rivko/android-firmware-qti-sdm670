#ifndef __PROXIMITY_EXPORTS_H__
#define __PROXIMITY_EXPORTS_H__

/*============================================================================
                           ProximityExports.h

DESCRIPTION:  Function definitions for the Proximity lib (libqcproximity).

Copyright (c) 2012-2015 Qualcomm Technologies, Inc.  All Rights Reserved.
Qualcomm Technologies Proprietary and Confidential.
==============================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

/*----------------------------------------------------------------------------
  Type Definitions
----------------------------------------------------------------------------*/
#define STUB_VERSION "0.0.0.0"

/**
  Proximity library return status
*/
typedef enum
{
  QC_US_PROXIMITY_LIB_STATUS_SUCCESS     =  0,   /* Success */
  QC_US_PROXIMITY_LIB_STATUS_FAILURE     =  1,   /* General failure */
  QC_US_PROXIMITY_LIB_STATUS_BAD_PARAMS  =  2,   /* Bad parameters */
} QcUsProximityLibStatusType;

/**
  Proximity library outcome type
*/
typedef enum
{
  QC_US_PROXIMITY_LIB_RESULT_NO_CHANGE =  -2,   /* Bypass update*/
  QC_US_PROXIMITY_LIB_RESULT_NOT_READY =  -1,  /* Not ready */
  QC_US_PROXIMITY_LIB_RESULT_IDLE      =  0,   /* No Proximity */
  QC_US_PROXIMITY_LIB_RESULT_DETECTED  =  1,   /* Proximity */
  QC_US_PROXIMITY_LIB_MAX_RESULT       =  0x1000, /* Max result from lib */
  QC_US_PROXIMITY_KEEP_ALIVE,                    /* keep alive (from daemon) */
} QcUsProximityLibResultType;

/**
  Configurations for the proximity algorithm
*/
typedef struct
{
  uint32_t TxTransparentDataSize;
  char TxTransparentData[];
} ProximityCfg;

/**
  The output of the proximity algorithm
*/
typedef struct
{
  QcUsProximityLibResultType proximity;
  int distance;
} ProximityOutput;

/*----------------------------------------------------------------------------
  Function Declarations
----------------------------------------------------------------------------*/
/*==============================================================================
  FUNCTION:  QcUsProximityLibGetSizes
==============================================================================*/
/**
 * Returns the sizes of buffers required by the proximity algorithm.
 *
 * @param pConfig[in]              The configuration of the algorithm to be used.
 * @param pWorkspaceSize[out]      The memory required by proximity algorithm in bytes.
 * @param pPatternSizeSamples[out] The pattern size to be used in samples (16 bit).
 *
 * @return 0 for success, 1 for failure.
 */
extern int QcUsProximityLibGetSizes(ProximityCfg const *pConfig,
                                    uint32_t *pWorkspaceSize,
                                    uint32_t *pPatternSizeSamples);

/*==============================================================================
  FUNCTION:  QcUsProximityLibInit
==============================================================================*/
/**
 * Initializes the proximity algorithm. 2nd call sets persistent data.
 *
 * @param pConfig[in]       The configuration of the algorithm.
 * @param pWorkspace[in]    Scratch buffer to be used by the algorithm.
 * @param workspaceSize[in] Size of the scratch buffer.
 *
 * @return 0 for success, 1 for failure.
 */
extern int QcUsProximityLibInit(ProximityCfg const *pConfig,
                                int8_t *pWorkspace,
                                uint32_t workspaceSize);

/*==============================================================================
  FUNCTION:  QcUsProximityLibGetPattern
==============================================================================*/
/**
 * Returns the pattern that would be used by the algorithm.
 *
 * @param pPattern[in]    Buffer to which the pattern will be written.
 * @param patternSize[in] Size of given buffer in samples (16 bit).
 *
 * @return 0 for success, 1 for failure.
 */
extern int QcUsProximityLibGetPattern(int16_t *pPattern,
                                      uint32_t patternSize);

/*==============================================================================
  FUNCTION:  QcUsProximityLibEngine
==============================================================================*/
/**
 * Proximity algorithm to be called for each US frame.
 *
 * @param pSamplesBuffer[in]  The buffer of samples to be used by the algorithm
 * @param pProximity[out]     The algorithm's decision
 *
 * @return 0 for success, 1 for failure.
 */
extern int QcUsProximityLibEngine(int16_t const *pSamplesBuffer,
                                  ProximityOutput *pProximity);

/*==============================================================================
  FUNCTION:  QcUsProximityLibSetDynamicConfig
==============================================================================*/
/**
 * Sets dynamic configuration to the proximity library according to the
 * application configuration.
 *
 * @param pDynamicConfig [in]    config buffer
 * @param dynamicConfigSize [in] config buffer length in integers
 *
 * @return 0 for success, 1 for failure.
 */
extern int QcUsProximityLibSetDynamicConfig(int* pDynamicConfig,
                                            uint32_t dynamicConfigSize);

/*==============================================================================
  FUNCTION:  QcUsProximityLibGetVersion
==============================================================================*/
/**
 * Returns the current library version
 *
 * @param pVersion The library version
 *
 * @return 0 for success, 1 for failure.
 */
extern int QcUsProximityLibGetVersion(int *pVersion);

/*==============================================================================
  FUNCTION:  QcUsProximityGetPersistentData
==============================================================================*/
/**
 * Fills out the current state of persistent data
 *
 * @param pConfig[out]      The persistent data.
 * @param pWorkspace[in]    Scratch buffer to be used by the algorithm.
 * @param maxConfigSize[in] Max size of persistent data buffer in bytes.
 *
 * @return 0 for success, 1 for failure.
 */
extern int QcUsProximityGetPersistentData(int8_t *pWorkspace,
                                          ProximityCfg *pConfig,
                                          uint32_t maxConfigSize);

#ifdef __cplusplus
}
#endif

#endif //__PROXIMITY_EXPORTS_H__
