/**
 * @file      swvdec_api.h
 * @brief     SwVdec framework API.
 * @copyright
 *            Copyright (c) 2015,2017 Qualcomm Technologies, Inc.
 *            All Rights Reserved.
 *            Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef _SWVDEC_API_H_
#define _SWVDEC_API_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Synchronous API to initialize SwVdec library.
 *
 * SwVdec initialized with default properties and callback function pointers.
 *
 * @param[in,out] p_swvdec:   Pointer to SwVdec handle.
 * @param[in]     codec:      Codec type.
 * @param[in]     p_callback: Pointer to callback functions structure.
 *
 * @retval SWVDEC_STATUS_SUCCESS
 * @retval SWVDEC_STATUS_NULL_POINTER
 * @retval SWVDEC_STATUS_NOT_IMPLEMENTED
 * @retval SWVDEC_STATUS_FAILURE
 * @retval SWVDEC_STATUS_INVALID_PARAMETERS
 */
SWVDEC_STATUS swvdec_init(SWVDEC_HANDLE   *p_swvdec,
                          SWVDEC_CODEC     codec,
                          SWVDEC_CALLBACK *p_callback);

/**
 * @brief Synchronous API to de-initialize SwVdec library.
 *
 * @param[in] swvdec: SwVdec handle.
 *
 * @retval SWVDEC_STATUS_SUCCESS
 * @retval SWVDEC_STATUS_NULL_POINTER
 */
SWVDEC_STATUS swvdec_deinit(SWVDEC_HANDLE swvdec);

/**
 * @brief Synchronous API to set a specific decoder property.
 *
 * @param[in] swvdec:     SwVdec handle.
 * @param[in] p_property: Pointer to property structure.
 *
 * @retval SWVDEC_STATUS_SUCCESS
 * @retval SWVDEC_STATUS_NULL_POINTER
 * @retval SWVDEC_STATUS_INVALID_STATE
 * @retval SWVDEC_STATUS_INVALID_PARAMETERS
 * @retval SWVDEC_STATUS_UNSUPPORTED
 * @retval SWVDEC_STATUS_NOT_IMPLEMENTED
 */
SWVDEC_STATUS swvdec_setproperty(SWVDEC_HANDLE    swvdec,
                                 SWVDEC_PROPERTY *p_property);

/**
 * @brief Synchronous API to get a specific decoder property.
 *
 * @param[in]     swvdec:     SwVdec handle.
 * @param[in,out] p_property: Pointer to property structure.
 *
 * @retval SWVDEC_STATUS_SUCCESS
 * @retval SWVDEC_STATUS_NULL_POINTER
 * @retval SWVDEC_STATUS_INVALID_STATE
 * @retval SWVDEC_STATUS_INVALID_PARAMETERS
 */
SWVDEC_STATUS swvdec_getproperty(SWVDEC_HANDLE    swvdec,
                                 SWVDEC_PROPERTY *p_property);

/**
 * @brief Synchronous API to start decoding.
 *
 * Codec-specific resources are allocated.
 *
 * @param[in] swvdec: SwVdec handle.
 *
 * @retval SWVDEC_STATUS_SUCCESS
 * @retval SWVDEC_STATUS_NULL_POINTER
 * @retval SWVDEC_STATUS_INVALID_STATE
 * @retval SWVDEC_STATUS_FAILURE
 */
SWVDEC_STATUS swvdec_start(SWVDEC_HANDLE swvdec);

/**
 * @brief Synchronous API to stop encoding.
 *
 * Codec-specific resources are de-allocated.
 *
 * @param[in] swvdec: SwVdec handle.
 *
 * @retval SWVDEC_STATUS_SUCCESS
 * @retval SWVDEC_STATUS_NULL_POINTER
 * @retval SWVDEC_STATUS_INVALID_STATE
 * @retval SWVDEC_STATUS_FAILURE
 */
SWVDEC_STATUS swvdec_stop(SWVDEC_HANDLE swvdec);

/**
 * @brief Asynchronous API to send an input buffer to SwVdec.
 *
 * The input buffer is read and returned to client via callback function
 * pfn_empty_buffer_done().
 *
 * @param[in] swvdec:      SwVdec handle.
 * @param[in] p_buffer_ip: Pointer to input buffer structure.
 *
 * @retval SWVDEC_STATUS_SUCCESS
 * @retval SWVDEC_STATUS_NULL_POINTER
 * @retval SWVDEC_STATUS_INVALID_STATE
 * @retval SWVDEC_STATUS_FAILURE
 */
SWVDEC_STATUS swvdec_emptythisbuffer(SWVDEC_HANDLE  swvdec,
                                     SWVDEC_BUFFER *p_buffer_ip);

/**
 * @brief Asynchronous API to send an output buffer to SwVdec.
 *
 * The output buffer is filled and returned to client via callback function
 * pfn_fill_buffer_done().
 *
 * @param[in] swvdec:      SwVdec handle.
 * @param[in] p_buffer_op: Pointer to output buffer structure.
 *
 * @retval SWVDEC_STATUS_SUCCESS
 * @retval SWVDEC_STATUS_NULL_POINTER
 * @retval SWVDEC_STATUS_INVALID_STATE
 * @retval SWVDEC_STATUS_FAILURE
 */
SWVDEC_STATUS swvdec_fillthisbuffer(SWVDEC_HANDLE  swvdec,
                                    SWVDEC_BUFFER *p_buffer_op);

/**
 * @brief Asynchronous API to flush input & output buffers.
 *
 * Output and/or input buffers with SwVdec are returned to client via their
 * respective callback functions. Completion of the flush operation is notified
 * to client via SWVDEC_EVENT_FLUSH_[OP|ALL]_DONE using callback function
 * pfn_swvdec_event_notification().
 *
 * @param[in] swvdec:     SwVdec handle.
 * @param[in] flush_type: Flush type.
 *
 * @retval SWVDEC_STATUS_SUCCESS
 * @retval SWVDEC_STATUS_NULL_POINTER
 * @retval SWVDEC_STATUS_INVALID_STATE
 * @retval SWVDEC_STATUS_INVALID_PARAMETERS
 */
SWVDEC_STATUS swvdec_flush(SWVDEC_HANDLE swvdec, SWVDEC_FLUSH_TYPE flush_type);

/**
 * @ breif map ION buffer address to DSP address space for VC1
 *
 * @param p_swvdec_buffer
 *
 * @return SWVDEC_STATUS
 */
SWVDEC_STATUS swvdec_map(SWVDEC_HANDLE swvdec, SWVDEC_BUFFER *p_swvdec_buffer);


/**
 * @ breif unmap ION buffer from DSP address space for VC1
 *  which was prevosuly maped using  map_imp
 *
 * @param p_swvdec_buffer
 *
 * @return SWVDEC_STATUS
 */
SWVDEC_STATUS swvdec_unmap(SWVDEC_HANDLE swvdec, SWVDEC_BUFFER *p_swvdec_buffer);


/**
 * @ breif register ION buffer with FastRPC module
 *
 * @param p_swvdec_buffer
 *
 * @return SWVDEC_STATUS
 */
SWVDEC_STATUS swvdec_register_buffer(SWVDEC_HANDLE swvdec,SWVDEC_BUFFER *p_swvdec_buffer);


/**
 * @ breif unregister ION buffer with FastRPC module
 *
 * @param p_swvdec_buffer
 *
 * @return SWVDEC_STATUS
 */
SWVDEC_STATUS swvdec_unregister_buffer(SWVDEC_HANDLE swvdec,SWVDEC_BUFFER *p_swvdec_buffer);

#ifdef __cplusplus
} // closing brace for: extern "C" {
#endif

#endif // #ifndef _SWVDEC_API_H_
