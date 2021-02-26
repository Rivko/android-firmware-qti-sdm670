#ifndef _SURROUND_REC_INTERFACE_H_
#define _SURROUND_REC_INTERFACE_H_
/*
 * Copyright (c) 2014, 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

/**
 * @file surround_rec_interface.h
 *
 * This header file declares the interface to the surround sound
 * recording algorithm.
 */

/* Error codes */
#define SURROUND_REC_SUCCESS             (0)
#define SURROUND_REC_ERR_INVALID_PARAM   (-101)
#define SURROUND_REC_ERR_NOMEM           (-102)

typedef char *get_param(void *handle);
typedef void set_param(void *handle, const char *value);

typedef struct get_param_data {
    const char *name;
    get_param *get_param_fn;
} get_param_data_t;

typedef struct set_param_data {
    const char *name;
    set_param *set_param_fn;
} set_param_data_t;

/**
 * Retrieve parameter data for surround sound recording
 * "get" parameters.
 *
 * @return Pointer to array of get_param_data_t structures, terminated by NULL.
 */
const get_param_data_t* surround_rec_get_get_param_data(void);

/**
 * Retrieve parameter data for surround sound recording
 * "set" parameters.
 *
 * @return Pointer to array of set_param_data_t structures, terminated by NULL.
 */
const set_param_data_t* surround_rec_get_set_param_data(void);

/**
 * Initialize surround sound recording algorithm.
 *
 * @param[out] handle
 *    Pointer to dynamically allocated surround sound data.
 *
 * @param[in] num_in_chan
 *    Number of input channels
 *
 * @param[in] num_out_chan
 *    Number of output channels
 *
 * @param[in] frame_len
 *    Number of samples per frame for one channel
 *
 * @param[in] sample_rate
 *    Sampling rate in Hz
 *
 * @param[in] config_file_name
 *    Path to config file
 *
 * @return 0, or error code
 */
int surround_rec_init(void **handle, int num_in_chan,
                      int num_out_chan, int frame_len, int sample_rate,
                      const char *config_file_name);

/**
 * Deinit surround sound recording algorithm, freeing allocated memory.
 *
 * @param[out] handle
 */
void surround_rec_deinit(void *handle);

/**
 * Surround sound recording process.
 *
 * @param[in/out] handle (from surround_rec_init function)
 * @param in_pcm[in] interleaved 16-bit PCM data
 * @param out_pcm[out] interleaved 16-bit PCM output
 */
void surround_rec_process(void *handle, const int16_t *in_pcm, int16_t *out_pcm);

#endif /* #ifndef _SURROUND_REC_INTERFACE_H_ */
