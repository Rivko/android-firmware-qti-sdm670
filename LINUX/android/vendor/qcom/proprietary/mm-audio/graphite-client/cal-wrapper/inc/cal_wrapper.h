/*
 * cal_wrapper.h
 *
 * This library contains the API to access calibration data related
 * to graphs and sub-graphs.
 *
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef __CAL_WRAPPER_H__
#define __CAL_WRAPPER_H__

/*
 * gcs_cal_wrapper_init - Initialize calibration wrapper for GCS
 *
 * Returns 0 on success and error code otherwise
 *
 * Prerequisites
 *    ACDB Loader should be initialized before calling this function
 */
int32_t gcs_cal_wrapper_init(void);

/*
 * gcs_cal_wrapper_deinit - Deinitialize calibration wrapper for GCS
 *
 * Returns 0 on success and error code otherwise
 *
 * Prerequisites
 *    Calibration wrapper should be initialized before calling this function
 */
int32_t gcs_cal_wrapper_deinit(void);

/*
 * gcs_cal_get_subgraph_list_size - Get size of subgraph list for a graph
 *
 * @u_id:        unique ID for the graph
 * @d_id:        ACDB device ID of the input device used for the graph
 * @cal_size:    size of the subgraph list data, to be returned to GCS
 *
 * Returns 0 on success and error code otherwise
 *
 * Prerequisites
 *    Calibration wrapper should be initialized
 */
int32_t gcs_cal_get_subgraph_list_size(uint32_t u_id, uint32_t d_id,
                                       uint32_t *cal_size);

/*
 * gcs_cal_get_subgraph_list - Get subgraph list for a graph
 *
 * @u_id:        unique ID for the graph
 * @d_id:        ACDB device ID of the input device used for the graph
 * @cal_buff:    pointer to the buffer to hold the subgraph list data
 * @cal_size:    size of the subgraph list data
 *
 * Returns 0 on success and error code otherwise
 *
 * Prerequisites
 *    Calibration wrapper should be initialized
 */
int32_t gcs_cal_get_subgraph_list(uint32_t u_id, uint32_t d_id,
                                  uint8_t *cal_buff, size_t cal_size);

/*
 * gcs_cal_get_subgraph_mod_info_list_size - Get size for module list for
 *                                           a subgraph
 *
 * @u_id:        unique ID for the graph
 * @d_id:        ACDB device ID of the input device used for the graph
 * @sg_id:       subgraph ID for which the module list is queried for
 * @cal_size:    size of the module list data, to be returned to GCS
 *
 * Returns 0 on success and error code otherwise
 *
 * Prerequisites
 *    Calibration wrapper should be initialized
 */
int32_t gcs_cal_get_subgraph_mod_info_list_size(uint32_t u_id, uint32_t d_id,
                                                uint32_t sg_id,
                                                uint32_t *cal_size);

/*
 * gcs_cal_get_subgraph_mod_info_list - Get module list for a subgraph
 *
 * @u_id:        unique ID for the graph
 * @d_id:        ACDB device ID of the input device used for the graph
 * @sg_id:       subgraph ID for which the module list is queried for
 * @cal_buff:    pointer to the buffer to hold the module list data
 * @cal_size:    size of the module list data
 *
 * Returns 0 on success and error code otherwise
 *
 * Prerequisites
 *    Calibration wrapper should be initialized
 */
int32_t gcs_cal_get_subgraph_mod_info_list(uint32_t u_id, uint32_t d_id,
                                           uint32_t sg_id,
                                           uint8_t *cal_buff,
                                           size_t cal_size);

/*
 * gcs_cal_get_subgraph_mod_conn_size - Get size of module connection data for
 *                                      a subgraph
 *
 * @u_id:        unique ID for the graph
 * @d_id:        ACDB device ID of the input device used for the graph
 * @sg_id:       subgraph ID for which the module connections are queried for
 * @cal_size:    size of the module connection data, to be returned to GCS
 *
 * Returns 0 on success and error code otherwise
 *
 * Prerequisites
 *    Calibration wrapper should be initialized
 */
int32_t gcs_cal_get_subgraph_mod_conn_size(uint32_t u_id, uint32_t d_id,
                                           uint32_t sg_id,
                                           uint32_t *cal_size);

/*
 * gcs_cal_get_subgraph_mod_conn - Get module connection data for a subgraph
 *
 * @u_id:        unique ID for the graph
 * @d_id:        ACDB device ID of the input device used for the graph
 * @sg_id:       subgraph ID for which the module connections are queried for
 * @cal_buff:    pointer to the buffer to hold the module connection data
 * @cal_size:    size of the module connection data
 *
 * Returns 0 on success and error code otherwise
 *
 * Prerequisites
 *    Calibration wrapper should be initialized
 */
int32_t gcs_cal_get_subgraph_mod_conn(uint32_t u_id, uint32_t d_id,
                                      uint32_t sg_id,
                                      uint8_t *cal_buff, size_t cal_size);

/*
 * gcs_cal_get_subgraph_conn_size - Get size of connections between subgraphs
 *                                  in a graph
 *
 * @u_id:         unique ID for the graph
 * @d_id:         ACDB device ID of the input device used for the graph
 * @src_sg_id:    source subgraph ID for which connections is queried for
 * @dst_sg_id:    destination subgraph ID for which connections is queried for
 * @cal_size:     size of the connection data, to be returned to GCS
 *
 * Returns 0 on success and error code otherwise
 *
 * Prerequisites
 *    Calibration wrapper should be initialized
 */
int32_t gcs_cal_get_subgraph_conn_size(uint32_t u_id, uint32_t d_id,
                                       uint32_t src_sg_id,
                                       uint32_t dst_sg_id,
                                       uint32_t *cal_size);
/*
 * gcs_cal_get_subgraph_conn - Get connections between subgraphs in a graph
 *
 * @u_id:         unique ID for the graph
 * @d_id:         ACDB device ID of the input device used for the graph
 * @src_sg_id:    source subgraph ID for which connections is queried for
 * @dst_sg_id:    destination subgraph ID for which connections is queried for
 * @cal_buff:     pointer to the buffer to hold the subgraph connection data
 * @cal_size:     size of the connection data
 *
 * Returns 0 on success and error code otherwise
 *
 * Prerequisites
 *    Calibration wrapper should be initialized
 */
int32_t gcs_cal_get_subgraph_conn(uint32_t u_id, uint32_t d_id,
                                  uint32_t src_sg_id, uint32_t dst_sg_id,
                                  uint8_t *cal_buff, size_t cal_size);

/*
 * gcs_cal_get_subgraph_mod_cal_size - Get size of calibration data of modules
 *                                     in a subgraph
 *
 * @u_id:        unique ID for the graph
 * @d_id:        ACDB device ID of the input device used for the graph
 * @sg_id:       subgraph ID for which module calibration is queried for
 * @cal_size:    size of the calibration data, to be returned to GCS
 *
 * Returns 0 on success and error code otherwise
 *
 * Prerequisites
 *    Calibration wrapper should be initialized
 */
int32_t gcs_cal_get_subgraph_mod_cal_size(uint32_t u_id, uint32_t d_id,
                                          uint32_t sg_id,
                                          uint32_t *cal_size);

/*
 * gcs_cal_get_subgraph_mod_cal - Get calibration data of modules
 *                                in a subgraph
 *
 * @u_id:        unique ID for the graph
 * @d_id:        ACDB device ID of the input device used for the graph
 * @sg_id:       subgraph ID for which module calibration is queried for
 * @cal_buff:    pointer to the buffer to hold the subgraph module cal data
 * @cal_size:    size of the calibration data
 *
 * Returns 0 on success and error code otherwise
 *
 * Prerequisites
 *    Calibration wrapper should be initialized
 */
int32_t gcs_cal_get_subgraph_mod_cal(uint32_t u_id, uint32_t d_id,
                                     uint32_t sg_id,
                                     uint8_t *cal_buff, size_t cal_size);

/*
 * gcs_cal_get_boottime_mod_list_size - Get size of list of modules, that are
 *                                      loaded at device boot time
 *
 * @cal_size:    size of the module list data, to be returned to GCS
 *
 * Returns 0 on success and error code otherwise
 *
 * Prerequisites
 *    Calibration wrapper should be initialized
 */
int32_t gcs_cal_get_boottime_mod_list_size(uint32_t *cal_size);

/*
 * gcs_cal_get_boottime_mod_list - Get list of modules, that are
 *                                 loaded at device boot time
 *
 * @cal_buff:    pointer to the buffer to hold the module list data
 * @cal_size:    size of the module list data
 *
 * Returns 0 on success and error code otherwise
 *
 * Prerequisites
 *    Calibration wrapper should be initialized
 */
int32_t gcs_cal_get_boottime_mod_list(uint8_t *cal_buff, size_t cal_size);

/*
 * gcs_cal_get_mod_loading_meta_info_size - Get size of loading meta info, for
 *                                          modules that are loaded at boottime
 *                                          or dynamically at usecase boundary
 *
 * @mod_type:     type of module (boottime, dunamic, static etc.), for which
 *                meta info is queried for
 * @mod_count:    number of modules
 * @mod_list:     list of modules for which meta info is queried for
 * @cal_size:     size of loading meta info data, to be returned to GCS
 *
 * Returns 0 on success and error code otherwise
 *
 * Prerequisites
 *    Calibration wrapper should be initialized
 */
int32_t gcs_cal_get_mod_loading_meta_info_size(uint32_t mod_type,
                                               uint32_t mod_count,
                                               uint8_t *mod_list,
                                               uint32_t *cal_size);

/*
 * gcs_cal_get_mod_loading_meta_info - Get loading meta info, for modules that
 *                                     are loaded at boottime or dynamically
 *                                     at usecase boundary
 *
 * @mod_type:     type of module (boottime, dunamic, static etc.), for which
 *                meta info is queried for
 * @mod_count:    number of modules
 * @mod_list:     list of modules for which meta info is queried for
 * @cal_buff:     pointer to the buffer to hold the meta info data
 * @cal_size:     size of loading meta info data
 *
 * Returns 0 on success and error code otherwise
 *
 * Prerequisites
 *    Calibration wrapper should be initialized
 */
int32_t gcs_cal_get_mod_loading_meta_info(uint32_t mod_type,
                                          uint32_t mod_count,
                                          uint8_t *mod_list,
                                          uint8_t *cal_buff,
                                          size_t cal_size);

#endif /* __CAL_WRAPPER_H__ */
