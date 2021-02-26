#ifndef DANGERGEN_H
#define DANGERGEN_H

/*============================================================================*/
/**
 @file dangergen.h

 @brief This file contains data types, constants that make up the
        Danger Driver public API.

 DESCRIPTION:
     Public Function Declarations 
     This is the Danger Driver client API. 

 EXTERNALIZED FUNCTIONS:
     See below.
*/
/*==============================================================================
  Copyright (c) 2009-2011, 2013-2015 Qualcomm Technologies, Inc.
  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  Export of this technology or software is regulated
  by the U.S. Government, Diversion contrary to U.S. law prohibited.
==============================================================================*/

/*==============================================================================
                             Edit History

$Header: //components/rel/core.qdsp6/2.1.c4/api/power/dangergen.h#1 $

                           EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when         who     what, where, why
--------     ---     ---------------------------------------------------------
01/22/15    Kingt    Initial version
==============================================================================*/

#include "DALStdDef.h"

#ifdef __cplusplus
extern "C" {
#endif

/// Marker for unspecified time value
#define DANGERGEN_UNSPECIFIED_TIME 0

/// Opaque type for danger driver handle
typedef void* dangergen_handle_type;

/// Danger driver result codes
typedef enum
{
    DANGERGEN_SUCCESS = 0,                  ///< Successful operation
    DANGERGEN_ERROR = 1,                    ///< Generic error
    DANGERGEN_ERROR_UNINITIALIZED = 2,      ///< Driver hasn't been initialized
    DANGERGEN_ERROR_UNSUPPORTED = 3,        ///< Unsupported request
    DANGERGEN_ERROR_BADPARAM = 4,           ///< Bad input parameter(s)
    DANGERGEN_ERROR_INVALID_STATE = 5,      ///< Invalid state
    // etc...
} dangergen_result_type;

/// Enumeration for danger generation methods.  Currently only the
/// counter-based / watchdog method is supported.
typedef enum
{
    DANGERGEN_METHOD_WATCHDOG = 0,          ///< Counter-based watchdog
    // DANGERGEN_METHOD_LATENCY = 1,
    // DANGERGEN_METHOD_FW = 2,
} dangergen_generator_method_type;

/// Enumeration for danger generation enable/disable modes.  
typedef enum
{
    DANGERGEN_ENABLE_MODE_SW = 0,         
    DANGERGEN_ENABLE_MODE_HW = 1,
    DANGERGEN_ENABLE_MODE_MAX,
} dangergen_generator_enable_mode;

/// Enumeration for PORTID needed to map interrupt ID on danger HW > Napali
typedef enum _dangergen_watchdog_portid
{
  DANGERGEN_PORTID_UNKNOWN = 0,
  DANGERGEN_WCD_SLIMBUS_PGD0,
  DANGERGEN_WCD_SLIMBUS_PGD1,
  DANGERGEN_WCD_SLIMBUS_PGD2,
  DANGERGEN_WCD_SLIMBUS_PGD3,
  DANGERGEN_WCD_SLIMBUS_PGD4,
  DANGERGEN_WCD_SLIMBUS_PGD5,
  DANGERGEN_WCD_SLIMBUS_PGD6,
  DANGERGEN_WCD_SLIMBUS_PGD7,
  DANGERGEN_WCD_SLIMBUS_PGD8,
  DANGERGEN_WCD_SLIMBUS_PGD9,
  DANGERGEN_WCD_SLIMBUS_PGD10,
  DANGERGEN_WCD_SLIMBUS_PGD11,
  DANGERGEN_WCD_SLIMBUS_PGD12,
  DANGERGEN_WCD_SLIMBUS_PGD13,
  DANGERGEN_WCD_SLIMBUS_PGD14,
  DANGERGEN_WCD_SLIMBUS_PGD15,
  DANGERGEN_WCD_SLIMBUS_PGD16,
  DANGERGEN_WCD_SLIMBUS_PGD17,
  DANGERGEN_WCD_SLIMBUS_PGD18,
  DANGERGEN_WCD_SLIMBUS_PGD19,
  DANGERGEN_WCD_SLIMBUS_PGD20,
  DANGERGEN_WCD_SLIMBUS_PGD21,
  DANGERGEN_WCD_SLIMBUS_PGD22,
  DANGERGEN_WCD_SLIMBUS_PGD23,

  DANGERGEN_QCA_SLIMBUS_PGD0,
  DANGERGEN_QCA_SLIMBUS_PGD1,
  DANGERGEN_QCA_SLIMBUS_PGD2,
  DANGERGEN_QCA_SLIMBUS_PGD3,
  DANGERGEN_QCA_SLIMBUS_PGD4,
  DANGERGEN_QCA_SLIMBUS_PGD5,
  DANGERGEN_QCA_SLIMBUS_PGD6,
  DANGERGEN_QCA_SLIMBUS_PGD7,
  DANGERGEN_QCA_SLIMBUS_PGD8,
  DANGERGEN_QCA_SLIMBUS_PGD9,
  DANGERGEN_QCA_SLIMBUS_PGD10,
  DANGERGEN_QCA_SLIMBUS_PGD11,
  DANGERGEN_QCA_SLIMBUS_PGD12,
  
  DANGERGEN_LPA_IF_RD_DMA0,
  DANGERGEN_LPA_IF_RD_DMA1,
  DANGERGEN_LPA_IF_RD_DMA2,
  DANGERGEN_LPA_IF_RD_DMA3,
  DANGERGEN_LPA_IF_RD_DMA4,
  DANGERGEN_LPA_IF_RD_DMA5,
  
  DANGERGEN_LPA_IF_WR_DMA0,
  DANGERGEN_LPA_IF_WR_DMA1,
  DANGERGEN_LPA_IF_WR_DMA2,
  DANGERGEN_LPA_IF_WR_DMA3,
  DANGERGEN_LPA_IF_WR_DMA4,
  DANGERGEN_LPA_IF_WR_DMA5,
  
  DANGERGEN_DP_DMA0,
  DANGERGEN_DP_DMA1,
  DANGERGEN_DP_DMA2,
  DANGERGEN_DP_DMA3,
  
  DANGERGEN_DM_Lite,
  
  DANGERGEN_RESAMPLER0,
  DANGERGEN_RESAMPLER1,
  
  DANGERGEN_PORTID_MAX
}dangergen_watchdog_portid;

/// Parameters for counter-based danger generation
typedef struct
{
    /// Expected processing period, in ns.  This must be specified by the user.
    uint32 period_ns;
    /// Threshold for first danger level, in nanoseconds.  This must be
    /// specified by the user.
    uint32 threshold1_ns;
    /// Threshold for second danger level, in nanoseconds.  This may be set to
    /// the desired threshold, or it can be set to DANGERGEN_UNSPECIFIED_TIME
    /// and the driver will set threshold automatically based on the first
    /// threshold.
    uint32 threshold2_ns;
    /// Threshold for third danger level, in nanoseconds.  This may be set to
    /// the desired threshold, or it can be set to DANGERGEN_UNSPECIFIED_TIME
    /// and the driver will set threshold automatically based on the first
    /// threshold.
    uint32 threshold3_ns;
    /// Measurement window, in nanoseconds.  This determines the maximum time
    /// that danger may be asserted.  After the window time expires, the HW
    /// will automatically de-assert danger.
    uint32 window_ns;
    /// Port Id Map
    dangergen_watchdog_portid port_id;
} dangergen_watchdog_params_type;

/// Structure for paramaters passed to dangergen_open() function
typedef struct
{
    /// Optional username used to identify the user.  This is used for
    /// informational purposes only.
    const char* username;
    /// Specifies the method of danger generation to be used
    dangergen_generator_method_type method;
} dangergen_open_params_type;

///
/// Initialization function for the danger driver.  This must be called
/// during system initialization, before any other danger driver APIs are used.
///
dangergen_result_type dangergen_driver_init(void);

///
/// Initialization function for a dangergen_open_params_type structure.  All
/// fields of the structure will be set to reasonable defaults.
///
/// @param params : Pointer to dangergen_open_params_type structure to be
///                 initialized
///
dangergen_result_type dangergen_init_open_params(
    dangergen_open_params_type* params);

///
/// Open a new handle to the danger driver.
///
/// @param params : Pointer to parameters for the open operation
/// @param handle_out : Pointer to the user's handle.  The handle value will be
///                     written here if the open operation is successful.
///
/// @return DANGERGEN_SUCCESS if the open operation completed successfully,
///         DANGERGEN_UNSUPPORTED if the requested danger generation method is
///              not supported
///
dangergen_result_type dangergen_open(dangergen_open_params_type* params,
    dangergen_handle_type* handle_out);

///
/// Initialization function for a dangergen_watchdog_params_type structure.
/// All fields of the structure will be set to reasonable defaults.  After this
/// functions is called, the user only needs to specify the expected processing
/// period (period_ns), and the desired threshold for the first danger level
/// (threshold1_ns); all other fields may be left as default values.
///
/// @param params : Pointer to dangergen_open_params_type structure to be
///                 initialized
///
dangergen_result_type dangergen_init_watchdog_params(
    dangergen_watchdog_params_type* params);

///
/// Configure the danger generator.
///
/// This function operates with no internal locking.
///
/// @param h : Handle to the danger driver
/// @param params : Pointer to dangergen_watchdog_params_type structure
///                 containing the user-specified configuration parameters
///
/// @return DANGERGEN_SUCCESS if the configuration operation completed
///             successfully,
///         DANGERGEN_ERROR_INVALID_STATE if the current state is not valid
///
dangergen_result_type dangergen_configure_watchdog(dangergen_handle_type h,
    dangergen_watchdog_params_type* params);

///
/// Enable the danger generator.  Once enabled, the generator's counter will
/// start.
///
/// This function operates with no internal locking.
///
/// @param h : Handle to the danger driver
///
/// @return DANGERGEN_SUCCESS if the enable operation completed successfully,
///         DANGERGEN_ERROR_INVALID_STATE if the current state is not valid
///
dangergen_result_type dangergen_enable(dangergen_handle_type h);

///
/// Switch danger generator mode.  By default, danger generator is always programmed as SW
/// HW mode should be explicitly programmed between each enable/disable calls. 
/// No need to call dangergen_enable if dangergen_switch_mode() call is executed
/// This function operates with no internal locking.
///
/// @param h : Handle to the danger driver
///      mode: HW-based or SW-based mode
/// @return DANGERGEN_SUCCESS if the enable operation completed successfully,
///         DANGERGEN_ERROR_INVALID_STATE if the current state is not valid
///
dangergen_result_type dangergen_switch_mode(dangergen_handle_type h, dangergen_generator_enable_mode mode);
///
/// Disable the danger generator.  Once disabled, the generator's counter will
/// be stopped.
///
/// This function operates with no internal locking.
///
/// @param h : Handle to the danger driver
///
/// @return DANGERGEN_SUCCESS if the disable operation completed successfully,
///         DANGERGEN_ERROR_INVALID_STATE if the current state is not valid
///
dangergen_result_type dangergen_disable(dangergen_handle_type h);

///
/// Refresh the danger generator.  This is used to signal that the user is
/// operating properly (out of danger).  This causes a subtraction from the
/// watchdog counter.  The danger generator must be enabled before making a
/// refresh request.
///
/// This function operates with no internal locking.
///
/// @param h : Handle to the danger driver
///
/// @return DANGERGEN_SUCCESS if the refresh operation completed successfully,
///         DANGERGEN_ERROR_INVALID_STATE if the current state is not valid
///
dangergen_result_type dangergen_refresh(dangergen_handle_type h);

///
/// Close an existing handle to the danger driver.  Upon successful
/// completion of the close operation, the handle becomes invalid and must no
/// longer be used.
///
/// @param h : The handle to be closed
///
/// @return DANGERGEN_SUCCESS if the close operation completed successfully
///
dangergen_result_type dangergen_close(dangergen_handle_type h);

#ifdef __cplusplus
}
#endif

#endif


