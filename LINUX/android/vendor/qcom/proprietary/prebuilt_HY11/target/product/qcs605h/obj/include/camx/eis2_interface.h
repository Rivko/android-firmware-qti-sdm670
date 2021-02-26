////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2013-2018 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __EIS2_INTERFACE_H__
#define __EIS2_INTERFACE_H__

/*------------------------------------------------------------------------
*       Include Files
* ----------------------------------------------------------------------- */

#include "is_interface.h"
#include "is_sns_lib.h"

#ifdef __cplusplus
extern "C" {
#endif

/*------------------------------------------------------------------------
*       Defines
* ----------------------------------------------------------------------- */

#define CORR2_WINDOW_LEN                16                  /**< Number of samples for correlation calculation */
#define GYRO_DATA_CYCLIC_BUFFER_SIZE    MAX_GYRO_SAMPLES    /**< Gyro FIFO number of entries */
#define EIS_2X_RSC_HIST_SIZE            16                  /**< History size for RSC */

/*------------------------------------------------------------------------
*       Type Declarations
* ----------------------------------------------------------------------- */

#pragma pack(push)
#pragma pack(4)

typedef struct
{
    int32_t     last_gyro[3];   /**< latch last gyro */
    int32_t     filt_pan[3];    /**< filtered pan */
    double      alpha[3];      /**< alpha - different for all axes */
} eis_pan_filter_s;

typedef struct
{
    int32_t x;
    int32_t y;
    int32_t z;
} eis2_position_type;

/** _eis2_context_type:
 *    @frame_num: current frame number
 *    @past_dis_offsets: recent history of DIS offsets
 *    @past_eis_offsets: recent history of EIS offsets
 *    @timestamp: timestamps of recent offsets
 *    @rolling_shutter_tform: rolling shutter correction transform
 *    @Qmat:
 *    @prev_Qnet:
 *    @Qnet:
 *    @projective_tform:
 *    @composite_tform:
 *    @bias: gyro sensor bias
 *    @kalman_params_x: Kalman filter parameters for x axis
 *    @kalman_params_y: Kalman filter parameters for y axis
 *    @alpha: pan filter coefficient
 *    @avg_gyro_stdev: filtered mean (over x, y, z) gyro standard deviation
 *    @is_shaking: flag to indicate motion
 *    @crop_factor: zoom
 *    @margin_x:
 *    @margin_y:
 *    @width:
 *    @height:
 *    @sensor_mount_angle: sensor mount angle (0, 90, 180, 270)
 *    @camera_position: camera position (front or back)
 *    @res_scale:
 *    @margin_scale: margin scale
 *    @crop_scale: crop scale
 *    @gyro_noise_floor: gyro noise floor
 *    @gyro_pixel_scale: gyro pixel scale

 *    @dis_bias_correction: Indicates whether DIS bias correction is enabled
 *    @sns_handle: sensors handle
 *    @gyro_samples_rs: gyro samples for calculating rolling shutter correction
 *       matrix
 *    @gyro_samples_3ds: gyro samples for calculating 3D shake rotation
 *       correction matrix
 *    @Rmat: for debug logging
 *    @pan: for debug logging
 *    @del_offset: for debug logging (applicable only when DIS bias correction
 *       is enabled)
 *
 * This structure maintains the EIS 2.0 algorithm context.
 **/

typedef struct eis2_context_sensor_s
{
    uint32_t cols_in;                   /**< Input frame width */
    uint32_t rows_in;                   /**< Input frame height */

    uint32_t readout_duration;          /**< readout duration in microseconds */

    int32_t focal_length;               /**< focal length in pixels */

    unsigned int sensor_mount_angle;    /**< sensor mount angle (0, 90, 180, 270) */
    CameraPosition camera_position;     /**< camera position (front or back) */
    double res_scale;                   /**< resolution scale */

    double margin_x;                    /**< margin in x direction */
    double margin_y;                    /**< margin in y direction */
    double crop_scale_using_matrix;     /**< When doing virtual margin using a matrix we need to implement the upscale in the matrix */

    uint32_t cols_crop_out;             /**< Columns crop size, before virtual margin upscale in ICA to upscaler. total_margin == in_size - crop_size */
    uint32_t rows_crop_out;             /**< Rows crop size, before virtual margin upscale in ICA to upscaler. total_margin == in_size - crop_size */

    gyro_interval_tuning_parameters_t gyro_interval_tuning_params;  /**< gyro interval tuning parameters */
}eis2_context_sensor;

#pragma pack(pop)

typedef struct
{
    uint64_t magic_start;       /**< Magic number - must be the first element */

    uint32_t frame_num;

#if 0  //TODO (etsairi): remove dead code 
    eis2_position_type past_dis_offsets[CORR2_WINDOW_LEN];
    eis2_position_type past_eis_offsets[CORR2_WINDOW_LEN];
    int64_t timestamp[CORR2_WINDOW_LEN];
    int bias[3];

    eis_kalman_type kalman_params_x;
    eis_kalman_type kalman_params_y;

    uint32_t dis_bias_correction;
    float gyro_pixel_scale;
    double del_offset[2];
    double margin_scale;
#endif /* 0 */

    mat3x3 rolling_shutter_tform;
    quaternion_type Qmat;
    quaternion_type prev_Qnet;
    quaternion_type Qnet;
    mat3x3 projective_tform;
    mat3x3 composite_tform;

    double alpha;
    double avg_gyro_stdev;
    boolean is_shaking;
    int32_t crop_factor;

    uint32_t cols_out; // final output size after scaling and virtual margin
    uint32_t rows_out;

    uint64_t prev_frame_time;

    double gyro_noise_floor;

    mat3x3 Rmat;
    double pan[3];

    double rsc_alpha;
    uint32_t rsc_last_pos;
    double rsc_hist[EIS_2X_RSC_HIST_SIZE];

    uint32_t gyro_data_buffer_cur_location;
    uint32_t gyro_data_buffer_is_full;
    int32_t gyro_data_buffer_max_valid_location;

    eis_pan_filter_s pan_filter;

    boolean is_warping_using_ica; // is set to True, use ICA for warping and adjust matrices accordingly, if set to false, use GPU to warp and set matrices accordingly
    boolean do_virtual_upscale_in_matrix;//
    boolean is_calc_transform_alignment_matrix_orig;
    boolean is_calc_transform_alignment_matrix_mctf;
    boolean is_sat_applied_in_ica;
    uint8_t active_sensor_idx;              /**< Active sensor index */
    uint8_t number_of_sensors;              /**< Number of sensors in video stream */
    eis2_context_sensor* sensor_config;     /**< array of sensor configurations */

    gyro_sample_s gyro_samples_rs[100];
    gyro_sample_s gyro_samples_3ds[100];

    gyro_sample_t gyro_data_buffer[GYRO_DATA_CYCLIC_BUFFER_SIZE];      /* gyro data cyclic buffer */
    gyro_sample_s gyro_buffer[MAX_GYRO_SAMPLES];    /**< Gyro buffer used for internal calculations */

    uint64_t gyro_last_ts;

    uint64_t magic_end; /**< Magic number - must be the last element */
} eis2_context_type;


/*------------------------------------------------------------------------
*       API Declarations
* ----------------------------------------------------------------------- */

/**
*  @brief   This function initializes the EIS 2.x algorithm, context is allocated by the user.
*
*  @param [out] eis             EIS 2.x context
*  @param [in]  init_common     Common input data to all sensors required to initialize the EIS 3.x algorithm
*  @param [in]  init_sensors    Array sensor specific input data required to initialize the EIS 3.x algorithm
*  @param [in]  num_sensors     init_sensors array size. Also number of sensors in the system, each instance
*                               requires a unique input data struct.
*
*  @return 0 in case of success, otherwise failed.
**/
EIS_VISIBILITY_PUBLIC
int eis2_initialize(
    eis2_context_type *eis,
    const is_init_data_common* init_common,
    const is_init_data_sensor *init_sensors,
    uint32_t num_sensors);

/**
*  @brief   Calculates the total margin used by EIS 2.x algorithm.
*           if (do_virtual_upscale_in_matrix == 0) stabilizationMargins = Phisical Margins + Virtual Margins
*               (on this case the Virtual Margins will be upscaled with FW interference, and be done either on upscaler or ICA)
*           else  stabilizationMargins = Phisical Margins
*               (on this case the Virtual Margins will be upscaled using matrices with no FW interference)
*
*  @note This function must be called after eis2_initialize().
*
*  @param [in]  in                      input struct
*  @param [out] stabilizationMargins    Total margin for EIS 2.x algorithm, as supplied to the FW.
*
*  @return 0 in case of success, otherwise failed.
**/
EIS_VISIBILITY_PUBLIC
int eis2_get_stabilization_margin_ex(
    const is_get_stabilization_margin* in,
    ImageDimensions* stabilizationMargins);


/**
*  @brief   Calculates the total margin used by EIS 2.x algorithm.
*           if (do_virtual_upscale_in_matrix == 0) stabilizationMargins = Phisical Margins + Virtual Margins
*               (on this case the Virtual Margins will be upscaled with FW interference, and be done either on upscaler or ICA)
*           else  stabilizationMargins = Phisical Margins
*               (on this case the Virtual Margins will be upscaled using matrices with no FW interference)
*
*  @note This function must be called after eis2_initialize().
*
*  @param [in]  eis                     EIS 2.x context
*  @param [in]  active_sensor_idx       Active sensor index
*  @param [out] stabilizationMargins    Total margin for EIS 2.x algorithm, as supplied to the FW.
*
*  @return 0 in case of success, otherwise failed.
**/
EIS_VISIBILITY_PUBLIC
int eis2_get_stabilization_margin(
    const eis2_context_type *eis,
    uint32_t active_sensor_idx,
    ImageDimensions* stabilizationMargins);

/**
*  @brief   Output the time interval for gyro data needed for this frame processing.
*           Should be called every frame after SOF, gyro output will use as input to eis2_process().
*
*  @param [in,out]  eis                 EIS 2.x context opaque handle
*  @param [in]      frame_time_input    frame time input to pre-process
*  @param [in]      active_sensor_idx   Active sensor index
*  @param [out]     gyro_req_interval   time interval of gyro samples that should be available for processing this frame
*
*  @return 0 in case of success, otherwise failed.
**/
EIS_VISIBILITY_PUBLIC
int eis2_get_gyro_time_interval(
    eis2_context_type *eis,
    const frame_times_t* frame_time_input,
    uint32_t active_sensor_idx,
    gyro_times_t *gyro_req_interval);

/**
*  @brief    Output the warpStruct in order to stabilize the image and correct rolling shutter.
*
*  @param [in,out]  eis             EIS 2.x context opaque handle
*  @param [in]      is_input        Input to EIS 2.x algorithm
*  @param [out]     is_output       Output of EIS 2.x algorithm
*
*  @return 0 in case of success, otherwise failed.
**/
EIS_VISIBILITY_PUBLIC
int eis2_process(eis2_context_type *eis, is_input_t *is_input, is_output_type *is_output);

/**
*  @brief    De-initializes the EIS 2.x algorithm context.
*
*  @param [in,out]  eis     EIS 2.x context opaque handle
*
**/
EIS_VISIBILITY_PUBLIC
int eis2_deinitialize(eis2_context_type *eis);


#ifdef __cplusplus
}
#endif

#endif
