#ifndef __P2P_EXPORTS_H__
#define __P2P_EXPORTS_H__

/*============================================================================
                           P2PExports.h

DESCRIPTION:  Function definitions for the P2P lib (libqcp2p).

Copyright (c) 2011,2015 Qualcomm Technologies, Inc.  All Rights Reserved.
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
  Define the max number of TX channels supported by the library.
*/
#define QC_US_P2P_MAX_TX_CHANNELS       4

/**
 Maximum number of users that the P2P library can track
*/
#define QC_US_P2P_MAX_USERS             96

/**
 * Distance value meaning "user not present"
*/
#define QC_US_P2P_DISTANCE_NOT_PRESENT  -1

#define SSID_LEN_BYTES 32
#define PASSWORD_LEN_BYTES 16
#define VENDOR_DATA_LEN_BYTES 48

/**
  P2P library return status
*/
typedef enum
{
  QC_US_P2P_LIB_STATUS_SUCCESS     =  0,   /* Success */
  QC_US_P2P_LIB_STATUS_FAILURE     =  1,   /* General failure */
  QC_US_P2P_LIB_STATUS_BAD_PARAMS  =  2,   /* Bad parameters */
} QcUsP2PLibStatusType;


// Positioning Result
typedef enum
{
  QC_US_P2P_POS_IDLE      =  0,   /* No status for frame */
  QC_US_P2P_POS_UPDATED   =  1,   /* Status available for frame */
} QcUsP2PPosResult;

// Data Transfer Result
typedef enum
{
  QC_US_P2P_DATA_XFER_IDLE      =  0,   /* No status for frame */
  QC_US_P2P_DATA_XFER_UPDATED   =  1,   /* Status available for frame */
} QcUsP2PDataXferResult;

// Configuration
typedef struct
{
  uint32_t TxTransparentDataSize;
  char TxTransparentData[];
} P2PCfg;

// User position
typedef struct
{
  int16_t azimuth;
  int16_t elevation;
  int16_t distance;  // -1 denotes not present
} P2PUserPos;

// Message
typedef struct
{
  char sSID[SSID_LEN_BYTES];
  char password[PASSWORD_LEN_BYTES];
  char vendorData[VENDOR_DATA_LEN_BYTES];
} P2PDataTransferMsg;

//  The output of the P2P algorithm.
typedef struct
{
  uint16_t            posResult;
  uint16_t            dataXferResult;
  P2PUserPos          userPos[QC_US_P2P_MAX_USERS];
  P2PDataTransferMsg  msg;
} P2POutput;

/*----------------------------------------------------------------------------
  Function Declarations
----------------------------------------------------------------------------*/

/*==============================================================================
  FUNCTION:  QcUsP2PLibGetSizes
===============================================================================*/
/*
 * Returns the sizes of buffers required by the P2P algorithm.
 *
 * @param pConfig[in]              The configuration of the algorithm to be used.
 * @param pWorkspaceSize[out]      The memory required by P2P algorithm in bytes.
 * @param pPatternSizeSamples[out] The pattern size to be used in sampels (16 bit).
 *
 * @return 0 for success, 1 for failure.
 */
extern int QcUsP2PLibGetSizes(P2PCfg const *pCfg,
                              uint32_t *pWorkspaceSize,
                              uint32_t *pPatternSizeSamples);

/*==============================================================================
  FUNCTION:  QcUsP2PLibInit
==============================================================================*/
/*
 * Initializes the P2P algorithm.
 *
 * @param pConfig[in]       The configuration of the algorithm.
 * @param pWorkspace[in]    Scratch buffer to be used by the algorithm.
 * @param workspaceSize[in] Size of the scratch buffer.
 *
 * @return 0 for success, 1 for failure.
 */
extern int QcUsP2PLibInit(P2PCfg const *pCfg,
                          int8_t *pWorkspace,
                          uint32_t workspaceSize);

/*==============================================================================
  FUNCTION:  QcUsP2PLibGetPattern
==============================================================================*/
/*
 * Returns the pattern that would be used by the algorithm.
 *
 * @param pPattern[in]   Buffer to which the pattern will be written.
 * @param patternSize[in] Size of given buffer in samples (16 bit).
 *
 * @return 0 for success, 1 for failure.
 */
extern int QcUsP2PLibGetPattern(int16_t *pPattern,
                                uint32_t patternSize);

/*==============================================================================
  FUNCTION:  QcUsP2PLibEngine
==============================================================================*/
/*
 * P2P algorithm to be called for each US frame.
 *
 * @param pSamplesBuffer[in] The buffer of samples to be used by the algorithm
 * @param pP2P[out]      The algorithm's decision
 *
 * @return 0 for success, 1 for failure.
 */
extern int QcUsP2PLibEngine(int16_t const *pSamplesBuffer,
                            P2POutput *pP2P);

/*==============================================================================
  FUNCTION:  QcUsP2PLibIsBusy
==============================================================================*/
/*
 * Check if the library is currently busy processing frames.
 * The LPASS P2P sub-service uses this as a hint to stop giving the library
 * buffers to prcess while it is busy. That way the library will have much less
 * "stale" buffers in the queue when it exits the busy loop.
 * It is safe to call this function from any thread, but only call it after library is
 * initialized.
 *
 * @param pIsBusy [out] Fill with busy flag, 0=not busy 1=busy
 *
 * @return 0 for success, 1 for failure.
 */
extern int QcUsP2PLibIsBusy(int* pIsBusy);

#ifdef __cplusplus
}
#endif

#endif // P2P_EXPORTS_H
