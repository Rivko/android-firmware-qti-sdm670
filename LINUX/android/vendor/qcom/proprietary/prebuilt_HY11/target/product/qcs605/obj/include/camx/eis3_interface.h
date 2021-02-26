////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2016-2018 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __EIS3_INTERFACE_H__
#define __EIS3_INTERFACE_H__

/*------------------------------------------------------------------------
*       Include Files
* ----------------------------------------------------------------------- */

#include "is_interface.h"
#include "is_common_intf.h"

#ifdef __cplusplus
extern "C" {
#endif

/*------------------------------------------------------------------------
*       API Declarations
* ----------------------------------------------------------------------- */

/**
*  @brief   This function allocates memory for EIS 3.x and initializes the algorithm.
*
*  @param [out] eis3_handle     EIS 3.x context opaque handle
*  @param [in]  init_common     Common input data to all sensors required to initialize the EIS 3.x algorithm
*  @param [in]  init_sensors    Array sensor specific input data required to initialize the EIS 3.x algorithm
*  @param [in]  num_sensors     init_sensors array size. Also number of sensors in the system, each instance
*                               requires a unique input data struct.
*
*  @return 0 in case of success, otherwise failed.
**/
EIS_VISIBILITY_PUBLIC
int32_t eis3_initialize(
    void** eis3_handle,
    const is_init_data_common* init_common,
    const is_init_data_sensor* init_sensors,
    uint32_t num_sensors);

/**
*  @brief   Calculates the total margin used by EIS 3.x algorithm.
*           if (do_virtual_upscale_in_matrix == 0)
*               stabilizationMargins = Phisical Margins + Virtual Margins
*               (on this case the Virtual Margins will be upscaled with FW interference, and be done either on upscaler or ICA)
*           else
*               stabilizationMargins = Phisical Margins
*               (on this case the Virtual Margins will be upscaled using matrices with no FW interference)
*
*  @note This function must be called for each sensor separately
*
*  @param [in]  in                      input struct
*  @param [out] stabilizationMargins    Total margin for EIS 3.x algorithm, as supplied to the FW.
*
*  @return 0 in case of success, otherwise failed.
**/
EIS_VISIBILITY_PUBLIC
int32_t eis3_get_stabilization_margin_ex
(
    const is_get_stabilization_margin* in,
    ImageDimensions* stabilizationMargins
);

/**
*  @brief   Calculates the total margin used by EIS 3.x algorithm.
*               if (do_virtual_upscale_in_matrix == 0)
*                   stabilizationMargins = Phisical Margins + Virtual Margins
*                   (on this case the Virtual Margins will be upscaled with FW interference, and be done either on upscaler or ICA)
*               else
*                   stabilizationMargins = Phisical Margins
*                   (on this case the Virtual Margins will be upscaled using matrices with no FW interference)
*
*  @note This function must be called after eis3_initialize().
*
*  @param [in]  eis                     EIS 3.x context
*  @param [in]  active_sensor_idx       Active sensor index
*  @param [out] stabilizationMargins    Total margin for EIS 3.x algorithm, as supplied to the FW.
*
*  @return 0 in case of success, otherwise failed.
**/
EIS_VISIBILITY_PUBLIC
int32_t eis3_get_stabilization_margin(
    void *eis,
    uint32_t active_sensor_idx,
    ImageDimensions* stabilizationMargins);

/**
*  @brief   Output the time interval for gyro data needed for this frame processing.
*           Should be called every frame after SOF, gyro output will use as input to eis3_process().
*
*  @param [in,out]  eis3_handle         EIS 3.x context opaque handle
*  @param [in]      frame_time_input    frame time input to pre-process
*  @param [in]      active_sensor_idx   Active sensor index
*  @param [out]     gyro_req_interval   time interval of gyro samples that should be available for processing this frame
*
*  @return 0 in case of success, otherwise failed.
**/
EIS_VISIBILITY_PUBLIC
int32_t eis3_get_gyro_time_interval(
    void* eis3_handle,
    const frame_times_t* frame_time_input,
    uint32_t active_sensor_idx,
    gyro_times_t* gyro_req_interval);

/**
*  @brief    Output the warpStruct in order to stabilize the image and correct rolling shutter.
*
*  @param [in,out]  eis3_handle     EIS 3.x context opaque handle
*  @param [in]      eis3_input      Input to EIS 3.x algorithm
*  @param [out]     is_output       Output of EIS 3.x algorithm
*
*  @return 0 in case of success, otherwise failed.
**/
EIS_VISIBILITY_PUBLIC
int32_t eis3_process(void* eis3_handle, const is_input_t* eis3_input, is_output_type* is_output);

/**
*  @brief    De-initializes the EIS 3.x algorithm context.
*
*  @param [in,out]  eis3_handle     EIS 3.x context opaque handle
*
**/
EIS_VISIBILITY_PUBLIC
void eis3_deinitialize(void** eis3_handle);

#ifdef __cplusplus
}
#endif

#endif //__EIS3_INTERFACE_H__
