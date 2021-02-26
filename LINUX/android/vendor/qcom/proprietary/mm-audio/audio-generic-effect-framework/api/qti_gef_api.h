/*
 * Copyright (c) 2016, Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef _QTI_GEF_API_H_
#define _QTI_GEF_API_H_

#include <qti_gef_datatypes.h>

/*
 * This function is used to register an effect session with GEF
 *
 * effectId  Id of the effect that will be enabled using this session
 * returns   valid handle if the session creation was succesful
 *           NULL if there is an error
 */
gef_handle_t* gef_register_session(effect_uuid_t effectId);

/*
 * This function is used to register the session with GEF
 *
 * handle     Handle to session to deregister
 * returns    0: Operation is successful
 *            EINVAL: When the session is invalid/handle is null etc
 */
int gef_deregister_session(gef_handle_t* handle);

/*
 * This function is used to indicate that the effect is enabled
 *
 * handle     Handle to session to deregister
 * returns    0: Operation is successful
 *            EINVAL: When the session is invalid/handle is null etc
 */
int gef_enable_effect(gef_handle_t* handle);

/*
 * This function is used to indicate that the effect is disabled.
 *
 * handle     Handle to session to deregister
 * returns    0: Operation is successful
 *            EINVAL: When the session is invalid/handle is null etc
 */
int gef_disable_effect(gef_handle_t* handle);

/*
 * This function is used to query the version number supported in GEF
 *
 * majorVersion   Major version number
 * minorVersion   Minor version number
 * returns        0: Operation is successful
 *                EINVAL: When the arguments are NULL
 */
int gef_query_version(int *majorVersion, int *minorVersion);

/*
 * This function is to be called by vendor abstraction layer to GEF to
 * register a callback. This callback will be invoked by GEF
 * under these circumstances
 *
 * 1. Error when sending a calibration
 * 2. Another effect which was also registered through GEF got enabled
 * 3. A device is connected to notify which device is connected
 *    and the channel map associated
 *
 * handle   handle to GEF
 * cb       pointer to callback
 * data     data that will be sent in the callback
 * returns  0: Operation is successful
 *          EINVAL: When the session is invalid/handle is null etc
 */
int gef_register_callback(gef_handle_t* handle, gef_func_ptr cb, void* data);

/*
 * This method sends the parameters that will be sent to the DSP
 * The parameters are automatically stored in ACDB cache.
 *
 * handle   Handle to GEF
 * params   Parameters of the effect
 * data     data corresponding to the effect
 * returns  0: Operation is successful
 *          EINVAL: When the session is invalid/handle is null etc
 *          ENODEV: When GEF has not been initialized
 *          ENOSYS: Sending parameters is not supported
 */
int gef_set_param_v1(gef_handle_t* handle,
        effect_config_params* params,
        effect_data_in* data);

/*
 * This api will retrieve the entire data binary from the DSP
 * for the module and parameter ids mentioned.
 *
 * Vendor abstraction layer is expected to allocate memory for
 * the buffer into which the data corresponding to the parameters
 * will be stored
 *
 * THIS API IS NOT SUPPORTED FOR NOW.
 * API will always return ENOSYS
 *
 * handle   Handle to GEF
 * params   Parameters of the effect
 * data     data corresponding to the effect
 * returns  ENOSYS: Retrieving parameters is not supported
 */
int gef_get_param_v1(const gef_handle_t* handle,
        effect_config_params* params,
        effect_data_out* data);

/*
 * This method sends the parameters that are to be stored in ACDB.
 *
 * handle   Handle to GEF
 * params   Parameters of the effect
 * data     data corresponding to the effect
 * returns  0: Operation is successful
 *          EINVAL: When the session is invalid/handle is null etc
 *          ENODEV: When GEF has not been initialized
 *          ENOSYS: Sending parameters is not supported
 */
int gef_store_cacheparam_v1(gef_handle_t* handle,
        effect_config_params* params,
        effect_data_in* data);


/*
 * This method retrieves the parameters that are stored in cache.
 * Vendor abstraction layer is expected to allocate memory for
 * the buffer into which the data corresponding to the parameters
 * will be stored
 *
 * THIS API IS NOT SUPPORTED FOR NOW
 * API will always return ENOSYS
 *
 * handle   Handle to GEF
 * params   Parameters of the effect
 * data     data corresponding to the effect
 * returns  ENOSYS: Retrieving parameters is not supported
 */
int gef_retrieve_cacheparam_v1(const gef_handle_t* handle,
        effect_config_params* params,
        effect_data_out* data);

#endif /* _QTI_GEF_API_H_ */
