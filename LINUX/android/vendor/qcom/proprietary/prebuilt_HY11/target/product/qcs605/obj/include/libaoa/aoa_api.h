/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef _AOA_API_H
#define _AOA_API_H

#ifdef __cplusplus
extern "C"
{
#endif

/** current API version */
#define		AOALIB_API_VERSION	1

/** opaque handle to AOA algorithm library */
typedef void *aoalib_handle_t;

/** advanced AOA results for debugging and deeper analysis */
typedef struct {
	/* primary and secondary azimuth and elevation,
	 * in case of ambuiguity
	 */
	double primaryAz, primaryEl, secondaryAz, secondaryEl;

	/* ratio between primary and secondary peak (0 means no ambuiguity) */
	double peaksRatio;

	/* internal algorithm state for debugging */
	double trackState;

	/* size of the correlation matrix */
	int xcorr_rows, xcorr_cols;

	/* correlation matrix from algorithm (for demo/debugging)
	 * xcorr_rows * xcorr_cols double values
	 */
	double *xcorr;

} aoalib_adv_res_t;

/**
 * This is a struct of function pointers that defines the function
 * callbacks populated by the AOA library code and used by location framework
 * to process AOA measurements.
 */
typedef struct {
	int (*aoa_get_api_version)(void);

	/** Initialize the AOA library and allocate resources */
	aoalib_handle_t (*aoa_init)(const char *db_fname, uint32_t type);

	/** Release resources allocated for the AOA library */
	void (*aoa_deinit)(aoalib_handle_t handle);

	/** Process AOA measurements and return result */
	int (*aoa_process)(aoalib_handle_t handle, uint8_t *data, int length,
			   uint32_t type, uint32_t mask,
			   double *az, double *el, double *zpfactor);

	/** Get advanced information from last aoa_process call */
	void (*aoa_get_adv_res)(aoalib_handle_t handle, aoalib_adv_res_t *res);
} aoalib_cb_t;

/**
 * Returns a pointer to the AOA library callback table struct. Called from
 * location framework to retrieve the functions needed to process
 * AOA measurements
 *
 * @return ptr to callback table struct
 */
aoalib_cb_t *aoa_get_cb_table();

/**
 * typedef for the function above, for getting with dlsym
 */
typedef aoalib_cb_t *(*aoa_get_cb_table_func)(void);

/**
 * Initialize the AOA algorithm library and allocate resources
 *
 * db_fname - calibration database file name
 * type - AOA type to support. Values from
 *  enum qca_wlan_vendor_attr_aoa_type
 *
 * Returns an allocated handle on success
 * On failure returns NULL and sets errno
 */
aoalib_handle_t aoa_init(const char *db_fname, uint32_t type);

/**
 * Release resources allocated for AOA algorithm library
 *
 * handle - handle allocated by aoa_init
 */
void aoa_deinit(aoalib_handle_t handle);


/**
 * Run the AOA algorithm and return result.
 *
 * handle - handle allocated by aoa_init
 * data - measurement data, interpreted according to AOA measurement type,
 *  antenna array mask and calibration data
 * length - length of input data
 * type - the type of AOA measurement performed from
 *  enum qca_wlan_vendor_attr_aoa_type
 * mask - bit mask indicating which antenna arrays used in the
 *  measurement
 * az, el - result azimuth and elevation angles, will be filled on success
 * zpfactor - if not NULL, filled with reliability score for the result
 *  (between 0 and 1, higher number means results are more reliable)
 *
 * Returns 0 on success, negative value on failure
 */
int aoa_process(aoalib_handle_t handle, uint8_t *data, int length,
		uint32_t type, uint32_t mask,
		double *az, double *el, double *zpfactor);

/**
 * Get advanced information from last aoa_process call
 * Can be used for demo, debugging or deeper analysis
 * Only valid after a successful call to aoa_process
 *
 * handle - handle allocated by aoa_init
 * res - structure of advanced information, will be filled
 */
void aoa_get_adv_res(aoalib_handle_t handle, aoalib_adv_res_t *res);

#ifdef __cplusplus
} // extern "C"
#endif

#endif /* _AOA_API_H */

