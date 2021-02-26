////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2013-2019 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __IS_INTERFACE_H__
#define __IS_INTERFACE_H__

#include <stdint.h>
#include <stdbool.h>
#include "NcLibWarpCommonDef.h"
#include "is_common_intf.h"

#define NUM_MATRIX_ELEMENTS        9
#define NUM_DG_MATRIX_ELEMENTS     2
#define IS_DG_FILTER_PARAM_LENGTH  16
#define EIS_STENGTH_MAX_ENTRIES    10
#define MAX_LDC_LUT_SIZE           1024
#define MAX_IS_DG_MESH_X           64
#define MAX_IS_DG_MESH_Y           96
#define MAX_IS_DG_NUM_VERTICES     ((MAX_IS_DG_MESH_X + 1)*(MAX_IS_DG_MESH_Y+1))
#define IS_DG_GRID_MAX_W           (MAX_IS_DG_MESH_X+1)
#define IS_DG_GRID_MAX_H           (MAX_IS_DG_MESH_Y+1)

#if defined(__GNUC__)
#define EIS_VISIBILITY_PUBLIC __attribute__ ((visibility ("default")))
#define EIS_VISIBILITY_LOCAL  __attribute__ ((visibility ("hidden")))
#else
#define EIS_VISIBILITY_PUBLIC __declspec(dllexport)
#define EIS_VISIBILITY_LOCAL
#endif // defined(__GNUC__)


/* Return Values */
#define IS_RET_BASE                 0x08000000
#define IS_RET_SUCCESS              (0)                         /**< Success return value */
#define IS_RET_FRAME_NOT_PROCESSES  (IS_RET_BASE + 1)           /**< Frame was not processed */
#define IS_RET_GENERAL_ERROR        (IS_RET_BASE + 2)           /**< General error return value */
#define IS_RET_INVALID_INPUT        (IS_RET_BASE + 3)           /**< Invalid input */
#define IS_RET_OUT_OF_MEMORY        (IS_RET_BASE + 4)           /**< Out of memory error */


#ifdef __cplusplus
extern "C" {
#endif

#pragma pack(push)
#pragma pack(4)

typedef bool boolean;

/** Location of camera enumerator */
typedef enum CameraPosition_t
{
    CAM_REAR      = 0,
    CAM_FRONT     = 1,
    CAM_REAR_AUX  = 2,
    CAM_FRONT_AUX = 3,
    CAM_EXTERNAL  = 4
} CameraPosition;


typedef struct EIS_GENERAL_TAG
{
    // Extra margin that allows the IS algorithm to operate on a margin that is larger than the actual physical margin provided by the sensor. (sum of both sides)
    // format: float
    float virtual_margin;

    // Focal  length in pixel unitsFocal length in pixel units for 1080p.  This value is scaled for other resolutions.
    // format: 16u
    uint32_t focal_length;

    // Gyro sampling frequency in Hz
    // format: 16u
    uint32_t gyro_frequency;

    // Gyro noise floor. Effective for EIS2 only.
    // format: float
    float gyro_noise_floor;

    // Scale factor to convert degree of rotation (yaw and pitch) to pixels. Effective for EIS2 only.
    // format: float
    float gyro_pixel_scale;

    // Reserved parameter 1
    // format: float
    float res_param_1;

    // Reserved parameter 2
    // format: float
    float res_param_2;

    // Reserved parameter 3
    // format: float
    float res_param_3;

    // Reserved parameter 4
    // format: float
    float res_param_4;

    // Reserved parameter 5
    // format: float
    float res_param_5;

    // Reserved parameter 6
    // format: float
    float res_param_6;

    // Reserved parameter 7
    // format: float
    float res_param_7;

    // Reserved parameter 8
    // format: float
    float res_param_8;

    // Reserved parameter 9
    // format: float
    float res_param_9;

    // Reserved parameter 10
    // format: float
    float res_param_10;

    // Reserved Lut parameter 1
    // format: float
    float res_lut_param_1[32];

    // Reserved Lut parameter 2
    // format: float
    float res_lut_param_2[32];
} EIS_GENERAL;


typedef struct EIS_TIMING_TAG
{
    // offset to adjust the 3D shake gyro interval start/end times to better align with the frame. (Offset between gyro timing and sof timing in ms)
    // format: 32s
    int32_t s3d_offset_1;

    // offset to adjust the 3D shake gyro interval start/end times to better align with the frame. (Offset between gyro timing and sof timing in ms)
    // format: 32s
    int32_t s3d_offset_2;

    // offset to adjust the 3D shake gyro interval start/end times to better align with the frame. (Offset between gyro timing and sof timing in ms)
    // format: 32s
    int32_t s3d_offset_3;

    // offset to adjust the 3D shake gyro interval start/end times to better align with the frame. (Offset between gyro timing and sof timing in ms)
    // format: 32s
    int32_t s3d_offset_4;

    // threshold whereby exposure times above and below this threshold results in a different offset getting applied to the 3D shake gyro time interval.
    // Thresholding for deciding s3d_offset based on integration time in seconds)
    // format: float
    float s3d_threshold_1;

    // threshold whereby exposure times above and below this threshold results in a different offset getting applied to the 3D shake gyro time interval.
    // Thresholding for deciding s3d_offset based on integration time in seconds)
    // format: float
    float s3d_threshold_2;

    // threshold whereby exposure times above and below this threshold results in a different offset getting applied to the 3D shake gyro time interval.
    // Thresholding for deciding s3d_offset based on integration time in seconds)
    // format: float
    float s3d_threshold_3;

    // threshold whereby exposure times above and below this threshold results in a different offset getting applied to the 3D shake gyro time interval.
    // Thresholding for deciding s3d_offset based on integration time in seconds)
    // format: float
    float s3d_threshold_4_ext;

    // offset to adjust the 3D shake gyro interval start/end times to better align with the frame. (Offset between gyro timing and sof timing in ms)
    // format: 32s
    int32_t rs_offset_1;

    // offset to adjust the 3D shake gyro interval start/end times to better align with the frame. (Offset between gyro timing and sof timing in ms)
    // format: 32s
    int32_t rs_offset_2;

    // offset to adjust the 3D shake gyro interval start/end times to better align with the frame. (Offset between gyro timing and sof timing in ms)
    // format: 32s
    int32_t rs_offset_3;

    // offset to adjust the 3D shake gyro interval start/end times to better align with the frame. (Offset between gyro timing and sof timing in ms)
    // format: 32s
    int32_t rs_offset_4;

    // threshold whereby exposure times above and below this threshold results in a different offset getting applied to the 3D shake gyro time interval.
    // Thresholding for deciding s3d_offset based on integration time in seconds)
    // format: float
    float rs_threshold_1;

    // threshold whereby exposure times above and below this threshold results in a different offset getting applied to the 3D shake gyro time interval.
    // Thresholding for deciding s3d_offset based on integration time in seconds)
    // format: float
    float rs_threshold_2;

    // threshold whereby exposure times above and below this threshold results in a different offset getting applied to the 3D shake gyro time interval.
    // Thresholding for deciding s3d_offset based on integration time in seconds)
    // format: float
    float rs_threshold_3;

    // threshold whereby exposure times above and below this threshold results in a different offset getting applied to the 3D shake gyro time interval.
    // Thresholding for deciding s3d_offset based on integration time in seconds)
    // format: float
    float rs_threshold_4_ext;
} EIS_TIMING;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// mesh_one_entry_t
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct mesh_one_entry_t
{
    float x_in;
    float y_in;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// mesh_t
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct mesh_t
{
    int32_t block_w;
    int32_t block_h;
    uint32_t input_width;
    uint32_t input_height;
    uint32_t output_width;
    uint32_t output_height;
    int32_t type; // 0 for pixel shifts, 1 for 3x3 matrix
    struct mesh_one_entry_t mapping[IS_DG_GRID_MAX_H][IS_DG_GRID_MAX_W];
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// eis_strength_one_entry_type
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct eis_strength_one_entry_type
{
    /// Exposure time of triggering the entry of the LUT
    float trigger_start;

    /// Exposure time of triggering the entry of the LUT
    float trigger_end;

    /// EIS strength
    float strength;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// eis_strength_type
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct eis_strength_type
{
    /// Exposure time is applied by this IIR filter to avoid sudden change of eis strength
    float temporal_filter;

    /// Number of LUT entries
    int32_t number_of_entries;

    /// interpolation entry for EIS strength
    struct eis_strength_one_entry_type eis_strength_entries[EIS_STENGTH_MAX_ENTRIES];
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// mvDGTC_tuning_type
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct mvDGTC_tuning_type
{
    /// Obsoleted
    uint32_t filter_type;

    /// Obsoleted
    uint32_t ldc_en;

    /// Obsoleted
    uint32_t rotation_relative_coord;

    /// Obsoleted
    uint32_t horizon_level_correction;

    /// parameters for low-pass filtering
    float filter_param[IS_DG_FILTER_PARAM_LENGTH];

    /// original size of ldc_r2_lut
    float ldc_r2_lut_size;

    /// 1D lens distortion model that maps pinhole to distorted image
    float ldc_r2_lut[MAX_LDC_LUT_SIZE];

    /// size of ldc_inv_r2_lut
    float ldc_inv_r2_lut_size;

    /// 1D inverse lens model
    float ldc_inv_r2_lut[MAX_LDC_LUT_SIZE];

    /// LDC mesh
    struct mesh_t ldc_mesh;

    /// EIS lowlight strength tuning parameters
    struct eis_strength_type eis_strength;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// mvDGTC_imu_calib_type
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct mvDGTC_imu_calib_type
{
    /// camera sensor pixel size in um
    float pixel_size_in_um;

    /// lens focal length in mm based on EIS use case
    float focal_length_in_mm;

    /// unit of accelerometer sensor
    uint32_t acc_unit;

    /// Description:   unit of gyro sensor
    uint32_t gyro_unit;

    /// this value will be added to gyro timestamps to solve timestamps mismatch between gyro and image frame
    int32_t constant_delay_off_pts_in_us;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// DGTC_data_type
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct DGTC_data_type
{
    /// rotation matrix which converts imu coordinate to camera coordinate
    float rotation[9];

    /// imu calibration data
    struct mvDGTC_imu_calib_type imu_calib;

    /// imu sampling rate
    uint32_t imu_data_sampling_rate;

    /// margin for frame-based image stabilization
    float fbis_margin;

    /// margin for rolling shutter correction
    float rsc_margin;

    /// EIS DG algorithm tuning parameters
    struct mvDGTC_tuning_type tuning_param;
}DGTC_DATA;

/** Chromatix configuration data structure */
typedef struct EIS_Chromatix_TAG
{
    EIS_GENERAL general;
    EIS_TIMING timing;
    DGTC_DATA dgtc_data;
} is_chromatix_info_t;

/** EIS algorithm type */
typedef enum
{
    IS_TYPE_NONE,
    IS_TYPE_DIS,
    IS_TYPE_GA_DIS,
    IS_TYPE_EIS_1_0,
    IS_TYPE_EIS_2_0,
    IS_TYPE_EIS_3_0,
    IS_TYPE_EIS_DG
} cam_is_type_t;

/** EIS matrix type */
typedef enum {
    IS_MATRIX_TYPE_2X1,
    IS_MATRIX_TYPE_3X3,
}is_matrix_t;

typedef struct
{
    double      data[3];    /**< 0==x, 1==y, 2==z , angular velocity [Rad/sec] */
    uint64_t    ts;         /**< time stamp in microseconds */
} gyro_sample_t;

typedef struct
{
    uint32_t        num_elements;   /**< number of elements in the buffer */
    gyro_sample_t*  gyro_data;      /**< gyro data */
} gyro_data_t;

/** Gyro interval data structure */
typedef struct
{
    uint64_t first_gyro_ts; /**< First gyro sample timestamps [microseconds] in a batch of gyro samples */
    uint64_t last_gyro_ts;  /**< Last gyro sample timestamps [microseconds] in a batch of gyro samples */
} gyro_times_t;

typedef struct {
  /*Full resolution details*/
  int                       full_width;          // Active array width
  int                       full_height;         // Active array height
  uint16_t                  binning_factor;
  uint16_t                  hor_binning_factor;
  /*Current sensor mode info*/
  uint32_t                  offset_x;
  uint32_t                  offset_y;
  uint32_t                  sensor_width;
  uint32_t                  sensor_height;

  /*lens info*/
  float                     focal_length;
  float                     pix_size;

  /*sensor timing info*/
  uint32_t                  ll_pck;
  uint32_t                  fl_lines;
} is_sensor_info;

/// @brief structure to contain logical ISP (IFE+IPE) window parameters for EISDG algortihm
typedef struct
{
    uint32_t  input_width;        ///< ISP input width
    uint32_t  input_height;       ///< ISP input height
    uint32_t  output_width;       //< ISP output width after scale/crop
    uint32_t  output_height;      ///< ISP output height after scale/crop
    uint32_t  isp_start_x;        ///< start x of crop wrt scale
    uint32_t  isp_start_y;        ///< start y of crop wrt scale
    uint32_t  isp_end_x;          ///< ISP scale output width
    uint32_t  isp_end_y;          ///< ISP scale output height
}is_isp_info;

/** Per sensor configuration data structure */
typedef struct is_init_data_s
{
    uint32_t is_input_frame_width;          /**< EIS input frame width */
    uint32_t is_input_frame_height;         /**< EIS input frame height */

    uint32_t readout_duration;              /**< readout duration in microseconds */

    uint32_t sensor_mount_angle;            /**< Sensor mount angle (0, 90, 180, 270) */
    CameraPosition camera_position;         /**< Camera position (front, back, etc.) */

    is_chromatix_info_t is_chromatix_info;  /**< Chromatix configurations */
} is_init_data_sensor;

/** Common configurations for all sensors data structure */
typedef struct is_init_data_common_s
{
    cam_is_type_t is_type;              /**< EIS algorithm version */

    uint32_t is_output_frame_width;     /**< EIS output frame width after virtual margin upscale */
    uint32_t is_output_frame_height;    /**< EIS output frame height after virtual margin upscale  */

    uint16_t frame_fps;                 /**< Frame rate */

    boolean is_warping_using_ica;       /**< if true then EIS warping matrix is executed in ICA. Otherwise in GPU.
                                         *
                                         *  @warning In case this value is false, IQ and performance degradation and could occur.
                                         */

    boolean do_virtual_upscale_in_matrix;   /**< if true then upscale is done by ICA matrix, otherwise by FW chosen upscaler
                                             *
                                             *  @warning In case this value is true, IQ and performance degradation and could occur.
                                             */


    boolean is_calc_transform_alignment_matrix_orig; /**< original frame alignment matrix using gyro, a matrix between origC to origP */

    boolean is_calc_transform_alignment_matrix_mctf; /**< stabilized frame alignment matrix using gyro, a final
                                                      *   matrix for MCTF matrix between stabC to stabP
                                                      */

    boolean is_sat_applied_in_ica; /**< If true, sensor alignment transform, SAT matrix will be applied in ICA along with
                                   *   EIS transform. EIS output matrices are already combined with SAT matrix on this case.
                                   *   Otherwise SAT is applied in GPU prior to calling EIS 3.x frame stabilization - eis3_process().
                                   *
                                   *  @warning In case this value is false, that is SAT is
                                   *           applied in GPU, IQ and performance degradation and could occur.
                                   */

    /*          EIS 2.x specific configurations             */

    /*          EIS 3.x specific configurations             */
    uint16_t buffer_delay;          /**< For EIS3 future looking only */
    uint16_t num_mesh_x;            /**< Number of x-matrices used for stabilization */
    uint16_t num_mesh_y;            /**< Number of y-matrices used for stabilization */

    /*          EIS DG specific configurations             */
    uint32_t prior_ldc_present;     /**< if ldc present is in pipeline */

} is_init_data_common;

#pragma pack(pop)

/* IS input data structure */
typedef struct
{
    uint32_t            frame_id;           /**< Frame ID */
    uint32_t            active_sensor_idx;  /**< Active sensor index */

    frame_times_t       frame_times;        /**< Frame time */

    gyro_data_t         gyro_data;          /**< Gyro data - gyro sample timestamps */

    WindowRegion        ifeZoomWindow;      /**< IFE zoom window. As supplied to FW. */
    WindowRegion        zoomWindow;         /**< IPE zoom window. As supplied to FW */

    boolean             is_last_frame;      /**< last frame indication. true if last frame */

    const mat3x3*       sat;                /**< Optional - if equal to NULL then EIS disregards this parameter.
                                            *
                                            * - Order of matrices operators from in->out is first SAT and then EIS.
                                            * - In order to align current sensor to previous sensor,
                                            *   use SAT matrix which is OUT2IN, thus the matrix direction is from
                                            *   previous sensor geometry to current sensor geometry.
                                            * - SAT transform shall be defined on resolution of input to EIS.
                                            * - SAT transform shall define center of image as origin (0,0).
                                            */
} is_input_t;

/** IS output data structure */
typedef struct
{
    uint32_t frame_id;              /**< Processed frame index  */
    uint32_t active_sensor_idx;     /**< Active sensor index, synced with frame_id */



    NcLibWarp stabilizationTransform; /**< Stabilization transform, as passed to NcLib
                                       *   Matrices are to synced with frame_id
                                       */

    boolean has_output;   /**< if true, then a frame was processed */
    boolean is_valid_transform_alignment_matrix_orig;           /**< If true then transform_alignment_matrix_orig is valid, otherwise invalid */
    boolean is_valid_transform_alignment_matrix_mctf;           /**< If true then transform_alignment_matrix_mctf is valid, otherwise invalid */

    float transform_alignment_matrix_orig[NUM_MATRIX_ELEMENTS]; /**< Original frame alignment matrix using gyro instead of LRME, a matrix between origC to origP */

    float transform_alignment_matrix_mctf[NUM_MATRIX_ELEMENTS]; /**< stabilized frame alignment matrix using gyro target, a final matrix for MCTF matrix between stabC to stabP */

    int num_matrices;
    float transform_matrix[(MAX_IS_DG_NUM_VERTICES) * NUM_DG_MATRIX_ELEMENTS];
    is_matrix_t matrix_type;
} is_output_type;

/** IS get stabilization margin input struct. Used by eis3_get_stabilization_margin_ex() */
typedef struct is_get_stabilization_margin_t
{
    uint32_t common_is_output_frame_height;             /**< Output frame height - Common to all sensors */
    uint32_t common_is_output_frame_width;              /**< Output frame width - Common to all sensors */
    bool     common_do_virtual_upscale_in_matrix;       /**< If true then upscale is done using matrix. Common to all sensors */

    uint32_t sensor_is_input_frame_height;              /**< Sensor input frame height */
    uint32_t sensor_is_input_frame_width;               /**< Sensor input frame width */
    float    sensor_virtual_margin;                     /**< Sensor virtual margins */
} is_get_stabilization_margin;


#ifdef __cplusplus
}
#endif

#endif /* _IS_INTERFACE_H_ */
