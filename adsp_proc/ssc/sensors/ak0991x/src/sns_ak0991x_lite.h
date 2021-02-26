#pragma once
/**
 * @file sns_ak0991x_lite.h
 *
 * compile switches for SEE-Lite.
 *
 * Copyright (c) 2016-2017 Asahi Kasei Microdevices
 * All Rights Reserved.
 * Confidential and Proprietary - Asahi Kasei Microdevices
 *
 * Copyright (c) 2016-2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 **/

/*--------------------------------------------------------------------------
 *
 * COMPILE CONDITION
 *
 *-------------------------------------------------------------------------*/
// Enable below macro to set dual sensor support mode
//#define AK0991X_ENABLE_DUAL_SENSOR

/*
 * For SEE-Lite Mode, please follow the procedure.
 * TODO1: define the AK0991X_ENABLE_SEE_LITE.
 * TODO2: Select a target device.
 * TODO3: Set a DRI/FIFO/S4S mode if the target device has the feature.
 */

// TODO:1 Enable below macro to set SEE Lite mode
//#define AK0991X_ENABLE_SEE_LITE           // Enable SEE-Lite mode

#ifdef AK0991X_ENABLE_SEE_LITE
// SEE-Lite Mode. Require to select a target device and configure mode setting.

//#define AK0991X_ENABLE_REGISTRY_ACCESS    // Enable registry access
//#define AK0991X_ENABLE_ALL_ATTRIBUTES     // Enable all attribute service
//#define AK0991X_ENABLE_DEBUG_MSG          // Enable debug messages
//#define AK0991X_ENABLE_DIAG_LOGGING       // Enable diagnostic logging
//#define AK0991X_ENABLE_POWER_RAIL         // Enable power rail reference
//#define AK0991X_ENABLE_DEINIT             // Enable deinit call

//TODO:2 Please select a target device for SEE-Lite
//#define AK0991X_TARGET_AK09911
//#define AK0991X_TARGET_AK09912
//#define AK0991X_TARGET_AK09913
//#define AK0991X_TARGET_AK09915C
//#define AK0991X_TARGET_AK09915D
#define AK0991X_TARGET_AK09916C
//#define AK0991X_TARGET_AK09916D
//#define AK0991X_TARGET_AK09917
//#define AK0991X_TARGET_AK09918

// AK09913/AK09916C/AK09918 don't have FUSE/DRI/FIFO/S4S, then no need to enable them.
#if     defined(AK0991X_TARGET_AK09911)
#define AK0991X_ENABLE_FUSE
#elif   defined(AK0991X_TARGET_AK09912)
#define AK0991X_ENABLE_FUSE
#define AK0991X_ENABLE_DRI
#elif   defined(AK0991X_TARGET_AK09915C)
#define AK0991X_ENABLE_DRI
#define AK0991X_ENABLE_FIFO
#elif   defined(AK0991X_TARGET_AK09915D)
#define AK0991X_ENABLE_DRI
#define AK0991X_ENABLE_FIFO
#define AK0991X_ENABLE_S4S
#elif   defined(AK0991X_TARGET_AK09916C)
#define AK0991X_FORCE_MAX_ODR_50HZ
#elif   defined(AK0991X_TARGET_AK09916D)
#define AK0991X_ENABLE_DRI
#elif   defined(AK0991X_TARGET_AK09917)
#define AK0991X_ENABLE_DRI
#define AK0991X_ENABLE_FIFO
#define AK0991X_ENABLE_S4S
#endif

//TODO:3 Define macros for DRI/FIFO/S4S mode if the target device has the feature.
#ifdef AK0991X_ENABLE_DRI
#define AK0991X_STATE_IS_DRI  // Set for activating DRI mode
#endif //  AK0991X_ENABLE_DRI

#ifdef AK0991X_ENABLE_FIFO
#define AK0991X_STATE_USE_FIFO  // Set for activating FIFO mode
#endif //  AK0991X_STATE_USE_FIFO

#ifdef AK0991X_ENABLE_S4S
//#define AK0991X_STATE_SUPPORTS_SYNC_STREAM  // Set for activating S4S mode
#endif //  AK0991X_ENABLE_S4S


#else // AK0991X_ENABLE_SEE_LITE

// Standard SEE Mode. Enabled all features.
#define AK0991X_ENABLE_REGISTRY_ACCESS    // Enable registry access
#define AK0991X_ENABLE_ALL_ATTRIBUTES     // Enable all attribute service
//#define AK0991X_ENABLE_DEBUG_MSG          // Enable debug messages
//#define AK0991X_ENABLE_DAE                // Enable DAE
#define AK0991X_ENABLE_DIAG_LOGGING       // Enable diagnostic logging
#define AK0991X_ENABLE_POWER_RAIL         // Enable power rail reference
#define AK0991X_ENABLE_DEINIT             // Enable deinit call
//#define AK0991X_ENABLE_S4S                // Enable S4S parts
#define AK0991X_ENABLE_ALL_DEVICES        // Enable AKM all sensors
#define AK0991X_ENABLE_FUSE               // Enable fuse rom
#define AK0991X_ENABLE_DRI                // Enable DRI
#define AK0991X_ENABLE_FIFO               // Enable FIFO
//#define AK0991X_ENABLE_DUAL_SENSOR        // Enable to set dual sensor support mode
//#define AK0991X_FORCE_MAX_ODR_50HZ        // Force MAX ODR to 50Hz

// this is setting for enable DEVICE_MODE_SENSOR
#define AK0991X_ENABLE_DEVICE_MODE_SENSOR // Enable devise_mode_sensor

#ifdef AK0991X_ENABLE_DEVICE_MODE_SENSOR
#define MAX_DEVICE_MODE_SUPPORTED 2       // change number 2/4/8 in order to match the faccal num in the registry
#else
#define MAX_DEVICE_MODE_SUPPORTED 1
#endif

#endif	// AK0991X_ENABLE_SEE_LITE
