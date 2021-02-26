////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//// Copyright (c) 2017 Qualcomm Technologies, Inc.
//// All Rights Reserved.
//// Confidential and Proprietary - Qualcomm Technologies, Inc.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file chincsdefs.h
/// @brief Define Qualcomm Technologies, Inc. NCS related structure definition
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef CHINCSDEFS_H
#define CHINCSDEFS_H

#ifdef __cplusplus

#include "chicommon.h"

static const UINT32 ChiNodeMaxRates = 10;

extern "C"
{
#endif // __cplusplus
#pragma pack(push, CDK_PACK_SIZE)

/// @brief This defines the Chi data request structure for non camera services.
typedef struct ChiNCSDataRequest
{
    size_t        size;              ///< Size of this structure
    struct
    {
        UINT64         tStart;       ///< Start time of the request
        UINT64         tEnd;         ///< End time of the request
    } windowRequest;
    CHIFENCEHANDLE     hChiFence;    ///< Chifence handle
    UINT               numSamples;   ///< Number of samples requested, if more than 0 it is considered a sample request
    CDKResult          result;       ///< Result of this request, read synchronously by the caller
} CHINCSDATAREQUEST;

/// @brief This defines the Chi accessible data Non Camera sensor source capabilities
typedef struct ChiNCSDataSourceCaps
{
    size_t        size;                          ///< Size of this structure
    ChiDataSource type;                          ///< Chi data souce type
    FLOAT         resolutions[ChiNodeMaxRates];  ///< Array of supported resolutions.
    UINT          numResolutions;                ///< Number of valid resolutions.
    FLOAT         rates[ChiNodeMaxRates];        ///< Array of supported sampling rates.
    UINT          numRates;                      ///< Number of valid sampling rates.
    BOOL          isValid;                       ///< is Valid flag indicating validity of ths structure.
} CHINCSDATASOURCE;

/// @brief This defines the Chi data Non camera service data source config structure
typedef struct ChiNCSDataSourceConfig
{
    size_t         size;           ///< Size of this structure
    ChiDataSource  sourceType;     ///< CHI Data Source type
    BOOL           operationMode;  ///< batched mode(0) / streaming mode(1)
    FLOAT          samplingRate;   ///< Sampling rate of this sensor
    UINT           reportRate;     ///< Report rate of this sensor
} CHINCSDATASOURCECONFIG;

/// @brief Chi gyro data structure
struct ChiNCSDataGyro
{
    FLOAT  x;           ///< Angular velocity about x axis.
    FLOAT  y;           ///< Angular velocity about y axis.
    FLOAT  z;           ///< Angular velocity about z axis.
    UINT64 timestamp;   ///< Qtimer Timestamp when the sample was captured.
};

/// @brief Chi accelerometer data structure
struct ChiNCSDataAccel
{
    FLOAT  x;           ///< Acceleration about the x-axis.
    FLOAT  y;           ///< Acceleration about the y-axis.
    FLOAT  z;           ///< Acceleration about the z-axis.
    UINT64 timestamp;   ///< Qtimer Timestamp when the sample was captured.
};

#pragma pack(pop)

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // CHINCSDEFS_H