/*
 * Copyright (c) 2016, Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 */

#ifndef _QTI_GEF_DATA_TYPES_H_
#define _QTI_GEF_DATA_TYPES_H_

#include <system/audio.h>
#include <hardware/audio_effect.h>

typedef void gef_handle_t;

/*
 * This enum defines the values for calibration type
 * ASM:- Module is present in ASM on ADSP
 * ADM:- Module is present in ADM on ADSP
 */
typedef enum {
    ASM = 0,
    ADM
} calibration_type;

/*
 * This declares various event types that
 * can be sent through the callback
 *
 * EFFECT_ENABLED     indicates that another effect got enabled
 * DEVICE_CONNECTED   device field of event_value indicates which device
 *                    and the channel map is indicated in channel_map field
 *                    in event_value
 */
typedef enum {
    EFFECT_ENABLED,
    DEVICE_CONNECTED
} event_type;

/*
 * This declares the values corresponding to the event types
 * that can be sent through the callback
 *
 * value            Indicates whether it's effect on or off.
 *                  This field is relevant only if EFFECT_ENABLED is set
 * effect_uuid      The UUID of the effect that got enabled.
 *                  This field is relevant only if EFFECT_ENABLED is set
 * device           Indicates the device connected.
 *                  This field is relevant only when DEVICE_CONNECTED is set
 * channel_mask     Indicates channel mask. This field is relevant only
 *                  when DEVICE_CONNECTED is set.
 * acdb_dev_id      Indicates acdb device id. This field is relevant only
 *                  when DEVICE_CONNECTED is set.
 */
typedef struct {
    int                   value;         // indicates error
    effect_uuid_t         effect_uuid;   // will be relevant only for EFFECT_ENABLED
    audio_devices_t       device;        // indicates the device connected
    audio_channel_mask_t  channel_mask;  // indicates the channel map
    int                   sample_rate;   // indicates the sample rate
    int                   acdb_dev_id;   // corresponding acdb device id
} event_value;

/*
 * This declares the prototype for the configuration parameters of the effect
 *
 * appType    AppType to which the calibration needs to be applied to.
 * deviceId   ACDB device id. This field will be ignored when calibration
 *            is sent to a topology not on ADM
 * topoId     Topology id of the module on DSP
 * calType    Calibration type enum to specify where the module
 *            is placed. ASM/ADM etc
 * debugFlags FLAGS indicating what all debugging parameters are enabled
 *            For now, only supported value is 1.
 * persist    To indicate that the values set have to be cached in ACDB
 */
typedef struct {
    int              app_type;
    int              device_id;
    int              sample_rate;
    int              topo_id;
    calibration_type cal_type;
    uint32_t         module_id;
    uint32_t         param_id;
    bool             persist;
} effect_config_params;

/*
 * This declares the prototype for the structure in which ACDB data will be filled in
 * during a _get_param call
 *
 * data      pointer to the buffer of size length that contains the parameters
 * length    length of the filled buffer
 */
typedef struct {
    void* const data;
    const int   length;
} effect_data_in;


/*
 * This declares the prototype for the structure in which ACDB
 * data will be filled in during a _get_param call
 *
 * data    pointer to the buffer of size length that contains the parameters
 * length  length of the allocated buffer. When returning from the call, the
 *         length will contain the length of the filled buffer
 */
typedef struct {
    void* data;
    int*  length;
} effect_data_out;


/*
 * This declares the prototype for function pointer that effect
 * implementation registers with GEF
 */
typedef void (*gef_func_ptr)(void*, event_type, event_value);

#endif /* _QTI_GEF_DATA_TYPES_H_ */
